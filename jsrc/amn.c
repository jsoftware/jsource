/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: a ind}z where z is sparse and ind is <"1 integers              */

/*  amne    a is the sparse element                                        */
/*  amna    a is an arbitrary dense array                                  */
/*  amnsp   a an arbitrary sparse array                                    */

#include "j.h"


static I jtcsize(J jt,A z,A ind){B*b;I h,j,m,r,*s;P*zp;
 r=AR(z); s=AS(z); zp=PAV(z);
 RZ(b=bfi(r,SPA(zp,a),0));
 m=1; j=h=AS(ind)[AR(ind)-1]; DQ(r-h, if(b[j])m*=s[j]; ++j;);
 R m;
}    /* data cell size in z for each element of index array ind */

static B jtiaddr(J jt,A z,A ind,A*i1,A*i2){A a,ai,as,ii,jj,q,t,x,y;I c,d,e,h,i,*iv,*jv,m,n,p,*qv,*s,*u,*v,*yv;P*zp;
 zp=PAV(z); a=SPA(zp,a); x=SPA(zp,x); s=1+AS(x);   // a=sparse axes, x=sparse values, s=->shape of sparse ITEM 
 h=AS(ind)[AR(ind)-1];   // h=# axes indexed 
 RZ(q=gt(sc(h),a));   // q=({:$ind) > a, i. e. mask of axes that are indexed by ind
 y=SPA(zp,i); if(!all1(q))RZ(y=repeatr(q,y));  // y=index table for z, but only the columns indexed by ind
 m=AS(y)[0]; yv=AV(y);  // m=#sparse items, yv->first sparse-index list (only columns indexed by ind)
 RZ(ai=IX(h));   // ai=i. #indexed sparse axes
 RZ(as=less(IX(AR(z)),a)); u=AV(as); n=AN(as);  // as=nonsparse axes of x, n=number of them, u->first
 GATV0(t,INT,n,1); v=AV1(t);    // t=place to build lengths of indexed nonsparse zxes
 e=0; d=1; DO(n, if(h>u[i])v[e++]=s[i]; else d*=s[i];);  // move the lengths of the indexed nonsparse axes to t; accumulate d=cell-size of nonindexed axes; e=#indexed nonsparse axes
 RZ(*i2=jj=tymes(sc(d),base2(vec(INT,e,v),repeatr(eps(ai,as),ind))));  // jj= (len of nonindexed part) * (shape of indexed axes) #: (ai e. as) #"_ 1 ind  i. e. offset to the start of the cell in the data area.
     // note that this cell is noncontiguous: parts of it may be indexed by ind.  We are at the place where all nonindexed axes are 0
 c=AS(y)[1];  // c=#sparse axes indexed by ind
 if(!c){   // if no sparse axes indexed by ind...
  n=AN(jj);  // n=number of rows of ind
  RZ(*i1=repeat(sc(n),IX(m)));  // i1 = (#ind) # i. #sparse items, i. e. repeat each row of ind withing each sparse cell.  ind is applied within each sparse cell
  RZ(*i2=reshape(sc(m*n),jj));  // i2 = the offsets to the data area are repeated, one for each row of ind
  R 1;
 }
 RZ(*i1=ii=indexof(y,repeatr(eps(ai,a),ind)));   // ii=y i. (sparse axes of ind), i. e. the cell-indexes of z that are indexed by ind
 if(c==AN(a))R 1;  // if all sparse axes are indexed by ind, finish, leave i1 and i2 as is
 n=AN(ii); iv=AV(ii); jv=AV(jj);   // n=#indexed cells in y  iv->index-list of first cell  jv->data offset for each row of ind
 GATV0(q,INT,n,1); qv=AV1(q);    // q=work area, qv->location 0
 for(i=h=0;i<n;++i){  // for each indexed cell in y...
  e=1; d=iv[i]; v=yv+c*d;  // d=index in y of row i of ind; v->index list in y
  DQ(m-d-1, if(ICMP(v,v+c,c))break; ++e; v+=c;);   // count e=consecutive # identical rows of y (#rows of y touched by a single row of ind) 
  qv[i]=e; h+=e;  // for each row of ind, qv[i] is # rows of y that ind covers; h is the total#
 }
 GATV0(t,INT,h,1); u=AV1(t); *i1=t;  // allocate result areas
 GATV0(t,INT,h,1); v=AV1(t); *i2=t;
 DO(n, e=qv[i]; d=iv[i]; p=jv[i]; DQ(e, *u++=d++; *v++=p;););  // for each row of ind, see how many rows of y it touches; copy out the index lists; repeat the start of the data cell
 R 1;
}    /* index i1 (in index matrix) and address i2 (in data array) from index array */

