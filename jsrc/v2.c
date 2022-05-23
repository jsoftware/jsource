/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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
 GATV(z,INT,n,AR(w),AS(w)); zv= AV(z);
 RZ(d=grade1(ravel(w)));  dv= AV(d);
 if(JT(jt,p4792)){I*u=AV(JT(jt,p4792)); c=AN(JT(jt,p4792)); while(n>k&&c>(x=wv[dv[k]]))zv[dv[k++]]=u[x];}
 else{
  while(n>k&&0==wv[dv[k]])zv[dv[k++]]=2;
  while(n>k&&1==wv[dv[k]])zv[dv[k++]]=3;
  while(n>k&&2==wv[dv[k]])zv[dv[k++]]=5;
 }
 if(n==k){RETF(z);} 
 j=3; p=0; e=PT; q=1+(I)sqrt((D)m); x=wv[dv[k]]; 
 GATV0(t,B01,q,1);         u=BAV(t); sieve(0L,q,u,u);
 #undef DF1
 GATV0(y,B01,MIN(m,MM),1); b=BAV(y); 
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

static I init4792(J jt) {
 if(!JT(jt,p4792)){RZ(JT(jt,p4792)=prime1(IX(4792L))); ACX(JT(jt,p4792));} R 1;
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
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z);
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
 if(b)RZ(z=prime1d(ISDENSETYPE(AT(w),FL)?w:cvt(FL,w)));
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
 GATV0(t,B01,q,1);         u =BAV(t); sieve(0L,q,u,u);
 GATV0(y,B01,MIN(m,MM),1); b =BAV(y); 
 GATV(z,INT,n,AR(w),AS(w)); zv= AV(z);
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
 GATV(z,B01,wn,AR(w),AS(w)); b=BAV(z);
 for(j=0;j<wn;++j){
  n=*wv++; v=pv;
  if(32>n)b[j]=pmsk[MAX(0,n)];
  else{b[j]=1; DQ(AN(JT(jt,p4792)), d=*v++; q=n/d; if(n==q*d){b[j]=0; break;}else if(q<d)break;);}
 }
 RETF(z);
}

static F1(jtxprimetest){A z;B*b,rat;I d,j,q,n,*pv,*v,wn,wt,*yv;X r,*wv,x,xmaxint,y;
 ARGCHK1(w);
 wn=AN(w); wt=AT(w); wv=XAV(w); pv=AV(JT(jt,p4792)); 
 rat=1&&wt&RAT; RZ(xmaxint=xc(2147483647L)); RZ(y=xc(-1L)); yv=AV(y);
 GATV(z,B01,wn,AR(w),AS(w)); b=BAV(z);
 for(j=0;j<wn;++j){
  x=*wv++; d=AV(x)[AN(x)-1]; b[j]=1; v=pv;
  if(rat&&xcompare(iv1,*wv++)){b[j]=0; continue;}
  ASSERT(d!=XPINF&&d!=XNINF,EVDOMAIN);
  if(0>=d)b[j]=0;
  else if(1==xcompare(x,xmaxint)){
   A *old=jt->tnextpushp;
   DQ(100, *yv=*v++; RZ(r=xrem(y,x)); if(!AV(r)[0]){b[j]=0; break;});
   if(b[j])RE(b[j]=xprimeq(100L,x));
   tpop(old);
  }else{
   n=xint(x); v=pv;
   if(32>n)b[j]=pmsk[MAX(0,n)];
   else DQ(AN(JT(jt,p4792)), d=*v++; q=n/d; if(n==q*d){b[j]=0; break;}else if(q<d)break;);
 }}
 RETF(z);
}    /* prime test for extended integers or rationals */

static F1(jtprimetest){A x;I t;
 ARGCHK1(w);
 t=AT(w);
 if((UI)SGNIF(t,B01X)>=(UI)AN(w))R reshape(shape(w),num(0));  // AN is 0, or t is boolean
 switch(CTTZ(t)){
  case INTX:            R iprimetest(w);
  default:             ASSERT(0,EVDOMAIN);
  case RATX: case XNUMX: R xprimetest(w);
  case FLX:  case CMPXX:
   {PUSHCCT(1.0-FUZZ) x=eq(t&FL?w:conjug(w),floor1(w)); POPCCT}
   R xprimetest(cvt(XNUM,tymes(w,x))); 
}}   /* primality test */


