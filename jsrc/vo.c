/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Box & Open                                                       */
#include "j.h"
#define ZZDEFN
#include "result.h"

I level(A w){A*wv;I d,j;
 if((-AN(w)&-(AT(w)&BOX+SBOX))>=0)R 0;
 d=0; wv=AAV(w);
 DO(AN(w), j=level(wv[i]); d=d<j?j:d;);
 R 1+d;
}

// return 0 if the level of w is greater than l, 1 if <=
// terminates early if possible
I levelle(A w,I l){
 if((-AN(w)&-(AT(w)&BOX+SBOX))>=0)R SGNTO0(~l);  // if arg is unboxed, its level is 0, so return 1 if l>=0
 if(l<=0)R 0;  // (arg is boxed) if l is <=0, arglevel is  > l
 --l; A *wv=AAV(w);
 DO(AN(w), if(!levelle(wv[i],l))R 0;);  // stop as soon as we see level big enough
 R 1;  // if it never gets big enough, say so, keep looking
}

F1(jtlevel1){RZ(w); R sc(level(w));}

F1(jtbox){A y,z,*zv;C*wv;I f,k,m,n,r,wr,*ws; 
 RZ(w); I wt=AT(w); FLAGT waf=AFLAG(w);
 ASSERT(!(SPARSE&wt),EVNONCE);
 FLAGT newflags = 0;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r;   // no RESETRANK because we call no primitives
 if(!f){
  // single box: fast path.  Allocate a scalar box and point it to w.  Mark w as incorporated
  // DO NOT take potentially expensive pass through w to find recursibility, because it may never be needed if this result expires without being assigned
  GAT0(z,BOX,1,0); INCORP(w); *(AAV(z))=w;
  if((waf&RECURSIBLE)||(wt&DIRECT)){newflags|=BOX; ACINCR(w);}  // if w is recursible or direct, mark new box recursible and correspondingly incr usecount of w.  We do this because w is already in cache now.
  AFLAG(z) = newflags;  // set NOSMREL if w is not boxed, or known to contain no relatives
 } else {
  // <"r
  ws=AS(w); I t=AT(w);
  CPROD(AN(w),n,f,ws); CPROD(AN(w),m,r,f+ws);
  k=m<<bplg(t); wv=CAV(w);
  GATV(z,BOX,n,f,ws); zv=AAV(z); 
   // The case of interest: non-relative w.  We have allocated the result; now we allocate a block for each cell of w and copy
   // the w values to the new block.  We set NOSMREL in the top block and the new ones if w is boxed or NOSMREL.  
   // If w is DIRECT, we make the result block recursive and increment the count of the others (we do so here because it saves a traversal of the new blocks
   // if the result block is assigned).  If w is indirect, we just leave everything nonrecursive to avoid traversing w, because
   // that will prove unnecessary if this result is not assigned.

  if(t&DIRECT){
   // Direct w.
   AFLAG(z) = newflags|BOX;  // Make result inplaceable and recursive
   // Since we are making the result recursive, we can save a lot of overhead by NOT putting the cells onto the tstack.  As we have marked the result as
   // recursive, it will free up the cells when it is deleted.  We want to end up with the usecount in the cells being 1, not inplaceable.  The result itself will be
   // inplaceable with a free on the tstack.
   // To avoid the tstack overhead, we switch the tpush pointer to our data area, so that blocks are filled in as they are allocated, with nothing put
   // onto the real tpop stack.  If we hit an error, that's OK, because whatever we did get allocated will be freed when the result block is freed.  We use GAE so that we don't abort on error
   A *pushxsave = jt->tnextpushp; jt->tnextpushp=zv;  // save tstack info before allocation
   DQ(n, GAE(y,t,m,r,f+ws,break); AFLAG(y)=newflags; AC(y)=ACUC1; MC(CAV(y),wv,k); wv+=k;);   // allocate, but don't grow the tstack.  Set usecount of cell to 1.  Put allocated addr into *jt->tnextpushp++
   jt->tnextpushp=pushxsave;   // restore tstack pointer
  }else{AFLAG(z) = newflags; DO(n, GA(y,t,m,r,f+ws); AFLAG(y)=newflags; MC(CAV(y),wv,k); wv+=k; zv[i]=y;); } // indirect w; don't set recursible, which might be expensive
 }
 RETF(z);
}    /* <"r w */

F1(jtboxopen){RZ(w); if((-AN(w)&-(AT(w)&BOX+SBOX))>=0){w = box(w);} R w;}

F2(jtlink){
RZ(a&&w);
#if FORCEVIRTUALINPUTS
 // to allow mapped-boxed tests to run, we detect when the virtual block being realized is at offset 0 from its
 // base block, and has the same atomsct/rank/shape.  Then we just return the base block, since the virtual block
 // is a complete alias for it.  This gets the flags right for mapped boxes, and doesn't require recopying a lot of memory for others
 // We also need this for DLL code that inplaces.
 // It's OK to revert to the original block, since we are mainly trying to see whether a virtual block is incorporated
 // We do this only for NJA base blocks, because otherwise would defeat the purpose of virtualizing everything
 if(AFLAG(w)&AFVIRTUAL && CAV(w)==CAV(ABACK(w)) && AN(w)==AN(ABACK(w)) && AR(w)==AR(ABACK(w))){
  I i; for(i=AR(w)-1; i>=0&&AS(w)[i]==AS(ABACK(w))[i];--i); if(i<0)w = ABACK(w);
 }
 if(AFLAG(a)&AFVIRTUAL && CAV(a)==CAV(ABACK(a)) && AN(a)==AN(ABACK(a)) && AR(a)==AR(ABACK(a))){
  I i; for(i=AR(a)-1; i>=0&&AS(a)[i]==AS(ABACK(a))[i];--i); if(i<0)a = ABACK(a);
 }
#endif
 if((-AN(w)&SGNIF(AT(w),BOXX))>=0){w = box(w);} R over(box(a),w);  // box empty or unboxed w, join to boxed a
}

