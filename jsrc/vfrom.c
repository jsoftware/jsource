/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: From & Associates. See Hui, Some Uses of { and }, APL87.         */

#include "j.h"


DF1(jtcatalog){F12IP;PROLOG(0072);A b,*wv,x,z,*zv;C*bu,*bv,**pv;I*cv,i,j,k,m=1,n,p,*qv,r=0,*s,t=0,*u;
 F1RANK(1,jtcatalog,self);
 ASSERTF(!ISSPARSE((AT(w))),EVNONCE,"sparse arrays not supported");
 if((-AN(w)&-(AT(w)&BOX))>=0)R box(w);   // empty or unboxed, just box it
 n=AN(w); wv=AAV(w);
 DO(n, x=C(wv[i]); if(likely(AN(x))){p=AT(x); t=t?t:p; ASSERTF(!ISSPARSE(p),EVNONCE,"sparse arrays not supported"); ASSERT(HOMO(t,p),EVINHOMO); RE(t=maxtype(t,p));});  // use vector maxtype; establish type of result
 t=t?t:B01; k=bpnoun(t);  // if all empty, use boolean for result
 GA10(b,t,n);      bv=CAV(b);  // allocate place to build each item of result - one atom from each box.  bv->item 0
 GATV0(x,INT,n,1);    qv=AV1(x);   // allocate vector of max-indexes for each box - only the address is used  qv->max-index 0
 GATV0(x,BOX,n,1);    pv=(C**)AV1(x);  // allocate vector of pointers to each box's data  pv->box-data-base 0
 RZ(x=apvwr(n,0L,0L)); cv=AV(x);   // allocate vector of current indexes, init to 0  cv->current-index 0
 DO(n, x=C(wv[i]); if(TYPESNE(t,AT(x)))RZ(x=cvt(t,x)); r+=AR(x); qv[i]=p=AN(x); DPMULDE(m,p,m); pv[i]=CAV(x););  // fill in *qv and *pv; calculate r=+/ #@$@> w, m=*/ */@$@> w
 GATV0(z,BOX,m,r);    zv=AAVn(r,z); s=AS(z);   // allocate result area
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

// return a single cell of w with rank wcr, from audited cell index j.  wr is AR(w), wt is AT(w)
static INLINE A jtget1cell(J jtfg,A w,I wcr,I j,I wt,I wr){F12IP; A z;
 I *ws=AS(w); I m; PROD(m,wcr,ws+wr-wcr);  // shape of w, number of atoms in a cell
 if(m<MINVIRTSIZE){  // if cell too small for virtual, allocate & fill here
  I k=bplg(wt); GA(z,wt,m,wcr,ws+wr-wcr) JMC(CAVn(wcr,z),CAV(w)+j*(m<<k),m<<k,0);  // copy in the data, possibly overstoring up to 7 bytes.  Nonrecursive block
  // We transferred one I/A out of w.  We must mark w non-pristine.  If it was inplaceable, we can transfer the pristine status.  We overwrite w because it is no longer in use
  PRISTXFERF(z,w)  // this destroys w
 }else{
  RZ(z=virtualip(w,j*m,wcr));   // if w is rank 2, could reuse inplaceable a for this virtual block
  // fill in shape and number of atoms.  ar can be anything.
  AN(z)=m; MCISH(AS(z),ws+wr-wcr,wcr)
  // When we create a virtual block we do not actually copy anything out of w, so it remains pristine.  The virtual block is not.
 }
 R z;
}

// block used to hold axis info, leading axis first
struct __attribute__((aligned(ABDY))) faxis {
 I lenaxis;  // the length of the axes (including frame) represented by this faxis struct, in items
 I lencell;  // size of item of this axis in atoms
 I nsel;  // number of selectors.  If negative, axis is complementary; nsel has ~(# of items to move to result)
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

// rflags is w minor cell rank/len of w frame/1B rank of result/1B /6B dimension of axes-1
// if a is inplaceable in jt, ind in the last axis is the area that can be used for the result
static A jtaxisfrom(J jtfg,A w,struct faxis *axes,I rflags){F12IP;I i;
 I r=rflags&0x3f, zr=(C)(rflags>>8), wf=(C)(rflags>>16), wcr=(C)(rflags>>24), hasr=(rflags>>7)&1;  // number of axes-1; result rank; w framelen; 1 iff 1st axis is from rank
 C *base=voidAV(w);  // will be starting cell number in all axes before last
 // convert lencell to bytes & roll it up; calculate base from sel0 values
 I wt=AT(w);
 I k=bplg(wt);  // lg of size of atom
 I celllen=axes[r].lencell;  // length of cell of last axis, in atoms
 I zn=celllen;  // number of atoms in result
 A z;  // result
 I nunitsels=r;  // number of leading axes containing exactly one selector.  When the axis below this prefix rolls over, we can stop, knowing that there are no higher selectors
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
  I nsel=axes[r].nsel;  // #selectors, neg if complementary
  I lenaxis=axes[r].lenaxis;  // length of last axis
  DPMULDE(zn,nsel^REPSGN(nsel),zn);  // * last-axis size, gives result size
  if(((nunitsels-r)|(zn-MINVIRTSIZE))>=0){  // if there is only one axis left, and result is big enough
   // There is only one application of the last axis.  If the indexes are sequential, we can make the result virtual
   // We allow virtualing even for NJA blocks.  We allow for ANCHORED because it's just too useful when the user knows what he's doing. 
   // If the block can be virtual, we create the virtual here and return
   I *sels=axes[r].sels;  // pointer to selectors of last axis
   I index0;  // indexes of first item if virtual is OK
   if(nsel>=0){  // normal axis
    index0=sels[0]; index0+=REPSGN(index0)&lenaxis;  // index of first item, accounting for negative indexing
    if((UI)index0>=(UI)lenaxis)goto novirtual; if(index0+nsel>lenaxis)goto novirtual;  // if first value out of range, or too many selectors, can't be virtual
    // check the last item before checking the middle.
    DQ(nsel-1, I indexn=sels[1+i]; indexn+=REPSGN(indexn)&lenaxis; if(indexn!=index0+1+i){goto novirtual;});  // no virtual if not sequential values
    nsel=AR(axes[r].indsubx.ind)!=1?0:nsel;  // if shape changes, set flag value to suppress returning entire w unchanged
   }else{
    // complementary indexing.  See if the values are consecutive (in-full is impossible)
    // The first index we produce is .sel0, which means that must be the index of the first gap.  Find the width
    // of that gap and see if it accounts for all the indexes
    nsel=~nsel;  // convert nsel to positive length - gives number of result slots.  lenaxis-nsel is original # unique complementary indexes 
    index0=axes[r].sel0; I axn=axes[r].lenaxis; I indexn=likely(index0<lenaxis-nsel)?axes[r].sels[index0]:axn;  // start of gap, axis len, end+1 of gap (or len of axis if there is no next sel)
    if(nsel>indexn-index0)goto novirtual;  // if the first gap does not account for all the selectors, we can't make a virtual one
   }
   // nsel has been modified if shape changes
   // indexes are consecutive and in range.  Make the result virtual.  Rank of w cell must be > 0, since we have >=2 consecutive result atoms
   // if the selected length is the axis length, and there is only one axis, we are taking the entire w arg - just return it
   if(unlikely((r+(nsel^lenaxis))==0))R RETARG(w);
   I baseadj=base-CAV(w);  // movement of base needed from upper selectors.  Must calculate before virtual in case it self-virtuals and moves w
   I wr=AR(w);  // get rank of w before it is destroyed by possible self-virtualing
   RZ(z=virtualip(w,index0*celllen,zr));  // inplace w OK since no cells repeated
   // fill in shape and number of atoms, and offset the data pointer using base
   AK(z)+=baseadj;  // move offset from start of w data to the cell selected by upper selectors
   // shape is 1s for upper axes/last-axis selectors/shape of cell lof last axis
   AN(z)=zn;
    I *zs=AS(z)+zr-wcr;  // pointer into result shape, moved around as we calculate; start pointing to cellshape
   // install the axes for the cell of w: all axes except the frame and the selectors
   MCISH(zs,AS(w)+wr-wcr,wcr)  // zs->start of cell shape
   // axes for the last selector
   if(axes[r].nsel>=0){MCISH(zs-=AR(axes[r].indsubx.ind),AS(axes[r].indsubx.ind),AR(axes[r].indsubx.ind));  // normal, back up to rank and copy
   }else{*--zs=~axes[r].nsel;  // complementary, treat as list of appropriate length
   }
   // install 1s for the rest of the shape
   if(zs!=AS(z))do{*--zs=1;}while(zs!=AS(z));
   RETF(z);
novirtual:;  // abort to here if virtual not allowed because indexes are not consecutive
  }
 }else{zn=0;}  // if w empty, z must be empty too, since no nonempty selector is valid on 0-len axis
 // allocate the result, or use a inplace for the result (which must not be unincorpable or DIRECT)
 if(!((I)jtfg&JTINPLACEA)){
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
#if C_AVX2 || EMU_AVX2
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
#if C_AVX2 || EMU_AVX2
  // if gather is supported, change nl to distance between gathers
  nl<<=(celllen>>LGSZI)<<(LGLGNPAR);  // nl becomes the stride in atoms between groups of gathers, 4*length of last axis
#endif
  // al still holds the length of the last axis
 }
#if C_AVX2 || EMU_AVX2  // WARNING!!! this conditional starts with else
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
#if C_AVX2 || EMU_AVX2
  case 0b11111: fcopygather8(fcopyvalNI) axflags-=AXFCKST0; break; case 0b10111: fcopygather8(fcopyval) break; case 0b01111: fcopygather8(fcopyvalN) break;
    // carry on with fewer audits if gather repeated
  case 0b00011: fcopygatherinfull break;
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
  I rodo=r-1;  // start on axis -2   TUNE handle axis -2 separately if registers are plenteous.  Have nextx/nsel/axr->sels be the values for axis 1, reloaded after the bottom of the wheel loop
  while(1){  // till we have handled all changing wheels
   struct faxis *axr=&axes[rodo];
   I nextx=axr->currselx+1;  // next index number to use; if taken in full, is also the index itself
   I nsel=REPSGN(axr->nsel)^axr->nsel;  // number of selectors in this wheel
   if(nextx!=nsel){
    // the current wheel is not rolling over
    axr->currselx=nextx;  // save selector#, which is the loop counter
    if(axr->sels!=0){  // if axis taken in full, nextx is already set
     // axis not in full and did not roll over.  Advance to next index
     I axn=axr->lenaxis;  // length of this axis of w
     if(likely(axr->nsel>=0)){  // complementary index?
      // normal non-complementary index.  Step to the next row
      SETNDX(nextx,axr->sels[nextx],axn);  // fetch next index
     }else{
      // complementary index.  .currselv is the last index we returned; .indsubx.subx is a valid starting place to search for the next index 
      // we know there is another valid index.
      nextx=axr->currselv+1; I subx=axr->indsubx.subx; // bit# to start look, place to look for match
      I nextcomp;  // next complementary index to use
      while((nextcomp=subx<axn-nsel?axr->sels[subx]:axn)<=nextx){  // when sel doesn't limit, advance to next sel; hidden last value of axn
       nextx+=nextcomp==nextx; axr->indsubx.subx=++subx;  // if sel matches nextx, advance nextx to skip it
      }
     }
    }
    base+=(nextx-axr->currselv)*axr->lencell;  // move base by amount of index movement
    axr->currselv=nextx;  // set next index, also saving last one used for relocation
    break;  // when wheel doesn't roll over, stop processing wheels
   }
   // here the current axis is rolling over.
   if(rodo<=0)goto endaxes;  // when we roll over the biggest wheel, quit
   axr->currselx=0;   // back to start
   base+=(axr->sel0-axr->currselv)*axr->lencell;  // move base by amount of index movement
   axr->indsubx.subx=axr->currselv=axr->sel0;  // set starting index for wheel (and hint, in case complementary)
   --rodo;  // back up to increment previous axis
  }  // wheel rolled over, advance to next wheel
 }  // loop till all _1-cells moved.   base has been advanced
endaxes:;
 RETF(z);
}

