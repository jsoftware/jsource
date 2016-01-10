/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Format: ": Dyad                                                         */

#include "j.h"


static F2(jtth2box){A z;I n,p,q,*v,x,y;
 p=jt->pos[0]; q=jt->pos[1];
 RZ(a=vi(a)); n=AN(a); v=AV(a);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(1==n||2==n,EVLENGTH);
 x=v[0]; y=2>n?0:v[1]; 
 ASSERT(0<=x&&x<=2&&0<=y&&y<=2,EVDOMAIN);
 jt->pos[0]=x; jt->pos[1]=y;
 z=thorn1(w); 
 jt->pos[0]=p; jt->pos[1]=q;
 R z;
}

static I jtc2j(J jt,B e,I m,C*zv){C c,*s,*t;I k,p;
 if(e&&(t=strchr(jt->th2buf,'e'))){
  ++t; t+='-'==*t;
  k=0; while(c=*(k+t),c=='0'||c=='+')++k;
  if(k){
   if(!c||' '==c){*t++='0'; --k;}
   while(*t=*(k+t))++t;
   p=m-(t-jt->th2buf); DO(p,*t++=' ';); if(0<=p)jt->th2buf[m]=0;
 }}
 t=jt->th2buf; k=strlen(t);
 if(!e&&(s=memchr(t,'-',k))){  /* turn -0 to 0 */
  *s=' '; 
  DO(k-(1+s-t), c=s[1+i]; if(c!='0'&&c!='.'){*s=CSIGN; break;});
  if(!m&&' '==*s){++t; --k;}
 }
 if(m&&m<k)memset(zv,'*',m);
 else{
  if(k<m){memset(zv+e*k,' ',m-k); if(!e)zv+=m-k;}
  DO(k, c=t[i]; *zv++='-'==c?CSIGN:c;);
 }
 R k;
}    /* c format to j format */

static B jtfmtex(J jt,I m,I d,I n,I*xv,B b,I c,I q,I ex){B bm=b||m;C*u,*v=jt->th2buf;I k;
 if(jt->th2bufn<20+d){A s; jt->th2bufn=20+d; GA(s,LIT,jt->th2bufn,1,0); v=jt->th2buf=CAV(s);}
 if(b)*v++='_'; else if(m)*v++=' '; *v++=' '; sprintf(v,FMTI,c); v+=q;
 k=(XBASEN+d+1-q)/XBASEN; k=MIN(n-1,k);
 DO(k, c=*--xv; sprintf(v,FMTI04,b?-c:c); v+=XBASEN;);
 k=v-jt->th2buf-(2+bm);
 if(k<d){memset(v,'0',d-k); v+=d-k;}
 else if(k>d&&(u=v=jt->th2buf+d+2+bm,'5'<=*v)){
  while('9'==*--u);
  if(' '!=*u)++*u; else{*++u='1'; ++ex;}
  memset(u+1,'0',v-u-1);
 }
 jt->th2buf[bm]=jt->th2buf[bm+1]; jt->th2buf[bm+1]='.'; sprintf(v-!d,"e"FMTI"",ex);
 R 1;
}    /* format one extended integer in exponential form */

static B jtfmtx(J jt,B e,I m,I d,C*s,I t,X*wv){B b;C*v=jt->th2buf;I c,n,p,q,*xv;X x;
 x=*wv; n=AN(x); xv=AV(x)+n-1; 
 c=*xv; b=0>c; if(b)c=-c;
 if(c==XPINF){if(b)*v++='_'; *v++='_'; *v=0; R 1;}
 q=c>999?4:c>99?3:c>9?2:1; p=q+XBASEN*(n-1);
 if(e)R fmtex(m,d,n,xv,b,c,q,p-1);
 else if(m&&m<b+p+d+!!d){memset(v,'*',m); v[m]=0;}
 else{
  if(jt->th2bufn<4+p+d){A s; jt->th2bufn=4+p+d; GA(s,LIT,jt->th2bufn,1,0); v=jt->th2buf=CAV(s);}
  if(' '==*s)*v++=' '; if(b)*v++='_'; 
  sprintf(v,FMTI,c); v+=q;
  DO(n-1, c=*--xv; sprintf(v,FMTI04,b?-c:c); v+=XBASEN;); 
  if(d){*v++='.'; memset(v,'0',d); v[d]=0;}
 }
 R 1;
}    /* format one extended integer */

