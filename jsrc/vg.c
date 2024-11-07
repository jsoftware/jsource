/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grades                                                           */

#include "j.h"
#include "vg.h"
// Places marked TUNE have parameters that must be tuned to the hardware


// Ideas for work:
// Sort/grade 2-integer lists by radix?
// check whether testing 6 at a time would help
// use movsb to copy blocks of addresses
// go bottom-up, grouping by 5 at a time.  Avoids function calls and guarantees 16B boundary alignment
//  after the bottom merge
// Look into interleaving the merge streams.  Worse for writes, better for reads
// Vector code for merge?

/************************************************************************/
/*                                                                      */
/* merge sort with special code for n<:5                                */
/*                                                                      */
/************************************************************************/
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
// This sequence compiles to move/testb/cmovne/cmovne with no branch & 1 test.  The first move is handled by renaming
#define CXCHG2(v0,v1) {void *v2=v0; B t=COMPFN(compn,v0,v1); v0=(!t)?v1:v0; v1=(!t)?v2:v1;}


// Comparison functions.  Do one comparison before the loop for a fast exit if it differs.
// On VS this sequence, where a single byte is returned, creates a CMP/JE/SETL sequence, performing only one (fused) compare
// #define COMPGRADE(T,t) T av=*a, bv=*b; if(av!=bv) R av t bv; while(--n){++a; ++b; av=*a, bv=*b; if(av!=bv) R av t bv;} R a<b;
#define COMPGRADE(T,t) do{T av=*a, bv=*b; if(av!=bv) R av t bv; if(!--n)break; ++a; ++b;}while(1); R a<b;
static INLINE B compiu(I n, I *a, I *b){COMPGRADE(I,<)}
static INLINE B compid(I n, I *a, I *b){COMPGRADE(I,>)}
static INLINE B compdu(I n, D *a, D *b){COMPGRADE(D,<)}
static INLINE B compdd(I n, D *a, D *b){COMPGRADE(D,>)}

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
static void msortitems(SORT *sortblok, void **(*sortfunc)(),  I n, void **zv, void **xv, I sortparm){
 // Initialize the result area to be 'in', with pointers to the input items
 C* item = sortblok->v; C*item0=item;  I inc = sortblok->k; void **zvv = (void**)zv; DQ(n, *zvv++=item; item+=inc;)
 // Do the grade
 void **sortres = (*sortfunc)(sortblok->f, sortparm, zv, n, xv);
 // Convert the result to item numbers in the main result area.
 I shift=CTTZI(inc);  // bit number of LSB
 if(inc==((I)1)<<shift){DQ(n, *zv++=(void *)((I)((C*)*sortres++-item0)>>shift););  // use shift for power of 2 itemsize
 }else{
  // Not a power of 2.  Use rounding multiply to avoid divide throughput
  D recipinc = (D)n/((D)n*(D)inc-0.25);  // make sure the result is correct after truncation, by shading the factor to the high side
  DQ(n, *zv++=(void*)(I)(((C*)*sortres++-item0)*recipinc););
 }
}


// Perform grade when zv contains indexes (or whatever else the caller needs), already filled in
void msort(SORT *sortblok, I n, void **zv, void **xv, I sortparm){
 // Do the grade
 void **sortres = jmsortmincomp(sortblok->f, sortparm, zv, n, xv);
 // Convert the result to the main result area, if it's not already there
 if(sortres!=zv)DQ(n, *zv++=*sortres++;);
}


// m: #cells in w (# sorts to do)   n: #items in a cell of w   ai: #atoms in an item of a cell of w
// JTDESCEND in jt is set for descending
#define GF(f)         B f(J jt,I m,I ai,I n,A w,I* RESTRICT zv)

/* m  - # cells (# individual grades to do) */
/* c  - # atoms in a cell                   */
/* n  - length of sort axis                 */
/* w  - array to be graded                  */
/* zv - result values                       */

static struct {
 CMP comproutine;   // 0 if comparison bundled w/sort
 void **(*sortfunc)();
} sortroutines[][2] = {  // index is [precisionx][up]
[B01X]={{compcd,jmsort},{compcu,jmsort}}, [LITX]={{compcd,jmsort},{compcu,jmsort}}, [INTX]={{0,jmsortid},{0,jmsortiu}}, [FLX]={{0,jmsortdd},{0,jmsortdu}},
[CMPXX]={{0,jmsortdd},{0,jmsortdu}},[BOXX]={{compr,jmsortmincomp},{compr,jmsortmincomp}}, [XNUMX]={{compxd,jmsortmincomp},{compxu,jmsortmincomp}}, [RATX]={{compqd,jmsortmincomp},{compqu,jmsortmincomp}},
[QPX]={{0,jmsortdd},{0,jmsortdu}},
[C2TX]={{compud,jmsort},{compuu,jmsort}}, [C4TX]={{comptd,jmsort},{comptu,jmsort}}, [SBTX]={{compcd,jmsort},{compcu,jmsort}},
[INT2X]={{compsd,jmsort},{compsu,jmsort}}, [INT4X]={{compld,jmsort},{complu,jmsort}},
};


