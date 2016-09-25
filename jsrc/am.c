/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Amend                                                          */

#include "j.h"


/*
static A jtmerge1(J jt,A w,A ind){PROLOG(0006);A z;C*v,*x;I c,k,r,*s,t,*u;
 RZ(w&&ind);
 RZ(ind=pind(IC(w),ind));
 r=MAX(0,AR(w)-1); s=1+AS(w); t=AT(w); c=aii(w);
 ASSERT(!(t&SPARSE),EVNONCE);
 ASSERT(r==AR(ind),EVRANK);
 ASSERT(!ICMP(s,AS(ind),r),EVLENGTH);
 GA(z,t,c,r,s); x=CAV(z); v=CAV(w); u=AV(ind); k=bp(t);
 DO(c, MC(x+k*i,v+k*(i+c*u[i]),k););
 EPILOG(z);
}
*/

#define MCASE(t,k)  ((t)+4*(k))
#define MINDEX        {j=*u++; if(0>j)j+=m; ASSERT(0<=j&&j<m,EVINDEX);}

static A jtmerge1(J jt,A w,A ind){A z;B*b;C*wc,*zc;D*wd,*zd;I c,it,j,k,m,r,*s,t,*u,*wi,*zi;
 RZ(w&&ind);
 r=MAX(0,AR(w)-1); s=1+AS(w); t=AT(w); k=bp(t); m=IC(w); c=aii(w);
 ASSERT(!(t&SPARSE),EVNONCE);
 ASSERT(r==AR(ind),EVRANK);
 ASSERT(!ICMP(s,AS(ind),r),EVLENGTH);
 GA(z,t,c,r,s);
 if(!(AT(ind)&B01+INT))RZ(ind=cvt(INT,ind));
 it=AT(ind); u=AV(ind); b=(B*)u;
 ASSERT(!c||1<m||!(it&B01),EVINDEX);
 ASSERT(!c||1!=m||!memchr(b,C1,c),EVINDEX);
 zi=AV(z); zc=(C*)zi; zd=(D*)zc;
 wi=AV(w); wc=(C*)wi; wd=(D*)wc;
 switch(MCASE(CTTZ(it),k)){
  case MCASE(B01X,sizeof(C)): DO(c,         *zc++=wc[*b++?i+c:i];); break;
  case MCASE(B01X,sizeof(I)): DO(c,         *zi++=wi[*b++?i+c:i];); break;
#if !SY_64
  case MCASE(B01X,sizeof(D)): DO(c,         *zd++=wd[*b++?i+c:i];); break;
#endif
  case MCASE(INTX,sizeof(C)): DO(c, MINDEX; *zc++=wc[i+c*j];); break;
  case MCASE(INTX,sizeof(I)): DO(c, MINDEX; *zi++=wi[i+c*j];); break;
#if !SY_64
  case MCASE(INTX,sizeof(D)): DO(c, MINDEX; *zd++=wd[i+c*j];); break;
#endif  
  default: if(it&B01)DO(c,         MC(zc,wc+k*(*b++?i+c:i),k); zc+=k;)
           else      DO(c, MINDEX; MC(zc,wc+k*(i+c*j     ),k); zc+=k;); break;
 }
 R RELOCATE(w,z);
}

#define CASE2Z(T)  {T*xv=(T*)AV(x),*yv=(T*)AV(y),*zv=(T*)AV(z); DO(n, zv[i]=bv[i]?yv[i]:xv[i];); R z;}
#define CASE2X(T)  {T*xv=(T*)AV(x),*yv=(T*)AV(y);               DO(n, if( bv[i])xv[i]=yv[i];);   R x;}
#define CASE2Y(T)  {T*xv=(T*)AV(x),*yv=(T*)AV(y);               DO(n, if(!bv[i])yv[i]=xv[i];);   R y;}
#define CASENZ(T)  {T*zv=(T*)AV(z); DO(n, j=iv[i]; if(0>j){j+=m; ASSERT(0<=j,EVINDEX);}else ASSERT(j<m,EVINDEX);  \
                       zv[i]=*(i+(T*)aa[j]);); R z;}

