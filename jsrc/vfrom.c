/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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
#define IDXASSERT(vo,vr,b) {\
 __m256d _mask=_mm256_castsi256_pd(_mm256_andnot_si256(vr,_mm256_sub_epi64(vr,b))); /* positive, and negative if you subtract axis length */\
 ASSERT(_mm256_testc_pd(_mask,_mm256_castsi256_pd(ones)),EVINDEX);}

#define GETIDX(v,p,b) {\
 __m256i _v=p; /* fetch a block of indices */\
 v=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(_v),_mm256_castsi256_pd(_mm256_add_epi64(_v,b)),_mm256_castsi256_pd(_v))); /* get indexes, add axis len if neg */\
 IDXASSERT(_v,v,b);}
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
  if(!((AT(a)&(NOUN&~(B01|INT|(SY_64*FL))))+(AT(w)&(NOUN&~(INT|(SY_64*FL)|BOX)))+AR(a)+(SGNTO0((((RANKT)jt->ranks-AR(w))|(AR(w)-1))))+(AFLAG(w)&AFNJA))){   // scaf NJAwhy
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
// see if row,col is in exclusion list.  Exclusion list is a list of col|col with the smaller value in the high-order part
// Result is 0 if OK to accept the combination
static int notexcluded(I *exlist,I nexlist,I col,I row){I colrow=(col<row)?(col<<32)+row:(row<<32)+col;  // canonicalize column numbers into one value
 while(nexlist--)if(*exlist++==colrow)R 0;
 R 1;
}
 
// everything we need for one core's execution - shared by all cores
struct __attribute__((aligned(CACHELINESIZE))) mvmctx {
 // returned section
 US ctxlock;  // used to lock changes
 D minimp; // minimum (=best) improvement found in any previous column, including in other threads, set to -inf to abort other threads
 D maxdanger;  // if we have not found a non-dangerous pivot, we want to take the one with the largest column value acroos threads
 I bestcolandrow;  // (best column found, or -1 if none found)/(best row in column.  Bit 31 set if forcefeasible required.  If unbounded, set to -1 (with minimp=-inf))
   // during the search, bit 63 indicates dangerous pivot
 I nimpandcols;  // number of improvement,number of cols finished
 I ndotprods;  // total # dotproducts evaluated
 // shared section
 UI nextcol;  // index of next column to process. Advanced by atomic operations.  For one-column/nonimp mode, the index of next row to process
 // the rest is moved into static names
 A ndxa;   // the indexes, always a list of ndxs
 I n;  // #rows/cols in M
 D *bv;  // pointer to bk.  If 0, this is a column extraction (if zv!=0) or a nonimproving pivot
 D *Frow;  // the selector row
 D *zv; // pointer to output for product mode
 D *parms;  // parm info
 I *bvgrd0, *bvgrde;  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk
 I (*axv)[2];  // pointer to ax data
 I *amv0;  // pointer to am block, and to the selected column's indexes
 D *avv0;  // pointer to av data, and to the selected column's values
 A qk;  // original M block
 A sched;  // ending schedule, INT vector.  After the ith improvement, exit if i>:#sched or we have examined (<:i){sched columns
} ;
#define ONECOLGRD0 ((I*)(NPAR*SZI))  // starting value of bvgrd for onecol, offset from 0 so that backing up bvgrde won't wrap around 0


// the processing loop for one core.  We take a slice of the columns/rows, repeatedly
// ti is the job#, not used except to detect error
static unsigned char jtmvmsparsex(J jt,struct mvmctx *ctx,UI4 ti){
 // transfer everything out of ctx into local names
#define YC(x) typeof(ctx->x) x=ctx->x;
 YC(ndxa)YC(n)YC(minimp)YC(bv)YC(parms)YC(zv)
 YC(bvgrd0)YC(bvgrde)YC(axv)YC(amv0)YC(avv0)YC(qk)YC(Frow)YC(sched)
#undef YC
 // perform the operation

 D minimpfound=0.0;  // minimum (=best) improvement found so far from a non-dangerous pivot
 I ndotprods=0;  // number of dot-products we perform here

 UI *ndx0=IAV(ndxa), nc=AN(ndxa);  // origin of ordered column numbers, number of columns

 D *mv0=DAV(qk), *mv0lo=mv0+n*n;  // pointer to start of Qk and the the lower half if any and we will use it (0 if none)
 if(unlikely(nc==0))R 0;  // abort with no action if no columns (possible only for DIP)

 // set up loop invariants based on operation type
 I colressize;  // for one-col/nonimp mode, the size of each reservation
 I collen;  // for one-col, the length of the column; for nonimp, the length of bkg.  The number of values in a single-column operation
 UI firstcol=0, lastreservedcol=0;  // we have reserved columns from firstcol to lastreservedcol-1 for us to calculate
 I currcolproxy=0;   // The column pointer, approximately.  Updated only for DIP mode.  Dpiv doesn't have the variability, and one-col doesn't care
 I resredwarn=nc-100*((*JT(jt,jobqueue))[0].nthreads+1);  // when col# gets above this, switch to reserving one at a time  scaf switch to 2/4 at a time?   move to main
  // parms are  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow
 __m256d qpthresh=_mm256_set1_pd(parms[0]);  // if |column value| < QpThresh, recalculate in quad precision.  In one-column mode, this holds the Store0Threshold: column values less than this are set to 0
 __m256d col0thresh;  //  minimum value considered valid for a column value (smaller are considered 0)
 __m256d colbk0thresh;  //  minimum value that will block a pivot when bk=0.  In nonimp mode this is the smallest value we will accept for a random pivot, coming from ColOkPivot
 D coldangerpivotthresh;  //  smallest allowed pivot, but is dangerous
 D colokpivotthresh;  // smallest pivot value considered non-dangerous.
 __m256d bk0thresh;  // smallest bk value considered nonzero
 I prirow;  //  priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so.  Set to LOW_VALUE if no priority row
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
 __m256i rowstride; // number of Ds in a row of M, in each lane for DIP or nonimp; spacing between groups of NPAR rows, for onecol
 __m256i rownums;  // DIP/nonimp: row numbers we are fetching from in the current pass.  Taken from bvgrd   in onecol/gradient, the offset in atoms to the next rows to fetch
 __m256i rownums0;  // gradient: rownums at the top of a column
 D maxdangerc;  // DIP: max col value of dangerous pivot found here
 I4 bestcol=-1; UI4 bestcolrow=0-1;  // best column found so far (bit 31 set if dangerous or none found), and the best row in that column (bit 31 set if forcefeasible required)

 // DIP: bv->bk values (0 for gradient mode), zv=1 waiting for first c>0, 3=waiting for first SPR, 2=we have an SPR.  Bit 2 of zv=0 to force qp, =1 to try dp first.
  //  Bit 3 means 'nonzero column value ignored, forcefeasible required' bit 4 is a temp flag meaning 'this column was an improvement'
 // nonimp: zv=4 (start in dp)
 // onecol: zv->output, bv=0 if dp, n if qp (which is always)
#define ZVFFREQDX 4  // set if we have encountered a column value that may cause us to need forcefeasible
#define ZVFFREQD (1LL<<ZVFFREQDX)  // set if we have encountered a column value that may cause us to need forcefeasible
#define ZVMODX 3  // set if the current column was an improvement
#define ZVMOD (1LL<<ZVMODX)  // set if the current column was an improvement
#define ZVDP 0b100  // set if we should start in double precision (set for DIP/nonimp)
#define ZVPOSCVFOUND 0b10  // set if we have found a positive column value, indicating column is bounded
#define ZVSPRNOTFOUND 0b01  // set if we have not yet found a valid SPR for the column
// low 2 bits are 00 for non-DIP mode: 000=one-column, 100=nonimp
#define ZVISDIP (ZVPOSCVFOUND|ZVSPRNOTFOUND)
#define ZVISNOTONECOL (ZVDP|ZVPOSCVFOUND|ZVSPRNOTFOUND)

 rowstride=_mm256_set1_epi64x(n);   // length of 1 row, which is right for DIP/nonimp
 if(bv==0){  // nonimp or one-column: both process a single column
  bv=(D*)n;  // offset to low part of *zv - saves a register
  lastreservedcol=1;  // cause each thread to process the same one column, without arbitrating for it
  colressize=8*NPAR*((*JT(jt,jobqueue))[0].nthreads+1);   // enough rows to allow each thread to arbitrate if they have all-zero jobs TUNE.  Must be multiple of NPAR so that bvgrde is not adjusted except at end    scaf move to main
  if(zv!=0){  // one-column
   // Set up for multithreading one-column mode, where we have to split the column.  Multithreading of larger problems is handled
   // by having each thread take columns in turn
   // Since there is no bv or bvgrd, we save a register by repurposing bv to have the offset to the low part of result, if qp
   // bvgrd continues to be the loop counter, but the region starts at 0
   // in quad-precision we take reservations of a few lines each time.  Everybody starts at 0
   collen=(I)bv;  // length of the column - we do it all
   rownums=_mm256_mul_epu32(rowstride,_mm256_loadu_si256((__m256i*)(&iotavec[0-IOTAVECBEGIN])));  // init atomic offset to successive rows 0 1 2 3
   rowstride=_mm256_slli_epi64(rowstride,LGNPAR);   // length of NPAR rows for onecol
   // we leave the low bits ov zv=000 for onecol mode
  }else{
   // nonimp mode
   collen=bvgrde-bvgrd0;  // the length of the column (length of bkgrade)
   zv=(D*)ZVDP;  // zv flags 100: nonimp mode
   colbk0thresh=_mm256_set1_pd(parms[4]);  // ColOkPivot, put into an often-used register
  }
  bvgrde=bvgrd0;  // each reservation is taken in the loop below; start it out empty
 }else{
  // DIP/gradient initialization.  bv=0 for gradient - in that case colbk0thresh is used for Kahan summation
  col0thresh=_mm256_set1_pd(parms[1]); colbk0thresh=_mm256_set1_pd(parms[2]); coldangerpivotthresh=parms[3]; colokpivotthresh=parms[4]; bk0thresh=_mm256_set1_pd(parms[5]); prirow=(I)parms[6];   // copy in parms
  *(I*)&minimp=__atomic_load_n((I*)&(ctx->minimp),__ATOMIC_ACQUIRE);  // in case some other thread has finished a column and given us a bogey, go get it
  zv=(D*)(ZVDP+ZVSPRNOTFOUND);  // set flag 101 indicating DIP, dp, waiting for first c>0; bit 4 (ffreqd) starts clear
  maxdangerc=0.0;  // indicate no dangerous pivot found yet
  // bvgrde is set in the caller to process the requested rows
  bvgrde-=(bvgrde-bvgrd0)&(NPAR-1)?NPAR:0;  // back bvgrde to the point of the incomplete remnant, if there is one
  if(bvgrd0==ONECOLGRD0){  // gradient mode: set up for sequential processing
   rownums0=rownums=_mm256_mul_epu32(rowstride,_mm256_loadu_si256((__m256i*)(&iotavec[0-IOTAVECBEGIN])));  // init atomic offset to successive rows 0 1 2 3
   rowstride=_mm256_slli_epi64(rowstride,LGNPAR);   // length of NPAR rows for onecol
  }
 }

 // loop over all columns requested.  Low 3 bits of zv indicate mode: 000=onecol 100=nonimp other=DIP
 while(1){
  // start of processing one column
  __m256d minspr;  // minimum valid SPR found so far, in each qword-lane (for gradients, max column value seen)
// later  I firstsprrow=0;  // first row in which a valid SPR was found (may be lower than the actual value)   scaf init needed for warning
  __m256d limitcs;  // col value, valid only when there is a valid SPR (for gradients, upper accumulator of sumsq of column values)
  __m256i limitrows=_mm256_set1_epi64x(1000000000);  // row number fetched from, valid only when there is a valid SPR.  This is a value from bvgrd.  Must be initialized so as not to match prirow.  Same for gradients

  // establish column reservation, depending on mode
  // get next col index to work on; stop if no more cols
  // We would like to take sequential columns, because they are likely to share columns of Qk.  But there is wild variation in the time spent on a column: the
  // average column cuts off within 50 rows, while a winning column goes to the end, perhaps thousands of rows.  As a compromise we take groups of columns until
  // the number of columns left is less than 100*number of threads.  Then we switch to one at a time.  We use a slightly out-of-date value of the current column
  if(firstcol>=lastreservedcol){
   // we have to refresh our reservation.  We never go through here for one-column/nonimp mode; nextcol is the column reservation then
   UI ressize=currcolproxy>resredwarn?1:8;  // take 8 at a time till we get near end
   firstcol=__atomic_fetch_add(&ctx->nextcol,ressize,__ATOMIC_ACQ_REL);  // get next sequential column number, reserve a section starting from there
   lastreservedcol=firstcol+ressize;  // remember end of our reservation
  }

  if(firstcol>=nc)break;  // exit if all columns have been processed
  I colx=ndx0[firstcol];  // get next column# to work on
  I an; D *vv; I *iv;  // number of sparse atoms in each row (0 if Ek column), pointers to row#s, values for this column
  if(colx>=n){  // not a basis column
   an=axv[colx][1];  // number of sparse atoms in each row
   vv=avv0+axv[colx][0];  // pointer to values for this section of A
   iv=amv0+axv[colx][0];  // pointer to row numbers of the values in *vv (these are the columns we fetch in turn from Ek)
  }else an=-1;  // an<0 means basis column

  // loop until we don't need to rerun at higher precision
  {  // this is a loop: we branch back to retry
   // col init
   __m256d minimpspr;  // minimp/Frow of the current column, which is the minimum SPR that does not cut off
   if((I)zv&ZVISDIP){
    // DIP/gradient mode
    // This is the retry point; put static inits above this
    if(1)zv=(D*)(ZVDP+ZVSPRNOTFOUND);else{retryinquad: zv=(D*)ZVSPRNOTFOUND;}  // if we need to retry in qp, so indicate
    if(likely(bvgrd0!=ONECOLGRD0)){
     minspr=_mm256_set1_pd(inf);  // DIP: minimum valid SPR found so far, in each qword-lane
     minimpspr=unlikely(Frow[colx]==0)?_mm256_setzero_pd():_mm256_set1_pd(minimp/Frow[colx]);  //minimum SPR that will not cut off
    }else{
     // gradient mode: limitcs/colbk0thresh are Kahan accumulator for sumsq; minspr holds max col value; limitrows holds index of max column values
     minspr=_mm256_set1_pd(infm); limitcs=_mm256_set1_pd(1.0/NPAR); colbk0thresh=_mm256_setzero_pd();  // gradient: the high-precision gradient sum (init to 1+)  also the largest c value
     minimpspr=_mm256_set1_pd(minimp*Frow[colx]*Frow[colx]);  // Frow^2 * best sumsq / best Frow^2, which is cutoff point for sumsq in new column (Frow^2)/sumsq > bestFrow^2/bestsumsq)
     // we must back up the column pointer each time to top-of-column
     rownums=rownums0;
    }
   }
   __m256d endmask=_mm256_setone_pd(); // mask for validity of next 4 words: used to control fetch from column and store into result row

   I *bvgrd;
   // create the column NPAR values at a time
   for(bvgrd=bvgrd0;;bvgrd+=NPAR){
    __m256i indexes;  // offset in atoms from Qk to the beginning of the row we are fetching = rownums*n
    if(likely(bvgrd<bvgrde)){  // not close to end of column or column section; there are a full 4 values to process
     if(bvgrd0!=ONECOLGRD0){   // DIP/nonimp
      rownums=_mm256_loadu_si256((__m256i*)bvgrd);  // fetch the next set of row#s
      indexes=_mm256_mul_epu32(rowstride,rownums);  // convert to atom #
     }else{  // onecol/gradient, which have no bkgrade
      indexes=rownums; rownums=_mm256_add_epi64(rownums,rowstride);  // sequential processing of entire column; advance rownums by 4 rows
     }
    }else{  // we have hit the end or the incomplete remnant
     if(unlikely(bvgrd!=bvgrde)){  // we did not do an even multiple of 4 (in which case there is no remnant)
      I mnvalid=(I)(bvgrde-bvgrd);  // this is ((end-4)-curr)=(end-curr)-4=#values left-4, which is _1-_3 first time, _5-_7 second time
      if(mnvalid>-NPAR){  // first time...
       // there is a remnant to process.  Get the endmask and use it to fetch rownums/indexes
       endmask=_mm256_loadu_pd((double*)(validitymask-mnvalid));   // 4-#values left is the correct mask, with (#values) ~0
       if(bvgrd0!=ONECOLGRD0){   // DIP/nonimp
        rownums=_mm256_maskload_epi64(bvgrd,_mm256_castpd_si256(endmask));  // fetch the remnant of row#s
        indexes=_mm256_mul_epu32(rowstride,rownums);  // convert to atom #
       }else{  // onecol/gradient
        indexes=rownums; rownums=_mm256_add_epi64(rownums,rowstride);  // sequential processing of entire column; advance rownums
       }
       goto notendcolumn;
      }
      // fall through second time we encounter an incomplete remnant
     }
     // here we are done with the column/section
     if((I)zv&ZVISDIP)break;  // end of column section is end of column except for one-column or nonimp, where we take multiple reservations per column
     // establish row reservation (one-col and nonimp mode only)
     // quad-prec one-column mode.  Take a set of rows to process.  The issue is that some rows are faster than others: rows with all 0s skip extended-precision
     // processing.  It might take 5 times as long to process one row as another.  To keep the tasks of equal size, we take a limited number of rows at a time.
     // Taking the reservation is an RFO cycle, which takes just about as long as checking a set of all-zero rows.  This suggests that the reservation should be
     // for NPAR*nthreads at least.
     I currx=bvgrde-bvgrd0;  // index we would be processing next.  If not at end, this is known not to require adjustment.
     I resrow=__atomic_fetch_add(&ctx->nextcol,colressize,__ATOMIC_ACQ_REL);
     if(resrow>=collen)break;  // finished when reservation is off the end
     I skipamt=resrow-currx;  // number of rows to skip
     zv+=skipamt;  // advance the store pointer.  Since the high and low planes may have different alignment we don't try to avoid false cacheline sharing, which will occur on the fringes
     I endx=resrow+colressize; endx=endx>collen?collen:endx;  // end+1 of the reservation (bv=n)
     bvgrd=bvgrd0+resrow; bvgrde=bvgrd0+endx;  // loop controls
     bvgrde-=endx&(NPAR-1)?NPAR:0;  // back bvgrde to the point of the incomplete remnant, if there is one (possible only at end)
     if(unlikely(NPAR-(endx-resrow)>0))endmask=_mm256_loadu_pd((double*)(validitymask+NPAR-(endx-resrow)));  // if we start on the remnant, fetch its mask
     if(bvgrd0!=ONECOLGRD0){   // nonimp, which has bvgrd
      if(unlikely(NPAR-(endx-resrow)>0))rownums=_mm256_maskload_epi64(bvgrd,_mm256_castpd_si256(endmask)); else rownums=_mm256_loadu_si256((__m256i*)bvgrd);  // fetch from updated bvgrd
      indexes=_mm256_mul_epu32(rowstride,rownums);  // convert to atom #
     }else{  // onecol
      rownums=_mm256_add_epi64(rownums,_mm256_set1_epi64x(skipamt*(I)bv));  // advance the atom-offsets over the skipped area (bv=n)
      indexes=rownums; rownums=_mm256_add_epi64(rownums,rowstride);  // sequential processing of entire column; advance rownums
     }
    }
notendcolumn: ;
    // rownums, indexes, bvgrd are all set up
    __m256d dotproducth,dotproductl;  // where we build the column value

    // Now mv and indexes are set up to read the correct rows
    // get the next NPAR values by dot-product with Av
    if(an<0){
     // fetching from the Ek matrix itself.  Just fetch the values from the column
     dotproducth=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0+colx,indexes,endmask,SZI);
     if(!((I)zv&ZVDP)){dotproductl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0lo+colx,indexes,endmask,SZI);}  // if quad-prec (onecol or DIP retry)
    }else{
     // fetching from A.  Form (Ek row) . (A column) for each of the 4 rows.  There must be at least 1 column selected
     if((I)zv&ZVDP){
      // DIP or nonimp, starting in dp. double-precision accumulate
      dotproducth=_mm256_setzero_pd();
      I k;  __m256d nonzerofound=_mm256_setzero_pd();  // will remain 0 if all values read in the lane are 0
      NOUNROLL for(k=0;k<an;++k){
       dotproductl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
       dotproducth=_mm256_fmadd_pd(dotproductl,_mm256_set1_pd(vv[k]),dotproducth);  // accumulate the dot-product
       nonzerofound=_mm256_or_pd(nonzerofound,_mm256_cmp_pd(dotproductl,nonzerofound,_CMP_NEQ_OQ));  // if nonzero found, remember
      }
      // if any of the results is below the qp threshold, revert to qp; but ignore any exact 0
      if(!_mm256_testz_pd(_mm256_cmp_pd(dotproducth,qpthresh,_CMP_LT_OQ),nonzerofound))goto revertqp;  // testz is 1 if all comparisons fail, i. e. no product is too low
     }else{
revertqp: ;
      // extended-precision accumulate, used only when Qk has quad-precision and we need the precision
      // It's not unusual for all 4 values fetched from Qk to be 0. 
      // For the first value, we can simplify the product with no accumulate
      I k;
      NOUNROLL for(k=0;k<an;++k){  // for each element of the dot-product
       __m256d tl,th2,tl2,vval;  // temps for value loaded, and multiplier from A column
       // get column number to fetch; fetch 4 rows
       dotproducth=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
       if(_mm256_testz_si256(_mm256_castpd_si256(dotproducth),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
       // there's a nonzero, calculate it
       tl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0lo+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
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
       th=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
       if(_mm256_testz_si256(_mm256_castpd_si256(th),_mm256_castpd_si256(endmask)))continue;  // if all values 0, skip em.  Probably worth a test
       tl=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),mv0lo+iv[k],indexes,endmask,SZI);  // fetch from up to 4 rows
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
    }  // end of creating NPAR values
    // process the NPAR generated values
    if((I)zv&ZVISDIP){
     if(likely(bvgrd0!=ONECOLGRD0)){
      // skip the block if all values are negative or near 0
      __m256d cnon0=_mm256_cmp_pd(dotproducth,col0thresh,_CMP_GT_OQ);  // ~0 for words that have positive c
      if(!_mm256_testz_pd(cnon0,endmask)){  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  if one is big enough...
       // DIP mode, looking for pivots.  process the values in parallel
       __m256d bk4=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),bv,rownums,endmask,SZI);  // the bk values we are working on
       zv=(D*)((I)zv|ZVPOSCVFOUND);  // if any c>thresh, zf is 0; set that we are bounded.
       // see if any of the new values is limiting: b/c < min SPR, c>ColThresh, bk>bkthresh
       // we perform this calculation on each lane separately, to save the expense of finding the minimum SPR each time one is calculated.
       // This means the values in each lane will be smaller than they would be with sharing, leading to perhaps 4x extra divisions.  That's a good trade.
       __m256d cbadifbk0=_mm256_cmp_pd(dotproducth,colbk0thresh,_CMP_GT_OQ);  // set to ~0 if c>ColBkThresh, i. e. column is blocked by bk=0 - always 0 if endmask 0
       __m256d ratios=_mm256_fnmadd_pd(_mm256_and_pd(cbadifbk0,minspr),dotproducth,bk4);  // b-minspr*c: sign set if b<minspr*c => b/c<minspr => this is a new min SPR in its lane.  If col is near 0, force minspr to 0, which clears the sign bit whenever bk4>0
         // by using cbadifbk0 rather than cnon0 we are ignoring the (valid) SPRs created by small column values.  This probably doesn't matter because those SPRs are
         // big anyway if bk!=0; but we do it mostly to match the dp code
       __m256d bknot0=_mm256_cmp_pd(bk4,bk0thresh,_CMP_GE_OQ); // we separate b into < threshold and >= threshold.   This must match the selection made for stall exit
       if(unlikely(!_mm256_testc_pd(bknot0,cbadifbk0)))goto abortcol;  // CF is 0 if any sign of ~a&b is 1 => ~(b>=bthresh) & c>BkThresh.  Positive c and b<=0 means we cannot pivot in this column
       if(unlikely(!_mm256_testc_pd(cbadifbk0,cnon0)))zv=(D*)((I)zv|ZVFFREQD);  // CF is 0 if any sign of ~a&b is 1 => ~(c>BkThresh) & c>0thresh.  We are ignoring a non0 pivot; we will need forcefeasible; flag that
       if(!_mm256_testz_pd(ratios,bknot0)){  // ZF is 1 if a&b signs all 0 => (new min&c ok if b!=0)&(b!=0) all 0 => no place where new min & b good & b good
        // normal case bypasses this block
        // at least one new SPR is limiting (in its lane).  Find the SPRs
        __m256d sprs=_mm256_div_pd(bk4,dotproducth);  // find SPRs, valid or not.  Not worth moving this before the mispredicting branch because it ties up IUs for so many cycles
        // the reciprocal takes a long time to run.  Make sure there are no dependencies.  The block of code
        // issued here will complete while the next block of dotproduct is being computed.
        if((I)zv&ZVSPRNOTFOUND){zv=(D*)((I)zv&~ZVSPRNOTFOUND); /* later firstsprrow=(bvgrd-bvgrd0);*/}  // index of (close to) first valid SPR.  Branch will predict correctly.  Set state to show that we are bounded and have first SPR
        __m256d validlimit=_mm256_and_pd(ratios,bknot0);  // pivot is valid and limiting - high bit only.  c too low was eliminated earlier; bk too low eliminated now
        minspr=_mm256_blendv_pd(minspr,sprs,validlimit);   // remember the limiting SPR, in each lane
        limitrows=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(limitrows),_mm256_castsi256_pd(rownums),validlimit));  // if there is a new limiting SPR, remember its row number
        limitcs=_mm256_blendv_pd(limitcs,dotproducth,validlimit);  // remember the column value at the limiting SPR.  We will classify it after we find the winning SPR
        // check for cutoff
        if(unlikely(!_mm256_testc_pd(_mm256_fmsub_pd(minimpspr,dotproducth,bk4),validlimit))){  // cutoff if Frow*b/c > minimp (Frow and minimp negative) => b/c < (minimp/Frow = minimpspr) => b < minimpspr*c => minimpspr*c-b > 0. i. e. any sign 0 when b!=0 => CF bit set
         // one of the values cuts off.  Abort this column, keeping the SPR and position.  But if the prirow is winning, don't abort - we want it out
         if(likely(_mm256_testz_si256(rowstride,_mm256_cmpeq_epi64(_mm256_set1_epi64x(prirow),limitrows))))goto abortcol;   // rowstride is any 256i with nonzero in all lanes
        }
       }
      }
     }else{
      // gradient mode.
      // because of the sparsity of the Ak matrix, it is worth testing for all the column values close to 0.  We could more easily test for all exactly equal 0, but we
      // don't know how many near-0s we are going to get
      if(!_mm256_testz_pd(_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,dotproducth),col0thresh,_CMP_GT_OQ),endmask)){  // testz is 1 if all comparisons fail, i. e. no product is big enough to process.  if one is big enough...
       // add new value^2 to gradient total, using Kahan summation (in limitcs/colbk0thresh)
       __m256d y, t; y=_mm256_fmadd_pd(dotproducth,dotproducth,colbk0thresh); t=_mm256_add_pd(limitcs,y); colbk0thresh=_mm256_sub_pd(y,_mm256_sub_pd(t,limitcs)); limitcs=t;  // accumulate col^2; 16 cycles latency, which will limit perf (slightly)
       // fetch the 4 bk values and see which ones are near0
       __m256d bk4=likely(bvgrd<bvgrde)?_mm256_loadu_pd(bv+(bvgrd-ONECOLGRD0)):_mm256_maskload_pd(bv+(bvgrd-ONECOLGRD0),_mm256_castpd_si256(endmask));  // the next bk values
       dotproducth=_mm256_and_pd(dotproducth,_mm256_cmp_pd(bk4,bk0thresh,_CMP_LT_OQ));   // clear any column values for which bk is not near0
       // remember column position (of the NPAR-word block) of largest value (in limitrows)
       limitrows=_mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(limitrows),_mm256_castsi256_pd(_mm256_set1_epi64x((I)bvgrd)),_mm256_cmp_pd(dotproducth,minspr,_CMP_GT_OQ)));
       // find largest column value to date (in minspr)
       minspr=_mm256_max_pd(minspr,dotproducth);  // find the winner
       if(unlikely((bvgrd-ONECOLGRD0)==(prirow&-NPAR))){
        // We are looking at the virtual row.  If it is eligible and not dangerous, stop right away and take it
        if(_mm256_movemask_pd(_mm256_cmp_pd(dotproducth,_mm256_set1_pd(colokpivotthresh),_CMP_GE_OQ))&(1<<(prirow&(NPAR-1)))){
         // We can pivot out the virtual row.  We take that whenever we can, preemptively
         ndotprods+=bvgrd-bvgrd0+1; bestcol=colx; bestcolrow=prirow; goto return4;
        }
       }
       // if gradient total cuts off, abort the column.  We have to collect the total across all lanes
       dotproducth=_mm256_add_pd(_mm256_permute_pd(t,0b0101),t); dotproducth=_mm256_add_pd(_mm256_permute4x64_pd(dotproducth,0b01001110),dotproducth);   // combine into one
       if(unlikely(!_mm256_testz_pd(_mm256_cmp_pd(dotproducth,minimpspr,_CMP_GT_OQ),endmask))){  // cutoff if Frow^2/sumsq<(best Frow^2/sumsq) => sumsq>(best sumsq)*(Frow/best Frow)^2  we save min of (best sumsq)/(best Frow^2) which is max of FOM
                // scaf this does not have to use testz - it is just comparing any lane but there is no way to refer to that
        // The column can be cut off.  But if there is a priority row, make sure we check it
        I skipamt=((prirow&-NPAR)-NPAR)-(bvgrd-ONECOLGRD0);  // number of rows to hop over to get to one row before prirow
        if(skipamt<0)goto abortcol;  // If we have already checked the virtual row, we can abort
        bvgrd+=skipamt;  // if we haven't checked the virtual row, skip to just before it.  If it isn't eligible we will abort next time
        rownums=_mm256_add_epi64(rownums,_mm256_mul_epu32(rowstride,_mm256_set1_epi64x(skipamt>>LGNPAR)));  // advance rownums in groups of 4 rows to match the #rows we skipped
       }
      }
     }
    }else if((I)zv&ZVISNOTONECOL){  // nonimp
     __m256d threshcmp;  // results of comparing column value ve min
     // looking for nonimproving pivots (only in cols where selector<0, in rows where bk is near0).  We choose the first one we see that is above the threshold (which here implies non-dangerous).
     if(unlikely(!_mm256_testz_pd(threshcmp=_mm256_cmp_pd(dotproducth,colbk0thresh,_CMP_GT_OQ),endmask))){  // if any compare is true, we have a match
      int mask1=_mm256_movemask_pd(threshcmp);  // see which value(s) succeeded
      // We must pick the first nonimp in the random order. bestcol is the order in bkg, bestcolrow is the index in bk
      ndotprods+=bvgrd-bvgrd0+1; bestcol=(bvgrd-bvgrd0)+CTTZI(mask1); bestcolrow=bvgrd[CTTZI(mask1)]; goto return2;
     }
    }else{
     // one-column mode: just store out the values, setting to 0 if below threshold
     __m256d okmask=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,dotproducth),qpthresh,_CMP_GT_OQ);  // 1s where we need to clamp
     dotproducth=_mm256_and_pd(dotproducth,okmask);  // set values < threshold to +0
     dotproductl=_mm256_and_pd(dotproductl,okmask);  // low part too.
     if(likely(_mm256_testc_pd(endmask,sgnbit))){  // if all lanes are valid...
      _mm256_storeu_pd(zv,dotproducth); _mm256_storeu_pd(zv+(I)bv,dotproductl);   // store high & low
     }else{
      _mm256_maskstore_pd(zv,_mm256_castpd_si256(endmask),dotproducth); _mm256_maskstore_pd(zv+(I)bv,_mm256_castpd_si256(endmask),dotproductl);  // store, masking
     }
     zv+=NPAR;  // advance to next output location
    }
   }
   // done with loop down one column.

   // End-of-column processing.  Collect stats and update parms for the next column

   if(!((I)zv&ZVISDIP))goto earlycol;  // oneprod/nonimp, only one column, skip setup
   // from here on, DIP/gradient mode; use zv to indicate 'improvement found'
   union __attribute__((aligned(CACHELINESIZE))) {I quadI[NPAR]; D quadD[NPAR]; } extractarea;  // place where we can see individual values
   if(unlikely(bvgrd0==ONECOLGRD0)){
    // gradient mode: limitcs/colbk0thresh are Kahan accumulator for sumsq; minspr holds max col value; limitrows holds index of max column values
    // We switch the sign of minspr because the code for SPR searches for smallest value
    _mm256_store_pd(&extractarea.quadD[0],minspr);   // we want the biggest positive value
    minspr=_mm256_sub_pd(_mm256_setzero_pd(),minspr);  // now the lanes have -(max value)
    zv=(D*)ZVPOSCVFOUND;
   }else{
    _mm256_store_pd(&extractarea.quadD[0],limitcs); 
   }
   // now minspr has a value to minimize, and extractarea has the true value to use
   // column ran to completion.  Detect unbounded
   if(!((I)zv&ZVPOSCVFOUND)){
    // no pivots found for a column, problem is unbounded, indicate which column in the NTT, i. e. the input value which is an identity column if < #A
    // BUT it is possible that a column that looks unbounded in double precision will be bounded in quad; so retry the column in that case
    if((I)zv&ZVDP)goto retryinquad;  // if we haven't used qp yet, retry the column in extended precision
    bestcolrow=0-1; bestcol=colx; goto return4;  // unbounded in qp: indicate where, with row=-1 as a flag
   }
   // it is just possible that the entire column would be processed with no SPR found (all the rows would have to have c=0 b=0).
   // In that case, ignore the column
   if(likely(!((I)zv&ZVSPRNOTFOUND))){   // state 1/3: no SPR received yet
    // The new column must have better gain than the previous best (since we had a pivot & didn't abort).  Remember where it was found and its improvement, unless it is dangerous.
    // Exception: if a nondangerous pivot pivots out a virtual row, we accept it immediately and abort all other processing
    __m256d allmin=_mm256_min_pd(minspr,_mm256_permute_pd(minspr,0b0101)); // allmin = min01 min01 min23 min23  could use integer in AVX-512
    allmin=_mm256_min_pd(allmin,_mm256_permute4x64_pd(allmin,0b00001010));  // allmin=min value in all lanes
    I minx=CTTZI(_mm256_movemask_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(allmin),_mm256_castpd_si256(minspr)))));  // index of a lane containing the minimum SPR
    D minc=extractarea.quadD[minx];  // get column value at minimum SPR
    _mm256_store_si256((__m256i *)&extractarea.quadI,limitrows); I minrownum=extractarea.quadI[minx];  // (DIP) get bkgrd index of minimum row (gradient) same, but pointer into bvgrd0 instead
    D spratmin=_mm256_cvtsd_f64(allmin);    // the minimum SPR itself
    if(likely(minc>=coldangerpivotthresh)){  // if pivot is so low that it will damage the problem, skip over it, try next column
     if(likely(minc>=colokpivotthresh)){  // if this pivot is not dangerous, see if it is a new global best (for the time being); if so, share the improvement and remember that we have it
      // update the best-gain-so-far to the actual value found - but only if this is not a dangerous pivot.  We don't want to cut off columns that are beaten only by a dangerous pivot
       // if the pivot was found in a virtual row, stop looking for other columns and take the one that gets rid of the virtual row.  But not if dangerous.
      I incumbentimpi=__atomic_load_n((I*)&(ctx->minimp),__ATOMIC_ACQUIRE);  // load incumbent best improvement
      D minimpthiscol;  // the best FOM we found for this column
      // get FOM for this column, and for gradient mode convert the bvgrd index to the correct index#
      if(likely(bvgrd0!=ONECOLGRD0))minimpthiscol=Frow[colx]*spratmin;  // (DIP) this MUST be nonzero, but it decreases in magnitude till we find the smallest pivotratio.  This is our local best for this column
      else{  // (gradient)
       limitcs=_mm256_add_pd(_mm256_permute_pd(limitcs,0b0101),limitcs); limitcs=_mm256_add_pd(_mm256_permute4x64_pd(limitcs,0b11100110),limitcs);   // combine into one
       *(I*)&minimpthiscol=_mm256_extract_epi64(_mm256_castpd_si256(limitcs),0);  // total sumsq+1 of column
       minimpthiscol=minimpthiscol/(Frow[colx]*Frow[colx]);  // gradient mode: save minimum of sumsq/Frow^2 which is recip of the maximum gradient (so smaller=better)
       minrownum=(I)((I*)minrownum-ONECOLGRD0)+minx;  // convert from bv ptr to index, and then add winning lane#
      }
      if(unlikely((I4)minrownum==(I4)prirow)){bestcol=colx; bestcolrow=minrownum|(((I)zv&ZVFFREQD)<<(31-ZVFFREQDX)); ndotprods+=bvgrd-bvgrd0; goto return4;}  // stop early if virtual pivot found, but take credit for the column we process here
      while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
       if(minimpthiscol>=*(D*)&incumbentimpi)break;  // if not new global best, don't update global.  In this case we don't need to remember the value of this column, since it has been beaten
       if(__atomic_compare_exchange_n((I*)&(ctx->minimp),&incumbentimpi,*(I*)&minimpthiscol,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){
        minimpfound=minimpthiscol;  // wait till now to remember best-in-thread, because numerical error may allow minimpthiscol to be not as good as an earlier column
        bestcol=colx; bestcolrow=minrownum|(((I)zv&ZVFFREQD)<<(31-ZVFFREQDX));  // update the found position
        zv=(D*)((I)zv|ZVMOD);  // set flag indicating 'this was an improvement'
        break;
       }  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
      }
     }else{
      // the best SPR was dangerous.  We don't update the improvement, and we don't try to pick the best improvement.  Rather, we keep the least dangerous pivot, i. e. the largest
      if(bestcol<0&&minc>maxdangerc){  // if we have not found a nondangerous pivot yet and this is a new local best among dangerous pivots...
       bestcol=colx|(1LL<<31); bestcolrow=minrownum|(((I)zv&ZVFFREQD)<<(31-ZVFFREQDX));  // update the found position as dangerous, with forcefeasible flag in bit 31.
       maxdangerc=minc;  // remember the largest dangerous pivot we have found here
      }
     }
    }
   }
   --bvgrd;  // undo the +1 in the product-accounting below

   // we have finished processing the column, and reporting its gain if DIP.  Now prepare for the next column
