/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Dictionary support                                                      */

#include "j.h"
// Dictionary support
#ifndef CRC32
#define HASH4(crc,x) ((UI4)(crc)*0x85249421+(UI4)(x))
#define HASH8(crc,x) HASH4(HASH4(crc,x),(UI8)(x)>>32)
#else
#define HASH4(crc,x) CRC32(crc,x)
#if BW==64
#define HASH8(crc,x) CRC32L(crc,x)
#else
#define HASH8(crc,x) HASH4(HASH4(crc,x),(UI8)(x)>>32)
#endif
#endif
#if BW==64
#define HASHI(crc,x) HASH8(crc,x)
#else
#define HASHI(crc,x) HASH4(crc,x)
#endif

// CRC32 of n floats.
static INLINE UI4 crcfloats(UI8 *v, I n){
 // Do 3 CRCs in parallel because the latency of the CRC instruction is 3 clocks.
 // This is executed repeatedly so we expect all the branches to predict correctly
 UI4 crc0=-1;  // convert bytecount to words
 if((n-=3)<0){crc0=HASH8(crc0,v[0]==0x8000000000000000?0:v[0]); if(n==-1)crc0=HASH8(crc0,v[1]==0x8000000000000000?0:v[1]); R crc0;}   // fast path for the common short case
 UI4 crc1=crc0, crc2=crc0;  // init all CRCs
 do{crc0=HASH8(crc0,v[n]==0x8000000000000000?0:v[n]); crc1=HASH8(crc1,v[n+1]==0x8000000000000000?0:v[n+1]); crc2=HASH8(crc2,v[n+2]==0x8000000000000000?0:v[n+2]);}while((n-=3)>=0);  // Do blocks of 24 bytes
 if(n>=-2){crc0=HASH8(crc0,v[0]==0x8000000000000000?0:v[0]); if(n>-2)crc1=HASH8(crc1,v[1]==0x8000000000000000?0:v[1]);}  // handle last 1 or 2 words
 R HASH8(crc1,(crc2<<29)+(crc1<<3));  // combine the CRCs, as quickly as possible
}

// CRC32 of n Is
static INLINE UI4 crcwords(UI *v, I n){
 // Do 3 CRCs in parallel because the latency of the CRC instruction is 3 clocks.
 // This is executed repeatedly so we expect all the branches to predict correctly
 UI4 crc0=-1;  // convert bytecount to words
 if((n-=3)<0){crc0=HASHI(crc0,v[0]); if(n==-1)crc0=HASHI(crc0,v[1]); R crc0;}   // fast path for the common short case
 UI4 crc1=crc0, crc2=crc0;  // init all CRCs
 do{crc0=HASHI(crc0,v[n]); crc1=HASHI(crc1,v[n+1]); crc2=HASHI(crc2,v[n+2]);}while((n-=3)>=0);  // Do blocks of 3 words
 if(n>=-2){crc0=HASHI(crc0,v[0]); if(n>-2)crc1=HASHI(crc1,v[1]);}  // handle last 1 or 2 words
 R HASH8(crc1,(crc2<<29)+(crc1<<3));  // combine the CRCs, as quickly as possible
}

// CRC32 of n bytes
static INLINE UI4 crcbytes(C *v, I n){
 UI4 wdcrc=n&-SZI?crcwords((UI*)v,n>>LGSZI):~0;  // take CRC of the fullword part, if any
 if(n&(SZI-1)){wdcrc=HASHI(wdcrc,((UI*)v)[n>>LGSZI]<<((SZI-(n&(SZI-1)))<<LGSZI));}  // if there are bytes, take their CRC after discarding garb.  Avoid overfetch
 R wdcrc;  // return composite CRC
}

// CRC32 of n boxes
static UI4 jtcrcy(J jt,A y);
static INLINE UI4 crcboxes(A *v, I n, J jt){UI4 crc=0; A cur=v[0]; DO(n, A nxt=v[i+1]; crc=HASH4(crc,jtcrcy(jt,C(cur))); cur=nxt;) R crc;}  // overfetch OK

// CRC32 of n Xs
static INLINE UI4 crcxnums(X *v, I n){UI4 crc=0; X cur=v[0]; DO(n, X nxt=v[i+1]; crc=HASH4(crc,likely(XLIMBLEN(v[i])!=0)?crcwords(UIAV(v[i]),XLIMBLEN(v[i])):~0); cur=nxt;) R crc;}

// CRC32 of y.  Floats must observe -0.
static UI4 jtcrcy(J jt,A y){
 I yt=AT(y), yn=AN(y); void *yv=voidAV(y);  // type of y, #atoms, address of data
 if(unlikely(yn==0))R ~0;  // so no inner routine need check for len=0
 if(yt&INT+(SZI==4)*(C4T+INT4))R crcwords(yv,yn);  // INT type, might be full words
 if(yt&B01+LIT+C2T+C4T+INT1+INT2+INT4)R crcbytes(yv,yn<<bplg(yt));   // direct non-float
 if(yt&FL+CMPX+QP)R crcfloats(yv,yn<<(((UI)yt>=CMPX)));  // float
 if(yt&BOX){R crcboxes(yv,yn,jt);}  // box, take CRC of all the boxes
 if(yt&RAT+XNUM){R crcxnums(yv,yn<<((UI)yt>=RAT));}  // RAT/XNUM, take CRC of all of them
 else SEGFAULT;  // scaf
}

// 16!:0 monad create 32-bit hash of y.  For float types, +-0 must have the same hash
DF2(jthashy){F12IP; ARGCHK2(a,w) RETF(sc(jtcrcy(jt,w)));}

#if 1   // obsolete 
// everything about the dictionary.  This struct overlays the entire block.  AM of the first
// cacheline is the lock for the dic.  The data starts at AS[1], the second cacheline.  The block
// is a recursive BOX but AK points past the areas that do not need to be freed, i.e to the boxed arrays
typedef struct ADic {
 I header[8];  // A header up through s[0].  DIC is always allocated with rank 1.
 struct Dic { // *** this group of values is updated atomically en bloc to make sure hashelesiz matches hash when possible.
  union {
   struct {
    C filler[5];
    C flags;
#define DICFBASE (offsetof(struct Dic,flags)*BB)   // bit offset of flags in hashsiz   
#define DICFIHF (I8)1  // set if using internal hashing function
#define DICFICF (I8)2  // set if using internal compare function
#define DICFKINDIR (I8)4   // set if keys are indirect type
#define DICFVINDIR (I8)8   // set if vals are indirect type
#define DICFRB (I8)16  // set if red/black tree
#define DICFRESIZE (I8)32  // set if we are running out & will have to resize
#define DICFSINGLETHREADED (I8)64  // set if we should bypass locking, assuming the user doesn't do put/get simultaneously
// NOTE bit 7 of flags must be 0, used as LSB of a length
    C emptysiz;  // length in bytes of the chain field in the empty chain - never exceeding length of key  Top 3 bits always 0, used as LSBs of hashelesiz
    C hashelesiz;  // number of bytes in a hash slot, 1-5 (we support only 4)
   };
   UI8 hashsiz;  //  number of elements in hash table.  We leave space for 40 bits but we don't support more than 32
  } ;
  UI4 (*hashfn)();  // hash function, user's or selected internal
  I (*compfn)();  // key-comparison function, user's or selected internal
   // *** start of area pointed by AK, which is the value of dic as a J noun
  A hash;  // the hash/tree table, rank 1  Note: This pointer may be stale.  MUST use voidAV1 to point to hash, and use only PREFETCH until you get a lock
  A keys;  // array of keys    nondisplayable if indirect
  A vals;   // array of values    nondisplayable if indirect
  // end of first cacheline.  Following are unchanging
  A kshape;   // shape of a key, always accessed by AAV1
  A vshape;  // shape of a value, always accessed by AAV1
  A hashcompself;  // self to use for user's hash/compare verb    displays as empty
  A locale;  // the numbered locale of the dictionary, saved here to protect it.  User hash/compare is called in this locale    displays as empty
   // *** end of J value
  UI4 ktype;  // type of a key
  UI4 vtype;   // type of a value
  union {
   struct {
    UI4 kitemlen;  // number of hash/compare items in a key when using 16!:0
    UI4 kbytelen;  // number of bytes in a key
   };
   UI8 klens;
  };
  UI4 vbytelen;  // number of bytes in a value, for copying
  UI4 kaii;   // atoms in a key
  UI4 vaii;   // atoms in a value
  C lgminsiz;  // lg(minimum cardinality).  When cardinality drops below 1LL<<lgminsiz, we resize.
  I filler1;
  // end of second cacheline.  Following changed only by put/del
  UI cardinality;  // number of kvs in the hashtable
  UI emptyn;  // index to next empty kv/treeslot.  EOC loops back on itself.  Resize when empty
  I filler2[SY_64?6:3];  // pad to cacheline (24 words on each system).
 } bloc;
} DIC;
#define ST UI4   // type of hash slot
#define STX UI8   // type of index to hash slot, which is + for found, 1s-comp for not found
#define STN 4  // width of hash slot


// Hashtable info ***********************
#define HASHNRES 4  // # reserved code points for empties: empty/birthstone/tombstone/tombstone+birthstone
#define HASHBSTN 1  // this bit set when the slot is a birthstone (a slot marked for fill by a put)
#define HASHTSTN 2  // this bit set when the slot is a tombstone (an empty slot that must not terminate a search for put)
#define HENCEMPTY(biaskx) ((biaskx)-HASHNRES)  // convert value in hash to index in kv of empty
#define HDECEMPTY(kx) ((kx)+HASHNRES)  // convert kx of empty to value in hash 

// Red/black tree info
#define TREENRES 2  // # reserved full (i. e. paired) slots for empties, also used to hold the root
#define RENCEMPTY(biaskx) (((biaskx)>>1)-TREENRES)  // convert index in tree (with LSB=color) to index in kv of empty
#define RDECEMPTY(kx) (2*((kx)+TREENRES))  // convert kx of empty to index in tree
#define NODE(x) ((x)&~1)   // convert node\color to node
#define COLOR(x) ((x)&1)   // convert node\color to color
#define PDIRNODE(x) ((x)&~1)   // convert node\dir to node
#define PDIRDIR(x) ((x)&1)   // convert node\dir to dir



// write l bytes from x to address a.  Destroys x, which must be a name.  Only low 3 bits of l are used
#define WRHASH1234(x,l,a) {if((l)&4)*(UI4*)(a)=x;else{if((l)&1){*(C*)(a)=x;x>>=8;}if((l)&2)*(US*)((C*)a+((l)&1))=x;}}  // avoid read, which will probably be a long miss.  Branches will predict
// obsolete // same for writing ~0
// obsolete #define WRCONST1234(x,l,a) {if((l)&4)*(UI4*)(a)=(x);else{if((l)&1){*(C*)(a)=(x);}if((l)&2)*(US*)((C*)a+((l)&1))=(x);}}  // avoid read, which will probably be a long miss.  Branches will predict

// u 16!:_1 y  adverb.  allocate hashtable.  Always called from the numbered locale of the dictionary.
// u is the hash/compare verb OR the DIC block for a dictionary that is being resized.
// y is (min #eles,max #eles,#hashslots);(flags: singlethreading (default 1));(key type;key shape);(value type;value shape)    to create a new dictionary
//      (min #eles,max #eles,#hashslots)   when a dictionary is being resized
//     if #hashslots is omitted, the map is a red/black tree
// result is DIC to use, ready for get/put, with hash/keys/vals/empty allocated, and 0 valid kvs
static DF1(jtcreatedic1){F12IP;A box,box1;  // temp for box contents
 ARGCHK1(w);
 A a=FAV(self)->fgh[0];  // extract u from the verb
 A z; GAT0(z,BOX,20,1) AK(z)=offsetof(struct ADic,bloc.hash); AN(z)=AS(z)[0]=7;  // allocate nonrecursive box, long enough to make the total allo big enough in 32- and 64-bit.  Then restrict to the boxes in case of error
 I flags;  // part of a union that gets overwritten
 if(AT(a)&VERB){  // initial creation
  //  install user's verb, & see if the user wants the internal functions
  AF fn=FAV(a)->valencefns[0]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[0])->valencefns[0]:fn; flags=fn==jthashy?DICFIHF:0;  // note if default hash
  fn=FAV(a)->valencefns[1]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[1])->valencefns[1]:fn; flags|=fn==jttao?DICFICF:0;  // note if default comp
  INCORPNV(a); ((DIC*)z)->bloc.hashcompself=a;    // save self pointer to user's function, which protects it while this dic is extant
  ((DIC*)z)->bloc.locale=jt->global;  // remember the locale this dictionary is in.  We protect it in the DIC block

  // flags, key, value sizes
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==4,EVLENGTH)  // 4 boxes
  box=C(AAV(w)[1]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)<=1,EVLENGTH)  // flags.  First is singlethreading (default 1), must have 0-1 values
  I deffg=DICFSINGLETHREADED; if(AN(box)>0){if(!(AT(box)&B01))RZ(box=ccvt(B01,box,0)) if(!BAV(box)[0])deffg=0;}  // set concurrent if user specifies it
  flags|=deffg;  // remember user's choice

  // keyspec.  must be 2 boxes
  box=C(AAV(w)[2]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)
  box1=C(AAV(box)[0]); I t; RE(t=i0(box1)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN) flags|=t&DIRECT?0:DICFKINDIR; // type.  convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid.  Remember if indirect
  box1=C(AAV(box)[1]); I r=AN(box1); ASSERT(AR(box1)<=1,EVRANK) ASSERT(r>=0,EVLENGTH) RZ(box1=ccvt(INT,ravel(box1),0)) I n, *s=IAV(box1); PRODX(n,r,s,1) ((DIC*)z)->bloc.kaii=n; ASSERT(n>0,EVLENGTH) // shape. copy to allow IAV1.  get # atoms in item & save
  ASSERT(AN(box1)<=9 || flags&DICFICF,EVNONCE)  // if the user has a compare function, we want virt to be on the stack to save registers.  Make sure the rank is OK then
  INCORPNV(box1); ((DIC*)z)->bloc.kshape=box1; ((DIC*)z)->bloc.ktype=t; I l=n<<bplg(t); ((DIC*)z)->bloc.kbytelen=l; // save shape & type; save #bytes in key
  UI4 (*fn2)()=l&(SZI-1)?(UI4 (*)())crcbytes:(UI4 (*)())crcwords; fn2=(t&XNUM+RAT)?crcxnums:fn2; fn2=(t&CMPX+FL+QP)?crcfloats:fn2; fn2=(t&BOX)?crcboxes:fn2; fn2=flags&DICFIHF?fn2:(UI4 (*)())FAV(a)->valencefns[0]; ((DIC*)z)->bloc.hashfn=fn2; // save internal or external hash function  
  I (*fn3)()=l&(SZI-1)?(I (*)())taoc:(I (*)())taoi; fn3=(t&XNUM+RAT)?taox:fn3; fn3=(t&CMPX+FL+QP)?taof:fn3; fn3=(t&BOX)?taor:fn3; fn3=flags&DICFICF?fn3:(I (*)())FAV(a)->valencefns[1]; ((DIC*)z)->bloc.compfn=fn3; // save int/ext comp function.  We care only about equality  
  I lsh=l&(SZI-1)?0:LGSZI; lsh=(t&BOX+XNUM+RAT)?LGSZI:lsh; lsh=(t&CMPX+FL+QP)?FLX:lsh; ((DIC*)z)->bloc.kitemlen=l>>lsh;  // length to use for internal hash/comp

  // valuespec.  must be 2 boxes
  box=C(AAV(w)[3]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)
  box1=C(AAV(box)[0]); RE(t=i0(box1)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN) flags|=t&DIRECT?0:DICFVINDIR;  // type. convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid.  Remember if indirect
  box1=C(AAV(box)[1]); r=AN(box1); ASSERT(AR(box1)<=1,EVRANK) ASSERT(r>=0,EVLENGTH) RZ(box1=ccvt(INT,ravel(box1),0)) s=IAV(box1); PRODX(n,r,s,1) ((DIC*)z)->bloc.vaii=n;  // shape. copy to allow IAV1.  get # atoms in item & save
  ASSERT(likely(n>0)||r==1,EVLENGTH)   // empty value only allowed at rank 1
  INCORPNV(box1); ((DIC*)z)->bloc.vshape=box1; ((DIC*)z)->bloc.vtype=t; ((DIC*)z)->bloc.vbytelen=n<<bplg(t);  // save shape & type; save # bytes for copy
  box=C(AAV(w)[0]);  // fetch size parameters
  ((DIC*)z)->bloc.flags=flags|=AN(box)==2?DICFRB:0;  // now that we have all the flags, save them.  Remember hash/tree type

 }else{  // resize
  flags=((DIC*)a)->bloc.flags;  // preserve flags over hashsiz write
  ((DIC*)z)->bloc=((DIC*)a)->bloc;  // init everything from the previous dic
  box=w;  // w is nothing but size parms
 }

 // box has the size parameters.  Audit & install into dic, overwriting anything that was copied.
 ASSERT(AR(box)<=1,EVRANK) ASSERT(BETWEENC(AN(box),2,3),EVLENGTH) if(!AT(box)&INT)RZ(box=ccvt(INT,box,0));  // sizes. must be box of 3 integers
 I redblack=!!(flags&DICFRB);  // if only min,max, that's a red/black tree
 UI hashsiz=((DIC*)z)->bloc.hashsiz=IAV(box)[2-redblack];   // move hashsiz, which overwrites flags/lgminsiz/hashelesiz
 ((DIC*)z)->bloc.lgminsiz=CTLZI(UIAV(box)[0]|1); UI maxeles=IAV(box)[1]; ASSERT((I)maxeles>0,EVDOMAIN)   // save min size, get max# kvs
 UI hashelesiz;   // length of a kv index, in the hashtable or tree: max slot#, plus reserved (empty/tombstone/birthstone).  Subtract 1 for max code point.  top bit#+1 is #bits we need; round that up to #bytes
 if(redblack){
  // red/black.  LSB of indexes is reserved for color
  hashelesiz=((DIC*)z)->bloc.hashelesiz=(CTLZI(maxeles+TREENRES-1)+1+1+(BB-1))>>LGBB;
 }else{   // hash, nothing reserved in index
  ASSERT(hashsiz>(maxeles+(maxeles>>4)),EVDEADLOCK)  // min, max, hash sizes.  Hash at least 6% spare
  hashelesiz=((DIC*)z)->bloc.hashelesiz=(CTLZI(maxeles+(redblack?TREENRES:HASHNRES)-1)+1+(BB-1))>>LGBB;
 }
 ((DIC*)z)->bloc.flags=flags;   // restore flags
 // Because we chain the empty keyslots on the empty chain, we must ensure that the length of that chain field
 // does not exceed the length of the key.  Because hashelesiz includes space for excess codepoints, it may
 // be too big.  We make sure that the chain does not have any wasted codepoints, and we calculate its length
 // separately, being enough to point to an UNBIASED slot and not enough to overflow the length.  Also, the chain length must not change over a resize.
 // For simplicity, we just use 4-byte empty chains unless the key is shorter than that.
 // We must bias/unbias these indexes for use in the hash/tree.
 if(unlikely(((DIC*)z)->bloc.kbytelen<4)){((DIC*)z)->bloc.emptysiz=((DIC*)z)->bloc.kbytelen; maxeles=MIN(maxeles,(UI)1<<(((DIC*)z)->bloc.kbytelen*SZI));}  // no need to allow for more elements than addressable keypoints.  Use chain len of 4 unless key shorter
