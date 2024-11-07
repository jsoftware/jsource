/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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
#define ZZFLAGISDYADX (AFPRISTINEX+1) // set if dyad call - MUST BE THE HIGHEST BIT - above PRISTINE
#define ZZFLAGISDYAD (((I)1)<<ZZFLAGISDYADX)


// Bivalent entry point.
// self is a cyclic iterator
// we extract the pointer to the verb to be executed, advance the cycle pointer (.gerx) with wraparound, and call the verb
// passes inplacing through
static DF2(jtexeccyclicgerund){  // call is w,self or a,w,self
 // find the real self, valence-dependent
  F2PREFIP;ARGCHK1(w);
 I ismonad=AT(w)==0; self=ismonad?w:self;
 I nexttoexec=FAV(self)->localuse.lu1.gercut.cgerx; A vbtoexec=C(AAV(FAV(self)->fgh[2])[nexttoexec]); AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec!=0,EVDOMAIN); // get fn to exec
 ++nexttoexec; nexttoexec=AN(FAV(self)->fgh[2])==nexttoexec?0:nexttoexec; FAV(self)->localuse.lu1.gercut.cgerx=nexttoexec; // cyclically advance exec pointer
 w=ismonad?vbtoexec:w; A z=(*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
 if(unlikely(z==0)){jteformat(jt,vbtoexec,ismonad?0:a,w,0);}  // if error, we must format it here because cyclic gerund is invalid
 R z;
}

// similar, for executing m@.v on cells sequentially.  This for I selectors
static DF2(jtexecgerundcellI){  // call is w,self or a,w,self
 // find the real self, valence-dependent
 F2PREFIP;ARGCHK1(w);
 I ismonad=AT(w)==0; self=ismonad?w:self;  // set ismonad if call is w,self; update self ptr then
 I nexttoexec=FAV(self)->localuse.lu1.gercut.cgerx;  // index of cell we are working on
 I gerx=IAV(FAV(self)->fgh[1])[nexttoexec];  // selector for that cell
 gerx+=REPSGN(gerx)&AN(FAV(self)->fgh[2]);  // if negative, back up from end
 ASSERTGOTO(BETWEENO(gerx,0,AN(FAV(self)->fgh[2])),EVINDEX,errorwind);  // must be in range of # gerunds
 A vbtoexec=C(AAV(FAV(self)->fgh[2])[gerx]); AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec!=0,EVDOMAIN); // get fn to exec
 ++nexttoexec; FAV(self)->localuse.lu1.gercut.cgerx=nexttoexec; // advance to next cell
 w=ismonad?vbtoexec:w; A z=(*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
 if(unlikely(z==0)){jteformat(jt,vbtoexec,ismonad?0:a,w,0);}  // if error, we must format it here because cyclic gerund is invalid
 R z;
// in case of index error we have to call eformat right here, because our caller does not have the self block for the overall gerund.  The a and w values are unused except for valence.  We pass in all the selectors
errorwind:;  // here if there is an error in the result of calculating the selector.  We must send that result to eformat
  jteformat(jt,FAV(self)->localuse.lu0.gerundself,ismonad?0:a,w,FAV(self)->fgh[1]);
  RETF(0);
}
// This for B selectors
static DF2(jtexecgerundcellB){  // call is w,self or a,w,self
 // find the real self, valence-dependent
 F2PREFIP;ARGCHK1(w);
 I ismonad=AT(w)==0; self=ismonad?w:self;  // set ismonad if call is w,self; update self ptr then
 I nexttoexec=FAV(self)->localuse.lu1.gercut.cgerx;
 I gerx=BAV(FAV(self)->fgh[1])[nexttoexec];
 gerx+=REPSGN(gerx)&AN(FAV(self)->fgh[2]);
 ASSERTGOTO(BETWEENO(gerx,0,AN(FAV(self)->fgh[2])),EVINDEX,errorwind);
 A vbtoexec=C(AAV(FAV(self)->fgh[2])[gerx]); AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec!=0,EVDOMAIN); // get fn to exec
 ++nexttoexec; FAV(self)->localuse.lu1.gercut.cgerx=nexttoexec;
 w=ismonad?vbtoexec:w; A z=(*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
 if(unlikely(z==0)){jteformat(jt,vbtoexec,ismonad?0:a,w,0);}  // if error, we must format it here because cyclic gerund can't
 R z;
errorwind:;  // here if there is an error in the result of calculating the selector.  We must send that result to eformat
  jteformat(jt,FAV(self)->localuse.lu0.gerundself,ismonad?0:a,w,FAV(self)->fgh[1]);
  RETF(0);
}

