/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Linear and Paren                                       */

#include "j.h"

#if !SY_WINCE
// extern int isdigit(int);
#include <ctype.h>
#endif

static F1(jtlnoun);
static F1(jtlnum);
static DF1(jtlrr);

#define NUMV(c)  (c==C9||c==CD||c==CA||c==CS)


static B jtlp(J jt,A w){B b=1,p=0;C c,d,q=CQUOTE,*v;I j=0,n;
 RZ(w);
 n=AN(w); v=CAV(w); c=*v; d=*(v+n-1);
 if(1==n||(2==n||3>=n&&' '==c)&&(d==CESC1||d==CESC2)||vnm(n,v))R 0;
 if(C9==ctype[(UC)c])DQ(n-1, d=c; c=ctype[(UC)*++v]; if(b=!NUMV(c)||d==CS&&c!=C9)break;)
 else if(c==q)   DQ(n-1, c=*v++; p^=(c==q); if(b=(p^1)&(c!=q)){break;})
 else if(c=='(') DQ(n-1, c=*v++; j+=c=='('?1:c==')'?-1:0; if(b=!j)break;)
 R b;
}    /* 1 iff put parens around w */

static A jtlcpa(J jt,B b,A w){A z=w;C*zv;I n;
 RZ(w);
 if(b){n=AN(w); GATV0(z,LIT,2+n,1); zv=CAV(z); *zv='('; MC(1+zv,AV(w),n); zv[1+n]=')';}
 R z;
}    /* if b then (w) otherwise just w */

static A jtlcpb(J jt,B b,A w){A z=w;B p;C c,*v,*wv,*zv;I n;
 RZ(w);
 n=AN(w); wv=CAV(w); 
 if(!b){
  c=ctype[(UC)*wv]; v=wv; p=0;
  if     (c==CQ)DQ(n-1, c=ctype[(UC)*++v]; p^=(c==CQ); if(p&(c!=CQ)){b=1; break;})
  else if(c==C9)DQ(n-1, c=ctype[(UC)*++v]; if(!(c==C9   ||c==CS   )){b=1; break;})
  else          DQ(n-1, c=      *++v ; if(!(c==CESC1||c==CESC2)){b=1; break;});
  if(b&&vnm(n,wv))b=0;
 }
 if(b){GATV0(z,LIT,2+n,1); zv=CAV(z); *zv='('; MC(1+zv,wv,n); zv[1+n]=')';}
 R z;
}

static A jtlcpx(J jt,A w){RZ(w); R CALL2(jt->lcp,lp(w),w,0);}

static F1(jtltiea){A t,*v,*wv,x,y;B b;C c;I n;
 RZ(w);
 n=AN(w); wv=AAV(w);  RZ(t=spellout(CGRAVE));
 GATV0(y,BOX,n+n,1); v=AAV(y);
 DO(n, *v++=i?t:mtv; x=wv[i]; c=ID(x); RZ(x=lrr(x)); 
     b=(C)(c-CHOOK)<=(C)(CFORK-CHOOK)||i&&lp(x); RZ(*v++=CALL2(jt->lcp,b,x,0)););
 R raze(y);
}

static F1(jtltieb){A pt,t,*v,*wv,x,y;B b;C c,*s;I n;
 RZ(w);
 n=AN(w); wv=AAV(w);  RZ(t=spellout(CGRAVE)); RZ(pt=over(scc(')'),t));
 GATV0(y,BOX,n+n,1); v=AAV(y);
 if(1>=n)x=mtv; else{GATV0(x,LIT,n-2,1); s=CAV(x); DQ(n-2, *s++='(';);}
 DO(n, x=i==1?t:x; x=i>1?pt:x; *v++=x; x=wv[i]; c=ID(x); RZ(x=lrr(x)); 
     b=(C)(c-CHOOK)<=(C)(CFORK-CHOOK)||i&&lp(x); RZ(*v++=CALL2(jt->lcp,b,x,0)););
 R raze(y);
}

static F1(jtlsh){R apip(thorn1(shape(w)),spellout(CDOLLAR));}

static F1(jtlshape){I r,*s;
 RZ(w);
 r=AR(w); s=AS(w);
 R 2==r&&(1==s[0]||1==s[1]) ? spellout((C)(1==s[1]?CCOMDOT:CLAMIN)) : !r ? mtv :
     1<r ? lsh(w) : 1<AN(w) ? mtv : spellout(CCOMMA);
}

