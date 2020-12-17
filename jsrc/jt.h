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

typedef struct rngparms {
  UF   rngF;          /* RNG: function to get the next random number     */
  I    rngS;          /* RNG: seeds                                      */
  UI*  rngV;          /* RNG: state vectors                              */
  UI4  rngM;          /* RNG: moduli                                     */
  S    rngI;          /* RNG: indices                                    */
 } RNGPARMS;

// should align this to cacheline bdy
typedef struct rngdata {
 RNGPARMS rngparms[5];  // parms for RNGs when used signly
 UI*  rngv;             /* RNG: rngV[rng]                                  */
 UI*  rngfxsv;          /* RNG: rngv for fixed seed (?.)                   */
 A    rngseed;          /* RNG: array seed                                 */
 S    rngi;             // RNG: current index into state array
 C    rngw;             /* RNG: # bits in a random #                       */
 C    rng;              /* RNG: generator selector                         */
 RNGPARMS rngparms0[5];  // parms for RNG 0
 } RNG;



// per-thread area.  Align on a 256B boundary to leave low 8 bits for flags (JTFLAGMSK is the list of bits)
// The first 2 cache lines is the hottest real estate in J, because they can be referenced with
// single-byte displacement.  Put your heaviest-used items here
// Must be aligned on a 256-byte boundary for flags; but better to align on a DRAM page boundary to avoid precharge
 struct __attribute__((aligned(JTFLAGMSK+1))) JTTstruct {
// things needed for memory allocation
 A* tnextpushp;       // pointer to empty slot in allocated-block stack.  When low bits are 00..00, pointer to previous block of pointers.  Chain in first block is 0

// things needed by name lookup (unquote)
 LS *callstack;   // [1+NFCALL]; // named fn calls: stack.  Usually only a little is used; the rest overflows onto a new DRAM page
 A sf;               /* for $:                                          */
 I modifiercounter;  // incremented whenever anything happens that could alter modifier lookup: assignment/deletion of a modifier, or any change to locales or path
 UI cstackmin;        // red warning for C stack pointer    
 I4 callstacknext;    /* named fn calls: current depth                   */
 I4 fcalln;           /* named fn calls: maximum permissible depth   migrated    */
 UI4 ranks;            // low half: rank of w high half: rank of a  for IRS 
 union {  // this union is 4 bytes long
  UI4 ui4;    // all 4 flags at once, access as ui4
  struct {
   union {
    US cx_us;       // accessing both flags at once
    struct {
     C    pmctr;     // set when PM is running
     UC   db;               /* debug flag; see 13!:0                           */
    } cx_c;        // accessing as bytes
   } cx;   // flags needed by unquote and jtxdefn
   union {
    US uq_us;       // accessing both flags at once
    struct {
     C    bstkreqd;   // set if we MUST create a stack entry for each named call
// obsolete #define PMCTRBPMON 1  // set if PM ctr is running
// obsolete #define PMCTRBSTKREQD 2
     B    spfreeneeded;     // When set, we should perform a garbage-collection pass
    } uq_c;        // accessing as bytes
   } uq;   // flags needed only by unquote
  } us;   // access as US
 } uflags;   // 
 A curname;          // current name, an A block containing an NM   
// end of cacheline 0
 A locsyms;  // local symbol table, or dummy empty symbol table if none
 A global;           /* global symbol table                          */
 I shapesink[2];     // garbage area used as load/store targets of operations we don't want to branch around migrated
 A idothash0;        // 2-byte hash table for use by i.    migrated
 A idothash1;        // 4-byte hash table for use by i.    migrated
// cacheline 1 ends inside the pool struct
 struct {
  I ballo;              // negative number of bytes in free pool, but with zero-point biased so that - means needs garbage collection 
  A pool;             // pointer to first free block
 } mfree[-PMINL+PLIML+1];      // pool info.  Use struct to keep cache footprint small
// end of cacheline 2

// things needed by execution of certain verbs
 D cct;               // complementary comparison tolerance 
 A fill;             // fill     stuck here as filler
 C* fillv;            /* fill value                                      */
 C fillv0[sizeof(Z)];/* default fill value                              */
 B foldrunning;      // 1 if fold is running (allows Z:)    
 C glock;            /* 0=unlocked, 1=perm lock, 2=temp lock            */
 UC jerr;             /* error number (0 means no error)    migrated             */
 UC jerr1;            /* last non-zero jerr                 migrated             */
 I4 currslistx;    // index into slist of the current script being executed (or -1 if none)
 RNG *rngdata;    // separately allocated block for RNG
 I4 nthreads;  // number of threads to use, or 0 if we haven't checked     
 I4 parsercalls;      /* # times parser was called          migrated             */
// end of cacheline 3

// things needed by parsing
 PFRAME parserstackframe;  // 4 words  
 L *assignsym;       // symbol-table entry for the symbol about to be assigned      scaf  need to use LX when multithreaded
 A nvra;             // data blocks that are in execution somewhere - always non-virtual, always rank 1, AS[0] holds current pointer

// things needed for allocation of large blocks
 I mfreegenallo;        // Amount allocated through malloc, biased
 I malloctotal;    // net total of malloc/free performed in m.c only
// end of cacheline 4
 I malloctotalhwmk;  // highest value since most recent 7!:1

// things needed by unusual verbs
 void *dtoa;             /* use internally by dtoa.c                        */
 A xmod;             /* extended integer: the m in m&|@f                */  
 C pp[8];            // print precision (sprintf field for numeric output)
 I bytes;            // bytes currently in use - used only during 7!:1
 I bytesmax;         // high-water mark of "bytes" - used only during 7!:1
 S etxn;             /* strlen(etx)                        migrated             */
 S etxn1;            /* last non-zero etxn                 migrated             */
 C pos[2];           /* boxed output x-y positioning                    */
 B iepdo;            /* 1 iff do iep                       migrated            */
 C xmode;            /* extended integer operating mode                 */
 C recurstate;       // state of recursions through JDo migrated
#define RECSTATEIDLE    0  // JE is inactive, waiting for work
#define RECSTATEBUSY    1  // JE is running a call from JDo
#define RECSTATEPROMPT  2  // JE is running, and is suspended having called the host for input
#define RECSTATERECUR   3  // JE is running and waiting for a prompt, and the host has made a recursive call to JDo (which must not prompt)
#if !(C_CRC32C && SY_64)
 UIL  ctmask;           /* 1 iff significant wrt ct; for i. and i:         */
#endif

// seldom-used fields
#if !USECSTACK
 I4   fdepi;            /* fn calls: current depth                         */
 I4   fdepn;            /* fn calls: maximum permissible depth             */
#else
#endif
 A* tstacknext;       // if not 0, points to the recently-used tstack buffer, whose chain field points to tstacknext  
 // end of cacheline 5
 A* tstackcurr;       // current allocation, holding NTSTACK bytes+1 block for alignment.  First entry points to next-lower allocation   
 I getlasterror;     /* DLL stuff                                       */
 I dlllasterror;     /* DLL stuff                                       */
 C *etx;  // [1+NETX];      // display text for last error (+1 for trailing 0)

// debugging info
 DC sitop;            /* pointer to top of SI stack                                 */
 UI cstackinit;       // C stack pointer at beginning of execution

// unordered symbols follow
#if !C_CRC32C
 I    hin;              /* used in dyad i. & i:                            */
 I*   hiv;              /* used in dyad i. & i:                            */
#endif
#if !(C_CRC32C && SY_64)
 I    min;              /* the r result from irange                        */
#endif

// end of cacheline 6 (normalment)
};
typedef struct JTTstruct JTT;
typedef JTT* JJ;  // thread-specific part of struct

