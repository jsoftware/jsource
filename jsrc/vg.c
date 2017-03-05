/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grades                                                           */

#include "j.h"
#include "vg.h"

// Ideas for work:
// for sort, don't convert the pointers back to indexes - just copy the data
// Sort/grade 2-integer lists by radix?
// check whether testing 6 at a time would help
// in the fast cases, could save the refetch of *loaddr and *hiaddr every time

/************************************************************************/
/*                                                                      */
/* merge sort with special code for n<:5                                */
/*                                                                      */
/************************************************************************/
#define VS(i,j)          (0<CALL1(jt->comp,u[i],u[j]))
#define XC(i,j)          {q=u[i]; u[i]=u[j]; u[j]=q;}
#define P3(i,j,k)        {ui=u[i]; uj=u[j]; uk=u[k]; u[0]=ui; u[1]=uj; u[2]=uk;}
#define P5(i,j,k,l,m)    {ui=u[i]; uj=u[j]; uk=u[k]; ul=u[l]; um=u[m];  \
                          u[0]=ui; u[1]=uj; u[2]=uk; u[3]=ul; u[4]=um;}

// Revised version, with renamed variables, for use in mincomp
#define VS2(i,j)          (!COMPFN(compn,in[i],in[j]))
#define XC2(i,j)          {void *q=in[i]; in[i]=in[j]; in[j]=q;}
#define P32(i,j,k)        {void *ui=in[i]; void *uj=in[j]; void *uk=in[k]; in[0]=ui; in[1]=uj; in[2]=uk;}
#define P52(i,j,k,l,m)    {void *ui=in[i]; void *uj=in[j]; void *uk=in[k]; void *ul=in[l]; void *um=in[m];  \
                          in[0]=ui; in[1]=uj; in[2]=uk; in[3]=ul; in[4]=um;}

// Compare and exchange v0 and v1
// obsolete #define CXCHG(v0,v1,v2) {v2=v0; v2=(void *)((I)v2+(I)v1); v0=!COMPFN(compn,v0,v1)?v1:v0; v2=(void *)((I)v2-(I)v0);}
// This sequence compiles to move/testb/cmovne/cmovne with no branch & 1 test.  The first move is handled by renaming
#define CXCHG2(v0,v1) {void *v2=v0; B t=COMPFN(compn,v0,v1); v0=(!t)?v1:v0; v1=(!t)?v2:v1;}

#if 0  // obsolete 
static void jtmsmerge(J jt,I n,I*u,I*v){I m,q,*x,*xx,*y,*yy,*z;int c;
 q=n/2; z=v;
 x=u;   xx=u+q-1;
 y=u+q; yy=u+n-1;
 while(1){
  c=CALL1(jt->comp,*x,*y);
  if(0<c){*z++=*y++; if(y>yy){m=z-v; z=u+m; DO(1+xx-x, *z++=*x++;); DO(m, *u++=*v++;); break;}}
  else   {*z++=*x++; if(x>xx){m=z-v;                                DO(m, *u++=*v++;); break;}}
}}


void jtmsort(J jt,I n,I*u,I*v){I a,b,c,d,q,ui,uj,uk,ul,um;
 switch(n){
  case 2: 
   if(VS(0,1))XC(0,1); 
   break;
  case 3:
   if(VS(0,1))XC(0,1);
   if(VS(1,2))if(VS(0,2))P3(2,0,1) else XC(1,2);
   break;
  case 4:
   if(VS(0,1))XC(0,1);
   if(VS(2,3))XC(2,3);
   if(VS(1,3)){XC(0,2); XC(1,3);}
   if(VS(1,2))if(VS(0,2))P3(2,0,1) else XC(1,2);
   break;
  case 5: 
   if(VS(0,1))XC(0,1);
   if(VS(2,3))XC(2,3);
   if(VS(1,3)){XC(0,2); XC(1,3);}
   if(VS(4,1))if(VS(4,3)){a=0; b=1; c=3; d=4;}else{a=0; b=1; c=4; d=3;}
   else       if(VS(4,0)){a=0; b=4; c=1; d=3;}else{a=4; b=0; c=1; d=3;}
   if(VS(2,b)){if(3!=c)if(VS(2,c))P5(a,b,c,2,d) else P5(a,b,2,c,d);}
   else       {        if(VS(2,a))P5(a,2,b,c,d) else P5(2,a,b,c,d);}
   break;
  default:
   if(5<n){
    q=n/2;
    msort(q,  u,  v); 
    msort(n-q,u+q,v);
    msmerge(n,u,v);
}}}
#else

// Comparison functions.  Do one comparison before the loop for a fast exit if it differs.
// On VS this sequence, where a single byte is returned, creates a CMP/JE/SETL sequence, performing only one (fused) compare
static __forceinline B compiu(I n, I *a, I *b){I av=*a, bv=*b; if(av!=bv) R av<bv; while(--n){++a; ++b; av=*a, bv=*b; if(av!=bv) R av<bv;} R a<b;}
static __forceinline B compid(I n, I *a, I *b){I av=*a, bv=*b; if(av!=bv) R av>bv; while(--n){++a; ++b; av=*a, bv=*b; if(av!=bv) R av>bv;} R a<b;}
static __forceinline B compdu(I n, D *a, D *b){D av=*a, bv=*b; if(av!=bv) R av<bv; while(--n){++a; ++b; av=*a, bv=*b; if(av!=bv) R av<bv;} R a<b;}
static __forceinline B compdd(I n, D *a, D *b){D av=*a, bv=*b; if(av!=bv) R av>bv; while(--n){++a; ++b; av=*a, bv=*b; if(av!=bv) R av>bv;} R a<b;}

// General sort, with comparisons by function call, but may do extra comparisons to avoid mispredicted branches
#define GRADEFNNAME jmsort
#define MERGEFNNAME jmerge
#define COMPFN (*comp)
#include "vgmerge.h"

// General sort, when comparisons are expensive.  Does minimal comparisons
#define GRADEFNNAME jmsortmincomp
#define MERGEFNNAME jmerge
#define COMPFN (*comp)
#include "vgmergemincomp.h"

// Sorts with inline comparisons
#define GRADEFNNAME jmsortiu
#define MERGEFNNAME jmergeiu
#define COMPFN compiu
#include "vgmerge.h"

#define GRADEFNNAME jmsortid
#define MERGEFNNAME jmergeid
#define COMPFN compid
#include "vgmerge.h"

#define GRADEFNNAME jmsortdu
#define MERGEFNNAME jmergedu
#define COMPFN compdu
#include "vgmerge.h"

#define GRADEFNNAME jmsortdd
#define MERGEFNNAME jmergedd
#define COMPFN compdd
#include "vgmerge.h"


