/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos aka Foreign: External, Experimental, & Extra                      */

#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "j.h"
#include "x.h"


// undocumented 13!: functions, used to test condrange
static DF2(jtfindrange){
 ARGCHK2(a,w);
 I *av = AV(a);
 CR rng = condrange(AV(w),AN(w),av[0],av[1],av[2]);  // starting min, starting max, max range
 R v2(rng.min,rng.range);
} // 13!:_1
static DF2(jtfindrange4){
 ARGCHK2(a,w);
 I *av = AV(a);
 CR rng = condrange4(C4AV(w),AN(w),av[0],av[1],av[2]);
 R v2(rng.min,rng.range);
}  // 13!:_2
static DF2(jtfindrange2){
 ARGCHK2(a,w);
 I *av = AV(a);
 CR rng = condrange2(USAV(w),AN(w),av[0],av[1],av[2]);
 R v2(rng.min,rng.range);
}  // 13!:_3

// 13!:_4  Return header info: t, flag, m, type, c, n, r
static DF1(jthdrinfo){A z;
 ARGCHK1(w);
 GAT0(z,INT,7,1);
 IAV(z)[0]=AK(w); IAV(z)[1]=AFLAG(w); IAV(z)[2]=AM(w); IAV(z)[3]=AT(w); IAV(z)[4]=AC(w); IAV(z)[5]=AN(w); IAV(z)[6]=AR(w);
 R z;
}

// 0!:_1 set skip character for scripts (13!:7 c makes scripts discard lines till one starts with NB.c
// Behavior undefined if a script is not being read
static DF1(jtskipinscript){
 ARGCHK1(w);
 ASSERT(AR(w)==0, EVRANK) ASSERT(AT(w)&LIT,EVDOMAIN)
 jt->scriptskipbyte=CAV(w)[0];  // save the skip byte
 R mtm;
}
// TUNE static I totprobes=0, totslots=0;  // umber of probes/slots

// the table of A blocks for each foreign.  m and n are in localuse.
static PRIM foreignA[320] = { {{AKXR(0),VERB&TRAVERSIBLE,0,VERB,ACPERMANENT,0,0},{{.valencefns={jtvalenceerr,jtvalenceerr},.fgh={0,0,0},.localuse.lu1.foreignmn={~0,~0},.flag=VASGSAFE,.flag2=0,.lrr=(RANK2T)((RMAX<<RANKTX)+RMAX),.mr=(RANKT)RMAX,.id=CIBEAM,}}} };

// probe at location given by (m,n), return index of empty slot
static I emptyslot(US m, US n){
 I probe=((CRC32((m<<16)+n,~0)&0xffff)*(sizeof(foreignA)/sizeof(foreignA[0])))>>16;  // create initial probe
// TUNE I nprobes=1;
 while((FAV((A)&foreignA[probe])->localuse.lu1.foreignmn[0]&FAV(((A)&foreignA[probe]))->localuse.lu1.foreignmn[1])!=(US)~0){if(unlikely(--probe<0))probe=(sizeof(foreignA)/sizeof(foreignA[0]))-1;}  // search for empty
// TUNE  if(nprobes>1)printf("mn=%d %d, nprobes=%lld\n",m,n,nprobes);
// TUNE totprobes+=nprobes; ++totslots; // TUNE
 R probe;  // return index of empty
}

// return addr of prim block for m!:n, or 0 if not found
static A findslot(US m,US n){
 I probe=((CRC32((m<<16)+n,~0)&0xffff)*(sizeof(foreignA)/sizeof(foreignA[0])))>>16;  // create initial probe
 while((FAV((A)&foreignA[probe])->localuse.lu1.foreignmn[0]^m)|(FAV((A)&foreignA[probe])->localuse.lu1.foreignmn[1]^n)){
  if(((FAV((A)&foreignA[probe]))->localuse.lu1.foreignmn[0]&FAV(((A)&foreignA[probe]))->localuse.lu1.foreignmn[1])==(US)~0)R 0;  // not found
  if(unlikely(--probe<0))probe=(sizeof(foreignA)/sizeof(foreignA[0]))-1; // advance probe
 }  // search for match
 R (A)&foreignA[probe];  // return address of match
}

