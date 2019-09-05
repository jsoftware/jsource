/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Match Associates                                                 */

#include "j.h"
#include "vcomp.h"


static B jtmatchsub(J,I,I,I,I,A,A,B* RESTRICT,B);
static F2(jtmatchs);

#define MCS(q,af,wf)  ((1<q?8:q?4:0)+(af?2:0)+(wf?1:0))
// set *x++ to b1 if *u=*v, b0 otherwise
#define QLOOP         b=b1; DO(q, if(u[i]!=v[i]){b^=1; break;}); *x++=b;
// comparison, with special cases for 1/more than 1, and looping over repeated cells
// we know there is a frame somewhere.  This is for plain byte comparison - no tolerance
#define EQV(T)        \
 {T h,* RESTRICT u=(T*)av,* RESTRICT v=(T*)wv;                                                   \
  q=k/sizeof(T);                                                             \
  switch(MCS(q,af,wf)){                                                      \
   case MCS(1,0,1): h=*u; if(b1)DQ(mn, *x++=h   ==*v++;) else DQ(mn, *x++=h   !=*v++;)  break;  \
   case MCS(1,1,0): h=*v; if(b1)DQ(mn, *x++=*u++==h;   ) else DQ(mn, *x++=*u++!=h;   ); break;  \
   case MCS(1,1,1): if(b1){                                                  \
                     if(1==n)      DQ(m,               *x++=*u++==*v++;  )   \
                     else if(af<wf)DQ(m, h=*u++; DQ(n, *x++=h   ==*v++;);)   \
                     else          DQ(m, h=*v++; DQ(n, *x++=*u++==h;   ););  \
                    }else{                                                   \
                     if(1==n)      DQ(m,               *x++=*u++!=*v++;  )   \
                     else if(af<wf)DQ(m, h=*u++; DQ(n, *x++=h   !=*v++;);)   \
                     else          DQ(m, h=*v++; DQ(n, *x++=*u++!=h;   ););  \
                    } break;                                                 \
   case MCS(2,0,1): DQ(mn, QLOOP;       v+=q;); break;                       \
   case MCS(2,1,0): DQ(mn, QLOOP; u+=q;      ); break;                       \
   case MCS(2,1,1): if(1==n)      DQ(m,       QLOOP; u+=q;   v+=q;)          \
                    else if(af<wf)DQ(m, DQ(n, QLOOP; v+=q;); u+=q;)          \
                    else          DQ(m, DQ(n, QLOOP; u+=q;); v+=q;); break;  \
 }}

// comparison function for non-float arrays, in chunks of size k, storing match results into *x and
// returning the first one as the result (nothing special about first, could be any one)
// This is a generalized version of the INNERT macro below
// If we get here, we know that at least one of the arguments has a frame
static B eqv(I af,I wf,I m,I n,I k,C*av,C*wv,B* RESTRICT x,B b1){B b,* RESTRICT xx=x;I mn=m*n,q;
  // select a comparison loop based on the size of the data area.  It's all about the fastest way to compare bytes
 if     (0==(k&(SZI-1))  )EQV(I)
#if SY_64
 else if(0==(k&(SZI4-1)))EQV(int)
#endif
 else if(0==(k&(SZS-1))  )EQV(S)
 else if(1==k)            EQV(C)
 else{
  if(af<wf)DQ(m, DQ(n, *x++=(!!memcmp(av,wv,k))^b1; wv+=k;); av+=k;)
  else          DQ(m, DQ(n, *x++=(!!memcmp(av,wv,k))^b1; av+=k;); wv+=k;);
 }
 R xx[0];
}    /* what memcmp should have been */

// Return 1 if a and w match, 0 if not
B jtequ(J jt,A a,A w){A x;
 RZ(a&&w);F2PREFIP;  // allow inplace request - it has no effect
 if(a==w)R 1;
 if(SPARSE&(AT(a)|AT(w))&&AR(a)&&AR(w)){RZ(x=matchs(a,w)); R*BAV(x);}
 R matchsub(0L,0L,1L,1L,a,w,0,C1);  // don't check level - it takes too long for big arrays
}