// obsolete  if(unlikely(hashelesiz>((DIC*)z)->bloc.kbytelen)){
// obsolete   maxeles=(I)1<<(((DIC*)z)->bloc.kbytelen*SZI);  // no need for more slots than possible code points
// obsolete   ((DIC*)z)->bloc.emptysiz=((DIC*)z)->bloc.kbytelen;  // don't overflow the key with the chain field
 else((DIC*)z)->bloc.emptysiz=4;   // not short key, use convenient chain length


 // allocate & protect hash/keys/vals, and put all keys on the empty chain
 // hashtbl is hashelsiz per entry
 // redblack has no empty table, and the tree (=hash) has shape nx2xhashelesiz (the LSB of indexes is color/0)
 UI htelesiz=hashelesiz*(1+redblack);   // length of tree/hash entry
 if(redblack){  // red/black: allocate n2nxh block for tree
  GATV0(box,LIT,(maxeles+TREENRES)*htelesiz*2,3) AS(box)[0]=maxeles+TREENRES; AS(box)[1]=2; AS(box)[2]=hashelesiz; INCORPNV(box)  // alloc incl res eles, the root pointer
  IAVn(3,box)[0]=2*0+0; IAVn(3,box)[1]=2*0+0;// empty tree.  parent of root is red NULL (with only one child) regardless of elesiz.  empty list is not biased.  First key points to root
 }else{  // hash: allocate hashtable, as a LIT list
  GATV0(box,LIT,hashsiz*hashelesiz,1) INCORPNV(box) mvc(hashsiz*hashelesiz,voidAV1(box),MEMSET00LEN,MEMSET00);   // allocate hash table & fill with empties
 }
 ((DIC*)z)->bloc.emptyn=0; ((DIC*)z)->bloc.hash=box;   // save tree/hash, and set root of empty chain as unbiased 0
 I t=((DIC*)z)->bloc.ktype; A sa=((DIC*)z)->bloc.kshape;   // key type & shape
 GA0(box,t,maxeles*((DIC*)z)->bloc.kaii,AN(sa)+1) AFLAG(box)=(t&RECURSIBLE)|(t&DIRECT?0:AFUNDISPLAYABLE); INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(sa),AN(sa)) ((DIC*)z)->bloc.keys=box;   // allocate array of keys, recursive and undisplayable if indirect because of empty chain
 void *ev=voidAVn(AN(sa)+1,box); DO(maxeles-1, *(UI4*)ev=i+1; ev=(void *)((I)ev+((DIC*)z)->bloc.kbytelen);)   // allocate empty list & chain empties together
 *(UI4*)ev=maxeles-1; // install end of chain loopback.  overstore OK
 t=((DIC*)z)->bloc.vtype; sa=((DIC*)z)->bloc.vshape;   // value type & shape
 GA0(box,t,maxeles*((DIC*)z)->bloc.vaii,AN(sa)+1) AFLAG(box)=(t&RECURSIBLE)|(t&DIRECT?0:AFUNDISPLAYABLE); INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(sa),AN(sa)) ((DIC*)z)->bloc.vals=box;   // allocate array of vals, recursive
 ((DIC*)z)->bloc.cardinality=0;  // init the dic is empty
 ra0(z); INCORP(z); AM(z)=0;  // make z recursive, protecting descendants; INCORP and clear the lock
 RETF(z)
}

// u 16!:_1 y.  For some reason jtlr won't display CIBEAM when it is a conjunction, so we have to make it an adverb.  We save u (the dictionary) for the verb
F1(jtcreatedic){F12IP;
 ARGCHK1(w)
 R fdef(0,CIBEAM,VERB,jtcreatedic1,jtvalenceerr,w,0,0,VFLAGNONE,RMAX,RMAX,RMAX);
}

// Dic locking ************************************************************

// We use a 4- or 6-step locking sequence.  In each step the new state is requested shortly before it is needed and then waited for when it is needed (to give the lock request time to propagate to all threads).
// Readers use the sequence
// DICLKRDRQ(dic,lv,cond)  request read lock.   If cond is true, use singlethreading version
// DICLKRDWTK(dic,lv)   wait for read lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// DICLKRDWTV(dic,lv)   when finished with keys, wait for values to be available
// DICLKRDRELK(dic,lv)   then release read lock on keys
// DICLKRDRELV(dic,lv)   release read lock on values at end
// DICLKRDRELKV(dic,lv)   if the read locks for keys & values finish simultaneously, release them both (replaces DICLKRDRELK and DICLKRDRELV)

// Writers use the sequence
// DICLKRWRQ(dic,lv)  // request read/write lock.  fairly soon...
// DICLKRWWT(dic,lv)  // ...wait for read/write lock to be granted (i. e. current-write# is our seq#).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock.
// DICLKWRRQ(dic,lv)  // request write lock on keys&values
// DICLKWRWTK(dic,lv)  // wait for write lock on keys (i. e. no readers looking at them).  Readers may still be reading values
// DICLKWRWTV(dic,lv)  // wait for write lock on values (i. e. no readers looking at them)
// DICLKWRRELK(dic,lv)  // release write lock on keys (optional) - we must hold a lock on the values
// DICLKWRRELV(dic,lv)  // release write lock on values (and keys if lock held).  This updates the current-writer #

// A read lock guarantees that the control info and tables will not be modified by any thread in a way that could affect a get().  Other modifications are allowed.
// A read-write lock guarantees that the control info and tables will not be modified by any other thread.  No modification by this thread mey affect a get() in progress.
//  When a read-write lock is granted, the granted thread is guaranteed to be the next to write.  It has ownership of the write-lock and sequence# bits until it releases the lock by writing to the sequence#
// A write lock guarantees that no other thread has a lock of any kind.  Any modification is allowed.  The lock can then be advanced to a value lock, which declared that it will not touch the hash or the keys
// but may modify values

// The AM field of dic is the lock. 0-15=#read locks outstanding 16-31=seq# of next write req 32=writelock request 33=valueslock req/active 48-63=seq# of current writer (if 32-33 not 00) or next writer (if 32-33 00)
#define DICLMSKSEQ 0x7fffLL  // sequence# of current write owner
#define DICLMSKWRK 0x8000LL  // Current write owner request lock on keys  Invalid to request keys w/o values.  Must be carry-out from incrementing DICLMSKSEQ
#define DICLMSKWRV 0x10000LL  // Current write owner request lock on values
#define DICLMSKOKRET 0x20000LL  // in return value, indicates OK return
#define DICLMSKRESIZEREQ 0x40000LL  // in return value, indicates resize required
#define DICLMSKRDK       0x3fff80000LL  // #read locks for keys outstanding
#define DICLMSKRDV   0x1fffc00000000LL  // #read locks for values outstanding
#define DICLMSKRW 0xfffe000000000000LL  // seq# of write ownership requests
#define LKSEQ(x) ((x)>>0)
#define LKRDK(x) ((x)>>19)
#define LKRDV(x) ((x)>>34)
#define LKRW(x) ((x)>>49)
#define DICLSINGLETHREADED DICLMSKWRK  // WR=10 is impossible and used internally to indicate singlethreading.  DICLMSKRDV=0 is used as flag during read to indicate singlethreading.  DICLMSKWRK says we have the lock, which may persist over resize
     //  No RFO cycles are performed.  If we chance to detect overlapping puts/gets we will abort, but there are no guarantees.  

#if PYXES
// throughout the locking sequence the value lv holds the last info read from the lock in AM(dic).  We keep this as up-to-date as possible to avoid extra reads.  If the lock is not contended it will be read seldom
#define DICLKRDRQ(dic,lv,cond) (lv=!(cond)?__atomic_add_fetch(&AM((A)dic),LOWESTBIT(DICLMSKRDK)+LOWESTBIT(DICLMSKRDV),__ATOMIC_ACQ_REL):DICLSINGLETHREADED)  // put up read request and read current status. Cannot overflow
#define DICLKRDWTK(dic,lv) if(unlikely((lv&DICLMSKWRK)!=0))if((lv&DICLMSKWRV)!=0)lv=diclkrdwtkv(dic,lv);  // wait till current owner finishes with hash/keys, and update status.  Skip if single-threaded (WR=10)
#define DICLKRDRELK(dic,lv) if((lv&DICLMSKRDV)!=0)(lv=__atomic_fetch_sub(&AM((A)dic),LOWESTBIT(DICLMSKRDK),__ATOMIC_ACQ_REL));  // if not single-threaded, remove read request for keys
#define DICLKRDWTV(dic,lv) if(unlikely((lv&DICLMSKWRK+DICLMSKWRV)==DICLMSKWRV))diclkrdwtv(dic);  // (after we have a key lock) wait till the owner, if any, finishes with values.  Then we can read keys but not change them.  Single-threaded (WR=10) always passes
     // We know that our WTK gave us a read lock with WRK=0,WRV=x.  If there was a writer, state will go to 00 when it finishes and then may go only as far as 11 while we retain our read lock.  Thus, anythong but 01 gives us a value lock.
#define DICLKRDRELV(dic,lv) if((lv&DICLMSKRDV)!=0)__atomic_fetch_sub(&AM((A)dic),LOWESTBIT(DICLMSKRDV),__ATOMIC_ACQ_REL);  // if not single-threaded, remove read request for values
#define DICLKRDRELKV(dic,lv) if((lv&DICLMSKRDV)!=0)__atomic_fetch_sub(&AM((A)dic),LOWESTBIT(DICLMSKRDK)+LOWESTBIT(DICLMSKRDV),__ATOMIC_ACQ_REL);  // remove read request for keys & values simultaneously

// obsolete #define DICLKRWRQ(dic,lv) lv=__atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKRWX,__ATOMIC_ACQ_REL); scafRWC(dic);   // put up prewrite request and read status, which establishes our sequence# in lv where it will remain until we release the lock.  Can overflow
// obsolete #define DICLKRWWT(dic,lv) if(unlikely((US)(lv>>DICLMSKRWX)!=(US)(lv>>DICLMSKSEQX)))diclkrwwt(dic,lv);  // wait until we are the lead writer, which is immediately if there are no others
// obsolete #define DICLKWRRQ(dic,rdct) { __atomic_store_n((US*)((C*)&AM((A)dic)+DICLMSKWRX/BB),(US)3,__ATOMIC_RELEASE); rdct=__atomic_load_n((US*)((C*)&AM((A)dic)+DICLMSKRDX/BB),__ATOMIC_ACQUIRE); }   // put up write request for key & value (we are known to be the lead writer and own the write bit, which must be clear)
// obsolete #define DICLKWRWT(dic,rdct) if(unlikely(rdct!=0))diclkwrwt(dic);  // wait until all reads are quiesced
// obsolete #define DICLKWRRELK(dic) __atomic_store_n((US*)((C*)&AM((A)dic)+DICLMSKWRX/BB),(US)2,__ATOMIC_RELEASE);   // release the keys, while still writing to values
// obsolete #define DICLKWRRELV(dic,lv) __atomic_store_n((UI4*)((C*)&AM((A)dic)+DICLMSKSEQX/BB),((lv)+1)&~(0xffffL<<(DICLMSKWRX-DICLMSKSEQX)),__ATOMIC_RELEASE); scafRWC(dic);   // remove write request and advance owner sequence# (suppressing overflow)
// candyass designers don't allow mixing atomic of different sizes
// We start a write sequence by putting up the prewrite rewuest and waiting to become the lead writer.  If there is a resize, which happens only after we have become the lead writer and acquired a write lock,
// we keep the same lock throughout the resize, and we use the existence of that lock as a flag to suppress requesting the write lock again.  That in turn means that we must ignore any write lock that we see before we have
// become the lead writer.  We just mask it off here on the initial prewrite response.  When we become lead writer we can be sure the lock is clear.
#define DICLKRWRQ(dic,lv,cond) (lv=!(cond)?__atomic_fetch_add(&AM((A)dic),LOWESTBIT(DICLMSKRW),__ATOMIC_ACQ_REL)&~(DICLMSKWRK+DICLMSKWRV):DICLSINGLETHREADED)   // put up prewrite request and read status, which establishes our sequence# in lv where it will remain until we have become lead writer.
    // the only thing valid in lv are our sequence# (MSKRW) and the current lead writer (MSKSEQ)
#define DICLKRWWT(dic,lv) if(likely((lv&DICLMSKWRK+DICLMSKWRV)==0))if(unlikely((((LKRW(lv)^LKSEQ(lv)))&DICLMSKSEQ))!=0)diclkrwwt(dic,lv);  // wait until we are the lead writer; but if we already have a lock,
       // we must be in resize; our lv may be stale then so we keep the old lock
// once we become the lead writer we know that no one else will change the lead writer, so we can safely update lv from the global
#define DICLKWRRQ(dic,lv) if(likely((lv&DICLMSKWRK+DICLMSKWRV)==0))lv=__atomic_add_fetch(&AM((A)dic),DICLMSKWRK+DICLMSKWRV,__ATOMIC_ACQ_REL);   // put up write request for key & value (we are known to be the lead writer and own the write bits, which must be clear), & set those bits in lv
#define DICLKWRWTK(dic,lv) if(unlikely(lv&DICLMSKRDK))lv=diclkwrwt(dic,0);  // wait until all reads have finished looking at keys.  If single-threading, test fails
   // we update that part of lv we don't control, so that the ultimate CAS will have a better chance of starting with the right value
#define DICLKCKSTAT(dic) __atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE)   // (optional) refetch status before we have to check it
#define DICLKWRWTV(dic,lv) if(unlikely(((LKRDV(lv)^LKRDK(lv))&0x7fff))!=0)lv=diclkwrwt(dic,-1);  // wait until all reads have finished looking at values.  RWWTK required before, so we know we have a lock on the keys, so any read-lock requests are transient  If single-threading, test fails
   // we update that part of lv we don't control, so that the ultimate CAS will have a better chance of starting with the right value
#define DICLKWRRELK(dic,lv) if((lv&DICLMSKWRV)!=0)lv=__atomic_sub_fetch(&AM((A)dic),DICLMSKWRK,__ATOMIC_ACQ_REL);   // we have keys locked; if values locked too (i. e. not single-threaded), release the keys, while still writing to values
   // we update that part of lv we don't control, so that the ultimate CAS will have a better chance of starting with the right value
// obsolete #define DICLKWRRELV(dic,lv) {__atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKSEQX,__ATOMIC_ACQ_REL); __atomic_fetch_and(&AM((A)dic),~((I)0xffff<<DICLMSKWRX),__ATOMIC_ACQ_REL);}   // advance owner sequence#, then clear write req (which handles overflow on the seq#)
#define DICLKWRRELV(dic,lv) if((lv&DICLMSKWRV)!=0){UI nv; do{nv=((lv&~DICLMSKWRK)+1)&~(DICLMSKWRK+DICLMSKWRV+DICLMSKOKRET+DICLMSKRESIZEREQ);}while(!casa(&AM((A)dic), &lv, nv));}    // if not single-threaded, advance owner sequence# (suppressing overflow), clear write req

// scaf should these founctions yield after a while?
// wait for read lock on keys+values in dic, which we have requested & found busy
static UI diclkrdwtkv(DIC *dic, UI lv){I n;
 // We know we just put up a read request and saw busy.  Rescind our read request and then quietly poll for the write to go away
 do{
  DICLKRDRELKV(dic,lv)  // remove read request
  for(n=5;;--n){
   delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
   if((__atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE)&DICLMSKWRK+DICLMSKWRV)!=DICLMSKWRK+DICLMSKWRV)break;  // lightweight wait until resource is available
  }
 }while(((DICLKRDRQ(dic,lv,0))&DICLMSKWRK+DICLMSKWRV)==DICLMSKWRK+DICLMSKWRV);  // put up our rd request again, wait for not-busy.  Usually succeeds
 R lv;
}

// wait for read lock on values in dic, which we have requested & found busy.
// We have a read lock already, so no RFO is required.  We just wait for the values to be ready
static UI diclkrdwtv(DIC *dic){I n;UI lv;
 for(n=5;;--n){
  delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
  if(((lv=__atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE))&DICLMSKWRK+DICLMSKWRV)!=DICLMSKWRV)break;  // lightweight wait until resource is available
 }
 R lv;
}

// obsolete // we have a read lock on keys in dic; wait for values to be available, keeping the lock the whole time
// obsolete static void diclkrdwtv(DIC *dic){I n;
// obsolete  // We know we just put up a read request and saw a writer.  Rescind our read request and then quietly poll for the write to go away
// obsolete  do{
// obsolete   for(n=5;;--n){
// obsolete    delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
// obsolete    if(__atomic_load_n((US*)((I)&AM((A)dic)+DICLMSKWRX/BB),__ATOMIC_ACQUIRE)==0)break;  // wait until write is complete
// obsolete   }
// obsolete  }while(((DICLKRDRQ(dic,n))&((I)2<<DICLMSKWRX))!=0);  // put up our rd request again, wait for any write to finish with keys and values
// obsolete  R;
// obsolete }
// obsolete 
// obsolete 
// wait for prewrite lock on dic, which is granted when we know no other writer is going to modify keys
static UI diclkrwwt(DIC *dic, UI lv){I n;
 // We know we just put up a read-write request and saw that we were not the first read-write requester.  Wait till we our seq# makes it to the top.  At that point we can change only flags in empties/tombstones, with no further action needed
 UI ourseq=LKRW(lv)&0x7fff;  // get our sequence#.  When the current sequence# matches and writes have cleared, we have the resource
 UI curseq=LKSEQ(lv)&0x7fff;  // next sequence# to grant, and the write-request bits
do{
  delay((US)ourseq-(US)curseq>1?50:20);  // delay a bit.  the long delay uses mm_pause.  We use it when we are not the next requester
  curseq=LKSEQ((lv=__atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE)))&0x7fff;  // refresh write bits\sequence#
 }while(ourseq!=curseq);   // exit when we get to the top
 R lv;
}

