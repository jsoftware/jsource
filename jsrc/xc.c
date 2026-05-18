/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Custom                                                           */

#include "j.h"


// obsolete F2(jtforeignextra){F12IP;
// obsolete  ARGCHK2(a,w);
// obsolete  R CDERIV(CIBEAM, 0,0, VFLAGNONE,RMAX,RMAX,RMAX);
// obsolete }
// obsolete 
// obsolete // create explicit equivalent of verb w
// obsolete // a is flags to handle lower recursions:
// obsolete //  0 to begin with
// obsolete //  1, 2, or 3 to indicate that a piece of the original input has been processed, and any $: found in a name must
// obsolete //   be replaced by an explicit equivalent with the indicated valence(s)
// obsolete F2(jtfixrecursive){F12IP;A z;
// obsolete  ARGCHK2(a,w);
// obsolete  df1(z,w,eval("1 : 'u y\n:\nx u y'"));
// obsolete  R z;
// obsolete }
