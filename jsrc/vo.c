/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Box & Open                                                       */

#include "j.h"


I level(A w){A*wv;I d,j,wd;
 if(!(AN(w)&&AT(w)&BOX+SBOX))R 0;
 d=0; wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(AN(w), j=level(WVR(i)); if(d<j)d=j;);
 R 1+d;
}

F1(jtlevel1){RZ(w); R sc(level(w));}

static I v00[] = {0, 0}; // used for rank
F1(jtbox0){
 // Process through box code, which already handles rank
 I *ranksave = jt->rank;
 if(AR(w))jt->rank=v00;
 A z=box(w);
 jt->rank=ranksave;
 R z;
}

F1(jtbox){A y,z,*zv;C*wv;I f,k,m,n,r,wr,*ws; 
 RZ(w); I wt=AT(w); FLAGT waf=AFLAG(w);
 ASSERT(!(SPARSE&wt),EVNONCE);
  // Set NOSMREL if w is not boxed or it has NOSMREL set
 FLAGT newflags = (waf | ((~wt)>>(BOXX-AFNOSMRELX))) & AFNOSMREL;
 if(!jt->rank){
  // single box: fast path.  Allocate a scalar box and point it to w.  Mark w as incorporated
  // DO NOT take potentially expensive pass through w to find recursibility, because it may never be needed if this result expires without being assigned
  GAT(z,BOX,1,0,0); INCORP(w); *(AAV(z))=w;
  if((waf&RECURSIBLE)||(wt&DIRECT)){newflags|=BOX; ACINCR(w);}  // if w is recursible or direct, mark new box recursible and correspondingly incr usecount of w.  We do this because w is already in cache now.
  AFLAG(z) = newflags;  // set NOSMREL if w is not boxed, or known to contain no relatives
 } else {
  // <"r
  ws=AS(w); wr=AR(w); r=jt->rank[1]; f=wr-r; I t=AT(w);
  CPROD(AN(w),n,f,ws); CPROD(AN(w),m,r,f+ws);
  k=m*bp(t); wv=CAV(w);
  GATV(z,BOX,n,f,ws); zv=AAV(z); 
  if(ARELATIVE(w)){GA(y,t,m,r,f+ws); A*v=(A*)wv; A1*u=(A1*)CAV(y); DO(n, DO(m, u[i]=AABS(*v++,w);); RZ(zv[i]=ca(y)););}  // relatives through a vanilla path: make absolute in the temp y; clone y; incorporate that into result
  else{
   // The case of interest: non-relative w.  We have allocated the result; now we allocate a block for each cell of w and copy
   // the w values to the new block.  We set NOSMREL in the top block and the new ones if w is boxed or NOSMREL.  
   // If w is DIRECT, we make the result block recursive and increment the count of the others (we do so here because it saves a traversal of the new blocks
   // if the result block is assigned).  If w is indirect, we just leave everything nonrecursive to avoid traversing w, because
   // that will prove unnecessary if this result is not assigned.

   if(t&DIRECT){
    // Direct w.
    AFLAG(z) = newflags|BOX;  // Make result inplaceable and recursive
    // Since we are making the result recursive, we can save a lot of overhead by NOT putting the cells onto the tstack.  As we have marked the result as
    // recursive, it will free up the cells when it is deleted.  We want to end up with the usecount in the cells being 1, not inplaceable.  The result itself will be
    // inplaceable with a free on the tstack.
    // To avoid the tstack overhead, we hijack the tstack pointers and set them up to have no effect on the actual tstack.  We restore them when we're
    // finished.  If we hit an error, that's OK, because whatever we did get allocated will be freed when the result block is freed.  We use GAE so that we don't abort on error
    I pushxsave = jt->tnextpushx; A *tstacksave = jt->tstack;  // save tstack info before allocation
    jt->tstack = (A*)(&jt->tnextpushx);  // use tnextpushx as a temp.  It will be destroyed, which is OK because we are setting it to 0 before every call
    DO(n, jt->tnextpushx=0; GAE(y,t,m,r,f+ws,break); AFLAG(y)=newflags; AC(y)=ACUC1; MC(CAV(y),wv,k); wv+=k; zv[i]=y;);   // allocate, but don't grow the tstack.  Set usecount of cell to 1
    jt->tstack=tstacksave; jt->tnextpushx=pushxsave;   // restore tstack pointers
   }else{AFLAG(z) = newflags; DO(n, GA(y,t,m,r,f+ws); AFLAG(y)=newflags; MC(CAV(y),wv,k); wv+=k; zv[i]=y;); } // boxed w; don't set recursible, but inherit nosm from w
  }
 }
 R z;
}    /* <"r w */

