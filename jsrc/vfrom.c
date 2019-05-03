/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: From & Associates. See Hui, Some Uses of { and }, APL87.         */

#include "j.h"


F1(jtcatalog){PROLOG(0072);A b,*wv,x,z,*zv;C*bu,*bv,**pv;I*cv,i,j,k,m=1,n,p,*qv,r=0,*s,t=0,*u;
 F1RANK(1,jtcatalog,0);
 if(!(AN(w)&&AT(w)&BOX+SBOX))R box(w);
 n=AN(w); wv=AAV(w); 
 DO(n, x=wv[i]; if(AN(x)){p=AT(x); t=t?t:p; ASSERT(HOMO(t,p),EVDOMAIN); RE(t=maxtype(t,p));});  // use vector maxtype
 t=t?t:B01; k=bpnoun(t);
 GA(b,t,n,1,0);      bv=CAV(b);
 GATV0(x,INT,n,1);    qv=AV(x);
 GATV0(x,BOX,n,1);    pv=(C**)AV(x);
 RZ(x=apvwr(n,0L,0L)); cv=AV(x);
 DO(n, x=wv[i]; if(TYPESNE(t,AT(x)))RZ(x=cvt(t,x)); r+=AR(x); qv[i]=p=AN(x); RE(m=mult(m,p)); pv[i]=CAV(x););
 GATV0(z,BOX,m,r);    zv=AAV(z); s=AS(z); 
 DO(n, x=wv[i]; u=AS(x); DO(AR(x),*s++=*u++;););
 for(i=0;i<m;i++){
  bu=bv-k;
  DO(n, MC(bu+=k,pv[i]+k*cv[i],k););
  DO(n, j=n-1-i; if(qv[j]>++cv[j])break; cv[j]=0;);
  RZ(*zv++=ca(b));
 }
 EPILOG(z);
}

// obsolete #define SETJ(jexp)    {j=(jexp); if(j<0)j+=p; ASSERT((UI)j<(UI)p,EVINDEX);}

#define SETNDX(ndxvbl,ndxexp,limexp)    {ndxvbl=(ndxexp); if((UI)ndxvbl>=(UI)limexp){ndxvbl+=(limexp); ASSERT((UI)ndxvbl<(UI)limexp,EVINDEX);}}  // if ndxvbl>p, adding p can never make it OK
#define SETNDXRW(ndxvbl,ndxexp,limexp)    {ndxvbl=(ndxexp); if((UI)ndxvbl>=(UI)limexp){(ndxexp)=ndxvbl+=(limexp); ASSERT((UI)ndxvbl<(UI)limexp,EVINDEX);}}  // this version write to input if the value was negative
#define SETJ(jexp) SETNDX(j,jexp,p)

#define IFROMLOOP(T)        \
 {T   * RESTRICT v=(T*)wv,* RESTRICT x=(T*)zv;  \
  if(1==an){v+=j;   DQ(m,                                    *x++=*v;       v+=p; );}  \
  else              DQ(m, DO(an, SETJ(av[i]);                *x++=v[j];);   v+=p; );   \
 }
#define IFROMLOOP2(T,qexp)  \
 {T* RESTRICT u,* RESTRICT v=(T*)wv,* RESTRICT x=(T*)zv;  \
  q=(qexp); pq=p*q;         \
  if(1==an){v+=j*q; DQ(m,                     u=v;     DQ(q, *x++=*u++;);   v+=pq;);}  \
  else              DQ(m, DO(an, SETJ(av[i]); u=v+j*q; DQ(q, *x++=*u++;);); v+=pq;);   \
 }

