/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Catenate and Friends                                             */

#include "j.h"


static A jtovs0(J jt,B p,I r,A a,A w){A a1,e,q,x,y,z;B*b;I at,*av,c,d,j,k,f,m,n,t,*v,wr,*ws,wt,zr;P*wp,*zp;
 ws=AS(w); wr=AR(w); f=wr-r; zr=wr+!r;
 ASSERT(IMAX>ws[f],EVLIMIT);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); y=SPA(wp,i); m=*AS(y); 
 a1=SPA(wp,a); c=AN(a1); av=AV(a1); RZ(b=bfi(zr,a1,1));
 at=AT(a); wt=AT(x);
 ASSERT(HOMO(at,wt),EVDOMAIN);
 t=maxtype(at,wt);
 ASSERT(t&(B01|INT|FL|CMPX),EVDOMAIN);  // verify supported sparse type
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt)){RZ(x=cvt(t,x)); RZ(e=cvt(t,e));}
 j=k=0; DO(f, if(b[i])++j; else ++k;);
 switch(2*b[f]+!equ(a,e)){
  case 0:  /* dense and a equal e */
   RZ(y=ca(y)); 
   RZ(x=p?irs2(x,a,0L,AR(x)-(1+k),0L,jtover):irs2(a,x,0L,0L,AR(x)-(1+k),jtover)); 
   break;
  case 1:  /* dense and a not equal to e */
   GATV0(q,INT,c,1); v=AV(q); DO(c, v[i]=ws[av[i]];); RZ(q=odom(2L,c,v));
   if(AN(q)>=AN(y)){
    RZ(z=shape(x)); *AV(z)=*AS(q); 
    RZ(x=from(grade1(over(y,less(q,y))),over(x,reshape(z,e))));
    y=q;
   }
   RZ(x=p?irs2(x,a,0L,AR(x)-(1+k),0L,jtover):irs2(a,x,0L,0L,AR(x)-(1+k),jtover));
   break;
  case 2:  /* sparse and a equals e */
   RZ(y=ca(y)); 
   if(!p){v=j+AV(y); DQ(m, ++*v; v+=c;);} 
   break;
  case 3:  /* sparse and a not equal to e */
   GATV0(q,INT,c,1); v=AV(q); DO(c, v[i]=ws[av[i]];); v[j]=1; RZ(q=odom(2L,c,v)); n=*AS(q);
   if(p){RZ(y=over(y,q)); v=AV(y)+j+m*c; d=ws[f]; DQ(n, *v=d; v+=c;);}
   else {RZ(y=over(q,y)); v=AV(y)+j+n*c;          DQ(m, ++*v; v+=c;);}
   RZ(q=shape(x)); *AV(q)=n; RZ(q=reshape(q,a)); RZ(x=p?over(x,q):over(q,x));
   if(f){RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));}
 }
 GASPARSE(z,STYPE(t),1,zr,ws); 
 if(r)++*(f+AS(z)); else *(wr+AS(z))=2;
 zp=PAV(z); SPB(zp,a,caro(ifb(zr,b))); SPB(zp,e,e); SPB(zp,i,y); SPB(zp,x,x);  // avoid readonly
 R z;
}    /* a,"r w (0=p) or w,"r a (1=p) where a is scalar and w is sparse */

