/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Word Formation                                                   */

#include "j.h"
#include "w.h"

#define SS              0    /* space                           */
#define SX              1    /* other                           */
#define SA              2    /* alphanumeric                    */
#define SN              3    /* N                               */
#define SNB             4    /* NB                              */
#define SNZ             5    /* NB.                             */
#define S9              6    /* numeric                         */
#define SQ              7    /* quote                           */
#define SQQ             8    /* even quotes                     */
#define SZ              9    /* trailing comment                */

#define EI              1    /* emit (b,i-1); b=.i              */
#define EN              2    /* b=.i                            */

typedef struct {C new,effect;} ST;

static ST state[10][9]={
/*SS */ {{SX,EN},{SS,0 },{SA,EN},{SN,EN},{SA,EN},{S9,EN},{SX,EN},{SX,EN},{SQ,EN}},
/*SX */ {{SX,EI},{SS,EI},{SA,EI},{SN,EI},{SA,EI},{S9,EI},{SX,0 },{SX,0 },{SQ,EI}},
/*SA */ {{SX,EI},{SS,EI},{SA,0 },{SA,0 },{SA,0 },{SA,0 },{SX,0 },{SX,0 },{SQ,EI}},
/*SN */ {{SX,EI},{SS,EI},{SA,0 },{SA,0 },{SNB,0},{SA,0 },{SX,0 },{SX,0 },{SQ,EI}},
/*SNB*/ {{SX,EI},{SS,EI},{SA,0 },{SA,0 },{SA,0 },{SA,0 },{SNZ,0},{SX,0 },{SQ,EI}},
/*SNZ*/ {{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SX,0 },{SX,0 },{SZ,0 }},
/*S9 */ {{SX,EI},{SS,EI},{S9,0 },{S9,0 },{S9,0 },{S9,0 },{S9,0 },{SX,0 },{SQ,EI}},
/*SQ */ {{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQQ,0}},
/*SQQ*/ {{SX,EI},{SS,EI},{SA,EI},{SN,EI},{SA,EI},{S9,EI},{SX,EI},{SX,EI},{SQ,0 }},
/*SZ */ {{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 }}
};
/*         CX      CS      CA      CN      CB      C9      CD      CC      CQ   */

F1(jtwordil){A z;C e,nv,s,t=0;I b,i,m,n,*x,xb,xe;ST p;UC*v;
 RZ(w);
 nv=0; s=SS;
 n=AN(w); v=UAV(w); GA(z,INT,1+n+n,1,0); x=1+AV(z);
 for(i=0;i<n;++i){
  p=state[s][wtype[v[i]]]; e=p.effect;
  if(e==EI){
   t&=s==S9;
   if(t){if(!nv){nv=1; xb=b;} xe=i;}
   else{if(nv){nv=0; *x++=xb; *x++=xe-xb;} *x++=b; *x++=i-b;}
  }
  s=p.new;
  if(e){b=i; t=s==S9;}
 }
 if(s==SQ){jsignal3(EVOPENQ,w,b); R 0;}
 t&=s==S9;
 if(t){*x++=xb=nv?xb:b; *x++=n-xb;}
 else{if(nv){*x++=xb; *x++=xe-xb;} if(s!=SS){*x++=b; *x++=n-b;}}
 m=x-AV(z); *AV(z)=s==SZ||s==SNZ?-m/2:m/2;
 R z;
}    /* word index & length; z is (# words),(i0,l0),(i1,l1),... */

/* locals in wordil:                                            */
/* b:  beginning index of current word                          */
/* e:  current effect                                           */
/* i:  index of current character being scanned                 */
/* m:  2 * actual number of words                               */
/* n:  length of input string w                                 */
/* nv: 1 iff numeric constant vector being built                */
/* p:  state table entry per current state & character          */
/* s:  current state                                            */
/* t:  1 iff current state is S9                                */
/* v:  ptr to input string                                      */
/* x:  ptr to current element of z being computed               */
/* xb: beginning index of current numeric vector                */
/* xe: end index of current numeric vector                      */
/* z:  result; maximum of n words                               */

F1(jtwords){A t,*x,z;C*s;I k,n,*y;
 F1RANK(1,jtwords,0);
 RZ(w=vs(w));
 RZ(t=wordil(w));
 s=CAV(w); y=AV(t); n=*y++; n=0>n?-n:n;
 GA(z,BOX,n,1,0); x=AAV(z);
 DO(n, k=*y++; RZ(*x++=str(*y++,s+k)););
 R z;
}


