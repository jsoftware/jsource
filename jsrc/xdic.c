/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Dictionary support                                                      */

#include "j.h"


#define scafINLINE NOINLINE  // scaf
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
static scafINLINE UI4 crcfloats(UI8 *v, I n){
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
static scafINLINE UI4 crcwords(UI *v, I n){
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
static scafINLINE UI4 crcbytes(C *v, I n){
 UI4 wdcrc=crcwords((UI*)v,n>>LGSZI);  // take CRC of the fullword part
 if(n&(SZI-1)){wdcrc=HASHI(wdcrc,((UI*)v)[n>>LGSZI]<<((SZI-(n&(SZI-1)))<<LGSZI));}  // if there are bytes, take their CRC after discarding garb.  Avoid overfetch
 R wdcrc;  // return composite CRC
}

// CRC32 of n boxes
static UI4 jtcrcy(J jt,A y);
static scafINLINE UI4 crcboxes(J jt, A *v, I n){UI4 crc=0; DO(n, crc=HASH4(crc,jtcrcy(jt,C(v[i])));) R crc;}

// CRC32 of n Xs
static scafINLINE UI4 crcxnums(X *v, I n){UI4 crc=0; DO(n, crc=HASH4(crc,crcwords(UIAV(v[i]),XLIMBLEN(v[i])));) R crc;}

// CRC32 of y.  Floats must observe -0.
static UI4 jtcrcy(J jt,A y){
 I yt=AT(y), yn=AN(y); void *yv=voidAV(y);  // type of y, #atoms, address of data
 if(yt&INT+(SZI==4)*(C4T+INT4))R crcwords(yv,yn);  // INT type, might be full words
 if(yt&B01+LIT+C2T+C4T+INT1+INT2+INT4)R crcbytes(yv,yn<<bplg(yt));   // direct non-float
 if(yt&FL+CMPX+QP)R crcfloats(yv,yn<<(((UI)yt>=CMPX)));  // float
 if(yt&BOX){R crcboxes(jt,yv,yn);}  // box, take CRC of all the boxes
 if(yt&RAT+XNUM){R crcxnums(yv,yn<<((UI)yt>=RAT));}  // RAT/XNUM, take CRC of all of them
 else SEGFAULT;  // scaf
}

// 16!:0 monad create 32-bit hash of y.  For float types, +-0 must have the same hash
DF2(jthashy){F12IP; ARGCHK2(a,w) RETF(sc(jtcrcy(jt,w)));}

#if 1   // obsolete 
// everything about the dictionary.  This struct overlays the entire block.  AM of the first
// cacheline is the lock for the dic.  The data starts at AS[1], the second cacheline.  The block
// is a recursive BOX but AK points past the areas that do not need to be freed, i.e keys
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
// NOTE bit 7 of flags must be 0, used as LSB of a length
    C lgminsiz;  // lg(minimum cardinality).  When cardinality drops below 1LL<<lgminsiz, we resize.  Top 3 bits always 0, used as LSBs of hashelesiz
    C hashelesiz;  // number of bytes in a hash slot, 1-5 (we support only 4)
   };
   UI8 hashsiz;  //  number of elements in hash table.  We leave space for 40 bits but we don't support more than 32
  } ;
  UI4 (*hashfn)();  // hash function, user's or selected internal
  I (*compfn)();  // key-comparison function, user's or selected internal
   // *** start of area pointed by AK, which is the value of dic as a J noun
  A hash;  // the hash table, rank 1  Note: This pointer may be stale.  MUST use voidAV1 to point to hash, and use only PREFETCH until you get a lock
  A keys;  // array of keys
  A vals;   // array of values
  A empty;  // LIT shape (maxeles,hashelesiz).  occupied kvs are -1; empties are in a chain that loops to itself at the end, rank 1
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
// 4 bytes free
  // end of second cacheline.  Following changed only by put/del
  UI cardinality;  // number of kvs in the hashtable
  UI emptyn;  // index to next empty kv.  EOC loops back on itself.  Resize when empty
  I filler2[SY_64?6:3];  // pad to cacheline (24 words on each system).
 } bloc;
} DIC;
#define ST UI4   // type of hash slot
#define STX UI8   // type of index to hash slot, which is + for found, 1s-comp for not found
#define STN 4  // width of hash slot

// u 16!:_1 y  adverb.  allocate hashtable.  Always called from the numbered locale of the dictionary.
// u is the hash/compare verb OR the DIC block for a dictionary that is being resized.
// y is (min #eles,max #eles,#hashslots);(flags, currently empty);(key type;key shape);(value type;value shape)    to create a new dictionary
//      (min #eles,max #eles,#hashslots)   when a dictionary is being resized
// result is DIC to use, ready for get/put, with hash/keys/vals/empty allocated, and 0 valid kvs
static DF1(jtcreatedic1){F12IP;A box,box1;  // temp for box contents
 ARGCHK1(w);
 A a=FAV(self)->fgh[0];  // extract u from the verb
 A z; GAT0(z,BOX,20,1) AK(z)=offsetof(struct ADic,bloc.hash); AN(z)=AS(z)[0]=8;  // allocate nonrecursive box, long enough to make the total allo big enough in 32- and 64-bit.  Then restrict to the boxes in case of error
 I flags;  // part of a union that gets overwritten
 if(AT(a)&VERB){  // initial creation
  //  install user's verb, & see if the user wants the internal functions
  AF fn=FAV(a)->valencefns[0]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[0])->valencefns[0]:fn; flags=fn==jthashy?DICFIHF:0;  // note if default hash
  fn=FAV(a)->valencefns[1]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[1])->valencefns[1]:fn; flags|=fn==jttao?DICFICF:0;  // note if default comp
  INCORPNV(a); ((DIC*)z)->bloc.hashcompself=a;    // save self pointer to user's function, which protects it while this dic is extant
  ((DIC*)z)->bloc.locale=jt->global;  // remember the locale this dictionary is in.  We protect it in the DIC block

  // flags, key, value sizes
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==4,EVLENGTH)  // 4 boxes
  box=C(AAV(w)[1]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==0,EVLENGTH)  // flags.  None currently supported

  // keyspec.  must be 2 boxes
  box=C(AAV(w)[2]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)
  box1=C(AAV(box)[0]); I t; RE(t=i0(box1)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN) flags|=t&DIRECT?0:DICFKINDIR; // type.  convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid.  Remember if indirect
  box1=C(AAV(box)[1]); ASSERT(AR(box1)<=1,EVRANK) ASSERT(AN(box1)>=0,EVLENGTH) RZ(box1=ccvt(INT,ravel(box1),0)) I n, *s=IAV(box1); PRODX(n,AN(box1),s,1) ((DIC*)z)->bloc.kaii=n; ASSERT(n>0,EVLENGTH) // shape. copy to allow IAV1.  get # atoms in item & save
  ASSERT(AN(box1)<=9 || flags&DICFICF,EVNONCE)  // if the user has a compare function, we want virt to be on the stack to save registers.  Make sure the rank is OK then
  INCORPNV(box1); ((DIC*)z)->bloc.kshape=box1; ((DIC*)z)->bloc.ktype=t; I l=n<<bplg(t); ((DIC*)z)->bloc.kbytelen=l; // save shape & type; save #bytes in key
  UI4 (*fn2)()=l&(SZI-1)?(UI4 (*)())crcbytes:(UI4 (*)())crcwords; fn2=(t&XNUM+RAT)?crcxnums:fn2; fn2=(t&CMPX+FL+QP)?crcfloats:fn2; fn2=(t&BOX)?crcboxes:fn2; fn2=flags&DICFIHF?fn2:(UI4 (*)())FAV(a)->valencefns[0]; ((DIC*)z)->bloc.hashfn=fn2; // save internal or external hash function  
  I (*fn3)()=l&(SZI-1)?(I (*)())taoc:(I (*)())taoi; fn3=(t&XNUM+RAT)?taox:fn3; fn3=(t&CMPX+FL+QP)?taof:fn3; fn3=(t&BOX)?taor:fn3; fn3=flags&DICFICF?fn3:(I (*)())FAV(a)->valencefns[1]; ((DIC*)z)->bloc.compfn=fn3; // save int/ext comp function.  We care only about equality  
  I lsh=l&(SZI-1)?0:LGSZI; lsh=(t&BOX+XNUM+RAT)?LGSZI:lsh; lsh=(t&CMPX+FL+QP)?FLX:lsh; ((DIC*)z)->bloc.kitemlen=l>>lsh;  // length to use for internal hash/comp

  // valuespec.  must be 2 boxes
  box=C(AAV(w)[3]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)
  box1=C(AAV(box)[0]); RE(t=i0(box1)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN) flags|=t&DIRECT?0:DICFVINDIR;  // type. convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid.  Remember if indirect
  box1=C(AAV(box)[1]); ASSERT(AR(box1)<=1,EVRANK) ASSERT(AN(box1)>=0,EVLENGTH) RZ(box1=ccvt(INT,ravel(box1),0)) s=IAV(box1); PRODX(n,AN(box1),s,1) ((DIC*)z)->bloc.vaii=n;  // shape. copy to allow IAV1.  get # atoms in item & save
  INCORPNV(box1); ((DIC*)z)->bloc.vshape=box1; ((DIC*)z)->bloc.vtype=t; ((DIC*)z)->bloc.vbytelen=n<<bplg(t);  // save shape & type; save # bytes for copy
  ((DIC*)z)->bloc.flags=flags;  // now that we have all the flags, save them
  box=C(AAV(w)[0]);  // fetch size parameters

 }else{  // resize
  flags=((DIC*)a)->bloc.flags;  // preserve flags over hashsiz write
  ((DIC*)z)->bloc=((DIC*)a)->bloc;  // init everything from the previous dic
  box=w;  // w is nothing but size parms
 }

 // box has the size parameters.  Audit & install into dic, overwriting anything that was copied.  But keep the 
 ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==3,EVLENGTH) if(!AT(box)&INT)RZ(box=ccvt(INT,box,0));  // sizes. must be box of 3 integers
 I hashsiz=((DIC*)z)->bloc.hashsiz=IAV(box)[2];   // move hashsiz, which overwrites flags/lgminsiz/hashelesiz
 ((DIC*)z)->bloc.lgminsiz=CTLZI(UIAV(box)[0]|1); I maxeles=IAV(box)[1]; ASSERT(maxeles>0,EVDOMAIN)   // save min size, get max# eles
 ASSERT(hashsiz>(maxeles+(maxeles>>4)),EVDEADLOCK)  // min, max, hash sizes.  Hash at least 6% spare
 I hashelesiz=((DIC*)z)->bloc.hashelesiz=(CTLZI(maxeles+4-1)+1+(BB-1))>>LGBB;  // max slot#, plus 4 (empty/tombstone/birthstone).  Subtract 1 for max code point.  top bit#+1 is #bits we need; round that up to #bytes
 ((DIC*)z)->bloc.flags=flags;   // restore flags

 // allocate & protect hash/keys/vals/empty
 GATV0(box,LIT,hashsiz*hashelesiz,1) INCORPNV(box) mvc(hashsiz*hashelesiz,voidAV1(box),MEMSET00LEN,MEMSET00); ((DIC*)z)->bloc.hash=box;  // allocate hash table & fill with empties
 GATV0(box,LIT,maxeles*hashelesiz,2) AS(box)[0]=maxeles; AS(box)[1]=hashelesiz; INCORPNV(box) void *ev=voidAV2(box); DO(maxeles-1, *(I*)ev=i+1; ev=(void *)((I)ev+hashelesiz);)   // allocate empty list & chain empties together
 *(I*)ev=maxeles-1; ((DIC*)z)->bloc.empty=box; // install end of chain loopback and point the dic to beginning of chain
 I t=((DIC*)z)->bloc.ktype; A sa=((DIC*)z)->bloc.kshape;   // key type & shape
 GA0(box,t,maxeles*((DIC*)z)->bloc.kaii,AN(sa)+1) AFLAG(box)=t&RECURSIBLE; INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(sa),AN(sa)) ((DIC*)z)->bloc.keys=box;   // allocate array of keys, recursive
 t=((DIC*)z)->bloc.vtype; sa=((DIC*)z)->bloc.vshape;   // key type & shape
 GA0(box,t,maxeles*((DIC*)z)->bloc.vaii,AN(sa)+1) AFLAG(box)=t&RECURSIBLE; INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(sa),AN(sa)) ((DIC*)z)->bloc.vals=box;   // allocate array of vals, recursive
 ((DIC*)z)->bloc.cardinality=0;  // init the dic is empty
 ra0(z); INCORP(z); AM(z)=0;  // make z recursive, protecting descendants; INCORP and clear the lock
 RETF(z)
}