static F1(jtlchar){A y;B b,p=1,r1;C c,d,*u,*v;I j,k,m,n;
 RZ(w);
 m=AN(alp); n=AN(w); j=n-m; r1=1==AR(w); u=v=CAV(w); d=*v;
 if(0<=j&&r1&&!memcmp(v+j,AV(alp),m)){ 
  if(!j)R cstr("a.");
  RZ(y=lchar(1==j?scc(*v):str(j,v)));
  R lp(y)?over(cstr("a.,~"),y):over(y,cstr(",a."));
 }
 if(r1&&m==n&&(y=icap(ne(w,alp)))&&m>AN(y)){
  if(1==AN(y))RZ(y=head(y));
  R over(over(cstr("a. "),lcpx(lnum(y))),over(cstr("}~"),lchar(from(y,w))));
 }
 j=2; b=7<n||1<n&&1<AR(w);
 DQ(n, c=*v++; j+=c==CQUOTE; b&=c==d; p&=(C)(c-32)<(C)(127-32);); 
 if(b){n=1; j=MIN(3,j);}
 if(!p){
  k=(UC)d; RZ(y=indexof(alp,w));
  if(r1&&n<m&&(!k||k==m-n)&&equ(y,apv(n,k,1L)))R over(thorn1(sc(d?-n:n)),cstr("{.a."));
  RZ(y=lnum(y));
  R lp(y)?over(cstr("a.{~"),y):over(y,cstr("{a.")); 
 }
 GATV0(y,LIT,n+j,1); v=CAV(y);
 *v=*(v+n+j-1)=CQUOTE; ++v;
 if(2==j)MC(v,u,n); else DQ(n, *v++=c=*u++; if(c==CQUOTE)*v++=c;);
 R over(b?lsh(w):lshape(w),y);
}    /* non-empty character array */

static F1(jtlbox){A p,*v,*vv,*wv,x,y;B b=0;I n;
 RZ(w);
 if(equ(ace,w)&&B01&AT(AAV0(w)))R cstr("a:");
 n=AN(w); wv=AAV(w); 
 DO(n, x=wv[i]; if(BOX&AT(x)){b=1; break;}); b|=1==n;
 GATV0(y,BOX,n+n-(1^b),1); v=vv=AAV(y);
 if(b){
  RZ(p=cstr("),(<"));
  DO(n, x=wv[i]; *v++=p; RZ(*v++=lnoun(x)););
  RZ(*vv=cstr(1==n?"<":"(<")); if(1<n)RZ(vv[n+n-2]=cstr("),<"));
  R over(lshape(w),raze(y));
 }
 DO(n, x=wv[i]; if((AR(x)^1)|(~AT(x)&LIT)){b=1; break;});
 if(!b){C c[256],d,*t;UC*s;
  memset(c,1,sizeof(c)); 
  RZ(x=raze(w)); s=UAV(x);
  DQ(AN(x), c[*s++]=0;);
  if(c[CQUOTE]&&equ(w,words(x)))R over(cstr(";:"),lchar(x));
  if(c[d=' ']||c[d='|']||c[d='/']||c[d=',']||c[d=';']){
   GATV0(y,LIT,n+AN(x),1); t=CAV(y);
   DO(n, x=wv[i]; *t++=d; MC(t,AV(x),AN(x)); t+=AN(x););
   RZ(y=lchar(y));
   R over(lshape(w),over(cstr(isdigit(*CAV(y))?"<;.(_1) ":"<;._1 "),y));
 }}
 RZ(p=cstr(";"));
 DO(n-1, RZ(*v++=lcpx(lnoun(wv[i]))); *v++=p;);
 RZ(*v=lnoun(wv[n-1]));
 R over(lshape(w),raze(y));
}    /* non-empty boxed array */

// Apply decoration as needed to a numeric character string w to give it the correct type t
// Result is A block for decorated string
A jtdecorate(J jt,A w,I t){
 if(AN(w)==0)R w;  // if empty string, don't decorate
 if(t&FL){
  // float: make sure there is a . somewhere, or infinity/indefinite ('_' followed by space/end/.), else put '.' on the end or at position of first 'e'
  B needdot = !memchr(CAV(w),'.',AN(w));  // check for decimal point
  if(needdot){DO(AN(w), if(CAV(w)[i]=='_' && (i==AN(w)-1 || CAV(w)[i+1]==' ')){needdot=0; break;} )}  // check for infinity
  if(needdot){w=over(w,scc('.')); RZ(w=rifvs(w)); DQ(AN(w) , if(CAV(w)[i]==' ')R w;  if(CAV(w)[i]=='e'){C f='.'; C *s=&CAV(w)[i]; DO(AN(w)-i, C ff=s[i]; s[i]=f; f=ff;)}) }
 }else if(t&INT){
  // integer: if the string contains nothing but one-digit 0/1 values, prepend a '0'
  I l=AN(w); C *s=CAV(w); do{if((*s&-2)!='0')break; ++s; if(--l==0)break; if(*s!=' ')break; ++s;}while(--l);
  if(l==0)w=over(scc('0'),w);
 }else if(t&XNUM+RAT){
  // numeric/rational: make sure there is an r/x somewhere in the string, else put one on the end
  if(!memchr(CAV(w),t&XNUM?'x':'r',AN(w)))w=apip(w,scc('x'));
 }
 R w;
}

