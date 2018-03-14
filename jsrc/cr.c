/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Rank Associates                                           */

#include "j.h"

#define DR(r)           (((UI)(I)(r)>RMAX)?RMAX:r)   // return RMAX if input is negative or > RMAX


// obsolete // effective rank: ar is rank of argument, r is rank of verb (may be negative)
// obsolete // result is rank of argument cell
// obsolete I efr(I ar,I r){R 0>r?MAX(0,r+ar):MIN(r,ar);}

// obsolete #define NEWYW   {GA(yw,wt,wcn,rr,ws+wf); vv=CAV(yw);}
// obsolete // Move a cell from the argument to the temporary area y? whose data is *uu or *vv.  The temp area starts out as nonrecursive, but it is possible
// obsolete // that the previous routine turned it recursive but left it marked as inplaceable.  In that case, we might reuse the block (or we might not, if
// obsolete // it's the last block - then we will just leave it as inplaceable recursive and its descendants will be freed eventually).  We don't just ras()
// obsolete // the new block, because the ra might be unnecessary.  Instead, we check BEFORE overwriting the block, and decrement the usecount in the old descendant
// obsolete // if the block has turned recursive (to account for the increment implied in the recursiveness).  We then make sure the temp starts each cell as
// obsolete // nonrecursive
// obsolete // todo kludge BUG: the call to fa() must loop over the contents.  But this may go away if the cell becomes VIRTUAL
// obsolete #define MOVEYW  {if(UCISRECUR(yw)){fa(*(A*)vv); AFLAG(yw)&=~RECURSIBLE;} MC(vv,v,wk); if(state&STATEWREL){RELORIGIN(worg,w); RZ(yw=relocate(worg-(I)yw,yw));} v+=wk;}

#define EMSK(x) (1<<((x)-1))
#define EXIGENTERROR (EMSK(EVALLOC) | EMSK(EVATTN) | EMSK(EVBREAK) | EMSK(EVINPRUPT) | EMSK(EVFACE) | EMSK(EVWSFULL) | EMSK(EVTIME) | EMSK(EVSTACK) | EMSK(EVSYSTEM) )  // errors that always create failure

#define STATEOUTERREPEATA 0x01
#define STATEINNERREPEATA 0x04
#define STATEINNERREPEATW 0x08
// 0x10 left open because it accumulates AFNOSMREL
#define STATENORM 0x20
#define STATEFIRST 0x40
#define STATEERR0 0x80
#define STATEERR 0x100
#define STATEARELX 9
#define STATEAREL (1<<STATEARELX)
#define STATEWRELX 10
#define STATEWREL (1<<STATEWRELX)
#define STATENOPOP 0x800   // set if not OK to tpop the stack
// obsolete #define STATEINCORPORATEDA 0x1000
// obsolete #define STATEINCORPORATEDW 0x2000


#define RCALL   CALL1(f1,yw,fs)
#define RDIRECT (wt&DIRECT)
#define RARG    {if(WASINCORP1(y,yw)){cc = 0;NEWYW;} MOVEYW;}
#define RARG1   {if(WASINCORP1(y,yw)){RZ(yw=ca(yw)); vv=CAV(yw);}}