// Must be aligned on a 256-byte boundary for flags; but better to align on a DRAM page boundary to avoid precharge
typedef struct JSTstruct {
// shared area
 C* adbreak;			/* must be first! ad mapped shared file break flag */
 C* adbreakr;         // read location: same as adbreak, except that when we are ignoring interrupts it points to 0

// parsing, lookup, explicit definition execution
 A stloc;            /* locales symbol table                            */
 A symp;             /* symbol pool array                               */
 A slist;            /* files used in right arg to 0!:    */
 B assert;           /* 1 iff evaluate assert. statements               */
 B stch;             /* enable setting of changed bit                   */
 C asgzomblevel;     // 0=do not assign zombie name before final assignment; 1=allow premature assignment of complete result; 2=allow premature assignment even of incomplete result   
 UC dbuser;           /* user-entered value for db          migrated             */
#if MEMAUDIT & 2
 C audittstackdisabled;   // set to 1 to disable auditing
#endif
// 4 bytes free

// stuff used during verb execution
 void *heap;            // heap handle for large allocations
 I mmax;             /* space allocation limit                          */
// end of cache line 0
 A stnum;            // numbered locale numbers or hash table - rank 1, holding symtab pointer for each entry.  0 means empty
 I igemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for integer matrix product.  _1 means 'never'
 I dgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for float matrix product.  _1 means 'never'
 I zgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for complex matrix product.  _1 means 'never'
 A implocref[2];     // references to 'u.'~ and 'v.'~, marked as implicit locatives scaf
 C baselocale[4];    // will be "base"
 UI4 baselocalehash;   // name hash for base locale
 A sbu;              /* SB data for each unique symbol                  */
// end of cache line 1
 A p4792;            // pointer to p: i. 4792, filled in on first use

// DLL stuff
 A cdarg;            /* table of 15!:0 parsed left arguments            */
 A cdhash;           // hash table of cdstr strings into cdarg// end of cache line 0
 A cdhashl;          // hash table of cdstr strings into module index
 A cdstr;            /* strings for cdarg                               */

// file stuff
 A flkd;             /* file lock data: number, index, length           */
 A fopa;             /* open files boxed names                          */
 A fopf;             /* open files corresp. file numbers                */
// end of cache line 2

// little-used stuff
 D tssbase;          /* initial 6!:0''                            */
 A iep;              /* immediate execution phrase                      */
 A pma;              /* perf. monitor: data area                        */
 A evm;              /* event messages                                  */
 I4 outmaxafter;      /* output: maximum # lines after truncation        */
 I4 outmaxbefore;     /* output: maximum # lines before truncation       */
 I4 outmaxlen;        /* output: maximum line length before truncation   */
 B retcomm;          /* 1 iff retain comments and redundant spaces      */
 UC seclev;           /* security level                                  */
 C locsize[2];       /* size indices for named and numbered locales     */
 C bx[11];               /* box drawing characters                          */
 UC cstacktype;  /* cstackmin set during 0: jt init  1: passed in JSM  2: set in JDo   migrated*/
 B sesm;             /* whether there is a session manager      migrated        */
 C nfe;              /* 1 for J native front end            migrated            */
 C oleop;            /* com flag to capture output          migrated            */
 UC outeol;           /* output: EOL sequence code, 0, 1, or 2             */
// end of cache line 3
 UC disp[7];          /* # different verb displays                       */
// 1 byte free

// front-end interface info
 void *smdowd;         /* sm.. sm/wd callbacks set by JSM()               */
 void *sminput;
 void *smoutput;
 void *smpoll;           /* re-used in wd                                   */
 void *opbstr;           /* com ptr to BSTR for captured output             */
 C *breakfn;  // [NPATH];   /* break file name                                 */
// end of cache line 4
 C* capture;          /* capture output for python->J etc.               */
 UI smoption;         /* wd options, see comment in jtwd                 */
 I sm;               /* sm options set by JSM()                         */
 I int64rflag;       /* com flag for returning 64-bit integers          */
 I transposeflag;    /* com flag for transposed arrays                  */
 void *iomalloc;   // address of block, if any, allocated in io.c to be returned to the FE
 I iomalloclen;   // length of the allocated block (in case we can reuse it)
 UI qtstackinit;      // jqt front-end C stack pointer    
// end of cache line 5
 I* breakfh;          /* win break file handle                           */
 I* breakmh;          /* win break map handle                            */
 A xep;              /* exit execution phrase                           */

// debug info
 A dbstops;          /* stops set by the user                           */
 A dbtrap;           /* trap, execute on suspension                     */
 I peekdata;         /* our window into the interpreter                 */

 JTT threaddata[MAXTHREADS] __attribute__((aligned(JTFLAGMSK+1)));
} JST;
typedef JST* JS;  // shared part of struct