// Handle the case statement abc =: pqr} x,...,y,:z, with in-place operation if pqr is Boolean and abc appears on the right
F1(jtcasev){A b,*u,*v,w1,x,y,z;B*bv,p,q;I*aa,c,*iv,j,m,n,r,*s,t;
 RZ(w);
 RZ(w1=ca(w)); u=AAV(w1);   // make a copy of the input, point to its value
 // the input is a boxed list.  The last 3 values are (name pqr);(index in which abc appeared in the x,y,... or -1 if it didn't);(original sentence queue)
 p=1; m=AN(w)-3; v=AAV(w); c=i0(v[m+1]);   // get # items in list, and index of the matching one
 // Now audit the input names (including pqr), since we haven't properly stacked them & checked them etc.
 // p is set to 0 if an audit fails
 DO(m+1, x=symbrd(v[i]); if(!x){p=0; RESETERR; break;} u[i]=x; p=p&&NOUN&AT(x););  // verify names defined, and are nouns
 if(p){
  b=u[m]; n=AN(b); r=AR(b); s=AS(b); t=AT(*u);  // length, rank, shape, of pqr; type of first value in list
  p=t&DIRECT&&AT(b)&NUMERIC;    // fail if first value in list is indirect or pqr is not numeric
  if(p)DO(m, y=u[i]; if(!(TYPESEQ(t,AT(y))&&r==AR(y)&&!ICMP(s,AS(y),r))){p=0; break;});  // fail if list is not homogeneous in type, rank, and shape
 }
 // If the audit failed, the sentence might work, but we won't be doing it here.  Go run the original sentence
 if(!p)R parse(v[m+2]);   // NOTE this will end up assigning the value twice: once in the parse, and again when we return.  Should we whack off the first two words?
 // We can do it here!  We split into two cases: Boolean pqr with two names in the list, which can never fail;
 // and all other, which may produce index error
 if(q=2==m&&B01&AT(b)){bv=BAV(b); x=u[0]; y=u[1];}  // fast case: exactly two names x and y
 else{   // slow case
  if(!(INT&AT(b)))RZ(b=cvt(INT,b));  // convert pqr to int if it's not already
  iv=AV(b);    // iv points to the input pqr
  GATV(x,INT,m,1,0); aa=AV(x); DO(m, aa[i]=(I)AV(u[i]););  // create x, which is a list of pointers to the values of the names
 }
 // Check to see if we can modify in-place.  We can do so only if abc was one of the two names on the right, and we have the
 // fast (no-error) case; and of course if the use-count is only 1.  But if the assignment is local, we also have to make
 // sure abc is locally defined
 if(p=q&&0<=c&&ACUC1>=AC(u[c])) {  // passes quick check
   p= !jt->local || *CAV(AAV(v[m+2])[1])!=CASGN || probe(AAV(v[m+2])[0], jt->local);  // OK if not in explicit, or not local assignment, or name defined
    // Get the pointer to the parsed sentence; go to its data; take pointer for word[1]; go to its (character) data; take first character
    // then look up the symbol entry for word[0]
 }
 if(p)z=u[c]; else GA(z,t,n,r,s);   // z = output area, which may be in-place
// switch((!q?12:!p?0:c==0?4:8)+(t&B01+LIT?0:t&INT?1:t&FL?2:3)){   // choose a comparison based on data
//  case  0: CASE2Z(C);  case  1: CASE2Z(I);  case  2: CASE2Z(D);  case  3: CASE2Z(Z);
//  case  4: CASE2X(C);  case  5: CASE2X(I);  case  6: CASE2X(D);  case  7: CASE2X(Z);
//  case  8: CASE2Y(C);  case  9: CASE2Y(I);  case 10: CASE2Y(D);  case 11: CASE2Y(Z);
//  case 12: CASENZ(C);  case 13: CASENZ(I);  case 14: CASENZ(D);  case 15: CASENZ(Z);
// 0 B01+LIT 1 INT 2 FL 3 CMPX 4 C2T 5 C4T 6 SBT 7 unused
 switch((!q?24:!p?0:c==0?8:16)+(t&B01+LIT?0:t&INT?1:t&FL?2:t&CMPX?3:t&C2T?4:t&C4T?5:6)){   // choose a comparison based on data
  case  0: CASE2Z(C);  case  1: CASE2Z(I);  case  2: CASE2Z(D);  case  3: CASE2Z(Z);
  case  4: CASE2Z(US); case  5: CASE2Z(C4); case  6: CASE2Z(I);
  case  8: CASE2X(C);  case  9: CASE2X(I);  case 10: CASE2X(D);  case 11: CASE2X(Z);
  case 12: CASE2X(US); case 13: CASE2X(C4); case 14: CASE2X(I);
  case 16: CASE2Y(C);  case 17: CASE2Y(I);  case 18: CASE2Y(D);  case 19: CASE2Y(Z);
  case 20: CASE2Y(US); case 21: CASE2Y(C4); case 22: CASE2Y(I);
  case 24: CASENZ(C);  case 25: CASENZ(I);  case 26: CASENZ(D);  case 27: CASENZ(Z);
  case 28: CASENZ(US); case 29: CASENZ(C4); case 30: CASENZ(I);
  default: ASSERTSYS(0,"casev");
}}   /* z=:b}x0,x1,x2,...,x(m-2),:x(m-1) */