// ind is array of indexes to cells
// We fill out the index-list (i) in z so that it has a list for each cell that will be modified by the indexes in ind.
// This involves finding each combination of sparse-axis value in ind, and expanding each to the full cell by appending each combination of the
// sparse axes that are NOT in ind.  Then, discard any cell-lists that are already in ind.  For each added cell, add an empty cell of sparse value (e) to the value (x) of z.
static A jtzpadn(J jt,A z,A ind,B ip){A a,ai,i1,p,p1,q,t,x,x0,y,y0,y1;B*b;I c,d,h,m,n;P*zp;
 // put sparse axi into names a,x,y; remember x&y as x0,y0
 zp=PAV(z); a=SPA(zp,a); x=x0=SPA(zp,x); y=y0=SPA(zp,i);  // a=list of sparse axes
 n=1; h=AS(ind)[AR(ind)-1];  // h=length of last axis of ind = len of frame of addressed cells
 RZ(ai=IX(h));   // ai = i. {:$ind = axis #s in frame
 RZ(t=eps(ai,a)); b=BAV(t); d=0; DO(h, if(b[i])++d;);  // t=mask of sparse axes in a that are in frame; d is # of them
 // create table of the sparse axes of ind.  If there are no sparse axes of ind, we use an empty table
 c=AN(a)-d;   // c=#sparse axes not in the frame, i. e. # sparse axes in the cells.
 RZ(i1=d<h?repeatr(t,ind):ind); if(2!=AR(ind))RZ(i1=d?reshape(v2(AN(i1)/d,d),i1):mtm);  // i1 = t#"_ 1 ind, i. e. ind containing only the sparse axes; form into table
 RZ(t=gt(sc(h),a)); RZ(y1=all1(t)?y:repeatr(t,y));  // y1=table of sparse indexes discarding any axes not in ind
 RZ(p=nub(less(i1,y1)));  // p=cells indexed by ind that are not already in z
 if(c){   // If there are sparse axes in the cells...
  // first, the cells in ind that are entirely missing from y.  We have to fill out the sparse axes not in ind
  // Also, if ind contains only nonsparse axes, we have to add a dummy empty row to ind so that all the sparse cells will be created
  I fullneeded=d<h&&AN(p)==0; A p2=p; if(fullneeded)RZ(p2=over(p,zeroionei(0)));  // p2 =. i. 1 0 if dummy needed
  RZ(t=from(less(a,ai),shape(z))); RZ(p1=odom(2L,c,AV(t))); n=AS(p1)[0];  // t=lengths of the axes in the cells; p1=odometer for them; n=total # cells in a modified item
  if(m=AS(p2)[0])RZ(p=stitch(repeat(sc(n),p2),reshape(v2(n*m,c),p1)));  // m=#new cells; if there are any, create p = (n # p) ,. ((n*#p) $ p1), i. e. extend p with indexes of sparse axes inside the cells
  // next, the cells of ind that are already in y, but perhaps partially
  RZ(t=nub(repeat(eps(y1,i1),y1)));   // t= ~. (y1 e. i1) # y1, i. e. rows of y1 that are already in ind
  RZ(t=stitch(repeat(sc(n),t),reshape(v2(n*AS(t)[0],c),p1)));  // t = (n # t) ,. (n*#t) $ p1, i. e. extend t with indexes of sparse axes in the cells
  if(AN(t)){RZ(p=over(p,t)); if(fullneeded)RZ(p=nub(p));} // join the indexes for the full and partially-full new atoms; if we added an empty cell, there may be overlap: remove it
  RZ(p=less(p,y));  // t = t -. y, i. e. discard any cells already in z
 }
 if(m=AS(p)[0]){  /* new cells being added */
  RZ(y=over(y,p)); RZ(q=grade1(y)); RZ(y=from(q,y));  // y =. y {~ q =. /: y =. y,p  i. e. sort y into cell order, with q the grade vector
  RZ(t=shape(x)); RZ(mkwris(t)) *AV(t)=m; RZ(x=from(q,over(x,reshape(t,SPA(zp,e)))));   // x =. q { x,sparse ele $~ t =. (#p) 0} $x (x=sparse values)  i. e. x has sparse values insered in new atoms
  // if z is assigned to a name, the use counts need to be adjusted: the old ones need to be decremented
  // to remove the assignment, and the new ones need to be incremented to prevent them from being freed
  // until the name is freed.  We detect the case from jt->zombieval being set to the address of z
  // (if the block could not be inplaced, z will have been changed)
  if(jt->zombieval==z){RZ(ras(y)); RZ(ras(x)); fa(y0); fa(x0);}
  SPB(zp,i,y); SPB(zp,x,x);  // install the new indexes and values
 }
 R z;
}    /* create new indexed cells in z as necessary */

