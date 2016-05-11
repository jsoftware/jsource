/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

#ifdef _WIN32
#include <windows.h>
#else
#define __cdecl
#endif

#include "j.h"


#define PSIZE       65536L         /* size of each pool                    */
#define PLIM        1024L          /* pool allocation for blocks <= PLIM   */
#define PLIML       10L            /* base 2 log of PLIM                   */

I mhb=sizeof(MS);                  /* # bytes in memory header             */
I mhw=sizeof(MS)/SZI;              /* # words in memory header             */

static A jttraverse(J,A,AF);


B jtmeminit(J jt){I k,m=MLEN;
 k=1; DO(m, msize[i]=k; k+=k;);  /* OK to do this line in each thread */
 jt->tbase=-NTSTACK;
 jt->ttop = NTSTACK;
 jt->mmax =msize[m-1];
 DO(m, jt->mfree[i]=0; jt->mfreeb[i]=0; jt->mfreet[i]=1048576;);
 R 1;
}


F1(jtspcount){A z;I c=0,j,m=1+PLIML,*v;MS*x;
 ASSERTMTV(w);
 GA(z,INT,2*m,2,0); v=AV(z);
 DO(m, j=0; x=(MS*)(jt->mfree[i]); while(x){x=(MS*)(x->a); ++j;} if(j){++c; *v++=msize[i]; *v++=j;});
 v=AS(z); v[0]=c; v[1]=2; AN(z)=2*c;
 R z;
}    /* 7!:3 count of unused blocks */

static int __cdecl spfreecomp(const void *x,const void *y){R *(I*)x<*(I*)y?-1:1;}

B jtspfree(J jt){A t;I c,d,i,j,m,n,*u,*v;MS*x;
 m=0; u=5+jt->mfreet; v=5+jt->mfreeb;
 /* DO(1+PLIML, if(jt->mfreet[i]<=jt->mfreeb[i]){j=jt->mfreeb[i]/msize[i]; m=MAX(m,j);}); */
 if(*++u<=*++v){j=*v/  64; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/ 128; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/ 256; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/ 512; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/1024; m=MAX(m,j);}
 if(!m)R 1;
 GA(t,INT,1+m,1,0); v=AV(t);
 /* must not allocate memory after this point */
 for(i=6;i<=PLIML;++i){
  if(jt->mfreet[i]>jt->mfreeb[i])continue;
  n=0; x=(MS*)(jt->mfree[i]); 
  while(x){v[n++]=(I)x; x=(MS*)(x->a);}
  qsort(v,n,SZI,spfreecomp); 
  j=0; u=0; c=msize[i]; d=PSIZE/c;
  while(n>j){
   x=(MS*)v[j];
   if(MFHEAD&x->mflag&&n>=j+d&&PSIZE==c+v[j+d-1]-v[j]){
    j+=d; 
    FREE(x); 
    jt->mfreeb[i]-=PSIZE;
   }else{x->a=u; u=(I*)v[j]; ++j;}
  }
  jt->mfree[i]=u; jt->mfreet[i]=1048576+jt->mfreeb[i];
 }
 R 1;
}    /* free unused blocks */

static F1(jtspfor1){
 RZ(w);
 if(BOX&AT(w)){A*wv=AAV(w);I wd=(I)w*ARELATIVE(w); DO(AN(w), spfor1(WVR(i)););}
 else traverse(w,jtspfor1); 
 if(1e9>AC(w)||AFSMM&AFLAG(w))
  if(AFNJA&AFLAG(w)){I j,m,n,p;
   m=SZI*WP(AT(w),AN(w),AR(w)); 
   n=p=m+mhb; 
   j=6; n>>=j; 
   while(n){n>>=1; ++j;} 
   if(p==msize[j-1])--j;
   jt->spfor+=msize[j];
  }else jt->spfor+=msize[((MS*)w-1)->j];
 R mtm;
}

F1(jtspfor){A*wv,x,y,z;C*s;D*v,*zv;I i,m,n,wd;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GA(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); m=AN(x); s=CAV(x);
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vnm(m,s),EVILNAME);
  RZ(y=symbrd(nfs(m,s))); 
  *v=0.0; spfor1(y); zv[i]=*v;
 }
 R z;
}    /* 7!:5 space for named object; w is <'name' */

