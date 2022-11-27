/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Scripts                                                          */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include "j.h"
#include "x.h"


B jtxsinit(JS jjt){A x;JJ jt=MTHREAD(jjt);
 GAT0(x,BOX,10,1); ACINITZAP(x); INITJT(jjt,slist)=x; AM(INITJT(jjt,slist))=0;  // init block, set item count to 0.  This block is NOT recursive but becomes one if extended
 MTHREAD(jjt)->currslistx=-1;  // indicate 'not in script' in master thread
 R 1;
}

F1(jtsnl){ASSERTMTV(w); R vec(BOX,AM(JT(jt,slist)),AAV(JT(jt,slist)));}
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

// handler for load command, 0!:0-112
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
 case 0: NOUNROLL while(x&&!jt->jerr){RESETERR x=jgets("   "); if(x==0)break; SETTRACK jtimmex(jtinplace,x=ddtokens(x,1+!!EXPLICITRUNNING)); tpop(old);} break;  // lgets returns 0 for error or EOF
 case 1: NOUNROLL while(x           ){if(!JT(jt,seclev))jtshowerr(jtinplace); RESETERR x=jgets("   ");  SETTRACK  jtimmex(jtinplace,x=ddtokens(x,1+!!EXPLICITRUNNING)); tpop(old);} break;
 case 2:
 case 3: {
#if SEEKLEAK
  I stbytes = spbytesinuse();
#endif
  NOUNROLL while(x&&!jt->jerr){RESETERR x=jgets("   "); if(x==0)break; SETTRACK jtimmea(jtinplace,x); tpop(old);}
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

static F1(jtaddscriptname){I i;A z;
 INCORP(w);  // make sure later ras() can't fail
 A boxw=box(ravel(w));   // take <w before locking
 WRITELOCK(JT(jt,startlock))
 // We call indexof, which is OK because on list vs atom it will just do a sequential search.  But we do have to set up AN/AS correctly
 I savn=AN(JT(jt,slist));
 AS(JT(jt,slist))[0]=AN(JT(jt,slist))=AM(JT(jt,slist));
 z=indexof(JT(jt,slist),boxw);  // look up only in the defined names
 AN(JT(jt,slist))=savn;  // restore count; shape is immaterial
 if(z==0)goto exit;  // if error in indexof, abort
 i=i0(z);  // get the index at which found
 if(AM(JT(jt,slist))==i){  // if string must be added...
  NOUNROLL while(AM(JT(jt,slist))==AN(JT(jt,slist)))RZ(jtextendunderlock(jt,&JT(jt,slist),&JT(jt,startlock),0))  // extend if list full
  ras(w); AAV(JT(jt,slist))[i]=w;
  AM(JT(jt,slist))=i+1;  // set new len
 }
exit:
 WRITEUNLOCK(JT(jt,startlock))
 R z;  // either where found or where added
}



static A jtlinf(J jt,A a,A w,C ce,B tso){A x,y,z;B lk=0;C*s;I i=-1,n,oldi=jt->currslistx;  // push script#
 ARGCHK2(a,w);
 ASSERT(AT(w)&BOX,EVDOMAIN);
 if(JT(jt,seclev)){
  y=C(AAV(w)[0]); n=AN(y); s=CAV(y); 
  ASSERT(LIT&AT(y),EVDOMAIN); 
  ASSERT(3<n&&!memcmpne(s+n-3,".js",3L)
	 ||4<n&&!memcmpne(s+n-4,".ijs",4L)
	 ||2<n&&!memcmpne(s+n-2,".j",2L),
	 EVSECURE);
 }
 RZ(x=jfread(w));
 // Remove UTF8 BOM if present - commented out pending resolution.  Other BOMs should not occur
 // if(!memcmp(CAV(x),"\357\273\277",3L))RZ(x=drop(num(3),x))
 // if this is a new file, record it in the list of scripts
 RZ(y=fullname(C(AAV(w)[0])));
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
 READLOCK(JT(jt,startlock)) I scriptn=AM(JT(jt,slist)); READUNLOCK(JT(jt,startlock))   // no problem if we lose lock since list only grows
 ASSERT(BETWEENO(i,-1,scriptn),EVINDEX);  // make sure it's _1 or valid index
 A rv=sc(jt->currslistx);  // save the old value
 RZ(rv); jt->currslistx=i;  // set the new value (if no error)
 R rv;  // return prev value
}

// entry points for 0!:0-0!:112
DF1(jtscm00 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm00, self); R r?line(w,-1L,0,0):linf(mark,w,0,0);}
DF1(jtscm01 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm01, self); R r?line(w,-1L,0,1):linf(mark,w,0,1);}
DF1(jtscm10 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm10, self); R r?line(w,-1L,1,0):linf(mark,w,1,0);}
DF1(jtscm11 ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscm11, self); R r?line(w,-1L,1,1):linf(mark,w,1,1);}
DF1(jtsct1  ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtsct1,  self); R r?line(w,-1L,2,1):linf(mark,w,2,1);}
DF1(jtscz1  ){I r; ARGCHK1(w);    r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F1RANK(     r,jtscz1,  self); R r?line(w,-1L,3,0):linf(mark,w,3,0);}

DF2(jtscm002){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm002,self); R r?line(w,-1L,0,0):linf(a,   w,0,0);}
DF2(jtscm012){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm012,self); R r?line(w,-1L,0,1):linf(a,   w,0,1);}
DF2(jtscm102){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm102,self); R r?line(w,-1L,1,0):linf(a,   w,1,0);}
DF2(jtscm112){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscm112,self); R r?line(w,-1L,1,1):linf(a,   w,1,1);}
DF2(jtsct2  ){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtsct2,  self); R r?line(w,-1L,2,1):linf(a,   w,2,1);}
DF2(jtscz2  ){I r; ARGCHK2(a,w); r=ISDENSETYPE(AT(w),LIT+C2T+C4T); F2RANK(RMAX,r,jtscz2,  self); R r?line(w,-1L,3,0):linf(a,   w,3,0);}
