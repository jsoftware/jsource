/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Format: ": Monad                                                        */

#include "j.h"

#if SY_64
#define WI          21L
#else
#define WI          12L
#endif

#define WD          (9L+NPP)
#define WZ          (WD+WD)
#define FMTF(f,T)   void f(J jt,C*s,T*v)
#define ENGAP(j,r,s,exp)  \
 {B b;I k=1,p=j,*sr=s+r-2; DO(p?r-1:0, k*=*(sr-i); b=!(p%k); exp;); }

static F1(jtthxqe);

static FMTF(jtfmtI,I){I x=*v;
 sprintf(s,FMTI,x);
 if('-'==*s)*s=CSIGN;
}

static FMTF(jtfmtD,D){B q;C buf[1+WD],c,*t;D x=*v;I k=0;
 if(!memcmp(v,&inf, SZD)){strcpy(s,"_" ); R;}
 if(!memcmp(v,&infm,SZD)){strcpy(s,"__"); R;}
 if(_isnan(*v)          ){strcpy(s,"_."); R;}
 x=*v; x=x==*(D*)minus0?0.0:x;  /* -0 to 0*/
 sprintf(buf,jt->pp,x);
 c=*buf; if(q=c=='-')*s++=CSIGN; q=q||(c=='+');
 if('.'==buf[q])*s++='0';
 MC(s,buf+q,WD+1-q);
 if(t=strchr(s,'e')){
  if('-'==*++t)*t++=CSIGN;
  while(c=*(k+t),c=='0'||c=='+')k++;
  if(k)while(*t=*(k+t))t++;
}}

static FMTF(jtfmtZ,Z){fmtD(s,&v->re); if(v->im){I k=strlen(s); *(k+s)='j'; fmtD(1+k+s,&v->im);}}

static void thcase(I t,I*wd,VF*fmt){
 switch(t){
  case CMPX: *wd=WZ; *fmt=jtfmtZ; break;
  case FL:   *wd=WD; *fmt=jtfmtD; break;
  default:   *wd=WI; *fmt=jtfmtI;
}}

I jtthv(J jt,A w,I n,C*s){A t;B ov=0;C buf[WZ],*x,*y=s;I k,n4=n-4,p,wd,wn,wt;VF fmt;
 RZ(w&&n);
 wn=AN(w); wt=AT(w); x=CAV(w); thcase(wt,&wd,&fmt);
 switch(wt){
  case XNUM: case RAT:
   RZ(t=thxqe(w)); p=AN(t); if(ov=n<p)p=n4; MC(y,AV(t),p); y+=p; break;
  case B01:
   if(ov=n<2*wn)p=n4/2; else p=wn; DO(p, *y++=*x++?'1':'0'; *y++=' ';); break;
  case INT:
	{C*t;I i,*v,x;
	v=AV(w);
    for(i=0;i<wn;++i){
     t=buf; x=*v++;
     sprintf(t,FMTI" ",x);
	 if('-'==*t)*t=CSIGN;
     p=strlen(t); if(ov=n4<p+y-s)break; strcpy(y,t); y+=p;
	}}
   break;
  default:
   k=bp(wt);
   if(n>=wn*wd)DO(wn, fmt(jt,y,x); y+=strlen(y); *y++=' '; x+=k;)
   else        DO(wn, fmt(jt,buf,x); p=strlen(buf); if(ov=n4<1+p+y-s)break; strcpy(y,buf); y+=p; *y++=' '; x+=k;);
 }
 if(ov){if(' '!=*(y-1))*y++=' '; memset(y,'.',3L); y+=3;}
 else if(' '==*(y-1))--y; 
 *y=0; R y-s;
}

