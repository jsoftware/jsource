/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Gerunds ` and `:                                          */

#include "j.h"

#define ZZDEFN
#define ZZFLAGWORD state
#include "result.h"
#define ZZFLAGARRAYWX 14  // w has multiple cells and must advance between iterations
#define ZZFLAGARRAYW (((I)1)<<ZZFLAGARRAYWX)
#define ZZFLAGARRAYAX 15  // w has multiple cells and must advance between iterations
#define ZZFLAGARRAYA (((I)1)<<ZZFLAGARRAYAX)
#define ZZFLAGISDYADX 22 // set if dyad call - MUST BE THE HIGHEST BIT - 21 is PRISTINE
#define ZZFLAGISDYAD (((I)1)<<ZZFLAGISDYADX)


// Bivalent entry point.
// self is a cyclic iterator
// we extract the pointer to the verb to be executed, advance the cycle pointer with wraparound, and call the verb
// passes inplacing through
static DF2(jtexeccyclicgerund){  // call is w,self or a,w,self
 // find the real self, valence-dependent
  F2PREFIP;ARGCHK1(w);
 I ismonad=(AT(w)>>VERBX)&1; self=ismonad?w:self;
 I nexttoexec=FAV(self)->localuse.lI; A vbtoexec=AAV(FAV(self)->fgh[2])[nexttoexec]; AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec!=0,EVDOMAIN); // get fn to exec
 ++nexttoexec; nexttoexec=AN(FAV(self)->fgh[2])==nexttoexec?0:nexttoexec; FAV(self)->localuse.lI=nexttoexec; // cyclically advance exec pointer
 w=ismonad?vbtoexec:w; R (*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
}
// similar, for executing m@.v.  This for I selectors
static DF2(jtexecgerundcellI){  // call is w,self or a,w,self
 // find the real self, valence-dependent
 F2PREFIP;ARGCHK1(w);
 I ismonad=(AT(w)>>VERBX)&1; self=ismonad?w:self;
 I nexttoexec=FAV(self)->localuse.lI;
 I gerx=IAV(FAV(self)->fgh[1])[nexttoexec];
 gerx+=REPSGN(gerx)&AN(FAV(self)->fgh[2]);
 ASSERT(BETWEENO(gerx,0,AN(FAV(self)->fgh[2])),EVINDEX);
 A vbtoexec=AAV(FAV(self)->fgh[2])[gerx]; AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec!=0,EVDOMAIN); // get fn to exec
 ++nexttoexec; FAV(self)->localuse.lI=nexttoexec; // cyclically advance exec pointer
 w=ismonad?vbtoexec:w; R (*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
}
// This for B selectors
static DF2(jtexecgerundcellB){  // call is w,self or a,w,self
 // find the real self, valence-dependent
 F2PREFIP;ARGCHK1(w);
 I ismonad=(AT(w)>>VERBX)&1; self=ismonad?w:self;
 I nexttoexec=FAV(self)->localuse.lI;
 I gerx=BAV(FAV(self)->fgh[1])[nexttoexec];
 gerx+=REPSGN(gerx)&AN(FAV(self)->fgh[2]);
 ASSERT(BETWEENO(gerx,0,AN(FAV(self)->fgh[2])),EVINDEX);
 A vbtoexec=AAV(FAV(self)->fgh[2])[gerx]; AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec!=0,EVDOMAIN); // get fn to exec
 ++nexttoexec; FAV(self)->localuse.lI=nexttoexec; // cyclically advance exec pointer
 w=ismonad?vbtoexec:w; R (*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
}

