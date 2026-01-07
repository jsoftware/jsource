/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: LP Foreign                                                       */

#ifdef _ASSERT2
#undef _ASSERT
#define _ASSERT
#endif
#include "j.h"

#if C_AVX2 || EMU_AVX2
// everything we need for one core's execution - shared by all cores
struct __attribute__((aligned(CACHELINESIZE))) mvmctx {
 // returned section

 union {
  D D;
  I I;  // we use integer arithmetic to compare floats, so the  float values must be nonnegative.  Also, we pun integer 0 = fp 0
 } sharedmin;   // minimum (=best) improvement found in any previous column, including in other threads, set to -inf to abort other threads
 I nimpandcols;  // number of improvement,number of cols finished
 I ndotprods;  // total # dotproducts evaluated
 I4 retinfo;  // returned data, col (if gradient) or row (if onecol)
 // shared section
 UI4 nextresv;  // index of next reservation. Advanced by atomic operations.  For one-column, this is a row; for gradient, a column.  Starts out after initial reservations
 US ctxlock;  // used to lock changes
 // all the rest is quickly moved into local storage in each job, we expect before any thread has finished and tries to modify it
 US ressize;  // number of columns/rows that should be taken initially
 // the rest of the first cacheline will be available as soon as the task starts.  Later cachelines will have to be transferred.
 // We put the first-needed data in cacheline 0, making sure we copy it out to avoid false sharing, because the line will be taken away at the end of each reservation
 I4 nbasiswfk;  // |value| is number of rows allocated in Qk; if negative, is 1's-complement of # rows of Qk proper, and Fk is also a row of Qk
 I4 nqkbcols;  // number of columns of Qk, i. e. # cols in basis
 union __attribute__ ((aligned (4))) {
  struct {
   I4 maxweights;  // max # weights in any column
   float mingradimp;  // (gradient only)  0 normally; >0 => finish up gradient calculation in any column that has a larger |gradient|, storing the len/sumsq in cutoffinfo; <0-> accept any gradient as high as 1/(1-MinGradImp) * true max
   UI4 nc;  // #cols in ndxa
   UI *ndx0;   // the column indexes, always a list of ndxs
// end of cacheline 0
   D *Frow;  // the selector-row data
   D (*cutoffstatus)[2];  //  (ncols,2) $ array holding (number of rows processed before cutoff, 0 if row invalid,gradient total for those rows).  If cutoff total is negative, it means that no positive column value has been seen
   C *rvtv;  // list of Rose variable types for the columns
   I (*axv)[2];  // pointer to ax data, (index,length)
   I4 resredwarn;  // row# beyond which we start making SMALLRESV allocations
  } grad;
  struct {
   I4 colx;  // index of the single column
   D *zv; // pointer to output for product mode, 0 for gradient mode
   I (*axv)[2];  // pointer to ax data, (index,length)
// end of cacheline 0
   D *parms;  // parm info
   D *bk;  // pointer to bk.  If 0, this is a column extraction (if zv!=0)
   D *bkbeta;  // beta values for the rows in bk.  The values don't change, but which ones are in bk do
   UI4 zstride;  // length of last axis of z: length of extended z, which includes Fk if present and is padded to batch boundary
   UI4 ressize2;  // reservation size to use after the initial reservation
  } spr;
 } u;  // 4.5 words into cacheline 1
 I4 qktrowstride;  // length of a padded row of Qkt
 I *amv0;  // pointer to am block, and to the selected column's indexes
 D *avv0;  // pointer to av data, and to the selected column's values
 D *qkt0;  // pointer to qkt data
 // end of cacheline 1
 D *bndrowmask;  // for each row in bk, 1 bit/value, indicating bound vbl  Ordered to be right for each bytelane.
};

// the last stages of the pipeline, which are a ring (here 2 long) of info for the column
struct __attribute__((aligned(NPAR*SZI))) pingpong {
 D initsumsq;  // starting value of sumsq: 1 + hidden row, or anything inherited from cutoffinfo
 I initrowct;  // from cutoffinfo: # rows, neg if known not-unbounded
 I an;  // number of weights in the column-1, or flag:  -1 for slack, -2 for column fully evaluated, -3 for column that cut off from starting position
 I colx;  // colx for the column
 D frowsq;  // Frow value for the column, squared
 __attribute__((aligned(NPAR*SZI))) I mvvv[];  // Am values for the column (with space for 1 extra I*), followed by Av values, each aligned
};

#define LGBNDROWBATCH 6
#define BNDROWBATCH ((I)1<<LGBNDROWBATCH)   // the number of bits of bndrowmask that we load at a time.
#define CMPBATCH 64  // frequency with which we compare for cutoff.  MUST NOT BE SMALLER than BNDROWBATCH so that we restart on a bndrowmask bdy
  // We collect the totals for every compare batch.  If we are using low precision for the gradient, we should collect totals often enough that we don't
  // lose precision in each lane
#define RELSIGMAX 7.88e-31  // We set to 0 any accumulation result that was less than this fraction of one of the accumulands

 // gradient: zv=curr col# in resv/flags/pipe/#cols left in resv/max # rows to read before nontemporal hint
#define ZVMAXROWS 0xffff  // mask for # rows/col to allow into cache.  Tested on batch bdy
#define MAXCACHEDROWS 0x4000  // all cached for now
#define ZVCOLCTX 16   // This field is decremented when the pipe is advanced, and the MSB is set if the advance puts a valid value into the first stage
#define ZVCOLCT ((I)0x3ff<<ZVCOLCTX)   // This field is decremented when the pipe is advanced, and the MSB is set if the advance puts a valid value into the first stage
#define ZVCOLCTDECR (-((I)1<<ZVCOLCTX))
#define ZVCOLCTVALID (((I)0x200<<ZVCOLCTX))  // This bit, 1 below ZVPIPE, is set when ZVNDAX has a valid column index
#define ZVPIPE (((I)0x1c00<<ZVCOLCTX))  // Startup pipeline, one bit per stage, 1 at start of column processing if the stage contains valid data
#define ZVPIPE1 (((I)0x1000<<ZVCOLCTX))  // PIPE0 after one advance, meaning 'data was ready before the most recent advance'
#define ZVPIPE0 (((I)0x800<<ZVCOLCTX))  // final stage, holding data for the column to process
#define ZVPIPEm1 (((I)0x400<<ZVCOLCTX))
#define ZVSHARINGMINX 29
#define ZVSHARINGMIN (1LL<<ZVSHARINGMINX)  // set if we should share the gradient if it is a new maximum
// 2 bits free
#define ZVNDAXX 32  // top part of zv is the index into ndxa of the next column to process
#define ZVNDAX (~(UI)0<<ZVNDAXX)  // mask for it
#define ZVNDAXINCR ((I)1<<ZVNDAXX)  // value to add to move to next index

#define BIGRESV 64  // size of large reservation.  As big as possible for column affinity in Qkt; but not so big as to fill the reorder buffer trying to move words to localndx, or to make localndx too big
#define SMALLRESV 8  // small reservation used when we get close to the end.  Large to get column affinity; but small to avoid starving late threads

// the processing loop for one core.  Gradients, over all columns
// ti is the piece# that this thread is taking, used for initial allocation
// TODO
// initial reservation
// remove indirect refs through ctx
// if only 1 weight, add another 0 row, remove test
// pipeline reservation
// no unroll mx
static unsigned char jtmvmsparsegradx(J jt,struct mvmctx *ctx,UI4 ti){
 // transfer everything out of ctx into local names.  We start with the data in the first cacheline and go from there, attempting to read
 // in the order the data will be needed.  We use atomic reads to force the reads to start early, since all but the first cacheline are remote now
#define YCUT(T,x) T x=(T)__atomic_load_n(&ctx->u.grad.x,__ATOMIC_ACQUIRE);
#define YCU(x) YCUT(typeof(ctx->u.grad.x),x)
#define YCT(T,x) T x=(T)__atomic_load_n(&ctx->x,__ATOMIC_ACQUIRE);
#define YC(x) YCT(typeof(ctx->x),x)
 YCU(maxweights)
 float mingradimp; *(I4*)&mingradimp = (I4)__atomic_load_n((I4*)&ctx->u.grad.mingradimp,__ATOMIC_ACQUIRE);  // needed to allocate row/weight area & init zv
 YCT(I,ressize)YCU(nc)  // needed for initial reservation
 YC(nbasiswfk)YCT(I,nqkbcols)   // needed in pipe stage -1, even first time
 YCU(ndx0)   // needed in final reservation stage, even first time
 YCU(cutoffstatus)YCU(axv)YCU(Frow)YCU(rvtv)  // needed in pipe stage -2
 YC(amv0)YC(avv0)  // needed in pipe stage -1, but not first time
 YC(qkt0)YC(bndrowmask)YCT(I4,qktrowstride)YCU(resredwarn)  // needed only after pipeline initialization
#undef YC
#undef YCT
#undef YCU
#undef YCUT
 // perform the operation
 I minvalueweshared=IMAX;  // the smallest value we have shared
 __m256d sqrt2=_mm256_set1_pd(sqrt(2.0));   // sqrt(2) for weighting bound rows
 I ndotprods=0, nimpandcols=0;  // number of dot-products we perform here, and #cols and improvements
 I nqkrows=nbasiswfk^REPSGN(nbasiswfk); // number of rows of Qk NOT including any Fk
 __m256i bndmskshift=_mm256_set_epi64x(0,16,32,48);  // bndmsk is littleendian, but reversed in 16-bit sections.  last section goes to lane 3, i. e. no shift
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);

 I mvvvtoalloc=(maxweights+1+NPAR-1)&-NPAR;  // round (max Axlen+1) up to batch bdy
 I bytesperppb=offsetof(struct pingpong,mvvv[2*mvvvtoalloc]);  // space for header, then Am followed by Av
 A ppb; GATV0(ppb,LIT,2*bytesperppb,1);  // two buffers
 struct pingpong *ppp=voidAV1(ppb);  // ping-pong pointer.  At top of loop, the block just used (=next to fill); after pipe, the block for the column to process
 I pppf=(I)ppp^((I)ppp+bytesperppb);  // (pingpong 0)^(pingpong 1), used to flip the pingpong

 I zv;
 D sharedminslack;  // value to multiply sharedmin by to get a value to beat.  1.0=no change, smaller values cut off columns earlier
 D sharedmin;  // recent minimum recip gradientsq found, updated at start of most columns
 if(mingradimp<0){  // If MinImp given...
  sharedminslack=1.0+mingradimp; sharedminslack*=sharedminslack; zv=ZVSHARINGMIN; *(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE); // create derater for recip colsq
 }else{
  sharedminslack=1.0;  // normal case, do not derate the gradient
  if(unlikely(mingradimp>0)){  // MinGradient given...
   sharedmin=1.0/mingradimp; sharedmin*=sharedmin;  // remember 1/(min gradient to compute)^2, which is what we share
   zv=0;  // init to suppress gradient update
  }else{zv=ZVSHARINGMIN; *(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE); // normal init, share gradients; init gradient
  }
 }
 zv|=ZVMAXROWS&MAXCACHEDROWS;  // install max # rows/col in cache
 // all of zv has been initialized.  COLCT is 0 indicating initialization; NDAX and PIPE are clear

 if(unlikely(nc==0))R 0;  // abort with no action if no columns

 I nextcol=ressize*ti;  // initial reservation - a different block for each job
 PREFETCH2(&ndx0[nextcol]);  // start moving the reservation into L2

 I retinfo;  // the return data for the best value we shared (col).  No need to initialize because we don't report a value if we never shared a gradient

 I4 __attribute__((aligned(NPAR*SZI))) localndx[BIGRESV+2];  // local storage for the indexes.  2 is for overfetch at end-of-pipe. I4 to save space - questionable decision

 I colxm1, anm1, axm1, cutrowm1=nqkrows, colrvtm1; D frowm1=1.0, cutsumsqm1=0.0;  // pipeline values passed from column to column: column#, Ax, cutoffinfo, frow
   // init cutrowm1/cutsumsq1 to look like a completed zero column; frowm1 to a gentlemanly selector

 do{  // loop till the pipeline has drained.  Reservations are seamlessly chained together
  // start of processing one column

  // column-initialization and reservation pipeline.  To run the column we start with fetches of col#, then Frow, cutoffinfo, indexes and weights.  If we are not processing a cached column each of these fetches
  // might miss out to D3$ or beyond.  To break the dependency chain, we pipeline each fetch.  ZVPIPE gives validity information for each stage.  The final stage is the
  // ping-pong buffer
  // The bit is set in ZVPIPE if a value was requested for the pipe stage in the previous column, and we expect it to be ready now
  // Stage -2 is always ready to run, possibly with a benign input.  ZVCOLCTVALID is 1 here if the value produced by -2 is valid
  // A new reservation is taken when the previous reservation is ending.  It is timed by the pipe counter:
  // counter=2: read the current shared column counter
  // counter=1: do the RMW cycle to take the reservation
  // counter=0: read in the reserved NTT column numbers.  During this step the last column of the previous reservation is started in pipe stage -2; the
  //   next time through the pipe will use the new NTT column values
  do{
   // process the pipe stages in reverse order, executing each only if there is valid data for it
   // We use conditionals and initial values to neutralize each stage, since testing the stage would mispredict several times per reservation
   // Pipe stage -1: read Am, Av.
   ppp->frowsq=frowm1*frowm1;
   ppp->colx=colxm1;  // save col# before we flag it
   D cutoffsumsq=sharedmin*sharedminslack*ppp->frowsq;   // Frow^2 * best sumsq / best Frow^2, which is cutoff point for sumsq in new column (Frow^2)/sumsq > bestFrow^2/bestsumsq)
   if(cutrowm1!=0){
    // we have partial results for the column.  Install them into ppp.  If they are enough to avoid processing the column, set the column number invalid (-1) which will prevent
    // us from wasting time fetching the column; we will skip it when it reaches the end of the pipe.  The sharedmin was read at the beginning of the column that just finished
    ppp->initrowct=cutrowm1; ppp->initsumsq=cutsumsqm1;  // init portion to process
    colxm1=ppp->initrowct>=nqkrows?-2:colxm1;   // colx=-2 if column has been processed to the end
      // cutoffsumsq took a long time to compute so we don't want to delay colxm1 till it is ready
   }else{
    ppp->initrowct=0; ppp->initsumsq=!(colrvtm1&0b10)?1.0:2.0;  // no info on column.  Init the col total with 1.0 + (1 more if hidden row exists)
   }
   if(unlikely(ppp->initsumsq>cutoffsumsq))colxm1=-3;    // if initial column position has already cut off, abort.  cutoffsumsq is still in process and we need colx now
   // At this point colxm1 has been set to -2 or -3 if the column doesn't need to be evaluated.  That flag info will be transferred to an
   if(colxm1>=nqkbcols){I avx;  // index of Am/Av block being moved
    // decision variable.  convert the column indexes into pointers into Qkt, and interleave them with the values
    if(unlikely(!BETWEENC(anm1,2,mvvvtoalloc))){
     ASSERTSYS(anm1!=0,"column has no weights");  // column must have at least 1 weight
     ASSERTSYS(anm1<mvvvtoalloc,"column has too many weights");
    }
    ppp->an=anm1-1; I *amv=amv0+axm1; D *avv=avv0+axm1;  // number of sparse atoms in each row-1; pointer to first col#; pointer to first weight
    // Stage the columnss and weights into D1$, in the ping-pong buffer.  This does an extra write & read compared to converting them to interleaved address/value directly,
    // but that takes 12 uops/4 weights, which for a column with a lot of weights will overfill the reorder buffer.  The staging takes 6 uops/4 weights
    NOUNROLL for(avx=0;avx<anm1;avx+=NPAR){  // for each block of input... (this overfetches but that's OK since we map enough for one 32-byte final fetch)
     _mm256_store_si256((__m256i*)&(ppp->mvvv)[avx],_mm256_loadu_si256((__m256i*)&amv[avx]));  // m0-m3
     _mm256_store_si256((__m256i*)&(ppp->mvvv)[avx+mvvvtoalloc],_mm256_loadu_si256((__m256i*)&avv[avx]));  // v0-v3
    }
   }else{
    // Slack variable, or initial/aborted with flag value.  Skip the weighting.
    colxm1=colxm1<0?colxm1:-1; ppp->an=colxm1;  // transfer the 'aborted column' flag to an; if normal Slack variable, set an=-1 as flag
   }
   // end pipe stage -1

   ppp=(struct pingpong*)((I)ppp^pppf);  // we have launched loads to fill the ping buffer (the one used in the previous loop).  Now flip to process pong, which was settling while ping was executing

   I colxm2=localndx[zv>>ZVNDAXX];  // Fetch next column# before we fill localndx for the new reservation.  This may be an overfetch, which we will modify later; or an initial fetch of garbage

   // If the current reservation is ending, start on the next one.  We do this after pipe stage -1 because both blocks may release a slug of offcore reads, and
   // we want to get the ones for pipe stage -1 first so we can start that column
   if((zv&ZVCOLCT)<=ZVCOLCTVALID+(2<<ZVCOLCTX)){  // if reservation is ending...
    if(nextcol<nc){
     // When nextcol exceeds the number of columns, we are finished.  This may happen when we fetch the current pointer, or make a reservation, or finish a reservation.
     // At that point, we stop coming through this block and when the pipe eventually drains we are finished
     if((zv&ZVCOLCT)==ZVCOLCTVALID+(2<<ZVCOLCTX))nextcol=__atomic_load_n(&ctx->nextresv,__ATOMIC_ACQUIRE);  // cycle 1..2, read the current col pointer
     else if((zv&ZVCOLCT)==ZVCOLCTVALID+(1<<ZVCOLCTX)){ressize=nextcol>resredwarn?SMALLRESV:BIGRESV; nextcol=__atomic_fetch_add(&ctx->nextresv,ressize,__ATOMIC_ACQ_REL);}  // cycle 1..1, make reservation
     else{I i;  // must be cycle 1..0 or 0 (initialization)
      // previous reservation will release its last column into -2 this cycle.  Copy the next NTT cols indexes.  This will release BIGRESV/(2*NPAR)*(7) instructions waiting for offcore.  56 inst, OK
      ressize=nextcol+ressize; ressize=ressize>=nc?nc:ressize; ressize-=nextcol; // calc end of our reservation, never longer than the data; convert to size
      __m256i swizzle=_mm256_set_epi32(7,5,3,1,6,4,2,0);  // for each dest lane, the source lane that has the value
      for(i=0;i<ressize;i+=2*NPAR){__m256i x0, x1;  // input locations
       x0=_mm256_loadu_si256((__m256i*)&ndx0[nextcol+i]); if(i+NPAR<ressize)x1=_mm256_loadu_si256((__m256i*)&ndx0[nextcol+i+NPAR]);  // x0=0o 1o 2o 3o x1=4o 5o 6o 7o  (o=zeros)
       __m256i shortx=_mm256_permutevar8x32_epi32(_mm256_blend_epi32(x0,_mm256_slli_epi64(x1,32),0b10101010),swizzle);  // ->o4 o5 o6 o7->04 15 26 37->01 23 45 67
       _mm256_store_si256((__m256i*)&localndx[i],shortx);  // copy the indexes - overfetch OK
      }
      // Set zv for the new batch of columns, plus the last one of the old pipe if any
      if(!(zv&ZVCOLCTVALID)){
       // cycle 0 - happens only at initialization of the first reservation.  colxm2 fetched garbage, and we have to wait for the first column value to be ready
       zv=zv+(ZVCOLCTVALID+ZVCOLCTDECR+(ressize<<ZVCOLCTX));  // starting col index=0 (in localndx), and arrange for 'valid' to be set for the # indexes we have.
       colxm2=localndx[0];  // start with the first column of the first reservation.  This waits for the reads to complete first time
      }else{
       // cycle 1..0 - normal chained reservation.  Leave pipe and colxm2 for the previous reservation, and set pointer/counters for the new.  This puns ZVCOLCTVALID, which is set for the new resv but used for the old; both 1, fortunately
       zv=(zv&~(ZVNDAX|ZVCOLCT))+(-((I)1<<ZVNDAXX)+ZVCOLCTVALID+(ressize<<ZVCOLCTX));  // starting col index=-1 (in localndx), and arrange for 'valid' to be set for the # indexes we have+1
      }
      nextcol+=ressize;  // advance nextcol to end+1 of reservation.  If this is past the end of input it will put us into termination
     }
    }else if(!(zv&ZVCOLCTVALID)){
     // pipe is draining to empty; we overfetched, meaning further pipe entries are invalid: perform a harmless simulation of stage -2
     cutrowm1=nqkrows; cutsumsqm1=0.0;  // this suffices to initialize stage -1 to bypass most processing.  frowm1 has a previous valid value.  colxm1 is garbage
     goto endpipem2;  // skip the fetches for stage -2 
    }
     // if we didn't take a reservation nextcol will stay set high and the pipeline will empty
   }

   // Pipe stage -2: read colx, Ax, Frow, RVT.
   colxm1=colxm2; frowm1=Frow[colxm2]; cutrowm1=cutoffstatus[colxm2][0]; cutsumsqm1=cutoffstatus[colxm2][1];  // other needed info
   colrvtm1=(rvtv[colxm2>>(LGBB-1)]>>((colxm2&((1LL<<(LGBB-1))-1))<<1))&((1LL<<2)-1);  // bound/enforcing flag for column
   colxm2=colxm2<nqkbcols?nqkbcols:colxm2; anm1=axv[colxm2][1]; axm1=axv[colxm2][0];  // if slack column, read from harmless decision vbl 0
endpipem2: ;  // come here in runout where pipe input is invalid
   // end pipe stage -2

   // advance the pipe, possibly with validity set for the incoming stage -2 data
   zv=(zv&~ZVPIPE)|((zv<<1)&ZVPIPE);  // advance the pipe, using current value of ndx validity

   // Pipe input stage: advance column index
   zv+=ZVNDAXINCR+ZVCOLCTDECR;  // advance colx index, decr valid col count, whose MSB is ZVCOLCTVALID, input to the pipe
   // end pipe input stage

  }while(!(zv&ZVPIPE1));  // loop till there is a full column's info to process
  // the pipe status after the advance indicates validity AFTER the next execution of a column; that is, PIPE0 is set if we
  // have started the loads in -1 that will advance to 0 during the next execution.  PIPE1 is set if ppp is ready to execute right now, i. e. PIPE0 was set before the pipe advanced.  We wait until we can execute right now.

  // end of initialization pipeline. start the next column, pointed to by ppp

  I an=ppp->an;  // # weights-1 (-1 if Slack col, -2 if column finished already, -3 if aborted already)
  I rowx=ppp->initrowct;  // the row number of Qk we are working on.  We stop when rowx=rown.  Always on a 32-row boundary
  D accumsumsq=ppp->initsumsq;  // total column norm so far
  rowx=rowx>=nqkrows?nqkrows:rowx;  // prevent rows from incrementing too many times
  ndotprods-=rowx*(an+1-REPSGN(an));  // treat (0,start] as a gap.  Count #fetches: an+1 if an>=0, an+2 if an=-1, immaterial if an<-1 (will be undone at end)

  // We calculated a cutoff point in pipe stage -1, using it to decide whether to load the Am/Av values.  sharedmin has since been updated, and might give a stricter cutoff.
  D cutoffsumsq=sharedmin*sharedminslack*ppp->frowsq;   // Frow^2 * best sumsq / best Frow^2, which is cutoff point for sumsq in new column (Frow^2)/sumsq > bestFrow^2/bestsumsq)
  // We do not use the new cutoff right away to set colx, because it has latency and the chance it has changed enough to cut off this column at the start is quite small

  D **mv0, *vv0;  // pointer to first column pointer, pointer to first weight
  // convert the column#s to Qkt row addresses, in place
  if(an>=0){I avx;
   if(likely(zv&ZVSHARINGMIN))*(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // fetch sharedmin, which we will use AFTER we process this column.  Do it early
   mv0=(D**)&ppp->mvvv[0]; vv0=(D*)&ppp->mvvv[mvvvtoalloc];  // the column will use addrs/weights out of ppp.  Get the starting addresses
   __m256i qkt0_4=_mm256_set1_epi64x((I)qkt0), qktrowstride_4=_mm256_set1_epi32(qktrowstride<<LGSZD);
   NOUNROLL for(avx=0;avx<=an;avx+=NPAR){  // for each block of input... (this overfetches but that's OK since we map enough for one 32-byte final fetch)
    _mm256_store_si256((__m256i*)&mv0[avx],_mm256_add_epi64(qkt0_4,_mm256_mul_epu32(_mm256_load_si256((__m256i*)&mv0[avx]),qktrowstride_4)));  // row#*row size + base of Qkt, for each row.  There may be garbage at the end
   }
   if(unlikely(an==0)){mv0[1]=mv0[0]; vv0[1]=0.0; an=1;}  // if only 1 weight (should not occur), add a weight of 0 to save testing in the loop
  }else if(an==-1){  // Slack variable.  Set flag length and repurpose nextmv to point to the sole row of Qkt.  Also refetch sharedmin
    if(likely(zv&ZVSHARINGMIN))*(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE); vv0=&qkt0[qktrowstride*ppp->colx];  // leave an=-1
  }else{if(an==-2)goto usefullrowtotal; goto abortcol;  // immediate termination/abort.  DO NOT fetch sharedmin, as we will be checking it presently.  rowx, accumsumsq needed
  }
if((rowx&(BNDROWBATCH/1-1))!=0)SEGFAULT;  // scaf  if just part of a col, it must leave us on a bndrowmask bdy

  __m256d accnorm4=_mm256_setzero_pd();  // accumulator for sumsq, in short bursts

  // Beat me, Daddy, 32 to the bar

  __m256d bndmsk;  // bitstream for each lane, giving (bound) info for the row.  bigendian!!
  while(rowx<nqkrows){  // for each row in the column, until we cut off

   // if this starts a boundmask block, fetch the mask for it
   if((rowx&(BNDROWBATCH/1-1))==0){
    // move the 64 bits to  the correct position in each lane
    bndmsk=_mm256_castsi256_pd(_mm256_sllv_epi64(_mm256_set1_epi64x(*(I*)&bndrowmask[rowx>>(LGBNDROWBATCH-0)]),bndmskshift));
   }

   // calculate several blocks of the dot-product
   // idea: reorder columns to use lines cached by previous columns
   // Main accumulation loop.  We do not pipeline mv because it looks like we are limited by instruction issue
   __m256d avweight,acc0,acc1,acc2,acc3,acc4,acc5,acc6,acc7;  // fma latency is 4 cycles; 2 launches/cycle; need 8 accumulators
   #define GRLD(accno,type) acc##accno=_mm256_castsi256_pd(_mm256_##type##_si256((__m256i*)&vv0[rowx+NPAR*accno]));
   #define GRMUL(accno,type) acc##accno=_mm256_mul_pd(avweight,_mm256_castsi256_pd(_mm256_##type##_si256((__m256i*)&mv[rowx+NPAR*accno])));
   #define GRFMA(accno,type) acc##accno=_mm256_fmadd_pd(avweight,_mm256_castsi256_pd(_mm256_##type##_si256((__m256i*)&mv[rowx+NPAR*accno])),acc##accno);
   if(rowx<=(zv&ZVMAXROWS)){
    // for the first rows of each column of Qk, allow the values to come into the caches, where they might be reused
    #define GRLMUL(accno) GRMUL(accno,load)
    #define GRLFMA(accno) GRFMA(accno,load)
    #define GRLLD(accno) GRLD(accno,load)
    if(an>=0){D *mv; UI mx=an;  // decision vbl.  Count mx down from #weights-1
     mv=mv0[mx]; avweight=_mm256_set1_pd(vv0[mx]); GRLMUL(0) GRLMUL(1) GRLMUL(2) GRLMUL(3) GRLMUL(4) GRLMUL(5) GRLMUL(6) GRLMUL(7)   // initialize accumulators with first product
     // How much to unroll this loop is a hard question.  There are 5 non-FMA instructions per loop, of which 1 is eliminable MOVs.  Estimating the average column to have 5 weights and cutoff in 256 rows,
     // there will be 40 FMAs, 40 LOAD, 25 overhead per batch, plus the batch processing of 40 inst.  These would run in 38 cycles and would be limited by the instruction launch.
     // By unrolling the loop we can cut the non-FMA to 2.5 inst/loop, total 13/batch, at the cost of 1 misbranch (~60 inst).
     // BUT: data will usually be coming from D2$ and will not be available at max rate.  If the actual rate is 200/320 of max rate, the 40 LOAD will limit to 32 cycles/batch, which is not limiting for the workload described.
     // However, if bound-row processing can be eliminated (by storing a version premultiplied by sqrt(2)), the batch processing will fall to 16 inst, which will launch faster than the LOADs can run.
     // Summary: it's a close question (in 2023) and we go for simplicity, because that will be best if bound-row is removed.
     do{mv=mv0[mx-1]; avweight=_mm256_set1_pd(vv0[mx-1]); GRLFMA(0) GRLFMA(1) GRLFMA(2) GRLFMA(3) GRLFMA(4) GRLFMA(5) GRLFMA(6) GRLFMA(7)}while(--mx);
    }else{  // slack vbl
     GRLLD(0) GRLLD(1) GRLLD(2) GRLLD(3) GRLLD(4) GRLLD(5) GRLLD(6) GRLLD(7)  // load column directly
    }
   }else{
    // later rows in the column are unlikely to be reused (the column will have cut off); don't let them evict more valuable early rows
    #define GRSMUL(accno) GRMUL(accno,stream_load)
    #define GRSFMA(accno) GRFMA(accno,stream_load)
    #define GRSLD(accno) GRLD(accno,stream_load)
    if(an>=0){D *mv; UI mx=an;  // decision vbl
     mv=mv0[mx]; avweight=_mm256_set1_pd(vv0[mx]); GRSMUL(0) GRSMUL(1) GRSMUL(2) GRSMUL(3) GRSMUL(4) GRSMUL(5) GRSMUL(6) GRSMUL(7)   // initialize accumulators with first product
     do{mv=mv0[mx-1]; avweight=_mm256_set1_pd(vv0[mx-1]); GRSFMA(0) GRSFMA(1) GRSFMA(2) GRSFMA(3) GRSFMA(4) GRSFMA(5) GRSFMA(6) GRSFMA(7)}while(--mx);
    }else{  // slack vbl
     GRSLD(0) GRSLD(1) GRSLD(2) GRSLD(3) GRSLD(4) GRSLD(5) GRSLD(6) GRSLD(7)  // load column directly
    }
   }

   rowx+=8*NPAR;  // advance input row number
   if(unlikely(rowx>nqkrows)){   // if we have to discard part of the last batch... (Note that we bypass this if the last batch is precisely filled)
    // if we make it to the end without cutting off, we have to zero the last value, which is Fk/ck rather than a real column value, and all later values which may be fetches off the end
    __m256d endmask=_mm256_loadu_pd((double*)(validitymask+NPAR-(nqkrows&(NPAR-1))));  // vvv00-> mask 0000, vvv01->1000, vvv10->1100, vvv11->1110
    #define GRCLR(accno,accno1) case accno: acc##accno=_mm256_and_pd(acc##accno,endmask); acc##accno1=_mm256_setzero_pd();   // falls through to next case
    switch((nqkrows>>LGNPAR)&(8-1)){GRCLR(0,1) GRCLR(1,2) GRCLR(2,3) GRCLR(3,4) GRCLR(4,5) GRCLR(5,6) GRCLR(6,7) case 7: acc7=_mm256_and_pd(acc7,endmask);}
   }

   // combine the blocks.   if row is bound, multiply its value by sqrt(2).  Then accumulate value^2 into accnorm4.  This will complete while the next burst is being calculated
   // idea: store bound rows with Qk values multiplied by sqrt(2).  Must undo for onecol, and must take into account during pivot
   // better idea: have single-precision version of Qkt, used only for gradients, with bound rows already multiplied by sqrt(2)
   #define GRSQ(accno) acc##accno=_mm256_blendv_pd(acc##accno,_mm256_mul_pd(acc##accno,sqrt2),bndmsk); acc##accno=_mm256_mul_pd(acc##accno,acc##accno); /* sq, or 2*sq */ \
   bndmsk=_mm256_castsi256_pd(_mm256_slli_epi64(_mm256_castpd_si256(bndmsk),1));  // move bound-row mask to the next group of rows.
   #define GRSQACC(accno,addend) acc##accno=_mm256_blendv_pd(acc##accno,_mm256_mul_pd(acc##accno,sqrt2),bndmsk); acc##accno=_mm256_fmadd_pd(acc##accno,acc##accno,addend); /* sq, or 2*sq */ \
   bndmsk=_mm256_castsi256_pd(_mm256_slli_epi64(_mm256_castpd_si256(bndmsk),1));  // move bound-row mask to the next group of rows.
   GRSQACC(0,accnorm4) GRSQ(1) GRSQ(2) GRSQ(3) GRSQACC(4,acc0) GRSQACC(5,acc1) GRSQACC(6,acc2) GRSQACC(7,acc3)  //  accnorm4+all accs into 4 acc4s

   acc5=_mm256_add_pd(acc4,acc5); acc7=_mm256_add_pd(acc6,acc7); accnorm4=_mm256_add_pd(acc5,acc7);  // accnorm4+all accs into 1 acc4.

   // if gradient total cuts off, abort the column.  We have to collect the total across all lanes
   // this test is long enough that we don't want to do it on every loop.  The best frequency is
   // sqrt(2 * (expected # loops) * (cost of test) / (cost of loop body))
   // mitigating factors are (1) computation is not lost if the column isn't modified by the pivot
   // (2) if the frequency is less than every 16 tests the branch will mispredict
   // Currently the body is 5 cycles/nonzero in A0 + 36, test is 8; expected #loops is, say, 200/32=7.  Best freq is
   // sqrt(2 * 7 * 8 / 61) = a little over 1.  We use 2.
   if((rowx&(CMPBATCH-1))==0){  // when we have moved to the START of the next compare batch, so that if we cut off we will be on a bndrow mask bdy
    // This block has no chained dependency - it just checks for cutoff
    accnorm4=_mm256_add_pd(_mm256_permute_pd(accnorm4,0b0101),accnorm4); accnorm4=_mm256_add_pd(_mm256_permute4x64_pd(accnorm4,0b01001110),accnorm4);   // combine into one
    accumsumsq+=_mm256_cvtsd_f64(accnorm4); accnorm4=_mm256_setzero_pd();  // transfer total to accumsumsq and reset
    if(unlikely(accumsumsq>cutoffsumsq)){  // cutoff if Frow^2/sumsq<(best Frow^2/sumsq) => sumsq>(best sumsq)*(Frow/best Frow)^2  we save min of (best sumsq)/(best Frow^2) which is max of FOM
     goto abortcol;   // rowx and accumsumsq must be set
    }
   }
  }
  // done with loop down one column.  Here the column ran to completion

  // End-of-column processing.  Collect stats and update parms for the next column
  // we are sharing the gradient, looking for the smallest (we currently have sumsq of columns)

  // calculate 1/gradient^2 for the column, which is positive and we want to minimize (to maximize gradient).  It is sumsq/Frow^2
  accnorm4=_mm256_add_pd(_mm256_permute_pd(accnorm4,0b0101),accnorm4); accnorm4=_mm256_add_pd(_mm256_permute4x64_pd(accnorm4,0b01001110),accnorm4);   // total sumsq
  accumsumsq+=_mm256_cvtsd_f64(accnorm4);  // final total of column normsq
usefullrowtotal:;  // come here when we started knowing the row total, in accumsumsq
  D recipgradsq=accumsumsq/ppp->frowsq;  // norm^2/Frow^2 = 1/gradient^2
  I valuetoshareminof=*(I*)&recipgradsq;  // share the recip gradient, looking for mimimum.  We have the gradient that was completed for this column
       // because atomics run int-only, this is the bit-pattern that we will minimize as an I
  // if this value is a new minimum, share it with the other cores (might not be if last batch went overlimit but was not tested for cutoff)
  // If the new value is equal to the old, there is no need to share it
  if(unlikely(valuetoshareminof<*(I*)&sharedmin) && (zv&ZVSHARINGMIN)){
   // the new value is lower than the lowest we know about.  Share it
   I incumbentimpi=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // load incumbent best improvement
   while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
    if(valuetoshareminof>=incumbentimpi)break;  // if not new global best, don't update global.  In this case we don't need to remember the value of this column, since it has been beaten
    if(__atomic_compare_exchange_n(&ctx->sharedmin.I,&incumbentimpi,valuetoshareminof,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){
     minvalueweshared=valuetoshareminof;  // wait till now to remember best-in-thread.  There is no need to remember a value that we didn't share
     retinfo=ppp->colx;  // save the col where we found the shared value
     incumbentimpi=valuetoshareminof;  // our value has been written
     break;
    }  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
   }
   *(I*)&sharedmin=incumbentimpi;  // since we went to the trouble of reading current sharedmin, remember what it is
  }

  // we have finished processing the column, and reporting its gradient if it was an improvement.  Incr stats and save column cutoff info
  I impcolincr;  // value we will add to the col-stats field
  //  ***** jump to here on aborted column *****   rowx must point to the batch after the last one we processed and must be on a BNDROW bdy
  if(1)impcolincr=0x100000001; else abortcol: impcolincr=1; // jump here if column aborted early, possibly on insufficient gain.  This is the normal path if abort, don't incr # completed columns
  ndotprods+=rowx*(an+1-REPSGN(an)); nimpandcols+=impcolincr; // accumulate # products performed, including the one we aborted out of; and 1 column, 0/1 improvements
  // save the total we accumulated.
if(rowx<nqkrows && (rowx&(BNDROWBATCH/1-1))!=0)SEGFAULT;  // scaf
  rowx=rowx>nqkrows?nqkrows:rowx;  // If rowx>nrows, back it up so accounting of reused products is accurate
  D Drowx=rowx;
  __atomic_store((I*)&cutoffstatus[ppp->colx][0],(I*)&Drowx,__ATOMIC_RELEASE);  // only integers can be atomic args
  __atomic_store((I*)&cutoffstatus[ppp->colx][1],(I*)&accumsumsq,__ATOMIC_RELEASE);
 }while(zv&(ZVPIPE0|ZVPIPEm1));  // end of column loop.  Stop if the pipe is empty after this column

 // No more columns to do.
 // return result to the ctx, if we have the winning value.  Simple writes to ctx do not require RELEASE order because the end-of-job code
 // and the threads sync through ctx->internal.nf; but they should be atomic in case we run on a small-word machine

 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL); __atomic_fetch_add(&ctx->nimpandcols,nimpandcols,__ATOMIC_ACQ_REL); // accumulate stats for the work done here: dot-products, improvements, columns
 // if the value we reported out might be the best, store its completion info out if it is actually the best.
 // For the nonce, we hope the thread ordering gives enough randomization for tied zero SPRs
 if(minvalueweshared<=*(I*)&sharedmin){  // don't bother if we know another thread is better
  I incumbentimpi=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // update incumbent best value
  if(minvalueweshared==incumbentimpi){  // check outside of lock for perf
   WRITELOCK(ctx->ctxlock)
   if(minvalueweshared==ctx->sharedmin.I){ctx->retinfo=retinfo;} // if we still have the best gain, report where we found it
   WRITEUNLOCK(ctx->ctxlock)
  }
 }
 R 0;
}


 // onecol+SPR
