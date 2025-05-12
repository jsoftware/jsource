/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Catenate and Friends                                             */

#include "j.h"


static A jtovs0(J jt,B p,I r,A a,A w){A a1,e,q,x,y,z;B*b;I at,*av,c,d,j,k,f,m,n,t,*v,wr,*ws,wt,zr;P*wp,*zp;
 ws=AS(w); wr=AR(w); f=wr-r; zr=wr+!r;
 ASSERT(IMAX>ws[f],EVLIMIT);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); y=SPA(wp,i); m=AS(y)[0]; 
 a1=SPA(wp,a); c=AN(a1); av=AV(a1); RZ(b=bfi(zr,a1,1));
 at=AT(a); wt=AT(x);
 ASSERT(HOMO(at,wt),EVINHOMO);
 t=maxtype(at,wt);
 ASSERT(t&SPARSABLE,EVDOMAIN);  // verify supported sparse type
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt)){RZ(x=cvt(t,x)); RZ(e=cvt(t,e));}
 j=k=0; DO(f, if(b[i])++j; else ++k;);
 switch(2*b[f]+!equ(a,e)){
 case 0:  /* dense and a equal e */
  RZ(y=ca(y)); 
  RZ(x=p?irs2(x,a,DUMMYSELF,AR(x)-(1+k),0L,jtover):irs2(a,x,DUMMYSELF,0L,AR(x)-(1+k),jtover)); 
  break;
 case 1:  /* dense and a not equal to e */
  GATV0(q,INT,c,1); v=AV1(q); DO(c, v[i]=ws[av[i]];); RZ(q=odom(2L,c,v));
  if(AN(q)>=AN(y)){
   RZ(z=shape(x)); *AV(z)=AS(q)[0]; 
   RZ(x=from(grade1(over(y,less(q,y))),over(x,reshape(z,e))));
   y=q;
  }
  RZ(x=p?irs2(x,a,DUMMYSELF,AR(x)-(1+k),0L,jtover):irs2(a,x,DUMMYSELF,0L,AR(x)-(1+k),jtover));
  break;
 case 2:  /* sparse and a equals e */
  RZ(y=ca(y)); 
  if(!p){v=j+AV(y); DQ(m, ++*v; v+=c;);} 
  break;
 case 3:  /* sparse and a not equal to e */
  GATV0(q,INT,c,1); v=AV1(q); DO(c, v[i]=ws[av[i]];); v[j]=1; RZ(q=odom(2L,c,v)); n=AS(q)[0];
  if(p){RZ(y=over(y,q)); v=AV(y)+j+m*c; d=ws[f]; DQ(n, *v=d; v+=c;);}
  else {RZ(y=over(q,y)); v=AV(y)+j+n*c;          DQ(m, ++*v; v+=c;);}
  RZ(q=shape(x)); *AV(q)=n; RZ(q=reshape(q,a)); RZ(x=p?over(x,q):over(q,x));
  if(f){RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));}
 }
 GASPARSE(z,STYPE(t),1,zr,ws); 
 if(r)++AS(z)[f]; else AS(z)[wr]=2;
 A bvec=ifb(zr,b); makewritable(bvec)  // avoid readonly
 zp=PAV(z); SPB(zp,a,bvec); SPB(zp,e,e); SPB(zp,i,y); SPB(zp,x,x);
 R z;
}    /* a,"r w (0=p) or w,"r a (1=p) where a is scalar and w is sparse */

static F2(jtovs){F12IP;A ae,ax,ay,q,we,wx,wy,x,y,z,za,ze;B*ab,*wb,*zb;I acr,ar,*as,at,c,m,n,r,t,*v,wcr,wr,*ws,wt,*zs;P*ap,*wp,*zp;
 ARGCHK2(a,w);
 acr=jt->ranks>>RANKTX; ar=AR(a); at=AT(a); acr=ar<acr?ar:acr; 
 wcr=(RANKT)jt->ranks; wr=AR(w); wt=AT(w); wcr=wr<wcr?wr:wcr; RESETRANK; 
 if(!ar)R ovs0(0,wcr,a,w);
 if(!wr)R ovs0(1,acr,w,a);
 if(ar>acr||wr>wcr)R sprank2(a,w,NOEMSGSELF,acr,wcr,jtover);
 r=MAX(ar,wr);
 if(r>ar)RZ(a=reshape(over(apv(r-ar,1L,0L),shape(a)),a)); as=AS(a);
 if(r>wr)RZ(w=reshape(over(apv(r-wr,1L,0L),shape(w)),w)); ws=AS(w);
 ASSERT(*as<IMAX-*ws,EVLIMIT);
/* non-sparse array needs to know the final sparse data type for sparseit */
 if((ISSPARSE(at)>ISSPARSE(wt))&&(at!=(t=maxtype(at,wt)))){
 RZ(a=cvt(t,a));at=AT(a);}
 else if((ISSPARSE(at)<ISSPARSE(wt))&&(wt!=(t=maxtype(at,wt)))){
 RZ(w=cvt(t,w));wt=AT(w);}
 if(!ISSPARSE(at)){wp=PAV(w); RZ(a=sparseit(a,SPA(wp,a),SPA(wp,e)));}
 if(!ISSPARSE(wt)){ap=PAV(a); RZ(w=sparseit(w,SPA(ap,a),SPA(ap,e)));}
 ap=PAV(a); RZ(ab=bfi(r,SPA(ap,a),1)); ae=SPA(ap,e); at=AT(ae);
 wp=PAV(w); RZ(wb=bfi(r,SPA(wp,a),1)); we=SPA(wp,e); wt=AT(we);
 ASSERT(equ(ae,we),EVNONCE);
 GATV0(q,B01,r,1); zb=BAV1(q); DO(r, zb[i]=ab[i]||wb[i];); za=ifb(r,zb); makewritable(za) c=AN(za);  // avoid readonly
 GATV0(q,INT,r,1); zs= AV1(q); DO(r, zs[i]=MAX(as[i],ws[i]););
 DO(r, if(zb[i]>ab[i]){RZ(a=reaxis(za,a)); break;});
 DO(r, if(zb[i]>wb[i]){RZ(w=reaxis(za,w)); break;});
 *zs=*as; DO(r, if(zs[i]>as[i]){RZ(a=take(q,a)); break;});
 *zs=*ws; DO(r, if(zs[i]>ws[i]){RZ(w=take(q,w)); break;});
 *zs=*as+*ws; t=maxtype(at,wt);
 ap=PAV(a); ay=SPA(ap,i); ax=SPA(ap,x); if(TYPESNE(t,at))RZ(ax=cvt(t,ax));
 wp=PAV(w); wy=SPA(wp,i); wx=SPA(wp,x); if(TYPESNE(t,wt))RZ(wx=cvt(t,wx));   // TYPESNE((t,at) again ???
 GASPARSE(z,STYPE(t),1,r,zs); zp=PAV(z);
 SPB(zp,a,za); SPBV(zp,e,ze,ca(TYPESEQ(t,at)?ae:we));
 if(*zb){
  SPB(zp,x,  over(ax,wx));
  SPBV(zp,i,y,over(ay,wy)); v=AV(y)+AN(ay); m=*as; DQ(AS(wy)[0], *v+=m; v+=c;);
 }else{C*av,*wv,*xv;I am,ak,i,j,k,mn,p,*u,wk,wm,xk,*yv;
  i=j=p=0; k=bpnoun(t); 
  m=AS(ay)[0]; u=AV(ay); av=CAV(ax); am=aii(ax); ak=k*am;
  n=AS(wy)[0]; v=AV(wy); wv=CAV(wx); wm=aii(wx); wk=k*wm; mn=m+n; xk=k*(am+wm);
  GATVR(y,INT,mn*c,      2,     AS(ay)); yv= AV2(y); AS(y)[0]=mn;                 
  I xr=AR(ax); GA(x,t,mn*(am+wm),AR(ax),AS(ax)); xv=CAVn(xr,x); AS(x)[0]=mn; AS(x)[1]=*zs; mvc(k*AN(x),xv,k,AV(ze));
  while(i<m||j<n){I cmp;
   if     (i==m)cmp= 1; 
   else if(j==n)cmp=-1;
   else         {cmp=0; DO(c, if(u[i]!=v[i]){cmp=u[i]<v[i]?-1:1; break;});}
   switch(cmp){
   case -1: ICPY(yv,u,c); u+=c; ++i; MC(xv,   av,ak); av+=ak; break;
   case  0: ICPY(yv,u,c); u+=c; ++i; MC(xv,   av,ak); av+=ak; ++p;  /* fall thru */
   case  1: ICPY(yv,v,c); v+=c; ++j; MC(xv+ak,wv,wk); wv+=wk;
   }
   yv+=c; xv+=xk;
  }
  SPB(zp,i,p?take(sc(mn-p),y):y); SPB(zp,x,p?take(sc(mn-p),x):x);
 }
 RETF(z);
}    /* a,"r w where a or w or both are sparse */

