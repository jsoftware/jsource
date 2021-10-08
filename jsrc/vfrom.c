/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: From & Associates. See Hui, Some Uses of { and }, APL87.         */

#include "j.h"


F1(jtcatalog){PROLOG(0072);A b,*wv,x,z,*zv;C*bu,*bv,**pv;I*cv,i,j,k,m=1,n,p,*qv,r=0,*s,t=0,*u;
 F1RANK(1,jtcatalog,DUMMYSELF);
 ASSERT(!ISSPARSE((AT(w))),EVNONCE);
 if((-AN(w)&-(AT(w)&BOX))>=0)R box(w);   // empty or unboxed, just box it
 n=AN(w); wv=AAV(w); 
 DO(n, x=wv[i]; if(AN(x)){p=AT(x); t=t?t:p; ASSERT(!ISSPARSE(p),EVNONCE); ASSERT(HOMO(t,p),EVDOMAIN); RE(t=maxtype(t,p));});  // use vector maxtype; establish type of result
 t=t?t:B01; k=bpnoun(t);  // if all empty, use boolean for result
 GA10(b,t,n);      bv=CAV(b);  // allocate place to build each item of result - one atom from each box.  bv->item 0
 GATV0(x,INT,n,1);    qv=AV(x);   // allocate vector of max-indexes for each box - only the address is used  qv->max-index 0
 GATV0(x,BOX,n,1);    pv=(C**)AV(x);  // allocate vector of pointers to each box's data  pv->box-data-base 0
 RZ(x=apvwr(n,0L,0L)); cv=AV(x);   // allocate vector of current indexes, init to 0  cv->current-index 0
 DO(n, x=wv[i]; if(TYPESNE(t,AT(x)))RZ(x=cvt(t,x)); r+=AR(x); qv[i]=p=AN(x); DPMULDE(m,p,m); pv[i]=CAV(x););  // fill in *qv and *pv; calculate r=+/ #@$@> w, m=*/ */@$@> w
 GATV0(z,BOX,m,r);    zv=AAV(z); s=AS(z);   // allocate result area
 // There is no need to turn off pristinity of w, because nothing was copied out by pointer (everything was copied & then cloned)
 // The result is certainly pristine if it is DIRECT
 AFLAGORLOCAL(z,(-(t&DIRECT))&AFPRISTINE)  // set result pristine if boxes DIRECT
 DO(n, x=wv[i]; u=AS(x); DQ(AR(x),*s++=*u++;););   // fill in shape: ; $&.> w
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
  __m256i temp=_mm256_setzero_si256();  // mask to use for gather into all bytes
  I * RESTRICT v=(I*)wv; I* RESTRICT x=(I*)zv;  // input and output pointers
  if(an==1){  // special case of atom {"1 y
   if(m==1){  // the atom { list case is pretty common
    *x=v[j];  // just move the one value
   }else{
    __m256i lanestride=_mm256_mul_epu32(wstride,_mm256_loadu_si256((__m256i*)&iotavec[-IOTAVECBEGIN]));  // each lane accesses a different cell
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
    v+=j;  // advance base pointer to the column we are fetching
    wstride=_mm256_slli_epi64(wstride,LGNPAR);  // repurpose wstride to be stride between groups of 4 cells
    DQ((m-1)>>LGNPAR, _mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,lanestride,ones,SZI)); lanestride=_mm256_add_epi64(lanestride,wstride);  x+=NPAR;)
    /* runout, using mask */ 
    _mm256_maskstore_epi64(x, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,lanestride,endmask,SZI));   // must use a different reg for source and index, lest VS2013 create an illegal instruction
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
     do{_mm256_maskstore_epi64(x, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }else if(an<=2*NPAR){
     do{_mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexes0,ones,SZI));
        _mm256_maskstore_epi64(x+NPAR, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }else if(an<=3*NPAR){
     do{_mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexes0,ones,SZI));
        _mm256_storeu_si256((__m256i*)(x+NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes1,ones,SZI));
        _mm256_maskstore_epi64(x+2*NPAR, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }else{
      do{_mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexes0,ones,SZI));
        _mm256_storeu_si256((__m256i*)(x+NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes1,ones,SZI));
        _mm256_storeu_si256((__m256i*)(x+2*NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes2,ones,SZI));
        _mm256_maskstore_epi64(x+3*NPAR, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,indexesn,endmask,SZI)); v+=p; x+=an;}while(--i);
    }
   }else{
    indexesn=_mm256_loadu_si256((__m256i*)(av+3*NPAR));   // fetch last block of indexes
    indexesn=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesn),_mm256_castsi256_pd(_mm256_add_epi64(indexesn,wstride)),_mm256_castsi256_pd(indexesn)));  // get indexes, add axis len if neg
    ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
    do{
     // 17+indexes.  We must read the tail repeatedly
     // this first execution audits the indexes and converts negatives
     _mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexes0,ones,SZI));  // process the indexes saved in registers
     _mm256_storeu_si256((__m256i*)(x+NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes1,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+2*NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes2,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+3*NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexesn,ones,SZI));
     I *RESTRICT avv=av+4*NPAR; x+=4*NPAR;  // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
     __m256i indexes;
     if(an>5*NPAR){
      indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
      DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
       __m256i indexesx=indexes;  // fetch a block of indexes
       indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
       anynegindex=_mm256_or_si256(anynegindex,indexesx); indexesx=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesx),_mm256_castsi256_pd(_mm256_add_epi64(indexesx,wstride)),_mm256_castsi256_pd(indexesx)));  // get indexes, add axis len if neg
       ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesx,_mm256_sub_epi64(indexesx,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
       _mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexesx,ones,SZI)); x+=NPAR;
      )
      anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg
      ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
      _mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexes,ones,SZI)); x+=NPAR;
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(avv,endmask);  // fetch a block of indexes
     anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg.  unfetched indexes are 0
     ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
     _mm256_maskstore_epi64(x, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,indexes,endmask,SZI)); x+=((an-1)&(NPAR-1))+1;   // must use a different reg for source and index, lest VS2013 create an illegal instruction
     v+=p;  // advance to next input cell
     --i;
     if(_mm256_testz_pd(_mm256_castsi256_pd(anynegindex),_mm256_castsi256_pd(anynegindex)))break;
    }while(i);
    while(i){
     // this second version comes into play if there were no negative indexes.  If there are negatives we end up auditing them repeatedly, too bad.
     _mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexes0,ones,SZI));  // process the indexes saved in registers
     _mm256_storeu_si256((__m256i*)(x+NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes1,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+2*NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexes2,ones,SZI));
     _mm256_storeu_si256((__m256i*)(x+3*NPAR), temp=_mm256_mask_i64gather_epi64(temp,v,indexesn,ones,SZI));
     I *RESTRICT avv=av+4*NPAR; x+=4*NPAR;  // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
     __m256i indexes;
     if(an>5*NPAR){
      indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
      DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
       __m256i indexesx=indexes;  // fetch a block of indexes
       indexes=_mm256_loadu_si256((__m256i*)avv); avv+=NPAR;  // fetch a block of indexes
       _mm256_storeu_si256((__m256i*)x, temp=_mm256_mask_i64gather_epi64(temp,v,indexesx,ones,SZI)); x+=NPAR;
      )
      _mm256_storeu_si256((__m256i*)x,temp=_mm256_mask_i64gather_epi64(temp,v,indexes,ones,SZI)); x+=NPAR;
     }
     // runout using mask
     indexes=_mm256_maskload_epi64(avv,endmask);  // fetch a block of indexes
     _mm256_maskstore_epi64(x, endmask, temp=_mm256_mask_i64gather_epi64(temp,v,indexes,endmask,SZI)); x+=((an-1)&(NPAR-1))+1;   // must use a different reg for source and index, lest VS2013 create an illegal instruction
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
// return 0 if error, 1 if the boxes were hot homogeneous.
A jtaindex(J jt,A a,A w,I wf){A*av,q,z;I an,ar,c,j,k,t,*u,*v,*ws;
 ARGCHK2(a,w);
 an=AN(a);
 if(!an)R (A)1;
 ws=wf+AS(w); ar=AR(a); av=AAV(a);  q=av[0]; c=AN(q);   // q=addr, c=length of first box
 if(!c)R (A)1;  // if first box is empty, return error to revert to the slow way
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 GATV0(z,INT,an*c,1+ar); MCISH(AS(z),AS(a),ar) AS(z)[ar]=c; v=AV(z);  // allocate array for result.  Mustn't copy shape from AS(a) - it overfetches
 for(j=0;j<an;++j){
  q=av[j]; t=AT(q);
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
 x=AAV(w)[0];
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
 c=AAV(a)[0]; t=AT(c); SETIC(c,n); v=AAV(c);   // B prob not reqd 
 s=AS(w)+wr-wcr;
 ASSERT(1>=AR(c),EVRANK);
 ASSERT(n<=wcr,EVLENGTH);
 if((-n&SGNIFNOT(t,BOXX))<0){RZ(ind=aindex(a,w,wf)); ind=(A)((I)ind&~1LL); if(ind)R frombu(ind,w,wf);}  // not empty and not boxed, handle as 1 index list
 if(wcr==wr){
  for(i=m=pr=0;i<n;++i){
   p=afi(s[i],v[i]);
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
  j=1+i; if(!p)p=afi(s[i],v[i]); q=j<n?afi(s[j],v[j]):ds(CACE); if(!(p&&q))break;  // pq are 0 if error.  Result of ds(CACE)=axis in full
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
    ASSERT(f==af || FFIEQ(f,af),EVDOMAIN);  // if index not integral, complain.  IMAX/IMIN will fail presently.  We rely on out-of-bounds conversion to peg out one side or other (standard violation)
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
  if(((AN(a)-1)|(AR(a)-2)|((AT(a)&NUMERIC)-1))>=0){A ind;   // a is an array with rank>1 and numeric.  Rank 1 is unusual & unimportant & we'll ignore it
   // Check indexes for validity; if valid, turn each row into a cell offset
   if(ind=celloffset(w,a)){
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
 R AT(w)&BOX?AAV(w)[index]:w;  // select the box, or return the entire unboxed w
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
 DO(n, A next=av[i]; if(((AT(z)>>BOXX)&1)>=(2*(AR(next)+(AT(next)&BOX))+AR(z))){RZ(z=jtquicksel(jt,next,z))}  // next is unboxed atom, z is boxed atom or list, use fast indexing  AR(next)==0 && !(AT(next)&BOX) && (AR(z)==0 || (AR(z)==1 && AT(z)&BOX))
      else{RZ(z=afrom(box(next),z)); ASSERT(((i+1-n)&-AR(z))>=0,EVRANK); if(((AR(z)-1)&SGNIF(AT(z),BOXX))<0)RZ(z=ope(z));}  // Rank must be 0 unless last; open if boxed atom
   );
 // Since the whole purpose of fetch is to copy one contents by address, we turn off pristinity of w
 PRISTCLRF(w)
 RETF(z);   // Mark the box as non-inplaceable, as above
}

// 128!:9 matrix times sparse vector with optional early exit
// y is (boolean exitvec, shape m);(I, int list of nonzero indexes in v, shape p);(V, float list of nonzero values in v, shape p);(M, shape m,n)
// Result is exitcode (boolean atom) ; product (I {"1 M) +/@:*"1 V (float list shape m)
// if (0>:i{product)+:(i{exitvec) for any i, exitcode is set to 1 and the product may be incomplete.  Otherwise exitcode is 0
// therefore, exitvec of 1 means 'no exit, calculate all products'
// Ex: 'rc col' =. (128!:40) goodbk;(ax ];.0 Am);(ax ];.0 Av);Qk
// Rank is infinite
F1(jtmvmsparse){PROLOG(832);
#if C_AVX2
 ASSERT(AR(w)==1,EVRANK);
 ASSERT(AN(w),EVLENGTH);  // audit overall w
 ASSERT(AT(w)&BOX,EVDOMAIN);
 // check ranks
 ASSERT(AR(AAV(w)[0])<=1,EVRANK);  // exitvec
 ASSERT(AR(AAV(w)[1])==1,EVRANK);  // I
 ASSERT(AR(AAV(w)[2])==1,EVRANK);  // V
 ASSERT(AR(AAV(w)[3])==2,EVRANK);  // M
 // check agreement
 ASSERT(AN(AAV(w)[1])==AN(AAV(w)[2]),EVLENGTH);   // I and V agree
 ASSERT(AR(AAV(w)[0])==0||AS(AAV(w)[3])[0]==AN(AAV(w)[0]),EVLENGTH);  // exitvec and M agree
 I n=AS(AAV(w)[3])[1];  // n=#cols in M
 I zn=AS(AAV(w)[3])[0];  // result is one atom per row of M
 I an=AN(AAV(w)[1]);  // an=#atoms in the sparse vector
 if(unlikely(an==0)){R link(num(0),reshape(sc(zn),zeroionei(0)));}   // empty a, each product is 0
 // convert types as needed; set ?v=pointer to data area for ?
 A exitcode=AAV(w)[0]; if(unlikely(!ISDENSETYPE(AT(exitcode),B01)))RZ(exitcode=cvt(B01,exitcode)); B *bv=BAV(exitcode);
 if(AR(AAV(w)[0])==0)bv=(B*)(I)BAV(exitcode)[0];  // if exitcode is an atom, put its value into bv
 A indexvec=AAV(w)[1]; if(unlikely(!ISDENSETYPE(AT(indexvec),INT)))RZ(indexvec=cvt(INT,indexvec)); I *iv=IAV(indexvec);
 A vvector=AAV(w)[2]; if(unlikely(!ISDENSETYPE(AT(vvector),FL)))RZ(vvector=cvt(FL,vvector)); D *vv=DAV(vvector);
 A mtx=AAV(w)[3]; if(unlikely(!ISDENSETYPE(AT(mtx),FL)))RZ(mtx=cvt(FL,mtx)); D *mv=DAV(mtx);
 // allocate the result area
 A z; GATV0(z,FL,zn,1); D *zv=DAV(z), *zv0=zv;
 // abort if empty input
 if(unlikely(zn==0))R link(num(0),z);  // exit fast if M empty
 // perform the operation
 __m256i endmask=_mm256_setzero_si256(); /* length mask for the last word */ 
 _mm256_zeroupperx(VOIDARG)
 __m256i wstride=_mm256_set1_epi64x(n);  // stride between cells in atoms - used for index check
 __m256i ones=_mm256_cmpgt_epi64(wstride,endmask);  // mask to use for gather into all bytes - set this way so compiler assigns a register
 __m256d temp=_mm256_setzero_pd();  // mask to use for gather into all bytes
 __m256d dotprod;  // place where product is assembled.
 UI i=AS(mtx)[0]; // loop counter for number of items to process
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-an)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
 __m256i anynegindex=_mm256_setzero_si256();  // accumulate sign bits of the indexes
 // Load the first 16 indexes and v-values into registers
 __m256i indexes0, indexes1, indexes2, indexesn;  // indexes, the last may be partial
 __m256d vvals0,vvals1,vvals2,vvalsn;  // the sparse vector, the last may be partial
 if(an>NPAR){
  indexes0=_mm256_loadu_si256((__m256i*)iv);   // fetch a block of indexes
  vvals0=_mm256_loadu_pd(vv);   // fetch a block of indexes
  indexes0=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes0),_mm256_castsi256_pd(_mm256_add_epi64(indexes0,wstride)),_mm256_castsi256_pd(indexes0)));  // get indexes, add axis len if neg
  ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes0,_mm256_sub_epi64(indexes0,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
  if(an>2*NPAR){
   indexes1=_mm256_loadu_si256((__m256i*)(iv+NPAR));   // fetch a block of indexes
   vvals1=_mm256_loadu_pd(vv+NPAR);   // fetch a block of indexes
   indexes1=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes1),_mm256_castsi256_pd(_mm256_add_epi64(indexes1,wstride)),_mm256_castsi256_pd(indexes1)));  // get indexes, add axis len if neg
   ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes1,_mm256_sub_epi64(indexes1,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
   if(an>3*NPAR){
    indexes2=_mm256_loadu_si256((__m256i*)(iv+2*NPAR));   // fetch a block of indexes
    vvals2=_mm256_loadu_pd(vv+2*NPAR);   // fetch a block of indexes
    indexes2=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes2),_mm256_castsi256_pd(_mm256_add_epi64(indexes2,wstride)),_mm256_castsi256_pd(indexes2)));  // get indexes, add axis len if neg
    ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes2,_mm256_sub_epi64(indexes2,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
   }
  }
 }
 if(an<=4*NPAR){
  indexesn=_mm256_maskload_epi64(iv+((an-1)&-NPAR),endmask);   // fetch last block of indexes
  vvalsn=_mm256_maskload_pd(vv+((an-1)&-NPAR),endmask);   // fetch a block of indexes
  indexesn=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesn),_mm256_castsi256_pd(_mm256_add_epi64(indexesn,wstride)),_mm256_castsi256_pd(indexesn)));  // get indexes, add axis len if neg
  ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
  // Now do the operation.  With <16 elements it doesn't help to use dual accumulators
