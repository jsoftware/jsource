/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos aka Foreign: External, Experimental, & Extra                      */

#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "j.h"
#include "x.h"

#define SDERIV(id,f1,f2,flag,m,l,r)  \
 fdef(0,id,VERB,secf1,secf2,a,w,v2((I)(f1?f1:jtdomainerr1),(I)(f2?f2:jtdomainerr2)),(flag),(I)m,(I)l,(I)r)

#define SDERI2(id,f1,f2,flag,m,l,r)  \
 fdef(0,id,VERB,f1,   secf2,a,w,v2((I)(f1?f1:jtdomainerr1),(I)(f2?f2:jtdomainerr2)),(flag),(I)m,(I)l,(I)r)


static DF1(secf1){F1PREFIP; A h=FAV(self)->fgh[2]; ASSERT(!JT(jt,seclev),EVSECURE); R CALL1IP((AF)AV(h)[0],  w,self);}
static DF2(secf2){F2PREFIP; A h=FAV(self)->fgh[2]; ASSERT(!JT(jt,seclev),EVSECURE); R CALL2IP((AF)AV(h)[1],a,w,self);}

// undocumented 13!:80 functions, used to test condrange
static DF2(jtfindrange){
 ARGCHK2(a,w);
 I *av = AV(a);
 CR rng = condrange(AV(w),AN(w),av[0],av[1],av[2]);  // starting min, starting max, max range
 R v2(rng.min,rng.range);
} // 13!:80
static DF2(jtfindrange4){
 ARGCHK2(a,w);
 I *av = AV(a);
 CR rng = condrange4(C4AV(w),AN(w),av[0],av[1],av[2]);
 R v2(rng.min,rng.range);
}  // 13!:81
static DF2(jtfindrange2){
 ARGCHK2(a,w);
 I *av = AV(a);
 CR rng = condrange2(USAV(w),AN(w),av[0],av[1],av[2]);
 R v2(rng.min,rng.range);
}  // 13!:82

// 13!:83  Return header info: t, flag, m, type, c, n, r
static DF1(jthdrinfo){A z;
 ARGCHK1(w);
 GAT0(z,INT,7,1);
 IAV(z)[0]=AK(w); IAV(z)[1]=AFLAG(w); IAV(z)[2]=AM(w); IAV(z)[3]=AT(w); IAV(z)[4]=AC(w); IAV(z)[5]=AN(w); IAV(z)[6]=AR(w);
 R z;
}

