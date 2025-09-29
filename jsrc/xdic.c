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
static NOINLINE /* scaf */ UI4 crcfloats(UI8 *v, I n){
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
static NOINLINE /* scaf */ UI4 crcwords(UI *v, I n){
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
static NOINLINE /* scaf */ UI4 crcbytes(C *v, I n){
 UI4 wdcrc=crcwords((UI*)v,n>>LGSZI);  // take CRC of the fullword part
 if(n&(SZI-1)){wdcrc=HASHI(wdcrc,((UI*)v)[n>>LGSZI]<<((SZI-(n&(SZI-1)))<<LGSZI));}  // if there are bytes, take their CRC after discarding garb.  Avoid overfetch
 R wdcrc;  // return composite CRC
}

// CRC32 of n boxes
static UI4 jtcrcy(J jt,A y);
static NOINLINE /* scaf */ UI4 crcboxes(J jt, A *v, I n){UI4 crc=0; DO(n, crc=HASH4(crc,jtcrcy(jt,C(v[i])));) R crc;}

// CRC32 of n Xs
static NOINLINE /* scaf */ UI4 crcxnums(X *v, I n){UI4 crc=0; DO(n, crc=HASH4(crc,crcwords(UIAV(v[i]),XLIMBLEN(v[i])));) R crc;}

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
#define DICFIHF 1  // set if using internal hashing function
#define DICFICF 2  // set if using internal compare function
    C lgminsiz;  // lg(minimum cardinality).  When cardinality drops below 1LL<<lgminsiz, we resize.  Top 3 bits always 0, used as LSBs of hashelesiz
    C hashelesiz;  // number of bytes in a hash slot, 1-5 (we support only 4)
   };
   UI hashsiz;  //  number of elements in hash table.  We leave space for 40 bits but we don't support more than 32
  } ;
  UI4 (*hashfn)();  // hash function, user's or selected internal
  signed char (*compfn)();  // key-comparison function, user's or selected internal
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

