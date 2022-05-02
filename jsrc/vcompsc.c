/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Comparatives                                                     */
/*                                                                         */
/* Special code for the following cases:                                   */
/*    comp i. 0:          i.&0@:comp    IIF0EPS                            */
/*    comp i. 1:          i.&1@:comp                                       */
/*    comp i: 0:          i:&0@:comp                                       */
/*    comp i: 1:          i:&1@:comp                                       */
/*    [: + / comp         + /@:comp                                        */
/*    [: +./ comp         +./@:comp                                        */
/*    0: e. comp          1: e. comp                                       */
//    [: I. comp          I.@:comp       no longer supported except for e.and E.
/* where comp is one of the following (in order):                                     */
/*    = ~: < <: >: > E. e.                                                 */
// The routines do not have IRS.

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// fz=bit0 = commutative, bit1 set if incomplete y must be filled with 0 (to avoid isub oflo), bit2 set if incomplete x must be filled with i (for fdiv NaN),
// bit3 set for int-to-float on x, bit4 for int-to-float on y
// bit5 set to suppress loop-unrolling
// bit6 set for bool-to-int on x, bit7 for bool-to-int on y
// bit8 set for bool-to-float on x, bit9 for bool-to-float on y
// bit12 this is a reverse-index loop
// bit13 this is +/@:comp: inv means total is number of trues, otherwise number of falses

// do one computation. xy bit 0 means fetch/incr y, bit 1 means fetch/incr x; bit 2 is set if the loop is not unrolled (we can bypassa section).  lineno is the offset to the row being worked on
#define CPRMDO(zzop,xy,fz,lineno,inv)  if((xy)&2)LDBID(xx,OFFSETBID(x,((fz)&0x1000?-1:1)*lineno*NPAR,fz,0x8,0x40,0x100),fz,0x8,0x40,0x100) if((xy)&1)LDBID(yy,OFFSETBID(y,((fz)&0x1000?-1:1)*lineno*NPAR,fz,0x10,0x80,0x200),fz,0x10,0x80,0x200)  \
     if((xy)&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if((xy)&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
     zzop; if((fz)&0x2000){acc##lineno=_mm256_add_epi64(acc##lineno,_mm256_castpd_si256(zz));} else {if((xy)&4){if((maskatend=_mm256_movemask_pd(zz))!=inv*15)goto outs0;} else if((maskatend=_mm256_movemask_pd(zz))!=inv*15)goto out##lineno;}

#define CPRMINCR(xy,fz,ct) if((xy)&2)INCRBID(x,((fz)&0x1000?-1:1)*(ct),fz,0x8,0x40,0x100) if((xy)&1)INCRBID(y,((fz)&0x1000?-1:1)*(ct),fz,0x10,0x80,0x200)

// set mask for last group.  Also advance pointer to last block if reversed scan
#define CPRMALIGN(zzop,xy,fz,len)  \
  if((fz)&0x1000){CPRMINCR(xy,fz,NPAR-(len)) endmask = _mm256_loadu_si256((__m256i*)(validitymask+5+(((len)-1)&(NPAR-1))));} else endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-(len))&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */

#define CPRMDUFF(zzop,xy,fz,len,lpmsk,inv) \
     if(!((fz)&(lpmsk))){ \
      orign2=n2=DUFFLPCT((len)-1,3);  /* # turns through duff loop */ \
      if(n2>0){ \
       backoff=DUFFBACKOFF((len)-1,3); \
       CPRMINCR(xy,fz,(backoff+1)*NPAR) \
       switch(backoff){ \
       do{ \
       case -1: CPRMDO(zzop,xy,fz,0,inv) case -2: CPRMDO(zzop,xy,fz,1,inv) case -3: CPRMDO(zzop,xy,fz,2,inv) case -4: CPRMDO(zzop,xy,fz,3,inv) \
       case -5: CPRMDO(zzop,xy,fz,4,inv) case -6: CPRMDO(zzop,xy,fz,5,inv) case -7: CPRMDO(zzop,xy,fz,6,inv) case -8: CPRMDO(zzop,xy,fz,7,inv) \
       CPRMINCR(xy,fz,8*NPAR) \
       }while(--n2!=0); \
       } \
       n2<<=3; orign2<<=3; orign2+=backoff+1; \
      } \
     }else{orign2=n2=(len-1)>>LGNPAR; \
      if(n2!=0){NOUNROLL do{CPRMDO(zzop,4+(xy),fz,0,inv) CPRMINCR(xy,fz,NPAR)}while(--n2!=0);}  \
     } \

#define CPRMMASK(zzop,xy,fz,inv) if((xy)&2)LDBIDM(xx,x,fz,0x8,0x40,0x100,endmask) if((xy)&1)LDBIDM(yy,y,fz,0x10,0x80,0x200,endmask)  \
  if((xy)&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if((xy)&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
  zzop; if((fz)&0x2000){acc7=_mm256_add_epi64(acc7,_mm256_castpd_si256(_mm256_and_pd(_mm256_castsi256_pd(endmask),zz))); goto outs1;} \
  else{maskatend=inv?_mm256_movemask_pd(_mm256_and_pd(_mm256_castsi256_pd(endmask),zz)):_mm256_movemask_pd(_mm256_or_pd(_mm256_xor_pd(ones,_mm256_castsi256_pd(endmask)),zz)); goto outs0;}


#define cprimop256(name,fz,pref,zzop,inv) \
A name(J jt,A a,A w){ \
 __m256d xx,yy,zz; void *x=voidAV(a),*y=voidAV(w); \
 __m256i endmask; /* length mask for the last word */ \
 __m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones))); \
 __m256i acc0=_mm256_setzero_si256(), acc1=acc0, acc2=acc0, acc3=acc0, acc4=acc0, acc5=acc0, acc6=acc0, acc7=acc0; \
 I natend, maskatend; \
 UI backoff; UI n2; I orign2; /* orign2 goes -1 during rev search */ \
 _mm256_zeroupperx(VOIDARG) \
   /* will be removed except for divide */ \
 CVTEPI64DECLS pref \
 I n0=AN(w); \
 if(AR(a)&AR(w)){ \
  /* vector-to-vector, no repetitions */ \
  /* align dest to NPAR boundary, if needed and len makes it worthwhile */ \
  CPRMALIGN(zzop,3,fz,n0) \
  CPRMDUFF(zzop,3,fz,n0,32+16+8,inv) \
  CPRMMASK(zzop,3,fz,inv) /* runout, using mask */ \
 }else{ \
  if(!((fz)&1)&&AR(a)==0){ \
   /* atom+vector */ \
   LDBID1(xx,x,fz,0x8,0x40,0x100) CVTBID1(xx,xx,fz,0x8,0x40,0x100)  \
   CPRMALIGN(zzop,1,fz,n0) \
   CPRMDUFF(zzop,1,fz,n0,32+16,inv) \
   CPRMMASK(zzop,1,fz,inv) /* runout, using mask */ \
  }else{ \
   /* vector+atom */ \
   if((fz)&1){I taddr=(I)x^(I)y; I tn=AN(a); x=AR(a)==0?y:x; n0=AR(a)==0?n0:tn; y=(D*)((I)x^taddr);}else n0=AN(a);  \
   LDBID1(yy,y,fz,0x10,0x80,0x200) CVTBID1(yy,yy,fz,0x10,0x80,0x200)  \
   CPRMALIGN(zzop,2,fz,n0) \
   CPRMDUFF(zzop,2,fz,n0,32+8,inv) \
   CPRMMASK(zzop,2,fz,inv) /* runout, using mask */ \
  } \
 } \
 out0: natend=0; goto out;  out1: natend=1; goto out;  out2: natend=2; goto out;  out3: natend=3; goto out; \
 out4: natend=4; goto out;  out5: natend=5; goto out;  out6: natend=6; goto out;  out7: natend=7; out: \
 n2<<=3; orign2<<=3; orign2+=natend+backoff+1; \
 outs0: orign2-=n2; orign2<<=LGNPAR; orign2+=(((fz)&0x1000?inv?0x4322111100000000:0x0000000011112234:inv?0x4010201030102010:0x0102010301020104)>>(maskatend<<2))&7; if((fz)&0x1000){orign2=n0-1-orign2; orign2=orign2<0?n0:orign2;} R sc(orign2); \
 /* add accums, which give negative result.  Convert to positive; if inv, that's the result, otherwise subtract from i0 */ \
 outs1: acc0=_mm256_add_epi64(acc0,acc1); acc2=_mm256_add_epi64(acc2,acc3); acc4=_mm256_add_epi64(acc4,acc5); acc6=_mm256_add_epi64(acc6,acc7); acc0=_mm256_add_epi64(acc0,acc2); acc4=_mm256_add_epi64(acc4,acc6); acc0=_mm256_add_epi64(acc0,acc4); \
  acc0=_mm256_add_epi64(acc0,_mm256_permute4x64_epi64(acc0,0b11111110)); acc0=_mm256_add_epi64(acc0,_mm256_castpd_si256(_mm256_permute_pd(_mm256_castsi256_pd(acc0),0x0f))); \
  orign2 = _mm256_extract_epi64(acc0,0); R sc(inv?-orign2:n0+orign2); \
}


#define INDF(f,T0,T1,F)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                      \
  av=(T0*)AV(a);                                                    \
  wv=(T1*)AV(w);             \
  if     (!AR(a)){x=*av;        DP(n=AN(w),          y=*wv++; if(F(x,y))R sc(n+i););} \
  else if(!AR(w)){y=*wv;        DP(n=AN(a), x=*av++;          if(F(x,y))R sc(n+i););} \
  else           {              DP(n=AN(w), x=*av++; y=*wv++; if(F(x,y))R sc(n+i););} \
  R sc(n);                                                                    \
 }
