/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Under and Each                                            */

#include "j.h"
#include "ve.h"

#ifdef BOXEDSPARSE
extern UC fboxedsparse;
#endif

static A jteverysp(J jt,A w,A fs){A*wv,x,z,*zv;P*wp,*zp;
 ARGCHK1(w);
 AF f1=FAV(fs)->valencefns[0];
 ASSERT((AT(w)&SPARSE+BOX)==SPARSE+BOX,EVNONCE);
 RZ(z=ca(w));
 wp=PAV(w); x=SPA(wp,x); wv=AAV(x);
 zp=PAV(z); x=SPA(zp,x); zv=AAV(x);
 DQ(AN(x), RZ(*zv++=CALL1(f1,C(*wv++),fs)));
 R z;
}

#define EVERYI(exp)  {RZ(x=exp); INCORP(x); RZ(*zv++=x); ASSERT(!ISSPARSE(AT(x)),EVNONCE);}
     /* note: x can be non-noun */

// NOTE: internal calls to every/every2 use a skeletal fs created by the EVERYFS macro.  It fills in only
// AK, valencefns, and flag.  If these routines use other fields, EVERYFS will need to fill them in
DF1(jteveryself){R jtevery(jt,w,FAV(self)->fgh[0]);}   // replace u&.> with u and process
// u&.>, but w may be a gerund, which makes the result a list of functions masquerading as an aray of boxes
A jtevery(J jt, A w, A fs){A * RESTRICT wv,x,z,* RESTRICT zv;
 F1PREFIP;ARGCHK1(w);RESETRANK;  // we claim to support IRS1 but really there's nothing to do for it
 I wt=AT(w), wflag=AFLAG(w), wr=AR(w);
 if(unlikely(ISSPARSE(wt)))R everysp(w,fs);
 I natoms=AN(w);
 // Allocate result area
 GATV(z,BOX,natoms,wr,AS(w));
 if(!natoms)R z;  // exit if no result atoms
 AF f1=FAV(fs)->valencefns[0];   // pointer to function to call
 A virtw; I flags;  // flags are: ACINPLACE=pristine result; JTWILLBEOPENED=nonrecursive result; BOX=input was boxed; ACPERMANENT=input was recursive inplaceable pristine, contents can be inplaced AFPRISTINE=w must be marked non-PRIST
 // If the result will be immediately unboxed, we create a NONrecursive result and we can store virtual blocks in it.  This echoes what result.h does.
 flags=ACINPLACE|((I)jtinplace&JTWILLBEOPENED)|(wt&BOX);
 // Get input pointer
 fauxblockINT(virtblockw,0,0);  // virtual block of rank 0, 0 atoms
 if(likely((flags&BOX)!=0)){virtw=C(*(wv=AAV(w)));  // if input is boxed, point to first box
  if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&SGNIF(wflag,AFPRISTINEX),w))flags|=ACPERMANENT&-(wflag&RECURSIBLE);  // indicates inplaceability of boxed contents - only if recursive block
 }else{
  // if input is not boxed, use a faux-virtual block to point to the atoms.  Repurpose unneeded wv to hold length
  fauxvirtual(virtw,virtblockw,w,0,ACUC1); AN(virtw)=1; wv=(A*)bpnoun(wt);  // note if w has gerunds, it is always boxed & doesn't go through here
  // If not boxed, can't be pristine
 }
 AFLAGINIT(z,(~flags<<(BOXX-JTWILLBEOPENEDX))&BOX)  // if WILLBEOPENED is NOT set, make the result a recursive box
 zv=AAV(z);
 // Get jt flags to pass to next level - take them from  fs, so that we always inplace this verb, which will allow us to set pristinity better
 // We must remove raze flags since we are using them here
 jtinplace=(J)((I)jt+((FAV(fs)->flag>>(VJTFLGOK1X-JTINPLACEWX))&JTINPLACEW));
 // If the verb returns its input block, we will have to turn off pristinity of the arg.  Replace w by its backing block
 if(wflag&AFVIRTUAL)w=ABACK(w);
 // *** now w has been replaced by its backing block, if it was virtual
 while(1){
  // If the input was pristine inplaceable and not virtual (i. e. originally unboxed), flag the contents as inplaceable UNLESS they are PERMANENT
  // If the input is inplaceable, there is no more use for it after this verb.  If it was pristine, every block in it is DIRECT and was either permanent or inplaceable when it was added; so if it's
  // not PERMANENT it is OK to change the usecount to inplaceable.  We must remove inplaceability on the usecount after execution, in case the input block is recursive and the contents now show a count of 2 (one
  // of which is the tpop).  Then we create a block with usecount 8..2: That's OK, but it must not be fa'd unless it is ra'd first, so that the ra will wipe out the inplaceability; if it is zapped or fa'd without
  // ra (as when FORK frees an unused block), the count must be tested so that it is zapped only if it is 8..1.  We do need to keep the usecount accurate after the return, though.
  // In fact the usecount of a pristine contents may be anything.  Consider ]&.> ]&.> y.  The first execution produces a recursive pristine block containing usecount 2.  The second
  // execution raises the usecount to 3 but the block is still inplaceable and has no other owner except the tstack.
  I wcbefore=AC(virtw); // get (always non-inplaceable) usecount before the call
  if(((AC(virtw)-(flags&ACPERMANENT))&ACINPLACE)<0){  // AC(virtw) always has sign 0, but may be PERMANENT
   ACIPYESLOCAL(virtw);  // make the block inplaceable
   // If we are setting the usecount to inplaceable, that must be a change, because we do that only on contents of boxes.  If the block is inplaceable, the system requires that AM point to a tpop-stack entry
   // that will free the block, and code may simulate a free by clearing that entry.  We can't be sure that the original tpush entry is still valid, but we do know that our w block is recursive and inplaceable, so we can use
   // any pointer to the block - for example *wv - as a zappable location.  If the block weren't recursive this wouldn't work.  See every2 for more
   AZAPLOC(virtw)=wv;  // point to a zappable entry
  }
  if(unlikely((x=CALL1IP(f1,virtw,fs))==0)){ // run the user's verb
   if(likely(flags&BOX))if(likely((I)*wv!=0))ACIPNO(virtw); R0;  // error: restore noninplaceability before we exit
  }
  // If x is DIRECT inplaceable, it must be unique and we can inherit them into a pristine result.  Otherwise clear pristinity
  if(likely((AT(x)&DIRECT)>0)){
   flags&=SGNIFPRISTINABLE(AC(x))|~ACINPLACE;  // sign bit of flags will hold PRISTINE status of result: 1 if all DIRECT and inplaceable or PERMANENT.  NOTE that x may be the initial virtw.
         // If so, we will still let it show PRISTINE in z, because for virtw to be inplace, it must be abandoned or virtual, neither of which can be used again
  }else{
   // not DIRECT.  result must be non-pristine, and we need to turn off pristinity of x since we are going to incorporate it
   flags&=~ACINPLACE;  // result not pristine
   {PRISTCLR(x)}  // x can never be pristine, since is being incorped
  }

  // Now that we have looked at the original usecount of x (in case it is =virtw), remove inplacing from virtw to restore its proper status
  // BUT: virtw may have been zapped & freed: detect that and don't touch virtw then.  We ensure (in FORK) that we leave *wv non0 unless we free the block
  if(likely(flags&BOX))if(likely((I)*wv!=0)){  // virtw was not destroyed
   ACIPNO(virtw);
   // if x=virtw, or the usecount of virtw changed, virtw has escaped and w must be marked as not PRISTINE
   flags|=(((wcbefore!=AC(virtw))|(x==virtw))<<AFPRISTINEX);
  }

  // prepare the result so that it can be incorporated into the overall boxed result
  if(likely(!(flags&JTWILLBEOPENED))) {
   // normal case where we are creating the result box.  Must incorp the result
   realizeifvirtual(x); razaptstackend(x);   // Since we are moving the result into a recursive box, we must ra() it.  This plus rifv plus pristine removal=INCORPRA.  We could save some fetches by bundling this code into the DIRECT path
     // razap OK, because if the result is inplaceable it must be newly created or an input from here; in either case the value is not up the tstack
  } else {
   // result will be opened.  It is nonrecursive.  description in result.h.  We don't have to realize or ra
   if(AFLAG(x)&AFUNINCORPABLE){RZ(x=clonevirtual(x));}
   // since we are adding the block to a NONrecursive boxed result,  we DO NOT have to raise the usecount of the block, but we do have to mark the block
   // non-inplaceable, because the next thing to open it might be each: each will set the inplaceable flag if the parent is abandoned, so as to allow
   // pristinity of lower results; thus we may not relax the rule that all contents must be non-inplaceable
   ACIPNO(x);  // can't ever have inplaceable contents
#if 0  // not clear this is worth doing,  and must check for pyxes
   if(ZZFLAGWORD&ZZFLAGCOUNTITEMS){
    // if the result will be razed next, we will count the items and store that in AM.  We will also ensure that the result boxes' contents have the same type
    // and item-shape.  If one does not, we turn off special raze processing.  It is safe to take over the AM field in this case, because we know this is WILLBEOPENED and
    // (1) will never assemble or epilog; (2) will feed directly into a verb that will discard it without doing any usecount modification
#if !ZZSTARTATEND  // going forwards
    A result0=AAV(zz)[0];   // fetch pointer to the first 
#else
    A result0=AAV(zz)[AN(zz)-1];  // fetch pointer to first value stored, which is in the last position
#endif
    I* zs=AS(z); I* ress=AS(result0); I zr=AR(z); I resr=AR(result0); //fetch info
    I diff=TYPESXOR(AT(z),AT(result0))|(MAX(zr,1)^MAX(resr,1)); resr=(zr>resr)?resr:zr;  DO(resr-1, diff|=zs[i+1]^ress[i+1];)  // see if there is a mismatch.  Fixed loop to avoid misprediction
    ZZFLAGWORD^=(diff!=0)<<ZZFLAGCOUNTITEMSX;  // turn off bit if so 
    I nitems=zs[0]; nitems=(zr==0)?1:nitems; AM(zz)+=nitems;  // add new items to count in zz.  zs[0] will never segfault, even if z is empty
   }
   // Note: by checking COUNTITEMS inside WILLBEOPENED we suppress support for COUNTITEMS in \. which sets WILLBEOPENEDNEVER.  It would be safe to
   // count then, because no virtual contents would be allowed.  But we are not sure that the EPILOG is safe, and this path is now off to the side
#endif
  }
#ifndef BOXEDSPARSE
  ASSERT(!ISSPARSE(AT(x)),EVNONCE);
#else
  ASSERT(fboxedsparse||!ISSPARSE(AT(x)),EVNONCE);
#endif
  // Store result & advance to next cell
  *zv++=x;
  if(unlikely(!--natoms))break;  // break to avoid fetching over the end of the input
  if(flags&BOX){virtw=C(*++wv);}else AK(virtw)+=(I)wv;  // advance to next input cell - either by fetching the next box or advancing the virtual pointer to the next atom
 }

 if(unlikely(flags&AFLAG(w)&AFPRISTINE))AFLAGCLRPRIST(w);  // if contents of w escaped, mark w no longer pristine
 // indicate pristinity of result
 AFLAGORLOCAL(z,(flags>>(ACINPLACEX-AFPRISTINEX))&AFPRISTINE)   // could synthesize rather than loading from z
 R z;
}