#undef ZVNEGATECOLX
#undef ZVNEGATECOL
#define ZVNEGATECOLX 0  // set if the result of the col-value calculation must be negated (for Enforcing vars)
#define ZVNEGATECOL (1LL<<ZVNEGATECOLX)  // set if the current column was an improvement
#undef ZVPOSCVFOUNDX
#undef ZVPOSCVFOUND
#define ZVSPR0X 1  // set if we have hit SPR of 0 and do not need to compute them any more
#define ZVSPR0 (1LL<<ZVSPR0X)
#define ZVPOSCVFOUNDX 2  // set if we are looking for improvement and want to abort when SPR=0
#define ZVPOSCVFOUND (1LL<<ZVPOSCVFOUNDX)
#define remflgs(zv) ((D*)((I)(zv)&~0x7))

#define ALMOSTINF 1e300  // A very large value, used when we have seen a positive col value but have no SPR to report

// we estimate processing 4 rows of QkA0 in 5 qp multiply-adds, each taking about 30 fp instructions - total 75 clocks at 2 launches per clock - plus time to write and do SPR, another
// ~10 clocks.  That's maybe 16ns.  A RFO takes 15-30 ns.  We will take the next reservation with 12 rows left in the current one
#define ROWSPERRESV 12   // number of rows we can process during one atomic_fetch_add

// the processing loop for one core, performing column fetch+SPR.  We take a slice of the rows, repeatedly
// ti is the job#, not used except to detect error
static unsigned char jtmvmsparsesprx(J jt,struct mvmctx *ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YCUT(T,x) T x=(T)__atomic_load_n(&ctx->u.spr.x,__ATOMIC_ACQUIRE);
#define YCU(x) YCUT(typeof(ctx->u.spr.x),x)
#define YCT(T,x) T x=(T)__atomic_load_n(&ctx->x,__ATOMIC_ACQUIRE);
#define YC(x) YCT(typeof(ctx->x),x)
 YCUT(I,colx)YCU(zv)YCT(I,nqkbcols)YCU(axv)YC(amv0)YC(avv0)YC(qkt0)YCT(I4,qktrowstride)YC(nbasiswfk)YC(ressize)YCUT(I,zstride)YCU(parms)YCU(bk)
 YC(bndrowmask)YCU(bkbeta)YCU(ressize2)
#undef YC
#undef YCT
#undef YCU
#undef YCUT
 // perform the operation
 I ndotprods=0;  // number of dot-products we perform here
 I frowbatchx;  // rowx of batch containing Fk, if any
 I qkstride=qktrowstride*nqkbcols;  // distance between planes of Qk
 __m256i bndmskshift=_mm256_set_epi64x(0,16,32,48);  // bndmsk is littleendian, but reversed in 16-bit sections.  last section goes to lane 3, i. e. no shift
 __m256d store0thresh=_mm256_set1_pd(parms[3]);  // In one-column mode, this holds the Store0Threshold: column values less than this are set to 0 when written out
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);

 // Initial zv has the NEGATECOL flag set if needed, others clear

 // Bring in the row address/weights for this column.  Slow if in D3$.  If we take a reservation next, that won't matter; start early in case 1 core
 D **mv0, *vv0;   // place to hold the addresses/weights of the columns being combined
 I an;  // number of weights in this dot-product
 if(colx>=nqkbcols){I avx;  // index of Am/Av block being moved
  // decision variable.  convert the column indexes into pointers into Qkt, and interleave them with the values
  an=axv[colx][1]; I wtofst=axv[colx][0];   // number of weights in column, offset to them
  I ntoalloc=(an+1+NPAR-1)&-NPAR;  // block that can hold all the pointers/weights
  A wts; GATV0(wts,INT,2*ntoalloc,1)  mv0=voidAV1(wts); vv0=(D*)(mv0+ntoalloc);   // place to hold the addresses/weights of the columns being combined
  I *amv=amv0+wtofst; D *avv=avv0+wtofst;  // number of sparse atoms in each row; pointer to first col#; pointer to first weight
  __m256i qkt0_4=_mm256_set1_epi64x((I)qkt0), qkrowstride_4=_mm256_set1_epi32(qktrowstride<<LGSZD);
  for(avx=0;avx<an;avx+=NPAR){  // for each block of input... (this overfetches but that's OK since we map enough for one 32-byte final fetch)
   __m256i mv4=_mm256_loadu_si256((__m256i*)&amv[avx]);   // read next group of 4 row#s
   __m256d av4=_mm256_loadu_pd(&avv[avx]);   // read next group of 4 values
   mv4=_mm256_add_epi64(qkt0_4,_mm256_mul_epu32(mv4,qkrowstride_4));  // row#*row size + base of Qkt, for each row.  There may be garbage at the end
   // We have 4 row-pointers and 4 values.  Write them out
   _mm256_storeu_si256((__m256i*)&mv0[avx],mv4); _mm256_storeu_pd((double*)&vv0[avx],av4);
  }
 }else{an=-1; vv0=(D*)&qkt0[qktrowstride*colx];}  // Slack variable.  Skip the weighting, leave an neg as flag.  Repurpose vv0 to point to col

 //   parms is #cols,maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,[MinSPR],[PriRow]
 __m256d col0thresh=_mm256_set1_pd(parms[2]);  //  minimum value considered valid for a column value (smaller are considered 0)
 __m256d bkovershoot=_mm256_set1_pd(parms[9]);  //  maximum amount a pivot is allowed to make bk overshoot
 __m256d bk0thresh=_mm256_set1_pd(parms[8]);  // smallest bk value considered nonzero
 I prirow=(I)parms[11];  //  priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so.  Set to LOW_VALUE if no priority row
 I nqkrows=ABS(nbasiswfk);  // number of rows of qk including any Fk
 I nhasfk=REPSGN(nbasiswfk); // -(Fk is an extra row)
 frowbatchx=(nqkrows-1)&-NPAR; frowbatchx|=~nhasfk;  // batch# containing last row (might be by itself); if there is no fk, batch is ~0, i. e. never
 __m256d sharedspr;  // the minimum across lanes shared, to reduce minimum sightings
 I limitrowx=IMAX;  // for SPR, the row of the current lowest SPR.   init row found to high-value, so that we don't share a value unless we actually hit it
 __m256d bndmsk;  // bitstream for each lane, giving (bound) info for the row.  bigendian!!
 D nzerobatches;  // number of batches with 0 SPR we have encountered


 I rowx=ressize*ti;  // the row number we are working on.  We stop when rowx=rown.  Always on an NPAR-row boundary
 I rown=rowx+ressize;  // index of last+1 row in current (or only) section of the column.  If there is a remnant, the value is backed up by 4.  If 0, we are processing the remnant
 I newrowx=rown;  // if we don't get a reservation, for termination purposes use the end+1 of the current one.  The only time we don't resv is when this is past the end
 I needresvx=(rown-ROWSPERRESV)&-NPAR; needresvx=needresvx<rowx?rowx:needresvx;   // point at which we need new resv.  Might be off the end, if we never need one
 rown=rown>nqkrows?nqkrows:rown; rown-=rown&(NPAR-1)?NPAR:0;  // clamp to end of column, and back up end if there is a remnant

 __m256d endmask=_mm256_setone_pd(); // mask for validity of next 4 words: used to control fetch from column and store into result row
 // The init SPR is inf for normal columns; but for bound column must init to beta, the SPR of the implied row
 // This was set in the initial value in the ctx, but it may have been reduced by other threads before we get to look at it
 D initspr=ctx->sharedmin.D;
 // If another thread has finished with 0 SPR, or has aborted, sharedmin has changed meaning to be -1/max column value found, or IMIN (=-0) for abort.
 // In that case we mustn't interpret the value as a positive SPR.  We abort if IMIN, because that might save a lot of work when another thread has aborted.
 // For 0 SPR, we indicate that we are looking for 0 SPR, but we don't try to start at the same value as the other thread, because owing to numerical error
 // we might end up thinking we had the winning 0 SPR with no idea what row it came from.  So we just set the max-col-value to a harmless invalid 0, which will
 // never be the smallest 0-SPR
 if(unlikely(*(I*)&initspr<0)){if(*(I*)&initspr==IMIN)R 0; initspr=0.; zv=(D*)((I)zv|ZVSPR0);}  // if remote abort, abort w/o reply; if 0-SPR, set up for that
 sharedspr=_mm256_set1_pd(initspr);

 // create the column NPAR values at a time
 // loop for each row, in groups of NPAR
 while(1){
  // Establish what batch of NPAR lines we should work on.  This block handles end-of-column/end-of-section
  if(unlikely(rowx>=rown)){   // if there are not a full 4 values to process
   // not 4 values.  If there is no remnant, we are through with the reservation for this column
   rown=rown==0?rowx:rown;  // rown=0 means we are processing the remnant; signal that it is finished
   if(rowx==rown){  // no unprocessed remnant?
    // Use the previously-read row reservation
    if(newrowx>=nqkrows)break;  // if there is no new reservation, exit.  This is the loop exit
    // We are going to take a new reservation.  Deduct the size of the gap from the # products to date
    ndotprods+=rowx-newrowx;  // we remove (new rowx-old rowx), i. e. the amount skipped, from the count of rows processed
    rowx=newrowx;   // start new reservation
    rown=rowx+ressize2; newrowx=rown; needresvx=(rown-ROWSPERRESV)&-NPAR; needresvx=needresvx<rowx?rowx:needresvx;  // set up for reading next reservation
    rown=rown>nqkrows?nqkrows:rown; rown-=rown&(NPAR-1)?NPAR:0;  // clamp to end of column, and back up end if there is a remnant
   }
   if(rowx>rown){  // if there is a remnant (which could be the first batch of a new reservation), process it and make sure we finish next time
    // there is a remnant to process.  Get the endmask for it
    endmask=_mm256_loadu_pd((double*)(validitymask+(rowx-rown)));   // (curr-(end-4)) = 4-(end-curr) = 4-#values left is the correct mask, with (#values) ~0
    rown=0;  // set flag value to indicate we are processing a remnant and endmask is set
   }
  }

  // When we get close enough to the end of the reservation, establish the next one.  This takes a RFO cycle
  if(unlikely(rowx==needresvx))newrowx=__atomic_fetch_add(&ctx->nextresv,ressize2,__ATOMIC_ACQ_REL);   // reserve the next block

  __m256d dotproducth,dotproductl;  // where we build the column value

  // if this starts a bound/enforcing block, fetch the mask for it
  if((rowx&(BNDROWBATCH/1-1))==0){
   // move the 64 bits to  the correct position in each lane
   bndmsk=_mm256_castsi256_pd(_mm256_sllv_epi64(_mm256_set1_epi64x(*(I*)&bndrowmask[rowx>>(LGBNDROWBATCH-0)]),bndmskshift));
  }

  // Do the extended-precision dot-product
  __m256d accmaxabs;  //  max magnitude of the inputs to the accumulation
  if(an>=0){  // decision variable, which requires weights
   // It's not unusual for all 4 values fetched from Qk to be 0. 
   // For the first value, we can simplify the product with no accumulate
   I mx;   // row index (of the nonzero components of this A0 col)
   NOUNROLL for(mx=0;mx<an;++mx){  // for each element of the dot-product
    __m256d tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
    D *mv=mv0[mx];  // advance the pipeline of row addresses in Qkt.  We don't unroll because we have a great many dependent instructions to release
    // get column number to fetch; fetch 4 rows
    dotproducth=_mm256_load_pd(&mv[rowx]);  // fetch 4 values.  Extras should be 0.  We use load, not loadu, because these fetches MUST be aligned
    if(_mm256_testz_si256(_mm256_castpd_si256(dotproducth),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
    // there's a nonzero, calculate it
    tl=_mm256_load_pd(&mv[rowx+qkstride]);  // fetch 4 values, low part
    vval=_mm256_set1_pd(vv0[mx]);  // load weight for the column
    // take the two products and canonicalize
    TWOPROD(dotproducth,vval,th2,tl2)  // high qk * col
    accmaxabs=_mm256_andnot_pd(sgnbit,dotproducth);   // init upper-significance holder
    tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add to high part
    TWOSUMBS(th2,tl2,dotproducth,dotproductl)  // combine parts, canonicalize
     // this is accurate to 104 bits from the largest of (new,old,new+old)
    goto accumulateqp; 
   }
   // if we end the loop, there must have been nothing but 0.  We know dotproducth was set to 0.
   dotproductl=dotproducth;  // set overall product to 0
   goto endqp;
accumulateqp: ;
   NOUNROLL for(++mx;mx<an;++mx){  // skip the first nonzero; for each remaining element of the dot-product
    __m256d th,tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
    D *mv=mv0[mx];  // advance the pipeline of row addresses in Qkt
    // get column number to fetch; fetch 4 rows
    th=_mm256_load_pd(&mv[rowx]);  // fetch 4 values.  Extras should be 0
    if(_mm256_testz_si256(_mm256_castpd_si256(th),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
    tl=_mm256_load_pd(&mv[rowx+qkstride]);  // fetch 4 values, low part
    vval=_mm256_set1_pd(vv0[mx]);  // load weight for the column
    // accumulate the dot-product
    TWOPROD(th,vval,th2,tl2)  // high qk * col
    accmaxabs=_mm256_max_pd(accmaxabs,_mm256_andnot_pd(sgnbit,dotproducth));   // update upper-significance holder
    tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add in extension of prev product
    TWOSUM(dotproducth,th2,dotproducth,vval)  // combine high parts in quad precision
    tl2=_mm256_add_pd(dotproductl,tl2); tl2=_mm256_add_pd(vval,tl2);  // add extensions, 3 products costs 2 bits of precision
    TWOSUM(dotproducth,tl2,dotproducth,dotproductl)  // combine high & extension for final form
    // this is accurate to 104 bits from the largest of (new,old,new+old).
   }
  }else{dotproducth=_mm256_load_pd(&vv0[rowx]); dotproductl=_mm256_load_pd(&vv0[rowx+qkstride]); accmaxabs=_mm256_setzero_pd();}  // slack vbl, just read column value, no need for noise test
endqp: ;

  // ************************************************************************** one-column mode+SPR: store out the values, setting to 0 if below threshold
  accmaxabs=_mm256_fmadd_pd(accmaxabs,_mm256_set1_pd(RELSIGMAX),store0thresh);  // no reason to keep values that are all noise.  We take 100 bits from max significance, but never less than store threshold
  __m256d okmask=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,dotproducth),accmaxabs,_CMP_GT_OQ);  // 0s where we need to clamp
  if((I)zv&ZVNEGATECOL){dotproducth=_mm256_xor_pd(sgnbit,dotproducth); dotproductl=_mm256_xor_pd(sgnbit,dotproductl);}  // Handle Enforcing column. branch will predict correctly and will seldom need the XOR
  dotproducth=_mm256_and_pd(dotproducth,okmask); dotproductl=_mm256_and_pd(dotproductl,okmask); // set values < threshold to +0, high and low parts
  _mm256_store_pd(remflgs(zv)+rowx,dotproducth); _mm256_store_pd(remflgs(zv)+rowx+zstride,dotproductl);   // store high & low.  This may overstore, and must always be aligned
if((rowx+NPAR-1)>=zstride)SEGFAULT;  // scaf
  // We treat Fk as part of the column, except that we don't want to include it in the SPR calculation.
  if(unlikely(rowx==frowbatchx)){
   // We hit the Fk batch.  Turn off the last lane of endmask, which must hold Fk
   endmask=_mm256_blend_pd(_mm256_setzero_pd(),_mm256_permute4x64_pd(endmask,0b11111001),0b0111);
   dotproducth=_mm256_and_pd(dotproducth,endmask);  // we expect dotprod 0 in any lane where endmask is 0.  Only high part used for SPR
  }
  // Calculate SPR for column.  process the values in parallel
  // Get the column values c and b values to use.  In bound columns we use abs(c). 
  __m256d bnddotproducth=_mm256_andnot_pd(_mm256_and_pd(bndmsk,sgnbit),dotproducth);  // take abs of col value if bound row
  __m256d cgt0=_mm256_cmp_pd(bnddotproducth,col0thresh,_CMP_GT_OQ);  // mask of valid c-value lanes.  Will always be 0 in lanes where endmask is 0 (must pad Qk with 0)
  if(!_mm256_testz_pd(cgt0,endmask)){  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  skip batch if so
   // there is a positive c value.  process the batch.  Get bk to use
   zv=(D*)((I)zv|ZVPOSCVFOUND);  // The positive col value makes the column bounded
   __m256d bk4, beta4;  // will be ~0 for words that have positive c; the 4 bk values we will use (with overshoot added by caller before we start)
   bk4=_mm256_loadu_pd(&bk[rowx]); beta4=_mm256_loadu_pd(&bkbeta[rowx]);  // load bk and beta to use  this overfetches the bk/bkbeta areas, which must be padded to batch length including Fk
   bk4=_mm256_blendv_pd(bk4,_mm256_sub_pd(beta4,bk4),_mm256_and_pd(bndmsk,dotproducth));  // b, except beta-b in bound rows with originally negative col
   // if bk < threshold, set it to 0.  If col value negative, mark the column invalid
   __m256d bknon0=_mm256_cmp_pd(bk4,bk0thresh,_CMP_GE_OQ);  // remember which bks are non0
   if(!((I)zv&ZVSPR0)){  // if we have not encountered SPR of 0, sharedspr is the spr
    // calculate the SPR
    bk4=_mm256_and_pd(bk4,bknon0);  // make near0 bk exactly 0
    __m256d ratios=_mm256_fmsub_pd(sharedspr,bnddotproducth,bk4);  // minspr*c-b: sign set if b>minspr*c => b/c>minspr => not new min-or-tied SPR.  minspr, c, and b are all nonneg in valid lanes, so no -0
if(!_mm256_testz_pd(_mm256_or_pd(bk4,_mm256_or_pd(sharedspr,bnddotproducth)),cgt0))ASSERTSYS(0,"neg0");  // scaf verify no -0
    if(!_mm256_testc_pd(ratios,cgt0)){  // CF is 1 if ~(not new min-or-tied)&(valid) is all 0, so 0 if any (min-or-tied)
     // SPR is a new low-or-tied.  If any valid bk is 0, we will be returning a nonimproving pivot
     // by simulation this code runs 4.7 times in 64 batches
     if(_mm256_testc_pd(bknon0,cgt0)){    // if all non0 flags in valid lanes are set, that means the SPR is nonzero 
      // New SPR is nonzero.  Calculate it and remember the new SPR in all lanes
      // We will have to find the location of the minimum value in the new batch, because it may be tied with the current minimum.  To silence invalid lanes
      // we keep the old minspr in them.  Could use infinity, but we might calculate an SPR of inf
      __m256d sprs=_mm256_blendv_pd(sharedspr,_mm256_div_pd(bk4,bnddotproducth),cgt0);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch because it ties up IUs for so many cycles
      // Find the new minimum
      sharedspr=_mm256_min_pd(sprs,_mm256_permute_pd(sprs,0b0101)); // sharedspr = min01 min01 min23 min23  could use integer in AVX-512
      sharedspr=_mm256_min_pd(sharedspr,_mm256_permute4x64_pd(sharedspr,0b00001010));  // sharedspr=min value in all lanes
      // remember the row that had the minimum.  Make sure we take only SPRs for valid col values
      limitrowx=rowx+CTTZI(_mm256_movemask_pd(_mm256_and_pd(cgt0,_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(sharedspr),_mm256_castpd_si256(sprs))))));  // index of a lane containing the minimum SPR (there must be one)
      // it is just barely possible that the division to calculate SPR gives a zero value even though bk is not 0 (underflow).  Since SPR=0 is a flag value indicating
      // no improvement, we will miss the limitrowx that is set here, and the pivot will be marked dangerous unless a different nondangerous nonimproving pivot
      // is found elsehere.  This can happen only once per thread, because once SPR=0 it is impossible to get low-or-tie unless bk=0.  We ignore the possibility,
      // because the column value would have to be HUGE to underflow to 0.
     }else{sharedspr=_mm256_setzero_pd(); if(parms[10]==0.0){limitrowx=-2; goto return4;} zv=(D*)((I)zv|ZVSPR0); goto startspr0;}   // FIRST 0 SPR: switch modes, change sharedspr to look for max column value; abort if SPR>0 needed; process through 0-SPR path
            // we don't check for column aborts from other threads because the juice isn't worth the squeeze
    }
   }else{
    // we have encountered an earlier SPR of 0.  We don't need to calculate an SPR: we choose the largest column-value where bk is near0 (if it's dangerous we catch it later), and break ties randomly
    // here sharedspr is the largest column value found so far
startspr0:;  // come here for the first 0-SPR batch, after mode switch.  The new column value may be very small but it is always > 0, so we will treat it as a starting batch below
    // take max of sharedspr and new column values where bk is 0.
    __m256d validcol=_mm256_and_pd(bnddotproducth,_mm256_andnot_pd(bknon0,cgt0));   // valid column values where bk is near0, 0 elsewhere.  Could do a single blend but we fear garbage in invalid col value   
    if(!_mm256_testz_pd(_mm256_cmp_pd(validcol,sharedspr,_CMP_GE_OQ),cgt0)){  // Zf if validcol<old max in all valid lanes.  We skip the batch then
     __m256d maxvalidcol=_mm256_max_pd(validcol,_mm256_permute_pd(validcol,0b0101)); maxvalidcol=_mm256_max_pd(maxvalidcol,_mm256_permute4x64_pd(maxvalidcol,0b00001010));  // maxvalidcol= max value in all lanes
     I candlanes=_mm256_movemask_pd(_mm256_cmp_pd(validcol,maxvalidcol,_CMP_EQ_OQ));   // mask of lanes that could be the result
     // Select a lane: if only one, take it, otherwise select a random lane containing the new maximum.  This distribution is not uniform, but that's OK since we are just trying to break cycles
     I randval=-1;  // init to no random value drawn  Low quality is OK.  We leave RNG unseeded so that repeated runs will give different sequences
     I lanehere=rowx+CTLZI(LOWESTBIT(candlanes)|(likely(candlanes==LOWESTBIT(candlanes))?0:(candlanes&(randval=rand()))));  // If multiple matches, randomly turn off candidates above the lowest; then find highest survivor
     if(likely(!_mm256_testz_pd(_mm256_cmp_pd(validcol,sharedspr,_CMP_GT_OQ),cgt0))){  // Zf if validcol<=old max in all valid lanes.  If not, we have a new undisputed high
      nzerobatches=1.0; limitrowx=lanehere;  // new maximum batch.  unconditionally take the value and set to 1 in equal group
      sharedspr=_mm256_max_pd(sharedspr,maxvalidcol);  // update shared max value to date
     }else{
      // Tie value with a previous batch.  randomly accept the new value so as to give a uniform distribution of batches
      nzerobatches+=1.0;  // include the new batch in the count of batches encountered
      limitrowx=(double)(randval<0?rand():randval)*nzerobatches<=(double)RAND_MAX?lanehere:limitrowx;  // accept new lane with declining probability
     }
    }
   } 
  } // skip batch if all column values 0

  bndmsk=_mm256_castsi256_pd(_mm256_slli_epi64(_mm256_castpd_si256(bndmsk),1));  // move bound-row mask to the next group of rows.
  rowx+=NPAR;  // advance input row number
 }

 // End-of-column processing.  Collect stats and update parms for the next column
 // we are sharing the SPR, looking for the smallest
 //  limitrowx has the row that the shared value was found in, or IMAX if none found to tie the incumbent

 I valuetoshareminof;   // because atomics run int-only, this is the bit-pattern that we will minimize as an I
 // sharedspr has the value to share.  We might not have achieved it here, though.
 // We recognize the value only if we actually achieved it in this thread, so that we won't report out invalid limitrowx (the issue is that
 // sharedspr may contain the initial value and we would have no valid value to override the default with)
 // If SPR=0, sharedspr has the positive column-value to maximize (it could be dangerous, but never 0).  We take the negative reciprocal, to give us something to minimize in fixed-point. 
 if((I)zv&ZVSPR0)sharedspr=_mm256_div_pd(_mm256_set1_pd(-1.0),sharedspr);  // if 0-SPR, neg-recip.  IEEE754 is sign/magnitude so the sign bit is a flag giving priority to 0-SPR but looking for small recip-column-value
 else if((I)zv&ZVPOSCVFOUND)sharedspr=_mm256_min_pd(_mm256_set1_pd(ALMOSTINF),sharedspr);  // If we found pos col value, noodge SPR to indicate that fact
 valuetoshareminof=_mm256_extract_epi64(_mm256_castpd_si256(sharedspr),0);

 // add to the stats
 I impcolincr;  // value we will add to the col-stats field
 //  rowx must point to the batch after the last one we processed and must be on a BNDROW bdy
 impcolincr=0x100000001;
 // exit if we have processed enough columns (in DIP mode)
 I nimpandcols=__atomic_add_fetch(&ctx->nimpandcols,impcolincr,__ATOMIC_ACQ_REL);  // add on one finished column, and 0 or 1 improvements
 ndotprods+=rowx;  // accumulate # products performed, including the one we aborted out of
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products

// return result to the ctx, if we have the winning value.  Simple writes to ctx do not require RELEASE order because the end-of-job code
// and the threads sync through ctx->internal.nf; but they should be atomic in case we run on a small-word machine

 // if the value we reported out might be the best, store its completion info out if it is actually the best.
 // For the nonce, we hope the thread ordering gives enough randomization for tied zero SPRs
 // We have to write on = so that the row# will be written for zero SPRs where some threads had no nondangerous pivots
 // Don't write if we have no valid row# to write: if no thread has one, the original value will persist.  The danger is that we will
 // start with the winning SPR coming from another thread (or the initial value), and at the end we have no row for it to have come from
 if(limitrowx!=IMAX && valuetoshareminof<=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE)){  // check outside of lock for perf
  WRITELOCK(ctx->ctxlock)
  if(valuetoshareminof<=ctx->sharedmin.I){ctx->sharedmin.I=valuetoshareminof; ctx->retinfo=limitrowx;} // if we still have the best gain, report where we found it
  WRITEUNLOCK(ctx->ctxlock)
 }
 R 0;

// following are the early return points: unbounded, pivot out virtual

return4:;  // we have a preemptive result.  store and set minimp =-inf to cut off all threads
 // the possibilities are unbounded and pivoting out a virtual.  We indicate unbounded by row=-1.  We may overstore another thread's result; that's OK
 // We must do this under lock to make sure all stores to bestcol/row have checked the minimp they use
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 WRITELOCK(ctx->ctxlock)
 ctx->sharedmin.I=IMIN;  // shut off other threads
 ctx->retinfo=limitrowx;  // set unbounded col or garbage, if aborting on 0 SPR
 WRITEUNLOCK(ctx->ctxlock)
 R 0;
}
#endif //C_AVX2 || EMU_AVX2

