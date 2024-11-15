/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Box & Open                                                       */
#include "j.h"
#define ZZDEFN
#include "result.h"

#ifdef BOXEDSPARSE
extern UC fboxedsparse;
#endif

I level(J jt,A w){A*wv;I d,j;
 ARGCHK1(w);
 if((-AN(w)&-(AT(w)&BOX))>=0)R 0;
 d=0; wv=AAV(w);
 DQ(AN(w), j=level(jt, C(wv[i])); d=d<j?j:d;);
 R 1+d;
}

// return 0 if the level of w is greater than l, 1 if <=
// terminates early if possible
I levelle(J jt,A w,I l){
 ARGCHK1(w);
 if((-AN(w)&-(AT(w)&BOX))>=0)R SGNTO0(~l);  // if arg is unboxed, its level is 0, so return 1 if l>=0
 if(l<=0)R 0;  // (arg is boxed) if l is <=0, arglevel is  > l
 --l; A *wv=AAV(w);
 DO(AN(w), if(!levelle(jt,C(wv[i]),l))R 0;);  // stop as soon as we see level big enough
 R 1;  // if it never gets big enough, say so, keep looking
}

F1(jtlevel1){ARGCHK1(w); I z=level(jt,w); RE(0) R sc(z);}

F1(jtbox){A y,z,*zv;C*wv;I f,k,m,n,r,wr,*ws; 
 F1PREFIP;ARGCHK1(w);I wt=AT(w); FLAGT waf=AFLAG(w);
#ifndef BOXEDSPARSE
 ASSERTF(!ISSPARSE(wt),EVNONCE,"can't box sparse arrays");
#else
 ASSERTF(fboxedsparse||!ISSPARSE(wt),EVNONCE,"can't box sparse arrays");
#endif
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r;   // no RESETRANK because we call no primitives
 if(likely(!f)){
  // single box: fast path.  Allocate a scalar box and point it to w.
  GAT0(z,BOX,1,0);
  I aband=SGNTO0(AC(w))&((I)jtinplace>>JTINPLACEWX);  // bit 0 = 1 if w is abandoned
  if(!((I)jtinplace&JTWILLBEOPENED)){
   // Normal case.  Mark w as incorporated.  Make all results recursive
   // If the input is DIRECT and abandoned inplaceable, mark the result as PRISTINE
   // If the input is abandoned and direct or recursive, zap it rather than raising the usecount
   AFLAGINIT(z,BOX+((-(wt&DIRECT))&((aband)<<AFPRISTINEX))) INCORPNC(w);  // this realizes w if virtual
   raczap(w,aband!=0)  // INCORPNC+this=INCORPRA, but using zap when abandoned
  }else{
   // WILLBEOPENED: the result itself will be discarded and only the contents will be used.
   // Keep the result nonrecursive and don't realize any virtuals, knowing that they will be be realized if necessary before they are incorporated later.  They will be freed in the caller
   // If the input is DIRECT and abandoned inplaceable non-virtual, mark the result as PRISTINE
   AFLAGINIT(z,(-(wt&DIRECT))&((aband)<<AFPRISTINEX)&~(waf<<(AFPRISTINEX-AFVIRTUALX)))
   INCORPNCUI(w); ACIPNO(w);  // realize unincorpable (but not virtual); w must be protected while it is in the box from argument deletion
  }
  AAV0(z)[0]=w;  // install the address of the (possibly realized) input
 } else {
  // <"r
#ifdef BOXEDSPARSE
 ASSERTF(fboxedsparse||!ISSPARSE(wt),EVNONCE,"can't box sparse arrays");    // <"r not implemented
#endif
  ws=AS(w);
  CPROD(AN(w),n,f,ws); CPROD(AN(w),m,r,f+ws);
  k=m<<bplg(wt); wv=CAV(w);
  // Since we are allocating the new boxes, the result will ipso facto be PRISTINE, as long as w is DIRECT and the result does not contain virtuals.  If w is not DIRECT, we can be PRISTINE if we ensure that
  // w is PRISTINE inplaceable, but we don't bother to do that because PRISTINE is used only for DIRECT contents.
  // If the input is DIRECT, mark the result as PRISTINE
  GATV(z,BOX,n,f,ws); AFLAGINIT(z,BOX+((((I)jtinplace&JTWILLBEOPENED)-1)&(-(wt&DIRECT))&AFPRISTINE)) if(unlikely(n==0)){RETF(z);}  // Recursive result; could avoid filling with 0 if we modified AN after error, or cleared after *tnextpushp
  // We have allocated the result; now we allocate a block for each cell of w and copy the w values to the new block.  If WILLOPEN is given, we synthesize a virtual block and leave
  // that in the result - it will be freed when the result is, since result is recursive

  // Since we are making the result recursive, we can save a lot of overhead by NOT putting the cells onto the tstack.  As we have marked the result as
  // recursive, it will free up the cells when it is deleted.  We want to end up with the usecount in the cells being 1, not inplaceable.  The result itself will be
  // inplaceable with a free on the tstack.
  // To avoid the tstack overhead, we switch the tpush pointer to our data area, so that blocks are filled in as they are allocated, with nothing put
  // onto the real tpop stack.  If we hit an error, that's OK, because whatever we did get allocated will be freed when the result block is freed.  We use GAE so that we don't abort on error
  A *pushxsave = jt->tnextpushp; jt->tnextpushp=AAVn(f,z);  // save tstack info before allocation
  JMCDECL(endmask) JMCSETMASK(endmask,k,0)   // set mask for JMCR - OK to copy SZIs
  A wback=ABACK(w); wback=AFLAG(w)&AFVIRTUAL?wback:w;   // w is the backer for new blocks unless it is itself virtual
  while(n--){
   if(!((I)jtinplace&JTWILLBEOPENED)){
    GAE(y,wt,m,r,f+ws,break); JMCR(CAVn(r,y),wv,k,0,endmask); INCORPRAZAPPED(y,wt);   // allocate, but don't grow the tstack.  Set usecount of cell to 1.  ra0() if recursible.  Put allocated addr into *jt->tnextpushp++
   }else{
    // WILLBEOPENED case.  We must make the block virtual.  We avoid the call overhead
    if((y=gafv(SZI*(NORMAH+wr)-1))==0)break;  // allocate the block, abort loop if error
    AT(y)=wt;
    ACINIT(y,ACUC1)   // transfer inplaceability from original block
    ARINIT(y,(RANKT)r); AN(y)=m;
    AK(y)=(wv-(C*)y);
    AFLAGINIT(y,AFVIRTUAL | (wt&RECURSIBLE))  // flags: recursive, not UNINCORPABLE, not NJA.
    MCISH(AS(y),f+ws,wr) // install shape
    ABACK(y)=wback;  // install pointer to backer
   }
   wv+=k; 
  }
  // raise the backer for all the virtual blocks taken from it.  The first one requires ra() to force the backer recursive; after that we can just add to the usecount.  And make w noninplaceable, since it now has an alias at large
  if(unlikely((I)jtinplace&JTWILLBEOPENED)){I nboxes=jt->tnextpushp-AAV(z); if(likely(nboxes!=0)){ACIPNO(w); ra(wback); ACADD(wback,nboxes-1);}}  // get # boxes allocated without error
  jt->tnextpushp=pushxsave;   // restore tstack pointer
  // OK to fail now - memory is restored
  //   Since we are copying contents of w, it must lose PRISTINE status if it is boxed
  PRISTCLRF(w);  // destroys w
  ASSERT(y!=0,EVWSFULL);  // if we broke out on allocation failure, fail.  Since the block is recursive, when it is tpop()d it will recur to delete contents
 }
 RETF(z);
}    /* <"r w */

