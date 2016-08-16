/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Atomic, Boxed, and 5!:0                                */

#include "j.h"
#include "w.h"


static F1(jtdrr){PROLOG(0055);A df,dg,fs,gs,hs,*x,z;B b,ex,xop;C c,id;I fl,*hv,m;V*v;
 RZ(w);
 if(AT(w)&NOUN)R w;
 if(AT(w)&NAME)R sfn(0,w);
 v=VAV(w); id=v->id; fl=v->flag; 
 fs=v->f; gs=v->g; hs=v->h; 
 if(fl&VXOPCALL)R drr(hs);
 xop=1&&VXOP&fl; ex=id==CCOLON&&hs&&!xop;
 b=id==CHOOK||id==CADVF; c=id==CFORK;
 m=!!fs+(gs||ex);
 if(!m)R spella(w);
 if(evoke(w))R sfn(0,fs);
 if(fs)RZ(df=fl&VGERL?every(fxeach(fs),0L,jtdrr):drr(fs));
 if(gs)RZ(dg=fl&VGERR?every(fxeach(gs),0L,jtdrr):drr(gs));
 if(ex)RZ(dg=unparsem(zero,w));
 m+=!b&&!xop||hs&&xop;
 GATV(z,BOX,m,1,0); x=AAV(z);
 RZ(x[0]=df);
 RZ(x[1]=b||c||xop?dg:fl&VDDOP?(hv=AV(hs),link(sc(hv[0]),link(spellout(id),sc(hv[1])))):spellout(id));
 if(2<m)RZ(x[2]=c||xop?drr(hs):dg);
 EPILOG(z);
}

F1(jtdrep){A z=drr(w); R z&&AT(z)&BOX?z:ravel(box(z));}


F1(jtaro){A fs,gs,hs,s,*u,*x,y,z;B ex,xop;C id;I*hv,m;V*v;
 RZ(w);
 if(FUNC&AT(w)){
  v=VAV(w); id=v->id; fs=v->f; gs=v->g; hs=v->h; 
  if(VXOPCALL&v->flag)R aro(hs);
  xop=1&&VXOP&v->flag;
  ex=hs&&id==CCOLON&&!xop;
  m=id==CFORK?3:!!fs+(ex||xop&&hs||!xop&&gs);
  if(!m)R spella(w);
  if(evoke(w))R sfn(0,fs);
 }
 GAT(z,BOX,2,1,0); x=AAV(z);
 if(NOUN&AT(w)){RZ(x[0]=ravel(scc(CNOUN))); x[1]=w; R z;}
 GATV(y,BOX,m,1,0); u=AAV(y);
 if(0<m)RZ(u[0]=aro(fs));
 if(1<m)RZ(u[1]=aro(ex?unparsem(zero,w):xop?hs:gs));
 if(2<m)RZ(u[2]=aro(hs));
 s=xop?aro(gs):VDDOP&v->flag?(hv=AV(hs),aro(foreign(sc(hv[0]),sc(hv[1])))):spellout(id);
 RZ(x[0]=s); x[1]=y;
 R z;
}

F1(jtarep){R box(aro(w));}


static F1(jtfxchar){A y;C c,d,id,*s;I m,n;
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(n,EVLENGTH);
 s=CAV(w); c=*(s+n-1);
 DO(n, d=s[i]; ASSERT(32<=d&&d<127,EVSPELL););
 if(CA==ctype[(UC)*s]&&c!=CESC1&&c!=CESC2)R swap(w);
 ASSERT(id=spellin(n,s),EVSPELL);
 if(id!=CFCONS)y=ds(id); else{m=s[n-2]-'0'; y=FCONS(CSIGN!=*s?sc(m):2==n?ainf:sc(-m));}
 ASSERT(y&&RHS&AT(y),EVDOMAIN);
 R y;
}

