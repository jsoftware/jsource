/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: File Open/Close                                                  */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#ifndef UNDER_CE
#include <io.h>
#include <fcntl.h>
#endif 
#endif
       
#include "j.h"
#include "x.h"

B jtxoinit(JS jjt, I nthreads){A x;JJ jt=MTHREAD(jjt);
#if SY_WIN32 && !SY_WINCE
 _setmode(_fileno(stdin),_O_BINARY);
 _setmode(_fileno(stdout),_O_BINARY);
 _setmode(_fileno(stderr),_O_BINARY);
#endif
 GAT0(x,INT,8,0);  ACINITZAP(x); INITJT(jjt,fopafl)=x; AM(INITJT(jjt,fopafl))=0;   // AM is # valid files.  Table has rank 0.  We allo as INT but the values are As - we never need to clear to 0
 R 1;
}

// check that x is in table of file#s.  Return x if so, otherwise 0 for error
// if the file is found, mark it as busy in fopafl.  This establishes a lock on that file to prevent it from being deleted while I/O is in process
// Every non-failing call to vfn must be matched by a call to unvfn to unlock the file
F jtvfn(J jt,F x){F z=0; READLOCK(JT(jt,flock)) A*v=AAV0(JT(jt,fopafl)); DQ(AM(JT(jt,fopafl)),if((F)AM(v[i])==x){z=x; READLOCK(v[i]->lock) break;}) READUNLOCK(JT(jt,flock)) ASSERT(z!=0,EVFNUM); R z;}

// remove internal file lock set by vfn().  Required exactly when vfn has returned non0.  The file is known to be in the file table, but its index may have changed.
// for syntactic ease we return the dummy argument
A jtunvfn(J jt,F x,A dummy){READLOCK(JT(jt,flock)) A*v=AAV0(JT(jt,fopafl)); DQ(AM(JT(jt,fopafl)),if((F)AM(v[i])==x){READUNLOCK(v[i]->lock) break;}) READUNLOCK(JT(jt,flock)) R dummy;}

// w is a user argument, either a number or a filename string.  If a number, return it (with error if it is 0); if a string return the file# if found, or 0 if not found
// return of 0 is not ipso facto an error
I jtfnum(J jt,A w){A y;I h,j,z=0;
 if(AT(w)&B01+INT){ASSERT(h=i0(w),EVFNUM); R h;}  // return numeric arg forthwith
 ASSERT(AT(w)&BOX,EVDOMAIN);
 y=C(AAV(w)[0]);
 ASSERT(AN(y),EVLENGTH);
 if(AT(y)&B01+INT){ASSERT(h=i0(y),EVFNUM); R h;}
 RZ(y=fullname(vslit(y)));  // get name to match
 READLOCK(JT(jt,flock)) A*v=AAV0(JT(jt,fopafl)); 
 DO(AM(JT(jt,fopafl)), if(equ(v[i],y)){z=AM(v[i]); break;} )  // if filename found, return its file#
 READUNLOCK(JT(jt,flock))
 R z;
}    /* file# corresp. to standard argument w */

// returns 0 if given file# is not open, otherwise the string name of the file
F1(jtfname){I j; A z=0;
 I h; ASSERT(h=i0(w),EVFNUM);
 READLOCK(JT(jt,flock))  A*v=AAV0(JT(jt,fopafl));
 DO(AM(JT(jt,fopafl)), if(h==AM(v[i])){z=ca(v[i]); break;} )  // if filename found, return its file#.  Clone the string because we need the AM field in the table; also we don't do EPILOG
 READUNLOCK(JT(jt,flock))
 R z;
}    /* string name corresp. to file# w */

// 1!:20
F1(jtjfiles){A y,z=0;
 ASSERTMTV(w);
 READLOCK(JT(jt,flock))  A*v=AAV0(JT(jt,fopafl)); I nrows=AM(JT(jt,fopafl));
 // We are doing an uncomfortable amount of memory allocating here under lock.
 // We have to clone the string to protect the AM field in the original, and the lock
 GAE0(y,BOX,2*nrows,2,goto errorexit;) A (*zv)[2]=(A (*)[2])AAV2(y); AS(y)[0]=nrows; AS(y)[1]=2;  // get addr of result data, fill in the shape
 DO(AS(y)[0], RZGOTO(zv[i][1]=ca(v[i]),errorexit); GAT0E(zv[i][0],INT,1,0,goto errorexit;) IAV0(zv[i][0])[0]=AM(v[i]);)  // fill in the boxed file# and a clone of the string
 z=y;  // success
errorexit: READUNLOCK(JT(jt,flock))
 z=grade2(z,z);  //
 R z;
}    /* file (number,name) table in order */

