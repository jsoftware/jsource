/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: file directory, attributes, & permission                         */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "j.h"
#include "x.h"

#if !SY_WINCE
char* toascbuf(char* s){ return s;}
char* tounibuf(char* s){ return s;}
#else
wchar_t* tounibuf(char* src)
{
 static wchar_t buf[2048+1];

 wchar_t* p=buf;
 if(2048>strlen(src))
 {
  while(*src) *p++=*src++;
 }
 *p=0;
 return buf;
}

char *toascbuf(wchar_t *src)
{
 static char buf[2048+1];

 char* p=buf;
 if(2048>wcslen(src))
 {
  while(*src) *p++=(char)*src++;
 }
 *p=0;
 return buf;
}
#define _A_NORMAL   FILE_ATTRIBUTE_NORMAL
#define _A_RDONLY   FILE_ATTRIBUTE_READONLY
#define _A_HIDDEN   FILE_ATTRIBUTE_HIDDEN
#define _A_SYSTEM   FILE_ATTRIBUTE_SYSTEM
#define _A_VOLID    0
#define _A_SUBDIR   FILE_ATTRIBUTE_DIRECTORY
#define _A_ARCH     FILE_ATTRIBUTE_ARCHIVE

#endif 

#if (SYS & SYS_DOS)

#if !SY_WINCE
#include <ctype.h>
#include <io.h>
#include <dos.h>
#include <direct.h>
#include <time.h>
#endif

#ifndef F_OK            /* for access() */
#define F_OK            0x00
#define X_OK            0x01
#define W_OK            0x02
#define R_OK            0x04
#endif

#ifndef _A_VOLID
#define _A_VOLID        0x00
#endif

#define _A_ALL          (_A_NORMAL+_A_RDONLY+_A_HIDDEN+_A_SYSTEM+_A_VOLID+ \
                         _A_SUBDIR+_A_ARCH)

static A jtattv(J jt,U x){A z;C*s;
 GAT0(z,LIT,6,1); s=CAV(z);
 s[0]=x&_A_RDONLY?'r':'-';
 s[1]=x&_A_HIDDEN?'h':'-';
 s[2]=x&_A_SYSTEM?'s':'-';
 s[3]=x&_A_VOLID ?'v':'-';
 s[4]=x&_A_SUBDIR?'d':'-';
 s[5]=x&_A_ARCH  ?'a':'-';
 R z;
}    /* convert from 16-bit attributes x into 6-element string */

static S jtattu(J jt,A w){C*s;I i,n;S z=0;
 RZ(w=vslit(w)); 
 n=AN(w); s=CAV(w);
 for(i=0;i<n;++i)switch(s[i]){
  case 'r': z^=_A_RDONLY; break;
  case 'h': z^=_A_HIDDEN; break;
  case 's': z^=_A_SYSTEM; break;
  case 'v': z^=_A_VOLID;  break;
  case 'd': z^=_A_SUBDIR; break;
  case 'a': z^=_A_ARCH;   break;
  case '-':               break;
  default:  ASSERT(0,EVDOMAIN);
 }
 R z;
}    /* convert from 6-element string into 16-bit attributes */

F1(jtfullname){C dirpath[_MAX_PATH];
 RZ(w=str0(vslit(w)));
#if SY_WINCE
 C*s;
 s=CAV(w); DQ(AN(w), if(' '!=*s)break; ++s;);
 if(*s=='\\'||*s=='/') strcpy(dirpath,s);
 else {strcpy(dirpath, "\\"); strcat(dirpath,s);}
#else
 wchar_t wdirpath[_MAX_PATH];
 RZ(w=toutf16x(w)); USAV(w)[AN(w)]=0;
 _wfullpath(wdirpath,USAV(w),_MAX_PATH);
 WideCharToMultiByte(CP_UTF8,0,wdirpath,1+(int)wcslen(wdirpath),dirpath,_MAX_PATH,0,0);
#endif
 R cstr(dirpath);
}

#if !SY_WINCE