// Perform grade.  zv contains pointers to items, filled in here
static void jtmsortitems(J jt, void **(*sortfunc)(),  I n, void **zv, void **xv){
 // Initialize the result area to be 'in', with pointers to the input items
 C* item = jt->compv; C*item0=item;  I inc = jt->compk; void **zvv = (void**)zv; DQ(n, *zvv++=item; item+=inc;)
 // Do the grade
 void **sortres = (*sortfunc)(jt->comp, jt->compusejt?(I)jt:jt->compn, zv, n, xv);
 // Convert the result to item numbers in the main result area.
 I shift=CTTZI(inc);  // bit number of LSB
 if(inc==1LL<<shift){DQ(n, *zv++=(void *)((I)((C*)*sortres++-item0)>>shift););  // use shift for power of 2 itemsize
 }else{
  // Not a power of 2.  Use rounding multiply to avoid divide throughput
  D recipinc = (D)n/((D)n*(D)inc-0.25);  // make sure the result is correct after truncation, by shading the factor to the high side
  DQ(n, *zv++=(void*)(I)(((C*)*sortres++-item0)*recipinc););
 }
}


// Perform grade when zv contains indexes (or whatever else the caller needs), already filled in
void jtmsort(J jt, I n, void **zv, void **xv){
 // Do the grade
 void **sortres = jmsortmincomp(jt->comp, jt->compusejt?(I)jt:jt->compn, zv, n, xv);
 // Convert the result to the main result area, if it's not already there
 if(sortres!=zv)DQ(n, *zv++=*sortres++;);
}
#endif


#define GF(f)         B f(J jt,I m,I c,I n,A w,I*zv)

/* m  - # cells (# individual grades to do) */
/* c  - # atoms in a cell                   */
/* n  - length of sort axis                 */
/* w  - array to be graded                  */
/* zv - result values                       */

static struct {
 CMP comproutine;
 void **(*sortfunc)();
} sortroutines[][2] = {  // index is [bitx][up]
[B01X]={{compcd,jmsort},{compcu,jmsort}}, [LITX]={{compcd,jmsort},{compcu,jmsort}}, [INTX]={{0,jmsortid},{0,jmsortiu}}, [FLX]={{0,jmsortdd},{0,jmsortdu}},
[CMPXX]={{0,jmsortdd},{0,jmsortdu}},[BOXX]={{compr,jmsortmincomp},{compr,jmsortmincomp}}, [XNUMX]={{compxd,jmsortmincomp},{compxu,jmsortmincomp}}, [RATX]={{compqd,jmsortmincomp},{compqu,jmsortmincomp}},
[C2TX]={{compud,jmsort},{compuu,jmsort}}, [C4TX]={{comptd,jmsort},{comptu,jmsort}}, [SBTX]={{compcd,jmsort},{compcu,jmsort}}
};



// should change args to avoid divide
static GF(jtgrx){A x;I ck,d,t,*xv;
 t=AT(w); ck=c*bp(t);
 jt->compk=ck/n; d=c/n; jt->compn=!(t&CMPX)?d:(d<<1); jt->compv=CAV(w); jt->compw=ARELATIVE(w)?w:0;
 jt->comp=sortroutines[CTTZ(t)][jt->compgt==1].comproutine; jt->compusejt = !!(t&BOX+XNUM+RAT);
 void **(*sortfunc)() = sortroutines[CTTZ(t)][jt->compgt==1].sortfunc;
// obsolete  switch(CTTZ(t)){
// obsolete   case BOXX:  jt->comp=ARELATIVE(w)?compr:compa; break;
// obsolete   case C2TX:  jt->comp=compu;                    break;
// obsolete   case C4TX:  jt->comp=c==n?compt1:compt;        break;
// obsolete   case INTX:  jt->comp=c==n?compi1:compi;        break;
// obsolete   case FLX:   jt->comp=c==n?compd1:compd;        break;
// obsolete   case CMPXX: jt->comp=compd; jt->compn=2*d;     break;
// obsolete   case XNUMX: jt->comp=compx;                    break;
// obsolete   case RATX:  jt->comp=compq;                    break;
// obsolete   default:   jt->comp=compc;
// obsolete  }
 GATV(x,INT,n,1,0); xv=AV(x);  /* work area for msmerge() */
 DO(m, msortitems(sortfunc,n,(void**)zv,(void**)xv); jt->compv+=ck; zv+=n;);
 R !jt->jerr;
}    /* grade"r w on general w */

/* grcol: grade/sort a halfword of an integer or a double      */
/* d:     # of possible different halfwords (65536 or 32768)   */
/* c:     smallest halfword value (0 or 32768)                 */
/* yv:    halfword buckets work area                           */
/* n:     # of sort elements                                   */
/* xv:    input permutation of sort elements                   */
/* zv:    output permutation                                   */
/* m:     # of halfwords in a sort element (2 or 4)            */
/* u:     ptr to sort elements                                 */
/* up:    1 if sort up; 0 if sort down                         */
/* split: 1 iff do split pass of halfword range                */
/* sort:  1 if sort; 0 if grade                                */
// flags: 8: full clear of buckets not needed; 4: sort mode; 2: up; 1: split

