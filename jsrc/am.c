/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Amend                                                          */

#include "j.h"
#include "ve.h"

#define MCASE(t,k)  (((t)&INT)+(k))   // t is B01 or INT, k is lg(size of item) if one of 0 2 3,  1 otherwise
#define MINDEX        {j=*u++; if(0>j)j+=m; ASSERT(BETWEENO(j,0,m),EVINDEX);}

// m} y
static A jtmerge1(J jt,A w,A ind){A z;B*b;C*wc,*zc;I c,it,j,k,m,r,*s,t,*u,*wi,*zi;
 ARGCHK2(w,ind);
 r=MAX(0,AR(w)-1); s=1+AS(w); t=AT(w); k=bplg(t); SETIC(w,m); c=aii(w);  // m = # items of w
 ASSERT(!ISSPARSE(t),EVNONCE);
 ASSERT(r==AR(ind),EVRANK);
 ASSERTAGREE(s,AS(ind),r);
 GA(z,t,c,r,s);
 if(!ISDENSETYPE(AT(ind),B01+INT))RZ(ind=cvt(INT,ind));
 it=AT(ind); u=AV(ind); b=(B*)u;
 ASSERT((-c&(m-2))>=0||(!c||(m==1&&!memchr(b,C1,c))),EVINDEX);  // unless items are empty, m must have items.  if m is 1 all selectors must be 0.  INT will be checked individually, so we just look at the first c bytes
 zi=AVn(r,z); zc=(C*)zi;
 wi=AV(w); wc=(C*)wi;
#if !SY_64
D*wd=(D*)wc,*zd=(D*)zc;
#endif
 I kx=k; kx=0b1101&(1LL<<k)?kx:1;  // set atom length to 2 if not one of 1 4 8
 switch(MCASE(it,kx)){
 case MCASE(B01,LGSZI): DO(c,         *zi++=wi[i+c*(I)*b++];); break;
 case MCASE(B01,0): DO(c,         *zc++=wc[i+c*(I)*b++];); break;
#if !SY_64
 case MCASE(B01,LGSZD): DO(c,         *zd++=wd[i+c*(I)*b++];); break;
#endif
 case MCASE(INT,0): DO(c, MINDEX; *zc++=wc[i+c*j];); break;
 case MCASE(INT,LGSZI): DO(c, MINDEX; *zi++=wi[i+c*j];); break;
#if !SY_64
 case MCASE(INT,LGSZD): DO(c, MINDEX; *zd++=wd[i+c*j];); break;
#endif  
 default: if(it&B01)DO(c,         MC(zc,wc+((i+c*(I)*b++)<<k),1LL<<k); zc+=1LL<<k;)
          else      DO(c, MINDEX; MC(zc,wc+((i+c*j     )<<k),1LL<<k); zc+=1LL<<k;); break;
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
 if(p=q&&0<=c&&AC(u[c])<=ACUC2&&!(AFLAG(u[c])&AFRO)) {  // passes quick check: 2-name fast case, reassigned name, modifiable usecount, not read-only
   p= (!EXPLICITRUNNING) || CAV(QCWORD(AAV(v[m+2])[1]))[0]!=CASGN || jtprobe((J)((I)jt+NAV(QCWORD(AAV(v[m+2])[0]))->m),NAV(QCWORD(AAV(v[m+2])[0]))->s,NAV(QCWORD(AAV(v[m+2])[0]))->hash, jt->locsyms);  // OK if not in explicit, or not local assignment, or name defined
    // Get the pointer to the parsed sentence; go to its data; take pointer for word[1]; go to its (character) data; take first character
    // then look up the symbol entry for word[0]
 }
 if(p)z=u[c]; else GA(z,t,n,r,s);   // z = output area, which may be in-place
// 0 B01+LIT 1 INT 2 FL 3 CMPX/QP 4 C2T/INT2 5 C4T/INT4 6 SBT 7 unused
 r=6; r=t&C4T+INT4?5:r;  r=t&C2T+INT2?4:r;  r=t&CMPX+QP?3:r;  r=t&FL?2:r;  r=t&INT?1:r;  r=t&B01+LIT?0:r; 
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
#define AMFLAGRTN ((I)0x3f<<AMFLAGRTNX)  // switch routine: 0llddd: cells have length 1<<ll with duff offset ddd; 1ll0dd single cell of length 1<<ll, duff offset dd;
                                 // 10b1xx non-atomic length cell, xx is 00=multiple cells of a, 01=single cell of a (length abytes) repeated in each result cell; 10=a is a single cell of length abytes=cellsize not repeated.  When xx is x0, b=0 means cellsize is a multiple of SZI
                                 // 1101xx recursive types, xx is 00=multiple cells of a, 01=single cell of a (length abytes) repeated in each result cell; 1y=a is a single cell of length abytes=(cellsize<<lgk), with y=1 if abytes=SZI
#define AMFLAGINFULL1X 7
#define AMFLAGINFULL1 ((I)1<<AMFLAGINFULL1X)  // axis 1 is taken in full
#define AMFLAGSPLITX 8  // axis number of the axis (if any) that a expires in the middle of.  Possible only if selector has rank>1.  We must reset a in the middle of processing the axis
#define AMFLAGDUFFWX 16
#define AMFLAGDUFFW ((I)1<<AMFLAGDUFFWX)  // duff backoff for w, 0-7
#define AMFLAGDUFFAX 24
#define AMFLAGDUFFA ((I)1<<AMFLAGDUFFAX)  // duff backoff for a, 0-7.  Must be highest field
 struct __attribute__((aligned(CACHELINESIZE))) axis{
  UI size;  // First, the length of this axis in the result.  Later, size in bytes of a cell of this axis
  C *base;  //  pointer to result cell of higher axis that this axis is indexing within.  base for axis i does not include index i.  Not needed for axis -1, or -2 if taken in full
  I max;  // number of indexes for this axis (if in full, total axis length).
  I *indexes;  // pointer to index array, or 0 if axis is taken in full
  UI scan;  // index number being worked on currently
  I resetadder;  // #atoms (later, bytes) to add to av when this axis rolls over (used to tell how to repeat a).  For ind forms, always resets after last axis; for axis forms, resets after a rollover of the lowest
                  // axis whose cell size is as big as a.  But if a single atom is repeated, always 0 to avoid destroying the value
  A ind;  // If there are indexes, pointer to the A block for them (used to check shape).  If no indexes, &mtv.  Thus the rank is always right even if the selector values are not
  I resetmod;  // if there is a split selector, requiring a to be reset in the middle of the selector, this is the number of items between resets
 };
// Handle a ind} w after indices have been converted to integer atom indexes, dense
// cellframelen is the number of axes of w that were used in computing the cell indexes, complemented if ind is axes.  Later axes of w are the cell shape
//   Value is (number of axes added for frame)/(framelen wrt a)/(framelen wrt w)/(#axes in ind)
// ind is the assembled indices OR a pointer to axes[]
static A jtmerge2(J jt,A a,A w,A ind,I cellframelen){F2PREFIP;A z;I t;
 ARGCHK2(a,w); RZ(ind);
 I aframelen=(I1)(cellframelen>>RANK2TX), wframelen=(I1)(cellframelen>>RANKTX), nframeaxes=(cellframelen>>(3*RANKTX)); cellframelen=(I1)cellframelen;  // length of frame of a and w; convert cellframelen to signed int
 //   w w w w w
 // m m m . w w   the rank of m may be more or less than cellframelen which is the number of axes that are covered by m.  For single boxed m, AR(ind)=cellframelen
 // <---a   a a
 // audit shape of a: must be a suffix of (shape of ind),(shape of selected cell)
 I compalen;  // len of shape of a that has been compared
 I acr=AR(a)-aframelen, wcr=AR(w)-wframelen;  // inner cell ranks
 I *as=AS(a)+aframelen, *ws=AS(w)+wframelen;  // address of shapes of cells of a/w
 if(cellframelen>=0){  // ind is indexes
  ASSERT(acr<=wcr+AR(ind)-cellframelen,EVRANK);   // max # axes in a is the axes in w, plus any surplus axes of m that did not go into selecting cells
  compalen=MAX(0,acr-(wcr-cellframelen));  // #axes of a that are outside of the cell in w
  ASSERTAGREE(as,AS(ind)+AR(ind)-compalen,compalen);  // shape of m{y is the shape of m, as far as it goes.  The first part of a may overlap with m
 }else{I wi; struct axis *axes=(struct axis*)ind;  // ind is axes.  The cell-frame part of the shape of a must be a suffix of the cell-frame part of axes
  I ai=compalen=MAX(0,acr-(wcr-~cellframelen));  // #axes of a that are outside of the cell in w
  // scan through the frame of ind, backwards.  Match each axis-shape with trailing shape of ind.  Stop when a is exhausted.  If a has surplus axes, that's an error
  for(wi=(~cellframelen);((ai-1)|(wi-1))>=0;--wi){  // loop through each axis of w, starting at the last.  Stop when wi OR ai hits 0
   // ai is the index+1 of the next axis of a to match
   struct axis *wx=&axes[wi+nframeaxes];  // the next axis of w to match to a.  axis 0 is left empty (bad decision)
   // the shape of the next selector of a comes from max if the axis is taken in full, or ind if it is not
   I *inds=AS(wx->ind); I indr=AR(wx->ind); inds=wx->indexes==0?&wx->max:inds;   // get rank/shape of selectors
   I aslen=MIN(ai,indr); ASSERTAGREE(as+ai-aslen,inds+AR(wx->ind)-aslen,aslen)  // verify that the shape of a matches ind at the tail of each
   ai-=aslen;  // account for axes of a that we have tested
  }
  ASSERT(ai==0,EVRANK);  // error if unmatched axes of a
 }
 ASSERTAGREE(as+compalen,ws+wcr-(acr-compalen),acr-compalen)  // the rest of the shape of m{y comes from shape of y

 // if there is surplus outer frame for a, replicate w to match the result size
 if(unlikely(aframelen>wframelen)){IRS2(a,w,0L,acr,wcr,jtright2,z); RZ(z); w=z; wframelen=aframelen;}  // w =. a ]"ranks w
 if(unlikely(!AN(w)))RCA(w);  // if y empty, return.  It's small.  Ignore inplacing
 if(unlikely(!AN(a)))RCA(w);  // if nothing to amend, return.  Ignore inplacing

 ASSERT((POSIFHOMO(AT(a),AT(w)))>=0,EVINHOMO);  // error if xy both not empty and not compatible
 t=maxtyped(AT(a),AT(w));  // get the type of the result: max of types
 if(TYPESNE(t,AT(a)))RZ(a=cvt(t,a));  // if a must change precision, do so
 // Keep the original address if the caller allowed it, precision of y is OK, the usecount allows inplacing, and the type is either
 // DIRECT or this is a boxed memory-mapped array; and don't inplace a =: a m} a or a =: x a} a
 // kludge this inplaces boxed mm arrays when usecount>2.  Seems wrong, but that's the way it was done
 // It is not possible to inplace a value that is backing a virtual block, because we inplace assigned names only when
 // the stack is empty, so if there is a virtual block it must be in a higher sentence, and the backing name must appear on the
 // stack in that sentence if the usecount is only 1.
 I ip = ASGNINPLACESGNNJA(SGNIF(jtinplace,JTINPLACEWX),w)
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
 I cellsize; PROD(cellsize,wcr-(REPSGN(cellframelen)^cellframelen),ws+(REPSGN(cellframelen)^cellframelen));  // number of atoms per index in ind
 if(unlikely(cellsize==0))RCA(w);  // nothing to move - exit so we can use UNTIL loops to move

 // initialization
 // if ind is axes, it has already been set up
 // if not, we treat it as a 1xAN(ind) array
 I an=AN(a);  // # atoms in a, negative if -@{`[`]}
 I acatoms; PROD(acatoms,acr,as) acatoms=an<0?an:acatoms;   // number of atoms in a major cell of a, negative (as flag) if -@{`[`]}
 I avnreset=-(acatoms<<lgk);  // amount to add to an av that has overrun to get it back to av0; after setup, the reset adder for axis -1
 I acresetcell=avnreset;  // copy, used for rare case of split selector
 C * RESTRICT av=CAV(a);
 JMCDECL(endmask)
 I n0,n1;  // # of iterations of axis -1,-2.  When ind is indices, these are calculated directly as (#indexes) and (#repetitions of a).  When axes, these are reloaded for each pass through the last 2 axes
 I *scan0, scan0reset;  // pointer to first index of last axis OR first index of ind; and the amount to add after an iteration to either continue or reset
 I *scan1;  // pointer to index of _2-cell being filled
 C *base;  // address of _1-cell being filled
 I r;  // #axes, +1.  axes[r] is the last axis
 struct axis *axes, localaxes[5];  // pointer to all axes; block to use for local axes derived from frame/ind (axis 0 not used, 0-2 for frame, 1-2 for ind)
 I amflags;  // flags and routine number to call in loop
 if(cellframelen>=0){  // ind is an array of 'fetch' indices, of arbitrary rank, construed as a 1-or 2-d array
  // the atoms of ind address cells of the entire w, of size cellsize.  We arbitrarily treat these as a 1- or 2-d array where each row of ind
  // matches at least one full application of a.  a is then repeated over the rows of ind
  // n0 is the length of a 'row' of ind, n1 is the # rows
  // first, handle the special cases of 0/1 cell.  We must handle 0 so that we can use UNTIL loops; we choose to handle 1
  // because it's common & avoids a lot of setup
  if(AN(ind)<2){  // 0/1 cell to move (but only if not -@:{`[`]})  
   if(unlikely(AN(ind)==0))RCA(w);  // nothing to store - exit so we can use UNTIL loops to move
   if(likely((an|-aframelen|-wframelen)>=0)){  // framelen!=0 means repeated store.  AN(a)<0 means we are running -@:{`[`]}, i. e. complementing FL atoms - so go through the code for that, below
    if(!UCISRECUR(z)){mvc(cellsize<<lgk,CAV(z)+IAV(ind)[0]*(cellsize<<lgk),an<<lgk,CAV(a)); // nonrecursive: copy in all of a, repeating as needed to fill the cell
    }else{cellsize<<=(t>>RATX); I ix0=IAV(ind)[0]*cellsize; I as=an<<(t>>RATX); base=CAV(z); do{A *avv=AAV(a); DQU(as, INSTALLBOXNVRECUR(((A*)base),ix0,*avv) ++ix0; ++avv;)}while(cellsize-=as);  // recursive z: increment usecount while installing
    }
    RETF(z);  // return the result
   }
  }
  PROD(n0,MAX(0,acr-(wcr-cellframelen)),AS(ind)+AR(ind)-MAX(0,acr-(wcr-cellframelen)))  // axes shared by a and ind
  PROD(n1,AR(ind)-MAX(0,acr-(wcr-cellframelen)),AS(ind))  // frame of ind wrt a
  if(unlikely((n0|n1)==0))RCA(w);  // nothing to store - exit so we can use UNTIL loops to move
  axes=localaxes;  // use the temp block for ind version
  base=CAV(z);  // the array in full is in play
  scan0reset=0;  // last-axis index pointer rolls along between iterations of 'axis -2'
  scan0=IAV(ind);  // keep pointer to the index list where we will use it - as indexes of last axis
  r=2;  // normally we will need only 2 axes
  // scan1 is not used
  amflags=AMFLAGINFULL1;  // there are no indexes for axis 1, so we say it is 'taken in full' to avoid fetching indexes.  This is true whether it is outer frame or not
 // if wframelen is not 0, the first axis must be for looping over the top-level cells of w.  If there is only one row in ind (i. e. only one level of looping is needed per last-level cell),
  // only the last ind-axis is needed.  If a must be advanced between rows of ind, another axis is needed for looping over the a-cells.
  if(likely(wframelen==0)){
   // no frame.  we must always have at least 2 axes, so fill in first axis even if it is only 1 long.  We don't have to allow for repeating the last 2 axes
   axes[1].size=0;   // don't reset to beginning of a between rows
   // n0 and n1 are set up as the loop counts for the last 2 axes
  }else{
   // there is outer frame
   I wocsize; PROD(wocsize,wcr,ws) wocsize<<=lgk;   // size in bytes of an outer cell of w, used to calculate cell sizes for each axis
   axes[1].scan=0; axes[1].indexes=0;   // axis in full, and get size of outer cell to advance to next outer cell of a after filling it 
   if(likely(aframelen==0)){  // wframe but no a frame: repeat the cell of a, using a single loop
    PROD(axes[1].max,wframelen,AS(w)) axes[1].size=wocsize;  // #cells in the frame of w; # bytes in a cell
    axes[2].resetadder=0;  // avnreset=-abytes to reset after last axis.  don't advance after that.  This is immaterial if n1==1
   }else{  // wframe & some aframe: we will need an extra axis
    axes[2].resetadder=-avnreset;  // avnreset=-abytes to reset after last axis.  Advance a when we exhaust repeats of a, if there are any
    if(unlikely(wframelen>aframelen)){  // if some cells of a are repeated, we must add an axis to repeat them
     axes[2].scan=0; axes[2].indexes=0;  // axis in full
     PROD(axes[2].max,wframelen-aframelen,AS(w)+aframelen) axes[2].size=wocsize; wocsize*=axes[2].max;  // #cells in the surplus frame of w; size of cell.  chain to axes[1] through wocsize
     r=3;  // we will need at least 3 axes
     axes[3].resetadder=0;  // Don't advance a after repeats of inner a-cells.  Immaterial if n1==1 (no such repeats)
     axes[1].base=base;  // All axes before the last 2 must have a base pointer indicating where they start.  For the frames axes (always taken in full) this is the overall base
    }else{  // wframe=aframe: no repeats of a, which we handle by not having an axis for those repeats
     avnreset=(n1==1)?0:avnreset;  // n1=1: r=2, let av advance after last axis (axes[2].resetadder immaterial)  n1>1: reset av after last axis, advance after next-last
    }
    PROD(axes[1].max,aframelen,AS(a)) axes[1].size=wocsize;  // #cells in the common frame of w and a (outermost count), and the size in atoms
   }
   // if the inner cells of a do not need to be repeated, we do not need a separate axis to repeat them
   if(n1==1){
    scan0reset=-n0;   // a inner cell not repeated: reset the index pointer after the last axis
    n1=axes[r-1].max;   // replace # repeats of inner a with # repeats of outer a
   }else{  //  if we must repeat cells of a within the last 2 axes
    ++r;  // add the new axis, now numbered r-1
    axes[r-1].max=n1;  // # repeats of a.  we will restore the axis _2-size after each pass through the last 2 axes
    axes[r-1].size=0;   // when we take a step in axis r-1, do not add to a, thus repeating a
    axes[r-1].indexes=0;  // indicate outer axis taken in full
    axes[r].max=n0;   // length of last access: enough for 1 pass through ind
    axes[r-2].base=base;  // All axes before the last 2 must have a base pointer indicating where they start.  For the frames axes (always taken in full) this is the overall base
   }
   axes[r].indexes=scan0;  // we will be reloading the scan pointer after each loop through the last 2 axes
  }
 }else{I i;
  // 'ind' is actually orthogonal axes, already filled in (in terms of atoms)
  axes=(struct axis *)ind;  // use the input block for all the axes
  amflags=0;  // init
  // now that we have the size of an atom, convert the cell-sizes-in-atoms to bytes
  // set the reset value at the correct level (if any)
  r=~cellframelen+nframeaxes;  // number of axes to process.  [0] is the array in full, [r] is the last axis
  n0=axes[r].max; n1=axes[r-1].max;  // # iterations left on last 2 axes
  scan0=axes[r].indexes; scan0reset=-n0;  // point to the indexes for the last axis (NEVER 0 meaning 'taken in full' which were discarded earlier); reset to start of indexes after each iteration
  base=CAV(z);  // the array in full is in play
  // roll up cell sizes, convert resetadder from atoms to bytes, and insert a resetadder at the point where a major cell of a is exhausted & must be repeated
  I prevsize=cellsize<<lgk;  //  size of cell of w at previous axis, in bytes.  Init to size of cell of last axis
  I axisressize=cellsize;  // size in atoms of full cell of a at this axis: following axis result size*number of indexes at this axis.  When this is = size of a, we must wrap a
  I awrappt=acatoms;  // wrap a when axis size is >= size of a major cell
  for(i=r;;--i){
   axisressize*=axes[i].max;  // ressize is size of an a-cell of the next axis
   I tsz=axes[i].size; axes[i].size=prevsize;  // .size is size of cell of w at this axis.
   if(unlikely(((awrappt-axisressize)&(1-AR(axes[i].ind)))<0)&&acatoms>cellsize){   // axisressize>awrappt && AR(axes[i].ind)>1; but not if a is repeated: reset doesn't happen then
    // a is exhausted in the middle of this axis.  We must reset repeatedly in the axis
    axes[i].resetmod=axes[i].max/(axisressize/awrappt); amflags|=i<<AMFLAGSPLITX;  // remember the axis that is split, and how often it must reset.  The case is very rare
   }
   if(i==1)break;
   prevsize*=tsz;  // Leave prevsize with size of cell of next axis
   axes[i].resetadder<<=lgk; // size in atoms of a cell of the NEXT axis; convert resetadder to bytes
   if(axisressize>=awrappt){    // if this axis is the first whose size is as big as a, reset after axis exhausted
    axes[i].resetadder+=avnreset; awrappt=IMAX;  // the += on resetadder is because the next axis may have asked for a rollover while this axis is asking for a reset; that cancels out to 0 (=advance to next cell)
   }
  }
  if(unlikely(axisressize==0))RCA(w);  // if nothing to move, exit so we can use UNTIL loops.  Have to wait till here for error-checking
  avnreset=axes[r].resetadder;  // move last-axis reset to known location for comp ease.  [1].resetadder is immaterial
  // create the starting base values for all axes.  base starts with the 'cell' for the array in full, which is the base of axis 0
  DO(r-1,
   axes[i+1].base=base;  // previous selected cell is the base of the next axis
   UI *andx=scan1=axes[i+1].indexes; andx=andx?andx:&axes[i+1].scan;  // point to next value; if indexes==0, taken in full, use the scan (=0) as the index
   base+=*andx*axes[i+1].size;  // get address of cell being worked on, for the next axis
  )
  // now base points to the base cell for axis -1, which includes the offset from the first index in axis -2.  scan1 has the address of the indexes for axis -2
  amflags|=(scan1==0)<<AMFLAGINFULL1X; ++scan1;  // Remember if axis -2 taken in full.  the calculation of base was a prefetch of the first index in axis -2.  Increment to the next position if any.  scan1 is unused if axis taken in full
 }

 // select routine to use based on argument size/type
 // cellsize is now atoms; change it to bytes when we have the routine, EXCEPT for recursive/-@{`[`]} types which stay as atoms
 if(likely(((C)(amflags>>AMFLAGSPLITX))<(C)(r-1))){  // normal case with no split selector in last 2 axes
  if(!UCISRECUR(z)){
   // not replacement of recursive indirect blocks
   if(cellsize<=acatoms){
    // a major cell of a is at least as big as a cell being replaced, and function is not -@{`[`]}
    if(((LOWESTBIT(cellsize)-cellsize)|(SZI-(cellsize<<lgk))|-aframelen)>=0){  // if cellsize is power of 2 and total cell is <= 8 bytes, and is the only cell of a
     // the cell can be moved as a primitive type.  We will use a Duff loop.  Calculate the offets and index, which depends on whether the cell is repeated
     // We don't have a loop that moves a single cell and allows an av address (ex: 99 100 (<1 2;2 3)}"0 2 i. 5 6 where 99 & 100 need to be replicated), so we can't come here & have to treat them as general cells
     C *avv=(C*)*(A*)av;  I repeatatom=(cellsize==acatoms); av=(cellsize==acatoms)?avv:av; avnreset=(cellsize==acatoms)?0:avnreset;  // repeatatom is 1 if the single atom is repeated (and there is only 1 outer cell).  In that case, replace av with the atom itself and clear the reset adder so it doesn't change
     I duffmask=0x7>>repeatatom;  // for nonrepeated cells, duff loop is 8 cells; for repeated atom, 4
     I backupct=(-n0)&duffmask;  //  duff backup
     n0=(n0+duffmask)>>(3-repeatatom);  // convert n0 into # turns through duff loop, giving 8/4 cells per turn
     amflags|=CTTZI(cellsize<<lgk)*8 + (repeatatom<<5) + (backupct*((1+AMFLAGDUFFW+AMFLAGDUFFA)^(repeatatom<<AMFLAGDUFFAX)));  // routine is 0brccDdd (r=repeat, cc=cellsize, ddd=duff backoff, D=0 if repeat)
    }else{
     // cell is not 1-, 2-, 4-, or 8-size, or there are multiple outer cells of a. We will have to use move loops.  No duff
     I mvinc=REPSGN((cellsize-acatoms)|(-aframelen));  // ~0 normally; 0 if a contains a single cell, and thus must not increment address after the move  (aca>csz or aframe)
     I mvbytelen=((cellsize<<lgk)&(SZI-1))!=0;  // 1 if cells are not word multiples, in which case we must not overstore
     amflags|=0b100110+(mvinc<<1)+(mvbytelen<<3);  // select appropriate code
     // avnreset is set to make the proper adjustment to av after it has been incremented in the loop.  If the loop doesn't increment, compensate by
     // adding the increment to avnreset
     avnreset+=(acatoms<<lgk)&~mvinc;  // if increment suppressed in loop, add the incr amount to avnreset
     JMCSETMASK(endmask,cellsize<<lgk,mvbytelen)   // prepare for repeated move
    }
   }else if(unlikely(acatoms<0)){  // AN<0 is a flag indicating -@:{`[`]}.  Type must be CMPX/FL/QP.  There is no a
    cellsize<<=lgk-LGSZD; lgk=LGSZD;  // lgk is 3 for FL, 4 for QP/CMPX.  Convert cellsize to count of D atoms, and switch atom size to SZD
    if(cellsize==1){  // just 1 word per cell, use duff loop
     I backupct=(-n0)&3;  //  duff backup
     n0=(n0+3)>>2;  // convert n0 into # turns through duff loop, giving 4 cells per turn
     amflags|=0b111100 + (backupct*((1+AMFLAGDUFFW)));  // routine is 0b1111dd (dd=duff backoff)
    }else amflags|=0b101111;  // block-negate routine
   }else{
    // a must be repeated to fill a replaced cell.  The address of a is never incremented in the loop
    amflags|=0b100101;   // use repeating code, with no duff bias.
    acatoms<<=lgk;  // repurpose atom count to byte count for this loop
    avnreset+=acatoms;  // compensate for not incrementing in the loop
   }
   cellsize<<=lgk;  // convert cellsize to bytes for the rest of the processing
  }else{
   // replacing recursive indirect blocks
   // cases: multiple cells of a; single cell of a, repeated; single cell of a, duped; single atom of a, repeated

   I mvinc=REPSGN((cellsize-acatoms)|(-aframelen));  // ~0 normally; 0 if a contains a single cell with no frame, and thus must not increment address after the move  (aca>csz or aframe)
   I nodupa=cellsize<=acatoms;  // 0 if the single cell of a must be duplicated in each replaced cell
   avnreset+=(acatoms<<lgk)&~mvinc;  // if increment suppressed in loop, add the incr amount to avnreset
   amflags|=0b110101+2*mvinc+nodupa;  // 110100=incremented a; 110110=single cell of a; 110101=single cell of a, duped
   I oneatom=(an+(t>>RATX))==1; C *avv=(C*)*(A*)av; av=oneatom?avv:av; avnreset=oneatom?0:avnreset; amflags|=3*oneatom;   // if a is one single atom, fetch the atom and set code to 110111
   cellsize<<=(t>>RATX);  // RAT has 2 boxes per atom, all other recursibles have 1 and are lower.  Repurpose cellsize as #indirect references
   acatoms<<=(t>>RATX);  // repurpose # atoms to be # recursibles per atom
  }
 }else{
  // the very rare case where a selector is split in one of the last 2 axes.  We use a routine that copies cells, resetting as needed
  amflags|=UCISRECUR(z)?0b101101:0b100111;  // recursive or normal
  // make changes like in the main line above
  if(cellsize>acatoms)acatoms<<=lgk;  // repurpose atom count to byte count for this loop
  cellsize<<=UCISRECUR(z)?(t>>RATX):lgk;
 }

// macros for negating axis -1
// cases 1111..  negate FL atoms (possibly in a QP).  We use 0.-value so as to set all 0 values to +0
#define CP11neg  /* each index copies a different cell to the result */ \
do{ \
 case 0b111100+0: ((D*)base)[scan0[0]]=0.0-((D*)base)[scan0[0]]; case 0b111100+1: ((D*)base)[scan0[1]]=0.0-((D*)base)[scan0[1]];   /* negate cells */ \
 case 0b111100+2: ((D*)base)[scan0[2]]=0.0-((D*)base)[scan0[2]]; case 0b111100+3: ((D*)base)[scan0[3]]=0.0-((D*)base)[scan0[3]]; \
 scan0+=4;  /* advance pointers */ \
}while(--i0);
// cells of arbitrary size
#define CP1xvneg case 0b101111: DQNOUNROLL(n0, av=base+cellsize**scan0++; minusDD AH2A(1,2*(cellsize>>LGSZD)+1,(D*)&dzero,(D*)av,(D*)av,jt);)  // complement all of each cell
 
// macros for copying axis -1
// cases 0ll...: ll is lg2(atom length) 0-3
#define CP11(t)  /* each index copies a different cell to the result */ \
do{ \
 case (CTTZI(sizeof(t)))*8+0: ((t*)base)[scan0[0]]=((t*)av)[0]; case (CTTZI(sizeof(t)))*8+1: ((t*)base)[scan0[1]]=((t*)av)[1];  /* copy cells */ \
 case (CTTZI(sizeof(t)))*8+2: ((t*)base)[scan0[2]]=((t*)av)[2]; case (CTTZI(sizeof(t)))*8+3: ((t*)base)[scan0[3]]=((t*)av)[3]; \
 case (CTTZI(sizeof(t)))*8+4: ((t*)base)[scan0[4]]=((t*)av)[4]; case (CTTZI(sizeof(t)))*8+5: ((t*)base)[scan0[5]]=((t*)av)[5]; \
 case (CTTZI(sizeof(t)))*8+6: ((t*)base)[scan0[6]]=((t*)av)[6]; case (CTTZI(sizeof(t)))*8+7: ((t*)base)[scan0[7]]=((t*)av)[7]; \
 scan0+=8; av=(C*)((t*)av+8);  /* advance pointers */ \
}while(--i0);
// cases 1ll0..: ll is lg2(atom length), data to move is in av (not *av)
#define CP1n(t)  /* each index copies the same cell to the result */ \
do{ \
 case (4+CTTZI(sizeof(t)))*8+0: ((t*)base)[scan0[0]]=(t)(I)av; case (4+CTTZI(sizeof(t)))*8+1: ((t*)base)[scan0[1]]=(t)(I)av;  /* copy cells */ \
 case (4+CTTZI(sizeof(t)))*8+2: ((t*)base)[scan0[2]]=(t)(I)av; case (4+CTTZI(sizeof(t)))*8+3: ((t*)base)[scan0[3]]=(t)(I)av; \
 scan0+=4;  /* advance pointers - but av is the value and does not advance */ \
}while(--i0);
// unusual length (preset by JMC) 10b1i0 i=0 to incr av, b=1 if no overstore allowed
#define CP1xv(bytelen,inc) case 0b100110+(bytelen<<3)-(inc<<1): DQNOUNROLL(n0, JMCR(base+cellsize**scan0++,av,cellsize,bytelen,endmask) if(inc)av+=cellsize;)  // copy n0 odd-length cells, advancing pointers to next input & output
// replicated cell of a
#define CPn1v case 0b100101: i0=n0;  /* no duff loop */ do{mvc(cellsize,base+*scan0++*cellsize,acatoms,av);}while(--i0);  /* each index replicates the same cell to fill result; don't increment input address */
// recursive indirect
#define CP11recur case 0b110100: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,*(A*)av) ++ix0; av+=sizeof(A*);)) }  // install *av++
#define CPn1recur case 0b110101: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; I cs=cellsize; do{A *avv=(A*)av; DQU(acatoms, INSTALLBOXNVRECUR(((A*)base),ix0,*avv) ++ix0; ++avv;)}while(cs-=acatoms);) }  // repeatedly install *av to fill single cell, don't advance av
#define CP1nrecur case 0b110110: { DQNOUNROLL(n0, I ix0=*scan0++*cellsize; A *avv=(A*)av; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,*avv) ++ix0; ++avv;)) }  // install *av, don't advance av
#define CP1narecur case 0b110111: { A ava=(A)av; DQNOUNROLL(n0, I ix0=*scan0++*cellsize; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,ava) ++ix0;)) }  // install av repeatedly without refetching

 // scatter-copy the data
 while(1){
  // loop over each combination of the last 2 axes
  if(--n1==0)goto skippre;
  do{
   C *basepre=amflags&AMFLAGINFULL1?base+axes[r-1].size:axes[r-1].base+axes[r-1].size**scan1++;  // prefetch next address of _1-cell being filled in result
skippre:;
   // move one _1-cell using the indexes.  We are in a loop through the _1-cells; each case in the switch below copies one _1-cell
   UI i0=n0;  /* number of duff loops for last axis */ \
   scan0-=((amflags>>AMFLAGDUFFWX)&0x7);  // pointer to first 0-cell index, biased by duff adj
   av-=cellsize*(amflags>>AMFLAGDUFFAX);  // bias output pointer too, but not if it is repeated
   switch(amflags&AMFLAGRTN){
   CP11neg break; CP1xvneg break; // negate FL atoms
   CP11(B) break; CP11(US) break; CP11(UI4) break; CP1n(B) break; CP1n(US) break; CP1n(UI4) break;
#if SY_64
   CP11(UI) break; CP1n(UI) break;
#endif
   CP1xv(1,0) break; CP1xv(1,1) break; CP1xv(0,0) break; CP1xv(0,1) break;
   CPn1v break; CP11recur break; CP1nrecur break; CPn1recur break; CP1narecur break;
   case 0b100111: case 0b101101:  // do-all copier when one of the last 2 axes is split.  Very rare.  100111 is normal, 101101 is recursive boxed
    DONOUNROLL(n0, if(amflags&8){I ix0=*scan0++*cellsize; A ava=(A)av; DQU(cellsize, INSTALLBOXNVRECUR(((A*)base),ix0,ava) ++ix0;)}
                                else JMC(base+cellsize**scan0++,av,cellsize,0)  // move 1 cell
                   av+=cellsize; if(((C)(amflags>>AMFLAGSPLITX))==(C)r&&(i+1)<n0&&(i+1)%axes[r].resetmod==0)av+=acresetcell;  // if axis -1 split, reset for it
    )
    if(((C)(amflags>>AMFLAGSPLITX))==(C)(r-1)&&n1!=0&&n1%axes[r-1].resetmod==0)av+=acresetcell;  // if axis -2 split, reset for it
   }
   base=basepre;  // use prefetch
   av+=avnreset;  // if we repeat a after _1-cell, do so.  If av holds a value, this will always add 0
   scan0+=scan0reset;  // if ind type, scan0 rolls along; otherwise reset after each repetition
  }while(--n1>0);
  if(n1==0)goto skippre;  // finish up last block if any
  // We have finished a cell in the last 2 axes.  Advance to next cell if any
  I rinc=r-2;  // odometer pointer to the cell being incremented
  while(1){
   if(rinc<=0)goto endamend;  // level 0 is the array in full.  It can't be incremented.  This is the loop exit, taken immediately if there are <=2 axes
   av+=axes[rinc+1].resetadder;  // if a resets for previous level, make that happen
   axes[rinc].scan++;  //  step to next position
   if(axes[rinc].scan!=(UI)axes[rinc].max){  // if wheel didn't roll over
    if(unlikely((C)rinc==(C)(amflags>>AMFLAGSPLITX))){
     // The very unusual case of a expiring mid-axis, which can happen when a selector has rank>1 (the bottom part of the selector matches the end of a).  When scan hits the reset point, we
     // back a to the start of the major cell and continue with the same selector.  This can happen for only one axis.  At the end we leave a pointing past the end of the cell
     if(axes[rinc].scan%axes[rinc].resetmod==0)av+=acresetcell;
    }
    break;  // stop odo if wheel didn't roll over
   }
   axes[rinc].scan=0;  // rollover.  reset the index pointer
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
  scan0=axes[r].indexes;   // reset the index pointer to the first index in the last axis, where the scatter-stores start
 }
endamend:;
 RETF(z);
}

// Convert list/table of indexes to array of cell offsets (the number of the atom starting the cell)
// w has rank > 0.  Result has shape }:$ind
// This is used by ind} and (<ind)}  which have different specs when ind is a list.  We use wframelen as a flag: if positive,
// we handle (<ind)} which treats ind as a 1-row table, otherwise ind} in which a list ind is treated like a table with rows of length 1, i. e. a list of atomic selectors
// The indexes are audited for validity and negative values
// This is like pind/aindex1 followed by pdt, but done in registers and without worrying about checks for overflow, since the result
// if valid will fit into an integer.
static A jtcelloffset(J jt,AD * RESTRICT w,AD * RESTRICT ind,I wframelen){A z=0;
 ARGCHK1(w);
 if(AR(ind)<2){
  // rank of ind is 0 or 1.  Treat as list of first-axis values (as is suitable for (<ind)}) unless ind} is specified and rank=1 and length>1; then treat as list of successive axes
  if((wframelen|(AN(ind)-2))<0)RZ(z=pind(AS(w)[~wframelen],ind));  // (m} only, or length<2) treat a list as a list of independent indexes.  pind handles that case quickly and possibly in-place.
  // if (<ind)} on list, leave z=0 and fall through to treat as table
 }
 if(likely(z==0)){  // if not handled already...
  wframelen^=REPSGN(wframelen);  // remove flag, leaving wframelen
  // rank of ind>1 (or = if (<ind)}), and rows of ind are longer than 1. process each row to a cell offset
  I naxes = AS(ind)[AR(ind)-1];
  I nzcells; PROD(nzcells,AR(ind)-1,AS(ind));
  if(!ISDENSETYPE(AT(ind),INT))RZ(ind=cvt(INT,ind));  // w is now an INT vector, possibly the input argument
  ASSERT(naxes<=AR(w)-wframelen,EVLENGTH);  // length of rows of table must not exceed rank of w
  GATV(z,INT,nzcells,AR(ind)-1,AS(ind)); I *zv=IAV(z);  // allocate result area, point to first cell location.
  I *iv=IAV(ind); // point to first row
  I *sv=&AS(w)[wframelen];  // axis lengths
  DQ(nzcells, I r=0; if(naxes){I i=0; I s=sv[i]; while(1){r*=s; I in=iv[i]; if((UI)in>=(UI)s){in+=s; ASSERT((UI)in<(UI)s,EVINDEX);} r+=in; if(++i==naxes)break; s=sv[i];}} *zv++=r; iv+=naxes;)  // carried dependency unimportant because short; use 0 if no axes
 }
 R z;
}

// axislen is length of axis, ind is doubly-boxed selector that is itself a boxed type, therefore complementary
// return list of indexes, or ds(CACE) if axis is taken in full
// used externally
// We return the actual indexes rather than a bitmask or the cleaned list of complementary indexes.  This is because
// (1) complementary amend is rare; (2) since we handle 2 axes at a time, many final routines would be needed to treat
// complemented axes separately 
static A jtcompidx(J jt,I axislen,A ind){
 ASSERT(!AR(ind),EVINDEX)  // contents must be a scalar box
 ind=C(AAV(ind)[0]);  // move to the contents
 if(AN(ind)==0)R ds(CACE);  // axis taken in full: return flag value
 ASSERT(axislen>0,EVINDEX)  // empty axis - no index would be valid, but we have indexes
 // there are values.  go through them, validating, and cross out the ones that are used
 // Since we expect the count to be small, we allocate a return block of the maximum size.  We then use the tail end
 // to hold a bitmask of the values that have not been crossed off
 RZ(ind=likely(ISDENSETYPE(AT(ind),INT))?ind:cvt(INT,ind));  // ind is now an INT vector, possibly the input argument
 I allolen=MAX(axislen,1);  // number of words needed.  There must be at least one value crossed off, but we always need at least 1 word for bitmask
                            // We also reserve one word of buffer between the indices and the bitmask since, in the case of (for example) (<<<_1) { i.65, we could end up overwriting the last word of the bitmask before reading it
                            // An alternative would be to pipeline the loop, loading the mask for the next iteration before completing the previous iteration
 A z; GATV0(z,INT,allolen,1) I *zv0=IAV1(z), *zv=zv0;   // allocate the result/temp block.  
 I bwds=(axislen+(BW-1))>>LGBW;  // number of words needed: one bit for each valid index value
 I *bv=zv+allolen-bwds; mvc(bwds*SZI,bv,SY_64?4*SZI:2*SZI,validitymask); bv[bwds-1]=~((~1ll)<<((axislen-1)&(BW-1)));  // fill the block with 1s to indicate we need to write; clear ending 0s
 I *iv=IAV(ind); DO(AN(ind), I ix=iv[i]; if((UI)ix>=(UI)axislen){ix+=axislen; ASSERT((UI)ix<(UI)axislen,EVINDEX)} bv[ix>>LGBW]&=~(1ll<<(ix&(BW-1))); )  // turn off the bit for each index
 I zbase=0; DO(bwds, I bmask=*bv++; while(bmask){I bitno=CTTZI(bmask); *zv++=zbase+bitno; bmask&=bmask-1;} zbase+=BW;)  // copy an index for each remaining bit, clearing LSBs one by one
 AN(z)=AS(z)[0]=zv-zv0;
 R z;
}

// Execution of x m}"r y.  Split on sparse/dense, passing on the dense to merge2, including inplaceability
A jtamendn2(J jt,A a,A w,AD * RESTRICT ind,A self){F2PREFIP;PROLOG(0007);A e,z; I atd,wtd,t,t1;P*p;
  // ind=m, the indexes to be modified
 ARGCHK3(a,w,ind);
 I acr=jt->ranks>>RANKTX; acr=AR(a)<acr?AR(a):acr; 
 I wcr=(RANKT)jt->ranks; wcr=AR(w)<wcr?AR(w):wcr; RESETRANK;
 I at=AT(a), wt=AT(w), indt=AT(ind);
 I cellframelen,cellx,indframe;  // for single-cell amend: frame of cell; its index; number of surplus leading axes of 1s in selector
 if(likely(!ISSPARSE(wt|indt))){
  // non-sparse.
  I ar=AR(a), wr=AR(w), aframelen=ar-acr,wframelen=wr-wcr;  // number of axes in frame
  // handle fast and common case, where ind selects a single non-DIRECT cell (must be no frame), and not -@{`[`]}
  I notonecelldirect=aframelen+wframelen+(wt&~DIRECT)+SGNTO0(AN(a));  // nonzero if cannot use single-cell code
  if((notonecelldirect+(AN(ind)^1)+(indt&~NUMERIC))==0){
   // get cell index and rank, and audit for validity
   if(unlikely(!(indt&INT+B01)))RZ(ind=cvt(INT,ind))  // index of the single cell
   I axlen=AS(w)[0]; cellframelen=wr>0; axlen=wr>0?axlen:1;  // len of frame of cell, and axis length
   cellx=BIV0(ind); cellx+=axlen&REPSGN(cellx); ASSERT(BETWEENO(cellx,0,axlen),EVINDEX)
   indframe=AR(ind);  // ind is a singleton, but it may have any number of leading axes of 1.  This is the count of leading axes
onecellframe:;   // come here when we detect single cell, possibly of higher rank.  indframe, cellframelen and cellx must be set; ind must not be used below
   I cellsize; PROD(cellsize,wr-cellframelen,AS(w)+cellframelen)  // size of cell
   // the selected are has shape ($ind),}.$w and must match the shape of a.  $ind is known to be all 1s.
   // so, a must match w for the smaller of (#$a, <:#$w) axes; then any remaining axes of a must be 1s; and no axes of a beyond ind.
   I matchrank=MIN(ar,wr-cellframelen);  // the number of axes of a and w that must match
   if(unlikely(ar-matchrank!=0)){ASSERT(ar<=indframe+wr-cellframelen,EVRANK) DO(ar-matchrank, ASSERT(AS(a)[i]==1,EVLENGTH))}  // 1s in higher axes
   ASSERTAGREE(AS(a)+ar-matchrank,AS(w)+wr-matchrank,matchrank)  // verify a can be replicated to fill the result cell
   // convert args as needed and check for inplaceability
   if(unlikely(cellsize==0))R RETARG(w);  // if nothing to install, return arg unchanged
   ASSERT((POSIFHOMO(AT(a),AT(w)))>=0,EVINHOMO);  // error if xy both not empty and not compatible
   I t=maxtyped(AT(a),AT(w));  // get the type of the result: max of types
   if(unlikely(TYPESNE(t,AT(a))))RZ(a=cvt(t,a));  // if a must change precision, do so
   I k=bplg(t);  // lg2 of an atom of result
   // inplaceability is explained in the main logic in merge2n
   if(ASGNINPLACESGNNJA(SGNIF(jtinplace,JTINPLACEWX),w)
      &&( ((AT(w)&t)>0)&(w!=a)&((w!=ABACK(a))|(~AFLAG(a)>>AFVIRTUALX)) )){ASSERT(!(AFRO&AFLAG(w)),EVRO); z=w;}  // inplaceable, use it.  w==ind OK
   else{RZ(z=cvt(t,w));}  // copy old block, converting if needed
   mvc(cellsize<<k,CAV(z)+(cellsize<<k)*cellx,AN(a)<<k,voidAV(a)); // copy a to the cell, replicating as needed
   RETF(z);
  }

  // fast path not available.  Build axes
  ASSERTAGREE(AS(a),AS(w),MIN(aframelen,wframelen));  // if rank, check agreement
  I nframeaxes;  // will be # axes added to accommodate outer frame
  z=0;  // use z to hold reworked ind.  Error if not reset

  // Now we create z, which will hold the info to be passed into merge2.  
  // numeric m turns into a single list of validated indexes, as does singly-boxed numeric m; double-boxed m turns into
  // a list of boxes, one for each index.
  // We also need to know the frame of the cells, in cellframelen
  struct axis *axes, localaxes[6]; A alloaxes;  // put axes here usually
  I *ws=AS(w); I indr=AR(ind);
  A ind0;  // the contents that have the numbers
  if(unlikely(wcr==0)){
   // wcell is an atom.  The best you can get is indexes of 0, and that's if all the selectors are 0.  Use from to check for validity.  No axes are used
   RZSUFF(z=from((SGNIF(AT(ind),BOXX)|(indr-2))<0?ind:box(ind),zeroionei(0)),R jteformat(jt,self,a,w,ind);); cellframelen=0;  // if numeric array rank>1, box before the call (i. 1 0 e. g.)
  }else if((-AN(ind)&SGNIF(AT(ind),BOXX))>=0){
   // ind is empty or not boxed.  If it is a list, audit it and use it.  If it is a table or higher, convert to cell indexes.  It will be used to fill in axis struct in merge2
   cellframelen=indr<2?1:AS(ind)[indr-1];  // #axes used: 1, if m is an atom/list; otherwise {:$m
   if(indr==0){  // scalar ind is common enough to test for
    if(!ISDENSETYPE(AT(ind),INT)){A tind; RZSUFF(tind=cvt(INT,ind),R jteformat(jt,self,a,w,ind);); ind=tind;}  // ind is now an INT vector, possibly the input selector
    if(likely((UI)IAV(ind)[0]<(UI)ws[wframelen]))z=ind; else{ASSERTSUFF(IAV(ind)[0]<0,EVINDEX,R jteformat(jt,self,a,w,ind);); ASSERTSUFF(IAV(ind)[0]+ws[wframelen]>=0,EVINDEX,R jteformat(jt,self,a,w,ind);); RZ(z=sc(IAV(ind)[0]+ws[wframelen]));}  // if the single index is in range, keep it; if neg, convert it quickly
   }else{  // ind is numeric list/array
    if(unlikely(AN(ind)+aframelen==0)){  // no modification: leave arg unchanged.  Worth 1 lousy instruction to test for
     if(indr==1){  // rank>1 case not worth handling
      ASSERT(acr<=wcr,EVRANK);   // max # axes in a is the axes in w, plus any surplus axes of m that did not go into selecting cells
      I compalen=MAX(0,acr-(wcr-cellframelen)); I *as=AS(a)+aframelen, *ws=AS(w)+wframelen;  // #axes of a that are outside of the cell in w
      ASSERTAGREE(as,AS(ind)+1-compalen,compalen);  // shape of m{y is the shape of m, as far as it goes.  The first part of a may overlap with m
      ASSERTAGREE(as+compalen,ws+wcr-(acr-compalen),acr-compalen)  // the rest of the shape of m{y comes from shape of y
      R RETARG(w);
     }
    }
    if((SGNTO0(indr-2)+notonecelldirect+(AN(ind)^cellframelen))==0){ind0=ind; goto indexforonecell;}  // if ind has rank>1, it's index lists: check for only 1 cell that can be processed quickly and xctl to it
    RZSUFF(z=jtcelloffset(jt,w,ind,~wframelen),R jteformat(jt,self,a,w,ind);); //  otherwise, create (or keep) list of cell indexes, of rank cellframelen.  ~ indicates ind} semantics
   }
  }else if(unlikely(indr!=0)){
   // All this is deprecated, should be domain error
   // ind is a list of boxes.  The contents had better all be numeric, and opening them must not use fill
   RZ(ind0=jtope((J)((I)jtinplace|JTNOFILL),ind)) ASSERT(AT(ind0)&NUMERIC,EVDOMAIN);  // open; contents must be numeric
   if(AR(ind0)==indr){
    if(JT(jt,deprecct)!=0)RZ(jtdeprecmsg(jt,1,"(001) (x (<\"0 array)} y): consider using (<<array)}\n"));
    goto doubleboxednumeric;  // the boxes were created with <"0 array.  That is the same as <<array now
   }else if(AR(ind0)==indr+1){
    if(JT(jt,deprecct)!=0)RZ(jtdeprecmsg(jt,2,"(002) (x (<\"1 array)} y): consider using (<array)} or array}\n"));
    goto boxednumeric;  // the boxes were created with  <"1 array, which is like <array now
   }else ASSERT(0,EVRANK)  // error if not <"0 or <"1
  }else{
   // ind is a single box, <selectors.  It must have rank 0 unless it is a deprecated form
   ind0=C(AAV(ind)[0]);  // discard ind, move to selectors
   if(AT(ind0)&BOX){
    // selectors are boxed.  They have selectors for sequential axes.  Put them into a multidimensional axis struct.  In this struct a pointer of 0 means
    // an axis taken in full
    ASSERTSUFF(AR(ind0)<2,EVRANK,R jteformat(jt,self,a,w,ind);); ASSERTSUFF(AN(ind0)<=wcr,EVLENGTH,R jteformat(jt,self,a,w,ind););   // array of axes must have rank<2, and must not exceed #axes in w
    if(AN(ind0)<2){
     // 0-1 selectors, turn it into an ind if it contains a list/atom or a box.  If it contains a table or higher, revert to general case
     if(AN(ind0)!=0){
      // the single selector is a singleton box <indexes or <<compindexes
      ind0=C(AAV(ind0)[0]);  // advance ind0 to indexes or <compindexes
      if(!(AT(ind0)&BOX))doubleboxednumeric: z=pind(ws[wframelen],ind0);  // not boxed - get/keep index list for first axis
      else{  // <<compidexes
       RZSUFF(z=jtcompidx(jt,ws[wframelen],ind0),R jteformat(jt,self,a,w,ind););   // resolve the comp indexes
       if(unlikely(z==ds(CACE)))RZ(z=IX(ws[wframelen]))  // rare (<<<'')} - must instantiate the in-full index vector
      }
      cellframelen=1;  // in this path, the cells are _1-cells
     }else{
      // empty list of selectors, that means 'all taken in full' - one selection of the whole.
      cellframelen=0; z=zeroionei(0);  // select everything, unusually - as cell 0 of an array that is the whole
     }
    }else{I i; I indn=AN(ind0); A *indv=AAV(ind0);  // number of axes and pointer to first
     // At least 2 axes given.  use/allocate the axis struct
     I awframelen=MAX(wframelen,aframelen);  // if a frame is longer than wframe, we are going to have to copy w to match it; use the longer
     // The axis block has one entry for the whole array, one for each axis in ind, one if there is wframe, and one if there is inner wframe (repeated cells of a)
     nframeaxes=(awframelen!=0)+BETWEENO(aframelen,1,awframelen);  // number of axes added for frame
     I indn1=indn+nframeaxes;  // indn1 is #axis blocks needed - 1
     if(likely((UI)indn1<sizeof(localaxes)/sizeof(localaxes[0]))){axes=localaxes;}else{GATV0(alloaxes,INT,(indn1+1)*(sizeof(localaxes)/(sizeof(I))),RCALIGN); axes=(struct axis *)voidAVCACHE(alloaxes);}  // allo as needed.  Rank 1 for cache alignment
     // fill in the struct for each axis in ind.  check indexes for validity (allocating a new block if any are negative); handle complementaries, and axes taken in full
     A ax;  // holds the index block for the current axis.  At end of loop, holds the ind block for axis 0, after any complementation
     I lastn=indn1;  // number of axis blocks in use after trailing taken-in-full removed
     for(i=indn+wframelen-1;i>=wframelen;--i,--indn1){  // for each axis, rolling up from the bottom.  The input axis in w is i, in ind is i-wframelen.  the output axis slot is indn1 (counting back from last slot);
      ax=C(indv[i-wframelen]);  // point to the index block
      I axlen=AS(w)[i];  // length of axis
      axes[indn1].size=axlen;  // save in axis struct
      axes[indn1].scan=0;  // start at beginning of indexes
      axes[indn1].resetadder=0;  // init to no reset at end of axis
      // resolve complementary index
      if((-AN(ax)&SGNIF(AT(ax),BOXX))>=0){   // notempty and boxed means complementary indexing
       // not complementary indexing: create or keep valid integer index selectors
       // the selector may have any shape.  If it is not a list, it will affect the rank of m{y.  We store into .ind to indicate the shape of the selector
       RZSUFF(ax=pind(axlen,ax),R jteformat(jt,self,a,w,ind););  // audit selectors
       axes[indn1].max=AN(ax);  // note how many selectors there are
       axes[indn1].indexes=IAV(ax);  // save pointer to the indexes
      }else{
       // here for complementary indexing
       RZSUFF(ax=jtcompidx(jt,axlen,ax),R jteformat(jt,self,a,w,ind););   // resolve the comp indexes
       if(ax!=ds(CACE)){  // if not taken in full...
        axes[indn1].max=AN(ax); axes[indn1].indexes=IAV(ax);  // set number of indexes, and their address
       }else{     // axis taken in full.  Tag with 0 pointer.  BUT the last axis cannot be taken in full: if we try, discard all trailing in-full axes  
        if(likely(indn1!=lastn)){    // non-trailing axis
         axes[indn1].max=axlen; axes[indn1].indexes=0; ax=mtv;   // total axis length, and 0 pointer to mean 'in full'; in-full always looks like a rank-1 selector
        }else{   // trailing axis taken in full: delete it
         if(likely(i!=wframelen))--lastn;else{RZ(ax=IX(axlen)) axes[indn1].max=AN(ax); axes[indn1].indexes=IAV(ax);}   // if rare (<<<'') in all axes, must instantiate the index vector & install in case there is rank
        }
       }
      }
      axes[indn1].ind=ax;  // save the selectors given or created - only the rank is used
     }
     if(unlikely(awframelen!=0)){
      // There is frame.  Fill in the first one or two axes
      axes[1].scan=0; axes[1].indexes=0; axes[1].ind=mtv;  // axis in full, and advance to next outer cell of a after filling it.  In-full axes are always rank 1
        // #cells in the common frame of w and a
      // if wframe>aframe, 2 loops are required, the second repeating major cells of a.  Cell2 3 and above have been filled in.
      if(unlikely(BETWEENO(aframelen,1,awframelen))){
       axes[2].scan=0; axes[2].indexes=0; axes[2].ind=mtv; // axis in full, and repeat the cell of a; In-full axis has rank 1
       PROD(axes[2].size,wframelen-aframelen,AS(w)+aframelen) axes[2].max=axes[2].size;  // #cells is the size of surplus frame of w
       PROD(axes[1].size,aframelen,AS(w)) axes[1].max=axes[1].size;   // shared frame gives # outer cells of a
       PROD(axes[2].resetadder,acr,AS(a)+aframelen)  // inner cell gives size of each one; roll over after repeats
      }else{
       // aframelen is 0 or = wframelen, only one loop is needed: it either always or never increments a.  Cells 2 and above have been filled in.
       PROD(axes[1].size,wframelen,AS(w))   // shared frame gives # cells of w
       if(unlikely(aframelen>wframelen)){I acells; PROD(acells,aframelen-wframelen,AS(a)+wframelen) axes[1].size*=acells;}  // if w is going to be replicated, predict that
       axes[1].max=axes[1].size;  // taken in full, so size=max
       if(unlikely(aframelen!=0)){PROD(axes[2].resetadder,acr,AS(a)+aframelen)}  // if a is all nonrepeated, advance after next axis rolls over.  If all repeated, leave unincremented
      }
     }
     if(likely(lastn>=2)){  // we need at least 2 axes for the amend loop
      // here we have succeeded in using the axes.  Indicate that fact by taking the ones comp of the framelen.
      cellframelen=~(lastn-nframeaxes); z=(A)axes;
     }else{
      // rare case where so many trailing axes were deleted that we don't have 2 axes left.  Revert to ind form, where ax is known to have the ind for axis 0
      cellframelen=lastn; z=ax;  // the single axis must be complementary
     }
    }
   }else{
boxednumeric:
    // Singly-boxed contents.   They must be an array of index lists; convert to array of cell indexes
    cellframelen=AS(ind0)[AR(ind0)-1]; cellframelen=AR(ind0)==0?1:cellframelen;  // length of a row of ind0 - number of axes used
    ASSERTSUFF(cellframelen<=wcr,EVLENGTH,R jteformat(jt,self,a,w,ind););  // can't have more selectors than axes
    if((notonecelldirect+(AN(ind0)^cellframelen))==0){
     // there is only one cell, and it can be handled by the fast code (i. e. direct and not -@{`[`]}).  The rank of the array it is in is immaterial, except for agreement with a
     // empty list (more precisely, empty 1-cells) also come through here for DIRECT one-cell types, and take the array in full (0 axes, with the rest taken in full)
indexforonecell:;  // cellframelen, wframelen (always 0), and ind0 must be set
     indframe=AR(ind0)-1; indframe^=REPSGN(indframe);  // number of leading singleton axes, to compare for agreement
     if(unlikely(!(AT(ind0)&INT)))RZ(ind0=cvt(INT,ind0));  // index must be integer
     cellx=0; DO(cellframelen, I axn=AS(w)[i]; I j=IAV(ind0)[i]; j+=axn&REPSGN(j); ASSERT(BETWEENO(j,0,axn),EVINDEX) cellx*=axn; cellx+=j;)  // convert the indexes to a single cell-index
     goto onecellframe; // xctl to the code that handles 1-cell amend
    }
    // Convert the array of index lists to an array of cell indexes
    if(likely(cellframelen!=0)){RZSUFF(z=jtcelloffset(jt,w,ind0,wframelen),R jteformat(jt,self,a,w,ind););}else{z=zeroionei(0);}  // if empty list, that means 'all taken in full' - one selection of the whole.  Otherwise convert the list to indexes
   }
  }
  z=jtmerge2(jtinplace,ISSPARSE(AT(a))?denseit(a):a,w,z,(cellframelen&255)+(wframelen<<RANKTX)+(aframelen<<RANK2TX)+(nframeaxes<<(3*RANKTX)));  //  dense a if needed; dense amend
  if(unlikely(z==0))jteformat(jt,self,a,w,ind);  // eformat this error while we have access to ind
  // We modified w which is now not pristine.
  PRISTCLRF(w)
  EPILOG(z);
 }
 // Otherwise, w is sparse
 // ?t = underlying type of ?, s?=nonzero if sparse
 F2RANK(acr,wcr,jtamendn2,self);
 atd=ISSPARSE(AT(a))?DTYPE(AT(a)):AT(a); wtd=ISSPARSE(AT(w))?DTYPE(AT(w)):AT(w);
 ASSERT(AT(ind)&NUMERIC+BOX||!AN(ind),EVDOMAIN);
 ASSERT(!ISSPARSE(AT(ind)),EVNONCE);  // m must be dense, and numeric or boxed
 if(AR(ind)==0&&AT(ind)&BOX){A ind0=(A)C(AAV(ind)[0]); if(AT(ind0)&NUMERIC&&AR(ind0)>1)ind=ind0;}  // if <array, extract array, which we will box presently
 if(AT(ind)&NUMERIC){  // numeric must have rank <3; rank 2 is scatter amend
  ASSERT(AR(ind)<3,EVRANK);
  if(AR(ind)==2){
   // rank-2 numeric ind.  This is scatter amend; process as as <"1 ind
   if(AN(ind)==0){
    // empty scatter amend.  <"1 ind doesn't preserve the length of the rows of ind, which we would need to calculate agreement
    // calculate agreement: ($a)-:(-#$a){.!._1(#ind),({:$ind)}.$w
    A temp; ASSERT(equ(shape(a),dfv2(temp,negate(jtrank(jt,a)),over(tally(ind),drop(tail(shape(ind)),shape(w))),fit(ds(CTAKE),num(-1)))),EVLENGTH);
    EPILOG(w);  // nothing to amend - return w
   }else{A aa=ind; RZ(ind=IRS1(aa,0,1,jtbox,ind));  // ind =. <"1 ind
   }
  }  // Convert empty 2-d to atom so aindex doesn't fail
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
 p=PAV(z); e=SPA(p,e); B b=!AR(a)&&equ(a,e);   // b means 'amending with sparse element'
 p=PAV(a); if(unlikely(ISSPARSE(AT(a))&&!equ(e,SPA(p,e)))){RZ(a=denseit(a)); }
 if(AT(ind)&NUMERIC||!AR(ind))z=(b?jtam1e:ISSPARSE(AT(a))?jtam1sp:jtam1a)(jt,a,z,AT(ind)&NUMERIC?box(ind):ope(ind),ip);
 else{RZSUFF(ind=aindex(ind,z,0L),z=0; goto exitra;); ind=(A)((I)ind&~1LL); ASSERTSUFF(ind!=0,EVNONCE,z=0; goto exitra;); z=(b?jtamne:ISSPARSE(AT(a))?jtamnsp:jtamna)(jt,a,z,ind,ip);}  // A* for the #$&^% type-checking
exitra:
 if(ip)ra(w);
 EPILOGZOMB(z);   // do the full push/pop since sparse in-place has zombie elements in z
}
static A jtamendn2c(J jt,A a,A w,A self){R jtamendn2(jt,a,w,VAV(self)->fgh[0],self);}  // entry point from normal compound

// Execution of x u} y.  Call (x u y) to get the indices, convert to cell indexes, then
// call merge2 to do the merge.  Pass inplaceability into merge2.
static DF2(amccv2){F2PREFIP;A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1];
 ARGCHK2(a,w); 
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);  // u} not supported for sparse
 A x;RZ(x=pind(AN(w),CALL2(f2,a,w,fs)));
 A z=jtmerge2(jtinplace,a,w,x,AR(w));   // The atoms of x include all axes of w, since we are addressing atoms
 if(unlikely(z==0))jteformat(jt,self,a,w,x);  // eformat this error while we have access to x
 // We modified w which is now not pristine.
 PRISTCLRF(w)
 RETF(z);
}


static DF1(mergn1){A ind,z; z=merge1(w,ind=VAV(self)->fgh[0]); if(unlikely(z==0))jteformat(jt,self,w,0,ind); R z;}
static DF1(mergv1){A fs=FAV(self)->fgh[0]; AF f1=FAV(fs)->valencefns[0];A ind,z; z=merge1(w,ind=CALL1(f1,w,fs)); if(unlikely(z==0))jteformat(jt,self,w,0,ind); R z;}

// called from m}, m is usually NOT a gerund
static B ger(J jt,A w){A*wv,x;
 if(likely(!(BOX&AT(w))||!AN(w)))R 0;
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
 } else if(likely(AT(w)&BOX)) {A *wv;I bmin=0,bmax=0;
  // boxed contents.  There must be exactly 2 boxes.  The first one may be a general AR; or the special cases singleton 0, 2, 3, or 4
  // Second may be anything for special case 0 (noun); otherwise must be a valid gerund, 1 or 2 boxes if first box is general AR, 2 boxes if special case
  // 2 (hook) or 4 (bident), 3 if special case 3 (fork)
  // 
  if(unlikely(!(n==2)))R 0;  // verify 2 boxes
  wv=AAV(w);  x=C(wv[0]); // point to pointers to boxes; point to first box contents
  // see if first box is a special flag
  if((SGNIF(AT(x),LITX)&(AR(x)-2)&((AN(x)^1)-1))<0){ // LIT, rank<2, AN=1
   c = CAV(x)[0];   // fetch that character
   if(c=='0')R 1;    // if noun, the second box can be anything & is always OK, don't require AR there
   I oride=2+(c&1);  // 2 if '2'/'4', 3 if '3'
   bmin=BETWEENC(c,'2','4')?oride:bmin; bmax=BETWEENC(c,'2','4')?oride:bmax; 
  }
  // If the first box is not a special case, it had better be a valid AR; and it will take 1 or 2 operands
  if(bmin==0){if(unlikely(!(gerar(jt,x))))R 0; bmin=1,bmax=2;}
  // Now look at the second box.  It should contain between bmin and bmax boxes, each of which must be an AR
  x=C(wv[1]);   // point to second box
  if(unlikely((SGNIF(AT(x),BOXX) & ((AR(x)^1)-1))>=0))R 0;   // verify it contains a list of boxes
  if(unlikely(!BETWEENC(AN(x),bmin,bmax)))R 0;  // verify correct number of boxes
  R gerexact(x);  // recursively audit the other ARs in the second box
 } else R 0;
 R 1;
}

B jtgerexact(J jt, A w){A*wv;
 if(unlikely(!(BOX&AT(w))))R 0;   // verify gerund is boxed
 if(unlikely(!(AN(w))))R 0;   // verify there are boxes
 wv=AAV(w);   // point to pointers to contents
 DO(AN(w), if(!(gerar(jt, C(wv[i]))))R 0;);   // fail if any box contains a non-gerund
 R 1;
}    /* 0 if w is definitely not a gerund; 1 if possibly a gerund */

// verb executed for v0`v1`v2} y
static DF1(jtgav1){V* RESTRICT sv=FAV(self); A ff,ffm,ffx,*hv=AAV(sv->fgh[2]);
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow in the loop in case the generated ff generates a recursive call to }
 // If the AR is a noun, just leave it as is
 I hn=AN(sv->fgh[2]);  // hn=# gerunds in h
 dfv1(ffm,w,hv[hn-2]);  // x v1 y - no inplacing
 RZ(ffm);
 RZ(ff=jtamend(jt,ffm,0));  // now ff represents (v1 y)}.  scaf avoid this call, go straight to mergn1
// obsolete  if(AT(hv[2])&NOUN){ffx=hv[2];}else{
 RZ(dfv1(ffx,w,hv[hn-1]))
 R dfv1(ffm,ffx,ff);
}

