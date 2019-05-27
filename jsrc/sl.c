/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Locales                                                   */

#include "j.h"

// Interfaces for numbered locales
#if 0
// Initialize the numbered-locale system
static A jtinitnl(J jt){A q;
 RZ(q=apvwr(40,-1L,0L));    jt->stnum=q;  //  start with 40 locales
 GATV0(q,INT,40,1);        jt->stptr=q; memset(AV(q),C0,40*SZI);
 R q;  // return no error
}

// Get the locale number to use for the next numbered locale.  (0 if error, with error code set) The locale need not be installed, if an error intervenes
static I jtgetnl(J jt){
 ASSERT(0<=jt->stmax,EVLOCALE);
 I m=AN(jt->stnum);
 // Extend in-use locales list if needed
 if(m<=jt->stused){
  A x=ext(1,jt->stnum); A y=ext(1,jt->stptr); RZ(x&&y); jt->stnum=x; jt->stptr=y;
  I *nv=m+AV(jt->stnum); A *pv=m+AAV(jt->stptr); DO(AN(x)-m, *nv++=-1; *pv++=0;); 
 }
 R jt->stmax;
}

// Install locale l in the numbered-locale table, at the number returned by the previous jtgetnl.  No error is possible
static void jtinstallnl(J jt, A l){
 ++jt->stused;
 A *pv=AAV(jt->stptr);
 DO(AN(jt->stnum), if(!pv[i]){ras(l); pv[i]=l; *(i+AV(jt->stnum))=jt->stmax; break;});
 ++jt->stmax;
}

// return the address of the locale block for number n, or 0 if not found
A jtfindnl(J jt, I n){
  I i, iend, *ibgn; for(i=0, iend=AN(jt->stnum), ibgn=IAV(jt->stnum); i<iend; ++i)if(ibgn[i]==n)R AAV(jt->stptr)[i];
  R 0;
}

// delete the locale numbered n, if it exists
static void jterasenl(J jt, I n){
  I i, iend, *ibgn; for(i=0, iend=AN(jt->stnum), ibgn=IAV(jt->stnum); i<iend; ++i)if(ibgn[i]==n){AAV(jt->stptr)[i]=0; AV(jt->stnum)[i]=-1; --jt->stused; break;};
 }

// return list of active numbered locales, using namelist mask
static A jtactivenl(J jt){A y;I n=0;C s[20];
  GATV0(y,BOX,jt->stused,1); A *yv=AAV(y); A *pv=AAV(jt->stptr); I *nv=AV(jt->stnum);
  DO(AN(jt->stptr), if(pv[i]){sprintf(s,FMTI,nv[i]); 
      if(jt->workareas.namelist.nla[*s]){RZ(yv[n++]=cstr(s)); if(n==jt->stused)break;}});
  R take(sc(n),y);
}

// iterator support
I jtcountnl(J jt) { R AN(jt->stnum); }  // number of locales to reference by index
A jtindexnl(J jt,I n) { R AAV(jt->stptr)[n]; }  // the locale address, or 0 if none

#elif 0
#define DELAYBEFOREREUSE 5000  // min number of locales to have before we start reusing
// Initialize the numbered-locale system.  Called during initialization, so no need for ras()
static A jtinitnl(J jt){A q;
 GATV0(q,INT,1,1);
 jt->stnum=q;  // save address of block
 jt->numloctbl=IAV(q);  // address of locale vector
 jt->numlocsize=1;  // length of locale vector
 jt->numlocdelqh=IAV(q);  // pointer to first locale number to allocate
 jt->numlocdelqn=1;  // init 1 value to allocate
 jt->numlocdelqt=IAV(q);  // pointer to end of queue (queue is never empty - it starts at 1 and never allocates unless there are DELAYBEFOREREUSE empties
 *jt->numlocdelqh=(I)IAV(q);  // Make the sole locale the end-of-chain by looping its chain to itself.  This is never needed as end-of-chain but it does ensure relocation
 R q;  // return no error
}