// Handle a ind} w after indices have been converted to integer
static A jtmerge2(J jt,A a,A w,A ind){F2PREFIP;A z;I an,ar,*as,at,in,ir,*iv,t,wn,wt;
 RZ(a&&w&&ind);
 // ?n=#atoms, ?t=type, ?r=rank, ?s->shape where ?=awi for xym
 an=AN(a); at=AT(a); ar=AR(a); as=AS(a); 
 wn=AN(w); wt=AT(w);
 in=AN(ind); ir=AR(ind); iv=AV(ind);
 ASSERT(!an||!wn||HOMO(at,wt),EVDOMAIN);  // error if xy not empty and not compatible
 ASSERT(ar<=ir,EVRANK);   // require shape of x to be a suffix of the shape of m
 ASSERT(!ICMP(as,AS(ind)+ir-ar,ar),EVLENGTH);
 if(!wn)R ca(w);  // if y empty, clone it & return.  It's small.  Ignore inplacing
 RE(t=an?maxtype(at,wt):wt);  // get the type of the result: max of types, but if x empty, leave y as is
 if(an&&!TYPESEQ(t,at))RZ(a=cvt(t,a));  // if a must change precision, do so
 // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the type is either
 // DIRECT or this is a boxed memory-mapped array; and don't inplace a =: a m} a
 // kludge this inplaces boxed mm arrays when usecount>2.  Seems wrong, but that's the way it was done
 I ip = ((I)jtinplace&JTINPLACEW) && (ACIPISOK(w) || jt->assignsym&&jt->assignsym->val==w&&(AC(w)<=1||(AFNJA&AFLAG(w))))
      &&TYPESEQ(t,wt)&&(wt&DIRECT||((t&BOX)&&AFNJA&AFLAG(w)))&&w!=a&&w!=ind;
 if(ip){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}
 // If not inplaceable, create a new block (cvt always allocates a new block) with the common precision.  Relocate it if necessary.
 else{RZ(z=cvt(t,w)); if(ARELATIVE(w))RZ(z=relocate((I)w-(I)z,z));}
 if(ip&&t&BOX&&AFNJA&AFLAG(w)){A*av,t,x,y;A1*zv;I ad,*tv;
  // in-placeable boxed memory-mapped array
  ad=(I)a*ARELATIVE(a); av=AAV(a); zv=A1AV(z);  // point to items of x and result
  GATV(t,INT,in,1,0); tv=AV(t); memset(tv,C0,in*SZI);   // allocate an array of pointers, one per item of x; clear to 0
  DO(in, y=smmcar(z,AVR(i%an)); if(!y)break; tv[i]=(I)y;);   // copy the items of x (repeated as needed) into the smm area of result
  if(!y){DO(in, if(!tv[i])break; smmfrr((A)tv[i]);); R 0;}   // if the copy failed, free the blocks in smm area and fail this call
  DO(in, x=(A)AABS(zv[iv[i]],z); zv[iv[i]]=AREL(tv[i],z); smmfrr(x););  // replace pointer to old block with (relative) pointer to new, then free the (absolute) old in smm area
 }else{
  // normal assignment - just copy the items.  Relocate relative blocks
  if(ARELATIVE(a))RZ(a=rca(a));
  if(ARELATIVE(z)){A*av=AAV(a),*zv=AAV(z);          DO(in, zv[iv[i]]=(A)AREL(av[i%an],z););}
  else{
   // Here for non-relative blocks.
   C* RESTRICT av0=CAV(a); I k=bp(t); C * RESTRICT avn=av0+(an*k);
   switch(k){
   case sizeof(C):
    {C * RESTRICT zv=CAV(z); C *RESTRICT av=(C*)av0; DO(in, zv[iv[i]]=*av; if((++av)==(C*)avn)av=(C*)av0;); break;}  // scatter-copy the data
   case sizeof(I):
    {I * RESTRICT zv=AV(z); I *RESTRICT av=(I*)av0; DO(in, zv[iv[i]]=*av; if((++av)==(I*)avn)av=(I*)av0;); break;}  // scatter-copy the data
   // no case for D, in case floating-point unit changes bitpatterns.  Safe to use I for D, though
   default:
    {C* RESTRICT zv=CAV(z); C *RESTRICT av=(C*)av0; DO(in, MC(zv+(iv[i]*k),av,k); if((av+=k)==avn)av=av0;);}  // scatter-copy the data
   }
  }
 }
 R z;
}

