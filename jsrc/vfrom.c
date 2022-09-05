/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: From & Associates. See Hui, Some Uses of { and }, APL87.         */

#include "j.h"


F1(jtcatalog){PROLOG(0072);A b,*wv,x,z,*zv;C*bu,*bv,**pv;I*cv,i,j,k,m=1,n,p,*qv,r=0,*s,t=0,*u;
 F1RANK(1,jtcatalog,DUMMYSELF);
 ASSERT(!ISSPARSE((AT(w))),EVNONCE);
 if((-AN(w)&-(AT(w)&BOX))>=0)R box(w);   // empty or unboxed, just box it
 n=AN(w); wv=AAV(w);
 DO(n, x=C(wv[i]); if(likely(AN(x))){p=AT(x); t=t?t:p; ASSERT(!ISSPARSE(p),EVNONCE); ASSERT(HOMO(t,p),EVDOMAIN); RE(t=maxtype(t,p));});  // use vector maxtype; establish type of result
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

#define SETNDX(ndxvbl,ndxexp,limexp)    {ndxvbl=(ndxexp); if((UI)ndxvbl>=(UI)limexp){ndxvbl+=(limexp); ASSERT((UI)ndxvbl<(UI)limexp,EVINDEX);}}  // if ndxvbl>p, adding p can never make it OK
#define SETNDXRW(ndxvbl,ndxexp,limexp)    {ndxvbl=(ndxexp); if((UI)ndxvbl>=(UI)limexp){(ndxexp)=ndxvbl+=(limexp); ASSERT((UI)ndxvbl<(UI)limexp,EVINDEX);}}  // this version write to input if the value was negative
#define SETJ(jexp) SETNDX(j,jexp,p)

#define IFROMLOOP(T)        \
 {T   * RESTRICT v=(T*)wv,* RESTRICT x=(T*)zv;  \
  if(1==an){v+=j;   DQ(m,                                    *x++=*v;       v+=p; );}  \
  else              DQ(m, DO(an, SETJ(av[i]);                *x++=v[j];);   v+=p; );   \
 }

// a is not boxed and not boolean (except when a is a singleton, which we pass through here to allow a virtual result)
F2(jtifrom){A z;C*wv,*zv;I acr,an,ar,*av,j,k,m,p,pq,q,wcr,wf,wn,wr,*ws,zn;
 F1PREFIP;
 ARGCHK2(a,w);
 // IRS supported but only at infinite rank.  This has implications for empty arguments.
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(unlikely(ar>acr))R rank2ex(a,w,DUMMYSELF,acr,wcr,acr,wcr,jtifrom);  // split a into cells if needed.  Only 1 level of rank loop is used
 // From here on, execution on a single cell of a (on matching cell(s) of w, or all w).  The cell of a may have any rank
 an=AN(a); wn=AN(w); ws=AS(w);
 if(unlikely(!ISDENSETYPE(AT(a),INT)))RZ(a=cvt(INT,a));  // convert boolean or other arg to int
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
// if(an==0 && wn==0 && ws[wf]==0)wcr=wr=0;     defer this pending analysis
 // If w is empty, portions of its shape may overflow during calculations, and there is no data to move (we still need to
 // audit for index error, though).  If w is not empty, there is no need to check for such overflow.  So we split the computation here.
 // Either way, we need   zn: #atoms in result   p: #items in a cell of w
 p=wcr?ws[wf]:1;
 av=AV(a);  // point to the selectors
 I wflag=AFLAG(w);
 if(likely(wn!=0)){
  // For virtual results we need: kn: number of atoms in an item of a cell of w;   
  PROD(k, wcr-1, ws+wf+1);  // number of atoms in an item of a cell
  // Also m: #wcr-cells in w 
  PROD(m,wf,ws); zn=k*m;  DPMULDE(an,zn,zn);
  if(((zn-2)|(wr-2)|-(wf|(wflag&(AFNJA))))>=0){  // zn>1 and not (frame or NJA) and rank>1.  Don't take the time creating a virtual block for a list.  User should use ;.0 for that  NJAwhy
   // result is more than one atom and does not come from multiple cells.  Perhaps it should be virtual.  See if the indexes are consecutive
   I index0 = av[0]; index0+=REPSGN(index0)&p;  // index of first item
   // check the last item before checking the middle.
   I indexn = av[an-1]; indexn+=REPSGN(indexn)&p;
   if(indexn==index0+an-1){
     I indexp=index0; DO(an-1, indexn=av[1+i]; indexn+=REPSGN(indexn)&p; if(indexn!=indexp+1){indexn=p; break;} indexp=indexn;);
   }else indexn=p;
   if((index0|(p-indexn-1))>=0){  // index0>0 and indexn<=p-1
    // indexes are consecutive and in range.  Make the result virtual.  Rank of w cell must be > 0, since we have >=2 consecutive result atoms
    RZ(z=virtualip(w,index0*k,ar+wr-1));
    // fill in shape and number of atoms.  ar can be anything.
    I* as=AS(a); AN(z)=zn; I *s=AS(z); MCISH(s,as,ar) MCISH(s+ar,ws+1,wr-1)
    RETF(z);
   }
  }
  // for copying items, we need    k: size in bytes of an item of a cell of w
  k<<=bplg(AT(w));
 } else {zn=0;}  // No data to move
 // Allocate the result area and fill in the shape
 GA00(z,AT(w),zn,ar+wr-(I )(0<wcr));  // result-shape is frame of w followed by shape of a followed by shape of item of cell of w; start with w-shape, which gets the frame
 MCISH(AS(z),AS(w),wf); MCISH(&AS(z)[wf],AS(a),ar); if(wcr)MCISH(&AS(z)[wf+ar],1+wf+ws,wcr-1);
 if(unlikely(!zn)){DO(an, SETJ(av[i])) R z;}  // If no data to move, just audit the indexes and quit
 // from here on we are moving items
 wv=CAV(w); zv=CAV(z); SETJ(*av);
 switch(k){
 case sizeof(I):
#if C_AVX2
 // moving I/D.  Use GATHER instruction.  Future hardware can exploit that.
 {__m256i endmask=_mm256_setzero_si256(); /* length mask for the last word */ 
  _mm256_zeroupperx(VOIDARG)
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
    indexes0=_mm256_loadu_si256((__m256i*)av);   // fetch a block of indexes
    indexes0=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes0),_mm256_castsi256_pd(_mm256_add_epi64(indexes0,wstride)),_mm256_castsi256_pd(indexes0)));  // get indexes, add axis len if neg
// obsolete     ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes0,_mm256_sub_epi64(indexes0,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
    ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes0,_mm256_sub_epi64(indexes0,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
    if(an>2*NPAR){
     indexes1=_mm256_loadu_si256((__m256i*)(av+NPAR));   // fetch a block of indexes
     indexes1=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes1),_mm256_castsi256_pd(_mm256_add_epi64(indexes1,wstride)),_mm256_castsi256_pd(indexes1)));  // get indexes, add axis len if neg
     ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes1,_mm256_sub_epi64(indexes1,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
     if(an>3*NPAR){
      indexes2=_mm256_loadu_si256((__m256i*)(av+2*NPAR));   // fetch a block of indexes
      indexes2=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes2),_mm256_castsi256_pd(_mm256_add_epi64(indexes2,wstride)),_mm256_castsi256_pd(indexes2)));  // get indexes, add axis len if neg
      ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes2,_mm256_sub_epi64(indexes2,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
     }
    }
   }
   if(an<=4*NPAR){
    indexesn=_mm256_maskload_epi64(av+((an-1)&-NPAR),endmask);   // fetch last block of indexes
    indexesn=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesn),_mm256_castsi256_pd(_mm256_add_epi64(indexesn,wstride)),_mm256_castsi256_pd(indexesn)));  // get indexes, add axis len if neg
    ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
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
    indexesn=_mm256_loadu_si256((__m256i*)(av+3*NPAR));   // fetch last block of indexes
    indexesn=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesn),_mm256_castsi256_pd(_mm256_add_epi64(indexesn,wstride)),_mm256_castsi256_pd(indexesn)));  // get indexes, add axis len if neg
    ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
    do{
     // 17+indexes.  We must read the tail repeatedly
     // this first execution audits the indexes and converts negatives
     _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));  // process the indexes saved in registers
     _mm256_storeu_si256((__m256i*)(x+NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes1,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+2*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes2,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+3*NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesn,ones,SZI));
     I *RESTRICT avv=av+4*NPAR; x+=4*NPAR;  // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
     __m256i indexes;
     if(an>5*NPAR){
      indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
      DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
       __m256i indexesx=indexes;  // fetch a block of indexes
       indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
       anynegindex=_mm256_or_si256(anynegindex,indexesx); indexesx=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesx),_mm256_castsi256_pd(_mm256_add_epi64(indexesx,wstride)),_mm256_castsi256_pd(indexesx)));  // get indexes, add axis len if neg
       ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesx,_mm256_sub_epi64(indexesx,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
       _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesx,ones,SZI)); x+=NPAR;
      )
      anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg
      ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
      _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes,ones,SZI)); x+=NPAR;
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(avv,endmask);  // fetch a block of indexes
     anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg.  unfetched indexes are 0
     ASSERT(_mm256_testc_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))),_mm256_castsi256_pd(ones)),EVINDEX);  // positive, and negative if you subtract axis length
     _mm256_maskstore_epi64(x, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes,endmask,SZI)); x+=((an-1)&(NPAR-1))+1;   // must use a different reg for source and index, lest VS2013 create an illegal instruction
     v+=p;  // advance to next input cell
     --i;
     if(_mm256_testz_pd(_mm256_castsi256_pd(anynegindex),_mm256_castsi256_pd(anynegindex)))break;
    }while(i);
    while(i){
     // this second version comes into play if there were no negative indexes.  If there are negatives we end up auditing them repeatedly, too bad.
     _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes0,ones,SZI));  // process the indexes saved in registers
     _mm256_storeu_si256((__m256i*)(x+NPAR), _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes1,ones,SZI));
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
}    /* a{"r w for numeric a */

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

