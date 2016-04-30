/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Explicit Definition : and Associates                      */

/* Usage of the f,g,h fields of : defined verbs:                           */
/*  f  character matrix of  left argument to :                             */
/*  g  character matrix of right argument to :                             */
/*  h  4-element vector of boxes                                           */
/*       0  vector of boxed tokens for f                                   */
/*       1  vector of triples of control information                       */
/*       2  vector of boxed tokens for g                                   */
/*       3  vector of triples of control information                       */

#include "j.h"
#include "d.h"
#include "p.h"
#include "w.h"

#define BASSERT(b,e)   {if(!(b)){jsignal(e); i=-1; z=0; continue;}}
#define BGA(v,t,n,r,s) BZ(v=ga(t,(I)(n),(I)(r),(I*)(s)))
#define BZ(e)          if(!(e)){i=-1; z=0; continue;}

#define LINE(sv)       {A x; \
                        h=sv->h; hv=AAV(sv->h); hi=a&&w?HN:0; \
                        line=AAV(hv[hi]); x=hv[1+hi]; n=AN(x); cw=(CW*)AV(x);}

typedef struct{A t,x,line;C*iv,*xv;I j,k,n;} CDATA;
/* for_xyz. t do. control data   */
/* line  'for_xyz.'              */
/* t     iteration array         */
/* n     #t                      */
/* k     length of name xyz      */
/* x     text xyz_index          */
/* xv    ptr to text xyz_index   */
/* iv    ptr to text xyz         */
/* j     iteration index         */

#define WCD            (sizeof(CDATA)/sizeof(I))

typedef struct{I d,t,e;} TD;
#define WTD            (sizeof(TD)/sizeof(I))
#define NTD            17     /* maximum nesting for try/catch */


static B jtforinit(J jt,CDATA*cv,A t){A x;C*s,*v;I k;
 ASSERT(t,EVCTRL);
 cv->t=ra(t);                            /* iteration array     */
 cv->n=IC(t);                            /* # of items in t     */
 cv->j=-1;                               /* iteration index     */
 cv->x=0;
 cv->k=k=AN(cv->line)-5;                 /* length of item name */
 if(0<k&&cv->n){                         /* for_xyz.            */
  s=4+CAV(cv->line); RZ(cv->x=x=ra(str(6+k,s))); 
  cv->xv=v=CAV(x); MC(k+v,"_index",6L);  /* index name          */
  cv->iv=s;                              /* item name           */
 }
 R 1;
}    /* for. do. end. initializations */

static B jtunstackcv(J jt,CDATA*cv){
 if(cv->x){ex(link(cv->x,str(cv->k,cv->iv))); fa(cv->x);}
 fa(cv->t); 
 memset(cv,C0,WCD*SZI); 
 R 1;
}

static void jttryinit(J jt,TD*v,I i,CW*cw){I j=i,t=0;
 v->d=v->t=0;
 while(t!=CEND){
  j=(j+cw)->go;
  switch(t=(j+cw)->type){
   case CCATCHD: v->d=j; break;
   case CCATCHT: v->t=j; break;
   case CEND:    v->e=j; break;
}}}  /* processing on hitting try. */

