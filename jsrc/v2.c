/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Primes and Factoring                                             */

#include "j.h"

#define MM   25000L     /* interval size to look for primes        */
#define PMAX 105097564L /* upper limit of p: ; (_1+2^31) = p: PMAX */
#define PT   500000L    /* interval size in ptt                    */


static const I ptt[]={
    7368791L,   15485867L,   23879539L,   32452867L,   41161751L,
   49979693L,   58886033L,   67867979L,   76918277L,   86028157L,
   95189093L,  104395303L,  113648413L,  122949829L,  132276713L,
  141650963L,  151048973L,  160481219L,  169941223L,  179424691L, /*  10e6 */
  188943817L,  198491329L,  208055047L,  217645199L,  227254213L,
  236887699L,  246534487L,  256203221L,  265892021L,  275604547L,
  285335587L,  295075153L,  304836293L,  314606891L,  324407131L,
  334214467L,  344032391L,  353868019L,  363720403L,  373587911L, /*  20e6 */
  383446691L,  393342743L,  403245719L,  413158523L,  423087253L,
  433024253L,  442967117L,  452930477L,  462900953L,  472882049L,
  482873137L,  492876863L,  502895647L,  512927377L,  522960533L,
  533000401L,  543052501L,  553105253L,  563178743L,  573259433L, /*  30e6 */
  583345003L,  593441861L,  603538541L,  613651369L,  623781269L,
  633910111L,  644047709L,  654188429L,  664338817L,  674506111L,
  684681301L,  694847539L,  705031199L,  715225741L,  725420411L,
  735632797L,  745843009L,  756065179L,  766301759L,  776531419L, /*  40e6 */
  786760649L,  797003437L,  807247109L,  817504253L,  827772511L,
  838041647L,  848321921L,  858599509L,  868891399L,  879190841L,
  889495223L,  899809363L,  910112683L,  920419823L,  930754037L,
  941083987L,  951421147L,  961748941L,  972092467L,  982451707L, /*  50e6 */
  992801861L, 1003162837L, 1013526181L, 1023893887L, 1034271001L,
 1044645419L, 1055040229L, 1065433427L, 1075824283L, 1086218501L,
 1096621151L, 1107029839L, 1117444369L, 1127870683L, 1138305547L,
 1148739817L, 1159168537L, 1169604841L, 1180041943L, 1190494771L, /*  60e6 */
 1200949609L, 1211405387L, 1221863261L, 1232332813L, 1242809783L,
 1253270833L, 1263751141L, 1274224999L, 1284710771L, 1295202523L,
 1305698249L, 1316196209L, 1326697579L, 1337195527L, 1347701867L,
 1358208613L, 1368724913L, 1379256029L, 1389786323L, 1400305369L, /*  70e6 */
 1410844907L, 1421376533L, 1431916091L, 1442469313L, 1453010737L,
 1463555011L, 1474118929L, 1484670179L, 1495213271L, 1505776963L,
 1516351777L, 1526922017L, 1537493917L, 1548074371L, 1558655507L,
 1569250363L, 1579833509L, 1590425983L, 1601020433L, 1611623887L, /*  80e6 */
 1622223991L, 1632828059L, 1643429663L, 1654054511L, 1664674819L,
 1675293223L, 1685912299L, 1696528907L, 1707155683L, 1717783153L,
 1728417367L, 1739062387L, 1749701927L, 1760341447L, 1770989611L,
 1781636627L, 1792287229L, 1802933621L, 1813593029L, 1824261419L, /*  90e6 */
 1834925117L, 1845587717L, 1856264467L, 1866941123L, 1877619461L,
 1888303063L, 1898979371L, 1909662913L, 1920354661L, 1931045239L,
 1941743653L, 1952429177L, 1963130473L, 1973828669L, 1984525423L,
 1995230821L, 2005933283L, 2016634099L, 2027354369L, 2038074751L, /* 100e6 */
 2048795527L, 2059519673L, 2070248617L, 2080975187L, 2091702673L,
 2102429887L, 2113154951L, 2123895979L, 2134651583L, 2145390539L,
};   /* p: PT*1+i.210 */

#define ptn (I)(sizeof(ptt)/sizeof(ptt[0]))

static I jtsup(J jt,I n,I*wv){I c,d,j,k;
 c=0; DO(n, j=wv[i]; ASSERT(0<=j,EVDOMAIN); if(c<j)c=j;); 
 ASSERT(c<=PMAX,EVLIMIT);
 j=1; k=0; DQ(128, if(c<=j)break; j+=j; ++k;); d=c*k; 
 R k&&c>d?IMAX:MAX(d,135L);
}    /* (_1+2^31) <. 135 >. (*>.@(2&^.)) >./ w */

static void sieve(I n,I m,B*b,B*u){I i,j,q;
 static const B t[]={
  0,1,0,0,0, 0,0,1,0,0, 0,1,0,1,0, 0,0,1,0,1, 0,0,0,1,0, 0,0,0,0,1,
  0,1,0,0,0, 0,0,1,0,0, 0,1,0,1,0, 0,0,1,0,1, 0,0,0,1,0, 0,0,0,0,1};
 mvc(m,b,30L,(C*)t+n%30); if(!n)b[1]=0; q=1+(I)sqrt(n+(D)m);
 if(n)for(i=7;i<q;i+=2){if(u[i]){j=n%i; j=j?i-j:0; while(j<m){b[j]=0; j+=i;}}}
 else for(i=7;i<q;i+=2){if(b[i]){j=i+i;            while(j<m){b[j]=0; j+=i;}}}
}    /* sieve b for n+i.m, but if 0=n then b=. 0 (2 3 5)}b */

static F1(jtprime1){A d,t,y,z;B*b,*u;I c,*dv,e,i,j,k,m,n,p,q,*wv,x,*zv;
 ARGCHK1(w);
 k=0; n=AN(w); wv=AV(w); RE(m=sup(n,wv)); RESETRANK; JBREAK0;
 I zr=AR(w); GATV(z,INT,n,AR(w),AS(w)); zv= AVn(zr,z);
 RZ(d=grade1(ravel(w)));  dv= AV(d);
 if(JT(jt,p4792)){I*u=AV(JT(jt,p4792)); c=AN(JT(jt,p4792)); while(n>k&&c>(x=wv[dv[k]]))zv[dv[k++]]=u[x];}
 else{
  while(n>k&&0==wv[dv[k]])zv[dv[k++]]=2;
  while(n>k&&1==wv[dv[k]])zv[dv[k++]]=3;
  while(n>k&&2==wv[dv[k]])zv[dv[k++]]=5;
 }
 if(n==k){RETF(z);} 
 j=3; p=0; e=PT; q=1+(I)sqrt((D)m); x=wv[dv[k]]; 
 GATV0(t,B01,q,1);         u=BAV1(t); sieve(0L,q,u,u);
 #undef DF1
 GATV0(y,B01,MIN(m,MM),1); b=BAV1(y); 
 for(;BETWEENO(p,0,m);p+=q){
  if(x>=e){c=x/PT; e=PT*(1+c); c=MIN(c,ptn); if(j<c*PT){j=c*PT; p=ptt[c-1];}}
  #define DF1(f) A jt##f##es(J w,A jt)
  JBREAK0; q=MIN(MM,m-p); sieve(p,q,b,u); c=j+q/3;
  if(x>c)for(i=1-(p&1);i<q;i+=2)j+=b[i];
  else   for(i=1-(p&1);i<q;i+=2)
   if(b[i]){while(j==x){zv[dv[k++]]=i+p; if(n==k)R z; x=wv[dv[k]];} ++j;}
 }
 while(n>k)zv[dv[k++]]=p; RETF(z);
}

