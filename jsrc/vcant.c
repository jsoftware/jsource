/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Transpose                                                        */

#include "j.h"

static F2(jtcanta);

static A jtcants(J jt,A a,A w,A z){A a1,q,y;B*b,*c;I*u,wr,zr;P*wp,*zp;
 ARGCHK3(a,w,z);
 RZ(a=grade1(a));
 wr=AR(w); wp=PAV(w); a1=SPA(wp,a);
 zr=AR(z); zp=PAV(z);
 ASSERT(wr==zr,EVNONCE);
 RZ(b=bfi(wr,a1,1));
 GATV0(q,B01,wr,1); c=BAV1(q); u=AV(a); DO(wr, c[i]=b[u[i]];);
 A bvec=ifb(wr,c); makewritable(bvec)
 SPB(zp,a,bvec);  // avoid readonly
 SPB(zp,e,ca(SPA(wp,e)));
 RZ(y=fromr(grade1(indexof(a,a1)),SPA(wp,i)));
 RZ(q=grade1(y));
 SPB(zp,i,from(q,y));
 SPB(zp,x,from(q,canta(over(zeroionei(0),increm(grade1(less(a,a1)))),SPA(wp,x))));
 R z;
}    /* w is sparse */

// do the innermost loop fast.  r must be >= 1
// we create result items in order.  tv[] is the representation in the index space of the result.  When
// we move a cell we increment the low-order index and propagate carries up the line.  Each entry of mv[] tells
// how many bytes to move the input pointer for a move of 1 in the result axis
// exp moves one cell into the result area with postincrement, e. g. *u++=*(C*)v;
#define CANTA(T,exp)  \
 {T*u=(T*)zv; C*v=(C*)wv;                                                  \
  NOUNROLL do{j = r-1; I mvr1=mv[j]; DQ(sv[j], exp; v+=mvr1;)                        \
   NOUNROLL do{v-=mv[j]*sv[j]; tv[j]=0; --j; if(j<0)break; v+=mv[j]; ++tv[j];}while(sv[j]==tv[j]);  \
  }while(j>=0); \
 }
#define CANTB(T,exp)  \
 {T*u=(T*)zv; C*v=(C*)wv; I v1=mv[r-1]; I v3=3*v1; I dofst=-sv[r-1]&3; UI dlct=(sv[r-1]+3)>>2;  \
  NOUNROLL do{UI dlct0=dlct; v-=dofst*v1; u-=dofst; switch(dofst){do{case 0: u[0]=*(T*)v; case 1: u[1]=*(T*)(v+v1); case 2: u[2]=*(T*)(v+2*v1); case 3: u[3]=*(T*)(v+v3); v+=4*v1; u+=4; }while(--dlct0);}                        \
   j = r-1; NOUNROLL do{v-=mv[j]*sv[j]; tv[j]=0; --j; if(j<0)break; v+=mv[j]; ++tv[j];}while(sv[j]==tv[j]);  \
  }while(j>=0); \
 }

