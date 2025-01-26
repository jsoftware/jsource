#include "viavx.h"

// requires: suff and TH to be defined

// *************** i., first class: intolerant comparisons, unboxed or boxed ***********************

// Routines to build the hash table from a.  hash calculates the hash function, usually referring to v (the input) and possibly other names.  exp is the comparison routine.

// special lookup routines to move the data rather than store its index, used for nub/less
#if 0  // obsolete 
#define XMVP(T,TH,hash,exp,stride,reflex) C *zc=(C*)zv;      \
 if(k==SZI){XDOP(T,TH,hash,exp,stride,{},{printf("i=%lld, (I*)_mm_extract_epi64(vp,1)=%p, wv+i=%p\n",i,(I*)_mm_extract_epi64(vp,1),wv+i); *(I*)zc=*(I*)_mm_extract_epi64(vp,1); zc+=SZI;},reflex); }  \
 else      {XDOP(T,TH,hash,exp,stride,{},{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k;},reflex); } ZCSHAPE;
#else
#define XMVP(T,TH,hash,exp,stride,reflex,XSEARCH)    \
 if(k==SZI){I *zc=zv; XDOP(T,TH,hash,exp,stride,{},{*zc=((I*)wv)[i]; ++zc;},reflex,XSEARCH); ZCSHAPEI; }  \
 else{C *zc=(C*)zv; XDOP(T,TH,hash,exp,stride,{},{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k;},reflex,XSEARCH); ZCSHAPE; }
#endif
// version for ([ -. -.)
#if 0  // obsolete 
#define XMVPI(T,TH,hash,exp,stride,reflex,XSEARCH)   C *zc=(C*)zv;    \
 if(k==SZI){XDOP(T,TH,hash,exp,stride,{*(I*)zc=*(I*)_mm_extract_epi64(vp,1); zc+=SZI;},{},reflex,XSEARCH); }  \
 else      {XDOP(T,TH,hash,exp,stride,{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k;},{},reflex,XSEARCH); } ZCSHAPE;
#else
#define XMVPI(T,TH,hash,exp,stride,reflex,XSEARCH)    \
 if(k==SZI){I *zc=zv; XDOP(T,TH,hash,exp,stride,{*zc=((I*)wv)[i]; ++zc;},{},reflex,XSEARCH); ZCSHAPEI; }  \
 else{C *zc=(C*)zv; XDOP(T,TH,hash,exp,stride,{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k;},{},reflex,XSEARCH); ZCSHAPE; }
#endif
// version for ~. inplace
#define XMVPIP(T,TH,hash,exp,stride,reflex,XSEARCH)   wsct=0;    \
 if(k==SZI){XDOPIP(T,TH,hash,exp,stride,{},{zv[wsct]=zv[i]; ++wsct;},reflex,XSEARCH); }  \
 else      {C *zc=(C*)zv; XDOPIP(T,TH,hash,exp,stride,{},{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k; ++wsct;},reflex,XSEARCH);  } \
 AN(z)=n*(AS(z)[0]=wsct);   /* wsct items, but there may be atoms/item */

// The main search routine, given a, w, mode, etc, for datatypes with no comparison tolerance

//ugh
#define CONCAT0(x,y) x##y
#define CONCAT(x,y) CONCAT0(x,y)

