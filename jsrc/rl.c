/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Linear and Paren                                       */

#include "j.h"

#if !SY_WINCE
// extern int isdigit(int);
#include <ctype.h>
#endif

// functions of this class add to the growing ltext
#define F1X(f)           A f(J jt,A w,A *ltext)
#define DF1X(f)           A f(J jt,A w,A self,A *ltext)
#define F2X(f)           A f(J jt,A a,A w,A *ltext)
static F1X(jtlnoun);
static F1X(jtlnum);
static DF1X(jtlrr);

#define NUMV(c)  (((1LL<<C9)|(1LL<<CD)|(1LL<<CS)|(1LL<<CA)|(1LL<<CN)|(1LL<<CB))&(1LL<<(c)))

// choose function for applying parentheses, for normal verbs and for tie strings
#define parfn ((I)jtinplace&JTPARENS?jtlcpb:jtlcpa)
#define tiefn ((I)jtinplace&JTPARENS?jtltieb:jtltiea)

// w is the displayable string for an entity.  esult is 1 if it needs parens if next to anything else; or -1 if a primitive that never needs parens
static I jtlp(J jt,A w){F1PREFIP;B b=1,p=0;C c,d,q=CQUOTE,*v;I j=0,n;
 ARGCHK1(w);
 n=AN(w); v=CAV(w); c=*v; d=v[n-1];
 if(1==n||(2==n||3>=n&&' '==c)&&(d==CESC1||d==CESC2)||vnm(n,v))R -1;  // if a primitive or name, it doesn't need parens
 if(C9==ctype[(UC)c])DQ(n-1, d=c; c=ctype[(UC)*++v]; if(b=!NUMV(c)||d==CS&&c!=C9)break;)  // numeric field: parens if contains nonnumeric char or space followed by non-(digit/sign)
 else if(c==q)   DQ(n-1, c=*v++; p^=(c==q); if(b=(p^1)&(c!=q)){break;})  // quoted string: paren if there is any non-quote after the matching quote
 else if(c=='(') DQ(n-1, c=*v++; j+=c=='('?1:c==')'?-1:0; if(b=!j)break;)  // (: paren if there is a character after matching )
 R b;
}    /* 1 iff put parens around w */

// add () when user asks for normal ()
static A jtlcpa(J jt,B b,A w){F1PREFIP;A z=w;C*zv;I n;
 ARGCHK1(w);
 if(b){n=AN(w); GATV0(z,LIT,2+n,1); zv=CAV1(z); *zv='('; MC(1+zv,AV(w),n); zv[1+n]=')';}
 R z;
}    /* if b then (w) otherwise just w */

// add (), when user asks for full ()
static A jtlcpb(J jt,B b,A w){F1PREFIP;A z=w;B p;C c,*v,*wv,*zv;I n;
 ARGCHK1(w);
 n=AN(w); wv=CAV(w); 
 if(!b){
  c=ctype[(UC)*wv]; v=wv; p=0;
  if     (c==CQ)DQ(n-1, c=ctype[(UC)*++v]; p^=(c==CQ); if(p&(c!=CQ)){b=1; break;})
  else if(c==C9)DQ(n-1, c=ctype[(UC)*++v]; if(!(c==C9   ||c==CS   )){b=1; break;})
  else          DQ(n-1, c=      *++v ; if(!(c==CESC1||c==CESC2)){b=1; break;});
  if(b&&vnm(n,wv))b=0;
 }
 if(b){GATV0(z,LIT,2+n,1); zv=CAV1(z); *zv='('; MC(1+zv,wv,n); zv[1+n]=')';}
 R z;
}

static A jtlcpx(J jt,A w){F1PREFIP;ARGCHK1(w); R parfn(jtinplace,lp(w)>0,w);}

// display a list of boxes as if they come from a gerund.  Normal ()
static F1X(jtltiea){F1PREFIP;A t,*v,*wv,x,y;B b;C c;I n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);  RZ(t=spellout(CGRAVE));
 GATV0(y,BOX,n+n,1); v=AAV1(y);
 DO(n, *v++=i?t:mtv; x=C(wv[i]); c=IDD(x); RZ(x=lrr(x)); 
     b=BETWEENC(c,CHOOK,CFORK)||i&&(lp(x)>0); RZ(*v++=parfn(jtinplace,b,x)););
 R raze(y);
}

