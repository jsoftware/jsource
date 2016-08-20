/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grades                                                           */

#include "j.h"
#include "vg.h"


/************************************************************************/
/*                                                                      */
/* merge sort with special code for n<:5                                */
/*                                                                      */
/************************************************************************/

static void jtmsmerge(J jt,I n,I*u,I*v){I m,q,*x,*xx,*y,*yy,*z;int c;
 q=n/2; z=v;
 x=u;   xx=u+q-1;
 y=u+q; yy=u+n-1;
 while(1){
  c=CALL1(jt->comp,*x,*y);
  if(0<c){*z++=*y++; if(y>yy){m=z-v; z=u+m; DO(1+xx-x, *z++=*x++;); DO(m, *u++=*v++;); break;}}
  else   {*z++=*x++; if(x>xx){m=z-v;                                DO(m, *u++=*v++;); break;}}
}}

#define VS(i,j)          (0<CALL1(jt->comp,u[i],u[j]))
#define XC(i,j)          {q=u[i]; u[i]=u[j]; u[j]=q;}
#define P3(i,j,k)        {ui=u[i]; uj=u[j]; uk=u[k]; u[0]=ui; u[1]=uj; u[2]=uk;}
#define P5(i,j,k,l,m)    {ui=u[i]; uj=u[j]; uk=u[k]; ul=u[l]; um=u[m];  \
                          u[0]=ui; u[1]=uj; u[2]=uk; u[3]=ul; u[4]=um;}

void jtmsort(J jt,I n,I*u,I*v){I a,b,c,d,q,ui,uj,uk,ul,um;
 switch(n){
  case 2: 
   if(VS(0,1))XC(0,1); 
   break;
  case 3:
   if(VS(0,1))XC(0,1);
   if(VS(1,2))if(VS(0,2))P3(2,0,1) else XC(1,2);
   break;
  case 4:
   if(VS(0,1))XC(0,1);
   if(VS(2,3))XC(2,3);
   if(VS(1,3)){XC(0,2); XC(1,3);}
   if(VS(1,2))if(VS(0,2))P3(2,0,1) else XC(1,2);
   break;
  case 5: 
   if(VS(0,1))XC(0,1);
   if(VS(2,3))XC(2,3);
   if(VS(1,3)){XC(0,2); XC(1,3);}
   if(VS(4,1))if(VS(4,3)){a=0; b=1; c=3; d=4;}else{a=0; b=1; c=4; d=3;}
   else       if(VS(4,0)){a=0; b=4; c=1; d=3;}else{a=4; b=0; c=1; d=3;}
   if(VS(2,b)){if(3!=c)if(VS(2,c))P5(a,b,c,2,d) else P5(a,b,2,c,d);}
   else       {        if(VS(2,a))P5(a,2,b,c,d) else P5(2,a,b,c,d);}
   break;
  default:
   if(5<n){
    q=n/2;
    msort(q,  u,  v); 
    msort(n-q,u+q,v);
    msmerge(n,u,v);
}}}


#define GF(f)         B f(J jt,I m,I c,I n,A w,I*zv)

/* m  - # cells (# individual grades to do) */
/* c  - # atoms in a cell                   */
/* n  - length of sort axis                 */
/* w  - array to be graded                  */
/* zv - result values                       */

static GF(jtgrx){A x;I ck,d,t,*xv;
 t=AT(w); ck=c*bp(t); 
 jt->compk=ck/n; d=c/n; jt->compn=d; jt->compv=CAV(w); jt->compw=w;
 switch(CTTZ(t)){
  case BOXX:  jt->comp=ARELATIVE(w)?compr:compa; break;
  case C2TX:  jt->comp=compu;                    break;
  case C4TX:  jt->comp=c==n?compt1:compt;        break;
  case INTX:  jt->comp=c==n?compi1:compi;        break;
  case FLX:   jt->comp=c==n?compd1:compd;        break;
  case CMPXX: jt->comp=compd; jt->compn=2*d;     break;
  case XNUMX: jt->comp=compx;                    break;
  case RATX:  jt->comp=compq;                    break;
  default:   jt->comp=compc;
 }
 GATV(x,INT,n,1,0); xv=AV(x);  /* work area for msmerge() */
 DO(m, DO(n, zv[i]=i;); msort(n,zv,xv); jt->compv+=ck; zv+=n;);
 R !jt->jerr;
}    /* grade"r w on general w */