// u&.> called from user level
DF2(jtevery2self){
 F2PREFIP;ARGCHK2(a,w);
 I ar=AR(a); I wr=AR(w);
 I cf=ar; cf=ar<wr?cf:wr;
 R jtevery2(jtinplace,a,w,FAV(self)->fgh[0]);  // replace u&.> with u and process
} 
// u&.>, but w may be a gerund, which makes the result a list of functions masquerading as an aray of boxes
// This routine is called internally from many places without agreement checks
// We could use a flag in jt to indicate agreement already checked
 // This is called with skeletal fs sometimes.  This routine refers to valencefns[1] and flag, and passes fs into execution
A jtevery2(J jt, A a, A w, A fs){A*av,*wv,x,z,*zv;
 F2PREFIP;ARGCHK2(a,w);
 AF f2=FAV(fs)->valencefns[1];
 // Get the number of atoms, and the number of times to repeat the short side.
 // The repetition is the count of the surplus frame.
 I rpti;  // number of times short frame must be repeated
 I natoms;  // total # cells
 I flags;  // 20=w is boxed 40=a is boxed 1=w is repeated 2=a is repeated
 // other flags are: ACINPLACE=pristine result; JTWILLBEOPENED=nonrecursive result; BOX=w input was boxed; BOX<<1=a was boxed;
 //            ACPERMANENT>>1=w input was inplaceable pristine, contents can be inplaced; ACPERMANENT=same for a
 //            AFPRISTINE=a contents of w has escaped; w must be marked non-PRISTINE  AFPRISTINE<<1=same for a
 {
  I ar=AR(a); I wr=AR(w);
  I cf=ar; A la=w; cf=ar<wr?cf:wr; la=ar<wr?la:a; I lr=ar+wr-cf;  // #common frame, Ablock with long shape, long rank.
  PROD(rpti,lr-cf,AS(la)+cf);   // */ excess rank, = # repeats needed of shorter
  natoms=MAX(AN(a),AN(w)); natoms=rpti==0?rpti:natoms;  // number of atoms.  Beware of empty arg with surplus frame containing 0; if an arg is empty, so is the result
  flags=(C)(REPSGN(1-rpti)&(SGNTO0(ar-wr)+1));  // if rpti<2, no repeat; otherwise repeat short frame 1 if ar>wr 2 if wr>ar
  // Verify agreement
  ASSERTAGREE(AS(a),AS(w),cf);  // frames must agree
  // Allocate result
  GATV(z,BOX,natoms,lr,AS(la)); if(unlikely(!natoms))RETF(z) zv=AAVn(lr,z);  // make sure we don't fetch outside empty arg
 }
 // If the result will be immediately unboxed, we create a NONrecursive result and we can store virtual blocks in it.  This echoes what result.h does.
 flags|=ACINPLACE|((I)jtinplace&JTWILLBEOPENED)|(AT(w)&BOX)|((AT(a)&BOX)<<1);
 AFLAGINIT(z,(~flags<<(BOXX-JTWILLBEOPENEDX))&BOX)  // if WILLBEOPENED is NOT set, make the result a recursive box
 A virtw;
 // Get input pointer
 fauxblockINT(virtblockw,0,0);  // virtual block of rank 0, 0 atoms
 if(likely((flags&BOX)!=0)){
  virtw=C(*(wv=AAV(w)));  // if input is boxed, point to first box
  if(ASGNINPLACESGN(SGNIF((I)jtinplace&~flags,JTINPLACEWX)&SGNIF(AFLAG(w),AFPRISTINEX),w))flags|=(ACPERMANENT>>1)&-(AFLAG(w)&RECURSIBLE);  // indicates inplaceability of boxed contents.  Never if arg repeated, only if recursive
 }else{
  // if input is not boxed, use a faux-virtual block to point to the atoms.  Repurpose unneeded wv to hold length
  fauxvirtual(virtw,virtblockw,w,0,ACUC1); AN(virtw)=1; wv=(A*)bpnoun(AT(w));  // note if w has gerunds, it is always boxed & doesn't go through here
  // If not boxed, can't be pristine
 }
 // repeat for a
 A virta;
 fauxblockINT(virtblocka,0,0);  // virtual block of rank 0, 0 atoms
 if(likely((flags&(BOX<<1))!=0)){
  virta=C(*(av=AAV(a)));
  if(ASGNINPLACESGN(SGNIF((I)jtinplace&~flags,JTINPLACEAX)&SGNIF(AFLAG(a),AFPRISTINEX),a))flags|=ACPERMANENT&-(AFLAG(a)&RECURSIBLE);
 }else{
  fauxvirtual(virta,virtblocka,a,0,ACUC1); AN(virta)=1; av=(A*)bpnoun(AT(a));
 }
 // Get jt flags to pass to next level - take them from  fs, so that we always inplace this verb, which will allow us to set pristinity better
 // We must remove raze flags since we are using them here
 // If the arguments are the same, we turn off inplacing for the function but not for the argument blocks.  It only matters if a block is returned: then
 // it's OK to treat the return as pristine the arguments are zombie, even if noninplaceable ones
 jtinplace=(J)((I)jt+((FAV(fs)->flag>>(VJTFLGOK2X-JTINPLACEWX))&(a!=w))*(JTINPLACEW+JTINPLACEA));
 // If the verb returns its input block, we will have to turn off pristinity of the arg.  Replace w by its backing block
 if(AFLAG(w)&AFVIRTUAL)w=ABACK(w);
 if(AFLAG(a)&AFVIRTUAL)a=ABACK(a);
 // *** now a & w have been replaced by its backing block, if it was virtual

 I rpt=rpti=-rpti;  // repeat counters
 while(1){
  // If the input was pristine inplaceable and not virtual (i. e. originally unboxed), flag the contents as inplaceable UNLESS they are PERMANENT
  // If the input is inplaceable, there is no more use for it after this verb.  If it was pristine, every block in it is DIRECT and was either permanent or inplaceable when it was added; so if it's
  // not PERMANENT it is OK to change the usecount to inplaceable.  We must remove inplaceability on the usecount after execution, in case the input block is recursive and the contents now show a count of 2
  // (the scenario is that a pristine recursive block has a contents that is on the tstack as well, thus with usecount 2, but it's still inplaceable
  // We may thus set the usecount in an arg to 8..2,  That's OK, because it cannot be fa'd unless it is ra'd first, and the ra will wipe out the inplaceability.  We do need to keep the usecount accurate, though.
  // Actually it is possible for the usecount of a pristine contents to be 8..anything.  Consider (]&.>@:hrecur)
  I wcbefore=AC(virtw); // get (always non-inplaceable) usecount before the call
  if(((AC(virtw)-((flags<<1)&ACPERMANENT))&ACINPLACE)<0){  // AC(virtw) always has sign 0
   ACIPYESLOCAL(virtw);  // make the block inplaceable
   // If we are setting the usecount to inplaceable, that must be a change, because we do that only on contents of boxes.  If the block is inplaceable, the system requires that AM point to a tpop-stack entry
   // that will free the block, and code may simulate a free by clearing that entry.  We can't be sure that the original tpush entry is still valid, but we do know that our w block is recursive and inplaceable, so we can use
   // any pointer to the block - for example *wv - as a zappable location
   AZAPLOC(virtw)=wv;  // point to a zappable entry
  }
  I acbefore=AC(virta);if(((AC(virta)-(flags&ACPERMANENT))&ACINPLACE)<0){ACIPYESLOCAL(virta);AZAPLOC(virta)=av;}  // note uses different flag

  if(unlikely((x=CALL2IP(f2,virta,virtw,fs))==0)){ // run the user's verb
   if(likely(flags&BOX))if(likely((I)*wv!=0))ACIPNO(virtw); if(likely(flags&(BOX<<1)))if(likely((I)*av!=0))ACIPNO(virta);  R0;  // error: restore noninplaceability before we exit
  }
  // If x is DIRECT inplaceable, it must be unique and we can inherit them into a pristine result.  Otherwise clear pristinity
  if(likely((AT(x)&DIRECT)>0)){
   flags&=SGNIFPRISTINABLE(AC(x))|~ACINPLACE;  // sign bit of flags will hold PRISTINE status of result: 1 if all DIRECT and inplaceable or PERMANENT.  NOTE that x may be the initial virtw.
         // If so, we will still let it show PRISTINE in z, because for virtw to be inplace, it must be abandoned or virtual, neither of which can be used again
  }else{
   // not DIRECT.  result must be non-pristine, and we need to turn off pristinity of x since we are going to incorporate it
   flags&=~ACINPLACE;  // result not pristine
   {PRISTCLR(x)}  // x can never be pristine, since is being incorped
  }

  // Now that we have looked at the original usecount of x (in case it is =virt[aw]), remove inplacing from virt[aw] to restore its proper status
  // We have to worry that block may have been freed.  That can happen only if it is zapped AND the usecount was 1; we can see if that happened.
  // Whenever w was boxed, wv points to it, possibly as a zaploc; and in FORK we ensure that we don't zap unless the block is freed; so if *wv is not clear the block has nott been freed
  if(likely(flags&BOX))if(likely((I)*wv!=0)){  // virtw was not destroyed
   ACIPNO(virtw);
   // if x=virtw, or the usecount of virtw changed, virtw has escaped and w must be marked as not PRISTINE.
   flags|=(((wcbefore!=AC(virtw))|(x==virtw))<<AFPRISTINEX); // block escaped, arg is no longer pristine
  }
  if(likely(flags&(BOX<<1)))if(likely((I)*av!=0)){ACIPNO(virta); flags|=(((acbefore!=AC(virta))|(x==virta))<<(AFPRISTINEX+1));}
  
  // prepare the result so that it can be incorporated into the overall boxed result
  if(likely(!(flags&JTWILLBEOPENED))) {
   // normal case where we are creating the result box.  Must incorp the result
   realizeifvirtual(x); razaptstackend(x);   // Since we are moving the result into a recursive box, we must ra() it.  This plus rifv plus pristine removal=INCORPRA.  We could save some fetches by bundling this code into the DIRECT path
  } else {
   // result will be opened.  It is nonrecursive.  description in result.h.  We don't have to realize or ra
   if(AFLAG(x)&AFUNINCORPABLE){RZ(x=clonevirtual(x));}
   // since we are adding the block to a NONrecursive boxed result,  we DO NOT have to raise the usecount of the block, but we do have to mark the block
   // non-inplaceable, because the next thing to open it might be each: each will set the inplaceable flag if the parent is abandoned, so as to allow
   // pristinity of lower results; thus we may not relax the rule that all contents must be non-inplaceable
   ACIPNO(x);  // can't ever have inplaceable contents
  }

  ASSERT(!ISSPARSE(AT(x)),EVNONCE);
  // Store result & advance to next cell
  *zv++=x;
  if(!--natoms)break;
  ++rpt; I endrpt=REPSGN(rpt); rpt=rpt==0?rpti:rpt;  // endrpt=0 if end of repeat, otherwise ~0.  Reload rpt at end
  if(!(flags&endrpt&2)){if(flags&(BOX<<1)){virta=C(*++av);}else AK(virta)+=(I)av;}  // advance unrepeated arg
  if(!(flags&endrpt&1)){if(flags&BOX){virtw=C(*++wv);}else AK(virtw)+=(I)wv;}
 }
 // remove pristinity of [aw] if necessary.  If they are virtual they point to their backer, which is already non-pristine, so no harm
 if(unlikely(flags&AFLAG(w)&AFPRISTINE))AFLAGCLRPRIST(w);  // if contents of w escaped, mark w no longer pristine
 if(unlikely((flags>>1)&AFLAG(a)&AFPRISTINE))AFLAGCLRPRIST(a);  // if contents of a escaped, mark a no longer pristine
 // indicate pristinity of result
 // for a repeated argument, we must remove pristinity from the result if we have cause to remove pristinity from the argument, since what
 // was captured externally could have been repeated in both places.  This is not needed if WILLBEOPENED but it doesn't hurt
 I xfernoprist = flags&(flags>>AFPRISTINEX); xfernoprist|=xfernoprist>>1;   // low 2 bits are repeat flags, then combine them
 AFLAGORLOCAL(z,(flags>>(ACINPLACEX-AFPRISTINEX))&AFPRISTINE&~(xfernoprist<<AFPRISTINEX))   // could synthesize rather than loading from z
 RETF(z);
}

