/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: L: and S:                                                 */

#include "j.h"


static A jtlev1(J jt,A w,A self){A fs;
 RZ(w&&self);
 if(jt->lmon>=level(w)){fs=VAV(self)->f; R CALL1(VAV(fs)->f1,w,fs);} else R every(w,self,jtlev1);
}

static A jtlev2(J jt,A a,A w,A self){A fs;
 RZ(a&&w&&self);
 switch((jt->lleft>=level(a)?2:0)+(jt->lright>=level(w))){
  case 0:  R every2(a,     w,     self,jtlev2);
  case 1:  R every2(a,     box(w),self,jtlev2);
  case 2:  R every2(box(a),w,     self,jtlev2);
  default: fs=VAV(self)->f; R CALL2(VAV(fs)->f2,a,w,fs);
}}

static I jtefflev(J jt,I j,A h,A x){I n,t; n=*(j+AV(h)); R n>=0?n:(t=level(x),MAX(0,n+t));}

static DF1(jtlcapco1){A z;I m;V*v=VAV(self); 
 RZ(w); 
 m=jt->lmon; jt->lmon=efflev(0L,v->h,w); 
 z=lev1(w,self);
 jt->lmon=m;
 R z;
}

static DF2(jtlcapco2){A z;I l,r;V*v=VAV(self);
 RZ(a&&w);
 l=jt->lleft;  jt->lleft =efflev(1L,v->h,a);
 r=jt->lright; jt->lright=efflev(2L,v->h,w);
 z=lev2(a,w,self);
 jt->lleft =l;
 jt->lright=r;
 R z;
}


static F1(jtscfn){
 RZ(w);
 if(jt->scn==AN(jt->sca)){RZ(jt->sca=ext(1,jt->sca)); jt->scv=AV(jt->sca);}
 jt->scv[jt->scn++]=(I)w;
 R zero;
}

static A jtlevs1(J jt,A w,A self){A fs;
 RZ(w&&self);
 if(jt->lmon>=level(w)){fs=VAV(self)->f; R scfn(CALL1(VAV(fs)->f1,w,fs));}else R every(w,self,jtlevs1);
}

static A jtlevs2(J jt,A a,A w,A self){A fs;
 RZ(a&&w&&self);
 switch((jt->lleft>=level(a)?2:0)+(jt->lright>=level(w))){
  case 0:  R every2(a,     w,     self,jtlevs2);
  case 1:  R every2(a,     box(w),self,jtlevs2);
  case 2:  R every2(box(a),w,     self,jtlevs2);
  default: fs=VAV(self)->f; R scfn(CALL2(VAV(fs)->f2,a,w,fs));
}}

static DF1(jtscapco1){A x,z=0;I m;V*v=VAV(self);
 RZ(w); 
 m=jt->lmon; jt->lmon=efflev(0L,v->h,w);
 GAT(x,INT,100,1,0); jt->scv=AV(x); jt->sca=x; jt->scn=0; 
 ra(jt->sca);
 x=levs1(w,self);
 jt->lmon=m;
 if(x)z=ope(vec(BOX,jt->scn,jt->scv)); 
 fa(jt->sca); 
 R z;
}

static DF2(jtscapco2){A x,z=0;I l,r;V*v=VAV(self); 
 RZ(a&&w); 
 l=jt->lleft;  jt->lleft =efflev(1L,v->h,a);
 r=jt->lright; jt->lright=efflev(2L,v->h,w);
 GAT(x,INT,100,1,0); jt->scv=AV(x); jt->sca=x; jt->scn=0; 
 ra(jt->sca); 
 x=levs2(a,w,self);
 jt->lleft =l;
 jt->lright=r;
 if(x)z=ope(vec(BOX,jt->scn,jt->scv)); 
 fa(jt->sca); 
 R z;
}


static A jtlsub(J jt,C id,A a,A w){A h,t;B b=id==CLCAPCO;I*hv,n,*v;
 RZ(a&&w);
 ASSERT(VERB&AT(a)&&NOUN&AT(w),EVDOMAIN);
 n=AN(w); 
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(0<n&&n<4,EVLENGTH);
 RZ(t=vib(w)); v=AV(t);
 GAT(h,INT,3,1,0); hv=AV(h);
 hv[0]=v[2==n]; hv[1]=v[3==n]; hv[2]=v[n-1];
 R fdef(id,VERB, b?jtlcapco1:jtscapco1,b?jtlcapco2:jtscapco2, a,w,h, VFLAGNONE, RMAX,RMAX,RMAX);
}

F2(jtlcapco){R lsub(CLCAPCO,a,w);}
F2(jtscapco){R lsub(CSCAPCO,a,w);}

