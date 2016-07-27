/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: s:                                                               */

#include "j.h"


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

#define BLACK           0
#define RED             1
#define ROOT            (jt->sbroot)
#define FILLFACTOR      (jt->sbfillfactor)
#define GAP             (jt->sbgap)  

static const  SBU       sentinel = {0,0,0,BLACK,0,0,0,IMIN,0,0};

/* #define TMP */
#ifdef TMP
#include <time.h>
static int              tmp_lr=0; 
static int              tmp_rr=0; 
static int              tmp_lt=0; 
static int              tmp_while=0; 
static int              tmp_node=0; 
static int              tmp_reorder=0; 
static int              tmp_moves=0; 
static int              tmp_imax=0; 
static int              tmp_rhit=0; 
static int              tmp_lhit=0;
static clock_t          clo;
static D                tickk=1.0/CLOCKS_PER_SEC;
#endif


/* implementation dependend declarations */
typedef enum {
    STATUS_OK,
    STATUS_MEM_EXHAUSTED,
    STATUS_DUPLICATE_KEY,
    STATUS_KEY_NOT_FOUND
} statusEnum;


#ifdef TMP
#define NODE(a)         (tmp_node++,a+jt->sbuv)
#else
#define NODE(a)         (a+jt->sbuv)
#endif

#define NODEM(a,b)      (jt->sbuv[a].b)
#define LEFT(a)         NODEM(a,left)
#define RIGHT(a)        NODEM(a,right)
#define ORDER(a)        NODEM(a,order)
#define INDEX(a)        NODEM(a,i)
#define LENGTH(a)       NODEM(a,n)
#define HASH(a)         NODEM(a,h)
#define COLOR(a)        NODEM(a,color)
#define DOWN(a)         NODEM(a,down)
#define UP(a)           NODEM(a,up)
#define PARENT(a)       NODEM(a,parent)
#define GRANDPARENT(a)  (PARENT(PARENT(a)))
#define ISLEFTCHILD(x)  (x ==  LEFT(PARENT(x)))
#define ISRIGHTCHILD(x) (x == RIGHT(PARENT(x)))
#define compLT(a,b)     Vcompare(jt,a,b)
#define compEQ(a,b)     ( a == b )



#ifdef TMP
static void showdepth(J jt, I node, int **ptr, I* size, I depth)
{
  if(LEFT(node) == 0) {
    (*ptr) = realloc((*ptr), sizeof(I)*((*size)+1));
    (*ptr)[(*size)++]=depth;
  }
  else             showdepth(jt,  LEFT(node), ptr, size, depth+1);

  if(RIGHT(node) == 0) {
    (*ptr) = realloc((*ptr), sizeof(I)*((*size)+1));
    (*ptr)[(*size)++]=depth;
  }
  else             showdepth(jt, RIGHT(node), ptr, size, depth+1);
}
#endif

static __inline int Vcompare(J jt,I a,I b){I m,n;SBU*u,*v;UC*s,*t;U2*p,*q;
#ifdef TMP
 tmp_lt++;
#endif
 u=a+jt->sbuv; m=u->n; s=(UC*)(jt->sbsv+u->i);
 v=b+jt->sbuv; n=v->n; t=(UC*)(jt->sbsv+v->i);
 switch((SBC2&u->flag?2:0)+(SBC2&v->flag?1:0)){
  case 0: {                                DO(MIN(m,n), if(*s!=*t)R *s<*t; ++s; ++t;);} break;
  case 1: {          q=(U2*)t;       n/=2; DO(MIN(m,n), if(*s!=*q)R *s<*q; ++s; ++q;);} break;
  case 2: {p=(U2*)s;           m/=2;       DO(MIN(m,n), if(*p!=*t)R *p<*t; ++p; ++t;);} break;
  case 3: {p=(U2*)s; q=(U2*)t; m/=2; n/=2; DO(MIN(m,n), if(*p!=*q)R *p<*q; ++p; ++q;);} 
 }
 R m<n;
}

