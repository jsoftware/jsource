/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"
#if C_CRC32C && SY_64
#include "viavx.h"

// ********************* i., third class: small-range arguments ****************************

// nub support, which creates & processes the bit-vector in one pass.  FULL is immaterial.
// T is the type for the result vector, stmt creates the result
// unpacked version expects default of 1
#define SNUB(decl, stmt) I zi=0, zie=asct;  decl  UC* RESTRICT hu=hh->data.UC-min; while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=0;  stmt  ++zi;}
// packed version.  Default of 0
#define SNUBP(decl, stmt) I zi=0, zie=asct;  decl  UC* RESTRICT hu=hh->data.UC-BYTENO(min); while(zi!=zie){UC v=hu[BYTENO(wv[zi])]; hu[BYTENO(wv[zi])]|=1<<BITNO(wv[zi]); v >>=BITNO(wv[zi]); v&=1; v^=1; stmt  ++zi;}

// creation of the value vector

// The bitmask was cleared to 0/1 by hashalloc depending on function
// Boolean/bit hashtables.  Used  where the position doesn't matter, i. e. for all but i./i: 
// Set TRUE for each value found.  hh->currentlo will always be 0.  zi starts at 0, since values don't matter
// The value in the cell is 0 or 1, since we don't care what the original position was
// We init the table to 0 or 1 depending on the primitive; but we always use 0 for PACKed bits, because it's never right to add
// an instruction to building the table
#define SDO(T,bitdef)  UC* RESTRICT hu=hh->data.UC-min; if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; DO(asct, hu[mav[i]]=1-bitdef;)}
#define SDOP(T)  UC* RESTRICT hu=hh->data.UC-BYTENO(min); if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; DO(asct, hu[BYTENO(mav[i])]|=1<<BITNO(mav[i]);)}  // packed version

// US/UI4 hashtables
// the value in the cell indicates the original input position; the index of the cell indicates the input value
// Go through and remember the index for each value.  Leaves last index, so used for i:.  Leave zi correct at end
#define SDOA(T,Ttype) I zi=hh->currentindexofst, zi0=zi, zie=zi+asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
                           if(!(mode&IPHOFFSET)){T* RESTRICT mav=av-zi; while(zi!=zie){hu[mav[zi]]=(Ttype)zi; ++zi;} zi=zi0;}
// No faster version for BASE0
// Go through and remember the index for each value.  Reverse order, so leaves first index, so used for i.
#define SDQA(T,Ttype) I zi=hh->currentindexofst, zi0=zi, zie=zi+asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo;  \
                           if(!(mode&IPHOFFSET)){T* RESTRICT mav=av-zi; do{--zie; hu[mav[zie]]=(Ttype)zie;}while(zie!=zi);}
// This version if we know the area starts at 0
#define SDQA0(T,Ttype) I zie=asct-1; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo;  \
                           if(!(mode&IPHOFFSET)){do{hu[av[zie]]=(Ttype)zie;}while(--zie>=0);}

// using the value vector: loop through the items of w, creating the output.
// if FULL is set, there is no need for range-checking the input

// first, versions for i. i:     vv is the value to use for not-found (always asct)
// This version for I values (which have a partial table); use the table only if the value is represented there.  vv is the not-found value.  Install it in default position
// Since the table is not FULL, the range scan of must have aborted; we scan forward from the beginning of w
#define SCOZ(T,Ttype,vv) {I * RESTRICT zv=AV(z)+l*wsct-zi; T* RESTRICT mwv=wv-zi; Ttype def[1]; zie=zi+wsct; def[0]=(Ttype)(vv+zi0); \
                          while(zi!=zie){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; I hvv; if((hvv=(I)*hv-zi0)<0)hvv=vv; zv[zi]=(Ttype)hvv; ++zi;}}
// this version is used when the result vector is known to be full (out-of-range not possible).  Omit the out-of-bounds check.  Since the range-scan of w ran to completion,
// the end of w is most likely to be in-cache; so scan backwards
#define SCOZF(T,Ttype,vv)  {I * RESTRICT zv=AV(z)+l*wsct-zi; T* RESTRICT mwv=wv-zi; zie=zi+wsct; while(zi!=zie){--zie; I hvv; if((hvv=(I)hu[mwv[zie]]-zi0)<0)hvv=vv; zv[zie]=(Ttype)hvv;}}
// these versions are used if the table is known to start at offset 0.  Omit the not-found check, since asct was already loaded; and zi is 0
#define SCOZ0(T,Ttype,vv) {I zi = -wsct; I * RESTRICT zv=AV(z)+l*wsct-zi; T* RESTRICT mwv=wv-zi; Ttype def[1]; def[0]=(Ttype)vv; \
                          while(zi){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; zv[zi]=*hv; ++zi;}}
