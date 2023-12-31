/* Copyright (c) 1990-2023, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: L: and S:                                                 */

#include "j.h"



// Result logger for S:   w is the result; we add it (boxed) to AK(self), reallocating as needed
// result is 0 for error or a harmless small result (0) which will be collected at higher levels and discarded
static DF1(jtscfn){
 ARGCHK1(w);
 if(AS(AKASA(self))[0]==AN(AKASA(self))){I n=AN(AKASA(self)); RZ(AKASA(self)=ext(1,AKASA(self))); AS(AKASA(self))[0]=n;}  // if current buffer is full, reallocate.  ext resets AS
 AAV(AKASA(self))[AS(AKASA(self))[0]++]=incorp(w);  // copy in new result pointer
 R mtv;  // harmless good return, with no atoms to save copying in every
}


// recursive execution of L:n/S:n .  self is suitable to pass to every, i. e. describes the u L: v node and has valencefns[0] pointing to here.  It is on the C stack.
// id in self distinguishes L: from S: 
// AT(self) is the trigger level (the original n)
// AM(self) is the block for u
static DF1(jtlev1){
 ARGCHK1(w);  // self is never 0
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[0];  // fetch verb and routine for leaf nodes.  Do it early
 if(levelle(jt,w,FAV(self)->localuse.lslevels[2])){A z; RZ(z=CALL1(fsf,w,fs)); if(FAV(self)->id!=CSCAPCO)RETF(z); R scfn(z,self);} else{STACKCHKOFL R every(w,self);}  // since this recurs, check stack  scaf if inplaceable, could have a version of every that replaces boxes in place
}

// Like monad, but AT(self) is left trigger level, AC(self) is the right trigger level 
static A jtlev2(J jt,A a,A w,A self){
 ARGCHK2(a,w);
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[1];  // fetch verb and routine for leaf nodes.  Do it early
 I aready=levelle(jt,a,FAV(self)->localuse.lslevels[0]); I wready=levelle(jt,w,FAV(self)->localuse.lslevels[1]);  // see if args are at the needed level
 // If both args are ready to process, do so.  Otherwise, drop down a level and try again.  If one arg is ready but the other isn't,
 // add a boxing level before we drop down so that when it is processed it will be the first level at which it became active.  To avoid 
 if(aready&wready){A z; RZ(z=CALL2(fsf,a,w,fs)); if(FAV(self)->id!=CSCAPCO)RETF(z); R scfn(z,self);  // call fn; if L: use the result; if S: call the logger
 }else{
  STACKCHKOFL  // since this recurs, check stack
  // if an argument is at level, we will box it to protect it through the recursion.  Since the only thing that will be done with this
  // box is immediate opening, we do it in place with a faux block to avoid repeatedly allocating single boxes
  fauxblockINT(bfaux,1,0);  // atomic box location
  if(aready|wready){A bx; fauxBOX(bx,bfaux,1,0) AAV0(bx)[0]=aready?a:w; a=aready?bx:a; w=aready?w:bx;}  // Exactly one arg is ready; box the ready arg and change its pointer
// obsolete   R every2(aready?box(a):a,wready?box(w):w,self);}
  R every2(a,w,self);
 }
 // We do this with the if statement rather than a computed branch in the hope that the CPU can detect patterns in the conditions.
 // There may be a structure in the user's data that could be detected for branch prediction.
}

//  convert negative level to level relative to bottom of h.  n is specified level
static I jtefflev(J jt,I n,A x){I t; R n>=0?n:(t=level(jt,x),MAX(0,n+t));}
// obsolete  n=AV(h)[j]

#if 0  // obsolete
static DF1(jtlcapco1){A z;V*v=FAV(self);    // scaf should make bivalent
 ARGCHK1(w);
 PRISTCLR(w)

 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[0]=jtlev1;  // fill in function pointer
// obsolete  AT(recurself)=efflev(0L,v->fgh[2],w);  // fill in the trigger level
 AT(recurself)=efflev(FAV(self)->localuse.lu1.levelmonad,w);  // fill in the trigger level
 FAV(recurself)->id=v->id;  //  indicate L:/S:
 RETF(lev1(w,recurself));
}