// a is numeric
F2(jtifrom){F12IP;A z;C*wv,*zv;I acr,an,ar,*av,j,k,p,pq,q,wcr,wf,wn,wr,*ws,zn;
 ARGCHK2(a,w);
 // IRS supported but only for a single a value.  This has implications for empty arguments.
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(unlikely(ar>acr))R rank2ex(a,w,DUMMYSELF,acr,wcr,acr,wcr,jtifrom);  // split a into cells if needed.  Only 1 level of rank loop is used
 // From here on, execution on a single cell of a (on matching cell(s) of w, or all w).  The cell of a may have any rank
 an=AN(a); wn=AN(w); ws=AS(w);
 if(unlikely(!ISDENSETYPE(AT(a),INT))){
  if(AR(a)<(AT(a)&B01))a=zeroionei(BAV(a)[0]);else{RZ(a=ccvt(INT,a,0)); jtfg=(J)((I)jtfg|JTINPLACEA);}  // convert boolean or other arg to int, with special check for scalar boolean.  Allocated result is always eligible to inplace
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
 // since inplacing may change the type, we further require that the block not be UNINCORPABLE, and the result also must be DIRECT since
 // the copy may be interrupted by index error and be left with invalid atoms, and if boxed may be to a recursive block.  Also, a must not be the same block as w
 jtfg=(J)((I)jtfg&~((((a!=w)&SGNTO0(AC(a)&SGNIFNOT(AFLAG(a),AFUNINCORPABLEX)&-(AT(w)&DIRECT)))<=(UI)(wf|(wcr^1)|(LGSZI^bplg(AT(w)))))<<JTINPLACEAX));
 RETF(jtaxisfrom(jtfg,w,axes,(wncr<<24)+(wf<<16)+((ar+wr-(I)(0<wcr))<<8)+r*0x81))  // move the values and return the result
}    /* a{"r w for numeric a */


// a is array whose 1-cells are index lists, w is array
// result is (<"1 a) { w
// wf is length of the frame
A jtfrombu(J jtfg,A a,A w,I wf){F12IP;
 ARGCHK2(a,w)
 if(unlikely(!(AT(a)&INT)))RZ(a=ccvt(INT,a,0));  // integral indexes required
 I *as=AS(a); I af=AR(a)-1; I naxa=as[af]; naxa=af>=0?naxa:1; af=af>=0?af:0;  // naxa is length of the index list, i. e. number of axes of w that disappear during indexing
 I wr=AR(w); I *ws=AS(w); I wcr=wr-wf;
 ASSERT(naxa<=wcr,EVLENGTH)  // the number of axes must not exceed the length of a major cell
 I nia; PROD(nia,af,as);     // number of 1-cells of a
 if(unlikely((-AN(a)&-AN(w))>=0)){A z, z1;  // empty array, either a or w
  // if empty a, return w-cell in full, one copy per 1-cell of a, as ({.^:({:$a)w)"_"1 a   .  "_ needed to prevent w from being construed as a gerund
  if(unlikely(AN(a)==0))R dfv1(z,a,qq(qq(df1(z1,w,powop(ds(CHEAD),tail(shape(a)),0)),ainf),zeroionei(1)));
  // empty w.  We must verify the indexes of a do not exceed w's shape
  I *ad=IAV(a); DO(nia, DO(naxa, ASSERT((*ad^REPSGN(*ad))<ws[i],EVINDEX); ++ad;) ) 
  // allocate empty result, move in shape: frame of w, frame of a, shape of item
  GA00(z,AT(w),0,wf+(wcr-naxa)+af); MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AS(a),af)  MCISH(AS(z)+wf+af,AS(w)+wf+naxa,wcr-naxa)
  R z;
 }
 // fast case: a is an atom or a list (now not empty).  Select the single cell, which may be virtual or pristine
 if(af+wf==0){I jj=0, *av=IAV(a); DONOUNROLL(naxa, I j; SETNDX(j,av[i],ws[i]) jj*=ws[i]; jj+=j;) R jtget1cell(jtfg,w,wr-naxa,jj,AT(w),wr);}
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
 RETF(jtaxisfrom((J)((I)jtfg&~JTINPLACEA),w,axes,((wcr-naxa)<<24)+(wf<<16)+(zr<<8)+r*0x81))  // move the values and return the result.  Pass through inplaceability of w, not a
}    /* (<"1 a){"r w, dense w, integer array a */

