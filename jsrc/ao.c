/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Oblique and Key                                                */

#include "j.h"

#ifdef MMSC_VER
#pragma warning(disable: 4244)
#endif

// This is the derived verb for f/. y
static DF1(jtoblique){A x,y,z;I m,n,r;D rkblk[16];
 RZ(w);F1PREFIP;
 r=AR(w);  // r = rank of w
 // create y= ,/ w - the _2-cells of w arranged in a list (virtual block)
 RZ(y=redcat(w,self)); if(1>=r){m=AN(w); n=1;}else{m=AS(w)[0]; n=AS(w)[1];}
 // Create x=+"0 1&i./ 2 {. $y
 A xm; RZ(xm=IX(m)); A xn; RZ(xn=IX(n));
 RZ(x=ATOMIC2(jt,xm,xn,rkblk,0L,1L,CPLUS)); AR(x)=1; *AS(x)=AN(x);
 // perform x f/. y, which does the requested operation, collecting the identical keys
 RZ(df2ip(z,x,y,sldot(VAV(self)->fgh[0])));
 // Final tweak: the result should have (0 >. <: +/ 2 {. $y) cells.  It will, as long as
 // m and n are both non0: when one is 0, result has 0 cells (but that cell is the correct result
 // of execution on a fill-cell).  Correct the length of the 0 case, when the result length should be nonzero
// if((m==0 || n==0) && (m+n>0)){R reitem(sc(m+n-1),x);}  This change withdrawn pending further deliberation
 RETF(z);
}


#define OBQCASE(t,id)    ((t)+(11*(id)))

#define OBQLOOP(Tw,Tz,zt,init,expr)  \
 {Tw* RESTRICT u,*v,*ww=(Tw*)wv;Tz x,* RESTRICT zz;                  \
  b=1; k=n1;                                     \
  GA(z,zt,d*c,r-1,1+s); AS(z)[0]=d; zz=(Tz*)AV(z); \
  DO(n,  v=ww+i;      u=v+n1*MIN(i,m1);     init; while(v<=(u-=n1))expr; *zz++=x;);  \
  DO(m1, v=ww+(k+=n); u=v+n1*MIN(m-i-2,n1); init; while(v<=(u-=n1))expr; *zz++=x;);  \
 }

// Derived verb for f//. y for atomic f
static DF1(jtobqfslash){A y,z;B b=0,p;C er,id,*wv;I c,d,k,m,m1,mn,n,n1,r,*s,wt;
 RZ(w);
 r=AR(w); s=AS(w); wt=AT(w); wv=CAV(w);
// obsolete  if(!(AN(w)&&1<r&&DENSE&wt))R oblique(w,self);  // revert to default if rank<2, empty, or sparse
 if((-AN(w)&(1-r)&-(DENSE&wt))>=0)R oblique(w,self);  // revert to default if rank<2, empty, or sparse.  This implies m/n below are non0
 y=FAV(self)->fgh[0]; y=FAV(y)->fgh[0]; id=FAV(y)->id/* obsolete vaid(y) */;
 m=s[0]; m1=m-1;
 n=s[1]; n1=n-1; mn=m*n; d=m+n-1; PROD(c,r-2,2+s);
// obsolete  if(1==m||1==n){GA(z,wt,AN(w),r-1,1+s); AS(z)[0]=d; MC(AV(z),wv,AN(w)<<bplg(wt)); R z;}
 if(((1-m)&(1-n))>=0){GA(z,wt,AN(w),r-1,1+s); AS(z)[0]=d; MC(AV(z),wv,AN(w)<<bplg(wt)); R z;}  // m=1 or n=1, return item of input
 if(wt&FL+CMPX)NAN0;
 if(1==c)switch(OBQCASE(CTTZ(wt),id)){
  case OBQCASE(B01X, CNE     ): OBQLOOP(B,B,wt,x=*u, x^=*u        ); break;
  case OBQCASE(B01X, CEQ     ): OBQLOOP(B,B,wt,x=*u, x=x==*u      ); break;
  case OBQCASE(B01X, CMAX    ):
  case OBQCASE(B01X, CPLUSDOT): OBQLOOP(B,B,wt,x=*u, x|=*u        ); break;
  case OBQCASE(B01X, CMIN    ):
  case OBQCASE(B01X, CSTAR   ):
  case OBQCASE(B01X, CSTARDOT): OBQLOOP(B,B,wt,x=*u, x&=*u        ); break;
  case OBQCASE(B01X, CLT     ): OBQLOOP(B,B,wt,x=*u, x=*u< x      ); break;
  case OBQCASE(B01X, CLE     ): OBQLOOP(B,B,wt,x=*u, x=*u<=x      ); break;
  case OBQCASE(B01X, CGT     ): OBQLOOP(B,B,wt,x=*u, x=*u> x      ); break;
  case OBQCASE(B01X, CGE     ): OBQLOOP(B,B,wt,x=*u, x=*u>=x      ); break;
  case OBQCASE(B01X, CPLUS   ): OBQLOOP(B,I,INT,x=*u, x+=*u       ); break;
  case OBQCASE(SBTX, CMAX    ): OBQLOOP(SB,SB,wt,x=*u, x=SBGT(x,*u)?x:*u ); break;
  case OBQCASE(SBTX, CMIN    ): OBQLOOP(SB,SB,wt,x=*u, x=SBLT(x,*u)?x:*u ); break;
  case OBQCASE(FLX,  CMAX    ): OBQLOOP(D,D,wt,x=*u, x=MAX(x,*u)  ); break;
  case OBQCASE(FLX,  CMIN    ): OBQLOOP(D,D,wt,x=*u, x=MIN(x,*u)  ); break;
  case OBQCASE(FLX,  CPLUS   ): OBQLOOP(D,D,wt,x=*u, x+=*u        ); break;
  case OBQCASE(CMPXX,CPLUS   ): OBQLOOP(Z,Z,wt,x=*u, x=zplus(x,*u)); break;
  case OBQCASE(XNUMX,CMAX    ): OBQLOOP(X,X,wt,x=*u, x=1==xcompare(x,*u)? x:*u); break;
  case OBQCASE(XNUMX,CMIN    ): OBQLOOP(X,X,wt,x=*u, x=1==xcompare(x,*u)?*u: x); break;
  case OBQCASE(XNUMX,CPLUS   ): OBQLOOP(X,X,wt,x=*u, x=xplus(x,*u)); break;
  case OBQCASE(RATX, CMAX    ): OBQLOOP(Q,Q,wt,x=*u, x=1==QCOMP(x,*u)? x:*u); break;
  case OBQCASE(RATX, CMIN    ): OBQLOOP(Q,Q,wt,x=*u, x=1==QCOMP(x,*u)?*u: x); break;
  case OBQCASE(RATX, CPLUS   ): OBQLOOP(Q,Q,wt,x=*u, x=qplus(x,*u)); break;
  case OBQCASE(INTX, CBW0001 ): OBQLOOP(I,I,wt,x=*u, x&=*u        ); break;
  case OBQCASE(INTX, CBW0110 ): OBQLOOP(I,I,wt,x=*u, x^=*u        ); break;
  case OBQCASE(INTX, CBW0111 ): OBQLOOP(I,I,wt,x=*u, x|=*u        ); break;
  case OBQCASE(INTX, CMAX    ): OBQLOOP(I,I,wt,x=*u, x=MAX(x,*u)  ); break;
  case OBQCASE(INTX, CMIN    ): OBQLOOP(I,I,wt,x=*u, x=MIN(x,*u)  ); break;
  case OBQCASE(INTX, CPLUS   ): 
   er=0; OBQLOOP(I,I,wt,x=*u, {p=0>x; x+=*u; BOV(p==0>*u&&p!=0>x);}); 
   if(er>=EWOV)OBQLOOP(I,D,FL,x=(D)*u, x+=*u);
 }
 if(wt&FL+CMPX)NAN1; RE(0);
 R b?z:oblique(w,self);
}    /* f//.y for atomic f */


#if SY_64
#define TYMESF(x)       {LD t=*u--*(LD)*v++; x=(I)t; BOV(t<IMIN||IMAX<t);}
#else
#define TYMESF(x)       {D  t=*u--*( D)*v++; x=(I)t; BOV(t<IMIN||IMAX<t);}
#endif
#define ACCUMF          {B p;I y; TYMESF(y); p=0>x; x+=y; BOV(p==0>y&&p!=0>x);}

#define PMCASE(t,c,d)   (65536*(c)+256*(d)+(t))

