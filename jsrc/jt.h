/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Definitions for jt ("jthis")                                            */

#if !SY_WINCE && (SYS & SYS_DOS)
#include <sys/stat.h>
#endif

#if  (SYS & SYS_UNIX)   /*   IVL   */
#include <sys/stat.h>
#endif

#if PYXES
#include <pthread.h>
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
 } RNGPARMS;  // 32 bytes

// should align this to cacheline bdy
typedef struct rngdata {
 RNGPARMS rngparms[5];  // parms for RNGs when used singly
 UI*  rngv;             /* RNG: rngV[rng]                                  */
 UI*  rngfxsv;          /* RNG: rngv for fixed seed (?.)                   */
 A    rngseed;          /* RNG: array seed                                 */
 S    rngi;             // RNG: current index into state array
 C    rngw;             /* RNG: # bits in a random #                       */
 C    rng;              /* RNG: generator selector                         */
 RNGPARMS rngparms0[5];  // parms for RNG 0
 } RNG;  // 342 bytes

#if PYXES
typedef struct jobstruct JOB;
// job queue for each threadpool.  ht[0] is used as the job lock, so we align to cacheline boundary to avoid false sharing
typedef struct __attribute__((aligned(CACHELINESIZE))) {
 JOB *ht[2];  // queue head/tail.  When empty, ht[0] is 0 and ht[1] points to ht[1] as a harmless store location.  The job MUST be on a cacheline boundary, because LSBs are used as a lock.  Modified only when the job lock is held
 UI4 futex;  // futex used by all threads in this JOBQ
 UI4 nuunfin;   // Number of unfinished user jobs, queued and running.  Modified only when the job lock is held
 UI4 keepwarmns;  // time in ns to spin-poll this jobq before going into wait state
 US nthreads;  // number of threads in this pool
 US waiters;  // Number of waiting threads.  Modified only when job lock is held, and may be higher than the actual number of threads waiting
} JOBQ;
#endif

