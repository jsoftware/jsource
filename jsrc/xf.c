/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Files                                                            */

/* File functions accept file number or boxed file name or 1 or 2          */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#define filesep '\\'
#else
#if defined(__GNUC__) && defined(_GNU_SOURCE)
#if !defined(__wasm__) && !defined(TARGET_IOS)
#include <dlfcn.h>
#endif
#endif
#include <sys/types.h>
#include <unistd.h>
#if !defined(__wasm__)
#include <fts.h>
#endif
#define filesep '/'
#ifdef ANDROID
/*
 * Strictly these functions were available before Lollipop/21, but there was an accidental ABI
 * breakage in 21 that means you can't write code that runs on current devices and pre-21 devices,
 * so we break the tie in favor of current and future devices.
 */

FTSENT* fts_children(FTS* __fts, int __options);
int fts_close(FTS* __fts);
FTS* fts_open(char* const* __path, int __options, int (*__comparator)(const FTSENT** __lhs, const FTSENT** __rhs));
FTSENT* fts_read(FTS* __fts);
int fts_set(FTS* __fts, FTSENT* __entry, int __options);
#endif
#endif

#include "j.h"
#include "x.h"

#if !SY_WIN32 && (SYS & SYS_DOS)
#include <dos.h>
#endif

#if (SYS & SYS_UNIX)
#include <stdlib.h>
typedef long long INT64;
static int rmdir2(const char *dir);
#endif

#if SY_WIN32 && !SY_WINCE
#include <direct.h>
#include <io.h>
static int rmdir2(J jt, const wchar_t *dir);
#endif


#if SY_64
static I fsize(F f){
 if(!f)R 0;
#if SY_WIN32
 R _filelengthi64(_fileno(f));
#elif defined(__linux__)
 //work around strange linux issue: for certain files in /proc (eg /proc/meminfo), fstat will return with S_ISREG set, but a size of 0 (even
 //though the file is not empty), and glibc gets confused; work around it by prodding the fd directly
 int fd=fileno(f);
 off_t c=lseek(fd,0,SEEK_CUR);if(c<0)R -1; //save current fd position, to avoid messing with libc internal state
 off_t z=lseek(fd,0,SEEK_END);if(z<0)R -1;
 lseek(fd,c,SEEK_SET); //restore fd position.  Nothing really to be done if this fails...
 R z;
#else
 struct stat stat;
 if(fstat(fileno(f),&stat))R -1;
 if(!S_ISREG(stat.st_mode))R -1;
 R stat.st_size;
#endif
}
#else
static I fsize(F f){
 RZ(f);
#if SY_WIN32
 R _filelength(_fileno(f));
#else
 if(fseek(f,0L,SEEK_END))R -1;
 R ftell(f);
#endif
}
#endif

static A jtrdns(J jt,F f){A za,z;I n=512;size_t r,tr=0;
 GAT0(za,LIT,512,1); clearerr(f);
 NOUNROLL while(!feof(f) && (r=fread(CAV(za)+tr,sizeof(C),n-tr,f))){
  tr+=r; if(tr==(U)n){RZ(za=ext(0,za));n*=2;}
 }
 if(tr==(U)n)z=za;
 else {GATV0(z,LIT,(I)tr,1); MC(CAV1(z),CAV(za),tr);}
 R z;
}    /* read entire file stream (non-seekable) */

A jtrd(J jt,F f,I j,I n){A z;C*x;I p=0;size_t q=1;
 RZ(f);
 if(0>n){if(j<0) n=-j; else n=fsize(f)-j;}

#if !SY_WINCE
 {INT64 v; v= j+((0>j)?fsize(f):0); fsetpos(f,(fpos_t*)&v);}
#else
 fseek(f,(long)(0>j?1+j:j),0>j?SEEK_END:SEEK_SET);
#endif

 clearerr(f);
 GATV0(z,LIT,n,1); x=CAV1(z);
 NOUNROLL while(q&&n>p){
  p+=q=fread(p+x,sizeof(C),(size_t)(n-p),f);
  if(ferror(f))R jerrno();
 }
 R z;
}    /* read file f for n bytes at j */

static B jtwa(J jt,F f,I j,A w){C*x;I n,p=0;size_t q=1;
 RZ(f&&w);
 n=AN(w)*(C4T&AT(w)?4:C2T&AT(w)?2:1); x=CAV(w);

#if !SY_WINCE
 {INT64 v; v= j+((0>j)?fsize(f):0); fsetpos(f,(fpos_t*)&v);}
#else
 fseek(f,(long)(0>j?1+j:j),0>j?SEEK_END:SEEK_SET);
#endif
 
 clearerr(f);
 NOUNROLL while(q&&n>p){
  p+=q=fwrite(p+x,sizeof(C),(size_t)(n-p),f); 
  if(ferror(f))R jerrno()?1:0;
 }
 R 1;
}    /* write/append string w to file f at j */