// a is array whose 1-cells are index lists, w is array
// result is the indexed items
// the numbers in a have been audited for validity
// w is length of the frame
A jtfrombu(J jt,A a,A w,I wf){F1PREFIP;A p,q,z;I ar,*as,h,m,r,*u,*v,wcr,wr,*ws;
 ar=AR(a); as=AS(a); h=as[ar-1];  // h is length of the index list, i. e. number of axes of w that disappear during indexing
 wr=AR(w); ws=AS(w); wcr=wr-wf;
 if((-AN(a)&-AN(w))>=0){  // empty array, either a or w
  // allocate empty result, move in shape: frame of w, frame of a, shape of item
  GA00(z,AT(w),0,wf+(wcr-h)+(ar-1)); MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AS(a),ar-1)  MCISH(AS(z)+wf+ar-1,AS(w)+wf+h,wcr-h)
  R z;
 }
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
}    /* (<"1 a){"r w, dense w, integer array a */

#define AUDITPOSINDEX(x,lim) if(!BETWEENO((x),0,(lim))){if((x)<0)break; ASSERT(0,EVINDEX);}
// a is list of boxes, w is array, wf is frame of operation, *ind will hold the result
// if the opened boxes have contents that are all lists with the same item shape (treating atoms as same as singleton lists), create an array of all the indexes; return that array
// return 0 if error, 1 if the boxes were not homogeneous.
A jtaindex(J jt,A a,A w,I wf){A*av,q,z;I an,ar,c,j,k,t,*u,*v,*ws;
 ARGCHK2(a,w);
 an=AN(a);
 if(!an)R (A)1;
 ws=wf+AS(w); ar=AR(a); av=AAV(a);  q=C(av[0]); c=AN(q);   // q=addr, c=length of first box
 if(!c)R (A)1;  // if first box is empty, return error to revert to the slow way
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 GATV0(z,INT,an*c,1+ar); MCISH(AS(z),AS(a),ar) AS(z)[ar]=c; v=AV(z);  // allocate array for result.  Mustn't copy shape from AS(a) - it overfetches
 for(j=0;j<an;++j){
  q=C(av[j]); t=AT(q);
  if(t&BOX)R (A)1;   // if any contents is boxed, error
  if(!ISDENSETYPE(t,INT))RZ(q=cvt(INT,q));  // if can't convert to INT, error
  if((((c^AN(q))-1)&(AR(q)-2))>=0)R (A)1;   // if not the same length, or rank>1, error
  u=AV(q);
  DO(c, SETNDX(k,u[i],ws[i]) *v++=k;);   // copy in the indexes, with correction for negative indexes
 }
 R z;
}    /* <"1 a to a where a is an integer index array */

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

// n is length of axis, w is doubly-unboxed selector
// result is list of selectors - complementary if w is boxed, with a: standing for axis taken in full
static A jtafi(J jt,I n,A w){A x;
 if((-AN(w)&SGNIF(AT(w),BOXX))>=0)R pind(n,w);   // empty or not boxed
 ASSERT(!AR(w),EVINDEX);  // if boxed, must be an atom
 x=C(AAV(w)[0]);
 R AN(x)?less(IX(n),pind(n,x)):ds(CACE);   // complementary
}

// general boxed a
static F2(jtafrom){PROLOG(0073);A c,ind,p=0,q,*v,y=w;B bb=1;I acr,ar,i=0,j,m,n,pr,*s,t,wcr,wf,wr;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(ar){  // if there is an array of boxes
  if(((ar^acr)|(wr^wcr))==0){RZ(ind=aindex(a,w,wf)); ind=(A)((I)ind&~1LL); if(ind)R frombu(ind,w,wf);}  // if boxing doesn't contribute to shape, open the boxes of a and copy the values
  R wr==wcr?rank2ex(a,w,DUMMYSELF,0L,wcr,0L,wcr,jtafrom):  // if a has frame, rank-loop over a
      df2(p,IRS1(a,0L,acr,jtbox,c),IRS1(w,0L,wcr,jtbox,ind),amp(ds(CLBRACE),ds(COPE)));  // (<"0 a) {&> <"0 w
 }
 c=C(AAV(a)[0]); t=AT(c); SETIC(c,n); v=AAV(c);   // B prob not reqd 
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
}    /* a{"r w for boxed index a */