static F1(jtthbit){A z;UC*x;C*y;I c,i,m,n,p,q,r,r1,*s;
 n=AN(w); r=AR(w); s=AS(w);
 c=r?s[r-1]:1; m=n/c; p=2*c-1;
 GA(z,LIT,m*p,r+!r,s); *(AS(z)+AR(z)-1)=p; 
 x=UAV(w); y=CAV(z);
 q=c/BB; r=c%BB; r1=c%BW?(BW-c%BW)/BB:0;
 for(i=0;i<m;++i){
  DO(q-!r, memcpy(y,bitdisp+2*BB**x,2*BB  ); ++x; y+=2*BB  ;);
  if(r)   {memcpy(y,bitdisp+2*BB**x,2*r -1); ++x; y+=2*r -1;}
  else    {memcpy(y,bitdisp+2*BB**x,2*BB-1); ++x; y+=2*BB-1;}
  x+=r1;
 }
 R z;
}

static F1(jtthb){A z;B*x;C*y;I c,m,n,p,r,*s;
 n=AN(w); r=AR(w); s=AS(w);
 c=r?s[r-1]:1; m=n/c; p=2*c-1;
 GA(z,LIT,m*p,r+!r,s); *(AS(z)+AR(z)-1)=p; 
 x=BAV(w); y=CAV(z);
 DO(m, DO(c-1, *y++=*x++?'1':'0'; *y++=' ';); *y++=*x++?'1':'0';);
 R z;
}

static F1(jtthn){A d,t,z;C*tv,*x,*y,*zv;I c,*dv,k,m,n,p,r,*s,wd;VF fmt;
 n=AN(w); r=AR(w); s=AS(w);
 thcase(AT(w),&wd,&fmt);
 GA(t,LIT,wd*(1+n),1,0); tv=CAV(t);
 if(1>=r){p=thv(w,AN(t),tv); ASSERTSYS(p,"thn"); AN(t)=*AS(t)=p; z=t;} 
 else{ 
  c=s[r-1]; m=n/c; k=bp(AT(w));
  y=tv-wd; x=CAV(w)-k; 
  RZ(d=apv(c,1L,0L)); dv=AV(d);
  DO(m, DO(c, fmt(jt,y+=wd,x+=k); p=strlen(y); dv[i]=MAX(dv[i],p);););
  --dv[c-1]; p=0; DO(c, p+=++dv[i];);
  GA(z,LIT,m*p,r+!r,s); *(AS(z)+AR(z)-1)=p; zv=CAV(z); memset(zv,' ',AN(z));
  y=tv; DO(m, DO(c, zv+=dv[i]; p=strlen(y); MC(zv-p-(c>1+i),y,p); y+=wd;););
 }
 R z;
}

static F1(jtthsb){A d,z;C*zv;I c,*dv,m,n,p,q,r,*s;SB*x,*y;SBU*u;
 n=AN(w); r=AR(w); s=AS(w); x=y=SBAV(w); q=jt->sbun;
 if(1>=r){
  c=n; 
  p=2*n-1; DO(c, p+=SBUV(*x++)->n;);
  GA(z,LIT,  p,1,   0); zv=CAV(z); memset(zv,' ',AN(z));
        DO(c, u=SBUV(*y++); *zv='`'; MC(1+zv,SBSV(u->i),u->n); zv+=2+u->n;);
 }else{
  c=s[r-1]; m=n/c; RZ(d=apv(c,0L,0L)); dv=AV(d);
  DO(m,    DO(c, p =SBUV(*x++)->n; dv[i]=MAX(dv[i],p);););
  p=-1; DO(c, p+=dv[i]+=2;); --dv[c-1];
  GA(z,LIT,m*p,r+!r,s); zv=CAV(z); memset(zv,' ',AN(z)); *(AS(z)+AR(z)-1)=p;
  DO(m, DO(c, u=SBUV(*y++); *zv='`'; MC(1+zv,SBSV(u->i),u->n); zv+=dv[i];););
 }
 R z;
}