A jtjstd(J jt,A w,A ind){A j=0,k,*v,x;B b;I d,i,id,n,r,*s,*u,wr,*ws;
 wr=AR(w); ws=AS(w); b=AN(ind)&&BOX&AT(ind);
 if(!wr)R from(ind,zero);
 if(b&&AR(ind)){
  RE(aindex(ind,w,0L,&j));
  if(!j){
   RZ(x=from(ind,increm(iota(shape(w))))); u=AV(x); 
   DO(AN(x), ASSERT(*u,EVDOMAIN); --*u; ++u;); 
   R x;
  }
  k=AAV0(ind); n=AN(k);
  GATV(x,INT,wr,1,0); u=wr+AV(x); s=wr+ws; d=1; DO(wr, *--u=d; d*=*--s;);
  R n==wr?pdt(j,x):irs2(pdt(j,vec(INT,n,AV(x))),iota(vec(INT,wr-n,ws+n)),0L,VFLAGNONE, RMAX,jtplus);
 }
 if(!b){n=1; RZ(j=pind(*ws,ind));}
 else{
  ind=AAV0(ind); n=AN(ind); r=AR(ind);
  ASSERT(!n&&1==r||AT(ind)&BOX+NUMERIC,EVINDEX);
  if(n&&!(BOX&AT(ind)))RZ(ind=every(ind,0L,jtright1));
  v=AAV(ind); id=(I)ind*ARELATIVE(ind);
  ASSERT(1>=r,EVINDEX);
  ASSERT(n<=wr,EVINDEX);
  d=n; DO(n, --d; if(!equ(ace,AADR(id,v[d])))break;); if(n)++d; n=d;
  j=zero;
  for(i=0;i<n;++i){
   x=AADR(id,v[i]); d=ws[i];
   if(AN(x)&&BOX&AT(x)){
    ASSERT(!AR(x),EVINDEX); 
    x=AAV0(x); k=IX(d);
    if(AN(x))k=less(k,pind(d,1<AR(x)?ravel(x):x));
   }else k=pind(d,x);
   RZ(j=irs2(tymes(j,sc(d)),k,0L,VFLAGNONE, RMAX,jtplus));
 }}
 R n==wr?j:irs2(tymes(j,sc(prod(wr-n,ws+n))),iota(vec(INT,wr-n,ws+n)),0L,VFLAGNONE, RMAX,jtplus);
}    /* convert ind in a ind}w into integer positions */