/* grcol: grade/sort a halfword of an integer or a double      */
/* d:     # of possible different halfwords (65536 or 32768)   */
/* c:     smallest halfword value (0 or 32768)                 */
/* yv:    halfword buckets work area                           */
/* n:     # of sort elements                                   */
/* xv:    input permutation of sort elements                   */
/* zv:    output permutation                                   */
/* m:     # of halfwords in a sort element (2 or 4)            */
/* u:     ptr to sort elements                                 */
/* up:    1 if sort up; 0 if sort down                         */
/* split: 1 iff do split pass of halfword range                */
/* sort:  1 if sort; 0 if grade                                */

void grcol(I d,I c,I*yv,I n,I*xv,I*zv,const I m,US*u,int up,int split,int sort){
     D*xx,*zz;I k,s,*t;US*v;
 s=0; memset(c+yv,C0,d*SZI); 
 v=u; 
 DO(n, ++yv[*v]; v+=m;);
 switch(up+2*split){
  case 0: t=yv+c+d;     DO(d,   --t; if(k=*t){*t=s; s+=k;}); break;
  case 1: t=yv+c-1;     DO(d,   ++t; if(k=*t){*t=s; s+=k;}); break;
  case 2: t=yv+c+d/2;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); 
          t=yv+c+d  ;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); break;
  case 3: t=yv+c+d/2-1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;}); 
          t=yv+c    -1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;});
 }
 v=u;
 if(sort){
  if(2==m)                       DO(n, zv[yv[*v        ]++]=xv[i]; v+=m;)
  else    {zz=(D*)zv; xx=(D*)xv; DO(n, zz[yv[*v        ]++]=xx[i]; v+=m;);}
 }else if(!xv)                   DO(n, zv[yv[*v        ]++]=   i ; v+=m;)
 else                            DO(n, zv[yv[v[m*xv[i]]]++]=xv[i];      );
}

void grcolu(I d,I c,UI*yv,I n,UI*xv,UI*zv,const I m,US*u,int up,int split,int sort){
     C4*xx,*zz;UI k,s,*t;US*v;
 s=0; memset(c+yv,C0,d*SZI); 
 v=u; 
 DO(n, ++yv[*v]; v+=m;);
 switch(up+2*split){
  case 0: t=yv+c+d;     DO(d,   --t; if(k=*t){*t=s; s+=k;}); break;
  case 1: t=yv+c-1;     DO(d,   ++t; if(k=*t){*t=s; s+=k;}); break;
  case 2: t=yv+c+d/2;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); 
          t=yv+c+d  ;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); break;
  case 3: t=yv+c+d/2-1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;}); 
          t=yv+c    -1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;});
 }
 v=u;
 if(sort){
  if(sizeof(I)==sizeof(C4))      DO(n, zv[yv[*v        ]++]=xv[i]; v+=m;)
  else  {zz=(C4*)zv; xx=(C4*)xv; DO(n, zz[yv[*v        ]++]=xx[i]; v+=m;);}
 }else if(!xv)                   DO(n, zv[yv[*v        ]++]=   i ; v+=m;)
 else                            DO(n, zv[yv[v[m*xv[i]]]++]=xv[i];      );
}