// 128!:9 matrix times sparse vector with optional early exit
// one column + SPR mode: (qp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
//   Ax,Am,Av are the sparse elements of A0
//   M is Qk.  There may be padding rows but NOT columns
//   RVT is Rose variable types, indicating which columns are (bound,enforced) (littleendian order, with enforced being the LSB)
//   bndrowmask indicates, for each row in Qk, whether the row is Bound
//   bk is the current bk (must include a 0 corresponding to Fk and then MUST BE 0-EXTENDED to a batch boundary) 
//   z is the result area for the requested column - qp, and must be able to handle overstore to a full batch, including the Fk value if any
//   ndx is the atomic column# of NTT requested
//   parms is #cols,x,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivotImp,ColOkPivotNonImp,Bk0Threshold,BkOvershoot,[MinSPR],[PriRow]
//    #cols is number of columns in the Ek portion of Qk; if Fk present as a row, value is 1s-comp
//    maxAx is the length of the longest dot-product
//    Col0Threshold is minimum |value| considered non0 for a column value
//    Store0Thresh: if |column value| is less than this, force it to 0
//    ColDangerPivot is the smallest allowed pivot, but is dangerous
//    ColOkPivotImp is the smallest improving pivot value considered non-dangerous
//    ColOkPivotNonImp is the smallest improving pivot value considered non-dangerous
//    Bk0Threshold is the smallest bk value considered nonzero
//    BkOvershoot is the maximum excursion into negative bk that we will allow on a pivot
//    MinSPR is the largest SPR that will abort this column (set to __ to allow any SPR, to 0. to require improvement)
//    PriRow is the priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so
//   bkbeta is beta for each row in Qk, i. e. corresponding to bk (including 0 extension)
//   beta is betas for all columns, used to init SPR for bound column
//  if ndx<m, the column is  ndx {"1 M; otherwise ((((ndx-m){Ax) ];.0 Am) {"1 M) +/@:*"1 ((ndx-m){Ax) ];.0 Av
//   multiplied by -1 if the column is enforcing
//  Do the product in quad precision
//  the column data is written to z
//  result is rc,pivotrow,#cols processed,#rows processed,(smallest SPR if >0,neg of largest col value if SPR=0 (nonimproving pivot))
//   rc is 0 (normal), 1 (dangerous), 2 (nonimproving), 3 (nonimproving and dangerous), 4 (preemptive result: SPR was less than MinSPR (SPR=0, row=-2); unbounded (SPR=inf, row=-1) )
// gradient mode: (dp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
//   cutoffinfo is internal stored state saved between gradient-mode calls; shape n,2
//   parms is #cols,maxAx,x,x,MinGradient/MinGradImp,x...
//    MinGradient is normally 0.
//      if positive, we do NOT share gradient values, and we finish up gradient calculation in any column that has a larger |gradient|, storing the len/sumsq in cutoffinfo
//      if negative, cut off a column when its gradient cannot beat the best-so-far by more than a factor of 1/(1-MinGradImp), i. e. MinGradImp=(min OK grad/actual max grad)-1
//   Frow is the selector row
//  result is rc,pivotcol or unbounded col,#cols processed,#rows processed,largest |gradient|
//   rc is 0 (normal), 4 (unbounded problem)
//   if MinGradient is given, rc is 0, col is _1, and recip gradient is 0 (all this because no reporting is done)
// findimprovement mode: (dp dotproduct) (not yet implemented)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta  where ndx is a list and z is empty
//   parms is #rows,maxAx,ColOkPivot,x,x,x,x,Bk0Threshold,BkOvershoot,PriRow
//  result is list of columns that have positive SPR (no line with col>ColBk0Threshold && bk<Bk0Threshold)
//  
// Rank is infinite
F1(jtmvmsparse){F12IP;PROLOG(832);
#if C_AVX2 || EMU_AVX2
 if(AN(w)==0){
  // empty arg just returns stats; none defined yet
  R mtv;
 }
struct mvmctx opctx;  // parms to all threads, and return values

 ASSERT(AR(w)==1,EVRANK);
 ASSERT(AN(w)>=11,EVLENGTH);  // audit overall w
 ASSERT(AT(w)&BOX,EVDOMAIN);
 A box;
 box=C(AAV(w)[9]); ASSERT(AT(box)&FL,EVDOMAIN); ASSERT(AR(box)==1,EVRANK); ASSERT(AN(box)>0,EVLENGTH);  // parm shape, type
 D *parms=DAV(box); I nparms=AN(box); I nandfk=opctx.nbasiswfk=(I)parms[0];   // flagged #rows in Qk
 box=C(AAV(w)[3]); ASSERT(AR(box)==3,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.qkt0=DAV(box); I nbasiscols=AS(box)[1]; // Qkt, possibly including space and Fk; # cols in basis
 opctx.qktrowstride=AS(box)[2];  // length of allocated row of Qkt, padded to batch multiple
 opctx.nqkbcols=AS(box)[1];  // number of rows in Qkt which is always # basis columns of Qk
 ASSERT(AS(box)[0]==2,EVLENGTH)  // Qk is qp
 I ninclfk=ABS(nandfk);   // number of rows to be processed including Fk
 ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"Qkt is not on cacheline bdy")  // we fetch along rows; insist on data alignment
 ASSERTSYS((AS(box)[2]&(NPAR-1))==0,"stride of Qkt is not a cacheline multiple")  // we fetch along rows; insist on data alignment
 box=C(AAV(w)[0]); ASSERT(AR(box)==3&&AS(box)[1]==2&&AS(box)[2]==1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN); I axn=AS(box)[0]; opctx.u.spr.axv=opctx.u.grad.axv=((I(*)[2])IAV(box))-nbasiscols;  // prebiased pointer to A0 part of NTT
 box=C(AAV(w)[1]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN); I amn=AN(box); opctx.amv0=IAV(box);  // col #s in basis
 box=C(AAV(w)[2]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN); I avn=AN(box); opctx.avv0=DAV(box);  // weights
 ASSERT(amn==avn,EVLENGTH);  // weights and col#s must agree
 box=C(AAV(w)[4]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&LIT,EVDOMAIN) C *rvtv=CAV(box);  // RVT
 box=C(AAV(w)[5]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&LIT,EVDOMAIN) opctx.bndrowmask=DAV(box);  // bndrowmask
 box=C(AAV(w)[8]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN)   // col indexes being evaluated
 I isgradmode; I nthreads=(*JT(jt,jobqueues))[0].nthreads+1;   // non0 if gradient mode; ptr to output if any; #threads available for processing
 unsigned char (*actionrtn)(JJ, void *, UI4);  // the routine to do the operation
 if(isgradmode=(AR(box)!=0)){ 
  // gradient mode (the dominant case)
  opctx.u.grad.rvtv=rvtv;
  I nc=opctx.u.grad.nc=AN(box);  ASSERT(AN(w)==11,EVLENGTH);  // audit overall w
  ASSERT(nc!=0,EVLENGTH)
  I *ndxa=opctx.u.grad.ndx0=IAV(box);  // addr of col#s
  DO(nc, ASSERT(BETWEENO(ndxa[i],0,nbasiscols+axn),EVINDEX))// audit col#s, which must be in range 0 to NTT width, i. e. #slacks + #decision vbls
  // box 6 reserved for schedule
  box=C(AAV(w)[7]); ASSERT(AR(box)==2,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.u.grad.cutoffstatus=(D(*)[2])DAV(box);  // saved info from previous scans
  ASSERT(nparms==6,EVLENGTH)  // gradient mode doesn't use much parms
  opctx.u.grad.maxweights=parms[1];  // max # weights in any column (gradient only)
  opctx.u.grad.mingradimp=parms[4];  //   0 normally; >0 => finish up gradient calculation in any column that has a larger |gradient|, storing the len/sumsq in cutoffinfo; <0-> accept any gradient as high as 1/(1-MinGradImp) * true max
  box=C(AAV(w)[10]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.u.grad.Frow=DAV(box);  // the entire selector row
  nthreads=likely(ninclfk*nc>1000)?nthreads:1;  // single-thread small problem
  // We estimate 1 gradient per ns.  Reduce number of threads to leave 2us of work for each thread
  I ressize;  // number of columns for each thread to take initially
  if(unlikely(ninclfk*nc<2000*nthreads)){I nneed=((ninclfk*nc)/2000); nthreads=MIN(nneed,nthreads); nthreads=MAX(1,nthreads);}
  // if the initial reservation is only 1, the thread code will not enter 0..1 state to request another resv; thus in that case nc must=1.  And init resv of 1 is bad anyway, because
  // all the threads will start and nothing will cut off.  So we limit the # threads to half the number of columns to do
  nthreads=MIN(nthreads,nc>>1);  // will ensure init resv of 2 unless nc=1
  if(likely(nc>nthreads*(BIGRESV+SMALLRESV)))ressize=BIGRESV;  // if plenty of cols, take max resv
  else if(nc<SMALLRESV*nthreads)ressize=nc/nthreads;  // not even a SMALL per thread - make initial alloc as big as possible, leaving just a little
  else{ressize=(nc-(nthreads*(SMALLRESV>>1)))/nthreads; ressize=MIN(ressize,BIGRESV);}  // more than a SMALL per thread - leave half a SMALL per thread
  opctx.ressize=ressize; opctx.nextresv=ressize*nthreads;  // set init resv size, and start of first to arbitrate for.  It would be OK for the last thread to have a partial allo, but we don't create that case
  // Switch to SMALL resv when we get close enough to the end.  The pipelined reservations are cheap, but we would like to keep column affinity
  opctx.u.grad.resredwarn=BIGRESV+nthreads*SMALLRESV;  // switch after the last BIGRESV that leaves enough room for each thread to take 1 more resv
  opctx.sharedmin.D=inf; // minimum improvement found.  Must be valid float so we can compute cutoff value
  opctx.retinfo=-1;  // Init col#, should never be survive unless no column ran to completion
  actionrtn=(unsigned char (*)(JJ, void *, UI4))jtmvmsparsegradx;
 }else{
  // onecol+SPR mode
  I colx=opctx.u.spr.colx=IAV(box)[0];  // there is only one col
  ASSERT(BETWEENO(colx,0,nbasiscols+axn),EVINDEX)  // verify in range
  ASSERT(AN(w)==12,EVLENGTH);  // audit overall w
  opctx.u.spr.parms=parms;
  box=C(AAV(w)[6]); ASSERT(BETWEENC(AR(box),1,2),EVRANK) ASSERT(AS(box)[AR(box)-1]>=((ninclfk+NPAR-1)&-NPAR),EVLENGTH) ASSERT(AT(box)&FL,EVDOMAIN) opctx.u.spr.bk=DAV(box);  // bk values allow overfetch; we use only high part.  Ensure big enough for pad
  I bkzstride=AS(box)[AR(box)-1];  // all the strides must be equal; we have just verified they are big enough
  ASSERTSYS((bkzstride&(NPAR-1))==0,"stride of bk is not a cacheline multiple")  // every row must be aligned
  ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"bk is not on cacheline bdy")  // we fetch along rows; insist on data alignment
  box=C(AAV(w)[7]); ASSERT(AR(box)==2,EVRANK) ASSERT(AS(box)[0]==2,EVLENGTH) ASSERT(AS(box)[AR(box)-1]==bkzstride,EVLENGTH) ASSERT(AT(box)&FL,EVDOMAIN)
  // get column info, a 2-bit field of (bound,enforcing)
  I colrvt=(rvtv[colx>>(LGBB-1)]>>((colx&((1LL<<(LGBB-1))-1))<<1))&((1LL<<2)-1);
  opctx.u.spr.zv=(D*)(((I)DAV(box)&~7)|((colrvt&ZVNEGATECOL)<<ZVNEGATECOLX));  // flags: enforcing column requiring negation, abort on nonimproving row
  ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"column result is not on cacheline bdy")  // we store along rows; insist on data alignment
  ASSERT(BETWEENC(nparms,10,12),EVLENGTH)  // SPR has lots of parms
  box=C(AAV(w)[10]); ASSERT(BETWEENC(AR(box),1,2),EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) ASSERT(AS(box)[AR(box)-1]==bkzstride,EVLENGTH) opctx.u.spr.bkbeta=DAV(box);  // beta values corresponding to bk; we use only high part
  ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"bkbeta is not on cacheline bdy")  // we fetch along rows; insist on data alignment
  opctx.u.spr.zstride=bkzstride;   // use the agreed stride
  // The initial shared value is infinity except in bound columns: then beta.  Initialize the found-row value to -1 for 'nothing found' normally,
  // but for bound columns use #rows which indicates a Nonbasic Swap
  if(colrvt&0b10){
   box=C(AAV(w)[11]); ASSERT(BETWEENC(AR(box),1,2),EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) ASSERT(AN(box)>colx,EVINDEX)  // betas normally dp, but we use only high part anyway.  Look up the column beta
   opctx.sharedmin.D=DAV(box)[colx];   // init bound-col min SPR to beta
   opctx.retinfo=ninclfk;  // Init col# - if no lower SPR found could survive to become a Swap
  }else{opctx.retinfo=-1; opctx.sharedmin.D=inf;} // non-bound, allow any improvement, init ret to 'no row found' - possible if no nondangerous pivot, or unbounded
  // Calculate the number of rows to reserve.  The issue is that some rows are faster than others: rows with all 0s skip extended-precision
  // processing.  It might take 5 times as long to process one row as another.  To keep the tasks of equal size, we take a limited number of rows at a time.
  // Taking the reservation is an RFO cycle, which takes just about as long as checking a set of all-zero rows.  This suggests that the reservation should be
  // for NPAR*nthreads at least.  And, the bound/enforcing mask is constructed to match the AVX lanes: one quantum of mask has (NPAR*BW/2) bitpairs; reservation must be a multiple of that

  nthreads=likely(ninclfk>256)?nthreads:1;  // single-thread small problem (qp).  Multithreading is questionable as the result column will be read in the master thread
  I nbndbatches=(ninclfk+(BNDROWBATCH-1))>>LGBNDROWBATCH;  // number of bnd batches, including a final shard
  nthreads=MAX(1,MIN(nbndbatches,nthreads));  // each batch except the last must have a full BNDROWBATCH
  // First, the size of each later reservation, which needs to be big enough that the fetch_adds don't pile up
  UI4 ressize2, ressize;  // the number of rows in each reservation after the first, the number in the first
  ressize2=ROWSPERRESV*nthreads;   // enough rows to allow each thread to arbitrate if they have normal jobs
  ressize2=(ressize2+(BNDROWBATCH-1))&-BNDROWBATCH;  // the bound/enforcing mask is built in units of BNDROWBATCH bits so as to fit in an AVX register, matching the row numbers.
  if(ninclfk<2*ressize2*nthreads){
   // the amount to be reserved after the initial resv is more than half the total.  We should move that to the
   // initial allocation, since there is no arbitration there
   ressize=(nbndbatches/nthreads)<<LGBNDROWBATCH;  // the most ressize can be is enough batches not to overrun the input
   ressize=MIN(ressize,32768-BNDROWBATCH);  // init alloc must fit into a US
   ressize=MIN(ressize,ressize2);  // a good and legal starting size
   ressize2=ninclfk/nthreads-ressize; ressize2=MAX(1,ressize2); ressize2=(ressize2+(BNDROWBATCH-1))&-BNDROWBATCH;  // distribute remaining batches evenly over the threads, always >0 per reservation
  }else{
   // the initial allocation can be followed by allocations of size ressize2.  Allocate half the rows in the initial allocation
   ressize=((nbndbatches>>1)/nthreads)<<LGBNDROWBATCH;  // half the total batches, evened out per thread.  There must be at least 1
   ASSERTSYS(ressize>0,"ressize")
   ressize=MIN(ressize,32768-BNDROWBATCH);  // init alloc must fit into a US
  }
  ressize=((nthreads-2)&(ninclfk-32768))<0?ninclfk:ressize;  // if just 1 thread and length fits in a US, allocate it all at the beginning
  opctx.ressize=ressize;
  opctx.nextresv=nthreads*ressize;  // set resv pointer to after the initial allocation
  opctx.u.spr.ressize2=ressize2;

  actionrtn=(unsigned char (*)(JJ, void *, UI4))jtmvmsparsesprx;
 }

 // initialize shared/returned section
 opctx.nimpandcols=0;  // number of improvement,number of cols finished
 opctx.ndotprods=0;  // total # dotproducts evaluated
 opctx.ctxlock=0;  // init lock available

 jtjobrun(jt,actionrtn,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 // atomic sync operation on the job queue guarantee that we can use regular loads from opctx

 A r; GAT0(r,FL,5,1); D *rv=DAV1(r);  // return area
 I retinfo=rv[1]=opctx.retinfo; rv[2]=(UI4)opctx.nimpandcols; rv[3]=opctx.ndotprods;
 if(isgradmode){
  // gradient mode.  We were minimizing 1/gradient^2; convert to gradient
  rv[0]=0.; if(unlikely(opctx.sharedmin.I==IMAX))rv[0]=3.; else rv[4]=sqrt(1/opctx.sharedmin.D);  // give ok return w/gradient.  In ridiculous case where no gradient was found, return no pivot
 }else{
  // onecol+SPR.  classify as normal/dangerous/nonimproving/nonexistent/unbounded
  D spr=rv[4]=opctx.sharedmin.D;  // return SPR/col val as informational data
  if(unlikely(opctx.sharedmin.I==IMIN)){rv[0]=4.;  // abort on SPR=0; set rc 4, leave row -2 and SPR 0
  }else if(unlikely(spr==inf)){rv[0]=4.; rv[1]=-1.;  // no SPR found: problem is unbounded.  Leave SPR inf as indication, set row=-1
  }else if(unlikely(spr==ALMOSTINF)){rv[0]=3.;  // Positive col value found, but no pivots, not even dangerous ones (perhaps blocked by small bk)
  }else{
   // sharedmin is positive SPR if improving; negative recip of max column value if nonimproving
   rv[0]=spr<0?2.:0.;  // rc if not dangerous: negative shared value is the reciprocal of the column value when SPR=0.  This is a nonimproving pivot, rc=2 then
   if(BETWEENO(retinfo,0,ninclfk)){  // normal nonswap row found
    D colval=ABS(remflgs(opctx.u.spr.zv)[retinfo]);  // the value in the SPR row of the column.  If neg, we will swap before pivot, so use |value|
    if(unlikely(colval<parms[spr<0?7:6]))rv[0]+=1.0;  // flag if dangerous, with threshold depending on improving status
   }  // if nonbasic swap row found, we will take the swap willy-nilly because its dangerousness doesn't hurt anything
  }
 }
 EPILOG(r);
#else
 ASSERT(0,EVNONCE);  // this requires AVX to make any sense
#endif
}

// start of 128!:19
#if C_AVX2 || EMU_AVX2

// the processing loop for one core.  Gradients, over all columns
// ti is the piece# that this thread is taking, used for initial allocation
// TODO
// initial reservation
// remove indirect refs through ctx
// if only 1 weight, add another 0 row, remove test
// pipeline reservation
// no unroll mx
static unsigned char jtmvmsparseegradx(J jt,struct mvmctx *ctx,UI4 ti){
 // transfer everything out of ctx into local names.  We start with the data in the first cacheline and go from there, attempting to read
 // in the order the data will be needed.  We use atomic reads to force the reads to start early, since all but the first cacheline are remote now
#define YCUT(T,x) T x=(T)__atomic_load_n(&ctx->u.grad.x,__ATOMIC_ACQUIRE);
#define YCU(x) YCUT(typeof(ctx->u.grad.x),x)
#define YCT(T,x) T x=(T)__atomic_load_n(&ctx->x,__ATOMIC_ACQUIRE);
#define YC(x) YCT(typeof(ctx->x),x)
 YCU(maxweights)
 float mingradimp; *(I4*)&mingradimp = (I4)__atomic_load_n((I4*)&ctx->u.grad.mingradimp,__ATOMIC_ACQUIRE);  // needed to allocate row/weight area & init zv
 YCT(I,ressize)YCU(nc)  // needed for initial reservation
 YC(nbasiswfk)YCT(I,nqkbcols)   // needed in pipe stage -1, even first time
 YCU(ndx0)   // needed in final reservation stage, even first time
 YCU(cutoffstatus)YCU(axv)YCU(Frow)YCU(rvtv)  // needed in pipe stage -2
 YC(amv0)YC(avv0)  // needed in pipe stage -1, but not first time
 YC(qkt0)YC(bndrowmask)YCT(I4,qktrowstride)YCU(resredwarn)  // needed only after pipeline initialization
#undef YC
#undef YCT
#undef YCU
#undef YCUT
 // perform the operation
 I minvalueweshared=IMAX;  // the smallest value we have shared
 __m256d sqrt2=_mm256_set1_pd(sqrt(2.0));   // sqrt(2) for weighting bound rows
 I ndotprods=0, nimpandcols=0;  // number of dot-products we perform here, and #cols and improvements
 I nqkrows=nbasiswfk^REPSGN(nbasiswfk); // number of rows of Qk NOT including any Fk
 __m256i bndmskshift=_mm256_set_epi64x(0, 0, 16, 16);  // bndmsk is littleendian, but reversed in 16-bit sections.  last section goes to lane 3, i. e. no shift
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);

 I mvvvtoalloc=(maxweights+1+NPAR-1)&-NPAR;  // round (max Axlen+1) up to batch bdy
 I bytesperppb=offsetof(struct pingpong,mvvv[2*mvvvtoalloc]);  // space for header, then Am followed by Av
 A ppb; GATV0(ppb,LIT,2*bytesperppb,1);  // two buffers
 struct pingpong *ppp=voidAV1(ppb);  // ping-pong pointer.  At top of loop, the block just used (=next to fill); after pipe, the block for the column to process
 I pppf=(I)ppp^((I)ppp+bytesperppb);  // (pingpong 0)^(pingpong 1), used to flip the pingpong

 I zv;
 D sharedminslack;  // value to multiply sharedmin by to get a value to beat.  1.0=no change, smaller values cut off columns earlier
 D sharedmin;  // recent minimum recip gradientsq found, updated at start of most columns
 if(mingradimp<0){  // If MinImp given...
  sharedminslack=1.0+mingradimp; sharedminslack*=sharedminslack; zv=ZVSHARINGMIN; *(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE); // create derater for recip colsq
 }else{
  sharedminslack=1.0;  // normal case, do not derate the gradient
  if(unlikely(mingradimp>0)){  // MinGradient given...
   sharedmin=1.0/mingradimp; sharedmin*=sharedmin;  // remember 1/(min gradient to compute)^2, which is what we share
   zv=0;  // init to suppress gradient update
  }else{zv=ZVSHARINGMIN; *(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE); // normal init, share gradients; init gradient
  }
 }
 zv|=ZVMAXROWS&MAXCACHEDROWS;  // install max # rows of fetched data in cache (32B per row), per column fetched
 // all of zv has been initialized.  COLCT is 0 indicating initialization; NDAX and PIPE are clear

 if(unlikely(nc==0))R 0;  // abort with no action if no columns

 I nextcol=ressize*ti;  // initial reservation - a different block for each job
 PREFETCH2(&ndx0[nextcol]);  // start moving the reservation into L2

 I retinfo;  // the return data for the best value we shared (col).  No need to initialize because we don't report a value if we never shared a gradient

 I4 __attribute__((aligned(NPAR*SZI))) localndx[BIGRESV+2];  // local storage for the indexes.  2 is for overfetch at end-of-pipe. I4 to save space - questionable decision

 I colxm1, anm1, axm1, cutrowm1=nqkrows, colrvtm1; D frowm1=1.0, cutsumsqm1=0.0;  // pipeline values passed from column to column: column#, Ax, cutoffinfo, frow
   // init cutrowm1/cutsumsq1 to look like a completed zero column; frowm1 to a gentlemanly selector

 do{  // loop till the pipeline has drained.  Reservations are seamlessly chained together
  // start of processing one column

  // column-initialization and reservation pipeline.  To run the column we start with fetches of col#, then Frow, cutoffinfo, indexes and weights.  If we are not processing a cached column each of these fetches
  // might miss out to D3$ or beyond.  To break the dependency chain, we pipeline each fetch.  ZVPIPE gives validity information for each stage.  The final stage is the
  // ping-pong buffer
  // The bit is set in ZVPIPE if a value was requested for the pipe stage in the previous column, and we expect it to be ready now
  // Stage -2 is always ready to run, possibly with a benign input.  ZVCOLCTVALID is 1 here if the value produced by -2 is valid
  // A new reservation is taken when the previous reservation is ending.  It is timed by the pipe counter:
  // counter=2: read the current shared column counter
  // counter=1: do the RMW cycle to take the reservation
  // counter=0: read in the reserved NTT column numbers.  During this step the last column of the previous reservation is started in pipe stage -2; the
  //   next time through the pipe will use the new NTT column values
  do{
   // process the pipe stages in reverse order, executing each only if there is valid data for it
   // We use conditionals and initial values to neutralize each stage, since testing the stage would mispredict several times per reservation
   // Pipe stage -1: read Am, Av.
   ppp->frowsq=frowm1*frowm1;
   ppp->colx=colxm1;  // save col# before we flag it
   D cutoffsumsq=sharedmin*sharedminslack*ppp->frowsq;   // Frow^2 * best sumsq / best Frow^2, which is cutoff point for sumsq in new column (Frow^2)/sumsq > bestFrow^2/bestsumsq)
   if(cutrowm1!=0){
    // we have partial results for the column.  Install them into ppp.  If they are enough to avoid processing the column, set the column number invalid (-1) which will prevent
    // us from wasting time fetching the column; we will skip it when it reaches the end of the pipe.  The sharedmin was read at the beginning of the column that just finished
    ppp->initrowct=cutrowm1; ppp->initsumsq=cutsumsqm1;  // init portion to process
    colxm1=ppp->initrowct>=nqkrows?-2:colxm1;   // colx=-2 if column has been processed to the end
      // cutoffsumsq took a long time to compute so we don't want to delay colxm1 till it is ready
   }else{
    ppp->initrowct=0; ppp->initsumsq=!(colrvtm1&0b10)?1.0:2.0;  // no info on column.  Init the col total with 1.0 + (1 more if hidden row exists)
   }
   if(unlikely(ppp->initsumsq>cutoffsumsq))colxm1=-3;    // if initial column position has already cut off, abort.  cutoffsumsq is still in process and we need colx now
   // At this point colxm1 has been set to -2 or -3 if the column doesn't need to be evaluated.  That flag info will be transferred to an
   if(colxm1>=nqkbcols){I avx;  // index of Am/Av block being moved
    // decision variable.  convert the column indexes into pointers into Qkt, and interleave them with the values
    if(unlikely(!BETWEENC(anm1,2,mvvvtoalloc))){
     ASSERTSYS(anm1!=0,"column has no weights");  // column must have at least 1 weight
     ASSERTSYS(anm1<mvvvtoalloc,"column has too many weights");
    }
    ppp->an=anm1-1; I *amv=amv0+axm1; D *avv=avv0+axm1;  // number of sparse atoms in each row-1; pointer to first col#; pointer to first weight
    // Stage the columnss and weights into D1$, in the ping-pong buffer.  This does an extra write & read compared to converting them to interleaved address/value directly,
    // but that takes 12 uops/4 weights, which for a column with a lot of weights will overfill the reorder buffer.  The staging takes 6 uops/4 weights
    NOUNROLL for(avx=0;avx<anm1;avx+=NPAR){  // for each block of input... (this overfetches but that's OK since we map enough for one 32-byte final fetch)
     _mm256_store_si256((__m256i*)&(ppp->mvvv)[avx],_mm256_loadu_si256((__m256i*)&amv[avx]));  // m0-m3
     _mm256_store_si256((__m256i*)&(ppp->mvvv)[avx+mvvvtoalloc],_mm256_loadu_si256((__m256i*)&avv[avx]));  // v0-v3
    }
   }else{
    // Slack variable, or initial/aborted with flag value.  Skip the weighting.
    colxm1=colxm1<0?colxm1:-1; ppp->an=colxm1;  // transfer the 'aborted column' flag to an; if normal Slack variable, set an=-1 as flag
   }
   // end pipe stage -1

   ppp=(struct pingpong*)((I)ppp^pppf);  // we have launched loads to fill the ping buffer (the one used in the previous loop).  Now flip to process pong, which was settling while ping was executing

   I colxm2=localndx[zv>>ZVNDAXX];  // Fetch next column# before we fill localndx for the new reservation.  This may be an overfetch, which we will modify later; or an initial fetch of garbage

   // If the current reservation is ending, start on the next one.  We do this after pipe stage -1 because both blocks may release a slug of offcore reads, and
   // we want to get the ones for pipe stage -1 first so we can start that column
   if((zv&ZVCOLCT)<=ZVCOLCTVALID+(2<<ZVCOLCTX)){  // if reservation is ending...
    if(nextcol<nc){
     // When nextcol exceeds the number of columns, we are finished.  This may happen when we fetch the current pointer, or make a reservation, or finish a reservation.
     // At that point, we stop coming through this block and when the pipe eventually drains we are finished
     if((zv&ZVCOLCT)==ZVCOLCTVALID+(2<<ZVCOLCTX))nextcol=__atomic_load_n(&ctx->nextresv,__ATOMIC_ACQUIRE);  // cycle 1..2, read the current col pointer
     else if((zv&ZVCOLCT)==ZVCOLCTVALID+(1<<ZVCOLCTX)){ressize=nextcol>resredwarn?SMALLRESV:BIGRESV; nextcol=__atomic_fetch_add(&ctx->nextresv,ressize,__ATOMIC_ACQ_REL);}  // cycle 1..1, make reservation
     else{I i;  // must be cycle 1..0 or 0 (initialization)
      // previous reservation will release its last column into -2 this cycle.  Copy the next NTT cols indexes.  This will release BIGRESV/(2*NPAR)*(7) instructions waiting for offcore.  56 inst, OK
      ressize=nextcol+ressize; ressize=ressize>=nc?nc:ressize; ressize-=nextcol; // calc end of our reservation, never longer than the data; convert to size
      __m256i swizzle=_mm256_set_epi32(7,5,3,1,6,4,2,0);  // for each dest lane, the source lane that has the value
      for(i=0;i<ressize;i+=2*NPAR){__m256i x0, x1;  // input locations
       x0=_mm256_loadu_si256((__m256i*)&ndx0[nextcol+i]); if(i+NPAR<ressize)x1=_mm256_loadu_si256((__m256i*)&ndx0[nextcol+i+NPAR]);  // x0=0o 1o 2o 3o x1=4o 5o 6o 7o  (o=zeros)
       __m256i shortx=_mm256_permutevar8x32_epi32(_mm256_blend_epi32(x0,_mm256_slli_epi64(x1,32),0b10101010),swizzle);  // ->o4 o5 o6 o7->04 15 26 37->01 23 45 67
       _mm256_store_si256((__m256i*)&localndx[i],shortx);  // copy the indexes - overfetch OK
      }
      // Set zv for the new batch of columns, plus the last one of the old pipe if any
      if(!(zv&ZVCOLCTVALID)){
       // cycle 0 - happens only at initialization of the first reservation.  colxm2 fetched garbage, and we have to wait for the first column value to be ready
       zv=zv+(ZVCOLCTVALID+ZVCOLCTDECR+(ressize<<ZVCOLCTX));  // starting col index=0 (in localndx), and arrange for 'valid' to be set for the # indexes we have.
       colxm2=localndx[0];  // start with the first column of the first reservation.  This waits for the reads to complete first time
      }else{
       // cycle 1..0 - normal chained reservation.  Leave pipe and colxm2 for the previous reservation, and set pointer/counters for the new.  This puns ZVCOLCTVALID, which is set for the new resv but used for the old; both 1, fortunately
       zv=(zv&~(ZVNDAX|ZVCOLCT))+(-((I)1<<ZVNDAXX)+ZVCOLCTVALID+(ressize<<ZVCOLCTX));  // starting col index=-1 (in localndx), and arrange for 'valid' to be set for the # indexes we have+1
      }
      nextcol+=ressize;  // advance nextcol to end+1 of reservation.  If this is past the end of input it will put us into termination
     }
    }else if(!(zv&ZVCOLCTVALID)){
     // pipe is draining to empty; we overfetched, meaning further pipe entries are invalid: perform a harmless simulation of stage -2
     cutrowm1=nqkrows; cutsumsqm1=0.0;  // this suffices to initialize stage -1 to bypass most processing.  frowm1 has a previous valid value.  colxm1 is garbage
     goto endpipem2;  // skip the fetches for stage -2 
    }
     // if we didn't take a reservation nextcol will stay set high and the pipeline will empty
   }

   // Pipe stage -2: read colx, Ax, Frow, RVT.
   colxm1=colxm2; frowm1=Frow[colxm2]; cutrowm1=cutoffstatus[colxm2][0]; cutsumsqm1=cutoffstatus[colxm2][1];  // other needed info
   colrvtm1=(rvtv[colxm2>>(LGBB-1)]>>((colxm2&((1LL<<(LGBB-1))-1))<<1))&((1LL<<2)-1);  // bound/enforcing flag for column
   colxm2=colxm2<nqkbcols?nqkbcols:colxm2; anm1=axv[colxm2][1]; axm1=axv[colxm2][0];  // if slack column, read from harmless decision vbl 0
endpipem2: ;  // come here in runout where pipe input is invalid
   // end pipe stage -2

   // advance the pipe, possibly with validity set for the incoming stage -2 data
   zv=(zv&~ZVPIPE)|((zv<<1)&ZVPIPE);  // advance the pipe, using current value of ndx validity

   // Pipe input stage: advance column index
   zv+=ZVNDAXINCR+ZVCOLCTDECR;  // advance colx index, decr valid col count, whose MSB is ZVCOLCTVALID, input to the pipe
   // end pipe input stage

  }while(!(zv&ZVPIPE1));  // loop till there is a full column's info to process
  // the pipe status after the advance indicates validity AFTER the next execution of a column; that is, PIPE0 is set if we
  // have started the loads in -1 that will advance to 0 during the next execution.  PIPE1 is set if ppp is ready to execute right now, i. e. PIPE0 was set before the pipe advanced.  We wait until we can execute right now.

  // end of initialization pipeline. start the next column, pointed to by ppp

  I an=ppp->an;  // # weights-1 (-1 if Slack col, -2 if column finished already, -3 if aborted already)
  D accumsumsq=ppp->initsumsq;  // total column norm so far
  I rowx=ppp->initrowct;  // the row number of Qk we are working on.  We stop when rowx=rown.  Always on a 32-row boundary
  rowx=rowx>=nqkrows?nqkrows:rowx;  // prevent rows from incrementing too many times
  I rowx2=rowx<<1; I nqkrows2=nqkrows<<1;   // for QP gradient, we process the low parts and the high, so double index and end position
  ndotprods-=rowx2*(an+1-REPSGN(an));  // treat (0,start] as a gap.  Count #fetches: an+1 if an>=0, an+2 if an=-1, immaterial if an<-1 (will be undone at end).  Must divide by 2 at end since 2 words per fetch

  // We calculated a cutoff point in pipe stage -1, using it to decide whether to load the Am/Av values.  sharedmin has since been updated, and might give a stricter cutoff.
  D cutoffsumsq=sharedmin*sharedminslack*ppp->frowsq;   // Frow^2 * best sumsq / best Frow^2, which is cutoff point for sumsq in new column (Frow^2)/sumsq > bestFrow^2/bestsumsq)
  // We do not use the new cutoff right away to set colx, because it has latency and the chance it has changed enough to cut off this column at the start is quite small

  D **mv0, *vv0;  // pointer to first column pointer, pointer to first weight
  // convert the column#s to Qkt row addresses, in place
  if(an>=0){I avx;
   if(likely(zv&ZVSHARINGMIN))*(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // fetch sharedmin, which we will use AFTER we process this column.  Do it early
   mv0=(D**)&ppp->mvvv[0]; vv0=(D*)&ppp->mvvv[mvvvtoalloc];  // the column will use addrs/weights out of ppp.  Get the starting addresses
   __m256i qkt0_4=_mm256_set1_epi64x((I)qkt0), qktrowstride_4=_mm256_set1_epi32(qktrowstride<<LGSZD);
   NOUNROLL for(avx=0;avx<=an;avx+=NPAR){  // for each block of input... (this overfetches but that's OK since we map enough for one 32-byte final fetch)
    _mm256_store_si256((__m256i*)&mv0[avx],_mm256_add_epi64(qkt0_4,_mm256_mul_epu32(_mm256_load_si256((__m256i*)&mv0[avx]),qktrowstride_4)));  // row#*row size + base of Qkt, for each row.  There may be garbage at the end
   }
   if(unlikely(an==0)){mv0[1]=mv0[0]; vv0[1]=0.0; an=1;}  // if only 1 weight (should not occur), add a weight of 0 to save testing in the loop
  }else if(an==-1){  // Slack variable.  Set flag length and repurpose nextmv to point to the sole row of Qkt.  Also refetch sharedmin
    if(likely(zv&ZVSHARINGMIN))*(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE); vv0=&qkt0[qktrowstride*ppp->colx];  // leave an=-1
  }else{if(an==-2)goto usefullrowtotal; goto abortcol;  // immediate termination/abort.  DO NOT fetch sharedmin, as we will be checking it presently.  rowx2, accumsumsq needed
  }
if((rowx2&(2*BNDROWBATCH/1-1))!=0)SEGFAULT;  // scaf  if just part of a col, it must leave us on a bndrowmask bdy

  __m256d accnorm4=_mm256_setzero_pd();  // accumulator for sumsq, in short bursts

  // Beat me, Daddy, 32 to the bar

  __m256i bndmsk;  // bitstream for each lane, giving (bound) info for the row.  bigendian!!
  while(rowx2<(nqkrows2)){  // for each row in the column, until we cut off

   // if this starts a boundmask block, fetch the mask for it
   if((rowx2&(2*BNDROWBATCH/1-1))==0){
    // move the 64 bits to  the correct position in each lane.  The 64 bits are (LSB) lane0 | lane1 | lane2 | lane3 (MSB)
    // The bits for each lane are in bigendian order so that they come out at the sign bit.  We then shift them to get
    // x,lane0 | x,lane2 || x,lane0 | x,lane2 || x,lane1 | x,lane3 || x,lane1 | x,lane3 ||  which is the order we use them in
    bndmsk=_mm256_sllv_epi64(_mm256_set1_epi64x(*(I*)&bndrowmask[rowx2>>((LGBNDROWBATCH-0)+1)]),bndmskshift);  // +1 to divide rowx2
   }

   // calculate several blocks of the dot-product
   // idea: reorder columns to use lines cached by previous columns
   // Main accumulation loop.  We do not pipeline mv because it looks like we are limited by instruction issue
   __m256d avweight,acc0,acc1,acc2,acc3,acc4,acc5,acc6,acc7;  // fma latency is 4 cycles; 2 launches/cycle; need 8 accumulators
#undef GRLD
#undef GRMUL
#undef GRFMA
   #define GRLD(accno,type) acc##accno=_mm256_castsi256_pd(_mm256_##type##_si256((__m256i*)&vv0[rowx2+NPAR*accno]));
   #define GRMUL(accno,type) acc##accno=_mm256_mul_pd(avweight,_mm256_castsi256_pd(_mm256_##type##_si256((__m256i*)&mv[rowx2+NPAR*accno])));
   #define GRFMA(accno,type) acc##accno=_mm256_fmadd_pd(avweight,_mm256_castsi256_pd(_mm256_##type##_si256((__m256i*)&mv[rowx2+NPAR*accno])),acc##accno);
   if(rowx2<=(zv&ZVMAXROWS)){
    // for the first rows of each column of Qk, allow the values to come into the caches, where they might be reused
#undef GRLLD
#undef GRLMUL
#undef GRLFMA
    #define GRLMUL(accno) GRMUL(accno,load)
    #define GRLFMA(accno) GRFMA(accno,load)
    #define GRLLD(accno) GRLD(accno,load)
    if(an>=0){D *mv; UI mx=an;  // decision vbl.  Count mx down from #weights-1
     mv=mv0[mx]; avweight=_mm256_set1_pd(vv0[mx]); GRLMUL(0) GRLMUL(1) GRLMUL(2) GRLMUL(3) GRLMUL(4) GRLMUL(5) GRLMUL(6) GRLMUL(7)   // initialize accumulators with first product
     // How much to unroll this loop is a hard question.  There are 5 non-FMA instructions per loop, of which 1 is eliminable MOVs.  Estimating the average column to have 5 weights and cutoff in 256 rows,
     // there will be 40 FMAs, 40 LOAD, 25 overhead per batch, plus the batch processing of 40 inst.  These would run in 38 cycles and would be limited by the instruction launch.
     // By unrolling the loop we can cut the non-FMA to 2.5 inst/loop, total 13/batch, at the cost of 1 misbranch (~60 inst).
     // BUT: data will usually be coming from D2$ and will not be available at max rate.  If the actual rate is 200/320 of max rate, the 40 LOAD will limit to 32 cycles/batch, which is not limiting for the workload described.
     // However, if bound-row processing can be eliminated (by storing a version premultiplied by sqrt(2)), the batch processing will fall to 16 inst, which will launch faster than the LOADs can run.
     // Summary: it's a close question (in 2023) and we go for simplicity, because that will be best if bound-row is removed.
     do{mv=mv0[mx-1]; avweight=_mm256_set1_pd(vv0[mx-1]); GRLFMA(0) GRLFMA(1) GRLFMA(2) GRLFMA(3) GRLFMA(4) GRLFMA(5) GRLFMA(6) GRLFMA(7)}while(--mx);
    }else{  // slack vbl
     GRLLD(0) GRLLD(1) GRLLD(2) GRLLD(3) GRLLD(4) GRLLD(5) GRLLD(6) GRLLD(7)  // load column directly
    }
   }else{
    // later rows in the column are unlikely to be reused (the column will have cut off); don't let them evict more valuable early rows
#undef GRSLD
#undef GRSMUL
#undef GRSFMA
    #define GRSMUL(accno) GRMUL(accno,stream_load)
    #define GRSFMA(accno) GRFMA(accno,stream_load)
    #define GRSLD(accno) GRLD(accno,stream_load)
    if(an>=0){D *mv; UI mx=an;  // decision vbl
     mv=mv0[mx]; avweight=_mm256_set1_pd(vv0[mx]); GRSMUL(0) GRSMUL(1) GRSMUL(2) GRSMUL(3) GRSMUL(4) GRSMUL(5) GRSMUL(6) GRSMUL(7)   // initialize accumulators with first product
     do{mv=mv0[mx-1]; avweight=_mm256_set1_pd(vv0[mx-1]); GRSFMA(0) GRSFMA(1) GRSFMA(2) GRSFMA(3) GRSFMA(4) GRSFMA(5) GRSFMA(6) GRSFMA(7)}while(--mx);
    }else{  // slack vbl
     GRSLD(0) GRSLD(1) GRSLD(2) GRSLD(3) GRSLD(4) GRSLD(5) GRSLD(6) GRSLD(7)  // load column directly
    }
   }

   rowx2+=8*NPAR;  // advance input row number
   if(unlikely(rowx2>nqkrows2)){   // if we have to discard part of the last batch... (Note that we bypass this if the last batch is precisely filled without Fk)
    // if we make it to the end without cutting off, we have to zero the last value, which is Fk/ck rather than a real column value, and all later values which may be fetches off the end
    __m256d oddmask=_mm256_loadu_pd((double*)(validitymask+2));  // mask to clear odd pair of an accum
#undef GRCLR
    #define GRCLR(accno) case accno*2: acc##accno=_mm256_setzero_pd(); if(0){case accno*2+1: acc##accno=_mm256_and_pd(acc##accno,oddmask);}
    switch((nqkrows2>>1)&(16-1)){GRCLR(0) GRCLR(1) GRCLR(2) GRCLR(3) GRCLR(4) GRCLR(5) GRCLR(6) GRCLR(7)}
   }

   // combine the blocks.   if row is bound, multiply its value by sqrt(2).  Then accumulate value^2 into accnorm4.  This will complete while the next burst is being calculated
   // idea: store bound rows with Qk values multiplied by sqrt(2).  Must undo for onecol, and must take into account during pivot
   // better idea: have single-precision version of Qkt, used only for gradients, with bound rows already multiplied by sqrt(2)
   // bndmsk is (LSL) x,lane0 | x,lane2 || x,lane0 | x,lane2 || x,lane1 | x,lane3 || x,lane1 | x,lane3 (MSL) ||  
   // we exchange 32-bit lanes to put 0 first and interleave 0/2 thereafter; after the operation we shift the odd lanes to advance to the next bit
   #define GRBNDSH0 bndmsk=_mm256_shuffle_epi32(bndmsk,0b10110001);
   #define GRBNDSH1 bndmsk=_mm256_blend_epi32(bndmsk,_mm256_slli_epi64(bndmsk,1),0b10101010);
   #define GRSQs(accno) GRBNDSH0 acc##accno=_mm256_blendv_pd(acc##accno,_mm256_mul_pd(acc##accno,sqrt2),_mm256_castsi256_pd(bndmsk)); acc##accno=_mm256_mul_pd(acc##accno,acc##accno); GRBNDSH1 /* sq, or 2*sq */
   #define GRSQACCs(accno,addend) GRBNDSH0 acc##accno=_mm256_blendv_pd(acc##accno,_mm256_mul_pd(acc##accno,sqrt2),_mm256_castsi256_pd(bndmsk)); acc##accno=_mm256_fmadd_pd(acc##accno,acc##accno,addend); GRBNDSH1 /* sq, or 2*sq */
   GRSQACCs(0,accnorm4) GRSQs(1) GRSQs(2) GRSQs(3) GRSQACCs(4,acc0) GRSQACCs(5,acc1) GRSQACCs(6,acc2) GRSQACCs(7,acc3)  //  accnorm4+all accs into 4 acc4s

   acc5=_mm256_add_pd(acc4,acc5); acc7=_mm256_add_pd(acc6,acc7); accnorm4=_mm256_add_pd(acc5,acc7);  // accnorm4+all accs into 1 acc4.

   // if gradient total cuts off, abort the column.  We have to collect the total across all lanes
   // this test is long enough that we don't want to do it on every loop.  The best frequency is
   // sqrt(2 * (expected # loops) * (cost of test) / (cost of loop body))
   // mitigating factors are (1) computation is not lost if the column isn't modified by the pivot
   // (2) if the frequency is less than every 16 tests the branch will mispredict
   // Currently the body is 5 cycles/nonzero in A0 + 36, test is 8; expected #loops is, say, 200/32=7.  Best freq is
   // sqrt(2 * 7 * 8 / 61) = a little over 1.  We use 2.
   if((rowx2&(2*CMPBATCH-1))==0){  // when we have moved to the START of the next compare batch, so that if we cut off we will be on a bndrow mask bdy
    // This block has no chained dependency - it just checks for cutoff
    accnorm4=_mm256_add_pd(_mm256_permute_pd(accnorm4,0b0101),accnorm4); accnorm4=_mm256_add_pd(_mm256_permute4x64_pd(accnorm4,0b01001110),accnorm4);   // combine into one
    accumsumsq+=_mm256_cvtsd_f64(accnorm4); accnorm4=_mm256_setzero_pd();  // transfer total to accumsumsq and reset
    if(unlikely(accumsumsq>cutoffsumsq)){  // cutoff if Frow^2/sumsq<(best Frow^2/sumsq) => sumsq>(best sumsq)*(Frow/best Frow)^2  we save min of (best sumsq)/(best Frow^2) which is max of FOM
     goto abortcol;   // rowx2 and accumsumsq must be set
    }
   }
  }
  // done with loop down one column.  Here the column ran to completion

  // End-of-column processing.  Collect stats and update parms for the next column
  // we are sharing the gradient, looking for the smallest (we currently have sumsq of columns)

  // calculate 1/gradient^2 for the column, which is positive and we want to minimize (to maximize gradient).  It is sumsq/Frow^2
  accnorm4=_mm256_add_pd(_mm256_permute_pd(accnorm4,0b0101),accnorm4); accnorm4=_mm256_add_pd(_mm256_permute4x64_pd(accnorm4,0b01001110),accnorm4);   // total sumsq
  accumsumsq+=_mm256_cvtsd_f64(accnorm4);  // final total of column normsq
usefullrowtotal:;  // come here when we started knowing the row total, in accumsumsq
  D recipgradsq=accumsumsq/ppp->frowsq;  // norm^2/Frow^2 = 1/gradient^2
  I valuetoshareminof=*(I*)&recipgradsq;  // share the recip gradient, looking for mimimum.  We have the gradient that was completed for this column
       // because atomics run int-only, this is the bit-pattern that we will minimize as an I
  // if this value is a new minimum, share it with the other cores (might not be if last batch went overlimit but was not tested for cutoff)
  // If the new value is equal to the old, there is no need to share it
  if(unlikely(valuetoshareminof<*(I*)&sharedmin) && (zv&ZVSHARINGMIN)){
   // the new value is lower than the lowest we know about.  Share it
   I incumbentimpi=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // load incumbent best improvement
   while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
    if(valuetoshareminof>=incumbentimpi)break;  // if not new global best, don't update global.  In this case we don't need to remember the value of this column, since it has been beaten
    if(__atomic_compare_exchange_n(&ctx->sharedmin.I,&incumbentimpi,valuetoshareminof,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){
     minvalueweshared=valuetoshareminof;  // wait till now to remember best-in-thread.  There is no need to remember a value that we didn't share
     retinfo=ppp->colx;  // save the col where we found the shared value
     incumbentimpi=valuetoshareminof;  // our value has been written
     break;
    }  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
   }
   *(I*)&sharedmin=incumbentimpi;  // since we went to the trouble of reading current sharedmin, remember what it is
  }

  // we have finished processing the column, and reporting its gradient if it was an improvement.  Incr stats and save column cutoff info
  I impcolincr;  // value we will add to the col-stats field
  //  ***** jump to here on aborted column *****   rowx2 must point to the batch after the last one we processed and must be on a BNDROW bdy
  if(1)impcolincr=0x100000001; else abortcol: impcolincr=1; // jump here if column aborted early, possibly on insufficient gain.  This is the normal path if abort, don't incr # completed columns
  ndotprods+=rowx2*(an+1-REPSGN(an)); nimpandcols+=impcolincr; // accumulate # products performed, including the one we aborted out of; and 1 column, 0/1 improvements
  // save the total we accumulated.
if(rowx2<nqkrows2 && (rowx2&(2*BNDROWBATCH/1-1))!=0)SEGFAULT;  // scaf
  rowx2=rowx2>nqkrows2?nqkrows2:rowx2;  // If rowx>nrows, back it up so accounting of reused products is accurate
  D Drowx2=rowx2>>1;   // convert from D to E index
  __atomic_store((I*)&cutoffstatus[ppp->colx][0],(I*)&Drowx2,__ATOMIC_RELEASE);  // only integers can be atomic args
  __atomic_store((I*)&cutoffstatus[ppp->colx][1],(I*)&accumsumsq,__ATOMIC_RELEASE);
 }while(zv&(ZVPIPE0|ZVPIPEm1));  // end of column loop.  Stop if the pipe is empty after this column

 // No more columns to do.
 // return result to the ctx, if we have the winning value.  Simple writes to ctx do not require RELEASE order because the end-of-job code
 // and the threads sync through ctx->internal.nf; but they should be atomic in case we run on a small-word machine

 ndotprods>>=1;   // convert # dotproducts from 1 per fetch to 1/2 per fetch
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL); __atomic_fetch_add(&ctx->nimpandcols,nimpandcols,__ATOMIC_ACQ_REL); // accumulate stats for the work done here: dot-products, improvements, columns
 // if the value we reported out might be the best, store its completion info out if it is actually the best.
 // For the nonce, we hope the thread ordering gives enough randomization for tied zero SPRs
 if(minvalueweshared<=*(I*)&sharedmin){  // don't bother if we know another thread is better
  I incumbentimpi=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // update incumbent best value
  if(minvalueweshared==incumbentimpi){  // check outside of lock for perf
   WRITELOCK(ctx->ctxlock)
   if(minvalueweshared==ctx->sharedmin.I){ctx->retinfo=retinfo;} // if we still have the best gain, report where we found it
   WRITEUNLOCK(ctx->ctxlock)
  }
 }
 R 0;
}


 // onecol+SPR
