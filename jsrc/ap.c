/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Prefix and Infix                                               */

#include "j.h"
#include "vasm.h"
#include "ve.h"


#define ZZDEFN
#include "result.h"


#define MINUSPA(b,r,u,v)  r=b?u-v:u+v;
#define MINUSPZ(b,r,u,v)  if(b)r=zminus(u,v); else r=zplus(u,v);
#define MINUSPX(b,r,u,v)  if(b)r=xminus(u,v); else r=xplus(u,v);
#define MINUSPQ(b,r,u,v)  if(b)r=qminus(u,v); else r=qplus(u,v);

#define DIVPA(b,r,u,v)    r=b?(DIV(u,(D)v)):TYMES(u,v);
#define DIVPZ(b,r,u,v)    if(b)r=zdiv(u,v); else r=ztymes(u,v);

// Don't RESTRICT y since function may be called inplace
#define PREFIXPFX(f,Tz,Tx,pfx,vecfn,retstmt)  \
 AHDRP(f,Tz,Tx){I i;Tz v;                                 \
  if(d==1)DQ(m, *z++=v=    *x++; DQ(n-1, *z=v=pfx(v,*x); ++z; ++x;))  \
  else{for(i=0;i<m;++i){                                              \
   DO(d, z[i]=    x[i];); x+=d;                                        \
   DQ(n-1, vecfn(AH2ANP_v(d,z,x,z+d,jt)); z+=d; x+=d; ); z+=d;                     \
 }}retstmt}  /* for associative functions only */

#define PREFIXNAN(f,Tz,Tx,pfx,vecfn)  \
 AHDRP(f,Tz,Tx){I i;Tz v;                                \
  NAN0;                                                               \
  if(d==1)DQ(m, *z++=v=    *x++; DQ(n-1, *z=v=pfx(v,*x); ++z; ++x;))  \
  else{for(i=0;i<m;++i){                                              \
   MC(z,x,d*sizeof(Tx)); x+=d;                                        \
   DQ(n-1, vecfn(AH2ANP_v(d,z,x,z+d,jt)); z+=d; x+=d; ); z+=d;                     \
  }}                                                                   \
  R NANTEST?EVNAN:EVOK;                                                             \
 }   /* for associative functions only */

#define PREFICPFX(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){I i;Tz v,* y;                                    \
  if(d==1)DQ(m, *z++=v=(Tz)*x++; DQ(n-1, *z=v=pfx(v,*x); ++z; ++x;))  \
  else{for(i=0;i<m;++i){                                              \
   y=z; DQ(d, *z++=(Tz)*x++;);                                        \
   DQ(n-1, DQ(d, *z=pfx(*y,*x); ++z; ++x; ++y;));                     \
 }}R EVOK;}  /* for associative functions only */

#define PREFIXALT(f,Tz,Tx,pfx,retstmt)  \
 AHDRP(f,Tz,Tx){B b;I i;Tz v,* y;                                                 \
  if(d==1)DQ(m, *z++=v=    *x++; b=0; DQ(n-1, b^=1; pfx(b,*z,v,*x); v=*z; ++z; ++x;))  \
  else{for(i=0;i<m;++i){                                                               \
   y=z; DQ(d, *z++=    *x++;); b=0;                                                    \
   DQ(n-1, b^=1; DQ(d, pfx(b,*z,*y,*x); ++z; ++x; ++y;));                              \
 }}retstmt}

#define PREALTNAN(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){B b;I i;Tz v,* y;                                                 \
  NAN0;                                                                                \
  if(d==1)DQ(m, *z++=v=    *x++; b=0; DQ(n-1, b^=1; pfx(b,*z,v,*x); v=*z; ++z; ++x;))  \
  else{for(i=0;i<m;++i){                                                               \
   y=z; DQ(d, *z++=    *x++;); b=0;                                                    \
   DQ(n-1, b^=1; DQ(d, pfx(b,*z,*y,*x); ++z; ++x; ++y;));                              \
  }}                                                                                    \
  R NANTEST?EVNAN:EVOK;                                                                               \
 }

#define PREFICALT(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){B b;I i;Tz v,* y;                                                 \
  if(d==1)DQ(m, *z++=v=(Tz)*x++; b=0; DQ(n-1, b^=1; pfx(b,*z,v,*x); v=*z; ++z; ++x;))  \
  else{for(i=0;i<m;++i){                                                               \
   y=z; DQ(d, *z++=(Tz)*x++;); b=0;                                                    \
   DQ(n-1, b^=1; DQ(d, pfx(b,*z,*y,*x); ++z; ++x; ++y;));                              \
 }}R EVOK;}

  
#if SY_ALIGN
#define PREFIXBFXLOOP(T,pfx)  \
 {T* xx=(T*)x,* yy,* zz=(T*)z;  \
  q=d/sizeof(T);             \
  DQ(m, yy=zz; DQ(q, *zz++=*xx++;); DQ(n-1, DQ(q, *zz++=pfx(*yy,*xx); ++xx; ++yy;)));  \
 R EVOK;}

// must not overstore because of inplace
#define PREFIXBFX(f,pfx,ipfx,spfx,bpfx,vexp)          \
 AHDRP(f,B,B){B* y;I j,q;                        \
  if(1==d)for(j=0;j<m;++j){vexp}                      \
  else DQ(m, y=z; JMC(z,x,d,1); z+=d; x+=d; DQ(n-1, DQ((d-1)>>LGSZI, *(I*)z=pfx(*(I*)y,*(I*)x); x+=SZI; y+=SZI; z+=SZI;) I nct=(-d)&(SZI-1); STOREBYTES(z,pfx(*(I*)y,*(I*)x),nct) x+=SZI-nct; y+=SZI-nct; z+=SZI-nct;))  \
  R EVOK;}    /* f/\"r z for boolean associative atomic function f */
#else
#define PREFIXBFX(f,pfx,ipfx,spfx,bpfx,vexp)          \
 AHDRP(f,B,B){B*tv;I i,q,r,t,*xi,*yi,*zi;                      \
  q=d/SZI; r=d%SZI; xi=(I*)x; zi=(I*)z; tv=(B*)&t;        \
  if(1==d)   for(r=0;r<m;++r){vexp}                              \
  else if(!r)for(i=0;i<m;++i){                                   \
   yi=zi; DQ(q, *zi++=*xi++;);                                   \
   DQ(n-1, DQ(q, *zi=pfx(*yi,*xi); ++zi; ++xi; ++yi;));          \
  }else for(i=0;i<m;++i){                                        \
   yi=zi; MC(zi,xi,d);                                           \
   xi=(I*)((B*)xi+d);                                            \
   zi=(I*)((B*)zi+d);                                            \
   DQ(n-1, DQ(q, *zi=pfx(*yi,*xi); ++zi; ++xi; ++yi;);           \
    t=pfx(*yi,*xi); z=(B*)zi; DO(r, z[i]=tv[i];);                \
    xi=(I*)(r+(B*)xi);                                           \
    yi=(I*)(r+(B*)yi);                                           \
    zi=(I*)(r+(B*)zi); );                                        \
 }}   /* f/\"r z for boolean associative atomic function f */
#endif

#define BFXANDOR(c0,c1)  \
 {B*y=memchr(x,c0,n); if(y){q=y-x; mvc(q,z,1,iotavec-IOTAVECBEGIN+(c1)); mvc(n-q,z+q,1,iotavec-IOTAVECBEGIN+(c0));}else mvc(n,z,1,iotavec-IOTAVECBEGIN+(c1)); x+=d*n; z+=d*n;}

PREFIXBFX( orpfxB, OR, IOR, SOR, BOR,  BFXANDOR(C1,C0))
PREFIXBFX(andpfxB, AND,IAND,SAND,BAND, BFXANDOR(C0,C1))
// 0 1 2 3 4 5 6 7
//
// 0 1 2 3 4 5 6 7
//   0 1 2 3 4 5 6
//
// 0 1 2 3 4 5 6 7
//   0 1 2 3 4 5 6
//     0 1 2 3 4 5
//       0 1 2 3 4
//
// 0 1 2 3 4 5 6 7
//   0 1 2 3 4 5 6
//     0 1 2 3 4 5
//       0 1 2 3 4
//         0 1 2 3
//           0 1 2
//             0 1
//               0
#if SY_64
#define PFXSXOR(t) t^=(t<<8); t^=(t<<16); t^=(t<<32);
#else
#define PFXSXOR(t) t^=(t<<8); t^=(t<<16);
#endif
// word-wide =/\ t holds the previous word result; its high byte is chained to the low byte of the next word.  XOR cascade follows the pattern above.  + differs from ~: in the alternating pattern
PREFIXBFX( nepfxB, NE, INE, SNE, BNE, {I t=0; DQ((n-1)>>LGSZI, t=*(I*)x^(t>>(BB*(SZI-1))); PFXSXOR(t) *(I*)z=t; x+=SZI; z+=SZI;) \
            I nct=(-n)&(SZI-1); t=*(I*)x^(t>>(BB*(SZI-1))); PFXSXOR(t) *(I*)z=t; STOREBYTES(z,t,nct) x+=SZI-nct; z+=SZI-nct;})
