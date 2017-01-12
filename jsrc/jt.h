/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Definitions for jt ("jthis")                                            */

#if !SY_WINCE && (SYS & SYS_DOS)
#include <sys/stat.h>
#endif

#if  (SYS & SYS_UNIX)   /*   IVL   */
#include <sys/stat.h>
#endif

/*
All allocated and variable data for a J instance is accessed through
its JST structure.

Only simple constant data can be global and are shared by J instances.

Constants such as mtm are created in the dll process_attach. They are
the same for all instances and are never freed.  The dll is never
allowed to unload as we don't have code to free mtm et. al.

j.map must be checked to ensure no 'bad' global data exists. In
windows the global data that needs scrutiny is in the 0003: section.
*/


typedef struct {
// The first cache line is the hottest real estate in J, because it is brought in by every
// memory-allocation function.  Put your heaviest-used items here
 C*   adbreak;			/* must be first! ad mapped shared file break flag */
 B    breakignore;      /* 1 to ignore break (input_jfe_ output_jfe_       */
 B    spfreeneeded;     // When set, we should perform a garbage-collection pass
 B    asgn;             /* 1 iff last operation on this line is assignment */
 B    dotnames;         /* 1 iff x. y. etc. names are permitted            */
 C    dbss;             /* single step mode                                */
 B    stch;             /* enable setting of changed bit                   */
 C    jerr;             /* error number (0 means no error)                 */
 C    asgzomblevel;     // 0=do not assign zombie name before final assignment; 1=allow premature assignment of complete result; 2=allow premature assignment even of incomplete result
 struct {
  I ballo;              // negative number of bytes in free pool, but with zero-point biased so that - means needs garbage collection 
  MS *pool;             // pointer to first free block
 }    mfree[-PMINL+PLIML+1];      // pool info.  Use struct to keep cache footprint small
 I    mfreegenallo;        // Amount allocated through malloc, biased
 void*heap;             /* heap handle                                     */
 A    fill;             /* fill                                            */
 C*   fillv;            /* fill value                                      */
// --- end of cache linepair 0
// parser values
 I*   rank;             /* for integrated rank support                     */
 A    zombieval;        // value of assignsym, if it can be reused
 L    *assignsym;       // symbol-table entry for the symbol about to be assigned
 I    parsercalls;      /* # times parser was called                       */
 PSTK* parserstkbgn;     // &start of parser stack
 PSTK* parserstkend1;    // &end+1 of parser stack
 DC   sitop;            /* top of SI stack                                 */
 A    local;            /* local symbol table                              */
 A    global;           /* global symbol table                             */
 A    symb;             /* symbol table for assignment                     */
 I    symindex;         /* symbol table index (monotonically increasing)   */
 L*   sympv;            /* symbol pool array value ptr, (L*)AV(jt->symp)   */
 A    stloc;            /* locales symbol table                            */
 I    pmctr;            /* perf. monitor: ctr>0 means do monitoring        */
 I    db;               /* debug flag; see 13!:0                           */
 I    glock;            /* 0=unlocked, 1=perm lock, 2=temp lock            */
// --- end cache linepair 1
 I    stmax;            /* numbered locales maximum number                 */
 A    stnum;            /* numbered locale numbers                         */
 A    stptr;            /* numbered locale symbol table ptrs               */
 I    stused;           /* entries in stnum/stptr in use                   */
 B    stswitched;       /* called fn switched locale                       */
 B    thornuni;         /* 1 iff ": allowed to produce C2T result          */
 B    jprx;             /* 1 iff ": for jprx (jconsole output)             */
 I    tnextpushx;       // running byte index of next store into tstack.  Mask off upper bits to get offset into current frame
 A*   tstack;           // current frame, holding NTSTACK bytes.  First extry is to next-lower block
 A*   tstacknext;       // if not 0, points to the recently-used tstack buffer, whose chain field points to tstack
 A    symp;             /* symbol pool array                               */
 I    rela;             /* if a is relative, a itself; else 0              */
 I    relw;             /* if w is relative, w itself; else 0              */
 A*   nvrav;            /* AAV(jt->nvra)                                   */
 I    nvrtop;           /* top of nvr stack; # valid entries               */
 I    mulofloloc;       // index of the result at which II multiply overflow occurred
// --- end cache linepair 2

 I    arg;              /* integer argument                                */
 I*   breakfh;          /* win break file handle                           */
 C    breakfn[NPATH];   /* break file name                                 */
 I*   breakmh;          /* win break map handle                            */
 C*   bx;               /* box drawing characters                          */
 A    bxa;              /* array of box drawing characters                 */
 I    bytes;            /* bytes currently in use                          */
 I    bytesmax;         /* high-water mark of "bytes"                      */
 A    cdarg;            /* table of 15!:0 parsed left arguments            */
 A    cdhash;           /* hash table of indices into cdarg                */
 A    cdhashl;          /* hash table of indices into cdarg                */
 I    cdna;             /* # of used entries in cdarg                      */
 I    cdnl;             /* # of used entries in cdhashl                    */
 I    cdns;             /* length of used portion of cdstr                 */
 A    cdstr;            /* strings for cdarg                               */
 CMP  comp;             /* comparison function in sort                     */
 int  compgt;           /* comparison: denotes greater than                */
 I    compk;            /* comparison: byte size of each item              */
 int  complt;           /* comparison: denotes less    than                */
 I    compn;            /* comparison: number of atoms in each item        */
 C*   compsev;          /* comparison: sparse element value ptr            */
 I    compsi;           /* comparison: sparse current cell index           */
 I*   compstv;          /* comparison: sparse element item indices         */
 I    compswf;          /* comparison: sparse wf value                     */
 I    compsxc;          /* comparison: sparse aii(x)                       */
 C*   compsxv;          /* comparison: sparse AV(x)                        */
 I    compsyc;          /* comparison: sparse aii(y) or *(1+AS(y))         */
 I*   compsyv;          /* comparison: sparse AV(y)                        */
 C*   compv;            /* comparison: beginning of data area              */
 A    compw;            /* comparison: orig arg. (for relative addressing) */
 D    ct;               /* comparison tolerance                            */
 D    ctdefault;        /* default comparison tolerance                    */
 UI   ctmask;           /* 1 iff significant wrt ct; for i. and i:         */
 A    curlocn;          /* current locale name corresp. to curname         */
 A    curname;          /* current name                                    */
 L*   cursymb;          /* current symbol table entry                      */
 AD   cxqueuehdr;       // Area used by jtxdefn to point to sections of lines to pass to parsex
 A    dbalpha;          /* left  argument for rerun                        */
 I    dbjump;           /* line to jump to                                 */
 A    dbomega;          /* right argument for rerun                        */
 A    dbresult;         /* result to pop to the next level                 */
 DC   dbssd;            /* stack entry d corresp. to d->dcss setting       */
 A    dbssexec;         /* single step: execute string                     */
 A    dbstops;          /* stops set by the user                           */
 C    dbsusact;         /* suspension action                               */
 A    dbtrap;           /* trap, execute on suspension                     */
 I    dbuser;           /* user-entered value for db                       */
 DC   dcs;              /* ptr to debug stack entry for current script     */
 C*   dirbase;          /* for directory search                            */
 C    diratts[7];       /* set by ismatch, read by dir1                    */
 C    dirmode[11];      /* set by ismatch, read by dir1                    */
 C    dirnamebuf[NPATH];/* for directory search                            */
 C    dirrwx[3];        /* set by ismatch, read by dir1                    */
#if !SY_WINCE
 struct stat dirstatbuf; //set by ismatch, read by dir1
#if !SY_64 && (SYS & SYS_LINUX)
 struct stat dummy1;    // stat above should be stat64
 struct stat dummy2;    // reserve extra to avoid stomping disp
#endif
#endif 
 I    disp[7];          /* # different verb displays                       */
 I    dlllasterror;     /* DLL stuff                                       */
 void*dtoa;             /* use internally by dtoa.c                        */
 C    etx[1+NETX];      /* display text for last error (+1 for trailing 0) */
 I    etxn;             /* strlen(etx)                                     */
 I    etxn1;            /* last non-zero etxn                              */
 A    evm;              /* event messages                                  */
 I    fcalli;           /* named fn calls: current depth                   */
 I    fcalln;           /* named fn calls: maximum permissible depth       */
 LS   fcallg[1+NFCALL]; /* named fn calls: stack                           */
 I    fdepi;            /* fn calls: current depth                         */
 I    fdepn;            /* fn calls: maximum permissible depth             */
 C    fillv0[sizeof(Z)];/* default fill value                              */
 A    flkd;             /* file lock data: number, index, length           */
 I    flkn;             /* file lock count                                 */
 A    fopa;             /* open files boxed names                          */
 A    fopf;             /* open files corresp. file numbers                */
 I    fopn;             /* open files count                                */
 D    fuzz;             /* fuzz (sometimes set to 0)                       */
 I    fxi;              /* f. depth countdown                              */
 A    fxpath;           /* f. path of names                                */
 A*   fxpv;             /* f. AAV(fxpath)                                  */
 I    getlasterror;     /* DLL stuff                                       */
 I    hin;              /* used in dyad i. & i:                            */
 I*   hiv;              /* used in dyad i. & i:                            */
 A    iep;              /* immediate execution phrase                      */
 I    jerr1;            /* last non-zero jerr                              */
 AF   lcp;              /* linear representation paren function            */
 I    lleft;            /* positive finite left    level                   */
 I    lmon;             /* positive finite monadic level                   */
 I    locsize[2];       /* size indices for named and numbered locales     */
 I    lright;           /* positive finite right   level                   */
 A    ltext;            /* linear representation text                      */
 AF   ltie;             /* linear representation tie   function            */
 I    min;              /* the r result from irange                        */
 I    mmax;             /* space allocation limit                          */
 I    mtyo;				/* jsto output type - jfwrite arg to jpr           */
 C*   mtyostr;          /* jsto string                                     */
 I    nfe;              /* 1 for J native front end                        */
 B    nla[256];         /* namelist names mask                             */
 I    nlt;              /* namelist type  mask                             */
 A    nvra;             /* data blocks that are in execution somewhere     */
 I    oleop;            /* com flag to capture output                      */
 void*opbstr;           /* com ptr to BSTR for captured output             */
 I    outeol;           /* output: EOL sequence code                       */
 I    outmaxafter;      /* output: maximum # lines after truncation        */
 I    outmaxbefore;     /* output: maximum # lines before truncation       */
 I    outmaxlen;        /* output: maximum line length before truncation   */
 C    outseq[3];		/* EOL: "LF" "CR" "CRLF"                           */
 I    peekdata;         /* our window into the interpreter                 */
 A    pma;              /* perf. monitor: data area                        */
 PM0* pmu;              /* perf. monitor: (PM0)AV(pma)                     */
 PM*  pmv;              /* perf. monitor: (PM*)(sizeof(PM0)+CAV(pma))      */
 I    pos[2];           /* boxed output x-y positioning                    */
 C    pp[8];            /* print precision                                 */
 AF   pre;              /* preface function for assignment                 */
 I    redefined;        /* symbol table entry of redefined explicit defn   */
 int  reginitflag;      /* 1 iff regular expression stuff initialized      */
 I    rng;              /* RNG: generator selector                         */
 UF   rngF[5];          /* RNG: function to get the next random number     */
 UI*  rngfxsv;          /* RNG: rngv for fixed seed (?.)                   */
 UF   rngf;             /* RNG: rngF[rng]                                  */
 I    rngI[5];          /* RNG: indices                                    */
 I    rngI0[5];         /* RNG: indices for RNG0                           */
 I    rngi;             /* RNG: current index                              */
 UI   rngM[5];          /* RNG: moduli                                     */
 I    rngS[5];          /* RNG: seeds                                      */
 A    rngseed;          /* RNG: array seed                                 */
 UI*  rngV[5];          /* RNG: state vectors                              */
 UI*  rngV0[5];         /* RNG: state vectors for RNG0                     */
 UI*  rngv;             /* RNG: rngV[rng]                                  */
 I    rngw;             /* RNG: # bits in a random #                       */
 I    sbfillfactor;     /* SB for binary tree                              */
 I    sbgap;            /* SB for binary tree                              */
 A    sbh;              /* SB hash table of indices; -1 means unused       */
 I*   sbhv;             /* SB points to ravel of sbh                       */
 I    sbroot;           /* SB root of the binary tree                      */
 A    sbs;              /* SB string                                       */
 I    sbsn;             /* SB string length so far                         */
 C*   sbsv;             /* SB points to ravel of sbs                       */
 A    sbu;              /* SB data for each unique symbol                  */
 I    sbun;             /* SB cardinality                                  */
 SBU* sbuv;             /* SB points to ravel of sbu                       */
 A    sca;              /* S: result vector                                */
 I    scn;              /* S: actual length of sca                         */
 I*   scv;              /* S: AV(sca)                                      */
 int  sdinited;         /* sockets                                         */
 I    seclev;           /* security level                                  */
 A    sf;               /* for $:                                          */
 A    slist;            /* files used in right arg to 0!:                  */
 I    slisti;           /* index into slist of current script              */ 
 I    slistn;           /* slist # of real entries                         */
 I    sm;               /* sm options set by JSM()                         */
 void*smdowd;
 void*sminput;
 void*smoutput;         /* sm.. sm/wd callbacks set by JSM()               */
 void*smpoll;           /* re-used in wd                                   */
 I    smoption;         /* wd options, see comment in jtwd                 */
 D    spfor;            /* semi-global for use by spfor()                  */
 C*   th2buf;           /* space for formatting one number                 */
 I    th2bufn;          /* current max length of buf                       */
 UI   timelimit;        /* execution time limit milliseconds               */
 A    xep;              /* exit execution phrase                           */
 B    assert;           /* 1 iff evaluate assert. statements               */
 B    iepdo;            /* 1 iff do iep                                    */
 B    nflag;            /* 1 if space required before name                 */
 B    pmrec;            /* perf. monitor: 0 entry/exit; 1 all              */
 B    retcomm;          /* 1 iff retain comments and redundant spaces      */
 B    sesm;             /* whether there is a session manager              */
 B    tmonad;           /* tacit translator: 1 iff monad                   */
 B    tostdout;         /* 1 if output to stdout                           */
 B    tsubst;           /* tacit translator                                */
 B    xco;              /* 1 iff doing x: conversion                       */
 I    int64flag;        /* com flag for returning 64-bit integers          */
 I    transposeflag;    /* com flag for transposed arrays                  */
 D    tssbase;          /* initial time of date                            */
 TA*  ttab;             /* tacit translator                                */
 I    ttabi;            /* tacit translator                                */
 I    ttabi0;           /* tacit translator                                */
 A    xmod;             /* extended integer: the m in m&|@f                */  
 I    xmode;            /* extended integer operating mode                 */
} JST;

typedef JST* J; 
