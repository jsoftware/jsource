/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Locales                                                   */

#include "j.h"

// Create symbol table: k is 0 for named, 1 for numbered, 2 for local; ptab[p] is the number of hash entries;
// n is length of name (or locale# to allocate, for numbered locales), u->name
// Result is SYMB type for the symbol table.  For global tables only, ra() has been executed
// on the result and on the name and path
A jtstcreate(J jt,C k,I p,I n,C*u){A g,*pv,x,xx,y;C s[20];I m,*nv;L*v;
 GATV(g,SYMB,ptab[p]+SYMLINFOSIZE,0,0);   // have prime number of hashchains, excluding LINFO
 // Allocate a symbol for the locale info, install in special hashchain 0.  Set flag; set sn to the symindex at time of allocation
 // (it is queried by 18!:31)
 // The allocation clears all the hash chain bases, including the one used for SYMLINFO
 RZ(v=symnew(&AV(g)[SYMLINFO],0)); v->flag|=LINFO; v->sn=jt->symindex++;   // allocate at head of chain
 switch(k){
  case 0:  /* named    locale */
   RZ(x=nfs(n,u));
   // Install name and path.  Path is 'z' except in z locale itself, which has empty path
   ra(x); LOCNAME(g)=x; xx=1==n&&'z'==*u?vec(BOX,0L,0L):zpath; ra(xx); LOCPATH(g) = xx;
   // Assign this name in the locales symbol table to point to the allocated SYMB block
   // This does ra() on g
   symbis(x,g,jt->stloc);
   break;
  case 1:  /* numbered locale */
   ASSERT(0<=jt->stmax,EVLOCALE);
   sprintf(s,FMTI,n); RZ(x=nfs(strlen(s),s));
   ra(x); LOCNAME(g)=x; ra(zpath); LOCPATH(g)=zpath;
   ++jt->stused;
   m=AN(jt->stnum);
   // Extend in-use locales list if needed
   if(m<jt->stused){
    x=ext(1,jt->stnum); y=ext(1,jt->stptr); RZ(x&&y); jt->stnum=x; jt->stptr=y;
    nv=m+AV(jt->stnum); pv=m+AAV(jt->stptr); DO(AN(x)-m, *nv++=-1; *pv++=0;); 
   }
   // Put this locale into the in-use list at an empty location.  ra(g) at that time
   pv=AAV(jt->stptr);
   DO(AN(jt->stnum), if(!pv[i]){ra(g); pv[i]=g; *(i+AV(jt->stnum))=n; break;});
   jt->stmax=n<IMAX?MAX(jt->stmax,1+n):-1;
   break;
  case 2:  /* local symbol table */
   // Local symbol tables use the rank as a flag word.  Initialize it with the value of p
   // that was used to create the table
   AR(g)=p;
   ;
 }
 R g;
}    /* create locale, named (0==k) or numbered (1==k) */

B jtsymbinit(J jt){A q;I n=40;
 jt->locsize[0]=3;  /* default hash table size for named    locales */
 jt->locsize[1]=2;  /* default hash table size for numbered locales */
 RZ(symext(0));     /* initialize symbol pool                       */
 GATV(q,SYMB,ptab[3+PTO]+SYMLINFOSIZE,1,0); jt->stloc=q;  // alloc space, leaving ptab[] hashchains
 RZ(q=apv(n,-1L,0L));    jt->stnum=q;
 GATV(q,INT,n,1,0);        jt->stptr=q; memset(AV(q),C0,n*SZI);
 RZ(jt->global=stcreate(0,5L+PTO,4L,"base"));
 RZ(           stcreate(0,7L+PTO,1L,"z"   ));
 R 1;
}


F1(jtlocsizeq){I*v; ASSERTMTV(w); v=jt->locsize; R v2(v[0],v[1]);}
     /* 9!:38 default locale size query */

