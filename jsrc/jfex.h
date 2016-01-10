// J Front End Example
// define minimal subset of JE for JFE use of JE

#ifdef J64
 typedef long long I;
#else
 typedef long I;
#endif

typedef char C;
typedef void* J;

typedef struct {I k,flag,m,t,c,n,r,s[1];}* A; // J array struct used in Jwd

// A field access
#define AK(x)           ((x)->k)        // offset of ravel wrt x
#define AT(x)           ((x)->t)        // Type; one of the #define below
#define AN(x)           ((x)->n)        // # elements in ravel
#define AR(x)           ((x)->r)        // Rank
#define AS(x)           ((x)->s)        // Pointer to shape

// A data access
#define AV(x)           ( (I*)((C*)(x)+AK(x)))  // pointer to ravel
#define BAV(x)          (      (C*)(x)+AK(x) )  // boolean
#define CAV(x)          (      (C*)(x)+AK(x) )  // character
#define USAV(x)         ((US*)((C*)(x)+AK(x)))  // wchar
#define UAV(x)          (     (UC*)(x)+AK(x) )  // unsigned character
#define IAV(x)          AV(x)                   // integer
#define DAV(x)          ( (D*)((C*)(x)+AK(x)))  // double
#define ZAV(x)          ( (Z*)((C*)(x)+AK(x)))  // complex

// A types - AT(x) and jga
#define B01             (I)1L           // B  boolean
#define LIT             (I)2L           // C  literal (character)
#define INT             (I)4L           // I  integer
#define FL              (I)8L           // D  double (IEEE floating point)
#define CMPX            (I)16L          // Z  complex

#define EVDOMAIN 3
