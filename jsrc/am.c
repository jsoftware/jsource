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

#define MCASE(t,k)  ((t)+16*(k))
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
 switch(MCASE(it,k)){
  case MCASE(B01,sizeof(C)): DO(c,         *zc++=wc[*b++?i+c:i];); break;
  case MCASE(B01,sizeof(I)): DO(c,         *zi++=wi[*b++?i+c:i];); break;
#if !SY_64
  case MCASE(B01,sizeof(D)): DO(c,         *zd++=wd[*b++?i+c:i];); break;
#endif
  case MCASE(INT,sizeof(C)): DO(c, MINDEX; *zc++=wc[i+c*j];); break;
  case MCASE(INT,sizeof(I)): DO(c, MINDEX; *zi++=wi[i+c*j];); break;
#if !SY_64
  case MCASE(INT,sizeof(D)): DO(c, MINDEX; *zd++=wd[i+c*j];); break;
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
  p=t&B01+LIT+INT+FL+CMPX&&AT(b)&NUMERIC;    // fail if first value in list is indirect or pqr is not numeric
  if(p)DO(m, y=u[i]; if(!(t==AT(y)&&r==AR(y)&&!ICMP(s,AS(y),r))){p=0; break;});  // fail if list is not homogeneous in type, rank, and shape
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
 switch((!q?12:!p?0:c==0?4:8)+(t&B01+LIT?0:t&INT?1:t&FL?2:3)){   // choose a comparison based on data
  case  0: CASE2Z(C);  case  1: CASE2Z(I);  case  2: CASE2Z(D);  case  3: CASE2Z(Z);
  case  4: CASE2X(C);  case  5: CASE2X(I);  case  6: CASE2X(D);  case  7: CASE2X(Z);
  case  8: CASE2Y(C);  case  9: CASE2Y(I);  case 10: CASE2Y(D);  case 11: CASE2Y(Z);
  case 12: CASENZ(C);  case 13: CASENZ(I);  case 14: CASENZ(D);  case 15: CASENZ(Z);
  default: ASSERTSYS(0,"casev");
}}   /* z=:b}x0,x1,x2,...,x(m-2),:x(m-1) */


A jtmerge2(J jt,A a,A w,A ind,B ip){A z;I an,ar,*as,at,in,ir,*iv,k,t,wn,wt;
 RZ(a&&w&&ind);
 an=AN(a); at=AT(a); ar=AR(a); as=AS(a); 
 wn=AN(w); wt=AT(w);
 in=AN(ind); ir=AR(ind); iv=AV(ind);
 ASSERT(!an||!wn||HOMO(at,wt),EVDOMAIN);
 ASSERT(ar<=ir,EVRANK);
 ASSERT(!ICMP(as,AS(ind)+ir-ar,ar),EVLENGTH);
 if(!wn)R ca(w);
 RE(t=an&&wn?maxtype(at,wt):wt);
 if(an&&t!=at)RZ(a=cvt(t,a));
 if(ip&&t==wt&&(!(t&BOX)||AFNJA&AFLAG(w))){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}
 else{RZ(z=cvt(t,w)); if(ARELATIVE(w))RZ(z=relocate((I)w-(I)z,z));}
 if(ip&&t&BOX&&AFNJA&AFLAG(w)){A*av,t,x,y;A1*zv;I ad,*tv;
  ad=(I)a*ARELATIVE(a); av=AAV(a); zv=A1AV(z);
  GATV(t,INT,in,1,0); tv=AV(t); memset(tv,C0,in*SZI);
  DO(in, y=smmcar(z,AVR(i%an)); if(!y)break; tv[i]=(I)y;);
  if(!y){DO(in, if(!tv[i])break; smmfrr((A)tv[i]);); R 0;}
  DO(in, x=(A)AABS(zv[iv[i]],z); zv[iv[i]]=AREL(tv[i],z); smmfrr(x););
 }else{
  if(ARELATIVE(a))RZ(a=rca(a));
  if(ARELATIVE(z)){A*av=AAV(a),*zv=AAV(z);          DO(in, zv[iv[i]]=(A)AREL(av[i%an],z););}
  else            {C*av=CAV(a),*zv=CAV(z); k=bp(t); DO(in, MC(zv+k*iv[i],av+k*(i%an),k); );}
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
  R n==wr?pdt(j,x):irs2(pdt(j,vec(INT,n,AV(x))),iota(vec(INT,wr-n,ws+n)),0L,0L,RMAX,jtplus);
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
   RZ(j=irs2(tymes(j,sc(d)),k,0L,0L,RMAX,jtplus));
 }}
 R n==wr?j:irs2(tymes(j,sc(prod(wr-n,ws+n))),iota(vec(INT,wr-n,ws+n)),0L,0L,RMAX,jtplus);
}    /* convert ind in a ind}w into integer positions */

/* Reference count for w for amend in place */
/* 1 jdo     tpop                           */
/* 2 amendn2 EPILOG/gc                      */
/* 1 jdo     tpop                           */