static DF2(jtxdefn){PROLOG;A cd,cl,cn,h,*hv,*line,loc=jt->local,t,td,u,v,z;B b,lk,named,ox=jt->xdefn;CDATA*cv;
  CW*ci,*cw;DC d=0;I hi,i=0,j,m,n,od=jt->db,old,r=0,st,tdi=0;TD*tdv;V*sv;X y;
 RE(0);
 z=mtm; cd=t=u=v=0; sv=VAV(self); st=AT(self);
 lk=jt->glock||VLOCK&sv->flag; named=VNAMED&sv->flag?1:0; cn=jt->curname; cl=jt->curlocn;
 d=named&&jt->db&&DCCALL==jt->sitop->dctype?jt->sitop:0; /* stack entry for dbunquote for this fn */
 if(VXOP&sv->flag){u=sv->f; v=sv->h; sv=VAV(sv->g);}
 if(st&ADV+CONJ){u=a; v=w;}
 LINE(sv); ASSERT(n,EVDOMAIN);
 RZ(jt->local=stcreate(2,1L,0L,0L));
 if(sv->flag&VTRY1+VTRY2){GA(td,INT,NTD*WTD,2,0); *AS(td)=NTD; *(1+AS(td))=WTD; tdv=(TD*)AV(td);}
 FDEPINC(1);   // do not use error exit after this point; use BASSERT, BGA, BZ
 jt->xdefn=1;
 IS(xnam,a); if(u){IS(unam,u); if(NOUN&AT(u))IS(mnam,u);}
 IS(ynam,w); if(v){IS(vnam,v); if(NOUN&AT(v))IS(nnam,v);}
 if(jt->dotnames){
  IS(xdot,a); if(u){IS(udot,u); if(NOUN&AT(u))IS(mdot,u);}
  IS(ydot,w); if(v){IS(vdot,v); if(NOUN&AT(v))IS(ndot,v);}
 }
 if(jt->db&&jt->sitop&&DCCALL==jt->sitop->dctype&&self==jt->sitop->dcf){
  jt->sitop->dcloc=jt->local; jt->sitop->dcc=hv[1+hi]; jt->sitop->dci=(I)&i;
 }
 old=jt->tbase+jt->ttop; 
 while(0<=i&&i<n){
  if(0<jt->pmctr&&C1==jt->pmrec&&named)pmrecord(cn,cl,i,a?VAL2:VAL1);
  if(jt->redefined&&jt->sitop&&jt->redefined==jt->sitop->dcn&&DCCALL==jt->sitop->dctype&&self!=jt->sitop->dcf){
   self=jt->sitop->dcf; sv=VAV(self); LINE(sv); jt->sitop->dcc=hv[1+hi];
   jt->redefined=0;
   if(i>=n)break;
  }
  ci=i+cw;
  switch(ci->type){
   case CTRY:
    BASSERT(tdi<NTD,EVLIMIT);
    tryinit(tdv+tdi,i,cw);
    if(jt->db)jt->db=(tdv+tdi)->d?jt->dbuser:DBTRY;
    ++tdi; ++i; 
    break;
   case CCATCH: case CCATCHD: case CCATCHT:
    if(tdi){--tdi; i=1+(tdv+tdi)->e; jt->db=od;}else i=ci->go; break;
   case CTHROW:
    BASSERT(0,EWTHROW);
   case CBBLOCK:
    tpop(old); z=parsex(vec(BOX,ci->n,line+ci->i),lk,ci,d);
    if(z||DB1==jt->db||DBERRCAP==jt->db||!jt->jerr)++i;
    else if(EWTHROW==jt->jerr){if(tdi&&(j=(tdv+tdi-1)->t)){i=1+j; RESETERR;}else BASSERT(0,EWTHROW);}
    else{i=ci->go; if(i<SMAX){RESETERR; if(tdi){--tdi; jt->db=od;}}}
    break;
   case CASSERT:
    if(!jt->assert){++i; break;}
   case CTBLOCK: 
    gc(z,old); t=parsex(vec(BOX,ci->n,line+ci->i),lk,ci,d);
    if(t||DB1==jt->db||DBERRCAP==jt->db||!jt->jerr)++i;
    else if(EWTHROW==jt->jerr){if(tdi&&(j=(tdv+tdi-1)->t)){i=1+j; RESETERR;}else BASSERT(0,EWTHROW);}
    else{i=ci->go; if(i<SMAX){RESETERR; if(tdi){--tdi; jt->db=od;}}}
    break;
   case CFOR:
   case CSELECT:
    if(!r)
     if(cd){m=AN(cd)/WCD; BZ(cd=ext(1,cd)); cv=(CDATA*)AV(cd)+m-1; r=AN(cd)/WCD-m;}
     else  {r=9; BGA(cd,INT,r*WCD,1,0); cv=(CDATA*)AV(cd)-1; ra(cd);}
    ++cv; --r; 
    cv->t=cv->x=0; cv->line=line[ci->i]; ++i;
    break;
   case CDOF:
    if(!cv->t){BZ(forinit(cv,t)); t=0;}
    ++cv->j;
    if(cv->j<cv->n){
     if(cv->x){A x;
      symbis(nfs(6+cv->k,cv->xv),x=sc(cv->j),  jt->local);
      symbis(nfs(  cv->k,cv->iv),from(x,cv->t),jt->local);
     }
     ++i; continue;
    }
   case CBREAKF:
   case CENDSEL:
    rat(z); unstackcv(cv); --cv; ++r; 
    i=ci->go; 
    break;
   case CRETURN:
    if(cd){rat(z); DO(AN(cd)/WCD-r, unstackcv(cv); --cv; ++r;);}
    i=ci->go;
    break;
   case CCASE:
   case CFCASE:
    if(!cv->t){BASSERT(t&&NOUN&AT(t),EVCTRL); BZ(cv->t=ra(boxopen(t))); t=0;}
    i=ci->go;
    break;
   case CDOSEL:
    BASSERT(!t||NOUN&AT(t),EVCTRL);
    i=t&&all0(eps(cv->t,boxopen(t)))?ci->go:1+i; 
    t=0; 
    break;
   case CDO:
    ++i; b=1;
    if(t){
     if(SPARSE&AT(t))BZ(t=denseit(t));
     BASSERT(NOUN&AT(t),EVCTRL);
     switch(AN(t)?AT(t):0){
      case RAT:
      case XNUM: y=*XAV(t); b=*AV(y)||1<AN(y); break;
      case CMPX: b=0!=*DAV(t)||0!=*(1+DAV(t)); break; 
      case FL:   b=0!=*DAV(t);                 break;
      case INT:  b=0!=*AV(t);                  break;
      case B01:  b=*BAV(t);
    }}
    t=0;
    if(b)break;
   default:
    JBREAK0;
    i=ci->go;
 }}
 FDEPDEC(1);  // OK to ASSERT now
 z=jt->jerr?0:z?ra(z):mtm;
 fa(cd);
 symfreeh(jt->local,0L); jt->local=loc; jt->asgn=0; jt->xdefn=ox;
 tpop(_ttop);
 if(z)ASSERT(st&ADV+CONJ||AT(z)&NOUN,EVSYNTAX);
 tpush(z);
 R z;
}