F1(jtboxopen){RZ(w); if(!(AN(w)&&BOX&AT(w))){w = box(w);} R w;}

F2(jtlink){RZ(a&&w); if(!(AN(w)&&AT(w)&BOX)){w = box(w);} R over(box(a),w);}

static B povtake(J jt,A a,A w,C*x){B b;C*v;I d,i,j,k,m,n,p,q,r,*s,*ss,*u,*uu,y;
 if(!w)R 0;
 r=AR(w); n=AN(w); k=bp(AT(w)); v=CAV(w);
 if(1>=r){MC(x,v,k*n); R 1;}
 m=AN(a); u=AV(a); s=AS(w);
 p=0; d=1; DO(r, if(u[m-1-i]==s[r-1-i]){d*=s[r-1-i]; ++p;}else break;);
 b=0; DO(r-p, if(b=1<s[i])break;);
 if(!b){MC(x,v,k*n); R 1;}
 k*=d; n/=d; ss=s+r-p; uu=u+m-p;
 for(i=0;i<n;++i){
  y=0; d=1; q=i; /* y=.a#.((-$a){.(($a)$1),$w)#:i */
  s=ss; u=uu; DO(r-p, j=*--s; y+=q%j*d; d*=*--u; q/=j;);
  MC(x+y*k,v,k); v+=k;
 }
 R 1;
}

static B jtopes1(J jt,B**zb,A*za,A*ze,I*zm,A cs,A w){A a,e=0,q,*wv,x;B*b;I i,k,m=0,n,*v,wcr;P*p;
 n=AN(w); wcr=AN(cs); wv=AAV(w);
 GATV(x,B01,wcr,1,0); b=BAV(x); memset(b,C0,wcr);
 for(i=0;i<n;++i)
  if(q=wv[i],SPARSE&AT(q)){
   p=PAV(q); x=SPA(p,x); m+=*AS(x);
   if(!e)e=SPA(p,e); else ASSERT(equ(e,SPA(p,e)),EVSPARSE);
   k=wcr-AR(q); DO(k, b[i]=1;); a=SPA(p,a); v=AV(a); DO(AN(a), b[k+*v++]=1;);
  }
 RZ(*za=ifb(wcr,b));    /* union of sparse axes           */
 *zb=b;                 /* mask corresp. to sparse axes   */
 *ze=e?e:zero;          /* sparse element                 */
 *zm=m;                 /* estimate # of non-sparse cells */
 R 1;
}

static B jtopes2(J jt,A*zx,A*zy,B*b,A a,A e,A q,I wcr){A x;B*c;I dt,k,r,*s,t;P*p;
 dt=AT(e); r=AR(q); k=wcr-r; t=AT(q);
 if(t&SPARSE){
  p=PAV(q);
  RZ(c=bfi(r,SPA(p,a),1));
  DO(r, if(b[k+i]!=c[i]){RZ(q=reaxis(ifb(r,k+b),q)); break;});
 }else{
  if(k){
   GA(x,t,AN(q),wcr,0); s=AS(x); DO(k, *s++=1;); ICPY(s,AS(q),r); 
   MC(AV(x),AV(q),AN(q)*bp(t)); q=x;
  }
  RZ(q=sparseit(t&dt?q:cvt(dt,q),a,e));
 }
 p=PAV(q);
 x=SPA(p,x); if(!(dt&AT(x)))RZ(x=cvt(dt,x));
 *zx=x;         /* data cells              */
 *zy=SPA(p,i);  /* corresp. index matrix   */
 R 1;
}