// bucket sort, 16 bits at a time
I grcol4(I d,I c,UI4*yv,I n,I*xv,I*zv,const I m,US*u,I flags){
     I split;US*v;I ct00, ctff;
 // clear the bucket area, unless the flags tell us that only the first and last buckets need be cleared
 yv[0]=yv[65535]=0;  // always clear the sign-extension values
 if(flags&8){
 // Here we know that the previous pass found all the values were sign-extensions.  So we just clear the first and last values, and the flag
  flags &= ~8;
 }else{memset(c+yv,C0,d*sizeof(*yv));}  // full clear if the fast option is not selected
 // increment the bucket for each input value
 v=u; DO(n, ++yv[*v]; v+=m;);
 // If all the values are sign-extensions (which will happen often) we can short-circuit much of the processing, including the rolling sum
 // Do this only when there is data/index to copy, to save us the trouble of having a case for synthesizing the index
 if(xv&&((ct00=yv[0])+(ctff=yv[65535])==n)){
  // All sign extensions.  If there is only one, do nothing except copy the input to the output
  flags|=8;  // tell the next pass the good news
  if(ct00&&ctff){S *vs=(S*)u;  // since we know they're sign-extensions, extend when we load
   // There are both positive & negative sign-extensions.  Simulate the full processing, but without the fetches from the table
   // We need the output index for 00 and ff values, which will depend on the up/split values
   // Copy 00 values into index 0 if up && !split  or !up && split; otherwise to ctff
   // copy ff values into index 0 if !up && !split  or  up && split; otherwise to ct00
   // So, zero the appropriate index & use ctff as the pos index, ct00 as the neg
   {I writeposto0=(((flags>>1)^flags)&1); ctff&=(writeposto0-=1); ct00&=~writeposto0;}  // if up^split, generate 1,0,ff, clearing ctff; otherwise 0,ff,00, clearing ct00
   // Read each data-value, and move the value (data or index) to the indicated output position
   if(flags&4){
    // sort mode. copy the data
// obsolete     if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 4 bytes
// obsolete     else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 8 bytes
    if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, I vv=*vs; I writeval=ct00; ct00-= vv; writeval=(vv+=1)?ctff:writeval; ctff+=vv; zvc[writeval]=xvc[i]; vs+=m;)}  // result and intermediates are 4 bytes
    else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, I vv=*vs; I writeval=ct00; ct00-= vv; writeval=(vv+=1)?ctff:writeval; ctff+=vv; zvc[writeval]=xvc[i]; vs+=m;)}  // result and intermediates are 8 bytes
   }else{
    // copy the index, refer through it to get the data.  xv[i] is the initial index mapped to current i.  We fetch the current word for that index, call that xvv
    // Depending on whether xvv is ffff or 0000, move the index xv[i] to the selected output location, and increment whichever
    // pointer it was moved to.
// should use conditionals to save arithmetic
    xv+=n; DP(n, I xvv=vs[m*xv[i]]; I writeval=ct00; ct00-= xvv; writeval=(xvv+=1)?ctff:writeval; ctff+=xvv; zv[writeval]=xv[i];)
   }
  }else{
   // All the sign-extensions are the same.  All there is to do is copy the input to the output.  could try to save the copy with pointer tricks?
   // The length, as below, depends on the operation mode
   memcpy(zv,xv,n*((flags&4)?m*sizeof(US):sizeof(I)));
  }
 }else{UI4 k;
  // Normal case.  Create +/\ of the bucket totals, or +/\. if sorting down (to preserve stability)
  // if this slice contains the sign bit, offset the scan to start at max neg (in the middle)
  // the result of this stage is the starting position in the output of each input value
  I tct = d>>(split=flags&1);  // number of iterations per section
  I tinc = (flags&2)-1;  // +1 if up, -1 if down
  UI4 *t=yv+c+(tct&((tinc>>(BW-1))|-split))+(tinc>>(BW-1));  // starting position: based on up/split: 00: +d-1  01: +d/2-1  10: +0  11: +d/2
  UI4 s=0; do{DP(tct, k=*t; *t=s; s+=k; t+=tinc;) t-=tinc*d;}while(--split>=0);  // 1 iteration if not split, 2 if split
// obsolete  switch(up+2*split){
// obsolete   case 0: t=yv+c+d;     DO(d,   --t; if(k=*t){*t=s; s+=k;}); break;
// obsolete   case 1: t=yv+c-1;     DO(d,   ++t; if(k=*t){*t=s; s+=k;}); break;
// obsolete   case 2: t=yv+c+d/2;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); 
// obsolete           t=yv+c+d  ;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); break;
// obsolete   case 3: t=yv+c+d/2-1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;}); 
// obsolete           t=yv+c    -1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;});
// obsolete  }
  // create the output.  Each input produces an output in the position indicated by yv.
  // If sort is set, we move the value; otherwise move the index.
  // First time through, when xv==0, we use an implied null index vector
  v=u;
  if(flags&4){
   // sort mode. copy the data
// obsolete   if(2==m)                       DO(n, zv[yv[*v        ]++]=xv[i]; v+=m;)
// obsolete   else    {zz=(D*)zv; xx=(D*)xv; DO(n, zz[yv[*v        ]++]=xx[i]; v+=m;);}
   if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 4 bytes
   else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 8 bytes
  }else{
   // copy the index, using xv or implied index vector
// obsolete   if(!xv)                   DO(n, zv[yv[*v        ]++]=   i ; v+=m;)
// obsolete   else                            DO(n, zv[yv[v[m*xv[i]]]++]=xv[i];      );
   if(!xv) {DO(n, zv[yv[*v]++]= i; v+=m;)}
   else{xv+=n; DP(n, I buckno=(UI4)v[m*xv[i]]; I buckval=yv[buckno]; zv[buckval]=xv[i]; yv[buckno]=(UI4)(buckval+=1);)}
  }
 }  // end special-case test
 R flags;  // return input flags, with info about clearing the next buffer
}

// version using 2-byte table, same as above
I grcol2(I d,I c,US*yv,I n,I*xv,I*zv,const I m,US*u,I flags){
     I split;US*v;I ct00, ctff;
 yv[0]=yv[65535]=0;
 if(flags&8){
  flags &= ~8;
 }else{memset(c+yv,C0,d*sizeof(*yv));}
 v=u; DO(n, ++yv[*v]; v+=m;);
 if(xv&&((ct00=yv[0])+(ctff=yv[65535])==n)){
  flags|=8;
  if(ct00&&ctff){S *vs=(S*)u;
   {I writeposto0=(((flags>>1)^flags)&1); ctff&=(writeposto0-=1); ct00&=~writeposto0;}
   if(flags&4){
    if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, I vv=*vs; I writeval=ct00; ct00-= vv; writeval=(vv+=1)?ctff:writeval; ctff+=vv; zvc[writeval]=xvc[i]; vs+=m;)}  // result and intermediates are 4 bytes
    else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, I vv=*vs; I writeval=ct00; ct00-= vv; writeval=(vv+=1)?ctff:writeval; ctff+=vv; zvc[writeval]=xvc[i]; vs+=m;)}  // result and intermediates are 8 bytes
   }else{
    xv+=n; DP(n, I xvv=vs[m*xv[i]]; I writeval=ct00; ct00-= xvv; writeval=(xvv+=1)?ctff:writeval; ctff+=xvv; zv[writeval]=xv[i];)
   }
  }else{
   memcpy(zv,xv,n*((flags&4)?m*sizeof(US):sizeof(I)));
  }
 }else{US k;
  I tct = d>>(split=flags&1);
  I tinc = (flags&2)-1;
  US *t=yv+c+(tct&((tinc>>(BW-1))|-split))+(tinc>>(BW-1));
  US s=0; do{DP(tct, k=*t; *t=s; s+=k; t+=tinc;) t-=tinc*d;}while(--split>=0);
  v=u;
  if(flags&4){
   if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 4 bytes
   else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 8 bytes
  }else{
   if(!xv) {DO(n, zv[yv[*v]++]= i; v+=m;)}
   else{xv+=n; DP(n, I buckno=v[m*xv[i]]; I buckval=(US)yv[buckno]; zv[buckval]=xv[i]; yv[buckno]=(US)(buckval+=1);)}
  }
 }
 R flags;
}


#if 0  // obsolete
// should use integer col
void grcolu(I d,I c,UI*yv,I n,UI*xv,UI*zv,const I m,US*u,int up,int split,int sort){
     C4*xx,*zz;UI k,s,*t;US*v;
 s=0; memset(c+yv,C0,d*SZI); 
 v=u; 
 DO(n, ++yv[*v]; v+=m;);
 switch(up+2*split){
  case 0: t=yv+c+d;     DO(d,   --t; if(k=*t){*t=s; s+=k;}); break;
  case 1: t=yv+c-1;     DO(d,   ++t; if(k=*t){*t=s; s+=k;}); break;
  case 2: t=yv+c+d/2;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); 
          t=yv+c+d  ;   DO(d/2, --t; if(k=*t){*t=s; s+=k;}); break;
  case 3: t=yv+c+d/2-1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;}); 
          t=yv+c    -1; DO(d/2, ++t; if(k=*t){*t=s; s+=k;});
 }
 v=u;
 if(sort){
  if(sizeof(I)==sizeof(C4))      DO(n, zv[yv[*v        ]++]=xv[i]; v+=m;)
  else  {zz=(C4*)zv; xx=(C4*)xv; DO(n, zz[yv[*v        ]++]=xx[i]; v+=m;);}
 }else if(!xv)                   DO(n, zv[yv[*v        ]++]=   i ; v+=m;)
 else                            DO(n, zv[yv[v[m*xv[i]]]++]=xv[i];      );
}
#endif