static A jtconstr(J jt,I n,C*s){A z;C b,c,p,*t,*x;I m=0;
 p=0; t=s; DO(n-2, c=*++t; b=c==CQUOTE; if(!b||p)m++;    p=b&&!p;);
 if(0==m)R aqq; else if(1==m&&(z=chr[(UC)s[1]]))R z;
 GA(z,LIT,m,1!=m,0); x=CAV(z);
 p=0; t=s; DO(n-2, c=*++t; b=c==CQUOTE; if(!b||p)*x++=c; p=b&&!p;);
 R z;
}

#define TNAME(i)    (NAME&AT(v[i]))
#define TASGN(i)    (ASGN&AT(v[i]))
#define TRBRACE(i)  (y=v[i], CRBRACE==ID(y))
#define TVERB(i,c)  (y=v[i], c      ==ID(y))
#define TAIA(i,j)   (TASGN(1) && TNAME(i) && TNAME(j) && AN(v[i])==AN(v[j]) && \
                        !memcmp(NAV(v[i])->s,NAV(v[j])->s,AN(v[i])))

F2(jtenqueue){A*v,*x,y,z;B b;C d,e,p,*s,*wi;I i,n,*u,wl;UC c;
 RZ(a&&w);
 s=CAV(w); u=AV(a); n=*u++; n=0>n?-(1+n):n;
 GA(z,BOX,n,1,0); x=v=AAV(z);
 for(i=0;i<n;i++){
  wi=s+*u++; wl=*u++; c=e=*wi; p=ctype[c]; b=0;
  if(1<wl){d=*(wi+wl-1); if(b=p!=C9&&d==CESC1||d==CESC2)e=spellin(wl,wi);}
  if(128>c&&(y=ds(e)))*x++=y;
  else if(e==CFCONS)RZ(*x++=FCONS(connum(wl-1,wi)))
  else switch(b?0:p){
   default: jsignal3(EVSPELL,w,wi-s); R 0;
   case C9: RZ(*x++=connum(wl,wi));   break;
   case CQ: RZ(*x++=constr(wl,wi));   break;
   case CA: ASSERTN(vnm(wl,wi),EVILNAME,nfs(wl,wi)); RZ(*x++=nfs(wl,wi)); 
 }}
 if(6<=n && TAIA(0,n-1) && ((b=TRBRACE(n-2)) || RPAR&AT(v[n-2])&&TRBRACE(n-3))){I c,j,p,q;
  /* abc=:pqr xyz}abc  or  abc=:pqr (xyz})abc */
  j=2; p=q=0;
  DO(n-j, y=v[j++]; c=AT(y); if(c&LPAR)++p; else if(c&RPAR)--p; if(!p)break;);
  DO(n-j, y=v[j++]; c=AT(y); if(c&LPAR)++q; else if(c&RPAR)--q; if(!q)break;);
  if(!p&&!q&&j>=n-2)v[b+n-3]=ds(CAMIP);
 }else if(5<=n && TAIA(0,2)){
  if(TVERB(3,CCOMMA) && !(AT(v[4])&ADV+CONJ) && 
      !(AT(v[4])&NAME&&(y=symbrd(v[4]),jt->etxn=jt->jerr=0,y)&&AT(y)&ADV+CONJ))
   v[3]=ds(CAPIP);  /* abc=: abc,blah */
  else if(7<=n&&!(AT(v[3])&VERB+ADV+CONJ)){I c,j,q;
   /* abc=:abc xyz}~ pqr */
   j=3; q=0;
   DO(n-j, y=v[j++]; c=AT(y); if(c&LPAR)++q; else if(c&RPAR)--q; if(!q)break;);
   if(!q&&j<n-2&&TRBRACE(j)&&v[1+j]==ds(CTILDE))v[j]=ds(CAMIP);
 }}else if(7<=n && TNAME(0) && TASGN(1) && TNAME(2) && TRBRACE(3) && TVERB(n-2,CLAMIN)){A p,*yv,z1;I c,j,k,m;
  /* abc=: pqr}x,y,:z */
  b=1; m=(n-3)/2; 
  j=4;      DO(m,   if(!TNAME(j)       ){b=0; break;} j+=2;);
  j=5; if(b)DO(m-2, if(!TVERB(j,CCOMMA)){b=0; break;} j+=2;);
  if(b){
   GA(z1,BOX,4,1,0); x=AAV(z1);
   GA(y,BOX,m+3,1,0); yv=AAV(y);
   c=-1; k=AN(v[0]); s=NAV(v[0])->s;
   j=4; DO(m, yv[i]=p=v[j]; j+=2; if(AN(p)&&!memcmp(s,NAV(p)->s,k))c=i;);
   yv[m]=v[2]; RZ(yv[m+1]=sc(c)); yv[m+2]=z;
   x[0]=v[0]; x[1]=v[1]; x[2]=ds(CCASEV); x[3]=y;
   R z1;
 }}
 R z;
}    /* produce boxed list of words suitable for parsing */
                                                            