// u 16!:_1 y.  For some reason jtlr won't display CIBEAM when it is a conjunction, so we have to make it an adverb.  We save u for the verb
F1(jtcreatedic){F12IP;
 ARGCHK1(w)
 R fdef(0,CIBEAM,VERB,jtcreatedic1,jtvalenceerr,w,0,0,VFLAGNONE,RMAX,RMAX,RMAX);
}

// Dic locking ************************************************************

// We use a 2- or 3-step locking sequence.  In each step the new state is requested shortly before it is needed and then waited for when it is needed (to give the lock request time to propagate to all threads).
// Readers use the sequence
// lv=DICLKRDRQ(dic)  // request read lock
// DICLKRDWT(dic,lv)  // wait for read lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// DICLKRDREL(dic)  // release read lock

// Writers use the sequence
// lv=DICLKRWRQ(dic)  // request read/write lock
// DICLKRWWT(dic,lv)  // wait for read/write lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// lv=DICLKWRRQ(dic)  // request write lock
// DICLKWRWT(dic,lv)  // wait for write lock to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
// DICLKWRREL(dic,lv)  // release write lock

// A read lock guarantees that the control info and tables will not be modified by any thread in a way that could affect a get().  Other modifications are allowed.
// A read-write lock guarantees that the control info and tables will not be modified by any other thread.  No modification by this thread mey affect a get() in progress.
//  When a read-write lock is granted, the granted thread is guaranteed to be the next to write.  It has ownership of the write-lock and sequence# bits until it releases the lock by writing to the sequence#
// A write lock guarantees that no other thread has a lock of any kind.  Any modification is allowed.  The lock can then be advanced to a value lock, which declared that it will not touch the hash or the keys
// but may moodify values

// scaf to add: write-lock the keys & values separately; free the keys first, then the values

#if PYXES
// The AM field of dic is the lock. 0-15=#read locks outstanding 16-31=seq# of next write req 32=writelock request 33=valueslock req/active 48-63=seq# of current writer (if 32-33 not 00) or next writer (if 32-33 00)
#define DICLMSKRDX 0LL  // read locks outstanding
#define DICLMSKRWX 16LL  // write locks request, by size 0-3
#define DICLMSKWRX 32LL  // write lock requested by current write owner
#define DICLMSKSEQX 48LL  // sequence# of current write owner

#define DICLKRDRQ(dic,lv) lv=__atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL);  // put up read request
#define DICLKRDWTK(dic,lv) if(unlikely((lv&((I)1<<DICLMSKWRX))!=0))lv=diclkrdwtk(dic);  // if someone is writing, wait till they finish with hash/keys 
#define DICLKRDWTV(dic,lv) if(unlikely((lv&((I)2<<DICLMSKWRX))!=0))diclkrdwtv(dic);  // if someone is writing, wait till they finish with values
#define DICLKRDREL(dic)   __atomic_fetch_sub(&AM((A)dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL);  // remove read request

#define DICLKRWRQ(dic,lv) lv=__atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKRWX,__ATOMIC_ACQ_REL);   // put up prewrite request
#define DICLKRWWT(dic,lv) if(unlikely((US)(lv>>DICLMSKRWX)!=(US)(lv>>DICLMSKSEQX)))diclkrwwt(dic,lv);  // wait until we are the lead writer, which is immediately if there are no others
#define DICLKWRRQ(dic,lv) { __atomic_store_n((US*)((C*)&AM((A)dic)+4),(US)3,__ATOMIC_RELEASE); lv=__atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE); }   // put up write request for key & value (we are known to be the lead writer and own the write bit, which must be clear)
#define DICLKWRWT(dic,lv) if(unlikely((lv&((I)0xffff<<DICLMSKRDX))!=0))diclkwrwt(dic);  // wait until all reads are quiesced
#define DICLKWRRELK(dic,lv) __atomic_store_n((US*)((C*)&AM((A)dic)+4),(US)2,__ATOMIC_RELEASE);   // release the keys, while still writing to values
#define DICLKWRRELV(dic,lv) __atomic_store_n((UI4*)((C*)&AM((A)dic)+4),(((lv)>>32)&0xffff0000)+0x10000,__ATOMIC_RELEASE);   // remove write request and advance sequence#

// wait for read lock on dic.
static I diclkrdwtk(DIC *dic){I n;
 // We know we just put up a read request and saw a writer.  Rescind our read request and then quietly poll for the write to go away
 do{
  DICLKRDREL(dic)  // remove read request
  for(n=5;;--n){
   delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
   if(__atomic_load_n((US*)((I)AM((A)dic)+4),__ATOMIC_ACQUIRE)==0)break;  // wait until write is complete
  }
 }while(((n=__atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL))&((I)1<<DICLMSKWRX))!=0);  // put up our rd request again, wait for any write to finish
 R n;
}

// we have a read lock on dic; wait for values to be available, keeping the lock the whole time
static void diclkrdwtv(DIC *dic){I n;
 // We know we just put up a read request and saw a writer.  Rescind our read request and then quietly poll for the write to go away
 do{
  for(n=5;;--n){
   delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
   if(__atomic_load_n((US*)((I)AM((A)dic)+4),__ATOMIC_ACQUIRE)==0)break;  // wait until write is complete
  }
 }while(((n=__atomic_fetch_add(&AM((A)dic),(I)2<<DICLMSKRDX,__ATOMIC_ACQ_REL))&((I)1<<DICLMSKWRX))!=0);  // put up our rd request again, wait for any write to finish
 R;
}