// per-thread area.  Align on a 256B boundary to leave low 8 bits for flags (JTFLAGMSK is the list of bits)
struct __attribute__((aligned(JTFLAGMSK+1))) JTTstruct {
 C _cl0[0];          // marker for the start of cacheline 0
// task-initialized values
// *********************************** the starting area contains values that are inherited from the spawning task en bloc.  Some of these are reinitialized
 A global;           // global symbol table inherit for task
 D cct;              // complementary comparison tolerance inherit for task  could be a float if non-complementary
 C boxpos;           // boxed output x-y positioning, low bits xxyy00 inherit for task
 C ppn;              // print precision (field width for numeric output) inherit for task
 C glock;            // 0=unlocked, 1=perm lock, 2=temp lock inherit for task  could merge into .db or boxpos
// 1 byte free
 union {  // this union is 4 bytes long on a 4-byte bdy
  UI4 ui4;    // all 4 flags at once, access as ui4
  struct {
   UC trace;  // tracing-related flags (debug and pm)  inherit
#define TRACEDB1                1  // full debug
#define TRACEPM                 2  // set when PM is running
#define TRACEDBSUSSS         0x20  // single-step request encountered - end suspension
#define TRACEDBSUSCLEAR      0x40  // set to forcibly end suspension
#define TRACEDBSUSFROMSCRIPT 0x80  // debug only, to keep reading from script during suspension
#define TRACEDB              0xfd  // any of the debug flags; see 13!:0
                                   // debug flags are also used for dbuser
   C init0area[0]; // label for initializing
                   // ************************************** here starts the area that is initialized to 0 when task starts 0x14
   C bstkreqd;   // set if we MUST create a stack entry for each named call
   union {
    US spflag; // access as short
    struct {
     B spfreeneeded;  // When set, we should perform a garbage-collection pass  persistent
     B sprepatneeded; // When low bit set, we should reclaim repat blocks.  Needs synchronisation, but rarely touched by other threads so this can stay in M/E
    };
   };
  };
 } uflags;   // 4 bytes
 I4 parsercalls;      // # times parser was called
 B iepdo;            // 1 iff do iep on going to immex     should be shared?
 C xmode;            // extended integer operating mode
 C emsgstate;   // disposition of error messages, which determines how detailed we make the message
#define EMSGSTATENOTEXT 1  // Set to suppress message text
#define EMSGSTATENOLINE 2  // Set to suppress line/col msgs
#define EMSGSTATENOEFORMAT 4  // Set to suppress call to eformat_j_ for detailed analysis
#define EMSGSTATEPARENX 3  // set if there are mismatched parens
#define EMSGSTATEPAREN (7<<EMSGSTATEPARENX)  // mask for the field
#define EMSGSTATEPARENTYPE (1<<EMSGSTATEPARENX)  // type of extra paren: 0=L 1=R
#define EMSGSTATEPARENPOSL (2<<EMSGSTATEPARENX)  // location of extra paren: to left or exactly at the error
#define EMSGSTATEPARENPOSR (4<<EMSGSTATEPARENX)  // location of extra paren: to right or exactly at the error
#define EMSGSTATEFORMATTED 0x80  // line has been through eformat - do not call again until errors reset
// 1 byte free
 I bytesmax;         // high-water mark of "bytes" - used only during 7!:1
 S etxn;             // strlen(etx) but set negative to freeze changes to the error line
 S etxn1;            // last non-zero etxn
 B foldrunning;      // 1 if fold is running (allows Z:)
 UC jerr;             // error number (0 means no error)
 UC jerr1;            // last non-zero jerr
 C namecaching;     // 1=for script 2=on
 A zombieval;    // the value that the verb result will be assigned to, if the assignment is safe and has inplaceable usecount and is not read-only
            // zombieval may have a stale address, if the name it came from was deleted after zombieval was set.  That's OK, because we use zombieval only to compare
            // against a named value that we have stacked; that value is guaranteed protected so zombieval cannot match it unless zombieval is valid.
 A xmod;             // extended integer: the m in m&|@f
// end of cacheline 0
 C _cl1[0];
// ************************************** here starts the part that is initialized to non0 values when the task is started.  Earlier values may also be initialized
 C initnon0area[0];
 A locsyms;  // local symbol table, or dummy empty symbol table if none init for task to emptylocale
 I4 currslistx;    // index into slist of the current script being executed (or -1 if none) init for task to -1  should be 2 bytes?
 US ranks;            // low half: rank of w high half: rank of a; for IRS. init for task to 3F3F
 C recurstate;       // state of recursions through JDo    init for task to BUSY
#define RECSTATEIDLE    0  // JE is inactive, waiting for work
#define RECSTATEBUSY    1  // JE is running a call from JDo
#define RECSTATEPROMPT  2  // JE is running, and is suspended having called the host for input
#define RECSTATERECUR   3  // JE is running and waiting for a prompt, and the host has made a recursive call to JDo (which must not prompt)
// **************************************  end of initialized part

 C persistarea[0];  // end of area set at task startup
// ************************************** everything after here persists over the life of the thread
 C fillv0len;   // length of fill installed in fillv0 (max 16)
 C taskstate;  // task state: modified by other tasks on a system lock or jbreak
#define TASKSTATERUNNINGX 0   // This thread has started running a task
#define TASKSTATERUNNING (1LL<<TASKSTATERUNNINGX)
#define TASKSTATELOCKACTIVEX 1  // thread has been notified that a systemlock has been called.  Transition of STATERUNNING is not allowed while LOCKACTIVE
#define TASKSTATELOCKACTIVE (1LL<<TASKSTATELOCKACTIVEX)
#define TASKSTATEACTIVEX 2  // thread is running on this JTT.  Changed under job lock.
#define TASKSTATEACTIVE (1LL<<TASKSTATEACTIVEX)
#define TASKSTATETERMINATEX 3  // thread has been signaled to terminate.  Changed under job lock.
#define TASKSTATETERMINATE (1LL<<TASKSTATETERMINATEX)
#define TASKSTATEFUTEXWAKEX 4  // wakeall is using jt->futexwt; don't delete it.  Accessed with RFO cycles
#define TASKSTATEFUTEXWAKE (1LL<<TASKSTATEFUTEXWAKEX)
 C threadpoolno;  // number of thread-pool this thread is in.  Filled in when thread created.
 C ndxinthreadpool;  // Sequential #in the threadpool of this thread.  Filled in when thread created
// 1 bytes free
 US symfreect[2];  // number of symbols in main and overflow local symbol free chains
 LX symfreehead[2];   // head of main and overflow symbol free chains
 UI cstackinit;       // C stack pointer at beginning of execution
 UI cstackmin;        // red warning for C stack pointer
 UI4*futexwt; // value this thread is currently waiting on, 0 if not waiting.  Used to wake sleeping threads during systemlock/jbreak.  In same cacheline as taskstate
 A filler1[1];
// end of cacheline 1