F1(jtjfperm1){A y,fn,z;C *s;F f;int x; US *p,*q;
 F1RANK(0,jtjfperm1,DUMMYSELF);
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)))} else ASSERT(y=C(AAV(w)[0]),EVFNUM)
 RZ(fn=toutf16x(y)); USAV(fn)[AN(fn)]=0;  // install termination
 p=USAV(fn); q=p+AN(fn)-3;
 GAT0(z,LIT,3,1); s=CAV(z);
 x=_waccess(p,R_OK); if(0>x)R jerrno();
 s[0]=x?'-':'r';
 s[1]=_waccess(p,W_OK)?'-':'w';
 s[2]=wcscmp(q,L"exe")&&wcscmp(q,L"bat")&&wcscmp(q,L"com")?'-':'x';
 R z;
}

F2(jtjfperm2){A y,fn;C*s;F f;int x=0;US *p;
 F2RANK(1,0,jtjfperm2,DUMMYSELF);
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)))} else ASSERT(y=C(AAV(w)[0]),EVFNUM)
 RZ(a=vslit(a)); ASSERT(3==AN(a),EVLENGTH); 
 RZ(fn=toutf16x(y)); USAV(fn)[AN(fn)]=0;  // install termination
 s=CAV(y);
 p=USAV(fn);
 s=CAV(a);
 if('r'==s[0]) x|=S_IREAD;  else ASSERT('-'==s[0],EVDOMAIN);
 if('w'==s[1]) x|=S_IWRITE; else ASSERT('-'==s[1],EVDOMAIN);
 if('x'==s[2]) x|=S_IEXEC;  else ASSERT('-'==s[2],EVDOMAIN);
 R _wchmod(p,x)?jerrno():mtm;
}

#else /* SY_WINCE: */

F1(jtjfperm1){A y,z;C*p,*q,*s;F f; DWORD attr;
 F1RANK(0,jtjfperm1,DUMMYSELF);
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)))} else ASSERT(y=C(AAV(w)[0]),EVFNUM)
 p=CAV(y); q=p+AN(y)-3;
 GAT0(z,LIT,3,1); s=CAV(z);
 if((attr=GetFileAttributes(tounibuf(p)))==0xFFFFFFFF)R jerrno();
 s[0]='r';
 s[1]=attr&FILE_ATTRIBUTE_READONLY?'-':'w';
 s[2]=strcmp(q,"exe")&&strcmp(q,"bat")&&strcmp(q,"com")?'-':'x';
 R z;
}

F2(jtjfperm2){ASSERT(0,EVNONCE);}

#endif
#endif

/* jdir produces a 5-column matrix of boxes:                 */
/* 0 name                                                    */
/* 1 time of last write, y m d h m s                         */
/* 2 size                                                    */
/* 3 permission -- 0 read  1 write  2 execute                */
/* 4 attributes                                              */
/*   0 read-only    3 volume label                           */
/*   1 hidden       4 directory                              */
/*   2 system       5 archive (modified since last back-up)  */

#if SY_WIN32

#include <stdlib.h>

UINT getfileattr(char *);
int setfileattr(char*, UINT);

static A jtdir1(J jt,LPWIN32_FIND_DATAW f,C* fn) {A z,*zv;C rwx[3],*s,*t;I n,ts[6];
 FILETIME local_ftime; SYSTEMTIME x;

 FileTimeToLocalFileTime(&f->ftLastWriteTime, &local_ftime);
 FileTimeToSystemTime(&local_ftime, &x);
 ts[0]=x.wYear;   ts[1]=x.wMonth;  ts[2]=x.wDay;
 ts[3]=x.wHour;   ts[4]=x.wMinute; ts[5]=x.wSecond;
 s=fn; n=strlen(s); t=s+n-3;
 rwx[0]='r';
 rwx[1]=f->dwFileAttributes & FILE_ATTRIBUTE_READONLY ?'-':'w';
 rwx[2]=strcmp(t,"exe")&&strcmp(t,"bat")&&strcmp(t,"com")?'-':'x';
 GAT0(z,BOX,5,1); zv=AAV(z);
 RZ(zv[0]=incorp(str(n,s))); 
 RZ(zv[1]=incorp(vec(INT,6L,ts)));
#if SY_64
 RZ(zv[2]=incorp(sc(((I)f->nFileSizeHigh<<32) + (I)f->nFileSizeLow)));
#else
 RZ(zv[2]=incorp(sc(   (f->nFileSizeHigh || 0>(I)f->nFileSizeLow)?-1:f->nFileSizeLow )));  
#endif
 RZ(zv[3]=incorp(str(3L,rwx)));
 RZ(zv[4]=incorp(attv((S)f->dwFileAttributes)));
 R z;
}

