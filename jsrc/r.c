/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Atomic, Boxed, and 5!:0                                */

#include "j.h"
#include "w.h"


static F1(jtdrr){PROLOG(0055);A df,dg,hs,*x,z;B b,ex,xop;C c,id;I fl,*hv,m;V*v;
 RZ(w);
 // If the input is a name, it must be from ".@'name' which turned into ".@(name+noun)  - or in debug, but that's discarded
 if(AT(w)&NAME){RZ(w=sfn(0,w));}
 // If noun, return the value of the noun.
 if(AT(w)&NOUN)R w;  // no quotes needed
 // Non-nouns and NMDOT names carry on
 v=FAV(w); id=v->id; fl=v->flag;
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=v->fgh[fndx]; A gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 hs=v->fgh[2]; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
 if(fl&VXOPCALL)R drr(hs);
 xop=1&&VXOP&fl; ex=id==CCOLON&&hs&&!xop;
 b=id==CHOOK||id==CADVF; c=id==CFORK;
 m=!!fs+(gs||ex);
 if(!m)R spella(w);
 if(evoke(w))R drr(sfne(w));  // turn nameref into string or verb; then take rep
 if(fs)RZ(df=fl&VGERL?every(fxeach(fs),0L,jtdrr):drr(fs));
 if(gs)RZ(dg=fl&VGERR?every(fxeach(gs),0L,jtdrr):drr(gs));
 if(ex)RZ(dg=unparsem(num[0],w));
 m+=!b&&!xop||hs&&xop;
 GATV0(z,BOX,m,1); x=AAV(z);
 RZ(x[0]=rifvs(df));
 RZ(x[1]=rifvs(b||c||xop?dg:fl&VDDOP?(hv=AV(hs),link(sc(hv[0]),link(spellout(id),sc(hv[1])))):spellout(id)));
 if(2<m)RZ(x[2]=rifvs(c||xop?drr(hs):dg));
 EPILOG(z);
}

F1(jtdrep){A z=drr(w); R z&&AT(z)&BOX?z:ravel(box(z));}


F1(jtaro){A fs,gs,hs,s,*u,*x,y,z;B ex,xop;C id;I*hv,m;V*v;
 RZ(w);
 if(FUNC&AT(w)){
  v=FAV(w); id=v->id;
  I fndx=(id==CBDOT)&&!v->fgh[0]; fs=v->fgh[fndx]; gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
  hs=v->fgh[2]; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
  if(VXOPCALL&v->flag)R aro(hs);
  xop=1&&VXOP&v->flag;
  ex=hs&&id==CCOLON&&!xop;
  m=id==CFORK?3:!!fs+(ex||xop&&hs||!xop&&gs);
  if(!m)R spella(w);
  if(evoke(w)){RZ(w=sfne(w)); if(FUNC&AT(w))w=aro(w); R w;}  // keep nameref as a string, UNLESS it is NMDOT, in which case use the (f.'d) verb value
 }
 GAT0(z,BOX,2,1); x=AAV(z);
 if(NOUN&AT(w)){RZ(x[0]=rifvs(ravel(scc(CNOUN)))); if(AT(w)&NAME)RZ(w=sfn(0,w)); x[1]=INCORPNA(w); RETF(z);}  // if name, must be ".@'name', format name as string
 GATV0(y,BOX,m,1); u=AAV(y);
 if(0<m)RZ(u[0]=rifvs(aro(fs)));
 if(1<m)RZ(u[1]=rifvs(aro(ex?unparsem(num[0],w):xop?hs:gs)));
 if(2<m)RZ(u[2]=rifvs(aro(hs)));
 s=xop?aro(gs):VDDOP&v->flag?(hv=AV(hs),aro(foreign(sc(hv[0]),sc(hv[1])))):spellout(id);
 RZ(x[0]=rifvs(s)); x[1]=INCORPNA(y);
 R z;
}

F1(jtarep){R box(aro(w));}