F1(jtspforloc){A*wv,x,y,z;C*s;D*v,*zv;I c,i,j,m,n,wd,*yv;L*u;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GA(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); m=AN(x); s=CAV(x);
  if(!m){m=4; s="base";}
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vlocnm(m,s),EVILNAME);
  y=stfind(0,m,s);
  ASSERT(y,EVLOCALE);
  *v=(D)msize[((MS*)y-1)->j];
  spfor1(LOCPATH(y)); spfor1(LOCNAME(y));
  m=AN(y); yv=AV(y); 
  for(j=1;j<m;++j){
   c=yv[j];
   while(c){*v+=sizeof(L); u=c+jt->sympv; spfor1(u->name); spfor1(u->val); c=u->next;}
  }
  zv[i]=*v;
 }
 R z;
}    /* 7!:6 space for a locale */


F1(jtmmaxq){ASSERTMTV(w); R sc(jt->mmax);}
     /* 9!:20 space limit query */

F1(jtmmaxs){I j,m=MLEN,n;
 RE(n=i0(vib(w)));
 ASSERT(1E5<=n,EVLIMIT);
 j=m-1; DO(m, if(n<=msize[i]){j=i; break;});
 jt->mmax=msize[j];
 R mtm;
}    /* 9!:21 space limit set */


void jtfr(J jt,A w){I j,n;MS*x;
 if(!w||--AC(w))R;
 x=(MS*)w-1; 
 j=x->j; n=msize[j];
 jt->bytes-=n;
 if(PLIML<j)FREE(x);  /* malloc-ed       */
 else{                /* pool allocation */
  x->a=jt->mfree[j]; 
  jt->mfree[j]=(I*)x; 
  jt->mfreeb[j]+=n;
}}

void jtfh(J jt,A w){fr(w);}

static A jtma(J jt,I m){A z;C*u;I j,n,p,*v;MS*x;
 n=p=m+mhb; 
 ASSERT(n<=jt->mmax,EVLIMIT);
 j=6; n>>=j; 
 while(n){n>>=1; ++j;} 
 if(p==msize[j-1])--j;
 n=msize[j];
 if(jt->mfree[j]){         /* allocate from free list         */
  z=(A)(mhw+jt->mfree[j]); 
  jt->mfree[j]=((MS*)(jt->mfree[j]))->a;
  jt->mfreeb[j]-=n;
 }else if(n>PLIM){         /* large block: straight malloc    */
  v=MALLOC(n);
  ASSERT(v,EVWSFULL); 
  z=(A)(v+mhw);
 }else{                    /* small block: do pool allocation */
  v=MALLOC(PSIZE);
  ASSERT(v,EVWSFULL);
  u=(C*)v; DO(PSIZE/n, x=(MS*)u; u+=n; x->a=(I*)u; x->j=(C)j; x->mflag=0;); x->a=0;
  ((MS*)v)->mflag=MFHEAD;
  z=(A)(mhw+v); 
  jt->mfree[j]=((MS*)v)->a;
  jt->mfreeb[j]+=PSIZE-n;
 } 
 JBREAK0;
 jt->bytes+=n; jt->bytesmax=MAX(jt->bytes,jt->bytesmax);
 x=(MS*)z-1; x->a=0; x->j=(C)j;
 R z;
}


static A jttraverse(J jt,A w,AF f){
 RZ(w);
 switch(AT(w)){
  case XD:
   {DX*v=(DX*)AV(w); DO(AN(w), CALL1(f,v->x,0L); ++v;);} break;
  case RAT:  
   {A*v=AAV(w); DO(2*AN(w), CALL1(f,*v++,0L););} break;
  case XNUM: case BOX:
   if(!(AFLAG(w)&AFNJA+AFSMM)){A*wv=AAV(w);I wd=(I)w*ARELATIVE(w); DO(AN(w), CALL1(f,WVR(i),0L););} break;
  case VERB: case ADV:  case CONJ: 
   {V*v=VAV(w); CALL1(f,v->f,0L); CALL1(f,v->g,0L); CALL1(f,v->h,0L);} break;
  case SYMB:
   {I k,*v=1+AV(w);L*u;
    CALL1(f,LOCPATH(w),0L);
    CALL1(f,LOCNAME(w),0L);
    DO(AN(w)-1, if(k=*v++){u=k+jt->sympv; CALL1(f,u->name,0L); CALL1(f,u->val,0L);});
   } break;
  case SB01: case SINT: case SFL: case SCMPX: case SLIT: case SBOX:
   {P*v=PAV(w); CALL1(f,SPA(v,a),0L); CALL1(f,SPA(v,e),0L); CALL1(f,SPA(v,i),0L); CALL1(f,SPA(v,x),0L);} break;
 }
 R mark;
}


