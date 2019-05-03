/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Binary Representation                                            */

#include "j.h"
#include "x.h"


F1(jtstype){RZ(w); R sc(AT(w)&-AT(w));}
     /* 3!:0 w */

// a is integer atom or list, values indicating the desired result
// atom values in x: 0=NJA, others reserved
F2(jtnouninfo2){A z;
 RZ(a&&w);
 RZ(a=vi(a)); // convert to integer, error if can't
 ASSERT(AR(a)<2,EVRANK);  // must be atom or list
 GATV(z,INT,AN(a),AR(a),AS(a));  // allocate result
 I *av=IAV(a), *zv=IAV(z);
 DQ(AN(z),   // install the requested info
  switch(*av++){
  default: ASSERT(0,EVDOMAIN); break;
  case 0: *zv++=(AFLAG(w)>>AFNJAX)&1; break;
  }
 )
 RETF(z);
}
// a 3!:9 w   noun info


/* binary and hex representation formats differ per J version              */
/* pre J6.01                                                               */
/*    (type, flag, #elements, rank, shape; ravel)                          */
/*    flag is set to 0 for 32 bits and _1 for 64 bits                      */
/* J6.01 and later                                                         */
/*    (flag, type, #elements, rank, shape; ravel)                          */
/*    first byte of flag is                                                */
/*      e0     32 bits, reversed byte order                                */
/*      e1     32 bits, reversed byte order                                */
/*      e2     64 bits, standard byte order                                */
/*      e3     64 bits, reversed byte order                                */
/*      other  pre 601 header                                              */


#define LGWS(d)         ((d)+2)  // LG(WS(d))
#define WS(d)           (((I)1)<<LGWS(d))                 /* word size in bytes              */
#define BH(d)           (4LL<<LGWS(d))               /* # non-shape header bytes in A   */
#define BF(d,a)         ((C*)(a)        )       /* flag                            */
#define BT(d,a)         ((C*)(a)+  WS(d))       /* type                            */
#define BTX(d,pre601,a) ((C*)(a)+  WS(d)*!pre601)
#define BN(d,a)         ((C*)(a)+(2LL<<LGWS(d)))       /* # elements in ravel             */
#define BR(d,a)         ((C*)(a)+(3LL<<LGWS(d)))       /* rank                            */
#define BS(d,a)         ((C*)(a)+(4LL<<LGWS(d)))       /* shape                           */
#define BV(d,a,r)       (BS(d,a)+((r)<<LGWS(d)))     /* value                           */
#define BU              (C_LE ? 1 : 0)


static I bsize(J jt,B d,B tb,I t,I n,I r,I*s){I c,k,m,w,z;
 w=WS(d);
 z=BH(d)+w*r;
 if(t&BIT){
  c=r?s[r-1]:1; m=c?n/c:0; 
  R z+w*m*((c+w*BB-1)/(w*BB));
 }else{
  k=t&INT+SBT+BOX+XNUM?w:t&RAT?w+w:bp(t); 
  R z+((n*k+(tb&&t&LAST0)+w-1)&(-w));
}}   /* size in byte of binary representation */


/* n:  # of words                */
/* v:  ptr to result             */
/* u:  ptr to argument           */
/* bv: 1 iff v is little-endian  */
/* bu: 1 iff u is little-endian  */
/* dv: 1 iff v is 64-bit         */
/* du: 1 iff u is 64-bit         */

#define MVCS(a,b,c,d)  (8*(a)+4*(b)+2*(c)+(d))

