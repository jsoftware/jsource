/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Input/Output                                                            */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#define _stdcall
#endif
#include <stdint.h>

#include "j.h"
#include "d.h"

extern void dllquit(J);

void jtwri(J jt,I type,C*p,I m,C*s){C buf[1024],*t=jt->outseq,*v=buf;I c,d,e,n;
 if(jt->tostdout){
  c=strlen(p);            /* prompt      */
  e=strlen(t);            /* end-of-line */
  n=sizeof(buf)-(c+e+1);  /* main text   */
  d=m>n?n-3:m;
  MC(v,p,c); v+=c;
  MC(v,s,d); v+=d; if(m>n){MC(v,"...",3L); v+=3;}
  MC(v,t,e); v+=e; 
  *v=0;
#ifdef ANDROID
  A z=tocesu8(str(strlen(buf),buf));
  *(CAV(z)+AN(z))=0;
  jsto(jt,type,CAV(z));
#else
  jsto(jt,type,buf);
#endif
}}

static void jtwrf(J jt,I n,C*v,F f){C*u,*x;I j=0,m;
 while(n>j){
  u=j+v; 
  m=(x=memchr(u,CLF,n-j))?1+x-u:n-j; 
  fwrite(u,sizeof(C),m,f);
  j+=m;
}}

A jtinpl(J jt,B b,I n,C*s){C c;I k=0;
 if(n&&(c=*(s+n-1),CLF==c||CCR==c))--n;
#if _WIN32
 if(n&&(c=*(s+n-1),CCR==c))--n;
#endif
 ASSERT(!*jt->adbreak,EVINPRUPT);
 if(!b){ /* 1==b means literal input */
  if(n&&COFF==*(s+n-1))joff(num[0]);
  c=jt->bx[9]; if((UC)c>127)DO(n, if(' '!=s[i]&&c!=s[i]){k=i; break;});
 }
 R str(n-k,s+k);
}

static I advl(I j,I n,C*s){B b;C c,*v;
 v=j+s; 
 DO(n-j, c=*v++; b=c==CCR; if(b||c==CLF)R j+1+i+(I )(b&&CLF==*v););
 R n;
}    /* advance one line on CR, CRLF, or LF */

void breakclose(J jt);

static C* nfeinput(J jt,C* s){A y;
 jt->adbreakr=&breakdata; y=exec1(cstr(s)); jt->adbreakr=jt->adbreak;
 if(!y){breakclose(jt);exit(2);} /* J input verb failed */
 jtwri(jt,MTYOLOG,"",strlen(CAV(y)),CAV(y));
 return CAV(y); /* don't combine with previous line! CAV runs (x) 2 times! */
}

A jtjgets(J jt,C*p){A y;B b;C*v;I j,k,m,n;UC*s;
 *jt->adbreak=0;
 if(b=1==*p)p=""; /* 1 means literal input */
 if(jt->dcs){   // DCSCRIPT debug type
  ++jt->dcs->dcn; j=jt->dcs->dcix; 
  y=jt->dcs->dcy; n=AN(y); s=UAV(y);
  if(!(j<n))R 0;
  jt->dcs->dcj=k=j;
  jt->dcs->dcix=j=advl(j,n,s);
  m=j-k; if(m&&32>s[k+m-1])--m; if(m&&32>s[k+m-1])--m;
  jtwri(jt,MTYOLOG,p,m,k+s);
  R inpl(b,m,k+s);
 }
 /* J calls for input in 3 cases:
    debug suspension for normal input
    n : 0 input lines up to terminating )
    1!:1[1 read from keyboard */
 showerr();
 if(jt->nfe)
  // Native Front End
  v=nfeinput(jt,*p?"input_jfe_'      '":"input_jfe_''");
 else{
  ASSERT(jt->sminput,EVBREAK); 
  v=((inputtype)(jt->sminput))(jt,p);
 }
 R inpl(b,(I)strlen(v),v);
}


