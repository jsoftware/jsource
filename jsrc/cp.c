/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Power Operator ^: and Associates                          */

#include "j.h"
#define ZZDEFN
#include "result.h"
#define STATEPOSITIVEPOWX 13
#define STATEPOSITIVEPOW (((I)1)<<STATEPOSITIVEPOWX)
#define STATEINFINITEPOWX 14
#define STATEINFINITEPOW (((I)1)<<STATEINFINITEPOWX)
#define STATENEEDNEWPOWX 15
#define STATENEEDNEWPOW (((I)1)<<STATENEEDNEWPOWX)
#define STATEMONADX VERBX  // 27 set 
#define STATEMONAD (((I)1)<<STATEMONADX)
#define ZZFLAGWORD state

// x is a block that contains cell results.  We open it; if that creates domain error we track down the error and change it to assembly error
A jtopenforassembly(J jt, A x){
 ARGCHK1(x);  // if error on x, abort this
 A z=ope(x);   // open x
 if(z==0&&jt->jerr==EVDOMAIN){  // domain error on open can only be assembly
  jt->etxinfo->asseminfo.assemframelen=1;  // len of frame of error
  jt->etxinfo->asseminfo.assemorigt=jt->etxinfo->asseminfo.assemwreckt=0;  // init dissimilar types not found
  DO(AN(x), if(AN(AAV(x)[i])){if(jt->etxinfo->asseminfo.assemorigt==0)jt->etxinfo->asseminfo.assemorigt=AT(AAV(x)[i]);  // look at nonempties: save the first and any incompatible
            else if(!HOMO(jt->etxinfo->asseminfo.assemorigt,AT(AAV(x)[i]))){jt->etxinfo->asseminfo.assemwreckt=AT(AAV(x)[i]); jt->etxinfo->asseminfo.assemwreckofst=i; break;}}
  )
  jt->etxinfo->asseminfo.assemshape[0]=AS(x)[0];  // results were in a list
  jt->jerr=EVASSEMBLY;  // switch to assembly error type
 }
 R z;
}

// chase the chain for transitive closure, starting at indexes[start].  length of indexes is n.  Result is list of indexes, or LIMIT error if cycle
static A jttclosure1(J jt, I start, I n, I *indexes){
 I c=start;
// should be if((UI)c>=(UI)n){c+=n; ASSERT((UI)c<(UI)n,EVINDEX);} I c0=c;   // c=current index, brought into range
// should be  I rn=0,p,c0=c; do{p=c; c=indexes[c]; if((UI)c>=(UI)n){c+=n; ASSERT((UI)c<(UI)n,EVINDEX);} ASSERT(rn<=n,EVLIMIT) ++rn;}while(c!=p);  // p is prev; rn is # loops, including the one that ends with the repeat
// should be  A z; GATV0(z,INT,rn,1) I *zv=IAV1(z); c=c0; DO(rn, zv[i]=c; c=indexes[c]; if((UI)c>=(UI)n)c+=n;);  // allocate the result and retraverse, copying
 // as implemented a final negative index is emitted even if it repeates the previous positive index - too late to change in the field
 I rn=0,p,c0=c; do{p=c; if((UI)c>=(UI)n){c+=n; ASSERT((UI)c<(UI)n,EVINDEX);} ASSERT(rn<=n,EVLIMIT) ++rn; c=indexes[c];}while(c!=p);  // p is prev; rn is # loops, including the one that ends with the repeat
 A z; GATV0(z,INT,rn,1) I *zv=IAV1(z); c=c0; DO(rn, zv[i]=c; if((UI)c>=(UI)n)c+=n; c=indexes[c];);  // allocate the result and retraverse, copying
 R z; 
}
// {&a^:(<_) w
// AR(a)<=1 and AN(w)!=0
static F2(jttclosure){
 ARGCHK2(a,w);
 I wt=AT(w); if(B01&wt)RZ(w=cvt(INT,w)); I wn=AN(w); I wr=AR(w); I *wv=AV(w);  // convert w to int, get n/t/r/data
 I *av=AV(a); I an=AN(a);
 if(likely(wr==0))R jttclosure1(jt,wv[0],an,av);  // single chain, return it
 // falling through for rare case of multiple chains
 PROLOG(0);
 A z; GATV(z,BOX,wn,wr,AS(w)) A *zv=AAVn(wr,z); // allocate one box per chain, point zv to boxes
 DO(wn, RZ(zv[i]=jttclosure1(jt,wv[i],an,av)) )  // fill in the boxes
 z=ev12(z,0,"((i.@#@$) |: (([ , (-#)~ # {:@[)&> >./@:(#@>)))");  // pad each box with repetitions of the last ele, unbox, transpose last axis to first
 EPILOG(z);
}

