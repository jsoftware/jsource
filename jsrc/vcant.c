/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Transpose                                                        */

#include "j.h"

static F2(jtcanta);

static A jtcants(J jt,A a,A w,A z){A a1,q,y;B*b,*c;I*u,wr,zr;P*wp,*zp;
 RZ(a&&w&&z);
 RZ(a=grade1(a));
 wr=AR(w); wp=PAV(w); a1=SPA(wp,a);
 zr=AR(z); zp=PAV(z);
 ASSERT(wr==zr,EVNONCE);
 RZ(b=bfi(wr,a1,1));
 GATV(q,B01,wr,1,0); c=BAV(q); u=AV(a); DO(wr, c[i]=b[u[i]];);
 SPB(zp,a,ifb(wr,c));
 SPB(zp,e,ca(SPA(wp,e)));
 RZ(y=fromr(grade1(indexof(a,a1)),SPA(wp,i)));
 RZ(q=grade1(y));
 SPB(zp,i,from(q,y));
 SPB(zp,x,from(q,canta(over(zero,increm(grade1(less(a,a1)))),SPA(wp,x))));
 R z;
}    /* w is sparse */

#if 0  // obsolete
#define CANTA(T,exp)  \
 {T*u=(T*)zv,*v=(T*)wv;                                                  \
  DO(zn, exp; j=r-1; ++tv[j]; d+=mv[j];                                  \
  while(j&&sv[j]==tv[j]){d+=mv[j-1]-mv[j]*sv[j]; tv[j]=0; ++tv[--j];});  \
 }
// do the innermost loop fast.  r must be >= 1
#define CANTA(T,exp)  \
 {T*u=(T*)zv,*v=(T*)wv;                                                  \
  do{j = r-1; I mvr1=mv[j]; DQ(sv[j], exp; d+=mvr1;)                        \
   do{d-=mv[j]*sv[j]; tv[j]=0; --j; if(j<0)break; d+=mv[j]; ++tv[j];}while(sv[j]==tv[j]);  \
  }while(j>=0); \
 }
#else
// do the innermost loop fast.  r must be >= 1
// we create result items in order.  tv[] is the representation in the index space of the result.  WWhen
// we move a cell we increment the low-order index and propagate carries up the line.  Each entry of mv[] tells
// how many bytes to move the input pointer for a move of 1 in the result axis
#define CANTA(T,exp)  \
 {T*u=(T*)zv; C*v=(C*)wv;                                                  \
  do{j = r-1; I mvr1=mv[j]; DQ(sv[j], exp; v+=mvr1;)                        \
   do{v-=mv[j]*sv[j]; tv[j]=0; --j; if(j<0)break; v+=mv[j]; ++tv[j];}while(sv[j]==tv[j]);  \
  }while(j>=0); \
 }
#endif

