/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: time and space                                                   */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#ifndef _WIN64
#ifndef PSAPI_VERSION
#define PSAPI_VERSION 1
#endif
#include <psapi.h>
#endif
#ifndef __MINGW32__
#include <time.h>
#else
#include <sys/time.h>
#endif
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "j.h"
#include "w.h"
#include "cpuinfo.h"

#if !SY_WINCE && (SY_WIN32 || (SYS & SYS_UNIX))
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

#if defined(_WIN32)
#include <psapi.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/message.h>  // for mach_msg_type_number_t
#include <mach/kern_return.h>  // for kern_return_t
#include <mach/task_info.h>
#else
#include <sys/resource.h>
#endif

#if defined(__OpenBSD__)
#include <kvm.h>
#include <fcntl.h>
#include <sys/sysctl.h>
#endif

F1(jtsp){F12IP;ASSERTMTV(w); R sc(spbytesinuse());}  //  7!:0

// 7!:1
// Return (current allo),(max since reset)
// If arg is an atom, reset hwmk to it
F1(jtsphwmk){F12IP;
  I curr = jt->malloctotal+jt->malloctotalremote; I hwmk = jt->malloctotalhwmk;
  if(AN(w)){I new; RE(new=i0(w)); jt->malloctotalhwmk=new;}
  R v2(curr,hwmk);
}

DF1(jtspit){F12IP;A z;I k; 
 F1RANK(1,jtspit,self);
 jt->bytesmax=k=spstarttracking(); A *old=jt->tnextpushp;  // start keeping track of bytesmax
 z=exec1(w); spendtracking();  // end tracking, even if there was an error
 RZ(z);
 tpop(old); // pop tstack so that a returned arg is taken off & usecount decremented - but not if error, which may set pm stack
 R sc(jt->bytesmax-k);
}   // 7!:2, calculate max space used

#if defined(__linux__)
typedef struct {
 unsigned long size,resident,share,text,lib,data,dt;
} statm_t;

static int read_off_memory_status(statm_t *result)
{
 const char* statm_path = "/proc/self/statm";
 FILE *f = fopen(statm_path,"r");
 if(!f) R 1;
 if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld", &result->size,&result->resident,&result->share,&result->text,&result->lib,&result->data,&result->dt)) { fclose(f); R 1; }
 fclose(f); R 0;
}
#endif

// 7!:7
// Return resident memory of the current process
F1(jtspresident){F12IP;
ASSERTMTV(w);
#if defined(__wasm__)
 R v2(0, 0);    // not implemented 
#elif defined(_WIN32)
 PROCESS_MEMORY_COUNTERS mem;
 BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), &mem, sizeof(mem));
 ASSERT(res != 0,EVFACE);
 R v2((I)mem.WorkingSetSize, (I)mem.PeakWorkingSetSize);
#elif defined(__APPLE__)
 mach_task_basic_info_data_t info;
 info.virtual_size = 0;
 mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
 kern_return_t res = task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count);
 ASSERT(res == KERN_SUCCESS,EVFACE);
 R v2((I)info.resident_size, (I)info.resident_size_max);
#else
// posix
 struct rusage mem;
 int res = getrusage(RUSAGE_SELF, &mem);
 ASSERT(res == 0,EVFACE);
#if defined(__linux__)
 statm_t result;
 if (read_off_memory_status(&result))
  R v2(1024*(I)mem.ru_maxrss, 1024*(I)mem.ru_maxrss);   // linux only implemented max rss
 else
  R v2(4096*(I)result.resident, 1024*(I)mem.ru_maxrss);
#elif defined(__OpenBSD__)
 kvm_t *kd=kvm_open(NULL,NULL,NULL,KVM_NO_FILES,"kvm_open");
 ASSERT(kd,EVFACE);
 int cnt;
 struct kinfo_proc *p=kvm_getprocs(kd,KERN_PROC_PID,getpid(),sizeof(struct kinfo_proc),&cnt);
 if(unlikely(cnt!=1)){kvm_close(kd);ASSERT(0,EVFACE);}
 A r=v2(p->p_vm_rssize*sysconf(_SC_PAGESIZE), 1024*(I)mem.ru_maxrss);
 kvm_close(kd);
 R r;