// w is a verb that refers to a cyclic gerund which is stored in h.
// z is a place to build a clone - 128 bytes on an I boundary
// Result is a clone that is used to hold which gerund is next to execute
// the gerund must not be empty
A jtcreatecycliciterator(J jt, A z, A w){
 // Create the (skeletal) clone, point it to come to the execution point, set the next-verb number (.gerx) to 0
 // AT is 0 to suppress eformat, as this 'verb' has invalid fgh
 ACFAUX(z,ACPERMANENT) AT(z)=0; FAV(z)->fgh[2]=FAV(w)->fgh[2]; FAV(z)->mr=FAV(w)->mr; FAV(z)->valencefns[0]=FAV(z)->valencefns[1]=jtexeccyclicgerund; FAV(z)->localuse.lu1.gercut.cgerx=0;
 FAV(z)->flag2=0; FAV(z)->id=CCYCITER;   // clear flags, and give this verb a proper id so it can be checked for
 if(MEMAUDIT&0xc)AFLAGFAUX(z,0)  // in debug, flags must be valid
 R z;
}
// Similar, but also install r, the list of gerund results that will select the verb to run.  Used when there are few cells.
// in this case gerx is the cell#, and will be used to step through the result selectors sequentially
static A jtcreategerunditerator(J jt, A z, A w, A r){  // z is result area, w is self for h@.g, r is selector list
 // Convert the selectors to integer/boolean
 if(!ISDENSETYPE(AT(r),(INT|B01)))RZ(r=cvt(INT,r));
 // Create the (skeletal) clone, point it to come to the execution point, set the next-verb number to 0
 // AT is 0 to suppress eformat, as this 'verb' has invalid fgh
 ACFAUX(z,ACPERMANENT) AT(z)=0; FAV(z)->fgh[2]=FAV(w)->fgh[2]; FAV(z)->fgh[1]=r; FAV(z)->mr=FAV(w)->mr;  // h->gerunds, g->values
 FAV(z)->valencefns[0]=FAV(z)->valencefns[1]=AT(r)&INT?jtexecgerundcellI:jtexecgerundcellB; FAV(z)->localuse.lu1.gercut.cgerx=0;
 FAV(z)->localuse.lu0.gerundself=w;  // save the self for m@.v so we can call eformat with it
 FAV(z)->flag2=0;
 if(MEMAUDIT&0xc)AFLAGFAUX(z,0)  // in debug, flags must be valid
 R z;
}

// w is a gerund whose max rank is r.  Result is a boxed array of VERBs, one for each gerund, if they are well formed
A jtfxeachv(J jt,I r,A w){A*wv,x,z,*zv;I n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(r>=AR(w),EVRANK);  // max rank allowed
 ASSERT(n!=0,EVLENGTH);  // gerund must not be empty
 ASSERT(BOX&AT(w),EVDOMAIN);   // must be boxed
 I zr=AR(w); GATV(z,BOX,n,AR(w),AS(w)); zv=AAVn(zr,z);  // allocate one box per gerund
 DO(n, RZ(zv[i]=x=incorp(fx(C(wv[i])))); ASSERT(VERB&AT(x),EVDOMAIN););   // create verb, verify it is a verb  No incorporation since it's not a noun
 R z;
}