static DF1(xv1){R df1(  w,VAV(self)->f);}
static DF2(xv2){R df2(a,w,VAV(self)->g);}

static DF1(xn1 ){R xdefn(0L,w, self);}
static DF1(xadv){R xdefn(w, 0L,self);}


static F1(jtxopcall){R jt->db&&DCCALL==jt->sitop->dctype?jt->sitop->dca:mark;}

static DF1(xop1){A ff,x;
 RZ(ff=fdef(CCOLON,VERB, xn1,jtxdefn, w,self,0L, VXOP|VAV(self)->flag, RMAX,RMAX,RMAX));
 RZ(x=xopcall(one));
 R x==mark?ff:namerefop(x,ff);
}

static DF2(xop2){A ff,x;
 RZ(ff=fdef(CCOLON,VERB, xn1,jtxdefn, a,self,w,  VXOP|VAV(self)->flag, RMAX,RMAX,RMAX));
 RZ(x=xopcall(one));
 R x==mark?ff:namerefop(x,ff);
}


static B jtxop(J jt,A h){A p,x,y;B b,*pv;I*xv;
 GA(x,INT,jt->dotnames?12:6,1,0); xv=AV(x);
                  xv[0]=(I)mnam; xv[1]=(I)nnam; xv[2]=(I)unam; xv[3]=(I)vnam; xv[ 4]=(I)xnam; xv[ 5]=(I)ynam; 
 if(jt->dotnames){xv[6]=(I)mdot; xv[7]=(I)ndot; xv[8]=(I)udot; xv[9]=(I)vdot; xv[10]=(I)xdot; xv[11]=(I)ydot;}
 RZ(y=raze(from(v2(0L,HN),h))); AT(y)=INT;
 RZ(p=eps(x,y)); pv=BAV(p);
 b=(pv[0]||pv[1]||pv[2]||pv[3])&&(pv[4]||pv[5]);
 if(!b&&jt->dotnames)b=(pv[6]||pv[7]||pv[8]||pv[9])&&(pv[10]||pv[11]);
 R b;
}    /* whether h denotes an explicit derived function */

static F1(jtcolon0){A l,z;C*p,*q,*s;I m,n;
 n=0; RZ(z=exta(LIT,1L,1L,300L)); s=CAV(z);
 while(1){
  RE(l=jgets("\001"));
  if(!l)break;
  m=AN(l); p=q=CAV(l); 
  if(m){while(' '==*p)++p; if(')'==*p){while(' '==*++p); if(p>=m+q)break;}}
  while(AN(z)<=n+m){RZ(z=ext(0,z)); s=CAV(z);}
  MC(s+n,q,m); n+=m; *(s+n)=CLF; ++n;
 }
 R str(n,s);
}    /* enter nl terminated lines; ) on a line by itself to exit */

