/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Boolean-Valued                                                   */

#include "j.h"
#include "ve.h"

#ifdef MMSC_VER
#pragma warning(disable: 4244)
#endif

BPFX( andBB, AND ,BAND, AND, BAND, _mm256_and_pd(u256,v256) , , )    
BPFX(  orBB, OR  ,BOR,  OR,  BOR, _mm256_or_pd(u256,v256) , , )    
BPFX(nandBB, NAND,BNAND,NAND,BNAND, _mm256_xor_pd(bool256,_mm256_and_pd(u256,v256)) , , __m256d bool256=_mm256_broadcast_sd((D*)&Ivalidboolean); )
BPFX( norBB, NOR ,BNOR, NOR, BNOR, _mm256_xor_pd(bool256,_mm256_or_pd(u256,v256)) , , __m256d bool256=_mm256_broadcast_sd((D*)&Ivalidboolean); )


F1(jtrazein){A z; R dfv2(z,w,box(raze(w)),amp(swap(ds(CEPS)),ds(COPE)));}


static F2(jtebarmat){A ya,yw,z;B b,*zv;C*au,*av,*u,*v,*v0,*wu,*wv;I*as,c,i,k,m,n,r,s,si,sj,t,*ws;
 ARGCHK2(a,w);
 as=AS(a);      av=CAV(a);
 ws=AS(w); v=v0=wv=CAV(w);
 si=as[0]; m=1+ws[0]-si;
 sj=as[1]; n=1+ws[1]-sj;
 t=AT(w); k=bpnoun(t); c=ws[1]; r=k*c; s=k*sj;
 GATVR(z,B01,AN(w),2,ws); zv=BAV2(z); mvc(AN(z),zv,MEMSET00LEN,MEMSET00);
 if(t&B01+LIT+C2T+C4T+INT+INT2+INT4+SBT||1.0==jt->cct&&t&FL+CMPX+QP)
  for(i=0;i<m;++i){
   DO(n, u=av; b=1; DO(si,                         if(memcmpne(u,v,s)){b=0; break;} u+=s; v+=r;); v=v0+=k; zv[i]=b;);
   zv+=c; v=v0=wv+=r;
 }else{
  GA10(ya,t,sj); au=CAV(ya);
  GA10(yw,t,sj); wu=CAV(yw);
  for(i=0;i<m;++i){
   DO(n, u=av; b=1; DO(si, MC(au,u,s); MC(wu,v,s); if(!equ(ya,yw)  ){b=0; break;} u+=s; v+=r;); v=v0+=k; zv[i]=b;);
   zv+=c; v=v0=wv+=r;
 }}
 RETF(z);
}    /* E. on matrix arguments */

static F2(jtebarvec){A y,z;B*zv;C*av,*wv,*yv;I an,k,n,s,t,wn;
 ARGCHK2(a,w); a=C(a); w=C(w);
 an=AN(a); av=CAV(a); 
 wn=AN(w); wv=CAV(w); n=1+wn-an; 
 t=AT(w); k=bpnoun(t); s=k*an;
 GATV0(z,B01,wn,AR(w)?1:0); zv=BAV(z); 
 if((-an&(an-wn))<0)mvc(wn-n,zv+n,MEMSET00LEN,MEMSET00); else mvc(wn,zv,MEMSET00LEN,MEMSET00);
 if(t&B01+LIT+C2T+C4T+INT+INT2+INT4+SBT||1.0==jt->cct&&t&FL+CMPX+QP)DO(n, zv[i]=!memcmpne(av,wv,s); wv+=k;)
 else{I yr=AR(a); GA0(y,t,an,AR(a)); yv=CAVn(yr,y); DO(n, MC(yv,wv,s); zv[i]=equ(a,C(y)); wv+=k;);}
 RETF(z);
}    /* E. on vector arguments */

/* preparations for ebar                    */
/* return code meaning:                     */
/* >0 is cardinality of range               */
/* <0:                                      */
/* -1: not homogeneous                      */
/* -2: rank is 2                            */
/* -3: rank > 2                             */
/* -4: not discrete type or range too large */