PREFIXBFX( eqpfxB, EQ, IEQ, SEQ, BEQ, {I t=VALIDBOOLEAN; DQ((n-1)>>LGSZI, t=*(I*)x^(t>>(BB*(SZI-1))); PFXSXOR(t) t^=ALTBYTES&VALIDBOOLEAN; *(I*)z=t; *(I*)z=t; x+=SZI; z+=SZI;) \
            I nct=(-n)&(SZI-1); t=*(I*)x^(t>>(BB*(SZI-1))); PFXSXOR(t) t^=ALTBYTES&VALIDBOOLEAN; STOREBYTES(z,t,nct) x+=SZI-nct; z+=SZI-nct;})

// m is */frame, n is #cells, d is length of each cell, p is 1 for <, 0 for <:
// the result is ~p until we hit an input=p; then p thereafter
static B jtpscanlt(J jt,I m,I d,I n,B*z,B*x,B p){A t;B*v;I i;
 mvc(m*n*d,z,1,iotavec-IOTAVECBEGIN+(p^1)); 
 if(1==d)DQ(m, if(v=memchr(x,p,n))*(z+(v-x))=p; z+=n; x+=n;)
 else{
  I *xiv0=(I*)x, *ziv0=(I*)z;  // word-long pointers running through each cell
  I npp; REPLBYTETOW(p^1,npp);  // one copy of p in each lane
  I nw=(d-1)>>LGSZI;  // number of words to process before the last
  for(i=0;i<m;++i){I j;
   I *xivc=xiv0, *zivc=ziv0;  // head of columns
   // process (up to) word-wide strips top to bottom, till every lane has been touched
   for(j=nw;j>0;--j){I k;
    I *ziv=zivc; I *xiv=xivc;  // scan pointers down the column
    I livelane=VALIDBOOLEAN;  // 1 bit if lane has NOT been finished
    for(k=n;k>0;--k){I xi=*xiv;  // if writing in place, must fetch before store
     *ziv=npp^((xi^npp)&livelane);  // xi^npp=1 if we have found the match in this column.  If that is 1 in a live lane, write p to the lane, otherwise ~p
     livelane=livelane&~(xi^npp);
     if(livelane==0)break;  // when all lanes have been found, stop the column
     ziv=(I*)((I)ziv+d);  xiv=(I*)((I)xiv+d);  // advance to next row in column 
    }
    zivc=(I*)((I)zivc+SZI);  xivc=(I*)((I)xivc+SZI);  // advance to next row in column 
   }
   I *ziv=zivc; I *xiv=xivc;  // scan pointers down the column
   I k,nunstored=(-d)&(SZI-1);  // number of bytes to 
   I livelane=VALIDBOOLEAN>>(nunstored<<LGBB);  // 1 bit if lane has NOT been finished - indicate the unstored lanes are finished to begin with
   for(k=n;k>0;--k){I xi=*xiv;  // if writing in place, must fetch before store
    I storeval=npp^((xi^npp)&livelane);  // xi^npp=1 if we have found the match in this column.  If that is 1 in a live lane, write p to the lane, otherwise ~p
    STOREBYTES(ziv,storeval,nunstored);  // store the modifiable part of the result - don't overwrite!
    livelane=livelane&~(xi^npp);  // if we wrote to the lane, mark it dead
    if(livelane==0)break;  // when all lanes have been found, stop the column
    ziv=(I*)((I)ziv+d);  xiv=(I*)((I)xiv+d);  // advance to next row in column 
   }
   xiv0=(I*)((I)xiv0+n*d); ziv0=(I*)((I)ziv0+n*d);  // advance to next cell
  }
 }
 R 1;
}    /* f/\"r w for < and <: */

AHDRP(ltpfxB,B,B){pscanlt(m,d,n,z,x,C1);R EVOK;}
AHDRP(lepfxB,B,B){pscanlt(m,d,n,z,x,C0);R EVOK;}

// result is alternating pp,~pp,... till a is encountered; then one atom from pa,~pa (as if started from 0), then one atom from ~ps,ps,... repeated
static B jtpscangt(J jt,I m,I d,I n,B*z,B*x,I apas){
 I a=apas&1, pp=(apas>>1)&1, pa=(apas>>2)&1, ps=apas>>3;  // extract values from mask
 if(d==1){I i;
  for(i=0;i<m;++i){
   A t;B b,*cc="\000\001\000\001\000\001\000\001\000",e,*p=cc+pp,*v;B*u;I i,j;  // *p must be overfetchable
   if(v=memchr(x,a,n)){
    j=v-x; b=j&1; 
    mvc(j,z,2L,p);  mvc(n-j,z+j,1,iotavec-IOTAVECBEGIN+(b^ps)); z[j]=b^pa;
   }else mvc(n,z,2L,p);
   z+=n; x+=n;
  }
 }else{I i;
  REPLBYTETOW(a^1,a); REPLBYTETOW(pp,pp); REPLBYTETOW(pa,pa); REPLBYTETOW(ps,ps);   // expand data to all lanes.  a is COMPLEMENT of input a
  ps^=pa; pa^=pp;  // we write pp,!pp,... in hit state, we write pa (alternating), i. e. we XOR with (pa^pp).  In posthit and later, we write ps alternating frozen, i. e. we XOR with pa^ps
  I *xiv0=(I*)x, *ziv0=(I*)z;  // word-long pointers running through each cell
  I nw=(d-1)>>LGSZI;  // number of words to process before the last
  for(i=0;i<m;++i){I j;
   I *xivc=xiv0, *zivc=ziv0;  // head of columns
   // process (up to) word-wide strips top to bottom, till every lane has been touched
   for(j=nw;j>0;--j){I k;
    I *ziv=zivc; I *xiv=xivc;  // scan pointers down the column
    I livelane=VALIDBOOLEAN;  // 1 bit if lane has NOT been finished
    I hit=0, posthit;  // 1 bit is this lane is encountering a for the first time this cycle, or did so last cycle
    I storeval=pp;  // init to store (alternating) lead byte
    for(k=n;k>0;--k){I xi=*xiv;  // if writing in place, must fetch before store
     // calculate the state to apply to this, after the new byte has been inspected: live, hit, hit1, or none if older
     posthit=hit; hit=(xi^a)&livelane; livelane&=~hit;  // xi^a=1 if we have found the match in this column, because a was complemented
     storeval^=(hit&pa) ^ (posthit&ps);  // apply the adjustments needed for hit & posthit state.  After that, no change
     *ziv=storeval;   // write out the value
     storeval^=livelane;  // complement the value for any lane still in the lead (alternating) phase
     ziv=(I*)((I)ziv+d);  xiv=(I*)((I)xiv+d);  // advance to next row in column 
    }
    zivc=(I*)((I)zivc+SZI);  xivc=(I*)((I)xivc+SZI);  // advance to next row in column 
   }
   I *ziv=zivc; I *xiv=xivc;  // scan pointers down the column
   I k,nunstored=(-d)&(SZI-1);  // number of bytes to 
   I livelane=VALIDBOOLEAN>>(nunstored<<LGBB);  // 1 bit if lane has NOT been finished - indicate the unstored lanes are finished to begin with
   I hit=0, posthit;  // 1 bit is this lane is encountering a for the first time this cycle, or did so last cycle
   I storeval=pp;  // init to store (alternating) lead byte
   for(k=n;k>0;--k){I xi=*xiv;  // if writing in place, must fetch before store
    posthit=hit; hit=(xi^a)&livelane; livelane&=~hit;  // xi^a=1 if we have found the match in this column, because a was complemented
    storeval^=(hit&pa) ^ (posthit&ps);  // apply the adjustments needed for hit & posthit state.  After that, no change
    STOREBYTES(ziv,storeval,nunstored);  // store the modifiable part of the result - don't overwrite!
    storeval^=livelane;  // complement the value for any lane still in the lead (alternating) phase
    ziv=(I*)((I)ziv+d);  xiv=(I*)((I)xiv+d);  // advance to next row in column 
   }
   xiv0=(I*)((I)xiv0+n*d); ziv0=(I*)((I)ziv0+n*d);  // advance to next cell
  }
 }
 R 1;
}    /* f/\"r w for > >: +: *: */

