/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Tree                                                   */

#include "j.h"

static F1(jttrr);


static F1(jttrc){A bot,p,*v,x,y;B b;C*bv,c,ul,ll,*pv;I j,k,m,*s,xn,*xv,yn,*yv;
 ARGCHK1(w);
 s=AS(w); v=AAV(w);
 xn=s[0]; RZ(x=apvwr(xn,0L,0L)); xv=AV(x);
 yn=s[1]; RZ(y=apvwr(yn,0L,0L)); yv=AV(y);
 j=0; DO(xn, xv[i]=SETIC(C(v[j]),k); j+=yn;);
 GATV0(bot,LIT,yn,1); bv=CAV1(bot);
 ul=JT(jt,bx)[0]; ll=JT(jt,bx)[6];
 for(j=b=0;j<xn;++j,b=0<j)
  for(k=0;k<yn;++k){
   p=C(*v++);
   if(AN(p)){
    m=AS(p)[1]; yv[k]=MAX(yv[k],m);
    pv=CAV(p); c=*pv;
    if(b&&(c==ul&&' '!=bv[k]||c!=' '&&ll==bv[k])){xv[j-1]+=1; b=0;}
    bv[k]=pv[AN(p)-m];
   }else bv[k]=' ';
  }
 R jlink(x,y);
}

static I jtpad(J jt,A a,A w,C*zv){C dash,*u,*v,*wv;I c,d,r,*s;
 ARGCHK2(a,w);
 s=AV(a); r=s[0]; d=s[1];
 if(AN(w)){
  c=AS(w)[1]; wv=CAV(w);
  if(c==d)MC(zv,wv,AN(w));
  else{I k;
   zv-=d; v=zv+c-1; u=wv-c; dash=JT(jt,bx)[10];
   DQ(SETIC(w,k), MC(zv+=d,u+=c,c); v+=d; if(dash==*v)mvc(d-c,1+v,1,iotavec-IOTAVECBEGIN+dash););
 }}
 R r*d;
}

static F1(jtgraft){A p,q,t,*u,x,y,z,*zv;C*v;I d,j,k,m,n,*pv,*s,xn,*xv,yn,*yv;
 RZ(t=trc(w)); u=AAV(t);
 x=C(u[0]); xn=AN(x); xv=AV(x); m=0; DO(xn,m+=xv[i];);
 y=C(u[1]); yn=AN(y); yv=AV(y);
 RZ(p=v2(0L,0L));  pv=AV(p);
 GATV0(z,BOX,yn,1); zv=AAV1(z);
 u=AAV(w);
 for(j=0;j<yn;++j){
  DPMULDE(m,yv[j],k); GATV0(q,LIT,k,2); s=AS(q); *s=m; *++s=yv[j];
  v=CAV2(q); mvc(AN(q),v,1,iotavec-IOTAVECBEGIN+' ');
  pv[1]=yv[j]; k=j-yn; DO(xn, *pv=xv[i]; RE(v+=pad(p,C(u[k+yn]),v)); k+=yn;);
  zv[j]=incorp(q);
 }
 t=zv[0]; n=yv[0];
 if(1==m)RZ(p=scc(JT(jt,bx)[10]))
 else{
  v=CAV(t);         DO(m, if(' '!=*v){j=i;   break;} v+=n;);
  v=CAV(t)+AN(t)-n; DO(m, if(' '!=*v){k=m-i; break;} v-=n;);
  d=k-j;
  GATV0(p,LIT,m,1); v=CAV1(p); mvc(m,v,1,iotavec-IOTAVECBEGIN+' ');
  if(1==d)*(v+j)=JT(jt,bx)[10]; else{ mvc(d,v+j,1,iotavec-IOTAVECBEGIN+JT(jt,bx)[9]); v[j]=*JT(jt,bx); v[k-1]=JT(jt,bx)[6];}
 }
 RZ(zv[0]=incorp(stitch(p,t)));
 R z;
}

static A jtcenter(J jt,A a,I j,I k,I m){A z;C*x;I n,*s,zn;
 ARGCHK1(a);
 n=AN(a); DPMULDE(m,n,zn); GATV0(z,LIT,zn,2); s=AS(z); *s=m; *++s=n;
 RE(zn=mult(m,n));
 x=CAV2(z); mvc(AN(z),x,1,iotavec-IOTAVECBEGIN+' '); MC(x+n*(j+((m-(j+k))>>1)),AV(a),n);
 R z;
}