// wait for write lock on keys and values in dic.  We have put up a write request.  If type is 0, we wait for key-reqs to go away.  If type is -1, we
// already have a lock on the keys and we wait for NET value-reqs to go away
static UI diclkwrwt(DIC *dic,I type){I n;
 // We are the next writer.  We just wait for read requests to clear.  We could improve this my adapting the wait to the usage pattern
 for(n=5;;--n){UI lv;
  delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
  lv=__atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE);  // get current value
  if((((LKRDV(lv)&type)^LKRDK(lv))&0x7fff)==0)R lv;  // wait till requests clear (keys or keys^values depending on type)
 }
}


#else

#define DICLKRDRQ(dic,lv,cond) (lv=0)
#define DICLKRDWTK(dic,lv)
#define DICLKRDRELK(dic,lv)
#define DICLKRDWTV(dic,lv)
#define DICLKRDRELV(dic,lv)
#define DICLKRDRELKV(dic,lv)

#define DICLKRWRQ(dic,lv,cond) lv=0;
#define DICLKRWWT(dic,lv)
#define DICLKWRRQ(dic,lv)
#define DICLKWRWTK(dic,lv)
#define DICLKCKSTAT(dic,lv)
#define DICLKWRWTV(dic,lv)
#define DICLKWRRELK(dic,lv)
#define DICLKWRRELV(dic,lv)
#endif

// **************************************************** resize ***************************************
// Request resize, in the dic locale
A dicresize(DIC *dic,J jt){
 A *_ttop=jt->tnextpushp;  // stack restore point
  // call dicresize in the locale of the dic.  No need for explicit locative because locale is protected by dic
 A nam, val;
 A savjtg=jt->global; SYMSETGLOBAL(dic->bloc.locale);  // push global, and move to locale.  We know resize is explicit verb so we don't need to worry about locals
 ASSERTGOTO(((nam=nfs(6,"resize",0))!=0)
  &&((val=jtsyrd1((J)((I)jt+NAV(nam)->m),NAV(nam)->s,NAV(nam)->hash,jt->global))!=0  // look up name in current locale and ra() if found
  &&((val=QCWORD(namerefacv(nam,QCWORD(val))))!=0)   // turn the value into a reference, undo the ra
  &&((val&&LOWESTBIT(AT(val))&VERB))),EVVALUE, exit)   // make sure the result is a verb
 A newdica; RZ(newdica=jtunquote(jt,(A)dic,val,val)); DIC *newdic=(DIC*)newdica;  // execute resize on the dic, returning new dic
 struct Dic t=dic->bloc; dic->bloc=newdic->bloc; newdic->bloc=t;  // Exchange the parms and data areas from the new dic to the old.  Since they are recursive, this exchanges ownership and will cause the old blocks to be freed when the new dic is.
 // NOTE: we keep the old blocks hanging around until the new have been allocated.  This seems unnecessary for the hashtable, but we do it because other threads still have the old pointers and may prefetch from
 //  the old hash.  This won't crash, but it might be slow if the old hash is no longer in mapped memory
 if(dic->bloc.flags&DICFRB){
  UI nhold=2*AS(newdic->bloc.hash)[0]; C *iv=CAV(newdic->bloc.hash), *ov=CAV(dic->bloc.hash); I il=newdic->bloc.hashelesiz, ol=dic->bloc.hashelesiz;  // number of half-tree-nodes to copy, including hidden ones; addr, len
  DO(nhold, *(UI4*)ov=_bzhi_u64(*(UI4*)iv,il<<LGBB); iv+=il; ov+=ol;)   // copy all the old nodes, zero-extending if needed.  Fails on a downsize!
  // for trees, we copy the old tree into the new one because it's still a valid prefix of the new larger tree and recreating it is slow.  The tree element may have changed size!
  // we must then copy the keys and values too, which is tricky because of empties.  To avoid looking at atoms we copy the keys and values en bloc and then switch the type of the old kv to INT, in effect freeing them one tme
  I kk=bplg(AT(newdic->bloc.keys)), kn=AN(newdic->bloc.keys), vk=bplg(AT(newdic->bloc.vals)), vn=AN(newdic->bloc.vals);  // kv, #atom & size of atom
  MC(voidAV(dic->bloc.keys), voidAV(newdic->bloc.keys), kn<<kk); AT(newdic->bloc.keys)=INT;   // copy & abandon the old keys, transferring ownership
  MC(voidAV(dic->bloc.vals), voidAV(newdic->bloc.vals), vn<<vk); AT(newdic->bloc.vals)=INT;   // copy & abandon the old values, transferring ownership
  UI nkold=AS(newdic->bloc.keys)[0]; dic->bloc.emptyn=nkold-1;  // the end of the free chain is always the last key, which is never allocated.  Point that to the first new key and make it the head of the free chain
  UI t=nkold; WRHASH1234(t, dic->bloc.emptysiz, &CAV(dic->bloc.keys)[(nkold-1)*newdic->bloc.kbytelen])  // hang the excess new empty chain off the old

 }
 // for hashtables, we let the resize function make the copy, to work for downsizing.  We have to recreate the hashtable anyway.

exit:;  // clean up from error
 tpop(_ttop);  // discard newdic and everything it refers to
 SYMSETGLOBAL(dic->bloc.locale);  // restore globals
 R (A)dic;  // always return the same block if no error.
}

// ************************************************ user compare function *****************************

// kl is keylength in bytes, ku is void* 'pointer' to key, kh is 'pointer' to key in keys array
// if we are calling a user compare function, the pointers are actually offsets and we use faux virtual blocks for the call
#define keysne(kl,kh,ku,cond,exit) (likely(cond)?((I (*)(I,void*,void*,J))*cf)(kl,kh,ku,jt) :  /* internal compare function */ \
 ({ AK((A)virt.u)=(I)ku; AK((A)virt.h)=(I)kh; A ka; RZGOTO(ka=((A (*)(J,A,A,A))*cf)(jt,(A)virt.h,(A)virt.u,virt.self),exit) likely(AN(ka))?BIV0(ka):0; }) )  /* user compare function */

typedef struct {
// obsolete I __attribute__((aligned(CACHELINESIZE))) u[16-AKXR(0)/SZI];   //  block used to access user's key
// obsolete I __attribute__((aligned(CACHELINESIZE))) h[16-AKXR(0)/SZI];   // block used to access key from hash
I __attribute__((aligned(CACHELINESIZE))) h[16];   // block used to access key from hash/tree - big enough for rank 9
I __attribute__((aligned(CACHELINESIZE))) u[16];   //  block used to access user's key - big enough for rank 9
A self;   //
} VIRT;

// fill in virtual block to hold one key
#define initvirt(v,dic) {AFLAG(v)=AFUNINCORPABLE+AFRO; AT(v)=dic->bloc.ktype; AN(v)=dic->bloc.kaii; AC(v)=ACPERMANENT; AR(v)=AN(dic->bloc.kshape); MCISH(AS(v),IAV1(dic->bloc.kshape),AN(dic->bloc.kshape));}

#define biasforcomp {k=(void *)((I)k-(I)&virt.u); kbase=(void *)((I)kbase-(I)&virt.h); }   // bias k and kbase back so that their values can be used in AK of virt.u and virt.v
#define unbiasforcomp {k=(void *)((I)k+(I)&virt.u); kbase=(void *)((I)kbase+(I)&virt.h); }   // restore original pointers

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- unordered map (hashed) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

// Macros to install/delete kvs or unload values, which have extra work to do for indirect types.  ind is a condition that is non0 for indirect type
#define PUTKVOLD(d,s,n,ind) {if(ind){A *dv=(A*)(d); A *sv=(A*)(s); A sa=sv[0]; DO((n)>>LGSZI, A sa1=sv[i+1]; A da=dv[i]; ra(sa) dv[i]=sa; if(likely(da!=0))fa(da) sa=sa1;)}else{MC(d,s,n);}}   // when old kv might exist
#define PUTKVNEW(d,s,n,ind) {if(ind){A *dv=(A*)(d); A *sv=(A*)(s); A sa=sv[0]; DO((n)>>LGSZI, A sa1=sv[i+1]; ra(sa) dv[i]=sa; sa=sa1;)}else{MC(d,s,n);}}  // when old kv are empty
#define GETV(d,s,n,ind) {if(ind){A *dv=(A*)(d); A *sv=(A*)(s); A sa=sv[0]; DO((n)>>LGSZI, A sa1=sv[i+1]; rareccontents(sa) dv[i]=sa; sa=sa1;)}else{MC(d,s,n);}}   // move value to result
#define DELKV(s,n,ind) {if(ind){A *sv=(A*)(s); A sa=sv[0]; DO((n)>>LGSZI, A sa1=sv[i+1]; fa(sa) sv[i]=0; sa=sa1;)}}   // deleting old kv

// k is A for keys, n is #keys, s is place for slot#s.  Hash each key, store, prefetch (possibly using wrong hash)
// if s is 0, we are using a user hash; use uits return area as s
// result is 0 on error
static INLINE I8* jtkeyprep(DIC *dic, void *k, I n, I8 *s,J jt,A ka){I i;
 UI8 hsz=dic->bloc.hashsiz; UI8 kib=dic->bloc.klens; UI4 (*hf)(void*,I,J)=dic->bloc.hashfn; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen
 if(likely(s!=0)) {
  // internal hash
  k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down.  This puts prefetches in the right order for get, wrong for put.  Pity.
  for(i=n;--i>=0;){
   k=(void*)((I)k-(kib>>32));  // back up to next key
   s[i]=(I8)(*hf)(k,(UI4)kib,jt); PREFETCH(&hashtbl[(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56)]);
  }
 }else{
  // user hash.  Call their function
  A h; RZ(h=((A (*)(J,A,A,A))*hf)(jt,ka,dic->bloc.hashcompself,dic->bloc.hashcompself))  // hash everything at once to avoid call overhead
  RZ(h=mkwris(h)) ASSERT(AN(h)==n,EVLENGTH) if(unlikely(!(AT(h)&INT)))RZ(h=ccvt(INT,h,0)) s=(I8*)IAV(h);  // set s to writable block
  if(!SY_64){A x; GATV0(x,FL,n,1) DO(n, I8AV1(x)[i]=((UI*)s)[i];) s=I8AV1(x);}  // s must be I8s
 }
 R s;
}
// k is A for keys, kvirt is virtual unincorpable block to point to key, s is place for slot#s.  Hash each key, convert to slot, store, prefetch

// ********************************** get/has **********************************

// Do the work for just 1 get. z is 0 for has
static INLINE A jtget1(DIC *dic,void *k,A z,J jt,A adyad){
 UI8 hsz=dic->bloc.hashsiz; UI8 kib=dic->bloc.klens; UI4 (*hf)(void*,I,J)=dic->bloc.hashfn;  // elesiz/hashsiz kbytelen/kitemlen hash function
 UI4 hsh=(*hf)(k,(UI4)kib,jt); PREFETCH(&CAV1(dic->bloc.hash)[(((UI8)hsh*(UI4)hsz)>>32)*(hsz>>56)]);
 UI lv; DICLKRDRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request read lock
 I (*cf)(I,void*,void*)=dic->bloc.compfn; I vn=dic->bloc.vbytelen;   // more nonresizable: kbytelen/kitemlen   compare fn   size of a value in bytes 
 DICLKRDWTK(dic,lv)   // wait for read lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz  prototype required to get arg converted to I
 C *kbase=CAV(dic->bloc.keys);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array

 // convert the hash slot#s to index into kvs.  Prefetch each value to get a leg up on moving the data.  We don't prefetch more to avoid needless bus bandwidth; perhaps we should check the length   scaf
    // the only advantage of the prefetch is that the value reads will clear earlier, allowing the fence when we end our lock to finish earlier
 I curslot=(((UI8)hsh*(UI4)hsz)>>32)*(hsz>>56); I hval; // convert hash to slot# and then to byte offset;  place where biased kv slot# is read into
 while(1){
  hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv
  if(withprob(hval<HASHTSTN,0.3))goto notfound;   // if we hit an empty (incl birthstone empty but not a tombstone), that ends the search (not found)
  if(withprob((hval-=HASHNRES)>=0,0.6) && withprob(((I (*)(I,void*,void*,J))*cf)((UI4)kib,k,kbase+(kib>>32)*hval,jt)==0,0.7))goto found;  // if we hit a non-tombstone that matches the key, exit found
  // here not found
  if(unlikely((curslot-=(hsz>>56))<0))curslot+=(UI4)hsz*(hsz>>56);  // move to next hash slot, wrap to end if we hit 0
 }
 void *vv;  // the value to copy, from either the value table or the default
found:;  // hval is the unbiased slot that was found
 if(z==0){z=num(1); goto retz;}  // for has, return boolean 1
 vv=CAV(dic->bloc.vals)+hval*vn;  // set copy-from address to the found value 
 if(0){
notfound:;
  if(z==0){z=num(0); goto retz;}  // for has, return boolean 0
  // get: return the default, after auditing it.
  ASSERTGOTO(adyad!=0,EVINDEX,exiterr)   // error if no default given
  ASSERTGOTO(AR(adyad)==AN(dic->bloc.vshape),EVRANK,exiterr) ASSERTGOTO(TESTAGREE(AS(adyad),IAV1(dic->bloc.vshape),AR(adyad)),EVLENGTH,exiterr)  // verify default has correct rank & shape
  if(unlikely((AT(adyad)&dic->bloc.vtype)==0)){RZGOTO(adyad=ccvt(dic->bloc.vtype,adyad,0),exiterr)}   // convert precision if needed
  vv=CAV(adyad);  // get address of values to move
  PRISTCLRF(adyad);  // Since we are taking out of a, it is no longer pristine.  this destroys a, but leaves it non0.
 }
 GETV(CAV(z),vv,dic->bloc.vbytelen,hsz&(DICFVINDIR<<DICFBASE));   // move the data & advance pointer to next one   scaf JMC?
retz:;   // release lock & return, with z set
  DICLKRDRELKV(dic,lv)  // remove lock on keys and values.  Not worth it to release keys early
  R z;
exiterr:; z=0; goto retz;
}

// k is A for keys, s is slot#s, zv is result area (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy the value (or default) to the result
// a is default if given, 0 if not; or 1 if has
// We take a read lock on the table and return with it
static INLINE B jtgetslots(DIC *dic,void *k,I n,I8 *s,void *zv,J jt,A a, VIRT virt){I i;
 UI lv; DICLKRDRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request read lock
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn; I vn=dic->bloc.vbytelen;   // more nonresizable: kbytelen/kitemlen   compare fn   size of a value in bytes 
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 DICLKRDWTK(dic,lv)   // wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 UI8 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz  prototype required to get arg converted to I
 C *kbase=CAV(dic->bloc.keys);  // address of first stored key.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals);  // address of first stored value.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the value array

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp

 // convert the hash slot#s to index into kvs.  Prefetch each value to get a leg up on moving the data.  We don't prefetch more to avoid needless bus bandwidth; perhaps we should check the length
    // the only advantage of the prefetch is that the value reads will clear earlier, allowing the fence when we end our lock to finish earlier
 for(i=n;--i>=0;){
  k=(void*)((I)k-(kib>>32));  // back up to next key
  I curslot=(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56); I hval; // convert hash to slot# and then to byte offset;  place where biased kv slot# is read into
  while(1){
   s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53))-HASHNRES;   // point to field beginning with hash value, clear higher bits. remember the unbiased hash value, which will be the index of the kv
   if(withprob(hval>=0,0.6)){if(withprob(keysne((UI4)kib,kbase+(kib>>32)*hval,k,hsz&(DICFICF<<DICFBASE),exitkeyvals)==0,0.7)){PREFETCH(vbase+s[i]*vn); break;}}  // if we hit a non-tombstone that matches the key, exit found
   else if(withprob(hval<HASHTSTN-HASHNRES,0.3))break;   // if we hit an empty (incl birthstone empty but not a tombstone), that ends the search (not found)
   // here not found
   if(unlikely((curslot-=(hsz>>56))<0))curslot+=(UI4)hsz*(hsz>>56);  // move to next hash slot, wrap to end if we hit 0
  }
 }

 
 // copy using the kv indexes we calculated.  Copy in ascending order so we can overstore    scaf overstore
 if(a==(A)1){DICLKRDRELKV(dic,lv) DO(n, ((C*)zv)[i]=s[i]>=0;)   // if processing has, just copy found status, then release lock
 }else{
  DICLKRDWTV(dic,lv) DICLKRDRELK(dic,lv) // We have finished our use of the keys, but we must wait till the values are safe to copy
  void *av=0;  // init to 'no default data pointer yet'.  We avoid checking the default until we know we need it
  I cur=s[0];  // unroll loop once.  cur if current value to work on
  for(i=0;i<n;++i){void *vv;  // pointer to value to move
   I nxt=s[i+1];  // fetch next value.  Overfetch OK
   if(likely(cur>=0))vv=vbase+cur*vn;   // this is the main line, key found
   else{   // default required, which we deem rare.
    if(unlikely(av==0)){
     // First time through here. We have to audit it & convert if needed
     ASSERTGOTO(a!=0,EVINDEX,exitvals)   // error if no default given
     ASSERTGOTO(AR(a)==AN(dic->bloc.vshape),EVRANK,exitvals) ASSERTGOTO(TESTAGREE(AS(a),IAV1(dic->bloc.vshape),AR(a)),EVLENGTH,exitvals)  // verify default has correct rank & shape
     if(unlikely((AT(a)&dic->bloc.vtype)==0)){RZGOTO(a=ccvt(dic->bloc.vtype,a,0),exitvals)}   // convert precision if needed
     av=CAV(a);  // get address of values to move
     PRISTCLRF(a);  // Since we are taking out of a, it is no longer pristine.  this destroys a, but leaves it non0.
    }
    vv=av;    // use the default after it has been audited
   }
   GETV(zv,vv,vn,hsz&(DICFVINDIR<<DICFBASE)); zv=(void *)((I)zv+vn);   // move the data & advance pointer to next one   scaf JMC?
   cur=nxt;
  }
  DICLKRDRELV(dic,lv)  // remove lock on values
}
 R 1;
