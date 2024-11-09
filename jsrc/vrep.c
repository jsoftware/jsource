/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: a#"r w                                                           */

#include "j.h"
#include "ve.h"


#define REPF(f)         A f(J jt,A a,A w,I wf,I wcr)


static REPF(jtrepzdx){A p,q,x;P*wp;
 F2PREFIP;ARGCHK2(a,w);
 if(ISSPARSE(AT(w))){wp=PAV(w); x=SPA(wp,e);}
 else x=jt->fill&&AN(jt->fill)?jt->fill:jtfiller(jt,AT(w),0,0);
 RZ(p=repeat(ravel(rect(a)),ravel(stitch(IX(wcr?AS(w)[wf]:1),num(-1)))));
 RZ(IRS2(w,x,NOEMSGSELF,wcr,0L,jtover,q));
 R IRS2(p,q,0L,1L,wcr+!wcr,jtfrom,x);
}    /* (dense complex) # (dense or sparse) */

static REPF(jtrepzsx){A q,x,y;I c,d,j,k=-1,m,p=0,*qv,*xv,*yv;P*ap;
 F2PREFIP;ARGCHK2(a,w);
 ap=PAV(a); x=SPA(ap,x); m=AN(x);
 if(!AN(SPA(ap,a)))R repzdx(ravel(x),w,wf,wcr);
 y=SPA(ap,i); yv=AV(y);
 RZ(x=cvt(INT,vec(FL,2*m,AV(x)))); xv=AV(x);
 if(equ(num(0),SPA(ap,e))){
  k=c=AS(w)[wf];
  if(!wf&&ISSPARSE(AT(w))){A a,y;I m,n,q,*v;P*wp;
   wp=PAV(w); a=SPA(wp,a);
   if(AN(a)&&!*AV(a)){
    y=SPA(wp,i); v=AS(y); m=v[0]; n=v[1]; v=AV(y);
    k=m?v[(m-1)*n]+1:0; q=0; 
    DQ(m, if(q==*v)++q; else if(q<*v){k=q; break;} v+=n;);
  }}
  ASSERT(k<=IMAX-1,EVLIMIT);
  if(c==k)RZ(w=irs2(sc(1+k),w,0L,0L,wcr,jttake));
  DO(2*m, ASSERT(0<=xv[i],EVDOMAIN); p+=xv[i]; ASSERT(0<=p,EVLIMIT););
  GATV0(q,INT,p,1); qv=AV1(q);
  DO(m, c=*xv++; d=*xv++; j=yv[i]; DQ(c, *qv++=j;); DQ(d, *qv++=k;);); 
  R irs2(q,w,0L,1L,wcr,jtfrom);
 }
 ASSERT(0,EVNONCE);
}    /* (sparse complex) #"r (dense or sparse) */