F1(jtjdir){PROLOG(0102);A z,fn,*zv;I j=0,n=32;HANDLE fh; WIN32_FIND_DATAW f; C fnbuffer[10000]; C* name;
 ARGCHK1(w);
 RZ(w=vslit(!AR(w)&&BOX&AT(w)?ope(w):w));
 RZ(fn=jttoutf16x(jt,w)); USAV(fn)[AN(fn)]=0;
 fh=FindFirstFileW(USAV(fn),&f);
 GATV0(z,BOX,n,1); zv=AAV(z);  // allocate result area
 if (fh!=INVALID_HANDLE_VALUE) {
  do {
   jttoutf8w(jt,fnbuffer,sizeof fnbuffer,f.cFileName);
   name = fnbuffer;
   if(strcmp(name,".")&&strcmp(name,"..")){  // do not include . and .. as results
    if(j==n){RZ(z=ext(0,z)); n=AN(z); zv=AAV(z);}  // if result area full, extend
    A t; RZ(t=incorp(jtdir1(jt,&f,fnbuffer))); 
    zv[j++]=t;
   }
  } while (FindNextFileW(fh,&f));
  FindClose(fh);
 }
 z=j?ope(j<n?vec(BOX,j,zv):z):reshape(v2(0L,5L),ds(CACE));
 EPILOG(z);
}

F1(jtjfatt1){A y,fn;F f;U x;
 F1RANK(0,jtjfatt1,DUMMYSELF);
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)))} else ASSERT(y=C(AAV(w)[0]),EVFNUM)
 RZ(fn=toutf16x(y)); USAV(fn)[AN(fn)]=0;  // install termination
 x=GetFileAttributesW(USAV(fn));
 if(INVALID_FILE_ATTRIBUTES!=x) R attv(x);
 jsignal(EVFNAME); R 0; 
}

F2(jtjfatt2){A y,fn;F f;U x;
 F2RANK(1,0,jtjfatt2,DUMMYSELF);
 RE(x=attu(a));
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)))} else ASSERT(y=vslit(C(AAV(w)[0])),EVFNUM)
 RZ(fn=toutf16x(y)); USAV(fn)[AN(fn)]=0;  // install termination
 if(SetFileAttributesW(USAV(fn), x)) R num(1);
 jsignal(EVFNAME); R 0;
}

#endif

#if (SYS & SYS_UNIX)

/* FIXME:   rename J link() function so we can include unistd.h */
// undefs to avoid darwin warnings - should be a better fix
#undef R_OK
#undef W_OK
#undef X_OK
#define R_OK    4               /* Test for read permission.    */
#define W_OK    2               /* Test for write permission.   */
#define X_OK    1               /* Test for execute permission. */

#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#if SYS&(SYS_SUN4+SYS_SGI)
#include "fnmatch.h"
#else
#include <fnmatch.h>
#endif


