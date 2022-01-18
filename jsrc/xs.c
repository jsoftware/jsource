/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Scripts                                                          */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include "j.h"
#include "x.h"


B jtxsinit(JS jjt,I nthreads){A x;JJ jt=MTHREAD(jjt);
 GAT0(x,BOX,10,1); ACINITZAP(x); INITJT(jjt,slist)=x; AS(INITJT(jjt,slist))[0]=0;  // init block, set item count to 0.  This block is NOT recursive but becomes one if extended
 MTHREAD(jjt)->currslistx=-1;  // indicate 'not in script' in master thread
 R 1;
}

F1(jtsnl){ASSERTMTV(w); R vec(BOX,AS(JT(jt,slist))[0],AAV(JT(jt,slist)));}
     /* 4!:3  list of script names */


#if (SYS & SYS_MACINTOSH)
void setftype(C*v,OSType type,OSType crea){C p[256];FInfo f;
 __c2p(v,p);
 GetFInfo(p,0,&f);
 f.fdType=type; f.fdCreator=crea;
 SetFInfo(p,0,&f);
}
#endif

/* line/linf arguments:                         */
/* a:   left argument for unlock                */
/* w:   input file or lines; 1 means keyboard   */
/* si:  script index                            */
/* ce:  0 stop on error                         */ 
/*      1 continue on error                     */
/*      2 stop on error or nonunit result       */
/*      3 ditto, but return 0 or 1 result and   */
/*        do not display error                  */
/* tso: echo to stdout                          */

#define SEEKLEAK 0
static A jtline(J jt,A w,I si,C ce,B tso){A x=mtv,z;DC d;
#if NAMETRACK
 // bring out the name, locale, and script into easy-to-display name
 C trackinfo[256]; int tracklineno=0;  // will hold line# followed by line
 forcetomemory(&trackinfo);
#define SETTRACK if(x){mvc(sizeof(trackinfo),trackinfo,1,iotavec-IOTAVECBEGIN+' '); \
  I trackwlen=sprintf(trackinfo,"%d: ",tracklineno++); \
  MC(trackinfo,CAV(x),MIN((I)sizeof(trackinfo)-trackwlen,AN(x)));}
#else
#define SETTRACK
#endif
 if(equ(w,num(1)))R mtm;
 RZ(w=vs(w));
 // Handle locking.  Global glock has lock status for higher levels.  We see if this text is locked; if so, we mark lock status for this level
 // We do not inherit the lock from higher levels, per the original design
 C oldk=jt->glock; // incoming lock status
 if((jt->glock=(AN(w)&&CFF==CAV(w)[0]))){
  RZ(w=unlock2(mtm,w));
  ASSERT(CFF!=CAV(w)[0],EVDOMAIN);
  si=-1; tso=0;  // if locked, keep shtum about internals
 }
 // similarly for namecaching.  We push the per-script status and let the on/off status run independently
 C oldnmcachescript=jt->namecaching&1;  // save the per-script part of the status
 FDEPINC(1);   // No ASSERTs or returns till the FDEPDEC below
 RZ(d=deba(DCSCRIPT,0L,w,(A)si));
 d->dcpflags=(!(tso&&!JT(jt,seclev)))<<JTPRNOSTDOUTX;  // set flag to indicate suppression of output
 J jtinplace=(J)((I)jt|d->dcpflags);  // create typeout flags to pass along: no output class, suppression as called for in tso
 d->dcss=1;  // indicate this script is not overridden by suspension
 A *old=jt->tnextpushp;
 switch(ce){
 // loop over the lines.  jgets may fail, in which case we leave that as the error code for the sentence.
 case 0: NOUNROLL while(x&&!jt->jerr){jt->etxn=0; x=jgets("   "); if(x==0)break; SETTRACK jtimmex(jtinplace,x=ddtokens(x,1+(AN(jt->locsyms)>1))); tpop(old);} break;  // lgets returns 0 for error or EOF
 case 1: NOUNROLL while(x           ){if(!JT(jt,seclev))jtshowerr(jtinplace); jt->jerr=0; x=jgets("   ");  SETTRACK  jtimmex(jtinplace,x=ddtokens(x,1+(AN(jt->locsyms)>1))); tpop(old);} break;
 case 2:
 case 3: {
#if SEEKLEAK
  I stbytes = spbytesinuse();
#endif
  NOUNROLL while(x&&!jt->jerr){jt->etxn=0; x=jgets("   "); if(x==0)break; SETTRACK jtimmea(jtinplace,x); tpop(old);}
#if SEEKLEAK
  I endbytes=spbytesinuse(); if(endbytes-stbytes > 1000)printf("%lld bytes lost\n",endbytes-stbytes);
#endif
  }
 }
  debz();
 FDEPDEC(1);  // ASSERT OK now
 jt->namecaching&=~1; jt->namecaching|=oldnmcachescript;  // pop the per-script part
 jt->glock=oldk; // pop lock status
 if(3==ce){z=num(jt->jerr==0); RESETERR; R z;}else RNE(mtm);
}

