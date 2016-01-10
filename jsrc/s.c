/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table                                                            */

#include "j.h"


/* a symbol table aka locale is a type INT vector                          */
/* the length is prime and is one of ptab[i]                               */
/* zero elements mean unused entry                                         */
/* non-zero elements are indices in the global symbol pool and             */
/*     are head pointers to a linked list                                  */
/* the first element is symbol pool index for locale info                  */

/* the global symbol pool is a type INT matrix                             */
/* the number of columns is symcol=ceiling(sizeof(L)/sizeof(I))            */
/* elements are interpreted per type L (see jtype.h)                       */
/*  A name - A name on LHS of assignment or locale name                    */
/*  A val  - value        or locale search path                            */
/*  I sn   - script index                                                  */
/*  I flag - various flags                                                 */
/*  I next - pointer to   successor in linked list                         */
/*  I prev - pointer to predecessor in linked list                         */
/*           if no predecessor then pointer to hash table entry, and       */
/*           flag will include LHEAD                                       */
/* a stack of free entries is kept using the next pointer                  */
/* jt->symp:     symbol pool array                                         */
/* jt->sympv:    symbol pool array pointer, (L*)AV(jt->symp)               */
/* jt->symindex: symbol table index (monotonically increasing)             */

/* numbered locales:                                                       */
/* jt->stnum: -1 means free; others are numbers in use                     */
/* jt->stptr:  0 means free; others are symbol tables                      */
/* jt->stused: # entries in stnum/stptr in use                             */
/* jt->stmax:  1 + maximum number extant                                   */

/* named locales:                                                          */
/* jt->stloc:  locales symbol table                                        */

static I symcol=(sizeof(L)+SZI-1)/SZI;

B jtsymext(J jt,B b){A x,y;I j,m,n,s[2],*v,xn,yn;L*u;
 if(b){y=jt->symp; j=((MS*)y-1)->j; n=*AS(y); yn=AN(y);}
 else {            j=12;            n=1;      yn=0;    }
 m=msize[1+j];                              /* new size in bytes           */
 m-=sizeof(MS)+SZI*(AH+2);                  /* less array overhead         */
 m/=symcol*SZI;                             /* new # rows                  */
 s[0]=m; s[1]=symcol; xn=m*symcol;          /* new pool array shape        */
 GA(x,INT,xn,2,s); v=AV(x);                 /* new pool array              */
 if(b)ICPY(v,AV(y),yn);                     /* copy old data to new array  */
 memset(v+yn,C0,SZI*(xn-yn));               /* 0 unused area for safety    */
 u=n+(L*)v; j=1+n;
 DO(m-n-1, u++->next=j++;);                 /* build free list extension   */
 if(b)u->next=jt->sympv->next;              /* push extension onto stack   */
 ((L*)v)->next=n;                           /* new stack top               */
 jt->symp =ra(x);                           /* preserve new array          */
 jt->sympv=(L*)AV(x);                       /* new array value ptr         */
 if(b)fa(y);                                /* release old array           */
 R 1;
}    /* 0: initialize (no old array); 1: extend old array */

L* jtsymnew(J jt,I*hv){I j;L*u,*v;
 while(!(j=jt->sympv->next))RZ(symext(1));  /* extend pool if req'd        */
 jt->sympv->next=(j+jt->sympv)->next;       /* new top of stack            */
 u=j+jt->sympv;
 if(u->next=*hv){v=*hv+jt->sympv; v->prev=j; v->flag^=LHEAD;}
 u->prev=(I)hv; u->flag=LHEAD;
 *hv=j;
 R u;
}    /* allocate a new pool entry and insert into hash table entry hv */

B jtsymfree(J jt,L*u){I q;
 q=u->next;
 if(q)(q+jt->sympv)->prev=u->prev; 
 if(LHEAD&u->flag){*(I*)u->prev=q; if(q)(q+jt->sympv)->flag|=LHEAD;}
 else (u->prev+jt->sympv)->next=q;
 fa(u->name); u->name=0;                    /* zero out data fields        */
 fa(u->val ); u->val =0; u->sn=u->flag=u->prev=0;
 u->next=jt->sympv->next;                   /* point to old top of stack   */
 jt->sympv->next=u-jt->sympv;               /* new          top of stack   */
 R 1;
}    /* free pool entry pointed to by u */

static SYMWALK(jtsymfreeha, B,B01,100,1, 1, RZ(symfree(d)))   /* free pool table entries      */

B jtsymfreeh(J jt,A w,L*v){I*wv;L*u;
 wv=AV(w);
 ASSERTSYS(*wv,"symfreeh");
 u=*wv+jt->sympv; 
 RZ(symfree(u));
 RZ(symfreeha(w));
 memset(wv,C0,AN(w)*SZI);
 fa(w);
 if(v){v->val=0; RZ(symfree(v));}
 R 1;
}    /* free entire hash table w, (optional) pointed by v */


