/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: L: and S:                                                 */

#include "j.h"


static A jtlev1(J jt,A w,A self){A fs;
 RZ(w&&self);
// obsolete  if(jt->lmon>=level(w)){fs=VAV(self)->f; R CALL1(VAV(fs)->f1,w,fs);} else R every(w,self,jtlev1);
 if(levelle(w,jt->lmon)){fs=VAV(self)->f; R CALL1(VAV(fs)->f1,w,fs);} else R every(w,self,jtlev1);
}

static A jtlev2(J jt,A a,A w,A self){A fs;
 RZ(a&&w&&self);
// obsolete  switch((jt->lleft>=level(a)?2:0)+(jt->lright>=level(w))){
 I aready=levelle(a,jt->lleft); I wready=levelle(w,jt->lright);  // see if args are at the needed level
 // If both args are ready to process, do so.  Otherwise, drop down a level and try again.  If one arg is ready but the other isn't,
 // add a boxing level before we drop down so that when it is processed it will be the first level at which it became active.  This result could
 // be achieved by altering the left/right levels, but Roger did it this way.
 if(aready&wready){fs=FAV(self)->f; R CALL2(FAV(fs)->f2,a,w,fs);
 }else{R every2(aready?box(a):a,wready?box(w):w,self,jtlev2);}
 // We do this with the if statement rather than a computed branch in the hope that the CPU can detect patterns in the conditions.
 // There may be a structure in the user's data that could be detected for branch prediction.
// obsolete  switch(2*levelle(a,jt->lleft)+levelle(w,jt->lright)){
// obsolete   case 0:  R every2(a,     w,     self,jtlev2);
// obsolete   case 1:  R every2(a,     box(w),self,jtlev2);
// obsolete   case 2:  R every2(box(a),w,     self,jtlev2);
// obsolete   default: fs=FAV(self)->f; R CALL2(FAV(fs)->f2,a,w,fs);
// obsolete  }
}

static I jtefflev(J jt,I j,A h,A x){I n,t; n=*(j+AV(h)); R n>=0?n:(t=level(x),MAX(0,n+t));}

static DF1(jtlcapco1){A z;I m;V*v=VAV(self); 
 RZ(w); 
 m=jt->lmon; jt->lmon=efflev(0L,v->h,w); 
 z=lev1(w,self);
 jt->lmon=m;
 RETF(z);
}

static DF2(jtlcapco2){A z;I l,r;V*v=VAV(self);
 RZ(a&&w);
 l=jt->lleft;  jt->lleft =efflev(1L,v->h,a);
 r=jt->lright; jt->lright=efflev(2L,v->h,w);
 z=lev2(a,w,self);
 jt->lleft =l;
 jt->lright=r;
 RETF(z);
}

// Result logger for S:   w is the result; we add it to jt->sca, reallocating as needed
// result is 0 for error or a harmless small result (0) which will be collected at higher levels and discarded
static F1(jtscfn){
 RZ(w);
// obsolete  if(jt->scn==AN(jt->sca)){RZ(jt->sca=ext(1,jt->sca)); jt->scv=AV(jt->sca);}
// obsolete  jt->scv[jt->scn++]=(I)rifvs(w);
 if(AS(jt->sca)[0]==AN(jt->sca)){I n=AN(jt->sca); RZ(jt->sca=ext(1,jt->sca)); AS(jt->sca)[0]=n;}  // if current buffer is full, reallocate.  ext resets AS
 AAV(jt->sca)[AS(jt->sca)[0]++]=rifvs(w);  // copy in new result pointer
 R zero;
}

static A jtlevs1(J jt,A w,A self){A fs;
 RZ(w&&self);
// obsolete  if(jt->lmon>=level(w)){fs=VAV(self)->f; R scfn(CALL1(VAV(fs)->f1,w,fs));}else R every(w,self,jtlevs1);
 if(levelle(w,jt->lmon)){fs=VAV(self)->f; RZ(scfn(CALL1(VAV(fs)->f1,w,fs)));}else RZ(every(w,self,jtlevs1));
 R zero;
}