static F2(jttroot){A t,x;B b;C*u,*v;I j=0,k=0,m,n,*s;
 ARGCHK2(a,w);
 m=AN(a); u=CAV(a); b=!m||1==m&&BETWEENC(*u,'0','9');
 GATV0(x,LIT,b?1:4+m,1); v=CAV1(x);
 *v=JT(jt,bx)[10]; if(!b){v[3+m]=JT(jt,bx)[10]; v[1]=v[2+m]=' '; MC(2+v,u,m);}
 t=C(AAV(w)[0]); s=AS(t); m=s[0]; n=s[1];
 u=CAV(t);         DO(m, if(' '!=*u){j=i; break;} u+=n;);
 u=CAV(t)+(m-1)*n; DO(m, if(' '!=*u){k=i; break;} u-=n;);
 R jlink(center(x,j,k,m),w);
}

static F1(jttleaf){A t,z;C*v;I n,*s;
 ARGCHK1(w);
 n=AN(w);
 GATV0(t,LIT,2+n,2); s=AS(t); s[0]=1; s[1]=2+n;
 v=CAV2(t); v[0]=JT(jt,bx)[10]; v[1]=' '; MC(2+v,AV(w),n);
 GAT0(z,BOX,1,1); AAV1(z)[0]=incorp(t);
 R z;
}

static F1(jttconnect){A*wv,x,y,z;B b,d;C c,*u,*xv,*yv,*zv;I e,i,j,m,n,p,q,zn;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w); y=C(wv[0]); m=AS(y)[0];
 e=0; DO(n,e+=AS(C(wv[i]))[1];);
 DPMULDE(m,e,zn); GATVR(z,LIT,zn,2,AS(y)); AS(z)[1]=e; zv=CAV2(z);
 for(i=0;i<n;++i){
  y=C(wv[i]); q=AS(y)[1]; yv=CAV(y);
  if(i){
   xv=CAV(x)+p-1;
   for(j=0;j<m;++j){
    b=JT(jt,bx)[10]==*xv; c=*yv; d=JT(jt,bx)[10]==yv[1];
    if(b&&c==JT(jt,bx)[9])c=JT(jt,bx)[5];
    if(d&&c==JT(jt,bx)[5])c=JT(jt,bx)[4];
    if(d&&c==JT(jt,bx)[9])c=JT(jt,bx)[3];
    if(b&&c==JT(jt,bx)[6])c=JT(jt,bx)[7];
    *yv=c; yv+=q; xv+=p;
  }}
  u=zv-e; yv=CAV(y)-q; DQ(m, MC(u+=e,yv+=q,q);); zv+=q;
  x=y; p=q;
 }
 R z;
}

EVERYFS(trrself,jttrr,0,0,VFLAGNONE)

static F1(jttreach){R troot(scc('0'),graft(ope(every(w,(A)&trrself))));}

static F1(jttrr){PROLOG(0058);A hs,s,t,*x,z;B ex,xop;C id;I fl,*hv,m;V*v;
 ARGCHK1(w);
 if(AT(w)&NOUN+NAME){RETF(tleaf(lrep(w)));}
 v=FAV(w); id=v->id; fl=v->flag;
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=v->fgh[fndx]; A gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 if(id==CATCO&&AT(w)&VERB&&FAV(gs)->id==CTDOT)R trr(gs);  // if <@:t. discard the <@:
 hs=v->fgh[2]; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
 if(id==CIBEAM&&!(AT(w)&CONJ)){fs=scib(FAV(w)->localuse.lu1.foreignmn[0]); gs=scib(FAV(w)->localuse.lu1.foreignmn[1]);} 
 if(id==CFORK&&hs==0){hs=gs; gs=fs; fs=ds(CCAP);}  // reconstitute capped fork
 if(fl&VXOPCALL){RETF(trr(hs));}
 xop=id==CCOLONE&&VXOP&fl; ex=id==CCOLONE&&hs&&!xop;
 m=(I )!!fs+(I )(gs||ex)+(I )(id==CFORK||xop&&hs);
 if(!m){RETF(tleaf(spella(w)));}
 if(evoke(w)){RZ(w=sfne(w)); RETF((AT(w)&FUNC?jttrr:jttleaf)(jt,w));}
 GATV0(t,BOX,m,1); x=AAV1(t);
 if(0<m)RZ(x[0]=incorp(fl&VGERL?treach(fxeach(fs,(A)&jtfxself[0])):trr(fs)));
 if(1<m)RZ(x[1]=incorp(fl&VGERR?treach(fxeach(gs,(A)&jtfxself[0])):ex?trr(unparsem(num(0),w)):trr(gs)));
 if(2<m)RZ(x[2]=incorp(trr(hs)));
 s=xop?spellout('0'):spellout(id);
 z=troot(s,graft(ope(t)));
 EPILOG(z);
}

F1(jttrep){PROLOG(0059); A z=tconnect(troot(mtv,trr(w))); EPILOG(z);}