abortcol: ; // jump here if column aborted early, possibly on insufficient gain.  This is the normal path
   // exit if we have processed enough columns (in DIP mode)
   I nimpandcols=__atomic_add_fetch(&ctx->nimpandcols,(((I)zv&ZVMOD)<<(32-ZVMODX))+1,__ATOMIC_ACQ_REL);  // add on one finished column, and 0 or 1 improvements
   if(unlikely((nimpandcols>>32)>AN(sched)))goto earlycol;  // if we have more improvements than the schedule imagined, finish
   if(unlikely((nimpandcols>>32)>0 && (UI4)nimpandcols>=IAV(sched)[(nimpandcols>>32)-1]))goto earlycol;  // if we have processed enough columns for this level of improvements, finish
   ndotprods+=bvgrd-bvgrd0+1;  // accumulate # products performed, including the one we aborted out of
   // prepare for next column
   *(I*)&minimp=__atomic_load_n((I*)&(ctx->minimp),__ATOMIC_ACQUIRE);  // update to find best improvement in any thread, to allow cutoff
   currcolproxy=__atomic_load_n(&ctx->nextcol,__ATOMIC_ACQUIRE);  // start load of column position for next spin through loop
   if(unlikely(minimp==infm))goto earlycol;  // if improvement has been pegged at limit, it's a signal to stop
  }  // this is the end of retryable loop-by-hand
  ++firstcol;  // update for next time
 }  // end of loop over columns