F1(jtboxopen){F1PREFIP; ARGCHK1(w); if((-AN(w)&-(AT(w)&BOX))>=0){w = jtbox(jtinplace,w);} R w;}

// x ; y, with options for x (,<) y   x (;<) y   x ,&< y
// This verb propagates WILLOPEN, so it must not raise usecounts or EPILOG or call a verb that does EPILOG if WILLBEOPENED is set on input.
// As a result of this we support both recursive and nonrecursive y inputs.  If y is unboxed, we create a recursive block if WILLOPEN is
// not set, or a nonrecursive block if WILLOPEN is set
DF2(jtjlink){
F2PREFIP;ARGCHK2(a,w);
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
#ifndef BOXEDSPARSE
 ASSERTF(!ISSPARSE(AT(a)|AT(w)),EVNONCE,"can't box sparse arrays");
#else
 ASSERTF(fboxedsparse||!ISSPARSE(AT(a)|AT(w)),EVNONCE,"can't box sparse arrays");
#endif
 I optype=FAV(self)->localuse.lu1.linkvb;  // flag: sign set if (,<) or ,&< or (;<) which will always box w; bit 0 set if (,<)
 optype|=((I)jtinplace&JTWILLBEOPENED)<<(BOXX-JTWILLBEOPENEDX);  // fold in BOX flag that tells us to allow virtual boxed results
 if(likely(!(optype&BOX))){realizeifvirtual(w);}  // it's going into an array, so realize it unless virtual results allowed
 // if (,<) and a is not boxed singleton atom/list, revert
 if(unlikely((optype&1)>((AT(a)>>BOXX)&SGNTO0((AR(a)-2)&((AN(a)^1)-1))))){R jthook2cell(jtinplace,a,w,self);}  // (,<) and ((not boxed) or (rank>1) or (n!=1)) - revert to normal processing - WILLOPEN is impossible
 I unboxempty=SGNIFNOT(AT(w),BOXX)|(AN(w)-1)|optype;  // sign set if w unboxed or empty, or the operation is (,<) or ,&< or (;<) which will always box w
 I aband=(a!=w)&SGNTO0(AC(w))&((I)jtinplace>>JTINPLACEWX);  // 1 if w is abandoned, 0 otherwise.  Must not accept a==w as it could lead to w containing itself
 if((unboxempty|((AN(w)|AR(w))-2))<0){A neww;   // unboxed/empty or force-boxed w, or AN(w)<2 and AR(w)<2
  // Here to start building the right-to-left result area.  We can do this is w is a singleton box or w must be boxed here
  // if w is unboxed/empty or is a singleton rank<2, allocate a  vector of 8 boxes, point AK to the next-last, and put w there as the new w.  Vector is recursive unless WILLBEOPENED
  GAT0(neww,BOX,8,1);  // allocate 8 boxes
  AN(neww)=AS(neww)[0]=1; AFLAGORLOCAL(neww,~optype&BOX); AK(neww)+=6*SZI;   // Make neww a singleton list, recursive unless WILLBEOPENED, with AK pointing to the next-last atom
  // If w was abandoned, zap it, else ra
  if(unboxempty<0){
   // w was unboxed or empty.  Put it directly into neww, then ra or zap it if recursive.  If DIRECT abandoned, make result PRISTINE
   AFLAGORLOCAL(neww,(-(AT(w)&DIRECT))&((aband)<<AFPRISTINEX))  // starts PRISTINE if abandoned DIRECT
   if(likely(!(optype&BOX))){INCORPNC(w); raczap(w,aband!=0)}else{ACIPNO(w);}  // INCORPNC+this=INCORPRA, but using zap when abandoned.  If not recursive, must be non-inplace
   AAV(neww)[0]=w;   // install w as first box
  }else{
   // w was boxed, & a known singleton.  Put the single value into neww, then ra or zap if neww recursive.  neww is PRISTINE if w is abandoned pristine
   // We don't have access to the tpush stack, but if w is abandoned recursive we can use the slot in w as a surrogate location to zap - maybe could even if nonrecursive?
   AFLAGORLOCAL(neww,AFLAG(w)&((aband)<<AFPRISTINEX)) AFLAGPRISTNO(w) // transfer pristinity from abandoned w to neww; clear in w since contents escaping
   AAV(neww)[0]=AAV(w)[0];   // install w as first box
   if(likely(!(optype&BOX))){   // if neww is recursive...
    if((AFLAG(w)&(aband<<BOXX))!=0){AAV(w)[0]=0;}else{ra(AAV(w)[0]);}  // ... if w is abandoned recursive box, we zap the pointer in w; otherwise ra() it
   }else{ACIPNO(AAV(w)[0]);}  // if neww NOT recursive, no ra needed, but must incorp w
  }
  aband=1;  // We can always start adding to the lists created here, UNLESS a and w were the same - 
  w=neww;  // switch to new list
 }
 // now w has been boxed if needed & includes the new w value

 if(likely(aband>=AR(w))){   // if w is abandoned rank 1: (rank can't be 0, since we would have replaced it above)
  // w was abandoned.  We will store back-to-front, on the assumption that ; usually happens in bunches and w probably came from ;
  // if new w has no space before the beginning, allocate a bigger block and move w to the end of it
  if(unlikely(AK(w)==AKXR(1))){A neww;   // no space at the beginning of w
   I neededn=AN(w)+4+8; neededn=CTLZI(neededn); neededn=(2LL<<neededn)-8;  // number of atoms needed in larger block: room for at least 4 more, rounded up to power of 2 after header
   GATV0(neww,BOX,neededn,1); AN(neww)=AS(neww)[0]=AN(w); AK(neww)+=(neededn-(AN(w)+4))*SZI;  // allocate and position AK to put AN(w) atoms at end, with several spaces extra in case user wants to append in place
   MC(AAV(neww),AAV(w),AN(w)*SZI);  // copy the atoms from old to new
   AFLAGINIT(neww,AFLAG(w)) AFLAGANDLOCAL(w,~RECURSIBLE)  // If w recursive: transfer ownership of old blocks to new, making neww recursive (& maybe PRISTINE) and w nonrecursive.  If w nonrecursive, so is neww and they are independent
   w=neww;  // start adding to the new block
  }
  aband=SGNTO0(AC(a))&((I)jtinplace>>JTINPLACEAX)&&(a!=w);  // bit 0 = 1 if a is abandoned and is not the same as x
  if(unlikely(optype&1)){  // if verb is (,<) we deal with contents of a
   // if verb is (,<), a must be a boxed singleton.  move the contents of a into w, then ra/zap as above if w recursive.  This doesn't propagate WILLOPEN so boxed virtuals in a are impossible
   // a was boxed, & a known singleton.  Put the single value into w, then ra or zap.  w loses pristinity unless a is abandoned pristine
   // We don't have access to the tpush stack, but if a is abandoned recursive we can use the slot in a as a surrogate location to zap
   AFLAGANDLOCAL(w,(AFLAG(a)&((aband)<<AFPRISTINEX))|~AFPRISTINE) AFLAGPRISTNO(a) // transfer pristinity from abandoned a to w; clear in a since contents escaping
    // it would be OK to leave a pristine if it was abandoned, because we know a is a singleton and we are zapping it; that would allow early release of a.  But we're scared.
   A acontents=AAV(a)[0];  // save contents in case we zap it
   // It is still possible that a is virtual, for example if it came from {. 2 1 $ a: or a virtual argument cell.  
   if(likely(AFLAG(w)&BOX))if((AFLAG(a)&AFVIRTUAL)<(AFLAG(a)&(aband<<BOXX))){AAV(a)[0]=0;}else{ra(AAV(a)[0]);}  // Incr usecount if w is recursive; zappable if abandoned recursive and nonvirtual
   a=acontents;  // move to the contents of a, which we will install into w.  It is already incorped and has the usecount right to go into a recursive block
  }else{  // not (,<), i. e.  ;  (;<)  ,&<   all of which box a
   // Store a into w & return.  If a was abandoned recursive or direct, zap it, else ra.  If a is DIRECT abandoned, allow w to stay PRISTINE
   AFLAGANDLOCAL(w,((-(AT(a)&DIRECT))&((aband)<<AFPRISTINEX))|~AFPRISTINE)  // stays PRISTINE if abandoned DIRECT
   // if w is recursive, or WILLOPEN is not set, realize any virtual a.  Virtual a allowed only in WILLOPEN nonrecursive result
   if(likely((AFLAG(w)|~optype)&BOX))realizeifvirtual(a)
   AFLAGPRISTNO(a)   // since a is incorporated, it can't be PRISTINE
   if(likely(AFLAG(w)&BOX)){raczap(a,aband!=0)}else{ACIPNO(a)} // INCORPNC+this=INCORPRA, but using zap when abandoned; mark a incorped
  }
  // a has the new value to add at the front of the list
  AK(w)-=SZI; AN(w)=AS(w)[0]=AN(w)+1; AAV(w)[0]=a;  // install a at front, add to counts
  RETF(w);  // return the augmented result
 }
 // else fall through to handle general case
 // if WILLBEOPENED, any virtual boxes in w must be realized
 if(unlikely(optype&AT(w)&~AFLAG(w)&BOX))RZ(realizeboxedvirtuals(w));  // realize virtuals, in place.  Required only if WILLOPEN is set, and w is nonrecursive boxed.  Result 0 is error
 if((-AN(w)&SGNIF(AT(w),BOXX))>=0){w = jtbox(JTIPWonly,w);}   // box empty or unboxed w
 RETF(jtover(jtinplace,jtbox(JTIPAtoW,a),w,ds(CSEMICO)));  // join to boxed a
}

