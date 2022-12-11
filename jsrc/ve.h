/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Atomic Verbs                                                            */

// shorthands for inplacing
#define VIP (VIPOKA+VIPOKW)   // inplace everything
#define VIPI0 ((VIPOKA*(sizeof(I)==sizeof(D))))  // inplace left arg if D is same length as I
#define VIP0I ((VIPOKW*(sizeof(I)==sizeof(D))))  // inplace right arg if D is same length as I
#define VIPID (VIPI0+VIPOKW)  // inplace D, and I if I is same length as D: I is left arg
#define VIPDI (VIP0I+VIPOKA)  // inplace D, and I if I is same length as D: I is right arg

#define VIPW (VIPOKW)   // inplace is OK
#define VIP64 ((VIPOKW*(sizeof(I)==sizeof(D))))  // inplace monad if D is same length as I



#define ADECL1  AHDR1
#define ADECL2  AHDR2
#define ADECLP  AHDRP
#define ADECLR  AHDRR
#define ADECLS  AHDRS

extern ADECL1(   absQ,Q,Q);
extern ADECL1(   absX,X,X);
extern ADECL1(  ceilQ,X,Q);
extern ADECL1( ceilQQ,Q,Q);
extern ADECL1(   expX,X,X);
extern ADECL1(  factD,D,D);
extern ADECL1(  factI,I,I);
extern ADECL1(  factQ,Q,Q);
extern ADECL1(  factX,X,X);
extern ADECL1(  factZ,Z,Z);
extern ADECL1( floorQ,X,Q);
extern ADECL1(floorQQ,Q,Q);
extern ADECL1(   logX,X,X);
extern ADECL1(  logQD,D,Q);
extern ADECL1(  logQZ,Z,Q);
extern ADECL1(  logXD,D,X);
extern ADECL1(  logXZ,Z,X);
extern ADECL1(   pixX,X,X);
extern ADECL1(   sgnQ,X,Q);
extern ADECL1(   sgnX,X,X);
extern ADECL1(  sqrtQ,Q,Q);
extern ADECL1(  sqrtX,X,X);