#undef ZVNEGATECOLX
#undef ZVNEGATECOL
#define ZVNEGATECOLX 0  // set if the result of the col-value calculation must be negated (for Enforcing vars)
#define ZVNEGATECOL (1LL<<ZVNEGATECOLX)  // set if the current column was an improvement
#undef ZVPOSCVFOUNDX
#undef ZVPOSCVFOUND
#define ZVSPR0X 1  // set if we have hit SPR of 0 and do not need to compute them any more
#define ZVSPR0 (1LL<<ZVSPR0X)
#define ZVPOSCVFOUNDX 2  // set if we are looking for improvement and want to abort when SPR=0
#define ZVPOSCVFOUND (1LL<<ZVPOSCVFOUNDX)
#define remflgs(zv) ((D*)((I)(zv)&~0x7))

#define ALMOSTINF 1e300  // A very large value, used when we have seen a positive col value but have no SPR to report

// we estimate processing 4 rows of QkA0 in 5 qp multiply-adds, each taking about 30 fp instructions - total 75 clocks at 2 launches per clock - plus time to write and do SPR, another
// ~10 clocks.  That's maybe 16ns.  A RFO takes 15-30 ns.  We will take the next reservation with 12 rows left in the current one
#define ROWSPERRESV 12   // number of rows we can process during one atomic_fetch_add

// the processing loop for one core, performing column fetch+SPR.  We take a slice of the rows, repeatedly
// ti is the job#, not used except to detect error
static unsigned char jtmvmsparseesprx(J jt,struct mvmctx *ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YCUT(T,x) T x=(T)__atomic_load_n(&ctx->u.spr.x,__ATOMIC_ACQUIRE);
#define YCU(x) YCUT(typeof(ctx->u.spr.x),x)
#define YCT(T,x) T x=(T)__atomic_load_n(&ctx->x,__ATOMIC_ACQUIRE);
#define YC(x) YCT(typeof(ctx->x),x)
 YCUT(I,colx)YCUT(E*,zv)YCT(I,nqkbcols)YCU(axv)YC(amv0)YC(avv0)YC(qkt0)YCT(I4,qktrowstride)YC(nbasiswfk)YC(ressize)YCUT(I,zstride)YCU(parms)YCUT(E*,bk)
 YC(bndrowmask)YCU(bkbeta)YCU(ressize2)
#undef YC
#undef YCT
#undef YCU
#undef YCUT
 // perform the operation
 I ndotprods=0;  // number of dot-products we perform here
 I frowbatchx;  // rowx of batch containing Fk, if any
 __m256i bndmskshift=_mm256_set_epi64x(0, 32, 16, 48);  // bndmsk is littleendian, but reversed in 16-bit sections.  last section goes to lane 3, i. e. no shift.  Process in 0213 order
 __m256d store0thresh=_mm256_set1_pd(parms[3]);  // In one-column mode, this holds the Store0Threshold: column values less than this are set to 0 when written out
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);

 // Initial zv has the NEGATECOL flag set if needed, others clear

 // Bring in the row address/weights for this column.  Slow if in D3$.  If we take a reservation next, that won't matter; start early in case 1 core
 D **mv0, *vv0;   // place to hold the addresses/weights of the columns being combined
 I an;  // number of weights in this dot-product
 if(colx>=nqkbcols){I avx;  // index of Am/Av block being moved
  // decision variable.  convert the column indexes into pointers into Qkt, and interleave them with the values
  an=axv[colx][1]; I wtofst=axv[colx][0];   // number of weights in column, offset to them
  I ntoalloc=(an+1+NPAR-1)&-NPAR;  // block that can hold all the pointers/weights
  A wts; GATV0(wts,INT,2*ntoalloc,1)  mv0=voidAV1(wts); vv0=(D*)(mv0+ntoalloc);   // place to hold the addresses/weights of the columns being combined
  I *amv=amv0+wtofst; D *avv=avv0+wtofst;  // number of sparse atoms in each row; pointer to first col#; pointer to first weight
  __m256i qkt0_4=_mm256_set1_epi64x((I)qkt0), qkrowstride_4=_mm256_set1_epi32(qktrowstride<<LGSZD);
  for(avx=0;avx<an;avx+=NPAR){  // for each block of input... (this overfetches but that's OK since we map enough for one 32-byte final fetch)
   __m256i mv4=_mm256_loadu_si256((__m256i*)&amv[avx]);   // read next group of 4 row#s
   __m256d av4=_mm256_loadu_pd(&avv[avx]);   // read next group of 4 values
   mv4=_mm256_add_epi64(qkt0_4,_mm256_mul_epu32(mv4,qkrowstride_4));  // row#*row size + base of Qkt, for each row.  There may be garbage at the end
   // We have 4 row-pointers and 4 values.  Write them out
   _mm256_storeu_si256((__m256i*)&mv0[avx],mv4); _mm256_storeu_pd((double*)&vv0[avx],av4);
  }
 }else{an=-1; vv0=(D*)&qkt0[qktrowstride*colx];}  // Slack variable.  Skip the weighting, leave an neg as flag.  Repurpose vv0 to point to col

 //   parms is #cols,maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,[MinSPR],[PriRow]
 __m256d col0thresh=_mm256_set1_pd(parms[2]);  //  minimum value considered valid for a column value (smaller are considered 0)
 __m256d bkovershoot=_mm256_set1_pd(parms[9]);  //  maximum amount a pivot is allowed to make bk overshoot
 __m256d bk0thresh=_mm256_set1_pd(parms[8]);  // smallest bk value considered nonzero
 I prirow=(I)parms[11];  //  priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so.  Set to LOW_VALUE if no priority row
 I nqkrows=ABS(nbasiswfk);  // number of rows of qk including any Fk
 I nhasfk=REPSGN(nbasiswfk); // -(Fk is an extra row)
 frowbatchx=(nqkrows-1)&-NPAR; frowbatchx|=~nhasfk;  // batch# containing last row (might be by itself); if there is no fk, batch is ~0, i. e. never
 __m256d sharedspr;  // the minimum across lanes shared, to reduce minimum sightings
 I limitrowx=IMAX;  // for SPR, the row of the current lowest SPR.   init row found to high-value, so that we don't share a value unless we actually hit it
 __m256d bndmsk;  // bitstream for each lane, giving (bound) info for the row.  bigendian!!
 D nzerobatches;  // number of batches with 0 SPR we have encountered


 I rowx=ressize*ti;  // the row number we are working on.  We stop when rowx=rown.  Always on an NPAR-row boundary
 I rown=rowx+ressize;  // index of last+1 row in current (or only) section of the column.  If there is a remnant, the value is backed up by 4.  If 0, we are processing the remnant
 I newrowx=rown;  // if we don't get a reservation, for termination purposes use the end+1 of the current one.  The only time we don't resv is when this is past the end
 I needresvx=(rown-ROWSPERRESV)&-NPAR; needresvx=needresvx<rowx?rowx:needresvx;   // point at which we need new resv.  Might be off the end, if we never need one
 rown=rown>nqkrows?nqkrows:rown; rown-=rown&(NPAR-1)?NPAR:0;  // clamp to end of column, and back up end if there is a remnant

 __m256d endmask=_mm256_setone_pd(); // mask for validity of next 4 words: used to control fetch from column and store into result row
 // The init SPR is inf for normal columns; but for bound column must init to beta, the SPR of the implied row
 // This was set in the initial value in the ctx, but it may have been reduced by other threads before we get to look at it
 D initspr=ctx->sharedmin.D;
 // If another thread has finished with 0 SPR, or has aborted, sharedmin has changed meaning to be -1/max column value found, or IMIN (=-0) for abort.
 // In that case we mustn't interpret the value as a positive SPR.  We abort if IMIN, because that might save a lot of work when another thread has aborted.
 // For 0 SPR, we indicate that we are looking for 0 SPR, but we don't try to start at the same value as the other thread, because owing to numerical error
 // we might end up thinking we had the winning 0 SPR with no idea what row it came from.  So we just set the max-col-value to a harmless invalid 0, which will
 // never be the smallest 0-SPR
 if(unlikely(*(I*)&initspr<0)){if(*(I*)&initspr==IMIN)R 0; initspr=0.; zv=(E*)((I)zv|ZVSPR0);}  // if remote abort, abort w/o reply; if 0-SPR, set up for that
 sharedspr=_mm256_set1_pd(initspr);

 // create the column NPAR values at a time
 // loop for each row, in groups of NPAR
 while(1){

  // Establish what batch of NPAR lines we should work on.  This block handles end-of-column/end-of-section
  if(unlikely(rowx>=rown)){   // if there are not a full 4 values to process
   // not 4 values.  If there is no remnant, we are through with the reservation for this column
   rown=rown==0?rowx:rown;  // rown=0 means we are processing the remnant; signal that it is finished
   if(rowx==rown){  // no unprocessed remnant?
    // Use the previously-read row reservation
    if(newrowx>=nqkrows)break;  // if there is no new reservation, exit.  This is the loop exit
    // We are going to take a new reservation.  Deduct the size of the gap from the # products to date
    ndotprods+=rowx-newrowx;  // we remove (new rowx-old rowx), i. e. the amount skipped, from the count of rows processed
    rowx=newrowx;   // start new reservation
    rown=rowx+ressize2; newrowx=nqkrows; needresvx=(rown-ROWSPERRESV)&-NPAR; needresvx=needresvx<rowx?rowx:needresvx; needresvx=rown>=nqkrows?nqkrows:needresvx;  // set up for reading next reservation; if we have the last resv, suppress taking another
    rown=rown>=nqkrows?nqkrows:rown; rown-=rown&(NPAR-1)?NPAR:0;  // clamp to end of column, and back up end if there is a remnant
   }
   if(rowx>rown){  // if there is a remnant (which could be the first batch of a new reservation), process it and make sure we finish next time
    // there is a remnant to process.  Get the endmask for it
    endmask=_mm256_permute4x64_pd(_mm256_loadu_pd((double*)(validitymask+(rowx-rown))),0b11011000);   // (curr-(end-4)) = 4-(end-curr) = 4-#values left is the correct mask, with (#values) ~0, permuted to 0213 order
    
    rown=0;  // set flag value to indicate we are processing a remnant and endmask is set
   }
  }

  // When we get close enough to the end of the reservation, establish the next one.  This takes a RFO cycle
  if(unlikely(rowx==needresvx))newrowx=__atomic_fetch_add(&ctx->nextresv,ressize2,__ATOMIC_ACQ_REL);   // reserve the next block

  __m256d dotproducth,dotproductl;  // where we build the column value

  // if this starts a bound/enforcing block, fetch the mask for it
  if((rowx&(BNDROWBATCH/1-1))==0){
   // move the 64 bits to  the correct position in each lane
   bndmsk=_mm256_castsi256_pd(_mm256_sllv_epi64(_mm256_set1_epi64x(*(I*)&bndrowmask[rowx>>(LGBNDROWBATCH-0)]),bndmskshift));
  }

  // Do the extended-precision dot-product
  __m256d accmaxabs;  //  max magnitude of the inputs to the accumulation
  if(an>=0){  // decision variable, which requires weights
   // It's not unusual for all 4 values fetched from Qk to be 0. 
   // For the first value, we can simplify the product with no accumulate
   I mx;   // row index (of the nonzero components of this A0 col)
   NOUNROLL for(mx=0;mx<an;++mx){  // for each element of the dot-product
    __m256d tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
    E *mv=(E*)mv0[mx];  // advance the pipeline of row addresses in Qkt.  We don't unroll because we have a great many dependent instructions to release
    // get column number to fetch; fetch 4 rows
    __m256d h0l0h1l1=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&mv[rowx])), h2l2h3l3=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&mv[rowx]+1));  // batch of Qkt, as Es
    dotproducth=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000);
    if(_mm256_testz_si256(_mm256_castpd_si256(dotproducth),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
    // there's a nonzero, calculate it
    tl=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // batch of Qkt, high & low separated in 0213 order
    vval=_mm256_set1_pd(vv0[mx]);  // load weight for the column
    // take the two products and canonicalize
    TWOPROD(dotproducth,vval,th2,tl2)  // high qk * col
    accmaxabs=_mm256_andnot_pd(sgnbit,dotproducth);   // init upper-significance holder
    tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add to high part
    TWOSUMBS(th2,tl2,dotproducth,dotproductl)  // combine parts, canonicalize
     // this is accurate to 104 bits from the largest of (new,old,new+old)
    goto accumulateqp; 
   }
   // if we end the loop, there must have been nothing but 0.  We know dotproducth was set to 0.
   dotproductl=dotproducth;  // set overall product to 0
   goto endqp;
accumulateqp: ;
   NOUNROLL for(++mx;mx<an;++mx){  // skip the first nonzero; for each remaining element of the dot-product
    __m256d th,tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
    E *mv=(E*)mv0[mx];  // advance the pipeline of row addresses in Qkt
    // get column number to fetch; fetch 4 rows
    __m256d h0l0h1l1=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&mv[rowx])), h2l2h3l3=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&mv[rowx]+1));  // batch of Qkt, as Es
    th=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000);
    if(_mm256_testz_si256(_mm256_castpd_si256(th),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
    tl=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // batch of Qkt, high & low separated in 0213 order
    vval=_mm256_set1_pd(vv0[mx]);  // load weight for the column
    // accumulate the dot-product
    TWOPROD(th,vval,th2,tl2)  // high qk * col
    accmaxabs=_mm256_max_pd(accmaxabs,_mm256_andnot_pd(sgnbit,dotproducth));   // update upper-significance holder
    tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add in extension of prev product
    TWOSUM(dotproducth,th2,dotproducth,vval)  // combine high parts in quad precision
    tl2=_mm256_add_pd(dotproductl,tl2); tl2=_mm256_add_pd(vval,tl2);  // add extensions, 3 products costs 2 bits of precision
    TWOSUM(dotproducth,tl2,dotproducth,dotproductl)  // combine high & extension for final form
    // this is accurate to 104 bits from the largest of (new,old,new+old).
   }
  }else{
   __m256d h0l0h1l1=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&((E*)vv0)[rowx])), h2l2h3l3=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&((E*)vv0)[rowx]+1));  // batch of Qkt, as Es
   dotproducth=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000); dotproductl=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111); accmaxabs=_mm256_setzero_pd();}  // slack vbl, just read column value, no need for noise test
endqp: ;

  // store out the values, setting to 0 if below threshold
  accmaxabs=_mm256_fmadd_pd(accmaxabs,_mm256_set1_pd(RELSIGMAX),store0thresh);  // no reason to keep values that are all noise.  We take 100 bits from max significance, but never less than store threshold
  __m256d okmask=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,dotproducth),accmaxabs,_CMP_GT_OQ);  // 0s where we need to clamp
  if((I)zv&ZVNEGATECOL){dotproducth=_mm256_xor_pd(sgnbit,dotproducth); dotproductl=_mm256_xor_pd(sgnbit,dotproductl);}  // Handle Enforcing column. branch will predict correctly and will seldom need the XOR
  dotproducth=_mm256_and_pd(dotproducth,okmask); dotproductl=_mm256_and_pd(dotproductl,okmask); // set values < threshold to +0, high and low parts
  _mm256_store_pd((D*)((__m256i*)((E*)remflgs(zv)+rowx)),_mm256_shuffle_pd(dotproducth,dotproductl,0b0000)); _mm256_store_pd((D*)(((__m256i*)((E*)remflgs(zv)+rowx))+1),_mm256_shuffle_pd(dotproducth,dotproductl,0b1111));  // write out the value, 0123
if((rowx+NPAR-1)>=zstride)SEGFAULT;  // scaf
  // We treat Fk as part of the column, except that we don't want to include it in the SPR calculation.
  if(unlikely(rowx==frowbatchx)){
   // We hit the Fk batch.  Turn off the last lane of endmask, which must hold Fk
   endmask=_mm256_blend_pd(_mm256_setzero_pd(),_mm256_permute4x64_pd(endmask,0b11011110),0b0111);   // AND in next-higher lane, in 0213 order
   dotproducth=_mm256_and_pd(dotproducth,endmask);  // we expect dotprod 0 in any lane where endmask is 0.  Only high part used for SPR
  }
  // Calculate SPR for column.  process the values in parallel
  // Get the column values c and b values to use.  In bound columns we use abs(c). 
  __m256d bnddotproducth=_mm256_andnot_pd(_mm256_and_pd(bndmsk,sgnbit),dotproducth);  // take abs of col value if bound row
  __m256d cgt0=_mm256_cmp_pd(bnddotproducth,col0thresh,_CMP_GT_OQ);  // mask of valid c-value lanes.  Will always be 0 in lanes where endmask is 0 (must pad Qk with 0)
  if(!_mm256_testz_pd(cgt0,endmask)){  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  skip batch if so
   // there is a positive c value.  process the batch.  Get bk to use
   zv=(E*)((I)zv|ZVPOSCVFOUND);  // The positive col value makes the column bounded
   __m256d bk4, beta4;  // will be ~0 for words that have positive c; the 4 bk values we will use (with overshoot added by caller before we start)
   bk4=_mm256_shuffle_pd(_mm256_loadu_pd((D*)&bk[rowx]),_mm256_loadu_pd((D*)&bk[rowx+2]),0b0000); beta4=_mm256_permute4x64_pd(_mm256_loadu_pd(&bkbeta[rowx]),0b11011000);  // load bk and beta to use  this overfetches the bk/bkbeta areas, which must be padded to batch length including Fk  0123->0213
   bk4=_mm256_blendv_pd(bk4,_mm256_sub_pd(beta4,bk4),_mm256_and_pd(bndmsk,dotproducth));  // b, except beta-b in bound rows with originally negative col
   // if bk < threshold, set it to 0.  If col value negative, mark the column invalid
   __m256d bknon0=_mm256_cmp_pd(bk4,bk0thresh,_CMP_GE_OQ);  // remember which bks are non0
   if(!((I)zv&ZVSPR0)){  // if we have not encountered SPR of 0, sharedspr is the spr
    // calculate the SPR
    bk4=_mm256_and_pd(bk4,bknon0);  // make near0 bk exactly 0
    __m256d ratios=_mm256_fmsub_pd(sharedspr,bnddotproducth,bk4);  // minspr*c-b: sign set if b>minspr*c => b/c>minspr => not new min-or-tied SPR.  minspr, c, and b are all nonneg in valid lanes, so no -0
if(!_mm256_testz_pd(_mm256_or_pd(bk4,_mm256_or_pd(sharedspr,bnddotproducth)),cgt0))ASSERTSYS(0,"neg0");  // scaf verify no -0
    if(!_mm256_testc_pd(ratios,cgt0)){  // CF is 1 if ~(not new min-or-tied)&(valid) is all 0, so 0 if any (min-or-tied)
     // SPR is a new low-or-tied.  If any valid bk is 0, we will be returning a nonimproving pivot
     // by simulation this code runs 4.7 times in 64 batches
     if(_mm256_testc_pd(bknon0,cgt0)){    // if all non0 flags in valid lanes are set, that means the SPR is nonzero 
      // New SPR is nonzero.  Calculate it and remember the new SPR in all lanes
      // We will have to find the location of the minimum value in the new batch, because it may be tied with the current minimum.  To silence invalid lanes
      // we keep the old minspr in them.  Could use infinity, but we might calculate an SPR of inf
      __m256d sprs=_mm256_blendv_pd(sharedspr,_mm256_div_pd(bk4,bnddotproducth),cgt0);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch because it ties up IUs for so many cycles
      // Find the new minimum
      sharedspr=_mm256_min_pd(sprs,_mm256_permute_pd(sprs,0b0101)); // sharedspr = min01 min01 min23 min23  could use integer in AVX-512
      sharedspr=_mm256_min_pd(sharedspr,_mm256_permute4x64_pd(sharedspr,0b00001010));  // sharedspr=min value in all lanes
      // remember the row that had the minimum.  Make sure we take only SPRs for valid col values
      limitrowx=rowx+(3&(0x11231120>>(_mm256_movemask_pd(_mm256_and_pd(cgt0,_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(sharedspr),_mm256_castpd_si256(sprs)))))<<1)));  // index of a lane containing the minimum SPR (there must be one).  0213->0123, giving 1 priority over 2
      // it is just barely possible that the division to calculate SPR gives a zero value even though bk is not 0 (underflow).  Since SPR=0 is a flag value indicating
      // no improvement, we will miss the limitrowx that is set here, and the pivot will be marked dangerous unless a different nondangerous nonimproving pivot
      // is found elsehere.  This can happen only once per thread, because once SPR=0 it is impossible to get low-or-tie unless bk=0.  We ignore the possibility,
      // because the column value would have to be HUGE to underflow to 0.
     }else{sharedspr=_mm256_setzero_pd(); if(parms[10]==0.0){limitrowx=-2; goto return4;} zv=(E*)((I)zv|ZVSPR0); goto startspr0;}   // FIRST 0 SPR: switch modes, change sharedspr to look for max column value; abort if SPR>0 needed; process through 0-SPR path
            // we don't check for column aborts from other threads because the juice isn't worth the squeeze
    }
   }else{
    // we have encountered an earlier SPR of 0.  We don't need to calculate an SPR: we choose the largest column-value where bk is near0 (if it's dangerous we catch it later), and break ties randomly
    // here sharedspr is the largest column value found so far
startspr0:;  // come here for the first 0-SPR batch, after mode switch.  The new column value may be very small but it is always > 0, so we will treat it as a starting batch below
    // take max of sharedspr and new column values where bk is 0.
    __m256d validcol=_mm256_and_pd(bnddotproducth,_mm256_andnot_pd(bknon0,cgt0));   // valid column values where bk is near0, 0 elsewhere.  Could do a single blend but we fear garbage in invalid col value   
    if(!_mm256_testz_pd(_mm256_cmp_pd(validcol,sharedspr,_CMP_GE_OQ),cgt0)){  // Zf if validcol<old max in all valid lanes.  We skip the batch then
     __m256d maxvalidcol=_mm256_max_pd(validcol,_mm256_permute_pd(validcol,0b0101)); maxvalidcol=_mm256_max_pd(maxvalidcol,_mm256_permute4x64_pd(maxvalidcol,0b00001010));  // maxvalidcol= max value in all lanes
     I candlanes=_mm256_movemask_pd(_mm256_cmp_pd(validcol,maxvalidcol,_CMP_EQ_OQ));   // mask of lanes that could be the result
     // Select a lane: if only one, take it, otherwise select a random lane containing the new maximum.  This distribution is not uniform, but that's OK since we are just trying to break cycles
     I randval=-1;  // init to no random value drawn  Low quality is OK.  We leave RNG unseeded so that repeated runs will give different sequences
     I lanehere=rowx+(3&(0xffffa5a0>>((LOWESTBIT(candlanes)|(likely(candlanes==LOWESTBIT(candlanes))?0:(candlanes&(randval=rand()))))<<1)));  // If multiple matches, randomly turn off candidates above the lowest; then find highest survivor   0213->0123
     if(likely(!_mm256_testz_pd(_mm256_cmp_pd(validcol,sharedspr,_CMP_GT_OQ),cgt0))){  // Zf if validcol<=old max in all valid lanes.  If not, we have a new undisputed high
      nzerobatches=1.0; limitrowx=lanehere;  // new maximum batch.  unconditionally take the value and set to 1 in equal group
      sharedspr=_mm256_max_pd(sharedspr,maxvalidcol);  // update shared max value to date
     }else{
      // Tie value with a previous batch.  randomly accept the new value so as to give a uniform distribution of batches
      nzerobatches+=1.0;  // include the new batch in the count of batches encountered
      limitrowx=(double)(randval<0?rand():randval)*nzerobatches<=(double)RAND_MAX?lanehere:limitrowx;  // accept new lane with declining probability
     }
    }
   } 
  } // skip batch if all column values 0

  bndmsk=_mm256_castsi256_pd(_mm256_slli_epi64(_mm256_castpd_si256(bndmsk),1));  // move bound-row mask to the next group of rows.
  rowx+=NPAR;  // advance input row number
 }

 // End-of-column processing.  Collect stats and update parms for the next column
 // we are sharing the SPR, looking for the smallest
 //  limitrowx has the row that the shared value was found in, or IMAX if none found to tie the incumbent

 I valuetoshareminof;   // because atomics run int-only, this is the bit-pattern that we will minimize as an I
 // sharedspr has the value to share.  We might not have achieved it here, though.
 // We recognize the value only if we actually achieved it in this thread, so that we won't report out invalid limitrowx (the issue is that
 // sharedspr may contain the initial value and we would have no valid value to override the default with)
 // If SPR=0, sharedspr has the positive column-value to maximize (it could be dangerous, but never 0).  We take the negative reciprocal, to give us something to minimize in fixed-point. 
 if((I)zv&ZVSPR0)sharedspr=_mm256_div_pd(_mm256_set1_pd(-1.0),sharedspr);  // if 0-SPR, neg-recip.  IEEE754 is sign/magnitude so the sign bit is a flag giving priority to 0-SPR but looking for small recip-column-value
 else if((I)zv&ZVPOSCVFOUND)sharedspr=_mm256_min_pd(_mm256_set1_pd(ALMOSTINF),sharedspr);  // If we found pos col value, noodge SPR to indicate that fact
 valuetoshareminof=_mm256_extract_epi64(_mm256_castpd_si256(sharedspr),0);

 // add to the stats
 I impcolincr;  // value we will add to the col-stats field
 //  rowx must point to the batch after the last one we processed and must be on a BNDROW bdy
 impcolincr=0x100000001;
 // exit if we have processed enough columns (in DIP mode)
 I nimpandcols=__atomic_add_fetch(&ctx->nimpandcols,impcolincr,__ATOMIC_ACQ_REL);  // add on one finished column, and 0 or 1 improvements
 ndotprods+=rowx;  // accumulate # products performed, including the one we aborted out of
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products

