/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Tree                                                   */

#include "j.h"

static F1(jttrr);


static F1(jttrc){A bot,p,*v,x,y;B b;C*bv,c,ul,ll,*pv;I j,k,m,*s,xn,*xv,yn,*yv;
 RZ(w);
 s=AS(w); v=AAV(w);
 xn=s[0]; RZ(x=apvwr(xn,0L,0L)); xv=AV(x);
 yn=s[1]; RZ(y=apvwr(yn,0L,0L)); yv=AV(y);
 j=0; DO(xn, xv[i]=IC(v[j]); j+=yn;);
 GATV0(bot,LIT,yn,1); bv=CAV(bot);
 ul=jt->bx[0]; ll=jt->bx[6];
 for(j=b=0;j<xn;++j,b=0<j)
  for(k=0;k<yn;++k){
   p=*v++;
   if(AN(p)){
    m=*(1+AS(p)); yv[k]=MAX(yv[k],m);
    pv=CAV(p); c=*pv;
    if(b&&(c==ul&&' '!=bv[k]||c!=' '&&ll==bv[k])){xv[j-1]+=1; b=0;}
    bv[k]=*(pv+AN(p)-m);
   }else bv[k]=' ';
  }
 R link(x,y);
}

static I jtpad(J jt,A a,A w,C*zv){C dash,*u,*v,*wv;I c,d,r,*s;
 RZ(a&&w);
 s=AV(a); r=s[0]; d=s[1];
 if(AN(w)){
  c=*(1+AS(w)); wv=CAV(w);
  if(c==d)MC(zv,wv,AN(w));
  else{
   zv-=d; v=zv+c-1; u=wv-c; dash=jt->bx[10];
   DQ(IC(w), MC(zv+=d,u+=c,c); v+=d; if(dash==*v)memset(1+v,dash,d-c););
 }}
 R r*d;
}

static F1(jtgraft){A p,q,t,*u,x,y,z,*zv;C*v;I d,j,k,m,n,*pv,*s,xn,*xv,yn,*yv;
 RZ(t=trc(w)); u=AAV(t);
 x=u[0]; xn=AN(x); xv=AV(x); m=0; DO(xn,m+=xv[i];);
 y=u[1]; yn=AN(y); yv=AV(y);
 RZ(p=v2(0L,0L));  pv=AV(p);
 GATV0(z,BOX,yn,1); zv=AAV(z);
 u=AAV(w);
 for(j=0;j<yn;++j){
  RE(k=mult(m,yv[j])); GATV0(q,LIT,k,2); s=AS(q); *s=m; *++s=yv[j];
  v=CAV(q); memset(v,' ',AN(q));
  pv[1]=yv[j]; k=j-yn; DO(xn, *pv=xv[i]; RE(v+=pad(p,u[k+=yn],v)););
  zv[j]=q;
 }
 t=zv[0]; n=yv[0];
 if(1==m)RZ(p=scc(jt->bx[10]))
 else{
  v=CAV(t);         DO(m, if(' '!=*v){j=i;   break;} v+=n;);
  v=CAV(t)+AN(t)-n; DO(m, if(' '!=*v){k=m-i; break;} v-=n;);
  d=k-j;
  GATV0(p,LIT,m,1); v=CAV(p); memset(v,' ',m);
  if(1==d)*(v+j)=jt->bx[10]; else{memset(v+j,jt->bx[9],d); *(v+j)=*jt->bx; *(v+k-1)=jt->bx[6];}
 }
 RZ(zv[0]=rifvs(stitch(p,t)));
 R z;
}

static A jtcenter(J jt,A a,I j,I k,I m){A z;C*x;I n,*s,zn;
 RZ(a);
 n=AN(a); RE(zn=mult(m,n)); GATV0(z,LIT,zn,2); s=AS(z); *s=m; *++s=n;
 x=CAV(z); memset(x,' ',AN(z)); MC(x+n*(j+((m-(j+k))>>1)),AV(a),n);
 R z;
}

