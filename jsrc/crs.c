/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Rank on Sparse Arrays                                     */

#include "j.h"


static A jtsprarg(J jt,I f,A x){A q;B*b,c;I r;P*xp;
 r=AR(x); xp=PAV(x);
 if(ISSPARSE(AT(x))){c=1; RZ(b=bfi(r,SPA(xp,a),1)); DO(f, if(!b[i]){c=0; break;});}
 else{c=0; GATV0(q,B01,r,1); b=BAV1(q); mvc(r,b,MEMSET00LEN,MEMSET00);}
 mvc(f,b,1,MEMSET01);
 R c||!r?x:reaxis(ifb(r,b),x);
}    /* ensure frame axes are sparse */

static A jtsprinit(J jt,I f,I r,I*s,I t,P*p){A a,a1,z;I n,*u,*v;P*zp;
 GASPARSE(z,t,1,r,f+s); zp=PAV(z); 
 a=SPA(p,a); n=AN(a)-f; u=f+AV(a); GATV0(a1,INT,n,1); v=AV1(a1); DO(n, v[i]=u[i]-f;);
 SPB(zp,a,a1);
 SPB(zp,e,ca(SPA(p,e)));
 SPB(zp,i,iota(v2(0L,n)));  // empty so cannot be readonly
 SPB(zp,x,repeat(num(0),SPA(p,x)));
 R z;
}    /* initialize an argument cell */

static B*jtspredge(J jt,A y,I f,I*zm){A q;B*b;I c,m,n,*v;
 v=AS(y); n=v[0]; c=v[1]; m=n?1:0;
 GATV0(q,B01,n,1); b=BAV1(q);
 if(n){
  if(f){v=AV(y); DO(n-1, if(b[i]=1&&ICMP(v,v+c,f))++m; v+=c;);}else mvc(n,b,MEMSET00LEN,MEMSET00);
  b[n-1]=1;
 }
 *zm=m;
 R b;
}    /* cell boundaries per index matrix y */

static A jtsprz(J jt,A z0,A y,A e,I f,I*s){A a,a0,q,y0,z;B d;I c,et,h,m,n,r,t,*u,*v,zt;P*ep,*zp,*zq;
 RZ(z0&&y&&e);
 ASSERT(AN(e),EVDOMAIN);
 if(AN(z0)&&!(AT(z0)&SPARSABLE))R z0;  // no backdoor to sparse boxes.  If non-sparsable type, leave dense
 if(ISSPARSE(AT(e))){ep=PAV(e); ASSERT(all1(eq(SPA(ep,e),SPA(ep,x))),EVSPARSE); q=SPA(ep,e);}
 else{RZ(q=reshape(mtv,e)); ASSERT(all1(eq(q,e)),EVSPARSE);}
 if(!AS(z0)[0]){
  t=AT(q); zt=STYPE(t);
  GASPARSE(z,zt,1L,f+AR(e),s); ICPY(f+AS(z),AS(e),AR(e));
  zp=PAV(z); SPB(zp,e,q); SPB(zp,a,mtv); 
  GATR(q,INT,0,2,(I*)&zeroZ); SPB(zp,i,q);   // zeroZ is a pun for 0 0!!
  GA00(q,t,0L,1+AR(z)); AS(q)[0]=0; ICPY(1+AS(q),AS(z),AR(z)); SPB(zp,x,q);
  R z;
 }
 e=q;
 zt=t=AT(z0); d=!ISSPARSE(t); if(d)zt=STYPE(t); else t=DTYPE(zt);
 et=AT(e); m=maxtype(et,t); zt=STYPE(m);
 r=AR(z0);
 GASPARSE(z,zt,1,f+r-1,s); ICPY(AS(z)+f,AS(z0)+1,r-1);
 zp=PAV(z); SPB(zp,e,TYPESEQ(m,et)?e:cvt(m,e));
 if(d){SPB(zp,a,apvwr(f,0L,1L)); SPB(zp,i,y); SPB(zp,x,TYPESEQ(m,t)?z0:cvt(m,z0)); R z;}
 zq=PAV(z0); y0=SPA(zq,i); v=AS(y0); n=v[0]; c=v[1]; v=AV(y0);
 ASSERT(equ(e,SPA(zq,e)),EVNONCE);
 h=AS(y)[0]; GATV0(q,INT,h,1); u=AV1(q); mvc(h*SZI,u,MEMSET00LEN,MEMSET00); 
 if(n){h=-1; DO(n-1, if(v[0]!=v[c]){u[*v]=i-h; h=i;} v+=c;); u[*v]=n-1-h;}
 SPB(zp,i,stitch(repeat(q,y),dropr(1L,y0)));
 a0=SPA(zq,a); v=AV(a0);
 GATV0(a,INT,f+c-1,1); u=AV1(a); DO(f, u[i]=i;); DO(c-1, u[f+i]=v[1+i]+f-1;); 
 SPB(zp,a,a);
// memory leak if(TYPESEQ(m,t))ras(SPA(zq,x));
 SPB(zp,x,TYPESEQ(m,t)?SPA(zq,x):cvt(m,SPA(zq,x)));
 R z;
}    /* result processing */

