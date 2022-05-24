/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Interval Index, mostly by Binary Search                          */

#include "j.h"


#define BXLOOP(T)  \
 {T*wv=(T*)AV(w),x;                                                  \
  if(descend){DQ(m, x=*wv++; I t=n; t=x>=0?p:t; t=x>=1?0:t; *zv++=t;);        /* p q */  \
  }else if(p<0){DQ(m, *zv++=n*(1<*wv++);) /*   q */  \
  }else if(q<0){DQ(m, *zv++=n*(0<*wv++););   /* p   */  \
  }else{DQ(m, x=*wv++; I t=n; t=x<=1?q:t; t=x<=0?0:t; *zv++=t;) /* p q */  \
  }  \
 }

static B jtiixBX(J jt,I n,I m,A a,A w,I*zv){B*av,*b,descend;I p,q;
 av=BAV(a); descend=av[0]>av[n-1];
 // p is index of 1st 0, or -1 if none; q similar for first 1
 b=memchr(av,1-descend,n);  // if ascending, get position of 1st 1; if descending, 1st 0.
 p=(b-av)-(1-descend); p=(1-descend)|(I)b?p:-1; p=descend|(I)b?p:0; // set p to position of 0 (descending) or before 1st 1 (ascending).  If descending & 0 not found, use -1; if ascending & 1 not found, use 0
 q=(b-av)-descend; q=descend|(I)b?q:-1; q=(1-descend)|(I)b?q:0; // set q to position of 1 (ascending) or before 1st 0 (descending).  If ascending & 1 not found, use -1; if descending & 0 not found, use 0
 switch(AT(w)){
 case INT: BXLOOP(I); break;
 case FL:  BXLOOP(D); break;
 case B01:
  b=BAV(w);
  if(descend){DQ(m, *zv++=p*!*b++;)         /* p q */
  }else if(p<0){mvc(m*SZI,zv,1,MEMSET00);  /*   q */
  }else if(q<0){DQ(m, *zv++=n* *b++;)  /* p   */
  }else{DQ(m, *zv++=q* *b++;)  /* p q */
  }
 }
 R 1;
}    /* a I."r w where a is a boolean list */

static B jtiixI(J jt,I n,I m,A a,A w,I*zv){A t;B ascend;I*av,j,p,q,*tv,*u,*v,*vv,*wv,x,y;
 av=AV(a); wv=AV(w);
 p=av[0]; q=av[n-1]; ascend=p<=q; if(!ascend){x=p; p=q; q=x;}
 GATV0(t,INT,1+q-p,1); v=AV(t); tv=v-p; vv=v+AN(t);  // v->buffer; tv->virtual buffer origin, where p=0; vv->buffer end
  // This could be recoded to allocate slots for <p and >q, but it would be better only if those cases were common
 if(ascend){u=av;     x=*u++; *v++=j=0; DQ(n-1, ++j; y=*u++; ASSERT(BETWEENC(y,x,q),EVDOMAIN); DQ(y-x, *v++=j;); x=y;);}
 else      {u=av+n-1; x=*u--;      j=n; DQ(n-1, --j; y=*u--; ASSERT(BETWEENC(y,x,q),EVDOMAIN); DQ(y-x, *v++=j;); x=y;);}
 if(ascend)DQ(m, x=*wv++; *zv++=x<=p?0:q<x?n:tv[x];)
 else      DQ(m, x=*wv++; *zv++=x>=q?0:p>x?n:tv[x];);
 R 1;
}    /* a I. w where a is a list of small range integers */

#define SBCOMP(x,y) (SBGT((x),(y))-SBLT((x),(y)))
#define COMPVLOOP(T,c)       \
 {T*u=(T*)uu,*v=(T*)vv; DQ(c, if(*u!=*v){cc=*u<*v?-1:1; break;} ++u; ++v;);}
#define COMPVLOOF(T,c,COMP)  \
 {T*u=(T*)uu,*v=(T*)vv; DQ(c, if(cc=COMP(*u,*v))break;          ++u; ++v;);}

#define MID(k,p,q)   k=(UI)((p)+(q))>>1  /* beware integer overflow */

#define BSLOOP1(CMP)           \
  p=0; q=n-1; y=*wv++;         \
  while(p<=q){MID(k,p,q); CMP; \
      if(b)q=k-1; else p=k+1;}
// Binary search without misprediction, on atoms
#define BSLOOP1x(CMP)    /* if CMP is true, move q; otherwise p */       \
  y=*wv++; p=-1; q=n; k=(q-1)>>1;         \
  do{   /* empty lists handled earlier */ \
   x=av[k]; /* read the next value */ \
   I k2, k1=k; MID(k2,k,q); k=k2-((q-p)>>1); /* calculate new values for k */\
   k=CMP?k:k2; p=CMP?p:k1; q=CMP?k1:q; /* move k first to start next fetch */\
  }while(q>p+1); 

