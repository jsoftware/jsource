/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Type Definitions                                                        */

#define U unsigned

#if (SYS & SYS_UNIX)
#define _stdcall      
#endif

#if SY_64
typedef long long          A1;
typedef unsigned long long BT;
typedef long long          I;
typedef long long          SB;
typedef unsigned long long UI;

#else
typedef long               A1;
typedef unsigned long      BT;
typedef long               I;
typedef long               SB;
typedef unsigned long      UI;
#endif

typedef char               B;
typedef char               C;
typedef char*              Ptr;
typedef short              S;
typedef short              C2;
typedef unsigned char      UC;
typedef unsigned short     US;
typedef unsigned short     U2;
typedef unsigned int       UINT;
typedef int                I4;
typedef double             D;
typedef FILE*              F;

typedef long double        LD;
typedef struct {I k,flag,m,t,c,n,r,s[1];}* A;
typedef struct {A a,t;}TA;
typedef A                (*AF)();
typedef UI               (*UF)();
typedef void             (*VF)();
typedef int              (*CMP)();    /* comparison function in sort     */
typedef A                  X;
typedef struct {X n,d;}    Q;
typedef struct {D re,im;}  Z;
typedef union {D d;UINT i[2];} DI;

#if (SYS & SYS_PC+SYS_MACINTOSH)        /* for use by the session manager  */
typedef S SI;
#else
typedef I SI;
#endif

/* Fields of type A                                                        */

#define AK(x)           ((x)->k)        /* offset of ravel wrt x           */
#define AFLAG(x)        ((x)->flag)     /* flag                            */
#define AM(x)           ((x)->m)        /* Max # bytes in ravel            */
#define AT(x)           ((x)->t)        /* Type; one of the #define below  */
#define AC(x)           ((x)->c)        /* Reference count.                */
#define AN(x)           ((x)->n)        /* # elements in ravel             */
#define AR(x)           ((x)->r)        /* Rank                            */
#define AH              7L              /* # non-shape header words in A   */
#define AS(x)           ((x)->s)        /* Pointer to shape                */

#if SY_64
#define AKX(x)          (SZI*(AH+AR(x)))
#define WP(t,n,r)       (AH+ r        +(1&&t&LAST0)+((t&NAME?sizeof(NM):0)+(n)*bp(t)+SZI-1)/SZI)
#else
#define AKX(x)          (SZI*(AH+AR(x)+!(1&AR(x))))
#define WP(t,n,r)       (AH+(r+!(1&r))+(1&&t&LAST0)+((t&NAME?sizeof(NM):0)+(n)*bp(t)+SZI-1)/SZI)
#endif
/* make sure array values are double-word aligned */

#define AV(x)           ( (I*)((C*)(x)+AK(x)))  /* pointer to ravel        */
#define BAV(x)          (      (C*)(x)+AK(x) )  /* boolean                 */
#define CAV(x)          (      (C*)(x)+AK(x) )  /* character               */
#define USAV(x)         ((US*)((C*)(x)+AK(x)))  /* wchar                   */
#define UAV(x)          (     (UC*)(x)+AK(x) )  /* unsigned character      */
#define NAV(x)          ((NM*)((C*)(x)+AK(x)))  /* name                    */
#define IAV(x)          AV(x)                   /* integer                 */
#define DAV(x)          ( (D*)((C*)(x)+AK(x)))  /* double                  */
#define ZAV(x)          ( (Z*)((C*)(x)+AK(x)))  /* complex                 */
#define XAV(x)          ( (X*)((C*)(x)+AK(x)))  /* extended                */
#define QAV(x)          ( (Q*)((C*)(x)+AK(x)))  /* rational                */
#define AAV(x)          ( (A*)((C*)(x)+AK(x)))  /* boxed                   */
#define A1AV(x)         ((A1*)((C*)(x)+AK(x)))  /* boxed relative address  */
#define VAV(x)          ( (V*)((C*)(x)+AK(x)))  /* verb, adverb, conj      */
#define PAV(x)          ( (P*)((C*)(x)+AK(x)))  /* sparse                  */
#define SBAV(x)         ((SB*)((C*)(x)+AK(x)))  /* symbol                  */

/* Types for AT(x) field of type A                                         */
/* Note: BOOL name conflict with ???; SCHAR name conflict with sqltypes.h  */