A jtsprank1(J jt,A w,A fs,I mr,AF f1){PROLOG(0043);A q,wx,wy,wy1,ww,z,ze,zi,*zv;B*wb;
     I c,i,*iv,j,k,m,n,*v,wcr,wf,wr,*ws,wt,*wv;P*wp,*wq;
 ARGCHK1(w);
 wr=AR(w); ws=AS(w); efr(wcr,wr,mr); wf=wr-wcr;
 if(!wf)R CALL1(f1,w,fs);
 DO(wf, ASSERT(ws[i]!=0,EVNONCE););
 RZ(w=sprarg(wf,w)); wp=PAV(w); wx=SPA(wp,x); wy=SPA(wp,i); 
 if(mr){
  wt=AT(w); 
  v=AS(wy); n=v[0]; c=v[1]; wv=AV(wy); RZ(wy1=dropr(wf,wy));
  RZ(wb=spredge(wy,wf,&m));
  RZ(ww=sprinit(wf,wcr,ws,wt,wp)); wq=PAV(ww);
  RZ(ze=CALL1(f1,ww,fs));
  GATV0(z,BOX,m,1); zv=AAV1(z);
  GATV0(zi,INT,m*wf,2); iv=AV2(zi); v=AS(zi); v[0]=m; v[1]=wf;
  for(i=j=0;i<m;++i){
   k=1+(B*)memchr(wb+j,C1,n-j)-(wb+j);
   ICPY(iv,wv+j*c,wf); iv+=wf;
   RZ(q=apv(k,j,1L)); SPB(wq,i,from(q,wy1)); SPB(wq,x,from(q,wx));
   RZ(zv[i]=incorp(CALL1(f1,ww,fs)));
   j+=k;
  }
  RZ(z=ope(z));
 }else{RZ(zi=ca(wy)); RZ(z=rank1ex(wx,fs,mr,f1)); RZ(ze=CALL1(f1,SPA(wp,e),fs));}  // mr is 0
 z=sprz(z,zi,ze,wf,ws);
 EPILOG(z);
}    /* f"r w on sparse arrays */

static I jtspradv(J jt,I n,B*b,I f,I r,I j,P*p,A*z){A s,x;I k;P*q;
 k=n?1+(B*)memchr(b+j,C1,n-j)-(b+j):0;
 x=SPA(p,x);
 if(r){
  q=PAV(*z);
  RZ(s=apv(k,j,1L)); 
  SPB(q,i,from(s,dropr(f,SPA(p,i)))); 
  SPB(q,x,from(s,x));
 }else RZ(*z=AN(x)?from(sc(j),x):ca(SPA(p,e)));
 R k;
}    /* advance to the next cell */

static A jtsprank2_0w(J jt,A a,A w,A fs,AF f2,I wf,I wcr){PROLOG(0044);A we,ww,y,z,zi,*zv;B*wb;
     I f,*iv,j,*v,wc,wj,wk,wm,wn,*ws,wt,*wv;P*wp;
 f=wf; ws=AS(w);
 RZ(w=sprarg(wf,w)); wt=AT(w); wp=PAV(w);
 y=SPA(wp,i); v=AS(y); wn=v[0]; wc=v[1]; wv=AV(y); RZ(wb=spredge(y,wf,&wm));
 RZ(ww=sprinit(wf,wcr,ws,wt,wp)); RZ(we=wcr?ca(ww):SPA(wp,e));
 GATV0(z,BOX,MAX(1,wm),1); zv=AAV1(z);
 GATV0(zi,INT,f*MAX(1,wm),2); iv=AV2(zi); v=AS(zi); v[0]=wm; v[1]=f;
 RE(wj=wk=spradv(wn,wb,wf,wcr,0L,wp,&ww)); j=0;
 while(1){
  ICPY(iv,wv,f); iv+=f; RZ(zv[j++]=incorp(CALL2(f2,a,ww,fs)));
  if(wj==wn)break;
  wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;
 }
 RZ(z=ope(z)); AS(z)[0]=wm;  // we did one cell of aa to get the shape, but now we have to set back to correct # indexes
 z=sprz(z,zi,CALL2(f2,a,we,fs),f,ws);
 EPILOG(z);
}