// 1!:1 read entire file
DF1(jtjfread){A z;F f,fp;
 ASSERT(!JT(jt,seclev),EVSECURE) 
 F1RANK(0,jtjfread,self);
 RE(f=stdf(w));  // f=file#, or 0 if w is a filename
 if(f){ // if special file, read it all, possibly with error
  if(1==(I)f)R jgets("\001");
  if(3==(I)f)R rdns(stdin);
  RZ(fp=vfn(f));} //ensure actual file pointer
 else{RZ(fp=jope(w,FREAD_O));} //otherwise open named file
 I fl=fsize(fp);
 z=fl<0?rdns(fp):rd(fp,0,fl);
 if(!f){fclose(fp);} //if we opened the file, close it
 else{jtunvfn(jt,f,0);} //otherwise, release the lock on it
 RETF(z);}

// 1!:2
DF2(jtjfwrite){B b;F f;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F2RANK(RMAX,0,jtjfwrite,self);
 if(BOX&AT(w)){ASSERT(1>=AR(a),EVRANK); ASSERT(!AN(a)||AT(a)&LIT+C2T+C4T,EVDOMAIN);}
 RE(f=stdf(w));
 if(2==(I)f){jtjpr((J)((I)jt|MTYOFILE),a); R a;}  // for write to stdout, convert to printable string and type out, with NOSTDOUT off
 if(4==(I)f){R (U)AN(a)!=fwrite(CAV(a),sizeof(C),AN(a),stdout)?jerrno():a;}
 if(5==(I)f){R (U)AN(a)!=fwrite(CAV(a),sizeof(C),AN(a),stderr)?jerrno():a;}
#ifdef ANDROID
 if(6==(I)f){A z=tocesu8(w); __android_log_write(ANDROID_LOG_DEBUG,(const char*)"libj",CAV(z)); R a;}
#endif
 if(b=!f)RZ(f=jope(w,FWRITE_O)) else RE(vfn(f)); 
 wa(f,0L,a); 
 if(b)fclose(f);else{fflush(f); jtunvfn(jt,f,0);}  // if numbered file, remove the inuse mark
 RNE(mtm);
}

// 1!:3
DF2(jtjfappend){B b;F f;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F2RANK(RMAX,0,jtjfappend,self);
 RE(f=stdf(w));
 if(2==(I)f){jpr(a); R a;}  // this forces typeout, with NOSTDOUT off
 ASSERT(!AN(a)||AT(a)&LIT+C2T+C4T,EVDOMAIN);
 ASSERT(1>=AR(a),EVRANK);
 if(b=!f)RZ(f=jope(w,FAPPEND_O)) else RE(vfn(f));
 wa(f,fsize(f),a);
 if(b)fclose(f);else{fflush(f); jtunvfn(jt,f,0);}  // if numbered file, remove the inuse mark
 RNE(mtm);
}

DF1(jtjfsize){B b;F f;I m;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F1RANK(0,jtjfsize,self);
 RE(f=stdf(w));
 if(b=!f)RZ(f=jope(w,FREAD_O)) else RE(vfn(f)); 
 m=fsize(f); 
 if(b)fclose(f);else{fflush(f); jtunvfn(jt,f,0);}  // if numbered file, remove the inuse mark
 RNE(sc(m));
}

// process index file arg for file number; return 0 if error or file name
// if the result is non0, vfn has been called to lock the file#
static F jtixf(J jt,A w){F f;
 ASSERT(2<=AN(w),EVLENGTH);
 switch(CTTZNOFLAG(AT(w))){
 default:  ASSERT(0,EVDOMAIN);
 case B01X: ASSERT(0,EVFNUM);
 case BOXX: ASSERT(2==AN(w),EVLENGTH); f=stdf(head(w)); break; 
 case INTX: f=(F)AV(w)[0]; ASSERT(2<(UI)f,EVFNUM);
 }
 R f?vfn(f):f;
}

