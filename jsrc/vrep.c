/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: a#"r w                                                           */

#include "j.h"


#define REPF(f)         A f(J jt,A a,A w,I wf,I wcr)


static REPF(jtrepzdx){A p,q,x;P*wp;
 RZ(a&&w);
 if(SPARSE&AT(w)){wp=PAV(w); x=SPA(wp,e);}
 else x=jt->fill&&AN(jt->fill)?jt->fill:filler(w);
 RZ(p=repeat(ravel(rect(a)),ravel(stitch(IX(wcr?*(wf+AS(w)):1),num[-1]))));
 RZ(q=irs2(w,x,0L,wcr,0L,jtover));
 R irs2(p,q,0L,1L,wcr+!wcr,jtfrom);
}    /* (dense complex) # (dense or sparse) */

static REPF(jtrepzsx){A q,x,y;I c,d,j,k=-1,m,p=0,*qv,*xv,*yv;P*ap;
 RZ(a&&w);
 ap=PAV(a); x=SPA(ap,x); m=AN(x);
 if(!AN(SPA(ap,a)))R repzdx(ravel(x),w,wf,wcr);
 y=SPA(ap,i); yv=AV(y);
 RZ(x=cvt(INT,vec(FL,2*m,AV(x)))); xv=AV(x);
 if(equ(num[0],SPA(ap,e))){
  k=c=*(wf+AS(w));
  if(!wf&&SPARSE&AT(w)){A a,y;I m,n,q,*v;P*wp;
   wp=PAV(w); a=SPA(wp,a);
   if(AN(a)&&!*AV(a)){
    y=SPA(wp,i); v=AS(y); m=v[0]; n=v[1]; v=AV(y);
    k=m?v[(m-1)*n]+1:0; q=0; 
    DO(m, if(q==*v)++q; else if(q<*v){k=q; break;} v+=n;);
  }}
  ASSERT(k<=IMAX-1,EVLIMIT);
  if(c==k)RZ(w=irs2(sc(1+k),w,0L,0L,wcr,jttake));
  DO(2*m, ASSERT(0<=xv[i],EVDOMAIN); p+=xv[i]; ASSERT(0<=p,EVLIMIT););
  GATV0(q,INT,p,1); qv=AV(q);
  DO(m, c=*xv++; d=*xv++; j=yv[i]; DO(c, *qv++=j;); DO(d, *qv++=k;);); 
  R irs2(q,w,0L,1L,wcr,jtfrom);
 }
 ASSERT(0,EVNONCE);
}    /* (sparse complex) #"r (dense or sparse) */


#define REPB(T)  \
 {T*u,*v=(T*)zv;                                                    \
  for(i=0;i<c;++i){                                                 \
   u=i*m+(T*)wv;                                                    \
   for(j=0,iv=(I*)b;j<q;++j,u+=SZI)switch(*iv++){                   \
    case B0001:                                  *v++=u[3]; break;  \
    case B0010:                       *v++=u[2];            break;  \
    case B0011:                       *v++=u[2]; *v++=u[3]; break;  \
    case B0100:            *v++=u[1];                       break;  \
    case B0101:            *v++=u[1];            *v++=u[3]; break;  \
    case B0110:            *v++=u[1]; *v++=u[2];            break;  \
    case B0111:            *v++=u[1]; *v++=u[2]; *v++=u[3]; break;  \
    case B1000: *v++=u[0];                                  break;  \
    case B1001: *v++=u[0];                       *v++=u[3]; break;  \
    case B1010: *v++=u[0];            *v++=u[2];            break;  \
    case B1011: *v++=u[0];            *v++=u[2]; *v++=u[3]; break;  \
    case B1100: *v++=u[0]; *v++=u[1];                       break;  \
    case B1101: *v++=u[0]; *v++=u[1];            *v++=u[3]; break;  \
    case B1110: *v++=u[0]; *v++=u[1]; *v++=u[2];            break;  \
    case B1111: *v++=u[0]; *v++=u[1]; *v++=u[2]; *v++=u[3];         \
   }                                                                \
   if(r){B*c=(B*)iv; DO(r, if(c[i])*v++=u[i];);}                      \
 }}