// general boxed a
static F2(jtafrom){F12IP; PROLOG(0073);
 ARGCHK2(a,w);
 I ar=AR(a); I acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 I wr=AR(w); I wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; I wf=wr-wcr; RESETRANK;
 // We have IRS on w, but not a.  If there are multiple cells of a we use a rank loop.
 if(unlikely(ar!=0)){A t0,t1,t2;  // if there is an array of boxes
  // Since the native rank of a is 0, if w has only one cell we can loop over boxes of a
  R wr==wcr?rank2ex(a,w,DUMMYSELF,0L,wcr,0L,wcr,jtafrom):  // if a has frame, rank-loop over a
    // If both a and w have cells, we must match them up by boxing
      dfv2(t0,IRS1(a,0L,acr,jtbox,t1),IRS1(w,0L,wcr,jtbox,t2),amp(ds(CLBRACE),ds(COPE)));  // > (<"arank a) {&> <"wrank w
 }
 // a is an atomic box.  Open it
 A c=C(AAV(a)[0]);  // contents of a
 if(!(AT(c)&BOX)){R jtfrombu(jtfg,c,w,wf);}  // if single-boxed, handle as <"1@[ { ].
 // Double-boxed. Set up axis structs
 // We DO NOT treat leading scalar indexes as a special case here.  Building & using the axis block is pretty cheap.  We catch them when we fill.
 ASSERT(1>=AR(c),EVRANK);  // boxes may not have rank > 1
 ASSERT(AN(c)<=wcr,EVLENGTH);  // number of axes must not exceed #axes in major cell
 I *ws=AS(w);  // #axes-1.  We need a leading axis in full if there are multiple cells of w
 struct faxis stataxes[5], *axes;  // one for frame, several for data
 I *cmbase;  // start of area we can use for complementary bitmasks
 UI naxesreq=AN(c)+!!wf;  // max # axes we might need
 if(likely(naxesreq<=sizeof(stataxes)/sizeof(stataxes[0]))){axes=stataxes; cmbase=(I*)&stataxes[naxesreq]; }
 else{A t; GATV0(t,INT,naxesreq*(sizeof(stataxes[0])>>LGSZI),1) axes=(struct faxis*)IAV1(t); cmbase=(I*)&stataxes[0];}  // rank 1 for alignment
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
    if(AR(aa)<(AT(aa)&B01))aa=zeroionei(BAV(aa)[0]);else RZ(aa=ccvt(INT,aa,0));  // convert boolean or other arg to int, with special check for scalar boolean
   }
   if(AN(aa)==0){
    // axis taken in full.  fill it in that way
    axes[i].nsel=axes[i].lenaxis;   // fill taken-in-full axis
    axes[i].currselx=0; axes[i].sels=0; axes[i].currselv=0; axes[i].sel0=0;  // fill in startpt - should be 1 store    if(unlikely(i==naxes))--naxes;  // taken-in-full cannot be last axis
    if(unlikely(i==r)){--r; ++wncr;}  // taken-in-full cannot be last axis.  treat as if omitted, i. e. as if AN(c) decremented
   }else{
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
   }
   axes[i].indsubx.ind=mtv; ++zr;  // triple-boxed selector gets one result axis
  }else{
   // normal axis.
   if(unlikely(!ISDENSETYPE(AT(aa),INT))){
    if(AR(aa)<(AT(aa)&B01))aa=zeroionei(BAV(aa)[0]);else RZ(aa=ccvt(INT,aa,0));  // convert boolean or other arg to int, with special check for scalar boolean
   }
   axes[i].nsel=AN(aa); axes[i].sels=IAV(aa); axes[i].sel0=0; axes[i].indsubx.ind=aa;
   // audit and translate first index to speed reset
   if(likely(axes[i].nsel!=0)){I sel0=axes[i].sels[0]; sel0+=REPSGN(sel0)&axes[i].lenaxis; ASSERT((UI)sel0<(UI)axes[i].lenaxis,EVINDEX) axes[i].sel0=sel0;}
   zr+=AR(aa);  // each axis of selectors goes into the result
  }
 }
 if(unlikely(r<hasr))R RETARG(w);  // if all axes taken in full, do nothing, return full w
 I *rla=&axes[0].lencell; rla=hasr?rla:jt->shapesink; *rla=celllen;  // if there is frame, it needs len of a major cell
 RETF(jtaxisfrom((J)((I)jtfg&~JTINPLACEA),w,axes,(wncr<<24)+(wf<<16)+(zr<<8)+(hasr<<7)+r))  // move the values and return the result
}    /* a{"r w for boxed index a */

