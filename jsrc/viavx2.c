/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"
#if C_CRC32C && SY_64
#include "viavx.h"

// ********************* i., second class: tolerant comparisons, possibly boxed **********************

// create tolerant hash for a single D
// Note: the masking may cause a nonzero to hash to negative zero.  This is OK, because any nonzero will not be
// tequal to +0 or -0.  But we must ensure that -0 hashes to the same value as +0, since those two numbers are equal.
#define HIDMSK(v) (likely(*(UIL*)v!=(UIL)NEGATIVE0)?CRC32LL(-1L,*(UIL*)v&ctmask):CRC32LL(-1L,0))
// save the mask result in m
#define HIDMSKSV(m,v) ((m=*(UIL*)v&ctmask), (likely(*(UIL*)v!=(UIL)NEGATIVE0)?CRC32LL(-1L,m):CRC32LL(-1L,0)) )
// save the unmasked D value (as bits) in m
#define HIDUMSKSV(m,v) ((m=*(UIL*)v), (likely(m!=(UIL)NEGATIVE0)?CRC32LL(-1L,m&ctmask):CRC32LL(-1L,0)) )
// create hash for a D type
#define HID(y)              CRC32LL(-1L,(y))
#define MASK(dd,xx)         {D dv=(xx); if(likely(*(UIL*)&dv!=(UIL)NEGATIVE0)){dd=*(UIL*)&dv&ctmask;}else{dd=0;} }

// FIND for read.  Stop loop if endtest is true; execute fstmt if match ((exp) is false).  hj holds the index of the value being tested
#define FINDRD(exp,hindex,endtest,fstmt) do{hj=hv[hindex]; if(endtest)break;if(!(exp)){fstmt break;}if(unlikely(--hindex<0))hindex+=p;}while(1);

// FIND for write.  j is the scan pointer through the hashtable, and has been initialized to the starting bucket
// store i into the hashtable if not found.  The test should be intolerant except for NUB/KEY operation
#define FINDWR(TH,exp) NOUNROLL do{if(asct==(hj=hv[j])){hv[j]=(TH)i; break;}if(!(exp))break;if(unlikely(--j<0))j+=p;}while(1);

// functions for building the hash table for tolerant comparison.  expa is the function for detecting matches on a values

// find a tolerant match for *v.  The hashtable has already been created.
// We have to look in two buckets: for the interval containing the value, and for the neighboring interval
// containing tolerantly equal values.  Because the interval is always at least as wide as the
// tolerant diameter (i. e. 2 tolerances wide), only one neighboring interval need be checked.
// For the default tolerance, the interval width is just a smidgen above the diameter for intervals
// near 1.1111; it is twice the diameter for intervals near 1.00000.  This means that a random
// value will have its tolerance circle spilling into the neighboring interval about
// 3/4 of the time.  because of that, we don't bother to check whether the circle actually
// does overlap [that would require calculating the upper and lower bounds and then doing bit fiddling on the result];
// we simply probe in the neighboring interval closer to the given value.
// We stack up as much calculation as possible before the searches to reduce the cost of a misprediction
//
// We calculate a half-interval above and below the value; one of those is in the neighboring interval,
// the other is in the same interval; we XOR to preserve the neighbor
//
// At the end of this calculation il contains the index of a match, or asct if no match.

// set dx to the 'other' interval from *v.
#define SETXVAL  xval=_mm256_castpd256_pd128(_mm256_broadcast_sd((D*)v)); xnew=_mm_mul_pd(xval,tltr); xrot=_mm_permute_pd(xnew,0x1); xnew=_mm_xor_pd(xnew,xval); xnew=_mm_xor_pd(xnew,xrot); dx=_mm_extract_epi64(_mm_castpd_si128(xnew),0);
#define TFINDXYT(TH,expa,expw,fstmt0,endtest1,fstmt1)  \
 {UIL dx;       \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; \
  SETXVAL \
  HASHSLOT(HID(dx&=ctmask)) FINDRD(expw,jx,asct==hj,fstmt0); il=hj; \
  FINDRD(expw,j,endtest1,fstmt1); \
 }