// grade doubles
// should do final polish of this code
static GF(jtgrd){A x,y;int b;D*v,*wv;I *g,*h,i,nneg,*xv;US*u;void *yv;
  // if not large and 1 atom per key, go do general grade
 if(!(c==n&&n>65536/3.5))R grx(m,c,n,w,zv);
 // grade float by radix sort of halfwords.  Save some control parameters
 wv=DAV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))/SZI)<<use4,1,0); yv=AV(y);}
 GATV(x,INT,n,1,0); xv=AV(x);  // allocate a ping-pong buffer for the result
// obsolete #if C_LE
// obsolete  d= 1; e=0; msbx=3;
// obsolete #else
// obsolete  d=-1; e=3; msbx=0;
// obsolete #endif
 for(i=0;i<m;++i){I colflags;  // loop over each cell of input
  u=(US*)wv+FPLSBWDX;   // point to LSB of input
  // count the number of negative values, call it nneg.  Set b to mean 'both negative and nonnegative are present'
  // If we are doing all negative values, just change the direction and return the sorted values without reversal
  {v=wv; nneg=0; DO(n, nneg+=(((US*)v)[FPMSBWDX]>>15); ++v;); b=0<nneg&&nneg<n;}
  // set the ping-pong buffer pointers so we will end up with the output in zv.  If b is set, we have 5 passes (one final sign-correction pass); if not, 4
  // h is the even-numbered output buffer.  if b is off, we start writing to x and finish in z; if b is set, we start writing to z, finish the 4th pass
  // in x, then the postpass ends in z
  g=b?xv:zv; h=b?zv:xv;
  // sort from LSB to MSB.  Sort in the order requested UNLESS all the values are negative; then reverse the order and save the postpass
  colflags=grcol(65536,0L,yv,n,0LL,h,sizeof(D)/sizeof(US),u,((jt->compgt+1)^((nneg==n)<<1)));  // 'up' in bit 1, inverted if all neg
  colflags=grcol(65536,0L,yv,n,h, g,sizeof(D)/sizeof(US),u+=WDINC,colflags);
  colflags=grcol(65536,0L,yv,n,g, h,sizeof(D)/sizeof(US),u+=WDINC,colflags);
  // for the MSB, which is signed, do the same thing, but to save a few cycles tell grcol if all the values are negative or nonnegative.  grcol will
  // save the time for clearing the unused half of the buffer.  If all neg, call with 32768,32768; if all nonneg, 32768,0; otherwise 65536,0
  grcol(32768<<b,(nneg==n)<<15,yv,n,h, g,sizeof(D)/sizeof(US),u+=WDINC,colflags);

  if(b){D d;I j,m,*u,*v,*vv;I nneg0; 
   // the input contained a mixture of neg/nonneg.  They are sorted into unsigned order in *xv.  Reorder them to have neg first, in reversed order, followed by positive
   // first, merge +0 and -0, which are widely separated in the reult (each at the beginning of their areas).  The merged result must be in
   // ascending order of index
   if(colflags&2){  // values were sorted up, & so now contain +0,  +, -0,  -
    I npos0; u=xv+(n-nneg); for(npos0=-(n-nneg);npos0<0&&wv[u[npos0]]==0;++npos0); npos0+=(n-nneg);  // Count +0
    u=xv+n; for(nneg0=-nneg;nneg0<0&&wv[u[nneg0]]==0;++nneg0); nneg0+=nneg;  // Count -0
    u=zv+nneg-nneg0; I ppos=0,pneg=0; DO(npos0+nneg0, *u++=(pneg>=nneg0||(ppos<npos0&&xv[ppos]<xv[n-nneg+pneg]))?xv[ppos++]:xv[n-nneg+pneg++];)  // merge
    // Copy the positives
    ICPY(u,xv+npos0,n-nneg-npos0); //  /: copy the nonnegatives to the end of result area
    u=zv;     v=xv+n;
   }else{  // values were sorted down, & so now contain  - , -0,  +, +0
    I npos0; u=xv+nneg-1; for(npos0=n-nneg;npos0>0&&wv[u[npos0]]==0;--npos0); npos0=(n-nneg)-npos0;  // Count +0
    u=xv-1; for(nneg0=nneg;nneg0>0&&wv[u[nneg0]]==0;--nneg0); nneg0=nneg-nneg0;  // Count -0
    u=zv+n-nneg-npos0; I ppos=0,pneg=0; DO(npos0+nneg0, *u++=(pneg>=nneg0||(ppos<npos0&&xv[n-npos0+ppos]<xv[nneg-nneg0+pneg]))?xv[n-npos0+ppos++]:xv[nneg-nneg0+pneg++];)  // merge
    // Copy the positives
    ICPY(zv,xv+nneg,n-nneg-npos0); //  /: copy the positives to the beginning of result area
    u=zv+n-nneg+nneg0;     v=xv+nneg-nneg0;
   }
// obsolete   {ICPY(  zv,nneg+xv,n-nneg); u=zv+n-nneg; v=nneg+xv;}  // \: copy the nonnegatives to the beginning of the result area
// BUG: should copy down in other order to avoid pointing past valid region.  As written will run over into the shape... but that will always be positive, at least
   // copy in the negatives, reversing the order.  Here u->place to put negatives, v->last+1 negative
   // as we copy, we have to keep equal keys in the original order, i. e. don't reverse them.
   j=0; d=wv[*(v-1)];
   // At start of this loop, d has the last of a set of (possibly only 1) equal values, j has the index of the output location where
   // the set will be stored.  v scans back through the negatives, looking for a new value, and when it finds one copies out all
   // the old values.  We know that there is at least one positive value in front of the first negative value, and that it must not compare equal,
   // so that each value will get pushed out
   DO(1+nneg-nneg0, --v; if(d!=wv[*v]){vv=1+v; m=i-j; DO(m, *u++=*vv++;); j=i; d=wv[*v];});
  }
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on real w; main code here is for c==n */

// ai==1 and n is large (>40000): grade by repeated bucketsort
static GF(jtgri1){A x,y;I*wv;I i,*xv;US*u;void *yv;
 wv=AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))/SZI)<<use4,1,0); yv=AV(y);}
 // allocate ping-pong for output area
 GATV(x,INT,n,1,0); xv=AV(x);
// obsolete e=SY_64?3:1;  // number-1 of 16-bit sections to process
// obsolete #if C_LE
// obsolete  d= 1;   // direction of processing, from LSB to MSB
// obsolete  e=0;  // offset to LSB
// obsolete #else
// obsolete  d=-1;
// obsolete  e=SY_64?3:1;  // offset to LSB
// obsolete #endif
 // for each sort...
 for(i=0;i<m;++i){I colflags;
  // process each 16-bit section of input
  u=(US*)wv+INTLSBWDX;   // point to LSB
  colflags=grcol(65536,0L,yv,n,0L,xv,sizeof(I)/sizeof(US),u,jt->compgt+1);  // move 'up' to bit 1
#if SY_64
  colflags=grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),u+=WDINC,colflags);
  colflags=grcol(65536,0L,yv,n,zv,xv,sizeof(I)/sizeof(US),u+=WDINC,colflags);
#endif
  grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),u+=WDINC,colflags|1);  // the 1 means 'handle sign bit'
  wv+=c; zv+=n;  // advance to next input/output area
 }
 R 1;
}    /* grade"r w on integer w where c==n */

