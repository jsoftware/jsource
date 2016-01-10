/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos aka Foreign: External, Experimental, & Extra                      */

#include "j.h"
#include "x.h"

#define SDERIV(id,f1,f2,m,l,r)  \
 fdef(id,VERB,secf1,secf2,a,w,v2((I)(f1?f1:jtdomainerr1),(I)(f2?f2:jtdomainerr2)),0L,(I)m,(I)l,(I)r)

#define SDERI2(id,f1,f2,m,l,r)  \
 fdef(id,VERB,f1,   secf2,a,w,v2((I)(f1?f1:jtdomainerr1),(I)(f2?f2:jtdomainerr2)),0L,(I)m,(I)l,(I)r)


static DF1(secf1){A h=VAV(self)->h; ASSERT(!jt->seclev,EVSECURE); R CALL1((AF)*   AV(h) ,  w,self);}
static DF2(secf2){A h=VAV(self)->h; ASSERT(!jt->seclev,EVSECURE); R CALL2((AF)*(1+AV(h)),a,w,self);}

static DF1(jtfindrange){I base,n,top;
 RZ(w);
 n=AN(w);
 irange(n,AV(w),&base,&top);
 R v2(base,top);
}

F2(jtforeign){I p,q;
 RZ(a&&w);
 p=i0(a); q=i0(w); RE(0);
 if(11==p)R fdef(CIBEAM,VERB, jtwd,0L, a,w,0L, 0L, 1L,RMAX,RMAX);
 if(q<0||XCC<=q)R CDERIV(CIBEAM, 0,0,  RMAX,RMAX,RMAX);
 switch(XC(p,q)){
  case XC(0,  0): 
  case XC(0,100): R SDERI2(CIBEAM, jtscm00,      jtscm002,     RMAX,RMAX,RMAX);
  case XC(0,  1): 
  case XC(0,101): R SDERI2(CIBEAM, jtscm01,      jtscm012,     RMAX,RMAX,RMAX);
  case XC(0, 10): 
  case XC(0,110): R SDERI2(CIBEAM, jtscm10,      jtscm102,     RMAX,RMAX,RMAX);
  case XC(0, 11): 
  case XC(0,111): R SDERI2(CIBEAM, jtscm11,      jtscm112,     RMAX,RMAX,RMAX);
  case XC(0,2):   R SDERI2(CIBEAM, jtsct1,       jtsct2,       RMAX,RMAX,RMAX);
  case XC(0,3):   R SDERI2(CIBEAM, jtscz1,       jtscz2,       RMAX,RMAX,RMAX);

  case XC(1,0):   R SDERIV(CIBEAM, jtjdir,       0,            RMAX,0,   0   );
  case XC(1,1):   R SDERIV(CIBEAM, jtjfread,     0,            0,   0,   0   );
  case XC(1,2):   R SDERIV(CIBEAM, 0,            jtjfwrite,    0,   RMAX,0   );
  case XC(1,3):   R SDERIV(CIBEAM, 0,            jtjfappend,   0,   RMAX,0   );
  case XC(1,4):   R SDERIV(CIBEAM, jtjfsize,     0,            0,   0,   0   );
  case XC(1,5):   R SDERIV(CIBEAM, jtjmkdir,     0,            0,   0,   0   );
  case XC(1,6):   R SDERIV(CIBEAM, jtjfatt1,     jtjfatt2,     0,   1,   0   );
  case XC(1,7):   R SDERIV(CIBEAM, jtjfperm1,    jtjfperm2,    0,   1,   0   );

  case XC(1,11):  R SDERIV(CIBEAM, jtjiread,     0,            1,   0,   0   );
  case XC(1,12):  R SDERIV(CIBEAM, 0,            jtjiwrite,    0,   RMAX,1   );
  case XC(1,20):  R SDERIV(CIBEAM, jtjfiles,     0,            RMAX,0,   0   );
  case XC(1,21):  R SDERIV(CIBEAM, jtjopen,      0,            0,   0,   0   );
  case XC(1,22):  R SDERIV(CIBEAM, jtjclose,     0,            0,   0,   0   );

#if (SYS & SYS_DOS+SYS_MACINTOSH)
  case XC(1,30):  R SDERIV(CIBEAM, jtjlocks,     0,            RMAX,0,   0   );
  case XC(1,31):  R SDERIV(CIBEAM, jtjlock,      0,            1,   0,   0   );
  case XC(1,32):  R SDERIV(CIBEAM, jtjunlock,    0,            1,   0,   0   );
#endif
  
  case XC(1,43):  R CDERIV(CIBEAM, jtpathcwd,    0,            RMAX,0,   0   );
  case XC(1,44):  R CDERIV(CIBEAM, jtpathchdir,  0,            RMAX,0,   0   );
  case XC(1,46):  R CDERIV(CIBEAM, jtpathdll,    0,            RMAX,0,   0   );

  case XC(1,55):  R SDERIV(CIBEAM, jtjferase,    0,            0,   0,   0   );

  case XC(2,0):   R SDERIV(CIBEAM, jthost,       0,            1,   0,   0   );
  case XC(2,1):   R SDERIV(CIBEAM, jthostne,     0,            1,   0,   0   );
  case XC(2,2):   R SDERIV(CIBEAM, jthostio,     0,            1,   0,   0   );
  case XC(2,3):   R SDERIV(CIBEAM, jtjwait,      0,            0,   0,   0   );
  case XC(2,5):   R SDERIV(CIBEAM, jtjgetenv,    0,            1,   0,   0   );
  case XC(2,6):   R SDERIV(CIBEAM, jtjgetpid,    0,            1,   0,   0   );
  case XC(2,55):  R SDERIV(CIBEAM, jtjoff,       0,            RMAX,0,   0   );

  case XC(3,0):   R CDERIV(CIBEAM, jtstype,      0,            RMAX,0,   0  );
  case XC(3,1):   R CDERIV(CIBEAM, jtbinrep1,    jtbinrep2,    RMAX,RMAX,RMAX);
  case XC(3,2):   R CDERIV(CIBEAM, jtunbin,      0,            RMAX,0,   0   );
  case XC(3,3):   R CDERIV(CIBEAM, jthexrep1,    jthexrep2,    RMAX,RMAX,RMAX);
  case XC(3,4):   R CDERIV(CIBEAM, 0,            jtic2,        0,   RMAX,RMAX);
  case XC(3,5):   R CDERIV(CIBEAM, 0,            jtfc2,        0,   RMAX,RMAX);
  case XC(3,6):   R CDERIV(CIBEAM, jtlock1,      jtlock2,      RMAX,RMAX,RMAX);
  case XC(3,7):   R CDERIV(CIBEAM, jtbit1,       jtbit2,       RMAX,RMAX,RMAX);

  case XC(4,0):   R CDERIV(CIBEAM, jtnc,         0,            0,   0,   0   );
  case XC(4,1):   R CDERIV(CIBEAM, jtnl1,        jtnl2,        RMAX,RMAX,RMAX);
  case XC(4,3):   R CDERIV(CIBEAM, jtsnl,        0,            RMAX,0,   0   );
  case XC(4,4):   R CDERIV(CIBEAM, jtscind,      0,            0,   0,   0   );
  case XC(4,5):   R CDERIV(CIBEAM, jtnch,        0,            RMAX,0,   0   );
  case XC(4,55):  R CDERIV(CIBEAM, jtex,         0,            0,   0,   0   );

  case XC(5,0):   R fdef(CIBEAM,ADV, jtfxx,0L,  a,w,0L,  0L,   0L,  0L,  0L  );
  case XC(5,1):   R CDERIV(CIBEAM, jtarx,        0,            0,   0,   0   );
  case XC(5,2):   R CDERIV(CIBEAM, jtdrx,        0,            0,   0,   0   );
  case XC(5,4):   R CDERIV(CIBEAM, jttrx,        0,            0,   0,   0   );
  case XC(5,5):   R CDERIV(CIBEAM, jtlrx,        0,            0,   0,   0   );
  case XC(5,6):   R CDERIV(CIBEAM, jtprx,        0,            0,   0,   0   );
  case XC(5,7):   R CDERIV(CIBEAM, 0,            jtxrx,        0,   0,   0   );
  case XC(5,30):  R CDERIV(CIBEAM, 0,            jtoutstr,     0,   RMAX,RMAX);

  case XC(6,0):   R CDERIV(CIBEAM, jtts0,        0,            RMAX,0,   0   );
  case XC(6,1):   R CDERIV(CIBEAM, jttss,        0,            RMAX,0,   0   );
  case XC(6,2):   R CDERIV(CIBEAM, jttsit1,      jttsit2,      1,   0,   1   );
  case XC(6,3):   R CDERIV(CIBEAM, jtdl,         0,            0,   0,   0   );
  case XC(6,4):   R CDERIV(CIBEAM, jtparsercalls,0,            RMAX,0,   0   );
  case XC(6,8):   R CDERIV(CIBEAM, jtqpfreq,     0,            RMAX,0,   0   );
  case XC(6,9):   R CDERIV(CIBEAM, jtqpctr,      0,            RMAX,0,   0   );
  case XC(6,10):  R CDERIV(CIBEAM, jtpmarea1,    jtpmarea2,    RMAX,RMAX,RMAX);
  case XC(6,11):  R CDERIV(CIBEAM, jtpmunpack,   0,            RMAX,0,   0   );
  case XC(6,12):  R CDERIV(CIBEAM, jtpmctr,      0,            RMAX,0,   0   );
  case XC(6,13):  R CDERIV(CIBEAM, jtpmstats,    0,            RMAX,0,   0   );

  case XC(7,0):   R CDERIV(CIBEAM, jtsp,         0,            RMAX,0,   0   );
  case XC(7,2):   R CDERIV(CIBEAM, jtspit,       0,            1,   0,   0   );
  case XC(7,3):   R CDERIV(CIBEAM, jtspcount,    0,            RMAX,0,   0   );
  case XC(7,5):   R CDERIV(CIBEAM, jtspfor,      0,            0,   0,   0   );
  case XC(7,6):   R CDERIV(CIBEAM, jtspforloc,   0,            0,   0,   0   );

  case XC(8,0):   R CDERIV(CIBEAM, jtfmt01,      jtfmt02,      RMAX,RMAX,RMAX);
  case XC(8,1):   R CDERIV(CIBEAM, jtfmt11,      jtfmt12,      RMAX,RMAX,RMAX);
  case XC(8,2):   R CDERIV(CIBEAM, jtfmt21,      jtfmt22,      RMAX,RMAX,RMAX);

  case XC(9,0):   R CDERIV(CIBEAM, jtrngseedq,   0,            RMAX,0,   0   );
  case XC(9,1):   R CDERIV(CIBEAM, jtrngseeds,   0,            RMAX,0,   0   );
  case XC(9,2):   R CDERIV(CIBEAM, jtdispq,      0,            RMAX,0,   0   );
  case XC(9,3):   R CDERIV(CIBEAM, jtdisps,      0,            RMAX,0,   0   );
  case XC(9,6):   R CDERIV(CIBEAM, jtboxq,       0,            RMAX,0,   0   );
  case XC(9,7):   R CDERIV(CIBEAM, jtboxs,       0,            RMAX,0,   0   );
  case XC(9,8):   R CDERIV(CIBEAM, jtevmq,       0,            RMAX,0,   0   );
  case XC(9,9):   R CDERIV(CIBEAM, jtevms,       0,            RMAX,0,   0   );
  case XC(9,10):  R CDERIV(CIBEAM, jtppq,        0,            RMAX,0,   0   );
  case XC(9,11):  R CDERIV(CIBEAM, jtpps,        0,            RMAX,0,   0   );
  case XC(9,12):  R CDERIV(CIBEAM, jtsysq,       0,            RMAX,0,   0   );
  case XC(9,14):  R CDERIV(CIBEAM, jtversq,      0,            RMAX,0,   0   );
  case XC(9,16):  R CDERIV(CIBEAM, jtposq,       0,            RMAX,0,   0   );
  case XC(9,17):  R CDERIV(CIBEAM, jtposs,       0,            RMAX,0,   0   );
  case XC(9,18):  R CDERIV(CIBEAM, jtctq,        0,            RMAX,0,   0   );
  case XC(9,19):  R CDERIV(CIBEAM, jtcts,        0,            RMAX,0,   0   );
  case XC(9,20):  R CDERIV(CIBEAM, jtmmaxq,      0,            RMAX,0,   0   );
  case XC(9,21):  R CDERIV(CIBEAM, jtmmaxs,      0,            RMAX,0,   0   );
  case XC(9,24):  R CDERIV(CIBEAM, jtseclevq,    0,            RMAX,0,   0   );
  case XC(9,25):  R CDERIV(CIBEAM, jtseclevs,    0,            RMAX,0,   0   );
  case XC(9,26):  R CDERIV(CIBEAM, jtiepq,       0,            RMAX,0,   0   ); 
  case XC(9,27):  R CDERIV(CIBEAM, jtieps,       0,            RMAX,0,   0   ); 
  case XC(9,28):  R CDERIV(CIBEAM, jtiepdoq,     0,            RMAX,0,   0   ); 
  case XC(9,29):  R CDERIV(CIBEAM, jtiepdos,     0,            RMAX,0,   0   ); 
  case XC(9,32):  R CDERIV(CIBEAM, jttlimq,      0,            RMAX,0,   0   ); 
  case XC(9,33):  R CDERIV(CIBEAM, jttlims,      0,            RMAX,0,   0   ); 
  case XC(9,34):  R CDERIV(CIBEAM, jtassertq,    0,            RMAX,0,   0   ); 
  case XC(9,35):  R CDERIV(CIBEAM, jtasserts,    0,            RMAX,0,   0   );
  case XC(9,36):  R CDERIV(CIBEAM, jtoutparmq,   0,            RMAX,0,   0   );
  case XC(9,37):  R CDERIV(CIBEAM, jtoutparms,   0,            RMAX,0,   0   );
  case XC(9,38):  R CDERIV(CIBEAM, jtlocsizeq,   0,            RMAX,0,   0   );
  case XC(9,39):  R CDERIV(CIBEAM, jtlocsizes,   0,            RMAX,0,   0   );
  case XC(9,40):  R CDERIV(CIBEAM, jtretcommq,   0,            RMAX,0,   0   );
  case XC(9,41):  R CDERIV(CIBEAM, jtretcomms,   0,            RMAX,0,   0   );
  case XC(9,42):  R CDERIV(CIBEAM, jtrngselectq, 0,            RMAX,0,   0   );
  case XC(9,43):  R CDERIV(CIBEAM, jtrngselects, 0,            RMAX,0,   0   );
  case XC(9,44):  R CDERIV(CIBEAM, jtrngstateq,  0,            RMAX,0,   0   );
  case XC(9,45):  R CDERIV(CIBEAM, jtrngstates,  0,            RMAX,0,   0   );
  case XC(9,46):  R CDERIV(CIBEAM, jtbreakfnq,   0,            RMAX,0,   0   );
  case XC(9,47):  R CDERIV(CIBEAM, jtbreakfns,   0,            RMAX,0,   0   );
  case XC(9,48):  R CDERIV(CIBEAM, jtdotnamesq,  0,            RMAX,0,   0   );
  case XC(9,49):  R CDERIV(CIBEAM, jtdotnamess,  0,            RMAX,0,   0   );

/* case XC(11,*):      handled at beginning */
/* case XC(12,*):      reserved for D.H. Steinbrook tree stuff         */

  case XC(13,0):  R CDERIV(CIBEAM, jtdbc,        0,            RMAX,0,   0   );
  case XC(13,1):  R CDERIV(CIBEAM, jtdbstack,    0,            RMAX,0,   0   );
  case XC(13,2):  R CDERIV(CIBEAM, jtdbstopq,    0,            RMAX,0,   0   );
  case XC(13,3):  R CDERIV(CIBEAM, jtdbstops,    0,            RMAX,0,   0   );
  case XC(13,4):  R CDERIV(CIBEAM, jtdbrun,      0,            RMAX,0,   0   );
  case XC(13,5):  R CDERIV(CIBEAM, jtdbnext,     0,            RMAX,0,   0   );
  case XC(13,6):  R CDERIV(CIBEAM, jtdbret,      0,            RMAX,0,   0   );
  case XC(13,7):  R CDERIV(CIBEAM, jtdbjump,     0,            RMAX,0,   0   );
  case XC(13,8):  R CDERIV(CIBEAM, jtdbsig1,     jtdbsig2,     RMAX,RMAX,RMAX);
  case XC(13,9):  R CDERIV(CIBEAM, jtdbrr1,      jtdbrr2,      RMAX,RMAX,RMAX);
  case XC(13,11): R CDERIV(CIBEAM, jtdberr,      0,            RMAX,0,   0   );
  case XC(13,12): R CDERIV(CIBEAM, jtdbetx,      0,            RMAX,0,   0   );
  case XC(13,13): R CDERIV(CIBEAM, jtdbcall,     0,            RMAX,0,   0   );
  case XC(13,14): R CDERIV(CIBEAM, jtdbtrapq,    0,            RMAX,0,   0   );
  case XC(13,15): R CDERIV(CIBEAM, jtdbtraps,    0,            RMAX,0,   0   );
  case XC(13,17): R CDERIV(CIBEAM, jtdbq,        0,            RMAX,0,   0   );
  case XC(13,18): R CDERIV(CIBEAM, jtdbstackz,   0,            RMAX,0,   0   );
  case XC(13,19): R CDERIV(CIBEAM, jtdbcutback,  0,            RMAX,0,   0   );
  case XC(13,20): R CDERIV(CIBEAM, jtdbstepover1,jtdbstepover2,RMAX,RMAX,RMAX);
  case XC(13,21): R CDERIV(CIBEAM, jtdbstepinto1,jtdbstepinto2,RMAX,RMAX,RMAX);
  case XC(13,22): R CDERIV(CIBEAM, jtdbstepout1, jtdbstepout2, RMAX,RMAX,RMAX);

  case XC(13,80): R CDERIV(CIBEAM, jtfindrange,  0,            RMAX,0,   0   );

  case XC(15,0):  R SDERIV(CIBEAM, 0,            jtcd,         0,   1L,  1L  );
  case XC(15,1):  R SDERIV(CIBEAM, jtmemr,       0,            RMAX,0,   0   );
  case XC(15,2):  R SDERIV(CIBEAM, 0,            jtmemw,       0,   RMAX,RMAX);
  case XC(15,3):  R SDERIV(CIBEAM, jtmema,       0,            RMAX,0,   0   );
  case XC(15,4):  R SDERIV(CIBEAM, jtmemf,       0,            RMAX,0,   0   );
  case XC(15,5):  R SDERIV(CIBEAM, jtcdf,        0,            RMAX,0,   0   );
  case XC(15,6):  R SDERIV(CIBEAM, jtdllsymget,  0,            RMAX,0,   0   );
  case XC(15,7):  R SDERIV(CIBEAM, jtdllsymset,  0,            RMAX,0,   0   );   
  case XC(15,8):  R SDERIV(CIBEAM, jtgh15,       0,            RMAX,0,   0   );
  case XC(15,9):  R SDERIV(CIBEAM, jtfh15,       0,            RMAX,0,   0   );
  case XC(15,10): R SDERIV(CIBEAM, jtcder,       0,            RMAX,0,   0   );
  case XC(15,11): R SDERIV(CIBEAM, jtcderx,      0,            RMAX,0,   0   );
  case XC(15,12): R CDERIV(CIBEAM, jtsmmblks,    0,            RMAX,0,   0   );
  case XC(15,13): R CDERIV(CIBEAM, jtcallback,   0,            RMAX,0,   0   );
  case XC(15,14): R SDERIV(CIBEAM, jtdllsymdat,  0,            RMAX,0,   0   );
  case XC(15,16): R SDERIV(CIBEAM, jtnfes,       0,            RMAX,0,   0   );
  case XC(15,17): R SDERIV(CIBEAM, jtcallbackx,  0,            RMAX,0,   0   );
  case XC(15,18): R SDERIV(CIBEAM, jtnfeoutstr,  0,            RMAX,0,   0   );

  case XC(18,0):  R CDERIV(CIBEAM, jtlocnc,      0,            0,   0,   0   );
  case XC(18,1):  R CDERIV(CIBEAM, jtlocnl1,     jtlocnl2,     RMAX,RMAX,RMAX);
  case XC(18,2):  R CDERIV(CIBEAM, jtlocpath1,   jtlocpath2,   0,   1,   0   );
  case XC(18,3):  R CDERIV(CIBEAM, jtloccre1,    jtloccre2,    RMAX,0,   RMAX);
  case XC(18,4):  R CDERIV(CIBEAM, jtlocswitch,  0,            RMAX,0,   0   );
  case XC(18,5):  R CDERIV(CIBEAM, jtlocname,    0,            RMAX,0,   0   );
  case XC(18,30): R CDERIV(CIBEAM, jtlocmap,     0,            RMAX,0,   0   );
  case XC(18,31): R CDERIV(CIBEAM, jtsympool,    0,            RMAX,0,   0   );
  case XC(18,55): R CDERIV(CIBEAM, jtlocexmark,  0,            0,   0,   0   );
  
  case XC(128,0): R CDERIV(CIBEAM, jtqr,         0,            2,   0,   0   );
  case XC(128,1): R CDERIV(CIBEAM, jtrinv,       0,            2,   0,   0   );
  case XC(128,2): R CDERIV(CIBEAM, 0,            jtapplystr,   0,   1,   RMAX);
  case XC(128,3): R CDERIV(CIBEAM, jtcrc1,       jtcrc2,       RMAX,RMAX,RMAX);
  case XC(128,4): R CDERIV(CIBEAM, jtrngraw,     0,            RMAX,0,   0   );
  case XC(128,5): R CDERIV(CIBEAM, jtisnan,      0,            RMAX,0,   0   );

  default:        R foreignextra(a,w);
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