// Same idea, but used for reflexives, where the table has not been built yet.  We save replicating the hash calculation, and also
// we know that there will be a match in the first search, which simplifies that search.
// For this routine expa MUST be an intolerant comparison
#define TFINDY1T(TH,expa,expw,fstmt0,endtest1,fstmt1)  \
 {UIL dx;      \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; \
  SETXVAL \
  FINDWR(TH,expa);  \
  HASHSLOT(HID(dx&=ctmask)) FINDRD(expw,jx,0,fstmt0); il=hj; \
  FINDRD(expw,j,endtest1,fstmt1); \
 }
// This version for nub/key.  We read first, and add to the table only if the value was not found
#define TFINDY1TKEY(TH,expa,expw,fstmt0,endtest1,fstmt1)  \
 {UIL dx;       \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; /* j=jx=main interval */ \
  SETXVAL \
  FINDRD(expw,jx,asct==hj,fstmt0); il=hj; /* read main interval */ \
  HASHSLOT(HID(dx&=ctmask)) /* j=other interval*/ \
  FINDRD(expw,j,endtest1,fstmt1); /* read other interval, set il to place found if any */ \
  if(il>=i){j=jx; FINDWR(TH,expa); il=i;} /* if new value, add to hash */ \
 }


// here comparing boxes.  Consider modifying hia to take a self/neighbor flag rather than a tolerance
#define TFINDBX(TH,expa,expw,fstmt0,endtest1,fstmt1)   \
 {HASHSLOT(cthia(ctmask,tl,C(*v))) jx=j; FINDRD(expw,j,asct==hj,fstmt0); il=hj;   \
 HASHSLOT(cthia(ctmask,tr,C(*v))) if(j!=jx){FINDRD(expw,j,endtest1,fstmt1);}  \
 }
// reflexive.  Because the compare for expa does not force intolerance, we must do so here.  This is required only for boxes, since the other expas are intolerant
#define TFINDBY(TH,expa,expw,fstmt0,endtest1,fstmt1)   \
 {HASHSLOT(cthia(ctmask,1.0,C(*v)))  PUSHCCT(1.0) FINDWR(TH,expa); POPCCT TFINDBX(TH,expa,expw,fstmt0,endtest1,fstmt1)}

// reflexive for nub/key.  First search the table, then insert only if no match found.
// Each FINDRD sets hj, leaving il and hj at the end; set il to be the lower.
// if not found, set il=i (that is, match on the newly-added element), and add that element
#define TFINDBYKEY(TH,expa,expw,fstmt0,endtest1,fstmt1)   \
 {HASHSLOT(cthia(ctmask,tl,C(*v))) jx=j; FINDRD(expw,j,asct==hj,fstmt0); il=hj;   \
 HASHSLOT(cthia(ctmask,tr,C(*v))) if(j!=jx){FINDRD(expw,j,endtest1,fstmt1); }  \
 if(il>=i){HASHSLOT(cthia(ctmask,1.0,C(*v)))  FINDWR(TH,expa); il=i;} \
 }

// ******* macros to process each item of w *******
// FXY is a TFIND macro, charged with setting il.
// Set il, execute the statement, advance to the next item
#define TDOXY(T,TH,FXY,expa,expw,fstmt0,endtest1,fstmt1,stmt)  \
 DO(wsct, FXY(TH,expa,expw,fstmt0,endtest1,fstmt1); stmt; v=(T*)((C*)v+k);)

// Same, but search from the end of y backwards (e. i: 0 etc)
#define TDQXY(T,TH,FXY,expa,expw,fstmt0,endtest1,fstmt1,stmt)  \
 v=(T*)((C*)v+k*(wsct-1)); DQ(wsct, FXY(TH,expa,expw,fstmt0,endtest1,fstmt1); stmt; v=(T*)((C*)v-k);)

// Version for ~. y and x -. y .  move the item to *zc++
// If we know the item is a single value we store it always and ratify the value by incrementing the pointer, using conditional instructions
// If the item might be long we move it only if it is valid
// For -.
#define TMVX(T,TH,FXY,expa,expw)   \
  {if(k==sizeof(T)){DQ(wsct, FXY(TH,expa,expw,goto found3;,hj==asct,il=hj;); *(T*)zc=*(T*)v; zc+=(il==asct)*sizeof(T); found3: v=(T*)((C*)v+k); );  \
            }else{DQ(wsct, FXY(TH,expa,expw,goto found2;,hj==asct,goto found2;); {MC(zc,v,k); zc+=k;}; found2: v=(T*)((C*)v+k); );}  \
 }