// k is length of an atom; c is atoms/result item; m is #items to move; z is result; w->arg to move; x->area to move to; z is A block for output area; somefill is neg if there is any fill
// result points to end+1 of area filled
// NOTE: AS(z)[0] is overwritten
static C*jtovgmove(J jt,I k,I c,I m,A z,A w,C*x,I somefill){I d,n,p=c*m;  // p=#atoms in result
   // z may not be boxed; but if it is, w must be also.
 if(likely(AR(w))){
// obsolete   n=AN(w); d=AN(s)-AR(w);
  n=AN(w); d=AR(z)-AR(w);   // d is number of added axes required to bring w up to shape of result
  if((~somefill|(-n&(d-1)))>=0)mvc(k*p,x,k,jt->fillv);  // fill required: fill needed somewhere, and w empty or shape short (d>0).  Fills unnecessarily if axis was extended with 1s and the other arg needed fill
  if(likely(n!=0)){  // nonempty cell, must copy in the data
   if(withprob(n<p,0.1)){  // incoming cell smaller than result area: take to result-cell size (uses fill)
// obsolete I *v=AV(s); *v=m; RZ(w=take(d?vec(INT,AR(w),d+v):s,w));
    fauxblockINT(sh,0,1); AK((A)sh)=(C*)(&AS(z)[d])-(C*)sh; AT((A)sh)=INT; AR((A)sh)=1; AN((A)sh)=AS((A)sh)[0]=AR(w);  // create arg to take: INT vector with shape of item of result
#if MEMAUDIT&0xe
    AFLAG((A)sh)=0;  // audits check it
#endif
    AS(z)[0]=m; RZ(w=take((A)sh,w));  // do the take, with fill
   }
   JMC(x,AV(w),k*AN(w),1);  // copy in the data, now the right cell shape but possibly shorter than the fill  kludge could avoid double copy
  }
 }else{  // scalar replication
  mvc(k*p,x,k,AV(w));
 }
 R x+k*p;
}    /* move an argument into the result area */