static F1(jtlnum1){A z;I t;
 RZ(w);
 t=AT(w);
 RZ(z=t&FL+CMPX?df1(w,fit(ds(CTHORN),sc((I)18))):thorn1(w));
 R decorate(z,t);
}    /* dense non-empty numeric vector */

static F1(jtlnum){A b,d,t,*v,y;B p;I n;
 RZ(t=ravel(w));
 n=AN(w);
 if(7<n||1<n&&1<AR(w)){
  // see if we can use a clever encoding
  d=minus(from(num[1],t),b=from(num[0],t));
  p=equ(t,plus(b,tymes(d,IX(n))));
  if(p){
   if(equ(d,num[0]))R over(lsh(w),lnum1(b));
   GAT0(y,BOX,6,1); v=AAV(y); v[0]=v[1]=v[2]=v[3]=mtv;
   if(p=!(equ(b,sc(n-1))&&equ(d,num[-1]))){
    if     (!equ(b,num[0]   )){v[0]=lnum1(b); v[1]=spellout(CPLUS);}
    if     ( equ(d,num[-1])) v[1]=spellout(CMINUS);
    else if(!equ(d,num[1]    )){v[2]=lnum1(d); v[3]=spellout(CSTAR);}
   }
   v[4]=spellout(CIOTA); v[5]=thorn1(p?shape(w):negate(shape(w)));
   RE(y); R raze(y);
  }
  RESETERR;   // if there was an error getting to p, clear it
 }
 // not clever; just out the atoms
 R over(lshape(w),lnum1(t));
}    /* dense numeric non-empty array */

static F1(jtlsparse){A a,e,q,t,x,y,z;B ba,be,bn;I j,r,*v;P*p;
 RZ(w);
 r=AR(w); p=PAV(w); a=SPA(p,a); e=SPA(p,e); y=SPA(p,i); x=SPA(p,x);
 bn=0; v=AS(w); DQ(r, if(!*v++){bn=1; break;});
 ba=0; if(r==AR(a)){v=AV(a); DO(r, if(i!=*v++){ba=1; break;});}
 be=!(AT(w)&SFL&&0==*DAV(e));
 if(be)RZ(z=over(lnoun(e),cstr(SB01&AT(w)?"":SINT&AT(w)?"+-~2":SFL&AT(w)?"+-~2.1":"+-~2j1")));
 if(be||ba){
  RZ(z=be?over(lcpx(lnoun(a)),       over(scc(';'),z)):lnoun(a));
  RZ(z=   over(lcpx(lnoun(shape(w))),over(scc(';'),z))         );
 }else RZ(z=lnoun(shape(w))); 
 RZ(z=over(cstr("1$."),z));
 if(bn||!*AS(y))R z;
 if(AN(a)){
  RZ(x=lcpx(lnoun(x)));
  RZ(y=1==r?lnoun(ravel(y)):over(cstr("(<\"1)"),lnoun(y)));
  RZ(t=over(x,over(cstr(" ("),over(y,cstr(")}"))))); 
 }else RZ(t=over(lcpx(lnoun(head(x))),cstr(" a:}"))); 
 ba=0; v=AV(a); DO(AN(a), if(i!=*v++){ba=1; break;});
 if(!ba)R over(t,z);
 RZ(q=less(IX(r),a));
 RZ(z=over(over(lcpx(lnoun(q)),cstr("|:")),z));
 RZ(z=over(t,z));
 RZ(q=grade1(over(less(IX(r),q),q)));
 j=r; v=AV(q); DO(r, if(i!=*v++){j=i; break;});
 R over(lcpx(lnoun(drop(sc(j),q))),over(cstr("|:"),z));
}    /* sparse array */