// a[i] is the axis of the result that axis i of w contributes to
// This is the inverse permutation of the x in x |: y
// This routine handles IRS on w only (by making higher axes passthroughs), and ignores the rank of a (assumes 1)
static F2(jtcanta){A m,s,t,z;B b;C*wv,*zv;I*av,j,*mv,r,*sv,*tv,wf,wr,*ws,zn,zr;
 RZ(a&&w);
// obsolete  av=AV(a); ws=AS(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; RESETRANK;
 av=AV(a); ws=AS(w); wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK;
 ASSERT(r==AN(a),EVLENGTH);
 if(wf=wr-r){  // if |:"r, handle the rank by prefixing a with leading axes 0 1 2...
  GATV(a,INT,wr,1,0); tv=AV(a); 
  DO(wf, tv[i]=i;); DO(r, tv[wf+i]=wf+av[i];);  // adjust other axes up to move out of the way of the prefix axes
  av=tv;
 }
 zr=-1; DO(wr, zr=MAX(zr,av[i]);); ++zr;  // zr = result rank: largest axis number in a + 1 (0 if a is atomic)
 GATV(m,INT,zr,1,0); mv=AV(m);  // mv[i] is distance (in cells) to move in w corresponding to move of 1 in axis i of the result
 GATV(s,INT,zr,1,0); sv=AV(s);  // sv[i] is the length of axis i in the result
 GATV(t,INT,wr,1,0); tv=AV(t);  // tv[i] starts as # atoms in an i-cell of w
 // calculate */\. ws, and simultaneously discard trailing axes that are unchanged in the transpose.  We detect these only
 // for transposes that do not include axes run together, i. e. a contains all the indexes.  [If there is an axis run together we
 // would have trouble deciding whether it could be elided.]  If a trailing axis can be discarded,
 // we include it in the cell size but not in the number of cells for the axis; and we reduce the ranks.  If there are axes run together,
 // ipso facto the largest number in a must be less than the (length of a)-1, and this loop aborts immediately.
 // r will hold number of unelided trailing axes of result
 I noelideend=0; I cellsizeb=bp(AT(w)); r=zr; I scanws=1; j=wr;  // cellsizeb is number of bytes in a cell of the transpose, after deleting trailing axes
 DO(wr, --j; tv[j]=scanws; if(noelideend|=(j^av[j])){scanws*=ws[j];}else{cellsizeb*=ws[j]; --r;});  // tv = */\. ws
// obsolete  d=1; j=wr; DO(wr, --j; tv[j]=d; d*=ws[j];);  // tv = */\. ws
 if(!r)R RETARG(w);  // if all the axes are elided, just return the input unchanged
 for(j=0,zn=1;j<zr;++j){  // for each axis of the result...  (must include deleted axes to get the shape of result axis, and total # items)
  UI axislenres=~0; I axislenin=0;  // axislenin will hold length of axis (in the input), axislenres is length of axis in result
  // look at all input axes, and accumulate info for each one that matches the result axis we are working on.
  // we are looking for the length of the axis and the stride between successive elements along that axis.
  // if there are multiple matches, those axes are run together: we take the stride in both, and limit the length to that of the shortest axis
  DO(wr, if(j==av[i]){axislenin+=tv[i]; axislenres=((UI)ws[i]<axislenres)?ws[i]:axislenres;});  // this includes deleted trailing axes of w, because we need the entire shape in zn/sv (mv doesn't matter)
  ASSERT(axislenres!=~0,EVINDEX);  // abort if there is no input axis as source for this result axis
  zn*=(I)axislenres; sv[j]=(I)axislenres; mv[j]=axislenin*cellsizeb;  // accumulate result: axis len multiplies * cells, smallest goes to sv, total stride to mv
 }
 b=1&&SPARSE&AT(w);
 GA(z,AT(w),b?1:zn,zr,sv);  // allocate result
 if(b)R cants(a,w,z); if(!zn)R z;  // if sparse, go to sparse transpose code.  If result is empty, return it now
// obsolete  d=1; r=zr; j=wr; DO(wr, --j; if(j!=av[j])break; d*=sv[j]; --r;);  // collect trailing unmodified axes into the result cell-size.
// obsolete  if(1<d)DO(r, mv[i]/=d;);  // if cell-size has increased, reduce movement vector accordingly, since it counts in cells (questionable decision)
 // now run the transpose
 zv=CAV(z); wv=CAV(w);
// obsolete if(r){
 memset(tv,C0,r*SZI);  // repurpose tv to be the index list of the input pointer, and set to 0s.  Only the first r axes matter
 switch(cellsizeb){
 case sizeof(C): CANTA(C, *u++=*(C*)v;); break;
 case sizeof(S): CANTA(S, *u++=*(S*)v;); break;
#if SY_64
 case sizeof(I4): CANTA(I4, *u++=*(I4*)v;); break;
#endif
 case sizeof(I): CANTA(I, *u++=*(I*)v;); break;
#if !SY_64 && SY_WIN32
 case sizeof(D): if(AT(w)&FL){CANTA(D, *u++=*(D*)v;); break;}
   // move as D type only if echt floats - otherwise they get corrupted.  If not float, fall through to...
#endif
 default:        CANTA(C, MC(u,v,cellsizeb); u+=cellsizeb;); break;
 }     
// obsolete }else{MC(zv,wv,c*zn);}  // could return w
 RELOCATE(w,z); RETF(z);  // should EPILOG?
}    /* dyadic transpose in APL\360, a f"(1,r) w where 1>:#$a  */

F1(jtcant1){I r; 
 RZ(w); 
// obsolete  if(jt->rank){jt->rank[0]=1; r=jt->rank[1];}else r=AR(w); 
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r;   // no RESETRANK; we pass the rank of w on
 A z=canta(apv(r,r-1,-1L),w);  // rank is set
 RZ(z);  INHERITNOREL(z,w); RETF(z);
}    /* |:"r w */

F2(jtcant2){A*av,p,t,y;I j,k,m,n,*pv,q,r,*v;
 RZ(a&&w);
// obsolete  q=jt->rank?jt->rank[0]:AR(a); 
// obsolete  r=jt->rank?jt->rank[1]:AR(w); RESETRANK;
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r; 
 q=jt->ranks>>RANKTX; q=AR(a)<q?AR(a):q; RESETRANK;
 if(1<q||q<AR(a))R rank2ex(a,w,0L,1,RMAX,q,r,jtcant2);
 if(BOX&AT(a)){
  RZ(y=pfill(r,t=raze(a))); v=AV(y);
  GATV(p,INT,AN(y),1,0); pv=AV(p);
  m=AN(a); n=AN(t); av=AAV(a); RELBASEASGN(a,a);
  j=0; DO(r-n,pv[*v++]=j++;); DO(m, k=AN(AVR(i)); DO(k,pv[*v++]=j;); if(k)++j;);
 }else p=pinv(pfill(r,a));
 A z= r<AR(w) ? irs2(p,w,0L,1L,r,jtcanta) : canta(p,w);
 RZ(z);  INHERITNOREL(z,w); RETF(z);
}    /* a|:"r w main control */ 