// verb executed for x v0`v1`v2} y
static DF2(jtgav2){F2PREFIP;V* RESTRICT sv=FAV(self); A ff,ffm,ffx,ffy,*hv=AAV(sv->fgh[2]);  // hv->verbs, already converted from gerunds
 A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); // protected addresses
 I hn=AS(sv->fgh[2])[0];  // hn=# gerunds in h
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow in the loop in case the generated ff generates a recursive call to }
 dfv2(ffm,a,w,hv[1]);  // x v1 y - no inplacing.
 RZ(ffm);
 RZ(ff=jtamend(jt,ffm,0));  // now ff represents(x v1 y)} .  0 indicates this recursive call into powop, which will cause nonce error if v1 returned another gerund
 // Protect any input that was returned by v1 (must be ][)
 if(a==ffm)jtinplace = (J)(intptr_t)((I)jtinplace&~JTINPLACEA); if(w==ffm)jtinplace = (J)(intptr_t)((I)jtinplace&~JTINPLACEW);
 PUSHZOMB
 // execute the gerunds that will give the arguments to ff.  But if they are nouns, leave as is
 // x v2 y - can inplace an argument that v0 is not going to use, except if a==w
 if(hn>=3){RZ(ffy = (FAV(hv[2])->valencefns[1])(a!=w&&(FAV(hv[2])->flag&VJTFLGOK2)?(J)(intptr_t)((I)jtinplace&(sv->flag|~(VFATOPL|VFATOPR))):jt ,a,w,hv[2]));  // flag self about f, since flags may be needed in f
 }else{ffy=w;}  // if v2 omitted or ], just use y directly
 // x v0 y - can inplace any unprotected argument
 RZ(ffx = (FAV(hv[0])->valencefns[1])((FAV(hv[0])->flag&VJTFLGOK2)?((J)(intptr_t)((I)jtinplace&((ffm==w||ffy==w?~JTINPLACEW:~0)&(ffm==a||ffy==a?~JTINPLACEA:~0)))):jt ,a,w,hv[0]));
 // execute ff, i. e.  ffx (x v1 y)} ffy .  Allow inplacing xy unless protected by the caller.  No need to pass WILLOPEN status, since the verb can't use it.  ff is needed to give access to m