AHDRP(  gtpfxB,B,B){pscangt(m,d,n,z,x,0x2);R EVOK;}
AHDRP(  gepfxB,B,B){pscangt(m,d,n,z,x,0xd);R EVOK;}
AHDRP( norpfxB,B,B){pscangt(m,d,n,z,x,0x5);R EVOK;}
AHDRP(nandpfxB,B,B){pscangt(m,d,n,z,x,0xa);R EVOK;}

#define PLUSP1(x) if(unlikely(__builtin_add_overflow((x),t,&t)))R EWOV;
#define MINUSP1(x) if(parity^=1){if(unlikely(__builtin_add_overflow(t,(x),&t)))R EWOV;}else if(unlikely(__builtin_sub_overflow(t,(x),&t)))R EWOV;
#define TYMESP1(x) if(unlikely(__builtin_mul_overflow((x),t,&t)))R EWOV;
#define ftz(ft1,offset) ft1(*(I*)((C*)x+(offset))) *(I*)((C*)z+(offset))=t;
// m is #cells, n is #items per cell, d is #atoms in item
#define PREFIXOVF(f,neut,ft1,dcls)  \
 AHDRR(f,I,I){I i;                          \
  UI dlct=(n+3)>>2; I backoff=(-n)&3;  \
  if(d==1){ \
   DQ(m, dcls\
   UI dlct0=dlct; x-=backoff; z-=backoff; I t=neut; switch(backoff){do{case 0: ft1(x[0]) z[0]=t; case 1: ft1(x[1]) z[1]=t; case 2: ft1(x[2]) z[2]=t; case 3: ft1(x[3]) z[3]=t; x+=4; z+=4; }while(--dlct0);}   \
   ) R EVOK;  \
  }        \
  I xstride1=d*SZI; I xstride3=3*xstride1;  \
  x=(I*)((C*)x-backoff*xstride1); z=(I*)((C*)z-backoff*xstride1); /* backoff for all cells */ \
  DQ(m,  \
   DQ(d, dcls \
    UI dlct0=dlct; I t=neut; switch(n&3){do{case 0: ftz(ft1,0) case 3: ftz(ft1,xstride1) case 2: ftz(ft1,2*xstride1) case 1: ftz(ft1,xstride3) x=(I*)((C*)x+4*xstride1); z=(I*)((C*)z+4*xstride1); }while(--dlct0);}   \
    x=(I*)((C*)x-4*xstride1*dlct)+1; z=(I*)((C*)z-4*xstride1*dlct)+1;  /* preserve backoff */  \
   )  \
   x+=(n-1)*d; z+=(n-1)*d;  \
  )  \
 R EVOK;  \
 }

PREFIXOVF( pluspfxI, 0, PLUSP1,) 
PREFIXOVF(minuspfxI, 0, MINUSP1, I parity=0;) 
PREFIXOVF(tymespfxI, 1, TYMESP1,)


PREFICPFX( pluspfxO, D, I,  PLUS   )
PREFICPFX(tymespfxO, D, I,  TYMES  )
PREFICALT(minuspfxO, D, I,  MINUSPA)
PREFIXPFX( pluspfxB, I, B,  PLUS, plusIB , R EVOK; )
AHDRP(pluspfxD,D,D){I i;
 NAN0;
 if(d==1){
#if C_AVX2 || (EMU_AVX2 && defined(__aarch64__))  // this version is not faster in emulation
  DQ(m,
  AVXATOMLOOPEVENODD(2,  // unroll not needed; need maskload to load 0s after valid area
    neut=_mm256_setzero_pd();
    __m256d acc0=neut; __m256d acc1=neut; __m256d accs;  // accumulator and place to save it 
    ,
    u=_mm256_add_pd(_mm256_permute4x64_pd(u,0x90),_mm256_blend_pd(u,neut,0x01));  // -123 + 0012
    u=_mm256_add_pd(_mm256_permute2f128_pd(u,neut,0x02),u);  // finish scan of the 4 input values --01 + 0123
    accs=_mm256_add_pd(acc0,acc1); acc0=_mm256_add_pd(acc0,u);  // accumulate in lane 3.  This is the only carried dependency for acc0
    u=_mm256_add_pd(u,_mm256_permute4x64_pd(accs,0xff));  // add in total previously accumulated
    ,
    u=_mm256_add_pd(_mm256_permute4x64_pd(u,0x90),_mm256_blend_pd(u,neut,0x01));  // -123 + 0012
    u=_mm256_add_pd(_mm256_permute2f128_pd(u,neut,0x02),u);  // finish scan of the 4 input values --01 + 0123
    accs=_mm256_add_pd(acc0,acc1); acc1=_mm256_add_pd(acc1,u);  // accumulate in lane 3.  This is the only carried dependency for acc1
    u=_mm256_add_pd(u,_mm256_permute4x64_pd(accs,0xff));  // add in total previously accumulated
    ,
    )
  )
#else
  I n3=n/3; I rem=n-n3*3;  // number of triplets, number of extras
  DQ(m, D t0; D t1; D t2; D t12; D t01; if(rem<1){t0=0.0; t12=t1=0.0;}else {*z++=t0=*x++; if(rem==1){t12=t1=0.0;}else{t12=t1=*x++; *z++=t0+t1;}} t2=0.0;
    DQ(n3, t0+=*x++; *z++ =t0+t12; t1+=*x++; t01=t0+t1; *z++ =t01+t2; t2+=*x++; *z++ =t2+t01; t12=t1+t2;)  // use 2 accumulators
  )
#endif
 }else{
  for(i=0;i<m;++i){                                              
   MC(z,x,d*sizeof(D)); x+=d; 
   DQ(n-1, plusDD AH2A_v(d,z,x,z+d,jt); z+=d; x+=d;); z+=d;
  }
 }
 R NANTEST?EVNAN:EVOK;
}   /* for associative functions only */

// macro version of the above, for general use
#define PREFIXPFXAVX2(fn,neutral,pfx,vecfn,avxinst,ending) \
AHDRP(fn,D,D){I i; \
 NAN0; \
 if(d==1){ \
  DQ(m, \
  AVXATOMLOOPEVENODD(2, \
    neut=_mm256_broadcast_sd(&neutral); \
    __m256d acc0=neut; __m256d acc1=neut; __m256d accs; \
    , \
    u=avxinst(_mm256_permute4x64_pd(u,0x90),_mm256_blend_pd(u,neut,0x01)); \
    u=avxinst(_mm256_permute2f128_pd(u,neut,0x02),u); \
    accs=avxinst(acc0,acc1); acc0=avxinst(acc0,u); \
    u=avxinst(u,_mm256_permute4x64_pd(accs,0xff)); \
    , \
    u=avxinst(_mm256_permute4x64_pd(u,0x90),_mm256_blend_pd(u,neut,0x01)); \
    u=avxinst(_mm256_permute2f128_pd(u,neut,0x02),u); \
    accs=avxinst(acc0,acc1); acc1=avxinst(acc1,u); \
    u=avxinst(u,_mm256_permute4x64_pd(accs,0xff)); \
    , \
    ) \
  ) \
 }else{ \
  for(i=0;i<m;++i){ \
   MC(z,x,d*sizeof(D)); x+=d;  \
   DQ(n-1, vecfn(AH2ANP_v(d,z,x,z+d,jt)); z+=d; x+=d;); z+=d; \
  } \
 } \
 ending \
}   /* for associative functions only */

PREFIXNAN( pluspfxZ, Z, Z,  zplus, plusZZ  )
PREFIXPFX( pluspfxX, X, X,  xplus, plusXX ,HDR1JERR; )
PREFIXPFX( pluspfxQ, Q, Q,  qplus, plusQQ ,HDR1JERR; )
static OP1XYZ(plus,I,I,I2,pfxplus)
PREFIXPFX( pluspfxI2, I, I2,  pfxplus, plus1II2I , R EVOK; )
static OP1XYZ(plus,I,I,I4,pfxplus)
PREFIXPFX( pluspfxI4, I, I4,  pfxplus, plus1II4I , R EVOK; )

PREFIXPFX(tymespfxD, D, D,  TYMES, tymesDD ,R EVOK;  )
PREFIXPFX(tymespfxZ, Z, Z,  ztymes, tymesZZ ,R EVOK;)
PREFIXPFX(tymespfxX, X, X,  xtymes, tymesXX ,R EVOK;)
PREFIXPFX(tymespfxQ, Q, Q,  qtymes, tymesQQ ,R EVOK;)

