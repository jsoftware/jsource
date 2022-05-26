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
#if !defined(_WIN32) && !defined(__linux__)
extern int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex, const struct timespec *restrict abs_timeout);
#endif
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
typedef struct {
 JOB *ht[2];  // queue head/tail.  When empty, ht[0] is 0 and ht[1] points to ht[1].  The job MUST be on a cacheline boundary, because LSBs are used as a lock.  Modified only when the job lock is held
 UI4 nuunfin;   // Number of unfinished user jobs, queued and running.  Modified only when the job lock is held
 US waiters;  // Number of waiting threads.  Modified only when mutex is held
// 2 bytes free
 pthread_mutex_t mutex; // no spinlock; glibc and apparently also msvc mutex is reasonably sophisticated and we have to hold the
// on UNIX, first cacheline ends here.  On windows this is still in the first cacheline
 pthread_cond_t cond;   // hold a lock after releasing a condition variable anyway.  Investigate more sophisticated schemes later
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
   union {
    US cx_us;       // accessing both flags at once
    struct {
     C    pmctr;     // set when PM is running inherit
     UC   db;               /* debug flag; see 13!:0 inherit                          */
    } cx_c;        // accessing as bytes
   } cx;   // flags needed by unquote and jtxdefn   inherit for task
   union {
    US uq_us;       // accessing both flags at once
    struct {
     B    spfreeneeded;     // When set, we should perform a garbage-collection pass  persistent    combine w/pmctr & db
     C init0area[0];  // label for initializing
// ************************************** here starts the area that is initialized to 0 when task starts 0x16
     C    bstkreqd;   // set if we MUST create a stack entry for each named call clear for task
    } uq_c;        // accessing as bytes
   } uq;   // flags needed only by unquote  clear for task
  } us;   // access as US
 } uflags;   // 4 bytes
 I4 parsercalls;      // # times parser was called clear for task
 B iepdo;            // 1 iff do iep on going to immex   init for task to 0   should be shared?
 C xmode;            // extended integer operating mode init for task to 0
// 2 bytes free
 I bytesmax;         // high-water mark of "bytes" - used only during 7!:1 clear for task
 S etxn;             // strlen(etx) but set negative to freeze changes to the error line  clear for task
 S etxn1;            // last non-zero etxn    clear for task
 B foldrunning;      // 1 if fold is running (allows Z:) clear for task
 UC jerr;             // error number (0 means no error)      clear for task
 UC jerr1;            // last non-zero jerr  clear for task
 C namecaching;     // 1=for script 2=on  clear for task
 A zombieval;    // the value that the verb result will be assigned to, if the assignment is safe and has inplaceable usecount and is not read-only
            // zombieval may have a stale address, if the name it came from was deleted after zombieval was set.  That's OK, because we use zombieval only to compare
            // against a named value that we have stacked; that value is guaranteed protected so zombieval cannot match it unless zombieval is valid.
 A xmod;             // extended integer: the m in m&|@f clear for task
// end of cacheline 0
 C _cl1[0];
// ************************************** here starts the part that is initialized to non0 values when the task is started.  Earlier values may also be initialized
 C initnon0area[0];
 A locsyms;  // local symbol table, or dummy empty symbol table if none init for task to emptylocale
 UI4 ranks;            // low half: rank of w high half: rank of a  for IRS init for task to 3F3F   should be 2 bytes?
 I4 currslistx;    // index into slist of the current script being executed (or -1 if none) init for task to -1  should be 2 bytes?
 C recurstate;       // state of recursions through JDo    init for task to BUSY
#define RECSTATEIDLE    0  // JE is inactive, waiting for work
#define RECSTATEBUSY    1  // JE is running a call from JDo
#define RECSTATEPROMPT  2  // JE is running, and is suspended having called the host for input
#define RECSTATERECUR   3  // JE is running and waiting for a prompt, and the host has made a recursive call to JDo (which must not prompt)
// **************************************  end of initialized part

 C persistarea[0];  // end of area set at startup
// ************************************** everything after here persists over the life of the thread
 C fillv0len;   // length of fill installed in fillv0
 C taskstate;  // task state: modified by other tasks on a system lock
