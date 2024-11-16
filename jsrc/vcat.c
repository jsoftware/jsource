/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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

static F2(jtovs){A ae,ax,ay,q,we,wx,wy,x,y,z,za,ze;B*ab,*wb,*zb;I acr,ar,*as,at,c,m,n,r,t,*v,wcr,wr,*ws,wt,*zs;P*ap,*wp,*zp;
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


static C*jtovgmove(J jt,I k,I c,I m,A s,A w,C*x,A z){I d,n,p=c*m;
   // z may not be boxed; but if it is, w must be also.
 if(AR(w)){
  n=AN(w); d=AN(s)-AR(w);
  if((-n&(d-1))>=0)mvc(k*p,x,k,jt->fillv);  // fill required: w empty or shape short (d>0)
  if(n){  // nonempty cell, must copy in the data
   if(n<p){I *v=AV(s); *v=m; RZ(w=take(d?vec(INT,AR(w),d+v):s,w));}  // incoming cell smaller than result area: take to result-cell size
   JMC(x,AV(w),k*AN(w),1);  // copy in the data, now the right cell shape but possibly shorter than the fill  kludge could avoid double copy
  }
 }else{  // scalar replication
  mvc(k*p,x,k,AV(w));
 }
 R x+k*p;
}    /* move an argument into the result area */

static F2(jtovg){A s,z;C*x;I ar,*as,c,k,m,n,r,*sv,t,wr,*ws,zn;
 ARGCHK2(a,w);
 // scaf most of this should move back to the caller so it can be outside the rank loop
 ar=AR(a); wr=AR(w); r=ar+wr?MAX(ar,wr):1;
 RZ(s=r?vec(INT,r,AS(r==ar?a:w)):num(2)); sv=AV(s);   // Allocate list for shape of composite item, init to longer shape; but if both are atoms, just the number 2, which we will not modify
 // Calculate the shape of the result: the shape of the item, max of input shapes
 if(m=MIN(ar,wr)){   // m=lower rank.  If either arg is a scalar, it wiil be replicated, so no need to check item size then
  as=ar+AS(a); ws=wr+AS(w); k=r;  // as, ws point past end of shape, k indexes past end of combined shape
  DQ(m, --as; --ws; sv[--k]=MAX(*as,*ws););   // set sv to max len of each axis, for the common rank
  DO(r-m, sv[i]=MAX(1,sv[i]););   // continue with the frame, where the shorter frame extends its item shape with 1s
 }
 PRODX(c,r-1,1+sv,1); m=AS(a)[0]; m=r>ar?1:m; n=AS(w)[0]; n=r>wr?1:n; // c=#atoms in result item; m, n=#items in each arg (1 if arg has lower rank)
 DPMULDE(c,m+n,zn);  // get total # atoms in result
 // Now that we have figured out the result shape we can decide whether we need fill
 I origwt=AT(w); if(unlikely(((AN(a)+AN(w)-zn)&(-MIN(ar,wr)))<0))RZ(w=setfv(a,w));  // set fill only if there are more result atoms than input atoms, and neither arg is an atom (which would replicate).  cvt w if needed
 if(unlikely(AT(a)!=(t=AT(w)))){t=maxtypedne(AT(a)|(AN(a)==0),t|(((t^origwt)+AN(w))==0)); t=LOWESTBIT(t); I atdiff=TYPESXOR(t,AT(a)); RZ(z=cvt(t,atdiff?a:w)) a=atdiff?z:a; w=atdiff?w:z;}  // convert args to compatible precisions, changing a and w if needed.  B01 if both empty.  If fill changed w, don't do B01 for it
 GA(z,t,zn,r,sv); AS(z)[0]=m+n; x=CAVn(r,z); k=bpnoun(t);  // allocate result with composite item shape; install #items; get len of an atom
 RZ(x=ovgmove(k,c,m,s,a,x,z));
 RZ(x=ovgmove(k,c,n,s,w,x,z));
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
   // copy one cell from a; advance z; advance a if not repeated
   JMCR(zv,av,ma,0,endmaska); zv+=ma; --arptct; av+=REPSGN(arptct)&ma; arptct+=REPSGN(arptct)&arptreset;
   // repeat for w
   JMCR(zv,wv,mw,0,endmaskw); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&mw; wrptct+=REPSGN(wrptct)&wrptreset;
  }
 }
}
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
int (*p[4]) (int x, int y);
static void(*moveawtbl[])() = {moveawVV,moveawVS,moveawSV,moveawVVI};
DF2(jtover){AD * RESTRICT z;I replct,framect,acr,af,ar,*as,ma,mw,p,q,r,t,wcr,wf,wr,*ws,zn;
 F2PREFIP;ARGCHK2(a,w);
 UI jtr=jt->ranks;//  fetch early
 if(unlikely(ISSPARSE(AT(a)|AT(w)))){R ovs(a,w);}  // if either arg is sparse, switch to sparse code
 // convert args to compatible precisions, changing a and w if needed.  Treat empty arg as boolean if the other is non-Boolean
 if(unlikely(AT(a)!=(t=AT(w)))){t=maxtypedne(AT(a)|((UI)-AN(a)<(UI)AN(w)),t|((UI)-AN(w)<(UI)AN(a))); t=LOWESTBIT(t); I atdiff=TYPESXOR(t,AT(a)); RZ(z=cvt(t,atdiff?a:w)) a=atdiff?z:a; w=atdiff?w:z;}
 ar=AR(a); wr=AR(w);
 acr=jtr>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;  // acr=rank of cell, af=len of frame, as->shape
 wcr=(RANKT)jtr; wcr=wr<wcr?wr:wcr; wf=wr-wcr;  // wcr=rank of cell, wf=len of frame, ws->shape
 // no RESETRANK - not required by ovv or main line here
 as=AS(a); ws=AS(w);
 if(af+wf==0){
#if 0  // we don't use ALLOWRETARG anywhere yet
 // if exactly one arg has no items in cell, and the empty does not have longer frame, and the frames agree,
 // and items have the same rank, and the empty item has no axis larger than the nonempty: return the nonempty
 // because some of the sparse code writes over the result from here, we enable this only if the call enables it
  I ai=AS(a)[0], wi=AS(w)[0]; wi=wr?wi:ai;  // item counts of args; force miscompare if both atoms
  if(unlikely((I)SGNTO0(-ai^-wi)>((ar^wr)|((I)jtinplace&JTALLOWRETARG)))){  // appending empty to nonempty, no frame, equal rank (not 0)
   A ea=ai?w:a, nea=ai?a:w;  // empty & nonempty args
   I emptybig; TESTXITEMSMALL(emptybig,AS(ea),AS(nea),ar) if(!emptybig)R RETARG(nea);  // if item not changing, return nonempty argument unchanged
  }
#endif
  // No frame.  See if ranks are equal or different by 1, and if the items have the same shape
  I lr=ar;  // rank of arg with long shape
  A l=a; l=wr>ar?w:l; lr=wr>ar?wr:lr;  // arg with long shape.  Not needed till later but we usually go through the fast path
  if(likely(2*lr-1<=ar+wr)){  // if ranks differ by at most 1
   // items have the same rank or one argument is an item of the other (cases where the ranks differ by more than 1 follow the general path below)
   // see if the shapes agree up to the shape of an item of the longer argument
   I mismatch=0; I cr=lr-((UI)lr>0);
   TESTDISAGREE(mismatch,as+ar-cr,ws+wr-cr,cr)  // compare the tail of the shapes, for the length of an item of the longer shape
   if(likely(!mismatch)){
    // The data can be copied in toto, with only the number of items changing.
    A s=(A)((I)a+(I)w-(I)l);  // arg with short shape
    // The rank is the rank of the long argument, unless both arguments are atoms; then it's 1
    // The itemcount is the sum of the itemcounts; but if the ranks are different, use 1 for the shorter; and if both ranks are 0, the item count is 2
    // empty items are OK: they just have 0 length but their shape follows the normal rules
    I si=AS(s)[0]; si=ar==wr?si:1; si+=AS(l)[0]; si=lr==0?2:si; lr=lr==0?1:lr; ASSERT(si>=0,EVLIMIT);  // get short item count; adjust to 1 if lower rank; add long item count; check for overflow; adjust if atom+atom
    I klg=bplg(t); I alen=AN(a)<<klg; I wlen=AN(w)<<klg;
    GA(z,t,AN(a)+AN(w),lr,AS(l)); AS(z)[0]=si; C *x=CAVn(lr,z);  // install # items after copying shape
    JMC(x,CAV(a),alen,0); JMC(x+alen,CAV(w),wlen,0);
    // If a & w are both recursive abandoned non-virtual, we can take ownership of the contents by marking them nonrecursive and marking z recursive.
    // We could also zap a & w, but we don't because it's just a box header and it will be freed by a caller anyway
    // We can't transfer ownership if one of the args is VIRTUAL, because a virtual block doesn't really own its contents
    // We can't transfer ownership if a=w, because the counts for the blocks would be one too low

    // The result can be pristine if both inputs are abandoned pristine, and are not the same block
    // If a and w are the same, we mustn't mark the result pristine!  It has repetitions
    I xfer, aflg=AFLAG(a), wflg=AFLAG(w);
    xfer=aflg&wflg&REPSGN((JTINPLACEA-((JTINPLACEA+JTINPLACEW)&(I)jtinplace))&AC(a)&AC(w))&-(a!=w);  // flags, if abandoned inplaceable and not the same block
    if(unlikely((xfer&=AFPRISTINE|((aflg|wflg)&AFVIRTUAL?0:RECURSIBLE))!=0)){  // preserve the PRISTINE flag and RECURSIBLE too, if not VIRTUAL
     // a and w are both non-shared blocks, and not VIRTUAL or PERMANENT.  xfer is the transferable recursibility if any, plus inherited pristinity
     AFLAGORLOCAL(z,xfer); xfer|=AFPRISTINE; AFLAGANDLOCAL(a,~xfer) AFLAGANDLOCAL(w,~xfer)  // transfer inplaceability/pristinity; always clear pristinity from a/w
    }else{AFLAGCLRPRIST(w); AFLAGCLRPRIST(a);}  // if we can't transfer pristinity, we must clear it
    // We extracted from a and w, so mark them (or the backer if virtual) non-pristine.  If both were pristine and abandoned, transfer its pristine status to the result
    // if they were boxed nonempty, a and w have not been changed.  Otherwise the PRISTINE flag doesn't matter.
    if(unlikely((aflg&AFVIRTUAL)!=0)){AFLAGPRISTNO(ABACK(a))}  //  like PRISTCOMSETF
    if(unlikely((wflg&AFVIRTUAL)!=0)){AFLAGPRISTNO(ABACK(w))}  //  like PRISTCOMSETF
    RETF(z);
   }
  }
 }
 // dissimilar items, or there is frame.  Mark the inputs non-pristine; leave the result non-pristine, since we don't know whether it has repetitions (we could figure that out)
 PRISTCLR(a) PRISTCLRNODCL(w)  // make inputs non-PRISTINE
 p=as[ar-1];   // p=len of last axis of cell.  Always safe to fetch first 
 q=ws[wr-1];   //  q=len of last axis of cell
 r=MAX(acr,wcr); r=(r==0)?1:r;  // r=cell-rank, or 1 if both atoms.
 // if max cell-rank>2, or an argument is empty, or (joining table/table or table/row with cells of different lengths), do general case
 if(((r-3)&-AN(a)&-AN(w)&((acr+wcr-3)|((p^q)-1)))>=0){  // r>2, or empty (if max rank <= 2 and sum of ranks >2, neither can possibly be an atom), and items (which are lists) have same length 
  RESETRANK; z=rank2ex(a,w,self,acr,wcr,acr,wcr,jtovg); R z;  // ovg calls other functions, so we clear rank
 }
 // joining rows, or table/row with same lengths, or table/atom.  In any case no fill is possible, but scalar replication might be
 I cc2a=as[ar-2]; p=acr?p:1; cc2a=acr<=1?1:cc2a; ma=cc2a*p; ma=wcr>acr+1?q:ma;  //   cc2a is # 2-cells of a; ma is #atoms in a cell of a EXCEPT when joining atom a to table w: then length of row of w
 I cc2w=ws[wr-2]; q=wcr?q:1; cc2w=wcr<=1?1:cc2w; mw=cc2w*q; mw=acr>wcr+1?p:mw;  // sim for w;
 I f=(wf>=af)?wf:af; I shortf=(wf>=af)?af:wf; I *s=(wf>=af)?ws:as;
 PROD(replct,f-shortf,s+shortf); PROD(framect,shortf,s);  // Number of cells in a and w; known non-empty shapes
 DPMULDE(replct*framect,ma+mw,zn);  // total # atoms in result
 GA(z,t,zn,f+r,s); if(unlikely(zn==0))RETF(z); s=AS(z)+f+r;   // allocate result; repurpose s to point to END+1 of shape field.  Return if area empty so we can use UNTIL loops
 if(2>r)s[-1]=ma+mw; else{s[-1]=acr?p:q; s[-2]=cc2a+cc2w;}  // fill in last 2 atoms of shape
 I klg=bplg(t);   // # bytes per atom of result
 // copy in the data, creating the result in order (to avoid page thrashing and to make best use of write buffers)
 // scalar replication is required for any arg whose rank is 0 and yet its length is >1.  Choose the copy routine based on that
 I sreps=SGNTO0((acr-1)&(1-ma))*2+(SGNTO0(((wcr-1)&(1-mw))));  // look for scalar reps
 sreps=(((((ma<<klg)^SZI)+((mw<<klg)^SZI))==0)>sreps)?3:sreps;  // if VV case moving exactly SZI, use routine for that
 moveawtbl[sreps](CAV(z),CAV(a),CAV(w),replct*framect,(I)1<<klg,ma<<klg,mw<<klg,(wf>=af)?replct:1,(wf>=af)?1:replct);
 RETF(z);
}    /* overall control, and a,w and a,"r w for cell rank <: 2 */