 C _cl2[0];
// things needed for memory allocation
 A*   tnextpushp;       // pointer to empty slot in allocated-block stack.  When low bits are 00..00, pointer to previous block of pointers.  Chain in first block is 0
 struct {
  I ballo;              // negative number of bytes in free pool, but with zero-point biased so that - means needs garbage collection 
  A pool;             // pointer to first free block
 } mfree[-PMINL+PLIML+1];      // pool info.  Use struct to keep cache footprint small
// cacheline 2 ends inside the pool struct (3 qwords extra)

// things needed by name lookup (unquote)
 LS *callstack;   // [1+NFCALL]; // named fn calls: stack.  Usually only a little is used
 A curname;          // current name, an A block containing an NM
 US fcalln;           // named fn calls: maximum permissible depth   could be a fixed value?
 US callstacknext;    // current stack pointer into callstack.  Could be elided if callstack put on a 16K boundary, not a bad idea anyway
 LX symfreetail1;  // tail pointer for local symbol overflow chain: symbols that have been returned but not yet given back to be shared by all threads
 DC sitop;            /* pointer to top of SI stack                                 */
 I bytes;            // bytes currently in use - used only during 7!:1
// end of cacheline 3

 C _cl4[0];
 I shapesink[SY_64?2:4];     // garbage area used as load/store targets of operations we don't want to branch around
// things needed for allocation of large blocks
 A* tstacknext;       // if not 0, points to the recently-used tstack allocation, whose first entry points to the current allocation  
 A* tstackcurr;       // current allocation, holding NTSTACK bytes+1 block for alignment.  First entry points to next-lower allocation   
 PFRAME parserstackframe;  // 4 words    sf field initialized at task-start
// end of cacheline 4

 C _cl5[0];
// things needed by execution of certain verbs
 A idothash0;        // 2-byte hash table for use by i.
 A idothash1;        // 4-byte hash table for use by i.
 A fill;             // user fill atom
 C* fillv;            // fill value, during primitive execution - used during parsing to hold pointer to routine to execute
 C fillv0[sizeof(Z)];/* default fill value                              */
 RNG *rngdata;    // separately allocated block for RNG
// seldom-used fields
 I malloctotalhwmk;  // highest value since most recent 7!:1
// end of cacheline 5

 C _cl6[0];
// seldom used,  but contended during system lock 
 C *etx;  // [1+NETX];      // display text for last error (+1 for trailing 0)
 void *dtoa;             /* use internally by dtoa.c                        */
 PSTK initparserstack[1];  // 2 words stack used for messages when we don't have a real one Only .a and .t are used, leaving 6 bytes free (.pt and .filler)
 I4 getlasterror;     // DLL error info from previous DLL call
 I4 dlllasterror;     // DLL domain error info (before DLL call)
 A repato; // outgoing repatriation chain; chain of objects which all belong to the same thread.  AAV0(repato) points to the last link in the chain, and AC(repato) is the cumulative #bytes in the chain
           // rationale: it's common to free many objects from the same thread at once (in particular, release boxed list from a pyx), so this amortises that work
           // it would be good to have a more general outgoing repatriation queue to handle better the case when you free objects from different threads; logic is more annoying there because you have to route the objects to their right destinations
           // snmalloc has a slick design but it sometimes 'repatriates' blocks to the wrong thread, so they may sometimes take multiple hops to get home, which is annoying.  An alternative is to use a fixed-sized array, and sort it once it fills up
           // perhaps something like an lru cache of threads recently freed to?  Do a linear scan of the first k entries (maybe w/short simd if the first is a miss), and if they all miss, then fall back to--snmalloc trick, or sort buffer, or something else
           // Or maybe a fixed-size cache, and anything that falls out of it gets immediately flushed?  I like that, because it helps prevent singleton allocations from getting lost
 I filler6[2];
// end of cacheline 6