// s is length of file; return index/length of substr
static B jtixin(J jt,A w,I s,I*i,I*n){A in,*wv;I j,k,m,*u;
 if(AT(w)&BOX){wv=AAV(w);  RZ(in=vi(C(wv[1]))); k=AN(in); u=AV(in);}
 else{in=w; k=AN(in)-1; u=1+AV(in);}
 ASSERT(1>=AR(in),EVRANK);
 ASSERT(k&&k<=(n?2:1),EVLENGTH);
 j=u[0]; j=0>j?s+j:j; m=1==k?s-j:u[1];  // j is index, m is length
 ASSERT(0<=j&&(!n||j<=s&&j+m<=s&&0<=m),EVINDEX);
 *i=j; if(n)*n=m;
 R 1;
}    /* process index file arg for index and length */

// 1!:11
DF1(jtjiread){A z=0;B b;F f;I i,n;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F1RANK(1,jtjiread,self);
 RE(f=ixf(w)); if(b=!f)RZ(f=jope(w,FREAD_O));  // b=filename, not number; if name, open the named file
 if(ixin(w,fsize(f),&i,&n))z=rd(f,i,n);
 if(b)fclose(f);else{fflush(f); jtunvfn(jt,f,0);}  // if numbered file, remove the inuse mark
 R z;
}

// 1!:12
DF2(jtjiwrite){B b;F f;I i;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F2RANK(RMAX,1,jtjiwrite,self);
 ASSERT(!AN(a)||AT(a)&LIT+C2T+C4T,EVDOMAIN);
 ASSERT(1>=AR(a),EVRANK);
 RE(f=ixf(w)); if(b=!f)RZ(f=jope(w,FUPDATE_O));  // b=filename, not number; if name, open the named file
 if(ixin(w,fsize(f),&i,0L))wa(f,i,a); 
 if(b)fclose(f);else{fflush(f); jtunvfn(jt,f,0);}  // if numbered file, remove the inuse mark
 RNE(mtm);
}


#if (SYS & SYS_MACINTOSH)

static B setparm(C*v,C*ms,HParamBlockRec mp){I n;
 n=strlen(v);
 ASSERT(n<=NPATH,EVLIMIT); *ms=n; MC(1+ms,v,n);
 mp.fileParam.ioNamePtr=ms;
 mp.fileParam.ioVRefNum=0;
 mp.fileParam.ioDirID  =0;
 R 1;
}

#define DIRF(f,fsub)  \
 B f(J jt,C*v){C ms[256];HParamBlockRec mp; \
  RZ(setparm(v,ms,mp));                     \
  ASSERT(!fsub(&mp,0),EVFACE);              \
  R 1;                                      \
 }

static DIRF(jtmkdir1,PBDirCreate)
static DIRF(jtrmdir1,PBHDelete  )

static B mkdir(C*v){R!mkdir1(v);}
static B rmdir(C*v){R!rmdir1(v);}

#endif


DF1(jtjmkdir){A y,z;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F1RANK(0,jtjmkdir,self);
 ASSERT(AT(w)&BOX,EVDOMAIN);
 RZ(y=str0(vslit(C(AAV(w)[0]))));
#if (SYS & SYS_UNIX)
 R mkdir(CAV(y),0775)?jerrno():num(1);
#else
 RZ(z=toutf16x(y)); USAV(z)[AN(z)]=0;  // install termination
 R _wmkdir(USAV(z))?jerrno():num(1);
#endif
}

// 1!:55
DF1(jtjferase){A y,fn;US*s;I h;
 ASSERT(!JT(jt,seclev),EVSECURE)
 F1RANK(0,jtjferase,self);
 RE(h=fnum(w));
 if(h) {ASSERT((y=fname(sc(h)))!=0,EVFNUM) RZ(y=str0(y))} else ASSERT((y=vslit(C(AAV(w)[0])))!=0,EVFNUM);
 if(h)RZ(jclose(sc(h)));
#if (SYS&SYS_UNIX)
 A y0=str0(y); R !unlink(CAV(y0))||!rmdir(CAV(y0))?num(1):jerrno();
#else
 RZ(fn=toutf16x(y)); USAV(fn)[AN(fn)]=0;  // install termination
 s=USAV(fn);
// #if SY_WIN32 && !SY_WINCE
 R !_wunlink(s)||!_wrmdir(s)?num(1):jerrno();
#endif
}    /* erase file or directory */

F1(jtpathcwd){C path[1+NPATH];US wpath[1+NPATH];
 ASSERT(!JT(jt,seclev),EVSECURE) ASSERTMTV(w);
#if (SYS & SYS_UNIX)
 ASSERT(getcwd(path,NPATH),EVFACE);
#else
 ASSERT(_wgetcwd(wpath,NPATH),EVFACE);
 jttoutf8w(jt,path,NPATH,wpath);
#endif
 R cstr(path);
}