static A jtastdn(J jt,A a,A z,A ind){A a1,q,r,s;B*b;I ar,*as,*av,d,ir,n,n1,*v,zr,*zs;P*zp;
 ar=AR(a); as=AS(a);
 zr=AR(z); zs=AS(z); 
 if(!ar)R a;
 ir=AR(ind); n=AS(ind)[ir-1]; d=(ir-1)+(zr-n); ASSERT(ar<=d,EVRANK);  // n=shape of item of i; d is # unindexed axes
 GATV0(s,INT,d,1); v=AV1(s); MCISH(v,AS(ind),ir-1); MCISH(v+ir-1,zs+n,zr-n);
 ASSERTAGREE(as,AV(s)+d-ar,ar);
 if(ar<d)RZ(a=reshape(s,a));
 zp=PAV(z); a1=SPA(zp,a); av=AV(a1); n1=n-1;
 GATV0(s,B01,zr,1); b=BAV1(s); 
 mvc(zr,b,MEMSET00LEN,MEMSET00); DO(AN(a1), b[av[i]]=1;); mvc(n,b,1,iotavec-IOTAVECBEGIN+(!memchr(b,C1,n)?C0:C1)); 

 GATV0(r,INT,zr-n1,1); v=AV1(r); *v++=ar-(zr-n); DQ(zr-n, *v++=1;);
 RZ(q=dgrade1(repeat(r,vec(B01,zr-n1,b+n1))));
 R equ(q,IX(ar))?a:cant2(q,a);
}    /* convert replacement array a into standard form relative to index array ind */

A jtamne(J jt,A a,A z,A ind,B ip){A i1,i2,x,y;C*u,*v;I*iv,*jv,k,m,n,vk,xk;P*zp;
 RZ(a&&z&&ind);
 RZ(iaddr(z,ind,&i1,&i2));
 zp=PAV(z); x=SPA(zp,x); y=SPA(zp,i);
 m=AS(y)[0]; n=AN(i1);
 k=bpnoun(AT(x)); xk=k*aii(x); vk=k*csize(z,ind);
 u=CAV(a); v=CAV(x); iv=AV(i1); jv=AV(i2);
 DO(n, if(m>iv[i])mvc(vk,v+xk*iv[i]+k*jv[i],k,u););
 R z;
}    /* a (<"1 ind)}z, sparse z, integer array ind, sparse element a replacement */

A jtamna(J jt,A a,A z,A ind,B ip){A i1,i2,x;C*u,*v;I*iv,*jv,k,n,vk,xk;P*zp;
 RZ(a&&z&&ind);
 RZ(z=zpadn(z,ind,ip));
 RZ(a=astdn(a,z,ind));
 RZ(iaddr(z,ind,&i1,&i2));
 zp=PAV(z); x=SPA(zp,x); n=AN(i1);
 k=bpnoun(AT(x)); xk=k*aii(x); vk=k*csize(z,ind);
 u=CAV(a); v=CAV(x); iv=AV(i1); jv=AV(i2); 
 if(AR(a))DO(n, mvc(vk,v+xk*iv[i]+k*jv[i],vk,u); u+=vk;)
 else     DO(n, mvc(vk,v+xk*iv[i]+k*jv[i],k,u););
 R z;
}    /* a (<"1 ind)}z, dense a,integer array ind, sparse z */

A jtamnsp(J jt,A a,A z,A ind,B ip){A i1,i2,t;C*ev,*u,*v,*vv;I c,*dv,i,*iv,j,*jv,k,m,n,p,q,r,*s,*yv,zk;P*ap,*zp;
 RZ(a&&z&&ind);
 r=AR(a); ap=PAV(a); t=SPA(ap,a); if(r>AN(t))RZ(a=reaxis(IX(r),a));
 RZ(a=astdn(a,z,ind));  ap=PAV(a);
 RZ(z=zpadn(z,ind,ip)); zp=PAV(z);
 RZ(iaddr(z,ind,&i1,&i2));
 s=AS(a); n=AN(i1); c=csize(z,ind); iv=AV(i1); jv=AV(i2);
 t=SPA(ap,i); yv= AV(t); m=AS(t)[0]; p=0;
 t=SPA(ap,e); ev=CAV(t);
 t=SPA(ap,x); u =CAV(t);
 t=SPA(zp,x); v =CAV(t); k=bpnoun(AT(t)); zk=k*aii(t);
 GATV0(t,INT,r,1); dv=AV1(t); mvc(SZI*r,dv,MEMSET00LEN,MEMSET00); dv[r-1]=-1;
 for(i=0;i<n;++i){
  vv=v+zk*iv[i]+k*jv[i];
  for(j=0;j<c;++j){
   q=r; DQ(r, --q; ++dv[q]; if(dv[q]<s[q])break; dv[q]=0;);
   q=1; while(p<m){DO(r, if(q=yv[i]-dv[i])break;); if(0<=q)break; ++p; yv+=r;}
   MC(vv,q?ev:u+k*p,k); 
   vv+=k;
 }}
 R z;
}    /* a (<"1 ind)}z, sparse a, integer array ind, sparse z */