static F1(jtlineit){
 R 1<AR(w)?ravel(stitch(w,scc(CLF))):AN(w)&&CLF==cl(w)?w:over(w,scc(CLF));
}

static B jtsent12c(J jt,A w,A*m,A*d){C*p,*q,*r,*s,*x;
 ASSERT(!AN(w)||LIT&AT(w),EVDOMAIN);
 ASSERT(2>=AR(w),EVRANK);
 RZ(w=lineit(w));
 x=p=r=CAV(w);  /* p: monad start; r: dyad start */
 q=s=p+AN(w);   /* q: monad end;   s: dyad end   */
 while(x<s){
  q=x;
  while(' '==*x)++x; if(':'==*x){while(' '==*++x); if(CLF==*x){r=++x; break;}}
  while(CLF!=*x++);
 }
 if(x==s)q=r=s;
 *m=df1(str(q-p,p),cut(ds(CBOX),num[-2]));
 *d=df1(str(s-r,r),cut(ds(CBOX),num[-2]));
 R 1;
}    /* literal fret-terminated or matrix sentences into monad/dyad */

static B jtsent12b(J jt,A w,A*m,A*d){A t,*wv,y,*yv;I j,*v,wd;
 ASSERT(1>=AR(w),EVRANK);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 GA(y,BOX,AN(w),AR(w),AS(w)); yv=AAV(y);
 DO(AN(w), RZ(yv[i]=vs(WVR(i))););
 RZ(t=indexof(y,link(chr[':'],str(1L,":")))); v=AV(t); j=MIN(*v,*(1+v));
 *m=take(sc(j  ),y); 
 *d=drop(sc(j+1),y);
 R 1;
}    /* boxed sentences into monad/dyad */

F2(jtcolon){A d,h,*hv,m;B b;C*s;I flag=0,n,p;
 RZ(a&&w);
 if(VERB&AT(a)&&VERB&AT(w)){V*v;
  v=VAV(a); if(CCOLON==v->id&&VERB&AT(v->f)&&VERB&AT(v->g))a=v->f;
  v=VAV(w); if(CCOLON==v->id&&VERB&AT(v->f)&&VERB&AT(v->g))w=v->g;
  R fdef(CCOLON,VERB,xv1,xv2,a,w,0L,0L,mr(a),lr(w),rr(w));
 }
 RE(n=i0(a));
 if(equ(w,zero)){RZ(w=colon0(mark)); if(!n)R w;}
 if(C2T&AT(w))RZ(w=cvt(LIT,w));
 if(10<n){s=CAV(w); p=AN(w); if(p&&CLF==s[p-1])RZ(w=str(p-1,s));}
 else{
  RZ(BOX&AT(w)?sent12b(w,&m,&d):sent12c(w,&m,&d));
  if(4==n){if(AN(m)&&!AN(d))d=m; m=mtv;}
  GA(h,BOX,2*HN,1,0); hv=AAV(h);
  RE(b=preparse(m,hv,   hv+1   )); if(b)flag|=VTRY1; hv[2   ]=jt->retcomm?m:mtv;
  RE(b=preparse(d,hv+HN,hv+HN+1)); if(b)flag|=VTRY2; hv[2+HN]=jt->retcomm?d:mtv;
 }
 if(!n)R ca(w);
 if(2>=n){
  RE(b=xop(h)); 
  if(b)flag|=VXOPR; 
  else if(2==n&&AN(m)&&!AN(d)){A*u=hv,*v=hv+HN,x; DO(HN, x=*u; *u++=*v; *v++=x;);}
 }
 flag|=VFIX;  // ensures that f. will not look inside n : n
 switch(n){
  case 1:  R fdef(CCOLON, ADV,  b?xop1:xadv,0L,    num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 2:  R fdef(CCOLON, CONJ, 0L,b?xop2:jtxdefn, num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 3:  R fdef(CCOLON, VERB, xn1,jtxdefn,       num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 4:  R fdef(CCOLON, VERB, xn1,jtxdefn,       num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 13: R vtrans(w);
  default: ASSERT(0,EVDOMAIN);
}}