F2(jtifrom){A z;C*wv,*zv;I acr,an,ar,*av,j,k,m,p,pq,q,wcr,wf,wk,wn,wr,*ws,zn;
 F1PREFIP;
 RZ(a&&w);
 // IRS supported.  This has implications for empty arguments.
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(ar>acr)R rank2ex(a,w,0L,acr,wcr,acr,wcr,jtifrom);  // split a into cells if needed.  Only 1 level of rank loop is used
 // From here on, execution on a single cell of a (on matching cell(s) of w, or all w).  The cell of a may have any rank
 an=AN(a); wn=AN(w); ws=AS(w);
 if(!(INT&AT(a)))RZ(a=cvt(INT,a));
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
// if(an==0 && wn==0 && ws[wf]==0)wcr=wr=0;     defer this pending analysis
 // If w is empty, portions of its shape may overflow during calculations, and there is no data to move (we still need to
 // audit for index error, though).  If w is not empty, there is no need to check for such overflow.  So we split the computation here.
 // Either way, we need   zn: #atoms in result   p: #items in a cell of w
 p=wcr?*(ws+wf):1;
 av=AV(a);  // point to the selectors
 I wflag=AFLAG(w);
 if(wn){
  // For virtual results we need: kn: number of atoms in an item of a cell of w;   
  PROD1(k, wcr-1, ws+wf+1);  // number of atoms in an item of a cell
  // Also m: #wcr-cells in w 
  PROD(m,wf,ws); zn=k*m;  RE(zn=mult(an,zn));
// correct  if(((zn-2)|-(wf|(wflag&(AFNJA))))>=0){  // zn>1 and not (frame or NJA)
  if((((AT(w)&(DIRECT|RECURSIBLE))-1)|(zn-2)|-(wf|(wflag&(AFNJA))))>=0){  // zn>1 and not (frame or NJA)
   // result is more than one atom and does not come from multiple cells.  Perhaps it should be virtual.  See if the indexes are consecutive
   I index0 = av[0]; index0+=(index0>>(BW-1))&p;  // index of first item
   // check the last item before checking the middle.
   I indexn = av[an-1]; indexn+=(indexn>>(BW-1))&p;
   if(indexn==index0+an-1){
     I indexp=index0; DO(an-1, indexn=av[1+i]; indexn+=(indexn>>(BW-1))&p; if(indexn!=indexp+1){indexn=p; break;} indexp=indexn;);
   }else indexn=p;
   if((index0|(p-indexn-1))>=0){  // index0>0 and indexn<=p-1
    // indexes are consecutive and in range.  Make the result virtual.  Rank of w cell must be > 0, since we have >=2 consecutive result atoms
    RZ(z=virtualip(w,index0*k,ar+wr-1));
    // fill in shape and number of atoms.  ar can be anything.
    I* as=AS(a); AN(z)=zn; I *s=AS(z); MCISH(s,as,ar) MCISH(s+ar,ws+1,wr-1)
    RETF(z);
   }
  }
  // for copying items, we need    k: size in bytes of an item of a cell of w
  k<<=bplg(AT(w));
 } else {zn=0;}  // No data to move
 // Allocate the result area and fill in the shape
 GA(z,AT(w),zn,ar+wr-(I )(0<wcr),0);  // result-shape is frame of w followed by shape of a followed by shape of item of cell of w; start with w-shape, which gets the frame
 MCISH(AS(z),AS(w),wf); MCISH(&AS(z)[wf],AS(a),ar); if(wcr)MCISH(&AS(z)[wf+ar],1+wf+ws,wcr-1);
 if(!zn){DO(an, SETJ(av[i])) R z;}  // If no data to move, just audit the indexes and quit
 // from here on we are moving items
 wk=k*p;   // stride between cells of w
 wv=CAV(w); zv=CAV(z); SETJ(*av);
 if(AT(w)&FL+CMPX){if(k==sizeof(D))IFROMLOOP(D) else IFROMLOOP2(D,k>>LGSZD);}
 else switch(k){
  case sizeof(C): IFROMLOOP(C); break; 
  case sizeof(S): IFROMLOOP(S); break;  
#if SY_64
  case sizeof(int):IFROMLOOP(int); break;
#endif
  case sizeof(I): IFROMLOOP(I); break;
  default:
   if     (0==(k&(SZI-1)))IFROMLOOP2(I,k>>LGSZI)
#if SY_64
   else if(0==(k&(SZI4-1)))IFROMLOOP2(int,k>>LGSZI4)
#endif
   else if(0==(k&(SZS-1)))IFROMLOOP2(S,k>>LGSZS)
   else{S*x,*u;
    q=1+(k>>LGSZS);
    if(1==an){wv+=k*j; DQ(m,                     x=(S*)zv; u=(S*) wv;      DQ(q, *x++=*u++;); zv+=k;   wv+=wk;);}
    else               DO(m, DO(an, SETJ(av[i]); x=(S*)zv; u=(S*)(wv+k*j); DQ(q, *x++=*u++;); zv+=k;); wv+=wk;);
  }
 }
 RETF(z);  // todo kludge should inherit norel
}    /* a{"r w for numeric a */

#define BSET(x,y0,y1,y2,y3)     *x++=y0; *x++=y1; *x++=y2; *x++=y3;
#define BSETV(b)                MC(v,wv+b*k,k); v+=k;

