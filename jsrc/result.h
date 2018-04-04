/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Rank Associates                                           */

// result collection and assembly for modifiers
// this file will be included 3 times:
// * outside of the loops to declare variables
// * after a result has been calculated, to store it into the final result
// * just before exit
//
// define the name ZZDEFN, ZZDECL, ZZBODY, or ZZEXIT to get the appropriate sectio 

// names used here and relating to the result are all prefixed zz

// flags are accessed in the user's flag word; the user tells us the name of the flag word and the name of the flags to use
// non-flags are expected to be in the names specified by this code

//********** defines *******************
#ifdef ZZDEFN
#define ZZFLAGNOPOPX 0 // set to suppress tpop
#define ZZFLAGNOPOP (1LL<<ZZFLAGNOPOPX)
#define ZZFLAGBOXATOPX 1 // set if u is <@f
#define ZZFLAGBOXATOP (1LL<<ZZFLAGBOXATOPX)
#define ZZFLAGUSEOPENX 2  // result contains a cell for which a full call to OPEN will be required (viz sparse)
#define ZZFLAGUSEOPEN (1LL<<ZZFLAGUSEOPENX)
#define ZZFLAGBOXALLOX 3  // zzbox has been allocated
#define ZZFLAGBOXALLO (1LL<<ZZFLAGBOXALLOX)
#define ZZFLAGHASUNBOXX 5  // result contains a nonempty non-box (this must equal BOX)
#define ZZFLAGHASUNBOX (1LL<<ZZFLAGHASUNBOXX)
#define ZZFLAGHASBOXX 6  // result contains a nonempty box
#define ZZFLAGHASBOX (1LL<<ZZFLAGHASBOXX)

// Set up initial frame info.  The names are used to save variables and to push these names into registers
 // If the function was marked as BOXATOP, we will do the boxing in the loop.  We wait until here to replace the <@f with a straight call to f, because
 // if there was only 1 cell earlier places might have called the function for <@f so we must leave that intact.