F2(jtfrom){I at;A z;
 F2PREFIP;
 ARGCHK2(a,w);
 at=AT(a);
 if(likely(!ISSPARSE(AT(a)|AT(w)))){
  // if B01|INT|FL atom { INT|FL|BOX array, and no frame, just pluck the value.  If a is inplaceable and not unincorpable, use it
  // If we turn the result to BOX it will have the original flags, i. e. it will be nonrecursive.  Thus fa will not free the contents, which do not have incremented usecount (and are garbage on error)
  // We allow FL only if it is the same saize as INT
  if(!((AT(a)&(NOUN&~(B01|INT|(SY_64*FL))))+(AT(w)&(NOUN&~(INT|(SY_64*FL)|BOX)))+AR(a)+(SGNTO0((((RANKT)jt->ranks-AR(w))|(AR(w)-1))))+(AFLAG(w)&AFNJA))){   // NJAwhy
   I av;  // selector value
   if(likely(!SY_64||AT(a)&(B01|INT))){av=BIV0(a);  // INT index
   }else{  // FL index
    D af=DAV(a)[0], f=jround(af); av=(I)f;
    ASSERT(ISFTOIOK(f,af),EVDOMAIN);  // if index not integral, complain.  IMAX/IMIN will fail presently.  We rely on out-of-bounds conversion to peg out one side or other (standard violation)
   }
   I wr1=AR(w)-1;
   if(wr1<=0){  // w is atom or list, result is atom
    // Get the area to use for the result: the a input if possible, else an INT atom
    if((SGNIF(jtinplace,JTINPLACEAX)&AC(a)&SGNIFNOT(AFLAG(a),AFUNINCORPABLEX))<0)z=a; else{GAT0(z,INT,1,0)}
    // Move the value and transfer the block-type
    I j; AT(z)=AT(w); SETNDX(j,av,AN(w)); IAV(z)[0]=IAV(w)[j];
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
   A (*fn)(J,A,A);
   fn=jtifrom; fn=at&BOX?jtafrom:fn; fn=at&(AN(a)!=1)?jtbfrom:fn; jtinplace=fn!=jtifrom?jt:jtinplace;  // boxed; boolean (not singleton); other (will be converted to INT)
   z=(*fn)(jtinplace,a,w);
   // Here we transferred out of w.  We must mark w non-pristine.  Since there may have been duplicates, we cannot mark z as pristine.  We overwrite w because it is no longer in use
   PRISTCLRF(w)
  }
 }else if(ISSPARSE(AT(a)&AT(w))){z=fromss(a,w);}  // sparse cases
 else if(ISSPARSE(AT(w))){z=at&BOX?frombs(a,w) : fromis(a,w);}
 else{z=fromsd(a,w);}
 RETF(z);
}   /* a{"r w main control */

F2(jtsfrom){
 if(!ISSPARSE((AT(w)))){
  // Not sparse.  Verify the indexes are numeric and not empty
  if(((AN(a)-1)|((AT(a)&NUMERIC)-1))>=0){A ind;   // a is a numeric array
   // Check indexes for validity; if valid, turn each row into a cell offset
   if(ind=jtcelloffset((J)((I)jt+JTCELLOFFROM),w,a)){
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
 }else{A ind;
  // sparse.  See if we can audit the index list.  If we can, use it, else execute the slow way
  RE(aindex1(a,w,0L,&ind)); if(ind)R frombsn(ind,w,0L);
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

F2(jtfetch){A*av, z;I n;F2PREFIP;
 F2RANKW(1,RMAX,jtfetch,DUMMYSELF);  // body of verb applies to rank-1 a, and must turn pristine off if used higher, since there may be repetitions.
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
// see if row,col is in exclusion list.  Exclusion list is a list of col|col with the smaller value in the high-order part
// Result is 0 if OK to accept the combination
static int notexcluded(I *exlist,I nexlist,I col,I row){I colrow=(col<row)?(col<<32)+row:(row<<32)+col;  // canonicalize column numbers into one value
 while(nexlist--)if(*exlist++==colrow)R 0;
 R 1;
}
 
// everything we need for one core's execution
struct __attribute__((aligned(CACHELINESIZE))) mvmctx {
 // returned section
 US ctxlock;  // used to lock changes
 I abortcolandrow;  // set to -1 normally, but if we have found something that will abort processing it is set.  There are 2 possibilities: unbounded, which has col=-1 and row negative; virtual pivot,
    // which we jump on with row and col set normally.  We set minimp=-inf when we store here, to cut off other threads
 I bestcolandrow[2];  // packed best col (h)/row (l). [0] is for normal pivots or nonimproving pivots, [1] for dangerous pivots
 // shared section
 D minimp;
 I ncolsproc;  // number of columns processed
 I ndotprods;  // total # dotproducts evaluated
 I taskmask;  // a bit for each task as we take it for work
 // the rest is moved into static names
 A ndxa;   // the indexes, always a boxed list of arrays
// obsolete  I nc; I *ndx; I *ndx0; I *ndxe;
 I n;  // #rows/cols in M
 D *bv;  // pointer to bk.  If 0, this is either a column extraction (if zv!=0) or a Dpiv operation (if zv=0)
                  // 0      1       2      3
 __m256d thresh;  // ColThr Inf    bkmin  MinPivot     validity thresholds, small positive values
// obsolete  __m256d oldcol;  // oldcol Frow   0      0            col value at previous smallest pivot / Frow of current col (always <=0)  0 0
// obsolete  __m256d oldbk;   // oldbk  minimp 0      0            bk value at previous smallest pivot, best gain available in a previous col (always <=0) 0 0
 I bestcol; I bestcolrow;  // col# and row#+mask for best value found from previous column, init to no col found, and best value not dangerous
 D *zv; D *Frow;  // pointer to output for product mode, Frow
 D nfreecolsd, ncolsd; D impfac;  // faction of cols to process before we insist on min improvement; min fraction of cols to process (always proc till improvement found); min gain to accept as an improvement after freecols
 I prirow;  // row to give priority to (virtual if any, else _1), or flag to Dpiv
 I *bvgrd0, *bvgrde;  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk  if zv!=0, we process rows in order
 I *exlist, nexlist, *yk;  // exclusion list: list of excluded col|row pairs, length
 D bkmin;  // the largest value for which bk is considered close enough to 0
 I (*axv)[2];  // pointer to ax data
 I *amv0;  // pointer to am block, and to the selected column's indexes
 D *avv0;  // pointer to av data, and to the selected column's values
 A qk;  // original M block
} ;


// the processing loop for one core.  We take a slice of the columns depending on our proc# in the threadpool
// ti is the job#, not used except to detect error
static unsigned char jtmvmsparsex(J jt,void* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(((struct mvmctx*)ctx)->x) x=((struct mvmctx*)ctx)->x;
 YC(ndxa)YC(n)YC(minimp)YC(bv)YC(thresh)YC(bestcol)YC(bestcolrow)YC(zv)YC(Frow)
 YC(impfac)YC(prirow)YC(bvgrd0)YC(bvgrde)YC(exlist)YC(nexlist)YC(yk)YC(bkmin)YC(axv)YC(amv0)YC(avv0)YC(qk)
#undef YC
 // perform the operation

#define PROCESSROWRATIOS /* We run this after the dot-product has been loaded into dotprod */ \
 if(bv!=0){ \
  /* DIP processing.  col data is in dotprod as col col col col */ \
  if(likely(exlist==0)){ /* normal look for improving pivot */  \
   __m256d ratios=_mm256_mul_pd(oldbk,dotprod);  /* ratios is col*oldbk col*minimp 0 0 */ \
   __m256d bks=_mm256_set1_pd(bv[i]);  /* bks = bk bk bk - */ \
   dotprod=_mm256_blend_pd(dotprod,bks,0b0100);  /* dotprod=col col bk col */ \
   __m256d tcond=_mm256_sub_pd(thresh,dotprod); I tmask=_mm256_movemask_pd(tcond); if((tmask&0b0101)==0b0001)goto abortcol;  /* tmask is col>ColThr 0 bk>bkmin col>ColMin; also in tcond; abort, avoiding unbounded, if col>ColThr & bk<=0 */ \
   I imask=i+(tmask<<32);  /* save tmask in the pivot; we care about bit 3 which is 1 if this is not a dangerous pivot.  That goes to bit 35 of limitrow */ \
   ratios=_mm256_fmsub_pd(bks,oldcol,ratios);  /* bk*oldcol-col*oldbk  bk*Frow-col*minimp 0 0  i. e.  1 if new smallest (possibly invalid) pivotratio/0 if abort on limited gain/0 /0 */ \
   tcond=_mm256_and_pd(tcond,ratios);  /* tcond is (new smallest pivotratio & col>ColThr [& bk>bkmin]) 0 0 0   */ \
   oldbk=_mm256_blendv_pd(oldbk,bks,tcond); oldcol=_mm256_blendv_pd(oldcol,dotprod,tcond);  /* if valid new smallest pivotratio, update col and bk where the smallest is found */ \
   I rmask=_mm256_movemask_pd(ratios); /*   rmask is new smallest (possibly invalid) pivotratio   !limited gain 0 0 */ \
   limitrow=(tmask&=1)&rmask?imask:limitrow;   /* col>ColThr & new smallest pivotratio: update best-value variables.  This updates even if bk=0, avoiding unbounded */ \
   if(tmask>(rmask>>1) && (I4)limitrow!=(I4)prirow)goto abortcol;  /* col>ColThr && abort on limited gain: abort UNLESS the current best is on a virtual row.  We give them priority */ \
  }else if(Frow!=0){ /* look for nonimproving pivot */  \
   if(ABS(_mm256_cvtsd_f64(dotprod))>=_mm256_cvtsd_f64(thresh) && notexcluded(exlist,nexlist,*ndx,yk[i])){ndotprods+=bvgrd-bvgrd0+1; minimpfound=1.0; bestcol=*ndx; bestcolrow=i; goto return2;};  /* any nonzero pivot is a pivot row, unless in the exclusion list */ \
  }else{ \
   /* we are looking for a pivot along a col with Frow=0.  We take it if ALL the zero-ck rows have negative c.  Then the pivot will actually move */ \
   D bk=bv[i]; D c=_mm256_cvtsd_f64(dotprod); \
   if(bkle0){  /* now we can count the number of new 0s, deducting it from new0ct */ \
    if(bk>=bkmin){bkle0=0; if(unlikely(bvgrd==bvgrd0))goto abortcol; /* if there were no negative bk, we can do nothing useful here */ \
    }else if(c>-_mm256_cvtsd_f64(thresh)){goto abortcol;  /* if any c nonneg when bk<=0, can't move, abort  */ \
    } \
   } \
   if(!bkle0){  /* find the smallest pivot ratio, & the frequency thereof */ \
    if(c>_mm256_cvtsd_f64(thresh)){  /* eligible pivot.  Compare pivot ratios bk/c */ \
     if(bkold*c>bk*cold){  /* bkold-(bk/c)*cold > 0: smaller pivot ratio.  c is positive */ \
      bkold=bk; cold=c; limitrow=i; \
     } \
    }else if(c>0)goto abortcol;  /* possible dangerous pivot: skip the column */ \
   } \
  } \
 }else if(zv==0){limitrow+=_mm256_cvtsd_f64(thresh)<=_mm256_cvtsd_f64(dotprod);  /* counting eligibles for Dpiv */ \
 }else{zv[i]=_mm256_cvtsd_f64(dotprod); /* just fetching the column: do it */ \
 }
 __m256d oldcol;  // oldcol Frow   0      0            col value at previous smallest pivot / Frow of current col (always <=0)  0 0
 __m256d oldbk;   // oldbk  minimp 0      0            bk value at previous smallest pivot, best gain available in a previous col (always <=0) 0 0
 D minimpfound=0.0;  // minimum (=best) improvement found so far from a non-dangerous pivot
 I ndotprods=0;  // number of dot-products we perform here

 // Figure out which task to take.  We use the core/thread#, and if somehow that task has been taken we take any other free one
 I taskno=jt->ndxinthreadpool;  // default task# to try
 if(unlikely(taskno>=AN(ndxa)))taskno=taskno%AN(ndxa);  // if more processors than tasks, take a value in range
 I trymask=1LL<<taskno;  // mask we will try to take
 D *mv0=DAV(qk);  // pointer to 
 while(1){
  I oldmask=__atomic_fetch_or(&((struct mvmctx*)ctx)->taskmask,trymask,__ATOMIC_ACQ_REL);  // try to reserve a task
  if(!(oldmask&trymask))break;  // if we reserved it, continue
  trymask=1LL<<(taskno=CTTZI(~oldmask));  // advance to the lowest 0 bit - an untaken task
 }
 ndxa=AAV(ndxa)[taskno];  // we have reserved task taskno; point to its indexes
 I *ndx0=IAV(ndxa), *ndx=ndx0, nc=AN(ndxa), *ndxe=ndx+nc; // pointer to column numbers in the order processed, and end+1, and #cols
 I nfreecols=(I)(((struct mvmctx*)ctx)->nfreecolsd*nc); I ncols=(I)(((struct mvmctx*)ctx)->ncolsd*nc);  // the prefix and total fractions are fractions of the size & must be adjusted
 if(unlikely(nc==0))R 0;  // abort with no action if no columns (possible only for DIP)
 if(bv!=0&&prirow>=0)zv=0;  // if we have DIP with a priority row, signal to process ALL rows in bk order.  It's not really needed but it ensures that if the prirow is tied for pivot in the first
                               // column, we will take it

 do{
  // start of processing one column
  I colx=*ndx;  // get next column# to work on
  I limitrow;  // the best row to use as a pivot for this column; or # qualifying Dpiv found.
  if(likely(bv!=0)){
   if(exlist==0){
    // col init
    oldcol=_mm256_set_pd(0.0,0.0,Frow[colx],0.0);  //  init to pivotratio=inf and gain 0, assuming minimp is 0 the first time
    oldbk=_mm256_set_pd(0.0,0.0,minimp,1.0);
   }
   limitrow=-1;  // init no eligible row found
  }else limitrow=0;  // for Dpiv, init to none found
#if 0
 I epcol=AR(qk)==3;  // flag if we are doing an extended-precision column fetch  scaf move out of loop, and into limitrow
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);  // scaf move out of loop
 __m256i rowstride=_mm256_set1_epi64x(n);  // number of Ds in a row of M, in each lane
  // init for the column
  __m256i endmask=_mm256_cmpeq_epi64(sgnbit,sgnbit); /* length mask for the last word */ 
  __m256i indexes, rownums;  // row numbers and atom indexes for the rows we are fetching
  I *bvgrd=bvgrd0; I i=-1; D bkold=inf, cold=1.0; I bkle0=1;
  if(bv==0&&zv!=0){  // one-column mode: gather offsets address successive rows
..
  }
  // create the column NPAR values at a time
  do{
   __m256i indexes;  // indexes for the rows we are fetching
   __m256d dotproducth,dotproductl;  // where we build the column value
   // get the validity mask for the gather and process: leave as ones until the end of the column
   if(unlikely(bvgrde-bvgrd<NPAR))endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-(bvgrde-bvgrd)));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
   // if DIP/Dpiv, fetch the row#s of the next group to process, in bkg order
   if(bv!=0||zv==0){
..
   }else{
    rownums=_mm256_add_epi64(rownums,_mm256_sll_epi64(rowstride,LGNPAR));  // advance NPAR rows to next sequential values
   }
   indexes=_mm256_mul_epu32(rowstride,rownums);
   // Now mv and indexes are set up to read the correct rows
   // get the next NPAR values by dot-product with Av
   if(colx<n){
    // fetching from the Ek matrix itself.  Just fetch the values from the column
    dotproducth=_mm256_setzero_pd(); dotproducth=_mm256_mask_i64gather_pd(qkvh,mv0+colx,indexes,endmask,SZI);
    if(epcol){dotproductl=_mm256_setzero_pd(); dotproductl=_mm256_mask_i64gather_pd(qkvh,mv0+n*n+colx,indexes,endmask,SZI);}
   }else{
    // fetching from A.  Form Ek row . A column for each of the 4 rows
    I an=axv[colx][1];  // number of sparse atoms in each row
    D *vv=avv0+axv[colx][0];  // pointer to values for this section of A
    I *iv=amv0+axv[colx][0];  // pointer to row numbers of the values in *vv (these are the columns we fetch in turn from Ek)
    if(likely(!epcol)){
     // single-precision accumulate
     dotproducth=_mm256_setzero_pd();
     I k;
     NOUNROLL for(k=0;k<an;++k){
      dotproductl=_mm256_setzero_pd(); dotproductl=_mm256_mask_i64gather_pd(qkvh,mv0+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
      dotproducth=_mm256_fmadd_pd(dotproductl,_mm256_set1_pd(vv[k]),dotproducth);  // accumulate the dot-product
     }
    }else{
     // extended-precision accumulate
     __m256d th,tl,tl2,vval;  // temps for value loaded, and multiplier from A column
     if(likely(an!=0){
      // get column number to fetch; fetch 4 rows
      th=_mm256_setzero_pd(); th=_mm256_mask_i64gather_pd(qkvh,mv0+iv[0],indexes,endmask,SZI);  // fetch from up to 4 rows
      tl=_mm256_setzero_pd(); tl=_mm256_mask_i64gather_pd(qkvh,mv0+n*n+iv[0],indexes,endmask,SZI);  // fetch from up to 4 rows
      vval=_mm256_set1_pd(vv[0]);  // load column value
      // initialize the dotproduct with the first product
      TWOPROD(th,vval,dotproducth,tl2)  // high qk * col
      tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add in extension of prev product
      TWOADD(dotproducth,tl2,dotproducth,dotproductl)  // combine high & low
      I k;
      for(k=1;k<an;++k){  // for each other element of the dot-product
       // get column number to fetch; fetch 4 rows
       th=_mm256_setzero_pd(); th=_mm256_mask_i64gather_pd(qkvh,mv0+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
       tl=_mm256_setzero_pd(); tl=_mm256_mask_i64gather_pd(qkvh,mv0+n*n+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
       vval=_mm256_set1_pd(vv[0]);  // load column value
       // accumulate the dot-product
       TWOPROD(th,vval,th,tl2)  // high qk * col
       tl2=_mm256_fmadd_pd(tl,vval,tl2);  // low qk*col, and add in extension of prev product
       TWOADD(dotproducth,th,dotproducth,vval) tl2=_mm256_add_pd(vval,tl2);  // add high parts & accum extension
       TWOADD(dotproducth,tl2,dotproducth,dotproductl)  // combine high & extension for final form
      }
     }else{dotproducth=dotproductl=_mm256_setzero_pd();}  // no columns should not occur.  Just 1 shouldn't either
    }
   }
   // process the NPAR generated values
   if(bv==0&&zv!=0){
    // one-column mode: just store out the values
..
   }else{
    // DIP/Dpiv mode: process each value in turn.  Since 0 values are never pivots, we can stop when all remaining values are 0
    indexes=rownums;  // repurpose indexes to hold the row-number we are working on
    while(_mm256_movemask_pd(_mm256_cmp_pd(dotproducth,_mm256_setzero_pd(),_CMP_EQ_OQ))!=0xf)){
     dotprod=_mm256_permute4x64_pd(dotproducth,0b00000000);  // copy next value into all lanes
     dotproducth=_mm256_permute4x64_pd(dotproducth,0b11111001); dotproducth=_mm256_blend_pd(dotproducth,_mm256_setzero_pd(),0b1000) // shift down one value for next time
     i=_mm256_extract_epi64(indexes,0); indexes=_mm256_permute4x64_pd(indexes,0b11111001); // get the row number we are trying to swap out; shift row number down for next loop
     PROCESSROWRATIOS
    }
   }
  }
  }while((bvgrd+=NPAR)<bvgrde);
#else
#define COLLPINIT I *bvgrd=bvgrd0; I i=-1; D *mv=mv0-n; D bkold=inf, cold=1.0; I bkle0=1;
#define COLLP do{if(unlikely(zv!=0)){++i; mv+=n;}else{i=*bvgrd; mv=mv0+n*i;} // for each row, i is the row#, mv points to the beginning of the row of M.  If we take the whole col, take it in order for cache.  Prefetch next row?
#define COLLPE }while(++bvgrd!=bvgrde);
  COLLPINIT
  // if the column is just to be fetched from M, do so without dot-product.  We can use gather down the column, but there's no gain
  __m256d dotprod;  // place where product is assembled or read into
  if(colx<n){  // scalar index list, means the column is an identity column.  Fetch it directly
   COLLP dotprod=_mm256_set1_pd(mv[colx]);   // fetch the column value into all 4 lanes
    PROCESSROWRATIOS
   COLLPE
  }else{
   // here for sparse dot-product.
// obsolete    colx-=n;  // the offset into A starts after the identity prefix
   // look up column info, get A values and row numbers
   I an=axv[colx][1];  // number of sparse atoms in each row
   D *vv=avv0+axv[colx][0];  // pointer to values for this section of A
   I *iv=amv0+axv[colx][0];  // pointer to row numbers of the values in *vv
   __m256i endmask=_mm256_setzero_si256(); /* length mask for the last word */ 
   _mm256_zeroupperx(VOIDARG)
   __m256i ones=_mm256_cmpeq_epi64(endmask,endmask);  // mask to use for gather into all bytes - set this way so compiler assigns a register
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-an)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
   // Load the first 16 indexes and v-values into registers, resolving negative indexes
   __m256i indexes0, indexes1, indexes2, indexesn;  // indexes, the last may be partial
   __m256d vvals0,vvals1,vvals2,vvalsn;  // the sparse vector, the last may be partial
   if(an>NPAR){
    indexes0=_mm256_loadu_si256((__m256i*)iv);   // fetch a block of indexes
    vvals0=_mm256_loadu_pd(vv);   // fetch a block of values
    if(an>2*NPAR){
     indexes1=_mm256_loadu_si256((__m256i*)(iv+NPAR));   // fetch a block of indexes
     vvals1=_mm256_loadu_pd(vv+NPAR);   // fetch a block of indexes
     if(an>3*NPAR){
      indexes2=_mm256_loadu_si256((__m256i*)(iv+2*NPAR));   // fetch a block of indexes
      vvals2=_mm256_loadu_pd(vv+2*NPAR);   // fetch a block of indexes
     }
    }
   }
   // indexes and values loaded, now do the dot-product
   if(an<=4*NPAR){
    // <=16 indexes.  We stay in registers
    indexesn=_mm256_maskload_epi64(iv+((an-1)&-NPAR),endmask);   // fetch last block of indexes
    vvalsn=_mm256_maskload_pd(vv+((an-1)&-NPAR),endmask);   // fetch last block of values
    // Now do the operation.  With <16 elements it doesn't help to use dual accumulators
 #define FINISHDOTPROD dotprod=_mm256_add_pd(dotprod,_mm256_permute4x64_pd(dotprod,0b01001110)); dotprod=_mm256_add_pd(dotprod,_mm256_permute_pd(dotprod,0b0101));   /* combine accumulators horizontally  01+=23, 0+=1: 4 copies */ \
    PROCESSROWRATIOS 

    // choose a processing loop based on # indexes.  Invalid parts are 0 and can go into the dot-product
    if(an<=NPAR){
     COLLP dotprod=_mm256_mul_pd(vvalsn,_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexesn,_mm256_castsi256_pd(endmask),SZI));
      FINISHDOTPROD
     COLLPE
    }else if(an<=2*NPAR){
     COLLP dotprod=_mm256_mul_pd(vvals0,_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes0,_mm256_castsi256_pd(ones),SZI));
        dotprod=_mm256_fmadd_pd(vvalsn, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexesn,_mm256_castsi256_pd(endmask),SZI),dotprod);
        FINISHDOTPROD
     COLLPE
    }else if(an<=3*NPAR){
     COLLP dotprod=_mm256_mul_pd(vvals0,_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes0,_mm256_castsi256_pd(ones),SZI));
        dotprod=_mm256_fmadd_pd(vvals1, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
        dotprod=_mm256_fmadd_pd(vvalsn, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexesn,_mm256_castsi256_pd(endmask),SZI),dotprod);
        FINISHDOTPROD
     COLLPE
    }else{
     COLLP dotprod=_mm256_mul_pd(vvals0,_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes0,_mm256_castsi256_pd(ones),SZI));
        dotprod=_mm256_fmadd_pd(vvals1, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
        dotprod=_mm256_fmadd_pd(vvals2, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes2,_mm256_castsi256_pd(ones),SZI),dotprod);
        dotprod=_mm256_fmadd_pd(vvalsn, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexesn,_mm256_castsi256_pd(endmask),SZI),dotprod);
        FINISHDOTPROD
      COLLPE
    }
   }else{
    // 17+indexes.  We must read the tail repeatedly.  It might gain a bit to have two accumulators but we don't yet.
    indexesn=_mm256_loadu_si256((__m256i*)(iv+3*NPAR));   // fetch last block of indexes
    vvalsn=_mm256_loadu_pd(vv+3*NPAR);   // fetch last block of values
    COLLP 
     // We don't audit the indexes, which must be positive
     dotprod=_mm256_mul_pd(vvals0,_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes0,_mm256_castsi256_pd(ones),SZI));
     dotprod=_mm256_fmadd_pd(vvals1, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
     dotprod=_mm256_fmadd_pd(vvals2, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes2,_mm256_castsi256_pd(ones),SZI),dotprod);
     dotprod=_mm256_fmadd_pd(vvalsn, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexesn,_mm256_castsi256_pd(ones),SZI),dotprod);
     I *RESTRICT ivv=iv+4*NPAR; D *RESTRICT vvv=vv+4*NPAR;     // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
     __m256i indexes; __m256d vvals;
     if(an>5*NPAR){
      indexes=_mm256_loadu_si256((__m256i*)ivv); ivv+=NPAR;  // fetch a block of indexes
      vvals=_mm256_loadu_pd(vvv); vvv+=NPAR;  // fetch a block of the vector
      DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
       __m256i indexesx=indexes; __m256d vvalsx=vvals;  // 
       indexes=_mm256_loadu_si256((__m256i*)ivv); ivv+=NPAR;  // fetch a block of indexes
       vvals=_mm256_loadu_pd(vvv); vvv+=NPAR;  // fetch a block of the vector
       dotprod=_mm256_fmadd_pd(vvalsx, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexesx,_mm256_castsi256_pd(ones),SZI),dotprod);
      )
      dotprod=_mm256_fmadd_pd(vvals, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes,_mm256_castsi256_pd(ones),SZI),dotprod);
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(ivv,endmask);  // fetch a block of indexes
     vvals=_mm256_maskload_pd(vvv,endmask);   // fetch a block of indexes
     dotprod=_mm256_fmadd_pd(vvals, _mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv,indexes,_mm256_castsi256_pd(endmask),SZI),dotprod);
     FINISHDOTPROD
    COLLPE
   }  // end 'long product'
  } // end 'needed dot-product'
