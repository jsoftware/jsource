/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Cuts                                                      */

#include "j.h"
#include "vcomp.h"
#define ZZDEFN
#include "result.h"
#define STATEHASGERUNDX 10
#define STATEHASGERUND (1LL<<STATEHASGERUNDX)
#define STATENEEDSASSEMBLYX 11
#define STATENEEDSASSEMBLY (1LL<<STATENEEDSASSEMBLYX)



static DF1(jtcut01){DECLF;A h,x;
 RZ(x=from(box(every(negate(shape(w)),0L,jtiota)),w));
 if(VGERL&sv->flag){h=sv->h; R df1(x,*AAV(h));}else R CALL1(f1,x,fs);
}    /* f;.0 w */

#if 0  // special cases no longer used
static F2(jtcut02v){A z;I*av,e,j,k,m,t,wk;
 m=AN(w); t=AT(w); wk=bp(t);
 av=AV(a); j=av[0]; e=av[1]; k=ABS(e);
 ASSERT(!e||-m<=j&&j<m,EVINDEX);
 if(0>j){j+=1+m-k; if(0>j){k+=j; j=0;}}else k=MIN(k,m-j);
 GA(z,t,k,1,0);
 MC(AV(z),CAV(w)+wk*j,wk*k); 
 R 0>e?reverse(z):z;
}    /* a ];.0 vector */

static F2(jtcut02m){A z;C*u,*v;I*av,c,d,e0,e1,j0,j1,k0,k1,m0,m1,*s,t,wk;
 s=AS(w); m0=s[0]; m1=s[1]; t=AT(w); wk=bp(t);
 av=AV(a);    
 if(4==AN(a)){j0=av[0]; e0=av[2]; k0=ABS(e0); j1=av[1]; e1=av[3]; k1=ABS(e1);}
 else        {j0=av[0]; e0=av[1]; k0=ABS(e0); j1=0;     e1=m1;    k1=e1;     }
 ASSERT(!e0||-m0<=j0&&j0<m0,EVINDEX);
 ASSERT(!e1||-m1<=j1&&j1<m1,EVINDEX);
 if(0>j0){j0+=1+m0-k0; if(0>j0){k0+=j0; j0=0;}}else k0=MIN(k0,m0-j0);
 if(0>j1){j1+=1+m1-k1; if(0>j1){k1+=j1; j1=0;}}else k1=MIN(k1,m1-j1);
 GA(z,t,k0*k1,2,0); s=AS(z); s[0]=k0; s[1]=k1;
 u=CAV(z); c=wk*k1;
 if(0>e0){d=-wk*m1; v=CAV(w)+wk*(j0*m1+j1+m1*(k0-1));}
 else    {d= wk*m1; v=CAV(w)+wk*(j0*m1+j1          );}
 DO(k0, MC(u,v,c); u+=c; v+=d;);
 RETF(0>e1?irs1(z,0L,1L,jtreverse):z);
}    /* a ];.0 matrix */
#endif

static DF2(jtcut02){DECLF;A *hv,q,qq,*qv,z,zz=0;C id;I*as,c,e,hn,i,ii,j,k,m,n,*u,*ws;PROLOG(876);
 RZ(a&&w);
#define ZZFLAGWORD state
 I state=0;  // init flags, including zz flags


 if(!(VGERL&sv->flag)){hv=0; id=ID(fs);  // if verb, point to its data and fetch its pseudocharacter
  // not gerund, OK to test fs
  if(FAV(fs)->mr>=AR(w)){
   // we are going to execute f without any lower rank loop.  Thus we can use the BOXATOP etc flags here.  These flags are used only if we go through the full assemble path
   state = (FAV(fs)->flag2&VF2BOXATOP1)>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // Don't touch fs yet, since we might not loop
   state &= ~((FAV(fs)->flag2&VF2ATOPOPEN1)>>(VF2ATOPOPEN1X-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
   state |= (-state) & FAV(self)->flag2 & (VF2WILLBEOPENED|VF2COUNTITEMS); // remember if this verb is followed by > or ; - only if we BOXATOP, to avoid invalid flag setting at assembly
  }
 }else{
  state |= STATEHASGERUND; A h=sv->h; hv=AAV(h); hn=AN(h); ASSERT(hn,EVLENGTH);  // Gerund case.  Mark it, set hv->1st gerund, hn=#gerunds.  Verify gerunds not empty
  id=0;  // set an invalid pseudochar id for the gerund, to indicate 'not a primitive'
 }
 I wr=AR(w);  // rank of w
// obsolete  if(VGERL&sv->flag){h=sv->h; hv=AAV(h); hn=AN(h);}  // h points to gerunds, or 0 if not gerund u
// obsolete  id=h?0:ID(fs); d=h?0:id==CBOX?1:2;   // d= 0: gerund u  1: <  2: other u   id is pseudochar of u, or 0 if gerund
 if(1>=AR(a))RZ(a=lamin2(zero,a));   // default list to be lengths starting at origin
 RZ(a=vib(a));  // audit for valid integers
 as=AS(a); m=AR(a)-2; PROD(n,m,as); c=as[1+m]; u=AV(a);  // n = # 2-cells in a, c = #axes of subarray given, m is index of next-last axis of a, u->1st atom of a
 ASSERT((-(as[m]^2)|(wr-c))>=0,EVLENGTH);    // shapes must end with 2,c where c does not exceed rank of r
 if(!n){  /* empty result; figure out result type */
  if(!(state&STATEHASGERUND))z=CALL1(f1,w,fs);else z=df1(w,hv[0]);
  if(z==0)z=zero;  // use zero as fill result if error
  GA(zz,AT(z),n,m+AR(z),0); I *zzs=AS(zz); I *zs=AS(z); 
// obsolete  DO(m, *zzs++=as[i];) DO(AR(z), *zzs++=zs[i];)  // move in frame of a followed by shape of result-cell
  MCISd(zzs,as,m) MCISd(zzs,zs,AR(z)) // move in frame of a followed by shape of result-cell
  RETF(zz);
 }
// obsolete // Handle special cases of ] or [ with direct data, if vector or table
// obsolete  if(2==AR(a)&&(id==CLEFT||id==CRIGHT)&&AT(w)&DIRECT)
// obsolete   if     (2==AN(a)&&1==AR(w))R cut02v(a,w);   //  todo kludge why not allow any rank > 0?
// obsolete   else if((4==AN(a)||2==AN(a))&&2==AR(w))R cut02m(a,w);  // why not rank > 0?
 // otherwise general case, one axis at a time
 ws=AS(w);
 // obsolete GATV(zz,BOX,n,m,as); zv=AAV(zz);
#define ZZDECL
#include "result.h"
 // If there is only one column, we will be using  a virtual block to access the subarray, so allocate it here
 //  (if the subarray is reversed, we won't need the block, but that's rare)
 I wcellsize;  // size of a cell in atoms of w.
 I origoffset;  // offset from virtual block to start of w
 I wcellbytes;  // size of a cell in bytes
 A virtw;  // virtual block to use if any
 fauxblock(virtwfaux);
 if(c==1){
  PROD(wcellsize,wr-1,ws+1);  // size in atoms of w cell
  // allocate virtual block
// obsolete   RZ(virtw = virtual(w,0,wr));    // allocate block
  fauxvirtual(virtw,virtwfaux,w,wr,ACUC1);  // allocate UNINCORPORABLE block
  // fill in shape
  MCIS(AS(virtw)+1,ws+1,wr-1);
// obsolete   I* RESTRICT vs=AS(virtw); DO(wr-1, vs[i+1]=ws[i+1];);  // shape of virtual matches shape of w except for #items
  // remember original offset.  Others will be based on this
  origoffset=AK(virtw);
  wcellbytes=wcellsize*bp(AT(w));  // bytes per cell
// obsolete   AFLAG(virtw)|=AFUNINCORPABLE;  // indicate that this is a moving virtual block and cannot EVER be incorporated
 }
 if(m){
   // There is a frame; we will have to assemble the results, even if there is only one
   // See if this verb is BOXATOP.  NOTE that if this is a gerund, fs is invalid and we mustn't check it.
   // We honor BOXATOP if the verb can operate on a cell of w in its entirety
   state |= STATENEEDSASSEMBLY;  // force us to go through the assembly code
   if(!(state&STATEHASGERUND))ZZFLAGWORD |= ((FAV(fs)->mr>=wr?VF2BOXATOP1:0)&(FAV(fs)->flag2&VF2BOXATOP1))>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // If this is BOXATOP, set so for loop.
// obsolete    ZZPARMS(0,0,as,m,n,1);  // set frame & # cells
   ZZPARMS(m,n,1)
#define ZZINSTALLFRAME(optr) MCISd(optr,as,m)
 }
 I gerundx=0; q=0;  // initialize to first gerund; we haven't allocated the input to {
 for(ii=n;ii;--ii){  // for each 2-cell of a.  u points to the cell
  if((-(c^1)|(e=u[1]))>=0){I axislen;  // e=length of 1st axis
   // non-reversed selection along a single axis: calculate length {. start }. w
   axislen=ws[0]; j=u[0];  // axislen=length of axis, j=starting position
   ASSERT(((-e)&((j+axislen)|(axislen-j-1)))>=0,EVINDEX);  // validate j in range if length not zero
   if(j>=0){e=MIN(e,axislen-j);}else{j+=axislen; e=MIN(e,j+1); j-=(e-1);}  // adjust j for negative j; clip endpoint to length of axis; move j to end of interval if reversed
   // Allocate a virtual block & fill it in
   // locate virtual block
   AK(virtw)=origoffset+j*wcellbytes;
   // fill in shape.  Rest of shape filled in at allocation
   AS(virtw)[0]=e; // shape of virtual matches shape of w except for #items
   AN(virtw)=e*wcellsize;  // install # atoms
   z = virtw;  // use the virtual block for the computation
  }else{I axislen;
   // general selection: multiple axes, or reversal.  We do not look for the case of one reversed axis (could avoid boxing input ot {) on grounds of rarity
   do{
    if(q){  // if we have already allocated the input area to {, fill it in
     // For each axis, create a boxed vector of boxed indexes to fetch, in the correct order
     for(i=0;i<c;++i){  // for each axis of the cell of a
      axislen=ws[i]; j=u[i]; e=u[i+c]; k=e; e>>=(BW-1); k^=e; k-=e;  // axislen=length of this axis, j=starting pos, e=sgn(length), k=ABS(length)
// obsolete     ASSERT(!e||-m<=j&&j<m,EVINDEX);  // validate j in range if length not zero
      ASSERT(((-k)&((j+axislen)|(axislen-j-1)))>=0,EVINDEX);  // validate j in range if length not zero
// obsolete    if(0>j){j+=1+m-k; if(0>j){k+=j; j=0;}}else k=MIN(k,m-j);  // adjust j/k for negative j; clip endpoint to length of axis
// obsolete   RZ(qv[i]=0>e?apv(k,j+k-1,-1L):0==j&&k==m?ace:apv(k,j,1L));  // create ascending or descending vector
      if(j>=0){k=MIN(k,axislen-j); j+=e&(k-1);}else{j+=axislen; k=MIN(k,j+1); j-=(~e)&(k-1);}  // adjust j for negative j; clip endpoint to length of axis; move j to end of interval if reversed
      RZ(qv[i]=apv(k,j,2*e+1));  // create ascending or descending vector.  The increment is 1 or -1
     }
     break;  // this is the loop exit
    }else{  // we have not allocated the input to {; do so now
     GATV(q,BOX,c,1,0); qv=AAV(q);   // allocate a vector of boxes, which will contain the selectors
     GAT(qq,BOX,1,0,0); *AAV(qq)=q;  // enclose that vector of boxes in a box to pass into {
    }
   }while(1);
   RZ(z=from(qq,w));
  }
  if(!(state&STATEHASGERUND)){RZ(z=CALL1(f1,z,fs));}else{RZ(z=df1(z,hv[gerundx])); ++gerundx; gerundx=(gerundx==hn)?0:gerundx;}
  if(!(state&STATENEEDSASSEMBLY)){if(AFLAG(z)&AFUNINCORPABLE){z=clonevirtual(z);} EPILOG(z);}  // if we have just 1 input and no frame, return the one result directly (for speed).  If it is UNINCORPABLE, it must not be allowed to escape - realize it
      // we use clonevirtual so that ];.0 can return a virtual block
#define ZZBODY
#include "result.h"

  u += 2*c;  // step to the next cell of a
// obsolete   RZ(*zv++=z);
// obsolete   RZ(*zv++=rifvs(df1(z,hv[ii%hn])));
// obsolete   switch(d){
// obsolete    case 0: RZ(*zv++=rifvs(df1(y,hv[ii%hn]))); break;
// obsolete    case 1: RZ(*zv++=rifvs(y));                break;
// obsolete    case 2: RZ(*zv++=rifvs(CALL1(f1,y,fs)));   break;
// obsolete    }
 }
#define ZZEXIT
#include "result.h"
 EPILOG(zz);
}    /* a f;.0 w */

// self is a compound, using @/@:/&/&:, that we tried to run with special code, but we found that we don't support the arguments
// here we revert to the non-special code for the compound
DF2(jtspecialatoprestart){
  RZ(a&&w&&self);  // return fast if there has been an error
  V *sv=FAV(self);  // point to verb info for the current overall compound
  R a==mark?(sv->id==CFORK?jtcork1:on1)(jt,w,self) : (sv->id==CFORK?jtcork2:jtupon2)(jt,a,w,self);  // figure out the default routine that should process the compound, and transfer to it
}


// a ;@:(<;.0) w where a has one column.  We allocate a single area and copy in the items
// if we encounter a reversal, we abort
// if it's a case we can't handle, we fail over to the normal code, with BOXATOP etc flags set
DF2(jtrazecut0){A z;C*wv,*zv;I ar,*as,(*av)[2],j,k,m,n,wt;
 RZ(a&&w);
 wt=AT(w); wv=CAV(w);
 ar=AR(a); as=AS(a);
// obsolete  if(!((2==ar||3==ar)&&wt&IS1BYTE&&1==AR(w)))R raze(df2(a,w,cut(ds(CBOX),zero)));
 // we need rank of a>2 (otherwise why bother?), rank of w>0, w not sparse, a not empty, w not empty (to make item-size easier)
 if((((ar-3)|(-(wt&SPARSE))|(AR(w)-1)|(AN(a)-1)))<0)R jtspecialatoprestart(jt,a,w,self);
 // the 1-cells of a must be 2x1
 if((as[ar-2]^2)|(as[ar-1]^1))R jtspecialatoprestart(jt,a,w,self);
// obsolete ASSERT(2==as[ar-2]&&1==as[ar-1],EVLENGTH);
 n=AS(w)[0]; m=AN(a)>>1;  // number of items of w, number of w-items in result
 // pass through a, counting result items.  abort if there is a reversal - too rare to bother with in the fast path
 RZ(a=vib(a)); av=(I(*)[2])AV(a); I nitems=0;
 // verify starting value in range (unless length=0); truncate length if overrun
 DO(m, j=av[i][0]; k=av[i][1]; if(k<0)R jtspecialatoprestart(jt,a,w,self); I jj=j+n; jj=(j>=0)?j:jj; ASSERT(((jj^(jj-n))|(k-1))<0,EVINDEX); j=n-jj; k=k>j?j:k; nitems+=k; ASSERT(nitems>=0,EVLIMIT))
 // audits passed and we have counted the items.  Allocate the result and copy
 I wcn; PROD(wcn,AR(w)-1,AS(w)+1);  // number of atoms per cell of w
 I zn; RE(zn=mult(wcn,nitems));  // number of atoms in result
 GA(z,wt,zn,AR(w),AS(w)); AS(z)[0]=nitems; zv=CAV(z);  // allocate a list of items of w, fill in length.  zv is running output pointer
 // copy em in.  We use MC because the strings are probably long and unpredictable - think HTML parsing
 I wcb=wcn*bp(wt);  // number of bytes in a cell of w
 DO(m, j=av[i][0]; k=av[i][1]; I jj=j+n; jj=(j>=0)?j:jj; j=n-jj; k=k>j?j:k; k*=wcb; MC(zv,wv+jj*wcb,k); zv+=k;)
#if 0 // obsolete
 RZ(z=exta(wt,1L,1L,n>>1)); zn=AN(z); zv=CAV(z); zu=zn+zv;
 for(i=0;i<m;++i){
  j=*av++; k=*av++; 
  ASSERT(-n<=j&&j<n,EVINDEX);
  q=0<=k?k:k==IMIN?IMAX:-k; d=0<=j?MIN(q,n-j):MIN(q,n+1+j);
  while(zu<d+zv){c=zv-CAV(z); RZ(z=ext(0,z)); zn=AN(z); v=CAV(z); zv=c+v; zu=zn+v;}
  switch((0<=j?2:0)+(0<=k)){
   case 0: v=wv+j+n+1;   DO(d, *zv++=*--v;); break;
   case 1: v=wv+j+n+1-d; DO(d, *zv++=*v++;); break;
   case 2: v=wv+j+d;     DO(d, *zv++=*--v;); break;
   case 3: v=wv+j;       DO(d, *zv++=*v++;);
 }}
 AN(z)=*AS(z)=zv-CAV(z);
#endif
 RETF(z);
}    /* a ;@:(<;.0) vector */


static DF2(jtcut2bx){A*av,b,t,x,*xv,y,*yv;B*bv;I an,bn,i,j,m,p,q,*u,*v,*ws;V*sv;
 RZ(a&&w&&self);
 sv=FAV(self); q=*AV(sv->g);
 an=AN(a); av=AAV(a); RELBASEASGN(a,a); ws=AS(w);
 ASSERT(an<=AR(w),EVLENGTH);
 GATV(x,BOX,an,1,0); xv=AAV(x);  // could be faux
 GATV(y,BOX,an,1,0); yv=AAV(y);
 for(i=0;i<an;++i){
  b=AVR(i); bn=AN(b); m=ws[i];
  ASSERT(1>=AR(b),EVRANK);
  if(!bn&&m){xv[i]=zero; RZ(yv[i]=sc(m));}
  else{
   if(!(B01&AT(b)))RZ(b=cvt(B01,b));
   if(!AR(b)){if(*BAV(b)){RZ(xv[i]=IX(m)); RZ(yv[i]=reshape(sc(m),0<q?one:zero));}else xv[i]=yv[i]=mtv; continue;}
   ASSERT(bn==m,EVLENGTH);
   bv=BAV(b); p=0; DO(bn, p+=bv[i];); 
   GATV(t,INT,p,1,0); u=AV(t); xv[i]=t;
   GATV(t,INT,p,1,0); v=AV(t); yv[i]=t; j=-1;
   if(p)switch(q){
    case  1: DO(bn, if(bv[i]){*u++=i  ; if(0<=j)*v++=i-j  ; j=i;}); *v=bn-j;   break;
    case -1: DO(bn, if(bv[i]){*u++=i+1; if(0<=j)*v++=i-j-1; j=i;}); *v=bn-j-1; break;
    case  2: DO(bn, if(bv[i]){*u++=j+1;         *v++=i-j  ; j=i;}); break;
    case -2: DO(bn, if(bv[i]){*u++=j+1;         *v++=i-j-1; j=i;}); break;
   }
  }
 }
 RZ(x=ope(catalog(x)));
 RZ(y=ope(catalog(y)));
 RZ(t=AN(x)?irs2(x,y,0L,1L,1L,jtlamin2):iota(over(shape(x),v2(2L,0L))));
 R cut02(t,w,self);
}    /* a f;.n w for boxed a, with special code for matrix w */


#define MCREL(uu,vv,n)   {A*u=(A*)(uu);A*v=(A*)(vv); DO((n), u[i]=AADR(wd,v[i]););}

#define CUTSWITCH(EACHC)  \
 switch(wd?0:id){A z,*za;C id1,*v1,*zc;I d,i,j,ke,q,*zi,*zs;                 \
  case CPOUND:                                                               \
   GATV(z,INT,m,1,0); zi=AV(z); EACHC(*zi++=d;); R z;                          \
  case CDOLLAR:                                                              \
   GATV(z,INT,m,1,0); zi=AV(z); EACHC(*zi++=d;);                               \
   R irs2(z,vec(INT,MAX(0,r-1),1+s),0L,0L,1L,jtover);                        \
  case CHEAD:                                                                \
   GA(z,t,m*c,r,s); zc=CAV(z); *AS(z)=m;                                     \
   EACHC(ASSERT(d,EVINDEX); MC(zc,v1,k); zc+=k;);                            \
   R z;                                                                      \
  case CTAIL:                                                                \
   GA(z,t,m*c,r,s); zc=CAV(z); *AS(z)=m;                                     \
   EACHC(ASSERT(d,EVINDEX); MC(zc,v1+k*(d-1),k); zc+=k;);                    \
   R z;                                                                      \
  case CCOMMA:                                                               \
  case CLEFT:                                                                \
  case CRIGHT:                                                               \
   e-=e&&neg; RE(d=mult(m*c,e));                                             \
   GA(z,t,d,id==CCOMMA?2:1+r,s-1); zc=CAV(z); fillv(t,d,zc);                 \
   zs=AS(z); zs[0]=m; zs[1]=id==CCOMMA?e*c:e; ke=k*e;                        \
   EACHC(MC(zc,v1,d*k);  zc+=ke;);                                           \
   R z;                                                                      \
  case CBOX:                                                                 \
   GA(z,m?BOX:B01,m,1,0); za=AAV(z);                                         \
   EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k); *za++=y;);             \
   R z;                                                                      \
  case CAT: case CATCO: case CAMP: case CAMPCO:                              \
   if(CBOX==ID(vf->f)&&(id1=ID(vf->g),id1==CBEHEAD||id1==CCTAIL)){           \
    GA(z,m?BOX:B01,m,1,0); za=AAV(z);                                        \
    EACHC(d=d?d-1:0; GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),id1==CBEHEAD?v1+k:v1,d*k); *za++=y;);               \
    R z;                                                                     \
   }                                                                         \
   /* note: fall through */                                                  \
  default:                                                                   \
   if(!m){y=reitem(zero,w); R iota(over(zero,shape(h?df1(y,*hv):CALL1(f1,y,fs))));}                            \
   GATV(z,BOX,m,1,0); za=AAV(z); j=0;                                          \
   switch((wd?2:0)+(h?1:0)){A Zz;                                                 \
    case 0: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k);  RZ(Zz = CALL1(f1,y,fs)); rifv(Zz); *za++=Zz; ); break; \
    case 1: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k);  RZ(Zz = df1(y,hv[j])); j=(1+j)%hn; rifv(Zz); *za++=Zz;); break; \
    case 2: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MCREL(AV(y),v1,d); RZ(Zz = CALL1(f1,y,fs));           rifv(Zz); *za++=Zz;); break; \
    case 3: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MCREL(AV(y),v1,d); RZ(Zz = df1(y,hv[j])); j=(1+j)%hn; rifv(Zz); *za++=Zz;); break; \
   }                                                                         \
   z=ope(z);                                                                 \
   EPILOG(z);                                                                \
 }

