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
F2(jtifrom){A z;C*wv,*zv;I acr,an,ar,*av,j,k,m,p,pq,q,wcr,wf,wk,wn,wr,*ws,zn;
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
 wk=k*p;   // stride between cells of w
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
    __m256i lanestride=_mm256_mul_epu32(wstride,_mm256_loadu_si256((__m256i*)&iotavec[-IOTAVECBEGIN]));  // each lane accesses a different cell
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
    v+=j;  // advance base pointer to the column we are fetching
    wstride=_mm256_slli_epi64(wstride,LGNPAR);  // repurpose wstride to be stride between groups of 4 cells
    DQ((m-1)>>LGNPAR, _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,lanestride,ones,SZI)); lanestride=_mm256_add_epi64(lanestride,wstride);  x+=NPAR;)
    /* runout, using mask */ 
    _mm256_maskstore_epi64(x, endmask, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,lanestride,endmask,SZI));   // must use a different reg for source and index, lest VS2013 create an illegal instruction
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
    ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes0,_mm256_sub_epi64(indexes0,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
    if(an>2*NPAR){
     indexes1=_mm256_loadu_si256((__m256i*)(av+NPAR));   // fetch a block of indexes
     indexes1=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes1),_mm256_castsi256_pd(_mm256_add_epi64(indexes1,wstride)),_mm256_castsi256_pd(indexes1)));  // get indexes, add axis len if neg
     ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes1,_mm256_sub_epi64(indexes1,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
     if(an>3*NPAR){
      indexes2=_mm256_loadu_si256((__m256i*)(av+2*NPAR));   // fetch a block of indexes
      indexes2=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes2),_mm256_castsi256_pd(_mm256_add_epi64(indexes2,wstride)),_mm256_castsi256_pd(indexes2)));  // get indexes, add axis len if neg
      ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes2,_mm256_sub_epi64(indexes2,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
     }
    }
   }
   if(an<=4*NPAR){
    indexesn=_mm256_maskload_epi64(av+((an-1)&-NPAR),endmask);   // fetch last block of indexes
    indexesn=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesn),_mm256_castsi256_pd(_mm256_add_epi64(indexesn,wstride)),_mm256_castsi256_pd(indexesn)));  // get indexes, add axis len if neg
    ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
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
    ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
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
       ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesx,_mm256_sub_epi64(indexesx,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
       _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexesx,ones,SZI)); x+=NPAR;
      )
      anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg
      ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
      _mm256_storeu_si256((__m256i*)x, _mm256_mask_i64gather_epi64(_mm256_setzero_si256(),v,indexes,ones,SZI)); x+=NPAR;
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(avv,endmask);  // fetch a block of indexes
     anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg.  unfetched indexes are 0
     ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
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
#else
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
static F2(jtbfrom){A z;B*av,*b;C*wv,*zv;I acr,an,ar,k,m,p,q,r,*u=0,wcr,wf,wk,wn,wr,*ws,zn;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(ar>acr)R rank2ex(a,w,DUMMYSELF,acr,wcr,acr,wcr,jtbfrom);  // use rank loop if multiple cells of a
 an=AN(a); wn=AN(w); ws=AS(w);
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce domain error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
 p=wcr?ws[wf]:1; q=an>>LGSZI; r=an&(SZI-1);   // p=# items of w
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
   DQ(m, DO(pn, j=e*pv[i]; DO(qn, *x++=v[j+qv[i]];         )); v+=n;);} break; // n=#_2-cells in a cell of w.
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
#endif
 