static A jtopes(J jt,I zt,A cs,A w){A a,d,e,sh,t,*wv,x,x1,y,y1,z;B*b;C*xv;I an,*av,c,dk,dt,*dv,i,j,k,m,m1,n,
     p,*s,*v,wcr,wr,xc,xk,yc,*yv,*zs;P*zp;
 n=AN(w); wr=AR(w); wv=AAV(w); wcr=AN(cs); dt=DTYPE(zt); dk=bp(dt);
 RZ(opes1(&b,&a,&e,&m,cs,w)); an=AN(a); av=AV(a);
 GA(z,zt,1L,wr+wcr,0); zs=AS(z); ICPY(zs,AS(w),wr); ICPY(zs+wr,AV(cs),wcr);
 zp=PAV(z); c=wcr-an; yc=wr+an;
 SPB(zp,e,e=cvt(dt,e));
 GATV(t,INT,yc, 1L,0L); v=AV(t); DO(wr, v[i]=i;); DO(an, v[wr+i]=wr+av[i];); SPB(zp,a,t);
 GATV(sh,INT,1+c,1L,0L); s=AV(sh); s[0]=m; j=1; DO(wcr, if(!b[i])s[j++]=zs[wr+i];); 
 RE(xc=prod(c,1+s)); xk=xc*dk;
 GATV(d,INT,wr,1,0); dv=AV(d); memset(dv,C0,wr*SZI);
 RE(i=mult(m,xc)); GA(x,dt, i,1+c,s); xv=CAV(x); mvc(m*xk,xv,dk,AV(e));
 RE(i=mult(m,yc)); GATV(y,INT,i,2L, 0L); v=AS(y); *v=m; v[1]=yc; yv=AV(y); memset(yv,C0,SZI*i);
 for(i=p=0;i<n;++i){
  RZ(opes2(&x1,&y1,b,a,e,wv[i],wcr)); v=AS(y1); m1=v[0]; k=v[1];
  if(m<p+m1){
   j=m; m=(i<n-1?m+m:0)+p+m1;
   RZ(x=take(sc(m),x)); xv=CAV(x)+p*xk; mvc(xk*(m-j),xv,dk,AV(e));
   RZ(y=take(sc(m),y)); yv= AV(y)+p*yc;
  }
  for(j=wr-1;j;--j)if(dv[j]==zs[j]){dv[j]=0; ++dv[j-1];}else break;
  v=AV(y1); DO(m1, ICPY(yv,dv,wr); ICPY(yv+yc-k,v,k); yv+=yc; v+=k;); 
  if(memcmp(1+AS(x1),1+s,SZI*c)){*s=m1; povtake(jt,sh,x1,xv);} else MC(xv,AV(x1),m1*xk);
  ++dv[wr-1]; xv+=m1*xk; p+=m1;
 }
 SPB(zp,x,p==m?x:take(sc(p),x));
 SPB(zp,i,p==m?y:take(sc(p),y));
 R z;
}