// self blocks to pass into every and thence into jtfx.  AK holds the parm into jtfx
PRIM jtfxself[2]={
{{0,0,0,0,0,0,0},{{.valencefns={jtfx,0},.fgh={0,0,0},.localuse=0,.flag=0,.flag2=0,.lrr=0,.mr=0,.id=0,.lu2.lc=0}}} ,
{{1,0,0,0,0,0,0},{{.valencefns={jtfx,0},.fgh={0,0,0},.localuse=0,.flag=0,.flag2=0,.lrr=0,.mr=0,.id=0,.lu2.lc=0}}}
};

// run jtfx on each box in w, turning AR into an A block
// self is a parm passed through to jtfx, coming from jtfxself above.  if AK(self) is nonzero, we return nouns as is
// Result claims to be an array of boxes, but each box holds an A with possibly a function type
DF1(jtfxeach){RETF(every(w,self));}

static DF1(jtcon1){A h,*hv,*x,z;V*sv;
 sv=FAV(self); h=sv->fgh[2]; hv=AAV(h);
 I zr=AR(h); GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAVn(zr,z);
 DQ(AN(h), RZ(*x++=incorp(CALL1(FAV(C(*hv))->valencefns[0],  w,C(*hv)))); ++hv;);
 R jtopenforassembly(jt,z);
}

static DF2(jtcon2){A h,*hv,*x,z;V*sv;
 sv=FAV(self); h=sv->fgh[2]; hv=AAV(h);
 I zr=AR(h); GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAVn(zr,z);
 DQ(AN(h), RZ(*x++=incorp(CALL2(FAV(C(*hv))->valencefns[1],a,w,C(*hv)))); ++hv;);
 R jtopenforassembly(jt,z);
}

// u`:3 insert 
static DF1(jtinsert){A hs,*hv,z;I hfx,j,m,n;A *old;
 ARGCHK1(w);
 SETIC(w,n); j=n-1; hs=FAV(self)->fgh[2]; m=AN(hs); hfx=j%m; hv=AAV(hs);  // m cannot be 0
 if(!n)R dfv1(z,w,iden(C(*hv)));
 RZ(z=from(num(-1),w));
 old=jt->tnextpushp;
 --m; DQ(n-1, --j; --hfx; hfx=(hfx<0)?m:hfx; RZ(z=CALL2(FAV(hv[hfx])->valencefns[1],from(sc(j),w),z,hv[hfx])); z=gc(z,old);)
 RETF(z);
}

// u`:m
F2(jtevger){F2PREFIP;A hs;I k;
 ARGCHK2(a,w);
 STACKCHKOFL  // because this is an execution, we must check the stack to avoid self-executions
 RE(k=i0(w)); 
 if(k==GTRAIN)R exg(a);
 RZ(hs=fxeachv(RMAX,a));
 ASSERT(BETWEENC(k^1,GAPPEND^1,GINSERT^1),EVDOMAIN)   // require valid type
 AF f1=k==GAPPEND?(AF)jtcon1:(AF)jtinsert; AF f2=k==GAPPEND?(AF)jtcon2:(AF)jtvalenceerr; I r2=k==GAPPEND?RMAX:0;  // variable parts
 R fdef(0,CGRCO,VERB, f1,f2,   a,w,hs, VGERL, RMAX,r2,r2);
}

// u`v.  Allow append-in-place to m
F2(jttie){F2PREFIP;ARGCHK2(a,w); R jtapip(jtinplace,VERB&AT(a)?arep(a):a,VERB&AT(w)?arep(w):w);}


