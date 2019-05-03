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

F2(jtfixrecursive){A f,g,y;
 RZ(a&&w);
 RZ(y=lrep(w)); 
 if(a==num[1]   ||a==num[3])RZ(f=colon(num[3],                over(y,cstr(" y")) ));
 if(a==num[2]||a==num[3])RZ(g=colon(num[4],over(cstr("x "),over(y,cstr(" y")))));
 R a==num[3]?colon(f,g):a==num[1]?f:g;
}