// 128!:9 matrix times sparse vector with optional early exit
// product mode:
// y is ndx;Ax;Am;Av;(M, shape m,n)  where ndx is an atom
// if ndx<m, the column is ndx {"1 M; otherwise ((((ndx-m){Ax) ];.0 Am) {"1 M) +/@:*"1 ((ndx-m){Ax) ];.0 Av
// Result for product mode (exitvec is scalar) is the product
// DIP mode
// y is ndx;Ax;Am;Av;(M, shape m,n);bkgrd;(ColThreshold,MinPivot,bkmin,NFreeCols,NCols,ImpFac,Nvirt);bk;Frow[;exclusion list;Yk]
// Result is rc,best row,best col,#cols scanned,#dot-products evaluated,best gain  (if rc e. 0 1 2)
//           rc,failing column of NTT, an element of ndx (if rc=4)
//  rc=0 is good; rc=1 means the pivot found is dangerously small; rc=2 nonimproving pivot found; rc=3 no pivot found, stall; rc=4 means the problem is unbounded (only the failing column follows)
//  rc=5 (not created - means problem is infeasible) rc=6=empty M, problem is malformed
// if the exclusion list is given, we stop on the first nonimproving pivot, and the exclusion list is used to prevent repetition of basis
// If Frow is empty, we are looking for nonimproving pivots in rows where the selector is 0.  In that case the bkgrd puts the bk values in descending order.  We return the first column that will make more 0 B rows non0 than non0 B rows 0.
//
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
 ASSERT(AR(C(AAV(w)[4]))==2,EVRANK);  // M
 // abort if no columns
 if(AN(C(AAV(w)[0]))==0)R num(6);  // if no cols (which happens at startup, return error indic)
 // check types.  Don't convert - force the user to get it right - except for ndx
 ASSERT(AT(C(AAV(w)[1]))&INT,EVDOMAIN);  // Ax, shape cols,2 1
 ASSERT(AT(C(AAV(w)[2]))&INT,EVDOMAIN);  // Am
 ASSERT(AT(C(AAV(w)[3]))&FL,EVDOMAIN);  // Av
 ASSERT(AT(C(AAV(w)[4]))&FL,EVDOMAIN);  // M
 // check agreement
 ASSERT(AS(C(AAV(w)[2]))[0]==AS(C(AAV(w)[3]))[0],EVLENGTH);   // Am and Av
 ASSERT(AS(C(AAV(w)[4]))[0]==AS(C(AAV(w)[4]))[1],EVLENGTH);   // M is square
 // extract pointers to tables
 A ndxa=C(AAV(w)[0]); ASSERT(AN(ndxa)!=0,EVLENGTH); if(!(AT(ndxa)&INT))RZ(ndxa=cvt(INT,ndxa));
 I *ndx0=IAV(ndxa), *ndx=ndx0, nc=AN(ndxa), *ndxe=ndx+nc;  // pointer to column numbers in the order processed, and end+1, and #cols
 I (*axv)[2]=(I(*)[2])IAV(C(AAV(w)[1]));  // pointer to ax data
 I *amv0=IAV(C(AAV(w)[2]));  // pointer to am block, and to the selected column's indexes
 D *avv0=DAV(C(AAV(w)[3]));  // pointer to av data, and to the selected column's values
 D *mv0=DAV(C(AAV(w)[4]));  // pointer to M data, and to the selected column (for identity columns) or to the current row of M (for sparse columns)
 D minimp=0.0, minimpfound=0.0;  // (always neg) min improvement we will accept, best improvement in any column so far.  Init to 0 so we take first column with a pivot
 I nvirt=0;  // number of virtual rows at the beginning of bkg - give them priority

 I n=AS(C(AAV(w)[4]))[0];  // n=#rows/cols in M
 // convert types as needed; set ?v=pointer to data area for ?
 D *bv; // pointer to b values if there are any
 __m256d thresh;  // ColThr Inf    bkmin  MinPivot     validity thresholds, small positive values
 __m256d oldcol;  // oldcol Frow   0      0            col value at previous smallest pivot / Frow of current col (always <=0)  0 0
 __m256d oldbk;   // oldbk  minimp 0      0            bk value at previous smallest pivot, best gain available in a previous col (always <=0) 0 0
 I bestcol=1LL<<(BW-1), bestcolrow=1LL<<(32+3);  // col# and row#+mask for best value found from previous column, init to no col found, and best value not dangerous
 I ndotprods=0;  // total # dotproducts evaluated
 A z; D *zv; D *Frow;  // pointer to output for product mode, Frow
 I nfreecols, ncols; D impfac;  // number of cols to process before we insist of min improvement; min number of cols to process (always proc till improvement found); min gain to accept as an improvement after freecols
 I *bvgrd0, *bvgrde;  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk  if zv!=0, we process rows in order
 I *exlist=0, nexlist, *yk;  // exclusion list: list of excluded col|row pairs, length
 D bkmin;  // the largest value for which bk is considered close enough to 0

 if(AR(C(AAV(w)[0]))==0){
  // single index value.  set bv=0 as a flag that we are storing the column
  bv=0; ASSERT(AN(w)==5,EVLENGTH);  // if goodvec is an atom, set bv=0 to indicate that bv is not used and verify no more input
  if(unlikely(n==0)){R reshape(sc(n),zeroionei(0));}   // empty M, each product is 0
  GATV0(z,FL,n,1); zv=DAV(z);  // allocate the result area for column extraction.  Set zv nonzero so we use bkgrd of i. #M
  bvgrd0=0; bvgrde=bvgrd0+AS(C(AAV(w)[4]))[0];  // length of column is #M
 }else{
  // A list of index values.  We are doing the DIP calculation
  ASSERT(AR(C(AAV(w)[5]))==1,EVRANK); ASSERT(AN(C(AAV(w)[5]))==0||AT(C(AAV(w)[5]))&INT,EVDOMAIN); bvgrd0=IAV(C(AAV(w)[5])); bvgrde=bvgrd0+AN(C(AAV(w)[5]));  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk
  if(AN(C(AAV(w)[5]))==0){RETF(num(6))}  // empty bk - give error/empty result 6
  ASSERT(BETWEENC(AN(w),8,11),EVLENGTH); 
  ASSERT(AR(C(AAV(w)[7]))<=1,EVRANK); ASSERT(AT(C(AAV(w)[7]))&FL,EVDOMAIN); ASSERT(AN(C(AAV(w)[7]))==AS(C(AAV(w)[4]))[0],EVLENGTH); bv=DAV(C(AAV(w)[7]));  // bk, one per row of M
  ASSERT(AR(C(AAV(w)[8]))<=1,EVRANK);   // Frow, one per row of M and column of A
  if(AN(C(AAV(w)[8]))==0)Frow=0;else{ASSERT(AT(C(AAV(w)[8]))&FL,EVDOMAIN); ASSERT(AN(C(AAV(w)[8]))==AS(C(AAV(w)[4]))[0]+AS(C(AAV(w)[1]))[0],EVLENGTH); Frow=DAV(C(AAV(w)[8]));}  // if Frow omitted we are looking to make bks nonzero
  ASSERT(AR(C(AAV(w)[6]))<=1,EVRANK); ASSERT(AT(C(AAV(w)[6]))&FL,EVDOMAIN); ASSERT(AN(C(AAV(w)[6]))==7,EVLENGTH);  // 7 float constants
  if(unlikely(n==0)){RETF(num(6))}   // empty M - should not occur, give error result 6
  bkmin=DAV(C(AAV(w)[6]))[2];
  thresh=_mm256_set_pd(DAV(C(AAV(w)[6]))[1],bkmin,inf,DAV(C(AAV(w)[6]))[0]); nfreecols=(I)(nc*DAV(C(AAV(w)[6]))[3]); ncols=(I)(nc*DAV(C(AAV(w)[6]))[4]); impfac=DAV(C(AAV(w)[6]))[5]; nvirt=(I)DAV(C(AAV(w)[6]))[6];
  zv=AN(C(AAV(w)[5]))==AN(C(AAV(w)[7]))?Frow:0;  // set zv nonzero as a flag to process leading columns in order, until we have an improvement to shoot at.  Do this only if ALL values in bk are to be processed
  if(AN(w)>9){
   ASSERT(AN(w)==11,EVLENGTH); 
   // An exclusion list is given (and thus also yk).  Remember their addresses.  Its presence puts us through the 'nonimproving path' case
   exlist=IAV(C(AAV(w)[9]));  // remember address of exclusions
   nexlist=AN(C(AAV(w)[9]));  // and length of list
   ASSERT(AR(C(AAV(w)[9]))<=1,EVRANK); ASSERT(nexlist==0||ISDENSETYPE(AT(C(AAV(w)[9])),INT),EVDOMAIN);  // must be integer list
   ASSERT(AR(C(AAV(w)[10]))<=1,EVRANK); ASSERT(ISDENSETYPE(AT(C(AAV(w)[10])),INT),EVDOMAIN); ASSERT(AN(C(AAV(w)[10]))==AS(C(AAV(w)[4]))[0],EVLENGTH); // yk, one per row of M
   yk=IAV(C(AAV(w)[10]));  // remember address of translation table of row# to basis column#
  }
 }

 // perform the operation