exitvals:; DICLKRDRELV(dic,lv) R 0;
exitkeyvals:;  DICLKRDRELKV(dic,lv) R 0;
}

//   get/has.   Bivalent. w is keys, [a] is default value, 0 if monad.  Called from parse/unquote as a,w,self or w,self,self.  dic was u to self
static DF2(jtdicget){F12IP;A z;
 ARGCHK2(a,w)
 A adyad=w!=self?a:0; w=w!=self?w:a;  // if dyad, keep a,w, otherwise 0,w
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 I t=dic->bloc.vtype; A sa=dic->bloc.vshape; I vaii=dic->bloc.vaii; t=FAV(self)->localuse.lu1.varno==0?t:B01; sa=FAV(self)->localuse.lu1.varno==0?sa:mtv; vaii=FAV(self)->localuse.lu1.varno==0?vaii:1; adyad=FAV(self)->localuse.lu1.varno==0?adyad:(A)1;  // type/shape of output, for get or has
 ASSERT((FAV(self)->localuse.lu1.varno|dic->bloc.vbytelen)!=0,EVDOMAIN)   // get not allowed when values are empty (only has)
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))   // convert type of w if needed
 if((wf+((dic->bloc.flags&DICFIHF+DICFICF)^(DICFIHF+DICFICF)))==0){
  // fast path: no frame, and no user functions
  z=0; if(adyad!=(A)1){GA0(z,t,vaii,AN(sa)) AFLAG(z)=t&RECURSIBLE; MCISH(AS(z),IAV1(sa),AN(sa))}   // for get, allocate recursive result area & install shape; for has, result will be constant
  z=jtget1(dic,voidAV(w),z,jt,adyad);  // get the result
 }else{
  // general case, including multiple keys
  if(unlikely(AN(w)==0)){A avalues=dic->bloc.vals; avalues=FAV(self)->localuse.lu1.varno==0?avalues:mtv; R reitem(drop(sc(-AN(dic->bloc.vshape)),w),avalues);}  // if no keys, return empty fast   ((-kshape) }. $w) $ values/''
  I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
  ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index

  GA0(z,t,kn*vaii,wf+AN(sa)) AFLAG(z)=t&RECURSIBLE; MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,IAV1(sa),AN(sa))   // allocate recursive result area & install shape
  // establish the area to use for s.  To avoid wasting a lot of stack space we use the virt-block area if that is not needed for user comp.  And if there is a user hash, we allocate
  // nothing & use the value returned by keyprep, which will be the result area from the user hash.
  VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer
  if(unlikely(!(dic->bloc.flags&DICFIHF)))s=0;   // user hash function, keyprep will allocate s
  else if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
  else{A x; GATV0(x,FL,kn,1) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
 
  void *k=voidAV(w);  // point to the key data
  RZ(s=jtkeyprep(dic,k,kn,s,jt,w))  // convert keys to slot#, possibly changing s
  if(!jtgetslots(dic,k,kn,s,voidAV(z),jt,adyad,virt))z=0;  // get the values and take a read lock on the dic.  If error, pass error through
 }
 RETF(z);
}


// ********************************** put **********************************

// put fast case: one key, no user hash or compare function.  We enter having requested a pre-write lock.
// Result is 0 on error, otherwise is the current lv with DICLMSKOKRET set and DICLMSKRESIZEREQ set if resize is needed
static INLINE UI8 jtput1(DIC *dic,void *k,void *v,UI8 lv,J jt){
 UI8 hsz=dic->bloc.hashsiz; UI8 kib=dic->bloc.klens; UI4 (*hf)(void*,I,J)=dic->bloc.hashfn; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen
 UI4 hsh=(*hf)(k,(UI4)kib,jt); PREFETCH(&hashtbl[(((UI8)hsh*(UI4)hsz)>>32)*(hsz>>56)]);
 // hash the key and prefetch from the hashtable
 I (*cf)(I,void*,void*)=dic->bloc.compfn;   // kbytelen/kitemlen  compare func unchanged by resize
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
    // with this lock we can add new kvs, or change an empty/tombstone to a birthstone; but no other hash changes, and no value overwrites
 DICLKWRRQ(dic,lv);  // request full write lock on k+v

 hsz=dic->bloc.hashsiz; hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz  base of hashtbl may have been changed by resize
 C *kbase=CAV(dic->bloc.keys);  // key 0 address for empties; Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals);  // pointer to values

 // Look up the key in the hashtable
 I curslot=(((UI8)hsh*(UI4)hsz)>>32); I tomb1=-1; I hval; //  convert hash to slot#;  first tombstone (init none);place to read biased kv slot# into
 while(1){
  I bhval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. this is the (biased) hashndx
  if(withprob((hval=bhval-HASHNRES)<0,0.3)){   // unbiased slot addr, in case found.  If hole or tombstone
   tomb1=tomb1<0?curslot:tomb1;  // remember first spot we can store into, empty or tombstone, and its type
   if(withprob(bhval<HASHTSTN,0.8))goto notfound;  // if we hit empty, we're done: mark first hole is birthstone; save its status before mark.  1 byte store for atomicity
  }else{if(withprob(((I (*)(I,void*,void*,J))*cf)((UI4)kib,k,kbase+(kib>>32)*hval,jt)==0,0.7))goto found;}  // if key match, we're done: we have saved the hashslot in s[] with high 32 bits 0
  // no match.  try next hashslot
  if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
 }  // never fall through
notfound:;  // tomb1 has the first slot we can store into
 DICLKWRWTK(dic,lv)  // wait for keys to be modifiable.  No resize is possible.
 // search ends at empty/tombstone, in hashslot (curslot).  Allocate a new empty, point curslot to it, move in the key, save the slot in s[cur] which is the first empty/tombstone found
   // in case of resize we have to keep the empty chain valid after each addition, since it is 
 hval=dic->bloc.emptyn;   // first empty
 I emptynxt=_bzhi_u64(*(UI4*)&kbase[hval*(kib>>32)],(hsz>>53));  // get next empty in chain
 if(unlikely(emptynxt==hval))goto resize;   // chain to self - indicates end of empties - must abort to resize
 dic->bloc.emptyn=emptynxt;   // save new root of empty chain
 emptynxt=HDECEMPTY(hval); WRHASH1234(emptynxt, hsz>>56, &hashtbl[tomb1*(hsz>>56)])   //  convert empty# to hashslot#; set hashtable to point to new kv (skipping over reserved hashslot#s)
 PUTKVNEW(kbase+hval*(kib>>32),k,kib>>32,hsz&(DICFKINDIR<<DICFBASE));    // copy the new key
 dic->bloc.cardinality++;  // account for the new key
 goto copyval;
 // fall through...
found:;  // hval is the kv slot we compared with, or a new empty kv slot
 DICLKWRWTK(dic,lv)  // wait for keys to be modifiable.  No resize is possible.  We have to wait for keys before we can wait for values.
 copyval:;
 DICLKWRWTV(dic,lv) DICLKWRRELK(dic,lv)  // wait for values to be modifiable, then release keys (questionable decision).  Return with lock on values
 I vb=dic->bloc.vbytelen; 
 PUTKVOLD(vbase+hval*vb,v,vb,hsz&(DICFVINDIR<<DICFBASE)); // copy the value
// obsolete  for(cur=croot;cur>=0;){I nxt=((I4*)(&s[cur]))[1]; UI kvslot=((UI4*)(&s[cur]))[0]; PUTKVOLD(vbase+kvslot*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE)); cur=nxt;} // chase the conflict keys, copying the value
 R lv|DICLMSKOKRET;  // good return, holding write lock on values or values/keys

resize:;  // resize required.  We already have a write lock
 R lv|DICLMSKOKRET+DICLMSKRESIZEREQ;  // signal need for resize; return holding write lock on keys & values
}


// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; return value of lv (to make release faster in caller), with RET bits set to indicate error or resize request
static INLINE UI8 jtputslots(DIC *dic,void *k,I n,void *v,I vn,I8 *s,J jt,UI lv,VIRT virt){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;   // kbytelen/kitemlen  compare func unchanged by resize
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
    // with this lock we can add new kvs, or change an empty/tombstone to a birthstone; but no other hash changes, and no value overwrites

 UI8 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz  base of hashtbl
// obsolete  C *kebase=CAV(dic->bloc.keys), *kbase=kebase-HASHNRES*(kib>>32);  // ket 0 address for empties; address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 C *kbase=CAV(dic->bloc.keys);  // key 0 address for empties; address corresponding to hash value of 0.
 C *vbase=CAV(dic->bloc.vals);  // pointer to values

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp

 // first pass over keys.  If key found, remember the biased kv# (will go to old chain).  If not found, remember the hashslot# and whether it was occupied by a birthstone; and make it a birthstone - will go to new or conflict chain
 // If we encounter a birthstone, that's a conflict.  We must remember the first tombstone we encounter because that will be the store point if we don't find a match
 // We search all the way to a hole so that if there is match we will find it before we take the write lock.  This has the benefit of speeding up the conflict search
 // We have to go in ascending order because later keys must overwrite earlier ones
 for(i=0;i<n;++i){
  I8 tomb1=~0; I8 curslot=(((UI8)s[i]*(UI4)hsz)>>32); I hval; // first tombstone (init none); convert hash to slot#;  place to read biased kv slot# into
  while(1){
   I bhval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. this is the (biased) hashndx
   if(withprob((hval=bhval-HASHNRES)<0,0.3)){   // unbiased slot addr, in case found.  If hole or tombstone
    I8 hc=((I8)bhval<<32)+curslot; tomb1=tomb1==~0?hc:tomb1;  // remember first spot we can store into, empty or tombstone, and its type
    if(withprob(bhval<HASHTSTN,0.8)){hval=tomb1>>32; tomb1=(UI4)tomb1; hashtbl[tomb1*(hsz>>56)]=hval|HASHBSTN; s[i]=((I8)0x100000000<<hval)+tomb1; break;}  // if we hit empty, we're done: mark first hole is birthstone; save its status before mark.  1 byte store for atomicity
   }else{s[i]=hval; if(withprob(keysne((UI4)kib,kbase+(kib>>32)*hval,(void*)((I)k+i*(kib>>32)),hsz&(DICFICF<<DICFBASE),errexit)==0,0.7))break;}  // if key match, we're done: we have saved the hashslot in s[] with high 32 bits 0
   // no match.  try next hashslot
   if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
  }
 }
 // new|conflict: s[i]=hole mask\hole hashndx (type is 1<<hole value)   old: s[i]=0\unbiased kv index of matching key   

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))unbiasforcomp
 // pass slot# back to front, putting them into 3 chains: new kvs, old keys, conflict keys (each in front-to-back order).  Conflict keys are assumed vary rare (usually repeated keys)
 I nroot=-1, oroot=-1, croot=-1, otail=0, nnew=0;;   // base of the 3 chains, inited to empty; tail of old chain, inited to 'not found yet' (safe even if result turns out to be 0); counter of new blocks
 DQ(n, I sh=((UI4*)(&s[i]))[1]; if(unlikely(sh&((1LL<<HASHBSTN)+(1LL<<(HASHTSTN+HASHBSTN))))){((UI4*)(&s[i]))[1]=croot; croot=i;}else{((I4*)(&s[i]))[1]=sh?nroot:oroot; nroot=sh?i:nroot; oroot=sh?oroot:i; nnew+=sh!=0; otail=sh|otail?otail:i;})
   // conflict if BSTN was set; otherwise old if high part of s[i]=0 

 // If the operation is going to run out of slots, abort before we start.
 if(unlikely(dic->bloc.cardinality+nnew>=(UI)AS(dic->bloc.keys)[0]))goto lockforresize;  // if this would fill the last key, resize
 dic->bloc.cardinality+=nnew;  // account for the new keys

 // Now we move the new kvs into empty slots.  This wipes out the chain fields in the empties, so we have to create a separate list of where the empties were
 UI4 empty64[64][2]; UI4 (*empties)[][2]=(UI4 (*)[][2])empty64; if((UI)nnew>sizeof(empty64)/sizeof(empty64[0])){A ea; GATV0E(ea,FL,nnew,0,goto errexit) empties=(UI4 (*)[][2])DAV0(ea);}    // holding area for our empty list

 // chase the new kvs, taking an empty slot for each and copying the k and v into the tables. s[] holds chain\hashslot
 I vb=dic->bloc.vbytelen; UI emptyx=dic->bloc.emptyn; I cur;  // empty area & current pointer into it.  Length of a element is hsz>>56
 for(nnew=0,cur=nroot;cur>=0;){
  I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  (*empties)[nnew][0]=((I4*)(&s[cur]))[0]; (*empties)[nnew][1]=emptyx; ++nnew;  // save (hashslot we are about to use),(kv index to put into the hashslot)
  UI emptynxt=_bzhi_u64(*(UI4*)&kbase[emptyx*(kib>>32)],(hsz>>45));  // get next empty, which must exist
  PUTKVNEW(kbase+emptyx*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32,hsz&(DICFKINDIR<<DICFBASE))
; PUTKVNEW(vbase+emptyx*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE));  // not worth testing to skip empty values
  cur=nxt; emptyx=emptynxt;  // advance to next
 }
 dic->bloc.emptyn=emptyx;  // We have allocated all the new blocks successfully.  Update the free root, freeing the old blocks

 DICLKWRRQ(dic,lv); DICLKWRWTK(dic,lv)  // request write lock and wait for keys to be modifiable.  No resize could be happening.

 // Now that we can freely modify the hash, put the biased hash index of the kv into the hash
 DQ(nnew, UI4 t=HDECEMPTY((*empties)[i][1]); WRHASH1234(t, hsz>>56, &hashtbl[(*empties)[i][0]*(hsz>>56)]))
// obsolete // scaf get hsh from empties
// obsolete  // chase the new kvs again, replacing the birthstone (indexed by s[i]) with the allocated kvslot#, which was the original empty position.  s[] holds chain\hashslot
// obsolete  I newi;  // seq # among the new KVs & empties
// obsolete  for(newi=0,cur=nroot;newi<nnew;++newi){  // cur chases the chain, newi indexes the empties
// obsolete   I8 nxthsh=s[cur];  // fetch chain\hashslot
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
// obsolete   UI emptynxt=_bzhi_u64(*(UI4*)&CAV(dic->bloc.keys)[emptyx*(kib>>32)],(hsz>>45));   // get next empty, abort if end of chain (loopback)
// obsolete   emptyx=HDECEMPTY(empties[newi]); WRHASH1234(emptyx, hsz>>56, &hashtbl[(UI4)nxthsh*(hsz>>56)])  // set hashtable to point to new kv (skipping over reserved hashslot#s)
// obsolete   cur=nxthsh>>32;  // advance to next
// obsolete  }

 // chase the conflict keys (in ascending order), updating everything.  They should be few.  For new kvs, also take an empty slot & update the keys.  Leave values in the conflict chain, in ascending order. s[i] indexes the hashslot
 // Since the original search searched all the way to a hole looking for a match, we know that the only match must come from a previous key in this put.  This key would necessarily have gone into the first tombstone found,
 // so we can stop the search when we hit a tombstone or a hole.
 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp   // first compare, then copy
 for(cur=croot,emptyx=dic->bloc.emptyn;cur>=0;){
  // cur is the current element of conflict chain, emptyx is the root of the empty chain
  I8 nxthsh=s[cur];  // fetch chain\hashslot
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  I curslot=(UI4)nxthsh;  I hval; // slot was originally birthstone where the search ended.  Now it has been filled in, and we resume the search there.  It could match right there.  hval is biased kv slot# from hashtable
  while(1){
   hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
   if((hval-=HASHNRES)<0)goto notfound;  // if we hit a hole (birthstones have been filled), go allocate a new kv
   if(keysne((UI4)kib,kbase+(kib>>32)*hval,(void*)((I)k+cur*(kib>>32)),hsz&(DICFICF<<DICFBASE),errexit)==0)goto found;   // if key matches, hval is the slot
   // no match
   if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
  }
notfound:;
   // search ends at empty/tombstone, in hashslot (curslot).  Allocate a new empty, point curslot to it, move in the key, save the slot in s[cur] which is the first empty/tombstone found
     // in case of resize we have to keep the empty chain valid after each addition, since it is 
   if(unlikely(!(hsz&(DICFICF<<DICFBASE))))unbiasforcomp   // copying coming up
   UI emptynxt=_bzhi_u64(*(UI4*)&kbase[emptyx*(kib>>32)],(hsz>>53));  // get next empty in chain
   if(unlikely(emptynxt==emptyx)){   // chain to self - indicates end of empties
    // no room in the empty chain - resize.  
    if(!(hsz&(DICFVINDIR<<DICFBASE)))goto resizedirect;  // if values are direct, it is OK to leave garbage in them
    s[cur]=-1; oroot=-1;  // indirect values: we must copy new values to keep the slots valid.  end chain and make slot pointer invalid; clear chain of old values
    hsz|=(DICFRESIZE<<DICFBASE);   // indicate resize needed
    break;  // continue with truncated value work
   }
   hval=emptyx; // get empty slot, save as biased kv slot#
   emptyx=HDECEMPTY(hval); WRHASH1234(emptyx, hsz>>56, &hashtbl[(UI4)curslot*(hsz>>56)])   // set hashtable to point to new kv (biased)
   PUTKVNEW(kbase+hval*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32,hsz&(DICFKINDIR<<DICFBASE));    // copy the new key
   dic->bloc.cardinality++;  // account for the new keys
   emptyx=emptynxt;  // advance to next empty now that we have filled one in
   if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp   // key copying finished, back to compares
   // fall through...
found:;  // hval is the kv slot we compared with, or a new empty kv slot
  // copy the values later; save the kv index in s
  ((UI4*)(&s[cur]))[0]=hval;  // remember the unbiased slot the value must be moved into.  This changes s[i] from hashslot index to kv index
  cur=nxthsh>>32;  // advance to next
 }
 // compares finished - no more copying of keys so no need for unbiascomp
 dic->bloc.emptyn=emptyx;   // save new root of empty chain

 if(!(hsz&(DICFRESIZE<<DICFBASE))){DICLKWRWTV(dic,lv) DICLKWRRELK(dic,lv)}    // wait for values to be free (at which point we automatically own them), then allow gets to look at hashtable & keys.  We still have a write lock.  But if we are resizing, leave the full lock

 // we have updated the keys and hash.  Now move the (non-new) values, indexed by s[i] (unbiased).  Every value gets moved once.  We move the old then the conflicts, knowing that any old must precede any conflict that maps to the same slot
 if(vb!=0){   // Skip all value moves if values not empty
// obsolete   vbase-=HASHNRES*vb;   // base pointer to allocated values, backed up to skip over the empty/birthstone/tombstone codes
  ((I4*)(&s[otail]))[1]=croot; oroot=oroot<0?croot:oroot;  // append conflict chain to old chain (if old chain is empty otail=0, which is OK to store into because ele 0 can never be a conflict); start on combined chain
  for(cur=oroot;cur>=0;){I8 nxtkv=s[cur]; if(likely((UI4)nxtkv!=(UI4)~0))PUTKVOLD(vbase+(UI4)nxtkv*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE)); cur=nxtkv>>32;} // chase the old keys, copying the value (except the last during resize)
 }