// m@.:v y.  Execute the verbs at high rank if the operands are large
// Bivalent entry point: called as (jt,w,self,self) or (jt,a,w,self)
static DF2(jtcasei12){A vres,z;I gerit[128/SZI],ZZFLAGWORD;
 F1PREFIP; ARGCHK2(a,w);
 PROLOG(997);
 // see if we were called as monad or dyad.  If monad, fix up w and self
 ZZFLAGWORD=EPMONAD?ZZFLAGINITSTATE|ZZFLAGWILLBEOPENED|ZZFLAGCOUNTITEMS:ZZFLAGINITSTATE|ZZFLAGWILLBEOPENED|ZZFLAGCOUNTITEMS|ZZFLAGISDYAD;  // we collect the results on the cells, but we don't assemble into a result.  To signal this, we force BOXATOP and set WILLBEOPENED
 jtinplace=(J)((I)jtinplace&(a==w?-4:-1));  // Don't allow inplacing if a==w dyad
// obsolete  self=AT(w)&VERB?w:self;
 w=EPMONAD?a:w;  // if monad, a==w
 I wr=AR(w); I ar=AR(a); I mr=MAX(wr,ar);    // ranks, and max rank
 // Execute v at infinite rank
 vres=FAV(self)->fgh[1];   // temp: verb to execute
 RZ(vres=CALL2(FAV(vres)->valencefns[ZZFLAGWORD>>ZZFLAGISDYADX],a,ZZFLAGWORD&ZZFLAGISDYAD?w:vres,vres));  // execute v at infinite rank, not inplace
 // Now vres is the array of selectors into the gerund.  There should be one for each cell of the input.  Check that
 // The rank of the result must be no more than the larger rank
 I vr=AR(vres);  // rank of selectors
 if(vr!=0){
  // The verb is not applied at maximum rank.  Do the full processing
  // grade the results of v and then sort them into order
  // for each unique value, collect the cells for that value and apply the verb to them (could use f/. code?)
  // reorder the results
  ASSERTGOTO(mr>=vr,EVRANK,errorwind);  // there must not be more selectors than atoms in the larger arg
  I disagree; TESTDISAGREE(disagree,AS(vres),AS(w),MIN(vr,wr));
  ASSERTGOTO(!disagree,EVLENGTH,errorwind)  // shapes must match
  if(ZZFLAGWORD&ZZFLAGISDYAD){TESTDISAGREE(disagree,AS(vres),AS(a),MIN(vr,ar));  ASSERTGOTO(!disagree,EVLENGTH,errorwind)}  // if dyad, check both
  I ncells; PROD(ncells,AR(vres),AS(vres));  // number of result cells
  I nar=AN(FAV(self)->fgh[2]);  // number of ARs in the gerund
  wr-=vr; wr=wr+vr?wr:IMIN;  // now wr is the rank of a cell of w (negative if repetition required in w), or IMIN if w was an atom
  ar-=vr; ar=ar+vr?ar:IMIN;  // same for a
  A zz=0;  // output area
  // We can run our trimmed-down rank loop here if there is no repetition of cells, OR if one atom is repeated over the entire other argument;
  // that is, if each cell-rank is positive, 0 or IMIN.  We also make sure there are enough results to make the processing worthwhile
  // Since the larger cell-rank must not be negative, we can look only at the smaller
  mr=MIN(ar,wr); // the smaller
  if(((-mr|~mr)&SGNIFDENSE(AT(w))&(2*nar-ncells))<0){  // if mr is IMIN or nonneg, and there are enough results compared to # gerunds, reduce # verb executions.  Sparse doesn't do this.
   // Make sure the results are integer or boolean
   if(!ISDENSETYPE(AT(vres),(B01|INT)))RZ(vres=cvt(INT,vres));
   // grade the results, as a list
   A gradepm; RZ(gradepm=grade1(ravel(vres)));
   // decide how many result boxes to allocate based on min/max result - it's a byte or an int
   I bmsk=-((AT(vres)>>(INTX-1))&2)+1;  // mask for getting valid bits from vres
   I bsiz=AT(vres)&B01?1:SZI;  // size of element of vres
   I minx=*(I*)(CAV(vres)+IAV(gradepm)[0]*bsiz)&bmsk;   // smallest result
   I maxx=*(I*)(CAV(vres)+IAV(gradepm)[AN(gradepm)-1]*bsiz)&bmsk;  // largest result
   ASSERTGOTO(minx>=-nar&&maxx<nar,EVINDEX,errorwind);  // verify that the results are in range
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
   I ak,wk=bpnoun(AT(w)); wk&=REPSGN(~wr);  // size of atom of w, but 0 if w is an atom (so we don't advance)
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
    ak=bpnoun(AT(a)); ak&=REPSGN(~ar);  // size of atom of a, but 0 if a is an atom (so we don't advance)
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
    NOUNROLL while(srchlo<srchhi-1){  // search until we have found the last point in the block (at srchlo).  srchhi may not be a valid index
     I probex=(srchlo+srchhi)>>1;  // ignore overflow, can't happen
     I proberes=*(I*)(CAV(vres)+gradev[probex]*bsiz)&bmsk;  // fetch next look
     srchlo=proberes==currres?probex:srchlo; srchhi=proberes==currres?srchhi:probex;  // move the correct end 
    }
    currres+=REPSGN(currres)&nar;  // convert currres to nonnegative form
    // fill in the virtual block for this block
    if(ZZFLAGWORD&ZZFLAGARRAYW){  // if w is not a repeated atom...
     AS(virtw)[0]=srchhi-blkstart; AN(virtw)=(srchhi-blkstart)*wck;
     ACRESET(virtw,ACUC1 + SGNONLYIF(state,ZZFLAGVIRTWINPLACEX))   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block (not if atom, which is never inplaceable)
    }
    if(ZZFLAGWORD&ZZFLAGARRAYA){  // if a is not a repeated atom...
     AS(virta)[0]=srchhi-blkstart; AN(virta)=AS(virta)[0]*ack;
     ACRESET(virta,ACUC1 + SGNONLYIF(state,ZZFLAGVIRTAINPLACEX))   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block (not if atom, which is never inplaceable)
    }
   //  pull the function for the value, execute on the value with forced attributes (ASSUMEBOXATOP, WILLBEOPENED, COUNTITEMS)
   // take inplaceability from the selected verb always - we have made the cells inplaceable if possible
    A fs=C(AAV(FAV(self)->fgh[2])[currres]);  // fetch the gerund to execute
    RZ(z=(FAV(fs)->valencefns[ZZFLAGWORD>>ZZFLAGISDYADX])((J)((I)jt+(REPSGN(SGNIF(FAV(fs)->flag,(ZZFLAGWORD>>ZZFLAGISDYADX)+VJTFLGOK1X))&(((I)2<<(ZZFLAGWORD>>ZZFLAGISDYADX))-1))),
     virta,ZZFLAGWORD&ZZFLAGISDYAD?virtw:fs,fs));  // execute gerund at infinite rank, inplace from VJTFLGOK1/2 depending on valence

#define ZZBODY  // assemble results
#define ZZASSUMEBOXATOP 1
#include "result.h"

    //  verify input itemcount is a prefix of result-shape
    ASSERT(AR(z)&&AS(z)[0]==srchhi-blkstart,EVRANK);  // must have the right item count
    blkstart=srchhi;  // advance for next block
    AK(virtw)+=AN(virtw)*wk;  // advance over bytes just processed (but don't move if argument is an atom)
    AK(virta)+=AN(virta)*ak;  // for a too
   }while(blkstart<graden);  // loop till we have processed all results

   // We have executed v on all the cells.  Put the results into the right order
   I nblkscreated=zzboxp-AAV(zz);
   if(ZZFLAGWORD&ZZFLAGCOUNTITEMS){
    // The items of the results had a uniform size and type.  We can skip collecting them together, and just move the items from the boxes to their
    // final resting place.  They can't be pyxes
    // get the size of each item
    I itemk; PROD(itemk,AR(AAV(zz)[0])-1,AS(AAV(zz)[0])+1); I zk=itemk<<bplg(AT(AAV(zz)[0]));  // get # atoms/cell, #bytes/cell
    // allocate result area (could be inplace; but the waste of memory in sort/from is more important)
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
     }else{DQ(blkitemct, MC(zv+zk* *agrade,resptr,zk); resptr+=zk; ++agrade;)  // copy blocks
     }
     ++blkx;  // advance to next input block
    }while(blkx!=nblkscreated);  // loop till all blocks processed
    zz=z;  // all moved -- switch to the shuffled output
   }else{
    // inhomogeneous types or shapes.  Instantiate the entire result and then shuffle it into place
    // first, install the actual number of boxes of result, which might differ from the calculated maximum
    AN(zz)=AS(zz)[0]=nblkscreated;  // # valid boxes
    zz=ev12(gradepm,zz,"(/:~   >@:;@:((<\"_1)&.>))");  // (> ; <"_1&.> zz) /: gradepm
    if(unlikely(zz==0&&jt->jerr==EVDOMAIN))goto assemblyerror;   // domain error here must be incompatible types
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
assemblyerror: RESETERR jt->etxinfo->asseminfo.assemframelen=0; jt->jerr=EVASSEMBLY;  // framelen=0 means we can't localize it; switch error to assembly.  Will be eformatted up the line
 }else{
  // There was only 1 cell in the result.  Apply the selected verb, inplace if possible
  I vx=i0(vres); RE(0);  // fetch index of gerund
  vx+=REPSGN(vx)&AN(FAV(self)->fgh[2]); ASSERTGOTO(BETWEENO(vx,0,AN(FAV(self)->fgh[2])),EVINDEX,errorwind);
  A ger=C(AAV(FAV(self)->fgh[2])[vx]);  // the selected gerund
  R (FAV(ger)->valencefns[state>>ZZFLAGISDYADX])((J)((REPSGN(SGNIF(FAV(ger)->flag,(state>>ZZFLAGISDYADX)+VJTFLGOK1X))|~JTFLAGMSK)&(I)jtinplace),a,state&ZZFLAGISDYAD?w:ger,ger);  // inplace if the verb can handle it
 }