PREFIXALT(minuspfxB, I, B,  MINUSPA, R EVOK;)
PREALTNAN(minuspfxD, D, D,  MINUSPA)
PREALTNAN(minuspfxZ, Z, Z,  MINUSPZ)
PREFIXALT(minuspfxX, X, X,  MINUSPX, HDR1JERR;)
PREFIXALT(minuspfxQ, Q, Q,  MINUSPQ, HDR1JERR;)

PREALTNAN(  divpfxD, D, D,  DIVPA  )
PREALTNAN(  divpfxZ, Z, Z,  DIVPZ  )

PREFIXPFX(  maxpfxI, I, I,  MAX , maxII   ,R EVOK;)
#if C_AVX2 || (EMU_AVX2 && defined(__aarch64__))   // not better in emulation
PREFIXPFXAVX2(maxpfxD,infm,MAX,maxDD,_mm256_max_pd,R EVOK;)
#else
PREFIXPFX(  maxpfxD, D, D,  MAX , maxDD   ,R EVOK;)
#endif
PREFIXPFX(  maxpfxX, X, X,  XMAX, maxXX   ,R EVOK;)
PREFIXPFX(  maxpfxQ, Q, Q,  QMAX, maxQQ   ,R EVOK;)
PREFIXPFX(  maxpfxS, SB,SB, SBMAX, maxSS  ,R EVOK;)

PREFIXPFX(  minpfxI, I, I,  MIN, minII    ,R EVOK;)
#if C_AVX2 || (EMU_AVX2 && defined(__aarch64__))   // not better in emulation
PREFIXPFXAVX2(minpfxD,inf,MIN,minDD,_mm256_min_pd,R EVOK;)
#else
PREFIXPFX(  minpfxD, D, D,  MIN, minDD    ,R EVOK;)
#endif
PREFIXPFX(  minpfxX, X, X,  XMIN, minXX   ,R EVOK;)
PREFIXPFX(  minpfxQ, Q, Q,  QMIN, minQQ   ,R EVOK;)
PREFIXPFX(  minpfxS, SB,SB, SBMIN, minSS  ,R EVOK;)

PREFIXPFX(bw0000pfxI, UI,UI, BW0000, bw0000II,R EVOK;)
PREFIXPFX(bw0001pfxI, UI,UI, BW0001, bw0001II,R EVOK;)
PREFIXPFX(bw0011pfxI, UI,UI, BW0011, bw0011II,R EVOK;)
PREFIXPFX(bw0101pfxI, UI,UI, BW0101, bw0101II,R EVOK;)
PREFIXPFX(bw0110pfxI, UI,UI, BW0110, bw0110II,R EVOK;)
PREFIXPFX(bw0111pfxI, UI,UI, BW0111, bw0111II,R EVOK;)
PREFIXPFX(bw1001pfxI, UI,UI, BW1001, bw1001II,R EVOK;)
PREFIXPFX(bw1111pfxI, UI,UI, BW1111, bw1111II,R EVOK;)

// This old prefix support is needed for sparse matrices

static DF1(jtprefix){A fs=FAV(self)->fgh[0]; I r;
 ARGCHK1(w);
 r = (RANKT)jt->ranks; RESETRANK; if(r<AR(w)){R rank1ex(w,self,r,jtprefix);}
 R eachl(apv(SETIC(w,r),1L,1L),w,atop(fs,ds(CTAKE)));
}    /* f\"r w for general f */

static DF1(jtgprefix){A h,*hv,z,*zv;I m,n,r;
 ARGCHK1(w);
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);
 r = (RANKT)jt->ranks; RESETRANK; if(r<AR(w)){R rank1ex(w,self,r,jtgprefix);}
 SETIC(w,n); 
 h=VAV(self)->fgh[2]; hv=AAV(h); m=AN(h);
 GATV0(z,BOX,n,1); zv=AAV1(z); I imod=0;
 DO(n, imod=(imod==m)?0:imod; RZ(zv[i]=dfv1(h,take(sc(1+i),w),hv[imod])); ++imod;);
 R jtopenforassembly(jt,z);
}    /* g\"r w for gerund g */


// This old infix support is needed for sparse matrices

// block a contains (start,length) of infix.  w is the A for the data.
// Result is new block containing the extracted infix
static F2(jtseg){A z;I c,k,m,n,*u,zn;
 ARGCHK2(a,w);
 // The (start,length) had better be integers.  Extract them into m,n
 if(INT&AT(a)){u=AV(a); m=*u; n=u[1];} else m=n=0;
 c=aii(w); k=c<<bplg(AT(w)); DPMULDE(n,c,zn);  // c=#atoms per item, k=#bytes/item, zn=atoms/infix
 GA(z,AT(w),zn,MAX(1,AR(w)),AS(w)); AS(z)[0]=n;  // Allocate array of items, move in shape, override # items
 // Copy the selected items to the new block and return the new block
 MC(AV(z),CAV(w)+m*k,n*k);
 R z;
}

// m is the infix length (x), w is the array (y)
// Result is A for an nx2 table of (starting item#,length) for each infix
static A jtifxi(J jt,I m,A w){A z;I d,j,k,n,p,*x;
 ARGCHK1(w);
 // p=|m, n=#items of w, d=#applications of u (depending on overlapping/nonoverlapping)
 p=ABS(m); SETIC(w,n);
 if(m>=0){d=MAX(0,1+n-m);}else{d=1+(n-1)/p; d=(n==0)?n:d;}
 // Allocate result, a dx2 table; install shape
 GATV0(z,INT,2*d,2); AS(z)[0]=d; AS(z)[1]=2;
 // x->result area; k=stride between infixes; j=starting index (prebiased); copy (index,length) for each infix;
 // repair last length if it runs off the end
 x=AV2(z); k=0>m?p:1; j=-k; DQ(d, *x++=j+=k; *x++=p;); if(d)*--x=MIN(p,n-j);
 R z;
}

// Entry point for infix.  a is x, w is y, fs points to u
static DF2(jtinfix){PROLOG(0018);A fs=FAV(self)->fgh[0]; A x,z;I m; 
 F2RANK(0,RMAX,jtinfix,self); // Handle looping over rank.  This returns here for each cell (including this test)
 // The rest of this verb handles a single cell
 // If length is infinite, convert to large integer
 // kludge - test for ==ainf should be replaced with a test for value; will fail if _ is result of expression like {._
 if(a==ainf)m=IMAX;
 else RE(m=i0(vib(a))); // get infix length as an integer
 // Create table of infix positions
 RZ(x=ifxi(m,w));
 // If there are infixes, apply fs@:jtseg (ac2 creates an A verb for jtseg)
 if(AS(x)[0])z=eachl(x,w,atop(fs,ac2(jtseg)));
 else{A s;I r, rr;
  // No infixes.  Create a cell of fills, apply fs to it, add a leading axis of 0 to the result
  // create a block containing the shape of the fill-cell.  The fill-cell is a list of items of y,
  // with the number of items being the infix-size if positive, or 0 if negative
  // r = rank of w, rr=rank of list of items of w, s is block for list of length rr; copy shape of r; override #items of infix
  r=AR(w); rr=MAX(1,r); GATV0(s,INT,rr,1); if(r)MCISH(AV1(s),AS(w),r); AV1(s)[0]=0>m?0:m==IMAX?1+SETIC(w,r):m;
  // Create fill-cell of shape s; apply u to it
  RZ(dfv1(x,jtfiller(jt,AT(w),rr,AS(s)),fs));
  // Prepend leading axis of 0 to the result
  z=reshape(over(zeroionei(0),shape(x)),x);
 } 
 EPILOG(z);
}

static DF1(jtinfix2){PROLOG(0019);A f; 
 f=FAV(self)->fgh[0]; f=FAV(f)->fgh[0];  // f=u in u/\ y
 A l=curtail(w), r=behead(w), z; IRS2(l,r,f,AR(w)-1,AR(w)-1,FAV(f)->valencefns[1],z); // (}: u"_1 }.) y
 EPILOG(z);
}    /* 2 f/\w, where f supports IRS */

static DF2(jtginfix){A h,*hv,x,z,*zv;I d,m,n;
 RE(m=i0(vib(a))); 
 RZ(x=ifxi(m,w));
 h=VAV(self)->fgh[2]; hv=AAV(h); d=AN(h);
 if(SETIC(x,n)){
  GATV0(z,BOX,n,1); zv=AAV1(z);
  DO(n, RZ(zv[i]=df1(h,seg(from(sc(i),x),w),C(hv[i%d]))););
  R jtopenforassembly(jt,z);
 }else{A s;
  RZ(s=AR(w)?shape(w):ca(iv0)); AV(s)[0]=ABS(m);
  RZ(dfv1(x,jtfiller(jt,AT(w),AR(s),AS(s)),C(*hv)));
  R reshape(over(zeroionei(0),shape(x)),x);
}}