// obsolete  for(cur=croot;cur>=0;){I nxt=((I4*)(&s[cur]))[1]; UI kvslot=((UI4*)(&s[cur]))[0]; PUTKVOLD(vbase+kvslot*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE)); cur=nxt;} // chase the conflict keys, copying the value
 R lv|DICLMSKOKRET+(hsz&(DICFRESIZE<<DICFBASE)?DICLMSKRESIZEREQ:0);  // return, holding write lock on values or values/keys

lockforresize:; DICLKWRRQ(dic,lv); DICLKWRWTK(dic,lv) goto lockvforresize;  // come here when we detect resize early.  Catch up by requesting a write lock

resizedirect:;  // resize request while processing the conflict list.
 // we moved some of the keys and then ran out of space.  But the values are direct, so we can leave the as is
 dic->bloc.emptyn=emptyx;   // save new root of empty chain
lockvforresize:;  // here when we have waited for keys, but not values yet
 DICLKWRWTV(dic,lv)  // We need a full write lock to resize, so wait till we have keys & values
 R lv|DICLMSKOKRET+DICLMSKRESIZEREQ;  // signal need for resize; return holding write lock on keys & values

errexit: R 0;   // return with error
}

//  put.  a is values, w is keys   dic was u to self
static DF2(jtdicput){F12IP;A z;
 ARGCHK2(a,w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.vtype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 I af=AR(a)-vr; ASSERT(af>=0,EVRANK) ASSERTAGREE(AS(a)+af,vs,vr)   // v must be a single value or an array of them, with correct shape
 UI lv;   // will hold most recent value of lock
 if((af+wf+((dic->bloc.flags&DICFIHF+DICFICF)^(DICFIHF+DICFICF)))==0){  // fast path?
  // put of a single key using internal hash/compare - the fast path
  if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0)) if(unlikely((AT(a)&vt)==0))RZ(a=ccvt(vt,a,0))  // convert type of k and v if needed
  DICLKRWRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
  void *k=voidAV(w), *v=voidAV(a);  // point to the key and value data
  while(1){  // loop till we have processed all the resizes
   lv=jtput1(dic,voidAV(w),voidAV(a),lv,jt); if(lv==0)goto errexit; if(likely(!(lv&DICLMSKRESIZEREQ)))break;  // do the hash & put; abort if error if no resize, finish, good or bad
   if(dicresize(dic,jt)==0)goto errexit;  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
   lv&=~(DICLMSKRESIZEREQ+DICLMSKOKRET);  // remove return flags from lv
  }
 }else{
  // fall through for multiple keys, or user functions
  I cf=MIN(af,wf); ASSERTAGREE(AS(a)+af-cf,AS(w)+wf-cf,cf)  // frames must be suffixes
  if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
  if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0)) if(unlikely((AT(a)&vt)==0))RZ(a=ccvt(vt,a,0))  // convert type of k and v if needed.  Agreement error has priority over type
  I kn; PROD(kn,wf,AS(w)) I vn; PROD(vn,wf,AS(w))   // kn = number of keys to be looked up  vn=#values to be looked up
  ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index
  // We do not have to make incoming kvs recursive, because the keys/vals tables do not take ownership of the kvs.  The input kvs must have their own protection, valid over the call.
  // For the same reason, we do not have to worry about the order in which kvs are added and deleted.

  VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

  DICLKRWRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
  void *k=voidAV(w), *v=voidAV(a);  // point to the key and value data
  while(1){  // loop till we have processed all the resizes
   if(unlikely(!(dic->bloc.flags&DICFIHF)))s=0;   // user hash function, keyprep will allocate s
   else if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
   else{A x; GATV0E(x,FL,kn,1,goto abortexit;) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
     // we have to reinit s in the resize loop because putslots may have modified it
 
   if(unlikely((s=jtkeyprep(dic,k,kn,s,jt,w))==0))goto errexit;  // hash keys & prefetch.  This may return as s a block that was allocated inside keyprep.  It must persist till the end
   lv=jtputslots(dic,k,kn,v,vn,s,jt,lv,virt); if(lv==0)goto errexit; if(likely(!(lv&DICLMSKRESIZEREQ)))break;  // do the puts; abort if error if no resize, finish, good or bad
   if(dicresize(dic,jt)==0)goto errexit;  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
   lv&=~(DICLMSKRESIZEREQ+DICLMSKOKRET);  // remove return flags from lv
  }
 }
 z=mtv; if(0){errexit: z=0; lv=DICLMSKWRV;}   // set lv so as to allow updating the current-owner flag - even if singlethreaded, since it doesn't matter then.  errexit is also entered from fast case, if error
 PRISTCLRF(w);    // we have taken from w; remove pristinity.  This destroys w.  We do this even in case of error because we may have moved some values before the error happened
abortexit:;
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
 RETF(z);
}

// ********************************** del **********************************

// k is A for keys, s is slot#s/returned status area: ~0=key not found, other=found
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; rc=0 if error, rc=-1 to request a resize
static INLINE UI8 jtdelslots(DIC *dic,void *k,I n,I8 *s,J jt,UI lv,VIRT virt){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;    // more nonresizable
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
    // with this lock we can add new kvs, or change an empty/tombstone to a birthstone; but no other hash changes, and no value overwrites

 UI8 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen  compare func  base of hashtbl
 C *kbase=CAV(dic->bloc.keys);  // address corresponding to unbiased hash value.

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp

 // first pass over keys.  If key found, add a record of (unbiased kvslot)\hashslot or ~0 for key not found
// obsolete  I ndels;  // init old chain empty
 for(i=0;i<n;++i){  // loop ascending to keep result order matching input
  I8 curslot=(((UI8)s[i]*(UI4)hsz)>>32); I hval; // convert hash to slot#; place where biased kv# comes in
  while(1){
   hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
   if(withprob((hval-=HASHNRES)<0,0.3)){  // hole or tombstone
    if(withprob(hval<HASHTSTN-HASHNRES,0.8)){s[i]=~0; break;}   // hole, exit not found
   }else if(withprob(keysne((UI4)kib,kbase+(kib>>32)*hval,(void*)((I)k+i*(kib>>32)),hsz&(DICFICF<<DICFBASE),errexit)==0,0.7)){s[i]=((I8)hval<<32)+curslot; break;}  // match, exit found
   if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
  }
 }

 DICLKWRRQ(dic,lv);   // request write lock
 I vb=dic->bloc.vbytelen; C *vbase=CAV(dic->bloc.vals);  // address corresponding to unbiased hash value of 0.
 UI emptyx=dic->bloc.emptyn; I cur; // starting root of free queue, current index, empty list
 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))unbiasforcomp
// obsolete  I ndel=0;  // number of deleted keys
 DICLKWRWTK(dic,lv)  // wait for write lock for  to be granted.  No resize is possible

 // process the list of kvslot\hashslot, turning each hashslot into a tombstone.  If it's already a tombstone, double del, ignore it & remove
 for(i=0;i<n;++i){
  I8 curslot=s[i];
  if(likely(curslot!=~0)){   // if slot not found, skip processing it
   UI hval=_bzhi_u64(*(UI4*)&hashtbl[(UI4)curslot*(hsz>>56)],(hsz>>53));  // read biased hashtable index, to see if it's a tombstone already (double del)
   if(likely(hval>=HASHNRES)){  // don't clear a slot more than once
    I t=HASHTSTN; WRHASH1234(t, hsz>>56, &hashtbl[(UI4)curslot*(hsz>>56)])  // mark the slot as a tombstone
// obsolete     s[i]=curslot;  // not a double delete, keep it for the last pass
   }else s[i]=~0;  // mark a double delete like empty
  }
 }

 DICLKWRWTV(dic,lv) DICLKWRRELK(dic,lv)    // Wait for values to be modifiable; then since the hashtable doesn't refer to any deleted keys, allow gets to look at hashtable & keys

 // process the list of kvslot\hashslot, putting the kv slots onto the empty chain & flipping trailing tombstones.  Flipping tombstones is safe since we are the lead writer
 I ndel=0;  // count of actual deletions
 for(i=0;i<n;++i){
  UI8 curslot=s[i];
  if(likely(curslot!=(UI8)~0)){  // if actual deletion...
   ++ndel;  // count the number of keys deleted
   UI kvx=curslot>>32;  // get unbiased index of deleted kv
   DELKV(kbase+kvx*(kib>>32),kib>>32,hsz&(DICFKINDIR<<DICFBASE)) DELKV(vbase+kvx*vb,vb,hsz&(DICFVINDIR<<DICFBASE))   // if k/v is indirect, free it & clear to 0
   I t=emptyx; WRHASH1234(t,hsz>>48,&kbase[kvx*(kib>>32)])  // chain old free chain from new deletion
   emptyx=kvx;  // put new deletion at top of the free chain, unbiased
   // Whenever we add a tombstone, we turn it into an empty if it is followed by an empty; and we continue this back into previous tombstones
   UI cs=(UI4)curslot; I curslotn=cs-1; if(unlikely(curslotn<0))curslotn+=(UI4)hsz;   // point to next slot
   if(_bzhi_u64(*(UI4*)&hashtbl[curslotn*(hsz>>56)],(hsz>>53))==0){
    // curslot is followed by a hole.  Flip it & its predecessors to holes
    do{hashtbl[cs*(hsz>>56)]=0; if(unlikely(++cs==(UI4)hsz))cs=0;}while(_bzhi_u64(*(UI4*)&hashtbl[cs*(hsz>>56)],(hsz>>53))==HASHTSTN);  // only the low byte needs to change
   }
  }
 }

// obsolete  // chase the old kvs, which give the hashslots to be deleted.  We turn each into a tombstone, put the kv slot on the empty chain, and flip trailing tombstones.  If it is a tombstone already, it's a double del, ignore it
// obsolete // scaf should first mark the tombstones, closing up double dels; then release lock on keys; the retraverse, fixing the empty chain and flipping trailing tombstones.  No other thread can write to the keys while we still have the value lock
// obsolete  for(cur=oroot;cur>=0;){
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
// obsolete   I8 curslot=((UI4*)(&s[cur]))[0];  // extract hashslot# to be emptied
// obsolete   UI hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // get biased kv slot to be released
// obsolete   if(likely(hval>=HASHNRES)){  // don't clear a slot more than once
// obsolete    I t=HASHTSTN; WRHASH1234(t, hsz>>56, &hashtbl[curslot*(hsz>>56)])  // mark the slot as a tombstone
// obsolete    DELKV(kbase+hval*(kib>>32),kib>>32,hsz&(DICFKINDIR<<DICFBASE)) DELKV(vbase+hval*vb,vb,hsz&(DICFVINDIR<<DICFBASE))   // if k/v is indirect, free it & clear to 0
// obsolete    t=emptyx; WRHASH1234(t, hsz>>48, &kbase[hval*(kib>>32)])  // chain old free chain from new deletion
// obsolete    emptyx=HENCEMPTY(hval);  // put new deletion at top of the free chain, unbiased
// obsolete    ++ndel;  // count the deletions
// obsolete    // Whenever we add a tombstone, we turn it into an empty if it is followed by an empty; and we continue this back into previous tombstones
// obsolete    I8 curslotn=curslot-1; if(unlikely(curslotn<0))curslotn+=(UI4)hsz;   // point to next slot
// obsolete    if(_bzhi_u64(*(UI4*)&hashtbl[curslotn*(hsz>>56)],(hsz>>53))==0){
// obsolete     // curslot is followed by a hole.  Flip it & its predecessors to holes
// obsolete     do{hashtbl[curslot*(hsz>>56)]=0; if(unlikely(++curslot==(UI4)hsz))curslot=0;}while(_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53))==HASHTSTN);  // only the low byte needs to change
// obsolete    }
// obsolete   }
// obsolete   cur=nxt;  // advance to next
// obsolete  }
 dic->bloc.emptyn=emptyx;  // Store updated free chain after all deletions
 dic->bloc.cardinality-=ndel;  // account for the deleted keys

 R lv|DICLMSKOKRET+(hsz&(DICFRESIZE<<DICFBASE)?DICLMSKRESIZEREQ:0);  // return, holding write lock on values or values/keys.  Use flag to force non0 for no error

errexit:; R 0;
// resize: R -1;  // signal need for resize; return holding write lock on keys & values
}


//  del.   w is keys   u was dic to self
static DF1(jtdicdel){F12IP;
 ARGCHK1(w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.ktype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))  // convert type of k if needed
// scaf have fast path for single delete
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index

 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

 UI lv; DICLKRWRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
 void *k=voidAV(w);  // point to the key and value data
 while(1){  // loop till we have processed all the resizes
  if(unlikely(!(dic->bloc.flags&DICFIHF)))s=0;   // user hash function, keyprep will allocate s
  else if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
  else{A x; GATV0E(x,FL,kn,1,goto errexit;) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
    // we must reinit s because it might have been changed by keyprep

  if(unlikely((s=jtkeyprep(dic,k,kn,s,jt,w))==0))goto errexit;  // hash keys & prefetch
  lv=jtdelslots(dic,k,kn,s,jt,lv,virt); if(lv==0)goto errexit; if(likely(!(lv&DICLMSKRESIZEREQ)))break;  // do the puts; abort if error if no resize, finish, good or bad
  if(dicresize(dic,jt)==0)goto errexit;  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
  lv&=~(DICLMSKRESIZEREQ+DICLMSKOKRET);  // remove return flags from lv
// obsolete   I rc=jtdelslots(dic,k,kn,s,jt,lv,virt); if(rc>0)break; if(rc==0)goto errexit;  // do the puts; if no resize, finish, good or bad
// obsolete   dicresize(dic,jt);  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
 }
 // s has the info about deletions.  Allocate a return area & return it
 A z; GATVE(z,B01,kn,wf,AS(w),goto errexit;) C *zv=CAVn(wf,z); DO(kn, zv[i]=s[i]!=~0;)
 if(0){errexit: z=0; lv=DICLMSKWRV;}   // set lv so as to allow updating the current-owner flag - even if singlethreaded, since it doesn't matter then
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
 RETF(z);
}

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- ordered map (red-black tree) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

// The tree is stored as child0/child1/parent, each a variable number of bytes.  The LSB of child0 is the color, 0=red, with the index of the child shifted
// since the tree node is exactly 2 children, nothing more, use hashtbl[(child01&~1)*len] to get to the child
// There is 1 tree node per key/value allocated
// empty tree nodes are on the empty chain, with loopback at end of chain
#if 0 // obsolete 
#define CHN(node,num) _bzhi_u64(*(UI4*)&hashtbl[(node)*nodeb+(num)*(nodexb>>16)],nodexb)   // get chain: 0=left 1=right 2=pdir
#define LC(node) CHN(node,0)   // left chain
#define RC(node) CHN(node,1)    // right chain
#define PCLR(node) CHN(node,2)  // parent\clr
#define CBYTE(node) hashtbl[(node)*nodeb+2*(nodexb>>16)]  // color byte (LSB=color)
#define WCHN(node,num,val) {UI4 t=(val); WRHASH1234(t, nodexb>>16, &hashtbl[(node)*nodeb+(num)*(nodexb>>16)]);}
#define WLC(node,val) WCHN(node,0,val)   // left chain
#define WRC(node,val) WCHN(node,1,val)    // right chain
#define WPCLR(node,val,clr) WCHN(node,2,2*(val)+clr)  // parent\clr
#define ROT(par,chl,pardir) {UI gpar=PCLR(par)>>1; WCHN(gpar,(par)!=LC(gpar),chl) WPCLR(par,chl,CBYTE(par)&1) WCHN(chl,(pardir)^1,par) \
     WCHN(par,pardir,CHN(chl,(pardir)^1)) WPCLR(CHN(chl,(pardir)^1),par,CBYTE(CHN(chl,(pardir)^1))&1) }
#define LROT(par,chl) ROT(par,chl,0)  // rotate when child is left child of parent
#define RROT(par,chl) ROT(par,chl,1)   //  rotate then child is right child of parent
#define XCROT(par,parclr) {C pc=CBYTE(par), cc=CBYTE(CHN(par,(1^parclr))); CBYTE(par)=(pc&~1)+(cc&1); CBYTE(CHN(par,(1^parclr)))=(cc&~1)+(pc&1);}  // swap colors of parent and not-rotated child
#endif