#define B01             (I)1L           /* B  boolean                      */
#define B01X 0
#define LIT             (I)2L           /* C  literal (character)          */
#define LITX 1
#define INT             (I)4L           /* I  integer                      */
#define INTX 2
#define FL              (I)8L           /* D  double (IEEE floating point) */
#define FLX 3
#define CMPX            (I)16L          /* Z  complex                      */
#define CMPXX 4
#define BOX             (I)32L          /* A  boxed                        */
#define BOXX 5
#define XNUM            (I)64L          /* X  extended precision integer   */
#define XNUMX 6
#define RAT             (I)128L         /* Q  rational number              */
#define RATX 7
#define BIT             (I)256L         /* BT bit boolean                  */
#define BITX 8
#define SB01            (I)1024L        /* P  sparse boolean               */
#define SB01X 10
#define SLIT            (I)2048L        /* P  sparse literal (character)   */
#define SLITX 11
#define SINT            (I)4096L        /* P  sparse integer               */
#define SINTX 12
#define SFL             (I)8192L        /* P  sparse floating point        */
#define SFLX 13
#define SCMPX           (I)16384L       /* P  sparse complex               */
#define SCMPXX 14
#define SBOX            (I)32768L       /* P  sparse boxed                 */
#define SBOXX 15
#define SBT             (I)65536L       /* SB symbol                       */
#define SBTX 16
#define C2T             (I)131072L      /* C2 unicode (2-byte characters)  */
#define C2TX 17
#define VERB            (I)262144L      /* V  verb                         */
#define VERBX 18
#define ADV             (I)524288L      /* V  adverb                       */
#define ADVX 19
#define CONJ            (I)1048576L     /* V  conjunction                  */
#define CONJX 20
#define ASGN            (I)2097152L     /* I  assignment                   */
#define ASGNX 21
#define MARK            (I)4194304L     /* I  end-of-stack marker          */
#define MARKX 22
#define SYMB            (I)8388608L     /* I  locale (symbol table)        */
#define SYMBX 23
#define CONW            (I)16777216L    /* CW control word                 */
#define CONWX 24
#define NAME            (I)33554432L    /* NM name                         */
#define NAMEX 25
#define LPAR            (I)67108864L    /* I  left  parenthesis            */
#define LPARX 26
#define RPAR            (I)134217728L   /* I  right parenthesis            */
#define RPARX 27
#define XD              (I)268435456L   /* DX extended floating point      */
#define XDX 28
#define XZ              (I)536870912L   /* ZX extended complex             */
#define XZX 29

#define ANY             -1L
#define SPARSE          (SB01+SINT+SFL+SCMPX+SLIT+SBOX)
#define DENSE           (NOUN&~SPARSE)
#define NUMERIC         (B01+BIT+INT+FL+CMPX+XNUM+RAT+XD+XZ+SB01+SINT+SFL+SCMPX)
#define DIRECT          (LIT+C2T+B01+BIT+INT+FL+CMPX+SBT)
#define JCHAR           (LIT+C2T+SLIT)
#define NOUN            (NUMERIC+JCHAR+BOX+SBOX+SBT)
#define FUNC            (VERB+ADV+CONJ)
#define RHS             (NOUN+FUNC)
#define IS1BYTE         (B01+LIT)
#define LAST0           (B01+LIT+C2T+NAME)

#define HOMO(s,t)       ((s)==(t) || (s)&NUMERIC&&(t)&NUMERIC || (s)&JCHAR&&(t)&JCHAR)
#define STYPE(t)        ((t)& B01?SB01:(t)& INT?SINT:(t)& FL?SFL:(t)& CMPX?SCMPX:(t)&LIT?SLIT:(t)& BOX?SBOX:0L)
#define DTYPE(t)        ((t)&SB01? B01:(t)&SINT? INT:(t)&SFL? FL:(t)&SCMPX? CMPX:(t)&SLIT?LIT:(t)&SBOX? BOX:0L)

/* Values for AFLAG(x) field of type A                                     */

#define AFRO            (I)1            /* read only; can't change data    */
#define AFNJA           (I)2            /* non-J alloc; i.e. mem mapped    */
#define AFSMM           (I)4            /* SMM managed                     */
#define AFREL           (I)8            /* uses relative addressing        */