static I jtebarprep(J jt,A a,A w,A*za,A*zw,I*zc){I ar,at,m,n,t,wr,wt,memlimit;CR rng;
 ar=AR(a); at=AT(a); m=AN(a);
 wr=AR(w); wt=AT(w); n=AN(w);
 ASSERT(!ISSPARSE(at|wt),EVNONCE);
 ASSERT(ar==wr||(ar+(wr^1))==0,EVRANK);
 if(unlikely(!HOMO(at,wt)))if(m&&n)R -1;
 if(1<wr)R 2==wr?-2:-3;
 t=maxtyped(at|(I )(m==0),wt|(I )(n==0)); t&=-t;  // default missing type to B01; if we select one, discard higher bits
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt))RZ(w=cvt(t,w));
 *za=a; *zw=w;
 // The inputs have been converted to common type
 memlimit = MIN(4*n+1,(I)((JT(jt,mmax)-100)>>LGSZI));  // maximum size we will allow our d to reach.  Used only for I type.
  // 4*the size of the search area seems big enough; but not more than what a single memory allocation supports.  The size
  // is measured in Is.  The 100 is to account for memory-manager overhead.  Minimum value must be > 0 for the <= test below
 switch(CTTZNOFLAG(t)){
 // calculate the number of distinct values in the range of the two operands.
 // for strings, we just assume the worst (all codes)
 // for ints, actually look at the data to get the range (min and #values+1).  If the min is > 0, and
 // the range can be extended to cover 0..d-1 without exceeding the bound on d, do so to make
 // the SUB0 and SUB1 expressions into EBLOOP simpler
 // We allocate an array for each result in range, so we have to get c and d right
 case C2TX:
 case LITX:
 case B01X: rng.min=0; rng.range=65536; rng.range=t&B01?2:rng.range; rng.range=t&LIT?256:rng.range; break;
 case INTX: case SBTX: rng = condrange(AV(a),m,IMAX,IMIN,memlimit);
            if(rng.range){rng = condrange(AV(w),n,rng.min,rng.min+rng.range-1,memlimit);} break;
 case C4TX: rng = condrange4(C4AV(a),m,-1,0,memlimit);
            if(rng.range){rng = condrange4(C4AV(w),n,rng.min,rng.min+rng.range-1,memlimit);} break;
 }
 if(0<rng.min&&rng.range&&rng.min+rng.range<=memlimit){rng.range+=rng.min; rng.min=0;}  // Extend lower bound to 0 if that doesn't make range too big
 *zc=rng.min;  // Now that we know c, return it
 // if the range of integers is too big, revert to simple search.
 // Also revert for continuous type.  But always use fast search for character/boolean types
 R t&B01+LIT+C2T||t&INT+SBT+C4T&&BETWEENC(rng.range,1,memlimit) ? rng.range : -4;
}

#define EBLOOP(T,SUB0,SUB1,ZFUNC)  \
   {T*u=(T*)av,*v=(T*)wv;                                            \
    DO(m, yv[SUB0]=m-i;);                                            \
    while(k< p){for(i=0;i<m&&u[i]==v[k+i];++i); ZFUNC; k+=yv[SUB1];} \
    if   (k==p){for(i=0;i<m&&u[i]==v[k+i];++i); ZFUNC;             } \
   }