#if SYS&SYS_UNIX
void breakclose(J jt)
{
 if(jt->adbreak==&breakdata) return;
 munmap(jt->adbreak,1);
 jt->adbreakr=jt->adbreak=&breakdata;
 close((intptr_t)jt->breakfh);
 jt->breakfh=0;
 unlink(jt->breakfn);
 *jt->breakfn=0;
}
#else
void breakclose(J jt)
{
 if(jt->adbreak==&breakdata) return;
 UnmapViewOfFile(jt->adbreak);
 jt->adbreakr=jt->adbreak=&breakdata;
 CloseHandle(jt->breakmh);
 jt->breakmh=0;
 CloseHandle(jt->breakfh);
 jt->breakfh=0;
#if SY_WINCE
 DeleteFile(tounibuf(jt->breakfn));
#else
 WCHAR wpath[NPATH];
 MultiByteToWideChar(CP_UTF8,0,jt->breakfn,1+(int)strlen(jt->breakfn),wpath,NPATH);
 DeleteFileW(wpath);
#endif
 *jt->breakfn=0;
}
#endif

F1(jtjoff){I x;
 RZ(w);
 x=i0(w);
 jt->jerr=0; jt->etxn=0; /* clear old errors */
 if(jt->sesm)jsto(jt, MTYOEXIT,(C*)x); else JFree(jt);
// let front-end to handle exit
// exit((int)x);
 R 0;
}

#define capturesize 80000

I jdo(J jt, C* lp){I e;A x;
 jt->jerr=0; jt->etxn=0; /* clear old errors */
 // The named-execution stack contains information on resetting the current locale.  If the first named execution deletes the locale it is running in,
 // that deletion is deferred until the locale is no longer running, which is never detected because there is no earlier named execution to clean up.
 // To prevent the stack from growing indefinitely, we reset it here.  It would be better (perhaps) if we actually PROCESSED the stack elements we cut,
 // but we don't bother.  Another possibility would be to reset the callstack only if it was 0, so that a recursive immex will have its deletes handled by
 // the resumption of the name that was interrupted.
 I4 savcallstack = jt->callstacknext;
#if USECSTACK
 if(0x8&jt->smoption)jt->cstackmin=0;
#endif
 if(jt->capture){
  if(jt->capturemax>capturesize){FREE(jt->capture); jt->capture=0; jt->capturemax=0;} // dealloc large capture buffer
  else jt->capture[0]=0; // clear capture buffer
 }
 A *old=jt->tnextpushp;
 *jt->adbreak=0;
 x=inpl(0,(I)strlen(lp),lp);
 while(jt->iepdo&&jt->iep){jt->iepdo=0; immex(jt->iep); if(savcallstack==0)CALLSTACKRESET jt->jerr=0; tpop(old);}
 if(!jt->jerr)immex(x);
 e=jt->jerr;
 if(savcallstack==0)CALLSTACKRESET jt->jerr=0;
 while(jt->iepdo&&jt->iep){jt->iepdo=0; immex(jt->iep); if(savcallstack==0)CALLSTACKRESET jt->jerr=0; tpop(old);}
 showerr();
 spfree();
 tpop(old);
 R e;
}

#define SZINT             ((I)sizeof(int))

C* getlocale(J jt){A y=locname(mtv); y=*AAV(y); R CAV(str0(y));}

DF1(jtwd){A z=0;C*p=0;D*pd;I e,*pi,t;V*sv;
  F1PREFIP;
  F1RANK(1,jtwd,self);
  RZ(w);
  ASSERT(2>AR(w),EVRANK);
  sv=VAV(self);
  t=i0(sv->fgh[1]);  // the n arg from the original 11!:n
  if(BETWEENO(t,2000,3000)/* obsolete (UI)(t-2000)<(UI)(3000-2000)*/ && AN(w) && !(LIT+C2T+C4T+INT&AT(w))) {  // 2000<=t<3000
    switch(UNSAFE(AT(w))) {
    case B01:
      RZ(w=vi(w));
      break;
    case FL:
      pd=DAV(w);
      {A wsav=w; GATV0(w,INT,AN(wsav),AR(wsav)); }
      pi=AV(w);
      DQ(AN(w),*pi++=(I)(jround(*pd++)););
      break;
    default:
      ASSERT(0,EVDOMAIN);
    }
  }
  RZ(w=jtmemu(jtinplace,w));
// t is 11!:t and w is wd argument
// smoption:
//   1=pass current locale
//   2=result not allocated by jga
//   4=use smpoll to get last result
//   8=multithreaded
  if(0x1&jt->smoption) {
    e=jt->smdowd ? ((dowdtype2)(jt->smdowd))(jt, (int)t, w, &z, getlocale(jt)) : EVDOMAIN;
  } else {
    e=jt->smdowd ? ((dowdtype)(jt->smdowd))(jt, (int)t, w, &z) : EVDOMAIN;
  }
  if(!e) R mtm;   // e==0 is MTM
  ASSERT(e<=0,e); // e>=0 is EVDOMAIN etc
  if(0x4&jt->smoption) RZ(z=(A)((polltype)(jt->smpoll))(jt, (int)t, (int)e));
  if(0x2&jt->smoption) z=ca(z);
  if(e==-2){      // e==-2 is lit pairs
    RZ(z=df1(z,cut(ds(CBOX),num[-2])));
    RETF(reshape(v2(AN(z)>>1,2L),z));
  } else {RETF(z);}
}

