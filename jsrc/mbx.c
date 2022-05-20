/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory-Mapped Boxed Arrays                                              */

#if 0
#include "j.h"


/* A SMM array w is laid-out as follows:                                   */
/*                                                                         */
/* Initial part of w is like a regular array of type A                     */
/* Following this part (after the ravel of w) is the SMM area              */
/* Let u point to the first byte of the SMM area                           */
/*                                                                         */
/* length        usage                                                     */
/* 4*AH          J array header; flag has AFNJA bit                        */
/* 4*64          shape padded to rank of 64                                */
/* 4*n           array elements, offsets from w                            */ 
/* ...                                                                     */
/* 4*MLEN        mfree pointers                                            */
/*                                                                         */
/* each SMM array entry has the usual MS*x fields,                         */
/*   j: mfree/msize index                                                  */
/*   a: pointer to next block (when in free list)                          */
/*      w-x                   (when allocated)                             */

#define SMMCTOTAL       0
#define SMMCINUSE       1
#define SMMCFREE        2

#define RMBX            64L                                  /* max rank for mbx      */
#define SMMFREE(a)      (I**)((I)(a)+(allosize(a)&-8)-SZI*MLEN)   /* address of free lists */

#define AABS(rel,k)     (k)  // used by Mbx
#define AREL(abs,k)     ((I)(abs)-(I)(k))   /* relative address from absolute address */  // used by Mbx
#define ARELATIVE(w)    0  // used by Mbx
#define RELBASEASGN(lett,w)

#define AFSMM           (I)0            /* SMM managed                     */
#define AFREL           (I)0            // uses relative addressing.

// **** following is the layout of SMM blocks, which keep a header before the allocated area
typedef struct {I*a;US j;US blkx;} MS;

#define mhb sizeof(MS)                  /* # bytes in memory header             */

static F1(jtsmmblkf);


static I smmsize(A a){
 R (allosize(a)&-8)-SZI*(SMMAH+RMBX+MLEN)-SZA*AN(a)-SZI*((SMMAH+RMBX+MLEN+AN(a))&1);
}    /* size of allocateable area */

static C*smmu(A a){I v;
 v=(I)(a)+SZI*(SMMAH+RMBX)+AN(a)*SZA;  /* 1st allocateable address   */
 R (C*)(((4+v)>>3)<<3);             /* ensure double word aligned */
}    /* first allocateable address */

static B jtsmminit(J jt,A a){C*u;I j,k,**mfree,n;MS*x;
 n=smmsize(a);
 ASSERT(0<n,EVALLOC);
 k=64; j=6; n=n>>j;
 mfree=SMMFREE(a); DO(MLEN, mfree[i]=0;);
 u=smmu(a);
 while(n){
  if(1&n){x=(MS*)u; x->j=(C)j; x->a=0; mfree[j]=(I*)AREL(x,a); u+=k;}
  n>>=1; k+=k; ++j;
 }
 R 1;
}    /* initialize SMM area */


void smmfrr(A w){A a;A1*wv;I j,**mfree;MS*x;
 x=(MS*)w-1;
 a=(A)AABS(x,x->a);
 j=x->j;
 if(BOX&AT(w)){wv=A1AV(w); DO(AN(w), smmfrr((A)AABS(w,wv[i])););}
 mfree=SMMFREE(a);
 x->a=mfree[j]; 
 mfree[j]=(I*)AREL(x,a);
}    /* free */

// Return the size of the allocated block w.  This is a power of 2.
I smmallosize(A w){
  R (((I)1)<<(((MS *)w-1)->j));
}

static B smmsplit(A a,I j){I i,k,**mfree,p;MS*x,*y;
 mfree=SMMFREE(a); p=MLEN;
 i=j;
 while(p>i&&!mfree[i])++i;
 if(!(p>i))R 0;
 k=(I)1<<(i-1);
 while(j<i){
  x=(MS*)AABS(a,mfree[i]);
  mfree[i]=x->a;
  y=(MS*)(k+(C*)x); y->a=0; y->j=(S)(i-1);
  x->a=(I*)AREL(y,a); x->j=(S)(i-1);
  mfree[i-1]=(I*)AREL(x,a);
  --i; k>>=1;
 }
 R 1;
}    /* ensure mfree[j] has a free block by splitting larger blocks */

static void smmput1(A a,I**mfree,I n,C*v){I j,k;MS*x;
 k=64; j=6; n>>=j;
 while(n){
  if(1&n){x=(MS*)v; x->j=(C)j; x->a=mfree[j]; mfree[j]=(I*)AREL(x,a); v+=k;}
  n>>=1; k+=k; ++j; 
}}   /* put block v of size n into free list(s) */