static DF2(jtlcapco2){A z;V*v=FAV(self);
 ARGCHK2(a,w);
 PRISTCLR(w) PRISTCLRNODCL(a)
 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[1]=jtlev2;  // fill in function pointer
// obsolete  AT(recurself)=efflev(1L,v->fgh[2],a); ACFAUX(recurself,efflev(2L,v->fgh[2],w))  // fill in the trigger levels
 AT(recurself)=efflev(FAV(self)->localuse.lu0.leveldyad[0],a); ACFAUX(recurself,efflev(FAV(self)->localuse.lu0.leveldyad[1],w))  // fill in the trigger levels
 FAV(recurself)->flag=VFLAGNONE;  // fill in the inplaceability flags, indicate L:
 RETF(lev2(a,w,recurself));
}

static DF1(jtlscapco1){PROLOG(555);I m;V*v=FAV(self);  // scaf should combine w/ L:, using id from self
 ARGCHK1(w);
 PRISTCLR(w)
 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[0]=jtlev1;  // fill in function pointer
// obsolete  AT(recurself)=efflev(0L,v->fgh[2],w);  // fill in the trigger level
 AT(recurself)=efflev(FAV(self)->localuse.lu1.levelmonad,w);  // fill in the trigger level
 C type=v->id; FAV(recurself)->id=type; // type of call, L: or S:
 if(type==CSCAPCO){A x; GAT0(x,INT,54,1); ACINITZAP(x) AKASA(recurself)=x; AS(x)[0]=0;}    // for S:, allocate place to save results & fill into self. this will hold boxes, but it is allocated as INTs so it won't be freed on error.  AS[0] holds # valid results
 // recurself->kchain will be used to collect results during the execution of the verb.  Since we don't know how many results there will be, jt->sca may be extended
 // in the middle of processing some other verb, and that verb might EPILOG and free the new buffer allocated by the extension.  Thus, we have to ra() the later buffers, and the easiest way to handle
 // things is to zap the first one too.  When we fa() at the end we may be freeing a different buffer, but that's OK since all have been raised.
 A z=lev1(w,recurself);
 if(type!=CSCAPCO)R z;  // return if L:
 if(likely(z!=0)){
  z=AKASA(recurself); AT(z)=BOX; AN(z)=AS(z)[0]; z=jtopenforassembly(jt,z); AT(z)=INT; // if no error, turn the extendable list into a list of boxes (fixing AN), and open it
 }
 fa(AKASA(recurself));  // match the zap, but not necessarily on the same block
 // always returns non-pristine
 EPILOG(z);
}


// u S: n - like L: except for calling the logger  scaf should combine, using id from  self
static DF1(jtlevs1){
 ARGCHK1(w);  // self is never 0
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[0];  // fetch verb and routine for leaf nodes.  Do it early
 if(levelle(jt,w,AT(self))){RZ(scfn(CALL1(fsf,w,fs),self));} else{STACKCHKOFL RZ(every(w,self));}  // since this recurs, check stack
 R num(0);
}

static DF2(jtlevs2){
 ARGCHK2(a,w);
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[1];  // fetch verb and routine for leaf nodes.  Do it early
 I aready=levelle(jt,a,AT(self)); I wready=levelle(jt,w,AC(self));  // see if args are at the needed level
 // If both args are ready to process, do so.  Otherwise, drop down a level and try again.  If one arg is ready but the other isn't,
 // add a boxing level before we drop down so that when it is processed it will be the first level at which it became active.  This result could
 // be achieved by altering the left/right levels, but Roger did it this way.
 if(aready&wready){RZ(scfn(CALL2(fsf,a,w,fs),self));
 }else{STACKCHKOFL RZ(every2(aready?box(a):a,wready?box(w):w,self));}  // since this recurs, check stack
 // We do this with the if statement rather than a computed branch in the hope that the CPU can detect patterns in the conditions.
 // There may be a structure in the user's data that could be detected for branch prediction.
 R num(0);
}
#endif

// execution of u L:/S: n y.  Bivalent.  Create the self to send to the recursion routine
// L: and S: will be rarely used on pristine blocks, which be definition have all DIRECT contents & would thus be
// better served by &.> .  Thus, we just mark the inputs as non-pristinable.
static DF2(jtlscapco12){PROLOG(556);A z;
 ARGCHK2(a,w);
 I wt=AT(w); self=wt&VERB?w:self;  // find the verb
 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)FAV(self)->fgh[0];  // fill in the pointer to u
 AT(recurself)=VERB;  // must mark type so bivalent check works
 FAV(recurself)->flag=VFLAGNONE;  // inplacing not allowed by jtlev[12]