static REPF(jtrepbdx){A z;I c,k,m,p;
 // wf and wcr are set.  a is repeated for each cell of w
 F2PREFIP;ARGCHK2(a,w);
 if(ISSPARSE(AT(w)))R irs2(ifb(AN(a),BAV(a)),w,0L,1L,wcr,jtfrom);
 m=AN(a);   // m is # minor cells in a major cell, i. e. #booleans in a
 void *zvv; void *wvv=voidAV(w); I n; // pointer to output area; pointer to input data; number of prefix bytes to skip in first cell
 p=bsum(m,BAV(a));  // p=# 1s in result, i. e. length of result item axis
 if(unlikely(m==p)){RETF(w);}  // if all the bits are 1, we can return very quickly.  It's rare, but so cheap to test for.
 PROD(c,wf,AS(w)); PROD(k,wcr-1,AS(w)+wf+1); // c=#major cells, k=#atoms per item of cell
 I zn=c*k*p;  // zn=#atoms in result
 k<<=bplg(AT(w));   // k is now # bytes/cell
 // if the result is inplaceable, AND it is not getting much shorter, keep the same result area
 // We retain the old block as long as the new one is at least half as big, without looking at total size of the allocation,
 // This could result in a very small block's remaining in a large allocation after repeated trimming.  We will accept the risk.
 // Accept only DIRECT blocks so we don't have to worry about explicitly freeing uncopied cells
#if C_AVX2 || EMU_AVX2
 I exactlen;  // will be 1 if overstore is not allowed on copy
#endif
 if(!ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&(m-2*p)&(-(AT(w)&DIRECT)),w)) {
  // normal non-in-place copy
    // no overflow possible unless a is empty; nothing  moved then, and zn is 0
  I zr=AR(w); GA00(z,AT(w),zn,zr); MCISH(AS(z),AS(w),zr) // allocate result
  zvv=voidAVn(zr,z);  // point to the output area
  // if blocks abandoned, pristine status can be transferred to the result, because we know we are not repeating any cells
  AFLAGORLOCAL(z,PRISTFROMW(w))  // result pristine if inplaceable input was - w prist cleared later
#if C_AVX2 || EMU_AVX2
  exactlen=0;  // OK to overstore when copying to new buffer
#endif
  n=0;  // cannot skip prefix of 1s if not inplace
 }else{
  z=w; // inplace
  // We are going to change the type/shape/rank of the virtual block.  If it is UNINCORPABLE (meaning it will be reused), create a new clone.  We will still inplace the data area
  if(AFLAG(z)&AFUNINCORPABLE){RZ(z=clonevirtual(z));}
  AN(z)=zn;  // Install the correct atom count
  // see how many leading values of the result are already in position.  We don't need to copy them in the first cell
  UI *avv=IAV(a); for(n=0;n<((m-1)>>LGSZI);++n)if(avv[n]!=VALIDBOOLEAN)break;  // n ends pointing to a word that is known not all valid, because there MUST be a 0 somewhere
  // now n has the number of words of a to skip.  Get the number of extra bytes in the last word.
  UI nextwd=avv[n];  // partially-filled word to check
  n<<=LGSZI;  // convert skipct to bytes
  // Convert skipcount to bytes, and advance wvv to point to the first cell that may move
  n+=CTTZI(nextwd^VALIDBOOLEAN)>>LGBB;  // complement; count original 1s, add to n.  m cannot be 0 so there must be a valid 0 bit in nextwd
  zvv=wvv=(C*)wvv+k*n;  // step input over items left in place; use that as the starting output pointer also
#if C_AVX2 || EMU_AVX2
  exactlen=!!(k&(SZI-1));  // if items are not multiples of I, require exact len.  Since we skip an unchanged prefix, we will seldom have address contention during the copy
#endif
  // since the input is abandoned and no cell is ever duplicated, pristinity is unchanged
 }
 AS(z)[wf]=p;  // move in length of item axis, #bytes per item of cell
 if(!zn)R z;  // If no atoms to process, return empty