#else
 R v2(1024*(I)mem.ru_maxrss, 1024*(I)mem.ru_maxrss);   // linux only implemented max rss
#endif
#endif
}

F1(jtparsercalls){F12IP;ASSERTMTV(w); R sc(jt->parsercalls);}

// 6!:5, window into the running J code
F1(jtpeekdata){F12IP;ARGCHK1(w);  I opeek=JT(jt,peekdata);
 JT(jt,peekdata)=i0(w);
#if ((MEMAUDIT&5)==5) && SY_64 // scaf
extern I allorunin, nalloblocks;
 allorunin=JT(jt,peekdata); if(allorunin==0)nalloblocks=0;  // reset remembered blocks on store
#endif
 R sc(opeek); }

// 13!:_9, set/get recurstate
F1(jtsetgetrecurstate){F12IP;I orstate=jt->recurstate; if(AN(w)){jt->recurstate=i0(w);} R sc(orstate); }

// 13!:_10, call JDo to execute sentence
F1(jtcallJDo){F12IP;ARGCHK1(w); PROLOG(0); ASSERT(AR(w)<=1,EVRANK) RZ(w=mkwris(str(AN(w),CAV(w)))) CAV(w)[AN(w)]=0; A z=sc(JDo(JJTOJ(jt),CAV(w))); EPILOG(z)  }

#if SY_WIN32
 /* defined in jdll.c */
#else
F1(jtts){F12IP;A z;D*x;struct tm tr,*t=&tr;struct timeval tv;
 ASSERTMTV(w);
 gettimeofday(&tv,NULL); t=localtime_r((time_t*)&tv.tv_sec,t);
 GAT0(z,FL,6,1); x=DAV1(z);
 x[0]=t->tm_year+1900;
 x[1]=t->tm_mon+1;
 x[2]=t->tm_mday;
 x[3]=t->tm_hour;
 x[4]=t->tm_min;
 x[5]=t->tm_sec+(D)tv.tv_usec/1e6;
 R z;
}
#endif

