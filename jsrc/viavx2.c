/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
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
// The test should be intolerant except for NUB/KEY operation
// store (storevbl) into the hashtable if not found.
#define FINDWR(TH,exp,storevbl) NOUNROLL do{if(asct==(hj=hv[j])){hv[j]=(TH)storevbl; break;}if(!(exp))break;if(unlikely(--j<0))j+=p;}while(1);

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

#define TFINDXYT(TH,expa,expw,fstmt0,endtest1,fstmt1,storevblunused)  \
 {UIL dx;       \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; \
  SETXVAL \
  HASHSLOT(HID(dx&=ctmask)) FINDRD(expw,jx,asct==hj,fstmt0); il=hj; \
  FINDRD(expw,j,endtest1,fstmt1); \
 }
// Same idea, but used for reflexives, where the table has not been built yet.  We save replicating the hash calculation, and also
// we know that there will be a match in the first search, which simplifies that search.
// For this routine expa MUST be an intolerant comparison
#define TFINDY1T(TH,expa,expw,fstmt0,endtest1,fstmt1,storevblunused)  \
 {UIL dx;      \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; \
  SETXVAL \
  FINDWR(TH,expa,i);  \
  HASHSLOT(HID(dx&=ctmask)) FINDRD(expw,jx,0,fstmt0); il=hj; \
  FINDRD(expw,j,endtest1,fstmt1); \
 }
// This version for nub/key.  We read first, and add to the table only if the value was not found.  At end il=i if the value was added
#define TFINDY1TKEY(TH,expa,expw,fstmt0,endtest1,fstmt1,storevbl)  \
 {UIL dx;       \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; /* j=jx=main interval */ \
  SETXVAL \
  FINDRD(expw,jx,asct==hj,fstmt0); il=hj; /* read main interval */ \
  HASHSLOT(HID(dx&=ctmask)) /* j=other interval*/ \
  FINDRD(expw,j,endtest1,fstmt1); /* read other interval, set il to place found if any */ \
  if(il>=i){j=jx; FINDWR(TH,expa,storevbl); il=i;} /* if new value, add to hash */ \
 }


// here comparing boxes.  Consider modifying hia to take a self/neighbor flag rather than a tolerance
#define TFINDBX(TH,expa,expw,fstmt0,endtest1,fstmt1,storevblunused)   \
 {HASHSLOT(cthia(ctmask,tl,C(*v))) jx=j; FINDRD(expw,j,asct==hj,fstmt0); il=hj;   \
 HASHSLOT(cthia(ctmask,tr,C(*v))) if(j!=jx){FINDRD(expw,j,endtest1,fstmt1);}  \
 }
// reflexive.  Because the compare for expa does not force intolerance, we must do so here.  This is required only for boxes, since the other expas are intolerant
#define TFINDBY(TH,expa,expw,fstmt0,endtest1,fstmt1,storevblunused)   \
 {HASHSLOT(cthia(ctmask,1.0,C(*v)))  PUSHCCT(1.0) FINDWR(TH,expa,i); POPCCT TFINDBX(TH,expa,expw,fstmt0,endtest1,fstmt1,)}

// reflexive for nub/key.  First search the table, then insert only if no match found.
// Each FINDRD sets hj, leaving il and hj at the end; set il to be the lower.
// if not found, set il=i (that is, match on the newly-added element), and add that element
#define TFINDBYKEY(TH,expa,expw,fstmt0,endtest1,fstmt1,storevbl)   \
 {HASHSLOT(cthia(ctmask,tl,C(*v))) jx=j; FINDRD(expw,j,asct==hj,fstmt0); il=hj;   \
 HASHSLOT(cthia(ctmask,tr,C(*v))) if(j!=jx){FINDRD(expw,j,endtest1,fstmt1); }  \
 if(il>=i){HASHSLOT(cthia(ctmask,1.0,C(*v)))  FINDWR(TH,expa,storevbl); il=i;} \
 }