// a[i] is the axis of the result that axis i of w contributes to - known to be valid and INT type
// This is the inverse permutation of the x in x |: y
// This routine handles IRS on w only (by making higher axes passthroughs), and ignores the rank of a (assumes 1)
static F2(jtcanta){A m,s,t,z;C*wv,*zv;I*av,j,*mv,r,*sv,*tv,wf,wr,*ws,zn,zr,ms[4],ss[4],ts[4];
 ARGCHK2(a,w);
 av=AV(a); ws=AS(w); wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK;
 ASSERT(r==AN(a),EVLENGTH);
 fauxblockINT(afaux,4,1);
 if(wf=wr-r){  // if |:"r, handle the rank by prefixing a with leading axes 0 1 2...
  fauxINT(a,afaux,wr,1) tv=AV(a); 
  DO(wf, tv[i]=i;); DO(r, tv[wf+i]=wf+av[i];);  // adjust other axes up to move out of the way of the prefix axes
  av=tv;
 }
 zr=-1; DO(wr, zr=MAX(zr,av[i]);); ++zr;  // zr = result rank: largest axis number in a + 1 (0 if a is atomic)
 if((zr|wr)<=(I)(sizeof(ms)/sizeof(ms[0])))mv=ms, sv=ss, tv=ts;
 else{  // if rank of array is large, allocate space for rank.  Otherwise use stack areas
  GATV0(m,INT,zr,1); mv=AV1(m);  // mv[i] is distance (in cells) to move in w corresponding to move of 1 in axis i of the result
  GATV0(s,INT,zr,1); sv=AV1(s);  // sv[i] is the length of axis i in the result
  GATV0(t,INT,wr,1); tv=AV1(t);  // tv[i] starts as # atoms in an i-cell of w
 }
 // calculate */\. ws, and simultaneously discard trailing axes that are unchanged in the transpose.  We detect these only
 // for transposes that do not include axes run together, i. e. a contains all the indexes.  [If there is an axis run together we
 // would have trouble deciding whether it could be elided.]  If a trailing axis can be discarded,
 // we include it in the cell size but not in the number of cells for the axis; and we reduce the ranks.  If there are axes run together,
 // ipso facto the largest number in a must be less than the (length of a)-1, and this loop aborts immediately.
 // r will hold number of unelided trailing axes of result
 I noelideend=0; I cellsizeb=bpnoun(AT(w)); r=zr; I scanws=1; j=wr;  // cellsizeb is number of bytes in a cell of the transpose, after deleting trailing axes
 DQ(wr, --j; tv[j]=scanws; if(noelideend|=(j^av[j])){scanws*=ws[j];}else{cellsizeb*=ws[j]; --r;});  // tv = */\. ws
 if(!r)R RETARG(w);  // if all the axes are elided, just return the input unchanged
 for(j=0,zn=1;j<zr;++j){  // for each axis of the result...  (must include deleted axes to get the shape of result axis, and total # items)
  UI axislenres=~0; I axislenin=0;  // axislenin will hold length of axis (in the input), axislenres is length of axis in result
  // look at all input axes, and accumulate info for each one that matches the result axis we are working on.
  // we are looking for the length of the axis and the stride between successive elements along that axis.
  // if there are multiple matches, those axes are run together: we take the stride in both, and limit the length to that of the shortest axis
  DO(wr, if(j==av[i]){axislenin+=tv[i]; axislenres=((UI)ws[i]<axislenres)?ws[i]:axislenres;});  // this includes deleted trailing axes of w, because we need the entire shape in zn/sv (mv doesn't matter)
  ASSERT(axislenres!=~0UL,EVINDEX);  // abort if there is no input axis as source for this result axis
  zn*=(I)axislenres; sv[j]=(I)axislenres; mv[j]=axislenin*cellsizeb;  // accumulate result: axis len multiplies * cells, smallest goes to sv, total stride to mv
 }
 if(unlikely(ISSPARSE(AT(w)))){GASPARSE(z,AT(w),1,zr,sv); R cants(a,w,z);}  // if sparse, go to sparse transpose code.
 GA(z,AT(w),zn,zr,sv); if(!zn)R z;  // allocate result.  If result is empty, return it now
 // now run the transpose
 zv=CAVn(zr,z); wv=CAV(w);
 mvc(r*SZI,tv,MEMSET00LEN,MEMSET00);  // repurpose tv to be the index list of the input pointer, and set to 0s.  Only the first r axes matter
 switch(cellsizeb){
 case sizeof(I): CANTB(I, *u++=*(I*)v;); break;
 case sizeof(C): CANTB(C, *u++=*(C*)v;); break;
 case sizeof(S): CANTB(S, *u++=*(S*)v;); break;
#if SY_64
 case sizeof(I4): CANTB(I4, *u++=*(I4*)v;); break;
#endif
#if !SY_64 && SY_WIN32
 case sizeof(D): if(AT(w)&FL){CANTB(D, *u++=*(D*)v;); break;}
   // move as D type only if echt floats - otherwise they get corrupted.  If not float, fall through to...
#endif
 default:        CANTA(C, MC(u,v,cellsizeb); u+=cellsizeb;); break;
 }     
 RETF(z);
}    /* dyadic transpose in APL\360, a f"(1,r) w where 1>:#$a  */

