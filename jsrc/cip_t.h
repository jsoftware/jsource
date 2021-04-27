/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Template for ipbx (boolean inner products)                              */

/* requires F one of |= &= ^= */

// v1 is the y argument to use if xatom is 1
// v0 is the y argument to use of xatom is 0
// av0 points to start of x data

// If an atom of x=esat, the rest of the row of a is immaterial
// If an atom of x=eskip, that innerproduct is immaterial
// loop for each 1-cell of a, producing one 1-cell of the result
for(i=0;i<m;++i, zv+=n, av0+=p){
 av = av0;  // av-> current row of a
 // Initialize result row using the first atom of a; if that will saturate (eg 1 in +./ . +.), do no more processing, skip to next row
 b=*av; av+=ana; MC(zv,b?v1:v0,n); if(b==esat)continue;
 // bring in the rest of the inner products for this 1-cell of a
 for(j=1;j<p;++j){
  // perform one inner product of b (an atom of x) with the corresponding cell f
  // if b==e, this will saturate the result; abort the innerproduct then
  b=*av; av+=ana; if(b==eskip)continue; if(b==esat){ mvc(n,zv,1,iotavec-IOTAVECBEGIN+(c==CPLUSDOT)); break;}
  // If the result doesn't saturate, perform the calculation.  uv->result, vv->item of w, depending on b value
  uu=(I*)zv; vv=(I*)(b?v1+j*wc:v0+j*wc);
  // Do all the fullword operations
  DQ(q, *uu++ F *vv++;);
  // Finish with masked-word operation
  STOREBYTES(uu,(*uu F *vv),r);  // process all but r bytes of the last word
 }
}
#undef F