 C _cl7[0];
 // Area used for intertask communication of memory allocation
 A repatq;  // queue of blocks allocated in this thread but freed by other threads.  Used as a lock, so put in its own cacheline.  Same format as repato above.  TODO would something with splay be more memory friendly than a straight chain?
 I mfreegenallo;        // Amount allocated through malloc, biased  modified only by owning thread
 I malloctotal;    // net total of malloc/free performed in m.c only  modified only by owning thread
 I filler7[5];
// end of cacheline 7
 C _cl8[0];

// stats I totalpops;
// stats I nonnullpops;
// the following lines are engaged only for low-performance builds, and must not be set in 64-bit builds lest blocks get too big
#if !SY_64
#if !C_CRC32C
 I    hin;              /* used in dyad i. & i:                            */
 I*   hiv;              /* used in dyad i. & i:                            */
#endif
#if !(C_CRC32C && SY_64)
 I    min;              /* the r result from irange                        */
#endif
#if !(C_CRC32C && SY_64)
 UIL  ctmask;           /* 1 iff significant wrt ct; for i. and i:         */
#endif
#if !USECSTACK
 I4   fdepi;            /* fn calls: current depth                         */
 I4   fdepn;            /* fn calls: maximum permissible depth             */
#else
#endif
#endif

};
typedef struct JTTstruct JTT;
typedef JTT* JJ;  // thread-specific part of struct
#define LGTHREADBLKSIZE 9  // log2 of threaddata