// *** start of non-sparse code ***

#define STATEISPREFIX 0x2000  // this is prefix rather than infix
#define STATESLASH2X 14  // f is f'/ and x is 2
#define STATESLASH2 ((I)1<<STATESLASH2X)

// prefix and infix: prefix if a is mark
static DF2(jtinfixprefix2){F2PREFIP;PROLOG(00202);A fs;I cger[128/SZI];
   I wt;
  ARGCHK1(w);
 F2RANKIP(0,RMAX,jtinfixprefix2,self);  // handle rank loop if needed
 wt=AT(w);
 if(unlikely(ISSPARSE(wt))){
  // Use the old-style non-virtual code for sparse types
  switch(((VAV(self)->flag&VGERL)>>(VGERLX-1)) + (a==mark)) {  // 2: is gerund  1: is prefix
  case (0+0): R jtinfix(jt,a,w,self);
  case (0+1): R jtprefix(jt,w,self);
  case (2+0): R jtginfix(jt,a,w,self);
  case (2+1): R jtgprefix(jt,w,self);
  }
 }
 // Not sparse.  Calculate the # result items
#define ZZFLAGWORD state
 I state=ZZFLAGINITSTATE;  // init flags, including zz flags

 // If the verb is a gerund, it comes in through h, otherwise the verb comes through f.  Set up for the two cases
 if(!(VGERL&FAV(self)->flag)){
  // not gerund: OK to test fs
  fs=FAV(self)->fgh[0];  // the verb we will execute
 }else{
  RZ(fs=createcycliciterator((A)&cger, self));  // use a verb that cycles through the gerunds.  NOTE cger is incompletely filled in & must be read with FAV()
 }
 V *vf=FAV(fs);  // if verb, point to its u operand
 if(vf->mr>=AR(w)){
  // we are going to execute f without any lower rank loop.  Thus we can use the BOXATOP etc flags here.  These flags are used only if we go through the full assemble path
  state |= vf->flag2>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // Don't touch fs yet, since we might not loop
  state &= ~(vf->flag2>>(VF2ATOPOPEN1X-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
  state &= ZZFLAGBOXATOP;  // we want just the one bit, BOXATOP1 & ~ATOPOPEN1
  state |= (-state) & (I)jtinplace & (ZZFLAGWILLBEOPENED|ZZFLAGCOUNTITEMS); // remember if this verb is followed by > or ; - only if we BOXATOP, to avoid invalid flag setting at assembly
 }
 AF f1=FAV(fs)->valencefns[0];  // point to the action routine now that we have handled gerunds

 I zi;  // number of items in the result
 I ilnval;  // value of a, set to -1 for prefix (for fill purposes)
 I ilnabs;  // abs(ilnval), or 1 if prefix.  Clamped to the # items of w
 I wc;  // number of atoms in a cell of w
 I remlen;  // number of items of w not processed yet (at start of loop, does not include the first infix).  When this goes to 0, we're done
 I stride;  // number of items to advance virtual-arg pointers by between cells
 I strideb;  // stride*number of bytes per cell (not used for prefix)
 I wi; SETIC(w,wi);  // wi=#items of w
 PROD(wc,AR(w)-1,AS(w)+1);  // #atoms in cell of a.  Overflow possible only if wi==0, which will go to fill
 // set up for prefix/infix.  Calculate # result slots
 if(a!=mark){
  // infix.
//  ilnval; RE(ilnval=i0(vib(a))); // ilnval=infix # (error if nonintegral; convert inf to HIGH_VALUE)
  RE(ilnval=i0(vib(a))); // ilnval=infix # (error if nonintegral; convert inf to HIGH_VALUE)
  if(ilnval>=0){
   // positive infix.  Stride is 1 cell.
   ilnabs=ilnval;
   zi=1+wi-ilnval;  // number of infix positions.  May be negative if no infixes.
   stride=1;   // advance 1 position per iteration
   remlen=zi;  // length is # nonoverlapping segments to do
  }else{
   // negative infix.  Stride is multiple cells.
   ilnabs=-ilnval; ilnabs^=REPSGN(ilnabs);  // abs(ilnval), and handle overflow
   zi=1+(wi-1)/ilnabs; zi=(wi==0)?wi:zi;  // calc number of partial infixes.  Because of C's rounding toward zero, we have to handle the wi==0 case separately, and anyway it
      // requires a design decision: we choose to treat it as 0 partitions of 0 length (rather than 1 partition of 0 length, or 0 partitions of length ilnabs)
   stride=ilnabs;  // advance by the stride
   remlen=wi;  // since there are no overlaps, set length-to-do to total length
  }
  strideb = (stride * wc) <<bplg(wt);  // get stride in bytes, for incrementing virtual-block offsets
 }else{
  // prefix.
  zi=wi;  // one prefix per item
  ilnabs=1;  // allocation of initial prefix is for 1 item
  ilnval=-1;  // set neg infix len to suppress fill if there are no result items
  stride=1;  // used only as loop counter
  remlen=zi;  // count # prefixes
  state |= STATEISPREFIX;
 }
 A zz=0;  // place where we will build up the homogeneous result cells
 if(zi>0){A z;
  // Normal case where there are cells.
  // loop over the infixes
#define ZZDECL
#include "result.h"
  ZZPARMS(1,zi,1,1)
#define ZZINSTALLFRAME(optr) *optr++=zi;

  // Allocate a virtual block for the argument, and give it initial shape and atomcount
  A virtw, virta;
  I vr=AR(w); vr+=(vr==0);  // rank of infix is same as rank of w, except that atoms are promoted to singleton lists

  // check for special case of 2 u/\ y; if found, set new function and allocate a second virtual argument
  // NOTE: gerund/ is encoded as `:, so we can be sure id==SLASH does not have gerund
  fauxblock(virtafaux); fauxblock(virtwfaux);
  if(((FAV(fs)->id^CSLASH)|((ilnabs|(wi&(SGNTO0(ilnval))))^2))){   // char==/ and (ilnabs==2, but not if input array is odd and ilnval is neg)
   // normal case, infix/prefix.  Allocate a virtual block
   fauxvirtual(virtw,virtwfaux,w,vr,ACUC1);

   ilnabs=(ilnabs>wi)?wi:ilnabs;  // ilnabs will be used to allocate virtual arguments - limit to size of w
   I *virtws=AS(virtw); virtws[0]=ilnabs; MCISH(virtws+1,AS(w)+1,vr-1) AN(virtw)=ilnabs*wc; // shape is (infix size),(shape of cell)  tally is #items*celllength
  }else{
   // 2 f/\ y.  The virtual args are now ITEMS of w rather than subarrays
   fauxvirtual(virta,virtafaux,w,vr-1,ACUC1); // first block is for a
   MCISH(AS(virta),AS(w)+1,vr-1); AN(virta)=wc; // shape is (shape of cell)  tally is celllength
   fauxvirtual(virtw,virtwfaux,w,vr-1,ACUC1);  // second is w
   AK(virtw) += strideb >> (SGNTO0(ilnval));  // we want to advance 1 cell.  If ilnval is positive, strideb is 1 cell; otherwise strideb is 2 cells
   MCISH(AS(virtw),AS(w)+1,vr-1); AN(virtw)=wc; // shape is (shape of cell)  tally is celllength
   // advance from f/ to f and get the function pointer.  Note that 2 <@(f/)\ will go through here too
   fs=FAV(fs)->fgh[0]; f1=FAV(fs)->valencefns[1];
   // mark that we are handling this case
   state |= STATESLASH2;
  }

  I gerundx = 0;  // in case we are doing gerunds, start on the first one
  while(1){

   // call the user's function
   if(!(state&(STATESLASH2))){
    // normal case: prefix/infix, no gerund
    RZ(z=CALL1(f1,virtw,fs));  //normal case
   }else {
    // 2 f/\ y case
    RZ(z=CALL2(f1,virta,virtw,fs));  //normal case
   }

#define ZZBODY  // assemble results
#include "result.h"

   // advance input pointer for next cell.  We keep the same virtual block(s) because it can't be incorporated into anything
   // We can't advance until after the assembly code has run, in case the verb returned the virtual block as its result

   // calculate the amount of data unprocessed after the result we just did.  If there is none, we're finished
   if((remlen-=stride)<=0)break;

   if(!(state&STATEISPREFIX)){
    // infix case, or 2 f/\ y.  Add to the virtual-block offset.  If this is a shard, reduce the size of the virtual block.
    // If this is 2 f/\ y, advance the second block as well.  It can't be both.
    AK(virtw) += strideb;
    if(((remlen-stride)|(SGNIF(state,STATESLASH2X)))<0){
     // we either have a shard or 2 f/\ y.
     if(state&STATESLASH2){
      // 2 f/\ y.  Advance the second pointer also.
      AK(virta) += strideb;
     }else{
      // we have hit a shard.  Reduce the shape and tally of the next argument
      AS(virtw)[0] += remlen-stride;   // reduce the # items by the amount of overhang
      AN(virtw) += (remlen-stride)*wc;  // reduce # atoms by amount of overhang, in atoms
     }
    }
   }else{
    // prefix.  enlarge the virtual block by 1 cell.
    AS(virtw)[0]++;  // one more cell
    AN(virtw) += wc;  // include its atoms
   }
  }

#define ZZEXIT
#include "result.h"

 }else{A z;
  // no cells - execute on a cell of fills
  // Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then

  // The cell to execute on depends on the arguments:
  // for prefix, 0 items of fill
  // for infix +, invabs items of fill
  // for infix -, 0 items of fill
  RZ(z=reitem(zeroionei(0),w));  // create 0 items of the type of w
  if(ilnval>=0){ilnval=(ilnval==IMAX)?(wi+1):ilnval; RZ(z=take(sc(ilnval),z));}    // if items needed, create them.  For compatibility, treat _ as 1 more than #items in w
  WITHDEBUGOFF(zz=CALL1(f1,z,fs);) if(EMSK(jt->jerr)&EXIGENTERROR)RZ(zz); RESETERR;
  RZ(zz=reshape(over(zeroionei(0),shape(zz?zz:mtv)),zz?zz:zeroionei(0)));
 }

// result is now in zz

 EPILOG(zz);
}

// prefix, vectors to common processor.  Handles IRS.  Supports inplacing
static DF1(jtinfixprefix1){F1PREFIP;
 I r = (RANKT)jt->ranks; RESETRANK; if(r<AR(w)){R jtrank1ex(jtinplace,w,self,r,jtinfixprefix1);}
 R jtinfixprefix2(jtinplace,mark,w,self);
}

//  f/\"r y    w is y, fs is in self
static DF1(jtpscan){A z;I f,n,r,t,wn,wr,*ws,wt;
 F1PREFIP;ARGCHK1(w);
 wt=AT(w);   // get type of w
 if(unlikely(ISSPARSE(wt)))R scansp(w,self,jtpscan);  // if sparse, go do it separately
 // wn = #atoms in w, wr=rank of w, r=effective rank, f=length of frame, ws->shape of w
 wn=AN(w); wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK; f=wr-r; ws=AS(w);
 // m = #cells, c=#atoms/cell, n = #items per cell
 SETICFR(w,f,r,n);  // wn=0 doesn't matter
 // If there are 0 or 1 items, or w is empty, return the input unchanged, except: if rank 0, return (($w),1)($,)w - if atomic op, do it right here, otherwise call the routine to get the shape of result cell
 if(((1-n)&-wn)>=0){R r?RETARG(w):reshape(apip(shape(w),zeroionei(1)),w);}  // n<2 or wn=0
 VARPS adocv; varps(adocv,self,wt,1);  // fetch info for f/\ and this type of arg
 if(!adocv.f)R IRS1(w,self,r,jtinfixprefix1,z);  // if there is no special function for this type, do general scan
 // Here is the fast special reduce for +/ etc
 I d,m; PROD(m,f,ws); PROD(d,r-1,ws+f+1);   // m=#scans, d=#atoms in a cell of each scan
 if((t=atype(adocv.cv))&&TYPESNE(t,wt))RZ(w=cvt(t,w));  // convert input if necessary
 // if inplaceable, reuse the input area for the result
 if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&SGNIF(adocv.cv,VIPOKWX),w))z=w; else GA(z,rtypew(adocv.cv,t),wn,wr,ws);
 I rc=((AHDRPFN*)adocv.f)(d,n,m,AV(w),AV(z),jt);
 if(unlikely((255&~EVNOCONV)&rc)){jsignal(rc); R (rc>=EWOV)?IRS1(w,self,r,jtpscan,z):0;} else R (adocv.cv&VRI+VRD)&&rc!=EVNOCONV?cvz(adocv.cv,z):z;
}    /* f/\"r w atomic f main control */