// Get the locale number to use for the next numbered locale.  (0 if error, with error code set)
// This call just gets the number to use, it does not allocate the number.  The locale will be installed later, unless an error intervenes
static I jtgetnl(J jt){
 if(jt->numlocdelqn<DELAYBEFOREREUSE){
  // There are not enough blocks in the delq to guarantee that the next free will lie fallow long enough.  Increase the allocation
  A x; UI oldsize=jt->numlocsize; do{RZ(x=ext(1,jt->stnum)) jt->stnum=x;}while(jt->numlocdelqn+AN(x)-oldsize<DELAYBEFOREREUSE);   // extend & save the allocation.  ext handles the usecount.  Stop when we have enough
  jt->numlocdelqn += AN(x)-oldsize;  // add the new allocation into the size of the delq
  I relodist=(I)IAV(x)-(I)jt->numloctbl;  // relocation factor
  I oldbase=(I)jt->numloctbl; I *reloptr=IAV(x);  // treat the locale pointer as integers for arithmetic here
  DQ(oldsize, if((UI)(*reloptr-oldbase)<(oldsize<<LGSZI))*reloptr+=relodist; ++reloptr;)  // if the pointer points to within the old block, relocate it to the new.  reloptr ends pointeing to the first new block
  jt->numlocdelqh=(I*)((I)jt->numlocdelqh+relodist); jt->numlocdelqt=(I*)((I)jt->numlocdelqt+relodist);  // relocate delq head/tail
  // we could make the new block the head of the alloq, but then block 1 would go out before block 0 and Chris would complain.  So we chain them off the tail
  *jt->numlocdelqt=(I)reloptr; // chain the upcoming blocks at end of queue
  DQ(AN(x)-oldsize-1, *reloptr=(I)(reloptr+1); reloptr=reloptr+1;)  // chain each block to the next, except for the last.  end pointing to the last
  *reloptr=(I)reloptr; jt->numlocdelqt=reloptr; // chain added blocks as allocable, in ascending order.  The last one loops to self to indicate end, and becomes the end of the delq
  jt->numloctbl=IAV(x); jt->numlocsize=(UI4)AN(x); // set address and length of new table
 }
 R jt->numlocdelqh-jt->numloctbl;  // return index of next allocation
}

// Install locale l in the numbered-locale table, at the number returned by the previous jtgetnl.  No error is possible
static void jtinstallnl(J jt, A l){
 I *nextallo=(I*)*jt->numlocdelqh;  // save new head of the allo chain
 ras(l); *jt->numlocdelqh=(I)l;  // protect l and store it as the new locale for its index
 jt->numlocdelqh=nextallo;  // set next block as new head of chain
 // we can't be allocating the end-of-chain unless DELAYBEFOREREUSE is 1
 --jt->numlocdelqn;  // reduce number-of-items in delq by the 1 we have just released
}

// return the address of the locale block for number n, or 0 if not found
A jtfindnl(J jt, I n){A z;
 findnlz(n,z); R z;
}

// delete the locale numbered n, if it exists
static void jterasenl(J jt, I n){
 if(jtfindnl(jt,n)){  // if locale exists
  I newblock=(I)&jt->numloctbl[n];
  // chain the new block off the old tail
  *jt->numlocdelqt=newblock;
  // set end-of-chain (loop to self) in the new block
  jt->numloctbl[n]=newblock;
  // set the new block as new tail
  jt->numlocdelqt=(I*)newblock;
  // add 1 to the size of the delq
  ++jt->numlocdelqn;
 }
}

// return list of active numbered locales, using namelist mask
static A jtactivenl(J jt){A y;
 GATV0(y,INT,AN(jt->stnum),1); I *yv=IAV(y);   // allocate place to hold numbers of active locales
 I nloc=0; DO(AN(jt->stnum), if(jtfindnl(jt,i)){yv[nloc]=i; ++nloc;})
 R every(take(sc(nloc),y),0,jtthorn1);  // ".&.> nloc{.y
}

