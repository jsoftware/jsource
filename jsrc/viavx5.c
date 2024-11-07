/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"
#if C_CRC32C && SY_64
#include "viavx.h"

// ***************** i., fifth class: boxed arguments ************************

// b means self-index and running i. i: ~. ~: (I.@~.)
// bk means i: (e. i: 0:) (e. i: 1:)   or prehashed version thereof, i. e. backwards
// We grade a, producing the ordering permutation.  Then we go through it to discard duplicates
static A jtnodupgrade(J jt,A a,I acr,I ac,I acn,I ad,I n,I asct,I md,I bk){A*av,h,*u,*v;I*hi,*hu,*hv,l,m1,q;
 RZ(IRS1(a,0L,acr,jtgrade1,h)); hv=AV(h)+bk*(asct-1); av=AAV(a);
 // if not self-index, close up the duplicates
 if(!(md&IIMODREFLEX))for(l=0;l<ac;++l,av+=acn,hv+=asct){  // for each item of the overall result
  // hi->next index in the grade result, q is its value, u->A block for that index
  // hu->next output position.  The first result always stays in place
  hi=hv; q=*hi; u=av+n*q;
  // loop through, setting q/v to the index/address of data.  If *v!=*u, accept q/v as a new value and set u=v
  // don't bother with testing equality if q<index of u (for ascending; reverse for descending)
  // if the list was shortened, replace the last position with -(length of shortened list).  This will be detected
  // and complemented to give (length of list)-1.  0 is OK too, indicating a 1-element list
  if(bk){hu=--hi; DQ(asct-1, q=*hi--; v=av+n*q; if((u<v)||!eqa(n,u,v)){u=v; *hu--=q;}); m1=hv-hu; if(asct>m1)hv[1-asct]=-m1;}
  else  {hu=++hi; DQ(asct-1, q=*hi++; v=av+n*q; if((v<u)||!eqa(n,u,v)){u=v; *hu++=q;}); m1=hu-hv; if(asct>m1)hv[asct-1]=-m1;}
 }
 R h;
} 

// hiinc will inc or dec hi
// zstmti initializes the first result index with the index of the smallest element
// zstmt1 is executed if v is a duplicate, and should repeat the previous result
// zstmt0 is executed if v is not a dup, and must advance the p marker to q as well as store the new result
// don't bother testing for equality if ascending order and q<p (since grade would preserve order if equal), or if descending and q>p
#define BSLOOPAA(hiinc,zstmti,zstmt1,zstmt0)  \
 {A* RESTRICT u=av,* RESTRICT v;I* RESTRICT hi=hv,p,q;             \
  p=*hi; hi+=(hiinc); u=av+n*p; zstmti;  /* u->first result value, install result for that value to index itself */      \
  DQ(asct-1, q=*hi; hi+=(hiinc); v=av+n*q; if(((hiinc>0&&v>u)||(hiinc<0&&v<u))&&eqa(n,u,v)){zstmt1;} else{u=v; zstmt0;}); /* 
   q is input element# that will have result index i, v->it; if *u=*v, v is a duplicate: map the result back to u (index=p)
   if *u!=*p, advance u/p to v/q and use q as the result index */ \
 }

// binary search through the list hu[]
// m1 is the number of items-1(= index of last item)
// p/q are left/right indexes for the binary search
// ii is the item number we start working on
// i is the item number of w we are working on
// icmp is the loop exit condition, finding the last i (depends on direction - I don't see why)
// iinc advances to the next item of w (depends on direction - I don't see why)
// uinc advances the pointer to the next item of w (depends on direction - I don't see why)
// zstmt creates the result when a match has been found.  At that point q=-2 if there was no match, otherwise
//  it holds the index of the match
#define BSLOOPAWX(ii,icmp,iinc,uinc,zstmt)  \
 {A* RESTRICT u=wv+n*(ii),* RESTRICT v;I i,j,p,q;I t;  \
  for(i=ii;icmp;iinc,uinc){          \
   p=0; q=m1;                        \
   NOUNROLL while(p<=q){                      \
    t=0; j=(p+q)>>1; v=av+n*hu[j];    \
    DO(n, if(t=compare(C(u[i]),C(v[i])))break;);  \
    if(0<t)p=j+1; else q=t?j-1:-2;   \
   }                                 \
   zstmt;                            \
  }  \
 }

// macros to create ascending or descending binary search, executing zstmt afterwards
#define BSLOOPAW(zstmt)     BSLOOPAWX(0  ,i< wsct,++i,u+=n,zstmt)
#define BSLOOQAW(zstmt)     BSLOOPAWX(wsct-1,i>=0,--i,u-=n,zstmt)