// original  DO(c, DO(m, if(b[i]){MC(zv,wv,k); zv+=k;} wv+=k;);); break;
 JMCDECL(endmask) JMCSETMASK(endmask,k,exactlen)   // set up for irregular move, if we need one
  
 while(--c>=0){
  // at top of loop n is biased by the number of leading bytes to skip. wvv points to the first byte to process
#if C_AVX2 || EMU_AVX2
  C *avv=CAV(a)+n; n=m-n;   // prime the pipeline for top of loop.
  __m256i i1=_mm256_set1_epi8(1);
  __m256i bitpipe00,bitpipe01,bitpipe10,bitpipe11;  // place to read in booleans and packed bits
#define BSIZE 32  // # bytes in a block
  __m256i bitendmask=_mm256_loadu_si256((__m256i*)(validitymask+((-n>>LGSZI)&(NPAR-1))));  // since alignment never changes, we can predict the validity for the last block
  if(n>=2*BSIZE){bitpipe10=_mm256_loadu_si256((__m256i*)(avv)); bitpipe11=_mm256_loadu_si256((__m256i*)(avv+BSIZE));}  // if there is a first FULL batch, prefetch it
  while(n>0){    // n is # bytes left to process
   // We process 64 bytes at a time, always reading ahead one block.  If there are >=64 items to do, bitpipe1 has the next set to process, from *avv
   // In case there are few 1s, we fast-skip over blocks of 0s.  Because the processing is so fast, we don't change alignment ever.
   I n0=n;  // remember where we started
   while(1){
    bitpipe00=bitpipe10; bitpipe01=bitpipe11;  // Move the next bits (if any) into pipe0
    if(n<=2*BSIZE)break;  // exit if there is no further batch.  n will never hit 0.  We may process an empty stack
    if(n>=4*BSIZE){bitpipe10=_mm256_loadu_si256((__m256i*)(avv+2*BSIZE)); bitpipe11=_mm256_loadu_si256((__m256i*)(avv+3*BSIZE));}  // if there is another FULL batch, prefetch it
    if(!_mm256_testz_si256(_mm256_or_si256(bitpipe00,bitpipe01),_mm256_or_si256(bitpipe00,bitpipe01)))break;  // exit if nonzero found.  Cannot be spurious
    // We hit 64 0s.  Advance over them
    avv+=2*BSIZE; n-=2*BSIZE;
   }
   wvv+=k*(n0-n);  // advance w pointer for the moves made, if any
   // Move the bits into bitstack
   I bitstack;  // the bits packed together
   if(n>=2*BSIZE){
    // n>=64, bitpipe0 has the bits to process (and if n>=128 bitpipe1 is in flight).
    bitstack=(I)(UI4)_mm256_movemask_epi8(_mm256_cmpeq_epi8(bitpipe00,i1))
            |((I)(UI4)_mm256_movemask_epi8(_mm256_cmpeq_epi8(bitpipe01,i1))<<BSIZE);
   }else{
    // n<64: we have to read the bits under mask to stay in bounds.  Read the last block, which requires mask
    bitstack=(I)(UI4)_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_maskload_epi64((I*)(avv+((n-1)&BSIZE)),bitendmask),i1));
    if(n>BSIZE){bitstack=(bitstack<<BSIZE)|(I)(UI4)_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_loadu_si256((__m256i*)(avv)),i1));}  // if there is a first block, read it too
    // mask off invalid bits
    bitstack&=~(-1LL<<n);  // leave n valid bits
   }
   avv+=2*BSIZE;  // we have moved 2 batches of bits into bitstack
#else
  UI *avv=(UI*)(CAV(a)+n); n=m-n; n+=((n&(SZI-1))?SZI:0); UI bits=*avv++;  // prime the pipeline for top of loop.  Extend n only if needed to get all bits, and only by a full word
  while(n>0){    // where we load bits SZI at a time
   // skip empty words, to get best speed on near-zero a.  This exits with the first unskipped word in bits
   NOUNROLL while(bits==0 && n>=(2*SZI)){bits=*avv++; n-=SZI; wvv=(C*)wvv+(k<<LGSZI);}  // fast-forward over zeros.  Always leave 1 word so we have a batch to process
   I batchsize=n>>LGSZI; batchsize=MIN(BB,batchsize);
   UI bitstack=0; NOUNROLL while(--batchsize>0){I bits2=*avv++; PACKBITSINTO(bits,bitstack); bits=bits2;};  // keep read pipe ahead
   // Handle the last word of the batch.  It might have non-Boolean data at the end, AFTER the Boolean padding.  Just clear the non-boolean part in this line
   bits&=VALIDBOOLEAN; PACKBITSINTO(bits,bitstack);
   // Now handle the last batch, by removing initial extension if any, discarding garbage bits at the end, and then shifting the lead bit down to bit 0
   if(n>=BW+SZI)bits=*avv++;else {n-=n&(SZI-1)?SZI:0; bitstack<<=(BW-n)&(SZI-1); bitstack>>=BW-n;}  // discard invalid trailing bits; shift leading byte to position 0.  For non-last batches, start on next batch
#endif
#define REPEATLOOP1(t,x) if(bitstack==0){zvv+=sizeof(t)*x; break;} *(t*)(zvv+sizeof(t)*x)=((t*)wvv)[CTTZI(bitstack)]; bitstack&=bitstack-1;
#define REPEATLOOP(t) NOUNROLL while(1){REPEATLOOP1(t,0) REPEATLOOP1(t,1) REPEATLOOP1(t,2) REPEATLOOP1(t,3) zvv+=sizeof(t)*4;} break;
   switch(k){  // copy the words
   case sizeof(UI): REPEATLOOP(UI) case sizeof(C): REPEATLOOP(C)  case sizeof(US): REPEATLOOP(US) 
#if BW==64
   case sizeof(UI4): REPEATLOOP(UI4) 
#endif
   default: NOUNROLL while(bitstack){I bitx=CTTZI(bitstack); JMCR(zvv,(C*)wvv+k*bitx,k,exactlen,endmask); zvv=(C*)zvv+k; bitstack&=bitstack-1;} break;  // overwrite OK
   }

   wvv=(C*)wvv+(k<<LGBW);  // advance base to next batch of 64
   n-=BW;  // decr count left
  }
  wvv=(C*)wvv+(k*n);  // in case we loop back, back wvv to start of next input area, taking away the part of the last BW section we didn't use
  n=0;  // no bias for cells after the first
 } 
 R z;
}    /* (dense boolean)#"r (dense or sparse) */