static F1(jtthx1){A z;B b;C*s,s1[2+XBASEN];I n,p,p1,*v;
 n=AN(w); v=AV(w)+n-1; b=0>*v; 
 p=*v; if(p==XPINF)R cstr("_"); else if(p==XNINF)R cstr("__");
 sprintf(s1,FMTI,*v); p1=strlen(s1);
 p=p1+XBASEN*(n-1);
 GA(z,LIT,p,1,0); s=CAV(z); 
 MC(s,s1,p1); if(b)*s=CSIGN; s+=p1; 
 DO(n-1, --v; sprintf(s,FMTI04,b?-*v:*v); s+=XBASEN;);
 R z;           
}

static A jtthq1(J jt,Q y){A c,d,z;B b;C*zv;I m,n=-1;
 RZ(c=thx1(y.n)); m=AN(c);
 d=y.d;
 if(b=1<AN(d)||1!=*AV(d)){RZ(d=thx1(y.d)); n=AN(d);}
 GA(z,LIT,m+n+1,1,0); zv=CAV(z);
 MC(zv,AV(c),m); if(b){*(zv+m)='r'; MC(zv+m+1,AV(d),n);}
 R z;
}

static A jtthdx1(J jt,DX y){A x,z;B b;C*s,s1[2+XBASEN],s2[20];I e,n,p,p1,p2,*v;
 e=y.e-1; x=y.x; p=y.p;
 n=AN(x); v=AV(x)+n-1; b=0>*v; 
 if(p==DXINF)R cstr("_"); else if(p==DXMINF)R cstr("__");
 sprintf(s1,FMTI,b?-*v:*v); p1=strlen(s1);
 if(e&&*v){s=s2; *s++='e'; if(0>e)*s++=CSIGN; sprintf(s,FMTI,0<e?e:-e); p2=strlen(s2);}else p2=0; 
 GA(z,LIT,b+p1+(1<p1)+XBASEN*(n-1)+p2,1,0); s=CAV(z);
 if(b)*s++=CSIGN; *s++=*s1; if(1<p1){*s++='.'; MC(s,1+s1,p1-1); s+=p1-1;}
 DO(n-1, --v; sprintf(s,FMTI04,b?-*v:*v); s+=XBASEN;);
 MC(s,s2,p2);
 R z;
}

static F1(jtthxqe){A d,t,*tv,*v,y,z;C*zv;I c,*dv,m,n,p,r,*s,*wv;
 n=AN(w); r=AR(w); s=AS(w); wv=AV(w);
 c=r?s[r-1]:1; m=n/c;
 GA(t,BOX,n,1,0); tv=AAV(t);
 RZ(d=apv(c,1L,0L)); dv=AV(d); v=tv;
 switch(AT(w)){
  case XNUM: {X*u =(X*) wv; DO(m, DO(c, RZ(*v++=y=thx1(*u++));  dv[i]=MAX(dv[i],AN(y));));} break;
  case RAT:  {Q*u =(Q*) wv; DO(m, DO(c, RZ(*v++=y=thq1(*u++));  dv[i]=MAX(dv[i],AN(y));));} break;
#ifdef UNDER_CE
  default: 
   if (AT(w)==XD){DX*u=(DX*)wv; DO(m, DO(c, RZ(*v++=y=thdx1(*u++)); dv[i]=MAX(dv[i],AN(y));));}
   else          {ZX*u=(ZX*)wv; ASSERT(0,EVNONCE);}
   break;
#else
  case XD:   {DX*u=(DX*)wv; DO(m, DO(c, RZ(*v++=y=thdx1(*u++)); dv[i]=MAX(dv[i],AN(y));));} break;
  case XZ:   {ZX*u=(ZX*)wv; ASSERT(0,EVNONCE);} break;
#endif
 }
 --dv[c-1];
 p=0; DO(c, p+=++dv[i];);
 GA(z,LIT,m*p,r+!r,s); *(AS(z)+AR(z)-1)=p; zv=CAV(z); memset(zv,' ',AN(z));
 v=tv; DO(m, DO(c, zv+=dv[i]; y=*v++; p=AN(y); MC(zv-p-(c>1+i),AV(y),p);));
 R z;
}