// should recopy this from int (not float)
static GF(jtgru1){A x,y;C4*wv;I i,*xv;US*u;void *yv;
 wv=C4AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))/SZI)<<use4,1,0); yv=AV(y);}
 GATV(x,INT,n,1,0); xv=AV(x);
// obsolete #if C_LE
// obsolete  d= 1; e=0;
// obsolete #else
// obsolete  d=-1; e=1;
// obsolete #endif
 for(i=0;i<m;++i){I colflags;
// obsolete   u=e+(US*)wv; 
// obsolete   v=wv; k=0; b=0;
// obsolete   g=b?xv:zv; h=b?zv:xv;
  u=(US*)wv+INTLSBWDX;   // point to LSB
  colflags=grcol(65536,0L,yv,n,0L,xv,sizeof(C4)/sizeof(US),u,jt->compgt+1);  // move 'up' to bit 1
  grcol(65536,0L,yv,n,xv,zv,sizeof(C4)/sizeof(US),u+=WDINC,colflags);
// obsolete   grcolu(p, 0L, yv,n,0L,h,sizeof(C4)/sizeof(US),u+0*d,k==n?!up:up,0,0);
// obsolete   grcolu(p, 0L, yv,n, h,g,sizeof(C4)/sizeof(US),u+1*d,k==n?!up:up,0,0);
// obsolete   if(b){C4 d;I j,m,*u,*v,*vv;
// obsolete    if(up){ICPY(k+zv,  xv,n-k); u=zv;     v=n+xv;}
// obsolete    else  {ICPY(  zv,k+xv,n-k); u=zv+n-k; v=k+xv;}
// obsolete    j=0; d=wv[*(v-1)];
// obsolete    DO(1+k, --v; if(d!=wv[*v]){vv=1+v; m=i-j; DO(m, *u++=*vv++;); j=i; d=wv[*v];});
// obsolete   }
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on c4t w where c==n */

#if 0  // obsolete
// returns *base = smallest value, *top = #values (1..2 is 2 values)
// returns 0 for *top if range is not representable in an integer
// returns 2 if n=0 (??)
// superseded by condrange
void irange(I n,I*v,I*base,I*top){I i,p,q;
#if 0  // obsolete
 I d,m=n/2,x,y;
 if(n>m+m)p=q=*v++; else if(n){q=IMAX; p=IMIN;}else p=q=0;
 for(i=0;i<m;++i){
  x=*v++; y=*v++; 
  if(x<y){if(x<q)q=x; if(p<y)p=y;}
  else   {if(y<q)q=y; if(p<x)p=x;}
 }
 *base=q; d=p-q; *top=0>d||d==IMAX?0:1+d;
#else
 q=IMAX; p=IMIN;
 for(i=0;i<n;++i){I x=*v++;
  if(x>p)p=x; if(x<q)q=x; 
 }
 *base=q; *top=MAX(1+p-q,0);
#endif
}

// copy of irange for sizeof(C4)==sizeof(int)
void c4range(I n,C4*v,C4*base,I*top){I i;C4 p,q;
#if 0  // obsolete
 I d,i,m=n/2;C4 p,q,x,y;
 if(n>m+m)p=q=*v++; else if(n){q=C4MAX; p=C4MIN;}else p=q=0;
 for(i=0;i<m;++i){
  x=*v++; y=*v++; 
  if(x<y){if(x<q)q=x; if(p<y)p=y;}
  else   {if(y<q)q=y; if(p<x)p=x;}
 }
 *base=q; d=(I)p-(I)q; *top=0>d||d>=IMAX?0:1+d;
#else
 q=C4MAX; p=C4MIN;
 for(i=0;i<n;++i){C4 x=*v++;
  if(x>p)p=x; if(x<q)q=x; 
 }
 *base=q; *top=MAX(1+(I)p-(I)q,0);
#endif
}

// obsolete F1(jtmaxmin){I base,top;
// obsolete  RZ(w);
// obsolete  ASSERT(INT&AT(w),EVDOMAIN);
// obsolete  irange(AN(w),AV(w),&base,&top);
// obsolete  R v2(base,base+top-1);
// obsolete }
// obsolete 
#endif

static GF(jtgri){A x,y;B up;I d,e,i,*v,*wv,*xv;UI4 *yv,*yvb;
 wv=AV(w); d=c/n;  // d=# ints in an item
 // select algorithm based on size & range.  To develop models for the different algorithms, modify the code here to force one choice
 // & run test lines.
 // Test line for lists for dependence on length of list:
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: y ?@$ y'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 3 + 10 %~ i. 30  // tweak range to places of interest
 // Results 3/2/2017 on Ivy Bridge:
 // smallrange 6.5e_14 5.5e_9 _8e_8 2e_4
 // radix 8.4e_14 2e_9 _1.4e_8 1.6e_4 
 // merge 1.2e_13 2.6e_8 _3e_7 4.5e_4
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: y ?@$ y'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 2 + 10 %~ i. 20
 // smallrange _8e_14 2.5E_9 _2.3e_8 2.7e_6
 // radix 8e_13 _4.6e_10 1.5e_8 6.5e_5
 // merge _1.5e_12 1.2e_8 _6.2e_8 3.7e_6
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: y ?@$ y'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 0 + 10 %~ i. 20
 // smallrange _1.3e_10 5.9e_9 _2.3e_8 5.5e_7
 // radix 9.8e_8 _4.7e_6 2.3e_5 _1.3e_5
 // merge _1.6e_10 1.4e_8 _4.2e_8 5.8e_7

// for lists, smallrange beats radix if range<~70000; smallrange beats merge if range<n*65; radix beats merge if n>1250 (or more depending on how well sign-detection works)

 // Test line for tables of 2 atoms/item:
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: (y,2) ?@$ y'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 0 + 10 %~ i. 20
 // smallrange 3.6e_10 _1.2e_8 5.9e_8 5.3e_7
 // merge _1.7e_10 2.8e_9 3e_8 4.4e_7
 // for items of length 2, smallrange wins unless range too big

 // Test line for tables of 3 atoms/item:
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: (y,3) ?@$ y'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 0 + 10 %~ i. 20
 // smallrange _4e_10 6.6e_9 5.5e_9 5.1e_7
 // merge _1e_10 1.1e_8 _2.5e_8 5.6e_7
 // for items of length 3, smallrange wins unless range too big

 // Test line to see effect of range alone on smallrange speed:
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: 1000 ?@$ y'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 1 + 10 %~ i. 50
 // smallrange _1.3e_15 5.6e_10 _7.4e_9 7.3e_6

 // Test line to see effect of length alone on smallrange speed:
 // (((20) 6!:2 '/: a'&(4 : 'x [ a =: y ?@$ 1000'))"0  %.  (*: ,. (* 2&^.) ,. ] ,. 1:)) <. 10 ^ 1 + 10 %~ i. 50
 // smallrange _4e_15 2.2e_9 _2.8e_8 1.9e_5

 // length causes time to increase at 1e_3 per 100000; range at 2.5e_4 per 100000, for 1-item keys
 // merge costs 1.2e_2 per 100000  (for <10000 items)
 // Mcost = 1.2e_2*n
 // Scost = 1e_3*n*(2*keylength-1) + 2.5e_4*r*keylength
 // so max range is n*(1.2e_2 - 1e_3*(2*keylength-1) / 2.5e_4*keylength
 // which we can approximate as 80>>keylength, where keylength<=6


 // figure out what algorithm to use
 // smallrange always beats radix, but loses to merge if the range is too high.  We assess the max acceptable range as
 // (80>>keylength)*(n-32) where 32 takes get/free overhead into account (merge starts right away, smallrange has to allocate and
 // clear buffers)
 I maxrange; CR rng;
 if(d<=6 && 0<(maxrange=MIN(16,80>>d)*(n-32))){rng = condrange(wv,AN(w),IMAX,IMIN,maxrange);  // test may overflow; OK
 }else rng.range=0;  // if smallrange impossible
// obsolete  UI4 lgn; CTLZI(n,lgn);
// obsolete  I maxrange = n<64?256:(I)((lgn*4-6)*((D)n*(D)n/(4.5*(D)c)));
// obsolete  CR rng = condrange(wv,AN(w),IMAX,IMIN,maxrange);  // for perf testing, change maxrange to IMAX
// obsolete irange(AN(w),wv,&q,&p); 
 // tweak this line to select path for timing
 // If there is only 1 item, radix beats merge for n>1300 or so (all positive) or more (mixed signed small numbers)
 if(!rng.range)R c==n&&n>2000?gri1(m,c,n,w,zv):grx(m,c,n,w,zv);  // revert to other methods if not small-range
// obsolete if(!p||k<p||(0.69*d*(p+2*n))>n*log((D)n))R c==n&&n>65536/1.5?gri1(m,c,n,w,zv):grx(m,c,n,w,zv);
// obsolete if(0<q&&q<k-p){p+=q; q=0;}
 // doing small-range grade.  Allocate a hashtable area.  We will access it as UI4
 GATV(y,C4T,rng.range,1,0); yvb=C4AV(y); yv=yvb-rng.min; up=1==jt->compgt;
 // if there are multiple ints per item, we have to do multiple passes.  Allocate a workarea
 // should start in correct position to end in z
 if(1<d){GATV(x,INT,n,1,0); xv=AV(x);
 }
 for(i=0;i<m;++i){  // Loop over each cell of w
  // if d>1, we will use zv and xv as alternate output pointers, leaving the final result in *zv.
  // If d is odd, we end up in the block we start in; not if d is even.  So, if d is even, we swap
  // zv and xv, to end up in zv.  xv is always defined when d is even.
  if(!(d&1)){I *tv=zv; zv=xv; xv=tv;}  // zv<->xv
  memset(yvb,C0,rng.range*sizeof(UI4));  // clear the table
  v=wv+d-1;   // start in the lowest-significance column
  // create frequency count for each value
// obsolete   if(rng.min) DO(n, ++yv[*v-rng.min]; v+=d;) 
  DQ(n, ++yv[*v]; v+=d;);
  // create +/\ of the counts (+/\. for grade down)
  if(up){UI4 k,s=0,*yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
  else{UI4 k,s=0;  DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
  // refetch each input and take its position from the +/\ list.  After taking a position, add 1 so the next identical value gets the next position
  // store the index of the fetched input value into the indicated position
  v=wv+d-1;  // rescan lowest-significance column
// obsolete   if(rng.min) DO(n, zv[yv[*v-rng.min]++]=i; v+=d;) else
  DO(n, zv[yv[*v]++]=i; v+=d;);
  v=wv+d-1;
  // if items contain multiple atoms, process the remanining atoms similarly, from lowest to highest significance
  for(e=d-2;0<=e;--e){  // loop d-1 times
   // exchange xv and zv.  We will now process from xv (the previous result) and output to zv.
   {I *tv=zv; zv=xv; xv=tv;}  // zv<->xv
   --v;  // back up to next-higher-significance column
   // clear the area
   memset(yvb,C0,rng.range*sizeof(UI4));
   // *v is the base of the vector to process, which has stride d.  Process the elements in the order given by the previous grade.
// obsolete    if(rng.min) DO(n, ++yv[*(v+d*g[i])-rng.min];) 
// obsolete    else  DO(n, ++yv[*(v+d*g[i])  ];);
   {I *vv=v; DQ(n, ++yv[*vv]; vv+=d;); }  // scan all the values
   // create running sum
// obsolete    if(up)DO(rng.range,      if(k=yv[i]){yv[i]=s; s+=k;}) 
// obsolete    else  DO(rng.range, --j; if(k=yv[j]){yv[j]=s; s+=k;});
   if(up){UI4 k, s=0, *yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
   else {UI4 k,s=0; DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
   // process the input atoms in the order of the grade so far, and find the result position as above.  Move the original index
   // for the item into the result
// obsolete    if(rng.min) DO(n, h[yv[*(v+d*g[i])-rng.min]++]=g[i];) else
   {I *gg=xv+n; DP(n, zv[yv[v[d*gg[i]]]++]=gg[i];);}
  }
  // At this point zv always points to the actual result area, so we can increment zv for the next run
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on small-range integers w */


#if 0  // obsolete
static GF(jtgru){A x,y;B b,up;I d,e,i,j,k,p,ps;C4 s,*v,q,*wv;UI*g,*h,*xv,*yv;
 wv=C4AV(w); d=c/n; k=4*n;
 c4range(AN(w),wv,&q,&p); 
 if(!p||k<p||(0.69*d*(p+2*n))>n*log((D)n))R c==n&&n>65536/1.5?gru1(m,c,n,w,zv):grx(m,c,n,w,zv);
 if(0<q&&(I)q<k-p){p+=q; q=0;}
 GATV(y,INT,p,1,0); yv=(UI*)AV(y); ps=p*SZI; up=1==jt->compgt;
 if(1<d){GATV(x,INT,n,1,0); xv=(UI*)AV(x);}
 for(i=0;i<m;++i){C4 k;
  s=0; j=p; memset(yv,C0,ps);
  v=wv+d-1;
  if(q) DO(n, ++yv[*v-q]; v+=d;) 
  else  DO(n, ++yv[*v  ]; v+=d;);
  if(up)DO(p,      if(k=(C4)yv[i]){yv[i]=s; s+=k;}) 
  else  DO(p, --j; if(k=(C4)yv[j]){yv[j]=s; s+=k;});
  v=wv+d-1;
  if(q) DO(n, zv[yv[*v-q]++]=i; v+=d;) 
  else  DO(n, zv[yv[*v  ]++]=i; v+=d;);
  v=wv+d-1;
  for(e=d-2,b=0;0<=e;--e){
   --v;
   if(b){g=xv; h=(UI*)zv; b=0;}else{g=(UI*)zv; h=xv; b=1;}
   s=0; j=p; memset(yv,C0,ps);
   if(q) DO(n, ++yv[*(v+d*g[i])-q];) 
   else  DO(n, ++yv[*(v+d*g[i])  ];);
   if(up)DO(p,      if(k=(C4)yv[i]){yv[i]=s; s+=k;}) 
   else  DO(p, --j; if(k=(C4)yv[j]){yv[j]=s; s+=k;});
   if(q) DO(n, h[yv[*(v+d*g[i])-q]++]=g[i];) 
   else  DO(n, h[yv[*(v+d*g[i])  ]++]=g[i];);
  }
  if(b)DO(n, zv[i]=xv[i];);
  wv+=c; zv+=n;
 }
 R 1;
#else
static GF(jtgru){A x,y;B up;I d,e,i,*xv;UI4 *yv,*yvb;C4 *v,*wv;
 wv=C4AV(w); d=c/n;
 I maxrange; CR rng;
 if(d<=6 && 0<(maxrange=MIN(16,80>>d)*(n-32))){rng = condrange4(wv,AN(w),-1,0,maxrange);
 }else rng.range=0;
 if(!rng.range)R c==n&&n>1500?gru1(m,c,n,w,zv):grx(m,c,n,w,zv);  // revert to other methods if not small-range
 GATV(y,C4T,rng.range,1,0); yvb=C4AV(y); yv=yvb-rng.min; up=1==jt->compgt;
 if(1<d){GATV(x,INT,n,1,0); xv=AV(x);
 }
 for(i=0;i<m;++i){
  if(!(d&1)){I *tv=zv; zv=xv; xv=tv;}
  memset(yvb,C0,rng.range*sizeof(UI4));
  v=wv+d-1;
  DQ(n, ++yv[*v]; v+=d;);
  if(up){UI4 k,s=0,*yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
  else{UI4 k,s=0; DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
  v=wv+d-1;
  DO(n, zv[yv[*v]++]=i; v+=d;);
  v=wv+d-1;
  for(e=d-2;0<=e;--e){
   {I *tv=zv; zv=xv; xv=tv;} 
   --v;
   memset(yvb,C0,rng.range*sizeof(UI4));
   {C4 *vv=v; DQ(n, ++yv[*vv]; vv+=d;); }
   if(up){UI4 k, s=0, *yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
   else {UI4 k,s=0; DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
   {I *gg=xv+n; DP(n, zv[yv[v[d*gg[i]]]++]=gg[i];);}
  }
  wv+=c; zv+=n;
 }
 R 1;
#endif
}    /* grade"r w on small-range c4t w */


#define DOCOL1(p,iicalc0,iicalc1,ind,vinc)  \
 {I*g,*h,   j=p-1,k,s=0;UC*v;                          \
  if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}        \
  memset(yv,C0,ps);                                    \
  v=vv; DO(n, ++yv[iicalc0]; v+=d;);                   \
  if(up)DO(p, k=yv[i]; yv[i  ]=s; s+=k;)               \
  else  DO(p, k=yv[j]; yv[j--]=s; s+=k;);              \
  v=vv; DO(n, h[yv[iicalc1]++]=ind;           vinc;);  \
 }

#define DOCOL4(p,iicalc0,iicalc1,ind,vinc)  \
 {I*g,*h,ii,j=p-1,k,s=0;UC*v;                          \
  if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}        \
  memset(yv,C0,ps);                                    \
  v=vv; DO(n, IND4(iicalc0); ++yv[ii]; v+=d;);         \
  if(up)DO(p, k=yv[i]; yv[i  ]=s; s+=k;)               \
  else  DO(p, k=yv[j]; yv[j--]=s; s+=k;);              \
  v=vv; DO(n, IND4(iicalc1); h[yv[ii]++]=ind; vinc;);  \
 }

static GF(jtgrb){A x;B b,up;I d,i,p,ps,q,*xv,yv[16];UC*vv,*wv;
 if(c>4*n*log((D)n))R grx(m,c,n,w,zv); 
 d=c/n; q=d/4; p=16; ps=p*SZI; wv=UAV(w); up=1==jt->compgt;
 if(1<q){GATV(x,INT,n,1,0); xv=AV(x);}
 for(i=0;i<m;++i){
  vv=wv+d; b=1&&q%2;
  if(q){   vv-=4; DOCOL4(p, *(int*)v, *(int*)v,         i,   v+=d);}
  DO(q-1,  vv-=4; DOCOL4(p, *(int*)v, *(int*)(v+d*g[i]),g[i],v   ););
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on boolean w, works 4 columns at a time (d%4 guaranteed to be 0)*/

static GF(jtgrc){A x;B b,q,up;I d,e,i,p,ps,*xv,yv[256];UC*vv,*wv;
 d=C2T&AT(w)?2*c/n:c/n;
 if(d>log((D)n))R grx(m,c,n,w,zv); 
 p=B01&AT(w)?2:256; ps=p*SZI; wv=UAV(w); up=1==jt->compgt;
 q=C2T&AT(w) && C_LE;
 if(1<d){GATV(x,INT,n,1,0); xv=AV(x);}
 for(i=0;i<m;++i){
  b=(B)(d%2); if(q){e=-3; vv=wv+d-2;}else{e=-1; vv=wv+d-1;}
                 DOCOL1(p,*v,*v,       i,   v+=d); if(q)e=1==e?(q==1?-3:-5):1; 
  DO(d-1, vv+=e; DOCOL1(p,*v,v[d*g[i]],g[i],v   ); if(q)e=1==e?(q==1?-3:-5):1;);
  wv+=d*n; zv+=n;
 }
 R 1;
}    /* grade"r w on boolean or char or unicode w */

static GF(jtgrs){R gri(m,c,n,sborder(w),zv);}    
     /* grade"r w on symbols w */

F2(jtgrade1p){PROLOG(0074);A x,z;I n,*s,*xv,*zv;
 s=AS(w); n=s[0]; jt->compn=s[1]-1; jt->compk=SZI*s[1];
 jt->comp=compp; jt->compsyv=AV(a); jt->compv=CAV(w); jt->compusejt=1;
 GATV(z,INT,n,1,0); zv=AV(z); /* obsolete DO(n, zv[i]=i;); */
 GATV(x,INT,n,1,0); xv=AV(x);
 msortitems(jmsort,n,(void**)zv,(void**)xv);
 EPILOG(z);
}    /* /:(}:a){"1 w , permutation a, integer matrix w */


/************************************************************************/
/*                                                                      */
/* /: and \: main control                                               */
/*                                                                      */
/************************************************************************/

#define GF(f)         B f(J jt,I m,I c,I n,A w,I*zv)

/* m  - # cells (# individual grades to do) */
/* c  - # atoms in a cell                   */
/* n  - length of sort axis                 */
/* w  - array to be graded                  */
/* zv - result values                       */

static B (*grroutine[])(J,I,I,I,A,I*) = {  // index is [bitx]
[B01X]=jtgrc, [LITX]=jtgrc, [INTX]=jtgri, [FLX]=jtgrd, [CMPXX]=jtgrx,[BOXX]=jtgrx, [XNUMX]=jtgrx, [RATX]=jtgrx, [C2TX]=jtgrc, [C4TX]=jtgru, [SBTX]=jtgrs};

// /: and \: with IRS support
F1(jtgr1){PROLOG(0075);A z;I c,f,ai,m,n,r,*s,t,wn,wr,zn;
 RZ(w);
 t=AT(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; jt->rank=0;
 f=wr-r; s=AS(w);
 // Calculate m: #cells in w   n: #items in a cell of w   ai: #atoms in an item of a cell of w  c: #atoms in a cell of w  
 n=r?s[f]:1; if(wn=AN(w)){
  // If w is not empty, it must have an acceptable number of cells
// obsolete  m=prod(f,s); c=m?AN(w)/m:prod(r,f+s); n=r?s[f]:1; RE(zn=mult(m,n));
  PROD(m,f,s); PROD(ai,r-1,f+s+1); c=ai*n; PROD(c,r,f+s); zn=m*n;
 }else{
  // empty w.  The number of cells may overflow, but reshape will catch that
  RE(zn=mult(prod(f,s),n));
 }
 // allocate the entire result area, one int per item in each input cell
 GATV(z,INT,zn,1+f,s); if(!r)*(AS(z)+f)=1;
 // if there are no atoms, or we are sorting things with 0-1 item, return an index vector of the appropriate shape 
 if(!wn||1>=n)R reshape(shape(z),IX(n));
 // do the grade, using a special-case routine if possible
 RZ((t&B01&&0==(ai&3)?jtgrb:grroutine[CTTZ(t)])(jt,m,c,n,w,AV(z)))
// obsolete  switch(CTTZ(t)) {
// obsolete  case B01X:
// obsolete // obsolete   if(t&B01&&0==(c/n)%4)RZ(grb(m,c,n,w,AV(z))) break;  // Booleans, multiples of 4 bytes
// obsolete   if(0==(ai&3)){RZ(grb(m,c,n,w,AV(z))) break;}  // Booleans, multiples of 4 bytes, otherwise fall through to...
// obsolete  case LITX: case C2T: RZ(grc(m,c,n,w,AV(z))) break;  // other 1- or 2-byte types
// obsolete  case SBTX: RZ(grs(m,c,n,w,AV(z))) break;   // symbols
// obsolete  case FLX: RZ(grd(m,c,n,w,AV(z))) break;  // floats, any shape
// obsolete  case INTX: RZ(gri(m,c,n,w,AV(z))) break;  // integer, any shape
// obsolete  case C4TX: RZ(gru(m,c,n,w,AV(z))) break;  // 4-byte characters
// obsolete  default: RZ(grx(m,c,n,w,AV(z))); break;   // anything else
// obsolete  }
 EPILOG(z);
}    /*   grade"r w main control for dense w */

#define GBEGIN(G,L)  A z;int ogt=jt->compgt,olt=jt->complt; jt->compgt=G; jt->complt=L
#define GEND(z)      jt->compgt=ogt; jt->complt=olt; R z

F1(jtgrade1 ){GBEGIN( 1,-1); RZ(   w); z=SPARSE&AT(w)?grd1sp(  w):gr1(  w); GEND(z);}
F1(jtdgrade1){GBEGIN(-1, 1); RZ(   w); z=SPARSE&AT(w)?grd1sp(  w):gr1(  w); GEND(z);}
F2(jtgrade2 ){GBEGIN( 1,-1); RZ(a&&w); z=SPARSE&AT(w)?grd2sp(a,w):gr2(a,w); GEND(z);}
F2(jtdgrade2){GBEGIN(-1, 1); RZ(a&&w); z=SPARSE&AT(w)?grd2sp(a,w):gr2(a,w); GEND(z);}


#define OSGT(i,j) (u[i]>u[j])

#define SORT4   \
 switch(n){                                                  \
  case 2:                                                    \
   if(OSGT(0,1))XC(0,1); break;                              \
  case 3:                                                    \
   if(OSGT(0,1))XC(0,1);                                     \
   if(OSGT(1,2))if(OSGT(0,2))P3(2,0,1) else XC(1,2); break;  \
  case 4:                                                    \
   if(OSGT(0,1))XC(0,1);                                     \
   if(OSGT(2,3))XC(2,3);                                     \
   if(OSGT(1,3)){XC(0,2); XC(1,3);}                          \
   if(OSGT(1,2))if(OSGT(0,2))P3(2,0,1) else XC(1,2);         \
 }

// Partitioning function for order statistics
// j is the order desired, w is the data .  t is a temp buffer we allocate to hold the shrinking partition
#define OSLOOP(T,ATOMF)  \
{T p0,p1,q,*tv,*u,ui,uj,uk,*v,*wv;                                                     \
  tv=wv=(T*)AV(w);                                                                     \
  while(1){                                                                            \
   if(4>=n){u=tv; SORT4; R ATOMF(tv[j]);}        /* stop loop on small partition */       \
   p0=tv[qv[i]%n]; ++i; if(i==qn)i=0;                                                  \
   p1=tv[qv[i]%n]; ++i; if(i==qn)i=0; if(p0>p1){q=p0; p0=p1; p1=q;}       /* create pivots p0, p1 selected from input, with p0 <= p1  */             \
   if(p0==p1){m0=m1=0; v=tv; DO(n, if(p0>*v)++m0;                     ++v;);}          \
   else      {m0=m1=0; v=tv; DO(n, if(p0>*v)++m0; else if(p1>*v)++m1; ++v;);}  /* count m0: # < p0; and m1: # p0<=x<p1  */         \
   c=m0+m1; m=j<m0?m0:j<c?m1:n-c;    /* calc size of partition holding the result */        \
   if(t)u=v=tv; else{GA(t,wt,m,1,0); u=tv=(T*)AV(t); v=wv;}                            \
   if     (j<m0){       DO(n, if(*v<p0        )*u++=*v; ++v;); n=m;}                   \
   else if(j<c ){j-=m0; DO(n, if(p0<=*v&&*v<p1)*u++=*v; ++v;); n=m;}                   \
   else if(c   ){j-=c;  DO(n, if(p1<=*v       )*u++=*v; ++v;); n=m;}                   \
   else{DO(n, if(p1<*v)*u++=*v; ++v;); m=u-tv; c=n-m; if(j<c)R ATOMF(p1); j-=c; n=m;}  \
 }}

F2(jtordstat){A q,t=0;I c,i=0,j,m,m0,m1,n,qn=53,*qv,wt;
 RZ(a&&w);
 n=AN(w); wt=AT(w);
 if(!(!AR(a)&&AT(a)&B01+INT&&4<n&&1==AR(w)&&wt&FL+INT))R from(a,grade2(w,w));
 RE(j=i0(a)); if(0>j)j+=n;
 ASSERT(0<=j&&j<n,EVINDEX);
 // deal 53 random large integers to provide pivots.  We reuse them if needed
 RZ(q=df2(sc(qn),sc(IMAX),atop(ds(CQUERY),ds(CDOLLAR)))); qv=AV(q);
 if(wt&FL)OSLOOP(D,scf) else OSLOOP(I,sc);
}    /* a{/:~w */

F2(jtordstati){A t;I n,wt;
 RZ(a&&w);
 n=AN(w); wt=AT(w);
 if(!(!AR(a)&&AT(a)&B01+INT&&4<n&&1==AR(w)&&wt&FL+INT))R from(a,grade1(w));
 RZ(t=ordstat(a,w));   // Get the value of the ath order statistic, then look up its index
 I j=0;  // =0 needed to stifle warning
 if(wt&FL){D p=*DAV(t),*v=DAV(w); DO(n, if(p==*v++){j=i; break;});}
 else     {I p=* AV(t),*v= AV(w); DO(n, if(p==*v++){j=i; break;});}
 R sc(j);
}    /* a {/:w */