// apply f2 on items of a or w against the entirety of the other argument.  Pass on rank of f2 to reduce rank nesting
DF2(jteachl){ARGCHK3(a,w,self); I lcr=AR(a)-((UI)AR(a)>0); I lr=lr(self); lr=lcr<lr?lcr:lr; I rr=rr(self); rr=AR(w)<rr?AR(w):rr; R rank2ex(a,w,self,lr,rr,lcr,AR(w),FAV(self)->valencefns[1]);}
DF2(jteachr){ARGCHK3(a,w,self); I rcr=AR(w)-((UI)AR(w)>0); I rr=rr(self); rr=rcr<rr?rcr:rr; I lr=lr(self); lr=AR(a)<lr?AR(a):lr; R rank2ex(a,w,self,lr,rr,AR(a),rcr,FAV(self)->valencefns[1]);}

// u&.v
// PUSH/POP ZOMB is performed in atop/amp/ampco
// under is for when we could not precalculate the inverse.  The verb is in localuse
static DF1(jtunder1){F1PREFIP;A fullf; RZ(fullf=atop(invrecur(fix(FAV(self)->localuse.lu1.wvb,sc(FIXASTOPATINV))),FAV(self)->fgh[2])); R (FAV(fullf)->valencefns[0])(FAV(fullf)->flag&VJTFLGOK1?jtinplace:jt,w,fullf);}
static DF2(jtunder2){F2PREFIP;A fullf; RZ(fullf=atop(invrecur(fix(FAV(self)->localuse.lu1.wvb,sc(FIXASTOPATINV))),FAV(self)->fgh[2])); R (FAV(fullf)->valencefns[1])(FAV(fullf)->flag&VJTFLGOK2?jtinplace:jt,a,w,fullf);}
// underh has the inverse precalculated, and the inplaceability set from it.  It handles &. and &.: which differ only in rank
static DF1(jtunderh1){A hs=FAV(self)->fgh[2]; F1PREFIP;R (FAV(hs)->valencefns[0])(jtinplace,w,hs);}
static DF2(jtunderh2){A hs=FAV(self)->fgh[2]; F2PREFIP;R (FAV(hs)->valencefns[1])(jtinplace,a,w,hs);}
// undco is for when we could not precalculate the inverse
static DF1(jtundco1){F1PREFIP;A fullf; RZ(fullf=atop(inv(FAV(self)->localuse.lu1.wvb),FAV(self)->fgh[2])); R (FAV(fullf)->valencefns[0])(FAV(fullf)->flag&VJTFLGOK1?jtinplace:jt,w,fullf);}
static DF2(jtundco2){F2PREFIP;A fullf; RZ(fullf=atop(inv(FAV(self)->localuse.lu1.wvb),FAV(self)->fgh[2])); R (FAV(fullf)->valencefns[1])(FAV(fullf)->flag&VJTFLGOK2?jtinplace:jt,a,w,fullf);}