static F2(jtovg){F12IP;A s,z;C*x;I ar,*as,c,k,m,n,*sv,t,wr,*ws,zn;
 ARGCHK2(a,w);
 // scaf most of this should move back to the caller so it can be outside the rank loop
 ar=AR(a); wr=AR(w);
 // First loop: calculate zn=#result atoms, c=#atoms in result item.  We go through the loop once for each number in the result-rank, fetch m/n (the values).  For all except the first,
 // we process by finding the result axis-length and multiplying it into the total size.  The result axis-length is the larger of the arg axis lengths; the arg lengths come from the shape
 // until the shape is exhausted; then the shape is extended with 1 for arrays, 0 for atoms (scalar extension, which takes the cell-shape from the other argument even if it contains a 0).
 // This overfetches ?r[-1], which is OK.
 as=AS(a); ws=AS(w); I ax=ar-1; I wx=wr-1; c=1; I extenmin=SGNTO0(~(ax|wx));  // if either arg is scalar, extend shape with 0; otherwise 1
 while(1){m=as[ax]; m=ax<0?extenmin:m; ax=ax<0?0:ax; n=ws[wx]; n=wx<0?extenmin:n; wx=wx<0?0:wx; if(ax+wx==0)break; I axlen=MAX(m,n); DPMULDDECLS DPMULDZ(c,axlen,c); if(axlen==0)goto emptyresult; --ax, --wx;}  // fetches AS[-1] which is OK.
 ASSERT(c>0,EVLIMIT);   // we end the loop with 0 product only if some product overflowed and was set to 0
emptyresult:;   // abort loop to come here if a result axis length is 0, which always produces a true empty result
 m=ar==0?1:m; n=wr==0?1:n;  // a scalar arg always has exactly one item
 DPMULDDECLS DPMULDE(c,m+n,zn);  // get total # atoms in result.  m, n invalid if c==0, no problem
 // Now that we have figured out the result size we can decide whether we need fill
 I somefill; I an=AN(a); I wn=AN(w);   // set if we must have a fill atom
 if(unlikely((somefill=((an+wn-zn)&(-MIN(ar,wr))))<0)){    // we need fill only if there are more result atoms than input atoms, and neither arg is an atom (which would replicate). 
  RZ(w=setfv(a,w)); t=AT(w); if(!(/*an!=0&&*/t&AT(a)))RZ(a=cvt(t,a)) // set fill, and convert w to its type.  Then convert a with different type,  Alas, we have to convert empties because take uses fill in ovgmove
 }else if(unlikely(AT(a)!=(t=AT(w)))){   // if no fill but dissimilar types...
  t=maxtypedne(AT(a)|((UI)-an<(UI)wn),t|((UI)-wn<(UI)an)); t=LOWESTBIT(t);   // If types not the same, get result type.  Treat empty arg as boolean if the other is nonempty.
  /*if((-an&-wn)<0)*/{I atdiff=TYPESXOR(t,AT(a)); RZ(z=cvt(t,atdiff?a:w)) a=atdiff?z:a; w=atdiff?w:z;}  // if both args nonempty, convert low arg to result precision.
 }
 I r=MAX(ar,wr); r=MAX(r,1); GA00(z,t,zn,r); x=CAVn(r,z); k=bpnoun(t);  // allocate result; get pointer to data & len of an atom
 as=AS(a); ws=AS(w); ax=ar-1; wx=wr-1; I *zs=&AS(z)[MAX(ax,wx)];  // zs is immaterial if both args are atoms
 // Second loop: fill in shape of z, except for the first value which is overwritten by ovgmove.
 while(1){m=as[ax]; m=ax<0?extenmin:m; ax=ax<0?0:ax; n=ws[wx]; n=wx<0?extenmin:n; wx=wx<0?0:wx; if(ax+wx==0)break; *zs=MAX(m,n); --ax, --wx, --zs;}  // Store max of extended shapes.  fetches AS[-1] which is OK.
 m=ar==0?1:m; n=wr==0?1:n;  // a scalar arg always has exactly one item
 RZ(x=jtovgmove(jt,k,c,m,z,a,x,somefill));
 RZ(x=jtovgmove(jt,k,c,n,z,w,x,somefill));
 AS(z)[0]=m+n;  // m and n are left to first value in (1-extended) rank
 RETF(z);
}    /* a,w general case for dense array with the same type; jt->ranks=~0 */

