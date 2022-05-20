/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Binary Representation                                            */

#include "j.h"
#include "x.h"

#ifdef MMSC_VER
#pragma warning(disable: 4101)
#endif

// conversion from internal type to the result in 3!:x, which matches the published types
static I toonehottype(I t){t<<=REPSGN(t)&10; R t&-t;}  // if sparse, shift type to sparse output position.  Return only the one bit
static I fromonehottype(I t){t&=-t; I issp=REPSGN(-(t&0xfc00)); R (t>>(issp&10))|(issp&SPARSE);}

F1(jtstype){ARGCHK1(w); R sc(toonehottype(AT(w)));}
     /* 3!:0 w */

// a is integer atom or list, values indicating the desired result
// atom values in x: 0=NJA, others reserved
F2(jtnouninfo2){A z;
 ARGCHK2(a,w);
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

static A jtbrep(J jt,B b,B d,A w);  // forward declaration
static A jthrep(J jt,B b,B d,A w);

// d & tb are something from the user
// t, n, r, s are from w (or INT if sparse)
// size is rounded to integral # words
static I bsize(J jt,B d,B tb,I t,I n,I r,I*s){I k,w,z;
 w=WS(d);
 z=BH(d)+w*r;
 k=t&INT+SBT+BOX+XNUM?w:t&RAT?w+w:bp(t); 
 R z+((n*k+(tb&&t&LAST0)+w-1)&(-w));
}   /* size in byte of binary representation, rounded up to even # words */

// like bsize, but recursive.  Add up the size of this block and the sizes of the descendants
// size is rounded to integral # words
static I bsizer(J jt,B d,B tb,A w){A *wv=AAV(w);
 I totalsize = bsize(jt,d,tb,AT(w),AN(w),AR(w),AS(w));
 if((AT(w)&DIRECT)>0)R totalsize;
 I nchildren = AN(w); nchildren<<=((AT(w)>>RATX)&1);  // # subblocks
 DO(nchildren, totalsize+=bsizer(jt,d,tb,C(wv[i]));)
 R totalsize;
}


#define MVCS(a,b,c,d)  (8*(a)+4*(b)+2*(c)+(d))

/* n:  # of words                */
/* v:  ptr to result             */
/* u:  ptr to argument           */
/* bv: 1 iff v is little-endian  */
/* bu: 1 iff u is little-endian  */
/* dv: 1 iff v is 64-bit         */
/* du: 1 iff u is 64-bit         */
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

// move the header, return new move point
static C*jtbrephdrq(J jt,B b,B d,A w,C *q){I f,r;I extt = toonehottype(AT(w));
  r=AR(w); f=0;
 RZ(mvw(BF(d,q),(C*)&f,    1L,b,BU,d,SY_64)); *q=d?(b?0xe3:0xe2):(b?0xe1:0xe0);
 RZ(mvw(BT(d,q),(C*)&extt,1L,b,BU,d,SY_64));
 RZ(mvw(BN(d,q),(C*)&AN(w),1L,b,BU,d,SY_64));
 RZ(mvw(BR(d,q),(C*)&r,1L,b,BU,d,SY_64));  // r is an I
 RZ(mvw(BS(d,q),(C*) AS(w),r, b,BU,d,SY_64));
 R BV(d,q,r);
}

// move the header for block w to the preallocated block y
static C*jtbrephdr(J jt,B b,B d,A w,A y){R jtbrephdrq(jt,b,d,w,CAV(y));}

static A jtbreps(J jt,B b,B d,A w){A q,y,z,*zv;C*v;I c=0,kk,m,n;P*wp;
 wp=PAV(w);
 n=1+sizeof(P)/SZI; kk=WS(d);
 GATV0(z,BOX,n,1); zv=AAV(z);
 GATV0(y,LIT,bsize(jt,d,1,INT,n,AR(w),AS(w)),1);
 v=brephdr(b,d,w,y);
 RZ(mvw(v,(C*)&c,1L,BU,b,d,SY_64));  /* reserved for flag */
 zv[0]=incorp(y); m=AN(y);
 RZ(zv[1]=q=incorp(brep(b,d,SPA(wp,a)))); RZ(mvw(v+  kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q);
 RZ(zv[2]=q=incorp(brep(b,d,SPA(wp,e)))); RZ(mvw(v+2*kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q);
 RZ(zv[3]=q=incorp(brep(b,d,SPA(wp,i)))); RZ(mvw(v+3*kk,(C*)&m,1L,b,BU,d,SY_64)); m+=AN(q);
 RZ(zv[4]=q=incorp(brep(b,d,SPA(wp,x)))); RZ(mvw(v+4*kk,(C*)&m,1L,b,BU,d,SY_64));
 R raze(z);
}    /* 3!:1 w for sparse w */

// copy data into bin/hex rep
static C* jtbrepfill(J jt,B b,B d,A w,C *zv){I klg,kk;
 C *origzv=zv;  // remember start of block
 zv=jtbrephdrq(jt,b,d,w,zv);   // fill in the header, advance pointer to after it
 C* u=CAV(w);  // input pointer
 I n=AN(w);  // #input atoms
 I t=AT(w);  // input type
 klg=bplg(t); kk=WS(d);
 if((t&DIRECT)>0){
  I blksize=bsizer(jt,d,1,w);  // get size of this block (never needs recursion)
  switch(CTTZ(t)){
  case SBTX:
  case INTX:  RZ(mvw(zv,u,n,  b,BU,d,SY_64)); break;
  case FLX:   RZ(mvw(zv,u,n,  b,BU,1,1    )); break;
  case CMPXX: RZ(mvw(zv,u,n+n,b,BU,1,1    )); break;
  default:
   // 1- and 2-byte C4T types, all of which have LAST0.  We need to clear the last
   // bytes, because the datalength is rounded up in bsize, and thus there are
   // up to 3 words at the end of y that will not be copied to.  We clear them to
   // 0 to provide repeatable results.
   // Make sure there is a zero byte if the string is empty
   {I suffsize = MIN(4*SZI,origzv+blksize-zv);  // len of area to clear to 0 
   mvc(suffsize,(origzv+blksize)-suffsize,1,MEMSET00);   // clear suffix
   MC(zv,u,n<<klg); break;}      // copy the valid part of the data
  }
  R origzv+blksize;  // return next output position
 }
 // Here for non-DIRECT values.  These recur through the boxes
 // we have already written the header for the indirect block.  Skip over the indirect pointers; they will be replaced by
 // offsets to each block
 A *wv=AAV(w); 
 n<<=(t>>RATX)&1;  // if RAT, double the number of indirects
 C* zvx=zv; zv += n*kk;  // save start of index, step over index
 // move in the blocks: first the offset, writing over the indirect block, then the data
 // the offsets are all relative to the start of the block, which is origzv
 DO(n, I offset=zv-origzv; RZ(mvw(zvx,(C*)&offset,1L,b,BU,d,SY_64)); zvx+=kk; RZ(zv=jtbrepfill(jt,b,d,C(wv[i]),zv));)
 R zv;
}    /* b iff reverse the bytes; d iff 64-bit */

// main entry point for brep.  First calculate the size by a (recursive) call; allocate; then make a (recursive) call to fill in the block
static A jtbrep(J jt,B b,B d,A w){A y;I t;
 ARGCHK1(w);
 t=AT(w); 
 if(unlikely(ISSPARSE(t)))R breps(b,d,w);  // sparse separately
 GATV0(y,LIT,bsizer(jt,d,1,w),1);   // allocate entire result
 RZ(jtbrepfill(jt,b,d,w,CAV(y)));   // fill it
 R y;  // return it
}

#if 0
static A jthrep(J jt,B b,B d,A w){A y,z;C c,*hex="0123456789abcdef",*u,*v;I n,s[2];
 RZ(y=brep(b,d,w));
 n=AN(y); s[0]=n>>LGWS(d); s[1]=2*WS(d); 
 GATVR(z,LIT,2*n,2,s);  
 u=CAV(y); v=CAV(z); 
 DQ(n, c=*u++; *v++=hex[(c&0xf0)>>4]; *v++=hex[c&0x0f];); 
 RETF(z);
}
#else
static A jthrep(J jt,B b,B d,A w){A y;C c,*hex="0123456789abcdef",*u,*v;I n,s[2],t;
 ARGCHK1(w);
 t=AT(w); 
 if(unlikely(ISSPARSE(t))){A z;  // sparse separately
  RZ(y=breps(b,d,w));
  n=AN(y); s[0]=n>>LGWS(d); s[1]=2*WS(d); 
  GATVR(z,LIT,2*n,2,s);  
  u=CAV(y); v=CAV(z); 
  DQ(n, c=*u++; *v++=hex[(c&0xf0)>>4]; *v++=hex[c&0x0f];); 
  RETF(z);
 }
 n=bsizer(jt,d,1,w); s[0]=n>>LGWS(d); s[1]=2*WS(d); 
 GATVR(y,LIT,2*n,2,s);   // allocate entire result
 RZ(jtbrepfill(jt,b,d,w,n+CAV(y)));   // fill brep starting from the middle of buffer
 u=n+CAV(y); v=CAV(y); 
 DQ(n, c=*u++; *v++=hex[(c&0xf0)>>4]; *v++=hex[c&0x0f];);   // in-place translation to hex
 R y;  // return it
}
#endif

F1(jtbinrep1){ARGCHK1(w); ASSERT(NOUN&AT(w),EVDOMAIN); R brep(BU,SY_64,w);}  /* 3!:1 w */
F1(jthexrep1){ARGCHK1(w); ASSERT(NOUN&AT(w),EVDOMAIN); R hrep(BU,SY_64,w);}  /* 3!:3 w */

F2(jtbinrep2){I k;
 ARGCHK2(a,w);
 RE(k=i0(a)); if(10<=k)k-=8;
 ASSERT(BETWEENC(k,0,3),EVDOMAIN);
 R brep((B)(k&1),(B)(2<=k),w);
}    /* a 3!:1 w */

F2(jthexrep2){I k;
 ARGCHK2(a,w); 
 RE(k=i0(a)); if(10<=k)k-=8;
 ASSERT(BETWEENC(k,0,3),EVDOMAIN);
 R hrep((B)(k&1),(B)(2<=k),w);
}    /* a 3!:3 w */


static S jtunh(J jt,C c){
 if(BETWEENC(c,'0','9'))R c-'0';
 if(BETWEENC(c,'a','f'))R 10+c-'a';
 ASSERT(0,EVDOMAIN);
}

static F1(jtunhex){A z;C*u;I c,n;UC p,q,*v;
 ARGCHK1(w);
 c=AS(w)[1];
 ASSERT(c==8||c==16,EVLENGTH);  
 n=AN(w)>>1; u=CAV(w);
 GATV0(z,LIT,n,1); v=UAV(z);
 DQ(n, p=*u++; q=*u++; *v++=16*unh(p)+unh(q););
 RE(z); RETF(z);
}

// create A from 3!:1 form
static A jtunbinr(J jt,B b,B d,B pre601,I m,A w){A y,z;C*u=(C*)w,*v;I e,j,kk,n,p,r,*s,t,*vv;
 ASSERT(m>BH(d),EVLENGTH);
 RZ(mvw((C*)&t,BTX(d,pre601,w),1L,BU,b,SY_64,d));
 RZ(mvw((C*)&n,BN(d,w),1L,BU,b,SY_64,d));
 RZ(mvw((C*)&r,BR(d,w),1L,BU,b,SY_64,d)); 
 kk=WS(d); v=BV(d,w,r);
 ASSERT(t==LOWESTBIT(t),EVDOMAIN);
 t=fromonehottype(t);
 ASSERT(t&NOUN,EVDOMAIN);
 ASSERT(0<=n,EVDOMAIN);
 ASSERT(BETWEENC(r,0,RMAX),EVRANK);
 p=bsize(jt,d,0,t,n,r,0L); e=t&RAT?n+n:ISSPARSE(t)?1+sizeof(P)/SZI:n; 
 ASSERT(m>=p,EVLENGTH);
 if(likely(!ISSPARSE(t))){GA00(z,t,n,r)}else{GASPARSE0(z,t,n,r)} s=AS(z);
 RZ(mvw((C*)s,BS(d,w),r,BU,b,SY_64,d)); 
 j=1; DO(r, ASSERT(0<=s[i],EVLENGTH); if(!ISSPARSE(t))j*=s[i];); 
 ASSERT(j==n,EVLENGTH);
 if(t&BOX+XNUM+RAT+SPARSE){GATV0(y,INT,e,1); vv=AV(y); RZ(mvw((C*)vv,v,e,BU,b,SY_64,d));}
 if(t&BOX+XNUM+RAT){A*zv=AAV(z);I i,k=0,*iv;
  RZ(y=indexof(y,y)); iv=AV(y);
  for(i=0;i<e;++i){
   j=vv[i]; 
   ASSERT(BETWEENO(j,0,m),EVINDEX);
   if(i>iv[i])zv[i]=zv[iv[i]];
   else{while(k<e&&j>=vv[k])++k; zv[i]=incorp(unbinr(b,d,pre601,k<e?vv[k]-j:m-j,(A)(u+j)));}
 }}else if(unlikely(ISSPARSE(t))){P*zp=PAV(z);
  j=vv[1]; ASSERT(BETWEENO(j,0,m),EVINDEX); SPB(zp,a,unbinr(b,d,pre601,vv[2]-j,(A)(u+j)));
  j=vv[2]; ASSERT(BETWEENO(j,0,m),EVINDEX); SPB(zp,e,unbinr(b,d,pre601,vv[3]-j,(A)(u+j)));
  j=vv[3]; ASSERT(BETWEENO(j,0,m),EVINDEX); SPB(zp,i,unbinr(b,d,pre601,vv[4]-j,(A)(u+j)));
  j=vv[4]; ASSERT(BETWEENO(j,0,m),EVINDEX); SPB(zp,x,unbinr(b,d,pre601,m    -j,(A)(u+j)));
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
 ARGCHK1(w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 if(2==AR(w))RZ(w=unhex(w));
 ASSERT(1==AR(w),EVRANK);
 m=AN(w);
 ASSERT(m>=8,EVLENGTH);
 q=(A)AV(w);
 switch(CAV(w)[0]){
  case (C)0xe0: R unbinr(0,0,0,m,q);
  case (C)0xe1: R unbinr(1,0,0,m,q);
  case (C)0xe2: R unbinr(0,1,0,m,q);
  case (C)0xe3: R unbinr(1,1,0,m,q);
 }
 /* code to handle pre 601 headers */
 d=1; v=8+CAV(w); DQ(8, if(CFF!=*v++){d=0; break;});       /* detect 64-bit        */
 ASSERT(m>=1+BH(d),EVLENGTH);
 b=0;
 if(!mvw((C*)&t,BTX(d,1,q),1L,BU,0,SY_64,d)){RESETERR; b=1;} /* detect reverse bytes */
 if(!mvw((C*)&n,BN(d,q),1L,BU,0,SY_64,d)){RESETERR; b=1;}
 if(!mvw((C*)&r,BR(d,q),1L,BU,0,SY_64,d)){RESETERR; b=1;}
 b=b||!(t&NOUN&&0<=n&&0<=r&&(r||1==n)&&m>=BH(d)+r*WS(d));
 if(!ISSPARSE(t)){
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

// 3!:4
F2(jtic2){A z;I j,m,n,p,*v,*x,zt;I4*y;UI4*y1;S*s;U short*u;
 ARGCHK2(a,w);
 ASSERT(1>=AR(w),EVRANK);
 n=AN(w);
 RE(j=i0(a));
#if SY_64
 ASSERT(ABS(j)<=4,EVDOMAIN);
#else
 ASSERT(ABS(j)<=2,EVDOMAIN);
#endif
// p=4==j||-4==j?4:3==j||-3==j?8:2==j||-2==j?4:2;
 p=ABS(j); p+=(I )(p==0)-((p&4)>>1);   // p becomes (|j){1 1 2 3 2
 if(0<j){m=n<<p; zt=LIT; if(!ISDENSETYPE(AT(w),INT))RZ(w=cvt(INT,w));}
 else   {m=n>>p; zt=INT; ASSERT(!n||ISDENSETYPE(AT(w),LIT),EVDOMAIN); ASSERT(!(n&((((I)1)<<p)-1)),EVLENGTH);} 
 GA10(z,zt,m); v=AV(z); x=AV(w); 
 switch(j){
  default: ASSERT(0,EVDOMAIN);
  case -4: y1=(UI4*)x;    DQ(m, *v++=    *y1++;); {RETF(z);}
  case  4: y1=(UI4*)v;    DQ(n, *y1++=(UI4)*x++;); {RETF(z);}
  case -3: ICPY(v,x,m); {RETF(z);}
  case  3: MC(v,x,m);   {RETF(z);}
  case -2: y=(I4*)x;      DQ(m, *v++=    *y++;); {RETF(z);}
  case  2: y=(I4*)v;      DQ(n, *y++=(I4)*x++;); {RETF(z);}
  case -1: s=(S*)x;       DQ(m, *v++=    *s++;); {RETF(z);}
  case  1: s=(S*)v;       DQ(n, *s++=(S) *x++;); {RETF(z);}
  case  0: u=(U short*)x; DQ(m, *v++=    *u++;); {RETF(z);}
}}

// 3!:5
F2(jtfc2){A z;D*x,*v;I j,m,n,p,zt;float*s;
 ARGCHK2(a,w);
 ASSERT(1>=AR(w),EVRANK);
 n=AN(w);
 RE(j=i0(a));
 p=2==j||-2==j?LGSZD:2;
 if(0<j){m=n<<p; zt=LIT; if(!ISDENSETYPE(AT(w),FL))RZ(w=cvt(FL,w));}
 else   {m=n>>p; zt=FL; ASSERT(!n||ISDENSETYPE(AT(w),LIT),EVDOMAIN); ASSERT(!(n&((((I)1)<<p)-1)),EVLENGTH);} 
 GA10(z,zt,m); v=DAV(z); x=DAV(w);
 switch(j){
  default: ASSERT(0,EVDOMAIN);
  case -2: MC(v,x,n); {RETF(z);}
  case  2: MC(v,x,m); {RETF(z);}
  case -1: s=(float*)x; DQ(m, *v++=       *s++;); {RETF(z);}
  case  1: s=(float*)v; DQ(n, *s++=(float)*x++;); {RETF(z);}
}}

// w is a box, result is 1 if it contains a  NaN
static B jtisnanq(J jt,A w){
 ARGCHK1(w);
 if(AT(w)&FL+CMPX){D *v=DAV(w); DQ(AN(w)<<((AT(w)>>CMPXX)&1), if(_isnan(v[i]))R 1;);}  // if there might be a NaN, return if there is one
 else if(AT(w)&BOX){A *v=AAV(w); STACKCHKOFL DQ(AN(w), if(isnanq(C(v[i])))R 1;);}  // if boxed, check each one recursively; ensure no stack overflow
 // other types never have NaN
 R 0;  // if we get here, there must be no NaN
}

// 128!:5  Result is boolean with same shape as w
F1(jtisnan){A*wv,z;B*u;D*v;I n,t;
 ARGCHK1(w);
 n=AN(w); t=AT(w);
 ASSERT(!ISSPARSE(t),EVNONCE);
 GATV(z,B01,n,AR(w),AS(w)); u=BAV(z);
 if (t&FL){v=DAV(w); DQ(n, *u++=_isnan(*v++););}  // float - check each atom
 else if(t&CMPX){v=DAV(w); DQ(n, *u++=_isnan(v[0])|_isnan(v[1]); v+=2;);}  // complex - check each half
 else if(t&BOX){wv=AAV(w); DO(n, *u++=isnanq(C(wv[i]));); RE(0);}  // boxed - check contents
 else mvc(n,u,1,MEMSET00);  // other types are never NaN
 RETF(z);
}


/* Copyright 2014, Jsoftware Inc.  All rights reserved. */
// datetime epoch routines

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MINY  1800  // minimum year allowed
#define MAXY  2200  // maximum year allowed
#define MIND  (I)73048    // number of days from epoch (20000101) to minimum day allowed 18000101
#define MAXD  (I)73414    // number of days from epoch (20000101) to first day not allowed 22010101 - must be >= MIND for range calculation to be accurate
#define BASE  946684800
#define NANOS 1000000000LL
#define SECS  86400

// e from yyyymmddhhmnss.  The argument is assumed to be well-formed
static I eft(I n,UI* e,UI* t)
{
#if SY_64
	I i; UI4 kk,M,Y,D; UI k,hh,mm,ss;  // use unsigned to make / and % generate better code
	for(i=0;i<n;++i){
	 k=t[i];  // read the yyyymmddhhmnss value
	 ss=k%100U; k=k/100U;  // ss yyyymmddhhmn
	 mm=k%100U; k=k/100U;  // ss mn yyyymmddhh
	 hh=k%100U; kk=(UI4)(k/100U);  // ss mn hh yyyymmdd.  yyyymmdd fits in UI4, so switch to that (faster /, %)
	 D=kk%100U; kk=kk/100U;  // ss mn hh D yyyymm
	 M=kk%100U;   // ss mn hh D M
  Y=kk/100U;  // ss mn hh D M Y

  // Now calculate number of days from epoch.  First reorder months so that the irregular February comes last, i. e. make the year start Mar 1
  UI4 janfeb=(I4)(M-3)>>(32-1);   // -1 if jan/feb
  Y+=janfeb; M+=janfeb&12;  // if janfeb, subtract 1 from year and add 12 to month
  // Add in leap-years (since the year 0, for comp. ease).  Year 2000 eg, which starts Mar 1, is a leap year and has 1 added to its day#s (since they come after Feb 29)
  D+=Y>>2;
  // Gregorian correction.  Since it is very unlikely we will encounter a date that needs correcting, we use an IF
  if((UI4)(Y-1901)>(UI4)(2100-1901)){  // date is outside 1901-2099
   D+=(((Y/100)>>2)-(Y/100))-((2000/400)-(2000/100));  // 1900 2100 2200 2300 2500 etc are NOT leapyears.  Create correction from Y2000 count
  }
  // Add in extra days for earlier 31-day months in this adjusted year (so add 0 in March)
  D+=(0x765544322110000>>(4*M))&0xf;  // starting with month 0, this is x x x 0 1 1 2 2 3 4 4 5 5 6 7
  // Calculate day from YMD.  Bias from day# of 20000101, accounting for leap-years from year 0 to that date.  Note 20000101 is NOT in a leapyear - it is in year 1999 here
  // The bias includes: subtracting 1 from day#; subtracting 1 from month#; Jan/Feb of 1999; Gregorian leapyears up to 2000
  I temp=(I)(365*Y + 30*M + D) - 730531;  // day# from epoch - can be negative
  // Combine everything into one # and store
 	e[i]=(NANOS*24LL*60LL*60LL)*temp + (NANOS*3600LL)*hh + (NANOS*60LL)*mm + NANOS*ss;  // eschew Horner's Rule because of multiply latency
	}
#endif
	return 0;
}


static const UC char2tbl[200] = {
'0','0' , '0','1' , '0','2' , '0','3' , '0','4' , '0','5' , '0','6' , '0','7' , '0','8' , '0','9' ,
'1','0' , '1','1' , '1','2' , '1','3' , '1','4' , '1','5' , '1','6' , '1','7' , '1','8' , '1','9' ,
'2','0' , '2','1' , '2','2' , '2','3' , '2','4' , '2','5' , '2','6' , '2','7' , '2','8' , '2','9' ,
'3','0' , '3','1' , '3','2' , '3','3' , '3','4' , '3','5' , '3','6' , '3','7' , '3','8' , '3','9' ,
'4','0' , '4','1' , '4','2' , '4','3' , '4','4' , '4','5' , '4','6' , '4','7' , '4','8' , '4','9' ,
'5','0' , '5','1' , '5','2' , '5','3' , '5','4' , '5','5' , '5','6' , '5','7' , '5','8' , '5','9' ,
'6','0' , '6','1' , '6','2' , '6','3' , '6','4' , '6','5' , '6','6' , '6','7' , '6','8' , '6','9' ,
'7','0' , '7','1' , '7','2' , '7','3' , '7','4' , '7','5' , '7','6' , '7','7' , '7','8' , '7','9' ,
'8','0' , '8','1' , '8','2' , '8','3' , '8','4' , '8','5' , '8','6' , '8','7' , '8','8' , '8','9' ,
'9','0' , '9','1' , '9','2' , '9','3' , '9','4' , '9','5' , '9','6' , '9','7' , '9','8' , '9','9'
};

static const I nanopowers[9] = {100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

// Convert nanosec to ISO8601
// w is input array, decimalpt is character to use if there are fractional sec, zuluflag is timezone character
// prec is -1 for date only, 0 for integer seconds, 1-9 for that many fractional seconds places
// prec of 7*SZI-21 means 'produce 7 ints per input time'
static A sfe(J jt,A w,I prec,UC decimalpt,UC zuluflag){
#if SY_64
	UI k; UI4 ymd,E,N,M,HMS,d,j,g,m,t,y;I i;A z;  // unsigned for faster / %
 // Validate input.  We will accept FL input, but it's not going to have nanosecond precision
 RZ(w=vi(w));  // convert to INT
 // Figure out size of result. 10 for date, 9 for time, 1 for binary point (opt), 1 for each fractional digit (opt), 1 for timezone
 I linelen=(10+9)-(REPSGN(prec)&9)+prec+(prec!=0)+(zuluflag=='Z');  // bytes per line of result: 20, but 10 if no date, plus one per frac digit, plus decimal point if any frac digits, 1 if Z
   // if we are running for 6!:15, linelen will come out 56 and the store will be 7 INTs
 // Allocate result area, one row per input value
 GATV0(z,LIT,AN(w)*linelen,AR(w)+1) MCISH(AS(z),AS(w),AR(w)) AS(z)[AR(w)]=linelen==7*SZI?7:linelen;
 // If the result will be INT, make it so
 if(linelen==56){AT(z)=INT; AN(z)>>=LGSZI;}
 if(AN(w)==0){RETF(z);}  // handle empty return
 I rows=AN(w);  // number of rows to process
 I *e=IAV(w);  // pointer to nanosecond data
 C *s=CAV(z);  // pointer to result

 // Loop for each time
	for(i=0;i<rows;++i, s+=linelen){
  // fetch the time.  If it is negative, add days amounting to the earliest allowed time so that the modulus calculations can always
  // be positive to get hmsn.  We will add the days back for all the day calculations, since they are in the Julian epoch anyway
		k= e[i] + (REPSGN(e[i])&(MIND*(I)24*(I)3600*(I)NANOS));  // ymdHMSN
  if((UI)k>=(UI)(MAXD*(I)24*(I)3600*(I)NANOS)){if(linelen==7*SZI)DO(7, ((I*)s)[i]=0;)else{DO(linelen, s[i]=' ';) s[0]='?';} continue;}  // input too low - probably DATAFILL(=IMIN) - return fast unknown
    // we use the fact that MAXD>MIND to get the out-of-bounds test right
  N=(UI4)(k%NANOS); k=k/NANOS;  // can't fast-divide by more than 32 bits.  k=ymdHMS N=nanosec
		HMS=(UI4)(k%((I)24*(I)3600));	ymd=(UI4)(k/((I)24*(I)3600));
  ymd-=(REPSGN(e[i])&MIND);  // remove negative-year bias if given
  E=HMS%60; HMS/=60;  // sec
  M=HMS%60; HMS/=60;  // minutes; HMS now=hours
  // Now the leap-year calculations.  We follow Richards at https://en.wikipedia.org/wiki/Julian_day#Julian_or_Gregorian_calendar_from_Julian_day_number
		j=ymd+2451545;  // Julian day number, by adding day-from-epoch to JD of epoch
		g=((3*((4*j+274277)/146097))>>2)-38;  // Gregorian correction for leapyears from year 0
		j+=1401+g;  // Julian day, plus 1401 (Julian leapyears to year 0), plus Gregorian leapyears
  // now find position within 1461-day 4-year cycle
		t=4*j+3;   // temp
		y=t/1461-4716;  // year number from Julian epoch, plus starting year of Julian epoch
		t=(t%1461)>>2;  // day number within year, which starts Mar 1
		m=(t*5+461)/153;  // razzmatazz to convert day# to month, 3-14
// Richards version		d=((t*5+2)%153)/5;
  d=(t+((0x444332221100000>>(m<<2))&0xf))%31+1;  // # days   start-of-month must advance to be on 31-day multiple, by month: x x x 0(Mar) 0(Apr) 1 1 2 2 2 3 3 4 4 4
  I4 janfeb=(I4)(12-m)>>(32-1); y-=janfeb; m-=janfeb&12;  // move jan-feb into next year number
  // Now write the result yyyy-mm-ddThh:mm:ss.nnnnnnnnn
  if(linelen!=7*SZI){  // normal case of LIT output
   // Store bytes two at a time using a lookup.  The lookup fits in 4 cache lines & so should be available for most of the loop.
   // (actually, at this time only the values 0-31 will be used much, just 1 cache line)
   // This uses the load/store unit heavily but it's OK if it backs up, since it is idle during the first half of the loop
   // This ends with 5 dependent integer multiplies; hope that's OK
   *(S*)(s+0) = ((S*)char2tbl)[y/100]; *(S*)(s+2) = ((S*)char2tbl)[y%100];
   s[4]='-'; *(S*)(s+5) = ((S*)char2tbl)[m];
   s[7]='-'; *(S*)(s+8) = ((S*)char2tbl)[d];
   if(linelen>(10+1)){  // if time requested...  (remember timezone)
    s[10]='T'; *(S*)(s+11) = ((S*)char2tbl)[HMS];
    s[13]=':'; *(S*)(s+14) = ((S*)char2tbl)[M];
    s[16]=':'; *(S*)(s+17) = ((S*)char2tbl)[E];
    if(linelen>(19+1)){  // if fractional nanosec requested
     s[19]=decimalpt;
     // We have to store a variable number of high-order digits.  We will format all 9 digits and optionally
     // store what we want to keep.  To reduce the length of the multiply chain we split the nanos into 
     // NOTE we do not round the nanoseconds
     UI4 nano4=N/100000; N%=100000;   // reduce dependency
     if(linelen>24)DQ(5, I sdig=N%10; N/=10; if(linelen>24+i)s[24+i]=(C)('0'+sdig);)   // the branches should predict correctly after a while
     DQ(4, I sdig=nano4%10; nano4/=10; if(linelen>20+i)s[20+i]=(C)('0'+sdig);)
    }
   }
   if(zuluflag=='Z')s[linelen-1]=zuluflag;
  }else{
   // 6!:15, store results as INTs
   ((I*)s)[0]=y; ((I*)s)[1]=m; ((I*)s)[2]=d; ((I*)s)[3]=HMS; ((I*)s)[4]=M; ((I*)s)[5]=E; ((I*)s)[6]=N;
  }
 }
	RETF(z);
#else
R 0;
#endif
}


// w is LIT array of ISO strings (rank>0, not empty), result is array of INTs with nanosecond time for each string
// We don't bother to support a boxed-string version because the strings are shorter than the boxes & it is probably just about as good to just open the boxed strings
// prec is -1 (day only) or 0,3,9 for that many fractional digits below seconds
static A efs(J jt,A w,I prec){
#if SY_64
	I i;A z;
 // Allocate result area
 I n; PROD(n,AR(w)-1,AS(w)); GATV(z,INT,n,AR(w)-1,AS(w))
 I strglen=AS(w)[AR(w)-1];
 C *s=CAV(w);  // point to start of first string
 UC afterday=(UC)((~prec)>>8);  //  0x00 if we stop  after the day, 0xff if we continue
	for(i=0;i<n;++i,s+=strglen){
 	UI4 Y,M,D,ss; I4 hh,mm;  // hh,mm are I because they may go negative during TZ adjustment
  // It's OK to overfetch from a string buffer, as long as you don't rely on the contents fetched.  They're padded
  // We will store an invalid byte on top of the character after the end of the string.  We'll be sure to restore it!
  UC savesentinel = s[strglen]; s[strglen]=0;  // install end-of-string marker
  UC *sp=s;  // scan pointer through the string
  // Read/convert two-digit things.  Once we commit to reading digits, we fail if there aren't two of them; so check types first as needed
  // We code this on the assumption that the format is constant throughout, and therefore branches will not be mispredicted after the first loop
#define DOERR {IAV(z)[i]=IMIN; goto err;}
#define ISDIGIT(d) (((UI4)d-(UI4)'0')<=((UI4)'9'-(UI4)'0'))
// same, but use c for the first digit
#define RDTWOC(z,min,max) if(!ISDIGIT(sp[1]))DOERR z=(UI4)c*10+((UI4)sp[1]-(UI4)'0')-((UI4)'0'*(UI4)10); if((UI4)(z-(min))>(UI4)((max)-(min)))DOERR sp+=2; c=*sp;
  UI N=0;  // init nanosec accum to 0
  // throughout this stretch we may have c set to 'next character'
  UC c=*sp;
  RDTWOC(Y,0,99); RDTWOC(M,0,99); Y=100*Y+M;  // fetch YYYY.  M is a temp
  if((UI4)(Y-MINY)>(UI4)(MAXY-MINY))DOERR
  if(!(c&~' ')){M=D=1; hh=mm=ss=0; goto gottime;}   // YYYY alone.  Default the rest
  if(c=='T'){M=D=1; goto gotdate;}    // YYYYT.  Default MD
  // normal case
  if(c=='-')c=*++sp;  // skip '-' if present
  RDTWOC(M,1,12);
  if(!(c&~' ')){D=1; hh=mm=ss=0; goto gottime;}   // YYYY-MM alone.  Default the rest
  if(c=='T'){D=1; goto gotdate;}    // YYYY-MMT.  Default D
  if(c=='-')c=*++sp;  // skip '-' if present
  RDTWOC(D,1,31);
  if(!(c&afterday)){hh=mm=ss=0; goto gottime;}   // YYYY-MM-DD alone, or after-day ignored.  Default the rest.  space here is a delimiter
gotdate: ;
  if((c=='T')|(c==' '))c=*++sp;  // Consume the T/sp if present.  It must be followed by HH.  sp as a separator is not ISO 8601
  if(!(c&~' ')){hh=mm=ss=0; goto gottime;}   // YYYY-MM-DDTbb treat this as ending the year, default the rest
  RDTWOC(hh,0,24);  // 24 allowed at midnight
  if((c<0x40) & (((I)1<<'+')|((I)1<<'-')|((I)1<<' ')|((I)1<<0))>>c){mm=ss=0; if((c&~' '))goto hittz; goto gottime;}
  if(c==':')c=*++sp;  // skip ':' if present
  RDTWOC(mm,0,59);
  if((c<0x40) & (((I)1<<'+')|((I)1<<'-')|((I)1<<' ')|((I)1<<0))>>c){ss=0; if((c&~' '))goto hittz; goto gottime;}
  if(c==':')c=*++sp;  // skip ':' if present
  RDTWOC(ss,0,60);  // 60 allowed for leap second
  // If the seconds have decimal extension, turn it to nanoseconds.  ISO8601 allows fractional extension on the last time component even if it's not SS, but we don't support that 
  if((c=='.')|(c==',')){
   c=*++sp;  // skip decimal point
   DO(prec, if(!ISDIGIT(c))break; N+=nanopowers[i]*((UI)c-(UI)'0'); c=*++sp;)  // harder than it looks!  We use memory to avoid long carried dependency from the multiply chain
   // discard trailing digits that we are skipping
   NOUNROLL while(ISDIGIT(c))c=*++sp;
  }
hittz:
  // Timezone [+-]HH[[:]MM]  or Z
  if((c=='+')|(c=='-')){
   I4 tzisplus=2*(c=='+')-1;   // +1 for +, -1 for -
   c=*++sp;  // skip tz indic
   I4 tzhm; RDTWOC(tzhm,0,23);
   // Apply tz adjustment to hours.  This may make hours negative; that's OK
   hh-=tzisplus*tzhm;    // +tz means UTC was advanced by tz hours; undo it
   if(c==':')c=*++sp;  // skip ':' if present
   if(ISDIGIT(c)){
    RDTWOC(tzhm,0,59);
    mm-=tzisplus*tzhm;    // same for minutes, may go negative
   }
  }else if(c=='Z')c=*++sp;  // no numbered timezone; skip Zulu timezone if given
  // Verify no significance after end
  NOUNROLL while(c){if(c!=' ')DOERR; c=*++sp;}
gottime: ;
  // We have all the components.  Combine Y M D hh mm ss N into nanosec time
  // This copies the computation in eft except that we have N here.  eft uses unsigned vbls for hh,mm, we don't - no problem

  // Now calculate number of days from epoch.  First reorder months so that the irregular February comes last, i. e. make the year start Mar 1
  UI4 janfeb=(I4)(M-3)>>(32-1);   // -1 if jan/feb
  Y+=janfeb; M+=janfeb&12;  // if janfeb, subtract 1 from year and add 12 to month
  // Add in leap-years (since the year 0, for comp. ease).  Year 2000 eg, which starts Mar 1, is a leap year and has 1 added to its day#s (since they come after Feb 29)
  D+=Y>>2;
  // Gregorian correction.  Since it is very unlikely we will encounter a date that needs correcting, we use an IF
  if(!BETWEENC(Y,1901,2100)){  // date is outside 1901-2099
   D+=(((Y/100)>>2)-(Y/100))-((2000/400)-(2000/100));  // 1900 2100 2200 2300 2500 etc are NOT leapyears.  Create correction from Y2000 count
  }
  // Add in extra days for earlier 31-day months in this adjusted year (so add 0 in March)
  D+=(0x765544322110000>>(4*M))&0xf;  // starting with month 0, this is x x x 0 1 1 2 2 3 4 4 5 5 6 7
  // Calculate day from YMD.  Bias from day# of 20000101, accounting for leap-years from year 0 to that date.  Note 20000101 is NOT in a leapyear - it is in year 1999 here
  // The bias includes: subtracting 1 from day#; subtracting 1 from month#; Jan/Feb of 1999; Gregorian leapyears up to 2000
  I t=(I)(365*Y + 30*M + D) - 730531;  // day# from epoch.  May be negative
  // Combine everything into one # and store
 	IAV(z)[i]=(NANOS*24LL*60LL*60LL)*t + (NANOS*3600LL)*hh + (NANOS*60LL)*mm + NANOS*ss + N;  // eschew Horner's Rule because of multiply latency

err:
  s[strglen]=savesentinel;  // restore end-of-string marker
 }
 RETF(z);
#else
R 0;
#endif
}




// 6!:14 Convert a block of integer yyyymmddHHMMSS to nanoseconds from year 2000
F1(jtinttoe){A z;I n;
 ARGCHK1(w);
 n=AN(w);
 ASSERT(SY_64,EVNONCE);
 RZ(w=vi(w));  // verify valid integer
 GATV(z,INT,n,AR(w),AS(w));
 eft(n,IAV(z),IAV(w));
 RETF(z);
}

// 6!:15 Convert a block of nanosecond times to Y M D h m s nanosec
F1(jtetoint){
 ARGCHK1(w);
 ASSERT(SY_64,EVNONCE);
 RETF(sfe(jt,w,7*SZI-20,0,0));  // special precision meaning 'store INTs'.  Turns into linelen=56
}

// 6!:16 convert a block of nanoseconds times to iso8601 format.  Result has an extra axis, long enough to hold the result
// Bivalent.  left arg is 3 characters, one to use as the decimal point, one to store just after the value (usually ' ' or 'Z'),
// one for result precision ('d'=date only, '0'-'9' give # fractional digits)
// Default is '. 0'
F2(jtetoiso8601){UC decimalpt,zuluflag;I prec;
 ARGCHK1(w);
 ASSERT(SY_64,EVNONCE);
 // If monad, supply defaults; if dyad, audit
 if(AT(w)&NOUN){  // dyad
  ASSERT(AT(a)&LIT,EVDOMAIN);
  ASSERT(AN(a)==3,EVLENGTH);
  ASSERT(AR(a)==1,EVRANK);  // a must be a 3-character list
  decimalpt=CAV(a)[0]; zuluflag=CAV(a)[1];
  // convert precision character to precision to use (_1 for date, 0-9)
  if(CAV(a)[2]=='d')prec=-1; else {prec=CAV(a)[2]-'0'; ASSERT((UI)prec<(UI)10,EVDOMAIN);}
 }else{
  w=a; decimalpt='.'; zuluflag=' '; prec=0;  // monad: switch argument, set defaults
 }
 RETF(sfe(jt,w,prec,decimalpt,zuluflag));
}

// 6!:17 convert a block of iso8601-format strings to nanosecond times.  Result has one INT for each string
// Bivalent.  left arg is 'd', '0', '3', or '9', like 3d digit of 6!:16, default '9'
F2(jtiso8601toe){A z;I prec;
 ARGCHK1(w);
 ASSERT(SY_64,EVNONCE);
 // If monad, supply defaults; if dyad, audit
 if(AT(w)&NOUN){  // dyad
  ASSERT(AT(a)&LIT,EVDOMAIN);
  ASSERT(AN(a)==1,EVLENGTH);
  ASSERT(AR(a)<=1,EVRANK);  // a must be a 1-character list or atom
  // convert precision character to precision to use (_1 for date, 0-9)
  if(CAV(a)[0]=='d')prec=-1; else {prec=CAV(a)[0]-'0'; ASSERT((UI)prec<(UI)10,EVDOMAIN); ASSERT(((I)1<<prec)&0x209,EVNONCE);}  // 0 3 9 allowed
 }else{
  w=a; prec=9; // monad: switch argument, set defaults
 }
 ASSERT(AT(w)&LIT,EVDOMAIN);  // must be LIT
 ASSERT(AR(w),EVRANK);    // must not be an atom
 if(!AN(w)){RETF(df1(z,w,qq(sc(IMIN),zeroionei(1))));}   // return _"1 w on empty w - equivalent
 RETF(efs(jt,w,prec));
}