// versions for rank 0 (including each).  Passes inplaceability through
// if there is only one cell, process it through under[h]1, which understands this type; if more, loop through
static DF1(jtunder10){R jtrank1ex0(jt,w,self,jtunder1);}  // pass inplaceability through
static DF1(jtunderh10){R jtrank1ex0(jt,w,self,jtunderh1);}  // pass inplaceability through
static DF2(jtunder20){R jtrank2ex0(jt,a,w,self,jtunder2);}  // pass inplaceability through
static DF2(jtunderh20){R jtrank2ex0(jt,a,w,self,jtunderh2);}  // pass inplaceability through

static DF1(jtunderai1){A fs=FAV(self)->fgh[0]; A x,y,z;B b;I j,n,*u,*v;UC f[256],*wv,*zv;
 ARGCHK1(w);
 if(b=LIT&AT(w)&&256<AN(w)){  // long w.  run on all bytecodes, as i. 128 2  and i. 8 32
        dfv1(x,iota(v2(128L, 2L)),fs); b=x&&256==AN(x)&&NUMERIC&AT(x);
  if(b){dfv1(y,iota(v2(  8L,32L)),fs); b=y&&256==AN(y)&&NUMERIC&AT(y);}
  if(b){x=vi(x); y=vi(y); b=x&&y;} 
  if(b){u=AV(x); v=AV(y); DO(256, j=*u++; if(j==*v++&&BETWEENO(j,-256,256))f[i]=(UC)(j&255); else{b=0; break;});}  // verify both results the same & in bounds
  if(jt->jerr)RESETERR;
 }         
 if(!b)R from(dfv1(z,indexof(ds(CALP),w),fs),ds(CALP));
 n=AN(w);
 I zr=AR(w); GATV(z,LIT,n,AR(w),AS(w)); zv=UAVn(zr,z); wv=UAV(w);
 if(!bitwisecharamp(f,n,wv,zv))DQ(n, *zv++=f[*wv++];); 
 RETF(z);
}    /* f&.(a.&i.) w */