DF1(jtcant1){I r; A z;
 F1PREFIP;ARGCHK1(w);
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r;   // no RESETRANK; we pass the rank of w on
#if 0  //  C_AVX2 || EMU_AVX2
 // This attempt to transpose in 4x4 blocks fails because the stores are very slow if not aligned, and cannot be aligned if the array has odd size.
 // It took 2.6 clocks/atom (instead of the expected 0.5) because of the store penalties
 if((-((r^2)|(AR(w)^2))|(AT(w)&(SPARSE|INT|FL)))>0){  // rank 2, FL/INT, not sparse
  // 2D transpose.  Do it 4x4 at a time with AVX instructions
  I m=AS(w)[0], n=AS(w)[1];  // m=# columns, n=#rows
  I m4=(m&-4)>>LGNPAR;   // m4=number of 4-blocks in the vertical stripes
  GATV0(z,INT,AN(w),2) AT(z)=AT(w); AS(z)[0]=n; AS(z)[1]=m;  // allocate result
  // We do vertical stripes left to right, finishing with the <4xn horizontal stripe at the bottom
  // vertical stripes.  Do 4x4 till the end, then the remnant
  UI n0;  // n0=number of columns remaining
  // constants for addressing row & columns
  I wstride1=(n&1)*SZI, wstride3=3*wstride1;  // stride between rows of w
  I zstride1=(m&1)*SZI, zstride3=3*zstride1;  // stride between rows of z
  void*wv=DAV(w);
  void*zv=DAV2(z);  // pointers to beginning of result area
  void *wv0=wv, *zv0=zv;  // running pointers to beginning of stripe
  if(likely(m4!=0)){  // if there are blocks to do...
   NOUNROLL for(n0=n;n0>=4;n0-=4){  // for each full vertical strip
    // wv0 and zv0 point to the starting location of the strip, input and output
    UI blkrem=m4;
    NOUNROLL do{
     __m256d w0,w1,w2,w3,w0t,w2t,wb;
     w0=_mm256_loadu_pd((D*)wv0); w1=_mm256_loadu_pd((D*)((I)wv0+wstride1)); w2=_mm256_loadu_pd((D*)((I)wv0+2*wstride1)); w3=_mm256_loadu_pd((D*)((I)wv0+wstride3));
     w0t=_mm256_unpacklo_pd(w0,w1); w2t=_mm256_unpacklo_pd(w2,w3);  // 0426, 8CAE
     wb=_mm256_blend_pd(w0t,w2t,0b0011); wb=_mm256_permute2f128_pd(wb,wb,0x01);  // 8C26, 268C
     w1=_mm256_unpackhi_pd(w0,w1); w3=_mm256_unpackhi_pd(w2,w3);  // 1537, 9DBF
     w0=_mm256_blend_pd(w0t,wb,0b1100); w2=_mm256_blend_pd(w2t,wb,0b0011);  // 048C, 26AE
     _mm256_storeu_pd((D*)zv0, w0); _mm256_storeu_pd((D*)((C*)zv0+2*zstride1), w2);
     wb=_mm256_blend_pd(w1,w3,0b0011); wb=_mm256_permute2f128_pd(wb,wb,0x01);  // 9D37, 379D
     w1=_mm256_blend_pd(w1,wb,0b1100); w3=_mm256_blend_pd(w3,wb,0b0011);  // 159D, 37BF
     _mm256_storeu_pd((D*)((C*)zv0+zstride1),w1); _mm256_storeu_pd((D*)((C*)zv0+zstride3), w3);
     wv0=(C*)wv0+(wstride1<<LGNPAR); zv0=(C*)zv0+SZI*NPAR;  // next input & output 4x4
    }while(--blkrem);
    // back up to start of strip, then advance to next position
    wv0=(C*)wv0-wstride1*m4*NPAR+SZI*NPAR;    // next vertical input strip
    zv0=(C*)zv0-m4*SZI*NPAR+zstride1*NPAR;  // next horizontal output strip
   }
   // wv0 and zv0 are pointing to the next input/output, but it is not 4 wide.
   if(n0>=2){
    // the remnant is 2-3 columns.  Process like a normal column, but shorten the read/write
    // use mask loads
    __m256i endmask= _mm256_loadu_si256((__m256i*)(validitymask+NPAR-n0));
    UI blkrem=m4;
    NOUNROLL do{
     __m256d w0,w1,w2,w3,w0t,w2t,wb;
     w0=_mm256_maskload_pd((D*)wv0,endmask); w1=_mm256_maskload_pd((D*)((I)wv0+wstride1),endmask); w2=_mm256_maskload_pd((D*)((I)wv0+2*wstride1),endmask); w3=_mm256_maskload_pd((D*)((I)wv0+wstride3),endmask);
     w0t=_mm256_unpacklo_pd(w0,w1); w2t=_mm256_unpacklo_pd(w2,w3);  // 0426, 8CAE
     wb=_mm256_blend_pd(w0t,w2t,0b0011); wb=_mm256_permute2f128_pd(wb,wb,0x01);  // 8C26, 268C
     w1=_mm256_unpackhi_pd(w0,w1); w3=_mm256_unpackhi_pd(w2,w3);  // 1537, 9DBF
     w0=_mm256_blend_pd(w0t,wb,0b1100); _mm256_storeu_pd((D*)zv0, w0);  // 048c
     if(n0==3){w2=_mm256_blend_pd(w2t,wb,0b0011); _mm256_storeu_pd((D*)((C*)zv0+2*zstride1), w2);}// 26AE
     wb=_mm256_permute2f128_pd(w3,w3,0x01);  // --9D
     w1=_mm256_blend_pd(w1,wb,0b1100);  // 159D
     _mm256_storeu_pd((D*)((C*)zv0+zstride1),w1);
     wv0=(C*)wv0+(wstride1<<LGNPAR); zv0=(C*)zv0+SZI*NPAR;  // next input & output 4x4
    }while(--blkrem);
   }else if(n0==1){
    // The remnant is 1 column.  Use scatter-write... if it is implemented
    DQ(m4*4, *(D*)zv0=*(D*)wv0; wv0=(C*)wv0+wstride1; zv0=(C*)zv0+SZI;)
   }  // if n0==0, nothing to do
  }
  // vertical strips finished, do the horizontal remnant if any
  if(m&=3){  // if there is a horizontal remnant...
   wv0=(C*)wv+m4*4*wstride1; zv0=(C*)zv+m4*4*SZI;
   DQ(n, void *wv1=wv0; void *zv1=zv0; DQ(m, *(D*)zv1=*(D*)wv1; wv1=(C*)wv1+wstride1; zv1=(C*)zv1+SZI;) wv0=(C*)wv0+SZI; zv0=(C*)zv0+zstride1;)
  }
 } else
#endif
  // !!!!! this might be an else-clause from the conditional section!!!!
 z=canta(apv(r,r-1,-1L),w); RZ(z);  // rank is set
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  If w was pristine and inplaceable, transfer its pristine status to the result
 // But if we are returning the input block unchanged, leave pristinity unchanged
 if(z!=w){PRISTXFERF(z,w)}
 RETF(z);  // This verb propagates WILLOPEN and must not perform EPILOG
}    /* |:"r w */