earlycol:;  // come here when we want to stop after the current column

 // return result to the ctx, if we have the winning value.  Simple writes to ctx do not require RELEASE order because the end-of-job code
 // and the threads sync through ctx->internal.nf; but they should be atomic in case we run on a small-word machine

 if(!((I)zv&ZVISDIP))R 0;  // if single column, ctx is unused for return; if nonimp, to get here there must be no pivot found, so just return then too
 // Here we are running DIP and reporting our best result (if it beats the incumbent)
 // operation complete; transfer results back to ctx.  To reduce stores we jam the col/row together
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 I incumbent;  // incumbent value, as integer (may be maxdanger or minimp)
 I bestcolandrow=((I)bestcol<<32)|bestcolrow;  // combined value to store: row, column, forcefeasible flag
 if(likely(bestcol>=0)){
  // nondangerous pivot found.  Store it out, under lock, if the local improvement equals the best found so far
  incumbent=__atomic_load_n((I*)&(ctx->minimp),__ATOMIC_ACQUIRE);  // load incumbent best value
  if(minimpfound==*(D*)&incumbent){  // check outside of lock for perf
   WRITELOCK(ctx->ctxlock)
   if(minimpfound==ctx->minimp){ctx->bestcolandrow=bestcolandrow;} // if we have the best gain, report where we found it
   WRITEUNLOCK(ctx->ctxlock)
  }
 }else{
  // dangerous pivot or no pivot found.  Store, under lock, if we have the largest column value and the incumbent is also dangerous or missing
  incumbent=__atomic_load_n((I*)&(ctx->maxdanger),__ATOMIC_ACQUIRE);  // load incumbent best value
  I incumbcol=__atomic_load_n(&(ctx->bestcolandrow),__ATOMIC_ACQUIRE);  // ...and col/row (neg if none or dangerous)
  if(incumbcol<0 && maxdangerc>*(D*)&incumbent){  // check outside of lock for perf
   WRITELOCK(ctx->ctxlock)
   if(ctx->bestcolandrow<0 && maxdangerc>ctx->maxdanger){ctx->bestcolandrow=bestcolandrow; ctx->maxdanger=maxdangerc;}   // if we still have the max value, report where we found it
   WRITEUNLOCK(ctx->ctxlock)
  }
 }
 R 0;