// display a list of boxes as if they come from a gerund.  Full ()
static F1X(jtltieb){F1PREFIP;A pt,t,*v,*wv,x,y;B b;C c,*s;I n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);  RZ(t=spellout(CGRAVE)); RZ(pt=over(scc(')'),t));
 GATV0(y,BOX,n+n,1); v=AAV1(y);
 if(1>=n)x=mtv; else{GATV0(x,LIT,n-2,1); s=CAV1(x); DQ(n-2, *s++='(';);}
 DO(n, x=i==1?t:x; x=i>1?pt:x; *v++=x; x=C(wv[i]); c=IDD(x); RZ(x=lrr(x)); 
     b=BETWEENC(c,CHOOK,CFORK)||i&&(lp(x)>0); RZ(*v++=parfn(jtinplace,b,x)););
 R raze(y);
}

// return string for the shape: 's$'
static F1X(jtlsh){F1PREFIP;R apip(thorn1(shape(w)),spellout(CDOLLAR));}

// return something to turn a list into the shape:
static F1X(jtlshape){F1PREFIP;I r,*s;
 ARGCHK1(w);
 r=AR(w); s=AS(w);
 R 2==r&&(1==s[0]||1==s[1]) ? spellout((C)(1==s[1]?CCOMDOT:CLAMIN)) : !r ? mtv :
     1<r ? lsh(w) : 1<AN(w) ? mtv : spellout(CCOMMA);
}

static F1X(jtlchar){F1PREFIP;A y;B b,p=1,r1;C c,d,*u,*v;I j,k,m,n;
 ARGCHK1(w);
 m=AN(ds(CALP)); n=AN(w); j=n-m; r1=1==AR(w); u=v=CAV(w); d=*v;  // m=256, n=string length, j=n-256, r1 set if rank is 1, u=v->string, d=first char
 if(0<=j&&r1&&!memcmpne(v+j,AV(ds(CALP)),m)){
  // string ends with an entire a. sequence
  if(!j)R cstr("a.");  // if that's all there is, use a.
  RZ(y=lchar(1==j?scc(*v):str(j,v)));  // recur on the rest of the string to get its lr
  R lp(y)>0?over(cstr("a.,~"),y):over(y,cstr(",a."));  // use rest,a. or a.,~rest depending on () needs
 }
 if(r1&&m==n&&(y=icap(ne(w,ds(CALP))))&&m>AN(y)){  // if 256-byte string, see where it differs from a.
  if(1==AN(y))RZ(y=head(y));
  R over(over(cstr("a. "),lcpx(lnum(y))),over(cstr("}~"),lchar(from(y,w))));   // use diff indx} a. or the like
 }
 // we will try for quoted string
 j=2; b=7<n||1<n&&1<AR(w);  // j will be # added chars (init 2 for outer quotes); look for repeated chars if 7 chars or rank>1
 DQ(n, c=*v++; j+=c==CQUOTE; b&=c==d; p&=(C)(c-32)<(C)(127-32);); // b=1 if all chars the same; p=1 if all printable; add to j for each quote found
 if(b){n=1; j=MIN(3,j);}  // if all repeated, back to 1 character, which j=2/3 dep whether it is a quote
 if(!p){  // if the string contains a nonprintable, represent it as nums { a.
  k=(UC)d; RZ(y=indexof(ds(CALP),w));
  if(r1&&n<m&&(!k||k==m-n)&&equ(y,apv(n,k,1L)))R over(thorn1(sc(d?-n:n)),cstr("{.a."));
  RZ(y=lnum(y));
  R lp(y)?over(cstr("a.{~"),y):over(y,cstr("{a.")); 
 }
 // out the enquoted string, preceded the the shape if repeated or not a list
 GATV0(y,LIT,n+j,1); v=CAV1(y);
 v[0]=v[n+j-1]=CQUOTE; ++v;
 if(2==j)MC(v,u,n); else DQ(n, *v++=c=*u++; if(c==CQUOTE)*v++=c;);
 R over(b?lsh(w):lshape(w),y);
}    /* non-empty character array */