// [a] u^:atom w and [a] u^:<(atom or '') w, bivalent.
static DF2(jtpowatom12){F1PREFIP;
 {A na=EPMONAD?0:a; w=EPMONAD?a:w; a=na;}  // w arg will be power result; a=0 if monad
 I poweratom=FAV(self)->localuse.lu1.poweratom;  // multiple/neg/power field
#define POWERAMULT (I)1   // set if power was boxed
#define POWERANEG (I)2   // set if power is negative
#define POWERADOWHILEX 2  // When this bit is set in an infinite power, we are running ^:_.
#define POWERADOWHILE ((I)1<<POWERADOWHILEX)
_Static_assert(POWERADOWHILE==JTDOWHILE,"bit field mismatch");
#define POWERABSX 3  // bits 2&up=|power|, which makes poweratom negative for infinite power
#define POWERABS ((I)1<<POWERABSX)
#define POWERAEXTX (BW-2)   // when this bit is CLEAR in an infinite power, we have advanced to the runout polishing stage
#define POWERAEXT ((I)1<<POWERAEXTX)
 A fs=FAV(self)->fgh[0];  // the verb u
 PROLOG(0);  // 
 A zz=0; // zz=result array if multiple results, 0 if single. 
 UI zzalloc;  // if zz!=0, the #atoms in the area allocated in zz
 if(unlikely(poweratom&POWERAMULT)){   // multiple results, which we flag here as unlikely
  zzalloc=(poweratom>>POWERABSX)+1; zzalloc=poweratom<0?32-(AKXR(1)>>LGSZI):zzalloc;   // allocate correct length, or a block of 28 if unknown size
  GATV0(zz,INT,zzalloc,1) AT(zz)=BOX; AFLAG(zz)=BOX&RECURSIBLE;   // alloc result area.  avoid init of BOX area
  INCORPRA(w); AN(zz)=1; AAV1(zz)[0]=w;   // install w as first result (fill in AS later)
 }
 if(unlikely(poweratom&POWERANEG)){
  // negative power not _1.  This is a path not worth much attention.  Create the inverse of u
  A uinv; WITHMSGSOFF(uinv=a?invamp(a,fs,0):inv(fs);) ASSERT(uinv!=0,EVDOMAIN) a=0; fs=uinv;  // try inverse, monad or dyad
 }
 A *old=jt->tnextpushp;  // save tpop pointer, so as to leave untouched zz and anything related to the inverse
 AF f12=FAV(fs)->valencefns[!!a];  // action routine for u
 I allowinplace=((FAV(fs)->flag>>(VJTFLGOK1X+!!a-JTINPLACEWX))&((poweratom&IMIN+POWERADOWHILE)==IMIN?0:JTINPLACEW))|~JTINPLACEW;  // suppress all inplacing if verb doesn't support it, or this is convergence (or multiple results, where jt is always 0)
 jtinplace=(J)((I)jtinplace&allowinplace&~(JTINPLACEA|(a==w)));  // never inplace a; pass inplacing of w only if !multiple & !infinite, and not same as a
 jtinplace=(J)((I)jtinplace|(poweratom&JTDOWHILE));  // set flag to indicate ^:_.
 poweratom>>=POWERABSX;  // convert powerabs to iteration counter
 for(;poweratom!=0;poweratom--){  // for all requested powers
  A wnew=CALL12IP(a,f12,a?a:w,w,fs);  // execute the verb, monad or dyad
  RZ(wnew);  // exit if error
  if(poweratom<0){   // convergence/dowhile search
   if((I)jtinplace&JTDOWHILE){if(wnew==(A)1)break;  // exit dowhile when v returns false, signaled to us by a special result
   }else if(equ(wnew,w)){   // otherwise, we are testing for convergence
    // convergence iteration that matches
    if(zz!=0)break;   // if multiple results, stop without storing the repetition
    if(AT(wnew)&EXACTNUMERIC)break;   // if perfect match, keep the value
    if(FAV(fs)->id==CPOWOP)break;  // u^:v^:_ is old-style DoWhile.  For compatibility, stop after the first match, which might have been triggered by v.  We could perhaps check this explicitly?
    // match on inexact value.  Start a counter; after a few matches, take what we have.
    // We are probably doing Newton polishing and we should go a couple more cycles after the first match
    if((poweratom<<1)<0)poweratom=IMIN+2;  // first match: start counter, which ends after 2 further polishing iterations
    else if((poweratom<<1)==0){w=wnew; break;}  // after counter, keep the last matching value
   }else{if((poweratom<<1)>=0)break;}  // convergence search, no match: if we have had a match, stop with the last match (w)
   w=wnew;  // remember the last value
  }
  // Turn on inplacing if it is possible after the initial execution.  Never if infinite, since the old block is needed for comparison; or if verb is not inplaceable.  N/C for multiple,
  // since the old block will be INCORPed anyway; OK for single UNLESS w could not be inplaced and u returned w as its result.  Once a chain of returning w has been broken
  // it cannot be resumed unless w has been marked noninplaceable.
  jtinplace=(J)((I)jtinplace|((wnew!=w)&(wnew!=a)&allowinplace));  // make sure we don't inplace a repeated arg
  w=wnew;  // advance the result to be the new w
  if(unlikely(zz!=0)){
   // multiple results.  incorp the result into zz, extending zz if needed
   UI newslot=AN(zz);  // where the new value will go
   if(withprob(newslot==zzalloc,0.03)){  // current alloc full?
    // current alloc is full.  Double the allocation, swap it with zz (transferring ownership), and copy the data
    zzalloc=2*zzalloc+(AKXR(1)>>LGSZI);  // new allocation, cacheline multiple
    A zznew; GATV0(zznew,INT,zzalloc,1) A *zznewzap=AZAPLOC(zznew); A *zzzap=AZAPLOC(zz);  // allocate, & get pointers to tstack slots old & new
    JMC(AAV1(zznew),AAV1(zz),newslot<<LGSZI,0) AT(zz)=INT; AFLAG(zz)=0; *zzzap=zznew; *zznewzap=zz; zz=zznew;  // swap buffers, transferring ownership to zznew & protecting it, neutering zz, setting zz to be freed
    AZAPLOC(zz)=zzzap; AT(zz)=BOX; AFLAG(zz)=BOX&RECURSIBLE;    // new zz now has pointers to allocated blocks and to its dedicated zaploc
   }
   realizeifvirtual(w); razaptstackend(w);  // realize w if needed for store.  Since zz has taken over protection of w, it is OK to zap w whenever its tstack entry is in this stack frame
   AAV1(zz)[newslot]=w; AN(zz)=newslot+1;  // install the new value & account for it in len
   if((newslot&7)==0)tpop(old);  // u shouldn't leave much lying around, but we clean up every so often
  }else{RZ(w=gc(w,old))}   // single result.  It stays in w but we need to free the old w
  JBREAK0   // allow ATTN to break into the loop
 }
 if(zz!=0){AS(zz)[0]=AN(zz); RZ(w=jtopenforassembly(jt,zz));}   // if multiple, assemble the results
 EPILOG(w)  // return the result
}