// Calculate the value to use for r arg of copyresultcell: bit 0=ra() flag, next 15=rank requiring fill, higher=-(#leading axes of 1)
// zs, zr = address/length of shape of result cell   s,r = address/length of shape of cell to copy
static I rescellrarg(I *zs, I zr, I *s, I r){
 zs+=zr, s+=r;  // advance to end+1 of shapes
 zr-=r; zr*=(RMAX+1);  // amount of surplus rank, moved to high bits
 for(;r;--r)if(*--zs!=*--s)break;  // leave r with #axes up to & including the last requiring fill
 R (r-zr)<<1;  // join fields & return
}

// copy w into memory area z, which is known to be big enough to hold it (like povtake, but recursive implementation that doesn't require pre-fill)
// the types of w and z are the same
// sizes is +/\. (shape of z),bp(t(z)), i. e. the number of bytes in a result cell of each possible rank
// r (bits 16 up) is the negative of number of leading axes of rank 1 to be appended to w
// r (bits 1-15) is the number of axes of w requiring fill (lower axes are taken in full)
// r (bit 0) is set if what is being moved is boxed pointers that need ra()
// s is the shape of w, *s being the length of the first axis after the leading appended 1s
// jt->fillv is set up, expanded to 16 bytes
// result is the new position in w after the move
static C *copyresultcell(J jt, C *z, C *w, I *sizes, I rf, I *s){I wadv;I r=rf>>1;
 // if the entire w matches z, just copy it without fill
 if(r==0){
  // r=0   This can only happen if lower r was 0 originally, since we stop recursion at r=1.  r=0 means that
  // the entire r matched the suffix of the shape of zcell, and we can copy the entire cell
  wadv=sizes[0];
  if(rf&1){DO(wadv>>LGSZI, A a=((A*)w)[i]; ra(a); ((A*)z)[i]=a;)}else{MC(z,w,wadv);}
  R wadv+w;
 }
 // otherwise there will be fill
 C *endoffill=z+sizes[0];  // save address of end of area
 if(r==1){
  // r=1 (after r1 exhausted).  Lower cells are taken in full, so we can copy the cells en bloc
  wadv = s[0]*sizes[1]; // number of bytes to move
  if(rf&1){DO(wadv>>LGSZI, A a=((A*)w)[i]; ra(a); ((A*)z)[i]=a;)}else{MC(z,w,wadv);}
  w+=wadv; z+=wadv; // move the valid data, and advance pointers
 }else if(r<1){
  // There is a leading 1 axis.  Recur once to copy the one cell
  w=copyresultcell(jt,z,w,sizes+1,rf+((RMAX+1)<<1),s); z+=sizes[1];  // z advances 1 cell
 }else{I i;
  // leading axis of w is given.  Loop to copy each cell with fill
  for(i=s[0];i;--i){
   w = copyresultcell(jt,z,w,sizes+1,rf-2,s+1);  // advance w to next input
   z += sizes[1];   // advance z to next output
  }
 }
 // copy the fill, from z (new output pointer) to endoffill (end+1 of output cell)
 mvc(endoffill-z,z,sizeof(jt->fillv0),jt->fillv0);
 R w;
}