#define PROCESSROWRATIOS /* We run this after the dot-product has been loaded into dotprod */ \
 if(likely(bv!=0)){ \
  /* DIP processing.  col data is in dotprod as col col col col */ \
  if(likely(exlist==0)){ /* normal look for improving pivot */  \
   __m256d ratios=_mm256_mul_pd(oldbk,dotprod);  /* ratios is col*oldbk col*minimp 0 0 */ \
   __m256d bks=_mm256_set1_pd(bv[i]);  /* bks = bk bk bk - */ \
   dotprod=_mm256_blend_pd(dotprod,bks,0b0100);  /* dotprod=col col bk col */ \
   __m256d tcond=_mm256_sub_pd(thresh,dotprod); I tmask=_mm256_movemask_pd(tcond); if((tmask&0b0101)==0b0001)goto abortcol;  /* tmask is col>ColThr 0 bk>bkmin col>ColMin; also in tcond; abort, avoiding unbounded, if col>ColThr & bk<=0 */ \
   I imask=i+(tmask<<32);  /* save tmask in the pivot; we care about bit 3 which is 1 if this is not a dangerous pivot */ \
   ratios=_mm256_fmsub_pd(bks,oldcol,ratios);  /* bk*oldcol-col*oldbk  bk*Frow-col*minimp 0 0  i. e.  1 if new smallest (possibly invalid) pivotratio/0 if abort on limited gain/0 /0 */ \
   tcond=_mm256_and_pd(tcond,ratios);  /* tcond is (new smallest pivotratio & col>ColThr [& bk>bkmin]) 0 0 0   */ \
   oldbk=_mm256_blendv_pd(oldbk,bks,tcond); oldcol=_mm256_blendv_pd(oldcol,dotprod,tcond);  /* if valid new smallest pivotratio, update col and bk where the smallest is found */ \
   I rmask=_mm256_movemask_pd(ratios); /*   rmask is new smallest (possibly invalid) pivotratio   !limited gain 0 0 */ \
   bestrow=(tmask&=1)&rmask?imask:bestrow;   /* col>ColThr & new smallest pivotratio: update best-value variables.  This updates even if bk=0, avoiding unbounded */ \
   if(tmask>(rmask>>1) && (I4)bestrow>=(I4)nvirt)goto abortcol;  /* col>ColThr && abort on limited gain: abort UNLESS the current best is on a virtual row.  We give them priority */ \
  }else if(Frow!=0){ /* look for nonimproving pivot */  \
   if(_mm256_cvtsd_f64(dotprod)>=_mm256_cvtsd_f64(thresh) && notexcluded(exlist,nexlist,*ndx,yk[i])){ndotprods+=bvgrd-bvgrd0+1; minimpfound=1.0; bestcol=*ndx; bestcolrow=i; goto return2;};  /* any c value>=ColThresh is a pivot row, unless in the exclusion list */ \
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
      bkold=bk; cold=c; bestrow=i; \
     } \
    }else if(c>0)goto abortcol;  /* possible dangerous pivot: skip the column */ \
   } \
  } \
 }else{zv[i]=_mm256_cvtsd_f64(dotprod); /* just fetching the column: do it */ \
 }