// ******* macros to process each item of w *******
// FXY is a TFIND macro, charged with setting il.
// Set il, execute the statement, advance to the next item
#define TDOXY(T,TH,FXY,expa,expw,fstmt0,endtest1,fstmt1,stmt,storevbl)  \
 DO(wsct, FXY(TH,expa,expw,fstmt0,endtest1,fstmt1,storevbl); stmt; v=(T*)((C*)v+k);)

// Same, but search from the end of y backwards (e. i: 0 etc)
#define TDQXY(T,TH,FXY,expa,expw,fstmt0,endtest1,fstmt1,stmt)  \
 v=(T*)((C*)v+k*(wsct-1)); DQ(wsct, FXY(TH,expa,expw,fstmt0,endtest1,fstmt1,); stmt; v=(T*)((C*)v-k);)

// Versions for ~. y  x -. y    x ([-.-.) y.  move the item to *zc++
// If we know the item is a single value we store it always and ratify the value by incrementing the pointer, using conditional instructions
// If the item might be long we move it only if it is valid
// For -.
#define TMVX(T,TH,FXY,expa,expw)   \
 {if(k==sizeof(T)){DQ(wsct, FXY(TH,expa,expw,goto found3;,hj==asct,il=hj;,); *(T*)zc=*(T*)v; zc+=(il==asct)*sizeof(T); found3: v=(T*)((C*)v+k); );  \
            }else{DQ(wsct, FXY(TH,expa,expw,goto found2;,hj==asct,goto found2;,); {MC(zc,v,k); zc+=k;}; found2: v=(T*)((C*)v+k); );}  \
 }
// For ([ -. -.)
#define TMVXI(T,TH,FXY,expa,expw)   \
 {if(k==sizeof(T)){DQ(wsct, FXY(TH,expa,expw,{},hj==asct,il=hj;,); *(T*)zc=*(T*)v; zc+=(il!=asct)*sizeof(T); v=(T*)((C*)v+k); );  \
            }else{DQ(wsct, FXY(TH,expa,expw,{},hj==asct,il=hj;,); if(il!=asct){MC(zc,v,k); zc+=k;}; v=(T*)((C*)v+k); );}  \
 }
// for ~.  Same idea, but reflexive.  FYY is always TFINDBYKEY
#define TMVY(T,TH,FYY,expa,expw,storevbl)   \
 {if(k==sizeof(T)){DO(wsct, FYY(TH,expa,expw,{},hj==asct,il=hj;,storevbl); *(T*)zc=*(T*)v; zc+=(i==il)*sizeof(T); v=(T*)((C*)v+k); );     \
             }else{DO(wsct, FYY(TH,expa,expw,{},hj==asct,il=hj;,storevbl); if(il==i){MC(zc,v,k); zc+=k;}; v=(T*)((C*)v+k); );}   \
 }