// {&x^:a: w   or  a {~^:a: w, bivalent.  Each supports 1 valence; we revert for the other, or in not integer args
static DF2(jtindexseqlim12){
 ARGCHK2(a,w);
 A origa=a, origw=w;  // in case we revert
 if(unlikely(EPMONAD)){if(FAV(FAV(self)->fgh[0])->id!=CAMP)goto revert; w=a; a=FAV(FAV(self)->fgh[0])->fgh[1];  // monad, verify {&x^:a:, use the x
 }else{if(FAV(FAV(self)->fgh[0])->id==CAMP)goto revert;}  // dyad, verify {~^:a:
 if(AR(a)<=1&&AT(a)&INT&&AN(w)&&AT(w)&B01+INT)R tclosure(a,w);
revert:;
 R jtpowatom12(jt,origa,origw,self);
}

// general u^:n w where n is any integer array or finite atom.  If atom, it will be negative.  Bivalent
static DF2(jtply12){A fs=FAV(self)->fgh[0]; PROLOG(0040);A zz=0;
#define ZZWILLBEOPENEDNEVER 1  // can't honor willbeopened because the results are recycled as inputs
#define ZZPOPNEVER 1  // can't pop the inputs - 
#define ZZDECL
#include "result.h"
 I state=ZZFLAGINITSTATE;  // flags for result.h
 state|=EPMONAD<<VERBX; w=EPMONAD?a:w;  // set MONAD flag, w=correct w, a=a or garbage
 AF f12=FAV(fs)->valencefns[!(state&VERB)]; // fetch routine for positive powers
 // p =. ~. sn=.(gn=./:,n) { ,n   which gives the list of distinct powers
 A n=FAV(self)->fgh[2]; A rn; RZ(rn=ravel(n));  // n is powers, rn is ravel of n
 A gn; RZ(gn=grade1(rn)); A p; RZ(p=nub(fromA(gn,rn)));  // gn is grade of power, p is sorted list of unique powers we want
 // find index of first nonneg power, remember, set scan pointer, set direction forward.  Set current power to 0.  Indic read of power needed
 I *pv=IAV(p); I np=AN(p);  // base of array of powers, and the number of them
 A z=w;  // the next input/previous result
 I zpow=0;  // The power corresponding to the value in z
 I pscan; for(pscan=0;pscan<np&&pv[pscan]<0;++pscan);  // scan pointer through powers, initialized to point to first nonnegative
 I pscan0=pscan;  // remember where the positive scan started
 state|=STATEPOSITIVEPOW|STATENEEDNEWPOW;  // start in positive direction
 // remember if there is an infinite power in this direction
 state|=(pv[np-1]==IMAX)<<STATEINFINITEPOWX;   // infinities are now +/- IMAX
 A invfs; if(pv[0]<0){RZ(invfs=state&STATEMONAD?inv(fs):invamp(a,fs,0)); state&=~ZZFLAGBOXATOP;}   // if inverse, look it up outside the tpop loop, and ignore <@ optimization that won't be in the inverse
 ZZPARMS(1,np,12,!(state&STATEMONAD))
#define ZZINSTALLFRAME(optr) *optr++=np;
 // loop for each result power:
 I neededpow;  // the next power we are looking for
 A *stkfreept=jt->tnextpushp;  // stack-free pointer: we will free non-result values back to this point
 while(1){
  // pscan points to the next power to read - could be out of bounds.  But we might not be reading yet
  // If read of power needed:
  if(state&STATENEEDNEWPOW){
   if(!BETWEENO(pscan,0,np)){
    // No new power this direction; switch direction if currently moving positive
    if(state&STATEPOSITIVEPOW){
     if((pscan=pscan0-1)>=0){  // start on first negative power.  If there is one.
      state&=~(STATEPOSITIVEPOW|STATEINFINITEPOW);  // start in negative direction
      state|=(pv[0]==-IMAX)<<STATEINFINITEPOWX;  // set if there is a __ power
      z=w;  // start back on the initial arg
      zpow=0;  // reset the power corresponding to z
      fs=invfs; f12=FAV(fs)->valencefns[0];  // switch over to the inverse function
     }
    }
    if(!BETWEENO(pscan,0,np))break;  // when we exhaust the powers, quit the loop
   }
   neededpow=pv[pscan]; pscan+=state&STATEPOSITIVEPOW?1:-1; state&=~STATENEEDNEWPOW;  // take next power; advance to following; set no power needed till this consumed
  }
  if((neededpow^REPSGN(neededpow))<=(zpow^REPSGN(neededpow))){  // check for closer to 0, based on direction
   // desired power<=current power, store result and indic power needed.  z has the value to use as result
   // Turn off pristinity in the result because storing it is a version of escape
   AFLAGPRISTNO(z)  // shared blocks can't be pristine
#define ZZBODY
#include "result.h"
   state|=STATENEEDNEWPOW;   // once we store a power we need the next one
  }else{  // don't execute the verb willy-nilly because the store we just made may be the last  (we could check pscan and execute if we know another power is needed - but don't execute if zpow is infinite)
   // else execute verb, increment current power.  If there is an infinite power, see if result matches input, set current power to infinite if so
   A oldz=z; RZ(z=CALL12(!(state&STATEMONAD),f12,state&STATEMONAD?z:a,z,fs));  // run the verb
   zpow+=state&STATEPOSITIVEPOW?1:-1;  // increment the power corresponding to z
   if(state&STATEINFINITEPOW){if(equ(oldz,z))zpow=state&STATEPOSITIVEPOW?IMAX:-IMAX;}  // if no change, make z apply to everything.  Don't check unless _ power given
   if(!(zpow&7)){
    // Since we are not popping inside the loop (because each result must be reused even after its values have been moved), we
    // have to pop explicitly.  We protect the components of the overall result and the most recent result
    // We do this only occasionally
    JBREAK0;  // while we're waiting, check for attention interrupt
    if(!gc3(&z,zz?&zz:0,zzbox?&zzbox:0,stkfreept))R0; // free old unused blocks
    if(zzbox){
     // zzbox is normally NONrecursive and we add boxes to it as they come in.  Protecting it has made it recursive, which will
     // cause a double-free if we add another box to it.  So we have to go through it and make it nonrecursive again.  We don't have to do it recursively.
     // This is regrettable, but rare.  If we cared, we could save the whole gc3() call any time the result is going to be stored in zzbox, since there's
     // nothing else to free; but that's not worth it.  zzbox itself becomes nonrecursive but its descendants remain recursive, which is important because
     // tpop expects recursive contents
     AFLAGANDLOCAL(zzbox,~BOX); DQ(AN(zzbox), if(AAV(zzbox)[i])tpush(AAV(zzbox)[i]);)  // mark zzbox nonrecursive; for each child, replace the implied free with an explicit one on the stack
    }
   } 
  }
 }
 // collect result
#define ZZEXIT
#include "result.h"
 // Now zz has the result
 // if (there is a negative power) p =. (nnegs }. p) , |. nnegs {. p to match the order in which results were stored
 if(pscan0){A sneg; RZ(sneg=sc(pscan0)); RZ(p=apip(drop(sneg,p),reverse(take(sneg,p))));}
 // result is ($n) $ (p i. ,n) { result - avoid the reshape if n is a list, and avoid the from if (p i. ,n) is an index vector
 if(!jtisravelix(jt,n))RZ(zz=fromA(indexof(p,rn),zz));   // if n is not i. #,n already, put zz into order of p i. ,n
 if(AR(n)!=1)zz=reitem(shape(n),zz);  // if n is an array, use its shape
 EPILOG(zz);
}