static A jttg(J jt){A t=jt->tstacka,z;
 RZ(z=ma(SZI*WP(BOX,NTSTACK,1L)));
 AT(z)=BOX; AC(z)=AR(z)=1; AN(z)=*AS(z)=NTSTACK; AM(z)=NTSTACK*SZA; AK(z)=AKX(z);
 jt->tstacka=z; jt->tstack=AAV(jt->tstacka); jt->tbase+=NTSTACK; jt->ttop=1;
 *jt->tstack=t;
 R z;
}

static void jttf(J jt){A t=jt->tstacka;
 jt->tstacka=*jt->tstack; jt->tstack=AAV(jt->tstacka); jt->tbase-=NTSTACK; jt->ttop=NTSTACK;
 fr(t);
}

F1(jttpush){
 RZ(w);
 traverse(w,jttpush);
 if(jt->ttop>=NTSTACK)RZ(tg());
 jt->tstack[jt->ttop]=w;
 ++jt->ttop;
 R w;
}

I jttpop(J jt,I old){
 while(old<jt->tbase+jt->ttop)if(1<jt->ttop)fr(jt->tstack[--jt->ttop]); else tf(); 
 R old;
}

A jtgc (J jt,A w,I old){ra(w); tpop(old); R tpush(w);}

void jtgc3(J jt,A x,A y,A z,I old){
 if(x)ra(x);    if(y)ra(y);    if(z)ra(z);
 tpop(old);
 if(x)tpush(x); if(y)tpush(y); if(z)tpush(z);
}


F1(jtfa ){RZ(w); traverse(w,jtfa ); fr(w);   R mark;}
F1(jtra ){RZ(w); traverse(w,jtra ); ++AC(w); R w;   }

static F1(jtra1){RZ(w); traverse(w,jtra1); AC(w)+=jt->arg; R w;}
A jtraa(J jt,I k,A w){A z;I m=jt->arg; jt->arg=k; z=ra1(w); jt->arg=m; R z;}

F1(jtrat){R ra(tpush(w));}

A jtga(J jt,I t,I n,I r,I*s){A z;I m,w;
 if(t&BIT){const I c=8*SZI;              /* bit type: pad last axis to fullword */
  ASSERTSYS(1>=r||s,"ga bit array shape");
  if(1>=r)w=(n+c-1)/c; else RE(w=mult(prod(r-1,s),(s[r-1]+c-1)/c));
  w+=WP(INT,0L,r); m=SZI*w; 
  ASSERT(     n>=0&&m>w&&w>0,EVLIMIT);   /* beware integer overflow */
 }else{
  w=WP(t,n,r);     m=SZI*w; 
  ASSERT(m>n&&n>=0&&m>w&&w>0,EVLIMIT);   /* beware integer overflow */
 }
 RZ(z=ma(m));
 if(!(t&DIRECT))memset(z,C0,m);
 if(t&LAST0){I*v=(I*)z+w-2; *v++=0; *v=0;}
 AC(z)=1; AN(z)=n; AR(z)=r; AFLAG(z)=0; AK(z)=AKX(z); AM(z)=msize[((MS*)z-1)->j]-(AK(z)+sizeof(MS)); 
 AT(z)=0; tpush(z); AT(z)=t;
 if(1==r&&!(t&SPARSE))*AS(z)=n; else if(r&&s)ICPY(AS(z),s,r);  /* 1==n always if t&SPARSE */
 R z;
}

A jtgah(J jt,I r,A w){A z;
 ASSERT(RMAX>=r,EVLIMIT); 
 RZ(z=ma(SZI*(AH+r)));
 AT(z)=0; ++AC(z); tpush(z);
 if(w){
  AFLAG(z)=0; AM(z)=AM(w); AT(z)=AT(w); AN(z)=AN(w); AR(z)=r; AK(z)=CAV(w)-(C*)z;
  if(1==r)*AS(z)=AN(w);
 }
 R z;
}    /* allocate header */ 