static REPF(jtrepbsx){A ai,c,d,e,g,q,x,wa,wx,wy,y,y1,z,zy;B*b;I*dv,*gv,j,m,n,*u,*v,*v0;P*ap,*wp,*zp;
 F2PREFIP;ARGCHK2(a,w);
 ap=PAV(a); e=SPA(ap,e); 
 y=SPA(ap,i); u=AV(y);
 x=SPA(ap,x); n=AN(x); b=BAV(x);
 if(!AN(SPA(ap,a)))R irs2(ifb(n,b),w,0L,1L,wcr,jtfrom);
 if(!*BAV(e)){
  GATV0(q,INT,n,1); v=v0=AV1(q); 
  DO(n, if(*b++)*v++=u[i];); 
  AN(q)=AS(q)[0]=v-v0; 
  R irs2(q,w,0L,1L,wcr,jtfrom);
 }
 wp=PAV(w);
 if(!ISSPARSE(AT(w))||all0(eq(sc(wf),SPA(wp,a)))){RZ(q=denseit(a)); R irs2(ifb(AN(q),BAV(q)),w,0L,1L,wcr,jtfrom);}  // here if dense w
 wa=SPA(wp,a); wy=SPA(wp,i); wx=SPA(wp,x);
 RZ(q=aslash(CPLUS,a));
 GASPARSE(z,AT(w),1,AR(w),AS(w)); AS(z)[wf]=m=*AV(q);
 RZ(c=indexof(wa,sc(wf)));
 RZ(y1=fromr(c,wy));
 RZ(q=not(eps(y1,ravel(repeat(not(x),y)))));
 m=AS(a)[0]-m;
 GATV0(ai,INT,m,1); v=AV1(ai); DO(n, if(!*b++)*v++=u[i];);
 RZ(g=grade1(over(ai,repeat(q,y1)))); gv=AV(g);
 GATV0(d,INT,AN(y1),1); dv=AV1(d); j=0; DO(AN(g), if(m>gv[i])++j; else dv[gv[i]-m]=j;);
 RZ(zy=mkwris(repeat(q,wy))); v=AV(zy)+*AV(c); m=AS(zy)[1]; DO(AS(zy)[0], *v-=dv[i]; v+=m;);
 zp=PAV(z);
 SPB(zp,a,ca(wa));
 SPB(zp,e,SPA(wp,e));
 SPB(zp,i,zy);
 SPB(zp,x,repeat(q,wx));
 R z;
}    /* (sparse boolean) #"r (dense or sparse) */