static B jtfmtq(J jt,B e,I m,I d,C*s,I t,Q*wv){B b;C*v=jt->th2buf;I c,ex=0,k,n,p,q,*xv;Q y;X a,g,x;
 y=*wv; x=y.n; c=XDIG(x); b=0>c; if(b)x=negate(x);
 if(c==XPINF||c==XNINF){if(e)*v++=' '; if(e>b)*v++=' '; if(b)*v++='_'; *v++='_'; *v=0; R 1;}
 RZ(a=xpow(xc(10L),xc(1+d)));
 if(e&&c&&0>xcompare(x,y.d)){
  ex=XBASEN*(AN(y.n)-AN(y.d));
  g=xtymes(x,xpow(xc(10L),xc(1+d-ex)));
  RZ(x=xdiv(g,y.d,XMFLR));
  while(1==xcompare(a,x)){--ex; g=xtymes(xc(10L),g); RZ(x=xdiv(g,y.d,XMFLR));}
  if(b)x=negate(x);
 }else x=xdiv(xtymes(y.n,a),y.d,XMFLR);
 RZ(x=xdiv(xplus(x,xc(5L)),xc(10L),XMFLR));
 n=AN(x); xv=AV(x)+n-1; c=*xv; b=0>c; if(b)c=-c;
 q=c>999?4:c>99?3:c>9?2:1; p=q+XBASEN*(n-1); if(c||!e)ex+=p-d-1;
 if(e)R fmtex(m,d,n,xv,b,c,q,ex);
 else if(m&&m<b+d+!!d+(0>ex?1:1+ex)){memset(v,'*',m); v[m]=0;}
 else{
  if(jt->th2bufn<4+p+d){A s; jt->th2bufn=4+p+d; GA(s,LIT,jt->th2bufn,1,0); v=jt->th2buf=CAV(s);}
  if(' '==*s)*v++=' '; if(b)*v++='_';
  if(0>ex){k=-ex-1; DO(1+MIN(d,k), *v++='0';);}
  sprintf(v,FMTI,c); v+=q;
  DO(n-1, c=*--xv; sprintf(v,FMTI04,b?-c:c); v+=XBASEN;);
  if(d){v[1]=0; DO(d, *v=*(v-1); --v;); *v='.';}
 }
 R 1;
}    /* format one rational number */

static void jtfmt1(J jt,B e,I m,I d,C*s,I t,C*wv){D y;
 switch(t){
  case B01:  sprintf(jt->th2buf,s,(D)*wv);     break;
  case INT:  sprintf(jt->th2buf,s,(D)*(I*)wv); break;
  case XNUM: fmtx(e,m,d,s,t,(X*)wv);          break;
  case RAT:  fmtq(e,m,d,s,t,(Q*)wv);          break;
  default:
   y=*(D*)wv; y=y?y:0.0;  /* -0 to 0 */
   if     (!memcmp(wv,&inf, SZD))strcpy(jt->th2buf,e?"  _" :' '==*s?" _" :"_" );
   else if(!memcmp(wv,&infm,SZD))strcpy(jt->th2buf,e?" __" :' '==*s?" __":"__");
   else if(_isnan(*wv)          )strcpy(jt->th2buf,e?"  _.":' '==*s?" _.":"_.");
   else sprintf(jt->th2buf,s,y);
}}   /* format one number */

static void jtth2c(J jt,B e,I m,I d,C*s,I n,I t,I wk,C*wv,I zk,C*zv){
 DO(n, fmt1(e,m,d,s,t,wv); c2j(e,m,zv); zv+=zk; wv+=wk;);
}    /* format a column */

static A jtth2a(J jt,B e,I m,I d,C*s,I n,I t,I wk,C*wv,B first){PROLOG;A y,z;B b=0;C*u,*yv,*zv;I i,m0=m,k,p,q;
 q=m?m:t&B01?3:t&INT?12:17; p=n*q;
 GA(z,LIT,p,2,0); *AS(z)=n; *(1+AS(z))=q; zv=CAV(z);
 if(m){th2c(e,m,d,s,n,t,wk,wv,m,zv); R z;}
 for(i=q=0;i<n;++i){
  fmt1(e,m0,d,s,t,wv);
  while(p<q+(I)strlen(jt->th2buf)+1){RZ(z=over(z,z)); p+=p; zv=CAV(z);}
  u=q+zv; q+=k=c2j(e,0L,u); b=b||CSIGN==*u; zv[q++]=0; m=MAX(m,k); wv+=wk;
 }
 m+=!first;
 GA(y,LIT,n*m,2,0); *AS(y)=n; *(1+AS(y))=m;
 yv=CAV(y); memset(yv,' ',AN(y));  u=zv; 
 if(e){yv+=!first; DO(n, q=strlen(u); MC(yv+(b&&CSIGN!=*u),u,q); yv+=m; u+=1+q;);}
 else {yv+=m;      DO(n, q=strlen(u); MC(yv-q,          u,q); yv+=m; u+=1+q;);}
 EPILOG(y);
}    /* like th2c, but allocates and returns array */