// wait for read-write lock on dic.
static void diclkrwwt(DIC *dic, I lv){I n;
 // We know we just put up a read-write request and saw that we were not the first read-write requester.  Wait till we make it to the top.  At that point we have the lock, with no further action needed
 while(1){
  US ourseq=lv>>DICLMSKRWX, curseq=lv>>DICLMSKSEQX;  // see where we stand
  if(ourseq==curseq)R;   // exit when we get to the top
  delay(ourseq-curseq>1?50:20);  // delay a bit.  the long delay uses mm_pause.  We use it when we are not the next requester
  lv=__atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE);  // refresh status
 }
}

// wait for write lock on dic.
static void diclkwrwt(DIC *dic){I n;
 // We are the next writer.  We just wait for read requests to clear.  We could improve this my adapting the wait to the usage pattern
 for(n=5;;--n){
  delay(n<0?50:10);  // delay a bit.  the long delay uses mm_pause.
  if(__atomic_load_n((US*)&AM((A)dic),__ATOMIC_ACQUIRE)==0)R;  // wait until reads clear
 }
}


#else
#define DICLKRDRQ(dic,lv) lv=0;
#define DICLKRDWTK(dic,lv) // if someone is writing, wait till they finish with hash/keys
#define DICLKRDWTV(dic,lv)  // if someone is writing, wait till they finish with values
#define DICLKRDREL(dic)

#define DICLKRWRQ(dic,lv) lv=0;
#define DICLKRWWT(dic,lv) if(lv);
#define DICLKWRRQ(dic,lv) lv=0;
#define DICLKWRWT(dic,lv) 
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

 A newdic; RZ(newdic=jtunquote(jt,(A)dic,val,val));  // execute resize on the dic, returning new dic
 struct Dic t=dic->bloc; ((DIC*)dic)->bloc=((DIC*)newdic)->bloc; ((DIC*)newdic)->bloc=t;  // Exchange the parms and data areas from the new dic to the old.  Since they are recursive, this exchanges ownership and will cause the old blocks to be freed when the new dic is.
 // NOTE: we keep the old blocks hanging around until the new have been allocated.  This seems unnecessary for the hashtable, but we do it because other threads still have the old pointers and may prefetch from
 //  the old hash.  This won't crash, but it might be slow if the old hash is no longer in mapped memory

exit:;  // clean up from error
 tpop(_ttop);  // discard newdic and everything it refers to
 SYMSETGLOBAL(dic->bloc.locale);  // restore globals
 R (A)dic;  // always return the same block if no error.
}

// ************************************************ user compare function *****************************

// kl is keylength in bytes, ku is void* 'pointer' to key, kh is 'pointer' to key in keys array
// if we are calling a user compare function, the pointers are actually offsets and we use faux virtual blocks for the call
#define keysne(kl,ku,kh,cond) likely(cond)?((I (*)(I,void*,void*))*cf)(kl,ku,kh) :  /* internal compare function */ \
 ({ AK((A)virt.u)=(I)ku; AK((A)virt.h)=(I)kh; A ka; RZ(ka=((A (*)(J,A,A,A))*cf)(jt,(A)virt.u,(A)virt.h,virt.self)) likely(AN(ka))?BIV0(ka):0; })  /* user compare function */

typedef struct {
I __attribute__((aligned(CACHELINESIZE))) u[16-AKXR(0)/SZI];   //  block used to access user's key
I __attribute__((aligned(CACHELINESIZE))) h[16-AKXR(0)/SZI];   // block used to access key from hash
A self;   //
} VIRT;

// fill in virtual block to hold one key
#define initvirt(v,dic) {AFLAG(v)=AFUNINCORPABLE+AFRO; AT(v)=dic->bloc.ktype; AN(v)=dic->bloc.kaii; AC(v)=ACPERMANENT; AR(v)=AN(dic->bloc.kshape); MCISH(AS(v),IAV1(dic->bloc.kshape),AN(dic->bloc.kshape));}

#define biasforcomp {k=(void *)((I)k-(I)&virt.u); kbase=(void *)((I)kbase-(I)&virt.h); }   // bias k and kbase back so that their values can be used in AK of virt.u and virt.v
#define unbiasforcomp {k=(void *)((I)k+(I)&virt.u); kbase=(void *)((I)kbase+(I)&virt.h); }   // restore original pointers

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- unordered map (hashed) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

// Hashtable info ***********************
#define HASHNRES 4  // # reserved code points for empties: empty/birthstone/tombstone/tombstone+birthstone
#define HASHBSTN 1  // this bit set when the slot is a birthstone (a slot marked for fill by a put)
#define HASHTSTN 2  // this bit set when the slot is a tombstone (an empty slot that must not terminate a search for put)

// Red/black tree info
#define TREENRES 2  // # reserved slots for empties, also used to hold the root

// Macros to install/delete kvs or unload values, which have extra work to do for indirect types.  ind is a condition that is non0 for indirect type
#define PUTKVOLD(d,s,n,ind) {if(ind){A sa=((A*)(s))[i]; DO((n)>>LGSZI, A sa1=((A*)(s))[i+1]; A da=((A*)(d))[i]; ra(sa) ((A*)(d))[i]=sa; fa(da) sa=sa1;)}else{MC(d,s,n);}}   // when old kv exist
#define PUTKVNEW(d,s,n,ind) {if(ind){A sa=((A*)(s))[i]; DO((n)>>LGSZI, A sa1=((A*)(s))[i+1]; ra(sa) ((A*)(d))[i]=sa; sa=sa1;)}else{MC(d,s,n);}}  // when old kv are empty
#define GETV(d,s,n,ind) {if(ind){A sa=((A*)(s))[i]; DO((n)>>LGSZI, A sa1=((A*)(s))[i+1]; rareccontents(sa) ((A*)(d))[i]=sa; sa=sa1;)}else{MC(d,s,n);}}   // move value to result
#define DELKV(s,n,ind) {if(ind){A sa=((A*)(s))[i]; DO((n)>>LGSZI, A sa1=((A*)(s))[i+1]; fa(sa) (s)[i]=0; sa=sa1;)}}   // deleting old kv

// k is A for keys, n is #keys, s is place for slot#s.  Hash each key, store, prefetch (possibly using wrong hash)
// if s is 0, we are using a user hash; use uits return area as s
// result is 0 on error
static scafINLINE I8* jtkeyprep(DIC *dic, void *k, I n, I8 *s,J jt,A ka){I i;
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

// ********************************** get **********************************

// k is A for keys, s is slot#s, zv is result area (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy the value (or default) to the result
// a is default if given, 0 if not 
// We take a read lock on the table and return with it
static scafINLINE B jtgetslots(DIC *dic,void *k,I n,I8 *s,void *zv,J jt,A a, VIRT virt){I i;
 I lv; DICLKRDRQ(dic,lv);   // request read lock
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn; I vn=dic->bloc.vbytelen;   // more nonresizable: kbytelen/kitemlen   compare fn   size of a value in bytes 
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 DICLKRDWTK(dic,lv)   // wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 UI8 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz  prototype required to get arg converted to I
 C *kbase=CAV(dic->bloc.keys)-HASHNRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals)-HASHNRES*vn;  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the value array

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp

 // convert the hash slot#s to index into kvs.  Prefetch each value to get a leg up on moving the data.  We don't prefetch more to avoid needless bus bandwidth; perhaps we should check the length   scaf
    // the only advantage of the prefetch is that the value reads will clear earlier, allowing the fence when we end our lock to finish earlier
 for(i=n;--i>=0;){
  k=(void*)((I)k-(kib>>32));  // back up to next key
  I curslot=(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56); UI hval; // convert hash to slot# and then to byte offset;  place where biased kv slot# is read into
  while(1){
   s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv
   if(withprob(hval<HASHTSTN,0.3))break;   // if we hit an empty (incl birthstone empty but not a tombstone), that ends the search (not found)
   if(withprob(hval>=HASHNRES,0.6) && withprob(keysne((UI4)kib,k,kbase+(kib>>32)*hval,hsz&(DICFICF<<DICFBASE))==0,0.7)){PREFETCH(vbase+s[i]*vn); break;}  // if we hit a non-tombstone that matches the key, exit found
   // here not found
   if(unlikely((curslot-=(hsz>>56))<0))curslot+=(UI4)hsz*(hsz>>56);  // move to next hash slot, wrap to end if we hit 0
  }
 }

 // copy using the kv indexes we calculated.  Copy in ascending order so we can overstore    scaf overstore
 void *av=0;  // init to 'no default data pointer yet'.  We avoid checking the default until we know we need it
 I cur=s[0];  // unroll loop once.  cur if current value to work on
 DICLKRDWTV(dic,lv)  // wait till values are safe.  No resize is possible here
 for(i=0;i<n;++i){void *vv;  // pointer to value to move
  I nxt=s[i+1];  // fetch next value.  Overfetch OK
  if(likely(cur>=HASHNRES))vv=vbase+cur*vn;   // this is the main line, key found
  else{   // default required, which we deem rare.
   if(unlikely(av==0)){
    // First time through here. We have to audit it & convert if needed
    ASSERT(a!=0,EVINDEX)   // error if no default given
    ASSERT(AR(a)==AN(dic->bloc.vshape),EVRANK) ASSERTAGREE(AS(a),IAV1(dic->bloc.vshape),AR(a))  // verify default has correct rank & shape
    if(unlikely((AT(a)&dic->bloc.vtype)==0)){RZ(a=ccvt(dic->bloc.vtype,a,0))}   // convert precision if needed
    av=CAV(a);  // get address of values to move
    PRISTCLRF(a);  // Since we are taking out of a, it is no longer pristine.  this destroys a, but leaves it non0.
   }
   vv=av;    // use the default after it has been audited
  }
  GETV(zv,vv,vn,hsz&(DICFVINDIR<<DICFBASE)); zv=(void *)((I)zv+vn);   // move the data & advance popinter to next one   scaf JMC?
  cur=nxt;
 }
 R 1;
}