static B jtrc(J jt,A w,A*px,A*py){A*v,x,y;I j,k,r,*s,xn,*xv,yn,*yv;
 RZ(w);
 r=AR(w); s=AS(w); v=AAV(w);
 xn=1<r?s[r-2]:1; RZ(*px=x=apv(1+xn,0L,0L)); xv=AV(x);
 yn=  r?s[r-1]:1; RZ(*py=y=apv(1+yn,0L,0L)); yv=AV(y);
 DO(AN(w), s=AS(*v++); j=i/yn%xn; k=i%yn; xv[j]=MAX(xv[j],s[0]); yv[k]=MAX(yv[k],s[1]););
 DO(xn, ASSERT(xv[i]<IMAX,EVLIMIT); ++xv[i];); 
 DO(yn, ASSERT(yv[i]<IMAX,EVLIMIT); ++yv[i];);
 R 1;
}

static void jtfram(J jt,I k,I n,I*x,C*v){C a,b=9==k,d,l,r;
 l=jt->bx[k]; a=b?' ':jt->bx[10]; d=b?l:jt->bx[1+k]; r=b?l:jt->bx[2+k];
 *v++=l; DO(n, memset(v,a,x[i]-1); v+=x[i]-1; *v++=d;); *--v=r;
}

static void jtfminit(J jt,I m,I ht,I wd,A x,A y,C*zv){C*u,*v;I p,xn,*xv,yn,*yv;
 p=ht*wd;
 xn=AN(x)-1; xv=AV(x);
 yn=AN(y)-1; yv=AV(y);
 fram(9L,yn,yv,zv); u=zv; DO(ht-2, MC(u+=wd,zv,wd););
 fram(3L,yn,yv,u=v=zv+wd**xv); DO(xn-1, MC(u+=wd*xv[1+i],v,wd););
 fram(0L,yn,yv,zv);
 fram(6L,yn,yv,zv+p-wd);
 u=zv; DO(m-1, MC(u+=p,zv,p););
}    /* Initialize with box-drawing characters */

static void jtfmfill(J jt,I p,I q,I wd,A w,A x,A y,C*zv){A e,*wv;C*u,*v;
  I c,d,i,j,k,n,r,*s,xn,xp,*xv,yn,yp,*yv;
 n=AN(w); wv=AAV(w);
 xp=jt->pos[0]; yp=jt->pos[1];
 xn=AN(x)-1; xv=AV(x); j=1; DO(1+xn, k=xv[i]; xv[i]=j; j+=k;);
 yn=AN(y)-1; yv=AV(y); j=1; DO(1+yn, k=yv[i]; yv[i]=j; j+=k;);
 for(i=0;i<n;++i){
  j=i/yn%xn; k=i%yn; d=i/q*p+wd*xv[j]+yv[k];
  e=wv[i]; s=AS(e); r=s[0]; c=s[1];
  if(xp)d+=(xv[1+j]-xv[j]-1-r)/(3-xp)*wd;
  if(yp)d+=(yv[1+k]-yv[k]-1-c)/(3-yp);
  u=zv+d-wd; v=CAV(e)-c; DO(r, MC(u+=wd,v+=c,c););
}}   /* fill each cell */

static F1(jtenframe){A x,y,z;C*zv;I ht,m,n,p,q,wd,wr,xn,*xv,yn,*yv,zn;
 RE(rc(w,&x,&y));
 n=AN(w); wr=MAX(2,AR(w));
 xn=AN(x)-1; xv=AV(x); ht=1; DO(xn, ht+=xv[i]; ASSERT(0<ht,EVLIMIT););
 yn=AN(y)-1; yv=AV(y); wd=1; DO(yn, wd+=yv[i]; ASSERT(0<wd,EVLIMIT););
 RE(p=mult(ht,wd)); q=MAX(1,xn*yn); m=n/q; RE(zn=mult(m,p));
 GA(z,LIT,zn,wr,AS(w)); *(AS(z)+wr-2)=ht; *(AS(z)+wr-1)=wd; 
 if(!n)R z;
 zv=CAV(z);
 fminit(m,ht,wd,x,y,zv);
 fmfill(p,q,wd,w,x,y,zv);
 R z;
}