// > y (rank is immaterial)
// If y cannot be inplaced, we have to make sure we don't return an inplaceable reference to a part of y.  This would happen
// if y contained inplaceable components (possible if y came from < yy or <"r yy).  In that case, mark the result as non-inplaceable.
// We don't support inplacing here yet so just do that always
F1(jtope){PROLOG(0080);A cs,*v,y,z;B b,c,h=1;C*x;I d,i,k,m,n,*p,q=RMAX,r=0,*s,t=0,*u,zn;
 RZ(w);
 n=AN(w); v=AAV(w); b=ARELATIVE(w);  // b=1 if w is relative
 if(!(n&&BOX&AT(w)))RCA(w); /* obsolete {GATV(z,B01,0L,1+AR(w),AS(w)); *(AR(w)+AS(w))=0; R z;} */
 if(!AR(w)){z=b?(A)AABS(*v,w):*v; ACIPNO(z); R z;}   // turn off inplacing if we are using the contents directly
 // set q=min rank of contents, r=max rank of contents
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i]; 
  q=MIN(q,AR(y)); 
  r=MAX(r,AR(y));
  // for nonempty contents, check for conformability and save highest-priority type
  if(AN(y)){
   k=AT(y); t=t?t:k; m=t|k;
   if(TYPESNE(t,k)){h=0; ASSERT(HOMO(t,k)&&!(m&SPARSE&&m&XNUM+RAT),EVDOMAIN); t=maxtype(t,k);}
 }}
 // if there were no nonempty contents, go back & pick highest-priority type of empty
 if(!t)DO(n, y=b?(A)AABS(v[i],w):v[i]; k=AT(y); RE(t=maxtype(t,k)););
 // allocate place to build shape of result-cell; initialize to 1s above q, zeros below (this is adding leading 1s to missing leading axes)
 GATV(cs,INT,r,1,0); u=AV(cs); DO(r-q, u[i]=1;); p=u+r-q; DO(q, p[i]=0;);
 // find the shape of a result-cell
 DO(n, y=b?(A)AABS(v[i],w):v[i]; s=AS(y); p=u+r-AR(y); DO(AR(y),p[i]=MAX(p[i],s[i]);););
 if(t&SPARSE)RZ(z=opes(t,cs,w))
 else{
  RE(m=prod(r,u)); RE(zn=mult(n,m)); k=bp(t); q=m*k;
  // Allocate result area & copy in shape (= frame followed by result-cell shape)
  GA(z,t,zn,r+AR(w),AS(w)); ICPY(AS(z)+AR(w),u,r); x=CAV(z);
  c=b&&t&BOX;   // set if result is relative
  if(c){AFLAG(z)=AFREL; p=AV(z); d=AREL(mtv,z); DO(zn, *p++=d;);} else fillv(t,zn,x);
  for(i=0;i<n;++i){
   y=b?(A)AABS(v[i],w):v[i];   // get pointer to contents, relocated if need be
   // if the contents of y is relative, clone it and relocate the clone, either to absolute (if result is absolute c==0) or relative to z (if result is relative)
   if(ARELATIVE(y))RZ(y=relocate((I)y-c*(I)z,ca(y)));
   if(h&&1>=r)                MC(x,AV(y),k*AN(y));
   else if(TYPESEQ(t,AT(y))&&m==AN(y))MC(x,AV(y),q); 
   else if(AN(y))             RZ(povtake(jt,cs,TYPESEQ(t,AT(y))?y:cvt(t,y),x)); 
   x+=q;
 }}
 EPILOG(z);
}