static SYMWALK(jtsympoola, I,INT,100,1, 1, *zv++=j;)

F1(jtsympool){A aa,*pu,q,x,y,*yv,z,*zv;I i,j,n,*u,*v,*xv;L*pv;
 RZ(w); 
 ASSERT(1==AR(w),EVRANK); 
 ASSERT(!AN(w),EVLENGTH);
 GA(z,BOX,3,1,0); zv=AAV(z);
 n=*AS(jt->symp); pv=jt->sympv;
 GA(x,INT,n*6,2,0); *AS(x)=n; *(1+AS(x))=6; xv= AV(x); zv[0]=x;
 GA(y,BOX,n,  1,0);                         yv=AAV(y); zv[1]=y;
 for(i=0;i<n;++i,++pv){         /* per pool entry       */
  *xv++=i;
  *xv++=(q=pv->val)?AT(pv->val):0;
  *xv++=pv->flag;
  *xv++=pv->sn;
  *xv++=pv->next;
  *xv++=pv->prev;
  RZ(*yv++=(q=pv->name)?sfn(1,q):mtv);
 }
 GA(y,BOX,n,1,0); yv=AAV(y); zv[2]=y;
 DO(n, yv[i]=mtv;);
 n=AN(jt->stloc); v=AV(jt->stloc); 
 for(i=0;i<n;++i)if(j=v[i]){    /* per named locales    */
  x=(j+jt->sympv)->val; 
  RZ(yv[j]=yv[*AV(x)]=aa=sfn(1,LOCNAME(x)));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 n=AN(jt->stptr); pu=AAV(jt->stptr);
 for(i=0;i<n;++i)if(x=pu[i]){   /* per numbered locales */
  RZ(      yv[*AV(x)]=aa=sfn(1,LOCNAME(x)));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 if(x=jt->local){               /* per local table      */
  RZ(      yv[*AV(x)]=aa=cstr("**local**"));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 R z;
}    /* 18!:31 symbol pool */


L*jtprobe(J jt,A a,A g){C*s;I*hv,k,m;L*v;NM*u;
 RZ(a&&g);
 u=NAV(a); m=u->m; s=u->s; k=u->hash%AN(g); hv=AV(g)+(k?k:1);
 if(!*hv)R jt->cursymb=0;                            /* (0) empty slot    */
 v=*hv+jt->sympv;
 while(1){
  u=NAV(v->name);
  if(m==u->m&&!memcmp(s,u->s,m))R jt->cursymb=v;     /* (1) exact match   */
  if(!v->next)R jt->cursymb=0;                       /* (2) link list end */
  v=v->next+jt->sympv;
}}

static L*jtprobeis(J jt,A a,A g){C*s;I*hv,k,m;L*v;NM*u;
 u=NAV(a); m=u->m; s=u->s; k=u->hash%AN(g); hv=AV(g)+(k?k:1);
 if(*hv){                                 /* !*hv means (0) empty slot    */
  v=*hv+jt->sympv;
  while(1){                               
   u=NAV(v->name);
   if(m==u->m&&!memcmp(s,u->s,m))R jt->cursymb=v;    /* (1) exact match   */
   if(!v->next)break;                                /* (2) link list end */
   v=v->next+jt->sympv;
 }}
 RZ(v=symnew(hv)); 
 v->name=ra(a);
 R jt->cursymb=v;
}    /* probe for assignment */

static L*jtsyrd1(J jt,A a,A g,B b){A*v,x,y;L*e;NM*av;
 if(b&&jt->local&&(e=probe(a,jt->local))){av=NAV(a); R av->e=e;}
 RZ(g&&(y=LOCPATH(g)));
 if(e=probe(a,g))R e;
 v=AAV(y); 
 DO(AN(y), x=v[i]; if(e=probe(a,stfind(1,AN(x),CAV(x))))break;);
 R e;
}    /* find name a where the current locale is g */ 

static A jtlocindirect(J jt,I n,C*u){A a,g=jt->global,x,y;B lcl=1;C*s,*v,*xv;I k,xn;L*e;
 s=n+u;
 while(u<s){
  v=s; while('_'!=*--v); ++v;
  k=s-v; s=v-2; RZ(a=nfs(k,v));
  e=syrd1(a,g,lcl); lcl=0; 
  ASSERTN(e,EVVALUE,a);
  y=e->val;  
  ASSERTN(!AR(y),EVRANK,a); 
  ASSERTN(BOX&AT(y),EVDOMAIN,a);
  x=AAV0(y); xn=AN(x); xv=CAV(x); 
  ASSERTN(1>=AR(x),EVRANK,a); 
  ASSERTN(xn,EVLENGTH,a);
  ASSERTN(LIT&AT(x),EVDOMAIN,a); 
  ASSERTN(vlocnm(xn,xv),EVILNAME,a);
  RZ(g=stfind(1,xn,xv));
 }
 R g;
}

L*jtsyrd(J jt,A a,A*symb){A g=jt->global;I m,n;NM*v;
 RZ(a);
 n=AN(a); v=NAV(a); m=v->m;
 if(n>m)RZ(g=NMILOC&v->flag?locindirect(n-m-2,2+m+v->s):stfind(1,n-m-2,1+m+v->s))
 if(symb)*symb=g;
 R syrd1(a,g,(B)(n==m));
}


static A jtdllsymaddr(J jt,A w,C flag){A*wv,x,y,z;I i,n,wd,*zv;L*v;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GA(z,INT,n,AR(w),AS(w)); zv=AV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); v=syrd(nfs(AN(x),CAV(x)),0L); 
  ASSERT(v,EVVALUE);
  y=v->val;
  ASSERT(NOUN&AT(y),EVDOMAIN);
  zv[i]=flag?(I)AV(y):(I)v;
 }
 R z;
}    /* 15!:6 (0=flag) or 15!:14 (1=flag) */

F1(jtdllsymget){R dllsymaddr(w,0);}
F1(jtdllsymdat){R dllsymaddr(w,1);}


F1(jtsymbrd){L*v; RZ(w); ASSERTN(v=syrd(w,0L),EVVALUE,w); R v->val;}

F1(jtsymbrdlock){A y;
 RZ(y=symbrd(w));
 R FUNC&AT(y)&&(jt->glock||VLOCK&VAV(y)->flag)?nameref(w):y;
}

B jtredef(J jt,A w,L*v){A f,oldn;DC c,d;
 d=jt->sitop; while(d&&!(DCCALL==d->dctype&&d->dcj))d=d->dclnk; if(!(d&&DCCALL==d->dctype&&d->dcj))R 1;
 oldn=jt->curname;
 if(v==(L*)d->dcn){
  jt->curname=d->dca; f=d->dcf;
  ASSERT(AT(f)==AT(w)&&(CCOLON==VAV(f)->id)==(CCOLON==VAV(w)->id),EVSTACK);
  d->dcf=w;
  if(CCOLON==VAV(w)->id)jt->redefined=(I)v;
  c=jt->sitop; while(c&&DCCALL!=c->dctype){c->dctype=DCJUNK; c=c->dclnk;}
 }
 c=d; while(c=c->dclnk){jt->curname=c->dca; ASSERT(!(DCCALL==c->dctype&&v==(L*)c->dcn),EVSTACK);}
 jt->curname=oldn;
 R 1;
}    /* check for changes to stack */

A jtsymbis(J jt,A a,A w,A g){A x;I m,n,wn,wr,wt;NM*v;L*e;V*wv;
 RZ(a&&w&&g);
 n=AN(a); v=NAV(a); m=v->m;
 if(n==m)ASSERT(!(jt->local&&g==jt->global&&probe(a,jt->local)),EVDOMAIN)
 else{C*s=1+m+v->s; RZ(g=NMILOC&v->flag?locindirect(n-m-2,1+s):stfind(1,n-m-2,s));}
 RZ(e=probeis(a,g)); 
 if(jt->db)RZ(redef(w,e));
 wt=AT(w);
 if(wt&FUNC&&(wv=VAV(w),wv->f)){if(wv->id==CCOLON)wv->flag|=VNAMED; if(jt->glock)wv->flag|=VLOCK;}
 x=e->val; 
 ASSERT(!(x&&AFRO&AFLAG(x)),EVRO);
 if(!(x&&AFNJA&AFLAG(x))){
  RZ(w=ra(AFNJA&AFLAG(w)?w:rca(w))); 
  nvrredef(x); 
  fa(x); 
  e->val=w;
 }else if(x!=w){  /* replacing mapped data */
  if(wt&BOX)R smmis(x,w);
  wn=AN(w); wr=AR(w); m=wn*bp(wt);
  ASSERT(wt&B01+INT+FL+CMPX+LIT,EVDOMAIN);
  ASSERT(AM(x)>=m,EVALLOC);
  AT(x)=wt; AN(x)=wn; AR(x)=wr; ICPY(AS(x),AS(w),wr); MC(AV(x),AV(w),m);
 }
 e->sn=jt->slisti;
 if(jt->stch&&(m<n||jt->local!=g&&jt->stloc!=g))e->flag|=LCH;
 R mark;
}    /* a: name; w: value; g: symbol table */