static B jtsmmjoin(J jt,A a,I j){A y;I m,**mfree,n,*p,*q;
 RZ(y=smmblkf(a));
 n=AS(y)[0];
 RZ(1<n);
 RZ(y=grade2(y,y));
 p=q=AV(y); m=0;
 DQ(n-1, if(p[2]==p[0]+p[1])q[1]+=p[3]; else{q+=2; q[0]=p[2]; q[1]=p[3]; ++m;} p+=2;);
 ++m;
 mfree=SMMFREE(a); DO(MLEN, mfree[i]=0;);
 p=AV(y);
 DQ(m, smmput1(a,mfree,p[1],(C*)p[0]); p+=2;);
 R mfree[j]||smmsplit(a,j);
}    /* ensure mfree[j] has a free block by joining smaller blocks */

static A jtsmma(J jt,A a,I m){A z;I j,n,**mfree,p;MS*x;
 JBREAK0;
 n=p=m+mhb; 
 ASSERT(n<=JT(jt,mmax),EVLIMIT);
 j=6; n>>=j; while(n){n>>=1; ++j;}
 if(p==(I)1<<(j-1))--j;
 mfree=SMMFREE(a);
 ASSERT(mfree[j]||smmsplit(a,j)||smmjoin(a,j),EVALLOC);
 x=(MS*)AABS(a,mfree[j]);
 z=(A)(1+x); 
 mfree[j]=x->a;
 x->a=(I*)AREL(a,x);
 R z;
}    /* allocate */


static A jtsmmga(J jt,A a,I t,I n,I r,I*s){A z;I m,w;
 w=WP(t,n,r); m=SZI*w; 
 ASSERT(RMAX>=r&&m>n&&n>=0&&m>w&&w>0,EVLIMIT);   /* beware integer overflow */
 RZ(z=smma(a,m));
 AT(z)=t; ACX(z); AN(z)=n; AR(z)=(RANKT)r; AFLAG(z)=AFSMM; AK(z)=AKX(z);
 if(r&&s)ICPY(AS(z),s,r); else AS(z)[0]=n;
 if(t&LAST0)*((I*)z+w-1)=0;
 R z;
}

static B jtsmmin(J jt,A a,A w){A*wv;I wd;MS*x;
 if(AFNJA&AFLAG(w))R a==w;  // if w is an NJA name, see if it equals a
 if(AFSMM&AFLAG(w)){  // if an SMM name, the prefix to the header points to the base block
  x=(MS*)w-1;
  if((I)a==(I)AABS(x,x->a))R 1;  // here w is a box allocated inside a
 }
 // otherwise, recur on contents
 if(BOX&AT(w)){wv=AAV(w); wd=(I)w*ARELATIVE(w); DO(AN(w), if(smmin(a,wv[i]))R 1;);}
 R 0;
}   /* 1 iff any leaf of w is part of SMM array a */

F2(jtsmmcar){A*wv,x,z;A1*zv;I n,t;
 ARGCHK1(w);
 n=AN(w); t=AT(w); 
 ASSERT(t&B01+LIT+C2T+C4T+INT+FL+CMPX+BOX+SBT,EVDOMAIN);
 RZ(z=smmga(a,t,n,AR(w),AS(w)));
 zv=A1AV(z); wv=AAV(w);
 if(t&BOX){RELBASEASGN(w,w) ; DO(n, RZ(x=smmcar(a,wv[i])); zv[i]=AREL(x,z););}
 else MC(zv,wv,n<<bplg(t));
 RETF (z);
}    /* make copy of w in SMM area of a */

F2(jtsmmis){A*wv,x;A1*av;I wn,wr;
 ARGCHK2(a,w);   
 if(a==w)R a;
 wn=AN(w); wr=AR(w);
 if(smmin(a,w))RZ(w=cpa(1,w));
 AK(a)=SZI*(SMMAH+64); AT(a)=AT(w); AN(a)=wn; AR(a)=(RANKT)wr;
 if(!smminit(a)){AT(a)=LIT; AN(a)=0; AR(a)=1; AS(a)[0]=0; R 0;}
 av=A1AV(a); wv=AAV(w); RELBASEASGN(w,w) ;
 DO(wn, x=smmcar(a,wv[i]); if(!x){AT(a)=LIT; AN(a)=0; AR(a)=1; AS(a)[0]=0; R 0;} av[i]=AREL(x,a););
 ICPY(AS(a),AS(w),wr);
 RETF(a);
}    /* a=:w where a is mapped and w is boxed */


A jtcpa(J jt,B b,A w){A*wv,z,*zv;
 if(0==b&&AFNJA&AFLAG(w)){R ras(w); }
 if(!(BOX&AT(w)))R ca(w);
 wv=AAV(w); RELBASEASGN(w,w) ;
 GATV(z,BOX,AN(w),AR(w),AS(w)); zv=AAV(z);
 DO(AN(w), RZ(zv[i]=cpa(b,wv[i])););
 R z;
}    /* copy w down to leaves, recursing on boxed AFNJA iff 1=b */