static F2(jtovs){A ae,ax,ay,q,we,wx,wy,x,y,z,za,ze;B*ab,*wb,*zb;I acr,ar,*as,at,c,m,n,r,t,*v,wcr,wr,*ws,wt,*zs;P*ap,*wp,*zp;
 RZ(a&&w);
 acr=jt->ranks>>RANKTX; ar=AR(a); at=AT(a); acr=ar<acr?ar:acr; 
 wcr=(RANKT)jt->ranks; wr=AR(w); wt=AT(w); wcr=wr<wcr?wr:wcr; RESETRANK; 
 if(!ar)R ovs0(0,wcr,a,w);
 if(!wr)R ovs0(1,acr,w,a);
 if(ar>acr||wr>wcr)R sprank2(a,w,0L,acr,wcr,jtover);
 r=MAX(ar,wr);
 if(r>ar)RZ(a=reshape(over(apv(r-ar,1L,0L),shape(a)),a)); as=AS(a);
 if(r>wr)RZ(w=reshape(over(apv(r-wr,1L,0L),shape(w)),w)); ws=AS(w);
 ASSERT(*as<IMAX-*ws,EVLIMIT);
 if(!(at&SPARSE)){wp=PAV(w); RZ(a=sparseit(a,SPA(wp,a),SPA(wp,e)));}
 if(!(wt&SPARSE)){ap=PAV(a); RZ(w=sparseit(w,SPA(ap,a),SPA(ap,e)));}
 ap=PAV(a); RZ(ab=bfi(r,SPA(ap,a),1)); ae=SPA(ap,e); at=AT(ae);
 wp=PAV(w); RZ(wb=bfi(r,SPA(wp,a),1)); we=SPA(wp,e); wt=AT(we);
 ASSERT(equ(ae,we),EVNONCE);
 GATV0(q,B01,r,1); zb=BAV(q); DO(r, zb[i]=ab[i]||wb[i];); RZ(za=caro(ifb(r,zb))); c=AN(za);  // avoid readonly
 GATV0(q,INT,r,1); zs= AV(q); DO(r, zs[i]=MAX(as[i],ws[i]););
 DO(r, if(zb[i]>ab[i]){RZ(a=reaxis(za,a)); break;});
 DO(r, if(zb[i]>wb[i]){RZ(w=reaxis(za,w)); break;});
 *zs=*as; DO(r, if(zs[i]>as[i]){RZ(a=take(q,a)); break;});
 *zs=*ws; DO(r, if(zs[i]>ws[i]){RZ(w=take(q,w)); break;});
 *zs=*as+*ws; t=maxtype(at,wt);
 ap=PAV(a); ay=SPA(ap,i); ax=SPA(ap,x); if(TYPESNE(t,at))RZ(ax=cvt(t,ax));
 wp=PAV(w); wy=SPA(wp,i); wx=SPA(wp,x); if(TYPESNE(t,at))RZ(wx=cvt(t,wx));
 GASPARSE(z,STYPE(t),1,r,zs); zp=PAV(z);
 SPB(zp,a,za); SPBV(zp,e,ze,ca(TYPESEQ(t,at)?ae:we));
 if(*zb){
  SPB(zp,x,  over(ax,wx));
  SPBV(zp,i,y,over(ay,wy)); v=AV(y)+AN(ay); m=*as; DQ(*AS(wy), *v+=m; v+=c;);
 }else{C*av,*wv,*xv;I am,ak,i,j,k,mn,p,*u,wk,wm,xk,*yv;
  i=j=p=0; k=bpnoun(t); 
  m=*AS(ay); u=AV(ay); av=CAV(ax); am=aii(ax); ak=k*am;
  n=*AS(wy); v=AV(wy); wv=CAV(wx); wm=aii(wx); wk=k*wm; mn=m+n; xk=k*(am+wm);
  GATVR(y,INT,mn*c,      2,     AS(ay)); yv= AV(y); AS(y)[0]=mn;                 
  GA(x,t,  mn*(am+wm),AR(ax),AS(ax)); xv=CAV(x); *AS(x)=mn; *(1+AS(x))=*zs; mvc(k*AN(x),xv,k,AV(ze));
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
   MC(x,AV(w),k*AN(w));  // copy in the data, now the right cell shape but possibly shorter than the fill  kludge could avoid double copy
  }
 }else{  // scalar replication
  mvc(k*p,x,k,AV(w));
 }
 R x+k*p;
}    /* move an argument into the result area */