// index by sorting a into order, then doing binary search on each item of w.
// Used only when ct=0 and (boxed rank>1 or boxes contain numeric arrays)

// This function needs acr for the sort.  We pass it in throuigh AM(h), since h is otherwise unused (except to hold the return value from IFORKEY)
// (if prehashing, we pass in acr only for the prehash, and h thereafter)
IOF(jtiobs){A*av,*wv,y;B *yb,*zb;C*zc;I acn,*hu,*hv,l,m1,md,s,wcn,*zi,*zv;
 md=mode&IIOPMSK;  // just the operation bits
 I bk=1&(((1<<IICO)|(1<<IJ0EPS)|(1<<IJ1EPS))>>md);  // set if the dup-scan is reverse direction
 if(mode==INUB||mode==INUBI){GATV0(y,B01,asct,1); yb=BAV1(y);}
 av=AAV(a);  acn=ak>>LGSZI;
 wv=AAV(w);  wcn=wk>>LGSZI;
 zi=zv=AV(z); zb=(B*)zv; zc=(C*)zv;
 // If a has not been sorted already, sort it
 if(!(mode&IPHOFFSET)){  // if we are not using a presorted table...
  // look for IIDOT/IICO/INUBSV/INUB/INUBI/IFORKEY - we set IIMODREFLEX if one of those is set.  They don't remove dups.
  // we don't set REFLEX if there is a prehash, because the prehash always removes dups, and we would be left missing some values
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md+=IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md);
  RZ(h=nodupgrade(a,(I)h,ac,acn,0,n,asct,md,bk));   // h is used to pass in acr
 }
 if(w==mark)R h;
 hv=AV(h)+bk*(asct-1);
 for(l=0;l<ac;++l,av+=acn,wv+=wcn,hv+=asct){  // loop for each result in a
  // m1=index of last item-1, which may be less than m-1 if there were discarded duplicates (signaled by last index <0)
  s=hv[bk?1-asct:asct-1]; m1=0>s?~s:asct-1; hu=hv-m1*bk;
  switch(md){
  // self-indexes
  case IIDOT|IIMODREFLEX:        BSLOOPAA(1,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=asct;     break;
  case IFORKEY|IIMODREFLEX: {I nuniq=0;BSLOOPAA(1,++nuniq;zv[p]=p+1,zv[q]=p;zv[p]++,++nuniq;zv[q]=(p=q)+1); zv+=asct; AM(h)=nuniq;     break;}
  case IICO|IIMODREFLEX:         BSLOOPAA(-1,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=asct;     break;
  case INUBSV|IIMODREFLEX:       BSLOOPAA(1,zb[p]=1,zb[q]=0,zb[q]=1  ); zb+=asct;     break;
  case INUB|IIMODREFLEX:         BSLOOPAA(1,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(asct, if(yb[i]){MC(zc,av+i*n,k); zc+=k;}); ZCSHAPE; break;
  case INUBI|IIMODREFLEX:        BSLOOPAA(1,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(asct, if(yb[i])*zi++=i;);                  ZISHAPE; break;
  // searches, by binary search
  case IIDOT:        BSLOOPAW(*zv++=-2==q?hu[j]:asct);                       break;
  case IICO:         BSLOOPAW(*zv++=-2==q?hu[j]:asct);                       break;
  case IEPS:         BSLOOPAW(*zb++=-2==q);                               break;
  case ILESS:        BSLOOPAW(if(-2< q){MC(zc,u,k); zc+=k;}); ZCSHAPE;    break;
  case IINTER:       BSLOOPAW(if(!(-2< q)){MC(zc,u,k); zc+=k;}); ZCSHAPE;    break;
  case II0EPS:  s=asct; BSLOOPAW(if(-2< q){s=i; break;});        *zi++=s;    break;
  case IJ0EPS:  s=asct; BSLOOQAW(if(-2< q){s=i; break;});        *zi++=s;    break;
  case II1EPS:  s=asct; BSLOOPAW(if(-2==q){s=i; break;});        *zi++=s;    break;
  case IJ1EPS:  s=asct; BSLOOQAW(if(-2==q){s=i; break;});        *zi++=s;    break;
  case IANYEPS: s=0; BSLOOPAW(if(-2==q){s=1; break;});        *zb++=(B)s; break;
  case IALLEPS: s=1; BSLOOPAW(if(-2< q){s=0; break;});        *zb++=(B)s; break;
  case ISUMEPS: s=0; BSLOOPAW(if(-2==q)++s);                  *zi++=s;    break;
  case IIFBEPS:      BSLOOPAW(if(-2==q)*zi++=i);              ZISHAPE;    break;
  }
 }
 R h;
}    /* a i.!.0 w on boxed a,w by grading and binary search */
#endif