/* Reference count for w for amend in place */
/* 1 jdo     tpop                           */
/* 2 amendn2 EPILOG/gc                      */
/* 1 jdo     tpop                           */

// Execution of x m} y.  Split on sparse/dense, passing on the dense to merge2, including inplaceability
static DF2(jtamendn2){F2PREFIP;PROLOG(0007);A e,z;I sa,sw; B b;I at,ir,it,t,t1,wt;P*p;
 A ind=VAV(self)->f;
 RZ(a&&w&&ind);
 // ?t = underlying type of ?, s?=nonzero if sparse
 at=AT(a); sa=at&SPARSE; if(sa)at=DTYPE(at);
 wt=AT(w); sw=wt&SPARSE; if(sw)wt=DTYPE(wt);
 it=AT(ind); ir=AR(ind);
 ASSERT(it&NUMERIC+BOX||!AN(ind),EVDOMAIN);
 ASSERT(it&DENSE,EVNONCE);  // m must be dense, and numeric or boxed
 if(sw){
  // Sparse w.  a an t must be compatible; sparse w must not be boxed
  ASSERT(!(wt&BOX),EVNONCE); ASSERT(HOMO(at,wt),EVDOMAIN);
  // set t to dense precision of result; t1=corresponding sparse precision; convert a if need be
  RE(t=maxtype(at,wt)); t1=STYPE(t); RZ(a=TYPESEQ(t,at)?a:cvt(sa?t1:t,a));
  // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the dense type is either
  // DIRECT or this is a boxed memory-mapped array
  B ip=((I)jtinplace&JTINPLACEW) && (ACIPISOK(w) || jt->assignsym&&jt->assignsym->val==w&&(AC(w)<=1||(AFNJA&AFLAG(w)&&AC(w)==2)))
      &&TYPESEQ(t,wt)&&(t&DIRECT);
  // see if inplaceable.  If not, convert w to correct precision (note that cvt makes a copy if the precision is already right)
  if(ip){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}else RZ(z=cvt(t1,w));
  // call the routine to handle the sparse amend
  p=PAV(z); e=SPA(p,e); b=!AR(a)&&equ(a,e);
  p=PAV(a); if(sa&&!equ(e,SPA(p,e))){RZ(a=denseit(a)); sa=0;}
  if(it&NUMERIC||!ir)z=(b?jtam1e:sa?jtam1sp:jtam1a)(jt,a,z,it&NUMERIC?box(ind):ope(ind),ip);
  else{RE(aindex(ind,z,0L,&ind)); ASSERT(ind,EVNONCE); z=(b?jtamne:sa?jtamnsp:jtamna)(jt,a,z,ind,ip);}
 }else{
  // Dense w.  Convert indexes to integer indexes, transfer to merge2 to do the work
  z=jtmerge2(jtinplace,sa?denseit(a):a,w,jstd(w,ind));
 }
 EPILOG(z);
}

#if 0
static DF2(amccn2){R amendn2(a,w,VAV(self)->f,0);}
static DF2(amipn2){R amendn2(a,w,VAV(self)->f,(B)(!(AT(w)&RAT+XNUM)&&(ACUC1>=AC(w)||AFNJA&AFLAG(w))));}
#endif

// Execution of x u} y.  Call u to get the indices, then
// call merge2 to do the merge.  Pass inplaceability into merge2.
static DF2(amccv2){F2PREFIP;DECLF; 
 RZ(a&&w); 
 ASSERT(DENSE&AT(w),EVNONCE);  // u} not supported for sparse
 R jtmerge2(jtinplace,a,w,pind(AN(w),CALL2(f2,a,w,fs)));
}


static DF1(mergn1){       R merge1(w,VAV(self)->f);}
static DF1(mergv1){DECLF; R merge1(w,CALL1(f1,w,fs));}