#if SY_64
#define TOTALPRIMES 30000  // pollard beats sieve when the values are big enough
#define MAXIFACTOR (350411ULL*350411ULL-1ULL)  // <: *: p: >: TOTALPRIMES, which is the largest value we can reliably factor using our prime table
#define N64BITRECIPS (4792-((4792-54)&-8))   // we process 72-bit recips in batches of 8; this many 64-bit reciprs aligns
#else
#define MAXIFACTOR 0x7fffffff  // in 32-bit we can factor any I
#endif
// init prime table.  The prime table contains:
// the first 4792 primes, which gets up to above (%: <: 2^31)
//  followed on 64-bit by:
// the 64-bit reciprocal of each prime.  The binary point is at bit 64 for the first 54, at bit 72 after that
// the primes up to prime 1000000, encoded as the prime gap: nnn->2-14, nnn000->16-28, nnnnnn000000->30-156 (154 is the largest gap found in the first 1e6)
static I init4792(J jt) {
 if(!JT(jt,p4792)){  // init only once
  RZ(JT(jt,p4792)=prime1(IX(4792L)));  // init early to speed the sieve
#if SY_64
  A p; RZ(p=prime1(IX(TOTALPRIMES))); I *pv=IAV(p);  // get the first million.  If we do more we must change the encoding if the max gap increases
  A z; GATV0(z,INT,2*4792+((TOTALPRIMES*6)/BW)+100,1); AS(z)[0]=AN(z)=4792; I *zv=IAV1(z);  // the length is of the small primes, plus some slop
  zv[0]=pv[0]; zv[4792+0]=0x8000000000000000;  // first reciprocal is exact: don't round up
  UI q1,r1;
  DO(N64BITRECIPS-1, zv[i+1]=pv[i+1]; q1=0x8000000000000000/pv[i+1]; r1=0x8000000000000000%pv[i+1]; zv[4792+i+1]=q1*2+(r1*2/pv[i+1])+1;)  // rest of the first group are rounded up
  DO(4792-N64BITRECIPS, zv[i+N64BITRECIPS]=pv[i+N64BITRECIPS];  q1=0x8000000000000000/pv[i+N64BITRECIPS]; r1=0x8000000000000000%pv[i+N64BITRECIPS]; zv[4792+i+N64BITRECIPS]=q1*512+(r1*512/pv[i+N64BITRECIPS])+1;)  // second group is rounded up with 8 upper zeros implied
  UI bits=0, nbits=0; US *out=(US*)&zv[2*4792];  // bitstream and output pointer 
  DO(TOTALPRIMES-4792, I gap=(pv[i+4792]-pv[i+4792-1])>>1; I encval=gap; encval=gap>7?(gap-7)*8:encval; encval=gap>14?(gap-15)*64:encval; I enclen=gap>7?6:3; enclen=gap>14?12:enclen;  // encoded bits and count
   bits|=encval<<nbits; nbits+=enclen; if(nbits>=16){*out++=(US)bits; bits>>=16; nbits-=16;}
  )
  bits|=0xfc0<<nbits; *out++=(US)bits; *out++=(US)(bits>>16);  // add stopper value, out the remnant
  JT(jt,p4792)=z;  // save complete table
#endif
 ACX(JT(jt,p4792));
 } R 1;
}

static I rem(D x,I d){R (I)jfloor(x-d*jfloor(x/(D)d));}

static void sieved(D n,I m,B*b){I c,d,e,i,q,r,*u,*v;
 static const B t[]={
  0,1,0,0,0, 0,0,1,0,0, 0,1,0,1,0, 0,0,1,0,1, 0,0,0,1,0, 0,0,0,0,1,
  0,1,0,0,0, 0,0,1,0,0, 0,1,0,1,0, 0,0,1,0,1, 0,0,0,1,0, 0,0,0,0,1};
 static const I dt[]={1,7,11,13,17,19,23,29};
 c=2*3*5; v=(I*)dt+sizeof(dt)/SZI; q=1+(I)sqrt(n+(D)m);
 mvc(m,b,30L,(B*)t+rem(n,30L));
 for(i=c;i<q;i+=c){
  u=(I*)dt;
  while(u<v){
   d=i+*u++; r=rem(n,d); e=r?d-r:0;
   while(d<m){b[e]=0; e+=d;}
 }}
}    /* sieve b for n+i.m; n>0; u is mask for primes */

static F1(jtprime1d){A d,z;D*wv,x,*zv;I*dv,k,n;
 ARGCHK1(w);
 n=AN(w); wv=DAV(w);
 I zr=AR(w); GATV(z,FL,n,AR(w),AS(w)); zv=DAVn(zr,z);
 RZ(d=grade1(ravel(w))); dv=AV(d);
 k=0; while(n>k&&(D)PMAX>=wv[dv[k]])++k;
 if(k){A y;I*yv;
  RZ(y=prime1(cvt(INT,from(take(sc(k),d),ravel(w))))); yv=AV(y);
  DO(k, zv[dv[i]]=(D)*yv++;);
 }
 if(k==n)R z;
 DO(n-k, x=wv[dv[i]]; ASSERT(0<=x&&x!=inf,EVDOMAIN););
 ASSERT(0,EVLIMIT);
}

F1(jtprime){PROLOG(0061);A z;B b=1;I n,t;
 ARGCHK1(w);
 RZ(init4792(jt));
 n=AN(w); t=AT(w);
 if(!ISDENSETYPE(t,INT))RZ(w=pcvt(INT,w));
 if(INT&AT(w)){
  // if the maximum in the argument is <= PMAX, call prime1.  Force minimum of interval to <=0
  // so that full range compares against PMAX
  if(condrange(AV(w),n,0,IMIN,PMAX).range){b=0; RZ(z=prime1(w));}
 }
 if(b)RZ(z=prime1d(ISDENSETYPE(AT(w),FL)?w:ccvt(FL,w,0)));
 if(t&XNUM+RAT)RZ(z=cvt(XNUM,z));
 EPILOG(z);
}    /* p:"r w */