#if 0 && !SY_64 && SY_WIN32  // obsolete - bit-oriented version is better
static REPF(jtrepbdx){A z;B*b;C*wv,*zv;I c,i,*iv,j,k,m,p,q,r,zn;
 RZ(a&&w);
 if(SPARSE&AT(w))R irs2(ifb(AN(a),BAV(a)),w,0L,1L,wcr,jtfrom);
 m=AN(a); q=m>>LGSZI; r=m&(SZI-1);
 ASSERT(m==*(wf+AS(w)),EVLENGTH);
 b=BAV(a); p=bsum(m,b); zn=m?p*(AN(w)/m):0; 
 ASSERT(0<=zn,EVLIMIT);
 GA(z,AT(w),zn,AR(w),AS(w)); *(wf+AS(z))=p;
 wv=CAV(w); zv=CAV(z);
 RE(c=prod(wf,AS(w)));
 if(zn)switch(k=(AN(w)/(c*m))<<bplg(AT(w)),FL&AT(w)||k!=sizeof(D)?k:0){
  case sizeof(C): REPB(C); break;
  case sizeof(S): REPB(S); break;
#if SY_64
  case sizeof(int): REPB(int); break;
#endif
  case sizeof(I): REPB(I); break;
  case sizeof(D): REPB(D); break;
  default: {C*u;I k1=k,k2=k*2,k3=k*3,k4=k*4,km=k*m;                                                     
  for(i=0;i<c;++i){
   u=i*km+wv;                                                      
   for(j=0,iv=(I*)b;j<q;++j,u+=k4)switch(*iv++){                                  
    case B0001: MC(zv,k3+u,k1); zv+=k1;                         break;
    case B0010: MC(zv,k2+u,k1); zv+=k1;                         break;
    case B0011: MC(zv,k2+u,k2); zv+=k2;                         break;
    case B0100: MC(zv,k1+u,k1); zv+=k1;                         break;
    case B0101: MC(zv,k1+u,k1); zv+=k1; MC(zv,k3+u,k1); zv+=k1; break;
    case B0110: MC(zv,k1+u,k2); zv+=k2;                         break;
    case B0111: MC(zv,k1+u,k3); zv+=k3;                         break;
    case B1000: MC(zv,   u,k1); zv+=k1;                         break;
    case B1001: MC(zv,   u,k1); zv+=k1; MC(zv,k3+u,k1); zv+=k1; break;
    case B1010: MC(zv,   u,k1); zv+=k1; MC(zv,k2+u,k1); zv+=k1; break;
    case B1011: MC(zv,   u,k1); zv+=k1; MC(zv,k2+u,k2); zv+=k2; break;
    case B1100: MC(zv,   u,k2); zv+=k2;                         break;
    case B1101: MC(zv,   u,k2); zv+=k2; MC(zv,k3+u,k1); zv+=k1; break;
    case B1110: MC(zv,   u,k3); zv+=k3;                         break;
    case B1111: MC(zv,   u,k4); zv+=k4;
   }
   if(r){B*c=(B*)iv; DO(r, if(c[i]){MC(zv,u+i*k,k); zv+=k;});}
 }}}
 R z;
}    /* (dense boolean)#"r (dense or sparse) */
#else
static REPF(jtrepbdx){A z;I c,k,m,p;
 // wf and wcr are set
 RZ(a&&w);F2PREFIP;
 if(SPARSE&AT(w))R irs2(ifb(AN(a),BAV(a)),w,0L,1L,wcr,jtfrom);
 m=AN(a);
 void *zvv; void *wvv=voidAV(w); I n=0; // pointer to output area; pointer to input data; number of prefix bytes to skip in first cell
 p=bsum(m,BAV(a));  // p=# 1s in result, i. e. length of result item axis
 PROD(c,wf,AS(w)); PROD(k,wcr-1,AS(w)+wf+1); // c=#cells, k=#atoms per item of cell
 I zn=c*k*p;  // zn=#atoms in result
 k<<=bplg(AT(w));   // k is now # bytes/cell
 // if the result is inplaceable, AND it is not getting much shorter, keep the same result area
 // We retain the old block as long as the new one is at least half as big, without looking at total size of the allocation,
 // This could result in a very small block's remaining in a large allocation after repeated trimming.  We will accept the risk.
 // Accept only DIRECT blocks so we don't have to worry about explicitly freeing uncopied cells
 if(!(((I)jtinplace&(((UI)(m-2*p))>>((BW-1)-JTINPLACEWX))) && ASGNINPLACE(w) && AT(w)&DIRECT)) {
  // normal non-in-place copy
    // no overflow possible unless a is empty; nothing  moved then, and zn is 0
  GA(z,AT(w),zn,AR(w),0); MCISH(AS(z),AS(w),AR(w)) // allocate result
  zvv=voidAV(z);  // point to the output area
 }else{
  z=w; // inplace
  if(m==p)R z;  // if all the bits are 1, we can return very quickly.  It's rare, but so cheap to test for.
  AN(z)=zn;  // Install the correct atom count
  // see how many leading values of the result are already in position.  We don't need to copy them in the first cell
  UI *avv=IAV(a); for(;n<(m>>LGSZI);++n)if(avv[n]!=VALIDBOOLEAN)break;
  // now n has the number of words to skip.  Convert that to bytes, and advance wvv to point to the first cell that may move
  n<<=LGSZI; wvv=(C*)wvv+k*n;
  zvv=CAV(z)+k*n;   // step the output pointer over the initial items left in place
 }
 AS(z)[wf]=p;  // move in length of item axis, #bytes per item of cell
 if(!zn)R z;  // If no atoms to process, return empty

// original  DO(c, DO(m, if(b[i]){MC(zv,wv,k); zv+=k;} wv+=k;);); break;

 
 while(--c>=0){
  // at top of loop n is biased by the number of leading bytes to skip. wvv points to the first byte to process
  UI *avv=(UI*)(CAV(a)+n); n=m-n+((m&(SZI-1))?SZI:0); UI bits=*avv++;  // prime the pipeline for top of loop
  while(n>0){    // where we load bits SZI at a time
   // skip empty words, to get best speed on near-zero a.  This exits with the first unskipped word in bits
   while(bits==0 && n>=(2*SZI)){bits=*avv++; n-=SZI; wvv=(C*)wvv+(k<<LGSZI);}  // fast-forward over zeros.  Always leave 1 word so we have a batch to process
   I batchsize=n>>LGSZI; batchsize=MIN(BB,batchsize);
   UI bitstack=0; while(--batchsize>0){I bits2=*avv++; PACKBITSINTO(bits,bitstack); bits=bits2;};  // keep read pipe ahead
   // Handle the last word of the batch.  It might have non-Boolean data at the end, AFTER the Boolean padding.  Just clear the non-boolean part in this line
   bits&=VALIDBOOLEAN; PACKBITSINTO(bits,bitstack);
   // Now handle the last batch, by discarding garbage bits at the end and then shifting the lead bit down to bit 0
   if(n>=BW+SZI)bits=*avv++;else {n-=n&(SZI-1)?SZI:0; bitstack<<=(BW-n)&(SZI-1); bitstack>>=BW-n;}  // discard invalid trailing bits; shift leading byte to position 0.  For non-last batches, start on next batch
   switch(k){  // copy the words
   case sizeof(C): while(bitstack){I bitx=CTTZI(bitstack); *(C*)zvv=((C*)wvv)[bitx]; zvv=(C*)zvv+k; bitstack&=bitstack-1;} break;
   case sizeof(US): while(bitstack){I bitx=CTTZI(bitstack); *(US*)zvv=((US*)wvv)[bitx]; zvv=(C*)zvv+k; bitstack&=bitstack-1;} break;
#if BW==64
   case sizeof(UI4): while(bitstack){I bitx=CTTZI(bitstack); *(UI4*)zvv=((UI4*)wvv)[bitx]; zvv=(C*)zvv+k; bitstack&=bitstack-1;} break;
#endif
   case sizeof(UI): while(bitstack){I bitx=CTTZI(bitstack); *(UI*)zvv=((UI*)wvv)[bitx]; zvv=(C*)zvv+k; bitstack&=bitstack-1;} break;
   default: while(bitstack){I bitx=CTTZI(bitstack); MC(zvv,(C*)wvv+k*bitx,k); zvv=(C*)zvv+k; bitstack&=bitstack-1;} break;
   }

   wvv=(C*)wvv+(k<<LGBW);  // advance base to next batch of 64
   n-=BW;  // decr count left
  }
  wvv=(C*)wvv+(k*n);  // in case we loop back, back wvv to start of next input area, taking away the part of the last BW section we didn't use
  n=0;  // no bias for cells after the first
 } 

 R z;
}    /* (dense boolean)#"r (dense or sparse) */
#endif

