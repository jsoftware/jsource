/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Amend                                                          */

#include "j.h"


#define MCASE(t,k)  ((t)+4*(k))
#define MINDEX        {j=*u++; if(0>j)j+=m; ASSERT(BETWEENO(j,0,m),EVINDEX);}

// m} y
static A jtmerge1(J jt,A w,A ind){A z;B*b;C*wc,*zc;D*wd,*zd;I c,it,j,k,m,r,*s,t,*u,*wi,*zi;
 ARGCHK2(w,ind);
 r=MAX(0,AR(w)-1); s=1+AS(w); t=AT(w); k=bpnoun(t); SETIC(w,m); c=aii(w);  // m = # items of w
 ASSERT(!ISSPARSE(t),EVNONCE);
 ASSERT(r==AR(ind),EVRANK);
 ASSERTAGREE(s,AS(ind),r);
 GA(z,t,c,r,s);
 if(!ISDENSETYPE(AT(ind),B01+INT))RZ(ind=cvt(INT,ind));
 it=AT(ind); u=AV(ind); b=(B*)u;
 ASSERT((-c&(m-2))>=0||(!c||(m==1&&!memchr(b,C1,c))),EVINDEX);  // unless items are empty, m must have items.  if m is 1 all selectors must be 0.  INT will be checked individually, so we just look at the first c bytes
 zi=AV(z); zc=(C*)zi; zd=(D*)zc;
 wi=AV(w); wc=(C*)wi; wd=(D*)wc;
 switch(MCASE(CTTZ(it),k)){
  case MCASE(B01X,sizeof(I)): DO(c,         *zi++=wi[i+c*(I)*b++];); break;
  case MCASE(B01X,sizeof(C)): DO(c,         *zc++=wc[i+c*(I)*b++];); break;
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
 // the input is a boxed list.  The last 3 values are (name pqr);(index in which abc appeared in the x,y,... or -1 if it didn't);(original sentence queue including flags)
 p=1; m=AN(w)-3; v=AAV(w); c=i0(v[m+1]);   // get # items in list, and index of the matching one
 // Now audit the input names (including pqr), since we haven't properly stacked them & checked them etc.
 // p is set to 0 if an audit fails
 DO(m+1, x=symbrd(v[i]); if(!x){p=0; RESETERR; break;} u[i]=x; p=p&!!(NOUN&AT(x)););  // verify names defined, and are nouns
 if(p){
  b=u[m]; n=AN(b); r=AR(b); s=AS(b); t=AT(*u);  // length, rank, shape, of pqr; type of first value in list
  p=(t&DIRECT)>0&&AT(b)&NUMERIC;    // fail if first value in list is indirect or pqr is not numeric
  if(p)DO(m, y=u[i]; if(!(TYPESEQ(t,AT(y))&&r==AR(y)&&!ICMP(s,AS(y),r))){p=0; break;});  // fail if list is not homogeneous in type, rank, and shape
 }
 // If the audit failed, the sentence might work, but we won't be doing it here.  Go run the original sentence
 if(!p)R PARSERVALUE(parse(v[m+2]));   // NOTE this will end up assigning the value twice: once in the parse, and again when we return.  Should we whack off the first two words?
 // We can do it here!  We split into two cases: Boolean pqr with two names in the list, which can never fail;
 // and all other, which may produce index error
 fauxblockINT(aafaux,4,1);
 if(q=2==m&&B01&AT(b)){bv=BAV(b); x=u[0]; y=u[1];}  // fast case: exactly two names x and y
 else{   // slow case
  if(!ISDENSETYPE(AT(b),INT))RZ(b=cvt(INT,b));  // convert pqr to int if it's not already
  iv=AV(b);    // iv points to the input pqr
  fauxINT(b,aafaux,m,1)  aa=AV(b); DO(m, aa[i]=(I)AV(u[i]););  // create b, which is a list of pointers to the values of the names
 }
 // Check to see if we can modify in-place.  We can do so only if abc was one of the two names on the right, and we have the
 // fast (no-error) case; and of course if the use-count is only 2 (incremented by the call to symbrd from 1).  But if the assignment is local, we also have to make
 // sure abc is locally defined
 if(p=q&&0<=c&&AC(u[c])<=ACUC2) {  // passes quick check
   p= (!EXPLICITRUNNING) || CAV(QCWORD(AAV(v[m+2])[1]))[0]!=CASGN || jtprobe((J)((I)jt+NAV(QCWORD(AAV(v[m+2])[0]))->m),NAV(QCWORD(AAV(v[m+2])[0]))->s,NAV(QCWORD(AAV(v[m+2])[0]))->hash, jt->locsyms);  // OK if not in explicit, or not local assignment, or name defined
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
 DO(m, w=C(u[i]); PRISTCLRF(w))
 RETF(z);
}   /* z=:b}x0,x1,x2,...,x(m-2),:x(m-1) */

#define AMFLAGRTNX 0
#define AMFLAGRTN ((I)1<<AMFLAGRTNX)  // switch routine: 0llddd: cells have length 1<<ll with duff offset ddd; 1ll0dd single cell of length 1<<ll, duff offset dd;
                                 // 10b1xx non-atomic length cell, xx is 00=multiple cells of a, 01=single cell of a (length abytes) repeated in each result cell; 10=a is a single cell of length abytes=cellsize not repeated.  When xx is x0, b=0 means cellsize is a multiple of SZI
                                 // 1101xx recursive types, xx is 00=multiple cells of a, 01=single cell of a (length abytes) repeated in each result cell; 1y=a is a single cell of length abytes=(cellsize<<lgk), with y=1 if abytes=SZI
#define AMFLAGINFULL1X 8
#define AMFLAGINFULL1 ((I)1<<AMFLAGINFULL1X)  // axis 1 is taken in full
#define AMFLAGDUFFWX 16
#define AMFLAGDUFFW ((I)1<<AMFLAGDUFFWX)  // duff backoff for w, 0-7
#define AMFLAGDUFFAX 24
#define AMFLAGDUFFA ((I)1<<AMFLAGDUFFAX)  // duff backoff for A, 0-7.  Must be highest field
 struct axis{
  UI size;  // First, the length of this axis in the result.  Later, size in bytes of a cell of this axis
  C *base;  //  pointer to cell of higher axis that this axis is indexing within.  base for axes i does not include index i.
  I max;  // number of indexes for this axis (if in full, total axis length).  Set to -1 at init to indicate the axis was an atom
  I *indexes;  // pointer to index array, or 0 is axis is taken in full
  UI scan;  // index number being worked on currently
  I resetadder;  // #bytes to add to av when this axis rolls over (used to tell how to repeat a).  For ind forms, always resets after last axis; for axis forms, resets after a rollover of the lowest
                  // axis whose cell size is as big as a.  But if a single atom is repeated, always 0 to avoid destroying the value
 };
// Handle a ind} w after indices have been converted to integer atom indexes, dense
// cellframelen is the number of axes of w that were used in computing the cell indexes, complemented if ind is axes
// ind is the assembled indices OR a pointer to axes[]
static A jtmerge2(J jt,A a,A w,A ind,I cellframelen){F2PREFIP;A z;I t;
 ARGCHK2(a,w); RZ(ind);
 I surplusind=AR(ind)-cellframelen; surplusind=cellframelen<0?0:surplusind;  // get # extra axes in ind beyond the ones that selected cells.  for axes, all axes select
 ASSERT(AR(a)<=AR(w)+surplusind,EVRANK);   // max # axes in a is the axes in w, plus any surplus axes of m that did not go into selecting cells
 //   w w w w w
 // m m m . w w   the rank of m may be more or less than cellframelen which is the number of axes that are covered by m.  For single boxed m, AR(ind)=cellframelen
 // <---a   a a
 // audit shape of a: must be a suffix of (shape of ind),(shape of selected cell)
 I compalen;  // len of shape of a that has been compared
 if(cellframelen>=0){  // ind is indexes
  compalen=MAX(0,AR(a)-(AR(w)-cellframelen));  // #axes of a that are outside of the cell in w
  ASSERTAGREE(AS(a),AS(ind)+AR(ind)-compalen,compalen);  // shape of m{y is the shape of m, as far as it goes.  The first part of a may overlap with m
 }else{  // ind is axes.  The cell-frame part of the shape of a must be a suffix of the cell-frame part of axes
  I aaxis=compalen=MAX(0,AR(a)-(AR(w)-~cellframelen));  // #axes of a that are outside of the cell in w
  // scan through the frame of ind, backwards.  If an axis has max<0, that was a selector that contained an atom: it disappeared from the shape of m{y and we skip it here (while resetting its len to +1).
  // compare the unskipped axes of ind with those of a.  Stop when a is exhausted.  If a has surplus axes, that's an error
  DQ(~cellframelen, if(((struct axis*)ind)[i+1].max<0)((struct axis*)ind)[i+1].max=1; else if(aaxis){ASSERT(((struct axis*)ind)[i+1].max==AS(a)[--aaxis],EVLENGTH)})
  ASSERT(aaxis==0,EVRANK);  // error if unmatched axes of a
 }
 ASSERTAGREE(AS(a)+compalen,AS(w)+AR(w)-(AR(a)-compalen),AR(a)-compalen);  // the rest of the shape of m{y comes from shape of y
 if(unlikely(!AN(w)))RCA(w);  // if y empty, return.  It's small.  Ignore inplacing
 ASSERT(HOMO(AT(a),AT(w))||(-AN(a)&-AN(w))>=0,EVDOMAIN);  // error if xy both not empty and not compatible
 t=AN(a)?maxtyped(AT(a),AT(w)):AT(w);  // get the type of the result: max of types, but if x empty, leave y as is
 if((-AN(a)&-TYPESXOR(t,AT(a)))<0)RZ(a=cvt(t,a));  // if a must change precision, do so
 // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the type is either
 // DIRECT or this is a boxed memory-mapped array; and don't inplace a =: a m} a or a =: x a} a
 // kludge this inplaces boxed mm arrays when usecount>2.  Seems wrong, but that's the way it was done
 // It is not possible to inplace a value that is backing a virtual block, because we inplace assigned names only when
 // the stack is empty, so if there is a virtual block it must be in a higher sentence, and the backing name must appear on the
 // stack in that sentence if the usecount is only 1.
 I ip = ASGNINPLACESGNNJA(SGNIF((I)jtinplace,JTINPLACEWX),w)
      &&( ((AT(w)&t&(DIRECT|RECURSIBLE))>0)&(w!=a)&(w!=ind)&((w!=ABACK(a))|(~AFLAG(a)>>AFVIRTUALX)) );
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
 I lgk=bplg(t);
 // Extract the number of axes included in each cell offset; get the cell size
 I cellsize; PROD(cellsize,AR(w)-(REPSGN(cellframelen)^cellframelen),AS(w)+(REPSGN(cellframelen)^cellframelen));  // number of atoms per index in ind
 if(unlikely(cellsize==0))RCA(w);  // nothing to move - exit so we can use UNTIL loops to move
 // initialization
 // if ind is axes, it has already been set up
 // if not, we treat it as a 1xAN(ind) array
 I avnreset=-AN(a)<<lgk;  // amount to add to an av that has overrun to get it back to av0; after setup, the reset adder for axis -1
 C * RESTRICT av=CAV(a);
 JMCDECL(endmask)
 I n0,n1;  // # of iterations of axis -1,-2
 I *scan0, scan0reset;  // pointer to first index of last axis OR first index of ind; and the amount to add after an iteration to either continue or reset
 I *scan1;  // pointer to index of _2-cell being filled
 C *base;  // address of _1-cell being filled
 I r;  // #axes, +1.  axes[r] is the last axis
 struct axis *axes, localaxes[3];  // pointer to all axes; block to use for local axes
 I amflags;  // flags and routine number to call in loop
 if(cellframelen>=0){  // ind is an array of 'fetch' indices, of arbitrary rank, construed as a 1-or 2-d array
  // the atoms of ind address cells of the entire w, of size cellsize.  We arbitrarily treat these as a 2-d array where each row of ind
  // matches one full application of a.  a is then repeated over the rows of ind
  // n0 is the length of a 'row' of ind, n1 is the # rows
  // first, handle the special cases of 0/1 cell.  We must handle 0 so that we can use UNTIL loops; we choose to handle 1
  // because it's common & avoids a lot of setup
  if(AN(ind)<2){  // 0/1 cell to move
   if(unlikely(AN(ind)==0))RCA(w);  // nothing to store - exit so we can use UNTIL loops to move
   if(!UCISRECUR(z)){mvc(cellsize<<lgk,CAV(z)+IAV(ind)[0]*(cellsize<<lgk),AN(a)<<lgk,CAV(a)); // copy in all of a, repeating as needed to fill the cell
   }else{cellsize<<=(t>>RATX); I ix0=IAV(ind)[0]*cellsize; I as=AN(a)<<(t>>RATX); base=CAV(z); do{A *avv=AAV(a); DQU(as, INSTALLBOXNVRECUR(((A*)base),ix0,*avv) ++ix0; ++avv;)}while(cellsize-=as);  // recursive z: increment usecount while installing
   }
   RETF(z);  // return the result
  }
  PROD(n0,MAX(0,AR(a)-(AR(w)-cellframelen)),AS(ind)+AR(ind)-MAX(0,AR(a)-(AR(w)-cellframelen)))  // axes shared by a and ind
  PROD(n1,AR(ind)-MAX(0,AR(a)-(AR(w)-cellframelen)),AS(ind))  // frame of ind
  if(unlikely(n0*n1==0))RCA(w);  // nothing to store - exit so we can use UNTIL loops to move
  // set size of _2-cell to 0 so that 'successive' cells stay on the array in full
  r=2;  // general ind is processed as if rank 2
  axes=localaxes;  // use the temp block for ind version
  scan0=IAV(ind);  // store pointer to the index list where we will use it - as indexes of last axis
  base=axes[0].base=CAV(z);  // the array in full is in play
  // scan1 is not used
  scan0reset=0;  // index pointer rolls along between iterations of 'axis -2'
  amflags=AMFLAGINFULL1;  // there are no indexes for axis 1, so we say it is 'taken in full' to avoid fetching indexes...
  axes[r-1].size=0;  // ... and set size=0 to keep base unchanged between _2-cells
 }else{  // 'ind' is actually orthogonal axes, filled in in terms of atoms
  axes=(struct axis *)ind;  // use the input block for all the axes
  // now that we have the size of an atom, convert the cell-sizes-in-atoms to bytes
  // set the reset value at the correct level (if any)
  r=~cellframelen;  // number of axes to process.  [0] is the array in full, [r] is the last zxis
  n0=axes[r].max; n1=axes[r-1].max;  // # iterations left on last 2 axes
  scan0=axes[r].indexes; scan0reset=-n0;  // point to the indexes for the last axis (NEVER 0 meaning 'taken in full' which were discarded earlier); reset to start of indexes after each iteration
  base=axes[0].base=CAV(z);  // the array in full is in play
  // roll up cell sizes
  I prevsize=cellsize<<lgk;  //  size of cell of previous axis, in bytes.  Init to size of cell of last axis
  I axisressize=prevsize;  // size of full result at this axis: following axis result size*number of indexes at this axis.  When this is = size of a, we must wrap a
  DQ(r, I tsz=axes[i+1].size; axes[i+1].size=prevsize; prevsize*=tsz; axisressize*=axes[i+1].max; if(axisressize==-avnreset){axes[i+1].resetadder=avnreset; avnreset=0;}) // size is size of 1 cell; if overall size is FIRST as big as a, reset after axis exhausted
  if(unlikely(axisressize==0))RCA(w);  // if nothing to move, exit so we can use UNTIL loops.  Have to wait till here for error-checking
  avnreset=axes[r].resetadder;  // move last-axis reset to known location for comp ease
  // create the starting base values for all axes.  base starts with the 'cell' for the array in full, which is the base of axis 0
  DO(r-1,
   axes[i+1].base=base;  // previous selected cell is the base of the next axis
   UI *andx=scan1=axes[i+1].indexes; andx=andx?andx:&axes[i+1].scan;  // point to next value; if indexes==0, taken in full, use the scan (=0) as the index
   base+=*andx*axes[i+1].size;  // get address of cell being worked on, for the next axis
  )
  // now base points to the base cell for axis -1, which includes the offset from the first index in axis -2.  scan1 has the address of the indexes for axis -2
  amflags=(scan1==0)<<AMFLAGINFULL1X; ++scan1;  // Remember if axis -2 taken in full.  the calculation of base was a prefetch of the first index in axis -2.  Increment to the next position if any.  scan1 is unused if axis taken in full
 }

 // select routine to use based on argument size/type
 // cellsize is now atoms; change it to bytes when we have the routine, EXCEPT for recursive types which stay as atoms
 I abytes=AN(a)<<lgk;  // number of bytes in a
 if(!UCISRECUR(z)){
  // not replacement of indirect blocks
  if(cellsize<=AN(a)){
   // a is at least as big as a cell being replaced
   if(((LOWESTBIT(cellsize)-cellsize)|(SZI-(cellsize<<lgk)))>=0){  // if cellsize is power of 2 and total cell is <= 8 bytes
    // the cell can be moved as a primitive type.  We will use a Duff loop.  Calculate the offets and index, which depends on whether the cell is repeated
    C *avv=(C*)*(A*)av;  I repeatatom=cellsize==AN(a); av=cellsize==AN(a)?avv:av; avnreset=cellsize==AN(a)?0:avnreset;  // repeatatom is 1 if the single atom is repeated.  In that case, replace av with the atom itself and clear the reset adder so it doesn't change
    I duffmask=0x7>>repeatatom;  // for nonrepeated cells, duff loop is 8 cells; for repeated atom, 4
    I backupct=(-n0)&duffmask;  //  duff backup
    n0=(n0+duffmask)>>(3-repeatatom);  // convert n0 into # turns through duff loop, giving 8/4 cells per turn
    amflags|=CTTZI(cellsize<<lgk)*8 + (repeatatom<<5) + (backupct*((1+AMFLAGDUFFW+AMFLAGDUFFA)^(repeatatom<<AMFLAGDUFFAX)));  // routine is 0brccDdd (r=repeat, cc=cellsize, ddd=duff backoff, D=0 if repeat)
   }else{
    // cell is not 1-, 2-, 4-, or 8-size. We will have to use move loops.  No duff
    I mvnoinc=cellsize==AN(a);  // 1 if a contains a single cells, and thus must not increment address after the move
    I mvbytelen=((cellsize<<lgk)&(SZI-1))!=0;  // 1 if cells are not word multiples, in which case we must not overstore
    amflags|=0b100100+(mvnoinc<<1)+(mvbytelen<<3);  // select appropriate code
    avnreset&=mvnoinc-1;  // if increment is supressed, also clear avnreset
    JMCSETMASK(endmask,cellsize<<lgk,mvbytelen)   // prepare for repeated move
   }
  }else{  // a must be repeated to fill a replaced cell
   amflags|=0b100101; avnreset=0; // use repeating code, with no duff bias.  Suppress reset of av, since it is never incremented
  }
  cellsize<<=lgk;  // convert cellsize to bytes for the rest of the processing
 }else{
  // replacing indirect blocks
  // cases: multiple cells of a; single cell of a, repeated; single cell of a, duped; single atom of a, repeated
  I repa=cellsize>=AN(a);  // 1 if a has a single cell
  I nodupa=cellsize<=AN(a);  // 1 if the single cell of a must be duplicated in each replaced cell
  avnreset&=repa-1;  // if a has a single cell, don't increment its address/value
  amflags|=0b110011+2*repa+nodupa;  // 110100=incremented a; 110110=single cell of a; 110101=single cell of a, duped
  I oneatom=abytes==(nodupa<<=LGSZI); C *avv=(C*)*(A*)av; av=oneatom?avv:av; amflags|=oneatom;   // if a is one nonduplicated atom, fetch the atom and set code to 110111
  cellsize<<=(t>>RATX);  // RAT has 2 boxes per atom, all other recursibles have 1 and are lower.  Leave cellsize as #indirect references
 }

// macros for copying axis -1
#define CP11(t)  /* each index copies a different cell to the result */ \
do{ \
 case (CTTZI(sizeof(t)))*8+0: ((t*)base)[scan0[0]]=((t*)av)[0]; case (CTTZI(sizeof(t)))*8+1: ((t*)base)[scan0[1]]=((t*)av)[1];  /* copy cells */ \
 case (CTTZI(sizeof(t)))*8+2: ((t*)base)[scan0[2]]=((t*)av)[2]; case (CTTZI(sizeof(t)))*8+3: ((t*)base)[scan0[3]]=((t*)av)[3]; \
 case (CTTZI(sizeof(t)))*8+4: ((t*)base)[scan0[4]]=((t*)av)[4]; case (CTTZI(sizeof(t)))*8+5: ((t*)base)[scan0[5]]=((t*)av)[5]; \
 case (CTTZI(sizeof(t)))*8+6: ((t*)base)[scan0[6]]=((t*)av)[6]; case (CTTZI(sizeof(t)))*8+7: ((t*)base)[scan0[7]]=((t*)av)[7]; \
 scan0+=8; av=(C*)((t*)av+8);  /* advance pointers */ \
}while(--i0);
#define CP1n(t)  /* each index copies the same cell to the result */ \
do{ \
 case (4+CTTZI(sizeof(t)))*8+0: ((t*)base)[scan0[0]]=(t)(I)av; case (4+CTTZI(sizeof(t)))*8+1: ((t*)base)[scan0[1]]=(t)(I)av;  /* copy cells */ \
 case (4+CTTZI(sizeof(t)))*8+2: ((t*)base)[scan0[2]]=(t)(I)av; case (4+CTTZI(sizeof(t)))*8+3: ((t*)base)[scan0[3]]=(t)(I)av; \
 scan0+=4;  /* advance pointers - but av is the value and does not advance */ \
}while(--i0);
#define CP1xv(bytelen,inc) case 0b100110+(bytelen<<3)-(inc<<1): DQNOUNROLL(n0, JMCR(base+cellsize**scan0++,av,cellsize,bytelen,endmask) if(inc)av+=cellsize;)  // copy 1 odd-length cell, advancing pointers to next input & output
#define CPn1v case 0b100101: i0=n0;  /* no duff loop */ do{mvc(cellsize,base+*scan0++*cellsize,abytes,av);}while(--i0);  /* each index replicates the same cell to fill result; don't increment input address */
#define CP11recur case 0b110100: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,*(A*)av) ++ix0; av+=sizeof(A*);)) }  // install *av++
#define CPn1recur case 0b110101: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; I cs=cellsize; do{A *avv=(A*)av; DQU(abytes>>LGSZI, INSTALLBOXNVRECUR(((A*)base),ix0,*avv) ++ix0; ++avv;)}while(cs-=abytes>>LGSZI);) }  // repeatedly install *av to fill single cell, don't advance av
#define CP1nrecur case 0b110110: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; A *avv=(A*)av; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,*avv) ++ix0; ++avv;)) }  // install *av, don't advance av
#define CP1narecur case 0b110111: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,(A)av) ++ix0;)) }  // install av repeatedly without refetching

 // scatter-copy the data
 while(1){
  // loop over each combination of the last 2 axes
  if(--n1==0)goto skippre;
  do{
   C *basepre=amflags&AMFLAGINFULL1?base+axes[r-1].size:axes[r-1].base+axes[r-1].size**scan1++;  // prefetch next address of _1-cell being filled in result
skippre:;
   // move one _1-cell using the indexes
   UI i0=n0;  /* number of duff loops for last axis */ \
   scan0-=((amflags>>AMFLAGDUFFWX)&0x7);  // pointer to first 0-cell index, biased by duff adj
   av-=cellsize*(amflags>>AMFLAGDUFFAX);  // bias output pointer too, but not if it is repeated
   switch(amflags&0x3f){
   case 0b100111: case 0b101101:  case 0b101111: case 0b111100:  case 0b111101:  case 0b111110:  case 0b111111:   // unused cases
   CP11(B) break; CP11(US) break; CP11(UI4) break; CP1n(B) break; CP1n(US) break; CP1n(UI4) break;
#if SY_64
   CP11(UI) break; CP1n(UI) break;
#endif
   CP1xv(1,0) break; CP1xv(1,1) break; CP1xv(0,0) break; CP1xv(0,1) break;
   CPn1v break; CP11recur break; CP1nrecur break; CPn1recur break; CP1narecur break;
   }
   base=basepre;  // use prefetch
   av+=avnreset;  // if we repeat a after _1-cell, do so.  If av holds a value, this will always add 0
   scan0+=scan0reset;  // if ind type, scan0 rolls along; otherwise reset after each repetition
  }while(--n1>0);
  if(n1==0)goto skippre;  // finish up last block if any
  // We have finished a cell in the last 2 axes.  Advance to next cell if any
  I rinc=r-2;  // odomoter pointer to the cell being incremented
  while(1){
   if(rinc<=0)goto endamend;  // level 0 is the array in full.  It can't be incremented.  This is the loop exit, taken immediately if there are <=2 axes
   av+=axes[rinc+1].resetadder;  // if a resets for previous level, make that happen
   axes[rinc].scan++;  //  step to next position
   if(axes[rinc].scan!=(UI)axes[rinc].max)break;  // stop odo if wheel didn't roll over
   axes[rinc].scan=0;  // reset on rollover
   --rinc;  // back to previous wheel
  }
  // recalc base pointer for next block.  rinc is the last wheel that moved.  Its base is unchanged.  Calculate new bases from rinc+1 through r-1.  'base' will be the base of the last axis,
  // which must include indexing for axis -2.
  base=axes[rinc].base;  // base going into last wheel that moved
  while(1){    // for each wheel that moved, and then continuing to axis -2
   scan1=axes[rinc].indexes; UI *andx=scan1+axes[rinc].scan; andx=scan1?andx:&axes[rinc].scan;  // point to next value; if indexes==0, taken in full, use the scan as the index
   base+=*andx*axes[rinc].size;  //  base rinc+index adjustment rinc gives base of axis rinc+1...
   if(++rinc==r)break;  // ...advance to that.  When the base is for the last axis, we are done
   axes[rinc].base=base;  // for axes up through axis -2, store the base 
  }
  // init for loopback: next _2-cell.  We finished with scan1 pointing to indexes for axis -2 and base holding the address of the first cell for axis -1
  ++scan1;  // calculating base was a prefetch; advance to the next index if any in axis -2.  Immaterial if axis -2 taken in full
  n1=axes[r-1].max;  // # iterations left on last 2 axes
 }
endamend:;
 RETF(z);
}

// Convert list/table of indexes to array of cell offsets (the number of the atom starting the cell)
// w has rank > 0.  Result has shape }:$ind
// This is used by ind} and <"1@[ { ]  which have different specs when ind is a list.  We use a flag in jt:
// we follow the spec for m}, in which a list ind is treated like a table with rows of length 1, unless JTCELLOFFROM
// is set, in which case we treat ind as a 1-row table
// The indexes are audited for validity and negative values
// This is like pind/aindex1 followed by pdt, but done in registers and without worrying about checks for overflow, since the result
// if valid will fit into an integer.  If there are no negative indexes, this method is just a teeny bit faster, because pind/aindex1 do one quick loop at full memory
// speed to validate the input, and pdt works well for a large number of short vectors - in particular it avoids the carried dependency between axes that
// Horner's Rule creates.  This version keeps things in registers and has less setup time; and it is much better if there are negative indexes.
A jtcelloffset(J jt,AD * RESTRICT w,AD * RESTRICT ind){A z=0;F1PREFIP;
 ARGCHK1(w);
 if(AR(ind)<2){
  // rank of ind is 0 or 1.  Treat as list of first-axis values (as is suitable for m}) unless FROM is specified and rank=1 and length>1; then treat as list of successive axes
  if(!((I)jtinplace&(AN(ind)>1)&JTCELLOFFROM))RZ(z=pind(AS(w)[0],ind));  // (m}only, or length<2) treat a list as a list of independent indexes.  pind handles that case quickly and possibly in-place.
  // if FROM on list, leave z=0 and fall through to treat as table
 }else if(AS(ind)[AR(ind)-1]==1){RZ(z=pind(AS(w)[0],IRS1(ind,0L,2L,jtravel,z)));  // if rows are 1 long, pind handles that too - remove the last axis
 }
 if(likely(z==0)){  // if not handled already...
  // rank of ind>1 (or = if CELLOFFFROM), and rows of ind are longer than 1. process each row to a cell offset
  I naxes = AS(ind)[AR(ind)-1];
  I nzcells; PROD(nzcells,AR(ind)-1,AS(ind));
  if(!ISDENSETYPE(AT(ind),INT))RZ(ind=cvt(INT,ind));  // w is now an INT vector, possibly the input argument
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
 wr=AR(w); ws=AS(w); b=-AN(ind)&SGNIF(AT(ind),BOXX);  // b<0 = indexes are boxed and there is at least one axis
 if(!wr){x=from(ind,zeroionei(0)); *cellframelen=0; R x;}  // if w is an atom, the best you can get is indexes of 0.  No axes are used
 if((b&-AR(ind))<0){   // array of boxed indexes
  RZ(j=aindex(ind,w,0L)); j=(A)((I)j&~1LL);  // see if the boxes are homogeneous, or erroneous
  if(!j){  // if not homogeneous...
   if(unlikely(JT(jt,deprecct)!=0))RZ(jtdeprecmsg(jt,1,"(001) inhomogeneous multiple selectors in (x m} y): consider applying them separately\n"));
   RZ(x=MODIFIABLE(from(ind,increm(iota(shape(w)))))); u=AV(x); // go back to the original indexes, select from table of all possible incremented indexes; since it is incremented, it is writable
   DQ(AN(x), ASSERT(*u,EVDOMAIN); --*u; ++u;);   // if anything required fill, it will leave a 0.  Fail then, and unincrement the indexes
   *cellframelen=AR(w); R x;   // the indexes are what we want, and they include all the axes of w
  }
  // Homogeneous boxes.  j has them in a single table.  turn each row into an index
  if(unlikely(JT(jt,deprecct)!=0))RZ(jtdeprecmsg(jt,2,"(002) homogeneous multiple selectors in (x m} y): consider using rank-2 m\n"));
  b=0; ind=j;  // use the code for numeric array
  // later this can use the code for table m
 }
 if(b>=0){
  // Numeric/empty m.  Each 1-cell is a list of indexes (if m is a list, each atom is a cell index)
  RZ(j=celloffset(w,ind));  // convert list/table to list of indexes, possibly in place
  n=AR(ind)<2?1:AS(ind)[AR(ind)-1];  // n=#axes used: 1, if m is a list; otherwise {:$m
 }else{  // a single box.  must contain heterogeneous boxes, or we would have handled it earlier
  ind=C(AAV(ind)[0]); n=AN(ind); r=AR(ind);  // ind etc now refer to the CONTENTS of the single box
  ASSERT((-(n|(r^1))&((AT(ind)&BOX+NUMERIC)-1))>=0,EVINDEX);  // must be empty list or numeric or boxed
  if(((n-1)|SGNIF(AT(ind),BOXX))>=0)RZ(ind=IRS1(ind,0,0,jtbox,j));  // if numeric, box each atom
  v=AAV(ind);   // now ind is a atom/list of boxes, one per axis
  ASSERT(1>=r,EVINDEX);  // not a table
  ASSERT(n<=wr,EVINDEX);  // not too many axes
  DQ(n, if(!equ(ds(CACE),C(v[i])))break; --n;);  // discard trailing (boxed) empty axes
  j=zeroionei(0);  // init list to a single 0 offset
  for(i=0;i<n;++i){  // for each axis, grow the cartesian product of the specified offsets
   x=C(v[i]); d=ws[i];  // d=length of axis i
   if((-AN(x)&SGNIF(AT(x),BOXX))<0){   // notempty and boxed
    ASSERT(!AR(x),EVINDEX); 
    x=C(AAV(x)[0]); k=IX(d);
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

// Execution of x m} y.  Split on sparse/dense, passing on the dense to merge2, including inplaceability
static DF2(jtamendn2){F2PREFIP;PROLOG(0007);A e,z; B b;I atd,wtd,t,t1;P*p;
 AD * RESTRICT ind; ind=C(VAV(self)->fgh[0]);  // ind=m, the indexes to be modified
 ARGCHK3(a,w,ind);
 if(likely(!ISSPARSE(AT(w)|AT(ind)))){
  // non-sparse.  The fast cases are: (1) numeric m; (2) single box m.  numeric m turns into a single list of validated indexes; single box m turns into
  // a list of boxes, one  for each index.  multiple-box m is converted to a list of indexes, possibly very inefficiently.  If we end up with a list of indexes,
  // we also need to know the frame of the cells
  I cellframelen;
  z=0;  // use z to hold reworked ind
  struct axis *axes, localaxes[4]; A alloaxes;  // put axes here usually
  I wr=AR(w); I *ws=AS(w); b=-AN(ind)&SGNIF(AT(ind),BOXX);  // b<0 = indexes are boxed and there is at least one axis
  if(unlikely(!wr)){RZ(z=from(ind,zeroionei(0))); cellframelen=0;}  // if w is an atom, the best you can get is indexes of 0.  No axes are used
  else if((-AN(ind)&SGNIF(AT(ind),BOXX))>=0){
   // ind is empty or not boxed.  If it is a list, audit it and use it.  If it is a table or higher, convert to cell indexes.
   cellframelen=AR(ind)<2?1:AS(ind)[AR(ind)-1];  // #axes used: 1, if m is a list; otherwise {:$m
   if(AR(ind)==0){  // scalar ind is common enough to test for
    if(!ISDENSETYPE(AT(ind),INT))RZ(ind=cvt(INT,ind));  // ind is now an INT vector, possibly the input selector
    if(likely((UI)IAV(ind)[0]<(UI)ws[0]))z=ind; else{ASSERT(IAV(ind)[0]<0,EVINDEX); ASSERT(IAV(ind)[0]+ws[0]>=0,EVINDEX); RZ(z=sc(IAV(ind)[0]+ws[0]));}  // if the single index is in range, keep it; if neg, convert it quickly
   }else RZ(z=jtcelloffset(jt,w,ind));  // create (or keep) list of cell indexes
  }else if(likely(AR(ind)==0)){
   // ind is a single box, <selectors.  It must have rank 0 because the rank affects the rank of m{y and thus the allowed rank of a.
   A ind0=C(AAV(ind)[0]);  // discard ind, move to selectors
   ASSERT(AN(ind0)<=AR(w),EVLENGTH);  // can't have more selectors than axes
   if(AT(ind0)&BOX){
    // selectors are boxed.  They have selectors for sequential axes.  Put them into a multidimensional axis struct.  In this struct a pointer of 0 means
    // an axis taken in full
    ASSERT(AR(ind0)<2,EVRANK); ASSERT(AN(ind0)<=AR(w),EVLENGTH);   // array of axes must have rank<2, and must not exceed #axes in w
    if(AN(ind0)<2){
     // 0-1 selectors, turn it into an ind if it contains a list/atom or a box.  If it contains a table or higher, revert to general case
     if(AN(ind0)!=0){
      // the single selector is a singleton box <indexes or <<compindexes
      ind0=C(AAV(ind0)[0]);  // advance ind0 to indexes or <compindexes
      if(!(AT(ind0)&BOX))z=pind(AS(w)[0],ind0);  // not boxed - get/keep index list for first axis
      else{  // <<compidexes
       ASSERT(!AR(ind0),EVINDEX);   // must be just one atomic box
       RZ(z=icap(jtmerge2((J)((I)jt+JTINPLACEW),num(0),reshape(sc(AS(w)[0]),num(1)),pind(AS(w)[0],C(AAV(ind0)[0])),1)))  // I. 0 ind} (#w) $ 1  - the unselected indexes, in ascending order
      }
      cellframelen=1;  // in this path, the cells are _1-cells
     }else{
      // empty list of selectors, that means 'all taken in full' - one selection of the whole.
      cellframelen=0; z=zeroionei(0);  // select everything
     }
    }else{I i;I indn=AN(ind0); A *indv=AAV(ind0);  // number of axes and pointer to first
     // At least 2 axes given.  use/allocate the axis struct
     if(likely((UI)AN(ind0)<sizeof(localaxes)/sizeof(localaxes[0]))){axes=localaxes;}else{GATV0(alloaxes,INT,AN(ind0)*sizeof(localaxes)/sizeof(I),0); axes=(struct axis *)voidAV0(alloaxes);}  // < because we add a leading axis for the array itself
     // fill in the struct for each axis.  check indexes for validity (allocating a new block  if any are negative); handle complementaries, and axes taken in full
     A ax;  // holds the index block for the current axis.  At end of loop, holds the ind block for axis 0, after complementation
     for(i=indn;i;--i){  // for each axis, rolling up from the bottom.  The input is i-1, the output axis is i (because we added the leading axis)
      ax=C(indv[i-1]);  // point to the index block
      I axlen=AS(w)[i-1];  // length of axis
      axes[i].size=axlen;  // save in axis struct
      axes[i].scan=0;  // start at beginning of indexes
      axes[i].resetadder=0;  // init to no reset at end of axis
      // resolve complementary index
      if((-AN(ax)&SGNIF(AT(ax),BOXX))>=0){   // notempty and boxed means complementary indexing
       // not complementary indexing: create or keep valid integer index list
       // the selector may have any shape.  If it is not a list, it will affect the rank of m{y.  We will handle two cases: atom and list selector.
       // If the selector is an atom, we mark its max as -1 to indicate that the axis is to be skipped when checking frames
       RZ(ax=pind(axlen,ax));  // audit selectors
       if(unlikely(AR(ax)>1)){
        // one of the selectors has rank >1.  We have to go to general case
        if(unlikely(JT(jt,deprecct)!=0))RZ(jtdeprecmsg(jt,3,"(003) axis selectors in (x m} y) have rank>1\n"));
        goto noaxes;  // table in selectors: abort axis work
       }
       axes[i].max=(AR(ax)-1)|AN(ax);  // note how many there are, or -1 if atom
       axes[i].indexes=IAV(ax);  // save pointer to the indexes
      }else{
       // here for complementary indexing
       ASSERT(!AR(ax),EVINDEX);   // must be just one box
       ax=C(AAV(ax)[0]);  // open it
       if(AN(ax)){  // if not taken in full...
        RZ(ax=icap(jtmerge2((J)((I)jt+JTINPLACEW),num(0),reshape(sc(axlen),num(1)),pind(axlen,ax),1)))  // I. 0 ind} (#w) $ 1  - the unselected indexes, in ascending order
        axes[i].max=AN(ax);  // note how many are left
        axes[i].indexes=IAV(ax);  // save pointer to the indexes
       }else{
        // axis taken in full.  Tag with 0 pointer.  BUT the last axis cannot be taken in full: if we try, discard all trailing in-full axes
        if(likely(i!=indn)){
         // non-trailing axis
         axes[i].max=axlen;  // use all values
         axes[i].indexes=0;  // ...implicitly, with 0 pointer
        }else{--indn; // trailing axis taken in full: delete it
        }
       }
      }
     }
     if(likely(indn>=2)){
      // here we have succeeded in using the axes.  Indicate that fact by taking the two's comp of the framelen
      cellframelen=~indn; z=(A)axes;
     }else{
      // rare case where so many trailing axes were deleted that we don't have 2 axes left.  Revert to ind form, where ax is known to have the ind for axis 0
      cellframelen=indn; z=indn?ax:zeroionei(0);  // set cell size, and axes if there are any; if no axes, just select array in full
     }
    }
   }else{
    // contents are not boxed.  They must be a single list/atom of successive axes; convert to a single cell index
    ASSERT(AR(ind0)<2,EVRANK);  // numeric contents must be atom or list
    cellframelen=AN(ind0);  // remember the size of the cells
    if(cellframelen){RZ(z=jtcelloffset((J)((I)jt+JTCELLOFFROM),w,ind0));}else{z=zeroionei(0);}  // if empty list, that means 'all taken in full' - one selection of the whole.  Otherwise convert the list to indexes
   }
  }
noaxes:;
  if(unlikely(z==0))z=jstd(w,ind,&cellframelen);  // get ind and framelen for complex indexes
  z=jtmerge2(jtinplace,ISSPARSE(AT(a))?denseit(a):a,w,z,cellframelen);  //  dense a if needed; dense amend
  // We modified w which is now not pristine.
  PRISTCLRF(w)
  EPILOG(z);
 }
 // Otherwise, w is sparse
 // ?t = underlying type of ?, s?=nonzero if sparse
 atd=ISSPARSE(AT(a))?DTYPE(AT(a)):AT(a); wtd=ISSPARSE(AT(w))?DTYPE(AT(w)):AT(w);
 ASSERT(AT(ind)&NUMERIC+BOX||!AN(ind),EVDOMAIN);
 ASSERT(!ISSPARSE(AT(ind)),EVNONCE);  // m must be dense, and numeric or boxed
 if(AT(ind)&NUMERIC){  // numeric must have rank <3; if rank is 2, we treat it as <"1 ind
  ASSERT(AR(ind)<3,EVRANK);
  if(AR(ind)==2){if(AN(ind)==0)ind=mtv; else{A aa=ind; RZ(ind=IRS1(aa,0,1,jtbox,ind));}}  // Convert empty 2-d to atom so aindex doesn't fail
 }
 // Sparse w.  a and t must be compatible; sparse w must not be boxed
 ASSERT(!(wtd&BOX),EVNONCE); ASSERT(HOMO(atd,wtd),EVDOMAIN);
 // set t to dense precision of result; t1=corresponding sparse precision; convert a if need be.  Change a's type but not its sparseness
 RE(t=maxtyped(atd,wtd)); t1=STYPE(t); RZ(a=TYPESEQ(t,atd)?a:cvt(ISSPARSE(AT(a))?t1:t,a));
 // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the dense type is either
 // DIRECT or this is a boxed memory-mapped array
 B ip=((I)jtinplace&JTINPLACEW) && (ACIPISOK(w) || jt->zombieval==w&&AC(w)<=ACUC2)
     &&TYPESEQ(t,wtd)&&(t&DIRECT)>0;
 // see if inplaceable.  If not, convert w to correct precision (note that cvt makes a copy if the precision is already right)
 if(ip){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w; fa(w);}else RZ(z=cvt(t1,w));  // don't know why, but sparse amend code requires AC=1
 // call the routine to handle the sparse amend
 p=PAV(z); e=SPA(p,e); b=!AR(a)&&equ(a,e);
 p=PAV(a); if(unlikely(ISSPARSE(AT(a))&&!equ(e,SPA(p,e)))){RZ(a=denseit(a)); }
 if(AT(ind)&NUMERIC||!AR(ind))z=(b?jtam1e:ISSPARSE(AT(a))?jtam1sp:jtam1a)(jt,a,z,AT(ind)&NUMERIC?box(ind):ope(ind),ip);
 else{RZ(ind=aindex(ind,z,0L)); ind=(A)((I)ind&~1LL); ASSERTSUFF(ind!=0,EVNONCE,z=0; goto exitra;); z=(b?jtamne:ISSPARSE(AT(a))?jtamnsp:jtamna)(jt,a,z,ind,ip);}  // A* for the #$&^% type-checking
exitra:
 if(ip)ra(w);
 EPILOGZOMB(z);   // do the full push/pop since sparse in-place has zombie elements in z
}

// Execution of x u} y.  Call (x u y) to get the indices, then
// call merge2 to do the merge.  Pass inplaceability into merge2.
static DF2(amccv2){F2PREFIP;DECLF; 
 ARGCHK2(a,w); 
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);  // u} not supported for sparse
 A x;RZ(x=pind(AN(w),CALL2(f2,a,w,fs)));
 A z=jtmerge2(jtinplace,a,w,x,AR(w));   // The atoms of x include all axes of w, since we are addressing atoms
 // We modified w which is now not pristine.
 PRISTCLRF(w)
 RETF(z);
}


static DF1(mergn1){       R merge1(w,VAV(self)->fgh[0]);}
static DF1(mergv1){DECLF; R merge1(w,CALL1(f1,w,fs));}

// called from m}, m is usually NOT a gerund
static B ger(J jt,A w){A*wv,x;
 if(!(BOX&AT(w)))R 0;
 wv=AAV(w); 
 DO(AN(w), x=C(wv[i]); if((-(BOX&AT(x))&(((AR(x)^1)|(AN(x)^2))-1))<0)x=C(AAV(x)[0]); if(((-(LIT&AT(x))&(AR(x)-2)&-AN(x)))>=0)R 0;);  // box/rank1/N=2; lit/R<2/N!=0
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
  wv=AAV(w);  x=C(wv[0]); // point to pointers to boxes; point to first box contents
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
  x=C(wv[1]);   // point to second box
  if((SGNIF(AT(x),BOXX) & ((AR(x)^1)-1))>=0)R 0;   // verify it contains a list of boxes
  if(!BETWEENC(AN(x),bmin,bmax))R 0;  // verify correct number of boxes
  R gerexact(x);  // recursively audit the other ARs in the second box
 } else R 0;
 R 1;
}

B jtgerexact(J jt, A w){A*wv;
 if(!(BOX&AT(w)))R 0;   // verify gerund is boxed
 if(!(AN(w)))R 0;   // verify there are boxes
 wv=AAV(w);   // point to pointers to contents
 DO(AN(w), if(!(gerar(jt, C(wv[i]))))R 0;);   // fail if any box contains a non-gerund
 R 1;
}    /* 0 if w is definitely not a gerund; 1 if possibly a gerund */


// u} handling.  This is not inplaceable but the derived verb is
F1(jtamend){F1PREFIP;
 ARGCHK1(w);
 if(VERB&AT(w)) R ADERIV(CRBRACE,mergv1,amccv2,VASGSAFE|VJTFLGOK2, RMAX,RMAX,RMAX);  // verb} 
 else if(ger(jt,w))R gadv(w,CRBRACE);   // v0`v1`v2}
 else           R ADERIV(CRBRACE,mergn1,jtamendn2,VASGSAFE|VJTFLGOK2, RMAX,RMAX,RMAX);  // m}
}

static DF2(jtamen2){ASSERT(0,EVNONCE);}

F1(jtemend){F1PREFIP;
 ASSERT(NOUN&AT(w),EVDOMAIN);
 R ADERIV(CEMEND,0L,jtamen2,VFLAGNONE, RMAX,RMAX,RMAX);
}