// a{"r w  We handle the fast cases (atom{array) and (empty{"r array) here.  For others we go to a type-dependent processor for a that will build index lists
DF2(jtfrom){F12IP;A z;
 ARGCHK2(a,w);
 I at=AT(a), wt=AT(w), ar=AR(a), wr=AR(w);
 if(likely(!ISSPARSE(at|wt))){
  PROLOG(000);
  // Handle the simple case of unboxed atom { array, and no frame: single cell
  // We don't process NJA through here because it might create a virtual block & we don't want NJAs rendered unmodifiable by virtual blocks
  if(!((at&BOX)+ar+(SGNTO0((((RANKT)jt->ranks-wr)|(wr-1))))+(AFLAG(w)&AFNJA))){   // if AR is unboxed atom and w has no frame
   I av;  // selector value
   if(likely(at&(B01|INT))){av=BIV0(a);  // B01/INT index.  We don't set at=INT for B01 because we aren't sure it's OK to overwrite a, which might be NJA.  Questionable analysis.
   }else{
    if(likely(at&FL)){  // FL index
     D af=DAV(a)[0], f=jround(af); av=(I)f; if(SY_64)at=INT;  // av=index; if INT atom can hold FL atom, pretend a is INT so we can 
     ASSERT(ISFTOIOK(f,af),EVDOMAIN);  // if index not integral, complain.  IMAX/IMIN will fail presently.  We rely on out-of-bounds conversion to peg out one side or other (standard violation)
    }else{RZ(a=cvt(INT,a)) av=IAV(a)[0]; at=INT;}  // other index - must be convertible to INT, do so
   }  // now av is the index and at has been modified, perhaps, to allow inplacing of converted a

   if((SGNIF(at,INTX)&-(wt&INT+(SY_64*FL)+BOX)&(wr-2))<0){  // w is atom or list whose atomsize is SZI; a is atom of same size, result is atom
    // here moving SZI-sized atoms, which means we can put the result on top of a if a is direct inplaceable abandoned
    // We focus on SZI-sized atoms because we move them without a loop and can inplace into a.  If we can't inplace into a we could revert to general 1-cell code below, but we skip quite a bit here
    I j; SETNDX(j,av,AN(w));  // fetch and audit index into j
    // Get the area to use for the result: the a input if possible (inplaceable, incorpable, DIRECT), else an INT atom. a=w OK!
    // We can't get away with changing the type for an INT atom a to BOX.  It would work if the a is not contents, but if it is pristine contents it may have
    // been made to appear inplaceable in jtevery.  In that case, when we change the AT we have the usecount wrong, because the block is implicitly recursive by virtue
    // of being contents.  It's not a good trade to check for recursiveness of contents in tpop (currently implied).
    if((SGNIF(jtfg,JTINPLACEAX)&AC(a)&(((AFLAG(a)|wt)&AFUNINCORPABLE+AFNJA+BOX)-1))<0){z=a; AT(z)=wt;} else{GA00(z,wt,1,0)}  // NJA=LIT, ok.  transfer the block-type if we reuse a
    // Move the value
    IAV(z)[0]=IAV(w)[j];   // change type only if the transfer succeeds, to avoid creating an invalid a block that eformat will look at
    // We transferred one I/A out of w.  We must mark w non-pristine.  If it was inplaceable, we can transfer the pristine status.  We overwrite w because it is no longer in use
    PRISTXFERF(z,w)  // this destroys w
   }else{
    // Not SZI-sized items.  w is not INT/FL/BOX or has rank >1, return single cell, possibly virtual
    I wi; SETIC(w,wi); // number of items in w
    I j; SETNDX(j,av,wi);  // j=positive index, audited
    I wcr=wr-1; wcr-=REPSGN(wcr);  // rank of cell of w: rank-1 or 0
    z=jtget1cell(jtfg,w,wcr,j,wt,wr);  // fetch the single cell, possibly as a virtual
#if 0  // obsolete
    I *ws=AS(w); I m; PROD(m,wcr,ws+wr-wcr);  // shape of w, number of atoms in a cell
    if(m<MINVIRTSIZE){  // if cell too small for virtual, allocate & fill here
     I k=bplg(wt); GA(z,wt,m,wcr,ws+wr-wcr) JMC(CAVn(wcr,z),CAV(w)+j*(m<<k),m<<k,0);  // copy in the data, possibly overstoring up to 7 bytes.  Nonrecursive block
     // We transferred one I/A out of w.  We must mark w non-pristine.  If it was inplaceable, we can transfer the pristine status.  We overwrite w because it is no longer in use
     PRISTXFERF(z,w)  // this destroys w
    }else{
     RZ(z=virtualip(w,j*m,wcr));   // if w is rank 2, could reuse inplaceable a for this virtual block
     // fill in shape and number of atoms.  ar can be anything.
     AN(z)=m; MCISH(AS(z),ws+1,wcr)
     // When we create a virtual block we do not actually copy anything out of w, so it remains pristine.  The virtual block is not.
    }
#endif
    RETF(z); // there have been no needless allocations, and thus no need for EPILOG
   }
  }else if(unlikely(AN(a)==0)){  // a is empty, so the result must be also.  Doesn't happen often but we save big when it does
   I zr=wr-1+SGNTO0(SGNIF(at,BOXX));  // rank of w, -1 if a is not boxed
   if(!((jt->ranks-((ar<<RANKTX)+wr))&(((RMAX+1)<<RANKTX)+(RMAX+1)))){  // is there frame?
    // The case of (empty array) { y (no frame).  Result is (($x),(}.^:(32~:3!:0 x) $y)) ($,) y.
    // $ (i.0 0) { (i. 4 5)  is 0 0 5;  $ (0 0$a:) { (i. 4 5) is 0 0 4 5.  $ (0$a:) { 5  is  $ (0$0) { 5  is 0
    zr=zr<0?0:zr;  // rank of cell of w
    // if result is empty, we can use a as the return element if it is incorpable and abandoned inplaceable or it is an empty of the right type
    if(((zr-1)&SGNIFNOT(AFLAG(a),AFUNINCORPABLEX)&((AC(a)&SGNIF(jtfg,JTINPLACEAX))|-(at&wt&NOUN)))<0){z=a; I *tv=&AT(a); tv=at&wt&NOUN?&jt->shapesink[0]:tv; *tv=wt;
    }else{GA00(z,wt,0,zr+ar); MCISH(AS(z),AS(a),ar) MCISH(AS(z)+ar,AS(w)+wr-zr,zr)  // if we can't reuse a, allocate & fill in
    }
   }else{
    // There is frame.  We have to check agreement.  shape is (long frame),(a cell shape),(w cell shape possibly beheaded)
    I af=ar-(jt->ranks>>RANKTX); af=af<0?0:af; I wf=wr-(RANKT)jt->ranks; wf=wf<0?0:wf; I lf=af<wf?wf:af; I cf=af<wf?af:wf; A la=af<wf?w:a;  // af, wf=lens of outer frame; lf=len of long frame; la->longer frame
    ASSERTAGREE(AS(a)+af-cf,AS(w)+wf-cf,cf)  // cf=common frame; verify common frames agree
    zr-=wf; zr=zr<0?0:zr;  // remove the w frame from w rank to get the cell-rank
    GA00(z,wt,0,lf+ar-af+zr); MCISH(AS(z),AS(la),lf) MCISH(AS(z)+lf,AS(a)+af,ar-af) MCISH(AS(z)+lf+ar-af,AS(w)+wr-zr,zr) // allocate the empty array & move in shape
   }
  }else{
   // not (atom/empty){array.  Process according to type of a
    RANK2T origranks=jt->ranks;  // remember original ranks in case of error
   if(!(at&BOX))z=jtifrom(jtfg,a,w);else z=jtafrom(jtfg,a,w);
   // If there was an error, call eformat while we still have the ranks.  convert default rank back to R2MAX to avoid "0 0 in msg
   if(unlikely(z==0)){jt->ranks=origranks!=RMAX?origranks:R2MAX; jteformat(jt,self,a,w,0); RESETRANK; R0}
   // Here we transferred out of w.  We must mark w non-pristine unless the result was virtual
   // Since there may have been duplicates, we cannot mark z as pristine.  We overwrite w because it is no longer in use
   if(!(AFLAG(z)&AFVIRTUAL))PRISTCLRF(w)
  }
  EPILOG(z);
 }else if(ISSPARSE(at&wt)){z=fromss(a,w);  // sparse cases
 }else if(ISSPARSE(wt)){z=at&BOX?frombs(a,w) : fromis(a,w);
 }else{z=fromsd(a,w);}
 RETF(z);
}   /* a{"r w main control */

DF2(fork242);
DF2(jtsfrom){F12IP;
 if(likely(!ISSPARSE((AT(w))))){
  // not sparse, transfer to code to handle it if nonempty numeric.  If nonnumeric (must be boxed),
  // if the arg is empty, the details are tricky depending on the type and we just revert to boxed code
  if(likely(((AN(a)-1)|((AT(a)&NUMERIC)-1))>=0))R jtfrombu(jt,a,w,0);  // handle if a is nonempty numeric
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
   RZ(z=ISDENSETYPE(t,INT)?ca(a):ccvt(INT,a,0)); v=AV(z);
   DQ(n, DO(c, SETNDXRW(k,*v,ws[i]) ++v;););  // convert indexes to nonnegative & check for in-range
  }
  R frombsn(z,w,0L);  // handle the special case
mustbox:;
 }
 // If we couldn't handle it as a special case, do it the hard way
 RETF(fork242(jt,a,w,self))  // revert using the fork for (f@[ g ])
}    /* (<"1 a){w */

static F2(jtmapx);
static EVERYFS(mapxself,0,jtmapx,0,VFLAGNONE)

static F2(jtmapx){F12IP;A z1,z2,z3;
 ARGCHK2(a,w);
 if(!(BOX&AT(w)))R ope(a);
 RZ(z1=catalog(every(shape(w),ds(CIOTA))));  // create index list of each box
 IRS1(z1,0,0,jtbox,z2);
 RZ(z2=every2(a,z2,(A)&sfn0overself));
 IRS1(z2,0,0,jtbox,z3);
 R every2(z3,w,(A)&mapxself);
}

F1(jtmap){F12IP;R mapx(ds(CACE),w);}

// extract the single box a from w and open it.  Don't mark it no-inplace.  If w is not boxed, it had better be an atom, and we return it after auditing the index
static F2(jtquicksel){F12IP;I index;
 RE(index=i0(a));  // extract the index
 SETNDX(index,index,AN(w))   // remap negative index, check range
 R AT(w)&BOX?C(AAV(w)[index]):w;  // select the box, or return the entire unboxed w
}

