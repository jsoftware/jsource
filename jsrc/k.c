/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conversions Amongst Internal Types                                      */

#include "j.h"
#include "vcomp.h"

#define KF1(f)          B f(J jt,A w,void*yv)
#define CVCASE(a,b)     (((a)<<3)+(b))   // The main cases fit in low 8 bits of mask

#if 0  // for bit types
#define TOBIT(T,AS)   {T*v=(T*)wv,x; \
  for(i=0;i<m;++i){                                                       \
   DO(q, k=0; DO(BB, if(x=*v++){if(AS)k|=bit[i]; else R 0;}); *zv++=k;);  \
   if(r){k=0; DO(r,  if(x=*v++){if(AS)k|=bit[i]; else R 0;}); *zv++=k;}   \
   DQ(r1, *zv++=0;);                                                      \
  }} 

static KF1(jtcvt2bit){I c,i,m,q,r,r1,wr,*ws,*wv;UC k,*zv=(UC*)yv;
 wv=AV(w); wr=AR(w); ws=AS(w); 
 c=wr?ws[wr-1]:1; m=c?AN(w)/c:0; q=c/BB; r=c%BB; r1=c%BW?(BW-c%BW)/BB:0;
 switch(CTTZNOFLAG(AT(w))){
  default:  R 0;
  case B01X: TOBIT(B, 1         ); break;
  case INTX: TOBIT(I, 1==x      ); break;
  case FLX:  TOBIT(D, FEQ(1.0,x)); break;
 }
 R 1;
}
#endif

static KF1(jtC1fromC2){UC*x;US c,*v;
 v=USAV(w); x=(C*)yv;
 DQ(AN(w), c=*v++; if(!(256>c))R 0; *x++=(UC)c;);
 R 1;
}

static KF1(jtC2fromC1){UC*v;US*x;
 v=UAV(w); x=(US*)yv;
 DQ(AN(w), *x++=*v++;);
 R 1;
}

static KF1(jtC1fromC4){UC*x;C4 c,*v;
 v=C4AV(w); x=(C*)yv;
 DQ(AN(w), c=*v++; if(!(256>c))R 0; *x++=(UC)c;);
 R 1;
}

static KF1(jtC2fromC4){US*x;C4 c,*v;
 v=C4AV(w); x=(US*)yv;
 DQ(AN(w), c=*v++; if(!(65536>c))R 0; *x++=(US)c;);
 R 1;
}

static KF1(jtC4fromC1){UC*v;C4*x;
 v=UAV(w); x=(C4*)yv;
 DQ(AN(w), *x++=*v++;);
 R 1;
}

static KF1(jtC4fromC2){US*v;C4*x;
 v=USAV(w); x=(C4*)yv;
 DQ(AN(w), *x++=*v++;);
 R 1;
}

static KF1(jtBfromI){B*x;I n,p,*v;
 n=AN(w); v=AV(w); x=(B*)yv;
 DQ(n, p=*v++; *x++=(B)p; if(p&-2)R 0;);
 R 1;
}

static KF1(jtBfromD){B*x;D p,*v;I n;
 n=AN(w); v=DAV(w); x=(B*)yv;
 DQ(n, p=*v++; if(p<-2||2<p)R 0;   // handle infinities
// obsolete   if(!p)*x++=0; else if(FIEQ(p,1.0))*x++=1; else R 0;);
  I val=2; val=(p==0)?0:val; val=FIEQ(p,1.0)?1:val; if(val==2)R 0; *x++=(B)val; )
 R 1;
}