F1(jtmat){A z;B b=0;C*v,*x;I c,k,m=1,p,q,qc,r,*s,zn;
 RZ(w);
 r=AR(w); s=AS(w); v=CAV(w);
 q=1<r?s[r-2]:1; c=r?s[r-1]:1;
 DO(r-2, if(!s[i]){b=1; break;});
 if(b)k=m=0; else{k=2<r?2-r:0; DO(r-2, p=m; m*=s[i]; ASSERT(m>=p,EVLIMIT); k+=m;);}
 RE(p=mult(m,q)+k*!!q); ASSERT(0<=p,EVLIMIT); RE(zn=mult(p,c));
 GA(z,LIT,zn,2,0); *AS(z)=p; *(1+AS(z))=c; x=CAV(z);
 if(2<r)fillv(LIT,zn,x);
 if(zn){RE(qc=mult(q,c)); DO(m, ENGAP(i*q,r,s,x+=c*b); MC(x,v,qc); x+=qc; v+=qc;);}
 R z;
}

static F1(jtmatth1){R mat(thorn1(w));}

static F1(jtthbox){A z;UC*s;static C ctrl[]=" \001\002\003\004\005\006\007   \013\014 ";
 RZ(z=enframe(every(w,0L,jtmatth1)));
 s=UAV(z); 
 DO(AN(z), if(14>s[i])s[i]=ctrl[s[i]];);
 R z;
}

static F1(jtths){A e,i,x,z;C c,*u,*v;I d,m,n,*s;P*p;
 RZ(scheck(w));
 p=PAV(w); e=SPA(p,e); i=SPA(p,i); x=SPA(p,x); 
 RZ(i=thorn1(i)); s=AS(i); m=s[0]; n=s[1];
 RZ(x=thorn1(1<AR(x)?x:table(x))); 
 RZ(e=shape(x)); s=AV(e)+AN(e)-1; *s=-(*s+3+n);
 RZ(z=take(e,x)); 
 u=CAV(i)-n;        
 d=aii(z); v=CAV(z)-d; DO(m, MC(v+=d,u+=n,n););
 if(2<AR(z))RZ(z=matth1(z));
 s=AS(z); d=*(1+s); v=1+CAV(z); c=jt->bx[9]; DO(*s, *(v+n)=c; v+=d;);
 R z;
}

F1(jtthorn1){PROLOG;A z;
 RZ(w);
 if(!AN(w))GA(z,LIT,0,AR(w),AS(w))
 else switch(CTTZ(AT(w))){
#ifdef UNDER_CE
  default:   if(AT(w)&XD+XZ)z=thxqe(w); else R 0; break;
  case XNUMX: case RATX:
             z=thxqe(w);                  break;
#else
  default:   R 0;
  case XNUMX: case RATX: case XDX: case XZX:
             z=thxqe(w);                  break;
#endif
  case BITX:  z=thbit(w);                  break;
  case B01X:  z=thb(w);                    break;
  case LITX:  z=ca(w);                     break;
  case C2TX:  z=rank1ex(w,0L,1L,jttoutf8); break;
  case BOXX:  z=thbox(w);                  break;
  case SBTX:  z=thsb(w);                   break;
  case NAMEX: z=sfn(0,w);                  break;
  case ASGNX: z=spellout(*CAV(w));         break;
  case INTX:  case FLX: case CMPXX:
             z=thn(w);                    break;
  case SB01X: case SINTX: case SFLX: case SCMPXX: case SLITX: case SBOXX:
             z=ths(w);                    break;
  case VERBX: case ADVX:  case CONJX:
   switch((jt->disp)[1]){
    case 1: z=thorn1(arep(w)); break;
    case 2: z=thorn1(drep(w)); break;
    case 4: z=thorn1(trep(w)); break;
    case 5: z=thorn1(lrep(w)); break;
    case 6: z=thorn1(prep(w)); break;
 }}
 EPILOG(z);
}