DF2(jtfetch){F12IP;A*av, z;I n;
 F2RANKW(1,RMAX,jtfetch,self);  // body of verb applies to rank-1 a, and must turn pristine off if used higher, since there may be repetitions.
 if(unlikely(ISSPARSE(AT(w)|AT(a)))){   // sparse x or y
  ASSERT(!ISSPARSE(AT(a)),EVNONCE)  // selector must be dense
  if(!(AT(a)&BOX))RZ(a=box(a));   // box open a
  ASSERT(AN(a)==1,EVLENGTH)   // must be a single selector, implement as (boxopen x) { y
  RETF(from(a,w));
 }
 if(!(BOX&AT(a))){
  // look for the common special case scalar { boxed vector.  This path doesn't run EPILOG
  if(((AT(w)>>BOXX)&1)>=(2*AR(a)+AR(w))){  // a is an atom, w is atom or list of boxes   AR(a)==0 && (AR(w)==0 || (AR(w)==1 && AT(w)&BOX))
   RZ(z=jtquicksel(jt,a,w));  // fetch selected box, opened.  If not a box, just return w
   // if the result is w, leave its inplaceability.  If contents, it will perforce be uninplaceable
#if AUDITBOXAC
   if(!(AFLAG(w)&AFVIRTUALBOXED)&&AC(z)<0)SEGFAULT;
#endif
// obsolete    RETF(z);   // turn off inplace if w not inplaceable, or jt not inplaceable.
  }else{
// obsolete   RZ(a=jtbox(JTIPAtoW,a));  // if not special case, box any unboxed a   scaf should call into call into frombu without boxing
   RZ(z=jtfrombu(jtfg,a,w,0)); if(((AT(w)>>BOXX)&1)>AR(z))z=C(AAV(z)[0]);  // a must be scatter-indexes.  No need to box them, just fetch.  If atomic box returned, open it
  }
  PRISTCLRF(w) // Since the whole purpose of fetch is to copy one contents by address, we turn off pristinity of w
  RETF(z); 
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


// Dictionary support

#ifndef CRC32
#define HASH4(crc,x) ((UI4)(crc)*0x85249421+(UI4)(x))
#define HASH8(crc,x) HASH4(HASH4(crc,x),(UI8)(x)>>32)
#else
#define HASH4(crc,x) CRC32(crc,x)
#if BW==64
#define HASH8(crc,x) CRC32L(crc,x)
#else
#define HASH8(crc,x) HASH4(HASH4(crc,x),(UI8)(x)>>32)
#endif
#endif
#if BW==64
#define HASHI(crc,x) HASH8(crc,x)
#else
#define HASHI(crc,x) HASH4(crc,x)
#endif

// CRC32 of n floats.
static INLINE UI4 crcfloats(UI8 *v, I n){
 // Do 3 CRCs in parallel because the latency of the CRC instruction is 3 clocks.
 // This is executed repeatedly so we expect all the branches to predict correctly
 UI4 crc0=-1;  // convert bytecount to words
 if((n-=3)<0){crc0=HASH8(crc0,v[0]==0x8000000000000000?0:v[0]); if(n==-1)crc0=HASH8(crc0,v[1]==0x8000000000000000?0:v[1]); R crc0;}   // fast path for the common short case
 UI4 crc1=crc0, crc2=crc0;  // init all CRCs
 do{crc0=HASH8(crc0,v[n]==0x8000000000000000?0:v[n]); crc1=HASH8(crc1,v[n+1]==0x8000000000000000?0:v[n+1]); crc2=HASH8(crc2,v[n+2]==0x8000000000000000?0:v[n+2]);}while((n-=3)>=0);  // Do blocks of 24 bytes
 if(n>=-2){crc0=HASH8(crc0,v[0]==0x8000000000000000?0:v[0]); if(n>-2)crc1=HASH8(crc1,v[1]==0x8000000000000000?0:v[1]);}  // handle last 1 or 2 words
 R HASH8(crc1,(crc2<<29)+(crc1<<3));  // combine the CRCs, as quickly as possible
}

// CRC32 of n Is
static INLINE UI4 crcwords(UI *v, I n){
 // Do 3 CRCs in parallel because the latency of the CRC instruction is 3 clocks.
 // This is executed repeatedly so we expect all the branches to predict correctly
 UI4 crc0=-1;  // convert bytecount to words
 if((n-=3)<0){crc0=HASHI(crc0,v[0]); if(n==-1)crc0=HASHI(crc0,v[1]); R crc0;}   // fast path for the common short case
 UI4 crc1=crc0, crc2=crc0;  // init all CRCs
 do{crc0=HASHI(crc0,v[n]); crc1=HASHI(crc1,v[n+1]); crc2=HASHI(crc2,v[n+2]);}while((n-=3)>=0);  // Do blocks of 3 words
 if(n>=-2){crc0=HASHI(crc0,v[0]); if(n>-2)crc1=HASHI(crc1,v[1]);}  // handle last 1 or 2 words
 R HASH8(crc1,(crc2<<29)+(crc1<<3));  // combine the CRCs, as quickly as possible
}

// CRC32 of n bytes
static INLINE UI4 crcbytes(C *v, I n){
 UI4 wdcrc=crcwords((UI*)v,n>>LGSZI);  // take CRC of the fullword part
 if(n&(SZI-1)){wdcrc=HASHI(wdcrc,((UI*)v)[n>>LGSZI]<<((SZI-(n&(SZI-1)))<<LGSZI));}  // if there are bytes, take their CRC after discarding garb.  Avoid overfetch
 R wdcrc;  // return composite CRC
}

// CRC32 of n boxes
static UI4 jtcrcy(J jt,A y);
static INLINE UI4 crcboxes(J jt, A *v, I n){UI4 crc=0; DO(n, crc=HASH4(crc,jtcrcy(jt,C(v[i])));) R crc;}

// CRC32 of n Xs
static INLINE UI4 crcxnums(X *v, I n){UI4 crc=0; DO(n, crc=HASH4(crc,crcwords(UIAV(v[i]),XLIMBLEN(v[i])));) R crc;}

// CRC32 of y.  Floats must observe -0.
static UI4 jtcrcy(J jt,A y){
 I yt=AT(y), yn=AN(y); void *yv=voidAV(y);  // type of y, #atoms, address of data
 if(yt&INT+(SZI==4)*(C4T+INT4))R crcwords(yv,yn);  // INT type, might be full words
 if(yt&B01+LIT+C2T+C4T+INT1+INT2+INT4)R crcbytes(yv,yn<<bplg(yt));   // direct non-float
 if(yt&FL+CMPX+QP)R crcfloats(yv,yn<<(((UI)yt>=CMPX)));  // float
 if(yt&BOX){R crcboxes(jt,yv,yn);}  // box, take CRC of all the boxes
 if(yt&RAT+XNUM){R crcxnums(yv,yn<<((UI)yt>=RAT));}  // RAT/XNUM, take CRC of all of them
 else SEGFAULT;  // scaf
}

// 16!:0 monad create 32-bit hash of y.  For float types, +-0 must have the same hash
DF2(jthashy){F12IP; ARGCHK2(a,w) RETF(sc(jtcrcy(jt,w)));}

#if 0
// everything about the dictionary.  This struct overlays the entire block.  AM of the first
// cacheline is the lock for the dic.  The data starts at AS[1], the second cacheline.  The block
// is a recursive BOX but AK points past the areas that do not need to be freed, i.e keys
typedef struct {
 I header[8];  // A header up through s[0].  DIC is always allocated with rank 1.
 struct Dic { // *** this group of values is updated atomically en bloc to make sure hashelesiz matches hash when possible.
  union {
   struct {
    filler[5];
    C flags;  
#define DICFIHF 1  // set if using internal hashing function
#define DICFICF 2  // set if using internal compare function
    C lgminsiz;  // lg(minimum cardinality).  When cardinality drops below 1LL<<lgminsiz, we resize.  Top 3 bits always 0, used as LSBs of hashelesize
    C hashelesize;  // number of bytes in a hash slot, 1-5 (we support only 4)
   }
   UI hashsiz;  //  number of elements in hash table.  We leave space for 40 bits but we don't support more than 32
  }
  UI4 (*hashfn)();  // hash function, user's or selected internal
  C (*compfn)();  // key-comparison function, user's or selected internal
  A hash;  // the hash table, rank 1  Note: This pointer may be stale.  MUST use voidAV1 to point to hash, and use only PREFETCH until you get a lock
  A keys;  // array of keys
  A vals;   // array of values
  A empty;  // LIT shape (maxeles,hashelesiz).  occupied kvs are -1; empties are in a chain that loops to itself at the end, rank 1
  // end of first cacheline.  Following are unchanging
  A hashcompself;  // self to use for user's hash/compare verb
  A locale;  // the numbered locale of the dictionary, saved here to protect it.  User hash/compare is called in this locale
  A kshape;   // shape of a key, always accessed by AAV1
  A vshape;  // shape of a value, always accessed by AAV1
  UI4 ktype;  // type of a key
  UI4 vtype;   // type of a value
  union {
   struct {
    UI4 kitemlen;  // number of hash/compare items in a key when using 16!:0
    UI4 kbytelen;  // number of bytes in a key
   }
   UI8 klens;
  }
  UI4 vbytelen;  // number of bytes in a value, for copying
  UI4 kaii;   // atoms in a key
  UI4 vaii;   // atoms in a value
// 4 bytes free
  // end of second cacheline.  Following changed only by put/del
  UI cardinality;  // number of kvs in the hashtable
  I emptyn;  // index to next empty kv.  -1 when empty.  Resize when empty
  I filler2[SY_64?6:3];  // pad to cacheline (24 words on each system).
 } bloc;
} DIC;
#define ST UI4   // type of hash slot
#define STX UI8   // type of index to hash slot, which is + for found, 1s-comp for not found
#define STN 4  // width of hash slot

// u 16!:_1 y  conjunction.  allocate hashtable.  Always called from the numbered locale of the dictionary.
// x is the hash/compare verb OR the DIC block for a dictionary that is being resized.
// y is (min #eles,max #eles,#hashslots);(flags, currently empty);(key type;key shape);(value type;value shape)    to create a new dictionary
//      (min #eles,max #eles,#hashslots)   when a dictionary is being resized
// result is DIC to use, ready for get/put, with hash/keys/vals/empty allocated, and 0 valid kvs
F2(jtcreatedic){F12IP;A box,box1;  // temp for box contents
 ARGCHK2(a,w);
 ASSERT(AT(a)&VERB,EVDOMAIN)  // u must be the user's verb
 A z; GAT0(z,BOX,20,1) AK(z)=offsetof(Dic,hash); AN(z)=8;  // allocate nonrecursive box, long enough to make the total allo big enough in 32- and 64-bit.  Then restrict to the boxes in case of error
 if(AT(a)&VERB){  // initial creation
  //  install user's verb, & see if the user wants the internal functions
  AF fn=FAV(a)->valencefns[0]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[0])->valencefns[0]:fn; ((DIC*)z)->flags=fn==jthashy?DICFIHF:0;  // note if default hash
  fn=FAV(a)->valencefns[1]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[0])->valencefns[1]:fn; I flags=((DIC*)z)->flags|=fn==jttao?DICFICF:0;  // note if default hash
  INCORPNV(a); ((DIC*)z)->hashcompself=a;    // save self pointer to user's function, which protects it while this dic is extant
  ((DIC*)z)->locale=jt->global;  // remember the locale this dictionary is in.  We protect it in the DIC block
  // flags, key, value sizes
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==4,EVLENGTH)  // 4 boxes
  box=C(AAV(w)[1]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==0,EVLENGTH)  // flags.  None currently supported
  box=C(AAV(w)[2]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)  // keyspec.  must be 2 boxes
  box1=C(AAV(box)[0]); I t; RE(t=i0(a)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN)  // type.  convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid
  box1=C(AAV(box)[1]); ASSERT(AR(box1)<=1,EVRANK) ASSERT(AN(box1)>=0) RZ(box1=ccvt(INT,ravel(box1))) I n, *s=IAV(box1); PRODX(n,AN(box1),s,1) ((DIC*)z)->kaii=n; ASSERT(n>0,EVLENGTH) // shape. copy to allow AAV1.  get # atoms in item & save
  INCORPNV(box1); ((DIC*)z)->kshape=box1; ((DIC*)z)->ktype=t; I l=n<<bplg(t); ((DIC*)z)->kbytelen=n; // save shape & type; save #bytes in key
  void (*fn)()=l&(SZI-1)?crcbytes:crcwords; fn=(t&XNUM+RAT)?crcxnums:fn; fn=(t&CMPX+FL+QP)?crcfloats:fn; fn=(t&BOX)?crcboxes:fn; fn=flags&DICFIHF?fn:FAV(a)->valencefns[0]; ((DIC*)z)->hashfn=fn; // save internal or external hash function  
  void (*fn)()=l&(SZI-1)?taoc:taoi; fn=(t&XNUM+RAT)?taox:fn; fn=(t&CMPX+FL+QP)?taof:fn; fn=(t&BOX)?taor:fn; fn=flags&DICFICF?fn:FAV(a)->valencefns[1]; ((DIC*)z)->compfn=fn; // save int/ext comp function.  We care only about equality  
  l>>=(fn==crcboxes)?LGSZI:0; l>>=(fn==crcfloats)?((t|(t>>(QPX-BOXX)))>>CMPX)+FLX:0; l>>=(fn==crcxnums)?(t>>RATX)+LGSZI:0; l>>=(fn==crcwords)?LGSZI:0; ((DIC*)z)->kitemlen=l;  // length to use for internal hash/comp
  box=C(AAV(w)[3]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)  // valuespec.  must be 2 boxes
  box1=C(AAV(box)[0]); RE(t=i0(a)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN)  // type. convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid
  box1=C(AAV(box)[1]); ASSERT(AR(box1)<=1,EVRANK) ASSERT(AN(box1)>=0) RZ(box1=ccvt(INT,ravel(box1))) I n, *s=IAV(box1); PRODX(n,AN(box1),s,1) ((DIC*)z)->vaii=n;  // shape. copy to allow AAV1.  get # atoms in item & save
  INCORPNV(box1); ((DIC*)z)->vshape=box1; ((DIC*)z)->vtype=t; ((DIC*)z)->vbytelen=n<<bplg(t);  // save shape & type; save # bytes for copy
  box=C(AAV(w)[0]);  // fetch size parameters
 }else{  // resize
  ((DIC*)z)->bloc=((DIC*)a)->bloc;  // init everything from the previous dic
  box=w;  // w is nothing but size parms
 }

 // box has the size parameters.  Audit & install into dic
 ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==3,EVLENGTH) if(!AT(box)&INT)RZ(box=ccvt(INT,box));  // sizes. must be box of 3 integers
 ((DIC*)z)->lgminsiz=CTLZI(UIAV(box)[0]|1); I maxeles=((DIC*)z)->maxeles=IAV(box)[1]; ASSERT(maxeles>0,EVDOMAIN) I hashsiz=(UI4)((DIC*)z)->hashsiz=IAV(box)[2]; ASSERT(hashsiz>(maxeles+(maxeles>>4)),EVDEADLOCK)  // min, max, hash sizes.  Hash at least 6% spare
 I hashelsesiz=((DIC*)z)->hashelesiz=(CTLZI(maxeles+2-1)+1+(BB-1))>>LGBB;  // max slot#, plus 2 (empty/tombstone).  Subtract 1 for max code point.  top bit#+1 is #bits we need; round that up to #bytes

 // allocate & protect hash/keys/vals/empty
 GATV0(box,LIT,hashsiz*hashelesiz,1) INCORPNV(box) mvc(hashsiz*hashelesiz,CAV1(box),MEMSETFFLEN,MEMSET00) (DIC*)z)->hash=box;  // allocate hash table & fill with empties
 GATV0(box,LIT,maxeles*hashelesiz,1) INCORPNV(box) void *ev=voidAV1(box); DO(maxeles-1, *(I*)ev=i+1; ev=(void *)((I)ev+hashelesiz);)   // allocate empty list & chain empties together
 *(I*)ev=maxeles-1; (boxn=0; (DIC*)z)->empty=box; // install end of chain loopback and point the dic to beginning of chain
 GATV0(box,((DIC*)z)->ktype,maxeles*((DIC*)z)->kaii,1) INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(((DIC*)z)->kshape),AN(((DIC*)z)->kshape)) (DIC*)z)->keys=box;   // allocate array of keys
 GATV0(box,((DIC*)z)->vtype,maxeles*((DIC*)z)->vaii,1) INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(((DIC*)z)->vshape),AN(((DIC*)z)->vshape)) (DIC*)z)->vals=box;   // allocate array of keys
 (DIC*)z)->cardinality=0;  // init the dic is empty
 // allocate DIC, move data pointer to keys
 // fill in header
 // allocate & fill in data areas
 // protect the copied/allocated blocks, make result recursive
 ra0(z);   // make z recursive, protecting descendants
 RETF(z)
}

