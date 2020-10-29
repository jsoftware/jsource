/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: L: and S:                                                 */

#include "j.h"


// execution of L:n .  self is suitable to pass to every, i. e. describes the u L: v node and has valencefns[0] pointing to here.  It is on the C stack.
// AT(self) is the trigger level (the original n)
// AM(self) is the block for u
static DF1(jtlev1){
 ARGCHK1(w);  // self is never 0
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[0];  // fetch verb and routine for leaf nodes.  Do it early
 if(levelle(w,AT(self))){R CALL1(fsf,w,fs);} else{STACKCHKOFL R every(w,self);}  // since this recurs, check stack
}

// Like monad, but AT(self) is left trigger level, AC(self) is the right trigger level 
static A jtlev2(J jt,A a,A w,A self){
 ARGCHK2(a,w);
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[1];  // fetch verb and routine for leaf nodes.  Do it early
 I aready=levelle(a,AT(self)); I wready=levelle(w,AC(self));  // see if args are at the needed level
 // If both args are ready to process, do so.  Otherwise, drop down a level and try again.  If one arg is ready but the other isn't,
 // add a boxing level before we drop down so that when it is processed it will be the first level at which it became active.  This result could
 // be achieved by altering the left/right levels, but Roger did it this way.
 if(aready&wready){R CALL2(fsf,a,w,fs);
 }else{STACKCHKOFL R every2(aready?box(a):a,wready?box(w):w,self);}  // since this recurs, check stack
 // We do this with the if statement rather than a computed branch in the hope that the CPU can detect patterns in the conditions.
 // There may be a structure in the user's data that could be detected for branch prediction.
}

static I jtefflev(J jt,I j,A h,A x){I n,t; n=*(j+AV(h)); R n>=0?n:(t=level(x),MAX(0,n+t));}

// execution of u L: n y.  Create the self to send to the recursion routine
// L: and S: will be rarely used on pristine blocks, which be definition have all DIRECT contents & would thus be
// better served by &.> .  Thus, we just mark the inputs as non-pristinable.
static DF1(jtlcapco1){A z;V*v=FAV(self); 
 ARGCHK1(w);
 PRISTCLR(w)

 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[0]=jtlev1;  // fill in function pointer
 AT(recurself)=efflev(0L,v->fgh[2],w);  // fill in the trigger level
 FAV(recurself)->flag=VFLAGNONE;  // fill in the inplaceability flags
 RETF(lev1(w,recurself));
}

static DF2(jtlcapco2){A z;V*v=FAV(self);
 ARGCHK2(a,w);
 PRISTCLR(w) PRISTCLRNODCL(a)
 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[1]=jtlev2;  // fill in function pointer
 AT(recurself)=efflev(1L,v->fgh[2],a); AC(recurself)=efflev(2L,v->fgh[2],w);  // fill in the trigger levels
 FAV(recurself)->flag=VFLAGNONE;  // fill in the inplaceability flags
 RETF(lev2(a,w,recurself));
}

// Result logger for S:   w is the result; we add it to AK(self), reallocating as needed
// result is 0 for error or a harmless small result (0) which will be collected at higher levels and discarded
static DF1(jtscfn){
 ARGCHK1(w);
 if(AS(AKASA(self))[0]==AN(AKASA(self))){I n=AN(AKASA(self)); RZ(AKASA(self)=ext(1,AKASA(self))); AS(AKASA(self))[0]=n;}  // if current buffer is full, reallocate.  ext resets AS
 AAV(AKASA(self))[AS(AKASA(self))[0]++]=incorp(w);  // copy in new result pointer
 R num(0);  // harmless good return
}

// u S: n - like L: except for calling the logger
static DF1(jtlevs1){
 ARGCHK1(w);  // self is never 0
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[0];  // fetch verb and routine for leaf nodes.  Do it early
 if(levelle(w,AT(self))){RZ(scfn(CALL1(fsf,w,fs),self));} else{STACKCHKOFL RZ(every(w,self));}  // since this recurs, check stack
 R num(0);
}

static DF2(jtlevs2){
 ARGCHK2(a,w);
 A fs=(A)AM(self); AF fsf=FAV(fs)->valencefns[1];  // fetch verb and routine for leaf nodes.  Do it early
 I aready=levelle(a,AT(self)); I wready=levelle(w,AC(self));  // see if args are at the needed level
 // If both args are ready to process, do so.  Otherwise, drop down a level and try again.  If one arg is ready but the other isn't,
 // add a boxing level before we drop down so that when it is processed it will be the first level at which it became active.  This result could
 // be achieved by altering the left/right levels, but Roger did it this way.
 if(aready&wready){RZ(scfn(CALL2(fsf,a,w,fs),self));
 }else{STACKCHKOFL RZ(every2(aready?box(a):a,wready?box(w):w,self));}  // since this recurs, check stack
 // We do this with the if statement rather than a computed branch in the hope that the CPU can detect patterns in the conditions.
 // There may be a structure in the user's data that could be detected for branch prediction.
  R num(0);
}