// u 16!:_1 y  conjunction.  allocate hashtable.  Always called from the numbered locale of the dictionary.
// x is the hash/compare verb OR the DIC block for a dictionary that is being resized.
// y is (min #eles,max #eles,#hashslots);(flags, currently empty);(key type;key shape);(value type;value shape)    to create a new dictionary
//      (min #eles,max #eles,#hashslots)   when a dictionary is being resized
// result is DIC to use, ready for get/put, with hash/keys/vals/empty allocated, and 0 valid kvs
F2(jtcreatedic){F12IP;A box,box1;  // temp for box contents
 ARGCHK2(a,w);
 ASSERT(AT(a)&VERB,EVDOMAIN)  // u must be the user's verb
 A z; GAT0(z,BOX,20,1) AK(z)=offsetof(struct ADic,bloc.hash); AN(z)=AS(z)[0]=8;  // allocate nonrecursive box, long enough to make the total allo big enough in 32- and 64-bit.  Then restrict to the boxes in case of error
 if(AT(a)&VERB){  // initial creation
  //  install user's verb, & see if the user wants the internal functions
  AF fn=FAV(a)->valencefns[0]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[0])->valencefns[0]:fn; ((DIC*)z)->bloc.flags=fn==jthashy?DICFIHF:0;  // note if default hash
  fn=FAV(a)->valencefns[1]; fn=(FAV(a)->id==CCOLON&&AT(FAV(a)->fgh[0])&VERB)?FAV(FAV(a)->fgh[0])->valencefns[1]:fn; I flags=((DIC*)z)->bloc.flags|=fn==jttao?DICFICF:0;  // note if default hash
  ASSERT((flags&DICFIHF+DICFICF)==DICFIHF+DICFICF,EVNONCE)  // only internal hash/compare for now
  INCORPNV(a); ((DIC*)z)->bloc.hashcompself=a;    // save self pointer to user's function, which protects it while this dic is extant
  ((DIC*)z)->bloc.locale=jt->global;  // remember the locale this dictionary is in.  We protect it in the DIC block

  // flags, key, value sizes
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==4,EVLENGTH)  // 4 boxes
  box=C(AAV(w)[1]); ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==0,EVLENGTH)  // flags.  None currently supported

  box=C(AAV(w)[2]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)  // keyspec.  must be 2 boxes
  box1=C(AAV(box)[0]); I t; RE(t=i0(box1)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN)  // type.  convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid
  box1=C(AAV(box)[1]); ASSERT(AR(box1)<=1,EVRANK) ASSERT(AN(box1)>=0,EVLENGTH) RZ(box1=ccvt(INT,ravel(box1),0)) I n, *s=IAV(box1); PRODX(n,AN(box1),s,1) ((DIC*)z)->bloc.kaii=n; ASSERT(n>0,EVLENGTH) // shape. copy to allow AAV1.  get # atoms in item & save
  INCORPNV(box1); ((DIC*)z)->bloc.kshape=box1; ((DIC*)z)->bloc.ktype=t; I l=n<<bplg(t); ((DIC*)z)->bloc.kbytelen=l; // save shape & type; save #bytes in key
  UI4 (*fn2)()=l&(SZI-1)?(UI4 (*)())crcbytes:(UI4 (*)())crcwords; fn2=(t&XNUM+RAT)?crcxnums:fn2; fn2=(t&CMPX+FL+QP)?crcfloats:fn2; fn2=(t&BOX)?crcboxes:fn2; fn2=flags&DICFIHF?fn2:(UI4 (*)())FAV(a)->valencefns[0]; ((DIC*)z)->bloc.hashfn=fn2; // save internal or external hash function  
  I (*fn3)()=l&(SZI-1)?(I (*)())taoc:(I (*)())taoi; fn3=(t&XNUM+RAT)?taox:fn3; fn3=(t&CMPX+FL+QP)?taof:fn3; fn3=(t&BOX)?taor:fn3; fn3=flags&DICFICF?fn3:(I (*)())FAV(a)->valencefns[1]; ((DIC*)z)->bloc.compfn=fn3; // save int/ext comp function.  We care only about equality  
  l>>=(fn2==crcboxes)?LGSZI:0; l>>=(fn2==crcfloats)?((t|(t>>(QPX-BOXX)))>>CMPX)+FLX:0; l>>=(fn2==crcxnums)?(t>>RATX)+LGSZI:0; l>>=(fn2==crcwords)?LGSZI:0; ((DIC*)z)->bloc.kitemlen=l;  // length to use for internal hash/comp

  box=C(AAV(w)[3]); ASSERT(AT(box)&BOX,EVDOMAIN) ASSERT(AR(box)==1,EVRANK) ASSERT(AN(box)==2,EVLENGTH)  // valuespec.  must be 2 boxes
  box1=C(AAV(box)[0]); RE(t=i0(box1)) ASSERT(((t=fromonehottype(t,jt))&NOUN)>0,EVDOMAIN)  // type. convert from 3!:0 form, which must be an atomic integer, to internal type, which must be valid
  box1=C(AAV(box)[1]); ASSERT(AR(box1)<=1,EVRANK) ASSERT(AN(box1)>=0,EVLENGTH) RZ(box1=ccvt(INT,ravel(box1),0)) s=IAV(box1); PRODX(n,AN(box1),s,1) ((DIC*)z)->bloc.vaii=n;  // shape. copy to allow AAV1.  get # atoms in item & save
  INCORPNV(box1); ((DIC*)z)->bloc.vshape=box1; ((DIC*)z)->bloc.vtype=t; ((DIC*)z)->bloc.vbytelen=n<<bplg(t);  // save shape & type; save # bytes for copy
  box=C(AAV(w)[0]);  // fetch size parameters
 }else{  // resize
  ((DIC*)z)->bloc=((DIC*)a)->bloc;  // init everything from the previous dic
  box=w;  // w is nothing but size parms
 }

 // box has the size parameters.  Audit & install into dic, overwriting anything that was copied
 ASSERT(AR(box)<=1,EVRANK) ASSERT(AN(box)==3,EVLENGTH) if(!AT(box)&INT)RZ(box=ccvt(INT,box,0));  // sizes. must be box of 3 integers
 ((DIC*)z)->bloc.lgminsiz=CTLZI(UIAV(box)[0]|1); I maxeles=IAV(box)[1]; ASSERT(maxeles>0,EVDOMAIN) I hashsiz=((DIC*)z)->bloc.hashsiz=IAV(box)[2]; ASSERT(hashsiz>(maxeles+(maxeles>>4)),EVDEADLOCK)  // min, max, hash sizes.  Hash at least 6% spare
 I hashelesiz=((DIC*)z)->bloc.hashelesiz=(CTLZI(maxeles+4-1)+1+(BB-1))>>LGBB;  // max slot#, plus 4 (empty/tombstone/birthstone).  Subtract 1 for max code point.  top bit#+1 is #bits we need; round that up to #bytes

 // allocate & protect hash/keys/vals/empty
 GATV0(box,LIT,hashsiz*hashelesiz,1) INCORPNV(box) mvc(hashsiz*hashelesiz,voidAV1(box),MEMSET00LEN,MEMSET00); ((DIC*)z)->bloc.hash=box;  // allocate hash table & fill with empties
 GATV0(box,LIT,maxeles*hashelesiz,2) AS(box)[0]=maxeles; AS(box)[1]=hashelesiz; INCORPNV(box) void *ev=voidAV2(box); DO(maxeles-1, *(I*)ev=i+1; ev=(void *)((I)ev+hashelesiz);)   // allocate empty list & chain empties together
 *(I*)ev=maxeles-1; ((DIC*)z)->bloc.empty=box; // install end of chain loopback and point the dic to beginning of chain
 GA0(box,((DIC*)z)->bloc.ktype,maxeles*((DIC*)z)->bloc.kaii,AN(((DIC*)z)->bloc.kshape)+1) INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(((DIC*)z)->bloc.kshape),AN(((DIC*)z)->bloc.kshape)) ((DIC*)z)->bloc.keys=box;   // allocate array of keys
 GA0(box,((DIC*)z)->bloc.vtype,maxeles*((DIC*)z)->bloc.vaii,AN(((DIC*)z)->bloc.kshape)+1) INCORPNV(box) AS(box)[0]=maxeles; MCISH(AS(box)+1,IAV1(((DIC*)z)->bloc.vshape),AN(((DIC*)z)->bloc.vshape)) ((DIC*)z)->bloc.vals=box;   // allocate array of vals
 ((DIC*)z)->bloc.cardinality=0;  // init the dic is empty
 ra0(z); INCORP(z); AM(z)=0;  // make z recursive, protecting descendants; INCORP and clear the lock
 RETF(z)
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
// A write lock guarantees that no other thread has a lock of any kind.  Any modification is allowed.

