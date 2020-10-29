/* Copyright 1990-2002, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Implementation Internals                                         */

#include "j.h"
#include "x.h"


F1(jtaflag1){ARGCHK1(w); R sc(AFLAG(w));}

F2(jtaflag2){I k; ARGCHK2(a,w); RE(k=i0(a)); AFLAG(w)=(FLAGT)k; R w;}

F1(jthash){RZ(w=vs(w)); R sc(hic(AN(w),UAV(w)));}