static DF1(jtscapco1){PROLOG(555);A x,z=0;I m;V*v=FAV(self);
 ARGCHK1(w);
 PRISTCLR(w)
 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[0]=jtlevs1;  // fill in function pointer
 AT(recurself)=efflev(0L,v->fgh[2],w);  // fill in the trigger level
 FAV(recurself)->flag=VFLAGNONE;  // fill in the inplaceability flags
 GAT0(x,INT,54,1); AKASA(recurself)=x; AS(x)[0]=0;    // allocate place to save results & fill into self. this will hold boxes, but it is allocated as INTs so it won't be freed on error.  AS[0] holds # valid results
 // jt->sca will be used to collect results during the execution of the verb.  Since we don't know how many results there will be, jt->sca may be extended
 // in the middle of processing some other verb, and that verb might EPILOG and free the new buffer allocated by the extension.  Thus, we have to ra() the later buffers, and the easiest way to handle
 // things is to ra() the first one too.  When we fa() at the end we may be freeing a different buffer, but that's OK since all have been raised.
 ras(AKASA(recurself));
 x=levs1(w,recurself);
 if(x){AT(AKASA(recurself))=BOX; AN(AKASA(recurself))=AS(AKASA(recurself))[0]; z=ope(AKASA(recurself)); AT(AKASA(recurself))=INT;} // if no error, turn the extendable list into a list of boxes (fixing AN), and open it
 fa(AKASA(recurself));  // match the ra(), but not necessarily on the same block
 // always returns non-pristine
 EPILOG(z);
}

static DF2(jtscapco2){PROLOG(556);A x,z=0;V*v=FAV(self); 
 ARGCHK2(a,w);
 PRISTCLR(w) PRISTCLRNODCL(a)
 PRIM shdr; A recurself=(A)&shdr;  // allocate the block we will recur with
 AM(recurself)=(I)v->fgh[0];  // fill in the pointer to u
 FAV(recurself)->valencefns[1]=jtlevs2;  // fill in function pointer
 AT(recurself)=efflev(1L,v->fgh[2],a); AC(recurself)=efflev(2L,v->fgh[2],w);  // fill in the trigger levels
 FAV(recurself)->flag=VFLAGNONE;  // fill in the inplaceability flags
 GAT0(x,INT,54,1); AKASA(recurself)=x; AS(x)[0]=0;    // allocate place to save results & fill into self. this will hold boxes, but it is allocated as INTs so it won't be freed on error.  AS[0] holds # valid results
 // jt->sca will be used to collect results during the execution of the verb.  Since we don't know how many results there will be, jt->sca may be extended
 // in the middle of processing some other verb, and that verb might EPILOG and free the new buffer allocated by the extension.  Thus, we have to ra() the later buffers, and the easiest way to handle
 // things is to ra() the first one too.  When we fa() at the end we may be freeing a different buffer, but that's OK since all have been raised.
 ras(AKASA(recurself));
 x=levs2(a,w,recurself);
 if(x){AT(AKASA(recurself))=BOX; AN(AKASA(recurself))=AS(AKASA(recurself))[0]; z=ope(AKASA(recurself)); AT(AKASA(recurself))=INT;} // if no error, turn the extendable list into a list of boxes (fixing AN), and open it
 fa(AKASA(recurself));  // match the ra(), but not necessarily on the same block
 // always returns non-pristine
 EPILOG(z);
}


static A jtlsub(J jt,C id,A a,A w){A h,t;B b=id==CLCAPCO;I*hv,n,*v;
 ARGCHK2(a,w);
 ASSERT((SGNIF(AT(a),VERBX)&-(AT(w)&NOUN))<0,EVDOMAIN);
 n=AN(w); 
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(BETWEENO(n,1,4),EVLENGTH);
 RZ(t=vib(w)); v=AV(t);
 GAT0(h,INT,3,1); hv=AV(h);  // save levels in h
 hv[0]=v[2==n]; hv[1]=v[3==n]; hv[2]=v[n-1];  // monad, left, right
 R fdef(0,id,VERB, b?jtlcapco1:jtscapco1,b?jtlcapco2:jtscapco2, a,w,h, VFLAGNONE, RMAX,RMAX,RMAX);
}

F2(jtlcapco){R lsub(CLCAPCO,a,w);}
F2(jtscapco){R lsub(CSCAPCO,a,w);}