static F2(jtovg){A s,z;C*x;I ar,*as,c,k,m,n,r,*sv,t,wr,*ws,zn;
 RZ(a&&w);
 I origwt=AT(w); RZ(w=setfv(a,w));
 if(AT(a)!=(t=AT(w))){t=maxtypedne(AT(a)|(AN(a)==0),t|(((t^origwt)+AN(w))==0)); t&=-t; if(!TYPESEQ(t,AT(a))){RZ(a=cvt(t,a));} else {RZ(w=cvt(t,w));}}  // convert args to compatible precisions, changing a and w if needed.  B01 if both empty.  If fill changed w, don't do B01 for it
 ar=AR(a); wr=AR(w); r=ar+wr?MAX(ar,wr):1;
 RZ(s=r?vec(INT,r,r==ar?AS(a):AS(w)):num[2]); sv=AV(s);   // Allocate list for shape of composite item
 // Calculate the shape of the result: the shape of the item, max of input shapes
 if(m=MIN(ar,wr)){
  as=ar+AS(a); ws=wr+AS(w); k=r;
  DQ(m, --as; --ws; sv[--k]=MAX(*as,*ws);); 
  DO(r-m, sv[i]=MAX(1,sv[i]););
 }
 RE(c=prod(r-1,1+sv)); m=AS(a)[0]; m=r>ar?1:m; n=AS(w)[0]; n=r>wr?1:n; // verify composite item not too big
 RE(zn=mult(c,m+n)); ASSERT(0<=m+n,EVLIMIT);
 GA(z,AT(a),zn,r,sv); AS(z)[0]=m+n; x=CAV(z); k=bpnoun(AT(a));
 RZ(x=ovgmove(k,c,m,s,a,x,z));
 RZ(x=ovgmove(k,c,n,s,w,x,z));
 RETF(z);
}    /* a,w general case for dense array with the same type; jt->ranks=~0 */

#if 0  // obsolete 
static F2(jtovv){A z;I m,t;
 t=AT(a); 
 GA(z,t,AN(a)+AN(w),1,0);  
 I klg=bplg(t); m=AN(a)<<klg; C *x=CAV(z); 
 MC(x,  AV(a),m      ); 
 MC(x+m,AV(w),AN(w)<<klg);
 RETF(z);
}    /* a,w for vectors/scalars with the same type */
#endif