static REPF(jtrepidx){A y;I j,m,p=0,*v,*x;A z; 
 F2PREFIP;ARGCHK2(a,w);
 RZ(a=vi(a)); x=IAV(a);
 m=AS(a)[0]; if(unlikely(m==0))RETF(RETARG(w));
#if 0  // when the integer reductions get faster
 mininsI(1,m,1,x,&p,jt); ASSERT(p>=0,EVDOMAIN);  // verify all values >=0
 ASSERT(EVOK==plusinsI(1,m,1,x,&p,jt),EVLIMIT);  // get total number of result, error if overflow
#else
 I anylt0=0, anyofl=0; UI dlct=(m+3)>>2; I backoff=(-m)&3;
#define COMP1(i) p+=xx[i]; anylt0|=xx[i]; if(i&1)anyofl|=p;  // looking for overflow every other time is enough, leaves branch slot
 I *xx=x-backoff; switch(backoff){do{case 0: COMP1(0) case 1: COMP1(1) case 2: COMP1(2) case 3: COMP1(3) xx+=4;}while(--dlct);}   \
 ASSERT(anylt0>=0,EVDOMAIN) ASSERT(anyofl>=0,EVLIMIT);
#endif
 if(unlikely(ISSPARSE(AT(w)))){
  GATV0(y,INT,p,1); v=AV1(y); 
  DO(m, j=i; DQ(x[j], *v++=j;););  // fill index vector with all the indexes
  R IRS2(y,w,0L,1L,wcr,jtfrom,z);
 }else{I itemsize, ncells, zn, j;  // # atoms in an item (then bytes), #cells to process, #atoms in result
  // non-sparse code.  copy the repeated items directly
  PROD(itemsize,wcr-1,AS(w)+wf+1) PROD(ncells,wf,AS(w)) DPMULDE(itemsize*ncells,p,zn) // itematoms*ncells cannot overflow in valid w unless p=0
  I itembytes=itemsize<<bplg(AT(w));  // #bytes in item
#if C_AVX2 || EMU_AVX2
  itemsize=itembytes==SZI?itemsize:0; itemsize=(p>>2)>m?itemsize:0;  // repurpose itemsize to # of added items to leave space for wide stores   TUNE  use word code if average # repeats > 4
#else
#define itemsize 0
#endif
  // If we are moving 8-byte items, we extend the allocation so that we can overstore up to 4 words.  This allows us to avoid remnant handling
  I zr=AR(w)+!wcr; GA00(z,AT(w),zn+(itemsize<<LGNPAR),zr); MCISH(AS(z),AS(w),zr) AS(z)[wf]=p; AN(z)=zn;  // allo result, copy shape but replace the lengthened axis, which may be added
  C *zv=CAVn(zr,z);  // output fill pointer
  C *wv=CAV(w);  // input item pointer, increments over input cells
  for(;ncells;--ncells){  // for each result cell
   // make x[j] copies of item wv[j]
   if(itembytes==SZI){
    if(itemsize){
#if C_AVX2 || EMU_AVX2
     for(j=0;j<m;++j){  // for each repeated item
      __m256i wd=_mm256_set1_epi64x(((I*)wv)[j]);  // the word to replicate, in each lane
      I wdstomove=x[j];  // # repeats
      // we can overstore 4 words, so we write out one copy forthwith.  We then copy 2 blocks at a time.  We may repeat the first address to get in sync
      // start with a store to allow aligning the output pointer.  Advance zv1 to a store boundary, but never past the end-of-area+1.  Decr count
//      _mm256_storeu_si256((__m256i*)zv1,wd);  // first store, which may be overwritten
      I wdadj=NPAR-(((I)zv>>LGSZI)&(NPAR-1));  // #words to end-of-block
      _mm256_maskstore_epi64((I*)zv,_mm256_loadu_si256((__m256i*)(validitymask+4-wdadj)),wd);  // first store, which may be overwritten
      C *zv1=zv;  // local store pointer, which may overwrite
      zv+=wdstomove*SZI;  // advance the real store pointer to the next valid output location for the next loop
      wdstomove-=wdadj; wdadj=wdstomove<0?0:wdadj;  wdstomove=wdstomove<0?0:wdstomove; zv1+=wdadj<<LGSZI;  // advance to align, unless that would go past end
      // move the rest of the data, aligned unless no more valid words.  We may overwrite
      _mm256_storeu_si256((__m256i*)zv1,wd);  // aligned store, which may be overwritten in full
      zv1+=((wdstomove+(NPAR-1))&NPAR)<<LGSZI;  // if 'incrementing addr' (i. e. odd # stores needed), increment for the stores
      C *zvend=zv1+(((wdstomove+(NPAR-1))>>(LGNPAR+1))*2*NPAR*SZI);  // number of pairs to store AFTER the first store.  repct 0->0, 1-4->0, 5-8->1 (repeating addr), 9-12->1 (incrementing addr)...
      NOUNROLL while(zv1!=zvend){
       _mm256_storeu_si256((__m256i*)zv1,wd); _mm256_storeu_si256((__m256i*)(zv1+NPAR*SZI),wd); zv1+=2*NPAR*SZI;   // write a pair.  Probably misaligned & thus slow
      }
     }
#endif
    }else{  // short runs on average.  loop seems to be fast as is
     for(j=0;j<m;++j){  // for each repeated item
      I wx=((I*)wv)[j]; DONOUNROLL(x[j], *(I*)zv=wx; zv+=SZI;)  // copy the atom
     }
    }
    wv+=m*SZI;  // advance I types all at once
   }else{
    JMCDECL(endmask) JMCSETMASK(endmask,itembytes,0)
    for(j=0;j<m;++j){  // for each repeated item
     DO(x[j], JMCR(zv,wv,itembytes,0,endmask) zv+=itembytes;) wv+=itembytes;
    }
   }
  }
 }
 RETF(z);
}    /* (dense  integer) #"r (dense or sparse) */
#undef itemsize