// For ([ -. -.)
#define TMVXI(T,TH,FXY,expa,expw)   \
  {if(k==sizeof(T)){DQ(wsct, FXY(TH,expa,expw,{},hj==asct,il=hj;); *(T*)zc=*(T*)v; zc+=(il!=asct)*sizeof(T); v=(T*)((C*)v+k); );  \
            }else{DQ(wsct, FXY(TH,expa,expw,{},hj==asct,il=hj;); if(il!=asct){MC(zc,v,k); zc+=k;}; v=(T*)((C*)v+k); );}  \
 }
// for ~.  Same idea, but reflexive.  FYY is always TFINDBYKEY
#define TMVY(T,TH,FYY,expa,expw)   \
  {if(k==sizeof(T)){DO(wsct, FYY(TH,expa,expw,{},hj==asct,il=hj;); *(T*)zc=*(T*)v; zc+=(i==il)*sizeof(T); v=(T*)((C*)v+k); );     \
             }else{DO(wsct, FYY(TH,expa,expw,{},hj==asct,il=hj;); if(il==i){MC(zc,v,k); zc+=k;}; v=(T*)((C*)v+k); );}   \
 }

// put these in dcls to declare names that will be used
#define INITXVAL  __m128d xval=_mm_setzero_pd();
#define INITTLTR __m128d tltr=_mm_set_pd(tl,tr);
#define INITXNEW   __m128d xnew=xval;
#define INITXROT  __m128d xrot=xval;

// Do the operation.  Build a hash for a except when self-index
// FXY is a TFIND macro, charged with setting il.
// FYY similar, for reflexive searches
// FYYKEY similar for nub/key - they add to the hash only if noun found
// expa - test for matches during hashing.  expa is true for NO match.  n v av hj are available as parameters
// expw - test for matches during lookup, i. e. reading hashtable.  expw is true for NO match.  n v av hj are available as parameters
#define IOFT(T,TH,f,hash,FXY,FYY,FYYKEY,expa,expw,dcls)   \
 IOF(f){I acn=ak/sizeof(T),  \
        wcn=wk/sizeof(T),* zv=AV(z);T* RESTRICT av=(T*)AV(a),* wv=(T*)AV(w);I md; \
        D tl=jt->cct,tr=1/tl;I il,jx;   \
        IH *hh=IHAV(h); I p=hh->datarange; TH * RESTRICT hv=hh->data.TH; UIL ctmask=calcctmask(jt->cct);   \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  INITXVAL dcls; \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */                            \
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md|=(IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md));  /* remember if this is reflexive, which doesn't prehash */  \
  jx=0;                                                                     \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv; if(w==mark){wsct=0;} \
  for(;ac>0;av+=acn,--ac,wv+=--wreps<0?wcn:0,wreps=wreps<0?wrepssv:wreps){                   \
   if(!(mode&IPHOFFSET)){  /* if we are not using a prehashed table */                                        \
    hashallo(hh,p,asct,mode);                                                           \
    if(!(IIMODREFLEX&md)){I cn= k/sizeof(T);  /* not reflexive */                                            \
     PUSHCCT(1.0) if(md==IICO)XDQAP(T,TH,hash,expa,cn) else XDOAP(T,TH,hash,expa,cn); POPCCT  /* all writes to hash must use intolerant compare */                \
     if(w==mark)break;                                                                \
    } \
   }                                                                                            \
   switch(md){                                                                                   \
   /* when we are searching up, we can stop the second search when it gets past the index found in the first search */ \
   case IIDOT: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zi++=il;); zv=zi; } break;  \
   case IIDOT|IIMODREFLEX: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYY,expa,expw,{},hj>=il,il=hj;,*zi++=il;); zv=zi; } break;  \
   case IFORKEY|IIMODREFLEX: {T * RESTRICT v=wv; I nuniq=0; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,nuniq+=i==il;*zi=il;zv[il]++;++zi;); AM(h)=nuniq; zv=zi; } break;  \
   case IICO:  {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi; zi=zv+=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj==asct,il=il==asct?hj:il;il=hj>il?hj:il;,*--zi=il;);} break;  \
   case IICO|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi; zi=zv+=wsct; TDQXY(T,TH,FYY,expa,expw,{},hj==asct,il=il==asct?hj:il;il=hj>il?hj:il;,*--zi=il;);} break;  \
   case INUBSV|IIMODREFLEX:{T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,*zb++=i==il;); zv=(I*)zb;} break;  /* zv must keep running */  \
   case INUB|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVY(T,TH,FYYKEY,expw,expw); ZCSHAPE; }    break;  \
   case ILESS: {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVX(T,TH,FXY,expa,expw); ZCSHAPE; }    break;  \
   case IINTER: {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVXI(T,TH,FXY,expa,expw); ZCSHAPE; }    break;  \
   case INUBI|IIMODREFLEX: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,*zi=i; zi+=(i==il);); ZISHAPE;} break;  \
   case IEPS:  {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zb++=(il!=asct);); zv=(I*)zb;} break;   /* zv must keep running */ \
   case II0EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=i; break;}); *zi=s;} break;  \
   case II1EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct!=il){s=i; break;}); *zi=s;} break;  \
   case IJ0EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=i; break;}); *zi=s;} break;  \
   case IJ1EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct!=il){s=i; break;}); *zi=s;} break;  \
   case ISUMEPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=0; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,s+=(asct>il);); *zi=s;}  break;  \
   case IANYEPS: {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; I s=0; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct>il){s=1; break;}); *zb=(B)s;} break;  \
   case IALLEPS: {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; I s=1; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=0; break;}); *zb=(B)s;} break;  \
   case IIFBEPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zi=i; zi+=(asct>il);); ZISHAPE;} break;  \
   } \
  }                                                                                             \
  R h;                                                                                           \
 }