// 6!:0
F1(jtts0){F12IP;A x,z;C s[9],*u,*v,*zv;D*xv;I n,q;
 ARGCHK1(w);
 ASSERT(1>=AR(w),EVRANK);
 RZ(x=ts(mtv));
 n=AN(w); xv=DAV(x);
 if(!n)R x;
 if(!ISDENSETYPE(AT(w),LIT))RZ(w=cvt(LIT,w));
 I zr=AR(w); GATV(z,LIT,n,AR(w),AS(w)); zv=CAVn(zr,z); MC(zv,CAV(w),n);
 q=0; v=zv; DQ(n, q+='Y'==*v++;); u=2==q?s+2:s;   // if only 2 Y, advance over century
 sprintf(s,FMTI04,(I)xv[0]);             v=zv; DQ(n, if(*v=='Y'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[1]);        u=s; v=zv; DQ(n, if(*v=='M'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[2]);        u=s; v=zv; DQ(n, if(*v=='D'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[3]);        u=s; v=zv; DQ(n, if(*v=='h'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI02,(I)xv[4]);        u=s; v=zv; DQ(n, if(*v=='m'){*v=*u++; if(!*u)break;} ++v;);
 sprintf(s,FMTI05,(I)(1000*xv[5])); u=s; v=zv; DQ(n, if(*v=='s'){*v=*u++; if(!*u)break;} ++v;);
 R z;
}


#ifdef SY_GETTOD
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

static const D qpm=4294967296.0;  /* 2^32 */

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


F1(jttss){F12IP;ASSERTMTV(w); R scf(tod()-JT(jt,tssbase));}

// 6!:2 dyad
DF2(jttsit2){F12IP;A z;D t;I n;I stackallo=0,i;
 F2RANK(0,1,jttsit2,self);
 PROLOG(000);
 RE(n=i0(a));
 RZ(w=ddtokens(vs(w),4+1+!!EXPLICITRUNNING));   // tokenize outside of timer.  We time as if the sentence were executed in an explicit defn
 // apply pppp to the sentence.  Create a 1-sentence block of control words
 A cwa; GAT0(cwa,LIT,2*sizeof(CW),1) AN(cwa)=1; CW *cwv=(CW*)voidAV1(cwa); cwv[0].tcesx=0+(CBBLOCK<<TCESXTYPEX); cwv[1].tcesx=AN(w);  // allo 2 CWs, of which 1 is the end marker
 RZ(w=mkwris(w)) RZ(pppp(jt,w,cwa))  // make sure we can write to w, and perform pppp on it
 I wn=AN(w); A *wv=AAV(w);  // get #words in sentence after pppp, and their address
 // The names don't have bucket info because we aren't creating an explicit definition.  That causes any assignment to search the local symbol table.  To avoid this time, we set bucket info in each assigned simple name:
 // to whatever is in the current local symbol table, or -1 if there is none
 for(i=0;i<wn;++i){
  if(QCPTYPE(wv[i])==QCNAMEASSIGNED || (QCTYPE(wv[i])&QCISLKPNAME)) { //  if a name, either assigned or used
   if((NAV(QCWORD(wv[i]))->flag&NMLOC+NMILOC+NMIMPLOC)==0){  // if locative, leave bucket info empty
    if(!EXPLICITRUNNING){NAV(QCWORD(wv[i]))->bucket=-1;  // run from keyboard, use -1 for bucket to suppress check for local sym
    }else{  // we are in an explicit definition
     // look up the name in the local symbol table
     UI4 hash=NAV(QCWORD(wv[i]))->hash; A g=jt->locsyms;  // hash, and pointer to (local) symbol table to look in
     I4 bucket=(I4)SYMHASH(hash,AN(g)-SYMLINFOSIZE);  // bucket number of name hash
     NAV(QCWORD(wv[i]))->bucket=bucket;  
     LX symx=LXAV0(g)[bucket];  // get index of start/next of chain.  0 at EOC.  flagged if the item pointed to is not PERMANENT 
     L *sympv=SYMORIGIN;  // base of symbol table
     L *sym=sympv+symx;  // first/next symbol address - might be the free root if symx is 0
     I symno=0;  // # symbols in chain before the one we found
     NOUNROLL while(SYMNEXTISPERM(symx)){  // loop is unrolled 1 time.  To match calclocalbuckets, we count only the permanent symbols
      // sym is the symbol to process, symx is its index.  Start by reading next in chain.  One overread is OK, will be symbol 0 (the root of the freequeue)
      IFCMPNAME(NAV(sym->name),NAV(QCWORD(wv[i]))->s,NAV(QCWORD(wv[i]))->m,hash,goto foundsym;)     // (1) exact match - if there is a value, return it.  valtype has QCSYMVAL semantics
      sym=sympv+(symx=sym->next);  // move to next symbol.  symx becomes invalid after last permanent
      ++symno;  // advance number of symbol being looked up
     }
     NAV(QCWORD(wv[i]))->bucketx=symno; NAV(QCWORD(wv[i]))->symx=0;  // the symbol was not found in the local symbol table.  bucketx will be the # permanent symbols, with no local symbol#
     if(0){
foundsym:;  // we found a matching symbol.  Switch over to it.  It might not have buckets, if it is an x/y that has was not used in the definition.
      wv[i]=MAKEFVAL(sym->name,QCTYPE(wv[i]));  // new address, old NAME type
// obsolete   If it has buckets, switch our pointer over to using it.  That way we will share the nameblock
// obsolete       if(NAV(sym->name)->symx){wv[i]=MAKEFVAL(sym->name,QCTYPE(wv[i]));  // new address, old type
// obsolete       }else{  // no name with buckets.  Install the position at end of chain
// obsolete       NAV(QCWORD(wv[i]))->bucketx=~symno;  // install complement of position in chain to indicate found position
// obsolete        NAV(QCWORD(wv[i]))->symx=AR(jt->locsyms)&ARLCLONED?0:SYMNEXT(symx);  // install symbol number if this is not a cloned definition
       // note: we don't have to worry about erasing references because symbols might escape from a modifier: the result of the sentence is unused.  We have slightly better bucket info than usual, no crime
     }
    }
   }
   // if the name is not shared, it is not a simple local name.
   // If it is also not indirect or mnuvxy, it is eligible for caching - if that is enabled
   if(QCPTYPE(wv[i])!=QCNAMEASSIGNED && (jt->namecaching & !(NAV(QCWORD(wv[i]))->flag&(NMILOC|NMMNUVXY|NMIMPLOC|NMSHARED))))NAV(QCWORD(wv[i]))->flag|=NMCACHED;
  }
 }
 STACKCHKOFL  // in case the sentence calls 6!:2 again, break the loop
 if(unlikely(jt->uflags.trace&TRACEDB1)||unlikely(jt->sitop!=0)){  // We don't need a new stack frame if there is one already and debug is off
  RZ(deba(DCPARSE,wv,(A)wn,0L)); stackallo=1;
 }
 A *old=jt->tnextpushp;
 t=qpc();  // start time
 // We attempt to time as if under jtxdefn, so we check ATTN and jt->jerr as a replacement for reading from the word block; and we tpop like jtxdefn
 DQ(n, z=PARSERVALUE(parsea(wv,wn)); if(unlikely(!z))break; ASSERTGOTO((__atomic_load_n((S*)JT(jt,adbreakr),__ATOMIC_ACQUIRE)&3)==0,EVATTN,exiterr) REGOTO(0,exiterr) if((UI)jt->tnextpushp-(UI)old>TPOPSLACKB*SZI)tpop(old););
 t=qpc()-t; // Run the sentence.  No need to run as exec since the result doesn't escape.  tpop like jtxdefn.  no tpop on error.
exit: ;
 if(unlikely(stackallo))debz();
 RZ(z);  // if error, fail the timing request
 z=scf(n?t/(n*pf):0);   // convert processor freq to seconds, get time per iteration
 EPILOG(z)
exiterr: z=0; goto exit;   // clear rc before cleanup
}


// 6!:2 monad
F1(jttsit1){F12IP;R tsit2(num(1),w);}

#ifdef _WIN32
#define sleepms(i) Sleep(i)
#else
#define sleepms(i) usleep(i*1000)
#endif

// 6!:3
F1(jtdl){F12IP;D m,n,*v;UINT ms,s;
 RZ(w=ccvt(FL,w,0));
 n=0; v=DAV(w); DQ(AN(w), m=*v++; ASSERT(0<=m,EVDOMAIN); n+=m;);
#if PYXES
 C sr=jtjsleep(jt,(UI)jfloor(n*1e9));ASSERT(!sr,sr);
#else
 s=(I)jfloor(n); ms=(I)jround(1000*(n-s));
#if SYS & SYS_MACINTOSH
 {I t=TickCount()+(I)(60*n); while(t>TickCount())JBREAK0;}
#else
 DQ(s, sleepms(1000); JBREAK0;);  // wait for BREAK (2 attns)
 sleepms(ms);
#endif
#endif
 R w;
}


F1(jtqpfreq){F12IP;ASSERTMTV(w); R scf(pf);}

F1(jtqpctr ){F12IP;ASSERTMTV(w); R scf(qpc());}

// 6!:12
F1(jtpmctr){F12IP;D x;I q;
 RE(q=i0(w));
 ASSERT(JT(jt,pma),EVDOMAIN);
 x=q+(D)((PM0*)(CAV1(JT(jt,pma))))->pmctr;
 ASSERT(IMIN<=x&&x<FLIMAX,EVDOMAIN);
 ((PM0*)(CAV1(JT(jt,pma))))->pmctr=q=(I)x; if(q)jt->uflags.trace|=TRACEPM;else jt->uflags.trace&=~TRACEPM; // tell cx and unquote to look for pm
 R sc(q);
}    /* add w to pmctr */

static F1(jtpmfree){F12IP;A x,y;C*c;I m;PM*v;PM0*u;
 if(w){
  c=CAV(w); u=(PM0*)c; v=(PM*)(c+sizeof(PM0)); 
  m=u->wrapped?u->n:u->i; 
// obsolete   DQ(m, x=v->name; if(x&&NAME&AT(x)&&AN(x)==AS(x)[0])fa(x); 
// obsolete         y=v->loc;  if(y&&NAME&AT(y)&&AN(y)==AS(y)[0])fa(y); ++v;);
  DQ(m, x=v->name; if(x)fa(x); y=v->loc;  if(y)fa(y); ++v;);  // free every block we added (we did ras then)
  fa(w);
 }
 R num(1);
}    /* free old data area */

F1(jtpmarea1){F12IP;R pmarea2(vec(B01,2L,&zeroZ),w);}  // 6!:10

// 6!:10  y is data area to use   x is 2-ele list (record all lines),(wrap the buffer).  Result is #entries
F2(jtpmarea2){F12IP;A x;B a0,a1,*av;C*v;I an,n=0,s=sizeof(PM),s0=sizeof(PM0),wn;PM0*u;
 ARGCHK2(a,w);
 RZ(a=cvt(B01,a));  // force x boolean
 an=AN(a);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(2>=an,EVLENGTH);  // x must be list/atom no more than 2 long
 av=BAV(a); 
 a0=0<an?av[0]:0;  // a0 set for 'record all'
 a1=1<an?av[1]:0;  // a1 set for 'wrap buffer'
 RZ(w=vs(w)); RZ(w=mkwris(w));  // make buffer a rank-1 list, and make sure it is writable
 wn=AN(w);  // wn=length in bytes
 ASSERT(!wn||wn>=s+s0,EVLENGTH);  // make sure it can record at least 1 sample
 x=JT(jt,pma);   // read incumbent sample buffer
 jt->uflags.trace&=~TRACEPM;  // clear sample counter and tracking status
 if(wn){ras(w); JT(jt,pma)=w;}else JT(jt,pma)=0;  // set new buffer address, if it is valid
 if(JT(jt,pma))spstarttracking();else spendtracking();  // track memory usage whenever PM is running
 RZ(pmfree(x));  // free the old buffer and all its contents, if there was one
 if(wn){  // if we are turning on profiling
  v=CAV1(w);   // we put a PM0 struct at the beginning of the buffer, and use the rest for PM records
  u=(PM0*)v; 
  u->rec=a0;
  u->n=n=(wn-s0)/s;   // fill in the header block
  u->i=0;
  u->s=jt->bytesmax=spbytesinuse();
  u->trunc=a1; 
  u->wrapped=0;
  u->pmctr=0;
 }
 R sc(n);
}

// Add an entry to the Performance Monitor area
// name and loc are A blocks for the name and current locale
// lc is the line number being executed, or _1 for start function, _2 for end function
// val is the valence 
void jtpmrecord(J jt,A name,A loc,I lc,int val){A x,y;B b;PM*v;PM0*u;
 u=(PM0*)CAV1(JT(jt,pma));  // u-> pm control area
 v=(PM*)(CAV1(JT(jt,pma))+sizeof(PM0))+u->i;  // v -> next PM slot to fill
 if(b=u->wrapped){x=v->name; y=v->loc;}  // If this slot already has valid name/loc, extract those values for free
 ++u->i;  // Advance index to next slot
 if(u->i>=u->n){u->wrapped=1; if(u->trunc){u->i=u->n-1; R;}else u->i=0;}  // If we stepped off the end,
  // reset next pointer to 0 (if not trunc) or stay pegged at then end (if trunc).  Trunc comes from the original x to start_jpm_
 v->name=name; if(name)ras(name);  // move name/loc; incr use counts
 v->loc =loc;  if(loc)ras(loc); if(b){fa(x); fa(y);}  // If this slot was overwritten, decr use counts, freeing
 v->val =val;  // Save valence
 v->lc  =lc;  // save line#/start/stop
 v->s=jt->bytesmax-u->s;
 u->s=jt->bytesmax=jt->bytes;
#if SY_WIN32
 QueryPerformanceCounter((LI*)v->t);  // Save PM info
#else
 {D d=tod(); MC(v->t,&d,sizeof(D));}
#endif
}

// 6!:11
F1(jtpmunpack){F12IP;A*au,*av,c,t,x,z,*zv;B*b;D*dv;I*iv,k,k1,m,n,p,q,wn,*wv;PM*v,*v0,*vq;PM0*u;
 ARGCHK1(w);
 ASSERT(JT(jt,pma),EVDOMAIN);
 if(!ISDENSETYPE(AT(w),INT))RZ(w=cvt(INT,w));
 wn=AN(w); wv=AV(w);
 u=(PM0*)AV(JT(jt,pma)); p=u->wrapped?u->n-u->i:0; q=u->i; n=p+q;
 GATV0(x,B01,n,1); b=BAV1(x); mvc(n,b,1,iotavec-IOTAVECBEGIN+(wn?C0:C1));
 if(wn){
  DO(wn, k=wv[i]; if(0>k)k+=n; ASSERT((UI)k<(UI)n,EVINDEX); b[k]=1;);
  m=0; 
  DO(n, if(b[i])++m;);
 }else m=n;
 v0=(PM*)(CAV1(JT(jt,pma))+sizeof(PM0)); vq=q+v0;
 GAT0(z,BOX,1+PMCOL,1); zv=AAV1(z);
 GATV0(t,BOX,2*m,1); av=AAV1(t); au=m+av;
 v=vq; DO(p, if(b[  i]){RZ(*av++=v->name?incorp(sfn(0,v->name)):mtv); RZ(*au++=v->loc?incorp(sfn(0,v->loc)):mtv);} ++v;); 
 v=v0; DO(q, if(b[p+i]){RZ(*av++=v->name?incorp(sfn(0,v->name)):mtv); RZ(*au++=v->loc?incorp(sfn(0,v->loc)):mtv);} ++v;); 
 RZ(x=indexof(t,t));
 RZ(c=eq(x,IX(SETIC(x,k1))));
 RZ(zv[6]=incorp(repeat(c,t)));
 RZ(x=indexof(repeat(c,x),x)); iv=AV(x);
 RZ(zv[0]=incorp(vec(INT,m,  iv)));
 RZ(zv[1]=incorp(vec(INT,m,m+iv)));
 GATV0(t,INT,m,1); zv[2]=incorp(t); iv=AV1(t); v=vq; DO(p, if(b[i])*iv++=(I)v->val;  ++v;); v=v0; DO(q, if(b[p+i])*iv++=(I)v->val; ++v;);
 GATV0(t,INT,m,1); zv[3]=incorp(t); iv=AV1(t); v=vq; DO(p, if(b[i])*iv++=v->lc; ++v;); v=v0; DO(q, if(b[p+i])*iv++=v->lc; ++v;);
 GATV0(t,INT,m,1); zv[4]=incorp(t); iv=AV1(t); v=vq; DO(p, if(b[i])*iv++=v->s;  ++v;); v=v0; DO(q, if(b[p+i])*iv++=v->s;  ++v;); 
 GATV0(t,FL, m,1); zv[5]=incorp(t); dv=DAV1(t);
#if SY_WIN32
 v=vq; DO(p, if(b[i]  )*dv++=(((LI*)v->t)->LowPart+qpm*((LI*)v->t)->HighPart)/pf; ++v;);
 v=v0; DO(q, if(b[p+i])*dv++=(((LI*)v->t)->LowPart+qpm*((LI*)v->t)->HighPart)/pf; ++v;);
#else
 v=vq; DO(p, if(b[i]  ){MC(dv,v->t,sizeof(D)); ++dv;} ++v;); 
 v=v0; DO(q, if(b[p+i]){MC(dv,v->t,sizeof(D)); ++dv;} ++v;); 
#endif
 R z;
}

// 6!:13
F1(jtpmstats){F12IP;A x,z;I*zv;PM0*u;
 ASSERTMTV(w);
 GAT0(z,INT,6,1); zv=AV1(z);
 if(x=JT(jt,pma)){
  u=(PM0*)AV(x);
  zv[0]=u->rec;
  zv[1]=u->trunc;
  zv[2]=u->n;
  zv[3]=u->wrapped?u->n:u->i;
  zv[4]=u->wrapped;
  zv[5]=((PM0*)(CAV1(JT(jt,pma))))->pmctr;
 }else zv[0]=zv[1]=zv[2]=zv[3]=zv[4]=zv[5]=0;
 R z;
}