F2(jtcant2){A*av,p,t,y;I j,k,m,n,*pv,q,r,*v;
 F2PREFIP;ARGCHK2(a,w); 
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r; 
 q=jt->ranks>>RANKTX; q=AR(a)<q?AR(a):q; RESETRANK;
 if(((q-2)&(AR(a)-q-1))>=0){t=rank2ex(a,w,DUMMYSELF,MIN(q,1),r,q,r,jtcant2); PRISTCLRF(w) RETF(t);} // rank loop on a.  Loses pristinity
 if(BOX&AT(a)){
  RZ(y=pfill(r,t=raze(a))); v=AV(y);
  GATV0(p,INT,AN(y),1); pv=AV1(p);
  m=AN(a); n=AN(t); av=AAV(a); 
  j=0; DO(r-n,pv[*v++]=j++;); DO(m, k=AN(C(av[i])); DQ(k,pv[*v++]=j;); j+=(k!=0););
 }else RZ(p=pinv(pfill(r,a)));
 A z; IRS2(p,w,0L,1L,r,jtcanta,z); RZ(z);  // Set rank for w in canta.  p is now INT type.  No need to check agreement since a has rank 1
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  If w was pristine and inplaceable, transfer its pristine status to the result
 PRISTXFERF(z,w)
 RETF(z);
}    /* a|:"r w main control */ 