static I jtsuq(J jt,I n,I*wv){I c=24; DO(n, c=MAX(c,wv[i]););  R c==0x7fffffff?c:1+c;}
     /* 1+24>.>./w */

F1(jtplt){PROLOG(0062);A d,t,y,z;B*b,*u,xt;I c,*dv,e,i,j,k,m,n,p,q,*wv,x,*zv;
 ARGCHK1(w);
 xt=1&&AT(w)&XNUM+RAT;
 if(!(INT&AT(w)))RZ(w=vi(ceil1(w))); wv=AV(w); JBREAK0;
 j=3; k=p=c=0; e=*ptt; n=AN(w); 
 RE(m=suq(n,wv)); ASSERT(m<=0x7fffffff,EVLIMIT); q=1+(I)sqrt((D)m); 
 GATV0(t,B01,q,1);         u =BAV1(t); sieve(0L,q,u,u);
 GATV0(y,B01,MIN(m,MM),1); b =BAV1(y); 
 I zr=AR(w); GATV(z,INT,n,AR(w),AS(w)); zv= AVn(zr,z);
 RZ(d=grade1(ravel(w)));  dv= AV(d);
 while(n>k&&2>=wv[dv[k]])zv[dv[k++]]=0; 
 while(n>k&&3>=wv[dv[k]])zv[dv[k++]]=1; 
 while(n>k&&5>=wv[dv[k]])zv[dv[k++]]=2; 
 if(n==k){EPILOG(z);} x=wv[dv[k]]; 
 for(;BETWEENO(p,0,m);p+=q){
  if(x>=e){
   while(ptn>c&&x>=ptt[c])++c; 
   if(j<c*PT){p=ptt[c-1]; e=c<ptn?ptt[c]:IMAX; j=c*PT;}
  }
  JBREAK0; q=MIN(MM,m-p); sieve(p,q,b,u);
  if(x>p+q)
   for(i=1-(p&1);i<q;i+=2)j+=b[i];
  else     
   for(i=1-(p&1);i<q;i+=2)if(b[i]){
    while(x<=p+i){zv[dv[k++]]=j; if(n==k){i=q; break;} x=wv[dv[k]];}
    ++j;
 }}
 while(n>k)zv[dv[k++]]=j; 
 if(xt)RZ(z=cvt(XNUM,z));
 EPILOG(z);
}    /* p:^:_1 w, the number of primes less than w */


static B jtxprimeq(J,I,X);
static F1(jtxprimetest);

static const B pmsk[]={0,0,1,1,0,1,0,1,0,0, 0,1,0,1,0,0,0,1,0,1, 0,0,0,1,0,0,0,0,0,1, 0,1};
     /* indicates which i<32 is prime */

static F1(jtiprimetest){A z;B*b;I d,j,n,*pv,q,*v,wn,*wv;
 ARGCHK1(w);
 wn=AN(w); wv=AV(w); pv=AV(JT(jt,p4792));
#if SY_64
 DO(wn, if(2147483647L<wv[i])R xprimetest(cvt(XNUM,w)););
#endif
 I zr=AR(w); GATV(z,B01,wn,AR(w),AS(w)); b=BAVn(zr,z);
 for(j=0;j<wn;++j){
  n=*wv++; v=pv;
  if(32>n)b[j]=pmsk[MAX(0,n)];
  else{b[j]=1; DQ(AN(JT(jt,p4792)), d=*v++; q=n/d; if(n==q*d){b[j]=0; break;}else if(q<d)break;);}
 }
 RETF(z);
}

static F1(jtxprimetest){
 ARGCHK1(w);
 I wn=AN(w);I wt=AT(w);X*wv=XAV(w);I*pv=AV(JT(jt,p4792)); 
 B rat=1&&wt&RAT; X xmaxint;RZ(xmaxint=xc(2147483647L)); X y;RZ(y=xc(-1L));I*yv=AV(y);
 A z;I zr=AR(w); GATV(z,B01,wn,AR(w),AS(w)); B*b=BAVn(zr,z);
 for(I j=0;j<wn;++j){
  X x= *wv++; I s= XSGN(x); b[j]= 1; I*v= pv;
  if(rat) {
   ASSERT(XSGN(*wv),EVDOMAIN);
   if(!ISX1(*wv++)){b[j]=0;continue;}
  }
  if(0>=s)b[j]=0;
  else if(1==xcompare(x,xmaxint)){
   A *old=jt->tnextpushp;
   DQ(100, *yv=*v++; X r=xrem(y,x);RZ(r); if(!AV(r)[0]){b[j]=0; break;});
   if(b[j])RE(b[j]=xprimeq(100L,x));
   tpop(old);
  }else{
   I n=xint(x);I*v=pv;I d=XLIMB0(x);
   if(32>n)b[j]=pmsk[MAX(0,n)];
   else DQ(AN(JT(jt,p4792)), d=*v++; I q=n/d; if(n==q*d){b[j]=0; break;}else if(q<d)break;);
 }}
 RETF(z);
}    /* prime test for extended integers or rationals */

static F1(jtprimetest){A x;I t;
 ARGCHK1(w);
 t=AT(w);
 if((UI)SGNIF(t,B01X)>=(UI)AN(w))R reshape(shape(w),num(0));  // AN is 0, or t is boolean
 switch(CTTZ(t)){
 case INT2X: case INT4X: RZ(w=cvt(INT,w))  // convert I2, I4 to long int & fall through to I
 case INTX:           R iprimetest(w);
 default:             ASSERT(0,EVDOMAIN);
 case RATX: case XNUMX: R xprimetest(w);
 case FLX:  case CMPXX: case QPX:
  {PUSHCCT(1.0-FUZZ) x=eq(t&FL?w:conjug(w),floor1(w)); POPCCT}
  R xprimetest(cvt(XNUM,tymes(w,x))); 
 }
}   /* primality test */