#define ZZPARMS(oframe,oframelen,iframe,iframelen,ncells,valence) zzcellp=(I)(oframe); zzcelllen=(oframelen); zzboxp=(A*)(iframe); zzwf=(iframelen); zzncells=(ncells); \
 if(ZZFLAGWORD&ZZFLAGBOXATOP){fs=VAV(fs)->g; f##valence=VAV(fs)->f##valence;}


#undef ZZDEFN
#endif

//********** declarations ***************
#ifdef ZZDECL
// user should define:
// ZZFLAGWORD name of flags
 A zzbox=0;  // place where we will save boxed inhomogeneous result cells
 A *zzboxp;  // pointer to next slot in zzbox.  Before zzbox is allocated, this is used to count the number of cells processed.  At start: &inner frame
 I zzcellp;  // offset (in bytes) of the next homogeneous result cell.  No gaps are left when an inhomogeneous cell is encountered.  At start: &outer frame
 I zzcelllen;  // length in by of a homogeneous result cell.  At start: length of outer frame
 I zzresultpri = 0;  // highest priority of boxed result-cells (bit 8=nonempty flag)
 A zzcellshape;  // INT array holding shape of result-cell, with one extra empty at the end
 I zzncells;   // number of cells in the result (input)
 I zzwf;  // length of frame of result.  At start: length of inner frame
 I zzold;  // place to tpop to between executions
 jt->rank=0;  // needed for cvt ?? scaf
#undef ZZDECL
#endif


//*********** storing results *************

#ifdef ZZBODY
// result is in z.  state here cannot be ERRFIRST

// obsolete // if the result is boxed, accumulate the SMREL info
// obsolete if(state&AFNOSMREL)state&=AFLAG(y)|~AFNOSMREL;  // if we ever get an SMREL (or a non-boxed result), stop looking

// process according to state.  Ordering is to minimize branch misprediction
do{
 if(zz){  // if we have allocated the result area, we are into normal processing
  // Normal case: not first time.  Move verb result to its resting place, unless the type/shape has changed
  if(!(ZZFLAGWORD&ZZFLAGBOXATOP)){  // is forced-boxed result?  If so, just move in the box
   // not forced-boxed.  Move the result cell into the result area unless the shape changes
   // first check the shape
   I zt=AT(z); I zzt=AT(zz); I zr=AR(z); I zzr=AR(zz); I * RESTRICT zs=AS(z); I * RESTRICT zzs=AS(zz)+zzwf; I zexprank=zzr-zzwf;
   // The main result must be recursive if boxed, because it has to get through EPILOG.  To avoid having to pass through the result issuing
   // ra() on the elements, we ra() each one as it comes along, while we have it in cache.  This leads to some fancy footwork at the end,
   // if we have to transfer the boxes from zz to a different output block: we want to avoid having to do usecount work then.  To assist
   // this, we want to be able to know that a result that contains boxes contains ONLY boxes - that way we know there will be no
   // conversion and no possible error during assembly.  We keep 2 flag bits to indicate the presence of boxed/nonboxed
   I zzbxm = (zt&BOX)+ZZFLAGHASUNBOX; zzbxm=AN(z)?zzbxm:0; ZZFLAGWORD |= zzbxm;  // accumulate error mask
     // change in rank/shape: fail
   zexprank=(zexprank!=zr)?-1:zexprank;  // if zexprank!=zr, make zexprank negative to make sure loop doesn't overrun the smaller shape
   DO(zexprank, zexprank=(zs[i]!=zzs[i])?-1:zexprank;)  // if shapes don't match, set zexprank
   if(!(zt&SPARSE) && zexprank==zr){  // if there was no wreck...
    // rank/shape did not change.  What about the type?
    if(TYPESNE(zt,zzt)){
     // The type changed.  Convert the types to match.
     I zpri=jt->typepriority[CTTZ(zt)]; I zzpri=jt->typepriority[CTTZ(zzt)]; zt=zzpri>zpri?zzt:zt;  // get larger priority   code copied from jtmaxtype but we know not sparse, not 0
     if(AN(z)){I zatomct;
      // nonempty cells. we must convert the actual data.  See which we have to change
      if(zt==zzt){
       // Here the type of z must change.  Just convert it to type zt
       ASSERT(z=cvt(zt,z),EVDOMAIN);
      }else{I zzatomshift=CTTZ(bp(zzt)); I zexpshift = CTTZ(bp(zt))-zzatomshift;  // convert zz from type zzt to type zt.  shift for size of atom; expansion factor of the conversion, as shift amount
       // here the old values in zz must change.  Convert them.  Use the special flag to cvt that converts only as many atoms as given
       zatomct=zzcellp>>zzatomshift;   // get # atoms that have been filled in
       ASSERT(ccvt(zt|NOUNCVTVALIDCT,zz,(A*)&zatomct),EVDOMAIN); zz=(A)zatomct;  // flag means convert zcellct atoms
       // change the strides to match the new cellsize
       zzcelllen<<=zexpshift; zzcellp<<=zexpshift;
       // recalculate whether we can pop the stack.  We can, if the type is DIRECT and zzbox has not been allocated.  We could start zz as B01 (pop OK), then promote to
       // XNUM (pop not OK), then to FL (pop OK again).  It's not vital to be perfect, but then again it's cheap to be
       ZZFLAGWORD&=~ZZFLAGNOPOP; ZZFLAGWORD|=((zt&DIRECT)?0:ZZFLAGNOPOP)|(ZZFLAGWORD>>(ZZFLAGBOXALLOX-ZZFLAGNOPOPX));
       zzold=jt->tnextpushx;  // reset the pop-back point so we don't free zz during a pop.  Could gc if needed
      }
     }else{
      // empty cells.  Just adjust the type, using the type priority
      AT(zz)=zt;  // use highest-priority empty
     }
    }
    // The result area and the new result now have compatible types.  Move the cells
    if(zzcelllen){  // questionable
     // Here there are cells to move
     if(zt&RECURSIBLE){
      // The result being built is recursible (meaning boxed, since it's a noun).  It has recursive count, so we have to increment the usecount of any blocks we add.
      // And, we want to remove the blocks from the source so that we can free the source block immediately.  We get a small edge by noting the special case when z is recursive with
      // a usecount of 1: then we can get the desired effect by just marking z as nonrecursible.  That has the effect of raising the usecount of the elements of zt by 1, so we don't
      // actually have to touch them.
      if(ACIPISOK(z)&&AFLAG(z)&RECURSIBLE){
       AFLAG(z)&=~RECURSIBLE;  // mark as nonrecursive, transferring ownership to the new block
       MC(CAV(zz)+zzcellp,AV(z),zzcelllen);  // move the result-cell to the output, advance to next output spot
      }else{
       // copy and raise the elements (normal path)
       A *zzbase=(A*)(CAV(zz)+zzcellp), *zbase=AAV(z); DO(AN(z), A zblk=zbase[i]; ra(zblk); zzbase[i]=zblk;)
      }
     }else{
      MC(CAV(zz)+zzcellp,AV(z),zzcelllen);  // move the result-cell to the output, advance to next output spot
     }
     zzcellp+=zzcelllen;  // advance to next cell
    }
    if(!(ZZFLAGWORD&ZZFLAGNOPOP))tpop(zzold);  // Now that we have copied to the output area, free what the verb allocated
   }else{  // there was a wreck
    if(zt&SPARSE){  // A good compiler will elide this test
     // we encountered a sparse result.  Ecch.  We are going to have to box all the results and open them.  Remember that fact
     ZZFLAGWORD|=ZZFLAGUSEOPEN;
    }
    do{
     if(ZZFLAGWORD&ZZFLAGBOXALLO){
      // not the first wreck: we have a pointer to the A block for the boxed area
      // while we have the cell in cache, update the maximum-result-cell-shape
      I zcsr=AS(zzcellshape)[0];  // z cell rank
      if(zr>zcsr){  // the new shape is longer than what was stored.  We have to extend the old shape with 1s
       I *zcsold=IAV(zzcellshape)+zcsr;  // save pointer to end+1 of current cell size
       if(zr>=AN(zzcellshape)){GATV(zzcellshape,INT,zr+3,1,0);}   // If old cell not big enough to hold new, reallocate with a little headroom.  Leave 1 extra for later
       AS(zzcellshape)[0]=zr;   // set the new result-cell rank
       I *zcsnew=IAV(zzcellshape)+zr;  // pointer to end+1 of new cell size
       DO(zcsr, *--zcsnew=*--zcsold;) DO(zr-zcsr, *--zcsnew=1;)   // move the old axes, followed by 1s for extra axes
      }
      // compare the old against the new, taking the max.  extend new with 1s if short
      I *zcs=IAV(zzcellshape); I zcs0; I zs0; DO(zcsr-zr, zcs0=*zcs; zcs0=(zcs0==0)?1:zcs0; *zcs++=zcs0;)  DO(zr, zcs0=*zcs; zs0=*zs++; zcs0=(zs0>zcs0)?zs0:zcs0; *zcs++=zcs0;)
      // Store the address of the result in the next slot
      INCORP(z);  // we can't store a virtual block, because its backer may change before we collect the final result
      *zzboxp=z;
      // update the result priority based on the type.  We prioritize all non-empties over empties
      I zpri=jt->typepriority[CTTZ(zt)]; zpri+=AN(z)?256:0; zzresultpri=(zpri>zzresultpri)?zpri:zzresultpri;
      break;
     }else{I nboxes;
      // first wreck.  Allocate a boxed array to hold the results that mismatch zz
      // use zzboxp to tell how many results have been processed already; allocate space for the rest
      PROD(nboxes,zzwf,AS(zz)); nboxes -= (zzboxp-(A*)0);   // see how many boxes we need: the number of cells, minus the number of cells processed so far
      // Allocate the boxed-result area.  Every result that doesn't match zz will be stored here, and we leave zeros for the places that DID match zz,
      // so that we can tell which result-cells come from zz and which from zzbox.
      // We DO NOT make zzbox recursive, so there will be no overhead on the usecount when zzbox is freed.  This is OK because we stop tpop'ing
      GATV(zzbox,BOX,nboxes,0,0);   // rank/shape immaterial
      zzboxp=AAV(zzbox);  // init pointer to filled boxes, will be the running storage pointer
      zzresultpri=0;  // initialize the result type to low-value
      // init the vector where we will accumulate the maximum shape along each axis.  The N field holds the allocated size and AS holds the actual size
      GATV(zzcellshape,INT,AR(zz)-zzwf+3,1,0); AS(zzcellshape)[0]=AR(zz)-zzwf; I *zzv=AS(zz)+zzwf, *zzcs=IAV(zzcellshape); DO(AS(zzcellshape)[0], zzcs[i]=zzv[i];);
      ZZFLAGWORD|=(ZZFLAGBOXALLO|ZZFLAGNOPOP);  // indicate we have allocated the boxed area, and that we can no longer pop back to our input, because those results are stored in a nonrecursive boxed array
     }
    }while(1);
   }
  }else{
   // forced-boxed result.  Must not be sparse.  The result box is recursive to begin with
   ASSERT(!(AT(z)&SPARSE),EVNONCE);
   realizeifvirtual(z); ra(z);   // Since we are moving the result into a recursive box, we must ra() it.  This plus rifv=INCORP
   *zzboxp=z;  // install the new box.  zzboxp is ALWAYS a pointer to a box when force-boxed result
  }
  zzboxp++;  // advance the box pointer, whether it points to valid data or is just a counter of early results
  break;  // skip the first-cell processing
 } else{I * RESTRICT is;
  // Processing the first cell.  Allocate the result area now that we know the shape/type of the result.
  // Get the rank/type to allocate for the presumed result
  // Get the type to allocate
  I natoms=AN(z);  // number of atoms per result cell
  I zzt=AT(z); I zzr=AR(z); zzt=(ZZFLAGWORD&ZZFLAGBOXATOP)?BOX:zzt; zzr=(ZZFLAGWORD&ZZFLAGBOXATOP)?0:zzr; natoms=(ZZFLAGWORD&ZZFLAGBOXATOP)?1:natoms;
  // If result is sparse, change the allocation to something that will never match a result (viz a list with negative shape)
  zzr=(zzt&SPARSE)?1:zzr; natoms=(zzt&SPARSE)?0:natoms;
  I nbytes=natoms*bp(zzt);  // number of bytes in one cell.  We have to save this while zzcelllen is tied up
  // names used for initial values: zzcelllen=aframelen  zzcellp->aframe  zzboxp->wframe  zzwf=wframelen
  // # cells in result is passed in as zzncells
  // Get # atoms to allocate
  RE(natoms=mult(natoms,zzncells));
  // Allocate the result
  GA(zz,zzt,natoms,zzcelllen+zzwf+zzr,0L); I * RESTRICT zzs=AS(zz);
  // If zz is boxed, make it recursive-usecount (without actually recurring, since it's empty)
  AFLAG(zz) |= zzt&RECURSIBLE;  // if recursible type, (viz box), make it recursible.  Leave usecount unchanged
  // If zz is not DIRECT, it will contain things allocated on the stack and we can't pop back to here
  ZZFLAGWORD |= (zzt&DIRECT)?0:ZZFLAGNOPOP;
  // Remember the point before which we allocated zz.  This will be the free-back-to point, unless we require boxes later
  zzold=jt->tnextpushx;  // pop back to AFTER where we allocated our result and argument blocks
  // Install shape
  is = (I*)zzcellp; DO(zzcelllen, *zzs++=*is++;);  // copy outer frame
  is = (I*)zzboxp; DO(zzwf, *zzs++=*is++;);  // copy inner frame
  // If we encounter a sparse result,  We are going to have to box all the results and open them.  If the sparse result is the first,
  // we are going to have a situation where nothing can ever get moved into zz, so we have to come up with a plausible zz to make that happen.  We create a zz with negative shape
  is = AS(z); zzt=-(zzt&SPARSE); DO(zzr, *zzs++=zzt|*is++;);    // copy result shape; but if SPARSE, make it negative to guarantee miscompare
  // Set up the pointers/sizes for the rest of the operation
  zzwf+=zzcelllen;  // leave zzwf as the total length of result frame
  zzcelllen=nbytes;   // cell length, for use in the main body
  zzboxp=AAV(zz); zzboxp=(ZZFLAGWORD&ZZFLAGBOXATOP)?zzboxp:0;  // Start out zzboxp so we can use it as a counter of cells processed before zzbox needed
  zzcellp=0;  // init output offset in zz to 0
 }
}while(1);  // go back to store the first result

#undef ZZBODY
#endif

//*********** exit ************************
#ifdef ZZEXIT
 ASSERT((ZZFLAGWORD&(ZZFLAGHASUNBOX|ZZFLAGHASBOX))!=(ZZFLAGHASUNBOX|ZZFLAGHASBOX),EVDOMAIN);  // if there is a mix of boxed and non-boxed results, fail
 if(ZZFLAGWORD&ZZFLAGBOXALLO){
  RZ(zz=assembleresults(ZZFLAGWORD,zz,zzbox,zzboxp,zzcellp,zzcelllen,zzresultpri,zzcellshape,zzncells,zzwf));  // inhomogeneous results: go assemble them
 }
// result is now in zz
#undef ZZFLAGWORD
// obsolete #undef ZZFLAGNOPOPX
// obsolete #undef ZZFLAGNOPOP
// obsolete #undef ZZFLAGBOXATOPX
// obsolete #undef ZZFLAGBOXATOP
// obsolete #undef ZZFLAGUSEOPENX
// obsolete #undef ZZFLAGUSEOPEN
// obsolete #undef ZZFLAGHASUNBOXX
// obsolete #undef ZZFLAGHASUNBOX
// obsolete #undef ZZFLAGHASBOXX
// obsolete #undef ZZFLAGHASBOX
// obsolete #undef ZZFLAGBOXALLOX
// obsolete #undef ZZFLAGBOXALLO
#undef ZZEXIT
#endif