extern ADECLP(   andpfxB,B, B );  extern ADECLR(   andinsB,B, B );  extern ADECLS(   andsfxB,B, B );
extern ADECLP(   divpfxD,D, D );  extern ADECLR(   divinsD,D, D );  extern ADECLS(   divsfxD,D, D );
extern ADECLP(   divpfxZ,Z, Z );  extern ADECLR(   divinsZ,Z, Z );  extern ADECLS(   divsfxZ,Z, Z );
extern ADECLP(    eqpfxB,B, B );  extern ADECLR(    eqinsB,B, B );  extern ADECLS(    eqsfxB,B, B );
extern ADECLP(    gepfxB,B, B );  extern ADECLR(    geinsB,B, B );  extern ADECLS(    gesfxB,B, B );
extern ADECLP(    gtpfxB,B, B );  extern ADECLR(    gtinsB,B, B );  extern ADECLS(    gtsfxB,B, B );
extern ADECLP(    lepfxB,B, B );  extern ADECLR(    leinsB,B, B );  extern ADECLS(    lesfxB,B, B );
extern ADECLP(    ltpfxB,B, B );  extern ADECLR(    ltinsB,B, B );  extern ADECLS(    ltsfxB,B, B );
extern ADECLP(   maxpfxD,D, D );  extern ADECLR(   maxinsD,D, D );  extern ADECLS(   maxsfxD,D, D );
extern ADECLP(   maxpfxI,I, I );  extern ADECLR(   maxinsI,I, I );  extern ADECLS(   maxsfxI,I, I );
extern ADECLP(   maxpfxQ,Q, Q );                                    extern ADECLS(   maxsfxQ,Q, Q );
extern ADECLP(   maxpfxS,SB,SB);  extern ADECLR(   maxinsS,SB,SB);  extern ADECLS(   maxsfxS,SB,SB);
extern ADECLP(   maxpfxX,X, X );  extern ADECLR(   maxinsX,X, X );  extern ADECLS(   maxsfxX,X, X );
extern ADECLP(   minpfxD,D, D );  extern ADECLR(   mininsD,D, D );  extern ADECLS(   minsfxD,D, D );
extern ADECLP(   minpfxI,I, I );  extern ADECLR(   mininsI,I, I );  extern ADECLS(   minsfxI,I, I );
extern ADECLP(   minpfxQ,Q, Q );                                    extern ADECLS(   minsfxQ,Q, Q );
extern ADECLP(   minpfxS,SB,SB);  extern ADECLR(   mininsS,SB,SB);  extern ADECLS(   minsfxS,SB,SB);
extern ADECLP(   minpfxX,X, X );  extern ADECLR(   mininsX,X, X );  extern ADECLS(   minsfxX,X, X );
extern ADECLP( minuspfxB,I, B );  extern ADECLR( minusinsB,I, B );  extern ADECLS( minussfxB,I, B );
extern ADECLP( minuspfxD,D, D );  extern ADECLR( minusinsD,D, D );  extern ADECLS( minussfxD,D, D );
extern ADECLP( minuspfxI,I, I );  extern ADECLR( minusinsI,I, I );  extern ADECLS( minussfxI,I, I );
extern ADECLP( minuspfxQ,Q, Q );  
extern ADECLP( minuspfxO,D, I );  extern ADECLR( minusinsO,D, I );  extern ADECLS( minussfxO,D, I );
extern ADECLP( minuspfxX,X, X );  
extern ADECLP( minuspfxZ,Z, Z );  extern ADECLR( minusinsZ,Z, Z );  extern ADECLS( minussfxZ,Z, Z );
extern ADECLP(  nandpfxB,B, B );  extern ADECLR(  nandinsB,B, B );  extern ADECLS(  nandsfxB,B, B );
extern ADECLP(    nepfxB,B, B );  extern ADECLR(    neinsB,B, B );  extern ADECLS(    nesfxB,B, B );
extern ADECLP(    orpfxB,B, B );  extern ADECLR(   norinsB,B, B );  extern ADECLS(   norsfxB,B, B );
extern ADECLP(   norpfxB,B, B );  extern ADECLR(    orinsB,B, B );  extern ADECLS(    orsfxB,B, B );
extern ADECLP(  pluspfxB,I, B );  extern ADECLR(  plusinsB,I, B );  extern ADECLS(  plussfxB,I, B );
extern ADECLP(  pluspfxD,D, D );  extern ADECLR(  plusinsD,D, D );  extern ADECLS(  plussfxD,D, D );
extern ADECLP(  pluspfxI,I, I );  extern ADECLR(  plusinsI,I, I );  extern ADECLS(  plussfxI,I, I );
extern ADECLP(  pluspfxO,D, I );  extern ADECLR(  plusinsO,D, I );  extern ADECLS(  plussfxO,D, I );
extern ADECLP(  pluspfxQ,Q, Q );                                    extern ADECLS(  plussfxQ,Q, Q );
extern ADECLP(  pluspfxX,X, X );                                    extern ADECLS(  plussfxX,X, X );
extern ADECLP(  pluspfxZ,Z, Z );  extern ADECLR(  plusinsZ,Z, Z );  extern ADECLS(  plussfxZ,Z, Z );
extern ADECLP( tymespfxD,D, D );  extern ADECLR( tymesinsD,D, D );  extern ADECLS( tymessfxD,D, D );
extern ADECLP( tymespfxI,I, I );  extern ADECLR( tymesinsI,I, I );  extern ADECLS( tymessfxI,I, I );
extern ADECLP( tymespfxO,D, I );  extern ADECLR( tymesinsO,D, I );  extern ADECLS( tymessfxO,D, I );
extern ADECLP( tymespfxQ,Q, Q );                                    extern ADECLS( tymessfxQ,Q, Q );
extern ADECLP( tymespfxX,X, X );                                    extern ADECLS( tymessfxX,X, X );
extern ADECLP( tymespfxZ,Z, Z );  extern ADECLR( tymesinsZ,Z, Z );  extern ADECLS( tymessfxZ,Z, Z );

