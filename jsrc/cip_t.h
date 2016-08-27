/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Template for ipbx (boolean inner products)                              */

/* requires F one of |= &= ^= */

// v1 is the y argument to use if xatom is 1
// v0 is the y argument to use of xatom is 0
// av0 points to start of x data
if(c==CPLUSDOT&&(c0==IPBX1||c1==IPBX1)||c==CSTARDOT&&(c0==IPBX0||c1==IPBX0)){
// +./ . (+. <: >: *:)   *./ . (*. < > +:)
 e=c==CPLUSDOT?c1==IPBX1:c1==IPBX0;  // e if +./ . (+. >:)   or *./ . (< +:) where x=1 overrides y (producing 1);  if e=0, x=0 overrides y (producing 1)
 // If an atom of x=e, the rest of the row of a is immaterial
 // loop for each 1-cell of a, producing one 1-cell of the result
 for(i=0;i<m;++i, zv+=n, av0+=p){
  av = av0;  // av-> current row of a
  // Initialize result row using the first atom of a; if that will saturate (eg 1 in +./ . +.), do no more processing, skip to next row
  b=*av; av+=ana; memcpy(zv,b?v1:v0,n); if(b==e)continue;
  // bring in the rest of the inner products for this 1-cell of a
  for(j=1;j<p;++j){
   // perform one inner product of b (an atom of x) with the corresponding cell f
   // if b==e, this will saturate the result; abort the innerproduct then
   b=*av; av+=ana; if(b==e){memset(zv,c==CPLUSDOT?C1:C0,n); break;}
   // If the result doesn't saturate, perform the calculation.  uv->result, vv->item of w, depending on b value
   uu=(I*)zv; vv=(I*)(b?v1+j*wc:v0+j*wc);
   // Do all the fullword operations
   DO(q, *uu++ F *vv++;);
   // Finish with byte operations
   u=(B*)uu; v=(B*)vv; DO(r, *u++ F *v++;);
  }
}}else if(c==CPLUSDOT&&(c0==IPBX0||c1==IPBX0)||c==CSTARDOT&&(c0==IPBX1||c1==IPBX1)||
    c==CNE&&(c0==IPBX0||c1==IPBX0)){
 // (+. ~:)/ . (*. < > +:)   *./ . (+. <: >: *:)
 e=c==CSTARDOT?c1==IPBX1:c1==IPBX0;
 for(i=0;i<m;++i, zv+=n, av0+=p){
  av=av0;
  b=*av; av+=ana; memcpy(zv,b?v1:v0,n);
  for(j=1;j<p;++j){
   b=*av; av+=ana; if(b==e)continue;
   uu=(I*)zv; vv=(I*)(b?v1+j*wc:v0+j*wc);
   DO(q, *uu++ F *vv++;);
   u=(B*)uu; v=(B*)vv; DO(r, *u++ F *v++;);
  }
}}else{
 // other cases
 av=av0;
 for(i=0;i<m;++i, zv+=n){
  memcpy(zv,*av?v1:v0,n); av+=ana;
  for(j=1;j<p;++j){
   uu=(I*)zv; vv=(I*)(*av?v1+j*wc:v0+j*wc); av+=ana;
   DO(q, *uu++ F *vv++;);
   u=(B*)uu; v=(B*)vv; DO(r, *u++ F *v++;);
  }
 }}
#undef F