// Verbs for the types of inputs
// jtio[dzea FL/CMPX/QP/BOX][1 if atom][2 if long table] all tolerant

// hashes using the 2-byte hashtable
// FL array
IOFT(D,US,jtiod, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0(v,av+n*hj,n  ), !jeqd(n,v,av+n*hj,tl),INITTLTR INITXNEW INITXROT D x)
// FL atom
IOFT(D,US,jtiod1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,*v!=av[hj],                       !TCMPEQ(tl,*(D*)v,av[hj] ),INITTLTR INITXNEW INITXROT D x)
// CMPLX array
IOFT(Z,US,jtioz, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !eqz(n,v,av+n*hj),INITTLTR INITXNEW INITXROT D x)
// CMPLX atom
IOFT(Z,US,jtioz1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !zeq( *v,av[hj] ),INITTLTR INITXNEW INITXROT D x)
// QP array
IOFT(E,US,jtioe, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !jteqe(jt,n,v,&av[n*hj]),INITTLTR INITXNEW INITXROT D x)
// QP atom
IOFT(E,US,jtioe1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !jteqe(jt,1,v,&av[hj] ),INITTLTR INITXNEW INITXROT D x)
// boxed array with more than 1 box
IOFT(A,US,jtioa, cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!eqa(n,v,av+n*hj),          !eqa(n,v,av+n*hj),)
// singleton box
IOFT(A,US,jtioa1,cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!equ(C(*v),C(av[hj])),!equ(C(*v),C(av[hj])),)

// hashes using the 4-byte hashtable
// FL array
IOFT(D,UI4,jtiod2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0(v,av+n*hj,n  ), !jeqd(n,v,av+n*hj,tl),INITTLTR INITXNEW INITXROT D x)
// FL atom
IOFT(D,UI4,jtiod12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,*v!=av[hj],                       !TCMPEQ(tl,*(D*)v,av[hj] ),INITTLTR INITXNEW INITXROT D x)
// CMPLX array
IOFT(Z,UI4,jtioz2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !eqz(n,v,av+n*hj),INITTLTR INITXNEW INITXROT D x)
// CMPLX atom
IOFT(Z,UI4,jtioz12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !zeq( *v,av[hj] ),INITTLTR INITXNEW INITXROT D x)
// QP array
IOFT(E,UI4,jtioe2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !jteqe(jt,n,v,&av[n*hj]),INITTLTR INITXNEW INITXROT D x)
// QP atom
IOFT(E,UI4,jtioe12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !jteqe(jt,1,v,&av[hj] ),INITTLTR INITXNEW INITXROT D x)
// boxed array with more than 1 box
IOFT(A,UI4,jtioa2, cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!eqa(n,v,av+n*hj),          !eqa(n,v,av+n*hj), D x)
// singleton box
IOFT(A,UI4,jtioa12,cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!equ(C(*v),C(av[hj])),!equ(C(*v),C(av[hj])),  D x)
#endif