static GF(jtgrd){A x,y;B b;D*v,*wv;I d,e,*g,*h,i,k,p,q,*xv,*yv;int up;US*u;
 if(!(c==n&&n>65536/3.5))R grx(m,c,n,w,zv);
 p=65536; q=p/2; up=1==jt->compgt; wv=DAV(w);
 GATV(y,INT,p,1,0); yv=AV(y);
 GATV(x,INT,n,1,0); xv=AV(x);
#if C_LE
 d= 1; e=0;
#else
 d=-1; e=3;
#endif
 for(i=0;i<m;++i){
  u=e+(US*)wv; 
  v=wv; k=0; DO(n, if(0>*v++)++k;); b=0<k&&k<n;
  g=b?xv:zv; h=b?zv:xv;
  grcol(p,    0L,      yv,n,0L,h,sizeof(D)/sizeof(US),u+0*d,k==n?!up:up,0,0);
  grcol(p,    0L,      yv,n,h, g,sizeof(D)/sizeof(US),u+1*d,k==n?!up:up,0,0);
  grcol(p,    0L,      yv,n,g, h,sizeof(D)/sizeof(US),u+2*d,k==n?!up:up,0,0);
  grcol(b?p:q,k==n?q:0,yv,n,h, g,sizeof(D)/sizeof(US),u+3*d,k==n?!up:up,0,0);
  if(b){D d;I j,m,*u,*v,*vv;
   if(up){ICPY(k+zv,  xv,n-k); u=zv;     v=n+xv;}
   else  {ICPY(  zv,k+xv,n-k); u=zv+n-k; v=k+xv;}
   j=0; d=wv[*(v-1)];
   DO(1+k, --v; if(d!=wv[*v]){vv=1+v; m=i-j; DO(m, *u++=*vv++;); j=i; d=wv[*v];});
  }
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on real w; main code here is for c==n */

static GF(jtgri1){A x,y;I*wv;I d,e,i,p,*xv,*yv;int up;US*u;
 p=65536; up=1==jt->compgt; wv=AV(w);
 GATV(y,INT,p,1,0); yv=AV(y);
 GATV(x,INT,n,1,0); xv=AV(x);
 e=SY_64?3:1;
#if C_LE
  d= 1; 
#else
  d=-1;
#endif
 for(i=0;i<m;++i){
  u=e*(-1==d)+(US*)wv;
  grcol(p,0L,yv,n,0L,xv,sizeof(I)/sizeof(US),u,    up,0,0);
#if SY_64
  grcol(p,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),u+1*d,up,0,0);
  grcol(p,0L,yv,n,zv,xv,sizeof(I)/sizeof(US),u+2*d,up,0,0);
#endif
  grcol(p,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),u+e*d,up,1,0);
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on integer w where c==n */

static GF(jtgru1){A x,y;B b;C4*v,*wv;I d,e,i,k,p,*xv,*yv;UI*g,*h;int up;US*u;
 p=65536; up=1==jt->compgt; wv=C4AV(w);
 GATV(y,INT,p,1,0); yv=AV(y);
 GATV(x,INT,n,1,0); xv=AV(x);
#if C_LE
 d= 1; e=0;
#else
 d=-1; e=1;
#endif
 for(i=0;i<m;++i){
  u=e+(US*)wv; 
  v=wv; k=0; b=0;
  g=b?xv:zv; h=b?zv:xv;
  grcolu(p, 0L, yv,n,0L,h,sizeof(C4)/sizeof(US),u+0*d,k==n?!up:up,0,0);
  grcolu(p, 0L, yv,n, h,g,sizeof(C4)/sizeof(US),u+1*d,k==n?!up:up,0,0);
  if(b){C4 d;I j,m,*u,*v,*vv;
   if(up){ICPY(k+zv,  xv,n-k); u=zv;     v=n+xv;}
   else  {ICPY(  zv,k+xv,n-k); u=zv+n-k; v=k+xv;}
   j=0; d=wv[*(v-1)];
   DO(1+k, --v; if(d!=wv[*v]){vv=1+v; m=i-j; DO(m, *u++=*vv++;); j=i; d=wv[*v];});
  }
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on c4t w where c==n */

// returns *base = smallest value, *top = #values (1..2 is 2 values)
// returns 0 for *top if range is not representable in an integer
void irange(I n,I*v,I*base,I*top){I d,i,m=n/2,p,q,x,y;
 if(n>m+m)p=q=*v++; else if(n){q=IMAX; p=IMIN;}else p=q=0;
 for(i=0;i<m;++i){
  x=*v++; y=*v++; 
  if(x<y){if(x<q)q=x; if(p<y)p=y;}
  else   {if(y<q)q=y; if(p<x)p=x;}
 }
 *base=q; d=p-q; *top=0>d||d==IMAX?0:1+d;
}    /* min and max in 1.5*n comparisons */

// copy of irange for sizeof(C4)==sizeof(int)
void c4range(I n,C4*v,C4*base,I*top){I d,i,m=n/2;C4 p,q,x,y;
 if(n>m+m)p=q=*v++; else if(n){q=C4MAX; p=C4MIN;}else p=q=0;
 for(i=0;i<m;++i){
  x=*v++; y=*v++; 
  if(x<y){if(x<q)q=x; if(p<y)p=y;}
  else   {if(y<q)q=y; if(p<x)p=x;}
 }
 *base=q; d=(I)p-(I)q; *top=0>d||d>=IMAX?0:1+d;
}    /* min and max in 1.5*n comparisons */

F1(jtmaxmin){I base,top;
 RZ(w);
 ASSERT(INT&AT(w),EVDOMAIN);
 irange(AN(w),AV(w),&base,&top);
 R v2(base,base+top-1);
}

static GF(jtgri){A x,y;B b,up;I d,e,*g,*h,i,j,k,p,ps,q,s,*v,*wv,*xv,*yv;
 wv=AV(w); d=c/n; k=4*n;
 irange(AN(w),wv,&q,&p); 
 if(!p||k<p||(0.69*d*(p+2*n))>n*log((D)n))R c==n&&n>65536/1.5?gri1(m,c,n,w,zv):grx(m,c,n,w,zv);
 if(0<q&&q<k-p){p+=q; q=0;}
 GATV(y,INT,p,1,0); yv=AV(y); ps=p*SZI; up=1==jt->compgt;
 if(1<d){GATV(x,INT,n,1,0); xv=AV(x);}
 for(i=0;i<m;++i){
  s=0; j=p; memset(yv,C0,ps);
  v=wv+d-1;
  if(q) DO(n, ++yv[*v-q]; v+=d;) 
  else  DO(n, ++yv[*v  ]; v+=d;);
  if(up)DO(p,      if(k=yv[i]){yv[i]=s; s+=k;}) 
  else  DO(p, --j; if(k=yv[j]){yv[j]=s; s+=k;});
  v=wv+d-1;
  if(q) DO(n, zv[yv[*v-q]++]=i; v+=d;) 
  else  DO(n, zv[yv[*v  ]++]=i; v+=d;);
  v=wv+d-1;
  for(e=d-2,b=0;0<=e;--e){
   --v;
   if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}
   s=0; j=p; memset(yv,C0,ps);
   if(q) DO(n, ++yv[*(v+d*g[i])-q];) 
   else  DO(n, ++yv[*(v+d*g[i])  ];);
   if(up)DO(p,      if(k=yv[i]){yv[i]=s; s+=k;}) 
   else  DO(p, --j; if(k=yv[j]){yv[j]=s; s+=k;});
   if(q) DO(n, h[yv[*(v+d*g[i])-q]++]=g[i];) 
   else  DO(n, h[yv[*(v+d*g[i])  ]++]=g[i];);
  }
  if(b)DO(n, zv[i]=xv[i];);
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on small-range integers w */


static GF(jtgru){A x,y;B b,up;I d,e,i,j,k,p,ps;C4 s,*v,q,*wv;UI*g,*h,*xv,*yv;
 wv=C4AV(w); d=c/n; k=4*n;
 c4range(AN(w),wv,&q,&p); 
 if(!p||k<p||(0.69*d*(p+2*n))>n*log((D)n))R c==n&&n>65536/1.5?gru1(m,c,n,w,zv):grx(m,c,n,w,zv);
 if(0<q&&(I)q<k-p){p+=q; q=0;}
 GATV(y,INT,p,1,0); yv=(UI*)AV(y); ps=p*SZI; up=1==jt->compgt;
 if(1<d){GATV(x,INT,n,1,0); xv=(UI*)AV(x);}
 for(i=0;i<m;++i){C4 k;
  s=0; j=p; memset(yv,C0,ps);
  v=wv+d-1;
  if(q) DO(n, ++yv[*v-q]; v+=d;) 
  else  DO(n, ++yv[*v  ]; v+=d;);
  if(up)DO(p,      if(k=(C4)yv[i]){yv[i]=s; s+=k;}) 
  else  DO(p, --j; if(k=(C4)yv[j]){yv[j]=s; s+=k;});
  v=wv+d-1;
  if(q) DO(n, zv[yv[*v-q]++]=i; v+=d;) 
  else  DO(n, zv[yv[*v  ]++]=i; v+=d;);
  v=wv+d-1;
  for(e=d-2,b=0;0<=e;--e){
   --v;
   if(b){g=xv; h=(UI*)zv; b=0;}else{g=(UI*)zv; h=xv; b=1;}
   s=0; j=p; memset(yv,C0,ps);
   if(q) DO(n, ++yv[*(v+d*g[i])-q];) 
   else  DO(n, ++yv[*(v+d*g[i])  ];);
   if(up)DO(p,      if(k=(C4)yv[i]){yv[i]=s; s+=k;}) 
   else  DO(p, --j; if(k=(C4)yv[j]){yv[j]=s; s+=k;});
   if(q) DO(n, h[yv[*(v+d*g[i])-q]++]=g[i];) 
   else  DO(n, h[yv[*(v+d*g[i])  ]++]=g[i];);
  }
  if(b)DO(n, zv[i]=xv[i];);
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on small-range c4t w */


#define DOCOL1(p,iicalc0,iicalc1,ind,vinc)  \
 {I*g,*h,   j=p-1,k,s=0;UC*v;                          \
  if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}        \
  memset(yv,C0,ps);                                    \
  v=vv; DO(n, ++yv[iicalc0]; v+=d;);                   \
  if(up)DO(p, k=yv[i]; yv[i  ]=s; s+=k;)               \
  else  DO(p, k=yv[j]; yv[j--]=s; s+=k;);              \
  v=vv; DO(n, h[yv[iicalc1]++]=ind;           vinc;);  \
 }

#define DOCOL4(p,iicalc0,iicalc1,ind,vinc)  \
 {I*g,*h,ii,j=p-1,k,s=0;UC*v;                          \
  if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}        \
  memset(yv,C0,ps);                                    \
  v=vv; DO(n, IND4(iicalc0); ++yv[ii]; v+=d;);         \
  if(up)DO(p, k=yv[i]; yv[i  ]=s; s+=k;)               \
  else  DO(p, k=yv[j]; yv[j--]=s; s+=k;);              \
  v=vv; DO(n, IND4(iicalc1); h[yv[ii]++]=ind; vinc;);  \
 }

static GF(jtgrb){A x;B b,up;I d,i,p,ps,q,*xv,yv[16];UC*vv,*wv;
 if(c>4*n*log((D)n))R grx(m,c,n,w,zv); 
 d=c/n; q=d/4; p=16; ps=p*SZI; wv=UAV(w); up=1==jt->compgt;
 if(1<q){GATV(x,INT,n,1,0); xv=AV(x);}
 for(i=0;i<m;++i){
  vv=wv+d; b=1&&q%2;
  if(q){   vv-=4; DOCOL4(p, *(int*)v, *(int*)v,         i,   v+=d);}
  DO(q-1,  vv-=4; DOCOL4(p, *(int*)v, *(int*)(v+d*g[i]),g[i],v   ););
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on boolean w, works 4 columns at a time (d%4 guaranteed to be 0)*/

static GF(jtgrc){A x;B b,q,up;I d,e,i,p,ps,*xv,yv[256];UC*vv,*wv;
 d=C2T&AT(w)?2*c/n:c/n;
 if(d>log((D)n))R grx(m,c,n,w,zv); 
 p=B01&AT(w)?2:256; ps=p*SZI; wv=UAV(w); up=1==jt->compgt;
 q=C2T&AT(w) && C_LE;
 if(1<d){GATV(x,INT,n,1,0); xv=AV(x);}
 for(i=0;i<m;++i){
  b=(B)(d%2); if(q){e=-3; vv=wv+d-2;}else{e=-1; vv=wv+d-1;}
                 DOCOL1(p,*v,*v,       i,   v+=d); if(q)e=1==e?(q==1?-3:-5):1; 
  DO(d-1, vv+=e; DOCOL1(p,*v,v[d*g[i]],g[i],v   ); if(q)e=1==e?(q==1?-3:-5):1;);
  wv+=d*n; zv+=n;
 }
 R 1;
}    /* grade"r w on boolean or char or unicode w */

static GF(jtgrs){R gri(m,c,n,sborder(w),zv);}    
     /* grade"r w on symbols w */

F2(jtgrade1p){PROLOG(0074);A x,z;I n,*s,*xv,*zv;
 s=AS(w); n=s[0]; jt->compn=s[1]-1; jt->compk=SZI*s[1];
 jt->comp=compp; jt->compsyv=AV(a); jt->compv=CAV(w);
 GATV(z,INT,n,1,0); zv=AV(z); DO(n, zv[i]=i;);
 GATV(x,INT,n,1,0); xv=AV(x);
 msort(n,zv,xv);
 EPILOG(z);
}    /* /:(}:a){"1 w , permutation a, integer matrix w */


/************************************************************************/
/*                                                                      */
/* /: and \: main control                                               */
/*                                                                      */
/************************************************************************/

F1(jtgr1){PROLOG(0075);A z;I c,f,m,n,r,*s,t,wr,zn;
 RZ(w);
 t=AT(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; jt->rank=0;
 f=wr-r; s=AS(w); m=prod(f,s); c=m?AN(w)/m:prod(r,f+s); n=r?s[f]:1;
 RE(zn=mult(m,n)); GATV(z,INT,zn,1+f,s); if(!r)*(AS(z)+f)=1;
 if(!c||1>=n)R reshape(shape(z),IX(n));
 if     (t&B01&&0==(c/n)%4)RZ(grb(m,c,n,w,AV(z)))
 else if(t&SBT            )RZ(grs(m,c,n,w,AV(z)))
 else if(t&FL             )RZ(grd(m,c,n,w,AV(z)))
 else if(t&INT            )RZ(gri(m,c,n,w,AV(z)))
 else if(t&IS1BYTE+C2T)    RZ(grc(m,c,n,w,AV(z)))
 else if(t&C4T)            RZ(gru(m,c,n,w,AV(z)))
 else                      RZ(grx(m,c,n,w,AV(z)));
 EPILOG(z);
}    /*   grade"r w main control for dense w */

#define GBEGIN(G,L)  A z;int ogt=jt->compgt,olt=jt->complt; jt->compgt=G; jt->complt=L
#define GEND(z)      jt->compgt=ogt; jt->complt=olt; R z

F1(jtgrade1 ){GBEGIN( 1,-1); RZ(   w); z=SPARSE&AT(w)?grd1sp(  w):gr1(  w); GEND(z);}
F1(jtdgrade1){GBEGIN(-1, 1); RZ(   w); z=SPARSE&AT(w)?grd1sp(  w):gr1(  w); GEND(z);}
F2(jtgrade2 ){GBEGIN( 1,-1); RZ(a&&w); z=SPARSE&AT(w)?grd2sp(a,w):gr2(a,w); GEND(z);}
F2(jtdgrade2){GBEGIN(-1, 1); RZ(a&&w); z=SPARSE&AT(w)?grd2sp(a,w):gr2(a,w); GEND(z);}


#define OSGT(i,j) (u[i]>u[j])

#define SORT4   \
 switch(n){                                                  \
  case 2:                                                    \
   if(OSGT(0,1))XC(0,1); break;                              \
  case 3:                                                    \
   if(OSGT(0,1))XC(0,1);                                     \
   if(OSGT(1,2))if(OSGT(0,2))P3(2,0,1) else XC(1,2); break;  \
  case 4:                                                    \
   if(OSGT(0,1))XC(0,1);                                     \
   if(OSGT(2,3))XC(2,3);                                     \
   if(OSGT(1,3)){XC(0,2); XC(1,3);}                          \
   if(OSGT(1,2))if(OSGT(0,2))P3(2,0,1) else XC(1,2);         \
 }

#define OSLOOP(T,ATOMF)  \
{T p0,p1,q,*tv,*u,ui,uj,uk,*v,*wv;                                                     \
  tv=wv=(T*)AV(w);                                                                     \
  while(1){                                                                            \
   if(4>=n){u=tv; SORT4; R ATOMF(tv[j]);}                                              \
   p0=tv[qv[i]%n]; ++i; if(i==qn)i=0;                                                  \
   p1=tv[qv[i]%n]; ++i; if(i==qn)i=0; if(p0>p1){q=p0; p0=p1; p1=q;}                    \
   if(p0==p1){m0=m1=0; v=tv; DO(n, if(p0>*v)++m0;                     ++v;);}          \
   else      {m0=m1=0; v=tv; DO(n, if(p0>*v)++m0; else if(p1>*v)++m1; ++v;);}          \
   c=m0+m1; m=j<m0?m0:j<c?m1:n-c;                                                      \
   if(t)u=v=tv; else{GA(t,wt,m,1,0); u=tv=(T*)AV(t); v=wv;}                            \
   if     (j<m0){       DO(n, if(*v<p0        )*u++=*v; ++v;); n=m;}                   \
   else if(j<c ){j-=m0; DO(n, if(p0<=*v&&*v<p1)*u++=*v; ++v;); n=m;}                   \
   else if(c   ){j-=c;  DO(n, if(p1<=*v       )*u++=*v; ++v;); n=m;}                   \
   else{DO(n, if(p1<*v)*u++=*v; ++v;); m=u-tv; c=n-m; if(j<c)R ATOMF(p1); j-=c; n=m;}  \
 }}

F2(jtordstat){A q,t=0;I c,i=0,j,m,m0,m1,n,qn=53,*qv,wt;
 RZ(a&&w);
 n=AN(w); wt=AT(w);
 if(!(!AR(a)&&AT(a)&B01+INT&&4<n&&1==AR(w)&&wt&FL+INT))R from(a,grade2(w,w));
 RE(j=i0(a)); if(0>j)j+=n;
 ASSERT(0<=j&&j<n,EVINDEX);
 RZ(q=df2(sc(qn),sc(IMAX),atop(ds(CQUERY),ds(CDOLLAR)))); qv=AV(q);
 if(wt&FL)OSLOOP(D,scf) else OSLOOP(I,sc);
}    /* a{/:~w */

F2(jtordstati){A t;I n,wt;
 RZ(a&&w);
 n=AN(w); wt=AT(w);
 if(!(!AR(a)&&AT(a)&B01+INT&&4<n&&1==AR(w)&&wt&FL+INT))R from(a,grade1(w));
 RZ(t=ordstat(a,w));
 I j=0;  // =0 needed to stifle warning
 if(wt&FL){D p=*DAV(t),*v=DAV(w); DO(n, if(p==*v++){j=i; break;});}
 else     {I p=* AV(t),*v= AV(w); DO(n, if(p==*v++){j=i; break;});}
 R sc(j);
}    /* a {/:w */
