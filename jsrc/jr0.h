/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* R0 and ASSERT (used twice in vq.c)                                      */

#ifndef FAIL
#define FAIL 0
#endif


#undef R0

#if FINDNULLRET // When we return 0, we should always have an error code set.  trap if not
#define R0 {if(!jt->jerr)SEGFAULT; R FAIL;}
#else
#define R0 R FAIL;
#endif

#undef ASSERT

#ifndef _ASSERT
#define ASSERT(b,e) {if(unlikely(!(b))){jsignal(e); R0;}}

#define ASSERTF(b,e,s...)     {if(unlikely(!(b))){jsignal(e); R 0;}}
#define ASSERTSUFF(b,e,suff)   {if(unlikely(!(b))){jsignal(e); {suff}}}  // when the cleanup is more than a goto
#define ASSERTGOTO(b,e,lbl)   ASSERTSUFF(b,e,goto lbl;)
#define ASSERTTHREAD(b,e)     {if(unlikely(!(b))){jtjsignal(jm,e); R 0;}}   // used in io.c to signal in master thread
#define ASSERTD(b,s)    {if(unlikely(!(b))){jsigd((s)); R 0;}}
#define ASSERTMTV(w)    {ARGCHK1(w); ASSERT(1==AR(w),EVRANK); ASSERT(!AN(w),EVLENGTH);}
#define ASSERTN(b,e,nm) {if(unlikely(!(b))){jtjsignale(jt,(e)|EMSGLINEISNAME|EMSGNOMSGLINE,(nm),0); R 0;}}  // signal error, overriding the running name with a different one
#define ASSERTNGOTO(b,e,nm,lbl) {if(unlikely(!(b))){jtjsignale(jt,(e)|EMSGLINEISNAME|EMSGNOMSGLINE,(nm),0); goto lbl;}}  // same, but without the exit
#define ASSERTPYX(e,line)   {jtjsignale(jt,(e)|(EMSGFROMPYX|EMSGNOEFORMAT|EMSGNOMSGLINE),line,0); R 0;}
#define ASSERTSYSCOMMON(b,s,stmt)  {if(unlikely(!(b))){fprintf(stderr,"system error: %s : file %s line %d\n",s,__FILE__,__LINE__); jsignal(EVSYSTEM); jtwri(jt,JJTOJ(jt),MTYOSYS,"",(I)strlen(s),s); stmt}}
#define ASSERTSYS(b,s)  ASSERTSYSCOMMON(b,s,R 0;)
#define ASSERTSYSGOTO(b,s,lbl)  ASSERTSYSCOMMON(b,s,goto lbl;)
#define ASSERTSYSV(b,s,stmt) ASSERTSYSCOMMON(b,s,stmt)
#define ASSERTW(b,e)    {if(unlikely(!(b))){if((e)<=NEVM)jsignal(e); else jt->jerr=(e); R;}}  // put error code into jerr, but signal only if nonretryable
#define ASSERTWR(c,e)   {if(unlikely(!(c))){R e;}}  // exit primitive with error code in return
#else
// version for debugging
#define ASSERT(b,e)     {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsignal(e); R0;}}
// #define ASSERT(b,e)     {if(unlikely(!(b))){if(!(jt->emsgstate&EMSGSTATETRAPPING)){char ermsg[2000];sprintf(ermsg,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__);fprintf(stderr,"%s",ermsg);jsto(jt,MTYOER,ermsg);jsignal(e);}R0;}}

#define ASSERTF(b,e,s...)     {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsignal(e); R 0;}}
#define ASSERTSUFF(b,e,suff)   {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsignal(e); {suff}}}  // when the cleanup is more than a goto
#define ASSERTGOTO(b,e,lbl)   ASSERTSUFF(b,e,goto lbl;)
#define ASSERTTHREAD(b,e)     {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jtjsignal(jm,e); R 0;}}   // used in io.c to signal in master thread
#define ASSERTD(b,s)    {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsigd((s)); R 0;}}
#define ASSERTMTV(w)    {ARGCHK1(w); ASSERT(1==AR(w),EVRANK); ASSERT(!AN(w),EVLENGTH);}
#define ASSERTN(b,e,nm) {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jtjsignale(jt,(e)|EMSGLINEISNAME|EMSGNOMSGLINE,(nm),0); R 0;}}  // signal error, overriding the running name with a different one
#define ASSERTNGOTO(b,e,nm,lbl) {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jtjsignale(jt,(e)|EMSGLINEISNAME|EMSGNOMSGLINE,(nm),0); goto lbl;}}  // same, but without the exit
#define ASSERTPYX(e,line)   {fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jtjsignale(jt,(e)|(EMSGFROMPYX|EMSGNOEFORMAT|EMSGNOMSGLINE),line,0); R 0;}
#define ASSERTSYSCOMMON(b,s,stmt)  {if(unlikely(!(b))){fprintf(stderr,"system error: %s : file %s line %d\n",s,__FILE__,__LINE__); jsignal(EVSYSTEM); jtwri(jt,JJTOJ(jt),MTYOSYS,"",(I)strlen(s),s); stmt}}
#define ASSERTSYS(b,s)  ASSERTSYSCOMMON(b,s,R 0;)
#define ASSERTSYSGOTO(b,s,lbl)  ASSERTSYSCOMMON(b,s,goto lbl;)
#define ASSERTSYSV(b,s,stmt) ASSERTSYSCOMMON(b,s,stmt)
#define ASSERTW(b,e)    {if(unlikely(!(b))){if((e)<=NEVM){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsignal(e);} else jt->jerr=(e); R;}}  // put error code into jerr, but signal only if nonretryable
#define ASSERTWR(c,e)   {if(unlikely(!(c))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); R e;}}  // exit primitive with error code in return
#endif