// scaf to add: write-lock the keys & values separately; free the keys first, then the values

#if PYXES
// The AM field of dic is the lock. 0-15=#read locks outstanding 16-31=#prewrite locks outstanding 32=write equest 48-63=seq# of current writer (if 32 set) or next writer (if 32 clear)
#define DICLMSKRDX 0LL  // read locks outstanding
#define DICLMSKRWX 16LL  // write locks request, by size 0-3
#define DICLMSKWRX 32LL  // write lock requested by current write owner
#define DICLMSKSEQX 48LL  // sequence# of current write owner

#define DICLKRDRQ(dic) __atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL)  // put up read request
#define DICLKRDWTK(dic,lv) if(unlikely((lv&((I)1<<DICLMSKWRX))!=0))lv=diclkrdwtk(dic);  // if someone is writing, wait till they finish with hash/keys 
#define DICLKRDWTV(dic,lv) if(unlikely((lv&((I)2<<DICLMSKWRX))!=0))diclkrdwtv(dic);  // if someone is writing, wait till they finish with values
#define DICLKRDREL(dic)   __atomic_fetch_sub(&AM((A)dic),(I)1<<DICLMSKRDX,__ATOMIC_ACQ_REL);  // remove read request

#define DICLKRWRQ(dic) __atomic_fetch_add(&AM((A)dic),(I)1<<DICLMSKRWX,__ATOMIC_ACQ_REL)   // put up prewrite request
#define DICLKRWWT(dic,lv) if(unlikely((US)(lv>>DICLMSKRWX)!=(US)(lv>>DICLMSKSEQX)))diclkrwwt(dic,lv);  // wait until we are the lead writer, which is immediately if there are no others
#define DICLKWRRQ(dic) ({ __atomic_store_n((US*)((C*)&AM((A)dic)+4),(US)3,__ATOMIC_RELEASE); __atomic_load_n(&AM((A)dic),__ATOMIC_ACQUIRE); })   // put up write request for key & value (we are known to be the lead writer and own the write bit, which must be clear)
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
#define DICLKRDRQ(dic) 0
#define DICLKRDWTK(dic,lv) // if someone is writing, wait till they finish with hash/keys
#define DICLKRDWTV(dic,lv)  // if someone is writing, wait till they finish with values
#define DICLKRDREL(dic)

