/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved. */
/* License in license.txt.                                   */
/*                                                                         */
/* Verbs: Boolean-Valued                                                   */

#include "j.h"
#include "ve.h"


BPFX( andBB, AND ,BAND, AND, BAND, _mm256_and_pd(u256,v256) , , )    
BPFX(  orBB, OR  ,BOR,  OR,  BOR, _mm256_or_pd(u256,v256) , , )    
BPFX(nandBB, NAND,BNAND,NAND,BNAND, _mm256_xor_pd(bool256,_mm256_and_pd(u256,v256)) , , __m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101)); )
BPFX( norBB, NOR ,BNOR, NOR, BNOR, _mm256_xor_pd(bool256,_mm256_or_pd(u256,v256)) , , __m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101)); )


F1(jtrazein){R df2(w,box(raze(w)),amp(swap(ds(CEPS)),ds(COPE)));}


static F2(jtebarmat){A ya,yw,z;B b,*zv;C*au,*av,*u,*v,*v0,*wu,*wv;I*as,c,i,k,m,n,r,s,si,sj,t,*ws;
 RZ(a&&w);
 as=AS(a);      av=CAV(a);
 ws=AS(w); v=v0=wv=CAV(w);
 si=as[0]; m=1+ws[0]-si;
 sj=as[1]; n=1+ws[1]-sj;
 t=AT(w); k=bpnoun(t); c=ws[1]; r=k*c; s=k*sj;
 GATVR(z,B01,AN(w),2,ws); zv=BAV(z); memset(zv,C0,AN(z));
 if(t&B01+LIT+C2T+C4T+INT+SBT||1.0==jt->cct&&t&FL+CMPX)
  for(i=0;i<m;++i){
   DO(n, u=av; b=1; DO(si,                         if(memcmp(u,v,s)){b=0; break;} u+=s; v+=r;); v=v0+=k; zv[i]=b;);
   zv+=c; v=v0=wv+=r;
 }else{
  GA(ya,t,sj,1,0); au=CAV(ya);
  GA(yw,t,sj,1,0); wu=CAV(yw);
  for(i=0;i<m;++i){
   DO(n, u=av; b=1; DO(si, MC(au,u,s); MC(wu,v,s); if(!equ(ya,yw)  ){b=0; break;} u+=s; v+=r;); v=v0+=k; zv[i]=b;);
   zv+=c; v=v0=wv+=r;
 }}
 RETF(z);
}    /* E. on matrix arguments */

static F2(jtebarvec){A y,z;B*zv;C*av,*wv,*yv;I an,k,n,s,t,wn;
 RZ(a&&w);
 an=AN(a); av=CAV(a); 
 wn=AN(w); wv=CAV(w); n=1+wn-an; 
 t=AT(w); k=bpnoun(t); s=k*an;
 GATV0(z,B01,wn,AR(w)?1:0); zv=BAV(z); 
// obsolete  if(an&&wn>an)memset(zv+n,C0,wn-n); else memset(zv,C0,wn);
 if((-an&(an-wn))<0)memset(zv+n,C0,wn-n); else memset(zv,C0,wn);
 if(t&B01+LIT+C2T+C4T+INT+SBT||1.0==jt->cct&&t&FL+CMPX)DO(n, zv[i]=!memcmp(av,wv,s); wv+=k;)
 else{GA(y,t,an,AR(a),0); yv=CAV(y); DO(n, MC(yv,wv,s); zv[i]=equ(a,y); wv+=k;);}
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
 ASSERT(!((at|wt)&SPARSE),EVNONCE);
// obsolete  ASSERT(ar==wr||!ar&&1==wr,EVRANK);
 ASSERT(ar==wr||(ar+(wr^1))==0,EVRANK);
 if(m&&n&&!HOMO(at,wt))R -1;
 if(1<wr)R 2==wr?-2:-3;
 t=maxtyped(at|(I )(m==0),wt|(I )(n==0)); t&=-t;  // default missing type to B01; if we select one, discard higher bits
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt))RZ(w=cvt(t,w));
 *za=a; *zw=w;
 // The inputs have been converted to common type
 memlimit = MIN(4*n+1,(I)((jt->mmax-100)>>LGSZI));  // maximum size we will allow our d to reach.  Used only for I type.
  // 4*the size of the search area seems big enough; but not more than what a single memory allocation supports.  The size
  // is measured in Is.  The 100 is to account for memory-manager overhead.  Minimum value must be > 0 for the <= test below
 switch(CTTZNOFLAG(t)){
  // calculate the number of distinct values in the range of the two operands.
  // for strings, we just assume the worst (all codes)
  // for ints, actually look at the data to get the range (min and #values+1).  If the min is > 0, and
  // the range can be extended to cover 0..d-1 without exceeding the bound on d, do so to make
  // the SUB0 and SUB1 expressions into EBLOOP simpler
  // We allocate an array for each result in range, so we have to get c and d right
  case INTX: case SBTX: rng = condrange(AV(a),m,IMAX,IMIN,memlimit);
             if(rng.range){rng = condrange(AV(w),n,rng.min,rng.min+rng.range-1,memlimit);} break;
  case C4TX: rng = condrange4(C4AV(a),m,-1,0,memlimit);
             if(rng.range){rng = condrange4(C4AV(w),n,rng.min,rng.min+rng.range-1,memlimit);} break;
  case C2TX:
  case LITX:
  case B01X: rng.min=0; rng.range=shortrange[t&B01+LIT][1]; break;
 }
 if(0<rng.min&&rng.range&&rng.min+rng.range<=memlimit){rng.range+=rng.min; rng.min=0;}  // Extend lower bound to 0 if that doesn't make range too big
 *zc=rng.min;  // Now that we know c, return it
 // if the range of integers is too big, revert to simple search.
 // Also revert for continuous type.  But always use fast search for character/boolean types