// these variants copy vectors or scalars, with optional repetition of items and, for the scalars, scalar repetition.  No fill.
// here when ma=mw=SZI
static void moveawVVI(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 if((arptct|wrptct)==0) {
   // fastest case: no replication, no scalars
   I wvo=wv-av;  // so we need increment only av
#if C_AVX2 || EMU_AVX2
   // loop 4 at a time, reading and writing two 32-byte sections
   while((c-=NPAR)>=0){
    __m256i avv=_mm256_loadu_si256((__m256i*)av), wvv=_mm256_loadu_si256((__m256i*)(av+wvo));  // read a0 a1 a2 a3  and w0 w1 w2 w3
    wvv=_mm256_permute4x64_epi64(wvv,0b01001110); // w2 w3 w0 w1
    __m256i awvv=_mm256_blend_epi32(avv,wvv,0b11110000); wvv=_mm256_blend_epi32(avv,wvv,0b00001111);  // a0 a1 w0 w1  and w2 w3 a2 a3
    _mm256_storeu_si256((__m256i*)zv,_mm256_permute4x64_epi64(awvv,0b11011000));  _mm256_storeu_si256((__m256i*)(zv+NPAR*SZI),_mm256_permute4x64_epi64(wvv,0b01110010));  // write a0 w0 a1 w1  and a2 w2 a3 w3
    zv+=2*NPAR*SZI; av+=NPAR*SZI;
   }
   c+=NPAR;   // get back to #Is left
#endif
   DQ(c, *(I*)zv=*(I*)av; *((I*)zv+1)=*(I*)(av+wvo); zv+=2*SZI; av+=SZI;) 
 }else{
  while(--c>=0){
   // copy one cell from a; advance z; advance a if not repeated
   *(I*)zv=*(I*)av; zv+=SZI; --arptct; av+=REPSGN(arptct)&SZI; arptct+=REPSGN(arptct)&arptreset;
   // repeat for w
   *(I*)zv=*(I*)wv; zv+=SZI; --wrptct; wv+=REPSGN(wrptct)&SZI; wrptct+=REPSGN(wrptct)&wrptreset;
  }
 }
}
static void moveawVV(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 JMCDECL(endmaska) JMCSETMASK(endmaska,ma,0)
 JMCDECL(endmaskw) JMCSETMASK(endmaskw,mw,0)
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 if((arptct|wrptct)==0) {
   // fastest case: no replication, no scalars
  while(--c>=0){
   // copy one cell from a; advance z; advance a
   JMCR(zv,av,ma,0,endmaska); zv+=ma; av+=ma;
   // repeat for w
   JMCR(zv,wv,mw,0,endmaskw); zv+=mw; wv+=mw;
  }
 }else{
  while(--c>=0){
   // copy one cell from a; advance z; advance a if not repeated (i. e. rptct=0).  Advance the long frame every time, repeating the shorter
   JMCR(zv,av,ma,0,endmaska); zv+=ma; --arptct; av+=REPSGN(arptct)&ma; arptct+=REPSGN(arptct)&arptreset;
   // repeat for w
   JMCR(zv,wv,mw,0,endmaskw); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&mw; wrptct+=REPSGN(wrptct)&wrptreset;
  }
 }
}
#if 0 // obsolete 
static void moveawVS(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 JMCDECL(endmaska) JMCSETMASK(endmaska,ma,0)
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 while(--c>=0){
  // copy one cell from a; advance z; advance a if not repeated
  JMCR(zv,av,ma,0,endmaska); zv+=ma; --arptct; av+=REPSGN(arptct)&ma; arptct+=REPSGN(arptct)&arptreset;
  // repeat for w
  mvc(mw,zv,k,wv); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&k; wrptct+=REPSGN(wrptct)&wrptreset;
 }
}
static void moveawSV(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 JMCDECL(endmaskw) JMCSETMASK(endmaskw,mw,0)
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 while(--c>=0){
  // copy one cell from a; advance z; advance a if not repeated
  mvc(ma,zv,k,av); zv+=ma; --arptct; av+=REPSGN(arptct)&k; arptct+=REPSGN(arptct)&arptreset;
  // repeat for w
  JMCR(zv,wv,mw,0,endmaskw); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&mw; wrptct+=REPSGN(wrptct)&wrptreset;
 }
}
#else
static void moveawS(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset,B anotatomic){
 JMCDECL(endmask) JMCSETMASK(endmask,anotatomic?ma:mw,0)
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 I aadv=anotatomic?ma:k, wadv=anotatomic?k:mw;  // advance the atomic arg by 1 atom, the other by cellsize.  SYMB means a is not atomic
 while(--c>=0){
  // copy one cell from a; advance z; advance a if not repeated
  if(!anotatomic)mvc(ma,zv,k,av); else JMCR(zv,av,ma,0,endmask) zv+=ma; --arptct; av+=REPSGN(arptct)&aadv; arptct+=REPSGN(arptct)&arptreset;
  // repeat for w
  if(!anotatomic)JMCR(zv,wv,mw,0,endmask) else mvc(mw,zv,k,wv); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&wadv; wrptct+=REPSGN(wrptct)&wrptreset;
 }
}
#endif
// obsolete int (*p[4]) (int x, int y);
// obsolete static void(*moveawtbl[])() = {moveawVV,moveawVS,moveawSV,moveawVVI};
DF2(jtover){F12IP;AD * RESTRICT z;I replct,framect,acr,ar,ma,mw,p,q,t,wcr,wr,zn;
 ARGCHK2(a,w);
 UI jtr=jt->ranks;//  fetch early
 if(unlikely(ISSPARSE(AT(a)|AT(w)))){R ovs(a,w);}  // if either arg is sparse, switch to sparse code
 // Examine args for compatibility.  Treat empty arg as boolean if the other is nonempty.  Do not convert until we know whether we have fill, to avoid a second conversion
 if(unlikely(AT(a)!=(t=AT(w)))){t=maxtypedne(AT(a)|((UI)-AN(a)<(UI)AN(w)),t|((UI)-AN(w)<(UI)AN(a))); t=LOWESTBIT(t)+RPAR; t+=t&AT(a)?0:CONJ;}  // t is result type; if it contains RPAR, a conversion is needed, CONJ is set if a must convert
 ar=AR(a); wr=AR(w);
 acr=jtr>>RANKTX; acr=ar<acr?ar:acr; UI af=ar-acr;  // acr=rank of cell, af=len of frame, as->shape
 wcr=(RANKT)jtr; wcr=wr<wcr?wr:wcr; UI wf=wr-wcr;  // wcr=rank of cell, wf=len of frame, ws->shape
 // no RESETRANK - not required by ovv or main line here
// obsolete  as=AS(a); ws=AS(w);
 PROLOG(000);   // we will allocate our result first so that we can tpop back to it without EPILOG.
 if(af+wf==0){
#if 0  // we don't use ALLOWRETARG anywhere yet
 // if exactly one arg has no items in cell, and the empty does not have longer frame, and the frames agree,
 // and items have the same rank, and the empty item has no axis larger than the nonempty: return the nonempty.
 // Because some of the sparse code writes over the result from here, we enable this only if the call enables it
  I ai=AS(a)[0], wi=AS(w)[0]; wi=wr?wi:ai;  // item counts of args; force miscompare if both atoms
  if(unlikely((I)SGNTO0(-ai^-wi)>((ar^wr)|((I)jtfg&JTALLOWRETARG)))){  // appending empty to nonempty, no frame, equal rank (not 0)
   A ea=ai?w:a, nea=ai?a:w;  // empty & nonempty args
   I emptybig; TESTXITEMSMALL(emptybig,AS(ea),AS(nea),ar) if(!emptybig)R RETARG(nea);  // if item not changing, return nonempty argument unchanged
  }
#endif
  // No frame.  See if ranks are equal or different by 1, and if the items have the same shape
  I lr=ar;  // rank of arg with long shape
  A l=a; l=wr>ar?w:l; lr=wr>ar?wr:lr;  // arg with long shape (a if =).  Not needed till later but we usually go through the fast path
  if(likely(2*lr-1<=ar+wr)){  // if ranks differ by at most 1
   // items have the same rank or one argument is an item of the other (cases where the ranks differ by more than 1 follow the general path below)
   // see if the shapes agree up to the shape of an item of the longer argument
   I cr=lr-((UI)lr>0);
   if(likely(TESTAGREE(AS(a)+ar-cr,AS(w)+wr-cr,cr))){  // compare the tail of the shapes, for the length of an item of the longer shape
    // The data can be copied in toto, with only the number of items changing.
    A s=(A)((I)a+(I)w-(I)l);  // arg with short shape
    // The rank is the rank of the long argument, unless both arguments are atoms; then it's 1
    // The itemcount is the sum of the itemcounts; but if the ranks are different, use 1 for the shorter; and if both ranks are 0, the item count is 2
    // empty items are OK: they just have 0 length but their shape follows the normal rules
    I si=AS(s)[0]; si=ar==wr?si:1; si+=__atomic_load_n(&AS(l)[0],__ATOMIC_ACQUIRE); si=lr==0?2:si; lr=lr==0?1:lr; ASSERT(si>=0,EVLIMIT);  // get short item count; adjust to 1 if lower rank; add long item count; check for overflow; adjust if atom+atom
      // The a block may be being extended in another thread.  We ensure that AS[0] is incremented AFTER AN to ensure that our allocation is adequate.  This can happen only if the apip started before we locked the block for this thread
    I alen=__atomic_load_n(&AN(a),__ATOMIC_ACQUIRE);  // ensure our copy matches the allocation even if AN incremented during allocation
if(lr==1&&si!=alen+AN(w))printf("Allocation with AN=%lld AS[0]=%lld\n",alen+AN(w),si);  // scaf
    GA(z,t&NOUN,alen+AN(w),lr,AS(l)); AS(z)[0]=si; C *x=CAVn(lr,z);   // install # items after copying shape, mark result in tstack
ASSERTSYS(!(AR(z)==1&&AN(z)<AS(z)[0]),"AS>AN");  // scaf
    if(unlikely(t&RPAR)){A zt; RZ(zt=cvt(t&NOUN,t&CONJ?a:w)) a=t&CONJ?zt:a; w=t&CONJ?w:zt;}   // convert the discrepant argument to type t
    I klg=bplg(t); alen<<=klg; I wlen=AN(w)<<klg;  // arg sizes in bytes
    JMC(x,CAV(a),alen,0); JMC(x+alen,CAV(w),wlen,0);
    if(withprob(t&NOUN&RECURSIBLE,0.2)){   //  recursive/pristine processing applies only to RECURSIBLEs 
     // If a & w are both recursive abandoned non-virtual, we can take ownership of the contents by marking them nonrecursive and marking z recursive.
     // We could also zap a & w, but we don't because it's just a box header and it will be freed by a caller anyway
     // We can't transfer ownership if one of the args is VIRTUAL, because a virtual block doesn't really own its contents
     // We can't transfer ownership if a=w, because the counts for the blocks would be one too low

     // The result can be pristine if both inputs are abandoned pristine, and are not the same block (note: if one is boxed, no coonversion will ever happen)
     // If a and w are the same, we mustn't mark the result pristine!  It has repetitions
     I xfer, aflg=AFLAG(a), wflg=AFLAG(w);
     xfer=aflg&wflg&REPSGN((JTINPLACEA-((JTINPLACEA+JTINPLACEW)&(I)jtfg))&AC(a)&AC(w))&-(a!=w);  // flags, if abandoned inplaceable and not the same block
     if(unlikely((xfer&=AFPRISTINE|((aflg|wflg)&AFVIRTUAL?0:RECURSIBLE))!=0)){  // preserve the PRISTINE flag and RECURSIBLE too, if not VIRTUAL
      // a and w are both non-shared blocks, and not VIRTUAL or PERMANENT.  xfer is the transferable recursibility if any, plus inherited pristinity
      AFLAGORLOCAL(z,xfer); xfer|=AFPRISTINE; AFLAGANDLOCAL(a,~xfer) AFLAGANDLOCAL(w,~xfer)  // transfer inplaceability/pristinity; always clear pristinity from a/w
     }else{AFLAGCLRPRIST(w); AFLAGCLRPRIST(a);}  // if we can't transfer pristinity, we must clear it
     // We extracted from a and w, so mark them (or the backer if virtual) non-pristine.  If both were pristine and abandoned, transfer its pristine status to the result
     // if they were boxed nonempty, a and w have not been changed.  Otherwise the PRISTINE flag doesn't matter.
     if(unlikely((aflg&AFVIRTUAL)!=0)){AFLAGPRISTNO(ABACK(a))}  //  like PRISTCOMSETF
     if(unlikely((wflg&AFVIRTUAL)!=0)){AFLAGPRISTNO(ABACK(w))}  //  like PRISTCOMSETF
    }
    if(unlikely(_ttop+1!=jt->tnextpushp))z=EPILOGNORET(z); RETF(z);  // if nothing to pop, return z as is; otherwise we musat EPILOG in case z needs protection from the frees
   }
  }
 }
 // dissimilar items, or there is frame.  Mark the inputs non-pristine; leave the result non-pristine, since we don't know whether it has repetitions (we could figure that out)
 PRISTCLR(a) PRISTCLRNODCL(w)  // make inputs non-PRISTINE   scaf verify that this is threadsafe
 p=AS(a)[ar-1];   // p=len of last axis of cell.  Always safe to fetch first 
 q=AS(w)[wr-1];   //  q=len of last axis of cell
// obsolete  r=MAX(acr,wcr); r=(r==0)?1:r;  // r=cell-rank, or 1 if both atoms.
 if(((MAX(acr,wcr)-3)&-AN(a)&-AN(w)&((acr+wcr-3)|((p^q)-1)))<0){  // r<=2, neither arg empty,  (sum of ranks<3 (if max rank <= 2 and sum of ranks >2, neither can possibly be an atom) OR items (which are lists) have same length)
  // joining atoms, rows, row/atom, or table/row with same lengths, or table/atom; possibly with frame.  In any case no fill is possible, but scalar replication might be
  // scaf this could be generalized to any time ranks differ by at most 1 and the items agree or one is atomic
  I awcrflg=4*acr+wcr;  // incredible register pressure.  Combine af/wf, and shift in flags as we calculate them
  I cc2a=__atomic_load_n(&AS(a)[ar-2],__ATOMIC_RELAXED); p=awcrflg&0b1100?p:1; cc2a=awcrflg&0b1000?cc2a:1; ma=cc2a*p; ma=awcrflg==0b0010?q:ma;  //   cc2a is # 2-cells of a; ma is #atoms in a cell of a EXCEPT when joining atom a to table w: then length of row of w; i. e. #atoms to fill from an item of a
  I cc2w=__atomic_load_n(&AS(w)[wr-2],__ATOMIC_RELAXED); q=awcrflg&0b0011?q:1; cc2w=awcrflg&0b0010?cc2w:1; cc2a+=cc2w; mw=cc2w*q; mw=awcrflg==0b1000?p:mw;  // sim for w; cc2a is combined length of axis -2 f can increment only once
  p=awcrflg&0b1100?p:q; awcrflg=2*awcrflg+((C)(0b100000100>>awcrflg)&(C)(p>1));   // len of 1-cell of result (if r=2); new low flag: scalar extension of more than 1 atom (lens are 2 and 0, len (which can't be 0) > 1
  awcrflg=2*awcrflg+(wf>=af);  t+=awcrflg<<23;  // save flags in t to free up awcrflg
     // new low flag wf>=af, i. e. a is short frame: a must repeat |replct| times between advances, while long frame advances every time (setting ct to 1)
     // *** awcrflg has been shifted and moved into t<<23, low flags are bit 1=(scalar exten) bit 0=(a is short frame) ***
// obsolete   replct^=REPSGN(af-wf);
// obsolete   I f=t&(0b000001<<23)?wf:af; I shortf=t&(0b000001<<23)?af:wf; I *s=AS(t&(0b000001<<23)?w:a);   // long frame, short frame, pointer to long shape
  I f=wf>=af?wf:af; I shortf=wf>=af?af:wf; I *s=AS(wf>=af?w:a);   // long frame, short frame, pointer to long shape
  p=t&(0b101000<<23)?p:ma+mw;  // last axis, depending on rank.  cc2a is 2nd-last axis, valid only if rank=2
// obsolete  I sreps=SGNTO0((acr-1)&(1-ma))*2+(SGNTO0(((wcr-1)&(1-mw))));  // look for scalar reps before subrt call
// obsolete   t|=(SGNTO0(((acr-1)&(1-ma))|((wcr-1)&(1-mw))))<<MARKX;  // set MARK in t if there is any scalar extended to >1 atom
  PROD(replct,f-shortf,s+shortf); f+=1+!!(t&(0b101000<<23)); PROD(framect,shortf,s); framect*=replct;  // Number of cells in a and w; known non-empty shapes; framect becomes size of both frames.  Repurpose f to total result rank
  DPMULDECLS DPMULDE(framect,ma+mw,zn);  // total # atoms in result
  GA(z,t&NOUN,zn,f,s); if(unlikely(AN(z)==0))RETF(z);   // allocate result; repurpose s to point to END+1 of shape field.  Return if area empty so we can use UNTIL loops.  Mark result in tstack
  if(unlikely(t&RPAR)){A zt; RZ(zt=cvt(t&NOUN,t&CONJ?a:w)) a=t&CONJ?zt:a; w=t&CONJ?w:zt;}   // convert the discrepant argument to type t
// obsolete   s=AS(z)+f+r; if(2>r)s[-1]=ma+mw; else{s[-1]=p; s[-2]=cc2a;}  // fill in last 2 atoms of shape
  AS(z)[AR(z)+!(t&(0b101000<<23))-2]=cc2a; AS(z)[AR(z)-1]=p;  // store [cc2a] p to last 1 or 2 items of shape
  I klg=bplg(t);   // # bytes per atom of result
  // copy in the data, creating the result in order (to avoid page thrashing and to make best use of write buffers)
  // scalar replication is required for any arg whose rank is 0 and yet its length is >1.  Choose the copy routine based on that
// obsolete   sreps=(((((ma<<klg)^SZI)+((mw<<klg)^SZI))==0)>sreps)?3:sreps;  // if VV case moving exactly SZI, use routine for that
  C *av=CAV(a), *wv=CAV(w), *zv=CAVn(AR(z),z); ma<<=klg; mw<<=klg;   // set up all 'subrt args', but the functions are inlined here
  I k=1, arptreset=replct; replct=t&(0b000001<<23)?k:replct; arptreset=t&(0b000001<<23)?arptreset:k; k<<=klg;  // repeat counts: aflewf is wf>=af: aw repeats are ~replct/1.  if wf<af, repeats 1/replct
  if(!(t&(0b000010<<23))){if((ma|mw)==SZI)moveawVVI(zv,av,wv,framect,k,ma,mw,arptreset,replct);else moveawVV(zv,av,wv,framect,k,ma,mw,arptreset,replct);  // if no scalar rep.  ?w must not be 0, so this tests both=SZI
// obsolete   }else{if(ma<mw)moveawSV(zv,av,wv,framect,k,ma,mw,arptreset,replct);else moveawVS(zv,av,wv,framect,k,ma,mw,arptreset,replct);}
  }else{moveawS(zv,av,wv,framect,k,ma,mw,arptreset,replct,!!(t&(0b110000<<23)));}   // if scalar rep
// obsolete   moveawtbl[sreps](CAV(z),CAV(a),CAV(w),replct*framect,(I)1<<klg,ma<<klg,mw<<klg,(wf>=af)?replct:1,(wf>=af)?1:replct);
  if(unlikely(_ttop+1!=jt->tnextpushp))z=EPILOGNORET(z); RETF(z);  // if nothing to pop, return z as is; otherwise we musat EPILOG in case z needs protection from the frees
 }
 // if max cell-rank>2, or an argument is empty, or (joining table/table or table/row with cells of different lengths), do general case
 // no special cases apply, perform general copy with rank and fill
 RESETRANK; z=rank2ex(a,w,self,acr,wcr,acr,wcr,jtovg); RETF(z);  // ovg calls other functions, so we clear rank.  rank2ex does EPILOG
}    /* overall control, and a,w and a,"r w for cell rank <: 2 */