F2(jtforeign){F2PREFIP;I p,q;
 ARGCHK2(a,w);
 ASSERT(AT(a)&NOUN&&AT(w)&NOUN,EVDOMAIN)
 p=i0(a); q=i0(w); RE(0);
 if(11==p)R fdef(0,CIBEAM,VERB, jtwd,0L, a,w,0L, VASGSAFE, 1L,RMAX,RMAX);
 ASSERT((UI)p<=(UI)128 && (UI)q<XCC,EVDOMAIN);
 switch(XC(p,q)){
  case XC(0,  0): 
  case XC(0,100): R SDERI2(CIBEAM, jtscm00,      jtscm002,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,  1): 
  case XC(0,101): R SDERI2(CIBEAM, jtscm01,      jtscm012,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0, 10): 
  case XC(0,110): R SDERI2(CIBEAM, jtscm10,      jtscm102,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0, 11): 
  case XC(0,111): R SDERI2(CIBEAM, jtscm11,      jtscm112,     VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,2):   R SDERI2(CIBEAM, jtsct1,       jtsct2,       VASGSAFE,RMAX,RMAX,RMAX);
  case XC(0,3):   R SDERI2(CIBEAM, jtscz1,       jtscz2,       VASGSAFE,RMAX,RMAX,RMAX);

  case XC(1,0):   R SDERIV(CIBEAM, jtjdir,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,1):   R SDERIV(CIBEAM, jtjfread,     0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(1,2):   R SDERIV(CIBEAM, 0,            jtjfwrite,    VASGSAFE,RMAX,RMAX,0   );
  case XC(1,3):   R SDERIV(CIBEAM, 0,            jtjfappend,   VASGSAFE,RMAX,RMAX,0   );
  case XC(1,4):   R SDERIV(CIBEAM, jtjfsize,     0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(1,5):   R SDERIV(CIBEAM, jtjmkdir,     0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(1,6):   R SDERIV(CIBEAM, jtjfatt1,     jtjfatt2,     VASGSAFE,0,   1,   0   );
  case XC(1,7):   R SDERIV(CIBEAM, jtjfperm1,    jtjfperm2,    VASGSAFE,0,   1,   0   );

  case XC(1,11):  R SDERIV(CIBEAM, jtjiread,     0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(1,12):  R SDERIV(CIBEAM, 0,            jtjiwrite,    VASGSAFE,RMAX,RMAX,1   );
  case XC(1,20):  R SDERIV(CIBEAM, jtjfiles,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,21):  R SDERIV(CIBEAM, jtjopen,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,22):  R SDERIV(CIBEAM, jtjclose,     0,            VASGSAFE,RMAX,RMAX,RMAX);

// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
  case XC(1,30):  R SDERIV(CIBEAM, jtjlocks,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,31):  R SDERIV(CIBEAM, jtjlock,      0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(1,32):  R SDERIV(CIBEAM, jtjunlock,    0,            VASGSAFE,1,   RMAX,RMAX);
#endif
  
  case XC(1,43):  R SDERIV(CIBEAM, jtpathcwd,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,44):  R SDERIV(CIBEAM, jtpathchdir,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(1,46):  R CDERIV(CIBEAM, jtpathdll,    0,            VASGSAFE,RMAX,RMAX,RMAX);

  case XC(1,55):  R SDERIV(CIBEAM, jtjferase,    0,            VASGSAFE,0,   RMAX,RMAX);

  case XC(2,0):   R SDERIV(CIBEAM, jthost,       0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,1):   R SDERIV(CIBEAM, jthostne,     0,            VASGSAFE,1,   RMAX,RMAX);
#if 0  // doesn't work
  case XC(2,2):   R SDERIV(CIBEAM, jthostio,     0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,3):   R SDERIV(CIBEAM, jtjwait,      0,            VASGSAFE,0,   RMAX,RMAX);
#endif
// (2,4) reserved for messages from host OS
  case XC(2,5):   R SDERIV(CIBEAM, jtjgetenv,    0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,6):   R SDERIV(CIBEAM, jtjgetpid,    0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,7):   R SDERIV(CIBEAM, jtjgetx,      0,            VASGSAFE,1,   RMAX,RMAX);
  case XC(2,8):   R SDERIV(CIBEAM, jtcerrno,     0,            VASGSAFE,RMAX,RMAX,RMAX);
// (2,9) reserved for messages from host OS
  case XC(2,55):  R CDERIV(CIBEAM,jtjoff,0,VASGSAFE,RMAX,0,0);

  case XC(3,0):   R CDERIV(CIBEAM, jtstype,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,1):   R CDERIV(CIBEAM, jtbinrep1,    jtbinrep2,    VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,2):   R CDERIV(CIBEAM, jtunbin,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,3):   R CDERIV(CIBEAM, jthexrep1,    jthexrep2,    VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,4):   R CDERIV(CIBEAM, 0,            jtic2,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,5):   R CDERIV(CIBEAM, 0,            jtfc2,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,6):   R CDERIV(CIBEAM, jtlock1,      jtlock2,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,10):  R CDERIV(CIBEAM, jttobase64,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(3,11):  R CDERIV(CIBEAM, jtfrombase64, 0,            VASGSAFE,RMAX,RMAX,RMAX);

  case XC(3,9):   R CDERIV(CIBEAM, 0,            jtnouninfo2,  VASGSAFE,RMAX,RMAX,RMAX);

  case XC(4,0):   R CDERIV(CIBEAM, jtnc,         0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(4,1):   R CDERIV(CIBEAM, jtnl1,        jtnl2,        VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,3):   R CDERIV(CIBEAM, jtsnl,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,4):   R CDERIV(CIBEAM, jtscind,      0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(4,5):   R CDERIV(CIBEAM, jtnch,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,6):   R CDERIV(CIBEAM, jtscriptstring,        0,   VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,7):   R CDERIV(CIBEAM, jtscriptnum,        0,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(4,8):   R fdef(0,CIBEAM,ADV, jtcreatecachedref,0L,  a,w,0L,        VASGSAFE,0L,  0L,  0L  );
  case XC(4,55):  R CDERIV(CIBEAM, jtex,         0,            VASGSAFE,0,   RMAX,RMAX);

  case XC(5,0):   R fdef(0,CIBEAM,ADV, jtfxx,0L,  a,w,0L,        VASGSAFE,0L,  0L,  0L  );
  case XC(5,1):   R CDERIV(CIBEAM, jtarx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,2):   R CDERIV(CIBEAM, jtdrx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,4):   R CDERIV(CIBEAM, jttrx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,5):   R CDERIV(CIBEAM, jtlrx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,6):   R CDERIV(CIBEAM, jtprx,        0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(5,7):   R CDERIV(CIBEAM, 0,            jtxrx,        VASGSAFE,RMAX,0,   0   );
  case XC(5,30):  R CDERIV(CIBEAM, 0,            jtoutstr,     VASGSAFE,RMAX,RMAX,RMAX);

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

  case XC(7,0):   R CDERIV(CIBEAM, jtsp,         0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,1):   R CDERIV(CIBEAM, jtsphwmk,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,2):   R CDERIV(CIBEAM, jtspit,       0,            VFLAGNONE,1,   RMAX,RMAX);
  case XC(7,3):   R CDERIV(CIBEAM, jtspcount,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(7,5):   R CDERIV(CIBEAM, jtspfor,      0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(7,6):   R CDERIV(CIBEAM, jtspforloc,   0,            VASGSAFE,0,   RMAX,RMAX);
  case XC(7,7):   R CDERIV(CIBEAM, jtspresident, 0,            VASGSAFE,RMAX,RMAX,RMAX);

  case XC(8,0):   R CDERIV(CIBEAM, jtfmt01,      jtfmt02,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(8,1):   R CDERIV(CIBEAM, jtfmt11,      jtfmt12,      VASGSAFE,RMAX,RMAX,RMAX);
  case XC(8,2):   R CDERIV(CIBEAM, jtfmt21,      jtfmt22,      VASGSAFE,RMAX,RMAX,RMAX);

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
  case XC(9,24):  R CDERIV(CIBEAM, jtseclevq,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,25):  R CDERIV(CIBEAM, jtseclevs,    0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(9,26):  R CDERIV(CIBEAM, jtiepq,       0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,27):  R CDERIV(CIBEAM, jtieps,       0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,28):  R CDERIV(CIBEAM, jtiepdoq,     0,            VASGSAFE,RMAX,RMAX,RMAX); 
  case XC(9,29):  R CDERIV(CIBEAM, jtiepdos,     0,            VASGSAFE,RMAX,RMAX,RMAX); 
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
  case XC(9,60):  R CDERIV(CIBEAM, jtleakblockread, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,61):  R CDERIV(CIBEAM, jtleakblockreset, 0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(9,63):  R CDERIV(CIBEAM, jtshowinplacing1, jtshowinplacing2,  VASGSAFE|VJTFLGOK1|VJTFLGOK2,RMAX,RMAX,RMAX);
  case XC(9,66):  R CDERIV(CIBEAM, jtcheckcompfeatures, 0,  VASGSAFE,RMAX,RMAX,RMAX);



/* case XC(11,*):      handled at beginning */
/* case XC(12,*):      reserved for D.H. Steinbrook tree stuff         */

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
  case XC(13,23): R CDERIV(CIBEAM, jtcheckfreepool, 0,         VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,80): R CDERIV(CIBEAM, 0,            jtfindrange,  VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,81): R CDERIV(CIBEAM, 0,            jtfindrange4, VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,82): R CDERIV(CIBEAM, 0,            jtfindrange2, VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,83): R CDERIV(CIBEAM, jthdrinfo,     0,           VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(13,84): R CDERIV(CIBEAM, 0,            jtauditpyx,   VFLAGNONE,RMAX,RMAX,RMAX);
#if PYXES
  case XC(13,85): R SDERIV(CIBEAM, jtnulljob,    0,            VASGSAFE,RMAX,RMAX,RMAX);
#endif
  case XC(13,99): R CDERIV(CIBEAM, jtstackfault,            0, VFLAGNONE,RMAX,RMAX,RMAX);

  case XC(15,0):  R SDERIV(CIBEAM, 0,            jtcd,         VASGSAFE|VJTFLGOK2,RMAX,   1L,  1L  );
  case XC(15,1):  R SDERIV(CIBEAM, jtmemr,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,2):  R SDERIV(CIBEAM, 0,            jtmemw,       VASGSAFE,RMAX,   RMAX,RMAX);
  case XC(15,3):  R SDERIV(CIBEAM, jtmema,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,4):  R SDERIV(CIBEAM, jtmemf,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,5):  R SDERIV(CIBEAM, jtcdf,        0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,6):  R SDERIV(CIBEAM, jtdllsymget,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,7):  R SDERIV(CIBEAM, jtdllsymset,  0,            VASGSAFE,RMAX,RMAX,RMAX);   
  case XC(15,8):  R SDERIV(CIBEAM, jtgh15,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,9):  R SDERIV(CIBEAM, jtfh15,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,10): R SDERIV(CIBEAM, jtcder,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,11): R SDERIV(CIBEAM, jtcderx,      0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,12): R SDERIV(CIBEAM, jtdllsymhdr,  0,            VASGSAFE,RMAX,RMAX,RMAX);
#if 0
  case XC(15,12): R CDERIV(CIBEAM, jtsmmblks,    0,            VASGSAFE,RMAX,RMAX,RMAX);
#endif
  case XC(15,13): R CDERIV(CIBEAM, jtcallback,   0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,14): R SDERIV(CIBEAM, jtdllsymdat,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,15): R SDERIV(CIBEAM, jtmemu,       jtmemu2,      VASGSAFE|VJTFLGOK1,RMAX,0,   0   );
  case XC(15,16): R SDERIV(CIBEAM, jtnfes,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,17): R SDERIV(CIBEAM, jtcallbackx,  0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,19): R SDERIV(CIBEAM, jtdllvaladdr,  0,            VASGSAFE,RMAX,RMAX,RMAX);
#if 0
  case XC(15,19): R SDERIV(CIBEAM, jtcdjt,       0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,20): R SDERIV(CIBEAM, jtcdlibl,     0,            VASGSAFE,RMAX,RMAX,RMAX);
  case XC(15,21): R SDERIV(CIBEAM, jtcdproc1,    jtcdproc2,    VASGSAFE,RMAX,RMAX,RMAX);
#endif

  case XC(18,0):  R CDERIV(CIBEAM, jtlocnc,      0,            VFLAGNONE,0,   RMAX,RMAX);
  case XC(18,1):  R CDERIV(CIBEAM, jtlocnl1,     jtlocnl2,     VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,2):  R CDERIV(CIBEAM, jtlocpath1,   jtlocpath2,   VFLAGNONE,0,   1,   0   );
  case XC(18,3):  R CDERIV(CIBEAM, jtloccre1,    jtloccre2,    VFLAGNONE,RMAX,0,   RMAX);
  case XC(18,4):  R CDERIV(CIBEAM, jtlocswitch,  0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,5):  R CDERIV(CIBEAM, jtlocname,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,30): R CDERIV(CIBEAM, jtlocmap,     0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,31): R CDERIV(CIBEAM, jtsympool,    0,            VFLAGNONE,RMAX,RMAX,RMAX);
  case XC(18,55): R CDERIV(CIBEAM, jtlocexmark,  0,            VFLAGNONE,0,   RMAX,RMAX);
  
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
  case XC(128,9): R CDERIVF(CIBEAM, jtmvmsparse,         0,VF2WILLOPEN1,     VASGSAFE,RMAX,   RMAX,RMAX);
  case XC(128,10): R CDERIV(CIBEAM, jtludecomp,         0,     VASGSAFE,RMAX,   RMAX,RMAX);


//  default:        R foreignextra(a,w);
  default:        ASSERT(0,EVDOMAIN);  // any unknown combination is a domain error right away
}}

/* SY_64 double trick - null routines here to avoid optimization */
#if SY_64 & SY_WIN32
void double_trick(D a,D b,D c,D d){;}
#endif

#if SY_64 && (SY_LINUX || SY_MAC)
void double_trick(D a,D b,D c,D d,D e,D f,D g,D h){;}
#endif

#ifdef C_CD_ARMHF
void double_trick(float f0,float f1,float f2,float f3,float f4,float f5,float f6,float f7,float f8,float f9,float f10,float f11,float f12,float f13,float f14,float f15){;}
#endif