/* locals in enqueue:                                           */
/* b:  1 iff current word is a primitive spelled with . or :    */
/* c:  first character in current word                          */
/* d:  last  character in current word                          */
/* e:  first character in current word, after spellin           */
/* i:  index of current word                                    */
/* n:  number of words                                          */
/* p:  character type of current character                      */
/* s:  ptr to value part of input string w                      */
/* u:  ptr to value part of word index & length info a          */
/* v:  ptr to value part of z                                   */
/* wi: index of current word in input string                    */
/* wl: length of current word                                   */
/* x:  ptr to result word being built                           */
/* y:  array temp                                               */
/* z:  result array of boxed list of words                      */


F1(jttokens){R enqueue(wordil(w),w);}


#define CHKJ(j)             ASSERT(0<=(j),EVINDEX);
#define EXTZ(T,p)           while(uu<p+u){k=u-(T*)AV(z); RZ(z=ext(0,z)); u=k+(T*)AV(z); uu=(T*)AV(z)+AN(z);}

#define EMIT0c(T,j,i,r,c)   {CHKJ(j); p=(i)-(j); EXTZ(T,1); RZ(*u++=str(p,(j)+wv));}
#define EMIT0b(T,j,i,r,c)   {CHKJ(j); p=(i)-(j); EXTZ(T,1); RZ(*u++=vec(B01,p,(j)+wv));}
#define EMIT0x(T,j,i,r,c)   {CHKJ(j); p=(i)-(j); EXTZ(T,1); GA(x,t0,p*wm,wr,AS(w0));  \
                                *AS(x)=p; MC(AV(x),wv0+wk*(j),wk*p); *u++=x;}
#define EMIT1(T,j,i,r,c)    {CHKJ(j); p=(i)-(j);            cc=(j)+wv; DO(p, *u++=*cc++;);}
#define EMIT1x(T,j,i,r,c)   {CHKJ(j); p=wk*((i)-(j));       MC(u,wv0+j*wk,p); u+=p;}
#define EMIT2(T,j,i,r,c)    {CHKJ(j); p=(i)-(j); EXTZ(T,2); *u++=(j); *u++=p;}
#define EMIT3(T,j,i,r,c)    {CHKJ(j);            EXTZ(T,1);                   *u++=(c)+q*(r);}
#define EMIT4(T,j,i,r,c)    {CHKJ(j); p=(i)-(j); EXTZ(T,3); *u++=(j); *u++=p; *u++=(c)+q*(r);}
#define EMIT5(T,j,x,r,c)    {if(0>(j))i=n;}

#define DO_ONE(T,EMIT) \
 switch(e=v[1]){                                                          \
  case 6:         i=n; break;                                             \
  case 2: case 3: if(0<=vi){EMIT(T,vj,vi,vr,vc); vi=vr=-1;} EMIT(T,j,i,r,c);       j=2==e?i:-1; break;  \
  case 4: case 5: if(r!=vr){if(0<=vi)EMIT(T,vj,vi,vr,vc); vj=j; vr=r; vc=c;} vi=i; j=4==e?i:-1; break;  \
  case 1:         j=i;                                                    \
 }

#define ZVAx                {}
#define ZVA5                {*zv++=i; *zv++=j; *zv++=r; *zv++=c; *zv++=v[0]; *zv++=v[1];}