// obsolete  POPZOMB; R (FAV(ff)->valencefns[1])(FAV(ff)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|((ffx!=protw&&ffx!=prota?JTINPLACEA:0)+(ffy!=protw&&ffy!=prota?JTINPLACEW:0))) ):jt,ffx,ffy,ff);
 POPZOMB; R ((AF)jtamendn2c)(FAV(ff)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|((ffx!=protw&&ffx!=prota?JTINPLACEA:0)+(ffy!=protw&&ffy!=prota?JTINPLACEW:0))) ):jt,ffx,ffy,ff);
}

// handle v0`v1[`v2]} to create the verb to process it when [x] and y arrive
static A jtgadv(J jt,A w){A hs;I n;
 ARGCHK1(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 I wn=AN(w); ASSERT(BETWEENC(wn,2,3),EVLENGTH);  // verify 2-3 gerunds
// obsolete RZ(hs=fxeachv(1,3==AN(w)?w:behead(reshape(num(4),w))));   // convert to v1`v0`v1 or v0`v1`v2; convert each gerund to verb
 RZ(hs=fxeachv(1,w));   // convert to v1`v0`v1 or v0`v1`v2; convert each gerund to verb
 // hs is a BOX array, but its elements are ARs and cannot be pyxes
 // The derived verb is ASGSAFE if all the components are; it has gerund left-operand; and it supports inplace operation on the dyad
 I alr=atoplr(AAV(hs)[0]);   // Also set the LSB flags to indicate whether v0 is u@[ or u@]
 I flag=VASGSAFE;  // where we will build verb flags, inited as if wn<3
 if(wn==3){if(FAV(AAV(hs)[2])->id==CRIGHT)AS(hs)[0]=2; else flag=FAV(AAV(hs)[2])->flag;}  // if v2=], remove it from gerund count in shape (leave in AN for free); if 3 gerunds, init from v2
 flag=(flag&FAV(AAV(hs)[0])->flag&FAV(AAV(hs)[1])->flag&VASGSAFE)+(VGERL|VJTFLGOK2)+(alr-2>0?alr-2:alr);  // wn may be 2 or 3
 R fdef(0,CRBRACE,VERB, jtgav1,jtgav2, w,0L,hs,flag, RMAX,RMAX,RMAX);  // create the derived verb
}

// Execution of x -@:{`[`]}"r y
static DF2(jtamnegate){F2PREFIP;
 ARGCHK2(a,w); 
 // if y is CMPX/FL/QP, execute markd x} y which means negate
 if(AT(w)&FL+CMPX+QP)R jtamendn2(jtinplace,markd((AT(w)>>FLX)&(FL+CMPX>>FLX)),w,a,self);
 // otherwise, revert to x -@:{`[`]}"r y, processed by jtgav2
 US ranks=jt->ranks; RESETRANK;
 R rank2exip(a,w,self,AR(a),MIN((RANKT)ranks,AR(w)),AR(a),MIN((RANKT)ranks,AR(w)),jtgav2);
}

// u} handling.  This is not inplaceable but the derived verb is.  Self can be 0 to indicate this is a recursive call from a subroutine of jtamend
DF1(jtamend){F1PREFIP;
 ARGCHK1(w);
 if(unlikely(AT(w)&VERB)) R fdef(0,CRBRACE,VERB,(AF)mergv1,(AF)amccv2,w,0L,0L,VASGSAFE|VJTFLGOK2, RMAX,RMAX,RMAX);  // verb} 
 else if(AT(w)&BOX&&ger(jt,w)){A z;
  ASSERT(self!=0,EVNONCE);  // execute exception if gerund returns gerund
  RZ(z=gadv(w))   // get verbs for v0`v1`v2}, as verbs
  A *hx=AAV(FAV(z)->fgh[2]);
  if((FAV(hx[0])->id&~1)==CATCO&&FAV(FAV(hx[0])->fgh[0])->id==CMINUS&&AT(FAV(hx[0])->fgh[1])&VERB&&FAV(FAV(hx[0])->fgh[1])->id==CFROM&&FAV(hx[1])->id==CLEFT&&(AS(FAV(z)->fgh[2])[0]<3||FAV(hx[2])->id==CRIGHT)){
   FAV(z)->valencefns[1]=jtamnegate;    // if gerund is -@[:]{`[ [`]], change the dyad function pointer to the special code
   FAV(z)->flag|=VIRS2;  // also support IRS for this case
  }
  R z;
 }else R fdef(0,CRBRACE,VERB,(AF)mergn1,(AF)jtamendn2c,w,0L,0L,VASGSAFE|VJTFLGOK2|VIRS2, RMAX,RMAX,RMAX);   // m}"r with IRS
}