static F1(jtaddscriptname){I i;
 RE(i=i0(indexof(vec(BOX,AS(JT(jt,slist))[0],AAV(JT(jt,slist))),box(ravel(w)))));  // look up only in the defined names
 if(AS(JT(jt,slist))[0]==i){
  if(AS(JT(jt,slist))[0]==AN(JT(jt,slist))){RZ(JT(jt,slist)=ext(1,JT(jt,slist)));}  // extend, preserving curr index (destroying len momentarily)
  INCORP(w); RZ(ras(w)); RZ(AAV(JT(jt,slist))[i]=w);
  AS(JT(jt,slist))[0]=i+1;  // set new len
 }
 R sc(i);
}



static A jtlinf(J jt,A a,A w,C ce,B tso){A x,y,z;B lk=0;C*s;I i=-1,n,oldi=jt->currslistx;  // push script#
 ARGCHK2(a,w);
 ASSERT(AT(w)&BOX,EVDOMAIN);
 if(JT(jt,seclev)){
  y=AAV(w)[0]; n=AN(y); s=CAV(y); 
  ASSERT(LIT&AT(y),EVDOMAIN); 
  ASSERT(3<n&&!memcmpne(s+n-3,".js",3L)||4<n&&!memcmpne(s+n-4,".ijs",4L),EVSECURE);
 }
 RZ(x=jfread(w));
 // Remove UTF8 BOM if present - commented out pending resolution.  Other BOMs should not occur
 // if(!memcmp(CAV(x),"\357\273\277",3L))RZ(x=drop(num(3),x))
 // if this is a new file, record it in the list of scripts
 RZ(y=fullname(AAV(w)[0]));
 A scripti; RZ(scripti=jtaddscriptname(jt,y)); i=IAV(scripti)[0];

 // set the current script number
 jt->currslistx=i;
 z=line(x,i,ce,tso); 
 jt->currslistx=oldi;  // pop script#
#if SYS & SYS_PCWIN
 if(lk)mvc(AN(x),AV(x),1,MEMSET00);  /* security paranoia */
#endif
 R z;
}

// 4!:6 add script name to list and return its index
F1(jtscriptstring){
 ASSERT(AT(w)&LIT,EVDOMAIN);  // literal
 ASSERT(AR(w)<2,EVRANK);  // list
 R jtaddscriptname(jt,w);   // add name if new; return index to name
}

// 4!:7 set script name to use and return previous value
F1(jtscriptnum){
 I i=i0(w);  // fetch index
 ASSERT(BETWEENO(i,-1,AS(JT(jt,slist))[0]),EVINDEX);  // make sure it's _1 or valid index
 A rv=sc(jt->currslistx);  // save the old value
 RZ(rv); jt->currslistx=i;  // set the new value (if no error)
 R rv;  // return prev value
}

F1(jtscm00 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm00, DUMMYSELF); R r?line(w,-1L,0,0):linf(mark,w,0,0);}
F1(jtscm01 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm01, DUMMYSELF); R r?line(w,-1L,0,1):linf(mark,w,0,1);}
F1(jtscm10 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm10, DUMMYSELF); R r?line(w,-1L,1,0):linf(mark,w,1,0);}
F1(jtscm11 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm11, DUMMYSELF); R r?line(w,-1L,1,1):linf(mark,w,1,1);}
F1(jtsct1  ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtsct1,  DUMMYSELF); R r?line(w,-1L,2,1):linf(mark,w,2,1);}
F1(jtscz1  ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscz1,  DUMMYSELF); R r?line(w,-1L,3,0):linf(mark,w,3,0);}

F2(jtscm002){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm002,DUMMYSELF); R r?line(w,-1L,0,0):linf(a,   w,0,0);}
F2(jtscm012){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm012,DUMMYSELF); R r?line(w,-1L,0,1):linf(a,   w,0,1);}
F2(jtscm102){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm102,DUMMYSELF); R r?line(w,-1L,1,0):linf(a,   w,1,0);}
F2(jtscm112){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm112,DUMMYSELF); R r?line(w,-1L,1,1):linf(a,   w,1,1);}
F2(jtsct2  ){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtsct2,  DUMMYSELF); R r?line(w,-1L,2,1):linf(a,   w,2,1);}
F2(jtscz2  ){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscz2,  DUMMYSELF); R r?line(w,-1L,3,0):linf(a,   w,3,0);}