// w is a verb that refers to a cyclic gerund which is stored in h.
// z is a place to build a clone
// Result is a clone that is used to hold which gerund is next to execute
// the gerund must not be empty
A jtcreatecycliciterator(J jt, A z, A w){
 // Create the (skeletal) clone, point it to come to the execution point, set the next-verb number to 0
 AC(z)=ACPERMANENT; AT(z)=VERB; A gerund=FAV(z)->fgh[2]=FAV(w)->fgh[2]; FAV(z)->mr=FAV(w)->mr; FAV(z)->valencefns[0]=FAV(z)->valencefns[1]=jtexeccyclicgerund; FAV(z)->localuse.lI=0;
 FAV(z)->flag2=0; FAV(z)->id=CCYCITER;   // clear flags, and give this verb a proper id so it can be checked for
 if(MEMAUDIT&0xc)AFLAG(z)=0;  // in debug, flags must be valid
 R z;
}
// Similar, but also install the list of gerund results that will select the verb to run
static A jtcreategerunditerator(J jt, A z, A w, A r){  // z is result area, w is gerunds, r is selector list
 // Convert the selectors to integer/boolean
 if(!(AT(r)&(INT|B01)))RZ(r=cvt(INT,r));
 // Create the (skeletal) clone, point it to come to the execution point, set the next-verb number to 0
 AC(z)=ACPERMANENT; AT(z)=VERB; FAV(z)->fgh[2]=FAV(w)->fgh[2]; FAV(z)->fgh[1]=r; FAV(z)->mr=FAV(w)->mr;
 FAV(z)->valencefns[0]=FAV(z)->valencefns[1]=AT(r)&INT?jtexecgerundcellI:jtexecgerundcellB; FAV(z)->localuse.lI=0;
 FAV(z)->flag2=0;
 if(MEMAUDIT&0xc)AFLAG(z)=0;  // in debug, flags must be valid
 R z;
}

// w is a gerund whose max rank is r.  Result is a boxed array of VERBs, one for each gerund, if they are well formed
A jtfxeachv(J jt,I r,A w){A*wv,x,z,*zv;I n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(r>=AR(w),EVRANK);  // max rank allowed
 ASSERT(n!=0,EVLENGTH);  // gerund must not be empty
 ASSERT(BOX&AT(w),EVDOMAIN);   // must be boxed
 GATV(z,BOX,n,AR(w),AS(w)); zv=AAV(z);  // allocate one box per gerund
 DO(n, RZ(zv[i]=x=incorp(fx(wv[i]))); ASSERT(VERB&AT(x),EVDOMAIN););   // create verb, verify it is a verb  No incorporation since it's not a noun
 R z;
}

// self blocks to pass into every and thence into jtfx.  AK holds the parm into jtfx
PRIM jtfxself[2]={ {{0,0,0,0,0,0,0},{{{jtfx,0},{0,0,0},0,0,0,0,0,0,0}}} , {{1,0,0,0,0,0,0},{{{jtfx,0},{0,0,0},0,0,0,0,0,0,0}}}};

// run jtfx on each box in w, turning AR into an A block
// self is a parm passed through to jtfx, coming from jtfxself above.  if AK(self) is nonzero, we return nouns as is
// Result claims to be an array of boxes, but each box holds a function
DF1(jtfxeach){RETF(every(w,self));}

static DF1(jtcon1){A h,*hv,*x,z;V*sv;
 PREF1(jtcon1);
 sv=FAV(self); h=sv->fgh[2]; hv=AAV(h);
 GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAV(z);
 DQ(AN(h), RZ(*x++=incorp(CALL1(FAV(*hv)->valencefns[0],  w,*hv))); ++hv;);
 R ope(z);
}

static DF2(jtcon2){A h,*hv,*x,z;V*sv;
 PREF2(jtcon2);
 sv=FAV(self); h=sv->fgh[2]; hv=AAV(h);
 GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAV(z);
 DQ(AN(h), RZ(*x++=incorp(CALL2(FAV(*hv)->valencefns[1],a,w,*hv))); ++hv;);
 R ope(z);
}

// u`:3 insert 
static DF1(jtinsert){A hs,*hv,z;I hfx,j,m,n;A *old;
 ARGCHK1(w);
 SETIC(w,n); j=n-1; hs=FAV(self)->fgh[2]; m=AN(hs); hfx=j%m; hv=AAV(hs);  // m cannot be 0
 if(!n)R df1(z,w,iden(*hv));
 RZ(z=from(num(-1),w));
 old=jt->tnextpushp;
 --m; DQ(n-1, --j; --hfx; hfx=(hfx<0)?m:hfx; RZ(z=CALL2(FAV(hv[hfx])->valencefns[1],from(sc(j),w),z,hv[hfx])); z=gc(z,old);)
 RETF(z);
}