#define FSMF(T,zt,zr,zm,cexp,EMIT,ZVA)    \
 {T*u,*uu;                                                                  \
  RZ(z=exta((zt),(zr),(zm),1==f||5==f?n:n/3));                              \
  if(1<(zr)){I*s=AS(z); s[1]=(zm); if(1==f&&2<wr)ICPY(1+s,1+AS(w0),wr-1);}  \
  zv=AV(z); u=(T*)zv; uu=u+AN(z);                                           \
  for(;i<n;++i,r=*v){c=(cexp); v=sv+2*(c+r*q); ZVA; DO_ONE(T,EMIT);}        \
  if(6!=e){                                                                 \
   if(0<=d)         {c=d;      v=sv+2*(c+r*q); ZVA; DO_ONE(T,EMIT);}        \
   else{                                                                    \
    if(0<=vi      )EMIT(T,vj,r==vr?n:vi,vr,vc);                             \
    if(r!=vr&&0<=j)EMIT(T,j,n,r,c);                                         \
  }}                                                                        \
  if(5==f)u=(T*)zv;                                                         \
  i=AN(z); AN(z)=j=u-(T*)AV(z); *AS(z)=j/(zm); if(i>3*j)RZ(z=ca(z));        \
 }

static A jtfsmdo(J jt,I f,A s,A m,I*ijrd,A w,A w0){A x,z;C*cc,*wv0;
     I c,d,e,i,j,k,*mv,n,p,q,r,*sv,t,t0,*v,vc,vi,vj,vr,wk,wm,wr,*zv;
 n=IC(w); t=AT(w);
 q=*(1+AS(s));
 sv=AV(s); mv=AV(m);
 i=ijrd[0]; j=ijrd[1]; r=ijrd[2]; d=ijrd[3]; vi=vj=vr=vc=-1;
 if(t&INT){t0=AT(w0); wr=AR(w0); wm=aii(w0); wk=wm*bp(AT(w0)); wv0=CAV(w0);}
 switch(f+(t&B01?0:t&LIT?10:20)){
  case  0: {B *wv=BAV(w); FSMF(A,BOX,1, 1,   wv[i] ,EMIT0b,ZVAx);} break;
  case  1: {B *wv=UAV(w); FSMF(B,B01,1, 1,   wv[i] ,EMIT1, ZVAx);} break;
  case  2: {B *wv=BAV(w); FSMF(I,INT,2, 2,   wv[i] ,EMIT2, ZVAx);} break;
  case  3: {B *wv=BAV(w); FSMF(I,INT,1, 1,   wv[i] ,EMIT3, ZVAx);} break;
  case  4: {B *wv=BAV(w); FSMF(I,INT,2, 3,   wv[i] ,EMIT4, ZVAx);} break;
  case  5: {B *wv=BAV(w); FSMF(I,INT,2, 6,   wv[i] ,EMIT5, ZVA5);} break;

  case 10: {UC*wv=UAV(w); FSMF(A,BOX,1, 1,mv[wv[i]],EMIT0c,ZVAx);} break;
  case 11: {UC*wv=UAV(w); FSMF(C,LIT,1, 1,mv[wv[i]],EMIT1, ZVAx);} break;
  case 12: {UC*wv=UAV(w); FSMF(I,INT,2, 2,mv[wv[i]],EMIT2, ZVAx);} break;
  case 13: {UC*wv=UAV(w); FSMF(I,INT,1, 1,mv[wv[i]],EMIT3, ZVAx);} break;
  case 14: {UC*wv=UAV(w); FSMF(I,INT,2, 3,mv[wv[i]],EMIT4, ZVAx);} break;
  case 15: {UC*wv=UAV(w); FSMF(I,INT,2, 6,mv[wv[i]],EMIT5, ZVA5);} break;

  case 20: {I *wv= AV(w); FSMF(A,BOX,1, 1,   wv[i] ,EMIT0x,ZVAx);} break;
  case 21: {I *wv= AV(w); FSMF(C,t0, wr,wm,  wv[i] ,EMIT1x,ZVAx);} break;
  case 22: {I *wv= AV(w); FSMF(I,INT,2, 2,   wv[i] ,EMIT2, ZVAx);} break;
  case 23: {I *wv= AV(w); FSMF(I,INT,1, 1,   wv[i] ,EMIT3, ZVAx);} break;
  case 24: {I *wv= AV(w); FSMF(I,INT,2, 3,   wv[i] ,EMIT4, ZVAx);} break;
  case 25: {I *wv= AV(w); FSMF(I,INT,2, 6,   wv[i] ,EMIT5, ZVA5);}
 }
 R z;
}