// Must be aligned on a 256-byte boundary for flags; but better to align on a DRAM page boundary to avoid precharge
typedef struct JSTstruct {
// shared area.  To implement critical regions we use locks in this struct.  To get best use of cache, we put the lock for
// a function in the same cacheline as the data used by the function.  To avoid false cacheline sharing, we have to make sure that only
// very-seldom-referenced data inhabits a cacheline that contains a lock.
// Cacheline 0 is special, because it contains adbreak, which is checked very frequently by all threads.  Therefore, to keep this cacheline
// in S state we must have everything else in the line be essentially read-only.
 C _cl0[0];
 C* adbreak;		// must be first! pointer to mapped shared file break flag.  Inits to jst->breakbytes; switched to file area if a breakfile is created
 C* adbreakr;         // read location: same as adbreak, except that when we are ignoring interrupts it points to a read-only byte of 0
 S systemlock;       // lock used for quiescing all tasks.  Bits in order of descending priority:    scaf use a flag in taskstate for check that we are in suspension
#define LOCKPRISYM 1  // lock is requested for symbol extension
#define LOCKPRIPATH 2  // lock is requested to change a locale path
#define LOCK78MEM 4  // lock is requested for 7!:8, total memory in use
#define LOCKPRIDEBUG 8  // lock is requested for debug suspension
#define LOCKALL (LOCKPRISYM+LOCKPRIPATH+LOCK78MEM+LOCKPRIDEBUG)  // for lockaccept, when we can accept anything
 S systemlocktct;   // counter field, used for systemlock sync, # running tasks
 S systemlockthreadct;  // for system lock, total # threads active
 US breakbytes;    // first byte: used for signals when there is no mapped breakfile.  Bit 0=ATTN request, bit 1=BREAK request.  Byte 1 used as error return value during systemlock
// obsolete  //void *heap;            // heap handle for large allocations
 I mmax;             // space allocation limit could be float or short float
 A stloc;            // named locales symbol table - this pointer never changes
 A *zpath;         // path 'z', used for all initial paths
 I filler0[2];
// end of cacheline 0

// Cacheline 1: DLL variables
 C _cl1[0];
 A cdarg;            /* table of 15!:0 parsed left arguments            */
 A cdhash;           // hash table of cdstr strings into cdarg
 A cdhashl;          // hash table of cdstr strings into module index
 A cdstr;            // strings for cdarg/cdhashl
 S cdlock;           // r/w lock for cdarg/cdhashl
 // rest of cacheline used only in exceptional paths
// 6 bytes free
#if MEMAUDIT & 2
 C audittstackdisabled;   // set to 1 to disable auditing
#endif
 I* breakfh;          /* win break file handle                           */
 I* breakmh;          /* win break map handle                            */
 C *breakfn;  // [NPATH];   /* break file name                                 */
// end of cacheline 1

// Cacheline 2: J symbol pool
 C _cl2[0];
 L *sympv;           // symbol pool array.  This is offset LAV0 into the allocated block.  Symbol 0 is used as the root of the free chain
 S symlock;          // r/w lock for symbol pool
 B stch;             // enable setting of changed bit during assignment
 C asgzomblevel;     // 0=do not assign zombie name before final assignment; 1=allow premature assignment of complete result; 2=allow premature assignment even of incomplete result  scaf remove?
 // rest of cacheline used only in exceptional paths
 S locdellock;  // lock to serialize user request to delete locale
 US promptthread;  // The thread that is allowed to prompt from keyboard.  0=master normally, but set to debug thread during suspension.  Host sentences are sent to this thread
// front-end interface info
 C *capture;          // capture output for python->J etc.
 void *smdowd;         /* sm.. sm/wd callbacks set by JSM()               */
 void *sminput;
 void *smoutput;
 A implocref[2];     // references to 'u.'~ and 'v.'~, marked as implicit locatives.
// end of cacheline 2

// Cacheline 3: Locales
 C _cl3[0];
 A stnum;            // numbered locale numbers or hash table - rank 1, holding symtab pointer for each entry.  0 means empty
 S stlock;           // r/w lock for stnum.  stloc is never modified, so we use the ->lock field of stloc to lock that table
 C locsize[2];       /* size indices for named and numbered locales     */
 C baselocale[4];    // will be "base"
 UI4 baselocalehash;   // name hash for base locale
 UC seclev;           /* security level                                  */
 UC dbuser;           // user-entered value for db, 0 or 1 if bit 7 set, take debug continuation from script.  See TRACEDB* flags above
 B assert;           // 1 iff evaluate assert. statements     
// 1 byte free
 // rest of cacheline used only in exceptional paths
// obsolete  UC wakeallct;  // number of calls to wakeall in process (can't be more than 2)
 void *smpoll;           /* re-used in wd                                   */
 void *opbstr;           /* com ptr to BSTR for captured output             */
 I filler3[3];
// end of cacheline 3

// Cacheline 4: Files
 C _cl4[0];
 A flkd;             /* file lock data: number, index, length           */
 A fopafl;         // table of open filenames; in each one AM is the file handle and the lock is used
 S flock;            // r/w lock for flkd/fopa/fopf, also used for thread creation/deletion
 // rest of cacheline used only in exceptional paths
 US nwthreads;    // number of worker threads allocated so far - changes protected by flock
 UC sm;               /* sm options set by JSM()                         */
 C smoption;         // wd options, see comment in jtwd
 UC int64rflag;       /* com flag for returning 64-bit integers          */
 UC transposeflag;    /* com flag for transposed arrays                  */
 void *iomalloc;   // address of block, if any, allocated in io.c to be returned to the FE
 I iomalloclen;   // length of the allocated block (in case we can reuse it)
 UI qtstackinit;      // jqt front-end C stack pointer
 A p4792;            // pointer to p: i. 4792, filled in on first use
 D tssbase;          /* initial 6!:0''                            */
// end of cacheline 4


// Cacheline 5: User symbols, also used for front-end locks
 C _cl5[0];
 A sbu;              /* SB data for each unique symbol                  */
 A sbhash;              // hashtable for symbols
 A sbstrings;          // string data for symbols
 S sblock;           // r/w lock for sbu
 S felock;           // r/w lock for host functions, accessed only at start/end of immex
 // rest of cacheline used only in exceptional paths
 I4 outmaxafter;      // output: maximum # lines after truncation    could be short float   */
 I4 outmaxbefore;     // output: maximum # lines before truncation     could be short float   */
 I4 outmaxlen;        // output: maximum line length before truncation  could be short float  */
 I peekdata;         /* our window into the interpreter                 */
 A iep;              /* immediate execution phrase                      */
 A pma;              /* perf. monitor: data area                        */
// end of cacheline 5

// Cacheline 6: debug, which is written so seldom that it can have read-only data
 C _cl6[0];
 A dbstops;          /* stops set by the user                           */
 A dbtrap;           // trap sentence, execute when going into suspension
 S dblock;           // lock on dbstops/dbtrap
 B retcomm;          /* 1 iff retain comments and redundant spaces      */
 UC outeol;           /* output: EOL sequence code, 0, 1, or 2             */
 B sesm;             /* whether there is a session manager             */
 C nfe;              /* 1 for J native front end                    */
 // rest of cacheline is essentially read-only
 FLOAT16 igemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for integer matrix product.  _1 means 'never'   scaf could be shorter
 FLOAT16 dgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for float matrix product.  _1 means 'never'
 FLOAT16 zgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for complex matrix product.  _1 means 'never'
// 4 bytes free 
 A evm;              // message text for the EVxxx codes
 I (*emptylocale)[MAXTHREADS][16];      // locale with no symbols, used when not running explicits, or to avoid searching the local syms.  Aligned on odd word boundary, must never be freed.  One per task, because they are modified
 I filler6[2];
// end of cacheline 6

// Cacheline 7: startup (scripts and deprecmsgs), essentially read-only
 C _cl7[0];
 A slist;            // boxed list of filenames used in right arg to 0!:, the entries made in sn field of L blocks are indexes into this.  AM has # valid entries
 A deprecex;  // list of INTs of messages not to display
 I4 deprecct;  // number of deprecation  errors to display, -1 to emsg
 S startlock;        // lock for slist
 // rest of cacheline used only in exceptional paths
 C bx[11];               /* box drawing characters                          */
 UC disp[7];          /* # different verb displays                       */
 US cachesizes[3];  // [0]: size of fastest cache  [1]: size of largest cache private to each core  [2]: size of largest cache shared by all cores, in multiples of 4KB
 C oleop;            /* com flag to capture output                    */
 UC cstacktype;  /* cstackmin set during 0: jt init  1: passed in JSM  2: set in JDo  */
#if PYXES
 JOBQ (*jobqueue)[MAXTHREADPOOLS];     // one JOBQ block for each threadpool
 I filler7[1];
#else
 I filler7[2];
#endif
// end of cacheline 7
 C _cl8[0];

 JTT threaddata[MAXTHREADS] __attribute__((aligned(JTFLAGMSK+1)));
} JST;   // __attribute__((aligned(JTALIGNBDY))) not allowed on windows
typedef JST* JS;  // shared part of struct


