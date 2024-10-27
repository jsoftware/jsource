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

// AR(a)<=1 and AN(w)!=0
static F2(jttclosure){A z;I an,*av,c,d,i,wn,wr,wt,*wv,*zv,*zz;
 ARGCHK2(a,w);
 wt=AT(w); wn=AN(w); wr=AR(w);
 if(B01&wt)RZ(w=cvt(INT,w)); wv=AV(w);
 av=AV(a); an=AN(a);
 RZ(z=exta(INT,1+wr,wn,20L));   // init the expanding result area
 zv=AV(z); zz=zv+AN(z);
 if(1==wn){   // just 1 result, which will be a list
  *zv++=c=*wv;  // store first selection
  NOUNROLL do{
   if(zv==zz){i=zv-AV(z); ASSERT(i<=an,EVLIMIT) RZ(z=ext(0,z)); zv=AV(z)+i; zz=AV(z)+AN(z);}  // if we have more results than items in a, there must be a cycle - quit
   d=c; if((UI)c>=(UI)an){c+=an; ASSERT((UI)c<(UI)an,EVINDEX);} *zv++=c=av[c];  // d is prev selection, c is next.  Store c
  }while(c!=d);  // stop when we get a repeated value (normal exit)
  d=(zv-AV(z))-1;
 }else{  // multiple starting points.  Each cell of result has one successor for each starting point.
  ICPY(zv,wv,wn); zv+=wn;
  I resultdiff;
  NOUNROLL do{
   if(zv==zz){i=zv-AV(z); ASSERT(i<=an*wn,EVLIMIT) RZ(z=ext(0,z)); zv=AV(z)+i; zz=AV(z)+AN(z);}  // break if there is a cycle
   resultdiff=0; DQ(wn, d=c=zv[-wn]; if((UI)c>=(UI)an){c+=an; ASSERT((UI)c<(UI)an,EVINDEX);} *zv++=c=av[c]; resultdiff|=c^d;);  // set diff if not a steady state
  }while(resultdiff);
  d=(zv-AV(z))/wn-1;
 }
 AS(z)[0]=d; AN(z)=d*wn; MCISH(1+AS(z),AS(w),wr); 
 RETF(z);
}    /* {&a^:(<_) w */

#if 0  // obsolete
static DF1(jtpowseqlim){PROLOG(0039);A x,y,z,*zv;I i,n;
 ARGCHK1(w);
 RZ(z=exta(BOX,1L,1L,20L)); zv=AAV(z); INCORP(w); *zv++=x=w;
 i=1; n=AN(z);
 NOUNROLL while(1){
  if(n==i){RZ(z=ext(0,z)); zv=i+AAV(z); n=AN(z);}
  A z0; RZ(x=df1(z0,y=x,self)); INCORP(x); *zv++=x;
  if(equ(x,y)){AN(z)=AS(z)[0]=i; break;}
  ++i;
 }
 z=jtopenforassembly(jt,z);
 EPILOG(z);
}    /* f^:(<_) w */

// u^:_ w  Bivalent, called as w,fs or a,w,fs
static DF2(jtpinf12){PROLOG(0340);A z;  // no reason to inplace, since w must be preserved for comparison, & a for reuse
 I i=0;
 I ismonad=(AT(w)>>VERBX)&1; self=ismonad?w:self;  // the call shows the dyad; if it's really a monad, w->self, leave arg in a
 A fs=FAV(self)->fgh[0]; w=ismonad?fs:w;   // for monad, fs->w
 AF f=FAV(fs)->valencefns[1-ismonad];
 while(1){
  RZ(z=CALL2(f,a,w,fs));  // call the fn, either monadic or dyadic
  A oldw=w; oldw=ismonad?a:oldw; w=ismonad?w:z; a=ismonad?z:a;  // oldw=input w to f; save result for next loop overwriting a(monad) or w(dyad)
  I isend=equ(z,oldw);  // remember if result is same as input
  if(!((isend-1)&++i&7)) {  // every so often, but always when we leave...
   JBREAK0;   // check for user interrupt, in case the function doesn't allocate memory
   RZ(z=EPILOGNORET(z));  // free up allocated blocks, but keep z.  If z is virtual it will be realized
   if(isend){RETF(z);}  // return at end
  }
    // make the new result the starting value for next loop, replacing w wherever it is
 }
}