F1(jtfsmvfya){PROLOG;A a,*av,m,s,x,z,*zv;I ad,an,c,e,f,ijrd[4],k,p,q,*sv,*v;
 RZ(a=w);
 ASSERT(1==AR(a),EVRANK);
 ASSERT(BOX&AT(a),EVDOMAIN);
 an=AN(a); av=AAV(a); ad=(I)a*ARELATIVE(a);
 ASSERT(2<=an&&an<=4,EVLENGTH);
 RE(f=i0(AVR(0)));
 ASSERT(0<=f&&f<=5,EVINDEX);
 RZ(s=vi(AVR(1))); sv=AV(s);
 ASSERT(3==AR(s),EVRANK);
 v=AS(s); p=v[0]; q=v[1]; ASSERT(2==v[2],EVLENGTH);
 v=sv; DO(p*q, k=*v++; e=*v++; ASSERT(0<=k&&k<p&&0<=e&&e<=6,EVINDEX););
 ijrd[0]=0; ijrd[1]=-1; ijrd[2]=0; ijrd[3]=-1;
 if(4==an){I d,i,j,n,r;
  RZ(x=vi(AVR(3))); n=AN(x); v=AV(x);
  ASSERT(1==AR(x),EVRANK);
  ASSERT(4>=n,EVLENGTH);
  if(1<=n) ijrd[0]=i=*v++;
  if(2<=n){ijrd[1]=j=*v++; ASSERT(j==-1||0<=j&&j<i,EVINDEX);}
  if(3<=n){ijrd[2]=r=*v++; ASSERT(       0<=r&&r<p,EVINDEX);}
  if(4==n){ijrd[3]=d=*v++; ASSERT(d==-1||0<=d&&d<q,EVINDEX);}
 }
 m=2==an?mtv:AVR(2); c=AN(m);
 ASSERT(1>=AR(m),EVRANK);
 if(!c&&1==AR(m)){   /* m is empty; w must be integer vector */  }
 else if(NUMERIC&AT(m)){
  ASSERT(c==AN(alp),EVLENGTH);
  RZ(m=vi(m)); v=AV(m); DO(c, k=v[i]; ASSERT(0<=k&&k<q,EVINDEX););
 }else ASSERT(BOX&AT(m),EVDOMAIN);
 GA(z,BOX,4,1,0); zv=AAV(z);
 RZ(zv[0]=sc(f)); zv[1]=s; zv[2]=m; RZ(zv[3]=vec(INT,4L,ijrd));
 R z;
}    /* check left argument of x;:y */

static A jtfsm0(J jt,A a,A w,C chka){PROLOG;A*av,m,s,x,w0=w;B b;I ad,c,f,*ijrd,k,md,n,p,q,*v;
 RZ(a&&w);
 if(chka)RZ(a=fsmvfya(a)); 
 av=AAV(a); ad=(I)a*ARELATIVE(a);
 f=i0(AVR(0)); s=AVR(1); m=AVR(2); ijrd=AV(AVR(3));
 n=AN(w); v=AS(s); p=v[0]; q=v[1];
 ASSERT(0<=ijrd[0]&&ijrd[0]<n,EVINDEX);
 b=1>=AR(w)&&(!n||LIT&AT(w)); c=AN(m);
 if(!c&&1==AR(m)){
  ASSERT(1>=AR(w),EVRANK);
  if(!(B01&AT(w))){RZ(w=w0=vi(w)); v=AV(w); DO(n, k=v[i]; ASSERT(0<=k&&k<q,EVINDEX););}
 }else if(NUMERIC&AT(m)){
  ASSERT(b,EVDOMAIN); 
  ASSERT(1>=AR(w),EVRANK);
 }else{A*mv,t,y;I j,r;
  ASSERT(BOX&AT(m),EVDOMAIN);
  RZ(y=raze(m)); r=AR(y); k=AN(y);
  ASSERT(r==AR(w)||r==1+AR(w),EVRANK);
  GA(x,INT,1+k,1,0); v=AV(x); v[k]=c; mv=AAV(m); md=(I)m*ARELATIVE(m); 
  DO(c, j=i; t=AADR(md,mv[i]); if(r&&r==AR(t))DO(*AS(t), *v++=j;) else *v++=j;);
  if(b){RZ(m=from(indexof(y,alp),x)); v=AV(m); DO(AN(alp), k=v[i]; ASSERT(0<=k&&k<q,EVINDEX););}
  else {ASSERT(q>c,EVINDEX); RZ(w=from(indexof(y,w),x));}
 }
 EPILOG(fsmdo(f,s,m,ijrd,w,w0));
}

F2(jtfsm){R fsm0(a,w,1);}
     /* x;:y */

DF1(jtfsmfx){RZ(w&&self); R fsm0(VAV(self)->f,w,0);}
     /* x&;: y */
