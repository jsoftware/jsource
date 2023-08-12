/* Copyright (c) 1990-2023, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: From & Associates. See Hui, Some Uses of { and }, APL87.         */

#include "j.h"


DF1(jtcatalog){PROLOG(0072);A b,*wv,x,z,*zv;C*bu,*bv,**pv;I*cv,i,j,k,m=1,n,p,*qv,r=0,*s,t=0,*u;
 F1RANK(1,jtcatalog,self);
 ASSERTF(!ISSPARSE((AT(w))),EVNONCE,"sparse arrays not supported");
 if((-AN(w)&-(AT(w)&BOX))>=0)R box(w);   // empty or unboxed, just box it
 n=AN(w); wv=AAV(w);
 DO(n, x=C(wv[i]); if(likely(AN(x))){p=AT(x); t=t?t:p; ASSERTF(!ISSPARSE(p),EVNONCE,"sparse arrays not supported"); ASSERT(HOMO(t,p),EVINHOMO); RE(t=maxtype(t,p));});  // use vector maxtype; establish type of result
 t=t?t:B01; k=bpnoun(t);  // if all empty, use boolean for result
 GA10(b,t,n);      bv=CAV(b);  // allocate place to build each item of result - one atom from each box.  bv->item 0
 GATV0(x,INT,n,1);    qv=AV(x);   // allocate vector of max-indexes for each box - only the address is used  qv->max-index 0
 GATV0(x,BOX,n,1);    pv=(C**)AV(x);  // allocate vector of pointers to each box's data  pv->box-data-base 0
 RZ(x=apvwr(n,0L,0L)); cv=AV(x);   // allocate vector of current indexes, init to 0  cv->current-index 0
 DO(n, x=C(wv[i]); if(TYPESNE(t,AT(x)))RZ(x=cvt(t,x)); r+=AR(x); qv[i]=p=AN(x); DPMULDE(m,p,m); pv[i]=CAV(x););  // fill in *qv and *pv; calculate r=+/ #@$@> w, m=*/ */@$@> w
 GATV0(z,BOX,m,r);    zv=AAV(z); s=AS(z);   // allocate result area
 // There is no need to turn off pristinity of w, because nothing was copied out by pointer (everything was copied & then cloned)
 // The result is certainly pristine if it is DIRECT
 AFLAGORLOCAL(z,(-(t&DIRECT))&AFPRISTINE)  // set result pristine if boxes DIRECT
 DO(n, x=C(wv[i]); u=AS(x); DQ(AR(x),*s++=*u++;););   // fill in shape: ; $&.> w
 for(i=0;i<m;i++){  // fill in each box
  bu=bv-k;
  DO(n, MC(bu+=k,pv[i]+k*cv[i],k););  // move in each atom  (we could stop after moving the lowest)
  DO(n, j=n-1-i; if(qv[j]>++cv[j])break; cv[j]=0;);  // increment and roll over the odometer
  RZ(*zv++=incorp(ca(b)));  // clone the items and move pointer to the result  ?clone not required?
 }
 EPILOG(z);
}

#define SETNDX(ndxvbl,ndxexp,limexp)    {ndxvbl=(ndxexp); if(unlikely((UI)ndxvbl>=(UI)limexp)){ndxvbl+=(limexp); ASSERT((UI)ndxvbl<(UI)limexp,EVINDEX);}}  // if ndxvbl>p, adding p can never make it OK
#define SETNDXRW(ndxvbl,ndxexp,limexp)    {ndxvbl=(ndxexp); if((UI)ndxvbl>=(UI)limexp){(ndxexp)=ndxvbl+=(limexp); ASSERT((UI)ndxvbl<(UI)limexp,EVINDEX);}}  // this version write to input if the value was negative
#define SETJ(jexp) SETNDX(j,jexp,p)
#if 0  // obsolete 

#define IFROMLOOP(T)        \
 {T   * RESTRICT v=(T*)wv,* RESTRICT x=(T*)zv;  \
  if(1==an){v+=j;   DQ(m,                                    *x++=*v;       v+=p; );}  \
  else              DQ(m, DO(an, SETJ(av[i]);                *x++=v[j];);   v+=p; );   \
 }
#endif

// block used to hold axis info, leading axis first
struct __attribute__((aligned(CACHELINESIZE))) faxis {
 I lenaxis;  // the length of the axes (including frame) represented by this faxis struct, in items
 I lencell;  // size of item of this axis in atoms
 I nsel;  // number of selectors.  If negative, axis is complementary and *sels is a bitmask, value is ~len
 union {
  A ind;  // (up until result allocation) the original block of selectors for this axis, for rank purposes, or 0 if none.
  I subx;   // (for complementary axes only, during the copy) index of next value to skip
 } indsubx;
 I currselx;  // the next selector index to use.  For complementary, points after the previous 1-bit.  Init to 0 in axisfrom; not used in last axis
 I *sels;  // selectors.  If 0, axis is taken in full
 I currselv;  // value of current selector (i. e. index of value being copied).  init=sel0.  Not used in last axis.  Not used if taken in full
 I sel0;  // value of first selector in this axis (index of first 1, if complementary).  Not set in last axis except when complementary.  0 if taken in full
};

#define AXFCKST0 0x8  // 0x18 is 11 to check neg&index, 10 for no checks, 01 to check neg, 00 for complementary axis
// macros to move the last axis.  base points to base of cell selected by axis -2, zbase to next output location
// ns is axes[r].nsel, nl is axes[r].lenaxis, ss is axes[r].sels
// T is cell type, S is shift lg(cellsize)
// in AVX2, sl holds selectors, al=lenaxis, al1=(lenaxis^sgnbit)-1, endmask is mask, sgnbit is 0x8..0
// xxxNI: check for negative and out-of-bounds indexes, once
// copyval: make sl valid
#define fcopyvalNI {__m256i slv=_mm256_cmpgt_epi64(_mm256_xor_si256(sl,sgnbit),al1); /* index>size-1, in offset binary */ \
  if(unlikely(!_mm256_testz_si256(slv,endmask))){  /* if any valid lane too high */ \
   sl=_mm256_add_epi64(sl,_mm256_and_si256(al,slv)); slv=_mm256_cmpgt_epi64(_mm256_xor_si256(sl,sgnbit),al1); /* add axislen to invalid lanes, test again */ \
   ASSERT(_mm256_testz_si256(slv,endmask),EVINDEX) axflags&=~AXFCKST0;  /* verify all lanes valid; indic we hit a negative */ \
  }}
#define fcopyvalN {__m256i slv=_mm256_cmpgt_epi64(_mm256_xor_si256(sl,sgnbit),al1); /* index>size-1, in offset binary */ \
  sl=_mm256_add_epi64(sl,_mm256_and_si256(al,slv));  /* adjust negative values */ \
  }
#define fcopyval  // if all indexes valid, nothing needed

// move 8-byte aligned cells using gather
#define fcopygather8(val)  {__m256i sl; \
 /* we want to keep the stores aligned on a cacheline boundary, so we back up the pointers accordingly */ \
 I bun=((I)zbase>>LGSZI)&(NPAR-1); I zvtoss=(I)ss-(I)zbase; C *zv=(C*)((I*)zbase-bun); /* backup amount, offset to bv, aligned zv */ \
 endmask=_mm256_loadu_si256((__m256i*)(validitymask+2*NPAR-bun));  /* valid part of first batch */ \
 C *lastzv=(C*)((I*)zv+((bun+ns-1)&-NPAR));  /* start of the block containing the last word */ \
 if(zv!=lastzv){  /* if there is a section using only the leading mask... */ \
  /* first section, masked at the front */ \
  sl=_mm256_maskload_epi64((I*)(zv+zvtoss),endmask); val /* load and validate a block of selectors */ \
  _mm256_maskstore_epi64((I*)zv, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),(I*)base,sl,endmask,SZI));   /* scatter read, write sequential */ \
  endmask=_mm256_cmpeq_epi64(endmask,endmask);  /* leading mask has now been used */ \
  while((zv+=NPAR*SZI)!=lastzv){  /* middle section, unmasked */ \
   sl=_mm256_loadu_si256((__m256i*)(zv+zvtoss));  val  /* load and validate selectors */ \
   _mm256_storeu_si256((__m256i*)zv, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),(I*)base,sl,endmask,SZI)); \
  } \
 } \
 /* always put out the last block, combining any unused mask */ \
 endmask=_mm256_and_si256(endmask,_mm256_loadu_si256((__m256i*)(validitymask+((NPAR-1)&-(bun+ns))))); /* mask at end, 0 3 2 1 */ \
 sl=_mm256_maskload_epi64((I*)(zv+zvtoss),endmask);   val \
 _mm256_maskstore_epi64((I*)zv, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),(I*)base,sl,endmask,SZI));   /* scatter read, write sequential */ \
\
 zbase=(I*)zbase+ns;   /* advance result pointer */ \
}

// atom {"1 array.  Go down the next-last axis, gathering groups of lines
#define fcopygatherinfull { \
 /* we want to keep the stores aligned on a cacheline boundary, so we back up the pointers accordingly */ \
 I bun=((I)zbase>>LGSZI)&(NPAR-1); C *zv=(C*)((I*)zbase-bun); /* backup amount, aligned zv */ \
 /* we have to avoid fetching out of bounds, lest we take a microprogram check that is slow. */ \
 /* For the first batch (including when it is last also), leave bv pointing to item 0 and put invalid indexes at 0 */ \
 I *bv=(I*)base;  /* back base to match adj to zbase */ \
 al1=_mm256_mul_epu32(al,_mm256_loadu_si256((__m256i*)(&iotavec[0-IOTAVECBEGIN-bun])));  /* offset 0 lands on first item */\
 endmask=_mm256_loadu_si256((__m256i*)(validitymask+2*NPAR-bun));  /* valid part of first batch */ \
 C *lastzv=(C*)((I*)zv+((bun+ns-1)&-NPAR));  /* start of the block containing the last word */ \
 if(zv!=lastzv){  /* if there is a section using only the leading mask... */ \
  /* first section, masked at the front */ \
  _mm256_maskstore_epi64((I*)zv, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),bv,_mm256_and_si256(al1,endmask),endmask,SZI));   /* scatter read, write sequential */ \
  bv+=nl; /* advance to second group of lines */ \
  /* put out the middle blocks.  We must adjust bv and the masks to make index 0 valid at the first position */ \
  al1=_mm256_mul_epu32(al,_mm256_loadu_si256((__m256i*)(&iotavec[0-IOTAVECBEGIN])));  /* offset 0 lands on first item */\
  bv-=bun*(nl>>LGNPAR);  /* Because the negative index gives an invalid offset from the 32-bit multiply, */\
  /* advance al1 to 0 1 2 3 and back base pointer to match amount we advanced al1.  Now index 0 is in bounds */ \
  endmask=_mm256_cmpeq_epi64(endmask,endmask);  /* set mask=1s for middles */ \
  while((zv+=NPAR*SZI)!=lastzv){  /* middle section, unmasked */ \
   _mm256_storeu_si256((__m256i*)zv, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),bv,al1,endmask,SZI)); \
   bv+=nl; /* advance to next group of lines */ \
  } \
  /* put out the last block. */ \
  endmask=_mm256_loadu_si256((__m256i*)(validitymask+((NPAR-1)&-(bun+ns)))); /* mask at end, 0 3 2 1 */  \
  _mm256_maskstore_epi64((I*)zv, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),bv,_mm256_and_si256(al1,endmask),endmask,SZI));   /* scatter read, write sequential */ \
 }else{  /* there is only one section, masked at both ends.  Leave bv pointing to item 0 */ \
  endmask=_mm256_and_si256(endmask,_mm256_loadu_si256((__m256i*)(validitymask+((NPAR-1)&-(bun+ns))))); /* mask at end, 0 3 2 1 */ \
  _mm256_maskstore_epi64((I*)zv, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),bv,_mm256_and_si256(al1,endmask),endmask,SZI));   /* scatter read, write sequential */ \
 } \
 zbase=(I*)zbase+ns;   /* advance result pointer */ \
}
// version not using gather, atom {"1 array
#define fcopyinfull(T) { T *zv=(T*)zbase, *bv=(T*)base; DQ(ns, *zv++=*bv; bv+=nl;) zbase=zv;} break;



#define fcopyNI(T,S) {T *zv=zbase,*bv=(T*)base; DO(ns, I sel=ss[i]; if(unlikely((UI)sel>=(UI)nl)){sel+=nl; axflags&=~AXFCKST0; ASSERT(((UI)sel<(UI)nl),EVINDEX)} zv[i]=bv[sel];) zbase=(C*)zbase+(ns<<S); axflags-=AXFCKST0;}
    // once we have vetted the parameters, there's no need to check for index error.  But possibly neg
#define fcopyNI8 fcopyNI(I,LGSZI)
#define fcopyNI4 fcopyNI(I4,2)
#define fcopyNI2 fcopyNI(S,1)
#define fcopyNI1 fcopyNI(C,0)
#define fcopyNIv(b) {C *zv=zbase,*bv=(C*)base; DO(ns, I sel=ss[i]; if(unlikely((UI)sel>=(UI)nl)){sel+=nl; axflags&=~AXFCKST0; ASSERT(((UI)sel<(UI)nl),EVINDEX)} JMCR(zv+i*celllen,bv+sel*celllen,celllen,(b),endmask) ) zbase=(C*)zbase+ns*celllen; axflags-=AXFCKST0;}
// xxxN: check for negative only
#define fcopyN(T,S) {T *zv=zbase,*bv=(T*)base; DO(ns, I sel=ss[i]; sel+=REPSGN(sel)&nl; zv[i]=bv[sel];) zbase=(C*)zbase+(ns<<S);}
#define fcopyN8 fcopyN(I,LGSZI)
#define fcopyN4 fcopyN(I4,2)
#define fcopyN2 fcopyN(S,1)
#define fcopyN1 fcopyN(C,0)
#define fcopyNv(b) {C *zv=zbase,*bv=(C*)base; DO(ns, I sel=ss[i]; sel+=REPSGN(sel)&nl; JMCR(zv+i*celllen,bv+sel*celllen,celllen,(b),endmask) ) zbase=(C*)zbase+ns*celllen;}
// xxx: no checks
#define fcopy(T,S) {T *zv=zbase,*bv=(T*)base; DO(ns, I sel=ss[i]; zv[i]=bv[sel];) zbase=(C*)zbase+(ns<<S);}
#define fcopy8 fcopy(I,LGSZI)
#define fcopy4 fcopy(I4,2)
#define fcopy2 fcopy(S,1)
#define fcopy1 fcopy(C,0)
#define fcopyv(b) {C *zv=zbase,*bv=(C*)base; DO(ns, I sel=ss[i]; JMCR(zv+i*celllen,bv+sel*celllen,celllen,(b),endmask) ) zbase=(C*)zbase+ns*celllen;}
#if 0  // obsolete
// xxxC: complementary indexing: sels are a mask.  Count consecutive bits and copy blocks.
#define fcopyC {I selx=axes[r].sel0; I selr=ns; C *zv=zbase; \
do{ \
 I wd; while(1){wd=ss[selx>>LGBW]>>(selx&(BW-1)); if(wd!=0)break; selx=(selx+BW)&-BW;} I s1=selx+CTTZI(wd); selx=s1;  /* find first 1, which must e present */ \
 while(1){wd=(UI)~ss[selx>>LGBW]>>(selx&(BW-1)); if(wd!=0)break; selx=(selx+BW)&-BW; if(selx>=s1+selr)break;} selx+=CTTZI(wd); selx=selx>=s1+selr?s1+selr:selx;  /* find next 0, if any */ \
 I sl=selx-s1; JMC(zv,base+s1*celllen,sl*celllen,0) zv+=sl*celllen; selr-=sl; /* copy the data */ \
}while(selr); zbase=zv;  /* loop till all 1s moved */ \
}
#else
// xxxC: complementary indexing: sels are sorted list of exclusions, with no duplicates.  .sel0 has index of first gap
#define fcopyC {I selx=axes[r].sel0, gap0=selx; C *zv=zbase; \
I nx=nl-ns;  /* number of complementary indexes */ \
while(1){ \
 I gapn=selx!=nx?ss[selx]:nl;  /* end+1 of gap, running to end on last gap */ \
 I gapl=gapn-gap0; JMC(zv,base+gap0*celllen,gapl*celllen,0) zv+=gapl*celllen; /* copy the data */ \
 if(selx==nx)break;  /* stop after final gap-to-end */ \
 while(1){++selx; ++gapn; if(selx==nx)break; if(ss[selx]!=gapn)break;}  /* find start of next gap */ \
 if(gapn>=nl)break; gap0=gapn; /* if trailing indexes pushed gap off the end, finis */ \
} \
zbase=zv; }
#endif
#if 0 // obsolete 


 {T *zv=zbase,*bv=(T*)base; I *ssv=ss; I msk=*ssv; DO(ns, while(msk==0){bv+=BW; msk=*++ssv;} *zv++=bv[CTTZI(msk)]; msk&=msk-1;) zbase=zv;}
#define fcopyC8 fcopyC(I)
#define fcopyC4 fcopyC(I4)
#define fcopyC2 fcopyC(S)
#define fcopyC1 fcopyC(C)
#define fcopyCv(b) {C *zv=zbase,*bv=(C*)base; I *ssv=ss; I msk=*ssv; DO(ns, while(msk==0){bv+=BW; msk=*++ssv;} JMCR(zv,bv+CTTZI(msk)*celllen,celllen,(b),endmask) zv+=celllen; msk&=msk-1;) zbase=zv;}
#endif

// rflags is w minor cell rank/len of w frame/1B rank of result/1B /6B dimension of axes-1
// if a is inplaceable in jt, ind in the last axis is the area that can be used for the result
static A jtaxisfrom(J jt,A w,struct faxis *axes,I rflags){F2PREFIP; I i;
 I r=rflags&0x3f, zr=(C)(rflags>>8), wf=(C)(rflags>>16), wcr=(C)(rflags>>24), hasr=(rflags>>7)&1;  // number of axes-1; result rank; w framelen; 1 iff 1st axis is from rank
 C *base=voidAV(w);  // will be starting cell number in all axes before last
 // convert lencell to bytes & roll it up; calculate base from sel0 values
 I wt=AT(w);
 I k=bplg(wt);  // lg of size of atom
// obsolete  I framesize=1;  // will hold #cells in frame of last axis
 I celllen=axes[r].lencell;  // length of cell of last axis, in atoms
 I zn=celllen;  // number of atoms in result
 A z;  // result
 I nunitsels=r;  // number of leading axes containing exctly one selector.  When the axis below this prefix rolls over, we can stop, knowing that there are no higher selectors
 if(likely(AN(w)!=0)){  // normal case, w has atoms
  for(i=r-1;i>=0;--i){  // for axes BEFORE the last
   I absnsel=axes[i].nsel^REPSGN(axes[i].nsel);  // adjust for complementary indexing
   DPMULDE(zn,absnsel,zn);  // count # cells in frame and selectors
   nunitsels=absnsel!=1?i:nunitsels;  // if this cell-count is not 1, reset the count of # 1s.
    // note: if some selector is empty and the others overflow, this will give limit error.  Sue me.
   axes[i].lencell<<=k;  // convert cellsize to bytes
   base+=axes[i].lencell*axes[i].sel0;  // for axes before last, add offset of first index
   axes[i].currselv=axes[i].sel0;  // set current=start, to begin
   axes[i].currselx=0;  // init first position being processed
  }
// obsolete   DPMULDE(framesize,celllen,zn);  // frame size * cell size
  I nsel=axes[r].nsel;  // #selectors, neg if complementary
  I lenaxis=axes[r].lenaxis;  // length of last axis
  DPMULDE(zn,nsel^REPSGN(nsel),zn);  // * last-axis size, gives result size
#define MINVIRTSIZE 32  // must have this many atoms to be virtual  scaf
  if(((nunitsels-r)|(zn-MINVIRTSIZE))>=0){  // if there is only one axis left, and result is big enough, and last axis not B01  scaf B01???
   // There is only one application of the last axis.  If the indexes are sequential, we can make the result virtual
   // Whether we should do so is a tricky question.  Surely, if the argument is big, since we may save a large indexed copy.
   // If the argument is small, the virtual is still better if it doesn't have to be realized; but it might be
   // realized in effect if it is unavailable for inplacing.  OTOH, if the argument is indirect the virtual does
   // not require individual usecounting of the atoms.
   //
   // It would be good if we could know if the result is going to be assigned, perhaps jt->zombieval=1.  We could
   // suppress the virtual then.
   //
   // We allow virtualing even for NJA blocks. 
   // result is more than one atom and does not come from multiple cells.  Perhaps it should be virtual.  See if the indexes are consecutive
   I *sels=axes[r].sels;  // pointer to selectors of last axis
   I index0, indexn;  // indexes of first & last values, in range if virtual is OK
   if(nsel>=0){  // normal axis
    index0 = sels[0]; index0+=REPSGN(index0)&lenaxis; indexn=0;  // index of first item, set last item OK
    // check the last item before checking the middle.
    DQ(nsel-1, indexn=sels[1+i]; indexn+=REPSGN(indexn)&lenaxis; if(indexn!=index0+1+i){indexn=lenaxis; break;});
    nsel=AR(axes[r].indsubx.ind)!=1?0:nsel;  // if shape changes, set flag value to suppress returning entire w unchanged
   }else{
    // complementary indexing.  See if the values are consecutive (in-full is impossible)
#if 0  //  obsolete 
    // the number of leading 0s is .sel0.  If #leading+#trailing+nsel=lenaxis, the 1s are consecutive
    nsel=~nsel;  // convert nsel to positive length
    I nzeros=index0=axes[r].sel0;  // init pos of first 1, and count of lower 0s
    I trailx=(lenaxis-1)>>LGBW;  // last word containing bits
    nzeros-=(BW-1)-((lenaxis-1)&(BW-1));  // we will clear bits past the end; they will reappear as 0s so subtract their count
    UI trailwd=(UI)sels[trailx]<<((BW-1)-((lenaxis-1)&(BW-1)))>>((BW-1)-((lenaxis-1)&(BW-1)));  // clear upper bits
    while(1){if(trailwd!=0){nzeros+=(BW-1)-CTLZI(trailwd); break;} nzeros+=BW; if(trailx==0)break; trailwd=(UI)sels[--trailx];}  // go till we hit a 1, which might not exist if nsel=0
    indexn=lenaxis-!!(lenaxis<=(nzeros+nsel));  // lenaxis-1 if consecutive (equality), lenaxis if not
#else
    // The first index we produce is .sel0, which means that must be the index of the first gap.  Find the width
    // of that gap and see if it accounts for all the indexes
    nsel=~nsel;  // convert nsel to positive length - gives number of surviving 
    index0=axes[r].sel0; I axn=axes[r].lenaxis; indexn=likely(index0<nsel)?axes[r].sels[index0]:axn;  // start of gap, axis len, end+1 of gap (may be len of axis)
    index0|=(indexn-index0)-nsel;  // if more cells than gap, turn index0 neg to stop virtual
    --indexn;  // convert to end-1
#endif
   }
   // nsel has been modified if shape changes
   if((index0|(lenaxis-indexn-1))>=0){  // index0>=0 and indexn<=lenaxis-1
    // indexes are consecutive and in range.  Make the result virtual.  Rank of w cell must be > 0, since we have >=2 consecutive result atoms
    // if the selected length is the axis length, and there is only one axis, we are taking the entire w arg - just return it
    if(unlikely((r+(nsel^lenaxis))==0))R RETARG(w);
    I baseadj=base-CAV(w);  // movement of base needed from upper selectors.  Must calculate before virtual in case it self-virtuals and moves w
    RZ(z=virtualip(w,index0*celllen,zr));  // inplace w OK since no cells repeated
    // fill in shape and number of atoms, and offset the data pointer using base
    AK(z)+=baseadj;  // move offset from start of w data to the cell selected by upper selectors
    // shape is 1s for upper axes/last-axis selectors/shape of cell lof last axis
    AN(z)=zn;
     I *zs=AS(z)+zr-wcr;  // pointer into result shape, moved around as we calculate; start pointing to cellshape
    // install the axes for the cell of w: all axes except the frame and the selectors
    MCISH(zs,AS(w)+AR(w)-wcr,wcr)  // zs->start of cell shape
    // axes for the last selector
    if(axes[r].nsel>=0){MCISH(zs-=AR(axes[r].indsubx.ind),AS(axes[r].indsubx.ind),AR(axes[r].indsubx.ind));  // normal, back up to rank and copy
    }else{*--zs=~axes[r].nsel;  // complementary, treat as list of appropriate length
    }
    // install 1s for the rest of the shape
    if(zs!=AS(z))do{*--zs=1;}while(zs!=AS(z));
    RETF(z);
   }
  }
 }else{zn=0;}  // if w empty, z must be empty too, since no nonempty selector is valid on 0-len axis
 // allocate the result, or use inplace the result (which must not be unincorpable or DIRECT)
 if(!((I)jtinplace&JTINPLACEA)){
  GA00(z,wt,zn,zr);  // result-shape is frame of w followed by shape of a followed by shape of item of cell of w; start with w-shape, which gets the frame
  // install shape: w frame, followed by shape of each selector, then shape of cell
  MCISH(AS(z),AS(w),wf);  // axes coming from w frame
  I *zs=&AS(z)[wf];
  for(i=hasr;i<=r;++i){  // for each axis coming from a
   if(axes[i].sels!=0){  // normal or complementary
    if(axes[i].nsel>=0){MCISH(zs,AS(axes[i].indsubx.ind),AR(axes[i].indsubx.ind)) zs+=AR(axes[i].indsubx.ind);  // normal, copy the rank
    }else{*zs++=~axes[i].nsel; axes[i].indsubx.subx=axes[i].sel0; // complementary, treat as list of appropriate length and init for copy
    }
   }else{*zs++=axes[i].nsel;}  // in full, treat as length with length of axis
  }
  // install the axes for the cell of w: trailing axes, using zr since we don't know how many axes were discarded
  MCISH(zs,AS(w)+AR(w)-wcr,wcr)
 }else{z=axes[r].indsubx.ind; AT(z)=wt;}  // if inplaceable, put the result where the axes were
    // no need to init subx, since this path cannot have complementary indexes

 if(unlikely(zn==0)){I j;  // If no data to move, just audit the indexes and quit
  for(j=hasr;j<=r;++j){  // for each axis coming from a
   if(axes[j].sels!=0){I al=axes[j].lenaxis; DO(axes[j].nsel, I sel=axes[j].sels[i]; sel+=REPSGN(sel)&al; ASSERT((UI)sel<(UI)al,EVINDEX)) }
  }
  R z;
 }

// obsolete  I noframe=REPSGN(framesize-2); axes+=r&noframe; r-=r&noframe;  // F..F if all previous axes are in base; only one selection pass needed (gives early exit)
 axes+=nunitsels; r-=nunitsels;  // Now that initial selectors are rolled into base, we can delete the consecutive leading single-selector axes.  When they start to roll over we can quit
 celllen<<=k;  // convert last-axis len to bytes

 // decide what copy routines to use for last axis.  
 JMCDECL(endmask)  // in case cellsize is irregular, define mask for JMC
 I axflags=(AXFCKST0*3);  // init axis-r test flags: 11 to check neg & invalid
 if(celllen==(celllen&-celllen&(2*SZI-1))){  // check for size 1 2 4 8
  // size can be moved by primitive store.  set size
  axflags|=CTTZI(celllen);  // bits 0-2 give size
 }else{
  JMCSETMASK(endmask,celllen,0)  // allow overcopy
  axflags|=0b100;  // size 100 means 'use move'
 }
#if C_AVX2
 __m256i al=_mm256_set1_epi64x(axes[r].lenaxis), al1, sgnbit;  // in all lanes: axis len
#endif
 axflags=axes[r].nsel<0?0b00111:axflags;   // for complementary axis, use single block-copying routine 0b00111
 I ns=axes[r].nsel, nl=axes[r].lenaxis, *ss=axes[r].sels; ns=REPSGN(ns)^ns;  // bring last-axis info into registers
 void *zbase=voidAV(z);  // running output pointer
 if(unlikely((UI)(~(axflags>>2)&SGNTO0(axes[r].nsel-2))>(UI)axes[r-((UI)r>0)].sels)){  // atom {"1 y &c.  last axis is 1 long, length 1/2/4/8 non-complementary, and previous axis taken in full
  // atom {"1 y, with short cells.  Process last 2 axes together; switch to code that goes down the column
  I sel=*ss; sel+=REPSGN(sel)&nl; ASSERT((UI)sel<(UI)nl,EVINDEX)  // validate the single selector in the last axis
  base+=sel*celllen;  // offset base to account for indexing in the discarded axis
  --r;  // remove the last axis
  ns=axes[r].nsel;  // ns now has the length of the axis taken in full
  axflags&=0b00011;  // select the copy-in-full code for the appropriate length of cell
#if C_AVX2
  // if gather is supported, change nl to distance between gathers
  nl<<=(celllen>>LGSZI)<<(LGLGNPAR);  // nl becomes the stride in atoms between groups of gathers, 4*length of last axis
#endif
  // al still holds the length of the last axis
 }
#if C_AVX2  // WARNING!!! this conditional starts with else
else{   // normal last axis
 axflags=(((axflags^0b11011)+((I)zbase&(SZI-1)))==0)?0b11111:axflags; // For 8-byte aligned cell, switch to the routine using gather.  it is conceivable that we could have an unaligned 8-byte cell if somebody reshaped a cell of irregular boundary
 // init vars needed for gather.  It is faster to set these than to test for whether we need to
 al1=_mm256_add_epi64(al,_mm256_set1_epi64x(Iimax));  // len-1 in offset binary
 sgnbit=_mm256_set1_epi64x(Iimin);  // 0x8..0
}
#endif
 // loop over all axes.
 while(1){
  // move one _1-cell using the indexes.  We are in a loop through the _1-cells; each case in the switch below copies one _1-cell
  switch(axflags&0x1f){
  case 0b11000: fcopyNI1 break; case 0b10000: fcopy1 break; case 0b01000: fcopyN1 break;
  case 0b11001: fcopyNI2 break; case 0b10001: fcopy2 break; case 0b01001: fcopyN2 break;
  case 0b11010: fcopyNI4 break; case 0b10010: fcopy4 break; case 0b01010: fcopyN4 break;
  case 0b11011: fcopyNI8 break; case 0b10011: fcopy8 break; case 0b01011: fcopyN8 break;
  case 0b11100: fcopyNIv(0) break; case 0b10100: fcopyv(0) break; case 0b01100: fcopyNv(0) break;
#if C_AVX2
  case 0b11111: fcopygather8(fcopyvalNI) axflags-=AXFCKST0; break; case 0b10111: fcopygather8(fcopyval) break; case 0b01111: fcopygather8(fcopyvalN) break;
    // carry on with fewer audits if gather repeated
  case 0b00011: fcopygatherinfull break;  // scaf should have non-AVX version for 1/2/4-byte
#else
  case 0b00011: fcopyinfull(I) break;
#endif
  // routines for atom {"1 y, depending on size
  case 0b00010: fcopyinfull(I4) break; case 0b00001: fcopyinfull(S) break; case 0b00000: fcopyinfull(C) break;

  case 0b00111: fcopyC break;   // complementary axis of any size
  default: break;
  }
  if(likely(r==0))break;  // if there is only 1 axis, we're done
  // roll up the axes, advancing the odometer 
  I rodo=r-1;  // start on axis -2   scaf handle axis -2 separately
  while(1){  // till we have handled all changing wheels
   struct faxis *axr=&axes[rodo];
   if(axr->sels==0){
    // axis taken in full.  advance to next cell
    base+=axr->lencell;  // advance to next sequential cell
    if(++axr->currselx!=axr->nsel)break;  // if this wheel doesn't roll over, stop here
    base-=axr->nsel*axr->lencell;  // rollover: back this wheel to the beginning
    axr->currselx=0;  // look at next wheel; reset this one
   }else if(++axr->currselx!=(REPSGN(axr->nsel)^axr->nsel)){
    // axis not in full and did not roll over.  Advance to next index
    I nextx;  // will be the next index to use
    if(likely(axr->nsel>=0)){  // complementary index?
     // normal non-complementary index.  Step to the next row
     SETNDX(nextx,axr->sels[axr->currselx],axr->lenaxis);  // fetch next index
    }else{
#if 0  // obsolete 
     // complementary index.  Start after currselv and find the next 1-bit   scaf should rewrite to use the complementary indexes, not a mask
     nextx=axr->currselv+1;  // bit# to start look
     while(1){  // it's gotta be there
      UI nextbits=(UI)axr->sels[nextx>>LGBW]>>(nextx&(BW-1));  // the rest of this word
      if(nextbits){nextx+=CTTZI(nextbits); break;}  // get index of lowest 1 bit
      nextx=(nextx+(BW-1))&-BW;  // if none, advance to next word and keep looking
     }
#else
     // complementary index.  .currselv is the last index we returned; .indsubx.subx is a valid starting place to search for the next index 
     // we know there is another valid index.
     nextx=axr->currselv+1; I subx=axr->indsubx.subx; // bit# to start look, place to look for match
     I nextcomp;  // next complementary index to use
     while((nextcomp=subx<axr->lenaxis-~axr->nsel?axr->sels[subx]:axr->lenaxis)<=nextx){  // when sel doesn't limit, advance to next sel
      nextx+=nextcomp==nextx; axr->indsubx.subx=++subx;  // if sel matches nextx, advance nextx to skip it
     }
#endif
    }
    base+=(nextx-axr->currselv)*axr->lencell;  // move base by amount of index movement
    axr->currselv=nextx;  // set next index
    break;  // when wheel doesn't roll over, stop processing wheels
   }
   // here the current axis is rolling over.
   if(rodo<=0)goto endaxes;  // when we roll over the biggest wheel, quit
   axr->currselx=0;   // back to start
   base+=(axr->sel0-axr->currselv)*axr->lencell;  // move base by amount of index movement
   axr->indsubx.subx=axr->currselv=axr->sel0;  // set starting index for wheel and hint
   --rodo;  // back up to increment previous axis
  }  // wheel rolled over, advance to next wheel
  // base has been advanced
 }  // loop till all _1-cells moved
endaxes:;
 RETF(z);
}