static __inline void rotateLeft(J jt, I x) {I y;
#ifdef TMP
    tmp_lr++;
#endif
   /***************************
    *  rotate node x to left  *
    ***************************/
    y = RIGHT(x);
    if (RIGHT(x)= LEFT(y))    PARENT(LEFT(y)) = x;         /* establish x->right link  */
    if (y)                    PARENT(y)       = PARENT(x); /* establish y->parent link */
    if (PARENT(x) == 0)       ROOT            = y;
    else if (ISLEFTCHILD(x))  LEFT (PARENT(x))= y;
    else                      RIGHT(PARENT(x))= y;
    if (LEFT(y) = x)          PARENT(x)       = y;         /* link x and y             */
}


static __inline void rotateRight(J jt, I x) {I y;
#ifdef TMP
    tmp_rr++;
#endif
   /***************************
    *  rotate node x to right *
    ***************************/
    y = LEFT(x);
    if (LEFT(x)= RIGHT(y))    PARENT(RIGHT(y))= x;         /* establish x->left link   */
    if (y)                    PARENT(y)       = PARENT(x); /* establish y->parent link */
    if (PARENT(x) == 0)       ROOT            = y;
    else if (ISRIGHTCHILD(x)) RIGHT(PARENT(x))= y;
    else                      LEFT (PARENT(x))= y;
    if (RIGHT(y)= x)          PARENT(x)       = y;         /* link x and y             */
}

static __inline void insertFixup(J jt, I x) {B b;I y;

   /*************************************
    *  maintain Red-Black tree balance  *
    *  after inserting node x           *
    *************************************/

    /* check Red-Black properties                                  */
    /* the repositioning is necessary to propogate the rebalancing */
    while (x != ROOT && COLOR(PARENT(x)) == RED) {
#ifdef TMP
        tmp_while++;
#endif
        if (ISLEFTCHILD(PARENT(x))) {           /* we have a violation          */
            y = RIGHT(GRANDPARENT(x));          /* uncle                        */
            b = COLOR(y)==BLACK;                /* uncle is BLACK               */
            if (b && ISRIGHTCHILD(x)) {x=PARENT(x); rotateLeft(jt,x);}             
            COLOR(PARENT(x)) = BLACK;
            COLOR(GRANDPARENT(x)) = RED;
            if (b) rotateRight(jt, GRANDPARENT(x));
            else   {COLOR(y)=BLACK; x=GRANDPARENT(x);}
        }else {                                 /* mirror image of above code   */
            y = LEFT(GRANDPARENT(x));           /* uncle                        */
            b = COLOR(y)==BLACK;                /* uncle is BLACK               */
            if (b && ISLEFTCHILD(x)) {x=PARENT(x); rotateRight(jt,x);}
            COLOR(PARENT(x)) = BLACK;
            COLOR(GRANDPARENT(x)) = RED;
            if (b) rotateLeft(jt, GRANDPARENT(x));
            else   {COLOR(y)=BLACK; x=GRANDPARENT(x);}
    }}
    COLOR(ROOT) = BLACK;
}

