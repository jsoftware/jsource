/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs                                                                 */

#include "j.h"

// These routines support IRS iff the underlying verb does, so all we have to do is switch the ranks if any and vector on to the function
// create inplace bits as copy of W, or swap A & W
static DF1(swap1){A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1];F1PREFIP; jtinplace = (J)(intptr_t)(((I)jtinplace&~JTINPLACEA)+2*((I)jtinplace&JTINPLACEW));
 // a~ carried the IRS flag from a and thus we might have ranks set.  If so, use them, and no need to check agreement again.  For ease, we just use whatever is set 
 A z; IRSIP2(w,w,fs,(RANKT)jt->ranks,(RANKT)jt->ranks,f2,z); R z;
}
static DF2(swap2){A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1];F2PREFIP; jtinplace = (J)(intptr_t)((I)jtinplace^((JTINPLACEW+JTINPLACEA)&(0x3C>>(2*((I)jtinplace&JTINPLACEW+JTINPLACEA)))));
 A z; IRSIP2(w,a,fs,(RANKT)jt->ranks,jt->ranks>>RANKTX,f2,z); R z;
}

// w~, which is either reflexive/passive or evoke
F1(jtswap){F1PREFIP;A y;C*s;I n;
 ARGCHK1(w); 
 if(VERB&AT(w)){
  // reflexive/passive.  Create verb that swaps.  Most flags do not apply to the derived verb
  I flag = FAV(w)->flag&(VIRS2|VJTFLGOK2); flag = (FAV(w)->flag&VASGSAFE)+flag+(flag>>1);  // set ASGSAFE, both inplace/irs bits from dyad; ISATOMIC immaterial, since always dyad
  I flag2 = ((FAV(w)->flag2&(VF2WILLOPEN2WPROP|VF2WILLOPEN2W))<<(VF2WILLOPEN2APROPX-VF2WILLOPEN2WPROPX)) | ((FAV(w)->flag2&(VF2WILLOPEN2APROP|VF2WILLOPEN2A))>>(VF2WILLOPEN2APROPX-VF2WILLOPEN2WPROPX));   // exchange WILLOPEN for dyad, clear for monad
  R fdef(flag2,CTILDE,VERB,(AF)(swap1),(AF)(swap2),w,0L,0L,flag,(I)(RMAX),(I)(rr(w)),(I)(lr(w)));
 }else{
  // evoke.  Ii must be LIT and convertible to ASCII.
  if((C2T+C4T)&AT(w))RZ(w=cvt(LIT,w)) else ASSERT(ISDENSETYPE(AT(w),LIT),EVDOMAIN);
  ASSERT(1>=AR(w),EVRANK);  // list or atom only
  n=AN(w); s=CAV(w); 
  ASSERT(vnm(n,s),EVILNAME);   // valid name
  RZ(y=nfs(AN(w),CAV(w)));  // create a NAME block for the string - not cacheable
  RZ(y=nameref(y,jt->locsyms));  // Create a name-reference pointing to the name
  // Make sure this reference is non-cached.  'name'~ is a way to get a non-cachable reference
  if(AT(y)&VERB+CONJ+ADV)FAV(y)->flag2&=~VF2CACHEABLE;  // turn off cachability if it's a reference (not if a noun, which doesn't have this flag)
  R y;
 }
}


static const B booltab[64]={
 0,0,0,0, 0,0,0,1, 0,0,1,0, 0,0,1,1,  0,1,0,0, 0,1,0,1, 0,1,1,0, 0,1,1,1,
 1,0,0,0, 1,0,0,1, 1,0,1,0, 1,0,1,1,  1,1,0,0, 1,1,0,1, 1,1,1,0, 1,1,1,1,
};

static DF2(jtbdot2){R from(plusA(duble(cvt(B01,a)),cvt(B01,w)),FAV(self)->fgh[2]);}  // dyad b. (2*a + w) { h

static DF1(jtbdot1){R bdot2(num(0),w,self);}

// (a b.) w
static DF1(jtbasis1){A fs=FAV(self)->fgh[0]; A z;D*x;I j;V*v;
 F1RANK(0,jtbasis1,self);
 RZ(w=vi(w));
 switch(AV(w)[0]){   // switch on arg
 case 0:  // rank
  GAT0(z,FL,3,1); x=DAV1(z); v=FAV(fs);
  j=v->mr; x[0]=j<=-RMAX?-inf:j>=RMAX?inf:j;
  j=lrv(v); x[1]=j<=-RMAX?-inf:j>=RMAX?inf:j;
  j=rrv(v); x[2]=j<=-RMAX?-inf:j>=RMAX?inf:j;
  R pcvt(INT,z);
 case -1: R lrep(inv (fs));  // inverse
 case  1: R lrep(iden(fs));  // neutral element
 default: ASSERT(0,EVDOMAIN);
 }
}