static A jtsprank2_a0(J jt,A a,A w,A fs,AF f2,I af,I acr){PROLOG(0045);A aa,ae,y,z,zi,*zv;B*ab;
     I f,*iv,j,*v,ac,aj,ak,am,an,*as,at,*av;P*ap;
 f=af; as=AS(a);
 RZ(a=sprarg(af,a)); at=AT(a); ap=PAV(a);
 y=SPA(ap,i); v=AS(y); an=v[0]; ac=v[1]; av=AV(y); RZ(ab=spredge(y,af,&am));
 RZ(aa=sprinit(af,acr,as,at,ap)); RZ(ae=acr?ca(aa):SPA(ap,e));
 GATV0(z,BOX,MAX(1,am),1); zv=AAV1(z);
 GATV0(zi,INT,f*MAX(1,am),2); iv=AV2(zi); v=AS(zi); v[0]=am; v[1]=f;
 RE(aj=ak=spradv(an,ab,af,acr,0L,ap,&aa)); j=0;
 while(1){
  ICPY(iv,av,f); iv+=f; RZ(zv[j++]=incorp(CALL2(f2,aa,w,fs)));
  if(aj==an)break;
  av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;
 }
 RZ(z=ope(z)); AS(z)[0]=am;  // we did one cell of aa to get the shape, but now we have to set back to correct # indexes
 z=sprz(z,zi,CALL2(f2,ae,w,fs),f,as);
 EPILOG(z);
}