// u`:m
F2(jtevger){A hs;I k;
 ARGCHK2(a,w);
 RE(k=i0(w)); 
 if(k==GTRAIN)R exg(a);
 RZ(hs=fxeachv(RMAX,a));
 switch(k){
  case GAPPEND:
   R fdef(0,CGRCO,VERB, jtcon1,jtcon2, a,w,hs, VGERL, RMAX,RMAX,RMAX);
  case GINSERT:
   ASSERT(1>=AR(a),EVRANK);
   R fdef(0,CGRCO,VERB, jtinsert,0L,   a,w,hs, VGERL, RMAX,0L,0L);
  default:
   ASSERT(0,EVDOMAIN);
}}

F2(jttie){ARGCHK2(a,w); R over(VERB&AT(a)?arep(a):a,VERB&AT(w)?arep(w):w);}


// m@.:v y.  Execute the verbs at high rank if the operands are large
// Bivalent entry point: called as (jt,w,self) or (jt,a,w,self)
static DF2(jtcasei12){A vres,z;I gerit[128/SZI],ZZFLAGWORD;
 F1PREFIP; ARGCHK2(a,w);
 PROLOG(997);
 // see if we were called as monad or dyad.  If monad, fix up w and self
 ZZFLAGWORD=AT(w)&VERB?ZZFLAGINITSTATE|ZZFLAGWILLBEOPENED|ZZFLAGCOUNTITEMS:ZZFLAGINITSTATE|ZZFLAGWILLBEOPENED|ZZFLAGCOUNTITEMS|ZZFLAGISDYAD;  // we collect the results on the cells, but we don't assemble into a result.  To signal this, we force BOXATOP and set WILLBEOPENED
 jtinplace=(J)((I)jtinplace&(a==w?-4:-1));  // Don't allow inplacing if a==w dyad
 self=AT(w)&VERB?w:self; w=AT(w)&VERB?a:w;  // if monad, a==w
 I wr=AR(w); I ar=AR(a); I mr=MAX(wr,ar);    // ranks, and max rank   scaf turn off IRS
 // Execute v at infinite rank
 vres=FAV(self)->fgh[1];   // temp: verb to execute
 RZ(vres=(FAV(vres)->valencefns[ZZFLAGWORD>>ZZFLAGISDYADX])(jt,a,ZZFLAGWORD&ZZFLAGISDYAD?w:vres,vres));  // execute v at infinite rank, not inplace
 // Now vres is the array of selectors into the gerund.  There should be one for each cell of the input.  Check that
 // The rank of the result must be no more than the larger rank
 I vr=AR(vres);  // rank of selectors
 if(vr!=0){
  // The verb is not applied at maximum rank.  Do the full processing
  ASSERT(mr>=vr,EVRANK);  // cell must not be bigger than the whole
  ASSERTAGREE(AS(vres),AS(w),MIN(vr,wr));  // shapes must match
  if(ZZFLAGWORD&ZZFLAGISDYAD)ASSERTAGREE(AS(vres),AS(a),MIN(vr,ar));   // if dyad, check both
  I ncells; PROD(ncells,AR(vres),AS(vres));  // number of result cells
  I nar=AN(FAV(self)->fgh[2]);  // number of ARs in the gerund
  wr-=vr; wr=wr+vr?wr:IMIN;  // now wr is the rank of a cell of w (negative if repetition required in w), or IMIN if w was an atom
  ar-=vr; ar=ar+vr?ar:IMIN;  // same for a
  A zz=0;  // output area
  // We can run our trimmed-down rank loop here if there is no repetition of cells, OR if one atom is repeated over the entire other argument;
  // that is, if each cell-rank is positive, 0 or IMIN.  We also make sure there are enough results to make the processing worthwhile
  // Since the larger cell-rank must not be negative, we can look only at the smaller
  mr=MIN(ar,wr); // the smaller
  if(((-mr|~mr)&((AT(w)&SPARSE)-1)&(2*nar-ncells))<0){  // if mr is IMIN or nonneg, and there are enough results compared to # gerunds, reduce # verb executions.  Sparse doesn't do this.
   // Make sure the results are integer or boolean
   if(!(AT(vres)&(B01|INT)))RZ(vres=cvt(INT,vres));
   // grade the results, as a list
   A gradepm; RZ(gradepm=grade1(ravel(vres)));
   // decide how many result boxes to allocate based on min/max result - it's a byte or an int
   I bmsk=-((AT(vres)>>(INTX-1))&2)+1;  // mask for getting valid bits from vres
   I bsiz=AT(vres)&B01?1:SZI;  // size of element of vres
   I minx=*(I*)(CAV(vres)+IAV(gradepm)[0]*bsiz)&bmsk;   // smallest result
   I maxx=*(I*)(CAV(vres)+IAV(gradepm)[AN(gradepm)-1]*bsiz)&bmsk;  // largest result
   ASSERT(minx>=-nar&&maxx<nar,EVINDEX);  // verify that the results are in range
   I numres= maxx - minx + 1;  // max possible # of selectors
   // get sorted input order - as long as it wasn't an atom, which we will have to repeat
   A sortw,sorta;
   // Now we construe the input as a list of ar/wr-cells.  If rank is neg, the input was an atom: don't reshape
   // from here on we have w always, and optional a.  But we call a always, with optional w.  That's OK, because we set up
   // omitted a to point to w and not to modify it.
   if(wr>=0){
    RZ(w=jtredcatcell(jtinplace,w,wr));  // inplaceability of original w
    RZ(sortw=from(gradepm,w));
    // Inplace the virtual block if that is allowed
    ZZFLAGWORD |= SGNTO0((-(AT(sortw)&TYPEVIPOK))&AC(sortw))<<ZZFLAGVIRTWINPLACEX;
    ZZFLAGWORD|=ZZFLAGARRAYW;  // indicate that virtw must be updated between iterations
   }else{
    sortw=w;
   }
   // create a virtual block for the input(s).
   A virtw,virta; fauxblock(virtwfaux); fauxblock(virtafaux);
   I virtr=(wr|REPSGN(wr))+1;   // rank of a list of cells, or 0 if original arg was an atom
   fauxvirtual(virtw,virtwfaux,sortw,virtr,ACUC1) MCISH(AS(virtw),AS(w),virtr); AN(virtw)=1;  // in case atom, set AN to 1
   // Create the size of a cell in atoms.  If the original argument was an atom (?r=IMAX), use 0 for the cellsize so that the cell is repeated
   I wck,ack; PROD(wck,virtr-1,AS(sortw)+1);  // number of atoms in a cell of w
   I ak,wk=bp(AT(w)); wk&=REPSGN(~wr);  // size of atom of k, but 0 if w is an atom (so we don't advance)
   if(ZZFLAGWORD&ZZFLAGISDYAD){   // if we need to repeat for a
    if(ar>=0){
     RZ(a=jtredcatcell((J)((I)jt|(((I)jtinplace>>(JTINPLACEAX-JTINPLACEWX))&JTINPLACEW)),a,ar));  // move inplaceability of original a to w
     RZ(sorta=from(gradepm,a));
    ZZFLAGWORD |= SGNTO0((-(AT(sorta)&TYPEVIPOK))&AC(sorta))<<ZZFLAGVIRTAINPLACEX;
     ZZFLAGWORD|=ZZFLAGARRAYA;
    }else{
     sorta=a;
    }
    virtr=(ar|REPSGN(ar))+1;   // rank of a list of cells, or 0 if original arg was an atom
    fauxvirtual(virta,virtafaux,sorta,virtr,ACUC1) MCISH(AS(virta),AS(a),virtr); AN(virta)=1;
    PROD(ack,virtr-1,AS(sorta)+1);  // number of atoms in a cell of w
    ak=bp(AT(a)); ak&=REPSGN(~ar);  // size of atom of k, but 0 if w is an atom (so we don't advance)
   }else{virta=virtw; ak=0;}  // if monad, set a=w, and make the addition to AK(a) harmless
#define ZZDECL
#define ZZASSUMEBOXATOP 1
#include "result.h"
   ZZPARMSNOFS(1,numres)
#define ZZINSTALLFRAME(optr) *optr++=zzncells;  

   // for each value of selector...
   I blkstart=0;   // start and end+1 of grade
   I *gradev=IAV(gradepm);  // base of grade vector
   I graden=AN(gradepm);  // number of result cells
   do{
    // blkstart is the beginning index of the next block.
    I currres=*(I*)(CAV(vres)+gradev[blkstart]*bsiz)&bmsk;  // the result value for this block
    // binary search through results to find length of block
    I srchlo=blkstart, srchhi=graden;  // srchlo is in the block, srchhi is not
    while(srchlo<srchhi-1){  // search until we have found the last point in the block (at srchlo).  srchhi may not be a valid index
     I probex=(srchlo+srchhi)>>1;  // ignore overflow, can't happen
     I proberes=*(I*)(CAV(vres)+gradev[probex]*bsiz)&bmsk;  // fetch next look
     srchlo=proberes==currres?probex:srchlo; srchhi=proberes==currres?srchhi:probex;  // move the correct end 
    }
    currres+=REPSGN(currres)&nar;  // convert currres to nonnegative form
    // fill in the virtual block for this block
    if(ZZFLAGWORD&ZZFLAGARRAYW){  // if w is not a repeated atom...
     AS(virtw)[0]=srchhi-blkstart; AN(virtw)=(srchhi-blkstart)*wck;
     AC(virtw)=ACUC1 + ((state&ZZFLAGVIRTWINPLACE)<<(ACINPLACEX-ZZFLAGVIRTWINPLACEX));   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block (not if atom, which is never inplaceable)
    }
    if(ZZFLAGWORD&ZZFLAGARRAYA){  // if a is not a repeated atom...
     AS(virta)[0]=srchhi-blkstart; AN(virta)=AS(virta)[0]*ack;
     AC(virta)=ACUC1 + ((state&ZZFLAGVIRTAINPLACE)<<(ACINPLACEX-ZZFLAGVIRTAINPLACEX));   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block (not if atom, which is never inplaceable)
    }
   //  pull the function for the value, execute on the value with forced attributes (ASSUMEBOXATOP, WILLBEOPENED, COUNTITEMS)
   // take inplaceability from the selected verb always - we have made the cells inplaceable if possible
    A fs=AAV(FAV(self)->fgh[2])[currres];  // fetch the gerund to execute
    RZ(z=(FAV(fs)->valencefns[ZZFLAGWORD>>ZZFLAGISDYADX])((J)((I)jt+((FAV(fs)->flag>>((ZZFLAGWORD>>ZZFLAGISDYADX)+VJTFLGOK1X))*(((I)1<<(ZZFLAGWORD>>ZZFLAGISDYADX))-1))),
     virta,ZZFLAGWORD&ZZFLAGISDYAD?virtw:fs,fs));  // execute gerund at infinite rank, inplace from VJTFLGOK1/2 depending on valence

#define ZZBODY  // assemble results
#include "result.h"

    //  verify input itemcount is a prefix of result-shape
    ASSERT(AR(z)&&AS(z)[0]==srchhi-blkstart,EVRANK);  // must have the right item count
    blkstart=srchhi;  // advance for next block
    AK(virtw)+=AN(virtw)*wk;  // advance over bytes just processed (but don't move if argument is an atom)
    AK(virta)+=AN(virta)*ak;  // for a too
   }while(blkstart<graden);  // loop till we have processed all results
   I nblkscreated=zzboxp-AAV(zz);
   if(ZZFLAGWORD&ZZFLAGCOUNTITEMS){
    // The items of the results had a uniform size and type.  We can skip collecting them together, and just move the items from the boxes to their
    // final resting place
    // get the size of each item
    I itemk; PROD(itemk,AR(AAV(zz)[0])-1,AS(AAV(zz)[0])+1); I zk=itemk<<bplg(AT(AAV(zz)[0]));  // get # atoms/cell, #bytes/cell
    // allocate result area (inplace if possible)  scaf
    GA(z,AT(AAV(zz)[0]),ncells*itemk,AR(AAV(zz)[0]),AS(AAV(zz)[0])); AS(z)[0]=ncells; // one cell per input item
    C *zv=CAV(z);  // pointer to output block
    // for each (actual) result block
    I blkx=0, *agrade=gradev;  // running index of block in zz, and dest index in gradev
    do{
     // move values into result
     A thisblk=AAV(zz)[blkx];  // the block we are going to move
     I blkitemct=AS(thisblk)[0];  // number of cells
     C *resptr=CAV(thisblk);  // running input pointer
     if(zk==SZI){DQ(blkitemct, *(I*)(zv+zk* *agrade)=*(I*)resptr; resptr+=zk; ++agrade;)   // copy words - could use scatter
     }else{DQ(blkitemct, memcpy(zv+zk* *agrade,resptr,zk); resptr+=zk; ++agrade;)  // copy blocks
     }
     ++blkx;  // advance to next input block
    }while(blkx!=nblkscreated);  // loop till all blocks processed
    zz=z;  // all moved -- switch to the shuffled output
   }else{
    // inhomogeneous types or shapes.  Instantiate the entire result and then shuffle it into place
    // first, install the actual number of boxes of result, which might differ from the calculated maximum
    AN(zz)=AS(zz)[0]=nblkscreated;  // # valid boxes
    zz=ev2(gradepm,zz,"(/:~   >@:;@:((<\"_1)&.>))");  // (> ; <"_1&.> zz) /: gradepm
   }
   // If the original input had structure, rearrange the result to match it
   if(vr>1)RZ(zz=reitem(shape(vres),zz));
  }else{
   // If there are too few values to justify the sort, create an indirect iterator for them and run it
   RZ(z=jtcreategerunditerator(jt,(A)&gerit,self,vres));
   ar&=~REPSGN(ar); wr&=~REPSGN(wr);   // if neg, set to 0 
   if(ZZFLAGWORD&ZZFLAGISDYAD){
    zz=rank2ex(a,w,z,ar,wr,ar,wr,FAV(z)->valencefns[1]);  // Execute on all cells
   }else{
    zz=rank1ex(w,z,wr,FAV(z)->valencefns[0]);  // Execute on all cells8
   }
  }
  EPILOG(zz);
 }else{
  // There was only 1 cell in the result.  Apply the selected verb, inplace if possible
  I vx=i0(vres); RE(0);  // fetch index of gerund
  vx+=REPSGN(vx)&AN(FAV(self)->fgh[2]); ASSERT(BETWEENO(vx,0,AN(FAV(self)->fgh[2])),EVINDEX);
  A ger=AAV(FAV(self)->fgh[2])[vx];  // the selected gerund
  R (FAV(ger)->valencefns[state>>ZZFLAGISDYADX])((J)((REPSGN(SGNIF(FAV(ger)->flag,(state>>ZZFLAGISDYADX)+VJTFLGOK1X))|~JTFLAGMSK)&(I)jtinplace),a,state&ZZFLAGISDYAD?w:ger,ger);  // inplace if the verb can handle it
 }
}