A jtassembleresults(J jt, I ZZFLAGWORD, A zz, A zzbox, A* zzboxp, I zzcellp, I zzcelllen, I zzresultpri, A zzcellshape, I zzncells, I zzwf, I startatend) {A zztemp;  // if we never allocated the boxed area (including force-boxed cases which never do) we just keep zz as the final result
 // startatend is 0 for forward, ~0 for reverse
 // Create a homogeneous array of results, by processing zzbox and zz one cell at a time from the end.
 //  Allocate the result area. (1) if USEOPEN and zz is empty, use zzbox; (2) if the largest cell-result is not bigger than the cells
 //  in zz, use zz.  Otherwise allocate a new area.

 I zzt=AT(zz);  // type of zz
 I natomszzcell; PROD(natomszzcell,AR(zz)-zzwf,AS(zz)+zzwf);  // number of atoms in cell of zz
 A* box0=AAV(zzbox)+(startatend&(AN(zzbox)-1));  // address of last valid box pointer, depending on direction of movement
 C* zzcell=CAV(zz)+zzcellp;  // address of last+1 cell moved to zz

 if(!(ZZFLAGWORD&ZZFLAGUSEOPEN)){
  // No sparse results.  We will bypass jtope and move the results into the result area here, with conversion and fill

  // Create the fill-cell we will need.  Note: all recursible fills must have the PERMANENT flag set, since we may not increment the usecount
  I zpri=jt->typepriority[CTTZ(zzt)]; zpri+=AN(zz)?256:0;   // priority of unboxed results, giving high pri to nonempty
  zzresultpri=(zpri>zzresultpri)?zpri:zzresultpri; I zft=((I)1)<<(jt->prioritytype[zzresultpri&255]);  // zft=highest precision encountered
  fillv(zft,1L,jt->fillv0); I zfs=bpnoun(zft); mvc(sizeof(jt->fillv0),jt->fillv0,zfs,jt->fillv0);  // create 16 bytes of fill.  zfs is byte=length of 1 atom of result type

  I *zzcs=AS(zzcellshape);  // zzcs->shape of padded result cell (may be a faux A block) AS[] is shape, AR is rank, AN is allocation
  I zzcr=AR(zzcellshape);  // zzcr=rank of result cell
  zzcs[zzcr]=zfs;  // length of 0-cell is byte-length of atom - store after the shape - we know there's room

  // if the result has different type from the values in zz, convert zz en bloc to type zft
  if(TYPESNE(zft,zzt)){I zzatomshift=CTTZ(bp(zzt)); I zexpshift = CTTZ(bp(zft))-zzatomshift;  // shift for size of atom; expansion factor of the conversion, as shift amount
   // here the old values in zz must change.  Convert them.  Use the special flag to cvt that converts only as many atoms as given
   I zatomct=(zzcellp>>zzatomshift)-(startatend&(AN(zz)-(zzcelllen>>zzatomshift)));   // get # atoms that have been filled in
   ASSERT(ccvt(zft|NOUNCVTVALIDCT,zz,(A*)&zatomct),EVDOMAIN); zz=(A)zatomct;  // flag means convert zcellct atoms.  Not worth checking for empty
   // change the strides to match the new cellsize
   if(zexpshift>=0){zzcelllen<<=zexpshift; zzcellp<<=zexpshift;}else{zzcelllen>>=-zexpshift; zzcellp>>=-zexpshift;}
   zzcell=CAV(zz)+zzcellp;  //  recalc address of last+1 cell moved to zz
  }
  // Now zz has the type zft

  I natomsresultcell; RE(natomsresultcell=prod(zzcr,zzcs));  // # atoms in actual result-cell.  Could overflow, if there is a mix of tall & wide
  I natomsresult; RE(natomsresult=mult(natomsresultcell,zzncells));  // number of atoms in result
  // Since we know the result-cell size in zzcellshape must be able to contain a cell of zz, we can test for equal rank and equal number of atoms.
  // But if the cell is empty, we can't rely on # atoms to verify the shape, and then we have to reallocate
  if((((AR(zz)-zzwf)^zzcr) | (natomsresultcell^natomszzcell) | !natomsresultcell)){
   // The overall result-cell differs in shape or type from the cells of zz.  We must allocate a new result area.
   GA(zztemp,zft,natomsresult,zzwf+zzcr,0);  // allocate result area, and point to shape
   MCISH(AS(zztemp),AS(zz),zzwf); MCISH(AS(zztemp)+zzwf,zzcs,zzcr);   // move in frame and shape

   // since zztemp is becoming the new result area, it should become inplace recursive if the type is recursible, and zz needs to
   // become nonrecursive, to transfer ownership of the contents of zz's blocks to zztemp without requiring explicit usecounting.
   // This might not work for XNUM/RAT types, because conversions may be required on results coming from zzbox, and those conversions
   // might fail: that would lose the blocks in zz that have not been copied to zztemp.  The problem does not exist for BOX type,
   // because we have already verified that there is not a mix of box/nonbox, so we can guarantee no failures until we get the
   // result area built.
   I zzrecur = AFLAG(zz)&BOX; AFLAG(zztemp) |= zzrecur; AFLAG(zz)^=zzrecur;  // transfer recursibility from zz to zztemp, but only for boxed result
  }else{
   // zz has the same item-shape as the final result (the items in zzbox must be smaller).  We can just keep zz as the final result area, and move the
   // items from zzbox into it.  NOTE that this may leave zztemp as a recursive XNUM/RAT, which it can't be if it was allocated above
   zztemp=zz;
  } // now zztemp has the result area, which might be the same as zz


  C *tempp=CAV(zztemp)+((~startatend)&(natomsresult*zfs));   // point to end+1 of result area, or beginning if going reverse

  // Calculate the vector of cell-lengths
  DQ(zzcr, zzcs[i]=zfs*=zzcs[i];);  // convert each atom of result-cell shape to the length in bytes of the corresponding cell
  // Now zfs is the length in bytes of each result-cell

  // Calculate r to use if we move from zz
  I *zzcopyress=AS(zz)+zzwf;  // &shape of a cell of zz
  I zzcopyresr=rescellrarg(zzcs,zzcr,zzcopyress,AR(zz)-zzwf);  // does not ever need ra

  // Adjust for loop direction
  tempp -= (~startatend)&zfs;  // tempp points to the current element.  If normal direction, we must back up to last-+1-1
  startatend = 2*startatend+1;  // from here on startatend is 1 for normal, -1 for reverse.  Use it to DEcrement zzboxp
  zfs *= startatend;  // include direction-of-movement in the size increment
  zzcelllen *= startatend;  // also in cell size, which from now on is now used only as the increment between cells of zz

  // For each result cell, copy (with conversion if necessary) from whichever source is valid
  // zzboxp points after the last box created; zzcell points after the last cell of zz; tempp points after last cell of result
  // box0 points to the first valid box location - everything before that comes from zz
  I zztemprecur = !!(AFLAG(zztemp)&RECURSIBLE);  // remember if we are moving into a recursive block
  while(--zzncells>=0){A zzboxcell;
   // We have to make sure we don't access zzboxp out of bounds, but it can go in either direction, which makes checking for out-of-bounds
   // a pain.  So we check before incrementing to see if zzboxp is exactly at the end: if so, we fail the test and leave it there permanently
   if(zzboxp!=box0 && (zzboxp-=startatend, zzboxcell= *zzboxp)){
    // cell comes from zzboxp.  Convert if necessary, then move.  Before moving, calculate the rank to use for the fill.
    // Don't convert empties, to make sure we don't have a failure while we are processing boxed results
    if(AN(zzboxcell)&&TYPESNE(zft,AT(zzboxcell))){
     if(!(zzboxcell=cvt(zft,zzboxcell))){
      // error during conversion.  THIS IS THE ONLY PLACE WHERE ERROR IS POSSIBLE DURING THE COPY.
      // If zz and zztemp are the same block, and that block is recursive, it may be in an invalid state: values have been copied
      // from zzcell up the the end of the block and are now duplicated.  To fix it, we have to zero out anything was was copied but
      // not overwritten: for the forward case, everything in the range from the
      // current zzcell up to the last tempp that was filled (note that *tempp has not been filled yet).  In other words, we have
      // to fill zzncells+1 cells: the ones we haven't started, plus the one we failed on
      if(zztemp==zz&&zztemprecur){
       memset((startatend>0)?zzcell:tempp,C0,(startatend>0)?tempp+zfs-zzcell:zzcell-tempp);  // clear, depending on direction.  If normal, we are going
         // from zzcell, which has been copied from, through the end of tempp, which has not been copied to yet.  If reverse, we go from
         // tempp, which has not been copied to, to zzcell, which has been copied.
      }
      R 0;
     }
    }
    I *zzbcs=AS(zzboxcell);  // convert if needed, point to shape
    // if zztemp is recursible, we must ra() the new block during the copy, since it comes from nonrecursive zzbox
    copyresultcell(jt, tempp, CAV(zzboxcell), zzcs, rescellrarg(zzcs,zzcr,zzbcs,AR(zzboxcell))+zztemprecur,zzbcs);  // combine rank and recursible flags
   }else{
    // cell comes from zz.  It needs no conversion, since we convert zz whenever we see a precision change.
    // If type is BOX, we know that zz was originally recursive and that its recursive status has been copied/identitied into zztemp; either way no ra is required
    // For other recursible types, the only way for zztemp to be recursive is for it to be identical with zz, so no ra is called for then either.
    // C *copyresultcell(J jt, C *z, C *w, I *sizes, I r, I *s){I wadv;
    zzcell-=zzcelllen;   // back up to next input cell.  This pointer moves only for results that come from zz
    if(tempp==zzcell)break;  // if we start to copy in-place, we must be before the first wreck, and we can leave remaining cells in place
    copyresultcell(jt,tempp,zzcell,zzcs,zzcopyresr,zzcopyress);
   }
   tempp-=zfs;  // back result pointers to next input/output positions.  This pointer moves for every result-cell.  zzboxp also moves for every result, till it pegs out
  }
  zz=zztemp;  // switch main result pointer to our result value
 }else{
  // some results were sparse.  We will have to use the general jtope code to sort them out.  Ecch
  if(AS(zz)[0]<0){zztemp=zzbox;  // if ALL results are in the boxed area, use the boxed area
  }else{
   // there are sparse results, but they came after we started boxing.  We have to allocate a full-sized boxed area and move the results to it,
   // boxing anything that comes from zz.  We use faux-virtual blocks for boxing cells of zz.  What a lot of code that so seldom gets used!
   GATV(zztemp,BOX,zzncells,zzwf,AS(zz));  // one box per result cell.  GA clears area to 0, which is unnecessary
   A* tempp=AAV(zztemp)+((~startatend)&(zzncells-1));   // point to beginning of result area
   // Adjust for loop direction
   startatend = 2*startatend+1;  // from here on startatend is 1 for normal, -1 for reverse.  Use it to DEcrement zzboxp

   while(--zzncells>=0){   // for each output position, from the end
    if(zzboxp!=box0 && (zzboxp-=startatend, *zzboxp))*tempp=*zzboxp;
    else{A zzz;  // we have to box the value from zz
     zzcell-=zzcelllen;  // back up to next cell in zz
     GA(zzz,zzt,0,zzwf,AS(zz)+zzwf); AN(zzz)=natomszzcell; AK(zzz)=zzcell-(C*)zzz;  // allocate empty header; fill in length; point to data in zz
       // All these blocks are single-use so we keep them nonrecursible and don't bother to mark them virtual or incur the overhead thereof.  Could use gah
     *tempp=zzz;
    }
    tempp-=startatend;   // back/forward to next output location
   }
  }
  zz=ope(zztemp);  // do the full open on the result 
 }
 R zz;
}