// u^:(<n) If n negative, take inverse of u; if n infinite, go to routine that checks for no change.  Otherwise convert to u^:(i.|n) and restart
static DF1(jtpowseq){A fs,gs,x;I n=IMAX;V*sv;
 ARGCHK1(w);
 sv=FAV(self); fs=sv->fgh[0]; gs=sv->fgh[1];
 ASSERT(!AR(gs),EVRANK);
 ASSERT(BOX&AT(gs),EVDOMAIN);
 x=C(AAV(gs)[0]); if(!AR(x))RE(n=i0(vib(x)));
 if(0>n){RZ(fs=inv(fs)); n=-n;}
 if(n==IMAX||1==AR(x)&&!AN(x))R powseqlim(w,fs);
 R df1(gs,w,powop(fs,IX(n),0));
}    /* f^:(<n) w */

// u^:n w where n is nonnegative finite integer atom (but never 0 or 1, which are handled as special cases)
static DF1(jtfpown){A fs,z;AF f1;I n;V*sv;A *old;
 F1PREFIP;ARGCHK1(w);
 sv=FAV(self);
 n=AV(sv->fgh[2])[0];
 fs=sv->fgh[0]; f1=FAV(fs)->valencefns[0];
 z=w; 
 old=jt->tnextpushp; 
 DQ(n, JBREAK0; RZ(z=CALL1IP(f1,z,fs)); z=gc(z,old);); // could force inplace after the first?
 RETF(z);
// }
}
#else
// [a] u^:atom w and [a] u^:<(atom or '') w, bivalent.
static DF2(jtpowatom12){F1PREFIP;
 I wt=AT(w); w=wt&VERB?a:w; a=wt&VERB?0:a;  // w arg will be power result; a=0 if monad
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
// obsolete  I allowinplace=(FAV(fs)->flag>>(VJTFLGOK1X+!!a-JTINPLACEW))&~SGNTO0(poweratom);  // suppress all inplacing if verb doesn't support it, or this is convergence
// obsolete  jtinplace=(J)((I)jtinplace&~(JTINPLACEA|(poweratom&(IMIN+POWERAMULT)?0:JTINPLACEW)|(a==w)));  // never inplace a; pass inplacing of w only if !multiple & !infinite, and not same as a
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
 }
 if(zz!=0){AS(zz)[0]=AN(zz); RZ(w=jtopenforassembly(jt,zz));}   // if multiple, assemble the results
 EPILOG(w)  // return the result
}

#endif

static DF1(jtindexseqlim1){A fs;
 ARGCHK1(w); 
 fs=FAV(self)->fgh[0];  // {&x
 R AN(w)&&AT(w)&B01+INT?tclosure(FAV(fs)->fgh[1],w):jtpowatom12(jt,w,self,self);
}    /* {&x^:(<_) w */

static DF2(jtindexseqlim2){
 ARGCHK2(a,w);
 R AR(a)<=1&&AT(a)&INT&&AN(w)&&AT(w)&B01+INT?tclosure(a,w):jtpowatom12(jt,a,w,self);
}    /* a {~^:(<_) w */