// @.n
static F2(jtgerfrom){A*av,*v,z;I n;
 ARGCHK2(a,w);  /* 1==AR(w)&&BOX&AT(w) */
 ASSERT(1>=AR(a),EVRANK);
 if(NUMERIC&AT(a))R from(a,w);
 else{
  ASSERT(BOX&AT(a),EVDOMAIN);
  n=AN(a); av=AAV(a); 
  GATV0(z,BOX,n,1); v=AAV(z);
  DO(n, RZ(*v++=incorp(gerfrom(av[i],w))););
  R z;
}}

F2(jtagendai){I flag;
 ARGCHK2(a,w)
 if(NOUN&AT(w))R exg(gerfrom(w,a));  // noun form, as before
 // verb v.  Create a "BOX" type holding the verb form of each gerund
 A avb; RZ(avb = incorp(fxeachv(1L,a)));
  // Calculate ASGSAFE from all of the verbs (both a and w), provided the user can handle it
 flag = VASGSAFE&FAV(w)->flag; A* avbv = AAV(avb); DQ(AN(avb), flag &= FAV(*avbv)->flag; ++avbv;);  // Don't increment inside FAV!
 R fdef(0,CATDOT,VERB, jtcasei12,jtcasei12, a,w,avb, flag+((VGERL|VJTFLGOK1|VJTFLGOK2)|FAV(ds(CATDOT))->flag), RMAX, RMAX, RMAX);
}