static KF1(jtIfromD){D p,q,*v;I i,k=0,n,*x;
 n=AN(w); v=DAV(w); x=(I*)yv;
#if SY_64
#if 1
 for(i=0;i<n;++i){
  p=v[i]; q=jround(p); I rq=(I)q;
  if(!(p==q || FIEQ(p,q)))R 0;  // must equal int, possibly out of range
  // out-of-range values don't convert, handle separately
  if(p<(D)IMIN){if(!(p>=IMIN*(1+jt->fuzz)))R 0; rq=IMIN;}  // if tolerantly < IMIN, error; else take IMIN
  else if(p>=-(D)IMIN){if(!(p<=IMAX*(1+jt->fuzz)))R 0; rq=IMAX;}  // if tolerantly > IMAX, error; else take IMAX
  *x++=rq;
 }
#else // obsolete
 q=IMIN*(1+jt->fuzz); D r=IMAX*(1+jt->fuzz);
 DO(n, p=v[i]; if(p<q||r<p)R 0;);
 for(i=0;i<n;++i){
  p=v[i]; q=jfloor(p);
  if         (FEQ(p,q)){k=(I)q; *x++=SGN(k)==SGN(q)?k:0>q?IMIN:IMAX;}
  else if(++q,FEQ(p,q)){k=(I)q; *x++=SGN(k)==SGN(q)?k:0>q?IMIN:IMAX;}
  else R 0;
 }
#endif
#else
 q=IMIN*(1+jt->fuzz); D r=IMAX*(1+jt->fuzz);
 DO(n, p=v[i]; if(p<q||r<p)R 0;);
 for(i=0;i<n;++i){
  p=v[i]; q=jfloor(p);
  if(FIEQ(p,q))*x++=(I)q; else if(FIEQ(p,1+q))*x++=(I)(1+q); else R 0;
 }
#endif
 R 1;
}

static KF1(jtDfromZ){D d,*x;I n;Z*v;
 n=AN(w); v=ZAV(w); x=(D*)yv;
 if(jt->fuzz)DQ(n, d=ABS(v->im); if(d!=inf&&d<=jt->fuzz*ABS(v->re)){*x++=v->re; v++;} else R 0;)
 else        DQ(n, d=    v->im ; if(!d                            ){*x++=v->re; v++;} else R 0;);
 R 1;
}

static KF1(jtXfromB){B*v;I n,u[1];X*x;
 n=AN(w); v=BAV(w); x=(X*)yv;
 DO(n, *u=v[i]; x[i]=rifvsdebug(vec(INT,1L,u)););           
 R !jt->jerr;
}

static KF1(jtXfromI){B b;I c,d,i,j,n,r,u[XIDIG],*v;X*x;
 n=AN(w); v=AV(w); x=(X*)yv;
 for(i=0;i<n;++i){
  c=v[i]; b=c==IMIN; d=b?-(1+c):ABS(c); j=0;
  DO(XIDIG, u[i]=r=d%XBASE; d=d/XBASE; if(r)j=i;);
  ++j; *u+=b;
  if(0>c)DO(XIDIG, u[i]=-u[i];);
  x[i]=rifvsdebug(vec(INT,j,u));
 }
 R !jt->jerr;
}

static X jtxd1(J jt,D p){PROLOG(0052);A t;D d,e=tfloor(p),q,r;I m,*u;
 switch(jt->xmode){
  case XMFLR:   p=e;                            break;
  case XMCEIL:  p=jceil(p);                      break;
  case XMEXACT: ASSERT(TEQ(p,e),EVDOMAIN); p=e; break;
  case XMEXMT:  if(!TEQ(p,e))R vec(INT,0L,&m);
 }
 if(p== inf)R vci(XPINF);
 if(p==-inf)R vci(XNINF);
 GAT0(t,INT,30,1); u=AV(t); m=0; d=ABS(p); 
 while(0<d){
  q=jfloor(d/XBASE); r=d-q*XBASE; u[m++]=(I)r; d=q;
  if(m==AN(t)){RZ(t=ext(0,t)); u=AV(t);}
 }
 if(!m){u[0]=0; ++m;}else if(0>p)DO(m, u[i]=-u[i];);
 A z=xstd(vec(INT,m,u));
 EPILOG(z);
}

static KF1(jtXfromD){D*v=DAV(w);X*x=(X*)yv; DO(AN(w), x[i]=rifvsdebug(xd1(v[i]));); R !jt->jerr;}

static KF1(jtBfromX){A q;B*x;I e;X*v;
 v=XAV(w); x=(B*)yv;
 DO(AN(w), q=v[i]; e=*AV(q); if((AN(q)^1)|(e&-2))R 0; x[i]=(B)e;);
 R 1;
}

static KF1(jtIfromX){I a,i,m,n,*u,*x;X c,p,q,*v;
 v=XAV(w); x=(I*)yv; n=AN(w);
 if(!(p=xc(IMAX)))R 0; if(!(q=xminus(negate(p),xc(1L))))R 0;
 for(i=0;i<n;++i){
  c=v[i]; if(!(1!=xcompare(q,c)&&1!=xcompare(c,p)))R 0;
  m=AN(c); u=AV(c)+m-1; a=0; DO(m, a=*u--+a*XBASE;); x[i]=a;
 }
 R 1;
}