// ; y general case, where rank > 1
// w is the data to raze, t is type of nonempty boxes of w, n=#,w, r=max rank of contents of w, v->w data,
// zb=1 if any of the contents uses relative addressing
static A jtrazeg(J jt,A w,I t,I n,I r,A*v,B zb){A h,h1,x,y,*yv,z,*zv;B b;C*zu;I c=0,d,i,j,k,m,q,*s,*v1,yr,*ys;UI p;
 d=SZI*(r-1); b=ARELATIVE(w);   // d=#bytes in (stored shape of result-cell)  b=1 if w uses relative addressing
 // Calculate the shape of a result-cell (it has rank r-1); and c, the number of result-cells
 GATV(h,INT,r,1,0); s=AV(h); memset(s,C0,r*SZI);  // h will hold the shape of the result; s->shape data; clear to 0 for compares below
 for(i=0;i<n;++i){   // loop over all contents
  // y->A block for contents of w[i]; yr=rank of y; ys->shape of y
  // if contents has the same rank as result, it is an array of result-cells, and each item adds
  // to c, the total # items in result; otherwise it is a single cell that will be promoted in rank to
  // become one result-cell.  Error if overflow (should be impossible).  j=#leading length-1 axes that need to be added
  y=b?(A)AABS(v[i],w):v[i]; yr=AR(y); ys=AS(y); c+=(0==(j=r-yr))?*ys:1; ASSERT(0<=c,EVLIMIT); 
  if(!yr)continue;   // do not perform rank extension of atoms
  // here we find the max cell size in *(s+1). *s is not used.  The maximum shape is taken
  // over extension axes of length 1, followed by the actual shape of the contents
  DO(j,  s[i]=MAX(1,    s[i]);     ); 
  DO(yr, s[j]=MAX(ys[i],s[j]); ++j;);
 }
 // Install the number of result items in s; m=total #result atoms
 *s=c; m=prod(r,s);
 // Now that we know the shape of the result-cell, we can decide, for each box, whether the
 // box contributes to the result, and whether it will be filled.  This matters only if a fill-cell has been specified.
 // If fill has been specified, we include its type in the result-type (a) only if some block gets filled
 // (this will cause all input-blocks to convert to the precision needed; any fill will be promoted to that type)
 // (b) all the blocks are empty (which can be detected because t has not been set yet)
 if(jt->fill) {
  if(t&&m) {  // Check cell-contents only if there are some nonempty contents, and if the result-cell is nonempty
     // these are different, eg for 0 1$4 which has no cells but they are nonempty, or 1 0 1$4 which has an empty result-cell
   for(i=0;i<n;++i) {   // for each box of contents
    y=b?(A)AABS(v[i],w):v[i]; yr=AR(y);   // y-> A block for contents of w[i]; yr = its rank
    if(!yr)continue; ys=AS(y);   // atoms are replicated, never filled; otherwise point to shape
    if(r==yr&&0==ys[0])continue;  // if y is unextended and has no cells, it will not contribute, no matter what the cell-shape
    // see if the shape of y-cell (after rank extension) matches the shape of result-cell.  If not, there will be fill
    for(yr=yr-1,k=r-1;yr>=0&&ys[yr]==s[k];--yr,--k);  // see if unextended cell-shape matches
    if(yr<0)while(k>0&&s[k]==1)--k;   // if all that match, check to see if extended cell-shape==1
    if(k>0) {   // If we compared all the way back to the entire rank or one short (since we only care about CELL shape), there will be no fill
     ASSERT(HOMO(t, AT(jt->fill)), EVDOMAIN); t = maxtype(t, AT(jt->fill));  // Include fill in the result-type.  It better fit in with the others
     break;  // one fill is enough
    }    
   }
  } else{t = AT(jt->fill);} // all empty cells (but possibly many of them), use fill type
 } else {
  // If no fill has been specified, the scan isn't needed, because all blocks will be extended with their
  // normal fill, which will be enough to hold the highest precision.  But if there are no nonempty blocks,
  // we have to scan to get a precision from among the empties
  // ensure literal fill consistent, coerce empty symbol to literal type - less surprise
//  if(!t){DO(n, y=b?(A)AABS(v[i],w):v[i]; t=MAX(UNSAFE(t),UNSAFE(AT(y)));)}
  if(!t){DO(n, y=b?(A)AABS(v[i],w):v[i]; t=MAX(UNSAFE(t),SBT&AT(y)?LIT:C4T&AT(y)?LIT:C2T&AT(y)?LIT:UNSAFE(AT(y))););}
 }

 // Now we know the type of the result.  Create the result.
 k=bp(t); p=c?k*m/c:0;  // k=#bytes in atom of result; p=#bytes/result cell
 GATV(h1,INT,r,1,0); v1=AV(h1);  // create place to hold shape of cell after rank extension
 GA(z,t,m,r,s); if(zb)AFLAG(z)=AFREL;   // create result area, shape s
 zu=CAV(z); zv=AAV(z);  // output pointers
 // loop through each contents and copy to the result area
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i];  // y->address of A block for v[i]
  if(TYPESNE(t,AT(y)))RZ(y=cvt(t,y));   // convert to result type if needed
  yr=AR(y); ys=AS(y);    // yr=rank of y, ys->shape of y
  if(!yr){
   // atomic contents; perform atomic replication
   if(t&BOX){x=(A)(*AV(y)+ARELATIVE(y)*(I)y-zb*(I)z); DO(p/SZA, *zv++=x;);}  // see jtraze; replicate pointer to data
   else     {mvc(p,zu,k,AV(y)); zu+=p;}   // copy the data, replicating
  } else {
   // nonatomic contents: rank extension+fill rather than replication
   // if IC(y)==0 this all does nothing, but perhaps not worth checking
   if(j=r-yr){DO(j,v1[i]=1;); ICPY(j+v1,ys,yr); RZ(y=reshape(h1,y)); }  // if rank extension needed, create rank 1 1...,yr and reshape to that shape
   if(memcmp(1+s,1+AS(y),d)){*s=IC(y); RZ(y=take(h,y));}  // if cell of y has different shape from cell of result, install the
     // #items into s (giving #cell,result-cell shape) and fill to that shape.  This destroys *s (#result items) buts leaves the rest of s
   if(t&BOX){yv=AAV(y); q=ARELATIVE(y)*(I)y-zb*(I)z; DO(AN(y), *zv++=(A)((I)yv[i]+q););}  // copy as above, no replication this time
   else     {j=k*AN(y); MC(zu,AV(y),j); zu+=j;}
  }
 }
 R z;
}    /* raze general case */

