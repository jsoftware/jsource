/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: time and space                                                   */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include "j.h"

#if !SY_WINCE && (SY_WIN32 || (SYS & SYS_LINUX))
#include <time.h>
#else
#if SYS & SYS_UNIX
#include <sys/time.h>
#endif
#endif

#if !SY_WIN32 && (SYS & SYS_DOS)
#include <dos.h>
#endif

#ifndef CLOCKS_PER_SEC
#if (SYS & SYS_UNIX)
#define CLOCKS_PER_SEC  1000000
#endif
#ifdef  CLK_TCK
#define CLOCKS_PER_SEC  CLK_TCK
#endif
#endif


F1(jtsp){ASSERTMTV(w); R sc(jt->bytes);}

F1(jtspit){A z;I k; 
 F1RANK(1,jtspit,0); 
 jt->bytesmax=k=jt->bytes;
 FDEPINC(1); z=exec1(w); FDEPDEC(1);
 RZ(z);
 R sc(jt->bytesmax-k);
}

F1(jtparsercalls){ASSERTMTV(w); R sc(jt->parsercalls);}

// 6!:5, window into the running J code
F1(jtpeekdata){ ASSERTMTV(w); R sc(jt->peekdata); }

// 6!:6, used to run tests that need to run in a sentence
F1 (jttestcode) {
#if defined(OBSOLETE)
// jtmult tests
 I multops[]  = {
 0,1,-1,20,-20,300,-300,65535,-65535,65536,-65536,65537,-65537,
 0x7ffffff0,-0x7ffffff0,0x7ffffffe,-0x7ffffffe,0x7fffffff,-0x7fffffff,
 0x80000000,-0x80000000,0x80000001,-0x80000001,0x80000002,-0x80000002,
 0xc0000000,-0xc0000000,0xfffffff0,-0xfffffff0,0xfffffffe,-0xfffffffe,
 0xffffffff,-0xffffffff,0x100000000,-0x100000000,0x100000001,-0x100000001,
 0x100000002,-0x100000002,0x200000567,-0x200000567,0x412345678,-0x412345678,
 0x4ffffdcba,-0x4ffffdcba,
 0x7ffffffe00000000,-0x7ffffffe00000000,
 0x7ffffffe00000001,-0x7ffffffe00000001,0x7ffffffe7ffffffe,-0x7ffffffe7ffffffe,
 0x7ffffffe7fffffff,-0x7ffffffe7fffffff,0x7ffffffe80000000,-0x7ffffffe80000000,
 0x7ffffffe80000001,-0x7ffffffe80000001,0x7ffffffe80000002,-0x7ffffffe80000002,
 0x7ffffffefffffffe,-0x7ffffffefffffffe,0x7ffffffeffffffff,-0x7ffffffeffffffff,
 0x7fffffff00000000,-0x7fffffff00000000,
 0x7fffffff00000001,-0x7fffffff00000001,0x7fffffff7ffffffe,-0x7fffffff7ffffffe,
 0x7fffffff7fffffff,-0x7fffffff7fffffff,0x7fffffff80000000,-0x7fffffff80000000,
 0x7fffffff80000001,-0x7fffffff80000001,0x7fffffff80000002,-0x7fffffff80000002,
 0x7ffffffffffffffe,-0x7ffffffffffffffe,0x7fffffffffffffff,-0x7fffffffffffffff,
 0x8000000000000000,-0x8000000000000000,
 0x8000000000000001,-0x8000000000000001,0x800000007ffffffe,-0x800000007ffffffe,
 0x800000007fffffff,-0x800000007fffffff,0x8000000080000000,-0x8000000080000000,
 0x8000000080000001,-0x8000000080000001,0x8000000080000002,-0x8000000080000002,
 0x80000000fffffffe,-0x80000000fffffffe,0x80000000ffffffff,-0x80000000ffffffff,
 0x8000000100000000,-0x8000000100000000,
 0x8000000100000001,-0x8000000100000001,0x800000017ffffffe,-0x800000017ffffffe,
 0x800000017fffffff,-0x800000017fffffff,0x8000000180000000,-0x8000000180000000,
 0x8000000180000001,-0x8000000180000001,0x8000000180000002,-0x8000000180000002,
 0x80000001fffffffe,-0x80000001fffffffe,0x80000001ffffffff,-0x80000001ffffffff,
 0x8000000200000000,-0x8000000200000000,
 0x8000000200000001,-0x8000000200000001,0x800000027ffffffe,-0x800000027ffffffe,
 0x800000027fffffff,-0x800000027fffffff,0x8000000280000000,-0x8000000280000000,
 0x8000000280000001,-0x8000000280000001,0x8000000280000002,-0x8000000280000002,
 0x80000002fffffffe,-0x80000002fffffffe,0x80000002ffffffff,-0x80000002ffffffff,
 0xfffffffe00000000,-0xfffffffe00000000,
 0xfffffffe00000001,-0xfffffffe00000001,0xfffffffe7ffffffe,-0xfffffffe7ffffffe,
 0xfffffffe7fffffff,-0xfffffffe7fffffff,0xfffffffe80000000,-0xfffffffe80000000,
 0xfffffffe80000001,-0xfffffffe80000001,0xfffffffe80000002,-0xfffffffe80000002,
 0xfffffffefffffffe,-0xfffffffefffffffe,0xfffffffeffffffff,-0xfffffffeffffffff,
 0xffffffff00000000,-0xffffffff00000000,
 0xffffffff00000001,-0xffffffff00000001,0xffffffff7ffffffe,-0xffffffff7ffffffe,
 0xffffffff7fffffff,-0xffffffff7fffffff,0xffffffff80000000,-0xffffffff80000000,
 0xffffffff80000001,-0xffffffff80000001,0xffffffff80000002,-0xffffffff80000002,
 0xfffffffffffffffe,-0xfffffffffffffffe,0xffffffffffffffff,-0xffffffffffffffff
};
 I i, j;
 mult(0x8000000000000000LL,-1LL);
 for(i = 0;i < sizeof(multops)/sizeof(multops[0]);++i){
  for(j = 0;j < sizeof(multops)/sizeof(multops[0]);++j){
   I res = mult(multops[i],multops[j]);
   I chkval = multops[i]*multops[j];
   I limitj;
   if(multops[i]==0x8000000000000000LL && multops[j]==-1LL)chkval = 0;
   if(multops[j] == 0x8000000000000000LL && multops[i] == -1LL)chkval = 0;
   if ((multops[i] != 0) && (multops[j] != 0) && (multops[i] != 1) && (multops[j] != 1) && (multops[i] != -1) && (multops[j] != -1)){
    if((multops[i]>0)==(multops[j]>0)) {
     limitj = (0x7fffffffffffffffLL / multops[i]);
     if(multops[j]<0 && multops[j] < limitj)chkval = 0;
     if(multops[j]>0 && multops[j] > limitj)chkval = 0;
    } else {
     limitj = (0x8000000000000000LL / multops[i]);
     if (multops[j]<0 && multops[j] < limitj)chkval = 0;
     if (multops[j]>0 && multops[j] > limitj)chkval = 0;
    }
   }
   if(res != chkval)*(I *)0 = i;
  }
 }
#endif
R sc(0);
}