static F1(jtlnoun0){A s,x;B r1;
 RZ(w);
 r1=1==AR(w); RZ(s=thorn1(shape(w)));
 switch(CTTZ(AT(w))){
  default:   R over(cstr("i."),s);
  case LITX:  x=cstr(   "''"); R r1?x:apip(apip(s,scc('$')),x);
  case C2TX:  x=cstr("u: ''"); R r1?x:apip(apip(s,scc('$')),x);
  case C4TX:  x=cstr("10&u: ''"); R r1?x:apip(apip(s,scc('$')),x);
  case BOXX:  R apip(s,cstr("$a:"    ));
  case B01X:  R apip(s,cstr("$0"     ));
  case FLX:   R apip(s,cstr("$0.5"   ));
  case CMPXX: R apip(s,cstr("$0j5"   ));
  case XNUMX: R apip(s,cstr("$0x"    ));
  case RATX:  R apip(s,cstr("$1r2"   ));
  case SBTX:  R apip(s,cstr("$s: ' '"));
}}   /* empty dense array */

static F1(jtlnoun){I t;
 RZ(w);
 t=AT(w);
 if(t&SPARSE)R lsparse(w);
 if(!AN(w))R lnoun0(w);
 switch(CTTZ(t)){
  case LITX: R lchar(w);
  case C2TX: R over(cstr("u: "),lnum(uco2(num[3],w)));
  case C4TX: R over(cstr("10&u: "),lnum(uco2(num[3],w)));
  case BOXX: R lbox(w);
  case SBTX: R over(cstr("s: "),lbox(sb2(num[5],w)));
  default:  R lnum(w);
}}

static A jtlsymb(J jt,C c,A w){A t;C buf[20],d,*s;I*u;V*v=FAV(w);
 if(VDDOP&v->flag){
  u=AV(v->fgh[2]); s=buf; 
  *s++=' '; *s++='('; s+=sprintf(s,FMTI,*u); spellit(CIBEAM,s); s+=2; s+=sprintf(s,FMTI,u[1]); *s++=')';
  RZ(t=str(s-buf,buf)); 
 }else RZ(t=spella(w));
 d=cf(t);
 R d==CESC1||d==CESC2?over(chr[' '],t):t;
}

static B laa(A a,A w){C c,d;
 if(!(a&&w))R 0;
 c=ctype[(UC)cl(a)]; d=ctype[(UC)cf(w)];
 R (c==C9||c==CA)&&(d==C9||d==CA);
}

// Is a string a number?  Must start with a digit and end with digit, x, or .
static B lnn(A a,A w){C c; if(!(a&&w))R 0; c=cl(a); R ('x'==c||'.'==c||C9==ctype[(UC)c])&&C9==ctype[(UC)cf(w)];}

static F2(jtlinsert){A*av,f,g,h,t,t0,t1,t2,*u,y;B b,ft,gt,ht;C c,id;I n;V*v;
 RZ(a&&w);
 n=AN(a); av=AAV(a);  
 v=VAV(w); id=v->id;
 b=id==CCOLON&&VXOP&v->flag;
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=v->fgh[fndx]; A gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
// ?t tells whether () is needed around the f/g/h component
 if(1<=n){f=av[0]; t=fs; c=ID(t); ft=c==CHOOK||c==CFORK||c==CADVF||(b||id==CFORK)&&NOUN&AT(t)&&lp(f);}  // f: () if it's hook fork && or noun left end of nvv or n (op)
 if(2<=n){g=av[1]; t=gs; c=ID(t); gt=VERB&AT(w)    ?c==CHOOK||c==CFORK:lp(g);}
 if(3<=n){h=av[2]; t=v->fgh[2]; c=ID(t); ht=VERB&AT(w)&&!b?c==CHOOK          :lp(h);}
 switch(!b?id:2==n?CHOOK:CFORK){
  case CADVF:
  case CHOOK:
   GAT0(y,BOX,3,1); u=AAV(y);
   u[0]=f=CALL2(jt->lcp,ft||lnn(f,g),f,0);
   u[2]=g=CALL2(jt->lcp,gt||b,       g,0);
   u[1]=str(' '==cf(g)||id==CADVF&&!laa(f,g)&&!(lp(f)&&lp(g))?0L:1L," ");
   RE(0); R raze(y);
  case CFORK:
   GAT0(y,BOX,5,1); u=AAV(y);
   RZ(u[0]=f=CALL2(jt->lcp,ft||lnn(f,g),   f,0));
   RZ(u[2]=g=CALL2(jt->lcp,gt||lnn(g,h)||b,g,0)); RZ(u[1]=str(' '==cf(g)?0L:1L," "));
   RZ(u[4]=h=CALL2(jt->lcp,ht,             h,0)); RZ(u[3]=str(' '==cf(h)?0L:1L," "));
   R raze(y);
  default:
   t0=CALL2(jt->lcp,ft||NOUN&AT(fs)&&!(VGERL&v->flag)&&lp(f),f,0);
   t1=lsymb(id,w);
   y=over(t0,laa(t0,t1)?over(chr[' '],t1):t1);
   if(1==n)R y;
   t2=lcpx(g);
   R over(y,laa(y,t2)?over(chr[' '],t2):t2);
}}