static REPF(jtrepisx){A e,q,x,y;I c,j,m,p=0,*qv,*xv,*yv;P*ap;
 F2PREFIP;ARGCHK2(a,w);
 ap=PAV(a); e=SPA(ap,e); 
 y=SPA(ap,i); yv=AV(y);
 x=SPA(ap,x); if(!(INT&AT(x)))RZ(x=cvt(INT,x)); xv=AV(x);
 if(!AN(SPA(ap,a)))R repidx(ravel(x),w,wf,wcr);
 if(!*AV(e)){
  m=AN(x);  
  DO(m, ASSERT(0<=xv[i],EVDOMAIN); p+=xv[i]; ASSERT(0<=p,EVLIMIT););
  GATV0(q,INT,p,1); qv=AV1(q); 
  DO(m, c=xv[i]; j=yv[i]; DQ(c, *qv++=j;);); 
  R irs2(q,w,0L,1L,wcr,jtfrom);
 }
 ASSERT(0,EVNONCE);
}    /* (sparse integer) #"r (dense or sparse) */


static REPF(jtrep1d){A z;C*wv,*zv;I c,k,m,n,p=0,q,t,*ws,zk,zn;
 F2PREFIP;ARGCHK2(a,w);
 t=AT(a); m=AN(a); ws=AS(w); SETICFR(w,wf,wcr,n);   // n=length of item axis in input.  If atom, is repeated to length of a
 t=m?t:B01;  // 
 if(t&CMPX){
  if(wcr)R repzdx(from(apv(n,0L,0L),a),w,                wf,wcr);
  else{A za; RZ(za=apv(m,0L,0L)); R repzdx(a,IRS2(za,w,0L,1L,0L,jtfrom,z),wf,1L );}
 }
 if(t&B01){p=bsum(m,BAV(a)); // bsum in case a is big.  Atomic boolean was handled earlier
 }else{I*x; 
  RZ(a=vi(a)); x=AV(a);
  I rawin=0, totin=0; DO(m, p+=x[i]; rawin|=x[i]; totin|=p;); ASSERT(0<=rawin,EVDOMAIN); ASSERT(0<=totin,EVLIMIT); // p=#items in result
 }
 DPMULDE(p,n,q);  // q=length of result item  axis.  +/a copies, each of length n
 DPMULDE(p,AN(w),zn);
 I zr=AR(w)+!wcr; GA00(z,AT(w),zn,zr); MCISH(AS(z),AS(w),zr) AS(z)[wf]=q;
 if(!zn)R z;
 wv=CAV(w); zv=CAVn(zr,z);
 PROD(c,wf+(I )(wcr!=0),ws); PROD(k,wcr-1,ws+wf+1); k <<=bplg(AT(w));  // c=#cell-items to process  k=#atoms per cell-item
 zk=p*k;  // # bytes to fill per item
 DQ(c, mvc(zk,zv,k,wv); zv+=zk; wv+=k;);
 R z;
}    /* scalar #"r dense   or   dense #"0 dense */