// When u^:gerund is encountered, we replace it with a verb that comes to one of these.
// This creates several names:
// sv->self data; fs=sv->fgh[0] (the A block for the f operand); f1=f1 in sv->fgh[0] (0 if sv->fgh[0]==0); f2=f2 in sv->fgh[0] (0 if sv->fgh[0]==0);
//                gs=sv->fgh[1] (the A block for the g operand); g1=f1 in sv->fgh[1] (0 if sv->fgh[1]==0); g2=f2 in sv->fgh[1] (0 if sv->fgh[1]==0)


static DF1(jtgcl1){DECLFG;A ff,z0,z1,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); df2(ff,df1(z0,w,hv[1]),gs,ds(sv->id)); FDEPDEC(d);
 R df1(z0,df1(z1,w,hv[2]),ff);
}

// this is u^:gerund y
// Here, f1 is the original u and g1 is the original v; hs points to the gerund
// First, we run hv[1]->valencefns[0] on y (this is gerund v1, the selector/power);
// then we run (sv->id)->valencefns[1] on fs (the original u) and the result of selector/power (with self set to (sv->id):
//     sv->id is the original conjunction, executed a second time now that we have the selector/power
//     this is a conjunction execution, executing a u^:n form, and creates a derived verb to perform that function; call that verb ff
// then we execute gerund v2 on y (with self set to v2)
// then we execute ff on the result of (v2 y), with self set to ff
static DF1(jtgcr1){DECLFG;A ff,z0,z1,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); df2(ff,fs,df1(z0,w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df1(z0,df1(z1,w,hv[2]),ff);
}

static DF2(jtgcl2){DECLFG;A ff,z0,z1,z2,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); df2(ff,df2(z0,a,w,hv[1]),gs,ds(sv->id)); FDEPDEC(d);
 R df2(z0,df2(z1,a,w,hv[0]),df2(z2,a,w,hv[2]),ff);
}

