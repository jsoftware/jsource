/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Custom                                                           */

#include "j.h"


F2(jtforeignextra){
 ARGCHK2(a,w);
 R CDERIV(CIBEAM, 0,0, VFLAGNONE,RMAX,RMAX,RMAX);
}

// create explicit equivalent of verb w
// a is flags to handle lower recursions:
//  0 to begin with
//  1, 2, or 3 to indicate that a piece of the original input has been processed, and any $: found in a name must
//   be replaced by an explicit equivalent with the indicated valence(s)
F2(jtfixrecursive){A z;
 ARGCHK2(a,w);
 df1(z,w,eval("1 : 'u y\n:\nx u y'"));
 R z;
}