static B jtrep1sa(J jt,A a,I*c,I*d){A x;B b;I*v;
 b=ISDENSETYPE(AT(a),CMPX);
 if(b)RZ(x=rect(a)) else x=a; 
 if(AR(a)){ASSERT(equ(num(1),aslash(CSTARDOT,le(zeroionei(0),ravel(x)))),EVDOMAIN); RZ(x=aslash(CPLUS,x));} 
 if(!ISDENSETYPE(AT(x),INT))RZ(x=cvt(INT,x));
 v=AV(x); *c=v[0]; *d=b?v[1]:0;
 ASSERT(0<=*c&&0<=*d,EVDOMAIN);
 R 1;
}    /* process a in a#"0 w */

static REPF(jtrep1s){A ax,e,x,y,z;B*b;I c,d,cd,j,k,m,n,p,q,*u,*v,wr,*ws;P*wp,*zp;
 F2PREFIP;ARGCHK2(a,w);
 if((AT(a)&(SPARSE+CMPX))==(SPARSE+CMPX))R rep1d(denseit(a),w,wf,wcr);
 RE(rep1sa(a,&c,&d)); cd=c+d;   // c=#repeats, d=#skips, cd=stride between repeats
 if(!ISSPARSE(AT(w)))R rep1d(d?jdot2(sc(c),sc(d)):sc(c),w,wf,wcr);  // here if dense w
 wr=AR(w); ws=AS(w); n=wcr?ws[wf]:1; DPMULDE(n,cd,m)
 wp=PAV(w); e=SPA(wp,e); ax=SPA(wp,a); y=SPA(wp,i); x=SPA(wp,x);  // y is nonsparse indexes, x is values, a is axes, e sparse element 
 GASPARSE(z,AT(w),1,wr+!wcr,ws); AS(z)[wf]=m; zp=PAV(z);
 RE(b=bfi(wr,ax,1));
 if(wcr&&b[wf]){    /* along sparse axis */
  u=AS(y); p=u[0]; q=u[1]; u=AV(y);  // p=# nonsparse rows, q=#axes  u->start of indexes
  RZ(x=repeat(sc(c),x));
  RZ(y=mkwris(repeat(sc(c),y)));   // repeat the rows of x and y
  if(p&&1<c){   // if there is something to do
   j=0; DO(wf, j+=b[i];); v=j+AV(y);  // j=# sparse axes in frame, v -> first index in cell
   if(AN(ax)==1+j){u+=j; DO(p, m=cd**u; u+=q; DO(c, *v=m+i; v+=q;););}  // if replicating the last sparse axis, simply turn each index into an interval of indexes
   else{A xx;I h,i,j1=1+j,*uu;   // replicating interior axis.  For each replicated index, we must count the number of nonsparse values that share the prefix.  j1 is index of the first sparse axis in the replicated cell
    // v has replicated index lists
    GATV0(xx,INT,j1,1); uu=AV1(xx);  // allocate vector that will hold the prefix, i. e. the sparse axes
    k=0; DO(j1, uu[i]=u[i];);   // initialize uu to the sparse indexes in the first row of input indexes.  k is start of matching area
    for(i=0;;++i,u+=q)   // for each input row...
     if(i==p||ICMP(uu,u,j1)){   // if we hit end-of-input or there is a change in the prefix...
      // when there is a change in index or we run off the end of the indexes, we have found the end+1 of the region of indexes that share a prefix.
      // The region starts at k and runs to i-1.  I don't think this region-finding is necessary.
      h=i-k; k=i; m=cd*uu[j];   // m is the remapped start of the indexes for he replicated area
      DO(h, DO(c, *v=m+i; v+=q;););  // remap the area
      if(i==p)break;  // stop after end-of-input
      DO(j1, uu[i]=u[i];);  // reinit with the nonmatching next prefix
     } 
    RZ(xx=grade1(y));
    RZ(x=from(xx,x));
    RZ(y=from(xx,y));
 }}}else{A xx;      /* along dense  axis */
  j=0; DO(wcr, j+=!b[wf+i];);
  RZ(y=ca(y));
  if(d){xx=jt->fill; jt->fill=e;}  // e cannot be virtual
  x=irs2(AR(a)&&CMPX&AT(a)?a:d?jdot2(sc(c),sc(d)):sc(c),x,0L,1L,j,jtrepeat); 
  if(d)jt->fill=xx; 
  RZ(x);
 }
 SPB(zp,e,e);
 SPB(zp,a,ax);
 SPB(zp,i,y);
 SPB(zp,x,x);
 R z;
}    /* scalar #"r sparse   or  sparse #"0 (dense or sparse) */