//  ~. inplace.  reflexive and we don't need 3 pointers.  FYY is always TFINDBYKEY.  wsct is the current result index and is storevbl
#define TMVYIP(T,TH,FYY,expa,expw,storevbl)  wsct=0;  \
 {if(k==sizeof(T)){DO(asct, FYY(TH,expa,expw,{},hj==asct,il=hj;,storevbl); ((T*)av)[wsct]=*(T*)v; wsct+=(i==il); v=(T*)((C*)v+sizeof(T)); );     \
             }else{DO(asct, FYY(TH,expa,expw,{},hj==asct,il=hj;,storevbl); if(il==i){MC(zc,v,k); zc+=k; ++wsct;}; v=(T*)((C*)v+k); );}   \
  AN(z)=n*(AS(z)[0]=wsct);   /* wsct items, but there may be atoms/item */ \
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
#define IOFT(T,TH,f,hash,FXY,FYY,FYYKEY,expa,expw,dcls,ALG)   \
 IOF(f){I acn=ak/sizeof(T),  \
        wcn=wk/sizeof(T),* zv=AV(z);T* RESTRICT av=(T*)AV(a),* wv=(T*)AV(w);I md; \
        D tl=jt->cct,tr=1/tl;I il,jx;   \
        IH *hh=IHAV(h); I p=hh->datarange; TH * RESTRICT hv=hh->data.TH; UIL ctmask=calcctmask(jt->cct);   \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  INITXVAL dcls; \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */                            \
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md|=(IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBIP)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md));  /* remember if this is reflexive, which doesn't prehash */  \
  jx=0;                                                                     \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv; if(w==mark){wsct=0;} \
  for(;ac>0;av+=acn,--ac,wv+=--wreps<0?wcn:0,wreps=wreps<0?wrepssv:wreps){                   \
   if(!(mode&IPHOFFSET)){  /* if we are not using a prehashed table */                                        \
    hashallo(hh,p,asct,mode);                                                           \
    if(!(IIMODREFLEX&md)){I cn= k/sizeof(T);  /* not reflexive */                                            \
     PUSHCCT(1.0) if(md==IICO)XDQAP##ALG(T,TH,hash,expa,cn,ALG) else XDOAP##ALG(T,TH,hash,expa,cn,ALG); POPCCT  /* all writes to hash must use intolerant compare */                \
     if(w==mark)break;                                                                \
    } \
   }                                                                                            \
   switch(md){                                                                                   \
   /* when we are searching up, we can stop the second search when it gets past the index found in the first search */ \
   case IIDOT: {T * RESTRICT v=wv; I j, hj; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,zv[i]=il;,); zv+=wsct; } break;  \
   case IIDOT|IIMODREFLEX: {T * RESTRICT v=wv; I j, hj; TDOXY(T,TH,FYY,expa,expw,{},hj>=il,il=hj;,zv[i]=il;,); zv+=wsct; } break;  \
   case IFORKEY|IIMODREFLEX: {T * RESTRICT v=wv; I nuniq=0; I j, hj; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,nuniq+=i==il;zv[i]=il;zv[il]++;,i); AM(h)=nuniq; zv+=wsct; } break;  \
   case IICO:  {T * RESTRICT v=wv; I j, hj; TDQXY(T,TH,FXY,expa,expw,{},hj==asct,il=il==asct?hj:il;il=hj>il?hj:il;,zv[i]=il;); zv+=wsct;} break;  \
   case IICO|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; TDQXY(T,TH,FYY,expa,expw,{},hj==asct,il=il==asct?hj:il;il=hj>il?hj:il;,zv[i]=il;); zv+=wsct;} break;  \
   case INUBSV|IIMODREFLEX:{T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,zb[i]=i==il;,i); zv=(I*)(zb+=wsct);} break;  /* zv must keep running */  \
   case INUB|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVY(T,TH,FYYKEY,expw,expw,i); ZCSHAPE; }    break;  \
   case INUBIP|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; C * RESTRICT zc=(C*)zv; TMVYIP(T,TH,FYYKEY,expw,expw,wsct);  }    break;  \
   case ILESS: {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVX(T,TH,FXY,expa,expw); ZCSHAPE; }    break;  \
   case IINTER: {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVXI(T,TH,FXY,expa,expw); ZCSHAPE; }    break;  \
   case INUBI|IIMODREFLEX: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,*zi=i; zi+=(i==il);,i); ZISHAPE;} break;  \
   case IEPS:  {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,zb[i]=(il!=asct);,); zv=(I*)(zb+=wsct);} break;   /* zv must keep running */ \
   case II0EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=i; break;},); *zi=s;} break;  \
   case II1EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct!=il){s=i; break;},); *zi=s;} break;  \
   case IJ0EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=i; break;}); *zi=s;} break;  \
   case IJ1EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct!=il){s=i; break;}); *zi=s;} break;  \
   case ISUMEPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=0; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,s+=(asct>il);,); *zi=s;}  break;  \
   case IANYEPS: {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; I s=0; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct>il){s=1; break;},); *zb=(B)s;} break;  \
   case IALLEPS: {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; I s=1; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=0; break;},); *zb=(B)s;} break;  \
   case IIFBEPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zi=i; zi+=(asct>il);,); ZISHAPE;} break;  \
   } \
  }                                                                                             \
  R h;                                                                                           \
 }

// Verbs for the types of inputs
// jtio[dzea FL/CMPX/QP/BOX][1 if atom][2 if long table] all tolerant