// these variants copy vectors or scalars, with optional repetition of items and, for the scalars, scalar repetition
static void moveawVV(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 if((arptct|wrptct)==0) {
   // fastest case: no replication, no scalars
  while(--c>=0){
   // copy one cell from a; advance z; advance a if not repeated
   MC(zv,av,ma); zv+=ma; av+=ma;
   // repeat for w
   MC(zv,wv,mw); zv+=mw; wv+=mw;
  }
 }else{
  while(--c>=0){
   // copy one cell from a; advance z; advance a if not repeated
   MC(zv,av,ma); zv+=ma; --arptct; av+=REPSGN(arptct)&ma; arptct+=REPSGN(arptct)&arptreset;
   // repeat for w
   MC(zv,wv,mw); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&mw; wrptct+=REPSGN(wrptct)&wrptreset;
  }
 }
}
static void moveawVS(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 while(--c>=0){
  // copy one cell from a; advance z; advance a if not repeated
  MC(zv,av,ma); zv+=ma; --arptct; av+=REPSGN(arptct)&ma; arptct+=REPSGN(arptct)&arptreset;
  // repeat for w
  mvc(mw,zv,k,wv); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&k; wrptct+=REPSGN(wrptct)&wrptreset;
 }
}
static void moveawSV(C *zv,C *av,C *wv,I c,I k,I ma,I mw,I arptreset,I wrptreset){
 I arptct=arptreset-1; I wrptct=wrptreset-1;
 while(--c>=0){
  // copy one cell from a; advance z; advance a if not repeated
  mvc(ma,zv,k,av); zv+=ma; --arptct; av+=REPSGN(arptct)&k; arptct+=REPSGN(arptct)&arptreset;
  // repeat for w
  MC(zv,wv,mw); zv+=mw; --wrptct; wv+=REPSGN(wrptct)&mw; wrptct+=REPSGN(wrptct)&wrptreset;
 }
}
int (*p[4]) (int x, int y);
static void(*moveawtbl[])() = {moveawVV,moveawVS,moveawSV};
F2(jtover){A z;C*zv;I replct,framect,acr,af,ar,*as,k,ma,mw,p,q,r,t,wcr,wf,wr,*ws,zn;
 RZ(a&&w);
 UI jtr=jt->ranks;//  fetch early
 if(SPARSE&(AT(a)|AT(w))){R ovs(a,w);}  // if either arg is sparse, switch to sparse code
 if(AT(a)!=(t=AT(w))){t=maxtypedne(AT(a)|(AN(a)==0),t|(AN(w)==0)); t&=-t; if(!TYPESEQ(t,AT(a))){RZ(a=cvt(t,a));} else {RZ(w=cvt(t,w));}}  // convert args to compatible precisions, changing a and w if needed.  Treat empty arg as boolean
 ar=AR(a); wr=AR(w);
 acr=jtr>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;  // acr=rank of cell, af=len of frame, as->shape
 wcr=(RANKT)jtr; wcr=wr<wcr?wr:wcr; wf=wr-wcr;  // wcr=rank of cell, wf=len of frame, ws->shape
 // no RESETRANK - not required by ovv or main line here
// should look for no frame, identical item shape after rank extension, copying en bloc
 as=AS(a); ws=AS(w);
 if(af+wf==0){
  // No frame.  See if ranks are equal or different by 1, and if the items have the same shape
  I lr=ar;  // rank of arg with long shape
  A l=a; l=wr>ar?w:l; lr=wr>ar?wr:lr;  // arg with long shape.  Not needed till later but we usually go through the fast path
  if(2*lr-1<=ar+wr){  // if ranks differ by at most 1
   // items have the same rank or one argument is an item of the other (cases where the ranks differ by more than 1 follow the general path below)
   // see if the shapes agree up to the shape of an item of the longer argument
   I mismatch=0; I *ase=as+ar-1, *wse=ws+wr-1; DQ(lr-1, mismatch|=*ase--^*wse--;);
   if(!mismatch){
    // The data can be copied in toto, with only the number of items changing.
    A s=(A)((I)a+(I)w-(I)l);  // arg with short shape
    // The rank is the rank of the long argument, unless both arguments are atoms; then it's 1
    // The itemcount is the sum of the itemcounts; but if the ranks are different, use 1 for the shorter; and if both ranks are 0, the item count is 2
    // empty items are OK: they just have 0 length but their shape follows the normal rules
    I si=AS(s)[0]; si=ar==wr?si:1; si+=AS(l)[0]; si=lr==0?2:si; lr=lr==0?1:lr; ASSERT(si>=0,EVLIMIT);  // get short item count; adjust to 1 if lower rank; add long item count; check for overflow; adjust if atom+atom
    I klg=bplg(t); I alen=AN(a)<<klg; I wlen=AN(w)<<klg;
    GA(z,t,AN(a)+AN(w),lr,AS(l)); AS(z)[0]=si; C *x=CAV(z);  // install # items after copying shape
    MC(x,CAV(a),alen); MC(x+alen,CAV(w),wlen);
    RETF(z);
   }
  }
 }
 // dissimilar items, or there is frame.
 p=as[ar-1];   // p=len of last axis of cell.  Always safe to fetch first 
 q=ws[wr-1];   //  q=len of last axis of cell
 r=MAX(acr,wcr); r=(r==0)?1:r;  // r=cell-rank, or 1 if both atoms.
 // if max cell-rank>2, or an argument is empty, or (joining table/table or table/row with cells of different lengths), do general case
 if((((2-r)|(AN(a)-1)|(AN(w)-1))<0)||2<acr+wcr&&p!=q){  // r>2, or empty.  If max rank <= 2 and sum of ranks >2, neither can be an atom
  RESETRANK; z=rank2ex(a,w,0L,acr,wcr,acr,wcr,jtovg); R z;  // ovg calls other functions, so we clear rank
 }
 // joining rows, or table/row with same lengths, or table/atom.  In any case no fill is possible
 I cc2a=as[ar-2]; p=acr?p:1; cc2a=acr<=1?1:cc2a; ma=cc2a*p; ma=wcr>acr+1?q:ma;  //   cc2a is # 2-cells of a; ma is #atoms in a cell of a EXCEPT when joining atom a to table w: then length of row of w
 I cc2w=ws[wr-2]; q=wcr?q:1; cc2w=wcr<=1?1:cc2w; mw=cc2w*q; mw=acr>wcr+1?p:mw;  // sim for w;
 I f=(wf>=af)?wf:af; I shortf=(wf>=af)?af:wf; I *s=(wf>=af)?ws:as;
 PROD(replct,f-shortf,s+shortf); PROD(framect,shortf,s);  // Number of cells in a and w; known non-empty shapes
 RE(zn=mult(replct*framect,ma+mw));  // total # atoms in result
 GA(z,t,zn,f+r,s); zv=CAV(z); s=AS(z)+f+r;   // allocate result; repurpose s to point to END+1 of shape field
 if(2>r)s[-1]=ma+mw; else{s[-1]=acr?p:q; s[-2]=cc2a+cc2w;}  // fill in last 2 atoms of shape
 k=bpnoun(t);   // # bytes per atom of result
 // copy in the data, creating the result in order (to avoid page thrashing and to make best use of write buffers)
 // scalar replication is required for any arg whose rank is 0 and yet its length is >1.  Choose the copy routine based on that
 moveawtbl[SGNTO0((acr-1)&(1-ma))*2+(SGNTO0(((wcr-1)&(1-mw))))](CAV(z),CAV(a),CAV(w),replct*framect,k,ma*k,mw*k,(wf>=af)?replct:1,(wf>=af)?1:replct);
 RETF(z);
}    /* overall control, and a,w and a,"r w for cell rank <: 2 */