// node here has LSB clear, index in upper bits
#define RDIR(node,dir) (*(UI4*)&hashtbl[((node)+(dir))*(nodeb>>24)]&_bzhi_u64(~(UI8)1,nodeb))
#define DLRC(name) UI8 name##ch; UI name##l,name##r,name##c;  // declare names for chrn etc
#define RLRC(name,node) name##ch=*(UI8*)&hashtbl[(node)*(nodeb>>24)]; name##l=name##ch&_bzhi_u64(~(UI8)1,nodeb); name##r=(name##ch>>(nodeb))&_bzhi_u64(~(UI8)1,nodeb); name##c=name##ch&1;  // clears LSBs in ##l and ##r
#define DRLRC(name,node) DLRC(name) RLRC(name,node)
#define DSOC(name) UI8 name##ch; UI name##s,name##o,name##c;  // declare names for chrn, ,clr, s (child in same direction as dir), o (child in opposite direction from dir)
#define RSOC(name,node,dir) {name##ch=*(UI8*)&hashtbl[(node)*(nodeb>>24)]; I ss=(dir)?(C)nodeb:0, os=(dir)?0:(C)nodeb; name##s=(name##ch>>ss)&_bzhi_u64(~(UI8)1,nodeb); name##o=(name##ch>>os)&_bzhi_u64(~(UI8)1,nodeb); name##c=name##ch&1;}
#define DRSOC(name,node,dir) DSOC(name) RSOC(name,node,dir)
#define SIB(node,lnode,rnode) ((lnode)^(rnode)^(node))  // sibling of current node
#define WLRC(node,num,val,clr) {UI4 t=(val)+(clr); WRHASH1234(t, (nodeb>>24), &hashtbl[((node)+(num))*(nodeb>>24)]);}
#define WLC(node,val,clr) WLRC(node,0,val,clr)   // left chain+color
#define WR(node,val) WLRC(node,1,val,0)    // right chain
#define CBYTE(node) hashtbl[(node)*(nodeb>>24)]  // color byte (LSB=color)
// rotate par[dir], into par.  newpar (=par[dir]) is new root  par is gpar[gdir]
#define ROT(par,dir,gpar,gdir,newpar) WLRC(par,dir,RDIR(newpar,(dir)^1),CBYTE(par)&1)  /* hang par[dir][near] off par[dir], keeping color */ \
 WLRC(newpar,(dir)^1,par,CBYTE(newpar)&1)  /* hang old root off new root, keeping color */ \
 WLRC(gpar,gdir,newpar,CBYTE(gpar)&1)  // point parent of old root to new root, keeping color

// dump out info for the tree starting at nodex, in ascending key order, checking for violations
// dirstack builds the LR direction info, currently depth long
// blackdepth is the number of black node between the current node and the root
// parentcolor is used to check for red violations
// *prevkey is the previous key (0 first time), used to check for order violation (default compare only)
// *leafblackdepth is the black depth of a leaf, used to check for height violations
// excludednode is set during deletion with the index of the node to delete.  We suppress it & its descendants
// result is last key we used
static A dumptree(J jt,DIC *dic, UI nodex, C *dirstack, I depth, I blackdepth, I parentcolor, A prevkey, I *leafblackdepth, I *noerr, UI excludednode, I doprint){PROLOG(000);
 if(nodex<(TREENRES<<1)||nodex==excludednode)R prevkey;  // 
 C *hashtbl=CAV3(dic->bloc.hash); I nodeb=dic->bloc.hashelesiz*(0x1000000+SZI)+(dic->bloc.flags<<8)+(dic->bloc.emptysiz<<19); DRLRC(curr,nodex)   // fetch tree info, then children+color
 dirstack[depth]='0'; prevkey=dumptree(jt,dic,currl,dirstack,depth+1,blackdepth+currc,currc,prevkey,leafblackdepth,noerr,excludednode,doprint);
 A k,v; RZ(k=from(sc(RENCEMPTY(nodex)),dic->bloc.keys)) RZ(v=from(sc(RENCEMPTY(nodex)),dic->bloc.vals))  // fetch current key/val
 A klr, vlr; RZ(klr=lrep(k)) RZ(vlr=lrep(v))  // displayable form of k,v
 if(doprint)printf("%.*s: node=0x%x key=%.*s val=%.*s c=%d l=0x%x r=0x%x",(int)depth,dirstack,(int)nodex,(int)AN(klr),CAV(klr),(int)AN(vlr),CAV(vlr),(int)currc,(int)currl,(int)currr);
 if(prevkey&&IAV(jttao(jt,prevkey,k))[0]>=0){if(doprint)printf(" key vio"); *noerr=0;}
 if(currc==1&&((currr|currl)&~1)==0){if(*leafblackdepth>=0&&*leafblackdepth!=blackdepth){if(doprint)printf(" black vio"); *noerr=0;} *leafblackdepth=blackdepth;}  // black & empty, check depth
 if(((parentcolor|currc)&1)==0){if(doprint)printf(" red vio"); *noerr=0;}
 if(doprint)printf("\n");
  // remember depth of each leaf
 dirstack[depth]='1'; k=dumptree(jt,dic,currr,dirstack,depth+1,blackdepth+currc,currc,k,leafblackdepth,noerr,excludednode,doprint);
 EPILOG(k);
}

// audit the subtree starting at nodex.  Prints the tree. Result is 1 if OK, 0 if audit failure 
static I auditnode(J jt,DIC *dic,UI nodex,UI excludednode, I doprint){A z;
 if(nodex<(TREENRES<<1)){if(doprint)printf("empty tree\n"); R 1;}  // no failures if tree empty
 C dirstack[128]; I leafblackdepth=-1; I noerr=1;
 RZ(z=dumptree(jt,dic,nodex,dirstack,0,0,1,0,&leafblackdepth,&noerr,excludednode,doprint))
 R noerr;  // return error value
}

// 16!:_7 Bivalent. Print and audit the tree in w. a is options: 1 (default)=printf.  Called ONLY by a,w,self or w,self,self
// Result is 1 if OK, 0 if error
DF2(jtdisprbdic){F12IP;
 ARGCHK1(w)
 A na=w==self?zeroionei(1):a; w=w==self?a:w; I opts; RE(opts=b0(na));   // get print options
 R sc(auditnode(jt,(DIC*)w,*(UI4AV3(((DIC*)w)->bloc.hash))&_bzhi_u64(~(UI8)1,((DIC*)w)->bloc.hashelesiz<<LGBB),~0LL,opts));
}
// ********************************** get/has **********************************
 // k is A for keys, s is slot#s, zv is result area (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy the value (or default) to the result
// a is default if given, 0 if not; or 1 if has
// We take a read lock on the table and release it
static INLINE B jtgetslotso(DIC *dic,void *k,I n,I8 *s,void *zv,J jt,A a, VIRT virt){I i;
 UI lv; DICLKRDRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request read lock
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn; I vb=dic->bloc.vbytelen;  // more nonresizable: kbytelen/kitemlen   compare fn  prototype required to get arg converted to I
 I nodeb=dic->bloc.hashelesiz*(0x1000000+SZI)+(dic->bloc.flags<<8)+(dic->bloc.emptysiz<<19);  // number of bytes in a tree node; (#bytes in node index)\(#bytes in empty-chain field\(flags)\(number of bits in a node index)
 DICLKRDWTK(dic,lv)   // wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 C *hashtbl=CAV3(dic->bloc.hash);  // pointer to tree base
 C *kbase=CAV(dic->bloc.keys)-TREENRES*(kib>>32);  // key address corresponding to tree value of 0.  Treevalues 0-1 are empty and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals)-TREENRES*vb;  // value address corresponding to tree value of 0.  Treevalues 0-1 are empty and do not take space in the value array

 // loop over keys (reverse order).  Find the key
 if(unlikely(!(nodeb&(DICFICF<<8))))biasforcomp
 void *ki=(void *)((I)k+(n-1)*(kib>>32));  // pointer to key being compared
 for(i=n-1;i>=0;--i){I nodex;
  UI8 chirn;  // both children
  UI rootx=*(UI4*)hashtbl&_bzhi_u64(~(UI8)1,nodeb);  // biased node# of the root of the tree
  I comp=1;  // will be compare result.  The root is considered to have compared low
  // the root is pointed to by hash0/dir0.  In an empty database hash0/dir0=0.
  for(nodex=rootx;;){  // traverse the tree, searching for index k.  Current node is nodex
   if(unlikely(nodex<(TREENRES<<1))){s[i]=0; break;}  // not found.  Set s[i] that way
   chirn=*(UI8*)&hashtbl[nodex*(nodeb>>24)];  // fetch both children
   s[i]=nodex;  // remember the node where we checked, freeing the register
   comp=keysne((UI4)kib,kbase+(kib>>32)*(nodex>>1),ki,nodeb&(DICFICF<<8),exitkeyvals);  // compare node key vs k, so k > node is ~0
   if(comp==0){PREFETCH(vbase+(s[i]>>1)*vb); break;}  // found at node nodex.  chirn have the children, parent the parent\dir (also in pdir[])
   nodex=(chirn>>(comp&nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb);  // choose left/right based on comparison, mask out garb.
  }
  ki=(void *)ki-(kib>>32);  // back up to next key
 }
 if(unlikely(!(nodeb&(DICFICF<<8))))unbiasforcomp

 // copy using the kv indexes we calculated.  Copy in ascending order so we can overstore    scaf overstore
 if(a==(A)1){DICLKRDRELKV(dic,lv) DO(n, ((C*)zv)[i]=s[i]>=TREENRES<<1;)  // if processing has, just copy found status
 }else{
  DICLKRDWTV(dic,lv) DICLKRDRELK(dic,lv)  // signal we are finished with keys; wait till values are safe to copy
  void *av=0;  // init to 'no default data pointer yet'.  We avoid checking the default until we know we need it
  I cur=s[0];  // unroll loop once.  cur if current value to work on
  for(i=0;i<n;++i){void *vv;  // index of kv to free
   I nxt=s[i+1];  // fetch next value.  Overfetch OK
   if(likely(cur>=(TREENRES<<1)))vv=vbase+(cur>>1)*vb;   // this is the main line, key found
   else{   // default required, which we deem rare.
    if(unlikely(av==0)){
     // First time through here. We have to audit it & convert if needed
     ASSERTGOTO(a!=0,EVINDEX,exitvals)   // error if no default given
     ASSERTGOTO(AR(a)==AN(dic->bloc.vshape),EVRANK,exitvals) ASSERTGOTO(TESTAGREE(AS(a),IAV1(dic->bloc.vshape),AR(a)),EVLENGTH,exitvals)  // verify default has correct rank & shape
     if(unlikely((AT(a)&dic->bloc.vtype)==0)){RZ(a=ccvt(dic->bloc.vtype,a,0))}   // convert precision if needed
     av=CAV(a);  // get address of values to move
     PRISTCLRF(a);  // Since we are taking out of a, it is no longer pristine.  this destroys a, but leaves it non0.
    }
    vv=av;    // use the default after it has been audited
   }
   GETV(zv,vv,vb,nodeb&(DICFVINDIR<<8)); zv=(void *)((I)zv+vb);   // move the data & advance pointer to next one   scaf JMC?
   cur=nxt;
  }
  DICLKRDRELV(dic,lv)  // values copied, release lock on them
 }
 R 1;
exitvals:; DICLKRDRELV(dic,lv) R 0;
exitkeyvals:;  DICLKRDRELKV(dic,lv) R 0;
}

//   get.   Bivalent. w is keys, [a] is default value, 0 if monad.  Called from parse/unquote as a,w,self or w,self,self.  dic was u to self
static DF2(jtdicgeto){F12IP;A z;
 ARGCHK2(a,w)
 A adyad=w!=self?a:0; w=w!=self?w:a;  // if dyad, keep a,w, otherwise 0,w
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 I t=dic->bloc.vtype; A sa=dic->bloc.vshape; I vaii=dic->bloc.vaii; t=FAV(self)->localuse.lu1.varno==0?t:B01; sa=FAV(self)->localuse.lu1.varno==0?sa:mtv; vaii=FAV(self)->localuse.lu1.varno==0?vaii:1; adyad=FAV(self)->localuse.lu1.varno==0?adyad:(A)1;  // type/shape of output, for get or has
 ASSERT((FAV(self)->localuse.lu1.varno|dic->bloc.vbytelen)!=0,EVDOMAIN)   // get not allowed on empty value
 if(unlikely(AN(w)==0)){A avalues=dic->bloc.vals; avalues=FAV(self)->localuse.lu1.varno==0?avalues:mtv; R reitem(drop(sc(-AN(dic->bloc.vshape)),w),avalues);}  // if no keys, return empty fast   ((-kshape) }. $w) $ values/''
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))   // convert type of w if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index

// obsolete  I t=dic->bloc.vtype; A sa=dic->bloc.vshape;
 GA0(z,t,kn*vaii,wf+AN(sa)) AFLAG(z)=t&RECURSIBLE; MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,IAV1(sa),AN(sa))   // allocate recursive result area & install shape
 // establish the area to use for s.  To avoid wasting a lot of stack space we use the virt-block area if that is not needed for user comp.  And if there is a user hash, we allocate
 // nothing & use the value returned by keyprep, which will be the result area from the user hash.
 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer
 if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
 else{A x; GATV0(x,FL,kn,1) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes; we use as UI8
 
 void *k=voidAV(w);  // point to the key data
 if(!jtgetslotso(dic,k,kn,s,voidAV(z),jt,adyad,virt))z=0;  // get the values and take a read lock on the dic.  If error, pass error through
 RETF(z);
}

// ********************************** getkv **********************************