// a is numeric
F2(jtifrom){A z;C*wv,*zv;I acr,an,ar,*av,j,k,p,pq,q,wcr,wf,wn,wr,*ws,zn;
 F1PREFIP;
 ARGCHK2(a,w);
 // IRS supported but only for a single a value.  This has implications for empty arguments.
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(unlikely(ar>acr))R rank2ex(a,w,DUMMYSELF,acr,wcr,acr,wcr,jtifrom);  // split a into cells if needed.  Only 1 level of rank loop is used
 // From here on, execution on a single cell of a (on matching cell(s) of w, or all w).  The cell of a may have any rank
 an=AN(a); wn=AN(w); ws=AS(w);
 if(unlikely(!ISDENSETYPE(AT(a),INT))){
  if(AR(a)<(AT(a)&B01))a=zeroionei(BAV(a)[0]);else{RZ(a=cvt(INT,a)); jtinplace=(J)((I)jtinplace|JTINPLACEA);}  // convert boolean or other arg to int, with special check for scalar boolean.  Allocated result is always eligible to inplace
 }
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
// if(an==0 && wn==0 && ws[wf]==0)wcr=wr=0;     defer this pending analysis
 // If w is empty, portions of its shape may overflow during calculations, and there is no data to move (we still need to
 // audit for index error, though).  If w is not empty, there is no need to check for such overflow.  So we split the computation here.
 // Either way, we need   zn: #atoms in result   p: #items in a cell of w
 p=likely(wcr!=0)?ws[wf]:1;  // p=number of items to be selected from
 // set up axis structs
 struct faxis axes[2];  // one for frame, one for data
 I m; PROD(m,wf,ws);  // #wcr-cells in w: tells if we need frame
 PROD(k, wcr-1, ws+wf+1);  // number of atoms in an item of a cell
 axes[0].lenaxis=m; axes[0].lencell=p*k; axes[0].nsel=m; axes[0].indsubx.ind=mtv; // fill in frame axis
 axes[0].currselx=0; axes[0].sels=0; axes[0].currselv=0; axes[0].sel0=0;  // fill in frame axis - should be 1 store
 I r=m>1;  // remember if frame is included as an axis.  #axes-1.  We need a leading axis in full if there are multiple cells of w

 // fill in last axis, for the indexes
 I wncr=wcr-((UI)wcr>0);  // rank of the cell that gets copied
 axes[r].lenaxis=p; axes[r].lencell=k; axes[r].nsel=an; axes[r].sels=AV(a); axes[r].indsubx.ind=a;  // fill in selection axis
 // if no frame, w cell-rank is 1, a is inplaceable, and an atom of w is the same size as an atom of a, preserve inplaceability of a (.ind is already filled in)
 // since inplacing may change the type, we further require that the block not be UNINCORPABLE, and the result also not DIRECT since
 // the copy may be interrupted by index error and be left with anvalid atoms
 jtinplace=(J)((I)jtinplace&~((SGNTO0(AC(a)&SGNIFNOT(AFLAG(a),AFUNINCORPABLEX)&-(AT(w)&DIRECT))<=(UI)(wf|(wcr^1)|(SZI^(1LL<<bplg(AT(w))))))<<JTINPLACEAX));
 RETF(jtaxisfrom(jtinplace,w,axes,(wncr<<24)+(wf<<16)+((ar+wr-(I)(0<wcr))<<8)+r*0x81))  // move the values and return the result
#if 0  // obsolete 
 I wflag=AFLAG(w);
 if(likely(wn!=0)){  // w has atoms; get # atoms in result
  // For virtual results we need: k: number of atoms in an item of a cell of w;   
  PROD(k, wcr-1, ws+wf+1);  // number of atoms in an item of a cell
  // Also m: #wcr-cells in w 
//  PROD(m,wf,ws); zn=k*m;  DPMULDE(an,zn,zn);
  if(((zn-2)|(wr-2)|-(wf|(wflag&(AFNJA))))>=0){  // zn>1 and not (frame or NJA) and rank>1.  Don't take the time creating a virtual block for a list.  User should use ;.0 for that  NJAwhy
   // result is more than one atom and does not come from multiple cells.  Perhaps it should be virtual.  See if the indexes are consecutive
   I index0 = av[0]; index0+=REPSGN(index0)&p;  // index of first item
   // check the last item before checking the middle.
   I indexn = av[an-1]; indexn+=REPSGN(indexn)&p;
   if(indexn==index0+an-1){
     I indexp=index0; DO(an-1, indexn=av[1+i]; indexn+=REPSGN(indexn)&p; if(indexn!=indexp+1){indexn=p; break;} indexp=indexn;);
   }else indexn=p;
   if((index0|(p-indexn-1))>=0){  // index0>=0 and indexn<=p-1
    // indexes are consecutive and in range.  Make the result virtual.  Rank of w cell must be > 0, since we have >=2 consecutive result atoms
    RZ(z=virtualip(w,index0*k,ar+wr-1));
    // fill in shape and number of atoms.  ar can be anything.
    I* as=AS(a); AN(z)=zn; I *s=AS(z); MCISH(s,as,ar) MCISH(s+ar,ws+1,wr-1)
    RETF(z);
   }
  }
  // for copying items, we need k: size in bytes of an item of a cell of w
// obsolete   k<<=bplg(AT(w));  // convert cellsize to bytes
 } else {zn=0;}  // No data to move
 // Allocate the result area and fill in the shape.  Could inplace here
 GA00(z,AT(w),zn,ar+wr-(I)(0<wcr));  // result-shape is frame of w followed by shape of a followed by shape of item of cell of w; start with w-shape, which gets the frame
 MCISH(AS(z),AS(w),wf); MCISH(&AS(z)[wf],AS(a),ar); if(wcr)MCISH(&AS(z)[wf+ar],1+wf+ws,wcr-1);
 if(unlikely(!zn)){DO(an, SETJ(av[i])) R z;}  // If no data to move, just audit the indexes and quit
 // from here on we are moving items
 wv=CAV(w); zv=CAV(z); SETJ(*av);
 switch(k){
 case sizeof(I):
#if C_AVX2
#define IDXASSERT(vo,vr,b) {\
 __m256d _mask=_mm256_castsi256_pd(_mm256_andnot_si256(vr,_mm256_sub_epi64(vr,b))); /* positive, and negative if you subtract axis length */\
 ASSERT(_mm256_testc_pd(_mask,_mm256_castsi256_pd(ones)),EVINDEX);}

#define GETIDX(v,p,b) {\
 __m256i _v=p; /* fetch a block of indices */\
 v=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(_v),_mm256_castsi256_pd(_mm256_add_epi64(_v,b)),_mm256_castsi256_pd(_v))); /* get indexes, add axis len if neg */\
 IDXASSERT(_v,v,b);}
 // moving I/D.  Use GATHER instruction.  Future hardware can exploit that.
 {__m256i endmask=_mm256_setzero_si256(); /* length mask for the last word */ 
  __m256i wstride=_mm256_set1_epi64x(p);  // atoms between cells
  __m256i ones=_mm256_cmpgt_epi64(wstride,endmask);  // mask to use for gather into all bytes - set this way so compiler assigns a register
  I * RESTRICT v=(I*)wv; I* RESTRICT x=(I*)zv;  // input and output pointers
  if(an==1){  // special case of atom {"1 y
   if(m==1){  // the atom { list case is pretty common
    *x=v[j];  // just move the one value
   }else{
    wstride=_mm256_insert_epi64(wstride,0,0);  // 0 p p p
    wstride=_mm256_add_epi64(wstride,_mm256_slli_si256(wstride,8));  // +0 0 0 p -> 0 p p 2p
    wstride=_mm256_add_epi64(wstride,_mm256_castpd_si256(_mm256_permute_pd(_mm256_castsi256_pd(wstride),0b0000)));  // +0 0 p p -> 0 p 2p 3p
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
    v+=j;  // advance base pointer to the column we are fetching
    DQ((m-1)>>LGNPAR, _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,wstride,ones,SZI)); v+=NPAR*p; x+=NPAR;)
    /* runout, using mask */ 
    _mm256_maskstore_epi64(x, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,wstride,endmask,SZI));   // must use a different reg for source and index, lest VS2013 create an illegal instruction
   }
  }else{  // Normal case: a list of indexes for each cell
   UI i=m; // loop counter for number of items to process
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-an)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
   __m256i anynegindex=_mm256_setzero_si256();  // accumulate sign bits of the indexes
   // Load the first 16 indexes and v-values into registers
   __m256i indexes0, indexes1, indexes2, indexesn;  // indexesn, the last, may be partial
   if(an>NPAR){
    GETIDX(indexes0,_mm256_loadu_si256((__m256i*)av),wstride);
    if(an>2*NPAR){
     GETIDX(indexes1,_mm256_loadu_si256((__m256i*)(av+NPAR)),wstride);
     if(an>3*NPAR){
      GETIDX(indexes2,_mm256_loadu_si256((__m256i*)(av+2*NPAR)),wstride);
     }
    }
   }
   if(an<=4*NPAR){
    GETIDX(indexesn,_mm256_maskload_epi64(av+((an-1)&-NPAR),endmask),wstride); // fetch last block of indices
    // Now do the gather/writes
    if(an<=NPAR){
     do{_mm256_maskstore_epi64(x, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }else if(an<=2*NPAR){
     do{_mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));
        _mm256_maskstore_epi64(x+NPAR, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }else if(an<=3*NPAR){
     do{_mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));
        _mm256_storeu_si256((__m256i*)(x+NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes1,ones,SZI));
        _mm256_maskstore_epi64(x+2*NPAR, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }else{
      do{_mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));
        _mm256_storeu_si256((__m256i*)(x+NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes1,ones,SZI));
        _mm256_storeu_si256((__m256i*)(x+2*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes2,ones,SZI));
        _mm256_maskstore_epi64(x+3*NPAR, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }
   }else{
    GETIDX(indexesn,_mm256_loadu_si256((__m256i*)(av+3*NPAR)),wstride);   // fetch last block of indices
    do{
     // 17+indexes.  We must read the tail repeatedly
     // this first execution audits the indexes and converts negatives
     _mm256_storeu_si256((__m256i*)(x+0*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));  // process the indexes saved in registers
     _mm256_storeu_si256((__m256i*)(x+1*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes1,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+2*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes2,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+3*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,ones,SZI));
     I *RESTRICT avv=av+4*NPAR; x+=4*NPAR;  // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
     __m256i indexes;
     if(an>5*NPAR){
      indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
      DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
       __m256i indexeso=indexes;__m256i indexesr;  // fetch a block of indexes
       indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
       anynegindex=_mm256_or_si256(anynegindex,indexeso); indexesr=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexeso),_mm256_castsi256_pd(_mm256_add_epi64(indexeso,wstride)),_mm256_castsi256_pd(indexeso)));  // get indexes, add axis len if neg
       IDXASSERT(indexeso,indexesr,wstride);
       _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesr,ones,SZI)); x+=NPAR;
      )
      anynegindex=_mm256_or_si256(anynegindex,indexes); __m256i indexesr=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg
      IDXASSERT(indexes,indexesr,wstride);
      _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesr,ones,SZI)); x+=NPAR;
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(avv,endmask);  // fetch a block of indexes
     anynegindex=_mm256_or_si256(anynegindex,indexes); __m256i indexesr=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg.  unfetched indexes are 0
     IDXASSERT(indexes,indexesr,wstride);
     _mm256_maskstore_epi64(x, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesr,endmask,SZI)); x+=((an-1)&(NPAR-1))+1;   // must use a different reg for source and index, lest VS2013 create an illegal instruction
     v+=p;  // advance to next input cell
     --i;
     if(_mm256_testz_pd(_mm256_castsi256_pd(anynegindex),_mm256_castsi256_pd(anynegindex)))break;
    }while(i);
    while(i){
     // this second version comes into play if there were no negative indexes.  If there are negatives we end up auditing them repeatedly, too bad.
     _mm256_storeu_si256((__m256i*)(x+0*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));  // process the indexes saved in registers
     _mm256_storeu_si256((__m256i*)(x+1*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes1,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+2*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes2,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+3*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,ones,SZI));
     I *RESTRICT avv=av+4*NPAR; x+=4*NPAR;  // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
     __m256i indexes;
     if(an>5*NPAR){
      indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
      DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
       __m256i indexesx=indexes;  // fetch a block of indexes
       indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
       _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesx,ones,SZI)); x+=NPAR;
      )
      _mm256_storeu_si256((__m256i*)x,_mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes,ones,SZI)); x+=NPAR;
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(avv,endmask);  // fetch a block of indexes
     _mm256_maskstore_epi64(x, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes,endmask,SZI)); x+=((an-1)&(NPAR-1))+1;   // must use a different reg for source and index, lest VS2013 create an illegal instruction
     v+=p;  // advance to next input cell
     --i;
    }
   }
  }
 }
#else //C_AVX2
 IFROMLOOP(I);
#endif
  break;
 case sizeof(C): IFROMLOOP(C); break; 
 case sizeof(S): IFROMLOOP(S); break;  
#if SY_64
 case sizeof(int):IFROMLOOP(int); break;
#endif
 default:
  // cells are not simple types.  We can safely move full words, since there is always extra buffer space at the end of any type that is not a word-multiple
  {C* RESTRICT u,* RESTRICT v=(C*)wv,* RESTRICT x=(C*)zv;
   pq=p*k;
   JMCDECL(endmask) JMCSETMASK(endmask,k,0) 
   if(1==an){v+=j*k; DQ(m,                     u=v;     JMCR(x,u,k,0,endmask) x+=k; v+=pq;);}
   else              DQ(m, DO(an, SETJ(av[i]); u=v+j*k; JMCR(x,u,k,0,endmask) x+=k;); v+=pq;)
  }
  break;
 }
 RETF(z);
#endif
}    /* a{"r w for numeric a */

#if 0 // obsolete
#define BSET(x,y0,y1,y2,y3)     *x++=y0; *x++=y1; *x++=y2; *x++=y3;
#define BSETV(b)                MC(v,wv+b*k,k); v+=k;

#if !SY_64 && SY_WIN32
#define BNNERN(T)   \
 {B*au=av;T*v=(T*)wv,v0,v1,*x=(T*)zv;                                               \
  DQ(m, v0=v[0]; v1=v[1]; u=(I*)av; DQ(q, switch(*u++){                             \
   case B0000: BSET(x,v0,v0,v0,v0); break;  case B0001: BSET(x,v0,v0,v0,v1); break; \
   case B0010: BSET(x,v0,v0,v1,v0); break;  case B0011: BSET(x,v0,v0,v1,v1); break; \
   case B0100: BSET(x,v0,v1,v0,v0); break;  case B0101: BSET(x,v0,v1,v0,v1); break; \
   case B0110: BSET(x,v0,v1,v1,v0); break;  case B0111: BSET(x,v0,v1,v1,v1); break; \
   case B1000: BSET(x,v1,v0,v0,v0); break;  case B1001: BSET(x,v1,v0,v0,v1); break; \
   case B1010: BSET(x,v1,v0,v1,v0); break;  case B1011: BSET(x,v1,v0,v1,v1); break; \
   case B1100: BSET(x,v1,v1,v0,v0); break;  case B1101: BSET(x,v1,v1,v0,v1); break; \
   case B1110: BSET(x,v1,v1,v1,v0); break;  case B1111: BSET(x,v1,v1,v1,v1); break; \
  });                                                                               \
  b=(B*)u; DQ(r, *x++=*b++?v1:v0;); v+=p;);                                         \
 }
#define BNNERM(T,T1)   \
 {B*au=av;T*c,*v=(T*)wv,v0,v1,*x=(T*)zv;T1 vv[16],*y;                                  \
  DQ(m, v0=v[0]; v1=v[1]; c=(T*)vv; y=(T1*)x; u=(I*)av;                                \
   BSET(c,v0,v0,v0,v0); BSET(c,v0,v0,v0,v1); BSET(c,v0,v0,v1,v0); BSET(c,v0,v0,v1,v1); \
   BSET(c,v0,v1,v0,v0); BSET(c,v0,v1,v0,v1); BSET(c,v0,v1,v1,v0); BSET(c,v0,v1,v1,v1); \
   BSET(c,v1,v0,v0,v0); BSET(c,v1,v0,v0,v1); BSET(c,v1,v0,v1,v0); BSET(c,v1,v0,v1,v1); \
   BSET(c,v1,v1,v0,v0); BSET(c,v1,v1,v0,v1); BSET(c,v1,v1,v1,v0); BSET(c,v1,v1,v1,v1); \
   DQ(q, switch(*u++){                                                                 \
    case B0000: *y++=vv[ 0]; break;  case B0001: *y++=vv[ 1]; break;                   \
    case B0010: *y++=vv[ 2]; break;  case B0011: *y++=vv[ 3]; break;                   \
    case B0100: *y++=vv[ 4]; break;  case B0101: *y++=vv[ 5]; break;                   \
    case B0110: *y++=vv[ 6]; break;  case B0111: *y++=vv[ 7]; break;                   \
    case B1000: *y++=vv[ 8]; break;  case B1001: *y++=vv[ 9]; break;                   \
    case B1010: *y++=vv[10]; break;  case B1011: *y++=vv[11]; break;                   \
    case B1100: *y++=vv[12]; break;  case B1101: *y++=vv[13]; break;                   \
    case B1110: *y++=vv[14]; break;  case B1111: *y++=vv[15]; break;                   \
   });                                                                                 \
   b=(B*)u; x=(T*)y; DQ(r, *x++=*b++?v1:v0;); v+=p;);                                  \
 }
#else
#define BNNERN(T)       {T*v=(T*)wv,*x=(T*)zv; DQ(m, b=av; DQ(an, *x++=v[*b++];); v+=p;);}
#define BNNERM(T,T1)    BNNERN(T)
#endif

#define INNER1B(T)  {T*v=(T*)wv,*x=(T*)zv; v+=*av; DQ(m, *x++=*v; v+=p;);}

// a is boolean
static F2(jtbfrom){A z;B*av,*b;C*wv,*zv;I acr,an,ar,k,m,p,*u=0,wcr,wf,wk,wn,wr,*ws,zn;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(ar>acr)R rank2ex(a,w,DUMMYSELF,acr,wcr,acr,wcr,jtbfrom);  // use rank loop if multiple cells of a
 an=AN(a); wn=AN(w); ws=AS(w);
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce domain error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
 p=wcr?ws[wf]:1;   // p=# items of w
#if !SY_64 && SY_WIN32
 I q=an>>LGSZI; I r=an&(SZI-1); 
#endif
 ASSERT(((p-2)&-(p|an))>=0||(p&all0(a)),EVINDEX);  // OK if p has >1 item, or if it has 0 but an is 0, or is a is all 0 and p has 1 item
 // We always need zn, the number of result atoms
 if(wn){
  // If there is data to move, we also need m: #cells of w   k: #bytes in an items of a cell of w   wk: #bytes in a cell of w
  PROD(m,wf,ws); PROD(k, wcr-1, ws+wf+1); zn=k*m; k<<=bplg(AT(w)); wk=k*p; DPMULDE(an,zn,zn);
 }else{zn=0;}
 GA00(z,AT(w),zn,ar+wr-(I )(0<wcr));
 MCISH(AS(z),AS(w),wf); MCISH(&AS(z)[wf],AS(a),ar); if(wcr)MCISH(&AS(z)[wf+ar],1+wf+ws,wcr-1);
 if(!zn)R z;  // If no data to move, just return the shape
 av=BAV(a); wv=CAV(w); zv=CAV(z);
 switch(k+k+(I )(1==an)){
  case   2*sizeof(I): BNNERN(I);   break;
  case   2*sizeof(C): BNNERM(C,I); break; 
  case 1+2*sizeof(C): INNER1B(C);  break;
  case 1+2*sizeof(S): INNER1B(S);  break;
#if SY_64
  case 1+2*sizeof(int): INNER1B(int);  break;
#endif
  case 1+2*sizeof(I): INNER1B(I);  break;
  default:
   if(1==an){wv+=k**av; DQ(m, MC(zv,wv,k); zv+=k; wv+=wk;);}
#if !SY_64 && SY_WIN32
   else{A x;C*v,*xv,*xv00,*xv01,*xv02,*xv03,*xv04,*xv05,*xv06,*xv07,*xv08,*xv09,*xv10,*xv11,
         *xv12,*xv13,*xv14,*xv15;I i,j,k4=k*4;
    GATV0(x,LIT,16*k4,1); xv=CAV(x);
    xv00=xv;       xv01=xv+   k4; xv02=xv+ 2*k4; xv03=xv+ 3*k4;
    xv04=xv+ 4*k4; xv05=xv+ 5*k4; xv06=xv+ 6*k4; xv07=xv+ 7*k4;
    xv08=xv+ 8*k4; xv09=xv+ 9*k4; xv10=xv+10*k4; xv11=xv+11*k4;
    xv12=xv+12*k4; xv13=xv+13*k4; xv14=xv+14*k4; xv15=xv+15*k4;
    for(i=0;i<m;++i){
     u=(I*)av; v=xv;
     BSETV(0); BSETV(0); BSETV(0); BSETV(0);   BSETV(0); BSETV(0); BSETV(0); BSETV(1);
     BSETV(0); BSETV(0); BSETV(1); BSETV(0);   BSETV(0); BSETV(0); BSETV(1); BSETV(1);
     BSETV(0); BSETV(1); BSETV(0); BSETV(0);   BSETV(0); BSETV(1); BSETV(0); BSETV(1);
     BSETV(0); BSETV(1); BSETV(1); BSETV(0);   BSETV(0); BSETV(1); BSETV(1); BSETV(1);
     BSETV(1); BSETV(0); BSETV(0); BSETV(0);   BSETV(1); BSETV(0); BSETV(0); BSETV(1);
     BSETV(1); BSETV(0); BSETV(1); BSETV(0);   BSETV(1); BSETV(0); BSETV(1); BSETV(1);
     BSETV(1); BSETV(1); BSETV(0); BSETV(0);   BSETV(1); BSETV(1); BSETV(0); BSETV(1);
     BSETV(1); BSETV(1); BSETV(1); BSETV(0);   BSETV(1); BSETV(1); BSETV(1); BSETV(1);
     for(j=0;j<q;++j,zv+=k4)switch(*u++){
      case B0000: MC(zv,xv00,k4); break;   case B0001: MC(zv,xv01,k4); break;
      case B0010: MC(zv,xv02,k4); break;   case B0011: MC(zv,xv03,k4); break;  
      case B0100: MC(zv,xv04,k4); break;   case B0101: MC(zv,xv05,k4); break;
      case B0110: MC(zv,xv06,k4); break;   case B0111: MC(zv,xv07,k4); break;
      case B1000: MC(zv,xv08,k4); break;   case B1001: MC(zv,xv09,k4); break;
      case B1010: MC(zv,xv10,k4); break;   case B1011: MC(zv,xv11,k4); break;
      case B1100: MC(zv,xv12,k4); break;   case B1101: MC(zv,xv13,k4); break;
      case B1110: MC(zv,xv14,k4); break;   case B1111: MC(zv,xv15,k4); break;
     }
     b=(B*)u; DQ(r, MC(zv,wv+k**b++,k); zv+=k;); wv+=wk;
   }}
#else
   else DQ(m, b=av; DQ(an, MC(zv,wv+k**b++,k); zv+=k;); wv+=wk;);
#endif
 }
 RETF(z);
}    /* a{"r w for boolean a */
#endif