static B jtth2ctrl(J jt,A a,A*ep,A*mp,A*dp,A*sp,I*zkp){A da,ea,ma,s;B b=1,*ev,r;
  C*sv;D x,y;I an,*av,d,*dv,i,m,*mv,sk=15,zk=0;Z*au;
 r=!(CMPX&AT(a)); jt->th2bufn=500;
 if(r)RZ(a=cvt(INT,a));
 an=AN(a); au=ZAV(a); av=AV(a);
 GA(ea,B01,an,   1,0); *ep=ea; ev=BAV(ea);
 GA(ma,INT,an,   1,0); *mp=ma; mv= AV(ma);
 GA(da,INT,an,   1,0); *dp=da; dv= AV(da);
 GA(s, LIT,an*sk,2,0); *sp=s;  sv=CAV(s); *AS(s)=an; *(1+AS(s))=sk;
 for(i=0;i<an;++i){
  if(r){m=av[i]; x=(D)m; d=0;}
  else{
   x=au[i].re; m=(I)tfloor(x); ASSERT(teq(x,(D)m),EVDOMAIN);
   y=au[i].im; d=(I)tfloor(y); ASSERT(teq(y,(D)d),EVDOMAIN); if(0>y)x=-1;
  }
  if(0>m)m=-m; if(0>d)d=-d; ASSERT(0<=m&&0<=d,EVLIMIT);
  if(0<=x)sprintf(sv, "%%"FMTI"."FMTI"f",  m,d);
  else    sprintf(sv, m?"%%- "FMTI"."FMTI"e" :"%%-"FMTI"."FMTI"e", m?m-1:0,d+!!(SYS&SYS_PC));
  sv+=sk; ev[i]=0>x; mv[i]=m; dv[i]=d; zk+=m; b=b&&m; 
  if(jt->th2bufn<m)jt->th2bufn=m; if(jt->th2bufn<500+d)jt->th2bufn=500+d;
 }
 GA(s,LIT,jt->th2bufn,1,0); jt->th2buf=CAV(s);
 *zkp=b?zk:0; R 1;
}    /* parse format control (left argument of ":) */

F2(jtthorn2){PROLOG;A da,ea,h,ma,s,y,*yv,z;B e,*ev;C*sv,*wv,*zv;I an,c,d,*dv,k,m,*mv,n,r,sk,t,wk,*ws,zk;
 F2RANK(1,RMAX,jtthorn2,0);
 an=AN(a); t=AT(w);
 if(t&BOX)R th2box(a,w);
 ASSERT(t&NUMERIC&&!(t&SPARSE)&&!(AT(a)&SPARSE),EVDOMAIN);
 r=AR(w); ws=AS(w); c=r?ws[r-1]:1; n=c?AN(w)/c:prod(r-1,ws);
 ASSERT(!AR(a)||c==an,EVLENGTH);
 k=bp(t); wk=c*k; wv=CAV(w)-k;
 RZ(th2ctrl(a,&ea,&ma,&da,&s,&zk));
 ev=BAV(ea); mv=AV(ma); dv=AV(da); sk=1<an?*(1+AS(s)):0; sv=CAV(s)-sk;
 if(zk||!AN(w)){
  if(1==an)zk*=c;
  GA(z,LIT,n*zk,r?r:1,ws); *(AS(z)+AR(z)-1)=zk; zv=CAV(z);
  DO(c, if(i<an){e=ev[i]; m=mv[i]; d=dv[i];} th2c(e,m,d,sv+=sk,n,t,wk,wv+=k,zk,zv); zv+=m;);
 }else{
  GA(y,BOX,c,1,0); yv=AAV(y);
  DO(c, if(i<an){e=ev[i]; m=mv[i]; d=dv[i];} RZ(yv[i]=th2a(e,m,d,sv+=sk,n,t,wk,wv+=k,(B)!i)););
  RZ(z=razeh(y));
  if(2<r||1==n&&2!=r){
   if(!r)r=1; 
   RZ(h=vec(INT,r,ws)); *(AV(h)+r-1)=*(1+AS(z)); 
   RZ(z=reshape(h,z));
 }}
 EPILOG(z);
}