// General grade.  jt has the JTDESCEND flag.  Build the control block to hold merge info
static GF(jtgrx){F1PREFJT;A x;I ck,t,*xv;I c=ai*n;
 t=AT(w); SORT sortblok;
 void **(*sortfunc)() = sortroutines[CTTZ(t)][(~(I)jtinplace>>JTDESCENDX)&1].sortfunc;  // the major routine to use (normal merge or minimum comparisons)
 sortblok.f=sortroutines[CTTZ(t)][(~(I)jtinplace>>JTDESCENDX)&1].comproutine;  // comparison function
 sortblok.jt=jtinplace;  // jt including direction bit
 I natoms=ai<<!!(t&CMPX+QP);   // number of atoms to compare in loop
 sortblok.n=natoms;  // pass in as parm
 natoms=(t&BOX+XNUM+RAT)?(I)&sortblok:natoms;  // for simple compares, pass in #atoms.  For more complex ones, the whole sort block
 sortblok.k=ai<<bplg(t); ck=sortblok.k*n;  // item size in bytes, for copying
 sortblok.v=CAV(w);  // address of the source data
 GATV0(x,INT,n,1); xv=AV1(x);  /* work area for msmerge() */
 DQ(m, msortitems(&sortblok,sortfunc,n,(void**)zv,(void**)xv,natoms); sortblok.v+=ck; zv+=n;);
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
// flags: 8: full clear of buckets not needed; 4: on for sort, off for grade; 2: up; 1: split

// bucket sort, 16 bits at a time
I grcol4(I d,I c,UI4*yv,I n,I*xv,I*zv,const I m,US*u,I flags){
     I split;US*v;I ct00, ctff;
 // clear the bucket area, unless the flags tell us that only the first and last buckets need be cleared
 yv[0]=yv[65535]=0;  // always clear the sign-extension values
 if(flags&8){
 // Here we know that the previous pass found all the values were sign-extensions.  So we just clear the first and last values, and the flag
  flags &= ~8;
 }else{mvc(d*sizeof(*yv),c+yv,MEMSET00LEN,MEMSET00);}  // full clear if the fast option is not selected
 // increment the bucket for each input value
 v=u; DQ(n, ++yv[*v]; v+=m;);
 // If all the values are sign-extensions (which will happen often) we can short-circuit much of the processing, including the rolling sum
 // Do this only when there is data/index to copy, to save us the trouble of having a case for synthesizing the index
 if(xv&&((ct00=yv[0])+(ctff=yv[65535])==n)){
  // All sign extensions.  If there is only one, do nothing except copy the input to the output
  flags|=8;  // tell the next pass the good news
  if((-ct00&-ctff)<0){S *vs=(S*)u;  // since we know they're sign-extensions, extend when we load
   // There are both positive & negative sign-extensions.  Simulate the full processing, but without the fetches from the table
   // We need the output index for 00 and ff values, which will depend on the up/split values
   // Copy 00 values into index 0 if up && !split  or !up && split; otherwise to ctff
   // copy ff values into index 0 if !up && !split  or  up && split; otherwise to ct00
   // So, zero the appropriate index & use ctff as the pos index, ct00 as the neg
   {I writeposto0=(((flags>>1)^flags)&1); ctff&=(writeposto0-=1); ct00&=~writeposto0;}  // if up^split, generate 1,0,ff, clearing ctff; otherwise 0,ff,00, clearing ct00
   // Read each data-value, and move the value (data or index) to the indicated output position
   if(flags&4){
    // sort mode. copy the data
    if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, I vv=*vs; I writeval=ct00; ct00-= vv; writeval=(vv+=1)?ctff:writeval; ctff+=vv; zvc[writeval]=xvc[i]; vs+=m;)}  // result and intermediates are 4 bytes
    else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, I vv=*vs; I writeval=ct00; ct00-= vv; writeval=(vv+=1)?ctff:writeval; ctff+=vv; zvc[writeval]=xvc[i]; vs+=m;)}  // result and intermediates are 8 bytes
   }else{
    // copy the index, refer through it to get the data.  xv[i] is the initial index mapped to current i.  We fetch the current word for that index, call that xvv
    // Depending on whether xvv is ffff or 0000, move the index xv[i] to the selected output location, and increment whichever
    // pointer it was moved to.
    xv+=n; DP(n, I xvv=vs[m*xv[i]]; I writeval=ct00; ct00-= xvv; writeval=(xvv+=1)?ctff:writeval; ctff+=xvv; zv[writeval]=xv[i];)
   }
  }else{
   // All the sign-extensions are the same.  All there is to do is copy the input to the output.  could try to save the copy with pointer tricks?
   // The length, as below, depends on the operation mode
   I csiz=m<<LGSZUS; csiz=flags&4?csiz:SZI; MC(zv,xv,n*csiz);
  }
 }else{UI4 k;
  // Normal case.  Create +/\ of the bucket totals, or +/\. if sorting down (to preserve stability)
  // if this slice contains the sign bit, offset the scan to start at max neg (in the middle)
  // the result of this stage is the starting position in the output of each input value
  I tct = d>>(split=flags&1);  // number of iterations per section
  I tinc = (flags&2)-1;  // +1 if up, -1 if down
  UI4 *t=yv+c+(tct&(REPSGN(tinc)|-split))+REPSGN(tinc);  // starting position: based on up/split: 00: +d-1  01: +d/2-1  10: +0  11: +d/2
  UI4 s=0; do{DP(tct, k=*t; *t=s; s+=k; t+=tinc;) t-=tinc*d;}while(--split>=0);  // 1 iteration if not split, 2 if split
  // create the output.  Each input produces an output in the position indicated by yv.
  // If sort is set, we move the value; otherwise move the index.
  // First time through, when xv==0, we use an implied null index vector
  v=u;
  if(flags&4){
   // sort mode. copy the data
   if(2==m) {C4 *zvc=(C4*)zv; C4 *xvc=(C4*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 4 bytes
   else {D *zvc=(D*)zv; D *xvc=(D*)xv+n; DP(n, zvc[yv[*v]++]=xvc[i]; v+=m;)}  // result and intermediates are 8 bytes
  }else{
   // copy the index, using xv or implied index vector
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
 }else{mvc(d*sizeof(*yv),c+yv,MEMSET00LEN,MEMSET00);}
 v=u; DQ(n, ++yv[*v]; v+=m;);
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
   MC(zv,xv,n*((flags&4)?m*sizeof(US):sizeof(I)));
  }
 }else{US k;
  I tct = d>>(split=flags&1);
  I tinc = (flags&2)-1;
  US *t=yv+c+(tct&(REPSGN(tinc)|-split))+REPSGN(tinc);
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



// grade doubles
#if BW==64
// grade doubles by hiding the item number in the value and sorting.  Requires ai==1.
// We interpret the input as integer form so that we can hide the item number in an infinity without turning it into a NaN
static GF(jtgrdq){F1PREFJT;
 // For stability, we keep all the interior sorts ascending.  Here we set a code to precondition the values so that comes out right
 I sortdown63=SGNIF(jtinplace,JTDESCENDX)&IMIN;  // sign bit set if sorting down; other bits 0
 // See how many bits we must reserve for the item number, and make a mask for the item number
 unsigned long hbit=CTLZI(n-1); ++hbit; I itemmask=((I)1<<hbit)-1;  // mask where the item number will go
 // Loop over each grade
 I *wv=IAV(w);  // we interpret the floats in w as if they were integers.
 while(--m>=0){
  // zv points to the output area, wv points to the input
  // Create the values to be sorted, in the result area
  // if sorting down, change sign of input
  // convert -0 to 0
  // if input neg, complement low bits to make correct sort order
  // install item number
  DO(n, I v=wv[i]^sortdown63; v^=(UI)REPSGN(v)>>1; v=(v==0)?-1:v; zv[i]=(v&(~itemmask))+i;)
  // sort the result area in place
  vvsortqs8ai(zv,n);  // always ascending
  // pass through the result area, removing the upper bits.  If consecutive values have the same upper bits, go through them,
  // replacing the upper bits with the actual bits from the input (after honoring sign/direction bits), and then re-sort the result in place.
  // remove the upper bits from that sorted result
  I nextv=zv[0];  // always has first value with a new key
  I i;for(i=0;i<n-1;++i){
   I currv=nextv;
   if(((nextv=zv[i+1])^currv)&~itemmask){zv[i]=currv&itemmask;  // normal case with no repetition
   }else{  // reprocess the repeated block
    I j=i;do{
     I v=wv[zv[j]&itemmask]^sortdown63; v^=(UI)REPSGN(v)>>1; v=(v==0)?-1:v; zv[j]=((v&itemmask)<<hbit)+(zv[j]&itemmask); // fetch original v, reconstitute; get itemmask in uppper bits 
    }while(!(++j==n || (((nextv=zv[j])^currv)&~itemmask)));
    vvsortqs8ai(zv+i,j-i);  // sort the collision area in place, ascending.  j points to first item beyond the collision area, and nextv is its value
    while(i<j){zv[i]&=itemmask; ++i;}
    i=j-1;  // pick up after the batch
   }
  }
  // We handled all the starting items up to the next-last.  The last item may be unprocessed, if it was not part of a batch.  Handle it now.
  zv[n-1]&=itemmask;
  // advance to next sort
  wv+=n; zv+= n;
 }
 R 1;
}

#endif


static GF(jtgrd){F1PREFJT;A x,y;int b;D*v,*wv;I *g,*h,nneg,*xv;US*u;void *yv;I c=ai*n;
#if BW==64
 if(ai==1){R jtgrdq(jtinplace,m,ai,n,w,zv);}  // if fast list code is available, always use it
#endif
  // if not large and 1 atom per key, go do general grade
 if(!(ai==1&&n>3300))R grx(m,ai,n,w,zv);  // Empirically derived crossover   TUNE
 // The rest of this routine is not used on lists when the fast list code is available
 // grade float by radix sort of halfwords.  Save some control parameters
 wv=DAV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV0(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1); yv=AV1(y);}
 GATV0(x,INT,n,1); xv=AV1(x);  // allocate a ping-pong buffer for the result
 while(--m>=0){I colflags;  // loop over each cell of input
  u=(US*)wv+FPLSBWDX;   // point to LSB of input
  // count the number of negative values, call it nneg.  Set b to mean 'both negative and nonnegative are present'
  // If we are doing all negative values, just change the direction and return the sorted values without reversal
  {v=wv; nneg=0; DQ(n, nneg+=(((US*)v)[FPMSBWDX]>>15); ++v;); b=BETWEENO(nneg,0,n);}
  // set the ping-pong buffer pointers so we will end up with the output in zv.  If b is set, we have 5 passes (one final sign-correction pass); if not, 4
  // h is the even-numbered output buffer.  if b is off, we start writing to x and finish in z; if b is set, we start writing to z, finish the 4th pass
  // in x, then the postpass ends in z
  g=b?xv:zv; h=b?zv:xv;
  // sort from LSB to MSB.  Sort in the order requested UNLESS all the values are negative; then reverse the order and save the postpass
  colflags=grcol(65536,0L,yv,n,0LL,h,sizeof(D)/sizeof(US),u,(((~(I)jtinplace>>(JTDESCENDX-1))&2)^((nneg==n)<<1)));  // 'up' in bit 1, inverted if all neg
  colflags=grcol(65536,0L,yv,n,h, g,sizeof(D)/sizeof(US),u+=WDINC,colflags);
  colflags=grcol(65536,0L,yv,n,g, h,sizeof(D)/sizeof(US),u+=WDINC,colflags);
  // for the MSB, which is signed, do the same thing, but to save a few cycles tell grcol if all the values are negative or nonnegative.  grcol will
  // save the time for clearing the unused half of the buffer.  If all neg, call with 32768,32768; if all nonneg, 32768,0; otherwise 65536,0
  grcol(32768<<b,(nneg==n)<<15,yv,n,h, g,sizeof(D)/sizeof(US),u+=WDINC,colflags);

  if(b){D d;I j,m,*u,*v,*vv;I nneg0; 
   // the input contained a mixture of neg/nonneg.  They are sorted into unsigned order in *xv.  Reorder them to have neg first, in reversed order, followed by positive
   // first, merge +0 and -0, which are widely separated in the result (each at the beginning of their areas).  The merged result must be in
   // ascending order of index
   if(colflags&2){  // values were sorted up, & so now contain +0,  +, -0,  -
    I npos0; u=xv+(n-nneg); for(npos0=-(n-nneg);npos0<0&&wv[u[npos0]]==0;++npos0); npos0+=(n-nneg);  // Count +0
    u=xv+n; for(nneg0=-nneg;nneg0<0&&wv[u[nneg0]]==0;++nneg0); nneg0+=nneg;  // Count -0
    u=zv+nneg-nneg0; I ppos=0,pneg=0; DQ(npos0+nneg0, *u++=(pneg>=nneg0||(ppos<npos0&&xv[ppos]<xv[n-nneg+pneg]))?xv[ppos++]:xv[n-nneg+pneg++];)  // merge
    // Copy the positives
    ICPY(u,xv+npos0,n-nneg-npos0); //  /: copy the nonnegatives to the end of result area
    u=zv;     v=xv+n;
   }else{  // values were sorted down, & so now contain  - , -0,  +, +0
    I npos0; u=xv+nneg-1; for(npos0=n-nneg;npos0>0&&wv[u[npos0]]==0;--npos0); npos0=(n-nneg)-npos0;  // Count +0
    u=xv-1; for(nneg0=nneg;nneg0>0&&wv[u[nneg0]]==0;--nneg0); nneg0=nneg-nneg0;  // Count -0
    u=zv+n-nneg-npos0; I ppos=0,pneg=0; DQ(npos0+nneg0, *u++=(pneg>=nneg0||(ppos<npos0&&xv[n-npos0+ppos]<xv[nneg-nneg0+pneg]))?xv[n-npos0+ppos++]:xv[nneg-nneg0+pneg++];)  // merge
    // Copy the positives
    ICPY(zv,xv+nneg,n-nneg-npos0); //  /: copy the positives to the beginning of result area
    u=zv+n-nneg+nneg0;     v=xv+nneg-nneg0;
   }
// BUG: should copy down in other order to avoid pointing past valid region.  As written will run over into the shape... but that will always be positive, at least
   // copy in the negatives, reversing the order.  Here u->place to put negatives, v->last+1 negative
   // as we copy, we have to keep equal keys in the original order, i. e. don't reverse them.
   j=0; d=wv[v[-1]];
   // At start of this loop, d has the last of a set of (possibly only 1) equal values, j has the index of the output location where
   // the set will be stored.  v scans back through the negatives, looking for a new value, and when it finds one copies out all
   // the old values.
   DO(nneg-nneg0, --v; if(d!=wv[*v]){vv=1+v; m=i-j; DO(m, *u++=*vv++;); j=i; d=wv[*v];});
   // We have to push out the last set by hand because *v is outside the input area.  That wouldn't pose a problem
   // except when there is rank; then *v may be the same value as d and a change would be missed
   DQ(nneg-nneg0-j, *u++=*v++;);  // v starts pointing to first neg
  }
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on real w; main code here is for c==n */

// ai==1 and n is big enough: grade by repeated bucketsort
static GF(jtgri1){F1PREFJT;A x,y;I*wv;I i,*xv;US*u;void *yv;I c=ai*n;
 wv=AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV0(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1); yv=AV1(y);}
 // allocate ping-pong for output area
 GATV0(x,INT,n,1); xv=AV1(x);
 // for each sort...
 for(i=0;i<m;++i){I colflags;
  // process each 16-bit section of input
  u=(US*)wv+INTLSBWDX;   // point to LSB
  colflags=grcol(65536,0L,yv,n,0L,xv,sizeof(I)/sizeof(US),u,(~(I)jtinplace>>(JTDESCENDX-1))&2);  // move 'up' to bit 1
#if SY_64
  colflags=grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),u+=WDINC,colflags);
  colflags=grcol(65536,0L,yv,n,zv,xv,sizeof(I)/sizeof(US),u+=WDINC,colflags);
#endif
  grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),u+=WDINC,colflags|1);  // the 1 means 'handle sign bit'
  wv+=c; zv+=n;  // advance to next input/output area
 }
 R 1;
}    /* grade"r w on integer w where c==n */