// action routines for u^:_1: general case.  look up the inverse
static DF1(jtinv1){A fs=FAV(self)->fgh[0]; F1PREFIP; A z; ARGCHK1(w);A i; RZ(i=inv(fs)); WITHEFORMATDEFERRED(z=(FAV(i)->valencefns[0])(FAV(i)->flag&VJTFLGOK1?jtinplace:jt,w,i,i);) RETF(z);}
  // we defer eformat till the caller so that the user doesn't see the description of the inverse, which might be unrecognizable
// for the dyad, look up the inverse of x&u
static DF2(jtinv2){ A fs=FAV(self)->fgh[0]; F2PREFIP; A z; ARGCHK2(a,w); A i; RZ(i=invamp(a,fs,0)); STACKCHKOFL WITHEFORMATDEFERRED(z=(FAV(i)->valencefns[0])(FAV(i)->flag&VJTFLGOK1?(J)((I)jtinplace&~JTINPLACEA):jt,w,i,i);) RETF(z); }  // the CHKOFL is to avoid tail recursion, which prevents a recursion loop from being broken
// if u has no names, meaning we can take the inverse early for the monadic case:
static DF1(jtinvh1){A hs=FAV(self)->fgh[2]; F1PREFIP;A z; ARGCHK1(w); z=(FAV(hs)->valencefns[0])(jtinplace,w,hs); RETF(z);}  // monadic inverse was already looked up in h
static DF1(jtinverr){F1PREFIP; ASSERT(0,EVDOMAIN);}  // uninvertible monad: come here if inverse invoked as a monad (x&u^:_1 might be OK as a dyad) 