static B povtake(J jt,A a,A w,C*x){B b;C*v;I d,i,j,k,m,n,p,q,r,*s,*ss,*u,*uu,y;
 if(!w)R 0;
 r=AR(w); n=AN(w); k=bpnoun(AT(w)); v=CAV(w);
 if(1>=r){MC(x,v,k*n); R 1;}
 m=AN(a); u=AV(a); s=AS(w);
 p=0; d=1; DO(r, if(u[m-1-i]==s[r-1-i]){d*=s[r-1-i]; ++p;}else break;);
 b=0; DO(r-p, if(b=1<s[i])break;);
 if(!b){MC(x,v,k*n); R 1;}
 k*=d; n/=d; ss=s+r-p; uu=u+m-p;
 for(i=0;i<n;++i){
  y=0; d=1; q=i; /* y=.a#.((-$a){.(($a)$1),$w)#:i */
  s=ss; u=uu; DQ(r-p, j=*--s; y+=q%j*d; d*=*--u; q/=j;);
  MC(x+y*k,v,k); v+=k;
 }
 R 1;
}

static B jtopes1(J jt,B**zb,A*za,A*ze,I*zm,A cs,A w){A a,e=0,q,*wv,x;B*b;I i,k,m=0,n,*v,wcr;P*p;
 n=AN(w); wcr=AN(cs); wv=AAV(w);
 GATV0(x,B01,wcr,1); b=BAV(x); memset(b,C0,wcr);
 for(i=0;i<n;++i)
  if(q=wv[i],SPARSE&AT(q)){
   p=PAV(q); x=SPA(p,x); m+=*AS(x);
   if(!e)e=SPA(p,e); else ASSERT(equ(e,SPA(p,e)),EVSPARSE);
   k=wcr-AR(q); DO(k, b[i]=1;); a=SPA(p,a); v=AV(a); DQ(AN(a), b[k+*v++]=1;);
  }
 RZ(*za=caro(ifb(wcr,b)));    /* union of sparse axes           */ // avoid readonly
 *zb=b;                 /* mask corresp. to sparse axes   */
 *ze=e?e:num[0];          /* sparse element                 */
 *zm=m;                 /* estimate # of non-sparse cells */
 R 1;
}