static F1(jtnextprime){
 ARGCHK1(w);
 I n=AN(w);
 if(unlikely((UI)SGNIF(AT(w),B01X)>=(UI)AN(w)))R reshape(shape(w),num(2));
 ASSERT(NUMERIC&AT(w),EVDOMAIN);
 A fs;RZ(fs=eval("2&+^:(0&p:)^:_"));   // create verb to test for primes, adding 2 each time
 if(unlikely(AT(w)&INT2+INT4))RZ(w=cvt(INT,w))
 if(INT&AT(w)){
  A x;I xr=AR(w); GATV(x,INT,n,AR(w),AS(w)); I*xv=AVn(xr,x);
  B b=1;I*wv=AV(w); // clear b if we would overflow int representation
  I k;DQ(n, k=*wv++; if(k>=IMAXPRIME){b=0; break;}else *xv++=2>k?2:(k+1)|1;);
  if(b)R rank1ex0(x,fs,FAV(fs)->valencefns[0]); // while not prime add 2
  RZ(w=cvt(XNUM,w));
 }
 if(ISDENSETYPE(AT(w),FL+RAT+QP))RZ(w=cvt(XNUM,floor1(maximum(num(0),w))));
 if(ISDENSETYPE(AT(w),CMPX))RZ(w=cvt(XNUM,floor1(maximum(num(0),ccvt(FL,w,0)))));
 A x;I xr=AR(w); GATV(x,XNUM,n,AR(w),AS(w)); X*xv=XAVn(xr,x);X*wv=XAV(w);
#define oddnext(Y) 1>XSGN(Y) ?X2 :XaddXX(y,(1&XLIMB0(Y))&&(2<XSGN(Y)||2<XLIMB0(Y)) ?X2 :X1)
 DQ(n,X y=*wv++;*xv++= oddnext(y);); // smallest candidate for next prime
#undef oddnext
 R rank1ex0(x,fs,FAV(fs)->valencefns[0]); // while not prime add 2
}

static F1(jtprevprime){
 ARGCHK1(w); I n=AN(w); if(!n)R w; ASSERT(NUMERIC&AT(w)&&!(B01&AT(w)),EVDOMAIN);
 A fs;RZ(fs=eval("_2&+^:(0&p:)^:_"));
 if(unlikely(AT(w)&INT2+INT4))RZ(w=cvt(INT,w))
 if(INT&AT(w)){
  A x;I xr=AR(w); GATV(x,INT,n,AR(w),AS(w));I*xv=AVn(xr,x);I*wv=AV(w);
  DQ(n, I k=*wv++; ASSERT(2<k,EVDOMAIN); *xv++=3==k?2:(k-2)|1;);
  R rank1ex0(x,fs,FAV(fs)->valencefns[0]);
 }
 if(ISDENSETYPE(AT(w),FL+RAT+QP))RZ(w=cvt(XNUM,ceil1(w)));
 if(ISDENSETYPE(AT(w),CMPX))RZ(w=cvt(XNUM,ceil1(ccvt(FL,w,0))));
 A x;I xr=AR(w); GATV(x,XNUM,n,AR(w),AS(w));X*xv=XAVn(xr,x);X*wv=XAV(w);
 DQ(n,
  A y=*wv++;
  I m=XSGN(y);
  ASSERT(0<m,EVDOMAIN);
  UI k=*UIAV1(y); if (1<m && 5>k) k+= 4;
  ASSERT(2<k, EVDOMAIN);
  *xv++=XsubXX(y, (k&1 && k>3) ?X2 :X1);
 );
 R rank1ex0(x,fs,FAV(fs)->valencefns[0]);
}

static F1(jttotient){A b,x,z;B*bv,p=0;I k,n,t;
 ARGCHK1(w);
 n=AN(w); t=AT(w);
 if(t&B01)RCA(w);
 I br=AR(w); GATV(b,B01,n,AR(w),AS(w)); bv=BAVn(br,b);
 if(t&INT){I*wv=AV(w),*xv;
  I xr=AR(w); GATV(x,INT,n,AR(w),AS(w)); xv=AVn(xr,x);
  DQ(n, k=*wv++; ASSERT(0<=k,EVDOMAIN); if(k){*bv++=1; *xv++=k;}else{*bv++=0; *xv++=1; p=1;};);
 }else{X*xv,y;
  RZ(x=cvt(XNUM,w)); xv=XAV(x);
  DO(n, y=xv[i]; k=XSGN(y); ASSERT(0<=k,EVDOMAIN); if(k)*bv++=1; else{*bv++=0; xv[i]=X1; p=1;});
 }
 A z0; z=cvt(AT(x),dfv1(z0,x,eval("(- ~:)&.q:\"0"))); 
 R p?tymesW(b,z):z;
}

/*
MillerRabin=: 100&$: : (4 : 0) " 0
 if. 0=2|y do. 2=y return. end.
 if. 74>y do. y e. i.&.(p:^:_1) 74 return. end.
 e=. huo y-1
 for_a. x witnesses y do. if. (+./c=y-1) +: 1={:c=. a y&|@^ e do. 0 return. end. end.
 1
)
*/

static B jtspspd(J jt,I b,I n,I d,I h){D a,n1,nn,x;
 if(b==n)R 1;
 a=1; x=(D)b; nn=(D)n; n1=(D)n-1;
 while(d){if(1&d)a=fmod(a*x,nn); x=fmod(x*x,nn); d>>=1;}
 if(a==1||a==n1)R 1;
 DQ(h-1, a=fmod(a*a,nn); if(a==n1)R 1;);
 R 0;
}

static B jtspspx(J jt,I b,I n,I d,I h){I ai,n1;X a,ox,xn;
 if(b==n)R 1;
 n1=n-1;
 ox=jt->xmod; jt->xmod=cvt(XNUM,sc(n)); a=xpow(xc(b),xc(d)); jt->xmod=ox;
 ai=xint(a);
 if(ai==1||ai==n1)R 1;
 xn=xc(n);
 DQ(h-1, a=xrem(xn,xtymes(a,a)); if(xint(a)==n1)R 1;);
 R 0;
}

static F1(jtdetmr){A z;B*zv;I d,h,i,n,wn,*wv;
 RZ(w=vi(w));
 wn=AN(w); wv=AV(w);
 I zr=AR(w); GATV(z,B01,wn,AR(w),AS(w)); zv=BAVn(zr,z);
 for(i=0;i<wn;++i){
  n=*wv++;
  if(1>=n||!(1&n)||0==n%3||0==n%5){*zv++=0; continue;}
  h=0; d=n-1; while(!(1&d)){++h; d>>=1;}
  if     (n< 9080191)*zv++=spspd(31,n,d,h)&&spspd(73,n,d,h);
  else if(n<94906266)*zv++=spspd(2 ,n,d,h)&&spspd( 7,n,d,h)&&spspd(61,n,d,h);
  else               *zv++=spspx(2 ,n,d,h)&&spspx( 7,n,d,h)&&spspx(61,n,d,h);
 }
 RE(0); R z;
}    /* deterministic Miller-Rabin */ 

F2(jtpco2){A z;B*b;I k;
 ARGCHK2(a,w);
 RZ(init4792(jt));
 RE(k=i0(a));
 switch(k){
 default: ASSERT(0,EVDOMAIN);
 case -4: R prevprime(w);
 case -1: R plt(w);
 case  0: R notW(primetest(w));
 case  1: R primetest(w);
 case  2: R qco2(scf(infm),w);
 case  3: R factor(w);
 case  4: R nextprime(w);
 case  5: R totient(w);
 case  6: R detmr(w);
 }
}   /* a p: w */

