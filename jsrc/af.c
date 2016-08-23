/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Fix                                                            */

#include "j.h"


F1(jtunname){A x;V*v;
 RZ(w); 
 v=VAV(w);
 if(CTILDE==v->id&&!jt->glock&&!(VLOCK&v->flag)){x=v->f; if(NAME&AT(x))R symbrd(x);}
 R w;
}

static B jtselfq(J jt,A w){A hs,*u;V*v;
 RZ(w);
 if(AT(w)&NOUN+NAME)R 0;
 v=VAV(w);
 switch(v->id){
  case CSELF:  
   R 1;
  case CATDOT:
  case CGRCO:
   if(hs=v->h){u=AAV(hs); DO(AN(hs), if(selfq(u[i]))R 1;);}
   R 0;
  default:     
   if(v->f&&selfq(v->f))R 1;
   if(v->g&&selfq(v->g))R 1;
   if(v->h&&selfq(v->h))R 1;
 }
 R 0;
}    /* 1 iff w contains $: */

static F2(jtfixa){A aa,f,g,h,wf,x,y,z=w;V*v;
 RZ(a&&w);
 if(NOUN&AT(w)||VFIX&VAV(w)->flag)R w;
 v=VAV(w); f=v->f; g=v->g; h=v->h; wf=ds(v->id); aa=a==zero?num[3]:a;
 if(!(f||g))R w;
 switch(v->id){
  case CSLASH: 
   R df1(fixa(num[2],f),wf);
  case CSLDOT: case CBSLASH: case CBSDOT:
   R df1(fixa(one,f),wf);
  case CAT: case CATCO: case CCUT:
   R df2(fixa(one,f),fixa(aa,g),wf);
  case CAMP: case CAMPCO: case CUNDER: case CUNDCO:
   R df2(fixa(aa,f),fixa(one,g),wf);
  case CCOLON:
   // handles only v : v; n : n had VFIX set & never gets here
   R df2(fixa(one,f),fixa(num[2],g),wf);
  case CADVF:
   R hook(fixa(num[3],f),fixa(num[3],g));
  case CHOOK:
   R hook(fixa(num[2],f),fixa(one,g));
  case CFORK:
   f=fixa(aa,f); g=fixa(num[ID(f)==CCAP?1:2],g); h=fixa(aa,h); R folk(f,g,h);
  case CATDOT:
  case CGRCO:
   RZ(f=every(every2(aa,h,0L,jtfixa),0L,jtaro)); 
   RZ(g=fixa(aa,g));
   R df2(f,g,wf);
  case CIBEAM:
   if(f)RZ(f=fixa(aa,f));
   if(g)RZ(g=fixa(aa,g));
   R f&&g ? (VDDOP&v->flag?df2(f,g,df2(head(h),tail(h),wf)):df2(f,g,wf)) : 
            (VDDOP&v->flag?df1(f,  df2(head(h),tail(h),wf)):df1(f,  wf)) ;
  case CTILDE:
   if(f&&NAME&AT(f)){
    RZ(y=sfn(0,f));
    if(all1(eps(box(y),jt->fxpath)))R w;
    ASSERT(jt->fxi,EVLIMIT);
    jt->fxpv[--jt->fxi]=y; 
    if(x=symbrdlock(f)){
     RZ(z=fixa(aa,x));
     if(a!=zero&&selfq(x))RZ(z=fixrecursive(a,z));
    }
    jt->fxpv[jt->fxi++]=mtv;
    RE(z);
    ASSERT(TYPESEQ(AT(w),AT(z))||AT(w)&NOUN&&AT(z)&NOUN,EVDOMAIN);
    R z;
   }else R df1(fixa(num[2],f),wf);
  default:
   if(f)RZ(f=fixa(aa,f));
   if(g)RZ(g=fixa(aa,g));
   R f&&g?df2(f,g,wf):f?df1(f,wf):w;
}}   /* 0=a if fix names; 1=a if fix names only if does not contain $: */


F1(jtfix){PROLOG(0005);A z;I*rv=jt->rank;
 RZ(w);
 jt->rank=0;
 RZ(jt->fxpath=reshape(sc(jt->fxi=(I)255),ace)); jt->fxpv=AAV(jt->fxpath);
 if(LIT&AT(w)){ASSERT(1>=AR(w),EVRANK); RZ(w=nfs(AN(w),CAV(w)));}
 ASSERT(AT(w)&NAME+VERB,EVDOMAIN);
 RZ(z=fixa(zero,AT(w)&VERB?w:symbrdlock(w)));
 if(AT(z)&VERB+ADV+CONJ){V*v=VAV(z); if(v->f){v->flag|=VFIX+VNAMED; v->flag^=VNAMED;}}
 jt->rank=rv; jt->fxpath=0;
 EPILOG(z);
}