// Create A for a string - name~, a primitive, or the boxed string
static DF1(jtfxchar){A y;C c,d,id,*s;I m,n;
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);  // string must be an atom or list
 ASSERT(n,EVLENGTH);
 s=CAV(w); c=*(s+n-1);
 DO(n, d=s[i]; ASSERT((C)(d-32)<(C)(127-32),EVSPELL););  // must be all ASCII
 if(CA==ctype[(UC)*s]&&c!=CESC1&&c!=CESC2)R swap(w);  // If name and not control word, treat as name~, create nameref
 ASSERT(id=spellin(n,s),EVSPELL);  // not name, must be control word or primitive.  Also classify string 
 if(id!=CFCONS)y=ds(id); else{m=s[n-2]-'0'; y=FCONS(CSIGN!=*s?scib(m):2==n?ainf:scib(-m));} // define 0:, if it's that, using boolean for 0/1
 ASSERT(y&&RHS&AT(y),EVDOMAIN);   // make sure it's a noun/verb/adv/conj
 if(!self || AT(y)&NOUN+VERB)R y;  // return any NV, or AC as well if it's not the top level
 R box(w);  // If top level, we have to make sure (<,'&')`  doesn't replace the left part with bare &
}

// Convert an AR to an A block.  w is a gerund that has been opened
// self is normally 0; if nonzero, we return a noun type ('0';<value) as is rather than returning value, and leave adv/conj ARs looking like nouns
DF1(jtfx){A f,fs,g,h,p,q,*wv,y,*yv;C id;I m,n=0;
 RZ(w);
 // if string, handle that special case (verb/primitive)
 if(LIT&AT(w))R fxchar(w,self);
 // otherwise, it had better be boxed with rank 0 or 1, and 1 or 2 atoms
 m=AN(w);   // m=#atoms
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(1==m||2==m,EVLENGTH);
 wv=AAV(w); y=wv[0];  // set wv->box pointers, y->first box
 // If the first box contains boxes, they are ARs - go expand them and save as fs
 // id will contains the type of the AR: 0=another AR, '0'=noun
 if(BOX&AT(y)){RZ(fs=fx(y)); id=0;}
 else{RZ(y=vs(y)); ASSERT(id=spellin(AN(y),CAV(y)),EVSPELL);}
 if(1<m){
  y=wv[1]; n=AN(y); yv=AAV(y); 
  if(id==CNOUN)R self?box(w):y;
  ASSERT(1>=AR(y),EVRANK);
  ASSERT(BOX&AT(y),EVDOMAIN);
 }
 switch(id){
  case CHOOK: case CADVF:
   ASSERT(2==n,EVLENGTH); R hook(fx(yv[0]),fx(yv[1]));
  case CFORK:
   ASSERT(3==n,EVLENGTH); 
   RZ(f=fx(yv[0])); ASSERT(AT(f)&VERB+NOUN,EVSYNTAX);
   RZ(g=fx(yv[1])); ASSERT(AT(g)&VERB,     EVSYNTAX);
   RZ(h=fx(yv[2])); ASSERT(AT(h)&VERB,     EVSYNTAX);
   R folk(f,g,h);
  default:
   if(id)fs=ds(id);
   ASSERT(fs&&RHS&AT(fs),EVDOMAIN);
   if(!n)R fs;
   ASSERT(1==n&&ADV&AT(fs)||2==n&&CONJ&AT(fs),EVLENGTH);
   if(0<n){RZ(p=fx(yv[0])); ASSERT(AT(p)&NOUN+VERB,EVDOMAIN);}
   if(1<n){RZ(q=fx(yv[1])); ASSERT(AT(q)&NOUN+VERB,EVDOMAIN);}
   R 1==n ? df1(p,fs) : df2(p,q,fs);
}}

static A jtunparse1(J jt,CW*c,A x,I j,A y){A q,z;C*s;I t;
 switch(t=c->type){
  case CBBLOCK: case CBBLOCKEND: case CTBLOCK: RZ(z=unparse(x));  break;
  case CASSERT:               RZ(q=unparse(x)); GATV0(z,LIT,8+AN(q),1); s=CAV(z); 
                              MC(s,"assert. ",8L); MC(8+s,CAV(q),AN(q)); break;
  case CLABEL:  case CGOTO:   RZ(z=ca(*AAV(x))); break;
  case CFOR:                  RZ(z=c->n?*AAV(x):spellcon(t)); break;
  default:                    RZ(z=spellcon(t));
 }
 if(j==c->source){
  GATV0(q,LIT,1+AN(y)+AN(z),1); s=CAV(q); 
  MC(s,CAV(y),AN(y)); s+=AN(y); *s++=' '; MC(s,CAV(z),AN(z)); 
  z=q;
 }
 R z;
}    /* unparse a single line */