static F1X(jtlbox){F1PREFIP;A p,*v,*vv,*wv,x,y;B b=0;I n;
 ARGCHK1(w);
 if(equ(ds(CACE),w)&&B01&AT(AAV(w)[0]))R cstr("a:");
 n=AN(w); wv=AAV(w); 
 DO(n, x=C(wv[i]); if(BOX&AT(x)){b=1; break;}); b|=1==n;
 GATV0(y,BOX,n+n-(1^b),1); v=vv=AAV1(y);
 if(b){
  RZ(p=cstr("),(<"));
  DO(n, x=C(wv[i]); *v++=p; RZ(*v++=lnoun(x)););
  RZ(*vv=cstr(1==n?"<":"(<")); if(1<n)RZ(vv[n+n-2]=cstr("),<"));
  R over(lshape(w),raze(y));
 }
 DO(n, x=C(wv[i]); if((AR(x)^1)|(~AT(x)&LIT)){b=1; break;});
 if(!b){C c[256],d,*t;UC*s;
  mvc(sizeof(c),c,1,MEMSET01); 
  RZ(x=raze(w)); s=UAV(x);
  DQ(AN(x), c[*s++]=0;);
  if(c[CQUOTE]&&equ(w,words(x)))R over(cstr(";:"),lchar(x));
  if(c[d=' ']||c[d='|']||c[d='/']||c[d=',']||c[d=';']){
   GATV0(y,LIT,n+AN(x),1); t=CAV1(y);
   DO(n, x=C(wv[i]); *t++=d; MC(t,AV(x),AN(x)); t+=AN(x););
   RZ(y=lchar(y));
   R over(lshape(w),over(cstr(isdigit(CAV(y)[0])?"<;.(_1) ":"<;._1 "),y));
 }}
 RZ(p=cstr(";"));
 DO(n-1, RZ(*v++=lcpx(lnoun(C(wv[i])))); *v++=p;);
 RZ(*v=lnoun(wv[n-1]));
 R over(lshape(w),raze(y));
}    /* non-empty boxed array */

A jtdinsert(J jt,A w,A ic,I ix){A l=sc4(INT,ix); R over(over(take(l,w),ic),drop(l,w));}   /* insert ic before position ix in w */

// Apply decoration as needed to a numeric character string w to give it the correct type t
// We know the string is a generated number, so it doesn't contain suffixes in the middle of the string
// Result is A block for decorated string
A jtdecorate(J jt,A w,I t){
 if(AN(w)==0)R w;  // if empty string, don't decorate
 if(t&FL){
  // float: make sure there is a . somewhere, or infinity/indefinite ('_' followed by space/end/.), else put '.' at end, floated back over exponent if any
  B needdot = !memchr(CAV(w),'.',AN(w));  // check for decimal point
  if(needdot){DO(AN(w), if(CAV(w)[i]=='_' && (i==AN(w)-1 || CAV(w)[i+1]==' ')){needdot=0; break;} )}  // check for infinity
  if(needdot){w=over(w,scc('.')); RZ(w=mkwris(w)); DQ(AN(w) , if(CAV(w)[i]==' ')R w;  if(CAV(w)[i]=='e'){C f='.'; C *s=&CAV(w)[i]; DO(AN(w)-i, C ff=s[i]; s[i]=f; f=ff;)}) }
 }else if(t&INT+INT2+INT4){
 // integer: if the string contains nothing but one-digit 0/1 values, insert '0' before last number
  I l=AN(w); C *s=CAV(w); NOUNROLL do{if((*s&-2)!='0')break; ++s; if(--l==0)break; if(*s!=' ')break; ++s;}while(--l);
  if(l==0){I ls=0; DQ(AN(w), if(CAV(w)[i]==' ') ls=i;); w=ls?jtdinsert(jt,w,scc('0'),ls+1):over(scc('0'),w);}
 }else if(t&XNUM+RAT+CMPX+QP+SP){
  C srch='x'; srch=t&RAT?'r':srch; srch=t&CMPX?'j':srch; srch=t&QP+SP?'f':srch;
  char *rep="x"; rep=t&RAT?"r1":rep; rep=t&CMPX?"j0":rep; rep=t&QP?"fq":rep; rep=t&SP?"fs":rep;
  if(!memchr(CAV(w),srch,AN(w)))w=over(w,cstr(rep));
 }
 R w;
}