#define SCOZF0(T,Ttype,vv) {zie=wsct-1; I * RESTRICT zv=AV(z)+l*wsct; while(zie>=0){zv[zie]=hu[wv[zie]]; --zie;}}  // backwards scan

// for e. -. u@e. ([ -. -.) - for each item of w, see if it is in the value table.  Set v to the value read from the table (1 if in table).
// The table is always allocated as a bit vector and therefore is at offset 0 in the table
// wv[i] is the data value read, if that's needed
// Declare the result vector, and prebias it for our loop if needed.  indexed is 1 if zv will be referred to as zv[i], 0 if by *zv
#define DCLZVO(T,indexed) T * RESTRICT zv=T##AV(z)+(l+indexed)*wsct;
#define SCOW(T,bitdef,stmt)  {UC def[1]; def[0]=bitdef; I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){T x=mwv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt}}
// faster version of SCOW for use when the table contains all possible values
#define SCOWF(T,bitdef,stmt)  {I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){UC v=hu[mwv[i]]; stmt}}
// packed version
#define SCOWP(T,bitdef,stmt)  {UC def[1]; def[0]=0; I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){T x=mwv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}
// packed full version
#define SCOWPF(T,bitdef,stmt)  {I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){T x=mwv[i]; UC *hv=hu+BYTENO(x); UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}

// just like SCOW/SCOWF but scanning from the end of w
#define DCLZVQ(T,unused) T * RESTRICT zv=T##AV(z)+l*wsct;
#define SCQW(T,bitdef,stmt)  {UC def[1]; def[0]=bitdef; I i; for(i=wsct-1;i>=0;--i){T x=wv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt}}
#define SCQWF(T,bitdef,stmt)  {I i; for(i=wsct-1;i>=0;--i){UC v=hu[wv[i]]; stmt}}
#define SCQWP(T,bitdef,stmt)  {UC def[1]; def[0]=0; I i; for(i=wsct-1;i>=0;--i){T x=wv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=(*hv>>BITNO(x))&1; stmt}}
#define SCQWPF(T,bitdef,stmt)  {I i; for(i=wsct-1;i>=0;--i){T x=wv[i]; UC *hv=hu+BYTENO(x); UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}

// Create basic/FULL/PACK/FULL+PACK versions (used for all boolean maps)
#define SMCASE0(casename, text) case casename: text break;
#define SMCASEF(casename, text) case IIMODFULL+casename: text break;
#define SMCASEP(casename, text) case IIMODPACK+casename: text break;
#define SMCASE1(casename, text) case IIMODFULL+IIMODPACK+casename: text break;
// action here is (parms to SCO*) cleanup  - the cleanup comes after the SCO* loop
#define SMFULLPACK(T,bitdef,casename,decl,action) SMCASE0(casename, {SDO(T,bitdef) decl SCOW action}) SMCASEF(casename, {SDO(T,bitdef) decl SCOWF action}) SMCASEP(casename, {SDOP(T) decl SCOWP action}) SMCASE1(casename, {SDOP(T) decl SCOWPF action})
#define SMFULLPACQ(T,bitdef,casename,decl,action) SMCASE0(casename, {SDO(T,bitdef) decl SCQW action}) SMCASEF(casename, {SDO(T,bitdef) decl SCQWF action}) SMCASEP(casename, {SDOP(T) decl SCQWP action}) SMCASE1(casename, {SDOP(T) decl SCQWPF action})
// EPS builds a full-size result and thus can scan FULLs in either order; we choose backwards
#define SMFULLPACKEPS(T,bitdef,casename,action) SMCASE0(casename, {SDO(T,bitdef) DCLZVO(B,1) SCOW action}) SMCASEF(casename, {SDO(T,bitdef) DCLZVQ(B,1) SCQWF action}) SMCASEP(casename, {SDOP(T) DCLZVO(B,1) SCOWP action}) SMCASE1(casename, {SDOP(T) DCLZVQ(B,1) SCQWPF action})

