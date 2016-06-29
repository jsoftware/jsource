/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Table of Primitive Symbols                                              */

#include "j.h"


C ctype[256]={
 0,  0,  0,  0,  0,  0,  0,  0,  0, CS,  0,  0,  0,  0,  0,  0, /* 0                  */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 1                  */
CS,  0,  0,  0,  0,  0,  0, CQ,  0,  0,  0,  0,  0,  0, CD,  0, /* 2  !"#$%&'()*+,-./ */
C9, C9, C9, C9, C9, C9, C9, C9, C9, C9, CC,  0,  0,  0,  0,  0, /* 3 0123456789:;<=>? */
 0, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, /* 4 @ABCDEFGHIJKLMNO */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA,  0,  0,  0,  0, C9, /* 5 PQRSTUVWXYZ[\]^_ */
 0, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, /* 6 `abcdefghijklmno */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA,  0,  0,  0,  0,  0, /* 7 pqrstuvwxyz{|}~  */
};
/*   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f   */

static B jtpdef(J jt,C id,I t,AF f1,AF f2,I m,I l,I r,I flag){A z;V*v;
 GA(z,t,1,0,0); ACX(z); v=VAV(z);
 v->f1=f1?f1:jtdomainerr1;  /* monad C function */
 v->f2=f2?f2:jtdomainerr2;  /* dyad  C function */
 v->mr=m;                   /* monadic rank     */
 v->lr=l;                   /* left    rank     */
 v->rr=r;                   /* right   rank     */
 v->fdep=1;                 /* function depth   */
 v->id=id;                  /* spelling         */
 v->flag=flag;              // flags
 pst[(UC)id]=z;             /* other fields are zeroed in ga() */
 R 1;
}

