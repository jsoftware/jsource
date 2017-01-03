/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs                                                                 */

#include "j.h"

// create inplace bits as copy of W, or swap A & W
static DF1(swap1){DECLF; F1PREFIP; R jt->rank?irs2(w,w,fs,jt->rank[1],jt->rank[1],f2):((jtinplace=(J)((I)jtinplace&JTINPLACEW)),(f2)((J)((I)jt|(((I)jtinplace<<1)+(I)jtinplace)),w,w,fs));}
static DF2(swap2){DECLF; F2PREFIP; R jt->rank?irs2(w,a,fs,jt->rank[1],jt->rank[0],f2):((jtinplace=(J)((I)jtinplace&(JTINPLACEW+JTINPLACEA))),(f2)((J)((I)jt|(((((I)jtinplace<<2)+(I)jtinplace)>>1)&(JTINPLACEW+JTINPLACEA))),w,a,fs));}

F1(jtswap){A y;C*s;I n;
 RZ(w); 
 if(VERB&AT(w)){I flag = VAV(w)->flag&(VIRS2|VINPLACEOK2); flag = (VAV(w)->flag&VASGSAFE)+flag+(flag>>1);  // set ASGSAFE, both inplace/irs bits from dyad; ISATOMIC immaterial, since always dyad
  R fdef(CTILDE,VERB,(AF)(swap1),(AF)(swap2),w,0L,0L,flag,(I)(RMAX),(I)(rr(w)),(I)(lr(w)));
 }else{
  if((C2T+C4T)&AT(w))RZ(w=cvt(LIT,w)) else ASSERT(LIT&AT(w),EVDOMAIN);
  ASSERT(1>=AR(w),EVRANK);
  n=AN(w); s=CAV(w); 
  ASSERT(vnm(n,s),EVILNAME); 
  RZ(y=nfs(AN(w),CAV(w)));
  R nameref(y);
}}


static B booltab[64]={
 0,0,0,0, 0,0,0,1, 0,0,1,0, 0,0,1,1,  0,1,0,0, 0,1,0,1, 0,1,1,0, 0,1,1,1,
 1,0,0,0, 1,0,0,1, 1,0,1,0, 1,0,1,1,  1,1,0,0, 1,1,0,1, 1,1,1,0, 1,1,1,1,
};

static DF2(jtbdot2){R from(plus(duble(cvt(B01,a)),cvt(B01,w)),VAV(self)->h);}

static DF1(jtbdot1){R bdot2(zero,w,self);}

static DF1(jtbasis1){DECLF;A z;D*x;I j;V*v;
 PREF1(jtbasis1);
 RZ(w=vi(w));
 switch(*AV(w)){
  case 0:
   GAT(z,FL,3,1,0); x=DAV(z); v=VAV(fs);
   j=v->mr; x[0]=j<=-RMAX?-inf:j>=RMAX?inf:j;
   j=v->lr; x[1]=j<=-RMAX?-inf:j>=RMAX?inf:j;
   j=v->rr; x[2]=j<=-RMAX?-inf:j>=RMAX?inf:j;
   R pcvt(INT,z);
  case -1: R lrep(inv (fs));
  case  1: R lrep(iden(fs));
  default: ASSERT(0,EVDOMAIN);
}}

F1(jtbdot){A b,h=0;I j,n,*v;
 RZ(w);
 if(VERB&AT(w))R ADERIV(CBDOT, jtbasis1,0L, 0L,0,0,0);
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 if(1==n){j=*v; ASSERT(-16<=j&&j<=34,EVINDEX);}
 else DO(n, j=*v++; ASSERT(-16<=j&&j<16,EVINDEX););
 if(1!=n||j<16){
  GAT(b,B01,64,2,0); *AS(b)=16; *(1+AS(b))=4; MC(AV(b),booltab,64L);
  RZ(h=cant2(IX(AR(w)),from(w,b)));
  R fdef(CBDOT,VERB, jtbdot1,jtbdot2, w,0L,h, VFLAGNONE, RMAX,0L,0L);
 }else switch(j){
  default: ASSERT(0,EVNONCE);
  case 16: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0000, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 17: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0001, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 18: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0010, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 19: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0011, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 20: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0100, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 21: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0101, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 22: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0110, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 23: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise0111, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 24: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1000, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 25: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1001, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 26: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1010, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 27: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1011, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 28: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1100, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 29: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1101, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 30: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1110, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 31: R fdef(CBDOT,VERB, jtbitwise1,jtbitwise1111, w,0L,0L, VASGSAFE|VIRS2|VINPLACEOK2, 0L,0L,0L);
  case 32: R fdef(CBDOT,VERB, jtbitwise1,jtbitwiserotate, w,0L,0L, VASGSAFE|VINPLACEOK2, 0L,0L,0L);
  case 33: R fdef(CBDOT,VERB, jtbitwise1,jtbitwiseshift, w,0L,0L, VASGSAFE|VINPLACEOK2, 0L,0L,0L);
  case 34: R fdef(CBDOT,VERB, jtbitwise1,jtbitwiseshifta, w,0L,0L, VASGSAFE|VINPLACEOK2, 0L,0L,0L);
}}