static B jtmvw(J jt,C*v,C*u,I n,B bv,B bu,B dv,B du){C c;
 switch((dv?8:0)+(du?4:0)+(bv?2:0)+bu){
  case MVCS(0,0,0,0): MC(v,u,n*4);                             break;
  case MVCS(0,0,0,1): DO(n, DO(4, v[3-i]=u[i];); v+=4; u+=4;); break;
  case MVCS(0,0,1,0): DO(n, DO(4, v[3-i]=u[i];); v+=4; u+=4;); break;
  case MVCS(0,0,1,1): MC(v,u,n*4);                             break;
  case MVCS(0,1,0,0): DO(n, c=127<(UC)u[0]?CFF:C0; DO(4, ASSERT(c==u[  i],EVLIMIT); v[i]=u[4+i];); v+=4; u+=8;); break;
  case MVCS(0,1,0,1): DO(n, c=127<(UC)u[7]?CFF:C0; DO(4, ASSERT(c==u[7-i],EVLIMIT); v[i]=u[3-i];); v+=4; u+=8;); break;
  case MVCS(0,1,1,0): DO(n, c=127<(UC)u[0]?CFF:C0; DO(4, ASSERT(c==u[3-i],EVLIMIT); v[i]=u[7-i];); v+=4; u+=8;); break;
  case MVCS(0,1,1,1): DO(n, c=127<(UC)u[7]?CFF:C0; DO(4, ASSERT(c==u[4+i],EVLIMIT); v[i]=u[  i];); v+=4; u+=8;); break;
  case MVCS(1,0,0,0): DO(n, c=127<(UC)u[0]?CFF:C0; DO(4, v[  i]=c; v[4+i]=u[i];); v+=8; u+=4;); break;
  case MVCS(1,0,0,1): DO(n, c=127<(UC)u[3]?CFF:C0; DO(4, v[3-i]=c; v[7-i]=u[i];); v+=8; u+=4;); break;
  case MVCS(1,0,1,0): DO(n, c=127<(UC)u[0]?CFF:C0; DO(4, v[7-i]=c; v[3-i]=u[i];); v+=8; u+=4;); break;
  case MVCS(1,0,1,1): DO(n, c=127<(UC)u[3]?CFF:C0; DO(4, v[4+i]=c; v[  i]=u[i];); v+=8; u+=4;); break;
  case MVCS(1,1,0,0): MC(v,u,n*8);                             break;
  case MVCS(1,1,0,1): DO(n, DO(8, v[7-i]=u[i];); v+=8; u+=8;); break;
  case MVCS(1,1,1,0): DO(n, DO(8, v[7-i]=u[i];); v+=8; u+=8;); break;
  case MVCS(1,1,1,1): MC(v,u,n*8);                             break;
 }
 R 1;
}    /* move n words from u to v */

static C*jtbrephdr(J jt,B b,B d,A w,A y){A q;I f,r;I extt = UNSAFE(AT(w));
 q=(A)AV(y); r=AR(w); f=0;
 RZ(mvw(BF(d,q),(C*)&f,    1L,b,BU,d,SY_64)); *CAV(y)=d?(b?0xe3:0xe2):(b?0xe1:0xe0);
 RZ(mvw(BT(d,q),(C*)&extt,1L,b,BU,d,SY_64));
 RZ(mvw(BN(d,q),(C*)&AN(w),1L,b,BU,d,SY_64));
 RZ(mvw(BR(d,q),(C*)&r,1L,b,BU,d,SY_64));  // r is an I
 RZ(mvw(BS(d,q),(C*) AS(w),r, b,BU,d,SY_64));
 R BV(d,q,r);
}

