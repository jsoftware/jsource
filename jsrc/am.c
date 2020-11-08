/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Amend                                                          */

#include "j.h"


/*
static A jtmerge1(J jt,A w,A ind){PROLOG(0006);A z;C*v,*x;I c,k,r,*s,t,*u;
 ARGCHK2(w,ind);
 RZ(ind=pind(IC(w),ind));
 r=MAX(0,AR(w)-1); s=1+AS(w); t=AT(w); c=aii(w);
 ASSERT(!(t&SPARSE),EVNONCE);
 ASSERT(r==AR(ind),EVRANK);
 ASSERT(!ICMP(s,AS(ind),r),EVLENGTH);
 GA(z,t,c,r,s); x=CAV(z); v=CAV(w); u=AV(ind); k=bpnoun(t);
 DO(c, MC(x+k*i,v+k*(i+c*u[i]),k););
 EPILOG(z);
}
*/

#define MCASE(t,k)  ((t)+4*(k))
#define MINDEX        {j=*u++; if(0>j)j+=m; ASSERT(BETWEENO(j,0,m),EVINDEX);}

// m} y
static A jtmerge1(J jt,A w,A ind){A z;B*b;C*wc,*zc;D*wd,*zd;I c,it,j,k,m,r,*s,t,*u,*wi,*zi;
 ARGCHK2(w,ind);
 r=MAX(0,AR(w)-1); s=1+AS(w); t=AT(w); k=bpnoun(t); SETIC(w,m); c=aii(w);  // m = # items of w
 ASSERT(!(t&SPARSE),EVNONCE);
 ASSERT(r==AR(ind),EVRANK);
 ASSERTAGREE(s,AS(ind),r);
 GA(z,t,c,r,s);
 if(!(AT(ind)&B01+INT))RZ(ind=cvt(INT,ind));
 it=AT(ind); u=AV(ind); b=(B*)u;
 ASSERT((-c&(m-2))>=0||(!c||(m==1&&!memchr(b,C1,c))),EVINDEX);  // unless items are empty, m must have items.  if m is 1 all selectors must be 0.  INT will be checked individually, so we just look at the first c bytes
 zi=AV(z); zc=(C*)zi; zd=(D*)zc;
 wi=AV(w); wc=(C*)wi; wd=(D*)wc;
 switch(MCASE(CTTZ(it),k)){
  case MCASE(B01X,sizeof(C)): DO(c,         *zc++=wc[i+c*(I)*b++];); break;
  case MCASE(B01X,sizeof(I)): DO(c,         *zi++=wi[i+c*(I)*b++];); break;
#if !SY_64
  case MCASE(B01X,sizeof(D)): DO(c,         *zd++=wd[i+c*(I)*b++];); break;
#endif
  case MCASE(INTX,sizeof(C)): DO(c, MINDEX; *zc++=wc[i+c*j];); break;
  case MCASE(INTX,sizeof(I)): DO(c, MINDEX; *zi++=wi[i+c*j];); break;
#if !SY_64
  case MCASE(INTX,sizeof(D)): DO(c, MINDEX; *zd++=wd[i+c*j];); break;
#endif  
  default: if(it&B01)DO(c,         MC(zc,wc+k*(i+c*(I)*b++),k); zc+=k;)
           else      DO(c, MINDEX; MC(zc,wc+k*(i+c*j     ),k); zc+=k;); break;
 }
 // We modified w which is now not pristine.
 PRISTCLRF(w)
 R z;
}

#define CASE2Z(T)  {T*xv=(T*)AV(x),*yv=(T*)AV(y),*zv=(T*)AV(z); DO(n, zv[i]=(bv[i]?yv:xv)[i];); break;}
#define CASE2X(T)  {T*xv=(T*)AV(x),*yv=(T*)AV(y);               DO(n, if( bv[i])xv[i]=yv[i];); z=x; break;}
#define CASE2Y(T)  {T*xv=(T*)AV(x),*yv=(T*)AV(y);               DO(n, if(!bv[i])yv[i]=xv[i];);  z=y; break;}
#define CASENZ(T)  {T*zv=(T*)AV(z); DO(n, j=iv[i]; if(0>j){j+=m; ASSERT(0<=j,EVINDEX);}else ASSERT(j<m,EVINDEX);  \
                       zv[i]=*(i+(T*)aa[j]);); break;}

