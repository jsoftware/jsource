/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// result collection and assembly for modifiers
// this file will be included 4 times:
// * at the top of the file to get the flag definitions
// * outside of the loops to declare variables
// * after a result has been calculated, to store it into the final result
// * just before exit
//
// define the name ZZDEFN, ZZDECL, ZZBODY, or ZZEXIT to get the appropriate section

// names used here and relating to the result are all prefixed zz

// flags are accessed in the user's flag word; the user tells us the name of the flag word and the name of the flags to use
// non-flags are expected to be in the names specified by this code

//********** defines *******************
#ifdef ZZDEFN
// make sure these don't overlap with the definitions in cr.c.  Note dependence below on AFPRISTINE
#define ZZFLAGBOXATOPX 1 // set if u is <@f - must not be above JTWILLBEOPENED and JTCOUNTITEMS
#define ZZFLAGBOXATOP (((I)1)<<ZZFLAGBOXATOPX)
#define ZZFLAGUSEOPENX (LGSZI^1)  // result contains a cell for which a full call to OPEN will be required (viz sparse)
#define ZZFLAGUSEOPEN (((I)1)<<ZZFLAGUSEOPENX)
#define ZZFLAGBOXALLOX LGSZI  // zzbox has been allocated
#define ZZFLAGBOXALLO (((I)1)<<ZZFLAGBOXALLOX)
// next flag must match VF2 flags in jtype.h, and must be higher than BOXATOP and lower than all recursible type-flags
#define ZZFLAGWILLBEOPENEDX 4  // the result will be unboxed by the next primitive, so we can leave virtual blocks in it, as long as they aren't ones we will modify.  Requires BOXATOP also.
#define ZZFLAGWILLBEOPENED (((I)1)<<ZZFLAGWILLBEOPENEDX)
#define ZZFLAGHASUNBOXX BOXX  // result contains a nonempty non-box (this must equal BOX)
#define ZZFLAGHASUNBOX (((I)1)<<ZZFLAGHASUNBOXX)
#define ZZFLAGHASBOXX (ZZFLAGHASUNBOXX+1)  // result contains a nonempty box (must be one bit above FLAGHASUNBOX)
#define ZZFLAGHASBOX (((I)1)<<ZZFLAGHASBOXX)
// next flag must match VF2 flags in jtype.h, and must be higher than BOXATOP, and spacing of VIRTUALBOXED->UNIFORMITEMS must match ZZFLAGWILLBEOPENED->ZZCOUNTITEMS
#define ZZFLAGCOUNTITEMSX 7  // RA should count the items and verify they are homogeneous (the next primitive is ;)
#define ZZFLAGCOUNTITEMS (((I)1)<<ZZFLAGCOUNTITEMSX)
// next flag must be spaced from ZZFLAGBOXATOP by the same distance as the spacing in VF2 flags of BOXATOP1 to ATOPOPEN1
#define ZZFLAGATOPOPEN1X 8 // set if v is f@>
#define ZZFLAGATOPOPEN1 (((I)1)<<ZZFLAGATOPOPEN1X)
// next 2 flags must be spaced from ZZFLAGBOXATOP by the same distance as the spacing in VF2 flags of BOXATOP2W to ATOPOPEN2W.  This means the dyad and monad flags overlap
#define ZZFLAGATOPOPEN2WX 8 // set if v is f@> for w
#define ZZFLAGATOPOPEN2W (((I)1)<<ZZFLAGATOPOPEN2WX)
#define ZZFLAGATOPOPEN2AX 9 // set if v is f@> for a
#define ZZFLAGATOPOPEN2A (((I)1)<<ZZFLAGATOPOPEN2AX)
#define ZZFLAGVIRTWINPLACEX 10 // set if the virtual block for w can be inplaced, based on the iterator context and inplaceability of the input, but ignoring whether the verb inplaces
#define ZZFLAGVIRTWINPLACE (((I)1)<<ZZFLAGVIRTWINPLACEX)
#define ZZFLAGVIRTAINPLACEX 11 // set if the virtual block for a can be inplaced, based on the iterator context and inplaceability of the input, but ignoring whether the verb inplaces
#define ZZFLAGVIRTAINPLACE (((I)1)<<ZZFLAGVIRTAINPLACEX)