static B jtopes2(J jt,A*zx,A*zy,B*b,A a,A e,A q,I wcr){A x;B*c;I dt,k,r,*s,t;P*p;
 dt=AT(e); r=AR(q); k=wcr-r; t=AT(q);
 if(t&SPARSE){
  p=PAV(q);
  RZ(c=bfi(r,SPA(p,a),1));
  DO(r, if(b[k+i]!=c[i]){RZ(q=reaxis(ifb(r,k+b),q)); break;});
 }else{
  if(k){
   GA(x,t,AN(q),wcr,0); s=AS(x); DQ(k, *s++=1;); MCISH(s,AS(q),r); 
   MC(AV(x),AV(q),AN(q)<<bplg(t)); q=x;
  }
  RZ(q=sparseit(t&dt?q:cvt(dt,q),a,e));
 }
 p=PAV(q);
 x=SPA(p,x); if(!(dt&AT(x)))RZ(x=cvt(dt,x));
 *zx=x;         /* data cells              */
 *zy=SPA(p,i);  /* corresp. index matrix   */
 R 1;
}

static A jtopes(J jt,I zt,A cs,A w){A a,d,e,sh,t,*wv,x,x1,y,y1,z;B*b;C*xv;I an,*av,c,dk,dt,*dv,i,j,k,m,m1,n,
     p,*s,*v,wcr,wr,xc,xk,yc,*yv,*zs;P*zp;
 n=AN(w); wr=AR(w); wv=AAV(w); wcr=AN(cs); dt=DTYPE(zt); dk=bpnoun(dt);
 RZ(opes1(&b,&a,&e,&m,cs,w)); an=AN(a); av=AV(a);
 GASPARSE(z,zt,1,wr+wcr,(I*)0); zs=AS(z); MCISH(zs,AS(w),wr); MCISH(zs+wr,AV(cs),wcr);
 zp=PAV(z); c=wcr-an; yc=wr+an;
 SPB(zp,e,cvt(dt,e)); e = SPA(zp,e);  // in case of reassignment by SPB
 GATV0(t,INT,yc, 1L); v=AV(t); DO(wr, v[i]=i;); DO(an, v[wr+i]=wr+av[i];); SPB(zp,a,t);
 GATV0(sh,INT,1+c,1L); s=AV(sh); s[0]=m; j=1; DO(wcr, if(!b[i])s[j++]=zs[wr+i];); 
 RE(xc=prod(c,1+s)); xk=xc*dk;
 GATV0(d,INT,wr,1); dv=AV(d); memset(dv,C0,wr*SZI);
 RE(i=mult(m,xc)); GA(x,dt, i,1+c,s); xv=CAV(x); mvc(m*xk,xv,dk,AV(e));
 RE(i=mult(m,yc)); GATV0(y,INT,i,2L); v=AS(y); *v=m; v[1]=yc; yv=AV(y); memset(yv,C0,SZI*i);
 for(i=p=0;i<n;++i){
  RZ(opes2(&x1,&y1,b,a,e,wv[i],wcr)); v=AS(y1); m1=v[0]; k=v[1];
  if(m<p+m1){
   j=m; m=(i<n-1?m+m:0)+p+m1;
   RZ(x=take(sc(m),x)); xv=CAV(x)+p*xk; mvc(xk*(m-j),xv,dk,AV(e));
   RZ(y=take(sc(m),y)); yv= AV(y)+p*yc;
  }
  for(j=wr-1;j;--j)if(dv[j]==zs[j]){dv[j]=0; ++dv[j-1];}else break;
  v=AV(y1); DQ(m1, ICPY(yv,dv,wr); ICPY(yv+yc-k,v,k); yv+=yc; v+=k;); 
  if(memcmp(1+AS(x1),1+s,SZI*c)){*s=m1; povtake(jt,sh,x1,xv);} else MC(xv,AV(x1),m1*xk);
  ++dv[wr-1]; xv+=m1*xk; p+=m1;
 }
 SPB(zp,x,p==m?x:take(sc(p),x));
 SPB(zp,i,p==m?y:take(sc(p),y));
 R z;
}  // > w when w contains sparse contents, maxtype=zt