// following are the early return points: nonimproving pivot, unbounded, pivot out virtual

return2:  // (nonimp) here we found a nonimproving pivot.  Cut off other threads, Report it if it is best, then return
 __atomic_store_n(&ctx->nextcol,collen,__ATOMIC_RELEASE);  // set next-'column' to 'finished' to suppress any further reservations
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 // save the pivot we found only if it is in the smallest 'column' (which means smallest bvgrd)
 I newval=((I)bestcol<<32)|bestcolrow;  // the composite we want to store, always positive
 I oldval=__atomic_load_n(&(ctx->bestcolandrow),__ATOMIC_ACQUIRE);  // incumbent, negative to begin with
 while(1){  // put the minimum found into the ctx for the job so that all threads can cut off quickly
  if((UI)newval>=(UI)oldval)break;  // if not new global best, don't update global.
  if(__atomic_compare_exchange_n(&(ctx->bestcolandrow),&oldval,newval,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))break;  // write; if written exit loop, otherwise reload incumbent.  Indicate we made an improvement
 }
 R 0;
return4:  // we have a preemptive DIP/gradient result.  store and set minimp =-inf to cut off all threads
 // the possibilities are unbounded and pivoting out a virtual.  We indicate unbounded by row=-1.  We may overstore another thread's result; that's OK
 // We must do this under lock to make sure all stores to bestcol/row have checked the minimp they use
 __atomic_fetch_add(&ctx->ndotprods,ndotprods,__ATOMIC_ACQ_REL);  // accumulate stats for the work done here: dot-products
 WRITELOCK(ctx->ctxlock)
 ctx->minimp=infm;  // shut off other threads
 ctx->bestcolandrow=((I)bestcol<<32)|bestcolrow;  // set unbounded col or col/row of virtual pivot, plus forcefeasible flag
 WRITEUNLOCK(ctx->ctxlock)
 R 0;
}
#endif //C_AVX2

