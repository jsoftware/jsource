/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Suffix and Outfix                                              */

#include "j.h"
#include "vasm.h"
#include "ve.h"


#define SUFFIXPFX(f,Tz,Tx,pfx)  \
 AHDRS(f,Tz,Tx){I i;Tz v,*y;                                        \
  x+=m*c; z+=m*c;                                              \
  if(c==n)DO(m, *--z=v=    *--x; DO(n-1, --x; --z; *z=v=pfx(*x,v);))  \
  else{I d=c/n; for(i=0;i<m;++i){                                              \
   y=z; DO(d, *--z=    *--x;);                                        \
   DO(n-1, DO(d, --x; --y; --z; *z=pfx(*x,*y);));                     \
 }}}

#define SUFFIXNAN(f,Tz,Tx,pfx)  \
 AHDRS(f,Tz,Tx){I i;Tz v,*y;                                        \
  NAN0;                                                               \
  x+=m*c; z+=m*c;                                              \
  if(c==n)DO(m, *--z=v=    *--x; DO(n-1, --x; --z; *z=v=pfx(*x,v);))  \
  else{I d=c/n; for(i=0;i<m;++i){                                              \
   y=z; DO(d, *--z=    *--x;);                                        \
   DO(n-1, DO(d, --x; --y; --z; *z=pfx(*x,*y);));                     \
  }}                                                                   \
  NAN1V;                                                              \
 }

#define SUFFICPFX(f,Tz,Tx,pfx)  \
 AHDRS(f,Tz,Tx){I i;Tz v,*y;                                        \
  x+=m*c; z+=m*c;                                              \
  if(c==n)DO(m, *--z=v=(Tz)*--x; DO(n-1, --x; --z; *z=v=pfx(*x,v);))  \
  else{I d=c/n; for(i=0;i<m;++i){                                              \
   y=z; DO(d, *--z=(Tz)*--x;);                                        \
   DO(n-1, DO(d, --x; --y; --z; *z=pfx(*x,*y);));                     \
 }}}

#define SUFFIXOVF(f,Tz,Tx,fs1,fvv)  \
 AHDRS(f,I,I){C er=0;I i,*xx,*y,*zz;                      \
  xx=x+=m*c; zz=z+=m*c;                              \
  if(c==n){                                                 \
   if(1==n)DO(m, *--z=*--x;)                                \
   else    DO(m, z=zz-=c; x=xx-=c; fs1(n,z,x); RER;)        \
  }else{I d=c/n; for(i=0;i<m;++i){                                   \
   DO(d, *--zz=*--xx;);                                     \
   DO(n-1, x=xx-=d; y=zz; z=zz-=d; fvv(d,z,x,y); RER;);     \
 }}}

#if SY_ALIGN
#define SUFFIXBFXLOOP(T,pfx)  \
 {T*xx=(T*)x,*yy,*zz=(T*)z;   \
  q=d/sizeof(T);              \
  DO(m, yy=zz; DO(q, *--zz=*--xx;); DO(n-1, DO(q, --xx; --yy; --zz; *zz=pfx(*xx,*yy);)));  \
 }
  
#define SUFFIXBFX(f,pfx,ipfx,spfx,bpfx,vexp)  \
 AHDRP(f,B,B){B v,*y;I d,q;                                        \
  d=c/n; x+=m*c; z+=m*c;                                           \
  if(1==d){DO(m, *--z=v=*--x; DO(n-1, --x; --z; *z=v=vexp;)); R;}  \
  if(0==d%sizeof(UI  )){SUFFIXBFXLOOP(UI,   pfx); R;}              \
  if(0==d%sizeof(UINT)){SUFFIXBFXLOOP(UINT,ipfx); R;}              \
  if(0==d%sizeof(US  )){SUFFIXBFXLOOP(US,  spfx); R;}              \
  DO(m, y=z; DO(d, *--z=*--x;); DO(n-1, DO(d, --x; --y; --z; *z=bpfx(*x,*y);)));  \
 }