// a is array whose 1-cells are index lists, w is array
// result is (<"1 a) { w
// wf is length of the frame
A jtfrombu(J jt,A a,A w,I wf){F2PREFIP;
 ARGCHK2(a,w)
 if(unlikely(!(AT(a)&INT)))RZ(a=cvt(INT,a));  // integral indexes required
 I *as=AS(a); I af=AR(a)-1; I naxa=as[af]; naxa=af>=0?naxa:1; af=af>=0?af:0;  // naxa is length of the index list, i. e. number of axes of w that disappear during indexing
 I nia; PROD(nia,af,as);     // number of 1-cells of a
 I wr=AR(w); I *ws=AS(w); I wcr=wr-wf;
 ASSERT(naxa<=wcr,EVLENGTH)  // the number of axes must not exceed the length of a major cell
 if(unlikely((-AN(a)&-AN(w))>=0)){A z;  // empty array, either a or w   scaf could avoid this?
  // if empty a, return w in full, one copy per 1-cell of a, as w"1 a   (}:$a) $ ,:w
  if(unlikely(AN(a)==0))R df1(z,a,qq(w,zeroionei(1)));
  // empty w. allocate empty result, move in shape: frame of w, frame of a, shape of item
  GA00(z,AT(w),0,wf+(wcr-naxa)+af); MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AS(a),af)  MCISH(AS(z)+wf+af,AS(w)+wf+naxa,wcr-naxa)
  R z;
 }
 // get #cells in selected portion, #cells indexed, and #atoms in the cell being indexed
 I wnk; PROD(wnk,wcr-naxa,ws+wf+naxa);  // #atoms in minor cell, below the indexing
 I ncx; PROD(ncx,naxa,ws+wf);  // # minor cells in major cell
 // calculate the rank of the result
 I zr=wr-naxa+af;  // remove the index axes from the result; if there are multiple indexes, keep their _1-axes
 // create the axis struct, with 1 or 2 axes.  The axis for the selectors is marked as having rank
 // 0 or 1, with the length of the axis being the combined size of the axes of the selectors
 // set up axis structs
 struct faxis axes[2];  // one for frame, one for data
 I m; PROD(m,wf,ws);  // #wcr-cells in w: tells if we need frame
 I wselsz; PROD(wselsz,wcr,ws+wf);  // #atoms in major cell of w
 axes[0].lenaxis=m; axes[0].lencell=wselsz; axes[0].nsel=m; axes[0].indsubx.ind=mtv; // fill in frame axis
 axes[0].currselx=0; axes[0].sels=0; axes[0].currselv=0; axes[0].sel0=0;  // fill in frame axis - should be 1 store
 I r=m>1;  // remember if frame is included as an axis.  #axes-1.  We need a leading axis in full if there are multiple cells of w
 // fill in last axis, for the indexes
 axes[r].lenaxis=ncx; axes[r].lencell=wnk; axes[r].nsel=nia;  // fill in selection axis, including # indexed items
 // convert a into an array of indexes
 //  first decide where to put the selectors
 I *indv;  // will point to where selectors will be put
 if(af==0){   // a has rank 0-1
  // the common case where a has a single atomic selector.  Build the sole selector in the axes block
  indv=&axes[r].sel0;  // there's only one number - put it into the initial-address spot
  axes[r].indsubx.ind=num(0);  // no result axes from this atomic selector - need any atom
 }else{
  A ind; GATV(ind,INT,nia,af,as)  // allocate a place for the results
  axes[r].indsubx.ind=ind; indv=IAV(ind);  // ind holds the values and the shape in the result
 }
 axes[r].sels=indv;  // point to the selectors from the axis block
 // now validate the indexes and calculate the selectors.  This is a short dot-product
 I *av=IAV(a); I *wsl=ws+wf;  // point to 1-cell of ind, and the axis lengths
 DO(nia, I s=0; DO(naxa, I v=av[i]; if((UI)v>=(UI)wsl[i]){v+=wsl[i]; ASSERT((UI)v<(UI)wsl[i],EVINDEX)} s=s*wsl[i]+v;) indv[i]=s; av+=naxa;)
 // sel0 not needed in last axis
 RETF(jtaxisfrom((J)((I)jtinplace&~JTINPLACEA),w,axes,((wcr-naxa)<<24)+(wf<<16)+(zr<<8)+r*0x81))  // move the values and return the result.  Pass through inplaceability of w, not a
#if 0   // obsolete

 fauxblockINT(pfaux,4,1); fauxINT(p,pfaux,h,1) v=AV(p)+h; u=ws+wf+h; m=1; DQ(h, *--v=m; m*=*--u;);  // m is number of items in the block of axes that index into w
 r=wr+1-h;  // rank of intermediate w arg is rank of w, minus h axes that go away and are replaced by 1 axis
 // We will use pdt to create an index to the cell
 A ind; RZ(ind=pdt(a,p));
 PROLOG(777);
 if(r==wr){
  q=w;
 }else{
  //  reshape w to combine the first h axes of each cell.  Be careful with the copying, because w and q may be the same block.  Some axes may be closed up by the copy
  RZ(q=virtualip(w,0,r)); AN(q)=AN(w); v=AS(q); MCISH(v,ws,wf); v[wf]=m; MCISH(v+wf+1,ws+wf+h,wcr-h);  /* q is reshape(.,w) */
 }
 IRS2(ind,q,0, RMAX,wcr+1-h,jtifrom,z);
 EPILOG(z);  // we have to release the virtual block so that w is inplaceable later on in the sentence
#endif
}    /* (<"1 a){"r w, dense w, integer array a */
#if 0  // obsolete


// needed for sparse
static B jtaindex1(J jt,A a,A w,I wf,A*ind){A z;I c,i,k,n,t,*v,*ws;
 ARGCHK2(a,w);
 n=AN(a); t=AT(a); *ind=0; if(AR(a)==0)R 0;  // revert to normal code for atomic a
 ws=wf+AS(w); c=AS(a)[AR(a)-1];   // c = length of 1-cell
 if(((n-1)|(c-1)|SGNIF(t,BOXX))<0)R 0;  // revert to normal code for empty or boxed a
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 PROD(n,AR(a)-1,AS(a));  v=AV(a); // n now=number of 1-cells of a   v=running pointer through a
 // Go through a fast verification pass.  If all values nonnegative and valid, return original a
 if(t&INT){  // if it's INT already, we don't need to move it.
  switch(c){I c0,c1,c2;
  case 2:
   c0=ws[0], c1=ws[1]; for(i=n;i>0;--i){AUDITPOSINDEX(v[0],c0) AUDITPOSINDEX(v[1],c1)  v+=2;} break;
  case 3:
   c0=ws[0], c1=ws[1], c2=ws[2]; for(i=n;i>0;--i){AUDITPOSINDEX(v[0],c0) AUDITPOSINDEX(v[1],c1) AUDITPOSINDEX(v[2],c2) v+=3;} break;
  default:
   for(i=n;i>0;--i){DO(c, k=*v; AUDITPOSINDEX(k,ws[i]) ++v;); if(k<0)break;} break; 
  }
 }else i=1;  // if not INT to begin with, we must convert
 if(likely(i==0)){z=a;  // If all indexes OK, return the original block
 }else{
  // There was a negative index.  Allocate a new block for a and copy to it.  It must be writable
  RZ(z=ISDENSETYPE(t,INT)?ca(a):cvt(INT,a)); v=AV(z);
  DQ(n, DO(c, SETNDXRW(k,*v,ws[i]) ++v;););  // convert indexes to nonnegative & check for in-range
 }
 *ind=z;
 R 1;
}    /* verify that <"1 a is valid for (<"1 a){w - used only for sparse matrices */

static A jtafrom2(J jt,A p,A q,A w,I r){A z;C*wv,*zv;I d,e,j,k,m,n,pn,pr,* RESTRICT pv,
  qn,qr,* RESTRICT qv,* RESTRICT s,wf,wr,* RESTRICT ws,zn;
 wr=AR(w); ws=AS(w); wf=wr-r; 
 pn=AN(p); pr=AR(p); pv=AV(p);
 qn=AN(q); qr=AR(q); qv=AV(q);
 if(AN(w)){
  // Set zn=#atoms of result  d=#atoms in a _2-cell of cell of w
  // e=length of axis corresponding to q  n=#_2-cells in a cell of w   m=#cells of w (frame*size of 2-cell*(# _2-cells = pn*qn))
  PROD(m,wf,ws); PROD(d,r-2,ws+wf+2); e=ws[1+wf]; n=e*ws[wf]; DPMULDE(qn,d*m,zn) DPMULDE(pn,zn,zn);
 }else{zn=0;}
 GA00(z,AT(w),zn,wf+pr+qr+r-2); MCISH(AS(z),ws,wf)   // don't overfetch AS(w)
 s=AS(z)+wf; MCISH(s,AS(p),pr); MCISH(s+pr,AS(q),qr); MCISH(s+pr+qr,ws+wf+2,r-2);
 if(!zn)R z;  // If no data to move, exit with empty.  Rank is right
 wv=CAV(w); zv=CAV(z); 
 switch(k=d<<bplg(AT(w))){   // k=*bytes in a _2-cell of a cell of w
#define INNER2(T) {T* RESTRICT v=(T*)wv,* RESTRICT x=(T*)zv;   \
   DQ(m, DO(pn, j=e*pv[i]; DO(qn, *x++=v[j+qv[i]];         )); v+=n;); break;} // n=#_2-cells in a cell of w.
 case sizeof(I): INNER2(I);
 case sizeof(C): INNER2(C);
 case sizeof(S): INNER2(S);
#if SY_64
 case sizeof(I4): INNER2(I4);
#endif
#if !SY_64 && SY_WIN32
 case sizeof(D): if(AT(w)&FL)INNER2(D);
  // copy only echt floats using floating-point moves.  Otherwise fall through to...
#endif
 default:        {C* RESTRICT v=wv,* RESTRICT x=zv-k;n=k*n;   // n=#bytes in a cell of w
  JMCDECL(endmask) JMCSETMASK(endmask,k,0) 
  DQ(m, DO(pn, j=e*pv[i]; DO(qn, x+=k; JMCR(x,v+k*(j+qv[i]),k,0,endmask);)); v+=n;);} break;
 }
 RETF(z);   // return block
}   /* (<p;q){"r w  for positive integer arrays p,q */
#endif
#if 0 // obsolete 


// n is length of axis, w is doubly-unboxed selector
// result is list of selectors - complementary if w is boxed, with a: standing for axis taken in full
static A jtafi(J jt,I n,A w){A x;
 if((-AN(w)&SGNIF(AT(w),BOXX))>=0)R pind(n,w);   // empty or not boxed
 R jtcompidx(jt,n,w);
// obsolete  ASSERT(!AR(w),EVINDEX);  // if boxed, must be an atom
// obsolete  x=C(AAV(w)[0]);
// obsolete  R AN(x)?less(IX(n),pind(n,x)):ds(CACE);   // complementary
}
#endif

// general boxed a
static F2(jtafrom){F2PREFIP; PROLOG(0073);
 ARGCHK2(a,w);
 I ar=AR(a); I acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 I wr=AR(w); I wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; I wf=wr-wcr; RESETRANK;
 // We have IRS on w, but not a.  If there are multiple cells of a we use a rank loop.
 if(unlikely(ar!=0)){A t0,t1,t2;  // if there is an array of boxes
  // Since the native rank of a is 0, if w has only one cell we can loop over boxes of a
// obsolete   if(((ar^acr)|(wr^wcr))==0){RZ(ind=aindex(a,w,wf)); ind=(A)((I)ind&~1LL); if(ind)R frombu(ind,w,wf);}  // if boxing doesn't contribute to shape, open the boxes of a and copy the values
  R wr==wcr?rank2ex(a,w,DUMMYSELF,0L,wcr,0L,wcr,jtafrom):  // if a has frame, rank-loop over a
    // If both a and w have cells, we must match them up by boxing
      df2(t0,IRS1(a,0L,acr,jtbox,t1),IRS1(w,0L,wcr,jtbox,t2),amp(ds(CLBRACE),ds(COPE)));  // > (<"arank a) {&> <"wrank w
 }
 // a is an atomic box.  Open it
 A c=C(AAV(a)[0]);  // contents of a
// obsolete  if(!(AT(c)&BOX)){ASSERT(AR(c)<=1,EVRANK) R jtfrombu(jtinplace,c,w,wf);}  // if single-boxed, handle as <"1@[ { ].
 if(!(AT(c)&BOX)){R jtfrombu(jtinplace,c,w,wf);}  // if single-boxed, handle as <"1@[ { ].
 // Double-boxed. Set up axis structs
 // We DO NOT treat leading scalar indexes as a special case here.  Building & using the axis block is pretty cheap.  We catch them when we fill.
 ASSERT(1>=AR(c),EVRANK);  // boxes may not have rank > 1
 ASSERT(AN(c)<=wcr,EVLENGTH);  // number of axes must not exceed #axes in major cell
 I *ws=AS(w);  // #axes-1.  We need a leading axis in full if there are multiple cells of w
 struct faxis stataxes[5], *axes;  // one for frame, several for data
 I *cmbase;  // start of area we can use for complementary bitmasks
 UI naxesreq=AN(c)+!!wf;  // max # axes we might need
 if(likely(naxesreq<=sizeof(stataxes)/sizeof(stataxes[0]))){axes=stataxes; cmbase=(I*)&stataxes[naxesreq]; }
 else{A t; GATV0(t,INT,naxesreq*(sizeof(stataxes[0])>>LGSZI),1) axes=(struct faxis *)IAV1(t); cmbase=(I*)&stataxes[0];}  // rank 1 for alignment
  // In case user specified rank, we fill in a single axis for the frame.  If there is only 1 frame cell we will overwrite the axis
 I m; PROD(m,wf,ws);  // #wcr-cells in w: tells if we need frame
 axes[0].lenaxis=m; axes[0].nsel=m; axes[0].indsubx.ind=mtv;
 axes[0].currselx=0; axes[0].sels=0; axes[0].currselv=0; axes[0].sel0=0;  // fill in frame axis - should be 1 store
 I hasr=m>1;  // remember if frame is included as an axis
 // process axes from back to front, building up cell sizes
 I wncr=wcr-AN(c);  // rank of cell indexed by last axis
 I i,celllen; PROD(celllen,wncr,ws+wf+AN(c))  // #atoms in cell of last selection
 I r=hasr+AN(c)-1;  // number of axis structs-1, init to all axes
 // point to axis area, using local if possible
 A *av=AAV(c);  // the boxes of c
 I zr=wf+wncr;  // result shape, frame plus cell.  The rest is ranks of a selectors
 for(i=r;i>=hasr;--i){  // for each axis, starting at the end
  A aa=AAV(c)[i-hasr];  // next box to process
  axes[i].lenaxis=ws[wf+i-hasr]; axes[i].lencell=celllen;  // fill in axis info
  celllen*=ws[wf+i-hasr];  // get len of cell of next axis, which includes length of this cell
  // fill in the axis struct
  if(AT(aa)&BOX){
   // triple-boxed, which is taken-in-full or complementary
   ASSERT(AR(aa)==0,EVRANK)  // triple-boxed, last box must be an atom
   aa=AAV0(aa)[0];  // fetch the triple-boxed contents
   ASSERT(AR(aa)<=1,EVRANK)  // which must be atom/list
   if(unlikely(!ISDENSETYPE(AT(aa),INT))){  // and must be numeric
    if(AR(aa)<(AT(aa)&B01))aa=zeroionei(BAV(aa)[0]);else RZ(aa=cvt(INT,aa));  // convert boolean or other arg to int, with special check for scalar boolean
   }
   if(AN(aa)==0){
    // axis taken in full.  fill it in that way
    axes[i].nsel=axes[i].lenaxis;   // fill taken-in-full axis
    axes[i].currselx=0; axes[i].sels=0; axes[i].currselv=0; axes[i].sel0=0;  // fill in startpt - should be 1 store    if(unlikely(i==naxes))--naxes;  // taken-in-full cannot be last axis
    if(unlikely(i==r)){--r; ++wncr;}  // taken-in-full cannot be last axis.  treat as if omitted, i. e. as if AN(c) decremented
   }else{
#if 0  // obsolete
    // complementary axis.  Convert it to a bitmask of the elements that survive; get their count and first index
    I axn=axes[i].lenaxis;  // length of axis
    I cn=(axn+(BW-1))>>LGBW, *cmv;  // number of words needed for bitmask; base of mask
    // decide where to build the mask - in stataxes if possible
    if(cn<=((I*)&stataxes[sizeof(stataxes)/sizeof(stataxes[0])]-cmbase)){cmv=cmbase; cmbase+=cn;}  // fits in spare space in axes
    else{A t; GATV0(t,INT,cn,0) cmv=IAV0(t);}  // allocation required
    mvc(cn*SZI,cmv,SY_64?4*SZI:2*SZI,validitymask);  // init to all present
    I *iv=IAV(aa), n1=axn;  // pointer to indexes, and init count of 1s to 'all'
    DO(AN(aa), I ix=iv[i];  // fetch index
                if((UI)ix>=(UI)axn){ix+=axn; ASSERT((UI)ix<(UI)axn,EVINDEX)}  // adjust neg, check for validity
                I incumb=cmv[ix>>LGBW]; I bit=1ll<<(ix&(BW-1)); cmv[ix>>LGBW]=incumb&~bit; n1-=!!(incumb&bit); // turn off
      )  // turn off the bit for each index, count number of 1s encountered
    axes[i].nsel=~n1; axes[i].sels=cmv;  // remember # of 1s (complement indicates complementary axis), location of mask
    if(likely(n1!=0)){for(n1=0;*cmv==0;n1+=BW,++cmv); axes[i].sel0=n1+CTTZI(*cmv);}  // find first 1
#else
    // complementary axis.  Sort it
    I axn=axes[i].lenaxis, an=AN(aa), *cmv;  // length of axis, number of selectors, place to sort into
    // copy into stataxes if possible, otherwise allocate (inplacing never possible)
    if(an<=((I*)&stataxes[sizeof(stataxes)/sizeof(stataxes[0])]-cmbase)){cmv=cmbase; cmbase+=an;}  // fits in spare space in axes
    else{A t; GATV0(t,INT,an,0) cmv=IAV0(t);}  // allocation required
    // insertion sort the audited indexes into *cmv, discarding duplicates
    I *iv=IAV(aa), cmvn=0;  // pointer to input, number of unique values sorted already
    DO(AN(aa), I ix=iv[i]; ix+=REPSGN(ix)&axn; ASSERT((UI)ix<(UI)axn,EVINDEX)  // fetch, resolve neg, check range
     I ins; for(ins=cmvn-1;ins>=0;--ins){if(ix==cmv[ins])goto dup; if(ix>cmv[ins])break; cmv[ins+1]=cmv[ins];}  // find insertion point
     cmv[ins+1]=ix; ++cmvn; if(0){dup: for(++ins;ins<cmvn;++ins)cmv[ins]=cmv[ins+1];} // if dup, reclose insertion point.  Do it each time since it probably never happens
    )
    axes[i].nsel=~(axn-cmvn); axes[i].sels=cmv;  // remember # of sels (complement indicates complementary axis), location of mask
    I gap; for(gap=0;gap<cmvn;++gap)if(cmv[gap]!=gap)break; axes[i].sel0=gap;  // find first index to write to
#endif
   }
   axes[i].indsubx.ind=mtv; ++zr;  // triple-boxed selector gets one result axis
  }else{
   // normal axis.
   if(unlikely(!ISDENSETYPE(AT(aa),INT))){
    if(AR(aa)<(AT(aa)&B01))aa=zeroionei(BAV(aa)[0]);else RZ(aa=cvt(INT,aa));  // convert boolean or other arg to int, with special check for scalar boolean
   }
   axes[i].nsel=AN(aa); axes[i].sels=IAV(aa); axes[i].sel0=0; axes[i].indsubx.ind=aa;
   // audit and translate first index to speed reset
   if(likely(axes[i].nsel!=0)){I sel0=axes[i].sels[0]; sel0+=REPSGN(sel0)&axes[i].lenaxis; ASSERT((UI)sel0<(UI)axes[i].lenaxis,EVINDEX) axes[i].sel0=sel0;}
   zr+=AR(aa);  // each axis of selectors goes into the result
  }
 }
 if(unlikely(r<hasr))R RETARG(w);  // if all axes taken in full, do nothing, return full w
 I *rla=&axes[0].lencell; rla=hasr?rla:jt->shapesink; *rla=celllen;  // if there is frame, it needs len of a major cell
 RETF(jtaxisfrom((J)((I)jtinplace&~JTINPLACEA),w,axes,(wncr<<24)+(wf<<16)+(zr<<8)+(hasr<<7)+r))  // move the values and return the result
#if 0
 s=AS(w)+wr-wcr;
 ASSERT(1>=AR(c),EVRANK);
 ASSERT(n<=wcr,EVLENGTH);
 if((-n&SGNIFNOT(t,BOXX))<0){RZ(ind=aindex(a,w,wf)); ind=(A)((I)ind&~1LL); if(ind)R frombu(ind,w,wf);}  // not empty and not boxed, handle as 1 index list
 if(wcr==wr){
  for(i=m=pr=0;i<n;++i){
   p=afi(s[i],C(v[i]));
   if(!(p&&(((AN(p)^1)-1)&-(AT(p)&NUMERIC))<0))break;  // if 1 selection from numeric axis, do selection here, by adding offset to selected cell
   pr+=AR(p); 
   RANKT rsav=AR(p); AR(p)=0; I px; PRODX(px,wcr-i-1,1+i+s,i0(p)) m+=px; AR(p)=rsav;  // kludge but easier than creating a fauxvirtual block
  }
 }
 if(i){I*ys;
  RZ(y=virtual(w,m,pr+wcr-i));
  ys=AS(y); DO(pr, *ys++=1;); MCISH(ys,s+i,wcr-i);
  I temp; PROD(temp,AR(y),AS(y)); AN(y)=temp;
 }
 // take remaining axes 2 at a time, properly handling omitted axes (ds(CACE)).  First time through p is set if there has been no error
 for(;i<n;i+=2){
  j=1+i; if(!p)p=afi(s[i],C(v[i])); q=j<n?afi(s[j],C(v[j])):ds(CACE); if(!(p&&q))break;  // pq are 0 if error.  Result of ds(CACE)=axis in full
  if(p!=ds(CACE)&&q!=ds(CACE)){y=afrom2(p,q,y,wcr-i);}
  else{
   if(q==ds(CACE)){q=p; j=i;}
   if(q!=ds(CACE))y=IRS2(q,y,0L,AR(q),wcr-j,jtifrom,w);
  }
  RZ(y); p=0;
 }
 // We have to make sure that a virtual NJA block does not become the result, because x,y and x u}y allow modifying NJAs even when the usecount is 1 or even 2.  Realize in that case
 RE(y); if(unlikely(AFLAG(y)&AFNJA)){RZ(y=ca(y));}   // If the result is NJA, it must be virtual (otherwise we allocated a new block here)
 EPILOG(y);
#endif
}    /* a{"r w for boxed index a */

DF2(jtfrom){I at;A z;
 F2PREFIP;
 ARGCHK2(a,w);
 at=AT(a);
 if(likely(!ISSPARSE(AT(a)|AT(w)))){
  // if B01|INT|FL atom { INT|FL|BOX array, and no frame, just pluck the value.  If a is inplaceable and not unincorpable, use it
  // If we turn the result to BOX it will have the original flags, i. e. it will be nonrecursive.  Thus fa will not free the contents, which do not have incremented usecount (and are garbage on error)
  // We allow FL only if it is the same size as INT
  if(!((AT(a)&(NOUN&~(B01|INT|(SY_64*FL))))+(AT(w)&(NOUN&~(INT|(SY_64*FL)|BOX)))+AR(a)+(SGNTO0((((RANKT)jt->ranks-AR(w))|(AR(w)-1))))+(AFLAG(w)&AFNJA))){   // scaf NJAwhy
   I av;  // selector value
   if(likely(!SY_64||AT(a)&(B01|INT))){av=BIV0(a);  // INT index
   }else{  // FL index
    D af=DAV(a)[0], f=jround(af); av=(I)f;
    ASSERT(ISFTOIOK(f,af),EVDOMAIN);  // if index not integral, complain.  IMAX/IMIN will fail presently.  We rely on out-of-bounds conversion to peg out one side or other (standard violation)
   }
   I wr1=AR(w)-1;
   if(wr1<=0){  // w is atom or list, result is atom
    // Get the area to use for the result: the a input if possible, else an INT atom. a=w OK!
    if((SGNIF(jtinplace,JTINPLACEAX)&AC(a)&SGNIFNOT(AFLAG(a),AFUNINCORPABLEX))<0)z=a; else{GAT0(z,INT,1,0)}
    // Move the value and transfer the block-type
    I j; SETNDX(j,av,AN(w)); IAV(z)[0]=IAV(w)[j]; AT(z)=AT(w);   // change type only if the transfer succeeds, to avoid creating an invalid a block that eformat will look at
    // Here we transferred an atom out of w.  We must mark w non-pristine.  If it was inplaceable, we can transfer the pristine status.  We overwrite w because it is no longer in use
    PRISTXFERF(z,w)  // this destroys w
   }else{
    // rank of w > 1, return virtual cell
    I *ws=AS(w);  // shape of w
    I m; PROD(m,wr1,ws+1);  // number of atoms in a cell
    I j; SETNDX(j,av,ws[0]);  // j=positive index
    RZ(z=virtualip(w,j*m,wr1));   // if w is rank 2, could reuse inplaceable a for this virtual block
    // fill in shape and number of atoms.  ar can be anything.
    AN(z)=m; MCISH(AS(z),ws+1,wr1)
    // When we create a virtual block we do not actually copy anything out of w, so it remains pristine.  The result is not.
   }
  }else{
   // not atom{array.  Process according to type of a
    RANK2T origranks=jt->ranks;  // remember original ranks in case of error
// obsolete   A (*fn)(J,A,A);
// obsolete   fn=jtifrom; fn=at&BOX?jtafrom:fn; fn=at&(AN(a)!=1)?jtbfrom:fn; jtinplace=fn!=jtifrom?jt:jtinplace;  // boxed; boolean (not singleton); other (will be converted to INT)
// obsolete   z=(*fn)(jtinplace,a,w);
   if(!(at&BOX))z=jtifrom(jtinplace,a,w);else z=jtafrom(jtinplace,a,w);
   // If there was an error, call eformat while we still have the ranks.  convert default rank back to R2MAX to avoid "0 0 in msg
// obsolete   if(unlikely(jt->jerr)){jt->ranks=origranks!=RMAX?origranks:R2MAX; jteformat(jt,self,a,w,0); RESETRANK;}
   if(unlikely(z==0)){jt->ranks=origranks!=RMAX?origranks:R2MAX; jteformat(jt,self,a,w,0); RESETRANK; R0}
   // Here we transferred out of w.  We must mark w non-pristine unless the result was virtual
   // Since there may have been duplicates, we cannot mark z as pristine.  We overwrite w because it is no longer in use
   if(!(AFLAG(z)&AFVIRTUAL))PRISTCLRF(w)
  }
 }else if(ISSPARSE(AT(a)&AT(w))){z=fromss(a,w);}  // sparse cases
 else if(ISSPARSE(AT(w))){z=at&BOX?frombs(a,w) : fromis(a,w);}
 else{z=fromsd(a,w);}
 RETF(z);
}   /* a{"r w main control */