// General setup for verbs that do not go through jtirs[12].  Some of these are marked as IRS verbs.  General
// verbs derived from u"n also come through here, via jtrank2.
// A verb u["n] using this function checks to see whether it has multiple cells; if so,
// it calls here, giving a callback; we split the arguments into cells and call the callback,
// which is often the same original function that called here.
// rr is the rank at which the verb will be applied: in u"n, the smaller of rank-of-u and n
A jtrank1ex(J jt,A w,A fs,I rr,AF f1){PROLOG(0041);A y,z,virtw;
   I k,mn,n=1,wcn,wf,wk,wr,*ws,wt,yn,yr,*ys,yt;
 RZ(w);
 wt=AT(w);
 if(wt&SPARSE)R sprank1(w,fs,rr,f1);  // this needs to be updated to handle multiple ranks
 // wr=rank, ws->shape, wcr=effective rank, wf=#frame (inner+outer)
 // if inner rank is > outer rank, set it equal to outer rank
 I state=STATEFIRST|AFNOSMREL;  // initial state: working on first item, OK to pop stack, no relative contents, etc
 wr=AR(w); ws=AS(w); efr(rr,wr,rr); wf=wr-rr; state |= STATEWREL&~ARELATIVES(w);   // relies on STATEWREL>BOX
// obsolete if(ARELATIVE(w))state|=STATEWREL;
 if(!wf){R CALL1(f1,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.  Should not occur
 // multiple cells.  Loop through them.

 I wn=AN(w);  // empty-operand indicator
 // Get size of each argument cell in atoms.  If this overflows, there must be a 0 in the frame, & we will have
 // gone through the fill path (& caught the overflow)
 RE(mn=prod(wf,ws)); PROD(wcn,rr,ws+wf);   // number of cells, number of atoms in a cell
 // Allocate workarea y? to hold one cell of ?, with uu,vv pointing to the data area y?
 // ?cn=number of atoms in a cell, ?k=#bytes in a cell
 wk=wcn*bp(wt);
 // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
 // The base pointer AK advances through the source argument.  But if an operand is empty (meaning that there are no output cells),
 // replace any empty operand with a cell of fills.  (Note that operands can have no atoms and yet the result can have cells,
 // if the cells are empty but the frame does not contain 0)
 if(mn|wn){RZ(virtw = virtual(w,0,rr)); {I * virtws = AS(virtw); DO(rr, virtws[i] = ws[wf+i];)} AN(virtw)=wcn;}
 else{RZ(virtw=reshape(vec(INT,rr,ws+wf),filler(w)));}

 if(mn){I i0, old;C *zv;
  // Normal case where there are cells.
  // loop over the frame
  for(i0=mn;i0;--i0){
   RZ(y=CALL1(f1,virtw,fs));
   // see if the workarea was incorporated into the result, for use next time through the loop
   if(state&AFNOSMREL)state&=AFLAG(y)|~AFNOSMREL;  // if we ever get an SMREL (or a non-boxed result), stop looking

   // process according to state
   if(state&STATENORM){
    // Normal case: not first time, no error found yet.  Move verb result to its resting place.  zv points to the next output location
    if(TYPESNE(yt,AT(y))||yr!=AR(y)||yr&&ICMP(AS(y),ys,yr)||ARELATIVE(y)){state^=(STATENORM|STATEERR0);}  //switch to ERR0 state if there is a change of cell type/rank/shape, or result is relative
    else{
     // Normal path.  
     MC(zv,AV(y),k); zv+=k;  // move the result-cell to the output, advance to next output spot
     if(!(state&STATENOPOP))tpop(old);  // Now that we have copied to the output area, free what the verb allocated
    }
   } else if(state&STATEFIRST){I *is, zn;
    // Processing the first cell.  Allocate the result area now that we know the shape/type of the result.  If an argument is memory-mapped,
    // we have to go through the box/unbox drill (why I don't know).  In that case, we switch this allocation to be a single box per result-cell,
    // to avoid having to reallocate immediately.  We also have to do this for sparse results, so that they will be collected into a single result at the end
    yt=AT(y);  // type of the first result
    if(!( (AFLAG(w)&(AFNJA|AFSMM|AFREL)) || (yt&SPARSE) ) ){
     yr=AR(y); yn=AN(y);
     RE(zn=mult(mn,yn));   // zn=number of atoms in all result cells (if they stay homogeneous)
     state^=(STATEFIRST|STATENORM);  // advance to STATENORM
     // If the results are not going to be DIRECT, they will be allocated up the stack, and we mustn't pop the stack between results
     if(!(yt&DIRECT))state |= STATENOPOP;
    }else{
     yt=BOX; yr=0; zn=mn; state^=(STATEFIRST|STATEERR);
    }
    GA(z,yt,zn,wf+yr,0L); I *zs=AS(z); zv=CAV(z);
    is = ws; DO(wf, *zs++=*is++;);  // copy frame
    if(!(state&STATEERR)){
     ys=AS(y); k=yn*bp(yt);   // save info about the first cell for later use
     is = AS(y); DO(yr, *zs++=*is++;);    // copy result shape - if going to error, not used
     MC(zv,AV(y),k); zv+=k;   // If there was a first cell, copy it in & advance to next output spot
     old=jt->tnextpushx;  // pop back to AFTER where we allocated our result and argument blocks
    }
   }

   if(state&(STATEERR0|STATEERR)){
    if(state&STATEERR0){
     // We had a wreck.  Either the first cell was not direct, or there was a change of type.  We cope by boxing
     // each individual result, so that we can open them at the end to produce a single result (which might fail when opened)
     // It would be nice if boxed results didn't go through this path
     // If the result is boxed, it means we detected the wreck before the initial allocation.  The initial allocation
     // is the boxed area where we build <"0 result, and zv points to the first box pointer.  We have nothing to adjust.
     C *zv1=CAV(z);   // pointer to cell data
     A zsav = z; GATV(z,BOX,mn,wf,AS(zsav)); A *x=AAV(z);   // allocate place for boxed result; copy frame part of result-shape.  Note GATV reassigns z early, need zsav
     // For each previous result, put it into a box and store the address in the result area
     // We have to calculate the number of cells, rather than using the output address, because the length of a cell may be 0
     // wrecki does not include the cell that caused the wreck
     I wrecki = mn-i0;
     DQ(wrecki , A q; GA(q,yt,yn,yr,ys); MC(AV(q),zv1,k); zv1+=k; *x++=q;)  // We know the type/shape/rank of the first result matches them all
     // from now on the main output pointer, zv, points into the result area for z
     zv = (C*)x;
     state^=(STATEERR0|STATEERR);  // advance to STATEERR
    }
    // Here for all errors, including the first after it has cleaned up the mess, and for sparse result the very first time with no mess
    // we are incorporating y into the boxed z, so we have to mark it as such (and possibly reallocate it)
    INCORP(y);
    *(A*)zv=y; zv+=sizeof(A*);   // move in the most recent result, advance pointer to next one
   }
   // advance input pointer for next cell.  We keep the same virtual block because it can't be incorporated into anything
   AK(virtw)+=wk;
  }
 }else{UC d; I *is, *zs;
  // no cells - execute on a cell of fills
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  d=jt->db; jt->db=0; y=CALL1(f1,virtw,fs); jt->db=d;
  if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(y); y=zero; RESETERR;}  // use 0 as result if error encountered
  GA(z,AT(y),0L,wf+AR(y),0L); zs=AS(z);
  is = ws; DO(wf, *zs++=*is++;);  // copy frame
  is = AS(y); DO(AR(y), *zs++=*is++;);    // copy result shape
 }

 if(state&STATEERR){z=ope(z);  // If we went to error state, we have created x <@f y; this creates > x <@f y which is the final result
 }else{AFLAG(z)|=state&AFNOSMREL;}  // if not error, we saw all the subcells, so if they're all non-rel we know.  This may set NOSMREL in a non-boxed result, but that's OK
 EPILOG(z);
}

A jtrank2ex(J jt,A a,A w,A fs,I lr,I rr,I lcr,I rcr,AF f2){PROLOG(0042);A y,virta,virtw,z;
   I acn,af,ak,ar,*as,at,k,mn,n=1,wcn,wf,wk,wr,*ws,wt,yn,yr,*ys,yt;
 I outerframect, outerrptct, innerframect, innerrptct, aof, wof, sof, lof, sif, lif, *lis, *los;

 RZ(a&&w);
 at=AT(a); wt=AT(w);
 if(at&SPARSE||wt&SPARSE)R sprank2(a,w,fs,lcr,rcr,f2);  // this needs to be updated to handle multiple ranks
// lr,rr are the ranks of the underlying verb.  lcr,rcr are the cell-ranks given by u"lcr rcr.
// If " was not used, lcr,rcr=lr,rr
// When processing v"l r the shapes look like:
// a frame   x x O  | x x x
//                   <---l-->
// w frame   x x    | x x x I
//                   <---r-->
// the outer frame is to the left of the |, inner frame to the right.
// the rank of v is not included; the frames shown above pick up after that.  There are two
// possible repetitions required: if there is mismatched frame BELOW the rank (l r), as shown by letter I above,
// the individual cells of the shorter-frame argument must be repeated.  innerrptct gives the
// number of times the cell should be repeated.  If there is mismatched frame ABOVE the rank (l r), as
// shown by letter O above, the rank-(l/r) cell of the short-frame argument must be repeated.  outerrptct
// tells how many times the cell should be repeated; outerrestartpt gives the address of the rank-(l/r) cell
// being repeated; outercellct gives the number of (below lr) cells that are processed before an outer repetition.
// The two repeats can be for either argument independently, depending on which frame is shorter.

 // ?r=rank, ?s->shape, ?cr=effective rank, ?f=#total frame (inner+outer), for each argument
 // if inner rank is > outer rank, set it equal to outer rank
 I state=STATEFIRST|AFNOSMREL;  // initial state: working on first item, OK to pop stack, no relative contents, etc
 ar=AR(a); as=AS(a); efr(lr,ar,lr); efr(lcr,ar,lcr); if(lr>lcr)lr=lcr; af=ar-lr; state |= STATEAREL&~ARELATIVES(a);   // relies on STATEAREL>BOX
// obsolete if(ARELATIVE(a))state|=STATEAREL;
 wr=AR(w); ws=AS(w); efr(rr,wr,rr); efr(rcr,wr,rcr); if(rr>rcr)rr=rcr; wf=wr-rr; state |= STATEWREL&~ARELATIVES(w);   // relies on STATEWREL>BOX
 if(!af&&!wf){R CALL2(f2,a,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.  Should not occur
 // multiple cells.  Loop through them.

 // Get the length of the outer frames, which are needed only if either "-rank is greater than the verb rank,
 // either argument has frame with respect to the "-ranks, and those frames are not the same length
 aof=ar-lcr; wof=wr-rcr;   // ?of = outer frame
 if(!((lcr>lr||rcr>rr)&&((aof>0)||(wof>0))&&aof!=wof)){los=0; lof=aof=wof=0; outerframect=outerrptct=1;  // no outer frame unless it's needed
 }else{
  // outerframect is the number of cells in the shorter frame; outerrptct is the number of cells in the residual frame
  if(aof>=wof){wof=wof<0?0:wof; lof=aof; sof=wof; los=as;}else{aof=aof<0?0:aof;  lof=wof; sof=aof; los=ws; state|=STATEOUTERREPEATA;}  // clamp smaller frame at min=0
  ASSERT(!ICMP(as,ws,sof),EVLENGTH);  // prefixes must agree
  RE(outerframect=prod(sof,los)); RE(outerrptct=prod(lof-sof,los+sof));  // get # cells in frame, and in unmatched frame
 }

 // Now work on inner frames.  Compare frame lengths after discarding outer frames
 // set lif=length of longer inner frame, sif=length of shorter inner frame, lis->longer inner shape
 if((af-aof)>(wf-wof)){
  // a has the longer inner frame.  Repeat cells of w
  lif=af-aof; sif=wf-wof; lis=as+aof;
  state |= STATEINNERREPEATW;
 } else if((af-aof)<(wf-wof)){
  // w has the longer inner frame.  Repeat cells of a
  lif=wf-wof; sif=af-aof; lis=ws+wof;
  state |= STATEINNERREPEATA;
 } else{
  // inner frames are equal.  No repeats
  lif=wf-wof; sif=af-aof; lis=ws+wof;
 }
 ASSERT(!ICMP(as+aof,ws+wof,sif),EVLENGTH);  // error if frames are not same as prefix
 RE(innerrptct=prod(lif-sif,lis+sif));  // number of repetitions per matched-frame cell
 RE(innerframect=prod(sif,lis));   // number of cells in matched frame

 I an=AN(a), wn=AN(w);  // empty-operand indicators
 // Migrate loops with count=1 toward the inner to reduce overhead.  We choose not to promote the outer to the inner if both
 // innerframect & innerrptct are 1, on grounds of rarity
 if(innerrptct==1){innerrptct=innerframect; innerframect=1; state &=~(STATEINNERREPEATW|STATEINNERREPEATA);}  // if only one loop needed, make it the inner, with no repeats

 // Get size of each argument cell in atoms.  If this overflows, there must be a 0 in the frame, & we will have
 // gone through the fill path (& caught the overflow)
 PROD(acn,lr,as+af); PROD(wcn,rr,ws+wf);
 // Allocate workarea y? to hold one cell of ?, with uu,vv pointing to the data area y?
 // ?cn=number of atoms in a cell, ?k=#bytes in a cell
 ak=acn*bp(at);    // reshape below will catch any overflow
 wk=wcn*bp(wt);

 // See how many cells are going to be in the result
 RE(mn=mult(mult(outerframect,outerrptct),mult(innerframect,innerrptct)));

 // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
 // The base pointer AK advances through the source argument.  But if an operand is empty (meaning that there are no output cells),
 // replace any empty operand with a cell of fills.  (Note that operands can have no atoms and yet the result can have cells,
 // if the cells are empty but the frame does not contain 0)
 if(mn|an){RZ(virta = virtual(a,0,lr)); {I * virtas = AS(virta); DO(lr, virtas[i] = as[af+i];)} AN(virta)=acn;}
 else{RZ(virta=reshape(vec(INT,lr,as+af),filler(a)));}
 if(mn|wn){RZ(virtw = virtual(w,0,rr)); {I * virtws = AS(virtw); DO(rr, virtws[i] = ws[wf+i];)} AN(virtw)=wcn;}
 else{RZ(virtw=reshape(vec(INT,rr,ws+wf),filler(w)));}

 if(mn){I i0, i1, i2, i3, old;C *zv;
  // Normal case where there are cells.
  // loop over the matched part of the outer frame
  for(i0=outerframect;i0;--i0){
   I outerrptstart=AK(state&STATEOUTERREPEATA?virta:virtw);
   // loop over the unmatched part of the outer frame, repeating the shorter argument
   for(i1=outerrptct;i1;--i1){  // make MOVEY? post-increment
    AK(state&STATEOUTERREPEATA?virta:virtw)=outerrptstart;   // if we loop, we know we must be repeating one or the other
    // loop over the matched part of the inner frame
    for(i2=innerframect;i2;--i2){
     // loop over the unmatched part of the inner frame, repeating the shorter argument
     for(i3=innerrptct;i3;--i3){
      // invoke the function, get the result for one cell
      RZ(y=CALL2(f2,virta,virtw,fs));
      // if the result is boxed, accumulate the SMREL info
      if(state&AFNOSMREL)state&=AFLAG(y)|~AFNOSMREL;  // if we ever get an SMREL (or a non-boxed result), stop looking

      // process according to state
      if(state&STATENORM){
       // Normal case: not first time, no error found yet.  Move verb result to its resting place.  zv points to the next output location
       if(TYPESNE(yt,AT(y))||yr!=AR(y)||yr&&ICMP(AS(y),ys,yr)||ARELATIVE(y)){state^=(STATENORM|STATEERR0);}  //switch to ERR0 state if there is a change of cell type/rank/shape, or result is relative
       else{
        // Normal path.
        MC(zv,AV(y),k); zv+=k;  // move the result-cell to the output, advance to next output spot
          // If the result-cells are pointers to boxes, we are adding a nonrecursive reference, which does not require any adjustment to usecounts.
          // If we anticipate making the result recursive, we will have to increment the usecount and also worry about backing out errors and wrecks.
        if(!(state&STATENOPOP))tpop(old);  // Now that we have copied to the output area, free what the verb allocated
       }
      } else if(state&STATEFIRST){I *is, zn;
       // Processing the first cell.  Allocate the result area now that we know the shape/type of the result.  If an argument is memory-mapped,
       // we have to go through the box/unbox drill, because the blocks coming out will have different relocations that must be preserved.
       //  In that case, we switch this allocation to be a single box per result-cell.
       //  We also have to do this for sparse results, so that they will be collected into a single result at the end
       yt=AT(y);  // type of the first result
       if(!( ((AFLAG(a)|AFLAG(w))&(AFNJA|AFSMM|AFREL)) || (yt&SPARSE) ) ){
        yr=AR(y); yn=AN(y);
        RE(zn=mult(mn,yn));   // zn=number of atoms in all result cells (if they stay homogeneous)
        state^=(STATEFIRST|STATENORM);  // advance to STATENORM
        // If the results are not going to be DIRECT, they will be allocated up the stack, and we mustn't pop the stack between results
        if(!(yt&DIRECT))state |= STATENOPOP;
       }else{
        yt=BOX; yr=0; zn=mn; state^=(STATEFIRST|STATEERR);
       }
       GA(z,yt,zn,lof+lif+yr,0L); I *zs=AS(z); zv=CAV(z);
       is = los; DO(lof, *zs++=*is++;);  // copy outer frame
       is = lis; DO(lif, *zs++=*is++;);  // copy inner frame
       if(!(state&STATEERR)){
        ys=AS(y); k=yn*bp(yt);   // save info about the first cell for later use
        is = AS(y); DO(yr, *zs++=*is++;);    // copy result shape
        MC(zv,AV(y),k); zv+=k;   // If there was a first cell, copy it in & advance to next output spot
        old=jt->tnextpushx;  // pop back to AFTER where we allocated our result and argument blocks
       }
      }

      if(state&(STATEERR0|STATEERR)){
       if(state&STATEERR0){
        // We had a wreck.  Either the first cell was not direct/boxed, or there was a change of type.  We cope by boxing
        // each individual result, so that we can open them at the end to produce a single result (which might fail when opened)
        // If the result is boxed, it means we detected the wreck before the initial allocation.  The initial allocation
        // is the boxed area where we build <"0 result, and zv points to the first box pointer.  We have nothing to adjust.
        C *zv1=CAV(z);   // pointer to cell data
        A zsav=z; GATV(z,BOX,mn,lof+lif,AS(zsav)); A *x=AAV(z);   // allocate place for boxed result; copy frame part of result-shape.  Note GATV reassigns z early, need zsav
        // For each previous result, put it into a box and store the address in the result area
        // We have to calculate the number of cells, rather than using the output address, because the length of a cell may be 0
        // wrecki does not include the cell that caused the wreck
        I wrecki = (innerrptct-i3) + innerrptct * ( (innerframect-i2) + innerframect * ( (outerrptct-i1) + outerrptct * (outerframect-i0) ) );
        DQ(wrecki , A q; GA(q,yt,yn,yr,ys); MC(AV(q),zv1,k); zv1+=k; *x++=q;)  // We know the type/shape/rank of the first result matches them all
        // from now on the main output pointer, zv, points into the result area for z
        zv = (C*)x;
        state^=(STATEERR0|STATEERR);  // advance to STATEERR
       }
       // Here for all errors, including the first after it has cleaned up the mess, and for sparse result the very first time with no mess
       // we are incorporating y into the boxed z, so we have to mark it as such (and possibly reallocate it)
       INCORP(y);
       *(A*)zv=y; zv+=sizeof(A*);   // move in the most recent result, advance pointer to next one
      }
      // advance input pointers for next cell.  We keep the same virtual block because it can't be incorporated into anything
      if(!(state&STATEINNERREPEATA))AK(virta)+=ak;
      if(!(state&STATEINNERREPEATW))AK(virtw)+=wk;
     }
      // advance input pointers for next cell.  We increment any block that was being held constant in the inner loop.  There can be only one such
     if(state&STATEINNERREPEATA)AK(virta)+=ak;
     if(state&STATEINNERREPEATW)AK(virtw)+=wk;
    }
   }
  }
 }else{UC d; I *is, *zs;
  // if there are no cells, execute on a cell of fills.
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  d=jt->db; jt->db=0; y=CALL2(f2,virta,virtw,fs); jt->db=d;
  if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(y); y=zero; RESETERR;}  // use 0 as result if error encountered
  GA(z,AT(y),0L,lof+lif+AR(y),0L); zs=AS(z);
  is = los; DO(lof, *zs++=*is++;);  // copy outer frame
  is = lis; DO(lif, *zs++=*is++;);  // copy inner frame
  is = AS(y); DO(AR(y), *zs++=*is++;);    // copy result shape
 }

 if(state&STATEERR){z=ope(z);  // If we went to error state, we have created x <@f y; this creates > x <@f y which is the final result
 }else{AFLAG(z)|=state&AFNOSMREL;}  // if not error, we saw all the subcells, so if they're all non-rel we know.  This may set NOSMREL in a non-boxed result, but that's OK
 EPILOG(z);
}

/* Integrated Rank Support                              */
/* f knows how to compute f"r                           */
/* jt->rank points to a 2-element vector of             */
/* (left, right (monadic)) ranks                        */
/* 0=jt->rank means f is not being called from rank     */
/* jt->rank is guaranteed positive                      */
/* jt->rank is guaranteed <: argument ranks             */
/* frame agreement is verified before invoking f        */
/* frames either match, or one is empty                 */
/* (i.e. prefix agreement invokes general case)         */
// If the action verb handles inplacing, we pass that through

A jtirs1(J jt,A w,A fs,I m,AF f1){A z;I*old,rv[2],wr; 
 F1PREFIP; RZ(w);
 wr=AR(w); rv[1]=efr(m,wr,m);
 if(fs&&!(VAV(fs)->flag&VINPLACEOK1))jtinplace=jt;  // pass inplaceability only if routine supports it
 if(m>=wr)R CALL1IP(f1,w,fs);
 rv[0]=0;
 old=jt->rank; jt->rank=rv; z=CALL1IP(f1,w,fs); jt->rank=old; 
 RETF(z);
}

// IRS setup for dyads x op y
// a is x, w is y
// fs is the f field of the verb (the verb to be applied repeatedly) - or 0 if none
//  if inplacing is enabled in jt, fs must be given
// l, r are nominal ranks of fs
// f2 is a setup verb (jtover, jtreshape, etc)
// IRS verbs are those that look at jt->rank.  This is where we set up jt->rank.  Once
// we have it, we call the setup verb, which will go on to do its internal looping and (optionally) call
// the verb f2 to finish operation on a cell
A jtirs2(J jt,A a,A w,A fs,I l,I r,AF f2){A z;I af,ar,*old,rv[2],wf,wr;
 // push the jt->rank (pointer to ranks) stack.  push/pop may not match, no problem
 F1PREFIP; RZ(a&&w);
 ar=AR(a); rv[0]=efr(l,ar,l); af=ar-l;  // get rank, effective rank of u"n, length of frame...
 wr=AR(w); rv[1]=efr(r,wr,r); wf=wr-r;     // ...for both args
 if(fs&&!(VAV(fs)->flag&VINPLACEOK2))jtinplace=jt;  // pass inplaceability only if routine supports it
 if(!(af||wf))R CALL2IP(f2,a,w,fs);   // if no frame, call setup verb and return result
 ASSERT(!ICMP(AS(a),AS(w),MIN(af,wf)),EVLENGTH);   // verify agreement
 old=jt->rank; jt->rank=rv; z=CALL2IP(f2,a,w,fs); jt->rank=old;   // save ranks, call setup verb, pop rank stack
  // Not all setup verbs (*f2)() use the fs argument.  
 RETF(z);
}


static DF1(cons1a){R VAV(self)->f;}
static DF2(cons2a){R VAV(self)->f;}

// Constant verbs do not inplace because we loop over cells.  We could speed this up if it were worthwhile.
static DF1(cons1){V*sv=VAV(self);
 RZ(w);
 I mr; efr(mr,AR(w),*AV(sv->h));
 R rank1ex(w,self,mr,cons1a);
}
static DF2(cons2){V*sv=VAV(self);I*v=AV(sv->h);
 RZ(a&&w);
 I lr2,rr2; efr(lr2,AR(a),v[1]); efr(rr2,AR(w),v[2]);
 R rank2ex(a,w,self,AR(a),AR(w),lr2,rr2,cons2a);
}

// Handle u"n y where u supports irs.  Since the verb may support inplacing even with rank (,"n for example), pass that through.
// If inplacing is allowed here, pass that on to irs.  It will see whether the action verb can support inplacing.
// NOTHING HERE MAY DEREFERENCE jt!!
static DF1(rank1i){DECLF;A h=sv->h;I*v=AV(h); R irs1(w,fs,*v,f1);}
static DF2(rank2i){DECLF;A h=sv->h;I*v=AV(h); R irs2(a,w,fs,v[1],v[2],f2);}

// u"n y when u does not support irs. We loop over cells, and as we do there is no reason to enable inplacing
static DF1(rank1){DECLF;A h=sv->h;I m,*v=AV(h),wr;
 RZ(w);
 wr=AR(w); efr(m,wr,v[0]);
 R m<wr?rank1ex(w,fs,m,f1):CALL1(f1,w,fs);
}

// For the dyads, rank2ex does a quadruply-nested loop over two rank-pairs, which are the n in u"n (stored in h) and the rank of u itself (fetched from u).
static DF2(rank2){DECLF;A h=sv->h;I ar,l=AV(h)[1],r=AV(h)[2],wr;
 RZ(a&&w);
 ar=AR(a); efr(l,ar,l);
 wr=AR(w); efr(r,wr,r);
 if(l<ar||r<wr) {I llr=VAV(fs)->lr, lrr=VAV(fs)->rr;  // fetch ranks of werb we are going to call
  // if the verb we are calling is another u"n, we can skip coming through here a second time & just go to the f2 for the nested rank
  if(f2==rank2&&!(AT(a)&SPARSE||AT(w)&SPARSE)){fs = VAV(fs)->f; f2=VAV(fs)->f2;}
  R rank2ex(a,w,fs,llr,lrr,l,r,f2);
 }else R CALL2(f2,a,w,fs);  // pass in verb ranks to save a level of rank processing if not infinite
}


// a"w; result is a verb
F2(jtqq){A h,t;AF f1,f2;D*d;I *hv,n,r[3],vf,*v;
 RZ(a&&w);
 // The h value in the function will hold the ranks from w.  Allocate it
 GAT(h,INT,3,1,0); hv=AV(h);  // hv->rank[0]
 if(VERB&AT(w)){
  // verb v.  Extract the ranks into a floating-point list
  GAT(t,FL,3,1,0); d=DAV(t);
  n=r[0]=hv[0]=mr(w); d[0]=n<=-RMAX?-inf:RMAX<=n?inf:n;
  n=r[1]=hv[1]=lr(w); d[1]=n<=-RMAX?-inf:RMAX<=n?inf:n;
  n=r[2]=hv[2]=rr(w); d[2]=n<=-RMAX?-inf:RMAX<=n?inf:n;
  // The floating-list is what we will call the v operand into rank?ex.  It holds the nominal verb ranks which may be negative
  // h is the integer version
  w=t;
 }else{
  // Noun v. Extract and turn into 3 values, stored in h
  n=AN(w);
  ASSERT(1>=AR(w),EVRANK);
  ASSERT(0<n&&n<4,EVLENGTH);
  RZ(t=vib(w)); v=AV(t);
  hv[0]=v[2==n]; r[0]=DR(hv[0]);
  hv[1]=v[3==n]; r[1]=DR(hv[1]);
  hv[2]=v[n-1];  r[2]=DR(hv[2]);
 }
 // r is the actual verb ranks, never negative.

 // Get the action routines and flags to use for the derived verb
 if(NOUN&AT(a)){f1=cons1; f2=cons2; ACIPNO(a);// use the constant routines for nouns; mark the constant non-inplaceable since it may be reused;
  // Mark the noun as non-inplaceable.  If the derived verb is used in another sentence, it must first be
  // assigned to a name, which will protects values inside it.
  ACIPNO(a);
  vf=VASGSAFE;    // the noun can't mess up assignment, and does not support IRS
 }else{
  // The flags for u indicate its IRS and atomic status.  If atomic (for monads only), ignore the rank, just point to
  // the action routine for the verb.  Otherwise, choose the appropriate rank routine, depending on whether the verb
  // supports IRS.  The IRS verbs may profitably support inplacing, so we enable it for them.
  V* av=VAV(a);   // point to verb info
  vf=av->flag&VASGSAFE;  // inherit ASGSAFE from u
  if(av->flag&VISATOMIC1){f1=av->f1;}else if(av->flag&VIRS1){f1=rank1i;vf|=VINPLACEOK1;}else{f1=rank1;}
  if(av->flag&VIRS2){f2=rank2i;vf|=VINPLACEOK2;}else{f2=rank2;}
 }

 // Create the derived verb.  The derived verb (u"n) NEVER supports IRS; it inplaces if the action verb u supports irs
 R fdef(CQQ,VERB, f1,f2, a,w,h, vf, r[0],r[1],r[2]);
}