// Return 1 if a and w match intolerantly, 0 if not
B jtequ0(J jt,A a,A w){
 F2PREFIP;  // allow inplace request - it has no effect
 PUSHCCT(1.0) B res=equ(a,w); POPCCT R res;
}

// Test for equality of functions, 1 if they match.  To match, the functions must have the same pseudocharacter and fgh
static B jteqf(J jt,A a,A w){A p,q;V*u=FAV(a),*v=FAV(w);
 if(!(TYPESEQ(AT(a),AT(w))&&u->id==v->id))R 0;
 p=u->fgh[0]; q=v->fgh[0]; if(!(!p&&!q||p&&q&&(p==q||matchsub(0L,0L,1L,1L,p,q,0,C1))))R 0;
 p=u->fgh[1]; q=v->fgh[1]; if(!(!p&&!q||p&&q&&(p==q||matchsub(0L,0L,1L,1L,p,q,0,C1))))R 0;
 p=u->fgh[2]; q=v->fgh[2];    R !p&&!q||p&&q&&(p==q||matchsub(0L,0L,1L,1L,p,q,0,C1));
}

// compare function for boxes.  Do a test on the single contents of the box.  Reset comparison direction to normal.
#define EQA(a,w)  (a==w||matchsub(0L,0L,1L,1L,a,w,0,C1))
// compare rationals
#define EQQ(a,w)  (equ(a.n,w.n)&&equ(a.d,w.d))

// compare arrays for equality of all values.  f is the compare function
// m=#cells of shorter frame, n=#times a cell of shorter frame must be repeated
// x[] is result array.  This can be 0 if we are doing a comparison inside a box, in which case
// we don't store the result.  In any case, b holds the result of the last comparison
#define INNERT(T,f)                  \
 {T* RESTRICT u=(T*)av,* RESTRICT v=(T*)wv;   /* u->a data, v->w data */           \
  if(1==n)      DO(m,       b=b1; DO(c, if(!f(u[i],v[i])){b^=1; break;}); if(x)x[j++]=b; u+=c; v+=c;)  \
  else if(af>wf)DO(m, DO(n, b=b1; DO(c, if(!f(u[i],v[i])){b^=1; break;}); x[j++]=b; u+=c;); v+=c;)  \
  else          DO(m, DO(n, b=b1; DO(c, if(!f(u[i],v[i])){b^=1; break;}); x[j++]=b; v+=c;); u+=c;)  \
 }

// compare functions for float/complex intolerant comparison
#define DEQCT0(a,w) ((a)==(w))
#define ZEQCT0(a,w) ( ((a).re==(w).re) && ((a).im==(w).im) )