static KF1(jtDfromX){D d,*x=(D*)yv,dm,dp;I c,i,n,*v,wn;X p,*wv;
 dp=1.7976931348623157e308; dm=-dp;
 wn=AN(w); wv=XAV(w);
 for(i=0;i<wn;++i){
  p=wv[i]; n=AN(p); v=AV(p)+n-1; c=*v;
  if     (c==XPINF)d=inf; 
  else if(c==XNINF)d=infm; 
  else{
   d=0.0; DQ(n, d=*v--+d*XBASE;);
  }
  x[i]=d;
 } 
 R 1;
}

static KF1(jtQfromX){X*v=XAV(w),*x=(X*)yv; DQ(AN(w), *x++=*v++; *x++=iv1;); R 1;}

static KF1(jtQfromD){B neg,recip;D c,d,t,*wv;I e,i,n,*v;Q q,*x;S*tv;
 if(!(w))R 0;
 n=AN(w); wv=DAV(w); x=(Q*)yv; tv=3*liln+(S*)&t;
 for(i=0;i<n;++i){
  t=wv[i]; 
  ASSERT(!_isnan(t),EVNAN);
  if(neg=0>t)t=-t; q.d=iv1;
  if     (t==inf)q.n=vci(XPINF);
  else if(t==0.0)q.n=iv0;
  else if(1.1102230246251565e-16<t&&t<9.007199254740992e15){
   d=jround(1/dgcd(1.0,t)); c=jround(d*t); 
   q.n=xd1(c); q.d=xd1(d); q=qstd(q);
  }else{
   if(recip=1>t)t=1.0/t;
   e=(I)(0xfff0&*tv); e>>=4; e-=1023;
   if(recip){q.d=xtymes(xd1(t/pow(2.0,e-53.0)),xpow(xc(2L),xc(e-53))); q.n=ca(iv1);}
   else     {q.n=xtymes(xd1(t/pow(2.0,e-53.0)),xpow(xc(2L),xc(e-53))); q.d=ca(iv1);}
  }
  q.n=rifvsdebug(q.n); q.d=rifvsdebug(q.d);
  if(neg){v=AV(q.n); DQ(AN(q.n), *v=-*v; ++v;);}
  *x++=q;
 }
 R !jt->jerr;
}

static KF1(jtDfromQ){D d,f,n,*x,xb=(D)XBASE;I cn,i,k,m,nn,pn,qn,r,*v,wn;Q*wv;X c,p,q,x2=0;
 wn=AN(w); wv=QAV(w); x=(D*)yv; nn=308/XBASEN;
 for(i=0;i<wn;++i){
  p=wv[i].n; pn=AN(p); k=1==pn?*AV(p):0;
  q=wv[i].d; qn=AN(q);
  if     (k==XPINF)x[i]=inf;
  else if(k==XNINF)x[i]=infm;
  else if(pn<=nn&&qn<=nn){
   n=0.0; f=1.0; v=AV(p); DO(pn, n+=f*v[i]; f*=xb;);
   d=0.0; f=1.0; v=AV(q); DO(qn, d+=f*v[i]; f*=xb;);
   x[i]=n/d;
  }else{
   k=5+qn; if(!x2)if(!(x2=xc(2L)))R 0;
   if(!(c=xdiv(take(sc(-(k+pn)),p),q,XMFLR)))R 0;
   cn=AN(c); m=MIN(cn,5); r=cn-(m+k); v=AV(c)+cn-m; 
   n=0.0; f=1.0; DO(m, n+=f*v[i]; f*=xb;);
   d=1.0; DQ(ABS(r), d*=xb;);
   x[i]=0>r?n/d:n*d;
 }}
 R 1;
}

static KF1(jtXfromQ){Q*v;X*x;
 v=QAV(w); x=(X*)yv;
 DQ(AN(w), if(!(equ(iv1,v->d)))R 0; *x++=v->n; ++v;);           
 R !jt->jerr;
}

// Imaginary parts have already been cleared
static KF1(jtZfromD){
 D *wv=DAV(w); Z *zv=yv; DQ(AN(w), zv++->re=*wv++;) R 1;
}

