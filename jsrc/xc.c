/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Custom                                                           */

#include "j.h"
#ifdef LOCALFOREIGNS
#include "../../licensed addons/xcl.h"
#endif


F2(jtforeignextra){
 RZ(a&&w);
 R CDERIV(CIBEAM, 0,0, VFLAGNONE,RMAX,RMAX,RMAX);
}

// create explicit equivalent of verb w
// a is flags to handle lower recursions:
//  0 to begin with
//  1, 2, or 3 to indicate that a piece of the original input has been processed, and any $: found in a name must
//   be replaced by an explicit equivalent with the indicated valence(s)
F2(jtfixrecursive){A f,g,y;
 RZ(a&&w);
 I ai=*IAV0(a);
 RZ(y=lrep(w)); 
 if(((I)1<<ai)&0xa)RZ(f=colon(num[3],                over(y,cstr(" y")) ));  // 1 or 3
 if(((I)1<<ai)&0xc)RZ(g=colon(num[4],over(cstr("x "),over(y,cstr(" y")))));  // 2 or 3
 R ai==3?colon(f,g):ai==1?f:g;
}