static REPF(jtrepbsx){A ai,c,d,e,g,q,x,wa,wx,wy,y,y1,z,zy;B*b;I*dv,*gv,j,m,n,*u,*v,*v0;P*ap,*wp,*zp;
 RZ(a&&w);
 ap=PAV(a); e=SPA(ap,e); 
 y=SPA(ap,i); u=AV(y);
 x=SPA(ap,x); n=AN(x); b=BAV(x);
 if(!AN(SPA(ap,a)))R irs2(ifb(n,b),w,0L,1L,wcr,jtfrom);
 if(!*BAV(e)){
  GATV0(q,INT,n,1); v=v0=AV(q); 
  DO(n, if(*b++)*v++=u[i];); 
  AN(q)=*AS(q)=v-v0; 
  R irs2(q,w,0L,1L,wcr,jtfrom);
 }
 wp=PAV(w);
 if(DENSE&AT(w)||all0(eq(sc(wf),SPA(wp,a)))){RZ(q=denseit(a)); R irs2(ifb(AN(q),BAV(q)),w,0L,1L,wcr,jtfrom);}  // here if dense w
 wa=SPA(wp,a); wy=SPA(wp,i); wx=SPA(wp,x);
 RZ(q=aslash(CPLUS,a));
 GASPARSE(z,AT(w),1,AR(w),AS(w)); *(wf+AS(z))=m=*AV(q);
 RZ(c=indexof(wa,sc(wf)));
 RZ(y1=fromr(c,wy));
 RZ(q=not(eps(y1,ravel(repeat(not(x),y)))));
 m=*AS(a)-m;
 GATV0(ai,INT,m,1); v=AV(ai); DO(n, if(!*b++)*v++=u[i];);
 RZ(g=grade1(over(ai,repeat(q,y1)))); gv=AV(g);
 GATV0(d,INT,AN(y1),1); dv=AV(d); j=0; DO(AN(g), if(m>gv[i])++j; else dv[gv[i]-m]=j;);
 RZ(zy=repeat(q,wy)); v=AV(zy)+*AV(c); m=*(1+AS(zy)); DO(*AS(zy), *v-=dv[i]; v+=m;);
 zp=PAV(z);
 SPB(zp,a,ca(wa));
 SPB(zp,e,SPA(wp,e));
 SPB(zp,i,zy);
 SPB(zp,x,repeat(q,wx));
 R z;
}    /* (sparse boolean) #"r (dense or sparse) */