#define AABS(rel,k)     ((I)(rel)+(I)(k))   /* absolute address from relative address */
#define AREL(abs,k)     ((I)(abs)-(I)(k))   /* relative address from absolute address */
#define ARELATIVE(w)    (AT(w)&BOX&&AFLAG(w)&AFNJA+AFSMM+AFREL)
#define AADR(w,z)       ((w)?(A)((I)(w)+(I)(z)):(z))
#define AVR(i)          AADR(ad,av[i])
#define IVR(i)          AADR(id,iv[i])
#define WVR(i)          AADR(wd,wv[i])
#define YVR(i)          AADR(yd,yv[i])
#define AAV0(w)         (ARELATIVE(w)?(A)(*AV(w)+(I)(w)):*AAV(w))
#define RELOCATE(w,z)   (ARELATIVE(w)?relocate((I)(w)-(I)(z),(z)):(z))


typedef struct {I i;US n,go,source;C type;} CW;

/* control word (always has corresponding token string)                             */
/* type   - as specified in w.h                                            */
/* go     - line number to go to                                           */
/* source - source line number                                             */
/* i      - beginning index of token string                                */
/* n      - length          of token string                                */


#define DCPARSE  1      /* sentence for parser                                          */
#define DCSCRIPT 2      /* script              -- line()                                */
#define DCCALL   3      /* verb/adv/conj call  -- dbunquote()                           */
#define DCJUNK   4      /* stack entry is obsolete                                      */

typedef struct DS{      /* 1 2 3                                                        */
 struct DS*dclnk;       /* x x x  link to next stack entry                              */
 A dca;                 /*     x  fn/op name                                            */
 A dcf;                 /*     x  fn/op                                                 */
 A dcx;                 /*     x  left argument                                         */
 A dcy;                 /* x x x  tokens; text        ; right argument                  */
 A dcloc;               /*     x  local symb table (0 if not explicit)                  */
 A dcc;                 /*     x  control matrix   (0 if not explicit)                  */
 I dci;                 /* x x x  index ; next index  ; ptr to line #                   */
 I dcj;                 /*   x x        ; prev index  ; error #                         */
 I dcn;                 /*   x x        ; line #      ; ptr to symb entry               */
 I dcm;                 /*   x x        ; script index; # of non-locale part of name    */
 I dcstop;              /*     x  the last stop in this function                        */
 C dctype;              /* x x x  type of entry (see #define DC*)                       */
 B dcsusp;              /* x   x  1 iff begins a debug suspension                       */
 C dcss;                /*     x  single step code                                      */
} DST;

typedef DST* DC;


typedef struct {I e,p;X x;} DX;
                                /* for the p field in DX */
#define DXIPREC         ((I)-1) /* infinite precision    */
#define DXINF           ((I)-2) /* _  infinity           */
#define DXMINF          ((I)-3) /* __ negative infinity  */

/* extended floating point                                                 */
/* e - exponent                                                            */
/* p - precision & other codes                                             */
/*        +ve   # of significant digits                                    */
/*        _1    infinite precision (with trailing 0s)                      */
/*        _2    infinity _                                                 */
/*        _3    negative infinity __                                       */
/* x - mantissa                                                            */
/*        least significant digit first                                    */
/*        decimal point after last digit                                   */


typedef struct {A name,val;I flag,sn,next,prev;} L;

/* symbol pool entry                           LINFO entry                 */
/* name - name on LHS of assignment         or locale name                 */
/* val  - value                             or locale search path          */
/* flag - various flags                                                    */
/* sn   - script index                                                     */
/* next - index of successor   in hash list or 0                           */
/* prev - index of predecessor in hash list or address of hash entry       */

#define LCH             (I)1            /* changed since last exec of 4!:5 */
#define LHEAD           (I)2            /* head pointer (no predecessor)   */
#define LINFO           (I)4            /* locale info                     */


typedef struct{A og,g;I ptr,flag;B sw0;} LS;

/* og:   old value of global                                               */
/* g:    global at this level                                              */
/* ptr:  index in pv/nv if numbered locale                                 */
/*       pointer to stloc entry if named locale                            */
/* flag: 1 if named    locale marked for destruction                       */
/*       2 if numbered locale marked for destruction                       */
/* sw0:  old value of stswitched                                           */


typedef struct{UI hash;I sn;L*e;UC m;C flag,s[1];} NM;