// [x] u^:v y, fast when result of v is 0 or 1 (=if statement).  jtflagging is that required by u.  JTDOWHILE can be set to indicate that this is called from ^:_. in which case we return (A)1 if v returned 0
// if result of v is not 0/1, we reexecute [x] u^:n y  where n is ([x] v y)
static DF2(jtpowv12cell){F2PREFIP;A z;PROLOG(0110);
 w=EPMONAD?0:w;  // w is 0 for monad
 A u,uc; I u0; A gs=FAV(self)->fgh[1]; A fs=FAV(self)->fgh[0]; AF uf=FAV(fs)->valencefns[!!w]; // fetch uself, which we always need, and uf, which we will need in the fast path
 RZ(u=CALL12(w,FAV(gs)->valencefns[!!w],a,w,gs));  // execute v, not inplace
 if(likely(((AT(u)&~(B01+INT))|AR(u)|((u0=BIV0(u))&~1))==0)||(AR(u)==0&&AT(u)&NUMERIC&&(uc=cvt(INT,u))!=0&&!((u0=IAV(uc)[0])&~1))){  // v result is atomic bool/int 0/1 (if statement)  overfetch possible but harmless
  if(u0){jtinplace=(J)((I)jtinplace&~(a==w?JTDOWHILE+JTINPLACEA+JTINPLACEW:JTDOWHILE)); z=CALL12IP(w,uf,a,w,fs);}   // v result is atomic INT/B01 1: execute u, inplace if possible - but suppress the DOWHILE flag, and all inplacing if a=w
  else{z=w?w:a; if((I)jtinplace&JTDOWHILE)R (A)1;}  // v result is atomic INT/B01 0: bypass.  If ^:_. return 1 to indicate u returned 0 - skips EPILOG but powatom12 will soon do one
 }else{ASSERT(!((I)jtinplace&JTDOWHILE),EVDOMAIN) RZ(u=powop(fs,u,(A)1));  // not a simple if statement: fail if called from ^:_.; create u^:n form of powop;
  jtinplace=FAV(u)->flag&(VJTFLGOK1<<(!!w))?jtinplace:jt;   // u might have been a gerund so there is no telling what the inplaceability of the new u is; refresh it
  z=CALL12IP(w,FAV(u)->valencefns[!!w],a,w,u);  // execute the power, inplace
 }
 EPILOG(z);
}