static A jtqco2x(J jt,I m,A w){A y;I c,*dv,i,*pv,*yv;X d,q,r,x;
 RZ(init4792(jt));
 if(!(XNUM&AT(w)))RZ(w=cvt(XNUM,w));
 x=XAV(w)[0]; pv=AV(JT(jt,p4792)); RZ(d=xc(2L)); dv=AV(d);
 GATV0(y,INT,m,1); yv=AV1(y); mvc(m*SZI,yv,MEMSET00LEN,MEMSET00);
 for(i=0;i<m;++i){
  c=0; *dv=pv[i];
  while(1){RZ(xdivrem(x,d,&q,&r)); if(!ISX0(r))break; ++c; x=q;}
  yv[i]=c; if(ISX1(x))break;
 }
 R cvt(XNUM,y);
}    /* m q: w where 0<:m and p: m is one xdigit and w is a single extended integer */

F2(jtqco2){A q,y,z;B b,bb,xt;I c,j,k,m,*qv,wn,wr,*yv,*zv;
 ARGCHK2(a,w);
 wn=AN(w); wr=AR(w); b=all1(lt(a,zeroionei(0))); xt=1&&AT(w)&XNUM+RAT;  // b: every a is < 0
 if(AR(a)||wr&&(b||xt))R rank2ex0(a,w,DUMMYSELF,jtqco2);
 if(!b&&xt){RE(m=i0(vib(a))); if(BETWEENO(m,0,1229))R qco2x(m,w);}  /* 1229=p:^:_1 XBASE */
 RZ(q=factor(w)); qv=AV(q);  // q is factors of w
 if(b)RZ(a=negate(a));
 bb=equ(a,ainf);  // bb: a is _ or __
 if(b&bb){ /* __ q: w */
  RZ(y=ne(q,curtail(over(zeroionei(0),q))));
  R lamin2(repeat(y,q),dfv1(z,y,cut(ds(CPOUND),zeroionei(1))));
 }
 RZ(y=vi(plt(q))); yv=AV(y);
 k=-1; DO(AN(y), if(k<yv[i])k=yv[i];); ++k;
 if(bb)m=k; else RE(m=i0(a));
 if(b){
  q=repeat(ge(y,sc(k-m)),q);
  R lamin2(nub(q),dfv2(z,q,q,sldot(ds(CPOUND))));
 }else{
  GATV0(z,INT,wn*m,1+wr); MCISH(AS(z),AS(w),wr) AS(z)[wr]=m; zv=AVn(1+wr,z);  // avoid overfetch of AS(w)
  mvc(AN(z)*SZI,zv,MEMSET00LEN,MEMSET00);
  j=0; c=AS(q)[wr]; DQ(wn, DQ(c, if(qv[j]>1&&m>yv[j])++zv[yv[j]]; ++j;); zv+=m;);  // ignore values of 0 or 1, which are fill
  RETF(AT(w)&XNUM+RAT?cvt(XNUM,z):z);
}}   /* a q: w for array w */

static F1(jtxfactor);

F1(jtfactor){PROLOG(0063);A y,z;
 ARGCHK1(w);
 if(unlikely(AN(w)==0)){R dfv1(z,w,qq(mtv,zeroionei(0)));}  // if w empty return an empty list for each value
 RZ(init4792(jt));
 if(AT(w)&XNUM+RAT)R xfactor(w);  // if XNUM, use elliptic routines
 if(ISDENSETYPE(AT(w),FL+CMPX+QP)){  //  only integers can be factored
  RZ(y=pcvt(INT,w)); 
  if(INT&AT(y))w=y; 
  else{RZ(y=pcvt(XNUM,xco1(w))); ASSERT(XNUM&AT(y),EVDOMAIN); R pcvt(INT,xfactor(y));}
 }
 if(unlikely(AT(w)&INT2+INT4))RZ(w=cvt(INT,w))
 F1RANKSUFF(0,jtfactor,DUMMYSELF,R maximum(z,zeroionei(1)););
 // from here on we are operating on a single atom in w
 RZ(w=vi(w));
 I nn=BIV0(w); ASSERT(nn>0,EVDOMAIN)  // n=value to factor
 UI n=nn;  // use unsigned for the rest
 UI *u=AV(JT(jt,p4792));
 I factors[BW-1]; I *v=factors;   // max number of factors is (BW-2) since 2^(BW-1) is IMIN
#if SY_64
 UI d,q; UI i; 
 // first batch: 64-bit reciprocals
 i=0; do{UI xx; UI r=u[i+4792]; d=u[i]; DPUMULU(r,n,xx,q) if(unlikely(n==d*q)){*v++=u[i]; n=q; continue;} if(q<d)goto endfac; ++i;}while(i<N64BITRECIPS);  // stop on 72-bit recip
 // second batch: 72-bit reciprocals
#define CKPR(ndx) {UI xx; UI r=u[i+4792+ndx]; DPUMULU(r,n,xx,q) if(unlikely((q&0xff)==0))if(n==(q>>=8)*(d=u[i+ndx])){*v++=d; n=q; continue;}}
 do{CKPR(0) CKPR(1) CKPR(2) CKPR(3) CKPR(4) CKPR(5) CKPR(6) CKPR(7) if(q<u[i+7])goto endfac; i+=8;}while(i<4792);  // repeat groups of 8 until repeated factors removed
 // third batch: gap-encoded primes
 d=u[4792-1]; US *gv=(US*)&u[4792*2]; UI bits=*gv++; UI nbits=16;  // stack of bits to work on
 while(1){
  // d is the previous prime.  We decode the gap and add to get the next prime.
  // The carried dependency is the number of bits in the stack, which controls whether the address is incremented.  We update this as quickly as possible.
  // The other dependency is the divide unit
  I code=bits;  // bits we will decode.  nbits is always in the range 0x10-0x2f here
  bits|=(UI)*gv<<nbits;  // fetch 16 bits of gap data, append to end of list.  We may append the same value multiple times
  I takebits=nbits&0x10; gv+=takebits>>4; nbits+=takebits;  // if nbits is 0x10-0x1f, accept the new bits and advance to next 16
  I codesize=2*((code&0x3f)==0)+((code&0x7)==0); nbits-=3*codesize+3; bits>>=3*codesize+3;  // codesize is 0/1/3; code length is 3 6 or 12
  // decode the gap length.  It would be OK to fetch from a table since this is out of the dependency loop
  I maskshiftbias=0x3f6f000007370700>>(codesize<<4);  // mask (8), shift (4), bias(4) for each code length
  I gap=(((code>>((maskshiftbias>>4)&0xf))&((maskshiftbias>>8)&0xff))+(maskshiftbias&0xf))<<1;  // gap till next prime
  if(gap==156)break;  // max gap in first million is 154: this is the end code
  d+=gap;  // value of next prime
  while(n%d==0){*v++=d; n=n/d; if(n<d)goto endfac;}  // remove all multiples of d
 }
endfac:;  // come here when q gets too small or we run out of primes
 *v=n;  // the last divide might have left us with a prime bigger than d but small enough to exit
 z=vec(INT,(v-factors)+BETWEENC(n,2,MAXIFACTOR),factors);  // create a suitable result block, including the ending n if it is not too big
 // If the factoring was incomplete, the unfactored part is bigger than the reach of our prime list and we will have to treat it as extended
 if(unlikely(n>MAXIFACTOR))z=over(z,xfactor(cvt(XNUM,sc(n))));  // append factors bigger than our prime list
#else
 UI d,q; DO(AN(JT(jt,p4792)), d=u[i]; q=n/d; while(n==q*d){*v++=d; n=q; q/=d;} if(q<d)break;);
 *v=n;  // the last divide might have left us with a prime bigger than d but small enough to exit
 z=vec(INT,(v-factors)+(n>1),factors);  // create a suitable result block
#endif
 EPILOG(z);
}    /* q:"r w */