extern ADECLP(bw0000pfxI,UI,UI);  extern ADECLR(bw0000insI,UI,UI);  extern ADECLS(bw0000sfxI,UI,UI);
extern ADECLP(bw0001pfxI,UI,UI);  extern ADECLR(bw0001insI,UI,UI);  extern ADECLS(bw0001sfxI,UI,UI);
                                  extern ADECLR(bw0010insI,UI,UI);  extern ADECLS(bw0010sfxI,UI,UI);
extern ADECLP(bw0011pfxI,UI,UI);  extern ADECLR(bw0011insI,UI,UI);  extern ADECLS(bw0011sfxI,UI,UI);
                                  extern ADECLR(bw0100insI,UI,UI);  extern ADECLS(bw0100sfxI,UI,UI);
extern ADECLP(bw0101pfxI,UI,UI);  extern ADECLR(bw0101insI,UI,UI);  extern ADECLS(bw0101sfxI,UI,UI);
extern ADECLP(bw0110pfxI,UI,UI);  extern ADECLR(bw0110insI,UI,UI);  extern ADECLS(bw0110sfxI,UI,UI);
extern ADECLP(bw0111pfxI,UI,UI);  extern ADECLR(bw0111insI,UI,UI);  extern ADECLS(bw0111sfxI,UI,UI);
                                  extern ADECLR(bw1000insI,UI,UI);  extern ADECLS(bw1000sfxI,UI,UI);
extern ADECLP(bw1001pfxI,UI,UI);  extern ADECLR(bw1001insI,UI,UI);  extern ADECLS(bw1001sfxI,UI,UI);
                                  extern ADECLR(bw1010insI,UI,UI);  extern ADECLS(bw1010sfxI,UI,UI);
                                  extern ADECLR(bw1011insI,UI,UI);  extern ADECLS(bw1011sfxI,UI,UI);
                                  extern ADECLR(bw1100insI,UI,UI);  extern ADECLS(bw1100sfxI,UI,UI);
                                  extern ADECLR(bw1101insI,UI,UI);  extern ADECLS(bw1101sfxI,UI,UI);
                                  extern ADECLR(bw1110insI,UI,UI);  extern ADECLS(bw1110sfxI,UI,UI);
extern ADECLP(bw1111pfxI,UI,UI);  extern ADECLR(bw1111insI,UI,UI);  extern ADECLS(bw1111sfxI,UI,UI);

#if (C_AVX&&SY_64) || EMU_AVX
#define BID void
#define PVB void
#define PVI void
#define PVD void
#else
#define BID UI
#define PVB B
#define PVI I
#define PVD D
#endif
extern ADECL2(bw0000II,BID,BID,BID);
extern ADECL2(bw0001II,BID,BID,BID);
extern ADECL2(bw0010II,BID,BID,BID);
extern ADECL2(bw0011II,BID,BID,BID);
extern ADECL2(bw0100II,BID,BID,BID);
extern ADECL2(bw0101II,BID,BID,BID);
extern ADECL2(bw0110II,BID,BID,BID);
extern ADECL2(bw0111II,BID,BID,BID);
extern ADECL2(bw1000II,BID,BID,BID);
extern ADECL2(bw1001II,BID,BID,BID);
extern ADECL2(bw1010II,BID,BID,BID);
extern ADECL2(bw1011II,BID,BID,BID);
extern ADECL2(bw1100II,BID,BID,BID);
extern ADECL2(bw1101II,BID,BID,BID);
extern ADECL2(bw1110II,BID,BID,BID);
extern ADECL2(bw1111II,BID,BID,BID);