#define INDF0(f,T0,T1,F,C)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                      \
  av=(T0*)AV(a);                                                    \
  wv=(T1*)AV(w);             \
  if     (!AR(a)){x=*av;        if(1.0==jt->cct)DP(n=AN(w),          y=*wv++; if(C(x,y))R sc(n+i);)else DP(n=AN(w),          y=*wv++; if(F(x,y))R sc(n+i);)} \
  else if(!AR(w)){y=*wv;        if(1.0==jt->cct)DP(n=AN(a), x=*av++;          if(C(x,y))R sc(n+i);)else DP(n=AN(a), x=*av++;          if(F(x,y))R sc(n+i);)} \
  else           {              if(1.0==jt->cct)DP(n=AN(w), x=*av++; y=*wv++; if(C(x,y))R sc(n+i);)else DP(n=AN(w), x=*av++; y=*wv++; if(F(x,y))R sc(n+i);)} \
  R sc(n);                                                                    \
 }

#define JNDF(f,T0,T1,F)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                          \
  av=(T0*)AV(a);                                                        \
  wv=(T1*)AV(w);                  \
  if     (!AR(a)){x=*av; wv+=AN(w); DQ(n=AN(w),          y=*--wv; if(F(x,y))R sc(i););} \
  else if(!AR(w)){y=*wv; av+=AN(a); DQ(n=AN(a), x=*--av;          if(F(x,y))R sc(i););} \
  else           {av+=AN(w); wv+=AN(w); DQ(n=AN(w), x=*--av; y=*--wv; if(F(x,y))R sc(i););} \
  R sc(n);                                                                        \
 }
#define JNDF0(f,T0,T1,F,C)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                          \
  av=(T0*)AV(a);                                                        \
  wv=(T1*)AV(w);                  \
  if     (!AR(a)){x=*av; wv+=AN(w); if(1.0==jt->cct)DQ(n=AN(w),          y=*--wv; if(C(x,y))R sc(i);)else DQ(n=AN(w),          y=*--wv; if(F(x,y))R sc(i);)} \
  else if(!AR(w)){y=*wv; av+=AN(a); if(1.0==jt->cct)DQ(n=AN(a), x=*--av;          if(C(x,y))R sc(i);)else DQ(n=AN(a), x=*--av;          if(F(x,y))R sc(i);)} \
  else           {av+=AN(w); wv+=AN(w); if(1.0==jt->cct)DQ(n=AN(w), x=*--av; y=*--wv; if(C(x,y))R sc(i);)else DQ(n=AN(w), x=*--av; y=*--wv; if(F(x,y))R sc(i);)} \
  R sc(n);                                                                        \
 }

#define SUMF(f,T0,T1,F)  \
 static F2(f){I m=0,n;T0*av,x;T1*wv,y;                       \
  av=(T0*)AV(a);                                         \
  wv=(T1*)AV(w);   \
  if     (!AR(a)){x=*av; DQ(n=AN(w),          y=*wv++; m+=(F(x,y)););} \
  else if(!AR(w)){y=*wv; DQ(n=AN(a), x=*av++;          m+=(F(x,y)););} \
  else           {       DQ(n=AN(w), x=*av++; y=*wv++; m+=(F(x,y)););} \
  R sc(m);                                                         \
 }