// Calculate the value to use for rf arg of copyresultcell: bit 0=ra() flag, next 15=rank requiring fill, higher=-(#leading axes of 1)
// zs, zr = address/length of shape of result cell   s,r = address/length of shape of cell to copy
static I rescellrarg(I *zs, I zr, I *s, I r){
 zs+=zr, s+=r;  // advance to end+1 of shapes
 zr-=r; zr*=(RMAX+1);  // amount of surplus rank, moved to high bits
 for(;r;--r)if(*--zs!=*--s)break;  // leave r with #axes up to & including the last requiring fill
 R (r-zr)<<1;  // join fields & return
}

// copy *w into memory area z, which is known to be big enough to hold it (like povtake, but recursive implementation that doesn't require pre-fill)
// the types of w and z are the same
// sizes is +/\. (shape of z),bpnoun(t(z)), i. e. the number of bytes in a result cell of each possible rank
// rf (bits 16 up) is the negative of number of leading axes of rank 1 to be appended to w
// rf (bits 1-15) is the number of axes of w requiring fill (lower axes are taken in full)
// rf (bit 0) is set if what is being moved is boxed pointers that need ra()
// s is the shape of w, *s being the length of the first axis after the leading appended 1s
// jt->fillv0 and fillv0len are set up
// result is the new position in w after the move
static C *copyresultcell(J jt, C *z, C *w, I *sizes, I rf, I *s){I wadv;I r=rf>>1;
 // if the entire w matches z, just copy it without fill
 if(r==0){
  // r=0   This can only happen if lower r was 0 originally, since we stop recursion at r=1.  r=0 means that
  // the entire r matched the suffix of the shape of zcell, and we can copy the entire cell
  wadv=sizes[0];
  if(unlikely(rf&1)){DO(wadv>>LGSZI, A a=((A*)w)[i]; ra(a); ((A*)z)[i]=a;)}else{JMC(z,w,wadv,1);}  // mustn't overcopy since we may go in reverse order
  R wadv+w;
 }
 // otherwise there will be fill
 C *endoffill=z+sizes[0];  // save address of end of area
 if(r==1){
  // r=1.  Lower cells are taken in full, so we can copy the cells en bloc
  wadv = s[0]*sizes[1]; // number of bytes to move
  if(unlikely(rf&1)){DO(wadv>>LGSZI, A a=((A*)w)[i]; ra(a); ((A*)z)[i]=a;)}else{MC(z,w,wadv);}
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
 // copy the fill, from z (new output pointer) to endoffill (end+1 of output cell).
 mvc(endoffill-z,z,jt->fillvlen,jt->fillv);    // use size of default fill
 R w;
}

// Variables are defined in result.h.  zz is values that matched the first cell, zzbox is the rest.  zzresultpri has combined types returned in zzbox
A jtassembleresults(J jt, I ZZFLAGWORD, A zz, A zzbox, A* zzboxp, I zzcellp, I zzcelllen, I zzresultpri, A zzcellshape, I zzncells, I zzwf, I startatend) {A zztemp;  // if we never allocated the boxed area (including force-boxed cases which never do) we just keep zz as the final result
 // startatend is 0 for forward, ~0 for reverse
 // Create a homogeneous array of results, by processing zzbox and zz one cell at a time from the end.
 //  Allocate the result area. (1) if USEOPEN and zz is empty, use zzbox; (2) if the largest cell-result is not bigger than the cells
 //  in zz, use zz.  Otherwise allocate a new area.  We normally copy from the end of the array to the front, so that we can stop, leaving
 //  values in place, when everything fits

 I zzt=AT(zz);  // type of zz
 I natomszzcell; PROD(natomszzcell,AR(zz)-zzwf,AS(zz)+zzwf);  // number of atoms in cell of zz
 A* box0=AAV(zzbox)+(startatend&(AN(zzbox)-1));  // address of last/first valid box pointer, depending on direction of movement
 C* zzcell=CAV(zz)+zzcellp;  // address of last+1 cell moved to zz

 if(likely(!(ZZFLAGWORD&ZZFLAGUSEOPEN))){
  // No sparse results.  We will bypass jtope and move the results into the result area here, with conversion and fill

  // Create the fill-cell we will need.  Note: all recursible fills must have the PERMANENT flag set, since we may not increment the usecount
  I zpri=TYPEPRIORITY(zzt); zpri+=AN(zz)?256:0;   // priority of unboxed results, giving high pri to nonempty
  I *zzcs=AS(zzcellshape);  // zzcs->shape of padded result cell (may be a faux A block) AS[] is shape, AR is rank, AN is allocation
  I zzcr=AR(zzcellshape);  // zzcr=rank of result cell
  zzresultpri=(zpri>zzresultpri)?zpri:zzresultpri; I zft=((I)1)<<(PRIORITYTYPE(zzresultpri&255));  // zft=highest precision encountered
  fillv0(zft);  // create default fill.

  I zfs=bpnoun(zft); zzcs[zzcr]=zfs;  // length of 0-cell is byte-length of atom - store after the shape - we know there's room.  zfs is byte=length of 1 atom of result type

  // if the result has different type from the values in zz, convert zz en bloc to type zft
  if(TYPESNE(zft,zzt)){I zzatomshift=CTTZ(bpnoun(zzt)); I zexpshift = CTTZ(bpnoun(zft))-zzatomshift;  // shift for size of atom; expansion factor of the conversion, as shift amount
   // here the old values in zz must change.  Convert them.  Tell cvt tp converts only as many atoms as given
   I zatomct=(zzcellp>>zzatomshift)-(startatend&(AN(zz)-(zzcelllen>>zzatomshift)))+1;   // get # atoms that have been filled in, offset by 1
   ASSERT(zz=ccvt(zft,zz,zatomct),EVDOMAIN);  // flag means convert zcellct atoms.  Not worth checking for empty
   // change the strides to match the new cellsize
   if(likely(zexpshift>=0)){zzcelllen<<=zexpshift; zzcellp<<=zexpshift;}else{zzcelllen>>=-zexpshift; zzcellp>>=-zexpshift;}
   zzcell=CAV(zz)+zzcellp;  //  recalc address of last+1 cell moved to zz
  }
  // Now zz has the type zft

  I natomsresultcell; PRODX(natomsresultcell,zzcr,zzcs,1);  // # atoms in actual result-cell.  Could overflow, if there is a mix of tall & wide
  I natomsresult; DPMULDE(natomsresultcell,zzncells,natomsresult);  // number of atoms in result
  // Since we know the result-cell size in zzcellshape must be able to contain a cell of zz, we can test for equal rank and equal number of atoms.
  // But if the cell is empty, we can't rely on # atoms to verify the shape, and then we have to reallocate
  if((((AR(zz)-zzwf)^zzcr) | (natomsresultcell^natomszzcell) | !natomsresultcell)){
   // The overall result-cell differs in shape or type from the cells of zz.  We must allocate a new result area.
   GA00(zztemp,zft,natomsresult,zzwf+zzcr);  // allocate result area, and point to shape
   MCISH(AS(zztemp),AS(zz),zzwf); MCISH(AS(zztemp)+zzwf,zzcs,zzcr);   // move in frame and shape

   // since zztemp is becoming the new result area, it should become inplace recursive if the type is recursible, and zz needs to
   // become nonrecursive, to transfer ownership of the contents of zz's blocks to zztemp without requiring explicit usecounting.
   // This might not work for XNUM/RAT types, because conversions may be required on results coming from zzbox, and those conversions
   // might fail: that would lose the blocks in zz that have not been copied to zztemp.  The problem does not exist for BOX type,
   // because we have already verified that there is not a mix of box/nonbox, so we can guarantee no failures until we get the
   // result area built.
   I zzrecur = AFLAG(zz)&BOX; AFLAGORLOCAL(zztemp,zzrecur) AFLAGANDLOCAL(zz,~zzrecur)  // transfer recursibility from zz to zztemp, but only for boxed result
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
    if((-AN(zzboxcell)&-TYPESXOR(zft,AT(zzboxcell)))<0){  // not empty and new type
     if(unlikely(!(zzboxcell=cvt(zft,zzboxcell)))){
      // error during conversion.  THIS IS THE ONLY PLACE WHERE ERROR IS POSSIBLE DURING THE COPY.
      // If zz and zztemp are the same block, and that block is recursive, it may be in an invalid state: values have been copied
      // from zzcell up the the end of the block and are now duplicated.  To fix it, we have to zero out anything was was copied but
      // not overwritten: for the forward case, everything in the range from the
      // current zzcell up to the last tempp that was filled (note that *tempp has not been filled yet).  In other words, we have
      // to fill zzncells+1 cells: the ones we haven't started, plus the one we failed on
      if(zztemp==zz&&zztemprecur){
       mvc((startatend>0)?tempp+zfs-zzcell:zzcell-tempp,(startatend>0)?zzcell:tempp,MEMSET00LEN,MEMSET00);  // clear, depending on direction.  If normal, we are going
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
     GA(zzz,zzt,0,zzwf,AS(zz)+zzwf); AN(zzz)=natomszzcell; AK(zzz)=zzcell-(C*)zzz;  // allocate empty header (which might fit in 1 cacheline); fill in length; point to data in zz
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

// move in a cell with fill
// a is shape of filled cell, w is cell (with correct type), *x is target address
// We assume the result area already has fill copied to it
static B povtake(J jt,A a,A w,C*x){B b;C*v;I d,i,j,k,m,n,p,q,r,*s,*ss,*u,*uu,y;
 ARGCHK1(w);
 r=AR(w); n=AN(w); k=bpnoun(AT(w)); v=CAV(w);
 if(1>=r){MC(x,v,k*n); R 1;}  // if list, fill is contiguous, just copy the data
 m=AN(a); u=AV(a); s=AS(w);
 p=0; d=1; DO(r, if(u[m-1-i]==s[r-1-i]){d*=s[r-1-i]; ++p;}else break;);  // p=#trailing axes of w that fill the cell; d=total # atoms in a p-cell of result
 b=0; DO(r-p, if(b=1<s[i])break;);  // b=0 iff all axes of w above the p-cell have unit length
 if(!b){MC(x,v,k*n); R 1;}  // unit-length cell filling low aces: fill is contiguous at end, just copy
 k*=d; ss=s+r-p; uu=u+m-p;  // k=#bytes of p-cell, n=#p-cells, ss->bottom axis+1 of p-frame of w, uu->bottom axis+1 of p-frame of a
 n/=d;
 for(i=0;i<n;++i){  // for each p-cell
  y=0; d=1; q=i; /* y=.a#.((-$a){.(($a)$1),$w)#:i */
  s=ss; u=uu; DQ(r-p, j=*--s; y+=q%j*d; d*=*--u; q/=j;);
  MC(x+y*k,v,k); v+=k;
 }
 R 1;
}

static B jtopes1(J jt,B**zb,A*za,A*ze,I*zm,A cs,A w){A a,e=0,q,*wv,x;B*b;I i,k,m=0,n,*v,wcr;P*p;
 n=AN(w); wcr=AN(cs); wv=AAV(w);
 GATV0(x,B01,wcr,1); b=BAV1(x); mvc(wcr,b,MEMSET00LEN,MEMSET00);
 for(i=0;i<n;++i)
  if(q=C(wv[i]),ISSPARSE(AT(q))){
   p=PAV(q); x=SPA(p,x); m+=AS(x)[0];
   if(!e)e=SPA(p,e); else ASSERT(equ(e,SPA(p,e)),EVSPARSE);
   k=wcr-AR(q); DO(k, b[i]=1;); a=SPA(p,a); v=AV(a); DQ(AN(a), b[k+*v++]=1;);
  }
 A bvec=ifb(wcr,b); makewritable(bvec) RZ(*za=bvec);    /* union of sparse axes           */ // avoid readonly
 *zb=b;                 /* mask corresp. to sparse axes   */
 *ze=e?e:num(0);          /* sparse element                 */
 *zm=m;                 /* estimate # of non-sparse cells */
 R 1;
}

static B jtopes2(J jt,A*zx,A*zy,B*b,A a,A e,A q,I wcr){A x;B*c;I dt,k,r,*s,t;P*p;
 dt=AT(e); r=AR(q); k=wcr-r; t=AT(q);
 if(ISSPARSE(t)){
  p=PAV(q);
  RZ(c=bfi(r,SPA(p,a),1));
  DO(r, if(b[k+i]!=c[i]){RZ(q=reaxis(ifb(r,k+b),q)); break;});
 }else{
  if(k){
   GA00(x,t,AN(q),wcr); s=AS(x); DQ(k, *s++=1;); MCISH(s,AS(q),r); 
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
 GASPARSE0(z,zt,1,wr+wcr); zs=AS(z); MCISH(zs,AS(w),wr); MCISH(zs+wr,AV(cs),wcr);
 zp=PAV(z); c=wcr-an; yc=wr+an;
 SPB(zp,e,cvt(dt,e)); e = SPA(zp,e);  // in case of reassignment by SPB
 GATV0(t,INT,yc, 1L); v=AVn(1L,t); DO(wr, v[i]=i;); DO(an, v[wr+i]=wr+av[i];); SPB(zp,a,t);
 GATV0(sh,INT,1+c,1L); s=AVn(1L,sh); s[0]=m; j=1; DO(wcr, if(!b[i])s[j++]=zs[wr+i];); 
 RE(xc=prod(c,1+s)); xk=xc*dk;
 GATV0(d,INT,wr,1); dv=AV1(d); mvc(wr*SZI,dv,MEMSET00LEN,MEMSET00);
 DPMULDE(m,xc,i) GA(x,dt,i,1+c,s); xv=CAVn(1+c,x); mvc(m*xk,xv,dk,AV(e));
 DPMULDE(m,yc,i) GATV0(y,INT,i,2L); v=AS(y); *v=m; v[1]=yc; yv=AVn(2L,y); mvc(SZI*i,yv,MEMSET00LEN,MEMSET00);
 for(i=p=0;i<n;++i){
  RZ(opes2(&x1,&y1,b,a,e,C(wv[i]),wcr)); v=AS(y1); m1=v[0]; k=v[1];
  if(m<p+m1){
   j=m; m=(i<n-1?m+m:0)+p+m1;
   RZ(x=take(sc(m),x)); xv=CAV(x)+p*xk; mvc(xk*(m-j),xv,dk,AV(e));
   RZ(y=take(sc(m),y)); yv= AV(y)+p*yc;
  }
  for(j=wr-1;j;--j)if(dv[j]==zs[j]){dv[j]=0; ++dv[j-1];}else break;
  v=AV(y1); DQ(m1, ICPY(yv,dv,wr); ICPY(yv+yc-k,v,k); yv+=yc; v+=k;); 
  if(memcmpne(1+AS(x1),1+s,SZI*c)){*s=m1; povtake(jt,sh,x1,xv);} else MC(xv,AV(x1),m1*xk);
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
F1(jtope){F1PREFIP;A cs,*v,y,z;C*x;I i,n,*p,q,r,*s,*u,zn;
 ARGCHK1(w);
 v=AAV(w);
 if(likely((RANKT)((AT(w)>>BOXX)&(BOX>>BOXX))>AR(w))){   // boxed and rank=0
  // scalar box: Turn off pristine in w since we are pulling an address from it.  Contents must not be inplaceable
  z=C(*v);
#if AUDITBOXAC
  if(!(AFLAG(w)&AFVIRTUALBOXED)&&AC(z)<0)SEGFAULT;
#endif
  PRISTCLRF(w) RETF(z);
 }
 n=AN(w);
 if(unlikely(((AT(w)&BOX)&REPSGN(-n))==0))RCA(w);  // return w if empty or open
 PROLOG(0080);
 // Here we have an array of boxes.  We will create a new block with the concatenated contents (even if there is only one box), and thus we don't need to turn off pristine in w
 // set q=min rank of contents, r=max rank of contents, t=type-masks for nonempty, te=type-masks for empty
 // Get the max cell-size if rank 1 or 2, and an indicator if all the shapes were equal
 // if (all shapes equal, taking omitted shape as 1), there is no fill
 //
 // We could keep track of whether the low 2 axes require fill, but fill overhead just isn't that high
 I *shapeptr; I maxshape0=0; I maxshape1=0; r=0; q=RMAX; I natoms=0;  // max axis_1, max axis _2, max rank, min rank, total number of atoms in input
 I te=0; I t=0;  // te=all types including empties; t=nonempty types.  If t!=0, te is immaterial
 for(i=0;i<n;++i){I s;
  y=C(v[i]); r=MAX(r,AR(y)); q=MIN(q,AR(y));  // could do this with shift if rank limited to 63
  natoms+=AN(y); if(likely(AN(y)!=0))t|=AT(y); te|=AT(y);  // count atoms.  accumulate types, either nonempty or empty.  Probably all the same AN, so use branch
  // accumulate max shape.  Extend short shapes with 1
  shapeptr=&AS(y)[AR(y)-1]; shapeptr=AR(y)>0?shapeptr:&oneone[1]; s=*shapeptr; maxshape0=MAX(maxshape0,s);
  --shapeptr; shapeptr=AR(y)>1?shapeptr:&oneone[1]; s=*shapeptr; maxshape1=MAX(maxshape1,s);
 }
 // r is (max) rank of result cell, q is smallest input-cell rank
 // If an input rank was > 2, we don't know whether we need fill or not, so assume the worst

 // if there was a nonempty, verify that the nonempties are compatible and find the highest-priority one
 // Fill creates a subtlety: we don't know whether empty boxes are going to contribute to
 // the result or not.  In a case like (0 2$a:),'' the '' will contribute, but the (0 2$a:) will
 // not.  And, we don't want to require compatibility with the fill-cell if nothing is filled.
 // So, we don't check compatibility for empty boxes.
 // The homogeneity flag h is set if max rank is 1 and there is 0 or 1 nonempty type.  In that case fill is contiguous for each cell and we just copy into the result area
 if(likely(t!=0)){
  // no mixed nonempties: t is homo num/char or all boxed or all symbol.
  ASSERT(0<=(POSIFHOMO(t,0)&-(t^BOX)&-(t^SBT)),EVINHOMO)
  ASSERT(((t^SPARSE)&SPARSE+XNUM+RAT)<=0,EVDOMAIN);  // don't allow a sparse that requires promotion to indirect
  te=t;  // te holds the type to use
 }
 I tsparse=te&SPARSE; te&=~SPARSE; // remove sparse flag
 t=te&-te; NOUNROLL while(te&=(te-1)){RE(t=maxtypedne(t,te&-te));}  // get highest-priority type
 t|=tsparse; ASSERT((t&(SPARSE|SPARSABLE))!=SPARSE,EVDOMAIN);  // error if result is unsparsable type
 // allocate place to build shape of result-cell;
 fauxblockINT(csfaux,5,1); I m;  // m is # atoms in cell
 if(likely((SGNIFSPARSE(t)|(1-r))>=0)){
  // Not sparse, and cell ranks were all < 2.  We know the max shape
  u=csfaux+2; u[r-1]=maxshape0; u[r-2]=maxshape1;  // u->cell shape; fill in the ranks we know
  DPMULDE(maxshape0,maxshape1,m);  // number of atoms in cell
 }else{
  // A cell had high rank (or was sparse).  We have to go back & recount the high ranks
  fauxINT(cs,csfaux,r+1,1) AS(cs)[0]=AN(cs)=r; u=IAV(cs);  // allocate extra axis for use in copyresultcell
  DO(r-q, u[i]=1;); p=u+r-q; DO(q-2, p[i]=0;); u[r-1]=maxshape0; u[r-2]=maxshape1;  // initialize to 1s above q, zeros below (this is adding leading 1s to missing leading axes); fill in known axes
  // find the shape of a result-cell
  DO(n, y=C(v[i]); s=AS(y); p=u+r-AR(y); DO(AR(y)-2,p[i]=MAX(p[i],s[i]);););  // go through blocks again finding max shape (not checking last 2 axes)
  if(unlikely(ISSPARSE(t))){z=opes(t,cs,w); EPILOG(z);} // if sparse, use sparse code
  else{
   PRODX(m,r,u,1);  // # atoms in cell
  }
 }
 // u->shape of cell, m=#atoms in cell.  Allocate result area & copy in shape (= frame followed by result-cell shape)
 DPMULDE(n,m,zn);  // Get total # results atoms now that we know result-cell size
 // now we know the number of atoms in the result.  If this differs from the number of input atoms, there is fill
 if(zn!=natoms){   // will there be fill?
  // there is fill
  ASSERT(!((I)jtinplace&JTNOFILL),EVASSEMBLY)  // error if fill not allowed
  if(unlikely(jt->fill)){A f=jt->fill;
   // user fill specified.  Install it, perhaps converting
   if(TYPESNE(t,AT(f))){ASSERT(HOMO(t,AT(f)),EVINHOMO) t=maxtypedne(t,AT(f)); if(AT(f)!=t)RZ(f=ccvt(t,f,0))}  // include fill in the type calc, and convert it if needed
   jt->fillvlen=bpnoun(t); jt->fillv=voidAV(f);  // use the specified single fill atom
  }else fillv0(t);  // default fill.  Go install it
 }
 I klg=bplg(t); GA00(z,t,zn,r+AR(w)); I *zcs=AS(z)+AR(w); MCISH(zcs,u,r); MCISH(AS(z),AS(w),AR(w))  // zcs->result-cell shape   klg=size of 1 atom
 x=CAV(z);  // x=output pointer, init to 1st cell
  // fill is (or may be) needed: create fill area, and convert cell-shape to cell-size vector needed by copyresultcell
 I zfs=(I)1<<klg; u[r]=zfs; DQ(r, u[i]=zfs*=u[i];)  // convert each atom of result-cell shape to the length in bytes of the corresponding cell; u->first length
 // Now move the results.  They may need conversion or fill
 JMCDECL(endmask) JMCSETMASK(endmask,m<<klg,0)
 for(i=0;i<n;++i){  // for each input box
  y=C(v[i]);   // get pointer to contents
#if AUDITBOXAC
   if(!(AFLAG(w)&AFVIRTUALBOXED)&&AC(y)<0)SEGFAULT;
#endif
  if(unlikely(!TYPESEQ(t,AT(y))))RZ(y=cvt(t,y));
  if(AN(y)==m)JMCR(x,CAV(y),m<<klg,0,endmask)
  else copyresultcell(jt,x,CAV(y),u,rescellrarg(zcs,r,AS(y),AR(y)),AS(y));
  x+=m<<klg;  // advance output pointer by cell length
 }
 EPILOG(z);
}

// ; y general case, where rank > 1 (therefore items are not atoms)
// w is the data to raze (boxed), t is type of nonempties (or empties/fill if there are no nonempties), n=#,w, r=max rank of contents of w, v->w data,
static A jtrazeg(J jt,A w,I t,I n,I r,A*v){A h,h1,y,z;C*zu;I c=0,i,j,k,m,*s,*v1,yr,*ys;I p;
 // Calculate the shape of a result-cell (it has rank r-1); c, the number of result-cells
 fauxblockINT(hfaux,4,1); fauxINT(h,hfaux,r,1) s=AV(h); mvc(r*SZI,s,MEMSET00LEN,MEMSET00);  // h will hold the shape of the result; s->shape data; clear to 0 for compares below
 I sigman=0, sigmascalars=0;  // total # atoms, # scalars
 for(i=0;i<n;++i){   // loop over all contents
  // y->A block for contents of w[i]; yr=rank of y; ys->shape of y
  // if contents has the same rank as result, it is an array of result-cells, and each item adds
  // to c, the total # items in result; otherwise it is a single cell that will be promoted in rank to
  // become one result-cell.  Error if overflow (should be impossible).  j=#leading length-1 axes that need to be added
  y=C(v[i]); yr=AR(y); ys=AS(y); I nitems=ys[0]; j=r-yr; nitems=j==0?nitems:1; c+=nitems; ASSERT(0<=c,EVLIMIT); 
  sigman+=AN(y);  // accumulate #atoms
  if(!yr){++ sigmascalars; continue;}   // do not perform rank extension of scalars, but count them
  // here we find the max cell size in *(s+1). *s is not used.  The maximum shape is taken
  // over extension axes of length 1, followed by the actual shape of the contents
  DO(j,  s[i]=MAX(1,    s[i]);     ); 
  DO(yr, s[j]=MAX(ys[i],s[j]); ++j;);
 }
 // Install the number of result items in s; m=total #result atoms; p=#atoms in 1 cell
 *s=c; PROD(p,r-1,s+1); DPMULDE(p,c,m);
 if(unlikely(jt->fill))if(m>sigman+sigmascalars*(p-1)){ASSERT(HOMO(t,AT(jt->fill)),EVINHOMO); t = maxtyped(t, AT(jt->fill));}  // If there is fill and #atoms out>#atoms in, include fill in the result-type.  It better fit in with the others

 // Now we know the type of the result.  Create the result.
 k=bpnoun(t); p*=k;  // k=#bytes in atom of result; p=#bytes/result cell
 fauxblockINT(h1faux,4,1); fauxINT(h1,h1faux,r,1) v1=AV(h1);  // create place to hold shape of cell after rank extension
 GA(z,t,m,r,s);    // create result area, shape s; zrel now is relocation offset for result
 zu=CAVn(r,z);  // output pointers
 // loop through each contents and copy to the result area
 for(i=0;i<n;++i){
  y=C(v[i]);  // y->address of A block for v[i]
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
F1(jtraze){A*v,y,z;C* RESTRICT zu;I *wws,d,i,klg,m=0,n,r=1,t=0,te=0;
 ARGCHK1(w);
 n=AN(w); v=AAV(w);  // n=#,w  v->w data
 if(unlikely(!(BOX&AT(w))))R n?ravel(w):mtv;   // if not boxed, just return ,w (but $0 if empty)
 if(unlikely(n<=1)){if(!n)R mtv; z=C(*v); PRISTCLRF(w) R AR(z)?z:ravel(z);}  // if just 1 box, return its contents - except ravel if atomic.  Since these contents are excaping via a pointer, w must lose pristinity
 // If there is more than 1 box w can remain pristine, because the (necessarily DIRECT) contents are copied to a new block
 // scan the boxes to create the following values:
 // m = total # items in contents; aim=#atoms per item;  r = maximum rank of contents
 // t = OR of types of nonempty blocks, te = OR of types of empties
 // if the block has been flagged as having homogeneous items, we can skip this step.  Such flagging indicates a compound like ;@:(<@f) where we knew during
 // result-assembly that we were going to raze the result, and we took the trouble to inspect the contents' shapes as they were going by.
 if(!(AFLAG(w)&AFUNIFORMITEMS)) {  // normal case
  for(i=0;i<n;++i){
   y=C(v[i]); r=MAX(r,AR(y));
   I yt=AT(y); te|=yt; m+=AN(y); yt=AN(y)?yt:0; t|=yt;
  }
  // if there was a nonempty, verify that the nonempties are compatible and find the highest-priority one
  // Fill creates a subtlety: we don't know whether empty boxes are going to contribute to
  // the result or not.  In a case like (0 2$a:),'' the '' will contribute, but the (0 2$a:) will
  // not.  And, we don't want to require compatibility with the fill-cell if nothing is filled.
  // So, we don't check compatibility for empty boxes.
  if(t){  // there was a nonempty
   ASSERT(0<=(POSIFHOMO(t,0)&-(t^BOX)&-(t^SBT)),EVDOMAIN)  // no mixed nonempties: t is homo num/char or all boxed or all symbol
   te=t;  // te holds the type to use
  }else if(jt->fill){te=AT(jt->fill);}  // all empty: use fill type if given; otherwise keep te as combined empty types
  t=te&-te; NOUNROLL while(te&=(te-1)){t=maxtypedne(t,te&-te);}  // get highest-priority type among all the set bits in either (all empties) or (all nonempties) or (fill)
  // t is the type to use.  i is 0 if there were no nonempties
  // if the cell-rank was 2 or higher, there may be reshaping and fill needed - go to the general case
  if(1<r)R razeg(w,t,n,r,v);
  // fall through for boxes containing lists and atoms, where the result is a list.  No fill possible, but if all inputs are
  // empty the fill-cell will give the type of the result (similar to 0 {.!.f 0$...)

  GA0(z,t,m,1);  // allocate the result area (rank 1)
  zu=CAV1(z); klg=bplg(t); // input pointers, depending on type; length of an item
  // loop through the boxes copying
  for(i=0;i<n;++i){
   y=C(v[i]); if(AN(y)){if(TYPESNE(t,AT(y)))RZ(y=cvt(t,y)); d=AN(y)<<klg; MC(zu,AV(y),d); zu+=d;}
  }
 }else{
  // special case where the result-assembly code checked to make sure the items were uniform.  In this case the number of items was hidden away in the AM field (otherwise unneeded, since we know the block isn't virtual)
  // and we just need to extract the rank and the type, and calculate the atoms-per-item.  We know there is at least one box.
  A ww0=AAV(w)[0]; t=AT(ww0); r=AR(ww0); r+=SGNTO0(r-1); wws=&AS(ww0)[0]; // ww0->1st contents; t=type; r=rank>.1; wws->item shape
  PROD(m,r-1,wws+1);  // get #atoms in an item of w
  I nitems=AS(w)[0];  // total # result items is stored in w
  GA(z,t,m*nitems,r,wws); AS(z)[0]=nitems; // allocate the result area; finish shape
  zu=CAVn(r,z); klg=bplg(t); // input pointers, depending on type; length of an item
  // loop through the boxes copying the data into sequential output positions.  pyx impossible
  DO(n, y=v[i]; d=AN(y)<<klg; JMC(zu,AV(y),d,0); zu+=d;)   // copy the items
 }

 RETF(z);
}

F1(jtrazeh){A*wv,y,z;C*xv,*yv,*zv;I c=0,ck,dk,i,k,n,p,r,*s,t;
 ARGCHK1(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 if(!AR(w))R ope(w);
 n=AN(w); wv=AAV(w);  y=C(wv[0]); SETIC(y,p); t=AT(y); k=bpnoun(t);  // k is size of an atom
 DO(n, I l; y=C(wv[i]); r=AR(y); ASSERT(p==SETIC(y,l),EVLENGTH); ASSERT(r&&r<=2&&TYPESEQ(t,AT(y)),EVNONCE); c+=1==r?1:AS(y)[1];);
 GA00(z,t,p*c,2); s=AS(z); s[0]=p; s[1]=c;  // p is # items in each input box, thus # rows in result; c is # atoms in each row
 if(t&BOX){
  // boxed contents.  Make the result recursive; since each input box is going into exactly one slot in the result, we get the usecounts right if we
  // raise the usecount in the contents of w
  z=jtra(w,BOX,z); AFLAGORLOCAL(z,BOX)
  PRISTCLRF(w);   // contents have escaped.  w is no longer used
 }
 zv=CAV2(z); ck=c*k;  // ck is length of one row in bytes
 for(i=0;i<n;++i){
  // zv is the start of the next output position in the first row
  y=C(wv[i]); dk=1==AR(y)?k:k*AS(y)[1];  // y is contents of this box; dk is # atoms in each row of THIS Input box;
  if(!dk)continue;  // if empty, nothing to move
  xv=zv; zv+=dk; // xv is output pointer for this column; advance zv to next column
  yv=CAV(y);
  DQ(p, MC(xv,yv,dk); yv+=dk; xv+=ck;);
 }
 RETF(z);
}    /* >,.&.>/,w */