// k is A for key0,:key1, flags is (return k, return v, include key-0, include k-end)
// We take a read lock on the table and release it
// virt is used as virtuals for compare, and then repurposed to hold indexes of kvs to be copied
static INLINE A jtgetkvslotso(DIC *dic,void *k,I flags,J jt,VIRT virt){I i;
 PROLOG(000);
 UI lv; DICLKRDRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request read lock
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn; I vn=dic->bloc.vbytelen;  // more nonresizable: kbytelen/kitemlen   compare fn  prototype required to get arg converted to I
 I nodeb=dic->bloc.hashelesiz*(0x1000000+SZI)+(dic->bloc.flags<<8)+(dic->bloc.emptysiz<<19);  // number of bytes in a tree node; (#bytes in node index)\(#bytes in empty-chain field\(flags)\(number of bits in a node index)
 DICLKRDWTK(dic,lv)   // wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 C *hashtbl=CAV3(dic->bloc.hash);  // pointer to tree base
 C *kbase=CAV(dic->bloc.keys)-TREENRES*(kib>>32);  // address corresponding to tree value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals)-TREENRES*vn;  // address corresponding to tree value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the value array
 I vb=dic->bloc.vbytelen;   //  len of 1 value

 UI4 rstack[64][2]; //  Stack currnode,rchild.  Return is always to the node representing the parent itself.
 UI4 res1[3];  // nodex or sp during tree search.  Values are stored into res1[comp+1] and the correct sign is selected at the end, giving the last thing stored with that sign
 UI8 chirn;  // both children
 if(unlikely(!(nodeb&(DICFICF<<8))))biasforcomp
 C *k01=(C*)k;  // pointer to the key we are comparing against: k0 to begin with
 UI nodex=*(UI4*)hashtbl&_bzhi_u64(~(UI8)1,nodeb);  // search node.  Init to biased node# of the root of the tree
 if(unlikely(nodex<(TREENRES<<1)))goto retempty; // empty database: nothing to do
 I comp=1;  // will be compare result at end of search.  The root is considered to have compared low
 rstack[0][0]=rstack[0][1]=0;  // top of stack is the empty-tree pointer
 res1[1+1]=0;  // init 'stack at first key' to 0 (invalid).  If it is still there after the search there were no valid keys
 I sp=1; // sp points to last valid stack entry+1
 // search down, looking for the min value, building the stack.  Call the end-of-search point L & compare result (tree-min) LC.
 // the search ends on a match (LC=0), or on a leaf node whose successor (i. e. the parent on the stack) is > min (LC<0), or on the last leaf, if the result is empty (LC>0)
 do{  // traverse the tree, searching for index k.  Current node is nodex
  chirn=*(UI8*)&hashtbl[nodex*(nodeb>>24)];  // fetch both children
  rstack[sp][0]=nodex;  // store this node
  rstack[sp][1]=(chirn>>(nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb);   // stack right child
  chirn=((UI8)rstack[sp][1]<<31)+((chirn>>(0&&nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb));  // extract left child, leave chirn as right\left
  comp=keysne((UI4)kib,kbase+(kib>>32)*(nodex>>1),k01,nodeb&(DICFICF<<8),exitkeyvals);  // compare node key vs k01 (key to be found), so node > min is 1
  if(comp==0)goto match;  // found at node nodex.  sp points to the children
  nodex=(chirn>>((UI)comp>>(BW-5)))&0xffffffff;  // choose left/right based on comparison
  // No match.  drop down to next node.
  res1[1+comp]=sp;  // remember sp of the first result in res1[1 or 2].  Stores to res[0] are wasted
  ++sp;  // advance sp to next empty slot, which we will fill presently
 }while(nodex>=(TREENRES<<1));  // loop till we hit end of tree
 // falling through, there was no match.  Take the last value that is > min
 sp=res1[1+1];  //  the last (i. e. smallest) key > k01 is in slot 1.  remember sp+1 for starting node
 flags|=4;  // indicate we should out the first key (since it isn't the min)
 if(unlikely(sp==0))goto retempty;  // if all comparisons were <, k0 > entire tree, nothing to return
match:;   // here when match, with sp set from the push of the matching node
 // rstack[sp] holds (startx,startxr).  We will go up from there, stopping when we reach the max.  We would like to avoid
 // comparing against the max at every node, so we go up the stack looking for the lowest level that is >=max.  Since we expect that a single request will return
 // a small fraction of the tree, this shouldn't take long and will be faster than searching from the top of the tree down.

 k01+=(kib>>32);   // from now on, comparisons are against max key

 UI dstartx; I sp1;  // will be starting point for downward search, holding the highest stacked node index <= max; stack pointer+1 when nodex was the current node
 for(nodex=rstack[sp][0],sp1=sp;sp1>0;sp1--){   // do not look at sp=0, which is 0
  UI nextx=rstack[sp1-1][0];  // unroll 1 loop - parent of nodex
  comp=keysne((UI4)kib,kbase+(kib>>32)*(nodex>>1),k01,nodeb&(DICFICF<<8),exitkeyvals);  // compare node key vs k01 (max), so node > max is 1
  if(comp>0)break;  // stop when node > max.  If node=max, we want to start on it, so we go 1 more turn
  PREFETCH(&hashtbl[rstack[sp1][1]*(nodeb>>24)]);  // prefetch right side on the way up
  dstartx=nodex;  // update dstartx to be the last value <= max
  nodex=nextx;  // advance to next
 }
 if(unlikely(sp1==sp))goto retempty;  // if startx was > max, there are no keys in the range

 nodex=dstartx;  // revert to search-down point, known to be <= max; we know end-of-region will be in a subtree of this node
 // The stack contained values that were tested and found to be > min.  We have gone up, finding the highest-up such value that is <= max.  That is the start point dstartx.
 //  We will be on the correct half of the tree, at least.  Find the ending node.  No stacking required
 while(1){  // traverse the tree, searching for index k.  Current node is nodex
  chirn=*(UI8*)&hashtbl[nodex*(nodeb>>24)];  // fetch both children
  comp=keysne((UI4)kib,kbase+(kib>>32)*(nodex>>1),k01,nodeb&(DICFICF<<8),exitkeyvals);  // compare node key vs k01, so node > max is 1
  res1[1+comp]=nodex;   // remember node of the first result in res1[1 or 2].  We could do this in endx, not using res1, but res1 doesn't require a register
  if(unlikely(comp==0)){goto matchdown;}  // found at node nodex.  chirn have the children
  nodex=(chirn>>(comp&nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb);  // choose left/right based on comparison, mask out garb.
  if(unlikely(nodex<(TREENRES<<1))){break;}  // not found.  comp is set
 }
 // fall through: no match: use the last node where node key was < max (i. e. comp=-1) 
 flags|=8;   // no match: flag that we should keep the end node
 comp=-1;  // set so we look at the last key < max
matchdown:;  // come here when comp=0, meaning exact match
 UI endx=res1[1+comp];  // remember last node in result

 if(unlikely(!(nodeb&(DICFICF<<8))))unbiasforcomp  // comparisons finished, prepare for copying

 // as we find the key indexes to return, prefetch key/value
 C *pfv=flags&1?kbase:vbase; I pfb=flags&1?(kib>>32):vb;   // base/stride for prefetch, keys if keys are written, else values

 // scan the tree/stack, starting at startx, and stopping when we get to endx.  sp points to nodex, so we will return to nodex's caller.  We will enter looking at nodex itself, as if the left side just returned
 UI4 *ziv=(UI4*)&virt; I zx=0;  // Repurpose virt area to store node#, used if there aren't many kvs; index of next slot in area
 // nodex is the node scan through the loop
 DLRC(nodex)
 nodex=rstack[sp][0]; nodexr=rstack[sp][1];    // init to first found node.   sp still holds its stackpos.  We don't need nodexl
 if(unlikely(nodex==endx)&&(flags&0b1100)!=0b1100)goto endscan;  // special case when there is only one node: if either boundary flag is clear (indicating indicating the boundary node should be elided), reject both boundaries for fast bypass
 if(flags&4)goto startmin; else goto startminex;  // Start moving right.  If we should out the min node, go there; otherwise to where we handle right side
 while(1){  // till we hit node >= max
  while(1){  // go down the left children, pushing onto the stack
   RLRC(nodex,nodex);  // read children
   if(nodexl<(TREENRES<<1))break;  // exit loop when no left child
   PREFETCH(&hashtbl[nodexr*(nodeb>>24)]);   // prefetch the right side of tree, which we will return to presently
   rstack[sp][0]=nodex; rstack[sp][1]=nodexr; nodex=nodexl; ++sp;  // push return, advance to left child
  }
  // no more left children.
startmin:;  // enter first time going right only
  while(1){  // process middle and right nodes
   // nodex is a middle node, with nodexr as right child
   // out the middle node
   if(zx+8*SZI/4==(LOWESTBIT(zx+8*SZI/4)&-(8*SZI))){  // if current allocation exceeded...  (when size+header size is a power of 2, at least 8*SZI - VIRT holds 32 Is, which is 32/64 UI4s)
    A zia; GATV0E(zia,INT4,zx+zx+8*SZI/4,1,goto exitkeyvals); MC(I4AV1(zia),ziv,zx*4); ziv=I4AV1(zia);  // double & copy
   }
   ziv[zx]=nodex;  // put the node out, in order
   PREFETCH(pfv+(nodex>>1)*pfb);  // prefetch the kv to be (possibly) moved
   // finish when we out the ending node
   if(nodex==endx){zx+=(flags>>3)&1; goto endscan;}  // accept last node if not suppressed
   ++zx;  // not end: accept the node
startminex:;  // enter first time when first node is suppressed.  We know that is not also the end node
   if(nodexr>=(TREENRES<<1)){nodex=nodexr; break;}  // if there is a right child, enter it
   // No right child, we must pop.  If the previous right child matches our nodex, we are returning to the right side
   do{
    --sp;  // pop back
    nodexl=nodex;  // save nodex for test
    nodex=rstack[sp][0]; nodexr=rstack[sp][1];  // restore nodex/nodexr for right-side processing in caller
   }while(nodexl==nodexr);  // if called from right side, end the call and try up one level; if from left, continue through loop to process the middle and right nodes of caller
     // note: this loop must end because we know endx is in the tree
  }
  // we fall through to here to return from a left-side call
 }
endscan:;   // come here when we exnounter the end node, possibly without looking at anything
 // finished, zx having the count

 // allocate the result(s), and run through the indexes, copying
 A zak=0, zav=0; I zn=zx;  // result keys, values; number of results
 if(flags&1){  // if user wants keys
  GAE0(zak,dic->bloc.ktype,dic->bloc.kaii*zn,1+AN(dic->bloc.kshape),goto exitkeyvals) AS(zak)[0]=zn; MCISH(&AS(zak)[1],IAV1(dic->bloc.kshape),AN(dic->bloc.kshape)) C *zv=CAVn(1+AN(dic->bloc.kshape),zak);  // allocate result
  nodex=ziv[0];  // 1 unroll
  for(zx=0;zx<zn;++zx){  // for each node in result
   UI nextx=ziv[zx+1];  // unroll loop.  1 overfetch OK
   GETV(zv,kbase+(kib>>32)*(nodex>>1),kib>>32,nodeb&(DICFKINDIR<<8)); zv=(void *)((I)zv+(kib>>32));   // move the data & advance pointer to next one   scaf JMC?
   if(flags&2)PREFETCH(vbase+(nodex>>1)*vb);   // prefetch the corresponding value
   nodex=nextx;  // advance to next
  }
 }
 DICLKRDRELK(dic,lv)   // release our lock on keys

 if(flags&2){  // if user wants values... 
  GAE0(zav,dic->bloc.vtype,dic->bloc.vaii*zn,1+AN(dic->bloc.vshape),goto exitvals) AS(zav)[0]=zn; MCISH(&AS(zav)[1],IAV1(dic->bloc.vshape),AN(dic->bloc.vshape)) C *zv=CAVn(1+AN(dic->bloc.vshape),zav);  // allocate result
  DICLKRDWTV(dic,lv)   // wait for values to be ready
  nodex=ziv[0];  // 1 unroll
  for(zx=0;zx<zn;++zx){  // for each node in result
   UI nextx=ziv[zx+1];  // unroll loop.  1 overfetch OK
   GETV(zv,vbase+vb*(nodex>>1),vb,nodeb&(DICFVINDIR<<8)); zv=(void *)((I)zv+vb);   // move the data & advance pointer to next one   scaf JMC?
   nodex=nextx;  // advance to next
  }
 }
 DICLKRDRELV(dic,lv)   // release our lock on values 
ret:;  // assemble & return result
 A z; if(zak==0||zav==0)z=(A)((I)zak+(I)zav); else z=jlink(zak,box(zav));  // return what was requested
 EPILOG(z);

retempty:;  // empty result
 zak=zav=0;  // 0 if not requested
 if(flags&1)RZGOTO(zak=from(mtv,dic->bloc.keys),exitkeyvals)  // '' { keys
 if(flags&2)RZGOTO(zav=from(mtv,dic->bloc.vals),exitkeyvals)  // '' { values
 DICLKRDRELKV(dic,lv)   // release lock on keys & values
 goto ret;

exitvals:; DICLKRDRELV(dic,lv)  R 0;  // release lock & exit
exitkeyvals:; DICLKRDRELKV(dic,lv)  R 0;  // release lock & exit
  
}


//   getkv.   Bivalent. w is k0,:kn, a is flags (#. k,v,min,max).  Called from parse/unquote as a,w,self or w,self,self.  dic was u to self
static DF2(jtdicgetkvo){F12IP;A z;
 ARGCHK2(a,w)
 I flags=0b1111;  // processing flags k,v,min,max
 if(w==self){w=a; // monad, keep default
 }else{   // extract for dyad
  ASSERT(AR(a)<=1,EVRANK) ASSERT(AN(a)<=4,EVLENGTH) if(unlikely(!(AT(a)&B01)))RZ(ccvt(B01,a,0))   //  convert to binary list, length <= 4
  DO(AN(a), flags&=~((BAV(a)[i]^1)<<i);)  // if bit i is 0, turn that bit off in flags
 }
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 ASSERT(likely(dic->bloc.vbytelen!=0)||!(flags&2),EVDOMAIN) ASSERT(AR(w)==kr+1,EVRANK) ASSERT(AS(w)[0]==2,EVLENGTH) ASSERTAGREE(AS(w)+1,ks,kr)   // can't read values if they are empty; w must be a single key or an array of them, with correct shape
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))   // convert type of w if needed

 VIRT virt; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer
 
 void *k=voidAV(w);  // point to the key data.  tree may be empty
 z=jtgetkvslotso(dic,k,flags,jt,virt);  // get the values and take a read lock on the dic.  If error, pass error through
 RETF(z);
}

// ********************************** put **********************************

// k is address of keys, v is address of values
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; rc=0 if error, otherwise current lv (which gives lock status), with OKRET set to be non0 and RESIZEREQ is a resize is needed
static INLINE UI8 jtputslotso(DIC *dic,void *k,I n,void *v,I vn,J jt,UI lv,VIRT virt){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;   // more nonresizable: kbytelen/kitemlen   compare fn  prototype required to get arg converted to I
 I nodeb=dic->bloc.hashelesiz*(0x1000000+SZI)+(dic->bloc.emptysiz<<19)+(dic->bloc.flags<<8);  // (#bytes in node index)\(#bits in empty-chain field\(flags)\(number of bits in a node index)
 I vb=dic->bloc.vbytelen;   //  len of 1 value
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 C *hashtbl=CAV3(dic->bloc.hash);  // pointer to tree base
 C *kbase=CAV(dic->bloc.keys)-TREENRES*(kib>>32);  // address corresponding to tree value of 0.  treevalues 0-1 are parents and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals)-TREENRES*vb;  // same for value

 DICLKWRRQ(dic,lv); DICLKWRWTK(dic,lv)  // request write lock and wait for keys & values to be modifiable.

 // loop over keys.  Find the key
 // Because we don't store parent info, we have to do each key separately, which requires keeping a write
 // lock after the first key.
 for(i=0;i<n;++i){UI nodex;
  UI8 chirn;  // both children
  if(unlikely(!(nodeb&(DICFICF<<8))))biasforcomp
  UI4 pdir[64]; I pi=0;   // parent/direction history; next slot to fill
  UI parent=0, rootx=*(UI4*)hashtbl&_bzhi_u64(~(UI8)1,nodeb);  // tree-node# of the root of the tree
  I comp=1;  // will be compare result.  The root is considered to have compared low
  // the root is pointed to by hash0/dir0.  In an empty database hash0/dir0=0.  In that case we fiddle things so that the first key is called new and the others are called conflict.  They all point to
  // hash0/dir0 as the end-of-search point.  The new will fill hash0/dir0 with a node, and the rest will start their search at that node, which is the true root of the tree.
  for(nodex=rootx;nodex>=(TREENRES<<1);){  // traverse the tree, searching for index k.  Current node is nodex (includes LSB).  Stop when we have fallen off the end of the tree (or tree is empty)
   chirn=*(UI8*)&hashtbl[nodex*(nodeb>>24)];  // fetch both children
   pdir[pi++]=parent+SGNTO0(comp);  // stack parent/dir going into nodex
   comp=keysne((UI4)kib,kbase+(kib>>32)*(nodex>>1),k,nodeb&(DICFICF<<8),errexit);  // compare node key vs k, so k > node is ~0
   if(comp==0){nodex|=1; if(unlikely(!(nodeb&(DICFICF<<8))))unbiasforcomp; goto finput;}  // found at node nodex.  Set nodex bit0 to indicate key already in place.  chirn have the children, parent the parent\dir (also in pdir[])
   parent=nodex;  // we will fetch again.  remember the parent of the next fetch
   nodex=(chirn>>(comp&nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb);  // choose left/right based on comparison, mask out garb. incl LSB
  }
  // the key was not found.  We have to add it.  Install the new node as parent[comp], with color red.  parent[pi-1] matches parent
  if(unlikely(!(nodeb&(DICFICF<<8))))unbiasforcomp;
  ++pi;  // we don't need to push parent, but to align stack properly we need to leave a space for it so that [pi-2] is gparent
  nodex=dic->bloc.emptyn;  // next slot to allocate, as empty index
  UI emptynxt=_bzhi_u64(*(UI4*)&kbase[(nodex+TREENRES)*(kib>>32)],(nodeb>>16));  // get next in free chain
  if(emptynxt==nodex)goto resize;  // if chained to self, that's end of chain, resize
  dic->bloc.emptyn=emptynxt;   // set new head of free chain
  ++dic->bloc.cardinality;  // increment number of keys
  nodex=RDECEMPTY(nodex);   // convert empty index to tree-index with LSB=0
  UI parentd=SGNTO0(comp);  // parent's direction
  WLC(nodex,0,0) WR(nodex,0)   // new node is empty red
  WLRC(parent,SGNTO0(comp),nodex,CBYTE(parent)&1)  // set as parent[comp], leaving parent color

  // We have installed nodex into the tree.  Leave nodex (with LSB 0) unchanged till the end, where we move in the key & value.
// obsolete PUTKVNEW(kbase+(nodex>>1)*(kib>>32),k,kib>>32,nodeb&(DICFKINDIR<<8));   // scaf to avoid msgs
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf

  // Go up through the tree, stopping when we get past the red violations or we see a way out
  UI snodex=nodex, gparent, gparentd, uncle;  // scan index up the tree; grandparent info for nodex, & uncle
  while(1){
   // at top of loop snodex (directed child of parent) is red, i. e. LSB=0
   if(CBYTE(parent)&1)goto finput;  // parent black: no red vio, finished, keep LSB=0 to copy key
   gparent=pdir[pi-2]; gparentd=PDIRDIR(gparent); gparent=PDIRNODE(gparent);   // grandparent\dir
   if(unlikely(gparent==0)){CBYTE(parent)|=1; goto finput;}  // no grandparent; safe to make parent (the root) black
   uncle=RDIR(gparent,gparentd^1);  // get parent's sibling (w/o dir)
   if(uncle<(TREENRES<<1)||CBYTE(uncle)&1)break;  // if uncle is black,  we can get out
   // exit criteria not matched.  Go up two levels to work on this node's grandparent & greatgrandparent;
// obsolete printf("paint parent & uncle black, gparent red, keep looking\n");  // scaf
   CBYTE(parent)|=1; CBYTE(uncle)|=1; CBYTE(gparent)&=~1;   // mark parent & uncle black, gparent red.  This fixes snodex & parent but may leave a red vio at gparent
   snodex=gparent;  // the node to work on next, always red
   parent=pdir[pi-3]; parentd=PDIRDIR(parent); parent=PDIRNODE(parent);  // back up to parent
   pi-=2;  // we pop up to the grandparent
  }
  // snodex is red, parent is red, grandparent & uncle are black.  We can get out in 1 or 2 rotations.
  if(gparentd!=parentd){
// obsolete printf("rotate 1\n");  // scaf
   // snodex is an inner child, i. e. left child of right child or the reverse.  Rotate snodex to where the parent is.
// obsolete    DRNFC(parent,parent,parentd)
   ROT(parent,parentd,gparent,gparentd,snodex)  // rotate inner child to parent
   parent=snodex;  // snodex moves to the parent position for the final rotate
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf
  }
  
// obsolete printf("rotate 2\n");  // scaf
// obsolete   --pi; // align for rotating gparent
// obsolete   DRNFC(gparent,gparent,gparentd)  // read children to rotate
  ROT(gparent,gparentd,PDIRNODE(pdir[pi-3]),PDIRDIR(pdir[pi-3]),parent)  // rotate parent (original red snodex if we did first rotate) over black grandparent
  CBYTE(gparent)^=1; CBYTE(parent)^=1;  // make parent black, gparent red, removing the violation
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf

finput:;  // install the new value, and maybe the key, at the kv slot corresponding to tree-node nodex
          // if LSB of nodex is set, suppress copying the key
  if(!(nodex&1))PUTKVNEW(kbase+(nodex>>1)*(kib>>32),k,kib>>32,nodeb&(DICFKINDIR<<8));   // k/v base are biased for copy
  PUTKVOLD(vbase+(nodex>>1)*vb,(void*)((I)v+(likely(i<vn)?i:i%vn)*vb),vb,nodeb&(DICFVINDIR<<8));
    // If we saved the kv#s, we could release the keys after the loop through the keys, and then copy the values at the end.  We don't because the search is so slow

  k=(void *)((I)k+(kib>>32));  // advance to next search key
 }
 R lv|DICLMSKOKRET;  // good return, holding lock
resize: R lv|DICLMSKOKRET+DICLMSKRESIZEREQ;  // signal need for resize; return holding write lock on keys & values
errexit: R 0;
}

//  put.  a is values, w is keys   dic was u to self
static DF2(jtdicputo){F12IP;
 ARGCHK2(a,w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.vtype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 I af=AR(a)-vr; ASSERT(af>=0,EVRANK) ASSERTAGREE(AS(a)+af,vs,vr)   // v must be a single value or an array of them, with correct shape
 I cf=MIN(af,wf); ASSERTAGREE(AS(a)+af-cf,AS(w)+wf-cf,cf)  // frames must be suffixes
 if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0)) if(unlikely((AT(a)&vt)==0))RZ(a=ccvt(vt,a,0))  // convert type of k and v if needed
 I kn; PROD(kn,wf,AS(w)) I vn; PROD(vn,wf,AS(w))   // kn = number of keys to be looked up  vn=#values to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index
 // We do not have to make incoming kvs recursive, because the keys/vals tables do not take ownership of the kvs.  The input kvs must have their own protection, valid over the call.
 // For the same reason, we do not have to worry about the order inwhich kvs are added and deleted.

 VIRT virt; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

// obsolete printf("put prerwrq, lk=0x%016llx\n",AM((A)dic));  // scaf
 UI lv; DICLKRWRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
// obsolete printf("put rwrq, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
 void *k=voidAV(w), *v=voidAV(a);  // point to the key and value data
 while(1){  // loop till we have processed all the resizes
// obsolete   if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
// obsolete   else{A x; GATV0E(x,FL,kn,1,goto abortexit;) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
// obsolete     // we have to reinit s in the resize loop because putslots may have modified it
 
  lv=jtputslotso(dic,k,kn,v,vn,jt,lv,virt); if(lv==0)goto errexit; if(likely(!(lv&DICLMSKRESIZEREQ)))break;  // do the puts; if no resize, finish, good or bad
// obsolete printf("after putslotso, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
  if(dicresize(dic,jt)==0)goto errexit;  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
  lv&=~(DICLMSKRESIZEREQ+DICLMSKOKRET);  // remove return flags from lv
 }
 A z=mtv; if(0){errexit: z=0; lv=DICLMSKWRV;}   // set lv so as to allow updating the current-owner flag - even if singlethreaded, since it doesn't matter then
 PRISTCLRF(w);    // we have taken from w; remove pristinity.  This destroys w.  We do this even in case of error because we may have moved some values before the error happened
// obsolete printf("put before final relv, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
// obsolete printf("put final relv, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
 RETF(z);
}


// del

// k is A for keys,
// resolve each key in the hash and delete them
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; rc=0 if error, otherwise current lv (which gives lock status), with OKRET set to be non0 and RESIZEREQ is a resize is needed
// zv is the return status for each key, indicating that the key was deleted (initialized to 'all found'
static INLINE UI8 jtdelslotso(DIC *dic,void *k,I n,J jt,UI lv,VIRT virt,B *zv){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;   // more nonresizable: kbytelen/kitemlen   compare fn  prototype required to get arg converted to I
 I nodeb=dic->bloc.hashelesiz*(0x1000000+SZI)+(dic->bloc.emptysiz<<19)+(dic->bloc.flags<<8);  // (#bytes in node index)\(#bits in empty-chain field\(flags)\(number of bits in a node index)
 I vb=dic->bloc.vbytelen;   //  len of 1 value
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 C *hashtbl=CAV3(dic->bloc.hash);  // pointer to tree base
 C *kbase=CAV(dic->bloc.keys)-TREENRES*(kib>>32), *kbasei=kbase;  // address corresponding to tree value of 0.  treevalues 0-1 are empty and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals);  // same for value

 DICLKWRRQ(dic,lv); DICLKWRWTK(dic,lv)  // request write lock and wait for keys & values to be modifiable.

 // loop over keys (reverse order).  Find the key, building parent info; then delete the key and value
 // Because we don't store parent info, we have to do each key separately, which requires keeping a write
 // lock after the first key - so we go ahead and keep the write lock throughout.
 if(unlikely(!(nodeb&(DICFICF<<8))))biasforcomp   // we compare, not copy, and we use the original addr for the delete
 void *ki=(void *)((I)k+(n-1)*(kib>>32));  // pointer to key being compared
 for(i=n-1;i>=0;--i){I nodex;
  UI8 chirn;  // both children
  UI4 pdir[64]; I pi=0;   // parent/direction history; next slot to fill
  UI parent=0, rootx=*(UI4*)hashtbl&_bzhi_u64(~(UI8)1,nodeb);  // biased node# of the root of the tree
  I comp=1;  // will be compare result.  The root is considered to have compared low
  // the root is pointed to by hash0/dir0.  In an empty database hash0/dir0=0.  In that case we fiddle things so that the first key is called new and the others are called conflict.  They all point to
  // hash0/dir0 as the end-of-search point.  The new will fill hash0/dir0 with a node, and the rest will start their search at that node, which is the true root of the tree.
  if(unlikely(rootx<(TREENRES<<1)))goto notfound; // empty database: nothing to do
  for(nodex=rootx;;){  // traverse the tree, searching for index k.  Current node is nodex
   chirn=*(UI8*)&hashtbl[nodex*(nodeb>>24)];  // fetch both children
   pdir[pi++]=parent+=SGNTO0(comp);  // stack parent/dir going into nodex
   comp=keysne((UI4)kib,kbase+(kib>>32)*(nodex>>1),ki,nodeb&(DICFICF<<8),errexit);  // compare node key vs k, so k > node is ~0
   if(comp==0)break;  // found at node nodex.  chirn have the children, parent the parent\dir (also in pdir[])
   parent=nodex;  // we will fetch again.  remember the parent of the next fetch
   nodex=(chirn>>(comp&nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb);  // choose left/right based on comparison, mask out garb.
   if(unlikely(nodex<(TREENRES<<1)))goto notfound;  // not found.
  }
  // we found the node to be deleted: nodex, whose children are in chirn.  pdir[pi-1] matches parent
  I nodexlc=_bzhi_u64(chirn,nodeb), nodexr=(chirn>>(nodeb&0xff))&_bzhi_u64(~(UI8)1,nodeb);  // the children\color
// obsolete printf("delete 0x%x: lc=0x%x r=0x%x\n",(int)nodex,(int)nodexlc,(int)nodexr);  // scaf
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf
  if((nodexlc&~1)&&nodexr){  // 2 children
// obsolete printf("2 children ");  // scaf
   // 2 children.  find in-order successor (last consecutive left child of right subtree), swap nodex with successor, continue
// obsolete    I nodexpi=pi; UI lsucc=nodexr|(nodexlc&1); ++pi;   // remember level of parent that will be swapped; init leftmost successor;  skip parent slot holding to-be-swapped node
   I nodexpi=pi; UI lsucc=nodexr; ++pi;   // remember level of parent that will be swapped; init leftmost successor;  skip parent slot holding to-be-swapped node
   DLRC(lsucc)  // decls for reading chirn
   while(1){
    // drop down to probe, updating parent chain
    RLRC(lsucc,lsucc)  // fetch probe
    if(lsuccl==0)break;  // stop when pointing to the first node that has no left child (i. e. smaller key)
    pdir[pi++]=lsucc; lsucc=lsuccl;  // remember parent & direction (always left), move to leftmost child
   }
   // now lsucc is the successor\0, lsuccl (always 0) and lsuccr are its children.  Put it where nodex was - keeping old colors
   WLRC(parent&~1,parent&1,lsucc,CBYTE(parent&~1)&1)   // parent[dir]=successor
   pdir[nodexpi]=lsucc|1;   // successor is parent of right chain if any
// obsolete    RLRC(lsucc,lsucc&~1)  // read successor children\color
   WLC(nodex,0,lsuccc) WR(nodex,lsuccr)   // put succ's children/color into nodex.  successor has no left child.  Keep color in tree, which means swapping it between nodes
   WLC(lsucc,nodexlc&~1,nodexlc&1) WR(lsucc,nodexr)  // put nodex's children/color into succ.  Keep color unchanged
   parent=pdir[pi-1];  // restore parent\dir to that of successor
   // no need to write nodex into parent - we are about to expunge nodex which will modify parent
// obsolete    WLRC(parent&~1,parent&1,nodex&~1,CBYTE(parent&~1)&1)   // scaf when auditing parent[dir]=successor

   nodexlc=lsuccl+lsuccc; nodexr=lsuccr;  // estab children of node to be deleted
// obsolete printf("succ=0x%x, lc=0x%x, r=0x%x\n",(int)lsucc,(int)nodexlc,(int)nodexr);  // scaf
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),nodex&~1,1);  // scaf
  }

  //  we must always get to here if there is a node to delete.  nodex now has 0-1 child, which we have
  // extracted.  We now remove nodex from the tree, never to be referred to again.  It is always the
  // child of its parent, and only the chain field in the parent is modified - nothing in nodex
  I emptyx=RENCEMPTY(nodex);  // put new deletion at top of the free chain, unbiased
  DELKV(kbasei+(emptyx+TREENRES)*(kib>>32),kib>>32,nodeb&(DICFKINDIR<<8)) DELKV(vbase+emptyx*vb,vb,nodeb&(DICFVINDIR<<8))   // if k/v is indirect, free it & clear to 0
  I t=dic->bloc.emptyn; WRHASH1234(t, nodeb>>19, &kbasei[(emptyx+TREENRES)*(kib>>32)])  // chain old free chain from new deletion
  dic->bloc.emptyn=emptyx;  // set new head of chain
  --dic->bloc.cardinality;  // remove deleted key from key count

  I child=(nodexlc|nodexr)&~1;  // the one child, if not 0
  if(child){  // is there a child?
// obsolete printf("1 child\n");  // scaf
   // the node has 1 child.  color the child black, and replace the node with the child (i. e. in the parent)
   CBYTE(child)|=1;  WLRC(parent&~1,parent&1,child,CBYTE(parent&~1)&1)   // parent[dir]=child
   goto findel;  // finished
  }
  // no children.
  WLRC(parent&~1,parent&1,0,CBYTE(parent&~1)&1)   // parent[dir]=NULL.  This removes nodex from the tree
  if(!(nodexlc&1))goto findel;   // childless red node.  delete it (i. e. in the parent), finished
  // black node without descendants.  full rebalance required
  DSOC(parent); I parentd,sibling; DSOC(sibling);  // parent names incl direction; sibling names
  I gparent;  // will hold node\dir of parent
  // parent matches pdir[--pi].
  while(1){
   // rebalancing loop.  We look only at parents & siblings of nodex, never nodex itself, which doesn't exist first time through
   parentd=parent&1; parent&=~1;  // from here on split parent & parentd
   if(!(parent))goto findel;  // top of tree, we're done
   // If there is red either in the parent, the sibling, or the sibling's children, we can restore black level by a rotation sequence.
   // we loop here till we see the red
   gparent=pdir[pi-2];  // unroll going up the tree, getting grandparent's node\dir
   RSOC(parent,parent,parentd) sibling=parento;  // fetch parents' children.  Same is the current node, opposite is the sibling.  Both must exist, but at the bottom the same has a stale pointer to the successor
   if(!(CBYTE(sibling)&1)){  // sibling red?
    // sibling is red.  We rotate it to be grandparent, which will leave snodex with a red parent
// obsolete printf("sibred\n");  // scaf
    ROT(parent,parentd^1,PDIRNODE(gparent),PDIRDIR(gparent),sibling)  // rotate parentn (red sibling) into parent.  It is in parent[parentd^1]
    CBYTE(sibling)|=1; CBYTE(parent)&=~1; parentc=0; // set sibling black, parent red (and update saved parent color to red, which will get us out of loop)
    gparent=sibling+parentd;  // old sibling is now black grandparent; but keep the original parent direction in node\dir
    sibling=RDIR(parent,parentd^1);   // Fetch new sibling, which is perforce black
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf
   }
   // Here sibling is black (it may have been a same-child, now swapped into that position)
   RSOC(sibling,sibling,parentd)  //  fetch sibling's children, as same/opp
   if(siblingo&&!(CBYTE(siblingo)&1))goto onephred;  // opp nephew red, go handle it in 1 rotation
   if(siblings&&!(CBYTE(siblings)&1))goto snephred;  // otherwise, same nephew red, go handle it in 2 rotations
   // sibling is black and has only black children
   if(!parentc)goto parred;  // parent red, go handle it
// obsolete printf("looping looking for red\n");  // scaf
   // parent, sibling, nephews (if any) all black.
   CBYTE(sibling)&=~1;   //  Turn the sibling red (which makes the tree rooted in parent balanced but still shorter than the one rooted in the parent's sibling).
   parent=gparent; --pi;  // Move up one level, continue rebalancing there
  }  // never fall through

  // rebalancing runout.  We have seen red, and can finish in at most 3 rotations

parred:;  // parent red, no red in sibling
// obsolete printf("parred\n");  // scaf
  CBYTE(parent)|=1; CBYTE(sibling)&=~1;  // set sibling red, parent black
  goto findel;  // finished
snephred:;  // opp nephew black, but same nephew red
// obsolete printf("snephred\n");  // scaf
  ++pi; ROT(sibling,parentd,parent,parentd^1,siblings) --pi;  // rotate red same nephew into sibling
  CBYTE(sibling)&=~1; CBYTE(siblings)|=1;  // set sibling red, former same nephew (now parent of sibling) black
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf
  siblingo=sibling; sibling=siblings;  // set up assignments for after fallthrough and rotation
  // fall through to...
onephred:;   // opp nephew red
// obsolete printf("onephred\n");  // scaf
  ROT(parent,parentd^1,PDIRNODE(gparent),PDIRDIR(gparent),sibling)  // rotate sibling up to parent
  CBYTE(sibling)=(CBYTE(sibling)&~1)|parentc;  // move parent's color to new parent
  CBYTE(parent)|=1;  // mark old parent (now near sibling) black
  CBYTE(siblingo)|=1;  // mark old opp nephew (now sibling) black
  // fall through to...
findel:;  // here when balancing complete
// obsolete printf("findel\n");
// obsolete auditnode(jt,dic,*(UI4AV3(dic->bloc.hash))&_bzhi_u64(~(UI8)1,dic->bloc.hashelesiz<<LGBB),~0LL,1);  // scaf
  if(0){notfound: zv[i]=0;}  // if key not found, so indicate in return
  if(unlikely(!(nodeb&(DICFICF<<8))))unbiasforcomp
  ki=(void *)((I)ki-(kib>>32));  // advance to next search key
// obsolete printf("looping for next key\n");
 }
 R lv|DICLMSKOKRET;  // good return, holding lock
// resize: R lv|DICLMSKOKRET+DICLMSKRESIZEREQ;  // signal need for resize; return holding write lock on keys & values
errexit: R 0;
}