A jtsprank2(J jt,A a,A w,A fs,I lr,I rr,AF f2){PROLOG(0046);A aa,ae,we,ww,y,zi,z,*zv;B*ab,b,*wb;I ac,acr,af,aj,ak,am,an,
     ar,*as,at,*av,d,f,g,*ii,*iv,j,k,m,s,*u,*v,wc,wcr,wf,wj,wk,wm,wn,wr,*ws,wt,*wv;P*ap,*wp;
 ARGCHK2(a,w);
 STACKCHKOFL
 ar=AR(a); as=AS(a); efr(acr,ar,lr); af=ar-acr; 
 wr=AR(w); ws=AS(w); efr(wcr,wr,rr); wf=wr-wcr; 
 if(!af&&!wf)R CALL2(f2,a,w,fs);
 DO(af, ASSERT(as[i]!=0,EVNONCE););
 DO(wf, ASSERT(ws[i]!=0,EVNONCE););
 if(!af)R sprank2_0w(a,w,fs,f2,wf,wcr);
 if(!wf)R sprank2_a0(a,w,fs,f2,af,acr);
 f=MIN(af,wf); g=MAX(af,wf); m=1;
 if(f<g){d=g-f; RZ(y=odom(2L,d,f+(af<wf?ws:as))); ii=AV(y); m=AS(y)[0];}
 ASSERT(!ICMP(as,ws,f),EVLENGTH);
 RZ(a=sprarg(af,a)); at=AT(a); ap=PAV(a);
 RZ(w=sprarg(wf,w)); wt=AT(w); wp=PAV(w);
 y=SPA(ap,i); v=AS(y); an=v[0]; ac=v[1]; av=an?AV(y):0; RZ(ab=spredge(y,af,&am));
 y=SPA(wp,i); v=AS(y); wn=v[0]; wc=v[1]; wv=wn?AV(y):0; RZ(wb=spredge(y,wf,&wm));
 RZ(aa=sprinit(af,acr,as,at,ap)); RZ(ae=acr?ca(aa):SPA(ap,e));
 RZ(ww=sprinit(wf,wcr,ws,wt,wp)); RZ(we=wcr?ca(ww):SPA(wp,e));
 b=af<wf; j=am*(af<wf?m:1)+wm*(af>wf?m:1);
 GATV0(z, BOX,MAX(1,j),  1); zv=AAV1(z);
 GATV0(zi,INT,MAX(1,j)*g,2); v=AS(zi); v[0]=j; v[1]=g; iv=AV2(zi); 
 RE(aj=ak=spradv(an,ab,af,acr,0L,ap,&aa));
 RE(wj=wk=spradv(wn,wb,wf,wcr,0L,wp,&ww)); j=s=k=0; u=ii; y=0; v=0;
 if(af==wf)while(av||wv){
  if(av&&wv)DO(f, if(s=av[i]-wv[i])break;) else s=av?-1:1;
  if     (0==s){RZ(zv[j++]=incorp(CALL2(f2,aa,ww,fs))); ICPY(iv,av,f); iv+=g;}
  else if(0> s){RZ(zv[j++]=incorp(CALL2(f2,aa,we,fs))); ICPY(iv,av,f); iv+=g;}
  else if(0< s){RZ(zv[j++]=incorp(CALL2(f2,ae,ww,fs))); ICPY(iv,wv,f); iv+=g;}
  if(0>=s){if(aj==an)av=0; else{av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;}}
  if(0<=s){if(wj==wn)wv=0; else{wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;}}
 }else while(av||wv){
  if(av&&wv&&f)DO(f, if(s=av[i]-wv[i])break;) else s=!f?0:av?-1:1;
  if(b&&0<s||!b&&0>s){RZ(zv[j++]=incorp(CALL2(f2,b?ae:aa,b?ww:we,fs))); ICPY(iv,b?wv:av,g); iv+=g; k=m;}
  else if(s){
   DQ(m, RZ(zv[j++]=y=incorp(y?ca(y):CALL2(f2,b?aa:ae,b?we:ww,fs))); ICPY(iv,b?av:wv,f); ICPY(iv+f,u,d); iv+=g; u+=d;); 
   u=ii; y=0; v=0;
  }else{
   while(ICMP(f+(b?wv:av),u,d)){
    RZ(zv[j++]=y=incorp(y?ca(y):CALL2(f2,b?aa:ae,b?we:ww,fs))); 
    ICPY(iv,wv,f); ICPY(iv+f,u,d); iv+=g; u+=d; ++k;
   }
   RZ(zv[j++]=incorp(CALL2(f2,aa,ww,fs))); ICPY(iv,b?wv:av,g); iv+=g; u+=d; ++k;
  }
  if     ( b&&0<=s)if(wj==wn)wv=v=0; else{v=wv; wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;}
  else if(!b&&0>=s)if(aj==an)av=v=0; else{v=av; av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;}
  if(b&&(!s&&!wv||v&&ICMP(v,wv,f))||!b&&(!s&&!av||v&&ICMP(v,av,f))){
   DQ(m-k, RZ(zv[j++]=y=incorp(y?ca(y):CALL2(f2,b?aa:ae,b?we:ww,fs))); ICPY(iv,b?av:wv,f); ICPY(iv+f,u,d); iv+=g; u+=d;);
   u=ii; y=0; k=0;
  }
  if     ( b&&0>=s&&(!v||ICMP(v,wv,f)))if(aj==an)av=0; else{av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;}
  else if(!b&&0<=s&&(!v||ICMP(v,av,f)))if(wj==wn)wv=0; else{wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;}
 }
 AN(z)=AS(z)[0]=AS(zi)[0]=j; AN(zi)=j*g;
 z=sprz(ope(z),zi,CALL2(f2,ae,we,fs),g,g==af?as:ws);
 EPILOG(z);
}    /* a f"r w on sparse arrays */