static A*jtunparse1a(J jt,I m,A*hv,A*zv){A*v,x,y;CW*u;I i,j,k;
 y=hv[0]; v=AAV(y); 
 y=hv[1]; u=(CW*)AV(y);
 y=0; j=k=-1;
 for(i=0;i<m;++i,++u){
  RZ(x=unparse1(u,vec(BOX,u->n,v+u->i),j,y)); 
  k=u->source;
  if(j<k){if(y)*zv++=y; DQ(k-j-1, *zv++=mtv;);}
  y=x; j=k;
 }
 if(y)*zv++=y;
 DQ(k-j-1, *zv++=mtv;);
 R zv;
}

F2(jtunparsem){A h,*hv,dc,ds,mc,ms,z,*zu,*zv;I dn,m,mn,n,p;V*wv;
 RZ(a&&w);
 wv=VAV(w); h=wv->fgh[2]; hv=AAV(h);
 mc=hv[1];    ms=hv[2];    m=mn=AN(mc);
 dc=hv[1+HN]; ds=hv[2+HN]; n=dn=AN(dc);
 p=n&&(m||3==i0(wv->fgh[0])||VXOPR&wv->flag);
 if(equ(mtv,hv[2])&&equ(mtv,hv[2+HN])){
  if(m)mn=1+((CW*)AV(mc)+m-1)->source;
  if(n)dn=1+((CW*)AV(dc)+n-1)->source;
  GATV0(z,BOX,p+mn+dn,1); zu=zv=AAV(z);
  RZ(zv=unparse1a(m,hv,   zv)); if(p)RZ(*zv++=chr[':']);
  RZ(zv=unparse1a(n,hv+HN,zv));
  ASSERTSYS(AN(z)==zv-zu,"unparsem zn");
 }else{
  mn=AN(ms); dn=AN(ds);
  GATV0(z,BOX,p+mn+dn,1); zv=AAV(z);
  ICPY(zv,AAV(ms),mn); zv+=mn; if(p)RZ(*zv++=chr[':']);
  ICPY(zv,AAV(ds),dn);
 }
 if(a==num[0]){RZ(z=ope(z)); if(1==AR(z))z=table(z);}
 R z;
}    /* convert h parameter for : definitions; open if a is 0 */

static F2(jtxrep){A h,*hv,*v,x,z,*zv;CW*u;I i,j,n,q[3],*s;V*wv; 
 RZ(a&&w);
 RE(j=i0(a)); ASSERT(1==j||2==j,EVDOMAIN); j=1==j?0:HN;
 ASSERT(AT(w)&VERB+ADV+CONJ,EVDOMAIN);
 wv=FAV(w); h=wv->fgh[2];
 if(!(h&&CCOLON==wv->id))R reshape(v2(0L,3L),ace); 
 hv=AAV(h);
 x=hv[  j]; v=    AAV(x); 
 x=hv[1+j]; u=(CW*)AV(x); n=AN(x);
 GATV0(z,BOX,3*n,2); s=AS(z); s[0]=n; s[1]=3;
 zv=AAV(z);
 for(i=0;i<n;++i,++u){
  RZ(*zv++=rifvs(sc(i)));
  q[0]=u->type; q[1]=u->go; q[2]=u->source; RZ(*zv++=rifvs(vec(INT,3L,q)));
  RZ(*zv++=rifvs(unparse1(u,vec(BOX,u->n,v+u->i),-1L,0L)));
 }
 R z;
}    /* explicit representation -- h parameter for : definitions */


F1(jtarx){F1RANK(0,  jtarx,0); R arep(  symbrdlocknovalerr(nfb(w)));}
F1(jtdrx){F1RANK(0,  jtdrx,0); R drep(  symbrdlocknovalerr(nfb(w)));}
F1(jttrx){F1RANK(0,  jttrx,0); R trep(  symbrdlocknovalerr(nfb(w)));}
F1(jtlrx){F1RANK(0,  jtlrx,0); R lrep(  symbrdlocknovalerr(nfb(w)));}
F1(jtprx){F1RANK(0,  jtprx,0); R prep(  symbrdlocknovalerr(nfb(w)));}

F2(jtxrx){F2RANK(0,0,jtxrx,0); R xrep(a,symbrdlock(nfb(w)));}
