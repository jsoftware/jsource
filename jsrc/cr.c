/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Rank Associates                                           */

#include "j.h"

#define DR(r)           (((UI)(I)(r)>RMAX)?RMAX:r)   // return RMAX if input is negative or > RMAX

#define ZZDEFN
#include "result.h"

// make sure these don't overlap with bits in result.h
#define STATEWNOTEMPTYX 16  // set if a arg has atoms
#define STATEWNOTEMPTY ((I)1<<STATEWNOTEMPTYX)
#define STATEANOTEMPTYX 17  // set if a arg has atoms
#define STATEANOTEMPTY ((I)1<<STATEANOTEMPTYX)
#define STATEAWNOTEMPTYX 18  // set if a and w args both have atoms
#define STATEAWNOTEMPTY ((I)1<<STATEAWNOTEMPTYX)
// These bits start where they do to avoid overlap with AFPRISTINE.  Also, they are above where rank significance is
#define STATEOUTERREPEATAX (AFPRISTINEX+1)
#define STATEOUTERREPEATA (((I)1)<<STATEOUTERREPEATAX)
#define STATEINNERREPEATWX (AFPRISTINEX+2)
#define STATEINNERREPEATW (((I)1)<<STATEINNERREPEATWX)
#define STATEINNERREPEATAX (AFPRISTINEX+3)
#define STATEINNERREPEATA (((I)1)<<STATEINNERREPEATAX)
// There must be NO higher bits than STATEINNERREPEATA, because we shift down and OR into flags

