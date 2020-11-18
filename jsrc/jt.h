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


typedef struct JSTstruct {
// The first 2 cache lines is the hottest real estate in J, because they can be referenced with
// $=must be a per-thread variable
// single-byte displacement.  Put your heaviest-used items here
 C*   adbreak;			/* must be first! ad mapped shared file break flag */
 C*   adbreakr;         // read location: same as adbreak, except that when we are ignoring interrupts it points to 0
// things needed by verb execution
 A*   tnextpushp;       // pointer to empty slot in allocated-block stack.  When low bits are 00..00, pointer to previous block of pointers.  Chain in first block is 0
 I    shapesink[2];     // garbage area used as load/store targets of operations we don't want to branch around
// things needed by name lookup (unquote)
 I    modifiercounter;  // incremented whenever anything happens that could alter modifier lookup: assignment/deletion of a modifier, or any change to locales or path
 UI4  ranks;            // low half: rank of w high half: rank of a  for IRS $
 union {
  UI4 ui4;    // all 4 flags at once, access as ui4
  struct {
   union {
    US cx_us;       // accessing both flags at once
    struct {
     C    glock;            /* 0=unlocked, 1=perm lock, 2=temp lock            */
     UC   db;               /* debug flag; see 13!:0                           */
    } cx_c;        // accessing as bytes
   } cx;   // flags needed by unquote and jtxdefn
   union {
    US uq_us;       // accessing both flags at once
    struct {
     C    pmctrbstk;  // pm and stacking info
#define PMCTRBPMON 1  // set if PM ctr is running
#define PMCTRBSTKREQD 2   // set if we MUST create a stack entry for each named call
     B    spfreeneeded;     // When set, we should perform a garbage-collection pass
    } uq_c;        // accessing as bytes
   } uq;   // flags needed only by unquote
  } us;   // access as US
 } uflags;   // $
 A    locsyms;  // local symbol table, or dummy empty symbol table if none  $
// ----- end of cache line 0
// things needed by parsing
 PFRAME parserstackframe;  // 4 words  $
 A    global;           /* global symbol table   $                        */
 A    sf;               /* for $:     $                                     */
 L    *assignsym;       // symbol-table entry for the symbol about to be assigned   $   scaf  need to use LX when muvltithreaded
 D    cct;               // complementary comparison tolerance $
// ----- end of cache line 1
 A    nvra;             // data blocks that are in execution somewhere - always non-virtual, always rank 1, AS[0] holds current pointer
 A    stloc;            /* locales symbol table                            */
 A    fill;             // fill     stuck here as filler
// things needed for memory allocation
 I    mfreegenallo;        // Amount allocated through malloc, biased
 void *heap;            // heap handle for large allocations
 I    mmax;             /* space allocation limit                          */
 struct {
  I ballo;              // negative number of bytes in free pool, but with zero-point biased so that - means needs garbage collection 
  A pool;             // pointer to first free block
 }    mfree[-PMINL+PLIML+1];      // pool info.  Use struct to keep cache footprint small
// --- end of cache line 3. 1 words carries over
// things needed by executing explicit defs
 A    curname;          // current name, an A block containing an NM   $
 UI   cstackmin;        // red warning for C stack pointer    $
 UI   qtstackinit;      // jqt front-end C stack pointer    $
 I4   callstacknext;           /* named fn calls: current depth      $             */
 I4   fcalln;           /* named fn calls: maximum permissible depth     $  */
 B    stch;             /* enable setting of changed bit       $            */
 UC   jerr;             /* error number (0 means no error)      $           */
 C    asgzomblevel;     // 0=do not assign zombie name before final assignment; 1=allow premature assignment of complete result; 2=allow premature assignment even of incomplete result   $
 B    assert;           /* 1 iff evaluate assert. statements               */
 B    foldrunning;      // 1 if fold is running (allows Z:)    $
 UC   jerr1;            /* last non-zero jerr    $                          */
 C    cxspecials;       // 1 if special testing needed in cx loop (pm or debug)    $
 B    iepdo;            /* 1 iff do iep    $                                */
 UC   prioritytype[11];  // type bit for the priority types
 UC   typepriority[19];  // priority value for the noun types
 B    sesm;             /* whether there is a session manager              */
 UC   dbuser;           /* user-entered value for db                       */
 A    flkd;             /* file lock data: number, index, length           */
 A    fopa;             /* open files boxed names                          */
 A    fopf;             /* open files corresp. file numbers                */
 I    getlasterror;     /* DLL stuff                                       */
// end cache line 5.
 void *dtoa;             /* use internally by dtoa.c           $             */
 I    bytes;            /* bytes currently in use                          */
 I    bytesmax;         /* high-water mark of "bytes"                      */
 I    mulofloloc;       // index of the result at which II multiply overflow occurred  scaf  $
 C    fillv0[sizeof(Z)];/* default fill value     $                         */
 C*   fillv;            /* fill value     $                                 */
 C    typesizes[32];    // the length of an allocated item of each type
// --- end cache line 6.  24 bytes carry over.  next cache line is junk; we don't expect to use these types much
 B    retcomm;          /* 1 iff retain comments and redundant spaces      */
 UC   seclev;           /* security level                                  */
 C    recurstate;       // state of recursions through JDo
#define RECSTATEIDLE    0  // JE is inactive, waiting for work
#define RECSTATEBUSY    1  // JE is running a call from JDo
#define RECSTATEPROMPT  2  // JE is running, and is suspended having called the host for input
#define RECSTATERECUR   3  // JE is running and waiting for a prompt, and the host has made a recursive call to JDo (which must not prompt)
// 2 bytes here
 B    directdef;         // 1 iff recognize DD scaf
 UC   disp[7];          /* # different verb displays                       */
 UC   outeol;           /* output: EOL sequence code                       */
// --- end cache line 7
 I    malloctotal;    // net total of malloc/free performed in m.c only
 I    malloctotalhwmk;  // highest value since most recent 7!:1
 void * iomalloc;   // address of block, if any, allocated in io.c to be returned to the FE
 I    iomalloclen;   // length of the allocated block (in case we can reuse it)
// --- end cache line 7/8
#if 0 // used only for direct locale numbering
 I*   numlocdelqh;      // head of deleted queue, waiting for realloc
 I    numlocdelqn;      // number of blocks on the deleted queue  could be UI4
 I*   numlocdelqt;       // tail of deleted queue
 I*   numloctbl;         // pointer to data area for locale-number to locale translation
 UI4  numlocsize;       // AN(jt->stnum)
#endif
 I    igemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for integer matrix product.  _1 means 'never'
 I    dgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for float matrix product.  _1 means 'never'
 I    zgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for complex matrix product.  _1 means 'never'
 A    implocref[2];     // references to 'u.'~ and 'v.'~, marked as implicit locatives scaf
 I4   parsercalls;      /* # times parser was called     $                  */
 I4   nthreads;  // number of threads to use, or 0 if we haven't checked     $
 A*   tstacknext;       // if not 0, points to the recently-used tstack buffer, whose chain field points to tstacknext   $
 A*   tstackcurr;       // current allocation, holding NTSTACK bytes+1 block for alignment.  First entry points to next-lower allocation   $
 D    cctdefault;        /* default complementary comparison tolerance set by user                    */
#if !(C_CRC32C && SY_64)
 UIL  ctmask;           /* 1 iff significant wrt ct; for i. and i:         */
#endif
 A    idothash0;        // 2-byte hash table for use by i.    $
 A    idothash1;        // 4-byte hash table for use by i.     $
 DC   sitop;            /* top of SI stack                                 */
 A    stnum;            // numbered locale numbers or hash table - rank 1, holding symtab pointer for each entry.  0 means empty
 C    baselocale[4];    // will be "base"
 UI4  baselocalehash;   // name hash for base locale
#if !USECSTACK
 I4   fdepi;            /* fn calls: current depth                         */
 I4   fdepn;            /* fn calls: maximum permissible depth             */
#else
 UI   cstackinit;       // C stack pointer at beginning of execution
#endif

// unordered symbols follow
#if !C_CRC32C
 I    hin;              /* used in dyad i. & i:                            */
 I*   hiv;              /* used in dyad i. & i:                            */
#endif
 A    symp;             /* symbol pool array                               */
 I*   breakfh;          /* win break file handle                           */
 I*   breakmh;          /* win break map handle                            */
 C    bx[11];               /* box drawing characters                          */
 A    cdarg;            /* table of 15!:0 parsed left arguments            */
 A    cdhash;           /* hash table of indices into cdarg                */
 A    cdhashl;          /* hash table of indices into cdarg                */
 A    cdstr;            /* strings for cdarg                               */

 A    dbalpha;          /* left  argument for rerun                        */
 I    dbjump;           /* line to jump to                                 */
 A    dbomega;          /* right argument for rerun                        */
 A    dbresult;         /* result to pop to the next level                 */
 C    dbss;             /* single step mode                                */
 DC   dbssd;            /* stack entry d corresp. to d->dcss setting       */
 A    dbssexec;         /* single step: execute string                     */
 C    dbsusact;         /* suspension action                               */

 A    dbstops;          /* stops set by the user                           */
 A    dbtrap;           /* trap, execute on suspension                     */
 DC   dcs;              /* ptr to debug stack entry for current script     */
 C*   capture;          /* capture output for python->J etc.               */
 I    dlllasterror;     /* DLL stuff                                       */
 S    etxn;             /* strlen(etx)                                     */
 S    etxn1;            /* last non-zero etxn                              */
 A    evm;              /* event messages                                  */
 A    iep;              /* immediate execution phrase                      */
 C    locsize[2];       /* size indices for named and numbered locales     */
#if !(C_CRC32C && SY_64)
 I    min;              /* the r result from irange                        */
#endif
 C    nfe;              /* 1 for J native front end                        */
 C    oleop;            /* com flag to capture output                      */
 void*opbstr;           /* com ptr to BSTR for captured output             */
 I4    outmaxafter;      /* output: maximum # lines after truncation        */
 I4    outmaxbefore;     /* output: maximum # lines before truncation       */
 I4    outmaxlen;        /* output: maximum line length before truncation   */
// obsolete  C    outseq[3];		    /* EOL: "LF" "CR" "CRLF"                           */
 I    peekdata;         /* our window into the interpreter                 */
 A    pma;              /* perf. monitor: data area                        */
 C    pos[2];           /* boxed output x-y positioning                    */
 C    pp[8];            // print precision (sprintf field for numeric output)
 A    p4792;            // pointer to p: i. 4792, filled in on first use
 I    redefined;        /* symbol table entry of redefined explicit defn  scaf */
 A    sbu;              /* SB data for each unique symbol                  */
 A    slist;            /* files used in right arg to 0!:                  */
 UC   cstacktype;  /* cstackmin set during 0: jt init  1: passed in JSM  2: set in JDo */
 I    sm;               /* sm options set by JSM()                         */
 void*smdowd;
 void*sminput;
 void*smoutput;         /* sm.. sm/wd callbacks set by JSM()               */
 void*smpoll;           /* re-used in wd                                   */
 UI   smoption;         /* wd options, see comment in jtwd                 */
 A    xep;              /* exit execution phrase                           */
 I    int64rflag;       /* com flag for returning 64-bit integers          */
 I    transposeflag;    /* com flag for transposed arrays                  */
 D    tssbase;          /* initial time of date                            */
 A    xmod;             /* extended integer: the m in m&|@f        $        */  
 C    xmode;            /* extended integer operating mode         $        */
#if MEMAUDIT & 2
 I    audittstackdisabled;   // set to 1 to disable auditing
#endif
 UF   rngF[5];          /* RNG: function to get the next random number     */
 UI   rngM[5];          /* RNG: moduli                                     */

 UI*  rngv;             /* RNG: rngV[rng]                                  */
 C    rng;              /* RNG: generator selector                         */
 UI*  rngfxsv;          /* RNG: rngv for fixed seed (?.)                   */
 S    rngI[5];          /* RNG: indices                                    */
 S    rngI0[5];         /* RNG: indices for RNG0                           */
 S    rngi;             /* RNG: current index                              */
 I    rngS[5];          /* RNG: seeds                                      */
 A    rngseed;          /* RNG: array seed                                 */
 UI*  rngV[5];          /* RNG: state vectors                              */
 UI*  rngV0[5];         /* RNG: state vectors for RNG0                     */
 C    rngw;             /* RNG: # bits in a random #                       */
 C    *etx;  // [1+NETX];      // display text for last error (+1 for trailing 0)  fits in main page
 LS   *callstack;   // [1+NFCALL]; // named fn calls: stack.  Usually only a little is used; the rest overflows onto a new DRAM page
 C    *breakfn;  // [NPATH];   /* break file name                                 */
} JST;
// obsolete  A*   nvrav;            /* AAV(jt->nvra)      $                             */
// obsolete  UI4  nvran;            // number of atoms in nvrav    $
// obsolete  B    asgn;             /* 1 iff last operation on this line is assignment  $  */
// obsolete  B    tostdout;         /* 1 if output to stdout       $                    */
// end cache line 4.  10 bytes carry over.  next cache line is junk; we don't expect to use these types much
// obsolete B    nflag;            /* 1 if space required before name               */
// obsolete  B    tmonad;           /* tacit translator: 1 iff monad         >          */
// obsolete  B    tsubst;           /* tacit translator           >                     */
// obsolete  I    flkn;             /* file lock count                                 */
// obsolete  I    fopn;             /* open files count                                */
// obsolete  D    fuzz;             /* fuzz (sometimes set to 0)  scaf  $                   */
// obsolete  B    prxthornuni;         /* 1 iff ": allowed to produce C2T result    >      */
// obsolete  B    jprx;             /* 1 iff ": for jprx (jconsole output)       >      */
// obsolete  C    unicodex78;       /* 1 iff disallow numeric argument for 7 8 u:  >  scaf  */
// 3 words free
// obsolete  I    filler[3];
// obsolete  I    stmax;            /* numbered locales maximum number                 */
// obsolete  A    stptr;            /* numbered locale symbol table ptrs               */
// obsolete  I    stused;           /* entries in stnum/stptr in use                   */
// obsolete  I    sttsize;          // length of hash table, =AN(jt->stnum)
// obsolete  L*   sympv;            /* symbol pool array value ptr, (L*)AV(jt->symp)   */
// obsolete  I    symindex;         /* symbol table index (monotonically increasing)   */
// obsolete  L*   cursymb;          /* current symbol table entry                      */
// obsolete  I    arg;              /* integer argument                                */
// obsolete  A    bxa;              /* array of box drawing characters                 */
// obsolete  I    cdna;             /* # of used entries in cdarg                      */
// obsolete  I    cdnl;             /* # of used entries in cdhashl                    */
// obsolete  I    cdns;             /* length of used portion of cdstr                 */
// obsolete  C*   dirbase;          /* for directory search                            */
// obsolete  C    diratts[7];       /* set by ismatch, read by dir1                    */
// obsolete  C    dirmode[11];      /* set by ismatch, read by dir1                    */
// obsolete  C    dirrwx[3];        /* set by ismatch, read by dir1                    */
// obsolete #if !SY_WINCE
// obsolete  struct stat dirstatbuf; //set by ismatch, read by dir1
// obsolete #if !SY_64 && (SYS & SYS_LINUX)
// obsolete  struct stat dummy1;    // stat above should be stat64
// obsolete  struct stat dummy2;    // reserve extra to avoid stomping disp
// obsolete #endif
// obsolete #endif 
// obsolete  I    fxi;              /* f. depth countdown                              */
// obsolete  A    fxpath;           /* f. path of names                                */
// obsolete  A*   fxpv;             /* f. AAV(fxpath)                                  */
// obsolete AF   lcp;              /* linear representation paren function            */
// obsolete  A    ltext;            /* linear representation text                      */
// obsolete AF   ltie;             /* linear representation tie   function            */
// obsolete  I    mtyo;				      /* jsto output type - jfwrite arg to jpr           */
// obsolete  C*   mtyostr;          /* jsto string                                     */
// obsolete  B    pmrec;            /* perf. monitor: 0 entry/exit; 1 all              */
// obsolete  I    pmctr;            /* perf. monitor: ctr>0 means do monitoring        */
// obsolete  PM0* pmu;              /* perf. monitor: (PM0)AV(pma)                     */
// obsolete  PM*  pmv;              /* perf. monitor: (PM*)(sizeof(PM0)+CAV(pma))      */
// obsolete  I    sbfillfactor;     /* ?SB for binary tree                              */
// obsolete  I    sbgap;            /* ?SB for binary tree                              */
// obsolete  A    sbh;              /* SB hash table of indices; -1 means unused       */
// obsolete  I*   sbhv;             /* *SB points to ravel of sbh                       */
// obsolete  I    sbroot;           /* ?SB root of the binary tree                      */
// obsolete  A    sbs;              /* *SB string                                       */
// obsolete  I    sbsn;             /* *SB string length so far                         */
// obsolete  C*   sbsv;             /* *SB points to ravel of sbs                       */
// obsolete I    sbun;             /* *SB cardinality                                  */
// obsolete  SBU* sbuv;             /* *SB points to ravel of sbu                       */
// obsolete  int  sdinited;         /* sockets                                         */
// obsolete  I4   slisti;           /* index into slist of current script     $         */
// obsolete  A    sclist;           /* slisti when items of slist were added           */
// obsolete  I    slistn;           /* slist # of real entries                         */
// obsolete  D    spfor;            /* semi-global for use by spfor()                  */
// obsolete  C*   th2buf;           /* space for formatting one number                 */
// obsolete  I    th2bufn;          /* current max length of buf                       */
// obsolete  UI   timelimit;        /* execution time limit milliseconds               */
// obsolete  TA*  ttab;             /* tacit translator                                */
// obsolete  I    ttabi;            /* tacit translator                                */
// obsolete  I    ttabi0;           /* tacit translator                                */
// obsolete  UF   rngf;             /* RNG: rngF[rng]                                  */
// obsolete // workareas for individual primitives, overlapping in the same memory
// obsolete union {
// obsolete struct{
// obsolete  D determ;  // determinant of the triangular matrix, if the matrix to be inverted was B01 or INT.  Set to 0 to suppress INT rounding
// obsolete } minv;
// obsolete  struct {
// obsolete   B    nla[256];         /* namelist names mask                             */
// obsolete   I    nlt;              /* namelist type  mask                             */
// obsolete  } namelist;
// obsolete struct {
// obsolete  I    postflags;  // what to do with the result
// obsolete } compsc;
// obsolete #if (SYS & SYS_UNIX)
// obsolete  C    dirnamebuf[NPATH];/* for directory search                            */
// obsolete #endif
// obsolete  struct {
// obsolete   CMP  comp;             /* comparison function in sort                     */
// obsolete   B    compusejt;        // set if the parameter to comparison function is jt rather than n
// obsolete   I    compk;            /* comparison: byte size of each item              */
// obsolete   I    complt;           /* comparison: denotes less    than                */
// obsolete   I    compn;            /* comparison: number of atoms in each item        */
// obsolete   C*   compsev;          /* comparison: sparse element value ptr            */
// obsolete   I    compsi;           /* comparison: sparse current cell index           */
// obsolete   I*   compstv;          /* comparison: sparse element item indices         */
// obsolete   I    compswf;          /* comparison: sparse wf value                     */
// obsolete   I    compsxc;          /* comparison: sparse aii(x)                       */
// obsolete   C*   compsxv;          /* comparison: sparse AV(x)                        */
// obsolete   I    compsyc;          /* comparison: sparse aii(y) or *(1+AS(y))         */
// obsolete   I*   compsyv;          /* comparison: sparse AV(y)                        */
// obsolete   C*   compv;            /* comparison: beginning of data area              */
// obsolete  } compare;
// obsolete } workareas;
// the offset at this point is about 0x14E8, so everything up to here will fit in a single 0x2000-byte DRAM page

typedef JST* J; 