static REPF(jtrepidx){A y;I j,m,p=0,*v,*x;
 RZ(a&&w);
 RZ(a=vi(a)); x=AV(a);
 m=*AS(a);
 DO(m, ASSERT(0<=x[i],EVDOMAIN); p+=x[i]; ASSERT(0<=p,EVLIMIT););
 GATV0(y,INT,p,1); v=AV(y); 
 DO(m, j=i; DO(x[j], *v++=j;););
 R irs2(y,w,0L,1L,wcr,jtfrom);
}    /* (dense  integer) #"r (dense or sparse) */

static REPF(jtrepisx){A e,q,x,y;I c,j,m,p=0,*qv,*xv,*yv;P*ap;
 RZ(a&&w);
 ap=PAV(a); e=SPA(ap,e); 
 y=SPA(ap,i); yv=AV(y);
 x=SPA(ap,x); if(!(INT&AT(x)))RZ(x=cvt(INT,x)); xv=AV(x);
 if(!AN(SPA(ap,a)))R repidx(ravel(x),w,wf,wcr);
 if(!*AV(e)){
  m=AN(x);  
  DO(m, ASSERT(0<=xv[i],EVDOMAIN); p+=xv[i]; ASSERT(0<=p,EVLIMIT););
  GATV0(q,INT,p,1); qv=AV(q); 
  DO(m, c=xv[i]; j=yv[i]; DO(c, *qv++=j;);); 
  R irs2(q,w,0L,1L,wcr,jtfrom);
 }
 ASSERT(0,EVNONCE);
}    /* (sparse integer) #"r (dense or sparse) */