//   get.   Bivalent. w is keys, [a] is default value, 0 if monad.  Called from parse/unquote as a,w,self or w,self,self.  dic was u to self
static DF2(jtdicget){F12IP;A z;
 ARGCHK2(a,w)
 A adyad=w!=self?a:0; w=w!=self?w:a;  // if dyad, keep a,w, otherwise 0,w
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){GA0(z,dic->bloc.vtype,0,wf+AN(dic->bloc.vshape)) MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,IAV1(dic->bloc.vshape),AN(dic->bloc.vshape)) R z;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))   // convert type of w if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index

 I t=dic->bloc.vtype; A sa=dic->bloc.vshape;
 GA0(z,t,kn*dic->bloc.vaii,wf+AN(sa)) AFLAG(z)=t&RECURSIBLE; MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,IAV1(sa),AN(sa))   // allocate recursive result area & install shape
 // establish the area to use for s.  To avoid wasting a lot of stack space we use the virt-block area if that is not needed for user comp.  And if there is a user hash, we allocate
 // nothing & use the value returned by keyprep, which will be the result area from the user hash.
 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer
 if(unlikely(!(dic->bloc.flags&DICFIHF)))s=0;   // user hash function, keyprep will allocate s
 else if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
 else{A x; GATV0(x,FL,kn,1) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
 
 void *k=voidAV(w);  // point to the key data
 RZ(s=jtkeyprep(dic,k,kn,s,jt,w))  // convert keys to slot#, possibly changing s
 if(!jtgetslots(dic,k,kn,s,voidAV(z),jt,adyad,virt))z=0;  // get the values and take a read lock on the dic.  If error, pass error through
 DICLKRDREL(dic)  // release read lock
 RETF(z);
}

// write l bytes from x to address a.  Destroys x, which must be a name
#define WRHASH1234(x,l,a) {if((l)&4)*(UI4*)(a)=x;else{if((l)&1){*(C*)(a)=x;x>>=8;}if((l)&2)*(US*)((C*)a+((l)&1))=x;}}  // avoid read, which will probably be a long miss.  Branches will predict
// same for writing ~0
#define WRCONST1234(x,l,a) {if((l)&4)*(UI4*)(a)=(x);else{if((l)&1){*(C*)(a)=(x);}if((l)&2)*(US*)((C*)a+((l)&1))=(x);}}  // avoid read, which will probably be a long miss.  Branches will predict


// ********************************** put **********************************

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; rc=0 if error, rc=-1 to request a resize
static scafINLINE I jtputslots(DIC *dic,void *k,I n,void *v,I vn,I8 *s,J jt,I lv,VIRT virt){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;   // kbytelen/kitemlen  compare func unchanged by resize
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
    // with this lock we can add new kvs, or change an empty/tombstone to a birthstone; but no other hash changes, and no value overwrites

 UI8 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz  base of hashtbl
 C *kbase=CAV(dic->bloc.keys)-HASHNRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp

 // first pass over keys.  If key found, remember the biased kv# (will go to old chain).  If not found, remember the hashslot# and whether it was occupied by a birthstone; and make it a birthstone - will go to new or conflict chain
 // If we encounter a birthstone, that's a conflict.  We must remember the first tombstone we encounter because that will be the store point if we don't find a match
 // We search all the way to a hole so that if there is match we will find it before we take the write lock.  This has the benefit of speeding up the conflict search
 // We have to go in ascending order because later keys must overwrite earlier ones
 for(i=0;i<n;++i){
  I8 tomb1=~0; I8 curslot=(((UI8)s[i]*(UI4)hsz)>>32); UI hval; // first tombstone (init none); convert hash to slot#;  place to read biased kv slot# into
  while(1){
   s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
   if(withprob(hval<HASHNRES,0.3)){
    I8 hc=((I8)hval<<32)+curslot; tomb1=tomb1==~0?hc:tomb1;  // remember first spot we can store into, empty or tombstone, and its type
    if(withprob(hval<HASHTSTN,0.8)){hval=tomb1>>32; tomb1=(UI4)tomb1; hashtbl[tomb1]=hval|HASHBSTN; s[i]=((I8)0x100000000<<hval)+tomb1; break;}  // if we hit empty, we're done: mark first hole is birthstone; save its status before mark.  1 byte store for atomicity
   }else if(withprob(keysne((UI4)kib,(void*)((I)k+i*(kib>>32)),kbase+(kib>>32)*hval,hsz&(DICFICF<<DICFBASE))==0,0.7))break;  // if key match, we're done: we have saved the hashslot in s[] with high 32 bits 0
   // no match.  try next hashslot
   if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
  }
 }

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))unbiasforcomp
 // pass slot# back to front, putting them into 3 chains: new kvs, old keys, conflict keys.  Conflict keys are assumed vary rare (usually repeated keys)
 I nroot=-1, oroot=-1, croot=-1, otail=0;   // base of the 3 chains, inited to empty; tail of old chain, inited to 'not found yet' (safe even if result turns out to be 0)
 DQ(n, I sh=((UI4*)(&s[i]))[1]; if(unlikely(sh&((1LL<<HASHTSTN)+(1LL<<(HASHTSTN+HASHBSTN))))){((UI4*)(&s[i]))[1]=croot; croot=i;}else{((I4*)(&s[i]))[1]=sh?nroot:oroot; nroot=sh?i:nroot; oroot=sh?oroot:i; otail=sh|otail?otail:i;})

 // chase the new kvs, taking an empty slot for each and copying the k and v into the tables. s[] holds chain\hashslot
 I vb=dic->bloc.vbytelen; C *empty=CAV2(dic->bloc.empty); UI emptyx=dic->bloc.emptyn; I cur;  // empty area & current pointer into it.  Length of a element is hsz>>56
 for(cur=nroot;cur>=0;){
  I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  UI emptynxt=_bzhi_u64(*(UI4*)&empty[emptyx*(hsz>>56)],(hsz>>53)); if(emptynxt==emptyx)goto resize;  // get next empty, abort if end of chain (indicated by loopback)
  PUTKVNEW(CAV(dic->bloc.keys)+emptyx*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32,hsz&(DICFKINDIR<<DICFBASE))
; PUTKVNEW(CAV(dic->bloc.vals)+emptyx*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE));
  cur=nxt; emptyx=emptynxt;  // advance to next
 }

 DICLKWRRQ(dic,lv); DICLKWRWT(dic,lv)  // request pre-write and wait for it to be granted.  No resize is possible.  scaf could put rq up a little earlier

 // chase the new kvs again, replacing the birthstone (indexed by s[i]) with the allocated kvslot#, which we get by retraversing the empties list.  Also mark the new kvs not-empty  s[] holds chain\hashslot
     // traversing a linked list sucks, but two at a time are no worse than 1, and the variable-length writes take a little time
 for(cur=nroot,emptyx=dic->bloc.emptyn;cur>=0;){
  I8 nxthsh=s[cur];  // fetch chain\hashslot
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  UI emptynxt=_bzhi_u64(*(UI4*)&empty[emptyx*(hsz>>56)],(hsz>>53));   // get next empty, abort if end of chain (loopback)
  WRCONST1234(~(I)0,hsz>>56,&empty[emptyx*(hsz>>56)]) emptyx+=HASHNRES; WRHASH1234(emptyx, hsz>>56, &hashtbl[(UI4)nxthsh*(hsz>>56)])  // set empty slot to ~0, set hashtable to point to new kv (skipping over reserved hashslot#s)
  cur=nxthsh>>32; emptyx=emptynxt;  // advance to next
 }
 dic->bloc.emptyn=emptyx;  // We have allocated all the new blocks successfully.  Update the free root

 // chase the conflict keys (in ascending order), updating everything.  They should be few.  For new kvs, also take an empty slot & update the keys.  Leave values in the conflict chain, in ascending order. s[i] indexes the hashslot
 // Since the original search searched all the way to a hold looking for a match, we know that the only match must come from a previous key in this put.  This key would necessarily have gone into the first tombstone found,
 // so we can stop the search when we hit a tombstone or a hole.
 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp
 for(cur=croot;cur>=0;){
  I8 nxthsh=s[cur];  // fetch chain\hashslot
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  I curslot=(UI4)nxthsh;  UI hval; // slot was originally birthstone where the search ended.  Now it has been filled in, and we resume the search there.  It could match right there.  hval is biased kv slot# from hashtable
  while(1){
   hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
   if(hval<HASHNRES)goto notfound;  // if we hit a hole (birthstones have been filled), go allocate a new kv
   if(keysne((UI4)kib,(void*)((I)k+i*(kib>>32)),kbase+(kib>>32)*hval,hsz&(DICFICF<<DICFBASE))==0)goto found;   // if key matches, hval is the slot
   // no match
   if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
  }
notfound:;
   // search ends at empty/tombstone, in hashslot (curslot).  Allocate a new empty, point curslot to it, move in the key, save the slot in s[cur] which is the first empty/tombstone found
   emptyx=dic->bloc.emptyn; UI emptynxt=_bzhi_u64(*(UI4*)&empty[emptyx*(hsz>>56)],(hsz>>53)); if(emptynxt==emptyx)goto resize; hval=emptyx+HASHNRES; dic->bloc.emptyn=emptynxt; // get empty slot, save as biased kv slot#
   WRCONST1234(~(I)0,hsz>>56,&empty[emptyx*(hsz>>56)]) emptyx=hval; WRHASH1234(emptyx, hsz>>56, &hashtbl[(UI4)nxthsh*(hsz>>56)])   // set empty slot to ~0, set hashtable to point to new kv (skipping over reserved hashslot#s)
   PUTKVNEW(kbase+hval*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32,hsz&(DICFKINDIR<<DICFBASE));    // copy the new key
found:;  // hval is the kv slot we compared with, or a new empty kv slot
  ((UI4*)(&s[cur]))[0]=hval;  // remember the biased slot the value must be moved into.  This changes s[i] from hashslot index to kv index
  cur=nxthsh>>32;  // advance to next
 }

 DICLKWRRELK(dic,lv)    // allow gets to look at hashtable & keys.  We still have a write lock

 // we have updated the keys and hash.  Now move the values, indexed by s[i] (biased).  Every value gets moved once.  We move the old then the conflicts, knowing that any old must precede any conflict that maps to the same slot
 C *vbase=CAV(dic->bloc.vals)-HASHNRES*vb;   // base pointer to allocated values, backed up to skip over the empty/birthstone/tombstone codes
 ((I4*)(&s[otail]))[1]=croot; oroot=oroot<0?croot:oroot;  // append conflict chain to old chain (if old chain is empty otail=0, which is OK to store into because ele 0 can never be a conflict); start on combined chain
 for(cur=oroot;cur>=0;){I8 nxtkv=s[cur]; PUTKVOLD(vbase+(UI4)nxtkv*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE)); cur=nxtkv>>32;} // chase the old keys, copying the value
