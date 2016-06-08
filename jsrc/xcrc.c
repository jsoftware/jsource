/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: CRC calculation                                                  */

#include "j.h"
#include "x.h"


typedef unsigned int II;

static II crctab[256];

static II jtcrcvalidate(J jt,A w){A*wv;B*v;I m,wd;II p,x,z=-1;
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 m=AN(w);
 if(m&&BOX&AT(w)){ASSERT(2>=m,EVLENGTH); wv=AAV(w); wd=(I)w*ARELATIVE(w); w=WVR(0); if(2==m)RE(z=(II)i0(WVR(1)));}
 if(B01&AT(w)){ASSERT(32==AN(w),EVLENGTH); v=BAV(w); p=0; DO(32, p<<=1; if(*v++)p|=1;);}
 else RE(p=(II)i0(w));
 DO(256, x=(II)i; DO(8, if(1&x)x=p^x>>1; else x>>=1;); crctab[i]=x;); 
 R z;
}

F1(jtcrc1){F1PREF;R crc2(sc(-306674912),w);}

F2(jtcrc2){F2PREF;I n;II z;UC*v;
 RZ(a&&w);
 ASSERT(1>=AR(a)&&1>=AR(w),EVRANK);
 n=AN(w); v=UAV(w);
 ASSERT(!n||LIT&AT(w),EVDOMAIN);
 RE(z=crcvalidate(a));
 DO(n, z=z>>8^crctab[255&(z^*v++)];);
 R sc((I)z^(I)-1);
}

F1(jtcrccompile){F1PREF;A h,*hv;II z;
 RZ(w);
 GA(h,BOX,2,1,0); hv=AAV(h);
 RE(z=crcvalidate(w));
 RZ(hv[0]=vec(INT,256L,crctab));
 RZ(hv[1]=sc((I)z));
 R h;
}

DF1(jtcrcfixedleft){F1PREF;A h,*hv;I n;II*t,z;UC*v;
 RZ(w);
 h=VAV(self)->h; hv=AAV(h); t=(II*)AV(hv[0]); z=(II)*AV(hv[1]);
 n=AN(w); v=UAV(w);
 ASSERT(!n||LIT&AT(w),EVDOMAIN); 
 DO(n, z=z>>8^t[255&(z^*v++)];);
 R sc((I)z^(I)-1);
}