#define BSLOOPN(NE,CMP)        \
  p=0; q=n-1;                  \
  while(p<=q){MID(k,p,q); u=av+c*k; v=wv; b=1; DQ(c, x=*u++; y=*v++; if(NE){CMP; break;}); /* make this compare fixed length? */   \
      if(b)q=k-1; else p=k+1;}
// Without misprediction.
#define BSLOOPNx(NE,CMP)    /* if CMP is true, move q; otherwise p */       \
  p=-1; q=n; k=(q-1)>>1;         \
  do{   /* empty lists handled earlier */ \
   u=av+k*c; v=wv; I _n=c; while(1){x=*u; y=*v; if(--_n==0||NE)break; ++u; ++v;};  \
   I k2, k1=k; MID(k2,k,q); k=k2-((q-p)>>1); /* calculate new values for k */\
   k=CMP?k:k2; p=CMP?p:k1; q=CMP?k1:q; /* move k first to start next fetch */\
  }while(q>p+1); 

#define BSLOOP(Ta,Tw)       \
 {Ta*av=(Ta*)AV(a),x; Tw y; Tw*wv=(Tw*)AV(w);                                             \
   if(c==1){if(ge!=1)DQ(m, BSLOOP1x(x>=y); *zv++=q;)else DQ(m, BSLOOP1x(x<=y); *zv++=q;)}      \
   else{if(ge!=1)DQ(m, Ta* u; Tw *v; BSLOOPNx(x!=y,x>=y); *zv++=q; wv+=c;)else DQ(m, Ta* u; Tw *v; BSLOOPNx(x!=y,x<=y); *zv++=q; wv+=c;)}      \
 }

#define BSLOOF(Ta,Tw,COMP)  \
 {Ta*av=(Ta*)AV(a),*u,x;                                              \
  Tw*wv=(Tw*)AV(w),*v,y;                                              \
  if(1==c) DQ(m, BSLOOP1(b=ge!=COMP(av[k],y));   *zv++=1+q;       )   \
  else     DQ(m, BSLOOPN(cc=COMP(x,y),b=gt==cc); *zv++=1+q; wv+=c;);  \
 }

// Combine two types into a single value.  Originally this was 7*s+t which produced
// large values unsuitable for a branch table, and also took advantage of the fact that
// codes produced by multiple combinations, such as LIT,B01 and B01,FL which both produce
// 1111 would not generate spurious accepted cases because only one of them is HOMO.
#define CVCASE(a,b)     (6*((0x28c>>(a))&7)+((0x28c>>(b))&7))   // Must distinguish 0 2 3 4 6 7->4 3 1 0 2 5  01010001100
#define CVCASECHAR(a,b) ((4*(0x30004>>(a))+(0x30004>>(b)))&0xf)  // distinguish character cases and SBT