F2(jtsfrom){
 if(likely(!ISSPARSE((AT(w))))){
  // not sparse, transfer to code to handle it if nonempty numeric.  If nonnumeric (must be boxed),
  // if the arg is empty, the details are tricky depending on the type and we just revert to boxed code
  if(likely(((AN(a)-1)|((AT(a)&NUMERIC)-1))>=0))R jtfrombu(jt,a,w,0);  // handle if a is nonempty numeric
#if 0   // obsolete
  // Not sparse.  Verify the indexes are numeric and not empty
  if(((AN(a)-1)|((AT(a)&NUMERIC)-1))>=0){A ind;   // a is a numeric array
   // Check indexes for validity; if valid, turn each row into a cell offset
   if(ind=jtcelloffset((J)((I)jt+JTCELLOFFROM),w,a,0)){
    // Fetch the cells and return.  ind is now an array of cell indexes.  View w as an array of those cells
    // We could do this with ifrom, but it validates the input and checks for virtual, neither of which is germane here.  Also, we would have
    // to reshape w into an array of cells.  Easier just to copy the data right here
    // Find the number of axes included in each cell offset; get the cell size
    I cellsize; PROD(cellsize,AR(w)-AS(a)[AR(a)-1],AS(w)+AS(a)[AR(a)-1]);  // number of atoms per index in ind
    I * RESTRICT iv=AV(ind);  // start of the cell-index array
    A z; GA00(z,AT(w),cellsize*AN(ind),AR(ind)+AR(w)-AS(a)[AR(a)-1])  MCISH(AS(z),AS(ind),AR(ind)) MCISH(AS(z)+AR(ind),AS(w)+AS(a)[AR(a)-1],AR(w)-AS(a)[AR(a)-1])  // shape from ind, then w
    cellsize <<= bplg(AT(w));   // change cellsize to bytes
    switch(cellsize){
    case sizeof(C):
     {C * RESTRICT zv=CAV(z); C *RESTRICT wv=CAV(w); DQ(AN(ind), *zv++=wv[*iv++];) break;}  // scatter-copy the data
    case sizeof(I):  // may include D
     {I * RESTRICT zv=IAV(z); I *RESTRICT wv=IAV(w); DQ(AN(ind), *zv++=wv[*iv++];) break;}  // scatter-copy the data, 8-byte chunks
    default: ;
     // It is OK to pad to an I boundary, because any block with cells not a multiple of I is padded to an I
     C* RESTRICT zv=CAV(z); C *RESTRICT wv=CAV(w);     JMCDECL(endmask) JMCSETMASK(endmask,cellsize,0) 
     DQ(AN(ind), JMCR(zv,wv+*iv++*cellsize,cellsize,0,endmask); zv+=cellsize;)
     break;
    }
    RETF(z);
   }
  }
#endif
 }else{
  // sparse.  See if we can audit the index list.  If we can, use it, else execute the slow way
  A z;I c,i,k,n,t,*v,*ws;
  n=AN(a); t=AT(a); if(AR(a)==0)goto mustbox;  // revert to normal code for atomic a
  ws=AS(w); c=AS(a)[AR(a)-1];   // c = length of 1-cell
  if(((n-1)|(c-1)|SGNIF(t,BOXX))<0)goto mustbox;  // revert to normal code for empty or boxed a
  ASSERT(c<=AR(w),EVLENGTH);
  PROD(n,AR(a)-1,AS(a));  v=AV(a); // n now=number of 1-cells of a   v=running pointer through a
  // Go through a fast verification pass.  If all values nonnegative and valid, return original a
  if(t&INT){  // if it's INT already, we don't need to move it.
#define AUDITPOSINDEX(x,lim) if(!BETWEENO((x),0,(lim))){if((x)<0)break; ASSERT(0,EVINDEX);}
   switch(c){I c0,c1,c2;
   case 2:
    c0=ws[0], c1=ws[1]; for(i=n;i>0;--i){AUDITPOSINDEX(v[0],c0) AUDITPOSINDEX(v[1],c1)  v+=2;} break;
   case 3:
    c0=ws[0], c1=ws[1], c2=ws[2]; for(i=n;i>0;--i){AUDITPOSINDEX(v[0],c0) AUDITPOSINDEX(v[1],c1) AUDITPOSINDEX(v[2],c2) v+=3;} break;
   default:
    for(i=n;i>0;--i){DO(c, k=*v; AUDITPOSINDEX(k,ws[i]) ++v;); if(k<0)break;} break; 
   }
  }else i=1;  // if not INT to begin with, we must convert
  if(likely(i==0)){z=a;  // If all indexes OK, return the original block
  }else{
   // There was a negative index.  Allocate a new block for a and copy to it.  It must be writable
   RZ(z=ISDENSETYPE(t,INT)?ca(a):cvt(INT,a)); v=AV(z);
   DQ(n, DO(c, SETNDXRW(k,*v,ws[i]) ++v;););  // convert indexes to nonnegative & check for in-range
  }
  R frombsn(z,w,0L);  // handle the special case
mustbox:;
 }
 // If we couldn't handle it as a special case, do it the hard way
 A z; RETF(from(IRS1(a,0L,1L,jtbox,z),w));
}    /* (<"1 a){w */

static F2(jtmapx);
static EVERYFS(mapxself,0,jtmapx,0,VFLAGNONE)

static F2(jtmapx){A z1,z2,z3;
 ARGCHK2(a,w);
 if(!(BOX&AT(w)))R ope(a);
 RZ(z1=catalog(every(shape(w),ds(CIOTA))));  // create index list of each box
 IRS1(z1,0,0,jtbox,z2);
 RZ(z2=every2(a,z2,(A)&sfn0overself));
 IRS1(z2,0,0,jtbox,z3);
 R every2(z3,w,(A)&mapxself);
}

F1(jtmap){R mapx(ds(CACE),w);}

// extract the single box a from w and open it.  Don't mark it no-inplace.  If w is not boxed, it had better be an atom, and we return it after auditing the index
static F2(jtquicksel){I index;
 RE(index=i0(a));  // extract the index
 SETNDX(index,index,AN(w))   // remap negative index, check range
 R AT(w)&BOX?C(AAV(w)[index]):w;  // select the box, or return the entire unboxed w
}

DF2(jtfetch){A*av, z;I n;F2PREFIP;
 F2RANKW(1,RMAX,jtfetch,self);  // body of verb applies to rank-1 a, and must turn pristine off if used higher, since there may be repetitions.
 if(!(BOX&AT(a))){
  // look for the common special case scalar { boxed vector.  This path doesn't run EPILOG
  if(((AT(w)>>BOXX)&1)>=(2*AR(a)+AR(w))){  // a is an atom, w is atom or list of boxes   AR(a)==0 && (AR(w)==0 || (AR(w)==1 && AT(w)&BOX))
   RZ(z=jtquicksel(jt,a,w));  // fetch selected box, opened.  If not a box, just return w
   // if the result is w, leave its inplaceability.  If contents, it will perforce be uninplaceable
#if AUDITBOXAC
   if(!(AFLAG(w)&AFVIRTUALBOXED)&&AC(z)<0)SEGFAULT;
#endif
   // Since the whole purpose of fetch is to copy one contents by address, we turn off pristinity of w
   PRISTCLRF(w)
   RETF(z);   // turn off inplace if w not inplaceable, or jt not inplaceable.
  }
  RZ(a=jtbox(JTIPAtoW,a));  // if not special case, box any unboxed a
 }
 n=AN(a); av=AAV(a); 
 if(!n)R w; z=w;
 DO(n, A next=C(av[i]); if(((AT(z)>>BOXX)&1)>=(2*(AR(next)+(AT(next)&BOX))+AR(z))){RZ(z=jtquicksel(jt,next,z))}  // next is unboxed atom, z is boxed atom or list, use fast indexing  AR(next)==0 && !(AT(next)&BOX) && (AR(z)==0 || (AR(z)==1 && AT(z)&BOX))
      else{RZ(z=afrom(box(next),z)); ASSERT(((i+1-n)&-AR(z))>=0,EVRANK); if(((AR(z)-1)&SGNIF(AT(z),BOXX))<0)RZ(z=ope(z));}  // Rank must be 0 unless last; open if boxed atom
   );
 // Since the whole purpose of fetch is to copy one contents by address, we turn off pristinity of w
 PRISTCLRF(w)
 RETF(z);   // Mark the box as non-inplaceable, as above
}

#if C_AVX2
#if 0  // obsolete 
// see if row,col is in exclusion list.  Exclusion list is a list of col|col with the smaller value in the high-order part
// Result is 0 if OK to accept the combination
static int notexcluded(I *exlist,I nexlist,I col,I row){I colrow=(col<row)?(col<<32)+row:(row<<32)+col;  // canonicalize column numbers into one value
 while(nexlist--)if(*exlist++==colrow)R 0;
 R 1;
}
#endif

// everything we need for one core's execution - shared by all cores
struct __attribute__((aligned(CACHELINESIZE))) mvmctx {
 // returned section
 union {
  D D;
  I I;  // we use integer arithmetic to compare floats, so the  float values must be nonnegative.  Also, we pun integer 0 = fp 0
 } sharedmin;   // minimum (=best) improvement found in any previous column, including in other threads, set to -inf to abort other threads
// obsolete  D maxdanger;  // if we have not found a non-dangerous pivot, we want to take the one with the largest column value acroos threads
 I retinfo;  // returned data, col (if gradient) or row (if onecol)
   // during the search, bit 63 indicates dangerous pivot
 I nimpandcols;  // number of improvement,number of cols finished
 I ndotprods;  // total # dotproducts evaluated
 // shared section
 UI4 nextresv;  // index of next reservation. Advanced by atomic operations.  For one-column, this is a row; for gradient, a column
 US ctxlock;  // used to lock changes
 I filler[3];  // pad 1st cacheline so we don't get false sharing
 // the rest is moved into static names
 UI *ndxa;   // the column indexes, always a list of ndxs
// obsolete  UI4 qkrowstride;  // }:$Qk, which will be padded to row multiple
 UI4 nc;  // #cols in ndxa
 UI4 bkzstride;  // length of last axis of bk/bkbeta/z: length of extended kb/beta/z, which includes Fk if present and is padded to batch boundary
 I4 nbasiswfk;  // |value| is number of rows allocated in Qk; if negative, is 1's-complement of # rows of Qk proper, and Fk is also a row of Qk
 I4 nthreads;  // number of threads when we started this job (may be inaccurate; for tuning only)
// obsolete  frowbatchx;  // if Fk is in Qk, this is the value rowx will have when it is processed.  If Fk not included, ~0.  onecol only - gradient simply uses smaller n
// obsolete  I qkstride;  // distance in atoms between halves of Qk
 D *bk;  // pointer to bk.  If 0, this is a column extraction (if zv!=0)
 D *bkbeta;  // beta values for the rows in bk.  The values don't change, but which ones are in bk do
 D *Frow;  // the selector row
 D *zv; // pointer to output for product mode, 0 for gradient mode
 D *parms;  // parm info
// obsolete I *bvgrd0, *bvgrde;  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk
 D *bndrowmask;  // for each row in bk, 1 bit/value, indicating bound vbl  Ordered to be right for each bytelane.
 I (*axv)[2];  // pointer to ax data, (index,length)
 I *amv0;  // pointer to am block, and to the selected column's indexes
 D *avv0;  // pointer to av data, and to the selected column's values
 A qk;  // original M block
// obsolete  A sched;  // ending schedule, INT vector.  After the ith improvement, exit if i>:#sched or we have examined (<:i){sched columns
// obsolete  A bkbound;  // b-beta for the bound variables; also gives the number of bound variables, which must be first in bv
 C *rvtv;  // list of Rose variable types for the columns (unchanging)
 D (*cutoffstatus)[2];  // (ncols,2) $ array holding (number of rows processed before cutoff, 0 if row invalid,gradient total for those rows).  If cutoff total is negative, it means that no positive column value has been seen
};
// obsolete #define ONECOLGRD0 ((I*)(NPAR*SZI))  // starting value of bvgrd for onecol, offset from 0 so that backing up bvgrde won't wrap around 0

#define BNDROWBATCH 64   // the number of bits of bndrowmask that we load at a time.
#define LGBNDROWBATCH 6
#define CMPBATCH 64  // frequency with which we compare for cutoff.  MUST NOT BE SMALLER than BNDROWBATCH so that we restart on a bndrowmask bdy
  // We collect the totals for every compare batch.  If we are using low precision for the gradient, we should collect totals often enough that we don't
  // lose precision in each lane

