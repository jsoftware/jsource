/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs                                                                 */

#include "j.h"

// These routines support IRS iff the underlying verb does, so all we have to do is switch the ranks if any and vector on to the function
// create inplace bits as copy of W, or swap A & W
static DF1(swap1){DECLF; F1PREFIP; jtinplace = (J)(intptr_t)(((I)jtinplace&~JTINPLACEA)+2*((I)jtinplace&JTINPLACEW));
 // a~ carried the IRS flag from a and thus we might have ranks set.  If so, use them, and no need to check agreement again.  For ease, we just use whatever is set 
 A z; IRSIP2(w,w,fs,(RANKT)jt->ranks,(RANKT)jt->ranks,f2,z); R z;
}
static DF2(swap2){DECLF; F2PREFIP; jtinplace = (J)(intptr_t)((I)jtinplace^((JTINPLACEW+JTINPLACEA)&(0x3C>>(2*((I)jtinplace&JTINPLACEW+JTINPLACEA)))));
 A z; IRSIP2(w,a,fs,(RANKT)jt->ranks,jt->ranks>>RANKTX,f2,z); R z;
}

// w~, which is either reflexive/passive or evoke
F1(jtswap){A y;C*s;I n;
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
  RZ(y=nfs(AN(w),CAV(w)));  // create a NAME block for the string
  RZ(y=nameref(y,jt->locsyms));  // Create a name-reference pointing to the name
  // Make sure this reference is non-cachable.  'name'~ is a way to get a non-cachable reference
  if(AT(y)&VERB+CONJ+ADV)FAV(y)->localuse.lu1.cachedref=0;  // turn off cachability if it's a reference (not if a noun)
  R y;
 }
}


static const B booltab[64]={
 0,0,0,0, 0,0,0,1, 0,0,1,0, 0,0,1,1,  0,1,0,0, 0,1,0,1, 0,1,1,0, 0,1,1,1,
 1,0,0,0, 1,0,0,1, 1,0,1,0, 1,0,1,1,  1,1,0,0, 1,1,0,1, 1,1,1,0, 1,1,1,1,
};

static DF2(jtbdot2){R from(plusA(duble(cvt(B01,a)),cvt(B01,w)),FAV(self)->fgh[2]);}  // dyad b. (2*a + w) { h

static DF1(jtbdot1){R bdot2(num(0),w,self);}

static DF1(jtbasis1){DECLF;A z;D*x;I j;V*v;
 PREF1(jtbasis1);
 RZ(w=vi(w));
 switch(AV(w)[0]){
  case 0:
   GAT0(z,FL,3,1); x=DAV(z); v=FAV(fs);
   j=v->mr; x[0]=j<=-RMAX?-inf:j>=RMAX?inf:j;
   j=lrv(v); x[1]=j<=-RMAX?-inf:j>=RMAX?inf:j;
   j=rrv(v); x[2]=j<=-RMAX?-inf:j>=RMAX?inf:j;
   R pcvt(INT,z);
  case -1: R lrep(inv (fs));
  case  1: R lrep(iden(fs));
  default: ASSERT(0,EVDOMAIN);
}}

F1(jtbdot){A b,h=0;I j=0,n,*v;
 ARGCHK1(w);
 if(VERB&AT(w))R ADERIV(CBDOT, jtbasis1,0L, 0L,0,0,0);
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 if(1==n){j=*v; ASSERT(BETWEENC(j,-16,34),EVINDEX);}
 else DQ(n, j=*v++; ASSERT(BETWEENC(j,-16,15),EVINDEX););  // j must be initialized because the loop might not run
 if(j<16){
  GAT0(b,B01,64,2); AS(b)[0]=16; AS(b)[1]=4; MC(AV(b),booltab,64L);
  RZ(h=rifvs(cant2(IX(AR(w)),from(w,b))));  // h is an array representing b.  One cell for each atom of b; cell is 4 values
  R fdef(0,CBDOT,VERB, jtbdot1,jtbdot2, 0L,w,h, VFLAGNONE, RMAX,0L,0L);
 }else{
  if(BETWEENC(j,32,34)){
   AF rtn=jtbitwiserotate; rtn=j==33?jtbitwiseshift:rtn;  rtn=j==34?jtbitwiseshifta:rtn; 
   R fdef(0,CBDOT,VERB, jtbitwise1,rtn, 0L,w,0L, VASGSAFE|VJTFLGOK2, 0L,0L,0L);
  }
  A z=ca(ds(j-16+CBW0000)); RZ(z); RZ(FAV(z)->fgh[1]=rifvs(w)); FAV(z)->id=CBDOT; RETF(z);  // use g field not f to avoid interfering with atomic2
 }
}

#if 0

/* The h parameter in self for u M.                 */
/* 3 element boxed list                             */
/* 0 - integer atom of # of entries in hash table   */
/* 1 - 2-column integer table of arguments          */
/*     arguments are machine word integers          */
/*     column 1 is right arg; column 0 is left arg  */
/*     column 0 is IMIN for monad                   */
/* 2 - box list of results corresp. to arguments    */
/*     unused entries are set to 0                  */
#endif