// Handle the case statement abc =: pqr} x,...,y,:z, with in-place operation if pqr is Boolean and abc appears on the right
F1(jtcasev){A b,*u,*v,w1,x,y,z;B*bv,p,q;I*aa,c,*iv,j,m,n,r,*s,t;
 ARGCHK1(w);
 RZ(w1=ca(w)); u=AAV(w1);   // make a copy of the input, point to its value
 // the input is a boxed list.  The last 3 values are (name pqr);(index in which abc appeared in the x,y,... or -1 if it didn't);(original sentence queue)
 p=1; m=AN(w)-3; v=AAV(w); c=i0(v[m+1]);   // get # items in list, and index of the matching one
 // Now audit the input names (including pqr), since we haven't properly stacked them & checked them etc.
 // p is set to 0 if an audit fails
 DO(m+1, x=symbrd(v[i]); if(!x){p=0; RESETERR; break;} u[i]=x; p=p&!!(NOUN&AT(x)););  // verify names defined, and are nouns
 if(p){
  b=u[m]; n=AN(b); r=AR(b); s=AS(b); t=AT(*u);  // length, rank, shape, of pqr; type of first value in list
  p=t&DIRECT&&AT(b)&NUMERIC;    // fail if first value in list is indirect or pqr is not numeric
  if(p)DO(m, y=u[i]; if(!(TYPESEQ(t,AT(y))&&r==AR(y)&&!ICMP(s,AS(y),r))){p=0; break;});  // fail if list is not homogeneous in type, rank, and shape
 }
 // If the audit failed, the sentence might work, but we won't be doing it here.  Go run the original sentence
 if(!p)R parse(v[m+2]);   // NOTE this will end up assigning the value twice: once in the parse, and again when we return.  Should we whack off the first two words?
 // We can do it here!  We split into two cases: Boolean pqr with two names in the list, which can never fail;
 // and all other, which may produce index error
 fauxblockINT(aafaux,4,1);
 if(q=2==m&&B01&AT(b)){bv=BAV(b); x=u[0]; y=u[1];}  // fast case: exactly two names x and y
 else{   // slow case
  if(!(INT&AT(b)))RZ(b=cvt(INT,b));  // convert pqr to int if it's not already
  iv=AV(b);    // iv points to the input pqr
  fauxINT(b,aafaux,m,1)  aa=AV(b); DO(m, aa[i]=(I)AV(u[i]););  // create b, which is a list of pointers to the values of the names
 }
 // Check to see if we can modify in-place.  We can do so only if abc was one of the two names on the right, and we have the
 // fast (no-error) case; and of course if the use-count is only 1.  But if the assignment is local, we also have to make
 // sure abc is locally defined
 if(p=q&&0<=c&&ACUC1>=AC(u[c])) {  // passes quick check
   p= (AN(jt->locsyms)==1) || CAV(AAV(v[m+2])[1])[0]!=CASGN || probe(NAV(AAV(v[m+2])[0])->m,NAV(AAV(v[m+2])[0])->s,NAV(AAV(v[m+2])[0])->hash, jt->locsyms);  // OK if not in explicit, or not local assignment, or name defined
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
 r=6; r=t&C4T?5:r;  r=t&C2T?4:r;  r=t&CMPX?3:r;  r=t&FL?2:r;  r=t&INT?1:r;  r=t&B01+LIT?0:r; 
 switch((!q?24:!p?0:c==0?8:16)+r){   // choose a comparison based on data
  case  0: CASE2Z(C);  case  1: CASE2Z(I);  case  2: CASE2Z(D);  case  3: CASE2Z(Z);
  case  4: CASE2Z(US); case  5: CASE2Z(C4); case  6: CASE2Z(I);
  case  8: CASE2X(C);  case  9: CASE2X(I);  case 10: CASE2X(D);  case 11: CASE2X(Z);
  case 12: CASE2X(US); case 13: CASE2X(C4); case 14: CASE2X(I);
  case 16: CASE2Y(C);  case 17: CASE2Y(I);  case 18: CASE2Y(D);  case 19: CASE2Y(Z);
  case 20: CASE2Y(US); case 21: CASE2Y(C4); case 22: CASE2Y(I);
  case 24: CASENZ(C);  case 25: CASENZ(I);  case 26: CASENZ(D);  case 27: CASENZ(Z);
  case 28: CASENZ(US); case 29: CASENZ(C4); case 30: CASENZ(I);
  default: ASSERTSYS(0,"casev");
 }
 // Mark all the inputs as nonpristine
 DO(m, w=u[i]; PRISTCLRF(w))
 RETF(z);
}   /* z=:b}x0,x1,x2,...,x(m-2),:x(m-1) */

// Handle a ind} w after indices have been converted to integer atoms, dense
// cellframelen is the number of axes of w that were used in computing the cell indexes
static A jtmerge2(J jt,A a,A w,A ind,I cellframelen){F2PREFIP;A z;I t;
 ARGCHK3(a,w,ind);
 ASSERT(HOMO(AT(a),AT(w))||(-AN(a)&-AN(w))>=0,EVDOMAIN);  // error if xy both not empty and not compatible
 ASSERT(AR(a)<=AR(w)+AR(ind)-cellframelen,EVRANK);   // max # axes in a is the axes in w, plus any surplus axes of m that did not go into selecting cells
 //   w w w w w
 // m m m . w w   the rank of m may be more or less than cellframelen which is the number of axes that are covered by m.  For single boxed m, AR(ind)=cellframelen
 // <---a   a a
 ASSERTAGREE(AS(a),AS(ind)+AR(ind)-MAX(0,AR(a)-(AR(w)-cellframelen)),MAX(0,AR(a)-(AR(w)-cellframelen)));  // shape of m{y is the shape of m, as far as it goes.  The first part of a may overlap with m
 ASSERTAGREE(AS(a)+MAX(0,AR(a)-(AR(w)-cellframelen)),AS(w)+AR(w)-(AR(a)-MAX(0,AR(a)-(AR(w)-cellframelen))),AR(a)-MAX(0,AR(a)-(AR(w)-cellframelen)));  // the rest of the shape of m{y comes from shape of y
 if(!AN(w))RCA(w);  // if y empty, return.  It's small.  Ignore inplacing
 t=AN(a)?maxtyped(AT(a),AT(w)):AT(w);  // get the type of the result: max of types, but if x empty, leave y as is
 if((-AN(a)&-TYPESXOR(t,AT(a)))<0)RZ(a=cvt(t,a));  // if a must change precision, do so
 // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the type is either
 // DIRECT or this is a boxed memory-mapped array; and don't inplace a =: a m} a or a =: x a} a
 // kludge this inplaces boxed mm arrays when usecount>2.  Seems wrong, but that's the way it was done
 // It is not possible to inplace a value that is backing a virtual block, because we inplace assigned names only when
 // the stack is empty, so if there is a virtual block it must be in a higher sentence, and the backing name must appear on the
 // stack in that sentence if the usecount is only 1.
 I ip = ASGNINPLACESGNNJA(SGNIF((I)jtinplace,JTINPLACEWX),w)
      &&( ((AT(w)&t&(DIRECT|RECURSIBLE))!=0)&(w!=a)&(w!=ind)&((w!=ABACK(a))|(~AFLAG(a)>>AFVIRTUALX)) );
 // if w is boxed, we have to make one more check, to ensure we don't end up with a loop if we do   (<a) m} a.  Force a to be recursive usecount, then see if the usecount of w is changed
 if(-ip&t&RECURSIBLE){
  I oldac = ACUC(w);  // remember original UC of w
  ra0(a);  // ensure a is recursive usecount.  This will be fast if a is one boxing level
  ip = AC(w)<=oldac;  // turn off inplacing if a referred to w
 } 

 if(ip){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}
 // If not inplaceable, create a new block (cvt always allocates a new block) with the common precision.  Relocate it if necessary.
 // after this, z cannot be virtual unless it is an inplace memory-mapped boxed array
 else{RZ(z=cvt(t,w));}
 // Could be in-place.  If boxed,
 // a has been forced to be recursive usecount, so any block referred to by a will not be freed by a free of w.
 // If w has recursive usecount, all the blocks referred to in w have had their usecount incremented; we must
 // free them before we overwrite them, and we must increment the usecount in the block we store into them
 // It is possible that the same cell of w will be written multiple times, so we do the fa-then-ra each time we store
 C* RESTRICT av0=CAV(a); I k=bpnoun(t); C * RESTRICT avn=av0+(AN(a)*k);
 // Extract the number of axes included in each cell offset; get the cell size
 I cellsize; PROD(cellsize,AR(w)-cellframelen,AS(w)+cellframelen);  // number of atoms per index in ind
 I *iv=AV(ind);  // start of the cell-index array
 if(UCISRECUR(z)){
  cellsize<<=(t>>RATX);  // RAT has 2 boxes per atom, all other recursibles have 1 and are lower
  {A * RESTRICT zv=AAV(z); A *RESTRICT av=(A*)av0; DO(AN(ind), I ix0=iv[i]*cellsize; DQ(cellsize, INSTALLBOXRECUR(zv,ix0,*av); ++ix0; ++av; av=(av==(A*)avn)?(A*)av0:av;))}
 }else{
  if(cellsize<=AN(a)){
   // there is more than one cell in a.  We can copy entire cells
   cellsize *= k;   // change cellsize to bytes
   switch(cellsize){
   case sizeof(C):
    {C * RESTRICT zv=CAV(z); C *RESTRICT av=(C*)av0; DO(AN(ind), zv[iv[i]]=*av; ++av; av=(av==(C*)avn)?(C*)av0:av;); break;}  // scatter-copy the data, cyclically
   case sizeof(I):  // may include D
    {I * RESTRICT zv=AV(z); I *RESTRICT av=(I*)av0; DO(AN(ind), zv[iv[i]]=*av; ++av; av=(av==(I*)avn)?(I*)av0:av;); break;}  // scatter-copy the data
   default: ;
     C* RESTRICT zv=CAV(z); C *RESTRICT av=(C*)av0; JMCDECL(endmask) JMCSETMASK(endmask,cellsize,1) DO(AN(ind), JMCR(zv+(iv[i]*cellsize),av,cellsize,loop1,1,endmask); av+=cellsize; av=(av==avn)?av0:av;);  // scatter-copy the data, cyclically.  Don't overwrite
   }
  }else{
   // the cellsize is bigger than a.  We will have to repeat a within each cell
   // We must repeat for each axis between the end of ind and the start of a
   I abytes=AN(a)*k;  // number of bytes in a
   cellsize *= k;   // change cellsize to bytes
   C* RESTRICT zv=CAV(z); DO(AN(ind), mvc(cellsize,zv+iv[i]*cellsize,abytes,av0); )  // scatter-copy the data, with repeat
  }
 }
 RETF(z);
}

// Convert list/table of indexes to a list of cell offsets (the number of the atom starting the cell)
// w has rank > 0.  Result has shape }:$ind
// This is used by ind} and <"1@[ { ]  which have different specs when ind is a list.  Here we follow the spec for m}, in which
// a list ind is treated like a table with rows of length 1
// The indexes are audited for validity and negative values
// This is like pind/aindex1 followed by pdt, but done in registers and without worrying about checks for overflow, since the result
// if valid will fit into an integer.  If there are no negative indexes, this method is just a teeny bit faster, because pind/aindex1 do one quick loop at full memory
// speed to validate the input, and pdt works well for a large number of short vectors - in particular it avoids the carried dependency between axes that
// Horner's Rule creates.  This version keeps things in registers and has less setup time; and it is much better if there are negative indexes.
A jtcelloffset(J jt,AD * RESTRICT w,AD * RESTRICT ind){A z;
 ARGCHK1(w);
 if(AR(ind)<2){RZ(z=pind(AS(w)[0],ind));  // (m}only) treat a list as a list of independent indexes.  pind handles that case quickly and possibly in-place.
 }else if(AS(ind)[AR(ind)-1]==1){RZ(z=pind(AS(w)[0],IRS1(ind,0L,2L,jtravel,z)));  // if rows are 1 long, pind handles that too - remove the last axis
 }else{
  // rank of ind>1, and rows of ind are longer than 1. process each row to a cell offset
  I naxes = AS(ind)[AR(ind)-1];
  I nzcells; PROD(nzcells,AR(ind)-1,AS(ind));
  RZ(ind=AT(ind)&INT?ind:cvt(INT,ind));  // w is now an INT vector, possibly the input argument
  ASSERT(naxes<=AR(w),EVLENGTH);  // length of rows of table must not exceed rank of w
  GATV(z,INT,nzcells,AR(ind)-1,AS(ind)); I *zv=IAV(z);  // allocate result area, point to first cell location.
  I *iv=IAV(ind);// point to first row
  // Do the verify/multiply depending on number of axes.
  if(naxes<3){
   // rank 2
   I ln0=AS(w)[0], ln1=AS(w)[1];
   DQ(nzcells, I in=iv[0]; if((UI)in>=(UI)ln0){in+=ln0; ASSERT((UI)in<(UI)ln0,EVINDEX);} I r=in*ln1;
                    in=iv[1]; if((UI)in>=(UI)ln1){in+=ln1; ASSERT((UI)in<(UI)ln1,EVINDEX);} r+=in;
                    *zv++=r; iv+=2;)
  }else if(naxes==3){
   // rank 3  Avoid Horner's Rule, which creates a carried dependency across the multiplies
   I ln0=AS(w)[0], ln1=AS(w)[1], ln2=AS(w)[2], ln12=ln1*ln2;
   DQ(nzcells, I in=iv[0]; if((UI)in>=(UI)ln0){in+=ln0; ASSERT((UI)in<(UI)ln0,EVINDEX);} I r=in*ln12;
                    in=iv[1]; if((UI)in>=(UI)ln1){in+=ln1; ASSERT((UI)in<(UI)ln1,EVINDEX);} r+=in*ln2;
                    in=iv[2]; if((UI)in>=(UI)ln2){in+=ln2; ASSERT((UI)in<(UI)ln2,EVINDEX);} r+=in;
                    *zv++=r; iv+=3;)
  }else{
   // rank 4+.  For simplicity we use Horner's Rule since this case is rare
   I ln0=AS(w)[0], ln1=AS(w)[1], lnn=AS(w)[naxes-1];
   DQ(nzcells, I in=iv[0]; if((UI)in>=(UI)ln0){in+=ln0; ASSERT((UI)in<(UI)ln0,EVINDEX);} I r=in*ln1;
                    in=iv[1]; if((UI)in>=(UI)ln1){in+=ln1; ASSERT((UI)in<(UI)ln1,EVINDEX);}
                    DO(naxes-3,                                               r=(r+in)*AS(w)[i+2];
                    in=iv[i+2]; if((UI)in>=(UI)AS(w)[i+2]){in+=AS(w)[i+2]; ASSERT((UI)in<(UI)AS(w)[i+2],EVINDEX);})
                                                                              r=(r+in)*lnn;
                    in=iv[naxes-1]; if((UI)in>=(UI)lnn){in+=lnn; ASSERT((UI)in<(UI)lnn,EVINDEX);} r+=in;
                    *zv++=r; iv+=naxes;)
  }
 }
 R z;
}

// Convert ind to a list of cell offsets.  Error if inhomogeneous cells.
// Result *cellframelen gives the number of axes of w that have been boiled down to indices in the result
static A jtjstd(J jt,A w,A ind,I *cellframelen){A j=0,k,*v,x;I b;I d,i,n,r,*u,wr,*ws;D rkblk[16];
 wr=AR(w); ws=AS(w); b=-AN(ind)&SGNIF(AT(ind),BOXX);  // b<0 = indexes are boxed and nonempty
 if(!wr){x=from(ind,zeroionei(0)); *cellframelen=0; R x;}  // if w is an atom, the best you can get is indexes of 0.  No axes are used
 if((b&-AR(ind))<0){   // array of boxed indexes
  RE(aindex(ind,w,0L,&j));  // see if the boxes are homogeneous
  if(!j){  // if not...
   RZ(x=MODIFIABLE(from(ind,increm(iota(shape(w)))))); u=AV(x); // go back to the original indexes, select from table of all possible incremented indexes; since it is incremented, it is writable
   DQ(AN(x), ASSERT(*u,EVDOMAIN); --*u; ++u;);   // if anything required fill, it will leave a 0.  Fail then, and unincrement the indexes
   *cellframelen=AR(w); R x;   // the indexes are what we want, and they include all the axes of w
  }
  // Homogeneous boxes.  j has them in a single table.  turn each row into an index
  b=0; ind=j;  // use the code for numeric array
  // later this can use the code for table m
 }
 if(b>=0){
  // Numeric m.  Each 1-cell is a list of indexes (if m is a list, each atom is a cell index)
  RZ(j=celloffset(w,ind));  // convert list/table to list of indexes, possibly in place
  n=AR(ind)<2?1:AS(ind)[AR(ind)-1];  // n=#axes used: 1, if m is a list; otherwise {:$m
 }else{  // a single box.
  ind=AAV(ind)[0]; n=AN(ind); r=AR(ind);  // ind etc now refer to the CONTENTS of the single box
  ASSERT((-(n|(r^1))&((AT(ind)&BOX+NUMERIC)-1))>=0,EVINDEX);  // must be empty list or numeric or boxed
  if(((n-1)|SGNIF(AT(ind),BOXX))>=0)RZ(ind=IRS1(ind,0,0,jtbox,j));  // if numeric, box each atom
  v=AAV(ind);   // now ind is a atom/list of boxes, one per axis
  ASSERT(1>=r,EVINDEX);  // not a table
  ASSERT(n<=wr,EVINDEX);  // not too many axes
  DQ(n, if(!equ(ds(CACE),v[i]))break; --n;);  // discard trailing (boxed) empty axes
  j=zeroionei(0);  // init list to a single 0 offset
  for(i=0;i<n;++i){  // for each axis, grow the cartesian product of the specified offsets
   x=v[i]; d=ws[i];
   if((-AN(x)&SGNIF(AT(x),BOXX))<0){   // notempty and boxed
    ASSERT(!AR(x),EVINDEX); 
    x=AAV(x)[0]; k=IX(d);
    if(AN(x))k=less(k,pind(d,1<AR(x)?ravel(x):x));
   }else k=pind(d,x);
   RZ(x=tymesA(j,sc(d)));
   RZ(j=ATOMIC2(jt,x,k,rkblk,0, RMAX,CPLUS));
  }
 }
 // now j is an array of offsets.  n is the number of axes that went into each atom of j.  Return the offsets
 // Because of the way ind was passed through a verb-compound in jtgav2, it cannot be virtual and thus it is safe to destroy AM.
 // It might be good to give a dispensation and allow virtual ind.  In that case, j might have survived this long and thus
 // be virtual.  In that case we must (1) realize it; (2) use the filler field (64-bit only) to pass back the number of axes; (3) use
 // a field in jt to pass back the number of axes.
 *cellframelen=n; R j;  // insert the number of axes used in each cell of j
}    /* convert ind in a ind}w into integer atom-offsets */

/* Reference count for w for amend in place */
/* 1 jdo     tpop                           */
/* 2 amendn2 EPILOG/gc                      */
/* 1 jdo     tpop                           */

// Execution of x m} y.  Split on sparse/dense, passing on the dense to merge2, including inplaceability
static DF2(jtamendn2){F2PREFIP;PROLOG(0007);A e,z; B b;I atd,wtd,t,t1;P*p;
 AD * RESTRICT ind=VAV(self)->fgh[0];
 ARGCHK3(a,w,ind);
 if(likely(!((AT(w)|AT(ind))&SPARSE))){
  I cellframelen; ind=jstd(w,ind,&cellframelen);   // convert indexes to cell indexes; remember how many were converted
  z=jtmerge2(jtinplace,AT(a)&SPARSE?denseit(a):a,w,ind,cellframelen);  //  dense a if needed; dense amend
  // We modified w which is now not pristine.
  PRISTCLRF(w)
  EPILOG(z);
 }
 // Otherwise, w is sparse
 // ?t = underlying type of ?, s?=nonzero if sparse
 atd=AT(a)&SPARSE?DTYPE(AT(a)):AT(a); wtd=AT(w)&SPARSE?DTYPE(AT(w)):AT(w);
 ASSERT(AT(ind)&NUMERIC+BOX||!AN(ind),EVDOMAIN);
 ASSERT(AT(ind)&DENSE,EVNONCE);  // m must be dense, and numeric or boxed
 // Sparse w.  a and t must be compatible; sparse w must not be boxed
 ASSERT(!(wtd&BOX),EVNONCE); ASSERT(HOMO(atd,wtd),EVDOMAIN);
 // set t to dense precision of result; t1=corresponding sparse precision; convert a if need be.  Change a's type but not its sparseness
 RE(t=maxtyped(atd,wtd)); t1=STYPE(t); RZ(a=TYPESEQ(t,atd)?a:cvt(AT(a)&SPARSE?t1:t,a));
 // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the dense type is either
 // DIRECT or this is a boxed memory-mapped array
 B ip=((I)jtinplace&JTINPLACEW) && (ACIPISOK(w) || jt->assignsym&&jt->assignsym->val==w&&(AC(w)<=1||(AFNJA&AFLAG(w)&&AC(w)==2)))
     &&TYPESEQ(t,wtd)&&(t&DIRECT);
 // see if inplaceable.  If not, convert w to correct precision (note that cvt makes a copy if the precision is already right)
 if(ip){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}else RZ(z=cvt(t1,w));
 // call the routine to handle the sparse amend
 p=PAV(z); e=SPA(p,e); b=!AR(a)&&equ(a,e);
 p=PAV(a); if(unlikely(AT(a)&SPARSE&&!equ(e,SPA(p,e)))){RZ(a=denseit(a)); }
 if(AT(ind)&NUMERIC||!AR(ind))z=(b?jtam1e:AT(a)&SPARSE?jtam1sp:jtam1a)(jt,a,z,AT(ind)&NUMERIC?box(ind):ope(ind),ip);
 else{RE(aindex(ind,z,0L,(A*)&ind)); ASSERT(ind!=0,EVNONCE); z=(b?jtamne:AT(a)&SPARSE?jtamnsp:jtamna)(jt,a,z,ind,ip);}  // A* for the #$&^% type-checking
 EPILOGZOMB(z);   // do the full push/pop since sparse in-place has zombie elements in z
}

// Execution of x u} y.  Call u to get the indices, then
// call merge2 to do the merge.  Pass inplaceability into merge2.
static DF2(amccv2){F2PREFIP;DECLF; 
 ARGCHK2(a,w); 
 ASSERT(DENSE&AT(w),EVNONCE);  // u} not supported for sparse
 A x;RZ(x=pind(AN(w),CALL2(f2,a,w,fs)));
 A z=jtmerge2(jtinplace,a,w,x,AR(w));   // The atoms of x include all axes of w, since we are addressing atoms
 // We modified w which is now not pristine.
 PRISTCLRF(w)
 RETF(z);
}


static DF1(mergn1){       R merge1(w,VAV(self)->fgh[0]);}
static DF1(mergv1){DECLF; R merge1(w,CALL1(f1,w,fs));}

// called from m}, m is usually NOT a gerund
static B ger(A w){A*wv,x;
 if(!(BOX&AT(w)))R 0;
 wv=AAV(w); 
 DO(AN(w), x=wv[i]; if((-(BOX&AT(x))&(((AR(x)^1)|(AN(x)^2))-1))<0)x=AAV(x)[0]; if(((-(LIT&AT(x))&(AR(x)-2)&-AN(x)))>=0)R 0;);  // box/rank1/N=2; lit/R<2/N!=0
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
   R (p>>7)|!!ds(p);  // return if valid primitive (all non-ASCII are valid primitives, but 0: is not in pst[] so force that in)
  }
 } else if(AT(w)&BOX) {A *wv;I bmin=0,bmax=0;
  // boxed contents.  There must be exactly 2 boxes.  The first one may be a general AR; or the special cases singleton 0, 2, 3, or 4
  // Second may be anything for special case 0 (noun); otherwise must be a valid gerund, 1 or 2 boxes if first box is general AR, 2 boxes if special case
  // 2 (hook) or 4 (bident), 3 if special case 3 (fork)
  // 
  if(!(n==2))R 0;  // verify 2 boxes
  wv = AAV(w);  x=wv[0]; // point to pointers to boxes; point to first box contents
  // see if first box is a special flag
  if((SGNIF(AT(x),LITX)&(AR(x)-2)&((AN(x)^1)-1))<0){ // LIT, rank<2, AN=1
   c = CAV(x)[0];   // fetch that character
   if(c=='0')R 1;    // if noun, the second box can be anything & is always OK, don't require AR there
   I oride=2+(c&1);  // 2 if '2'/'4', 3 if '3'
   bmin=BETWEENC(c,'2','4')?oride:bmin; bmax=BETWEENC(c,'2','4')?oride:bmax; 
  }
  // If the first box is not a special case, it had better be a valid AR; and it will take 1 or 2 operands
  if(bmin==0){if(!(gerar(jt,x)))R 0; bmin=1,bmax=2;}
  // Now look at the second box.  It should contain between bmin and bmax boxes, each of which must be an AR
  x = wv[1];   // point to second box
  if((SGNIF(AT(x),BOXX) & ((AR(x)^1)-1))>=0)R 0;   // verify it contains a list of boxes
  if(!BETWEENC(AN(x),bmin,bmax))R 0;  // verify correct number of boxes
  R gerexact(x);  // recursively audit the other ARs in the second box
 } else R 0;
 R 1;
}