// obsolete  AT(recurself)=efflev(1L,v->fgh[2],a); ACFAUX(recurself,efflev(2L,v->fgh[2],w))  // fill in the trigger levels

 C type=FAV(self)->id; FAV(recurself)->id=type; // type of call, L: or S:
 // obsolete FAV(recurself)->flag=VFISSCO;  // fill in the inplaceability flags, indicate S:
 if(type==CSCAPCO){A x; GAT0(x,INT,54,1); ACINITZAP(x) AKASA(recurself)=x; AS(x)[0]=0;}    // for S:, allocate place to save results & fill into self. this will hold boxes, but it is allocated as INTs so it won't be freed on error.  AS[0] holds # valid results
 // recurself->kchain will be used to collect results during the execution of the verb.  Since we don't know how many results there will be, jt->sca may be extended
 // in the middle of processing some other verb, and that verb might EPILOG and free the new buffer allocated by the extension.  Thus, we have to ra() the later buffers, and the easiest way to handle
 // things is to zap the first one too.  When we fa() at the end we may be freeing a different buffer, but that's OK since all have been raised.

 if(wt&VERB){  // monad (use a to refer to the actual w)
  FAV(recurself)->localuse.lslevels[2]=efflev(FAV(self)->localuse.lslevels[2],a);  // fill in the trigger levels
  FAV(recurself)->valencefns[0]=jtlev1; PRISTCLR(a) z=lev1(a,recurself);  // set recursion pointer and pristinity and start recursion
 }else{  // dyad
  FAV(recurself)->localuse.lslevels[0]=efflev(FAV(self)->localuse.lslevels[0],a); FAV(recurself)->localuse.lslevels[1]=efflev(FAV(self)->localuse.lslevels[1],w);  // fill in the trigger levels
  FAV(recurself)->valencefns[1]=jtlev2; PRISTCLR(w) PRISTCLRNODCL(a) z=lev2(a,w,recurself);  // set recursion pointer and pristinity and start recursion
 }
 if(type!=CSCAPCO)R z;  // return if L:, fall through for S:

 if(likely(z!=0)){
  // S: succeeded.  Assemble the results.  First make the result array look boxed, open it, then change to INT to avoid frees
  A x=AKASA(recurself); AT(x)=BOX; AN(x)=AS(x)[0]; z=jtopenforassembly(jt,x); AT(x)=INT;
 }
 fa(AKASA(recurself));  // match the initial zap, but not necessarily on the same block
 EPILOG(z); // always returns non-pristine
}


static A jtlsub(J jt,C id,A a,A w){A h,t;I*hv,n,*v;
 ARGCHK2(a,w);
 ASSERT((SGNIF(AT(a),VERBX)&-(AT(w)&NOUN))<0,EVDOMAIN);
 n=AN(w); 
 ASSERT(1>=AR(w),EVRANK);  // atom or list
 ASSERT(BETWEENO(n,1,4),EVLENGTH);  // 1-3 items
 RZ(t=vib(w)); v=AV(t);  // must be integral.  Overwhemlingly, boolean atom
// obsolete  GAT0(h,INT,3,1); hv=AV(h);  // save levels in h should use private fields in verb
// obsolete  hv[0]=v[2==n]; hv[1]=v[3==n]; hv[2]=v[n-1];  // monad, left, right
// obsolete  R fdef(0,id,VERB, b?jtlcapco1:jtscapco1,b?jtlcapco2:jtscapco2, a,w,h, VFLAGNONE, RMAX,RMAX,RMAX);
 I4 d0=v[3==n]; d0=d0!=v[3==n]?0x7fffffff+SGNTO0(v[3==n]):d0;  // left dyad; if there is upper significance, use I4 IMIN/IMAX
 I4 d1=v[n-1]; d1=d1!=v[n-1]?0x7fffffff+SGNTO0(v[n-1]):d1;  // right dyad; if there is upper significance, use I4 IMIN/IMAX
 I4 m=v[2==n]; m=m!=v[2==n]?0x7fffffff+SGNTO0(v[2==n]):m;  // monad; if there is upper significance, use I4 IMIN/IMAX
 A z; fdefallo(z); fdeffillall(z,0,id,VERB, jtlscapco12,jtlscapco12, a,w,0, VFLAGNONE, RMAX,RMAX,RMAX,fffv->localuse.lslevels[0]=d0;fffv->localuse.lslevels[1]=d1,fffv->localuse.lslevels[2]=m);
 R z;

}

F2(jtlcapco){F2PREFIP;R lsub(CLCAPCO,a,w);}
F2(jtscapco){F2PREFIP;R lsub(CSCAPCO,a,w);}