#define TASKSTATERUNNINGX 0   // task has started
#define TASKSTATERUNNING (1LL<<TASKSTATERUNNINGX)
#define TASKSTATELOCKACTIVEX 1  // task is waiting for any reason
#define TASKSTATELOCKACTIVE (1LL<<TASKSTATELOCKACTIVEX)
#define TASKSTATEACTIVEX 2  // thread is running on this JTT.  Changed under job lock.
#define TASKSTATEACTIVE (1LL<<TASKSTATEACTIVEX)
#define TASKSTATETERMINATEX 3  // thread has been signaled to terminate.  Changed under job lock.
#define TASKSTATETERMINATE (1LL<<TASKSTATETERMINATEX)
// 1 byte free
 US symfreect[2];  // number of symbols in main and overflow local symbol free chains
 LX symfreehead[2];   // head of main and overflow symbol free chains
 UI cstackinit;       // C stack pointer at beginning of execution
 UI cstackmin;        // red warning for C stack pointer
 A filler1[2];
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
 US fcalln;           /* named fn calls: maximum permissible depth     */
 US callstacknext;    // current stack pointer into callstack.  Could be elided if callstack put on a 16K boundary, not a bad idea anyway
 LX symfreetail1;  // tail pointer for overflow chain
 DC sitop;            /* pointer to top of SI stack                                 */
 I bytes;            // bytes currently in use - used only during 7!:1
// end of cacheline 3

 C _cl4[0];
 I shapesink[SY_64?2:4];     // garbage area used as load/store targets of operations we don't want to branch around
// things needed for allocation of large blocks
 A* tstacknext;       // if not 0, points to the recently-used tstack buffer, whose chain field points to tstacknext  
 A* tstackcurr;       // current allocation, holding NTSTACK bytes+1 block for alignment.  First entry points to next-lower allocation   
 PFRAME parserstackframe;  // 4 words    sf field initialized at task-start
// end of cacheline 4

 C _cl5[0];
// things needed by execution of certain verbs
 A idothash0;        // 2-byte hash table for use by i.
 A idothash1;        // 4-byte hash table for use by i.
 A fill;             // user fill atom
 C* fillv;            /* fill value                                      */
 C fillv0[sizeof(Z)];/* default fill value                              */
 RNG *rngdata;    // separately allocated block for RNG
// seldom-used fields
 I malloctotalhwmk;  // highest value since most recent 7!:1
// end of cacheline 5

 C _cl6[0];
// seldom used,  but contended during system lock 
 C *etx;  // [1+NETX];      // display text for last error (+1 for trailing 0)
 void *dtoa;             /* use internally by dtoa.c                        */
 PSTK initparserstack[1];  // stack used for messages when we don't have a real one
 I4 getlasterror;     // DLL error info from previous DLL call
 I4 dlllasterror;     // DLL domain error info (before DLL call)
#if PYXES
 pthread_t pthreadid;  // OS-dependent thread ID.  We need it only for destroying tasks.
 C filler7[16-sizeof(pthread_t)];  // trouble if it's bigger than this!
#else
 I filler7[2];
#endif
 I filler71[1];
// end of cacheline 6

 C _cl7[0];
 // Area used for intertask communication of memory allocation
 A repatq[-PMINL+PLIML+1];  // queue of blocks allocated in this thread but freed by other threads.  Used as a lock, so put in its own cacheline.  We have 5 queues to avoid muxing; could do with 1
 I4 repatbytes;  // number of bytes repatriated since the last garbage collection, modified by all threads
// 4 bytes free
 I mfreegenallo;        // Amount allocated through malloc, biased  modified onlt by owning thread
 I malloctotal;    // net total of malloc/free performed in m.c only  modified onlt by owning thread
// end of cacheline 7
// stats I totalpops;
// stats I nonnullpops;
// the following lines are engaged only for low-performance builds
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
 S systemlock;       // lock used for quiescing all tasks.  Bits in order of descending priority:
#define LOCKPRISYM 1  // lock is requested for symbol extension
#define LOCKPRIPATH 2  // lock is requested to change a locale path
#define LOCKPRIDEBUG 4  // lock is requested for debug suspension
 S systemlocktct;   // counter field, used for systemlock sync
 US breakbytes;    // first byte: used for signals when there is no mapped breakfile.  Bit 0=ATTN request, bit 1=BREAK request.  Byte 1 used as error return value during systemlock
 B stch;             /* enable setting of changed bit                   */
 C asgzomblevel;     // 0=do not assign zombie name before final assignment; 1=allow premature assignment of complete result; 2=allow premature assignment even of incomplete result  scaf remove?
 void *heap;            // heap handle for large allocations
 I mmax;             /* space allocation limit                          */
 A stloc;            // named locales symbol table - this pointer never changes
 A zpath;         // path 'z', used for all initial paths
 I filler0[1];
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
 // rest of cacheline used only in exceptional paths
 S locdellock;  // lock to serialize user request to delete locale