static GF(jtgru1){F1PREFJT;A x,y;C4*wv;I i,*xv;US*u;void *yv;I c=ai*n;
 wv=C4AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV0(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1); yv=AV1(y);}
 GATV0(x,INT,n,1); xv=AV1(x);
 for(i=0;i<m;++i){I colflags;
  u=(US*)wv+INTLSBWDX;   // point to LSB
  colflags=grcol(65536,0L,yv,n,0L,xv,sizeof(C4)/sizeof(US),u,(~(I)jtinplace>>(JTDESCENDX-1))&2);  // move 'up' to bit 1
  grcol(65536,0L,yv,n,xv,zv,sizeof(C4)/sizeof(US),u+=WDINC,colflags);
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on c4t w where c==n */

#if BW==64
// grade INTs by hiding the item number in the value and sorting.  Requires ai==1.
static GF(jtgriq){F1PREFJT;
 // For stability, we keep all the interior sorts ascending.  Here we set a code to precondition the values so that comes out right
 I gradedown=REPSGN(SGNIF(jtinplace,JTDESCENDX));  // ~0 if sorting down, else 0
 // See how many bits we must reserve for the item number, and make a mask for the item number
 unsigned long hbit=CTLZI(n-1); ++hbit; I itemmask=((I)1<<hbit)-1;  // mask where the item number will go
 I itemmsb=(I)1<<(BW-1-hbit); I itemsigmsk=2*-itemmsb;  // get bit at place we will shift into sign bit, and a mask for all higher bits
 // Loop over each grade
 I *wv=IAV(w);  // we interpret the floats in w as if they were integers.
 while(--m>=0){
  // Sort one list.  zv points to the output area, wv points to the input
  // Create the values to be sorted, in the result area
  // if sorting down,complement input
  // shift value to clear space for item number; abort if that would lose significance
  // install item number
  I siglost=0;  // init to no signifiance lost
  DO(n, I v=wv[i]^gradedown; if(siglost|=itemsigmsk&((v&-itemmsb)+itemmsb))break; zv[i]=(v<<hbit)+i;)
  // If there was no loss of significance, just sort the values and return the indexes
  if(!siglost){
   vvsortqs8ai(zv,n);  // sort em (in place), ascending
   DO(n, zv[i]&=itemmask;)   // the indexes are right
  }else{
   // We encountered significance when we tried to shift the values to make room for the indexes.  We will have to sort
   // and then go back to re-sort equal partitions.  At least there is a lot of significance and probably not many collisions
   // First, install the item number in the LSBs
   DO(n, I v=wv[i]^gradedown; zv[i]=(v&~itemmask)+i;)
   // sort the result area in place
   vvsortqs8ai(zv,n);
   // pass through the result area, removing the upper bits.  If consecutive values have the same upper bits, go through them,
   // replacing the upper bits with the actual bits from the input (after honoring sign/direction bits), and then re-sort the result in place.
   // remove the upper bits from that sorted result
   I nextv=zv[0];  // always has first value with a new key
   I i;for(i=0;i<n-1;++i){
    I currv=nextv;
    if(((nextv=zv[i+1])^currv)&~itemmask){zv[i]=currv&itemmask;  // normal case with no repetition
    }else{  // reprocess the repeated block
     I j=i;do{
      I v=wv[zv[j]&itemmask]^gradedown; zv[j]=((v&itemmask)<<hbit)+(zv[j]&itemmask); // fetch original v, reconstitute; get itemmask in upper bits 
     }while(!(++j==n || (((nextv=zv[j])^currv)&~itemmask)));
     vvsortqs8ai(zv+i,j-i);  // sort the collision area in place.  j points to first item beyond the collision area, and nextv is its value
     while(i<j){zv[i]&=itemmask; ++i;}  // the item numbers are guaranteed right after this sort
     i=j-1;  // pick up after the batch
    }
   }
   // We handled all the starting items up to the next-last.  The last item may be unprocessed, if it was not part of a batch.  Handle it now.
   zv[n-1]&=itemmask;
  }
  // advance to next sort
  wv+=n; zv+= n;
 }
 R 1;
}

#endif


static GF(jtgri){F1PREFJT;A x,y;B up;I e,i,* RESTRICT v,*wv,*xv;UI4 * RESTRICT yv,* RESTRICT yvb;I c=ai*n;
 wv=AV(w);
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
 // for atoms, smallrange wins if the range is less than (2n lgn) unless that exceeds L2; then 4n.  quicksort wins otherwise, except for length 5500-500000 when there are <= two bytes of significance: then radix wins
 // for lists, smallrange always beats radix, but loses to merge if the range is too high.  We assess the max acceptable range as
 // (80>>keylength)*(n), smaller if the range would exceed cache size
 CR rng;
#if 0 // turn this on for performance measurements selecting algorithm on length
 if((n&3)==0)R gri1(m,ai,n,w,zv);  // radix
 if((n&3)==1)R grx(m,ai,n,w,zv);  // merge
 if((n&3)==2)R jtgriq(jt,m,ai,n,w,zv);  // quicksort
 // otherwise smallrange
 rng = condrange(wv,AN(w),IMAX,IMIN,IMAX);
#else
#if SY_64  // no quickgrade unless INTs are 64 bits
 if(ai==1){  // for atoms, usually use smallrange or quicksort
  if(n<10)R jtgriq(jtinplace,m,ai,n,w,zv);  // for short lists just use qsort
  UI4 lgn3=CTLZI(n); lgn3 = (UI4)((lgn3*8) - 8 + (n>>(lgn3-3)));  // approx lg(n)<<3
  rng = condrange(wv,AN(w),IMAX,IMIN,MIN((L2CACHESIZE>>LGSZI),(n*lgn3)>>(3-1)));  // let range go up to 2n lgn, but never more than L2 size
  if(!rng.range){
   if(!BETWEENC(n,5500,500000))R jtgriq(jtinplace,m,ai,n,w,zv);  // quicksort except for 5500-500000
   // in the middle range, we still use quicksort if the atoms have more than 2 bytes of significance.  We just spot-check rather than running condrange,
   // because the main appl is sorting timestamps, which are ALL big
   DO(10, if(0xffffffff00000000 & (0x0000000080000000+wv[i<<9]))R jtgriq(jtinplace,m,ai,n,w,zv);)  // quicksort if more than 2 bytes of significance, sampling the input
   R gri1(m,ai,n,w,zv);  // moderate-range middle lengths use radix sort
  }
  // fall through to small-range
 }else  // ! we already have range, don't calculate it again
#endif
 // watch out! an else clause is active
 if(ai<=6){rng = condrange(wv,AN(w),IMAX,IMIN,(MIN(((ai*n<(L2CACHESIZE>>LGSZI))?16:4),80>>ai))*n);  // test may overflow; OK   TUNE
 }else rng.range=0;  // if smallrange impossible
 // tweak this line to select path for timing
 // If there is only 1 item, radix beats merge for n>1300 or so (all positive) or more (mixed signed small numbers)
 if(!rng.range)R c==n&&n>2000?gri1(m,ai,n,w,zv):grx(m,ai,n,w,zv);  // revert to other methods if not small-range   TUNE
#endif
 // doing small-range grade.  Allocate a hashtable area.  We will access it as UI4
 GATV0(y,C4T,rng.range,1); yvb=C4AV1(y); yv=yvb-rng.min; up=(~(I)jtinplace>>JTDESCENDX)&1;
 // if there are multiple ints per item, we have to do multiple passes.  Allocate a workarea
 // should start in correct position to end in z
 if(1<ai){GATV0(x,INT,n,1); xv=AV1(x);
 }
 for(i=0;i<m;++i){  // Loop over each cell of w
  // if d>1, we will use zv and xv as alternate output pointers, leaving the final result in *zv.
  // If d is odd, we end up in the block we start in; not if d is even.  So, if d is even, we swap
  // zv and xv, to end up in zv.  xv is always defined when d is even.
  if(!(ai&1)){I *tv=zv; zv=xv; xv=tv;}  // zv<->xv
  mvc(rng.range*sizeof(UI4),yvb,MEMSET00LEN,MEMSET00);  // clear the table
  v=wv+ai-1;   // start in the lowest-significance column
  // create frequency count for each value
  DQ(n, ++yv[*v]; v+=ai;);
  // create +/\ of the counts (+/\. for grade down)
  if(up){UI4 k,s=0,*yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
  else{UI4 k,s=0;  DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
  // refetch each input and take its position from the +/\ list.  After taking a position, add 1 so the next identical value gets the next position
  // store the index of the fetched input value into the indicated position
  v=wv+ai-1;  // rescan lowest-significance column
  DO(n, zv[yv[*v]++]=i; v+=ai;);  // do the grade
  v=wv+ai-1;
  // if items contain multiple atoms, process the remanining atoms similarly, from lowest to highest significance
  for(e=ai-2;0<=e;--e){  // loop d-1 times
   // exchange xv and zv.  We will now process from xv (the previous result) and output to zv.
   {I *tv=zv; zv=xv; xv=tv;}  // zv<->xv
   --v;  // back up to next-higher-significance column
   // clear the area
   mvc(rng.range*sizeof(UI4),yvb,MEMSET00LEN,MEMSET00);
   // *v is the base of the vector to process, which has stride d.  Process the elements in the order given by the previous grade.
   {I *vv=v; DQ(n, ++yv[*vv]; vv+=ai;); }  // scan all the values
   // create running sum
   if(up){UI4 k, s=0, *yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
   else {UI4 k,s=0; DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
   // process the input atoms in the order of the grade so far, and find the result position as above.  Move the original index
   // for the item into the result
   {I *gg=xv+n; DP(n, zv[yv[v[ai*gg[i]]]++]=gg[i];);}
  }
  // At this point zv always points to the actual result area, so we can increment zv for the next run
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on integers w, checking for range */


static GF(jtgru){F1PREFJT;A x,y;B up;I e,i,*xv;UI4 *yv,*yvb;C4 *v,*wv;I c=ai*n;
 wv=C4AV(w);
 CR rng;
 if(ai<=6){rng = condrange4(wv,AN(w),-1,0,(MIN(((ai*n<(L2CACHESIZE>>LGSZI))?16:4),80>>ai))*n);   //  TUNE
 }else rng.range=0;
 if(!rng.range)R c==n&&n>1500?gru1(m,ai,n,w,zv):grx(m,ai,n,w,zv);  // revert to other methods if not small-range    TUNE
 GATV0(y,C4T,rng.range,1); yvb=C4AV1(y); yv=yvb-rng.min; up=(~(I)jtinplace>>JTDESCENDX)&1;
 if(1<ai){GATV0(x,INT,n,1); xv=AV1(x);
 }
 for(i=0;i<m;++i){
  if(!(ai&1)){I *tv=zv; zv=xv; xv=tv;}
  mvc(rng.range*sizeof(UI4),yvb,MEMSET00LEN,MEMSET00);
  v=wv+ai-1;
  DQ(n, ++yv[*v]; v+=ai;);
  if(up){UI4 k,s=0,*yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
  else{UI4 k,s=0; DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
  v=wv+ai-1;
  DO(n, zv[yv[*v]++]=i; v+=ai;);
  v=wv+ai-1;
  for(e=ai-2;0<=e;--e){
   {I *tv=zv; zv=xv; xv=tv;} 
   --v;
   mvc(rng.range*sizeof(UI4),yvb,MEMSET00LEN,MEMSET00);
   {C4 *vv=v; DQ(n, ++yv[*vv]; vv+=ai;); }
   if(up){UI4 k, s=0, *yvi = yvb+rng.range; DP(rng.range, k=yvi[i]; yvi[i]=s; s+=k;) }
   else {UI4 k,s=0; DQ(rng.range, k=yvb[i]; yvb[i]=s; s+=k;); }
   {I *gg=xv+n; DP(n, zv[yv[v[ai*gg[i]]]++]=gg[i];);}
  }
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on c4t w, checking for range */


#define DOCOL1(p,iicalc0,iicalc1,ind,vinc)  \
 {I j=p-1,k,s=0;UC*v;                          \
  if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}        \
  mvc(ps,yv,MEMSET00LEN,MEMSET00);                                    \
  v=vv; DQ(n, ++yv[iicalc0]; v+=ai;);                   \
  if(up)DO(p, k=yv[i]; yv[i  ]=s; s+=k;)               \
  else  DQ(p, k=yv[j]; yv[j--]=s; s+=k;);              \
  v=vv; DO(n, h[yv[iicalc1]++]=ind;           vinc;);  \
 }

#define DOCOL4(p,iicalc0,iicalc1,ind,vinc)  \
 {I ii,j=p-1,k,s=0;UC*v;                          \
  if(b){g=xv; h=zv; b=0;}else{g=zv; h=xv; b=1;}        \
  mvc(ps,yv,MEMSET00LEN,MEMSET00);                                    \
  v=vv; DQ(n, IND4(iicalc0); ++yv[ii]; v+=ai;);         \
  if(up)DO(p, k=yv[i]; yv[i  ]=s; s+=k;)               \
  else  DQ(p, k=yv[j]; yv[j--]=s; s+=k;);              \
  v=vv; DO(n, IND4(iicalc1); h[yv[ii]++]=ind; vinc;);  \
 }

static GF(jtgrb){F1PREFJT;A x;B b,up;I *g,*h,i,p,ps,q,*xv,yv[16];UC*vv,*wv;I c=ai*n;
 UI4 lgn=CTLZI(n);
 if((UI)ai>4*lgn)R grx(m,ai,n,w,zv);     // TUNE
 q=ai>>2; p=16; ps=p*SZI; wv=UAV(w); up=(~(I)jtinplace>>JTDESCENDX)&1;
 if(1<q){GATV0(x,INT,n,1); xv=AV1(x);}
 for(i=0;i<m;++i){
  vv=wv+ai; b=(q&1);
  if(q){   vv-=4; DOCOL4(p, *(int*)v, *(int*)v,         i,   v+=ai);}
  DO(q-1,  vv-=4; DOCOL4(p, *(int*)v, *(int*)(v+ai*g[i]),g[i],v   ););
  wv+=c; zv+=n;
 }
 R 1;
}    /* grade"r w on boolean w, works 4 columns at a time (d%4 guaranteed to be 0)*/

static GF(jtgrc){F1PREFJT;A x;B b,q,up;I *g,*h,e,i,p,ps,*xv,yv[256];UC*vv,*wv;
 UI4 lgn=CTLZI(n);
 if((UI)ai>lgn)R grx(m,ai,n,w,zv);   // TUNE
 ai<<=((AT(w)>>C2TX)&1);
 p=B01&AT(w)?2:256; ps=p*SZI; wv=UAV(w); up=(~(I)jtinplace>>JTDESCENDX)&1;
 q=C2T&AT(w) && C_LE;
 if(1<ai){GATV0(x,INT,n,1); xv=AV1(x);}
 for(i=0;i<m;++i){
  b=(B)(ai&1); if(q){e=-3; vv=wv+ai-2;}else{e=-1; vv=wv+ai-1;}
                 DOCOL1(p,*v,*v,       i,   v+=ai); if(q)e=1==e?(q==1?-3:-5):1; 
  DO(ai-1, vv+=e; DOCOL1(p,*v,v[ai*g[i]],g[i],v   ); if(q)e=1==e?(q==1?-3:-5):1;);
  wv+=ai*n; zv+=n;
 }
 R 1;
}    /* grade"r w on boolean or char or unicode w */

static GF(jtgrs){F1PREFJT;R gri(m,ai,n,sborder(w),zv);}    
     /* grade"r w on symbols w */

F2(jtgrade1p){PROLOG(0074);A x,z;I n,*s,*xv,*zv;
 RZ(x=curtail(a)); IRS2(x,w,0L,1L,1L,jtfrom,z); z=grade1(z); EPILOG(z);
 // A special sort function for this is a bad idea, because the indirection is repeated so often
}    /* /:(}:a){"1 w , permutation a, integer matrix w */


/************************************************************************/
/*                                                                      */
/* /: and \: main control                                               */
/*                                                                      */
/************************************************************************/


/* m  - # cells (# individual grades to do) */
/* c  - # atoms in a cell                   */
/* n  - length of sort axis                 */
/* w  - array to be graded                  */
/* zv - result values                       */

static B (*grroutine[])(J,I,I,I,A,I*) = {  // index is [bitx]
[B01X]=jtgrc, [LITX]=jtgrc, [INTX]=jtgri, [FLX]=jtgrd, [CMPXX]=jtgrx,[BOXX]=jtgrx, [XNUMX]=jtgrx, [RATX]=jtgrx, [QPX]=jtgrx,[C2TX]=jtgrc, [C4TX]=jtgru, [INT2X]=jtgrx, [INT4X]=jtgrx, [SBTX]=jtgrs};

// /: and \: with IRS support
A jtgr1(J jt,A w){F1PREFJT;PROLOG(0075);A z;I f,ai,m,n,r,*s,t,wn,wr,zn;
 ARGCHK1(w);
 t=AT(w); wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK;
 f=wr-r; s=AS(w);
 // Calculate m: #cells in w   n: #items in a cell of w   ai: #atoms in an item of a cell of w  c: #atoms in a cell of w  
 SETICFR(w,f,r,n);  if(wn=AN(w)){
  // If w is not empty, it must have an acceptable number of cells
  PROD(m,f,s); PROD(ai,r-1,f+s+1); zn=m*n;
 }else{
  // empty w.  The number of cells may overflow, but reshape will catch that
  DPMULDE(prod(f,s),n,zn);
 }
 // allocate the entire result area, one int per item in each input cell
 GATV0(z,INT,zn,1+f); MCISH(AS(z),s,f) if(unlikely(!r))AS(z)[f]=1;else AS(z)[f]=AS(w)[f];  // mustn't overfetch shape if r=0
 // if there are no atoms, or we are grading things with 0-1 item, return an index vector of the appropriate shape 
 if(((wn-1)|(n-2))<0)R reshape(shape(z),IX(n));
 // do the grade, using a special-case routine if possible
 RZ((t&B01&&0==(ai&3)?jtgrb:grroutine[CTTZ(t)])(jtinplace,m,ai,n,w,AV(z)))
 EPILOG(z);
}    /*   grade"r w main control for dense w */

F1(jtgrade1 ){A z; ARGCHK1(w); J jtinplace=(J)((I)jt&~JTDESCEND); if(likely(!ISSPARSE(AT(w))))RETF(gr1(w)); RETF(grd1sp(w));}
F1(jtdgrade1){A z; ARGCHK1(w); J jtinplace=(J)(((I)jt&~JTFLAGMSK)|JTDESCEND); if(likely(!ISSPARSE(AT(w))))RETF(gr1(w)); RETF(grd1sp(w));}
// Since grade2 pulls from a, mark a as non-pristine.  But since there can be no repeats, transfer a's pristinity to result if a is inplaceable
// We do this in jtgr2 because it has a branch where all boxed values go
F2(jtgrade2 ){F2PREFIP;A z; ARGCHK2(a,w); if(likely(!ISSPARSE(AT(w))))RETF(jtgr2((J)((I)jtinplace&~JTDESCEND),a,w)); RETF(jtgrd2sp((J)((I)jtinplace&~JTDESCEND),a,w));}
F2(jtdgrade2){F2PREFIP;A z; ARGCHK2(a,w); if(likely(!ISSPARSE(AT(w))))RETF(jtgr2((J)((I)jtinplace|JTDESCEND),a,w)); RETF(jtgrd2sp((J)((I)jtinplace|JTDESCEND),a,w));}


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

#define NRANDS 36  // must be even
// Partitioning function for order statistics
// j is the order desired, w is the data .  t is a temp buffer we allocate to hold the shrinking partition
#define OSLOOP(T,ATOMF)  \
{T p0,p1,q,*tv,*u,ui,uj,uk,*v,*wv;                                                     \
  tv=wv=(T*)AV(w);                                                                     \
  while(1){                                                                            \
   if(4>=n){u=tv; SORT4; R ATOMF(tv[j]);}        /* stop loop on small partition */       \
   p0=tv[(I)(qv[i]*n)]; --i;                                                  \
   p1=tv[(I)(qv[i]*n)]; --i; i=(i<0)?NRANDS-1:i; if(p0>p1){q=p0; p0=p1; p1=q;}       /* create pivots p0, p1 selected from input, with p0 <= p1  */             \
   {m0=m1=0; v=tv; DQ(n, m0+=*v<p0; m1+=*v<p1; ++v;);}  /* count m0: # < p0; and m1: # < p1  */         \
   {I l=0,h=m0; l=j>=m0?m0:l; h=j>=m0?m1:h; l=j>=m1?m1:l; h=j>=m1?n:h; m=h-l;}   /* calc size of partition holding the result */\
   if(t)u=v=tv; else{GA10(t,wt,m+1); u=tv=(T*)AV(t); v=wv;}  /* allow for 1 overstore */                            \
   if     (j<m0){       DQ(n, *u=*v; u+=*v<p0; ++v;);}                   \
   else if(j<m1 ){DQ(n, *u=*v; u+=(p0<=*v)&(*v<p1); ++v;); j-=m0;}                   \
   else if(m1   ){DQ(n, *u=*v; u+=p1<=*v; ++v;); j-=m1;}                   \
   else{DQ(n, *u=*v; u+=*v>p1; ++v;); m=u-tv; n-=m; if(j<n)R ATOMF(p1); j-=n;} /* pivots both low; use > to split */ \
   n=m; \
  }  \
 }

F2(jtordstat){A q,t=0;I j,m,m0,m1,n,wt;D *qv;
 I i=NRANDS-1;  // i points to the next random number to draw
 ARGCHK2(a,w);
 n=AN(w); wt=AT(w); RE(j=i0(a));
 if(((4-n)&((AR(a)|(1^AR(w)))-1)&(-(wt&FL+INT)))>=0)R from(a,grade2(w,w));  // if not int/float, or short, or not (atom a and list w), do full grade
 if((UI)j>=(UI)n){j+=n; ASSERT((UI)j<(UI)n,EVINDEX);}
 // deal a bunch of random floats to provide pivots.  We reuse them if needed
 RZ(q=jtrollksub(jt,sc(NRANDS),zeroionei(0))); qv=DAV(q);
 if(wt&FL)OSLOOP(D,scf) else OSLOOP(I,sc);
}    /* a{/:~w */

F2(jtordstati){A t;I n,wt;
 ARGCHK2(a,w);
 n=AN(w); wt=AT(w);
 if(((4-n)&((AR(a)|(1^AR(w)))-1)&(-(wt&FL+INT)))>=0)R from(a,grade1(w));
 RZ(t=ordstat(a,w));   // Get the value of the ath order statistic, then look up its index
 I j=0;  // =0 needed to stifle warning
 if(wt&FL){D p=DAV(t)[0],*v=DAV(w); DO(n, if(p==*v++){j=i; break;});}
 else     {I p=AV(t)[0],*v= AV(w); DO(n, if(p==*v++){j=i; break;});}
 R sc(j);
}    /* a {/:w */
