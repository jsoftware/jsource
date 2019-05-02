/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Interval Index, mostly by Binary Search                          */

#include "j.h"


#define BXLOOP(T)  \
 {T*wv=(T*)AV(w),x;                                                  \
  switch((4*descend)+(0<=p?2:0)+(I )(0<=q)){                             \
   case 1: DO(m, *zv++=n*(1<*wv++););              break; /*   q */  \
   case 2: DO(m, *zv++=n*(0<*wv++););              break; /* p   */  \
   case 3: DO(m, x=*wv++; *zv++=x<=0?0:x<=1?q:n;); break; /* p q */  \
   case 7: DO(m, x=*wv++; *zv++=x>=1?0:x>=0?p:n;);        /* p q */  \
 }}

static B jtiixBX(J jt,I n,I m,A a,A w,I*zv){B*av,*b,descend;I p,q;
 av=BAV(a); descend=av[0]>av[n-1];
 b=memchr(av,C0,n); p=b?b-av:-1;
 b=memchr(av,C1,n); q=b?b-av:-1;
 switch(CTTZNOFLAG(AT(w))){
  case INTX: BXLOOP(I); break;
  case FLX:  BXLOOP(D); break;
  case B01X:
   b=BAV(w);
   switch((4*descend)+(0<=p?2:0)+(I )(0<=q)){
    case 1: memset(zv,C0,m*SZI);   break;  /*   q */
    case 2: DO(m, *zv++=n* *b++;); break;  /* p   */
    case 3: DO(m, *zv++=q* *b++;); break;  /* p q */
    case 7: DO(m, *zv++=p*!*b++;);         /* p q */
 }}
 R 1;
}    /* a I."r w where a is a boolean list */

static B jtiixI(J jt,I n,I m,A a,A w,I*zv){A t;B ascend;I*av,j,p,q,*tv,*u,*v,*vv,*wv,x,y;
 av=AV(a); wv=AV(w);
 p=av[0]; q=av[n-1]; ascend=p<=q; if(!ascend){x=p; p=q; q=x;}
 GATV0(t,INT,1+q-p,1); v=AV(t); tv=v-p; vv=v+AN(t);
 if(ascend){u=av;     x=*u++; *v++=j=0; DO(n-1, ++j; y=*u++; ASSERT(p<=y&&y<=q&&vv>=v+y-x,EVDOMAIN); DO(y-x, *v++=j;); x=y;);}
 else      {u=av+n-1; x=*u--;      j=n; DO(n-1, --j; y=*u--; ASSERT(p<=y&&y<=q&&vv>=v+y-x,EVDOMAIN); DO(y-x, *v++=j;); x=y;);}
 if(ascend)DO(m, x=*wv++; *zv++=x<=p?0:q<x?n:tv[x];)
 else      DO(m, x=*wv++; *zv++=x>=q?0:p>x?n:tv[x];);
 R 1;
}    /* a I. w where a is a list of small range integers */

#define SBCOMP(x,y)       \
 ((SBLT((x),(y)))?-1:(SBGT((x),(y)))?1:0)
#define COMPVLOOP(T,c)       \
 {T*u=(T*)uu,*v=(T*)vv; DO(c, if(*u!=*v){cc=*u<*v?-1:1; break;} ++u; ++v;);}
#define COMPVLOOF(T,c,COMP)  \
 {T*u=(T*)uu,*v=(T*)vv; DO(c, if(cc=COMP(*u,*v))break;          ++u; ++v;);}

#define MID(k,p,q)   k=(UI)(p+q)>>1  /* beware integer overflow */

#define BSLOOP1(CMP)           \
  p=0; q=n-1; y=*wv++;         \
  while(p<=q){MID(k,p,q); CMP; \
      if(b)q=k-1; else p=k+1;}
#define BSLOOPN(NE,CMP)        \
  p=0; q=n-1;                  \
  while(p<=q){MID(k,p,q); u=av+c*k; v=wv; b=1; DO(c, x=*u++; y=*v++; if(NE){CMP; break;});  \
      if(b)q=k-1; else p=k+1;}

#define BSLOOP(Ta,Tw)       \
 {Ta*av=(Ta*)AV(a),*u,x;                                              \
  Tw*wv=(Tw*)AV(w),*v,y;                                              \
  switch((1==c?0:2)+(I )(1==ge)){                                         \
   case 0: DO(m, BSLOOP1(b=av[k]>=y); *zv++=1+q;       ); break;      \
   case 1: DO(m, BSLOOP1(b=av[k]<=y); *zv++=1+q;       ); break;      \
   case 2: DO(m, BSLOOPN(x!=y,b=x>y); *zv++=1+q; wv+=c;); break;      \
   case 3: DO(m, BSLOOPN(x!=y,b=x<y); *zv++=1+q; wv+=c;); break;      \
 }}

#define BSLOOF(Ta,Tw,COMP)  \
 {Ta*av=(Ta*)AV(a),*u,x;                                              \
  Tw*wv=(Tw*)AV(w),*v,y;                                              \
  if(1==c) DO(m, BSLOOP1(b=ge!=COMP(av[k],y));   *zv++=1+q;       )   \
  else     DO(m, BSLOOPN(cc=COMP(x,y),b=gt==cc); *zv++=1+q; wv+=c;);  \
 }

// Combine two types into a single value.  Originally this was 7*s+t which produced
// large values unsuitable for a branch table, and also took advantage of the fact that
// codes produced by multiple combinations, such as LIT,B01 and B01,FL which both produce
// 1111 would not generate spurious accepted cases because only one of them is HOMO.
#define TT(s,t) (((s)<<5)+(t))