// > y (rank is immaterial)
// If y cannot be inplaced, we have to make sure we don't return an inplaceable reference to a part of y.  This would happen
// if y contained inplaceable components (possible if y came from < yy or <"r yy).  In that case, mark the result as non-inplaceable.
// We don't support inplacing here yet so just do that always
F1(jtope){PROLOG(0080);A cs,*v,y,z;I nonh;C*x;I i,n,*p,q=RMAX,r=0,*s,t=0,te=0,*u,zn;
 RZ(w);
 n=AN(w); v=AAV(w);
 if(!(BOX&(AT(w)&REPSGN(-n))))RCA(w);  // return w if empty or open
 if(!AR(w)){z=*v; ACIPNO(z); R z;}   // scalar box: turn off inplacing if we are using the contents directly
 // set q=min rank of contents, r=max rank of contents
 for(i=0;i<n;++i){
  y=v[i]; r=MAX(r,AR(y)); q=MIN(q,AR(y));
  if(AN(y))t|=AT(y); else te|=AT(y);  // accumulate types, either nonempty or empty
 }
 // if there was a nonempty, verify that the nonempties are compatible and find the highest-priority one
 // Fill creates a subtlety: we don't know whether empty boxes are going to contribute to
 // the result or not.  In a case like (0 2$a:),'' the '' will contribute, but the (0 2$a:) will
 // not.  And, we don't want to require compatibility with the fill-cell if nothing is filled.
 // So, we don't check compatibility for empty boxes.
 // The homogeneity flag h is set if max rank is 1 and there is 0 or 1 nonempty type.  In that case fill is 1-dimensional and we just copy into the result area
 nonh = (r&~1) | (t&(t-1));  // non homogeneous if rank is not 0 or 1, or if there is more than 1 bit set in t
 if(t){
  ASSERT(HOMONE(t,0)||t==BOX||t==SBT,EVDOMAIN);  // no mixed nonempties
  ASSERT(!(t&SPARSE&&t&XNUM+RAT),EVDOMAIN);  // don't allow a sparse that requires promotion to indirect
  te=t;  // te holds the type to use
 }
 t=te&-te; while(te&=(te-1)){RE(t=maxtypene(t,te&-te));}  // get highest-priority type (which may be sparse)
 // allocate place to build shape of result-cell; initialize to 1s above q, zeros below (this is adding leading 1s to missing leading axes)
 fauxblockINT(csfaux,4,1); fauxINT(cs,csfaux,r,1) u=AV(cs); DO(r-q, u[i]=1;); p=u+r-q; DO(q, p[i]=0;);
 // find the shape of a result-cell
 DO(n, y=v[i]; s=AS(y); p=u+r-AR(y); DO(AR(y),p[i]=MAX(p[i],s[i]);););
 if(t&SPARSE)RZ(z=opes(t,cs,w))
 else{I klg; I m;
  RE(m=prod(r,u)); RE(zn=mult(n,m)); klg=bplg(t); q=m<<klg;
  // Allocate result area & copy in shape (= frame followed by result-cell shape)
  GA(z,t,zn,r+AR(w),AS(w)); MCISH(AS(z)+AR(w),u,r); x=CAV(z); fillv(t,zn,x);  // init to a:  fills
  for(i=0;i<n;++i){
   y=v[i];   // get pointer to contents, relocated if need be
   if(!nonh)                MC(x,AV(y),AN(y)<<klg);  // homogeneous atomic types: fill only at end, copy the valid part
   else if(TYPESEQ(t,AT(y))&&m==AN(y))MC(x,AV(y),q);   // cell of maximum size: copy it entire
   else if(AN(y))             RZ(povtake(jt,cs,TYPESEQ(t,AT(y))?y:cvt(t,y),x));  // otherwise add fill
   x+=q;
 }}
 EPILOG(z);
}