#define SUMF0(f,T0,T1,F,C)  \
 static F2(f){I m=0,n;T0*av,x;T1*wv,y;                       \
  av=(T0*)AV(a);                                         \
  wv=(T1*)AV(w);   \
  if     (!AR(a)){x=*av; if(1.0==jt->cct)DQ(n=AN(w),          y=*wv++; m+=(C(x,y));)else DQ(n=AN(w),          y=*wv++; m+=(F(x,y));)} \
  else if(!AR(w)){y=*wv; if(1.0==jt->cct)DQ(n=AN(a), x=*av++;          m+=(C(x,y));)else DQ(n=AN(a), x=*av++;          m+=(F(x,y));)} \
  else           {       if(1.0==jt->cct)DQ(n=AN(w), x=*av++; y=*wv++; m+=(C(x,y));)else DQ(n=AN(w), x=*av++; y=*wv++; m+=(F(x,y));)} \
  R sc(m);                                                         \
 }


/* Now define byte-parallel (word at a time) versions of above  */

#define JNDBR(yy)      if(r&&(y=yy))DO(r, if(yv[r-1-i])R sc(n-1-i););

#define ASSIGNX(v)     {x=*(C*)v; x|=x<<8; x|=x<<16; x|=x<<(32&(BW-1)); }
#define INDB3          {n=(UI)n>i*(UI)SZI+(CTTZI(y)>>LGBB)?i*SZI+(CTTZI(y)>>LGBB):n; break;}
#define JNDB3          {UI4 bitno; CTLZI(y,bitno); n=(i*SZI+(bitno>>LGBB)); break;}

#define INDB(f,T0,T1,F)  \
 static F2(f){I an,*av,n,q,wn,*wv,x,y;                                 \
  an=AN(a); av=AV(a);                                                             \
  wn=AN(w); wv=AV(w); n=1; n=AR(a)?an:n; n=AR(w)?wn:n;                      \
  q=(n+(SZI-1))>>LGSZI;                                           \
  if     (!AR(a)){ASSIGNX(av); DO(q, if(y=F(x,    *wv))INDB3; ++wv;);}  \
  else if(!AR(w)){ASSIGNX(wv); DO(q, if(y=F(*av,x    ))INDB3; ++av;);}  \
  else           {             DO(q, if(y=F(*av,*wv))INDB3; ++av; ++wv;);}  \
  R sc(n);                                                                        \
 }

#define JNDB(f,T0,T1,F)  \
 static F2(f){I an,*av,n,q,r,wn,*wv,x,y;                                             \
  an=AN(a); av=AV(a);                                                                         \
  wn=AN(w); wv=AV(w); n=1; n=AR(a)?an:n; n=AR(w)?wn:n;                      \
  if((q=(n-1)>>LGSZI)<0)R zeroionei(0); r=((n-1)&(SZI-1));  /* # first bytes to do minus 1 */                                                                           \
  I i=q;                                                                       \
  if     (!AR(a)){ASSIGNX(av); wv+=q; y=(F(x,*wv))&(((I)0x100<<(r<<3))-1); while(1){if(y)JNDB3; if(--i<0)break; --wv; y=F(x,*wv);} }  \
  else if(!AR(w)){ASSIGNX(wv); av+=q; y=(F(*av,x))&(((I)0x100<<(r<<3))-1); while(1){if(y)JNDB3; if(--i<0)break; --av; y=F(*av,x);} }  \
  else           {av+=q;       wv+=q; y=(F(*av,*wv))&(((I)0x100<<(r<<3))-1); while(1){if(y)JNDB3; if(--i<0)break; --av; --wv; y=F(*av,*wv);} }  \
  R sc(n);                                                                                    \
 }

#define SUMB(f,T0,T1,F)  \
 static F2(f){I an,*av,n,p,r1,wn,*wv,z=0;UI t,x;              \
  an=AN(a); av=AV(a);                                                        \
  wn=AN(w); wv=AV(w); n=1; n=AR(a)?an:n; n=AR(w)?wn:n;                      \
  p=n>>LGSZI; r1=n&(SZI-1);                                       \
  if     (!AR(a)){                                                           \
   ASSIGNX(av);                                                              \
   NOUNROLL while((p-=255)>0){t=0; DQ(255, t+=F(x,    *wv); ++wv;); ADDBYTESINI(t); z+=t;}              \
         t=0; DQ(p+255,   t+=F(x,    *wv); ++wv;); ADDBYTESINI(t); z+=t; x=F(x,  *wv);  \
  }else if(!AR(w)){                                                          \
   ASSIGNX(wv);                                                              \
   NOUNROLL while((p-=255)>0){t=0; DQ(255, t+=F(*av,x    ); ++av;); ADDBYTESINI(t); z+=t;}              \
         t=0; DQ(p+255,   t+=F(*av,x    ); ++av;); ADDBYTESINI(t); z+=t; x=F(*av,x  );  \
  }else{                                                                     \
   NOUNROLL while((p-=255)>0){t=0; DQ(255, t+=F(*av,*wv); ++av; ++wv;); ADDBYTESINI(t); z+=t;}              \
         t=0; DQ(p+255,   t+=F(*av,*wv); ++av; ++wv;); ADDBYTESINI(t); z+=t; x=F(*av,*wv);  \
  }                                                                          \
  x &= ((I)1<<(r1<<LGBB))-1; ADDBYTESINI(x); z+=x;    /* C_LE */                                                       \
  R sc(z);                                                                   \
 }


#if (C_AVX&&SY_64) || EMU_AVX
#define XCTL0(nm0) if(jt->cct==1.0)R nm0(jt,a,w); __m256d cct=_mm256_broadcast_sd(&jt->cct);  // xfer to intolerant if called for
#define ZZTEQ zz=_mm256_xor_pd(_mm256_cmp_pd(xx,_mm256_mul_pd(yy,cct),_CMP_GT_OQ),_mm256_cmp_pd(yy,_mm256_mul_pd(xx,cct),_CMP_LE_OQ));  // tolerant =
#define NOTZZ  zz=_mm256_xor_pd(zz,ones)