// structural under, i. e. u&.v when v is a special noninvertible form that we recognize.  Currently only , and m&{ are recognized
static DF1(jtsunder){F1PREFIP;PROLOG(777);
 I origacw=AC(w);  // preserve original inplaceability of y
 I negifipw=ASGNINPLACENEG(SGNIF(jtinplace,JTINPLACEWX),w);   // get inplaceability of y
 A v=FAV(self)->fgh[1]; A vz; RZ(vz=(FAV(v)->valencefns[0])(jt,w,v));  // execute v y, not allowing inplaceing since we have to store back
 I negifip;  // inplaceability of final result
 if((negifip=negifipw&SGNIF(AFLAG(vz),AFVIRTUALX))<0){
  // v returned a virtual block (which must be backed by y or its backer), and y was inplaceable: mark the result as virtual inplaceable nonincorpable
  AFLAG(vz)|=AFUNINCORPABLE; ACSETLOCAL(vz,ACINPLACE+ACUC1);
 }
 negifipw=vz!=w?negifipw:0;  // if we pass y into  u, clear flag that allows us to restore usecount of y
 A u=FAV(self)->fgh[0]; I ipok=vz!=w?JTINPLACEW:0; ipok=FAV(u)->flag&VJTFLGOK1?ipok:0;
 A uz; RZ(uz=(FAV(u)->valencefns[0])((J)((I)jt+ipok),vz,u,u)); negifip=uz==vz?negifip:0;     // execute u rv; rv is inplaceable unless it was passthrough
 if(negifip<0){  // u inplaced and didn't change the rank or item count
  // if u returned the block backed by originally-inplaceable y, it must have run inplace, and we can return the original y, restored to inplaceability.  tpop the virtual result of v first
  // NOTE that jtvirtual will not virtual-in-place an UNINCORPABLE block, which vz must be; so we can be sure the shape didn't change
  tpop(_ttop);  // free up whatever we allocated here
  RETF(RETARG(w));
 }else{A z;
  // otherwise return u-result v^:_1 y
  // we want to inplace into y if possible.  To do this, we restore the original usecount of y (provided y was not passed in to u - we know v doesn't change it); but before
  // we do that we have to remove any virtual blocks created here so that they don't raise y
  rifv(uz);  // if uz is a virtual, realize it in case it is backed by y
  RZ(uz=EPILOGNORET(uz));  // free any virtual blocks we created
  if((origacw&negifipw&(AC(w)-2))<0)ACRESET(w,origacw)  // usecount of y has been restored; restore inplaceability.  The use of origacw is subtle.  In a multithreaded system you mustn't reset the usecount lest another thread
      // has raised it.  So, we reset AC to ACINPLACE only in the case where it was originally inplaceable, because then we can be sure the same block is not in use in another thread.
      // Also, if AC(w) is above 1, it has escaped and must no longer be inplaced.  If it isn't above 1, it must be confined to here
  // do the inverse
  if(FAV(v)->id==CCOMMA){RZ(z=reshape(shape(w),uz));  // inv for , is ($w)&($,)
  }else{RZ(z=jtamendn2(jtinplace,uz,w,FAV(v)->fgh[0],ds(CAMEND)));   // inv for m&{ is m}&w
  }
  EPILOG(z);
 }
}