static B jtDXfI(J jt,I p,A w,DX*x){B b;I e,c,d,i,j,n,r,u[XIDIG],*v;
 n=AN(w); v=AV(w);
 for(i=0;i<n;++i){
  c=v[i]; b=c==IMIN; d=b?-(1+c):ABS(c); j=0;
  DO(XIDIG, u[i]=r=d%XBASE; d/=XBASE; if(r)j=i;);
  ++j; *u+=b;
  e=XBASEN*(j-1); d=u[j-1]; while(d){++e; d/=10;} 
  if(0>c)DO(j, u[i]=-u[i];);
  x[i].e=e; x[i].p=p; x[i].x=vec(INT,j,u);;
 }
 R !jt->jerr;
}

/*
static B jtDXfI(J jt,I p,A w,DX*x){A y;I b,c,d,dd,e,i,m,n,q,r,*wv,*yv;
 n=AN(w); wv=AV(w); m=(p+XBASEN-1)/XBASEN;
 for(i=0;i<n;++i){
  c=wv[i]; d=dd=c==IMIN?-(1+c):ABS(c); 
  if(d){e=0; while(d){++e; r=d%10; d=d/10;}}else e=1;
  GATV0(y,INT,m,1); yv=AV(y);
  r=p%XBASEN; q=!!r+((e-r)+XBASEN-1)/XBASEN; 
  if(d=(e-r)%XBASEN){b=1; DQ(XBASEN, b*=10; --d; if(!d)break;);}else b=XBASE;
  DQ(m-q, *yv++=0;);
  d=dd/b; r=dd%b; r+=c==IMIN; r*=XBASE/b; 
  if(0>c){*yv++=-r; DQ(q-1, r=d%XBASE; d=d/XBASE; *yv++=-r;);}
  else   {*yv++= r; DQ(q-1, r=d%XBASE; d=d/XBASE; *yv++= r;);}
  x[i].e=e-1; x[i].p=p; x[i].x=y;
 }
 R !jt->jerr;
} */    /* most significant digit last, decimal point before last digit */