// a b.
F1(jtbdot){F1PREFIP;A b,h=0;I j=0,n,*v;
 ARGCHK1(w);
 A z; fdefallo(z)
 if(VERB&AT(w)){fdeffill(z,0,CBDOT,VERB,(AF)(jtbasis1),jtvalenceerr,w,0L,0L,VFLAGNONE,0,0,0) RETF(z);}
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 if(1==n){j=*v; ASSERT(BETWEENC(j,-16,34),EVINDEX);}
 else DQ(n, j=*v++; ASSERT(BETWEENC(j,-16,15),EVINDEX););  // j must be initialized because the loop might not run
 if(j<16){
  GAT0(b,B01,64,2); AS(b)[0]=16; AS(b)[1]=4; MC(AV2(b),booltab,64L);
  RZ(h=rifvs(cant2(IX(AR(w)),from(w,b))));  // h is an array representing b.  One cell for each atom of b; cell is 4 values
  fdeffill(z,0,CBDOT,VERB, jtbdot1,jtbdot2, 0L,w,h, VFLAGNONE, RMAX,0L,0L); RETF(z);
 }else{
  if(BETWEENC(j,32,34)){
   AF rtn=jtbitwiserotate; rtn=j==33?jtbitwiseshift:rtn;  rtn=j==34?jtbitwiseshifta:rtn; 
   fdeffill(z,0,CBDOT,VERB, jtbitwise1,rtn, 0L,w,0L, VASGSAFE|VJTFLGOK2, 0L,0L,0L); RETF(z);
  }
  z=ca(ds(j-16+CBW0000)); RZ(z); RZ(FAV(z)->fgh[1]=rifvs(w)); FAV(z)->id=CBDOT; RETF(z);  // use g field not f to avoid interfering with atomic2
 }
}

/* The h parameter in self for u M.                 */
/* 3 element boxed list                             */
/* 0 - integer atom of # of entries in hash table   */
/* 1 - 2-column integer table of arguments          */
/*     arguments are machine word integers          */
/*     column 1 is right arg; column 0 is left arg  */
/*     column 0 is IMIN for monad                   */
/* 2 - box list of results corresp. to arguments    */
/*     unused entries are set to 0                  */

#if SY_64
#define INITHASH(tbl,x,y) ((((UI4)(7*(UI)x+10495464745870458733U*(UI)y))*(UI)AN(tbl))>>32)  // starting hash index for a given x,y
#define LOCKLOC ht3->lock
#else
#define INITHASH(tbl,x,y) (((UI4)(7*(UI)x+10495464745870458733U*(UI)y))*(UIL)AN(tbl))>>32;  // starting hash index for a given x,y
#define LOCKLOC jt->etxn1  // any address will do, since locks are NOPs in 32-bit
#endif

// self->h is the hash block, a 3-element box that never moves, allocated at rank 0.  We lock on the h block
// AAV(h)[0] is the hashtable, holding indexes into the other tables.  AM has the active count, -1 for empty slots.  Allocated rank 0
// AAV(h)[1] is the key, an INT table holding y [and x, IMIN if no x].  AM has the active count.  Allocated rank 2
// AAV(h)[2] is the value, a BOX list corresponding to the key.  AM has the active count.  Allocated rank 0
// insert the result z for the key x,y.  The key probably doesn't exist in the hash
static A jtmemoput(J jt,I x,I y,A self,A z){
 RZ(z); rifvs(z);  // realize any virtual z
 // We need a write lock since we are modifying the table.  Then look to see if the tables need to be extended
 A ht3=FAV(self)->fgh[2];  // the 3-column hash table
 WRITELOCK(LOCKLOC);
 while(1){  // until all the tables are OK.  If a line fails, it has released the lock
  if(AM(AAV0(ht3)[1])==AS(AAV0(ht3)[1])[0]){RZ(jtextendunderlock(jt,&AAV0(ht3)[1],&LOCKLOC,0)) continue;}  // extend the key table
  if(AM(AAV0(ht3)[2])==AN(AAV0(ht3)[2])){RZ(jtextendunderlock(jt,&AAV0(ht3)[2],&LOCKLOC,0)) continue;}  // extend the result table
  if(AM(AAV0(ht3)[0])*2>AN(AAV0(ht3)[0])){RZ(jtextendunderlock(jt,&AAV0(ht3)[0],&LOCKLOC,1)) continue;}  // extend the hash table, noting that it is a hash
  break;
 }
 // when we get here we have the write lock and all the tables have the needed space.  Insert the result
 A hasht=AAV0(ht3)[0], keys=AAV0(ht3)[1], results=AAV0(ht3)[2];
 I (*v)[2]=(I (*)[2])IAV2(keys); I *hv=IAV0(hasht);   // point to keys and ht3able
 if(AM(hasht)==0){  // rehash the table if it was resized.  Table was initialized to -1
  DO(AM(results), I j=INITHASH(hasht,v[i][1],v[i][0]); while(0<=hv[j]){if(unlikely(--j<0))j+=AN(hasht);} hv[j]=i;)  // for each key, insert hash
  AM(hasht)=AM(results); // set # entries in hash
 }
 // install args and result in the next slot.  It is possible that two threads might store the same value; that's OK
 INCORPRA(z); AAV0(results)[AM(results)]=z;  // ra because ht3 is recursive
 v[AM(keys)][0]=y; v[AM(keys)][1]=x;   // install arg values

 I hi=INITHASH(hasht,x,y); while(0<=hv[hi]){if(unlikely(--hi<0))hi+=AN(hasht);}           // init and find the insertion point
 // hi is an empty slot.  Install the index of the keys/results we just added
 hv[hi]=AM(results)++;                      // have the hash point to new result, incr the # results
 ++AM(keys);                           // incr # keys, in lockstep with results
 ++AM(hasht);   // incr # hash entries
 WRITEUNLOCK(LOCKLOC); 
 R z;
}