#if 0 // used only for direct locale numbering
 I*   numlocdelqh;      // head of deleted queue, waiting for realloc
 I    numlocdelqn;      // number of blocks on the deleted queue  could be UI4
 I*   numlocdelqt;       // tail of deleted queue
 I*   numloctbl;         // pointer to data area for locale-number to locale translation
 UI4  numlocsize;       // AN(JT(jt,stnum))
#endif

#undef J
#define J JJ
#define JorJJTOJ(jj)  ((JS)((I)(jj)&-JTALIGNBDY))
#if MAXTHREADS>1  // for multithreading
#define JJTOJ(jj) JorJJTOJ(jj)
#else
#define JJTOJ(jj) ((JS)((I)(jj)-offsetof(struct JSTstruct,threaddata)))
#endif
#define JT(p,n) (JJTOJ(p)->n)
#define INITJT(p,n) (p)->n   // in init functions, jjt points to the JS block and we use this to reference components
#define MTHREAD(jjt) (&jjt->threaddata[0])   // jt for master thread.  jjt is the shared jt pointer
#define MDTHREAD(jjt) (&jjt->threaddata[jjt->promptthread])     // jt for master/debug thread.  jjt is the shared jt pointer
#define THREADID(jt) ((((I)(jt)&(JTALIGNBDY-1))>>LGTHREADBLKSIZE)-(offsetof(struct JSTstruct, threaddata[0])>>LGTHREADBLKSIZE))  // thread number from jt.  Thread 0 is the master
#define JTTHREAD0(jt) (JJTOJ(jt)->threaddata)   // the array of JTT structs
#define JTFORTHREAD(jt,n) (&(JTTHREAD0(jt)[n]))   // JTT struct for thread n
#define NALLTHREADS(jt) (1+JT(jt,nwthreads))
#define THREADIDFORWORKER(n) ((n)+1)  // convert worker# to thread#
// given a pointer which might be a JST* or JTT*, set pointers to use for the shared and thread regions.
// If we were given JST*, keep it as shared & use master thread; if JTT*, keep it as thread & use shared region
#define SETJTJM(injstout,jttout) \
 JJ jttout; \
 if((I)injstout&((JTALIGNBDY-1)&~JTFLAGMSK)){jttout=(JJ)injstout; injstout=JJTOJ(injstout);   /* if jt is a thread pointer, keep it as jm and set jt to the shared */ \
 }else{jttout=MDTHREAD(injstout);}  // if jt is a shared pointer, keep it as jt and use the master/debug thread as jm

