/* Copyright 1990-2002, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Implementation Internals                                         */

#include "j.h"
#include "x.h"


F1(jtaflag1){RZ(w); R sc(AFLAG(w));}

F2(jtaflag2){I k; RZ(a&&w); RE(k=i0(a)); AFLAG(w)=k; R w;}

F1(jthash){RZ(w=vs(w)); R sc(hic(AN(w),UAV(w)));}