#endif
  // done with one column.  Collect stats and update parms for the next column
  if(bv==0){  // one product, or Dpiv
   if(zv)break;  // if just one product, skip the setup for next column
   // here for Dpiv.  limitrow+1 is the # pivots found; we add/sub/store that in the Dpiv block based on 'prirow'
   exlist[*ndx]=exlist[*ndx]*!!prirow+limitrow*(prirow|1);  //  add/sub/init Dpiv value.  Only one thread ever touches a column 
  }else if(exlist==0){  // looking for nonimproving pivots?
   // not looking for nonimproving pivots.  Do a normal pivot-pick
   // column ran to completion.  Detect unbounded
   if(limitrow<0){bestcolrow=limitrow; bestcol=*ndx; goto return4;}  // no pivots found for a column, problem is unbounded, indicate which column in the NTT, i. e. the input value which is an identity column if < #A
          // but if we are looking for nonimproving pivots, we don't use F or bk and this is invalid
   // The new column must have better gain than the previous one (since we had a pivot & didn't abort).  Remember where it was found and its improvement, unless it is dangerous.  Bit 35 set if NOT dangerous
   // Exception: if a nondangerous pivot pivots out a virtual row, we accept it immediately and abort all other processing
   if(likely((bestcol|SGNIF(limitrow,32+3))<0)){  // if this pivot is not dangerous, remember it.  Also remember if it is the first pivot, in case there are only dangerous pivots
    bestcol=*ndx; bestcolrow=limitrow;  // update the found position for any nondangerous pivot, or the first time in case we have only dangerous pivots
    // update the best-gain-so-far to the actual value found - but only if this is not a dangerous pivot.  We don't want to cut off columns that are beaten only by a dangerous pivot
    if(likely((limitrow&(1LL<<32+3))!=0)){  // if the improvement is one we are content with...
     // if the pivot was found in a virtual row, stop looking for other columns and take the one that gets rid of the virtual row.  But not if dangerous.
     if(unlikely((I4)limitrow==(I4)prirow)){++ndx; ndotprods+=bvgrd-bvgrd0; goto return4;}  // stop early if virtual pivot found, but take credit for the column we process here
     I incumbentimpi=__atomic_load_n((I*)&(((struct mvmctx*)ctx)->minimp),__ATOMIC_ACQUIRE);  // load incumbent best value
     while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
      minimpfound=(Frow[*ndx]*_mm256_cvtsd_f64(oldbk))/_mm256_cvtsd_f64(oldcol);  // this MUST be nonzero, but it decreases in magnitude till we find the smallest pivotratio.  This updates our local best
      if(minimpfound>=*(D*)&incumbentimpi)break;  // if not new global best, don't update global
      if(__atomic_compare_exchange_n((I*)&(((struct mvmctx*)ctx)->minimp),&incumbentimpi,*(I*)&minimpfound,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))break;  // write; if written exit loop, otherwise reload incumbent
     }
     zv=0;  // now that we have an improvement to shoot at, we will benefit by scanning cols in bk order
    }
   }
   --bvgrd;  // undo the +1 in the product-accounting below