static char breaknone=0;

B jtsesminit(J jt){jt->adbreakr=jt->adbreak=&breakdata; R 1;}

int _stdcall JDo(J jt, C* lp){int r;
 r=(int)jdo(jt,lp);
 while(jt->nfe){
  A *old=jt->tnextpushp; r=(int)jdo(jt,nfeinput(jt,"input_jfe_'   '")); tpop(old);
 }
 R r;
} 

C* _stdcall JGetR(J jt){
 R jt->capture?jt->capture:(C*)"";
}

/* socket protocol CMDGET name */
A _stdcall JGetA(J jt, I n, C* name){A x;
 jt->jerr=0;
 RZ(x=symbrdlock(nfs(n,name)));
 ASSERT(!(FUNC&AT(x)),EVDOMAIN);
 R binrep1(x);
}

/* socket protocol CMDSET */
I _stdcall JSetA(J jt,I n,C* name,I dlen,C* d){
 jt->jerr=0;
 if(!vnm(n,name)) R EVILNAME;
 A *old=jt->tnextpushp;
 symbisdel(nfs(n,name),jtunbin(jt,str(dlen,d)),jt->global);
 tpop(old);
 R jt->jerr;
}

/* set jclient callbacks */
void _stdcall JSM(J jt, void* callbacks[])
{
 jt->smoutput = (outputtype)callbacks[0];
 jt->smdowd = callbacks[1];
 jt->sminput = (inputtype)callbacks[2];
 jt->smpoll = (polltype)callbacks[3];
 jt->sm = 0xff & (I)callbacks[4];
 jt->qtstackinit = (SMQT==jt->sm) ? (uintptr_t)callbacks[3] : 0;
 jt->smoption = ((~0xff) & (UI)callbacks[4]) >> 8;
 if(jt->sm==SMJAVA) jt->smoption |= 0x8;  /* assume java is multithreaded */
 if(SMQT==jt->sm){
  jt->smoption = (~0x4) & jt->smoption;  /* smpoll not used */
  // If the user is giving us a better view of the stack through jt->qtstackinit, use it.  We get just the top-of-stack
  // address so we have to guess about the bottom, using our view of the minimum possible stack we have.
  // if cstackmin is 0, the user has turned off stack checking and we honor that decision
  if(jt->qtstackinit&&jt->cstackmin)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }
}

/* set jclient callbacks from values - easier for nodejs */
void _stdcall JSMX(J jt, void* out, void* wd, void* in, void* poll, I opts)
{
 jt->smoutput = (outputtype)out;
 jt->smdowd = wd;
 jt->sminput = (inputtype)in;
 jt->smpoll = (polltype)poll;
 jt->sm = 0xff & opts;
 jt->qtstackinit = (SMQT==jt->sm) ? (uintptr_t)poll : 0;
 jt->smoption = ((~0xff) & (UI)opts) >> 8;
 if(jt->sm==SMJAVA) jt->smoption |= 0x8;  /* assume java is multithreaded */
 if(SMQT==jt->sm){
  jt->smoption = (~0x4) & jt->smoption;  /* smpoll not used */
  if(jt->qtstackinit&&jt->cstackinit)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }
}