#if C_AVX2 || EMU_AVX2
// fast scanner.  wn must not be 0, an must be > 1
// type>>56 is 0 (E.), 1 (+/@E.), 2 (I.@E.), 3 (i.&1@E.), 4(+./@E.)
// parameter and return interpretation depend on function
static A jtebar1C(J jt, C *av, C *wv, I an, I wn, C* zv, I type, A z){
 // Init 32-byte copies of the first characters to match
 __m256i a0 = _mm256_set1_epi8(av[0]);  // fetch first byte into all byte positions
 __m256i a1 = _mm256_set1_epi8(av[1]);  // fetch first byte into all byte positions
 // figure endpoint for fast search: 32 bytes before end if an <=32; 
 // if an > 32, we will skip the final section if the endpoint is end-an+1: no match possible if wv=wvend
 I temp = an - 1; temp = temp <32?32: temp;   // back up 32 bytes, or n-1, whichever is greater
 C *wvend = wv + wn - temp;
 // Load the search string - up to 32 bytes of it - into a register.  Don't overfetch end of buffer
 temp = 31; temp = an<temp?an:temp;  // length of valid data, max 31.  If any byte of an 8-byte section is valid, we can fetch the whole section
#ifdef MMSC_VER
 __m256i a32 = _mm256_maskload_epi64((__int64*) av, _mm256_loadu_si256((__m256i*)(validitymask + (3 - (temp >> 3)))));
#else
 __m256i a32 = _mm256_maskload_epi64((long long const *) av, _mm256_loadu_si256((__m256i*)(validitymask + (3 - (temp >> 3)))));
#endif
 // Load the mask of bits that must be set to declare a match on the whole string
 temp = 32; temp = an<temp?an:temp; I fullmatchmsk = type + (UI4)(-(1LL<<temp));  // high-order 1 bits past the part that needs to be 1s in the mask - max length is 32  top 32 bits 0
 // We put type into fullmatchmsk to save a register
 // scan full 31-byte sections until we hit one containing the first 2 characters
 C *wv0=wv;  // save initial pointer, to get index of match
 UI match0=0;  // bit 32 is the match status of the byte before wv; 0 initially
 // the match status of the first character stored in bit 32
 UI4 matchmsk; __m256i ws;
 while(wv<wvend){
#define EBAR1(offset)  {ws=_mm256_loadu_si256((__m256i*)(wv+offset*32)); match0 = ((UI)(UI4)_mm256_movemask_epi8(_mm256_cmpeq_epi8(a0, ws)))*2+(match0>>32); \
 matchmsk=match0&_mm256_movemask_epi8(_mm256_cmpeq_epi8(a1,ws)); if(unlikely(matchmsk!=0)){wv+=offset*32; goto matchfnd;}}

  UI n2=DUFFLPCTV((wvend-wv)+32,2,LGSZI+LGNPAR);  /* # turns through duff loop */
  if(n2>0){
   I backoff=DUFFBACKOFFV((wvend-wv)+32,2,LGSZI+LGNPAR);
   wv += (backoff+1)*(SZI*NPAR);
   switch(backoff){
   do{
   case -1: EBAR1(0) case -2: EBAR1(1) case -3: EBAR1(2) case -4: EBAR1(3)
   wv +=4*(SZI*NPAR);
   }while(--n2!=0);
   }
  }
  break;  // if no start found, exit loop.  Back wv to the 1st character, because we haven't checked that position yet
matchfnd: ;
  C *endwv=wv+32;  // after we try these 32 start positions, wv will advance to next block
   // match.  Trying each possible match, refetch it and see if it matches the entire search string
   // the matches found in matchmsk start at wv-1; we back up the pointer now to indicate that.  At the end
   // we will restore to the position in scan
  --wv;  // make wv position agree with matchmsk
  do{
   wv += CTTZI(matchmsk);  // advance to possible match
   matchmsk >>= CTTZI(matchmsk);  // shift the bit of the first match off, so we start after it for the next match
   matchmsk>>=1;
   if(wv < wvend){   // if there are enough bytes left to try fetching
    ws = _mm256_loadu_si256((__m256i*)wv);  // fetch the string, in which the first 2 bytes match
    I match32 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a32, ws));
    // output the result if any; advance 1 byte to continue search for start characters
    match32 = ((I4)fullmatchmsk|match32)==(I4)~0;  // if all bits =, produce a 1.  match32 has garbage in top 8 bits
    // if the search string is longer than 32 bytes, finish the comparison
    if((-match32&(32-an))<0)match32=1^memcmpne(av+32,wv+32,an-32);
    // perform the action based on the input type (now in fullmatchmsk)
    if((fullmatchmsk>>56)<1){zv[wv-wv0] = (C)match32;  // E.
    }else if((fullmatchmsk>>56)==1){zv+=match32&1;   // +/@E.
    }else if((fullmatchmsk>>56)<3){     // I.@E.  extend if needed; always write result to avoid misbranch
     if(unlikely((I*)zv==IAV(z)+AN(z))){I m=AN(z); RZ(z=ext(0,z)); zv=(C*)(m+IAV(z));} *(I*)zv=wv-wv0; zv=(C*)((I*)zv+(match32&1));
    }else{ if(match32&1)if((fullmatchmsk>>56)==3)R sc(wv-wv0);  // i.&1@:E.
    else R num(1);  // +./@E.
    }
    wv += 1;
   }else{endwv=wv+1; matchmsk=0;}  // if we run past the valid compare area, stop comparing & don't advance pointers.  Set wv so that after we exit the loop and back up, we are on the wv we found here
  }while(matchmsk!=0);
  wv=endwv;  // step up to next block, skipping trailing 0s in this block.  This undoes the decrementing of wv
 }
 // There is one trailing section of 0<length<=32.  Process it without fetching out of bounds
 // If an>32, there is no need to look
 if(an<=32){
  // This is like the loop above, but we avoid overfetch and exit the loop when out of data.  We crawl through the match positions
  // The wv passed in from above points to the second character checked, so we have to back up one position
  wv-=wv!=wv0;  // back up wv to point to the first possible match position - unless that would be before the start of the data
  wvend=wv0+wn-an+1;  // first inadmissible position
  while(wv<wvend){
   // read in valid bytes to end of string
#ifdef MMSC_VER
   __m256i ws = _mm256_maskload_epi64((__int64*) wv, _mm256_loadu_si256((__m256i*)(validitymask + MIN((3 - ((wv0+wn-wv-1) >> 3))))));  // bytes of w
#else
   __m256i ws = _mm256_maskload_epi64((long long const *) wv, _mm256_loadu_si256((__m256i*)(validitymask + (3 - MIN(3,((wv0+wn-wv-1) >> 3))))));  // bytes of w
#endif
   // see if the first 2 characters are matched in sequence
   I4 match0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a0, ws)); UI4 match1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a1, ws));
   matchmsk = match0&(match1>>1);
   if(matchmsk&1){
    // match on 1st char.  See if it all matches in this position
    match0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a32, ws));
    // output the result if any; advance 1 byte to continue search for start characters
    match0 = ((I4)fullmatchmsk|match0)==(I4)~0;  // if all bits =, produce a 1.  match0 has garbage in top 8 bits
    // perform the action based on the input type (now in fullmatchmsk)
    if((fullmatchmsk>>56)<1){zv[wv-wv0] = (C)match0;  // E.
    }else if((fullmatchmsk>>56)==1){zv+=match0&1;   // +/@E.
    }else if((fullmatchmsk>>56)<3){     // I.@E.  extend if needed; always write result to avoid misbranch
     if((I*)zv==IAV(z)+AN(z)){I m=AN(z); RZ(z=ext(0,z)); zv=(C*)(m+IAV(z));} *(I*)zv=wv-wv0; zv=(C*)((I*)zv+(match0&1));
    }else{ if(match0&1)if((fullmatchmsk>>56)==3)R sc(wv-wv0);  // i.&1@:E.
    else R num(1);  // +./@E.
    }
    matchmsk &= ~1;  // turn off the match we have processed
   }
   wv += CTTZI(matchmsk|(1LL<<31));  // advance to possible match
  }
 }
 // Return with value appropriate for function
 if((fullmatchmsk>>56)<1){R 0;  // E.
 }else if((fullmatchmsk>>56)==1){R sc((I)zv);   // +/@E.
 }else if((fullmatchmsk>>56)<3){AN(z)=AS(z)[0]=(I*)zv-IAV(z); R z;     // I.@E.  install actual item count
 }else if((fullmatchmsk>>56)==3){R sc(wn);  // i.&1@:E.
 }else{R num(0);  // +./@E.
 }
}
#endif