// x I. y
F2(jticap2){A*av,*wv,z;C*uu,*vv;I ar,*as,at,b,c,ck,cm,ge,gt,j,k,m,n,p,q,r,t,wr,*ws,wt,* RESTRICT zv;I cc;
 ARGCHK2(a,w);
 ar=AR(a); at=AT(a); as=AS(a); SETIC(a,n); r=ar-1<0?0:ar-1;  // n=length of 1-cell of a, r=frame of a
 wr=AR(w); wt=AT(w); ws=AS(w);
 ASSERT(r<=wr,EVRANK);
 ASSERTAGREE(as+ar-r,ws+wr-r,r)
 ASSERT((POSIFHOMO(at,wt)&-AN(a)&-AN(w))>=0,EVDOMAIN); ASSERT(!ISSPARSE(at|wt),EVNONCE); // if no empties, verify agreement & non-sparse
 CPROD(AN(w),m,wr-r,ws); CPROD(AN(w),c,r,ws+wr-r);  // m=#atoms in result   c=# atoms in a cell of w
 GATV(z,INT,m,wr-r,ws); zv=AV(z);
 if(((m-1)|(n-1)|(c-1))<0){DQ(m, *zv++=0;); R z;}  // exit with zeros for empty args
 t=maxtyped(at,wt);
 if(1==c){  // the search is for atoms
  if(at&B01&&wt&B01+INT+FL){RZ(iixBX(n,m,a,w,zv)); R z;}
  if(at&wt&INT){
   // Integer search.  check for small-range
   UI r=IAV(a)[n-1]-IAV(a)[0]; r=IAV(a)[n-1]<IAV(a)[0]?0-r:r;  // get range, which may overflow I but will stay within UI
   UI4 nlg; CTLZI(n,nlg); nlg=(nlg<<1)+(SGNTO0(SGNIF((n<<1),nlg)));   // approx lg with 1 bit frac precision.  Can't shift 64 bits in case r=1
   if((I)((r>>2)+2*n)<(I)(m*nlg)){RZ(iixI(n,m,a,w,zv)); R z;}  // weight misbranches as equiv to 8 stores
  }
 }
 cc=0; uu=CAV(a); vv=CAV(a)+(c*(n-1)<<bplg(at));
 // first decide if the input array is ascending or descending, comparing the first & last items atom by atom
 switch(CTTZ(at)){
  case INTX:  COMPVLOOP(I, c);           break;
  default:   ASSERT(0,EVNONCE);
  case B01X:  COMPVLOOP(B, c);           break;
  case LITX:  COMPVLOOP(UC,c);           break;
  case FLX:   COMPVLOOP(D, c);           break;
  case CMPXX: COMPVLOOP(D, c+c);         break;
  case C2TX:  COMPVLOOP(US,c);           break;
  case C4TX:  COMPVLOOP(C4,c);           break;
  case SBTX:  COMPVLOOF(SB,c, SBCOMP  ); break;
  case XNUMX: COMPVLOOF(X, c, xcompare); break;
  case RATX:  COMPVLOOF(Q, c, qcompare); break;
  case BOXX:  
   av=AAV(a); wv=AAV(w);
   DO(c, if(cc=compare(C(av[i]),C(av[i+c*(n-1)])))break;);
 }
 ge=cc; gt=-ge;
 if(unlikely(t&JCHAR+SBT)){
  switch(CVCASECHAR(CTTZ(at),CTTZ(wt))){
  case CVCASECHAR(LITX, C2TX ): BSLOOP(UC,US); break;
  case CVCASECHAR(LITX, C4TX ): BSLOOP(UC,C4); break;
#if C_LE
  case CVCASECHAR(LITX, LITX ): BSLOOP(UC,UC); break;
#else
  case CVCASECHAR(LITX, LITX ): if(1&c){BSLOOP(UC,UC); break;}else c>>=1; /* fall thru */
#endif
  case CVCASECHAR(C2TX, C2TX ): BSLOOP(US,US); break;
  case CVCASECHAR(C2TX, C4TX ): BSLOOP(US,C4); break;
  case CVCASECHAR(C2TX, LITX ): BSLOOP(US,UC); break;
  case CVCASECHAR(C4TX, C2TX ): BSLOOP(C4,US); break;
  case CVCASECHAR(C4TX, C4TX ): BSLOOP(C4,C4); break;
  case CVCASECHAR(C4TX, LITX ): BSLOOP(C4,UC); break;
  case CVCASECHAR(SBTX, SBTX ): BSLOOF(SB,SB, SBCOMP  ); break;
  default:   ASSERT(0,EVNONCE);
  }
 }else if(unlikely(t&BOX)){
   for(j=0,cm=c*m;j<cm;j+=c){
    p=0; q=n-1;
    while(p<=q){
     MID(k,p,q); ck=c*k; I b=1; 
     DO(c, if(cc=compare(C(av[i+ck]),C(wv[i+j]))){b=gt==cc; break;});
     if(b)q=k-1; else p=k+1;
    } 
    *zv++=1+q;
   }
 }else{
  switch(CVCASE(CTTZ(at),CTTZ(wt))){
  case CVCASE(B01X, B01X ): BSLOOP(C, C ); break;
  case CVCASE(B01X, INTX ): BSLOOP(C, I ); break;
  case CVCASE(B01X, FLX  ): BSLOOP(C, D ); break;
  case CVCASE(INTX, B01X ): BSLOOP(I, C ); break;
  case CVCASE(INTX, INTX ): BSLOOP(I, I ); break;
  case CVCASE(INTX, FLX  ): BSLOOP(I, D ); break;
  case CVCASE(FLX,  B01X ): BSLOOP(D, C ); break;
  case CVCASE(FLX,  INTX ): BSLOOP(D, I ); break;
  case CVCASE(CMPXX,CMPXX): c+=c;  /* fall thru */
  case CVCASE(FLX,  FLX  ): BSLOOP(D, D ); break;
  case CVCASE(XNUMX,XNUMX): BSLOOF(X, X, xcompare); break;
  case CVCASE(RATX, RATX ): BSLOOF(Q, Q, qcompare); break;
  default:
   ASSERT(TYPESNE(at,wt),EVNONCE);
   if(TYPESNE(t,at))RZ(a=cvt(t,a));
   if(TYPESNE(t,wt))RZ(w=cvt(t,w));
   switch(t){
   case CMPX: c+=c;  /* fall thru */ 
   case FL:   BSLOOP(D,D);           break;
   case XNUM: BSLOOF(X,X, xcompare); break;
   case RAT:  BSLOOF(Q,Q, qcompare); break;
   default:   ASSERT(0,EVNONCE);
   }
  }
 }
 RETF(z);
}    /* a I. w */