static statusEnum insert(J jt, I key) {
    I current,dist,i,keep1,keep2,lorder,parent,rorder,to_the_left,to_the_right;SBU *x;
#ifdef TMP
    static I icount=0;
#endif

    if (key < 0) R STATUS_KEY_NOT_FOUND;

   /***********************************************
    *  allocate node for data and insert in tree  *
    ***********************************************/

    /* find future parent */
    current = ROOT;  /* jt-> root points to the int value of the root symbol */
    parent = to_the_left = to_the_right = 0;

    while (current != 0) {
        if (compEQ(key, current))return STATUS_DUPLICATE_KEY;
        parent = current;
        if(compLT(key, current)){to_the_right=current; current= LEFT(current);}
        else                    {to_the_left =current; current=RIGHT(current);}
    }

#ifdef TMP
    icount++;
    if (icount==10000&&0) {I corder,running;
     icount=running=corder=0;
     do {
      ORDER(running)=corder;
      corder+=FILLFACTOR;
      running=UP(running);
     } while(running);
    }
#endif

    /* get the new node */

    lorder = to_the_left  ? ORDER(to_the_left)  : 0;
    rorder = to_the_right ? ORDER(to_the_right) : lorder + 2 * FILLFACTOR;

    if(rorder-lorder<2) {    /*  if(rorder-lorder<(2*GAP)) { */
      i=0;
      /*parameter GAP is TWICE the difference in order numbers we want after re-ordering*/
      while(to_the_right&&to_the_left&&(ORDER(to_the_right)-ORDER(to_the_left))<(GAP*++i)){
       keep1=to_the_left;  to_the_left =DOWN(to_the_left);
       keep2=to_the_right; to_the_right=UP  (to_the_right);
      }

#ifdef TMP
      if(!to_the_left )UP(0)  =keep1,i++,tmp_lhit++ ;
      if(!to_the_right)DOWN(0)=keep2,i++,tmp_rhit++;
      tmp_imax=__max(i,tmp_imax);
      tmp_moves+=2*i;
      tmp_reorder++;
#else
      if(!to_the_left )UP(0)  =keep1,i++;
      if(!to_the_right)DOWN(0)=keep2,i++;
#endif

      lorder= to_the_left  ? ORDER(to_the_left ) : rorder-2*i*FILLFACTOR;
      rorder= to_the_right ? ORDER(to_the_right) : lorder+2*i*FILLFACTOR;
      dist = (rorder-lorder)/(2*i);

      while(--i) {
       to_the_left =UP  (to_the_left);  lorder+=dist; ORDER(to_the_left )=lorder;
       to_the_right=DOWN(to_the_right); rorder-=dist; ORDER(to_the_right)=rorder;
      }
    }

    x = NODE(key);
    x->parent= parent;
    x->left  = 0;
    x->right = 0;
    x->color = RED;
    x->order = (rorder+lorder)/2;
    x->up    = to_the_right; DOWN(to_the_right)=key;
    x->down  = to_the_left;  UP(to_the_left)   =key;

    /* insert node in tree */
    if     (0==parent)          ROOT         =key;
    else if(compLT(key, parent))LEFT(parent) =key;
    else                        RIGHT(parent)=key;

    insertFixup(jt, key);

    return STATUS_OK;
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

static I jtsbextend(J jt,I n,C*s,UI h,I hi){A x;I c,*hv,j,p;SBU*v;
 c=jt->sbun;
 if(c==*AS(jt->sbu)){                   /* extend sbu unique symbols    */
  RZ(x=ext(1,jt->sbu)); jt->sbu=x; jt->sbuv=(SBU*)AV(x);
 }
 if(AN(jt->sbs)<n+jt->sbsn){            /* extend sbs strings           */
  GATV(x,LIT,2*(n+jt->sbsn),1,0); MC(CAV(x),jt->sbsv,jt->sbsn);
  fa(jt->sbs); ra(x); jt->sbs=x; jt->sbsv=CAV(x);
 }
 if(AN(jt->sbh)<2*c){                   /* extend sbh hash table        */
  p=2*AN(jt->sbh); DO(64, if(p<=ptab[i]){p=ptab[i]; break;});
  RZ(x=apv(p,-1L,0L)); hv=AV(x); v=jt->sbuv;
  DO(c, j=v++->h%p; while(0<=hv[j])j=(1+j)%p; hv[j]=i;);
  fa(jt->sbh); ra(x); jt->sbh=x; jt->sbhv= AV(x);
  hi=h%p;                               /* new hi wrt new sbh size      */
  while(0<=hv[hi])hi=(1+hi)%p; 
 }
 R hi;
}

static SB jtsbinsert(J jt,B c2,I n,C*s,UI h,I hi){I c,m,p;SBU*u;
 c=jt->sbun;                            /* cardinality                  */
 m=jt->sbsn;                            /* existing # chars in sbs      */
 p=c2&&m%2;                             /* pad for alignment            */
 RE(hi=sbextend(n+p,s,h,hi));           /* extend global tables as req'd*/
 MC(SBSV(m+p),s,n);                     /* copy string into sbs         */
 u=SBUV(c); u->i=m+p; u->n=n; u->h=h;   /* index/length/hash            */
 u->flag=c2?SBC2:0;
 ASSERTSYS(STATUS_OK==insert(jt,c),"sbinsert");
 (jt->sbhv)[hi]=c;                      /* make sbh point to new symbol */
 ++jt->sbun;                            /* # unique symbols             */
 jt->sbsn+=n+p;                         /* # chars in sbs               */
 R(SB)c;
}    /* insert new symbol */

static SB jtsbprobe(J jt,B c2,I n,C*s){B b;C*t;I hi,hn,ui;SBU*u;UI h;
 h=c2?hic2(n,(UC*)s):hic(n,(UC*)s);
 hn=AN(jt->sbh);                        /* size of hast table           */
 hi=h%hn;                               /* index into hash table        */
 while(1){
  ui=(jt->sbhv)[hi];                    /* index into unique symbols    */
  if(0>ui)R sbinsert(c2,n,s,h,hi);      /* new symbol                   */
  u=SBUV(ui);
  if(h==u->h){                          /* old symbol, maybe            */
   t=SBSV(u->i);
   switch((c2?2:0)+(u->flag&SBC2?1:0)){
    case 1: if(n==u->n/2){C2*q=(C2*)t; b=1; DO(n,   if(s[i]!=q[i]){b=0; break;}); if(b)R(SB)ui;}
    case 2: if(n==u->n*2){C2*q=(C2*)s; b=1; DO(n/2, if(t[i]!=q[i]){b=0; break;}); if(b)R(SB)ui;} 
    case 3:
    case 0: if(n==u->n&&!memcmp(t,s,n))R(SB)ui; break;
  }}
  hi=(1+hi)%hn;                         /* next hash table index        */
}}   /* insert new symbol or get existing symbol */


static A jtsbunstr(J jt,I q,A w){A z;B c2;I i,j,m,wn;SB*zv;
 RZ(w);
 if(!AN(w))R vec(SBT,0L,0L);
 ASSERT(AT(w)&LIT+C2T,EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 c2=1&&AT(w)&C2T; wn=AN(w);
 if(c2){C2 c,*wv=(C2*)AV(w); 
  c=wv[q==-1?0:wn-1];
  m=0; DO(wn, if(c==wv[i])++m;);
  GATV(z,SBT,m,1,0); zv=SBAV(z);
  if(q==-1){for(i=j=1;i<=wn;++i)if(c==wv[i]||i==wn){RE(*zv++=sbprobe(c2,2*(i-j),(C*)(j+wv))); j=i+1;}}
  else     {for(i=j=0;i< wn;++i)if(c==wv[i]       ){RE(*zv++=sbprobe(c2,2*(i-j),(C*)(j+wv))); j=i+1;}}
 }else{C c,*wv=CAV(w); 
  c=wv[q==-1?0:wn-1];
  m=0; DO(wn, if(c==wv[i])++m;);
  GATV(z,SBT,m,1,0); zv=SBAV(z);
  if(q==-1){for(i=j=1;i<=wn;++i)if(c==wv[i]||i==wn){RE(*zv++=sbprobe(c2,i-j,j+wv)); j=i+1;}}
  else     {for(i=j=0;i< wn;++i)if(c==wv[i]       ){RE(*zv++=sbprobe(c2,i-j,j+wv)); j=i+1;}}
 }
 R z;
}    /* monad s: on leading (_1=q) or trailing (_2=q) character separated strings */

static A jtsbunlit(J jt,C cx,A w){A z;B c2;I i,m,wc,wr,*ws;SB*zv;
 RZ(w);
 ASSERT(!AN(w)||AT(w)&LIT+C2T,EVDOMAIN);
 ASSERT(1<AR(w),EVRANK);
 c2=1&&AT(w)&C2T; wr=AR(w); ws=AS(w); wc=ws[wr-1];
 RE(m=wc?AN(w)/wc:prod(wr-1,ws));
 GATV(z,SBT,m,wr-1,ws); zv=SBAV(z);
 if(!wc)memset(zv,C0,m*sizeof(SB));
 else if(c2){C2 c=(C2)cx,*s,*wv=(C2*)AV(w);
  for(i=0;i<m;++i){
   s=wc+wv; DO(wc, if(c!=*--s)break;);   /* exclude trailing "blanks"    */
   RE(*zv++=sbprobe(c2,2*((c!=*s)+s-wv),(C*)wv));
   wv+=wc;
 }}else{C c=cx,*s,*wv=CAV(w);
  for(i=0;i<m;++i){
   s=wc+wv; DO(wc, if(c!=*--s)break;);   /* exclude trailing "blanks"    */
   RE(*zv++=sbprobe(c2,(c!=*s)+s-wv,wv));
   wv+=wc;
 }}
 R z;
}    /* each row of literal array w less the trailing "blanks" is a symbol */

static F1(jtsbunbox){A*wv,x,z;B c2;I i,m,n,wd;SB*zv;
 RZ(w);
 ASSERT(!AN(w)||BOX&AT(w),EVDOMAIN);
 m=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 GATV(z,SBT,m,AR(w),AS(w)); zv=SBAV(z);
 for(i=0;i<m;++i){
  x=WVR(i); n=AN(x); c2=1&&AT(x)&C2T;
  ASSERT(!n||AT(x)&LIT+C2T,EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  RE(*zv++=sbprobe(c2,c2?n+n:n,CAV(x)));
 }
 R z;
}    /* each element of boxed array w is a string */

static F1(jtsbunind){A z;I j,n,*zv;
 RZ(z=cvt(INT,w));
 zv=AV(z); n=jt->sbun;
 DO(AN(w), j=*zv++; ASSERT(0<=j&&j<n,EVINDEX););
 AT(z)=SBT;
 R z;
}    /* w is a numeric array of symbol indices */

#ifdef TMP
F1(jtsb1){
 A abc;
 clo=clock();
 RZ(w);
 switch(AT(w)){
  default:  ASSERT(0,EVDOMAIN);
  case C2T:
  case LIT: abc=(1>=AR(w)?sbunstr(-1L,w):sbunlit(' ',w)); break;
  case BOX: abc=(sbunbox(w));
 }  
 clo-=clock();
 R abc;
}
#else
F1(jtsb1){
 RZ(w);
 switch(AT(w)){
  default:  ASSERT(0,EVDOMAIN);
  case C2T:
  case LIT: R 1>=AR(w)?sbunstr(-1L,w):sbunlit(' ',w);
  case BOX: R sbunbox(w);
}}   /* monad s: main control */
#endif


F1(jtsborder){A z;I n,*zv;SB*v;
 RZ(w);
 n=AN(w); v=SBAV(w);
 ASSERT(!n||SBT&AT(w),EVDOMAIN);
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
 DO(n, *zv++=SBUV(*v++)->order;);
 R z;
}    /* order numbers for symbol array w */

static F1(jtsbbox){A z,*zv;C*s;I n;SB*v;SBU*u;
 RZ(w);
 n=AN(w); v=SBAV(w);
 ASSERT(!n||SBT&AT(w),EVDOMAIN);
 GATV(z,BOX,n,AR(w),AS(w)); zv=AAV(z);
 DO(n, u=SBUV(*v++); s=SBSV(u->i); RZ(*zv++=SBC2&u->flag?vec(C2T,u->n/2,s):str(u->n,s)););
 R z;
}    /* boxed strings for symbol array w */

#define C2FSB(zv,u,q,m,c)  \
 {C*s=SBSV(u->i);I k=u->n;                                         \
  if(SBC2&u->flag){MC(zv,s,k); zv+=k/=2;}else DO(k, *zv++=*s++;);  \
  if(2==q)*zv++=c; else if(3==q)DO(m-k, *zv++=c;);                 \
 }

static A jtsbstr(J jt,I q,A w){A z;B c2=0;C c;I m,n;SB*v,*v0;SBU*u;
 RZ(w);
 m=n=AN(w); v=v0=SBAV(w); c=1==q?'`':C0;
 ASSERT(!n||SBT&AT(w),EVDOMAIN);
 DO(n, u=SBUV(*v++); if(u->flag&SBC2){c2=1; m+=u->n/2;}else m+=u->n;); 
 v=v0; 
 GA(z,c2?C2T:LIT,m,1,0);
 if(c2){C2*zv;
  zv=(C2*)AV(z); 
  if(1==q)*zv++=c;
  DO(n-1, u=SBUV(*v++); C2FSB(zv,u,2,0,c););
  if(n){  u=SBUV(*v++); C2FSB(zv,u,q,0,c);}
 }else{C*zv;
  zv=CAV(z); 
  if(1==q)*zv++=c;
  DO(n-1, u=SBUV(*v++); MC(zv,SBSV(u->i),u->n); zv+=u->n; *zv++=c;); 
  if(n){  u=SBUV(*v++); MC(zv,SBSV(u->i),u->n); zv+=u->n; if(2==q)*zv=c;}
 }
 R z;
}    /* leading (1=q) or trailing (2=q) separated string for symbol array w */

static A jtsblit(J jt,C c,A w){A z;B c2=0;I k,m=0,n;SB*v,*v0;SBU*u;
 RZ(w);
 n=AN(w); v=v0=SBAV(w);
 ASSERT(!n||SBT&AT(w),EVDOMAIN);
 DO(n, u=SBUV(*v++); k=u->n; if(u->flag&SBC2){c2=1; k/=2;} if(m<k)m=k;); 
 v=v0;
 GA(z,c2?C2T:LIT,n*m,1+AR(w),AS(w)); *(AR(w)+AS(z))=m;
 if(c2){C2*zv=(C2*)AV(z); DO(n, u=SBUV(*v++); C2FSB(zv,u,3,m,c););}
 else  {C*zv=CAV(z); memset(zv,c,n*m); DO(n, u=SBUV(*v++); MC(zv,SBSV(u->i),u->n); zv+=m;);}
 R z;
}    /* literal array for symbol array w padded with c */


static F1(jtsbhashstat){A z;I j,k,n,p,*zv;SBU*v;
 n=jt->sbun; v=jt->sbuv; p=AN(jt->sbh);
 GATV(z,INT,n,1,0); zv=AV(z);
 DO(n, j=v++->h%p; k=1; while(i!=(jt->sbhv)[j]){j=(j+1)%p; ++k;} *zv++=k;);
 R z;
}    /* # queries in hash table for each unique symbol */

static A jtsbcheck1(J jt,A una,A sna,A u,A s,A h,A roota,A ff,A gp){PROLOG;A x,*xv,y;
     B b,*dnv,*lfv,*rtv,*upv;C*ptv,*sv;I c,f,g,hn,*hv,i,j,r,sn,un,*yv;SBU*uv,*v;
 RZ(una&&sna&&u&&s&&h);
 ASSERTD(!AR(una),"c atom");            /* cardinality   */
 ASSERTD(INT&AT(una),"c integer");
 c=*AV(una);
 ASSERTD(0<=c,"c non-negative");
 ASSERTD(!AR(sna),"sn atom");           /* string length */
 ASSERTD(INT&AT(sna),"sn integer");
 sn=*AV(sna); 
 ASSERTD(0<=sn,"sn non-negative");      /* root          */
 ASSERTD(!AR(roota),"root atom"); 
 ASSERTD(INT&AT(roota),"root integer");
 r=*AV(roota);
 ASSERTD(0<=r,"root non-negative");
 ASSERTD(r<c,"root bounded by c");
 ASSERTD(!AR(ff),"ff atom");            /* fill factor   */
 ASSERTD(INT&AT(ff),"ff integer");
 f=*AV(ff);
 ASSERTD(0<=f,"ff non-negative");
 ASSERTD(!AR(gp),"gap atom");           /* gap           */
 ASSERTD(INT&AT(gp),"gap integer");
 g=*AV(gp);
 ASSERTD(0<=g,"gap non-negative");
 ASSERTD(g<f,"gap bounded by ff");
 sv=CAV(s);
 un=*AS(u); uv=(SBU*)AV(u);
 hn= AN(h); hv=AV(h);
 ASSERTD(2==AR(u),"u matrix");
 ASSERTD(INT&AT(u),"u integer");
 ASSERTD(*(1+AS(u))==sizeof(SBU)/SZI,"u #columns");
 ASSERTD(c<=un,"c bounded by #u");
 ASSERTD(1==AR(s),"s vector");
 ASSERTD(LIT&AT(s),"s literal");
 ASSERTD(sn<=AN(s),"sn bounded by #s");
 ASSERTD(1==AR(h),"h vector");
 ASSERTD(INT&AT(h),"h integer");
 ASSERTD(c<=AN(h),"c bounded by #h");
 ASSERTD(equ(vec(INT,1L,&hn),factor(sc(hn))),"#h prime");
 b=0; DO(AN(h), j=hv[i]; if(-1==j)b=1; else ASSERTD(0<=j&&j<c,"h index"););
 ASSERTD(b,"h full");
 GATV(x,B01,c,1,0); lfv=BAV(x); memset(lfv,C0,c);
 GATV(x,B01,c,1,0); rtv=BAV(x); memset(rtv,C0,c);
 GATV(x,B01,c,1,0); dnv=BAV(x); memset(dnv,C0,c);
 GATV(x,B01,c,1,0); upv=BAV(x); memset(upv,C0,c);
 GATV(x,LIT,c,1,0); ptv=CAV(x); memset(ptv,C0,c); ptv[0]=1;
 GATV(x,BOX,c,1,0); xv=AAV(x); RZ(xv[0]=str(uv->n,sv+uv->i));
 GATV(y,INT,c,1,0); yv= AV(y); yv[0]=uv->order;
 for(i=1,v=1+uv;i<c;++i,++v){B c2;I ord,vi,vn;UC*vc;UI k;
  c2=1&&v->flag&SBC2;
  vi=v->i;
  vn=v->n;
  vc=(UC*)(sv+vi);
  ASSERTD(0<=vi&&vi<=sn,"u index");
  ASSERTD(!(c2&&vi%2),"u index alignment");
  ASSERTD(0<=vn&&!(c2&&vn%2),"u length");
  ASSERTD(sn>=vi+vn,"u index/length");
  k=(c2?hic2:hic)(vn,vc);
  ASSERTD(k==v->h,"u hash");
  j=k%hn; while(i!=hv[j]&&0<=hv[j])j=(1+j)%hn;
  ASSERTD(i==hv[j],"u/h mismatch");
  ASSERTD(BLACK==v->color||RED==v->color,"u color");
  RZ(xv[i]=c2?vec(C2T,vn/2,vc):str(vn,vc));
  yv[i]=ord=v->order;
  j=v->parent; ASSERTD(    0<=j&&j<c&&2>=++ptv[j],"u parent");                        
  j=v->left;   ASSERTD(!j||0<=j&&j<c&&1>=++lfv[j]&&     ord>(j+uv)->order ,"u left"       );
  j=v->right;  ASSERTD(!j||0<=j&&j<c&&1>=++rtv[j]&&     ord<(j+uv)->order ,"u right"      );
  j=v->down;   ASSERTD(    0<=j&&j<c&&1>=++dnv[j]&&(!j||ord>(j+uv)->order),"u predecessor");
  j=v->up;     ASSERTD(    0<=j&&j<c&&1>=++upv[j]&&(!j||ord<(j+uv)->order),"u successor"  );
 }
 ASSERTD(equ(grade1(x),grade1(y)),"u order");
 EPILOG(one);
}

static F1(jtsbcheck){R sbcheck1(sc(jt->sbun),sc(jt->sbsn),jt->sbu,jt->sbs,jt->sbh,sc(ROOT),sc(FILLFACTOR),sc(GAP));}

static F1(jtsbsetdata){A h,s,u,*wv,x;I wd;
 RZ(w);
 ASSERTD(BOX&AT(w),"arg type");
 ASSERTD(1==AR(w), "arg rank");
 ASSERTD(8==AN(w), "arg length");
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 RZ(sbcheck1(WVR(0),WVR(1),WVR(2),WVR(3),WVR(4),WVR(5),WVR(6),WVR(7)));
 jt->sbun=*AV(WVR(0));
 jt->sbsn=*AV(WVR(1));
 RZ(x=ca(WVR(2))); ra(x); u=jt->sbu; jt->sbu=x; jt->sbuv=(SBU*)AV(x);
 RZ(x=ca(WVR(3))); ra(x); s=jt->sbs; jt->sbs=x; jt->sbsv=     CAV(x);
 RZ(x=ca(WVR(4))); ra(x); h=jt->sbh; jt->sbh=x; jt->sbhv=      AV(x);
 ROOT      =*AV(WVR(5));
 FILLFACTOR=*AV(WVR(6));
 GAP       =*AV(WVR(7));
 fa(u); fa(s); fa(h);
 R one;
}

static F1(jtsbgetdata){A z,*zv;
 GAT(z,BOX,8,1,0); zv=AAV(z);
 RZ(zv[0]=sc(jt->sbun));
 RZ(zv[1]=sc(jt->sbsn));
 RZ(zv[2]=ca(jt->sbu));
 RZ(zv[3]=ca(jt->sbs));
 RZ(zv[4]=ca(jt->sbh));
 RZ(zv[5]=sc(ROOT));
 RZ(zv[6]=sc(FILLFACTOR));
 RZ(zv[7]=sc(GAP));
 R z;
}

F2(jtsb2){A z;I j,k,n;
#ifdef TMP
 I*zv;
#endif
 RZ(a&&w);
 RE(j=i0(a)); n=AN(w);
 ASSERT(!(1<=j&&j<=7)||!n||SBT&AT(w),EVDOMAIN);
 switch(j){
  default:   ASSERT(0,EVDOMAIN);
  case 0:
   RE(k=i0(w));
   switch(k){
    default: ASSERT(0,EVDOMAIN);
    case 0:  R sc(jt->sbun);
    case 1:  R sc(jt->sbsn);
    case 2:  R ca(jt->sbu);
    case 3:  R ca(jt->sbs);
    case 4:  R ca(jt->sbh);
    case 5:  R sc(ROOT);
    case 6:  R sc(FILLFACTOR);
    case 7:  R sc(GAP);
    case 10: R sbgetdata(zero);
    case 11: R sbcheck(zero);
    case 12: R sbhashstat(zero);
   }
  case  1:   R sbstr(1L,w);
  case -1:   R sbunstr(-1L,w);
  case  2:   R sbstr(2L,w);
  case -2:   R sbunstr(-2L,w);
  case  3:   R sblit(C0,w);
  case -3:   R sbunlit(C0,w);
  case  4:   R sblit(' ',w);
  case -4:   R sbunlit(' ',w);
  case  5:   R sbbox(w);
  case -5:   R sbunbox(w);
  case  6:   RZ(z=ca(w)); AT(z)=INT; R z;
  case -6:   R sbunind(w);
  case  7:   R sborder(w);
  case 10:   R sbsetdata(w);
  case 16:   GAP = 4;                                       R sc(GAP);
  case 17:   GAP++;         ASSERT(FILLFACTOR>GAP,EVLIMIT); R sc(GAP);
  case 18:   GAP--;                                         R sc(GAP);
  case 19:   FILLFACTOR=1024;                               R sc(FILLFACTOR);
  case 20:   FILLFACTOR*=2;                                 R sc(FILLFACTOR);
  case 21:   FILLFACTOR/=2; ASSERT(FILLFACTOR>GAP,EVLIMIT); R sc(FILLFACTOR);
#ifdef TMP
  case 22:
    GAT(z,INT,10,1,0); zv=AV(z);
    zv[0] = tmp_lr      = 0;
    zv[1] = tmp_rr      = 0;
    zv[2] = tmp_lt      = 0;
    zv[3] = tmp_while   = 0;
    zv[4] = tmp_node    = 0;
    zv[5] = tmp_reorder = 0;
    zv[6] = tmp_moves   = 0;
    zv[7] = tmp_imax    = 0;
    zv[8] = tmp_lhit;
    zv[9] = tmp_rhit;
    R z;
  case 23:
    GAT(z,INT,10,1,0);
    zv[0] = tmp_lr;
    zv[1] = tmp_rr;
    zv[2] = tmp_lt;
    zv[3] = tmp_while;
    zv[4] = tmp_node;
    zv[5] = tmp_reorder;
    zv[6] = tmp_moves;
    zv[7] = tmp_imax;
    zv[8] = tmp_lhit;
    zv[9] = tmp_rhit;
    R z;
  case 24:   R sc((I)clo);
  case 25:   R scf(tickk);
#endif
}}

// This is an initialization routine, so memory allocations performed here are NOT
// automatically freed by tpop()
B jtsbtypeinit(J jt){A x;I c=sizeof(SBU)/SZI,s[2];
 s[0]=2000; s[1]=c;
 GA(x,LIT,20000,1,0);       jt->sbs=x; jt->sbsv=     CAV(x); jt->sbsn=0;  // size too big for GAT; initialization anyway
 RZ(x=apv(ptab[5+PTO],-1L,0L)); jt->sbh=x; jt->sbhv=      AV(x);
 GATV(x,INT,*s*c,2,s);        jt->sbu=x; jt->sbuv=(SBU*)AV(x);
 GAP=15;                /* TWICE the difference in order numbers we want after re-ordering */
 FILLFACTOR=1024;
 ROOT=0;                /* initialize binary tree; initialize the empty symbol (used as fill) */
 jt->sbuv[0]=sentinel;
 jt->sbun=1;
 *jt->sbhv=0;
 R 1;
}    /* initialize global data for SBT datatype */