#define DICLKRWRQ(dic) 0
#define DICLKRWWT(dic,lv) 
#define DICLKWRRQ(dic) 0
#define DICLKWRWT(dic,lv) 0
#define DICLKWRRELK(dic,lv) 
#define DICLKWRRELV(dic,lv) 
#endif

// Request resize.  jt->globals has been set to the dic locale, so we look up resize there
A dicresize(DIC *dic,J jt){
 A *_ttop=jt->tnextpushp;  // stack restore point
  // call dicresize in the locale of the dic.  No need for explicit locative because locale is protected by dic
 A nam, val;
 ASSERT(((nam=nfs(6,"resize",0))!=0)
  &&((val=jtsyrd1((J)((I)jt+NAV(nam)->m),NAV(nam)->s,NAV(nam)->hash,jt->global))!=0  // look up name in current locale and ra() if found
  &&((val=QCWORD(namerefacv(nam,QCWORD(val))))!=0)   // turn the value into a reference, undo the ra
  &&((val&&LOWESTBIT(AT(val))&VERB))),EVVALUE)   // make sure the result is a verb
 A newdic; RZ(newdic=jtunquote(jt,(A)dic,val,val));  // execute resize on the dic, returning new dic
 struct Dic t=dic->bloc; ((DIC*)dic)->bloc=((DIC*)newdic)->bloc; ((DIC*)newdic)->bloc=t;  // Exchange the parms and data areas from the new dic to the old.  Since they are recursive, this exchanges ownership and will cause the old blocks to be freed when the new dic is.
 // NOTE: we keep the old blocks hanging around until the new have been allocated.  This seems unnecessary for the hashtable, but we do it because other threads still have the old pointers and may prefetch from
 //  the old hash.  This won't crash, but it might be slow if the old hash is no longer in mapped memory
 tpop(_ttop);  // discard newdic and everything it refers to
 R (A)dic;  // always return the same block if no error.
}

// Hashtable info ***********************
#define HASHNRES 4  // # reserved code points for empties: empty/birthston/tombstone/tombstone+birthstone
#define HASHBSTN 1  // this bit set when the slot is a birthstone (a slot marked for fill by a put)
#define HASHTSTN 2  // this bit set when the slot is a tombstone (an empty slot that must not terminate a search