static REPF(jtrep1d){A z;C*wv,*zv;I c,k,m,n,p=0,q,t,*ws,zk,zn;
 RZ(a&&w);
 t=AT(a); m=AN(a); ws=AS(w); n=wcr?ws[wf]:1;  // n=length of item axis in input.  If aton, is repeated to length of a
 if(t&CMPX){
  if(wcr)R repzdx(from(apv(n,0L,0L),a),w,                wf,wcr);
  else   R repzdx(a,irs2(apv(m,0L,0L),w,0L,1L,0L,jtfrom),wf,1L ); 
 }
 if(t&B01){p=bsum(m,BAV(a)); // bsum in case a is big.  Atomic boolean was handled earlier
 }else{I*x; 
  RZ(a=vi(a)); x=AV(a); 
  DO(m, ASSERT(0<=x[i],EVDOMAIN); p+=x[i]; ASSERT(0<=p,EVLIMIT););  // p=#items in result
 }
 RE(q=mult(p,n));  // q=length of result item  axis.  +/a copies, each of length n
 RE(zn=mult(p,AN(w)));
 GA(z,AT(w),zn,AR(w)+!wcr,0); MCISH(AS(z),AS(w),AR(z)) AS(z)[wf]=q;
 if(!zn)R z;
 wv=CAV(w); zv=CAV(z);
 PROD(c,wf+(I )(wcr!=0),ws); PROD1(k,wcr-1,ws+wf+1); k <<=bplg(AT(w));  // c=#cell-items to process  k=#atoms per cell-item
 zk=p*k;  // # bytes to fill per item
 DO(c, mvc(zk,zv,k,wv); zv+=zk; wv+=k;);
 R z;
}    /* scalar #"r dense   or   dense #"0 dense */

static B jtrep1sa(J jt,A a,I*c,I*d){A x;B b;I*v;
 b=1&&AT(a)&CMPX;
 if(b)RZ(x=rect(a)) else x=a; 
 if(AR(a)){ASSERT(equ(num[1],aslash(CSTARDOT,le(zeroionei[0],ravel(x)))),EVDOMAIN); RZ(x=aslash(CPLUS,x));} 
 if(!(INT&AT(x)))RZ(x=cvt(INT,x));
 v=AV(x); *c=v[0]; *d=b?v[1]:0;
 ASSERT(0<=*c&&0<=*d,EVDOMAIN);
 R 1;
}    /* process a in a#"0 w */