// open named file in the OS.  Result is file handle, or 0 if error
F jtjope(J jt,A w,C*mode){A t;F f;I n;static I nf=25; A z;
 ARGCHK1(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 RZ(t=str0(vslit(C(AAV(w)[0]))));
 n=AN(t)-1;
 ASSERT(n!=0,EVLENGTH);
#if (SYS&SYS_UNIX)
{
 C* cs=CAV(t);
 f=fopen(cs,mode);
 if(!f&&errno==ENOENT&&!strcmp(mode,FUPDATE_O))f=fopen(cs,FUPDATEC_O);
 if(!f&&errno==EACCES&& strcmp(mode,FREAD_O  ))f=fopen(cs,FREAD_O);
}
#else
{
 US usmode[10]; US*s; I i;
 RZ(z=jttoutf16x(jt,t));
 s=USAV(z);
 for(i=0;i<(I)strlen(mode);++i){usmode[i]=(US)mode[i];}
 usmode[i]=0;
#if !SY_WINCE 
 f=_wfopen(s,usmode);
 if(!f&&errno==ENOENT&&!wcscmp(usmode,FLUPDATE_O))f=_wfopen(s,FLUPDATEC_O);
 if(!f&&errno==EACCES&& wcscmp(usmode,FLREAD_O  ))f=_wfopen(s,FLREAD_O);
#else
 {
 f=_wfopen(s,usmode);
 if(!f&&!wcscmp(usmode,FLUPDATE_O))f=_wfopen(s,FLUPDATEC_O); // no errno on wince
 if(!f&& wcscmp(usmode,FLREAD_O  ))f=_wfopen(s,FLREAD_O);
 }
#endif
}
#endif
 R f?f:(F)jerrno();
}

// 1!:21
F1(jtjopen){A z;I h;
 ARGCHK1(w);
 if(!AN(w))R w;
 if(AR(w))R rank1ex0(w,DUMMYSELF,jtjopen);
 RE(h=fnum(w));  // return non0 if the string is the # of an already-open file
 if(h){RZ(z=sc(h)); RZ(fname(z)); R z;}  // if already open, return # provided the file is open (it wouldn't be if the arg was an invalid file#)
  // opening a file by number seems weird - the only # you can use is for an open file, so what's the point?
 else{A ww;
  // opening a file by name.  We open the file and then add it to the table.  If the same name is opened more than once, they get
  // separate table entries
  RZ(h=(I)jope(w,FUPDATE_O));
  RZ(ww=mkwris(fullname(vslit(C(AAV(w)[0])))));
  WRITELOCK(JT(jt,flock))
  while(AM(JT(jt,fopafl))==AN(JT(jt,fopafl)))RZ(jtextendunderlock(jt,&JT(jt,fopafl),&JT(jt,flock),0))
  AAV0(JT(jt,fopafl))[AM(JT(jt,fopafl))]=ww; ACINITZAP(ww) AM(ww)=h;  // install new string, with file handle in AM
  ++AM(JT(jt,fopafl));
  WRITEUNLOCK(JT(jt,flock))
  R sc(h);
}}   /* open the file named w if necessary; return file# */

#if 0  // doesn't work
B jtadd2(J jt,F f1,F f2,C*cmd){A c,x;I ct=AM(JT(jt,fopf));
 if(f1==NULL) {AM(JT(jt,fopf))=ct+2;R 1;};
 GATV0(c,LIT,1+strlen(cmd),1);MC(CAV(c)+1,cmd,AN(c)-1);cmd=CAV(c);
 if(ct+2>AN(JT(jt,fopf))){RZ(JT(jt,fopa)=ext(1,JT(jt,fopa))); RZ(JT(jt,fopf)=ext(1,JT(jt,fopf))); AM(JT(jt,fopf))=ct;}
 *cmd='<';x=cstr(cmd); ACINITZAP(x) RZ(AAV(JT(jt,fopa))[ct]=x); RZ(IAV(JT(jt,fopf))[ct]=(I)f1);
 *cmd='>';x=cstr(cmd); ACINITZAP(x) RZ(AAV(JT(jt,fopa))[ct+1]=x); RZ(IAV(JT(jt,fopf))[ct+1]=(I)f2);
 R 1;
}   /* add 2 entries to AM(JT(jt,fopf)) table (for hostio); null arg commits entries */
#endif

// 1!:22
F1(jtjclose){A*av;I*iv,j,h;
 ARGCHK1(w);
 if(!AN(w))R w;
 if(AR(w))R rank1ex0(w,DUMMYSELF,jtjclose);
 RZ(h=fnum(w));  // get the file # of the file referred to.  If nonexistent, fail
 // There is nothing to prevent two threads from closing the same file.  The results would be unpredictable: another thread could
 // reallocate the same file# as one of the closing threads, with the result that the wrong file gets closed.  This is a user sync error.
 // Release any locks held on the file being freed
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 RZ(unlk(h));
#endif
 // first, remove the table entry so no other threads can write to the file
 A dela=0;  // the string we deleted
 WRITELOCK(JT(jt,flock)) A*v=AAV0(JT(jt,fopafl));
 DO(AM(JT(jt,fopafl)), if(h==AM(v[i])){dela=v[i]; v[i]=v[--AM(JT(jt,fopafl))]; break;} )  // if file# is open, remove it from the open list
 WRITEUNLOCK(JT(jt,flock))
 // we have detached the string for the closing file.  It is possible that there wasn't one, if another thread got in and deleted it first.
 ASSERT(dela,EVFNUM)
 // Wait for any outstanding I/O to complete
 WRITELOCK(dela->lock) WRITEUNLOCK(dela->lock) fa(dela);  // active I/O sets a readlock on dela.  Wait for all to finish.  It can't start again because the string is gone.  Free dela finally
 if(fclose((F)h))R jerrno();   // try to close the file, fail if error
 R num(1);  // always return success
}    /* close file# w */

F jtstdf(J jt,A w){A y;F f;I n,r,t;
 ARGCHK1(w);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(!AR(w),EVRANK);
 if(BOX&AT(w)){
  y=C(AAV(w)[0]); t=AT(y); n=AN(y); r=AR(y);
  if(t&(LIT+C2T+C4T)){ASSERT(1>=r,EVRANK); ASSERT(n!=0,EVLENGTH); R 0;}
/*!
  if(t&C2T){ASSERT(1>=r,EVRANK); ASSERT(n!=0,EVLENGTH); ASSERT(vc1(n,USAV(y)),EVDOMAIN); R 0;}
     vc1 can now be killed off
*/
  if(t&B01+INT)R stdf(y);
  ASSERT(0,EVDOMAIN);
 }
 f=(F)i0(w); 
 ASSERT(f!=0,EVFNUM); 
 R f;
}    /* 0 if w is a boxed file name; n if w is integer or boxed integer */
