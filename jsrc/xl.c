/* Copyright 1990-2005, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: File Lock/Unlock                                                 */

#include "j.h"
#ifdef link
#undef link           /* name conflict ? */
#endif
#include "x.h"

#if SY_WINCE || !(SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#define LOCK 1
static B jtdolock(J jt,B lk,F f,I i,I n){ASSERT(0,EVNONCE);}
#endif

#if (SYS & SYS_UNIX) || (SY_WIN32 && SYS&SYS_DOS && !SY_WINCE)
typedef long long INT64;
#include <stdint.h>
#define LOCK 1
#if (SYS & SYS_UNIX)
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#else
#include <sys/locking.h>
#include <io.h>
#endif

// extern int _locking(int,int,long);

static B jtdolock(J jt,B lk,F f,I i,I n){I e;long c;fpos_t v; fpos_t q;
 c=fgetpos(f,(fpos_t*)&q);
 if(0!=c)R (B)(intptr_t)jerrno();
 {INT64 vv; vv=i; v=*(fpos_t*)&vv;}
 c=fsetpos(f,(fpos_t*)&v);
 if(0!=c)R (B)(intptr_t)jerrno();
#if SY_WIN32
 e=_locking(_fileno(f),lk?_LK_NBLCK:_LK_UNLCK,(long)n);
#else
#ifdef ANDROID
 e=flock(fileno(f),lk?LOCK_EX+LOCK_NB:LOCK_UN);
#else
 e=lockf(fileno(f),lk?F_TLOCK:F_ULOCK,(I)n);
#endif
#endif
 fsetpos(f,(fpos_t*)&q);
#ifdef __MINGW32__
 R !e?1:(errno==EEXIST||errno==ENOENT||errno==EACCES)?0:(B)(intptr_t)jerrno();
#else
 R !e?1:errno==EACCES?0:(B)(intptr_t)jerrno();
#endif
}
#endif

#ifndef LOCK
static B jtdolock(J jt,B lk,F f,I i,I n){I e;
 e=lk?lock(fileno(f),i,n):unlock(fileno(f),i,n);
 R !e?1:errno==EACCES?0:(intptr_t)jerrno();
}
#endif

#define LKC  3      /* number of columns in jt->flkd table       */

B jtxlinit(J jt){A x;I*s;
 GAT0(x,INT,20*LKC,2); ras(x); s=AS(x); s[0]=20; s[1]=LKC;
 jt->flkd=x;
 R 1;
}

F1(jtjlocks){A y; ASSERTMTV(w); y=take(sc(jt->flkn),jt->flkd); R grade2(y,y);}
     /* return the locks, a 3-column table of (number,index,length) */

F1(jtjlock){B b;I*v;
 F1RANK(1,jtjlock,0);
 RZ(w=vi(w)); 
 ASSERT(LKC==AN(w),EVLENGTH);
 v=AV(w); RE(vfn((F)*v)); ASSERT(0<=v[1]&&0<=v[2],EVDOMAIN); 
 if(jt->flkn==*AS(jt->flkd))RZ(jt->flkd=ext(1,jt->flkd));
 RE(b=dolock(1,(F)v[0],v[1],v[2]));
 if(!b)R num[0];
 ICPY(AV(jt->flkd)+LKC*jt->flkn,v,LKC); ++jt->flkn;
 R num[1];
}    /* w is (number,index,length); lock the specified region */

static A jtunlj(J jt,I j){B b;I*u,*v;
 RE(j);
 ASSERT(0<=j&&j<jt->flkn,EVINDEX);
 u=AV(jt->flkd); v=u+j*LKC;
 RE(b=dolock(0,(F)v[0],v[1],v[2]));
 if(!b)R num[0];
 --jt->flkn; 
 if(j<jt->flkn)ICPY(v,u+jt->flkn*LKC,LKC); else *v=0; 
 R num[1];
}    /* unlock the j-th entry in jt->flkd */

B jtunlk(J jt,I x){I j=0,*v=AV(jt->flkd); 
 while(j<jt->flkn){while(x==*v)RZ(unlj(j)); ++j; v+=LKC;} 
 R 1;
}    /* unlock all existing locks for file# x */

F1(jtjunlock){
 F1RANK(1,jtjunlock,0); 
 ASSERT(INT&AT(w),EVDOMAIN); 
 R unlj(i0(indexof(jt->flkd,w))); 
}    /* w is (number,index,length); unlock the specified region */