#if !SY_64 && SY_WIN32
#define BNNERN(T)   \
 {B*au=av;T*v=(T*)wv,v0,v1,*x=(T*)zv;                                               \
  DO(m, v0=v[0]; v1=v[1]; u=(I*)av; DO(q, switch(*u++){                             \
   case B0000: BSET(x,v0,v0,v0,v0); break;  case B0001: BSET(x,v0,v0,v0,v1); break; \
   case B0010: BSET(x,v0,v0,v1,v0); break;  case B0011: BSET(x,v0,v0,v1,v1); break; \
   case B0100: BSET(x,v0,v1,v0,v0); break;  case B0101: BSET(x,v0,v1,v0,v1); break; \
   case B0110: BSET(x,v0,v1,v1,v0); break;  case B0111: BSET(x,v0,v1,v1,v1); break; \
   case B1000: BSET(x,v1,v0,v0,v0); break;  case B1001: BSET(x,v1,v0,v0,v1); break; \
   case B1010: BSET(x,v1,v0,v1,v0); break;  case B1011: BSET(x,v1,v0,v1,v1); break; \
   case B1100: BSET(x,v1,v1,v0,v0); break;  case B1101: BSET(x,v1,v1,v0,v1); break; \
   case B1110: BSET(x,v1,v1,v1,v0); break;  case B1111: BSET(x,v1,v1,v1,v1); break; \
  });                                                                               \
  b=(B*)u; DO(r, *x++=*b++?v1:v0;); v+=p;);                                         \
 }
#define BNNERM(T,T1)   \
 {B*au=av;T*c,*v=(T*)wv,v0,v1,*x=(T*)zv;T1 vv[16],*y;                                  \
  DO(m, v0=v[0]; v1=v[1]; c=(T*)vv; y=(T1*)x; u=(I*)av;                                \
   BSET(c,v0,v0,v0,v0); BSET(c,v0,v0,v0,v1); BSET(c,v0,v0,v1,v0); BSET(c,v0,v0,v1,v1); \
   BSET(c,v0,v1,v0,v0); BSET(c,v0,v1,v0,v1); BSET(c,v0,v1,v1,v0); BSET(c,v0,v1,v1,v1); \
   BSET(c,v1,v0,v0,v0); BSET(c,v1,v0,v0,v1); BSET(c,v1,v0,v1,v0); BSET(c,v1,v0,v1,v1); \
   BSET(c,v1,v1,v0,v0); BSET(c,v1,v1,v0,v1); BSET(c,v1,v1,v1,v0); BSET(c,v1,v1,v1,v1); \
   DO(q, switch(*u++){                                                                 \
    case B0000: *y++=vv[ 0]; break;  case B0001: *y++=vv[ 1]; break;                   \
    case B0010: *y++=vv[ 2]; break;  case B0011: *y++=vv[ 3]; break;                   \
    case B0100: *y++=vv[ 4]; break;  case B0101: *y++=vv[ 5]; break;                   \
    case B0110: *y++=vv[ 6]; break;  case B0111: *y++=vv[ 7]; break;                   \
    case B1000: *y++=vv[ 8]; break;  case B1001: *y++=vv[ 9]; break;                   \
    case B1010: *y++=vv[10]; break;  case B1011: *y++=vv[11]; break;                   \
    case B1100: *y++=vv[12]; break;  case B1101: *y++=vv[13]; break;                   \
    case B1110: *y++=vv[14]; break;  case B1111: *y++=vv[15]; break;                   \
   });                                                                                 \
   b=(B*)u; x=(T*)y; DO(r, *x++=*b++?v1:v0;); v+=p;);                                  \
 }
#else
#define BNNERN(T)       {T*v=(T*)wv,*x=(T*)zv; DQ(m, b=av; DQ(an, *x++=*(v+*b++);); v+=p;);}
#define BNNERM(T,T1)    BNNERN(T)
#endif

#define INNER1B(T)  {T*v=(T*)wv,*x=(T*)zv; v+=*av; DQ(m, *x++=*v; v+=p;);}