static B ger(A w){A*wv,x;I wd;
 if(!(BOX&AT(w)))R 0;
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(AN(w), x=WVR(i); if(BOX&AT(x)&&1==AR(x)&&2==AN(x))x=AAV0(x); if(!(LIT&AT(x)&&1>=AR(x)&&AN(x)))R 0;);
 R 1;
}    /* 0 if w is definitely not a gerund; 1 if possibly a gerund */

// w is the contents of a presumptive AR/gerund.  Return 1 if good form for AR/gerund, 0 if not
// we do not check the type of names, since they might become defined later
static B gerar(J jt, A w){A x; C c;
 I n = AN(w); 
 // literal contents are OK if a valid name or a primitive
 if(AT(w)&LIT) {
  C *stg = CAV(w);
  if(!vnm(n,stg)){
   // not name, see if valid primitive
   UC p = spellin(n,stg);
   R p>=128||ds(p);  // return if valid primitive (all pseudochars are valid primitives, but 0: is not in pst[])
  }
 } else if(AT(w)&BOX) {A *wv;I wd,bmin=0,bmax=0;
  // boxed contents.  There must be exactly 2 boxes.  The first one may be a general AR; or the special cases singleton 0, 2, 3, or 4
  // Second may be anything for special case 0 (noun); otherwise must be a valid gerund, 1 or 2 boxes if first box is general AR, 2 boxes if special case
  // 2 (hook) or 4 (bident), 3 if special case 3 (fork)
  // 
  RZ(n==2);  // verify 2 boxes
  wv = AAV(w); wd = (I)w*ARELATIVE(w); x=WVR(0); // point to pointers to boxes; point to first box contents
  // see if first box is a special flag
  if(LIT&AT(x) && 1>=AR(x) && 1==AN(x)){
   c = CAV(x)[0];   // fetch that character
   if(c=='0')R 1;    // if noun, the second box can be anything & is always OK, don't require AR there
   else if(c=='2'||c=='4')bmin=bmax=2;
   else if(c=='3')bmin=bmax=3;
  }
  // If the first box is not a special case, it had better be a valid AR; and it will take 1 or 2 operands
  if(bmin==0){RZ(gerar(jt,x)); bmin=1,bmax=2;}
  // Now look at the second box.  It should contain between bmin and bmax boxes, each of which must be an AR
  x = WVR(1);   // point to second box
  RZ(BOX&AT(x) && 1==AR(x));   // verify it contains a list of boxes
  RZ(bmin<=AN(x)&&bmax>=AN(x));  // verify correct number of boxes
  R gerexact(x);  // recursively audit the other ARs in the second box
 } else R 0;
 R 1;
}

B jtgerexact(J jt, A w){
 A*wv; I wd;
 RZ(BOX&AT(w));   // verify gerund is boxed
 RZ(AN(w));   // verify there are boxes
 wv = AAV(w); wd = (I)w*ARELATIVE(w);  // point to pointers to contents
 DO(AN(w), RZ(gerar(jt, WVR(i))););   // fail if any box contains a non-gerund
 R 1;
}    /* 0 if w is definitely not a gerund; 1 if possibly a gerund */


// u} handling.  This is not inplaceable but the derived verb is
F1(jtamend){
 RZ(w);
 if(VERB&AT(w)) R ADERIV(CRBRACE,mergv1,amccv2,VASGSAFE|VINPLACEOK2, RMAX,RMAX,RMAX);  // verb} 
 else if(ger(w))R gadv(w,CRBRACE);   // v0`v1`v2}
 else           R ADERIV(CRBRACE,mergn1,jtamendn2,VASGSAFE|VINPLACEOK2, RMAX,RMAX,RMAX);  // m}
}

static DF2(jtamen2){ASSERT(0,EVNONCE);}

F1(jtemend){
 ASSERT(NOUN&AT(w),EVDOMAIN);
 R ADERIV(CEMEND,0L,jtamen2,VFLAGNONE, RMAX,RMAX,RMAX);
}