static F1(jtlcolon){A*v,x,y;C*s,*s0;I m,n;
 RZ(y=unparsem(num[1],w));
 n=AN(y); v=AAV(y); RZ(x=lrr(VAV(w)->fgh[0]));
 if(2>n||2==n&&1==AN(v[0])&&':'==*CAV(v[0])){
  if(!n)R over(x,str(5L," : \'\'"));
  y=lrr(v[2==n]);
  if(2==n)y=over(str(5L,"\':\'; "),y);
  R over(over(x,str(3L," : ")),lcpx(y));
 }
 m=0; DO(n, m+=AN(v[i]););
 GATV0(y,LIT,2+n+m,1);
 s=s0=CAV(y);
 DO(n, *s++=CLF; y=v[i]; m=AN(y); MC(s,CAV(y),m); s+=m;);
 *s++=CLF; *s++=')'; 
 RZ(y=str(s-s0,s0));
 jt->ltext=jt->ltext?over(jt->ltext,y):y;
 R over(x,str(4L," : 0"));
}

// Main routine for () and linear rep.  w is to be represented
static DF1(jtlrr){A hs,t,*tv;C id;I fl,m;V*v;
 RZ(w);
 // If name, it must be in ".@'name', or (in debug mode) the function name, which we will discard
 if(AT(w)&NAME){RZ(w=sfn(0,w));}
 if(AT(w)&NOUN)R lnoun(w);
 // if f is 0, we take f from g.  In other words, adverbs can put their left arg in either f or g.  u b. uses g so that it can leave f=0 to allow it to function as an ATOMIC2 op
 v=VAV(w); id=v->id;
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=v->fgh[fndx]; A gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 hs=v->fgh[2]; fl=v->flag; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
 if(fl&VXOPCALL)R lrr(hs);
 m=(I )!!fs+(I )(gs&&id!=CBOX)+(I )(id==CFORK)+(I )(hs&&id==CCOLON&&VXOP&fl);  // BOX has g for BOXATOP; ignore it
 if(!m)R lsymb(id,w);
 if(evoke(w)){RZ(w=sfne(w)); if(FUNC&AT(w))w=lrr(w); R w;}  // keep named verb as a string, UNLESS it is NMDOT, in which case use the (f.'d) verb value
 if(!(VXOP&fl)&&hs&&BOX&AT(hs)&&id==CCOLON)R lcolon(w);
 GATV0(t,BOX,m,1); tv=AAV(t);
 if(2<m)RZ(tv[2]=lrr(hs));
 // for top-level of gerund (indicated by self!=0), any noun type could not have come from an AR, so return it as is
 if(1<m)RZ(tv[1]=fl&VGERR?CALL1(jt->ltie,self?fxeachacv(gs):fxeach(gs),0L):lrr(gs));
 if(0<m)RZ(tv[0]=fl&VGERL?CALL1(jt->ltie,self?fxeachacv(fs):fxeach(fs),0L):lrr(fs));
 R linsert(t,w);
}

// Create linear representation of w.  Call lrr, which creates an A for the text plus jt->ltext which is appended to it.
// jt->lcp and jt->ltie are routines for handling adding enclosing () and handling `
F1(jtlrep){PROLOG(0056);A z;
 jt->ltext=0; jt->lcp=(AF)jtlcpa; jt->ltie=jtltiea;
 RE(z=jtlrr(jt,w,w));  // the w for self is just any nonzero to indicate top-level call
 if(jt->ltext)z=apip(z,jt->ltext);
 jt->ltext=0;
 EPILOG(z);
}

// Create paren representation of w.  Call lrr, which creates an A for the text plus jt->ltext which is appended to it.
// jt->lcp and jt->ltie are routines for handling adding enclosing () and handling `
F1(jtprep){PROLOG(0057);A z;
 jt->ltext=0; jt->lcp=(AF)jtlcpb; jt->ltie=jtltieb;
 RE(z=jtlrr(jt,w,w));
 if(jt->ltext)z=apip(z,jt->ltext);
 jt->ltext=0;
 EPILOG(z);
}