// Do the operation on small-range arguments
// COZ1 is the result loop for i. i:  we will have to have a bit version for e.
// COW is the result loop for (e. i. 1:)  ([: +./ e.) ([: +/ e.) ([: I. e.) (e. i. 0:) ([: *./ e.) -.
// CQW is the result loop for (e. i: 1:) (e. i: 0:)
// cm is the number of cells of w per cell of a 
#define IOFSMALLRANGE(f,T,Ttype)    \
 IOF(f){IH *hh=IHAV(h);I l;T* RESTRICT av,* RESTRICT wv;T max,min; UI p; \
  mode|=((mode&(IIOPMSK&~(IIDOT^IICO)))|((I)a^(I)w)|(ac^wc))?0:IIMODREFLEX; \
  av=(T*)AV(a); wv=(T*)AV(w); \
  min=(T)hh->datamin; p=hh->datarange; max=min+(T)p-1; \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv; if(w==mark){wsct=0;} \
  for(l=0;l<ac;++l,av+=asct,wv+=--wreps<0?wsct:0,wreps=wreps<0?wrepssv:wreps){ \
   if(!(mode&(IPHOFFSET))){mode = hashallo(hh,p,asct,mode);}  /* clear table & set parms for this loop - only if not using prehashed table, which always start at offset 0 */ \
   switch(mode&(IIOPMSK|IIMODFULL|IIMODPACK|IIMODBASE0)){ /* We know the setting of IIMODBITS without looking */ \
   default: ASSERTSYS(0,"switch failure in i."); \
     /* a lot of the l*wsct in the cases below could be removed because the verbs lack IRS, but we're leaving them in for now */ \
     /* reflexives, which include nubs, do not require a FULL version.  i.~ and i:~ don't need PACK versions either, since they deal in full indexes only */ \
   case IIMODREFLEX+IIMODFULL+IIDOT: \
   case IIMODREFLEX+IIDOT: \
     {I zi=hh->currentindexofst, zi0=zi, zie=zi+asct;  I * RESTRICT zv=AV(z)+l*asct-zi; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
      while(zi!=zie){I vv = hu[mwv[zi]]; vv=(vv<zi0)?zi:vv; hu[mwv[zi]]=(Ttype)vv; zv[zi]=vv-zi0;  ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ \
   case IIMODBASE0+IIMODREFLEX+IIMODFULL+IIDOT: \
   case IIMODBASE0+IIMODREFLEX+IIDOT: \
     {I zi=0;  I * RESTRICT zv=AV(z)+l*asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
      while(zi!=asct){I vv = hu[wv[zi]]; vv=(vv==asct)?zi:vv; hu[wv[zi]]=(Ttype)vv; zv[zi]=vv;  ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ \
   case IIMODREFLEX+IIMODFULL+IICO: \
   case IIMODREFLEX+IICO: \
     {I zi=hh->currentindexofst, zi0=zi, zie=zi+asct;  I * RESTRICT zv=AV(z)+l*asct-zi; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; \
      do{--zie; I vv = hu[mwv[zie]]; vv=(vv<zi0)?zie:vv; hu[mwv[zie]]=(Ttype)vv; zv[zie]=vv-zi0;}while(zie!=zi); } break; /* same in reverse */ \
   case IIMODBASE0+IIMODREFLEX+IIMODFULL+IICO: \
   case IIMODBASE0+IIMODREFLEX+IICO: \
     {I zie=asct-1;  I * RESTRICT zv=AV(z)+l*asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; \
      do{I vv = hu[wv[zie]]; vv=(vv==asct)?zie:vv; hu[wv[zie]]=(Ttype)vv; zv[zie]=vv; --zie; }while(zie>=0); } break; /* same in reverse */ \
 \
    /* NUB types use Boolean masks but do not depend on FULL, since they cannot miss.  But they must support PACK. */ \
   case IIMODFULL+INUBSV: \
   case INUBSV:            {SNUB (B * RESTRICT zv=BAV(z)+l*asct;  ,  zv[zi]=v;) } break; \
   case IIMODPACK+IIMODFULL+INUBSV: \
   case IIMODPACK+INUBSV:  {SNUBP(B * RESTRICT zv=BAV(z)+l*asct;  ,  zv[zi]=v;) } break; \
   case IIMODFULL+INUB: \
   case INUB:              {SNUB (T * RESTRICT zv=(T*)AV(z); T *zv0=zv;   ,  *zv=wv[zi]; zv+=v;) AS(z)[0]=zv-zv0; AN(z)=n*(zv-zv0); } break;  \
   case IIMODPACK+IIMODFULL+INUB: \
   case IIMODPACK+INUB:    {SNUBP(T * RESTRICT zv=(T*)AV(z); T *zv0=zv;   ,  *zv=wv[zi]; zv+=v;) AS(z)[0]=zv-zv0; AN(z)=n*(zv-zv0); } break;  \
   case IIMODFULL+INUBI: \
   case INUBI:             {SNUB (I * RESTRICT zv=AV(z); I *zv0=zv;   ,   *zv=zi; zv+=v;) AS(z)[0]=AN(z)=zv-zv0; } break;  \
   case IIMODPACK+IIMODFULL+INUBI: \
   case IIMODPACK+INUBI:   {SNUBP(I * RESTRICT zv=AV(z); I *zv0=zv;   ,   *zv=zi; zv+=v;) AS(z)[0]=AN(z)=zv-zv0; } break;  \
     /* non-reflexives can benefit from FULL checking.  IIDOT and IICO need full indexes (and thus use BASE0, but no PACK); everything else is Boolean */\
   case IIDOT:             {SDQA(T,Ttype);  SCOZ(T,Ttype,asct);} break;  \
   case IIMODFULL+IIDOT:   {SDQA(T,Ttype);  SCOZF(T,Ttype,asct);} break;  \
   case IIMODBASE0+IIDOT:  {SDQA0(T,Ttype);  SCOZ0(T,Ttype,asct);} break;  \
   case IIMODBASE0+IIMODFULL+IIDOT: {SDQA0(T,Ttype);  SCOZF0(T,Ttype,asct);} break;  \
   case IICO:              {SDOA(T,Ttype);  SCOZ(T,Ttype,asct);} break;  \
   case IIMODFULL+IICO:    {SDOA(T,Ttype); SCOZF(T,Ttype,asct);} break;  \
   case IIMODBASE0+IICO:   {SDOA(T,Ttype);  SCOZ0(T,Ttype,asct);} break;  \
   case IIMODBASE0+IIMODFULL+IICO: {SDOA(T,Ttype); SCOZF0(T,Ttype,asct);} break;  \
    /* Boolean indexes from here on.  These must support FULL and PACK.  bitdef appears twice */ \
   SMFULLPACKEPS(T,0,IEPS,  (T,0,zv[i]=v;)) /* EPS scans FULL args backwards for cache coherence */ \
   SMFULLPACK(T,1,ILESS,  DCLZVO(T,0) T *zv0=zv; , (T,1,*zv=mwv[i]; zv+=v;); AS(z)[0]= zv-zv0; AN(z)=n*(zv-zv0);)  \
   SMFULLPACK(T,0,IINTER,  DCLZVO(T,0) T *zv0=zv; , (T,0,*zv=mwv[i]; zv+=v;); AS(z)[0]= zv-zv0; AN(z)=n*(zv-zv0);)  \
   SMFULLPACK(T,0,II0EPS, DCLZVO(I,0) I s=wsct; , (T,0,if(!v){s+=i; break;});*zv++=s;)  \
   SMFULLPACK(T,0,II1EPS, DCLZVO(I,0) I s=wsct; , (T,0,if(v){s+=i; break;});*zv++=s; )  \
   SMFULLPACQ(T,0,IJ0EPS, DCLZVQ(I,0) I s=wsct; , (T,0,if(!v){s=i; break;});*zv++=s;)  \
   SMFULLPACQ(T,0,IJ1EPS, DCLZVQ(I,0) I s=wsct; , (T,0,if(v){s=i; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,IANYEPS,DCLZVO(B,0) B s=0; , (T,0,if(v){s=1; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,IALLEPS,DCLZVO(B,0) B s=1; , (T,0,if(!v){s=0; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,ISUMEPS,DCLZVO(I,0) I s=0; , (T,0,s+=v;); *zv++=s;)  \
   SMFULLPACK(T,0,IIFBEPS,DCLZVO(I,0) I *zv0=zv; , (T,0,*zv=i+wsct; zv+=v;); AS(z)[0]=AN(z)=zv-zv0;)  \
   }  \
  }  \
  R h; \
 }

// The verbs to do the work, for different item lengths and hashtable sizes
IOFSMALLRANGE(jtio12,UC, US)  IOFSMALLRANGE(jtio14,UC, UI4)  // 1-byte items, using small/large hashtable
IOFSMALLRANGE(jtio22,US, US)  IOFSMALLRANGE(jtio24,US, UI4)  // 2-byte items, using small/large hashtable
IOFSMALLRANGE(jtio42,UI4,US)  IOFSMALLRANGE(jtio44,UI4,UI4)  // 4-byte items, using small/large hashtable
IOFSMALLRANGE(jtio82,I,  US)  IOFSMALLRANGE(jtio84,I,  UI4)  // SZI-byte items, using small/large hashtable
#endif