errorwind:;  // here if there is an error in the result of calculating the selector.  We must send that result to eformat
  jteformat(jt,self,ZZFLAGWORD&ZZFLAGISDYAD?a:0,w,vres);
  RETF(0);
}

// @.n
static F2(jtgerfrom){A*av,*v,z;I n;
 ARGCHK2(a,w);  /* 1==AR(w)&&BOX&AT(w) */
 ASSERT(1>=AR(a),EVRANK);
 if(NUMERIC&AT(a))R from(a,w);
 else{
  ASSERT(BOX&AT(a),EVDOMAIN);
  n=AN(a); av=AAV(a); 
  GATV0(z,BOX,n,1); v=AAV1(z);
  DO(n, RZ(*v++=incorp(gerfrom(C(av[i]),w))););
  R z;
}}

// initial handler for u@.v, to create resulting verb
F2(jtagendai){F2PREFIP;I flag;
 ARGCHK2(a,w)
 ASSERT(NOUN&AT(a),EVDOMAIN);  // u must always be a gerund
 if(unlikely(NOUN&AT(w)))R exg(gerfrom(w,a));  // noun form, as before
 // verb v.  Create a "BOX" type holding the verb form of each gerund
 A avb; RZ(avb = incorp(fxeachv(1L,a)));
  // Calculate ASGSAFE from all of the verbs (both a and w), provided the user can handle it
 flag = VASGSAFE&FAV(w)->flag; A* avbv = AAV(avb); DQ(AN(avb), flag &= FAV(*avbv)->flag; ++avbv;);  // Don't increment inside FAV!
 R fdef(0,CATDOT,VERB, jtcasei12,jtcasei12, a,w,avb, flag+((VGERL|VJTFLGOK1|VJTFLGOK2)|FAV(ds(CATDOT))->flag), RMAX, RMAX, RMAX);
}



#if 0 // obsolete 

static DF1(jtgf1){A z,h=FAV(self)->fgh[2]; R dfv1(z,  w,C(AAV(h)[0]));}
static DF2(jtgf2){A z,h=FAV(self)->fgh[2]; R dfv2(z,a,w,C(AAV(h)[0]));}

A jtvger2(J jt,C id,A a,A w){A h,*hv,x;V*v;
 RZ(x=a?a:w);
 ASSERT(2==AN(x),EVLENGTH);
 RZ(h=fxeachv(1L,x)); hv=AAV(h); v=VAV(C(*hv));
 R fdef(0,id,VERB, jtgf1,jtgf2, x,a?w:0L, h, VGERL, (I)v->mr,lrv(v),rrv(v));
}    /* verify and define 2-element gerund */
#endif