#if SY_64
// obsolete #define HIC(x,y)  ((UI)x+10495464745870458733U*(UI)y)
#define INITHASH(tbl,x,y) ((((UI4)(7*(UI)x+10495464745870458733U*(UI)y))*(UI)AN(tbl))>>32)  // starting hash index for a given x,y
#else
// obsolete #define HIC(x,y)  ((UI)x+2838338383U*(UI)y)
#define INITHASH(tbl,x,y) (((UI4)(7*(UI)x+10495464745870458733U*(UI)y))*(UIL)AN(tbl))>>32;  // starting hash index for a given x,y
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
#if BW==64
 WRITELOCK(ht3->lock);
 while(1){  // until all the tables are OK
  if(AM(AAV0(ht3)[1])==AS(AAV0(ht3)[1])[0]){RZ(jtextendunderlock(jt,&AAV0(ht3)[1],&ht3->lock,0)) continue;}  // extend the key table
  if(AM(AAV0(ht3)[2])==AN(AAV0(ht3)[2])){RZ(jtextendunderlock(jt,&AAV0(ht3)[2],&ht3->lock,0)) continue;}  // extend the result table
  if(AM(AAV0(ht3)[0])*2>AN(AAV0(ht3)[0])){RZ(jtextendunderlock(jt,&AAV0(ht3)[0],&ht3->lock,1)) continue;}  // extend the hash table, noting that it is a hash
  break;
 }
#endif
 // when we get here we have the write lock and all the tables have the needed space.  Insert the result
 A hasht=AAV0(ht3)[0], keys=AAV0(ht3)[1], results=AAV0(ht3)[2];
 I (*v)[2]=(I (*)[2])IAV2(keys); I *hv=IAV0(hasht);   // point to keys and ht3able
 if(AM(hasht)==0){  // rehash the table if it was resized.  Table was initialized to -1
  DO(AM(results), I j=INITHASH(hasht,v[i][1],v[i][0]); while(0<=hv[j]){if(unlikely(--j<0))j+=AN(hasht);} hv[j]=i;)  // for each key, insert hash
  AM(hasht)=AM(results); // set # entries in hash
 }
 // install args and result in the next slot.  It is possible that two threads might store the same value; that's OK
 ra(z); AAV0(results)[AM(results)]=z;  // ra because ht3 is recursive
 v[AM(keys)][0]=y; v[AM(keys)][1]=x;   // install arg values

 I hi=INITHASH(hasht,x,y); while(0<=hv[hi]){if(unlikely(--hi<0))hi+=AN(hasht);}           // init and find the insertion point
 // hi is an empty slot.  Install the index of the keys/results we just added
 hv[hi]=AM(results)++;                      // have the hash point to new result, incr the # results
 ++AM(keys);                           // incr # keys, in lockstep with results
 ++AM(hasht);   // incr # hash entries
#if BW==64
 WRITEUNLOCK(ht3->lock); 
#endif
 R z;
}

// look up a given (x,y) combination.  x is IMIN for monadic references (IMIN is not a cacheable argument)
// Result is the stored result if any, or 0 if none found
static A jtmemoget(J jt,I x,I y,A self){A z=0;  // init to not found
 // Take a read lock on the table
 A ht3=FAV(self)->fgh[2];  // the 3-column hash table
 READLOCK(ht3->lock);
 A hasht=AAV0(ht3)[0], keys=AAV0(ht3)[1], results=AAV0(ht3)[2];
 I (*v)[2]=(I (*)[2])IAV2(keys); I *hv=IAV0(hasht);   // point to keys and hashtable
 I hi=INITHASH(hasht,x,y); while(hv[hi]>=0){if(((v[hv[hi]][0]^y)|(v[hv[hi]][1]^x))==0){z=AAV0(results)[hv[hi]]; break;} if(unlikely(--hi<0))hi+=AN(hasht);}           // init search, exiting when key found
 READUNLOCK(ht3->lock);
 R z;  // return match if found, 0 if not
}


#if 0  // obsolete 

static A jtmemoget(J jt,I x,I y,A self){A h,*hv,q;I*jv,k,m,*v;
 h=FAV(self)->fgh[2]; hv=AAV(h); 
 q=hv[1]; jv=AV(q); m=AS(q)[0];
 k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v&&!(y==*v&&x==v[1])){v+=2; if(v==jv+2*m)v=jv;}  // search hash table, stop on match or end
 R AAV(hv[2])[((v-jv)>>1)];  // return match if found, 0 if not
}