static F1(jtnextprime){A b,fs,x,y;B*bv;I k,n,*xv,*yv;X*wv;
 ARGCHK1(w);
 n=AN(w);
 if((UI)SGNIF(AT(w),B01X)>=(UI)AN(w))R reshape(shape(w),num(2));
 ASSERT(NUMERIC&AT(w),EVDOMAIN);
 RZ(fs=eval("2&+^:(0&p:)^:_"));
 GATV(x,INT,n,AR(w),AS(w)); xv=AV(x);
 if(INT&AT(w)){B b=1;I*wv=AV(w);
  DQ(n, k=*wv++; if(k==IMAX){b=0; break;}else *xv++=2>k?2:(k+1)|1;);
  if(b)R rank1ex0(x,fs,FAV(fs)->valencefns[0]);
  RZ(w=cvt(XNUM,w));
 }
 if(ISDENSETYPE(AT(w),FL+RAT))RZ(w=cvt(XNUM,floor1(w)));
 if(ISDENSETYPE(AT(w),CMPX))RZ(w=cvt(XNUM,floor1(cvt(FL,w))));
 GATV(b,B01,n,AR(w),AS(w)); bv=BAV(b); wv=XAV(w);
 DQ(n, y=*wv++; yv=AV(y); *bv++=0<yv[AN(y)-1]; k=*yv; *xv++=AN(y)==1&&2>k?2-k:(k&1)+1; );
 R rank1ex0(tymes(b,plus(w,x)),fs,FAV(fs)->valencefns[0]);
}

static F1(jtprevprime){A fs,x,y;I k,m,n,*xv,*yv;X*wv;
 ARGCHK1(w);
 n=AN(w);
 ASSERT(!n||NUMERIC&AT(w)&&!(B01&AT(w)),EVDOMAIN);
 RZ(fs=eval("_2&+^:(0&p:)^:_"));
 GATV(x,INT,n,AR(w),AS(w)); xv=AV(x);
 if(INT&AT(w)){I*wv=AV(w);
  DQ(n, k=*wv++; ASSERT(2<k,EVDOMAIN); *xv++=3==k?2:(k-2)|1;);
  R rank1ex0(x,fs,FAV(fs)->valencefns[0]);
 }
 if(ISDENSETYPE(AT(w),FL+RAT))RZ(w=cvt(XNUM,ceil1(w)));
 if(ISDENSETYPE(AT(w),CMPX))RZ(w=cvt(XNUM,ceil1(cvt(FL,w))));
 wv=XAV(w);
 DQ(n, y=*wv++; yv=AV(y); m=AN(y); k=*yv; ASSERT(0<yv[m-1]&&(1<m||2<k),EVDOMAIN); *xv++=1==m&&3==k?1:1+(k&1););
 R rank1ex0(minus(w,x),fs,FAV(fs)->valencefns[0]);
}

static F1(jttotient){A b,x,z;B*bv,p=0;I k,n,t;
 ARGCHK1(w);
 n=AN(w); t=AT(w);
 if(t&B01)RCA(w);
 GATV(b,B01,n,AR(w),AS(w)); bv=BAV(b);
 if(t&INT){I*wv=AV(w),*xv;
  GATV(x,INT,n,AR(w),AS(w)); xv=AV(x);
  DQ(n, k=*wv++; ASSERT(0<=k,EVDOMAIN); if(k){*bv++=1; *xv++=k;}else{*bv++=0; *xv++=1; p=1;};);
 }else{X*xv,y;
  RZ(x=cvt(XNUM,w)); xv=XAV(x);
  DO(n, y=xv[i]; k=AV(y)[AN(y)-1]; ASSERT(0<=k,EVDOMAIN); if(k)*bv++=1; else{*bv++=0; xv[i]=iv1; p=1;});
 }
 A z0; z=cvt(AT(x),df1(z0,x,eval("(- ~:)&.q:"))); 
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
 GATV(z,B01,wn,AR(w),AS(w)); zv=BAV(z);
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
}}   /* a p: w */

static A jtqco2x(J jt,I m,A w){A y;I c,*dv,i,*pv,*yv;X d,q,r,x;
 RZ(init4792(jt));
 if(!(XNUM&AT(w)))RZ(w=cvt(XNUM,w));
 x=XAV(w)[0]; pv=AV(JT(jt,p4792)); RZ(d=xc(2L)); dv=AV(d);
 GATV0(y,INT,m,1); yv=AV(y); mvc(m*SZI,yv,1,MEMSET00);
 for(i=0;i<m;++i){
  c=0; *dv=pv[i];
  while(1){RZ(xdivrem(x,d,&q,&r)); if(AV(r)[0])break; ++c; x=q;}
  yv[i]=c; if(1==AN(x)&&1==AV(x)[0])break;
 }
 R cvt(XNUM,y);
}    /* m q: w where 0<:m and p: m is one xdigit and w is a single extended integer */