// execution of [x] u^:gerund y as (x v0 y) u^:(x v1 y) (x v2 y)   or  u^:(v1 y) (v2 y)
// if v1 evaluates to 0, avoid evaluating v0
// Apply the gerunds to xy, then  apply u^:n
static DF2(jtgcr12){F2PREFIP;PROLOG(0);
 w=EPMONAD?0:w;  // throughout this bivalent routine a is w/a w is 0/w  (monad/dyad) w!=0 is the dyad flag
 jtinplace=(J)((I)jtinplace&((a==w)?~(JTINPLACEA+JTINPLACEW):~0));  // cannot inplace a/w if they are equal
 I origjtinplace=(I)jtinplace;  // remember initial inplacing status of args - after equality check
 A *hv=AAV(FAV(self)->fgh[2]); I hn=AN(FAV(self)->fgh[2]);  // hv->v0`v1`v2.  These cannot be pyxes.  hn is their number
 STACKCHKOFL     // unless we prevent it, the gerund might return a gerund, creating an infinite loop
 A ff;   // the routine for the function -  u or u^:newpower
 A z0; RZ(z0=CALL12(w,FAV(hv[hn-2])->valencefns[!!w],a,w,hv[hn-2]))  // z=.[x] v1 y  which is the power to use
 A uc; I u0; if(likely(((AT(z0)&~(B01+INT))|AR(z0)|((u0=BIV0(z0))&~1))==0)||(AR(z0)==0&&AT(z0)&NUMERIC&&(uc=cvt(INT,z0))!=0&&!((u0=IAV(uc)[0])&~1))){  // v1 result is atomic bool/int 0/1 (if statement)
  ff=FAV(self)->fgh[0]; ff=u0?ff:0;  // we will execute u or nothing; ff tells which
 }else{  // v1 returned something other than 0/1
  RZ(ff=powop(FAV(self)->fgh[0],z0,(A)1))  // ff=.u^:power, a verb to execute.  Any arg used in ff will no longer be inplaceable
  u0=1;  // indicate we will execute ff
 }
 A v2=hv[hn-1];  // the verb for v2 (frees hv over call)
 if(w&&3-hn<u0){   // execute v0 if dyad, but only if we are going to execute u and v0 was given (i. e. hn=3 and u0=1)  could use MIN and UI
  RZ(z0=CALL12(w,FAV(hv[0])->valencefns[1],a,w,hv[0]))  // z0=[x] v0 y  if this is a dyad
  jtinplace=(J)((I)jtinplace&~(JTINPLACEA*(z0==a)+JTINPLACEW*(z0==w)));  // if v0 returned an argument, remove v2 inplacing for that argument
 }else z0=w?a:0;  // if dyad, set v0 result to x for omitted v0; if monad, set z0=0 as a flag
 jtinplace=(FAV(v2)->flag&(VJTFLGOK1<<!!z0))?jtinplace:jt;  // turn off inplacing if v2 doesn't support it
 A z; RZ(z=CALL12IP(w,FAV(v2)->valencefns[!!z0],a,w,v2))  // z=[x] v2 y  inplacing if supported
 if(ff){  // if the power was 0, keep the y value as the result, otherwise...
  // we allow inplacing z0 and z UNLESS they match an input x/y that was not inplaceable
  I zipno=(((z==w)&~origjtinplace) | ((z==a)&~(origjtinplace>>!!z0)));  // if z=w, it must be dyadic & we look at original w.  If z=a it could be either & we look at dyad/monad depending
  I z0ipno=!!z0 & (((z0==w)&~origjtinplace) | ((z0==a)&~(origjtinplace>>!!z0)));  // same for z0, but never if monad
  jtinplace=(J)((I)jt+((JTINPLACEA+JTINPLACEW)^(JTINPLACEA*z0ipno+JTINPLACEW*zipno)));  // inplace unless copied.  If monad, z0=w=0
  jtinplace=(FAV(ff)->flag&(VJTFLGOK1<<!!z0))?jtinplace:jt;  // turn off inplacing if v2 doesn't support it
  z=CALL12IP(z0,FAV(ff)->valencefns[!!z0],z0?z0:z,z,ff);  // z=[z0] u^:power z
 }
 EPILOG(z);
}