DF2(jtstitch){I ar,wr; A z;
 F2PREFIP;ARGCHK2(a,w);
 ar=AR(a); wr=AR(w);
 ASSERT((-ar&-wr&-(AS(a)[0]^AS(w)[0]))>=0,EVLENGTH);  // a or w scalar, or same # items    always OK to fetch s[0]
 if(likely(((SGNIFDENSE(AT(a)|AT(w)))&(2-ar)&(2-wr))>=0))R IRSIP2(a,w,self,(ar-1)&RMAX,(wr-1)&RMAX,jtover,z);  // not sparse or rank>2
 R stitchsp2(a,w);  // sparse rank <=2 separately
}

F1(jtlamin1){A x;I* RESTRICT s,* RESTRICT v,wcr,wf,wr; 
 F1PREFIP;ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; RESETRANK; wf=wr-wcr;
 fauxblockINT(wfaux,4,1); fauxINT(x,wfaux,1+wr,1) v=AV(x);
 s=AS(w); MCISH(v,s,wf); v[wf]=1; MCISH(v+wf+1,s+wf,wcr);  // frame, 1, shape - the final shape
 R jtreshape(jtinplace,x,w);
}    /* ,:"r w */

DF2(jtlamin2){A z;I ar,p,q,wr;
 // Because we don't support inplacing here, the inputs & results will be marked non-pristine.  That's OK because scalar replication might have happened.
 ARGCHK2(a,w); 
 ar=AR(a); p=jt->ranks>>RANKTX; p=ar<p?ar:p;  // p=cell rank of a, q=cell rank of w
 wr=AR(w); q=(RANKT)jt->ranks; q=wr<q?wr:q; RESETRANK;
 if(p)RZ(a=IRS1(a,0L,p,jtlamin1,z));
 if(q)RZ(w=IRS1(w,0L,q,jtlamin1,z));
 RZ(IRS2(a,w,self,p+!!p,q+!!q,jtover,z));
 if(!(p|q))z=IRS1(z,0L,0L,jtlamin1,a);
 RETF(z);
}    /* a,:"r w */

