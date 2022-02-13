/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */

#include "j.h"

#if ARTIFHIPREC
// w is a block that will become the contents of a box.  Put it inside a hiprec and return the address of the hiprec.
// aflag is the boxing flag of the block the result is going to go into.  w has been prepared for that type
A jtartiffut(J jt,A w,I aflag){A z;
 GAT0(z,BOX,1,0); AFLAG(z)|=BOX; AAV0(z)[0]=w; AT(z)|=HIPREC; if(aflag&BOX){ra(z);}else{ACIPNO(z); ra(w);}  // just one of z/w must be adjusted to the recursion environment
 R z;
}
#endif
// x is a A holding a hiprecs value.  Return its value when it has been resolved
A jthipval(J jt,A w){
 // read the hiprecs value.  Since the creating thread has a release barrier after creation and another after final resolution, we can be sure
 // that if we read nonzero the hiprec has been resolved, even without an acquire barrier
 A res=AAV0(w)[0];  // fetch the possible value
 if(unlikely(res==0)){
  // here the hiprec was unresolved.  Wait for it
  SEGFAULT;  // scaf
 }
 // res now contains the certified value of the hiprec.
 ASSERT(((I)res&-256)!=0,(I)res)   // if error, return the error code
 R res;  // otherwise return the resolved value
}