// obsolete  for(cur=croot;cur>=0;){I nxt=((I4*)(&s[cur]))[1]; UI kvslot=((UI4*)(&s[cur]))[0]; PUTKVOLD(vbase+kvslot*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE)); cur=nxt;} // chase the conflict keys, copying the value
 R 1;  // return, holding write lock on values

resize: R -1;  // signal need for resize; return holding write lock on keys & values
}

//  put.  a is values, w is keys   dic was u to self
static DF2(jtdicput){F12IP;
 ARGCHK2(a,w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.ktype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 I af=AR(a)-vr; ASSERT(af>=0,EVRANK) ASSERTAGREE(AS(a)+af,vs,vr)   // v must be a single value or an array of them, with correct shape
 I cf=MIN(af,wf); ASSERTAGREE(AS(a)+af-cf,AS(w)+wf-cf,cf)  // frames must be suffixes
 if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0)) if(unlikely((AT(a)&vt)==0))RZ(a=ccvt(vt,w,0))  // convert type of k and v if needed
 I kn; PROD(kn,wf,AS(w)) I vn; PROD(vn,wf,AS(w))   // kn = number of keys to be looked up  vn=#values to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index
 // We do not have to make incoming kvs recursive, because the keys/vals tables do not take ownership of the kvs.  The input kvs must have their own protection, valid over the call.
 // For the same reason, we do not have to worry about the order inwhich kvs are added and deleted.

 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

 I lv; DICLKRWRQ(dic,lv);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
 void *k=voidAV(w), *v=voidAV(a);  // point to the key and value data
 while(1){  // loop till we have processed all the resizes
  if(unlikely(!(dic->bloc.flags&DICFIHF)))s=0;   // user hash function, keyprep will allocate s
  else if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
  else{A x; GATV0E(x,FL,kn,1,goto abortexit;) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
    // we have to reinit s in the resize loop because putslots may have modified it
 
  if(unlikely((s=jtkeyprep(dic,k,kn,s,jt,w))==0))goto errexit;  // hash keys & prefetch.  This may return as s a block that was allocated inside keyprep.  It must persist till the end
  I rc=jtputslots(dic,k,kn,v,vn,s,jt,lv,virt); if(rc>0)break; if(rc==0)goto errexit;  // do the puts; if no resize, finish, good or bad
  if(dicresize(dic,jt)==0)goto errexit;  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
 }
 A z=mtv; if(0){errexit: z=0;}
 PRISTCLRF(w);    // we have taken from w; remove pristinity.  This destroys w.  We do this even in case of error because we may have moved some values before the error happened
abortexit:;
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
 RETF(z);
}

// ********************************** del **********************************

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; rc=0 if error, rc=-1 to request a resize
static scafINLINE I jtdelslots(DIC *dic,void *k,I n,I8 *s,J jt,I lv,VIRT virt){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;    // more nontresizable
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
    // with this lock we can add new kvs, or change an empty/tombstone to a birthstone; but no other hash changes, and no value overwrites

 UI8 hsz=dic->bloc.hashsiz; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen  compare func  base of hashtbl
 C *kbase=CAV(dic->bloc.keys)-HASHNRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array

 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))biasforcomp

 // first pass over keys.  If key found, remember the biased kv# on the old chain
 I oroot=-1;  // init old chain empty
 for(i=n-1;i>=0;--i){
  I8 curslot=(((UI8)s[i]*(UI4)hsz)>>32); UI hval; // convert hash to slot#; place where biased kv# comes in
  while(1){
   hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
   if(withprob(hval<HASHNRES,0.3)){  // hole or tombstone
    if(withprob(hval<HASHTSTN,0.8))break;   // hole, exit not found
   }else if(withprob(keysne((UI4)kib,(void*)((I)k+i*(kib>>32)),kbase+(kib>>32)*hval,hsz&(DICFICF<<DICFBASE))==0,0.7)){s[i]=((I8)oroot<<32)+curslot; oroot=i; break;}  // match, exit found, putting departing hashslot on old chain
   // no match.
   if(unlikely(--curslot<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
  }
 }

 DICLKWRRQ(dic,lv);   // request write lock
 I vb=dic->bloc.vbytelen; C *vbase=CAV(dic->bloc.vals)-HASHNRES*vb;  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 UI emptyx=dic->bloc.emptyn; I cur; C *empty=CAV2(dic->bloc.empty); // starting root of free queue, current index, empty list
 if(unlikely(!(hsz&(DICFICF<<DICFBASE))))unbiasforcomp
 DICLKWRWT(dic,lv)  // wait for write lock to be granted.  No resize is possible

 // chase the old kvs, which give the hashslots to be deleted.  We turn each into a tombstone, put the kv slot on the empty chain, and flip trailing tombstones.  If it is a tombstone already, it's a double del, ignore it
 for(cur=oroot;cur>=0;){
  I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  I8 curslot=((UI4*)(&s[cur]))[0];  // extract hashslot# to be emptied
  UI hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // get biased kv slot to be released
  if(likely(hval>=HASHNRES)){  // don't clear a slot more than once
   I t=HASHTSTN; WRHASH1234(t, hsz>>56, &hashtbl[curslot*(hsz>>56)])  // mark the slot as a tombstone
   hval-=HASHNRES;  // remove bias from hval, giving actual slot index
   t=emptyx; WRHASH1234(t, hsz>>56, &empty[hval*(hsz>>56)])  // chain old free chain from new deletion
   emptyx=hval;  // put new deletion at top of the free chain
   DELKV(kbase+hval*(kib>>32),kib>>32,hsz&(DICFKINDIR<<DICFBASE)) DELKV(vbase+hval*vb,vb,hsz&(DICFVINDIR<<DICFBASE))   // if k/v is indirect, free it & clear to 0
   // Whenever we add a tombstone, we turn it into an empty if it is followed by an empty; and we continue this back into previous tombstones
   I8 curslotn=curslot-1; if(unlikely(curslotn<0))curslotn+=(UI4)hsz;   // point to next slot
   if(_bzhi_u64(*(UI4*)&hashtbl[curslotn*(hsz>>56)],(hsz>>53))==0){
    // curslot is followed by a hole.  Flip it & its predecessors to holes
    do{hashtbl[curslot*(hsz>>56)]=0; if(unlikely(++curslot==(UI4)hsz))curslot=0;}while(_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53))==HASHTSTN);  // only the low byte needs to change
   }
  }
  cur=nxt;  // advance to next
 }
 dic->bloc.emptyn=emptyx;  // Store updated free chain after all deletions

 DICLKWRRELK(dic,lv)    // allow gets to look at hashtable & keys

 R 1;  // return, holding write lock on values

// resize: R -1;  // signal need for resize; return holding write lock on keys & values
}


