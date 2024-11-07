/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"
#if C_CRC32C && SY_64
#include "viavx.h"

// *************************** sixth class: hashing w ***********************
// used when w is shorter than a and thus more likely to fit into cache.  Also allows early exit when all results found.
//  Intolerant comparisons only.   Used for i./i:/e. only, and never reflexive

// start a new class.  The index is i.  It does not need to be translated, since it will be where the data for its class is.
// The root of the class is the first index found (i).  Init to 'not found' (could be faster to init them all at once)
#define XDOWNEWCLASS(TH,TZ,zptr,nfval) {++chainct; indtdd[i]=(TH)i; zptr[i]=(TZ)nfval;}
// add to an old class.  The index is i; hj is the root of the class (the smallest index for the class).
#define XDOWOLDCLASS(TH) {indtdd[i]=(TH)hj;}

// A value from a matched the hashtable.  The hashtable entry will be erased; here we store the index of the match (i) into
// the result hashchain (anchored at hj)
#define XDOWFOUND(TZ,zptr,zvalue,earlyexit) {zptr[hj]=(TZ)zvalue; if(--chainct==0)goto earlyexit;}

// After the results have been calculated, go through the indirect table and copy the result for any value that is not start-of-class
#define XDOWINDCLASS(zptr) {I zx; DO(wsct, if((zx=indtdd[i])!=i){zptr[i]=zptr[zx];});}

// Traverse w in forward direction, adding values to the hash table
#define XDOWP(T,TH,TZ,zptr,hash,exp,stride,nfval) XSEARCH(T,TH,w,w,hash,exp,stride,XDOWOLDCLASS(TH),XDOWNEWCLASS(TH,TZ,zptr,nfval),1,0, (i=0;i<wsct-2;++i) ,wsct-1)

// Traverse a in forward direction, executing fstmt/nfstmt depending on found/notfound; clearing the hash if a match is found
#define XDOWA(T,TH,TZ,zptr,hash,exp,stride,zvalue,earlyexit) XSEARCH(T,TH,a,w,hash,exp,stride,XDOWFOUND(TZ,zptr,zvalue,earlyexit),{},2,0, (i=0;i<asct-2;++i) ,asct-1)
// same, traversing a from back to front
#define XDQWA(T,TH,TZ,zptr,hash,exp,stride,zvalue,earlyexit) XSEARCH(T,TH,a,w,hash,exp,(-(stride)),XDOWFOUND(TZ,zptr,zvalue,earlyexit),{},2,cn*(asct-1), (i=asct-1;i>1;--i) ,0)

#define IOFXW(T,TH,f,setup,hash,exp,stride)   \
 IOF(f){I acn=ak/sizeof(T),cn=k/sizeof(T),l,p,  \
        wcn=wk/sizeof(T);T* RESTRICT av=(T*)AV(a),* RESTRICT wv=(T*)AV(w);I md; TH * RESTRICT hv; \
        IH *hh=IHAV(h); p=hh->datarange;  hv=hh->data.TH;  \
 \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  setup \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */  \
  A indtbl; GATV0(indtbl,INT,((asct*sizeof(TH)+SZI)>>LGSZI),0); TH * RESTRICT indtdd=TH##AV0(indtbl); \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){I chainct=0;  /* number of chains in w */   \
   /* zv progresses through the result - for those versions that support IRS */ \
   hashallo(hh,p,wsct,mode); \
     \
   switch(md){                                                                       \
   case IICO:\
   case IIDOT: {I * RESTRICT zv=AV(z)+l*wsct; XDOWP(T,TH,I,zv,hash,exp,stride,asct); \
     if(md==IIDOT)XDOWA(T,TH,I,zv,hash,exp,stride,i,allfound1)else XDQWA(T,TH,I,zv,hash,exp,stride,i,allfound1)  allfound1: XDOWINDCLASS(zv); } break;  \
   case IEPS: {B * RESTRICT zb=BAV(z)+l*wsct; XDOWP(T,TH,B,zb,hash,exp,stride,0); XDOWA(T,TH,B,zb,hash,exp,stride,1,allfound2); allfound2: XDOWINDCLASS(zb); } break; \
   } \
  }                                                                                  \
  R h;                                                                               \
 }

