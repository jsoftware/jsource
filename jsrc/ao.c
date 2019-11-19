/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Oblique and Key                                                */

#include "j.h"
// TODO: remove idiv from keymean

// This is the derived verb for f/. y
static DF1(jtoblique){A x,y;I m,n,r;D rkblk[16];
 RZ(w);
 r=AR(w);  // r = rank of w
 // create y= ,/ w - the _2-cells of w arranged in a list (virtual block)
 RZ(y=redcat(w,self)); if(1>=r){m=AN(w); n=1;}else{m=AS(w)[0]; n=AS(w)[1];}
 // Create x=+"0 1&i./ 2 {. $y
 A xm; RZ(xm=IX(m)); A xn; RZ(xn=IX(n));
 RZ(x=ATOMIC2(jt,xm,xn,rkblk,0L,1L,CPLUS)); AR(x)=1; *AS(x)=AN(x);
 // perform x f/. y, which does the requested operation, collecting the identical keys
 RZ(x=df2(x,y,sldot(VAV(self)->fgh[0])));
 // Final tweak: the result should have (0 >. <: +/ 2 {. $y) cells.  It will, as long as
 // m and n are both non0: when one is 0, result has 0 cells (but that cell is the correct result
 // of execution on a fill-cell).  Correct the length of the 0 case, when the result length should be nonzero
// if((m==0 || n==0) && (m+n>0)){R reitem(sc(m+n-1),x);}  This change withdrawn pending further deliberation
 RETF(x);
}


#define OBQCASE(t,id)    ((t)+(9*(id)))

#define OBQLOOP(Tw,Tz,zt,init,expr)  \
 {Tw* RESTRICT u,*v,*ww=(Tw*)wv;Tz x,* RESTRICT zz;                  \
  b=1; k=n1;                                     \
  GA(z,zt,d*c,r-1,1+s); AS(z)[0]=d; zz=(Tz*)AV(z); \
  DO(n,  v=ww+i;      u=v+n1*MIN(i,m1);     init; while(v<=(u-=n1))expr; *zz++=x;);  \
  DO(m1, v=ww+(k+=n); u=v+n1*MIN(m-i-2,n1); init; while(v<=(u-=n1))expr; *zz++=x;);  \
 }

// Derived verb for f//. y
static DF1(jtobqfslash){A y,z;B b=0,p;C er,id,*wv;I c,d,k,m,m1,mn,n,n1,r,*s,wt;
 RZ(w);
 r=AR(w); s=AS(w); wt=AT(w); wv=CAV(w);
 if(!(AN(w)&&1<r&&DENSE&wt))R oblique(w,self);  // revert to default if rank<2, empty, or sparse
 y=FAV(self)->fgh[0]; y=VAV(y)->fgh[0]; id=vaid(y);
 m=s[0]; m1=m-1;
 n=s[1]; n1=n-1; mn=m*n; d=m+n-1; PROD(c,r-2,2+s);
 if(1==m||1==n){GA(z,wt,AN(w),r-1,1+s); AS(z)[0]=d; MC(AV(z),wv,AN(w)<<bplg(wt)); R z;}
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

DF2(jtpolymult){A f,g,y,z;B b=0;C*av,c,d,*wv;I at,i,j,k,m,m1,n,p,t,wt,zn;V*v;
 RZ(a&&w&&self);
 ASSERT(!((AT(a)|AT(w))&SPARSE),EVNONCE);
 m=AN(a); n=AN(w); m1=m-1; zn=m+n-1; k=MIN(m,n);
 at=AT(a); wt=AT(w); t=maxtyped(at,wt);
 if(TYPESNE(t,at))RZ(a=cvt(t,a)); at=AT(a); av=CAV(a);
 if(TYPESNE(t,wt))RZ(w=cvt(t,w)); wt=AT(w); wv=CAV(w);
 v=FAV(self);  // f//. 
 f=v->fgh[0]; y=FAV(f)->fgh[0]; y=VAV(y)->fgh[0]; c=vaid(y);  // f/, then f
 g=v->fgh[1]; y=VAV(g)->fgh[0];              d=vaid(y);   // g taken from g/
// obsolete  if(!(m&&1==AR(a)&&n&&1==AR(w)))R obqfslash(df2(a,w,g),f);  // if empty, or not lists, do general code
 if(!(m&&1==AR(a)&&n&&1==AR(w)))R obqfslash(df2(a,w,g),f);  // if empty, or not lists, do general code.  Never happens.
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
  {A a1,y;I*aa,i,*u,*ww=(I*)wv,*v,*yv,*zv;VA2 adocv,adocvsum;
   b=1;
   adocv=var(ds(CSTAR),at,wt); adocvsum=vains(ds(CPLUS),wt);
   GATV0(a1,INT,m,1); aa=AV(a1); u=m+(I*)av; DO(m, aa[i]=*--u;);
   GATV0(y,INT,MIN(m,n),1); yv=AV(y);
   GATV0(z,INT,zn,1); zv=AV(z);
   for(i=0;i<zn;++i){
    j=MIN(i,m1); u=aa+m1-j; v=ww+i-j;
    p=MIN(1+i,zn-i); p=MIN(p,k);
    ((AHDR2FN*)adocv.f)((I)1,p,u,v,yv,jt); ((AHDRRFN*)adocvsum.f)((I)1,p,(I)1,yv,zv,jt);
    ++zv;
   }
   if(EWOV<=jt->jerr){RESETERR; PMLOOP(I,D,FL, x=*u--*(D)*v++, x+=*u--*(D)*v++);}  // erroneous fa(z) removed; any error >= EWOV will be an overflow
  break;}
 }
 if(t&FL+CMPX)NAN1; RE(0);
 if(!b)R obqfslash(df2(a,w,g),f);
 RETF(z);
}    /* f//.@(g/) for atomic f, g */