DF2(jtstitch){F12IP;I ar,wr; A z;
 ARGCHK2(a,w);
 ar=AR(a); wr=AR(w);
 ASSERT((-ar&-wr&-(AS(a)[0]^AS(w)[0]))>=0,EVLENGTH);  // a or w scalar, or same # items    always OK to fetch s[0]
 if(likely(((SGNIFDENSE(AT(a)|AT(w)))&(2-ar)&(2-wr))>=0))R IRSIP2(a,w,self,(ar-1)&RMAX,(wr-1)&RMAX,jtover,z);  // not sparse or rank>2
 R stitchsp2(a,w);  // sparse rank <=2 separately
}

F1(jtlamin1){F12IP;A x;I* RESTRICT s,* RESTRICT v,wcr,wf,wr; 
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; RESETRANK; wf=wr-wcr;
 fauxblockINT(wfaux,4,1); fauxINT(x,wfaux,1+wr,1) v=IAV(x);
 s=AS(w); MCISH(v,s,wf); v[wf]=1; MCISH(v+wf+1,s+wf,wcr);  // frame, 1, shape - the final shape
 R jtreshape(jtfg,x,w);
}    /* ,:"r w */

DF2(jtlamin2){F12IP;A z;I ar,p,q,wr;
 // Because we don't support inplacing here, the inputs & results will be marked non-pristine.  That's OK because scalar replication might have happened.
 ARGCHK2(a,w); 
 PROLOG(000);
 ar=AR(a); p=jt->ranks>>RANKTX; p=ar<p?ar:p;  // p=cell rank of a, q=cell rank of w
 wr=AR(w); q=(RANKT)jt->ranks; q=wr<q?wr:q; RESETRANK;
 if(p)RZ(a=IRS1(a,0L,p,jtlamin1,z));
 if(q)RZ(w=IRS1(w,0L,q,jtlamin1,z));
 RZ(IRS2(a,w,self,p+!!p,q+!!q,jtover,z));
 if(!(p|q))z=IRS1(z,0L,0L,jtlamin1,a);
 EPILOG(z);
}    /* a,:"r w */