static DF2(jtgcr2){DECLFG;A ff,z0,z1,z2,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); df2(ff,fs,df2(z0,a,w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df2(z0,df2(z1,a,w,hv[0]),df2(z2,a,w,hv[2]),ff);
}

// called for gerund} or ^:gerund forms.  id is the pseudocharacter for the modifier (} or ^:)
// Creates a verb that will run jtgc[rl][12] to execute the gerunds on the xy arguments, and
// then execute the operation
// a is the original u, w is the original v
A jtgconj(J jt,A a,A w,C id){A hs,y;B na;I n;
 ARGCHK2(a,w);
 ASSERT(((AT(a)|AT(w))&(VERB|BOX))==(VERB|BOX),EVDOMAIN);  // v`box or box`v
 na=1&&BOX&AT(a); y=na?a:w; n=AN(y);  // na is 1 for gerund}; y is the gerund
 ASSERT(1>=AR(y),EVRANK);
 ASSERT((n&-2)==2,EVLENGTH);  // length is 2 or 3
 ASSERT(BOX&AT(y),EVDOMAIN);
 RZ(hs=fxeach(3==n?y:link(scc(CLBKTC),y),(A)&jtfxself[0]));
 R fdef(0,id,VERB, na?jtgcl1:jtgcr1,na?jtgcl2:jtgcr2, a,w,hs, na?VGERL:VGERR, RMAX,RMAX,RMAX);
}

// verb executed for v0`v1`v2} y
static DF1(jtgav1){DECLF;A ff,ffm,ffx,*hv=AAV(sv->fgh[2]);
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow in the loop in case the generated ff generates a recursive call to }
 // If the AR is a noun, just leave it as is
 FDEPINC(d=fdep(hv[1]));
 df1(ffm,w,hv[1]);  // x v1 y - no inplacing
 FDEPDEC(d);
 RZ(ffm);  // OK to fail after FDEPDEC
 RZ(df1(ff,ffm,ds(sv->id)));   // now ff represents (v1 y)}
 if(AT(hv[2])&NOUN){ffx=hv[2];}else{RZ(df1(ffx,w,hv[2]))}
 R df1(ffm,ffx,ff);
}