F1(jtfx){A f,fs,g,h,p,q,*wv,y,*yv;C id;I m,n=0,wd,yd;
 RZ(w);
 if(LIT&AT(w))R fxchar(w);
 m=AN(w); wd=(I)w*ARELATIVE(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(1==m||2==m,EVLENGTH);
 wv=AAV(w); y=WVR(0);
 if(BOX&AT(y)){RZ(fs=fx(y)); id=0;}
 else{RZ(y=vs(y)); ASSERT(id=spellin(AN(y),CAV(y)),EVSPELL);}
 if(1<m){
  y=WVR(1); n=AN(y); yv=AAV(y); yd=(I)y*ARELATIVE(y);
  if(id==CNOUN)R y;
  ASSERT(1>=AR(y),EVRANK);
  ASSERT(BOX&AT(y),EVDOMAIN);
 }
 switch(id){
  case CHOOK: case CADVF:
   ASSERT(2==n,EVLENGTH); R hook(fx(YVR(0)),fx(YVR(1)));
  case CFORK:
   ASSERT(3==n,EVLENGTH); 
   RZ(f=fx(YVR(0))); ASSERT(AT(f)&VERB+NOUN,EVSYNTAX);
   RZ(g=fx(YVR(1))); ASSERT(AT(g)&VERB,     EVSYNTAX);
   RZ(h=fx(YVR(2))); ASSERT(AT(h)&VERB,     EVSYNTAX);
   R folk(f,g,h);
  default:
   if(id)fs=ds(id);
   ASSERT(fs&&RHS&AT(fs),EVDOMAIN);
   if(!n)R fs;
   ASSERT(1==n&&ADV&AT(fs)||2==n&&CONJ&AT(fs),EVLENGTH);
   if(0<n){RZ(p=fx(YVR(0))); ASSERT(AT(p)&NOUN+VERB,EVDOMAIN);}
   if(1<n){RZ(q=fx(YVR(1))); ASSERT(AT(q)&NOUN+VERB,EVDOMAIN);}
   R 1==n ? df1(p,fs) : df2(p,q,fs);
}}

static A jtunparse1(J jt,CW*c,A x,I j,A y){A q,z;C*s;I t;
 switch(t=c->type){
  case CBBLOCK: case CTBLOCK: RZ(z=unparse(x));  break;
  case CASSERT:               RZ(q=unparse(x)); GATV(z,LIT,8+AN(q),1,0); s=CAV(z); 
                              MC(s,"assert. ",8L); MC(8+s,CAV(q),AN(q)); break;
  case CLABEL:  case CGOTO:   RZ(z=ca(*AAV(x))); break;
  case CFOR:                  RZ(z=c->n?*AAV(x):spellcon(t)); break;
  default:                    RZ(z=spellcon(t));
 }
 if(j==c->source){
  GATV(q,LIT,1+AN(y)+AN(z),1,0); s=CAV(q); 
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
  if(j<k){if(y)*zv++=y; DO(k-j-1, *zv++=mtv;);}
  y=x; j=k;
 }
 if(y)*zv++=y;
 DO(k-j-1, *zv++=mtv;);
 R zv;
}

F2(jtunparsem){A h,*hv,dc,ds,mc,ms,z,*zu,*zv;I dn,m,mn,n,p;V*wv;
 RZ(a&&w);
 wv=VAV(w); h=wv->h; hv=AAV(h);
 mc=hv[1];    ms=hv[2];    m=mn=AN(mc);
 dc=hv[1+HN]; ds=hv[2+HN]; n=dn=AN(dc);
 p=n&&(m||3==i0(wv->f)||VXOPR&wv->flag);
 if(equ(mtv,hv[2])&&equ(mtv,hv[2+HN])){
  if(m)mn=1+((CW*)AV(mc)+m-1)->source;
  if(n)dn=1+((CW*)AV(dc)+n-1)->source;
  GATV(z,BOX,p+mn+dn,1,0); zu=zv=AAV(z);
  RZ(zv=unparse1a(m,hv,   zv)); if(p)RZ(*zv++=chr[':']);
  RZ(zv=unparse1a(n,hv+HN,zv));
  ASSERTSYS(AN(z)==zv-zu,"unparsem zn");
 }else{
  mn=AN(ms); dn=AN(ds);
  GATV(z,BOX,p+mn+dn,1,0); zv=AAV(z);
  ICPY(zv,AAV(ms),mn); zv+=mn; if(p)RZ(*zv++=chr[':']);
  ICPY(zv,AAV(ds),dn);
 }
 if(a==zero){RZ(z=ope(z)); if(1==AR(z))z=table(z);}
 R z;
}    /* convert h parameter for : definitions; open if a is 0 */

static F2(jtxrep){A h,*hv,*v,x,z,*zv;CW*u;I i,j,n,q[3],*s;V*wv; 
 RZ(a&&w);
 RE(j=i0(a)); ASSERT(1==j||2==j,EVDOMAIN); j=1==j?0:HN;
 ASSERT(AT(w)&VERB+ADV+CONJ,EVDOMAIN);
 wv=VAV(w); h=wv->h;
 if(!(h&&CCOLON==wv->id))R reshape(v2(0L,3L),ace); 
 hv=AAV(h);
 x=hv[  j]; v=    AAV(x); 
 x=hv[1+j]; u=(CW*)AV(x); n=AN(x);
 GATV(z,BOX,3*n,2,0); s=AS(z); s[0]=n; s[1]=3;
 zv=AAV(z);
 for(i=0;i<n;++i,++u){
  RZ(*zv++=sc(i));
  q[0]=u->type; q[1]=u->go; q[2]=u->source; RZ(*zv++=vec(INT,3L,q));
  RZ(*zv++=unparse1(u,vec(BOX,u->n,v+u->i),-1L,0L));
 }
 R z;
}    /* explicit representation -- h parameter for : definitions */


F1(jtarx){F1RANK(0,  jtarx,0); R arep(  symbrdlock(nfb(w)));}
F1(jtdrx){F1RANK(0,  jtdrx,0); R drep(  symbrdlock(nfb(w)));}
F1(jttrx){F1RANK(0,  jttrx,0); R trep(  symbrdlock(nfb(w)));}
F1(jtlrx){F1RANK(0,  jtlrx,0); R lrep(  symbrdlock(nfb(w)));}
F1(jtprx){F1RANK(0,  jtprx,0); R prep(  symbrdlock(nfb(w)));}

F2(jtxrx){F2RANK(0,0,jtxrx,0); R xrep(a,symbrdlock(nfb(w)));}