/*
 https://webbox.lafayette.edu/~reiterc/j/vector/factor_ecj.html
 Elliptic curve arithmetic and factorization.
 factor_ecj.ijs
 Cliff Reiter
 June 2003

 Elliptic curves are E=.a,b where (y^2)=(x^3)+(a*x)+b
 Moduli are n where n-:0 corresponds to rational arithmetic
*/

static B jtsmallprimes(J jt,I n,X y,A*zs,X*zx){mpX(y);
 ASSERT(n<=1229&&n<=AN(JT(jt,p4792)),EVLIMIT);
 I m=0,*pv=AV(JT(jt,p4792));B pq=0; // m: count of small primes
 { // calculate m (and pq which is set if prime list would include a sieved prime)
  mpX0(r);mpX0(q);mpX0(x);jmpz_set(mpx,mpy);GEMP0;
  for(I j=0;j<n;j++){
   I d=pv[j];jmpz_fdiv_qr_ui(mpq,mpr,mpx,d);GEMP0;
   while(!mpr->_mp_size){m++;jmpz_set(mpx,mpq);GEMP0;jmpz_fdiv_qr_ui(mpq,mpr,mpx,d);GEMP0;}
   if(0>jmpz_cmpabs_ui(mpq,d)){
    if(1>jmpz_cmpabs_ui(mpx,99460729L)&&!(1==mpx->_mp_size&&1==*mpx->_mp_d))m+=pq=1;
    break;
   }
  }
  jmpz_clear(mpx);jmpz_clear(mpq);jmpz_clear(mpr);
 }
 A s;GATVR(s,INT,m,1,&m);I*sv,*v=sv=AV1(s);mpX0(r);mpX0(q);mpX0(x);jmpz_set(mpx,mpy);GEMP0;
 for(I j=0;j<n;j++){
  I d=pv[j];jmpz_fdiv_qr_ui(mpq,mpr,mpx,d);GEMP0;
  while(!mpr->_mp_size){*v++=pv[j];jmpz_set(mpx,mpq);GEMP0;jmpz_fdiv_qr_ui(mpq,mpr,mpx,d);GEMP0;}
  if(0>jmpz_cmpabs_ui(mpq,d))break;
 }
 jmpz_clear(mpq);jmpz_clear(mpr);
 X x;if(pq){*v++=*mpx->_mp_d;jmpz_clear(mpx);x=X1;}else x=Xmp(x);
 RZ(*zs=cvt(XNUM,s)); *zx=x;
 R 1;
} /* remove small prime factors */

/* if 0=n xprimeq y, then y is certainly composite; and                    */
/* if 1=n xprimeq y, then y is prime with a probability of error of 0.25^n */

static B jtxprimeq(J jt,I n,X y){mpX(y); 
 B r= jmpz_probab_prime_p(mpy,n); GEMP0;
 if (unlikely(1==r)) {r= jmpz_probab_prime_p(mpy,n); GEMP0;}
 if (unlikely(1==r)) {r= jmpz_probab_prime_p(mpy,n); GEMP0;}
 if (unlikely(1==r)) {r= jmpz_probab_prime_p(mpy,n); GEMP0;}
 R !!r;
}

static XF1(jtpollard_p_1){A om=jt->xmod;X z=X1;
 I n=MIN(1229,AN(JT(jt,p4792))),*pv=AV(JT(jt,p4792)); D m=log((D)pv[n-1]);
 X c;RZ(c=xc(2L));
 RZ(jt->xmod=scx(w));
 A *old=jt->tnextpushp;
 for(I i=0;i<n;++i){
  D p=(D)pv[i]; I e=(I)pow(p,jfloor(m/log(p)));
  RZ(c=xpow(c,XgetI(e)));
  X g;RZ(g=xgcd(w,xminus(c,X1)));
  if(!equ(g,X1)&&!equ(g,w)){z=g; break;}
  c=(X)gc((A)c,old);
 }
 jt->xmod=om; 
 R z;
}

static XF1(jtpollard_rho){
 I n=10000;
 X y1,y2;RZ(y1=y2=X2);
 A *old=jt->tnextpushp;
 for(I i=0;i<n;++i){
  RZ(y1=xrem(w,xplus(X1,xsq(y1))));
  RZ(y2=xrem(w,xplus(X1,xsq(xplus(X1,xsq(y2))))));
  X g;RZ(g=xgcd(w,xrem(w,xminus(y2,y1))));
  if(!ISX1(g)&&!equ(g,w))R g;
  if(!gc3(&y1,&y2,0L,old))R0;
 }
 R X1;
}

static B jtranec(J jt,X w,X*zg,X*za,X*zb,X*zx,X*zy){A mm,t;I*tv;X a,aa,b,bb,g,x,y;
 g=w; RZ(mm=reshape(sc(3L),sc(IMAX)));
 while(!xcompare(g,w)){
  RZ(t=roll(mm)); tv=AV(t); 
  RZ(x=xc(tv[0])); 
  RZ(y=xc(tv[1])); 
  RZ(a=xc(tv[2]));
  RZ(b=xrem(w,xminus(xsq(y),xtymes(x,xplus(a,xsq(x))))));
  RZ(aa=xtymes(xc( 4L),xtymes(a,xsq(a))));
  RZ(bb=xtymes(xc(27L),         xsq(b) ));
  RZ(g=xgcd(w,xplus(aa,bb)));
 }
 *zg=g; *za=a; *zb=b; *zx=x; *zy=y;
 R 1;
}    /* random elliptic curve */

