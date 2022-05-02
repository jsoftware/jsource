/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Function Call Information                                        */

#include "j.h"
#include "d.h"


static F1(jtdfrep){ARGCHK1(w); R NOUN&AT(w)?w:lrep(w);}

static SYMWALK(jtdloc,A,BOX,5,2,1,{RZ(*zv++=incorp(sfn(0,d->name))); RZ(*zv++=incorp(dfrep(d->val)));})

static B jtdrow(J jt,DC si,DC s0,A*zv){A fs,q,*qv,y,z;C c;
 fs=si->dcf;
 GATV0(q,BOX,si->dcx&&si->dcy?2:1,1); qv=AAV(q);  // allo place to store arg list
 if(si->dcx)RZ(*qv++=incorp(dfrep(si->dcx)));   // fill in x if any
 if(si->dcy)RZ(*qv++=incorp(dfrep(si->dcy)));  // fill in y if any
 RZ(*zv++=incorp(sfn(0,si->dca)));                     /* 0 name                     */
 RZ(*zv++=incorp(sc(si->dcj)));                        /* 1 error number             */
 RZ(*zv++=incorp(sc(lnumsi(si))));                     /* 2 line number              */
 *zv++=num(ADV&AT(fs)?1:CONJ&AT(fs)?2:3);  /* 3 name class               */
 RZ(*zv++=incorp(lrep(fs)));                           /* 4 linear rep.              */
 *zv++=0;                                  /* 5 script name, filled in later              */
 RZ(*zv++=incorp(q));                                  /* 6 argument list            */
 if(si->dcloc){RZ(y=dloc(si->dcloc)); RZ(*zv++=incorp(grade2(y,ope(IRS1(y,0L,1L,jthead,z)))));}
 else         RZ(*zv++=incorp(iota(v2(0L,2L))));   /* 7 locals                   */  // empty so cannot be readonly
 c=si->dcsusp||s0&&DCPARSE==s0->dctype&&s0->dcsusp?'*':' ';
 RZ(*zv++=incorp(scc(c)));                         /* 8 * if begins a suspension */
 R 1;
}    /* construct one row of function call matrix */

F1(jtdbcall){A y,*yv,z,zz,*zv;DC si,s0=0;I c=9,m=0,*s;
 ASSERTMTV(w);
 si=jt->sitop;
 NOUNROLL while(si){if(DCCALL==si->dctype)++m; si=si->dclnk;}  // count # rows in result
 GATV0(z,BOX,m*c,2); s=AS(z); s[0]=m; s[1]=c;  // allocate result, install shape
 si=jt->sitop; zv=AAV(z);
 NOUNROLL while(si){if(DCCALL==si->dctype){RZ(drow(si,s0,zv)); zv+=c;} s0=si; si=si->dclnk;}  // create one row for each CALL, in z
 RZ(y=from(scind(IRS1(z,0L,1L,jthead,zz)),over(snl(mtv),ds(CACE))));  // get script index for each line of stack; then fetch the name, or a: if no name
 yv=AAV(y); zv=5+AAV(z);
 DQ(m, *zv=incorp(*yv); yv++; zv+=c;);  // copy the script names into column 5
 RETF(z);
}    /* 13!:13 function call matrix */