static DF2(jtkey);

static DF2(jtkeysp){PROLOG(0008);A b,by,e,q,x,y,z;I j,k,n,*u,*v;P*p;
 RZ(a&&w);
 SETIC(a,n); 
 RZ(q=indexof(a,a)); p=PAV(q); 
 x=SPA(p,x); u=AV(x);
 y=SPA(p,i); v=AV(y);
 e=SPA(p,e); k=i0(e); 
 j=0; DO(AN(x), if(k<=u[i])break; if(u[i]==v[i])++j;);
 RZ(b=ne(e,x));
 RZ(by=repeat(b,y));
 RZ(x=key(repeat(b,x),from(ravel(by),w),self));
 GASPARSE(q,SB01,1,1,(I*)0); *AS(q)=n;  /* q=: 0 by}1$.n;0;1 */
 p=PAV(q); SPB(p,a,iv0); SPB(p,e,num[1]); SPB(p,i,by); SPB(p,x,reshape(tally(by),num[0]));
 RZ(z=over(df1(repeat(q,w),VAV(self)->fgh[0]),x));
 z=j?cdot2(box(IX(1+j)),z):z;
 EPILOG(z);
}

// a u/. w.  Self-classify a, then rearrange w and call cut
static DF2(jtkey){F2PREFIP;PROLOG(0009);A frets,wperm,z;
 RZ(a&&w);
 {I t1,t2; ASSERT(SETIC(a,t1)==SETIC(w,t2),EVLENGTH);}  // verify agreement
 if(SPARSE&AT(a))R keysp(a,w,self);  // if sparse, go handle it
 RZ(a=indexof(a,a));  rifv(a); // self-classify the input using ct set before this verb; we are going to modify a, so make sure it's not virtual
 PUSHCCT(jt->cctdefault);  // now that partitioning is over, reset ct for the executions of u
 // Allocate the area for the reordered copy of the input.  Do these calls early to free up registers for the main loop
 GA(wperm,AT(w),AN(w),AR(w),AS(w)); // Note we could avoid initialization of indirect types, since we are filling it all
 I celllen; PROD1(celllen,AR(w)-1,AS(w)+1); celllen <<= bplg(AT(w));  // length of a cell of w, in bytes
 I nitems=AN(a);
 I nfrets=0;  // We will accumulate # frets found
 UC *fretp;  // pointer to where frets are built

 // pass through the input, incrementing each reference
 // If y is inplaceable we can probably use it to store the frets, since it is copied sequentially.  The requirements are:
 // The fret area must start at least 5 bytes before the data in y, to leave space for the first fret before the first cell of y is moved (i. e. rank>0 for 64-bit, >1 for 32-bit)
 // The cells must be at least 5 bytes long so that the frets don't overrun them
 // The cells must be in the actual fret area, i. e not virtual or NJA 
 // The cells must be DIRECT type so that it is OK to write garbage over them
 // We are taking advantage here of the fact that the shape comes before the cells in a standard GA noun
 // We don't have to worry about a==w because a has already been modified
 if((I)jtinplace&(I )((AFLAG(w)&(AFVIRTUAL|AFNJA))==0)&((UI)((-(I )(AT(w)&DIRECT))&AC(w)&(4-celllen)&((I )(SZI==4)-AR(w)))>>(BW-1-JTINPLACEWX)))frets=w; else GATV0(frets,LIT,nitems,0);   // 1 byte per fret is adequate
 I *av=IAV(a);  // av->a data
 {I *av2, *avend=av+nitems; for(av2=av;av2!=avend;++av2)++av[*av2];}  // first time the root of the partition points to & increments itself
 // Now each item av[i] is either (1) smaller than i, which means that it is extending a previous key; or (2) greater than i, which
 // means it starts a new partition whose length is a[i]-i.  Process the values in order, creating partitions as they come up, and
 // moving the data for each input value in turn, reading in order and scatter-writing.
 fretp=CUTFRETFRETS(frets);  // Place where we will store the fret-lengths.  They are 1 byte normally, or 5 bytes for groups longer than 254
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
   if(avvalue<255)*fretp++ = (UC)avvalue; else{*fretp++ = 255; *(UI4*)fretp=(UI4)avvalue; fretp+=SZUI4; nfrets-=SZUI4;}
   partitionptr=(I*)nextpartitionx;  // copy this item's data to the start of the partition
   nextpartitionx+=avvalue*celllen;  // reserve output space for the partition
   avvalue=i;   // shift meaning of avvalue from length to index, where the partition pointer will be stored
  }

  // copy the data to the end of its partition and advance the partition pointer
  if(celllen<MEMCPYTUNELOOP) {  // copy by hand if that's faster (0 len OK)
   I n=celllen; while((n-=SZI)>=0){*partitionptr++=*wv++;}
     // move full words.  Must not overwrite the area, since we are scatter-writing.
   if(n&(SZI-1)){STOREBYTES(partitionptr,*wv,-n); partitionptr = (I*)((C*)partitionptr+SZI+n); wv = (I*)((C*)wv+SZI+n);}  // Use test because this code is repeated
  }else{MC(partitionptr,wv,celllen); partitionptr = (I*)((C*)partitionptr+celllen); wv = (I*)((C*)wv+celllen);}

  av[avvalue]=(I)partitionptr;  // store updated end-of-partition after move
 }

 // Frets are calculated and w is reordered.  Call cut to finish the job.  We have to store the count and length of the frets
 CUTFRETCOUNT(frets)=fretp-CUTFRETFRETS(frets)+nfrets;  // # frets is #bytes stored, minus the length of the extended encodings
 CUTFRETEND(frets)=(I)fretp;   // pointer to end+1 of data
 // wperm is always inplaceable.  If u is inplaceable, make the call to cut inplaceable
 // We pass the self pointer for /. into cut, as it uses that fact to interpret a
 z=jtcut2((J)(intptr_t)((I)jt+((FAV(self)->flag&VGERL)?0:(FAV(FAV(self)->fgh[0])->flag>>(VJTFLGOK1X-JTINPLACEWX))&JTINPLACEW)),frets,wperm,self);
 POPCCT
 EPILOG(z);
}    /* a f/. w for dense x & w */

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
 x=FAV(self)->fgh[0]; d=vaid(VAV(x)->fgh[0]); if(B01&wt){d=d==CMAX?CPLUSDOT:d; d=(d==CMIN)|(d==CSTAR)?CSTARDOT:d;}
 if(!(AN(a)&&AN(w)&&at&DENSE&&
     (wt&B01&&(d==CEQ||d==CPLUSDOT||d==CSTARDOT||d==CNE||d==CPLUS)||
     wt&SBT&&(d==CMIN||d==CMAX)||
     wt&INT&&(17<=d&&d<=25)||
     wt&INT+FL&&(d==CMIN||d==CMAX||d==CPLUS) )))R key(a,w,self);
 CRT rng=keyrs(a,MAX(2*n,65536)); c=aii(w); at=rng.type; r=rng.minrange.min; s=rng.minrange.range; m=s;
 zt=wt; zt=wt&INT?FL:zt; zt=wt&B01?INT:zt; zt=d==CPLUS?zt:wt;  // type of result, promoting bool and int but only if +
 /* obsolete zt=d==CPLUS?(wt&B01?INT:wt&INT?FL:wt):wt;*/ bb=s!=0;
 if(bb){
  GATV0(b,B01,s,  1); bv=BAV(b); memset(bv,C1,s); bv-=r;
  GA(q,zt, s*c,1,0); qv0=AV(q);
 }else{RZ(x=indexof(a,a)); xv=AV(x); m=0; u=xv; DO(n, *u=i==*u?m++:xv[*u]; ++u;);}
 GA(z,zt,m*c,wr,AS(w)); AS(z)[0]=m; zv0=AV(z);
 if(wt&FL)NAN0;
 PUSHCCT(jt->cctdefault)
 switch(KCASE(d,CTTZ(wt))){
  case KCASE(CEQ,     B01X): KACC(*v==y,    B, B, 1   ); break;
  case KCASE(CPLUSDOT,B01X): KACC(*v||y,    B, B, 0   ); break;
  case KCASE(CSTARDOT,B01X): KACC(*v&&y,    B, B, 1   ); break;
  case KCASE(CNE,     B01X): KACC(*v!=y,    B, B, 0   ); break;
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
  case KCASE(25,      INTX): KACC(~(*v^y),  UI,UI,-1  );
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
 if(!(AN(a)&&AN(w)&&at&DENSE&&wt&B01+INT+FL))R df2(a,w,folk(sldot(slash(ds(CPLUS))),ds(CDIV),sldot(ds(CPOUND))));
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

// indexed by [chartype][k]
UI4 shortrange[3][4] = {{0,65536,65536,0}, {0,2,258,0}, {0,256,65536,0}};  // C2T, B01, LIT

F1(jtgroup){PROLOG(0014);A c,d,x,z,*zv;I**cu,*cv,*dv,j,k,m,n,t,*u,*v,*wv,zn=0;CR rng;
 RZ(w);
 if(SPARSE&AT(w))RZ(w=denseit(w));
 SETIC(w,n); t=AT(w); k=n?aii(w)<<bplg(t):0;
 if(!AN(w)){GATV0(z,BOX,n?1:0,1); if(n)RZ(*AAV(z)=IX(n)); R z;}
 if(2>=k){rng.range=shortrange[t&(B01+LIT)][k]; rng.min = 0;}
 else if(k==sizeof(C4)&&t&C4T){rng=condrange4(C4AV(w),n,-1,0,2*n);}
 else if(k==SZI&&t&INT+SBT){rng=condrange(AV(w),n,IMAX,IMIN,2*n);}
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

#define KEYTALLY(T)     {T*u;                             \
                         u=(T*)av; DQ(n, ++*(qv+*u++););  \
                         u=(T*)av; DQ(n, v=qv+*u++; if(*v){*zv++=*v; *v=0; if(s==++j)break;});}

static DF2(jtkeytally){PROLOG(0016);A q;I at,*av,j=0,k,n,r,s,*qv,*u,*v;
 RZ(a&&w);
 SETIC(a,n); at=AT(a); av=AV(a);
 ASSERT(n==SETIC(w,k),EVLENGTH);
 if(!AN(a))R vec(INT,n?1:0,&n);
 if(at&SPARSE)R keytallysp(a);
 CRT rng = keyrs(a,MAX(2*n,65536)); at=rng.type; r=rng.minrange.min; s=rng.minrange.range;
// obsolete  if(n&&at&B01&&1>=AR(a)){B*b=(B*)av; k=bsum(n,b); R !k||n==k?vci(k?k:n):v2(*b?k:n-k,*b?n-k:k);}  // boolean a, just add the 1s
 if((-n&SGNIF(at,B01X)&(AR(a)-2))<0){B*b=(B*)av; k=bsum(n,b); R (UI)(k-1)>=(UI)(n-1)?vci(n):v2(*b?k:n-k,*b?n-k:k);}  // nonempty rank<2 boolean a, just add the 1s
 if(s){A z;I*zv;
  GATV0(z,INT,s,1); zv=AV(z);
  GATV0(q,INT,s,1); qv=AV(q)-r;
  u=qv+r; DQ(s, *u++=0;);
  switch(CTTZ(at)){
   case LITX: KEYTALLY(UC); break;
   case C2TX: KEYTALLY(US); break;
   case C4TX: KEYTALLY(C4); break;
   case SBTX: KEYTALLY(SB); break;
   case INTX: KEYTALLY(I ); break;
  }
  AN(z)=*AS(z)=j;
  EPILOG(z);
 }
 RZ(q=indexof(a,a)); realizeifvirtual(q);
 if(!AR(q))R iv1; 
 v=qv=AV(q);
 u=qv; DQ(n, ++*(qv+*u++););
 u=qv; DO(n, k=*u++; if(i<k){j+=*v++=k-i; if(n==j)break;});
 *AS(q)=AN(q)=v-qv;
 EPILOG(q);
}    /* x #/.y main control & dense x */


#define KEYHEADTALLY(Tz,Ta,Tw,exp0,exp1)  \
 {Ta*u;Tw*wv=(Tw*)AV(w);Tz*zz=(Tz*)zv;    \
  u=(Ta*)av; DQ(n, ++*(qv+*u++););        \
  u=(Ta*)av; DO(n, v=qv+*u++; if(*v){*zz++=exp0; *zz++=exp1; k+=*v; if(n==k)break; *v=0;}); \
  AN(z)=zz-(Tz*)zv;                       \
 }

static DF2(jtkeyheadtally){PROLOG(0017);A f,q,x,y,z;B b;I at,*av,k,n,r,s,*qv,*u,*v,wt,*zv;
 RZ(a&&w);
 SETIC(a,n); wt=AT(w);
 ASSERT(n==SETIC(w,k),EVLENGTH);
 ASSERT(!n||wt&NUMERIC,EVDOMAIN);
 if(SPARSE&AT(a)||1<AR(w)||!n||!AN(a))R key(a,w,self);
 CRT rng = keyrs(a,MAX(2*n,65536)); at=rng.type; r=rng.minrange.min; s=rng.minrange.range;
 av=AV(a); 
 f=FAV(self)->fgh[0]; f=VAV(f)->fgh[0]; b=CHEAD==ID(f);
 if(at&B01&&1>=AR(a)){B*c,*d,*p=(B*)av;I i,j,m;
  c=d=p;
  if(*p){i=0; d=(B*)memchr(p,C0,n); j=d?d-p:0;}
  else  {j=0; c=(B*)memchr(p,C1,n); i=c?c-p:0;}
  k=bsum(n,p); m=c&&d?2:1;
  GATV0(x,INT,m,1); v=AV(x); *v++=MIN(i,j);      if(c&&d)*v=MAX(i,j); 
  GATV0(y,INT,m,1); v=AV(y); *v++=i<j||!d?k:n-k; if(c&&d)*v=i<j?n-k:k;
  R stitch(b?from(x,w):y,b?y:from(x,w));
 }
 if(at&LIT+C2T+C4T+INT+SBT&&wt&B01+INT+FL&&s){
  GA(z,wt&FL?FL:INT,2*s,2,0); zv=AV(z);
  GATV0(q,INT,s,1); qv=AV(q)-r;
  u=qv+r; DQ(s, *u++=0;); k=0;
  r=0; r=at&C2T?3:r; r=at&INT?6:r; r=at&C4T?9:r; r=at&SBT?12:r; 
// obsolete   switch(15*b+(at&SBT?12:at&C4T?9:at&INT?6:at&C2T?3:0)+(wt&FL?2:wt&INT?1:0)){
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
 }else{
  RZ(q=indexof(a,a));
  x=repeat(eq(q,IX(n)),w); y=keytally(q,q,0L); z=stitch(b?x:y,b?y:x);
 }
 EPILOG(z);
}    /* x ({.,#)/.y or x (#,{.)/.y */


F1(jtsldot){A h=0;AF f1=jtoblique,f2;C c,d,e;I flag=0;V*v;
// NOTE: u/. is processed using the code for u;.1 and passing the self for /. into the cut verb.  So, the self produced
// by /. and ;.1 must be the same as far as flags etc.
 RZ(w);
 if(NOUN&AT(w)){flag=VGERL; RZ(h=fxeachv(1L,w));}
 v=VAV(w);
 switch(ID(w)){  // no default for f2: every path must set it
  case CPOUND: f2=jtkeytally; break;
  case CSLASH: f2=jtkeyslash; if(vaid(v->fgh[0]))f1=jtobqfslash; break;
  case CFORK:  if(v->valencefns[0]==(AF)jtmean){f2=jtkeymean; break;}
               c=ID(v->fgh[0]); d=ID(v->fgh[1]); e=ID(v->fgh[2]); 
               if(d==CCOMMA&&(c==CHEAD&&e==CPOUND||c==CPOUND&&e==CHEAD)){f2=jtkeyheadtally; break;}
               // otherwise fall through to...
  default: f2=jtkey; flag |= (FAV(w)->flag&VASGSAFE)|VJTFLGOK2;  // pass through ASGSAFE.  jtkey can handle inplace
 }
 R fdef(0,CSLDOT,VERB, f1,f2, w,0L,h, flag, RMAX,RMAX,RMAX);
}