// the processing loop for one core.  We take a slice of the columns/rows, repeatedly
// ti is the job#, not used except to detect error
static unsigned char jtmvmsparsex(J jt,struct mvmctx *ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(ndxa)YC(nbasiswfk)YC(nc)YC(bkzstride)YC(nthreads)YC(bk)YC(parms)YC(zv)
 YC(bndrowmask)YC(cutoffstatus)YC(axv)YC(amv0)YC(avv0)YC(qk)YC(Frow)YC(bkbeta)YC(rvtv)
#undef YC
 // perform the operation
 I minvalueweshared=IMAX;  // the smallest value we have shared
 I retinfo;  // the return data for the best value we shared (col if gradient, row if onecol)
// obsolete D minimpfound=0.0;  // minimum (=best) improvement found so far from a non-dangerous pivot
 I ndotprods=0;  // number of dot-products we perform here
 I frowbatchx;  // in onecol, rowx of batch containing Fk, if any
 D sharedmin;  // minimum value fetched from other threads (either SPR or gradient)
 I qkrowstride=AS(qk)[2];  // length of allocated row of Qk, padded to batch multiple
 I nrows=AS(qk)[1];  // number of rows in Qk.  The last might be Fk, depending on the basis info; we calculate the dot-product for them all, but not the SPR on Fk
 I qkstride=qkrowstride*nrows;  // distance between planes of Qk
 __m256i bndmskshift=_mm256_set_epi64x(0,16,32,48);  // bndmsk is littleendian, but reversed in 16-bit sections.  last section goes to lane 3, i. e. no shift

 UI *ndx0=ndxa;  // origin of ordered column numbers, number of columns
// obsolete  D *bkboundv;  // addr of bound-var info
// obsolete  I *bvgrdesect[2];  // lengths of processing sections: bound then normal

 D *mv0=DAV(qk);   // start of Qk
// obsolete , *mv0lo=mv0+n*n;  // pointer to start of Qk and the the lower half if any
 if(unlikely(nc==0))R 0;  // abort with no action if no columns

 // set up loop invariants based on operation type
 I colressize;  // for one-col, the number of rows in each reservation
// obsolete  I collen;  // for one-col, the length of the column; for nonimp, the length of bkg.  The number of values in a single-column operation
 UI firstcol=0, lastreservedcol=0;  // we have reserved columns from firstcol to lastreservedcol-1 for us to calculate.  firstcol is the running column index
 I currcolproxy=0;   // The column pointer, approximately.  Updated only for gradient
 I resredwarn=nc-10*((I)nthreads+1);  // when col# gets above this, switch to reserving fewer
//   parms is #rows,maxAx,Col0Threshold,Store0Thresh,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,PriRow
 __m256d store0thresh=_mm256_set1_pd(parms[3]);  // In one-column mode, this holds the Store0Threshold: column values less than this are set to 0 when written out
 __m256d col0thresh;  //  minimum value considered valid for a column value (smaller are considered 0)
// obsolete  __m256d colbk0thresh;  //  minimum value that will block a pivot when bk=0
// obsolete  D swapbounty;  //  factor to increase gain by for Nonbasic swaps
// obsolete  D coldangerpivotthresh;  //  smallest allowed pivot, but is dangerous
 D colokpivotthresh;  // smallest pivot value considered non-dangerous. (onecol+SPR)
 D abortspr;  // if the SPR does not exceed this, abort the column  (onecol+SPR)
 __m256d bk0thresh;  // smallest bk value considered nonzero
 I prirow;  //  priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so.  Set to LOW_VALUE if no priority row
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
 __m256i rowgather; // offset for each atom of a gather, spaced one row apart
 B sharingmin;  // set (in gradient mode) in the normal case where we are cutting off rows based on sharedmin from other threads
 D sharedminslack;  // (gradient mode) value to multiply sharedmin by to get a value to beat.  1.0=no change, smaller values cut off columns earlier
// obsolete  __m256i rownums;  // the offset in atoms to the next rows to fetch
// obsolete  __m256i rownums0;  // gradient: rownums at the top of a column
// obsolete  D maxdangerc;  // DIP: max col value of dangerous pivot found here
// obsolete  I4 bestcol=-1; UI4 bestcolrow=0-1;  // best column found so far (bit 31 set if dangerous or none found), and the best row in that column (bit 31 set if forcefeasible required)
 // gradient: zv=0 (flags set) bv=0
 // onecol+SPR: zv->output bv->bk values
// obsolete    //  Bit 3 means 'nonzero column value ignored, forcefeasible required' bit 4 is a temp flag meaning 'this column was an improvement'
// obsolete  // nonimp: zv=4 (start in dp)
// obsolete  // onecol: zv->output, bv=->bk values
// obsolete #define ZVMODX 3  // set if the current column was an improvement
// obsolete #define ZVMOD (1LL<<ZVMODX)  // set if the current column was an improvement
#define ZVNEGATECOLX 0  // set if the result of the col-value calculation must be negated (for Enforcing vars)
#define ZVNEGATECOL (1LL<<ZVNEGATECOLX)  // set if the current column was an improvement
#define ZVFFREQDX 1  // set if we have encountered a column value that may cause us to need forcefeasible (in onecol)
#define ZVFFREQD (1LL<<ZVFFREQDX)
#define ZVPOSCVFOUNDX 1  // set if we have found a positive column value, indicating column is bounded (in gradient)
#define ZVPOSCVFOUND (1LL<<ZVPOSCVFOUNDX)  // set if we have found a positive column value, indicating column is bounded (in gradient)
// obsolete #define ZVDP 0b100  // set if we should start in double precision (set for gradient)
// obsolete #define ZVUNBOUNDX 5  // set if the rows we are processing are bound rows
// obsolete #define ZVUNBOUND (1LL<<ZVUNBOUNDX)
// obsolete #define ZVISDIPX 6  // set if DIP mode (ZVISDIPGRAD also set)
// obsolete #define ZVISDIP (1LL<<ZVISDIPX)  // set if the current column was an improvement
// obsolete #define ZVSPRNOTFOUND 0b01  // set if we have not yet found a valid SPR for the column
// obsolete // low 2 bits are 00 for non-DIP/gradient mode: 000=one-column, 100=nonimp
// obsolete #define ZVISDIPGRAD (ZVPOSCVFOUND|ZVSPRNOTFOUND)
#define ZVISONECOL (((I)zv&~(ZVPOSCVFOUND|ZVNEGATECOL))!=0)  // zv!=0, except for flag bits
#define remflgs(zv) ((D*)((I)(zv)&~(ZVPOSCVFOUND|ZVNEGATECOL)))

// obsolete  rowstride=_mm256_set1_epi64x(n);   // length of 1 row, which is right for DIP/nonimp
 rowgather=_mm256_mul_epu32(_mm256_set1_epi64x(qkrowstride),_mm256_loadu_si256((__m256i*)(&iotavec[0-IOTAVECBEGIN])));  // init atomic offset to successive rows 0 1 2 3
// obsolete  rowstride=_mm256_slli_epi64(rowstride,LGNPAR);   // length of NPAR rows for onecol
 prirow=(I)parms[10];   // copy in parms
 I nbasiscols=nbasiswfk;  // number of basis columns of qk not including any Fk; this may be flagged with Fk info
 *(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // in case some other thread has finished a column and given us a bogey, go get it
 col0thresh=_mm256_set1_pd(parms[2]);  // column |values| less than this are considered 0
 if(zv==0){  // gradient initialization.
  sharingmin=1;  // normally, we are cutting off using shared minimum
  if(parms[4]<0){  // If MinImp given...
   sharedminslack=1.0+parms[4]; sharedminslack*=sharedminslack;  // create derater for recip colsq
  }else{
   sharedminslack=1.0;  // normal case, do not derate the gradient
   if(parms[4]>0){  // MinGradient given...
    sharedmin=1.0/parms[4]; sharedmin*=sharedmin;  // remember 1/(min gradient to compute)^2, which is what we share
    sharingmin=0;  // suppress updating that value
   }
  }
  bk0thresh=_mm256_set1_pd(sqrt(2.0));   // bk0thresh is sqrt(2) for gradient
  // nbasiscols needs no change
// obsolete   zv=(D*)ZVDP;  // set flag 101 indicating DP, no positive CV found, not negating column
// obsolete   maxdangerc=0.0;  // indicate no dangerous pivot found yet
// obsolete   bkboundv=DAV(bkbound);  // addr of bound-var info
// obsolete   if((I)zv&ZVISDIP)swapbounty=parms[7];  // swapbounty only for DIP
// obsolete   if(AN(parms)==7){  // gradient mode: set up for sequential processing
// obsolete    rownums0=rownums=_mm256_mul_epu32(rowstride,_mm256_loadu_si256((__m256i*)(&iotavec[0-IOTAVECBEGIN])));  // init atomic offset to successive rows 0 1 2 3
// obsolete    rowstride=_mm256_slli_epi64(rowstride,LGNPAR);   // length of NPAR rows for gradient
// obsolete    zv=(D*)(ZVDP+ZVSPRNOTFOUND);  // indicate dp, gradient (because DIP not set)
// obsolete    bvgrde-=(bvgrde-bvgrd0)&(NPAR-1)?NPAR:0;  // back bvgrde to the point of the incomplete remnant, if there is one
// obsolete   }
// obsolete   // bvgrde is set in the caller to process the requested rows; back it up here so that it triggers for the remnant if any
// obsolete   // calculate bvgrde for each section, based on length, and back each up to the remnant
// obsolete   bvgrdesect[0]=bvgrd0+AN(bkbound)-(AN(bkbound)&(NPAR-1)?NPAR:0);  // first section is the bound variables, backed up to remnant
// obsolete   bvgrdesect[1]=bvgrde-(((bvgrde-(bvgrd0+AN(bkbound)))&(NPAR-1))?NPAR:0);  // second section is the rest; the blocks are aligned to the end of the first section
 }else{  // one-column: process a single column, including SPR
  I nhasfk=REPSGN(nbasiscols); nbasiscols=nbasiscols^nhasfk;  // -(Fk is an extra row); # valid columns of Qk
  frowbatchx=(nrows-1)&-NPAR; frowbatchx|=~nhasfk;  // batch# containing last row (might be by itself); if it's not fk, batch is ~0, i. e. never
// obsolete   nrows=nbasiscols-nhasfk;  // #rows to calculate, including fk if present
// obsolete   bv=(D*)n;  // offset to low part of *zv - saves a register
  bk0thresh=_mm256_set1_pd(parms[7]);
  colressize=8*NPAR*((I)nthreads+1);   // enough rows to allow each thread to arbitrate if they have all-zero jobs
  colressize=(colressize+(BNDROWBATCH-1))&-BNDROWBATCH;  // the bound/enforcing mask is built in units of BNDROWBATCH bits so as to fit in an AVX register, matching the row numbers.  There are 2 bits per value.  They must stay aligned.
  colokpivotthresh=parms[6]; abortspr=parms[9];
// obsolete  coldangerpivotthresh=parms[5];
// obsolete  colbk0thresh=_mm256_set1_pd(parms[4]);
// obsolete   if(likely(zv!=0)){  // one-column
  // Set up for multithreading one-column mode, where we have to split the column.  Multithreading of gradient is handled
  // by having each thread take columns in turn
// obsolete   // Since there is no bv or bvgrd, we save a register by repurposing bv to have the offset to the low part of result, if qp
// obsolete   // bvgrd continues to be the loop counter, but the region starts at 0
// obsolete   // in quad-precision we take reservations of a few lines each time.  Everybody starts at 0
// obsolete   collen=n;  // length of the column - we do it all
// obsolete   // we leave the low bits of zv=000 for onecol mode
// obsolete   }else{
// obsolete    // nonimp mode
// obsolete    collen=bvgrde-bvgrd0;  // the length of the column (length of bkgrade)
// obsolete    zv=(D*)ZVDP;  // zv flags 100: nonimp mode
// obsolete    colbk0thresh=_mm256_set1_pd(parms[4]);  // ColOkPivot, put into an often-used register
// obsolete   }
// obsolete   bvgrde=bvgrd0;  // each reservation is taken in the loop below; start it out empty
  lastreservedcol=1;  // cause each thread to process the same one column, without arbitrating for it.  Could branch into loop after reservation
 }

 D nzerobatches;  // number of batches with 0 SPR we have encountered
 // loop over all columns requested.  Low 3 bits of zv indicate mode: 000=onecol 100=nonimp other=DIP
 while(1){  // loop for each column
  // start of processing one column
// obsolete  __m256d minspr;  // minimum valid SPR found so far, in each qword-lane (for gradients, max column value seen)
  __m256d sharednormorspr;  // (gradient) the total sumsq of column values so far; (onecol+SPR): the minimum across lanes shared, to reduce minimum sightings
// obsolete  I firstsprrow=0;  // first row in which a valid SPR was found (may be lower than the actual value)   scaf init needed for warning
// obsolete   __m256d totalnorm;  // for gradients, accumulator of sumsq of column values
  I limitrowx;  // for SPR, the row of the current lowest SPR
// obsolete   __m256i limitrows=_mm256_set1_epi64x(1000000000);  // row number fetched from, valid only when there is a valid SPR.  Must be initialized so as not to match prirow.  Same for gradients
  __m256d bndmsk;  // bitstream for each lane, giving (bound) info for the row.  bigendian!!
  D accumsumsq;  // accumulator of column norm, updated periodically for precision reasons.  Only for gradient

  // establish column reservation, for gradient mode
  // get next col index to work on; stop if no more cols
  // We would like to take sequential columns, because they are likely to share columns of Qk.  But there is wild variation in the time spent on a column: the
  // average column cuts off within 50 rows, while a winning column goes to the end, perhaps thousands of rows.  As a compromise we take groups of columns until
  // the number of columns left is less than 100*number of threads.  Then we switch to one at a time.  We use a slightly out-of-date value of the current column
  if(firstcol>=lastreservedcol){
   // we have to refresh our reservation.  We never go through here for one-column; nextresv is the column reservation then
   if(firstcol>=nc)break;  // exit if all columns have been processed.  This is the exit from the column loop
   UI ressize=currcolproxy>resredwarn?4:32;  // take big batches till we get near the end, for 2 reasons. (1) we want contiguous allocation within column family for sharing cached data
               // (2) with early cutoff we might process a column very quickly and me need more to even the load
   firstcol=__atomic_fetch_add(&ctx->nextresv,ressize,__ATOMIC_ACQ_REL);  // get next sequential column number, reserve a section starting from there
   if(firstcol>=nc)break;  // if no columns to reserve, exit loop
   lastreservedcol=firstcol+ressize; lastreservedcol=lastreservedcol>nc?nc:lastreservedcol;  // remember end of our reservation, never longer than the data
  }

  I colx=ndx0[firstcol];  // get next column# to work on

// obsolete   // loop until we don't need to rerun at higher precision
// obsolete   {
// obsolete   // this is a loop: we branch back to retry
  I rowx=0;  // the row number we are working on.  We stop when rowx=rown.  Always on an NPAR-row boundary
  I rown;  // index of last+1 row in current (or only) section of the column.  If there is a remnant, the value is backed up by 4.  If 0, we are processing the remnant
  // col init
  __m256d endmask=_mm256_setone_pd(); // mask for validity of next 4 words: used to control fetch from column and store into result row
  D cutoffnorm;  // if the column norm gets above this we can cut the column off, since its (-) gradient will be too high
// obsolete   limitrowx=nrows;  // init to out-of-bounds row so that if we don't improve we do a swap
  // get column info, a 2-bit field of (bound,enforcing)
  I colrvt=(rvtv[colx>>(LGBB-1)]>>((colx&((1LL<<(LGBB-1))-1))<<1))&((1LL<<2)-1);
  zv=(D*)(((I)zv&~(ZVPOSCVFOUND+ZVNEGATECOL))|((colrvt&1)<<ZVNEGATECOLX));  // If this column is Enforcing, remember so we change its sign
  if(likely(!ZVISONECOL)){    // gradient mode
   // gradient mode: sharednormorspr is accumulator for sumsq
   // see if we have a partial (or even complete) total from a previous run.  If so, start there
   if(cutoffstatus[colx][0]){  // row# of 0 means no data
    rowx=cutoffstatus[colx][0]; accumsumsq=cutoffstatus[colx][1];  // resume point, and total at that point
    // negative row value means that the rows processed so far included a positive col value, i. e. not unbounded
    zv=(D*)((I)zv|(SGNTO0(rowx)<<ZVPOSCVFOUNDX)); rowx=ABS(rowx);  // extract boundedness info, move to flag
    rowx=rowx>=nrows?nrows:rowx;  // prevent rows from incrementing too many times
    ndotprods-=rowx;  // treat (0,start] as a gap
    if(rowx>=nrows)goto usefullrowtotal;  // if the saved info covered the whole col, just use it.
if((rowx&(BNDROWBATCH/1-1))!=0)SEGFAULT;  // scaf  if just part of a col, it must leave us on a bndrowmask bdy
   }else{
    // Starting row from scratch.  init the norm total: add 1.0 normally, but in bound column 2.0 to account for the hidden row
    accumsumsq=!(colrvt&0b10)?1.0:2.0;
   }
   sharednormorspr=_mm256_setzero_pd();  // init col total to 0
// obsolete    zv=(D*)ZVDP;  // restore gradient mode, no flags
// obsolete    I isboundcol=(rvtv[colx]&0b110)==0;  // rvt 0 & 1 are Bound variables
// obsolete    minsprshared=_mm256_setzero_pd();   // biggest value seen yet - init to 0
// obsolete    if(){   // not bound column
// obsolete     sharednormorspr=_mm256_set1_pd(1.0/NPAR);  //  total column norm of 1
// obsolete    }else{
// obsolete     // Bound column.  We must init to the implied row, which has a column value of 1 which goes into the total and is a valid initial magnitude
// obsolete     sharednormorspr=_mm256_set1_pd(2.0/NPAR);  // column norm of 2, 1 for the implied row
// obsolete    }
// obsolete     colbk0thresh=_mm256_setzero_pd();  // gradient: the high-precision gradient sum (init to 1+, 2+ if bound col)  also the largest c value
// obsolete       // minsprshared is set to 0 because there MUST be a positive column value, or the problem would be unbounded; and we use negative pivot to indicate Swap needed
   rown=nrows-(nrows&(NPAR-1)?NPAR:0);  // process full columns; back off if there is a remnant
   if(unlikely(rowx>rown)){endmask=_mm256_loadu_pd((double*)(validitymask+(rowx-rown))); rown=0;}  // if we START on a remnant, set up for it
   // we launched a read of sharedmin as long ago as we could, and we have waited as long as we can for it to complete.
   // We don't bother aborting on unbounded: it's a fatal error and we will catch it in the end
   cutoffnorm=sharedmin*sharedminslack*Frow[colx]*Frow[colx];  // Frow^2 * best sumsq / best Frow^2, which is cutoff point for sumsq in new column (Frow^2)/sumsq > bestFrow^2/bestsumsq)
   if(unlikely(accumsumsq>cutoffnorm))goto abortcol;   // It is conceivable that the initial position cuts off, even if no values have been calculated.  rowx and accumsumsq must be set
// obsolete     // we must back up the column pointer each time to top-of-column
// obsolete     rownums=rownums0;
// obsolete     bvgrde=bvgrdesect[0];  // first section is bound vbls
  }else{  // onecol+DIP
// obsolete     // This is the retry point; put static inits above this
// obsolete     if(1)zv=(D*)(ZVDP+ZVSPRNOTFOUND+ZVISDIP);else{retryinquad: zv=(D*)(ZVSPRNOTFOUND+ZVISDIP);}  // if we need to retry in qp, so indicate.  Start with UNBOUND off, no FFREQD
// obsolete     D cutoffspr=unlikely(Frow[colx]==0)?0.:minimp/Frow[colx];  //minimum SPR that will not cut off
// obsolete     minimpspr=_mm256_set1_pd(cutoffspr);  // load into all lanes
// obsolete     bvgrde=bvgrdesect[0];  // first section is bound vbls
// obsolete    I isboundcol=(rvtv[colx]&0b110)==0;  // rvt 0 & 1 are Bound variables
// obsolete    if(!isboundcol){   // not bound column
// obsolete     sharednormorspr=_mm256_set1_pd(inf);  // DIP: minimum valid SPR found so far, in each qword-lane
// obsolete    }else{
// obsolete     // Bound column.  we must initialize the SPR to beta and set the row# to out of bounds.  This initialization is executed only once
// obsolete // obsolete      if(unlikely(prirow>=0&&cutoffspr>swapbounty*betav[colx]))goto abortcol;  // if the swap would cut off, skip the column - any other pivot would have even lower SPR.  If there is a prirow, wait till we have looked at it
// obsolete // obsolete                      // we must apply the swap bounty to the gain, since if this survives it will be a Nonbasic Bound Swap
// obsolete     sharednormorspr=_mm256_set1_pd(bkbeta[colx]);  // SPR: init nonbasic column to SPR of beta.  limitrowx has been initialized for this
// obsolete // obsolete     limitcs=_mm256_set1_pd(1.0);  // set c value in the implied row
// obsolete // obsolete      zv=(D*)((I)zv^(ZVSPRNOTFOUND|ZVPOSCVFOUND));  // this implied pivot is a valid one
// obsolete     zv=(D*)((I)zv|((rvtv[colx]&1)<<ZVNEGATECOLX));  // If this column is Enforcing, remember so we change its sign
// obsolete    }

// obsolete    minsprshared=minspr;  // we keep this min-across-lanes version to reduce the number of divides
// obsolete     // one-column mode.  We don't have zv as a flag so we repurpose minsprshared to indicate that the column must be negated
// obsolete     if(likely((rvtv[colx]&0b001)==0))minsprshared=_mm256_setzero_pd();else minsprshared=sgnbit;   // no sign-change if not Enforcing column
   rown=0;  // we get a reservation for each group of rows; so indicate the current allocation is empty
   nzerobatches=0.0;  // init we have not encountered any 0 SPRs
   limitrowx=IMAX;  // init row found to high-value, so that we don't share a value unless we actually hit it
   // The only sharing is at startup, but we might as well take advantage of what other threads found.  We have to eliminate the flag values -1 and IMIN
   if(unlikely(*(I*)&sharedmin<0)){    // share values < 0 are special; either -1 (nonimproving nondangerous) or IMIN (abort)
    if(unlikely(*(I*)&sharedmin==IMIN))goto earlycol;  // if improvement has been pegged at limit, it's a signal to stop
    sharedmin=0.0;   // sharedmin must never go negative - we rely on not hitting -0 in the calculation
   }
   // The init SPR is high-value for normal columns; but for bound column must init to beta, the SPR of the implied row
   // This was set in the initial value in the ctx
// obsolete    sharednormorspr=_mm256_set1_pd(!(colrvt&0b10)?inf:bkbeta[colx]);
   sharednormorspr=_mm256_set1_pd(sharedmin);
  }
// obsolete    __m256d impliedsign=likely(rvtv[colx]!=8)?_mm256_setzero_pd():sgnbit;   // remember if we are fetching an enforcing bound column.  We must use _1 for the implied value for slacks then

  // Now that we know we aren't skipping the column, read the information about the cols and weights of the dot-product.  This can be improved:
  // 1. we should move the weights to a local area so they are used from the same cache locations
  // 2. we should use a ping/pong buffer to pipeline the info, giving a full column time for the transfer
  // 3. we should compress the info and transfer it in full cacheline fetches
  I an; D *vv; I *iv;  // number of sparse atoms in each row (0 if Ek column), pointers to row#s, values for this column
  if(colx>=nbasiscols){  // not a basis column (=slack)
   // scaf we could compress these and expand them here
   an=axv[colx][1];  // number of sparse atoms in each row
   vv=avv0+axv[colx][0];  // pointer to values for this section of A
   iv=amv0+axv[colx][0];  // pointer to row numbers of the values in *vv (these are the columns we fetch in turn from Ek)
  }else an=-1;  // an<0 means basis column

// obsolete    I *bvgrd;
  // create the column NPAR values at a time
  // loop for each row, in groups of NPAR
  while(1){
// obsolete    __m256i indexes;  // offset in atoms from Qk to the beginning of the row we are fetching = rownums*n

   // Establish what batch of NPAR lines we should work on.  This block handles end-of-column/end-of-section
// obsolete    if(likely(bvgrd<bvgrde)){  // normal case not close to end of column or column section; there are a full 4 values to process
   if(unlikely(rowx>=rown)){   // if there are not a full 4 values to process
// obsolete     if(likely((I)zv&ZVISDIPGRAD)){   // DIP/gradient
// obsolete      rownums=_mm256_loadu_si256((__m256i*)bvgrd);  // fetch the next set of row#s
// obsolete      indexes=_mm256_mul_epu32(rowstride,rownums);  // convert to atom #
// obsolete     }else{  // onecol, which has no bkgrade
// obsolete      indexes=rownums; rownums=_mm256_add_epi64(rownums,rowstride);  // sequential processing of entire column; advance rownums by 4 rows
// obsolete     }
// obsolete    }else{  // we have hit the end or the incomplete remnant
    // not 4 values.  If there is no remnant, we are through with the reservation for this column
// obsolete     if(bvgrd!=bvgrde){  // we did not do an even multiple of 4 (in which case there is no remnant)
// obsolete      I mnvalid=(I)(bvgrde-bvgrd);  // this is ((end-4)-curr)=(end-curr)-4=#values left-4, which is _1-_3 first time, _5-_7 second time
    rown=rown==0?rowx:rown;  // rown=0 means we are processing the remnant; signal that it is finished
    if(rowx==rown){  // no unprocessed remnant?
     // here we are done with the column/section.  Exit loop if finished
     if(likely(!ZVISONECOL))break;    // gradient mode - we process whole columns
// obsolete     if((I)zv&ZVISDIPGRAD){
// obsolete      // DIP/gradient: if we were doing the bound variables, we now have to switch to unbound
// obsolete      if((I)zv&ZVUNBOUND)break;   // finishing unbound, column is done
// obsolete      // bvgrd now points past the end of the bound variables; set bvgrde to the end of the unbound
// obsolete      bvgrde=bvgrdesect[1];  // finish up the unbound
// obsolete      zv=(D*)((I)zv|ZVUNBOUND);  // indicate we are working on it
// obsolete      endmask=_mm256_setone_pd();  // init all lanes valid
// obsolete      continue;  // go back to refetch with the new bvgrd/bvgrde
// obsolete     }  // end of column section is end of column except for one-column, where we take multiple reservations per column
     // one-column mode: establish row reservation
     // quad-prec one-column mode.  Take a set of rows to process.  The issue is that some rows are faster than others: rows with all 0s skip extended-precision
     // processing.  It might take 5 times as long to process one row as another.  To keep the tasks of equal size, we take a limited number of rows at a time.
     // Taking the reservation is an RFO cycle, which takes just about as long as checking a set of all-zero rows.  This suggests that the reservation should be
     // for NPAR*nthreads at least.  And, the bound/enforcing mask is constructed to match the AVX lanes: one quantum of mask has (NPAR*BW/2) bitpairs; reservation must be a multiple of that
// obsolete     I currx=bvgrde-bvgrd0;  // index we would be processing next.  If not at end, this is known not to require adjustment.
// obsolete      I ninclfk=nrows+(frowbatchx<(UI4)~0);  // number of rows to create, including Fk if present
     if(rowx>=nrows)break;  // if we know we are the last reservation, no need to look at the shared value.
     // We are going to take a new reservation.  Deduct the size of the gap from the # products to date
     ndotprods+=rowx;  // we will end up removing (new rowx-old rowx), i. e. the amount skipped
     rowx=__atomic_fetch_add(&ctx->nextresv,colressize,__ATOMIC_ACQ_REL);   // reserve the next block
     ndotprods-=rowx;  // finish removing gap
// obsolete     if(resrow>=collen)break;  // finished when reservation is off the end
     if(rowx>=nrows)break;  // finished when reservation is off the end
// obsolete     I skipamt=resrow-currx;  // number of rows to skip
// obsolete      I skipamt=resrow-rowx;  // number of rows to skip between current position and new reservation
// obsolete      zv+=skipamt; mv+=skipamt*n;  // advance the store pointer and col base pointer.  Since the high and low planes may have different alignment we don't try to avoid false cacheline sharing, which will occur on the fringes
// obsolete     I endx=resrow+colressize; endx=endx>collen?collen:endx;  // end+1 of the reservation
// obsolete     bvgrd=bvgrd0+resrow; bvgrde=bvgrd0+endx;  // loop controls
// obsolete     bvgrde-=endx&(NPAR-1)?NPAR:0;  // back bvgrde to the point of the incomplete remnant, if there is one (possible only at end)
     rown=rowx+colressize; rown=rown>nrows?nrows:rown; rown-=rown&(NPAR-1)?NPAR:0;  // clamp to end of column, and back up end if there is a remnant
// obsolete     if(unlikely(NPAR-(endx-resrow)>0))endmask=_mm256_loadu_pd((double*)(validitymask+NPAR-(endx-resrow)));  // if we start on the remnant, fetch its mask
// obsolete      if(unlikely(rown<rowx)){endmask=_mm256_loadu_pd((double*)(validitymask+(rowx-rown))); rown=rowx+NPAR;} // if we START on the remnant, fetch its mask and ensure we don't process it again
// obsolete      if(bvgrd0!=ONECOLGRD0){   // nonimp, which has bvgrd
// obsolete       if(unlikely(NPAR-(endx-resrow)>0))rownums=_mm256_maskload_epi64(bvgrd,_mm256_castpd_si256(endmask)); else rownums=_mm256_loadu_si256((__m256i*)bvgrd);  // fetch from updated bvgrd
// obsolete       indexes=_mm256_mul_epu32(rowstride,rownums);  // convert to atom #
// obsolete      }else{  // onecol
// obsolete     rownums=_mm256_add_epi64(rownums,_mm256_set1_epi64x(skipamt*(I)bv));  // advance the atom-offsets over the skipped area (bv=n)
// obsolete     indexes=rownums; rownums=_mm256_add_epi64(rownums,rowstride);  // sequential processing of entire column; advance rownums
// obsolete      }
    }
    if(rowx>rown){  // if there is a remnant (which could be the first batch of a new reservation), process it and make sure we finish next time
// obsolete      I mnvalid=rown-rowx;  // this is ((end-4)-curr)=(end-curr)-4=#values left-4, which is _1-_3
// obsolete      if(mnvalid+NPAR>0){  // first time...
     // there is a remnant to process.  Get the endmask for it
     endmask=_mm256_loadu_pd((double*)(validitymask+(rowx-rown)));   // (curr-(end-4)) = 4-(end-curr) = 4-#values left is the correct mask, with (#values) ~0
// obsolete       if(likely((I)zv&ZVISDIPGRAD)){   // DIP/nonimp
// obsolete        rownums=_mm256_maskload_epi64(bvgrd,_mm256_castpd_si256(endmask));  // fetch the remnant of row#s
// obsolete        indexes=_mm256_mul_epu32(rowstride,rownums);  // convert to atom #
// obsolete       }else{  // onecol/gradient
// obsolete        indexes=rownums; rownums=_mm256_add_epi64(rownums,rowstride);  // sequential processing of entire column; advance rownums by NPAR rows
// obsolete       }
     rown=0;  // set flag value to indicate we are processing a remnant and endmask is set
// obsolete       goto notendcolumn;
// obsolete      }
// obsolete      bvgrd=bvgrde+NPAR;  // in case we need to pick up with unbound vars, advance bvgrd to the correct end+1 of section (if bvgrd=bvgrde it's already there)
// obsolete      rownums=_mm256_add_epi64(rownums,_mm256_mul_epi32(_mm256_srli_epi64(rowstride,LGNPAR),_mm256_set1_epi64x(mnvalid+NPAR))); // in case gradient mode, back up rownums by the # unprocessed values in last batch
// obsolete      // fall through second time we encounter an incomplete remnant
    }
   }
// obsolete notendcolumn: ;

// obsolete    // rownums, indexes, bvgrd are all set up
   __m256d dotproducth,dotproductl;  // where we build the column value

   D *mv=mv0+rowx*qkrowstride;  // point to the base of the block we are loading from

   // if this starts a bound/enforcing block, fetch the mask for it
   if((rowx&(BNDROWBATCH/1-1))==0){
    // move the 64 bits to  the correct position in each lane
    bndmsk=_mm256_castsi256_pd(_mm256_sllv_epi64(_mm256_set1_epi64x(*(I*)&bndrowmask[rowx>>(LGBNDROWBATCH-0)]),bndmskshift));
   }

   // get the next NPAR values by dot-product with Av
   if(an>=0){
    // fetching from A.  Form (Ek row) . (A column) for each of the 4 rows.  There must be at least 1 column selected
    if(!ZVISONECOL){
     // DIP/gradient, starting in dp. double-precision accumulate
     dotproducth=_mm256_setzero_pd();
     I k;
// obsolete   __m256d nonzerofound=_mm256_setzero_pd();  // will remain 0 if all values read in the lane are 0
     NOUNROLL for(k=0;k<an;++k){
      dotproductl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv+iv[k],rowgather,endmask,SZI);  // fetch from up to 4 rows
      dotproducth=_mm256_fmadd_pd(dotproductl,_mm256_set1_pd(vv[k]),dotproducth);  // accumulate the dot-product
// obsolete       nonzerofound=_mm256_or_pd(nonzerofound,_mm256_cmp_pd(dotproductl,nonzerofound,_CMP_NEQ_OQ));  // if nonzero found, remember
     }
// obsolete      // if any of the results is below the qp threshold, revert to qp; but ignore any exact 0
// obsolete      if(!_mm256_testz_pd(_mm256_cmp_pd(dotproducth,qpthresh,_CMP_LT_OQ),nonzerofound))goto revertqp;  // testz is 1 if all comparisons fail, i. e. no product is too low
    }else{
// obsolete revertqp: ;
     // extended-precision accumulate, used only when Qk has quad-precision and we need the precision
     // It's not unusual for all 4 values fetched from Qk to be 0. 
     // For the first value, we can simplify the product with no accumulate
     I k; D *mvlo=mv+qkstride;   // offset to low half of Qk
     NOUNROLL for(k=0;k<an;++k){  // for each element of the dot-product
      __m256d tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
      // get column number to fetch; fetch 4 rows
      dotproducth=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv+iv[k],rowgather,endmask,SZI);  // fetch from up to 4 rows
      if(_mm256_testz_si256(_mm256_castpd_si256(dotproducth),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
      // there's a nonzero, calculate it
      tl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mvlo+iv[k],rowgather,endmask,SZI);  // fetch from up to 4 rows
      vval=_mm256_set1_pd(vv[k]);  // load column value
      // take the two products and canonicalize
      TWOPROD(dotproducth,vval,th2,tl2)  // high qk * col
      tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add to high part
      TWOSUMBS(th2,tl2,dotproducth,dotproductl)  // combine parts, canonicalize
       // this is accurate to 104 bits from the largest of (new,old,new+old)
      goto accumulateqp; 
     }
     // if we end the loop, there must have been nothing but 0.  We know dotproducth was set to 0.
     dotproductl=dotproducth;  // set overall product to 0
     goto endqp;
accumulateqp: ;
     NOUNROLL for(++k;k<an;++k){  // skip the first nonzero; for each remaining element of the dot-product
      __m256d th,tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
      // get column number to fetch; fetch 4 rows
      th=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv+iv[k],rowgather,endmask,SZI);  // fetch from up to 4 rows
      if(_mm256_testz_si256(_mm256_castpd_si256(th),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
      tl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mvlo+iv[k],rowgather,endmask,SZI);  // fetch from up to 4 rows
      vval=_mm256_set1_pd(vv[k]);  // load column value
      // accumulate the dot-product
      TWOPROD(th,vval,th2,tl2)  // high qk * col
      tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add in extension of prev product
      TWOSUM(dotproducth,th2,dotproducth,vval)  // combine high parts in quad precision
      tl2=_mm256_add_pd(dotproductl,tl2); tl2=_mm256_add_pd(vval,tl2);  // add extensions, 3 products costs 2 bits of precision
      TWOSUM(dotproducth,tl2,dotproducth,dotproductl)  // combine high & extension for final form
      // this is accurate to 104 bits from the largest of (new,old,new+old).
     }
endqp: ;
    }
   }else{
    // fetching from the Ek matrix itself.  Just fetch the values from the column
    dotproducth=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv+colx,rowgather,endmask,SZI);
    if(ZVISONECOL){dotproductl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv+qkstride+colx,rowgather,endmask,SZI);}  // if quad-prec (onecol)
   }  // end of creating NPAR values

   // process the NPAR generated values
   if(likely(!ZVISONECOL)){  // ********************************************* gradient mode
     // gradient mode.  Gradient is sum of squares of column values.  We find the column with the largest gradient.
     // bk0thresh is sqrt(2)
// obsolete      if(!_mm256_testz_pd(_mm256_cmp_pd(absdph,col0thresh,_CMP_GT_OQ),endmask)){  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  if one is big enough...
     if(!((I)zv&ZVPOSCVFOUND)){     // if any value is positive above threshold, the problem is bounded.  In bound rows, use the absolute value for the boundedness check.  This will predict correctly all but one time.
      if((I)zv&ZVNEGATECOL){dotproducth=_mm256_xor_pd(sgnbit,dotproducth);}  // Handle Enforcing column. branch will predict correctly and will seldom need the XOR
      zv=(D*)((I)zv|(!_mm256_testz_pd(_mm256_cmp_pd(_mm256_andnot_pd(_mm256_and_pd(sgnbit,bndmsk),dotproducth),col0thresh,_CMP_GT_OQ),endmask)<<ZVPOSCVFOUNDX));  //  force positive if bndmsk
     }
     // we don't test for all-0 values, even though fairly common, because the processing here is so short
// obsolete       __m256d colsq, t; colsq=_mm256_fmadd_pd(dotproducth,dotproducth,colbk0thresh);   // colsq is column value^2 (for unbound col)
// obsolete       if((I)zv&ZVNEGATECOL)dotproducth=_mm256_xor_pd(sgnbit,dotproducth);  // Handle Enforcing column. branch will predict correctly and will seldom need the XOR
// obsolete       // fetch the 4 bk values and see which ones are near0
// obsolete       // for Bound rows, select the b/b-beta that corresponds to the sign of c
// obsolete       __m256d bk4;  // b value, to be tested against 0
// obsolete       if(((I)zv&ZVUNBOUND)){  // we are into the unbound rows
// obsolete        bk4=likely(bvgrd<bvgrde)?_mm256_loadu_pd(bv+(bvgrd-bvgrd0)):_mm256_maskload_pd(bv+(bvgrd-bvgrd0),_mm256_castpd_si256(endmask));  // the next bk values.  Need 0s at end
// obsolete       }else{  // still checking bound rows
// obsolete        // if the rows are Bound vars, they have to count twice (with the same column value)
// obsolete        __m256d bkbound4;  // b-beta to use if c<0
// obsolete        if(likely(bvgrd<bvgrde))bk4=_mm256_loadu_pd(bv+(bvgrd-bvgrd0)), bkbound4=_mm256_loadu_pd(bkboundv+(bvgrd-bvgrd0));
// obsolete        else bk4=_mm256_maskload_pd(bv+(bvgrd-bvgrd0),_mm256_castpd_si256(endmask)), bkbound4=_mm256_maskload_pd(bkboundv+(bvgrd-bvgrd0),_mm256_castpd_si256(endmask));  // the next bk values.  Need 0s at end
// obsolete        colsq=_mm256_fmadd_pd(dotproducth,dotproducth,colsq);   // for bound col, 
// obsolete        bk4=_mm256_blendv_pd(bk4,bkbound4,dotproducth);  // bk if c positive, b-beta if negative
// obsolete        dotproducth=absdph;  // now dotproducth is the c value in the direction we can go, and bk4 is the b value for that var
// obsolete       }
// obsolete       // add new value^2 (possibly doubled) to gradient total, using Kahan summation (in limitcs/colbk0thresh)
// obsolete       t=_mm256_add_pd(limitcs,colsq); colbk0thresh=_mm256_sub_pd(colsq,_mm256_sub_pd(t,limitcs)); limitcs=t;  // accumulate col^2; 16 cycles latency, which will limit perf (slightly)
// obsolete       // find the largest column value occurring on a row where bk is near0
// obsolete       dotproducth=_mm256_and_pd(dotproducth,_mm256_cmp_pd(bk4,bk0thresh,_CMP_LT_OQ));   // clear any column values for which bk is not near0
     // if row is bound, multiply its value by sqrt(2).  Then accumulate value^2 into sharednormorspr
     dotproducth=_mm256_blendv_pd(dotproducth,_mm256_mul_pd(dotproducth,bk0thresh),bndmsk); sharednormorspr=_mm256_fmadd_pd(dotproducth,dotproducth,sharednormorspr);  // accumulate norm
// obsolete       // remember column position of largest value in each lane
// obsolete       limitrows=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(limitrows),_mm256_castsi256_pd(_mm256_set1_epi64x((I)bvgrd)),_mm256_cmp_pd(dotproducth,minspr,_CMP_GT_OQ)));
// obsolete       limitrows=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(limitrows),_mm256_castsi256_pd(rownums),_mm256_cmp_pd(dotproducth,minsprshared,_CMP_GT_OQ)));
// obsolete       // find largest column value to date (in minsprshared)
// obsolete       minsprshared=_mm256_max_pd(minsprshared,dotproducth);  // find the winner
// obsolete       __m256i loadingprirow=_mm256_cmpeq_epi64(_mm256_set1_epi64x(prirow),rownums);  // mask of lanes loading prirow
// obsolete       if(unlikely(!_mm256_testz_si256(rowstride,loadingprirow))){   // rowstride is any 256i with nonzero in all lanes
// obsolete        // we are looking at the priority row.  If it is eligible and not dangerous, stop right away and take it
// obsolete        if(!_mm256_testz_pd(_mm256_castsi256_pd(loadingprirow),_mm256_cmp_pd(dotproducth,_mm256_set1_pd(colokpivotthresh),_CMP_GE_OQ))){
// obsolete       if(unlikely((bvgrd-bvgrd0)==(prirow&-NPAR))){
// obsolete        // We are looking at the virtual row.
// obsolete        if(_mm256_movemask_pd(_mm256_cmp_pd(dotproducth,_mm256_set1_pd(colokpivotthresh),_CMP_GE_OQ))&(1<<(prirow&(NPAR-1)))){
// obsolete         // We can pivot out the virtual row.  We take that whenever we can, preemptively
// obsolete         ndotprods+=bvgrd-bvgrd0+1; bestcol=colx; bestcolrow=prirow; goto return4;
// obsolete        }
// obsolete       }
     // if gradient total cuts off, abort the column.  We have to collect the total across all lanes
     // this test is long enough that we don't want to do it on every loop.  The best frequency is
     // sqrt(2 * (expected # loops) * (cost of test) / (cost of loop body))
     // mitigating factors are (1) computation is not lost if the column isn't modified by the pivot
     // (2) if the frequency is less than every 16 cycles the branch will mispredict
     // Currently the body is ~40 cycles, the test 6.  If n/2=500 we can test every 16 cycles.
     // If we transpose Qk and go to half-precision, this will change radically
     if((rowx&(CMPBATCH-1))==((CMPBATCH-1)&-NPAR)){  // at the END of a compare batch, so that if we cut off we will be on a bndrow mask bdy
      dotproducth=_mm256_add_pd(_mm256_permute_pd(sharednormorspr,0b0101),sharednormorspr); dotproducth=_mm256_add_pd(_mm256_permute4x64_pd(dotproducth,0b01001110),dotproducth);   // combine into one
      accumsumsq+=_mm256_cvtsd_f64(dotproducth); sharednormorspr=_mm256_setzero_pd();  // transfer total to accumsumsq
      if(unlikely(accumsumsq>cutoffnorm)){  // cutoff if Frow^2/sumsq<(best Frow^2/sumsq) => sumsq>(best sumsq)*(Frow/best Frow)^2  we save min of (best sumsq)/(best Frow^2) which is max of FOM
// obsolete      if(unlikely(!_mm256_testz_pd(_mm256_cmp_pd(dotproducth,cutoffnorm,_CMP_GT_OQ),endmask))){
// obsolete               // scaf this does not have to use testz - it is just comparing any lane but there is no way to refer to that
// obsolete        // The column can be cut off.  But if there is a priority row, make sure we check it.  The easy way to do this is to cut off only after we have started
// obsolete        // processing the unbound vars, since the prirow is always the first of those
// obsolete        I skipamt=((prirow&-NPAR)-NPAR)-(bvgrd-bvgrd0);  // number of rows to hop over to get to one row before prirow
// obsolete        if(skipamt<0)goto abortcol;  // If we have already checked the virtual row, we can abort
// obsolete        bvgrd+=skipamt;  // if we haven't checked the virtual row, skip to just before it.  If it isn't eligible we will abort next time
// obsolete        rownums=_mm256_add_epi64(rownums,_mm256_mul_epu32(rowstride,_mm256_set1_epi64x(skipamt>>LGNPAR)));  // advance rownums in groups of 4 rows to match the #rows we skipped
// obsolete        if((I)zv&ZVUNBOUND)
       rowx+=NPAR;  // account for the block we just calculated.  This must move us to a bndrow     mask bdy
       goto abortcol;   // rowx and accumsumsq must be set
      }
     }
    
// obsolete     }else if((I)zv&ZVISNOTONECOL){  // nonimp   scaf can remove
// obsolete      __m256d threshcmp;  // results of comparing column value ve min
// obsolete      // looking for nonimproving pivots (only in cols where selector<0, in rows where bk is near0).  We choose the first one we see that is above the threshold (which here implies non-dangerous).
// obsolete      if(unlikely(!_mm256_testz_pd(threshcmp=_mm256_cmp_pd(dotproducth,colbk0thresh,_CMP_GT_OQ),endmask))){  // if any compare is true, we have a match
// obsolete       int mask1=_mm256_movemask_pd(threshcmp);  // see which value(s) succeeded
// obsolete       // We must pick the first nonimp in the random order. bestcol is the order in bkg, bestcolrow is the index in bk
// obsolete       ndotprods+=bvgrd-bvgrd0+1; bestcol=(bvgrd-bvgrd0)+CTTZI(mask1); bestcolrow=bvgrd[CTTZI(mask1)]; goto return2;
// obsolete      }
   }else{
    // ************************************************************************** one-column mode+SPR: store out the values, setting to 0 if below threshold
    __m256d okmask=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,dotproducth),store0thresh,_CMP_GT_OQ);  // 0s where we need to clamp