#define ZZFLAGPRISTINEX AFPRISTINEX  // 24 set if the result is PRISTINE, i. e. boxed and made up entirely of DIRECT inplaceable results.  Immaterial if not boxed
#define ZZFLAGPRISTINE AFPRISTINE


// The caller should set the initial state to ZZFLAGINITSTATE
#define ZZFLAGINITSTATE ZZFLAGPRISTINE

#define ZZFAUXCELLSHAPEMAXRANK 4  // we reserve a faux A block on the stack big enough to handle results of this rank

// Set up initial frame info.  The names are used to save variables and to push these names into registers
// THIS MUST NOT BE EXECUTED UNTIL YOU HAVE COMMITTED TO THE RESULT LOOP!
// If the function was marked as BOXATOP, we will do the boxing in the loop.  We wait until here to replace the <@f with a straight call to f, because
// if there was only 1 cell earlier places might have called the function for <@f so we must leave that intact.
// Where f is depends on whether the modifier is f@:g or ([: g h)
#define ZZPARMSNOFS(framelen,ncells) zzframelen=(framelen); zzncells=(ncells);
#define ZZPARMS(framelen,ncells,valence) ZZPARMSNOFS(framelen,ncells)  \
 if(!ZZASSUMEBOXATOP&&ZZFLAGWORD&ZZFLAGBOXATOP){fs=FAV(fs)->fgh[1]; f##valence=FAV(fs)->valencefns[valence-1];}  // if BOXATOP, we must look back into the verb

// user must define ZZINSTALLFRAME(optr) to move frame into optr++ (don't forget to increment optr!)


#undef ZZDEFN
#endif

//********** declarations ***************
#ifdef ZZDECL
// user should define:
// ZZFLAGWORD name of flags
// ZZINSTALLFRAME(zzs) code to initialize frame into *zzs++
 A zzbox=0;  // place where we will save boxed inhomogeneous result cells
 I zzresultpri = 0;  // highest priority of boxed result-cells (bit 8=nonempty flag)
#ifndef ZZASSUMEBOXATOP  // Set in grade - we never do epilog & thus don't need to count items
 I zzcounteditems=0;  // if we count the number of items in the result, this is where we do it
#endif
 A *zzboxp;  // pointer to next slot in zzbox.  Before zzbox is allocated, this is used to count the number of cells processed.
 I zzcellp;  // offset (in bytes) of the next homogeneous result cell.  No gaps are left when an inhomogeneous cell is encountered.
 I zzcelllen;  // length in bytes of a homogeneous result cell.
 A zzcellshape;  // INT array holding shape of result-cell, with one extra empty at the end.  SA[] is the data, AR is the valid length, AN is the allocated length.  May be a faux block with nothing else valid
 I zzncells;   // number of cells in the result (input)
 I zzframelen;  // length of frame of result.
 I zzfauxcellshape[ZZFAUXCELLSHAPEMAXRANK+1+2];  // will be zzcellshape for ranks < 4.  We reserve space only for AN and AS, and don't touch anything earlier.  1 is to leave 1 spare at the end, 2 is the length of AN and AR
 JMCDECL(zzendmask)  // will hold copy mask for copying result cells
#ifndef ZZWILLBEOPENEDNEVER
#define ZZWILLBEOPENEDNEVER 0  // user sets to 1 if WILLBEOPENED is not honored
#endif
#ifndef ZZSTARTATEND
#define ZZSTARTATEND 0  // user defines as 1 to build result starting at the end
#endif
#ifndef ZZPOPNEVER
#define ZZPOPNEVER 0  // user defines as 1 to force us to NEVER tpop in the loop
#endif
#ifndef ZZASSUMEBOXATOP  // set to cause us to put the result of the user's function into the result without boxing
#define ZZASSUMEBOXATOP 0
#endif

#undef ZZDECL
#endif

//*********** storing results *************

#ifdef ZZBODY
// result is in z.

// process according to state.  Ordering is to minimize branch misprediction
do{
 if(zz){  // if we have allocated the result area, we are into normal processing
  // Normal case: not first time.  Move verb result to its resting place, unless the type/shape has changed

  // The original result z will either be incorporated into zz or its items will be copied.  In either case, that makes z non-PRISTINE.
  I zzzaflag=AFLAG(z);  // we save AFLAG(z) for its PRISTINE flag.  We have to clear PRISTINE before the tpop, so here is convenient.  We just need the one flag for a short while
  if(AT(z)&BOX){PRISTCLR(z)}   // since it will be repeated, run the test

  if(!(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)){  // is forced-boxed result?  If so, just move in the box
   // not forced-boxed.  Move the result cell into the result area unless the shape changes
   // Whether the shape changes or not, if z was originally pristine inplaceable, all its boxes must be unique and we can inherit them into a pristine result.
   // It won't matter whether the boxes go through wreck processing or not, since any fill is framing fill and not a repeated cell.  Otherwise clear pristinity
   ZZFLAGWORD&=((AC(z)>>(BW-AFPRISTINEX))&zzzaflag)|~ZZFLAGPRISTINE;
   // first check the shape
   I zt=AT(z); I zzt=AT(zz); I zr=AR(z); I zzr=AR(zz); I * RESTRICT zs=AS(z); I * RESTRICT zzs=AS(zz)+zzframelen; I zexprank=zzr-zzframelen;
   // The main result must be recursive if boxed, because it has to get through EPILOG.  To avoid having to pass through the result issuing
   // ra() on the elements, we ra() each one as it comes along, while we have it in cache.  This leads to some fancy footwork at the end,
   // if we have to transfer the boxes from zz to a different output block: we want to avoid having to do usecount work then.  To assist
   // this, we want to be able to know that a result that contains boxes contains ONLY boxes - that way we know there will be no
   // conversion and no possible error during assembly.  We keep 2 flag bits to indicate the presence of boxed/nonboxed
   I zzbxm = (zt&BOX)+ZZFLAGHASUNBOX; zzbxm=AN(z)?zzbxm:0; ZZFLAGWORD |= zzbxm;  // accumulate error mask
     // change in rank/shape: fail
   zexprank=(zexprank!=zr)?-1:zexprank;  // if zexprank!=zr, make zexprank negative to make sure loop doesn't overrun the smaller shape
   DO(zexprank, zexprank+=zs[i]^zzs[i];)  // if shapes don't match, set zexprank
   if(likely(!((zt&SPARSE) + (zexprank^zr)))){  // if there was no wreck...
    // rank/shape did not change.  What about the type?
    if(unlikely(TYPESNE(zt,zzt))){
     // The type changed.  Convert the types to match.
     zt=maxtypedne(zt,zzt);  // get larger priority
     if(likely(AN(z)!=0)){I zatomct;
      // nonempty cells. we must convert the actual data.  See which we have to change
      if(zt==zzt){
       // Here the type of z must change.  Just convert it to type zt
       ASSERT(z=cvt(zt,z),EVDOMAIN);
      }else{I zzatomshift=CTTZ(bpnoun(zzt)); I zexpshift = CTTZ(bpnoun(zt))-zzatomshift;  // convert zz from type zzt to type zt.  shift for size of atom; expansion factor of the conversion, as shift amount
       // here the old values in zz must change.  Convert them.  Use the special flag to cvt that converts only as many atoms as given
#if !ZZSTARTATEND
       zatomct=zzcellp>>zzatomshift;   // get # atoms that have been filled in
#else
       zatomct=((zzcellp+zzcelllen)>>zzatomshift)-AN(zz);   // get # atoms that have been filled in, not including what we haven't filled yet in this cell
                   // make negative to tell ccvt that the value to change are at the end of the block
#endif
       ASSERT(ccvt(zt|NOUNCVTVALIDCT,zz,(A*)&zatomct),EVDOMAIN); zz=(A)zatomct;  // flag means convert only # atoms given in zatomct
       // change the strides to match the new cellsize
       if(zexpshift>=0){zzcelllen<<=zexpshift; zzcellp<<=zexpshift;}else{zzcelllen>>=-zexpshift; zzcellp>>=-zexpshift;}
       // if the new type is recursible, make sure zz is recursive.  This simplifies logic below
       ra00(zz,zt);  // make recursive if recursible
       JMCSETMASK(zzendmask,zzcelllen,ZZSTARTATEND)   // when len changes, reset mask
      }
     }else{
      // empty cells.  Just adjust the type, using the type priority
      AT(zz)=zt;  // use highest-priority empty
      AFLAGANDLOCAL(zz,~RECURSIBLE) AFLAGORLOCAL(zz,(zt&RECURSIBLE) & ((!ZZWILLBEOPENEDNEVER&&ZZFLAGWORD&ZZFLAGWILLBEOPENED)-1))  // move RECURSIBLE, if any, to new position, if not WILLBEOPENED.
     }
    }
    // The result area and the new result now have identical shapes and precisions (or compatible precisions and are empty).  Move the cells
    // The result being built is recursive if recursible.  It has recursive count, so we have to increment the usecount of any blocks we add.
    // And, we want to remove the blocks from the source so that we can free the source block immediately.  We get a small edge by handling here the special case when z is recursive inplaceable:
    // then we can get the desired effect by copying the cells and zapping z.  That has the effect of raising the usecount of the elements of z by 1, so we don't
    // actually have to touch them.  This is a transfer of ownership, and would fail if the new block is not inplaceable: for example, if the block is in a name, with
    // no frees on the tstack, it could have usecount of 1.  Transferring ownership would then leave the block in the name without an owner, and when zz is deleted the
    // name would be corrupted
    // If the value is NOT zappable, raise the usecounts of its children (leave its usecount alone)
    // it is zappable if inplaceable, direct or recursive, not virtual.  NOTE that strictly speaking a value is zappable only if inplaceable and abandoned.  Because z is a result,
    // we can be sure it is abandoned ONLY because we know that the callers of this routine use unincorpable arguments, and so it is impossible for an inplaceable but non-abandoned
    // value to have come out of the user's verb: it would have to be unincorpable and we will realize it.
#if ZZPOPNEVER
    I zzoktozap=0;  // if we have to keep the result, never zap it
#else
    I zzoktozap=AC(z)&SGNIFNOT(AFLAG(z),AFVIRTUALX);  // neg if inplaceable, not virtual
#endif
    if(unlikely(((zzoktozap&(-(AFLAG(z)&RECURSIBLE)))|((AT(z)&RECURSIBLE)-1))>=0)){  // not ok to zap OR must be made recursive, only if type is recursible
     // if unzappable OR recursible nonrecursive, raise children - even if z is UNINCORPABLE or VIRTUAL.  The children are about to be copied
     // We raise only the children, not the base block.  This converts the children to recursive usecount.  We leave the base block nonrecursive if it started
     // that way.  We may zap it later.  By not making the base recursive, we add 1 to the effective usecount of the children
     jtra(z,AT(z),0);   // raise children only and make them recursive
    }
    // copy the cells, which have been raised if needed.  If we are copying forward, it is OK to copy fullwords
    JMCR(CAV(zz)+zzcellp,AV(z),zzcelllen,ZZSTARTATEND,zzendmask)
    // Release the result now that we have copied its elements.  We do this rather than calling tpop to save the overhead, on the grounds that
    // any routine that allocated memory should have freed it, so the only thing on the tpop stack should be the result.  We do this only if the
    // result was inplaceable: otherwise the block is protected somewhere else and we can't free it.
    // We free only the z block itself, not its children: children were incorporated above
    // if the value iz zappable, zap it (it may have become zappable, if it turned recursive above).  Free only the root block
    // We should do this for virtual blocks also, to get the full effect of tpop.  When we can zap virtuals we will
    if(likely(zzoktozap<0)){*AZAPLOC(z)=0; mf(z);}  // free the root block.  If is has descendants their ownership was transferred to zz.
#if !ZZSTARTATEND
    zzcellp+=zzcelllen;  // advance to next cell
#else
    zzcellp-=zzcelllen;  // back up to next cell
#endif
    // **** z may have been destroyed and must not be used from here on ****
   }else{  // there was a wreck
    if(unlikely(ISSPARSE(zt))){
     // we encountered a sparse result.  Ecch.  We are going to have to box all the results and open them.  Remember that fact
     ZZFLAGWORD|=ZZFLAGUSEOPEN;
    }
    do{
     if(ZZFLAGWORD&ZZFLAGBOXALLO){
      // not the first wreck: we have a pointer to the A block for the boxed area
      // while we have the cell in cache, update the maximum-result-cell-shape
      I zcsr=AR(zzcellshape);  // z cell rank
      if(zr>zcsr){  // the new shape is longer than what was stored.  We have to extend the old shape with 1s
       I *zcsold=AS(zzcellshape)+zcsr;  // save pointer to end+1 of current cell size
       if(zr>=AN(zzcellshape)){GATV0(zzcellshape,INT,zr+3,0);}   // If old cell not big enough to hold new, reallocate with a little headroom.  >= to leave 1 extra for later
       AR(zzcellshape)=(RANKT)zr;   // set the new result-cell rank
       I *zcsnew=AS(zzcellshape)+zr;  // pointer to end+1 of new cell size
       DQ(zcsr, *--zcsnew=*--zcsold;) DQ(zr-zcsr, *--zcsnew=1;)   // move the old axes, followed by 1s for extra axes
       zcsr=zr;  // now the stored cell has a new rank
      }
      // compare the old against the new, taking the max.  extend new with 1s if short
      I *zcs=AS(zzcellshape); I zcs0; I zs0; DQ(zcsr-zr, zcs0=*zcs; zcs0=(zcs0==0)?1:zcs0; *zcs++=zcs0;)  DQ(zr, zcs0=*zcs; zs0=*zs++; zcs0=(zs0>zcs0)?zs0:zcs0; *zcs++=zcs0;)
      // Store the address of the result in the next slot
      INCORP(z);  // we can't store a virtual block, because its backer may change before we collect the final result
      *zzboxp=z;
      // update the result priority based on the type.  We prioritize all non-empties over empties
      I zpri=TYPEPRIORITY(zt); zpri+=AN(z)?256:0; zzresultpri=(zpri>zzresultpri)?zpri:zzresultpri;
      break;
     }else{I nboxes;
      // first wreck.  Allocate a boxed array to hold the results that mismatch zz
      // use zzboxp to tell how many results have been processed already; allocate space for the rest
#if !ZZSTARTATEND  // going forwards
      PROD(nboxes,zzframelen,AS(zz)); nboxes -= (UI)zzboxp/sizeof(A*);    // see how many boxes we need: the number of cells, minus the number of cells processed so far
#else
      nboxes = 1 + (UI)zzboxp/sizeof(A*);   // if box-pointer counts down, it already holds the # boxes left to do
#endif
      // Allocate the boxed-result area.  Every result that doesn't match zz will be stored here, and we leave zeros for the places that DID match zz,
      // so that we can tell which result-cells come from zz and which from zzbox.
      // We DO NOT make zzbox recursive, so there will be no overhead on the usecount when zzbox is freed.  Also, if WILLBEOPENED is set we may put
      // virtual blocks into the result; and virtuals are freed only by tpop and shouldn't have their usecounts messed with
      GATV0(zzbox,BOX,nboxes,0);   // rank/shape immaterial
      zzboxp=AAV(zzbox);  // init pointer to filled boxes, will be the running storage pointer
#if ZZSTARTATEND
      zzboxp+=nboxes-1;  // when running backwards, start at end
#endif
      zzresultpri=0;  // initialize the result type to low-value
      // init the vector where we will accumulate the maximum shape along each axis.  The AN field holds the allocated size and AR holds the actual size; AS[] is the data
      // We use a faux-A block to catch most of the cases.  The part before AN is not allocated on the stack and we don't refer to it
      if(likely(AR(zz)-zzframelen<=ZZFAUXCELLSHAPEMAXRANK)){zzcellshape=(A)((I)zzfauxcellshape-offsetof(AD,n)); AN(zzcellshape)=ZZFAUXCELLSHAPEMAXRANK+1;} else {GATV0(zzcellshape,INT,AR(zz)-zzframelen+3,0);}
      AR(zzcellshape)=(RANKT)(AR(zz)-zzframelen); MCISH(AS(zzcellshape),AS(zz)+zzframelen,AR(zz)-zzframelen);
      ZZFLAGWORD|=(ZZFLAGBOXALLO);  // indicate we have allocated the boxed area
     }
    }while(1);
   }
  }else{
   // forced-boxed result.  Must not be sparse.  The result box is recursive to begin with, unless WILLBEOPENED is set
   ASSERT(!ISSPARSE(AT(z)),EVNONCE);
   // If z is DIRECT inplaceable, it must be unique and we can inherit them into a pristine result.  Otherwise clear pristinity
   ZZFLAGWORD&=((AC(z)>>(BW-AFPRISTINEX))&(-(AT(z)&DIRECT)))|~ZZFLAGPRISTINE;
   if(likely(ZZWILLBEOPENEDNEVER||!(ZZFLAGWORD&ZZFLAGWILLBEOPENED))) {
    // normal case where we are creating the result box.  Must incorp the result.  Can't see an advantage is storing the virtual temporarily, and that would require testing for UNINCORP block
    realizeifvirtual(z); razap(z);   // Since we are moving the result into a recursive box, we must ra() it.  This plus rifv plus pristine flagging (above) =INCORPRA
    *zzboxp=z;  // install the new box.  zzboxp is ALWAYS a pointer to a box when force-boxed result
   } else {
    // The result of this verb will be opened next, so we can take some liberties with it.  We don't need to realize any virtual block EXCEPT one that we might
    // be reusing in this loop.  The user flags those UNINCORPABLE.  Rather than realize it we just make a virtual clone, since realizing might be expensive.
    // That is, if z is one of the virtual blocks we use to track subarrays, we mustn't incorporate it, so we clone it.  These subarrays can be inputs to functions
    // but never an output from the block it is created in, since it changes during the loop.  Thus, UNINCORPABLEs are found only in the loop that created them.
    // It might be better to keep the result recursive and transfer ownership of the virtual block, but not by much.
    if(AFLAG(z)&AFUNINCORPABLE){RZ(z=clonevirtual(z));}
    // since we are adding the block to a NONrecursive boxed result,  we DO NOT have to raise the usecount of the block, but we do have to mark the block
    // non-inplaceable, because the next thing to open it might be each: each will set the inplaceable flag if the parent is abandoned, so as to allow
    // pristinity of lower results; thus we may not relax the rule that all contents must be non-inplaceable
    // box code all over assumes that contents are never inplaceable, and since we go through here only when we are going through box code next, we honor that
    ACIPNO(z); *zzboxp=z;  // install the new box.  zzboxp is ALWAYS a pointer to a box when force-boxed result
    if(unlikely((ZZFLAGWORD&ZZFLAGCOUNTITEMS)!=0)){
     // if the result will be razed next, we will count the items and store that in AM.  We will also ensure that the result boxes' contents have the same type
     // and item-shape.  If one does not, we turn off special raze processing.  It is safe to take over the AM field in this case, because we know this is WILLBEOPENED and
     // (1) will never assemble or epilog; (2) will feed directly into a verb that will discard it without doing any usecount modification
     I diff;  // Will be set to 0 if we are unable to report the # items
#if PYXES
     // If the returned result is a pyx, we can't look into it to get its type/len.  We could see if the pyx has been resolved, but we don't
     if(likely(!(diff=AT(z)&PYX))){  // if the result is a pyx that can't be inspected, skip it
#else
     {
#endif
#if !ZZSTARTATEND  // going forwards
       A result0=AAV(zz)[0];   // fetch pointer to the first 
#else
       A result0=AAV(zz)[AN(zz)-1];  // fetch pointer to first value stored, which is in the last position
#endif
      // if the items of the new 
      I* zs=AS(z); I* ress=AS(result0); I zr=AR(z); I resr=AR(result0); //fetch info
      diff=TYPESXOR(AT(z),AT(result0))|(MAX(zr,1)^MAX(resr,1)); resr=(zr>resr)?resr:zr;  DO(resr-1, diff|=zs[i+1]^ress[i+1];)  // see if there is a mismatch.  Fixed loop to avoid misprediction
#if !ZZASSUMEBOXATOP  // ASSUMEBOXATOP is set only for m@.v, which never checks counteditems
      I nitems=zs[0]; nitems=(zr==0)?1:nitems; zzcounteditems+=nitems;  // add new items to count in zz.  zs[0] will never segfault, even if z is empty
#endif
     }
     ZZFLAGWORD^=(diff!=0)<<ZZFLAGCOUNTITEMSX;  // turn off bit if we can't say the items are homogeneous
    }
    // Note: by checking COUNTITEMS inside WILLBEOPENED we suppress support for COUNTITEMS in \. which sets WILLBEOPENEDNEVER.  It would be safe to
    // count then, because no virtual contents would be allowed.  But we are not sure that the EPILOG is safe, and this path is now off to the side
   }
  }
  // zzboxp does double duty.  Before the first wreck, it just counts the number of times we wrote to zz before the wreck.  After the first
  // wreck (or for ANY force-boxed), it points to the place where the next boxed result will be stored.  In this mode, boxp is advanced for
  // every result-cell, skipping over any that went into zz, so that we can use zzbox to tell which result-cell comes from zz and which from zzbox.
  // Thus, zzboxp is always incremented.
#if !ZZSTARTATEND  // going forwards
  ++zzboxp;
#else
  --zzboxp;
#endif
  break;  // skip the first-cell processing
 }else{I * RESTRICT is;
  // Processing the first cell.  Allocate the result area now that we know the shape/type of the result.
  // Get the rank/type to allocate for the presumed result
  // Get the type to allocate
  I natoms=AN(z);  // number of atoms per result cell
  I zzt=AT(z); I zzr=AR(z); zzt=(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)?BOX:zzt; zzr=(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)?0:zzr; natoms=(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)?1:natoms;
  zzcelllen=natoms<<bplg(zzt);  // number of bytes in one cell.
  JMCSETMASK(zzendmask,zzcelllen,ZZSTARTATEND)   // set mask for JMCR

  // # cells in result is passed in as zzncells
  // Get # atoms to allocate
  DPMULDE(natoms,zzncells,natoms)
  // Allocate the result.  If zzt is sparse we are just allocating 0 atoms in a nonrecursive block and the type is immaterial, because we are going to ensure that nothing is
  // ever copied into zzt.  BUT we must remove the sparse signbits from upper type so that the rank is not decremented when GA00 combines the rank and type, because we use the rank as a workarea
  if(unlikely(ISSPARSE(zzt))){zzr=1; natoms=0; zzt=(UI4)zzt;}  // If result is sparse, allocate 0 atoms; later, change the allocation to something that will never match a result (viz a list with negative shape).  Leave 1 bit of SPARSE for test below
  GA00(zz,zzt,natoms,zzframelen+zzr); I * RESTRICT zzs=AS(zz);  // rank is aframelen+wframelen+resultrank
  // If zz is recursible, make it recursive-usecount (without actually recurring, since it's empty), unless WILLBEOPENED is set, since then we may put virtual blocks in the boxed array
  AFLAGORLOCAL(zz,(zzt&RECURSIBLE) & ((ZZFLAGWORD&ZZFLAGWILLBEOPENED)-1))  // if recursible type, (viz box), make it recursible.  But not if WILLBEOPENED set. Leave usecount unchanged
  // Install result frame by running user's routine.  zzs must be left pointing to the cell-shape
  ZZINSTALLFRAME(zzs)
  // Install the result shape.  If we encounter a sparse result,  We are going to have to box all the results and open them.  If the sparse result is the first,
  // we are going to have a situation where nothing can ever get moved into zz, so we have to come up with a plausible zz to make that happen.  We create a zz with negative shape
  is = AS(z); zzt=zzt&SPARSE; DQ(zzr, *zzs++=zzt|*is++;);    // copy result shape; but if SPARSE, make it negative to guarantee miscompare
  // Set up the pointers/sizes for the rest of the operation
  zzboxp=AAV(zz); zzboxp=ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP?zzboxp:0;  // zzboxp=0 normally (to count stores), but for BOXATOP is the store pointer
#if !ZZSTARTATEND  // going forwards
  zzcellp=0;  // init output offset in zz to 0
#else
  zzcellp=zzcelllen*(zzncells-1);  // init output offset in zz to end+1 of 
  zzboxp+=zzncells-1;     // move zzboxp to end of block
#endif
 }
}while(1);  // go back to store the first result

#undef ZZINSTALLFRAME
#undef ZZBODY
#endif

//*********** exit ************************
#ifdef ZZEXIT
 // result is now in zz, which must not be 0

 // If WILLBEOPENED is set, there is no reason to EPILOG.  We didn't have any wrecks, we didn't allocate any blocks, and we kept the
 // result as a nonrecursive block.  In fact, we must avoid EPILOG because that would increment the usecount of the contents and apply the death warrant; then
 // when the block was finally freed the backer would leak, because the check for the backer is applied only in tpop

 // if ZZFLAGCOUNTITEMS is still set, we got through assembly with all boxed homogeneous. Mark the result.
 // Any bypass path to here must clear ZZFLAGCOUNTITEMS.  Same with WILLBEOPENED, which turns into AFVIRTUALBOXED
 if(ZZFLAGWORD&ZZFLAGWILLBEOPENED){
  AFLAGORLOCAL(zz,(ZZFLAGWORD&(ZZFLAGWILLBEOPENED|ZZFLAGCOUNTITEMS))<<(AFUNIFORMITEMSX-ZZFLAGCOUNTITEMSX))
  if(ZZFLAGWORD&ZZFLAGCOUNTITEMS)AS(zz)[0]=zzcounteditems;  // Store the # items into shape[0] of result
  RETF(zz);
 }

 ASSERT((ZZFLAGWORD&(ZZFLAGHASUNBOX|ZZFLAGHASBOX))!=(ZZFLAGHASUNBOX|ZZFLAGHASBOX),EVDOMAIN);  // if there is a mix of boxed and non-boxed results, fail
 if(ZZFLAGWORD&ZZFLAGBOXALLO){
  RZ(zz=assembleresults(ZZFLAGWORD,zz,zzbox,zzboxp,zzcellp,zzcelllen,zzresultpri,zzcellshape,zzncells,zzframelen,-ZZSTARTATEND));  // inhomogeneous results: go assemble them
 }
 // assembly may have added framing fill but it didn't repeat any cells.  If we thought the result was pristine, it is
 AFLAGORLOCAL(zz,ZZFLAGWORD&ZZFLAGPRISTINE)
#undef ZZFLAGWORD
#undef ZZWILLBEOPENEDNEVER
#undef ZZSTARTATEND
#undef ZZPOPNEVER
#undef ZZINSTALLFRAME
#undef ZZASSUMEBOXATOP

#undef ZZEXIT
#endif