#else
#define SUFFIXBFX(f,pfx,ipfx,spfx,bpfx,vexp)  \
 AHDRS(f,B,B){B v;I d,i,q,r,t,*xi,*yi,*zi;                         \
  d=c/n; x+=m*c; z+=m*c;                                           \
  if(1==d){DO(m, *--z=v=*--x; DO(n-1, --x; --z; *z=v=vexp;)); R;}  \
  q=d/SZI; r=d%SZI; xi=(I*)x; zi=(I*)z;                            \
  if(0==r)for(i=0;i<m;++i){                                        \
   yi=zi; DO(q, *--zi=*--xi;);                                     \
   DO(n-1, DO(q, --xi; --yi; --zi; *zi=pfx(*xi,*yi);));            \
  }else for(i=0;i<m;++i){                                          \
   yi=zi; DO(q, *--zi=*--xi;);                                     \
   x=(B*)xi; z=(B*)zi; DO(r, *--z=*--x;); xi=(I*)x; zi=(I*)z;      \
   DO(n-1, DO(q, --xi; --yi; --zi; *zi=pfx(*xi,*yi););             \
    xi=(I*)((B*)xi-r);                                             \
    yi=(I*)((B*)yi-r);                                             \
    zi=(I*)((B*)zi-r); t=pfx(*xi,*yi); MC(zi,&t,r););              \
 }}
#endif

SUFFIXBFX(   orsfxB, OR,  IOR,  SOR,  BOR,  *x||v   ) 
SUFFIXBFX(  andsfxB, AND, IAND, SAND, BAND, *x&&v   )
SUFFIXBFX(   eqsfxB, EQ,  IEQ,  SEQ,  BEQ,  *x==v   )
SUFFIXBFX(   nesfxB, NE,  INE,  SNE,  BNE,  *x!=v   )
SUFFIXBFX(   ltsfxB, LT,  ILT,  SLT,  BLT,  *x< v   )
SUFFIXBFX(   lesfxB, LE,  ILE,  SLE,  BLE,  *x<=v   )
SUFFIXBFX(   gtsfxB, GT,  IGT,  SGT,  BGT,  *x> v   )
SUFFIXBFX(   gesfxB, GE,  IGE,  SGE,  BGE,  *x>=v   )
SUFFIXBFX(  norsfxB, NOR, INOR, SNOR, BNOR, !(*x||v))
SUFFIXBFX( nandsfxB, NAND,INAND,SNAND,BNAND,!(*x&&v))

SUFFIXOVF( plussfxI, I, I,  PLUSS, PLUSVV)
SUFFIXOVF(minussfxI, I, I, MINUSS,MINUSVV)
SUFFIXOVF(tymessfxI, I, I, TYMESS,TYMESVV)

SUFFICPFX( plussfxO, D, I, PLUS  )
SUFFICPFX(minussfxO, D, I, MINUS )
SUFFICPFX(tymessfxO, D, I, TYMES )

SUFFIXPFX( plussfxB, I, B, PLUS  )
SUFFIXNAN( plussfxD, D, D, PLUS  )
SUFFIXNAN( plussfxZ, Z, Z, zplus )
SUFFIXPFX( plussfxX, X, X, xplus )
SUFFIXPFX( plussfxQ, Q, Q, qplus )

SUFFIXPFX(minussfxB, I, B, MINUS )
SUFFIXNAN(minussfxD, D, D, MINUS )
SUFFIXNAN(minussfxZ, Z, Z, zminus)

SUFFIXPFX(tymessfxD, D, D, TYMES )
SUFFIXPFX(tymessfxZ, Z, Z, ztymes)
SUFFIXPFX(tymessfxX, X, X, xtymes)
SUFFIXPFX(tymessfxQ, Q, Q, qtymes)

SUFFIXNAN(  divsfxD, D, D, DIV   )
SUFFIXNAN(  divsfxZ, Z, Z, zdiv  )

SUFFIXPFX(  maxsfxI, I, I, MAX   )
SUFFIXPFX(  maxsfxD, D, D, MAX   )
SUFFIXPFX(  maxsfxX, X, X, XMAX  )
SUFFIXPFX(  maxsfxQ, Q, Q, QMAX  )
SUFFIXPFX(  maxsfxS, SB,SB,SBMAX )

SUFFIXPFX(  minsfxI, I, I, MIN   )
SUFFIXPFX(  minsfxD, D, D, MIN   )
SUFFIXPFX(  minsfxX, X, X, XMIN  )
SUFFIXPFX(  minsfxQ, Q, Q, QMIN  )
SUFFIXPFX(  minsfxS, SB,SB,SBMIN )