/* hash: hash for  non-locale part of name                                 */
/* m:    length of non-locale part of name                                 */
/* sn:   symbol table number on last reference                             */
/* e:    symbol pool entry   on last reference                             */
/* s:    points to string part of full name (1 to ?? characters)           */

#define NMLOC           1       /* direct   locale abc_lm_                 */
#define NMILOC          2       /* indirect locale abc__de__fgh ...        */
#define NMDOT           4       /* one of the names m. n. u. v. x. y.      */


typedef struct {I a,e,i,x;} P;

/* value fields of sparse array types                                      */
/* fields are offsets from beginning of the P struct                       */
/* a: sparse axes                                                          */
/* e: sparse element                                                       */
/* i: index matrix, columns correspond to a                                */
/* x: value cells corresponding to rows of i                               */

#define SPA(p,a)        ((A)((p)->a+(C*)(p)))
#define SPB(p,a,x)      {(p)->a=(C*)(x)-(C*)(p); RZ(p->a+(C*)(p));}


/* performance monitoring stuff */

typedef struct{
 A name;                /* verb/adverb/conjunction name                    */
 A loc;                 /* locale name                                     */
 I lc;                  /* line number (-1 for entry; -2 for exit)         */
 I s;                   /* space                                           */
 I t[2];                /* time                                            */
 C val;                 /* valence: 1 or 2                                 */
 C unused[3];           /* padding                                         */
} PM;

#define PMCOL  6        /* # of fields in PM                               */

typedef struct{
 I n;                   /* maximum number of records                       */
 I i;                   /* index of next record to be written              */
 I s;                   /* initial bytesmax value                          */
 B rec;                 /* what to record (0 entry & exit; 1 all)          */
 B trunc;               /* what to do on overflow (0 wrap; 1 truncate)     */
 B wrapped;             /* 1 iff wrapping has happened                     */
 C unused[1];           /* padding                                         */
} PM0;


/* each unique symbol has a row in jt->sbu                                 */
/* a row is interpreted per SBU                                            */
/* for best results make sizeof(SBU) a multiple of sizeof(I)               */
 
typedef struct{
 I  i;                  /* index into sbs                                  */
 I  n;                  /* length                                          */
 UI h;                  /* hash value                                      */
 I  color;              /* binary tree: color                              */
 I  parent;             /* binary tree: index of parent                    */
 I  left;               /* binary tree: index of left  child               */
 I  right;              /* binary tree: index of right child               */
 I  order;              /* order number                                    */
 I  down;               /* predecessor in ordering                         */
 I  up;                 /* successor   in ordering                         */
 I  flag;               /* bit flags                                       */
} SBU;

#define SBC2  1         /* 1 iff 2-byte character                          */


typedef struct {AF f1,f2;A f,g,h;I flag,mr,lr,rr,fdep;C id;} V;

#define ID(f)           (f&&FUNC&AT(f)?VAV(f)->id:C0)
  
                                        /* type V flag values              */
                                        /* < 256 see vcompsc.c             */
#define VGERL           (I)256          /* gerund left  argument           */
#define VGERR           (I)512          /* gerund right argument           */
#define VTAYFINITE      (I)1024         /* t. finite polynomial            */
#define VIRS1           (I)2048         /* monad has integral rank support */
#define VIRS2           (I)4096         /* dyad  has integral rank support */
#define VFLR            (I)8192         /* function is <.@g                */
#define VCEIL           (I)16384        /* function is >.@g                */
#define VMOD            (I)32768        /* function is m&|@g               */
#define VLOCK           (I)65536        /* function is locked              */
#define VNAMED          (I)131072       /* named explicit defn             */
#define VFIX            (I)262144       /* f. applied                      */
#define VXOPR           (I)524288       /* : defn with u. and x.           */
#define VXOP            (I)1048576      /* : defn derived fn               */
#define VXOPCALL        (I)2097152      /* : defn derived fn call          */
#define VTRY1           (I)4194304      /* monad contains try.             */
#define VTRY2           (I)8388608      /* dyad  contains try.             */
#define VDDOP           (I)16777216     /* derived from a derived operator */


typedef struct {DX re;DX im;} ZX;

/* extended complex                                                        */
/* re - real part                                                          */
/* im - imaginary part                                                     */