// u&.v
F2(jtunder){F2PREFIP;A x,wvb=w;AF f1,f2;B b,b1;C c,uid;I gside=-1;V*u,*v;
 ARGCHK2(a,w);
 A z; fdefallo(z)
 if(AT(w)&BOX){
  // Must be the gerund form.  Extract v and remember which argument it will apply to
  ASSERT((AR(w)^1)+(AN(w)^2)==0,EVDOMAIN);  // must be 2-element list
  ASSERT((AN(AAV(w)[0])==0) | (AN(AAV(w)[1])==0),EVDOMAIN);  // one must be empty
  gside=AN(AAV(w)[0])==0;  // the index to the argument v will act on (or -1 if not gerund).  Can't be a pyx
  wvb=fx(AAV(w)[gside]);  // turn the gerund into a verb
 }
 ASSERTVV(a,wvb); v=FAV(wvb);  // v is V* for w
 c=0; f1=0; f2=0;
 // Set flag with ASGSAFE status of u/v, and inplaceable.  It will stay inplaceable unless we select an uninplaceable processing routine, or we
 // learn that v is uninplaceable.  If v is unknown, keep inplaceable, because we will later evaluate the compound & might be able to inplace then
 I flag = (FAV(a)->flag&v->flag&VASGSAFE) + (VJTFLGOK1|VJTFLGOK2);
 // Look for special cases of v.  If v is WILLOPEN, so will the compound be - for all valences
 switch(v->id&gside){  // never special if gerund - this could evaluate to 0 or 1, neither of which is one of these codes
 case COPE:   // u&.>
  fdeffill(z,VF2WILLOPEN1|VF2WILLOPEN2A|VF2WILLOPEN2W,CUNDER,VERB,jteveryself,jtevery2self,a,w,0,flag|VIRS1,0,0,0) R z;   // this is the commonest case.  Return fast, avoiding analysis below
   // We do not expose BOXATOP or ATOPOPEN flags, because we want all u&.> to go through this path & thus we don't want to allow other loops to break in
   // We set VIRS1 just in case a user writes u&.>"n which we can ignore
   // The flags are ignored during u&.>, but they can forward through to affect previous verbs.
 case CFORK: c=((ID(v->fgh[2]))&~1)!=CLEFT;  // set c to 0 if non-capped fork with h ][; fall through
 case CAMP: {
   u=FAV(a);  // point to u in u&.v.  v is f1&g1 or (f1 g1 h1)
   if(b1=CSLASH==(uid=u->id)){x=u->fgh[0]; if(AT(x)&VERB){u=FAV(x);uid=u->id;}else uid=0;}   // uid=id of u; b1=u is x/, then uid=id of x      cases: f&.{f1&g1 or (f1 g1 h1)}  b1=0    f/&.{f1&g1 or (f1 g1 h1)}   b1=1
   b=CBDOT==uid&&(x=u->fgh[1],(((AR(x)-1)&SGNIF(AT(x),INTX))<0)&&BETWEENC(IAV(x)[0],16,32));   // b if f=m b. or m b./   where m is atomic int 16<=m<=32
   C vv=IDD(v->fgh[1]);  // 
   if(CIOTA==vv&&(!c)&&equ(ds(CALP),v->fgh[0])){   // w is  {a.&i.  or  (a. i. ][)}
    f1=b&b1?jtbitwiseinsertchar:jtunderai1;    // m b./ &. {a.&i.  or  (a. i. ][)}   or  f &. {a.&i.  or  (a. i. ][)}
    f2=((uid^CMIN)>>1)+b1?f2:(AF)jtcharfn2; f2=b>b1?(AF)jtbitwisechar:f2;   // {>. or <.} &. {a.&i.  or  (a. i. ][)}   or m b. &. {a.&i.  or  (a. i. ][)}
    flag&=~(VJTFLGOK1|VJTFLGOK2);   // not perfect, but ok
   }
   if(vv==CFROM&&AT(v->fgh[0])&NOUN)goto sunder;  // u&.(m&{)), structural under
   break;
  }
 case CCOMMA:  // u&., structural under
sunder:  // come here for all structural under
  fdeffill(z,VF2NONE,CUNDER,VERB,jtsunder,jtvalenceerr,a,w,0,flag,RMAX,RMAX,RMAX) R z;  // process the structural under when the argument arrives
 }
 I flag2=(FAV(wvb)->flag2&(VF2WILLOPEN1|VF2USESITEMCOUNT1))*((VF2WILLOPEN1+VF2WILLOPEN2A+VF2WILLOPEN2W)>>VF2WILLOPEN1X);
 I r=mr(wvb);
 // Create the standard g^:_1 @ (f & g) to use if we have no special processing (not needed if a.&i., but that's rare)
 // if gerund form, use (f g)"lf mg  for a:`v or (f~ g)~"mg rf for v`a:
 // First, create the part after the inverse
 A h; I rmr=r, rlr, rrr; 
 if(gside<0){h=amp(a,wvb); rlr=rrr=rmr;  // normal case, f&g"mg
 }else{
  f1=jtvalenceerr;  // monad not allowed with gerund v
  if(gside==0){rlr=rmr; rrr=(RANKT)FAV(a)->lrr; h=qq(swap(hook(swap(a),wvb,mark)),v2(rlr,rrr));  // (f~ g)~"mw rf
  }else{rlr=FAV(a)->lrr>>RANKTX; rrr=rmr; h=qq(hook(a,wvb,mark),v2(rlr,rrr));  // (f g)"lf mg
  }
 }
 ASSERT(h!=0,EVDOMAIN);
 // If we can calculate the inverse now, do it and so indicate
 if(nameless(wvb)){h=atop(inv(wvb),h); ASSERT(h!=0,EVDOMAIN); flag|=VFUNDERHASINV; } // h must be valid for free.  If no names in w, take the inverse and maek it as done
 // under12 are inplaceable, and pass inplaceability based on the calculated verb.  underh just passes inplaceability through, so we have to transfer the setting from h here,
 // just in case the calculated verb is not inplaceable
 // The standard verbs start with a rank loop; set the flag indicating that
 if(!f1){f1=r?(flag&VFUNDERHASINV?jtunderh1:jtunder1):(flag&VFUNDERHASINV?jtunderh10:jtunder10); flag2|=VF2RANKATOP1; flag&=FAV(h)->flag|(~VJTFLGOK1);}  // allow inplace if v is known inplaceable
 if(!f2){f2=rlr+rrr?(flag&VFUNDERHASINV?jtunderh2:jtunder2):(flag&VFUNDERHASINV?jtunderh20:jtunder20); flag2|=VF2RANKATOP2; flag&=FAV(h)->flag|(~VJTFLGOK2);}  // allow inplace if v is known inplaceable
 fdeffillall(z,flag2,CUNDER,VERB,(AF)(f1),(AF)(f2),a,w,h,(flag),rmr,rlr,rrr,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.wvb=wvb); R z;
}