F2(jtqco2){A q,y,z;B b,bb,xt;I c,j,k,m,*qv,wn,wr,*yv,*zv;
 ARGCHK2(a,w);
 wn=AN(w); wr=AR(w); b=all1(lt(a,zeroionei(0))); xt=1&&AT(w)&XNUM+RAT;
 if(AR(a)||wr&&(b||xt))R rank2ex0(a,w,DUMMYSELF,jtqco2);
 if(!b&&xt){RE(m=i0(vib(a))); if(BETWEENO(m,0,1229))R qco2x(m,w);}  /* 1229=p:^:_1 XBASE */
 RZ(q=factor(w)); qv=AV(q);
 if(b)RZ(a=negate(a));
 bb=equ(a,ainf);
 if(b&bb){ /* __ q: w */
  RZ(y=ne(q,curtail(over(zeroionei(0),q))));
  R lamin2(repeat(y,q),df1(z,y,cut(ds(CPOUND),zeroionei(1))));
 }
 RZ(y=vi(plt(q))); yv=AV(y);
 k=-1; DO(AN(y), if(k<yv[i])k=yv[i];); ++k;
 if(bb)m=k; else RE(m=i0(a));
 if(b){
  q=repeat(ge(y,sc(k-m)),q);
  R lamin2(nub(q),df2(z,q,q,sldot(ds(CPOUND))));
 }else{
  GATV0(z,INT,wn*m,1+wr); MCISH(AS(z),AS(w),wr) AS(z)[wr]=m; zv=AV(z);  // avoid overfetch of AS(w)
  mvc(AN(z)*SZI,zv,1,MEMSET00);
  j=0; c=AS(q)[wr]; DQ(wn, DQ(c, if(qv[j]&&m>yv[j])++zv[yv[j]]; ++j;); zv+=m;);
  RETF(AT(w)&XNUM+RAT?cvt(XNUM,z):z);
}}   /* a q: w for array w */

static F1(jtxfactor);

F1(jtfactor){PROLOG(0063);A y,z;I c,d,i,k,m,n,q,*u,*v,wn,*wv,*zv;
 ARGCHK1(w);
 RZ(init4792(jt));
 if(AT(w)&XNUM+RAT)R xfactor(w);
 if(ISDENSETYPE(AT(w),FL+CMPX)){
  RZ(y=pcvt(INT,w)); 
  if(INT&AT(y))w=y; 
  else{RZ(y=pcvt(XNUM,xco1(w))); ASSERT(XNUM&AT(y),EVDOMAIN); R pcvt(INT,xfactor(y));}
 }
 RZ(w=vi(w));
 wn=AN(w); wv=AV(w);
 n=0; DO(wn, k=wv[i]; ASSERT(0<k,EVDOMAIN); n=MAX(n,k););
#if SY_64
 if(n>2147483647)R cvt(INT,xfactor(w));
#endif
 u=AV(JT(jt,p4792)); c=8*SZI-2;
 GATV0(z,INT,c*wn,1+AR(w)); MCISH(AS(z),AS(w),AR(w)) AS(z)[AR(w)]=c; v=zv=AV(z);
 for(i=m=0;i<wn;++i){
  n=*wv++;
  DO(AN(JT(jt,p4792)), d=u[i]; q=n/d; while(n==q*d){*v++=d; n=q; q/=d;} if(q<d)break;);
  if(1<n)*v++=n;
  d=v-zv; m=MAX(m,d); zv+=c; while(v<zv)*v++=0; 
 }
 z=c==m?z:taker(m,z);
 EPILOG(z);
}    /* q:"r w */


/*
 http://ww2.lafayette.edu/~reiterc/j/vector/factor_ecj.html
 Elliptic curve arithmetic and factorization.
 factor_ecj.ijs
 Cliff Reiter
 June 2003

 Elliptic curves are E=.a,b where (y^2)=(x^3)+(a*x)+b
 Moduli are n where n-:0 corresponds to rational arithmetic
*/

