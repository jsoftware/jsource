/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: CRC calculation                                                  */

#include "j.h"
#include "x.h"


// Calculate byte-at-a-time CRC table in *crctab, and return the starting value as the result
static UINT jtcrcvalidate(J jt,A w, UINT* crctab){A*wv;B*v;I m;UINT p,x,z=-1;
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 m=AN(w);
 if(m&&BOX&AT(w)){ASSERT(2>=m,EVLENGTH); wv=AAV(w);  w=wv[0]; if(2==m)RE(z=(UINT)i0(wv[1]));}
 if(B01&AT(w)){ASSERT(32==AN(w),EVLENGTH); v=BAV(w); p=0; DO(32, p<<=1; if(*v++)p|=1;);}
 else RE(p=(UINT)i0(w));
 DO(256, x=(UINT)i; DO(8, if(1&x)x=p^x>>1; else x>>=1;); crctab[i]=x;); 
 R z;
}

F1(jtcrc1){R crc2(sc(-306674912),w);}

F2(jtcrc2){I n;UINT z;UC*v; UINT crctab[256];
 RZ(a&&w);
 ASSERT(1>=AR(a)&&1>=AR(w),EVRANK);
 n=AN(w); v=UAV(w);
// ASSERT(!n||AT(w)&LIT+C2T+C4T,EVDOMAIN);
 ASSERT(!n||AT(w)&LIT,EVDOMAIN);
 RE(z=crcvalidate(a,crctab));
 n=AT(w)&C4T?(4*n):AT(w)&C2T?n+n:n;
 DO(n, z=z>>8^crctab[255&(z^*v++)];);  // do the computation using unsigned z
 R sc((I)(I4)(z^-1L));  // sign-extend result if needed to make 64-bit and 32-bit the same numeric value
}

F1(jtcrccompile){A h,*hv;UINT z; UINT crctab[256];
 RZ(w);
 GAT0(h,BOX,2,1); hv=AAV(h);
 RE(z=crcvalidate(w,crctab));
 RZ(hv[0]=rifvs(vec(LIT,sizeof(crctab),crctab)));  // Save the table.  We don't have any other good type to use
 RZ(hv[1]=rifvs(sc((I)z)));
 R h;
}

DF1(jtcrcfixedleft){A h,*hv;I n;UINT*t,z;UC*v;
 RZ(w);
 h=FAV(self)->fgh[2]; hv=AAV(h); t=(UINT*)AV(hv[0]); z=(UINT)*AV(hv[1]);
 n=AN(w); v=UAV(w);
 ASSERT(!n||AT(w)&LIT+C2T+C4T,EVDOMAIN);
 n=AT(w)&C4T?(4*n):AT(w)&C2T?n+n:n;
 DO(n, z=z>>8^t[255&(z^*v++)];);
 R sc((I)(I4)(z^-1L));
}