// Dic locking ************************************************************

// We use a 2- or 3-step locking sequence.  In each step the new state is requested shortly before it is needed and then waited for when it is needed (to give the lock request time to propagate to all threads).
// Readers use the sequence
// lv=DICLKRDRQ(dic)  // request read lock
// DICLKRDWT(dic,lv)  // wait for read lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// DICLKRDREL(dic)  // release read lock

// Writers use the sequence
// lv=DICLKRWRQ(dic)  // request read/write lock
// DICLKRWWT(dic,lv)  // wait for read/write lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// lv=DICLKWRRQ(dic)  // request write lock
// DICLKWRWT(dic,lv)  // wait for write lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// DICLKWRREL(dic,lv)  // release write lock

// A read lock guarantees that the control info and tables will not be modified by any thread in a way that could affect a get().  Other modifications are allowed.
// A read-write lock guarantees that the control info and tables will not be modified by any other thread.  No modification by this thread mey affect a get() in progress.
//  When a read-write lock is granted, the granted thread is guaranteed to be the next to write.  It has ownership of the write-lock and sequence# bits until it releases the lock by writing to the sequence#
// A write lock guarantees that no other thread has a lock of any kind.  Any modification is allowed.

#if PYXES
// The AM field of dic is the lock. 0-15=#read locks outstanding 16-31=#prewrite locks outstanding 32=write equest 48-63=seq# of current writer (if 32 set) or next writer (if 32 clear)
#define DICLMSKRDX 0LL  // read locks outstanding
#define DICLMSKRWX 16LL  // write locks request, by size 0-3
#define DICLMSKWRX 32LL  // write lock requested by current write owner
#define DICLMSKSEQX 48LL  // sequence# of current write owner