/* Return mode_t formatted into 11-character buffer supplied by the caller.  The last byte of the buffer is the string terminator \0 */
static C*modebuf(mode_t m,C* b){C c;I t=m;
 strcpy(b+1,"rwxrwxrwx");
 DO(9, if(!(m&1))b[9-i]='-'; m>>=1;); 
 if(t&S_ISUID)b[3]=(b[3]=='x')?'s':'S';
 if(t&S_ISGID)b[6]=(b[6]=='x')?'s':'S';
 if(t&S_ISVTX)b[9]=(b[9]=='x')?'t':'T';
 switch(t&S_IFMT){
  case S_IFBLK:  b[0]='b'; break;
  case S_IFCHR:  b[0]='c'; break;
  case S_IFDIR:  b[0]='d'; break;
#if !(SYS & SYS_UNIX)
  case S_IFFIFO: b[0]='f'; break;    /*IVL */
#endif
  case S_IFLNK:  b[0]='l'; break;
  case S_IFSOCK: b[0]='s'; break;
  case S_IFREG:  b[0]='-'; break;
  default:       b[0]='?';
 }
 R b;
}


static int ismatch(J jt,C*pat,C*name,struct stat *dirstatbuf,C *diratts, C *dirmode,C *dirrwx,C *dirnamebuf,C *dirbase){ 
#if !SY_64 && defined(ANDROID)
// Android issue
// Long Long (64 bit) fields are not 8 bytes aligned
 UC* raw_stat=(UC*)dirstatbuf;
 raw_stat[sizeof(struct stat)-1]=98;
 raw_stat[sizeof(struct stat)-2]=76;
 raw_stat[sizeof(struct stat)-3]=54;
#endif
 strcpy(dirbase,name); if(stat(dirnamebuf,dirstatbuf))R 0;
 if('.'!=*pat && ((!strcmp(name,"."))||(!strcmp(name,".."))))R 0;
 if(fnmatch(pat,name,0)) R 0;
/* Set up dirrwx, diratts, and dirmode for this file */
 dirrwx[0]=access(dirnamebuf,R_OK)?'-':'r';
 dirrwx[1]=access(dirnamebuf,W_OK)?'-':'w';
 dirrwx[2]=access(dirnamebuf,X_OK)?'-':'x';
 strcpy(diratts,"------");
 diratts[0]=(dirrwx[0]=='r'&&dirrwx[1]=='-')?'r':'-';
 diratts[1]=('.'==name[0])?'h':'-';
 modebuf(dirstatbuf[0].st_mode,dirmode);
 diratts[4]=('d'==dirmode[0])?'d':'-';
 R 1;
}

static A jtdir1(J jt,struct dirent*f,struct stat *dirstatbuf,C *diratts, C *dirmode,C *dirrwx){A z,*zv;C*s,att[16];I n,ts[6],i,m,sz;S x;struct tm tmr,*tm=&tmr;
 tm=localtime_r(&dirstatbuf[0].st_mtime,tm);
 ts[0]=1900+tm->tm_year; ts[1]=1+tm->tm_mon; ts[2]=tm->tm_mday;
 ts[3]=tm->tm_hour; ts[4]=tm->tm_min; ts[5]=tm->tm_sec;
 s=f->d_name; n=strlen(s);
 GAT0(z,BOX,6,1); zv=AAV(z);
 RZ(zv[0]=incorp(vec(LIT,n,s))); 
 RZ(zv[1]=incorp(vec(INT,6L,ts)));
 sz=dirstatbuf[0].st_size;
#if !SY_64 && defined(ANDROID)
 UC* raw_stat=(UC*)dirstatbuf;
 if(raw_stat[sizeof(struct stat)-1]==98 &&
    raw_stat[sizeof(struct stat)-2]==76 &&
    raw_stat[sizeof(struct stat)-3]==54){
    // Wrong stat size. Long Long (64bit) fields are not 8 bytes aligned.
     uint* raw_stat32 = (uint*)dirstatbuf;
     sz=(I)(size_t)raw_stat32[11]; // st_size from the packed (without alignment) structure
 }
#endif
 sz=sz<0?-1:sz;
 RZ(zv[2]=incorp(sc(sz)));
 RZ(zv[3]=incorp(vec(LIT,3L, dirrwx )));
 RZ(zv[4]=incorp(vec(LIT, 6L,diratts)));
 RZ(zv[5]=incorp(vec(LIT,10L,dirmode)));
 R z;
}