C* _stdcall JGetLocale(J jt){return getlocale(jt);}

A _stdcall Jga(J jt, I t, I n, I r, I*s){
 return ga(t, n, r, s);
}

void oleoutput(J jt, I n, char* s); /* SY_WIN32 only */

/* jsto - display output in output window */
// type is mtyo of string, s->null-terminated string
void jsto(J jt,I type,C*s){C e;I ex;
 if(jt->nfe)
 {
  // here for Native Front End state, toggled by 15!:16
  C q[]="0 output_jfe_ (15!:18)0";
  q[0]+=(C)type;
  jt->mtyostr=s;
  e=jt->jerr; ex=jt->etxn;
  jt->jerr=0; jt->etxn=0;
  jt->adbreakr=&breakdata;exec1(cstr(q));jt->adbreakr=jt->adbreak;
  jt->jerr=e; jt->etxn=ex; 
 }else{
  // Normal output.  Call the output routine
  if(jt->smoutput){((outputtype)(jt->smoutput))(jt,(int)type,s);R;} // JFE output
#if SY_WIN32 && !SY_WINCE && defined(OLECOM)
  if(jt->oleop && (type & MTYOFM)){oleoutput(jt,strlen(s),s);R;} // ole output
#endif
  // lazy - should check alloc size is reasonable
  if(!jt->capture){jt->capture=MALLOC(jt->capturemax=MAX(2+strlen(s),capturesize));strcpy(jt->capture,s);}
  else {
   if(jt->capturemax<2+strlen(jt->capture)+strlen(s))jt->capture=REALLOC(jt->capture,jt->capturemax=2+strlen(jt->capture)+strlen(s)+capturesize);
   strcat(jt->capture,s);
  }
 }
 R;
}

#if SYS&SYS_UNIX

C dll_initialized= 0; // dll init sets to 1

// dll init on load - eqivalent to windows DLLMAIN DLL_ATTACH_PROOCESS
__attribute__((constructor)) static void Initializer(int argc, char** argv, char** envp)
{
 J jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1);
 if(!jtnobdy) R;
 J jt = (J)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 memset(jt,0,sizeof(JST));
 if(!jtglobinit(jt)){free(jtnobdy); R;}
 dll_initialized= 1; jt->heap=(void *)jtnobdy;  // save allo address for later free
}

J JInit(void){
 if(!dll_initialized) R 0; // constructor failed
 J jtnobdy;
 RZ(jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1));
 J jt = (J)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 memset(jt,0,sizeof(JST));
 if(!jtjinit2(jt,0,0)){free(jtnobdy); R 0;};
 jt->heap=(void *)jtnobdy;  // save allo address for later free
 R jt;
}

// old verson - before constructor - might be needed in systems that don't have the facility
/*
J JInit(void){
 static J g_jt=0;
 J jt;

 if(!g_jt)
 {
  J g_jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1);
  if(!g_jtnobdy) R 0;
  g_jt = (J)(((I)g_jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
  memset(g_jt,0,sizeof(JST));
  if(!jtglobinit(g_jt)){free(g_jtnobdy);g_jt=0; R 0;}
  g_jt->heap=(void *)g_jtnobdy;  // save allo address for later free
 }
 J jtnobdy; RZ(jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1));
 jt = (J)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 memset(jt,0,sizeof(JST));
 if(!jtjinit2(jt,0,0)){free(jtnobdy); R 0;};
 jt->heap=(void *)jtnobdy;  // save allo address for later free
 R jt;
}
*/

// clean up at the end of a J instance
int JFree(J jt){
  if(!jt) R 0;
  breakclose(jt);
  jt->jerr=0; jt->etxn=0; /* clear old errors */
  if(jt->xep&&AN(jt->xep)){A *old=jt->tnextpushp; immex(jt->xep); fa(jt->xep); jt->xep=0; jt->jerr=0; jt->etxn=0; tpop(old); }
  dllquit(jt);  // clean up call dll
  free(jt->heap);  // free the initial allocation
  R 0;
}
#endif

F1(jtbreakfnq){
 ASSERTMTV(w);
 R cstr(jt->breakfn);
}