// a is boolean
static F2(jtbfrom){A z;B*av,*b;C*wv,*zv;I acr,an,ar,k,m,p,q,r,*u=0,wcr,wf,wk,wn,wr,*ws,zn;
 RZ(a&&w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(ar>acr)R rank2ex(a,w,0L,acr,wcr,acr,wcr,jtbfrom);
 an=AN(a); wn=AN(w); ws=AS(w);
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce domain error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
// if(an==0 && wn==0 && ws[wf]==0)wcr=wr=0;
 p=wcr?*(ws+wf):1; q=an>>LGSZI; r=an&(SZI-1);
 ASSERT(2<=p||1==p&&all0(a)||!p&&!an,EVINDEX);
 // We always need zn, the number of result atoms
 if(wn){
  // If there is data to move, we also need m: #cells of w   k: #bytes in an items of a cell of w   wk: #bytes in a cell of w
  PROD(m,wf,ws); PROD1(k, wcr-1, ws+wf+1); zn=k*m; k<<=bplg(AT(w)); wk=k*p; RE(zn=mult(an,zn));
 }else{zn=0;}
 GA(z,AT(w),zn,ar+wr-(I )(0<wcr),0);
 MCISH(AS(z),AS(w),wf); MCISH(&AS(z)[wf],AS(a),ar); if(wcr)MCISH(&AS(z)[wf+ar],1+wf+ws,wcr-1);
// obsolete  s=AS(z)+wf; MCISH(s,AS(a),ar); MCISH(s+ar,1+wf+ws,wcr-1);
 if(!zn)R z;  // If no data to move, just return the shape
 av=BAV(a); wv=CAV(w); zv=CAV(z);
 switch(k+k+(I )(1==an)){
  case   2*sizeof(I): BNNERN(I);   break;
  case   2*sizeof(C): BNNERM(C,I); break; 
  case 1+2*sizeof(C): INNER1B(C);  break;
  case 1+2*sizeof(S): INNER1B(S);  break;
#if SY_64
  case 1+2*sizeof(int): INNER1B(int);  break;
#endif
  case 1+2*sizeof(I): INNER1B(I);  break;
  default:
   if(1==an){wv+=k**av; DQ(m, MC(zv,wv,k); zv+=k; wv+=wk;);}
#if !SY_64 && SY_WIN32
   else{A x;C*v,*xv,*xv00,*xv01,*xv02,*xv03,*xv04,*xv05,*xv06,*xv07,*xv08,*xv09,*xv10,*xv11,
         *xv12,*xv13,*xv14,*xv15;I i,j,k4=k*4;
    GATV0(x,LIT,16*k4,1); xv=CAV(x);
    xv00=xv;       xv01=xv+   k4; xv02=xv+ 2*k4; xv03=xv+ 3*k4;
    xv04=xv+ 4*k4; xv05=xv+ 5*k4; xv06=xv+ 6*k4; xv07=xv+ 7*k4;
    xv08=xv+ 8*k4; xv09=xv+ 9*k4; xv10=xv+10*k4; xv11=xv+11*k4;
    xv12=xv+12*k4; xv13=xv+13*k4; xv14=xv+14*k4; xv15=xv+15*k4;
    for(i=0;i<m;++i){
     u=(I*)av; v=xv;
     BSETV(0); BSETV(0); BSETV(0); BSETV(0);   BSETV(0); BSETV(0); BSETV(0); BSETV(1);
     BSETV(0); BSETV(0); BSETV(1); BSETV(0);   BSETV(0); BSETV(0); BSETV(1); BSETV(1);
     BSETV(0); BSETV(1); BSETV(0); BSETV(0);   BSETV(0); BSETV(1); BSETV(0); BSETV(1);
     BSETV(0); BSETV(1); BSETV(1); BSETV(0);   BSETV(0); BSETV(1); BSETV(1); BSETV(1);
     BSETV(1); BSETV(0); BSETV(0); BSETV(0);   BSETV(1); BSETV(0); BSETV(0); BSETV(1);
     BSETV(1); BSETV(0); BSETV(1); BSETV(0);   BSETV(1); BSETV(0); BSETV(1); BSETV(1);
     BSETV(1); BSETV(1); BSETV(0); BSETV(0);   BSETV(1); BSETV(1); BSETV(0); BSETV(1);
     BSETV(1); BSETV(1); BSETV(1); BSETV(0);   BSETV(1); BSETV(1); BSETV(1); BSETV(1);
     for(j=0;j<q;++j,zv+=k4)switch(*u++){
      case B0000: MC(zv,xv00,k4); break;   case B0001: MC(zv,xv01,k4); break;
      case B0010: MC(zv,xv02,k4); break;   case B0011: MC(zv,xv03,k4); break;  
      case B0100: MC(zv,xv04,k4); break;   case B0101: MC(zv,xv05,k4); break;
      case B0110: MC(zv,xv06,k4); break;   case B0111: MC(zv,xv07,k4); break;
      case B1000: MC(zv,xv08,k4); break;   case B1001: MC(zv,xv09,k4); break;
      case B1010: MC(zv,xv10,k4); break;   case B1011: MC(zv,xv11,k4); break;
      case B1100: MC(zv,xv12,k4); break;   case B1101: MC(zv,xv13,k4); break;
      case B1110: MC(zv,xv14,k4); break;   case B1111: MC(zv,xv15,k4); break;
     }
     b=(B*)u; DO(r, MC(zv,wv+k**b++,k); zv+=k;); wv+=wk;
   }}
#else
   else DO(m, b=av; DO(an, MC(zv,wv+k**b++,k); zv+=k;); wv+=wk;);
#endif
 }
 RETF(z);  // todo kludge should inherit norel
}    /* a{"r w for boolean a */

// a is array whose 1-cells are index lists, w is array
// result is the indexed items
// the numbers in a have been audited for validity
// w is length of the frame
A jtfrombu(J jt,A a,A w,I wf){F1PREFIP;A p,q,z;B b=0;I ar,*as,h,m,r,*u,*v,wcr,wr,*ws;
 ar=AR(a); as=AS(a); h=as[ar-1];  // h is length of the index list, i. e. number of axes of w that disappear during indexing
 wr=AR(w); ws=AS(w); wcr=wr-wf;
 DO(ar, if(!as[i]){b=1; break;});
 DO(wr, if(!ws[i]){b=1; break;});
 if(b){  // empty array, either a or w
  GA(z,AT(w),0,wf+(wcr-h)+(ar-1),0); u=AS(z);
  v=ws;      DO(wf,    *u++=*v++;);
  v=as;      DO(ar-1,  *u++=*v++;);
  v=ws+wf+h; DO(wcr-h, *u++=*v++;);
  R z;
 }
 fauxblockINT(pfaux,4,1); fauxINT(p,pfaux,h,1) v=AV(p)+h; u=ws+wf+h; m=1; DO(h, *--v=m; m*=*--u;);  // m is number of items in the block of axes that index into w
 r=wr+1-h;  // rank of result is rank of w, minus h axes that go away and are replaced by 1 axis
 // We will use pdt to create an index to the cell
 if(r==wr){
  z=irs2(pdt(a,p),w,VFLAGNONE, RMAX,wcr+1-h,jtifrom);
 }else{
  //  reshape w to combine the first h axes of each cell
  RZ(q=virtualip(w,0,r)); AN(q)=AN(w); v=AS(q); MCISH(v,ws,wf); v[wf]=m; MCISH(v+wf+1,ws+wf+h,wcr-h);  /* q is reshape(.,w) */
  z=irs2(pdt(a,p),q,VFLAGNONE, RMAX,wcr+1-h,jtifrom);
 }
 RETF(z);
}    /* (<"1 a){"r w, dense w, integer array a */

#define AUDITPOSINDEX(x,lim) if((UI)(x)>=(UI)(lim)){if((x)<0)break; ASSERT(0,EVINDEX);}
// a is boxed list, w is array, wf is frame of operation, *ind will hold the result
// if the opened boxes have contents with the same item shape (treating atoms as same as singleton lists), create an array of all the indexes; put that into *ind and return 1.
// otherwise return 0
B jtaindex(J jt,A a,A w,I wf,A*ind){A*av,q,z;I an,ar,c,j,k,t,*u,*v,*ws;
 RZ(a&&w);
 an=AN(a); *ind=0;
 if(!an)R 0;
 ws=wf+AS(w); ar=AR(a); av=AAV(a);  q=av[0]; c=AN(q);
 if(!c)R 0;
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 GATV(z,INT,an*c,1+ar,AS(a)); AS(z)[ar]=c; v=AV(z);
 for(j=0;j<an;++j){
  q=av[j]; t=AT(q);
  if(t&BOX)R 0;
  if(!(t&INT))RZ(q=cvt(INT,q));
  if(!(c==AN(q)&&1>=AR(q)))R 0; 
  u=AV(q);
// obsolete   DO(c, k=u[i]; if(0>k)k+=ws[i]; ASSERT((UI)k<(UI)ws[i],EVINDEX); *v++=k;);
  DO(c, SETNDX(k,u[i],ws[i]) *v++=k;);
 }
 *ind=z;
 R 1;
}    /* <"1 a to a where a is an integer index array */

static B jtaindex1(J jt,A a,A w,I wf,A*ind){A z;I c,i,k,n,t,*v,*ws;
 RZ(a&&w);
 n=AN(a); t=AT(a); *ind=0; if(AR(a)==0)R 0;  // revert to normal code for atomic a
 ws=wf+AS(w); c=*(AS(a)+AR(a)-1);   // c = length of 1-cell
 if(((n-1)|(c-1)|-(t&BOX))<0)R 0;  // revert to normal code for empty or boxed a
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 PROD(n,AR(a)-1,AS(a));  v=AV(a); // n now=number of 1-cells of a   v=running pointer through a
 // Go through a fast verification pass.  If all values nonnegative and valid, return original a
 if(t&INT){  // if it's INT already, we don't need to move it.
  switch(c){I c0,c1,c2;
  case 2:
   c0=ws[0], c1=ws[1]; for(i=n;i>0;--i){AUDITPOSINDEX(v[0],c0) AUDITPOSINDEX(v[1],c1)  v+=2;} break;
  case 3:
   c0=ws[0], c1=ws[1], c2=ws[2]; for(i=n;i>0;--i){AUDITPOSINDEX(v[0],c0) AUDITPOSINDEX(v[1],c1) AUDITPOSINDEX(v[2],c2) v+=3;} break;
  default:
   for(i=n;i>0;--i){DO(c, k=*v; AUDITPOSINDEX(k,ws[i]) ++v;); if(k<0)break;} break; 
  }
 }else i=1;  // if not INT to begin with, we must convert
 if(i==0){z=a;  // If all indexes OK, return the original block
 }else{
  // There was a negative index.  Allocate a new block for a and copy to it.
  RZ(z=t&INT?ca(a):cvt(INT,a));  v=AV(z);
// obsolete   DO(n, DO(c, k=*v; if(0>k)*v=k+=ws[i]; ASSERT((UI)k<(UI)ws[i],EVINDEX); ++v;););  // convert indexes to nonnegative & check for in-range
  DQ(n, DO(c, SETNDXRW(k,*v,ws[i]) ++v;););  // convert indexes to nonnegative & check for in-range
 }
 *ind=z;
 R 1;
}    /* verify that <"1 a is valid for (<"1 a){w */

static A jtafrom2(J jt,A p,A q,A w,I r){A z;C*wv,*zv;I d,e,j,k,m,n,pn,pr,* RESTRICT pv,
  qn,qr,* RESTRICT qv,* RESTRICT s,wf,wr,* RESTRICT ws,zn;
 wr=AR(w); ws=AS(w); wf=wr-r; 
 pn=AN(p); pr=AR(p); pv=AV(p);
 qn=AN(q); qr=AR(q); qv=AV(q);
 if(AN(w)){
  // Set zn=#atoms of result  d=#atoms in a _2-cell of cell of w
  // e=length of axis corresponding to q  n=#_2-cells in a cell of w   m=#cells of w (frame*size of 2-cell*(# _2-cells = pn*qn))
  PROD(m,wf,ws); PROD(d,r-2,ws+wf+2); e=ws[1+wf]; n=e*ws[wf]; RE(zn=mult(pn,mult(qn,d*m)));
 }else{zn=0;}
 GA(z,AT(w),zn,wf+pr+qr+r-2,ws);
 s=AS(z)+wf; MCISH(s,AS(p),pr); MCISH(s+pr,AS(q),qr); MCISH(s+pr+qr,ws+wf+2,r-2);
 if(!zn)R z;  // If no data to move, exit with empty.  Rank is right
 wv=CAV(w); zv=CAV(z); 
 switch(k=d<<bplg(AT(w))){   // k=*bytes in a _2-cell of a cell of w
#define INNER2(T) {T* RESTRICT v=(T*)wv,* RESTRICT x=(T*)zv;   \
   DO(m, DO(pn, j=e*pv[i]; DO(qn, *x++=v[j+qv[i]];         )); v+=n;); R z;}  // n=#_2-cells in a cell of w
  case sizeof(I): INNER2(I);
  case sizeof(C): INNER2(C);
  case sizeof(S): INNER2(S);
#if SY_64
  case sizeof(I4): INNER2(I4);
#endif
#if !SY_64 && SY_WIN32
  case sizeof(D): if(AT(w)&FL)INNER2(D);
   // copy only echt floats using floating-point moves.  Otherwise fall through to...
#endif
  default:        {C* RESTRICT v=wv,* RESTRICT x=zv-k;n=k*n;   // n=#bytes in a cell of w
   DO(m, DO(pn, j=e*pv[i]; DO(qn, MC(x+=k,v+k*(j+qv[i]),k);)); v+=n;); R z;}
 }   /* (<p;q){"r w  for positive integer arrays p,q */
}