// match two values, returning b1 if match, 1^b1 if no match.  If the values are functions, that's all we return.  If the values are nouns, we
// store the match value(s) in *x.  x may be 0, if af and wf are 0 and m and n are 1.  In this case we don't store anything.
// but return the match status.  We use this when comparing boxed arrays or functions
// b1 is the value to use for 'match' - 1 normally, but 0 for top level of -.@-:
// m=#cells of shorter frame, n=#times a cell of shorter frame must be repeated
// the comparands may not be sparse
static B jtmatchsub(J jt,I af,I wf,I m,I n,A a,A w,B* RESTRICT x,B b1){B b;C*av,*wv;I at,c,j=0,p,q,t,wt;
 // we tested for a==w before the call, to save on call overhead
 // m*n cannot be 0.  If this is a recursive call, m=n=1; while if it is the first call, empty m/n were handled at the top level
 p=AR(a)-af; at=UNSAFE(AT(a));
 q=AR(w)-wf; wt=UNSAFE(AT(w)); 
 // p=cell-rank of a; q=cell-rank of w; ?t=type;
 // c=#atoms in a cell, b is 1 if rank or cell-shape mismatches, or if cells are not empty and types are incompatible
 // We know that either there is no frame or both arguments are nonempty (Empty arguments with frame can happen only at the top level
 // and were handled there).
 PROD(c,p,af+AS(a)); b=p!=q||ICMP(af+AS(a),wf+AS(w),p)||c&&!HOMO(at,wt);
 // If we know the result - either they mismatch, or the cell is empty, or the buffers are identical - return all success/failure
 if(b||!c||a==w){if(x)memset(x,b^b1,m*n); R b^b1;}
 // If we're comparing functions, return that result
 t=at;  //  in case types identical, pick one
 if(t&FUNC)R (!eqf(a,w))^b1;  // true value, but switch if return is not 'match'
 // If the types mismatch, convert as needed to the common (unsafe) type calculated earlier
 if(at!=wt) {
  t=maxtypedne(at,wt);
  if(at!=t)RZ(a=t&XNUM?xcvt(XMEXMT,a):cvt(t,a));
  if(wt!=t)RZ(w=t&XNUM?xcvt(XMEXMT,w):cvt(t,w));
 }
 // If a has no frame, it might be the shorter frame and therefore repeated; but in that case
 // m will be 1 (1 cell in shorter frame).  So it is safe to increment each address by c in the compare loops
 av=CAV(a);
 wv=CAV(w);
 // do the comparison, leaving the last result in b
 switch(CTTZ(t)){
  // Take the case of no frame quickly, because it happens on each recursion and also in much user code
 default:   c <<= bplg(t); if(af|wf){b = eqv(af,wf,m,n,c,av,wv,x,b1);}else{b = (!!memcmp(av,wv,c))^b1; if(x)x[0]=b;} break; // change c to number of bytes in cell
 case FLX:   if(1.0!=jt->cct)INNERT(D,TEQ)else INNERT(D,DEQCT0) break;
 case CMPXX: if(1.0!=jt->cct)INNERT(Z,zeq)else INNERT(Z,ZEQCT0) break;
 case XNUMX: INNERT(X,equ); break;
 case RATX:  INNERT(Q,EQQ); break;
 case BOXX:
   INNERT(A,EQA); break;
 } R b;
}