// if there is not a prehashed hashtable, we clear the hashtable and fill it from a, then hash & check each item of w
#define IOFX(T,f,setup,hash,exp,stride,XSEARCH)   \
 IOF(CONCAT(f,suff)){I acn=ak/sizeof(T),cn=k/sizeof(T),l,p,  \
        wcn=wk/sizeof(T),*zv=AV(z);T* RESTRICT av=(T*)AV(a),* RESTRICT wv=(T*)AV(w);I md; TH * RESTRICT hv; \
        IH *hh=IHAV(h); p=hh->datarange;  hv=hh->data.TH;  \
 \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  setup \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */                                            \
    /* look for IIDOT/IICO/INUBSV/INUB/INUBI/IFORKEY - we set IIMODREFLEX if one of those is set */ \
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md|=IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBIP)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md);  /* remember if this is reflexive, which doesn't prehash */  \
  if(w==mark){wsct=0;}   /* if prehashing, turn off the second half */                          \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv;  \
  for(l=0;l<ac;++l,av+=acn,wv+=--wreps<0?wcn:0,wreps=wreps<0?wrepssv:wreps){                                                 \
   /* zv progresses through the result - for those versions that support IRS */ \
   if(!(mode&IPHOFFSET)){hashallo(hh,p,asct,mode); if(!(md&IIMODREFLEX)){if(md==IICO)XDQAP(T,TH,hash,exp,stride,XSEARCH) else XDOAP(T,TH,hash,exp,stride,XSEARCH);} if(wsct==0)break;}  \
   switch(md){ \
    /* i.~ - one-pass operation.  Fill in the table and result as we go */ \
   case IIDOT|IIMODREFLEX: { XDOP(T,TH,hash,exp,stride,{zv[i]=hj;},{zv[i]=i;},1,XSEARCH); zv+=wsct;} break;      \
   case IFORKEY|IIMODREFLEX: { I nuniq=0; XDOP(T,TH,hash,exp,stride,{++zv[hj]; zv[i]=hj;},{++nuniq; zv[i]=i+1;},1,XSEARCH); AM(h)=nuniq;} break;      \
   case IICO|IIMODREFLEX: {XDQP(T,TH,hash,exp,stride,{zv[i]=hj;},{zv[i]=i;},1,XSEARCH) zv+=wsct;} break;      \
    /* normal i./i: - use the table */ \
   case IICO: \
   case IIDOT: { XDOP(T,TH,hash,exp,stride,{zv[i]=hj;},{zv[i]=hj;},0,XSEARCH); zv+=wsct; }                          break;  \
   case INUBSV|IIMODREFLEX: { B *zb=(B*)zv; XDOP(T,TH,hash,exp,stride,{zb[i]=0;},{zb[i]=1;},1,XSEARCH) zv=(I*)(zb+=wsct);} /* IRS - keep zv running */  break;  \
   case INUBIP|IIMODREFLEX: {       XMVPIP(T,TH,hash,exp,stride,3,XSEARCH);              }   break;  \
   case INUB|IIMODREFLEX: {       XMVP(T,TH,hash,exp,stride,1,XSEARCH);              }   break;  \
   case ILESS: { XMVP(T,TH,hash,exp,stride,0,XSEARCH);              }   break;  \
   case IINTER: { XMVPI(T,TH,hash,exp,stride,0,XSEARCH);          }   break;  \
   case INUBI|IIMODREFLEX: {I *zi=zv;  XDOP(T,TH,hash,exp,stride,{},{*zi++=i;},1,XSEARCH) ZISHAPE; }   break;  \
   case IEPS: { B *zb=(B*)zv;  XDOP(T,TH,hash,exp,stride,{zb[i]=1;},{zb[i]=0;},0,XSEARCH) zv=(I*)(zb+=wsct);} /* this has IRS, so zv must be kept right */                       break;  \
    /* the rest are f@:e., none of which have IRS */ \
   case II0EPS: { I s; XDOP(T,TH,hash,exp,stride,{},{s=i; goto exit0;},0,XSEARCH); s=wsct; exit0: *zv=s; }   break; /* i.&0@:e. */   \
   case II1EPS: { I s; XDOP(T,TH,hash,exp,stride,{s=i; goto exit1;},{},0,XSEARCH); s=wsct; exit1: *zv=s; }  break; /* i.&1@:e. */  \
   case IJ0EPS: { I s; XDQP(T,TH,hash,exp,stride,{},{s=i; goto exit2;},0,XSEARCH); s=wsct; exit2: *zv=s; }   break; /* i:&0@:e. */  \
   case IJ1EPS: { I s; XDQP(T,TH,hash,exp,stride,{s=i; goto exit3;},{},0,XSEARCH); s=wsct; exit3: *zv=s; }   break; /* i:&1@:e. */  \
   case ISUMEPS: { I s=0; XDOP(T,TH,hash,exp,stride,{++s;},{},0,XSEARCH); *zv=s; }   break; /* +/@:e. */  \
   case IANYEPS: { B s; XDOP(T,TH,hash,exp,stride,{s=1; goto exit5;},{},0,XSEARCH); s=0; exit5: *(B*)zv=s; } break; /* +./@:e. */  \
   case IALLEPS: { B s; XDOP(T,TH,hash,exp,stride,{},{s=0; goto exit6;},0,XSEARCH); s=1; exit6: *(B*)zv=s; } break; /* *./@:e. */  \
   case IIFBEPS: { I *zi=zv; XDOP(T,TH,hash,exp,stride,{*zi++=i;},{},0,XSEARCH); ZISHAPE; }   break; /* I.@:e. */  \
   } \
  }                                                                                 \
  R h;                                                                               \
 }

// jtioa* BOX
// jtiox  XNUM
// jtioq  RAT
// jtioi1 k==SZI, INT/SBT/char/bool not small-range
// jtioi  list of >1 INT
// jtioCk k-sized bool/char
// jtioc  k!=SZI, bool (must be list of em)/char/INT/SBT
// jtioc01 intolerant FL atom
// jtioc0 intolerant FL array
// jtioz01 intolerant CMPX atom
// jtioz0 intolerant CMPX array

IOFX(A, jtioax1,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(av[hj])),1,XSEARCHV) // boxed exact 1-element item
IOFX(A, jtioau,, hiau(C(*v)),      !equ(C(*v),C(av[hj])),    1,XSEARCHV) // boxed uniform type
IOFX(X, jtiox,,  hix(v),           !eqnx(n,v,av+n*hj),      cn,XSEARCHV) // extended integer
IOFX(Q, jtioq,,  hiq(v),           !eqnq(n,v,av+n*hj),      cn,XSEARCHV) // rational number
IOFX(C, jtioc,,  hic(k,(UC*)v),    memcmpne(v,av+k*hj,k),   cn,XSEARCHV) // boolean, char, or integer*
IOFX(I, jtioi,COMPSETUP,hici(n,v),COMPCALL(av),             cn,XSEARCHV) // INT array, not float
IOFX(C2,jtioC2,, hici1((C2*)v),    *v!=av[hj],               1,XSEARCHV) // 2-byte (char/INT2)
IOFX(C4,jtioC4,, hici1((C4*)v),    *v!=av[hj],               1,XSEARCHV) // 4-byte (char/INT4)
IOFX(I, jtioi1,, hici1(v),         *v!=av[hj],               1,XSEARCHV) // len=8, not float
IOFX(I, jtio16,,hici2(v[0],v[1]), cmpi2(v,av+2*hj),         2,XSEARCHV) // len=16, not float
IOFX(D, jtioc01,,hic01((UIL*)v),   *v!=av[hj],               1,XSEARCHV) // float atom
IOFX(Z, jtioz01,,hic0(2,(UIL*)v),  (v[0].re!=av[hj].re)||(v[0].im!=av[hj].im), 1,XSEARCHV) // complex atom
IOFX(D, jtioc0,, hic0(n,(UIL*)v),  fcmp0(v,&av[n*hj],n),    cn,XSEARCHV) // float array
IOFX(Z, jtioz0,, hic0(2*n,(UIL*)v),fcmp0((D*)v,(D*)&av[n*hj],2*n),cn,XSEARCHV) // complex array