IOFXW(A, US,jtiowax1,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(wv[hj])), 1) // boxed exact 1-element item
IOFXW(A, US,jtiowau,, hiau(C(*v)),      !equ(C(*v),C(wv[hj])),     1) // boxed uniform type
IOFXW(X, US,jtiowx,,  hix(v),           !eqnx(n,v,wv+n*hj),       cn) // extended integer
IOFXW(Q, US,jtiowq,,  hiq(v),           !eqnq(n,v,wv+n*hj),       cn) // rational number
IOFXW(C, US,jtiowc,,  hic(k,(UC*)v),    memcmpne(v,wv+k*hj,k),    cn) // boolean, char, or integer
IOFXW(I, US,jtiowi,COMPSETUP,hici(n,v),COMPCALL(wv),              cn) // INT array, not float
IOFXW(C2,US,jtiow21,, hici1((C2*)v),    *v!=wv[hj],                1) // 2-byte (char)
IOFXW(C4,US,jtiow41,, hici1((C4*)v),    *v!=wv[hj],                1) // 4-byte (char)
IOFXW(I, US,jtiowi1,, hici1(v),         *v!=wv[hj],                1) // len=8, not float
IOFXW(I, US,jtiow161,,hici2(v[0],v[1]), cmpi2(v,wv+2*hj),          2) // len=8, not float
IOFXW(D, US,jtiowc01,,hic01((UIL*)v),   *v!=wv[hj],                1) // float atom
IOFXW(Z, US,jtiowz01,,hic0(2,(UIL*)v),  (v[0].re!=wv[hj].re)||(v[0].im!=wv[hj].im), 1) // complex atom
IOFXW(D, US,jtiowc0,, hic0(n,(UIL*)v),  fcmp0(v,&wv[n*hj],n),     cn) // float array
IOFXW(Z, US,jtiowz0,, hic0(2*n,(UIL*)v),fcmp0((D*)v,(D*)&wv[n*hj],2*n),cn) // complex array

IOFXW(A, UI4,jtiowax12,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(wv[hj])), 1) // boxed exact 1-element item
IOFXW(A, UI4,jtiowau2,, hiau(C(*v)),      !equ(C(*v),C(wv[hj])),     1) // boxed uniform type
IOFXW(X, UI4,jtiowx2,,  hix(v),           !eqnx(n,v,wv+n*hj),       cn) // extended integer
IOFXW(Q, UI4,jtiowq2,,  hiq(v),           !eqnq(n,v,wv+n*hj),       cn) // rational number
IOFXW(C, UI4,jtiowc2,,  hic(k,(UC*)v),    memcmpne(v,wv+k*hj,k),    cn) // boolean, char, or integer
IOFXW(I, UI4,jtiowi2,COMPSETUP,hici(n,v),COMPCALL(wv),              cn) // INT array, not float
IOFXW(C2,UI4,jtiow212,, hici1((C2*)v),    *v!=wv[hj],                1) // 2-byte (char)
IOFXW(C4,UI4,jtiow412,, hici1((C4*)v),    *v!=wv[hj],                1) // 4-byte (char)
IOFXW(I, UI4,jtiowi12,, hici1(v),         *v!=wv[hj],                1) // len=8, not float
IOFXW(I, UI4,jtiow1612,,hici2(v[0],v[1]), cmpi2(v,wv+2*hj),          2) // len=16, not float
IOFXW(D, UI4,jtiowc012,,hic01((UIL*)v),   *v!=wv[hj],                1) // float atom
IOFXW(Z, UI4,jtiowz012,,hic0(2,(UIL*)v),  (v[0].re!=wv[hj].re)||(v[0].im!=wv[hj].im), 1) // complex atom
IOFXW(D, UI4,jtiowc02,, hic0(n,(UIL*)v),  fcmp0(v,&wv[n*hj],n),           cn) // float array
IOFXW(Z, UI4,jtiowz02,, hic0(2*n,(UIL*)v),fcmp0((D*)v,(D*)&wv[n*hj],2*n),  cn) // complex array
#endif