#define PMLOOP(Tw,Tz,zt,expr0,expr)  \
 {Tw*aa=(Tw*)av,* RESTRICT u,* RESTRICT v,*ww=(Tw*)wv;Tz x,* RESTRICT zv;  \
  b=1; GATVS(z,zt,zn,1,0,zt##SIZE,GACOPYSHAPE0,R 0); zv=(Tz*)AV(z);       \
  for(i=0;i<zn;++i){                         \
   j=MIN(i,m1); u=aa+j; v=ww+i-j;            \
   p=MIN(1+i,zn-i); p=MIN(p,k);              \
   expr0; DQ(p-1, expr;); *zv++=x;           \
 }}

DF2(jtpolymult){A f,g,z;B b=0;C*av,c,d,*wv;I at,i,j,k,m,m1,n,p,t,wt,zn;V*v;
 RZ(a&&w&&self);
 ASSERT(!((AT(a)|AT(w))&SPARSE),EVNONCE);
 m=AN(a); n=AN(w); m1=m-1; zn=m+n-1; k=MIN(m,n);
 at=AT(a); wt=AT(w); t=maxtyped(at,wt);
 if(TYPESNE(t,at))RZ(a=cvt(t,a)); at=AT(a); av=CAV(a);
 if(TYPESNE(t,wt))RZ(w=cvt(t,w)); wt=AT(w); wv=CAV(w);
 v=FAV(self);  // f//. @ (g/)
// obsolete  f=v->fgh[0]; y=FAV(f)->fgh[0]; y=FAV(y)->fgh[0]; c=vaid(y);  //
// obsolete  g=v->fgh[1]; y=FAV(g)->fgh[0];              d=vaid(y);   // g taken from g/
 f=v->fgh[0]; g=v->fgh[1];
 c=FAV(FAV(FAV(f)->fgh[0])->fgh[0])->id;   // id of f     f//. f/ f
 d=FAV(FAV(g)->fgh[0])->id;   // id of g     g/ g
// obsolete  if(!(m&&1==AR(a)&&n&&1==AR(w)))R obqfslash(df2(z,a,w,g),f);  // if empty, or not lists, do general code.  Never happens.
// obsolete  if(!(m&&1>=AR(a)&&n&&1>=AR(w)))R obqfslash(df2(z,a,w,g),f);  // if empty, or not lists, do general code.  Never happens.
 if((-m&(AR(a)-2)&-n&(AR(w)-2))>=0)R obqfslash(df2(z,a,w,g),f);  // if empty, or not atoms/lists, do general code.  Never happens.
 // from here on polymult on nonempty lists
 if(t&FL+CMPX)NAN0;
 switch(PMCASE(CTTZ(t),c,d)){
  case PMCASE(B01X, CNE,    CMAX    ): 
  case PMCASE(B01X, CNE,    CPLUSDOT): PMLOOP(B,B,B01,  x=*u--|*v++, x^=*u--|*v++); break;
  case PMCASE(B01X, CNE,    CSTAR   ): 
  case PMCASE(B01X, CNE,    CMIN    ): 
  case PMCASE(B01X, CNE,    CSTARDOT): PMLOOP(B,B,B01,  x=*u--&*v++, x^=*u--&*v++); break;
  case PMCASE(B01X, CPLUS,  CMAX    ): 
  case PMCASE(B01X, CPLUS,  CPLUSDOT): PMLOOP(B,I,INT,  x=*u--|*v++, x+=*u--|*v++); break;
  case PMCASE(B01X, CPLUS,  CSTAR   ): 
  case PMCASE(B01X, CPLUS,  CMIN    ): 
  case PMCASE(B01X, CPLUS,  CSTARDOT): PMLOOP(B,I,INT,  x=*u--&*v++, x+=*u--&*v++); break;
  case PMCASE(FLX,  CPLUS,  CSTAR   ): PMLOOP(D,D,FL,   x=*u--**v++, x+=*u--**v++); break;
  case PMCASE(CMPXX,CPLUS,  CSTAR   ): PMLOOP(Z,Z,CMPX, x=ztymes(*u--,*v++), x=zplus(x,ztymes(*u--,*v++))); break;
  case PMCASE(XNUMX,CPLUS,  CSTAR   ): PMLOOP(X,X,XNUM, x=xtymes(*u--,*v++), x=xplus(x,xtymes(*u--,*v++))); break;
  case PMCASE(RATX, CPLUS,  CSTAR   ): PMLOOP(Q,Q,RAT,  x=qtymes(*u--,*v++), x=qplus(x,qtymes(*u--,*v++))); break;
  case PMCASE(INTX, CBW0110,CBW0001 ): PMLOOP(I,I,INT,  x=*u--&*v++, x^=*u--&*v++); break;
  case PMCASE(INTX, CPLUS,  CSTAR   ): 
/*
   er=0; PMLOOP(I,I,INT, TYMESF(x), ACCUMF);
*/
  // here for +//.@(*/)
  {A a1,y;I*aa,i,*u,*ww=(I*)wv,*v,*yv,*zv;VA2 adocv; VARPS adocvsum;
   b=1;
   adocv=var(ds(CSTAR),at,wt); varps(adocvsum,FAV(f)->fgh[0],wt,0);  // get sum routine from f/, which is +/
// obsolete adocvsum=vains(ds(CPLUS),wt);
   GATV0(a1,INT,m,1); aa=AV(a1); u=m+(I*)av; DO(m, aa[i]=*--u;);
   GATV0(y,INT,MIN(m,n),1); yv=AV(y);
   GATV0(z,INT,zn,1); zv=AV(z);
   for(i=0;i<zn;++i){
    j=MIN(i,m1); u=aa+m1-j; v=ww+i-j;
    p=MIN(1+i,zn-i); p=MIN(p,k);
    I rc=((AHDR2FN*)adocv.f)((I)1,p,u,v,yv,jt); if(rc&255)jsignal(rc); if(255&(rc=((AHDRRFN*)adocvsum.f)((I)1,p,(I)1,yv,zv,jt)))jsignal(rc);
    ++zv;
   }
   if(EWOV<=jt->jerr){RESETERR; PMLOOP(I,D,FL, x=*u--*(D)*v++, x+=*u--*(D)*v++);}  // erroneous fa(z) removed; any error >= EWOV will be an overflow
  break;
  }
 }
 if(t&FL+CMPX)NAN1; RE(0);
 if(!b)R obqfslash(df2(z,a,w,g),f);
 RETF(z);
}    /* f//.@(g/) for atomic f, g */

// start of x f/. y (Key)
static DF2(jtkey);

static DF2(jtkeysp){PROLOG(0008);A b,by,e,q,x,y,z;I j,k,n,*u,*v;P*p;
 RZ(a&&w);
 {I t2; ASSERT(SETIC(a,n)==SETIC(w,t2),EVLENGTH);}  // verify agreement.  n is # items of a
 RZ(q=indexof(a,a)); p=PAV(q); 
 x=SPA(p,x); u=AV(x);
 y=SPA(p,i); v=AV(y);
 e=SPA(p,e); k=i0(e); 
 j=0; DO(AN(x), if(k<=u[i])break; if(u[i]==v[i])++j;);
 RZ(b=ne(e,x));
 RZ(by=repeat(b,y));
 RZ(x=key(repeat(b,x),from(ravel(by),w),self));
 GASPARSE(q,SB01,1,1,(I*)0); *AS(q)=n;  /* q=: 0 by}1$.n;0;1 */
 p=PAV(q); SPB(p,a,iv0); SPB(p,e,num(1)); SPB(p,i,by); SPB(p,x,reshape(tally(by),num(0)));
 RZ(z=over(df1(b,repeat(q,w),VAV(self)->fgh[0]),x));
 z=j?cdot2(box(IX(1+j)),z):z;
 EPILOG(z);
}

// a u/. w.  Self-classify a, then rearrange w and call cut.  Includes special cases for f//.
static DF2(jtkey){F2PREFIP;PROLOG(0009);A ai,z=0;I nitems;
 RZ(a&&w);
 if(unlikely(SPARSE&AT(a)))R keysp(a,w,self);  // if sparse, go handle it
 {I t2; ASSERT(SETIC(a,nitems)==SETIC(w,t2),EVLENGTH);}  // verify agreement.  nitems is # items of a
// obsolete  RZ(a=indexof(a,a));  // self-classify the input using ct set before this verb; we are going to modify a, so make sure it's not virtual
 RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
 // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
 // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
 // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
 // of indexofsub being set.
 PUSHCCT(jt->cctdefault);  // now that partitioning is over, reset ct for the executions of u
 I cellatoms; PROD(cellatoms,AR(w)-1,AS(w)+1);   // length of a cell of w, in atoms
 // if this is a supported f//., handle it without calling cut.  We can take it if the flag says f//. and the rank is >1 or the type is one we can do cheaply: B01/INT/FL 
 if(unlikely(SZI==SZD&&FAV(self)->flag&VFKEYSLASHT)){  // f//. where f is + >. <. mean   Implementation requires SZI==SZD
  if((((AR(w)-2)|((AT(w)&CMPX+XNUM+RAT)-1))&((AT(w)&FAV(self)->flag&VFKEYSLASHT)-1))>=0){  // rank>=2 and extended, or type we can handle locally  flag is B01+INT+FL for <. >., B01+FL for +  (we don't handle int ovfl)
   // We are going to handle the //. locally
   A freq;  // for mean, save frequencies here
   I keyslashfn=(FAV(self)->flag>>VFKEYSLASHFX)&3; // 0-3 for <. >. + mean
   // Figure out the type of result, and which routine will handle the operation
   I ztoride=(0x0e010000>>((4*keyslashfn+((AT(w)>>INTX)&3))<<1))&3;  // z type override. 00=none, 01=B to I 10=B to FL 11=I to FL  bits are opFI, values are (0) 0 3 2 (0) 0 x 1 (0) 0 0 0 (0) 0 0 0
   ztoride=AT(w)&FL+INT+B01?ztoride:0;  // no override for other types
   I zt=AT(w)^(((((INT+FL)<<12)+((B01+FL)<<8)+((B01+INT)<<4))>>(ztoride<<2))&0xf);  // switch result precision as needed
   I celllen = cellatoms<<bplg(zt);  // length of a cell of z, in bytes
#define RTNCASE(r,t) ((r)*4+(((t)>>INTX)&3))
   I routineid; VA2 adocv;   // case index to use 
   if(AR(w)<=1){
    // partitions have rank 1, so operations are on atoms.  set the index
    routineid=RTNCASE(keyslashfn,zt) ^ ztoride; // min, max are normal.  + is B01 (BtoI) or FL.  mean is B01 (BtoF), INT (ItoF), or FL
// obsolete     routineid=RTNCASE((FAV(self)->flag&VFKEYSLASHF)>>VFKEYSLASHFX,zt);  // case index if this is an atom
   }else{
    // partitions are arrays.  Operate on them in the output area
    routineid=RTNCASE(ztoride,INT+FL);  // special case index for action routine: 3 for normal (for <. >.), 7 for BtoI (for +), 11 for BtoF (for mean), 15 for ItoF (for mean) 17 = FL mean 16 for other mean e. g. INTX
    I meanrtn=16+((zt>>FLX)&1);  // 17 if FL result, 16 if not
    routineid=keyslashfn==routineid?meanrtn:routineid;  // 'other mean': only way they can be = is if both are 3.  Switch to 16/17, which differ in type of freq result
    A accfn=FAV(FAV(self)->fgh[0])->fgh[0]; accfn=keyslashfn==3?ds(CPLUS):accfn;
    adocv=var(accfn,AT(w),zt);  // get dyadic action routine for f out of f//. or (+/%#)/. for the given arguments
   }
   
   if(!((I)ai&1)){
    // not smallrange processing.  ai has combined fret/frequency information
    I nfrets=AM(ai);  // before we possibly clone it, extract # frets found
    makewritable(ai);  // we modify the size+index info to be running endptrs into the reorder area
    // allocate the result area(s)
    GA(z,zt,nfrets*cellatoms,AR(w),AS(w)); AS(z)[0]=nfrets; if(AN(z)==0)R z;  // avoid calls with empty args
    if(unlikely(keyslashfn==3)){
     GA(freq,zt&FL?FL:INT,nfrets,1,0);  // allocate place for divisor - INT if result may be XNUM/RAT
    }
    // loop through the index, copying the first value and operating on the others
    I indexx, indexn;  // index into the ai data, length of ai data
    I *ai0=IAV(ai);  // base of index table
    void *wv;  // current argument item

#define KSLGLP(wincr,zincr,pwvinit,pwvupd) \
 {void *nextpartition=voidAV(z);  /* place where next partition will be started */ \
 for(indexx=0, indexn=AN(ai), wv=voidAV(w); indexx!=indexn;++indexx, wv=(C*)wv+(wincr)){ \
     I nextx=ai0[indexx];  /* result of classifier for next item */ \
     if(nextx>=indexx){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      ai0[indexx]=(I)nextpartition;  /* save start point for partition */ \
      void *partition=nextpartition; \
      pwvinit  /* move the data in */ \
      nextpartition=(C*)nextpartition+(zincr);  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      void *partition=(void *)ai0[nextx];  /* partition we are extending */ \
      pwvupd \
     } \
    } \
 }
#define KSLGLPMEAN(wincr,zincr,pwvinit,pwvupd,freqt) \
 {I nextpartition=0;  /* index where next partition will be started */ \
 freqt *fv0=(freqt *)voidAV(freq); \
 void *zv0=voidAV(z); \
 for(indexx=0, indexn=AN(ai), wv=voidAV(w); indexx!=indexn;++indexx, wv=(C*)wv+(wincr)){ \
     I nextx=ai0[indexx];  /* result of classifier for next item */ \
     if(nextx>=indexx){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      ai0[indexx]=nextpartition;  /* index of partition */ \
      void *partition=(void *)((C*)zv0+nextpartition*(zincr));  /* partition we are extending */ \
      pwvinit  /* move the data in */ \
      fv0[nextpartition]=1; /* init freq to 1 */ \
      nextpartition++;  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      fv0[ai0[nextx]]++; \
      void *partition=(void *)((C*)zv0+ai0[nextx]*(zincr));  /* partition we are extending */ \
      pwvupd \
     } \
    } \
 }
    switch(routineid){
    case 3: {I rc; KSLGLP(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // <. >.
    case 7: {I rc; KSLGLP(cellatoms,celllen,DO(cellatoms, ((I*)partition)[i]=((C*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // + B01
    case 11: {I rc; KSLGLPMEAN(cellatoms,celllen,DO(cellatoms, ((D*)partition)[i]=((B*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean B01
    case 15: {I rc; KSLGLPMEAN(celllen,celllen,DO(cellatoms, ((D*)partition)[i]=((I*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean INT
    case 16: {I rc; KSLGLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),I) break;}  // mean for XNUM/RAT
    case 17: {I rc; KSLGLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean for FL
    case RTNCASE(0,B01): KSLGLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition&=*(B*)wv;) break;  // <.
    case RTNCASE(0,INT): KSLGLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0<w0?p0:w0;  *(I*)partition=p0;) break;  // <.
    case RTNCASE(0,FL): KSLGLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition<*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // <.
    case RTNCASE(1,B01): KSLGLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition|=*(B*)wv;) break;  // <.
    case RTNCASE(1,INT): KSLGLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0>w0?p0:w0;  *(I*)partition=p0;) break;  // >.
    case RTNCASE(1,FL): KSLGLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition>*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // >.
    case RTNCASE(2,B01): KSLGLP(1,SZI,*(I*)partition=*(B*)wv;,*(I*)partition+=*(B*)wv;) break;  // +  the input is B01 but the result is INT
    case RTNCASE(2,FL): NAN0; KSLGLP(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;) NAN1 break;
    case RTNCASE(3,B01): NAN0; KSLGLPMEAN(1,SZD,*(D*)partition=*(B*)wv;,*(D*)partition+=*(B*)wv;,D) NAN1 break;
    case RTNCASE(3,INT): NAN0; KSLGLPMEAN(SZI,SZD,*(D*)partition=*(I*)wv;,*(D*)partition+=*(I*)wv;,D) NAN1 break;
    case RTNCASE(3,FL): NAN0; KSLGLPMEAN(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;,D) NAN1 break;
    }
   }else{
    // small-range processing of f//.
    // indexofsub detected that small-range processing is in order.  Information about the range is secreted in fields of a
    ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item in bytes, smallest item, range+1
    // allocate a tally area and clear it to ~0
    A ftbl; GATV0(ftbl,INT,p,1); void **ftblv=voidAV(ftbl); memset(ftblv,~0,p<<LGSZI);
    // pass through the inputs, setting ftblv to 0 if there is a partition, and counting the number of partitions
    I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
    ftblv-=datamin;  // bias starting addr so that values hit the table
    I nparts=0;  // number of partitions in the result
    I *av=IAV(a); DQ(nitems, void * of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=0; nparts+=(I)of&1; av=(I*)((I)av+k);)   // count partitions and set pointers there to 0
    // allocate the result area(s)
    GA(z,zt,nparts*cellatoms,AR(w),AS(w)); AS(z)[0]=nparts; if(AN(z)==0)R z;  // avoid calls with empty args
    if(unlikely(keyslashfn==3)){
     GA(freq,zt&FL?FL:INT,nparts,1,0);  // allocate place for divisor - INT if result may be XNUM/RAT
    }
    // pass through the inputs again, accumulating the result.  First time we encounter a partition, initialize its pointer and the result cell
    // loop through the index, copying the first value and operating on the others
    void *nextpartition=voidAV(z);  // place where next partition will be started
    void *wv;  // current argument item

#define KSLSRLP(wincr,zincr,pwvinit,pwvupd) \
 {void *nextpartition=voidAV(z);  /* place where next partition will be started */ \
 for(av=IAV(a), wv=voidAV(w); nitems; av=(I*)((I)av+k), wv=(C*)wv+(wincr),--nitems){ \
     void *partition=ftblv[*av&valmsk];  /* partition address, or 0 if not allocated */ \
     if(!partition){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      ftblv[*av&valmsk]=nextpartition;  /* save start point for partition */ \
      partition = nextpartition; \
      pwvinit  /* move the data in */ \
      nextpartition=(C*)nextpartition+(zincr);  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      pwvupd \
     } \
    } \
 }
#define KSLSRLPMEAN(wincr,zincr,pwvinit,pwvupd,freqt) \
 {I nextpartition=1;  /* index where next partition will be started - can't be 0 */ \
 freqt *fv0=(freqt *)(CAV(freq)-SZI);  /* bias to match nextpartition */ \
 void *zv0=(void *)(CAV(z)-(zincr)); \
 for(av=IAV(a), wv=voidAV(w); nitems; av=(I*)((I)av+k), wv=(C*)wv+(wincr),--nitems){ \
     I partitionx=(I)ftblv[*av&valmsk];  /* partition index, or 0 if not allocated */ \
     if(!partitionx){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      partitionx=nextpartition; \
      ftblv[*av&valmsk]=(void *)partitionx;  /* save start point for partition */ \
      fv0[partitionx]=1; \
      void *partition=(void *)((C*)zv0+partitionx*(zincr));  /* partition we are extending */ \
      pwvinit  /* move the data in */ \
      ++nextpartition;  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      fv0[partitionx]++; \
      void *partition=(void *)((C*)zv0+partitionx*(zincr));  /* partition we are extending */ \
      pwvupd \
     } \
    } \
 }

    switch(routineid){
    case 3: {I rc; KSLSRLP(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // <. >.
    case 7: {I rc; KSLSRLP(cellatoms,celllen,DO(cellatoms, ((I*)partition)[i]=((C*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // + B01
    case 11: {I rc; KSLSRLPMEAN(cellatoms,celllen,DO(cellatoms, ((D*)partition)[i]=((B*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean B01
    case 15: {I rc; KSLSRLPMEAN(celllen,celllen,DO(cellatoms, ((D*)partition)[i]=((I*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean INT
    case 16: {I rc; KSLSRLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),I) break;}  // mean for XNUM/RAT
    case 17: {I rc; KSLSRLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean for FL
    case RTNCASE(0,B01): KSLSRLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition&=*(B*)wv;) break;  // <.
    case RTNCASE(0,INT): KSLSRLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0<w0?p0:w0;  *(I*)partition=p0;) break;  // <.
    case RTNCASE(0,FL): KSLSRLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition<*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // <.
    case RTNCASE(1,B01): KSLSRLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition|=*(B*)wv;) break;  // <.
    case RTNCASE(1,INT): KSLSRLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0>w0?p0:w0;  *(I*)partition=p0;) break;  // >.
    case RTNCASE(1,FL): KSLSRLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition>*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // >.
    case RTNCASE(2,B01): KSLSRLP(1,SZD,*(I*)partition=*(B*)wv;,*(I*)partition+=*(B*)wv;) break;  // +  the input is B01 but the result is INT
    case RTNCASE(2,FL): NAN0; KSLSRLP(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;) NAN1 break;
    case RTNCASE(3,B01): NAN0; KSLSRLPMEAN(1,SZD,*(D*)partition=*(B*)wv;,*(D*)partition+=*(B*)wv;,D) NAN1 break;
    case RTNCASE(3,INT): NAN0; KSLSRLPMEAN(SZI,SZD,*(D*)partition=*(I*)wv;,*(D*)partition+=*(I*)wv;,D) NAN1 break;
    case RTNCASE(3,FL): NAN0; KSLSRLPMEAN(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;,D) NAN1 break;
    }
   }
   // Finally, if this was mean, divide the total by the frequency
   if(unlikely(((FAV(self)->flag&VFKEYSLASHF)>>VFKEYSLASHFX)==3)){z=divideAW(z,freq);}  // always inplaceable
   EPILOG(z);
  }
 }

 // Here it wasn't fspecial//. - reorder the input and create frets
 // Allocate the area for the reordered copy of the input.  Do these calls early to free up registers for the main loop
 A frets,wperm;  // fret block, and place to reorder w
 UC *fretp;  // where the frets will be stored
 GA(wperm,AT(w),AN(w),AR(w),AS(w)); // Note we could avoid initialization of indirect types, since we are filling it all
 I celllen = cellatoms<<bplg(AT(w));  // length of a cell of w, in bytes
 JMCDECL(endmask) JMCSETMASK(endmask,celllen,1)   // set mask for JMCR
 // Once we know the number of frets, we can allocate the fret area.  If the number is small, we can use the canned area on the C stack.
 // The max # bytes needed is 4*((max # partitions of size>=256) clamped at # frets) + #frets
 // If y is inplaceable we can probably use it to store the frets, since it is copied sequentially.  The requirements are:
 // The fret area must start at least 5 bytes before the data in y, to leave space for the first fret before the first cell of y is moved (i. e. rank>0 for 64-bit, >1 for 32-bit)
 // The cells must be at least 5 bytes long so that the frets don't overrun them
 // The cells must be in the actual fret area (following the shape), i. e not virtual or NJA 
 // The cells must be DIRECT type so that it is OK to write garbage over them
 // We are taking advantage here of the fact that the shape comes before the cells in a standard GA noun
 // We don't have to worry about a==w because a has already been modified

 I nfrets;  // we get the # frets early and use that for allocation
 I localfrets[32];  // if we don't have too many frets, we can put them on the C stack
 if(!((I)ai&1)){
  // NOT small-range processing: go through the index+size table to create the frets and reordered data for passing to cut
  nfrets=AM(ai);  //fetch # frets before we possibly clone ai
  I maxfretsize=(nitems>>8); maxfretsize=maxfretsize<nfrets?nfrets:maxfretsize; maxfretsize=4*maxfretsize+nfrets+1;  // max # bytes needed for frets, if some are long
  if((UI)maxfretsize<sizeof(localfrets)-NORMAH*SZI){frets=(A)localfrets; AT(frets)=0;} // Cut tests the type field - only
  else if((I)jtinplace&(I)((AFLAG(w)&(AFVIRTUAL|AFNJA))==0)&((UI)((-(I )(AT(w)&DIRECT))&AC(w)&(4-celllen)&((I )(SZI==4)-AR(w)))>>(BW-1-JTINPLACEWX)))frets=w;
  else GATV0(frets,LIT,maxfretsize,0);   // 1 byte per fret is adequate, since we have padding
  fretp=CUTFRETFRETS(frets);  // Place where we will store the fret-lengths.  They are 1 byte normally, or 5 bytes for groups longer than 254

  makewritable(ai);  // we modify the size+index info to be running endptrs into the reorder area
  // pass through the input, incrementing each reference
  I *av=IAV(ai);  // av->a data
 #if 0  // obsolete
  {I *av2, *avend=av+nitems; for(av2=av;av2!=avend;++av2)++av[*av2];}  // first time the root of the partition points to & increments itself
 #endif
  // Now each item av[i] is either (1) smaller than i, which means that it is extending a previous key; or (2) greater than i, which
  // means it starts a new partition whose length is a[i]-i.  Process the values in order, creating partitions as they come up, and
  // moving the data for each input value in turn, reading in order and scatter-writing.
  I i; I nextpartitionx;  // loop index, address of place to store next partition
  I * RESTRICT wv=IAV(w);   // source & target pointers
  for(i=0, nextpartitionx=(I)IAV(wperm);i<nitems;++i){
   I * RESTRICT partitionptr;  // pointer to where output will go
   I avvalue=av[i];  // fetch partition length/index
   if(avvalue<i){  // this value extends its partition
    partitionptr=(I*)av[avvalue];  // addr of end of selected partition
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue-=i;  // length of partition
    if(avvalue<255)*fretp++ = (UC)avvalue; else{*fretp++ = 255; *(UI4*)fretp=(UI4)avvalue; fretp+=SZUI4;}
    partitionptr=(I*)nextpartitionx;  // copy this item's data to the start of the partition
    nextpartitionx+=avvalue*celllen;  // reserve output space for the partition
    avvalue=i;   // shift meaning of avvalue from length to index, where the partition pointer will be stored
   }

   av[avvalue]=(I)partitionptr+celllen;  // store updated end-of-partition after move
// obsolete    // copy the data to the end of its partition and advance the partition pointer
// obsolete    if(celllen<MEMCPYTUNELOOP) {  // copy by hand if that's faster (0 len OK)
// obsolete     I n=celllen; while((n-=SZI)>=0){*partitionptr++=*wv++;}
// obsolete       // move full words.  Must not overwrite the area, since we are scatter-writing.
// obsolete     if(n&(SZI-1)){STOREBYTES(partitionptr,*wv,-n); wv = (I*)((C*)wv+SZI+n);}  // Use test because this code is repeated
// obsolete    }else{MC(partitionptr,wv,celllen); wv = (I*)((C*)wv+celllen);}
   JMCR(partitionptr,wv,celllen,loop1,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing

  }
 }else{I *av;  // running pointer through the inputs
  // indexofsub detected that small-range processing is in order.  Information about the range is secreted in fields of a
  ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item, smallest item, range+1
  // allocate a tally area and clear it.  Could use narrower table perhaps
  A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=IAV(ftbl); memset(ftblv,0,p<<LGSZI);
  // pass through the inputs, counting the negative of the number of slots mapped to each index
  I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
  ftblv-=datamin;  // bias starting addr so that values hit the table
  nfrets=nitems;  // initialize fret counter, decremented for each non-fret
  av=IAV(a); DQ(nitems, I tval=ftblv[*av&valmsk]; ftblv[*av&valmsk]=tval-1; nfrets-=SGNTO0(tval); av=(I*)((I)av+k);)   // build (negative) frequency table; sub 1 for each non-fret
  // now that we have the number of freats, we can allocate the fret block
  I maxfretsize=(nitems>>8); maxfretsize=maxfretsize<nfrets?nfrets:maxfretsize; maxfretsize=4*maxfretsize+nfrets+1;  // max # bytes needed for frets
  if((UI)maxfretsize<sizeof(localfrets)-NORMAH*SZI){frets=(A)localfrets; AT(frets)=0;} // Cut tests the type field - obly
  else if((I)jtinplace&(I)((AFLAG(w)&(AFVIRTUAL|AFNJA))==0)&((UI)((-(I)(AT(w)&DIRECT))&AC(w)&(4-celllen)&((I)(SZI==4)-AR(w)))>>(BW-1-JTINPLACEWX)))frets=w;
  else GATV0(frets,LIT,maxfretsize,0);   // 1 byte per fret is adequate, since we have padding
  fretp=CUTFRETFRETS(frets);  // Place where we will store the fret-lengths.  They are 1 byte normally, or 5 bytes for groups longer than 254

  // pass through the inputs again.  If we encounter a negative value, allocate the next fret.  Copy the next item and advance
  // that partition's fret pointer
  // Now each item ftblv[av[i]] is either (1) nonnegative, which means that it is extending a previous key; or (2) negative, which
  // means it starts a new partition whose length is -ftblv[av[i]].  Process the values in order, creating partitions as they come up, and
  // moving the data for each input value in turn, reading in order and scatter-writing.

  I i; I nextpartitionx;  // loop index, index of place to store next partition
  I * RESTRICT wv=IAV(w);   // source pointer
  C * RESTRICT wpermv=CAV(wperm);  // addr of output area
  for(av=IAV(a), i=nitems, nextpartitionx=0;i;--i){
   I partitionndx;  // index of where this output will go
   I *slotaddr=&ftblv[*av&valmsk];  // the slot in the table we are processing
   I avvalue=*slotaddr;  // fetch input value, mask to valid portion, fetch partition length/index
   if(avvalue>=0){  // this value extends its partition
    partitionndx=avvalue;  // index of current value in selected partition
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue= -avvalue;  // length of partition
    if(avvalue<255)*fretp++ = (UC)avvalue; else{*fretp++ = 255; *(UI4*)fretp=(UI4)avvalue; fretp+=SZUI4;}
    partitionndx=nextpartitionx;  // copy this item's data to the start of the partition
    nextpartitionx+=avvalue;  // reserve output space for the partition
   }

   I *partitionptr=(I*)(wpermv+partitionndx*celllen);  // place to copy next input to
   *slotaddr=partitionndx+1;  // store updated next-in-partition after move
   // copy the data to the end of its partition and advance the input pointer
// obsolete    if(celllen<MEMCPYTUNELOOP) {  // copy by hand if that's faster (0 len OK)
// obsolete     I n=celllen; while((n-=SZI)>=0){*partitionptr++=*wv++;}
// obsolete       // move full words.  Must not overwrite the area, since we are scatter-writing.
// obsolete     if(n&(SZI-1)){STOREBYTES(partitionptr,*wv,-n); wv = (I*)((C*)wv+SZI+n);}  // Use test because this code is repeated
// obsolete    }else{MC(partitionptr,wv,celllen); wv = (I*)((C*)wv+celllen);}
   JMCR(partitionptr,wv,celllen,loop2,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing

   av=(I*)((I)av+k);  // advance to next input value
  }
 }

 // Frets are calculated and w is reordered.  Call cut to finish the job.  We have to store the count and length of the frets
 CUTFRETCOUNT(frets)=nfrets;  // # frets is #bytes stored, minus the length of the extended encodings
 CUTFRETEND(frets)=(I)fretp;   // pointer to end+1 of data
 // wperm is always inplaceable.  If u is inplaceable, make the call to cut inplaceable
 // Transfer pristinity of w to wperm so we can see if it went away, but only if w is pristine inplaceable.  We want to leave
 // wperm pristine so it can be used, but it's pristine only is w is zombie.  We sacrifice pristinity to inplaceability
 AFLAG(wperm)|=AFLAG(w)&AFPRISTINE&REPSGN(AC(w)&SGNIF(jtinplace,JTINPLACEWX));
 // We pass the self pointer for /. into cut, as it uses the id therein to interpret a
 z=jtcut2((J)(intptr_t)((I)jt+((FAV(self)->flag&VGERL)?0:(FAV(FAV(self)->fgh[0])->flag>>(VJTFLGOK1X-JTINPLACEWX))&JTINPLACEW)),frets,wperm,self);
 // If the operation turned off pristinity of wperm, do the same for w.  Remember that pristinity only matters if the block is inplaceable
 AFLAG(w)&=AFLAG(wperm)|~AFPRISTINE;
 POPCCT
 EPILOG(z);
}    /* a f/. w for dense x & w */

// bivalent entry point: a </. w   or  (</. i.@#) w
DF2(jtkeybox){F2PREFIP;PROLOG(0009);A ai,z=0;I nitems;
 RZ(a&&w);  // we don't neep ip, but all jtkey dyads must support it
 if(unlikely(SPARSE&AT(a)))R (AT(w)&NOUN?(AF)jtkeysp:(AF)jthook1cell)(jt,a,w,self);  // if sparse, go handle it
 SETIC(a,nitems);   // nitems is # items in a and w
 I cellatoms, celllen;  // number of atoms in an item of w, and the number of bytes therein.  celllen is negative for the monad
 struct AD fauxw;  // needed only for (</. i.@#) but must stay in scope
 if(likely(AT(w)&NOUN)){
  // dyad: </.
  I t2; ASSERT(nitems==SETIC(w,t2),EVLENGTH);  // verify agreement
  PROD(cellatoms,AR(w)-1,AS(w)+1);   // length of a cell of w, in atoms
  celllen=cellatoms<<bplg(AT(w));  // length of a cell of w, in bytes
 }else{
  // monad: (<./ i.@#) .  Create a suitable w and switch to it
  cellatoms=1; celllen=-1;  // len and flag of cells from the synthetic index vector
  // Fill in enough of fauxw to stand in for the vector.  We need type=INT and rank=1, and the shape needs to be a fetchable address but the value doesn't matter.
  // The data address returned by IAV needs to be 0
  w=&fauxw;  // switch to synthetic w
  AK(w)=-(I)w; AT(w)=INT; AR(w)=1;
 }
 JMCDECL(endmask) JMCSETMASK(endmask,celllen,1)   // set mask for JMCR.  Harmless if celllen=-1

 // Note: self is invalid from here on
// obsolete  RZ(a=indexof(a,a));  // self-classify the input using ct set before this verb; we are going to modify a, so make sure it's not virtual
 RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
 // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
 // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
 // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
 // of indexofsub being set.
 PUSHCCT(jt->cctdefault);  // now that partitioning is over, reset ct for the executions of u

 A *pushxsave;  // place to save the tpop stack when we hijack it
  A y;  // name under which boxes are allocated
 if(!((I)ai&1)){
  // NOT small-range processing: go through the index+size table to create the frets and reordered data for passing to cut
  I nboxes=AM(ai);  //fetch # frets before we possibly clone ai
  makewritable(ai);  // we modify the size+index info to be running endptrs into the reorder area
  // allocate the result, which will be recursive, and set up to fill it with blocks off the tstack
  GATV0(z,BOX,nboxes,1); if(nboxes==0)RETF(z); // allocate result, and exit if empty for comp ease below
  // boxes will be in AAV(z), in order.  Details of hijacking tnextpushp are discussed in jtbox().
  // We have to do it a little differently here, because when we allocate a block the contents come in piecemeal and we have no
  // convenient time to INCORPRA the new contents.  So instead we leave the outer box non-recursive, and rely on the EPILOG to make
  // it recursive, including recurring on the contents of the boxes.  To make this work we install an AC of 0 in the allocated
  // boxes, because they have no tpop outstanding.  The EPILOG will raise that to 1.
  pushxsave = jt->tnextpushp; jt->tnextpushp=AAV(z);  // save tstack info before allocation
  // **** MUST NOT FAIL FROM HERE UNTIL THE END, WHERE THE ALLOCATION SYSTEM CAN BE RESTORED ****

  // pass through the input, incrementing each reference
  I *av=IAV(ai);  // av->a data
  // Now each item av[i] is either (1) smaller than i, which means that it is extending a previous key; or (2) greater than i, which
  // means it starts a new partition whose length is a[i]-i.  Process the values in order, creating partitions as they come up, and
  // moving the data for each input value in turn, reading in order and scatter-writing.
  I i;   // loop index
  I * RESTRICT wv=IAV(w);   // source pointer, advanced item by item
  I wt=AT(w); I yr=MAX(AR(w),1);   // type of cells, and rank
  for(i=0;i<nitems;++i){
   I * RESTRICT partitionptr;  // pointer to where output will go
   I avvalue=av[i];  // fetch partition length/index
   if(avvalue<i){  // this value extends its partition
    partitionptr=(I*)av[avvalue];  // addr of end of selected partition
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue-=i;  // length of partition
    GAE(y,wt,cellatoms*avvalue,yr,AS(w),break); AC(y)=0; AS(y)[0]=avvalue; // allocate a region for the boxed data and set usecount to 0 since it is not on the tstack.  EPILOG will raise it to 1
    partitionptr=IAV(y);  // start of partition: in the data area of the block
    avvalue=i;   // shift meaning of avvalue from length to index, where the partition pointer will be stored
   }

   if(celllen>=0) {
     av[avvalue]=(I)partitionptr+celllen;  // store updated end-of-partition after move
     // copy the data to the end of its partition and advance the partition pointer
// obsolete      if(celllen<MEMCPYTUNELOOP) {  // copy by hand if that's faster (0 len OK)
// obsolete       I n=celllen; while((n-=SZI)>=0){*partitionptr++=*wv++;}
// obsolete         // move full words.  Must not overwrite the area, since we are scatter-writing.
// obsolete       if(n&(SZI-1)){STOREBYTES(partitionptr,*wv,-n); wv = (I*)((C*)wv+SZI+n);}  // Use test because this code is repeated
// obsolete      }else{MC(partitionptr,wv,celllen); wv = (I*)((C*)wv+celllen);}
    JMCR(partitionptr,wv,celllen,loop3,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing

   }else{  // flag for (<./ i.@#)
     *partitionptr=(I)wv;   // store wv itself, which is the index vector
     wv=(I*)((I)wv+1);  // advance index vector
     av[avvalue]=(I)partitionptr+SZI;  // store updated end-of-partition after move
   }
  }
 }else{I *av;  // running pointer through the inputs
  // indexofsub detected that small-range processing is in order.  Information about the range is secreted in fields of ai
  ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item, smallest item, range+1
  // allocate a tally area and clear it.  Could use narrower table perhaps
  A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=IAV(ftbl);
  // pass through the inputs, counting the number of slots mapped to each index
  // This is tricky, because we want the slot to hold either the number of slots (i. e. size of box) or the next address to store into for the slot.
  // But the address can be anything - ah, but not quite.  The address can't be inside an allocated block, for example it can't be inside a.
  // So we initialize the table to the address of the start of the data of a, and any number in the range (a,AN(a)+a] will be known to hold a count, not a pointer
  // NOTE that we know the items of a are not empty, so the prohibited address space is big enough
  I freqminval=(I)voidAV(a)+1;  // values from here to here+nitems-1 are frequencies
  {I mm=freqminval-1; mvc(p<<LGSZI,ftblv,SZI,&mm);}  // init table to '0' values
  I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
  ftblv-=datamin;  // bias starting addr so that values hit the table
  I nboxes=0;  // initialize fret counter, incremented for each fret
  av=IAV(a); DQ(nitems, I tval=ftblv[*av&valmsk]; ftblv[*av&valmsk]=++tval; nboxes+=tval==freqminval; av=(I*)((I)av+k);)   // build (negative) frequency table; sub 1 for each non-fret
  // allocate the result, which will be recursive, and set up to fill it with blocks off the tstack
  GATV0(z,BOX,nboxes,1); if(nboxes==0)RETF(z); // allocate result, and exit if empty for comp ease below
  // boxes will be in AAV(z), in order.  Details discussed in jtbox().
  // We will later make the block recursive usecount, and we will set all the initial usecounts to 1 since they are not on the tpop stack
  pushxsave = jt->tnextpushp; jt->tnextpushp=AAV(z);  // save tstack info before allocation
  // **** MUST NOT FAIL FROM HERE UNTIL THE END, WHERE THE ALLOCATION SYSTEM CAN BE RESTORED ****
  // pass through the inputs again.  If we encounter a frequency value, allocate the next box.  Copy the next item and advance
  // that partition's pointer

  I i; // loop index
  I * RESTRICT wv=IAV(w);   // source pointer
  I wt=AT(w); I yr=MAX(AR(w),1);   // type of cells, and rank
  for(av=IAV(a), i=nitems;i;--i){
   I *partitionptr;  // recipient of next cell
   I *slotaddr=&ftblv[*av&valmsk];  // the slot in the table we are processing
   I avvalue=*slotaddr;  // fetch partition length/pointer
   if((UI)(avvalue-freqminval)>=(UI)nitems){  // this value extends its partition
    partitionptr=(I*)avvalue;  // load into the address in the slot
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue=avvalue-freqminval+1;  // length of partition
    GAE(y,wt,cellatoms*avvalue,yr,AS(w),break); AC(y)=0; AS(y)[0]=avvalue; // allocate a region for the boxed data and set usecount to 0 since it is not on the tstack.  EPILOG will raise it to 1
    partitionptr=(I*)IAV(y);  // start the data for the partition at the beginning of the allocated box's data
   }

   // copy the data to the end of its partition and advance the partition pointer
   if(celllen>=0) {
     *slotaddr=(I)partitionptr+celllen;  // store updated end-of-partition after move
     // copy the data to the end of its partition and advance the partition pointer
// obsolete      if(celllen<MEMCPYTUNELOOP) {  // copy by hand if that's faster (0 len OK)
// obsolete       I n=celllen; while((n-=SZI)>=0){*partitionptr++=*wv++;}
// obsolete         // move full words.  Must not overwrite the area, since we are scatter-writing.
// obsolete       if(n&(SZI-1)){STOREBYTES(partitionptr,*wv,-n); wv = (I*)((C*)wv+SZI+n);}  // Use test because this code is repeated
// obsolete      }else{MC(partitionptr,wv,celllen); wv = (I*)((C*)wv+celllen);}
     JMCR(partitionptr,wv,celllen,loop4,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing
   }else{  // flag for (<./ i.@#)
     *slotaddr=(I)partitionptr+SZI;  // store updated end-of-partition after move
     *partitionptr=(I)wv;   // store wv itself, which is the index vector
     wv=(I*)((I)wv+1);  // advance index vector
   }

   av=(I*)((I)av+k);  // advance to next input value
  }
 }
 // restore the allocation system
 jt->tnextpushp=pushxsave;   // restore tstack pointer
 // Set PRISTINE if w now has DIRECT type (note that w has been switched to INT for (<./ i.@#))
 AFLAG(z)=(-(AT(w)&DIRECT) & AFPRISTINE);  // maybe pristine
 ASSERT(y!=0,EVWSFULL);  // if we broke out on allocation failure, fail.
 POPCCT
 EPILOG(z);
}    // a <./ w

// indexed by [chartype][k]
const UI4 shortrange[3][4] = {{0,65536,65536,0}, {0,2,258,0}, {0,256,65536,0}};  // C2T, B01, LIT


#if 0  // obsolete 
typedef struct {   // return struct from jtkeyrs
 CR minrange;  // detected min/range
 I type;  // pseudotype to use
} CRT;


static CRT jtkeyrs(J jt,A a,UI maxrange){I ac; CRT res;
 res.minrange.min=res.minrange.range=0; res.type=AT(a); ac=aii(a);
 if(2>=ac)switch(CTTZ(res.type)){
  case C2TX: if(1==ac)res.minrange.range=65536;                      break;
  case C4TX: if(1==ac){res.minrange=condrange4(C4AV(a),AN(a),-1,0,maxrange);} break;
  case B01X: if(1==ac)res.minrange.range=2;   else{res.minrange.range=258;   res.type=C2T;} break;
  case LITX: if(1==ac)res.minrange.range=256; else{res.minrange.range=65536; res.type=C2T;} break;
  case INTX: if(1==ac)res.minrange=condrange(AV(a),AN(a),IMAX,IMIN,maxrange);    break;
  case SBTX: if(1==ac){res.type=INT; res.minrange.range=jt->sbun; if(65536<res.minrange.range)res.minrange=condrange(AV(a),AN(a),IMAX,IMIN,maxrange);}
 }
 R res;
}

#define KCASE(d,t)          (t+5*d)
#define KACC1(F,Ta)  \
 {Ta*u;                                                          \
  if(1==c){                                                      \
   u=(Ta*)av0; DQ(n, v=qv+  *u++;       y=*wv++; *v=F;       );  \
   u=(Ta*)av0; DQ(n, if(bv[j=*u++]){*zv++=qv[j];                  bv[j]=0; if(s==++m)break;});  \
  }else{                                                         \
   u=(Ta*)av0; DQ(n, v=qv+c**u++; DQ(c, y=*wv++; *v=F; ++v;););  \
   u=(Ta*)av0; DQ(n, if(bv[j=*u++]){v=qv+c*j; DQ(c, *zv++=*v++;); bv[j]=0; if(s==++m)break;});  \
 }}
#define KACC(F,Tz,Tw,v0)   \
 {Tw*wv=(Tw*)wv0,y;Tz*qv=(Tz*)qv0,*v,*zv=(Tz*)zv0;               \
  if(bb){                                                        \
   m=0; v=qv; DQ(AN(q), *v++=v0;); qv-=r*c;                      \
   switch(CTTZNOFLAG(at)){                                                   \
    case B01X: KACC1(F,B ); break;                                \
    case LITX: KACC1(F,UC); break;                                \
    case C2TX: KACC1(F,US); break;                                \
    case C4TX: KACC1(F,C4); break;                                \
    case SBTX: KACC1(F,SB); break;                                \
    case INTX: KACC1(F,I ); break;                                \
  }}else{                                                        \
   v=zv; DQ(m*c, *v++=v0;);                                      \
   if(1==c)DQ(n, v=zv+  *xv++;       y=*wv++; *v=F;       )      \
   else    DQ(n, v=zv+c**xv++; DQ(c, y=*wv++; *v=F; ++v;););     \
 }}

static DF2(jtkeyslash){PROLOG(0012);A b,q,x,z=0;B bb,*bv,pp=0;C d;I at,*av0,c,n,j,m,*qv0,r,s,*u,wr,wt,*wv0,*xv,zt,*zv0;
 RZ(a&&w);
 at=AT(a); av0=AV(a); SETIC(a,n); 
 wt=AT(w); wv0=AV(w); wr=AR(w);
 ASSERT(n==SETIC(w,m),EVLENGTH);
// obsolete x=; d=vaid(VAV(x)->fgh[0]);
 d=FAV(FAV(FAV(self)->fgh[0])->fgh[0])->id;  // self is f//.   get  f/   then f  then id of f
d=0;  // scaf temporarily disable this until we decide it's worthwhile (and works - currently + has an overflow problem)
 if(B01&wt){d=d==CMAX?CPLUSDOT:d; d=(d==CMIN)|(d==CSTAR)?CSTARDOT:d;}
 if(!(AN(a)&&AN(w)&&at&DENSE&&
     (wt&B01&&(d==CEQ||d==CPLUSDOT||d==CSTARDOT||d==CNE||d==CPLUS)||
     wt&SBT&&(d==CMIN||d==CMAX)||
     wt&INT&&BETWEENC(d,17,25)||
     wt&INT+FL&&(d==CMIN||d==CMAX||d==CPLUS) )))R key(a,w,self);
 CRT rng=keyrs(a,MAX(2*n,65536)); c=aii(w); at=rng.type; r=rng.minrange.min; s=rng.minrange.range; m=s;
 zt=wt; zt=wt&INT?FL:zt; zt=wt&B01?INT:zt; zt=d==CPLUS?zt:wt;  // type of result, promoting bool and int but only if +
 bb=s!=0;
 if(bb){
  GATV0(b,B01,s,  1); bv=BAV(b); memset(bv,C1,s); bv-=r;
  GA(q,zt, s*c,1,0); qv0=AV(q);
 }else{RZ(x=indexof(a,a)); makewritable(x); xv=AV(x); m=0; u=xv; DO(n, *u=i==*u?m++:xv[*u]; ++u;);}  // kludge should remove misbranch
 GA(z,zt,m*c,wr,AS(w)); AS(z)[0]=m; zv0=AV(z);
 if(wt&FL)NAN0;
 PUSHCCT(jt->cctdefault)
 switch(KCASE(d,CTTZ(wt))){
  case KCASE(CEQ,     B01X): KACC(*v^y^1,    B, B, 1   ); break;
  case KCASE(CPLUSDOT,B01X): KACC(*v|y,    B, B, 0   ); break;
  case KCASE(CSTARDOT,B01X): KACC(*v&y,    B, B, 1   ); break;
  case KCASE(CNE,     B01X): KACC(*v^y,    B, B, 0   ); break;
  case KCASE(CMIN,    SBTX): KACC(SBLT(*v,y)?*v:y,SB,SB,jt->sbuv[0].down); break;
  case KCASE(CMAX,    SBTX): KACC(SBGT(*v,y)?*v:y,SB,SB,0); break;
  case KCASE(CMIN,    INTX): KACC(MIN(*v,y),I, I, IMAX); break;  
  case KCASE(CMIN,    FLX ): KACC(MIN(*v,y),D, D, inf ); break;
  case KCASE(CMAX,    INTX): KACC(MAX(*v,y),I, I, IMIN); break;
  case KCASE(CMAX,    FLX ): KACC(MAX(*v,y),D, D, infm); break;
  case KCASE(CPLUS,   B01X): KACC(*v+y,     I, B, 0   ); break;
  case KCASE(CPLUS,   INTX): KACC(*v+y,     D, I, 0.0 ); pp=1; break;
  case KCASE(CPLUS,   FLX ): KACC(*v+y,     D, D, 0.0 ); break;
  case KCASE(17,      INTX): KACC(*v&y,     UI,UI,-1  ); break;
  case KCASE(22,      INTX): KACC(*v^y,     UI,UI,0   ); break;
  case KCASE(23,      INTX): KACC(*v|y,     UI,UI,0   ); break;
  case KCASE(25,      INTX): KACC(~(*v^y),  UI,UI,-1  ); break;
// kludge this fails for other boolean values
 }
 POPCCT
 if(wt&FL)NAN1;
 *AS(z)=m; AN(z)=m*c; if(pp)RZ(z=pcvt(INT,z));
 EPILOG(z);
}    /* x f//.y */

#define KMCASE(ta,tw)  (4*ta+tw) // (ta+65536*tw)
#define KMACC(Ta,Tw) \
 {Ta*u=(Ta*)av;Tw*v=(Tw*)wv;                                        \
  if(1==c)DQ(n, ++pv[*u];                   qv[*u]+=*v++;   ++u;)   \
  else    DQ(n, ++pv[*u]; vv=qv+c**u; DQ(c, *vv++ +=*v++;); ++u;);  \
 }
#define KMSET(Ta)    \
 {Ta*u=(Ta*)av;                                                                                           \
  if(1==c)DQ(n, if(pv[j=*u++]){                             *zv++=qv[j]/pv[j]; pv[j]=0; if(s==++m)break;})   \
  else    DQ(n, if(pv[j=*u++]){vv=qv+c*j; d=(D)pv[j]; DQ(c, *zv++=*vv++/d;);   pv[j]=0; if(s==++m)break;});  \
 }
#define KMFUN(Tw)    \
 {Tw*v=(Tw*)wv;                                                      \
  if(1==c)DQ(n, j=*xv++; ++pv[j]; zv[j]+=*v++;)                      \
  else    DQ(n, j=*xv++; ++pv[j]; vv=zv+j*c; DQ(c, *vv+++=*v++;););  \
 }

static DF2(jtkeymean){PROLOG(0013);A p,q,x,z;D d,*qv,*vv,*zv;I at,*av,c,j,m=0,n,*pv,r,s,*u,wr,wt,*wv,*xv;
 RZ(a&&w);
 at=AT(a); av=AV(a); SETIC(a,n); 
 wt=AT(w); wv=AV(w); wr=AR(w);
 ASSERT(n==SETIC(w,j),EVLENGTH);
// obsolete  if(!(AN(a)&&AN(w)&&at&DENSE&&wt&B01+INT+FL))R df2(z,a,w,folk(sldot(slash(ds(CPLUS))),ds(CDIV),sldot(ds(CPOUND))));
 if((-AN(a)&-AN(w)&-(at&DENSE)&-(wt&B01+INT+FL))>=0)R df2(z,a,w,folk(sldot(slash(ds(CPLUS))),ds(CDIV),sldot(ds(CPOUND))));
 CRT rng = keyrs(a,MAX(2*n,65536)); at=rng.type; r=rng.minrange.min; s=rng.minrange.range; c=aii(w);
 if(wt&FL)NAN0;
 if(s){
  GATV0(p,INT,s,  1); pv= AV(p); memset(pv,C0,s*  SZI); pv-=r;
  GATV0(q,FL, s*c,1); qv=DAV(q); memset(qv,C0,s*c*SZD); qv-=r*c;
  GATV(z,FL, s*c,wr,AS(w)); zv=DAV(z);
  switch(KMCASE(CTTZ(at),CTTZ(wt))){
   case KMCASE(B01X,B01X): KMACC(B, B); break;
   case KMCASE(B01X,INTX): KMACC(B, I); break;
   case KMCASE(B01X,FLX ): KMACC(B, D); break;
   case KMCASE(LITX,B01X): KMACC(UC,B); break;
   case KMCASE(LITX,INTX): KMACC(UC,I); break;
   case KMCASE(LITX,FLX ): KMACC(UC,D); break;
   case KMCASE(C2TX,B01X): KMACC(US,B); break;
   case KMCASE(C2TX,INTX): KMACC(US,I); break;
   case KMCASE(C2TX,FLX ): KMACC(US,D); break;
   case KMCASE(C4TX,B01X): KMACC(C4,B); break;
   case KMCASE(C4TX,INTX): KMACC(C4,I); break;
   case KMCASE(C4TX,FLX ): KMACC(C4,D); break;
   case KMCASE(SBTX,B01X): KMACC(SB,B); break;
   case KMCASE(SBTX,INTX): KMACC(SB,I); break;
   case KMCASE(SBTX,FLX ): KMACC(SB,D); break;
   case KMCASE(INTX,B01X): KMACC(I ,B); break;
   case KMCASE(INTX,INTX): KMACC(I ,I); break;
   case KMCASE(INTX,FLX ): KMACC(I ,D);
  }
  switch(CTTZNOFLAG(at)){
   case B01X: KMSET(B ); break;
   case LITX: KMSET(UC); break;
   case C2TX: KMSET(US); break;
   case C4TX: KMSET(C4); break;
   case SBTX: KMSET(SB); break;
   case INTX: KMSET(I ); break;
  }
  AS(z)[0]=m; AN(z)=m*c;
 }else{
  RZ(x=indexof(a,a)); xv=AV(x); m=0; u=xv; DO(n, *u=i==*u?m++:xv[*u]; ++u;);
  GATV0(p,INT,m,  1);           pv= AV(p); memset(pv,C0,m*  SZI);
  GATV(z,FL, m*c,wr,AS(w)); AS(z)[0]=m; zv=DAV(z); memset(zv,C0,m*c*SZD);
  switch(CTTZNOFLAG(wt)){
   case B01X: KMFUN(B); break;
   case INTX: KMFUN(I); break;
   case FLX:  KMFUN(D); break;
  }
  if(1==c)DQ(m, *zv++/=*pv++;) else DQ(m, d=(D)*pv++; DQ(c, *zv++/=d;););
 }
 if(wt&FL)NAN1;
 EPILOG(z);
}    /* x (+/%#)/.y */


#define GRPCD(T)            {T*v=(T*)wv; DO(n, j=*v++; if(0<=dv[j])++cv[j]; else{dv[j]=i; cv[j]=1; ++zn;});}
#define GRPIX(T,asgn,j,k)   {T*v=(T*)wv; DO(n, j=asgn; if(m>=j)*cu[k]++=i; \
                                 else{GATV0(x,INT,cv[k],1); *zv++=x; u=AV(x); *u++=m=j; cu[k]=u;})}


F1(jtgroup){PROLOG(0014);A c,d,x,z,*zv;I**cu,*cv,*dv,j,k,m,n,t,*u,*v,*wv,zn=0;CR rng;
 RZ(w);
 if(SPARSE&AT(w))RZ(w=denseit(w));
 SETIC(w,n); t=AT(w); k=n?aii(w)<<bplg(t):0;
 if(!AN(w)){GATV0(z,BOX,n?1:0,1); if(n)RZ(*AAV(z)=IX(n)); R z;}
 if(2>=k){rng.range=shortrange[t&(B01+LIT)][k]; rng.min = 0;}  // kludge scaf use shift
// obsolete  else if(k==sizeof(C4)&&t&C4T){rng=condrange4(C4AV(w),n,-1,0,2*n);}
// obsolete  else if(k==SZI&&t&INT+SBT){rng=condrange(AV(w),n,IMAX,IMIN,2*n);}
 else if(((k^sizeof(C4))+(t&(NOUN&~C4T)))==0){rng=condrange4(C4AV(w),n,-1,0,2*n);}
 else if(((k^SZI)+(t&(NOUN&~(INT+SBT))))==0){rng=condrange(AV(w),n,IMAX,IMIN,2*n);}
 else{rng.range=0;}
 if(rng.range){
  GATV0(c,INT,rng.range,1); cv=AV(c)-rng.min;  /* counts  */
  GATV0(d,INT,rng.range,1); dv=AV(d)-rng.min;  /* indices */
  wv=AV(w); v=dv+rng.min; DQ(rng.range, *v++=-1;);
  switch(k){
   case 1:   GRPCD(UC); break;
   case 2:   GRPCD(US); break;
#if SY_64
   case 4:   GRPCD(C4); break;
#endif
   case SZI: GRPCD(I);
 }}else{
  RZ(w=indexof(w,w)); wv=AV(w);
  GATV0(c,INT,n,1); cv=AV(c);
  m=-1; v=wv; DQ(n, j=*v++; if(m>=j)++cv[j]; else{m=j; cv[j]=1; ++zn;});
 }
 GATV0(z,BOX,zn,1); zv=AAV(z);
 m=-1; cu=(I**)cv;
 switch(!!rng.range*k){
  case 1:   GRPIX(UC,dv[k=*v++],j,k); break;
  case 2:   GRPIX(US,dv[k=*v++],j,k); break;
#if SY_64
  case 4:   GRPIX(C4,dv[k=*v++],j,k); break;
#endif
  case SZI: GRPIX(I ,dv[k=*v++],j,k); break;
  default:  GRPIX(I ,     *v++ ,j,j);
 }
 EPILOG(z);
}    /* (</. i.@#) w */

#endif

static DF2(jtkeytally);

static F1(jtkeytallysp){PROLOG(0015);A b,e,q,x,y,z;I c,d,j,k,*u,*v;P*p;
 RZ(w);
 RZ(q=indexof(w,w));
 p=PAV(q); 
 x=SPA(p,x); u=AV(x); c=AN(x);
 y=SPA(p,i); v=AV(y);
 e=SPA(p,e); k=i0(e); 
 j=0; DO(c, if(k<=u[i])break; if(u[i]==v[i])++j;);
 RZ(b=ne(e,x));
 RZ(x=repeat(b,x)); RZ(x=keytally(x,x,mark)); u=AV(x); d=AN(x);
 GATV0(z,INT,1+d,1); v=AV(z);
 DQ(j, *v++=*u++;); *v++=SETIC(w,k)-bsum(c,BAV(b)); DQ(d-j, *v++=*u++;);
 EPILOG(z);
}    /* x #/.y , sparse x */

#if 0  // obsolete
#define KEYTALLY(T)     {T*u;                             \
                         u=(T*)av; I npart=0; DQ(n, I *ta=qv+*u++; I t=*ta; *ta=t+1; npart+=SGNTO0(t-1););  \
                         GATV0(z,INT,npart,1); zv=AV(z);  /* output area: one per bucket */ \
                         u=(T*)av; I tally; do{v=qv+*u++; tally=*v; *v=0; *zv=tally; tally=SGNTO0(-tally); zv+=tally;}while(npart-=tally);}
// obsolete                          u=(T*)av; DQ(n, v=qv+*u++; if(*v){*zv++=*v; *v=0; if(s==++j)break;});}
// Create total # of each type in av[]; npart is #distinct values.  Counters are ainitialized to -1
#define KEYTALLY(T) { \
 T *u; u=(T*)av; npart=0; DQ(n, I *ta=qv+*u++; I t=*ta; I firstinc=SGNTO0(t); *ta=t+firstinc+1; npart+=firstinc;); \
 GATV0(z,INT,npart,1); zv=AV(z);  /* output area: one per bucket */ \
 u=(T*)av; I tally; do{v=qv+*u++; tally=*v; *v=-1; *zv=tally; tally=SGNTO0(-tally); zv+=tally;}while(npart-=tally); \
 }
#endif

static DF2(jtkeytally){F2PREFIP;PROLOG(0016);A z,q;I at,j,k,n,r,s,*qv,*u,*v;
 RZ(a&&w);  // we don't neep ip, but all jtkey dyads must support it
 SETIC(a,n); at=AT(a);
 ASSERT(n==SETIC(w,k),EVLENGTH);
 if(!AN(a))R vec(INT,!!n,&AS(a)[0]);  // handle case of empties - a must have rank, so use AS[0] as  proxy for n
 if(unlikely(at&SPARSE))R keytallysp(a);
// obsolete CRT rng = keyrs(a,MAX(2*n,65536)); at=rng.type; r=rng.minrange.min; s=rng.minrange.range;
 if((-n&SGNIF(at,B01X)&(AR(a)-2))<0){B*b=BAV(a); k=bsum(n,b); R BETWEENO(k,1,n)?v2(*b?k:n-k,*b?n-k:k):vci(n);}  // nonempty rank<2 boolean a, just add the 1s
 A ai;  // result from classifying a
 RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
 // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
 // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
 // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
 // of indexofsub being set.
 if((I)ai&1){  // if small-range
  // we should do small-range processing.  Extract the info
  ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item in bytes, smallest item, range+1
  // allocate a tally area and clear it to 0
  A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=voidAV(ftbl); memset(ftblv,~0,p<<LGSZI);
  // pass through the inputs, setting ftbl to tally-1, and counting the number of partitions
  I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
  ftblv-=datamin;  // bias starting addr so that values hit the table
  I nparts=0;  // number of partitions in the result
  // count tally and partition
  I *av=IAV(a); DQ(n, I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=of+1; nparts+=SGNTO0(of); av=(I*)((I)av+k);)   // count partitions and set pointers there to 0
  // allocate result area
  GATV0(z,INT,nparts,1);  // output area: one per partition
  // pass the input again, copying out the values.  Use branches to avoid the long carried dependency
  av=IAV(a); I *zv=AV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){*zv++=of+1; if(--nparts==0)break;} av=(I*)((I)av+k);}
#if 0  // obsolete 
 if(s){A z;I*zv;
  // small-range case with s buckets.
// obsolete  GATV0(z,INT,s,1); zv=AV(z);  // output area: one per bucket
  GATV0(q,INT,s,1); qv=AV(q)-r;   // biased start of area where we count occurrences
  u=qv+r; DQ(s, *u++=-1;);   // clear the counters to -1
  I npart;  // number of result values
  switch(CTTZ(at)){
   case LITX: KEYTALLY(UC); break;
   case C2TX: KEYTALLY(US); break;
   case C4TX: KEYTALLY(C4); break;
   case SBTX: KEYTALLY(SB); break;
   case INTX: KEYTALLY(I ); break;
  }

// obsolete   AN(z)=*AS(z)=s-j;   // see how many different values there actually were
#endif
  EPILOG(z);
 }

 // not smallrange processing.  ai has combined fret/frequency information
 I nparts=AM(ai);  // before we possibly clone it, extract # frets found
 makewritable(ai);  // we modify the size+index info
 // allocate the result area
 GATV0(z,INT,nparts,1);   // avoid calls with empty args
 // pass through the table, writing out every value that starts a new partition.  The partition size is encoded in the value
 I i=0; I *av=IAV(ai);I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){*zv++=a0-i; if(--nparts==0)break;} ++i;}
#if 0

 // here when small-range not applicable
 RZ(q=indexof(a,a)); makewritable(q) /* obsolete realizeifvirtual(q);*/   // self-classify the atoms of a
// obsolete if(!AR(q))R iv1; 
// obsolete  v=
 qv=AV(q);
// obsolete  u=qv; DQ(n, ++*(qv+*u++););
// obsolete  u=qv; I npart=0; DO(n, I tu=*u++; ++*(qv+tu); npart+=REPSGN(tu-i)+1;);   // total each different value in its first occurrence, by adding to the index; so result is (index+#higher-or-equal values mapped to it)
 u=qv; I npart=0;  // scan pointer for slots; number of unique values
 I *chn=qv;  // pointer to first of the most recent string of consecutive misses.  The value stored there is -(#misses)
 do{
  I ux=*u;  // fetch index for slot being analyzed
  I* acux=&qv[ux];  // address of contents at ux: pointer to the value being incremented
  I cux=*acux;  // value being incremented
  cux=acux==u?0:cux;  // if this is the first time encountering this slot, remove the index value so count starts at 0
  *chn=(I)chn-(I)u;  // chn points to previous miss position, which is u if the previous slot was not a miss.  In case u is a miss, store # consecutive misses (neg)
       // OK to use byte distance, since it can't span more than half the address space
  *acux=cux+1;   // increment the total count.  If previous slot was not a miss, and this also, this overwrites the write to the chain
  ++u;  // advance to next slot
  chn=--cux<0?u:chn;  // move chain if the current slot is not a miss
  npart+=SGNTO0(cux);  // if not a miss, increment count of unique values
 }while(--n);  // n is gone now
 A z;I *zv; I skipmsk;
 GATV0(z,INT,npart,1); zv=AV(z);  /* output area: one per bucket */
 u=qv;
 // move values from q to z.  If an encountered count is negative, don't advance the output pointer, but skip that many slots of input
 // to avoid a carried dependency over the input values, we use a branch to decide what to do.  After the beginning it will alternate skips and moves,
 // which the branch predictor will lock onto
#if 0 // obsolete 
 do{
  I tally=*u; *zv=tally;  // presumptively move one value
  skipmsk=REPSGN(tally);  // ~0 if this is a skip
  u=(I*)((I)u-(skipmsk&tally));  // if neg, skip by the count given - it's in bytes
  ++skipmsk;  // now skipmsk is 1 if no skip, 0 if skip
  zv+=skipmsk; u+=skipmsk; // advance if there was no skip
 }while(npart-=skipmsk);  // loop till all non-skips have been written
#else
 do{
  I tally=*u;
  if(tally<0){
   // it's a skip
   u=(I*)((I)u-tally);  // if neg, skip by the count given - it's in bytes
   tally=*u;  // only one skip in a row
  }
  // here it's not a skip - move the result 
  *zv++=tally;  // presumptively move one value
  ++u; // advance to next value/skip
 }while(--npart);  // loop till all non-skips have been written
#endif
#endif
 EPILOG(z);

// obsolete  *AS(q)=AN(q)=npart;
// obsolete  u=qv; DO(n, k=*u++; if(i<k){j+=*v++=k-i; if(n==j)break;});
// obsolete  u=qv; I tally; I indx=0; I *vend=v+npart; do{tally=*u++-indx; *v=tally; ++indx; tally=REPSGN(tally); v+=tally+1;}while(v!=vend);  // calc tally in each slot, which is never 0
// obsolete  *AS(q)=AN(q)=v-qv;
// obsolete  EPILOG(q);
}    /* x #/.y main control & dense x */

#if 0 // obsolete
#define KEYHEADTALLY(Tz,Ta,Tw,exp0,exp1)  \
 {Ta*u;Tw*wv=(Tw*)AV(w);Tz*zz=(Tz*)zv;    \
  u=(Ta*)av; DQ(n, ++*(qv+*u++););        \
  u=(Ta*)av; DO(n, v=qv+*u++; if(*v){*zz++=exp0; *zz++=exp1; k+=*v; if(n==k)break; *v=0;}); \
  AN(z)=zz-(Tz*)zv;                       \
 }

// npart is #distinct values.  Counters are initialized to -1
// Each row has the type of the final result (Tw) but what is stored in it is actually two INTs: frequency and original slot#.
// The order depends on b, which is the index the tally should be stored to
#define KEYHEADBUILD(Ta) { \
 Ta *u;  u=(Ta*)av; npart=0; DQ(n, I *ta=qv+*u++; I t=*ta; I firstinc=SGNTO0(t); *ta=t+firstinc+1; npart+=firstinc;); \
 GATV0(z,wt&FL?FL:INT,2*npart,2); zv=IAV(z)+b; I bc=1-(b<<1); AS(z)[0]=npart; AS(z)[1]=2; I zinc=wt&FL?(SZD*2):(SZI*2);   /* output area: one per bucket offset zv to offset of tally */ \
 u=(Ta*)av; I tally; I headx=0; I i=npart; do{v=qv+*u++; tally=*v; *v=-1; zv[bc]=headx++; zv[0]=tally; tally=REPSGN(-tally); zv=(I*)((I)zv+(tally&zinc));}while(i+=tally); \
 }

// process the result of the above.  If the type of w is FL (case 1), we must convert the frequency to FL.
#define KEYHEADFILL(Tw,Tz,casen) { \
 Tz *optr=(Tz*)AV(z); Tw *wv=(Tw*)AV(w); \
 DQ(npart, I *iptr=(I*)optr; Tz head=wv[iptr[b^1]]; if(casen==1)optr[b]=iptr[b]; optr[b^1]=head; optr+=2;) \
}
#endif


static DF2(jtkeyheadtally){F2PREFIP;PROLOG(0017);A f,q,x,y,z;I b;I at,*av,k,n,r,*qv,*u,*v,wt,*zv;
 RZ(a&&w);  // we don't neep ip, but all jtkey dyads must support it
 SETIC(a,n); wt=AT(w);
 ASSERT(n==SETIC(w,k),EVLENGTH);
// obsolete  ASSERT(!n||wt&NUMERIC,EVDOMAIN);
 ASSERT((-n&((wt&NUMERIC)-1))>=0,EVDOMAIN); // OK if n=0 or numeric w
// obsolete  if(SPARSE&AT(a)||1<AR(w)||!n||!AN(a))R key(a,w,self);  // if sparse or w has rank>1 or a has no cells or no stoms, revert
 if(unlikely((((SPARSE&AT(a))-1)&((I)AR(w)-2)&(-n)&(-AN(a)))>=0))R key(a,w,self);  // if sparse or w has rank>1 or a has no cells or no atoms, revert
 av=AV(a);
 f=FAV(self)->fgh[0]; f=VAV(f)->fgh[0]; b=CHEAD==ID(f);  // b is 1 for {.,#  0 for #,{.  i. e. index of tally
// obsolete  CRT rng = keyrs(a,MAX(2*n,65536)); at=rng.type; r=rng.minrange.min; I s=rng.minrange.range;
 if(AT(a)&B01&&1>=AR(a)){B*p=(B*)av;I i,j,m;  // first special case: boolean list/atom
// obsolete   c=d=p;
  if(*p){i=0; B *d=(B*)memchr(p,C0,n); j=d-p; j=d?j:0;} // i=index of first 1, j=index of first 0 (0 if not found)
  else  {j=0; B *c=(B*)memchr(p,C1,n); i=c-p; i=c?i:0;}
// obsolete   k=bsum(n,p); m=c&&d?2:1;  // k=# 1s
// obsolete   GATV0(x,INT,m,1); v=AV(x); *v++=MIN(i,j);      if(c&&d)*v=MAX(i,j); 
// obsolete   GATV0(y,INT,m,1); v=AV(y); *v++=i<j||!d?k:n-k; if(c&&d)*v=i<j?n-k:k;
  k=bsum(n,p); m=i+j?1:0;  // k=# 1s  m is 1 if there are 0s and 1s
  GATV0(x,INT,m+1,1); v=AV(x); v[m]=i+j; v[0]=0;  // 0=index of first item (always 0); 1 if it exists is the other
  GATV0(y,INT,m+1,1); v=AV(y); j=n-k; k=i?j:k; k&=-m; v[0]=k; v[m]=n-k;  // if 1st value is 0, complement k; if only 1 value, clear k
  RZ(x=w=from(x,w)); x=b?x:y; y=b?y:w; R stitch(x,y);
 }
 // for other types of a, we handle it quickly only if w is B01/INT/FL
 if(wt&B01+INT+FL){
  A ai;  // result from classifying a
  RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
  // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
  // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
  // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
  // of indexofsub being set.
  if((I)ai&1){  // if small-range
   // we should do small-range processing.  Extract the info
   ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item in bytes, smallest item, range+1
   // allocate a tally area and clear it to 0
   A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=voidAV(ftbl); memset(ftblv,~0,p<<LGSZI);
   // pass through the inputs, setting ftbl to tally-1, and counting the number of partitions
   I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
   ftblv-=datamin;  // bias starting addr so that values hit the table
   I nparts=0;  // number of partitions in the result
   // count tally and partition
   I *av=IAV(a); DQ(n, I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=of+1; nparts+=SGNTO0(of); av=(I*)((I)av+k);)   // count partitions and set pointers there to 0
   // allocate result area
   GA(z,MAX(wt,INT),nparts*2,2,0); AS(z)[0]=nparts; AS(z)[1]=2;  // output area: one per partition
   // pass the input again, copying out the values.  Use branches to avoid the long carried dependency
   I i=0; av=IAV(a);   // item index and input scan pointer
   if(wt&INT){
    I *wv=IAV(w); I *zv=IAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=of+1; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }else if(wt&B01){
    B *wv=BAV(w); I *zv=IAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=of+1; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }else{  // FL
    D *wv=DAV(w); D *zv=DAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=(D)(of+1); zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }
#if 0  // obsolete 
 if(s){A z;I*zv;
  // small-range case with s buckets.
// obsolete  GATV0(z,INT,s,1); zv=AV(z);  // output area: one per bucket
  GATV0(q,INT,s,1); qv=AV(q)-r;   // biased start of area where we count occurrences
  u=qv+r; DQ(s, *u++=-1;);   // clear the counters to -1
  I npart;  // number of result values
  switch(CTTZ(at)){
   case LITX: KEYTALLY(UC); break;
   case C2TX: KEYTALLY(US); break;
   case C4TX: KEYTALLY(C4); break;
   case SBTX: KEYTALLY(SB); break;
   case INTX: KEYTALLY(I ); break;
  }

// obsolete   AN(z)=*AS(z)=s-j;   // see how many different values there actually were
#endif
  }else{

   // not smallrange processing.  ai has combined fret/frequency information
   I nparts=AM(ai);  // before we possibly clone it, extract # frets found
   makewritable(ai);  // we modify the size+index info
   // allocate the result area
   GA(z,MAX(wt,INT),nparts*2,2,0); AS(z)[0]=nparts; AS(z)[1]=2;  // output area: one per partition
   // pass through the table, writing out every value that starts a new partition.  The partition size is encoded in the value
   I i=0; I *av=IAV(ai);   // item index and input scan pointer
   if(wt&INT){
    I *wv=IAV(w); I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=a0-i; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} ++i;}
   }else if(wt&B01){
    B *wv=BAV(w); I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=a0-i; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} ++i;}
   }else{  // FL
    D *wv=DAV(w); D *zv=DAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=(D)(a0-i); zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} ++i;}
   }
#if 0  // obsolete
// obsolete  if(at&LIT+C2T+C4T+INT+SBT&&wt&B01+INT+FL&&s){  // second special case: small-range
 if((-(at&LIT+C2T+C4T+INT+SBT)&-(wt&B01+INT+FL)&-s)<0){  // second special case: small-range on integral x when w is a compatible type
  GATV0(q,INT,s,1); qv=AV(q)-r;  // allocate the frequency table
  u=qv+r; DQ(s, *u++=-1;);  // initialize frequencies to -1
  // build the result table, containing integers.  The table holds frequency and index of the value
  I npart;  // number of result values
  switch(CTTZ(at)){
   case INTX: KEYHEADBUILD(I ); break;
   case LITX: KEYHEADBUILD(UC); break;
   case C2TX: KEYHEADBUILD(US); break;
   case C4TX: KEYHEADBUILD(C4); break;
   case SBTX: KEYHEADBUILD(SB); break;
  }
  // replace the head indexes with the value there
  if(wt&INT)KEYHEADFILL(I,I,0)
  else if(wt&FL)KEYHEADFILL(D,D,1)
  else KEYHEADFILL(B,I,0)
#if 0  // obsolete
  GA(z,wt&FL?FL:INT,2*s,2,0); zv=AV(z);  // output area is INT/FL depending on w
 k=0;
  r=0; r=at&C2T?3:r; r=at&INT?6:r; r=at&C4T?9:r; r=at&SBT?12:r; 
  switch(15*b+r+((wt>>INTX)&3)){
   case  0: KEYHEADTALLY(I,UC,B,*v,   wv[i]); break;
   case  1: KEYHEADTALLY(I,UC,I,*v,   wv[i]); break;
   case  2: KEYHEADTALLY(D,UC,D,(D)*v,wv[i]); break;
   case  3: KEYHEADTALLY(I,US,B,*v,   wv[i]); break;
   case  4: KEYHEADTALLY(I,US,I,*v,   wv[i]); break;
   case  5: KEYHEADTALLY(D,US,D,(D)*v,wv[i]); break;
   case  6: KEYHEADTALLY(I,I ,B,*v,   wv[i]); break;
   case  7: KEYHEADTALLY(I,I ,I,*v,   wv[i]); break;
   case  8: KEYHEADTALLY(D,I ,D,(D)*v,wv[i]); break;
   case  9: KEYHEADTALLY(I,C4,B,*v,   wv[i]); break;
   case 10: KEYHEADTALLY(I,C4,I,*v,   wv[i]); break;
   case 11: KEYHEADTALLY(D,C4,D,(D)*v,wv[i]); break;
   case 12: KEYHEADTALLY(I,SB,B,*v,   wv[i]); break;
   case 13: KEYHEADTALLY(I,SB,I,*v,   wv[i]); break;
   case 14: KEYHEADTALLY(D,SB,D,(D)*v,wv[i]); break;
   case 15: KEYHEADTALLY(I,UC,B,wv[i],*v   ); break;
   case 16: KEYHEADTALLY(I,UC,I,wv[i],*v   ); break;
   case 17: KEYHEADTALLY(D,UC,D,wv[i],(D)*v); break;
   case 18: KEYHEADTALLY(I,US,B,wv[i],*v   ); break;
   case 19: KEYHEADTALLY(I,US,I,wv[i],*v   ); break;
   case 20: KEYHEADTALLY(D,US,D,wv[i],(D)*v); break;
   case 21: KEYHEADTALLY(I,I ,B,wv[i],*v   ); break;
   case 22: KEYHEADTALLY(I,I ,I,wv[i],*v   ); break;
   case 23: KEYHEADTALLY(D,I ,D,wv[i],(D)*v); break;
   case 24: KEYHEADTALLY(I,C4,B,wv[i],*v   ); break;
   case 25: KEYHEADTALLY(I,C4,I,wv[i],*v   ); break;
   case 26: KEYHEADTALLY(D,C4,D,wv[i],(D)*v); break;
   case 27: KEYHEADTALLY(I,SB,B,wv[i],*v   ); break;
   case 28: KEYHEADTALLY(I,SB,I,wv[i],*v   ); break;
   case 29: KEYHEADTALLY(D,SB,D,wv[i],(D)*v); break;
  }
  *AS(z)=AN(z)>>1; *(1+AS(z))=2;
#endif
 }else{  // must self-classify a
  RZ(q=indexof(a,a)); makewritable(q)
  // if w is compatible with INT (i. e. is B01+INT+FL list), scan the self-classify result 
  if(wt&B01+INT+FL){
   // obsolete  v=
   qv=AV(q);
// obsolete  u=qv; DQ(n, ++*(qv+*u++););
// obsolete  u=qv; I npart=0; DO(n, I tu=*u++; ++*(qv+tu); npart+=REPSGN(tu-i)+1;);   // total each different value in its first occurrence, by adding to the index; so result is (index+#higher-or-equal values mapped to it)
   u=qv; I npart=0;  // scan pointer for slots; number of unique values
   I *chn=qv;  // pointer to first of the most recent string of consecutive misses.  The value stored there is -(#misses)
   do{
    I ux=*u;  // fetch index for slot being analyzed
    I* acux=&qv[ux];  // address of contents at ux: pointer to the value being incremented
    I cux=*acux;  // value being incremented
    cux=acux==u?0:cux;  // if this is the first time encountering this slot, remove the index value so count starts at 0
    *chn=(I)chn-(I)u;  // chn points to previous miss position, which is u if the previous slot was not a miss.  In case u is a miss, store # consecutive misses (neg).  Can't overflow, even as bytes
    *acux=cux+1;   // increment the total count.  If previous slot was not a miss, and this also, this overwrites the write to the chain
    ++u;  // advance to next slot
    chn=--cux<0?u:chn;  // move chain if the current slot is not a miss
    npart+=SGNTO0(cux);  // if not a miss, increment count of unique values
   }while(--n);  // n is gone now
   /* I *zv; */ I skipmsk;
   GATV0(z,wt&FL?FL:INT,2*npart,2); AS(z)[0]=npart; AS(z)[1]=2; I *u0=u=qv;
   // move values from q to z.  If an encountered count is negative, don't advance the output pointer, but skip that many slots of input

// we chase the chain using a branch to avoid the carried dependency.  After a startup phase the skip will almost always be taken
#define KEYHEADFILLGEN(Tw,Tz) \
   {Tz *zv=(Tz*)AV(z);  /* output scan pointer */ \
   Tw *wv=(Tw*)AV(w);  /* start of w list */ \
   zv+=b; I bc=1-(b<<1);  /* advance zv to point to tally slot, bc the  distance to the head */ \
   do{ \
    I tally=*u; \
    if(tally<0){ \
     u=(I*)((I)u-tally);  /* if neg, skip by the count given - it's in bytes */ \
     tally=*u;  /* only one skip in a row */ \
    } \
    zv[0]=tally; zv[bc]=wv[u-u0];  /* move tally and head */ \
    zv+=2; ++u; /* advance to next value/skip */ \
   }while(--npart); \
   }

#if 0  // obsolete
    I tally=*u; zv[0]=tally;  /* presumptively move one count... */ \
    zv[bc]=wv[u-u0];  /* ...and headvalue */ \
    skipmsk=REPSGN(tally);  /* ~0 if this is a skip */ \
    u=(I*)((I)u-(skipmsk&tally));  /* if neg, skip by the count given - it's in bytes */ \
    ++skipmsk;  /* now skipmsk is 1 if no skip, 0 if skip */ \
    zv+=skipmsk*2; u+=skipmsk; /* advance if there was no skip */ \
   }while(npart-=skipmsk);  /* loop till all non-skips have been written */
#endif

   if(wt&INT)KEYHEADFILLGEN(I,I)
   else if(wt&FL)KEYHEADFILLGEN(D,D)
   else KEYHEADFILLGEN(B,I)
 }
#endif
  }
 }else{  // no special processing
  RZ(q=indexof(a,a)); x=repeat(eq(q,IX(n)),w); y=keytally(q,q,0L); z=stitch(b?x:y,b?y:x);  // (((i.~a) = i. # a) # w) ,. (#/.~ i.~ a)   for ({. , #)
 }
 EPILOG(z);
}    /* x ({.,#)/.y or x (#,{.)/. y */


F1(jtsldot){A h=0;AF f1=jtoblique,f2;C c,d,e;I flag=VJTFLGOK1|VJTFLGOK2;V*v;
// NOTE: u/. is processed using the code for u;.1 and passing the self for /. into the cut verb.  So, the self produced
// by /. and ;.1 must be the same as far as flags etc.
 RZ(w);
 if(NOUN&AT(w)){flag|=VGERL; RZ(h=fxeachv(1L,w));}
 v=VAV(w);
 switch(ID(w)){  // no default for f2: every path must set it
  case CPOUND: f2=jtkeytally; break;
// obsolete   case CSLASH: f2=jtkeyslash; if(vaid(v->fgh[0]))f1=jtobqfslash; break;
  case CSLASH: f2=jtkey; if(AT(v->fgh[0])&VERB&&FAV(v->fgh[0])->flag&VISATOMIC2){ // f//.  if f is atomic2
   /*scaf f2=jtkeyslash;*/ f1=jtobqfslash; flag&=~VJTFLGOK1;
   // dyad f//. is special for f=+ >. <.   we set flags to indicate the operation and the allowed types
#define keyslashvalues(w)CCM(w,CPLUS)+CCM(w,CMIN)+CCM(w,CMAX)
    CCMWDS(keyslash) CCMCAND(keyslash,cand,FAV(v->fgh[0])->id) if(CCMTST(cand,FAV(v->fgh[0])->id)){
     I op=FAV(v->fgh[0])->id&1; op=FAV(v->fgh[0])->id==CPLUS?2:op;  // 0=<. 1=>. 2=+
     flag += (((((2<<VFKEYSLASHFX)+((FL+B01)<<VFKEYSLASHTX))<<16) + (((1<<VFKEYSLASHFX)+((FL+INT+B01)<<VFKEYSLASHTX))<<8) + ((0<<VFKEYSLASHFX)+((FL+INT+B01)<<VFKEYSLASHTX))) >> (op<<3)) & (VFKEYSLASHT+VFKEYSLASHF);   // get flag bits
    }

   } break;
  case CBOX: f2=jtkeybox; break;  // </.
  case CFORK:  if(v->valencefns[0]==(AF)jtmean){/* obsolete f2=jtkeymean*/flag+=(3<<VFKEYSLASHFX)+((FL+INT+B01)<<VFKEYSLASHTX);  // (+/%#)/., treated as f//.
               }else{c=ID(v->fgh[0]); d=ID(v->fgh[1]); e=ID(v->fgh[2]); 
                if(((c^e)==(CHEAD^CPOUND))&&d==CCOMMA&&(c==CHEAD||c==CPOUND)){f2=jtkeyheadtally; break;}
               }
               // otherwise (including keymean) fall through to...
  default: f2=jtkey; flag |= (FAV(w)->flag&VASGSAFE);  // pass through ASGSAFE.
 }
 R fdef(0,CSLDOT,VERB, f1,f2, w,0L,h, flag, RMAX,RMAX,RMAX);
}