static F2(jtmatchs){A ae,ax,p,q,we,wx,x;B*b,*pv,*qv;D d;I acr,an=0,ar,c,j,k,m,n,r,*s,*v,wcr,wn=0,wr;P*ap,*wp;
 RZ(a&&w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; r=ar;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; RESETRANK;
 if(ar>acr||wr>wcr)R rank2ex(a,w,0L,acr,wcr,acr,wcr,jtmatchs);
 if(ar!=wr||memcmp(AS(a),AS(w),r*SZI)||!HOMO(AT(a),AT(w)))R num[0];
 GATV0(x,B01,r,1L); b=BAV(x); memset(b,C0,r);
 if(SPARSE&AT(a)){ap=PAV(a); x=SPA(ap,a); v=AV(x); an=AN(x); DO(an, b[v[i]]=1;);}
 if(SPARSE&AT(w)){wp=PAV(w); x=SPA(wp,a); v=AV(x); wn=AN(x); DO(wn, b[v[i]]=1;);} 
 c=0; DO(r, c+=b[i];);
 if(an<c||DENSE&AT(a))RZ(a=reaxis(ifb(r,b),a)); ap=PAV(a); ae=SPA(ap,e); ax=SPA(ap,x); m=*AS(ax);
 if(wn<c||DENSE&AT(w))RZ(w=reaxis(ifb(r,b),w)); wp=PAV(w); we=SPA(wp,e); wx=SPA(wp,x); n=*AS(wx);
 RZ(x=indexof(SPA(ap,i),SPA(wp,i))); v=AV(x);
 GATV0(p,B01,m,1); pv=BAV(p);
 GATV0(q,B01,n,1); qv=BAV(q); 
 memset(pv,C1,m); DO(n, j=*v++; if(j<m)pv[j]=qv[i]=0; else qv[i]=1;);
 if(memchr(pv,C1,m)&&!all1(eq(we,repeat(p,ax))))R num[0];
 if(memchr(qv,C1,n)&&!all1(eq(ae,repeat(q,wx))))R num[0];
 j=0; DO(m, if(pv[i])++j;);
 k=0; DO(n, if(qv[i])++k; qv[i]=!qv[i];);
 if(!equ(from(repeat(q,x),ax),repeat(q,wx)))R num[0];
 x=SPA(ap,a); v=AV(x); s=AS(a); d=1.0; DO(AN(x), d*=s[v[i]];);
 R d==m+k&&d==n+j||equ(ae,we)?num[1]:num[0];
}    /* a -:"r w on sparse arrays */


F2(jtmatch){A z;I af,f,m,n,mn,*s,wf;
 RZ(a&&w);
 if(SPARSE&(AT(a)|AT(w)))R matchs(a,w);
 af=AR(a)-(I)(jt->ranks>>RANKTX); af=af<0?0:af; wf=AR(w)-(I)((RANKT)jt->ranks); wf=wf<0?0:wf; RESETRANK;
 // If either operand is empty return without any comparisons.  In this case we have to worry that the
 // number of cells may overflow, even if there are no atoms
 if(!AN(a)||!AN(w)){B b; I p;
  // no atoms.  The shape of the result is the length of the longer frame.  See how many cells that is
  if(af>wf){f=af; s=AS(a); RE(mn = prod(af,AS(a)));}else{f=wf; s=AS(w); RE(mn = prod(wf,AS(w)));}
  // The result for each cell is 1 if the cell-shapes are the same
  p=AR(a)-af; b=p==(AR(w)-wf)&&!ICMP(af+AS(a),wf+AS(w),p);   // b =  shapes are the same
  // Allocate & return result
  GATV(z,B01,mn,f,s); memset(BAV(z),b,mn); R z;
 }
 // There are atoms.
 // Create m: #cells in shorter (i. e. common) frame  n: # times cell of shorter frame is repeated
 if(af>wf){f=af; s=AS(a); PROD(m,wf,s); PROD(n,af-wf,wf+s);}
 else     {f=wf; s=AS(w); PROD(m,af,s); PROD(n,wf-af,af+s);}
 mn=m*n;  // total number of matches to do, i. e. # results
 GATV(z,B01,mn,f,s); matchsub(af,wf,m,n,a,w,BAV(z),C1);
 RETF(z);
}    /* a -:"r w */

F2(jtnotmatch){A z;I af,f,m,n,mn,*s,wf;
 RZ(a&&w);
 if(SPARSE&(AT(a)|AT(w)))R matchs(a,w);
 af=AR(a)-(I)(jt->ranks>>RANKTX); af=af<0?0:af; wf=AR(w)-(I)((RANKT)jt->ranks); wf=wf<0?0:wf; RESETRANK;
 // If either operand is empty return without any comparisons.  In this case we have to worry that the
 // number of cells may overflow, even if there are no atoms
 if(!AN(a)||!AN(w)){B b; I p;
  // no atoms.  The shape of the result is the length of the longer frame.  See how many cells that is
  if(af>wf){f=af; s=AS(a); RE(mn = prod(af,AS(a)));}else{f=wf; s=AS(w); RE(mn = prod(wf,AS(w)));}
  // The result for each cell is 1 if the cell-shapes are the same
  p=AR(a)-af; b=!(p==(AR(w)-wf)&&!ICMP(af+AS(a),wf+AS(w),p));   // b =  shapes are the same
  // Allocate & return result
  GATV(z,B01,mn,f,s); memset(BAV(z),b,mn); R z;
 }
 // There are atoms.
 // Create m: #cells in shorter (i. e. common) frame  n: # times cell of shorter frame is repeated
 if(af>wf){f=af; s=AS(a); PROD(m,wf,s); PROD(n,af-wf,wf+s);}
 else     {f=wf; s=AS(w); PROD(m,af,s); PROD(n,wf-af,af+s);}
 mn=m*n;  // total number of matches to do, i. e. # results
 GATV(z,B01,mn,f,s); matchsub(af,wf,m,n,a,w,BAV(z),C0);
 RETF(z);
}    /* a -.@-:"r w */