// obsolete      dotproducth=_mm256_and_pd(_mm256_xor_pd(dotproducth,minsprshared),okmask);  // set values < threshold to +0
// obsolete      dotproductl=_mm256_and_pd(_mm256_xor_pd(dotproductl,minsprshared),okmask);  // low part too.
    if((I)zv&ZVNEGATECOL){dotproducth=_mm256_xor_pd(sgnbit,dotproducth); dotproductl=_mm256_xor_pd(sgnbit,dotproductl);}  // Handle Enforcing column. branch will predict correctly and will seldom need the XOR
    dotproducth=_mm256_and_pd(dotproducth,okmask); dotproductl=_mm256_and_pd(dotproductl,okmask); // set values < threshold to +0, high and low parts
// obsolete     if(likely(_mm256_testc_pd(endmask,sgnbit))){  // if all lanes are valid...
    _mm256_storeu_pd(remflgs(zv)+rowx,dotproducth); _mm256_storeu_pd(remflgs(zv)+rowx+bkzstride,dotproductl);   // store high & low.  This may overstore
// obsolete     }else{
// obsolete      _mm256_maskstore_pd(remflgs(zv)+rowx,_mm256_castpd_si256(endmask),dotproducth); _mm256_maskstore_pd(remflgs(zv)+rowx+n,_mm256_castpd_si256(endmask),dotproductl);  // store, masking
// obsolete     }
    // We treat Fk as part of the column, except that we don't want to include it in the SPR calculation.
    if(unlikely(rowx==frowbatchx)){
     // We hit the Fk batch.  Turn off the last lane of endmask, which must hold Fk
     endmask=_mm256_blend_pd(_mm256_setzero_pd(),_mm256_permute4x64_pd(endmask,0b11111001),0b0111);
     dotproducth=_mm256_and_pd(dotproducth,endmask);  // we expect dotprod 0 in any lane where endmask is 0.  Only high part used for SPR
    }
// obsolete     zv+=NPAR;  // advance to next output location, preserving flags
// obsolete    if(((I)zv&(ZVISDIPGRAD|ZVISDIP))>ZVISDIP){  // ********************************* DIP
// obsolete     if(!(bvgrd0!=ONECOLGRD0)){  // DIP
    // Calculate SPR for column.  process the values in parallel
    // Get the column values c and b values to use.  In bound columns we use abs(c).  b is b usually, but beta-b in bound columns where c was negative
    __m256d bnddotproducth=_mm256_andnot_pd(_mm256_and_pd(bndmsk,sgnbit),dotproducth);  // take abs of col value if bound row
    __m256d cgt0=_mm256_cmp_pd(bnddotproducth,col0thresh,_CMP_GT_OQ);  // mask of valid c-value lanes.  Will always be 0 in lanes where endmask is 0 (must pad Qk with 0)
    if(!_mm256_testz_pd(cgt0,endmask)){  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  skip batch if so
     // there is a positive c value.  process the batch.  Get bk to use
     __m256d bk4, beta4;  // will be ~0 for words that have positive c; the 4 bk values we will use (with overshoot added by caller before we start)
     bk4=_mm256_loadu_pd(&bk[rowx]); beta4=_mm256_loadu_pd(&bkbeta[rowx]);  // load bk and beta to use  this overfetches the bk/bkbeta areas, which must be padded to batch length including Fk
     bk4=_mm256_blendv_pd(bk4,_mm256_sub_pd(beta4,bk4),_mm256_and_pd(bndmsk,dotproducth));  // b, except beta-b in bound rows with originally negative col
     // if bk < threshold, set it to 0.  If col value negative, mark the column invalid
     __m256d bknon0=_mm256_cmp_pd(bk4,bk0thresh,_CMP_GE_OQ);  // remember which bks are non0
     bk4=_mm256_and_pd(bk4,bknon0);  // make near0 bk exactly 0
     __m256d ratios=_mm256_fmsub_pd(sharednormorspr,bnddotproducth,bk4);  // minspr*c-b: sign set if b>minspr*c => b/c>minspr => not new min-or-tied SPR.  minspr, c, and b are all nonneg in valid lanes, so no -0
if(!_mm256_testz_pd(_mm256_or_pd(bk4,_mm256_or_pd(sharednormorspr,bnddotproducth)),cgt0))SEGFAULT;  // scaf verify no -0
     if(!_mm256_testc_pd(ratios,cgt0)){  // CF is 1 if ~(not new min-or-tied)&(valid) is all 0, so 0 if any (min-or-tied)
      // SPR is a new low-or-tied (we have to accept ties so all bk=0 lines will come through here).  If any valid bk is 0, we will be returning a nonimproving pivot
      // by simulation this code runs 4.7 times in 64 batches
      if(_mm256_testc_pd(bknon0,cgt0)){    // if all non0 flags in valid lanes are set, that means the SPR is nonzero 
       // New SPR is nonzero.  Calculate it and remember the new SPR in all lanes
       // We will have to find the location of the minimum value in the new batch, because it may be tied with the current minimum.  To silence invalid lanes
       // we keep the old minspr in them.  Could use infinity, but we might calculate an SPR of inf
       __m256d sprs=_mm256_blendv_pd(sharednormorspr,_mm256_div_pd(bk4,bnddotproducth),cgt0);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch because it ties up IUs for so many cycles
       // Find the new minimum
       sharednormorspr=_mm256_min_pd(sprs,_mm256_permute_pd(sprs,0b0101)); // sharednormorspr = min01 min01 min23 min23  could use integer in AVX-512
       sharednormorspr=_mm256_min_pd(sharednormorspr,_mm256_permute4x64_pd(sharednormorspr,0b00001010));  // sharednormorspr=min value in all lanes
       // remember the row that had the minimum.  Make sure we take only SPRs for valid col values
       limitrowx=rowx+CTTZI(_mm256_movemask_pd(_mm256_and_pd(cgt0,_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(sharednormorspr),_mm256_castpd_si256(sprs))))));  // index of a lane containing the minimum SPR (there must be one)
       // it is just barely possible that the division to calculate SPR gives a zero value even though bk is not 0 (underflow).  Since SPR=0 is a flag value indicating
       // no improvement, we will miss the limitrowx that is set here, and the pivot will be marked dangerous unless a different nondangerous nonimproving pivot
       // is found elsehere.  This can happen only once per thread, because once SPR=0 it is impossible to get low-or-tie unless bk=0.  We ignore the possibility,
       // because the column value would have to HUGE to underflow to 0.
      }else{
       // One of the valid-column bk values is 0.  We randomly choose a nondangerous pivot with bk=0.  Once we hit a 0 SPR we will come through here on all subsequent 'new' SPRs
       I candlanes=_mm256_movemask_pd(_mm256_andnot_pd(bknon0,_mm256_cmp_pd(bnddotproducth,_mm256_set1_pd(colokpivotthresh),_CMP_GE_OQ)));   // lanes that could be the result
       if(likely(candlanes!=0)){   // if there is a nondangerous value...
        // deal a random value.  Low quality is OK.  We leave RNG unseeded so that repeated runs will give different sequences
        I4 randval=rand();  // a few bits
        // select a random candidate lane.  This distribution is not uniform, but that's OK since we are just trying to break cycles
        I lanehere=rowx+CTLZI(LOWESTBIT(candlanes)|(candlanes&randval));  // randomly turn off candidates above the lowest, then find highest survivor
        // randomly accept the new value so as to give a uniform distribution of batches
        nzerobatches+=1.0;  // include the new batch in the count of batches encountered
        limitrowx=(double)randval*nzerobatches<=(double)RAND_MAX?lanehere:limitrowx;  // accept new lane with declining probability
       }
       sharednormorspr=_mm256_setzero_pd();  // whether the value is dangerous or not, the new SPR is +0 (important so we can compare against integer 0).  We may find no nondangerous value
      }
      // if the SPR goes to the abort threshold (-inf normally, but 0.0 to indicate we expect improvement), abort the column.  We DO NOT check for aborts issued by other
      // columns, because the case is rare and the sharing has a cost.  We do force out -inf for the share, which will at least prevent other threads from
      // wasting bus cycles sharing their values
      if(unlikely(_mm256_cvtsd_f64(sharednormorspr)<=abortspr)){retinfo=-1; goto return4;}  // abort if SPR low enough
     }
    }
// obsolete     // fetch the b values for the rows. Handle bound vars, which choose from 2 possible b values
// obsolete     __m256d cnon0, bk4;  // will be ~0 for words that have positive c; the 4 bk values we will use (with overshoot added by caller before we start)
// obsolete     if((I)zv&ZVUNBOUND){  // normal unbound case.  The b values come in order
// obsolete      if((I)zv&ZVNEGATECOL)dotproducth=_mm256_xor_pd(sgnbit,dotproducth);  // Handle Enforcing column. branch will predict correctly and will seldom need the XOR
// obsolete      // skip the block if all values are negative or near 0
// obsolete      cnon0=_mm256_cmp_pd(dotproducth,col0thresh,_CMP_GT_OQ);  // ~0 for valid positive c values
// obsolete      if(_mm256_testz_pd(cnon0,endmask))goto skip0col;  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  if one is big enough...
// obsolete      bk4=likely(bvgrd<bvgrde)?_mm256_loadu_pd(bv+(bvgrd-bvgrd0)):_mm256_maskload_pd(bv+(bvgrd-bvgrd0),_mm256_castpd_si256(endmask));  // the next bk values.  Need 0s at end
// obsolete     }else{
// obsolete      // processing bound row: we have to choose which one we are using based on the sign of the column value.
// obsolete      // choose bv (b) or bkboundv (b-beta) based on sign of c.  Values are ordered to match bkgrd
// obsolete      __m256d bkbound4, dph=dotproducth;
// obsolete      cnon0=_mm256_cmp_pd(dotproducth=_mm256_andnot_pd(sgnbit,dotproducth),col0thresh,_CMP_GT_OQ);  // take absolute value of c; ~0 for valid positive c values
// obsolete      if(_mm256_testz_pd(cnon0,endmask))goto skip0col;  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  if one is big enough...
// obsolete      if(likely(bvgrd<bvgrde))bk4=_mm256_loadu_pd(bv+(bvgrd-bvgrd0)), bkbound4=_mm256_loadu_pd(bkboundv+(bvgrd-bvgrd0));
// obsolete      else bk4=_mm256_maskload_pd(bv+(bvgrd-bvgrd0),_mm256_castpd_si256(endmask)), bkbound4=_mm256_maskload_pd(bkboundv+(bvgrd-bvgrd0),_mm256_castpd_si256(endmask));  // the next bk values.  Need 0s at end
// obsolete      if((I)zv&ZVNEGATECOL)dph=_mm256_xor_pd(sgnbit,dph);  // Handle Enforcing. branch will predict correctly and will seldom need the XOR
// obsolete      bk4=_mm256_blendv_pd(bk4,bkbound4,dph);  // bk if c positive, b-beta if negative
// obsolete     }
// obsolete        __m256d bk4=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),bv,rownums,endmask,SZI);  // the bk values we are working on
// obsolete     // if(there are values to process){
// obsolete      zv=(D*)((I)zv|ZVPOSCVFOUND);  // if any c>thresh, zf is 0; set that we are bounded.
// obsolete      // see if any of the new values is limiting: b/c < min SPR, c>ColThresh, bk>bkthresh
// obsolete      // we perform this calculation on each lane separately, but share the smallest values each time
// obsolete      __m256d cbadifbk0=_mm256_cmp_pd(dotproducth,colbk0thresh,_CMP_GT_OQ);  // set to ~0 if c>ColBkThresh, i. e. column is blocked by bk=0 - always 0 if endmask 0
// obsolete      __m256d ratios=_mm256_fnmadd_pd(_mm256_and_pd(cbadifbk0,minsprshared),dotproducth,bk4);  // b-minspr*c: sign set if b<minspr*c => b/c<minspr => this is a new min SPR in its lane.  If col is near 0, force minspr to 0, which clears the sign bit whenever bk4>0
// obsolete        // by using cbadifbk0 rather than cnon0 we are ignoring the (valid) SPRs created by small column values.  This probably doesn't matter because those SPRs are
// obsolete        // big anyway if bk!=0; but we do it mostly to match the dp code
// obsolete      __m256d bknot0=_mm256_cmp_pd(bk4,bk0thresh,_CMP_GE_OQ); // we separate b into < threshold and >= threshold.   This must match the selection made for stall exit
// obsolete      if(unlikely(!_mm256_testc_pd(bknot0,cbadifbk0)))goto abortcol;  // CF is 0 if any sign of ~a&b is 1 => ~(b>=bthresh) & c>BkThresh.  Positive c and b<=0 means we cannot pivot in this column
// obsolete      if(unlikely(!_mm256_testc_pd(cbadifbk0,cnon0)))zv=(D*)((I)zv|ZVFFREQD);  // CF is 0 if any sign of ~a&b is 1 => ~(c>BkThresh) & c>0thresh.  We are ignoring a non0 pivot; we will need forcefeasible; flag that
// obsolete      if(!_mm256_testz_pd(ratios,bknot0)){  // ZF is 1 if a&b signs all 0 => (new min&c ok if b!=0)&(b!=0) all 0 => no place where new min & c good & b good
// obsolete       // normal case bypasses this block
// obsolete       // at least one new SPR is limiting (in its lane).  Find the SPRs
// obsolete       __m256d sprs=_mm256_div_pd(bk4,dotproducth);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch because it ties up IUs for so many cycles
// obsolete       // the reciprocal takes a long time to run.  Make sure there are no dependencies.  The block of code
// obsolete       // issued here will complete while the next block of dotproduct is being computed.
// obsolete       if((I)zv&ZVSPRNOTFOUND){zv=(D*)((I)zv&~ZVSPRNOTFOUND); /* later firstsprrow=(bvgrd-bvgrd0);*/}  // index of (close to) first valid SPR.  Branch will predict correctly.  Set state to show that we are bounded and have first SPR
// obsolete       __m256d validlimit=_mm256_and_pd(ratios,bknot0);  // pivot is valid and limiting - high bit only.  c too low was eliminated earlier; bk too low eliminated now
// obsolete       minspr=_mm256_blendv_pd(minspr,sprs,validlimit);   // remember the limiting SPR, in each lane
// obsolete       limitrows=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(limitrows),_mm256_castsi256_pd(rownums),validlimit));  // if there is a new limiting SPR, remember its row number
// obsolete       limitcs=_mm256_blendv_pd(limitcs,dotproducth,validlimit);  // remember the column value at the limiting SPR.  We will classify it after we find the winning SPR
// obsolete       // check for cutoff
// obsolete       if(unlikely(!_mm256_testc_pd(_mm256_fmsub_pd(minimpspr,dotproducth,bk4),validlimit))){  // cutoff if Frow*b/c > minimp (Frow and minimp negative) => b/c < (minimp/Frow = minimpspr) => b < minimpspr*c => minimpspr*c-b > 0. i. e. any sign 0 when b!=0 => CF bit set
// obsolete        // one of the values cuts off.  Abort this column, keeping the SPR and position.  But if the prirow is winning, don't abort - we want it out
// obsolete        if(likely(_mm256_testz_si256(rowstride,_mm256_cmpeq_epi64(_mm256_set1_epi64x(prirow),limitrows))))goto abortcol;   // rowstride is any 256i with nonzero in all lanes
// obsolete       }
// obsolete       // update the shared minimum-spr in all lanes
// obsolete       minsprshared=_mm256_min_pd(minspr,_mm256_permute_pd(minspr,0b0101)); // minsprshared = min01 min01 min23 min23  could use integer in AVX-512
// obsolete       minsprshared=_mm256_min_pd(minsprshared,_mm256_permute4x64_pd(minsprshared,0b00001010));  // minsprshared=min value in all lanes
// obsolete      }
// obsolete     skip0batch: ;  // all column values were 0/small so we skipped them all
   }  // end of processing by mode
   bndmsk=_mm256_castsi256_pd(_mm256_slli_epi64(_mm256_castpd_si256(bndmsk),1));  // move bound-row mask to the next group of rows.
   rowx+=NPAR;  // advance input row number
// obsolete  mv+=n<<LGNPAR;; advance fetch base pointer
  }
  // done with loop down one column.  Here the column ran to completion

  // End-of-column processing.  Collect stats and update parms for the next column
  // In onecol+SPR mode, we a sharing the SPR, looking for the smallest
  //  limitrowx has the row that the shared value was found in, or IMAX if none found to tie the incumbent
  // In gradient mode, we are sharing the gradient, looking for the smallest (we currently have sumsq of columns)

  I valuetoshareminof;   // because atomics run int-only, this is the bit-pattern that we will minimize as an I
  if(likely(!ZVISONECOL)){
   // gradient mode.  Abort if unbounded column
   if(!((I)zv&ZVPOSCVFOUND)){retinfo=colx; goto return4;}  // no positive column value, problem is unbounded
   // calculate 1/gradient^2 for the column, which is positive and we want to minimize (to maximize gradient).  It is sumsq/Frow^2
   sharednormorspr=_mm256_add_pd(_mm256_permute_pd(sharednormorspr,0b0101),sharednormorspr); sharednormorspr=_mm256_add_pd(_mm256_permute4x64_pd(sharednormorspr,0b01001110),sharednormorspr);   // total sumsq
   accumsumsq+=_mm256_cvtsd_f64(sharednormorspr);  // final total of column normsq
usefullrowtotal:;  // come here when we started knowing the row total, in accumsumsq
   D recipgradsq=accumsumsq/(Frow[colx]*Frow[colx]);  // 1/gradient^2
   valuetoshareminof=*(I*)&recipgradsq;  // share the recip gradient, looking for mimimum.  We have the gradient that was completed for this column
  }else{
   // onecol+SPR. sharednormorspr already has the nonneg value to be shared and minimized.  We might not have achieved it here, though
   // We recognize the value only if we actually achieved it in this thread, so that we won't report out invalid retinfo (the issue is that
   // sharednormorspr may contain the initial value and we would have no valid value to override the default with)
   // SPR=0 poses a special challenge, because an early SPR might wipe out the default value and there might be no nondangerous value
   // to replace it with.  So we use two different SPR values for SPR=0: 0 to mean dangerous, i. e. no valid row, and IMIN (=-0) to mean that the row
   // is valid.  As usual, we will store the row, which might be garbage, whenever we see that we have the minimum share.  The caller must recoognize
   // the SPR value
   I threadspr=_mm256_extract_epi64(_mm256_castpd_si256(sharednormorspr),0);
   valuetoshareminof=limitrowx==IMAX?IMAX:threadspr;  // if we never actually saw a minimum value, set share to greater than inf, to ensure no sharing
   if(threadspr==0)valuetoshareminof=nzerobatches==0.?0:-1;  // if nonimproving separate into danger/nondanger cases
  }

  // if this value is a new minimum, share it with the other cores (might not be if last batch went over but was not tested for cutoff)
  // If the new value is equal to the old, there is no need to share it, but we must remember the retinfo, in case we end up with the minimum and write out our retinfo
  if(valuetoshareminof<=*(I*)&sharedmin){  // if we might be reporting this value out at the end...
   retinfo=likely(!ZVISONECOL)?colx:limitrowx;  // save the col/row, as appropriate
   if(valuetoshareminof<*(I*)&sharedmin){  // if the value is dominated by a previously-read value, it can't need reporting
    // new best as far as we know.  try updating the current value
    I incumbentimpi=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // load incumbent best improvement
    while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
     if(valuetoshareminof>incumbentimpi)break;  // if not new global best, don't update global.  In this case we don't need to remember the value of this column, since it has been beaten
     if(__atomic_compare_exchange_n(&ctx->sharedmin.I,&incumbentimpi,valuetoshareminof,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){
      minvalueweshared=valuetoshareminof;  // wait till now to remember best-in-thread.  There is no need to remember a value that we didn't share
// obsolete      bestcol=colx; bestcolrow=limitrowx;  // update the found position
      break;
     }  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
    }
   }
  }

// obsolete   if(!((I)zv&ZVISDIPGRAD))goto earlycol;  // oneprod, only one column, skip setup
// obsolete   // from here on, DIP/gradient mode; use zv to indicate 'improvement found'
// obsolete   union __attribute__((aligned(CACHELINESIZE))) {I quadI[NPAR]; D quadD[NPAR]; } extractarea;  // place where we can see individual values
// obsolete // obsolete   if(unlikely(bvgrd0==ONECOLGRD0)){
// obsolete   if((I)zv&ZVISDIP){
// obsolete    _mm256_store_pd(&extractarea.quadD[0],limitcs); // DIP mode: leave minspr as is (holding SPR), save column values
// obsolete   }else{
// obsolete    // gradient mode: limitcs/colbk0thresh are Kahan accumulator for sumsq; minsprshared holds max col value; limitrows holds index of max column values
// obsolete    // We switch the sign of minsprshared because the code for SPR searches for smallest value
// obsolete    _mm256_store_pd(&extractarea.quadD[0],minsprshared);   // we want the biggest positive value
// obsolete    minspr=_mm256_sub_pd(_mm256_setzero_pd(),minsprshared);  // now the lanes of minspr have -(max value)
// obsolete    zv=(D*)ZVPOSCVFOUND;   // set cv found, SPR found; continue with DIP mode off
// obsolete   }
// obsolete   // now minspr has a value to minimize, and extractarea has the true value to use

// obsolete   // column ran to completion.  Detect unbounded
// obsolete   if(!((I)zv&ZVPOSCVFOUND)){
// obsolete    // no pivots found for a column, problem is unbounded, indicate which column in the NTT, i. e. the input value which is an identity column if < #A
// obsolete    // BUT it is possible that a column that looks unbounded in double precision will be bounded in quad; so retry the column in that case
// obsolete    if((I)zv&ZVDP)goto retryinquad;  // if we haven't used qp yet, retry the column in extended precision
// obsolete    bestcolrow=0-1; bestcol=colx; goto return4;  // unbounded in qp: indicate where, with row=-1 as a flag
// obsolete   }
// obsolete   // it is just possible that the entire column would be processed with no SPR found (all the rows would have to have c=0 b=0).
// obsolete   // In that case, ignore the column
// obsolete   if(likely(!((I)zv&ZVSPRNOTFOUND))){   // state 1/3: no SPR received yet
// obsolete    // The new column must have better gain than the previous best (since we had a pivot & didn't abort).  Remember where it was found and its improvement, unless it is dangerous.
// obsolete    // Exception: if a nondangerous pivot pivots out a virtual row, we accept it immediately and abort all other processing
// obsolete    __m256d allmin=_mm256_min_pd(minspr,_mm256_permute_pd(minspr,0b0101)); // allmin = min01 min01 min23 min23  could use integer in AVX-512
// obsolete    allmin=_mm256_min_pd(allmin,_mm256_permute4x64_pd(allmin,0b00001010));  // allmin=min value in all lanes
// obsolete    I minx=CTTZI(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(allmin),_mm256_castpd_si256(minspr)))));  // index of a lane containing the minimum SPR
// obsolete    D minc=extractarea.quadD[minx];  // get column value at minimum SPR
// obsolete    _mm256_store_si256((__m256i *)&extractarea.quadI,limitrows); I minrownum=extractarea.quadI[minx];  // get bkgrd index of winning row
// obsolete    D spratmin=_mm256_cvtsd_f64(allmin);    // the minimum SPR itself
// obsolete    if(likely(minc>=coldangerpivotthresh)){  // if pivot is so low that it will damage the problem, skip over it, try next column
// obsolete     if(likely(minc>=colokpivotthresh)){  // if this pivot is not dangerous, see if it is a new global best (for the time being); if so, share the improvement and remember that we have it
// obsolete      // update the best-gain-so-far to the actual value found - but only if this is not a dangerous pivot.  We don't want to cut off columns that are beaten only by a dangerous pivot
// obsolete       // if the pivot was found in a virtual row, stop looking for other columns and take the one that gets rid of the virtual row.  But not if dangerous.
// obsolete      I incumbentimpi=__atomic_load_n((I*)&(ctx->minimp),__ATOMIC_ACQUIRE);  // load incumbent best improvement
// obsolete      D minimpthiscol;  // the best FOM we found for this column
// obsolete      // get FOM for this column
// obsolete // obsolete       if(likely(bvgrd0!=ONECOLGRD0))minimpthiscol=Frow[colx]*spratmin;  // (DIP) this MUST be nonzero, but it decreases in magnitude till we find the smallest pivotratio.  This is our local best for this column
// obsolete      if(likely((I)zv&ZVISDIP)){
// obsolete       minimpthiscol=Frow[colx]*spratmin;  // (DIP) this MUST be nonzero, but it decreases in magnitude till we find the smallest pivotratio.  This is our local best for this column
// obsolete       if(minrownum==n)minimpthiscol*=swapbounty;  // if the min SPR was for a (nonbasic) bound swap, exaggerate its improvement
// obsolete      }else{  // (gradient)
// obsolete       totalnorm=_mm256_add_pd(_mm256_permute_pd(totalnorm,0b0101),totalnorm); totalnorm=_mm256_add_pd(_mm256_permute4x64_pd(totalnorm,0b11100110),totalnorm);   // take total across lanes
// obsolete       *(I*)&minimpthiscol=_mm256_extract_epi64(_mm256_castpd_si256(totalnorm),0);  // total sumsq+1 of column
// obsolete       minimpthiscol=minimpthiscol/(Frow[colx]*Frow[colx]);  // gradient mode: save minimum of sumsq/Frow^2 which is recip of the maximum gradient^2 (so smaller=better)
// obsolete // obsolete        minrownum=(I)((I*)minrownum-ONECOLGRD0)+minx;  // convert from bv ptr to index, and then add winning lane#
// obsolete      }
// obsolete      if(unlikely((I4)minrownum==(I4)prirow)){bestcol=colx; bestcolrow=minrownum|(((I)zv&ZVFFREQD)<<(31-ZVFFREQDX)); ndotprods+=bvgrd-bvgrd0; goto return4;}  // stop early if virtual pivot found, but take credit for the column we process here
// obsolete      while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
// obsolete       if(minimpthiscol>=*(D*)&incumbentimpi)break;  // if not new global best, don't update global.  In this case we don't need to remember the value of this column, since it has been beaten
// obsolete       if(__atomic_compare_exchange_n((I*)&(ctx->minimp),&incumbentimpi,*(I*)&minimpthiscol,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){
// obsolete        minimpfound=minimpthiscol;  // wait till now to remember best-in-thread, because numerical error may allow minimpthiscol to be not as good as an earlier column
// obsolete        bestcol=colx; bestcolrow=minrownum|(((I)zv&ZVFFREQD)<<(31-ZVFFREQDX));  // update the found position
// obsolete        zv=(D*)((I)zv|ZVMOD);  // set flag indicating 'this was an improvement'
// obsolete        break;
// obsolete       }  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
// obsolete      }
// obsolete     }else{
// obsolete      // the best SPR was dangerous.  We don't update the improvement, and we don't try to pick the best improvement.  Rather, we keep the least dangerous pivot, i. e. the largest
// obsolete      if(bestcol<0&&minc>maxdangerc){  // if we have not found a nondangerous pivot yet and this is a new local best among dangerous pivots...
// obsolete       bestcol=colx|(1LL<<31); bestcolrow=minrownum|(((I)zv&ZVFFREQD)<<(31-ZVFFREQDX));  // update the found position as dangerous, with forcefeasible flag in bit 31.
// obsolete       maxdangerc=minc;  // remember the largest dangerous pivot we have found here
// obsolete      }
// obsolete     }
// obsolete    }
// obsolete   }
// obsolete   --bvgrd;  // undo the +1 in the product-accounting below

  // we have finished processing the column, and reporting its gain if DIP.  Now prepare for the next column
  I impcolincr;  // value we will add to the col-stats field
  //  ***** jump to here on aborted column *****   rowx must point to the batch after the last one we processed
  if(1)impcolincr=0x100000001; else abortcol: impcolincr=1; // jump here if column aborted early, possibly on insufficient gain.  This is the normal path if abort, don't incr # completed columns
  // exit if we have processed enough columns (in DIP mode)
  I nimpandcols=__atomic_add_fetch(&ctx->nimpandcols,impcolincr,__ATOMIC_ACQ_REL);  // add on one finished column, and 0 or 1 improvements