// return result to the ctx, if we have the winning value.  Simple writes to ctx do not require RELEASE order because the end-of-job code
// and the threads sync through ctx->internal.nf; but they should be atomic in case we run on a small-word machine

 // if the value we reported out might be the best, store its completion info out if it is actually the best.
 // For the nonce, we hope the thread ordering gives enough randomization for tied zero SPRs
 // We have to write on = so that the row# will be written for zero SPRs where some threads had no nondangerous pivots
 // Don't write if we have no valid row# to write: if no thread has one, the original value will persist.  The danger is that we will
 // start with the winning SPR coming from another thread (or the initial value), and at the end we have no row for it to have come from
 if(limitrowx!=IMAX && valuetoshareminof<=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE)){  // check outside of lock for perf
  WRITELOCK(ctx->ctxlock)
  if(valuetoshareminof<=ctx->sharedmin.I){ctx->sharedmin.I=valuetoshareminof; ctx->retinfo=limitrowx;} // if we still have the best gain, report where we found it
  WRITEUNLOCK(ctx->ctxlock)
 }
 R 0;

// following are the early return points: unbounded, pivot out virtual

return4:;  // we have a preemptive result.  store and set minimp =-inf to cut off all threads
 // the possibilities are unbounded and pivoting out a virtual.  We indicate unbounded by row=-1.  We may overstore another thread's result; that's OK
 // We must do this under lock to make sure all stores to bestcol/row have checked the minimp they use
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 WRITELOCK(ctx->ctxlock)
 ctx->sharedmin.I=IMIN;  // shut off other threads
 ctx->retinfo=limitrowx;  // set unbounded col or garbage, if aborting on 0 SPR
 WRITEUNLOCK(ctx->ctxlock)
 R 0;
}
#endif //C_AVX2 || EMU_AVX2

// 128!:19 matrix times sparse vector with optional early exit
// one column + SPR mode: (qp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
//   Ax,Am,Av are the sparse elements of A0
//   M is Qkt.  There may be padding columns but NOT rows QP
//   RVT is Rose variable types, indicating which columns are (bound,enforced) (littleendian order, with enforced being the LSB)
//   bndrowmask indicates, for each row in Qk, whether the row is Bound
//   bk is the current bk (must include a 0 corresponding to Fk and then MUST BE 0-EXTENDED to a batch boundary) QP
//   z is the result area for the requested column - qp, and must be able to handle overstore to a full batch, including the Fk value if any
//   ndx is the atomic column# of NTT requested
//   parms is #cols,x,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivotImp,ColOkPivotNonImp,Bk0Threshold,BkOvershoot,[MinSPR],[PriRow]
//    #cols is number of columns in the Ek portion of Qk; if Fk present as a row, value is 1s-comp
//    maxAx is the length of the longest dot-product
//    Col0Threshold is minimum |value| considered non0 for a column value
//    Store0Thresh: if |column value| is less than this, force it to 0
//    ColDangerPivot is the smallest allowed pivot, but is dangerous
//    ColOkPivotImp is the smallest improving pivot value considered non-dangerous
//    ColOkPivotNonImp is the smallest improving pivot value considered non-dangerous
//    Bk0Threshold is the smallest bk value considered nonzero
//    BkOvershoot is the maximum excursion into negative bk that we will allow on a pivot
//    MinSPR is the largest SPR that will abort this column (set to __ to allow any SPR, to 0. to require improvement)
//    PriRow is the priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so
//   bkbeta is beta for each row in Qk, i. e. corresponding to bk (including 0 extension)
//   beta is betas for all columns, used to init SPR for bound column
//  if ndx<m, the column is  ndx {"1 M; otherwise ((((ndx-m){Ax) ];.0 Am) {"1 M) +/@:*"1 ((ndx-m){Ax) ];.0 Av
//   multiplied by -1 if the column is enforcing
//  Do the product in quad precision
//  the column data is written to z
//  result is rc,pivotrow,#cols processed,#rows processed,(smallest SPR if >0,neg of largest col value if SPR=0 (nonimproving pivot))
//   rc is 0 (normal), 1 (dangerous), 2 (nonimproving), 3 (nonimproving and dangerous), 4 (preemptive result: SPR was less than MinSPR (SPR=0, row=-2); unbounded (SPR=inf, row=-1) )
// gradient mode: (dp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
//   cutoffinfo is internal stored state saved between gradient-mode calls; shape n,2
//   parms is #cols,maxAx,x,x,MinGradient/MinGradImp,x...
//    MinGradient is normally 0.
//      if positive, we do NOT share gradient values, and we finish up gradient calculation in any column that has a larger |gradient|, storing the len/sumsq in cutoffinfo
//      if negative, cut off a column when its gradient cannot beat the best-so-far by more than a factor of 1/(1-MinGradImp), i. e. MinGradImp=(min OK grad/actual max grad)-1
//   Frow is the selector row QP
//  result is rc,pivotcol or unbounded col,#cols processed,#rows processed,largest |gradient|
//   rc is 0 (normal), 4 (unbounded problem)
//   if MinGradient is given, rc is 0, col is _1, and recip gradient is 0 (all this because no reporting is done)
// findimprovement mode: (dp dotproduct) (not yet implemented)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta  where ndx is a list and z is empty
//   parms is #rows,maxAx,ColOkPivot,x,x,x,x,Bk0Threshold,BkOvershoot,PriRow
//  result is list of columns that have positive SPR (no line with col>ColBk0Threshold && bk<Bk0Threshold)
//  
// Rank is infinite
F1(jtmvmsparsee){F12IP;PROLOG(832);
#if C_AVX2 || EMU_AVX2
 if(AN(w)==0){
  // empty arg just returns stats; none defined yet
  R mtv;
 }
struct mvmctx opctx;  // parms to all threads, and return values

 ASSERT(AR(w)==1,EVRANK);
 ASSERT(AN(w)>=11,EVLENGTH);  // audit overall w
 ASSERT(AT(w)&BOX,EVDOMAIN);
 A box;
 box=C(AAV(w)[9]); ASSERT(AT(box)&FL,EVDOMAIN); ASSERT(AR(box)==1,EVRANK); ASSERT(AN(box)>0,EVLENGTH);  // parm shape, type
 D *parms=DAV(box); I nparms=AN(box); I nandfk=opctx.nbasiswfk=(I)parms[0];   // flagged #rows in Qkt
 box=C(AAV(w)[3]); ASSERT(AR(box)==2,EVRANK) ASSERT(AT(box)&QP,EVDOMAIN) opctx.qkt0=DAV(box); I nbasiscols=AS(box)[0]; // Qkt, possibly including space and Fk; # cols in basis
 opctx.qktrowstride=AS(box)[1]<<1;  // length of allocated row of Qkt in Ds, padded to batch multiple
 opctx.nqkbcols=AS(box)[0];  // number of rows in Qkt which is always # basis columns of Qk
 I ninclfk=ABS(nandfk);   // number of rows to be processed including Fk
 ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"Qkt is not on cacheline bdy")  // we fetch along rows; insist on data alignment
 ASSERTSYS((AS(box)[1]&(NPAR-1))==0,"stride of Qkt is not a cacheline multiple")  // we fetch along rows; insist on data alignment
 box=C(AAV(w)[0]); ASSERT(AR(box)==3&&AS(box)[1]==2&&AS(box)[2]==1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN); I axn=AS(box)[0]; opctx.u.spr.axv=opctx.u.grad.axv=((I(*)[2])IAV(box))-nbasiscols;  // prebiased pointer to A0 part of NTT
 box=C(AAV(w)[1]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN); I amn=AN(box); opctx.amv0=IAV(box);  // col #s in basis
 box=C(AAV(w)[2]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN); I avn=AN(box); opctx.avv0=DAV(box);  // weights
 ASSERT(amn==avn,EVLENGTH);  // weights and col#s must agree
 box=C(AAV(w)[4]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&LIT,EVDOMAIN) C *rvtv=CAV(box);  // RVT
 box=C(AAV(w)[5]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&LIT,EVDOMAIN) opctx.bndrowmask=DAV(box);   // bndrowmask
 box=C(AAV(w)[8]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN)   // col indexes being evaluated
 I isgradmode; I nthreads=(*JT(jt,jobqueues))[0].nthreads+1;   // non0 if gradient mode; ptr to output if any; #threads available for processing
 unsigned char (*actionrtn)(JJ, void *, UI4);  // the routine to do the operation
 if(isgradmode=(AR(box)!=0)){ 
  // gradient mode (the dominant case)
  opctx.u.grad.rvtv=rvtv;
  I nc=opctx.u.grad.nc=AN(box);  ASSERT(AN(w)==11,EVLENGTH);  // audit overall w
  ASSERT(nc!=0,EVLENGTH)
  I *ndxa=opctx.u.grad.ndx0=IAV(box);  // addr of col#s
  DO(nc, ASSERT(BETWEENO(ndxa[i],0,nbasiscols+axn),EVINDEX))// audit col#s, which must be in range 0 to NTT width, i. e. #slacks + #decision vbls
  // box 6 reserved for schedule
  box=C(AAV(w)[7]); ASSERT(AR(box)==2,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.u.grad.cutoffstatus=(D(*)[2])DAV(box);  // saved info from previous scans
  ASSERT(nparms==6,EVLENGTH)  // gradient mode doesn't use much parms
  opctx.u.grad.maxweights=parms[1];  // max # weights in any column (gradient only)
  opctx.u.grad.mingradimp=parms[4];  //   0 normally; >0 => finish up gradient calculation in any column that has a larger |gradient|, storing the len/sumsq in cutoffinfo; <0-> accept any gradient as high as 1/(1-MinGradImp) * true max
  box=C(AAV(w)[10]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.u.grad.Frow=DAV(box);  // the entire selector row
  nthreads=likely(ninclfk*nc>1000)?nthreads:1;  // single-thread small problem
  // We estimate 1 gradient per ns.  Reduce number of threads to leave 2us of work for each thread
  I ressize;  // number of columns for each thread to take initially
  if(unlikely(ninclfk*nc<2000*nthreads)){I nneed=((ninclfk*nc)/2000); nthreads=MIN(nneed,nthreads); nthreads=MAX(1,nthreads);}
  // if the initial reservation is only 1, the thread code will not enter 0..1 state to request another resv; thus in that case nc must=1.  And init resv of 1 is bad anyway, because
  // all the threads will start and nothing will cut off.  So we limit the # threads to half the number of columns to do
  nthreads=MIN(nthreads,nc>>1);  // will ensure init resv of 2 unless nc=1
  if(likely(nc>nthreads*(BIGRESV+SMALLRESV)))ressize=BIGRESV;  // if plenty of cols, take max resv
  else if(nc<SMALLRESV*nthreads)ressize=nc/nthreads;  // not even a SMALL per thread - make initial alloc as big as possible, leaving just a little
  else{ressize=(nc-(nthreads*(SMALLRESV>>1)))/nthreads; ressize=MIN(ressize,BIGRESV);}  // more than a SMALL per thread - leave half a SMALL per thread
  opctx.ressize=ressize; opctx.nextresv=ressize*nthreads;  // set init resv size, and start of first to arbitrate for.  It would be OK for the last thread to have a partial allo, but we don't create that case
  // Switch to SMALL resv when we get close enough to the end.  The pipelined reservations are cheap, but we would like to keep column affinity
  opctx.u.grad.resredwarn=BIGRESV+nthreads*SMALLRESV;  // switch after the last BIGRESV that leaves enough room for each thread to take 1 more resv
  opctx.sharedmin.D=inf; // minimum improvement found.  Must be valid float so we can compute cutoff value
  opctx.retinfo=-1;  // Init col#, should never be survive unless no column ran to completion
  actionrtn=(unsigned char (*)(JJ, void *, UI4))jtmvmsparseegradx;
 }else{
  // onecol+SPR mode
  I colx=opctx.u.spr.colx=IAV(box)[0];  // there is only one col
  ASSERT(BETWEENO(colx,0,nbasiscols+axn),EVINDEX)  // verify in range
  ASSERT(AN(w)==12,EVLENGTH);  // audit overall w
  opctx.u.spr.parms=parms;
  box=C(AAV(w)[6]); ASSERT(AR(box)==1,EVRANK) ASSERT(AS(box)[AR(box)-1]>=((ninclfk+NPAR-1)&-NPAR),EVLENGTH) ASSERT(AT(box)&QP,EVDOMAIN) opctx.u.spr.bk=DAV(box);  // bk values allow overfetch; we use only high part.  Ensure big enough for pad
  I bkzstride=AS(box)[AR(box)-1];  // all the strides must be equal; we have just verified they are big enough
  ASSERTSYS((bkzstride&(NPAR-1))==0,"stride of bk is not a cacheline multiple")  // every row must be aligned
  ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"bk is not on cacheline bdy")  // we fetch along rows; insist on data alignment
  box=C(AAV(w)[7]); ASSERT(AR(box)==1,EVRANK) ASSERT(AS(box)[0]==bkzstride,EVLENGTH) ASSERT(AT(box)&QP,EVDOMAIN)  // result area, stored in place
  ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"column result is not on cacheline bdy")  // we store along rows; insist on data alignment
  // get column info, a 2-bit field of (bound,enforcing)
  I colrvt=(rvtv[colx>>(LGBB-1)]>>((colx&((1LL<<(LGBB-1))-1))<<1))&((1LL<<2)-1);
  opctx.u.spr.zv=(D*)(((I)DAV(box)&~7)|((colrvt&ZVNEGATECOL)<<ZVNEGATECOLX));  // flags: enforcing column requiring negation, abort on nonimproving row
  ASSERT(BETWEENC(nparms,10,12),EVLENGTH)  // SPR has lots of parms
  box=C(AAV(w)[10]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) ASSERT(AS(box)[0]==bkzstride,EVLENGTH) opctx.u.spr.bkbeta=DAV(box);  // beta values corresponding to bk; we use only high part
  ASSERTSYS(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,"bkbeta is not on cacheline bdy")  // we fetch along rows; insist on data alignment
  opctx.u.spr.zstride=bkzstride;   // use the agreed stride
  // The initial shared value is infinity except in bound columns: then beta.  Initialize the found-row value to -1 for 'nothing found' normally,
  // but for bound columns use #rows which indicates a Nonbasic Swap
  if(colrvt&0b10){
   box=C(AAV(w)[11]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) ASSERT(AN(box)>colx,EVINDEX)  // betas normally dp, but we use only high part anyway.  Look up the column beta
   opctx.sharedmin.D=DAV(box)[colx];   // init bound-col min SPR to beta
   opctx.retinfo=ninclfk;  // Init col# - if no lower SPR found could survive to become a Swap
  }else{opctx.retinfo=-1; opctx.sharedmin.D=inf;} // non-bound, allow any improvement, init ret to 'no row found' - possible if no nondangerous pivot, or unbounded
  // Calculate the number of rows to reserve.  The issue is that some rows are faster than others: rows with all 0s skip extended-precision
  // processing.  It might take 5 times as long to process one row as another.  To keep the tasks of equal size, we take a limited number of rows at a time.
  // Taking the reservation is an RFO cycle, which takes just about as long as checking a set of all-zero rows.  This suggests that the reservation should be
  // for NPAR*nthreads at least.  And, the bound/enforcing mask is constructed to match the AVX lanes: one quantum of mask has (NPAR*BW/2) bitpairs; reservation must be a multiple of that

  nthreads=likely(ninclfk>256)?nthreads:1;  // single-thread small problem (qp).  Multithreading is questionable as the result column will be read in the master thread
  I nbndbatches=(ninclfk+(BNDROWBATCH-1))>>LGBNDROWBATCH;  // number of bnd batches, including a final shard
  nthreads=MAX(1,MIN(nbndbatches,nthreads));  // each batch except the last must have a full BNDROWBATCH
  // First, the size of each later reservation, which needs to be big enough that the fetch_adds don't pile up
  UI4 ressize2, ressize;  // the number of rows in each reservation after the first, the number in the first
  ressize2=ROWSPERRESV*nthreads;   // enough rows to allow each thread to arbitrate if they have normal jobs
  ressize2=(ressize2+(BNDROWBATCH-1))&-BNDROWBATCH;  // the bound/enforcing mask is built in units of BNDROWBATCH bits so as to fit in an AVX register, matching the row numbers.
  if(ninclfk<2*ressize2*nthreads){
   // the amount to be reserved after the initial resv is more than half the total.  We should move that to the
   // initial allocation, since there is no arbitration there
   ressize=(nbndbatches/nthreads)<<LGBNDROWBATCH;  // the most ressize can be is enough batches not to overrun the input
   ressize=MIN(ressize,32768-BNDROWBATCH);  // init alloc must fit into a US
   ressize=MIN(ressize,ressize2);  // a good and legal starting size
   ressize2=ninclfk/nthreads-ressize; ressize2=MAX(1,ressize2); ressize2=(ressize2+(BNDROWBATCH-1))&-BNDROWBATCH;  // distribute remaining batches evenly over the threads.  Ensure ressize2>0
  }else{
   // the initial allocation can be followed by allocations of size ressize2.  Allocate half the rows in the initial allocation
   ressize=((nbndbatches>>1)/nthreads)<<LGBNDROWBATCH;  // half the total batches, evened out per thread.  There must be at least 1
   ASSERTSYS(ressize>0,"ressize")
   ressize=MIN(ressize,32768-BNDROWBATCH);  // init alloc must fit into a US
  }
  ressize=((nthreads-2)&(ninclfk-32768))<0?ninclfk:ressize;  // if just 1 thread and length fits in a US, allocate it all at the beginning
  opctx.ressize=ressize;
  opctx.nextresv=nthreads*ressize;  // set resv pointer to after the initial allocation
  opctx.u.spr.ressize2=ressize2;

  actionrtn=(unsigned char (*)(JJ, void *, UI4))jtmvmsparseesprx;
 }

 // initialize shared/returned section
 opctx.nimpandcols=0;  // number of improvement,number of cols finished
 opctx.ndotprods=0;  // total # dotproducts evaluated
 opctx.ctxlock=0;  // init lock available

 jtjobrun(jt,actionrtn,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 // atomic sync operation on the job queue guarantee that we can use regular loads from opctx

 A r; GAT0(r,FL,5,1); D *rv=DAV1(r);  // return area
 I retinfo=rv[1]=opctx.retinfo; rv[2]=(UI4)opctx.nimpandcols; rv[3]=opctx.ndotprods;
 if(isgradmode){
  // gradient mode.  We were minimizing 1/gradient^2; convert to gradient
  rv[0]=0.; if(unlikely(opctx.sharedmin.I==IMAX))rv[0]=3.; else rv[4]=sqrt(1/opctx.sharedmin.D);  // give ok return w/gradient.  In ridiculous case where no gradient was found, return no pivot
 }else{
  // onecol+SPR.  classify as normal/dangerous/nonimproving/nonexistent/unbounded
  D spr=rv[4]=opctx.sharedmin.D;  // return SPR/col val as informational data
  if(unlikely(opctx.sharedmin.I==IMIN)){rv[0]=4.;  // abort on SPR=0; set rc 4, leave row -2 and SPR 0
  }else if(unlikely(spr==inf)){rv[0]=4.; rv[1]=-1.;  // no SPR found: problem is unbounded.  Leave SPR inf as indication, set row=-1
  }else if(unlikely(spr==ALMOSTINF)){rv[0]=3.;  // Positive col value found, but no pivots, not even dangerous ones (perhaps blocked by small bk)
  }else{
   // sharedmin is positive SPR if improving; negative recip of max column value if nonimproving
   rv[0]=spr<0?2.:0.;  // rc if not dangerous: negative shared value is the reciprocal of the column value when SPR=0.  This is a nonimproving pivot, rc=2 then
   if(BETWEENO(retinfo,0,ninclfk)){  // normal nonswap row found
    D colval=ABS(((E*)remflgs(opctx.u.spr.zv))[retinfo].hi);  // the value in the SPR row of the column.  If neg, we will swap before pivot, so use |value|
    if(unlikely(colval<parms[spr<0?7:6]))rv[0]+=1.0;  // flag if dangerous, with threshold depending on improving status
   }  // if nonbasic swap row found, we will take the swap willy-nilly because its dangerousness doesn't hurt anything
  }
 }
 EPILOG(r);
#else
 ASSERT(0,EVNONCE);  // this requires AVX to make any sense
#endif
}

#if C_AVX2 || EMU_AVX2
// everything we need for one core's execution
struct __attribute__((aligned(CACHELINESIZE))) ekctx {
 I rowsperthread;  // number of rows of prx for each thread to handle
 // the rest is moved into static names
 // arguments
 I4 coln0;  // number of paired columns
 I4 ncvals;  // number of values in pivot col, which may exceed #prx
 A qk;
 A prx;
 A pcx;
 A pivotcolnon0;
 A pivotrownon0;
 A absfuzzmplr;
} ;
// the processing loop for one core.  We take groups of rows, in order
static unsigned char jtekupdatex(J jt,struct ekctx* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(rowsperthread)YC(prx)YC(qk)YC(pcx)YC(pivotcolnon0)YC(pivotrownon0)YC(absfuzzmplr)
#undef YC

 D *mplrd=DAV(absfuzzmplr);  // pointer to absfuzz/mplr data
 __m256d pcoldh, pcoldl=_mm256_setzero_pd();  // value from pivotcolnon0, multiplying one row
 __m256d prowdh, prowdl=_mm256_setzero_pd();  // values from pivotrownon0
 __m256d mabsfuzz=_mm256_set1_pd(ABS(*mplrd));  // comparison tolerance (if given)
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
 I dpflag=(I)(*mplrd<0.)<<5;  // qp precision flags: 1=Qk 2=pivotcolnon0 4=pivotrownon0 8=mplr exists 16=both mpcands dp, no mplr 32=Qk update in batches of NPAR consecutive addrs
 D *qkv=DAV(qk); I qksize=AS(qk)[AR(qk)-1]; I t=AS(qk)[AR(qk)-2]; t=AR(prx)==0?1:t; I qksizesq=qksize*t; dpflag|=AR(qk)>AR(prx)+1;  // pointer to qk data, length of a row, offset to low part if present.  offset is */_2{.$Qk, or {:$bk
 UI rowx=ti*rowsperthread, rown=AN(prx), slicen=rown; slicen=rown<rowx+rowsperthread?slicen:rowx+rowsperthread;   // current row, ending row+1 taken for the current task#
 I *rowxv=IAV(prx); D *pcn0v=DAV(pivotcolnon0); dpflag|=(AR(pivotcolnon0)>1)<<1;  // address of row indexes, column data
 UI coln=AN(pcx); I *colxv=IAV(pcx); D *prn0v=DAV(pivotrownon0); UI prnstride=AS(pivotrownon0)[AR(pivotrownon0)-1]; dpflag|=(AR(pivotrownon0)>1)<<2;  // # cols, address of col indexes, stride betw rows. row data.  Note: in scatter mode coln can be used instaed of prnstride
 dpflag|=AR(absfuzzmplr)<<3; dpflag&=~(AR(absfuzzmplr)<<5);  // set flag if mplr is given; if not mplr, clear 'batch mode'
 dpflag|=((dpflag&0b1110)==0)<<4;  // set 'single-precision multiply' flag if that is enough
 // for each row
 for(;rowx<slicen;++rowx){
  // get the address of this row in Qk
  D *qkvrow=qkv+rowxv[rowx]*qksize;   // the row of Qk being modified
  // fetch the pivotcol value into all lanes.  Change its sign so we subtract the product
  pcoldh=_mm256_xor_pd(sgnbit,_mm256_set1_pd(pcn0v[rowx]));  // fetch high part of pivotcol value
  if(!(dpflag&32)){  // normal mode, gather, scatter, duplicate check
   if(dpflag&2)pcoldl=_mm256_xor_pd(sgnbit,_mm256_set1_pd(pcn0v[rown+rowx]));  // fetch low part if it is present
   UI colx; I okwds=NPAR; I okmsk=(1LL<<NPAR)-1;  //  number/mask of valid wds in block
   // for each column-group
   __m256d endmask=sgnbit;  // mask for maskload and gather, indicating # words to process.  Starts all valid, reset for last batch or for any mplr
   for(colx=0;colx<coln;colx+=okwds){
    __m256i prn0x;  // indexes of nonzero values in row
    // get the mask of valid values, fetch pivotrow values, fetch the Qk indexes to modify
    if(coln-colx>=NPAR){  // all lanes valid
     prn0x=_mm256_loadu_si256((__m256i*)(colxv+colx));  // load the indexes into Qk
     prowdh=_mm256_loadu_pd(prn0v+colx);  // load next 4 non0 values in pivotrow
    if(likely(dpflag&4))prowdl=_mm256_loadu_pd(prn0v+coln+colx);  // and low part if present
    }else{
     endmask=_mm256_loadu_pd((double*)(validitymask+NPAR-(coln-colx)));  // mask of valid lanes
     okmsk=_mm256_movemask_pd(endmask);  // mask of valid words in this block - always at least 1
     prn0x=_mm256_maskload_epi64(colxv+colx,_mm256_castpd_si256(endmask));  // load the indexes into Qk
     prowdh=_mm256_maskload_pd(prn0v+colx,_mm256_castpd_si256(endmask));  // load next 4 non0 values in pivotrow
     if(likely(dpflag&4))prowdl=_mm256_maskload_pd(prn0v+coln+colx,_mm256_castpd_si256(endmask));  // and low part if present
    }
    // mplr is given, we are modifying ck/Rk, which have only one row but may repeat column #s.  We have to
    // make sure that we don't work on the same column# in one batch of NPAR, because one update would be lost.
    // We compare the indexes against each other and set endmask so as to stop before a repetition of index
    if(unlikely(dpflag&8)){
     // compare 0-1 and 2-3
     __m256i cmp01012323=_mm256_cmpeq_epi64(prn0x,_mm256_shuffle_epi32(prn0x,0b01001110));  // 0=1 0=1 2=3 2=3
     // if 0=1, clear 1 2 3; if 2=3, clear 3
     endmask=_mm256_castsi256_pd(_mm256_andnot_si256(_mm256_or_si256(_mm256_permute2x128_si256(cmp01012323,cmp01012323,0b00001000),_mm256_blend_epi32(_mm256_setzero_si256(),cmp01012323,0b11001100)),_mm256_castpd_si256(endmask)));
     // compare 0-2 and 1-3
     __m256i cmpxxxx0213=_mm256_cmpeq_epi64(prn0x,_mm256_permute4x64_epi64(prn0x,0b01000000));  // x x 0=2 1=3
     // if 0=2, clear 2 3; if 1=3, clear 3
     endmask=_mm256_castsi256_pd(_mm256_andnot_si256(_mm256_blend_epi32(_mm256_setzero_si256(),_mm256_or_si256(_mm256_shuffle_epi32(cmpxxxx0213,0b01000100),cmpxxxx0213),0b11110000),_mm256_castpd_si256(endmask)));
     // compare 0-3 and 1-2
     __m256i cmpxxxx0312=_mm256_cmpeq_epi64(prn0x,_mm256_permute4x64_epi64(prn0x,0b00010000));  // x x 0=3 1=2
     // if 1=2, clear 2 3; if 0=3 clear 3
     endmask=_mm256_castsi256_pd(_mm256_andnot_si256(_mm256_blend_epi32(_mm256_setzero_si256(),_mm256_or_si256(_mm256_shuffle_epi32(cmpxxxx0312,0b01000100),cmpxxxx0312),0b11110000),_mm256_castpd_si256(endmask)));
     mabsfuzz=_mm256_maskload_pd(mplrd+colx,_mm256_castpd_si256(endmask));  // load next 4 non0 values in pivotrow
     okmsk=_mm256_movemask_pd(endmask);  // mask of valid words in this block - always at least 1
     okwds=(0b100000000110010010>>okmsk)&7;  // Advance to next nonrepeated column.  valid values are 1 3 7 15 for which we want results 1 2 3 4
    }
    // gather the high parts of Qk
    __m256d qkvh=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),qkvrow,prn0x,endmask,SZI);
    // create max(abs(qkvh),abs(pcoldh*prowdh)) which will go into threshold calc
    if(unlikely(!(dpflag&1))){
     // single-precision calculation
     // if mplr given, multiply prow by it
     if(dpflag&8)prowdh=_mm256_mul_pd(prowdh,mabsfuzz);
     // calculate old - pcol*prow
     qkvh=_mm256_fmadd_pd(prowdh,pcoldh,qkvh);
     if(!(dpflag&8)){
      // thresholding - convert maxabs to abs(qkvh) > thresh: if 0, means result should be forced to 0
      qkvh=_mm256_and_pd(qkvh,_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),mabsfuzz,_CMP_GT_OQ)); // zero if lower than fuzz (high part)
     }
     // scatter the results (high part)
//   _mm256_mask_i64scatter_pd(qkvrow,endmask,prn0x,qkvh,SZI);
     qkvrow[_mm256_extract_epi64(prn0x,0)]=_mm256_cvtsd_f64(qkvh);   // this is extract_pd
     if(likely(okmsk&0b1000))goto storeh1111; if(okmsk&0b100)goto storeh111; if(okmsk&0b10)goto storeh11; goto storeh1;
     storeh1111: qkvrow[_mm256_extract_epi64(prn0x,3)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvh,0b11100111));
     storeh111: qkvrow[_mm256_extract_epi64(prn0x,2)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvh,0b11100110));
     storeh11: qkvrow[_mm256_extract_epi64(prn0x,1)]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvh,0b0001));
     storeh1: ;
    }else{
     // extended-precision calculation
     __m256d iph,ipl,isl;  // intermediate products and sums
     __m256d qkvl;  // low-order part of result

     // if mplr given, multiply prow by it
     if(dpflag&8){
      // mplr is dp, prow is qp.
      TWOPRODQD(prowdh,prowdl,mabsfuzz,iph,ipl) prowdh=iph; prowdl=ipl;
     }
     // (iph,ipl) = - prowdh*pcoldh    we could skip the extended calc if both mpcands are dp, as they are for some swap calcs; but we deem it not worthwhile
     TWOPROD(prowdh,pcoldh,iph,ipl)  // (prowdh,pcoldh) to high precision
     __m256d magqh=qkvh;   // save high part of one addend
     if(!(dpflag&16)){ipl=_mm256_fmadd_pd(prowdh,pcoldl,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh,ipl);}  // accumulate middle pps - can skip for b0 when both mpcands are dp
     // Because we added 3 low-order values (with the same shift) - 4 if mplr used - , we are limiting precision to 104 bits
     qkvl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),qkvrow+qksizesq,prn0x,endmask,SZI);  // gather the low parts of Qk
     // (qkvh,qkvl) - (prowdh,prowdl) * (pcoldh,pcoldl)
     // Do high-precision add of qkvh and iph.  If this decreases the absvalue of qkvh, we will lose precision because of insufficient
     // bits of qkv.  If this increases the absvalue of qkvh, all of qkvl will contribute and the limit of validity will be
     // from the product.  In either case it is safe to accumulate all the partial products and ipl into qkvl
     qkvl=_mm256_add_pd(qkvl,ipl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
     TWOSUM(qkvh,iph,qkvh,isl)   // combine the high parts
     isl=_mm256_add_pd(isl,qkvl);  // add the combined low parts
     // Make sure qkvl is much less than qkvh
     TWOSUM(qkvh,isl,qkvh,qkvl)  // put qkvh into canonical form
     if(!(dpflag&8)){
      // thresholding - combine mabsfuzz with relative max;  if > |qphi|, means result should be forced to 0
      magqh=_mm256_fmadd_pd(_mm256_andnot_pd(sgnbit,magqh),_mm256_set1_pd(RELSIGMAX),mabsfuzz);  // composite threshold: a fraction of the magnitude of one arg, plus an absolute min
      magqh=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),magqh,_CMP_GT_OQ);   // maxqh = 0 if result too small
      qkvl=_mm256_and_pd(qkvl,magqh); // zero if lower than fuzz (low part)
      qkvh=_mm256_and_pd(qkvh,magqh); // zero if lower than fuzz (high part)
     }
     // scatter the results (both parts)