static B leafrel(A w){A*v;
 if(BOX&AT(w)){
  if(AFLAG(w)&(AFNJA))R 0;   // exit fast if we know there's no relative
   // kludge we could improve this by noting that absence of NOSMREL is decisive if recursive usecount
  if(AFLAG(w)&AFREL)R 1;
  v=AAV(w); DO(AN(w), if(leafrel(v[i]))R 1;);
 }
 R 0;
}    /* 1 iff a leaf of w contains a relative address */

// if w is non-boxed but in the memory-mapped system, clone it to get it out of the memory-mapped boxed area
// if boxed, check to see if any leaf has AFSMM or AFREL; if so, clone the whole thing
F1(jtrca){
 ARGCHK1(w);
 if(!(BOX&AT(w)))R AFSMM&AFLAG(w)?ca(w):w;
 R leafrel(w)?cpa(0,w):w;
}



static F1(jtsmmblkf){A z;I**mfree,p,q,*v,*zv;MS*x;
 ARGCHK1(w);
 mfree=SMMFREE(w);
 p=MLEN; q=0; 
 DO(p, v=mfree[i]; while(v){x=(MS*)AABS(v,w); ++q;                           v=x->a;});
 GATV0(z,INT,2*q,2); AS(z)[0]=q; AS(z)[1]=2; zv=AV(z);
 DO(p, v=mfree[i]; while(v){x=(MS*)AABS(v,w); *zv++=(I)x; *zv++=(I)1<<(x->j); v=x->a;});
 RETF(z);
}    /* blocks free as a 2-column matrix of (address,size) */

static I smmblkun(B b,A w){A1*wv;I z=0;MS*x;
 x=(MS*)w-1;
 if(b&&x->a)z=1;
 if(BOX&AT(w)){wv=A1AV(w); DO(AN(w), z+=smmblkun(1,   (A)AABS(wv[i],w)););}
 R z;
}    /* # of lines in the result of smmblku */

static I* smmblku1(B b,I*zv,A w){A1*wv;MS*x;
 x=(MS*)w-1;
 if(b&&x->a){*zv++=(I)x; *zv++=(I)1<<(x->j);}
 if(BOX&AT(w)){wv=A1AV(w); DO(AN(w), zv=smmblku1(1,zv,(A)AABS(wv[i],w)););}
 R zv;
}

static A jtsmmblku(J jt,A w){A z;I n;
 ARGCHK1(w);
 n=smmblkun(0,w);
 GATV0(z,INT,2*n,2); AS(z)[0]=n; AS(z)[1]=2;
 smmblku1(0,AV(z),w);
 R z;
}    /* blocks in use */

F1(jtsmmblks){A x,y,z;I n,t,*v,*zv;
 ARGCHK1(w);
#if FORCEVIRTUALINPUTS
 // To allow us to run mbx tests in FORCEVIRTUAL compilation, chase any VIRTUAL block back to its ultimate backing block and get stats for that
 while(AFLAG(w)&AFVIRTUAL)w=ABACK(w);
#endif
 t=AT(w); 
 ASSERT(AFNJA&AFLAG(w)&&t&BOX,EVDOMAIN);
 RZ(x=smmblku(w));
 RZ(y=smmblkf(w));
 n=1+AS(x)[0]+AS(y)[0];
 GATV0(z,INT,3*n,2); AS(z)[0]=n; AS(z)[1]=3; zv=AV(z); 
 *zv++=IMIN; *zv++=IMIN; *zv++=IMIN;
 v=AV(x); DQ(AS(x)[0], *zv++=*v++; *zv++=*v++; *zv++=SMMCINUSE;);
 v=AV(y); DQ(AS(y)[0], *zv++=*v++; *zv++=*v++; *zv++=SMMCFREE; );
 RZ(z=grade2(z,z)); zv=AV(z);
 *zv++=(I)smmu(w); *zv++=smmsize(w); *zv++=SMMCTOTAL;
 RETF(z);
}    /* 15!:12 all the blocks in an SMM variable as 3-column matrix */


// F2(jtafr2){A x,*wv;A1*wu;
//  ARGCHK2(a,w);
//  wv=AAV(w); wu=A1AV(w);
//  DO(AN(w), x=(A)AABS(wu[i],a); if(BOX&AT(x))RZ(x=afr2(a,x)); wv[i]=x;);
//  R w;
// }    /* w has addresses relative to a; works in place */

A relocate(I m,A w){A1*wv; ARGCHK1(w); AFLAG(w)|=AFREL; wv=A1AV(w); DO(AN(w), wv[i]+=m;); R w;}
     /* add m to the addresses in w; works in place */
#endif