// iterator support
I jtcountnl(J jt) { R jt->numlocsize; }  // number of locales to reference by index
A jtindexnl(J jt,I n) { R findnl(n); }  // the locale address, or 0 if none
#else
// Hashed version, without locale reuse
#if BW==64
#define HASHSLOT(x,tsize) (((UI)((UI4)(x)*(UI4)2654435761U)*(UI)(tsize))>>32)
#else
#define HASHSLOT(x,tsize) (((UI4)(x)*(UI4)2654435761U)%(UI4)(tsize))
#endif
// Initialize the numbered-locale system.  Called during initialization, so no need for ras()
static A jtinitnl(J jt){A q;
 I s; FULLHASHSIZE(5*1,INTSIZE,0,0,s);  // at least 5 slots, so we always have at least 2 empties
 GATV0(q,INT,s,0); memset(IAV(q),0,s*SZI);  // allocate hashtable and clear to 0
 jt->stnum=q;  // save address of block
 jt->stmax=0;  // set next number to allocate
 jt->stused=0;  // set number in use
 jt->sttsize=s;  // set size of table
 R q;  // return no error
}

// Get the locale number to use for the next numbered locale.  (0 if error, with error code set)
// This call just gets the number to use, it does not allocate the number.  The locale will be installed later, unless an error intervenes
static I jtgetnl(J jt){
 // If the table is too close to full, reallocate it & copy.
 // We let the table get up to half full, figuring that on average it will be somewhat less
 if(2*jt->stused>jt->sttsize){
  // Allocate a new block.  Don't use ext because we have to keep the old one for rehashing
  I s; FULLHASHSIZE(2*jt->stused+2,INTSIZE,0,0,s);
  A new; GATV0(new,INT,s,0); memset(IAV(new),0,s*SZI);  // allocate hashtable and clear to 0
  // rehash the old table into the new
  I i; for(i=0;i<jt->sttsize;++i){
   A st; if(st=(A)IAV(jt->stnum)[i]){  // if there is a value hashed...
    I probe=HASHSLOT(NAV(LOCNAME(st))->bucketx,s);  // start of search.  Look backward, wrapping around, until we find an empty.  We never have duplicates
    while(IAV(new)[probe]){if(--probe<0)probe=AN(new)-1;}  // find empty slot
    IAV(new)[probe]=(I)st;  // install in new hashtable
   }
  }
  fa(jt->stnum); ras(new); jt->stnum=new; jt->sttsize=AN(new); // install the new table, release the old
 }
 R jt->stmax;  // return index of next allocation
}

// Install locale l in the numbered-locale table, at the number returned by the previous jtgetnl.  No error is possible
static void jtinstallnl(J jt, A l){
 ras(l);  // protect new value in table
 I probe=HASHSLOT(jt->stmax,jt->sttsize);  // start of search.  Look backward, wrapping around, until we find an empty.  We never have duplicates
 while(IAV0(jt->stnum)[probe]){if(--probe<0)probe=jt->sttsize-1;}  // find empty slot
 IAV0(jt->stnum)[probe]=(I)l;  // put new locale in the empty slot
 ++jt->stmax;  // increment next-locale ticket
 ++jt->stused;  // increment number of locales outstanding
}

// return the address of the locale block for number n, or 0 if not found
A jtfindnl(J jt, I n){
 I probe=HASHSLOT(n,jt->sttsize);  // start of search.  Look backward, wrapping around, until we find match or an empty.
 while(IAV0(jt->stnum)[probe]){if(NAV(LOCNAME((A)IAV0(jt->stnum)[probe]))->bucketx==n)R (A)IAV0(jt->stnum)[probe]; if(--probe<0)probe=jt->sttsize-1;}  // return if locale match; wrap around at beginning of block
 R 0;  // if no match, return failure
}