B jtpinit(J jt){A t;C*s;
 MC(wtype,ctype,256L); wtype['N']=CN; wtype['B']=CB;
 GA(alp,LIT,NALP,1,0); s=CAV(alp); DO(NALP,*s++=(C)i;); 
 /*  a. */  pst[(UC)CALP ]=t=alp;                 ACX(t);
 /*  a: */  pst[(UC)CACE ]=t=ace=sc4(BOX,(I)mtv); ACX(t);
 /*  (  */  pst[(UC)CLPAR]=t=sc4(LPAR,0L);        ACX(t);
 /*  )  */  pst[(UC)CRPAR]=t=sc4(RPAR,0L);        ACX(t);
 /*  =. */  GA(t,ASGN,1,0,0); ACX(t); *CAV(t)=CASGN;  pst[(UC)CASGN ]=t;
 /*  =: */  GA(t,ASGN,1,0,0); ACX(t); *CAV(t)=CGASGN; pst[(UC)CGASGN]=t;
                                                        
 /*  =  */  pdef(CEQ,     VERB, jtsclass,  jteq,     RMAX,0,   0   ,VINPLACEOK2);
 /*  =. */       /* see above */
 /*  =: */       /* see above */
 /*  <  */  pdef(CBOX,    VERB, jtbox,     jtlt,     RMAX,0,   0   ,VINPLACEOK2);
 /*  <. */  pdef(CFLOOR,  VERB, jtfloor1,  jtminimum,0,   0,   0   ,VINPLACEOK2);
 /*  <: */  pdef(CLE,     VERB, jtdecrem,  jtle,     0,   0,   0   ,VINPLACEOK1+VINPLACEOK2);
 /*  >  */  pdef(COPE,    VERB, jtope,     jtgt,     0,   0,   0   ,VINPLACEOK2);
 /*  >. */  pdef(CCEIL,   VERB, jtceil1,   jtmaximum,0,   0,   0   ,VINPLACEOK2);
 /*  >: */  pdef(CGE,     VERB, jtincrem,  jtge,     0,   0,   0   ,VINPLACEOK1+VINPLACEOK2);
 /*  +  */  pdef(CPLUS,   VERB, jtconjug,  jtplus,   0,   0,   0   ,VINPLACEOK2);
 /*  +. */  pdef(CPLUSDOT,VERB, jtrect,    jtgcd,    0,   0,   0   ,VINPLACEOK2);
 /*  +: */  pdef(CPLUSCO, VERB, jtduble,   jtnor,    0,   0,   0   ,VINPLACEOK1);
 /*  *  */  pdef(CSTAR,   VERB, jtsignum,  jttymes,  0,   0,   0   ,VINPLACEOK2);
 /*  *. */  pdef(CSTARDOT,VERB, jtpolar,   jtlcm,    0,   0,   0   ,VINPLACEOK2);
 /*  *: */  pdef(CSTARCO, VERB, jtsquare,  jtnand,   0,   0,   0   ,VINPLACEOK1+VINPLACEOK2);
 /*  -  */  pdef(CMINUS,  VERB, jtnegate,  jtminus,  0,   0,   0   ,VINPLACEOK1+VINPLACEOK2);
 /*  -. */  pdef(CNOT,    VERB, jtnot,     jtless,   0,   RMAX,RMAX,VINPLACEOK1);
 /*  -: */  pdef(CHALVE,  VERB, jthalve,   jtmatch,  0,   RMAX,RMAX,VINPLACEOK1);
 /*  %  */  pdef(CDIV,    VERB, jtrecip,   jtdivide, 0,   0,   0   ,VINPLACEOK1+VINPLACEOK2);
 /*  %. */  pdef(CDOMINO, VERB, jtminv,    jtmdiv,   2,   RMAX,2   ,0);
 /*  %: */  pdef(CSQRT,   VERB, jtsqroot,  jtroot,   0,   0,   0   ,0);
 /*  ^  */  pdef(CEXP,    VERB, jtexpn1,   jtexpn2,  0,   0,   0   ,0);
 /*  ^. */  pdef(CLOG,    VERB, jtlogar1,  jtlogar2, 0,   0,   0   ,0);
 /*  ^: */  pdef(CPOWOP,  CONJ, 0L,        jtpowop,  0,   0,   0   ,0);
 /*  $  */  pdef(CDOLLAR, VERB, jtshapex,  jtreitem, RMAX,1,   RMAX,0);
 /*  $. */  pdef(CSPARSE, VERB, jtsparse1, jtsparse2,RMAX,RMAX,RMAX,0);
 /*  $: */  pdef(CSELF,   VERB, jtself1,   jtself2,  RMAX,RMAX,RMAX,0);
 /*  ~  */  pdef(CTILDE,  ADV,  jtswap,    0L,       0,   0,   0   ,0);
 /*  ~. */  pdef(CNUB,    VERB, jtnub,     0L,       RMAX,0,   0   ,0);
 /*  ~: */  pdef(CNE,     VERB, jtnubsieve,jtne,     RMAX,0,   0   ,VINPLACEOK2);
 /*  |  */  pdef(CSTILE,  VERB, jtmag,     jtresidue,0,   0,   0   ,0);
 /*  |. */  pdef(CREV,    VERB, jtreverse, jtrotate, RMAX,1,   RMAX,0);
 /*  |: */  pdef(CCANT,   VERB, jtcant1,   jtcant2,  RMAX,1,   RMAX,0);
 /*  .  */  pdef(CDOT,    CONJ, 0L,        jtdot,    0,   0,   0   ,0);
 /*  .. */  pdef(CEVEN,   CONJ, 0L,        jteven,   0,   0,   0   ,0);
 /*  .: */  pdef(CODD,    CONJ, 0L,        jtodd,    0,   0,   0   ,0);
 /*  :  */  pdef(CCOLON,  CONJ, 0L,        jtcolon,  0,   0,   0   ,0);
 /*  :. */  pdef(COBVERSE,CONJ, 0L,        jtobverse,0,   0,   0   ,0);
 /*  :: */  pdef(CADVERSE,CONJ, 0L,        jtadverse,0,   0,   0   ,0);
 /*  ,  */  pdef(CCOMMA,  VERB, jtravel,   jtover,   RMAX,RMAX,RMAX,0);
 /*  ,* */  pdef(CAPIP,   VERB, 0L,        jtapip,   0,   RMAX,RMAX,0);  // f1 gets filled in with pointer to a name when this is used
 /*  ,. */  pdef(CCOMDOT, VERB, jttable,   jtstitch, RMAX,RMAX,RMAX,0);
 /*  ,: */  pdef(CLAMIN,  VERB, jtlamin1,  jtlamin2, RMAX,RMAX,RMAX,0);
 /*  ;  */  pdef(CSEMICO, VERB, jtraze,    jtlink,   RMAX,RMAX,RMAX,0);
 /*  ;. */  pdef(CCUT,    CONJ, 0L,        jtcut,    0,   0,   0   ,0);
 /*  ;: */  pdef(CWORDS,  VERB, jtwords,   jtfsm,    1,   RMAX,RMAX,0);
 /*  #  */  pdef(CPOUND,  VERB, jttally,   jtrepeat, RMAX,1,   RMAX,0);
 /*  #. */  pdef(CBASE,   VERB, jtbase1,   jtbase2,  1,   1,   1   ,0);
 /*  #: */  pdef(CABASE,  VERB, jtabase1,  jtabase2, RMAX,1,   0   ,0);
 /*  !  */  pdef(CBANG,   VERB, jtfact,    jtoutof,  0,   0,   0   ,0);
 /*  !. */  pdef(CFIT,    CONJ, 0L,        jtfit,    0,   0,   0   ,0);
 /*  !: */  pdef(CIBEAM,  CONJ, 0L,        jtforeign,0,   0,   0   ,0);
 /*  /  */  pdef(CSLASH,  ADV,  jtslash,   0L,       0,   0,   0   ,0);
 /*  /. */  pdef(CSLDOT,  ADV,  jtsldot,   0L,       0,   0,   0   ,0);
 /*  /: */  pdef(CGRADE,  VERB, jtgrade1,  jtgrade2, RMAX,RMAX,RMAX,0);
 /*  \  */  pdef(CBSLASH, ADV,  jtbslash,  0L,       0,   0,   0   ,0);
 /*  \. */  pdef(CBSDOT,  ADV,  jtbsdot,   0L,       0,   0,   0   ,0);
 /*  \: */  pdef(CDGRADE, VERB, jtdgrade1, jtdgrade2,RMAX,RMAX,RMAX,0);
 /*  [  */  pdef(CLEFT,   VERB, jtright1,  jtleft2,  RMAX,RMAX,RMAX,0);
 /*  [: */  pdef(CCAP,    VERB, 0L,        0L,       RMAX,RMAX,RMAX,0);
 /*  ]  */  pdef(CRIGHT,  VERB, jtright1,  jtright2, RMAX,RMAX,RMAX,0);
 /*  {  */  pdef(CLBRACE, VERB, jtcatalog, jtfrom,   1,   0,   RMAX,0);
 /*  {. */  pdef(CHEAD,   VERB, jthead,    jttake,   RMAX,1,   RMAX,0);
 /*  {: */  pdef(CTAIL,   VERB, jttail,    0L,       RMAX,0,   0   ,0);
 /*  }  */  pdef(CRBRACE, ADV,  jtrbrace,  0L,       0,   0,   0   ,0);
 /*  }* */  pdef(CAMIP,   ADV,  jtamip,    0L,       0,   0,   0   ,0);   // f2 gets filled in with pointer to a name when this is used
 /*  }* */  pdef(CCASEV,  VERB, jtcasev,   0L,       RMAX,RMAX,RMAX,0);   // f2 gets filled in with pointer to a name when this is used
 /*  }. */  pdef(CBEHEAD, VERB, jtbehead,  jtdrop,   RMAX,1,   RMAX,0);
 /*  }: */  pdef(CCTAIL,  VERB, jtcurtail, 0L,       RMAX,0,   0   ,0);
 /*  "  */  pdef(CQQ,     CONJ, 0L,        jtqq,     0,   0,   0   ,0);
 /*  ". */  pdef(CEXEC,   VERB, jtexec1,   jtexec2,  1,   RMAX,RMAX,0);
 /*  ": */  pdef(CTHORN,  VERB, jtthorn1,  jtthorn2, RMAX,1,   RMAX,0);
 /*  `  */  pdef(CGRAVE,  CONJ, 0L,        jttie,    0,   0,   0   ,0);
 /*  `. */       /* undefined */
 /*  `: */  pdef(CGRCO,   CONJ, 0L,        jtevger,  0,   0,   0   ,0);
 /*  @  */  pdef(CAT,     CONJ, 0L,        jtatop,   0,   0,   0   ,0);
 /*  @. */  pdef(CATDOT,  CONJ, 0L,        jtagenda, 0,   0,   0   ,0);
 /*  @: */  pdef(CATCO,   CONJ, 0L,        jtatco,   0,   0,   0   ,0);
 /*  &  */  pdef(CAMP,    CONJ, 0L,        jtamp,    0,   0,   0   ,0);
 /*  &. */  pdef(CUNDER,  CONJ, 0L,        jtunder,  0,   0,   0   ,0);
 /*  &: */  pdef(CAMPCO,  CONJ, 0L,        jtampco,  0,   0,   0   ,0);
 /*  ?  */  pdef(CQUERY,  VERB, jtroll,    jtdeal,   0,   0,   0   ,0);
 /*  ?. */  pdef(CQRYDOT, VERB, jtrollx,   jtdealx,  RMAX,0,   0   ,0);
 /*  ?: */       /* undefined */
 /* {:: */  pdef(CFETCH,  VERB, jtmap,     jtfetch,  RMAX,1,   RMAX,0);
 /* }:: */  pdef(CEMEND,  ADV,  jtemend,   0L,       0,   0,   0   ,0);
 /* &.: */  pdef(CUNDCO,  CONJ, 0L,        jtundco,  0,   0,   0   ,0);
 /*  a. */       /* see above */
 /*  a: */       /* see above */
 /*  A. */  pdef(CATOMIC, VERB, jtadot1,   jtadot2,  1,   0,   RMAX,0);
 /*  b. */  pdef(CBDOT,   ADV,  jtbdot,    0L,       0,   0,   0   ,0);
 /*  C. */  pdef(CCYCLE,  VERB, jtcdot1,   jtcdot2,  1,   1,   RMAX,0);
 /*  d. */  pdef(CDDOT,   CONJ, 0L,        jtddot,   0,   0,   0   ,0);
 /*  D. */  pdef(CDCAP,   CONJ, 0L,        jtdcap,   0,   0,   0   ,0);
 /*  D: */  pdef(CDCAPCO, CONJ, 0L,        jtdcapco, 0,   0,   0   ,0);
 /*  e. */  pdef(CEPS,    VERB, jtrazein,  jteps,    RMAX,RMAX,RMAX,0);
 /*  E. */  pdef(CEBAR,   VERB, 0L,        jtebar,   0,   RMAX,RMAX,0);
 /*  f. */  pdef(CFIX,    ADV,  jtfix,     0L,       0,   0,   0   ,0);
 /*  H. */  pdef(CHGEOM,  CONJ, 0L,        jthgeom,  0,   0,   0   ,0);
 /*  i. */  pdef(CIOTA,   VERB, jtiota,    jtindexof,1,   RMAX,RMAX,0);
 /*  i: */  pdef(CICO,    VERB, jtjico1,   jtjico2,  0,   RMAX,RMAX,0);
 /*  I. */  pdef(CICAP,   VERB, jticap,    jticap2,  1,   RMAX,RMAX,0);
 /*  j. */  pdef(CJDOT,   VERB, jtjdot1,   jtjdot2,  0,   0,   0   ,0);
 /*  L. */  pdef(CLDOT,   VERB, jtlevel1,  0L,       RMAX,0,   0   ,0);
 /*  L: */  pdef(CLCAPCO, CONJ, 0L,        jtlcapco, 0,   0,   0   ,0);
 /*  m. */       /* see above */
 /*  M. */  pdef(CMCAP,   ADV,  jtmemo,    0L,       0,   0,   0   ,0);
 /*  n. */       /* see above */
 /*  o. */  pdef(CCIRCLE, VERB, jtpix,     jtcircle, 0,   0,   0   ,0);
 /*  p. */  pdef(CPOLY,   VERB, jtpoly1,   jtpoly2,  1,   1,   0   ,0);
 /*  p..*/  pdef(CPDERIV, VERB, jtpderiv1, jtpderiv2,1,   0,   1   ,0);
 /*  p: */  pdef(CPCO,    VERB, jtprime,   jtpco2,   0,   RMAX,RMAX,0);
 /*  q: */  pdef(CQCO,    VERB, jtfactor,  jtqco2,   0,   0,   0   ,0);
 /*  r. */  pdef(CRDOT,   VERB, jtrdot1,   jtrdot2,  0,   0,   0   ,0);
 /*  s: */  pdef(CSCO,    VERB, jtsb1,     jtsb2,    RMAX,RMAX,RMAX,0);
 /*  S: */  pdef(CSCAPCO, CONJ, 0L,        jtscapco, 0,   0,   0   ,0);
 /*  t. */  pdef(CTDOT,   ADV,  jttdot,    0L,       0,   0,   0   ,0);
 /*  t: */  pdef(CTCO,    ADV,  jttco,     0L,       0,   0,   0   ,0);
 /*  T. */  pdef(CTCAP,   CONJ, 0L,        jttcap,   0,   0,   0   ,0);
 /*  u. */       /* see above */
 /*  u: */  pdef(CUCO,    VERB, jtuco1,    jtuco2,   RMAX,RMAX,RMAX,0);
 /*  v. */       /* see above */
 /*  x. */       /* see above */
 /*  x: */  pdef(CXCO,    VERB, jtxco1,    jtxco2,   RMAX,RMAX,RMAX,0);
 /*  y. */       /* see above */

 if(jt->jerr){printf("pinit failed; error %hhi\n", jt->jerr); R 0;} else R 1;
}