#define COLLPINIT I *bvgrd=bvgrd0; I i=-1; D *mv=mv0-n; D bkold=inf, cold=1.0; I bkle0=1;
#define COLLP do{if(unlikely(zv!=0)){++i; mv+=n;}else{i=*bvgrd; mv=mv0+n*i;} // for each row, i is the row#, mv points to the beginning of the row of M.  If we take the whole col, take it in order for cache.  Prefetch next row?
#define COLLPE }while(++bvgrd!=bvgrde);
 do{
  I colx=*ndx;  // get next column# to work on
  I bestrow;  // the best row to use as a pivot for this column
  if(likely(bv!=0)){
   if(exlist==0){
    // col init
    oldcol=_mm256_set_pd(0.0,0.0,Frow[colx],0.0);  //  init to pivotratio=inf and gain 0, assuming minimp is 0 the first time
    oldbk=_mm256_set_pd(0.0,0.0,minimp,1.0);
   }
   bestrow=-1;  // init no eligible row found
  }
  COLLPINIT
  // if the column is just to be fetched from M, do so without dot-product.  We can use gather down the column, but there's no gain
  __m256d dotprod;  // place where product is assembled or read into
  if(colx<n){  // scalar index list, means the column is an identity column.  Fetch it directly
   COLLP dotprod=_mm256_set1_pd(mv[colx]);   // fetch the column value into all 4 lanes
    PROCESSROWRATIOS
   COLLPE
  }else{
   // here for sparse dot-product.
   colx-=n;  // the offset into A starts after the identity prefix
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
  // done with one column.  Collect stats and update parms for the next column
  if(unlikely(!bv))break;  // if just one product, skip the setup for next column
  if(exlist==0){  // looking for nonimproving pivots?
   // not looking for nonimproving pivots.  Do a normal pivot-pick
   // column ran to completion.  Detect unbounded
   if(bestrow<0)R v2(4,*ndx);  // no pivots found for a column, problem is unbounded, indicate which column in the NTT, i. e. the input value which is an identity column if < #A
          // but if we are looking for nonimproving pivots, we don't use F or bk and this is invalid
   // The new column must have better gain than the previous one (since we had a pivot & didn't abort).  Remember where it was found and its improvement, unless it is dangerous
   if(likely((bestcol|SGNIF(bestrow,32+3))<0)){  // if this is the first valid pivot, or the previous pivot is dangerous
    bestcol=*ndx; bestcolrow=bestrow;
    // update the best-gain-so-far to the actual value found - but only if this is not a dangerous pivot.  We don't want to cut off columns that are beaten only by a dangerous pivot
    if(likely((bestrow&(1LL<<32+3))!=0)){  // if the improvement is one we are content with...
     minimpfound=(Frow[*ndx]*_mm256_cvtsd_f64(oldbk))/_mm256_cvtsd_f64(oldcol);  // this MUST be nonzero, but it decreases in magnitude till we find the smallest pivotratio
     zv=0;  // now that we have an improvement to shoot at, we will benefit by scanning cols in bk order
     // if the pivot was found in a virtual row, stop looking for other columns and take the one that gets rid of the virtual row.  But not if dangerous.
     if((I4)bestrow<(I4)nvirt){++ndx; ndotprods+=bvgrd-bvgrd0; break;}  // stop early if virtual pivot found, but take credit for the column we process here
    }
   }
   --bvgrd;  // undo the +1 in the product-accounting below
abortcol:  // here is column aborted early, possibly on insufficient gain
   if(unlikely(!bv))break;  // if just one product, skip the setup for next column
   ndotprods+=bvgrd-bvgrd0+1;  // accumulate # products performed, including the one we aborted out of
   if(unlikely((--ncols<0)&&zv==0)){++ndx; break;}  // quit if we have made an improvement AND have processed enough columns - include the aborted column in the count
   // prepare for next column
   minimp=minimpfound; if(--nfreecols<0)minimp*=impfac;  // for the first cols, accept any improvement; after that, insist on more
  }else{
   // we are looking for nonimproving pivots.  Skip all the gain accounting, and just remember how many products we did
   ndotprods+=bvgrd-bvgrd0;  // accumulate # products performed
   if(Frow==0){
    // accept a zero-Frow pivot if it exists, has more non0 than 0, and is not excluded
    if(bestrow>=0 && notexcluded(exlist,nexlist,*ndx,yk[bestrow])){
     minimpfound=1.0; bestcol=*ndx; bestcolrow=bestrow; goto return2;
    }
   }
  }
 }while(++ndx!=ndxe);  // end loop over columns
 // prepare final result
 if(bv){
  // DIP call.
 return2: ;
  GAT0(z,FL,6,1); zv=DAV(z);
  zv[0]=minimpfound==0.0?3:0; zv[0]=bestcolrow&(1LL<<(32+3))?zv[0]:1; zv[0]=minimpfound==1.0?2:zv[0];  // rc=1 if best pivot is dangerous; 2 if nonimproving; otherwise 3 if no pivot (stall), 0 if improving pivot found
  zv[1]=(UI4)bestcol; zv[2]=(UI4)bestcolrow;  // rc (normal or dangerous pivot), col, row
  zv[3]=ndx-ndx0; zv[4]=ndotprods; zv[5]=minimpfound;  // other stats: #cols, #dotproducts, bext improvement
 }  // if call is just to fetch the column, return it, it's already in z
 EPILOG(z);
#else
 ASSERT(0,EVNONCE);  // this requires fast gather to make any sense
#endif
}