abortcol:  // here if column aborted early, possibly on insufficient gain
   if(unlikely(!bv))break;  // if just one product, skip the setup for next column
   ndotprods+=bvgrd-bvgrd0+1;  // accumulate # products performed, including the one we aborted out of
   if(unlikely((--ncols<0)&&(SGNIF(bestcolrow,32+3)<0))){++ndx; break;}  // quit if we have made a non-dangerous improvement AND have processed enough columns - include the aborted column in the count
   // prepare for next column
   *(I*)&minimp=__atomic_load_n((I*)&(((struct mvmctx*)ctx)->minimp),__ATOMIC_ACQUIRE);  // update to find best improvement in any thread, to allot cutoff
   if(unlikely(minimp==infm))break;  // if improvement has been pegged at limit, it's a signal to stop
   if(unlikely(--nfreecols<0))minimp*=impfac;  // for the first cols, accept any improvement; after that, insist on more
  }else{
   // we are looking for nonimproving pivots.  Skip all the gain accounting, and just remember how many products we did
   ndotprods+=bvgrd-bvgrd0;  // accumulate # products performed
   if(Frow==0){
    // accept a zero-Frow pivot if it exists, has more non0 than 0, and is not excluded
    if(limitrow>=0 && notexcluded(exlist,nexlist,*ndx,yk[limitrow])){
     minimpfound=1.0; bestcol=*ndx; bestcolrow=limitrow; goto return2;
    }
   }
  }
 }while(++ndx!=ndxe);  // end loop over columns
 if(bv==0)R 0;  // if Dpiv or single column, ctx is unused for return
 // operation complete; transfer results back to ctx.  To reduce stores we jam the col/row together
 __atomic_fetch_add(&((struct mvmctx*)ctx)->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 __atomic_fetch_add(&((struct mvmctx*)ctx)->ncolsproc,ndx-ndx0,__ATOMIC_ACQ_REL);  // ...and # columns inspected
 // We store dangerous & normal pivots separately.  Dangerous pivots we take as they come (because their improvement has not been stored);
 // for normal pivots we store only if our best improvement is the best encountered so far
 I bestcolandrow=(bestcol<<32)|(UI4)bestcolrow;  // the pivot we found, with flags removed
 if(unlikely((bestcol|SGNIF(bestcolrow,32+3))>=0)){((struct mvmctx*)ctx)->bestcolandrow[1]=bestcolandrow;  // if pivot found and dangerous, just store it willy-nilly
 }else{
  // normal pivot.  We want to pick the best.  We store our best pivot only if it is the best seen in any thread so far.  Unfortunately we need a lock for that
  // We don't know for sure we found any pivot; if not we will certainly not show an improvement
  if(minimpfound==((struct mvmctx*)ctx)->minimp){  // check outside of lock for perf
   WRITELOCK(((struct mvmctx*)ctx)->ctxlock)
   if(minimpfound==((struct mvmctx*)ctx)->minimp)((struct mvmctx*)ctx)->bestcolandrow[0]=bestcolandrow;  // if we have the best gain, report where we found it
   WRITEUNLOCK(((struct mvmctx*)ctx)->ctxlock)
  }
 }
 R 0;
return2:  // here we found a nonimproving pivot.  Save it, possibly overwriting a value stored in a different thread
 __atomic_fetch_add(&((struct mvmctx*)ctx)->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 __atomic_fetch_add(&((struct mvmctx*)ctx)->ncolsproc,ndx-ndx0,__ATOMIC_ACQ_REL);  // ...and # columns inspected
 ((struct mvmctx*)ctx)->minimp=1.0;  // flag that we found a nonimproving pivot
 ((struct mvmctx*)ctx)->bestcolandrow[0]=(bestcol<<32)|(UI4)bestcolrow;  // the pivot we found
 R 0;  // scaf
return4:  // we have a preemptive result.  store it in abortcolandrow, and set minimp =-inf to cut off all threads
 // the possibilities are unbounded and pivoting out a virtual.  We indicate unbounded by row=-1.  We may overstore another thread's result; that's OK
 __atomic_fetch_add(&((struct mvmctx*)ctx)->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 __atomic_fetch_add(&((struct mvmctx*)ctx)->ncolsproc,ndx-ndx0,__ATOMIC_ACQ_REL);  // ...and # columns inspected
 ((struct mvmctx*)ctx)->minimp=infm;  // shut off other threads
 ((struct mvmctx*)ctx)->abortcolandrow=(bestcol<<32)|(UI4)bestcolrow;  // set unbounded col or col/row of virtual pivot
 R 0;
}
#endif //C_AVX2

// 128!:9 matrix times sparse vector with optional early exit
// product mode:
//  y is ndx;Ax;Am;Av;(M, shape m,n)  where ndx is an atom
//  if ndx<m, the column is ndx {"1 M; otherwise ((((ndx-m){Ax) ];.0 Am) {"1 M) +/@:*"1 ((ndx-m){Ax) ];.0 Av
//   if M has rank 3 (with 2={.$M), do the product in extended precision
//  Result for product mode (exitvec is scalar) is the product, one column of M
// DIP/Dpiv mode:
// y is ndx;Ax;Am;Av;(M, shape m,n);bkgrd;(ColThreshold/PivTol,MinPivot,bkmin,NFreeCols,NCols,ImpFac,Virtx/Dpivdir);bk/'';Frow[;exclusion list/Dpiv;Yk]
// Result is rc,best row,best col,#cols scanned,#dot-products evaluated,best gain  (if rc e. 0 1 2)
//           rc,failing column of NTT, an element of ndx (if rc=4)
//  rc=0 is good; rc=1 means the pivot found is dangerously small; rc=2 nonimproving pivot found; rc=3 no pivot found, stall; rc=4 means the problem is unbounded (only the failing column follows)
//  rc=5 (not created - means problem is infeasible) rc=6=empty M, problem is malformed
// if the exclusion list is given, we stop on the first nonimproving pivot, and the exclusion list is used to prevent repetition of basis
// If Frow is empty, we are looking for nonimproving pivots in rows where the selector is 0.  In that case the bkgrd puts the bk values in descending order.  We return the first column that will make more 0 B rows non0 than non0 B rows 0.
// If bk is empty, we are looking in bkgrd columns and counting the #places where c>=PivTol and accumulating into Dpiv under control of Dpivdir (-1=decr, 1=incr; init to 0 if neg)
// Rank is infinite
F1(jtmvmsparse){PROLOG(832);
#if C_AVX2
 ASSERT(AR(w)==1,EVRANK);
 ASSERT(AN(w)>=5,EVLENGTH);  // audit overall w
 ASSERT(AT(w)&BOX,EVDOMAIN);
 // check ranks
 ASSERT(AR(C(AAV(w)[0]))<=1,EVRANK);  // ndx
 ASSERT(AR(C(AAV(w)[1]))==3&&AS(C(AAV(w)[1]))[1]==2&&AS(C(AAV(w)[1]))[2]==1,EVRANK);  // Ax, shape cols,2 1
 ASSERT(AR(C(AAV(w)[2]))==1,EVRANK);  // Am
 ASSERT(AR(C(AAV(w)[3]))==1,EVRANK);  // Av
 ASSERT(AR(C(AAV(w)[4]))==2||(AR(C(AAV(w)[4]))==3&&AR(C(AAV(w)[0]))==0),EVRANK);  // M
 // abort if no columns
 if(AN(C(AAV(w)[0]))==0)R num(6);  // if no cols (which happens at startup, return error indic)
 // check types.  Don't convert - force the user to get it right
 ASSERT(AT(C(AAV(w)[1]))&INT,EVDOMAIN);  // Ax, shape cols,2 1
 ASSERT(AT(C(AAV(w)[2]))&INT,EVDOMAIN);  // Am
 ASSERT(AT(C(AAV(w)[3]))&FL,EVDOMAIN);  // Av
 ASSERT(AT(C(AAV(w)[4]))&FL,EVDOMAIN);  // M
 // check agreement
 ASSERT(AS(C(AAV(w)[2]))[0]==AS(C(AAV(w)[3]))[0],EVLENGTH);   // Am and Av
 ASSERT(AS(C(AAV(w)[4]))[AR(C(AAV(w)[4]))-2]==AS(C(AAV(w)[4]))[AR(C(AAV(w)[4]))-1],EVLENGTH);   // M is square

 // indexes must be an atom, a single list of integers, or a list of boxes containing integers
 // we don't allow conversion so as to force the user to get it right, for speed
 A ndxa=C(AAV(w)[0]); ASSERT(AT(ndxa)&BOX+INT,EVDOMAIN);
 if(likely(AT(ndxa)&BOX)){  // if list of boxes, ensure each holds a list of integers, possibly empty
  I ncols=0;
  DO(AN(ndxa), ASSERT(AN(AAV(ndxa)[i])==0||(AT(AAV(ndxa)[i])&INT),EVDOMAIN)  ASSERT(AR(AAV(ndxa)[i])<=1,EVDOMAIN) ncols+=AN(AAV(ndxa)[i]); )
  if(ncols==0)R num(6);  // if no cols (which happens at startup, return error indic)
 }
 if(unlikely(AT(ndxa)&INT)){A t; GAT0(t,BOX,1,0) AAV0(t)[0]=ndxa; ndxa=t;} // if ndxa is integer list, make it a list of one box
 ASSERT(AN(ndxa)<=MAXTHREADSINPOOL+1,EVLIMIT)   // if we don't have a bit in trymask for each task, that's too many tasks
// obsolete  ASSERT(AN(ndxa)!=0,EVLENGTH); if(!(AT(ndxa)&INT))RZ(ndxa=cvt(INT,ndxa));
 // extract pointers to tables
 D minimp=0.0;  // (always neg) min improvement we will accept, best improvement in any column so far.  Init to 0 so we take first column with a pivot

 I n=AS(C(AAV(w)[4]))[1];  // n=#rows/cols in M
 // convert types as needed; set ?v=pointer to data area for ?
 D *bv; // pointer to b values if there are any
 __m256d thresh;  // ColThr Inf    bkmin  MinPivot     validity thresholds, small positive values
 I bestcol=1LL<<(BW-1), bestcolrow=0;  // col# and row#+mask for best value found from previous column, init to no col found, and best value 'dangerous or not found'
// obsolete 1LL<<(32+3);
 A z; D *zv; D *Frow;  // pointer to output for product mode, Frow
 D nfreecolsd, ncolsd; D impfac;  // number of cols to process before we insist on min improvement; min number of cols to process (always proc till improvement found); min gain to accept as an improvement after freecols
 I prirow;  // row to get priority (virtual), or flag to Dpiv
 I *bvgrd0, *bvgrde;  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk  if zv!=0, we process rows in order
 I *exlist=0, nexlist, *yk;  // exclusion list: list of excluded col|row pairs, length
 D bkmin;  // the largest value for which bk is considered close enough to 0

 if(AR(C(AAV(w)[0]))==0){
  // single index value.  set bv=0, zv non0 as a flag that we are storing the column
  bv=0; ASSERT(AN(w)==5,EVLENGTH);  // if goodvec is an atom, set bv=0 to indicate that bv is not used and verify no more input
  if(unlikely(n==0)){R reshape(drop(num(-1),shape(C(AAV(w)[4]))),zeroionei(0));}   // empty M, each product is 0
  I epcol=AR(C(AAV(w)[4]))==3;  // flag if we are doing an extended-precision column fetch
  GATV(z,FL,n<<epcol,1+epcol,AS(C(AAV(w)[4]))); zv=DAV(z);  // allocate the result area for column extraction.  Set zv nonzero so we use bkgrd of i. #M
  bvgrd0=0; bvgrde=bvgrd0+n;  // length of column is #M
 }else{
  // A list of index values.  We are doing the DIP calculation or Dpiv
  ASSERT(AR(C(AAV(w)[5]))==1,EVRANK); ASSERT(AN(C(AAV(w)[5]))==0||AT(C(AAV(w)[5]))&INT,EVDOMAIN); bvgrd0=IAV(C(AAV(w)[5])); bvgrde=bvgrd0+AN(C(AAV(w)[5]));  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk
  if(AN(C(AAV(w)[5]))==0){RETF(num(6))}  // empty bk - give error/empty result 6
  ASSERT(BETWEENC(AN(w),8,11),EVLENGTH); 
  ASSERT(AR(C(AAV(w)[8]))<=1,EVRANK);   // Frow, one per row of M and column of A
  if(AN(C(AAV(w)[8]))==0)Frow=0;else{ASSERT(AT(C(AAV(w)[8]))&FL,EVDOMAIN); ASSERT(AN(C(AAV(w)[8]))==AS(C(AAV(w)[4]))[0]+AS(C(AAV(w)[1]))[0],EVLENGTH); Frow=DAV(C(AAV(w)[8]));}  // if Frow omitted we are looking to make bks nonzero
  ASSERT(AR(C(AAV(w)[6]))<=1,EVRANK); ASSERT(AT(C(AAV(w)[6]))&FL,EVDOMAIN); ASSERT(AN(C(AAV(w)[6]))==7,EVLENGTH);  // 7 float constants
  if(unlikely(n==0)){RETF(num(6))}   // empty M - should not occur, give error result 6
  bkmin=DAV(C(AAV(w)[6]))[2];
  thresh=_mm256_set_pd(DAV(C(AAV(w)[6]))[1],bkmin,inf,DAV(C(AAV(w)[6]))[0]); nfreecolsd=(DAV(C(AAV(w)[6]))[3]); ncolsd=(DAV(C(AAV(w)[6]))[4]); impfac=DAV(C(AAV(w)[6]))[5]; prirow=(I)DAV(C(AAV(w)[6]))[6];
  ASSERT(AR(C(AAV(w)[7]))<=1,EVRANK);
  if(AN(C(AAV(w)[7]))!=0){
   // Normal DIP calculation
   ASSERT(AT(C(AAV(w)[7]))&FL,EVDOMAIN); ASSERT(AN(C(AAV(w)[7]))==AS(C(AAV(w)[4]))[0],EVLENGTH); bv=DAV(C(AAV(w)[7]));  // bk, one per row of M
   zv=AN(C(AAV(w)[5]))==AN(C(AAV(w)[7]))?Frow:0;  // set zv nonzero as a flag to process leading columns in order, until we have an improvement to shoot at.  Do this only if ALL values in bk are to be processed
   if(AN(w)>9){
    // nonimproving pivots with exlist
    ASSERT(AN(w)==11,EVLENGTH); 
    // An exclusion list is given (and thus also yk).  Remember their addresses.  Its presence puts us through the 'nonimproving path' case
    exlist=IAV(C(AAV(w)[9]));  // remember address of exclusions
    nexlist=AN(C(AAV(w)[9]));  // and length of list
    ASSERT(AR(C(AAV(w)[9]))<=1,EVRANK); ASSERT(nexlist==0||ISDENSETYPE(AT(C(AAV(w)[9])),INT),EVDOMAIN);  // must be integer list
    ASSERT(AR(C(AAV(w)[10]))<=1,EVRANK); ASSERT(ISDENSETYPE(AT(C(AAV(w)[10])),INT),EVDOMAIN); ASSERT(AN(C(AAV(w)[10]))==AS(C(AAV(w)[4]))[0],EVLENGTH); // yk, one per row of M
    yk=IAV(C(AAV(w)[10]));  // remember address of translation table of row# to basis column#
   }
  }else{
   // Dpiv counting, with Dpiv.  exlist is the Dpiv area
   ASSERT(AN(w)==10,EVLENGTH); 
   bv=zv=0;
   exlist=IAV(C(AAV(w)[9]));  // remember address of exclusions
   ASSERT(AR(C(AAV(w)[9]))==1,EVRANK); ASSERT(ISDENSETYPE(AT(C(AAV(w)[9])),INT),EVDOMAIN);  // must be integer list
   ASSERT(AN(C(AAV(w)[9]))==AS(C(AAV(w)[1]))[0]+AS(C(AAV(w)[4]))[0],EVLENGTH);  // length of Dpiv is #cols of A + #rows of A (=M)
   z=mtv;  // no error is possible; use harmless return value
  }
 }


#define YC(n) .n=n,
struct mvmctx opctx={.ctxlock=0,.abortcolandrow=-1,.bestcolandrow={-1,-1},YC(ndxa)YC(n)YC(minimp)YC(bv)YC(thresh)YC(bestcol)YC(bestcolrow)YC(zv)YC(Frow)YC(nfreecolsd)
 YC(ncolsd)YC(impfac)YC(prirow)YC(bvgrd0)YC(bvgrde)YC(exlist)YC(nexlist)YC(yk)YC(bkmin).axv=((I(*)[2])IAV(C(AAV(w)[1])))-n,.amv0=IAV(C(AAV(w)[2])),.avv0=DAV(C(AAV(w)[3])),.qk=C(AAV(w)[4]),
 .ndotprods=0,.ncolsproc=0,.taskmask=0};
#undef YC

 // The number of tasks in the job is the number of boxed lists in ndxa
 jtjobrun(jt,jtmvmsparsex,&opctx,AN(ndxa),0);  // go run the tasks - default to threadpool 0

 // prepare final result
 if(likely(bv!=0)){
  // DIP call.
  if(unlikely(opctx.abortcolandrow!=-1))if((I4)opctx.abortcolandrow==-1)R v2(4,opctx.abortcolandrow>>32);  // if unbounded, return col# 
  GAT0(z,FL,6,1); zv=DAV(z);
  I rc=opctx.minimp==0.0?1:0; I bestcolandrow=opctx.bestcolandrow[rc];  // if no normal pivot, look at [1] to see if there is a dangerous one
  bestcolandrow=opctx.minimp==infm?opctx.abortcolandrow:bestcolandrow;  // if abort called, use the col/row from the abort field
  rc=bestcolandrow<0?3:rc; bestcolandrow=bestcolandrow<0?0:bestcolandrow; rc=opctx.minimp==1.0?2:rc;  // rc=1 if best pivot is dangerous; 2 if nonimproving; otherwise 3 if no pivot (stall), 0 if improving pivot found
       // if no pivot, set row/col to 0 for testcases
  zv[0]=rc; zv[1]=bestcolandrow>>32; zv[2]=(UI4)bestcolandrow;  // rc (normal or dangerous pivot), col, row
  zv[3]=opctx.ncolsproc; zv[4]=opctx.ndotprods; zv[5]=opctx.minimp;  // other stats: #cols, #dotproducts, best improvement
 }  // if call is just to fetch the column, return it, it's already in z
 EPILOG(z);
#else
 ASSERT(0,EVNONCE);  // this requires fast gather to make any sense
#endif
}

// everything we need for one core's execution
struct __attribute__((aligned(CACHELINESIZE))) ekctx {
 I taskmask;  // a bit for each task as we take it for work
 // the rest is moved into static names
 // arguments
 A qk;
 A prx;
 A pcx;
 A pivotcolnon0;
 A newrownon0;
 D relfuzz;
} ;


// the processing loop for one core.  We take a slice of the columns depending on our proc# in the threadpool
// ti is the job#, not used except to detect error
static unsigned char jtekupdatex(J jt,void* const ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(((struct ekctx*)ctx)->x) x=((struct ekctx*)ctx)->x;
 YC(prx)YC(qk)YC(pcx)YC(pivotcolnon0)YC(newrownon0)YC(relfuzz)
#undef YC

 __m256d pcoldh, pcoldl=_mm256_setzero_pd();  // value from pivotcolnon0, multiplying one row
 __m256d prowdh, prowdl=_mm256_setzero_pd();  // values from newrownon0
 __m256d mrelfuzz=_mm256_set1_pd(relfuzz);  // comparison tolerance
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
 I dpflag=0;  // precision flags: 1=Qk 2=pivotcolnon0 4=newrownon0
 D *qkv=DAV(qk); I qksize=AS(qk)[AR(qk)-1]; I qksizesq=qksize*qksize; dpflag|=AR(qk)>2;  // pointer to qk data, length of a row, offset to low part if present
 UI rowx=0, rown=AN(prx); I *rowxv=IAV(prx); D *pcn0v=DAV(pivotcolnon0); dpflag|=(AR(pivotcolnon0)>1)<<1;  // current row, # rows, address of row indexes, column data
 UI coln=AN(pcx); I *colxv=IAV(pcx); D *prn0v=DAV(newrownon0); dpflag|=(AR(newrownon0)>1)<<2;  // # cols, address of col indexes. row data
 // for each row
 for(rowx=0;rowx<rown;++rowx){
  // get the address of this row in Qk
  D *qkvrow=qkv+rowxv[rowx]*qksize;   // the row of Qk being modified
  // fetch the pivotcol value into all lanes
  pcoldh=_mm256_set1_pd(pcn0v[rowx]);  // fetch high part of pivotcol value
  if(dpflag&2)pcoldl=_mm256_set1_pd(pcn0v[rown+rowx]);  // fetch low part if it is present
  UI colx;
  // for each column-group
  for(colx=0;colx<coln;colx+=NPAR){
   __m256i endmask;  // maks for maskload and gather, indicating # words to process
   __m256i prn0x;  // indexes of nonzero values in row
   // get the mask of valid values, fetch pivotrow values, fetch the Qk indexes to modify
   if(coln-colx>=NPAR){  // all lanes valid
    prn0x=_mm256_loadu_si256((__m256i_u*)(colxv+colx));  // load the indexes into Qk
    prowdh=_mm256_loadu_pd(prn0v+colx);  // load next 4 non0 values in pivotrow
    if(dpflag&4)prowdl=_mm256_loadu_pd(prn0v+coln+colx);  // and low part if present
    endmask=sgnbit;  // indicate all lanes valid
   }else{
    endmask=_mm256_loadu_si256((__m256i*)(validitymask+NPAR-(coln-colx)));  // mask of valid lanes
    prn0x=_mm256_maskload_epi64(colxv+colx,endmask);  // load the indexes into Qk
    prowdh=_mm256_maskload_pd(prn0v+colx,endmask);  // load next 4 non0 values in pivotrow
    if(dpflag&4)prowdl=_mm256_maskload_pd(prn0v+coln+colx,endmask);  // and low part if present
   }
   // gather the high parts of Qk
   __m256d qkvh=_mm256_setzero_pd(); qkvh=_mm256_mask_i64gather_pd(qkvh,qkvrow,prn0x,endmask,SZI);
   // create max(abs(qkvh),abs(pcoldh*prowdh)) which will go into threshold calc
   __m256d maxabs=_mm256_max_pd(_mm256_andnot_pd(sgnbit,qkvh),_mm256_andnot_pd(sgnbit,_mm256_mul_pd(pcoldh,prowdh)));
   if(!(dpflag&1)){
    // single-precision calculation
    // calculate old - pcol*prow
    qkvh=_mm256_fnmadd_pd(prowdh,pcoldh,qkvh);
    // convert maxabs to abs(qkvh) - maxabs*thresh: if < 0, means result should be forced to 0
    maxabs=_mm256_fnmadd_pd(maxabs,mrelfuzz,_mm256_andnot_pd(sgnbit,qkvh));
   }else{
    // extended-precision calculation
    __m256d iph,ipl,isl;  // intermediate products and sums
    __m256d qkvl;  // low-order part of result

    // (iph,ipl) = - prowdh*pcoldh
    TWOPROD(prowdh,pcoldh,iph,ipl)  // (prowdh,prowdl) to high precision
    iph=_mm256_xor_pd(sgnbit,iph);  // change sign for subtract
    if(_mm256_movemask_pd(_mm256_cmp_pd(qkvh,_mm256_setzero_pd(),_CMP_EQ_OQ))==0xf){
     // qkvh is all 0 - the result is simply (-iph,-ipl) 
     qkvh=iph; qkvl=_mm256_xor_pd(sgnbit,ipl);  // -iph, -ipl
     maxabs=_mm256_setzero_pd();  // do not force any values to true zero
    }else{
     // normal case where qkvh not all 0
     qkvl=_mm256_mask_i64gather_pd(qkvh,qkvrow+qksizesq,prn0x,endmask,SZI);  // gather the low parts of Qk
     // (qkvh,qkvl) - (prowdh,prowdl) * (pcoldh,pcoldl)
     // Do high-precision add of qkvh and iph.  If this decreases the absvalue of qkvh, we will lose precision because of insufficient
     // bits of qkv.  If this increases the absvalue of qkvh, all of qkvl will contribute and the limit of validity will be
     // from the product.  In either case it is safe to accumulate all the partial products and ipl into qkvl
     qkvl=_mm256_sub_pd(qkvl,ipl); qkvl=_mm256_fnmadd_pd(prowdh,pcoldl,qkvl); qkvl=_mm256_fnmadd_pd(prowdl,pcoldh,qkvl);  // the middle pps.  low*low will never contribute unless qkv is exhausted & thus noise
     TWOSUM(qkvh,iph,qkvh,isl)   // combine the high parts
     isl=_mm256_add_pd(isl,qkvl);  // add the combined low parts
     // Make sure qkvl is much less than qkvh
     TWOSUM(qkvh,isl,qkvh,qkvl)  // put pkvh into canonical form
     // convert maxabs to abs(qkvh) - maxabs*thresh: if < 0, means result should be forced to 0
     maxabs=_mm256_fnmadd_pd(maxabs,mrelfuzz,_mm256_andnot_pd(sgnbit,qkvh)); 

     // zero if lower than fuzz (low part)
     qkvl=_mm256_blendv_pd(qkvl,_mm256_setzero_pd(),maxabs);
    }
    // scatter the results (low part)
    qkvrow[_mm256_extract_epi64(prn0x,0)+qksizesq]=_mm256_cvtsd_f64(qkvl);
    if(coln-colx>1)qkvrow[_mm256_extract_epi64(prn0x,1)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvl,0b0001));
    if(coln-colx>2)qkvrow[_mm256_extract_epi64(prn0x,2)+qksizesq]=_mm256_cvtsd_f64(qkvl=_mm256_permute4x64_pd (qkvl,0b01001110));
    if(coln-colx>3)qkvrow[_mm256_extract_epi64(prn0x,3)+qksizesq]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvl,0b0001));
   }
   // zero if lower than fuzz (high part)
   qkvh=_mm256_blendv_pd(qkvh,_mm256_setzero_pd(),maxabs);
   // scatter the results (high part)
//   _mm256_mask_i64scatter_pd(qkvrow,endmask,prn0x,qkvh,SZI);
   qkvrow[_mm256_extract_epi64(prn0x,0)]=_mm256_cvtsd_f64(qkvh);
   if(coln-colx>1)qkvrow[_mm256_extract_epi64(prn0x,1)]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvh,0b0001));
   if(coln-colx>2)qkvrow[_mm256_extract_epi64(prn0x,2)]=_mm256_cvtsd_f64(qkvh=_mm256_permute4x64_pd (qkvh,0b01001110));
   if(coln-colx>3)qkvrow[_mm256_extract_epi64(prn0x,3)]=_mm256_cvtsd_f64(_mm256_permute_pd(qkvh,0b0001));
  }
 }
 R 0;
}