SUFFIXPFX(bw0000sfxI, UI,UI, BW0000)
SUFFIXPFX(bw0001sfxI, UI,UI, BW0001)
SUFFIXPFX(bw0010sfxI, UI,UI, BW0010)
SUFFIXPFX(bw0011sfxI, UI,UI, BW0011)
SUFFIXPFX(bw0100sfxI, UI,UI, BW0100)
SUFFIXPFX(bw0101sfxI, UI,UI, BW0101)
SUFFIXPFX(bw0110sfxI, UI,UI, BW0110)
SUFFIXPFX(bw0111sfxI, UI,UI, BW0111)
SUFFIXPFX(bw1000sfxI, UI,UI, BW1000)
SUFFIXPFX(bw1001sfxI, UI,UI, BW1001)
SUFFIXPFX(bw1010sfxI, UI,UI, BW1010)
SUFFIXPFX(bw1011sfxI, UI,UI, BW1011)
SUFFIXPFX(bw1100sfxI, UI,UI, BW1100)
SUFFIXPFX(bw1101sfxI, UI,UI, BW1101)
SUFFIXPFX(bw1110sfxI, UI,UI, BW1110)
SUFFIXPFX(bw1111sfxI, UI,UI, BW1111)


static DF1(jtsuffix){DECLF;I r;
 RZ(w);
 if(jt->rank&&jt->rank[1]<AR(w)){r=jt->rank[1]; jt->rank=0; R rank1ex(w,self,r,jtsuffix);}
 jt->rank=0;
 R eachl(IX(IC(w)),w,atop(fs,ds(CDROP)));
}    /* f\."r w for general f */

static DF1(jtgsuffix){A h,*hv,z,*zv;I m,n,r;
 RZ(w);
 if(jt->rank&&jt->rank[1]<AR(w)){r=jt->rank[1]; jt->rank=0; R rank1ex(w,self,jt->rank[1],jtgsuffix);}
 jt->rank=0;
 n=IC(w); 
 h=VAV(self)->h; hv=AAV(h); m=AN(h);
 GATV(z,BOX,n,1,0); zv=AAV(z);
 DO(n, RZ(zv[i]=df1(drop(sc(i),w),hv[i%m])););
 R ope(z);
}    /* g\."r w for gerund g */

#define SSGULOOP(T)  \
 {T*v=(T*)zv;                      \
  for(i=0;i<n1;++i){               \
   RZ(q=CALL2(f2,x,y,fs)); RZ(TYPESEQ(t,AT(q))&&!AR(q)); \
   *v--=*(T*)AV(q);                \
   AK(x)-=k; AK(y)-=k; tpop(old);  \
 }}

static DF1(jtssgu){A fs,q,x,y,z;AF f2;C*zv;I i,k,m,n1,old,r,t;V*sv=VAV(self);
 fs=VAV(sv->f)->f; f2=VAV(fs)->f2;
 r=AR(w)-1; n1=IC(w)-1; m=aii(w); t=AT(w); k=m*bp(t);
 RZ(z=ca(w)); zv=CAV(z)+k*n1;
 RZ(q=tail(w));
 RZ(y=gah(r,q)); ICPY(AS(y),AS(q),r); AK(y)=(I)zv-(I)y; zv-=k; 
 RZ(x=gah(r,q)); ICPY(AS(x),AS(q),r); AK(x)=(I)zv-(I)x;
 old=jt->tnextpushx;
 switch(r?0:k){
  case sizeof(C): SSGULOOP(C); break;
  case sizeof(I): SSGULOOP(I); break;
#if SY_64
  case sizeof(int): SSGULOOP(int); break;
#endif
  case sizeof(S): SSGULOOP(S); break;
#if ! SY_64
  case sizeof(D): SSGULOOP(D); break;
#endif
  case sizeof(Z): SSGULOOP(Z); break;
  default:
   for(i=0;i<n1;++i){       
    RZ(q=CALL2(f2,x,y,fs)); RZ(TYPESEQ(t,AT(q))&&r==AR(q)&&!ICMP(AS(y),AS(q),r)); 
    MC(zv,CAV(q),k); zv-=k; 
    AK(x)-=k; AK(y)-=k; 
    tpop(old);
 }}
 R z;
}    /* same as ssg but for uniform function f */