//   _mm256_mask_i64scatter_pd(qkvrow,endmask,prn0x,qkvh,SZI);
     qkvrow[_mm256_extract_epi64(prn0x,0)]=_mm256_cvtsd_f64(qkvh);
     qkvrow[_mm256_extract_epi64(prn0x,0)+qksizesq]=_mm256_cvtsd_f64(qkvl);
     if(likely(okmsk&0b1000))goto storeq1111; if(okmsk&0b100)goto storeq111; if(okmsk&0b10)goto storeq11; goto storeq1;
     storeq1111: qkvrow[_mm256_extract_epi64(prn0x,3)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvh,0b11100111));
     qkvrow[_mm256_extract_epi64(prn0x,3)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvl,0b11100111));
     storeq111: qkvrow[_mm256_extract_epi64(prn0x,2)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd (qkvh,0b11100110));
     qkvrow[_mm256_extract_epi64(prn0x,2)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute4x64_pd (qkvl,0b11100110));
     storeq11: qkvrow[_mm256_extract_epi64(prn0x,1)]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvh,0b0001));
     qkvrow[_mm256_extract_epi64(prn0x,1)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvl,0b0001));
     storeq1: ;
    }
   }
  }else{
   // Batch Qk update: always qp, of aligned 128-byte blocks, no check for duplicates
   pcoldl=_mm256_xor_pd(sgnbit,_mm256_set1_pd(pcn0v[rown+rowx]));  // fetch low part of col value, i. e. the value common to this row
   UI colx;  //  index to work on
   // for each block
   for(colx=0;colx<coln;++colx){
    I blockx=colxv[colx];  // get index of next args
    __m256d iph,ipl,isl;  // intermediate products and sums
    // we do not use aligned loads for the row values, because sometimes they have been copied to an unaligned temp buffer (as on a swap).  They will be from
    // fast local cache anyway
    __m256d prowdh=_mm256_loadu_pd(&prn0v[blockx]),qkvh=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&qkvrow[blockx]));  // high parts of row & result args
    __m256d prowdl=_mm256_loadu_pd(&prn0v[blockx+prnstride]),qkvl=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)&qkvrow[blockx+qksizesq]));  // low parts of row & result args

    // (iph,ipl) = - prowdh*pcoldh    we could skip the extended calc if both mpcands are dp, as they are for some swap calcs; but we deem it not worthwhile
    TWOPROD(prowdh,pcoldh,iph,ipl)  // (prowdh,pcoldh) to high precision
    __m256d magqh;   // will hold combined relative+absolute zero tolerance
    ipl=_mm256_fmadd_pd(prowdh,pcoldl,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh,ipl);  // accumulate middle pps - can skip for b0 when both mpcands are dp
    // Because we added 3 low-order values (with the same shift), we are limiting precision to 104 bits
    // (qkvh,qkvl) - (prowdh,prowdl) * (pcoldh,pcoldl)
    // Do high-precision add of qkvh and iph.  If this decreases the absvalue of qkvh, we will lose precision because of insufficient
    // bits of qkv.  If this increases the absvalue of qkvh, all of qkvl will contribute and the limit of validity will be
    // from the product.  In either case it is safe to accumulate all the partial products and ipl into qkvl
    // thresholding - combine mabsfuzz with relative max;  if > |qphi|, means result should be forced to 0
    magqh=_mm256_fmadd_pd(_mm256_andnot_pd(sgnbit,qkvh),_mm256_set1_pd(RELSIGMAX),mabsfuzz);  // composite threshold: a fraction of the magnitude of one arg, plus an absolute min
    qkvl=_mm256_add_pd(qkvl,ipl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
    TWOSUM(qkvh,iph,qkvh,isl)   // combine the high parts
    isl=_mm256_add_pd(isl,qkvl);  // add the combined low parts
    // Make sure qkvl is much less than qkvh
    TWOSUM(qkvh,isl,qkvh,qkvl)  // put qkvh into canonical form
    magqh=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),magqh,_CMP_GT_OQ);   // maxqh = 0 if result too small
    qkvl=_mm256_and_pd(qkvl,magqh); qkvh=_mm256_and_pd(qkvh,magqh); // zero if lower than fuzz
    _mm256_stream_pd(&qkvrow[blockx],qkvh); _mm256_stream_pd(&qkvrow[blockx+qksizesq],qkvl);  // write out the new Qk.  Should stream? 
   }
  }
 }  // loop to next row
 R 0;
}

// 128!:12  calculate
// Qk/bk=: (((<prx;pcx) { Qk) ((~:!.absfuzz) * -) pivotcolnon0 */ pivotrownon0 [* mplr]) (<prx;pcx)} Qk/bk
// with high precision
// a is prx;pcx;pivotcolnon0;pivotrownon0;absfuzz/mplr (mplr if not atom)
//  mplr is list of dp values or atomic threshold.  If -threshold, use abs, and pcx is a list of cache-aligned offsets into blocks in pivotrownon0
// w is Qk or bk.  If bk, prx must be scalar 0
// If Qk has rank > rank pivotrownon0 + rank prx, or pivotcolnon0/pivotrownon0 rank 2, calculate them in extended FP precision
// Qk/bk is modified in place
F2(jtekupdate){F12IP;
 ARGCHK2(a,w);
 // extract the inputs
 A qk=w; ASSERT(AT(w)&FL,EVDOMAIN) ASSERT(ASGNINPLACESGN(SGNIF(jtfg,JTINPLACEWX),w),EVNONCE)
 ASSERT(AT(a)&BOX,EVDOMAIN) ASSERT(AR(a)==1,EVRANK) ASSERT(AN(a)==5,EVLENGTH)  // a is 5 boxes
 A box0=C(AAV(a)[0]), box1=C(AAV(a)[1]), box2=C(AAV(a)[2]), box3=C(AAV(a)[3]), box4=C(AAV(a)[4]);
 A prx=box0; ASSERT(AT(prx)&INT,EVDOMAIN) ASSERT(AR(prx)<=1,EVRANK)  // prx is integer list or atom
 A pcx=box1; ASSERT(AT(pcx)&INT,EVDOMAIN) ASSERT(AR(pcx)==1,EVRANK)  // pcx is integer list
 A pivotcolnon0=box2; ASSERT(AT(pivotcolnon0)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(pivotcolnon0),1,2),EVRANK)
 ASSERT(AR(pivotcolnon0)==1||AS(pivotcolnon0)[0]==2, EVLENGTH)  // pivotcolnon0 is float or extended list
 A pivotrownon0=box3; ASSERT(AT(pivotrownon0)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(pivotrownon0),1,2),EVRANK)
 ASSERT(AR(pivotrownon0)==1||AS(pivotrownon0)[0]==2,EVLENGTH)  // pivotrownon0 is float or extended list
 A absfuzzmplr=box4; ASSERT(AT(absfuzzmplr)&FL,EVDOMAIN) ASSERT(AR(absfuzzmplr)<=1,EVRANK)  // absfuzz is a float atom; mplr is a float list
 I isbatch=0;  // set if we are doing batches of aligned accesses
 if(AR(absfuzzmplr)==1)ASSERT(AN(absfuzzmplr)==AN(pcx),EVLENGTH)  // if mplr, must be one per input in row
 else isbatch=DAV(absfuzzmplr)[0]<0.;  // batch is negative threashold
 // agreement
 ASSERT(BETWEENC(AR(w),AR(prx)+1,AR(prx)+2),EVRANK)  // Qk is nxn; bk is n, treated as a single row.  Each may be quadprec
 ASSERT(AR(w)==AR(prx)+1||AS(w)[0]==2,EVLENGTH)
 if(AR(prx)!=0){ASSERT(AS(w)[AR(w)-1]>=AS(w)[AR(w)-2]-1,EVLENGTH) DO(AN(prx), ASSERT(IAV(prx)[i]<AS(w)[AR(w)-2],EVINDEX))} else{ASSERT(IAV(prx)[0]==0,EVINDEX)}  // Qk/bk rows may be padded (Qk might include Fk); valid row indexes
 ASSERT(AN(prx)==AS(pivotcolnon0)[AR(pivotcolnon0)-1],EVLENGTH)  // indexes and values must agree
 I m=AN(prx), n=AN(pcx);  // # rows & columns to modify
 if(isbatch){
  // we are processing cache-aligned blocks
  ASSERTSYS(((I)DAV(qk)&((SZD<<LGNPAR)-1))==0,"Qkt is not on cacheline bdy")  // we fetch along rows; insist on data alignment
  ASSERTSYS((AS(qk)[2]&(NPAR-1))==0,"stride of Qkt is not a cacheline multiple")  // we fetch along rows; insist on data alignment
// no longer  ASSERTSYS(((I)DAV(pivotrownon0)&((SZD<<LGNPAR)-1))==0,"row values not on cacheline bdy")  // we fetch along rows; insist on data alignment
// no longer  ASSERTSYS((AS(pivotrownon0)[1]&(NPAR-1))==0,"stride of row values is not a cacheline multiple")  // we fetch along rows; insist on data alignment
  DO(n, ASSERT(IAV(pcx)[i]<AS(w)[AR(w)-1],EVINDEX) ASSERTSYS((IAV(pcx)[i]&(NPAR-1))==0,"pcx not 0(mod NPAR)"))  // verify valid column indexes, aligned to batches
  n<<=LGNPAR;  // actually NPAR values per index
 }else{
  ASSERT(n==AS(pivotrownon0)[AR(pivotrownon0)-1],EVLENGTH) // must have one index per value
  DO(n, ASSERT(IAV(pcx)[i]<AS(w)[AR(w)-1],EVINDEX))  // verify valid column indexes
 }

 // do the work
 
 // figure out how many threads to use, how many lines to take in each one
#define TASKMINATOMS ((2*2000)/10)  // TUNE a cached atom takes 10 clocks to compute; an uncached one takes 15? (2022 Alder Lake).  We don't want to start a task with less than 2000 clocks, so insist on twice that many
 I nthreads=(*JT(jt,jobqueues))[0].nthreads+1;  // the number of threads we would like to use (including master), init to # available
 I rowsperthread=m;  // will be #rows each processor should take
 if(((1-m)&(1-nthreads)&(TASKMINATOMS-m*n))>=0)nthreads=1;  // if only one thread, or job too small, use just one thread
 else{
  // big enough for multithreading.
  rowsperthread=(m+nthreads-1)/nthreads;  // number of threads per processor, rounded up
 }
#undef TASKMINATOMS
#define YC(n) .n=n,
struct ekctx opctx={YC(rowsperthread)YC(prx)YC(qk)YC(pcx)YC(pivotcolnon0)YC(pivotrownon0)YC(absfuzzmplr)};
#undef YC

 if(nthreads>1)jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtekupdatex,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 else jtekupdatex(jt,&opctx,0);  // the one-thread case is not uncommon and we avoid thread overhead then

 R qk;
}






// the processing loop for one core.  We take groups of rows, in order
// 'col' and 'row' refer to Qkt.  'pivotcol' is the pivot column, 'pivotrow' is the column of Qkt (=row of Qk)
static unsigned char jtqktupdatex(J jt,struct ekctx* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(rowsperthread)YC(pcx)YC(qk)YC(prx)YC(coln0)YC(ncvals)YC(pivotcolnon0)YC(pivotrownon0)YC(absfuzzmplr)
#undef YC

 D *mplrd=DAV(absfuzzmplr);  // pointer to absfuzz/mplr data
__m256d prowdh, prowdl=_mm256_setzero_pd();  // values from the col of Qkt
 __m256d mabsfuzz=_mm256_set1_pd(ABS(*mplrd));  // comparison tolerance (if given)
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
 I dpflag=(I)(*mplrd<0.)<<5;  // qp precision flags: 1 2 4 res 8=mplr exists 16 res 32=Qkt update in batches of NPAR consecutive addrs
 dpflag|=AR(absfuzzmplr)<<3; dpflag&=~(AR(absfuzzmplr)<<5);  // set flag if mplr is given; if mplr, clear 'batch mode'
 E *qkv=EAV(qk); I qksize=AS(qk)[AR(qk)-1]; I t=AS(qk)[AR(qk)-2]; t=AR(pcx)==0?1:t;  // pointer to qk data, length of a row,
 UI rowx=ti*rowsperthread, rown=AN(pcx), slicen=rown; slicen=rown<rowx+rowsperthread?slicen:rowx+rowsperthread;   // current row, ending row+1 taken for the current task#
 I *rowxv=IAV(pcx); E *prn0v=EAV(pivotrownon0);  // address of row indexes, values from pivot row (each of which modifies a row of Qkt)
 I *colxv=IAV(prx); E *pcn0v=EAV(pivotcolnon0);  // address of col indexes, data repeated for each row of Qkt Note: in scatter mode coln can be used instead of prnstride
 E *pcn1v=pcn0v+coln0;  // pairs index 2 Es in rowvalues per colx; singles index 1.  biasing the basepointer up makes the transition smooth
// for each row
 for(;rowx<slicen;++rowx){
  E *qkvrow=qkv+rowxv[rowx]*qksize;   // base of the row of Qkt being modified
  // fetch the pivotrow value into all lanes.  Change its sign so we subtract the product
  prowdh=_mm256_xor_pd(sgnbit,_mm256_set1_pd(prn0v[rowx].hi));  // fetch high part of pivotrow value into all lanes
  prowdl=_mm256_xor_pd(sgnbit,_mm256_set1_pd(prn0v[rowx].lo));  // fetch low part of pivotrow value into all lanes
 // obsolete  if(!(dpflag&32)){  // if not batch mode...
   I colx;
// I okmsk=(1LL<<NPAR)-1;  //  number/mask of valid wds in block
   // for each column-group in Qkt (4 lanes)
   __m256d endmask;  // mask for 'maskload' and gather, indicating # words to process.  Starts all valid, reset for last batch or for any mplr
   // first handle the paired columns.  They always come in pairs of pairs, never have a multiplier, never ck
   for(colx=0;colx<coln0*(I)sizeof(*colxv);colx+=2*sizeof(*colxv)){  // here colx is offset to colxv (16 bytes for two pairs) and also to pcn0v (64 bytes).  The value in *(colxv+colx) is byte offset to value in row
   // fetch pivotcol values (high-low), fetch the Qkt byte indexes to modify.  We may overfetch
   I o0=((I*)((I)colxv+colx))[0], o1=((I*)((I)colxv+colx))[1];  // fetch offset to pair0, pair1
   __m256d pcoldh0213=_mm256_loadu_pd((D*)((I)pcn0v+colx*(2*sizeof(E)/sizeof(*colxv))));  // load next 4 non0 values in pivotcol - high part
    __m256d pcoldl0213=_mm256_loadu_pd((D*)((I)pcn0v+(colx+sizeof(*colxv))*(2*sizeof(E)/sizeof(*colxv))));  // load next 4 non0 values in pivotcol - low part
    // gather the values from Qkt
   __m256d h0l0h1l1=_mm256_loadu_pd((D*)((I)qkvrow+o0)),h2l2h3l3=_mm256_loadu_pd((D*)((I)qkvrow+o1));  // load     scaf use non-temporal load?
    __m256d qkvh=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), qkvl=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // batch of Qkt, high & low separated in 0213 order

    // create max(abs(qkvh),abs(pcoldh*prowdh)) which will go into threshold calc
    __m256d iph,ipl,isl;  // intermediate products and sums
    // (iph,ipl) = - prowdh*pcoldh
    TWOPROD(prowdh,pcoldh0213,iph,ipl)  // (prowdh,pcoldh) to high precision
    __m256d magqh=qkvh;   // save high part of one addend
    ipl=_mm256_fmadd_pd(prowdh,pcoldl0213,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh0213,ipl);  // accumulate middle pps
    // Because we added 3 low-order values (with the same shift) - 4 if mplr used - , we are limiting precision to 104 bits
    // (qkvh,qkvl) - (prowdh,prowdl) * (pcoldh,pcoldl)
    // Do high-precision add of qkvh and iph.  If this decreases the absvalue of qkvh, we will lose precision because of insufficient
    // bits of qkv.  If this increases the absvalue of qkvh, all of qkvl will contribute and the limit of validity will be
    // from the product.  In either case it is safe to accumulate all the partial products and ipl into qkvl
    qkvl=_mm256_add_pd(qkvl,ipl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
    TWOSUM(qkvh,iph,qkvh,isl)   // combine the high parts
    isl=_mm256_add_pd(isl,qkvl);  // add the combined low parts
    // Make sure qkvl is much less than qkvh
    TWOSUM(qkvh,isl,qkvh,qkvl)  // put qkvh into canonical form

    // thresholding - combine mabsfuzz with relative max;  if > |qphi|, means result should be forced to 0
    magqh=_mm256_fmadd_pd(_mm256_andnot_pd(sgnbit,magqh),_mm256_set1_pd(RELSIGMAX),mabsfuzz);  // composite threshold: a fraction of the magnitude of one arg, plus an absolute min
    magqh=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),magqh,_CMP_GT_OQ);   // maxqh = 0 if result too small
    qkvl=_mm256_and_pd(qkvl,magqh); // zero if lower than fuzz (low part)
    qkvh=_mm256_and_pd(qkvh,magqh); // zero if lower than fuzz (high part)

    // scatter the results (both parts)
   _mm256_storeu_pd((D*)((I)qkvrow+o0),_mm256_shuffle_pd(qkvh,qkvl,0b0000));  _mm256_storeu_pd((D*)((I)qkvrow+o1),_mm256_shuffle_pd(qkvh,qkvl,0b1111));   // scaf use non-temporal store?
  }
   I okwds=NPAR;  // # valid words in a batch.  Starts at max; never modified until last batch EXCEPT when ck mode, then set always
   for(;colx-(ncvals-coln0)*(I)sizeof(*colxv)<0;colx+=okwds*sizeof(*colxv)){
   // fetch pivotcol values (high-low), fetch the Qkt byte indexes to modify.  We may overfetch
   I o3=((I*)((I)colxv+colx))[3],o2=((I*)((I)colxv+colx))[2],o1=((I*)((I)colxv+colx))[1],o0=((I*)((I)colxv+colx))[0];  // offset to 4 Es in pivotrow and Qkt
    __m256d pcoldh0213, pcoldl0213;  // the next row multipliers, in 0213 order
    __m256d iph,ipl,isl;  // intermediate products and sums
    if(unlikely(dpflag&8)){   // modifying ck/Rk?
     // mplr is given, we are modifying ck/Rk, which have only one row but may repeat column #s.  We have to
     // make sure that we don't work on the same column# twice in a batch of NPAR, because one update would be lost.
     // We compare the indexes against each other and shorten okwds as needed
     __m256d h0l0h1l1=_mm256_loadu_pd((D*)((I)pcn1v+colx*(sizeof(E)/sizeof(*colxv)))),h2l2h3l3=_mm256_loadu_pd((D*)((I)pcn1v+(colx+2*sizeof(*colxv))*(sizeof(E)/sizeof(*colxv))));  // load the values
     mabsfuzz=_mm256_permute4x64_pd(_mm256_loadu_pd((D*)((I)mplrd+colx)),0b11011000);  // load next 4 multiplier values for ck, put into 0213 order
     iph=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), ipl=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // put values into 0213 order
     // Use the multiplier.  mplr is dp, pcol is qp.
     TWOPRODQD(iph,ipl,mabsfuzz,pcoldh0213,pcoldl0213)  // take mplr (dp) * row values (qp), leave in position of row values, in 0213 order
     okwds=o3==o2?3:4; okwds=o3==o1?3:okwds; okwds=o3==o0?3:okwds; okwds=o2==o1?2:okwds; okwds=o2==o0?2:okwds; okwds=o1==o0?1:okwds;   // validity stops after last nonrepeated index
     if(unlikely(okwds<4)){o3=o0; o2=okwds<3?o0:o2; o1=okwds<2?o0:o1;}  // omit words by writing them to o0 which will be overwritten.  Shouldn't happen often
   }else{   // not ck mode
     pcoldh0213=_mm256_loadu_pd((D*)((I)pcn1v+colx*(sizeof(E)/sizeof(*colxv))));  // load next 4 non0 values in pivotcol - high part (already put into 0213 order)
     pcoldl0213=_mm256_loadu_pd((D*)((I)pcn1v+(colx+2*sizeof(*colxv))*(sizeof(E)/sizeof(*colxv))));  // load next 4 non0 values in pivotcol - low part
   }
    // gather the values from Qkt
   __m256d h0l0h1l1=_mm256_blend_pd(_mm256_loadu_pd((D*)((I)qkvrow+o0)),_mm256_loadu_pd((D*)((I)qkvrow+o1-sizeof(E))),0b1100);  // read accumulands 0-1 
    __m256d h2l2h3l3=_mm256_blend_pd(_mm256_loadu_pd((D*)((I)qkvrow+o2)),_mm256_loadu_pd((D*)((I)qkvrow+o3-sizeof(E))),0b1100);  // read accumulands 2-3
    __m256d h0h2h1h3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), l0l2l1l3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // convert to 0213 order, hi & lo

    // (iph,ipl) = - prowdh*pcoldh
    TWOPROD(prowdh,pcoldh0213,iph,ipl)  // (prowdh,pcoldh) to high precision
   ipl=_mm256_fmadd_pd(prowdh,pcoldl0213,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh0213,ipl);  // accumulate middle pps
    // Because we added 3 low-order values (with the same shift) - 4 if mplr used - , we are limiting precision to 104 bits
    // (h0h2h1h3,l0l2l1l3) - (prowdh,prowdl) * (pcoldh,pcoldl)
    // Do high-precision add of h0h2h1h3 and iph.  If this decreases the absvalue of h0h2h1h3, we will lose precision because of insufficient
    // bits of qkv.  If this increases the absvalue of h0h2h1h3, all of l0l2l1l3 will contribute and the limit of validity will be
    // from the product.  In either case it is safe to accumulate all the partial products and ipl into l0l2l1l3
    l0l2l1l3=_mm256_add_pd(l0l2l1l3,ipl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
    TWOSUM(h0h2h1h3,iph,h0h2h1h3,isl)   // combine the high parts
    isl=_mm256_add_pd(isl,l0l2l1l3);  // add the combined low parts
    // Make sure l0l2l1l3 is much less than h0h2h1h3
    TWOSUM(h0h2h1h3,isl,h0h2h1h3,l0l2l1l3)  // put h0h2h1h3 into canonical form
    if(likely(!(dpflag&8))){
     // thresholding - combine mabsfuzz with relative max;  if > |qphi|, means result should be forced to 0
    // create max(abs(h0h2h1h3),abs(pcoldh*prowdh)) which will go into threshold calc
     __m256d magqh=_mm256_fmadd_pd(_mm256_andnot_pd(sgnbit,pcoldh0213),_mm256_set1_pd(RELSIGMAX),mabsfuzz);  // composite threshold: a fraction of the magnitude of one arg, plus an absolute min
     magqh=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,h0h2h1h3),magqh,_CMP_GT_OQ);   // maxqh = 0 if result too small
     l0l2l1l3=_mm256_and_pd(l0l2l1l3,magqh); // zero if lower than fuzz (low part)
     h0h2h1h3=_mm256_and_pd(h0h2h1h3,magqh); // zero if lower than fuzz (high part)
    }
    // scatter the results (both parts)
    // convert from 0213 to E order & write out.  Write back to front so the first value of repeated indexes survives
    h0l0h1l1=_mm256_shuffle_pd(h0h2h1h3,l0l2l1l3,0b0000); h2l2h3l3=_mm256_shuffle_pd(h0h2h1h3,l0l2l1l3,0b1111);  // convert result to E order
    _mm_storeu_pd((D*)((I)qkvrow+o3),_mm256_castpd256_pd128(_mm256_permute4x64_pd(h2l2h3l3,0b01001110))); _mm_storeu_pd((D*)((I)qkvrow+o2),_mm256_castpd256_pd128(h2l2h3l3));   // store 3&2
    _mm_storeu_pd((D*)((I)qkvrow+o1),_mm256_castpd256_pd128(_mm256_permute4x64_pd(h0l0h1l1,0b01001110)));  _mm_storeu_pd((D*)((I)qkvrow+o0),_mm256_castpd256_pd128(h0l0h1l1));   // store 1&0

#if 0  // obsolete
  }else{
   // Batch Qkt update: always qp, of aligned 128-byte blocks, no check for duplicates
   UI colx;  //  index to work on
   // for each block
   for(colx=0;colx<coln;++colx){
    I blockx=colxv[colx];  // get byte index of next batch in row
    __m256d iph,ipl,isl;  // intermediate products and sums
    // we do not use aligned loads for the row values, because sometimes they have been copied to an unaligned temp buffer (as on a swap).  They will be from fast local cache anyway

    __m256d pcoldh=_mm256_loadu_pd(pcn0v+colx*8), pcoldl=_mm256_loadu_pd(pcn0v+colx*8+4);  // next 4 pivotcol values, high then low in 0213 order
    __m256d h0l0h1l1=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)((I)qkvrow+blockx))), h2l2h3l3=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)((I)qkvrow+blockx)+1));  // batch of Qkt, as Es
    __m256d qkvh=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), qkvl=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // batch of Qkt, high & low separated in 0213 order

    TWOPROD(prowdh,pcoldh,iph,ipl)  // (prowdh,pcoldh) to high precision
    __m256d magqh;   // will hold combined relative+absolute zero tolerance
    ipl=_mm256_fmadd_pd(prowdh,pcoldl,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh,ipl);  // accumulate middle pps - can skip for b0 when both mpcands are dp
    // Because we added 3 low-order values (with the same shift), we are limiting precision to 104 bits
    // (qkvh,qkvl) - (prowdh,prowdl) * (pcoldh,pcoldl)
    // Do high-precision add of qkvh and iph.  If this decreases the absvalue of qkvh, we will lose precision because of insufficient
    // bits of qkv.  If this increases the absvalue of qkvh, all of qkvl will contribute and the limit of validity will be
    // from the product.  In either case it is safe to accumulate all the partial products and ipl into qkvl
    // thresholding - combine mabsfuzz with relative max;  if > |qphi|, means result should be forced to 0
    magqh=_mm256_fmadd_pd(_mm256_andnot_pd(sgnbit,qkvh),_mm256_set1_pd(RELSIGMAX),mabsfuzz);  // composite threshold: a fraction of the magnitude of one arg, plus an absolute min
    qkvl=_mm256_add_pd(qkvl,ipl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
    TWOSUM(qkvh,iph,qkvh,isl)   // combine the high parts
    isl=_mm256_add_pd(isl,qkvl);  // add the combined low parts
    // Make sure qkvl is much less than qkvh
    TWOSUM(qkvh,isl,qkvh,qkvl)  // put qkvh into canonical form
    magqh=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),magqh,_CMP_GT_OQ);   // maxqh = 0 if result too small
    qkvl=_mm256_and_pd(qkvl,magqh); qkvh=_mm256_and_pd(qkvh,magqh); // zero if lower than fuzz
    _mm256_stream_pd((D*)(__m256i*)((I)qkvrow+blockx),_mm256_shuffle_pd(qkvh,qkvl,0b0000)); _mm256_stream_pd((D*)((__m256i*)((I)qkvrow+blockx)+1),_mm256_shuffle_pd(qkvh,qkvl,0b1111));  // write out the new Qk.
   }
#endif
  }
 }  // loop to next row
 R 0;
}

// 128!:22  calculate
// Qkt/bk=: (((<pcx;prx) { Qkt) ((~:!.absfuzz) * -) pivotcolnon0 */ pivotrownon0 [* mplr]) (<pcx;prx)} Qkt/bk
// with high precision
// a is pcx;prx;pivotrownon0;pivotcolnon0;absfuzz/mplr (mplr if not atom)
// pcx is indexes of non0 in col of Qkt (=row of Qk), i. e. the rows of Qkt to be modified; for ck/Rk, which have one row, pcx is 0
// prx is indexes of non0 in pivot col, which give the column indexes of Qkt/ck/Rk to be modified (i. e. each index in Qkt)
// pivotrownon0 is pcx { col of Qkt, each of which multiplies the entire pivot column
// pivotcolnon0 is (prx after pair filling) { pivot column, which maps over rows of Qkt
//  mplr is list of dp values or atomic threshold.  If -threshold, use abs, and pcx is a list of cache-aligned offsets into blocks in pivotrownon0
// prx is ordered so that the first part is paired even/odd indexes that reside in a single cacheline.  The number of these pairs is pivotcolnon0 -&# prx
// w is Qkt or bk.
F2(jtqktupdate){F12IP;
 ARGCHK2(a,w);
 // extract the inputs
 A qk=w; ASSERT(AT(w)&QP,EVDOMAIN) ASSERT(ASGNINPLACESGN(SGNIF(jtfg,JTINPLACEWX),w),EVNONCE)  // w must be inplaceable
 ASSERT(AT(a)&BOX,EVDOMAIN) ASSERT(AR(a)==1,EVRANK) ASSERT(AN(a)==5,EVLENGTH)  // a is 5 boxes
 A box0=C(AAV(a)[0]), box1=C(AAV(a)[1]), box2=C(AAV(a)[2]), box3=C(AAV(a)[3]), box4=C(AAV(a)[4]);
 A pcx=box0; ASSERT(AT(pcx)&INT,EVDOMAIN) ASSERT(AR(pcx)<=1,EVRANK)  // pcx is integer list or atom
 A prx=box1; ASSERT(AT(prx)&INT,EVDOMAIN) ASSERT(AR(prx)==1,EVRANK)  // prx is integer list
 A pivotrownon0=box2; ASSERT(AT(pivotrownon0)&QP,EVDOMAIN) ASSERT(AR(pivotrownon0)==AR(pcx),EVRANK)
 A pivotcolnon0=box3; ASSERT(AT(pivotcolnon0)&QP,EVDOMAIN) ASSERT(AR(pivotcolnon0)==1,EVRANK)
 A absfuzzmplr=box4; ASSERT(AT(absfuzzmplr)&FL,EVDOMAIN) ASSERT(AR(absfuzzmplr)<=1,EVRANK)  // absfuzz is a float atom; mplr is a float list
if(AR(absfuzzmplr)==1)ASSERT(AN(absfuzzmplr)==AN(prx),EVLENGTH)  // if mplr, must be one per input in row
 else ASSERT(DAV(absfuzzmplr)[0]>=0.,EVDOMAIN)  // neg threrashold not allowed any more
// agreement
 ASSERT(AR(w)==AR(pcx)+AR(prx),EVRANK)  // Qkt is nxn+1..4; bk is n, treated as a single row.
 if(AR(pcx)!=0){DO(AN(pcx), ASSERT(IAV(pcx)[i]<AS(w)[0],EVINDEX))} else{ASSERT(IAV(pcx)[0]==0,EVINDEX)}  // valid row indexes
 ASSERT(AN(pcx)==AN(pivotrownon0),EVLENGTH)  // indexes and values must agree  scaf shapes should be identical
 I m=AN(pcx), n=AN(prx);  // # rows & columns/pairs to modify
 ASSERTSYS(((I)DAV(qk)&((SZD<<LGNPAR)-1))==0,"Qkt is not on cacheline bdy")  // we fetch along rows; insist on data alignment
 ASSERTSYS(AR(qk)<2||(AS(qk)[1]&(NPAR-1))==0,"stride of Qkt is not a cacheline multiple")  // we fetch along rows; insist on data alignment
ASSERT(n<=AS(pivotcolnon0)[AR(pivotcolnon0)-1],EVLENGTH) // must have one index per value
 DO(n, ASSERT(IAV(prx)[i]<AS(w)[AR(w)-1],EVINDEX))  // verify valid column indexes

 // transpose pivotcolnon0 into needed order
 I ncvals=AN(pivotcolnon0);  // # column values, which exceeds # column indexes if there are pairs
 I coln0=ncvals-AN(prx);   // number of pairs is # by which #values exceeds #indexes
#if 0  // obsolete 
 if(isbatch){  // obsolete 
  // batch mode transpose selected parts of pivotcolnon0 (which is the entire pivot column) into 0213 order, by cachelines
  DO(AN(prx), __m256d h0l0h1l1=_mm256_loadu_pd((D*)&pcnv[prxv[i]]); __m256d h2l2h3l3=_mm256_loadu_pd((D*)&pcnv[prxv[i]+2]);
   _mm256_storeu_pd((D*)&pcntv[i*4],_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000)); _mm256_storeu_pd((D*)&pcntv[i*4+2],_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111)); )
  pivotcolnon0=pcnt;   // use the transposed version
 }else{
#endif
 if(AR(absfuzzmplr)!=1){  // if no mplr given, we will be doing full batches in ALU and will always fetch 4 values at a time.  Put them into 0213 order
  A pcnt; GATV0(pcnt,QP,((ncvals-1)|3)+1,1) E *pcntv=EAV1(pcnt); E *pcnv=EAV(pivotcolnon0);
  DO((ncvals+(NPAR-1))>>LGNPAR, __m256d h0l0h1l1=_mm256_loadu_pd((D*)&pcnv[i*NPAR]); __m256d h2l2h3l3=_mm256_loadu_pd((D*)&pcnv[i*NPAR+2]);
   _mm256_storeu_pd((D*)&pcntv[i*4],_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000)); _mm256_storeu_pd((D*)&pcntv[i*4+2],_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111)); )
  pivotcolnon0=pcnt;   // use the transposed version
 }

 // convert column #s to byte offset of Es, and repeat the last one 3 times for safety
 I *prxv=IAV(prx);  // col#s
 A prxt; GATV0(prxt,INT,AN(prx)+3,1) I *prxtv=IAV1(prxt); 
 DO(AN(prx), prxtv[i]=prxv[i]*sizeof(E);) prxtv[AN(prx)+2]=prxtv[AN(prx)+1]=prxtv[AN(prx)+0]=prxtv[AN(prx)-1];
 prx=prxt;  // use the reoffset version

 // do the work
 
 // figure out how many threads to use, how many lines to take in each one