// hashes using the 2-byte hashtable
IOFT(D,US,jtiod, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0(v,av+n*hj,n  ), !jeqd(n,v,av+n*hj,tl),INITTLTR INITXNEW INITXROT D x,algv)    // FL array
IOFT(Z,US,jtioz, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !eqz(n,v,av+n*hj),INITTLTR INITXNEW INITXROT D x,algv)     // CMPLX array
IOFT(Z,US,jtioz1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !zeq( *v,av[hj] ),INITTLTR INITXNEW INITXROT D x,algv)     // CMPLX atom
IOFT(E,US,jtioe, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !jteqe(jt,n,v,&av[n*hj]),INITTLTR INITXNEW INITXROT D x,algv)     // QP array
IOFT(E,US,jtioe1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !jteqe(jt,1,v,&av[hj] ),INITTLTR INITXNEW INITXROT D x,algv)     // QP atom
IOFT(A,US,jtioa, cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!eqa(n,v,av+n*hj),          !eqa(n,v,av+n*hj),,algv)     // boxed array with more than 1 box
IOFT(A,US,jtioa1,cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!equ(C(*v),C(av[hj])),!equ(C(*v),C(av[hj])),,algv)     // singleton box
// hashes using the 4-byte hashtable
IOFT(D,UI4,jtiod2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0(v,av+n*hj,n  ), !jeqd(n,v,av+n*hj,tl),INITTLTR INITXNEW INITXROT D x,algv)     // FL array
IOFT(Z,UI4,jtioz2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !eqz(n,v,av+n*hj),INITTLTR INITXNEW INITXROT D x,algv)     // CMPLX array
IOFT(Z,UI4,jtioz12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !zeq( *v,av[hj] ),INITTLTR INITXNEW INITXROT D x,algv)     // CMPLX atom
IOFT(E,UI4,jtioe2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !jteqe(jt,n,v,&av[n*hj]),INITTLTR INITXNEW INITXROT D x,algv)     // QP array
IOFT(E,UI4,jtioe12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !jteqe(jt,1,v,&av[hj] ),INITTLTR INITXNEW INITXROT D x,algv)     // QP atom
IOFT(A,UI4,jtioa2, cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!eqa(n,v,av+n*hj),          !eqa(n,v,av+n*hj), D x,algv)     // boxed array with more than 1 box
IOFT(A,UI4,jtioa12,cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!equ(C(*v),C(av[hj])),!equ(C(*v),C(av[hj])),  D x,algv)     // singleton box

#if 1  // obsolete  
// algt compares.  x is a register with the sought value in lane 1, y is the value from the hash
#define CNEtD(x,y) ({ \
 __m256d hashv=_mm256_set1_pd(y); hashv=_mm256_blend_pd(x,hashv,0b0010); \
 (3*_mm256_movemask_pd(_mm256_fmsub_pd(cct,hashv,_mm256_permute_pd(hashv,0b0001))))&2; /* tolerant comparison by our defn */ \
})  // mismatch if sign bits not =
#define CNE0tD(x,y) ({ /* this version for exact comparison */ \
 _mm256_movemask_pd(_mm256_cmp_pd(x,_mm256_set1_pd(y),_CMP_NEQ_UQ))&1; /* intolerant comparison */ \
})

// fetch into the bucketed queue, creating the queue of values, primary bucket, secondary bucket.  May overfetch by 3 words on either end
#define BUCKETalgt(src,index,store) bq=_mm256_loadu_pd(&src##v[index-((store)&256?NPAR-1:0)]); /* load the values */ \
 bq=_mm256_and_pd(bq,_mm256_cmp_pd(bq,_mm256_setzero_pd(),_CMP_NEQ_UQ)); /* convert -0 to +0 */ \
 bqp=_mm256_mul_pd(bq,cct); bqx=_mm256_mul_pd(bq,ccti);  /* get lower & upper limits of tolerant equality */ \
 bqx=_mm256_xor_pd(bqx,bqp); bqp=_mm256_and_pd(bq,bmsk); bqx=_mm256_xor_pd(bqx,bq); bqx=_mm256_and_pd(bqx,bmsk); // create buckets
   // At most one of upper/lower neighbors are in a different bucket.  Find that neighbor, setting to the primary if there is no overlap