#define DDD(v)   {*v++='.'; *v++='.'; *v++='.';}
#define EOL(zv)  {zv[0]=eov[0]; zv[1]=eov[1]; zv+=m; ++lc;}
#define BDC(x)   if(16<=x&&x<=26){*(zv-1)='\342'; *zv++='\224'; *zv++=bdc[x];}

static I scanbdc(C*v,I h,I nq,I c,I lb,I la){C*u,x;I m;
 u=v; x=0; m=0;
 if(h>=nq)
  DO(c*nq, x=*u++; if(16<=x&&x<=26)m+=3;)
 else{
  DO(c*lb, x=*u++; if(16<=x&&x<=26)m+=3;);
  u=v+c*(nq-la);
  DO(c*la, x=*u++; if(16<=x&&x<=26)m+=3;);
 }
 R m;
}    /* scan for box drawing chars requiring additional space */

static I scaneol(C*v,I h,I nq,I c,I lb,I la){C e,*u,x;I m;
 u=v; x=0; m=0;
 if(h>=nq)
  DO(c*nq, e=x; x=*u++; if(x==CCR)++m; else if(x==CLF)e==CCR?--m:++m;)
 else{
  DO(c*lb, e=x; x=*u++; if(x==CCR)++m; else if(x==CLF)e==CCR?--m:++m;);
  u=v+c*(nq-la);
  DO(c*la, e=x; x=*u++; if(x==CCR)++m; else if(x==CLF)e==CCR?--m:++m;);
 }
 R m;
}    /* scan for EOL requiring additional space */

/* zn:  max length of zu,zv            */
/* zu:  points to start   of data area */
/* zv:  points to 1 + end of data area */
/* lb:  # lines before ...             */
/* la:  # lines after  ...             */
/* m:   # chars in end-of-line         */
/* eo:  eol if 1 char                  */
/* eol: eol if 2 chars                 */

static C*dropl(I zn,C*zu,C*zv,I lb,I la,I m,C eo,C*eov){C*u,*v;I lc=0,n,p,q;
 p=q=0; u=zu; v=zv;
 if(1==m){
  DO(zn, if(p>=lb)break; if(eo      ==*u++      )p++;);
  DO(zn, if(q> la)break; if(eo      ==*--v      )q++;);
 }else{
  DO(zn, if(p>=lb)break; if(*(S*)eov==*(S*)(u++))p++;);
  DO(zn, if(q> la)break; if(*(S*)eov==*(S*)(--v))q++;);
 }
 DDD(u); EOL(u); n=zv-(m+v); memmove(u,m+v,n);
 R u+n;
}    /* drop excessive lines */