// Append, including tests for append-in-place
A jtapip(J jt, A a, A w){F2PREFIP;A h;
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
 // Allow only DIRECT and BOX types, to simplify usecounting
 if((SGNIF((I)jtinplace,JTINPLACEAX)&-ar&~(ar-wr)&~jtrm)<0){  // inplaceable, ar!=0, wr<=ar, ranks=MAX, all close at hand
  UI virtreqd=0;  // the inplacing test sets this if the result must be virtual
  I lgk=bplg(at); I lgatomsini=MAX(LGSZI-lgk,0);  // lg of size of atom of a; lg of number of atoms in an I (0 if <1)
  // Because the test for inplaceability is rather lengthy, start with a quick check of the atom counts.  If adding the atoms in w to those in a
  // would push a over a power-of-2 boundary, skip the rest of the testing.  We detect this by absence of carry out of the high bit (inside EXTENDINPLACE)
  if(EXTENDINPLACENJA(a,w) && ((at&(DIRECT|BOX))|(AT(w)&SPARSE))>0) {
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
    fgwd|=(((a!=w)&((I)jtinplace>>JTINPLACEWX)&SGNTO0(AC(w)&-wr))<<AFPRISTINEX)&AFLAG(w);  // set if w qualifies as pristine before extension
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
// obsolete     I naxes = MIN(wr,ar); u=ar+(AS(a))-naxes; v=wr+(AS(w))-naxes;  // point to the axes to compare
    I *u=(AS(a))+ar, *v=(AS(w))+wr, mnaxes = -MIN(wr,ar-1); v=mnaxes>=0?u:v;  // point past the end of axes.  mnaxes is -#axes to compare, i. e. >=0 if next values SHOULD NOT be compared.  If none, set v=u to always compare =
    I p=u[-1]-v[-1]; ++mnaxes; u+=REPSGN(mnaxes); v+=REPSGN(mnaxes);  // compare last axis; advance if there is another
    p|=u[-1]-v[-1]; if(unlikely(mnaxes++<0))do{ --u; --v; p|=u[-1]-v[-1];}while(++mnaxes<0);  // compare axis -2, and then any others
// obsolete    // For each axis to compare, see if a is bigger/equal/smaller than w; OR into p
// obsolete    p=0; DQ(naxes, p |= *u++-*v++;);
    // Now p<0 if ANY axis of a needs extension - can't inplace then.  If p>0, transfer a flag for that to fgwd
// obsolete    if(likely(p==0)||p>0&&(fgwd&=~(FGWNOFILL+FGWNOCELLFILL),!jt->fill||(fgwd&=~AFPRISTINE,TYPESEQ(at,AT(jt->fill))))){  // if there is fill, for comp ease we demand it have the type of a; if user fill, turn off pristinity of result
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
// obsolete     if(allosize(a)>=ak+wk+(REPSGN((-(at&LAST0))&((AFLAG(a)&AFNJA)-1))&(SZI-1))){    // SZI-1 if LAST0 && !NJA
// obsolete       if(likely(allosize>=(ak+wk+MAX(SZI-(1LL<<(fgwd&FGLGK)),0)))){    // ensure a SZI can be fetched/stored at the last valid atom's address, adding 7, 6, 4, 0
      I extrasize; if(likely(!(AFLAG(a)&AFNJA+AFVIRTUAL)))extrasize=FHRHSIZE(AFHRH(a))-AK(a)-(((SZI-1)<<(fgwd&FGLGK))&(SZI-1)); else extrasize=AFLAG(a)&AFVIRTUAL?-1:AM(a);  // a can't be GMP; inline the computation of allosize.  VIRTUAL must be UNINCORPABLE - pity to have to wait so long
      if(likely(extrasize>=ak+wk)){    // if free space is sufficient...
       // We have passed all the tests.  Inplacing is OK.
       AFLAGRESET(a,AFLAG(a)&(fgwd|~AFPRISTINE))  // clear pristine flag in a if w is not also (a must not be virtual)
       // If w must change precision, do.  This is where we catch domain errors.  We wait till here in case a and w should be converted to the type of user fill (in jtover)
       if(unlikely(TYPESNE(at,AT(w)))){RZ(w=cvt(at,w));}
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
        if(!(fgwd&FGWNOCELLFILL)){h=vec(INT,AR(w),AS(a)+(fgwd>>FGARMINUSWRX)); makewritable(h); IAV1(h)[0]=AS(fgwd&FGARMINUSWR?a:w)[fgwd>>FGARMINUSWRX]; RZ(w=take(h,w));}
// obsolete  wv=CAV(w);, wv->w data
        // If an item of a is bigger than the entire (possibly expanded) nonatomic w,
        // copy fill to the output area.  Start the copy after the area that will be filled in by w
        wlen=AN(w)<<(fgwd&FGLGK); // the length in bytes of the data in w after fill to item size
        if(((wlen-wk))<0){RZ(jtsetfv1(jt,w,AT(w))); mvc(wk-wlen,av+wlen,(1LL<<(fgwd&FGLGK)),jt->fillv);}
       }
// obsolete        jtinplace=jt->fill?0:jtinplace;  // we are filling.  If the fill for box is not the default a:, remove pristinity from w and thus from a
       // Fill has been installed.  Copy in the actual data, replicating if w is atomic
       if(!(fgwd&FGWATOMIC)){JMC(av,CAV(w),wlen,1)} else mvc(wk,av,(1LL<<(fgwd&FGLGK)),CAV(w));  // no overcopy because there could be fill
       // a was inplaceable & thus not virtual, but we must clear pristinity from w wherever it is
       PRISTCLRF(w)  // this destroys w!
       // The data has been copied.  Now adjust the result block to match.  If the operation is virtual extension we have to allocate a new block for the result
       if(likely(!(fgwd&FGVIRTREQD))){
        // Normal append-in-place.
        // Update the # items in a, and the # atoms
        AS(a)[0]+=wm; AN(a)+=wn;
        // if a has recursive usecount, increment the usecount of the added data - including any fill
        // convert wn to be the number of indirect pointers in the added data (RAT types have 2, the rest have 1)
        if(UCISRECUR(a)){wn<<=((fgwd&FGLGK)-LGSZI); A* aav=(A*)av; DO(wn, ra(aav[i]);)}
       }else{
        // virtual extension.  Allocate a virtual block, which will extend past the original block.  Fill in AN and AS for the block
        A oa=a; ar=AR(a); RZ(a=virtual(a,0,ar)); AN(a)=AN(oa)+wn; AS(a)[0]=AS(oa)[0]+wm; MCISH(&AS(a)[1],&AS(oa)[1],ar-1);
       }
       RETF(a);
      }else ASSERT(!(AFLAG(a)&AFNJA),EVALLOC)  // if we failed only because the new value wouldn't fit, signal EVALLOC if NJA, to avoid creating a huge unassignable value
     } // end 'no fill required with dissimilar type'
    }  // end 'items of a are big enough'
   }  // end 'a and w compatible in rank and type'
  }   // end 'inplaceable usecount'
 }  // end 'inplaceable'
 R(jtover(jtinplace,a,w,ds(CCOMMA)));  // if there was trouble, failover to non-in-place code
}    /* append in place if possible */