// ; y general case, where rank > 1 (therefore items are not atoms)
// w is the data to raze (boxed), t is type of nonempty boxes of w, n=#,w, r=max rank of contents of w, v->w data,
// zrel=1 if any of the contents uses relative addressing
static A jtrazeg(J jt,A w,I t,I n,I r,A*v,I nonempt){A h,h1,y,z;C*zu;I c=0,i,j,k,m,*s,*v1,yr,*ys;UI p;
 // Calculate the shape of a result-cell (it has rank r-1); and c, the number of result-cells
 fauxblockINT(hfaux,4,1); fauxINT(h,hfaux,r,1) s=AV(h); memset(s,C0,r*SZI);  // h will hold the shape of the result; s->shape data; clear to 0 for compares below
 for(i=0;i<n;++i){   // loop over all contents
  // y->A block for contents of w[i]; yr=rank of y; ys->shape of y
  // if contents has the same rank as result, it is an array of result-cells, and each item adds
  // to c, the total # items in result; otherwise it is a single cell that will be promoted in rank to
  // become one result-cell.  Error if overflow (should be impossible).  j=#leading length-1 axes that need to be added
  y=v[i]; yr=AR(y); ys=AS(y); c+=(0==(j=r-yr))?*ys:1; ASSERT(0<=c,EVLIMIT); 
  if(!yr)continue;   // do not perform rank extension of atoms
  // here we find the max cell size in *(s+1). *s is not used.  The maximum shape is taken
  // over extension axes of length 1, followed by the actual shape of the contents
  DO(j,  s[i]=MAX(1,    s[i]);     ); 
  DO(yr, s[j]=MAX(ys[i],s[j]); ++j;);
 }
 // Install the number of result items in s; m=total #result atoms
 *s=c; RE(m=prod(r,s)); PROD(p,r-1,s+1);
 // Now that we know the shape of the result-cell, we can decide, for each box, whether the
 // box contributes to the result, and whether it will be filled.  This matters only if a fill-cell has been specified.
 // If fill has been specified, we include its type in the result-type (a) only if some block gets filled
 // (this will cause all input-blocks to convert to the precision needed; any fill will be promoted to that type)
 // (b) all the blocks are empty (which can be detected because t has not been set yet)
 if(jt->fill) {  // if it was ;!.f
  if(nonempt&&m) {  // Check cell-contents only if there are some nonempty contents, and if the result-cell is nonempty
     // these are different, eg for 0 1$4 which has no cells but they are nonempty, or 1 0 1$4 which has an empty result-cell
   for(i=0;i<n;++i) {   // for each box of contents
    y=v[i]; yr=AR(y);   // y-> A block for contents of w[i]; yr = its rank
    if(!yr)continue; ys=AS(y);   // atoms are replicated, never filled; otherwise point to shape
    if(r==yr&&0==ys[0])continue;  // if y is unextended and has no cells, it will not contribute, no matter what the cell-shape
    // see if the shape of y-cell (after rank extension) matches the shape of result-cell.  If not, there will be fill
    for(yr=yr-1,k=r-1;yr>=0&&ys[yr]==s[k];--yr,--k);  // see if unextended cell-shape matches
    if(yr<0)while(k>0&&s[k]==1)--k;   // if all that match, check to see if extended cell-shape==1
    if(k>0) {   // If we compared all the way back to the entire rank or one short (since we only care about CELL shape), there will be no fill
     ASSERT(HOMO(t, AT(jt->fill)), EVDOMAIN); t = maxtyped(t, AT(jt->fill));  // Include fill in the result-type.  It better fit in with the others
     break;  // one fill is enough
    }    
   }
  }
 }

 // Now we know the type of the result.  Create the result.
 k=bpnoun(t); p*=k;  // k=#bytes in atom of result; p=#bytes/result cell
 fauxblockINT(h1faux,4,1); fauxINT(h1,h1faux,r,1) v1=AV(h1);  // create place to hold shape of cell after rank extension
 GA(z,t,m,r,s);    // create result area, shape s; zrel now is relocation offset for result
 zu=CAV(z);  // output pointers
 // loop through each contents and copy to the result area
 for(i=0;i<n;++i){
  y=v[i];  // y->address of A block for v[i]
  if(TYPESNE(t,AT(y)))RZ(y=cvt(t,y));   // convert to result type if needed
  yr=AR(y); ys=AS(y);    // yr=rank of y, ys->shape of y
  if(!yr){
   // atomic contents; perform atomic replication
   {mvc(p,zu,k,AV(y)); zu+=p;}   // copy the data, replicating
  } else {
   // nonatomic contents: rank extension+fill rather than replication
   // if IC(y)==0 this all does nothing, but perhaps not worth checking
   if(j=r-yr){DO(j,v1[i]=1;); MCISH(j+v1,ys,yr); RZ(y=reshape(h1,y)); }  // if rank extension needed, create rank 1 1...,yr and reshape to that shape
   for(j=1;j<r;++j)if(s[j]!=AS(y)[j])break; if(j!=r){SETIC(y,*s); RZ(y=take(h,y));}  // if cell of y has different shape from cell of result, install the
     // #items into s (giving #cell,result-cell shape) and fill to that shape.  This destroys *s (#result items) buts leaves the rest of s
   {j=k*AN(y); MC(zu,AV(y),j); zu+=j;}
  }
 }
 RETF(z);
}    /* raze general case */