static A jtjprx(J jt,I ieol,I maxlen,I lb,I la,A w){A y,z;B ch;C e,eo,*eov,*v,x,*zu,*zv;D lba;
     I c,c1,h,i,j,k,lc,m,n,nbx,nq,p,q,r,*s,zn;S eol;
     static C bdc[]="123456789_123456\214\254\220\234\274\244\224\264\230\202\200";
 RZ(y=thorn1(w));
 ch=1&&AT(w)&LIT+C2T+SBT;
 r=AR(y); s=AS(y); v=CAV(y); eov=(C*)&eol;
 q=1<r?s[r-2]:1; c=r?s[r-1]:1; RE(n=prod(r-2,s)); RE(nq=mult(n,q));
 if(ieol){m=2; *eov=CCR; *(1+eov)=CLF;}else{m=1; *eov=eo=CLF;}
 c1=MIN(c,maxlen); lba=(D)lb+la;
 p=2<r?2-r:0; h=1; DO(r-2, if(s[i]){h*=s[i]; p+=h;}else{p=0; break;});
 h=lba<IMAX?lb+la:IMAX; h=MIN(nq,h); 
 RE(zn=(3+m)+(q?p*m:0)+mult(h,ch?c+m+(3+m)*(1+c/maxlen):c1+m+3*(c1<c)));
 if(ch&&1<m             )zn+=    scaneol(v,h,nq,c,lb,la);
 if(ch||AT(w)&BOX+SPARSE)zn+=nbx=scanbdc(v,h,nq,c,lb,la);
 GA(z,LIT,zn,1,0); zu=zv=CAV(z);
 h=lba<nq+(q?p:0)?lb:IMAX;
 for(i=lc=0;i<nq;++i){
  if(0==i%q)ENGAP(i,r,s,if(b)EOL(zv));
  if(h<=lc&&nq>la){h=IMAX; p=nq-la; v+=c*(p-i); i=p-1; DDD(zv);}
  else if(ch)for(j=k=x=0;j<c;++j){
   e=x; x=*v++;
   if     (x==CCR){          EOL(zv); k=0;}
   else if(x==CLF){if(e!=CCR)EOL(zv); k=0;}
   else if(x)     {if(k<c1){*zv++=x; BDC(x);} else if(k==c1)DDD(zv); ++k;}
  }else if(nbx){DO(c1, *zv++=x=*v++; BDC(x);); if(c1<c){v+=c-c1; DDD(zv);}}
  else         {MC(zv,v,c1); zv+=c1; v+=c1;    if(c1<c){v+=c-c1; DDD(zv);}}
  EOL(zv);
 }
 if(lc>1+lba)zv=dropl(zn,zu,zv,lb,la,m,eo,eov);
 p=zv-zu; 
 ASSERTSYS(p<=zn,"jprx zn");
 *zv=0; z->n=*(z->s)=p;
 R z;
}    /* output string from array w */

F2(jtoutstr){I*v;
 RZ(a&&w);
 RZ(a=vib(a));
 ASSERT(1==AR(a), EVRANK);
 ASSERT(4==AN(a), EVLENGTH);
 ASSERT(INT&AT(a),EVDOMAIN);
 v=AV(a);
 ASSERT(0<=v[0]&&v[0]<=2,EVINDEX);
 ASSERT(0<=v[1],EVDOMAIN);
 ASSERT(0<=v[2],EVDOMAIN);
 ASSERT(0<=v[3],EVDOMAIN);
 R jprx(v[0],v[1],v[2],v[3],w);
}

static F1(jtjpr1){PROLOG;A z;
 RZ(z=jprx(jt->outeol,jt->outmaxlen,jt->outmaxbefore,jt->outmaxafter,w));
 if(AN(z))jsto(jt,jt->mtyo==0?MTYOFM:jt->mtyo,CAV(z));
 EPILOG(mtm);
}

F1(jtjpr){A y;I i,n,t,*v;
 RZ(w);
 t=AT(w);
 if(t&NOUN&&jt->tostdout)RZ(jpr1(w))
 else if(t&VERB+ADV+CONJ){
  RZ(y=evoke(w)?symbrdlock(VAV(w)->f):w);
  if(jt->tostdout){
   n=*jt->disp; v=1+jt->disp;
   for(i=0;i<n;++i)switch(*v++){
    case 1: RZ(jpr1(arep(y))); break;
    case 2: RZ(jpr1(drep(y))); break;
    case 4: RZ(jpr1(trep(y))); break;
    case 5: RZ(jpr1(lrep(y))); break;
    case 6: RZ(jpr1(prep(y))); break;
 }}}
 R mtm;
}