//  del.   w is keys   u was dic to self
static DF1(jtdicdel){F12IP;
 ARGCHK1(w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.ktype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))  // convert type of k  if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index

 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

 I lv; DICLKRWRQ(dic,lv);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
 void *k=voidAV(w);  // point to the key and value data
 while(1){  // loop till we have processed all the resizes
  if(unlikely(!(dic->bloc.flags&DICFIHF)))s=0;   // user hash function, keyprep will allocate s
  else if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
  else{A x; GATV0E(x,FL,kn,1,goto errexit;) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
    // we must reinit s because it might have been change by keyprep

  if(unlikely((s=jtkeyprep(dic,k,kn,s,jt,w))==0))goto errexit;  // hash keys & prefetch

  I rc=jtdelslots(dic,k,kn,s,jt,lv,virt); if(rc>0)break; if(rc==0)goto errexit;  // do the puts; if no resize, finish, good or bad
  dicresize(dic,jt);  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
 }
 A z=mtv; if(0){errexit: z=0;}
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
 RETF(z);
}

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- ordered map (red-black tree) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

// The tree is stored as child0/child1/parent, each a variable number of bytes.  The MSB of child0 is the color, 0=black
// There is 1 tree node per key/value allocated
// empty tree nodes are on the empty chain, with loopback at end of chain


// ********************************** get **********************************

// k is A for keys, s is slot#s, zv is result area (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy the value (or default) to the result
// a is default if given, 0 if not 
// We take a read lock on the table and return with it
static scafINLINE B jtgetslotso(DIC *dic,void *k,I n,I8 *s,void *zv,J jt,A a, VIRT virt){I i;
 I lv; DICLKRDRQ(dic,lv);   // request read lock
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn; I vn=dic->bloc.vbytelen;   // more nonresizable: kbytelen/kitemlen   compare fn  prototype required to get arg converted to I   size of a value in bytes 
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 I nodeb=3*dic->bloc.hashelesiz; I nodexb=nodeb*SZI+(dic->bloc.flags<<8);  // number of bytes in a tree node; number of bits in a node index, with flag bits for DICFICF
 DICLKRDWTK(dic,lv)   // wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 C *hashtbl=CAV1(dic->bloc.hash);  // pointer to tree base
 I rootx=_bzhi_u64(*(UI4*)hashtbl,nodexb);  // biased node# of the root of the tree
 C *kbase=CAV(dic->bloc.keys)-TREENRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 C *vbase=CAV(dic->bloc.vals)-TREENRES*vn;  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the value array

 if(unlikely(!(nodexb&(DICFICF<<8))))biasforcomp

 // Look up each key.  Prefetch each value to get a leg up on moving the data.  We don't prefetch more to avoid needless bus bandwidth; perhaps we should check the length   scaf
    // the only advantage of the prefetch is that the value reads will clear earlier, allowing the fence when we end our lock to finish earlier
 for(i=n;--i>=0;){I nodex;
  k=(void*)((I)k-(kib>>32));  // back up to next key
  for(nodex=rootx;;){  // traverse the tree, searching for index k.  Current node is nodex
   if(unlikely(nodex<TREENRES))break;  // not found
   I childs=*(I8*)&hashtbl[nodex*nodeb];  // fetch both children
   I comp=keysne((UI4)kib,kbase+(kib>>32)*nodex,k,nodexb&(DICFICF<<8));  // compare node key vs k, so k > node is ~0
   if(comp==0){PREFETCH(vbase+nodex*vn); break;}  // found at node nodex.  Start a prefetch of  the value
   nodex=_bzhi_u64(childs>>(nodexb&comp),nodexb);  // choose left/right based on comparison, mask out garb.
  }
  s[i]=nodex;  // remember where the search ended
 }

 // copy using the kv indexes we calculated.  Copy in ascending order so we can overstore    scaf overstore
 void *av=0;  // init to 'no default data pointer yet'.  We avoid checking the default until we know we need it
 I cur=s[0];  // unroll loop once.  cur if current value to work on
 DICLKRDWTV(dic,lv)  // wait till values are safe.  No resize is possible here
 for(i=0;i<n;++i){void *vv;  // pointer to value to move
  I nxt=s[i+1];  // fetch next value.  Overfetch OK
  if(likely(cur>=TREENRES))vv=vbase+cur*vn;   // this is the main line, key found
  else{   // default required, which we deem rare.
   if(unlikely(av==0)){
    // First time through here. We have to audit it & convert if needed
    ASSERT(a!=0,EVINDEX)   // error if no default given
    ASSERT(AR(a)==AN(dic->bloc.vshape),EVRANK) ASSERTAGREE(AS(a),IAV1(dic->bloc.vshape),AR(a))  // verify default has correct rank & shape
    if(unlikely((AT(a)&dic->bloc.vtype)==0)){RZ(a=ccvt(dic->bloc.vtype,a,0))}   // convert precision if needed
    av=CAV(a);  // get address of values to move
    PRISTCLRF(a);  // Since we are taking out of a, it is no longer pristine.  this destroys a, but leaves it non0.
   }
   vv=av;    // use the default after it has been audited
  }
  GETV(zv,vv,vn,nodexb&(DICFICF<<8)); zv=(void *)((I)zv+vn);   // move the data & advance popinter to next one   scaf JMC?
  cur=nxt;
 }
 R 1;
}

//   get.   Bivalent. w is keys, [a] is default value, 0 if monad.  Called from parse/unquote as a,w,self or w,self,self.  dic was u to self
static DF2(jtdicgeto){F12IP;A z;
 ARGCHK2(a,w)
 A adyad=w!=self?a:0; w=w!=self?w:a;  // if dyad, keep a,w, otherwise 0,w
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){GA0(z,dic->bloc.vtype,0,wf+AN(dic->bloc.vshape)) MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,IAV1(dic->bloc.vshape),AN(dic->bloc.vshape)) R z;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))   // convert type of w if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index

 I t=dic->bloc.vtype; A sa=dic->bloc.vshape;
 GA0(z,t,kn*dic->bloc.vaii,wf+AN(sa)) AFLAG(z)=t&RECURSIBLE; MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,IAV1(sa),AN(sa))   // allocate recursive result area & install shape
 // establish the area to use for s.  To avoid wasting a lot of stack space we use the virt-block area if that is not needed for user comp.  And if there is a user hash, we allocate
 // nothing & use the value returned by keyprep, which will be the result area from the user hash.
 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer
 if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
 else{A x; GATV0(x,FL,kn,1) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
 
 void *k=voidAV(w);  // point to the key data
 if(!jtgetslotso(dic,k,kn,s,voidAV(z),jt,adyad,virt))z=0;  // get the values and take a read lock on the dic.  If error, pass error through
 DICLKRDREL(dic)  // release read lock
 RETF(z);
}