static DF2(jtinfixd){A z;C*x,*y;I c=0,d,k,m,n,p,q,r,*s,wr,*ws,wt,zc; 
 F2RANKW(0,RMAX,jtinfixd,self);
 wr=AR(w); ws=AS(w); wt=AT(w); SETIC(w,n);   // n=#items of w
 RE(m=i0(vib(a))); if(m==IMAX){m=n+1;} p=m==IMIN?IMAX:ABS(m);  // m=x arg, p=abs(m)
 if(0>m){p=MIN(p,n); if(likely(p!=0))d=(n+p-1)/p;else d=0;}else{ASSERT(IMAX-1>n-m,EVDOMAIN); d=MAX(0,1+n-m);}  // d=#partitions; error if n-m+1 overflows
 if(unlikely(CCOMMA==FAV(FAV(self)->fgh[0])->id)){RE(c=aii(w)); DPMULDE(p,c,zc) r=2;}  // c=#atoms in item of w; zc=#atoms in cell of result; r=result rank
 else{if(n)RE(c=aii(w)); zc=p; r=wr?1+wr:2;}   // if w has no items, proceed allocating 0 items of result
 // if m<0 and there is no final shard, create a virtual result
 q=d*p-n;   // number of uncopied or overcopied cells.  Uncopied cells are a shard when m<0; overcopied cells happen whem m>1.
 if(likely(q==0)){
  // Each input will be moved once.  We can make a virtual block using ($,)
  // Create the shape of the result
  fauxblockINT(afaux,4,1); fauxINT(z,afaux,r,1) s=IAV1(z); s[0]=d; s[1]=zc; MCISH(s+2,1+ws,r-2);  // allocate and copy shape
  R reshape(z,w);  // return the reshaped w
 }
 GA00(z,wt,d*p*c,r); x=CAVn(r,z); y=CAV(w);   // allocate result, set x=output pointer y=input pointer
 s=AS(z); s[0]=d; s[1]=zc; MCISH(s+2,1+ws,r-2);   // install shape
 I katom=(I)1<<bplg(wt); k=c<<bplg(wt);   // k=#bytes in a cell of result
 if(likely(AN(z)!=0)){
  if(m>=0){ q=p*k; JMCDECL(endmask) JMCSETMASK(endmask,q,0) DQ(d, JMCR(x,y,q,0,endmask) x+=q; y+=k;);  // m>0, overlapping inputs, copy them
  }else{JMC(x,y,n*k,0)  fillv0(wt); mvc(q*k,x+n*k,jt->fillvlen,jt->fillv);    // nonoverlapping: copy en bloc and fill
  }
 }
 RETF(z);
}    /* a[\w and a]\w and a,\w */


#define SETZ    {s=yv; DQ(c, *zv++=*s++;  );}
#define SETZD   {s=yv; DQ(c, *zv++=*s++/d;);}