// ; y
F1(jtraze){A*v,y,*yv,z,*zv;B b,zb;C*zu;I d,i,k,m=0,n,q,r=1,t=0,yt;
 RZ(w);
 n=AN(w); v=AAV(w); b=ARELATIVE(w); zb=b;  // n=#,w  v->w data  zb=b='boxed relative addressing'
 if(!n)R mtv;   // if empty operand, return boolean empty
 if(!(BOX&AT(w)))R ravel(w);   // if not boxed, just return ,w
 if(1==n){RZ(z=b?(A)AABS(*v,w):*v); R AR(z)?z:ravel(z);}  // if just 1 box, return its contents - except ravel if atomic
 // scan the boxes to create the following values:
 // m = total # atoms in contents; r = maximum rank of contents
 // t = type of result (maxtype among types of nonempty contents)
 // zb set if any of the nonempty contents uses relative addressing
 // Fill creates a subtlety: we don't know whether empty boxes are going to contribute to
 // the result or not.  In a case like (0 2$a:),'' the '' will contribute, but the (0 2$a:) will
 // not.  And, we don't want to require compatibility with the fill-cell if nothing is filled.
 // So, we don't check compatibility for empty boxes.
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i]; m+=d=AN(y); r=MAX(r,AR(y)); 
  if(d){
   yt=AT(y); 
   if(t){ASSERT(HOMO(t,yt),EVDOMAIN); t=maxtype(t,yt);}else t=yt;  // detect incompatible datatypes (only if nonempty)
   zb=zb||ARELATIVE(y);
 }}
 // if the cell-rank was 2 or higher, there may be reshaping and fill needed - go to the general case
 if(1<r)R razeg(w,t,n,r,v,zb);
 // fall through for boxes containing lists and atoms, where the result is a list.  No fill possible, but if all inputs are
 // empty the fill-cell will give the type of the result (similar to 0 {.!.f 0$...)

 // If all the contents were empty, rescan and set the result type to the
 // largest type of the (empty) contents.  Note that this scan takes the simple
 // MAX of types, rather than using maxtype - why?  If fill is specified, it overrides
 // NOTE: arguably this should consider only contents that have cells that will contribute to the result;
 // but this is how it was done originally
  // ensure literal fill consistent, coerce empty symbol to literal type - less surprise
// if(!t){if(jt->fill){t=AT(jt->fill);}else{DO(n, y=b?(A)AABS(v[i],w):v[i]; t=MAX(UNSAFE(t),UNSAFE(AT(y)));)}}
 if(!t){if(jt->fill){t=AT(jt->fill);}else{DO(n, y=b?(A)AABS(v[i],w):v[i]; t=MAX(UNSAFE(t),SBT&AT(y)?LIT:C4T&AT(y)?LIT:C2T&AT(y)?LIT:UNSAFE(AT(y))););}}
 GA(z,t,m,r,0); if(zb&&!(t&DIRECT))AFLAG(z)=AFREL;  // allocate the result area; mark relative if any contents relative
 zu=CAV(z); zv=AAV(z); k=bp(t); // inpout pointers, depending on type; length of an item
 // loop through the boxes copying: the pointers, if boxed; the data, if not boxed
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i];   // y->box[i]
  if(AN(y)){
   // if contents are boxes, calculate q to be the offset to add to make relative: 
   // if neither y nor z is relative, that's 0 (absolute addressing)
   // if z is relative and y is not, that's -z (convert absolute y to relative to z)
   // if z and y are both relative, it's y-z (convert y to absolute, then make relative to z)
   // y relative and z not is impossible
   if(t&BOX){yv=AAV(y); q=ARELATIVE(y)*(I)y-zb*(I)z; DO(AN(y), *zv++=(A)((I)yv[i]+q););}  // yv->contents, copy each pointer
   // For other (always nonrelative) contents, convert data if necessary, then copy it
   else     {if(TYPESNE(t,AT(y)))RZ(y=cvt(t,y)); d=k*AN(y); MC(zu,AV(y),d); zu+=d;}
 }}
 R z;
}