_Static_assert(sizeof(struct JSTstruct)<=JTALIGNBDY,"too many threads");  // assert not too many threads
_Static_assert(offsetof(struct JSTstruct, threaddata[1])-offsetof(struct JSTstruct, threaddata[0])==((I)1<<LGTHREADBLKSIZE),"threaddata size");  // assert size of threaddata what we expected

#if SY_64
_Static_assert(offsetof(JTT,_cl0)==0*64,"cacheline 0 offset wrong");
_Static_assert(offsetof(JTT,_cl1)==1*64,"cacheline 1 offset wrong");
_Static_assert(offsetof(JTT,_cl2)==2*64,"cacheline 2 offset wrong");
//_Static_assert(offsetof(JTT,_cl3)==3*64); //cacheline 3 ends in the middle of an object
_Static_assert(offsetof(JTT,_cl4)==4*64,"cacheline 4 offset wrong");
_Static_assert(offsetof(JTT,_cl5)==5*64,"cacheline 5 offset wrong");
_Static_assert(offsetof(JTT,_cl6)==6*64,"cacheline 6 offset wrong");
_Static_assert(offsetof(JTT,_cl7)==7*64,"cacheline 7 offset wrong");
_Static_assert(offsetof(JTT,_cl8)==8*64,"cacheline 8 offset wrong");
_Static_assert(offsetof(JST,_cl0)==0*64,"cacheline 0 offset wrong");
_Static_assert(offsetof(JST,_cl1)==1*64,"cacheline 1 offset wrong");
_Static_assert(offsetof(JST,_cl2)==2*64,"cacheline 2 offset wrong");
_Static_assert(offsetof(JST,_cl3)==3*64,"cacheline 3 offset wrong");
_Static_assert(offsetof(JST,_cl4)==4*64,"cacheline 4 offset wrong");
_Static_assert(offsetof(JST,_cl5)==5*64,"cacheline 5 offset wrong");
_Static_assert(offsetof(JST,_cl6)==6*64,"cacheline 6 offset wrong");
_Static_assert(offsetof(JST,_cl7)==7*64,"cacheline 7 offset wrong");
_Static_assert(offsetof(JST,_cl8)==8*64,"cacheline 8 offset wrong");
#endif