#define EACHCUTSP(stmt)  \
 if(pfx)for(i=m;i>=1;--i){q=yu[i-1]-yu[i  ]; d=q-neg; v1=wv+k*(b+p); stmt; p+=q;}  \
 else   for(i=1;i<=m;++i){q=yu[i  ]-yu[i-1]; d=q-neg; v1=wv+k*(b+p); stmt; p+=q;}

static F1(jtcps){A z;P*wp,*zp;
 GA(z,AT(w),1,AR(w),AS(w)); 
 zp=PAV(z);
 wp=PAV(w); 
 SPB(zp,a,SPA(wp,a)); 
 SPB(zp,e,SPA(wp,e));
 SPB(zp,i,SPA(wp,i));
 R z;
}

static A jtselx(J jt,A x,I r,I i){A z;I c,k;
 c=aii(x); k=c*bp(AT(x));
 GA(z,AT(x),r*c,AR(x),AS(x)); *AS(z)=r;
 MC(CAV(z),CAV(x)+i*k,r*k);
 R z;
}    /* (i+i.r){x */

static A jtsely(J jt,A y,I r,I i,I j){A z;I c,*s,*v;
 c=*(1+AS(y));
 GATV(z,INT,r*c,2,0); s=AS(z); s[0]=r; s[1]=c;
 v=AV(z);
 ICPY(v,AV(y)+i*c,r*c);
 DO(r, *v-=j; v+=c;);
 R z;
}    /* ((i+i.r){y)-"1 ({:$y){.j */