// 128!:9 matrix times sparse vector with optional early exit, quad precision
// product mode (aka one-column mode):
//  y is ndx;Ax;Am;Av;(M, shape 2,m,n);threshold  where ndx is an atom
//  if ndx<m, the column is ndx {"1 M; otherwise ((((ndx-m){Ax) ];.0 Am) {"1 M) +/@:*"1 ((ndx-m){Ax) ];.0 Av
//  Do the product in quad precision
//  Result for product mode (exitvec is scalar) is the product, one column of M, shape 2,m.  Values closer to 0 than the threshold are clamped to 0
// DIP mode:
//  Only the high part of M is used normally
//  y is ndx;Ax;Am;Av;(M, shape 2,m,n);parms;bkgrd;bk;Frow;sched
//  parms is QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow
//   QpThresh: if |column value| < QpThresh, recalculate in quad precision
//   Col0Threshold is minimum value considered valid for a column value (smaller are considered 0)
//   ColBk0Threshold is minimum value that will block a pivot when bk=0
//   ColDangerPivot is the smallest allowed pivot, but is dangerous
//   ColOkPivot is the smallest pivot value considered non-dangerous
//   Bk0Threshold is the smallest bk value considered nonzero
//   PriRow is the priority row (usually a virtual row) - if it can be pivoted out, we choose the column that does so
//  sched is list of integers: after i{sched columns, stop if we have found i improvements
//  Result is rc,best row,best col,#cols scanned,#dot-products evaluated,best gain  (if rc e. 0 1 2)
//            rc,failing column of NTT, an element of ndx (if rc=4)
//   rc=0 is good; rc=1 pivot requires forcefeasible; rc=3 no pivot found, stall; rc=4 means the problem is unbounded (only the failing column follows)
//   rc=5 (not created - means problem is infeasible) rc=6=empty M, problem is malformed
// gradient mode:
//  y is ndx;Ax;Am;Av;(M, shape 2,m,n);parms;bk;Frow;sched
//   find the (col,row) to maximize colvalue on the edge with the most negative gradient, which is Frow/sqrt(>: sum of squared column values)
//  parms is QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow
// nonimp mode:
//  y is ndx;Ax;Am;Av;(M, shape 2,m,n);(parms as above, only first 4 needed);bkgrd
//   stop when first nonimproving pivot if sound.  ndx must be a singleton list