static F1X(jtlnum1){F1PREFIP;A z,z0;I t;
 ARGCHK1(w);
 t=AT(w);
 // use full for float values not going to screen; otherwise the default 
 RZ(z=(t&FL+CMPX+QP)&&!((I)jtinplace&JTPRFORSCREEN)?dfv1(z0,w,fit(ds(CTHORN),sc((I)(t&QP?35:18)))):thorn1(w));
 R decorate(z,t);
}    /* dense non-empty numeric vector */

static F1X(jtlnum){F1PREFIP;A b,d,t,*v,y;B p;I n;
 RZ(t=ravel(w));
 n=AN(w);
 if(7<n||1<n&&1<AR(w)){
  // see if we can use a clever encoding
  d=minus(from(num(1),t),b=from(num(0),t));
  p=equ(t,plus(b,tymes(d,IX(n))));
  if(p){
   if(equ(d,num(0)))R over(lsh(w),lnum1(b));
   GAT0(y,BOX,6,1); v=AAV1(y); v[0]=v[1]=v[2]=v[3]=mtv;
   if(p=!(equ(b,sc(n-1))&&equ(d,num(-1)))){
    if     (!equ(b,num(0)   )){v[0]=lnum1(b); v[1]=spellout(CPLUS);}
    if     ( equ(d,num(-1))) v[1]=spellout(CMINUS);
    else if(!equ(d,num(1)    )){v[2]=lnum1(d); v[3]=spellout(CSTAR);}
   }
   v[4]=spellout(CIOTA); v[5]=thorn1(p?shape(w):negate(shape(w)));
   RE(y); R raze(y);
  }
  RESETERRC;   // if there was an error getting to p, clear it - don't clear error text if frozen
 }
 // not clever; just out the atoms
 R over(lshape(w),lnum1(t));
}    /* dense numeric non-empty array */