#define MOVSUMAVG(Tw,Ty,ty,Tz,tz,xd,SET)  \
 {Tw*u,*v;Ty*s,x=0,*yv;Tz*zv;                                  \
  GATVS(z,tz,c*(1+p),AR(w),AS(w),tz##SIZE,GACOPYSHAPE,R 0); AS(z)[0]=1+p;                    \
  zv=(Tz*)AV(z); u=v=(Tw*)AV(w);                               \
  if(1==c){                                                    \
   DQ(m, x+=*v++;); *zv++=xd;                                  \
   DQ(p, x+=(Ty)*v++-(Ty)*u++; *zv++=xd;);                     \
  }else{                                                       \
   GATVS(y,ty,c,1,0,ty##SIZE,GACOPYSHAPE0,R 0); s=yv=(Ty*)AV1(y); DQ(c, *s++=0;);            \
   DQ(m, s=yv; DQ(c, *s+++=*v++;);); SET;                      \
   DQ(p, s=yv; DQ(c, x=*s+++=(Ty)*v++-(Ty)*u++; *zv++=xd;););  \
 }}

static A jtmovsumavg1(J jt,I m,A w,A fs,B avg){A y,z;D d=(D)m;I c,p,wt;
 SETIC(w,p); p-=m; wt=AT(w); c=aii(w);
 switch(((wt>>(INTX-1))&6)+avg){
 case 0:       MOVSUMAVG(B,I,INT,I,INT,x,  SETZ ); break;
 case 1:       MOVSUMAVG(B,I,INT,D,FL, x/d,SETZD); break;
 case 2: 
  // start min at 0 so range is max+1; make sure all totals fit in an int; use integer code if so
  {I maxval = (I)((D)IMAX/(D)MAX(2,m))-1;
  CR rng=condrange(AV(w),AN(w),0,0,maxval<<1);
  if(rng.range && MAX(rng.range,-rng.min)<maxval){
   MOVSUMAVG(I,I,INT,I,INT,x,  SETZ )
  }else{MOVSUMAVG(I,D,FL, D,FL, x,  SETZ );} break;}
 case 3:       MOVSUMAVG(I,D,FL, D,FL, x/d,SETZD); break;
 case 4: NAN0; MOVSUMAVG(D,D,FL, D,FL, x,  SETZ ); NAN1; break;
 case 5: NAN0; MOVSUMAVG(D,D,FL, D,FL, x/d,SETZD); NAN1; break;
 }
 RETF(z);
}    /* m +/\w or (if 0=avg) m (+/%#)\w (if 1=avg); bool or integer or float; 0<m */

static A jtmovsumavg(J jt,I m,A w,A fs,B avg){A z;
 z=movsumavg1(m,w,fs,avg);
 if(jt->jerr==EVNAN)RESETERR else R z;
 R jtinfixprefix2(jt,sc(m),w,fs);
}

static DF2(jtmovavg){I m,j;
 F2RANK(0,RMAX,jtmovavg,self);
 RE(m=i0(vib(a)));SETIC(w,j);
 if(0<m&&m<=j&&AT(w)&B01+FL+INT)R movsumavg(m,w,self,1);   // j may be 0
 R jtinfixprefix2(jt,a,w,self);
}    /* a (+/ % #)\w */

#define MOVMINMAX(T,type,ie,CMP)    \
 {T d,e,*s,*t,*u,*v,x=ie,*yv,*zv;                              \
  zv=(T*)AV(z); u=v=(T*)AV(w);                                 \
  if(1==c){                                                    \
   DQ(m, d=*v++; if(d CMP x)x=d;); *zv++=x;                    \
   for(i=0;i<p;++i){                                           \
    d=*v++; e=*u++;                                            \
    if(d CMP x)x=d; else if(e==x){x=d; t=u; DQ(m-1, e=*t++; if(e CMP x)x=e;);}  \
    *zv++=x;                                                   \
  }}else{                                                      \
   GATVS(y,type,c,1,0,type##SIZE,GACOPYSHAPE0,R 0); s=yv=(T*)AV1(y); DQ(c, *s++=ie;);          \
   DQ(m, s=yv; DQ(c, d=*v++; if(d CMP *s)*s=d; ++s;);); SETZ; /* should store to sink instead of branching */ \
   for(i=0;i<p;++i){                                           \
    for(j=0,s=yv;j<c;++j,++s){                                 \
     d=*v++; e=*u++; x=*s;                                     \
     if(d CMP x)x=d; else if(e==x){x=d; t=c+u-1; DQ(m-1, e=*t; t+=c; if(e CMP x)x=e;);}  \
     *s=x;                                                     \
    }                                                          \
    SETZ;                                                      \
 }}}

#define MOVMINMAXS(T,type,ie,CMP)    \
 {T d,e,*s,*t,*u,*v,x=ie,*yv,*zv;                              \
  zv=(T*)AV(z); u=v=(T*)AV(w);                                 \
  if(1==c){                                                    \
   DQ(m, d=*v++; if(CMP(d,x))x=d;); *zv++=x;                    \
   for(i=0;i<p;++i){                                           \
    d=*v++; e=*u++;                                            \
    if(CMP(d,x))x=d; else if(e==x){x=d; t=u; DQ(m-1, e=*t++; if(CMP(e,x))x=e;);}  \
    *zv++=x;                                                   \
  }}else{                                                      \
   GATVS(y,type,c,1,0,type##SIZE,GACOPYSHAPE0,R 0); s=yv=(T*)AV1(y); DQ(c, *s++=ie;);          \
   DQ(m, s=yv; DQ(c, d=*v++; if(CMP(d,*s))*s=d; ++s;);); SETZ;  /* should store to sink instead of branching */ \
   for(i=0;i<p;++i){                                           \
    for(j=0,s=yv;j<c;++j,++s){                                 \
     d=*v++; e=*u++; x=*s;                                     \
     if(CMP(d,x))x=d; else if(e==x){x=d; t=c+u-1; DQ(m-1, e=*t; t+=c; if(CMP(e,x))x=e;);}  \
     *s=x;                                                     \
    }                                                          \
    SETZ;                                                      \
 }}}

static A jtmovminmax(J jt,I m,A w,A fs,B max){A y,z;I c,i,j,p,wt;
 SETIC(w,p); p-=m; wt=AT(w); c=aii(w);
 GA(z,AT(w),c*(1+p),AR(w),AS(w)); AS(z)[0]=1+p;
 switch(max + ((wt>>(INTX-1))&6)){
// no max sym now  case 0: MOVMINMAXS(SB,SBT,SBUV4(JT(jt,sbu))[0].down,SBLE); break;
 case 0: MOVMINMAXS(SB,SBT,0,SBLE); break;
 case 1: MOVMINMAXS(SB,SBT,0,SBGE); break;
 case 2: MOVMINMAX(I,INT,IMAX,<=); break;
 case 3: MOVMINMAX(I,INT,IMIN,>=); break;
 case 4: MOVMINMAX(D,FL, inf ,<=); break;
 case 5: MOVMINMAX(D,FL, infm,>=); break;
 }
 RETF(z);
}    /* a <./\w (0=max) or a >./\ (1=max); vector w; integer/float/symbol; 0<m */

static A jtmovandor(J jt,I m,A w,A fs,B or){A y,z;B b0,b1,d,e,*s,*t,*u,*v,x,*yv,*zv;I c,i,j,p;
 SETIC(w,p); p-=m; c=aii(w); x=b0=or^1; b1=or;
 GATV(z,B01,c*(1+p),AR(w),AS(w)); AS(z)[0]=1+p;
 zv=BAV(z); u=v=BAV(w);
 if(1==c){
  DQ(m, if(b1==*v++){x=b1; break;}); *zv++=x; v=u+m;
  for(i=0;i<p;++i){
   d=*v++; e=*u++;
   if(d==b1)x=d; else if(e==b1){x=d; t=u; DQ(m-1, if(b1==*t++){x=b1; break;});}
   *zv++=x;
 }}else{
  GATV0(y,B01,c,1); s=yv=BAV1(y); DQ(c, *s++=b0;);
  DQ(m, s=yv; DQ(c, if(b1==*v++)*s=b1; ++s;);); SETZ;
  for(i=0;i<p;++i){
   for(j=0,s=yv;j<c;++j,++s){
    d=*v++; e=*u++; x=*s;
    if(d==b1)x=d; else if(e==b1){x=d; t=c+u-1; DQ(m-1, e=*t; t+=c; if(b1==e){x=b1; break;});}
    *s=x;
   }
   SETZ;
 }}
 RETF(z);
}    /* a *./\w (0=or) or a +./\ (1=or); boolean w; 0<m */

static A jtmovbwandor(J jt,I m,A w,A fs,B or){A z;I c,p,*s,*t,*u,x,*zv;
 SETIC(w,p); p-=m; c=aii(w);
 GATV(z,INT,c*(1+p),AR(w),AS(w)); AS(z)[0]=1+p;
 zv=AV(z); u=AV(w);
 if(c)switch(or+(1==c?0:2)){
 case 0: DQ(1+p, x=*u++; t=u; DQ(m-1, x&=*t++;); *zv++=x;); break;
 case 1: DQ(1+p, x=*u++; t=u; DQ(m-1, x|=*t++;); *zv++=x;); break;
 case 2: DQ(1+p, ICPY(zv,u,c); t=u+=c; DQ(m-1, s=zv; DQ(c, *s++&=*t++;);); zv+=c;); break;
 case 3: DQ(1+p, ICPY(zv,u,c); t=u+=c; DQ(m-1, s=zv; DQ(c, *s++|=*t++;);); zv+=c;); break;
 }
 RETF(z);
}    /* a 17 b./\w (0=or) or a 23 b./\ (1=or); integer w; 0<m */

static A jtmovneeq(J jt,I m,A w,A fs,B eq){A y,z;B*s,*u,*v,x,*yv,*zv;I c,p;
 SETIC(w,p); p-=m; c=aii(w); x=eq;
 GATV(z,B01,c*(1+p),AR(w),AS(w)); AS(z)[0]=1+p;
 zv=BAV(z); u=v=BAV(w);
 if(1<c){GATV0(y,B01,c,1); s=yv=BAV1(y); DQ(c, *s++=eq;);}
 switch(eq+(1<c?2:0)){
 case 0: DQ(m,                   x   ^=   *v++;  ); *zv++=x; DQ(p,                   *zv++=x   ^=   *u++^ *v++;  ); break;
 case 1: DQ(m,                   x    =x==*v++;  ); *zv++=x; DQ(p,                   *zv++=x    =x==*u++==*v++;  ); break;
 case 2: DQ(m, s=yv; DQ(c,       *s++^=   *v++;);); SETZ;    DQ(p, s=yv; DQ(c,       *zv++=*s++^=   *u++^ *v++;);); break;
 case 3: DQ(m, s=yv; DQ(c, x=*s; *s++ =x==*v++;);); SETZ;    DQ(p, s=yv; DQ(c, x=*s; *zv++=*s++ =x==*u++==*v++;););
 }
 RETF(z);
}    /* m ~:/\w (0=eq) or m =/\ (1=eq); boolean w; 0<m */

static A jtmovbwneeq(J jt,I m,A w,A fs,B eq){A y,z;I c,p,*s,*u,*v,x,*yv,*zv;
 SETIC(w,p); p-=m; c=aii(w); x=eq?-1:0;
 GATV(z,INT,c*(1+p),AR(w),AS(w)); AS(z)[0]=1+p;
 zv=AV(z); u=v=AV(w);
 if(1<c){GATV0(y,INT,c,1); s=yv=AV1(y); DQ(c, *s++=x;);}
 switch(eq+(1<c?2:0)){
 case 0: DQ(m,                   x   ^=    *v++ ;  ); *zv++=x; DQ(p,                   *zv++=x   ^=      *u++^*v++  ;  ); break;
 case 1: DQ(m,                   x    =~(x^*v++);  ); *zv++=x; DQ(p,                   *zv++=x    =~(x^~(*u++^*v++));  ); break;
 case 2: DQ(m, s=yv; DQ(c,       *s++^=    *v++ ;);); SETZ;    DQ(p, s=yv; DQ(c,       *zv++=*s++^=      *u++^*v++  ;);); break;
 case 3: DQ(m, s=yv; DQ(c, x=*s; *s++ =~(x^*v++););); SETZ;    DQ(p, s=yv; DQ(c, x=*s; *zv++=*s++ =~(x^~(*u++^*v++));););
 }
 RETF(z);
}    /* m 22 b./\w (0=eq) or m 25 b./\ (1=eq); integer w; 0<m */

static DF2(jtmovfslash){A x,z;B b;C id,*wv,*zv;I d,m,m0,p,t,wk,wt,zi,zk,zt;
 F2RANK(0,RMAX,jtmovfslash,self);
 SETIC(w,p); wt=AT(w);   // p=#items of w
 RE(m0=i0(vib(a))); m=REPSGN(m0); m=(m^m0)-m; m^=REPSGN(m);  // m0=infx x,  m=abs(m0), handling IMIN
 if(m==1)R AR(w)?w:ravel(w);  // 1 f/\ w is always w, except on an atom
 if((SGNIF((m0==2)&FAV(self)->flag,VFSCANIRSX)&SGNIFDENSE(wt)&(1-p))<0)R jtinfix2(jt,w,self);  // if  2 u/\ y supports IRS, go do (}: u }.) y - faster than cells - if >1 cell and dense  uses VFSCANIRSX=0
 if((((2^m)-1)|(m-1)|(p-m))<0)R jtinfixprefix2(jt,a,w,self);  // If m is 0 or 2, or if there is just 1 infix, go to general case
 x=FAV(self)->fgh[0]; x=FAV(x)->fgh[0]; id=FAV(x)->id; 
 if(wt&B01){id=id==CMIN?CSTARDOT:id; id=id==CMAX?CPLUSDOT:id;}
 if(id==CBDOT&&(x=VAV(x)->fgh[1],INT&AT(x)&&!AR(x)))id=(C)AV(x)[0];
 switch(AR(w)&&BETWEENC(m0,0,AS(w)[0])?id:0){
 case CPLUS:    if(wt&B01+INT+FL)R movsumavg(m,w,self,0); break;
 case CMIN:     if(wt&SBT+INT+FL)R movminmax(m,w,self,0); break;
 case CMAX:     if(wt&SBT+INT+FL)R movminmax(m,w,self,1); break;
 case CSTARDOT: if(wt&B01       )R  movandor(m,w,self,0); break;
 case CPLUSDOT: if(wt&B01       )R  movandor(m,w,self,1); break;
 case CNE:      if(wt&B01       )R   movneeq(m,w,self,0); break;
 case CEQ:      if(wt&B01       )R   movneeq(m,w,self,1); break;
 case CBW1001:  if(wt&    INT   )R movbwneeq(m,w,self,1); break;
 case CBW0110:  if(wt&    INT   )R movbwneeq(m,w,self,0); break;
 }
 VARPS adocv;
 varps(adocv,self,wt,0); if(!adocv.f)R jtinfixprefix2(jt,a,w,self);  // if no special routine for insert, do general case
 if(m0>=0){zi=MAX(0,1+p-m);}else{zi=1+(p-1)/m; zi=(p==0)?p:zi;}  // zi = # result cells
 PROD(d,AR(w)-1,AS(w)+1) b=0>m0&&zi*m!=p;   // b='has shard'
 zt=rtypew(adocv.cv,wt); RESETRANK;
 GA(z,zt,d*zi,MAX(1,AR(w)),AS(w)); AS(z)[0]=zi;
 if(d*zi==0){RETF(z);}  // mustn't call adocv on empty arg!
 if((t=atype(adocv.cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wt=AT(w);}
 zv=CAV(z); zk=d<<bplg(zt); 
 wv=CAV(w); wk=(0<=m0?d:d*m)<<bplg(wt);
 I rc=EVOK;
 DQ(zi-b, I lrc=((AHDRPFN*)adocv.f)(d,m,(I)1,wv,zv,jt); rc=lrc<rc?lrc:rc; zv+=zk; wv+=wk;);
 if(b){m=p-m*(zi-1); if(m>1){I lrc=((AHDRPFN*)adocv.f)(d,m,(I)1,wv,zv,jt); rc=lrc<rc?lrc:rc;}else{copyTT(zv,wv,d,zt,wt);}}
 if(255&rc){jsignal(rc); if(rc>=EWOV){RESETERR; R movfslash(a,ccvt(FL,w,0),self);}R0;}else R z;
}    /* a f/\w */

static DF1(jtiota1){I j; R apv(SETIC(w,j),1L,1L);}

F1(jtbslash){F1PREFIP;A f;AF f1=jtinfixprefix1,f2=jtinfixprefix2;V*v;I flag=FAV(ds(CBSLASH))->flag;
;
 ARGCHK1(w);
 A z; fdefallo(z)
 if(NOUN&AT(w)){A fixw; RZ(fixw=fxeachv(1L,w)); fdeffill(z,0,CBSLASH,VERB, jtinfixprefix1,jtinfixprefix2, w,0L,fixw, VGERL|flag, RMAX,0L,RMAX); RETF(z);}
 // falling through, w is verb
 v=FAV(w);  // v is the u in u\ y
 flag|=v->flag&VASGSAFE;  // if u is asgsafe, so is u\ y
 switch(v->id){
 case CSLASH: ;  // never gerund/ which is coded as GRCO
  A u=v->fgh[0];  // the u in u/\ y
  if(AT(u)&VERB)flag |= (FAV(u)->flag >> (VIRS2X-VFSCANIRSX)) & VFSCANIRS;  // indic if we should use {: f }: for 2 /\ y
  f2=jtmovfslash; if(FAV(u)->flag&VISATOMIC2){f1=jtpscan; flag|=VJTFLGOK1;} break;
 case CPOUND:
  f1=jtiota1; break;
 case CLEFT: case CRIGHT: case CCOMMA:
  f2=jtinfixd; break;
 case CFORK:  
  if(v->valencefns[0]==(AF)jtmean)f2=jtmovavg; break;
 default:
  flag |= VJTFLGOK1|VJTFLGOK2; break; // The default u\ looks at WILLBEOPENED
 }
 fdeffillall(z,0,CBSLASH,VERB,f1,f2,w,0L,0L,flag,RMAX,0L,RMAX,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.redfn=v->id==CSLASH?v->localuse.lu1.redfn:0)
 // Fill in the lvp[1] field: with 0 if not f/\; with the lookup field for f/ if f/\ .   f is nonnull if f/\ .
 RETF(z);
}

A jtascan(J jt,C c,A w){ARGCHK1(w); A z; R dfv1(z,w,bslash(slash(ds(c))));}