static DF2(jtgav2){F2PREFIP;DECLF;A ff,ffm,ffx,ffy,*hv=AAV(sv->fgh[2]);  // hv->gerunds
A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); // protected addresses
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow in the loop in case the generated ff generates a recursive call to }
 // If the AR is a noun, just leave it as is
 FDEPINC(d=fdep(hv[1]));
 df2(ffm,a,w,hv[1]);  // x v1 y - no inplacing.
 FDEPDEC(d);
 RZ(ffm);  // OK to fail after FDEPDEC
 RZ(df1(ff,ffm,ds(sv->id)));   // now ff represents (x v1 y)}  .  Alas, ffm can no longer be virtual
 // Protect any input that was returned by v1 (must be ][)
 if(a==ffm)jtinplace = (J)(intptr_t)((I)jtinplace&~JTINPLACEA); if(w==ffm)jtinplace = (J)(intptr_t)((I)jtinplace&~JTINPLACEW);
 PUSHZOMB
 // execute the gerunds that will give the arguments to ff.  But if they are nouns, leave as is
 // x v2 y - can inplace an argument that v0 is not going to use, except if a==w
 RZ(ffy = (FAV(hv[2])->valencefns[1])(a!=w&&(FAV(hv[2])->flag&VJTFLGOK2)?(J)(intptr_t)((I)jtinplace&(sv->flag|~(VFATOPL|VFATOPR))):jt ,a,w,hv[2]));  // flag self about f, since flags may be needed in f
 // x v0 y - can inplace any unprotected argument
 RZ(ffx = (FAV(hv[0])->valencefns[1])((FAV(hv[0])->flag&VJTFLGOK2)?((J)(intptr_t)((I)jtinplace&((ffm==w||ffy==w?~JTINPLACEW:~0)&(ffm==a||ffy==a?~JTINPLACEA:~0)))):jt ,a,w,hv[0]));
 // execute ff, i. e.  (x v1 y)} .  Allow inplacing xy unless protected by the caller.  No need to pass WILLOPEN status, since the verb can't use it.  ff is needed to give access to m
 POPZOMB; R (FAV(ff)->valencefns[1])(FAV(ff)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|((ffx!=protw&&ffx!=prota?JTINPLACEA:0)+(ffy!=protw&&ffy!=prota?JTINPLACEW:0))) ):jt,ffx,ffy,ff);
}