static DF1(jtssg){A fs,q,y,z,*zv;AF f2;B p;C*u,*v;I i,k,n,yn,yr,*ys,yt;V*sv=VAV(self);
 if(jt->rank&&jt->rank[1]<AR(w))R rank1ex(w,self,jt->rank[1],jtssg);
 jt->rank=0; 
 fs=VAV(sv->f)->f; f2=VAV(fs)->f2;
 n=IC(w); p=ARELATIVE(w);
 if(DIRECT&AT(w)){RE(z=ssgu(w,self)); if(z)R z;}
 GATV(z,BOX,n,1,0); zv=n+AAV(z); 
 RZ(*--zv=q=tail(w)); yt=AT(q); yn=AN(q); yr=AR(q); ys=1+AS(w);
 k=yn*bp(yt); v=CAV(w)+k*(n-1);
 for(i=1;i<n;++i){
  v-=k;
  GA(y,yt,yn,yr,ys); u=CAV(y); 
  if(p){A1*wv=(A1*)v,*yv=(A1*)u;I d=(I)w-(I)y; AFLAG(y)=AFREL; DO(yn, yv[i]=d+wv[i];);}else MC(u,v,k);
  RZ(*--zv=q=CALL2(f2,y,q,fs));
 }
 R ope(z);
}    /* f/\."r w for general f and 1<(-r){$w and -.0 e.$w */

A jtscansp(J jt,A w,A self,AF sf){A e,ee,x,z;B*b;I f,m,j,r,t,rv[2],wr;P*wp,*zp;
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 wp=PAV(w); e=SPA(wp,e); RZ(ee=over(e,e));
 if(!equ(ee,CALL1(sf,ee,self))){
  RZ(x=denseit(w));
  rv[1]=r; jt->rank=rv; RZ(z=CALL1(sf,x,self)); jt->rank=0; 
  R z;
 }else{
  RZ(b=bfi(wr,SPA(wp,a),1));
  if(r&&b[f]){b[f]=0; RZ(w=reaxis(ifb(wr,b),w));}
  j=f; m=0; DO(wr-f, m+=!b[j++];);
 }
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x);
 rv[1]=m; jt->rank=rv; RZ(x=CALL1(sf,x,self)); jt->rank=0;
 t=maxtype(AT(e),AT(x)); RZ(e=cvt(t,e)); if(TYPESNE(t,AT(x)))RZ(x=cvt(t,x));
 GA(z,STYPE(t),1,wr+!m,AS(w)); if(!m)*(wr+AS(z))=1;
 zp=PAV(z); 
 SPB(zp,e,e); 
 SPB(zp,x,x); 
 SPB(zp,i,ca(SPA(wp,i))); 
 SPB(zp,a,ca(SPA(wp,a)));
 R z;
}    /* f/\"r or f/\."r on sparse w */