F2(jtebar){PROLOG(0065);A y,z;B*zv;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 ARGCHK2(a,w);
 ASSERT(!ISSPARSE(AT(a)|AT(w)), EVNONCE);
 ASSERT((AR(a) == AR(w)) || (AR(a) + (AR(w) ^ 1)) == 0, EVRANK);
 if(AN(a)==1)R eq(reshape(mtv,a),w);  // if a is a singleton, just revert to =
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 if(unlikely(d<0)){
  switch(d){
  case -1: R reshape(shape(w),num(0));
  case -2: R ebarmat(a,w);
  case -3: R dfv2(z,shape(a),w,cut(amp(a,ds(CMATCH)),num(3)));
  case -4: R ebarvec(a,w);
  }
 }
 I zr=AR(w); GATV0(z,B01,n,AR(w)); zv=BAVn(zr,z); mvc(n,zv,1,iotavec-IOTAVECBEGIN+(m==0)); if((-m&-n)>=0)R z;  // if x empty, return all 1s
#if C_AVX2 || EMU_AVX2
 if(AT(w)&LIT+B01){jtebar1C(jt, av,wv, m,n,zv,0,0); R z;}
#endif
 GATV0(y,INT,d,1); yv= AV1(y); DO(d, yv[i]=1+m;);
 switch(CTTZ(AT(w))){
 case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(I, u[i],  v[k+m],   zv[k]=i==m); break;
 case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(SB,u[i],  v[k+m],   zv[k]=i==m); break;
 case C2TX:      EBLOOP(US,u[i],  v[k+m],   zv[k]=i==m); break;
 case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(C4,u[i],  v[k+m],   zv[k]=i==m); break;
#if !C_AVX2 && !EMU_AVX2
default:
            EBLOOP(UC,u[i],  v[k+m],   zv[k]=i==m);
#endif
 }
 EPILOG(z);
}    /* Daniel M. Sunday, CACM 1990 8, 132-142 */