static DF2(jtcut2sx){PROLOG(0024);DECLF;A h=0,*hv,y,yy;B b,neg,pfx,*u,*v;C id;I d,e,hn,m,n,p,t,yn,*yu,*yv;P*ap;V*vf;
 PREF2(jtcut2sx);
 n=IC(w); t=AT(w); m=*AV(sv->g); neg=0>m; pfx=m==1||m==-1; b=neg&&pfx;
 RZ(a=a==mark?eps(w,take(num[pfx?1:-1],w)):DENSE&AT(a)?sparse1(a):a);
 ASSERT(n==*AS(a),EVLENGTH);
 ap=PAV(a);
 if(!(equ(zero,SPA(ap,e))&&AN(SPA(ap,a))))R cut2(cvt(B01,a),w,self); 
 vf=VAV(fs);
 if(VGERL&sv->flag){h=sv->h; hv=AAV(h); hn=AN(h); id=0;}else id=vf->id; 
 y=SPA(ap,i); yn=AN(y); yv=AV(y); u=v=BAV(SPA(ap,x)); e=m=0;
 GATV(yy,INT,1+yn,1,0); yu=AV(yy); *yu++=p=pfx?n:-1;
 switch(pfx+(id==CLEFT||id==CRIGHT||id==CCOMMA?2:0)){
  case 0:          DO(yn, if(*v){++m;      *yu++=  yv[v-u];              } ++v;); break;
  case 1: v+=yn-1; DO(yn, if(*v){++m;      *yu++=  yv[v-u];              } --v;); break;
  case 2:          DO(yn, if(*v){++m; d=p; *yu++=p=yv[v-u]; e=MAX(e,p-d);} ++v;); break;
  case 3: v+=yn-1; DO(yn, if(*v){++m; d=p; *yu++=p=yv[v-u]; e=MAX(e,d-p);} --v;);
 }
 yu=AV(yy); p=pfx?yu[m]:0;
 if(t&DENSE){C*wv;I c,k,r,*s;
  r=MAX(1,AR(w)); s=AS(w); wv=CAV(w); c=aii(w); k=c*bp(t); RELBASEASGNB(w,w);
  CUTSWITCH(EACHCUTSP)
 }else if(id==CPOUND){A z;I i,*zi; 
  GATV(z,INT,m,1,0); zi=AV(z); 
  if(pfx)for(i=m;i>=1;--i)*zi++=(yu[i-1]-yu[i  ])-neg;
  else   for(i=1;i<=m;++i)*zi++=(yu[i  ]-yu[i-1])-neg;
  EPILOG(z);
 }else{A a,ww,x,y,z,*za,zz;I c,i,j,q,qn,r;P*wp,*wwp;
  wp=PAV(w); a=SPA(wp,a); x=SPA(wp,x); y=SPA(wp,i); yv=AV(y); r=*AS(y); c=*(1+AS(y));
  RZ(ww=cps(w)); wwp=PAV(ww);
  GATV(z,BOX,m,1,0); za=AAV(z);
  switch(AN(a)&&*AV(a)?2+pfx:pfx){
   case 0:
    p=yu[0]; DO(r, if(p<=yv[c*i]){p=i; break;});
    for(i=1;i<=m;++i){
     j=yu[i]; DO(q=r-p, if(j<yv[c*(p+i)]){q=i; break;}); qn=q;
     if(neg)DO(qn=r-p, if(j-1<yv[c*(p+i)]){qn=i; break;});
     *AS(ww)=(yu[i]-yu[i-1])-neg; 
     SPB(wwp,i,sely(y,qn,p,1+yu[i-1]));
     SPB(wwp,x,selx(x,qn,p));
     RZ(*za++=zz=h?df1(ww,hv[(i-1)%hn]):CALL1(f1,ww,fs));
     // reallocate ww if it was used, which we detect by seeing the usecount incremented.  This requires that everything that
     // touches a buffer either copy it or rat().  So that ] doesn't have to rat(), we also detect reuse here if the same buffer
     // is returned to us
     p+=q; if(WASINCORP1(zz,ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
   case 1:
    p=yu[m]; DO(r, if(p<=yv[c*i]){p=i; break;});
    for(i=m;i>=1;--i){
     j=yu[i-1]; DO(q=r-p, if(j<=yv[c*(p+i)]){q=i; break;}); qn=q;
     if(neg){j=yu[i]; qn=0; DO(r-p, if(j<yv[c*(p+i)]){qn=q-i; break;});}
     *AS(ww)=(yu[i-1]-yu[i])-neg; 
     SPB(wwp,i,sely(y,qn,p+q-qn,yu[i]+neg));
     SPB(wwp,x,selx(x,qn,p+q-qn));
     RZ(*za++=zz=h?df1(ww,hv[(m-i)%hn]):CALL1(f1,ww,fs));
     p+=q; if(WASINCORP1(zz,ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
   case 2:
    for(i=1;i<=m;++i){
     q=yu[i]-yu[i-1]; *AS(ww)=q-neg;
     SPB(wwp,x,irs2(apv(q-neg,p,1L),x,0L,1L,-1L,jtfrom));
     RZ(*za++=zz=h?df1(ww,hv[(i-1)%hn]):CALL1(f1,ww,fs));
     p+=q; if(WASINCORP1(zz,ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
   case 3:
    for(i=m;i>=1;--i){
     q=yu[i-1]-yu[i]; *AS(ww)=q-neg;
     SPB(wwp,x,irs2(apv(q-neg,p+neg,1L),x,0L,1L,-1L,jtfrom));
     RZ(*za++=zz=h?df1(ww,hv[(i-1)%hn]):CALL1(f1,ww,fs));
     p+=q; if(WASINCORP1(zz,ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
  }
  z=ope(z);
  EPILOG(z);
}}   /* sparse f;.n (dense or sparse) */


static C*jtidenv0(J jt,A a,A w,V*sv,I zt,A*zz){A fs,y;
 *zz=0; 
 fs=sv->f;
 RE(y=df1(zero,iden(VAV(fs)->f)));
 if(TYPESLT(zt,AT(y))){*zz=df1(cut2(a,w,cut(ds(CBOX),sv->g)),amp(fs,ds(COPE))); R 0;}
 if(TYPESGT(zt,AT(y)))RE(y=cvt(zt,y)); 
 R CAV(y);
}    /* pointer to identity element */

/* locals in cut2:                       */
/* b    1 iff _1 cut                     */
/* c    atoms in an item of w            */
/* d    adjusted length of current cut   */
/* e    max width of a cut               */
/* h    gerund                           */
/* hv   gerund                           */
/* id   function code                    */
/* k    # bytes in an item of w          */
/* m    # of cuts                        */
/* n    #a and #w                        */
/* neg  1 iff _1 or _2 cut               */
/* p    remaining length in a            */
/* pfx  1 iff 1 or _1 cut                */
/* q    length of current cut            */
/* sep  the cut character                */
/* u    ptr to a for next    cut         */
/* v    ptr to a for current cut         */
/* v1   ptr to w for current cut         */
/* wd   non0 iff w is relative              */
#if 1
// the values are 1 byte, either 8 bits or 1 bit
#define FRETLOOPBYTE(decl,compI,comp) \
{ \
 UC val=*(UC*)fret, *avv=(UC*)av;  /* compare value, pointer to input */ \
 decl /* other variables needed */ \
 d=-pfx; I nleft=n;  /* if prefix, first omitted fret starts at length 0; set number of items yet to process */ \
 /* The search step finds one fret and consumes it.  For pfx, the first count is the #items BEFORE the first fret, \
 subsequent items are the interfret distance, and the last is the length of the remnant PLUS 1 to account for the \
 final fret which is in the last partition.  For !pfx, the first count INCLUDES the first fret, subsequent items are interfret \
 distance, and the remnant at the end AFTER that last fret is discarded. */ \
 while(nleft){I cmpres=0;  /* =0 for the warning */ \
  /* for byte-at-a-time searches we can save some time by skipping 4/8 bytes at a time */ \
  while(nleft>=SZI){I avvI = *(I*)avv; /* read one word */ \
   compI  /* Code (if any) to convert avvI to a value that is non0 iff there is a match */ \
   if(avvI!=0){  /* if we can't skip,  exit loop and search byte by byte */ \
    I skiphalf=((avvI&IHALF0)==0)<<(LGSZI-1); avv+=skiphalf; d+=skiphalf; nleft-=skiphalf;  /* if first half empty, skip over it - remove if unaligned load penalty */ \
    break; \
   } \
   avv+=SZI; d+=SZI; nleft-=SZI;  /* skip the whole 8 bytes */ \
  } \
  if(!nleft)break;  /* if we skipped over everything, we're through */ \
  I testct=BW; testct=(nleft>testct)?testct:nleft;  /* testct=# compares to do, BW max */ \
  /* rattle off compares; save the number in cmpres, MSB=1st compare.  We keep the count the same for prediction */ \
  I testi=testct; do{UI match=(comp); ++avv; cmpres=2*cmpres+match;}while(--testi); \
  /* process them out of cmpres, writing lengths */ \
  testi=testct;  /* save # cells processed */ \
  cmpres<<=BW-testct;  /* if we didn't shift in BW bits, move the first one we did shift to the MSB */ \
  while(cmpres){ \
   UI4 ctz; CTLZI(cmpres,ctz); I len=BW-ctz; testct-=len; d+=len; /* get # leading bits including the 1; decr count of unprocessed bits; set d=length of next field to output */ \
   if(d<255)*pd++ = (UC)d; else{*pd++ = 255; *(UI4*)pd=(UI4)d; pd+=SZUI4; m-=SZUI4;}  /* write out encoded length; keep track of # long fields emitted */ \
   if(pd>=pdend){RZ(pd0=jtgetnewpd(jt,pd,pd0)); pdend=(C*)CUTFRETEND(pd0); pd=CUTFRETFRETS(pd0);}  /* if we filled the current buffer, get a new one */ \
   cmpres<<=1; cmpres<<=(len-=1); d=0;   /* discard bit up to & incl the fret; clear the carryover of #cells in partition */ \
  } \
  d += testct;  /* add in any bits not shifted out of cmpres as going into d */ \
  nleft -= testi;  /* decr number of cells to do */ \
 } \
}

// Template for comparisons without byte-wide checks
#define FRETLOOPNONBYTE(decl, comp) \
{ \
 decl \
 d=-pfx; I nleft=n;  /* if prefix, first omitted fret starts at length 0; set number of items yet to process */ \
 /* The search step finds one fret and consumes it.  For pfx, the first count is the #items BEFORE the first fret, \
 subsequent items are the interfret distance, and the last is the length of the remnant PLUS 1 to account for the \
 final fret which is in the last partition.  For !pfx, the first count INCLUDES the first fret, subsequent items are interfret \
 distance, and the remnant at the end AFTER that last fret is discarded. */ \
 while(nleft){I cmpres=0;  /* =0 for the warning */ \
  I testct=BW; testct=(nleft>testct)?testct:nleft;  /* testct=# compares to do, BW max */ \
  /* rattle off compares; save the number in cmpres, MSB=1st compare.  We keep the count the same for prediction */ \
  I testi=testct; do{comp   cmpres=2*cmpres+match;}while(--testi); \
  /* process them out of cmpres, writing lengths */ \
  testi=testct;  /* save # cells processed */ \
  cmpres<<=BW-testct;  /* if we didn't shift in BW bits, move the first one we did shift to the MSB */ \
  while(cmpres){ \
   UI4 ctz; CTLZI(cmpres,ctz); I len=BW-ctz; testct-=len; d+=len; /* get # leading bits including the 1; decr count of unprocessed bits; set d=length of next field to output */ \
   if(d<255)*pd++ = (UC)d; else{*pd++ = 255; *(UI4*)pd=(UI4)d; pd+=SZUI4; m-=SZUI4;}  /* write out encoded length; keep track of # long fields emitted */ \
   if(pd>=pdend){RZ(pd0=jtgetnewpd(jt,pd,pd0)); pdend=(C*)CUTFRETEND(pd0); pd=CUTFRETFRETS(pd0);}  /* if we filled the current buffer, get a new one */ \
   cmpres<<=1; cmpres<<=(len-=1); d=0;   /* discard bits up to & incl the fret; clear the carryover of #cells in partition */ \
  } \
  d += testct;  /* add in any bits not shifted out of cmpres as going into d */ \
  nleft -= testi;  /* decr number of cells to do */ \
 } \
}

// the values are intolerant values that match a machine variable size
#define FRETLOOPSGL(T) FRETLOOPNONBYTE(T val=*(T*)fret; T *avv=(T*)av;  , \
  UI match=(val==*avv); ++avv; \
)

// the values are D, which use tolerance
#define FRETLOOPSGLD FRETLOOPNONBYTE(D val=*(D*)fret; D *avv=(D*)av; D cct=1.0-jt->ct;  , \
  UI match=(TCMPEQB(cct,val,*avv)); ++avv; \
)
#if 0 // scaf not used
// the values are of irregular length, but short enough that we should do all the comparisons to avoid misprediction
// k is the number of bytes to compare
#define FRETLOOPMULTFIX FRETLOOPNONBYTE(void *avv=av;  , \
   I ni; UI match=1;void *f=fret; \
   for(ni=k-SZI;ni>=0;ni-=SZI){match&=(*(I*)f==*(I*)avv); avv=(I*)avv+1; f=(I*)f+1;} /* compare the fullwords */ \
   if((BW==64)&&(k&4)){match&=(*(UI4*)f==*(UI4*)avv); avv=(UI4*)avv+1; f=(UI4*)f+1;} \
   if(k&2){match&=(*(US*)f==*(US*)avv); avv=(US*)avv+1; f=(US*)f+1;} \
   if(k&1){match&=(*(UC*)f==*(UC*)avv); avv=(UC*)avv+1; f=(UC*)f+1;} \
)

// the values are of irregular length and we break out of the compare loop
// k is the number of bytes to compare
#define FRETLOOPMULTVAR FRETLOOPNONBYTE(void *avv=av;  , \
   I ni; void *f=fret; I match; \
   for(ni=k-SZI;ni>=0;ni-=SZI){if(*(I*)f!=*(I*)avv)break; avv=(I*)avv+1; f=(I*)f+1;} /* compare the fullwords */ \
   if(!(match=(UI)ni>>(BW-1))){avv=(C*)avv+ni+SZI; f=(C*)f+ni+SZI; \
   }else{if((BW==64)&&(k&4)){match&=(*(UI4*)f==*(UI4*)avv); avv=(UI4*)avv+1; f=(UI4*)f+1;} \
   if(k&2){match&=(*(US*)f==*(US*)avv); avv=(US*)avv+1; f=(US*)f+1;} \
   if(k&1){match&=(*(UC*)f==*(UC*)avv); avv=(UC*)avv+1; f=(UC*)f+1;} \
   } \
)

// the values are of irregular length and we break out of the compare loop - tolerant version
// k is the number of bytes to compare
#define FRETLOOPMULTVARTOL FRETLOOPNONBYTE(D *avv=(D*)av; D ctol=1-jt->ct;  , \
   I ni; D *f=(D*)fret; I match; \
   for(ni=k>>LGSZD;ni>0;ni--){if(TCMPNE(ctol,*f,*avv))break; avv=avv+1; f=f+1;} /* compare the fullwords */ \
   match=(UI)(ni-1)>>(BW-1); avv=avv+ni; f=f+ni; \
)
#endif

// 1st word in buf is chain, 2nd is end+1 of data
#define EACHCUT(stmt) \
 do{UC *pdend=(UC*)CUTFRETEND(pd0);   /* 1st ele is # eles; get &chain  */ \
  while(pd<pdend){   /* step to first/next; process each fret.  Quit when pointing to end */ \
   UI len=*pd++; if(len==255){len=*(UI4*)pd; pd+=SZUI4;} d=len-neg;  /* fetch size, adjust if neg */ \
   stmt;  /* perform user function */  \
   v1 += len*k;  /* advance to next fret position in w */ \
  }  /* loop till buffer all processed */ \
  if(!(pd0=CUTFRETCHAIN(pd0)))break;  /* step to next buffer; exit if none */ \
  pd=CUTFRETFRETS(pd0);  /* point to 1st d */  \
 }while(1);  /* when buffer exhausted, chain to next; stop when no next */


// allocate a new fret buffer and chain it to the old one
// pd is &last element+1 (i. e. endptr of current buffer)
// pd0 is &current buffer
// result is &new buffer, or 0 if error
// we fill in the chain and length fields in the current buffer, and set the length field of the new
// buffer to the size we allocated
#define FRETALLOSIZE (512-10)  // number of frets to allocate at a time - if they are long type (i. e. I).  For frets less than 255
   // items long, we can store 4 or 8 times as many
static A jtgetnewpd(J jt, UC* pd, A pd0){A new;
 CUTFRETEND(pd0)=(I)pd;  // fill in size of current buffer
 GATV(new,INT,FRETALLOSIZE,0,0);  // get a new buffer.  rank immaterial.  Use GATV because of large size
 CUTFRETCHAIN(pd0)=new;  // chain the new block to the old
 pd0=new;  // step pd0 to new block
 CUTFRETEND(pd0)=(I)(CUTFRETFRETS(pd0)+(FRETALLOSIZE*SZI)-10);  // return value: endptr of new buffer
 R pd0;  // return address of data in the new block
}


DF2(jtcut2){F2PREFIP;PROLOG(0025);DECLF;A *hv,z,zz;I neg,pfx;C id,*v1,*wv,*zc;
     I ak,at,wcn,d,hn,k,m=0,n,r,wt,*zi;I d1[128]; A pd0; UC *pd, *pdend;
 PREF2(jtcut2);
 if(SB01&AT(a)||SPARSE&AT(w))R cut2sx(a,w,self);
#define ZZFLAGWORD state
 I state=0;  // init flags, including zz flags

 n=IC(w); wt=AT(w);   // n=#items of w; wt=type of w
 r=MAX(1,AR(w)); wv=CAV(w); wcn=aii(w); k=wcn*bp(wt);   // r=rank>.1, s->w shape, wv->w data, wcn=#atoms in cell of w, k=#bytes in cell of w;
 // If the verb is a gerund, it comes in through h, otherwise the verb comes through f.  Set up for the two cases
 if(!(VGERL&sv->flag)){id=FAV(fs)->id;  // if verb, point to its data and fetch its pseudocharacter
  // not gerund: OK to test fs
  if(FAV(fs)->mr>=r){
   // we are going to execute f without any lower rank loop.  Thus we can use the BOXATOP etc flags here.  These flags are used only if we go through the full assemble path
   state = (FAV(fs)->flag2&VF2BOXATOP1)>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // Don't touch fs yet, since we might not loop
   state &= ~((FAV(fs)->flag2&VF2ATOPOPEN1)>>(VF2ATOPOPEN1X-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
   state |= (-state) & FAV(self)->flag2 & (VF2WILLBEOPENED|VF2COUNTITEMS); // remember if this verb is followed by > or ; - only if we BOXATOP, to avoid invalid flag setting at assembly
  }
 }else{
  state |= STATEHASGERUND; A h=sv->h; hv=AAV(h); hn=AN(h); ASSERT(hn,EVLENGTH);  // Gerund case.  Mark it, set hv->1st gerund, hn=#gerunds.  Verify gerunds not empty
  id=0;  // set an invalid pseudochar id for the gerund, to indicate 'not a primitive'
 }

 // Time to find the frets.  If a is the impossible type INT+LIT, a contains the frets already, in the single buffer (that means we are acting on behalf of Key /.)
 if(sv->id==CCUT){   // see if we are acting on behalf of /.  Fall through if not
  pfx=IAV(sv->g)[0]; neg=(UI)pfx>>(BW-1); pfx&=1;  // neg=cut type is _1/_2; pfx=cut type is 1/_1
  if(a!=mark){  // dyadic forms
   if(((AN(a)-1)&(-n))<0){  // empty x, do one call on the entire w if y is non-empty
    if(state&STATEHASGERUND){A h=hv[0]; R CALL1(VAV(h)->f1,w,h);}
    else R CALL1(f1,w,fs);
   }
   if(((-AN(a))&(-(BOX&AT(a))))<0)R cut2bx(a,w,self);  // handle boxed a separately if a not empty
   if(!(B01&AT(a)))RZ(a=cvt(B01,a));  // convert other a to binary, error if impossible
   if(!AR(a))RZ(a=reshape(sc(n),a));   // extend scalar x to length of y
   ak=1; at=B01;  // cell of a is 1 byte, and it's Boolean
  }else{
   // monadic forms.  If we can handle the type/length here, leave it; otherwise convert to Boolean.
   // If w is Boolean, we have to pretend it's LIT so we use the correct fret value rather than 1
   if(wt&(B01|LIT|INT|FL|C2T|C4T|SBT)&&k&&((BW==32&&wt&FL&&k==SZD)||(k<=SZI&&(k&-k)==k))){a=w; ak=k; at=(wt+B01)&~B01;  // monadic forms: if w is an immediate type we can handle, and the length is a machine-word length, use w unchanged
   }else{RZ(a=n?eps(w,take(num[pfx?1:-1],w)):mtv); ak=1; at=B01;}  // any other w, replace by w e. {.w (or {: w).  Set ak to the length of a cell of a, in bytes.  Empty cells of w go through here to convert to list
  }
  ASSERT(n==IC(a),EVLENGTH);

  // *fret is value to match; n is #items to match; pd0=&d1, pd->d1 fretarea pdend=&d1[max+1] (pointers into current fret buffer) k=item length av->data to compare
  pd0=(A)&d1; pd=CUTFRETFRETS(pd0); pdend=(C*)&d1+sizeof(d1)-10;  // pd0, pd, pdend start out set for first buffer

  void *av=voidAV(a);  // point to the start of the fret data
  void *fret=(C*)av+ak*((n-1)&(pfx-1));  // point to the fret
  // The type of a is always one we can handle here - other types have been converted to B01.  B01 types look for 1, others look for fret value.  Select routine based on length/tolerance/byte-boolean
  I rtnx = CTTZ(k); rtnx=(at&B01)?4:rtnx; rtnx=(at&FL)?5:rtnx;  // 0-3=bytes, 4=B01, 5=FL
  switch(rtnx){
  case 0: // single bytes
   FRETLOOPBYTE(I valI=val|((UI)val<<8); valI|=valI<<16; if(BW==64)valI|=valI<<(BW/2);   ,
    I avvdiff=valI^avvI; avvI=((~avvdiff)&(avvdiff-(I)0x0101010101010101))&(I)0x8080808080808080;    ,
    val==*avv
    ) break;
  case 1: // 2 bytes
   FRETLOOPSGL(US) break;
  case 2: // 4 bytes
   FRETLOOPSGL(UI4) break;
#if BW==64
  case 3: // 8 bytes
   FRETLOOPSGL(UI) break;
#endif
  case 4: // single-byte Boolean, looking for 1s
   FRETLOOPBYTE( , , *avv) break;
  case 5: // float (tolerant)
   FRETLOOPSGLD break;
  }

  // partition sizes have been calculated.
  // prepare for the looping.  We need:
  // pd0 pointing to first buffer
  // pd->first d to process.  We skip the first d if pfx
  // m has # d values
  // v1->first w-cell to process, taking into account skipped cells owing to neg and omitted headers

  // if pfx, emit the runout field for the last d (if suffix these are discarded).  We must add one to d
  // to include the prefix in the length of d
  if(pfx){++d; if(d<255)*pd++ = (UC)d; else{*pd++ = 255; *(UI4*)pd=(UI4)d; pd+=SZUI4; m-=SZUI4;}}
  // close the last d buffer
  CUTFRETCHAIN(pd0)=0; CUTFRETEND(pd0)=(I)pd;  // set end-of-chain and endptr in last d buffer
  // chase the chains to count the number of d values.  Each is one byte long, except for the long ones which are 1+SZI bytes.  But we subtracted SZI from m for
  // every long encoding, so if we just total the number of bytes in the blocks and add it to m, we will have the number of d-values
  for(pd0=(A)&d1; pd0; pd0=CUTFRETCHAIN(pd0))m += (UC*)CUTFRETEND(pd0)-CUTFRETFRETS(pd0);  // add in bufend-bufstart
  // set pointers to first d and first cell
  v1=wv; pd0=(A)&d1; pd=CUTFRETFRETS(pd0);
  // if pfx, skip over the first d and any w-cells indicated in it.  Also skip over 1 more w-cell if neg indicates the first cell should be skipped
  if(pfx){
   UI len=*pd++; if(len==255){len=*(UI4*)pd; pd+=SZUI4;}  // get first d and skip over it
   v1+=(len+neg)*k;  // skip over the before-first-prefix cells, and the fret cell if neg
   --m;  // remove the discarded d from the count
  }
 }else{
  // Here we are processing /.; the original a had the fret sizes built already.  Just set up the loop variables based on them
  m=CUTFRETCOUNT(a);  // # frets, set by caller
  pd0=a;  // &first block of frets
  pd=CUTFRETFRETS(a);  // &first fret
  CUTFRETCHAIN(a)=0;  // indicate only 1 block in use.  Note this overlaps with AK & thus cannot be set until pd extracted
  v1=wv;  // we always start with the first cell
     // CUTFRETEND in input block is set by caller
  pfx=1; neg=0;  // This is a ;.1 cut
 }

 // At this point we have m, the number of result cells; pd0, pointer to first block of lengths; pd, pointer to first fret-length to use; v1, pointer to first participating cell of w; pfx, neg

 // process, handling special cases
 zz=0;   // indicate no result from special cases
 switch(id){
  case CPOUND:
   GATV(zz,INT,m,1,0); zi=AV(zz); EACHCUT(*zi++=d;); 
   break;
  case CDOLLAR:
   GATV(zz,INT,m,1,0); zi=AV(zz); EACHCUT(*zi++=d;);
   R irs2(zz,vec(INT,MAX(0,r-1),AS(w)+1),0L,0L,1L,jtover);
  case CHEAD:
   GA(zz,wt,m*wcn,r,AS(w)); zc=CAV(zz); *AS(zz)=m;
   EACHCUT(if(d)MC(zc,v1,k); else fillv(wt,wcn,zc); zc+=k;);
   break;
  case CTAIL:
   GA(zz,wt,m*wcn,r,AS(w)); zc=CAV(zz); *AS(zz)=m;
   EACHCUT(if(d)MC(zc,v1+k*(d-1),k); else fillv(wt,wcn,zc); zc+=k;);
   break;
// obsolete   case CCOMMA:
// obsolete   case CLEFT:
// obsolete   case CRIGHT:
// scaf MUST CALCULATE e or discard this, which might be better
// obsolete    e-=e&&neg; RE(d=mult(m*c,e));
// obsolete    GA(zz,wt,d,id==CCOMMA?2:1+r,s-1); zc=CAV(zz); fillv(wt,d,zc);
// obsolete    zs=AS(zz); zs[0]=m; zs[1]=id==CCOMMA?e*c:e; ke=k*e;
// obsolete    EACHCUT(MC(zc,v1,d*k);  zc+=ke;);
// obsolete    break;
// obsolete  case CBOX:
// obsolete // scaf perhaps should discard this, if the box can be labeled BOXATOP+null fn (result will create recursive block)
// obsolete   GA(zz,m?BOX:B01,m,1,0); za=AAV(zz);
// obsolete    EACHCUT(GA(z,wt,d*c,r,s); *AS(z)=d; MC(AV(z),v1,d*k); *za++=z;);
// obsolete    break;
// obsolete   case CAT: case CATCO: case CAMP: case CAMPCO:
// scaf should take this under BOXATOP?
// obsolete    if(CBOX==ID(vf->f)&&(id1=ID(vf->g),id1==CBEHEAD||id1==CCTAIL)){
// obsolete     GA(zz,m?BOX:B01,m,1,0); za=AAV(zz);
// obsolete     EACHCUT(d=d?d-1:0; GA(z,wt,d*c,r,s); *AS(z)=d; MC(AV(z),id1==CBEHEAD?v1+k:v1,d*k); *za++=z;);
// obsolete    }
// obsolete    break;
  case CSLASH:
   {
   VA2 adocv = vains(FAV(fs)->f,wt);  // qualify the operation, returning action routine and conversion info
   if(adocv.f){C*z0=0,*zc;I t,zk,zt;  // if the operation is a primitive that we can  apply / to...
    zt=rtype(adocv.cv);
    GA(zz,zt,m*wcn,r,AS(w)); *AS(zz)=m; 
    if(!AN(zz))R zz;
    I atomsize=bp(zt);
    zc=CAV(zz); zk=wcn*atomsize;
    if((t=atype(adocv.cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wv=CAV(w);}
// obsolete     EACHCUT(if(d)adocv.f(jt,1L,d*c,d,zc,v1); else{if(!z0){z0=idenv0(a,w,sv,zt,&z); 
    EACHCUT(if(d)adocv.f(jt,1L,wcn,d,zc,v1); else{if(!z0){z0=idenv0(a,w,sv,zt,&z); // compared to normal prefixes, c means d and d means n
        if(!z0){if(z)R z; else break;}} mvc(zk,zc,atomsize,z0);} zc+=zk;);
    if(jt->jerr)R jt->jerr>=EWOV?cut2(a,w,self):0; else R adocv.cv&VRI+VRD?cvz(adocv.cv,zz):zz;
    break;
    }
  }
 }

 // If we didn't get a result from the special cases, run the normal result loop
 if(!zz){
  if(m){
   // There are cells.  Run the result loop over them
// obsolete    if(!(state&STATEHASGERUND))ZZFLAGWORD |= ((VAV(fs)->mr>=r?VF2BOXATOP1:0)&(VAV(fs)->flag2&VF2BOXATOP1))>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // If this is BOXATOP, set so for loop.  Don't touch fs yet, since we might not loop
   // Allocate the virtual block we will use for arguments
   A virtw; fauxblock(virtwfaux); fauxvirtual(virtw,virtwfaux,w,r,ACUC1|ACINPLACE);  // allocate UNINCORPORABLE block
// obsolete  RZ(virtw=virtual(w,0,r));
   // Copy in the shape of a cell.  The number of cells in a subarray will depend on d
   MCIS(AS(virtw)+1,AS(w)+1,r-1); // obsolete I* virts=AS(virtw); DO(r-1, virts[i+1]=s[i+1];)
   // Set the offset to the first data
   AK(virtw)=v1-(C*)virtw;  // v1 is set to point to starting cell; transfer that info
// obsolete    AFLAG(virtw)|=AFUNINCORPABLE;  // indicate that this is a moving virtual block and cannot EVER be incorporated
   // Self-virtual blocks also modify the shape of a block, but that code notifies
   // us through a flag bit.
   jtinplace = (J)((I)jtinplace & ((((wt&TYPEVIPOK)!=0)&(AC(w)>>(BW-1)))*JTINPLACEW-(JTINPLACEW<<1)));  // turn off inplacing unless DIRECT and w is inplaceable

#define ZZDECL
#include "result.h"
// obsolete    I mtmp=m; ZZPARMS(0,0,&mtmp,1,m,1);
   ZZPARMS(1,m,1)
#define ZZINSTALLFRAME(optr) *optr++=m;
   I gerundx=0;  // if we have gerunds, this indicates which one we should run next

   do{UC *pdend=(UC*)CUTFRETEND(pd0);   /* 1st ele is # eles; get &chain  */
    while(pd<pdend){   /* step to first/next; process each fret.  Quit when pointing to end */
     UI len=*pd++; if(len==255){len=*(UI4*)pd; pd+=SZUI4;} d=len-neg;  /* fetch size, adjust if neg */
     AS(virtw)[0]=d; AN(virtw)=wcn*d; // install the size of the partition into the virtual block, and # atoms
     // call the user's function
     if(!(state&STATEHASGERUND)){
      RZ(z=CALL1IP(f1,virtw,fs));  //normal case
     }else{
      RZ(z=df1(virtw,hv[gerundx])); ++gerundx; gerundx=(gerundx==hn)?0:gerundx;  // gerund case.  Advance gerund cyclically
     }

#define ZZBODY  // assemble results
#include "result.h"

     AK(virtw) += len*k;   // advance to data position of next fret
// obsolete      if(AFLAG(virtw)&AFVIRTUALINPLACE){
// obsolete           // The block was self-virtualed or otherwise modified its header.  Restore its original shape/type
// obsolete        AR(virtw)=(RANKT)r; AT(virtw)=wt; MCIS(AS(virtw)+1,AS(w)+1,r-1); AFLAG(virtw) &= ~AFVIRTUALINPLACE;  // restore all fields that might have been modified.  Pity there are so many
// obsolete      }
    }  /* loop till buffer all processed */
    if(!(pd0=CUTFRETCHAIN(pd0)))break;  /* step to next buffer; exit if none */
    pd=CUTFRETFRETS(pd0);  /* point to 1st d */
   }while(1);  /* when buffer exhausted, chain to next; stop when no next */

#define ZZEXIT  // handle fill & dissimilar results
#include "result.h"

  }else{
   // No frets.  Apply the operand to 0 items; return (0,$result) $ result (or $,'' if error on fill-cell)
   RZ(z=reitem(zeroi,w));  // create 0 items of the type of w
   UC d=jt->db; jt->db=0; zz=(state&STATEHASGERUND)?df1(z,hv[0]):CALL1(f1,z,fs); jt->db=d; if(EMSK(jt->jerr)&EXIGENTERROR)RZ(zz); RESETERR;
   RZ(zz=reshape(over(zero,shape(zz?zz:mtv)),z));
// obsolete   z=reitem(zero,w); zz=(state&STATEHASGERUND)?df1(z,hv[0]):CALL1(f1,z,fs); if(EMSK(jt->jerr)&EXIGENTERROR)RZ(zz); RESETERR; R iota(over(zero,shape(zz?zz:mtv)));
  }
 }
 EPILOG(zz);
}    /* f;.1  f;._1  f;.2  f;._2  monad and dyad */
#else

// execute stmt on each fret.  Here we are handling special cases so we know the results are well-behaved
// and stmt can just move its result to the output area
#define EACHCUT(stmt)  \
 for(i=1;i<=m;++i){                                 \
  if(pfx&&i==m)q=p;                                 \
  else{u=memchr(v+pfx,sep,p-pfx); u+=!pfx; q=u-v;}  \
  d=q-neg; v1=wv+k*(b+n-p);                         \
  stmt;                                             \
  p-=q; v=u;                                        \
 }



// Here for general case, where results must be boxed & reopened. y is a temp name used for the input cell and its result.
// allbx starts as 1; if all results return atomic boxes, it stays there.  When allbx is set, the contents of the
// result (a single A) is copied to the result; otherwise the result itself is copied as a cell (& will be unboxed later).
// If a non-atomic-box result is encountered, previous results are revisited to box them.
#define EACHCUTG(stmt)  \
 for(i=1;i<=m;++i){                                 \
  if(pfx&&i==m)q=p;                                 \
  else{u=memchr(v+pfx,sep,p-pfx); u+=!pfx; q=u-v;}  \
  d=q-neg; v1=wv+k*(b+n-p);                         \
  old=jt->tnextpushx;                           \
  GA(y,wt,d*c,r,s); *AS(y)=d;                       \
  stmt;                                             \
  if(allbx&&!AR(y)&&BOX&AT(y))*za++=y=*AAV(y);      \
  else if(!allbx){rifv(y); *za++=y;}                \
  else{I ii=i-1;                                    \
   allbx=0;                                         \
   za=AAV(z); DO(ii, RZ(*za++=rifvs(box(*za)));); rifv(y); *za++=y; \
   old=jt->tnextpushx;                          \
  }                                                 \
  y=gc(y,old);                                        \
  p-=q; v=u;                                        \
 }

static DF2(jtcut2){PROLOG(0025);DECLF;A h=0,*hv,y,z=0,*za;B b,neg,pfx;C id,id1,sep,*u,*v,*v1,*wv,*zc;
     I c,cv,e=0,d,hn,i,k,ke,m=0,n,old,p,q,r,*s,wt,*zi,*zs;V*vf;VF ado;
 PREF2(jtcut2);
 if(SB01&AT(a)||SPARSE&AT(w))R cut2sx(a,w,self);
 p=n=IC(w); wt=AT(w); k=*AV(sv->g); neg=0>k; pfx=k==1||k==-1; b=neg&&pfx;  // todo kludge combine flag bits
 if(a!=mark){  // dyadic forms
  if(!AN(a)&&n){  // empty x, do one call if y is non-empty
   if(VGERL&sv->flag){h=sv->h; ASSERT(AN(h),EVLENGTH); h=*AAV(h); R CALL1(VAV(h)->f1,w,h);}
   else R CALL1(f1,w,fs);
  }
  if(AN(a)&&BOX&AT(a))R cut2bx(a,w,self);  // handle boxed a separately
  if(!(B01&AT(a)))RZ(a=cvt(B01,a));  // convert other a to binary
  if(!AR(a))RZ(a=reshape(sc(n),a));   // extend scalar x to length of y
  v=CAV(a); sep=C1;
 }else if(1>=AR(w)&&wt&IS1BYTE){a=w; v=CAV(a); sep=v[pfx?0:n-1];}  // monadic forms: 1-byte w, use w itself as the fret vector
 else{RZ(a=n?eps(w,take(num[pfx?1:-1],w)):mtv); v=CAV(a); sep=C1;}  // multibyte w, calculate frets
 // now v->byte vector of frets, sep=fret character
 ASSERT(n==IC(a),EVLENGTH);
 vf=VAV(fs);
 if(VGERL&sv->flag){h=sv->h; hv=AAV(h); hn=AN(h); id=0;}else id=vf->id; 
 r=MAX(1,AR(w)); s=AS(w); wv=CAV(w); c=aii(w); k=c*bp(wt); RELBASEASGNB(w,w);
 // count the frets
 switch(pfx+(id==CLEFT||id==CRIGHT||id==CCOMMA?2:0)){
  case 0: if(AT(a)&B01&&C1==sep)m=bsum(n,v); 
          else{--v;    DO(n, if(sep==*++v) ++m;                    ); v=CAV(a);}    break;
  case 1: if(AT(a)&B01&&C1==*v )m=bsum(n,v);
          else{u=v+=n; DO(n, if(sep==*--v){++m;               u=v;}); p-=u-v; v=u;} break;
  case 2: u=--v;       DO(n, if(sep==*++v){++m; e=MAX(e,v-u); u=v;}); v=CAV(a);     break;
  case 3: u=v+=n;      DO(n, if(sep==*--v){++m; e=MAX(e,u-v); u=v;}); p-=u-v; v=u;
 }
 // process, handling special cases
 switch(wd?0:id){
  case CPOUND:
   GATV(z,INT,m,1,0); zi=AV(z); EACHCUT(*zi++=d;); 
   break;
  case CDOLLAR:
   GATV(z,INT,m,1,0); zi=AV(z); EACHCUT(*zi++=d;);
   R irs2(z,vec(INT,MAX(0,r-1),1+s),0L,0L,1L,jtover);
  case CHEAD:
   GA(z,wt,m*c,r,s); zc=CAV(z); *AS(z)=m;
   EACHCUT(if(d)MC(zc,v1,k); else fillv(wt,c,zc); zc+=k;);
   break;
  case CTAIL:
   GA(z,wt,m*c,r,s); zc=CAV(z); *AS(z)=m;
   EACHCUT(if(d)MC(zc,v1+k*(d-1),k); else fillv(wt,c,zc); zc+=k;);
   break;
  case CCOMMA:
  case CLEFT:
  case CRIGHT:
   e-=e&&neg; RE(d=mult(m*c,e));
   GA(z,wt,d,id==CCOMMA?2:1+r,s-1); zc=CAV(z); fillv(wt,d,zc);
   zs=AS(z); zs[0]=m; zs[1]=id==CCOMMA?e*c:e; ke=k*e;
   EACHCUT(MC(zc,v1,d*k);  zc+=ke;);
   break;
  case CBOX:
   GA(z,m?BOX:B01,m,1,0); za=AAV(z);
   EACHCUT(GA(y,wt,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k); *za++=y;);
   break;
  case CAT: case CATCO: case CAMP: case CAMPCO:
   if(CBOX==ID(vf->f)&&(id1=ID(vf->g),id1==CBEHEAD||id1==CCTAIL)){
    GA(z,m?BOX:B01,m,1,0); za=AAV(z);
    EACHCUT(d=d?d-1:0; GA(y,wt,d*c,r,s); *AS(y)=d; MC(AV(y),id1==CBEHEAD?v1+k:v1,d*k); *za++=y;);
   }
   break;
  case CSLASH:
   vains(vaid(vf->f),wt,&ado,&cv);  // qualify the operation, returning action routine and conversion info
   if(ado){C*z0=0,*zc;I t,zk,zt;  // if the operation is a primitive that we can  apply / to...
    zt=rtype(cv);
    GA(z,zt,m*c,r,s); *AS(z)=m; 
    if(!AN(z))R z;
    zc=CAV(z); zk=c*bp(zt);
    if((t=atype(cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wv=CAV(w);}
    EACHCUT(if(d)ado(jt,1L,d*c,d,zc,v1); else{if(!z0){z0=idenv0(a,w,sv,zt,&y); 
        if(!z0){if(y)R y; else break;}} mvc(zk,zc,zk/c,z0);} zc+=zk;);
    if(jt->jerr)R jt->jerr>=EWOV?cut2(a,w,self):0; else R cv&VRI+VRD?cvz(cv,z):z;
  }
 }
 if(!z){B allbx=1;
  if(!m){y=reitem(zero,w); y=h?df1(y,*hv):CALL1(f1,y,fs); RESETERR; R iota(over(zero,shape(y?y:mtv)));}
  GATV(z,BOX,m,1,0); za=AAV(z);
  switch((wd?2:0)+(h?1:0)){
   case 0: EACHCUTG(MC(AV(y),v1,d*k);  RZ(y=CALL1(f1,y,fs));     ); break;
   case 1: EACHCUTG(MC(AV(y),v1,d*k);  RZ(y=df1(y,hv[(i-1)%hn]));); break;  // todo kludge avoid divide here & above
   case 2: EACHCUTG(MCREL(AV(y),v1,d); RZ(y=CALL1(f1,y,fs));     ); break;
   case 3: EACHCUTG(MCREL(AV(y),v1,d); RZ(y=df1(y,hv[(i-1)%hn])););
  }
  if(!allbx)RZ(z=ope(z));
 }
 EPILOG(z);
}    /* f;.1  f;._1  f;.2  f;._2  monad and dyad */
#endif
static DF1(jtcut1){R cut2(mark,w,self);}


#define PSCASE(id,zt,wt)    ((id)+256*(zt)+1024*(wt))
#define PSLOOP(Tz,Tw,F,v0)      \
    {C*u;Tw* RESTRICT wv;Tz s=v0,x,* RESTRICT zv;                   \
     GA(z,zt,n,1,0);                            \
     u=m+av; wv=m+(Tw*)AV(w); zv=m+(Tz*)AV(z);  \
     switch(pfx+2*(id==CBSLASH)){               \
      case 0: DO(n, x=*--wv; if(*--u)s=v0; *--zv=F;              ); break;  /* <@(f/\.);.2 */  \
      case 1: DO(n, x=*--wv;               *--zv=F; if(*--u)s=v0;); break;  /* <@(f/\.);.1 */  \
      case 2: DO(n, x=*wv++;               *zv++=F; if(*u++)s=v0;); break;  /* <@(f/\ );.2 */  \
      case 3: DO(n, x=*wv++; if(*u++)s=v0; *zv++=F;              ); break;  /* <@(f/\ );.1 */  \
    }}

static A jtpartfscan(J jt,A a,A w,I cv,B pfx,C id,C ie){A z=0;B*av;I m,n,zt;
 n=AN(w); m=id==CBSDOT?n:0; zt=rtype(cv); av=BAV(a);
 switch(PSCASE(ie,CTTZ(zt),CTTZ(AT(w)))){
  case PSCASE(CPLUS,   INTX,B01X):       PSLOOP(I,B,s+=x,      0   );       break;
  case PSCASE(CPLUS,   FLX, FLX ): NAN0; PSLOOP(D,D,s+=x,      0.0 ); NAN1; break;
  case PSCASE(CMAX,    INTX,INTX):       PSLOOP(I,I,s=MAX(s,x),IMIN);       break;
  case PSCASE(CMAX,    FLX, FLX ):       PSLOOP(D,D,s=MAX(s,x),-inf);       break;
  case PSCASE(CMIN,    INTX,INTX):       PSLOOP(I,I,s=MIN(s,x),IMAX);       break;
  case PSCASE(CMIN,    FLX, FLX ):       PSLOOP(D,D,s=MIN(s,x),inf );       break;
  case PSCASE(CMAX,    B01X,B01X):
  case PSCASE(CPLUSDOT,B01X,B01X):       PSLOOP(B,B,s|=x,      0   );       break;
  case PSCASE(CMIN,    B01X,B01X):
  case PSCASE(CSTARDOT,B01X,B01X):       PSLOOP(B,B,s&=x,      1   );       break;
  case PSCASE(CNE,     B01X,B01X):       PSLOOP(B,B,s^=x,      0   );       break;
  case PSCASE(CEQ,     B01X,B01X):       PSLOOP(B,B,s=s==x,    1   );       break;
 }
 R z;
}    /* [: ; <@(ie/\);.k  on vector w */

// ;@((<@(f/\));._2 _1 1 2) when  f is atomic   also @: but only when no rank loop required
// NOTE: if there are no cuts, this routine produces different results from the normal routine if the operation is one we recognise.
//  This routine produces an extra axis, as if the shape of the boxed result were preserved even when there are no boxed results
DF2(jtrazecut2){A fs,gs,y,z=0;B b,neg,pfx;C id,sep,*u,*v,*wv,*zv;I d,k,m=0,wi,p,q,r,*s,wt;
    V *sv,*vv;VA2 adocv;
 RZ(a&&w);
 sv=FAV(self); gs=CFORK==sv->id?sv->h:sv->g; vv=VAV(gs); y=vv->f; fs=VAV(y)->g;  // self is ;@:(<@(f/\);.1)     gs  gs is <@(f/\);.1   y is <@(f/\)  fs is   f/\  ...
 p=wi=IC(w); wt=AT(w); k=*AV(vv->g); neg=0>k; pfx=k==1||k==-1; b=neg&&pfx;
 id=FAV(fs)->id;  // fs is f/id   where id is \ \.
// obsolete  if((id==CBSLASH||id==CBSDOT)&&(vv=VAV(fv->f),CSLASH==vv->id)){
  // if f is atomic/\ or atomic /\., set ado and cv with info for the operation
 if(id==CBSLASH)adocv = vapfx(FAV(FAV(fs)->f)->f,wt);   // FAV(fs)->f is f/    FAV(FAV(fs)->f)->f is f
 else           adocv = vasfx(FAV(FAV(fs)->f)->f,wt); 
// obsolete }
 if(SPARSE&AT(w)||!adocv.f)R jtspecialatoprestart(jt,a,w,self);  // if sparse w or nonatomic function, do it the long way
 if(a!=mark){   // dyadic case
  if(!(AN(a)&&1==AR(a)&&AT(a)&B01+SB01))R jtspecialatoprestart(jt,a,w,self);  // if a is not nonempty boolean list, do it the long way.  This handles ;@: when a has rank>1
  if(AT(a)&SB01)RZ(a=cvt(B01,a));
  v=CAV(a); sep=C1;
 }else if(1>=AR(w)&&wt&IS1BYTE){a=w; v=CAV(a); sep=v[pfx?0:wi-1];}  // monad.  Create char list of frets
 else{RZ(a=wi?eps(w,take(num[pfx?1:-1],w)):mtv); v=CAV(a); sep=C1;}
 // v-> byte list of frets, sep is the fret char
 ASSERT(wi==IC(a),EVLENGTH);
 r=MAX(1,AR(w)); s=AS(w); wv=CAV(w); d=aii(w); k=d*bp(wt);
 if(pfx){u=v+wi; while(u>v&&sep!=*v)++v; p=u-v;}
// obsolete  if(ado){
 I t,zk,zt;                     /* atomic function f/\ or f/\. */
 if((t=atype(adocv.cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wv=CAV(w);}
 zt=rtype(adocv.cv); zk=d*bp(zt);
 if(1==r&&!neg&&B01&AT(a)&&p==wi&&v[pfx?0:wi-1]){RE(z=partfscan(a,w,adocv.cv,pfx,id,vaid(VAV(fs)->f))); if(z)R z;}
 GA(z,zt,AN(w),r,s); zv=CAV(z);
 while(p){I n;
  if(u=memchr(v+pfx,sep,p-pfx))u+=!pfx; else{if(!pfx)break; u=v+p;}
  q=u-v;
  if(n=q-neg){
// obsolete    adocv.f(jt,1L,c*d,d,zv,wv+k*(b+n-p));
   adocv.f(jt,1L,d,n,zv,wv+k*(b+wi-p));
   if(jt->jerr)R jt->jerr>=EWOV?razecut2(a,w,self):0;  // if overflow, restart the whole thing with conversion to float
   m+=n; zv+=n*zk; 
  }
  p-=q; v=u;  
 }
 *AS(z)=m; AN(z)=m*d; R adocv.cv&VRI+VRD?cvz(adocv.cv,z):z;
#if 0 // obsolete.  It handled other BOXATOPs, but we do that better in result.h now
 }else{B b1=0;I old,wc=c,yk,ym,yr,*ys,yt;   /* general f */
  RZ(x=gah(r,w)); ICPY(AS(x),s,r);  // allocate a header for the cell
  while(p){
   if(u=memchr(v+pfx,sep,p-pfx))u+=!pfx; else{if(!pfx)break; u=v+p;}
   q=u-v; d=q-neg;
   *AS(x)=d; AN(x)=wc*d; AK(x)=(wv+k*(b+n-p))-(C*)x;  // point the header to the cell
   old=jt->tnextpushx;
   RZ(y=df1(x,fs)); ym=IC(y);
   if(!z){yt=AT(y); yr=AR(y); ys=AS(y); c=aii(y); yk=c*bp(yt); GA(z,yt,n*c,MAX(1,yr),ys); *AS(z)=n; zv=CAV(z);}  // allocate result first time
   if(!(TYPESEQ(yt,AT(y))&&yr==AR(y)&&(1>=yr||!ICMP(1+AS(y),1+ys,yr-1)))){z=0; break;}  // if no change of result item-shape, continue
   while(IC(z)<=m+ym){RZ(z=ext(0,z)); zv=CAV(z); b1=0;}  // extend result if needed
   MC(zv+m*yk,CAV(y),ym*yk);    // copy in the result
   if(b1&&!(yt&DIRECT))y=gc(y,old);  // free up memory, except for the actual result y  kludge why spare y?
   b1=1; m+=ym; p-=q; v=u;
  }
  if(!b1&&ie)GA(z,wt,AN(w),r,s);
 }
 // if z is nonzero, it has the result.  Otherwise failover to the slow way
 if(z){*AS(z)=m; AN(z)=m*c; R cv&VRI+VRD?cvz(cv,z):z;}
// obsolete  else R raze(cut2(B01&AT(a)?a:eq(scc(sep),a),w,gs));
 else R jtspecialatoprestart(jt,a,w,self);
#endif
}   

DF1(jtrazecut1){R razecut2(mark,w,self);}

#if 0 // obsolete
static F2(jttesa){A x;I*av,c,d,k,p=IMAX,r,*s,t,*u,*v;
 RZ(a&&w);
 t=AT(a);
 RZ(a=vib(a)); 
 r=AR(a); s=AS(a); c=r?s[r-1]:1; av=AV(a); d=AR(w);  // r = #rows of s; c=#axes specd in x, av->data; d=rank of w
 ASSERT(d>=c&&(2>r||2==*s),EVLENGTH);  // x must not be bigger than called for by rank of w, and must be a list of 2-item table
 if(2<=r)DO(c, ASSERT(0<=av[i],EVDOMAIN););
 if(2==r&&c==d&&t&INT){RETF(a);}  // if all axes covered with start/stride, return a as is
 GATV(x,INT,2*d,2,0); s=AS(x); s[0]=2; s[1]=d;  // allocate space for start/stride
 u=AV(x); v=u+d; s=AS(w);
 if(2==r)DO(c,   *u++=av[i]; k=av[i+c]; *v++=k==p?s[i]:k==-p?-s[i]:k;);
 if(2> r)DO(c,   *u++=1;     k=av[i];   *v++=k==p?s[i]:k==-p?-s[i]:k;);
 s+=c;   DO(d-c, *u++=0; *v++=*s++;);
 RETF(x);
}    /* tesselation standardized left argument */
#endif
// if pv given, it is the place to put the shapes of the top 2 result axes.  If omitted, do them all and return an A block for them
// if pv is given, set pv[0] to 0 if there is a 0 anywhere in the result frame
// we look at all the axes even if we don't store them all; if any are 0 we set
static A jttesos(J jt,A a,A w,I n, I *pv){A p;I*av,c,axisct,k,m,s,*ws;
 RZ(a&&w);
 axisct=c=AS(a)[1]; av=AV(a); ws=AS(w);
 if(pv){c=(c>2)?2:c; p=0; // if more than 2 axes requested, limit the return to that
 }else{GATV(p,INT,c,1,0); pv=AV(p); AS(p)[0]=c;}  // all requested, make an A block for it
 if(n>0)DO(axisct, m=av[i]; s=ws[i]; if(!((I)p|(m = m?(s+m-1)/m:1&&s)))pv[0]=0;; if(i<c)pv[i]=m;)
 else   DO(axisct, m=av[i]; k=av[axisct+i]; s=ws[i]-ABS(k); if(!((I)p|(m = 0>s?0:m?(k||s%m)+s/m:1)))pv[0]=0; if(i<c)pv[i]=m;);
 R p;
}    /* tesselation result outer shape */

#if 0
static A jttesmatu(J jt,A a,A w,A self,A p,B e){DECLF;A x,y,z,z0;C*u,*v,*v0,*wv,*yv,*zv;
     I*av,i,k,m,mc,mi,mj,mr,nc,nr,old,*pv,r,s,*s1,sc,sj,sr,t,tc,tr,*ws,yc,yr,zk,zn,zr,*zs,zt;
 ws=AS(w); t=AT(w); k=bp(t); r=k*ws[1];
 av=AV(a); pv=AV(p); wv=CAV(w);
 nr=pv[0]; sr=av[2]; mr=av[0]; mi=r*mr; tr=ws[0];
 nc=pv[1]; sc=av[3]; mc=av[1]; mj=k*mc; sj=k*sc;
 if(!(nr&&nc&&nr>=sr&&nc>=sc))R A0;  // abort if can't use this code
 GA(y,t,sr*sc,2,2+av); yv=CAV(y);
 u=yv; v=wv; DO(sr, MC(u,v,sj); u+=sj; v+=r;); 
 RZ(z0=CALL1(f1,y,fs)); zt=AT(z0);   // execute on first cell
 if(!(zt&DIRECT))R A0;  // if result not DIRECT, abort, use general code
 zn=AN(z0); zr=AR(z0); zs=AS(z0); zk=zn*bp(zt); m=zr*SZI;
 GA(z,zt,zn*nr*nc,2+zr,0); s1=AS(z); ICPY(s1,pv,2); ICPY(2+s1,zs,zr); zv=CAV(z);
 old=jt->tnextpushx;
 if(e) for(i=0;i<nr;++i){  /* f;._3 */
  v=v0=wv+i*mi;
  DO(nc, 
      u=yv; DO(sr, MC(u,v,sj); u+=sj; v+=r;); v=v0+=mj; RZ(x=CALL1(f1,y,fs));
      if(!(TYPESEQ(zt,AT(x))&&zr==AR(x)&&!(m&&memcmp(zs,AS(x),m))))R A0; MC(zv,AV(x),zk); zv+=zk; tpop(old););  // abort if result mismatch
 }else for(i=0;i<nr;++i){  /* f;. 3 */
  v=v0=wv+i*mi; yr=MIN(tr,sr); tr-=mr; tc=ws[1];
  DO(nc, yc=MIN(tc,sc); tc-=mc; s=yc*k; 
      u=yv; DO(yr, MC(u,v,s ); u+=sj; v+=r;); v=v0+=mj; RZ(x=CALL1(f1,yr<sr||yc<sc?take(v2(yr,yc),y):y,fs));
      if(!(TYPESEQ(zt,AT(x))&&zr==AR(x)&&!(m&&memcmp(zs,AS(x),m))))R A0; MC(zv,AV(x),zk); zv+=zk; tpop(old););  // abort if result mismatch
 }
 R z;
}    /* f;._3 (1=e) or f;.3 (0=e), matrix w, positive size, hopefully uniform direct f */

static A jttesmat(J jt,A a,A w,A self,A p,B e){DECLF;A y,z,*zv,zz=0;C*u,*v,*v0,*wv,*yv;
     I*av,i,j,k,mc,mi,mj,mr,nc,nr,*pv,r,s,sc,sj,sr,t,tc,tr,*ws,yc,yr;
 ws=AS(w); t=AT(w); k=bp(t); r=k*ws[1];
 av=AV(a); pv=AV(p); wv=CAV(w);
 nr=pv[0]; sr=av[2]; mr=av[0]; mi=r*mr; tr=ws[0];
 nc=pv[1]; sc=av[3]; mc=av[1]; mj=k*mc; sj=k*sc;
 GA(y,t,sr*sc,2,2+av); yv=CAV(y);  // allocate area to hold item
 GATV(z,BOX,nr*nc,2,pv); zv=AAV(z);  // allocate result area
 for(i=0;i<nr;++i){
  v=v0=wv+i*mi; yr=MIN(tr,sr); tr-=mr; tc=ws[1];
  for(j=0;j<nc;++j){
   yc=MIN(tc,sc); tc-=mc; s=yc*k; 
   if(WASINCORP1(zz,y)){GA(y,t,sr*sc,2,2+av); yv=CAV(y);}  // reallo y if in use, or if returned from ][
   u=yv; DO(yr, MC(u,v,e?sj:s); u+=sj; v+=r;); v=v0+=mj;   // copy in the next input item
   *zv++=zz=rifvs(CALL1(f1,e||yr==sr&&yc==sc?y:take(v2(yr,yc),y),fs));
  }
 }
 RE(0); R ope(z);
}    /* f;._3 (1=e) or f;.3 (0=e), matrix w, positive size */

static DF2(jttess2){A gs,p,y,z;I*av,n,t;
 PREF2(jttess2);
 RZ(a=tesa(a,w));   // expand x to canonical form
 av=AV(a); gs=VAV(self)->g; n=*AV(gs);
 RZ(p=tesos(a,w,n));  // outer shape of overall result, from frames
 if(DENSE&AT(w)&&2==AR(w)&&0<=av[2]&&0<=av[3]){  // matrix with nonreversed blocks
  RE(z=tesmatu(a,w,self,p,(B)(0>n)));   // try uniform-result code first
  if(!z)z=tesmat(a,w,self,p,(B)(0>n));  // try general  matrix code
  if(z&&!AN(z)){  // if result found, but empty, make result an empty with the type of the result of execution on the entire w
   y=df1(w,VAV(self)->f); RESETERR;
   t=y?AT(y):B01;
   if(TYPESNE(t,AT(z)))GA(z,t,0L,AR(z),AS(z));
  }
  RETF(z);
 }
 R cut02(irs2(cant1(tymes(head(a),cant1(abase2(p,iota(p))))), tail(a),0L,1L,1L,jtlamin2),w,self);
}
#else // WIP


static F2(jttesa){A x;I*av,ac,c,d,k,p=IMAX,r,*s,t,*u,*v;
 RZ(a&&w);
 RZ(a=vib(a));    // convert a to integer
 t=AT(a);
 r=AR(a); s=AS(a); ac=c=r?s[r-1]:1; av=AV(a); d=AR(w);  // r = rank of x; s->shape of x; c=#axes specd in x, av->data; d=rank of w
 ASSERT(d>=c&&(2>r||2==s[0]),EVLENGTH);  // x must not be bigger than called for by rank of w, and must be a list or 2-item table
 if(2<=r)DO(c, ASSERT(0<=av[i],EVDOMAIN););  // if movement vector given, it must be nonnegative
 if(2==r&&t&INT){RETF(a);}  // if we can use a as given, return a as is
 GATV(x,INT,2*c,2,0); s=AS(x); s[0]=2; s[1]=c;  // allocate space for start/stride, only for axes that will be modified.  We will modify it
 u=AV(x); v=u+c; s=AS(w);
 if(2==r)DO(c,   *u++=av[i]; k=av[i+ac]; *v++=k==p?s[i]:k==-p?-s[i]:k;);
 if(2> r)DO(c,   *u++=1;     k=av[i];   *v++=k==p?s[i]:k==-p?-s[i]:k;);
 RETF(x);
}    /* tesselation standardized left argument */

#define STATEREFLECTX 0x400
#define STATEREFLECTY 0x800
#define STATETAKE 0x1000
static DF2(jttess2){A z,zz=0,virtw,strip;I n,rs[3],cellatoms,cellbytes,vmv,hmv,vsz,hsz,hss,hds,vss1,vss,vds1,vds,vlrc,vtrc,lrchsiz,hi,vi,vkeep1,vtrunc,hkeep1,htrunc;C *svh,*dvh;
 PROLOG(600); PREF2(jttess2);   // enforce left rank 2
#define ZZFLAGWORD state
 I state;
 RZ(a=tesa(a,w));   // expand x to canonical form, with trailing axes-in-full deleted
 A gs=FAV(self)->g; n=IAV(gs)[0]; state=(~n)&STATETAKE;  // set TAKE bit if code=3: we will shorten out-of-bounds args
 I wr=AR(w); I wt=AT(w); // rank of w, type of w
 I *as=AS(a), *av=IAV(a), axisct=as[1];  // a-> shape of a, axisct=# axes in a, av->mv/size area
 // get shape of final result
 tesos(a,w,n,rs);      // vert/horiz shape of 1st 2 axes of result, from argument sizes and cut type
 I nregcells; PROD(nregcells,axisct,av+axisct);  // get size of a region, not including taken-in-full axes.  Must be no bigger than # atoms in w
 if((-(wt&DENSE)&(-wr)&(-AN(w))&(-nregcells)&(-AN(a))&(-rs[0]))>=0){A p;  // if any of those is 0...
  // w is sparse, atomic, or empty, or the region has no axes or is empty, or the result is empty.  Go the slow way: create a selector block for each cell, and then apply u;.0 to each cell
  // trailing axes taken in full will be omitted from the shape of the result
  RZ(p=tesos(a,w,n,0));  // recalculate all the result shapes
  RETF(cut02(irs2(cant1(tymes(head(a),cant1(abase2(p,iota(p))))), tail(a),0L,1L,1L,jtlamin2),w,self));  // ((|: ({.a) * |: (#: i.)p) ,:"1 ({:a)) u;.0 w
 }
 DECLF;  // get the function pointers
 I *ws=AS(w);  // ws-> shape of w
 // get address of end+1 of the source data, so we can avoid out-of-bounds
 C *sdataend=CAV(w)+AN(w)*bp(wt);  // addr+1 of source data
 I inrecursion=(n+256)&512;   // 512 if bit 8 and 9 of n have different values, otherwise 0
 if(axisct>2) {A next2;  // we build the u for the next 2 axes here
  // We do two axes of tessellation at a time.  If there are more than two axes, replace the verb with a verb that will reapply
  // the tessellation lacking the first two axes.  Unfortunately this differs considerably between ;.3 and ;._3
  // The ;._3 cut is pretty efficient, moving the data only once per pair of axes added.  The ;.3 cut creates argument cells, which
  // is inefficient; but that case will be very rare
  next2=qq(amp(drop(v2(0,2),a),cut(ds((((4-axisct)|n)<0)?CRIGHT:CBOX),sc(n^256^(inrecursion>>1)))),num[-2]);  // RIGHT if n<0 or axisct>4
  if(n<0){
   // ;._3, the more usual and faster case
   // we will recur on ((0 2}.x)&(];.n)"_2 to build up _2-cells of the final result.  To save a smidgen, we will suppress the final
   // transpose during the recursion by using a large value for n; we will undo all the transposes at once during the last return
   // build the recursion down 2 levels: (0 2}.x)&(];.n)"_2
   // If this is the top level of recursion, prefix it with (original cut)"wr@:(??...&|:)@:
   if(!inrecursion){
    // The shape returned from ];.n is a mixture of subarray shapes Sn and result shapes Rn.  The first two result-shape values R0 and R1
    // are implied, as they are encoded in the position of the current cell.  The other shapes are interleaved:
    // 3 axes: S0 S1 R2 S2 [rest of ws beyond the axes]
    // 4 axes: S0 S1 R3 R2 S2 S3 [rest of ws beyond the axes]
    // 5 axes: S0 S1 R3 R2 S2 S3 R4 S4 [rest of ws beyond the axes]
    // these must be transposed to result in R2..Rn S0..Sn Wx..Wn
    A xposearg; fauxblockINT(xfaux,5,1); fauxINT(xposearg,xfaux,wr+axisct-2,1) /* obsolete GATV(xposearg,INT,wr+axisct-2,1,0); AS(xposearg)[0]=wr+axisct-2; */ I *xpv=IAV(xposearg);
    DO((axisct>>1)-1, *xpv++ = 4*i+3; *xpv++=4*i+2;) if(axisct&1)*xpv++=2*axisct-4;  // Rn
    DO(axisct>>1, *xpv++ = 4*i+0; *xpv++=4*i+1;) if(axisct&1)*xpv++=2*axisct-3;  // Sn
    DO(wr-axisct, *xpv++=2*axisct+i-2;);  // Wn, all the rest
    next2=atco(atco(qq(fs,sc(wr)),amp(xposearg,ds(CCANT))),next2);  // combine it all
   }
  }else{
   // ;.3.  The cells coming out of the lower tessellations may have dissimilar shape so we have to box them
   // If the next level is the last, it becomes (0 2}.x)&(<;.n)"_2  - adding one boxing level.  Otherwise use ] instead of < above
   next2=qq(amp(drop(v2(0,2),a),cut(ds(CBOX),sc(n^256^(inrecursion>>1)))),num[-2]);
   // collect the components that contribute to a single input-to-u, one box for each : (<@:>"2)@:(0 1&|:)@:next2
   next2=atco(atco(qq(atco(ds(CBOX),ds(COPE)),num[2]),amp(v2(0,1),ds(CCANT))),next2);
   if(!inrecursion){
    // at the top level, add on u@>
    next2=atco(atop(fs,ds(COPE)),next2);
   }
  }
  fs=next2; f1=FAV(fs)->f1;   // get the function corresponding to the new verb
 }
 // Now that we really know what fs is, see if it is a BOXATOP form
 state |= ((FAV(fs)->mr>=wr?VF2BOXATOP1:0)&FAV(fs)->flag2)>>(VF2BOXATOP1X-ZZFLAGBOXATOPX);  // If this is BOXATOP and it will generate a single box, set so for result loop.  Don't touch fs yet, since we might not loop
 state &= ~((FAV(fs)->flag2&VF2ATOPOPEN1)>>(VF2ATOPOPEN1X-ZZFLAGBOXATOPX));  // We don't handle &.> here; ignore it
 // We don't honor WILLBEOPENED; perhaps we should

 // Start preparing for loop
 I axisproc=axisct; axisproc=axisct>2?2:axisproc;  // number of axes to process here: 1 or 2
 vmv=av[0]; vsz=av[axisct+0];  // v movement vector and size
 PROD(cellatoms,wr-axisproc,ws+axisproc); cellbytes = cellatoms*bp(wt);  // get # atoms in cell, then #bytes
 // Figure out the size of the cells we will be making blocks of.  They have all but the first 1 or 2 axes of a/w
 fauxblock(virtwfaux);
 if(axisproc==1){
  // Here there is just one axis.  We will never have to move anything.
  // Set up the variables for the 2-D loop, most of which we don't need
  vds=cellbytes*vmv;  // the amount by which we advance the virtual-block pointer: 1 row 
  rs[1]=1;  // one horizontal iteration
  hsz=1;  // treat horiz size as one cell
  htrunc=0; hkeep1=hsz; hmv=0;  // no truncation owing to horizontal position
  vtrc=vlrc=0;  // stifle all copying to the strip, which doesn't exist
  hss=hds=lrchsiz=0; vds1=vss1=0; vss=0; svh=dvh=0; // no horiz looping, so this is immaterial
  vss=0;   // since we never loop back in horiz loop, this is immaterial

// obsolete   RZ(virtw=virtual(w,0,wr));
  fauxvirtual(virtw,virtwfaux,w,wr,ACUC1);  // allocate the virtual block, pointing into w
 }else{
  // Here there are at least 2 axes.  We will process the first 2, carefully laying the cells into memory to require minimal copying of data
  // We will advance along the leading axis, building up a tall narrow strip of cells.  When we advance along the second axis, we will be able to
  // reuse the overlapping parts of the strip.

  hmv=av[1];  hsz=av[axisct+1];    // h movement vector and size
  state|=(vsz>>(BW-1))&STATEREFLECTY; state|=(hsz>>(BW-1))&STATEREFLECTX; 
  vsz=(vsz<0)?-vsz:vsz; hsz=(hsz<0)?-hsz:hsz;  // take abs(size)
  // Calculate the amount to add to the strip position when we move along the second axis.  If there is no overlap in the second axis, just move 0.
  hss = hmv*cellbytes; I hdsc=(hmv>=hsz)?0:hmv;  // horiz source stride, dest stride in cells.  A mv of 0 must have the same effect as an mv of infinity
  // Calculate the amount to add to the strip position when we move along the leading axis.  If there is no overlap on either axis, move 0.  If horiz overlap only, move vsize.  If vert overlap, move vert mv
  vss1 = cellbytes*ws[1]; vss=vss1*vmv; vds1=cellbytes*hsz; I vdsc=(hdsc==0)?0:vsz; vdsc=(((vmv-1)|(vsz-vmv-1))<0)?vdsc:vmv;   // if vmv<=0 || vsz-vmv<=0
                           // vert source stride to move 1 row, stride betw cells, dest stride 1 row (in bytes), 1 result cell (in dest rows)
  // number of rows to move to dest when going down 1 cell
  vlrc=vmv; vlrc=(vlrc>vsz)?vsz:vlrc;  // vert pos of lower-right corner.
  lrchsiz=hmv; lrchsiz=(hdsc==0)?hsz:lrchsiz;   // length uncovered by a move, but size if that is 0 (meaning no move & thus no left side)
  vds = vdsc*vds1;  hds=hdsc*cellbytes; lrchsiz*=cellbytes; // make into byte counts
  // figure out how many (if any) of the cells for each axis are going to be truncated; and what the size of the first truncated block will be
  if(state&STATETAKE){I space, nfit;  // 3-cut, which may have shards - calcuate horizontal shard info
   // space after first; divide by mv; count how many fit including the first; get vi when we hit truncation; see how many items are valid in 1st truncated cell 
   space=ws[1]-hsz; nfit=hmv?(space/hmv):0; nfit=(space<0)?0:nfit+1; htrunc=rs[1]-nfit; hkeep1=ws[1]-nfit*hmv;
  }
  // see how many atoms are needed in the strip.  There needs to be enough for one top, plus all the bottoms, plus destination stride for each horizontal move
  I stripn = (hsz*(vsz-vlrc+vlrc*rs[0])+hdsc*rs[1])*cellatoms;  // total number of atoms in the strip
  // Allocate the area we will use for the strip
  GA(strip,wt,stripn,0,0);  // allocate strip - rank immaterial
  // Allocate the virtual block we will use to address subarrays
// obsolete   RZ(virtw=virtual(strip,0,wr)); AFLAG(virtw)|=AFUNINCORPABLE;  // indicate that this is a moving virtual block and cannot EVER be incorporated
  fauxvirtual(virtw,virtwfaux,strip,wr,ACUC1);
  // Move in the left side of the first strip (the left edge of first column)
  svh=CAV(w); dvh=CAV(strip);   // ?vh=pointer to top-left of first column
  C *svb=svh; C *dvb=dvh;  // running pointers used to fill the strip
  // Start with the first cell, above the bottom part.  Copy the left-hand side (length vds1-lrchsiz).
  vtrc=vsz-vlrc;  // number of lines to copy in top half
  DO(vtrc, I sh=sdataend-(svb+vds1-lrchsiz); sh=(sh>=0)?0:sh; sh+=vds1-lrchsiz; sh=(sh<0)?0:sh; MC(dvb,svb,sh); svb+=vss1; dvb+=vds1;);  // copies to dest are all sequential; sequential rows of src
  // copy the bottom-left part of the first cell, and of all cells if the cells overlap (i. e. vds!=0) (length vds1-lrchsiz)
  DO(vds?rs[0]:1, C *svb1=svb; DO(vlrc, I sh=sdataend-(svb1+vds1-lrchsiz); sh=(sh>=0)?0:sh; sh+=vds1-lrchsiz; sh=(sh<0)?0:sh; MC(dvb,svb1,sh); svb1+=vss1; dvb+=vds1;); svb+=vss;);  // sequential to dest; hop to cells of src
  // advance the horiz pointers, which now point to the top-left of the top cell, to the top-left of the top-right area of that cell
  svh+=(vds1-lrchsiz); dvh+=(vds1-lrchsiz);
 }
 // The rest is the same for 1- and 2-axis cut
 I hvirtofst=AK(virtw); I * RESTRICT virtws=AS(virtw);  // save initial offset (to top-left of source)
 // Install shape/item count into the virtual array.  First install height; then width if given; then the rest of the shape of w
 *virtws++=vsz; if(axisproc>1){*virtws++=hsz;} MCISd(virtws,ws+axisproc,wr-axisproc) /* obsolete  DO(wr-axisproc, *virtws++=ws[axisproc+i];); */ AN(virtw) = vsz*hsz*cellatoms;
 // calculate vertical shard info & see if there are any shards
 if(state&STATETAKE){I space, nfit;  // 3-cut, which may have shards
  // space after first; divide by mv; count how many fit including the first; get vi when we hit truncation; see how many items are valid in 1st truncated cell 
  space=ws[0]-vsz; nfit=vmv?(space/vmv):0; nfit=(space<0)?0:nfit+1; vtrunc=rs[0]-nfit; vkeep1=ws[0]-nfit*vmv;
  if(vtrunc<=0&&htrunc<=0)state &= ~STATETAKE;  // if we don't have any shards after all, turn off looking for them
 }
 // for each horizontal position, move in the right edge and process

#define ZZDECL
#include "result.h"
 I mn=rs[0]*rs[1];  // number of cells in the result
// obsolete  rs[2]=rs[0]; ZZPARMS(0,0,rs+axisproc-1,axisproc,mn,1)  // Note the 2-axis result is transposed, so shape is reversed here for that case
  ZZPARMS(axisproc,mn,1)
#define ZZINSTALLFRAME(optr) *optr++=rs[axisproc-1];if(axisproc>1)*optr++=rs[0];  // Note the 2-axis result is transposed, so shape is reversed here for that case

 for(hi=rs[1];hi;--hi){C *svv, *dvv;  // pointers within the lower-right corner, starting at the top-left of the corner.  Before that we use them to advance through top-right
  // move in the top-right part of top cell, stopping before the lower-right corner.
  svv=svh; dvv=dvh; // init to top-left of right edge (length=lcrhsiz)
  DO(vtrc, I sh=sdataend-(svv+lrchsiz); sh=(sh>=0)?0:sh; sh+=lrchsiz; sh=(sh<0)?0:sh; MC(dvv,svv,sh); svv+=vss1; dvv+=vds1;);
  // now ?vv points to top-left of lower-right quadrant of top cell
  // point the virtual-block pointer to the top of the row
  AK(virtw)=hvirtofst;
  // move horiz calc outside loop
  I hkeep=hkeep1-(htrunc-hi)*hmv;  // horiz length to keep
  // for each cell in the vertical strip, move in the lower-right corner and execute
  for(vi=rs[0];vi;--vi){A opcell=virtw;
   // move in the lower-right corner.  Avoid overrun, and detect if there was overrun.  Watch out for addresses wrapping around the end of memory (length=lcrhsiz)
   C *dvlrc=dvv, *svlrc=svv; DO(vlrc, I sh=sdataend-(svlrc+lrchsiz); sh=(sh>=0)?0:sh; sh+=lrchsiz; sh=(sh<0)?0:sh; MC(dvlrc,svlrc,sh); svlrc+=vss1; dvlrc+=vds1;);
   // If a copy was truncated, take only the valid elements.
   if(state&(STATEREFLECTX|STATEREFLECTY|STATETAKE)){  // something might be truncated/reflected
    I vkeep=vkeep1-(vtrunc-vi)*vmv; vkeep=(vkeep>vsz)?vsz:vkeep; // vertical length to keep
    if(((vkeep-vsz)|(hkeep-hsz))<0){   // if either axis must be shortened...
     RZ(opcell=take((hkeep-hsz)>=0?sc(vkeep):v2(vkeep,hkeep),virtw));
    }
    if(state&STATEREFLECTY)RZ(opcell=reverse(opcell));  // reverse vertical
    if(state&STATEREFLECTX)RZ(opcell=df1(opcell,qq(ds(CREV),num[-1])));  // reverse horizontal
   }
   // execute the verb on the cell and assemble results
   RZ(z=CALL1(f1,opcell,fs)); 

#define ZZBODY  // assemble results
#include "result.h"

   // advance to next vertical (i. e. first-axis) position
   AK(virtw)+=vds; svv+=vss; dvv+=vds;
  }
  // advance to next horizontal position
  svh+=hss; dvh+=hds; hvirtofst+=hds;
 }

#define ZZEXIT
#include "result.h"

// if we took 2 axes, they are transposed in the result.  reverse the transpose. - unless this is a recursive call and we are deferring the transpose until the top level because n is negative
 if(!(((axisproc<<9)|(inrecursion&n))&512)){A xposeaxes;  // axisproc=1, or bit 9 of n mismatches bit 8 (with n>0), is enough to turn off transpose
  RZ(xposeaxes=IX(AR(zz))); realizeifvirtual(xposeaxes); IAV(xposeaxes)[0]=1;  IAV(xposeaxes)[1]=0;  // xpose arg, 1 0 2 3 4...
  RZ(zz=cant2(xposeaxes,zz));
 }

 AFLAG(zz)|=AFNOSMREL;  // obsolete.  We used to check state
 EPILOG(zz);

}
#endif

static DF1(jttess1){A s;I m,r,*v;
 RZ(w);
 r=AR(w); RZ(s=shape(w)); rifvs(s); v=AV(s);
 m=IMAX; DO(r, if(m>v[i])m=v[i];); DO(r, v[i]=m;);
 R tess2(s,w,self);
}


F2(jtcut){A h=0;I flag=0,k;
// NOTE: u/. is processed using the code for u;.1 and passing the self for /. into the cut verb.  So, the self produced
// by /. and ;.1 must be the same as far as flags etc.  For the shared case, inplacing is OK
 RZ(a&&w);
 ASSERT(NOUN&AT(w),EVDOMAIN);
 RZ(w=vi(w));
 RE(k=i0(w));
 if(NOUN&AT(a)){flag=VGERL; RZ(h=fxeachv(1L,a)); ASSERT(-2<=k&&k<=2,EVNONCE);}
 switch(k){
  case 0:          R fdef(0,CCUT,VERB, jtcut01,jtcut02, a,w,h, flag, RMAX,2L,RMAX);
  case 1: case -1:
  case 2: case -2: if(!(NOUN&AT(a)))flag=(FAV(a)->flag&VINPLACEOK1)*((VINPLACEOK2+VINPLACEOK1)/VINPLACEOK1); R fdef(0,CCUT,VERB, jtcut1, jtcut2,  a,w,h, flag, RMAX,1L,RMAX);
  case 3: case -3: case 259: case -259: R fdef(0,CCUT,VERB, jttess1,jttess2, a,w,h, flag, RMAX,2L,RMAX);
  default:         ASSERT(0,EVDOMAIN);
}}
