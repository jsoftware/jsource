/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"
#if C_CRC32C && SY_64
#include "viavx.h"

// ******************* i., fourth class: sequential comparison ***************************************
// implemented only for i. i: e. u/.   - perhaps should revert for other compounds

// fz=bit0 = commutative, bit1 set if incomplete y must be filled with 0 (to avoid isub oflo), bit2 set if incomplete x must be filled with i (for fdiv NaN),
// bit3 set for int-to-float on x, bit4 for int-to-float on y
// bit5 set to suppress loop-unrolling
// bit6 set for bool-to-int on x, bit7 for bool-to-int on y
// bit8 set for bool-to-float on x, bit9 for bool-to-float on y
// bit12 this is a reverse-index loop
// bit13 not used
// bit14 this is u./
// bit15 this is e.

// do one computation. xy bit 0 means fetch/incr y, bit 1 means fetch/incr x.  bit 2 is never used.  lineno is the offset to the row being worked on
#define CPRMDO(name,zzop,xy,fz,lineno,inv)  if((xy)&2)LDBID(xx,OFFSETBID(x,((fz)&0x1000?-1:1)*lineno*NPAR,fz,0x8,0x40,0x100),fz,0x8,0x40,0x100) if((xy)&1)LDBID(yy,OFFSETBID(y,((fz)&0x1000?-1:1)*lineno*NPAR,fz,0x10,0x80,0x200),fz,0x10,0x80,0x200)  \
     if((xy)&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if((xy)&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
     zzop; if(unlikely((maskatend=_mm256_movemask_pd(zz))!=inv*15)){orign2+=lineno; goto name##out;}

#define CPRMINCR(xy,fz,ct) if((xy)&2)INCRBID(x,((fz)&0x1000?-1:1)*(ct),fz,0x8,0x40,0x100) if((xy)&1)INCRBID(y,((fz)&0x1000?-1:1)*(ct),fz,0x10,0x80,0x200)

// set mask for last group.  Also advance pointer to last block if reversed scan
#define CPRMALIGN(zzop,xy,fz,len)  \
  if((fz)&0x1000){CPRMINCR(xy,fz,NPAR-(len)) endmask = _mm256_loadu_si256((__m256i*)(validitymask+5+(((len)-1)&(NPAR-1))));} else endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-(len))&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */

// the short loop is never used here
#define CPRMDUFF(name,zzop,xy,fz,len,lpmsk,inv) \
     if(!((fz)&(lpmsk))){ \
      orign2=n2=DUFFLPCT((len)-1,3);  /* # turns through duff loop */ \
      orign2<<=3; \
      if(n2>0){ \
       backoff=DUFFBACKOFF((len)-1,3); \
       orign2+=backoff+1;  \
       CPRMINCR(xy,fz,(backoff+1)*NPAR) \
       switch(backoff){ \
       do{ \
       case -1: CPRMDO(name,zzop,xy,fz,0,inv) case -2: CPRMDO(name,zzop,xy,fz,1,inv) case -3: CPRMDO(name,zzop,xy,fz,2,inv) case -4: CPRMDO(name,zzop,xy,fz,3,inv) \
       case -5: CPRMDO(name,zzop,xy,fz,4,inv) case -6: CPRMDO(name,zzop,xy,fz,5,inv) case -7: CPRMDO(name,zzop,xy,fz,6,inv) case -8: CPRMDO(name,zzop,xy,fz,7,inv) \
       CPRMINCR(xy,fz,8*NPAR) \
       }while(--n2!=0); \
       } \
      } \
     }else{orign2=n2=(len-1)>>LGNPAR; \
      if(n2!=0){NOUNROLL do{CPRMDO(name,zzop,4+(xy),fz,0,inv) CPRMINCR(xy,fz,NPAR)}while(--n2!=0);}  \
     } \

#define CPRMMASK(zzop,xy,fz,inv) if((xy)&2)LDBIDM(xx,x,fz,0x8,0x40,0x100,endmask) if((xy)&1)LDBIDM(yy,y,fz,0x10,0x80,0x200,endmask)  \
  if((xy)&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if((xy)&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
  zzop; maskatend=(!!inv^!!((fz)&0x8000))?_mm256_movemask_pd(_mm256_and_pd(_mm256_castsi256_pd(endmask),zz)):_mm256_movemask_pd(_mm256_or_pd(_mm256_xor_pd(ones,_mm256_castsi256_pd(endmask)),zz));  // shift in match for i., nomatch for e.

#define seqsch256(name,fz,pref,zzop,inv) \
{ \
 __m256d xx,yy,zz; void *x,*y; \
 __m256i endmask; /* length mask for the last word */ \
 __m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones))); \
 __m256i acc0=_mm256_setzero_si256(), acc1=acc0, acc2=acc0, acc3=acc0, acc4=acc0, acc5=acc0, acc6=acc0, acc7=acc0; \
 I maskatend, nuniq=0; \
 UI backoff; UI n2; I orign2; /* orign2 goes -1 during rev search */ \
   /* will be removed except for divide */ \
 CVTEPI64DECLS pref \
 y=v; void *x0=u; \
 for(;ac;--ac){I nw=wsct; \
  for(;nw;--nw){x=x0; \
   I n0=asct; \
   LDBID1(yy,y,fz,0x10,0x80,0x200) CVTBID1(yy,yy,fz,0x10,0x80,0x200) \
   CPRMALIGN(zzop,2,fz,n0) \
   CPRMDUFF(name,zzop,2,fz,n0,32+8,inv) \
   CPRMMASK(zzop,2,fz,inv) /* runout, using mask */ \
name##out: \
   if((fz)&0x8000){*(C*)zv=maskatend!=inv*15; zv=(C*)zv+1; \
   }else{ \
    n2<<=3; \
    orign2-=n2; orign2<<=LGNPAR; \
    orign2+=(((fz)&0x1000?inv?0x4322111100000000:0x0000000011112234:inv?0x4010201030102010:0x0102010301020104)>>(maskatend<<2))&7; if((fz)&0x1000){orign2=n0-1-orign2; orign2=orign2<0?n0:orign2;} \
    *(I*)zv=orign2; if((fz)&0x4000){nuniq+=(orign2-wsct+nw)==0; ((I*)zv)[orign2-wsct+nw]++;} zv=(I*)zv+1;  /* if key, look back to incr the first in class */ \
   } \
   INCRBID(y,q,fz,0x10,0x80,0x200) \
  } \
  INCRBID(x0,p,fz,0x08,0x40,0x100) \
  y=(1==wc)?v:y; \
 } \
 if((fz)&0x4000)AM(z)=nuniq; \
}


#define IOSCCASE(bit,multi,mode) ((18*(multi)+((bit)-((bit)>=16?2:0)))*4+((mode)&3))

// xe is the expression for reading one comparand, exp is the expression for 'no match between x and av[j]')
// loop through storing the index at which a match was found
#define SCDO(bit,T,exp)  \
   case IOSCCASE(bit,0,IIDOT): {T*v0=(T*)v,*wv=(T*)v,x; T*av=(T*)u+asct; DQ(ac, DQ(wsct, x=*wv; j=-asct;   while(j<0 &&(exp))++j; *(I*)zv=j+=asct; zv=(I*)zv+1;       wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,0,IFORKEY): {T*v0=(T*)v,*wv=(T*)v,x; I nuniq=0; T*av=(T*)u+asct; DQ(ac, DO(wsct, x=*wv; j=-asct;   while(j<0 &&(exp))++j; *(I*)zv=j+=asct; nuniq+=(j-i)==0; ((I*)zv)[j-i]++; zv=(I*)zv+1;       wv+=q;); AM(z)=nuniq; av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,0,IICO):  {T*v0=(T*)v,*wv=(T*)v,x; T*av=(T*)u; DQ(ac, DQ(wsct, x=*wv; j=asct-1; while(0<=j&&(exp))--j; *(I*)zv=(j=0>j?asct:j); zv=(I*)zv+1; wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,0,IEPS):  {T*v0=(T*)v,*wv=(T*)v,x; T*av=(T*)u+asct; DQ(ac, DQ(wsct, x=*wv; j=-asct;   while(j<0 &&(exp))++j; *(C*)zv=SGNTO0(j); zv=(C*)zv+1;     wv+=q;); av+=p; if(1==wc)wv=v0;);} break;

// same but the cells have n atoms, each of which is compared.  comparands are wv[jj] and avv[jj]
#define SCDON(bit,T,exp)  \
   case IOSCCASE(bit,1,IIDOT): {T*v0=(T*)v,*wv=(T*)v; T*av=(T*)u; DQ(ac, DQ(wsct, j=-asct;   T*avv=av; do{I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break; avv+=n;}while(++j<0);   *(I*)zv=j+=asct; zv=(I*)zv+1;       wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,1,IFORKEY): {T*v0=(T*)v,*wv=(T*)v; I nuniq=0; T*av=(T*)u; DQ(ac, DO(wsct, j=-asct;   T*avv=av; do{I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break; avv+=n;}while(++j<0);  *(I*)zv=j+=asct; nuniq+=(j-i)==0; ((I*)zv)[j-i]++; zv=(I*)zv+1;       wv+=q;); AM(z)=nuniq; av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,1,IICO):  {T*v0=(T*)v,*wv=(T*)v; T*av=(T*)u; DQ(ac, DQ(wsct, j=asct-1;  T*avv=av+asct*n; do{avv-=n; I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break;}while(--j>=0);     *(I*)zv=(j=0>j?asct:j); zv=(I*)zv+1; wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,1,IEPS):  {T*v0=(T*)v,*wv=(T*)v; T*av=(T*)u; DQ(ac, DQ(wsct, j=-asct;   T*avv=av; do{I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break; avv+=n;}while(++j<0);    *(C*)zv=SGNTO0(j); zv=(C*)zv+1;     wv+=q;); av+=p; if(1==wc)wv=v0;);} break;

// ac is # outer cells of a, asct=#items in 1 inner cell, wc is #outer search cells, wsct is #items to search for per outer cell
// n is #atoms in a cell
A jtiosc(J jt,I mode,I n,I asct,I wsct,I ac,I wc,A a,A w,A z){I j,p,q; void *u,*v,*zv;
 p=ac>1?asct:0; q=REPSGN(1-(wc|wsct)); p*=n; q&=n;  // q=1<wc||1<wsct; number of atoms to move between repeats p=*atoms of a to move between repeats
 zv=voidAV(z); u=voidAV(a); v=voidAV(w);
 // Create a pseudotype 19 (=XDX) for intolerant comparison.
 I at=AT(a); at|=(at&C2T+C4T)>>(C2TX-INT2X); // shift C2T/C4T (17/18) down to INT2/INT4 (9/10)
 I bit=CTTZ(at); bit=(at&(jt->cct==1.0?FL:0))?XDX:bit;
 switch(IOSCCASE(bit,n>1,mode)){
  SCDO(B01X,C,x!=av[j]      );
  SCDO(LITX,C,x!=av[j]      );
  SCDO(INT2X,S,x!=av[j]      );
  SCDO(INT4X,C4,x!=av[j]      );
  SCDO(CMPXX,Z,!zeq(x, av[j]));
  SCDO(QPX,E,NEE(x, av[j]));
  SCDO(XNUMX,A,!equx(x, av[j]));
  SCDO(RATX,Q,!QEQ(x, av[j]));
  SCDO(SBTX,SB,x!=av[j]      );
  SCDO(BOXX,A,!equ(C(x),C(av[j])));
#if C_AVX2 || EMU_AVX2
  // The instruction set is too quirky to do this with macros
  case IOSCCASE(XDX,0,IIDOT): seqsch256(seqschidotDD0,0x0000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break;
  case IOSCCASE(XDX,0,IFORKEY): seqsch256(seqschkeyDD0,0x4000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
  case IOSCCASE(XDX,0,IICO): seqsch256(seqschicoDD0,0x1000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
  case IOSCCASE(XDX,0,IEPS): seqsch256(seqschepsDD0,0x8000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
#define ZZTEQ zz=_mm256_xor_pd(_mm256_cmp_pd(xx,_mm256_mul_pd(yy,cct),_CMP_GT_OQ),_mm256_cmp_pd(yy,_mm256_mul_pd(xx,cct),_CMP_LE_OQ));  // tolerant =
  case IOSCCASE(FLX,0,IIDOT): seqsch256(seqschidotDD,0x0000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
  case IOSCCASE(FLX,0,IFORKEY): seqsch256(seqschkeyDD,0x4000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
  case IOSCCASE(FLX,0,IICO): seqsch256(seqschicoDD,0x1000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
  case IOSCCASE(FLX,0,IEPS): seqsch256(seqschepsDD,0x8000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
#else
  SCDO(XDX,D,x!=av[j])
  SCDO(FLX,D,!TCMPEQ(jt->cct,x,av[j]));
#endif


#if C_AVX2 || EMU_AVX2
  case IOSCCASE(INTX,0,IIDOT): seqsch256(seqschidotII,0x0000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
  case IOSCCASE(INTX,0,IFORKEY): seqsch256(seqschkeyII,0x4000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
  case IOSCCASE(INTX,0,IICO): seqsch256(seqschicoII,0x1000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
  case IOSCCASE(INTX,0,IEPS): seqsch256(seqschepsII,0x8000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
#else
  SCDO(INTX,I,x!=av[j]      );
#endif

  SCDON(B01X,C,wvv[jj]!=avv[jj]      );
  SCDON(LITX,C,wvv[jj]!=avv[jj]      );
  SCDON(INT2X,S, wvv[jj]!=avv[jj]      );
  SCDON(INT4X,C4,wvv[jj]!=avv[jj]      );
  SCDON(CMPXX,Z, !zeq(wvv[jj], avv[jj]));
  SCDON(QPX,E, !NEE(wvv[jj], avv[jj]));
  SCDON(XNUMX,A, !equ(wvv[jj], avv[jj]));
  SCDON(RATX,Q, !QEQ(wvv[jj], avv[jj]));
  SCDON(INTX,I, wvv[jj]!=avv[jj]      );
  SCDON(SBTX,SB,wvv[jj]!=avv[jj]      );
  SCDON(BOXX,A, !equ(C(wvv[jj]),C(avv[jj])));
  SCDON(XDX,D, wvv[jj]!=avv[jj]) ;
  SCDON(FLX,D, !TCMPEQ(jt->cct,wvv[jj],avv[jj]));
  default:  jsignal(EVSYSTEM);
 }

 R z;  // return non-error indic, but also where the partition count for IFORKEY is stored
}    /* right argument cell is scalar or list; only for modes IIDOT IICO IEPS, and intolerant IFORKEY */
#endif