// 4 bytes free
// front-end interface info
 C *capture;          // capture output for python->J etc.  scaf could be byte?
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
 UC dbuser;           // user-entered value for db, 0 or 1 if bit 7 set, take debug continuation from script
 B assert;           /* 1 iff evaluate assert. statements               */
 // rest of cacheline used only in exceptional paths
// 2 bytes free
 void *smpoll;           /* re-used in wd                                   */
 void *opbstr;           /* com ptr to BSTR for captured output             */
 I filler3[3];
// end of cacheline 3

// Cacheline 4: Files
 C _cl4[0];
 A flkd;             /* file lock data: number, index, length           */
 A fopafl;         // table of open filenames; in each one AM is the file handle and the lock is used
 S flock;            // r/w lock for flkd/fopa/fopf
 // rest of cacheline used only in exceptional paths
 S nwthreads;    // number of worker threads allocated so far
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
 I4 outmaxafter;      /* output: maximum # lines after truncation     scaf could be S   */
 I4 outmaxbefore;     /* output: maximum # lines before truncation       */
 I4 outmaxlen;        /* output: maximum line length before truncation   */
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
 // rest of cacheline is essentially read-only
 float igemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for integer matrix product.  _1 means 'never'   scaf could be shorter
 float dgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for float matrix product.  _1 means 'never'
 float zgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for complex matrix product.  _1 means 'never'
 A evm;              /* event messages                                  */
 I (*emptylocale)[MAXTASKS][16];      // locale with no symbols, used when not running explicits, or to avoid searching the local syms.  Aligned on odd word boundary, must never be freed.  One per task, because they are modified
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
 B sesm;             /* whether there is a session manager             */
 C nfe;              /* 1 for J native front end                    */
 C oleop;            /* com flag to capture output                    */
 UC cstacktype;  /* cstackmin set during 0: jt init  1: passed in JSM  2: set in JDo  */
 // 6 bytes free
#if PYXES
 JOBQ *jobqueue;      // accessed indirectly to avoid spilling into the next cache line, as layout is annoying; never changes
#endif
// end of cacheline 7

 JTT threaddata[MAXTASKS] __attribute__((aligned(JTFLAGMSK+1)));
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
#if MAXTASKS>1  // for multithreading
#define JJTOJ(jj) ((JS)((I)(jj)&-JTALIGNBDY))
#else
#define JJTOJ(jj) ((JS)((I)(jj)-offsetof(struct JSTstruct,threaddata)))
#endif
#define JT(p,n) (JJTOJ(p)->n)
#define INITJT(p,n) (p)->n   // in init functions, jjt points to the JS block and we use this to reference components
#define MTHREAD(jjt) (&jjt->threaddata[0])   // jt for master thread.  jjt is the shared jt pointer
#define THREADID(jt) ((((I)(jt)&(JTALIGNBDY-1))>>LGTHREADBLKSIZE)-(offsetof(struct JSTstruct, threaddata[0])>>LGTHREADBLKSIZE))  // thread number from jt.  Thread 0 is the master
#define JTTHREAD0(jt) (JJTOJ(jt)->threaddata)   // the array of JTT structs
#define JTFORTHREAD(jt,n) (&(JTTHREAD0(jt)[n]))   // JTT struct for thread n
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
_Static_assert(offsetof(JST,_cl0)==0*64,"cacheline 0 offset wrong");
_Static_assert(offsetof(JST,_cl1)==1*64,"cacheline 1 offset wrong");
_Static_assert(offsetof(JST,_cl2)==2*64,"cacheline 2 offset wrong");
_Static_assert(offsetof(JST,_cl3)==3*64,"cacheline 3 offset wrong");
_Static_assert(offsetof(JST,_cl4)==4*64,"cacheline 4 offset wrong");
_Static_assert(offsetof(JST,_cl5)==5*64,"cacheline 5 offset wrong");
_Static_assert(offsetof(JST,_cl6)==6*64,"cacheline 6 offset wrong");
_Static_assert(offsetof(JST,_cl7)==7*64,"cacheline 7 offset wrong");
#endif