// called for ^:(v0`v1`v2) forms.
// Creates a verb that will run jtgcr12 to execute the gerunds on the xy arguments, and
// then execute the operation
// a is the original u (a verb), w is the original n (boxed)
static A jtgconj(J jt,A a,A w){A hs;
 ARGCHK2(a,w);
 ASSERT((AN(w)&-2)==2,EVLENGTH);  // length is 2 or 3
 RZ(hs=fxeachv(1,w));  // convert gerund to array of A blocks, each verified to be a verb.
 R fdef(0,CPOWOP,VERB, jtgcr12,jtgcr12, a,w,hs, VGERR+VJTFLGOK1+VJTFLGOK2, RMAX,RMAX,RMAX);
}



// This executes the conjunction u^:v to produce a derived verb.  If the derived verb
// contains verb v or gerund v, it executes v on the xy arguments and then calls jtpowop
// to recreate a new derived verb with the function specified by the result of v.
// The result of v could be a gerund, which would cause the process to continue, so we put the
// kibosh on it by setting self (otherwise unused, and set to nonzero in the initial invocation
// from parse) to 0 in all calls resulting from execution of gerund v.  Then we fail any gerund
// if self is 0.
DF2(jtpowop){F2PREFIP;B b;V*v;
 ARGCHK2(a,w);
 ASSERT(AT(a)&VERB,EVDOMAIN);  // u must be a verb
 A z; fdefallo(z)  // allocate normal result area
 AF f1,f2; I flag;  // derived-verb handlers; flags for the verb we build
 A h;  // A block for the power list converted to integer (initially, remaining for jtply12 only); u^:_1 (for jtinv[12]); 0 for others
 I encn;  // the encoded form of the integer power to be passed to pow12n
 if(AT(w)&VERB){
  // u^:v.  Create derived verb to handle it. The action routines are inplaceable; take ASGSAFE from u and v, inplaceability from u
  f1=f2=jtpowv12cell; h=0; flag=(FAV(a)->flag&FAV(w)->flag&VASGSAFE)+(FAV(a)->flag&(VJTFLGOK1|VJTFLGOK2));
 }else{
  // u^:n.  Check for special types.
  I n; // the power, the functions
  if(unlikely(BOX&AT(w))){A x,y;
   // Boxed v.  It could be <n or [v0`]v1`v2 or <''.
   if(!AR(w)&&(x=C(AAV(w)[0]),!AR(x)&&NUMERIC&AT(x)||1==AR(x)&&!AN(x))){  // scalar box whose contents are numeric atom or empty list   <numatom or a:
    // here for <numatom or a: .  That will be handled as an integer power with multiple results, usually
    n=AN(x)?i0(x):IMAX; RE(0);  // get power, using _ for <''
    flag=VFLAGNONE;  // No inplacing allowed for multiple-result types
    ASSERT(n!=0,EVDOMAIN);  // <0 arg not allowed: would be nothing
    f1=f2=jtpowatom12; v=FAV(a);   // use atomic-power handler
    // if u is {&n or {~, and n is <_ or <'', do the tclosure trick
    if(likely(n==IMAX)){
     if(CAMP==v->id&&(CFROM==IDD(v->fgh[0])&&(y=v->fgh[1],INT&AT(y)&&1==AR(y)))){f1=jtindexseqlim12;}  // {&b^:a: y
     else if(CTILDE==v->id&&CFROM==IDD(v->fgh[0])){f2=jtindexseqlim12;}   // x {~^:a: y
    }
    encn=((ABS(n)-1)<<POWERABSX)+(n<0?POWERANEG:0)+POWERAMULT;  // save the power, in flagged form.  count is # powers to evaluate, which is n-1
   }else R gconj(a,w);  // not <numatom or a: .  Create the derived verb for [v0`]v1`v2, return that
// to prevent recursion loop   ASSERT(self!=0,EVDOMAIN);  // If gerund returns gerund, error.  This check is removed pending further design
   // falling through for <numatom or a: to fill in the result
   h=0;  // must be 0 if unused
  }else{
   // unboxed n.
   // handle the very important case of scalar   int/boolean   n of 0/1
   if(likely(((AT(w)&~(B01+INT))|AR(w)|(BIV0(w)&~1))==0))R BIV0(w)?a:ds(CRIGHT);  //  u^:0 is like ],  u^:1 is like u   AR(w)==0 and B01|INT and BAV0=0 or 1   upper AT flags not allowed in B01/INT    overfetch possible but harmless
   // falling through for other cases (including non-B01/INT)
   if(w==ds(CUSDOT)){   // power is _.
    ASSERT(FAV(a)->valencefns[0]==jtpowv12cell,EVDOMAIN)  // enforce u is u^:v all verbs
    flag = FAV(a)->flag&VASGSAFE+VJTFLGOK2+VJTFLGOK1; n=IMIN; // u^:v^:_. inherits inplaceability from u^:v, set exponent IMIN to indic u^:v^:_.
   }else{   // normal power, not _.
    RZ(h=vib(w)); ASSERT(AN(h)!=0,EVDOMAIN); n=IAV(h)[0]; n=n==IMIN?-IMAX:n;   // hs=n coerced to integer (must not be 0); n=power (if atomic), exponent IMIN reserved for ^:_.
   }
   if(likely(!AR(w))){  // input is an atom
    // Handle the important cases: atomic _1 (inverse), 0 (nop), 1 (execute u), _ (converge), _. (dowhile)
    if(!(n&~1))R a=n?a:ds(CRIGHT);  //  the if statement: u^:0 is like ],  u^:1 is like u
    // falling through is not the if statement
    f1=f2=jtpowatom12; flag=VJTFLGOK1+VJTFLGOK2;   // init to handler for general atomic power.  All atomic-power entry points support inplacing
    h=0;    // inverse, if we can calculate it (we no longer need the list of powers)
    if(likely((n<<1)==-2)){  //  u^:_1 or u^:_
     if(n<0){  // u^:_1
      // if there are no names, calculate the monadic inverse and save it in h.  Inverse of the dyad, or the monad if there are names,
      // must wait until we get arguments.  We can't trust the inverse to be ASGSAFE
      f2=jtinv2; f1=jtinv1; if(nameless(a)){WITHMSGSOFF(if(h=inv(a)){f1=jtinvh1;flag=VJTFLGOK2+(FAV(h)->flag&VJTFLGOK1);}else{f1=jtinverr;})} // h must be valid A block for free.  If no names in w, take the inverse.  If it doesn't exist, fail the monad but keep the dyad going
     }else flag=VFLAGNONE;     // if u^:_, never allow inplacing since we are converging
     // Note: negative powers other than _1 are resolved in the action routine
    }
    encn=(ABS(n)<<POWERABSX)+(n<0?POWERANEG:0); encn=n==IMIN?((I)-1*POWERABS)+POWERADOWHILE:encn;  // save the power, in flagged form, for powatom12
   }else{f1=f2=jtply12; flag=VFLAGNONE;}  // non-atomic power: handle general case, which does not support inplacing
   // fall through to create result
  }  // end of 'u^:numeric'
 }  // end of 'u^:n'
 fdeffill(z,0,CPOWOP,VERB, f1,f2, a,w,h,flag, RMAX,RMAX,RMAX);   // Create derived verb: pass in integer powers or inverse as h
 FAV(z)->localuse.lu1.poweratom=encn;   // pass power info for powatom12, garbage for others
 RETF(z);
}