// ********************************** put **********************************

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic; rc=0 if error, rc=-1 to request a resize
static scafINLINE I jtputslotso(DIC *dic,void *k,I n,void *v,I vn,I8 *s,J jt,I lv,VIRT virt){I i;
 if(unlikely(!(dic->bloc.flags&DICFICF))){initvirt((A)virt.u,dic); initvirt((A)virt.h,dic); virt.self=dic->bloc.hashcompself; }   // fill in nonresizable info
 UI8 kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn;   // more nonresizable: kbytelen/kitemlen   compare fn  prototype required to get arg converted to I
 I nodeb=3*dic->bloc.hashelesiz; I nodexb=nodeb*(0x10000+SZI)+(dic->bloc.flags<<8);  // number of bytes in a tree node; (#bytes in node index)\(flags)\(number of bits in a node index)
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock

 C *hashtbl=CAV1(dic->bloc.hash)-TREENRES*nodeb;  // pointer to tree base
 I rootx=_bzhi_u64(*(UI4*)hashtbl,nodexb);  // biased node# of the root of the tree
 C *kbase=CAV(dic->bloc.keys)-TREENRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array

 if(unlikely(!(nodexb&(DICFICF<<8))))biasforcomp

 // first pass over keys.  If key found, remember the biased kv# (will go to old chain).  If not found, remember the index of the NUL and make it a birthstone - will go to new or conflict chain
 // If there is match we will find it before we take the write lock.
 // We have to go in ascending order because later keys must overwrite earlier ones
 void *ki=k;  // pointer to key being compared
 for(i=0;i<n;++i){I nodex;
  I comp;  // will be compare result
  // the root is pointed to by hash0/dir0.  In an empty database hash0/dir0=0.  In that case we fiddle things so that the first key is called new and the others are called conflict.  They all point to
  // hash0/dir0 as the end-of-search point.  The new will fill hash0/dir0 with a node, and the rest will start their search at that node, which is the true root of the tree.
  if(unlikely(rootx<TREENRES)){s[i]=1; rootx=i!=0; comp=1;}  // empty database: s[i]=1 to avoid modifying tree0, set conflict except on first key, direction always 0
  for(nodex=rootx;;){  // traverse the tree, searching for index k.  Current node is nodex
   if(unlikely(nodex<TREENRES)){hashtbl[s[i]*nodeb]=1; ((I4*)(&s[i]))[1]=2*comp+(nodex&1); break;}  // not found.  Top half is comp\conflict
   I childs=*(I8*)&hashtbl[nodex*nodeb];  // fetch both children
   s[i]=nodex;  // current node may become found loc or end of search, init as if found
   comp=keysne((UI4)kib,kbase+(kib>>32)*nodex,ki,nodexb&(DICFICF<<8));  // compare node key vs k, so k > node is ~0
   if(comp==0){break;}  // found at node nodex.
   nodex=_bzhi_u64(childs>>(nodexb&comp),nodexb);  // choose left/right based on comparison, mask out garb.
  }
  ki=(void *)((I)ki+(kib>>32));  // advance to next search key
 }

 if(unlikely(!(nodexb&(DICFICF<<8))))unbiasforcomp
 // pass slot# back to front, putting them into 3 chains: new kvs, old keys, conflict keys.  Conflict keys are assumed vary rare (multiple keys in a small range)
 // The chain element has (31 bits of chain)\(1 bit indicating which child was null (used only for new and conflict chains))\(32 bits of node index)
 I nroot=-1, oroot=-1, croot=-1, otail=0;   // base of the 3 chains, inited to empty; tail of old chain, inited to 'not found yet' (safe even if result turns out to be 0)
 DQ(n, UI4 sh=((UI4*)(&s[i]))[1]; UI dir=sh>>31; if(unlikely(sh&1)){((UI4*)(&s[i]))[1]=2*croot+dir; croot=i;}else{((I4*)(&s[i]))[1]=2*(sh?nroot:oroot)+dir; nroot=sh?i:nroot; oroot=sh?oroot:i; otail=sh|otail?otail:i;})

 // chase the new kvs, initing an empty slot for each and copying the k and v into the tables. s[] holds chain\hashslot
 I vb=dic->bloc.vbytelen; UI emptyx=dic->bloc.emptyn; I cur;  // current headptr of free node chain.  Length of an element is nodeb
 for(cur=nroot;cur>=0;){
  I8 nxthsh=s[cur];  // fetch chain\dir\treeslot of parent
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  UI emptynxt=_bzhi_u64(*(UI4*)&hashtbl[emptyx*nodeb],nodexb); if(emptynxt==emptyx)goto resize;  // get next empty, abort if end of chain (indicated by loopback)
  PUTKVNEW(CAV(dic->bloc.keys)+emptyx*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32,nodexb&(DICFICF<<8))
; PUTKVNEW(CAV(dic->bloc.vals)+emptyx*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,nodexb&(DICFICF<<8));
  UI4 parent=2*nxthsh; WRHASH1234(parent, nodexb>>16, &hashtbl[emptyx*nodeb+(nodexb>>15)])  // new node is red, with parent where the search stopped
  parent=0; WRHASH1234(parent, nodexb>>16, &hashtbl[emptyx*nodeb]) WRHASH1234(parent, nodexb>>16, &hashtbl[emptyx*nodeb+(nodexb>>16)])  // set both chilren empty
  cur=nxthsh>>33; emptyx=emptynxt;  // advance to next
 }

 DICLKWRRQ(dic,lv); DICLKWRWT(dic,lv)  // request pre-write and wait for it to be granted.  No resize is possible.  scaf could put rq up a little earlier

 // chase the new kvs again, installing the allocated kvslot#, which we get by retraversing the empties list, into the correct child field of the ending NULL, thus adding it to the tree.  s[] holds chain\dir\treeslot
     // traversing a linked list sucks, but two at a time are no worse than 1, and the variable-length writes take a little time
 I ntail;  // tail of new chain
 if(nroot>=0){
  for(cur=nroot,emptyx=dic->bloc.emptyn;;){
   I8 nxthsh=s[cur];  // fetch chain\dir\treeslot
// obsolete   I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
   UI emptynxt=_bzhi_u64(*(UI4*)&hashtbl[emptyx*nodeb],nodexb);   // get next empty, abort if end of chain (loopback)
   WRHASH1234(emptyx, nodexb>>16, &hashtbl[(UI4)nxthsh*nodeb+(-((nxthsh>>32)&1)&(nodexb>>16))])  // install newly-allocated slot into the parent
   if(nxthsh<0){ntail=cur; break;}   // when we hit tail, remember it & exit loop
   cur=nxthsh>>33; emptyx=emptynxt;  // advance to next
  }
 }
 dic->bloc.emptyn=emptyx;  // We have allocated all the new blocks successfully.  Update the free root

 // chase the conflict keys (in ascending order), updating everything.  They should be few.  For ones found to be new kvs, also take an empty slot & update the keys.  Leave values in the conflict chain, in ascending order.  s[] holds chain\dir\treeslot
 // Since the original search searched all the way looking for a match, we know that the only match must come from a previous key in this put.
 if(unlikely(!(nodexb&(DICFICF<<8))))biasforcomp
 for(cur=croot;cur>=0;){I nodex;
  I comp;  // will be compare result
  void *ki=(void*)((I)k+i*(kib>>32));  // key being sought
  // the database cannot be empty here.  If it started empty, the first put was made into a new node pointed to by hash0/dir0.  Initial cur puts into an empty database will start these searches there.
  I8 nxthsh=s[cur];  // fetch chain\dir\hashslot where search ended.  It must perforce have been filled by a pointer to an empty
  I childs;  // the two children
  for(childs=*(I8*)&hashtbl[(UI4)nxthsh*nodeb], comp=2*((nxthsh>>32)&1)-1;;){  // traverse the tree, searching for index k.  Current node is nodex
   nodex=_bzhi_u64(childs>>(nodexb&comp),nodexb);  // choose left/right based on comparison, mask out garb.
   if(nodex<TREENRES){goto notfound;}  // not found.  Will be new or conflict
   I childs=*(I8*)&hashtbl[nodex*nodeb];  // fetch both children
   ((I4*)(&s[cur]))[0]=nodex;  // current node may become found loc or end of search, init as if found
   comp=keysne((UI4)kib,kbase+(kib>>32)*nodex,ki,nodexb&(DICFICF<<8));  // compare node key vs k, so k > node is ~0
   if(comp==0){goto found;}  // found at node nodex.
  }
notfound:;
   // search ends at NULL, in s[cur] at direction comp.  Allocate a new empty, overwrite the NULL with it, move in the key, save the slot in s[cur] which is the first empty/tombstone found
   emptyx=dic->bloc.emptyn; UI emptynxt=_bzhi_u64(*(UI4*)&hashtbl[emptyx*nodeb],nodexb); if(emptynxt==emptyx)goto resize; dic->bloc.emptyn=emptynxt; // get empty slot, save as biased kv slot#
   UI4 parent=2*(UI4)s[cur]; WRHASH1234(parent, nodexb>>16, &hashtbl[emptyx*nodeb+(nodexb>>15)])  // new node is red, with parent where the search stopped
   parent=0; WRHASH1234(parent, nodexb>>16, &hashtbl[emptyx*nodeb]) WRHASH1234(parent, nodexb>>16, &hashtbl[emptyx*nodeb+(nodexb>>16)])  // set both children empty
   PUTKVNEW(kbase+emptyx*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32,nodexb&(DICFICF<<8));    // copy the new key.  Value can wait
   ((UI4*)(&s[cur]))[0]=emptyx;  // remember the biased slot the value must be moved into.  This changes s[i] from hashslot index to kv index
found:;  // s[cur] has the kv slot: the found key, or the added empty kv slot
  cur=nxthsh>>33;  // advance to next
 }

 // rebalance the tree, starting at each addition made by new or conflict.  We unnecessarily look at conflict-old keys that are still in the conflict chain, but they should be rare (only rewrites within a single batch) & not worth discarding
 if(croot>=0)((I4*)(&s[ntail]))[1]=croot;  // append conflict chain to new chain.  If conflict chain is nonempty so must new be, so ntail will be valid whenever we store through it
 for(cur=nroot;cur>=0;){   // on new chain followed by conflict chain...
  I8 nxtkv=s[cur];
  // cur is an added node, initially red.  We look for red/black violations, going up the tree.  hash0 (parent of the root) is always red.
  UI pclr=_bzhi_u64(*(UI4*)&hashtbl[cur*nodeb+(nodexb>>15)],nodexb);   // fetch parent\clr
  while((pclr&1)==0){   // while we are looking at a red node... (when we get to a black node we are in sync)
   UI sib=_bzhi_u64(*(UI4*)&hashtbl[(pclr>>1)*nodeb],nodexb);   // parent's left child (our putative sibling)
   C sibc=hashtbl[sib*nodeb+(nodexb>>15)];   //   color of our sibling
   if(sibc&1){UI t;  // parent's left child is black and we are red.  We must be the right child.  We cannot be the root
    // left-leaning violation.  Exchange colors of cur (always red) & par.  Change sib <- par -> cur -> (c1,c2)  to  (sib,c1) <- par <- cur -> c2
    UI parpclr=_bzhi_u64(*(UI4*)&hashtbl[(pclr>>1)*nodeb+(nodexb>>15)],nodexb);  // parent's parent\clr
    t=2*(pclr>>1)+(parpclr&1); WRHASH1234(t, nodexb>>16, &hashtbl[cur*nodeb+(nodexb>>15)])  // cur's new parent is parent's parent, color is parent's clr
    t=2*cur+0; WRHASH1234(t, nodexb>>16, &hashtbl[(pclr>>1)*nodeb+(nodexb>>15)])  // par's new parent is cur, color is cur's clr
    UI c1=_bzhi_u64(*(UI4*)&hashtbl[cur*nodeb],nodexb); t=2*(pclr>>1)+(hashtbl[c1*nodeb+(nodexb>>15)]&1); WRHASH1234(t, nodexb>>16, &hashtbl[c1*nodeb+(nodexb>>15)])  // c1's new parent is par, keeping c1's color
    t=c1; WRHASH1234(t, nodexb>>16, &hashtbl[(pclr>>1)*nodeb+(nodexb>>16)])  // c1 becomes par's right child
    t=pclr>>1; WRHASH1234(t, nodexb>>16, &hashtbl[cur*nodeb])   // par becomes cur's left child
    I parparleft=_bzhi_u64(*(UI4*)&hashtbl[(parpclr>>1)*nodeb],nodexb); t=cur; WRHASH1234(t, nodexb>>16, &hashtbl[(parpclr>>1)*nodeb+(parparleft!=cur)*(nodexb>>16)]) // parent's parent's left child; cur becomes correct child of parent's parent
    cur=pclr>>1; pclr=parpclr;  // step back toward the root
   }
   // cur and pclr are set
   UI parpclr=_bzhi_u64(*(UI4*)&hashtbl[(pclr>>1)*nodeb+(nodexb>>15)],nodexb);   // parent's parent\clr
   if(parpclr&1)break;  // if parent is not red, there can't be a red-red vio, done
   // parent is red (red-red vio).  Can't be at root.   parpclr is  grandparent of cur.  Must be black since par-gpar can't be red-red
   UI parsib=_bzhi_u64(*(UI4*)&hashtbl[(parpclr>>1)*nodeb+(nodexb>>16)],nodexb);  // grandparent's right child (putative sibling of parent)
   C parsibc=hashtbl[parsib*nodeb+(nodexb>>15)];   //   color of parent's sibling
   if(parsibc&1){UI t;
    // par is red and its right sibling is black.  It must be the left child.  Exchange colors of gpar (always black) & par, change (c1,c2) <- par <- gpar -> parsib  to  c1 <- par -> gpar -> (c2,parsib)..  This clears the vio, because par becomes black
    UI gparpclr=_bzhi_u64(*(UI4*)&hashtbl[(parpclr>>1)*nodeb+(nodexb>>15)],nodexb);  // gparent's parent\clr
    t=2*(gparpclr>>1)+1; WRHASH1234(t, nodexb>>16, &hashtbl[(pclr>>1)*nodeb+(nodexb>>15)])  // par's new parent is gparent's parent, color is gparent's clr (black)
    t=2*(pclr>>1)+0; WRHASH1234(t, nodexb>>16, &hashtbl[(parpclr>>1)*nodeb+(nodexb>>15)])  // gpar's new parent is par, color is par's clr (red)
    UI c2=_bzhi_u64(*(UI4*)&hashtbl[cur*nodeb+(nodexb>>16)],nodexb); t=2*(parpclr>>1)+(hashtbl[c2*nodeb+(nodexb>>15)]&1); WRHASH1234(t, nodexb>>16, &hashtbl[c2*nodeb+(nodexb>>15)])  // c2's new parent is gpar, keeping c2's color
    t=c2; WRHASH1234(t, nodexb>>16, &hashtbl[(parpclr>>1)*nodeb])  // c2 becomes gpar's left child
    t=parpclr>>1; WRHASH1234(t,nodexb>>16,&hashtbl[(pclr>>1)*nodeb+(nodexb>>16)])   // gpar becomes par's right child
    I parparleft=_bzhi_u64(*(UI4*)&hashtbl[(gparpclr>>1)*nodeb],nodexb); t=pclr>>1; WRHASH1234(t, nodexb>>16, &hashtbl[(parpclr>>1)*nodeb+(parparleft!=cur)*(nodexb>>16)]) // gpar's parent's left child; par becomes correct child of gpar's parent
    break;  // vio is cleared
   }else{
    // both children of gpar are red.  Make them black and gpar red, and move up to gpar to continue looking for vio
    hashtbl[(parpclr>>1)*nodeb+(nodexb>>15)]&=~1;   // make grandparent red
    hashtbl[_bzhi_u64(*(UI4*)&hashtbl[(parpclr>>1)*nodeb],nodexb)*nodeb+(nodexb>>15)]|=1;   // make grandparent's left child black
    hashtbl[_bzhi_u64(*(UI4*)&hashtbl[(parpclr>>1)*nodeb+(nodexb>>16)],nodexb)*nodeb+(nodexb>>15)]|=1;   // make grandparent's right child black
    cur=parpclr>>1;  // advance to new code
   }

  }

  cur=nxtkv>>33;
 }

 DICLKWRRELK(dic,lv)    // allow gets to look at hashtable & keys.  We still have a write lock

 // we have updated the keys and hash.  Now move the values, indexed by s[i] (biased).  Every value gets moved once.  We move the old then the conflicts, knowing that any old must precede any conflict that maps to the same slot
 C *vbase=CAV(dic->bloc.vals)-TREENRES*vb;   // base pointer to allocated values, backed up to skip over the empty/birthstone/tombstone codes
 ((I4*)(&s[otail]))[1]=croot; oroot=oroot<0?croot:oroot;  // append conflict chain to old chain (if old chain is empty otail=0, which is OK to store into because ele 0 can never be a conflict); start on combined chain
 for(cur=oroot;cur>=0;){I8 nxtkv=s[cur]; PUTKVOLD(vbase+(UI4)nxtkv*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,nodexb&(DICFICF<<8)); cur=nxtkv>>33;} // chase the old keys, copying the value
// obsolete  for(cur=croot;cur>=0;){I nxt=((I4*)(&s[cur]))[1]; UI kvslot=((UI4*)(&s[cur]))[0]; PUTKVOLD(vbase+kvslot*vb,(void*)((I)v+(likely(cur<vn)?cur:cur%vn)*vb),vb,hsz&(DICFVINDIR<<DICFBASE)); cur=nxt;} // chase the conflict keys, copying the value
 R 1;  // return, holding write lock on values

resize: R -1;  // signal need for resize; return holding write lock on keys & values
}