static F1X(jtlsparse){F1PREFIP;A a,e,q,t,x,y,z;B ba,be,bn;I j,r,*v;P*p;
 ARGCHK1(w);
 r=AR(w); p=PAV(w); a=SPA(p,a); e=SPA(p,e); y=SPA(p,i); x=SPA(p,x);
 bn=0; v=AS(w); DQ(r, if(!*v++){bn=1; break;});
 ba=0; if(r==AR(a)){v=AV(a); DO(r, if(i!=*v++){ba=1; break;});}
 be=!(AT(w)&FL&&0==*DAV(e));
 if(be)RZ(z=over(lnoun(e),cstr(B01&AT(w)?"":INT&AT(w)?"+-~2":FL&AT(w)?"+-~2.1":"+-~2j1")));
 if(be||ba){
  RZ(z=be?over(lcpx(lnoun(a)),       over(scc(';'),z)):lnoun(a));
  RZ(z=   over(lcpx(lnoun(shape(w))),over(scc(';'),z))         );
 }else RZ(z=lnoun(shape(w))); 
 RZ(z=over(cstr("1$."),z));
 if(bn||!AS(y)[0])R z;
 if(AN(a)){
  RZ(x=lcpx(lnoun(x)));
  RZ(y=1==r?lnoun(ravel(y)):over(cstr("(<)"),lnoun(y)));
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

static F1X(jtlnoun0){F1PREFIP;A s,x;B r1;
 ARGCHK1(w);
 r1=1==AR(w); RZ(s=thorn1(shape(w)));
 switch(CTTZ(AT(w))){
 default:    R apip(s,cstr("$00"    ));  // over(cstr("i."),s);
 case LITX:  x=cstr(   "''"); R r1?x:apip(apip(s,scc('$')),x);
 case C2TX:  x=cstr("u: ''"); R r1?x:apip(apip(s,scc('$')),x);
 case C4TX:  x=cstr("10&u: ''"); R r1?x:apip(apip(s,scc('$')),x);
 case BOXX:  R apip(s,cstr("$a:"    ));
 case B01X:  R apip(s,cstr("$0"     ));
 case FLX:   R apip(s,cstr("$0."    ));
 case CMPXX: R apip(s,cstr("$0j0"   ));
 case QPX:   R apip(s,cstr("$0fq"   ));
 case XNUMX: R apip(s,cstr("$0x"    ));
 case RATX:  R apip(s,cstr("$0r0"   ));
 case SBTX:  R apip(s,cstr("$s: ' '"));
 }
}   /* empty dense array */


static F1X(jtlnoun){F1PREFIP;I t;
 ARGCHK1(w);
 t=AT(w);
 if(unlikely(ISSPARSE(t)))R lsparse(w);
 if(!AN(w))R lnoun0(w);
 switch(CTTZ(t)){
 default:  R lnum(w);
 case LITX: R lchar(w);
 case C2TX: R over(cstr("u: "),lnum(uco2(num(3),w)));
 case C4TX: R over(cstr("10&u: "),lnum(uco2(num(3),w)));
 case BOXX: R lbox(w);
 case SBTX: R over(cstr("s: "),lbox(sb2(num(5),w)));
 }
}

static A jtlsymb(J jt,C c,A w,A *ltext){F1PREFIP;A t;C buf[20],d,*s;I*u;V*v=FAV(w);
 {RZ(t=spella(w)); if(AN(t)==1&&(CAV(t)[0]=='{'||CAV(t)[0]=='}')){RZ(t=mkwris(t)); AS(t)[0]=AN(t)=2; CAV(t)[1]=' '; }}  // add trailing space to { } to avoid DD codes
 d=CAV(t)[0];
 R d==CESC1||d==CESC2?over(chrspace,t):t;
}

static B laa(A a,A w){C c,d;
 if(!(a&&w))R 0;
 c=ctype[(UC)CAV(a)[AN(a)-1]]; d=ctype[(UC)CAV(w)[0]];
 R ((c|d)&(0xf&~(CA|C9)))^1;  // 1 if c,d both alphameric
}

// Is a string a number?  Must start with a digit and end with digit, x, or .
static B lnn(A a,A w){C c; if(!(a&&w))R 0; c=CAV(a)[AN(a)-1]; R ('x'==c||'.'==c||C9==ctype[(UC)c])&&C9==ctype[(UC)CAV(w)[0]];}

// ? insert spacing between components of trains
static F2X(jtlinsert){F1PREFIP;A*av,f,g,h,t,t0,t1,t2,*u,y;B b,ft,gt,ht;C c,id;I n;V*v;
 ARGCHK2(a,w);
 n=AN(a); av=AAV(a);  
 v=VAV(w); id=v->id;
 b=id==CCOLONE&&VXOP&v->flag;  // b if operator, which is spaced as if a hook/fork: u body [v]
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=CNULL(v->fgh[fndx]); A gs=CNULL(v->fgh[fndx^1]); A hs=CNULL(v->fgh[2]);  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 if(id==CIBEAM&&!(AT(w)&CONJ)){fs=scib(FAV(w)->localuse.lu1.foreignmn[0]); gs=scib(FAV(w)->localuse.lu1.foreignmn[1]);}  // scb to simplify display
 if(id==CFORK&&hs==0){hs=gs; gs=fs; fs=ds(CCAP);}  // reconstitute capped fork
// ?t tells whether () is needed around the f/g/h component
 if(1<=n){f=C(av[0]); t=fs; c=ID(t); ft=BETWEENC(c,CHOOK,CADVF)||(b||id==CFORK)&&NOUN&AT(t)&&(lp(f)>0);}  // f: () if it's invisible   or   noun left end of nvv or n (op)
 if(2<=n){g=C(av[1]); t=gs; c=ID(t); gt=VERB&AT(w)    ?BETWEENC(c,CHOOK,CADVF):((BETWEENC(id,CHOOK,CADVF))|lp(g))>0;}  // g: paren any invisible modifier
 if(3<=n){h=C(av[2]); t=hs; c=ID(t); ht=VERB&AT(w)&&!b?c==CHOOK:((BETWEENC(id,CHOOK,CADVF)&&!b)|lp(h))>0;}  // h: in verb fork, paren hook; in trident, paren any train
 switch(!(b||BETWEENC(id,CHOOK,CADVF))?id:2==n?CHOOK:CFORK){  // if operator or invisible, ignore the type and space based on length
 case CADVF:
 case CHOOK:
  GAT0(y,BOX,3,1); u=AAV1(y);
  u[0]=f=parfn(jtinplace,ft||lnn(f,g),f);
  u[2]=g=parfn(jtinplace,gt||b,       g);
  u[1]=str(' '==CAV(g)[0]||id==CADVF&&!laa(f,g)&&!((lp(f)>0)&&(lp(g)>0))?0L:1L," ");
  RE(0); R raze(y);
 case CFORK:
  GAT0(y,BOX,5,1); u=AAV1(y);
  RZ(u[0]=f=parfn(jtinplace,ft||lnn(f,g),   f));
  RZ(u[2]=g=parfn(jtinplace,gt||lnn(g,h)||b,g)); RZ(u[1]=str(' '==CAV(g)[0]?0L:1L," "));
  RZ(u[4]=h=parfn(jtinplace,ht,             h)); RZ(u[3]=str(' '==CAV(h)[0]?0L:1L," "));
  R raze(y);
 default:
  t0=parfn(jtinplace,ft||NOUN&AT(fs)&&!(VGERL&v->flag)&&(lp(f)>0),f);
  t1=lsymb(id,w);
  y=over(t0,laa(t0,t1)?over(chrspace,t1):t1);
  if(1==n)R y;
  t2=lcpx(g);
  R over(y,laa(y,t2)?over(chrspace,t2):t2);
 }
}

// create linear rep for m : n
// JT has valence-suppression flags
static F1X(jtlcolon){F1PREFIP;A*v,x,y;C*s,*s0;I m,n;
 RZ(y=jtunparsem(jtinplace,num(1),w));   // extract the valences of w, run together: a list of boxes
 n=AN(y); v=AAV(y); RZ(x=lrr(C(VAV(w)->fgh[0])));  // n=#lines, v->line 0, get x=linear rep for m (string form of a digit)
 if((I)jtinplace&JTPRFORSCREEN && FAV(w)->flag&VISDD){A z;  // defn was {{ }} and we are printing it to screen
  // we can display the defn as a DD.
  C hdr[5]; I hdrl=0; hdr[hdrl++]='{'; hdr[hdrl++]='{'; // install {{
  if(FAV(w)->flag&VDDHASCTL){   // user gave ')?'
    hdr[hdrl++]=')';  // install in hdr
    C pos='v'; pos=FAV(w)->valencefns[0]==jtvalenceerr?'d':pos; pos=FAV(w)->valencefns[1]==jtvalenceerr?'m':pos; // type, if verb
    pos=AT(w)&ADV?'a':pos; pos=AT(w)&CONJ?'c':pos;   // override if not verb
    hdr[hdrl++]=pos; hdr[hdrl++]=CLF;  // install type, giving {{)tLF
  }else hdr[hdrl++]=' ';  // ordinary DD: {{SP
  R raze(over(dfv2(z,box(str(hdrl,hdr)),ravel(stitch(box(scc(CLF)),y)),amend(num(0))),box(str(2,"}}"))));  //  ; ((<hdr) 0} , (<LF) ,. y) ,  <tlr
 }
 if(!((I)jtinplace&JTEXPVALENCEOFF)&&(2>n||2==n&&1==AN(v[0])&&':'==CAV(C(v[0]))[0])){  // if all valences requested, and only one line, or monadic valence empty (i. e. first line is ':')
  // return one-line definition m : 'string'.  m will come from x
  if(!n)R over(x,str(5L," : \'\'"));  // empty string, return m : ''
  y=lrr(C(v[2==n]));   // convert n to string form, with quotes
  if(2==n)y=over(str(5L,"\':\'; "),y);  // If the line was from the dyad, prepend  ':'; (which will get parenthesized) (could be adv/conj)
  R over(over(x,str(3L," : ")),lcpx(y));  // m : 'string'
 }
 // multiline definition (or single valence requested).  Append the body to ltext, return the (m : 0) as the main result
 RZ(y=apip(raze(stitch(box(scc(CLF)),y)),str(2,"\n)")))   // (; ((<LF) ,. y)) , LF,')'
 *ltext=*ltext?over(*ltext,y):y;
 R over(x,str(4L," : 0"));   // result is m : 0
}

// Main routine for () and linear rep.  w is to be represented
static DF1X(jtlrr){F1PREFIP;A hs,t,*tv;C id;I fl,m;V*v;
 ARGCHK1(w);
 // If name, it must be in ".@'name', or (in debug mode) the function name, which we will discard
 if(AT(w)&NAME){RZ(w=sfn(0,w));}
 if(unlikely(AFLAG(w)&AFRO))if(AT(w)&VERB){R str(12,"cocurrent_z_");}  // readonly name, must not expand it.  We don't have access to the name used
 if(AT(w)&NOUN)R lnoun(C(w));
 v=VAV(w); id=v->id;  // outer verb, & its id
 // if f is 0, we take f from g.  In other words, adverbs can put their left arg in either f or g.  u b. uses g so that it can leave f=0 to allow it to function as an ATOMIC2 op
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=CNULL(v->fgh[fndx]); A gs=CNULL(v->fgh[fndx^1]);  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 if(id==CATCO&&AT(w)&VERB&&FAV(gs)->id==CTDOT)R lrr(gs);  // if <@:t. discard the <@:
 if(id==CIBEAM&&!(AT(w)&CONJ)){fs=scib(FAV(w)->localuse.lu1.foreignmn[0]); gs=scib(FAV(w)->localuse.lu1.foreignmn[1]);}  // scb to simplify display
 hs=CNULL(v->fgh[2]); fl=v->flag; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
 if(id==CFORK&&hs==0){hs=gs; gs=fs; fs=ds(CCAP);}  // reconstitute capped fork
 if(fl&VXOPCALL)R lrr(hs);   // pseudo-named entity created during debug of operator.  The defn is in h
 m=(I)!!fs+(I)(gs&&id!=CBOX)+(I)(BETWEENC(id,CFORK,CADVF)&&hs)+(I)(hs&&id==CCOLONE&&VXOP&fl);  // BOX has g for BOXATOP; ignore it; get # nonzero values in f g h
 if(!m)R lsymb(id,w);  // if none, it's a primitive, out it
 if(evoke(w)){RZ(w=sfne(w)); if(FUNC&AT(w))w=lrr(w); R w;}  // keep named verb as a string, UNLESS it is NMDOT, in which case use the (f.'d) verb value
 if(!(VXOP&fl)&&hs&&BOX&AT(hs)&&id==CCOLONE)R jtlcolon(jtinplace,w,ltext);  // x : with boxed h - must be explicit defn, for which we might suppress a valence
 // display of a single valence applies only to an explicit definition.  It wouldn't be a bad idea for others, but that would require inspecting the op to see
 // which valences are active on each side.  For the nonce we display everything
 jtinplace=(J)((I)jtinplace&~JTEXPVALENCEOFF);  // display both valences of compounds
 GATV0(t,BOX,m,1); tv=AAV1(t);
 if(2<m)RZ(tv[2]=incorp(lrr(hs)));   // fill in h if present
 // for top-level of gerund (indicated by self!=0), any noun type could not have come from an AR, so return it as is
 if(1<m)RZ(tv[1]=incorp(fl&VGERR?tiefn(jtinplace,fxeach(gs,(A)&jtfxself[!!self]),ltext):lrr(0&&BETWEENC(id,CFDOT,CFCODOT)?hs:gs)));  // fill in g if present
 if(0<m)RZ(tv[0]=incorp(fl&VGERL?tiefn(jtinplace,fxeach(fs,(A)&jtfxself[!!self]),ltext):lrr(fs)));  // fill in f (always present)
 R linsert(t,w);
}

// Create linear representation of w.  Call lrr, which creates an A for the text plus ltext which is appended to it.
// jt flags in subroutines indicate the handling of adding enclosing () and handling `
// JTEXPVALENCEOFF (bits 2-3) indicate explicit valences that are suppressed
// JTPRFORSCREEN indicates that the result is for the user, not 5!:5
// This routine MUST NOT be called with normal inplacing bits
F1(jtlrep){F1PREFIP;PROLOG(0056);A z;A ltextb=0, *ltext=&ltextb;
 RE(z=jtlrr((J)((I)jtinplace&~(JTNORESETERR|JTPARENS)),w,w,ltext));  // the w for self is just any nonzero to indicate top-level call.  Clear paren flags to start.  Exit if error
 if(*ltext)z=apip(z,*ltext);
 EPILOG(z);
}

// Create paren representation of w.  Call lrr, which creates an A for the text plus jt->ltext which is appended to it.
// jt->lcp and jt->ltie are routines for handling adding enclosing () and handling `
F1(jtprep){PROLOG(0057);A z;A ltextb=0, *ltext=&ltextb;
 RE(z=jtlrr((J)((I)jt|JTPARENS),w,w,ltext));
 if(*ltext)z=apip(z,*ltext);
 EPILOG(z);
}