static B jtsmallprimes(J jt,I n,X x,A*zs,X*zx){A s;I i,m,*pv,*sv,*v;X d,q,r;
 ASSERT(n<=1229&&n<=AN(JT(jt,p4792)),EVLIMIT);
 pv=AV(JT(jt,p4792)); m=(I)(3.322*XBASEN*AN(x)); 
 GATV0(s,INT,m,1); v=sv=AV(s);
 A *old=jt->tnextpushp;
 for(i=0;i<n;++i){
  RZ(d=xc(pv[i])); 
  RZ(xdivrem(x,d,&q,&r));   /* d must have only one "digit" */
  while(!xcompare(r,iv0)){*v++=pv[i]; x=q; RZ(xdivrem(q,d,&q,&r));} 
  if(-1==xcompare(q,d))break;
  x=gc(x,old);
 }
 if(1>xcompare(x,xc(99460729L))&&!(1==AN(x)&&1==XDIG(x))){*v++=xint(x); x=iv1;}
 AN(s)=AS(s)[0]=v-sv;
 RZ(*zs=cvt(XNUM,s)); *zx=x;
 R 1;
}    /* remove small prime factors */

/* if 0=n xprimeq y, then y is certainly composite; and                    */
/* if 1=n xprimeq y, then y is prime with a probability of error of 0.25^n */

static B jtxprimeq(J jt,I n,X y){A h,om=jt->xmod;B b;I*dv,i,k,*pv;X d,m,t,x,y1;
 ASSERT(n<=AN(JT(jt,p4792)),EVLIMIT);
 pv=AV(JT(jt,p4792));
 GAT0(h,XNUM,1,0); XAV(h)[0]=y; jt->xmod=h; 
 k=0; RZ(t=xc(2L)); RZ(m=y1=xminus(y,iv1)); 
 while(0==(AV(m)[0]&1)){++k; RZ(m=xdiv(m,t,XMFLR));}
 GAT0(d,INT,1,1); dv=AV(d);  // could use faux block
 A *old=jt->tnextpushp;
 for(i=0;i<n;++i){
  dv[0]=pv[i]; RZ(x=xpow(d,m)); b=1==AN(x)&&1==AV(x)[0];
  DQ(k*!b, if(!xcompare(x,y1)){b=1; break;} RZ(x=xrem(y,xsq(x))););
  tpop(old);
  if(!b)break;
 }
 jt->xmod=om; R b;
}    /* y assumed to be not in n{.JT(jt,p4792) */

static XF1(jtpollard_p_1){A om=jt->xmod;D p,m;I e,i,n,*pv;X c,g,z=iv1;
 n=MIN(1229,AN(JT(jt,p4792))); pv=AV(JT(jt,p4792)); m=log((D)pv[n-1]);
 RZ(c=xc(2L));
 RZ(jt->xmod=scx(w));
 A *old=jt->tnextpushp;
 for(i=0;i<n;++i){
  p=(D)pv[i]; e=(I)pow(p,jfloor(m/log(p)));
  RZ(c=xpow(c,sc(e)));
  RZ(g=xgcd(w,xminus(c,iv1)));
  if(!equ(g,iv1)&&!equ(g,w)){z=g; break;}
  c=(X)gc((A)c,old);
 }
 jt->xmod=om; 
 R z;
}

static XF1(jtpollard_rho){I i,n;X g,y1,y2;
 n=10000;
 RZ(y1=y2=xc(2L));
 A *old=jt->tnextpushp;
 for(i=0;i<n;++i){
  RZ(y1=xrem(w,xplus(iv1,xsq(y1))));
  RZ(y2=xrem(w,xplus(iv1,xsq(xplus(iv1,xsq(y2))))));
  RZ(g=xgcd(w,xrem(w,xminus(y2,y1))));
  if(!equ(g,iv1)&&!equ(g,w))R g;
  if(!gc3(&y1,&y2,0L,old))R0;
 }
 R iv1;
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
 s[0]=n; s[1]=2; GATVR(z,INT,2*n,2,s); zv=v=AV(z);
 RZ(t=cvt(FL,scx(w))); d=DAV(t)[0]; 
 lg=log(d); c=log(sqrt(d)); r=exp(sqrt(0.5)+sqrt(c*log(c)))/lg;
 DO(n, c=lg*pow(r,i/n1); p=c*log(c); if(p>=2147483647)break; *v++=(I)jfloor(c); *v++=(I)p;);
 m=(v-zv)>>1; ASSERT(m!=0,EVLIMIT);
 AS(z)[0]=m; AN(z)=2*m;
 R z;
}