// Convert the data in w to the type t.  A new buffer is always created (with a
// copy of the data if w is already of the right type), and returned in *y.  Result is
// 0 if error, 1 if success.  If the conversion loses precision, error is returned
B jtccvt(J jt,I tflagged,A w,A*y){A d;I n,r,*s,wt; void *wv,*yv;I t=tflagged&~NOUNCVTVALIDCT;
 if(!(w))R 0;
 r=AR(w); s=AS(w);
 // Handle sparse
 switch((t&SPARSE?2:0)+(AT(w)&SPARSE?1:0)){I t1;P*wp,*yp;
  case 1: RZ(w=denseit(w)); break;  // sparse to dense
  case 2: RZ(*y=sparseit(cvt(DTYPE(t),w),IX(r),cvt(DTYPE(t),num[0]))); R 1;  // dense to sparse; convert type first (even if same dtype)
  case 3: // sparse to sparse
   t1=DTYPE(t);
   GASPARSE(*y,t,1,r,s); yp=PAV(*y); wp=PAV(w);
   SPB(yp,a,ca(SPA(wp,a)));
   SPB(yp,i,ca(SPA(wp,i)));
   SPB(yp,e,cvt(t1,SPA(wp,e)));
   SPB(yp,x,cvt(t1,SPA(wp,x)));
   R 1;
 }
 // Now known to be non-sparse
 n=AN(w); wt=AT(w);
 // If type is already correct, return a clone - should not occur
 if(TYPESEQ(t,wt)){RZ(*y=ca(w)); R 1;}
 // else if(n&&t&JCHAR){ASSERT(HOMO(t,wt),EVDOMAIN); RZ(*y=uco1(w)); R 1;}
 // Kludge on behalf of result assembly: we want to be able to stop converting after the valid cells.  If NOUNCVTVALIDCT is set in the type,
 // we use the input *y as as override on the # cells to convert.  We use it to replace n (for use here) and yv, and AK(w) and AN(w) for the subroutines.
 // If NOUNCVTVALIDCT is set, w is modified: the caller must restore AN(w) and AK(w) if it needs it
 // TODO: same-length conversion could be done in place
 GA(d,t,n,r,s); yv=voidAV(d);  // allocate the same # atoms, even if we will convert fewer
 if(tflagged&NOUNCVTVALIDCT){
  I inputn=*(I*)y;  // fetch input, in case it is called for
  if(inputn>0){  // if converting the leading values, just update the counts
   n=inputn;  // set the counts for local use, and in the block to be converted
  }else{  // if converting trailing values...
   I offset=(n+inputn)<<bplg(t);  // byte offset to start of data
   AK(w)+=(n+inputn)<<bplg(wt); yv=(I*)((C*)yv+((n+inputn)<<bplg(t)));  // advance input and output pointers to new area
   n=-inputn;  // get positive # atoms to convert
  }
  AN(w)=n;  // change atomct of w to # atoms to convert
 }
 // If n and AN have been modified, it doesn't matter for rank-1 arguments whether the shape of the result is listed as n or s[0] since only n atoms will
 // be used.  For higher ranks, we need the shape from s.  So it's just as well that we take the shape from s now
 *y=d;  wv=voidAV(w); // return the address of the new block
 if(t&CMPX)fillv(t,n,(C*)yv);   // why??  just fill in imaginary parts as we need to
 if(!n)R 1;
 // Perform the conversion based on data types
 // For branch-table efficiency, we split the C2T and C4T and BIT conversions into one block, and
 // the rest in another
 if ((t|wt)&(C2T+C4T+BIT+SBT+XD+XZ)) {   // there are no SBT+XD+XZ conversions, but we have to show domain error
   // we must account for all NOUN types.  Low 8 bits have most of them, and we know type can't be sparse.  This picks up the others
  ASSERT(!((t|wt)&(SBT+XD+XZ)),EVDOMAIN);  // No conversions for these types
  switch (CVCASE(CTTZ(t),CTTZ(wt))){
   case CVCASE(LITX, C2TX): R C1fromC2(w, yv);
   case CVCASE(LITX, C4TX): R C1fromC4(w, yv);
   case CVCASE(C2TX, LITX): R C2fromC1(w, yv);
   case CVCASE(C2TX, C4TX): R C2fromC4(w, yv);
   case CVCASE(C4TX, LITX): R C4fromC1(w, yv);
   case CVCASE(C4TX, C2TX): R C4fromC2(w, yv);
#if 0  // bit types
   case CVCASE(BITX, B01X): R cvt2bit(w, yv);
   case CVCASE(BITX, INTX): R cvt2bit(w, yv);
   case CVCASE(BITX, FLX): R cvt2bit(w, yv);
   case CVCASE(BITX, CMPXX): GATV(d, FL, n, r, s); if(!(DfromZ(w, AV(d))))R 0; R cvt2bit(d, yv);
#endif
   default:                ASSERT(0, EVDOMAIN);
  }
 }
 switch (CVCASE(CTTZ(t),CTTZ(wt))){
  case CVCASE(INTX, B01X): {I*x = yv; B*v = (B*)wv; DQ(n, *x++ = *v++;); } R 1;
  case CVCASE(XNUMX, B01X): R XfromB(w, yv);
  case CVCASE(RATX, B01X): GATV(d, XNUM, n, r, s); R XfromB(w, AV(d)) && QfromX(d, yv);
  case CVCASE(FLX, B01X): {D*x = (D*)yv; B*v = (B*)wv; DQ(n, *x++ = *v++;); } R 1;
  case CVCASE(CMPXX, B01X): {Z*x = (Z*)yv; B*v = (B*)wv; DQ(n, x++->re = *v++;); } R 1;
  case CVCASE(B01X, INTX): R BfromI(w, yv);
  case CVCASE(XNUMX, INTX): R XfromI(w, yv);
  case CVCASE(RATX, INTX): GATV(d, XNUM, n, r, s); R XfromI(w, AV(d)) && QfromX(d, yv);
  case CVCASE(FLX, INTX): {D*x = (D*)yv; I*v = wv; DQ(n, *x++ = (D)*v++;); } R 1;
  case CVCASE(CMPXX, INTX): {Z*x = (Z*)yv; I*v = wv; DQ(n, x++->re = (D)*v++;); } R 1;
  case CVCASE(B01X, FLX): R BfromD(w, yv);
  case CVCASE(INTX, FLX): R IfromD(w, yv);
  case CVCASE(XNUMX, FLX): R XfromD(w, yv);
  case CVCASE(RATX, FLX): R QfromD(w, yv);
  case CVCASE(CMPXX, FLX): R ZfromD(w, yv);
  case CVCASE(B01X, CMPXX): GATV(d, FL, n, r, s); if(!(DfromZ(w, AV(d))))R 0; R BfromD(d, yv);
  case CVCASE(INTX, CMPXX): GATV(d, FL, n, r, s); if(!(DfromZ(w, AV(d))))R 0; R IfromD(d, yv);
  case CVCASE(XNUMX, CMPXX): GATV(d, FL, n, r, s); if(!(DfromZ(w, AV(d))))R 0; R XfromD(d, yv);
  case CVCASE(RATX, CMPXX): GATV(d, FL, n, r, s); if(!(DfromZ(w, AV(d))))R 0; R QfromD(d, yv);
  case CVCASE(FLX, CMPXX): R DfromZ(w, yv);
  case CVCASE(B01X, XNUMX): R BfromX(w, yv);
  case CVCASE(INTX, XNUMX): R IfromX(w, yv);
  case CVCASE(RATX, XNUMX): R QfromX(w, yv);
  case CVCASE(FLX, XNUMX): R DfromX(w, yv);
  case CVCASE(CMPXX, XNUMX): GATV(d, FL, n, r, s); if(!(DfromX(w, AV(d))))R 0; R ZfromD(d, yv);
  case CVCASE(B01X, RATX): GATV(d, XNUM, n, r, s); if(!(XfromQ(w, AV(d))))R 0; R BfromX(d, yv);
  case CVCASE(INTX, RATX): GATV(d, XNUM, n, r, s); if(!(XfromQ(w, AV(d))))R 0; R IfromX(d, yv);
  case CVCASE(XNUMX, RATX): R XfromQ(w, yv);
  case CVCASE(FLX, RATX): R DfromQ(w, yv);
  case CVCASE(CMPXX, RATX): GATV(d, FL, n, r, s); if(!(DfromQ(w, AV(d))))R 0; R ZfromD(d, yv);
  default:                ASSERT(0, EVDOMAIN);
 }
}