// handle v0`v1[`v2]} to create the verb to process it when [x] and y arrive
// The id is the pseudocharacter for the function, which is passed in as the pchar for the derived verb
A jtgadv(J jt,A w,C id){A hs;I n;
 ARGCHK1(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(n&&n<=3,EVLENGTH);  // verify 1-3 gerunds
 ASSERT(BOX&AT(w),EVDOMAIN);
 RZ(hs=fxeach(3==n?w:behead(reshape(num(4),w)),(A)(&jtfxself[0])));   // convert to v0`v0`v0, v1`v0`v1, or v0`v1`v2; convert each gerund to verb
 // hs is a BOX array, but its elements are ARs
 // The derived verb is ASGSAFE if all the components are; it has gerund left-operand; and it supports inplace operation on the dyad
 // Also set the LSB flags to indicate whether v0 is u@[ or u@]
 ASSERT(AT(AAV(hs)[0])&AT(AAV(hs)[1])&AT(AAV(hs)[2])&VERB,EVDOMAIN);
 I flag=(FAV(AAV(hs)[0])->flag&FAV(AAV(hs)[1])->flag&FAV(AAV(hs)[2])->flag&VASGSAFE)+(VGERL|VJTFLGOK2)+atoplr(AAV(hs)[0]);
 R fdef(0,id,VERB, jtgav1,jtgav2, w,0L,hs,flag, RMAX,RMAX,RMAX);  // create the derived verb
}


static DF1(jtgf1){A z,h=FAV(self)->fgh[2]; R df1(z,  w,AAV(h)[0]);}
static DF2(jtgf2){A z,h=FAV(self)->fgh[2]; R df2(z,a,w,AAV(h)[0]);}

A jtvger2(J jt,C id,A a,A w){A h,*hv,x;V*v;
 RZ(x=a?a:w);
 ASSERT(2==AN(x),EVLENGTH);
 RZ(h=fxeachv(1L,x)); hv=AAV(h); v=VAV(*hv);
 R fdef(0,id,VERB, jtgf1,jtgf2, x,a?w:0L, h, VGERL, (I)v->mr,lrv(v),rrv(v));
}    /* verify and define 2-element gerund */