static A jtdb1b2(J jt,I n,X w){A t,z;D c,d,lg,n1=(D)n-1,p,r;I m,s[3],*v,*zv;
 s[0]=n; s[1]=2; GATVR(z,INT,2*n,2,s); zv=v=AV2(z);
 RZ(t=ccvt(FL,scx(w),0)); d=DAV(t)[0]; 
 lg=log(d); c=log(sqrt(d)); r=exp(sqrt(0.5)+sqrt(c*log(c)))/lg;
 DO(n, c=lg*pow(r,i/n1); p=c*log(c); if(p>=2147483647)break; *v++=(I)jfloor(c); *v++=(I)p;);
 m=(v-zv)>>1; ASSERT(m!=0,EVLIMIT);
 AS(z)[0]=m; AN(z)=2*m;
 R z;
}

static B jtecd(J jt,X n,X a,X b,X*q,X*z){X m,s,x2,y2,yy,z2;
 A *old=jt->tnextpushp;
 if(0==xcompare(q[1],X0)||0==xcompare(q[2],X0)){z[0]=X0; z[1]=X1; z[2]=X0;}
 else{
  RZ(m=xplus(xtymes(xc(3L),xsq(q[0])),xtymes(a,xsq(xsq(q[2])))));
  RZ(yy=xsq(q[1]));
  RZ(s=xtymes(xc(4L),xtymes(q[0],yy)));
  RZ(x2=xplus(xsq(m),xtymes(xc(-2L),s)));
  RZ(y2=xplus(xtymes(m,xminus(s,x2)),xtymes(xc(-8L),xsq(yy))));
  RZ(z2=xtymes(xc(2L),xtymes(q[1],q[2])));
  RZ(z[0]=xrem(n,x2)); RZ(z[1]=xrem(n,y2)); RZ(z[2]=xrem(n,z2));
 }
 if(!gc3(z,z+1,z+2,old))R0;
 R 1;
}    /* elliptic curve double point (mod proj coord) */

static B jteca(J jt,X n,X a,X b,X*p,X*q,X*z){
 A *old=jt->tnextpushp;
 if     (0==xcompare(p[2],X0)){z[0]=q[0]; z[1]=q[1]; z[2]=q[2];}
 else if(0==xcompare(q[2],X0)){z[0]=p[0]; z[1]=p[1]; z[2]=p[2];}
 else{X m,r,s1,s2,t,t1,t2,u1,u2,w,w2,x3,y3,z12,z22,z3;
  RZ(u1=xtymes(q[0],z12=xsq(p[2]))); RZ(s1=xtymes(q[1],xtymes(p[2],z12)));
  RZ(u2=xtymes(p[0],z22=xsq(q[2]))); RZ(s2=xtymes(p[1],xtymes(q[2],z22)));
  RZ(w=xminus(u1,u2));
  if(0==xcompare(w,X0))RZ(ecd(n,a,b,p,z))
  else{
   RZ(r=xminus(s1,s2));
   RZ(t=xplus(u1,u2));
   RZ(m=xplus(s1,s2));
   RZ(w2=xsq(w));
   RZ(x3=xminus(xsq(r),xtymes(t,w2)));
   RZ(t1=xtymes(r,xplus(xtymes(xc(-2L),x3),xtymes(t,w2))));
   RZ(t2=xtymes(m,xtymes(w,w2)));
   RZ(y3=xdiv(xminus(t1,t2),xc(2L),XMFLR));
   RZ(z3=xtymes(p[2],xtymes(q[2],w)));
   RZ(z[0]=xrem(n,x3)); RZ(z[1]=xrem(n,y3)); RZ(z[2]=xrem(n,z3));
 }}
 if(!gc3(z,z+1,z+2,old))R0;
 R 1;
}    /* elliptic curve add (mod proj coord) */

#define BIT0 ((I)1<<(BW-1))

static B jtecm(J jt,X n,X a,X b,I m,X*p,X*z){
 A *old=jt->tnextpushp;
 if(0==m){z[0]=X0; z[1]=X1; z[2]=X0;}
 else{I k;UI c,d;X pm[3],q[3];
  q[0]=p[0]; q[1]=p[1]; q[2]=p[2]; 
  pm[0]=p[0]; RZ(pm[1]=xminus(X0,p[1])); pm[2]=p[2]; 
  k=CTLZI(3*m); c=((3*m)&~1)<<(BW-k); d=(m&~1)<<(BW-k); --k; // k=bit# of MSB+1
  DQ(k, RZ(ecd(n,a,b,q,q)); if(BIT0&(c^d))RZ(eca(n,a,b,q,c&BIT0?p:pm,q)); c<<=1; d<<=1;);
  z[0]=q[0]; z[1]=q[1]; z[2]=q[2];
 }
 if(!gc3(z,z+1,z+2,old))R0;
 R 1;
}    /* scalar mult ladder (mod proj coord) */

static B jtecm_s1(J jt,X n,X a,X b,I b1,X*q,X*z){A tt;D d,lg;I dd,m,*pv;X x[3];
 A *old=jt->tnextpushp;
 lg=log((D)b1); RE(m=i0(plt(sc(b1))));
 if(m<=AN(JT(jt,p4792)))pv=AV(JT(jt,p4792)); else{RZ(tt=prime1(IX(m))); pv=AV(tt);}
 x[0]=q[0]; x[1]=q[1]; x[2]=q[2];
 DQ(m, d=(D)*pv++; dd=(I)pow(d,jfloor(5e-14+lg/log(d))); RZ(ecm(n,a,b,dd,x,x)););
 z[0]=x[0]; z[1]=x[1]; z[2]=x[2];
 if(!gc3(z,z+1,z+2,old))R0;
 R 1;
}

static B jtecm_s2(J jt,X n,X a,X b,I b1,I b2,X*q,X*z){A sda,tt;I d,di,i,k,m,p0,*pd,*v;X*s1,*sd,*sd0,*sdd,*t,x[3];
 RZ(tt=plt(v2(b1,b2))); v=AV(tt); m=(v[1]-v[0])-1;
 RZ(tt=prime1(apv(1+m,v[0],1L))); pd=v=AV(tt); p0=*v;
 d=0; DQ(m, v[0]=k=-1+((v[1]-v[0])>>1); ++v; d=MAX(d,k);); ++d; d=MIN(100,d);
 GATV0(sda,XNUM,3*d,2); sd0=sd=XAV2(sda); v=AS(sda); v[0]=d; v[1]=3;
 RZ(ecd(n,a,b,q,sd)); s1=t=sd; sd+=3;
 DQ(d-1, eca(n,a,b,s1,t,sd); t=sd; sd+=3;); sd=sd0; sdd=t;
 RZ(ecm(n,a,b,p0,q,x));
 A *old=jt->tnextpushp;
 for(i=0;i<m;++i){
  di=pd[i];
  DQ(di/d, RZ(eca(n,a,b,x,sdd,x)););
  RZ(eca(n,a,b,x,sd+3*(di%d),x));
 if(!gc3(x,x+1,x+2,old))R0;
 }
 z[0]=x[0]; z[1]=x[1]; z[2]=x[2];
 R 1;
}