F1(jtlocsizes){I p,q,*v;
 RZ(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(2==AN(w),EVLENGTH);
 RZ(w=vi(w)); v=AV(w); p=v[0]; q=v[1];
 ASSERT(0<=p&&0<=q,EVDOMAIN);
 ASSERT(p<nptab-PTO&&q<nptab-PTO,EVLIMIT);
 jt->locsize[0]=p;
 jt->locsize[1]=q;
 R mtm;
}    /* 9!:39 default locale size set */


static A jtstfindnum(J jt,B b,I k){A y;I j;
 RZ(y=indexof(jt->stnum,sc(k))); j=*AV(y); 
 if(j<AN(jt->stnum))R*(j+AAV(jt->stptr)); 
 else if(b){ASSERT(k>=jt->stmax,EVLOCALE); R stcreate(1,jt->locsize[1]+PTO,k,0L);}
 else R 0;
}    /* stfind for numbered locales */

A jtstfind(J jt,B b,I n,C*u){I old;L*v;
 if(!n){n=4; u="base";}
 if('9'>=*u)R stfindnum(b,strtoI(u,NULL,10));
 else{
  old=jt->tnextpushx; v=probe(nfs(n,u),jt->stloc); tpop(old);
  R v?v->val:b?stcreate(0,jt->locsize[0]+PTO,n,u):0;
}}   /* find the symbol table for locale u, create if b and non-existent */

static A jtvlocnl(J jt,B b,A w){A*wv,y;C*s;I i,m,n,wd;
 RZ(w);
 n=AN(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 for(i=0;i<n;++i){
  y=WVR(i); m=AN(y); s=CAV(y);
  ASSERT(1>=AR(y),EVRANK);
  ASSERT(m,EVLENGTH);
  ASSERT(LIT&AT(y),EVDOMAIN);
  if(b)ASSERTN(vlocnm(m,s),EVILNAME,nfs(m,s));
 }
 R w;
}    /* validate namelist of locale names */

static I jtprobenum(J jt,C*u){I j; 
 RE(j=i0(indexof(jt->stnum,sc((I)strtoI(u,NULL,(I)10))))); 
 R j<AN(jt->stnum)?j:-1;
}    /* probe for numbered locales */


F1(jtlocnc){A*wv,y,z;C c,*u;I i,m,n,wd,*zv;
 RZ(vlocnl(0,w));
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
 for(i=0;i<n;++i){
  y=WVR(i); m=AN(y); u=CAV(y); c=*u; 
  if(!vlocnm(m,u))zv[i]=-2;
  else if(c<='9') zv[i]=0<=probenum(u)?1:-1;
  else            zv[i]=probe(nfs(m,u),jt->stloc)?0:-1;
 }
 R z;
}    /* 18!:0 locale name class */

static F1(jtlocnlx){A*pv,y,*yv,z;B*wv;C s[20];I m=0,n=0,*nv;
 RZ(w=cvt(B01,w)); wv=BAV(w); DO(AN(w), m|=1+wv[i];);
 if(1&m)z=nlsym(jt->stloc);
 if(2&m){
  GATV(y,BOX,jt->stused,1,0); yv=AAV(y); pv=AAV(jt->stptr); nv=AV(jt->stnum);
  DO(AN(jt->stptr), if(pv[i]){sprintf(s,FMTI,nv[i]); 
      if(jt->nla[*s]){RZ(yv[n++]=cstr(s)); if(n==jt->stused)break;}});
  y=take(sc(n),y);
 }
 z=0==m?mtv:1==m?z:2==m?y:over(y,z);
 R grade2(z,ope(z));
}

F1(jtlocnl1){memset(jt->nla,C1,256); R locnlx(w);}
    /* 18!:1 locale name list */

F2(jtlocnl2){UC*u;
 RZ(a&&w);
 ASSERT(LIT&AT(a),EVDOMAIN);
 memset(jt->nla,C0,256); 
 u=UAV(a); DO(AN(a),jt->nla[*u++]=1;);
 R locnlx(w); 
}    /* 18!:1 locale name list */

static A jtlocale(J jt,B b,A w){A g,*wv,y;I wd;
 RZ(vlocnl(1,w));
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(AN(w), y=WVR(i); RZ(g=stfind(b,AN(y),CAV(y))););
 R g;
}    /* last locale (symbol table) from boxed locale names */

F1(jtlocpath1){A g; F1RANK(0,jtlocpath1,0); RZ(g=locale(1,w)); R LOCPATH(g);}
     /* 18!:2  query locale path */

F2(jtlocpath2){A g,x;
 F2RANK(1,0,jtlocpath2,0);
 RZ(  locale(1,a)); RZ(x=every(ravel(a),0L,jtravel));
 RZ(g=locale(1,w));
 fa(LOCPATH(g)); ra(x); LOCPATH(g)=x;
 R mtm;
}    /* 18!:2  set locale path */


static F2(jtloccre){A g,y;C*s;I n,p,*u;L*v;
 RZ(a&&w);
 if(MARK&AT(a))p=PTO+jt->locsize[0]; else{RE(p=PTO+i0(a)); ASSERT(PTO<=p,EVDOMAIN); ASSERT(p<nptab,EVLIMIT);}
 y=AAV0(w); n=AN(y); s=CAV(y);
 if(v=probe(nfs(n,s),jt->stloc)){
  g=v->val; 
  u=1+AV(g); DO(AN(g)-1, ASSERT(!u[i],EVLOCALE););
  RZ(symfreeh(g,v));
 } 
 RZ(stcreate(0,p,n,s));
 R box(ca(y));
}    /* create a locale named w with hash table size a */

static F1(jtloccrenum){C s[20];I k=jt->stmax,p;
 RZ(w);
 if(MARK&AT(w))p=PTO+jt->locsize[1]; else{RE(p=PTO+i0(w)); ASSERT(PTO<=p,EVDOMAIN); ASSERT(p<nptab,EVLIMIT);}
 RZ(stcreate(1,p,k,0L));
 sprintf(s,FMTI,k); 
 R box(cstr(s));
}    /* create a numbered locale with hash table size n */

F1(jtloccre1){
 RZ(w);
 if(AN(w))R rank2ex(mark,vlocnl(1,w),0L,0L,0L,jtloccre);
 ASSERT(1==AR(w),EVRANK);
 R loccrenum(mark);
}    /* 18!:3  create locale */

F2(jtloccre2){
 RZ(a&&w);
 if(AN(w))R rank2ex(a,vlocnl(1,w),0L,0L,0L,jtloccre);
 ASSERT(1==AR(w),EVRANK);
 R rank1ex(a,0L,0L,jtloccrenum);
}    /* 18!:3  create locale with specified hash table size */


F1(jtlocswitch){A g; 
 ASSERT(!AR(w),EVRANK); 
 RZ(g=locale(1,w)); 
 jt->global=g; jt->stswitched=1;
 R mtm;
}    /* 18!:4  switch locale */

F1(jtlocname){A g=jt->global;
 ASSERTMTV(w);
 ASSERT(g,EVLOCALE);
 R box(sfn(0,LOCNAME(g)));
}    /* 18!:5  current locale name */

static SYMWALK(jtlocmap1,I,INT,18,3,1,
    {I t=AT(d->val);
     *zv++=i; 
     *zv++=t&NOUN?0:t&VERB?3:t&ADV?1:t&CONJ?2:(t==SYMB)?6:-2;
     *zv++=(I)sfn(1,d->name);})

F1(jtlocmap){A g,q,x,y,*yv,z,*zv;I c=-1,d,j=0,m,*qv,*xv;
 RZ(w);
 ASSERT(!AR(w),EVRANK);
 RE(g=equ(w,zero)?jt->stloc:equ(w,one)?jt->local:locale(0,w));
 ASSERT(g,EVLOCALE);
 RZ(q=locmap1(g)); qv=AV(q);
 m=*AS(q);
 GATV(x,INT,m*3,2,AS(q)); xv= AV(x);
 GATV(y,BOX,m,  1,0    ); yv=AAV(y);
 DO(m, *xv++=d=*qv++; *xv++=j=c==d?1+j:0; *xv++=*qv++; c=d; *yv++=(A)*qv++;);
 GAT(z,BOX,2,1,0); zv=AAV(z); zv[0]=x; zv[1]=y;
 R z;
}    /* 18!:30 locale map */

static SYMWALK(jtredefg,B,B01,100,1,1,RZ(redef(mark,d)))
     /* check for redefinition (erasure) of entire symbol table */

F1(jtlocexmark){A g,*pv,*wv,y,z;B b,c,*zv;C*u;I i,j,m,n,*nv,wd;L*v;
 RZ(vlocnl(1,w));
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 nv=AV(jt->stnum); pv=AAV(jt->stptr);
 GATV(z,B01,n,AR(w),AS(w)); zv=BAV(z);
 for(i=0;i<n;++i){
  zv[i]=1; y=WVR(i); g=0; m=AN(y); u=CAV(y); b='9'>=*u;
  if(b){j=probenum(u);               if(0<=j)g=pv[j]; }
  else {v=probe(nfs(m,u),jt->stloc); if(v   )g=v->val;}
  if(g){
   c=1;
   DO(1+jt->fcalli, if(g==jt->fcallg[i].g){jt->fcallg[i].flag=1+b; jt->fcallg[i].ptr=b?j:(I)v; c=0; break;});
   if(c){
    if(b){RZ(redefg(g)); RZ(symfreeh(g,0L)); pv[j]=0; nv[j]=-1; --jt->stused;}
    else {RZ(redefg(g)); RZ(symfreeh(g,v ));}
    if(g==jt->global)jt->global=0;
 }}}
 R z;
}    /* 18!:55 destroy a locale (but only mark for destruction if on stack) */

B jtlocdestroy(J jt,I i){A g,*pv;B b;I j,*nv;L*v;
 nv=AV(jt->stnum); pv=AAV(jt->stptr);
 g=jt->fcallg[i].g; b=1==jt->fcallg[i].flag?0:1;
 if(b){j=(I )jt->fcallg[i].ptr; RZ(redefg(g)); RZ(symfreeh(g,0L)); pv[j]=0; nv[j]=-1; --jt->stused;}
 else {v=(L*)jt->fcallg[i].ptr; RZ(redefg(g)); RZ(symfreeh(g,v ));}
 if(g==jt->global)jt->global=0;
 R 1;
}    /* destroy locale jt->callg[i] (marked earlier by 18!:55) */