F2(jtstitch){B sp2;I ar,wr; A z;
 RZ(a&&w);
 ar=AR(a); wr=AR(w); sp2=(SPARSE&(AT(a)|AT(w)))&&2>=ar&&2>=wr;
 ASSERT(!ar||!wr||*AS(a)==*AS(w),EVLENGTH);
 if((((SPARSE&(AT(a)|AT(w)))-1)&(2-ar)&(2-wr))>=0)R IRS2(a,w,0L,(ar-1)&RMAX,(wr-1)&RMAX,jtover,z);  // not sparse or rank>2
 R stitchsp2(a,w);  // sparse rank <=2 separately
}

F1(jtlamin1){A x;I* RESTRICT s,* RESTRICT v,wcr,wf,wr; 
 RZ(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; RESETRANK; wf=wr-wcr;
 fauxblockINT(wfaux,4,1); fauxINT(x,wfaux,1+wr,1) v=AV(x);
 s=AS(w); MCISH(v,s,wf); v[wf]=1; MCISH(v+wf+1,s+wf,wcr);  // frame, 1, shape - the final shape
 R reshape(x,w);
}    /* ,:"r w */

F2(jtlamin2){A z;I ar,p,q,wr;
 RZ(a&&w); 
 ar=AR(a); p=jt->ranks>>RANKTX; p=ar<p?ar:p;
 wr=AR(w); q=(RANKT)jt->ranks; q=wr<q?wr:q; RESETRANK;
 if(p)RZ(a=IRS1(a,0L,p,jtlamin1,z));
 if(q)RZ(w=IRS1(w,0L,q,jtlamin1,z));
 RZ(IRS2(a,w,0L,p+!!p,q+!!q,jtover,z));
 if(!(p|q))z=IRS1(z,0L,0L,jtlamin1,a);
 RETF(z);
}    /* a,:"r w */

// Append, including tests for append-in-place
A jtapip(J jt, A a, A w){F2PREFIP;A h;C*av,*wv;I ak,k,p,*u,*v,wk,wm,wn;
 RZ(a&&w);
 // Allow inplacing if we have detected an assignment to a name on the last execution, and the address
 // being assigned is the same as a, and the usecount of a allows inplacing; or
 // the argument a is marked inplaceable.  Usecount of <1 is inplaceable, and for memory-mapped nouns, 2 is also OK since
 // one of the uses is for the mapping header.
 // In both cases we require the inplaceable bit in jt, so that a =: (, , ,) a  , which has assignsym set, will inplace only the last append
 // This is 'loose' inplacing, which doesn't scruple about globals appearing on the stack elsewhere
 // Allow only DIRECT and BOX types, to simplify usecounting
 I virtreqd=0;  // the inplacing test sets this if the result must be virtual
 // Because the test for inplaceability is rather lengthy, start with a quick check of the atom counts.  If adding the atoms in w to those in a
 // would push a over a power-of-2 boundary, skip  the rest of the testing.  We detect this by absence of carry out of the high bit (inside EXTENDINPLACE)
 if((((I)jtinplace&JTINPLACEA) && EXTENDINPLACENJA(a,w)) && AT(a)&(DIRECT|BOX)) {I an=AN(a);
  // if w is boxed, we have some more checking to do.  We have to make sure we don't end up with a box of a pointing to a itself.  The only way
  // this can happen is if w is (<a) or (<<a) or the like, where w does not have a recursive usecount.  The fastest way to check this would be to
  // crawl through w looking for a.
  // Instead, we simply convert w to recursive-usecount.  This may take some time if w is complex, but it will (1) increment the
  // usecount of a if any part of w refers to a (3) make the eventual incrementing of usecount in a quicker.  After we have resolved w we see if the usecount of a has budged.  If not, we can proceed with inplacing.
  if(AT(a)&BOX){
   I oldac = ACUC(a);  // remember original UC of a
   ra0(w);  // ensure w is recursive usecount.  This will be fast if w has 1=L.
   if(AC(a)>oldac)an = 0;  // turn off inplacing if w referred to a
   an&=virtreqd-1;  // turn off inplacing if the result must be virtual
  }

  // Here the usecount indicates inplaceability.  We have to see if the argument ranks and shapes permit it also
  // We disqualify inplacing if a is empty (because we wouldn't know what type to make the result, and anyway there may be axes
  // in the shape that are part of the shape of an item), or if a is atomic (because
  // we would have to replicate a, and anyway how much are you saving?), or if w has higher rank than a (because the rank of the
  // result would increase, and there's no room in the shape)
  // jt->ranks is ~0 unless there are operand cells, which disqualify us.  There are some cases where it
  // would be OK to inplace an operation where the frame of a (and maybe even w) is all 1s, but that's not worth checking for
  // OK to use type as proxy for size, since indirect types are excluded
#if BW==64
  if(((an-1)|(AR(a)-1)|(AR(a)-AR(w))|(AT(a)-AT(w))|((I)jt->ranks-(I)(RANK2T)~0))>=0){  // a not empty, a not atomic, ar>=wr, atype >= wtype, no jt->ranks given
#else
  if(((an-1)|(AR(a)-1)|(AR(a)-AR(w))|(AT(a)-AT(w)))>=0&&(jt->ranks==(RANK2T)~0)){  // a not empty, a not atomic, ar>=wr, atype >= wtype, no jt->ranks given
#endif
   //  Check the item sizes.  Set p<0 if the
   // items of a require fill (ecch - can't go inplace), p=0 if no padding needed, p>0 if items of w require fill
   // If there are extra axes in a, they will become unit axes of w.  Check the axes of w that are beyond the first axis
   // of a, because the first axis of a tells how many items there are - that number doesn't matter, it's the
   // shape of an item of result that matters
   I naxes = MIN(AR(w),AR(a)); u=AR(a)+(AS(a))-naxes; v=AR(w)+(AS(w))-naxes;  // point to the axes to compare
   // Calculate k, the size of an atom of a; ak, the number of bytes in a; wm, the number of result-items in w
   // (this will be 1 if w has to be rank-extended, otherwise the number of items in w); wk, the number of bytes in
   // items of w (after its conversion to the precision of a)
   k=bpnoun(AT(a)); ak=k*an; wm=AR(a)==AR(w)?AS(w)[0]:1; wn=wm*aii(a); wk=k*wn;  // We don't need this yet but we start the computation early
   // For each axis to compare, see if a is bigger/equal/smaller than w; OR into p
   p=0; DQ(naxes, p |= *u++-*v++;);
   // Now p<0 if ANY axis of a needs extension - can't inplace then
   if(p>=0) {
    // See if there is room in a to fit w (including trailing pad)
    if(allosize(a)>=ak+wk+(AT(a)&LAST0?SZI:0)){
     // We have passed all the tests.  Inplacing is OK.
     // If w must change precision, do.  This is where we catch domain errors.
     if(TYPESGT(AT(a),AT(w)))RZ(w=cvt(AT(a),w));
     // If the items of w must be padded to the result item-size, do so.
     // If the items of w are items of the result, we simply extend each to the shape of
     // an item of a, leaving the number of items unchanged.  Otherwise, the whole of w becomes an
     // item of the result, and it is extended to the size of a corresponding cell of a.  The extra
     // rank is implicit in the shape of a.
     if(p){RZ(h=caro(vec(INT,AR(w),AS(a)+AR(a)-AR(w)))); if(AR(a)==AR(w))AV(h)[0]=AS(w)[0]; RZ(w=take(h,w));}
     av=ak+CAV(a); wv=CAV(w);   // av->end of a data, wv->w data
     // If an item of a is higher-rank than the entire w (except when w is an atom, which gets replicated),
     // copy fill to the output area.  Start the copy after the area that will be filled in by w
     I wlen = k*AN(w); // the length in bytes of the data in w
     if((-AR(w)&(1+AR(w)-AR(a)))<0){RZ(setfv(a,w)); mvc(wk-wlen,av+wlen,k,jt->fillv);}
     // Copy in the actual data, replicating if w is atomic
     if(AR(w))MC(av,wv,wlen); else mvc(wk,av,k,wv);
     // The data has been copied.  Now adjust the result block to match.  If the operation is virtual extension we have to allocate a new block for the result
     if(!virtreqd){
      // Normal append-in-place.
      // Update the # items in a, and the # atoms
      AS(a)[0]+=wm; AN(a)+=wn;
      // if a has recursive usecount, increment the usecount of the added data - including any fill
      // convert wn to be the number of indirect pointers in the added data (RAT types have 2, the rest have 1)
      if(UCISRECUR(a)){wn*=k>>LGSZI; A* aav=(A*)av; DO(wn, ras(aav[i]);)}
     }else{
      // virtual extension.  Allocate a virtual block, which will extend past the original block.  Fill in AN and AS for the block
      A oa=a; RZ(a=virtual(a,0,AR(a))); AN(a)=AN(oa)+wn; AS(a)[0]=AS(oa)[0]+wm; MCISH(&AS(a)[1],&AS(oa)[1],AR(oa)-1);
     }
     RETF(a);
    }
   }
  }
 }
 R(over(a,w));  // if there was trouble, failover to non-in-place code
}    /* append in place if possible */
