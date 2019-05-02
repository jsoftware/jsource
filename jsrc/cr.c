/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Rank Associates                                           */

#include "j.h"

#define DR(r)           (((UI)(I)(r)>RMAX)?RMAX:r)   // return RMAX if input is negative or > RMAX

#define ZZDEFN
#include "result.h"

// make sure these don't overlap with bits in result.h
#define STATEOUTERREPEATAX 12
#define STATEOUTERREPEATA (((I)1)<<STATEOUTERREPEATAX)
#define STATEINNERREPEATWX 13
#define STATEINNERREPEATW (((I)1)<<STATEINNERREPEATWX)
#define STATEINNERREPEATAX 14
#define STATEINNERREPEATA (((I)1)<<STATEINNERREPEATAX)
// There must be NO higher bits than STATEINNERREPEATA, because we shift down and OR into flags

// General setup for verbs that do not go through jtirs[12].  Some of these are marked as IRS verbs.  General
// verbs derived from u"n also come through here, via jtrank2.
// A verb u["n] using this function checks to see whether it has multiple cells; if so,
// it calls here, giving a callback; we split the arguments into cells and call the callback,
// which is often the same original function that called here.
// rr is the rank at which the verb will be applied: in u"n, the smaller of rank-of-u and n
A jtrank1ex(J jt,AD * RESTRICT w,A fs,I rr,AF f1){F1PREFIP;PROLOG(0041);A z,virtw;
   I mn,n=1,wcn,wf,wk,wr,*ws,wt;
 RZ(w);
 wt=AT(w);
 if(wt&SPARSE)R sprank1(w,fs,rr,f1);
#define ZZFLAGWORD state
 I state=0;  // init flags, including zz flags
 // wr=rank, ws->shape, wcr=effective rank, wf=#frame (inner+outer)
 // if inner rank is > outer rank, set it equal to outer rank
 wr=AR(w); ws=AS(w); efr(rr,wr,rr);  // get rank at which to apply the verb
 // RANKONLY verbs were handled in the caller to this routine, but fs might be RANKATOP.  In that case we could include its rank in the loop here,
 // if its rank is not less than the outer rank (we would simply ignore it), but we don't bother.  If its rank is smaller we can't ignore it because assembly might affect
 // the order of fill.  But if f is BOXATOP, there will be no fill, and we can safely use the smaller rank
 if(fs&&FAV(fs)->flag2&VF2BOXATOP1){
  I mr=FAV(fs)->mr; efr(rr,mr,rr);
  state = (FAV(fs)->flag2&VF2BOXATOP1)>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // If this is BOXATOP, set so for loop.  Don't touch fs yet, since we might not loop
  state &= ~((FAV(fs)->flag2&VF2ATOPOPEN1)>>(VF2ATOPOPEN1X-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
  // if we are using the BOXATOP from f, we can also use the raze flags.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-state) & (I)jtinplace & (JTWILLBEOPENED|JTCOUNTITEMS);
 }
 wf=wr-rr;
 if(!wf){R CALL1IP(f1,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.
 // multiple cells.  Loop through them.
 // Get size of each argument cell in atoms.  If this overflows, there must be a 0 in the frame, & we will have
 // gone through the fill path (& caught the overflow)
 RE(mn=prod(wf,ws)); PROD(wcn,rr,ws+wf);   // number of cells, number of atoms in a cell
 // ?cn=number of atoms in a cell, ?k=#bytes in a cell
 wk=wcn<<bplg(wt);

 A zz=0;  // place where we will build up the homogeneous result cells
 if(mn){I i0;
  // Normal case where there are cells.
  // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
  // The base pointer AK advances through the source argument. 

  fauxblock(virtwfaux);
  // if the original block was direct inplaceable, make the virtual block inplaceable.  (We can't do this for indirect blocks because a virtual block is not marked recursive - rather it increments
  // the usecount of the entire backing block - and modifying the virtual contents would leave the usecounts invalid if the backing block is recursive.  Maybe could do this if it isn't?)
  // Don't pass WILLOPEN status - we use that at this level
  jtinplace = (J)((I)jtinplace & (~(JTWILLBEOPENED+JTCOUNTITEMS)) & ((((wt&TYPEVIPOK)!=0)&(AC(w)>>(BW-1)))*JTINPLACEW-(JTINPLACEW<<1)));  // turn off inplacing unless DIRECT and w is inplaceable
  fauxvirtual(virtw,virtwfaux,w,rr,ACUC1|ACINPLACE) MCISH(AS(virtw),ws+wf,rr); AN(virtw)=wcn;
  // mark the virtual block inplaceable; this will be ineffective unless the original w was direct inplaceable, and inplacing is allowed by u
#define ZZDECL
#include "result.h"
  ZZPARMS(wf,mn,1)
#define ZZINSTALLFRAME(optr) MCISHd(optr,ws,wf)
  for(i0=mn;i0;--i0){
   AC(virtw)=ACUC1|ACINPLACE;   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block
   RZ(z=CALL1IP(f1,virtw,fs));

#define ZZBODY  // assemble results
#include "result.h"

   // advance input pointer for next cell.  We keep the same virtual block because it can't be incorporated into anything; but the virtual block was inplaceable the
   // AK, AN, AR, AS, AT fields may have been modified.  We restore them
   AK(virtw) += wk;
  }

#define ZZEXIT
#include "result.h"

 }else{UC d; I *zzs;
  // no cells - execute on a cell of fills
  RZ(virtw=reshape(vec(INT,rr,ws+wf),filler(w)));  // The cell of fills
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  d=jt->uflags.us.cx.cx_c.db; jt->uflags.us.cx.cx_c.db=0; z=CALL1(f1,virtw,fs); jt->uflags.us.cx.cx_c.db=d;
  if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num[0]; RESETERR;}  // use 0 as result if error encountered
  GA(zz,AT(z),0L,wf+AR(z),0L); zzs=AS(zz); MCISH(zzs,ws,wf); MCISH(zzs+wf,AS(z),AR(z));
 }

// result is now in zz

 EPILOG(zz);
}

// Streamlined version when rank is 0.  In this version we look for ATOPOPEN (i. e. each and every)
// f1 is the function to use if there are no flags, OR if there is just 1 cell with no frame or a cell of fill 
A jtrank1ex0(J jt,AD * RESTRICT w,A fs,AF f1){F1PREFIP;PROLOG(0041);A z,virtw;
   I mn,n=1,wk,wr,*ws,wt;
 RZ(w);
 wr=AR(w);   // rank of w
 if(!wr){R CALL1IP(f1,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.  Make this as fast as possible.
 // Switch to sparse code if argument is sparse
 wt=AT(w);
 if(wt&SPARSE)R sprank1(w,fs,0,f1);
#define ZZFLAGWORD state
 // wr=rank, ws->shape
 // Each cell is an atom.  Get # atoms (=#result cells)
 // ?k=#bytes in a cell, ?s->shape
 ws=AS(w); mn=AN(w); wk=bpnoun(wt);

 A zz=0;  // place where we will build up the homogeneous result cells

 // Look for the forms we handle specially: <@:f (not here where rank=0)  <@f  f@>   and their combinations  <@(f@>) f&.> (<@:f)@>  but not as combinations  (<@f)@> (unless f has rank _) <@:(f@>)   also using &
 I state=0;

 if(mn){  // if no cells, go handle fill before we advance over flags
  // Here there are cells to execute on.  Collect ATOP flags

  // RANKONLY verbs contain an invalid f1 pointer (it was used to get to a call to here).  We have to step over the RANKONLY to get to what we can execute
  while(FAV(fs)->flag2&VF2RANKONLY1){fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[0];}

  while(1){  // loop collecting ATOPs
   I fstate=(FAV(fs)->flag2&(VF2BOXATOP1|VF2ATOPOPEN1))>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // extract <@ and @> status bits from f
   if(fstate&state||!fstate)break;  // If this f overlaps with old, or it's not just a flag node, we have to stop
   if(fstate&ZZFLAGATOPOPEN1){
    // @> &> &.>
    //  Advance to the f of f@>
    fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[0];
   }else{
    // <@: <@ <& <&:
    // Because the outermost rank is 0, <@f by itself is OK; but later, as in (<@f)@>, it is not.  <@:f is.  So check for infinite rank
    if(state&ZZFLAGATOPOPEN1 && FAV(fs)->mr<RMAX)break;  // not first, and not infinite rank: ignore
    // Advance fs to the g of <@g
    fs=FAV(fs)->fgh[1+((FAV(fs)->flag2>>VF2ISCCAPX)&1)]; f1=FAV(fs)->valencefns[0];
   }
   state|=fstate;  // We accepted the new f, so take its flags
  }

  A *wav;   // virtwk is offset of virtual block/pointer to next box
  // Normal case where there are cells.
  // RANKONLY verbs were handled in the caller to this routine, but fs might be RANKATOP.  In that case we could include its rank in the loop here,
  // if its rank is not less than the outer rank (we would simply ignore it), but we don't bother.  If its rank is smaller we can't ignore it because assembly might affect
  // the order of fill.  But if f is BOXATOP, there will be no fill, and we can safely use the smaller rank
#define ZZDECL
#include "result.h"
  ZZPARMSNOFS(wr,mn)
  // if we are using the BOXATOP from f, we can also use the raze flags from the caller.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-(state&ZZFLAGBOXATOP)) & (I)jtinplace & (JTWILLBEOPENED+JTCOUNTITEMS);

  // Now that we have handled the structural requirements of ATOPOPEN, clear it if w is not open
  // Allocate a non-in-place virtual block unless this is ATOPOPEN and w is boxed, in which case we will just use the value of the A block
  fauxblock(virtwfaux);
  if(!(state&ZZFLAGATOPOPEN1)||!(wt&BOX)){
   fauxvirtual(virtw,virtwfaux,w,0,ACUC1); AN(virtw)=1; state&=~ZZFLAGATOPOPEN1;
  }else{wav=AAV(w); virtw=*wav++;}
#define ZZINSTALLFRAME(optr) MCISHd(optr,ws,wr)
  do{
   RZ(z=CALL1(f1,virtw,fs));

#define ZZBODY  // assemble results
#include "result.h"

   if(--mn==0)break;  // exit loop before last fetch to avoid fetching out of bounds
   // advance input pointer for next cell.  We keep the same virtual block because it can't be incorporated into anything
   if(!(state&ZZFLAGATOPOPEN1)){AK(virtw) += wk;}else{virtw=*wav++;}
  }while(1);

#define ZZEXIT
#include "result.h"

 }else{UC d; I *zzs;
  // no cells - execute on a cell of fills
  RZ(virtw=filler(w));  // The cell of fills
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  if(!(FAV(fs)->flag2&VF2BOXATOP1)){
   d=jt->uflags.us.cx.cx_c.db; jt->uflags.us.cx.cx_c.db=0; z=CALL1(f1,virtw,fs); jt->uflags.us.cx.cx_c.db=d;   // normal execution on fill-cell
   if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num[0]; RESETERR;}  // use 0 as result if error encountered
  }else{
   // If we are executing a BOXATOP on a single cell, we know the result is going to be an atomic box.  We don't bother executing the verb at all then.
   // jmf.ijs unknowingly takes advantage of this fact, and would crash if executed on an empty cell
   z=ace;  // cell 'returned' a:
  }
  GA(zz,AT(z),0L,wr+AR(z),0L); zzs=AS(zz); MCISH(zzs,ws,wr); MCISH(zzs+wr,AS(z),AR(z));
 }

// result is now in zz

 EPILOG(zz);
}

A jtrank2ex(J jt,AD * RESTRICT a,AD * RESTRICT w,A fs,I lr,I rr,I lcr,I rcr,AF f2){F2PREFIP;PROLOG(0042);A virta,virtw,z;
   I acn,af,ak,ar,*as,at,mn,n=1,wcn,wf,wk,wr,*ws,wt;
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
#define ZZFLAGWORD state
 I ZZFLAGWORD=0;  // init flags, including zz flags
 ar=AR(a); as=AS(a); efr(lcr,ar,lcr); efr(lr,lcr,lr);
 wr=AR(w); ws=AS(w); efr(rcr,wr,rcr); efr(rr,rcr,rr);

 // RANKONLY verbs were handled in the caller to this routine, but fs might be RANKATOP.  In that case we can include its rank in the loop here, which will save loop setups
 if(fs&&(I)(((FAV(fs)->flag2&(VF2RANKATOP2|VF2BOXATOP2))-1)|(-((rr^rcr)|(lr^lcr))))>=0){  // prospective new ranks to include
  efr(lr,lr,(I)FAV(fs)->lr); efr(rr,rr,(I)FAV(fs)->rr);  // get the ranks if we accept the new cell
  state = (FAV(fs)->flag2&VF2BOXATOP2)>>(VF2BOXATOP2X-ZZFLAGBOXATOPX);  // If this is BOXATOP, set so for loop.  Don't touch fs yet, since we might not loop
  state &= ~((FAV(fs)->flag2&VF2ATOPOPEN2W)>>(VF2ATOPOPEN2WX-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
  // if we are using the BOXATOP from f, we can also use the raze flags.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-state) & (I)jtinplace & (JTWILLBEOPENED|JTCOUNTITEMS);
 }

 af=ar-lr; wf=wr-rr;   // frames wrt innermost cell
 if(!(af|wf)){R CALL2IP(f2,a,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.
 // multiple cells.  Loop through them.

 // Get the length of the outer frames, which are needed only if either "-rank is greater than the verb rank,
 // either argument has frame with respect to the "-ranks, and those frames are not the same length
 aof=ar-lcr; wof=wr-rcr;   // ?of = outer frame
 if(0<=(((lr-lcr)|(rr-rcr))&(-(aof^wof)))){los=0; lof=aof=wof=0; outerframect=outerrptct=1;  // no outer frame unless it's needed
 }else{
  // outerframect is the number of cells in the shorter frame; outerrptct is the number of cells in the residual frame
  if(aof>=wof){wof=wof<0?0:wof; lof=aof; sof=wof; los=as;}else{aof=aof<0?0:aof;  lof=wof; sof=aof; los=ws; state|=STATEOUTERREPEATA;}  // clamp smaller frame at min=0
  ASSERTAGREE(as,ws,sof)  // prefixes must agree
  RE(outerframect=prod(sof,los)); RE(outerrptct=prod(lof-sof,los+sof));  // get # cells in frame, and in unmatched frame
 }

 // Now work on inner frames.  Compare frame lengths after discarding outer frames
 // set lif=length of longer inner frame, sif=length of shorter inner frame, lis->longer inner shape
 if((af-aof)==(wf-wof)){
  // inner frames are equal.  No repeats
  lif=wf-wof; sif=af-aof; lis=ws+wof;
 } else if((af-aof)<(wf-wof)){
  // w has the longer inner frame.  Repeat cells of a
  lif=wf-wof; sif=af-aof; lis=ws+wof;
  state |= STATEINNERREPEATA;
 } else{
  // a has the longer inner frame.  Repeat cells of w
  lif=af-aof; sif=wf-wof; lis=as+aof;
  state |= STATEINNERREPEATW;
 }
 ASSERTAGREE(as+aof,ws+wof,sif)  // error if frames are not same as prefix
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
 ak=acn<<bplg(at);    // reshape below will catch any overflow
 wk=wcn<<bplg(wt);

 // See how many cells are going to be in the result
 RE(mn=mult(mult(outerframect,outerrptct),mult(innerframect,innerrptct)));

 // See which arguments we can inplace.  The key is that they have to be not repeated.  This means outerrptct=1, and the specified argument not repeated in the inner loop.  Also,
 // a and w mustn't be the same block (one cannot be a virtual of the other unless the backer's usecount disables inplacing)
 // Don't pass WILLOPEN to the callee - it si consumed at this level
 jtinplace = (J)(intptr_t)((I)jtinplace & (~(JTWILLBEOPENED+JTCOUNTITEMS)) & ~(((I )(a==w)|(I )(outerrptct!=1))*(JTINPLACEA+JTINPLACEW)|(state>>STATEINNERREPEATWX)));  // turn off inplacing if variable is inner-repeated, or any outer repeat, or identical args

 // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
 // The base pointer AK advances through the source argument.  But if an operand is empty (meaning that there are no output cells),
 // replace any empty operand with a cell of fills.  (Note that operands can have no atoms and yet the result can have nonempty cells,
 // if the cells are empty but the frame does not contain 0)
 //
 // Self-virtual blocks modify the shape of a block, but that code notifies
 // us through a flag bit.
 fauxblock(virtwfaux); fauxblock(virtafaux); 
 if(mn|an){
  jtinplace = (J)(intptr_t)((I)jtinplace & ((((at&TYPEVIPOK)!=0)&(AC(a)>>(BW-1)))*JTINPLACEA+~JTINPLACEA));  // turn off inplacing unless DIRECT and a is inplaceable.
  fauxvirtual(virta,virtafaux,a,lr,ACUC1|ACINPLACE) MCISH(AS(virta),as+af,lr); AN(virta)=acn;
 }else{RZ(virta=reshape(vec(INT,lr,as+af),filler(a)));}

 if(mn|wn){  // repeat for w
  jtinplace = (J)(intptr_t)((I)jtinplace & ((((wt&TYPEVIPOK)!=0)&(AC(w)>>(BW-1)))*JTINPLACEW+~JTINPLACEW));  // turn off inplacing unless DIRECT and w is inplaceable.
  fauxvirtual(virtw,virtwfaux,w,rr,ACUC1|ACINPLACE) MCISH(AS(virtw),ws+wf,rr); AN(virtw)=wcn;
 }else{RZ(virtw=reshape(vec(INT,rr,ws+wf),filler(w)));}

 A zz=0;  // place where we will build up the homogeneous result cells
 if(mn){I i0, i1, i2, i3;
  // Normal case where there are cells.
  // loop over the matched part of the outer frame

#define ZZDECL
#include "result.h"
  ZZPARMS(lof+lif,mn,2)
#define ZZINSTALLFRAME(optr) MCISHd(optr,los,lof) MCISHd(optr,lis,lif)

  for(i0=outerframect;i0;--i0){
   I outerrptstart=AK(state&STATEOUTERREPEATA?virta:virtw);
   // loop over the unmatched part of the outer frame, repeating the shorter argument
   for(i1=outerrptct;i1;--i1){  // make MOVEY? post-increment
    AK(state&STATEOUTERREPEATA?virta:virtw) = outerrptstart;
    // loop over the matched part of the inner frame
    for(i2=innerframect;i2;--i2){
     // loop over the unmatched part of the inner frame, repeating the shorter argument
     for(i3=innerrptct;i3;--i3){
      AC(virta)=ACUC1|ACINPLACE;   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block
      AC(virtw)=ACUC1|ACINPLACE; 
      // invoke the function, get the result for one cell
      RZ(z=CALL2IP(f2,virta,virtw,fs));

#define ZZBODY  // assemble results
#include "result.h"

      // advance input pointers for next cell.  We keep the same virtual block because it can't be incorporated into anything; but if the virtual block was inplaceable the
   // AK, AN, AR, AS, AT fields may have been modified.  We restore them
      if(!(state&STATEINNERREPEATA)){
       AK(virta) += ak;
      }

      if(!(state&STATEINNERREPEATW)){
       AK(virtw) += wk;
      }
     }
      // advance input pointers for next cell.  We increment any block that was being held constant in the inner loop.  There can be only one such.  Such an arg is never inplaced
     if(state&STATEINNERREPEATA)AK(virta) += ak;
     else if(state&STATEINNERREPEATW)AK(virtw) += wk;
    }
   }
  }

#define ZZEXIT
#include "result.h"

 }else{UC d; I *zzs;
  // if there are no cells, execute on a cell of fills.
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  d=jt->uflags.us.cx.cx_c.db; jt->uflags.us.cx.cx_c.db=0; z=CALL2(f2,virta,virtw,fs); jt->uflags.us.cx.cx_c.db=d;
  if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num[0]; RESETERR;}  // use 0 as result if error encountered
  GA(zz,AT(z),0L,lof+lif+AR(z),0L); zzs=AS(zz);
  MCISH(zzs,los,lof); MCISH(zzs+lof,lis,lif); MCISH(zzs+lof+lif,AS(z),AR(z));
 }

// result is now in zz
 EPILOG(zz);
}

// version for rank 0.  We look at ATOPOPEN too.  f2 is the function to use if there is no frame
A jtrank2ex0(J jt,AD * RESTRICT a,AD * RESTRICT w,A fs,AF f2){F2PREFIP;PROLOG(0042);A virta,virtw,z;
   I ak,ar,*as,at,ict,oct,mn,wk,wr,*ws,wt;
 RZ(a&&w); ar=AR(a); wr=AR(w); if(!(ar|wr))R CALL2IP(f2,a,w,fs);   // if no frame, make just 1 call
 at=AT(a); wt=AT(w);
 if(at&SPARSE||wt&SPARSE)R sprank2(a,w,fs,0,0,f2);  // this needs to be updated to handle multiple ranks
#define ZZFLAGWORD state

 // Verify agreement
 as=AS(a); ws=AS(w); ASSERTAGREE(as,ws,MIN(ar,wr))

 // Calculate strides for inner and outer loop.  Cell-size is known to be 1 atom.  The stride of the inner loop is 1 atom, except for a
 // repeated value, of which there can be at most 1.  For a repeated value, we set the stride to 0 and remember the repetition count and stride
 ak=bpnoun(at); wk=bpnoun(wt);  // stride for 1 atom
 if(ar>=wr) { // a will be the long frame
  mn=AN(a);  // result has shape of longer frame, & same # atoms
  if(ar==wr){  // no surplus frame: common case
   ict=mn; oct=0; // leave frames as is, set loop counters
  }else{
   PROD(oct,wr,as); PROD(ict,ar-wr,as+wr); wr=wk; wk=0;  // set repeat counts, outer/inner strides
  }
 }else{
   mn=AN(w);
   PROD(oct,ar,ws); PROD(ict,wr-ar,ws+ar); as=ws; ar=wr; wr=ak; ak=0;  // set repeat counts, outer/inner strides
 }
 // Now as/ar are the addr/length of the long frame
 // wr is the atomsize*repeatct for the repeated arg
 // ak/wk are strides, 0 for a repeated arg
 // ict is the inner repeat count: length of surplus frame if any, else length of args
 // oct is the outer repeat count: length of common frame if there is a surplus frame, else 0



 // Look for the forms we handle specially: <@:f (not here where rank=0)  <@f  f@>   and their combinations  <@(f@>) f&.> (<@:f)@>  but not as combinations  (<@f)@> (unless f has rank _) <@:(f@>)   also using &
 // For the nonce, we assume that VF2ATOPOPEN2A and VF2ATOPOPEN2W are always the same
 I state=0;

 A zz=0;  // place where we will build up the homogeneous result cells
 fauxblock(virtafaux);  fauxblock(virtwfaux);

 if(mn){
  // Collect flags <@ and @> from the nodes.  It would be nice to do this even on empty arguments, but that would complicate our job in coming up with a fill-cell or argument cell, because
  // we would have to keep track of whether we passed an ATOPOPEN.  But then we could avoid executing the fill cell any time the is a BOXATOP, even down the stack.  As it is, the only time we
  // elide the execution is when BOXATOP occurs at the first node, i.e. for an each that is not boxed

  // RANKONLY verbs contain an invalid f1 pointer (it was used to get to a call to here).  We have to step over the RANKONLY to get to what we can execute
  while(FAV(fs)->flag2&VF2RANKONLY2){fs=FAV(fs)->fgh[0]; f2=FAV(fs)->valencefns[1];}

  while(1){  // loop collecting ATOPs
   I fstate=(FAV(fs)->flag2&(VF2BOXATOP2|VF2ATOPOPEN2A|VF2ATOPOPEN2W))>>(VF2BOXATOP2X-ZZFLAGBOXATOPX);  // extract <@ and @> status bits from f
   if(fstate&state||!fstate)break;  // If this f overlaps with old, or it's not a flag-only node, we have to stop
   if(fstate&ZZFLAGATOPOPEN2W){
    // @> &> &.>
    //  Advance to the f of f@>
    fs=FAV(fs)->fgh[0]; f2=FAV(fs)->valencefns[1];
   }else{
    // <@: <@ <& <&:
    // Because the outermost rank is 0, <@f by itself is OK; but later, as in (<@f)@>, it is not.  <@:f is.  So check for infinite rank
    if(state&ZZFLAGATOPOPEN2W && FAV(fs)->mr<RMAX)break;  // not first, and not infinite rank: ignore
    // Advance fs to the g of <@g
    fs=FAV(fs)->fgh[1+((FAV(fs)->flag2>>VF2ISCCAPX)&1)]; f2=FAV(fs)->valencefns[1];
   }
   state|=fstate;  // We accepted the new f, so take its flags
  }

  // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
  // The base pointer AK advances through the source argument.  But if an operand is empty (meaning that there are no output cells),
  // replace any empty operand with a cell of fills.  (Note that operands can have no atoms and yet the result can have nonempty cells,
  // if the cells are empty but the frame does not contain 0)
  //
  // Self-virtual blocks modify the shape of a block, but that code notifies
  // us through a flag bit.
  A *aav, *wav;
  // Normal case where there are cells.

  // if we are using the BOXATOP from f, we can also use the raze flags.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-(state&ZZFLAGBOXATOP)) & (I)jtinplace & (JTWILLBEOPENED+JTCOUNTITEMS);

  // Now that we have handled the structural requirements of ATOPOPEN, clear it if the argument is not boxed
  // Allocate a non-in-place virtual block unless this is ATOPOPEN and w is boxed, in which case we will just use the value of the A block
  if(!(state&ZZFLAGATOPOPEN2W)||!(wt&BOX)){
   fauxvirtual(virtw,virtwfaux,w,0,ACUC1); AN(virtw)=1; state&=~ZZFLAGATOPOPEN2W;
  }else{wav=AAV(w); virtw=*wav;}
  if(!(state&ZZFLAGATOPOPEN2A)||!(at&BOX)){
   fauxvirtual(virta,virtafaux,a,0,ACUC1); AN(virta)=1; state&=~ZZFLAGATOPOPEN2A;
  }else{aav=AAV(a); virta=*aav;}
  
  // loop over the matched part of the outer frame

#define ZZDECL
#include "result.h"
  ZZPARMSNOFS(ar,mn)
#define ZZINSTALLFRAME(optr) MCISHd(optr,as,ar)

  do{I i0=ict;
   do{
    RZ(z=CALL2IP(f2,virta,virtw,fs));

#define ZZBODY  // assemble results
#include "result.h"

    if(--i0==0)break;  // stop before we load the last+1 item
    if(!(state&ZZFLAGATOPOPEN2A)){AK(virta) += ak;}else{aav=(A*)((I)aav+ak); virta=*aav;}
    if(!(state&ZZFLAGATOPOPEN2W)){AK(virtw) += wk;}else{wav=(A*)((I)wav+wk); virtw=*wav; }
   }while(1);
   // we have stopped with the pointers pointing to the last item read.  Advance them both to the next atom
   if(--oct<=0)break;  // if no more cells, avoid fetching out of bounds
   if(!(state&ZZFLAGATOPOPEN2A)){AK(virta) += ak?ak:wr;}else{virta=*++aav;}
   if(!(state&ZZFLAGATOPOPEN2W)){AK(virtw) += wk?wk:wr;}else{virtw=*++wav;}
  }while(1);

#define ZZEXIT
#include "result.h"

 }else{UC d; I *zzs;
  // if there are no cells, execute on a cell of fills.
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then

  if(!(FAV(fs)->flag2&VF2BOXATOP2)){
   if(!AN(a)){RZ(virta=filler(a));}else{virta = virtual(a,0,0); AN(virta)=1;}  // if there are cells, use first atom; else fill atom
   if(!AN(w)){RZ(virtw=filler(w));}else{virtw = virtual(w,0,0); AN(virtw)=1;}
   d=jt->uflags.us.cx.cx_c.db; jt->uflags.us.cx.cx_c.db=0; z=CALL2(f2,virta,virtw,fs); jt->uflags.us.cx.cx_c.db=d;   // normal execution on fill-cell
   if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num[0]; RESETERR;}  // use 0 as result if error encountered
  }else{
   // If we are executing a BOXATOP on a single cell, we know the result is going to be an atomic box.  We don't bother executing the verb at all then.
   z=ace;  // cell 'returned' a:
  }
  GA(zz,AT(z),0L,ar+AR(z),0L); zzs=AS(zz); MCISH(zzs,as,ar); MCISH(zzs+ar,AS(z),AR(z));  // allocate result, copy frame and shape
 }

// result is now in zz
 EPILOG(zz);
}

// Call a function that has Integrated Rank Support
// The function may leave the rank set on exit; we clear it
/* f knows how to compute f"r                           */
// jt->ranks is rank of monad or leftrank<<16 + rightrank
// jt->ranks is ~0 if the call is not through IRS
  // every call to IRS resets jt->ranks at the end
/* frame agreement is verified before invoking f        */
/* frames either match, or one is empty                 */
/* (i.e. prefix agreement invokes general case)         */
// If the action verb handles inplacing, we pass that through

// irs1() and irs2() are simply calls to the IRS-savvy function f[12] with the specified rank

A jtirs1(J jt,A w,A fs,I m,AF f1){A z;I wr; 
 F1PREFIP; RZ(w);
// Get the rank of w; if the requested rank m is > wr, use ~0 because some verbs test for that as an expedient
// If m is negative, use wr+m but never < 0
 wr=AR(w); m=m>=wr?(RANKT)~0:m; wr+=m; wr=wr<0?0:wr; wr=m>=0?m:wr;   // requested rank, after negative resolution, or ~0
 jt->ranks=(RANK2T)wr;  // install rank for called routine
 z=CALL1IP(f1,w,fs);
 jt->ranks=(RANK2T)~0;  // reset rank to infinite
 RETF(z);
}

// IRS setup for dyads x op y.  This routine sets jt->rank and calls the verb, which loops if it needs to
// a is x, w is y
// fs is the f field of the verb (the verb to be applied repeatedly) - or 0 if none (if we are called internally)
//  if inplacing is enabled in jt, fs must be given
// l, r are nominal ranks of fs
// f2 is the verb that does the work (jtover, jtreshape, etc).  Normally it will loop using rank?ex if it needs to
// IRS verbs are those that look at jt->rank.  This is where we set up jt->rank.  Once
// we have it, we call the setup verb, which will go on to do its internal looping and (optionally) call
// the verb f2 to finish operation on a cell
A jtirs2(J jt,A a,A w,A fs,I l,I r,AF f2){A z;I ar,wr;
 F2PREFIP; RZ(a&&w);
 wr=AR(w); r=r>=wr?(RANKT)~0:r; wr+=r; wr=wr<0?0:wr; wr=r>=0?r:wr; r=AR(w)-wr;   // wr=requested rank, after negative resolution, or ~0; r=frame of w, possibly negative if no frame
 ar=AR(a); l=l>=ar?(RANKT)~0:l; ar+=l; ar=ar<0?0:ar; ar=l>=0?l:ar; l=AR(a)-ar;   // ar=requested rank, after negative resolution, or ~0; l=frame of a, possibly negative if no frame
 l=MIN(r,l); l=l<0?0:l;  // get length of frame
 ASSERTAGREE(AS(a),AS(w),l)  // verify agreement before we modify jt->ranks
 jt->ranks=(RANK2T)((ar<<RANKTX)+wr);  // install as parm to the function.  Set to ~0 if possible
 z=CALL2IP(f2,a,w,fs);   // save ranks, call setup verb, pop rank stack
   // Not all verbs (*f2)() use the fs argument.
 jt->ranks=(RANK2T)~0;  // reset rank to infinite
 RETF(z);
}


static DF1(cons1a){R FAV(self)->fgh[0];}
static DF2(cons2a){R FAV(self)->fgh[0];}

// Constant verbs do not inplace because we loop over cells.  We could speed this up if it were worthwhile.
static DF1(cons1){V*sv=FAV(self);
 RZ(w);
 I mr; efr(mr,AR(w),*AV(sv->fgh[2]));
 R rank1ex(w,self,mr,cons1a);
}
static DF2(cons2){V*sv=FAV(self);I*v=AV(sv->fgh[2]);
 RZ(a&&w);
 I lr2,rr2; efr(lr2,AR(a),v[1]); efr(rr2,AR(w),v[2]);
 R rank2ex(a,w,self,AR(a),AR(w),lr2,rr2,cons2a);
}

// Handle u"n y where u supports irs.  Since the verb may support inplacing even with rank (,"n for example), pass that through.
// If inplacing is allowed here, pass that on to irs.  It will see whether the action verb can support inplacing.
// THIS SUPPORTS INPLACING: NOTHING HERE MAY DEREFERENCE jt!!
static DF1(rank1i){DECLF;A h=sv->fgh[2];I*v=AV(h); R irs1(w,fs,*v,f1);}
static DF2(rank2i){DECLF;A h=sv->fgh[2];I*v=AV(h); R irs2(a,w,fs,v[1],v[2],f2);}

// u"n y when u does not support irs. We loop over cells, and as we do there is no reason to enable inplacing
// THIS SUPPORTS INPLACING: NOTHING HERE MAY DEREFERENCE jt!!
static DF1(rank1){DECLF;A h=sv->fgh[2];I m,*v=AV(h),wr;
 RZ(w);
 wr=AR(w); efr(m,wr,v[0]);
 // We know that the first call is RANKONLY, and we consume any other RANKONLYs in the chain until we get to something else.  The something else becomes the
 // fs/f1 to rank1ex.  Until we can handle multiple fill neighborhoods, we mustn't consume a verb of lower rank
 while(FAV(fs)->flag2&VF2RANKONLY1){
  h=FAV(fs)->fgh[2]; I hm=AV(h)[0]; efr(hm,m,hm); if(hm<m)break;  // if new rank smaller than old, abort
  m=hm; fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[0];
 }
 R m<wr?rank1ex(w,fs,m,f1):CALL1(f1,w,fs);
}
// Version for rank 0.  Call rank1ex0, pointing to the u"r
static DF1(jtrank10atom){ A fs=FAV(self)->fgh[0]; R (FAV(fs)->valencefns[0])(jt,w,fs);}  // will be used only for no-frame executions.  Otherwise will be replaced by the flags loop
static DF1(jtrank10){R jtrank1ex0(jt,w,self,jtrank10atom);}  // pass inplaceability through.


// For the dyads, rank2ex does a quadruply-nested loop over two rank-pairs, which are the n in u"n (stored in h) and the rank of u itself (fetched from u).
// THIS SUPPORTS INPLACING: NOTHING HERE MAY DEREFERENCE jt!!
static DF2(rank2){DECLF;A h=sv->fgh[2];I ar,l=AV(h)[1],r=AV(h)[2],wr;
 RZ(a&&w);
 ar=AR(a); efr(l,ar,l);
 wr=AR(w); efr(r,wr,r);
 if(((l-ar)|(r-wr))<0) {I llr=l, lrr=r;  // inner ranks, if any
 // We know that the first call is RANKONLY, and we consume any other RANKONLYs in the chain until we get to something else.  The something else becomes the
 // fs/f1 to rank1ex.  We have to stop if the new ranks will not fit in the two slots allotted to them.
 // This may lead to error until we support multiple fill neighborhoods
  while(FAV(fs)->flag2&VF2RANKONLY2){
   h=FAV(fs)->fgh[2]; I hlr=AV(h)[1]; I hrr=AV(h)[2]; efr(hlr,llr,hlr); efr(hrr,lrr,hrr);  // fetch ranks of new verb, resolve negative, clamp against old inner rank
   if((hlr^llr)|(hrr^lrr)){  // if there is a new rank to insert...
    if((l^llr)|(r^lrr))break;  // if lower slot full, exit, we can't add a new one
    llr=hlr; lrr=hrr;  // install new inner ranks, where they are new lows
   }
   // either we can ignore the new rank or we can consume it.  In either case pass on to the next one
   fs=FAV(fs)->fgh[0]; f2=FAV(fs)->valencefns[1];   // advance to the new function
  }
  R rank2ex(a,w,fs,llr,lrr,l,r,f2);
 }else R CALL2(f2,a,w,fs);  // pass in verb ranks to save a level of rank processing if not infinite.  Preserves inplacing
}
// Version for rank 0.  Call rank1ex0, pointing to the u"r
static DF2(jtrank20atom){ A fs=FAV(self)->fgh[0]; R (FAV(fs)->valencefns[1])(jt,a,w,fs);}  // will be used only for no-frame executions.  Otherwise will be replaced by the flags loop
static DF2(jtrank20){R jtrank2ex0(jt,a,w,self,jtrank20atom);}  // pass inplaceability through.


// a"w; result is a verb
F2(jtqq){A h,t;AF f1,f2;D*d;I *hv,n,r[3],vf,flag2=0,*v;
 RZ(a&&w);
 // The h value in the function will hold the ranks from w.  Allocate it
 GAT0(h,INT,3,1); hv=AV(h);  // hv->rank[0]
 if(VERB&AT(w)){
  // verb v.  Extract the ranks into a floating-point list
  GAT0(t,FL,3,1); d=DAV(t);
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
  ASSERT((UI)(n-1)<3,EVLENGTH);
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
  V* av=FAV(a);   // point to verb info
  // The flags for u indicate its IRS and atomic status.  If atomic (for monads only), ignore the rank, just point to
  // the action routine for the verb.  Otherwise, choose the appropriate rank routine, depending on whether the verb
  // supports IRS.  The IRS verbs may profitably support inplacing, so we enable it for them.
  vf=av->flag&(VASGSAFE|VJTFLGOK1|VJTFLGOK2);  // inherit ASGSAFE from u, and inplacing
  if(av->flag&VISATOMIC1){f1=av->valencefns[0];}else{if(av->flag&VIRS1){f1=rank1i;}else{f1=r[0]?rank1:jtrank10; flag2|=VF2RANKONLY1;}}
  if(av->flag&VIRS2){f2=rank2i;}else{f2=(r[1]|r[2])?rank2:jtrank20;flag2|=VF2RANKONLY2;}
  // Test for special cases
  if(av->valencefns[1]==jtfslashatg && r[1]==1 && r[2]==1){  // f@:g"1 1 where f and g are known atomic
   I isfork=av->id==CFORK;
   if(FAV(FAV(isfork?av->fgh[1]:av->fgh[0])->fgh[0])->id==CPLUS && FAV(isfork?av->fgh[2]:av->fgh[1])->id==CSTAR) {
    // +/@:*"1 1 or ([: +/ *)"1 1 .  Use special rank-1 routine.  It supports IRS, but not inplacing (fslashatg didn't inplace either)
    f2=jtsumattymes1; vf |= VIRS2; flag2 &= ~VF2RANKONLY2;  // switch to new routine, which supports IRS
   }
  }
 }

 // Create the derived verb.  The derived verb (u"n) NEVER supports IRS; it inplaces if the action verb u supports inplacing
 R fdef(flag2,CQQ,VERB, f1,f2, a,w,h, vf, r[0],r[1],r[2]);
}