// ; y
F1(jtraze){A*v,y,z,* RESTRICT zv;C* RESTRICT zu;I *wws,d,i,klg,m=0,n,r=1,t=0,te=0;
 RZ(w);
 n=AN(w); v=AAV(w);  // n=#,w  v->w data
 if(!n)R mtv;   // if empty operand, return boolean empty
 if(!(BOX&AT(w)))R ravel(w);   // if not boxed, just return ,w
 if(1==n){RZ(z=*v); R AR(z)?z:ravel(z);}  // if just 1 box, return its contents - except ravel if atomic
 // scan the boxes to create the following values:
 // m = total # items in contents; aim=#atoms per item;  r = maximum rank of contents
 // t = OR of types of nonempty blocks, te = OR of types of empties
 // if the block has been flagged as having homogeneous items, we can skip this step.  Such flagging indicates a compound like ;@:(<@f) where we knew during
 // result-assembly that we were going to raze the result, and we took the trouble to inspect the contents' shapes as they were going by.
 if(!(AFLAG(w)&AFUNIFORMITEMS)) {  // normal case
  for(i=0;i<n;++i){
   y=v[i]; r=MAX(r,AR(y));
   I yt=AT(y); te|=yt; m+=AN(y); yt=AN(y)?yt:0; t|=yt;
  }
  // if there was a nonempty, verify that the nonempties are compatible and find the highest-priority one
  // Fill creates a subtlety: we don't know whether empty boxes are going to contribute to
  // the result or not.  In a case like (0 2$a:),'' the '' will contribute, but the (0 2$a:) will
  // not.  And, we don't want to require compatibility with the fill-cell if nothing is filled.
  // So, we don't check compatibility for empty boxes.
  i=t;  // save indicator of nonempties
  if(t){
   ASSERT(HOMONE(t,0)||t==BOX||t==SBT,EVDOMAIN);  // no mixed nonempties
   te=t;  // te holds the type to use
  }else if(jt->fill){te=AT(jt->fill);}  // all empty: use fill type if given.
  t=te&-te; while(te&=(te-1)){t=maxtypedne(t,te&-te);}  // get highest-priority type
  // t is the type to use.  i is 0 if there were no nonempties
  // if there are only empties, t is the type to use
  // if the cell-rank was 2 or higher, there may be reshaping and fill needed - go to the general case
  if(1<r)R razeg(w,t,n,r,v,i);
  // fall through for boxes containing lists and atoms, where the result is a list.  No fill possible, but if all inputs are
  // empty the fill-cell will give the type of the result (similar to 0 {.!.f 0$...)

  GA(z,t,m,r,0);  // allocate the result area; mark relative if any contents relative
  // now zrel has been repurposed to relocation offset for z (0 if not relative)
  zu=CAV(z); zv=AAV(z); klg=bplg(t); // input pointers, depending on type; length of an item
  // loop through the boxes copying
  for(i=0;i<n;++i){
   y=v[i]; if(AN(y)){if(TYPESNE(t,AT(y)))RZ(y=cvt(t,y)); d=AN(y)<<klg; MC(zu,AV(y),d); zu+=d;}
  }
 }else{
  // special case where the result-assembly code checked to make sure the items were uniform.  In this case the number of items was hidden away in the AM field (otherwise unneeded, since we know the block isn't virtual)
  // and we just need to extract the rank and the type, and calculate the atoms-per-item.  We know there is at least one box.
  A ww0=AAV(w)[0]; t=AT(ww0); r=AR(ww0); r+=SGNTO0(r-1); wws=&AS(ww0)[0]; // ww0->1st contents; t=type; r=rank>.1; wws->item shape
  PROD(m,r-1,wws+1);  // get #atoms in an item of w
  I nitems=AM(w);  // total # result items is stored in w
  GA(z,t,m*nitems,r,wws); AS(z)[0]=nitems; // allocate the result area; finish shape
  // now zrel has been repurposed to relocation offset for z (0 if not relative)
  zu=CAV(z); zv=AAV(z); klg=bplg(t); // input pointers, depending on type; length of an item
  // loop through the boxes copying the data into sequential output positions
  for(i=0;i<n;++i){
   y=v[i]; if(AN(y)){d=AN(y)<<klg; MC(zu,AV(y),d); zu+=d;}
  }
 }

 RETF(z);
}

// TODO: remove divides from razeh

F1(jtrazeh){A*wv,y,z;C*xv,*yv,*zv;I c=0,ck,dk,i,k,n,p,r,*s,t;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 if(!AR(w))R ope(w);
 n=AN(w); wv=AAV(w);  y=wv[0]; SETIC(y,p); t=AT(y); k=bpnoun(t);
 DO(n, I l; y=wv[i]; r=AR(y); ASSERT(p==SETIC(y,l),EVLENGTH); ASSERT(r&&r<=2&&TYPESEQ(t,AT(y)),EVNONCE); c+=1==r?1:*(1+AS(y)););
 GA(z,t,p*c,2,0); s=AS(z); *s=p; *(1+s)=c; 
 zv=CAV(z); ck=c*k;
 for(i=0;i<n;++i){
  y=wv[i]; dk=1==AR(y)?k:k**(1+AS(y)); xv=zv; zv+=dk;
  if(!dk)continue;
  if(t&BOX)RZ(y=car(y));
  yv=CAV(y);
  switch(0==(I)xv%dk&&0==ck%dk?dk:0){
   case sizeof(I): {I*u,*v=(I*)yv; DQ(p, u=(I*)xv; *u=*v++;    xv+=ck;);} break;
#if SY_64
   case sizeof(int):{int*u,*v=(int*)yv; DQ(p, u=(int*)xv; *u=*v++; xv+=ck;);} break;
#endif
   case sizeof(S): {S*u,*v=(S*)yv; DQ(p, u=(S*)xv; *u=*v++;    xv+=ck;);} break;
   case sizeof(C):                 DQ(p, *xv=*yv++;            xv+=ck;);  break;
   default:                        DQ(p, MC(xv,yv,dk); yv+=dk; xv+=ck;); 
 }}
 RETF(z);
}    /* >,.&.>/,w */