//  put.  a is values, w is keys   dic was u to self
static DF2(jtdicputo){F12IP;
 ARGCHK2(a,w)
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I vt=dic->bloc.ktype; I vr=AN(dic->bloc.vshape), *vs=IAV1(dic->bloc.vshape);   // value info
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 I af=AR(a)-vr; ASSERT(af>=0,EVRANK) ASSERTAGREE(AS(a)+af,vs,vr)   // v must be a single value or an array of them, with correct shape
 I cf=MIN(af,wf); ASSERTAGREE(AS(a)+af-cf,AS(w)+wf-cf,cf)  // frames must be suffixes
 if(unlikely(AN(w)==0)){R mtv;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0)) if(unlikely((AT(a)&vt)==0))RZ(a=ccvt(vt,w,0))  // convert type of k and v if needed
 I kn; PROD(kn,wf,AS(w)) I vn; PROD(vn,wf,AS(w))   // kn = number of keys to be looked up  vn=#values to be looked up
 ASSERT((UI)kn<=(UI)2147483647,EVLIMIT)   // no more than 2^31-1 kvs: we use a signed 32-bit index
 // We do not have to make incoming kvs recursive, because the keys/vals tables do not take ownership of the kvs.  The input kvs must have their own protection, valid over the call.
 // For the same reason, we do not have to worry about the order inwhich kvs are added and deleted.

 VIRT virt; I8 *s; virt.self=dic->bloc.hashcompself;  // place for virtuals (needed by user comp fns); key/hash workarea; fill in self pointer

 I lv; DICLKRWRQ(dic,lv);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
 void *k=voidAV(w), *v=voidAV(a);  // point to the key and value data
 while(1){  // loop till we have processed all the resizes
  if((kn<=(I)(offsetof(VIRT,self)%8))>(~dic->bloc.flags&DICFICF))s=(I8*)&virt;  // if the workarea will fit into virt, and we don't need virt for compare fns, use it.  virt.self is available for overfetch
  else{A x; GATV0E(x,FL,kn,1,goto abortexit;) s=(I8*)voidAV1(x);}   // allocate a region.  FL is 8 bytes
    // we have to reinit s in the resize loop because putslots may have modified it
 
  I rc=jtputslotso(dic,k,kn,v,vn,s,jt,lv,virt); if(rc>0)break; if(rc==0)goto errexit;  // do the puts; if no resize, finish, good or bad
  if(dicresize(dic,jt)==0)goto errexit;  // If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
 }
 A z=mtv; if(0){errexit: z=0;}
 PRISTCLRF(w);    // we have taken from w; remove pristinity.  This destroys w.  We do this even in case of error because we may have moved some values before the error happened
abortexit:;
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
 RETF(z);
}





// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- user-level primitives *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-


// u 16!:_4  del: u=dic
// We create a verb to handle (del y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicdelc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(0,CMODX,VERB, jtdicdel,jtvalenceerr, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// u 16!:_2  get: u=dic
// We create a verb to handle (get y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicgetc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(0,CMODX,VERB, jtdicget,jtdicget, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

// u 16!:_3  put: u=dic
// We create a verb to handle (x put y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicputc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(0,CMODX,VERB, jtvalenceerr,jtdicput, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}
#endif