#define DICLKRDRQ(dic) __atomic_fetch_add(&AM(dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL)  // put up read request
#define DICLKRDWT(dic,lv) if(unlikely((lv&((I)1<<DICLMSKWRX))!=0))diclkrdwt(dic);  // if someone is writing, wait till they finish
#define DICLKRDREL(dic)   __atomic_fetch_sub(&AM(dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL);  // remove read request

#define DICLKRWRQ(dic) __atomic_fetch_add(&AM(dic),(I)1<<DICLMSKRWX,__ATOMIC_ACQ_REL)   // put up prewrite request
#define DICLKRWWT(dic,lv) if(unlikely((US)(lv>>DICLMSKRWX)!=(US)(lv>>DICLMSKSEQX)))diclkrwwt(dic,lv);  // wait until we are the lead writer, which is immediately if there are no others
#define DICLKWRRQ(dic) ({ __atomic_store_n(&(US*)((I)AM(dic)+4,(US)1,__ATOMIC_RELEASE); __atomic_load_n(&AM(dic),__ATOMIC_ACQUIRE); })   // put up write request (we are known to be the lead writer and own the write bit, which must be clear)
#define DICLKWRWT(dic,lv) if(unlikely((lv&((I)0xffff<<DICLMSKRDX))!=0))diclkwrwt(dic);  // wait until all reads are quiesced
#define DICLKWRRQ(dic,lv) __atomic_store_n(&(UI4*)((I)AM(dic)+4,(((lv)>>32)&0xffff0000)+0x10000,__ATOMIC_RELEASE);   // remove write request and advance sequence#

// wait for read lock on dic.
static void diclkrdwt(DIC *dic){I n;
 // We know we just put up a read request and saw a writer.  Rescind our read request and then quietly poll for the write to go away
 do{
  DICLKRDREL(dic)  // remove read request
  for(n=5;;--n){
   delay(n<0?50:10)  // delay a bit.  the long delay uses mm_pause.
   if(__atomic_load_n(&(US*)((I)AM(dic)+4,__ATOMIC_ACQUIRE)==0)break;  // wait until write is complete
  }
 }while((__atomic_fetch_add(&AM(dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL)&((I)1<<DICLMSKWRX))!=0);  // put up our rd request again, wait for any write to finish
 R;
}

// wait for read-write lock on dic.
static void diclkrwwt(DIC *dic, I lv){I n;
 // We know we just put up a read-write request and saw that we were not the first read-write requester.  Wait till we make it to the top.  At that point we have the lock, with no further action needed
 while(1){
  US ourseq=lv>>DICLMSKRWX, curseq=lv>>DICLMSKSEQX;  // see where we stand
  if(ourseq==curseq)R;   // exit when we get to the top
  delay(ourseq-curseq>1?50:20)  // delay a bit.  the long delay uses mm_pause.  We use it when we are not the next requester
  lv=__atomic_load_n(&AM(dic),__ATOMIC_ACQUIRE);  // refresh status
 }
}

// wait for write lock on dic.
static void diclkwrwt(DIC *dic){I n;
 // We are the next writer.  We just wait for read requests to clear.  We could improve this my adapting the wait to the usage pattern
 for(n=5;;--n){
  delay(n<0?50:10)  // delay a bit.  the long delay uses mm_pause.
  if(__atomic_load_n(&(US*)AM(dic),__ATOMIC_ACQUIRE)==0)R;  // wait until reads clear
 }
}


#else
#define DICLKRDRQ(dic) 0
#define DICLKRDWT(dic,lv) 
#define DICLKRDREL(dic) 0

#define DICLKRWRQ(dic) 0
#define DICLKRWWT(dic,lv) 
#define DICLKWRRQ(dic) 0
#define DICLKWRWT(dic,lv) 0
#define DICLKWRRQ(dic,lv)
#endif

// Request resize.  jt->globals has been set to the dic locale, so we look up resize there
A dicresize(DIC dic,J jt){
 A *_ttop=jt->tnextpushp;  // stack restore point
  // call dicresize in the locale of the dic.  No need for explicit locative because locale is protected by dic
 A nam, val;
 ASSERT((nam=nfs(10,"resize",0)!=0)
  &&((val=jtsyrd1((J)((I)jt+NAV(nam)->m),NAV(nam)->s,NAV(nam)->hash,jt->global))!=0  // look up name in current locale and ra() if found
  &&((val=QCWORD(namerefacv(nam,QCWORD(val))))!=0)   // turn the value into a reference, undo the ra
  &&((val&&LOWESTBIT(AT(val))&VERB))),EVVALUE)   // make sure the result is a verb
 A newdic; RZ(newdic=jtunquote(jt,(A)dic,val,val));  // execute resize on the dic, returning new dic
 struct Dic t=dic->bloc; dic->bloc=newdic->bloc; newdic->bloc=t;  // Exchange the parms and data areas from the new dic to the old.  Since they are recursive, this exchanges ownership and will cause the old blocks to be freed when the new dic is.
 // NOTE: we keep the old blocks hanging around until the new have been allocated.  This seems unnecessary for the hashtable, but we do it because other threads still have the old pointers and may prefetch from
 //  the old hash.  This won't crash, but it might be slow if the old hash is no longer in mapped memory
 tpop(_ttop);  // discard newdic and everything it refers to
 R dic;  // always return the same block if no error.
}


// k is A for keys, n is #keys, s is place for slot#s.  Hash each key, store, prefetch (possibly using wrong hash)
// This version works on internal hash functions only
// result is 0 on error
B jtkeyprep(DIC dic, void *k, UI8 n, I8 *s,J jt){UI8 i;
 UI8 hsz=dic->hsizes; I kib=dic->klens; UI4 (*hf)()=dic->hashfn; C *hashtbl=AAV1(dic->hash);  // elesiz/hashsiz kbytelen/kitemlen
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 for(i=n;--i>=0;){
  k=(void*)((I)k-(kib>>32));  // back up to next key
  s[i]=(I8)(*hf)(k,(UI4)kib,jt); PREFETCH(hashtbl[(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56)];
 }
 R 1;
 )
}
// k is A for keys, kvirt is virtual unincorpable block to point to key, s is place for slot#s.  Hash each key, convert to slot, store, prefetch

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy the value to the result
// This version works on internal compare functions only
B jtgetslots(DIC dic,void *k,UI8 n,I8 *s,void *zv,A jt){UI8 i;
 UI8 hsz=dic->hsizes; I kib=dic->klens; C (*cf)()=dic->compfn; C *hashtbl=AAV1(dic->hash);  // elesiz/hashsiz kbytelen/kitemlen
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 C *kbase=CAV(dic->keys)-2*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0 & 1 are empty and tombstone and do not take space in the key array
 // convert the hash slot#s to index into kvs
 for(i=n;--i>=0;){
  k=(void*)((I)k-(kib>>32));  // back up to next key
  I8 curslot=(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56);  // convert hash to slot# and then to byte offset.
  UI8 hval; s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv
  while(unlikely(hval<2) || unlikely((*cf)((UI4)kib,k,kbase+(kib>>32)*hval))){  // loop till we hit a valid value that compares equal on the key
   if(s[i]==0)break;   // if we hit an empty, that ends the search
   if(unlikely((curslot-=(hsz>>56))<0))curslot+=(UI4)hsz*(hsz>>56);  // move to next hash slot, wrap to end if we hit 0
   s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53));  // fetch next hash value
  }
 }
 // copy using the kv indexes we calculated.  Copy in ascending order so we can overstore
 I vn=dic->vbytelen; C *vbase=CAV(dic->vals)-2*vn;  // size of a value; address corresponding to hash value of 0.  Hashvalues 0 & 1 are empty and tombstone and do not take space in the value array
 DO(n, ASSERT(s[i]>1,EVINDEX) MC(zv,vbase+s[i]*vn,vn); zv=(void *)((I)z+vn);)   // scaf JMC?
 R 1;
}