F1(jtjdir){PROLOG(0103);A*v,z,*zv;C*dir,*pat,*s,*x;I j=0,n=32;DIR*DP;struct dirent *f;
 C diratts[7]; C dirmode[11];  C dirrwx[3];
 struct stat dirstatbuf[3];  // for some reason there were 2 dummy blocks reserved after the buffer for 32-bit Linux.  Preserve that
 C dirnamebuf[NPATH];  // workarea
 ARGCHK1(w);
 RZ(w=str0(vslit(!AR(w)&&BOX&AT(w)?ope(w):w)));
 s=CAV(w);
 if(x=strrchr(s,'/')){dir=s==x?(C*)"/":s; pat=x+1; *x=0;}else{dir="."; pat=s;}
 if(NULL==(DP=opendir(dir)))R reshape(v2(0L,6L),ds(CACE));
 /*
  * SYSV and BSD have different return types for sprintf(),
  * so we use less efficient but portable code.
  */
 sprintf(dirnamebuf,"%s/",dir); C *dirbase=dirnamebuf+strlen(dirnamebuf); f=readdir(DP);
 GATV0(z,BOX,n,1); zv=AAV(z);
 while(f){
  if(ismatch(jt,pat,f->d_name,dirstatbuf,diratts,dirmode,dirrwx,dirnamebuf,dirbase)){
   if(j==n){RZ(z=ext(0,z)); n=AN(z); zv=AAV(z);}
   RZ(zv[j++]=incorp(jtdir1(jt,f,dirstatbuf,diratts,dirmode,dirrwx)));
  }
  f=readdir(DP);
 }
 closedir(DP);
 z=j?ope(j<n?vec(BOX,j,zv):z):reshape(v2(0L,6L),ds(CACE));
 EPILOG(z);
}



F1(jtjfatt1){ASSERT(0,EVNONCE);}
F2(jtjfatt2){ASSERT(0,EVNONCE);}


F1(jtjfperm1){A y;F f;C b[11];
 struct stat dirstatbuf[3];
 F1RANK(0,jtjfperm1,DUMMYSELF);
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)));y=str0(y);} else ASSERT(y=str0(vslit(C(AAV(w)[0]))),EVFNUM)
 if(0!=stat(CAV(y),dirstatbuf))R jerrno();
 R vec(LIT,9L,1+modebuf(dirstatbuf[0].st_mode,b));
}


static const struct tperms {C*c;I p[4];} permtab[]=
    {  {"-r"  ,{0,S_IRUSR}},
       {"-w"  ,{0,S_IWUSR}},
       {"-xSs",{0,S_IXUSR,S_ISUID,S_ISUID+S_IXUSR}},
       {"-r"  ,{0,S_IRGRP}},
       {"-w"  ,{0,S_IWGRP}},
       {"-xSs",{0,S_IXGRP,S_ISGID,S_ISGID+S_IXGRP}},
       {"-r"  ,{0,S_IROTH}},
       {"-w"  ,{0,S_IWOTH}},
       {"-xTt",{0,S_IXOTH,S_ISVTX,S_ISVTX+S_IXOTH}},
    };

F2(jtjfperm2){A y;C*s;F f;int x=0,i;C*m;
 F2RANK(1,0,jtjfperm2,DUMMYSELF);
 RE(f=stdf(w)); if(f){RZ(y=fname(sc((I)f)));y=str0(y);} else ASSERT(y=str0(vslit(C(AAV(w)[0]))),EVFNUM)
 RZ(a=vslit(a)); ASSERT(9==AN(a),EVLENGTH); s=CAV(a);
 for(i=0;i<9;i++)
    {ASSERT(NULL!=(m=strchr(permtab[i].c,s[i])),EVDOMAIN);
     x|=permtab[i].p[m-permtab[i].c];}
 R chmod(CAV(y),x)?jerrno():mtm;
}


#endif

/* ----------------------------------------------------------------------- */


#if ! (SYS & SYS_DOS)
F1(jtfullname){R w;}
#endif