/* The h parameter in self for u M.                 */
/* 3 elememt boxed list                             */
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
 h=VAV(self)->h; hv=AAV(h); 
 q=hv[1]; jv=AV(q); m=*AS(q);
 k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v&&!(y==*v&&x==v[1])){v+=2; if(v==jv+2*m)v=jv;}
 R*(AAV(hv[2])+(v-jv)/2);
}

static A jtmemoput(J jt,I x,I y,A self,A z){A*cv,h,*hv,q;I c,*jv,k,m,*mv,*v;
 RZ(z);
 c=ACUC(self)-VAV(self)->execct; h=VAV(self)->h; hv=AAV(h);  // c = # fa()s needed to deallocate self, not counting the ones that just protect the name
 q=hv[0]; mv= AV(q);
 q=hv[1]; jv= AV(q);
 q=hv[2]; cv=AAV(q); m=AN(q);
 // If the buffer must be extended, allocate a new one
 if(m<=2**mv){A cc,*cu=cv,jj;I i,*ju=jv,n=m,*u;I _ttop=jt->tnextpushx;
  v=ptab+PTO; while(m>=*v)++v; m=*v;
  RZ(jj=reshape(v2(m,2L),sc(IMIN))); jv= AV(jj);
  GATV(cc,BOX,m,1,0);                  cv=AAV(cc);
  for(i=0,u=ju;i<n;++i,u+=2)if(IMIN!=*u){
   k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v){v+=2; if(v==jv+2*m)v=jv;}
   cv[(v-jv)/2]=cu[i]; cu[i]=0; v[0]=u[0]; v[1]=u[1];
  }
  // Free the old buffer.  Transfer the usecount from the old to the new, by adding the number of times the memoed verb will free this buffer
  q=hv[1]; AC(q)=1; fa(q); ACINCRBY(jj,c); hv[1]=jj;  // force fa(); transfer usecount to new buffer
  q=hv[2]; AC(q)=1; fa(q); ACINCRBY(cc,c); hv[2]=cc;
  tpop(_ttop);  // get the new buffers off the tpush stack so we can safely free them in the lines above.
   // We have to do this because we have no guarantee that our caller will do a tpop before calling us again, and we
   // might end up forcing AC(q) to 1 and freeing it while it has an outstanding free on the stack.
   // This decrements the usecount of cc so that it now equals the number of times self must be fa()d to deallocate it.
 }
 ++*mv;
 k=HIC(x,y)%m; v=jv+2*k; while(IMIN!=*v){v+=2; if(v==jv+2*m)v=jv;}
 // bump the usecount of the result by the number of times it will be freed by this memoed verb
 cv[(v-jv)/2]=raa(c,z); v[0]=y; v[1]=x; 
 R z;
}

static I jtint0(J jt,A w){A x;
 if(AR(w))R IMIN;
 if(NUMERIC&AT(w))switch(UNSAFE(AT(w))){
  case B01: R (I)*BAV(w);
  case INT: R *AV(w);
 }
 x=pcvt(INT,w); 
 R x&&INT&AT(x)?*AV(x):IMIN; 
}

static DF1(jtmemo1){DECLF;A z;I x,y;
 RZ(w);
 x=IMIN; y=int0(w);
 if(y==IMIN)R CALL1(f1,w,fs);
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL1(f1,w,fs));
}

static DF2(jtmemo2){DECLF;A z;I x,y; 
 RZ(a&&w);
 x=int0(a); y=int0(w);
 if(x==IMIN||y==IMIN)R CALL2(f2,a,w,fs);
 R (z=memoget(x,y,self))?z:memoput(x,y,self,CALL2(f2,a,w,fs));  // if memo lookup returns empty, run the function and remember the result
}

// Create the memoed verb.  We create an h argument that is a list of 3 boxes, containing:
// 0 number of memoed results m
// 1 mx2 INT table of input value; index of result
// 2 boxed list containing results
// All these start life on the tpush stack
F1(jtmemo){A h,*hv,q;I m;V*v;
 RZ(w);
 ASSERT(VERB&AT(w),EVDOMAIN);
 v=VAV(w); m=ptab[1+PTO];
 GAT(h,BOX,3,1,0); hv=AAV(h);
 GAT(q,INT,1,0,0); *AV(q)=0;        hv[0]=q;  // is modified; musn't use sc()
 RZ(q=reshape(v2(m,2L),sc(IMIN))); hv[1]=q;
 GATV(q,BOX,m,1,0);                  hv[2]=q;
 R fdef(CMCAP,VERB,jtmemo1,jtmemo2,w,0L,h,0L,v->mr,v->lr,v->rr);
}