// General setup for verbs that do not go through jtirs[12].  Some of these are marked as IRS verbs.  General
// verbs derived from u"n also come through here, via jtrank2.
// A verb u["n] using this function checks to see whether it has multiple cells; if so,
// it calls here, giving a callback; we split the arguments into cells and call the callback,
// which is often the same original function that called here.
// rr is the rank at which the verb will be applied: in u"n, the smaller of rank-of-u and n
// JT flags: all these routines use JTINPLACEW, JTINPLACEA, JTCOUNTITEMS, JTWILLBEOPENED.  Other flags are untouched
A jtrank1ex(J jt,AD * RESTRICT w,A fs,I rr,AF f1){F1PREFIP;PROLOG(0041);A z,virtw;
   I mn,wcn,wf,wk;
 ARGCHK1(w);
 wf=AR(w)-rr;
 if(unlikely(!wf)){R CALL1IP(f1,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.
 if(unlikely(ISSPARSE(AT(w))))R sprank1(w,fs,rr,f1);
#define ZZFLAGWORD state
 I state=ZZFLAGINITSTATE;  // init flags, including zz flags
 // RANKONLY verbs were handled in the caller to this routine, but fs might be RANKATOP.  In that case we could include its rank in the loop here,
 // if its rank is not less than the outer rank (we would simply ignore it), but we don't bother.  If its rank is smaller we can't ignore it because assembly might affect
 // the order of fill.  But if f is BOXATOP, there will be no fill, and we can safely use the smaller rank
 if(unlikely((FAV(fs)->flag2&VF2BOXATOP1)!=0)){
  I mr=FAV(fs)->mr; rr=rr<mr?rr:mr;
  state |= (FAV(fs)->flag2&VF2BOXATOP1)>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // If this is BOXATOP, set so for loop.  Don't touch fs yet, since we might not loop
  state &= ~((FAV(fs)->flag2&VF2ATOPOPEN1)>>(VF2ATOPOPEN1X-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
  // if we are using the BOXATOP from f, we can also use the raze flags.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-state) & (I)jtinplace & (JTWILLBEOPENED|JTCOUNTITEMS);
  wf=AR(w)-rr;  // refresh frame
 }
 // multiple cells.  Loop through them.
 // Get size of each argument cell in atoms.  If this overflows, there must be a 0 in the frame, & we will have
 // gone through the fill path (& caught the overflow)
 CPROD(AN(w),mn,wf,AS(w)); PROD(wcn,rr,AS(w)+wf);   // number of cells, number of atoms in a cell
 // ?cn=number of atoms in a cell, ?k=#bytes in a cell
 wk=wcn<<bplg(AT(w));

 A zz=0;  // place where we will build up the homogeneous result cells
 if(likely(mn!=0)){I i0;
  // Normal case where there are cells.
  // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
  // The base pointer AK advances through the source argument. 

  fauxblock(virtwfaux);
  // if the original block was direct inplaceable, make the virtual block inplaceable.  (We can't do this for indirect blocks because a virtual block is not marked recursive - rather it increments
  // the usecount of the entire backing block - and modifying the virtual contents would leave the usecounts invalid since the backing block is always recursive (having been ra'd).  Maybe could do this if it isn't?)
  // Don't pass WILLOPEN status - we use that at this level
  fauxvirtual(virtw,virtwfaux,w,rr,ACUC1) MCISH(AS(virtw),AS(w)+wf,rr); AN(virtw)=wcn;
  // Calculate inplaceability.  Since the inplaceability of f1 was passed into f"r, we don't need to look it up: we just pass the original
  // jtinplace through, except that we remove WILLOPEN status which we are picking up at this level
  jtinplace = (J)((I)jtinplace & ~(JTWILLBEOPENED+JTCOUNTITEMS));
  // Mark the virtual block as inplaceable only if w is fully inplaceable.  We have to turn off inplaceability in the virtual block so that
  // a non-inplaceable value might cause PRISTINE to be set.  We also require the type to be right, with some allowances for &.>
  state |= (UI)(SGNIF(jtinplace,JTINPLACEWX)&~((AT(w)&TYPEVIPOK)-(f1!=jteveryself))&AC(w))>>(BW-1-ZZFLAGVIRTWINPLACEX);   // requires JTINPLACEWX==0.  Single flag bit

#define ZZDECL
#include "result.h"
  ZZPARMS(wf,mn,1,1)
#define ZZINSTALLFRAME(optr) MCISHd(optr,AS(w),wf)
  for(i0=mn;i0;--i0){
   ACRESET(virtw,ACUC1 + SGNONLYIF(state,ZZFLAGVIRTWINPLACEX))   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block
   RZ(z=CALL1IP(f1,virtw,fs));

#define ZZBODY  // assemble results
#include "result.h"

   // advance input pointer for next cell.  We keep the same virtual block because it can't be incorporated into anything
   AK(virtw) += wk;
  }

#define ZZEXIT
#include "result.h"

 }else{I *zzs;
  // no cells - execute on a cell of fills
  RZ(virtw=jtfiller(jt,AT(w),rr,AS(w)+wf));  // The cell of fills
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  WITHDEBUGOFF(z=CALL1(f1,virtw,fs);)
  if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num(0); RESETERR;}  // use 0 as result if error encountered
  I zr=AR(z); GA00(zz,AT(z),0L,wf+zr); zzs=AS(zz); MCISH(zzs,AS(w),wf); MCISH(zzs+wf,AS(z),zr);
 }

// result is now in zz

 EPILOG(zz);
}

// Streamlined version when rank is 0.  In this version we look for ATOPOPEN (i. e. every)
// f1 is the function to use if there are no flags, OR if there is just 1 cell with no frame or a cell of fill
// "Supports IRS", which means nothing except that we ignore input rank
A jtrank1ex0(J jt,AD * RESTRICT w,A fs,AF f1){F1PREFIP;PROLOG(0041);A z,virtw;
   I wk;
 ARGCHK1(w);
 RESETRANK;  // in case we are called with IRS, clear it
 if(unlikely(!AR(w))){R CALL1IP(f1,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.  Make this as fast as possible.
 // Switch to sparse code if argument is sparse
 if(unlikely(ISSPARSE(AT(w))))R sprank1(w,fs,0,f1);
#define ZZFLAGWORD state
 // wr=rank, ws->shape
 // Each cell is an atom.  Get # atoms (=#result cells)
 // ?k=#bytes in a cell, ?s->shape
 I mn=AN(w); wk=bpnoun(AT(w));

 A zz=0;  // place where we will build up the homogeneous result cells

 // Look for the forms we handle specially: <@:f (not here where rank=0)  <@f  f@>   and their combinations  <@(f@>) f&.> (<@:f)@>  but not as combinations  (<@f)@> (unless f has rank _) <@:(f@>)   also using &
 I state=ZZFLAGINITSTATE;

 if(likely((AN(w))!=0)){  // if no cells, go handle fill before we advance over flags
  // Here there are cells to execute on.  Collect ATOP flags
  // RANKONLY verbs contain an invalid f1 pointer (it was used to get to a call to here).  We have to step over the RANKONLY to get to what we can execute
  NOUNROLL while(FAV(fs)->flag2&VF2RANKONLY1){fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[0];}

  while(1){  // loop collecting ATOPs
   I fstate=(FAV(fs)->flag2&(VF2BOXATOP1|VF2ATOPOPEN1))>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // extract <@ and @> status bits from f
   if((((fstate&state)-1)&-fstate)>=0)break;  // If this f overlaps with old, or it's not just a flag node, we have to stop
   if(fstate&ZZFLAGATOPOPEN1){
    // @> &> &.>
    //  Advance to the f of f@>
    fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[0];
   }else{
    // <@: <@ <& <&:
    // Because the outermost rank is 0, <@f by itself is OK; but later, as in (<@f)@>, it is not.  <@:f is.  So check for infinite rank
    if(state&ZZFLAGATOPOPEN1 && FAV(fs)->mr<RMAX)break;  // not first, and not infinite rank: ignore
    // Advance fs to the g of <@:g, and the function with it
    fs=FAV(fs)->fgh[1]; f1=FAV(fs)->valencefns[0];
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
  ZZPARMSNOFS(AR(w),mn)
  // if we are using the BOXATOP from f, we can also use the raze flags from the caller.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-(state&ZZFLAGBOXATOP)) & (I)jtinplace & (JTWILLBEOPENED+JTCOUNTITEMS);

  // Now that we have handled the structural requirements of ATOPOPEN, clear it if w is not open
  // Allocate a non-in-place virtual block unless this is ATOPOPEN and w is boxed, in which case we will just use the value of the A block
  fauxblock(virtwfaux);
  if(likely((SGNIF(state,ZZFLAGATOPOPEN1X)&SGNIF(AT(w),BOXX))>=0)){   // if not (ATOPOPEN and w boxed)...
   fauxvirtual(virtw,virtwfaux,w,0,ACUC1); AN(virtw)=1; state&=~ZZFLAGATOPOPEN1;
   // Mark the virtual block as inplaceable only if w is fully inplaceable.  We have to turn of inplaceability in the virtual block so that
   // a non-inplaceable value might cause PRISTINE to be set.  We also require the type to be right, with some allowances for &.>
   state |= (UI)(SGNIF(jtinplace,JTINPLACEWX)&~((AT(w)&TYPEVIPOK)-(f1!=jteveryself))&AC(w))>>(BW-1-ZZFLAGVIRTWINPLACEX);   // requires JTINPLACEWX==0.  Single flag bit
   // Init the inplaceability of virtw.  We do this here because in the loop we handle it only for low rank (i. e. virtwfaux) so as to avoid inplacing ATOPOPEN.
   // Thus, for higher rank we set it only this once.  It will stay right unless it gets virtualed
   ACRESET(virtw,ACUC1 + SGNONLYIF(state,ZZFLAGVIRTWINPLACEX))
  }else{wav=AAV(w); virtw=C(*wav++);
  }
  // Since the inplaceability of f1 was passed into f"r, we don't need to look it up: we just pass the original
  // jtinplace through, except that we remove WILLOPEN status which we are picking up at this level
  jtinplace = (J)((I)jtinplace & ~(JTWILLBEOPENED+JTCOUNTITEMS));

#define ZZINSTALLFRAME(optr) MCISHd(optr,AS(w),AR(w))
  do{
   RZ(z=CALL1IP(f1,virtw,fs));
#define ZZBODY  // assemble results
#include "result.h"

   if(--mn==0)break;  // exit loop before last fetch to avoid fetching out of bounds
   AC((A)virtwfaux)=ACUC1 + SGNONLYIF(state,ZZFLAGVIRTWINPLACEX);   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block
     // for simplicity we apply this correction to the faux block, so as to ensure we don't do it when ATOPOPEN is active.  This means ranks higher than the max for
     // the faux virtual block miss out on the reinitialization.  That's not so bad
   // advance input pointer for next cell.  We keep the same virtual block because it can't be incorporated into anything
   if(likely(!(state&ZZFLAGATOPOPEN1))){AK(virtw) += wk;}else{virtw=C(*wav++);}
  }while(1);

#define ZZEXIT
#include "result.h"

 }else{I *zzs;
  // no cells - execute on a cell of fills
  RZ(virtw=jtfiller(jt,AT(w),0,0));  // The cell of fills
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  if(!(FAV(fs)->flag2&VF2BOXATOP1)){
   WITHDEBUGOFF(z=CALL1(f1,virtw,fs);)   // normal execution on fill-cell
   if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num(0); RESETERR;}  // use 0 as result if error encountered
  }else{
   // If we are executing a BOXATOP on a single cell, we know the result is going to be an atomic box.  We don't bother executing the verb at all then.
   // jmf.ijs unknowingly takes advantage of this fact, and would crash if executed on an empty cell
   z=ds(CACE);  // cell 'returned' a:
  }
  I zr=AR(z),wr=AR(w); GA00(zz,AT(z),0L,wr+zr); zzs=AS(zz); MCISH(zzs,AS(w),wr); MCISH(zzs+wr,AS(z),zr);
 }

// result is now in zz

 EPILOG(zz);
}

A jtrank2ex(J jt,AD * RESTRICT a,AD * RESTRICT w,A fs,UI lrrrlcrrcr,AF f2){
 I lrrr=(RANK2T)lrrrlcrrcr; I lcrrcr=lrrrlcrrcr>>RANK2TX;  // inner, outer ranks
 F2PREFIP;PROLOG(0042);A virta,virtw,z;I acn,ak,mn,wcn,wk;
 I outerframect, outerrptct, innerframect, innerrptct, aof, wof, sof, lof, sif, lif, *lis, *los;
 ARGCHK2(a,w);
 if(unlikely((UI)lrrr==(((UI)AR(a)<<RANKTX)+AR(w)))){R CALL2IP(f2,a,w,fs);}  // if there's only one cell and no frame, run on it, that's the result.
 if(unlikely(ISSPARSE(AT(a)|AT(w))))R sprank2(a,w,fs,(UI)lcrrcr>>RANKTX,lcrrcr&RANKTMSK,f2);  // this needs to be updated to handle multiple ranks

// lr,rr are the ranks of the underlying verb.  lcr,rcr are the cell-ranks given by u"lcr rcr.
// If " was not used, lcr,rcr=lr,rr usually
// The ranks of the arguments have already been applied, so that we know that lr<=lcr<=AR(a), & similarly for w
// When processing v"l r the shapes look like:
// a frame   x x O  | x x x
//                   <---l-->
// w frame   x x    | x x x I
//                   <---r-->
// the outer frame (from ?cr) is to the left of the |, inner frame (from ?r) to the right.
// the rank of v is not included; the frames shown above pick up after that.  There are two
// possible repetitions required: if there is mismatched frame BELOW the rank (l r), as shown by letter I above,
// the individual cells of the shorter-frame argument must be repeated.  innerrptct gives the
// number of times the cell should be repeated.  If there is mismatched frame ABOVE the rank (l r), as
// shown by letter O above, the rank-(l/r) cell of the short-frame argument must be repeated.  outerrptct
// tells how many times the cell should be repeated; outerrestartpt gives the address of the rank-(l/r) cell
// being repeated; outercellct gives the number of (below lr) cells that are processed before an outer repetition.
// The two repeats can be for either argument independently, depending on which frame is shorter.

 // multiple cells.  Loop through them.

 // ?r=rank, ?s->shape, ?cr=effective rank, ?f=#total frame (inner+outer), for each argument
 // if inner rank is > outer rank, set it equal to outer rank
#define ZZFLAGWORD state
 I state=ZZFLAGINITSTATE;  // init flags, including zz flags

 // fetch empty-argument flags
 state|=(SGNTO0(-AN(a))<<STATEANOTEMPTYX)|(SGNTO0(-AN(w))<<STATEWNOTEMPTYX)|((SGNTO0(-AN(a))&SGNTO0(-AN(w)))<<STATEAWNOTEMPTYX);
 // RANKONLY verbs were handled in the caller to this routine, but fs might be RANKATOP.  In that case we can include its rank in the loop here, which will save loop setups
 if(unlikely((UI)(FAV(fs)->flag2&(VF2RANKATOP2|VF2BOXATOP2))>(UI)REPSGN(lrrr-lcrrcr))){  // if there are new ranks to include, provided ?r=?cr
 I lr=(UI)lrrr>>RANKTX, rr=lrrr&RANKTMSK;
 I t=(I)lr(fs); lr=t<lr?t:lr; t=(I)rr(fs); rr=t<rr?t:rr;   // get the ranks if we accept the new cell
  state |= (FAV(fs)->flag2&VF2BOXATOP2)>>(VF2BOXATOP2X-ZZFLAGBOXATOPX);  // If this is BOXATOP, set so for loop.  Don't touch fs yet, since we might not loop
  state &= ~((FAV(fs)->flag2&VF2ATOPOPEN2W)>>(VF2ATOPOPEN2WX-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
  // if we are using the BOXATOP from f, we can also use the raze flags.  Set these only if BOXATOP to prevent us from incorrectly
  // marking the result block as having uniform items if we didn't go through the assembly loop here
  state |= (-state) & (I)jtinplace & (JTWILLBEOPENED|JTCOUNTITEMS);
  lrrr=(lr<<RANKTX)+rr;  // reconstitute the combined llrr
 }
 UI afwf=((UI)AR(a)<<RANKTX)+AR(w)-lrrr;   // frames

 // Get the length of the outer frames, which are needed only if either "-rank is greater than the verb rank,
 // either argument has frame with respect to the "-ranks, and those frames are not the same length
 I aofwof=((UI)AR(a)<<RANKTX)+AR(w)-lcrrcr;  // outer frames
 if(likely((I)((lrrr-lcrrcr)&(-((aofwof>>RANKTX)^(aofwof&RANKTMSK))))>=0)){los=0; lof=aof=wof=0; outerframect=outerrptct=1;  // no outer frame unless it's needed
 }else{
  // outerframect is the number of cells in the shorter frame; outerrptct is the number of cells in the residual frame
  // find smaller/larger frame/shape, and indicate if a is the repeated arg (otherwise we assume w)
  wof=aofwof&RANKTMSK; aof=aofwof>>RANKTX; lof=MAX(aof,wof); sof=MIN(aof,wof); los=AS(a); los=aof-wof<0?AS(w):los; state|=(aof-wof)&STATEOUTERREPEATA;
  ASSERTAGREE(AS(a),AS(w),sof)  // prefixes must agree
  CPROD(state&STATEAWNOTEMPTY,outerframect,sof,los); CPROD(state&STATEAWNOTEMPTY,outerrptct,lof-sof,los+sof);  // get # cells in frame, and in unmatched frame
 }

 // Now work on inner frames.  Compare frame lengths after discarding outer frames
 // set lif=length of longer inner frame, sif=length of shorter inner frame, lis->longer inner shape
 {
  I ea=(afwf>>RANKTX)-aof, ew=(afwf&RANKTMSK)-wof; sif=MIN(ea,ew); lif=MAX(ea,ew);  // lif=long inner frame, sif=short
  state|=(ea-ew)&STATEINNERREPEATA;  // if w is longer, indicate repeating a.  The flag bit is far above the max rank
  lis=AS(w)+wof; I *sis=AS(a)+aof;  // start out as the two inner shapes lis=w, sis=a
  ASSERTAGREE(sis,lis,sif)  // error if frames are not same as prefix
  lis=ew-ea<=0?sis:lis; state|=(ew-ea)&STATEINNERREPEATW;  // make lis the shape pointer for longer; if a is longer, indicate repeating w
 }
 CPROD(state&STATEAWNOTEMPTY,innerrptct,lif-sif,lis+sif);  // number of repetitions per matched-frame cell
 CPROD(state&STATEAWNOTEMPTY,innerframect,sif,lis);   // number of cells in matched frame

 // Migrate loops with count=1 toward the inner to reduce overhead.  We choose not to promote the outer to the inner if both
 // innerframect & innerrptct are 1, on grounds of rarity
 if(likely(innerrptct==1)){innerrptct=innerframect; innerframect=1; state &=~(STATEINNERREPEATW|STATEINNERREPEATA);}  // if only one loop needed, make it the inner, with no repeats

 // Get size of each argument cell in atoms.  If this overflows, there must be a 0 in the frame, & we will have
 // gone through the fill path (& caught the overflow)
 PROD(acn,(UI)lrrr>>RANKTX,AS(a)+(afwf>>RANKTX)); PROD(wcn,lrrr&RANKTMSK,AS(w)+(afwf&RANKTMSK));
 // ?cn=number of atoms in a cell, ?k=#bytes in a cell
 ak=acn<<bplg(AT(a));    // reshape below will catch any overflow
 wk=wcn<<bplg(AT(w));

 // See how many cells are going to be in the result
 DPMULDE(outerframect,outerrptct,mn)  DPMULDE(innerframect,mn,mn)  DPMULDE(innerrptct,mn,mn) 


 // allocate the virtual blocks that we will use for the arguments, and fill in the shape of a cell of each
 // The base pointer AK advances through the source argument.  But if an operand is empty (meaning that there are no output cells),
 // replace any empty operand with a cell of fills.  (Note that operands can have no atoms and yet the result can have nonempty cells,
 // if the cells are empty but the frame does not contain 0)
 //
 // If an arg is pristine, any cell of it must ipso facto be pristine, as long as it in inplaceable (& therefore not repeated here)
 //
 // See which arguments we can inplace.  The key is that they have to be not repeated.  This means outerrptct=1, and the specified argument not repeated in the inner loop.  Also,
 // a and w mustn't be the same block (one cannot be a virtual of the other unless the backer's usecount disables inplacing)
 fauxblock(virtwfaux); fauxblock(virtafaux); 
 if(likely((mn|(state&STATEANOTEMPTY))!=0)){
  // OK to inplace an arg if it's not the same as the other, not repeated, correct type (unless &.>), inplaceable usecount
  state |= (UI)(SGNIF((a!=w)&(outerrptct==1),0)&SGNIF(jtinplace,JTINPLACEAX)&AC(a)&~(((AT(a)&TYPEVIPOK)-(f2!=jtevery2self))|SGNIF(state,STATEINNERREPEATAX)))>>(BW-1-ZZFLAGVIRTAINPLACEX);   // requires JTINPLACEWX==0.  Single flag bit  sign=0 if (VIPOK or &.>) 
  fauxvirtual(virta,virtafaux,a,(UI)lrrr>>RANKTX,ACUC1) MCISH(AS(virta),AS(a)+(afwf>>RANKTX),(UI)lrrr>>RANKTX); AN(virta)=acn;
  // Init the inplaceability of virtw.  We do this here because in the loop we handle it only for low rank (i. e. virt[aw]faux) so as to avoid inplacing fill.
  // Thus, for higher rank we set it only this once.  It will stay right unless it gets virtualed
  ACRESET(virta,ACUC1 + SGNONLYIF(state,ZZFLAGVIRTAINPLACEX))
 }else{RZ(virta=jtfiller(jt,AT(a),(UI)lrrr>>RANKTX,AS(a)+(afwf>>RANKTX)));}

 if(likely((mn|(state&STATEWNOTEMPTY))!=0)){  // repeat for w
  state |= (UI)(SGNIF((a!=w)&(outerrptct==1),0)&SGNIF(jtinplace,JTINPLACEWX)&AC(w)&~(((AT(w)&TYPEVIPOK)-(f2!=jtevery2self))|SGNIF(state,STATEINNERREPEATWX)))>>(BW-1-ZZFLAGVIRTWINPLACEX);   // requires JTINPLACEWX==0.  Single flag bit  sign=0 if (VIPOK or &.>) 
  fauxvirtual(virtw,virtwfaux,w,lrrr&RANKTMSK,ACUC1) MCISH(AS(virtw),AS(w)+(afwf&RANKTMSK),lrrr&RANKTMSK); AN(virtw)=wcn;
  ACRESET(virtw,ACUC1 + SGNONLYIF(state,ZZFLAGVIRTWINPLACEX))
 }else{RZ(virtw=jtfiller(jt,AT(w),lrrr&RANKTMSK,AS(w)+(afwf&RANKTMSK)));}
 // Allow inplacing if the verb supports it, but with the raze flags removed.  We can be loose here because we must be strict about the virt inplaceability to get pristinity right.
 jtinplace = (J)(intptr_t)((I)jtinplace & (~(JTWILLBEOPENED+JTCOUNTITEMS)));

 A zz=0;  // place where we will build up the homogeneous result cells
 if(likely(mn!=0)){I i0, i1, i2, i3;  // likely on single word fails
  // Normal case where there are cells.
  // loop over the matched part of the outer frame

#define ZZDECL
#include "result.h"
  ZZPARMS(lof+lif,mn,2,2)
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
      AC((A)virtafaux)=ACUC1 + SGNONLYIF(state,ZZFLAGVIRTAINPLACEX);   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block - only if faux (thus not filler)
      AC((A)virtwfaux)=ACUC1 + SGNONLYIF(state,ZZFLAGVIRTWINPLACEX); 
      // invoke the function, get the result for one cell
      RZ(z=CALL2IP(f2,virta,virtw,fs));
#if AUDITEXECRESULTS
      auditblock(jt,z,1,1);
#endif

#define ZZBODY  // assemble results
#include "result.h"

      // advance input pointers for next cell.  We keep the same virtual blocks because they can't be incorporated into anything
      if(likely(!(state&STATEINNERREPEATA))){  // predictable
       AK(virta) += ak;
      }

      if(likely(!(state&STATEINNERREPEATW))){  // predictable
       AK(virtw) += wk;
      }
     }
      // advance input pointers for next cell.  We increment any block that was being held constant in the inner loop.  There can be only one such.  Such an arg is never inplaced
     if(unlikely((state&STATEINNERREPEATA)!=0))AK(virta) += ak;
     else if(unlikely((state&STATEINNERREPEATW)!=0))AK(virtw) += wk;
    }
   }
  }

#define ZZEXIT
#include "result.h"

 }else{I *zzs;
  // if there are no cells, execute on a cell of fills.
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  WITHDEBUGOFF(z=CALL2(f2,virta,virtw,fs);)
#if AUDITEXECRESULTS
  auditblock(jt,z,1,1);
#endif
  if(unlikely(jt->jerr!=0)){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num(0); RESETERR;}  // use 0 as result if error encountered
  I zr=AR(z); GA00(zz,AT(z),0L,lof+lif+zr); zzs=AS(zz);
  MCISH(zzs,los,lof); MCISH(zzs+lof,lis,lif); MCISH(zzs+lof+lif,AS(z),zr);
 }

// result is now in zz
 EPILOG(zz);
}

// version for rank 0.  We look at ATOPOPEN too.  f2 is the function to use if there is no frame
// This code does not set inplaceability on nonrepeated cells - hardly useful at rank 0
A jtrank2ex0(J jt,AD * RESTRICT a,AD * RESTRICT w,A fs,AF f2){F2PREFIP;PROLOG(0042);A virta,virtw,z;
   I ak,ar,*as,ict,oct,mn,wk,wr,*ws;
 ARGCHK2(a,w); ar=AR(a); wr=AR(w); if(unlikely(!(ar+wr)))R CALL2IP(f2,a,w,fs);   // if no frame, make just 1 call
 if(unlikely(ISSPARSE(AT(a)|AT(w))))R sprank2(a,w,fs,0,0,f2);  // this needs to be updated to handle multiple ranks

#define ZZFLAGWORD state

 // Verify agreement
 as=AS(a); ws=AS(w); ASSERTAGREE(as,ws,MIN(ar,wr))

 // Calculate strides for inner and outer loop.  Cell-size is known to be 1 atom.  The stride of the inner loop is 1 atom, except for a
 // repeated value, of which there can be at most 1.  For a repeated value, we set the stride to 0 and remember the repetition count and stride
 ak=bpnoun(AT(a)); wk=bpnoun(AT(w));  // stride for 1 atom
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
 I state=ZZFLAGINITSTATE;

 A zz=0;  // place where we will build up the homogeneous result cells
 fauxblock(virtafaux);  fauxblock(virtwfaux);

 if(likely(mn!=0)){   // likely fails on single word
  // Collect flags <@ and @> from the nodes.  It would be nice to do this even on empty arguments, but that would complicate our job in coming up with a fill-cell or argument cell, because
  // we would have to keep track of whether we passed an ATOPOPEN.  But then we could avoid executing the fill cell any time the is a BOXATOP, even down the stack.  As it is, the only time we
  // elide the execution is when BOXATOP occurs at the first node, i.e. for an each that is not boxed

  // RANKONLY verbs contain an invalid f1 pointer (it was used to get to a call to here).  We have to step over the RANKONLY to get to what we can execute
  NOUNROLL while(FAV(fs)->flag2&VF2RANKONLY2){fs=FAV(fs)->fgh[0]; f2=FAV(fs)->valencefns[1];}

  while(1){  // loop collecting ATOPs
  I fstate=(FAV(fs)->flag2&(VF2BOXATOP2|VF2ATOPOPEN2A|VF2ATOPOPEN2W))>>(VF2BOXATOP2X-ZZFLAGBOXATOPX);  // extract <@ and @> status bits from f
   if((((fstate&state)-1)&-fstate)>=0)break;  // If this f overlaps with old, or it's not just a flag node, we have to stop
   if(fstate&ZZFLAGATOPOPEN2W){
    // @> &> &.>
    //  Advance to the f of f@>
    fs=FAV(fs)->fgh[0]; f2=FAV(fs)->valencefns[1];
   }else{
    // <@: <@ <& <&:
    // Because the outermost rank is 0, <@f by itself is OK; but later, as in (<@f)@>, it is not.  <@:f is.  So check for infinite rank
    if(state&ZZFLAGATOPOPEN2W && FAV(fs)->mr<RMAX)break;  // not first, and not infinite rank: ignore
    // Advance fs to the g of <@:g, and the function with it
    fs=FAV(fs)->fgh[1]; f2=FAV(fs)->valencefns[1];
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
  if(likely((SGNIF(state,ZZFLAGATOPOPEN2WX)&SGNIF(AT(w),BOXX))>=0)){
   fauxvirtual(virtw,virtwfaux,w,0,ACUC1); AN(virtw)=1; state&=~ZZFLAGATOPOPEN2W;
  }else{wav=AAV(w); virtw=C(*wav);}
  if(likely((SGNIF(state,ZZFLAGATOPOPEN2AX)&SGNIF(AT(a),BOXX))>=0)){
   fauxvirtual(virta,virtafaux,a,0,ACUC1); AN(virta)=1; state&=~ZZFLAGATOPOPEN2A;
  }else{aav=AAV(a); virta=C(*aav);}
  
  // loop over the matched part of the outer frame

#define ZZDECL
#include "result.h"
  ZZPARMSNOFS(ar,mn)
#define ZZINSTALLFRAME(optr) MCISHd(optr,as,ar)

  do{I i0=ict;
   do{
    RZ(z=CALL2(f2,virta,virtw,fs));

#define ZZBODY  // assemble results
#include "result.h"

    if(--i0==0)break;  // stop before we load the last+1 item
    if(likely(!(state&ZZFLAGATOPOPEN2A))){AK(virta) += ak;}else{aav=(A*)((I)aav+ak); virta=C(*aav);}
    if(likely(!(state&ZZFLAGATOPOPEN2W))){AK(virtw) += wk;}else{wav=(A*)((I)wav+wk); virtw=C(*wav); }
   }while(1);
   // we have stopped with the pointers pointing to the last item read.  Advance them both to the next atom
   if(--oct<=0)break;  // if no more cells, avoid fetching out of bounds
   if(likely(!(state&ZZFLAGATOPOPEN2A))){AK(virta) += ak?ak:wr;}else{++aav; virta=C(*aav);}
   if(likely(!(state&ZZFLAGATOPOPEN2W))){AK(virtw) += wk?wk:wr;}else{++wav; virtw=C(*wav);}
  }while(1);

#define ZZEXIT
#include "result.h"

 }else{I *zzs;
  // if there are no cells, execute on a cell of fills.
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then

  if(!(FAV(fs)->flag2&VF2BOXATOP2)){
   if(!AN(a)){RZ(virta=jtfiller(jt,AT(a),0,0));}else{virta = virtual(a,0,0); AN(virta)=1;}  // if there are cells, use first atom; else fill atom
   if(!AN(w)){RZ(virtw=jtfiller(jt,AT(w),0,0));}else{virtw = virtual(w,0,0); AN(virtw)=1;}
   WITHDEBUGOFF(z=CALL2(f2,virta,virtw,fs);)   // normal execution on fill-cell
   if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(z); z=num(0); RESETERR;}  // use 0 as result if error encountered
  }else{
   // If we are executing a BOXATOP on a single cell, we know the result is going to be an atomic box.  We don't bother executing the verb at all then.
   z=ds(CACE);  // cell 'returned' a:
  }
  I zr=AR(z); GA00(zz,AT(z),0L,ar+zr); zzs=AS(zz); MCISH(zzs,as,ar); MCISH(zzs+ar,AS(z),zr);  // allocate result, copy frame and shape
 }

// result is now in zz
 EPILOG(zz);
}

// This code has been superseded by rank[12](i|in) and is called only by sparse routines
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

// irs1() and irs2() are simply calls to the IRS-savvy function f[12] with the specified rank, faster than creating a verb with rank

A jtirs1(J jt,A w,A fs,I m,AF f1){A z;I wr; 
 F1PREFIP; ARGCHK1(w);
// Get the rank of w; if the requested rank m is > wr, use ~0 because some verbs test for that as an expedient
// If m is negative, use wr+m but never < 0
 wr=AR(w); m=m>=wr?R2MAX:m; wr+=m; wr=wr<0?0:wr; wr=m>=0?m:wr;   // requested rank, after negative resolution, or ~0
 jt->ranks=(RANK2T)wr;  // install rank for called routine
 z=CALL1IP(f1,w,fs);
 jt->ranks=R2MAX;  // reset rank to infinite
 RETF(z);
}

// IRS setup for dyads x op y.  This routine sets jt->ranks and calls the verb, which loops if it needs to
// a is x, w is y
// fs is the f field of the verb (the verb to be applied repeatedly) - or 0 if none (if we are called internally)
//  if inplacing is enabled in jt, fs must be given
// l, r are nominal ranks of fs
// f2 is the verb that does the work (jtover, jtreshape, etc).  Normally it will loop using rank?ex if it needs to
// IRS verbs are those that look at jt->rank.  This is where we set up jt->rank.  Once
// we have it, we call the setup verb, which will go on to do its internal looping and (optionally) call
// the verb f2 to finish operation on a cell
A jtirs2(J jt,A a,A w,A fs,I l,I r,AF f2){A z;I ar,wr;
 F2PREFIP; ARGCHK2(a,w);
 wr=AR(w); r=r>=wr?RMAX:r; wr+=r; wr=wr<0?0:wr; wr=r>=0?r:wr; r=AR(w)-wr;   // wr=requested rank, after negative resolution, or ~0; r=frame of w, possibly negative if no frame
 ar=AR(a); l=l>=ar?RMAX:l; ar+=l; ar=ar<0?0:ar; ar=l>=0?l:ar; l=AR(a)-ar;   // ar=requested rank, after negative resolution, or ~0; l=frame of a, possibly negative if no frame
 ASSERTAGREE(AS(a),AS(w),MAX(0,MIN(r,l)))  // verify agreement before we modify jt->ranks
 jt->ranks=(RANK2T)((ar<<RANKTX)+wr);  // install as parm to the function.  Set to ~0 if possible
 z=CALL2IP(f2,a,w,fs);   // save ranks, call setup verb, pop rank stack
   // Not all verbs (*f2)() use the fs argument.
 jt->ranks=R2MAX;  // reset rank to infinite
 RETF(z);
}


static DF1(cons1a){R FAV(self)->fgh[0];}
static DF2(cons2a){R FAV(self)->fgh[0];}

// Constant verbs do not inplace because we loop over cells.  We could speed this up if it were worthwhile.
static DF1(cons1){
 ARGCHK1(w);
 I mr; efr(mr,AR(w),(I)FAV(self)->localuse.lu1.srank[0]);
 R rank1ex(w,self,mr,cons1a);
}
static DF2(cons2){
 ARGCHK2(a,w);
 I lr2,rr2; efr(lr2,AR(a),(I)FAV(self)->localuse.lu1.srank[1]); efr(rr2,AR(w),(I)FAV(self)->localuse.lu1.srank[2]);
 R rank2ex(a,w,self,lr2,rr2,lr2,rr2,cons2a);
}

// cyclic-gerund verbs for m"n create an iterator from the gerund and pass that into rank processing, looping over cells
static DF1(cycr1){V*sv=FAV(self); I cger[128/SZI];
 ARGCHK1(w);
 RZ(self=createcycliciterator((A)&cger, self));  // fill in an iterator for this gerund
 I mr; efr(mr,AR(w),(I)sv->localuse.lu1.srank[0]);
 R rank1ex(w,self,mr,FAV(self)->valencefns[0]);  // callback is to the cyclic-execution function
}
static DF2(cycr2){V*sv=FAV(self); I cger[128/SZI];
 ARGCHK2(a,w);
 RZ(self=createcycliciterator((A)&cger, self));  // fill in an iterator for this gerund
 I lr2,rr2; efr(lr2,AR(a),(I)sv->localuse.lu1.srank[1]); efr(rr2,AR(w),(I)sv->localuse.lu1.srank[2]);
 R rank2ex(a,w,self,lr2,rr2,lr2,rr2,FAV(self)->valencefns[1]);  // callback is to the cyclic-execution function
}




// Handle u"n y where u supports irs.  Since the verb may support inplacing even with rank (,"n for example), pass that through.
static DF1(rank1i){A fs=FAV(self)->fgh[0]; AF f1=FAV(fs)->valencefns[0];F1PREFIP;ARGCHK1(w);  // this version when requested rank is positive
 I m=FAV(self)->localuse.lu1.srank[0]; I r=AR(w); m+=REPSGN(m)&r; m=m<0?0:m; m=m>=r?RMAX:m; jt->ranks=(RANK2T)(m);  // install rank for called routine
 A z=CALL1IP(f1,w,fs);
 jt->ranks=R2MAX;  // reset rank to infinite
 RETF(z);
}

// dyadic forms also check agreement wrt the given ranks
static DF2(rank2i){A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1]; F2PREFIP;ARGCHK1(w);
 I ar=FAV(self)->localuse.lu1.srank[1]; I r=AR(a); ar+=REPSGN(ar)&r; ar=ar<0?0:ar; ar=ar>=r?RMAX:ar; I af=r-ar;   // left rank and frame
 I wr=FAV(self)->localuse.lu1.srank[2];   r=AR(w); wr+=REPSGN(wr)&r; wr=wr<0?0:wr; wr=wr>=r?RMAX:wr; I wf=r-wr;   // right rank and frame
 ASSERTAGREE(AS(a),AS(w),MAX(0,MIN(wf,af)));  // verify agreement before we modify jt->ranks
 jt->ranks=(RANK2T)((ar<<RANKTX)+wr);  // install as parm to the function.  Set to ~0 if possible
 A z=CALL2IP(f2,a,w,fs);   // save ranks, call setup verb, pop rank stack
 jt->ranks=R2MAX;  // reset rank to infinite
 RETF(z);
}

// u"n y when u does not support irs. We loop over cells, and as we do there is no reason to enable inplacing
// This routine supports jtflags by not touching jt - pass it through
static DF1(rank1){A fs=FAV(self)->fgh[0]; AF f1=FAV(fs)->valencefns[0]; I m,wr;
 ARGCHK1(w);
 wr=AR(w); efr(m,wr,(I)FAV(self)->localuse.lu1.srank[0]);
 // We know that the first call is RANKONLY, and we consume any other RANKONLYs in the chain until we get to something else.  The something else becomes the
 // fs/f1 to rank1ex.  Until we can handle multiple fill neighborhoods, we mustn't consume a verb of lower rank  scaf should consume anyway, let user control?
 if(likely(!FAV(self)->localuse.lu1.srank[3])){  // unless the user has said this rank must be separate...
  NOUNROLL while(FAV(fs)->flag2&VF2RANKONLY1){
   I hm=FAV(fs)->localuse.lu1.srank[0]; efr(hm,m,hm); if(hm<m)break;  // if new rank smaller than old, abort
   m=hm; fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[0];
  }
 }
 R m<wr?rank1ex(w,fs,m,f1):CALL1(f1,w,fs);
}
#define GEMIN0(a,b,c) ((a-b)&(a-c)) // sign is 0 if a>=MIN(b,c): a>=b or a>=c
#define LEMIN0(a,b,c) ((b-a)|(c-a)) // sign is 0 if a<=MIN(b,c): a<=b and a<=c

// This routine supports jtflags by not touching jt - pass it through
static DF1(rank1q){  // fast version: nonneg rank, no check for multiple RANKONLY
 ARGCHK1(w);
 // rank is considered nugatory if the rank of u<=n or rank of arg <=n or if n=rank of u, i. e. if n>=MIN(ranku,rankarg)
 // This gives error in the case
 // (1&+@>)"1 ] 2 2 $ 1 2;3;4;0
 // If you run this at rank 0, the fill will be calculated onver the whole array, while if you interpose a rank-1 step
 // the last row will fill separately.  User can give a floating-point rank to mean 'force the rank regardless'
 I r=AR(w); A fs=FAV(self)->fgh[0]; I m=FAV(self)->localuse.lu1.srank[0];   // r=arg rank  fs->u  m=rank from n
 I um=FAV(fs)->mr;
 if(unlikely(GEMIN0(m,r,um)>=0))if(likely(!FAV(self)->localuse.lu1.srank[3]))RETF(CALL1(FAV(fs)->valencefns[0],w,fs))  // rank is nugatory - bypass it
 r=r>m?m:r;  // clamp rank at arg rank - MIN(n, rankarg)
 R rank1ex(w,fs,r,FAV(fs)->valencefns[0]);
}

// Version for rank 0.  Call rank1ex0, pointing to the u"r
static DF1(jtrank10atom){ A fs=FAV(self)->fgh[0]; R CALL1(FAV(fs)->valencefns[0],w,fs);}  // will be used only for no-frame executions.  Otherwise will be replaced by the flags loop
static DF1(jtrank10){R jtrank1ex0(jt,w,self,jtrank10atom);}  // pass inplaceability through.

// For the dyads, rank2ex does a quadruply-nested loop over two rank-pairs, which are the n in u"n (stored in h) and the rank of u itself (fetched from u).

// This routine supports jtflags by not touching jt - pass it through
static DF2(rank2){A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1]; I ar,l=FAV(self)->localuse.lu1.srank[1],r=FAV(self)->localuse.lu1.srank[2],wr;
 ARGCHK2(a,w);
 ar=AR(a); efr(l,ar,l);
 wr=AR(w); efr(r,wr,r);  // now l<=ar, r<=wr
 I ulr=FAV(fs)->lrr>>RANKTX, urr=FAV(fs)->lrr&RANKTMSK;  // left & right ranks of u
 if(unlikely((-((ulr^l)|(urr^r))&(LEMIN0(ar,l,ulr)|GEMIN0(r,wr,urr))&(LEMIN0(wr,r,urr)|GEMIN0(l,ar,ulr)))>=0))if(likely(!FAV(self)->localuse.lu1.srank[3]))RETF(CALL2(FAV(fs)->valencefns[1],a,w,fs))  // rank is nugatory - bypass it
 I llr=l, lrr=r;  // inner ranks, if any
 // We know that the current call is RANKONLY, and we consume any other RANKONLYs in the chain until we get to something else.  The something else becomes the
 // fs/f1 to rank1ex.  We have to stop if the new ranks will not fit in the two slots allotted to them.
 // This may lead to error until we support multiple fill neighborhoods - use floating-point n to suppress
 if(likely(!FAV(self)->localuse.lu1.srank[3])){  // unless the user has said this rank must be separate...
  NOUNROLL while(FAV(fs)->flag2&VF2RANKONLY2){
   I hlr=FAV(fs)->localuse.lu1.srank[1]; I hrr=FAV(fs)->localuse.lu1.srank[2]; efr(hlr,llr,hlr); efr(hrr,lrr,hrr);  // fetch ranks of new verb, resolve negative, clamp against old inner rank
   if((hlr^llr)|(hrr^lrr)){  // if there is a new rank to insert...
    if((l^llr)|(r^lrr))break;  // if lower slot full, exit, we can't add a new one
    llr=hlr; lrr=hrr;  // install new inner ranks, where they are new lows
   }
   // either we can ignore the new rank or we can consume it.  In either case pass on to the next one
   fs=FAV(fs)->fgh[0]; f2=FAV(fs)->valencefns[1];   // advance to the new function
  }
 }
 R rank2ex(a,w,fs,llr,lrr,l,r,f2);
}

// This version for use when the ranks are nonnegative and u is not RANKONLY
// This routine supports jtflags by not touching jt - pass it through
static DF2(rank2q){
 ARGCHK2(a,w);
 A fs=FAV(self)->fgh[0]; I ulr=FAV(fs)->lrr>>RANKTX, urr=FAV(fs)->lrr&RANKTMSK;  // u, left & right ranks of u
 I ar=AR(a), wr=AR(w), l=FAV(self)->localuse.lu1.srank[1], r=FAV(self)->localuse.lu1.srank[2];  // ranks of args, ranks from n
 // See if this use of rank is nugatory.  An arg has 1 cell if rank of arg<=MIN(n,rank of u); middle cells if n<MIN(rank of arg,rank of u) and rank of u not _; unchanged rank if n=rank of u.
 // Rank can be omitted if it is true for either arg that (arg has 1 cell and other arg does not have middle cells), or both args have unchanged rank
 //              0=unch rnk        0=ar<=MIN     0=n>=MIN (right)      0=wr<MIN       0=n>=MIN (left)
 ulr+=(UI)ulr>=RMAX; urr+=(UI)urr>=RMAX;  // rank _ can be hiding negative rank, for which we don't know the cells; force mismatch then, and below assume inner cells
 if(unlikely((-((ulr^l)|(urr^r))&(LEMIN0(ar,l,ulr)|GEMIN0(r,wr,urr)|SGNIF(ulr,LGRMAX))&(LEMIN0(wr,r,urr)|GEMIN0(l,ar,ulr)|SGNIF(urr,LGRMAX)))>=0))if(likely(!FAV(self)->localuse.lu1.srank[3]))RETF(CALL2(FAV(fs)->valencefns[1],a,w,fs))  // rank is nugatory - bypass it
 ar=ar>l?l:ar; wr=wr>r?r:wr;   // clamp ranks at argument rank
 RETF(rank2ex(a,w,fs,ar,wr,ar,wr,FAV(fs)->valencefns[1]))
}

// Version for rank 0.  Call rank2ex0, pointing to the u"r
static DF2(jtrank20atom){ A fs=FAV(self)->fgh[0]; R (FAV(fs)->valencefns[1])(jt,a,w,fs);}  // will be used only for no-frame executions.  Otherwise will be replaced by the flags loop
static DF2(jtrank20){R jtrank2ex0(jt,a,w,self,jtrank20atom);}  // pass inplaceability through.


// a"w; result is a verb
F2(jtqq){F2PREFIP;AF f1,f2;I hv[3],n,r[3],vf,flag2=0,*v;A ger=0;C lc=0;
 ARGCHK2(a,w);
 A z; fdefallo(z)
 // The localuse value in the function will hold the ranks from w.
 I isfloat=AT(w)&FL;  // before we change w, remember if the value given was float.  We pass this through in srank[3] to tell the processing routines not to combine the rank op with other
 if(unlikely(VERB&AT(w))){
  // verb v.  Extract the ranks into an integer list, which goes into the derived verb
  r[0]=hv[0]=mr(w);
  r[1]=hv[1]=lr(w);
  r[2]=hv[2]=rr(w);
 }else{A t;
  // Noun v. Extract and turn into 3 values, stored in h
  n=AN(w);
  ASSERT(1>=AR(w),EVRANK);
  ASSERT((UI)(n-1)<3,EVLENGTH);
  RZ(t=vib(w)); v=AV(t);
  hv[0]=v[2==n]; hv[0]=hv[0]>RMAX?RMAX:hv[0]; hv[0]=hv[0]<-RMAX?-RMAX:hv[0]; r[0]=DR(hv[0]);
  hv[1]=v[3==n]; hv[1]=hv[1]>RMAX?RMAX:hv[1]; hv[1]=hv[1]<-RMAX?-RMAX:hv[1]; r[1]=DR(hv[1]);
  hv[2]=v[n-1];  hv[2]=hv[2]>RMAX?RMAX:hv[2]; hv[2]=hv[2]<-RMAX?-RMAX:hv[2]; r[2]=DR(hv[2]);
 }
 // r is the actual verb ranks, i. e. _ if given ranks are negative.  h may be negative

 // Get the action routines and flags to use for the derived verb
 if(unlikely(NOUN&AT(a))){  // could be gerund"n or noun"n
  // gerund requires: some rank not RMAX; boxed m; rank of m=1; and then the gerund must be well formed
  if(((hv[0]^RMAX)|(hv[1]^RMAX)|(hv[2]^RMAX)) && !((AR(a)^1) | (AT(a)&(NOUN&~BOX))) && (ger=fxeachv(1LL,a))){
   f1=cycr1; f2=cycr2;  // process this with the cyclic-gerund routines
   vf=VFLAGNONE;   // the cyclic processor does not inplace or IRS and we don't bother figuring out whether it is ASGSAFE
  } else {
   RESETERR;  // the gerund check may have raised an error
   f1=cons1; f2=cons2;    // use the constant routines for nouns
   // Mark the noun as non-inplaceable.  If the derived verb is used in another sentence, it must first be
   // assigned to a name, which will protects values inside it.
   ACIPNO(a);
   vf=VASGSAFE;    // the noun can't mess up assignment, and does not support IRS or inplacing
  }
 }else{
  V* av=FAV(a);   // point to verb info
  // if the rank is superfluous (meaning it is exactly the same as the rank of the verb) ignore it, returning the original verb.  We have seen
  // enough beginner code with +"0 to make this worthwhile.  The display will leave out the "0, to emphasize the equivalence.  We do this only
  // for noun w, to allow use of +"+ to avoid special code.  BUT: we can't trust a rank in a of _, which might be hiding an active negative rank; so we
  // keep the rank if only one of the dyad ranks in a is _
  I alr=av->lrr>>RANKTX, arr=av->lrr&RANKTMSK; 
  if(unlikely((av->mr==hv[0])&&(alr==hv[1])&&(arr==hv[2])&&((alr!=RMAX&&arr!=RMAX)||alr==arr)&&!(VERB&AT(w))))R a;  // first test is usually enough
  // The flags for u indicate its IRS and atomic status.  If atomic (for monads only), ignore the rank, just point to
  // the action routine for the verb.  Otherwise, choose the appropriate rank routine, depending on whether the verb
  // supports IRS.  The IRS verbs may profitably support inplacing, so we enable it for them.
  vf=av->flag&(VASGSAFE|VJTFLGOK1|VJTFLGOK2);  // inherit ASGSAFE from u, and inplacing
  // For monads: atomic verbs ignore rank, but they require the localuse field, so we can't just point the rank verb at them; we use a passthrough routine instead.  Otherwise, if the verb supports
  // IRS, go to the appropriate routine depending on the sign of rank; otherwise we will be doing an explicit rank loop: distinguish
  // rank-0, quick rank (rank is positive and a is NOT a rankonly type that may need to be combined), and all-purpose cases
  if(av->flag&VISATOMIC1){f1=jtrank10atom;}else{if(av->flag&VIRS1){f1=rank1i;}else{f1=hv[0]?(hv[0]>=0&&!(av->flag2&VF2RANKONLY1)?rank1q:rank1):jtrank10; flag2|=VF2RANKONLY1;}}
  // if the monad rank in v is 0, we can surely ignore any higher rank, except in the rank of the compound.  We set IRS1 here so any later "n is fast
  vf|=(hv[0]==0)<<VIRS1X;
  // For dyad: atomic verbs take the rank from this block, so we take the action routine, and also the parameter it needs; these parameters mean that only
  // nonnegative rank can be accomodated; otherwise, use processor for IRS (there is one for nonnegative, one for negative rank); if not IRS, there are processors for:
  // rank 0; nonneg ranks where fs is NOT a rank operator; general case
  if(av->flag&VFUSEDOK2&&(hv[1]|hv[2])>=0){f2=av->valencefns[1]; lc=av->lu2.lc;}  // transfer the fn-address and fn-code from the atomic to the fused block
  else if(av->flag&VIRS2){f2=rank2i;}else{f2=(hv[1]|hv[2])?((hv[1]|hv[2])>=0&&!(av->flag2&VF2RANKONLY2)?rank2q:rank2):jtrank20;flag2|=VF2RANKONLY2;}
  // Test for special cases
  if(av->valencefns[1]==jtfslashatg && r[1]==1 && r[2]==1){  // f/@:g"1 1 where f and g are known atomic
   if(FAV(FAV(av->fgh[0])->fgh[0])->id==CPLUS && FAV(av->fgh[1])->id==CSTAR) {
    // +/@:*"1 1 or ([: +/ *)"1 1 .  Use special rank-1 routine.  It supports IRS, but not inplacing (fslashatg didn't inplace either)
    f2=jtsumattymes1; vf |= VIRS2; flag2 &= ~VF2RANKONLY2; vf &=~(VJTFLGOK2);  // switch to new routine, which supports IRS but not inplacing
   }
  }
 }

 // Create the derived verb.  The derived verb (u"n) inplaces if the action verb u supports inplacing; it supports IRS only for monadic rank 0
 fdeffillall(z,flag2,CQQ,VERB, f1,f2, a,w,ger, vf, r[0],r[1],r[2],FAV(z)->lu2.lc=lc,FAV(z)->localuse.lu1.srank[0]=(I4)hv[0]; FAV(z)->localuse.lu1.srank[1]=(I4)hv[1]; FAV(z)->localuse.lu1.srank[2]=(I4)hv[2]; FAV(z)->localuse.lu1.srank[3]=isfloat);
 R z;
}
