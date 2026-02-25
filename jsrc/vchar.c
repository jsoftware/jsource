/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Character Functions  u&(a.&i.) and u&.(a.&i.)                    */

#include "j.h"
#include "ve.h"

#if C_AVX2 || EMU_AVX2
#define XLT(u,v) ((u)<(v))
#define XGT(u,v) ((u)>(v))
#define XLE(u,v) ((u)<=(v))
#define XGE(u,v) ((u)>=(v))
static APF256CC(2,min,C,C,MIN)
static APF256CC(2,max,C,C,MAX)
static APF256CC(2,lt,C,C,XLT)
static APF256CC(2,gt,C,C,XGT)
static APF256CC(2,le,C,C,XLE)
static APF256CC(2,ge,C,C,XGE)
#else
static APFX(maxCC, UC,UC,UC, MAX,, R EVOK;)
static APFX(minCC, UC,UC,UC, MIN,, R EVOK;)
static AIFX(ltCC,  B, UC,UC, <  )
static AIFX(leCC,  B, UC,UC, <= )
static AIFX(geCC,  B, UC,UC, >= )
static AIFX(gtCC,  B, UC,UC, >  )
#endif

// obsolete #define PLUSX +
DF2(jtcharfn2){F12IP;A z;B b;C c;I an,ar,*as,m,n,wn,wr,*ws,zn,zt;V*v;VF ado=0;
 ARGCHK2(a,w);
 v=FAV(self); c=FAV(v->fgh[0])->id;
 if(CUNDER==v->id)
  switch(c){
  case CMAX:  zt=LIT; ado=(VF)maxCC; break;
  case CMIN:  zt=LIT; ado=(VF)minCC; break;
  default: ASSERTSYS(ado,"charfn2 ado");
  }
 else
  switch(c){
  case CEQ:   R eq(a,w);
  case CNE:   R ne(a,w);
  case CLT:   zt=B01; ado=(VF)ltCC;  break;
  case CLE:   zt=B01; ado=(VF)leCC;  break;
  case CGE:   zt=B01; ado=(VF)geCC;  break;
  case CGT:   zt=B01; ado=(VF)gtCC;  break;
  default: ASSERTSYS(ado,"charfn2 ado");
  }
 
 an=AN(a); ar=AR(a); as=AS(a);
 wn=AN(w); wr=AR(w); ws=AS(w);
 ASSERTAGREE(as,ws,MIN(ar,wr))
 b=ar<=wr; zn=b?wn:an; m=b?an:wn; I r=b?wr:ar; I *s=b?ws:as; I rs=b?ar:wr; PROD(n,r-rs,s+rs);
 GA(z,zt,zn,r,s); if(!zn)R z;
 ((AHDR2FN*)ado)AH2A(m,n==1?~m:2*n+b,CAV(a),CAV(w),CAVn(r,z),jt);
 RETF(z);
}