#if 0 // used only for direct locale numbering
 I*   numlocdelqh;      // head of deleted queue, waiting for realloc
 I    numlocdelqn;      // number of blocks on the deleted queue  could be UI4
 I*   numlocdelqt;       // tail of deleted queue
 I*   numloctbl;         // pointer to data area for locale-number to locale translation
 UI4  numlocsize;       // AN(JT(jt,stnum))
#endif
// obsolete C    typesizes[32];    // the length of an allocated item of each type
// obsolete  UC   prioritytype[11];  // type bit for the priority types
// obsolete  UC   typepriority[19];  // priority value for the noun types
// obsolete  C cxspecials;       // 1 if special testing needed in cx loop (pm or debug)    
// obsolete  I    mulofloloc;       // index of the result at which II multiply overflow occurred  scaf  $
// obsolete  D    cctdefault;        /* default complementary comparison tolerance set by user                    */
// obsolete  C    outseq[3];		    /* EOL: "LF" "CR" "CRLF"                           */
// obsolete I    redefined;        /* symbol table entry of redefined explicit defn  scaf */
// obsolete  DC   dcs;              /* ptr to debug stack entry for current script     */
// obsolete A     dbalpha;          /* left  argument for rerun                        */
// obsolete  I    dbjump;           /* line to jump to                                 */
// obsolete  A    dbomega;          /* right argument for rerun                        */
// obsolete  A    dbresult;         /* result to pop to the next level                 */
// obsolete  C    dbss;             /* single step mode                                */
// obsolete  DC   dbssd;            /* stack entry d corresp. to d->dcss setting       */
// obsolete  A    dbssexec;         /* single step: execute string                     */
// obsolete  C    dbsusact;         /* suspension action                               */
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
// obsolete  I    sttsize;          // length of hash table, =AN(JT(jt,stnum))
// obsolete  L*   sympv;            /* symbol pool array value ptr, (L*)AV(JT(jt,symp))   */
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
// obsolete // the offset at this point is about 0x14E8, so everything up to here will fit in a single 0x2000-byte DRAM page