static XF1(jtfac_ecm){A tt;I b1,b2,*b1b2,i,m;X a,b,g,q[3];
 RZ(tt=db1b2(20L,w)); SETIC(tt,m); b1b2=AV(tt);
 A *old=jt->tnextpushp;
 for(i=0;i<m;++i){
  b1=b1b2[0]; b2=b1b2[1]; b1b2+=2;
  ranec(w,&g,&a,&b,q,q+1); q[2]=X1;
  if(!ISX1(g)&&xcompare(g,w))R g;
  RZ(ecm_s1(w,a,b,b1,q,q));
  RZ(g=xgcd(w,q[2]));
  if(!ISX1(g)&&xcompare(g,w))R g;
  if(ISX1(g)){
   RZ(ecm_s2(w,a,b,b1,b2,q,q));
   RZ(g=xgcd(w,q[2]));
   if(!ISX1(g)&&xcompare(g,w))R g;
  }
  tpop(old);
 }
 R X1;
}

// q: on extended numbers
static F1(jtxfactor){PROLOG(0064);
 F1RANK(0,jtxfactor,DUMMYSELF);
 if(!(XNUM&AT(w)))RZ(w=cvt(XNUM,w));
 X x=XAV(w)[0];
 // ASSERT(m!=XPINF&&m!=XNINF&&0<m,EVDOMAIN);
 if(1>xcompare(x,xc(MAXIFACTOR)))  // if factorable as I, do that
	 R xco1(factor(sc(xint(x))));
 A z;RZ(smallprimes(1229L,x,&z,&x));
 A st;GAT0(st,XNUM,20,1); X*sv0,*sv=sv0=XAV1(st); *sv++=x;
 B b=0;while(sv-sv0){
  x=*--sv; 
  I k;if(2>(k=sv-sv0)){A stsav = st; GATV0(st,XNUM,2*AN(stsav),1); MC(XAV1(st),sv0,k*sizeof(A)); sv0=XAV1(st); sv=k+sv0;}
  if(1>xcompare(x,xc(2147483647L))){RZ(z=apip(z,factor(sc(xint(x))))); continue;}
  if(xprimeq(100L,x)){RZ(z=apip(z,scx(x))); continue;}
  X g;RZ(g=pollard_p_1(x)); if(!ISX1(g)){*sv++=g; RZ(*sv++=xdiv(x,g,XMFLR)); continue;}
  RZ(g=pollard_rho(x)); if(!ISX1(g)){*sv++=g; RZ(*sv++=xdiv(x,g,XMFLR)); continue;}
  DO(jt->ecmtries, RZ(g=fac_ecm(x)); if(!ISX1(g)){*sv++=g; RZ(*sv++=xdiv(x,g,XMFLR)); goto found;})
  ASSERT(0,EVNONCE);
found:;
 }
 z=grade2(z,z);
 EPILOG(z);
}

/* ---------------------------------------------------- */

static F1(test_ecm){A*wv,z;X*ab,n,*zv;
 ARGCHK1(w);
 RZ(init4792(jt));
 ASSERT(4==AN(w),EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 wv=AAV(w); A wv0,wv1,wv2,wv3; wv0=C(wv[0]); wv1=C(wv[1]); wv2=C(wv[2]); wv3=C(wv[3]); 
 ASSERT(XNUM&AT(wv0),EVDOMAIN); ASSERT(1==AR(wv0),EVRANK); ASSERT(2==AN(wv0),EVLENGTH);
 ASSERT(XNUM&AT(wv1),EVDOMAIN); ASSERT(0==AR(wv1),EVRANK);
 ASSERT(INT&AT(wv2),EVDOMAIN);
 ASSERT(XNUM&AT(wv3),EVDOMAIN);
 n=XAV(wv1)[0];
 ab=XAV(wv0);
 GAT0(z,XNUM,3,1); zv=XAV1(z);
 RZ(ecm(n,ab[0],ab[1],i0(wv2),XAV(wv3),zv));
 RETF(z);
}

static F1(test_ecm_s1){A*wv,z;X*ab,n,*zv;
 ARGCHK1(w);
 RZ(init4792(jt));
 ASSERT(4==AN(w),EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 wv=AAV(w); A wv0,wv1,wv2,wv3; wv0=C(wv[0]); wv1=C(wv[1]); wv2=C(wv[2]); wv3=C(wv[3]); 
 ASSERT(XNUM&AT(wv0),EVDOMAIN); ASSERT(1==AR(wv0),EVRANK); ASSERT(2==AN(wv0),EVLENGTH);
 ASSERT(XNUM&AT(wv1),EVDOMAIN); ASSERT(0==AR(wv1),EVRANK);
 ASSERT(INT&AT(wv2),EVDOMAIN);
 ASSERT(XNUM&AT(wv3),EVDOMAIN);
 n=XAV(wv1)[0];
 ab=XAV(wv0);
 GAT0(z,XNUM,3,1); zv=XAV1(z);
 RZ(ecm_s1(n,ab[0],ab[1],i0(wv2),XAV(wv3),zv));
 RETF(z);
}

static F1(test_ecm_s2){A*wv,z;I*b1b2;X*ab,n,*zv;
 ARGCHK1(w);
 RZ(init4792(jt));
 ASSERT(4==AN(w),EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 wv=AAV(w); A wv0,wv1,wv2,wv3; wv0=C(wv[0]); wv1=C(wv[1]); wv2=C(wv[2]); wv3=C(wv[3]); 
 ASSERT(XNUM&AT(wv0),EVDOMAIN); ASSERT(1==AR(wv0),EVRANK); ASSERT(2==AN(wv0),EVLENGTH);
 ASSERT(XNUM&AT(wv1),EVDOMAIN); ASSERT(0==AR(wv1),EVRANK);
 ASSERT(INT &AT(wv2),EVDOMAIN); ASSERT(1==AR(wv2),EVRANK); ASSERT(2==AN(wv0),EVLENGTH);
 ASSERT(XNUM&AT(wv3),EVDOMAIN);
 n=XAV(wv1)[0];
 ab=XAV(wv0);
 b1b2=AV(wv2);
 GAT0(z,XNUM,3,1); zv=XAV1(z);
 RZ(ecm_s2(n,ab[0],ab[1],b1b2[0],b1b2[1],XAV(wv3),zv));
 RETF(z);
}

static F1(test_fac_ecm){
 ARGCHK1(w);
 RZ(init4792(jt));
 ASSERT(!AR(w),EVRANK);
 ASSERT(XNUM&AT(w),EVDOMAIN);
 R scx(fac_ecm(XAV(w)[0]));
}
