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
  R fdef(0,CTILDE,VERB,(AF)(swap1),(AF)(swap2),w,0L,0L,flag,(I)(RMAX),(I)(rr(w)),(I)(lr(w)));
 }else{
  // evoke.  Ii must be LIT and convertible to ASCII.
  if((C2T+C4T)&AT(w))RZ(w=cvt(LIT,w)) else ASSERT(LIT&AT(w),EVDOMAIN);
  ASSERT(1>=AR(w),EVRANK);  // list or atom only
  n=AN(w); s=CAV(w); 
  ASSERT(vnm(n,s),EVILNAME);   // valid name
  RZ(y=nfs(AN(w),CAV(w)));  // create a NAME block for the string
  R nameref(y,jt->locsyms);  // Create a name-reference pointing to the name
}}


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
 }else switch(j){
  case 32: R fdef(0,CBDOT,VERB, jtbitwise1,jtbitwiserotate, 0L,w,0L, VASGSAFE|VJTFLGOK2, 0L,0L,0L);
  case 33: R fdef(0,CBDOT,VERB, jtbitwise1,jtbitwiseshift, 0L,w,0L, VASGSAFE|VJTFLGOK2, 0L,0L,0L);
  case 34: R fdef(0,CBDOT,VERB, jtbitwise1,jtbitwiseshifta, 0L,w,0L, VASGSAFE|VJTFLGOK2, 0L,0L,0L);
  // The code uses a VERB with id CBDOT to stand for the derived verb of m b. .  This is used for spellout and for inverses, so we retain it.
  // We copy the other information from the verb that executes the function.  This contains pointers to the routines, and to the function table
  default: {A z=ca(ds(j-16+CBW0000)); RZ(z); RZ(FAV(z)->fgh[1]=rifvs(w)); FAV(z)->id=CBDOT; RETF(z);}  // use g field not f to avoid interfering with atomic2
 }
}


/* The h parameter in self for u M.                 */
/* 3 element boxed list                             */
/* 0 - integer atom of # of entries in hash table   */
/* 1 - 2-column integer table of arguments          */
/*     arguments are machine word integers          */
/*     column 1 is right arg; column 1 is left arg  */
/*     column 0 is IMIN for monad                   */
/* 2 - box list of results corresp. to arguments    */
/*     unused entries are set to 0                  */

#if SY_64
#define HIC(x,y)  ((UI)x+10495464745870458733U*(UI)y)
#else
#define HIC(x,y)  ((UI)x+2838338383U*(UI)y)
#endif

static A jtmemoget(J jt,I x,I y,A self){A h,*hv,q;I*jv,k,m,*v;
 h=FAV(self)->fgh[2]; hv=AAV(h); 
 q=hv[1]; jv=AV(q); m=*AS(q);
 k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v&&!(y==*v&&x==v[1])){v+=2; if(v==jv+2*m)v=jv;}  // search hash table, stop on match or end
 R AAV(hv[2])[((v-jv)>>1)];  // return match if found, 0 if not
}

static A jtmemoput(J jt,I x,I y,A self,A z){A*cv,h,*hv,q;I *jv,k,m,*mv,*v;
 RZ(z);
 h=FAV(self)->fgh[2]; hv=AAV(h);  // c = # fa()s needed to deallocate self, not counting the ones that just protect the name
 q=hv[0]; mv= AV(q);
 q=hv[1]; jv= AV(q);
 q=hv[2]; cv=AAV(q); m=AN(q);
 // If the buffer must be extended, allocate a new one
 if(m<=2**mv){A cc,*cu=cv,jj;I i,*ju=jv,n=m,*u;A* _ttop=jt->tnextpushp;
  FULLHASHSIZE(2**mv,BOXSIZE,1,0,m);  // # boxes to allocate to get at least 2**mv slots
  RZ(jj=mkwris(reshape(v2(m,2L),sc(IMIN)))); jv= AV(jj);  // init arg table to IMIN
  GATV0(cc,BOX,m,1);                  cv=AAV(cc);
  for(i=0,u=ju;i<n;++i,u+=2){if(IMIN!=*u){  // copy the hash - does this lose the buffer for an arg of IMIN?
   // the current slot in the memo table is filled.  Rehash it, and move the args into *jv and the values into *cv
   k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v){v+=2; if(v==jv+2*m)v=jv;}
   cv[(v-jv)>>1]=cu[i]; v[0]=u[0]; v[1]=u[1];
  }cu[i]=0;}  // always clear the pointer to the value so that we don't free the value when we free the old table
  // Free the old buffers, ras() the new to make them recursive usect, then clear the tpops to bring the usecount down to 1
  q=hv[1]; AC(q)=1; fa(q); INSTALLBOXNF(h,hv,1,jj);  // expunge old table, install new one.  Could use mf().  h is not virtual
  q=hv[2]; AC(q)=1; fa(q); ACINCR(cc); hv[2]=cc;   // not INSTALLBOX(h,hv,2,cc); because we DO NOT want to increment the count in the value.  But we do in the cc itself
  tpop(_ttop);  // get the new buffers off the tpush stack so we can safely free them in the lines above. (no longer needed)
 }
 ++*mv;
 k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v){v+=2; if(v==jv+2*m)v=jv;}
 // bump the usecount of the result to account for new ref from table
 RZ(ras(z)); cv[(v-jv)>>1]=z; v[0]=y; v[1]=x; 
 R z;
}

static I jtint0(J jt,A w){A x;
 if(AR(w))R IMIN;
 if(NUMERIC&AT(w))switch(UNSAFE(AT(w))){
  case B01: R (I)BAV(w)[0];
  case INT: R AV(w)[0];
 }
 x=pcvt(INT,w); 
 R x&&INT&AT(x)?AV(x)[0]:IMIN; 
}

static DF1(jtmemo1){DECLF;A z;I x,y;
 ARGCHK1(w);
 x=IMIN; y=int0(w);
 if(y==IMIN)R CALL1(f1,w,fs);
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL1(f1,w,fs));
}

static DF2(jtmemo2){DECLF;A z;I x,y; 
 ARGCHK2(a,w);
 x=int0(a); y=int0(w);
 if(MIN(x,y)==IMIN)R CALL2(f2,a,w,fs);  // IMIN is unmemoable, run fn
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL2(f2,a,w,fs));  // if memo lookup returns empty, run the function and remember the result
}

// Create the memoed verb.  We create an h argument that is a list of 3 boxes, containing:
// 0 number of memoed results m
// 1 mx2 INT table of input value; index of result
// 2 boxed list containing results
// All these start life on the tpush stack
F1(jtmemo){PROLOG(300);A h,*hv,q;I m;V*v;
 ARGCHK1(w);
 ASSERT(VERB&AT(w),EVDOMAIN);
 v=FAV(w); FULLHASHSIZE(30,BOXSIZE,1,0,m);  // m = # items to allocate
 GAT0(h,BOX,3,1); hv=AAV(h);
 GAT0(q,INT,1,0); AV(q)[0]=0;        hv[0]=q;  // is modified; musn't use sc()
 RZ(q=reshape(v2(m,2L),sc(IMIN)));  RZ(hv[1]=mkwris(q));
 GATV0(q,BOX,m,1);                 hv[2]=q;
 EPILOG(fdef(0,CMCAP,VERB,jtmemo1,jtmemo2,w,0L,h,0L,v->mr,lrv(v),rrv(v)));
 // Now we have converted the verb result to recursive usecount, and gotten rid of the pending tpops for the components of h
}