// k is A for keys, n is #keys, s is place for slot#s.  Hash each key, store, prefetch (possibly using wrong hash)
// This version works on internal hash functions only
// result is 0 on error
static NOINLINE /* scaf */ B jtkeyprep(DIC *dic, void *k, I n, I8 *s,J jt){I i;
 UI hsz=dic->bloc.hashsiz; I kib=dic->bloc.klens; UI4 (*hf)()=dic->bloc.hashfn; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 for(i=n;--i>=0;){
  k=(void*)((I)k-(kib>>32));  // back up to next key
  s[i]=(I8)(*hf)(k,(UI4)kib,jt); PREFETCH(hashtbl[(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56)]);
 }
 R 1;
}
// k is A for keys, kvirt is virtual unincorpable block to point to key, s is place for slot#s.  Hash each key, convert to slot, store, prefetch

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy the value to the result
// a is the original a, 0 if monad
// This version works on internal compare functions only
// We must hold a read lock on the table
static NOINLINE /* scaf */ B jtgetslots(DIC *dic,void *k,I n,I8 *s,void *zv,J jt,A a){I i;
 I lv=DICLKRDRQ(dic); DICLKRDWTK(dic,lv)  // request read lock and wait for it to be granted.  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
 UI hsz=dic->bloc.hashsiz; I kib=dic->bloc.klens; I (*cf)(I,void*,void*)=dic->bloc.compfn; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen
 k=(void*)((I)k+n*(kib>>32));  // move to end+1 key to save a reg by counting down
 C *kbase=CAV(dic->bloc.keys)-HASHNRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array
 // convert the hash slot#s to index into kvs
 for(i=n;--i>=0;){
  k=(void*)((I)k-(kib>>32));  // back up to next key
  I curslot=(((UI8)s[i]*(UI4)hsz)>>32)*(hsz>>56);  // convert hash to slot# and then to byte offset.
  UI hval; s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv
  while(unlikely(hval<HASHNRES) || unlikely((*cf)((UI4)kib,k,kbase+(kib>>32)*hval))){  // loop till we hit a valid value that compares equal on the key
        // clang compiler bug 202510: if cf is declared without prototype, the (UI4) is ignored
   if(s[i]<HASHTSTN)break;   // if we hit an empty (incl birthstone empty but not a tombstone), that ends the search
   if(unlikely((curslot-=(hsz>>56))<0))curslot+=(UI4)hsz*(hsz>>56);  // move to next hash slot, wrap to end if we hit 0
   s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot],(hsz>>53));  // fetch next hash value
  }
 }
 // copy using the kv indexes we calculated.  Copy in ascending order so we can overstore
 DICLKRDWTV(dic,lv)  // wait till values are safe
 I vn=dic->bloc.vbytelen; C *vbase=CAV(dic->bloc.vals)-HASHNRES*vn;  // size of a value; address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the value array
 void *av=0;  // init to 'no default data pointer yet'.  We avoid checking the default until we know we need it
 for(i=0;i<n;++i){void *vv;  // pointer to value to move
  if(likely(s[i]>=HASHNRES))vv=vbase+s[i]*vn;   // this is the main line, key found
  else{   // default required, which we deem rare.
   if(unlikely(av==0)){
    // First time through here. We have to audit it & convert if needed
    ASSERT(a!=0,EVINDEX)   // error if no default given
    ASSERT(AR(a)==AN(dic->bloc.vshape),EVRANK) ASSERTAGREE(AS(a),IAV1(dic->bloc.vshape),AR(a))  // verify default has correct rank & shape
    if(unlikely((AT(a)&dic->bloc.vtype)==0))RZ(a=ccvt(dic->bloc.vtype,a,0))   // convert precision if needed
    av=CAV(a);  // 
   }
   vv=av;    // use the default after it has been audited
  }
  MC(zv,vv,vn); zv=(void *)((I)zv+vn);   // move the data & advance popinter to next one   scaf JMC?
 }
 R 1;
}

//   get.  conjunction.  u is dic, v is hash/comp function.    Bivalent. w is keys, [a] is default value, 0 if monad.  Called from parse/unquote as a,w,self or w,self,self
// This version for internal functions only
static DF2(jtdicget){F12IP;A z;
 ARGCHK2(a,w)
 A adyad=w!=self?a:0; w=w!=self?w:a;  // if dyad, keep a,w, otherwise 0,w
 DIC *dic=(DIC*)FAV(self)->fgh[0]; I kt=dic->bloc.ktype; I kr=AN(dic->bloc.kshape), *ks=IAV1(dic->bloc.kshape);  // point to dic block, key type, shape of 1 key.  Must not look at hash etc yet
 I wf=AR(w)-kr; ASSERT(wf>=0,EVRANK) ASSERTAGREE(AS(w)+wf,ks,kr)   // w must be a single key or an array of them, with correct shape
 if(unlikely(AN(w)==0)){GA0(z,dic->bloc.vtype,0,wf+AN(dic->bloc.vshape)) MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AAV1(dic->bloc.vshape),AN(dic->bloc.vshape)) R z;}  // if no keys, return empty fast
 if(unlikely((AT(w)&kt)==0))RZ(w=ccvt(kt,w,0))   // convert type of w if needed
 I kn; PROD(kn,wf,AS(w))   // kn = number of keys to be looked up
 GA0(z,dic->bloc.vtype,kn*dic->bloc.vaii,wf+AN(dic->bloc.vshape)) MCISH(AS(z),AS(w),wf) MCISH(AS(z)+wf,AAV1(dic->bloc.vshape),AN(dic->bloc.vshape))   // allocate result area & install shape
 I8 sd[16], *s=sd; if(unlikely(kn>(I)(sizeof(sd)/sizeof(sd[0])))){GATV0(z,FL,kn,1) s=(I8*)voidAV1(z);}   // allocate slots block, locally if possible.  FL is always 8 bytes
 void *k=voidAV(w);  // point to the key data
 RZ(jtkeyprep(dic,k,kn,s,jt))  // convert keys to slot#
 RZ(jtgetslots(dic,k,kn,s,voidAV(z),jt,adyad));  // get the values
 DICLKRDREL(dic)  // release read lock
 RETF(z);
}