// obsolete  R t&B01+LIT+C2T||t&INT+SBT+C4T&&0<rng.range&&rng.range<=memlimit ? rng.range : -4;
 R t&B01+LIT+C2T||t&INT+SBT+C4T&&(UI)(rng.range-1)<=(UI)(memlimit-1) ? rng.range : -4;
}

#define EBLOOP(T,SUB0,SUB1,ZFUNC)  \
   {T*u=(T*)av,*v=(T*)wv;                                            \
    DO(m, yv[SUB0]=m-i;);                                            \
    while(k< p){for(i=0;i<m&&u[i]==v[k+i];++i); ZFUNC; k+=yv[SUB1];} \
    if   (k==p){for(i=0;i<m&&u[i]==v[k+i];++i); ZFUNC;             } \
   }

F2(jtebar){PROLOG(0065);A y,z;B*zv;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R reshape(shape(w),num[0]);
  case -2: R ebarmat(a,w);
  case -3: R df2(shape(a),w,cut(amp(a,ds(CMATCH)),num[3]));
  case -4: R ebarvec(a,w);
 }
 GATV0(z,B01,n,AR(w)); zv=BAV(z); memset(zv,C0,n);
 GATV0(y,INT,d,1); yv= AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(I, u[i],  v[k+m],   zv[k]=i==m); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(SB,u[i],  v[k+m],   zv[k]=i==m); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   zv[k]=i==m); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(C4,u[i],  v[k+m],   zv[k]=i==m); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   zv[k]=i==m);
 }
 EPILOG(z);
}    /* Daniel M. Sunday, CACM 1990 8, 132-142 */


F2(jti1ebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R sc(n);
  case -4: R indexof(ebarvec(a,w),num[1]);
 }
 GATV0(y,INT,d,1); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)R sc(k)); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)R sc(k)); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)R sc(k)); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)R sc(k)); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)R sc(k));
 }
 R sc(n);
}    /* a (E. i. 1:) w where a and w are atoms or lists */

F2(jtsumebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv,z=0;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R num[0];
  case -4: R aslash(CPLUS,ebarvec(a,w));
 }
 GATV0(y,INT,d,1); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)++z); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)++z); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)++z); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)++z); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)++z);
 }
 R sc(z);
}    /* a ([: +/ E.) w where a and w are atoms or lists */

F2(jtanyebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R num[0];
  case -4: R aslash(CPLUSDOT,ebarvec(a,w));
 }
 GATV0(y,INT,d,1); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)R num[1]) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)R num[1]); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)R num[1]) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)R num[1]); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)R num[1]); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)R num[1]) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)R num[1]); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)R num[1]);
 }
 R num[0];
}    /* a ([: +./ E.) w where a and w are atoms or lists */

#define IFB1  \
 {if(zu==zv){I m=zu-AV(z); RZ(z=ext(0,z)); zv=m+AV(z); zu=AN(z)+AV(z);} *zv++=k;}

F2(jtifbebar){A y,z;C*av,*wv;I c,d,i,k=0,m,n,p,*yv,*zu,*zv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R mtv;
  case -4: R icap(ebarvec(a,w));
 }
 GATV0(z,INT,MAX(22,n>>7),1); zv=AV(z); zu=zv+AN(z);
 GATV0(y,INT,d,1); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)IFB1); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)IFB1); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)IFB1); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)IFB1); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)IFB1);
 }
 AN(z)=*AS(z)=zv-AV(z);
 RETF(z);
}    /* a ([: I. E.) w where a and w are atoms or lists */