// delete the locale numbered n, if it exists
static void jterasenl(J jt, I n){
 I probe=HASHSLOT(n,jt->sttsize);  // start of search.  Look backward, wrapping around, until we find a match or an empty.
 while(IAV0(jt->stnum)[probe]){if(NAV(LOCNAME((A)IAV0(jt->stnum)[probe]))->bucketx==n)break; if(--probe<0)probe=jt->sttsize-1;}  // wrap around at beginning of block
 // We have found the match, or are at an empty if no match.  Either way, mark the location as empty and scan forward to find the next empty,
 // moving back blocks that might have hashed into the newly vacated spot
 if(IAV0(jt->stnum)[probe])--jt->stused;  // if we found something to delete, decrement # locales outstanding
 while(1){  // probe points to either the original deletion point or a value that was just copied to an earlier position.  Either way it gets deleted
  IAV0(jt->stnum)[probe]=0;  // delete the now-invalid or -moved location
  I lastdel=probe;    // remember where the hole is
  I probehash;   // will hold original hash of probe
  do{
   if(--probe<0)probe=jt->sttsize-1;  // back up to next location to inspect
   if(!IAV0(jt->stnum)[probe])R;  // if we hit another hole, there can be no more value that need copying, we're done  *** RETURN POINT ***
   probehash=HASHSLOT(NAV(LOCNAME((A)IAV0(jt->stnum)[probe]))->bucketx,jt->sttsize);  // see where the probed cell would like to hash
    // If we are not allowed to move the new probe into the hole, because its hash is after the probe position but before-or-equal the hole,
    // we leave it in place and continue looking at the next position.  This test must be performed cyclically, because the probe may have wrapped around 0
  }while((probe<=probehash&&probehash<lastdel)||(probe>lastdel&&(probe<=probehash||probehash<lastdel)));  // first half is normal, second if probe wrapped around
  // here lastdel is the hole, and probe is a slot that hashed somewhere before lastdel.  We can safely move the probe to cover the hole.
  // This creates a new hole at probe, which we loop back to clear & then try to fill
  IAV0(jt->stnum)[lastdel]=IAV0(jt->stnum)[probe];  // move the hole forward
 }
}

// return list of active numbered locales, using namelist mask
static A jtactivenl(J jt){A y;
 GATV0(y,INT,jt->sttsize,1); I *yv=IAV(y);   // allocate place to hold numbers of active locales
 I nloc=0; DO(jt->sttsize, if(IAV0(jt->stnum)[i]){yv[nloc]=NAV(LOCNAME((A)IAV0(jt->stnum)[i]))->bucketx; ++nloc;})
 R every(take(sc(nloc),y),0,jtthorn1);  // ".&.> nloc{.y
}

// iterator support.  countnl returns a number of iterations.  indexnl returns the A block (or 0) for 
I jtcountnl(J jt) { R jt->sttsize; }  // number of locales to reference by index
A jtindexnl(J jt,I n) { R (A)IAV0(jt->stnum)[n]; }  // the locale address, or 0 if none

#endif

// Create symbol table: k is 0 for named, 1 for numbered, 2 for local; p is the number of hash entries including SYMLINFOSIZE;
// n is length of name (or locale# to allocate, for numbered locales), u->name
// Result is SYMB type for the symbol table.  For global tables only, ras() has been executed
// on the result and on the name and path
// For named/numbered types, SYMLINFO (hash chain #0) is filled in to point to the name and path
//   the name is an A type holding an NM, which has hash filled in, and, for numbered locales, the bucketx filled in with the locale number
A jtstcreate(J jt,C k,I p,I n,C*u){A g,x,xx;C s[20];L*v;
 GATV0(g,SYMB,(p+1)&-2,0);   // have odd number of hashchains, excluding LINFO
 // Allocate a symbol for the locale info, install in special hashchain 0.  Set flag; set sn to the symindex at time of allocation
 // (it is queried by 18!:31)
 // The allocation clears all the hash chain bases, including the one used for SYMLINFO
 RZ(v=symnew(&LXAV0(g)[SYMLINFO],0)); v->flag|=LINFO; v->sn=(US)jt->symindex++;   // allocate at head of chain
 switch(k){
  case 0:  /* named    locale */
   RZ(x=nfs(n,u));  // this fills in the hash for the name
   // Install name and path.  Path is 'z' except in z locale itself, which has empty path
   RZ(ras(x)); LOCNAME(g)=x; xx=1==n&&'z'==*u?vec(BOX,0L,0L):zpath; ras(xx); LOCPATH(g) = xx;   // ras() is never VIRTUAL
   // Assign this name in the locales symbol table to point to the allocated SYMB block
   // This does ras() on g
   symbis(x,g,jt->stloc);
   break;
  case 1:  /* numbered locale */
   sprintf(s,FMTI,n); RZ(x=nfs(strlen(s),s)); NAV(x)->bucketx=n; // this fills in the hash for the name; we save locale# if numeric
   RZ(ras(x)); LOCNAME(g)=x; ras(zpath); LOCPATH(g)=zpath;  // ras() is never virtual
   // Put this locale into the in-use list at an empty location.  ras(g) at that time
   jtinstallnl(jt, g);  // put the locale into the numbered list at the value most recently returned (which must be n)
   break;
  case 2:  /* local symbol table */
   // Don't invalidate ACV lookups, since the local symbol table is not in any path
   ;
 }
 R g;
}    /* create locale, named (0==k) or numbered (1==k) */