#define FINISHDOTPROD dotprod=_mm256_add_pd(dotprod,_mm256_permute2f128_pd(dotprod,dotprod,0x01)); dotprod=_mm256_add_pd(dotprod,_mm256_permute_pd(dotprod,0xf));   /* combine accumulators horizontally  01+=23, 0+=1 */ \
  *(I*)zv=_mm256_extract_epi64(_mm256_castpd_si256(dotprod),0x0); /* store the single result from 0 */ \
  if((I)_mm256_extract_epi64(_mm256_castpd_si256(dotprod),0x0)>0){if(bv==0||((UI)bv>1&&bv[zv-zv0]==0))goto gt0result;} \
  ++zv; mv+=n; 

  if(an<=NPAR){
   do{dotprod=_mm256_mul_pd(vvalsn,temp=_mm256_mask_i64gather_pd(temp,mv,indexesn,_mm256_castsi256_pd(endmask),SZI));
      FINISHDOTPROD}while(--i);
  }else if(an<=2*NPAR){
   do{dotprod=_mm256_mul_pd(vvals0,temp=_mm256_mask_i64gather_pd(temp,mv,indexes0,_mm256_castsi256_pd(ones),SZI));
      dotprod=_mm256_fmadd_pd(vvalsn, temp=_mm256_mask_i64gather_pd(temp,mv,indexesn,_mm256_castsi256_pd(endmask),SZI),dotprod);
      FINISHDOTPROD}while(--i);
  }else if(an<=3*NPAR){
   do{dotprod=_mm256_mul_pd(vvals0,temp=_mm256_mask_i64gather_pd(temp,mv,indexes0,_mm256_castsi256_pd(ones),SZI));
      dotprod=_mm256_fmadd_pd(vvals1, temp=_mm256_mask_i64gather_pd(temp,mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
      dotprod=_mm256_fmadd_pd(vvalsn, temp=_mm256_mask_i64gather_pd(temp,mv,indexesn,_mm256_castsi256_pd(endmask),SZI),dotprod);
      FINISHDOTPROD}while(--i);
  }else{
   do{dotprod=_mm256_mul_pd(vvals0,temp=_mm256_mask_i64gather_pd(temp,mv,indexes0,_mm256_castsi256_pd(ones),SZI));
      dotprod=_mm256_fmadd_pd(vvals1, temp=_mm256_mask_i64gather_pd(temp,mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
      dotprod=_mm256_fmadd_pd(vvals2, temp=_mm256_mask_i64gather_pd(temp,mv,indexes2,_mm256_castsi256_pd(ones),SZI),dotprod);
      dotprod=_mm256_fmadd_pd(vvalsn, temp=_mm256_mask_i64gather_pd(temp,mv,indexesn,_mm256_castsi256_pd(endmask),SZI),dotprod);
      FINISHDOTPROD}while(--i);
  }
 }else{
  indexesn=_mm256_loadu_si256((__m256i*)(iv+3*NPAR));   // fetch last block of indexes
  vvalsn=_mm256_loadu_pd(vv+3*NPAR);   // fetch a block of indexes
  indexesn=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesn),_mm256_castsi256_pd(_mm256_add_epi64(indexesn,wstride)),_mm256_castsi256_pd(indexesn)));  // get indexes, add axis len if neg
  ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesn,_mm256_sub_epi64(indexesn,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
  do{
   // 17+indexes.  We must read the tail repeatedly.  It might gain a bit to have two accumulators but we don't yet.
   // this first execution audits the indexes and converts negatives
   dotprod=_mm256_mul_pd(vvals0,temp=_mm256_mask_i64gather_pd(temp,mv,indexes0,_mm256_castsi256_pd(ones),SZI));
   dotprod=_mm256_fmadd_pd(vvals1, temp=_mm256_mask_i64gather_pd(temp,mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
   dotprod=_mm256_fmadd_pd(vvals2, temp=_mm256_mask_i64gather_pd(temp,mv,indexes2,_mm256_castsi256_pd(ones),SZI),dotprod);
   dotprod=_mm256_fmadd_pd(vvalsn, temp=_mm256_mask_i64gather_pd(temp,mv,indexesn,_mm256_castsi256_pd(ones),SZI),dotprod);
   I *RESTRICT ivv=iv+4*NPAR; D *RESTRICT vvv=vv+4*NPAR;     // init input pointer to start of indexes not loaded into registers
   __m256i indexes; __m256d vvals;
   if(an>5*NPAR){
    indexes=_mm256_loadu_si256((__m256i*)ivv); ivv+=NPAR;  // fetch a block of indexes
    vvals=_mm256_loadu_pd(vvv); vvv+=NPAR;  // fetch a block of the vector
    DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
     __m256i indexesx=indexes; __m256d vvalsx=vvals;  // loop unroll
     indexes=_mm256_loadu_si256((__m256i*)ivv); ivv+=NPAR;  // fetch a block of indexes
     vvals=_mm256_loadu_pd(vvv); vvv+=NPAR;  // fetch a block of the vector
     anynegindex=_mm256_or_si256(anynegindex,indexesx); indexesx=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexesx),_mm256_castsi256_pd(_mm256_add_epi64(indexesx,wstride)),_mm256_castsi256_pd(indexesx)));  // get indexes, add axis len if neg
     ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexesx,_mm256_sub_epi64(indexesx,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
     dotprod=_mm256_fmadd_pd(vvalsx, temp=_mm256_mask_i64gather_pd(temp,mv,indexesx,_mm256_castsi256_pd(ones),SZI),dotprod);
    )
    anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg
    ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
    dotprod=_mm256_fmadd_pd(vvals, temp=_mm256_mask_i64gather_pd(temp,mv,indexes,_mm256_castsi256_pd(ones),SZI),dotprod);
   }
   // runout using mask
   indexes=_mm256_maskload_epi64(ivv,endmask);  // fetch a block of indexes
   vvals=_mm256_maskload_pd(vvv,endmask);   // fetch a block of indexes
   anynegindex=_mm256_or_si256(anynegindex,indexes); indexes=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(indexes),_mm256_castsi256_pd(_mm256_add_epi64(indexes,wstride)),_mm256_castsi256_pd(indexes)));  // get indexes, add axis len if neg.  unfetched indexes are 0
   ASSERT(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_andnot_si256(indexes,_mm256_sub_epi64(indexes,wstride))))==0xf,EVINDEX);  // positive, and negative if you subtract axis length
   dotprod=_mm256_fmadd_pd(vvals, temp=_mm256_mask_i64gather_pd(temp,mv,indexes,_mm256_castsi256_pd(endmask),SZI),dotprod);
   FINISHDOTPROD --i;
   if(_mm256_testz_pd(_mm256_castsi256_pd(anynegindex),_mm256_castsi256_pd(anynegindex)))break;
  }while(i);
  while(i){
   // this second version comes into play if there were no negative indexes.  If there are negatives we end up auditing them repeatedly, too bad.
   dotprod=_mm256_mul_pd(vvals0,temp=_mm256_mask_i64gather_pd(temp,mv,indexes0,_mm256_castsi256_pd(ones),SZI));
   dotprod=_mm256_fmadd_pd(vvals1, temp=_mm256_mask_i64gather_pd(temp,mv,indexes1,_mm256_castsi256_pd(ones),SZI),dotprod);
   dotprod=_mm256_fmadd_pd(vvals2, temp=_mm256_mask_i64gather_pd(temp,mv,indexes2,_mm256_castsi256_pd(ones),SZI),dotprod);
   dotprod=_mm256_fmadd_pd(vvalsn, temp=_mm256_mask_i64gather_pd(temp,mv,indexesn,_mm256_castsi256_pd(ones),SZI),dotprod);
   I *RESTRICT ivv=iv+4*NPAR; D *RESTRICT vvv=vv+4*NPAR;     // init input pointer to start of indexes not loaded into registers, advance output pointer over the prefix
   __m256i indexes; __m256d vvals;
   if(an>5*NPAR){
    indexes=_mm256_loadu_si256((__m256i*)ivv); ivv+=NPAR;  // fetch a block of indexes
    vvals=_mm256_loadu_pd(vvv); vvv+=NPAR;  // fetch a block of the vector
    DQNOUNROLL((an-5*NPAR-1)>>LGNPAR,
     __m256i indexesx=indexes; __m256d vvalsx=vvals;  // 
     indexes=_mm256_loadu_si256((__m256i*)ivv); ivv+=NPAR;  // fetch a block of indexes
     vvals=_mm256_loadu_pd(vvv); vvv+=NPAR;  // fetch a block of the vector
     dotprod=_mm256_fmadd_pd(vvalsx, temp=_mm256_mask_i64gather_pd(temp,mv,indexesx,_mm256_castsi256_pd(ones),SZI),dotprod);
    )
    dotprod=_mm256_fmadd_pd(vvals, temp=_mm256_mask_i64gather_pd(temp,mv,indexes,_mm256_castsi256_pd(ones),SZI),dotprod);
   }
   // runout using mask
   indexes=_mm256_maskload_epi64(ivv,endmask);  // fetch a block of indexes
   vvals=_mm256_maskload_pd(vvv,endmask);   // fetch a block of indexes
   dotprod=_mm256_fmadd_pd(vvals, temp=_mm256_mask_i64gather_pd(temp,mv,indexes,_mm256_castsi256_pd(endmask),SZI),dotprod);
   FINISHDOTPROD --i;
  }
 }
 EPILOG(link(num(0),z));
gt0result: R link(num(1),mtv);  // if we get a > 0 element that is not disabled, abort
#else
 ASSERT(0,EVNONCE);  // this requires fast gather to make any sense
#endif
}