static A jtbreps(J jt,B b,B d,A w){A q,y,z,*zv;C*v;I c=0,kk,m,n;P*wp;
 wp=PAV(w);
 n=1+sizeof(P)/SZI; kk=WS(d);
 GATV0(z,BOX,n,1); zv=AAV(z);
 GATV0(y,LIT,bsize(jt,d,1,INT,n,AR(w),AS(w)),1);
 v=brephdr(b,d,w,y);
 RZ(mvw(v,(C*)&c,1L,BU,b,d,SY_64));  /* reserved for flag */
 zv[0]=y; m=AN(y);
 RZ(zv[1]=q=brep(b,d,SPA(wp,a))); RZ(mvw(v+  kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q);
 RZ(zv[2]=q=brep(b,d,SPA(wp,e))); RZ(mvw(v+2*kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q);
 RZ(zv[3]=q=brep(b,d,SPA(wp,i))); RZ(mvw(v+3*kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q);
 RZ(zv[4]=q=brep(b,d,SPA(wp,x))); RZ(mvw(v+4*kk,(C*)&m,1L,b,BU,d,SY_64));
 R raze(z);
}    /* 3!:1 w for sparse w */

A jtbrep(J jt,B b,B d,A w){A q,*wv,y,z,*zv;C*u,*v;I e,klg,kk,m,n,t;
 RZ(w);
 e=n=AN(w); t=UNSAFE(AT(w)); u=CAV(w); klg=bplg(t); kk=WS(d);
 if(t&SPARSE)R breps(b,d,w);
 GATV0(y,LIT,bsize(jt,d,1,t,n,AR(w),AS(w)),1);
 v=brephdr(b,d,w,y);
 if(t&DIRECT)switch(CTTZ(t)){
  case SBTX:
  case INTX:  RZ(mvw(v,u,n,  b,BU,d,SY_64)); R y;
  case FLX:   RZ(mvw(v,u,n,  b,BU,1,1    )); R y;
  case CMPXX: RZ(mvw(v,u,n+n,b,BU,1,1    )); R y;
  default:
   // 1- and 2-byte C4T types, all of which have LAST0.  We need to clear the last
   // bytes, because the datalength is rounded up in bsize, and thus there are
   // up to 3 words at the end of y that will not be copied to.  We clear them to
   // 0 to provide repeatable results.
   // Make sure there is a zero byte if the string is empty
   {I suffsize = MIN(4*SZI,(CAV(y)+AN(y))-(C*)v);  // len of area to clear to 0 
   memset((CAV(y)+AN(y))-suffsize,C0,suffsize);   // clear suffix
   MC(v,u,n<<klg); R y;}      // copy the valid part of the data
 }
 if(t&RAT){e+=n; GATV0(q,XNUM,e,1); MC(AV(q),u,n<<klg);}
 else     RZ(q=1<AR(w)?ravel(w):w);
 m=AN(y); wv=AAV(w); 
 GATV0(z,BOX,1+e,1); zv=AAV(z); 
 *zv++=y;
 DO(e, RZ(*zv++=q=brep(b,d,wv[i])); RZ(mvw(v+i*kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q););
 R raze(z);
}    /* b iff reverse the bytes; d iff 64-bit */

static A jthrep(J jt,B b,B d,A w){A y,z;C c,*hex="0123456789abcdef",*u,*v;I n,s[2];
 RZ(y=brep(b,d,w));
 n=AN(y); s[0]=n>>LGWS(d); s[1]=2*WS(d); 
 GATVR(z,LIT,2*n,2,s);  
 u=CAV(y); v=CAV(z); 
 DO(n, c=*u++; *v++=hex[(c&0xf0)>>4]; *v++=hex[c&0x0f];); 
 RETF(z);
}

F1(jtbinrep1){RZ(w); ASSERT(NOUN&AT(w),EVDOMAIN); R brep(BU,SY_64,w);}  /* 3!:1 w */
F1(jthexrep1){RZ(w); ASSERT(NOUN&AT(w),EVDOMAIN); R hrep(BU,SY_64,w);}  /* 3!:3 w */

F2(jtbinrep2){I k;
 RZ(a&&w);
 RE(k=i0(a)); if(10<=k)k-=8;
 ASSERT(k<=0||k<=3,EVDOMAIN);
 R brep((B)(k&1),(B)(2<=k),w);
}    /* a 3!:1 w */

F2(jthexrep2){I k;
 RZ(a&&w); 
 RE(k=i0(a)); if(10<=k)k-=8;
 ASSERT(k<=0||k<=3,EVDOMAIN);
 R hrep((B)(k&1),(B)(2<=k),w);
}    /* a 3!:3 w */


static S jtunh(J jt,C c){
 if('0'<=c&&c<='9')R c-'0';
 if('a'<=c&&c<='f')R 10+c-'a';
 ASSERT(0,EVDOMAIN);
}

static F1(jtunhex){A z;C*u;I c,n;UC p,q,*v;
 RZ(w);
 c=*(1+AS(w));
 ASSERT(c==8||c==16,EVLENGTH);  
 n=AN(w)>>1; u=CAV(w);
 GATV0(z,LIT,n,1); v=UAV(z);
 DO(n, p=*u++; q=*u++; *v++=16*unh(p)+unh(q););
 RE(z); RETF(z);
}

static A jtunbinr(J jt,B b,B d,B pre601,I m,A w){A y,z;C*u=(C*)w,*v;I e,j,kk,n,p,r,*s,t,*vv;
 ASSERT(m>BH(d),EVLENGTH);
 RZ(mvw((C*)&t,BTX(d,pre601,w),1L,BU,b,SY_64,d));
 RZ(mvw((C*)&n,BN(d,w),1L,BU,b,SY_64,d));
 RZ(mvw((C*)&r,BR(d,w),1L,BU,b,SY_64,d)); 
 kk=WS(d); v=BV(d,w,r);
 ASSERT((t==LOWESTBIT(t))&&t&(B01|INT|FL|CMPX|BOX|XNUM|RAT|LIT|C2T|C4T|SB01|SLIT|SINT|SFL|SCMPX|SBOX|SBT),EVDOMAIN);
 ASSERT(0<=n,EVDOMAIN);
 ASSERT(0<=r&&r<=RMAX,EVRANK);
 p=bsize(jt,d,0,t,n,r,0L); e=t&RAT?n+n:t&SPARSE?1+sizeof(P)/SZI:n; 
 ASSERT(m>=p,EVLENGTH);
 if(t&DENSE){GA(z,t,n,r,0)}else{GASPARSE(z,t,n,r,(I*)0)} s=AS(z);
 RZ(mvw((C*)s,BS(d,w),r,BU,b,SY_64,d)); 
 j=1; DO(r, ASSERT(0<=s[i],EVLENGTH); if(t&DENSE)j*=s[i];); 
 ASSERT(j==n,EVLENGTH);
 if(t&BOX+XNUM+RAT+SPARSE){GATV0(y,INT,e,1); vv=AV(y); RZ(mvw((C*)vv,v,e,BU,b,SY_64,d));}
 if(t&BOX+XNUM+RAT){A*zv=AAV(z);I i,k=0,*iv;
  RZ(y=indexof(y,y)); iv=AV(y);
  for(i=0;i<e;++i){
   j=vv[i]; 
   ASSERT(0<=j&&j<m,EVINDEX);
   if(i>iv[i])zv[i]=zv[iv[i]];
   else{while(k<e&&j>=vv[k])++k; zv[i]=rifvs(unbinr(b,d,pre601,k<e?vv[k]-j:m-j,(A)(u+j)));}
 }}else if(t&SPARSE){P*zp=PAV(z);
  j=vv[1]; ASSERT(0<=j&&j<m,EVINDEX); SPB(zp,a,unbinr(b,d,pre601,vv[2]-j,(A)(u+j)));
  j=vv[2]; ASSERT(0<=j&&j<m,EVINDEX); SPB(zp,e,unbinr(b,d,pre601,vv[3]-j,(A)(u+j)));
  j=vv[3]; ASSERT(0<=j&&j<m,EVINDEX); SPB(zp,i,unbinr(b,d,pre601,vv[4]-j,(A)(u+j)));
  j=vv[4]; ASSERT(0<=j&&j<m,EVINDEX); SPB(zp,x,unbinr(b,d,pre601,m    -j,(A)(u+j)));
 }else if(n)switch(CTTZNOFLAG(t)){
  case B01X:  {B c,*zv=BAV(z); DO(n, c=v[i]; ASSERT(c==C0||c==C1,EVDOMAIN); zv[i]=c;);} break; 
  case SBTX:
  case INTX:  RZ(mvw(CAV(z),v,n,  BU,b,SY_64,d)); break;
  case FLX:   RZ(mvw(CAV(z),v,n,  BU,b,1,    1)); break;
  case CMPXX: RZ(mvw(CAV(z),v,n+n,BU,b,1,    1)); break;
  default:   e=n<<bplg(t); ASSERTSYS(e<=allosize(z),"unbinr"); MC(CAV(z),v,e);
 }
 RE(z); RETF(z);
}    /* b iff reverse the bytes; d iff argument is 64-bits */

F1(jtunbin){A q;B b,d;C*v;I c,i,k,m,n,r,t;
 RZ(w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 if(2==AR(w))RZ(w=unhex(w));
 ASSERT(1==AR(w),EVRANK);
 m=AN(w);
 ASSERT(m>=8,EVLENGTH);
 q=(A)AV(w);
 switch(*CAV(w)){
  case (C)0xe0: R unbinr(0,0,0,m,q);
  case (C)0xe1: R unbinr(1,0,0,m,q);
  case (C)0xe2: R unbinr(0,1,0,m,q);
  case (C)0xe3: R unbinr(1,1,0,m,q);
 }
 /* code to handle pre 601 headers */
 d=1; v=8+CAV(w); DO(8, if(CFF!=*v++){d=0; break;});       /* detect 64-bit        */
 ASSERT(m>=1+BH(d),EVLENGTH);
 b=0;
 if(!mvw((C*)&t,BTX(d,1,q),1L,BU,0,SY_64,d)){RESETERR; b=1;} /* detect reverse bytes */
 if(!mvw((C*)&n,BN(d,q),1L,BU,0,SY_64,d)){RESETERR; b=1;}
 if(!mvw((C*)&r,BR(d,q),1L,BU,0,SY_64,d)){RESETERR; b=1;}
 b=b||!(t&NOUN&&0<=n&&0<=r&&(r||1==n)&&m>=BH(d)+r*WS(d));
 if(t&DENSE){
  v=BS(d,q); c=1;
  for(i=0;!b&&i<r;++i){
   if(!mvw((C*)&k,v,1L,BU,0,SY_64,d)){RESETERR; b=1;}
   v+=WS(d); c*=k;
   if(!(0<=k&&(!n||0<=c&&k<=n&&c<=n)))b=1;
  }
  b=b||n!=c;
 }
 R unbinr(b,d,1,m,q);
}    /* 3!:2 w, inverse for binrep/hexrep */


F2(jtic2){A z;I j,m,n,p,*v,*x,zt;I4*y;UI4*y1;S*s;U short*u;
 RZ(a&&w);
 ASSERT(1>=AR(w),EVRANK);
 n=AN(w);
 RE(j=i0(a));
#if SY_64
 ASSERT(ABS(j)<=4,EVDOMAIN);
#else
 ASSERT(ABS(j)<=2,EVDOMAIN);
#endif
// long way p=4==j||-4==j?4:3==j||-3==j?8:2==j||-2==j?4:2;
 p=ABS(j); p+=(I )(p==0)-((p&4)>>1);   // p becomes (|j){1 1 2 3 2
 if(0<j){m=n<<p; zt=LIT; if(!(INT&AT(w)))RZ(w=cvt(INT,w));}
 else   {m=n>>p; zt=INT; ASSERT(!n||LIT&AT(w),EVDOMAIN); ASSERT(!(n&((((I)1)<<p)-1)),EVLENGTH);} 
 GA(z,zt,m,1,0); v=AV(z); x=AV(w); 
 switch(j){
  default: ASSERT(0,EVDOMAIN);
  case -4: y1=(UI4*)x;    DO(m, *v++=    *y1++;); {RETF(z);}
  case  4: y1=(UI4*)v;    DO(n, *y1++=(UI4)*x++;); {RETF(z);}
  case -3: ICPY(v,x,m); {RETF(z);}
  case  3: MC(v,x,m);   {RETF(z);}
  case -2: y=(I4*)x;      DO(m, *v++=    *y++;); {RETF(z);}
  case  2: y=(I4*)v;      DO(n, *y++=(I4)*x++;); {RETF(z);}
  case -1: s=(S*)x;       DO(m, *v++=    *s++;); {RETF(z);}
  case  1: s=(S*)v;       DO(n, *s++=(S) *x++;); {RETF(z);}
  case  0: u=(U short*)x; DO(m, *v++=    *u++;); {RETF(z);}
}}

F2(jtfc2){A z;D*x,*v;I j,m,n,p,zt;float*s;
 RZ(a&&w);
 ASSERT(1>=AR(w),EVRANK);
 n=AN(w);
 RE(j=i0(a));
 p=2==j||-2==j?LGSZD:2;
 if(0<j){m=n<<p; zt=LIT; if(!(FL&AT(w)))RZ(w=cvt(FL,w));}
 else   {m=n>>p; zt=FL; ASSERT(!n||LIT&AT(w),EVDOMAIN); ASSERT(!(n&((((I)1)<<p)-1)),EVLENGTH);} 
 GA(z,zt,m,1,0); v=DAV(z); x=DAV(w);
 switch(j){
  default: ASSERT(0,EVDOMAIN);
  case -2: MC(v,x,n); {RETF(z);}
  case  2: MC(v,x,m); {RETF(z);}
  case -1: s=(float*)x; DO(m, *v++=       *s++;); {RETF(z);}
  case  1: s=(float*)v; DO(n, *s++=(float)*x++;); {RETF(z);}
}}


static B jtisnanq(J jt,A w){A q,*u,x,x1,*xv,y,*yv;D*v;I m,n,t,top;
 RZ(w);
 GATV0(x,INT,BOX&AT(w)?2*AN(w):1,1); xv=AAV(x);
 *xv=w; top=1;
 while(top){
  --top; y=xv[top]; n=AN(y); t=AT(y);
  if(t&FL+CMPX){v=DAV(y); DO(t&CMPX?n+n:n, if(_isnan(*v++))R 1;);}
  else if(t&BOX){
   m=top+n; yv=AAV(y); 
   if(m>AN(y)){GATV0(x1,INT,2*m,1); u=AAV(x1); ICPY(u,xv,top); fa(x); x=x1; xv=u;}
   u=xv+top; DO(n, q=yv[i]; if(AT(q)&FL+CMPX+BOX)*u++=q;); top=u-xv;
 }}
 R 0;
}

F1(jtisnan){A*wv,z;B*u;D*v;I n,t;
 RZ(w);
 n=AN(w); t=AT(w);
 ASSERT(t&DENSE,EVNONCE);
 GATV(z,B01,n,AR(w),AS(w)); u=BAV(z);
 if     (t&FL  ){v=DAV(w); DO(n, *u++=_isnan(*v++););}
 else if(t&CMPX){v=DAV(w); DO(n, *u++=_isnan(*v)||_isnan(*(v+1)); v+=2;);}
 else if(t&BOX ){wv=AAV(w);  DO(n, *u++=isnanq(wv[i]);); RE(0);}
 else memset(u,C0,n);
 RETF(z);
}


F1(jtbit1){A z;B*wv;BT*zv;I c,i,j,n,p,q,r,*s;UI x,y;
 RZ(w);
 if(!(B01&AT(w)))RZ(w=cvt(B01,w));
 n=AN(w); r=AR(w); wv=BAV(w); s=AS(w);
 GA(z,BIT,n,AR(w),AS(w)); zv=(BT*)AV(z);
 if(!r)*zv=*wv?'\200':0;
 else if(n){
  c=8*SZI; 
  i=s[r-1]; r= p=n/i; q=i/c; r=i-c*q;
  for(i=0;i<p;++i){
   for(j=0;j<q;++j){
    x=0; y=1+(UI)IMAX; 
    DO(c, if(*wv++)x^=y; y>>=1;); 
    *zv++=x;
   }
    x=0; y=1+(UI)IMAX; 
    DO(r, if(*wv++)x^=y; y>>=1;); 
    *zv++=x;
  }
 }
 R z;
}    /* convert byte booleans to bit booleans */

F2(jtbit2){
 ASSERT(0,EVNONCE);
}    /* convert byte booleans to bit booleans */