// called at initialization before memory reset
void jtforeigninit(J jt){UI i;
 // Copy the template from the first initialized row to all the rest
 for(i=1;i<sizeof(foreignA)/sizeof(foreignA[0]);++i)foreignA[i]=foreignA[0];  // init most of the fields in each verb
 I Andx;  // index of next slot in the mn table, and next A block.  These are built sequentially

#define MN(m,n) Andx=emptyslot((m),(n)); FAV((A)&foreignA[Andx])->localuse.lu1.foreignmn[0]=(m); FAV((A)&foreignA[Andx])->localuse.lu1.foreignmn[1]=(n);  // point Andx to the next A to be built

#define XPRIM(ps,fn1,fn2,flg1,flg2,m,lr,rr) \
 if((ps)!=VERB)AT((A)&foreignA[Andx])=(ps); \
 if((fn1)!=0)FAV((A)&foreignA[Andx])->valencefns[0]=(fn1); if((fn2)!=0)FAV((A)&foreignA[Andx])->valencefns[1]=(fn2); \
 if((flg1)!=VASGSAFE)FAV((A)&foreignA[Andx])->flag=(flg1); if((flg2)!=0)FAV((A)&foreignA[Andx])->flag2=(flg2); \
 if((m)!=RMAX)FAV((A)&foreignA[Andx])->mr=(m); if((((lr)<<RANKTX)+(rr))!=((RMAX<<RANKTX)+RMAX))FAV((A)&foreignA[Andx])->lrr=((lr)<<RANKTX)+(rr);

 // Create all the blocks.  They must not produce CONJ
 // order the creation of blocks by importance, because the earliest blocks added to the hash will resolve with fewest misses
 // Most of the first section resolve in a single probe.  Avg # probes 2.3
 MN(0,100) XPRIM(VERB, jtscm00,      jtscm002,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,0)   XPRIM(VERB, jtjdir,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,1)   XPRIM(VERB, jtjfread,     0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(1,2)   XPRIM(VERB, 0,            jtjfwrite,    VASGSAFE,VF2NONE,RMAX,RMAX,0   );
 MN(1,3)   XPRIM(VERB, 0,            jtjfappend,   VASGSAFE,VF2NONE,RMAX,RMAX,0   );
 MN(1,4)   XPRIM(VERB, jtjfsize,     0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(1,5)   XPRIM(VERB, jtjmkdir,     0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);

 MN(1,11)  XPRIM(VERB, jtjiread,     0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(1,12)  XPRIM(VERB, 0,            jtjiwrite,    VASGSAFE,VF2NONE,RMAX,RMAX,1   );

 MN(3,0)   XPRIM(VERB, jtstype,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,10)  XPRIM(VERB, jttobase64,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,11)  XPRIM(VERB, jtfrombase64, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,12)  XPRIM(VERB, 0,            jtlowerupper, VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,0)   XPRIM(VERB, jtnc,         0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(6,0)   XPRIM(VERB, jtts0,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,1)   XPRIM(VERB, jttss,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,14)  XPRIM(VERB, jtinttoe,    0,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,15)  XPRIM(VERB, jtetoint,    0,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,16)  XPRIM(VERB, jtetoiso8601,jtetoiso8601,  VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,17)  XPRIM(VERB, jtiso8601toe,jtiso8601toe,  VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,18)  XPRIM(VERB, jtstringtoe,jtstringtoe,    VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(8,0)   XPRIM(VERB, jtfmt01,      jtfmt02,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(8,1)   XPRIM(VERB, jtfmt11,      jtfmt12,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(8,2)   XPRIM(VERB, jtfmt21,      jtfmt22,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,30)  XPRIM(VERB, jtcurnameq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(15,0)  XPRIM(VERB, 0,            jtcd,         VASGSAFE|VJTFLGOK2,VF2NONE,RMAX,   1L,  1L  );
 MN(15,1)  XPRIM(VERB, jtmemr,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,2)  XPRIM(VERB, 0,            jtmemw,       VASGSAFE,VF2NONE,RMAX,   RMAX,RMAX);
 MN(15,3)  XPRIM(VERB, jtmema,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,4)  XPRIM(VERB, jtmemf,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,15) XPRIM(VERB, jtmemu,       jtmemu2,      VASGSAFE|VJTFLGOK1,VF2NONE,RMAX,0,   0   );
 MN(18,2)  XPRIM(VERB, jtlocpath1,   jtlocpath2,   VFLAGNONE,VF2NONE,0,   1,   0   );
 MN(18,3)  XPRIM(VERB, jtloccre1,    jtloccre2,    VFLAGNONE,VF2NONE,RMAX,0,   RMAX);
 MN(18,4)  XPRIM(VERB, jtlocswitch,  0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(18,5)  XPRIM(VERB, jtlocname,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,2) XPRIM(VERB, 0,            jtapplystr,   VFLAGNONE,VF2NONE,RMAX,1,   RMAX);
 MN(128,5) XPRIM(VERB, jtisnan,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,9) XPRIM(VERB, jtmvmsparse,         0,     VASGSAFE,VF2WILLOPEN1,RMAX,   RMAX,RMAX);
 MN(128,11) XPRIM(VERB, 0,           jtlrtrim,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,12) XPRIM(VERB, 0,           jtekupdate,     VASGSAFE|VJTFLGOK2,VF2WILLOPEN2A,RMAX,RMAX,RMAX);
 MN(128,13) XPRIM(VERB, jtfindspr, 0,                VASGSAFE|VJTFLGOK2,VF2WILLOPEN2A,RMAX,RMAX,RMAX);

// little-used fns follow

 MN(0,  0) XPRIM(VERB, jtscm00,      jtscm002,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,  1) XPRIM(VERB, jtscm01,      jtscm012,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,101) XPRIM(VERB, jtscm01,      jtscm012,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0, 10) XPRIM(VERB, jtscm10,      jtscm102,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,110) XPRIM(VERB, jtscm10,      jtscm102,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0, 11) XPRIM(VERB, jtscm11,      jtscm112,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,111) XPRIM(VERB, jtscm11,      jtscm112,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,2)   XPRIM(VERB, jtsct1,       jtsct2,       VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,3)   XPRIM(VERB, jtscz1,       jtscz2,       VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(0,4)   XPRIM(VERB, jtscy1,       jtscy2,       VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,6)   XPRIM(VERB, jtjfatt1,     jtjfatt2,     VASGSAFE,VF2NONE,0,   1,   0   );
 MN(1,7)   XPRIM(VERB, jtjfperm1,    jtjfperm2,    VASGSAFE,VF2NONE,0,   1,   0   );
 MN(1,20)  XPRIM(VERB, jtjfiles,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,21)  XPRIM(VERB, jtjopen,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,22)  XPRIM(VERB, jtjclose,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 MN(1,30)  XPRIM(VERB, jtjlocks,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,31)  XPRIM(VERB, jtjlock,      0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(1,32)  XPRIM(VERB, jtjunlock,    0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
#endif
  
 MN(1,43)  XPRIM(VERB, jtpathcwd,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,44)  XPRIM(VERB, jtpathchdir,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(1,46)  XPRIM(VERB, jtpathdll,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);

 MN(1,55)  XPRIM(VERB, jtjferase,    0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(2,0)   XPRIM(VERB, jthost,       0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(2,1)   XPRIM(VERB, jthostne,     0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
#if 0  // doesn't work
 MN(2,2)   XPRIM(VERB, jthostio,     0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(2,3)   XPRIM(VERB, jtjwait,      0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
#endif
// (2,4) reserved for messages from host OS
// (2,9) reserved for messages from host OS
 MN(2,5)   XPRIM(VERB, jtjgetenv,    0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(2,6)   XPRIM(VERB, jtjgetpid,    0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(2,7)   XPRIM(VERB, jtjgetx,      0,            VASGSAFE,VF2NONE,1,   RMAX,RMAX);
 MN(2,8)   XPRIM(VERB, jtcerrno,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(2,10)  XPRIM(VERB, jtgsignal,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(2,55)  XPRIM(VERB,jtjoff,0,VASGSAFE,VF2NONE,RMAX,0,0);
 MN(3,1)   XPRIM(VERB, jtbinrep1,    jtbinrep2,    VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,2)   XPRIM(VERB, jtunbin,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,3)   XPRIM(VERB, jthexrep1,    jthexrep2,    VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,4)   XPRIM(VERB, 0,            jtic2,        VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,5)   XPRIM(VERB, 0,            jtfc2,        VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,6)   XPRIM(VERB, jtlock1,      jtlock2,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(3,9)   XPRIM(VERB, 0,            jtnouninfo2,  VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,1)   XPRIM(VERB, jtnl1,        jtnl2,        VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,3)   XPRIM(VERB, jtsnl,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,4)   XPRIM(VERB, jtscind,      0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(4,5)   XPRIM(VERB, jtnch,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,6)   XPRIM(VERB, jtscriptstring,        0,   VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,7)   XPRIM(VERB, jtscriptnum,        0,      VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(4,8)   XPRIM(ADV, jtcreatecachedref,jtvalenceerr,   VASGSAFE,VF2NONE,0L,  0L,  0L  );
 MN(4,55)  XPRIM(VERB, jtex,         0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(5,0)   XPRIM(ADV, jtfxx,jtvalenceerr,          VASGSAFE,VF2NONE,0L,  0L,  0L  );
 MN(5,1)   XPRIM(VERB, jtarx,        0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(5,2)   XPRIM(VERB, jtdrx,        0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(5,4)   XPRIM(VERB, jttrx,        0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(5,5)   XPRIM(VERB, jtlrx,        0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(5,6)   XPRIM(VERB, jtprx,        0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(5,7)   XPRIM(VERB, 0,            jtxrx,        VASGSAFE,VF2NONE,RMAX,0,   0   );
 MN(6,2)   XPRIM(VERB, jttsit1,      jttsit2,      VFLAGNONE,VF2NONE,1,   0,   1   );
 MN(6,3)   XPRIM(VERB, jtdl,         0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(6,4)   XPRIM(VERB, jtparsercalls,0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,5)   XPRIM(VERB, jtpeekdata,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);  // turn on to provide a J window into the running system
  // MN(6,6)   XPRIM(VERB, jtprocarch,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);   // read whether AVX supported
  // MN(6,7)   XPRIM(VERB, jtprocfeat,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);   // report cpu features
 MN(6,8)   XPRIM(VERB, jtqpfreq,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,9)   XPRIM(VERB, jtqpctr,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,10)  XPRIM(VERB, jtpmarea1,    jtpmarea2,    VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,11)  XPRIM(VERB, jtpmunpack,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,12)  XPRIM(VERB, jtpmctr,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(6,13)  XPRIM(VERB, jtpmstats,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(7,0)   XPRIM(VERB, jtsp,         0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(7,1)   XPRIM(VERB, jtsphwmk,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(7,2)   XPRIM(VERB, jtspit,       0,            VFLAGNONE,VF2NONE,1,   RMAX,RMAX);
 MN(7,3)   XPRIM(VERB, jtspcount,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(7,5)   XPRIM(VERB, jtspfor,      0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(7,6)   XPRIM(VERB, jtspforloc,   0,            VASGSAFE,VF2NONE,0,   RMAX,RMAX);
 MN(7,7)   XPRIM(VERB, jtspresident, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(7,8)   XPRIM(VERB, jtspallthreads, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,0)   XPRIM(VERB, jtrngseedq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,1)   XPRIM(VERB, jtrngseeds,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,2)   XPRIM(VERB, jtdispq,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,3)   XPRIM(VERB, jtdisps,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,4)   XPRIM(VERB, jtnmcacheq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,5)   XPRIM(VERB, jtnmcaches,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,6)   XPRIM(VERB, jtboxq,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,7)   XPRIM(VERB, jtboxs,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,8)   XPRIM(VERB, jtevmq,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,9)   XPRIM(VERB, jtevms,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,10)  XPRIM(VERB, jtppq,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,11)  XPRIM(VERB, jtpps,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,12)  XPRIM(VERB, jtsysq,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,14)  XPRIM(VERB, jtversq,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,16)  XPRIM(VERB, jtposq,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,17)  XPRIM(VERB, jtposs,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,18)  XPRIM(VERB, jtctq,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,19)  XPRIM(VERB, jtcts,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,20)  XPRIM(VERB, jtmmaxq,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,21)  XPRIM(VERB, jtmmaxs,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,22)  XPRIM(VERB, jtqfill,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,23)  XPRIM(VERB, 0,            jtindaudit,   VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,24)  XPRIM(VERB, jtseclevq,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,25)  XPRIM(VERB, jtseclevs,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,26)  XPRIM(VERB, jtiepq,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(9,27)  XPRIM(VERB, jtieps,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(9,28)  XPRIM(VERB, jtiepdoq,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(9,29)  XPRIM(VERB, jtiepdos,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(9,32)  XPRIM(VERB, jtecmtriesq,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(9,33)  XPRIM(VERB, jtecmtriess,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,34)  XPRIM(VERB, jtassertq,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX); 
 MN(9,35)  XPRIM(VERB, jtasserts,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,36)  XPRIM(VERB, jtoutparmq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,37)  XPRIM(VERB, jtoutparms,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,38)  XPRIM(VERB, jtlocsizeq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,39)  XPRIM(VERB, jtlocsizes,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,40)  XPRIM(VERB, jtretcommq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,41)  XPRIM(VERB, jtretcomms,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,42)  XPRIM(VERB, jtrngselectq, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,43)  XPRIM(VERB, jtrngselects, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,44)  XPRIM(VERB, jtrngstateq,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,45)  XPRIM(VERB, jtrngstates,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,46)  XPRIM(VERB, jtbreakfnq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,47)  XPRIM(VERB, jtbreakfns,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,52)  XPRIM(VERB, jtasgzombq,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,53)  XPRIM(VERB, jtasgzombs,   0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,54)  XPRIM(VERB, jtdeprecxq,   0,        VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,55)  XPRIM(VERB, jtdeprecxs,   0,       VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
#if MEMHISTO
 MN(9,54)  XPRIM(VERB, jtmemhistoq, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,55)  XPRIM(VERB, jtmemhistos, 0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,62)  XPRIM(VERB, jtmemhashq, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,63)  XPRIM(VERB, jtmemhashs, 0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
#endif
 MN(9,56)  XPRIM(VERB, jtcpufeature, jtcpufeature2, VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,57)  XPRIM(VERB, jtaudittdisab, 0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,58)  XPRIM(VERB, jtgemmtune, jtgemmtune2,    VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,59)  XPRIM(VERB, jtemsglevel, 0,    VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,60)  XPRIM(VERB, jtleakblockread, 0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,61)  XPRIM(VERB, jtleakblockreset, 0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,63)  XPRIM(VERB, jtshowinplacing1, jtshowinplacing2,  VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE,RMAX,RMAX,RMAX);
 MN(9,66)  XPRIM(VERB, jtcheckcompfeatures, 0,  VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,0)  XPRIM(VERB, jtdbc,        0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,1)  XPRIM(VERB, jtdbstack,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,2)  XPRIM(VERB, jtdbstopq,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,3)  XPRIM(VERB, jtdbstops,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,4)  XPRIM(VERB, jtdbrun,      0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,5)  XPRIM(VERB, jtdbnext,     0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,6)  XPRIM(VERB, jtdbret,      0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,7)  XPRIM(VERB, jtdbjump,     0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,8)  XPRIM(VERB, jtdbsig1,     jtdbsig2,     VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,9)  XPRIM(VERB, jtdbrr1,      jtdbrr2,      VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,10) XPRIM(VERB, 0,0, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);  // still in stdlib
 MN(13,11) XPRIM(VERB, jtdberr,      jtdberr2,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,12) XPRIM(VERB, jtdbetx,      0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,13) XPRIM(VERB, jtdbcall,     0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,14) XPRIM(VERB, jtdbtrapq,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,15) XPRIM(VERB, jtdbtraps,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,16) XPRIM(VERB, 0,0, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);  // still in stdlib
 MN(13,17) XPRIM(VERB, jtdbq,        0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,18) XPRIM(VERB, jtdbstackz,   0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,19) XPRIM(VERB, jtdbcutback,  0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,20) XPRIM(VERB, jtdbstepover1,jtdbstepover2,VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,21) XPRIM(VERB, jtdbstepinto1,jtdbstepinto2,VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,22) XPRIM(VERB, jtdbstepout1, jtdbstepout2, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,23) XPRIM(VERB, jtdbpasss, jtdbpasss, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,5)  XPRIM(VERB, jtcdf,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,7)  XPRIM(VERB, jtdllsymset,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);   
 MN(15,8)  XPRIM(VERB, jtgh15,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,9)  XPRIM(VERB, jtfh15,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,10) XPRIM(VERB, jtcder,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,11) XPRIM(VERB, jtcderx,      0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,12) XPRIM(VERB, jtdllsymhdr,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
#if 0  //
 MN(15,12) XPRIM(VERB, jtsmmblks,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
#endif
 MN(15,13) XPRIM(VERB, jtcallback,   jtcallback2,  VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,14) XPRIM(VERB, jtdllsymdat,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,16) XPRIM(VERB, jtnfes,       0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,17) XPRIM(VERB, jtcallbackx,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,18) XPRIM(VERB, jtmemalign,   jtmemalign,   VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,19) XPRIM(VERB, jtdllvaladdr, 0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,20) XPRIM(VERB, jtcddlopen,   0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,21) XPRIM(VERB, 0,            jtcddlsym,    VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,22) XPRIM(VERB, jtcddlclose,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,23) XPRIM(VERB, jtcdq,        0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(18,0)  XPRIM(VERB, jtlocnc,      0,            VFLAGNONE,VF2NONE,0,   RMAX,RMAX);
 MN(18,1)  XPRIM(VERB, jtlocnl1,     jtlocnl2,     VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(18,55) XPRIM(VERB, jtlocexmark,  0,            VFLAGNONE,VF2NONE,0,   RMAX,RMAX);
 MN(128,0) XPRIM(VERB, jtqr,         0,            VASGSAFE,VF2NONE,2,   RMAX,RMAX);
 MN(128,1) XPRIM(VERB, jtrinv,       0,            VASGSAFE,VF2NONE,2,   RMAX,RMAX);
 MN(0,-1) XPRIM(VERB, jtskipinscript,            0, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(5,-1)  XPRIM(VERB, 0,            jtoutstr,     VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,-1) XPRIM(VERB, 0,            jtfindrange,  VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,-2) XPRIM(VERB, 0,            jtfindrange4, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,-3) XPRIM(VERB, 0,            jtfindrange2, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,-4) XPRIM(VERB, jthdrinfo,     0,           VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,-5) XPRIM(VERB, 0,            jtauditpyx,   VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(13,-6) XPRIM(VERB, jtstackfault,            0, VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
#if PYXES
 MN(13,-7) XPRIM(VERB, jtnulljob,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
#endif
 MN(13,-8) XPRIM(VERB, jtcheckfreepool, 0,         VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(15,6)  XPRIM(VERB, jtdllsymget,  0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(18,-1) XPRIM(VERB, jtlocmap,     0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(18,-2) XPRIM(VERB, jtsympool,    0,            VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(18,-3) XPRIM(VERB, jtlocnlz1,    0,     VFLAGNONE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,3) XPRIM(VERB, jtcrc1,       jtcrc2,       VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,4) XPRIM(VERB, jtrngraw,     0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,6) XPRIM(VERB, jtshasum1,    jtshasum2,    VASGSAFE,VF2NONE,1,1,RMAX);
 MN(128,7) XPRIM(VERB, 0,            jtaes2,       VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);
 MN(128,8) XPRIM(VERB, jtqhash12,    jtqhash12,    VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE,RMAX,RMAX,RMAX);  
 MN(128,10) XPRIM(VERB, jtludecomp,         0,     VASGSAFE,VF2NONE,RMAX,   RMAX,RMAX);
//  MN(128,110) XPRIM(VERB, jttest1,    0,            VASGSAFE,VF2NONE,RMAX,RMAX,RMAX);

// TUNE printf("avg # probes=%7.3f\n",(double)totprobes/(double)totslots);
}
#if 1   // obsolete 
// m!:n, requiring m & n to be nouns, and always returning AVN (never C)
// All the verbs are prebuilt in foreignA.  Only the second cacheline of each will be used.  The m & n args are in localuse.lu1.foreignmn[] and are not
// saved in fgh.  foreignA is accessed as an open hash with linear probing.  We install the most common foreigns first so they will take the fewest probes.
F2(jtforeign){F2PREFIP;I p,q;A z;
 ARGCHK2(a,w);
// obsolete  ASSERT(AT(a)&NOUN&&AT(w)&NOUN,EVDOMAIN)
 ASSERT(!((AT(a)|AT(w))&VERB),EVDOMAIN)
 p=i0(a); q=i0(w); RE(0);
 if(p!=11){  // normal m!:n
  ASSERT(BETWEENC(p,0,128),EVDOMAIN) ASSERT(BETWEENC(q,-10,111),EVDOMAIN)   // check reasonable inputs
  ASSERT((z=findslot(p,q))!=0,EVDOMAIN)  // look up the (m,n), fail if not found
  RETF(z);  // return the block we found
 }else{
  // 11!:n (wd) has a wide range of options, but they all go through one routine.  We allocate each time
  fdefallo(z);  // allocate the block
  fdeffillall(z,0,CIBEAM,VERB, jtwd,jtvalenceerr, 0,0,0L, VASGSAFE, 1L,RMAX,RMAX,fffv->localuse.lu1.foreignmn[0]=p,fffv->localuse.lu1.foreignmn[1]=q);
  R z;
 }
}
#else

F2(jtforeign){F2PREFIP;I p,q;
 ARGCHK2(a,w);
 ASSERT(AT(a)&NOUN&&AT(w)&NOUN,EVDOMAIN)
 p=i0(a); q=i0(w); RE(0);
#define XC(p,q) q
 // We use a 2-level switch to avoid the huge gaps in the table
 // negative values of q are internal-use-only foreigns
 switch(p){
 case 0:
  switch(q){
  case XC(0,  0): 
  case XC(0,100): R CDERIV(CIBEAM, jtscm00,      jtscm002,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,  1): 
  case XC(0,101): R CDERIV(CIBEAM, jtscm01,      jtscm012,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0, 10): 
  case XC(0,110): R CDERIV(CIBEAM, jtscm10,      jtscm102,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0, 11): 
  case XC(0,111): R CDERIV(CIBEAM, jtscm11,      jtscm112,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,2):   R CDERIV(CIBEAM, jtsct1,       jtsct2,       VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,3):   R CDERIV(CIBEAM, jtscz1,       jtscz2,       VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,4):   R CDERIV(CIBEAM, jtscy1,       jtscy2,       VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,-1): R CDERIV(CIBEAM, jtskipinscript,            0, VFLAGNONE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 1:
  switch(q){
  case XC(1,0):   R CDERIV(CIBEAM, jtjdir,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,1):   R CDERIV(CIBEAM, jtjfread,     0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(1,2):   R CDERIV(CIBEAM, 0,            jtjfwrite,    VASGSAFE,RMAX,RMAX,0   );
  case XC(1,3):   R CDERIV(CIBEAM, 0,            jtjfappend,   VASGSAFE,RMAX,RMAX,0   );
  case XC(1,4):   R CDERIV(CIBEAM, jtjfsize,     0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(1,5):   R CDERIV(CIBEAM, jtjmkdir,     0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(1,6):   R CDERIV(CIBEAM, jtjfatt1,     jtjfatt2,     VASGSAFE,0,   1,   0   );
  case XC(1,7):   R CDERIV(CIBEAM, jtjfperm1,    jtjfperm2,    VASGSAFE,0,   1,   0   );

  case XC(1,11):  R CDERIV(CIBEAM, jtjiread,     0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(1,12):  R CDERIV(CIBEAM, 0,            jtjiwrite,    VASGSAFE,RMAX,RMAX,1   );
  case XC(1,20):  R CDERIV(CIBEAM, jtjfiles,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,21):  R CDERIV(CIBEAM, jtjopen,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,22):  R CDERIV(CIBEAM, jtjclose,     0,            VASGSAFE,RMAX,RMAX,RMAX);

// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
  case XC(1,30):  R CDERIV(CIBEAM, jtjlocks,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,31):  R CDERIV(CIBEAM, jtjlock,      0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(1,32):  R CDERIV(CIBEAM, jtjunlock,    0,            VASGSAFE,1,   RMAX,RMAX);
#endif
  
  case XC(1,43):  R CDERIV(CIBEAM, jtpathcwd,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,44):  R CDERIV(CIBEAM, jtpathchdir,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,46):  R CDERIV(CIBEAM, jtpathdll,    0,            VASGSAFE,RMAX,RMAX,RMAX);

  case XC(1,55):  R CDERIV(CIBEAM, jtjferase,    0,            VASGSAFE,0,   RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 2:
  switch(q){
  case XC(2,0):   R CDERIV(CIBEAM, jthost,       0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,1):   R CDERIV(CIBEAM, jthostne,     0,            VASGSAFE,1,   RMAX,RMAX);
#if 0  // doesn't work
  case XC(2,2):   R CDERIV(CIBEAM, jthostio,     0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,3):   R CDERIV(CIBEAM, jtjwait,      0,            VASGSAFE,0,   RMAX,RMAX);
#endif
// (2,4) reserved for messages from host OS
  case XC(2,5):   R CDERIV(CIBEAM, jtjgetenv,    0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,6):   R CDERIV(CIBEAM, jtjgetpid,    0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,7):   R CDERIV(CIBEAM, jtjgetx,      0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,8):   R CDERIV(CIBEAM, jtcerrno,     0,            VASGSAFE,RMAX,RMAX,RMAX);
// (2,9) reserved for messages from host OS
  case XC(2,10):  R CDERIV(CIBEAM, jtgsignal,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(2,55):  R CDERIV(CIBEAM,jtjoff,0,VASGSAFE,RMAX,0,0);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 3:
  switch(q){
  case XC(3,0):   R CDERIV(CIBEAM, jtstype,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,1):   R CDERIV(CIBEAM, jtbinrep1,    jtbinrep2,    VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,2):   R CDERIV(CIBEAM, jtunbin,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,3):   R CDERIV(CIBEAM, jthexrep1,    jthexrep2,    VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,4):   R CDERIV(CIBEAM, 0,            jtic2,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,5):   R CDERIV(CIBEAM, 0,            jtfc2,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,6):   R CDERIV(CIBEAM, jtlock1,      jtlock2,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,9):   R CDERIV(CIBEAM, 0,            jtnouninfo2,  VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,10):  R CDERIV(CIBEAM, jttobase64,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,11):  R CDERIV(CIBEAM, jtfrombase64, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,12):  R CDERIV(CIBEAM, 0,            jtlowerupper, VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 4:
  switch(q){
  case XC(4,0):   R CDERIV(CIBEAM, jtnc,         0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(4,1):   R CDERIV(CIBEAM, jtnl1,        jtnl2,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,3):   R CDERIV(CIBEAM, jtsnl,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,4):   R CDERIV(CIBEAM, jtscind,      0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(4,5):   R CDERIV(CIBEAM, jtnch,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,6):   R CDERIV(CIBEAM, jtscriptstring,        0,   VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,7):   R CDERIV(CIBEAM, jtscriptnum,        0,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,8):   R fdef(0,CIBEAM,ADV, jtcreatecachedref,jtvalenceerr,  a,w,0L,        VASGSAFE,0L,  0L,  0L  );
  case XC(4,55):  R CDERIV(CIBEAM, jtex,         0,            VASGSAFE,0,   RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 5:
  switch(q){
  case XC(5,0):   R fdef(0,CIBEAM,ADV, jtfxx,jtvalenceerr,  a,w,0L,        VASGSAFE,0L,  0L,  0L  );
  case XC(5,1):   R CDERIV(CIBEAM, jtarx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,2):   R CDERIV(CIBEAM, jtdrx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,4):   R CDERIV(CIBEAM, jttrx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,5):   R CDERIV(CIBEAM, jtlrx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,6):   R CDERIV(CIBEAM, jtprx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,7):   R CDERIV(CIBEAM, 0,            jtxrx,        VASGSAFE,RMAX,0,   0   );
  case XC(5,-1):  R CDERIV(CIBEAM, 0,            jtoutstr,     VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 6:
  switch(q){
  case XC(6,0):   R CDERIV(CIBEAM, jtts0,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,1):   R CDERIV(CIBEAM, jttss,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,2):   R CDERIV(CIBEAM, jttsit1,      jttsit2,      VFLAGNONE,1,   0,   1   );
  case XC(6,3):   R CDERIV(CIBEAM, jtdl,         0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(6,4):   R CDERIV(CIBEAM, jtparsercalls,0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,5):   R CDERIV(CIBEAM, jtpeekdata,   0,            VASGSAFE,RMAX,RMAX,RMAX);  // turn on to provide a J window into the running system
  // case XC(6,6):   R CDERIV(CIBEAM, jtprocarch,   0,            VASGSAFE,RMAX,RMAX,RMAX);   // read whether AVX supported
  // case XC(6,7):   R CDERIV(CIBEAM, jtprocfeat,   0,            VASGSAFE,RMAX,RMAX,RMAX);   // report cpu features
  case XC(6,8):   R CDERIV(CIBEAM, jtqpfreq,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,9):   R CDERIV(CIBEAM, jtqpctr,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,10):  R CDERIV(CIBEAM, jtpmarea1,    jtpmarea2,    VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,11):  R CDERIV(CIBEAM, jtpmunpack,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,12):  R CDERIV(CIBEAM, jtpmctr,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,13):  R CDERIV(CIBEAM, jtpmstats,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,14):  R CDERIV(CIBEAM, jtinttoe,    0,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,15):  R CDERIV(CIBEAM, jtetoint,    0,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,16):  R CDERIV(CIBEAM, jtetoiso8601,jtetoiso8601,  VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,17):  R CDERIV(CIBEAM, jtiso8601toe,jtiso8601toe,  VASGSAFE,RMAX,RMAX,RMAX);
  case XC(6,18):  R CDERIV(CIBEAM, jtstringtoe,jtstringtoe,    VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 7:
  switch(q){
  case XC(7,0):   R CDERIV(CIBEAM, jtsp,         0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,1):   R CDERIV(CIBEAM, jtsphwmk,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,2):   R CDERIV(CIBEAM, jtspit,       0,            VFLAGNONE,1,   RMAX,RMAX);
  case XC(7,3):   R CDERIV(CIBEAM, jtspcount,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,5):   R CDERIV(CIBEAM, jtspfor,      0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(7,6):   R CDERIV(CIBEAM, jtspforloc,   0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(7,7):   R CDERIV(CIBEAM, jtspresident, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,8):   R CDERIV(CIBEAM, jtspallthreads, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 8:
  switch(q){
  case XC(8,0):   R CDERIV(CIBEAM, jtfmt01,      jtfmt02,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(8,1):   R CDERIV(CIBEAM, jtfmt11,      jtfmt12,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(8,2):   R CDERIV(CIBEAM, jtfmt21,      jtfmt22,      VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 9:
  switch(q){
  case XC(9,0):   R CDERIV(CIBEAM, jtrngseedq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,1):   R CDERIV(CIBEAM, jtrngseeds,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,2):   R CDERIV(CIBEAM, jtdispq,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,3):   R CDERIV(CIBEAM, jtdisps,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,4):   R CDERIV(CIBEAM, jtnmcacheq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,5):   R CDERIV(CIBEAM, jtnmcaches,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,6):   R CDERIV(CIBEAM, jtboxq,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,7):   R CDERIV(CIBEAM, jtboxs,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,8):   R CDERIV(CIBEAM, jtevmq,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,9):   R CDERIV(CIBEAM, jtevms,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,10):  R CDERIV(CIBEAM, jtppq,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,11):  R CDERIV(CIBEAM, jtpps,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,12):  R CDERIV(CIBEAM, jtsysq,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,14):  R CDERIV(CIBEAM, jtversq,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,16):  R CDERIV(CIBEAM, jtposq,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,17):  R CDERIV(CIBEAM, jtposs,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,18):  R CDERIV(CIBEAM, jtctq,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,19):  R CDERIV(CIBEAM, jtcts,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,20):  R CDERIV(CIBEAM, jtmmaxq,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,21):  R CDERIV(CIBEAM, jtmmaxs,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,22):  R CDERIV(CIBEAM, jtqfill,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,23):  R CDERIV(CIBEAM, 0,            jtindaudit,   VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,24):  R CDERIV(CIBEAM, jtseclevq,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,25):  R CDERIV(CIBEAM, jtseclevs,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,26):  R CDERIV(CIBEAM, jtiepq,       0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,27):  R CDERIV(CIBEAM, jtieps,       0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,28):  R CDERIV(CIBEAM, jtiepdoq,     0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,29):  R CDERIV(CIBEAM, jtiepdos,     0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,30):  R CDERIV(CIBEAM, jtcurnameq,   0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,32):  R CDERIV(CIBEAM, jtecmtriesq,    0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,33):  R CDERIV(CIBEAM, jtecmtriess,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,34):  R CDERIV(CIBEAM, jtassertq,    0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,35):  R CDERIV(CIBEAM, jtasserts,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,36):  R CDERIV(CIBEAM, jtoutparmq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,37):  R CDERIV(CIBEAM, jtoutparms,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,38):  R CDERIV(CIBEAM, jtlocsizeq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,39):  R CDERIV(CIBEAM, jtlocsizes,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,40):  R CDERIV(CIBEAM, jtretcommq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,41):  R CDERIV(CIBEAM, jtretcomms,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,42):  R CDERIV(CIBEAM, jtrngselectq, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,43):  R CDERIV(CIBEAM, jtrngselects, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,44):  R CDERIV(CIBEAM, jtrngstateq,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,45):  R CDERIV(CIBEAM, jtrngstates,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,46):  R CDERIV(CIBEAM, jtbreakfnq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,47):  R CDERIV(CIBEAM, jtbreakfns,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,52):  R CDERIV(CIBEAM, jtasgzombq,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,53):  R CDERIV(CIBEAM, jtasgzombs,   0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,54):  R CDERIV(CIBEAM, jtdeprecxq,   0,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,55):  R CDERIV(CIBEAM, jtdeprecxs,   0,       VFLAGNONE,RMAX,RMAX,RMAX);
#if MEMHISTO
  case XC(9,54):  R CDERIV(CIBEAM, jtmemhistoq, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,55):  R CDERIV(CIBEAM, jtmemhistos, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,62):  R CDERIV(CIBEAM, jtmemhashq, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,63):  R CDERIV(CIBEAM, jtmemhashs, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
#endif
  case XC(9,56):  R CDERIV(CIBEAM, jtcpufeature, jtcpufeature2, VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,57):  R CDERIV(CIBEAM, jtaudittdisab, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,58):  R CDERIV(CIBEAM, jtgemmtune, jtgemmtune2,    VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,59):  R CDERIV(CIBEAM, jtemsglevel, 0,    VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,60):  R CDERIV(CIBEAM, jtleakblockread, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,61):  R CDERIV(CIBEAM, jtleakblockreset, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,63):  R CDERIV(CIBEAM, jtshowinplacing1, jtshowinplacing2,  VASGSAFE|VJTFLGOK1|VJTFLGOK2,RMAX,RMAX,RMAX);
  case XC(9,66):  R CDERIV(CIBEAM, jtcheckcompfeatures, 0,  VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 11: R fdef(0,CIBEAM,VERB, jtwd,jtvalenceerr, a,w,0L, VASGSAFE, 1L,RMAX,RMAX);
 case 13:
  switch(q){
  case XC(13,0):  R CDERIV(CIBEAM, jtdbc,        0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,1):  R CDERIV(CIBEAM, jtdbstack,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,2):  R CDERIV(CIBEAM, jtdbstopq,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,3):  R CDERIV(CIBEAM, jtdbstops,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,4):  R CDERIV(CIBEAM, jtdbrun,      0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,5):  R CDERIV(CIBEAM, jtdbnext,     0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,6):  R CDERIV(CIBEAM, jtdbret,      0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,7):  R CDERIV(CIBEAM, jtdbjump,     0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,8):  R CDERIV(CIBEAM, jtdbsig1,     jtdbsig2,     VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,9):  R CDERIV(CIBEAM, jtdbrr1,      jtdbrr2,      VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,10):  R CDERIV(CIBEAM, 0,0, VFLAGNONE,RMAX,RMAX,RMAX);  // still in stdlib
  case XC(13,11): R CDERIV(CIBEAM, jtdberr,      jtdberr2,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,12): R CDERIV(CIBEAM, jtdbetx,      0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,13): R CDERIV(CIBEAM, jtdbcall,     0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,14): R CDERIV(CIBEAM, jtdbtrapq,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,15): R CDERIV(CIBEAM, jtdbtraps,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,16):  R CDERIV(CIBEAM, 0,0, VFLAGNONE,RMAX,RMAX,RMAX);  // still in stdlib
  case XC(13,17): R CDERIV(CIBEAM, jtdbq,        0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,18): R CDERIV(CIBEAM, jtdbstackz,   0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,19): R CDERIV(CIBEAM, jtdbcutback,  0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,20): R CDERIV(CIBEAM, jtdbstepover1,jtdbstepover2,VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,21): R CDERIV(CIBEAM, jtdbstepinto1,jtdbstepinto2,VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,22): R CDERIV(CIBEAM, jtdbstepout1, jtdbstepout2, VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,23): R CDERIV(CIBEAM, jtdbpasss, jtdbpasss, VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,-1): R CDERIV(CIBEAM, 0,            jtfindrange,  VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,-2): R CDERIV(CIBEAM, 0,            jtfindrange4, VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,-3): R CDERIV(CIBEAM, 0,            jtfindrange2, VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,-4): R CDERIV(CIBEAM, jthdrinfo,     0,           VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,-5): R CDERIV(CIBEAM, 0,            jtauditpyx,   VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,-6): R CDERIV(CIBEAM, jtstackfault,            0, VFLAGNONE,RMAX,RMAX,RMAX);
#if PYXES
  case XC(13,-7): R CDERIV(CIBEAM, jtnulljob,    0,            VASGSAFE,RMAX,RMAX,RMAX);
#endif
  case XC(13,-8): R CDERIV(CIBEAM, jtcheckfreepool, 0,         VFLAGNONE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 15:
  switch(q){
  case XC(15,0):  R CDERIV(CIBEAM, 0,            jtcd,         VASGSAFE|VJTFLGOK2,RMAX,   1L,  1L  );
  case XC(15,1):  R CDERIV(CIBEAM, jtmemr,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,2):  R CDERIV(CIBEAM, 0,            jtmemw,       VASGSAFE,RMAX,   RMAX,RMAX);
  case XC(15,3):  R CDERIV(CIBEAM, jtmema,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,4):  R CDERIV(CIBEAM, jtmemf,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,5):  R CDERIV(CIBEAM, jtcdf,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,6):  R CDERIV(CIBEAM, jtdllsymget,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,7):  R CDERIV(CIBEAM, jtdllsymset,  0,            VASGSAFE,RMAX,RMAX,RMAX);   
  case XC(15,8):  R CDERIV(CIBEAM, jtgh15,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,9):  R CDERIV(CIBEAM, jtfh15,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,10): R CDERIV(CIBEAM, jtcder,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,11): R CDERIV(CIBEAM, jtcderx,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,12): R CDERIV(CIBEAM, jtdllsymhdr,  0,            VASGSAFE,RMAX,RMAX,RMAX);
#if 0  //
  case XC(15,12): R CDERIV(CIBEAM, jtsmmblks,    0,            VASGSAFE,RMAX,RMAX,RMAX);
#endif
  case XC(15,13): R CDERIV(CIBEAM, jtcallback,   jtcallback2,  VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,14): R CDERIV(CIBEAM, jtdllsymdat,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,15): R CDERIV(CIBEAM, jtmemu,       jtmemu2,      VASGSAFE|VJTFLGOK1,RMAX,0,   0   );
  case XC(15,16): R CDERIV(CIBEAM, jtnfes,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,17): R CDERIV(CIBEAM, jtcallbackx,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,18): R CDERIV(CIBEAM, jtmemalign,   jtmemalign,   VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,19): R CDERIV(CIBEAM, jtdllvaladdr, 0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,20): R CDERIV(CIBEAM, jtcddlopen,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,21): R CDERIV(CIBEAM, 0,            jtcddlsym,    VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,22): R CDERIV(CIBEAM, jtcddlclose,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,23): R CDERIV(CIBEAM, jtcdq,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 18:
  switch(q){
  case XC(18,0):  R CDERIV(CIBEAM, jtlocnc,      0,            VFLAGNONE,0,   RMAX,RMAX);
  case XC(18,1):  R CDERIV(CIBEAM, jtlocnl1,     jtlocnl2,     VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,2):  R CDERIV(CIBEAM, jtlocpath1,   jtlocpath2,   VFLAGNONE,0,   1,   0   );
  case XC(18,3):  R CDERIV(CIBEAM, jtloccre1,    jtloccre2,    VFLAGNONE,RMAX,0,   RMAX);
  case XC(18,4):  R CDERIV(CIBEAM, jtlocswitch,  0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,5):  R CDERIV(CIBEAM, jtlocname,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,-1): R CDERIV(CIBEAM, jtlocmap,     0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,-2): R CDERIV(CIBEAM, jtsympool,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,-3):  R CDERIV(CIBEAM, jtlocnlz1,    0,     VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,55): R CDERIV(CIBEAM, jtlocexmark,  0,            VFLAGNONE,0,   RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
 case 128:
  switch(q){
  case XC(128,0): R CDERIV(CIBEAM, jtqr,         0,            VASGSAFE,2,   RMAX,RMAX);
  case XC(128,1): R CDERIV(CIBEAM, jtrinv,       0,            VASGSAFE,2,   RMAX,RMAX);
  case XC(128,2): R CDERIV(CIBEAM, 0,            jtapplystr,   VFLAGNONE,RMAX,1,   RMAX);
  case XC(128,3): R CDERIV(CIBEAM, jtcrc1,       jtcrc2,       VASGSAFE,RMAX,RMAX,RMAX);
  case XC(128,4): R CDERIV(CIBEAM, jtrngraw,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(128,5): R CDERIV(CIBEAM, jtisnan,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(128,6): R CDERIV(CIBEAM, jtshasum1,    jtshasum2,    VASGSAFE,1,1,RMAX);
  case XC(128,7): R CDERIV(CIBEAM, 0,            jtaes2,       VASGSAFE,RMAX,RMAX,RMAX);
  case XC(128,8): R CDERIV(CIBEAM, jtqhash12,    jtqhash12,    VASGSAFE|VJTFLGOK1|VJTFLGOK2,RMAX,RMAX,RMAX);  
//  case XC(128,110): R CDERIV(CIBEAM, jttest1,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(128,9): R CDERIVF(CIBEAM, jtmvmsparse,         0,     VASGSAFE,VF2WILLOPEN1,RMAX,   RMAX,RMAX);
  case XC(128,10): R CDERIV(CIBEAM, jtludecomp,         0,     VASGSAFE,RMAX,   RMAX,RMAX);
  case XC(128,11): R CDERIV(CIBEAM, 0,           jtlrtrim,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(128,12): R CDERIVF(CIBEAM, 0,           jtekupdate,     VASGSAFE|VJTFLGOK2,VF2WILLOPEN2A,RMAX,RMAX,RMAX);
  case XC(128,13): R CDERIVF(CIBEAM, jtfindspr, 0,                VASGSAFE|VJTFLGOK2,VF2WILLOPEN2A,RMAX,RMAX,RMAX);
  default: ASSERT(0,EVDOMAIN); break;
  } break;
//  default:        R foreignextra(a,w);
  default: ASSERT(0,EVDOMAIN); break;
 }
 ASSERT(0,EVVALENCE);  // any unknown combination is a domain error right away
}
#endif

/* SY_64 double trick - null routines here to avoid optimization */
#if SY_64 & SY_WIN32
void double_trick(D a,D b,D c,D d){;}
#endif

#if SY_64 && (SY_LINUX || SY_MAC || SY_FREEBSD || SY_OPENBSD)
void double_trick(D a,D b,D c,D d,D e,D f,D g,D h){;}
#endif

#ifdef C_CD_ARMHF
void double_trick(float f0,float f1,float f2,float f3,float f4,float f5,float f6,float f7,float f8,float f9,float f10,float f11,float f12,float f13,float f14,float f15){;}
#endif