// clear rank before calling ccvt - needed for sparse arrays only but returns the block as the result
A jtcvt(J jt,I t,A w){A y;B b; 
 RANK2T oqr=jt->ranks; RESETRANK; b=ccvt(t,w,&y); jt->ranks=oqr;
 ASSERT(b,EVDOMAIN);
 R y;
}

// Convert numeric type to lowest precision that fits.  Push fuzz/rank onto a stack,
// and use 'exact' and 'no rank' for them.  If mode=0, do not promote XNUM/RAT to fixed-length types.
// If mode bit 1 is set, minimum precision is INT; if mode bit 2 is set, minimum precision is FL
// Result is a new buffer, always
A jtbcvt(J jt,C mode,A w){FPREFIP; A y,z=w;D ofuzz;
 RZ(w);
 ofuzz=jt->fuzz; RANK2T oqr=jt->ranks;  // save status for comparison, and ranks in case sparse needs them (should let sparse do the saving)
 jt->fuzz=0;     RESETRANK;
 // for rationals, try converting to XNUM; otherwise stay at rational
 if(RAT&AT(w))z=ccvt(XNUM,w,&y)?y:w;
#ifdef NANFLAG
 // there may be values (especially b types) that were nominally CMPX but might actually be integers.  Those were
 // stored with the real part being the actual integer value and the imaginary part as the special 'flag' value.  We
 // handle those here.  If all the imaginary parts were flags, we accept all the integer parts and change the type
 // to integer.  If none of the imaginary parts were flags, we leave the input unchanged.  If some were flags, we
 // convert the flagged values to float and keep the result as complex
 if((((AN(w)-1)|(AT(w)&CMPX)-1))>=0){  // not empty AND complex
  I allflag=1, anyflag=0; Z *wv = ZAV(w); DO(AN(w), I isflag=*(I*)&wv[i].im==NANFLAG; allflag&=isflag; anyflag|=isflag;)
  if(anyflag){
   I ipok=(-(I)jtinplace&JTINPLACEW) & AC(w);  // both sign bits set (<0) if inplaceable
   if(allflag){
    if(ipok>=0)GATV(z,INT,AN(w),AR(w),AS(w));
    I *zv=IAV(z);  // output area
    DO(AN(w), zv[i]=*(I*)&wv[i].re;)  // copy the results as integers
   }else{
    if(ipok>=0)GATV(z,CMPX,AN(w),AR(w),AS(w));
    Z *zv=ZAV(z);  // output area
    DO(AN(w), if(*(I*)&wv[i].im==NANFLAG){zv[i].re=(D)*(I*)&wv[i].re; zv[i].im=0.0;}else{zv[i]=wv[i];})  // copy floats, and converts any integers back to float
   }
   w=z;  // this result is now eligible for further demotion
  }
 }
#endif
 // for all numerics, try Boolean/int/float in order, stopping when we find one that holds the data
 if(mode&1||!(AT(w)&XNUM+RAT)){  // if we are not stopping at XNUM/RAT
  z=!(mode&6)&&ccvt(B01,w,&y)?y:!(mode&4)&&ccvt(INT,w,&y)?y:ccvt(FL,w,&y)?y:w;  // convert to enabled modes one by one, stopping when one works
 }
 jt->fuzz=ofuzz; jt->ranks=oqr;
 RNE(z);
}    /* convert to lowest type. 0=mode: don't convert XNUM/RAT to other types */