#if SY_WIN32
 /* defined in jdll.c */
#else
F1(jtts){A z;D*x;struct tm*t;struct timeval tv;
 ASSERTMTV(w);
 gettimeofday(&tv,NULL); t=localtime((time_t*)&tv.tv_sec);
 GA(z,FL,6,1,0); x=DAV(z);
 x[0]=t->tm_year+1900;
 x[1]=t->tm_mon+1;
 x[2]=t->tm_mday;
 x[3]=t->tm_hour;
 x[4]=t->tm_min;
 x[5]=t->tm_sec+(D)tv.tv_usec/1e6;
 R z;
}
#endif

F1(jtts0){A x,z;C s[9],*u,*v,*zv;D*xv;I n,q;
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 RZ(x=ts(mtv));
 n=AN(w); xv=DAV(x);
 if(!n)R x;
 if(!(AT(w)&LIT))RZ(w=cvt(LIT,w));
 GA(z,LIT,n,AR(w),AS(w)); zv=CAV(z); memcpy(zv,CAV(w),n);
 q=0; v=zv; DO(n, if('Y'==*v++)++q;); u=2==q?s+2:s;
 sprintf(s,FMTI04,(I)xv[0]);             v=zv; DO(n, if(*v=='Y'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[1]);        u=s; v=zv; DO(n, if(*v=='M'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[2]);        u=s; v=zv; DO(n, if(*v=='D'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[3]);        u=s; v=zv; DO(n, if(*v=='h'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[4]);        u=s; v=zv; DO(n, if(*v=='m'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI05,(I)(1000*xv[5])); u=s; v=zv; DO(n, if(*v=='s'){*v=*u++; if(!*u)break;} ++v;);
 R z;
}


#if SY_GETTOD
D tod(void){struct timeval t; gettimeofday(&t,NULL); R t.tv_sec+(D)t.tv_usec/1e6;}
#else
#if SY_WINCE
D tod(void){SYSTEMTIME t; GetLocalTime(&t); R t.wSecond+(D)t.wMilliseconds/1e3;}
#else
D tod(void){R(D)clock()/CLOCKS_PER_SEC;}
#endif
#endif


#if SY_WIN32

typedef LARGE_INTEGER LI;

static D qpm=4294967296.0;  /* 2^32 */

D qpf(void){LI n; QueryPerformanceFrequency(&n); R n.LowPart+qpm*n.HighPart;}

static D qpc(void){LI n; QueryPerformanceCounter(&n); R n.LowPart+qpm*n.HighPart;}

#else

D qpf(void){R (D)CLOCKS_PER_SEC;}

static D qpc(void){R tod()*CLOCKS_PER_SEC;}

#endif

/* 
// by Mark VanTassel from The Code Project

__int64 GetMachineCycleCount()
{      
   __int64 cycles;
   _asm rdtsc; // won't work on 486 or below - only pentium or above
   _asm lea ebx,cycles;
   _asm mov [ebx],eax;
   _asm mov [ebx+4],edx;
   return cycles;
}
*/


F1(jttss){ASSERTMTV(w); R scf(tod()-jt->tssbase);}

F2(jttsit2){A z;D t;I n,old;
 F2RANK(0,1,jttsit2,0);
 RE(n=i0(a));
 FDEPINC(1);  // No ASSERTs/returns till the DEPDEC below
 t=qpc(); 
 old=jt->tbase+jt->ttop; DO(n, z=exec1(w); if(!z)break; tpop(old);); 
 t=qpc()-t;
 FDEPDEC(1);  // Assert OK now
 RZ(z);
 R scf(n?t/(n*pf):0);
}

F1(jttsit1){R tsit2(one,w);}

#ifdef _WIN32
#define sleepms(i) Sleep(i)
#else
#define sleepms(i) sleep((i+500)/1000)
#endif

F1(jtdl){D m,n,*v;UINT ms,s;
 RZ(w=cvt(FL,w));
 n=0; v=DAV(w); DO(AN(w), m=*v++; ASSERT(0<=m,EVDOMAIN); n+=m;);
 s=(UINT)jfloor(n); ms=(UINT)jfloor(0.5+1000*(n-s));
#if SYS & SYS_MACINTOSH
 {I t=TickCount()+(I)(60*n); while(t>TickCount())JBREAK0;}
#else
 DO(s, sleepms(1000); JBREAK0;);
 sleepms(ms);
#endif
 R w;
}


F1(jtqpfreq){ASSERTMTV(w); R scf(pf);}

F1(jtqpctr ){ASSERTMTV(w); R scf(qpc());}

F1(jtpmctr){D x;I q;
 RE(q=i0(w));
 ASSERT(jt->pma,EVDOMAIN);
 x=q+(D)jt->pmctr;
 ASSERT(IMIN<=x&&x<=IMAX,EVDOMAIN);
 jt->pmctr=q=(I)x; 
 R sc(q);
}    /* add w to pmctr */

static F1(jtpmfree){A x,y;C*c;I m;PM*v;PM0*u;
 if(w){
  c=CAV(w); u=(PM0*)c; v=(PM*)(c+sizeof(PM0)); 
  m=u->wrapped?u->n:u->i; 
  DO(m, x=v->name; if(x&&NAME==AT(x)&&AN(x)==*AS(x))fa(x); 
        y=v->loc;  if(y&&NAME==AT(y)&&AN(y)==*AS(y))fa(y); ++v;);
  fa(w);
 }
 R one;
}    /* free old data area */

F1(jtpmarea1){R pmarea2(vec(B01,2L,&zeroZ),w);}

F2(jtpmarea2){A x;B a0,a1,*av;C*v;I an,n=0,s=sizeof(PM),s0=sizeof(PM0),wn;PM0*u;
 RZ(a&&w);
 ASSERT(prokey, EVDOMAIN);
 RZ(a=cvt(B01,a)); 
 an=AN(a);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(2>=an,EVLENGTH);
 av=BAV(a); 
 a0=0<an?av[0]:0;
 a1=1<an?av[1]:0;
 RZ(w=vs(w));
 wn=AN(w);
 ASSERT(!wn||wn>=s+s0,EVLENGTH);
 x=jt->pma;
 jt->pmctr=0;
 jt->pma=wn?ra(w):0;
 RZ(pmfree(x));
 if(wn){
  v=CAV(w);
  jt->pmu=u=(PM0*)v; 
  jt->pmv=(PM*)(s0+v); 
  jt->pmrec=u->rec=a0;
  u->n=n=(wn-s0)/s; 
  u->i=0;
  u->s=jt->bytesmax=jt->bytes;
  u->trunc=a1; 
  u->wrapped=0;
 }
 R sc(n);
}

void jtpmrecord(J jt,A name,A loc,I lc,int val){A x,y;B b;PM*v;PM0*u;
 u=jt->pmu;
 v=jt->pmv+u->i;
 if(b=u->wrapped){x=v->name; y=v->loc;}
 ++u->i;
 if(u->i>u->n){u->wrapped=1; if(u->trunc){u->i=u->n; R;}else u->i=0;}
 v->name=name; if(name)++AC(name);
 v->loc =loc;  if(loc )++AC(loc ); if(b){fa(x); fa(y);}
 v->val =val;
 v->lc  =lc;
 v->s=jt->bytesmax-u->s;
 u->s=jt->bytesmax=jt->bytes;
#if SY_WIN32
 QueryPerformanceCounter((LI*)v->t);
#else
 {D d=tod(); MC(v->t,&d,sizeof(D));}
#endif
}

F1(jtpmunpack){A*au,*av,c,t,x,z,*zv;B*b;D*dv;I*iv,k,m,n,p,q,wn,*wv;PM*v,*v0,*vq;PM0*u;
 RZ(w);
 ASSERT(jt->pma,EVDOMAIN);
 if(!(INT&AT(w)))RZ(w=cvt(INT,w));
 wn=AN(w); wv=AV(w);
 u=(PM0*)AV(jt->pma); p=u->wrapped?u->n-u->i:0; q=u->i; n=p+q;
 GA(x,B01,n,1,0); b=BAV(x); memset(b,wn?C0:C1,n);
 if(wn){
  DO(wn, k=wv[i]; if(0>k)k+=n; ASSERT(0<=k&&k<n,EVINDEX); b[k]=1;);
  m=0; 
  DO(n, if(b[i])++m;);
 }else m=n;
 v0=jt->pmv; vq=q+v0;
 GA(z,BOX,1+PMCOL,1,0); zv=AAV(z);
 GA(t,BOX,2*m,1,0); av=AAV(t); au=m+av;
 v=vq; DO(p, if(b[  i]){RZ(*av++=v->name?sfn(0,v->name):mtv); RZ(*au++=v->loc?sfn(0,v->loc):mtv);} ++v;); 
 v=v0; DO(q, if(b[p+i]){RZ(*av++=v->name?sfn(0,v->name):mtv); RZ(*au++=v->loc?sfn(0,v->loc):mtv);} ++v;); 
 RZ(x=indexof(t,t));
 RZ(c=eq(x,IX(IC(x))));
 RZ(zv[6]=repeat(c,t));
 RZ(x=indexof(repeat(c,x),x)); iv=AV(x);
 RZ(zv[0]=vec(INT,m,  iv));
 RZ(zv[1]=vec(INT,m,m+iv));
 GA(t,INT,m,1,0); zv[2]=t; iv=AV(t); v=vq; DO(p, if(b[i])*iv++=(I)v->val;  ++v;); v=v0; DO(q, if(b[p+i])*iv++=(I)v->val; ++v;);
 GA(t,INT,m,1,0); zv[3]=t; iv=AV(t); v=vq; DO(p, if(b[i])*iv++=v->lc; ++v;); v=v0; DO(q, if(b[p+i])*iv++=v->lc; ++v;);
 GA(t,INT,m,1,0); zv[4]=t; iv=AV(t); v=vq; DO(p, if(b[i])*iv++=v->s;  ++v;); v=v0; DO(q, if(b[p+i])*iv++=v->s;  ++v;); 
 GA(t,FL, m,1,0); zv[5]=t; dv=DAV(t);
#if SY_WIN32
 v=vq; DO(p, if(b[i]  )*dv++=(((LI*)v->t)->LowPart+qpm*((LI*)v->t)->HighPart)/pf; ++v;);
 v=v0; DO(q, if(b[p+i])*dv++=(((LI*)v->t)->LowPart+qpm*((LI*)v->t)->HighPart)/pf; ++v;);
#else
 v=vq; DO(p, if(b[i]  ){MC(dv,v->t,sizeof(D)); ++dv;} ++v;); 
 v=v0; DO(q, if(b[p+i]){MC(dv,v->t,sizeof(D)); ++dv;} ++v;); 
#endif
 R z;
}

F1(jtpmstats){A x,z;I*zv;PM0*u;
 ASSERTMTV(w);
 GA(z,INT,6,1,0); zv=AV(z);
 if(x=jt->pma){
  u=(PM0*)AV(x);
  zv[0]=u->rec;
  zv[1]=u->trunc;
  zv[2]=u->n;
  zv[3]=u->wrapped?u->n:u->i;
  zv[4]=u->wrapped;
  zv[5]=jt->pmctr;
 }else zv[0]=zv[1]=zv[2]=zv[3]=zv[4]=zv[5]=0;
 R z;
}


F1(jttlimq){ASSERTMTV(w); R scf(0.001*jt->timelimit);}

F1(jttlims){D d;
 RZ(w);
 ASSERT(!AR(w),EVRANK);
 if(!(FL&AT(w)))RZ(w=cvt(FL,w));
 d=*DAV(w);
 ASSERT(0<=d,EVDOMAIN);
 ASSERT(IMAX>1000*d,EVLIMIT);
 jt->timelimit=(UI)(1000*d);
 R mtm;
}