F1(jtrazeh){A*wv,y,z;C*xv,*yv,*zv;I c=0,ck,dk,i,k,n,p,r,*s,t,wd;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 if(!AR(w))R ope(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); y=WVR(0); p=IC(y); t=AT(y); k=bp(t);
 DO(n, y=WVR(i); r=AR(y); ASSERT(p==IC(y),EVLENGTH); ASSERT(r&&r<=2&&TYPESEQ(t,AT(y)),EVNONCE); c+=1==r?1:*(1+AS(y)););
 GA(z,t,p*c,2,0); s=AS(z); *s=p; *(1+s)=c; 
 zv=CAV(z); ck=c*k;
 for(i=0;i<n;++i){
  y=WVR(i); dk=1==AR(y)?k:k**(1+AS(y)); xv=zv; zv+=dk;
  if(!dk)continue;
  if(wd&&t&BOX)RZ(y=car(y));
  yv=CAV(y);
  switch(0==(I)xv%dk&&0==ck%dk?dk:0){
   case sizeof(I): {I*u,*v=(I*)yv; DO(p, u=(I*)xv; *u=*v++;    xv+=ck;);} break;
#if SY_64
   case sizeof(int):{int*u,*v=(int*)yv; DO(p, u=(int*)xv; *u=*v++; xv+=ck;);} break;
#endif
   case sizeof(S): {S*u,*v=(S*)yv; DO(p, u=(S*)xv; *u=*v++;    xv+=ck;);} break;
   case sizeof(C):                 DO(p, *xv=*yv++;            xv+=ck;);  break;
   default:                        DO(p, MC(xv,yv,dk); yv+=dk; xv+=ck;); 
 }}
 R z;
}    /* >,.&.>/,w */


#define EXTZ    if(vv<=d+v){m=v-CAV(z); RZ(z=ext(0,z)); v=m+CAV(z); vv=CAV(z)+k*AN(z);}

F2(jtrazefrom){A*wv,y,z;B b;C*v,*vv;I an,c,d,i,j,k,m,n,r,*s,t,*u,wn;
 RZ(a&&w);
 an=AN(a); wn=AN(w);
 if(b=NUMERIC&AT(a)&&1==AR(a)&&BOX&AT(w)&&!ARELATIVE(w)&&1==AR(w)&&1<wn&&an>10*wn){
  wv=AAV(w); y=*wv; r=AR(y); s=1+AS(y); n=B01&AT(a)?2:wn;
  for(i=m=t=0;b&&i<n;++i){
   y=wv[i]; b=r==AR(y)&&!(1<r&&ICMP(s,1+AS(y),r-1));
   if(AN(y)){m+=AN(y); if(t)b=b&&TYPESEQ(t,AT(y)); else t=AT(y);}
 }}
 if(!(b&&t&DIRECT))R raze(from(a,w));
 c=aii(y); k=bp(t); 
 RZ(z=exta(t,r,c,(I)((1.2*an*m)/(n*c)))); u=AS(z); *u++=AN(z)/c; DO(r-1, *u++=*s++;);
 v=CAV(z); vv=v+k*AN(z);
 if(B01&AT(a)){B*av=BAV(a);        
  for(i=0;i<an;++i){
   y=wv[*av++]; d=k*AN(y); EXTZ; MC(v,AV(y),d); v+=d;
 }}else{I*av; 
  RZ(a=vi(a)); av=AV(a); 
  for(i=0;i<an;++i){
   j=*av++; if(0>j){j+=wn; ASSERT(0<=j,EVINDEX);}else ASSERT(j<wn,EVINDEX);
   y=wv[j];     d=k*AN(y); EXTZ; MC(v,AV(y),d); v+=d;
 }}
 AN(z)=(v-CAV(z))/k; *AS(z)=AN(z)/c;     
 R z;
}    /* a ;@:{ w */