static F2(jttroot){A t,x;B b;C*u,*v;I j=0,k=0,m,n,*s;
 RZ(a&&w);
 m=AN(a); u=CAV(a); b=!m||1==m&&'0'<=*u&&*u<='9';
 GATV0(x,LIT,b?1:4+m,1); v=CAV(x);
 *v=jt->bx[10]; if(!b){v[3+m]=jt->bx[10]; v[1]=v[2+m]=' '; MC(2+v,u,m);}
 t=*AAV(w); s=AS(t); m=*s; n=*(1+s);
 u=CAV(t);         DO(m, if(' '!=*u){j=i; break;} u+=n;);
 u=CAV(t)+(m-1)*n; DO(m, if(' '!=*u){k=i; break;} u-=n;);
 R link(center(x,j,k,m),w);
}

static F1(jttleaf){A t,z;C*v;I n,*s;
 RZ(w);
 n=AN(w);
 GATV0(t,LIT,2+n,2); s=AS(t); s[0]=1; s[1]=2+n;
 v=CAV(t); v[0]=jt->bx[10]; v[1]=' '; MC(2+v,AV(w),n);
 GAT0(z,BOX,1,1); *AAV(z)=t;
 R z;
}

static F1(jttconnect){A*wv,x,y,z;B b,d;C c,*u,*xv,*yv,*zv;I e,i,j,m,n,p,q,zn;
 RZ(w);
 n=AN(w); wv=AAV(w); y=*wv; m=*AS(y);
 e=0; DO(n,e+=*(1+AS(wv[i])););
 RE(zn=mult(m,e)); GATVR(z,LIT,zn,2,AS(y)); AS(z)[1]=e; zv=CAV(z);
 for(i=0;i<n;++i){
  y=wv[i]; q=*(1+AS(y)); yv=CAV(y);
  if(i){
   xv=CAV(x)+p-1;
   for(j=0;j<m;++j){
    b=jt->bx[10]==*xv; c=*yv; d=jt->bx[10]==*(1+yv);
    if(b&&c==jt->bx[9])c=jt->bx[5];
    if(d&&c==jt->bx[5])c=jt->bx[4];
    if(d&&c==jt->bx[9])c=jt->bx[3];
    if(b&&c==jt->bx[6])c=jt->bx[7];
    *yv=c; yv+=q; xv+=p;
  }}
  u=zv-e; yv=CAV(y)-q; DQ(m, MC(u+=e,yv+=q,q);); zv+=q;
  x=y; p=q;
 }
 R z;
}

static F1(jttreach){R troot(scc('0'),graft(ope(every(w,0L,jttrr))));}

static F1(jttrr){PROLOG(0058);A hs,s,t,*x,z;B ex,xop;C id;I fl,*hv,m;V*v;
 RZ(w);
 if(AT(w)&NOUN+NAME){RETF(tleaf(lrep(w)));}
 v=FAV(w); id=v->id; fl=v->flag;
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=v->fgh[fndx]; A gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 hs=v->fgh[2]; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
 if(fl&VXOPCALL){RETF(trr(hs));}
 xop=1&&VXOP&fl; ex=id==CCOLON&&hs&&!xop;
 m=(I )!!fs+(I )(gs||ex)+(I )(id==CFORK||xop&&hs);
 if(!m){RETF(tleaf(spella(w)));}
 if(evoke(w)){RZ(w=sfne(w)); RETF((AT(w)&FUNC?jttrr:jttleaf)(jt,w));}
 GATV0(t,BOX,m,1); x=AAV(t);
 if(0<m)RZ(x[0]=rifvs(fl&VGERL?treach(fxeach(fs)):trr(fs)));
 if(1<m)RZ(x[1]=rifvs(fl&VGERR?treach(fxeach(gs)):ex?trr(unparsem(num[0],w)):trr(gs)));
 if(2<m)RZ(x[2]=rifvs(trr(hs)));
 s=xop?spellout('0'):fl&VDDOP?(hv=AV(hs),over(thorn1(sc(hv[0])),over(spellout(id),thorn1(sc(hv[1]))))):spellout(id);
 z=troot(s,graft(ope(t)));
 EPILOG(z);
}

F1(jttrep){PROLOG(0059); A z=tconnect(troot(mtv,trr(w))); EPILOG(z);}