#if 0 // not working
A jtva2s(J jt,A a,A w,C id,VF ado,I cv,I t,I zt,I lr,I rr){PROLOG(0047);A aa,ae,we,ww,y,zi,z,*zv;B*ab,b,*wb;I ac,acr,af,aj,ak,am,an,
     ar,*as,at,*av,d,f,g,*ii,*iv,j,k,m,s,*u,*v,wc,wcr,wf,wj,wk,wm,wn,wr,*ws,wt,*wv;P*ap,*wp;
 ARGCHK2(a,w);
 ar=AR(a); as=AS(a); efr(acr,ar,lr); af=ar-acr; 
 wr=AR(w); ws=AS(w); efr(wcr,wr,rr); wf=wr-wcr; 
 if(!af&&!wf){ado(jt,a,w); R 0;}
 DO(af, ASSERT(as[i]!=0,EVNONCE););
 DO(wf, ASSERT(ws[i]!=0,EVNONCE););
 if(!ar){ado(jt,a,w); R 0;}
 if(!wr){ado(jt,a,w); R 0;}
 f=MIN(af,wf); g=MAX(af,wf); m=1;
 if(f<g){d=g-f; RZ(y=odom(2L,d,f+(af<wf?ws:as))); ii=AV(y); m=AS(y)[0];}
 ASSERT(!ICMP(as,ws,f),EVLENGTH);
 RZ(a=sprarg(af,a)); at=AT(a); ap=PAV(a);
 RZ(w=sprarg(wf,w)); wt=AT(w); wp=PAV(w);
 y=SPA(ap,i); v=AS(y); an=v[0]; ac=v[1]; av=an?AV(y):0; RZ(ab=spredge(y,af,&am));
 y=SPA(wp,i); v=AS(y); wn=v[0]; wc=v[1]; wv=wn?AV(y):0; RZ(wb=spredge(y,wf,&wm));
 RZ(aa=sprinit(af,acr,as,at,ap)); RZ(ae=acr?ca(aa):SPA(ap,e));
 RZ(ww=sprinit(wf,wcr,ws,wt,wp)); RZ(we=wcr?ca(ww):SPA(wp,e));
 b=af<wf; j=am*(af<wf?m:1)+wm*(af>wf?m:1);
 GATV0(z, BOX,MAX(1,j),  1); zv=AAV1(z);
 GATV0(zi,INT,MAX(1,j)*g,2); v=AS(zi); v[0]=j; v[1]=g; iv=AV2(zi); 
 RE(aj=ak=spradv(an,ab,af,acr,0L,ap,&aa));
 RE(wj=wk=spradv(wn,wb,wf,wcr,0L,wp,&ww)); j=s=k=0; u=ii; y=0; v=0;
 if(af==wf)while(av||wv){
  if(av&&wv)DO(f, if(s=av[i]-wv[i])break;) else s=av?-1:1;
  if     (0==s){ado(jt,aa,ww); RZ(zv[j++]); ICPY(iv,av,f); iv+=g;}
  else if(0> s){ado(jt,aa,we); RZ(zv[j++]); ICPY(iv,av,f); iv+=g;}
  else if(0< s){ado(jt,ae,ww); RZ(zv[j++]); ICPY(iv,wv,f); iv+=g;}
  if(0>=s){if(aj==an)av=0; else{av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;}}
  if(0<=s){if(wj==wn)wv=0; else{wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;}}
 }else while(av||wv){
  if(av&&wv&&f)DO(f, if(s=av[i]-wv[i])break;) else s=!f?0:av?-1:1;
  if(b&&0<s||!b&&0>s){ado(jt,b?ae:aa,b?ww:we); RZ(zv[j++]); ICPY(iv,b?wv:av,g); iv+=g; k=m;}
  else if(s){
   DQ(m, ado(jt,b?aa:ae,b?we:ww); RZ(zv[j++]=y=y?ca(y):0); ICPY(iv,b?av:wv,f); ICPY(iv+f,u,d); iv+=g; u+=d;); 
   u=ii; y=0; v=0;
  }else{
   while(ICMP(f+(b?wv:av),u,d)){
    ado(jt,b?aa:ae,b?we:ww); RZ(zv[j++]=y=y?ca(y):0); 
    ICPY(iv,wv,f); ICPY(iv+f,u,d); iv+=g; u+=d; ++k;
   }
   ado(jt,aa,ww); RZ(zv[j++]); ICPY(iv,b?wv:av,g); iv+=g; u+=d; ++k;
  }
  if     ( b&&0<=s)if(wj==wn)wv=v=0; else{v=wv; wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;}
  else if(!b&&0>=s)if(aj==an)av=v=0; else{v=av; av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;}
  if(b&&(!s&&!wv||v&&ICMP(v,wv,f))||!b&&(!s&&!av||v&&ICMP(v,av,f))){
   DQ(m-k, ado(jt,b?aa:ae,b?we:ww); RZ(zv[j++]=y=y?ca(y):0); ICPY(iv,b?av:wv,f); ICPY(iv+f,u,d); iv+=g; u+=d;);
   u=ii; y=0; k=0;
  }
  if     ( b&&0>=s&&(!v||ICMP(v,wv,f)))if(aj==an)av=0; else{av+=ak*ac; RE(ak=spradv(an,ab,af,acr,aj,ap,&aa)); aj+=ak;}
  else if(!b&&0<=s&&(!v||ICMP(v,av,f)))if(wj==wn)wv=0; else{wv+=wk*wc; RE(wk=spradv(wn,wb,wf,wcr,wj,wp,&ww)); wj+=wk;}
 }
 AN(z)=AS(z)[0]=AS(zi)[0]=j; AN(zi)=j*g;
 R 0;
 /* EPILOG(sprz(ope(z),zi,CALL2(f2,ae,we,fs),g,g==af?as:ws)); */
}    /* a f"r w on sparse arrays for atomic f */
#endif