// obsolete   if(unlikely((nimpandcols>>32)>AN(sched)))goto earlycol;  // if we have more improvements than the schedule imagined, finish
// obsolete   if(unlikely((nimpandcols>>32)>0 && (UI4)nimpandcols>=IAV(sched)[(nimpandcols>>32)-1]))goto earlycol;  // if we have processed enough columns for this level of improvements, finish
// obsolete   ndotprods+=bvgrd-bvgrd0+1;  // accumulate # products performed, including the one we aborted out of
  ndotprods+=rowx;  // accumulate # products performed, including the one we aborted out of
  if(unlikely(ZVISONECOL))break;   // onecol+SPR does not loop for another column
  // gradient mode.  save the total we accumulated.  If the index is >= nrows it may be too high but that's OK
if(rowx<nrows && (rowx&(BNDROWBATCH/1-1))!=0)SEGFAULT;  // scaf
  rowx=(I)zv&ZVPOSCVFOUND?-rowx:rowx;  // make rowx negative if we have verified column is unbounded.  The bounding row may not be in the total; that's OK
  D Drowx=rowx;
  __atomic_store((I*)&cutoffstatus[colx][0],(I*)&Drowx,__ATOMIC_RELEASE);  // only integers can be atomic args
  __atomic_store((I*)&cutoffstatus[colx][1],(I*)&accumsumsq,__ATOMIC_RELEASE);
  // still gradient mode.  prepare for next column
  if(likely(sharingmin))*(I*)&sharedmin=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // update to find best improvement in any thread, to allow cutoff
  currcolproxy=__atomic_load_n(&ctx->nextresv,__ATOMIC_ACQUIRE);  // start load of column position for next spin through loop
// obsolete  }  // this is the end of retryable loop-by-hand
  ++firstcol;  // advance column index
 }  // end of loop over columns
earlycol:;  // come here when we want to stop after the current column

 // return result to the ctx, if we have the winning value.  Simple writes to ctx do not require RELEASE order because the end-of-job code
 // and the threads sync through ctx->internal.nf; but they should be atomic in case we run on a small-word machine

// obsolete  if(!((I)zv&ZVISDIPGRAD))R 0;  // if single column, ctx is unused for return; if nonimp, to get here there must be no pivot found, so just return then too
// obsolete  // Here we are running DIP/gradient and reporting our best result (if it beats the incumbent)
// obsolete  // operation complete; transfer results back to ctx.  To reduce stores we jam the col/row together
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 // if the value we reported out might be the best, store its completion info out if it is actually the best.
 // For the nonce, we hope the thread ordering gives enough randomization for tied zero SPRs
 // We have to write on = so that the row# will be written for zero SPRs where some threads had no nondangerous pivots
 if(minvalueweshared<=*(I*)&sharedmin){  // don't bother if we know another thread is better
// obsolete   I bestcolandrow=((I)bestcol<<32)|bestcolrow;  // combined value to store: row, column, forcefeasible flag
// obsolete  if(likely(bestcol>=0)){
// obsolete   // nondangerous pivot found.  Store it out, under lock, if the local improvement equals the best found so far
// obsolete    // Don't write if this is a zero SPR (in SPR mode) and this thread doesn't have a nondangerous pivot
// obsolete    if(likely(!(ZVISONECOL && minvalueweshared==0 && nzerobatches==0.))){  // if not zero SPR with no pivot...
  I incumbentimpi=__atomic_load_n(&ctx->sharedmin.I,__ATOMIC_ACQUIRE);  // load incumbent best value
  if(minvalueweshared==incumbentimpi){  // check outside of lock for perf
   WRITELOCK(ctx->ctxlock)
   if(minvalueweshared==ctx->sharedmin.I){ctx->retinfo=retinfo;} // if we still have the best gain, report where we found it
   WRITEUNLOCK(ctx->ctxlock)
  }
// obsolete    }
 }
// obsolete  }else{
// obsolete   // dangerous pivot or no pivot found.  Store, under lock, if we have the largest column value and the incumbent is also dangerous or missing
// obsolete   incumbent=__atomic_load_n((I*)&(ctx->maxdanger),__ATOMIC_ACQUIRE);  // load incumbent best value
// obsolete  I incumbcol=__atomic_load_n(&(ctx->bestcolandrow),__ATOMIC_ACQUIRE);  // ...and col/row (neg if none or dangerous)
// obsolete   if(incumbcol<0 && maxdangerc>*(D*)&incumbent){  // check outside of lock for perf
// obsolete    WRITELOCK(ctx->ctxlock)
// obsolete    if(ctx->bestcolandrow<0 && maxdangerc>ctx->maxdanger){ctx->bestcolandrow=bestcolandrow; ctx->maxdanger=maxdangerc;}   // if we still have the max value, report where we found it
// obsolete    WRITEUNLOCK(ctx->ctxlock)
// obsolete   }
// obsolete  }
 R 0;

// following are the early return points: nonimproving pivot, unbounded, pivot out virtual

// obsolete return2:  // (nonimp) here we found a nonimproving pivot.  Cut off other threads, Report it if it is best, then return
// obsolete  __atomic_store_n(&ctx->nextcol,collen,__ATOMIC_RELEASE);  // set next-'column' to 'finished' to suppress any further reservations
// obsolete  __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
// obsolete  // save the pivot we found only if it is in the smallest 'column' (which means smallest bvgrd)
// obsolete  I newval=((I)bestcol<<32)|bestcolrow;  // the composite we want to store, always positive
// obsolete  I oldval=__atomic_load_n(&(ctx->bestcolandrow),__ATOMIC_ACQUIRE);  // incumbent, negative to begin with
// obsolete  while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
// obsolete   if((UI)newval>=(UI)oldval)break;  // if not new global best, don't update global.
// obsolete   if(__atomic_compare_exchange_n(&(ctx->bestcolandrow),&oldval,newval,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))break;  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
// obsolete  }
// obsolete  R 0;
return4:;  // we have a preemptive DIP/gradient result.  store and set minimp =-inf to cut off all threads
 // the possibilities are unbounded and pivoting out a virtual.  We indicate unbounded by row=-1.  We may overstore another thread's result; that's OK
 // We must do this under lock to make sure all stores to bestcol/row have checked the minimp they use
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 WRITELOCK(ctx->ctxlock)
 ctx->sharedmin.I=IMIN;  // shut off other threads
 ctx->retinfo=retinfo;  // set unbounded col or col/row of virtual pivot
 WRITEUNLOCK(ctx->ctxlock)
 R 0;
}
#endif //C_AVX2

// 128!:9 matrix times sparse vector with optional early exit
// one column + SPR mode: (qp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
//   Ax,Am,Av are the sparse elements of A0
//   M is Qk.  There may be padding rows but NOT columns
//   RVT is Rose variable types, indicating which columns are (bound,enforced) (littleendian order, with enforced being the LSB)
//   bndrowmask indicates, for each row in Qk, whether the row is Bound
//   bk is the current bk (must include a 0 corresponding to Fk and then MUST BE 0-EXTENDED to a batch boundary) 
//   z is the result area for the requested column - qp, and must be able to handle overstore to a full batch, including the Fk value if any
//   ndx is the atomic column# of NTT requested
//   parms is #rows,maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,[BkOvershoot],[MinSPR],[PriRow]
//    #rows is number of rows in the Ek portion of Qk; if Fk present too, value is 1s-comp
//    maxAx is the length of the longest dot-product
//    Col0Threshold is minimum |value| considered non0 for a column value
//    Store0Thresh: if |column value| is less than this, force it to 0
// obsolete //    ColBk0Threshold is minimum value that will block a pivot when bk=0
//    ColDangerPivot is the smallest allowed pivot, but is dangerous
//    ColOkPivot is the smallest pivot value considered non-dangerous
//    Bk0Threshold is the smallest bk value considered nonzero
//    BkOvershoot is the maximum excursion into negative bk that we will allow on a pivot
//    MinSPR is the largest SPR that will abort this column
//    PriRow is the priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so
//   bkbeta is beta for each row in Qk, i. e. corresponding to bk (including 0 extension)
//   beta is betas for all columns, used to init SPR for bound column
//  if ndx<m, the column is  ndx {"1 M; otherwise ((((ndx-m){Ax) ];.0 Am) {"1 M) +/@:*"1 ((ndx-m){Ax) ];.0 Av
//   multiplied by -1 if the column is enforcing
//  Do the product in quad precision
//  the column data is written to z
//  result is rc,pivotrow,#cols processed,#rows processed,smallest SPR
//   rc is 0 (normal), 1 (dangerous), 2 (nonimproving), 3 (no pivot found - all 0 SPRs were dangerous), 4 (SPR was less than MinSPR)
// gradient mode: (dp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
//   cutoffinfo is internal stored state saved between gradient-mode calls; shape n,2
//   parms is #rows,maxAx,Col0Threshold,x,MinGradient/MinGradImp,x
//    MinGradient is normally 0.
//      if positive, we do NOT share gradient values, and we finish up gradient calculation in any column that has a larger |gradient|, storing the len/sumsq in cutoffinfo
//      if negative, cut off a column when its gradient cannot beat the best-so-far by more than |MinGradImp|
//   Frow is the selector row
//  result is rc,pivotcol or unbounded col,#cols processed,#rows processed,largest |gradient|
//   rc is 0 (normal), 4 (unbounded problem)
// findimprovement mode: (dp dotproduct)
//  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta  where ndx is a list and z is empty
//   parms is #rows,maxAx,ColOkPivot,x,x,x,x,Bk0Threshold,BkOvershoot,PriRow
//  result is list of columns that have positive SPR (no line with col>ColBk0Threshold && bk<Bk0Threshold)
//  
// Rank is infinite
F1(jtmvmsparse){PROLOG(832);
#if C_AVX2
 if(AN(w)==0){
  // empty arg just returns stats; none defined yet
  R mtv;
 }
// obsolete #define YC(n) .n=n,
struct mvmctx opctx;  // parms to all threads, and return values
// obsolete ={.ctxlock=0,.bestcolandrow=-1,.ndxa=box0,YC(n)YC(bv)YC(zv)
// obsolete  YC(bvgrd0)YC(bvgrde)YC(parms)YC(sched)YC(Frow)YC(minimp)YC(bkbound)YC(betav)YC(rvtv).axv=((I(*)[2])IAV(box1))-n,.amv0=IAV(box2),.avv0=DAV(box3),.qk=box4,  // minimp cannot be _ because it may be multiplied by 0
// obsolete  .ndotprods=0,.nimpandcols=0,.nextcol=0,.maxdanger=0.0};  // bias axv pointer back to start of (virtual) NTT
// obsolete #undef YC

 ASSERT(AR(w)==1,EVRANK);
 ASSERT(AN(w)>=11,EVLENGTH);  // audit overall w
 ASSERT(AT(w)&BOX,EVDOMAIN);
 A box;
// obsolete =C(AAV(w)[0]), box1=C(AAV(w)[1]), box2=C(AAV(w)[2]), box3=C(AAV(w)[3]), box4=C(AAV(w)[4]), box5=C(AAV(w)[5]);
// obsolete  A box6=C(AAV(w)[6]), box7=C(AAV(w)[7]), box8=C(AAV(w)[8]), box9=C(AAV(w)[9]), box10=C(AAV(w)[10]);
 box=C(AAV(w)[9]); ASSERT(AT(box)&FL,EVDOMAIN); ASSERT(AR(box)==1,EVRANK); ASSERT(AN(box)>0,EVLENGTH);  // parm shape, type
 D *parms=opctx.parms=DAV(box); I nparms=AN(box); I nandfk=opctx.nbasiswfk=(I)parms[0]; I nbasiscols=nandfk^REPSGN(nandfk);   // flagged n
 box=C(AAV(w)[3]); ASSERT(AR(box)==3,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.qk=box;  // Qk, possibly including space and Fk
 ASSERT(AS(box)[0]==2,EVLENGTH)  // Qk is qp
 I ninclfk=AS(box)[1];   // number of rows to be processed including Fk
 ASSERT(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,EVNONCE)  // we fetch along rows; insist on data alignment
// obsolete  opctx.qkrowstride=AS(box)[2];  // length of row of Qk
// obsolete  opctx.qkstride=AS(box)[1]*AS(box)[2];  // distance between planes of Qk
 I minbkzstride=(ninclfk+NPAR-1)&-NPAR;  // size of bk/bkbeta/z: enough for last batch including Fk if present
 box=C(AAV(w)[0]); ASSERT(AR(box)==3&&AS(box)[1]==2&&AS(box)[2]==1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN); I axn=AN(box); opctx.axv=((I(*)[2])IAV(box))-nbasiscols;  // prebiased pointer to A0 part of NTT
 box=C(AAV(w)[1]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN); I amn=AN(box); opctx.amv0=IAV(box);  // col #s
 box=C(AAV(w)[2]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN); I avn=AN(box); opctx.avv0=DAV(box);  // weights
 ASSERT(amn==avn,EVLENGTH);  // weights and col#s must agree
 box=C(AAV(w)[4]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&LIT,EVDOMAIN) C *rvtv=opctx.rvtv=CAV(box);  // RVT
 box=C(AAV(w)[5]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&LIT,EVDOMAIN) opctx.bndrowmask=DAV(box);  // bndrowmask
 box=C(AAV(w)[8]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AT(box)&INT,EVDOMAIN) I nc=opctx.nc=AN(box);
 ASSERT(nc!=0,EVLENGTH)
 opctx.ndxa=IAV(box); I col0=IAV(box)[0];   // col #(s), first col#
 I isgradmode; D *zv; I nthreads=(*JT(jt,jobqueue))[0].nthreads+1;   // non0 if gradient mode; ptr to output if any; #threads
 if(isgradmode=(AR(box)!=0)){ 
  // gradient mode (the dominant case)
  ASSERT(AN(w)==11,EVLENGTH);  // audit overall w
  // box 6 reserved for schedule
  box=C(AAV(w)[7]); ASSERT(AR(box)==2,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.cutoffstatus=(D(*)[2])DAV(box);  // saved info from previous scans
// obsolete   ASSERT(AC(box)<=3,EVFACE) ASSERT(AFLAG(box)&AFKNOWNNAMED,EVNONCE)  // cutoffinfo unaliased.  We have to allow usecount of 3 for the tests.  Normally uc will be 2
  zv=opctx.zv=0;  // zv=0 means gradient mode
  ASSERT(nparms==6,EVLENGTH)  // gradient mode doesn't use much parms
  box=C(AAV(w)[10]); ASSERT(AR(box)==1,EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) opctx.Frow=DAV(box);  // the entire selector row
  nthreads=likely(ninclfk*nc>1000)?nthreads:1;  // single-thread small problem
  // frowbatchx unused
  opctx.sharedmin.D=inf; // minimum improvement
  opctx.retinfo=-1;  // Init col#, should never be used
 }else{
  // onecol+SPR mode
  ASSERT(AN(w)==12,EVLENGTH);  // audit overall w
  box=C(AAV(w)[6]); ASSERT(BETWEENC(AR(box),1,2),EVRANK) ASSERT(AS(box)[AR(box)-1]>=minbkzstride,EVLENGTH) ASSERT(AT(box)&FL,EVDOMAIN) opctx.bk=DAV(box);  // bk values allow overfetch; we use only high part
  minbkzstride=AS(box)[AR(box)-1];  // all the strides must be equal; we have just verified they are big enough
  ASSERT(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,EVNONCE)  // we fetch along rows; insist on data alignment
  box=C(AAV(w)[7]); ASSERT(AR(box)==2,EVRANK) ASSERT(AS(box)[0]==2,EVLENGTH) ASSERT(AS(box)[AR(box)-1]==minbkzstride,EVLENGTH) ASSERT(AT(box)&FL,EVDOMAIN) zv=opctx.zv=DAV(box);  // zv values allow overstore
// obsolete   ASSERT(AC(box)<=3,EVFACE) ASSERT(AFLAG(box)&AFKNOWNNAMED,EVNONCE)  // zv unaliased
  ASSERT(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,EVNONCE)  // we store along rows; insist on data alignment
  ASSERT(BETWEENC(nparms,10,11),EVLENGTH)  // SPR has lots of parms
  box=C(AAV(w)[10]); ASSERT(BETWEENC(AR(box),1,2),EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) ASSERT(AS(box)[AR(box)-1]==minbkzstride,EVLENGTH) opctx.bkbeta=DAV(box);  // beta values corresponding to bk; we use only high part
  ASSERT(((I)DAV(box)&((SZD<<LGNPAR)-1))==0,EVNONCE)  // we fetch along rows; insist on data alignment
  opctx.bkzstride=minbkzstride;   // use the agreed stride
  nthreads=likely(ninclfk>256)?nthreads:1;  // single-thread small problem (qp)
  // The initial shared value is infinity except in bound columns: then beta.  Initialize the found-row value to -1 for 'nothing found' normally,
  // but for bound columns use #rows which indicates a Nonbasic Swap
  I colrvt=(rvtv[col0>>(LGBB-1)]>>((col0&((1LL<<(LGBB-1))-1))<<1))&((1LL<<2)-1);  // get column info, a 2-bit field of (bound,enforcing)
  if(colrvt&0b10){
   box=C(AAV(w)[11]); ASSERT(BETWEENC(AR(box),1,2),EVRANK) ASSERT(AT(box)&FL,EVDOMAIN) ASSERT(AN(box)>col0,EVINDEX)  // betas normally dp, but we use only high part anyway.  Look up the column beta
   opctx.sharedmin.D=DAV(box)[col0];   // init bound-col min SPR to beta
   opctx.retinfo=ninclfk;  // Init col# - if no lower SPR found could survive to become a Swap
  }else{opctx.retinfo=-1; opctx.sharedmin.D=inf;} // non-bound, allow any improvement, init ret to 'no row found' - possible if no nondangerous pivot, or unbounded (should not occur)
 }

 // initialize shared/returned section
 opctx.nimpandcols=0;  // number of improvement,number of cols finished
 opctx.ndotprods=0;  // total # dotproducts evaluated
 opctx.nextresv=0;  // start in row/col 0
 opctx.ctxlock=0;  // init lock available
 opctx.nthreads=nthreads;  // number of threads to use

 jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtmvmsparsex,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 // atomic sync operation on the job queue guarantee that we can use regular loads from opctx

 A r; GAT0(r,FL,5,1); D *rv=DAV(r);  // return area
 I retinfo=rv[1]=opctx.retinfo; rv[2]=(UI4)opctx.nimpandcols; rv[3]=opctx.ndotprods;
 if(unlikely(opctx.sharedmin.I==IMIN)){rv[0]=4.; rv[4]=0.;}  // if unbounded (gradient) or too-low SPR (onecol), give rc 4
 else if(isgradmode){
  // gradient mode.  We were minimizing 1/gradient^2; convert to gradient
  rv[0]=0.; rv[4]=sqrt(1/opctx.sharedmin.D);  // else give ok return w/gradient
 }else{
  // onecol+SPR.  classify as normal/dangerous/nonimproving/nonexistent/unbounded (=insufficient SPR, which sets SPR=-inf)
  D spr=rv[4]=opctx.sharedmin.D;  // return SPR as informational data
  // spr=0 means nonimproving dangerous pivot I. e. no pivot), spr=-1 means nonimproving nondangerous
  if(unlikely(opctx.sharedmin.I==0)){spr=inf; rv[1]=-1.;}  // detect nonimproving pivot but dangerous; other nonimproving pivots have valid rowx
  if(unlikely(opctx.sharedmin.I==-1)){spr=rv[4]=0;}  // detect nonimproving nondangerous pivot; give SPR of 0; row is valid
  D colval;  // the |value| in the column, provided exceptions do not apply
  if(BETWEENO(retinfo,0,ninclfk)){  // normal nonswap SPR found
   colval=ABS(zv[retinfo]);  // the value in the SPR row of the column.  If neg, we will swap before pivot, so use |value|
  }else{
   colval=inf;  // if the row# indicates a swap, use a harmless column value
  }
  rv[0]=unlikely(spr==inf)?3:unlikely(colval<parms[5])?3:unlikely(colval<parms[6])?1:spr==0?2:0;  // SPR=0 means nonimproving; col<minpivot means dangerous
 }
 EPILOG(r);
#else
 ASSERT(0,EVNONCE);  // this requires fast gather to make any sense
#endif
}

#if C_AVX2
// everything we need for one core's execution
struct __attribute__((aligned(CACHELINESIZE))) ekctx {
 I rowsperthread;  // number of rows of prx for each thread to handle
 // the rest is moved into static names
 // arguments
 A qk;
 A prx;
 A pcx;
 A pivotcolnon0;
 A newrownon0;
 A absfuzzmplr;
} ;
// the processing loop for one core.  We take groups of rows, in order
static unsigned char jtekupdatex(J jt,struct ekctx* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(rowsperthread)YC(prx)YC(qk)YC(pcx)YC(pivotcolnon0)YC(newrownon0)YC(absfuzzmplr)
#undef YC

 D *mplrd=DAV(absfuzzmplr);  // pointer to absfuzz/mplr data
 __m256d pcoldh, pcoldl=_mm256_setzero_pd();  // value from pivotcolnon0, multiplying one row
 __m256d prowdh, prowdl=_mm256_setzero_pd();  // values from newrownon0
 __m256d mabsfuzz=_mm256_set1_pd(*mplrd);  // comparison tolerance (if given)
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
 I dpflag=0;  // qp precision flags: 1=Qk 2=pivotcolnon0 4=newrownon0 8=mplr exists 16=both mpcands dp, no mplr
 D *qkv=DAV(qk); I qksize=AS(qk)[AR(qk)-1]; I t=AS(qk)[AR(qk)-2]; t=AR(prx)==0?1:t; I qksizesq=qksize*t; dpflag|=AR(qk)>AR(prx)+1;  // pointer to qk data, length of a row, offset to low part if present.  offset is */_2{.$Qk, or {:$bk
 UI rowx=ti*rowsperthread, rown=AN(prx), slicen=rown; slicen=rown<rowx+rowsperthread?slicen:rowx+rowsperthread;   // current row, ending row+1 taken for the current task#
 I *rowxv=IAV(prx); D *pcn0v=DAV(pivotcolnon0); dpflag|=(AR(pivotcolnon0)>1)<<1;  // address of row indexes, column data
 UI coln=AN(pcx); I *colxv=IAV(pcx); D *prn0v=DAV(newrownon0); dpflag|=(AR(newrownon0)>1)<<2;  // # cols, address of col indexes. row data
 dpflag|=AR(absfuzzmplr)<<3;  // set flag if mplr is given
 dpflag|=((dpflag&0b1110)==0)<<4;  // set 'single-precision multiply' flag if that is enough
 // for each row
 for(;rowx<slicen;++rowx){
  // get the address of this row in Qk
  D *qkvrow=qkv+rowxv[rowx]*qksize;   // the row of Qk being modified
  // fetch the pivotcol value into all lanes
  pcoldh=_mm256_set1_pd(pcn0v[rowx]);  // fetch high part of pivotcol value
  if(dpflag&2)pcoldl=_mm256_set1_pd(pcn0v[rown+rowx]);  // fetch low part if it is present
  UI colx; I okwds=NPAR; I okmsk=(1LL<<NPAR)-1;  //  number/mask of valid wds in block
  // for each column-group
  __m256d endmask=sgnbit;  // mask for maskload and gather, indicating # words to process.  Starts all valid, reset for last batch or for any mplr
  for(colx=0;colx<coln;colx+=okwds){
   __m256i prn0x;  // indexes of nonzero values in row
   // get the mask of valid values, fetch pivotrow values, fetch the Qk indexes to modify
   if(coln-colx>=NPAR){  // all lanes valid
    prn0x=_mm256_loadu_si256((__m256i_u*)(colxv+colx));  // load the indexes into Qk
    prowdh=_mm256_loadu_pd(prn0v+colx);  // load next 4 non0 values in pivotrow
    if(likely(dpflag&4))prowdl=_mm256_loadu_pd(prn0v+coln+colx);  // and low part if present
// obsolete     if(dpflag&8)mabsfuzz=_mm256_loadu_pd(mplrd+colx);
   }else{
    endmask=_mm256_loadu_pd((double*)(validitymask+NPAR-(coln-colx)));  // mask of valid lanes
    okmsk=_mm256_movemask_pd(endmask);  // mask of valid words in this block - always at least 1
    prn0x=_mm256_maskload_epi64(colxv+colx,_mm256_castpd_si256(endmask));  // load the indexes into Qk
    prowdh=_mm256_maskload_pd(prn0v+colx,_mm256_castpd_si256(endmask));  // load next 4 non0 values in pivotrow
    if(likely(dpflag&4))prowdl=_mm256_maskload_pd(prn0v+coln+colx,_mm256_castpd_si256(endmask));  // and low part if present
   }
   // mplr is given, we are modifying ck/Rk, which have only one row but may repeat column #s.  We have to
   // make sure that we don't work on the same column# in one batch of NPAR, because one update would be lost.
   // We compare the indexes against each other and set endmask so as to stop before a repetition of index
   if(unlikely(dpflag&8)){
    // compare 0-1 and 2-3
    __m256i cmp01012323=_mm256_cmpeq_epi64(prn0x,_mm256_shuffle_epi32(prn0x,0b01001110));  // 0=1 0=1 2=3 2=3
    // if 0=1, clear 1 2 3; if 2=3, clear 3
    endmask=_mm256_castsi256_pd(_mm256_andnot_si256(_mm256_or_si256(_mm256_permute2x128_si256(cmp01012323,cmp01012323,0b00001000),_mm256_blend_epi32(_mm256_setzero_si256(),cmp01012323,0b11001100)),_mm256_castpd_si256(endmask)));
    // compare 0-2 and 1-3
    __m256i cmpxxxx0213=_mm256_cmpeq_epi64(prn0x,_mm256_permute4x64_epi64(prn0x,0b01000000));  // x x 0=2 1=3
    // if 0=2, clear 2 3; if 1=3, clear 3
    endmask=_mm256_castsi256_pd(_mm256_andnot_si256(_mm256_blend_epi32(_mm256_setzero_si256(),_mm256_or_si256(_mm256_shuffle_epi32(cmpxxxx0213,0b01000100),cmpxxxx0213),0b11110000),_mm256_castpd_si256(endmask)));
    // compare 0-3 and 1-2
    __m256i cmpxxxx0312=_mm256_cmpeq_epi64(prn0x,_mm256_permute4x64_epi64(prn0x,0b00010000));  // x x 0=3 1=2
    // if 1=2, clear 2 3; if 0=3 clear 3
    endmask=_mm256_castsi256_pd(_mm256_andnot_si256(_mm256_blend_epi32(_mm256_setzero_si256(),_mm256_or_si256(_mm256_shuffle_epi32(cmpxxxx0312,0b01000100),cmpxxxx0312),0b11110000),_mm256_castpd_si256(endmask)));
    mabsfuzz=_mm256_maskload_pd(mplrd+colx,_mm256_castpd_si256(endmask));  // load next 4 non0 values in pivotrow
    okmsk=_mm256_movemask_pd(endmask);  // mask of valid words in this block - always at least 1
    okwds=(0b100000000110010010>>okmsk)&7;  // Advance to next nonrepeated column.  valid values are 1 3 7 15 for which we want results 1 2 3 4
   }
   // gather the high parts of Qk
   __m256d qkvh=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),qkvrow,prn0x,endmask,SZI);
   // create max(abs(qkvh),abs(pcoldh*prowdh)) which will go into threshold calc
   __m256d maxabs=_mm256_max_pd(_mm256_andnot_pd(sgnbit,qkvh),_mm256_andnot_pd(sgnbit,_mm256_mul_pd(pcoldh,prowdh)));
   if(unlikely(!(dpflag&1))){
    // single-precision calculation
    // if mplr given, multiply prow by it
    if(dpflag&8)prowdh=_mm256_mul_pd(prowdh,mabsfuzz);
    // calculate old - pcol*prow
    qkvh=_mm256_fnmadd_pd(prowdh,pcoldh,qkvh);
    if(!(dpflag&8)){
     // thresholding - convert maxabs to abs(qkvh) > thresh: if 0, means result should be forced to 0
     maxabs=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),mabsfuzz,_CMP_GT_OQ);   // maxabs = 0 if result too small
     qkvh=_mm256_and_pd(qkvh,maxabs); // zero if lower than fuzz (high part)
    }
    // scatter the results (high part)
//   _mm256_mask_i64scatter_pd(qkvrow,endmask,prn0x,qkvh,SZI);
    qkvrow[_mm256_extract_epi64(prn0x,0)]=_mm256_cvtsd_f64(qkvh);
    if(likely(okmsk&0b1000))goto storeh1111; if(okmsk&0b100)goto storeh111; if(okmsk&0b10)goto storeh11; goto storeh1;
    storeh1111: qkvrow[_mm256_extract_epi64(prn0x,3)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvh,0b11100111));
    storeh111: qkvrow[_mm256_extract_epi64(prn0x,2)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvh,0b11100110));
    storeh11: qkvrow[_mm256_extract_epi64(prn0x,1)]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvh,0b0001));
    storeh1: ;
   }else{
    // extended-precision calculation
    __m256d iph,ipl,isl;  // intermediate products and sums
    __m256d qkvl;  // low-order part of result

    // if mplr given, multiply prow by it
    if(dpflag&8){
     // mplr is dp, prow is qp.
     TWOPRODQD(prowdh,prowdl,mabsfuzz,iph,ipl) prowdh=iph; prowdl=ipl;
    }
    // (iph,ipl) = - prowdh*pcoldh    scaf we could skip the extended calc if both mpcands are dp, as they are for swaps
    TWOPROD(prowdh,pcoldh,iph,ipl)  // (prowdh,pcoldh) to high precision
    if(!(dpflag&16)){ipl=_mm256_fmadd_pd(prowdh,pcoldl,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh,ipl);}  // accumulate middle pps - can skip for b0 when both mpcands are dp
    iph=_mm256_xor_pd(sgnbit,iph); ipl=_mm256_xor_pd(sgnbit,ipl);  // change sign for subtract
    // Because we added 3 low-order values (with the same shift) - 4 if mplr used - , we are limiting precision to 104 bits
    if(_mm256_testz_si256(_mm256_castpd_si256(qkvh),_mm256_castpd_si256(qkvh))){  // all 0?  our numbers can never be -0 since they come out of addition
     // qkvh is all 0 - the result is simply (-iph,-ipl) 
     TWOSUMBS(iph,ipl,qkvh,qkvl)  // canonical form
    }else{
     // normal case where qkvh not all 0
     qkvl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),qkvrow+qksizesq,prn0x,endmask,SZI);  // gather the low parts of Qk
     // (qkvh,qkvl) - (prowdh,prowdl) * (pcoldh,pcoldl)
     // Do high-precision add of qkvh and iph.  If this decreases the absvalue of qkvh, we will lose precision because of insufficient
     // bits of qkv.  If this increases the absvalue of qkvh, all of qkvl will contribute and the limit of validity will be
     // from the product.  In either case it is safe to accumulate all the partial products and ipl into qkvl
     qkvl=_mm256_add_pd(qkvl,ipl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
     TWOSUM(qkvh,iph,qkvh,isl)   // combine the high parts
     isl=_mm256_add_pd(isl,qkvl);  // add the combined low parts
     // Make sure qkvl is much less than qkvh
     TWOSUM(qkvh,isl,qkvh,qkvl)  // put qkvh into canonical form
     if(!(dpflag&8)){
      // thresholding - convert maxabs to abs(qkvh) - maxabs*thresh: if < 0, means result should be forced to 0
      maxabs=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,qkvh),mabsfuzz,_CMP_GT_OQ);   // maxabs = 0 if result too small
      qkvl=_mm256_and_pd(qkvl,maxabs); // zero if lower than fuzz (low part)
      qkvh=_mm256_and_pd(qkvh,maxabs); // zero if lower than fuzz (high part)
     }
    }
    // scatter the results (both parts)
