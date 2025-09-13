/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
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
#define ZZPARMS(framelen,ncells,valencedcl,valence) ZZPARMSNOFS(framelen,ncells)  \
 if(!ZZASSUMEBOXATOP&&ZZFLAGWORD&ZZFLAGBOXATOP){fs=FAV(fs)->fgh[1]; f##valencedcl=FAV(fs)->valencefns[valence-1];}  // if BOXATOP, we must look back into the verb

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
 I zznunboxed;  // number of cells written to zz before the first wreck (invalid if no wreck)
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
#define ZZPOPNEVER 0  // user defines as 1 to force us to NEVER tpop/zap in the loop.  Used if INCORPABLE args used to create result
#endif
#ifndef ZZASSUMEBOXATOP  // set to cause us to put the result of the user's function into the result without boxing
#define ZZASSUMEBOXATOP 0
#endif

#undef ZZDECL
#endif

//*********** storing results *************

#ifdef ZZBODY
// result is in z.  The arguments to the verb that produced the result must be UNINCORPABLE to prevent the result from premature free: or else ZZPOPNEVER must be set

// process according to state.  Ordering is to minimize branch misprediction
do{
 if(likely(zz!=0)){  // if we have allocated the result area, we are into normal processing
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
   if(unlikely(ISSPARSE(zt)) || unlikely(zexprank!=zr))goto iswreck;  // sparse is a wreck in itself; otherwise wreck if change of rank
   if(unlikely(!TESTAGREE(zs,zzs,zexprank)))goto iswreck;    // if shapes don't match, it's a wreck
   if(likely(1)){  // normal case of no wreck
    // rank/shape match the items in zz.  We will copy this value as another item in zz, even if there have been earlier wrecks.
    if(unlikely(TYPESNE(zt,zzt))){  //  What about the type?
     // The type changed.  Convert the types to match.
     if(unlikely((POSIFHOMO(zzt,zt)&-zzcelllen)<0)){jt->etxinfo->asseminfo.assemwreckt=zt; goto assemblyerror;}  // if incompatible, flag assembly error
     zt=maxtypedne(zt,zzt);  // get larger priority
     if(likely(AN(z)!=0)){I zatomct;
      // nonempty cells. we must convert the actual data.  See which we have to change
      if(zt==zzt){
       // Here the type of z must change.  Just convert it to type zt
       RZ(z=cvt(zt,z));
      }else{I zzatomshift=CTTZ(bpnoun(zzt)); I zexpshift = CTTZ(bpnoun(zt))-zzatomshift;  // convert zz from type zzt to type zt.  shift for size of atom; expansion factor of the conversion, as shift amount
       // here the old values in zz must change.  Convert them.  Use the special flag to cvt that converts only as many atoms as given
#if !ZZSTARTATEND
       zatomct=(zzcellp>>zzatomshift)+1;   // get # atoms that have been filled in, offset by 1
#else
       zatomct=(((zzcellp+zzcelllen)>>zzatomshift)-AN(zz))-1;   // get 1s comp of # atoms that have been filled in, not including what we haven't filled yet in this cell (neg indicates trailing items)
#endif
       RZ(zz=ccvt(zt,zz,zatomct));  // flag means convert only # atoms given in zatomct
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
     zzoktozap=(I)jtra(z,AT(z),(A)zzoktozap);   // raise children only and make them recursive
    }
    // copy the cells, which have been raised if needed.  If we are copying forward, it is OK to copy fullwords
    JMCR(CAV(zz)+zzcellp,AV(z),zzcelllen,ZZSTARTATEND,zzendmask)
    // Release the cell result now that we have copied its elements.  We do this rather than calling tpop to save the overhead, on the grounds that
    // any routine that allocated memory should have freed it, so the only thing on the tpop stack should be the result.  We do this only if the
    // result was inplaceable: otherwise the block is protected somewhere else and we can't free it.
    // We free only the z block itself, not its children: children were incorporated above
    // if the value iz zappable, zap it (it may have become zappable, if it turned recursive above).  Free only the root block
    // We should do this for virtual blocks also, to get the full effect of tpop.  When we can zap virtuals we will
    if(likely(zzoktozap<0)){ZAPTSTACKEND(z) mf(z);}  // free the root block.  If it has descendants their ownership was transferred to zz.
    //   It would be a disaster to back the tstack to in front of a valid 'old' pointer held somewhere.  The subsequent tpop would never end.  Here we know that the result block, at the least, was pushed onto the tstack
 
#if !ZZSTARTATEND
    zzcellp+=zzcelllen;  // advance to next cell
#else
    zzcellp-=zzcelllen;  // back up to next cell
#endif
    // **** z may have been destroyed and must not be used from here on ****
   }else{  // the current cell-result is a wreck
iswreck:;
    if(unlikely(ISSPARSE(zt))){
     // we encountered a sparse result.  Ecch.  We are going to have to box all the results and open them.  Remember that fact
     ZZFLAGWORD|=ZZFLAGUSEOPEN;
    }
    // see if the new results are compatible with the incumbents (provided they are both not empty)
    if(unlikely((POSIFHOMO(AT(zz),zt)&-zzcelllen&-AN(z))<0)){jt->etxinfo->asseminfo.assemwreckt=zt; goto assemblyerror;}
    do{
     if(ZZFLAGWORD&ZZFLAGBOXALLO){
      // not the first wreck: we have a pointer to the A block for the boxed area
      // while we have the cell in cache, update the maximum-result-cell-shape
      I zcsr=AR(zzcellshape);  // incumbent z cell rank
      if(zr>zcsr){  // the new shape is longer than what was stored.  We have to extend the old shape with 1s.  We have preallocated extra space so we may slide the shape down front-to-back.
       I *zcsold=AS(zzcellshape);  // save pointer to start of current cell size
       if(unlikely(zr>=AN(zzcellshape))){GATV0(zzcellshape,INT,zr+3,0);}   // If old cell not big enough to hold new, reallocate with a little headroom.  >= to leave 1 extra for later
       AR(zzcellshape)=(RANKT)zr;   // set the new result-cell rank
       I *zcsnew=AS(zzcellshape)+zr-zcsr;  // pointer to trailing zcsr atoms of new location of axes, which might overlap the old
       DQNOUNROLL(zcsr, zcsnew[i]=zcsold[i];) zcsnew-=zr-zcsr; DQNOUNROLL(zr-zcsr, zcsnew[i]=1;)   // move the old axes, followed by 1s for extra axes.  The areas may overlap at the beginning, so go back to front
       zcsr=zr;  // now the stored cell has a new rank
      }
      // compare the old against the new, taking the max.  extend new with 1s if short
      I *zcs=AS(zzcellshape); I zcs0; I zs0; DQNOUNROLL(zcsr-zr, zcs0=zcs[i]; zcs0=(zcs0==0)?1:zcs0; zcs[i]=zcs0;) zcs+=zcsr-zr; DQNOUNROLL(zr, zcs0=zcs[i]; zs0=zs[i]; zcs0=(zs0>zcs0)?zs0:zcs0; zcs[i]=zcs0;)
      // Store the address of the result in the next slot
      INCORP(z);  // we can't store a virtual block, because its backer may change before we collect the final result
      *zzboxp=z;
      // update the result priority based on the type.  We prioritize all non-empties over empties
      I zpri=TYPEPRIORITY(zt); zpri+=AN(z)?256:0; zzresultpri=(zpri>zzresultpri)?zpri:zzresultpri;
      break;
     }else{I nboxes;
      // first wreck.  Allocate a boxed array to hold the results that mismatch zz
      // use zzboxp to tell how many results have been processed already; allocate space for the rest
      zznunboxed=(UI)zzboxp>>LGSZI;  // number of boxes filled before first wreck
#if !ZZSTARTATEND  // going forwards
      PROD(nboxes,zzframelen,AS(zz)); nboxes -= zznunboxed;    // see how many boxes we need: the number of cells, minus the number of cells processed so far
#else
      nboxes = 1 + zznunboxed;   // if box-pointer counts down, it already holds the # boxes left to do
#endif
      // Allocate the boxed-result area.  Every result that doesn't match zz will be stored here, and we leave zeros for the places that DID match zz,
      // so that we can tell which result-cells come from zz and which from zzbox.
      // We DO NOT make zzbox recursive, so there will be no overhead on the usecount when zzbox is freed.  Also, if WILLBEOPENED is set we may put
      // virtual blocks into the result; and virtuals are freed only by tpop and shouldn't have their usecounts messed with
      GATV0(zzbox,BOX,nboxes,0);   // rank/shape immaterial
      zzboxp=AAV0(zzbox);  // init pointer to filled boxes, will be the running storage pointer
#if ZZSTARTATEND
      zzboxp+=nboxes-1;  // when running backwards, start at end
#endif
      zzresultpri=0;  // initialize the result type to low-value
      // init the vector where we will accumulate the maximum shape along each axis.  The AN field holds the allocated size and AR holds the actual size; AS[] is the data
      // We use a faux-A block to catch most of the cases.  The part before AN is not allocated on the stack and we don't refer to it
      // The shape starts at AS, and if the rank increases we have to slide the shape down.  If we ended the shape at the and of the allocated area we could just extend it with
      // 1s in place.  We don't do this because it would add a bit to the shape-comparison time, and rank extensions are very rare - especially a second one.
      if(likely(AR(zz)-zzframelen<=ZZFAUXCELLSHAPEMAXRANK)){zzcellshape=(A)((I)zzfauxcellshape-offsetof(AD,n)); AN(zzcellshape)=ZZFAUXCELLSHAPEMAXRANK+1;} else {GATV0(zzcellshape,INT,AR(zz)-zzframelen+3,0);}
      AR(zzcellshape)=(RANKT)(AR(zz)-zzframelen); MCISH(AS(zzcellshape),AS(zz)+zzframelen,AR(zz)-zzframelen);
      ZZFLAGWORD|=(ZZFLAGBOXALLO);  // indicate we have allocated the boxed area
     }
    }while(1);
   }
  }else{
   // forced-boxed result.  Must not be sparse.  The result box is recursive to begin with, unless WILLBEOPENED is set.  Wrecks are impossible
   ASSERT(!ISSPARSE(AT(z)),EVNONCE);
   // If z is DIRECT inplaceable, it must be unique and we can inherit them into a pristine result.  Otherwise clear pristinity
   ZZFLAGWORD&=((AC(z)>>(BW-AFPRISTINEX))&(-(AT(z)&DIRECT)))|~ZZFLAGPRISTINE;
   if(likely(ZZWILLBEOPENEDNEVER||!(ZZFLAGWORD&ZZFLAGWILLBEOPENED))) {
    // normal case where we are creating the result box.  Must incorp the result.  Can't see an advantage is storing the virtual temporarily, and that would require testing for UNINCORP block
    if(unlikely(AFLAG(z)&AFANCHORED)){RZ(z=ca(z)) ACINITZAP(z);}  // user shouldn't box a ANCHORED, but if so we clone it
    else{realizeifvirtual(z); razaptstackend(z);}   // Since we are moving the result into a recursive box, we must ra() it.  This plus rifv plus pristine flagging (above) =INCORPRA.  We trouble to see if we can shorten tstack, since that is likely
    *zzboxp=z;  // install the new box.  zzboxp is ALWAYS a pointer to a box when force-boxed result
   }else{
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
     // the result will be razed next.  We will count the items and store that in AM.  We will also ensure that the result boxes' contents have the same type
     // and item-shape.  If one does not, we turn off special raze processing.  It is safe to take over the AM field in this case, because we know this is WILLBEOPENED and
     // (1) will never assemble or epilog; (2) will feed directly into a verb that will discard it without doing any usecount modification
#if PYXES
     // If the returned result is a pyx, we can't look into it to get its type/len.  We could see if the pyx has been resolved, but we don't
     if(unlikely(AT(z)&PYX))goto inhomoitems;   // if the result is a pyx, which can't be inspected, treat as inhomogeneous item
#endif
      // not a pyx - we can count the items
#if !ZZSTARTATEND  // going forwards
     A result0=AAV(zz)[0];   // fetch pointer to the first 
#else
     A result0=AAV(zz)[AN(zz)-1];  // fetch pointer to first value stored, which is in the last position
#endif
     // see if the items of the new match the old, and increment the number of items
     I* zs=AS(z); I* ress=AS(result0); I zr=AR(z); I resr=AR(result0); //fetch info
#if !ZZASSUMEBOXATOP  // ASSUMEBOXATOP is set only for m@.v, which never checks counteditems
     I nitems=zs[0]; nitems=(zr==0)?1:nitems; zr=zr==0?1:zr; zzcounteditems+=nitems;  // add new items to count in zz.  zs[0] will never segfault, even if z is empty
#else
     zr=zr==0?1:zr;   // treat atom like singleton list
#endif
     resr=resr==0?1:resr;  // treat atom like singleton list
     if(unlikely(TYPESXOR(AT(z),AT(result0))+(zr^resr)))goto inhomoitems; if(likely(TESTAGREE(zs+1,ress+1,resr-1)))goto homoitems;
inhomoitems: ;  // here when we have had a mismatch on shape etc - stop looking at it
     ZZFLAGWORD&=~ZZFLAGCOUNTITEMS;  // turn off bit (which must be set now) if we can't say the items are homogeneous
homoitems:;  // here when new item is same shape etc as old - count was updated
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
  I natoms=AN(z); I zzt=AT(z); I zzr=AR(z);   // number of atoms per result cell; type and rank of first result cell
  // Type of zz is that of z, except for BOXATOP: then it's BOX.  The value can be a pyx only if BOXATOP is set, so we know zz can't itself be a pyx
  zzt=(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)?BOX:zzt; zzr=(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)?0:zzr; natoms=(ZZASSUMEBOXATOP||ZZFLAGWORD&ZZFLAGBOXATOP)?1:natoms;
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
  // Install the result shape.
  MCISH(zzs,AS(z),zzr)    // copy result shape
  if(unlikely(zzt&SPARSE)){zzs[zzr-1]=SPARSE>>1;}  // If we encounter a sparse result,  We are going to have to box all the results and open them.  If the sparse result is the first,
         // we are going to have a situation where nothing can ever get moved into zz, so we have to come up with a plausible zzs to make that happen.  Sparse cannot be an atom!
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
#if ANASARGEEMENT
// Not applicable because Store the # items into shape[0] of result
  R zz;
#else
  RETF(zz);
#endif
 }

 if(ZZFLAGWORD&ZZFLAGBOXALLO){
  RZ(zz=assembleresults(ZZFLAGWORD,zz,zzbox,zzboxp,zzcellp,zzcelllen,zzresultpri,zzcellshape,zzncells,zzframelen,-ZZSTARTATEND));  // inhomogeneous results: go assemble them
 }else if(unlikely(0)){
assemblyerror: ;  // assembly error.  Fill in the information that will be needed for formatting
  jt->etxinfo->asseminfo.assemframelen=zzframelen;  // length of frame of assembled data
  jt->etxinfo->asseminfo.assemorigt=AT(zz);  // AT of first result
  MC(jt->etxinfo->asseminfo.assemshape,AS(zz),zzframelen<<LGSZI);  // frame of assembled data
#if !ZZSTARTATEND  // going forwards
  jt->etxinfo->asseminfo.assemwreckofst=(ZZFLAGWORD&ZZFLAGBOXALLO)?(zzboxp-AAV(zzbox))+zznunboxed:(UI)zzboxp>>LGSZI;  // index of (unwritten) inhomo item
#else
  jt->etxinfo->asseminfo.assemwreckofst=(ZZFLAGWORD&ZZFLAGBOXALLO)?(zzboxp-AAV(zzbox)):(UI)zzboxp>>LGSZI;  // index of (unwritten) inhomo item
#endif
  ASSERT(0,EVASSEMBLY)
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