static B jtecd(J jt,X n,X a,X b,X*q,X*z){X m,s,x2,y2,yy,z2;
 A *old=jt->tnextpushp;
 if(0==xcompare(q[1],iv0)||0==xcompare(q[2],iv0)){z[0]=iv0; z[1]=iv1; z[2]=iv0;}
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
 if     (0==xcompare(p[2],iv0)){z[0]=q[0]; z[1]=q[1]; z[2]=q[2];}
 else if(0==xcompare(q[2],iv0)){z[0]=p[0]; z[1]=p[1]; z[2]=p[2];}
 else{X m,r,s1,s2,t,t1,t2,u1,u2,w,w2,x3,y3,z12,z22,z3;
  RZ(u1=xtymes(q[0],z12=xsq(p[2]))); RZ(s1=xtymes(q[1],xtymes(p[2],z12)));
  RZ(u2=xtymes(p[0],z22=xsq(q[2]))); RZ(s2=xtymes(p[1],xtymes(q[2],z22)));
  RZ(w=xminus(u1,u2));
  if(0==xcompare(w,iv0))RZ(ecd(n,a,b,p,z))
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

#if SY_64
#define BIT0 0x8000000000000000
#else
#define BIT0 0x80000000
#endif

static B jtecm(J jt,X n,X a,X b,I m,X*p,X*z){
 A *old=jt->tnextpushp;
 if(0==m){z[0]=iv0; z[1]=iv1; z[2]=iv0;}
 else{I k;UI c,d;X pm[3],q[3];
  q[0]=p[0]; q[1]=p[1]; q[2]=p[2]; 
  pm[0]=p[0]; RZ(pm[1]=xminus(iv0,p[1])); pm[2]=p[2]; 
  CTLZI(3*m,k); c=((3*m)&~1)<<(BW-k); d=(m&~1)<<(BW-k); --k; // k=bit# of MSB+1
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
 GATV0(sda,XNUM,3*d,2); sd0=sd=XAV(sda); v=AS(sda); v[0]=d; v[1]=3;
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
  ranec(w,&g,&a,&b,q,q+1); q[2]=iv1;
  if(xcompare(g,iv1)&&xcompare(g,w))R g;
  RZ(ecm_s1(w,a,b,b1,q,q));
  RZ(g=xgcd(w,q[2]));
  if(xcompare(g,iv1)&&xcompare(g,w))R g;
  if(0==xcompare(g,iv1)){
   RZ(ecm_s2(w,a,b,b1,b2,q,q));
   RZ(g=xgcd(w,q[2]));
   if(xcompare(g,iv1)&&xcompare(g,w))R g;
  }
  tpop(old);
 }
 R iv1;
}

static F1(jtxfactor){PROLOG(0064);A st,z;B b=0;I k,m;X g,*sv,*sv0,x;
 F1RANK(0,jtxfactor,DUMMYSELF);
 if(!(XNUM&AT(w)))RZ(w=cvt(XNUM,w));
 x=XAV(w)[0]; m=XDIG(x);
 ASSERT(m!=XPINF&&m!=XNINF&&0<m,EVDOMAIN);
 if(1>xcompare(x,xc(2147483647L)))R xco1(factor(sc(xint(x))));
 RZ(smallprimes(1229L,x,&z,&x));
 GAT0(st,XNUM,20,1); sv=sv0=XAV(st); *sv++=x;
 while(sv-sv0){
  x=*--sv; 
  if(2>(k=sv-sv0)){A stsav = st; GATV0(st,XNUM,2*AN(stsav),1); MC(XAV(st),sv0,k*sizeof(A)); sv0=XAV(st); sv=k+sv0;}
  if(1>xcompare(x,xc(2147483647L))){RZ(z=apip(z,factor(sc(xint(x))))); continue;}
  if(xprimeq(100L,x)){RZ(z=apip(z,scx(x))); continue;}
  RZ(g=pollard_p_1(x)); if(g!=iv1){*sv++=g; RZ(*sv++=xdiv(x,g,XMFLR)); continue;}
  RZ(g=pollard_rho(x)); if(g!=iv1){*sv++=g; RZ(*sv++=xdiv(x,g,XMFLR)); continue;}
  if(!b){b=1; RZ(rngseeds(sc(jt->rngdata->rngparms[jt->rngdata->rng].rngS))); RZ(roll(v2(m,m*m)));} 
  RZ(g=fac_ecm(x));     if(g!=iv1){*sv++=g; RZ(*sv++=xdiv(x,g,XMFLR)); continue;}
  ASSERT(0,EVNONCE);
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
 GAT0(z,XNUM,3,1); zv=XAV(z);
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
 GAT0(z,XNUM,3,1); zv=XAV(z);
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
 GAT0(z,XNUM,3,1); zv=XAV(z);
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