// get.  conjunction.  u is dic, v is hash/comp function.    w is keys
// This version for internal functions only
DF1(jtdicget){A z;
 ARGCHK1(w)
 DIC dic=FAV(self)->fgh[0]; I kt=dic->ktype; I kr=AN(dic->kshape), *ks=AAV1(dic->kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){GA0(z,dic->vtype,0,wf+AN(dic->vshape)) MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AAV1(dic->vshape),AN(dic->vshape)) R z;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0)RZ(w=ccvt(kt,w))   // convert type of w if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 GA0(z,dic->vtype,kn*dic->vaii,wf+AN(dic->vshape)) MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AAV1(dic->vshape),AN(dic->vshape))   // allocate result area & install shape
 I8 s[100]; if(unlikely(kn>sizeof(s)/sizeof(s[0]))){GATV0(z,FL,kn,1) s=(I8*)voidAV1(z);}   // allocate slots block, locally if possible.  FL is always 8 bytes
 void *k=voidAV(w);  // point to the key data
 RZ(jtkeyprep(dic,k,kn,s,jt))  // convert keys to slot#
 I lv=DICLKRDRQ(dic) DICLKRDWT(dic,lv)  // request read lock and wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
 RZ(jtgetslots(dic,k,kn,s,voidAV(z),jt));  // get the values
 DICLKRDREL(dic)  // release read lock
 RETF(z);
}

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
B jtputslots##STN(DIC dic,A k,A kvirt,STX *s,A z,A jt){
 // init empty-slot index
 // this loop will be unrolled 3 times, one for each fetch from remote memory
 // read from the slot
 // prefetch key and value
 // loop till found: compare key[slot] against k[i]
 // if match: save neg. index of value slot
 // if no match: save index of hash slot for re-search; copy kv to empty slot, abort if no slot (resize), advance empty-slot index
}

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
B jtputvalues##STN(DIC dic,A k,A kvirt,STX *s,A z,A jt){
 // init empty-slot index
 // for each slot:
  // if positive (no match) find empty hashslot & point it to empty-slot; advance empty-slot index
  // else copy value to value slot
 // restore empty-slot index, update cardinality
}

// put.  conjunction.  u is dic, v is hash/comp function.  a is values, w is keys
DF2(jtdicput##STN){
 DIC dic=FAV(self)->fgh[0];  // point to dic block
 // allocate slots block
 // if keys are not the right shape & type, convert/extend them
 // if values are not the right shape&type convert/extend them
 // allocate result area
 // allocate virtual block for key
 // allocate virtual block for value
 RZ(jtkeyprep(dic,k,kvirt,s,jt))  // hash kreys & prefetch
 // take prewrite lock;
 // refresh dic info
 z=jtputslots##STN(dic,k,kvirt,s,z,jt);  // classify keys & move new kvs
 // take write lock;
 if(resize requested)R dicresize();
 z=jtputvalues##STN(dic,k,kvirt,s,z,jt);  // finish the values
 // release write lock;
 RETF(z);
}


// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
B jtdelslots##STN(DIC dic,A k,A kvirt,STX *s,A z,A jt){
 // init empty-slot index
 // this loop will be unrolled 3 times, one for each fetch from remote memory
 // read from the slot
 // prefetch key
 // loop till found: compare key[slot] against k[i]
 // if match: save neg. index of value slot
}

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
B jtdelslots##STN(DIC dic,A k,A kvirt,STX *s,A z,A jt){
 // init empty-slot index
 // for each slot:
  // if neg, add key# at base of empty chain
 // restore empty-slot index, update cardinality
 // if (cardinality less than minimum)abort, call for resize
}

// del.  conjunction.  u is dic, v is hash/comp function.  w is keys
DF1(jtdicdel##STN){
 DIC dic=FAV(self)->fgh[0];  // point to dic block
 // allocate slots block
 // if keys are not the right shape & type, convert/extend them
 // allocate virtual block for key
 RZ(jtkeyprep(dic,k,kvirt,s,jt))  // convert keys to slot#
 // take prewrite lock;
 // refresh dic info
 z=jtdelslots##STN(dic,k,kvirt,s,z,jt);  // classify keys & move new kvs
 // take write lock;
 if(resize requested)R dicresize();
 z=jtdelkeys##STN(dic,k,kvirt,s,z,jt);  // finish the deletion
 // release write lock;
 RETF(z);
}

// u 16!:_2  get: u=dic
// We create a verb to handle (get y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicgetc){
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(z,0,CIBEAM,VERB, jtdicget,jtvalenceerr, w,0,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// u 16!:_3  put: u=dic
// We create a verb to handle (x put y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicputc){
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(z,0,CIBEAM,VERB, jtvalenceerr,jtdicput, w,0,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// u 16!:_4 v  del: u=dic
// We create a verb to handle (del y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicdelc){
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(z,0,CIBEAM,VERB, jtdicdel,jtvalenceerr, w,0,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}
#endif