// clone w, returning the address of the cloned area
F1(jtca){A z;I t;P*wp,*zp;
 RZ(w);
 t=AT(w);
 GA(z,t,AN(w),AR(w),AS(w)); if(AFLAG(w)&AFNJA+AFSMM+AFREL)AFLAG(z)=AFREL;
 if(t&SPARSE){
  wp=PAV(w); zp=PAV(z);
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,ca(SPA(wp,x)));
 }else MC(AV(z),AV(w),AN(w)*bp(t)+(t&NAME?sizeof(NM):0)); 
 R z;
}

F1(jtcar){A*u,*wv,z;I n,wd;P*p;V*v;
 RZ(z=ca(w));
 n=AN(w);
 switch(AT(w)){
  case RAT:  n+=n;
  case XNUM:
  case BOX:  u=AAV(z); wv=AAV(w); wd=(I)w*ARELATIVE(w); DO(n, RZ(*u++=car(WVR(i)));); break;
  case SB01: case SLIT: case SINT: case SFL: case SCMPX: case SBOX:
   p=PAV(z); 
   SPB(p,a,car(SPA(p,a)));
   SPB(p,e,car(SPA(p,e)));
   SPB(p,i,car(SPA(p,i)));
   SPB(p,x,car(SPA(p,x)));
   break;
  case VERB: case ADV: case CONJ: 
   v=VAV(z); 
   if(v->f)RZ(v->f=car(v->f)); 
   if(v->g)RZ(v->g=car(v->g)); 
   if(v->h)RZ(v->h=car(v->h));
 }
 R z;
}

B jtspc(J jt){A z; RZ(z=MALLOC(1000)); FREE(z); R 1; }

A jtext(J jt,B b,A w){A z;I c,k,m,m1,t;
 RZ(w);                               /* assume AR(w)&&AN(w)    */
 m=*AS(w); c=AN(w)/m; t=AT(w); k=c*bp(t);
 GA(z,t,2*AN(w),AR(w),AS(w)); 
 MC(AV(z),AV(w),m*k);                 /* copy old contents      */
 if(b){ra(z); fa(w);}                 /* 1=b iff w is permanent */
 *AS(z)=m1=AM(z)/k; AN(z)=m1*c;       /* "optimal" use of space */
 if(!(t&DIRECT))memset(CAV(z)+m*k,C0,k*(m1-m));
 R z;
}

A jtexta(J jt,I t,I r,I c,I m){A z;I k,m1; 
 GA(z,t,m*c,r,0); 
 k=bp(t); *AS(z)=m1=AM(z)/(c*k); AN(z)=m1*c;
 if(2==r)*(1+AS(z))=c;
 if(!(t&DIRECT))memset(AV(z),C0,k*AN(z));
 R z;
}    /* "optimal" allocation for type t rank r, c atoms per item, >=m items */


/* debugging tools  */

B jtcheckmf(J jt){C c;I i,j;MS*x,*y;
 for(j=0;j<=PLIML;++j){
  i=0; y=0; x=(MS*)(jt->mfree[j]); /* head ptr for j-th pool */
  while(x){
   ++i; c=x->mflag;
   if(!(j==x->j)){
    ASSERTSYS(0,"checkmf 0");
   }
   if(!(!c||c==MFHEAD)){
    ASSERTSYS(0,"checkmf 1");
   }
   y=x; x=(MS*)x->a;
 }}
 R 1;
}    /* traverse free list */

B jtchecksi(J jt){DC d;I dt;
 d=jt->sitop;
 while(d&&!(DCCALL==d->dctype&&d->dcj)){
  dt=d->dctype;
  if(!(dt==DCPARSE||dt==DCSCRIPT||dt==DCCALL||dt==DCJUNK)){
   ASSERTSYS(0,"checksi 0");
  }
  if(!(d!=d->dclnk)){
   ASSERTSYS(0,"checksi 1");
  }
  d=d->dclnk;
 }
 R 1;
}    /* traverse stack per jt->sitop */