static A jtamendn2(J jt,A a,A w,A ind,B ip){PROLOG(0007);A e,z;B b,sa,sw;I at,ir,it,t,t1,wt;P*p;
 RZ(a&&w&&ind);
 at=AT(a); sa=1&&at&SPARSE; if(sa)at=DTYPE(at);
 wt=AT(w); sw=1&&wt&SPARSE; if(sw)wt=DTYPE(wt);
 it=AT(ind); ir=AR(ind);
 ASSERT(it&NUMERIC+BOX||!AN(ind),EVDOMAIN);
 ASSERT(it&DENSE,EVNONCE);
 if(sw){
  ASSERT(!(wt&BOX),EVNONCE); ASSERT(HOMO(at,wt),EVDOMAIN);
  RE(t=maxtype(at,wt)); t1=STYPE(t); RZ(a=t==at?a:cvt(sa?t1:t,a));
  if(ip=ip&&t==wt&&t1!=BOX){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}else RZ(z=cvt(t1,w));
  p=PAV(z); e=SPA(p,e); b=!AR(a)&&equ(a,e);
  p=PAV(a); if(sa&&!equ(e,SPA(p,e))){RZ(a=denseit(a)); sa=0;}
  if(it&NUMERIC||!ir)z=(b?jtam1e:sa?jtam1sp:jtam1a)(jt,a,z,it&NUMERIC?box(ind):ope(ind),ip);
  else{RE(aindex(ind,z,0L,&ind)); ASSERT(ind,EVNONCE); z=(b?jtamne:sa?jtamnsp:jtamna)(jt,a,z,ind,ip);}
 }else z=merge2(sa?denseit(a):a,w,jstd(w,ind),ip);
 EPILOG(z);
}

static DF2(amccn2){R amendn2(a,w,VAV(self)->f,0);}
static DF2(amipn2){R amendn2(a,w,VAV(self)->f,(B)(!(AT(w)&RAT+XNUM)&&(ACUC1>=AC(w)||AFNJA&AFLAG(w))));}

static DF2(amccv2){DECLF; 
 RZ(a&&w); 
 ASSERT(DENSE&AT(w),EVNONCE);
 R merge2(a,w,pind(AN(w),CALL2(f2,a,w,fs)),0);
}

static DF2(amipv2){DECLF; 
 RZ(a&&w); 
 ASSERT(DENSE&AT(w),EVNONCE);
 R merge2(a,w,pind(AN(w),CALL2(f2,a,w,fs)),(B)(!(AT(w)&RAT+XNUM)&&(ACUC1>=AC(w)||AFNJA&AFLAG(w))));
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
 if(AT(w)==LIT) {
  C *stg = CAV(w);
  if(!vnm(n,stg)){
   // not name, see if valid primitive
   UC p = spellin(n,stg);
   R p>=128||ds(p);  // return if valid primitive (all pseudochars are valid primitives, but 0: is not in pst[])
  }
 } else if(AT(w)==BOX) {A *wv;I wd,bmin=0,bmax=0;
  // boxed contents.  There must be exactly 2 boxes.  The first one may be a general AR; or the special cases singleton 0, 2, 3, or 4
  // Second may be anything for special case 0 (noun); otherwise must be a valid gerund, 1 or 2 boxes if first box is general AR, 2 boxes if special case
  // 2 (hook) or 4 (bident), 3 if special case 3 (fork)
  // 
  RZ(n==2);  // verify 2 boxes
  wv = AAV(w); wd = (I)w*ARELATIVE(w); x=WVR(0); // point to pointers to boxes; point to first box contents
  // see if first box is a special flag
  if(LIT==AT(x) && 1>=AR(x) && 1==AN(x)){
   c = CAV(x)[0];   // fetch that character
   if(c=='0')R 1;    // if noun, the second box can be anything & is always OK, don't require AR there
   else if(c=='2'||c=='4')bmin=bmax=2;
   else if(c=='3')bmin=bmax=3;
  }
  // If the first box is not a special case, it had better be a valid AR; and it will take 1 or 2 operands
  if(bmin==0){RZ(gerar(jt,x)); bmin=1,bmax=2;}
  // Now look at the second box.  It should contain between bmin and bmax boxes, each of which must be an AR
  x = WVR(1);   // point to second box
  RZ(BOX==AT(x) && 1==AR(x));   // verify it contains a list of boxes
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


static A jtamend(J jt,A w,B ip){
 RZ(w);
 if(VERB&AT(w)) R ADERIV(CRBRACE,mergv1,ip?amipv2:amccv2,RMAX,RMAX,RMAX);
 else if(ger(w))R gadv(w,CRBRACE);
 else           R ADERIV(CRBRACE,mergn1,ip?amipn2:amccn2,RMAX,RMAX,RMAX);
}

F1(jtrbrace){R amend(w,0);}
DF1(jtamip){ R amend(w,!jtpiplocalerr(jt, self)); }


static DF2(jtamen2){ASSERT(0,EVNONCE);}

F1(jtemend){
 ASSERT(NOUN&AT(w),EVDOMAIN);
 R ADERIV(CEMEND,0L,jtamen2,RMAX,RMAX,RMAX);
}