// write l bytes from x to address a.  Destroys x, which must be a name
#define WRHASH1234(x,l,a) {if((l)&4)*(UI4*)(a)=x;else{if((l)&1){*(C*)(a)=x;x>>=8;}if((l)&2)*(US*)((C*)a+((l)&1))=x;}}  // avoid read, which will probably be a long miss.  Branches will predict
// same for writing ~0
#define WR1s1234(l,a) {if((l)&4)*(UI4*)(a)=~(I)0;else{if((l)&1){*(C*)(a)=~(I)0;}if((l)&2)*(US*)((C*)a+((l)&1))=~(I)0;}}  // avoid read, which will probably be a long miss.  Branches will predict

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
// We have requested a prewrite lock; we may even have a full write lock on the keys and value
// return holding a write lock on this dic, and rc=0 to request a resize
static NOINLINE /* scaf */ B jtputslots(DIC *dic,void *k,I n,void *v,I vn,I8 *s,J jt,I lv){I i;
 DICLKRWWT(dic,lv)  // wait for pre-write lock to be granted (NOP if we already have a write lock).  The DIC may have been resized during the wait, so pointers and limits must be refreshed after the lock
    // with this lock we can add new kvs, or change an empty/tombstone to a birthstone; but no other hash changes, and no value overwrites

 UI hsz=dic->bloc.hashsiz; I kib=dic->bloc.klens; I (*cf)()=dic->bloc.compfn; C *hashtbl=CAV1(dic->bloc.hash);  // elesiz/hashsiz kbytelen/kitemlen  compare func  base of hashtbl
 C *kbase=CAV(dic->bloc.keys)-HASHNRES*(kib>>32);  // address corresponding to hash value of 0.  Hashvalues 0-3 are empty/tombstone/birthstone and do not take space in the key array

 // first pass over keys.  If key found, remember the biased kv# (will go to old chain).  If not found, remember the hashslot# and whether it was occupied by a birthstone; and make it a birthstone - will go to new or conflict chain
 // We have to go in ascending order because later keys must overwrite earlier ones
 for(i=0;i<n;++i){
  I8 curslot=(((UI8)s[i]*(UI4)hsz)>>32);  // convert hash to slot# and then to byte offset.
  UI hval; s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
  while(hval>=HASHNRES && !(*cf)((UI4)kib,(void*)((I)k+i*(kib>>32)),kbase+(kib>>32)*hval)){  // loop till we hit a valid value that compares equal on the key
   if(unlikely(curslot--<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
   s[i]=hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));  // fetch next hash value, flagged as old key
  }
  if(s[i]<HASHNRES){hashtbl[curslot]=s[i]|HASHBSTN; s[i]=((I8)0x100000000<<s[i])+curslot;}   // if search ends at empty/birthstone; replace it with a birthstone and set s[i] to type of empty\hashslot#
         // We must touch only the low byte so the store can be atomic in the hashvalue
 }

 // pass slot# back to front, putting them into 3 chains: new kvs, old keys, conflict keys.  Conflict keys are assumed vary rare (usually repeated keys)
 I nroot=-1, oroot=-1, croot=-1;   // base of the 3 chains, inited to empty
 DQ(n, I sh=((UI4*)(&s[i]))[1]; if(unlikely(sh&((1LL<<HASHTSTN)+(1LL<<(HASHTSTN+HASHBSTN))))){((UI4*)(&s[i]))[1]=croot; croot=i;}else{((I4*)(&s[i]))[1]=sh?nroot:oroot; nroot=sh?i:nroot; oroot=sh?oroot:i;})

 // chase the new kvs, taking an empty slot for each and copying the k and v into the tables. s[] holds the hashslot
 I vb=dic->bloc.vbytelen; C *empty=CAV2(dic->bloc.empty); UI emptyx=dic->bloc.emptyn; I cur;  // empty area & current pointer into it.  Length of a element is hsz>>56
 for(cur=nroot;cur>=0;){
  I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  UI emptynxt=_bzhi_u64(*(UI4*)&empty[emptyx*(hsz>>56)],(hsz>>53)); if(emptynxt==emptyx)goto resize;  // get next empty, abort if end of chain (loopback)
  MC(CAV(dic->bloc.keys)+emptyx*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32); MC(CAV(dic->bloc.vals)+emptyx*vb,(void*)((I)v+cur*vb),vb);
  cur=nxt; emptyx=emptynxt;  // advance to next
 }

 lv=DICLKWRRQ(dic); DICLKWRWT(dic,lv)  // request pre-write and wait for it to be granted.  No resize is possible

 // chase the new kvs again, replacing the birthstone (indexed bu s[i]) with the allocated kvslot#, which we get by retraversing the empties list.  Also mark the new kvs not-empty
 for(cur=nroot,emptyx=dic->bloc.emptyn;cur>=0;){
  I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  UI emptynxt=_bzhi_u64(*(UI4*)&empty[emptyx*(hsz>>56)],(hsz>>53));   // get next empty, abort if end of chain (loopback)
  WR1s1234(hsz>>56,&empty[emptyx*(hsz>>56)]) emptyx+=HASHNRES; WRHASH1234(emptyx, hsz>>56, &hashtbl[(UI4)s[cur]*(hsz>>56)])  // set empty slot to ~0, set hashtable to point to new kv (skipping over reserved hashslot#s)
  cur=nxt; emptyx=emptynxt;  // advance to next
 }

 // chase the conflict keys (in ascending order), updating everything.  They should be few.  For new kvs, also take an empty slot & update the keys.  Leave values in the conflict chain, in ascending order. s[i] indexes the hashslot
 for(cur=croot;cur>=0;){
  I nxt=((I4*)(&s[cur]))[1];  // unroll the fetch once
  I curslot=(UI4)s[cur];  // slot was originally birthstone where the search ended.  Now it has been filled in, and we resume the search there.  It could match right there
  UI hval; hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));   // point to field beginning with hash value, clear higher bits. remember the hash value, which will be the index of the kv.  high 32 0='old key'
  while(hval>=HASHNRES && !(*cf)((UI4)kib,(void*)((I)k+i*(kib>>32)),kbase+(kib>>32)*hval)){  // loop till we hit a valid value that compares equal on the key
   if(unlikely(curslot--<0))curslot+=(UI4)hsz;  // move to next hash slot, wrap to end if we hit 0
   hval=_bzhi_u64(*(UI4*)&hashtbl[curslot*(hsz>>56)],(hsz>>53));  // fetch next hash value
  }
  if(hval<HASHNRES){
   // search ends at empty/tombstone, in hashslot (curslot).  Allocate a new empty, point curslot to it, move in the key, save the slot in s[cur]
   emptyx=dic->bloc.emptyn; UI emptynxt=_bzhi_u64(*(UI4*)&empty[emptyx*(hsz>>56)],(hsz>>53)); if(emptynxt==emptyx)goto resize; hval=emptyx+HASHNRES; dic->bloc.emptyn=emptynxt; // get empty slot, save as biased kv slot#
   WR1s1234(hsz>>56,&empty[emptyx*(hsz>>56)]) emptyx=hval; WRHASH1234(emptyx, hsz>>56, &hashtbl[(UI4)s[cur]*(hsz>>56)])   // set empty slot to ~0, set hashtable to point to new kv (skipping over reserved hashslot#s)
   MC(kbase+hval*(kib>>32),(void*)((I)k+cur*(kib>>32)),kib>>32);    // copy the new key
  }
  ((UI4*)(&s[cur]))[0]=hval;  // remember the biased slot the value must be moved into.  This changes s[i] from hashslot index to kv index
  cur=nxt;  // advance to next
 }

 DICLKWRRELK(dic,lv)    // allow gets to look at hashtable & keys

 // we have updated the keys and hash.  Now move the values, indexed by s[i] (biased).  Every value gets moved once.  We move the old then the conflicts, knowing that any old must precede any conflict that maps to the same slot
 C *vbase=CAV(dic->bloc.vals)-HASHNRES*vb;   // base pointer to allocated values, backed up to skip over the empty/birthstone/tombstone codes
 for(cur=oroot;cur>=0;){I nxt=((I4*)(&s[cur]))[1]; UI kvslot=((UI4*)(&s[cur]))[0]; MC(vbase+kvslot*vb,(void*)((I)v+cur*vb),vb); cur=nxt;} // chase the old keys, copying the value
 for(cur=croot;cur>=0;){I nxt=((I4*)(&s[cur]))[1]; UI kvslot=((UI4*)(&s[cur]))[0]; MC(vbase+kvslot*vb,(void*)((I)v+cur*vb),vb); cur=nxt;} // chase the conflict keys, copying the value
 R 1;  // return, holding write lock on values