// fetch one value from the bucketed queue.  The actual value goes into lane 0 of dest; the primary bucket value goes into
// dest##p; the XOR offset for the secondary bucket goes into dest##x
#define FETCHalgt(dest,src,index,store) if((store)&256){bqp=_mm256_permute4x64_pd(bqp,0b00011011); dest##p=_mm256_extract_epi64(_mm256_castpd_si256(bqp),0x0); bq=_mm256_permute4x64_pd(bq,0b00011011); dest=bq; \
 if(!((store)&1)){bqx=_mm256_permute4x64_pd(bqx,0b00011011); dest##x=_mm256_extract_epi64(_mm256_castpd_si256(bqx),0x0);}}   /* reverse, shift queue before extracting */ \
 else{dest##p=_mm256_extract_epi64(_mm256_castpd_si256(bqp),0x0); bqp=_mm256_permute4x64_pd(bqp,0b11111001); dest=bq; bq=_mm256_permute4x64_pd(bq,0b11111001); \
 if(!((store)&1)){dest##x=_mm256_extract_epi64(_mm256_castpd_si256(bqx),0x0); bqx=_mm256_permute4x64_pd(bqx,0b11111001);}}   /* forward, shift queue after extracting */
// hash one value (both primary and XOR), create the indexes into the hashtable, and then prefetch the primary value
#define HASHalgt(destsrc,HASH,store) {destsrc##p=(HASH(destsrc##p)*(UIL)(p))>>32; PREFETCH((C*)&hv[destsrc##p]); if(!((store)&1))destsrc##x=(HASH(destsrc##x)*(UIL)(p))>>32;}
// Pass the hash table for one value, like above. stop if index exceeds xlim
// cases are given by store, with bits meaning:
// 0 intolerant store only nonreflexive (table build)
// 2 normal tolerant search nonreflexive, forward or reverse - result will always be the last hash value inspected
// 3 e. &c. tolerant search nonreflexive index not needed
// 4 tolerant search + intolerant search/store reflexive (i./i: reflexive) store-search continues after tolerant match
// 6 tolerant search/store reflexive (/. or ~.) store suppressed on tolerant match
// 7 /. or ~. inplace - tolerant search/store reflexive, and use current wsct (running size of nub) as hash index of new store, store suppressed on tolerant match
// 8 set for tolerant i: (either reflexive or not), indicating that the LARGER result of the two searches should be used
// 9 set when we are processing the secondary bucket and can exit if the position passes the result of the first bucket
// at end, harg is the hashtable slot we ended on (used for ~. and /.: we fill the slot in if there was not a tolerant match)
// fndx is the location of the tolerant match, if there is one.  After the primary, fndx=max if no match; after secondary, 0 or max depending on direction
#define FIND1algt(warg,harg,T,TH,hsrc,mismatch,mismatchx,store,lbl) \
NOUNROLL for(;;({--harg; if(unlikely(harg<0))harg+=p;harg;})){  /* loop until we hit a match or an empty slot (hj==hsrc##sct) */ \
 I hj=hv[harg];  /* fetch the hash index, which points back into the source table */ \
 if((store)==4)fndx=hj; /* simple case in primary, save a branch */ \
 if(hj==hsrc##sct || ((store)&512 && ((store)&256?hj<fndx:hj>fndx))){  /* stopper slot encountered - empty, or second pass that has overshot the index found in the first pass */ \
  if(!((store)&512) && (store)&0b11000000)fndx=hsrc##sct;  /* notfound in ~. first pass: indicate no tolerant match */ \
  if(((store)&~256)==1||((store)&~256)==16){hv[harg]=(TH)i; if((store)&1)goto lbl##notfound;}  /* intolerant store first pass: we must be at an empty slot, store into it.  If reflexive, indicate match in new stored element */ \
  break;  /* exit not found in this bucket */ \
 }else{  /* nonempty slot found.  See if it matches */ \
  if(!((store)&1)){  /* first look is for tolerant match, skip if intolerant store */ \
   if(likely(!mismatch(warg,hsrc##v[hj]))){  /* if there is a tolerant match */ \
    if((store)&8)goto lbl##found;  /* if match on e., just say found, no need to check secondary */ \
    else if((store)&16 && !((store)&512))fndx=fndx==i?hj:fndx;  /* if first pass reflexive, we must keep looking for exact match, remember the first tolerant match */ \
    else{if((store)!=4)fndx=hj; break;} /* looking just for 1 tolerant match per bucket: remember it and stop looking */ \
   }else continue; /* if tolerant mismatch, can't be exact match, go to next slot */ \
  } \
 } /* falling through, we have a nonempty slot that we have to check for an exact match - type 1 or 16 */ \
 if(likely(!mismatchx(warg,hsrc##v[hj]))){if((store)&1)goto lbl##found; break;}  /* exact match terminates the search without store */ \
}

#define FINDalgt(warg,harg,T,TH,hsrc,mismatch,mismatchx,fstmt,nfstmt,store,lbl) { \
 if((store)&16)fndx=i;   /* set fndx to 'found at i' if intolerant reflexive, otherwise will be set by search if needed */ \
 FIND1algt(warg,harg##p,T,TH,hsrc,mismatch,mismatchx,store,lbl)  /* set harg with place to store in primary, fndx to result of tolerant match */ \
 /* intolerant store (table build nonreflexive) has already branch to found/notfound */ \
 if(unlikely(harg##p!=harg##x)){  /* if there is a secondary bucket */ \
  /* if the primary search matched (including the reflexive which always matches), leave fndx to cause early termination of the secondary; otherwise set fndx to accept any match */ \
  if(!((store)&0b11011000)){fndx=fndx==hsrc##sct?((store)&256?-1:hsrc##sct):fndx;} \
  FIND1algt(warg,harg##x,T,TH,hsrc,mismatch,mismatchx,((store)+512),lbl)  /* set fndx/harg from match on socondary */ \
  if((store)&8)goto lbl##notfound;  /* if e. missed both buckets, finis */ \
  /* combine primary & secondary giving overall found result */ \
  fndx=fndx==((store)&256?-1:hsrc##sct)?hsrc##sct:fndx;  /* if no match found, convert to canonical mismatch value */ \
 }else if((store)&8)goto lbl##notfound;  /* if e. missed, finis */ \
 if((store)&0b11000000){if(fndx!=hsrc##sct){if((store)&64)hv[harg##p]=(TH)i;else hv[harg##p]=wsct++;}} /* ~. and /. store to the hashtable only if no tolerant match: store i if not inplace, wsct if inplace */ \
 if(fndx==hsrc##sct){lbl##notfound:; nfstmt}else{lbl##found:; fstmt}  /* run user's result instructions */ \
}

// Traverse the hash table for an argument with a type, i. e. one that can be indexed by i
#define XSEARCHalgt(T,TH,src,hsrc,hash,mismatch,mismatchx,stride,fstmt,nfstmt,store,len,lbl,ALG) \
{ \
 __m256d w2, w1, w0; I w2p, w2x, wh1p, wh1x, h0p, h0x; \
 I i=(store)&256?src##sct-1:0;  /* needed for FIND */ \
 BUCKETalgt(src,i,store) \
 FETCHalgt(w2,src##v,i,store) /* fetch into item 1 */ \
 if(likely(src##sct>1)){ \
     w1=w2; wh1p=w2p; if(!((store)&1))wh1x=w2x; FETCHalgt(w2,src##v,i+((store)&256?-1:1),store) HASHalgt(wh1,hash,store) /* fetch into item 2, hash item 1 */ \
  for(;i!=(store)&256?1:src##sct-2;i=i+((store)&256?-1:1)){ \
   /* find slot 0, hash slot 1, and fetch slot 2 */ \
   if((store)&256?((src##sct-i)&3)==3:(i&3)==2){BUCKETalgt(src,i+((store)&256?-2:2),store)}  /* if we need 4 new input words, get them */ \
   w0=w1; w1=w2; h0p=wh1p; wh1p=w2p; if(!((store)&1)){h0x=wh1x; wh1x=w2x;} FETCHalgt(w2,src##v,i+((store)&256?-2:2),store); HASHalgt(wh1,hash,store) FINDalgt(w0,h0,T,TH,hsrc,mismatch,mismatchx,fstmt,nfstmt,store,lbl##0) \
  } \
  FINDalgt(w1,wh1,T,TH,hsrc,mismatch,mismatchx,fstmt,nfstmt,store,lbl##1) i=i+((store)&256?-1:1); /* find slot 1 */ \
 } \
 HASHalgt(w2,hash,store) FINDalgt(w2,w2,T,TH,hsrc,mismatch,mismatchx,fstmt,nfstmt,store,lbl##2) /* hash & find slot 2 */ \
}

// version for atomic tolerant types, where ALG is algt
#define IOFT1(T,TH,f,hash,mismatch,mismatchx,ALG)   \
IOF(f){ \
 I acn=ak/sizeof(T),  \
 wcn=wk/sizeof(T),* zv=AV(z);T* RESTRICT av=(T*)AV(a),* wv=(T*)AV(w);I md; \
 __m256d bq, bqp, bqx, cct=_mm256_set1_pd(jt->cct), ccti=_mm256_set1_pd(1.000000000000001*(2.0-jt->cct));  /*  bucket info, stored & calculated NPAR at a time. bq is the actual data, bqp is the pruimary bucket, bqx is the secondary bucket cct and 1/cct, used to see whether the value overlaps buckets */ \
 /* bmsk holds 16 tolerance intervals at the large end.  The bigger the bucket  the less often we have to check 2 buckets, but the more hits we might have in a bucket. */ \
 /* with 16 intervals we still have 38 bits of mantissa in the bucket, which is plenty.  For non-atomic searches we use a minimal bucket size because comparse are slow */ \
 __m256d bmsk; UIL bmsku; if(likely(jt->cct==1.0-FUZZ))bmsku=(UIL)0xffffffffffffc000LL; else if(likely(jt->cct==1.0))bmsku=(UIL)~0LL; \
                          else{D ct=1.0-jt->cct; I8 exp=(((*(I8*)&ct-1)>>52)&0x7ff)-0x3fe+44+10+4; exp=exp<0?0:exp; bmsku=-((UIL)1<<exp);} /* convert ct of 10..0 to 01..1, then look at exponent and make the size match on the default 2^_44 */ \
 bmsk=_mm256_castsi256_pd(_mm256_set1_epi64x(bmsku));  /* bmsk in all lanes */ \
 IH *hh=IHAV(h); I p=hh->datarange; TH * RESTRICT hv=hh->data.TH;    \
 I fndx; /* index match found at; initally comes from hashtable */ \
 md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */ \
 if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md|=(IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBIP)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md));  /* remember if this is reflexive, which doesn't prehash */  \
 I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv; if(w==mark){wsct=0;} \
 for(;ac>0;av+=acn,--ac,wv+=--wreps<0?wcn:0,wreps=wreps<0?wrepssv:wreps){ \
  if(!(mode&IPHOFFSET)){  /* if we are not using a prehashed table */ \
   hashallo(hh,p,asct,mode);      /* allocate the table */ \
   if(!(IIMODREFLEX&md)){I cn= k/sizeof(T);  /* not reflexive */ \
    if(md==IICO)XSEARCHalgt(T,TH,a,a,hash,mismatch,mismatchx,,,,1,asct,lblsf,ALG) else XSEARCHalgt(T,TH,a,a,hash,mismatch,mismatchx,,,,(256+1),asct,lblsr,ALG)  /* all writes to hash must use intolerant compare */ \
    if(w==mark)break; /* if the prehash was all we are interested in, skip the search */ \
   } \
  }  \
  switch(md){ \
  case IIDOT: case IICO: {XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,zv[i]=fndx;,zv[i]=fndx;,4,wsct,lbls0,ALG) zv+=wsct; } break;  \
  case IIDOT|IIMODREFLEX: {XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,zv[i]=fndx;,zv[i]=fndx;,16,wsct,lbls1,ALG) zv+=wsct; } break;  \
  case IICO|IIMODREFLEX: {XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,zv[i]=fndx;,zv[i]=fndx;,256+16,wsct,lbls2,ALG) zv+=wsct; } break;  \
  case IFORKEY|IIMODREFLEX: {wsct=0; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,zv[i]=fndx;zv[fndx]++;,wsct++;zv[fndx]++;,64,asct,lbls3,ALG) AM(h)=wsct; zv+=wsct; } break; /* scaf zv+ not needed */ \
  case INUBSV|IIMODREFLEX:{XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,((B*)zv)[i]=0;,((B*)zv)[i]=1;,8,wsct,lbls4,ALG) zv=(I*)((I)zv+wsct);} break;  /* zv must keep running */  \
  case INUB|IIMODREFLEX: {wsct=0; XSEARCH##ALG(T,TH,a,a,hash,mismatch,mismatchx,STRIDE,,((T*)zv)[wsct++]=wv[i];,64,asct,lbls5,ALG) AS(z)[0]=AN(z)=wsct; }    break;  \
  case INUBIP|IIMODREFLEX: {wsct=0; XSEARCH##ALG(T,TH,a,a,hash,mismatch,mismatchx,STRIDE,,((T*)zv)[wsct++]=wv[i];,128,asct,lbls6,ALG) AS(z)[0]=AN(z)=wsct; }    break;  \
  case INUBI|IIMODREFLEX: {wsct=0; XSEARCH##ALG(T,TH,a,a,hash,mismatch,mismatchx,STRIDE,,((I*)zv)[wsct++]=i;,64,asct,lbls7,ALG) AS(z)[0]=AN(z)=wsct; } break;  \
  case ILESS: {I zct=0; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,,((T*)zv)[zct++]=wv[i];,8,wsct,lbls8,ALG) AS(z)[0]=AN(z)=zct; }    break;  \
  case IINTER: {I zct=0; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,((T*)zv)[zct++]=wv[i];,,8,wsct,lbls9,ALG) AS(z)[0]=AN(z)=zct; }    break;  \
  case IEPS:  {XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,((B*)zv)[i]=1;,((B*)zv)[i]=0;,8,wsct,lbls10,ALG) zv=(I*)((I)zv+wsct);} break;   /* zv must keep running */ \
  case II0EPS: case II1EPS: {I s; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,if(md==II1EPS){s=i; goto lbls11fin;},if(md==II0EPS){s=i; goto lbls11fin;},8,wsct,lbls11,ALG) s=wsct; lbls11fin: *(I*)zv=s;} break;  \
  case IJ0EPS: case IJ1EPS: {I s; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,if(md==IJ1EPS){s=i; goto lbls12fin;},if(md==IJ0EPS){s=i; goto lbls12fin;},256+8,wsct,lbls12,ALG) s=wsct; lbls12fin: *(I*)zv=s;} break;  \
  case ISUMEPS: {I s=0; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,++s;,,8,wsct,lbls13,ALG) *(I*)zv=s; }  break;  \
  case IANYEPS: {B s; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,s=1;goto lbls14fin;,,8,wsct,lbls14,ALG) s=0; lbls14fin: *(B*)zv=s;} break;  \
  case IALLEPS: {B s; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,,s=0;goto lbls15fin;,8,wsct,lbls15,ALG) s=1; lbls15fin: *(B*)zv=s;} break;  \
  case IIFBEPS: {I zct=0; XSEARCH##ALG(T,TH,w,a,hash,mismatch,mismatchx,STRIDE,((I*)zv)[zct++]=i;,,8,wsct,lbls16,ALG) AS(z)[0]=AN(z)=zct; }    break;  \
  } \
 } \
 R h;  \
}
#endif
IOFT1(D,US,jtiod1,HASHiCRC, CNEtD,CNE0tD,algt)     // FL atom
// test IOFT(D,US,jtiod1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,*v!=av[hj],                       !TCMPEQ(tl,*(D*)v,av[hj] ),INITTLTR INITXNEW INITXROT D x,algv)     // FL atom
IOFT(D,UI4,jtiod12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,*v!=av[hj],                       !TCMPEQ(tl,*(D*)v,av[hj] ),INITTLTR INITXNEW INITXROT D x,algv)     // FL atom


#endif