B jtsymbinit(J jt){A q;
 jt->locsize[0]=3;  /* default hash table size for named    locales */
 jt->locsize[1]=2;  /* default hash table size for numbered locales */
 RZ(symext(0));     /* initialize symbol pool                       */
 I p; FULLHASHSIZE(400,SYMBSIZE,1,SYMLINFOSIZE,p);
 GATV0(q,SYMB,p,0); jt->stloc=q;  // alloc space, clear hashchains.  No name/val for stloc
 jtinitnl(jt);  // init numbered locales
 FULLHASHSIZE(1LL<<10,SYMBSIZE,1,SYMLINFOSIZE,p);  // about 2^11 chains
 RZ(jt->global=stcreate(0,p,sizeof(jt->baselocale),jt->baselocale));
 FULLHASHSIZE(1LL<<12,SYMBSIZE,1,SYMLINFOSIZE,p);  // about 2^13 chains
 RZ(           stcreate(0,p,1L,"z"   ));
 R 1;
}


F1(jtlocsizeq){I*v; ASSERTMTV(w); v=jt->locsize; R v2(v[0],v[1]);}
     /* 9!:38 default locale size query */

F1(jtlocsizes){I p,q,*v;
 RZ(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(2==AN(w),EVLENGTH);
 RZ(w=vi(w)); v=AV(w); p=v[0]; q=v[1];
 ASSERT(0<=p&&0<=q,EVDOMAIN);
 ASSERT(p<14&&q<14,EVLIMIT);
 jt->locsize[0]=p;
 jt->locsize[1]=q;
 R mtm;
}    /* 9!:39 default locale size set */


// find the symbol table for locale with name u which has length n and hash/number bucketx
// locale name is known to be valid
// n=0 means 'use base locale'
// n=-1 means 'numbered locale, don't bother checking digits'   u is invalid
A jtstfind(J jt,I n,C*u,I bucketx){L*v;
 if(!n){n=sizeof(jt->baselocale); u=jt->baselocale;bucketx=jt->baselocalehash;}
 if(n>0&&'9'<*u){  // named locale   > because baselocale is known to be non-empty
  v=probe(n,u,(UI4)bucketx,jt->stloc);
  if(v)R v->val;   // if there is a symbol, return its value
 }else{
  R findnl(bucketx);
 }
 R 0;  // not found
}

// look up locale name, and create the locale if not found
// bucketx is hash (for named locale) or number (for numeric locale)
// n=0 means 'use base locale'
// n=-1 means 'numbered locale, don't bother checking digits'
A jtstfindcre(J jt,I n,C*u,I bucketx){
 A v = stfind(n,u,bucketx);  // lookup
 if(v)R v;  // return if found
 if(n>=0&&'9'<*u){  // nonnumeric locale:
  I p; FULLHASHSIZE(1LL<<(5+jt->locsize[0]),SYMBSIZE,1,SYMLINFOSIZE,p);
  R stcreate(0,p,n,u);  // create it with name
 }else{
  ASSERT(0,EVLOCALE); // illegal to create numeric locale explicitly
 }
}

// b is flags: 1=check name for validity, 2=do not allow numeric locales (whether atomic or not)
static A jtvlocnl(J jt,I b,A w){A*wv,y;C*s;I i,m,n;
 RZ(w);
 if((!(b&2)) && (AT(w)&INT || AT(w)&B01/C_LE && !AR(w)))R w;  // integer list or scalar boolean is OK
 n=AN(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 wv=AAV(w); 
 for(i=0;i<n;++i){
  y=wv[i];  // pointer to box
  if((!(b&2))&&!AR(y)&&AT(y)&(INT|B01)/C_LE)continue;   // scalar numeric locale is ok
  m=AN(y); s=CAV(y);
  ASSERT(1>=AR(y),EVRANK);
  ASSERT(m,EVLENGTH);
  ASSERT(LIT&AT(y),EVDOMAIN);
  ASSERT(!(b&2) || CAV(y)[0]>'9',EVDOMAIN);  // numeric locale not allowed except when called for in b
  if(b&1)ASSERTN(vlocnm(m,s),EVILNAME,nfs(m,s));
 }
 R w;
}    /* validate namelist of locale names  Returns list if all valid, else 0 for error */

// p points to string of length l.  Result stored in z, which must be a declared lvalue
// overflow is ignored, much as strtoI did (it clamped, which is similarly invalid).  We expect the user to have audited the length
#define strtoI10(p,l,z) {z=0; C* p_ = p; DQ(l, z=4*z+z; z=2*z + (*p_++-'0');)}
// subroutine version
I strtoI10s(I l,C* p) {I z; strtoI10(p,l,z); R z; }

F1(jtlocnc){A*wv,y,z;C c,*u;I i,m,n,*zv;
 RZ(vlocnl(0,w));
 n=AN(w); wv=AAV(w); 
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
 if(AT(w)&(INT|B01)){IAV(z)[0]=findnl(BIV0(w))?1:-1; RETF(z);}
 for(i=0;i<n;++i){
  y=wv[i];
  if(!AR(y)&&AT(y)&((INT|B01))){  // atomic numeric locale
   zv[i]=findnl(BIV0(y))?1:-1;  // OK because the boxed value cannot be virtual, thus must have padding
  }else{
   // string locale, whether number or numeric
   m=AN(y); u=CAV(y); c=*u; 
   if(!vlocnm(m,u))zv[i]=-2;
   else if(c<='9') zv[i]=findnl(strtoI10s(m,u))?1:-1;
   else            zv[i]=probe(m,u,(UI4)nmhash(m,u),jt->stloc)?0:-1;
  }
 }
 RETF(z);
}    /* 18!:0 locale name class */

static F1(jtlocnlx){A y,z;B*wv;I m=0;
 RZ(w=cvt(B01,w)); wv=BAV(w); DO(AN(w), m|=1+wv[i];);
 if(1&m)z=nlsym(jt->stloc);
 if(2&m)RZ(y=jtactivenl(jt));  // get list of active locales
 z=0==m?mtv:1==m?z:2==m?y:over(y,z);
 R grade2(z,ope(z));
}

F1(jtlocnl1){memset(jt->workareas.namelist.nla,C1,256); R locnlx(w);}
    /* 18!:1 locale name list */

F2(jtlocnl2){UC*u;
 RZ(a&&w);
 ASSERT(LIT&AT(a),EVDOMAIN);
 memset(jt->workareas.namelist.nla,C0,256); 
 u=UAV(a); DO(AN(a),jt->workareas.namelist.nla[*u++]=1;);
 R locnlx(w); 
}    /* 18!:1 locale name list */

static A jtlocale(J jt,B b,A w){A g=0,*wv,y;
 if(!AR(w) && AT(w)&(INT|B01))R (b?jtstfindcre:jtstfind)(jt,-1,0,BIV0(w));  // atomic integer is OK
 RZ(vlocnl(1,w));
 wv=AAV(w); 
 DO(AN(w), y=AT(w)&BOX?AAV(w)[i]:sc(IAV(w)[i]); if(!(g=(b?jtstfindcre:jtstfind)(jt,AT(y)&((INT|B01))?-1:AN(y),CAV(y),AT(y)&((INT|B01))?BIV0(y):BUCKETXLOC(AN(y),CAV(y)))))R 0;);
 R g;
}    /* last locale (symbol table) from boxed locale names; 0 if none or error.  if b=1, create locale */

F1(jtlocpath1){AD * RESTRICT g; AD * RESTRICT z; F1RANK(0,jtlocpath1,0); ASSERT(vlocnl(1,w),EVDOMAIN); RZ(g=locale(1,w));
 g=LOCPATH(g); RZ(z=ca(g)); DO(AN(g), A t; RZ(t=ca(AAV(g)[i])); AS(t)[0]=AN(t); AAV(z)[i]=t;) R z;
}
 // for paths, the shape holds the bucketx.  We must create a new copy that has the shape restored
     /* 18!:2  query locale path */

F2(jtlocpath2){A g; AD * RESTRICT x;
 F2RANK(1,0,jtlocpath2,0);
 if(AN(a))RZ(  locale(1,a)); RZ(x=every(ravel(a),0L,jtravel));  // Don't audit empty a
 RZ(g=locale(1,w));
 // paths are special: the shape of each string holds the bucketx for the string.  Install that.
 AD * RESTRICT z; RZ(z=ca(x)); DO(AN(x), A t; RZ(t=ca(AT(AAV(x)[i])&((INT|B01))?thorn1(AAV(x)[i]):AAV(x)[i]));  AS(t)[0]=BUCKETXLOC(AN(t),CAV(t)); AAV(z)[i]=t;)  // ? why so many copies?  test before thorn1 not reqd
 fa(LOCPATH(g)); ras(z); LOCPATH(g)=z;
 ++jt->modifiercounter;  // invalidate any extant lookups of modifier names
 R mtm;
}    /* 18!:2  set locale path */


static F2(jtloccre){A g,y;C*s;I n,p;L*v;
 RZ(a&&w);
 if(MARK&AT(a))p=jt->locsize[0]; else{RE(p=i0(a)); ASSERT(0<=p,EVDOMAIN); ASSERT(p<14,EVLIMIT);}
 y=AAV0(w); n=AN(y); s=CAV(y);
 if(v=probe(n,s,(UI4)nmhash(n,s),jt->stloc)){   // scaf this is disastrous if the named locale is on the stack
  // named locale exists.  Verify no defined names, then delete it
  g=v->val; 
  LX *u=SYMLINFOSIZE+LXAV0(g); DO(AN(g)-SYMLINFOSIZE, ASSERT(!u[i],EVLOCALE););
  probedel(n,s,(UI4)nmhash(n,s),jt->stloc);  // delete the symbol for the locale, and the locale itself
 }
 FULLHASHSIZE(1LL<<(p+5),SYMBSIZE,1,SYMLINFOSIZE,p);  // get table, size 2^p+6 minus a little
 RZ(stcreate(0,p,n,s));
 R box(ca(y));
}    /* create a locale named w with hash table size a */

static F1(jtloccrenum){C s[20];I k,p;
 RZ(w);
 if(MARK&AT(w))p=jt->locsize[1]; else{RE(p=i0(w)); ASSERT(0<=p,EVDOMAIN); ASSERT(p<14,EVLIMIT);}
 RE(k=jtgetnl(jt));
 FULLHASHSIZE(1LL<<(p+5),SYMBSIZE,1,SYMLINFOSIZE,p);  // get table, size 2^p+6 minus a little
 RZ(stcreate(1,p,k,0L));
 sprintf(s,FMTI,k); 
 R box(cstr(s));
}    /* create a numbered locale with hash table size n */

F1(jtloccre1){
 RZ(w);
 if(AN(w))R rank2ex(mark,vlocnl(2+1,w),0L,0L,0L,0L,0L,jtloccre);
 ASSERT(1==AR(w),EVRANK);
 R loccrenum(mark);
}    /* 18!:3  create locale */

F2(jtloccre2){
 RZ(a&&w);
 if(AN(w))R rank2ex(a,vlocnl(2+1,w),0L,0L,0L,0L,0L,jtloccre);
 ASSERT(1==AR(w),EVRANK);
 R rank1ex(a,0L,0L,jtloccrenum);
}    /* 18!:3  create locale with specified hash table size */


F1(jtlocswitch){A g;
 RZ(w);
 ASSERT(!AR(w),EVRANK); 
 RZ(g=locale(1,w));
 // put a marker for the operation on the call stack
 // If there is no name executing, there would be nothing to process this push; so don't push for unnamed execs (i. e. from console)
 if(jt->curname)pushcallstack1(CALLSTACKPOPFROM,jt->global);
 jt->global=g;
 ++jt->modifiercounter;  // invalidate any extant lookups of modifier names

 R mtm;
}    /* 18!:4  switch locale */

F1(jtlocname){A g=jt->global;
 ASSERTMTV(w);
 ASSERT(g,EVLOCALE);
 R box(sfn(0,LOCNAME(g)));
}    /* 18!:5  current locale name */

static SYMWALK(jtlocmap1,I,INT,18,3,1,
    {I t=AT(d->val);
     *zv++=i; 
     *zv++=t&NOUN?0:t&VERB?3:t&ADV?1:t&CONJ?2:(t==SYMB)?6:-2;
     *zv++=(I)rifvs(sfn(1,d->name));})  // this is going to be put into a box

F1(jtlocmap){A g,q,x,y,*yv,z,*zv;I c=-1,d,j=0,m,*qv,*xv;
 RZ(w);
 ASSERT(!AR(w),EVRANK);
 RE(g=equ(w,zeroionei[0])?jt->stloc:equ(w,zeroionei[1])?jt->local:locale(0,w));
 ASSERT(g,EVLOCALE);
 RZ(q=locmap1(g)); qv=AV(q);
 m=*AS(q);
 // split the q result between two boxes
 GATVR(x,INT,m*3,2,AS(q)); xv= AV(x);
 GATV0(y,BOX,m,  1); yv=AAV(y);
 DO(m, *xv++=d=*qv++; *xv++=j=c==d?1+j:0; *xv++=*qv++; c=d; *yv++=(A)*qv++;);
 GAT0(z,BOX,2,1); zv=AAV(z); zv[0]=x; zv[1]=y;
 R z;
}    /* 18!:30 locale map */

static SYMWALK(jtredefg,B,B01,100,1,1,RZ(redef(mark,d)))
     /* check for redefinition (erasure) of entire symbol table */

F1(jtlocexmark){A g,*wv,y,z;B *zv;C*u;I i,m,n;L*v;
 RZ(vlocnl(1,w));
 if(AT(w)&B01)RZ(w=cvt(INT,w));  // Since we have an array, we must convert b01 to INT
 n=AN(w); wv=AAV(w); 
 GATV(z,B01,n,AR(w),AS(w)); zv=BAV(z);
 for(i=0;i<n;++i){
  g=0;
  if(AT(w) & (INT)){zv[i]=1; g = findnl(IAV(w)[i]);
  }else{
   zv[i]=1; y=wv[i];
   if(AT(y)&(INT|B01)){g = findnl(BIV0(y));
   }else{
    m=AN(y); u=CAV(y);
    if('9'>=*u){g = findnl(strtoI10s(m,u));}
    else {v=probe(m,u,(UI4)nmhash(m,u),jt->stloc); if(v)g=v->val;}  // g is locale block for named locale
   }
  }
  if(g){I k;  // if the specified locale exists in the system...
   // See if we can find the locale on the execution stack.  If so, set the DELETE flag
   for(k=0;k<jt->callstacknext;++k)if(jt->callstack[k].value==g)break;
   if(k<jt->callstacknext)
    jt->callstack[k].type|=CALLSTACKDELETE;  // name active on stack; mark for deletion
   else if(g==jt->global){
    // Name is not on stack but it is executing now.  Add a change+delete entry for it.  There may be multiple of these outstanding
    pushcallstack1(CALLSTACKCHANGELOCALE|CALLSTACKDELETE,g);  // mark locale for deletion
   } else locdestroy(g);  // not on stack and not running - destroy immediately
  }
 }
 R z;
}    /* 18!:55 destroy a locale (but only mark for destruction if on stack) */

// destroy symbol table g.  
B jtlocdestroy(J jt,A g){
 ++jt->modifiercounter;  // invalidate any extant lookups of modifier names
 // Look at the name to see whether the locale is named or numbered
 NM *locname=NAV(LOCNAME(g));  // NM block for name
 B isnum = '9'>=locname->s[0];  // first char of name tells the type
 if(isnum){
  // For numbered locale, find the locale in the list of numbered locales, wipe it out, free the locale, and decrease the number of those locales
  RZ(redefg(g)); jterasenl(jt,locname->bucketx); fr(g);  // remove the locale from the hash table, then free the locale and its names
 } else {
  // For named locale, find the entry for this locale in the locales symbol table, and free the locale and the entry for it
  RZ(redefg(g)); probedel(locname->m,locname->s,locname->hash,jt->stloc);  // free the L block for the locale, which frees the locale itself and its names
 }
 if(g==jt->global)jt->global=0;
 R 1;
}    /* destroy locale jt->callg[i] (marked earlier by 18!:55) */