F2(jticap2){A*av,*wv,z;B b;C*uu,*vv;I ar,*as,at,c,ck,cm,ge,gt,j,k,m,n,p,q,r,t,*u,*v,wr,*ws,wt,* RESTRICT zv;I cc;
 RZ(a&&w);
 ar=AR(a); at=AT(a); as=AS(a); n=ar?*as:1; r=ar?ar-1:0;
 wr=AR(w); wt=AT(w); ws=AS(w); b=!AN(a)||!AN(w);  // b if something is empty
 ASSERT(r<=wr,EVRANK);
 u=as+ar; v=ws+wr; DO(r, ASSERT(*--u==*--v,EVLENGTH););
 ASSERT(b||HOMO(at,wt),EVDOMAIN);
 ASSERT(b||at&DENSE&&wt&DENSE,EVNONCE);
 CPROD(AN(a),m,wr-r,ws); CPROD(AN(w),c,r,ws+wr-r);
 GATV(z,INT,m,wr-r,ws); zv=AV(z);
 if(!m||!n||!c){DO(m, *zv++=0;); R z;}  // exit with zeros for empty args
 t=maxtyped(at,wt);
 if(1==c){
  if(at&B01&&wt&B01+INT+FL){RZ(iixBX(n,m,a,w,zv)); R z;}
  if(at&INT&&wt&INT){D r;
   v=AV(a); r=(D)v[n-1]-(D)v[0]; if(0>r)r=-r;
   if(m+r<1.4*m*log((D)n)){RZ(iixI(n,m,a,w,zv)); R z;}
 }}
 jt->workareas.compare.complt=-1; cc=0; uu=CAV(a); vv=CAV(a)+(c*(n-1)<<bplg(at));
 // first decide if the input array is ascending or descending  todo kludge just compare first to last
 switch(CTTZ(at)){
  default:   ASSERT(0,EVNONCE);
  case B01X:  COMPVLOOP(B, c);           break;
  case LITX:  COMPVLOOP(UC,c);           break;
  case INTX:  COMPVLOOP(I, c);           break;
  case FLX:   COMPVLOOP(D, c);           break;
  case CMPXX: COMPVLOOP(D, c+c);         break;
  case C2TX:  COMPVLOOP(US,c);           break;
  case C4TX:  COMPVLOOP(C4,c);           break;
  case SBTX:  COMPVLOOF(SB,c, SBCOMP  ); break;
  case XNUMX: COMPVLOOF(X, c, xcompare); break;
  case RATX:  COMPVLOOF(Q, c, qcompare); break;
  case BOXX:  
   av=AAV(a); wv=AAV(w);
   DO(c, if(cc=compare(av[i],av[i+c*(n-1)]))break;);
 }
 ge=cc; gt=-ge;
 switch(TT(CTTZ(at),CTTZ(wt))){
  case TT(B01X, B01X ): BSLOOP(C, C ); break;
  case TT(B01X, INTX ): BSLOOP(C, I ); break;
  case TT(B01X, FLX  ): BSLOOP(C, D ); break;
  case TT(LITX, C2TX ): BSLOOP(UC,US); break;
  case TT(LITX, C4TX ): BSLOOP(UC,C4); break;
#if C_LE
  case TT(LITX, LITX ): BSLOOP(UC,UC); break;
#else
  case TT(LITX, LITX ): if(1&c){BSLOOP(UC,UC); break;}else c>>=1; /* fall thru */
#endif
  case TT(C2TX, C2TX ): BSLOOP(US,US); break;
  case TT(C2TX, C4TX ): BSLOOP(US,C4); break;
  case TT(C2TX, LITX ): BSLOOP(US,UC); break;
  case TT(C4TX, C2TX ): BSLOOP(C4,US); break;
  case TT(C4TX, C4TX ): BSLOOP(C4,C4); break;
  case TT(C4TX, LITX ): BSLOOP(C4,UC); break;
  case TT(SBTX, SBTX ): BSLOOF(SB,SB, SBCOMP  ); break;
  case TT(INTX, B01X ): BSLOOP(I, C ); break;
  case TT(INTX, INTX ): BSLOOP(I, I ); break;
  case TT(INTX, FLX  ): BSLOOP(I, D ); break;
  case TT(FLX,  B01X ): BSLOOP(D, C ); break;
  case TT(FLX,  INTX ): BSLOOP(D, I ); break;
  case TT(CMPXX,CMPXX): c+=c;  /* fall thru */
  case TT(FLX,  FLX  ): BSLOOP(D, D ); break;
  case TT(XNUMX,XNUMX): BSLOOF(X, X, xcompare); break;
  case TT(RATX, RATX ): BSLOOF(Q, Q, qcompare); break;
  case TT(BOXX, BOXX ):
   for(j=0,cm=c*m;j<cm;j+=c){
    p=0; q=n-1;
    while(p<=q){
     MID(k,p,q); ck=c*k; b=1; 
     DO(c, if(cc=compare(av[i+ck],wv[i+j])){b=gt==cc; break;});
     if(b)q=k-1; else p=k+1;
    } 
    *zv++=1+q;
   }
   break;
  default:
   ASSERT(TYPESNE(at,wt),EVNONCE);
   if(TYPESNE(t,at))RZ(a=cvt(t,a));
   if(TYPESNE(t,wt))RZ(w=cvt(t,w));
   switch(CTTZ(t)){
    case CMPXX: c+=c;  /* fall thru */ 
    case FLX:   BSLOOP(D,D);           break;
    case XNUMX: BSLOOF(X,X, xcompare); break;
    case RATX:  BSLOOF(Q,Q, qcompare); break;
    default:   ASSERT(0,EVNONCE);
 }}
 RETF(z);
}    /* a I."r w */