//   _mm256_mask_i64scatter_pd(qkvrow,endmask,prn0x,qkvh,SZI);
    qkvrow[_mm256_extract_epi64(prn0x,0)]=_mm256_cvtsd_f64(qkvh);
    qkvrow[_mm256_extract_epi64(prn0x,0)+qksizesq]=_mm256_cvtsd_f64(qkvl);
    if(likely(okmsk&0b1000))goto storeq1111; if(okmsk&0b100)goto storeq111; if(okmsk&0b10)goto storeq11; goto storeq1;
    storeq1111: qkvrow[_mm256_extract_epi64(prn0x,3)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvh,0b11100111));
    qkvrow[_mm256_extract_epi64(prn0x,3)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute4x64_pd(qkvl,0b11100111));
    storeq111: qkvrow[_mm256_extract_epi64(prn0x,2)]=_mm256_cvtsd_f64(_mm256_permute4x64_pd (qkvh,0b11100110));
    qkvrow[_mm256_extract_epi64(prn0x,2)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute4x64_pd (qkvl,0b11100110));
    storeq11: qkvrow[_mm256_extract_epi64(prn0x,1)]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvh,0b0001));
    qkvrow[_mm256_extract_epi64(prn0x,1)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvl,0b0001));
    storeq1: ;
   }

  }
 }
 R 0;
}

// 128!:12  calculate
// Qk/bk=: (((<prx;pcx) { Qk) ((~:!.absfuzz) * -) pivotcolnon0 */ newrownon0 [* mplr]) (<prx;pcx)} Qk/bk
// with high precision
// a is prx;pcx;pivotcolnon0;newrownon0;absfuzz/mplr (mplr if not atom)
// w is Qk or bk.  If bk, prx must be scalar 0
// If Qk has rank > rank newrownon0 + rank prx, or pivotcolnon0/newrownon0 rank 2, calculate them in extended FP precision
// Qk/bk is modified in place
F2(jtekupdate){F2PREFIP;
 ARGCHK2(a,w);
 // extract the inputs
 A qk=w; ASSERT(AT(w)&FL,EVDOMAIN) ASSERT(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w),EVNONCE)
 ASSERT(AT(a)&BOX,EVDOMAIN) ASSERT(AR(a)==1,EVRANK) ASSERT(AN(a)==5,EVLENGTH)  // a is 5 boxes
 A box0=C(AAV(a)[0]), box1=C(AAV(a)[1]), box2=C(AAV(a)[2]), box3=C(AAV(a)[3]), box4=C(AAV(a)[4]);
 A prx=box0; ASSERT(AT(prx)&INT,EVDOMAIN) ASSERT(AR(prx)<=1,EVRANK)  // prx is integer list or atom
 A pcx=box1; ASSERT(AT(pcx)&INT,EVDOMAIN) ASSERT(AR(pcx)==1,EVRANK)  // pcx is integer list
 A pivotcolnon0=box2; ASSERT(AT(pivotcolnon0)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(pivotcolnon0),1,2),EVRANK)
 ASSERT(AR(pivotcolnon0)==1||AS(pivotcolnon0)[0]==2, EVLENGTH)  // pivotcolnon0 is float or extended list
 A newrownon0=box3; ASSERT(AT(newrownon0)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(newrownon0),1,2),EVRANK)
 ASSERT(AR(newrownon0)==1||AS(newrownon0)[0]==2,EVLENGTH)  // newrownon0 is float or extended list
 A absfuzzmplr=box4; ASSERT(AT(absfuzzmplr)&FL,EVDOMAIN) ASSERT(AR(absfuzzmplr)<=1,EVRANK)  // absfuzz is a float atom; mplr is a float list
 if(AR(absfuzzmplr)==1)ASSERT(AN(absfuzzmplr)==AN(pcx),EVLENGTH)  // if mplr, must be one per input in row
 // agreement
 ASSERT(BETWEENC(AR(w),AR(prx)+1,AR(prx)+2),EVRANK)  // Qk is nxn; bk is n, treated as a single row.  Each may be quadprec
 ASSERT(AR(w)==AR(prx)+1||AS(w)[0]==2,EVLENGTH)
 if(AR(prx)!=0){ASSERT(AS(w)[AR(w)-1]>=AS(w)[AR(w)-2]-1,EVLENGTH) DO(AN(prx), ASSERT(IAV(prx)[i]<AS(w)[AR(w)-2],EVINDEX))} else{ASSERT(IAV(prx)[0]==0,EVINDEX)}  // Qk/bk rows may be padded (Qk might include Fk); valid row indexes
 ASSERT(AN(prx)==AS(pivotcolnon0)[AR(pivotcolnon0)-1],EVLENGTH) ASSERT(AN(pcx)==AS(newrownon0)[AR(newrownon0)-1],EVLENGTH)   // indexes and values must agree
 // audit the indexes
 DO(AN(pcx), ASSERT(IAV(pcx)[i]<AS(w)[AR(w)-1],EVINDEX))  // verify valid column indexes
 // do the work
 
 // figure out how many threads to use, how many lines to take in each one
#define TASKMINATOMS ((2*2000)/6)  // TUNE a cached atom takes 5 clocks to compute; an uncached one takes 20 (2022 Alder Lake).  We don't want to start a task with less than 2000 clocks, so insist on twice that many
 I m=AN(prx), n=AN(pcx);  // # rows & columns to modify
 I nthreads=(*JT(jt,jobqueue))[0].nthreads+1;  // the number of threads we would like to use (including master), init to # available
 I rowsperthread=m;  // will be #rows each processor should take
 if(((1-m)&(1-nthreads)&(TASKMINATOMS-m*n))>=0)nthreads=1;  // if only one thread, or job too small, use just one thread
 else{
  // big enough for multithreading.
  rowsperthread=(m+nthreads-1)/nthreads;  // number of threads per processor, rounded up
 }
#undef TASKMINATOMS
#define YC(n) .n=n,
struct ekctx opctx={YC(rowsperthread)YC(prx)YC(qk)YC(pcx)YC(pivotcolnon0)YC(newrownon0)YC(absfuzzmplr)};
#undef YC

 if(nthreads>1)jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtekupdatex,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 else jtekupdatex(jt,&opctx,0);  // the one-thread case is not uncommon and we avoid thread overhead then

 R qk;
}
#else
F2(jtekupdate){ASSERT(0,EVNONCE);}
#endif

#if C_AVX2
// everything we need for one core's execution
struct __attribute__((aligned(CACHELINESIZE))) sprctx {
 I rowsperthread;  // number of rows of prx for each thread to handle
 // the rest is moved into static names
 // arguments
 A ck;
 A bk;
 I m;   // length of a column
 D ckthreshd;
 D bkthreshd;
 // result
 I row;  // the row number we found
} ;
// the processing loop for one core.  We take groups of rows, in order
static unsigned char jtfindsprx(J jt,struct sprctx* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(rowsperthread)YC(ck)YC(bk)YC(m)YC(ckthreshd)YC(bkthreshd)
#undef YC

 // establish pointers, offsets, and sizes.  We keep pointers to the beginning of the block and offset into the region we're working on
 D *ck0=DAV(ck);  // pointer to base of ck area
 D *bk0=DAV(bk);  // pointer to base of bk area
 I i0=rowsperthread*ti;  // starting offset of our block
 I m0=m-i0; m0=m0>rowsperthread?rowsperthread:m0;  // number of atoms in our area, MIN(rowsperthread,length of bk/ck)
 I i0e=m0+i0;  // end+1 offset for our block

 // find the best SPR, dp only

 // init for the column
 __m256i endmask=_mm256_setone_epi64(); // mask for validity of next 4 words: used to control fetch from column and store into result row
 __m256d minspr=_mm256_set1_pd(inf); // min spr found per lane
 __m256d ckthresh=_mm256_set1_pd(ckthreshd); // column threshold
 __m256d bkthresh=_mm256_set1_pd(bkthreshd); // column threshold
 I i;   // index to the block-of-4 we are working on
 // create the column NPAR values at a time
 for(i=i0;i<i0e;i+=NPAR){
  // get the validity mask and process: leave as ones until the end of the column
  __m256d ck4, bk4;  // values we are working on
  if(likely(i0e-i>=NPAR)){
   ck4=_mm256_loadu_pd(&ck0[i]); bk4=_mm256_loadu_pd(&bk0[i]);  // read in ck and bk
  }else{
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-(i0e-i)));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
   ck4=_mm256_maskload_pd(&ck0[i],endmask); bk4=_mm256_maskload_pd(&bk0[i],endmask);  // read in ck and bk
  }
  if(!_mm256_testz_si256(_mm256_castpd_si256(ck4),_mm256_castpd_si256(ck4))){  // if all c values are 0.0, we can skip everything - not unlikely
   // see if any of the new values is limiting: b/c < min SPR, c>ColThresh, bk>bkthresh
   // we perform this calculation on each lane separately, to save the expense of finding the minimum SPR each time one is calculated.
   // This means the values in each lane will be smaller than they would be with sharing, leading to perhaps 2 extra divisions.  That's a good trade.
   __m256d ratios=_mm256_fmsub_pd(minspr,ck4,bk4);  // minspr*c-b: sign set if this is NOT a new min SPR in its lane
   __m256d coverthresh=_mm256_cmp_pd(ck4,ckthresh,_CMP_GT_OQ);  // set to ~0 if c>ColThresh - never if endmask 0
   // we separate b into < threshold and >= threshold.   This must match the selection made for stall exit
   __m256d sprvalid=_mm256_and_pd(coverthresh,_mm256_cmp_pd(bk4,bkthresh,_CMP_GE_OQ));  // compare sets to ~0 if b>=bthresh - never if endmask is 0   set to ~0 if c>thresh & b>=thresh, i. e. SPR is valid
   if(!_mm256_testc_pd(ratios,sprvalid)){  // CF is 1 if ~a&b signs all 0 => ~(~new min)&(c good&b good) all 0 => no place where new min & c good & b good
    // at least one new SPR is limiting (in its lane).  Find the SPRs
    __m256d sprs=_mm256_div_pd(bk4,ck4);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch
    // the reciprocal takes a long time to run.  Make sure there are no dependencies.
    sprvalid=_mm256_andnot_pd(ratios,sprvalid);  // sprvalid now means 'valid and limiting' - high bit only
    minspr=_mm256_blendv_pd(minspr,sprs,sprvalid);   // remember the limiting SPR, in each lane
   }
  }
 }  // end of loop to find minspr per lane

 // find the smallest SPR, and its row#, of the ones in minspr.  In case of ties take the LAST
 __m256d allmin=_mm256_min_pd(minspr,_mm256_permute_pd(minspr,0b0101)); // allmin = min01 min01 min23 min23  could use integer in AVX-512
 allmin=_mm256_min_pd(allmin,_mm256_permute4x64_pd(allmin,0b00001010));  // allmin=min value in all lanes
 // now allmin has the min SPR in all lanes

 // We know that the overall column has a pivot, but if we are a thread it is possible that we have no pivot.  Exit now in that case.
 if(_mm256_cvtsd_f64(allmin)==inf)R 0;   // nothing more to do uif no SPRs

 // get the list of row#s that are in contention for best pivot
 // In QP, the true SPR may be up to 1.5 DP ULP greater than calculated value.  Go back and get the list of row#s that overlap with that
 // create the column NPAR values at a time
 allmin=_mm256_mul_pd(allmin,_mm256_set1_pd(1.0000000000000003));  // add 2 ULPs in DP
 A conten; GA0(conten,INT,m,0) I *conten0=IAV(conten), conteni=0;  // get address of place to store contenders
 for(i=i0;i<i0e;i+=NPAR){
  // get the validity mask and process: leave as ones until the end of the column
  __m256d ck4, bk4;  // values we are working on
  if(likely(i0e-i>=NPAR)){
   ck4=_mm256_loadu_pd(&ck0[i]); bk4=_mm256_loadu_pd(&bk0[i]);  // read in ck and bk
  }else{
   ck4=_mm256_maskload_pd(&ck0[i],endmask); bk4=_mm256_maskload_pd(&bk0[i],endmask);  // read in ck and bk.  endmask is set from earlier loop
  }
  if(!_mm256_testz_si256(_mm256_castpd_si256(ck4),_mm256_castpd_si256(ck4))){  // if all c values are 0.0, we can skip everything - not unlikely
   // see if any of the new values is limiting: b/c < min SPR, c>ColThresh, bk>bkthresh
   // we perform this calculation on each lane separately, to save the expense of finding the minimum SPR each time one is calculated.
   // This means the values in each lane will be smaller than they would be with sharing, leading to perhaps 2 extra divisions.  That's a good trade.
   __m256d ratios=_mm256_fmsub_pd(allmin,ck4,bk4);  // allmin*c-b: sign set if this is NOT a new min SPR in its lane
   __m256d coverthresh=_mm256_cmp_pd(ck4,ckthresh,_CMP_GT_OQ);  // set to ~0 if c>ColThresh - never if endmask 0
   // we separate b into < threshold and >= threshold.   This must match the selection made for stall exit
   __m256d sprvalid=_mm256_and_pd(coverthresh,_mm256_cmp_pd(bk4,bkthresh,_CMP_GE_OQ));  // compare sets to ~0 if b>=bthresh - never if endmask is 0   set to ~0 if c>thresh & b>=thresh, i. e. SPR is valid
   if(!_mm256_testc_pd(ratios,sprvalid)){  // CF is 1 if ~a&b signs all 0 => ~(~new min)&(c good&b good) all 0 => no place where new min & c good & b good
    // at least one new SPR has overlap.  Remember the numbers
    I contenmsk=_mm256_movemask_pd(_mm256_andnot_pd(ratios,sprvalid));  // bitmask indicating overlap positions
    do{conten0[conteni++]=i+CTTZI(contenmsk); contenmsk&=contenmsk-1;}while(contenmsk);  // copy the indexes of the set bits one by one
   }
  }
 }  // end of loop to find contenders

 I zrow;  // the row number containing the minimum SPR - our result
 if(likely(((1-conteni)&(1-AR(ck)))>=0))zrow=conten0[0];  // usually there is 1 contender.  And if we don't have QP, we'll just take a good one (might not be best)
 else{
  // there are contenders, and we must pick the best, by QP cross-multiplies
  // cull those to best row in each lane, in QP, and then to a single lane
  __m256d ckbh=_mm256_setzero_pd(), ckbl=ckbh, bkbh=_mm256_set1_pd(1.0), bkbl=bkbh;  // current 'best' values in each lane - init to b/c=infinity
  __m256d ck4h, ck4l, bk4h, bk4l;  // new values to try
  __m256i rownums;  // row numbers we are fetching from in the current iteration 
  __m256d limitrows=_mm256_setzero_pd();  // row numbers of lowest SPRs.  Init not needed 
#define keepbest { /* ckb* and ck4* are loaded.  In each lane, leave the smaller SPR in ckb* and limitrows */ \
 /* we see if b4/c4 < bb/cb, where if b4=c4=0 it must NOT compare lower, ever.  Take b4*cb < c4*bb */ \
 __m256d b4cbh, b4cbl, c4bbh, c4bbl, t0, t1;  /* quad-precision cross products */ \
 TWOPROD(bk4h,ckbh,t0,t1)  /* b4 * cb high parts */ \
 t1=_mm256_fmadd_pd(bk4h,ckbl,t1);  /* bh * cl into low part */ \
 t1=_mm256_fmadd_pd(bk4l,ckbh,t1);  /* bl * ch into low part */ \
 TWOSUMBS(t0,t1,b4cbh,b4cbl)  /* convert to canonical form.  We ignore low*low, which costs us 1 bit */ \
 TWOPROD(ck4h,bkbh,t0,t1)  /* c4 * bb high parts */ \
 t1=_mm256_fmadd_pd(ck4h,bkbl,t1);  /* ch * bl into low part */ \
 t1=_mm256_fmadd_pd(ck4l,bkbh,t1);  /* cl * bh into low part */ \
 TWOSUMBS(t0,t1,c4bbh,c4bbl)  /* convert to canonical form.  We ignore low*low, which costs us 1 bit */ \
 /* set t0 to 1s if b4*cb < c4*bb */ \
 /* we rely on consistent rounding: it must never happen that qp a>b and high(a)<high(b) */ \
 t0=_mm256_or_pd(_mm256_cmp_pd(b4cbh,c4bbh,_CMP_LT_OQ),_mm256_and_pd(_mm256_cmp_pd(b4cbh,c4bbh,_CMP_EQ_OQ),_mm256_cmp_pd(b4cbl,c4bbl,_CMP_LT_OQ))); /* < hi, or = hi < lo */ \
 ckbh=_mm256_blendv_pd(ckbh,ck4h,t0); ckbl=_mm256_blendv_pd(ckbl,ck4l,t0); bkbh=_mm256_blendv_pd(bkbh,bk4h,t0); bkbl=_mm256_blendv_pd(bkbl,bk4l,t0);   /* keep the winner */ \
 limitrows=_mm256_blendv_pd(limitrows,_mm256_castsi256_pd(rownums),t0);  /* also remember the row */ \
}

  endmask=_mm256_setone_epi64(); // mask for validity of next 4 words: used to control fetch from indexes and column
  for(i=0;i<conteni+LGNPAR*NPAR;i+=NPAR){  // we go through the loop for all inputs, then twice more to combine lanes
   if(i<conteni){  // the first loops read new values; the last combine lanes
    // get the validity mask and process: leave as ones until the end of the column
    if(likely(conteni-i>=NPAR)){
     rownums=_mm256_loadu_si256((__m256i*)&conten0[i]);
    }else{
     endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-(conteni-i)));
     rownums=_mm256_maskload_epi64(&conten0[i],endmask);
    }
    ck4h=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),ck0,rownums,_mm256_castsi256_pd(endmask),SZI);
    ck4l=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),ck0+m,rownums,_mm256_castsi256_pd(endmask),SZI);
    bk4h=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),bk0,rownums,_mm256_castsi256_pd(endmask),SZI);
    bk4l=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),bk0+m,rownums,_mm256_castsi256_pd(endmask),SZI);
   }else if(i<conteni+NPAR){  // next-last iteration
     ck4h=_mm256_permute_pd(ckbh,0b0101); ck4l=_mm256_permute_pd(ckbl,0b0101); bk4h=_mm256_permute_pd(bkbh,0b0101); bk4l=_mm256_permute_pd(bkbl,0b0101);  rownums=_mm256_castpd_si256(_mm256_permute_pd(limitrows,0b0101));  // compare within lanes
   }else{  // last iteration
     ck4h=_mm256_permute4x64_pd(ckbh,0b01001110); ck4l=_mm256_permute4x64_pd(ckbl,0b01001110); bk4h=_mm256_permute4x64_pd(bkbh,0b01001110); bk4l=_mm256_permute4x64_pd(bkbl,0b01001110);  rownums=_mm256_castpd_si256(_mm256_permute4x64_pd(limitrows,0b01001110));  // compare across lanes
   }
   keepbest
  }  // end of loop to pick best contender

  zrow=_mm256_extract_epi64(_mm256_castpd_si256(limitrows),0);  // remember the winning row

 }  // end 'there are contenders'
 // compare our best row against the incumbent in the ctx, and update the incumbent if the new value is an improvement
 while(1){  // till we succeed in writing or we realize we don't have the best
  I incumbent=__atomic_load_n(&ctx->row,__ATOMIC_ACQUIRE);  // get the current best row
  if(incumbent>=0){  // if there is no incumbent, we know we beat it
   __m256d ckbh=_mm256_set1_pd(ck0[incumbent]), ckbl=_mm256_set1_pd(ck0[m+incumbent]), bkbh=_mm256_set1_pd(bk0[incumbent]), bkbl=_mm256_set1_pd(bk0[m+incumbent]);  // incumbent 'best' values in each lane
   __m256d ck4h=_mm256_set1_pd(ck0[zrow]), ck4l=_mm256_set1_pd(ck0[m+zrow]), bk4h=_mm256_set1_pd(bk0[zrow]), bk4l=_mm256_set1_pd(bk0[m+zrow]);  // new values to try
   __m256d limitrows=_mm256_castsi256_pd(_mm256_set1_epi64x(incumbent));  // row numbers of lowest SPRs to date
   __m256i rownums=_mm256_set1_epi64x(zrow);  // row numbers of new SPR
   keepbest  // see if we keep our value
   if(zrow!=_mm256_extract_epi64(_mm256_castpd_si256(limitrows),0))break;  // if we are not the best, exit loop, we're done
  }
  // we have the best.  Try to install it in row
  if(__atomic_compare_exchange_n(&ctx->row,&incumbent,zrow,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))break;  // write; if written exit loop, otherwise reload incumbent
 }
 R 0;
}


// 128!:13  find minimum pivot ratio in quad precision  OR  take reciprocal
// w is ck;bk;ColThresh,bkthresh  or   ck value
// result is row#,reciprocal of pivot hi,lo
F1(jtfindspr){F1PREFIP;
 ARGCHK1(w);
 I row;  // will hold the row of the SPR
 D *ckd;  // pointer to column data
 I isqp;  // will be set if there is a qp value
 I m;  // number of items in ck
 // extract the inputs
 if(AT(w)&FL){
  // reciprocal-only case
  ASSERT(AR(w)<=1,EVRANK) ASSERT(AR(w)==0||AS(w)[0]==2,EVLENGTH)  // must be atom or 2-atom list
  isqp=AR(w); ckd=DAV(w);
  A z; GAT0(z,FL,2,1)
  __m256d pivhi=_mm256_set1_pd(ckd[0]), reciphi=_mm256_set1_pd(1.0/ckd[0]);
  if(isqp){
   __m256d pivlo=_mm256_set1_pd(ckd[1]), reciplo=_mm256_set1_pd(0.0), t0, t1, prodh, prodl;
   DO(2,  TWOPROD(reciphi,pivhi,t0,t1) t1=_mm256_fmadd_pd(reciplo,pivhi,t1); t1=_mm256_fmadd_pd(reciphi,pivlo,t1); TWOSUMBS(t0,t1,prodh,prodl)   // qp piv * recip - will be very close to 1.0
          t0=_mm256_sub_pd(_mm256_sub_pd(_mm256_set1_pd(1.0),prodh),prodl);  // - (amount the product is too high).  1-prodh gives exact result, commensurate with prodl
          t0=_mm256_fmadd_pd(reciphi,t0,reciplo); t1=reciphi; TWOSUMBS(t1,t0,reciphi,reciplo)  // -(toohigh)*recip(hi) gives correction to recip: add it in
   )
   DAV(z)[1]=_mm256_cvtsd_f64(reciplo);  // lower recip
  }else DAV(z)[1]=0.0;
  DAV(z)[0]=_mm256_cvtsd_f64(reciphi);  // always write upper/only recip
  RETF(z);  // return qp reciprocal
 }else{
  // search for the best SPR
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==3,EVLENGTH)  // w is 3 boxes
  A box0=C(AAV(w)[0]), box1=C(AAV(w)[1]), box2=C(AAV(w)[2]);
  A ck=box0; ASSERT(AT(ck)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(ck),1,2),EVRANK) ASSERT(AR(ck)==1||AS(ck)[0]==2,EVLENGTH)  // ck is FL list or 2xn table
  isqp=!(AR(ck)==1); ckd=DAV(ck);  // remember if qp, and address of the column data
  A bk=box1; ASSERT(AT(bk)&FL,EVDOMAIN) ASSERT(AR(bk)==AR(ck),EVRANK) ASSERT(AS(bk)[0]==AS(ck)[0],EVLENGTH) ASSERT(AR(bk)==1||AS(bk)[1]==AS(ck)[1],EVLENGTH)  // bk is float list or 2xn table, must match ck
  A thresh=box2; ASSERT(AT(thresh)&FL,EVDOMAIN) ASSERT(AR(thresh)==1,EVRANK) ASSERT(AN(thresh)==2,EVLENGTH)  // thresh is 2 floats

  // do the work
  m=AS(ck)[AR(ck)-1];   // length of a column
  // figure out how many threads to use, how many lines to take in each one
#define TASKMINATOMS ((2*2000)/2)  // TUNE Values will be in cache.  Normal DP comp is 2 clocks per atom.  We don't want to start a task with less than 2000 clocks, so insist on twice that many
  I nthreads=(*JT(jt,jobqueue))[0].nthreads+1;  // the number of threads we would like to use (including master), init to # available
  I rowsperthread=m;  // will be #rows each processor should take
  if(((1-nthreads)&(TASKMINATOMS-rowsperthread))>=0)nthreads=1;  // if only one thread, or job too small, use just one thread
  else{
   // big enough for multithreading.
   rowsperthread=(m+nthreads-1)/nthreads;  // number of threads per processor, rounded up
  }
#undef TASKMINATOMS

  // init the context
#define YC(n) .n=n,
struct sprctx opctx={YC(rowsperthread)YC(ck)YC(bk)YC(m).ckthreshd=DAV(thresh)[0],.bkthreshd=DAV(thresh)[1]};
#undef YC

  // run the operation
  opctx.row=-1;  // init row# to invalid.  Improving threads will update it
  if(nthreads>1)jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtfindsprx,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
  else jtfindsprx(jt,&opctx,0);  // the one-thread case is not uncommon and we avoid thread overhead then

  row=opctx.row;
  // if no SPR was found, signal error.  This is possible only if the only pivots were near-threshold in dp and below threshold in qp
  ASSERT(opctx.row>=0,EVLIMIT);
  RETF(sc(row))  // return the row#
 }
}
#else
F1(jtfindspr){ASSERT(0,EVNONCE);}
#endif