A (*reptab[])() = {jtrepisx,jtrepidx,jtrepbsx,jtrepbdx,jtrepzsx,jtrepzdx,jtrep1s,jtrep1d};
F2(jtrepeat){A z;I acr,ar,wcr,wf,wr;
 F2PREFIP;ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 I adense=SGNIFDENSE(AT(a));  // sign set if a is dense
 I att=(AT(a)&B01)+((AT(a)&CMPX)>>(CMPXX-1));  // 0 if INT/FL 1 if B01 2 if CMPX
 att=(-acr&-wcr)>=0?3:att;  // override with 3 if either a or w is an atom
 adense&=(-acr&-wcr)|SGNIFDENSE(AT(w));  // if a or w is an atom, require both a and w dense
 A (*repfn)() = reptab[2*att+SGNTO0(adense)];  // fetch address to call

 // special case: if a is atomic 1, and cells of w are not atomic.  a=0 is fast in the normal path
 if(((-wcr)&(ar-1)&(-(AT(a)&(B01|INT))))<0){I aval = BIV0(a);  // no fast support for float; take all of INT, or 1 bit of B01
  if(!(aval&-2LL)){  // 0 or 1
   if(aval==1)R RETARG(w);   // 1 # y, return y
   if(!ISSPARSE(AT(w))){GA00(z,AT(w),0,AR(w)); MCISH(AS(z),AS(w),AR(w)) AS(z)[wf]=0; RETF(z);}  // 0 # y, return empty
  }
 }
 if(((1-acr)|(acr-ar))<0){z=rank2ex(a,w,DUMMYSELF,MIN(1,acr),wcr,acr,wcr,jtrepeat); PRISTCLRF(w) RETF(z);}  // multiple cells - must lose pristinity; loop if multiple cells of a
 ASSERT((-acr&-wcr)>=0||(AS(a)[0]==AS(w)[wf]),EVLENGTH);  // require agreement if neither cell is an atom
 z=(*repfn)(jtinplace,a,w,wf,wcr);
 // mark w not pristine, since we pulled from it
 PRISTCLRF(w)
 RETF(z);
}    /* a#"r w main control */
