/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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

#define LKC  3      /* number of columns in JT(jt,flkd) table       */

B jtxlinit(JS jjt,I nthreads){A x;I*s;JJ jt=MTHREAD(jjt);
 GAT0(x,INT,20*LKC,2); ACINITZAP(x); s=AS(x); s[0]=20; s[1]=LKC;  // called at init; shape is 20 3
 INITJT(jjt,flkd)=x; AM(INITJT(jjt,flkd))=0;  // AM holds the # valid entries
 R 1;
}

// 1!:30
// Our preference is to perform no system calls under readlock/writelock.  Here we violate that rule by executing J verbs
// under the lock.  It's just too easy that way, and these functions are little-used and have low overhead.  Since the returns are
// small - virtual blocks or scalars - we could allocate/free a block or two to make sure they are available, but we don't bother
F1(jtjlocks){A y; ASSERTMTV(w); READLOCK(JT(jt,flock)) y=take(sc(AM(JT(jt,flkd))),JT(jt,flkd)); READUNLOCK(JT(jt,flock)) R grade2(y,y);}
     /* return the locks, a 3-column table of (number,index,length) */

// 1!:31
F1(jtjlock){B b;I*v;
 F1RANK(1,jtjlock,DUMMYSELF);
 RZ(w=vi(w)); 
 ASSERT(LKC==AN(w),EVLENGTH);
 v=AV(w); ASSERT(0<=v[1]&&0<=v[2],EVDOMAIN); RE(vfn((F)*v));
 RE(b=(B)(I)jtunvfn(jt,(F)v[0],(A)(I)dolock(1,(F)v[0],v[1],v[2])));
 if(!b)R num(0);
 WRITELOCK(JT(jt,flock))
 while(AM(JT(jt,flkd))==AS(JT(jt,flkd))[0])RZ(jtextendunderlock(jt,&JT(jt,flkd),&JT(jt,flock),0))
 ICPY(AV(JT(jt,flkd))+LKC*AM(JT(jt,flkd)),v,LKC); ++AM(JT(jt,flkd));
 WRITEUNLOCK(JT(jt,flock))
 R num(1);
}    /* w is (number,index,length); lock the specified region */

// remove entry j from the lock table and unlock the file it points to
// We enter holding a write lock on flock.  We relinquish this lock before returning
static A jtunlj(J jt,I j){B b;I*u,*v;
 if(unlikely(jt->jerr!=0)){WRITEUNLOCK(JT(jt,flock)); R0}
 if(unlikely(!BETWEENO(j,0,AM(JT(jt,flkd))))){WRITEUNLOCK(JT(jt,flock)); ASSERT(0,EVINDEX);}
 u=AV(JT(jt,flkd)); v=u+j*LKC;
 I v0=v[0], v1=v[1], v2=v[2];  // save the values from the lock-table entry before we destroy it
 --AM(JT(jt,flkd)); 
 if(j<AM(JT(jt,flkd)))ICPY(v,u+AM(JT(jt,flkd))*LKC,LKC);  // fill vacated hole if not at end
 WRITEUNLOCK(JT(jt,flock))  // we are committed to the unlock, so it is OK to show the file as not in the lock table
 RE(b=dolock(0,(F)v0,v1,v2));
 R num(!!b);
}    /* unlock the j-th entry in JT(jt,flkd) */

B jtunlk(J jt,I x){
 // Since each unlock releases the write lock here, the table may get scrambled between unlocks.
 // Therefore, we rescan from the beginning for every delete.  That would suck if there were numerous locks extant.
 while(1){
  WRITELOCK(JT(jt,flock))
  I *v=AV(JT(jt,flkd)); 
  DO(AM(JT(jt,flkd)), if(x==IAV2(JT(jt,flkd))[3*i]){RZ(unlj(i)); goto found;})
  WRITEUNLOCK(JT(jt,flock)) break;  // after a pass with no file matches, we are done.  relinquish the lock and exit loop
found: ;   // here when a file was unlocked.  We know we have given up the lock on flkd
 }
 R 1;
}    /* unlock all existing locks for file# x */

// 1!:32
F1(jtjunlock){
 F1RANK(1,jtjunlock,DUMMYSELF); 
 ASSERT(INT&AT(w),EVDOMAIN); 
 WRITELOCK(JT(jt,flock))
 R unlj(i0(indexof(JT(jt,flkd),w))); 
}    /* w is (number,index,length); unlock the specified region */