// if the exclusion list is given, we use it to prevent repetition of basis.  Yk must be given, holding the column #s of the rows of M
// Rank is infinite
F1(jtmvmsparse){PROLOG(832);
#if C_AVX2
if(AN(w)==0){
 // empty arg just returns stats
 R mtv;
}
 ASSERT(AR(w)==1,EVRANK);
 ASSERT(AN(w)>=5,EVLENGTH);  // audit overall w
 ASSERT(AT(w)&BOX,EVDOMAIN);
 A box0=C(AAV(w)[0]), box1=C(AAV(w)[1]), box2=C(AAV(w)[2]), box3=C(AAV(w)[3]), box4=C(AAV(w)[4]), box5=C(AAV(w)[5]);
 // check ranks
 ASSERT(AR(box0)<=1,EVRANK);  // ndx
 ASSERT(AR(box1)==3&&AS(box1)[1]==2&&AS(box1)[2]==1,EVRANK);  // Ax, shape cols,2 1
 ASSERT(AR(box2)==1,EVRANK);  // Am
 ASSERT(AR(box3)==1,EVRANK);  // Av
 ASSERT(AR(box4)==3,EVRANK);  // M and bk must be quad
 // abort if no columns
 if(AN(box0)==0)R num(6);  // if no cols (which happens at startup, return error indic)
 // check types.  Don't convert - force the user to get it right
 ASSERT(AT(box1)&INT,EVDOMAIN);  // Ax, shape cols,2 1
 ASSERT(AT(box2)&INT,EVDOMAIN);  // Am
 ASSERT(AT(box3)&FL,EVDOMAIN);  // Av
 ASSERT(AT(box4)&FL,EVDOMAIN);  // M
 ASSERT(AT(box5)&FL,EVDOMAIN);  // parms
 // check agreement
 ASSERT(AS(box2)[0]==AS(box3)[0],EVLENGTH);   // Am and Av
 ASSERT(AS(box4)[AR(box4)-2]==AS(box4)[AR(box4)-1],EVLENGTH);   // M is square

 // indexes must be an atom or a single list of integers
 // we don't allow conversion so as to force the user to get it right, for speed
 ASSERT(AT(box0)&INT,EVDOMAIN);
 // extract pointers to tables

 I n=AS(box4)[1];  // n=#rows/cols in M
 // convert types as needed; set ?v=pointer to data area for ?
 D *bv; // pointer to b values if there are any
 D *parms=DAV(box5);  // pointer to misc processing controls
 D *Frow;  // pointer to selector-row values
 I bestcol=1LL<<(BW-1), bestcolrow=0;  // col# and row#+mask for best value found from previous columns, init to no col found, and best value 'dangerous or not found'
 A z; D *zv;  // pointer to result, and output for product mode
 I *bvgrd0, *bvgrde;  // bkgrd: the order of processing the rows, and end+1 ptr
 A sched;  // exit schedule
 D minimp;  // DIP: min (=best) improvement found so far; gradient: min (=best) value of recip of gradient^2, i. e sumsq / (Frow^2) 

 // Run the operation on every thread we have, if it is big enough (as it usually will be)
 UI nthreads;  // number of threads we will use

 if(AR(box0)==0){
  // single index value.  set bv=0, zv non0 as a flag that we are storing the column
  bv=0; Frow=0; ASSERT(AN(w)==6,EVLENGTH);  // if column is an atom, set bv=0 to indicate that bv is not used; Frow=0 to indicate not gradient and verify no more input
  nthreads=(*JT(jt,jobqueue))[0].nthreads+1; nthreads=n<1000?1:nthreads;  // a wild guess at what 'too small' would be for a single column
  if(unlikely(n==0)){R reshape(drop(num(-1),shape(box4)),zeroionei(0));}   // empty M, each product is 0
  ASSERT(AR(box5)==0,EVRANK);  // thresh must be a float atom
  I epcol=AR(box4)==3;  // flag if we are doing an extended-precision column fetch
  GATV(z,FL,n<<epcol,1+epcol,AS(box4)); zv=DAV(z);  // allocate the result area for column extraction.  Set zv nonzero so we use bkgrd of i. #M
  bvgrd0=ONECOLGRD0;  // for comp. ease, shift bv to constant value; length calculated per thread
 }else{
  // A list of index values.  We are doing the DIP calculation/gradient/nonimp
  zv=0;  // clear zv as flag to indicate not one-col
  ASSERT(AR(box5)<=1,EVRANK); ASSERT(AT(box5)&FL,EVDOMAIN); ASSERT(BETWEENC(AN(box5),4,8),EVLENGTH);  // 4-8 float constants
  ASSERT(AN(w)>=7,EVLENGTH); 
  if(unlikely(n==0)){RETF(num(6))}   // empty M - should not occur, give error result 6
  if(AN(box0)==0){RETF(num(6))}  // empty ndx - give error/empty result 6
  nthreads=(*JT(jt,jobqueue))[0].nthreads+1;   // start nthreads calc: the total # threads
  if(AN(w)==7||AN(w)==10){
   // DIP/nonimp, which have bvgrd
   A box6=C(AAV(w)[6]);
   if(AN(box6)==0){RETF(num(6))}  // empty bkgrd - give error/empty result 6
   ASSERT(AR(box6)==1,EVRANK); ASSERT(AT(box6)&INT,EVDOMAIN); bvgrd0=IAV(box6); bvgrde=bvgrd0+AN(box6);  // bkgrd: the order of processing the rows, and end+1 ptr   normally /: bk
   nthreads=AN(box6)*AN(box0)<1000?1:nthreads;  // a wild guess at what 'too small' would be for DIP/nonimp
   DO(AN(box6), ASSERT((UI)bvgrd0[i]<(UI)n,EVINDEX); )  // verify bv indexes in bounds if M not empty
  }else{  // gradient
   bvgrd0=ONECOLGRD0;  // if no bvgrd, set to known low value for comp. ease
   bvgrde=bvgrd0+n;  // use end pointer to indicate length
   nthreads=n*AN(box0)<1000?1:nthreads;  // similar wild guess for gradient
  }
  if(AN(w)==7){  // nonimp mode  ndx;Ax;Am;Av;(M, shape 2,m,n);(parms as above, only first 4 needed);bkgrd  bvgrd* already set
   bv=0;   // indicate not DIP
   Frow=0;  // indicate not gradient
   minimp=0.0;  // minimp not used, but passes through to result
   ASSERT(AN(box0)==1,EVLENGTH)
  }else if(AN(w)==9){  // gradient mode  ndx;Ax;Am;Av;(M, shape 2,m,n);(parms as above);bk;Frow;sched
   A box6=C(AAV(w)[6]), box7=C(AAV(w)[7]), box8=C(AAV(w)[8]);
   ASSERT(AT(box6)&FL,EVDOMAIN); ASSERT(AR(box6)==AR(box4)-1,EVRANK); ASSERT(AS(box6)[0]==AS(box4)[0]&&AS(box6)[1]==AS(box4)[1],EVLENGTH); bv=DAV(box6);  // bk, one per row of M
   ASSERT(AR(box7)<=1,EVRANK); ASSERT(AT(box7)&FL,EVDOMAIN); ASSERT(AN(box7)==n+AS(box1)[0],EVLENGTH);  // Frow must be as long as the NTT
   Frow=DAV(box7);  // Frow, one per row of M and column of A.  Nonzero indicates gradient
   ASSERT(AR(box8)<=1,EVRANK) ASSERT(((AN(box8)-1)|SGNIF(AT(box8),INTX))<0,EVDOMAIN)   // must be INT if not empty
   sched=box8;  // exit schedule
   minimp=inf;  // improvements are positive and work down from infinity
  }else{  // DIP
   ASSERT(AN(w)==10,EVLENGTH)  // ndx;Ax;Am;Av;(M, shape 2,n,n);parms;bkgrd;bk;Frow;sched
   A box7=C(AAV(w)[7]), box8=C(AAV(w)[8]), box9=C(AAV(w)[9]);
   ASSERT(AT(box7)&FL,EVDOMAIN); ASSERT(AR(box7)==AR(box4)-1,EVRANK); ASSERT(AS(box7)[0]==AS(box4)[0]&&AS(box7)[1]==AS(box4)[1],EVLENGTH); bv=DAV(box7);  // bk, one per row of M
   ASSERT(AR(box8)<=1,EVRANK); ASSERT(AT(box8)&FL,EVDOMAIN); ASSERT(AN(box8)==n+AS(box1)[0],EVLENGTH);  // Frow must be as long as the NTT
   Frow=DAV(box8);  // Frow, one per row of M and column of A
   ASSERT(AR(box9)<=1,EVRANK) ASSERT(((AN(box9)-1)|SGNIF(AT(box9),INTX))<0,EVDOMAIN)   // must be INT if not empty
   sched=box9;  // exit schedule
   minimp=0.0;  // improvements are negative and work down from 0.  Mustn't go positive because of inequality
  }
 }

#define YC(n) .n=n,
struct mvmctx opctx={.ctxlock=0,.bestcolandrow=-1,.ndxa=box0,YC(n)YC(bv)YC(zv)
 YC(bvgrd0)YC(bvgrde)YC(parms)YC(sched)YC(Frow)YC(minimp).axv=((I(*)[2])IAV(box1))-n,.amv0=IAV(box2),.avv0=DAV(box3),.qk=box4,  // minimp cannot be _ because it may be multiplied by 0
 .ndotprods=0,.nimpandcols=0,.nextcol=0,.maxdanger=0.0};  // bias axv pointer back to start of (virtual) NTT
#undef YC

 jtjobrun(jt,(unsigned char (*)(JJ, void *, UI4))jtmvmsparsex,&opctx,nthreads,0);  // go run the tasks - default to threadpool 0
 // atomic sync operation on the job queue guarantee that we can use regular loads from opctx

 // prepare final result
 if(likely(bv!=0||zv==0)){
  // DIP call, gradient, or nonimp.  onecol returns a result list, allocated above
  GAT0(z,FL,6,1); zv=DAV(z);
  I bestcolandrow=opctx.bestcolandrow;  // Get col/row, each with sign-bit flags
  if(bestcolandrow==-1){zv[0]=3; zv[1]=0; zv[2]=0;  // no pivot found, so indicate
  }else if(unlikely((I4)bestcolandrow==-1)){zv[0]=4; zv[1]=(bestcolandrow>>32)&0x7fffffff; zv[2]=0; // unbounded (row=-1), so indicate
  }else{
   zv[0]=(bestcolandrow>>31)&1;  // good rc: 0/1 indicating forcefeasible
   zv[1]=((bv!=0?bestcolandrow:0)>>32)&0x7fffffff; zv[2]=bestcolandrow&0x7fffffff;   // row/col, with the flags removed.  For nonimp, set col to 0
  }
  zv[3]=(UI4)opctx.nimpandcols; zv[4]=opctx.ndotprods; zv[5]=opctx.minimp;  // other stats: #cols, #dotproducts, best improvement
 }  // if call is just to fetch the column, return it, it's already in z
 EPILOG(z);
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
 I dpflag=0;  // precision flags: 1=Qk 2=pivotcolnon0 4=newrownon0 8=mplr exists
 D *qkv=DAV(qk); I qksize=AS(qk)[AR(qk)-1]; I t=AR(prx)+1; t=(t!=1)?qksize:t; I qksizesq=qksize*t; dpflag|=AR(qk)>AR(prx)+1;  // pointer to qk data, length of a row, offset to low part if present.  offset is qksize^2, or bksize
 UI rowx=ti*rowsperthread, rown=AN(prx), slicen=rown; slicen=rown<rowx+rowsperthread?slicen:rowx+rowsperthread;   // current row, ending row+1 taken for the current task#
 I *rowxv=IAV(prx); D *pcn0v=DAV(pivotcolnon0); dpflag|=(AR(pivotcolnon0)>1)<<1;  // address of row indexes, column data
 UI coln=AN(pcx); I *colxv=IAV(pcx); D *prn0v=DAV(newrownon0); dpflag|=(AR(newrownon0)>1)<<2;  // # cols, address of col indexes. row data
 dpflag|=AR(absfuzzmplr)<<3;  // set flag if mplr is given
 // for each row
 for(;rowx<slicen;++rowx){
  // get the address of this row in Qk
  D *qkvrow=qkv+rowxv[rowx]*qksize;   // the row of Qk being modified
  // fetch the pivotcol value into all lanes
  pcoldh=_mm256_set1_pd(pcn0v[rowx]);  // fetch high part of pivotcol value
  if(dpflag&2)pcoldl=_mm256_set1_pd(pcn0v[rown+rowx]);  // fetch low part if it is present
  UI colx; I okwds=NPAR; I okmsk=(1LL<<NPAR)-1;  //  number/mask of valid wds in block
  // for each column-group
  for(colx=0;colx<coln;colx+=okwds){
   __m256d endmask;  // mask for maskload and gather, indicating # words to process
   __m256i prn0x;  // indexes of nonzero values in row
   // get the mask of valid values, fetch pivotrow values, fetch the Qk indexes to modify
   if(coln-colx>=NPAR){  // all lanes valid
    prn0x=_mm256_loadu_si256((__m256i_u*)(colxv+colx));  // load the indexes into Qk
    prowdh=_mm256_loadu_pd(prn0v+colx);  // load next 4 non0 values in pivotrow
    if(dpflag&4)prowdl=_mm256_loadu_pd(prn0v+coln+colx);  // and low part if present
    if(dpflag&8)mabsfuzz=_mm256_loadu_pd(mplrd+colx);
    endmask=sgnbit;  // indicate all lanes valid
   }else{
    endmask=_mm256_loadu_pd((double*)(validitymask+NPAR-(coln-colx)));  // mask of valid lanes
    okmsk=_mm256_movemask_pd(endmask);  // mask of valid words in this block - always at least 1
    prn0x=_mm256_maskload_epi64(colxv+colx,_mm256_castpd_si256(endmask));  // load the indexes into Qk
    prowdh=_mm256_maskload_pd(prn0v+colx,_mm256_castpd_si256(endmask));  // load next 4 non0 values in pivotrow
    if(dpflag&4)prowdl=_mm256_maskload_pd(prn0v+coln+colx,_mm256_castpd_si256(endmask));  // and low part if present
    if(dpflag&8)mabsfuzz=_mm256_maskload_pd(mplrd+colx,_mm256_castpd_si256(endmask));  // load next 4 non0 values in pivotrow
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
    okmsk=_mm256_movemask_pd(endmask);  // mask of valid words in this block - always at least 1
    okwds=(0b100000000110010010>>okmsk)&7;  // Advance to next nonrepeated column.  valid values are 1 3 7 15 for which we want results 1 2 3 4
   }
   // gather the high parts of Qk
   __m256d qkvh=_mm256_mask_i64gather_pd(_mm256_setzero_pd(),qkvrow,prn0x,endmask,SZI);
   // create max(abs(qkvh),abs(pcoldh*prowdh)) which will go into threshold calc
   __m256d maxabs=_mm256_max_pd(_mm256_andnot_pd(sgnbit,qkvh),_mm256_andnot_pd(sgnbit,_mm256_mul_pd(pcoldh,prowdh)));
   if(!(dpflag&1)){
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
    // (iph,ipl) = - prowdh*pcoldh
    TWOPROD(prowdh,pcoldh,iph,ipl)  // (prowdh,pcoldh) to high precision
    ipl=_mm256_fmadd_pd(prowdh,pcoldl,ipl); ipl=_mm256_fmadd_pd(prowdl,pcoldh,ipl);  // accumulate middle pps
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
 if(AR(prx)!=0){ASSERT(AS(w)[AR(w)-1]==AS(w)[AR(w)-2],EVLENGTH) DO(AN(prx), ASSERT(IAV(prx)[i]<AS(w)[AR(w)-2],EVINDEX))} else{ASSERT(IAV(prx)[0]==0,EVINDEX)}  // Qk must be square; bk not; valid row indexes
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