B jtgerexact(J jt, A w){A*wv;
 if(!(BOX&AT(w)))R 0;   // verify gerund is boxed
 if(!(AN(w)))R 0;   // verify there are boxes
 wv = AAV(w);   // point to pointers to contents
 DO(AN(w), if(!(gerar(jt, wv[i])))R 0;);   // fail if any box contains a non-gerund
 R 1;
}    /* 0 if w is definitely not a gerund; 1 if possibly a gerund */


// u} handling.  This is not inplaceable but the derived verb is
F1(jtamend){
 ARGCHK1(w);
 if(VERB&AT(w)) R ADERIV(CRBRACE,mergv1,amccv2,VASGSAFE|VJTFLGOK2, RMAX,RMAX,RMAX);  // verb} 
 else if(ger(w))R gadv(w,CRBRACE);   // v0`v1`v2}
 else           R ADERIV(CRBRACE,mergn1,jtamendn2,VASGSAFE|VJTFLGOK2, RMAX,RMAX,RMAX);  // m}
}

static DF2(jtamen2){ASSERT(0,EVNONCE);}

F1(jtemend){
 ASSERT(NOUN&AT(w),EVDOMAIN);
 R ADERIV(CEMEND,0L,jtamen2,VFLAGNONE, RMAX,RMAX,RMAX);
}