// general u^:n w where n is any integer array or finite atom.  If atom, it will be negative
static DF1(jtply1){A fs=FAV(self)->fgh[0]; AF f1=FAV(fs)->valencefns[0]; PROLOG(0040);A zz=0;
#define ZZWILLBEOPENEDNEVER 1  // can't honor willbeopened because the results are recycled as inputs
#define ZZPOPNEVER 1  // can't pop the inputs - 
#define ZZDECL
#include "result.h"
 I state=ZZFLAGINITSTATE;  // flags for result.h
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
 ZZPARMS(1,np,1)
#define ZZINSTALLFRAME(optr) *optr++=np;
 // loop for each result power:
 I neededpow;  // the next power we are looking for
 A invfs; if(pv[0]<0)RZ(invfs=inv(fs));   // if there is an inverse, look it up outside the loop so we don't free it
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
      fs=invfs; f1=FAV(fs)->valencefns[0];  // switch over to the inverse function
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
   A oldz=z; RZ(z=CALL1(f1,z,fs));  // run the verb
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

static DF1(jtinv1){A fs=FAV(self)->fgh[0]; F1PREFIP;A z; ARGCHK1(w);A i; RZ(i=inv((fs))); WITHEFORMATDEFERRED(z=(FAV(i)->valencefns[0])(FAV(i)->flag&VJTFLGOK1?jtinplace:jt,w,i);) RETF(z);}  // was invrecur(fix(fs))
  // we defer eformat till the caller so that the user doesn't see the description of the inverse, which might be unrecognizable
static DF1(jtinvh1){A hs=FAV(self)->fgh[2]; F1PREFIP;A z; ARGCHK1(w); z=(FAV(hs)->valencefns[0])(jtinplace,w,hs); RETF(z);}
static DF2(jtinv2){A fs=FAV(self)->fgh[0]; A z; ARGCHK2(a,w); WITHEFORMATDEFERRED(df1(z,w,inv(amp(a,fs)));) STACKCHKOFL RETF(z);}  // the CHKOFL is to avoid tail recursion, which prevents a recursion loop from being broken
static DF1(jtinverr){F1PREFIP;ASSERT(0,EVDOMAIN);}  // used for uninvertible monads

// old static CS2(jtply2, df1(z,w,powop(amp(a,fs),gs,0)),0107)  // dyad adds x to make x&u, and then reinterpret the compound.  We could interpret u differently now that it has been changed (x {~^:a: y)
DF2(jtply2){PROLOG(107);A fs=FAV(self)->fgh[0]; A gs=FAV(self)->fgh[1]; A z, zz; z=(df1(zz,w,powop(amp(a,fs),gs,0))); EPILOG(z);}  // scaf remove


static DF1(jtpowg1){A z,h=FAV(self)->fgh[2]; R df1(z,  w,C(AAV(h)[0]));}  // scaf bivalent
static DF2(jtpowg2){A z,h=FAV(self)->fgh[2]; R df2(z,a,w,C(AAV(h)[0]));}

// When u^:v is encountered, we replace it with a verb that comes to one of these.
// This creates a verb, jtpowxx, which calls jtdf1 within a PROLOG/EPILOG pair, after creating several names:
// sv->self data; fs=sv->fgh[0] (the A block for the f operand); f1=f1 in sv->fgh[0] (0 if sv->fgh[0]==0); f2=f2 in sv->fgh[0] (0 if sv->fgh[0]==0);
//                gs=sv->fgh[1] (the A block for the g operand); g1=f1 in sv->fgh[1] (0 if sv->fgh[1]==0); g2=f2 in sv->fgh[1] (0 if sv->fgh[1]==0)
// Here, f1 is the original u and g1 is the original v
// We call g1 (=original v), passing in y (and gs as self).  This returns v y
// We then call powop(original u,result of v y), which is the VN case for u^:(v y) and creates a derived verb to perform that function 
// Finally df1 treats the powop result as self, calling self/powop->valencefns[0] (the appropriate power case based on v y)
//   with the y arg as the w operand (and self/powop included to provide access to the original u)
// We allow v to create a gerund, but we do not allow a gerund to create a gerund.

// We catch the special cases 0  & 1 here, mostly for branch-prediction purposes.  All results of g1/g2 will be nouns, while
// most instances of u^:v (run through powop) have v as verb

#if 0  // obsolete
// here for u^:v y
static DF1(jtpowv1cell){F1PREFIP;A z;PROLOG(0108);
A u; A v; RZ(u=CALL1(g1,  w,gs));  /* execute v */
if(!AR(u) && (v=vib(u)) && !(IAV(v)[0]&~1)){z=IAV(v)[0]?(FAV(fs)->valencefns[0])(FAV(fs)->flag&VJTFLGOK1?jtinplace:jt,w,fs,fs):w;}
else{RESETERR; RZ(u = powop(fs,u,(A)1)); 
z=(FAV(u)->valencefns[0])(FAV(u)->flag&VJTFLGOK1?jtinplace:jt,w,u,u);}
EPILOG(z);
}
// here for x u^:v y 
static DF2(jtpowv2cell){F2PREFIP;A z;PROLOG(0109);
A u; A v; RZ(u=CALL2(g2,a,w,gs));  /* execute v */
if(!AR(u) && (v=vib(u)) && !(IAV(v)[0]&~1)){z=IAV(v)[0]?(FAV(fs)->valencefns[1])(FAV(fs)->flag&VJTFLGOK2?jtinplace:jt,a,w,fs):w;}
else{RESETERR; RZ(u = powop(fs,u,(A)1)); 
z=(FAV(u)->valencefns[1])(FAV(u)->flag&VJTFLGOK2?jtinplace:jt,a,w,u);}
EPILOG(z);}

// here for x u@:]^:v y and x u@]^:v y
static DF2(jtpowv2acell){F2PREFIP;A z;PROLOG(0110);
jtinplace=(J)((I)jtinplace&~JTINPLACEA); /* monads always have IP2 clear */
A u; A v; fs=FAV(fs)->fgh[0]; RZ(u=CALL2(g2,a,w,gs));  /* execute v */
if(!AR(u) && (v=vib(u)) && !(IAV(v)[0]&~1)){z=IAV(v)[0]?(FAV(fs)->valencefns[0])(FAV(fs)->flag&VJTFLGOK1?jtinplace:jt,w,fs,fs):w;}
else{RESETERR; RZ(u = powop(fs,u,(A)1)); 
z=(FAV(u)->valencefns[0])(FAV(u)->flag&VJTFLGOK1?jtinplace:jt,w,u,u);}
EPILOG(z);}

// here for x u@:]^:v y and x u@]^:v y
static DF2(jtpowv2acell){F2PREFIP;A z;PROLOG(0110);
 A u; I u0; A gs=FAV(self)->fgh[1]; A fs=FAV(FAV(self)->fgh[0])->fgh[0]; AF uf=FAV(fs)->valencefns[0]; // fetch uself, which we always need, and uf, which we will need in the fast path, from the u@]
 J sjtip=(J)((I)jtinplace&~JTINPLACEA); jtinplace=(J)((I)jtinplace&~JTINPLACEW); // monads always have IP2 clear; v must not inplace y
 RZ(u=CALL2IP(FAV()->valencefns[1],a,w,gs));  // execute v, inplace on x only
 if(likely(!AR(u)) && likely(ISDENSETYPE(AT(u),INT+B01)) && likely(!((u0=BIV0(u))&~1))){  // v result is 0/1
  if(u0){jtinplace=FAV(u)->flag&VJTFLGOK1?sjtip:jt; z=CALL1IP(uf,w,fs);  // if u result is atomic INT/B01 0/1, execute forthwith, inplace if possible (y only)
  }else{z=w;}
 }else{  // not a simple if statement
  RZ(u=powop(fs,u,(A)1)); jtinplace=FAV(u)->flag&VJTFLGOK1?sjtip:jt; // create u^:n form of powop (without @]); can it inplace?
  z=CALL1IP(FAV(u)->valencefns[0],w,u);  // execute the power, inplace
 }
 EPILOG(z);
}

#else
// [x] u^:v y, fast when result of v is 0 or 1 (=if statement).  jtflagging is that required by u.  JTDOWHILE can be set to indicate that this is called from ^:_. in which case we return (A)1 if u was 0
static DF2(jtpowv12cell){F2PREFIP;A z;PROLOG(0110);
 w=AT(w)&VERB?0:w;  // w is 0 for monad
 A u,uc; I u0; A gs=FAV(self)->fgh[1]; A fs=FAV(self)->fgh[0]; AF uf=FAV(fs)->valencefns[!!w]; // fetch uself, which we always need, and uf, which we will need in the fast path
 RZ(u=CALL12(w,FAV(gs)->valencefns[!!w],a,w,gs));  // execute v, not inplace
// obsolete  if(likely(!AR(u)) && likely(ISDENSETYPE(AT(u),INT+B01)) && likely(!((u0=BIV0(u))&~1))){  // v result is 0/1
 if(likely(((AT(u)&~(B01+INT))|AR(u)|((u0=BIV0(u))&~1))==0)||(AR(u)==0&&AT(u)&NUMERIC&&(uc=cvt(INT,u))!=0&&!((u0=IAV(uc)[0])&~1))){  // v result is atomic bool/int 0/1 (if statement)  overfetch possible but harmless
  if(u0){jtinplace=(J)((I)jtinplace&~(a==w?JTDOWHILE+JTINPLACEA+JTINPLACEW:JTDOWHILE)); z=CALL12IP(w,uf,a,w,fs);}   // v result is atomic INT/B01 1: execute u, inplace if possible - but suppress the DOWHILE flag, and all inplacing if a=w
  else{z=w?w:a; if((I)jtinplace&JTDOWHILE)R (A)1;}  // v result is atomic INT/B01 0: bypass.  If ^:_. return 1 to indicate u returned 0 - skips EPILOG but powatom12 will soon do one
 }else{ASSERT(!((I)jtinplace&JTDOWHILE),EVDOMAIN) RZ(u=powop(fs,u,(A)1));  // not a simple if statement: fail if called from ^:_.; create u^:n form of powop;
  jtinplace=FAV(u)->flag&(VJTFLGOK1<<(!!w))?jtinplace:jt;   // u might have been a gerund so there is no telling what the inplaceability of the new u is; refresh it
  z=CALL12IP(w,FAV(u)->valencefns[!!w],a,w,u);  // execute the power, inplace
 }
 EPILOG(z);
}

// obsolete jtinplace=FAV(fs)->flag&(VJTFLGOK1<<(!!w))?jtinplace:jt; 
// obsolete 
#endif


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
 AF f1,f2;   // derived-verb handler
 I flag=VFLAGNONE;  // flags for the verb we build
 A h;  // A block for the power list converted to integer (initially, remaining for jtply1 only); u^:_1 (for jtinv[12]); 0 for others
 I encn;  // the encoded form of the integer power to be passed to pow12n
 if(AT(w)&VERB){
  // u^:v.  Create derived verb to handle it.
// obsolete   v=FAV(a); b=((v->id&~1)==CATCO)&&ID(v->fgh[1])==CRIGHT;  // detect u@]^:v  (or @:)
  v=FAV(a);
// obsolete  b=v->valencefns[1]==jtonright12;   // detect u@]^:v  (or @:)
  // The action routines are inplaceable; take ASGSAFE from u and v, inplaceability from u
  f1=f2=jtpowv12cell; h=0; flag=(v->flag&FAV(w)->flag&VASGSAFE)+(v->flag&(VJTFLGOK1|VJTFLGOK2));
// obsolete   fdeffill(z,0L,CPOWOP,VERB,jtpowv12cell,jtpowv12cell,a,w,0L,(v->flag&FAV(w)->flag&VASGSAFE)+(v->flag&(VJTFLGOK1|VJTFLGOK2)), RMAX,RMAX,RMAX)
// obsolete   RETF(z);
 }else{
  // u^:n.  Check for special types.
  I n; // the power, the functions
  if(unlikely(BOX&AT(w))){A x,y;
   // Boxed v.  It could be <n or [v0`]v1`v2 or <''.
   if(!AR(w)&&(x=C(AAV(w)[0]),!AR(x)&&NUMERIC&AT(x)||1==AR(x)&&!AN(x))){  // scalar box whose contents are numeric atom or empty list   <numatom or a:
    // here for <numatom or a: .  That will be handled as an integer power with multiple results, usually
    n=AN(x)?i0(x):IMAX; RE(0);  // get power, using _ for <''
    ASSERT(n!=0,EVDOMAIN);  // <0 arg not allowed: would be nothing
// obsolete    f1=jtpowseq; f2=jtply2; v=FAV(a);
    f1=f2=jtpowatom12; v=FAV(a);   // use atomic-power handler
    // if u is {&n or {~, and n is <_ or <'', do the tclosure trick
// obsolete    if((!AN(x)||FL&AT(x)&&inf==DAV(x)[0])){
    if(n==IMAX){
     if(CAMP==v->id&&(CFROM==IDD(v->fgh[0])&&(y=v->fgh[1],INT&AT(y)&&1==AR(y)))){f1=jtindexseqlim1;}  // {&b^:a: y
     else if(CTILDE==v->id&&CFROM==IDD(v->fgh[0])){f2=jtindexseqlim2;}   // x {~^:a: y
    }
// obsolete     fdeffill(z,0L,CPOWOP,VERB,f1,f2,a,w,0L,VFLAGNONE, RMAX,RMAX,RMAX)  // never allow inplacing when there are multiple results
    encn = FAV(z)->localuse.lu1.poweratom=((ABS(n)-1)<<POWERABSX)+(n<0?POWERANEG:0)+POWERAMULT;  // save the power, in flagged form.  count is # powers to evaluate, which is n-1
// obsolete     RETF(z);
   }else R gconj(a,w,CPOWOP);  // not <numatom or a: .  Create the derived verb for [v0`]v1`v2, return that
//    ASSERT(self!=0,EVDOMAIN);  // If gerund returns gerund, error.  This check is removed pending further design
   // falling through for boxed numeric to fill in the result
   h=0;  // must be 0 if unused
  }else{
   // unboxed n.
   // handle the very important case of scalar   int/boolean   n of 0/1
 // obsolete  if(likely(((-(AT(w)&B01+INT))&((AR(w)|((UI)BIV0(w)>>1))-1))<0))R a=BIV0(w)?a:ds(CRIGHT);  //  u^:0 is like ],  u^:1 is like u   AR(w)==0 and B01|INT and BAV0=0 or 1
   if(likely(((AT(w)&~(B01+INT))|AR(w)|(BIV0(w)&~1))==0))R a=BIV0(w)?a:ds(CRIGHT);  //  u^:0 is like ],  u^:1 is like u   AR(w)==0 and B01|INT and BAV0=0 or 1   upper AT flags not allowed in B01/INT    overfetch possible but harmless
   if(w==ds(CUSDOT)){   // power is _.
    ASSERT(FAV(a)->valencefns[0]==jtpowv12cell,EVDOMAIN)  // enforce u is u^:v all verbs
    n=IMIN;  // set _. value in n
    flag = FAV(a)->flag&VASGSAFE+VJTFLGOK2+VJTFLGOK1;  // u^:v^:_. inherits inplaceability from u^:v
   }else{   // normal power, not _.
    RZ(h=vib(w));   // hs=n coerced to integer
    ASSERT(AN(h)!=0,EVDOMAIN);  // empty power is error
    n=IAV(h)[0];  // the exponent n (if atomic)
    n=n==IMIN?-IMAX:n;  // exponent IMIN reserved for ^:_.
   }
   if(likely(!AR(w))){  // input is an atom
    // Handle the important cases: atomic _1 (inverse), 0 (nop), 1 (execute u), _ (converge), _. (dowhile)
    if(!(n&~1))R a=n?a:ds(CRIGHT);  //  the if statement: u^:0 is like ],  u^:1 is like u 
    f1=f2=jtpowatom12;   // init to handler for atomic power
    h=0;    // inverse, if we can calculate it (we no longer need the list of powers)
    if((n<<1)==-2){  //  u^:_1 or u^:_
     if(n<0){  // u^:_1
      // if there are no names, calculate the monadic inverse and save it in h.  Inverse of the dyad, or the monad if there are names,
      // must wait until we get arguments
      f2=jtinv2; f1=jtinv1; if(nameless(a)){WITHMSGSOFF(if(h=inv(a)){f1=jtinvh1;}else{f1=jtinverr;})} // h must be valid for free.  If no names in w, take the inverse.  If it doesn't exist, fail the monad but keep the dyad going
      flag = (FAV(a)->flag&VASGSAFE) + (h?FAV(h)->flag&VJTFLGOK1:VJTFLGOK1);  // inv1 inplaces and calculates ip for next step; invh has ip from inverse (monad only)
// obsolete else{  // u^:_
// obsolete     f1=f2=jtpinf12;
// obsolete     flag=VFLAGNONE;
// obsolete      fdeffill(z,0,CPOWOP,VERB,jtpinf12,jtpinf12,a,w,0,VFLAGNONE,RMAX,RMAX,RMAX);
     }
     // Note: negative powers other than _1 are resolved in the action routine
     // if u^:_, never allow inplacing since we are converging
// obsolete    fdeffill(z,0,CPOWOP,VERB,f1,f2,a,w,h,flag,RMAX,RMAX,RMAX);
// obsolete    FAV(z)->localuse.lu1.poweratom=(IMAX&(ABS(n)<<POWERABSX))+(n<0?POWERANEG:0);  // save the power, in flagged form
    }
// obsolete     fdeffill(z,0,CPOWOP,VERB,f1,f2,a,w,h,flag,RMAX,RMAX,RMAX);
    encn=(ABS(n)<<POWERABSX)+(n<0?POWERANEG:0); encn=n==IMIN?((I)-1*POWERABS)+POWERADOWHILE:encn;  // save the power, in flagged form
// obsolete     FAV(z)->localuse.lu1.poweratom=encn;
// obsolete     RETF(z);
// obsolete   if(IAV(hs)[0]>=0){f1=jtfpown; flag=FAV(a)->flag&VJTFLGOK1;}  // if nonneg atom, go to special routine for that, which supports inplace
   }else{f1=jtply1; f2=jtply2;}  // non-atomic power: handle general case
   // fall through to create result
  }  // end of 'u^:numeric'
 }  // end of 'u^:n'
// obsolete  I m=AN(hs); // m=#atoms of n; n=1st atom; r=n has rank>0
// obsolete  fdeffill(z,0,CPOWOP,VERB, f1,jtply2, a,w,hs,flag, RMAX,RMAX,RMAX);   // Create derived verb: pass in integer powers as h
 fdeffill(z,0,CPOWOP,VERB, f1,f2, a,w,h,flag, RMAX,RMAX,RMAX);   // Create derived verb: pass in integer powers as h
 FAV(z)->localuse.lu1.poweratom=encn;   // pass power info for powatom12, garbage for others
 RETF(z);
}