#define TASKMINATOMS ((2*2000)/10)  // TUNE a cached atom takes 10 clocks to compute; an uncached one takes 15? (2022 Alder Lake).  We don't want to start a task with less than 2000 clocks, so insist on twice that many
 I nthreads=(*JT(jt,jobqueues))[0].nthreads+1;  // the number of threads we would like to use (including master), init to # available
 I rowsperthread=m;  // will be #rows each processor should take
 if(((1-m)&(1-nthreads)&(TASKMINATOMS-m*n))>=0)nthreads=1;  // if only one thread, or job too small, use just one thread
 else{
  // big enough for multithreading.
  rowsperthread=(m+nthreads-1)/nthreads;  // number of threads per processor, rounded up
 }
#undef TASKMINATOMS
#define YC(n) .n=n,
struct ekctx opctx={YC(rowsperthread)YC(pcx)YC(qk)YC(prx)YC(coln0)YC(ncvals)YC(pivotcolnon0)YC(pivotrownon0)YC(absfuzzmplr)};
#undef YC

 if(nthreads>1)jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtqktupdatex,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 else jtqktupdatex(jt,&opctx,0);  // the one-thread case is not uncommon and we avoid thread overhead then

 R qk;
}

#else
F2(jtekupdate){F12IP;ASSERT(0,EVNONCE);}
F2(jtqktupdate){F12IP;ASSERT(0,EVNONCE);}
#endif

#if C_AVX2 || EMU_AVX2
// everything we need for one core's execution
struct __attribute__((aligned(CACHELINESIZE))) sprctx {
 I rowsperthread;  // number of rows of prx for each thread to handle
 // the rest is moved into static names
 // arguments
 A ck;
 A bk;
 I m;   // length of a column
 D ckthreshd;
 D bkthreshd;
 // result
 I row;  // the row number we found
} ;
// the processing loop for one core.  We take groups of rows, in order
static unsigned char jtfindsprx(J jt,struct sprctx* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(rowsperthread)YC(ck)YC(bk)YC(m)YC(ckthreshd)YC(bkthreshd)
#undef YC

 // establish pointers, offsets, and sizes.  We keep pointers to the beginning of the block and offset into the region we're working on
 D *ck0=DAV(ck);  // pointer to base of ck area
 D *bk0=DAV(bk);  // pointer to base of bk area
 I i0=rowsperthread*ti;  // starting offset of our block
 I m0=m-i0; m0=m0>rowsperthread?rowsperthread:m0;  // number of atoms in our area, MIN(rowsperthread,length of bk/ck)
 I i0e=m0+i0;  // end+1 offset for our block

 // find the best SPR, dp only

 // init for the column
 __m256i endmask=_mm256_setone_epi64(); // mask for validity of next 4 words: used to control fetch from column and store into result row
 __m256d minspr=_mm256_set1_pd(inf); // min spr found per lane
 __m256d ckthresh=_mm256_set1_pd(ckthreshd); // column threshold
 __m256d bkthresh=_mm256_set1_pd(bkthreshd); // column threshold
 I i;   // index to the block-of-4 we are working on
 // create the column NPAR values at a time
 for(i=i0;i<i0e;i+=NPAR){
  // get the validity mask and process: leave as ones until the end of the column
  __m256d ck4, bk4;  // values we are working on
  if(likely(i0e-i>=NPAR)){
   ck4=_mm256_loadu_pd(&ck0[i]); bk4=_mm256_loadu_pd(&bk0[i]);  // read in ck and bk
  }else{
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-(i0e-i)));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
   ck4=_mm256_maskload_pd(&ck0[i],endmask); bk4=_mm256_maskload_pd(&bk0[i],endmask);  // read in ck and bk
  }
  if(!_mm256_testz_si256(_mm256_castpd_si256(ck4),_mm256_castpd_si256(ck4))){  // if all c values are 0.0, we can skip everything - not unlikely
   // see if any of the new values is limiting: b/c < min SPR, c>ColThresh, bk>bkthresh
   // we perform this calculation on each lane separately, to save the expense of finding the minimum SPR each time one is calculated.
   // This means the values in each lane will be smaller than they would be with sharing, leading to perhaps 2 extra divisions.  That's a good trade.
   __m256d ratios=_mm256_fmsub_pd(minspr,ck4,bk4);  // minspr*c-b: sign set if this is NOT a new min SPR in its lane
   __m256d coverthresh=_mm256_cmp_pd(ck4,ckthresh,_CMP_GT_OQ);  // set to ~0 if c>ColThresh - never if endmask 0
   // we separate b into < threshold and >= threshold.   This must match the selection made for stall exit
   __m256d sprvalid=_mm256_and_pd(coverthresh,_mm256_cmp_pd(bk4,bkthresh,_CMP_GE_OQ));  // compare sets to ~0 if b>=bthresh - never if endmask is 0   set to ~0 if c>thresh & b>=thresh, i. e. SPR is valid
   if(!_mm256_testc_pd(ratios,sprvalid)){  // CF is 1 if ~a&b signs all 0 => ~(~new min)&(c good&b good) all 0 => no place where new min & c good & b good
    // at least one new SPR is limiting (in its lane).  Find the SPRs
    __m256d sprs=_mm256_div_pd(bk4,ck4);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch
    // the reciprocal takes a long time to run.  Make sure there are no dependencies.
    sprvalid=_mm256_andnot_pd(ratios,sprvalid);  // sprvalid now means 'valid and limiting' - high bit only
    minspr=_mm256_blendv_pd(minspr,sprs,sprvalid);   // remember the limiting SPR, in each lane
   }
  }
 }  // end of loop to find minspr per lane

 // find the smallest SPR, and its row#, of the ones in minspr.  In case of ties take the LAST
 __m256d allmin=_mm256_min_pd(minspr,_mm256_permute_pd(minspr,0b0101)); // allmin = min01 min01 min23 min23  could use integer in AVX-512
 allmin=_mm256_min_pd(allmin,_mm256_permute4x64_pd(allmin,0b00001010));  // allmin=min value in all lanes
 // now allmin has the min SPR in all lanes

 // We know that the overall column has a pivot, but if we are a thread it is possible that we have no pivot.  Exit now in that case.
 if(_mm256_cvtsd_f64(allmin)==inf)R 0;   // nothing more to do uif no SPRs

 // get the list of row#s that are in contention for best pivot
 // In QP, the true SPR may be up to 1.5 DP ULP greater than calculated value.  Go back and get the list of row#s that overlap with that
 // create the column NPAR values at a time
 allmin=_mm256_mul_pd(allmin,_mm256_set1_pd(1.0000000000000003));  // add 2 ULPs in DP
 A conten; GA0(conten,INT,m,0) I *conten0=IAV0(conten), conteni=0;  // get address of place to store contenders
 for(i=i0;i<i0e;i+=NPAR){
  // get the validity mask and process: leave as ones until the end of the column
  __m256d ck4, bk4;  // values we are working on
  if(likely(i0e-i>=NPAR)){
   ck4=_mm256_loadu_pd(&ck0[i]); bk4=_mm256_loadu_pd(&bk0[i]);  // read in ck and bk
  }else{
   ck4=_mm256_maskload_pd(&ck0[i],endmask); bk4=_mm256_maskload_pd(&bk0[i],endmask);  // read in ck and bk.  endmask is set from earlier loop
  }
  if(!_mm256_testz_si256(_mm256_castpd_si256(ck4),_mm256_castpd_si256(ck4))){  // if all c values are 0.0, we can skip everything - not unlikely
   // see if any of the new values is limiting: b/c < min SPR, c>ColThresh, bk>bkthresh
   // we perform this calculation on each lane separately, to save the expense of finding the minimum SPR each time one is calculated.
   // This means the values in each lane will be smaller than they would be with sharing, leading to perhaps 2 extra divisions.  That's a good trade.
   __m256d ratios=_mm256_fmsub_pd(allmin,ck4,bk4);  // allmin*c-b: sign set if this is NOT a new min SPR in its lane
   __m256d coverthresh=_mm256_cmp_pd(ck4,ckthresh,_CMP_GT_OQ);  // set to ~0 if c>ColThresh - never if endmask 0
   // we separate b into < threshold and >= threshold.   This must match the selection made for stall exit
   __m256d sprvalid=_mm256_and_pd(coverthresh,_mm256_cmp_pd(bk4,bkthresh,_CMP_GE_OQ));  // compare sets to ~0 if b>=bthresh - never if endmask is 0   set to ~0 if c>thresh & b>=thresh, i. e. SPR is valid
   if(!_mm256_testc_pd(ratios,sprvalid)){  // CF is 1 if ~a&b signs all 0 => ~(~new min)&(c good&b good) all 0 => no place where new min & c good & b good
    // at least one new SPR has overlap.  Remember the numbers
    I contenmsk=_mm256_movemask_pd(_mm256_andnot_pd(ratios,sprvalid));  // bitmask indicating overlap positions
    do{conten0[conteni++]=i+CTTZI(contenmsk); contenmsk&=contenmsk-1;}while(contenmsk);  // copy the indexes of the set bits one by one
   }
  }
 }  // end of loop to find contenders

 I zrow;  // the row number containing the minimum SPR - our result
 if(likely(((1-conteni)&(1-AR(ck)))>=0))zrow=conten0[0];  // usually there is 1 contender.  And if we don't have QP, we'll just take a good one (might not be best)
 else{
  // there are contenders, and we must pick the best, by QP cross-multiplies
  // cull those to best row in each lane, in QP, and then to a single lane
  __m256d ckbh=_mm256_setzero_pd(), ckbl=ckbh, bkbh=_mm256_set1_pd(1.0), bkbl=bkbh;  // current 'best' values in each lane - init to b/c=infinity
  __m256d ck4h, ck4l, bk4h, bk4l;  // new values to try
  __m256i rownums;  // row numbers we are fetching from in the current iteration 
  __m256d limitrows=_mm256_setzero_pd();  // row numbers of lowest SPRs.  Init not needed 
#define keepbest { /* ckb* and ck4* are loaded.  In each lane, leave the smaller SPR in ckb* and limitrows */ \
 /* we see if b4/c4 < bb/cb, where if b4=c4=0 it must NOT compare lower, ever.  Take b4*cb < c4*bb */ \
 __m256d b4cbh, b4cbl, c4bbh, c4bbl, t0, t1;  /* quad-precision cross products */ \
 TWOPROD(bk4h,ckbh,t0,t1)  /* b4 * cb high parts */ \
 t1=_mm256_fmadd_pd(bk4h,ckbl,t1);  /* bh * cl into low part */ \
 t1=_mm256_fmadd_pd(bk4l,ckbh,t1);  /* bl * ch into low part */ \
 TWOSUMBS(t0,t1,b4cbh,b4cbl)  /* convert to canonical form.  We ignore low*low, which costs us 1 bit */ \
 TWOPROD(ck4h,bkbh,t0,t1)  /* c4 * bb high parts */ \
 t1=_mm256_fmadd_pd(ck4h,bkbl,t1);  /* ch * bl into low part */ \
 t1=_mm256_fmadd_pd(ck4l,bkbh,t1);  /* cl * bh into low part */ \
 TWOSUMBS(t0,t1,c4bbh,c4bbl)  /* convert to canonical form.  We ignore low*low, which costs us 1 bit */ \
 /* set t0 to 1s if b4*cb < c4*bb */ \
 /* we rely on consistent rounding: it must never happen that qp a>b and high(a)<high(b) */ \
 t0=_mm256_or_pd(_mm256_cmp_pd(b4cbh,c4bbh,_CMP_LT_OQ),_mm256_and_pd(_mm256_cmp_pd(b4cbh,c4bbh,_CMP_EQ_OQ),_mm256_cmp_pd(b4cbl,c4bbl,_CMP_LT_OQ))); /* < hi, or = hi < lo */ \
 ckbh=_mm256_blendv_pd(ckbh,ck4h,t0); ckbl=_mm256_blendv_pd(ckbl,ck4l,t0); bkbh=_mm256_blendv_pd(bkbh,bk4h,t0); bkbl=_mm256_blendv_pd(bkbl,bk4l,t0);   /* keep the winner */ \
 limitrows=_mm256_blendv_pd(limitrows,_mm256_castsi256_pd(rownums),t0);  /* also remember the row */ \
}

  endmask=_mm256_setone_epi64(); // mask for validity of next 4 words: used to control fetch from indexes and column
  for(i=0;i<conteni+LGNPAR*NPAR;i+=NPAR){  // we go through the loop for all inputs, then twice more to combine lanes
   if(i<conteni){  // the first loops read new values; the last combine lanes
    // get the validity mask and process: leave as ones until the end of the column
    if(likely(conteni-i>=NPAR)){
     rownums=_mm256_loadu_si256((__m256i*)&conten0[i]);
    }else{
     endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-(conteni-i)));
     rownums=_mm256_maskload_epi64(&conten0[i],endmask);
    }
    ck4h=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),ck0,rownums,_mm256_castsi256_pd(endmask),SZI);
    ck4l=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),ck0+m,rownums,_mm256_castsi256_pd(endmask),SZI);
    bk4h=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),bk0,rownums,_mm256_castsi256_pd(endmask),SZI);
    bk4l=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),bk0+m,rownums,_mm256_castsi256_pd(endmask),SZI);
   }else if(i<conteni+NPAR){  // next-last iteration
     ck4h=_mm256_permute_pd(ckbh,0b0101); ck4l=_mm256_permute_pd(ckbl,0b0101); bk4h=_mm256_permute_pd(bkbh,0b0101); bk4l=_mm256_permute_pd(bkbl,0b0101);  rownums=_mm256_castpd_si256(_mm256_permute_pd(limitrows,0b0101));  // compare within lanes
   }else{  // last iteration
     ck4h=_mm256_permute4x64_pd(ckbh,0b01001110); ck4l=_mm256_permute4x64_pd(ckbl,0b01001110); bk4h=_mm256_permute4x64_pd(bkbh,0b01001110); bk4l=_mm256_permute4x64_pd(bkbl,0b01001110);  rownums=_mm256_castpd_si256(_mm256_permute4x64_pd(limitrows,0b01001110));  // compare across lanes
   }
   keepbest
  }  // end of loop to pick best contender

  zrow=_mm256_extract_epi64(_mm256_castpd_si256(limitrows),0);  // remember the winning row

 }  // end 'there are contenders'
 // compare our best row against the incumbent in the ctx, and update the incumbent if the new value is an improvement
 while(1){  // till we succeed in writing or we realize we don't have the best
  I incumbent=__atomic_load_n(&ctx->row,__ATOMIC_ACQUIRE);  // get the current best row
  if(incumbent>=0){  // if there is no incumbent, we know we beat it
   __m256d ckbh=_mm256_set1_pd(ck0[incumbent]), ckbl=_mm256_set1_pd(ck0[m+incumbent]), bkbh=_mm256_set1_pd(bk0[incumbent]), bkbl=_mm256_set1_pd(bk0[m+incumbent]);  // incumbent 'best' values in each lane
   __m256d ck4h=_mm256_set1_pd(ck0[zrow]), ck4l=_mm256_set1_pd(ck0[m+zrow]), bk4h=_mm256_set1_pd(bk0[zrow]), bk4l=_mm256_set1_pd(bk0[m+zrow]);  // new values to try
   __m256d limitrows=_mm256_castsi256_pd(_mm256_set1_epi64x(incumbent));  // row numbers of lowest SPRs to date
   __m256i rownums=_mm256_set1_epi64x(zrow);  // row numbers of new SPR
   keepbest  // see if we keep our value
   if(zrow!=_mm256_extract_epi64(_mm256_castpd_si256(limitrows),0))break;  // if we are not the best, exit loop, we're done
  }
  // we have the best.  Try to install it in row
  if(__atomic_compare_exchange_n(&ctx->row,&incumbent,zrow,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))break;  // write; if written exit loop, otherwise reload incumbent
 }
 R 0;
}


// 128!:13  find minimum pivot ratio in quad precision  OR  take reciprocal
// w is ck;bk;ColThresh,bkthresh  or   ck value
// result is row#,reciprocal of pivot hi,lo
F1(jtfindspr){F12IP;
 ARGCHK1(w);
 I row;  // will hold the row of the SPR
 D *ckd;  // pointer to column data
 I isqp;  // will be set if there is a qp value
 I m;  // number of items in ck
 // extract the inputs
 if(AT(w)&FL){
  // reciprocal-only case
  ASSERT(AR(w)<=1,EVRANK) ASSERT(AR(w)==0||AS(w)[0]==2,EVLENGTH)  // must be atom or 2-atom list
  isqp=AR(w); ckd=DAV(w);
  A z; GAT0(z,FL,2,1)
  __m256d pivhi=_mm256_set1_pd(ckd[0]), reciphi=_mm256_set1_pd(1.0/ckd[0]);
  if(isqp){
   __m256d pivlo=_mm256_set1_pd(ckd[1]), reciplo=_mm256_set1_pd(0.0), t0, t1, prodh, prodl;
   DO(2,  TWOPROD(reciphi,pivhi,t0,t1) t1=_mm256_fmadd_pd(reciplo,pivhi,t1); t1=_mm256_fmadd_pd(reciphi,pivlo,t1); TWOSUMBS(t0,t1,prodh,prodl)   // qp piv * recip - will be very close to 1.0
          t0=_mm256_sub_pd(_mm256_sub_pd(_mm256_set1_pd(1.0),prodh),prodl);  // - (amount the product is too high).  1-prodh gives exact result, commensurate with prodl
          t0=_mm256_fmadd_pd(reciphi,t0,reciplo); t1=reciphi; TWOSUMBS(t1,t0,reciphi,reciplo)  // -(toohigh)*recip(hi) gives correction to recip: add it in
   )
   DAV1(z)[1]=_mm256_cvtsd_f64(reciplo);  // lower recip
  }else DAV1(z)[1]=0.0;
  DAV1(z)[0]=_mm256_cvtsd_f64(reciphi);  // always write upper/only recip
  RETF(z);  // return qp reciprocal
 }else{
  // search for the best SPR
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==3,EVLENGTH)  // w is 3 boxes
  A box0=C(AAV(w)[0]), box1=C(AAV(w)[1]), box2=C(AAV(w)[2]);
  A ck=box0; ASSERT(AT(ck)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(ck),1,2),EVRANK) ASSERT(AR(ck)==1||AS(ck)[0]==2,EVLENGTH)  // ck is FL list or 2xn table
  isqp=!(AR(ck)==1); ckd=DAV(ck);  // remember if qp, and address of the column data
  A bk=box1; ASSERT(AT(bk)&FL,EVDOMAIN) ASSERT(AR(bk)==AR(ck),EVRANK) ASSERT(AS(bk)[0]==AS(ck)[0],EVLENGTH) ASSERT(AR(bk)==1||AS(bk)[1]==AS(ck)[1],EVLENGTH)  // bk is float list or 2xn table, must match ck
  A thresh=box2; ASSERT(AT(thresh)&FL,EVDOMAIN) ASSERT(AR(thresh)==1,EVRANK) ASSERT(AN(thresh)==2,EVLENGTH)  // thresh is 2 floats

  // do the work
  m=AS(ck)[AR(ck)-1];   // length of a column
  // figure out how many threads to use, how many lines to take in each one
#define TASKMINATOMS ((2*2000)/2)  // TUNE Values will be in cache.  Normal DP comp is 2 clocks per atom.  We don't want to start a task with less than 2000 clocks, so insist on twice that many
  I nthreads=(*JT(jt,jobqueues))[0].nthreads+1;  // the number of threads we would like to use (including master), init to # available
  I rowsperthread=m;  // will be #rows each processor should take
  if(((1-nthreads)&(TASKMINATOMS-rowsperthread))>=0)nthreads=1;  // if only one thread, or job too small, use just one thread
  else{
   // big enough for multithreading.
   rowsperthread=(m+nthreads-1)/nthreads;  // number of threads per processor, rounded up
  }
#undef TASKMINATOMS

  // init the context
#define YC(n) .n=n,
struct sprctx opctx={YC(rowsperthread)YC(ck)YC(bk)YC(m).ckthreshd=DAV(thresh)[0],.bkthreshd=DAV(thresh)[1]};
#undef YC

  // run the operation
  opctx.row=-1;  // init row# to invalid.  Improving threads will update it
  if(nthreads>1)jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtfindsprx,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
  else jtfindsprx(jt,&opctx,0);  // the one-thread case is not uncommon and we avoid thread overhead then

  row=opctx.row;
  // if no SPR was found, signal error.  This is possible only if the only pivots were near-threshold in dp and below threshold in qp
  ASSERT(opctx.row>=0,EVLIMIT);
  RETF(sc(row))  // return the row#
 }
}
#else
F1(jtfindspr){F12IP;ASSERT(0,EVNONCE);}
#endif


#if (C_AVX2 || EMU_AVX2)

// everything we need for one core's execution
#define RINGROWS 64  // must be power of 2
#define RINGCOLS 256  // max width of a stripe - must allow mask to fit in 8 bytes
#define MAXOP 100  // max # ops - must match with the value in xa.c
#define MAXNON0 128  // max # non0 values in a stripe
#define LGRESBLKE 2  // number of Es in a result block
#define RESBLKE ((I)1<<LGRESBLKE)
#define B8ROWS 8  // number of rows of ring processed before release

struct __attribute__((aligned(CACHELINESIZE))) bopctx {
 I4 nstripes;  // number of stripes in the input
 I4 qktnrows, qktncols;  // shape of Qkt
 US nops;  // number of outer products
 US nthreads;  // number of threads
 US debopts;  // nt control
 // arguments
 
 A *oprowmasks;  // AAV(rowmask), each box holding a mask or a list of indexes of nonzeros
 A *oprowvals;   // AAV(non0 rowvalues), which scatter-map to the rows of Qkt - each on a cacheline boundary
 A *opcolmasks;  // AAV(colmask), each box holding a mask or a list of indexes of nonzeros
 A *opcolvals;   // AAV(non0 colvalues), which scatter-map to the cols of Qkt - each on a cacheline boundary
 I4 *(*colndxs)[MAXOP];  // pointers to column indexes, each having one value per stripe.  filled in by threads
 I (*stripestartend1)[][2];  // start and end+1 for each stripe [][2]
 I4 (*opstripebsum)[][MAXOP];  // number of 1 bits in mask[0..stripe][op] - leading 0 omitted
 I *stripegrade;  // downgrade of computational weight of stripes, so we take biggest first
 
 E *qkt;    // pointer to the array - must be cache-aligned and cache-multiple in each row
 D threshold;  // Result |values| below this are clamped to 0
 // atomics, in new cacheline
 I4 colndxct;  // next column index to create, initialized to nthreads
 I4 colndx9ct;  // early completion of column indexes (first 9 values created), inited to nopct and decremented
 I4 resvx;   // next stripe to reserve, initialized to nthreads
};

 typedef struct opi {
  struct opi *chain;  // &next active op
  UI8 rbmask;   // mask of resultblocks in each row touched by this op
  E colvalahead;
  I4 *acolndxs;  // addr of start of col indexes, one per value in acolvals
  E *acolvals;  // addr of start of nonzero values in column
  UI8 endofsts;  // 4 US values: (index of start of offsets/vals in stripe*)/end4/end2/end1 offsets
  I4 colndxahead;  // next col index
  I4 rowindex;  // index into (*acol(ndx|val)s) of the next position in this column
 } opinfo;  // info for each op

#define HIGHVALUEI4 (((UI4)~0)>>1)

// the processing loop for one core.  We take stripes, in order of decresing computational weight
static unsigned char jtbatchopx(J jt,struct bopctx* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
#define RYC(x) RESTRICT YC(x)
 YC(nops)YC(nthreads)YC(nstripes)YC(qktnrows)YC(qktncols)YC(debopts)RYC(oprowmasks)RYC(oprowvals)RYC(opcolmasks)RYC(opcolvals)RYC(colndxs)RYC(stripestartend1)RYC(opstripebsum)RYC(stripegrade)RYC(qkt)
#undef YC
 __attribute__((aligned(CACHELINESIZE))) E ring[RINGROWS][RINGCOLS];   // the ring - 1/2 of D2$
 __m256d zerothresh=_mm256_set1_pd(ctx->threshold);  // near0 threshold

 opinfo opstat[MAXOP];  // collected info about the ops in the current stripe
 US stripeofst[MAXOP*MAXNON0+3];  // byte offset to each non0 value in the stripe, for loading the ring value.  Allocated in groups of 4.  +3 because of overstore
 __attribute__ ((aligned (CACHELINESIZE))) E stripe0213[MAXOP*MAXNON0+3];  // row values for stripe, 0213, possible overstore

 // values in the stripe for each op, in format needed
 __m256d *releaseqktringbase[RINGROWS];  // base of Qkt region mapped to ring.  Must be on RBLOCK boundary; low bits are the real ring row containing the values.  rows are shuffled to compact empty rows.
 UI8 releaserowmask[RINGROWS];  // mask of all the resultblocks that have been modified in this line  Must be cleared after each release, i. e. 0 when starting new 8block

 // Before calculating ops, the threads need to calculate indexes for each op, to wit (1) for each stripe, the number of values whose indexes are before the stripe; (2) the indexes of the non0s in the column.
 // The entire stripe index is needed before any calculations can be made, but we can get started with the first 8block as soon as we have the first 9 column indexes of the ops in a stripe.  We do the 2 tasks
 // per op by arbitrating for them (using thread index the first time); we signal first completion when we have written 9 column indexes in the column, and then final completion after finishing the column index.
 // Each task can run its first 8block after first completion, and then must wait for final completion for the rest.
 I opno=ti;  // op# to create the index for.  First one is our thread#
 while(opno<2*nops){A ma;  // if the first reservation is too high, we have more threads than ops.  skip it then
  // calculate column index for the op - the offset into mask/cols as we process each stripe
  if(opno<nops){
   // first job: calculate stripe index for the op
   I bsumtodate=0; C *mask=CAV(oprowmasks[opno]);  // total 1s found, running pointer to mask
   DO(nstripes-1, I start=(*stripestartend1)[i][0], end=(*stripestartend1)[i][1]-SZI, bsum=0; while(start<end){bsum+=*(I*)&mask[start]; start+=SZI;} bsum+=*(I*)&mask[start]<<(start==end?0:BW/2);
     bsum+=bsum>>32; bsum+=bsum>>16; bsum+=bsum>>8; bsumtodate+=(C)bsum; __atomic_store_n(&(*opstripebsum)[i][opno],bsumtodate,__ATOMIC_RELEASE);)  // add; if last word incomplete, it must have exactly 32 bits
   __atomic_store_n(&(*opstripebsum)[nstripes-1][opno],AN(oprowvals[opno]),__ATOMIC_RELEASE);  // infer the length of the last stripe so that we don't have to mask garbage bytes if the mask is short
   __atomic_fetch_add(&ctx->colndx9ct,-1,__ATOMIC_ACQ_REL);  // the stripe is required for early release
  }else{
   opno-=nops;  // convert back to op#
   I ncvals=AN(opcolvals[opno]); __m256i *cmask=(__m256i*)BAV(opcolmasks[opno]);  // # non0s left in op column, pointer to mask
   GATV0(ma,INT4,ncvals+1,1)  // allocate space for index (incl 1 sentinel), on cacheline boundary
   I4 *mav=I4AV1(ma), *mavn=mav+9; __atomic_store_n(&(*colndxs)[opno],mav,__ATOMIC_RELEASE);  // Get address of index, publish the address to other threads.  Lots of false sharing on this store!
   I fetchndx=0;  // index of first bit in this word
   while(1){  // till we have stored all the index
    I bits=(UI)(UI4)_mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_loadu_si256(cmask),_mm256_setzero_si256())); ++cmask;  // fetch and step over next 32 bits.  Overfetch possible and OK
    while(bits){*mav++=fetchndx+CTTZI(bits); if(unlikely(mav==mavn))__atomic_fetch_add(&ctx->colndx9ct,-1,__ATOMIC_ACQ_REL); bits&=bits-1; if(--ncvals<=0)goto finndx;}  // store index for every non0, stop when all done.  Signal first completion after prefix
    fetchndx+=sizeof(*cmask);  // advance to next mask base, adding 1 for each bit we read
   }  finndx:;
   if(unlikely(mav<mavn))__atomic_fetch_add(&ctx->colndx9ct,-1,__ATOMIC_ACQ_REL); *mav=HIGHVALUEI4;  // Signal first completion if we haven't already; install sentinel at end
  }

  opno=__atomic_fetch_add(&ctx->colndxct,1,__ATOMIC_ACQ_REL);  // reserve next row.  Every thread will finish with one failing reservation, which may be the first one
 }

 // initialize internal areas while we wait for indexes to settle.
 mvc(sizeof(ring),ring,MEMSET00LEN,MEMSET00);  // clear the ring to all 0.0
 mvc(sizeof(releaserowmask),releaserowmask,MEMSET00LEN,MEMSET00);  // clear the release info to 0
 DONOUNROLL(RINGROWS, releaseqktringbase[i]=(__m256d*)i;)

 DONOUNROLL(nops, opstat[i].acolvals=EAV(opcolvals[i]);)  // get pointer to values in each column
 __m256d sgnbit=_mm256_castsi256_pd(_mm256_set1_epi64x(Iimin));  // 0x8..0

 I yd=10; while(__atomic_load_n(&ctx->colndx9ct,__ATOMIC_ACQUIRE)!=0){johnson(20*nthreads); if(--yd<0){YIELD};}  // delay until we have early completion on each op.  We allow 1 read every 20ns

 DONOUNROLL(nops, opstat[i].acolndxs=(*colndxs)[i]; if((*colndxs)[i]==0)SEGFAULT;)  // scaf  get start address of index for each op.  This is generally coming from another core so no reason to unroll

 I stripex=ti;  // initial stripe reservation, from thread#
 // state needed to release one row of ring (viz relstart). 
#define RELEASEBLOCKCT 8  // number of RBLOCKS to handle at a time.  This should be as big as we can make it without filling write buffers. 2 blocks=1 cacheline
 UI releaseblockmask=0; I releasect; __m256d *releaseqktringbasecurr;  // mask of blocks in row, index of row being released, normal burst length, amount of processing before next burst, actual burst length, Qkt addr of burst
#define CYCBETWEENRELEASE0 500  // estimated clocks to receive DRAM data.  We try to burst only this often so that write buffers can drain
#define CYCPERINSERT 12   // number of cycles per insertion
#define RELEASEDELAYCT0 ((CYCBETWEENRELEASE0/CYCPERINSERT)*sizeof(US))  // unbiased delay, measured in #offsets processed
I releasedelaythreaded=RELEASEDELAYCT0*20/MIN(20,MAX(5,nthreads)), releasedelayct0;  // delay after considering thread count, biased delay given ring status
 // portmanteau register holding ring status