F1(jtpathchdir){A z;
 ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE)
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 ASSERT((LIT+C2T+C4T)&AT(w),EVDOMAIN);
#if (SYS & SYS_UNIX)
 ASSERT(!chdir(CAV(toutf8x(w))),EVFACE);
#else
 RZ(z=toutf16x(toutf8(w))); USAV(z)[AN(z)]=0;  // install termination
 ASSERT(0==_wchdir(USAV(z)),EVFACE);
#endif
 R mtv;
}

#if SY_WINCE
#define _wgetenv(s)  (0)
#endif

// 2!:5
DF1(jtjgetenv){
 ASSERT(!JT(jt,seclev),EVSECURE)
 F1RANK(1,jtjgetenv,self);
 ASSERT((LIT+C2T+C4T)&AT(w),EVDOMAIN);
#if (SYS & SYS_UNIX)
 {
  C*s;
  R(s=getenv(CAV(toutf8x(w))))?cstr(s):num(0); // toutf8x has trailing nul
 }
#else
 {
  A z; US* us;
  RZ(z=toutf16x(toutf8(w))); USAV(z)[AN(z)]=0;  // install termination
  us=_wgetenv(USAV(z));
  if(!us)R num(0);
  GATV0(z,C2T,wcslen(us),1);
  MC(USAV1(z),us,2*wcslen(us));
  R toutf8(z);
 }
#endif
 R num(0);
}

// 2!:6
F1(jtjgetpid){
 ASSERT(!JT(jt,seclev),EVSECURE)
 ASSERTMTV(w);
#if SY_WIN32
 R(sc(GetCurrentProcessId()));
#else
 R(sc(getpid()));
#endif
}

// #if (SYS & SYS_UNIX)
F1(jtpathdll){
 ASSERT(!JT(jt,seclev),EVSECURE) ASSERTMTV(w);
#if (SYS & SYS_UNIX)
char p[PATH_MAX]; extern C sopath[];
#else
char p[_MAX_PATH]; extern C sopath[];
#endif
 strcpy(p,sopath);
 if(strlen(p)&&(filesep==p[strlen(p)-1])) p[strlen(p)-1]=0;
 R cstr(p);
}

#if (SYS & SYS_UNIX) && !defined(__wasm__)
int rmdir2(const char *dir)
{
 int ret=0;
 FTS *ftsp=NULL;
 FTSENT *curr;

 char *files[]={ (char *) dir, NULL };

 // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
 //                in multithreaded programs
 // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
 //                of the specified directory
 // FTS_XDEV     - Don't cross filesystem boundaries
 ftsp=fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
 if (!ftsp) {
//  fprintf(stderr, "%s: fts_open failed: %s\n", dir, strerror(errno));
  ret=-1;
  goto finish;
 }

 while ((curr=fts_read(ftsp))) {
  switch (curr->fts_info) {
  case FTS_NS:
  case FTS_DNR:
  case FTS_ERR:
//   fprintf(stderr, "%s: fts_read error: %s\n", curr->fts_accpath, strerror(curr->fts_errno));
  break;

  case FTS_DC:
  case FTS_DOT:
  case FTS_NSOK:
  // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
  // passed to fts_open()
  break;

  case FTS_D:
  // Do nothing. Need depth-first search, so directories are deleted
  // in FTS_DP
  break;

  case FTS_DP:
  case FTS_F:
  case FTS_SL:
  case FTS_SLNONE:
  case FTS_DEFAULT:
   if (remove(curr->fts_accpath) < 0) {
//    fprintf(stderr, "%s: Failed to remove: %s\n", curr->fts_path, strerror(errno));
    ret=-1;
   }
   break;
  }
 }

finish:
 if (ftsp) {
  fts_close(ftsp);
 }

 return ret;
}
#endif
#if SY_WIN32 && !SY_WINCE
int rmdir2(J jt, const wchar_t *dir){A z;US*zv;
 SHFILEOPSTRUCTW sh;
 GATV0(z,C2T,wcslen(dir)+2,1); zv=USAV1(z);
 MC(zv,dir,wcslen(dir)*sizeof(wchar_t));
 zv[1+wcslen(dir)]=zv[wcslen(dir)]=0;  // doubly null terminated string
 sh.hwnd   = NULL;
 sh.wFunc  = FO_DELETE;
 sh.pFrom  = zv;
 sh.pTo    = NULL;
 sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
 sh.hNameMappings = 0;
 sh.lpszProgressTitle = NULL;
 R SHFileOperationW (&sh);
}
#endif