static A jtlevs2(J jt,A a,A w,A self){A fs;
 RZ(a&&w&&self);
 I aready=levelle(a,jt->lleft); I wready=levelle(w,jt->lright);  // see if args are at the needed level
 // If both args are ready to process, do so.  Otherwise, drop down a level and try again.  If one arg is ready but the other isn't,
 // add a boxing level before we drop down so that when it is processed it will be the first level at which it became active.  This result could
 // be achieved by altering the left/right levels, but Roger did it this way.
 if(aready&wready){fs=FAV(self)->f; RZ(scfn(CALL2(FAV(fs)->f2,a,w,fs)));
 }else{RZ(every2(aready?box(a):a,wready?box(w):w,self,jtlevs2));}
// obsolete  switch((jt->lleft>=level(a)?2:0)+(jt->lright>=level(w))){
// obsolete  switch(2*levelle(a,jt->lleft)+levelle(w,jt->lright)){
// obsolete  case 0:  RZ(every2(a,     w,     self,jtlevs2)); break;
// obsolete  case 1:  RZ(every2(a,     box(w),self,jtlevs2)); break;
// obsolete  case 2:  RZ(every2(box(a),w,     self,jtlevs2)); break;
// obsolete  default: fs=FAV(self)->f; RZ(scfn(CALL2(FAV(fs)->f2,a,w,fs))); break;
// obsolete  }
  R zero;
}

static DF1(jtscapco1){PROLOG(555);A x,z=0;I m;V*v=VAV(self);
 RZ(w);
 A scastk=jt->sca; m=jt->lmon; jt->lmon=efflev(0L,v->h,w);  // stack level of any executing L:/S:, get level to use
// obsolete  GAT(x,INT,100,1,0); jt->scv=AV(x); jt->sca=x; jt->scn=0;   // allocate place to save results. this will hold boxes, but it is allocated as INTs so it won't be freed on error
 GAT(x,INT,100,1,0); jt->sca=x; AS(x)[0]=0;    // allocate place to save results. this will hold boxes, but it is allocated as INTs so it won't be freed on error.  AS[0] holds # valid results
 // jt->sca will be used to collect results during the execution of the verb.  Since we don't know how many results there will be, jt->sca may be extended
 // in the middle of processing some other verb, and that verb might EPILOG and free the new buffer allocated by the extension.  Thus, we have to ra() the later buffers, and the easiest way to handle
 // things is to ra() the first one too.  When we fa() at the end we may be freeing a different buffer, but that's OK since all have been raised.
 ras(jt->sca);
 x=levs1(w,self);  // execute the verb.  DO NOT exit on error
// obsolete  if(x)z=ope(vec(BOX,jt->scn,jt->scv)); // if no error, copy the valid results to a boxed vector  (could just change type of box without reallocating); open to produce final result
 if(x){AT(jt->sca)=BOX; AN(jt->sca)=AS(jt->sca)[0]; z=ope(jt->sca); AT(jt->sca)=INT;} // if no error, turn the extendable list into a list of boxes (fixing AN), and open it
 fa(jt->sca);  // matcb the ra(), but not necessarily on the same block
 jt->lmon=m; jt->sca=scastk;   // pop level stack
// obsolete  RETF(z);  // should EPILOG
 EPILOG(z);
}

static DF2(jtscapco2){PROLOG(556);A x,z=0;I l,r;V*v=VAV(self); 
 RZ(a&&w); 
 A scastk=jt->sca; l=jt->lleft;  jt->lleft =efflev(1L,v->h,a);
 r=jt->lright; jt->lright=efflev(2L,v->h,w);
 GAT(x,INT,100,1,0); jt->sca=x; AS(x)[0]=0;    // allocate place to save results. this will hold boxes, but it is allocated as INTs so it won't be freed on error.  AS[0]=# valid results
 ras(jt->sca); 
 x=levs2(a,w,self);
// obsolete  if(x)z=ope(vec(BOX,jt->scn,jt->scv)); 
 if(x){AT(jt->sca)=BOX; AN(jt->sca)=AS(jt->sca)[0]; z=ope(jt->sca); AT(jt->sca)=INT;} // if no error, turn the extendable list into a list of boxes (fixing AN), and open it
 fa(jt->sca); 
 jt->lleft =l; jt->lright=r; jt->sca=scastk;
 EPILOG(z);
}


static A jtlsub(J jt,C id,A a,A w){A h,t;B b=id==CLCAPCO;I*hv,n,*v;
 RZ(a&&w);
 ASSERT(VERB&AT(a)&&NOUN&AT(w),EVDOMAIN);
 n=AN(w); 
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(0<n&&n<4,EVLENGTH);
 RZ(t=vib(w)); v=AV(t);
 GAT(h,INT,3,1,0); hv=AV(h);  // save levels in h
 hv[0]=v[2==n]; hv[1]=v[3==n]; hv[2]=v[n-1];  // monad, left, right
 R fdef(0,id,VERB, b?jtlcapco1:jtscapco1,b?jtlcapco2:jtscapco2, a,w,h, VFLAGNONE, RMAX,RMAX,RMAX);
}

F2(jtlcapco){R lsub(CLCAPCO,a,w);}
F2(jtscapco){R lsub(CSCAPCO,a,w);}