static REPF(jtrep1s){A ax,e,x,y,z;B*b;I c,d,cd,j,k,m,n,p,q,*u,*v,wr,*ws;P*wp,*zp;
 RZ(a&&w);
 if(AT(a)&SCMPX)R rep1d(denseit(a),w,wf,wcr);
 RE(rep1sa(a,&c,&d)); cd=c+d;
 if(DENSE&AT(w))R rep1d(d?jdot2(sc(c),sc(d)):sc(c),w,wf,wcr);  // here if dense w
 wr=AR(w); ws=AS(w); n=wcr?*(wf+ws):1; RE(m=mult(n,cd));
 wp=PAV(w); e=SPA(wp,e); ax=SPA(wp,a); y=SPA(wp,i); x=SPA(wp,x);
 GASPARSE(z,AT(w),1,wr+!wcr,ws); *(wf+AS(z))=m; zp=PAV(z);
 RE(b=bfi(wr,ax,1));
 if(wcr&&b[wf]){    /* along sparse axis */
  u=AS(y); p=u[0]; q=u[1]; u=AV(y);
  RZ(x=repeat(sc(c),x));
  RZ(y=repeat(sc(c),y));
  if(p&&1<c){
   j=0; DO(wf, j+=b[i];); v=j+AV(y);
   if(AN(ax)==1+j){u+=j; DO(p, m=cd**u; u+=q; DO(c, *v=m+i; v+=q;););}
   else{A xx;I h,i,j1=1+j,*uu;
    GATV0(xx,INT,j1,1); uu=AV(xx);
    k=0; DO(j1, uu[i]=u[i];);
    for(i=0;i<p;++i,u+=q)
     if(ICMP(uu,u,j1)||i==p-1){
      h=(I )(i==p-1)+i-k; k=i; m=cd*uu[j]; 
      DO(j1, uu[i]=u[i];);
      DO(h, DO(c, *v=m+i; v+=q;););
     } 
    RZ(xx=grade1(y));
    RZ(x=from(xx,x));
    RZ(y=from(xx,y));
 }}}else{A xx;      /* along dense  axis */
  j=0; DO(wcr, j+=!b[wf+i];);
  RZ(y=ca(y));
  if(d){xx=jt->fill; jt->fill=e;}  // e cannot be virtual
  x=irs2(AR(a)&&CMPX&AT(a)?a:d?jdot2(sc(c),sc(d)):sc(c),x,0L,1L,j,jtrepeat); 
  if(d)jt->fill=xx; 
  RZ(x);
 }
 SPB(zp,e,e);
 SPB(zp,a,ax);
 SPB(zp,i,y);
 SPB(zp,x,x);
 R z;
}    /* scalar #"r sparse   or  sparse #"0 (dense or sparse) */


F2(jtrepeat){A z;I acr,ar,wcr,wf,wr;
 RZ(a&&w);F2PREFIP;
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK; 
 // special case: if a is atomic 1, and cells of w are not atomic.  a=0 is fast in the normal path
 if(wcr&&!ar&&AT(a)&(B01|INT)) {I aval = AT(a)&B01?(I)BAV(a)[0]:IAV(a)[0];  // no fast support for float
  if(!(aval&-2LL)){  // 0 or 1
   if(aval==1)R RETARG(w);   // 1 # y, return y
   if(!(AT(w)&SPARSE)){GA(z,AT(w),0,AR(w),0); MCISH(AS(z),AS(w),AR(w)) AS(z)[wf]=0; RETF(z);}  // 0 # y, return empty
  }
 }
 if(1<acr||acr<ar)R rank2ex(a,w,0L,1L,RMAX,acr,wcr,jtrepeat);  // loop if multiple cells of a
 ASSERT(!acr||!wcr||(AS(a)[0]==AS(w)[wf]),EVLENGTH);
 if(!acr||!wcr){RZ(z=!((AT(a)|AT(w))&SPARSE)?rep1d(a,w,wf,wcr):rep1s(a,w,wf,wcr)); RETF(z);}   // a is atom, or w is an atom and a has rank <= 1
 if(AT(a)&B01 +SB01 ){RZ(z=AT(a)&DENSE?repbdx(a,w,wf,wcr):repbsx(a,w,wf,wcr)); RETF(z);}
 if(AT(a)&CMPX+SCMPX){RZ(z=AT(a)&DENSE?repzdx(a,w,wf,wcr):repzsx(a,w,wf,wcr)); RETF(z);}
 /* integer */    {RZ(z=AT(a)&DENSE?repidx(a,w,wf,wcr):repisx(a,w,wf,wcr)); RETF(z);}
}    /* a#"r w main control */