resize: R 0;  // return holding write lock on keys & values
}

//  put.  conjunction.  u is dic, v is hash/comp function.  a is values, w is keys
// This version for internal functions only
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
 I8 sd[16], *s=sd; if(unlikely(kn>(I)(sizeof(sd)/sizeof(sd[0])))){A z; GATV0(z,FL,kn,1) s=(I8*)voidAV1(z);}   // allocate slots block, locally if possible.  FL is always 8 bytes
 void *k=voidAV(w);  // point to the key data
 RZ(jtkeyprep(dic,k,kn,s,jt))  // hash keys & prefetch
 void *v=voidAV(a);  // point to the value data
 I lv=DICLKRWRQ(dic);   // request prewrite lock, which we keep until end of operation (perhaps including resize)
 while(jtputslots(dic,k,kn,v,vn,s,jt,lv)==0)dicresize(dic,jt);  // do the puts.  If we have to resize, we abort with the puts partially complete, and then retry, keeping the dic under lock the whole time
 DICLKWRRELV(dic,lv)    // we are finished. advance sequence# and allow everyone to look at values
 RETF(mtv);
}

#if 0

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
B jtdelslots##STN(DIC *dic,A k,A kvirt,STX *s,A z,A jt){
 // init empty-slot index
 // this loop will be unrolled 3 times, one for each fetch from remote memory
 // read from the slot
 // prefetch key
 // loop till found: compare key[slot] against k[i]
 // if match: save neg. index of value slot
}