// n is length of axis, w is doubly-unboxed selector
// result is list of selectors - complementary if w is boxed
static A jtafi(J jt,I n,A w){A x;
 if(!(AN(w)&&BOX&AT(w)))R pind(n,w);
 ASSERT(!AR(w),EVINDEX);  // if boxed, must be an atom
 x=AAV0(w);
 R AN(x)?less(IX(n),pind(n,x)):ace; 
}

static F2(jtafrom){PROLOG(0073);A c,ind,p=0,q,*v,y=w;B bb=1;I acr,ar,i=0,j,m,n,pr,*s,t,wcr,wf,wr;
 RZ(a&&w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(ar){  // if there is an array of boxes
// obsolete  if(ar==acr&&wr==wcr){RE(aindex(a,w,wf,&ind)); if(ind)R frombu(ind,w,wf);}
  if(((ar^acr)|(wr^wcr))==0){RE(aindex(a,w,wf,&ind)); if(ind)R frombu(ind,w,wf);}  // if boxing doesn't contribute to shape, open the boxes of a and copy the values
  R wr==wcr?rank2ex(a,w,0L,0L,wcr,0L,wcr,jtafrom):  // if a has frame, rank-loop over a
      df2(irs1(a,0L,acr,jtbox),irs1(w,0L,wcr,jtbox),amp(ds(CLBRACE),ds(COPE)));  // (<"0 a) {&> <"0 w
 }
 c=AAV0(a); t=AT(c); n=IC(c); v=AAV(c);   // B prob not reqd 
 s=AS(w)+wr-wcr;
 ASSERT(1>=AR(c),EVRANK);
 ASSERT(n<=wcr,EVLENGTH);
 if(n&&!(t&BOX)){RE(aindex(a,w,wf,&ind)); if(ind)R frombu(ind,w,wf);}
 if(wcr==wr)for(i=m=pr=0;i<n;++i){
  p=afi(s[i],v[i]);
  if(!(p&&1==AN(p)&&INT&AT(p)))break;
  pr+=AR(p); 
  m+=*AV(p)*prod(wcr-i-1,1+i+s);
 }
 if(i){I*ys;
  RZ(y=virtual(w,m,pr+wcr-i));
  ys=AS(y); DO(pr, *ys++=1;); MCISH(ys,s+i,wcr-i);
  AN(y)=prod(AR(y),AS(y));
 }
 // take axes 2 at a time, properly handling omitted axes.  First time through p is set
 for(;i<n;i+=2){
  j=1+i; if(!p)p=afi(s[i],v[i]); q=j<n?afi(s[j],v[j]):ace; if(!(p&&q))break;
  if(p!=ace&&q!=ace){y=afrom2(p,q,y,wcr-i);}
  else if(p!=ace)   {y=irs2(p,y,0L,AR(p),wcr-i,jtifrom);}
  else if(q!=ace)   {y=irs2(q,y,0L,AR(q),wcr-j,jtifrom);}
  p=0;
 }
 // We have to make sure that a virtual NJA block does not become the result, because x,y and x u}y allow modifying those even when the usecount is 1.  Realize in that case
 RE(y); if(AFLAG(y)&AFNJA)RZ(y=ca(y));   EPILOG(y);   // If the result is NJA, it must be virtual.
}    /* a{"r w for boxed index a */

F2(jtfrom){I at;A z;
 F2PREFIP;
 RZ(a&&w);
 at=AT(a);
 switch((at&SPARSE?2:0)+(AT(w)&SPARSE?1:0)){
  case 0: z=at&BOX?afrom(a,w)  :at&B01&&AN(a)!=1?bfrom(a,w):jtifrom(jtinplace,a,w); break;  // make 0{ and 1{ allow virtual result
  case 1: z=at&BOX?frombs(a,w) :                  fromis(a,w); break;
  case 2: z=fromsd(a,w); break;
  default: z=fromss(a,w); break;
 }
 RZ(z); RETF(z);
}   /* a{"r w main control */

F2(jtsfrom){
 if(!(SPARSE&AT(w))){
  // Not sparse.  Verify the indexes are numeric and not empty
  if(((AN(a)-1)|(AR(a)-2)|((AT(a)&NUMERIC)-1))>=0){A ind;   // a is an array with rank>1 and numeric.  Rank 1 is unusual & unimportant & we'll ignore it
   // Check indexes for validity; if valid, turn each row into a cell offset
   if(ind=celloffset(w,a)){
    // Fetch the cells and return.  ind is now an array of cell indexes.  View w as an array of those cells
    // We could do this with ifrom, but it validates the input and checks for virtual, neither of which is germane here.  Also, we would have
    // to reshape w into an array of cells.  Easier just to copy the data right here
    // Find the number of axes included in each cell offset; get the cell size
    I cellsize; PROD(cellsize,AR(w)-AS(a)[AR(a)-1],AS(w)+AS(a)[AR(a)-1]);  // number of atoms per index in ind
    I * RESTRICT iv=AV(ind);  // start of the cell-index array
    A z; GA(z,AT(w),cellsize*AN(ind),AR(ind)+AR(w)-AS(a)[AR(a)-1],0)  MCISH(AS(z),AS(ind),AR(ind)) MCISH(AS(z)+AR(ind),AS(w)+AS(a)[AR(a)-1],AR(w)-AS(a)[AR(a)-1])  // shape from ind, then w
    cellsize <<= bplg(AT(w));   // change cellsize to bytes
    switch(cellsize){
    case sizeof(C):
     {C * RESTRICT zv=CAV(z); C *RESTRICT wv=CAV(w); DQ(AN(ind), *zv++=wv[*iv++];) break;}  // scatter-copy the data
    case sizeof(I):  // may include D
     {I * RESTRICT zv=IAV(z); I *RESTRICT wv=IAV(w); DQ(AN(ind), *zv++=wv[*iv++];) break;}  // scatter-copy the data, 8-byte chunks
    default:
     // handle small integral number of words with a local loop
     if(!(cellsize&~(MEMCPYTUNELOOP-SZI))){  // length is an even number of I and not too big
      C* RESTRICT zv=CAV(z); C *RESTRICT wv=CAV(w); DQ(AN(ind), MCIS((I*)zv,(I*)(wv+*iv++*cellsize),cellsize>>LGSZI) zv+=cellsize;)  // use local copy
     }else{
      C* RESTRICT zv=CAV(z); C *RESTRICT wv=CAV(w); DQ(AN(ind), MC(zv,wv+*iv++*cellsize,cellsize); zv+=cellsize;)  // use local copy
     }
     break;
    }
    RETF(z);
   }
  }
 }else{A ind;
  // sparse.  See if we can audit the index list.  If we can, use it, else execute the slow way
  RE(aindex1(a,w,0L,&ind)); if(ind)R frombsn(ind,w,0L);
 }
 // If we couldn't handle it as a special case, do it the hard way
 RETF(from(irs1(a,0L,1L,jtbox),w));
}    /* (<"1 a){w */

static F2(jtmapx){
 RZ(a&&w);
 if(!(BOX&AT(w)))R ope(a);
 R every2(box0(every2(a,box0(catalog(every(shape(w),0L,jtiota))),0L,jtover)),w,0L,jtmapx);
}

F1(jtmap){R mapx(ace,w);}

// extract the single box a from w and open it.  Don't mark it no-inplace.
static F2(jtquicksel){I index;
 RE(index=i0(a));  // extract the index
// obsolete  index=index+((index>>(BW-1))&AN(w)); ASSERT((UI)index<(UI)AN(w),EVINDEX);   // remap negative index, check range
 SETNDX(index,index,AN(w))   // remap negative index, check range
 R AAV(w)[index];  // select the box
}

F2(jtfetch){A*av, z;I n;F2PREFIP;
 F2RANK(1,RMAX,jtfetch,0);
 if(!(BOX&AT(a))){
  // look for the common special case scalar { boxed vector.  This path doesn't run EPILOG
  if(!AR(a) && AR(w)==1 && AT(w)&BOX){
   RZ(z=jtquicksel(jt,a,w));
   // Inplaceability depends on the context.  If the overall operand is either noninplaceable or in a noninplaceable context, we must
   // protect the value we fetch (the overall operand would matter only if it was flagged without a ra())
   if(!ACIPISOK(w)||!((I)jtinplace&JTINPLACEW))ACIPNO(z); RETF(z);
  }
  RZ(a=box(a));  // if not special case, box any unboxed a
 }
 n=AN(a); av=AAV(a); 
 if(!n)R w; z=w;
 DO(n, A next=av[i]; if(!AR(next) && !(AT(next)&BOX) && AR(z)==1 && AT(z)&BOX){RZ(z=jtquicksel(jt,next,z))}
      else{RZ(z=afrom(box(next),z)); if(i<n-1)ASSERT(!AR(z),EVRANK); if(!AR(z)&&AT(z)&BOX)RZ(z=ope(z));}
   );
 if(!ACIPISOK(w)||!((I)jtinplace&JTINPLACEW))ACIPNO(z); RETF(z);   // Mark the box as non-inplaceable, as above
}