#undef J
#define J JJ
#if MAXTHREADS>1  // for multithreading
#define JJTOJ(jj) ((JS)((I)(jj)&-JTALIGNBDY))
#else
// scaf testing only #define JJTOJ(jj) ((JS)((I)(jj)-offsetof(struct JSTstruct,threaddata)+(jj->ranks&0)+((((I)jj^0x200)&0x1e00)<<48)))
#define JJTOJ(jj) ((JS)((I)(jj)-offsetof(struct JSTstruct,threaddata)))
#endif
#define JT(p,n) JJTOJ(p)->n
#define INITJT(p,n) (p)->n   // in init functions, jjt points to the JS block and we use this to reference components
#define MTHREAD(jt) (&jt->threaddata[0])   // master thread for shared jt
#define LGTHREADBLKSIZE 9  // log2 of threaddata
#define THREADID(jt) (((jt)&(JTALIGNBDY-1)-offsetof(struct JSTstruct, threaddata[0]))>>LGTHREADBLKSIZE)  // thread number from jt
enum {xxxx = 1/(offsetof(struct JSTstruct, threaddata[MAXTHREADS])<=JTALIGNBDY) };  // assert not too many threads
enum {xxxxx = 1/(offsetof(struct JSTstruct, threaddata[1])-offsetof(struct JSTstruct, threaddata[0])==((I)1<<LGTHREADBLKSIZE)) };  // assert size of threaddata what we expected