// =
cprimop256(i0eqII,0x0001,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(i0eqBI,0x0040,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); ,1)
cprimop256(i0eqIB,0x0080,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); ,1)
cprimop256(i0eqDD0,0x0001,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(i0eqDD,0x0001,XCTL0(i0eqDD0), ZZTEQ ,1)
cprimop256(i0eqBD0,0x0100,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(i0eqBD,0x0100,XCTL0(i0eqBD0), ZZTEQ ,1)
cprimop256(i0eqID0,0x0008,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(i0eqID,0x0008,XCTL0(i0eqID0), ZZTEQ ,1)
cprimop256(i0eqDB0,0x0200,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(i0eqDB,0x0200,XCTL0(i0eqDB0), ZZTEQ ,1)
cprimop256(i0eqDI0,0x0010,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(i0eqDI,0x0010,XCTL0(i0eqDI0), ZZTEQ ,1)
cprimop256(j0eqII,0x1001,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(j0eqBI,0x1040,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); ,1)
cprimop256(j0eqIB,0x1080,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); ,1)
cprimop256(j0eqDD0,0x1001,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(j0eqDD,0x1001,XCTL0(j0eqDD0), ZZTEQ ,1)
cprimop256(j0eqBD0,0x1100,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(j0eqBD,0x1100,XCTL0(j0eqBD0), ZZTEQ ,1)
cprimop256(j0eqID0,0x1008,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(j0eqID,0x1008,XCTL0(j0eqID0), ZZTEQ ,1)
cprimop256(j0eqDB0,0x1200,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(j0eqDB,0x1200,XCTL0(j0eqDB0), ZZTEQ ,1)
cprimop256(j0eqDI0,0x1010,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(j0eqDI,0x1010,XCTL0(j0eqDI0), ZZTEQ ,1)
cprimop256(sumeqII,0x2001,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(sumeqBI,0x2040,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); ,1)
cprimop256(sumeqIB,0x2080,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); ,1)
cprimop256(sumeqDD0,0x2001,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(sumeqDD,0x2001,XCTL0(sumeqDD0), ZZTEQ ,1)
cprimop256(sumeqBD0,0x2100,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(sumeqBD,0x2100,XCTL0(sumeqBD0), ZZTEQ ,1)
cprimop256(sumeqID0,0x2008,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(sumeqID,0x2008,XCTL0(sumeqID0), ZZTEQ ,1)
cprimop256(sumeqDB0,0x2200,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(sumeqDB,0x2200,XCTL0(sumeqDB0), ZZTEQ ,1)
cprimop256(sumeqDI0,0x2010,,zz=_mm256_cmp_pd(xx,yy,_CMP_NEQ_OQ),0)
cprimop256(sumeqDI,0x2010,XCTL0(sumeqDI0), ZZTEQ ,1)

// ~:
cprimop256(i0neII,0x0001,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(i0neBI,0x0040,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(i0neIB,0x0080,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(i0neDD0,0x0001,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(i0neDD,0x0001,XCTL0(i0neDD0), ZZTEQ,0)
cprimop256(i0neBD0,0x0100,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(i0neBD,0x0100,XCTL0(i0neBD0), ZZTEQ,0)
cprimop256(i0neID0,0x0008,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(i0neID,0x0008,XCTL0(i0neID0), ZZTEQ,0)
cprimop256(i0neDB0,0x0200,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(i0neDB,0x0200,XCTL0(i0neDB0), ZZTEQ,0)
cprimop256(i0neDI0,0x0010,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(i0neDI,0x0010,XCTL0(i0neDI0), ZZTEQ,0)
cprimop256(j0neII,0x1001,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(j0neBI,0x1040,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(j0neIB,0x1080,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(j0neDD0,0x1001,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(j0neDD,0x1001,XCTL0(j0neDD0), ZZTEQ,0)
cprimop256(j0neBD0,0x1100,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(j0neBD,0x1100,XCTL0(j0neBD0), ZZTEQ,0)
cprimop256(j0neID0,0x1008,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(j0neID,0x1008,XCTL0(j0neID0), ZZTEQ,0)
cprimop256(j0neDB0,0x1200,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(j0neDB,0x1200,XCTL0(j0neDB0), ZZTEQ,0)
cprimop256(j0neDI0,0x1010,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(j0neDI,0x1010,XCTL0(j0neDI0), ZZTEQ,0)
cprimop256(sumneII,0x2001,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(sumneBI,0x2040,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(sumneIB,0x2080,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(sumneDD0,0x2001,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(sumneDD,0x2001,XCTL0(sumneDD0), ZZTEQ,0)
cprimop256(sumneBD0,0x2100,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(sumneBD,0x2100,XCTL0(sumneBD0), ZZTEQ,0)
cprimop256(sumneID0,0x2008,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(sumneID,0x2008,XCTL0(sumneID0), ZZTEQ,0)
cprimop256(sumneDB0,0x2200,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(sumneDB,0x2200,XCTL0(sumneDB0), ZZTEQ,0)
cprimop256(sumneDI0,0x2010,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0)
cprimop256(sumneDI,0x2010,XCTL0(sumneDI0), ZZTEQ,0)

// <
cprimop256(i0ltII,0x0000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(i0ltBI,0x0040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(i0ltIB,0x0080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(i0ltDD0,0x0000,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(i0ltDD,0x0000,XCTL0(i0ltDD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0ltBD0,0x0100,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(i0ltBD,0x0100,XCTL0(i0ltBD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0ltID0,0x0008,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(i0ltID,0x0008,XCTL0(i0ltID0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0ltDB0,0x0200,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(i0ltDB,0x0200,XCTL0(i0ltDB0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0ltDI0,0x0010,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(i0ltDI,0x0010,XCTL0(i0ltDI0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0ltII,0x1000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(j0ltBI,0x1040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(j0ltIB,0x1080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(j0ltDD0,0x1000,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(j0ltDD,0x1000,XCTL0(j0ltDD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0ltBD0,0x1100,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(j0ltBD,0x1100,XCTL0(j0ltBD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0ltID0,0x1008,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(j0ltID,0x1008,XCTL0(j0ltID0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0ltDB0,0x1200,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(j0ltDB,0x1200,XCTL0(j0ltDB0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0ltDI0,0x1010,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(j0ltDI,0x1010,XCTL0(j0ltDI0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumltII,0x2000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(sumltBI,0x2040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(sumltIB,0x2080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,1)
cprimop256(sumltDD0,0x2000,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(sumltDD,0x2000,XCTL0(sumltDD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumltBD0,0x2100,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(sumltBD,0x2100,XCTL0(sumltBD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumltID0,0x2008,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(sumltID,0x2008,XCTL0(sumltID0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumltDB0,0x2200,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(sumltDB,0x2200,XCTL0(sumltDB0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumltDI0,0x2010,,zz=_mm256_cmp_pd(xx,yy,_CMP_GE_OQ),0)
cprimop256(sumltDI,0x2010,XCTL0(sumltDI0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)

// <:
cprimop256(i0leII,0x0000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(i0leBI,0x0040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(i0leIB,0x0080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(i0leDD0,0x0000,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(i0leDD,0x0000,XCTL0(i0leDD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0leBD0,0x0100,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(i0leBD,0x0100,XCTL0(i0leBD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0leID0,0x0008,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(i0leID,0x0008,XCTL0(i0leID0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0leDB0,0x0200,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(i0leDB,0x0200,XCTL0(i0leDB0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(i0leDI0,0x0010,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(i0leDI,0x0010,XCTL0(i0leDI0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0leII,0x1000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(j0leBI,0x1040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(j0leIB,0x1080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(j0leDD0,0x1000,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(j0leDD,0x1000,XCTL0(j0leDD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0leBD0,0x1100,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(j0leBD,0x1100,XCTL0(j0leBD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0leID0,0x1008,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(j0leID,0x1008,XCTL0(j0leID0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0leDB0,0x1200,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(j0leDB,0x1200,XCTL0(j0leDB0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(j0leDI0,0x1010,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(j0leDI,0x1010,XCTL0(j0leDI0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumleII,0x2000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(sumleBI,0x2040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(sumleIB,0x2080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0)
cprimop256(sumleDD0,0x2000,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(sumleDD,0x2000,XCTL0(sumleDD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumleBD0,0x2100,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(sumleBD,0x2100,XCTL0(sumleBD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumleID0,0x2008,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(sumleID,0x2008,XCTL0(sumleID0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumleDB0,0x2200,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(sumleDB,0x2200,XCTL0(sumleDB0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)
cprimop256(sumleDI0,0x2010,,zz=_mm256_cmp_pd(xx,yy,_CMP_GT_OQ),0)
cprimop256(sumleDI,0x2010,XCTL0(sumleDI0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_LT_OQ)),1)

// >:
cprimop256(i0geII,0x0000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(i0geBI,0x0040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(i0geIB,0x0080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(i0geDD0,0x0000,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(i0geDD,0x0000,XCTL0(i0geDD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0geBD0,0x0100,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(i0geBD,0x0100,XCTL0(i0geBD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0geID0,0x0008,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(i0geID,0x0008,XCTL0(i0geID0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0geDB0,0x0200,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(i0geDB,0x0200,XCTL0(i0geDB0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0geDI0,0x0010,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(i0geDI,0x0010,XCTL0(i0geDI0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0geII,0x1000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(j0geBI,0x1040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(j0geIB,0x1080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(j0geDD0,0x1000,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(j0geDD,0x1000,XCTL0(j0geDD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0geBD0,0x1100,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(j0geBD,0x1100,XCTL0(j0geBD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0geID0,0x1008,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(j0geID,0x1008,XCTL0(j0geID0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0geDB0,0x1200,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(j0geDB,0x1200,XCTL0(j0geDB0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0geDI0,0x1010,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(j0geDI,0x1010,XCTL0(j0geDI0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgeII,0x2000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(sumgeBI,0x2040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(sumgeIB,0x2080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx)));,0)
cprimop256(sumgeDD0,0x2000,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(sumgeDD,0x2000,XCTL0(sumgeDD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgeBD0,0x2100,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(sumgeBD,0x2100,XCTL0(sumgeBD0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgeID0,0x2008,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(sumgeID,0x2008,XCTL0(sumgeID0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgeDB0,0x2200,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(sumgeDB,0x2200,XCTL0(sumgeDB0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgeDI0,0x2010,,zz=_mm256_cmp_pd(xx,yy,_CMP_LT_OQ),0)
cprimop256(sumgeDI,0x2010,XCTL0(sumgeDI0), ZZTEQ zz=_mm256_or_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)

// >
cprimop256(i0gtII,0x0000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(i0gtBI,0x0040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(i0gtIB,0x0080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(i0gtDD0,0x0000,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(i0gtDD,0x0000,XCTL0(i0gtDD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0gtBD0,0x0100,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(i0gtBD,0x0100,XCTL0(i0gtBD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0gtID0,0x0008,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(i0gtID,0x0008,XCTL0(i0gtID0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0gtDB0,0x0200,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(i0gtDB,0x0200,XCTL0(i0gtDB0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(i0gtDI0,0x0010,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(i0gtDI,0x0010,XCTL0(i0gtDI0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0gtII,0x1000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(j0gtBI,0x1040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(j0gtIB,0x1080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(j0gtDD0,0x1000,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(j0gtDD,0x1000,XCTL0(j0gtDD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0gtBD0,0x1100,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(j0gtBD,0x1100,XCTL0(j0gtBD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0gtID0,0x1008,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(j0gtID,0x1008,XCTL0(j0gtID0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0gtDB0,0x1200,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(j0gtDB,0x1200,XCTL0(j0gtDB0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(j0gtDI0,0x1010,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(j0gtDI,0x1010,XCTL0(j0gtDI0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgtII,0x2000,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(sumgtBI,0x2040,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(sumgtIB,0x2080,,zz=_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,1)
cprimop256(sumgtDD0,0x2000,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(sumgtDD,0x2000,XCTL0(sumgtDD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgtBD0,0x2100,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(sumgtBD,0x2100,XCTL0(sumgtBD0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgtID0,0x2008,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(sumgtID,0x2008,XCTL0(sumgtID0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgtDB0,0x2200,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(sumgtDB,0x2200,XCTL0(sumgtDB0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)
cprimop256(sumgtDI0,0x2010,,zz=_mm256_cmp_pd(xx,yy,_CMP_LE_OQ),0)
cprimop256(sumgtDI,0x2010,XCTL0(sumgtDI0), ZZTEQ zz=_mm256_andnot_pd(zz,_mm256_cmp_pd(xx,yy,_CMP_GT_OQ)),1)

#else
INDF( i0eqII,I,I,ANE  ) INDF( i0eqIB,I,B,ANE  )   INDF( i0eqBI,B,I,ANE  )  INDF0( i0eqDD,D,D,TNE,NE0  )
INDF0( i0eqBD,B,D,TNEXD,NEXD0) INDF0( i0eqID,I,D,TNEXD,NEXD0) INDF0( i0eqDB,D,B,TNEDX,NEDX0)  INDF0( i0eqDI,D,I,TNEDX,NEDX0)
JNDF( j0eqII,I,I,ANE  )   JNDF( j0eqBI,B,I,ANE  )  JNDF0( j0eqBD,B,D,TNEXD,NEXD0)
JNDF( j0eqIB,I,B,ANE  )   JNDF0( j0eqID,I,D,TNEXD,NEXD0)
JNDF0( j0eqDB,D,B,TNEDX,NEDX0)  JNDF0( j0eqDI,D,I,TNEDX,NEDX0)  JNDF0( j0eqDD,D,D,TNE,NE0  )
 INDF( i0neBI,B,I,AEQ  )  INDF0( i0neBD,B,D,TEQXD,EQXD0)  /* ~: */
INDF( i0neIB,I,B,AEQ  )  INDF( i0neII,I,I,AEQ  )  INDF0( i0neID,I,D,TEQXD,EQXD0)
INDF0( i0neDB,D,B,TEQDX,EQDX0)  INDF0( i0neDI,D,I,TEQDX,EQDX0)  INDF0( i0neDD,D,D,TEQ,EQ0  )
  JNDF( j0neBI,B,I,AEQ  )  JNDF0( j0neBD,B,D,TEQXD,EQXD0)
JNDF( j0neIB,I,B,AEQ  )  JNDF( j0neII,I,I,AEQ  )  JNDF0( j0neID,I,D,TEQXD,EQXD0)
JNDF0( j0neDB,D,B,TEQDX,EQDX0)  JNDF0( j0neDI,D,I,TEQDX,EQDX0)  JNDF0( j0neDD,D,D,TEQ,EQ0  )
  INDF( i0ltBI,B,I,AGE  )  INDF0( i0ltBD,B,D,TGEXD,GEXD0)  /* <  */
INDF( i0ltIB,I,B,AGE  )  INDF( i0ltII,I,I,AGE  )  INDF0( i0ltID,I,D,TGEXD,GEXD0)
INDF0( i0ltDB,D,B,TGEDX,GEDX0)  INDF0( i0ltDI,D,I,TGEDX,GEDX0)  INDF0( i0ltDD,D,D,TGE,GE0  )
  JNDF( j0ltBI,B,I,AGE  )  JNDF0( j0ltBD,B,D,TGEXD,GEXD0)
JNDF( j0ltIB,I,B,AGE  )  JNDF( j0ltII,I,I,AGE  )  JNDF0( j0ltID,I,D,TGEXD,GEXD0)
JNDF0( j0ltDB,D,B,TGEDX,GEDX0)  JNDF0( j0ltDI,D,I,TGEDX,GEDX0)  JNDF0( j0ltDD,D,D,TGE,GE0  )
  INDF( i0leBI,B,I,AGT  )  INDF0( i0leBD,B,D,TGTXD,GTXD0)  /* <: */
INDF( i0leIB,I,B,AGT  )  INDF( i0leII,I,I,AGT  )  INDF0( i0leID,I,D,TGTXD,GTXD0)
INDF0( i0leDB,D,B,TGTDX,GTDX0)  INDF0( i0leDI,D,I,TGTDX,GTDX0)  INDF0( i0leDD,D,D,TGT,GT0  )
  JNDF( j0leBI,B,I,AGT  )  JNDF0( j0leBD,B,D,TGTXD,GTXD0)
JNDF( j0leIB,I,B,AGT  )  JNDF( j0leII,I,I,AGT  )  JNDF0( j0leID,I,D,TGTXD,GTXD0)
JNDF0( j0leDB,D,B,TGTDX,GTDX0)  JNDF0( j0leDI,D,I,TGTDX,GTDX0)  JNDF0( j0leDD,D,D,TGT,GT0  )
  INDF( i0geBI,B,I,ALT  )  INDF0( i0geBD,B,D,TLTXD,LTXD0)  /* >: */
INDF( i0geIB,I,B,ALT  )  INDF( i0geII,I,I,ALT  )  INDF0( i0geID,I,D,TLTXD,LTXD0)
INDF0( i0geDB,D,B,TLTDX,LTDX0)  INDF0( i0geDI,D,I,TLTDX,LTDX0)  INDF0( i0geDD,D,D,TLT ,LT0 )
  JNDF( j0geBI,B,I,ALT  )  JNDF0( j0geBD,B,D,TLTXD,LTXD0)
JNDF( j0geIB,I,B,ALT  )  JNDF( j0geII,I,I,ALT  )  JNDF0( j0geID,I,D,TLTXD,LTXD0)
JNDF0( j0geDB,D,B,TLTDX,LTDX0)  JNDF0( j0geDI,D,I,TLTDX,LTDX0)  JNDF0( j0geDD,D,D,TLT,LT0  )
  INDF( i0gtBI,B,I,ALE  )  INDF0( i0gtBD,B,D,TLEXD,LEXD0)  /* >  */
INDF( i0gtIB,I,B,ALE  )  INDF( i0gtII,I,I,ALE  )  INDF0( i0gtID,I,D,TLEXD,LEXD0)
INDF0( i0gtDB,D,B,TLEDX,LEDX0)  INDF0( i0gtDI,D,I,TLEDX,LEDX0)  INDF0( i0gtDD,D,D,TLE,LE0  )
  JNDF( j0gtBI,B,I,ALE  )  JNDF0( j0gtBD,B,D,TLEXD,LEXD0)
JNDF( j0gtIB,I,B,ALE  )  JNDF( j0gtII,I,I,ALE  )  JNDF0( j0gtID,I,D,TLEXD,LEXD0)
JNDF0( j0gtDB,D,B,TLEDX,LEDX0)  JNDF0( j0gtDI,D,I,TLEDX,LEDX0)  JNDF0( j0gtDD,D,D,TLE,LE0  )
SUMF(sumeqII,I,I,AEQ  ) SUMF(sumeqBI,B,I,AEQ  )  SUMF0(sumeqBD,B,D,TEQXD,EQXD0)
SUMF(sumeqIB,I,B,AEQ  )    SUMF0(sumeqID,I,D,TEQXD,EQXD0)
SUMF0(sumeqDB,D,B,TEQDX,EQDX0)  SUMF0(sumeqDI,D,I,TEQDX,EQDX0)  SUMF0(sumeqDD,D,D,TEQ,EQ0  )
  SUMF(sumneBI,B,I,ANE  )  SUMF0(sumneBD,B,D,TNEXD,NEXD0)
SUMF(sumneIB,I,B,ANE  )  SUMF(sumneII,I,I,ANE  )  SUMF0(sumneID,I,D,TNEXD,NEXD0)
SUMF0(sumneDB,D,B,TNEDX,NEDX0)  SUMF0(sumneDI,D,I,TNEDX,NEDX0)  SUMF0(sumneDD,D,D,TNE,NE0  )
  SUMF(sumltBI,B,I,ALT  )  SUMF0(sumltBD,B,D,TLTXD,LTXD0)
SUMF(sumltIB,I,B,ALT  )  SUMF(sumltII,I,I,ALT  )  SUMF0(sumltID,I,D,TLTXD,LTXD0)
SUMF0(sumltDB,D,B,TLTDX,LTDX0)  SUMF0(sumltDI,D,I,TLTDX,LTDX0)  SUMF0(sumltDD,D,D,TLT,LT0  )
 SUMF(sumleBI,B,I,ALE  )  SUMF0(sumleBD,B,D,TLEXD,LEXD0)
SUMF(sumleIB,I,B,ALE  )  SUMF(sumleII,I,I,ALE  )  SUMF0(sumleID,I,D,TLEXD,LEXD0)
SUMF0(sumleDB,D,B,TLEDX,LEDX0)  SUMF0(sumleDI,D,I,TLEDX,LEDX0)  SUMF0(sumleDD,D,D,TLE,LE0  )
  SUMF(sumgeBI,B,I,AGE  )  SUMF0(sumgeBD,B,D,TGEXD,GEXD0)
SUMF(sumgeIB,I,B,AGE  )  SUMF(sumgeII,I,I,AGE  )  SUMF0(sumgeID,I,D,TGEXD,GEXD0)
SUMF0(sumgeDB,D,B,TGEDX,GEDX0)  SUMF0(sumgeDI,D,I,TGEDX,GEDX0)  SUMF0(sumgeDD,D,D,TGE,GE0  )
 SUMF(sumgtBI,B,I,AGT  )  SUMF0(sumgtBD,B,D,TGTXD,GTXD0)
SUMF(sumgtIB,I,B,AGT  )  SUMF(sumgtII,I,I,AGT  )  SUMF0(sumgtID,I,D,TGTXD,GTXD0)
SUMF0(sumgtDB,D,B,TGTDX,GTDX0)  SUMF0(sumgtDI,D,I,TGTDX,GTDX0)  SUMF0(sumgtDD,D,D,TGT,GT0  )
#endif

INDB( i0eqBB,B,B,NE   )  


JNDB( j0eqBB,B,B,NE   )

SUMB(sumeqBB,B,B,EQ   ) 

INDB( i0neBB,B,B,EQ   ) 

JNDB( j0neBB,B,B,EQ   )

SUMB(sumneBB,B,B,NE   )
INDB( i0ltBB,B,B,GE   )

JNDB( j0ltBB,B,B,GE   )

SUMB(sumltBB,B,B,LT   )
INDB( i0leBB,B,B,GT   )

JNDB( j0leBB,B,B,GT   )
SUMB(sumleBB,B,B,LE   ) 
INDB( i0geBB,B,B,LT   )

JNDB( j0geBB,B,B,LT   )

SUMB(sumgeBB,B,B,GE   )
INDB( i0gtBB,B,B,LE   )

JNDB( j0gtBB,B,B,LE   )

SUMB(sumgtBB,B,B,GT   ) 
static AF atcompxy[]={  /* table for (B01,INT,FL) vs. (B01,INT,FL) */
 i0eqBB, i0eqBI, i0eqBD,   i0eqIB, i0eqII, i0eqID,   i0eqDB, i0eqDI, i0eqDD,   /* 0 */
 i0neBB, i0neBI, i0neBD,   i0neIB, i0neII, i0neID,   i0neDB, i0neDI, i0neDD,
 i0ltBB, i0ltBI, i0ltBD,   i0ltIB, i0ltII, i0ltID,   i0ltDB, i0ltDI, i0ltDD,
 i0leBB, i0leBI, i0leBD,   i0leIB, i0leII, i0leID,   i0leDB, i0leDI, i0leDD,
 i0geBB, i0geBI, i0geBD,   i0geIB, i0geII, i0geID,   i0geDB, i0geDI, i0geDD,
 i0gtBB, i0gtBI, i0gtBD,   i0gtIB, i0gtII, i0gtID,   i0gtDB, i0gtDI, i0gtDD,
 j0eqBB, j0eqBI, j0eqBD,   j0eqIB, j0eqII, j0eqID,   j0eqDB, j0eqDI, j0eqDD,   /* 2 */
 j0neBB, j0neBI, j0neBD,   j0neIB, j0neII, j0neID,   j0neDB, j0neDI, j0neDD,
 j0ltBB, j0ltBI, j0ltBD,   j0ltIB, j0ltII, j0ltID,   j0ltDB, j0ltDI, j0ltDD,
 j0leBB, j0leBI, j0leBD,   j0leIB, j0leII, j0leID,   j0leDB, j0leDI, j0leDD,
 j0geBB, j0geBI, j0geBD,   j0geIB, j0geII, j0geID,   j0geDB, j0geDI, j0geDD,
 j0gtBB, j0gtBI, j0gtBD,   j0gtIB, j0gtII, j0gtID,   j0gtDB, j0gtDI, j0gtDD,
 sumeqBB,sumeqBI,sumeqBD,  sumeqIB,sumeqII,sumeqID,  sumeqDB,sumeqDI,sumeqDD,  /* 4 */
 sumneBB,sumneBI,sumneBD,  sumneIB,sumneII,sumneID,  sumneDB,sumneDI,sumneDD,
 sumltBB,sumltBI,sumltBD,  sumltIB,sumltII,sumltID,  sumltDB,sumltDI,sumltDD,
 sumleBB,sumleBI,sumleBD,  sumleIB,sumleII,sumleID,  sumleDB,sumleDI,sumleDD,
 sumgeBB,sumgeBI,sumgeBD,  sumgeIB,sumgeII,sumgeID,  sumgeDB,sumgeDI,sumgeDD,
 sumgtBB,sumgtBI,sumgtBD,  sumgtIB,sumgtII,sumgtID,  sumgtDB,sumgtDI,sumgtDD,
};

INDB( i0eqC,C,C,ACNE)  INDB( i0neC,C,C,ACEQ)
JNDB( j0eqC,C,C,ACNE)  JNDB( j0neC,C,C,ACEQ)
SUMB(sumeqC,C,C,ACEQB)  SUMB(sumneC,C,C,ACNEB)

static AF atcompC[]={   /* table for LIT vs. LIT */
  i0eqC,  i0neC, 0L,0L,0L,0L,
  j0eqC,  j0neC, 0L,0L,0L,0L,
 sumeqC, sumneC, 0L,0L,0L,0L,
};

INDF( i0eqUS,US,US,ANE)  INDF( i0neUS,US,US,AEQ)
JNDF( j0eqUS,US,US,ANE)  JNDF( j0neUS,US,US,AEQ)
SUMF(sumeqUS,US,US,AEQ)  SUMF(sumneUS,US,US,ANE)

static AF atcompUS[]={   /* table for C2T vs. C2T */
  i0eqUS,  i0neUS, 0L,0L,0L,0L,
  j0eqUS,  j0neUS, 0L,0L,0L,0L,
 sumeqUS, sumneUS, 0L,0L,0L,0L,
};

INDF( i0eqC4,C4,C4,ANE)  INDF( i0neC4,C4,C4,AEQ)
JNDF( j0eqC4,C4,C4,ANE)  JNDF( j0neC4,C4,C4,AEQ)
SUMF(sumeqC4,C4,C4,AEQ)  SUMF(sumneC4,C4,C4,ANE)

static AF atcompC4[]={   /* table for C4T vs. C4T */
  i0eqC4,  i0neC4, 0L,0L,0L,0L,
  j0eqC4,  j0neC4, 0L,0L,0L,0L,
 sumeqC4, sumneC4, 0L,0L,0L,0L,
};

INDF( i0eqS,SB,SB,ANE) INDF( i0neS,SB,SB,AEQ) 
JNDF( j0eqS,SB,SB,ANE) JNDF( j0neS,SB,SB,AEQ) 
SUMF(sumeqS,SB,SB,AEQ) SUMF(sumneS,SB,SB,ANE)

INDF( i0ltS,SB,SB,SBGE) INDF( i0leS,SB,SB,SBGT) INDF( i0geS,SB,SB,SBLT) INDF( i0gtS,SB,SB,SBLE)
JNDF( j0ltS,SB,SB,SBGE) JNDF( j0leS,SB,SB,SBGT) JNDF( j0geS,SB,SB,SBLT) JNDF( j0gtS,SB,SB,SBLE)
SUMF(sumltS,SB,SB,SBLT) SUMF(sumleS,SB,SB,SBLE) SUMF(sumgeS,SB,SB,SBGE) SUMF(sumgtS,SB,SB,SBGT)


static AF atcompSB[]={  /* table for SBT vs. SBT */
  i0eqS, i0neS, i0ltS, i0leS, i0geS, i0gtS,
  j0eqS, j0neS, j0ltS, j0leS, j0geS, j0gtS,
 sumeqS,sumneS,sumltS,sumleS,sumgeS,sumgtS,
};

// This table is indexed by m[5 4 3] but only a few combinations are generated
static AF atcompX[]={0L, jti1ebar, 0L, 0L, jtsumebar, jtanyebar};


// f (the comparison op) is bits 0-2 of m, the combining op is 3-5 of m
/*            f  0   1   2   3   4   5   6   7  */
/* m             =   ~:  <   <:  >:  >   E.  e. */
/* 0 f i. 0:     0   1   2   3   4   5   6   7  */
/* 1 f i. 1:     8   9  10  11  12  13  14  15  */
/* 2 f i: 0:    16  17  18  19  20  21  22  23  */
/* 3 f i: 1:    24  25  26  27  28  29  30  31  */
/* 4 [: + / f   32  33  34  35  36  37  38  39  */
/* 5 [: +./ f   40  41  42  43  44  45  46  47  */
/* 6 [: *./ f   48  49  50  51  52  53  54  55  */

// Here to choose the function to use, after the arguments are known.
// We require the ranks to be <2 for processing here except for @e., which requires that the result of e. have rank<2
// If the form is [: f/ g  and g is a simple comparison, use f/@g code for higher ranks
// If no routine found, return 0 to failover to normal path
// result bits 0-1 indicate postprocessing needed: 0x=none, 10=+./ (result is binary 0 if search completed), 11=*./ (result is binary 1 if search completed)
AF jtatcompf(J jt,A a,A w,A self){I m;
 ARGCHK2(a,w);
 m=FAV(self)->flag&255;
 if((m&6)!=6){   // normal comparison
  // verify rank is OK, based on operation
  if((AR(a)|AR(w))>1){R (m>=(4<<3))?(AF)jtfslashatg:0;}   // If an operand has rank>1, reject it unless it can be turned to f/@g special. postflags are 0
  ASSERT(AN(a)==AN(w)||((AR(a)&AR(w))==0),EVLENGTH)   // agreement is same length or one an atom - we know ranks<=1
  if(unlikely((-AN(a)&-AN(w))>=0))R0;  // if either arg empty, skip our loop
  // split m into search and comparison
  I search=m>>3; I comp=m&7;
  // Change +./ to i.&1, *./ to i.&0; save flag bits to include in return address
  I postflags=(0xc0>>search)&3; search=(0x0143210>>(search<<2))&15;  // flags: 00 00 00 00 00 10 11, rev/overlap to 11000000   search: 0 1 2 3 4 1 0
  // Change i.&1@:comp to i.&0@:compx, sim for i:  XOR comp with 000 001 000 110 000 110
  comp^=(0x606010>>(((search&1)+(comp&6))<<2))&7; search>>=1;  // complement comp if search is i&1; then the only search values are 0, 2, 4 so map them to 012.  Could reorder compares to = ~: < >: > <: to save code here
  if(!((AT(a)|AT(w))&((NOUN|SPARSE)&~(B01+INT+FL)))){
   // numeric types that we can handle here, for sure
   R (AF)((I)atcompxy[6*9*search+9*comp+3*(AT(a)>>INTX)+(AT(w)>>INTX)]+postflags);
  }
  // Other types have a chance only if they are equal types; fetch from the appropriate table then
  if(ISDENSETYPE(AT(a)&AT(w)|((AT(a)|AT(w))&SPARSE),LIT+C2T+C4T+SBT)){R (AF)((I)(AT(a)&LIT?atcompC:AT(a)&C2T?atcompUS:AT(a)&C4T?atcompC4:atcompSB)[6*search+comp]+postflags);}
  R 0;
 }else{  // E. (6) or e. (7)
  if(unlikely((AR(a)|AR(w))>1)){if(!(m&1)||AR(a)>(AR(w)?AR(w):1))R0;}  // some rank > 1, fail if E. or (e. returns rank>1)
  if(unlikely(((m&1)|(AN(a)-1))==0))R 0;  // E. when a is a singleton - no need for the full E. treatment
  if(likely(m&1))R jtcombineeps;  // e. types: direct i.-family types, go to routine to vector there; postflags are 0
  // all that's left is E.
  R atcompX[m>>3];  // choose i.-family routine; postflags are 0
 }
}    /* function table look-up for  comp i. 1:  and  i.&1@:comp  etc. */