// Append, including tests for append-in-place
F2(jtapip){F12IP;A h;
 ARGCHK2(a,w);
 // if exactly one arg has no items in cell, and the empty does not have longer frame, and the frames agree,
 // and items have the same rank, and the empty item has no axis larger than the nonempty: return the nonempty
 // here we require no frame as well
 I at=AT(a), ar=AR(a), wr=AR(w), ac=AC(a), an=AN(a), jtrm=(I)jt->ranks-(I)R2MAX;  // unchanging values
 I ai=AS(a)[0], wi=AS(w)[0]; wi=wr?wi:ai;  // item counts of args; force miscompare if both atoms
 if(unlikely((I)SGNTO0(-ai^-wi)>((ar^wr)-jtrm))){  // appending empty to nonempty, no frame, equal rank (not 0), no rank given
  if(likely(!ISSPARSE(at|AT(w)))){   // sparse blocks have weird shape - we can't handle them
   A ea=ai?w:a, nea=ai?a:w;  // empty & nonempty args
   I emptybig; TESTXITEMSMALL(emptybig,AS(ea),AS(nea),ar) if(!emptybig)R RETARG(nea);  // if item not changing, return nonempty argument unchanged
  }
 }
 // Allow inplacing if we have detected an assignment to a name on the last execution, and the address
 // being assigned is the same as a, and the usecount of a allows inplacing; or
 // the argument a is marked inplaceable.  Usecount of <1 is inplaceable, and for memory-mapped nouns, 2 is also OK since
 // one of the uses is for the mapping header.
 // In both cases we require the inplaceable bit in jt, so that a =: (, , ,) a  , which has zombieval set, will inplace only the last append
 // Allow only DIRECT and BOX types, to simplify usecounting (we don't have to EPILOG for RAT/XNUM)
 if((SGNIF((I)jtfg,JTINPLACEAX)&-ar&~(ar-wr)&~jtrm)<0){  // inplaceable, ar!=0, wr<=ar, ranks=MAX, all close at hand
  UI virtreqd=0;  // the inplacing test sets this if the result must be virtual
  I lgk=bplg(at); I lgatomsini=MAX(LGSZI-lgk,0);  // lg of size of atom of a; lg of number of atoms in an I (0 if <1)
  // Because the test for inplaceability is rather lengthy, start with a quick check of the atom counts.  If adding the atoms in w to those in a
  // would push a over a power-of-2 boundary, skip the rest of the testing.  We detect this by absence of carry out of the high bit (inside EXTENDINPLACE)
  if(EXTENDINPLACENJA(a,w) && ((at&(DIRECT|BOX))|(AT(w)&SPARSE))>0){
   // collect some values into a flags register
#define FGLGK 0x7
#define FGVIRTREQDX 3    // if virtual extension required
#define FGVIRTREQD (1LL<<FGVIRTREQDX)
#define FGWATOMICX 8     // if w is atomic - higher than highest bit of rank
#define FGWATOMIC (1LL<<FGWATOMICX)
#define FGWNOFILLX 9 // if fill is required for any reason
#define FGWNOFILL (1LL<<FGWNOFILLX)
#define FGWNOCELLFILLX 10 // if w has an interior axis that requires fill, i. e. take
#define FGWNOCELLFILL (1LL<<FGWNOCELLFILLX)
#define FGWPRISTX AFPRISTINEX  // 24 if w and inplacing do not prevent keeping a pristine
#define FGWPRIST (1LL<<FGWPRISTX)
#define FGARMINUSWRX 25    // ar minus wr, never negative.  Must be highest field
#define FGARMINUSWR (RMAX<<FGARMINUSWRX)
      I fgwd= ((ar-wr)<<FGARMINUSWRX) + FGWNOCELLFILL + FGWNOFILL + ((wr-1)&FGWATOMIC) + (virtreqd<<FGVIRTREQDX) + lgk;  // collect flags.  If item of a has higher rank than w, force fill
   // if w is boxed, we have some more checking to do.  We have to make sure we don't end up with a box of a pointing to a itself.  The only way
   // this can happen is if w is (<a) or (<<a) or the like, where w does not have a recursive usecount.  The fastest way to check this would be to
   // crawl through w looking for a.
   // Instead, we simply convert w to recursive-usecount.  This may take some time if w is complex, but it will (1) increment the
   // usecount of a if any part of w refers to a (3) make the eventual incrementing of usecount in a quicker.  After we have resolved w we see if the usecount of a has budged.  If not, we can proceed with inplacing.
   if(unlikely(at&BOX)){
    // result is pristine if a and w both are, and they are not the same block, and there is no fill, w is abandoned and not atomic
    fgwd|=(((a!=w)&((I)jtfg>>JTINPLACEWX)&SGNTO0(AC(w)&-wr))<<AFPRISTINEX)&AFLAG(w);  // set if w qualifies as pristine before extension
    an&=virtreqd-1;  // turn off inplacing if the result must be virtual
    ra0(w);  // ensure w is recursive usecount.  This will be fast if w has 1=L.
    an=(AC(a)>ac)?0:an;  // turn off inplacing if w referred to a
   }
   // Here the usecount indicates inplaceability.  We have to see if the argument ranks and shapes permit it also
   // We disqualify inplacing if a is empty (because we wouldn't know what type to make the result, and anyway there may be axes
   // in the shape that are part of the shape of an item), or if a is atomic (because
   // we would have to replicate a, and anyway how much are you saving?), or if w has higher rank than a (because the rank of the
   // result would increase, and there's no room in the shape)
   // jt->ranks is ~0 unless there are operand cells, which disqualify us.  There are some cases where it
   // would be OK to inplace an operation where the frame of a (and maybe even w) is all 1s, but that's not worth checking for
   // We use type priority to decide whether a would have to be converted
   I zt=maxtyped(at,AT(w));  // the type of the result
   if((((an-1)|-(at^zt))>=0)){  // a not empty, atype = resulttype.
    //  Check the item sizes.  Set p<0 if the items of w require fill (ecch - can't go inplace), p=0 if no padding needed, p>0 if items of w require internal fill
    // If there are extra axes in a, they are created as unit axes of w with no action needed.  Check the axes of w that are beyond the first axis
    // of a, because the first axis of a tells how many items there are - that number doesn't matter, it's the shape of an item of result that matters
    I *u=(AS(a))+ar, *v=(AS(w))+wr, mnaxes = -MIN(wr,ar-1); v=mnaxes>=0?u:v;  // point past the end of axes.  mnaxes is -#axes to compare, i. e. >=0 if next values SHOULD NOT be compared.  If none, set v=u to always compare =
    I p=u[-1]-v[-1]; ++mnaxes; u+=REPSGN(mnaxes); v+=REPSGN(mnaxes);  // compare last axis; advance if there is another
    p|=u[-1]-v[-1]; if(unlikely(mnaxes++<0))do{ --u; --v; p|=u[-1]-v[-1];}while(++mnaxes<0);  // compare axis -2, and then any others
    // Now p<0 if ANY axis of a needs extension - can't inplace then.  If p>0, transfer a flag for that to fgwd
    if(likely(p>=0)){  // if a can be left in place...
     fgwd&=p>0?~(FGWNOFILL+FGWNOCELLFILL):~0;  // if cell of a has an axis bigger than w, w will require internal fill
     // Calculate k, the size of an atom of a; ak, the number of bytes in a; wm, the number of result-items in w
     // (this will be 1 if w has to be rank-extended, otherwise the number of items in w); wk, the number of bytes in
     // items of w (after its conversion to the precision of a)
     I wn; PROD(wn,ar-1,AS(a)+1) fgwd&=((wn>AN(w))?~FGWNOFILL:~0);  // wn starts as size of cell of a; if w is smaller than that, it will have to fill, either internal or external
     // There will be a delay in settling the value of fgwd.  To reduce the latency if fill is required we could move some computation here.  But the compiler would defer it anyway
     if(likely(fgwd&FGWNOFILL)||likely(!jt->fill)||(fgwd&=~AFPRISTINE,TYPESEQ(at,AT(jt->fill)))){  // OK if we won't fill, or there is no user fill; if user fill, must not change precision of a
      I wm=AS(w)[0]; wm=fgwd&FGARMINUSWR?1:wm; wn*=wm;  // if ar=wr, wm=#w and wn=#atoms in w cells of a; if ar>wr, wm=1 and wn=#atoms in 1 cell of a
      I ak=an<<(fgwd&FGLGK),wk=wn<<(fgwd&FGLGK);  // get length of a and w in bytes
      // See if there is room in a to fit w (including trailing pad to ensure a SZI can be fetched/stored at the last valid atom's address- but no pad for NJA blocks, to allow appending to the limit)
      I extrasize; if(likely(!(AFLAG(a)&AFNJA+AFVIRTUAL)))extrasize=FHRHSIZE(AFHRH(a))-AK(a)-(((SZI-1)<<(fgwd&FGLGK))&(SZI-1)); else extrasize=AFLAG(a)&AFVIRTUAL?-1:AM(a);  // a can't be GMP; inline the computation of allosize.  VIRTUAL must be UNINCORPABLE - pity to have to wait so long
      if(likely(extrasize>=ak+wk)){    // if free space is sufficient...
       // *********************** We have passed all the tests.  Inplacing is OK.
       AFLAGRESET(a,AFLAG(a)&(fgwd|~AFPRISTINE))  // clear pristine flag in a if w is not also (a must not be virtual)
       //  Now adjust the result block to match the upcoming copy.  If the operation is virtual extension we have to allocate a new block for the result
       if(unlikely(fgwd&FGVIRTREQD)){    // If virtual extension...
        A oa=a; ar=AR(a); RZ(a=virtual(a,0,ar)); AN(a)=AN(oa)+wn; AS(a)[0]=AS(oa)[0]+wm; MCISH(&AS(a)[1],&AS(oa)[1],ar-1);  // Allocate a virtual block, which will extend past the original block.  Fill in AN and AS for the block
       }
       PROLOG(000);  // everything allocated after this is scratch area
       if(unlikely(TYPESNE(at,AT(w)))){RZ(w=cvt(at,w));}       // If w must change precision, do.  This is where we catch domain errors.  We wait till here in case a and w should be converted to the type of user fill (in jtover)
       // If the items of w must be padded to the result item-size, do so.
       // If the items of w are items of the result, we simply extend each to the shape of
       // an item of a, leaving the number of items unchanged.  Otherwise, the whole of w becomes an
       // item of the result, and it is extended to the rank/size of an item of a.  The extra
       // rank is implicit in the shape of a.  BUT never pad an atomic w.  If we add user fill, we must remove pristinity on the result
       // The take relies on the fill value
       I wlen;  // length of w data
       C *av=ak+CAV(a);   // av->end of a data
       if(likely((fgwd&FGWATOMIC+FGWNOFILL)!=0)){wlen=AN(w)<<(fgwd&FGLGK); // no fill: av->end of a data; the length in bytes of the data in w
  // if w cell must be expanded, whether by leading or internal axis, but not by scalar replication
       }else{
        // fill required, and no atomic replication.  First expand the entire w as needed, then framing fill as needed for extra axes
        if(!(fgwd&FGWNOCELLFILL)){RZ(h=vec(INT,AR(w),AS(a)+(fgwd>>FGARMINUSWRX))); makewritable(h); IAV1(h)[0]=AS(fgwd&FGARMINUSWR?a:w)[fgwd>>FGARMINUSWRX]; RZ(w=take(h,w));}
        // If an item of a is bigger than the entire (possibly expanded) nonatomic w,
        // copy fill to the output area.  Start the copy after the area that will be filled in by w
        wlen=AN(w)<<(fgwd&FGLGK); // the length in bytes of the data in w after fill to item size
        if(((wlen-wk))<0){RZ(jtsetfv1(jt,w,AT(w))); mvc(wk-wlen,av+wlen,(1LL<<(fgwd&FGLGK)),jt->fillv);}
       }
       // Fill has been installed.  Copy in the actual data, replicating if w is atomic
       if(!(fgwd&FGWATOMIC)){JMC(av,CAV(w),wlen,1)} else mvc(wk,av,(1LL<<(fgwd&FGLGK)),CAV(w));  // no overcopy because there could be fill
       // a was inplaceable & thus not virtual, but we must clear pristinity from w wherever it is
       PRISTCLRF(w)  // this destroys w!
       // The data has been copied.  No more errors are possible.  It is safe to modify the size of a
       if(likely(!(fgwd&FGVIRTREQD))){     // Normal append-in-place.
        // if a has recursive usecount (which precludes virtual extension), increment the usecount of the added data - including any fill
        if(UCISRECUR(a)){wn<<=((fgwd&FGLGK)-LGSZI); A* aav=(A*)av; DO(wn, ra(aav[i]);)}        // convert wn to be the number of indirect pointers in the added data (RAT types have 2, the rest have 1)
        AN(a)+=wn; __atomic_store_n(&AS(a)[0],AS(a)[0]+wm,__ATOMIC_RELEASE);  // Update the # items in a, and the # atoms - after the data is fully ready, in case another thread is watching
       }
       if(unlikely(_ttop!=jt->tnextpushp))tpop(_ttop);  // if virtual, ttop is after the result; otherwise result was surely not allocated here.  EPILOG not needed because no non-BOX indirects allowed, and BOX doesn't call cvt
       RETF(a);
      }else ASSERT(!(AFLAG(a)&AFNJA),EVALLOC)  // if we failed only because the new value wouldn't fit, signal EVALLOC if NJA, to avoid creating a huge unassignable value
     } // end 'no fill required with dissimilar type'
    }  // end 'items of a are big enough'
   }  // end 'a and w compatible in rank and type'
  }   // end 'inplaceable usecount'
 }  // end 'inplaceable'
 R(jtover(jtfg,a,w,ds(CCOMMA)));  // if there was trouble, failover to non-in-place code

}    /* append in place if possible */