F2(jti1ebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 ARGCHK2(a,w);

 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 if(unlikely(d<0)){
  switch(d){
  case -1: R sc(n);
  case -4: R indexof(ebarvec(a,w),num(1));
 }
 }
#if C_AVX2 || EMU_AVX2
 if(AT(w)&LIT+B01)R jtebar1C(jt, av,wv, m,n,0,3LL<<56,0);
#endif
 GATV0(y,INT,d,1); yv= AV1(y); DO(d, yv[i]=1+m;);
 switch(CTTZ(AT(w))){
 case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)R sc(k)); break;
 case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)R sc(k)); break;
 case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)R sc(k)); break;
 case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)R sc(k)); break;
#if !C_AVX2 && !EMU_AVX2
default:
       EBLOOP(UC,u[i],  v[k+m],   if(i==m)R sc(k));
#endif
 }
 R sc(n);
}    /* a (E. i. 1:) w where a and w are atoms or lists */

F2(jtsumebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv,z=0;
 ARGCHK2(a,w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 if(unlikely(d<0)){
  switch(d){
  case -1: R num(0);
  case -4: R aslash(CPLUS,ebarvec(a,w));
  }
 }
 if((-m&-n)>=0){R sc(n);}  // empty argument.  If m, it matches everywhere, so use n; if n, it's 0, use it
#if C_AVX2 || EMU_AVX2
 if(AT(w)&LIT+B01)R jtebar1C(jt, av,wv, m,n,0,1LL<<56,0);
#endif
 GATV0(y,INT,d,1); yv= AV1(y); DO(d, yv[i]=1+m;);
 switch(CTTZ(AT(w))){
 case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)++z); break;
 case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)++z); break;
 case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)++z); break;
 case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)++z); break;
#if !C_AVX2 && !EMU_AVX2
default:
       EBLOOP(UC,u[i],  v[k+m],   if(i==m)++z);
#endif
 }
 R sc(z);
}    /* a ([: +/ E.) w where a and w are atoms or lists */

F2(jtanyebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 ARGCHK2(a,w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 if(unlikely(d<0)){
  switch(d){
  case -1: R num(0);
  case -4: R aslash(CPLUSDOT,ebarvec(a,w));
  }
 }
 if((-m&-n)>=0){R num(SGNTO0(-n));}  // empty argument.  If m, it matches everywhere, so use n; if n, it's 0, use it - 0/1 only
#if C_AVX2 || EMU_AVX2
 if(AT(w)&LIT+B01)R jtebar1C(jt, av,wv, m,n,0,4LL<<56,0);
#endif
 GATV0(y,INT,d,1); yv= AV1(y); DO(d, yv[i]=1+m;);
 switch(CTTZ(AT(w))){
 case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)R num(1)) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)R num(1)); break;
 case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)R num(1)) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)R num(1)); break;
 case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)R num(1)); break;
 case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)R num(1)) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)R num(1)); break;
#if !C_AVX2 && !EMU_AVX2
default:
       EBLOOP(UC,u[i],  v[k+m],   if(i==m)R num(1));
#endif
 }
 R num(0);
}    /* a ([: +./ E.) w where a and w are atoms or lists */

#define IFB1  \
 {if(zu==zv){I m=zu-AV(z); RZ(z=ext(0,z)); zv=m+AV(z); zu=AN(z)+AV(z);} *zv++=k;}

F2(jtifbebar){A y,z;C*av,*wv;I c,d,i,k=0,m,n,p,*yv,*zu,*zv;
 ARGCHK2(a,w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 if(unlikely(d<0)){
  switch(d){
  case -1: R mtv;
  case -4: R icap(ebarvec(a,w));
  }
 }
 if((-m&-n)>=0){R icap(ebar(a,w));}  // empty argument.
 GATV0(z,INT,MAX(22,n>>7),1); zv=AV1(z); zu=zv+AN(z);
#if C_AVX2 || EMU_AVX2
 if(AT(w)&LIT+B01){
   if(m==1){R icap(ebar(a,w));}  // if a is 1 char, we can't use the fast code.  Other forms are checked in vcompsc
   R jtebar1C(jt, av,wv, m,n,CAV(z),2LL<<56,z);
 }
#endif
 GATV0(y,INT,d,1); yv= AV1(y); DO(d, yv[i]=1+m;);
 switch(CTTZ(AT(w))){
 case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)IFB1); break;
 case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)IFB1); break;
 case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)IFB1); break;
 case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)IFB1); break;
#if !C_AVX2 && !EMU_AVX2
default:
       EBLOOP(UC,u[i],  v[k+m],   if(i==m)IFB1);
#endif
 }
 AN(z)=AS(z)[0]=zv-AV(z);
 RETF(z);
}    /* a ([: I. E.) w where a and w are atoms or lists */