static DF1(jtsscan){A y,z;C id;I c,cv,f,m,n,r,rr[2],t,wn,wr,*ws,wt,zt;VF ado;
 RZ(w);
 wt=AT(w);
 if(SPARSE&wt)R scansp(w,self,jtsscan);
 wn=AN(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; ws=AS(w); 
 PROD(m,f,ws); PROD(c,r,f+ws); n=r?ws[f]:1;  // will not be used if WN==0, so PROD ok
 y=VAV(self)->f; id=vaid(VAV(y)->f); 
 if(2>n||!wn){if(id){jt->rank=0; R r?ca(w):reshape(over(shape(w),one),w);}else R suffix(w,self);}
 vasfx(id,wt,&ado,&cv);
 if(!ado)R ssg(w,self);
 if((t=atype(cv))&&TYPESNE(t,wt))RZ(w=cvt(t,w));
 zt=rtype(cv); jt->rank=0;
 GA(z,zt,wn,wr,ws);
 ado(jt,m,c,n,AV(z),AV(w));
 if(jt->jerr)R jt->jerr>=EWOV?(rr[1]=r,jt->rank=rr,sscan(w,self)):0; else R cv&VRI+VRD?cvz(cv,z):z;
}    /* f/\."r w main control */


static F2(jtomask){A c,r,x,y;I m,n,p;
 RZ(a&&w);
 RE(m=i0(a)); p=ABS(m); n=IC(w);
 r=sc(0>m?(n+p-1)/p:MAX(0,1+n-m)); c=tally(w);
 x=reshape(sc(p),  zero);
 y=reshape(0>m?c:r,one );
 R reshape(over(r,c),over(x,y));
}

static DF2(jtgoutfix){A h,*hv,x,z,*zv;I m,n;
 RZ(x=omask(a,w));
 n=IC(x);
 h=VAV(self)->h; hv=AAV(h); m=AN(h);
 GATV(z,BOX,n,1,0); zv=AAV(z);
 DO(n, RZ(zv[i]=df1(repeat(from(sc(i),x),w),hv[i%m])););
 R ope(z);
}

static AS2(jtoutfix, eachl(omask(a,w),w,atop(fs,ds(CPOUND))),0117)

static DF2(jtofxinv){A f,fs,z;C c;I t;V*v;
 F2RANK(0,RMAX,jtofxinv,self);
 fs=VAV(self)->f; f=VAV(fs)->f; v=VAV(f); c=v->id; t=AT(w);
 if(!(c==CPLUS||c==CBDOT&&t&INT||(c==CEQ||c==CNE)&&t&B01))R outfix(a,w,self);
 z=irs2(df1(w,fs),df2(a,w,bslash(fs)),VFLAGNONE, RMAX,-1L,c==CPLUS?(AF)jtminus:v->f2);
 if(jt->jerr==EVNAN){RESETERR; R outfix(a,w,self);}else R z;
}    /* a f/\. w where f has an "inverse" */

static DF2(jtofxassoc){A f,i,j,p,s,x,z;C id,*zv;I c,cv,d,k,kc,m,r,t;V*v;VF ado;
 F2RANK(0,RMAX,jtofxassoc,self);
 m=IC(w); RE(k=i0(a)); c=ABS(k);
 f=VAV(self)->f; x=VAV(f)->f; v=VAV(x); id=CBDOT==v->id?(C)*AV(v->f):v->id;
 if(k==IMIN||m<=c||id==CSTARDOT&&!(B01&AT(w)))R outfix(a,w,self);
 if(-1<=k){d=m-c;     RZ(i=apv(d,0L, 1L)); RZ(j=apv(d,c,1L));}
 else     {d=(m-1)/c; RZ(i=apv(d,c-1,c )); RZ(j=apv(d,c,c ));}
 RZ(p=from(i,df1(w,bslash(f)))); 
 RZ(s=from(j,df1(w,bsdot(f))));
 r=AR(p); c=aii(p); t=AT(p); k=bp(t); kc=k*c;
 RZ(var(id,p,p,t,t,&ado,&cv)); 
 ASSERTSYS(ado,"ofxassoc");
 GA(z,t,c*(1+d),r,AS(p)); *AS(z)=1+d; zv=CAV(z);
 MC(zv,     AV(s),          kc);                     /* {.s           */
 if(1<d)ado(jt,1,c*(d-1),1L,zv+kc,AV(p),kc+CAV(s));  /* (}:p) f (}.s) */
 MC(zv+kc*d,CAV(p)+kc*(d-1),kc);                     /* {:p           */
 if(jt->jerr>=EWOV){RESETERR; R ofxassoc(a,cvt(FL,w),self);}else R z;
}    /* a f/\. w where f is an atomic associative fn */

static DF1(jtiota1rev){R apv(IC(w),IC(w),-1L);}

F1(jtbsdot){A f;AF f1=jtsuffix,f2=jtoutfix;C id;V*v;
 RZ(w);
 if(NOUN&AT(w))R fdef(CBSLASH,VERB, jtgsuffix,jtgoutfix, w,0L,fxeachv(1L,w), VGERL|VAV(ds(CBSLASH))->flag, RMAX,0L,RMAX);
 v=VAV(w);
 switch(v->id){
  case CPOUND: f1=jtiota1rev; break;
  case CSLASH:
   f1=jtsscan; 
   f=v->f; id=ID(f); if(id==CBDOT){f=VAV(f)->f; if(INT&AT(f)&&!AR(f))id=(C)*AV(f);}
   switch(id){
    case CPLUS:   case CEQ:     case CNE:     case CBW0110:  case CBW1001:               
     f2=jtofxinv;   break;
    case CSTAR:   case CMAX:    case CMIN:    case CPLUSDOT: case CSTARDOT: 
    case CBW0000: case CBW0001: case CBW0011: case CBW0101:  case CBW0111: case CBW1111: 
     f2=jtofxassoc;
 }}
 R ADERIV(CBSDOT,f1,f2,VAV(ds(CBSDOT))->flag,RMAX,0,RMAX);
}