// add new arg(s)/result pair which missed during lookup.  Args are I, result is A
static A jtmemoput(J jt,I x,I y,A self,A z){A*cv,h,*hv,q;I *jv,k,m,*mv,*v;
 RZ(z);
 h=FAV(self)->fgh[2]; hv=AAV(h);  // c = # fa()s needed to deallocate self, not counting the ones that just protect the name
 q=hv[0]; mv= AV(q);
 q=hv[1]; jv= AV(q);
 q=hv[2]; cv=AAV(q); m=AN(q);
 // If the buffer must be extended, allocate a new one
 if(m<=2**mv){A cc,*cu=cv,jj;I i,*ju=jv,n=m,*u;
  FULLHASHSIZE(2**mv,BOXSIZE,1,0,m);  // # boxes to allocate to get at least 2**mv slots
  RZ(jj=mkwris(reshape(v2(m,2L),sc(IMIN)))); ACINITZAP(jj) jv= AV(jj);  // init arg table to IMIN
  GATV0(cc,BOX,m,1); ACINITZAPRECUR(cc,BOX) cv=AAV(cc);  // the old table is recursive - the new one must be too
  for(i=0,u=ju;i<n;++i,u+=2){
   if(IMIN!=*u){  // copy the hash - does this lose the buffer for an arg of IMIN?
    // the current slot in the memo table is filled.  Rehash it, and move the args into *jv and the values into *cv
    k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v){v+=2; if(v==jv+2*m)v=jv;}
    cv[(v-jv)>>1]=cu[i]; v[0]=u[0]; v[1]=u[1];
   }
   cu[i]=0;  // always clear the pointer to the value so that we don't free the value when we free the old table
  }
  // Free the old buffers, ras() the new to make them recursive usect, then clear the tpops to bring the usecount down to 1
  // h has recursive usecount
  mf(hv[1]); hv[1]=jj;   // expunge old table, new one raised above, install.   h is not virtual
  mf(hv[2]); hv[2]=cc;   // not INSTALLBOX(h,hv,2,cc); because we DO NOT want to increment the counts in the values already in the table.  cc itself raised above
 }
 ++*mv;
 k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v){v+=2; if(v==jv+2*m)v=jv;}
 // bump the usecount of the result to account for new ref from table
 RZ(ras(z)); cv[(v-jv)>>1]=z; v[0]=y; v[1]=x; 
 R z;
}
#endif

// w is an arg; result is IMIN if not memoable
// memoable is: atomic int/bool or other with int value
static I jtint0(J jt,A w){A x;
 if(unlikely(AR(w)))R IMIN;
 if(unlikely(!(ISDENSETYPE(AT(w),B01+INT))))w=pcvt(INT,w);
 R w&&INT+B01&AT(w)?BIV0(w):IMIN;
}

static DF1(jtmemo1){DECLF;A z;I x,y;
 ARGCHK1(w);
 x=IMIN; y=int0(w);
 if(y==IMIN)R CALL1(f1,w,fs);  // if unmemoable, just run the function off-hash
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL1(f1,w,fs));
}

static DF2(jtmemo2){DECLF;A z;I x,y; 
 ARGCHK2(a,w);
 x=int0(a); y=int0(w);
 if(MIN(x,y)==IMIN)R CALL2(f2,a,w,fs);  // IMIN is unmemoable, run fn
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL2(f2,a,w,fs));  // if memo lookup returns empty, run the function and remember the result
}

// Create the memoed verb.  We create an h argument of hashtable;key;value, as described above
F1(jtmemo){PROLOG(300);A h,*hv;I m;
 ARGCHK1(w);
 ASSERT(VERB&AT(w),EVDOMAIN);
 V *v=FAV(w); FULLHASHSIZE(30,BOXSIZE,1,0,m);  // m = # items to allocate
 GAT0(h,BOX,3,0); hv=AAV0(h);  // the components of fdef must be recursive if recursible
 // the tables are standard extendible, with # items in AM, thus must be zapped
 // So, we defer initializing them until they have been made recursive inside fdef
 GAT0(hv[0],INT,m,0) GAT0(hv[1],INT,2*(m>>1),2) GAT0(hv[2],BOX,m>>1,0)  // allo hash/keys/results
// obsolete  GAT0(q,INT,1,0); AV(q)[0]=0;        hv[0]=incorp(q);  // is modified; musn't use sc()
// obsolete  RZ(q=reshape(v2(m,2L),sc(IMIN)));  RZ(hv[1]=incorp(mkwris(q)));
// obsolete  GATV0(q,BOX,m,1);                 hv[2]=incorp(q);
 A z=fdef(0,CMCAP,VERB,jtmemo1,jtmemo2,w,0L,h,0L,v->mr,lrv(v),rrv(v));
 AM(hv[0])=0; mvc(m*SZI,AAV0(hv[0]),1,MEMSETFF);  // clear hash table
 AM(hv[1])=0; AS(hv[1])[0]=m>>1;  // init empty key table, 2 INTs each row
 AM(hv[2])=0;   // init empty result table, recursive
 R z;
 // Now we have converted the verb result to recursive usecount, and gotten rid of the pending tpops for the components of h
}