F2(jtundco){F2PREFIP;AF f1=0,f2;I gside=-1, flag=0;
 ARGCHK2(a,w);
 A z; fdefallo(z)
 A wvb=w;  // the verb we will take the inverse of
 if(AT(w)&BOX){
  // Must be the gerund form.  Extract v and remember which argument it will apply to
  ASSERT((AR(w)^1)+(AN(w)^2)==0,EVDOMAIN);  // must be 2-element list
  ASSERT((AN(AAV(w)[0])==0) | (AN(AAV(w)[1])==0),EVDOMAIN);  // one must be empty
  gside=AN(AAV(w)[0])==0;  // the index to the argument v will act on (or -1 if not gerund)
  wvb=fx(AAV(w)[gside]);  // turn the gerund into a verb
 }
 ASSERTVV(a,wvb);
 // Set flag with ASGSAFE status of u/v, and inplaceability of f1/f2
 // Create the standard g^:_1 @ (f & g) to use if we have no special processing (not needed if a.&i., but that's rare)
 // if gerund form, use (f g)"lf mg  for a:`v or (f~ g)~"mg rf for v`a:
 // First, create the part after the inverse
 A h;
 if(gside<0){h=ampco(a,wvb); // normal case, f&:g
 }else{
  f1=jtvalenceerr;  // monad not allowed with gerund v
  if(gside==0){h=swap(hook(swap(a),wvb,mark));  // (f~ g)~
  }else{h=hook(a,wvb,mark);  // (f g)
  }
 }
 ASSERT(h!=0,EVDOMAIN);
 // If we can calculate the inverse now, do it and so indicate
 if(nameless(wvb)){h=atop(inv(wvb),h); ASSERT(h!=0,EVDOMAIN); flag|=VFUNDERHASINV; } // h must be valid for free.  If no names in w, take the inverse and maek it as done
 // under12 are inplaceable, and pass inplaceability based on the calculated verb.  underh just passes inplaceability through, so we have to transfer the setting from h here,
 // just in case the calculated verb is not inplaceable
 if(!f1)f1=flag&VFUNDERHASINV?jtunderh1:jtundco1; f2=flag&VFUNDERHASINV?jtunderh2:jtundco2; flag |= (FAV(a)->flag&FAV(wvb)->flag&VASGSAFE) + (FAV(h)->flag&(VJTFLGOK1|VJTFLGOK2));
 fdeffillall(z,0,CUNDCO,VERB,(AF)(f1),(AF)(f2),a,w,h,flag,RMAX,RMAX,RMAX,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.wvb=wvb); R z;
}