#define RINGDCTX 48  // bit position of delayct, which counts USs and goes positive when it is OK to release RELEASEBLOCKCT blocks.  Ends at sign bit
#define RINGDCTMASK 0xffff000000000000
#define RINGFINALSYNC 0x40000  // set when we have waited for final completion of all op indexes
#define RINGNNTX 16  // non-non-temporal flags: for read and write.  0 (default) means non-temporal, 1=cached
#define RINGNNTREAD ((I)1<<RINGNNTX)
#define RINGNNTWRITE ((I)2<<RINGNNTX)
#define RINGACCREQD 0x8000  // set for ops after first, which require adding into the ring
#define RINGRELSTARTX 8  // bit position of relstart, the next/current row of the ring to relesse
#define RINGRELSTARTMASK ((RINGROWS-1)<<RINGRELSTARTX)
#define RINGRELSTARTWRAP (~(RINGROWS<<RINGRELSTARTX))   // mask to wrap relstart when incremented
#define RINGB8STARTX 0   // start of ring area where next 8block is built AND end+1 pointer of data released to ring
#define RINGB8STARTMASK ((RINGROWS-1)<<RINGB8STARTX)
 I ringdctrlb8=(debopts<<RINGNNTX)&(RINGNNTREAD+RINGNNTWRITE);  // delay count, nt flags, relstart, b8start
    // when relstart==b8start, ring is empty.  releaseblockmask is always 0 then & only then.  releasedelayct is set negative to delay the next batch; it increments as blocks are put into the ring.
    // releasect counts the actual burst length, which is RELEASEBLOCKCT unless the ring is full or processing is over, in which case it is set to high-value to flush the ring
 while(stripex<nstripes){  // ... for each reservation...
  I stripe=stripegrade[stripex];  // get the actual stripe# to process

  // establish the values for each op in this stripe
  I io, sstart=(*stripestartend1)[stripe][0];   // start col for stripe
  I4 minnextrow=HIGHVALUEI4;  //  starting row for next 8batch
  E *qktcol=qkt+sstart;  // address of the output column in row 0
  opinfo *aops=0;  // chain of active ops, init to empty
  I nextopofstvalx=0;  // index of starting offset/val of next op in this stripe
  for(io=0;io<nops;++io){   // for each op:
   I stripeval0x=unlikely(stripe==0)?0:(*opstripebsum)[(stripe-1)][io];  // starting index of offsets/values in this stripe
   I ssize=(*opstripebsum)[stripe][io]-stripeval0x;  // number of non0 values in this stripe
if(!BETWEENC(ssize,0,MAXNON0))SEGFAULT;   // scaf
   if(ssize){    // if this op intersects this stripe...
    opstat[io].rowindex=0; I4 nx=opstat[io].colndxahead=opstat[io].acolndxs[0]; opstat[io].colvalahead=opstat[io].acolvals[0];  // start on first row; prefetch first.
    minnextrow=nx<minnextrow?nx:minnextrow; // Keep track of smallest start value.  The readahead will take a long time, but we do not use minnextrow in this loop 
    // read the mask for this op and convert it to offsets into ring and mask of resultblocks
    opstat[io].chain=aops; aops=&opstat[io];  // add this op to the active chain
    opstat[io].rbmask=0;  // init mask of touched blocks
    // Go through the mask, classifying each set bit into a run of 4, a run of 2, or a run of 1.
    US non0pos[MAXNON0+RESBLKE];  // for each non0, the position in the row it came from; top 2 bits are lgsz.  Index of entry implies its position in the compacted values
    UI8 sizn=0;  // for each size, the number of values found so far at that size.  3 packed US values.  We don't use an array because the carried dependency of an update is too long
    C *smask=BAV(oprowmasks[io])+sstart;  // start of bitmask for this stripe in this op.  The offset is the position in the mask
    UI8 rbmask=0;  // accumulator for mask of touched resblks
    UI bitvec; I nvalsfnd=0, bitofst=0;  // vector of bits; number of values found so far; offset for bit 0 in bitvec;
    while(1){
     bitvec=(UI)(UI4)_mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_loadu_si256((__m256i*)(smask+bitofst)),_mm256_setzero_si256()));  // read next 32 bits
     I nrembits=32;  // #bits left in bitvec.  Bits past the last value are invalid
     while(1){
      if(bitvec==0){bitofst+=nrembits; break;}  // no more bits in this section, advance to next
      I low1=CTTZI(bitvec); bitvec>>=low1; bitofst+=low1; nrembits-=low1;   // shift out low 0s
      I vallimit=MIN(ssize-nvalsfnd,4); I validbits=bitvec&~(~0<<vallimit);   // max # 1s that can be valid; discard bits that must be after the last valid value
      I sizefoundx=(validbits>>1)&1; sizefoundx=validbits==0b1111?2:sizefoundx; I sizefound=1LL<<sizefoundx;   // sizefound is 1/2/4, # consecutive low bits found
      US sizepos=(sizefoundx<<14)+bitofst; non0pos[nvalsfnd]=sizepos; non0pos[nvalsfnd+1]=sizepos+1; non0pos[nvalsfnd+2]=sizepos+2; non0pos[nvalsfnd+3]=sizepos+3;  // add 4 copies to result in case we need them
      if(unlikely(sizefound+nvalsfnd==ssize)){  // if this block exhausts the non0 values, remember them and exit
       sizn+=sizefound<<(sizefoundx<<4); nvalsfnd+=sizefound;  // count # vals at this size, step over the valid ones
       rbmask|=((((bitofst&(RESBLKE-1))+(sizefound-1))>>(LGRESBLKE-1))|1)<<(bitofst>>LGRESBLKE);   // flag resblks touched by this block
       goto finclass;
      }
      if(unlikely((sizefound&3)==nrembits))break;  // if we find 1 or 2 bits exactly at the end of the block, don't match them in case a 4-block straddles the boundary
      sizn+=sizefound<<(sizefoundx<<4); nvalsfnd+=sizefound;    // count # vals at this size, step over them
      rbmask|=((((bitofst&(RESBLKE-1))+(sizefound-1))>>(LGRESBLKE-1))|1)<<(bitofst>>LGRESBLKE);   // flag resblks touched by this block.  Close question whether to do this here or one values at a time.  We choose here because
               // the carried dependency on bitvec is 11 clocks and we need work to fill the time in the loop 
      bitvec>>=sizefound; bitofst+=sizefound; nrembits-=sizefound;  // discard the bits from the block
     }
    }
finclass:;  // we have classified the blocks
    opstat[io].rbmask=rbmask;  // save touched blocks in op/stripe
    //  non0pos now gives the blocksize and input position of each non0 value.  Put the offsets in order in stripeofst and the values in order in stripe0123
    US *offsetsbase=&stripeofst[nextopofstvalx]; E *valsbase0213=&stripe0213[nextopofstvalx];  // start of offsets for this stripe, start of values in 0213 order
    E *valsbasein=&EAV(oprowvals[io])[stripeval0x];  // position of first compacted value for this stripe
    UI8 locn=(sizn>>16)+(sizn>>32);  // starting position in each size, running total of sizes.  3 packed US values.  We don't use an array because the carried dependency of an update is too long
    DO(nvalsfnd, I lgsz=non0pos[i]>>10; I rowofst=(non0pos[i]&0x03ff)*sizeof(E); I slot=(locn>>lgsz)&0x3ff; locn+=(UI8)1<<lgsz; offsetsbase[slot]=rowofst; valsbase0213[slot]=valsbasein[i];)  // assemble values and byte-offsets into Qk
    // square the offsets up to RESBLK boundaries.
    if(unlikely((sizn&0xffff)==0)){
     // there are no 1-blocks.  If there are an odd# 2-blocks, replicate the last one (we always replicate, then change only if odd)
     I locn1=(locn>>16)&0xffff;  // index of end+1 2-block
     offsetsbase[locn1]=offsetsbase[locn1-2]; offsetsbase[locn1+1]=offsetsbase[locn1-1]; valsbase0213[locn1]=valsbase0213[locn1-2]; valsbase0213[locn1+1]=valsbase0213[locn1-1]; locn+=0x20002; 
    }else{
     // If there are 1-3 1-blocks replicate (we always replicate, change # only if remnant)
     I locn0=locn&0xffff;  // index of end+1 1-block
     offsetsbase[locn0]=offsetsbase[locn0+1]=offsetsbase[locn0+2]=offsetsbase[locn0-1]; valsbase0213[locn0]=valsbase0213[locn0+1]=valsbase0213[locn0+2]=valsbase0213[locn0-1]; locn+=3;
    }
    locn&=0xfffcfffcfffc;  // put all blocks on a 4-boundary.  This is subtle!  If there is an odd 2-block (which must be followed by a 1-block), it converts the odd 2-block to a 1-block.
    // save info for this op and advance to the next
    opstat[io].endofsts=(nextopofstvalx<<48)+(locn*sizeof(US)); nextopofstvalx+=locn&0xffff;  // save startoffset/end4/end2/end1 (converted from index to US-offset), and advance nextstripx by end1 to get to next op
   }
  }
  // convert the values from all ops to 0213 order, in place
 DO(nextopofstvalx>>2, __m256d h0l0h1l1=_mm256_loadu_pd((D*)&stripe0213[RESBLKE*i]); __m256d h2l2h3l3=_mm256_loadu_pd((D*)&stripe0213[RESBLKE*i+2]); 
     _mm256_storeu_pd((D*)&stripe0213[RESBLKE*i],_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000)); _mm256_storeu_pd((D*)&stripe0213[RESBLKE*i+2],_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111));)

  // we keep all ops in a single list, removing ones that have been fully processed.   It might be right to keep a second list with ops that have not entered yet, heaped on row of entry.  Depends on overlap.

  // ops ready, now calculate the entire stripe.  The ops are visited in reverse order, for comp ease
  I releasedelaythreadedstripe=aops->chain==0?0:releasedelaythreaded;  // throttle info for this stripe.  If only 1 active OP, don't throttle anything
  while(aops){  // keep processing rows as long as there is an active op.  minnextrow is set with the starting row for the new 8block
   // loop to process one 8block
   I b8qktrow=minnextrow; minnextrow=HIGHVALUEI4;  // remember starting row# in qkt = start of 8 window
   opinfo *currop, *prevop;  // op being processed, prev ptr used to delete blocks
   for(ringdctrlb8&=~RINGACCREQD,currop=aops,prevop=0;currop;ringdctrlb8|=RINGACCREQD){  // each op, with ACCREQD set in all but the first
    I4 nextrowinop;  // next row to process in this op
    I4 rowindex=currop->rowindex;  // index of the values we read ahead in this op
    I endofsts=currop->endofsts; US *aof=&stripeofst[endofsts>>48]; __m256d *ava=(__m256d*)&stripe0213[endofsts>>48];  // offset/end4/end2/end1; base of offsets/values for each row of the stripe/op 
    while((nextrowinop=currop->colndxahead)-b8qktrow<B8ROWS){  // if next row is in 8block
     // next row can be processed in this 8block.  Load the column info and update the readahead
     I releasex=(ringdctrlb8+(nextrowinop-b8qktrow))&RINGB8STARTMASK;  // index of release slot for this row, which holds the mapping to actual ring row and the mask
     I ringx=(I)releaseqktringbase[releasex];  // ring row to fill - Qkt part is always 0 here
     E *r0=ring[ringx];  // base the offsets will be applied against
     releaserowmask[releasex]|=currop->rbmask;  // make note of the resultblocks that will be modified by this row
     __m256d colh=_mm256_set1_pd(currop->colvalahead.hi), coll=_mm256_set1_pd(currop->colvalahead.lo);  // copy column value into all lanes
     currop->colndxahead=currop->acolndxs[++rowindex]; currop->colvalahead=currop->acolvals[rowindex];  // read ahead for next row
     I andx=0;  // counts in steps of RESBLKE*sizeof(one offset).  With this stride we can use andx to point to offsets and andx*sizeof(E)/sizeof(one offset) (=8) to point to row values
     // Calculate one row of the op
     I alen;  // ending offset for a section
     // the 4-blocks
     for(alen=(endofsts>>32)&0xffff;andx<alen;andx+=(RESBLKE*sizeof(aof[0]))){__m256d h0l0h1l1,h2l2h3l3;  // temps needed
      I o0=((US*)((C*)aof+andx))[0];  // offsets to ring values to accumulate into
      __m256d rowh=_mm256_load_pd((D*)((C*)ava+andx*sizeof(E)/sizeof(aof[0]))), rowl=_mm256_load_pd((D*)((C*)(ava+1)+andx*sizeof(E)/sizeof(aof[0])));  // the outer-product values
      __m256d iph,ipl,isl;  // intermediate products and sums
      // multiply
      TWOPROD(rowh,colh,iph,ipl)  // (rowh,colh) to high precision
      ipl=_mm256_fmadd_pd(rowh,coll,ipl); ipl=_mm256_fmadd_pd(rowl,colh,ipl);  // accumulate middle pps
      if(ringdctrlb8&RINGACCREQD){   // normally we have to add the product into the incumbent ring value
       // accumulate into Qk
       h0l0h1l1=_mm256_loadu_pd((D*)((I)r0+o0)); h2l2h3l3=_mm256_loadu_pd((D*)((I)r0+o0+sizeof(__m256d)));   // read accumulands 0123 
       __m256d h0h2h1h3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), l0l2l1l3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // convert to 0213 order, hi & lo
       // Because we added 3 low-order values (with the same shift), we are limiting precision to 104 bits
       // Now add in the incumbent Qk value
       // Do high-precision add of h0h2h1h3 and iph.  If this decreases the absvalue of h0h2h1h3, we will lose precision because of insufficient
       // bits of qkv.  If this increases the absvalue of h0h2h1h3, all of l0l2l1l3 will contribute and the limit of validity will be
       // from the product.  In either case it is safe to accumulate all the partial products and ipl into l0l2l1l3
       l0l2l1l3=_mm256_add_pd(l0l2l1l3,ipl);  // the middle pps.  low*low will never contribute unless value is exhausted & thus noise
       TWOSUM(iph,h0h2h1h3,iph,ipl)   // combine the high parts
       ipl=_mm256_add_pd(ipl,l0l2l1l3);  // add the combined low parts
       // At this point h0h2h1h3/l0l2l1l3 are a good high-precision product, with all the significance we can get, but it is possible that h<l if there was massive cancellation.  Even in that case,
       // the next add will restore h>l except in the case when the next h is >h but <l.  In that case, h can remain <l but there is no loss of significance, which is still limited by l.
       // Even though h>l, they may be close in magnitude.  After the final accumulation into Qkt we will ensure h>>l
      }
      // store results.
      h0l0h1l1=_mm256_shuffle_pd(iph,ipl,0b0000); h2l2h3l3=_mm256_shuffle_pd(iph,ipl,0b1111);  // convert result to E order
      _mm256_storeu_pd((D*)((I)r0+o0),h0l0h1l1); _mm256_storeu_pd((D*)((I)r0+o0+sizeof(__m256d)),h2l2h3l3);    // store results
     }
     // the 2-blocks
     for(alen=(endofsts>>16)&0xffff;andx<alen;andx+=(RESBLKE*sizeof(aof[0]))){__m256d h0l0h1l1,h2l2h3l3;  // temps needed
      I o0=((US*)((C*)aof+andx))[0]; I o2=((US*)((C*)aof+andx))[2];  // offsets to ring values to accumulate into
      __m256d rowh=_mm256_load_pd((D*)((C*)ava+andx*sizeof(E)/sizeof(aof[0]))), rowl=_mm256_load_pd((D*)((C*)(ava+1)+andx*sizeof(E)/sizeof(aof[0])));  // the outer-product values
      __m256d iph,ipl,isl;  // intermediate products and sums
      TWOPROD(rowh,colh,iph,ipl)  // (rowh,colh) to high precision
      ipl=_mm256_fmadd_pd(rowh,coll,ipl); ipl=_mm256_fmadd_pd(rowl,colh,ipl);  // accumulate middle pps
      if(ringdctrlb8&RINGACCREQD){   // normally we have to add the product into the incumbent ring value
       h0l0h1l1=_mm256_loadu_pd((D*)((I)r0+o0)); h2l2h3l3=_mm256_loadu_pd((D*)((I)r0+o2));   // read accumulands 0123 
       __m256d h0h2h1h3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), l0l2l1l3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // convert to 0213 order, hi & lo
       l0l2l1l3=_mm256_add_pd(l0l2l1l3,ipl);  // the middle pps.  low*low will never contribute unless value is exhausted & thus noise
       TWOSUM(iph,h0h2h1h3,iph,ipl)   // combine the high parts
       ipl=_mm256_add_pd(ipl,l0l2l1l3);  // add the combined low parts
      }
      h0l0h1l1=_mm256_shuffle_pd(iph,ipl,0b0000); h2l2h3l3=_mm256_shuffle_pd(iph,ipl,0b1111);  // convert result to E order
      _mm256_storeu_pd((D*)((I)r0+o0),h0l0h1l1); _mm256_storeu_pd((D*)((I)r0+o2),h2l2h3l3);    // store results
     }

     // 1-blocks
     for(alen=endofsts&0xffff;andx<alen;andx+=(RESBLKE*sizeof(aof[0]))){__m256d h0l0h1l1,h2l2h3l3;  // temps needed
      I o1=((US*)((C*)aof+andx))[1]; I o0=((US*)((C*)aof+andx))[0]; I o3=((US*)((C*)aof+andx))[3]; I o2=((US*)((C*)aof+andx))[2];  // offsets to ring values to accumulate into
      __m256d rowh=_mm256_load_pd((D*)((C*)ava+andx*sizeof(E)/sizeof(aof[0]))), rowl=_mm256_load_pd((D*)((C*)(ava+1)+andx*sizeof(E)/sizeof(aof[0])));  // the outer-product values
      __m256d iph,ipl,isl;  // intermediate products and sums
      TWOPROD(rowh,colh,iph,ipl)  // (rowh,colh) to high precision
      ipl=_mm256_fmadd_pd(rowh,coll,ipl); ipl=_mm256_fmadd_pd(rowl,colh,ipl);  // accumulate middle pps
      if(ringdctrlb8&RINGACCREQD){   // normally we have to add the product into the incumbent ring value
       h0l0h1l1=_mm256_blend_pd(_mm256_loadu_pd((D*)((I)r0+o0)),_mm256_loadu_pd((D*)((I)r0+o1-sizeof(E))),0b1100);  // read accumulands 0-1 
       h2l2h3l3=_mm256_blend_pd(_mm256_loadu_pd((D*)((I)r0+o2)),_mm256_loadu_pd((D*)((I)r0+o3-sizeof(E))),0b1100);  // read accumulands 2-3
       __m256d h0h2h1h3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b0000), l0l2l1l3=_mm256_shuffle_pd(h0l0h1l1,h2l2h3l3,0b1111);  // convert to 0213 order, hi & lo
       l0l2l1l3=_mm256_add_pd(l0l2l1l3,ipl);  // the middle pps.  low*low will never contribute unless value is exhausted & thus noise
       TWOSUM(iph,h0h2h1h3,iph,ipl)   // combine the high parts
       ipl=_mm256_add_pd(ipl,l0l2l1l3);  // add the combined low parts
      }
      h0l0h1l1=_mm256_shuffle_pd(iph,ipl,0b0000); h2l2h3l3=_mm256_shuffle_pd(iph,ipl,0b1111);  // convert result to E order
      _mm_storeu_pd((D*)((I)r0+o0),_mm256_castpd256_pd128(h0l0h1l1)); _mm_storeu_pd((D*)((I)r0+o2),_mm256_castpd256_pd128(h2l2h3l3));   // store 0&2
      h0l0h1l1=_mm256_permute4x64_pd(h0l0h1l1,0b01001110); h2l2h3l3=_mm256_permute4x64_pd(h2l2h3l3,0b01001110);  // shift 1&3
      _mm_storeu_pd((D*)((I)r0+o1),_mm256_castpd256_pd128(h0l0h1l1)); _mm_storeu_pd((D*)((I)r0+o3),_mm256_castpd256_pd128(h2l2h3l3));   // store 1&3 - 3 must be last because the others may have the same offset
     }
     // send a few values to Qkt if enough time has elapsed
     ringdctrlb8+=andx<<RINGDCTX;  // add the number of blocks we processed.  When the total goes nonnegative we can release again
     if(releaseblockmask>(UI)REPSGN(ringdctrlb8)){  // if there are released values...  (mask>0 and delayct nonneg)
      releasect=RELEASEBLOCKCT;  // set releasect to the number of blocks to take.  We will stop when the row is empty in any case
releaserow:;  // entered from below to drain the ring, either on buffer-full or at end-of-operation.  releasect is set to high-value in that case, and delay value to 0
      __m256d *releaseringbase=(__m256d*)ring[(I)releaseqktringbasecurr&(RINGROWS-1)];  // get base address in ring.  clang loads releaseqktringbasecurr a little late
      __m256d *releaseqkbase=(__m256d*)((I)releaseqktringbasecurr&-RINGROWS);  // get base address in Qkt
      ringdctrlb8+=releasedelayct0;  // add the (negative) amount of delay until we can do another release, keeping credit for any (positive) extra time we built up in storing into ring
      do{
       // calculate a result block
       I blockbyteofst=CTTZI(releaseblockmask)*sizeof(E)*RESBLKE;  // get offset to next modified block in this row
       __m256d iph,ipl,qkE01,qkE23;  // intermediate products and sums
       if(ringdctrlb8&RINGNNTREAD){qkE01=_mm256_castsi256_pd(_mm256_load_si256((__m256i*)((I)releaseqkbase+blockbyteofst))), qkE23=_mm256_castsi256_pd(_mm256_load_si256((__m256i*)((I)releaseqkbase+blockbyteofst)+1));}  // read qk, going through cache
       else{qkE01=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)((I)releaseqkbase+blockbyteofst))), qkE23=_mm256_castsi256_pd(_mm256_stream_load_si256((__m256i*)((I)releaseqkbase+blockbyteofst)+1));}  // read qk, bypass caches
       __m256d ringE01=_mm256_load_pd((D*)(__m256d*)((I)releaseringbase+blockbyteofst)), ringE23=_mm256_load_pd((D*)((__m256d*)((I)releaseringbase+blockbyteofst)+1));  // read ring
       __m256d qk0213h=_mm256_shuffle_pd(qkE01,qkE23,0b0000), qk0213l=_mm256_shuffle_pd(qkE01,qkE23,0b1111);  // convert to 0213 order, hi & lo
       __m256d ring0213h=_mm256_shuffle_pd(ringE01,ringE23,0b0000), ring0213l=_mm256_shuffle_pd(ringE01,ringE23,0b1111);  // convert to 0213 order, hi & lo
       TWOSUM(qk0213h,ring0213h,iph,ipl)   // combine the high parts
       ipl=_mm256_add_pd(ipl,_mm256_add_pd(qk0213l,ring0213l));  // add the combined low parts
       TWOSUM(iph,ipl,qk0213h,qk0213l)  // put h0h2h1h3 into canonical form.  TWOSUMBS might be OK here: if iph<ipl we have had massive cancellation and lost most precision; then it will not hurt to mistakenly call iph the bigger
       // zero the |value|s that are below threshold
       iph=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qk0213h),zerothresh,_CMP_GT_OQ);   // iph = 0 if result too small
       qk0213h=_mm256_and_pd(qk0213h,iph); qk0213l=_mm256_and_pd(qk0213l,iph); // set values to 0 if lower than threshold
       // write out the block and clear the ring for next user
       qkE01=_mm256_shuffle_pd(qk0213h,qk0213l,0b0000); qkE23=_mm256_shuffle_pd(qk0213h,qk0213l,0b1111);  // convert result to E order
       if(ringdctrlb8&RINGNNTWRITE){_mm256_store_pd((D*)(__m256d*)((I)releaseqkbase+blockbyteofst),qkE01); _mm256_store_pd((D*)((__m256d*)((I)releaseqkbase+blockbyteofst)+1),qkE23);}   // store qk, going throough cache
       else{_mm256_stream_pd((D*)(__m256d*)((I)releaseqkbase+blockbyteofst),qkE01); _mm256_stream_pd((D*)((__m256d*)((I)releaseqkbase+blockbyteofst)+1),qkE23);}   // store qk, going throough cache
       _mm256_storeu_pd((D*)(__m256d*)((I)releaseringbase+blockbyteofst),_mm256_setzero_pd()); _mm256_storeu_pd((D*)((__m256d*)((I)releaseringbase+blockbyteofst)+1),_mm256_setzero_pd());   // zero ring for next use
       if((releaseblockmask&=(releaseblockmask-1))==0)goto rowfin;  // advance to next block, exit if none
      }while(--releasect);  // could use PEXT & block mask to avoid need for releasect here

      if(0){rowfin:;  // come here when a row has been fully sent to Qkt  releaseblockmask==0 in open code always means nothing to release
       I relstart=(ringdctrlb8&RINGRELSTARTMASK)>>RINGRELSTARTX;  // extract ending release row#
       releaserowmask[relstart]=0;  // when we finish a row, we must leave it with an empty mask
       releaseqktringbase[relstart]=(__m256d*)((I)releaseqktringbase[relstart]&(RINGROWS-1));  // clear Qkt, leaving ring row#
       relstart=(relstart+1)&(RINGROWS-1); ringdctrlb8=(ringdctrlb8+(1<<RINGRELSTARTX))&RINGRELSTARTWRAP;   // advance to next released row
       if(((ringdctrlb8-relstart)&(RINGROWS-1))!=0){  // if the release area is not empty after removing the finished row...
        releaseblockmask=releaserowmask[relstart]; releaseqktringbasecurr=releaseqktringbase[relstart];  // move next row to the release variables.  blockmask=0 means no work
        // Here we loop back to handle exception cases: (1) ring full; (2) operation finished.  We set releasect to high-value 
        if(unlikely(((ringdctrlb8-relstart)&(RINGROWS-1))>=(RINGROWS-B8ROWS))){ringdctrlb8&=~RINGDCTMASK; goto releaserow;}  // if ring is still full, wait for it to drain.  Clear delay count for when we resume
        if(unlikely(stripex>=nstripes))goto releaserow;  // if the problem is over, flush the entire ring.  delayct immaterial
       }
       if(unlikely(releasect>100))if(stripex>=nstripes)goto finis; else goto caughtup;   // if we are coming out of a loopback, go to the right place: end, or just after we released into the full ring
      }
     }
    }
    // one op finished.  Move to next
    currop->rowindex=rowindex;  // putback the updated readahead index of the current op
    opinfo *currchn=currop->chain;   // pointer to next 
    if(unlikely(nextrowinop==HIGHVALUEI4)){if(unlikely(prevop==0))aops=currchn; else prevop->chain=currchn; if(likely(aops!=0)&&unlikely(aops->chain==0))releasedelaythreadedstripe=0;}  // if currop expired, remove it from chain; if only 1 op left, stop throttling release
    else{prevop=currop; minnextrow=nextrowinop<minnextrow?nextrowinop:minnextrow;} //  move to next, keep track of smallest start value
    currop=currchn;
   }
  
   // 8block finished.  close up the rows and release them to the output stage.  For each nonempty row we write out the address of the Qkt data, and copy the
   // mask.  We also have to make sure the mask is cleared to 0, and the Qkt base is cleared, in all rows that go unreleased
   I origb8=ringdctrlb8&RINGB8STARTMASK, b8start=origb8, sx=b8start; I qktrcol=(I)&qktcol[b8qktrow*qktncols];  // store index, address of the modified row of Qkt corresponding to b8start
   UI8 skiprows=0;  // mask of empty rows that must be added to the end
   DONOUNROLL(B8ROWS,
    skiprows=(skiprows&-RINGROWS)+(I)releaseqktringbase[b8start];  // remember new row in case we skip it
    releaseqktringbase[sx]=(__m256d*)(qktrcol+(I)releaseqktringbase[b8start]); UI8 msk=releaserowmask[sx]=releaserowmask[b8start];  // set ringbase/mask in output ring, which are all that is needed to update Qkt
    sx=(sx+(msk>0))&(RINGROWS-1); b8start=(b8start+1)&(RINGROWS-1); qktrcol+=qktncols*sizeof(E);  // advance to next input/output positions, and Qkt base
    skiprows<<=((msk>0)^1)<<3;  // if we skip the row, shift it up to safety (8 bits)
   )
   // rows [sx,b8start) were skipped: store the skipped row, clearing the mask and qkt base.  This returns the skipped rows to the pool
   while(b8start!=sx){skiprows>>=8; b8start=(b8start-1)&(RINGROWS-1); releaserowmask[b8start]=0; releaseqktringbase[b8start]=(__m256d*)(skiprows&(RINGROWS-1));}   // preserve row# mapped to this output-ring slot
   ringdctrlb8=(ringdctrlb8&~RINGB8STARTMASK)+b8start;  // b8start has been advanced over all the nonskipped rows.  Put it into the portmanteau, which releases them
   // Now that we have released an 8block, we must wait for final completion on all the op column indexes
   if(unlikely(!(ringdctrlb8&RINGFINALSYNC))){I yd=10; while(__atomic_load_n(&ctx->colndxct,__ATOMIC_ACQUIRE)<nthreads+2*nops){johnson(20*nthreads); if(--yd<0){YIELD}} ringdctrlb8|=RINGFINALSYNC;}  // delay until indexes are ready (2*nops op indexes, plus one failure for each thread).  We allow 1 read every 20ns
   if(releaseblockmask==0){releaseblockmask=releaserowmask[origb8]; releaseqktringbasecurr=releaseqktringbase[origb8]; ringdctrlb8&=~RINGDCTMASK;}  // if release queue was empty, move first row to the release variables.  blockmask=0 means no work.
      // delayct has been incrementing continuously, perhaps overflowing - clear it to start releasing
   else if(unlikely((((ringdctrlb8-(ringdctrlb8>>RINGRELSTARTX))&(RINGROWS-1)))>=(RINGROWS-B8ROWS))){releasect=HIGHVALUEI4; ringdctrlb8&=~RINGDCTMASK; goto releaserow;}  // if ring is full, loop in release to wait for it to drain, and clear delayct to ensure releasing continues
caughtup:;  // here when we have removed the ring-full situation
   // throttle the release depending on ring-full status.  We figure this only when we add rows because it's not important to keep it exactly right
   releasedelayct0=(-((((RINGROWS-((ringdctrlb8-(ringdctrlb8>>RINGRELSTARTX)-1)&(RINGROWS-1)))>>3)+1)*releasedelaythreadedstripe)>>3)<<RINGDCTX;  // decrease delay (which is negative) with each eight-row section filled
  }  // end 'while aops'
  stripex=__atomic_fetch_add(&ctx->resvx,1,__ATOMIC_ACQ_REL);  // reserve next row.  Every thread will finish with one failing reservation
 }
 if(releaseblockmask){releasect=HIGHVALUEI4; goto releaserow;}  // all stripes done - keep releasing any remnant
finis:;  // here we have flushed the ring at the end
 R 0;
}


// 128!:14  apply outer-products in parallel.  Bivalent
// a (optional) is debug options
// w is name;rowmasks;rowvalues,colmasks;colvalues;threshold;stripes;(\:comploads)
// name (usually 'Qkt') is the source/destination, QP type.  shape (r,c).  It must be globally assigned with usecount 1, aligned on a cacheline boundary with rows that are even # cachelines
// rowmasks (boxed shape p, the number of outer products).  Each contents is a boolean list, length <=r, indicating the position of non0 in rowvalues.  (+/@> rowmask) <: #@> rowvalues
// rowvalues (boxed shape p).  Each contents is QP list of non0 row values
// colmasks (boxed shape p).  Each contents is a boolean list, length =r, indicating the position of non0 in rowvalues.  (+/@> colmask) <: #@> rowvalues
// colvalues (boxed shape p).  Each contents is QP list of non0 col values
// threshold is a float atom.  Result |values| less than threshold are forced to 0
// stripes shape (s (number of stripes),2) giving start and end+1 of each stripe
// compgrade shape s  is (\: comploads), the order stripes should be processed in to go from slowest to fastest
// result is value of name after modification
F2(jtbatchop){F12IP;PROLOG(000);
 ARGCHK1(w);
 if(likely(AT(w)&VERB)){w=a; a=zeroionei(0);} I debopts; RE(debopts=i0(a))  // default options for monad and verify integer atom
 I4 *(colndxs)[MAXOP];  // pointers to column indexes, filled in by threads
 struct bopctx opctx={.nthreads=((lda(&JT(jt,systemlock))>2)?0:(*JT(jt,jobqueues))[0].nthreads)+1, .colndxs=&colndxs, .debopts=debopts,};  // suppress worker threads during system lock (i. e. debug)

 // extract the inputs
 ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==8,EVLENGTH)  // w is 8 boxes
 A box;  // will hold the contents of each box in turn
 box=C(AAV(w)[5]);  // threshold
 if(!(AT(box)&FL))RZ(box=cvt(FL,box)); ASSERT(AR(box)==0,EVRANK) opctx.threshold=DAV(box)[0];  // convert to FL
 box=C(AAV(w)[6]);  // stripes
 ASSERT(AT(box)&INT,EVDOMAIN) ASSERT(AR(box)==2,EVRANK) ASSERT(AS(box)[1]==2,EVLENGTH) opctx.stripestartend1=(I (*)[][2])IAV(box); opctx.nstripes=AS(box)[0];  // must be INT [][2]
 DO(opctx.nstripes, ASSERT((*opctx.stripestartend1)[i][1]-(*opctx.stripestartend1)[i][0]<=RINGCOLS,EVLIMIT))   // verify max stripe width not exceeded
 GATV0(box,INT4,opctx.nstripes*MAXOP,1); opctx.opstripebsum=(I4 (*)[][MAXOP])I4AV(box);  // allocate running-index area, save in ctx
 box=C(AAV(w)[7]);  // compgrade
 ASSERT(AT(box)&INT,EVDOMAIN) ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==opctx.nstripes,EVLENGTH) opctx.stripegrade=IAV(box);  // must be INT [#stripes]
 box=C(AAV(w)[0]);  // 'Qkt'
 ASSERT(AT(box)&LIT,EVDOMAIN) ASSERT(AR(box)<=1,EVRANK)  A nm; RZ(nm=nfs(AN(box),CAV(box),0)); A qktf=syrd(nm,jt->locsyms); A qkt=QCWORD(qktf); ASSERT(qkt!=0,EVVALUE)  // name exists
 // from here till end we must take errors through 'errexit'
 A z=0;  // presumptive error return
 ASSERTGOTO(AT(qkt)&QP,EVDOMAIN,errexit) ASSERTGOTO(AR(qkt)==2,EVRANK,errexit) opctx.qkt=EAV(qkt);  // QP, rank=2
 ASSERTSYSGOTO(((I)opctx.qkt&(CACHELINESIZE-1))==0,"accumuland not on cache bdy",errexit)  // data must be on cache bdy
 opctx.qktnrows=AS(qkt)[0]; opctx.qktncols=AS(qkt)[1];  // remember shape in parm block 
 ASSERTSYSGOTO((((I)opctx.qktncols*sizeof(E))&(CACHELINESIZE-1))==0,"accumuland length not a cacheline multiple",errexit)  // data must be on cache bdy
 ASSERTSYSGOTO((AC(qkt)-!!((I)qktf&QCFAOWED))==1,"accumuland is aliased",errexit);   // count, after removing the ra from syrd, must be 1
 box=C(AAV(w)[1]);  // row masks
 ASSERTGOTO(AT(box)&BOX,EVDOMAIN,errexit) ASSERTGOTO(AR(w)<=1,EVRANK,errexit) opctx.nops=AN(box); ASSERT(opctx.nops<=MAXOP,EVLIMIT) opctx.oprowmasks=AAV(box);  // must be list of boxes; save number and address
 DO(opctx.nops, A sb=C(opctx.oprowmasks[i]); ASSERTGOTO(AT(sb)==B01,EVDOMAIN,errexit) ASSERTGOTO(AR(sb)<=1,EVRANK,errexit) ASSERTGOTO(AN(sb)<=opctx.qktncols,EVLENGTH,errexit) )  // must be boolean mask no longer than the row
 box=C(AAV(w)[2]);  // row values
 ASSERTGOTO(AT(box)&BOX,EVDOMAIN,errexit) ASSERTGOTO(AR(w)<=1,EVRANK,errexit) ASSERTGOTO(AN(box)==opctx.nops,EVLENGTH,errexit) opctx.oprowvals=AAV(box);  // must be list of boxes; save number and address
 DO(opctx.nops, A sb=C(opctx.oprowvals[i]); ASSERTGOTO(AT(sb)==QP,EVDOMAIN,errexit) ASSERTGOTO(AR(sb)<=1,EVRANK,errexit) ASSERTGOTO(AN(sb)<=opctx.qktncols,EVLENGTH,errexit) )  // must be quadprec no longer than the row
 box=C(AAV(w)[3]);  // col masks
 ASSERTGOTO(AT(box)&BOX,EVDOMAIN,errexit) ASSERTGOTO(AR(w)<=1,EVRANK,errexit) ASSERTGOTO(AN(box)==opctx.nops,EVLENGTH,errexit) opctx.opcolmasks=AAV(box);  // must be list of boxes; save number and address
 DO(opctx.nops, A sb=C(opctx.opcolmasks[i]); ASSERTGOTO(AT(sb)==B01,EVDOMAIN,errexit) ASSERTGOTO(AR(sb)<=1,EVRANK,errexit) ASSERTGOTO(AN(sb)<=opctx.qktnrows,EVLENGTH,errexit) )  // must be boolean mask no longer than the col
 box=C(AAV(w)[4]);  // col values
 ASSERTGOTO(AT(box)&BOX,EVDOMAIN,errexit) ASSERTGOTO(AR(w)<=1,EVRANK,errexit) ASSERTGOTO(AN(box)==opctx.nops,EVLENGTH,errexit) opctx.opcolvals=AAV(box);  // must be list of boxes; save number and address
 DO(opctx.nops, A sb=C(opctx.opcolvals[i]); ASSERTGOTO(AT(sb)==QP,EVDOMAIN,errexit) ASSERTGOTO(AR(sb)<=1,EVRANK,errexit) ASSERTGOTO(AN(sb)<=opctx.qktnrows,EVLENGTH,errexit) )  // must be quadprec no longer than the col
 opctx.colndxct=opctx.resvx=opctx.nthreads; opctx.colndx9ct=2*opctx.nops;  // each thread will fetch at least once; those with a job# less than nops will fetch again.  When all have missed once, we are synced.  9ct is early completion, counting down, one for stripe index one for top of column index
  // run the operation
 jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtbatchopx,&opctx,opctx.nthreads,0);  // execute on all threads
 z=qkt;  // good return
errexit:;  // here on error to undo syrd
 if((I)qktf&QCFAOWED)fa(qkt);  // if syrd issued ra, undo it
 EPILOG(z);  // return the value


}
#else
F2(jtbatchop){F12IP;ASSERT(0,EVNONCE);}
#endif