// look up a given (x,y) combination.  x is IMIN for monadic references (IMIN is not a cacheable argument)
// Result is the stored result if any, or 0 if none found
static A jtmemoget(J jt,I x,I y,A self){A z=0;  // init to not found
 // Take a read lock on the table
 A ht3=FAV(self)->fgh[2];  // the 3-column hash table
 READLOCK(LOCKLOC);
 A hasht=AAV0(ht3)[0], keys=AAV0(ht3)[1], results=AAV0(ht3)[2];
 I (*v)[2]=(I (*)[2])IAV2(keys); I *hv=IAV0(hasht);   // point to keys and hashtable
 I hi=INITHASH(hasht,x,y); while(hv[hi]>=0){if(((v[hv[hi]][0]^y)|(v[hv[hi]][1]^x))==0){z=AAV0(results)[hv[hi]]; break;} if(unlikely(--hi<0))hi+=AN(hasht);}           // init search, exiting when key found
 READUNLOCK(LOCKLOC);
 R z;  // return match if found, 0 if not
}


// w is an arg; result is IMIN if not memoable
// memoable is: atomic int/bool or other with int value
static I jtint0(J jt,A w){A x;
 if(unlikely(AR(w)))R IMIN;
 if(unlikely(!(ISDENSETYPE(AT(w),B01+INT))))w=pcvt(INT,w);
 R w&&INT+B01&AT(w)?BIV0(w):IMIN;
}

static DF2(jtmemo12){A fs=FAV(self)->fgh[0];A z;I x,y;   // w is 0 for monad
 ARGCHK2(a,w); w=EPDYAD?w:0; AF f12=FAV(fs)->valencefns[!!w];
 x=int0(a); y=w&&x!=IMIN?int0(w):x;  // get arg value, IMIN if not memoable; for monad let y=x
 if(MIN(x,y)==IMIN)R CALL12(w,f12,a,w,fs);  // IMIN is unmemoable, run fn
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL12(w,f12,a,w,fs));  // if memo lookup returns empty, run the function and remember the result
}

// Create the memoed verb.  We create an h argument of hashtable;key;value, as described above
F1(jtmemo){F1PREFIP;PROLOG(300);A h,*hv;I m;
 ARGCHK1(w);
 ASSERT(VERB&AT(w),EVDOMAIN);
 V *v=FAV(w); FULLHASHSIZE(30,BOXSIZE,1,0,m);  // m = # items to allocate
 GAT0(h,BOX,3,0); hv=AAV0(h); AFLAGINIT(h,BOX)  // the components of fdef must be recursive if recursible
 // the tables are standard extendible, with # items in AM, thus must be zapped
 // So, we defer initializing them until they have been made recursive inside fdef
 GAT0(hv[0],INT,m,0) ACINITZAP(hv[0]) GAT0(hv[1],INT,2*(m>>1),2) ACINITZAP(hv[1]) GAT0(hv[2],BOX,m>>1,0) ACINITZAP(hv[2])  // allo hash/keys/results
 A z=fdef(0,CMCAP,VERB,jtmemo12,jtmemo12,w,0L,h,0L,v->mr,lrv(v),rrv(v));
 AM(hv[0])=0; mvc(m*SZI,AAV0(hv[0]),1,MEMSETFF);  // clear hash table
 AM(hv[1])=0; AS(hv[1])[0]=m>>1;  // init empty key table, 2 INTs each row
 AM(hv[2])=0;   // init empty result table, recursive
 R z;
 // Now we have converted the verb result to recursive usecount, and gotten rid of the pending tpops for the components of h
}
