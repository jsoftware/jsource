/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
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


B jtxoinit(J jt){A x;
#if SY_WIN32 && !SY_WINCE
 _setmode(_fileno(stdin),_O_BINARY);
 _setmode(_fileno(stdout),_O_BINARY);
 _setmode(_fileno(stderr),_O_BINARY);
#endif
 GAT0(x,BOX,8,1); memset(AV(x),C0,AN(x)*SZI); ras(x); jt->fopa=x;
 GAT0(x,INT,8,1);                             ras(x); jt->fopf=x;
 R 1;
}

F jtvfn(J jt,F x){I*v=AV(jt->fopf); DO(jt->fopn,if(x==(F)*v++)R x;); ASSERT(0,EVFNUM);}
     /* check that x is in table of file#s */

I jtfnum(J jt,A w){A y;I h,j;
 if(AT(w)&B01+INT){ASSERT(h=i0(w),EVFNUM); R h;}
 ASSERT(AT(w)&BOX,EVDOMAIN);
 y=AAV0(w);
 ASSERT(AN(y),EVLENGTH);
 if(AT(y)&B01+INT){ASSERT(h=i0(y),EVFNUM); R h;}
 RE(j=i0(indexof(vec(BOX,jt->fopn,AAV(jt->fopa)),box(fullname(vslit(y)))))); 
 R j<jt->fopn?*(j+AV(jt->fopf)):0;
}    /* file# corresp. to standard argument w */

F1(jtfname){I j; 
 RE(j=i0(indexof(jt->fopf,w)));
 ASSERT(j<jt->fopn,EVFNUM);
 R ca(*(j+AAV(jt->fopa)));
}    /* string name corresp. to file# w */

F1(jtjfiles){A y;
 ASSERTMTV(w);
 RZ(y=vec(INT,jt->fopn,AV(jt->fopf)));
 R grade2(stitch(box0(y),vec(BOX,jt->fopn,AV(jt->fopa))),y);
}    /* file (number,name) table */

F jtjope(J jt,A w,C*mode){A t;F f;I n;static I nf=25; A z;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 RZ(t=str0(vslit(AAV0(w))));
 n=AN(t)-1;
 ASSERT(n,EVLENGTH);
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

F1(jtjopen){A z;I h;
 RZ(w);
 if(!AN(w))R w;
 if(AR(w))R rank1ex(w,0L,0L,jtjopen);
 RE(h=fnum(w));
 if(h){RZ(z=sc(h)); RZ(fname(z)); R z;}
 else{A ww;
  if(jt->fopn==AN(jt->fopf)){RZ(jt->fopa=ext(1,jt->fopa)); RZ(jt->fopf=ext(1,jt->fopf));}
  RZ(*(jt->fopn+IAV(jt->fopf))=h=(I)jope(w,FUPDATE_O));
  RZ(ww=fullname(vslit(AAV0(w)))); RZ(ras(ww));
  RZ(*(jt->fopn+AAV(jt->fopa))=ww);
 
  ++jt->fopn;
  R sc(h);
}}   /* open the file named w if necessary; return file# */

B jtadd2(J jt,F f1,F f2,C*cmd){A c,x;
 if(f1==NULL) {jt->fopn+=2;R 1;};
 GATV0(c,LIT,1+strlen(cmd),1);MC(CAV(c)+1,cmd,AN(c)-1);cmd=CAV(c);
 if(jt->fopn+1>=AN(jt->fopf)){RZ(jt->fopa=ext(1,jt->fopa)); RZ(jt->fopf=ext(1,jt->fopf));}
 *cmd='<';x=cstr(cmd); RZ(ras(x)); RZ(*(jt->fopn+AAV(jt->fopa)  )=x); RZ(*(jt->fopn+IAV(jt->fopf)  )=(I)f1);
 *cmd='>';x=cstr(cmd); RZ(ras(x)); RZ(*(jt->fopn+AAV(jt->fopa)+1)=x); RZ(*(jt->fopn+IAV(jt->fopf)+1)=(I)f2);
 fa(c); R 1;
}   /* add 2 entries to jt->fopn table (for hostio); null arg commits entries */


F1(jtjclose){A*av;I*iv,j;
 RZ(w);
 if(!AN(w))R w;
 if(AR(w))R rank1ex(w,0L,0L,jtjclose);
 RE(j=i0(indexof(jt->fopf,sc(fnum(w))))); ASSERT(j<jt->fopn,EVFNUM);
 av=AAV(jt->fopa); iv=IAV(jt->fopf); 
// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 RZ(unlk(iv[j]));
#endif
 if(fclose((F)iv[j]))R jerrno();
 --jt->fopn; fa(av[j]); if(j<jt->fopn){av[j]=av[jt->fopn]; iv[j]=iv[jt->fopn];}
 R num[1];
}    /* close file# w */

F jtstdf(J jt,A w){A y;F f;I n,r,t;
 RZ(w);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(!AR(w),EVRANK);
 if(BOX&AT(w)){
  y=AAV0(w); t=AT(y); n=AN(y); r=AR(y);
  if(t&(LIT+C2T+C4T)){ASSERT(1>=r,EVRANK); ASSERT(n,EVLENGTH); R 0;}
/*!
  if(t&C2T){ASSERT(1>=r,EVRANK); ASSERT(n,EVLENGTH); ASSERT(vc1(n,USAV(y)),EVDOMAIN); R 0;}
     vc1 can now be killed off
*/
  if(t&B01+INT)R stdf(y);
  ASSERT(0,EVDOMAIN);
 }
 f=(F)i0(w); 
 ASSERT(f,EVFNUM); 
 R f;
}    /* 0 if w is a boxed file name; n if w is integer or boxed integer */