F1(jticvt){A z;D*v,x;I i,n,*u;
 RZ(w);
 n=AN(w); v=DAV(w);
 GATV(z,INT,n,AR(w),AS(w)); u=AV(z);
 for(i=0;i<n;++i){
  x=*v++; if(x<IMIN||IMAX<x)R w;  // if conversion will fail, skip it
#if 0 && SY_64  // obsolete
  k=(I)x; *u++=SGN(k)==SGN(x)?k:0>x?IMIN:IMAX;
#else
  *u++=(I)x;
#endif
 }
 R z;
}

A jtpcvt(J jt,I t,A w){A y;B b;RANK2T oqr=jt->ranks;
 RESETRANK; b=ccvt(t,w,&y); jt->ranks=oqr;
 R b?y:w;
}    /* convert w to type t, if possible, otherwise just return w */

#if !C_CRC32C
F1(jtcvt0){I n,t,*u,*v,z0,z1;
 RZ(w);
 t=AT(w); n=AN(w); 
 if(n&&t&FL+CMPX){
  if(t&CMPX)n+=n; u=AV(w); v=(I*)minus0; z0=z1=*v;
#if SY_64
             DQ(n, if(z0==*u            )*u=0;        ++u; ); 
#else
  z1=*(1+v); DQ(n, if(z0==u[0]&&z1==u[1])u[0]=u[1]=0; u+=2;);
#endif
 }
 R w;
}    /* convert -0 to 0 in place */
#endif


A jtxcvt(J jt,I m,A w){A z;I old=jt->xmode; jt->xmode=m; z=cvt(XNUM,w); jt->xmode=old; R z;}

F1(jtxco1){RZ(w); ASSERT(AT(w)&DENSE,EVNONCE); R cvt(AT(w)&B01+INT+XNUM?XNUM:RAT,w);}

F2(jtxco2){A z;B b;I j,n,r,*s,t,*wv,*zu,*zv;
 RZ(a&&w);
 n=AN(w); r=AR(w); t=AT(w);
 ASSERT(t&DENSE,EVNONCE);
 RE(j=i0(a));
 switch(j){
  case -2: R aslash1(CDIV,w);
  case -1: b=jt->xco; jt->xco=1; z=bcvt(1,w); jt->xco=b; R z;
  case  1: R xco1(w);
  case  2: 
   if(!(t&RAT))RZ(w=cvt(RAT,w));
   GATV(z,XNUM,2*n,r+1,AS(w)); AS(z)[r]=2;
   MC(AV(z),AV(w),2*n*SZI);
   R z;
  case  3:
   ASSERT(t&XD+XZ,EVDOMAIN);
   b=(~t>>XDX)&1;   // b=NOT XD
// obsolete    GATV0(z,INT,b?n:2*n,b?r:1+r); s=AS(z); if(!b)*s++=2; MCISH(s,AS(w),r);
   GATV0(z,INT,n<<b,r+b); s=AS(z); if(b)*s++ =2; MCISH(s,AS(w),r);
   zv=AV(z); zu=n+zv; wv=AV(w);
   if(t&XD){DX*v=(DX*)wv;   DQ(n,         *zv++=v->p;);}
   else    {ZX*v=(ZX*)wv,y; DQ(n, y=*v++; *zv++=y.re.p; *zu++=y.im.p;);}
   R z;
  default:
   ASSERT(20<=j,EVDOMAIN);
   GA(z,t&CMPX?XZ:XD,n,r,AS(w));
   if(t&INT){RZ(DXfI(j,w,(DX*)AV(z))); R z;}
   ASSERT(0,EVNONCE);
}}