extern ADECL2(  andBB,void,void,void);
extern ADECL2(  binDD,D,D,D);
extern ADECL2(  binQQ,X,Q,Q);
extern ADECL2(  binXX,X,X,X);
extern ADECL2(  binZZ,Z,Z,Z);
extern ADECL2(  cirBD,D,B,D);
extern ADECL2(  cirID,D,I,D);
extern ADECL2(  cirDD,D,D,D);
extern ADECL2(  cirZZ,Z,Z,Z);
extern ADECL2(  divBB,PVD,PVB,PVB);
extern ADECL2(  divBD,PVD,PVB,PVD);
extern ADECL2(  divBI,PVD,PVB,PVI);
extern ADECL2(  divDB,PVD,PVD,PVB);
extern ADECL2(  divDD,PVD,PVD,PVD);
extern ADECL2(  divDI,PVD,PVD,PVI);
extern ADECL2(  divIB,PVD,PVI,PVB);
extern ADECL2(  divID,PVD,PVI,PVD);
extern ADECL2(  divII,PVD,PVI,PVI);
extern ADECL2(  divQQ,Q,Q,Q);
extern ADECL2(  divXX,X,X,X);
extern ADECL2(  divZZ,Z,Z,Z);
extern ADECL2(   eqAA,B,A,A);
extern ADECL2(   eqBB,void,void,void);
extern ADECL2(   eqBD,B,B,D);
extern ADECL2(   eqBI,B,B,I);
extern ADECL2(   eqCC,void,void,void);  // really B,C,C
extern ADECL2(   eqCS,B,UC,US);
extern ADECL2(   eqDB,B,D,B);
extern ADECL2(   eqDD,B,D,D);
extern ADECL2(   eqDI,B,D,I);
extern ADECL2(   eqIB,B,I,B);
extern ADECL2(   eqID,B,I,D);
extern ADECL2(   eqII,B,I,I);
extern ADECL2(   eqQQ,B,Q,Q);
extern ADECL2(   eqSC,B,US,UC);
extern ADECL2(   eqSS,B,S,S);
extern ADECL2(   eqXX,B,X,X);
extern ADECL2(   eqZZ,B,Z,Z);
extern ADECL2(  gcdDD,D,D,D);
extern ADECL2(  gcdII,I,I,I);
extern ADECL2(  gcdIO,D,I,I);
extern ADECL2(  gcdQQ,Q,Q,Q);
extern ADECL2(  gcdXX,X,X,X);
extern ADECL2(  gcdZZ,Z,Z,Z);
extern ADECL2(   geBB,void,void,void);
extern ADECL2(   geBD,B,B,D);
extern ADECL2(   geBI,B,B,I);
extern ADECL2(   geDB,B,D,B);
extern ADECL2(   geDD,B,D,D);
extern ADECL2(   geDI,B,D,I);
extern ADECL2(   geIB,B,I,B);
extern ADECL2(   geID,B,I,D);
extern ADECL2(   geII,B,I,I);
extern ADECL2(   geQQ,B,Q,Q);
extern ADECL2(   geSS,B,SB,SB);
extern ADECL2(   geXX,B,X,X);
extern ADECL2(   gtBB,void,void,void);
extern ADECL2(   gtBD,B,B,D);
extern ADECL2(   gtBI,B,B,I);
extern ADECL2(   gtDB,B,D,B);
extern ADECL2(   gtDD,B,D,D);
extern ADECL2(   gtDI,B,D,I);
extern ADECL2(   gtIB,B,I,B);
extern ADECL2(   gtID,B,I,D);
extern ADECL2(   gtII,B,I,I);
extern ADECL2(   gtSS,B,SB,SB);
extern ADECL2(   gtQQ,B,Q,Q);
extern ADECL2(   gtXX,B,X,X);
extern ADECL2(  lcmDD,D,D,D);
extern ADECL2(  lcmII,I,I,I);
extern ADECL2(  lcmIO,D,I,I);
extern ADECL2(  lcmQQ,Q,Q,Q);
extern ADECL2(  lcmXX,X,X,X);
extern ADECL2(  lcmZZ,Z,Z,Z);
extern ADECL2(   leBB,void,void,void);
extern ADECL2(   leBD,B,B,D);
extern ADECL2(   leBI,B,B,I);
extern ADECL2(   leDB,B,D,B);
extern ADECL2(   leDD,B,D,D);
extern ADECL2(   leDI,B,D,I);
extern ADECL2(   leIB,B,I,B);
extern ADECL2(   leID,B,I,D);
extern ADECL2(   leII,B,I,I);
extern ADECL2(   leQQ,B,Q,Q);
extern ADECL2(   leSS,B,SB,SB);
extern ADECL2(   leXX,B,X,X);
extern ADECL2(   ltBB,void,void,void);
extern ADECL2(   ltBD,B,B,D);
extern ADECL2(   ltBI,B,B,I);
extern ADECL2(   ltDB,B,D,B);
extern ADECL2(   ltDD,B,D,D);
extern ADECL2(   ltDI,B,D,I);
extern ADECL2(   ltIB,B,I,B);
extern ADECL2(   ltID,B,I,D);
extern ADECL2(   ltII,B,I,I);
extern ADECL2(   ltSS,B,SB,SB);
extern ADECL2(   ltQQ,B,Q,Q);
extern ADECL2(   ltXX,B,X,X);
extern ADECL2(   eqabsDD,B,D,D);
extern ADECL2(   neabsDD,B,D,D);
extern ADECL2(   ltabsDD,B,D,D);
extern ADECL2(   leabsDD,B,D,D);
extern ADECL2(   geabsDD,B,D,D);
extern ADECL2(   gtabsDD,B,D,D);
extern ADECL2(  maxBD,PVD,PVB,PVD);
extern ADECL2(  maxBI,PVI,PVB,PVI);
extern ADECL2(  maxDB,PVD,PVD,PVB);
extern ADECL2(  maxDD,PVD,PVD,PVD);
extern ADECL2(  maxDI,PVD,PVD,PVI);
extern ADECL2(  maxIB,PVI,PVI,PVB);
extern ADECL2(  maxID,PVD,PVI,PVD);
extern ADECL2(  maxII,PVI,PVI,PVI);
extern ADECL2(  maxQQ,Q,Q,Q);
extern ADECL2(  maxSS,SB,SB,SB);
extern ADECL2(  maxXX,X,X,X);
extern ADECL2(  minBD,PVD,PVB,PVD);
extern ADECL2(  minBI,PVI,PVB,PVI);
extern ADECL2(  minDB,PVD,PVD,PVB);
extern ADECL2(  minDD,PVD,PVD,PVD);
extern ADECL2(  minDI,PVD,PVD,PVI);
extern ADECL2(  minIB,PVI,PVI,PVB);
extern ADECL2(  minID,PVD,PVI,PVD);
extern ADECL2(  minII,PVI,PVI,PVI);
extern ADECL2(  minQQ,Q,Q,Q);
extern ADECL2(  minSS,SB,SB,SB);
extern ADECL2(  minXX,X,X,X);
extern ADECL2(minusBB,PVI,PVB,PVB);
extern ADECL2(minusBD,PVD,PVB,PVD);
extern ADECL2(minusBI,PVI,PVB,PVI);
extern ADECL2(minusBIO,D,B,I);
extern ADECL2(minusDB,PVD,PVD,PVB);
extern ADECL2(minusDD,PVD,PVD,PVD);
extern ADECL2(minusDI,PVD,PVD,PVI);
extern ADECL2(minusIB,PVI,PVI,PVB);
extern ADECL2(minusID,PVD,PVI,PVD);
extern ADECL2(minusII,PVI,PVI,PVI);
extern ADECL2(minusIIO,D,I,I);
extern ADECL2(minusIO,D,I,I);
extern ADECL2(minusQQ,Q,Q,Q);
extern ADECL2(minusXX,X,X,X);
extern ADECL2(minusZZ,Z,Z,Z);
extern ADECL2( nandBB,void,void,void);
extern ADECL2(   neAA,B,A,A);
extern ADECL2(   neBB,void,void,void);
extern ADECL2(   neBD,B,B,D);
extern ADECL2(   neBI,B,B,I);
extern ADECL2(   neCC,void,void,void);  // really B,C,C
extern ADECL2(   neCS,B,UC,US);
extern ADECL2(   neDB,B,D,B);
extern ADECL2(   neDD,B,D,D);
extern ADECL2(   neDI,B,D,I);
extern ADECL2(   neIB,B,I,B);
extern ADECL2(   neID,B,I,D);
extern ADECL2(   neII,B,I,I);
extern ADECL2(   neQQ,B,Q,Q);
extern ADECL2(   neSC,B,US,UC);
extern ADECL2(   neSS,B,S,S);
extern ADECL2(   neXX,B,X,X);
extern ADECL2(   neZZ,B,Z,Z);
extern ADECL2(  norBB,void,void,void);
extern ADECL2(   orBB,void,void,void);
extern ADECL2( plusBB,PVI,PVB,PVB);
extern ADECL2( plusBI,PVI,PVB,PVI);
extern ADECL2( plusBIO,D,B,I);
extern ADECL2( plusBD,PVD,PVB,PVD);
extern ADECL2( plusDB,PVD,PVD,PVB);
extern ADECL2( plusDD,PVD,PVD,PVD);
extern ADECL2( plusDI,PVD,PVD,PVI);
extern ADECL2( plusDX,DX,DX,DX);
extern ADECL2( plusIB,PVI,PVI,PVB);
extern ADECL2( plusID,PVD,PVI,PVD);
extern ADECL2( plusII,PVI,PVI,PVI);
extern ADECL2( plusIIO,D,I,I);
extern ADECL2( plusIO,D,I,I);
extern ADECL2( plusQQ,Q,Q,Q);
extern ADECL2( plusXX,X,X,X);
extern ADECL2( plusZZ,Z,Z,Z);
extern ADECL2(  powBD,D,B,D);
extern ADECL2(  powBI,D,B,I);
extern ADECL2(  powDB,D,D,B);
extern ADECL2(  powDD,D,D,D);
extern ADECL2(  powDI,D,D,I);
extern ADECL2(  powIB,I,I,B);
extern ADECL2(  powID,D,I,D);
extern ADECL2(  powII,D,I,I);
extern ADECL2(  powQQ,Q,Q,Q);
extern ADECL2(  powXX,X,X,X);
extern ADECL2(  powZZ,Z,Z,Z);
extern ADECL2(  remDD,D,D,D);
extern ADECL2(  remII,I,I,I);
extern ADECL2(  remID,I,I,D);
extern ADECL2(  remQQ,Q,Q,Q);
extern ADECL2(  remXX,X,X,X);
extern ADECL2(  remZZ,Z,Z,Z);
extern ADECL2(tymesBD,PVD,PVB,PVD);
extern ADECL2(tymesBI,PVI,PVB,PVI);
extern ADECL2(tymesDB,PVD,PVD,PVB);
extern ADECL2(tymesDD,PVD,PVD,PVD);
extern ADECL2(tymesDI,PVD,PVD,PVI);
extern ADECL2(tymesIB,PVI,PVI,PVB);
extern ADECL2(tymesID,PVD,PVI,PVD);
extern ADECL2(tymesII,I,I,I);
extern I tymesIIO(I,I,I*,I*,D*,I);
extern ADECL2(tymesIO,D,I,I);
extern ADECL2(tymesQQ,Q,Q,Q);
extern ADECL2(tymesXX,X,X,X);
extern ADECL2(tymesZZ,Z,Z,Z);

extern ADECL2(   eqUU,B,C4,C4);
extern ADECL2(   eqUS,B,C4,US);
extern ADECL2(   eqSU,B,US,C4);
extern ADECL2(   eqUC,B,C4,UC);
extern ADECL2(   eqCU,B,UC,C4);
extern ADECL2(   neUU,B,C4,C4);
extern ADECL2(   neUS,B,C4,US);
extern ADECL2(   neSU,B,US,C4);
extern ADECL2(   neUC,B,C4,UC);
extern ADECL2(   neCU,B,UC,C4);