// 128!:12  calculate
// Qk=: (((<prx;pcx) { Qk) ((~:!.relfuzz) * -) pivotcolnon0 */ newrownon0) (<prx;pcx)} Qk
// with high precision
// a is prx;pcx;pivotcolnon0;newrownon0;relfuzz
// w is Qk
// If Qk has rank 3, or pivotcolnon0/newrownon0 rank 2, calculate them in extended FP precision
// Qk is modified in place
F2(jtekupdate){F2PREFIP;
 ARGCHK2(a,w);
 // extract the inputs
 A qk=w; ASSERT(AT(w)&FL,EVDOMAIN) ASSERT(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w),EVNONCE) ASSERT(BETWEENC(AR(w),2,3),EVRANK)
 ASSERT(AR(w)==2||AS(w)[0]==2, EVLENGTH) ASSERT(AS(w)[AR(w)-1]==AS(w)[AR(w)-2],EVLENGTH)
 ASSERT(AT(a)&BOX,EVDOMAIN) ASSERT(AR(a)==1,EVRANK) ASSERT(AN(a)==5,EVLENGTH)  // a is 5 boxes
 A prx=AAV(a)[0]; ASSERT(AT(prx)&INT,EVDOMAIN) ASSERT(AR(prx)==1,EVRANK)  // prx is integer list
 A pcx=AAV(a)[1]; ASSERT(AT(pcx)&INT,EVDOMAIN) ASSERT(AR(pcx)==1,EVRANK)  // pcx is integer list
 A pivotcolnon0=AAV(a)[2]; ASSERT(AT(pivotcolnon0)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(pivotcolnon0),1,2),EVRANK)
 ASSERT(AR(pivotcolnon0)==1||AS(pivotcolnon0)[0]==2, EVLENGTH)  // pivotcolnon0 is float or extended list
 A newrownon0=AAV(a)[3]; ASSERT(AT(newrownon0)&FL,EVDOMAIN) ASSERT(BETWEENC(AR(newrownon0),1,2),EVRANK)
 ASSERT(AR(newrownon0)==1||AS(newrownon0)[0]==2, EVLENGTH)  // newrownon0 is float or extended list
 A tmp=AAV(a)[4]; if(!(AT(tmp)&FL))RZ(tmp=cvt(FL,tmp)); ASSERT(AR(tmp)==0,EVRANK) D relfuzz=DAV(tmp)[0];  // relfuzz is a float atom
 // agreement
 ASSERT(AN(prx)==AS(pivotcolnon0)[AR(pivotcolnon0)-1],EVLENGTH) ASSERT(AN(pcx)==AS(newrownon0)[AR(newrownon0)-1],EVLENGTH)   // indexes and values must agree
 // do the work
#define YC(n) .n=n,
struct ekctx opctx={YC(prx)YC(qk)YC(pcx)YC(pivotcolnon0)YC(newrownon0)YC(relfuzz)};

#undef YC
 jtekupdatex(jt,&opctx,0);

 R qk;
}