F1(jtbreakfns){A z;I *fh,*mh=0; void* ad;
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w)<NPATH,EVDOMAIN);
 w=str0(w);
 if(!strcmp(jt->breakfn,CAV(w))) R mtm;
 breakclose(jt);
#if SYS&SYS_UNIX
 fh=(I*)(I)open(CAV(w),O_RDWR);
 ASSERT(-1!=(I)fh,EVDOMAIN);
 ad=mmap(0,1,PROT_READ|PROT_WRITE,MAP_SHARED,(I)fh,0);
 if(0==ad){close((intptr_t)fh); ASSERT(0,EVDOMAIN);}
#else
 RZ(z=toutf16x(w));
 fh=CreateFileW(USAV(z),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
 ASSERT(INVALID_HANDLE_VALUE!=fh,EVDOMAIN);
 mh=CreateFileMapping(fh,0,PAGE_READWRITE,0,1,0);
 if(0==mh){CloseHandle(fh); ASSERT(0,EVDOMAIN);}
 ad=MapViewOfFile(mh,FILE_MAP_WRITE,0,0,0);
 if(0==ad){CloseHandle(mh); CloseHandle(fh); ASSERT(0,EVDOMAIN);}
#endif
 strcpy(jt->breakfn,CAV(w));
 jt->breakfh=fh;
 jt->breakmh=mh;
 jt->adbreakr=jt->adbreak=ad;
 R mtm;
}

int valid(C* psrc, C* psnk)
{
 while(*psrc == ' ') ++psrc;
 if(!isalpha(*psrc)) return EVILNAME;
 while(isalnum(*psrc) || *psrc=='_') *psnk++ = *psrc++;
 while(*psrc == ' ') ++psrc;
 if(*psrc) return EVILNAME;
 *psnk = 0;
 return 0;  
}

int _stdcall JGetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 A a; char gn[256];
 if(strlen(name) >= sizeof(gn)) return EVILNAME;
 if(valid(name, gn)) return EVILNAME;
 RZ(a=symbrdlock(nfs(strlen(gn),gn)));
    if(FUNC&AT(a))R EVDOMAIN;
 *jtype = AT(a);
 *jrank = AR(a);
 *jshape = (I)AS(a);
 *jdata = (I)AV(a);
 return 0;
}

static int setterm(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 A a;
 I k=1,i,n;
 char gn[256];

 switch(CTTZNOFLAG(*jtype))
 {
 case LITX:
 case B01X:
  n = sizeof(char);
  break;

 case C2TX:
  n = sizeof(unsigned short);
  break;

 case C4TX:
  n = sizeof(unsigned int);
  break;

 case INTX:
 case SBTX:
  n = sizeof(I);
  break;
  
 case FLX:
  n = sizeof(double);
  break;
  
 case CMPXX:
  n = 2 * sizeof(double);
  break;
  
 default:
  return EVDOMAIN; 
 }

 // validate name
 if(strlen(name) >= sizeof(gn)) return EVILNAME;
 if(valid(name, gn)) return EVILNAME; 
 for(i=0; i<*jrank; ++i) k *= ((I*)(*jshape))[i];
 a = ga(*jtype, k, *jrank, (I*)*jshape);
 if(!a) return EVWSFULL;
 MC(AV(a), (void*)*jdata, n*k);
 jset(gn, a);
 return jt->jerr;
}

int _stdcall JSetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 int er;

 PROLOG(0051);
 er = setterm(jt, name, jtype, jrank, jshape, jdata);
 tpop(_ttop);
 return er;
}

#define EDCBUSY -1
#define EDCEXE -2

C* esub(J jt, I ec)
{
 if(!ec) return "";
 if(ec == EDCBUSY) return "busy with previous input";
 if(ec == EDCEXE) return "not supported in EXE server";
 if(ec > NEVM || ec < 0) return "unknown error";
 return CAV(AAV(jt->evm)[ec]);
}

int _stdcall JErrorTextM(J jt, I ec, I* p)
{
 *p = (I)esub(jt, ec);
 return 0;
}

#if 0
int enabledebug=0;
F1(jttest1){
 RZ(w);
 if((AT(w)&B01+INT)&&AN(w)){
  enabledebug=i0(w);
 }
 R sc(1);
}
#endif