//  del.   w is keys   u was dic to self
static DF1(jtdicdelo){F12IP;
 ARGCHK1(w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.ktype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))  // convert type of k  if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a 32-bit index with LSB=0

 VIRT virt; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

// obsolete printf("del prerwrq, lk=0x%016llx\n",AM((A)dic));  // scaf
 A z; GATVE(z,B01,kn,wf,AS(w),goto errexit;) C *zv=CAVn(wf,z); mvc(kn,zv,1,MEMSET01);   // allocate return area & init to 'all found'
 UI lv; DICLKRWRQ(dic,lv,dic->bloc.flags&DICFSINGLETHREADED);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
// obsolete printf("del rwrq, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
 void *k=voidAV(w);  // point to the key and value data
 while(1){  // loop till we have processed all the resizes
  lv=jtdelslotso(dic,k,kn,jt,lv,virt,zv); if(lv==0)goto errexit; if(1||likely(!(lv&DICLMSKRESIZEREQ)))break; // do the dels; if no resize (not supported yet), finish, good or bad
// obsolete printf("del after delslotso, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
  dicresize(dic,jt);  // If we have to resize, we abort with the dels partially complete, and then retry, keeping the dic under lock the whole time
 }
 if(0){errexit: z=0; lv=DICLMSKWRV;}   // set lv so as to allow updating the current-owner flag - even if singlethreaded, since it doesn't matter then
// obsolete printf("del before final relv, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
 DICLKWRRELV(dic,lv)    // we are finished.  advance sequence# and allow everyone to look at values.  We must have a lock
// obsolete printf("del final relv, lv=%016llx lk=0x%016llx\n",lv,AM((A)dic));  // scaf
 RETF(z);
}

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- user-level primitives *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-


// u 16!:_2  get: u=dic
// We create a verb to handle (get y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicgetc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 A z; fdefallo(z);
 fdeffillall(z,0,CMODX,VERB, !(((DIC*)w)->bloc.flags&DICFRB)?jtdicget:jtdicgeto,!(((DIC*)w)->bloc.flags&DICFRB)?jtdicget:jtdicgeto,w,self,0, VFLAGNONE, RMAX,RMAX,RMAX,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.varno=0);
 R z;
}

// u 16!:_12  has: u=dic
// We create a verb to handle (has y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdichasc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 A z; fdefallo(z);
 fdeffillall(z,0,CMODX,VERB, !(((DIC*)w)->bloc.flags&DICFRB)?jtdicget:jtdicgeto,jtvalenceerr,w,self,0, VFLAGNONE, RMAX,RMAX,RMAX,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.varno=1);
 R z;
}

// u 16!:_3  put: u=dic
// We create a verb to handle (x put y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicputc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(0,CMODX,VERB, jtvalenceerr,!(((DIC*)w)->bloc.flags&DICFRB)?jtdicput:jtdicputo, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// u 16!:_4  del: u=dic
// We create a verb to handle (del y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicdelc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(0,CMODX,VERB, !(((DIC*)w)->bloc.flags&DICFRB)?jtdicdel:jtdicdelo,jtvalenceerr, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// u 16!:_6  getkv: u=dic
// We create a verb to handle (del y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicgetkvc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 ASSERT(((DIC*)w)->bloc.flags&DICFRB,EVDOMAIN)    // requires red/black tree
 R fdef(0,CMODX,VERB,jtdicgetkvo,jtdicgetkvo, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// x 16!:_5 dic   return list of empty keyslots.  If x=1, also delete the empty chainfields
// No locks; if you need a write lock, take it before calling.
DF2(jtdicempties){F12IP;
 ARGCHK2(a,w)
 DIC *dic=(DIC*)w;
 UI lv;
 I x; RE(x=i0(a)); ASSERT(BETWEENC(x,0,1),EVDOMAIN)  // x must be 0 or 1
 I nodeb=(dic->bloc.emptysiz<<LGBB); UI kb=dic->bloc.kbytelen;  // #bits in empty-chain field; #bytes in key
 C *kv=CAV(dic->bloc.keys);   // point to start of keys
 I nempty=0; UI emptyx;  // # of empties & index of first one
 for(emptyx=dic->bloc.emptyn;;){
  UI emptynxt=_bzhi_u64(*(UI4*)&kv[emptyx*kb],nodeb); ++nempty;  // count this empty, get next one
  if(emptynxt==emptyx)break;  // If loopback (EOC), stop counting
  emptyx=emptynxt;  // advance to next
 }
 A z; GATV0(z,INT,nempty,1) I *zv=IAV1(z);  // allocate result; pointer to first atom
 for(nempty=0,emptyx=dic->bloc.emptyn;;){
  zv[nempty]=emptyx;  // return index of empty
  UI emptynxt=_bzhi_u64(*(UI4*)&kv[emptyx*kb],nodeb); ++nempty;  // count this empty, get next one
  if(x==1){I t=0; WRHASH1234(t, nodeb>>3, &kv[emptyx*kb])}  // optionally erase the chain
  if(emptynxt==emptyx)break;  // If loopback (EOC), stop counting
  emptyx=emptynxt;  // advance to next
 }
 if(x==1){dic->bloc.emptyn=-1;}  // set chain empty if we have deleted it.  The noun is still undisplayable
 RETF(z)
 }

// x 16!:_8 dic   return dic stats
DF2(jtdicstats){F12IP;A z;
 ARGCHK2(a,w)
 DIC *dic=(DIC*)w;
 I type; RE(type=i0(a))   // get the stat arg
 switch(type){
 case 0: z=sc(dic->bloc.cardinality); break; // nkeys
 default: ASSERT(0,EVDOMAIN)
 }
 RETF(z);
}


#endif