// k is A for keys, s is slot#s, z is result block (rank 1 for isin, >1 for get)
// resolve each key in the hash and copy new kvs
B jtdelslots##STN(DIC *dic,A k,A kvirt,STX *s,A z,A jt){
 // init empty-slot index
 // for each slot:
  // if neg, add key# at base of empty chain
 // restore empty-slot index, update cardinality
 // if (cardinality less than minimum)abort, call for resize
}

// del.  conjunction.  u is dic, v is hash/comp function.  w is keys
DF1(jtdicdel##STN){
 DIC *dic=FAV(self)->fgh[0];  // point to dic block
 // allocate slots block
 // if keys are not the right shape & type, convert/extend them
 // allocate virtual block for key
 RZ(jtkeyprep(dic,k,kvirt,s,jt))  // convert keys to slot#
 // take prewrite lock;
 // refresh dic info
 z=jtdelslots##STN(dic,k,kvirt,s,z,jt);  // classify keys & move new kvs
 // take write lock;
 if(resize requested)R dicresize();
 z=jtdelkeys##STN(dic,k,kvirt,s,z,jt);  // finish the deletion
 // release write lock;
 RETF(z);
}

// u 16!:_4  del: u=dic
// We create a verb to handle (del y).  It is up to the user (or a name) to run it in the correct locale.  We raise the locale to keep it valid while this verb is about.
DF1(jtdicdelc){F12IP;
 // We must not anticipate any values about the Dic because they may change during a resize and will not be visible to threads that have not taken a lock on the Dic
 ARGCHK1(w)
 R fdef(0,CMODX,VERB, jtdicdel,jtvalenceerr, w,self,0, VFLAGNONE, RMAX,RMAX,RMAX); 
}

#endif
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