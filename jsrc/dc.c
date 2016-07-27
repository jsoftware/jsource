/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Function Call Information                                        */

#include "j.h"
#include "d.h"


static F1(jtdfrep){RZ(w); R NOUN&AT(w)?w:lrep(w);}

static SYMWALK(jtdloc,A,BOX,5,2,1,{RZ(*zv++=sfn(0,d->name)); RZ(*zv++=dfrep(d->val));})

static B jtdrow(J jt,DC si,DC s0,A*zv){A fs,q,*qv,y;C c;
 fs=si->dcf;
 GATV(q,BOX,si->dcx&&si->dcy?2:1,1,0); qv=AAV(q); 
 if(si->dcx)*qv++=dfrep(si->dcx); 
 if(si->dcy)*qv++=dfrep(si->dcy);
 *zv++=sfn(0,si->dca);                     /* 0 name                     */
 *zv++=sc(si->dcj);                        /* 1 error number             */
 *zv++=sc(lnumsi(si));                     /* 2 line number              */
 *zv++=num[ADV&AT(fs)?1:CONJ&AT(fs)?2:3];  /* 3 name class               */
 *zv++=lrep(fs);                           /* 4 linear rep.              */
 *zv++=0;                                  /* 5 script name              */
 *zv++=q;                                  /* 6 argument list            */
 if(si->dcloc){RZ(y=dloc(si->dcloc)); RZ(*zv++=grade2(y,ope(irs1(y,0L,1L,jthead))));}
 else         RZ(*zv++=iota(v2(0L,2L)));   /* 7 locals                   */
 c=si->dcsusp||s0&&DCPARSE==s0->dctype&&s0->dcsusp?'*':' ';
 RZ(*zv++=scc(c));                         /* 8 * if begins a suspension */
 R 1;
}    /* construct one row of function call matrix */

F1(jtdbcall){A y,*yv,z,*zv;DC si,s0=0;I c=9,m=0,*s;
 ASSERTMTV(w);
 si=jt->sitop; while(si){if(DCCALL==si->dctype)++m; si=si->dclnk;}
 GATV(z,BOX,m*c,2,0); s=AS(z); s[0]=m; s[1]=c;
 si=jt->sitop; zv=AAV(z);
 while(si){if(DCCALL==si->dctype){RZ(drow(si,s0,zv)); zv+=c;} s0=si; si=si->dclnk;}
 RZ(y=from(scind(irs1(z,0L,1L,jthead)),over(snl(mtv),ace)));
 yv=AAV(y); zv=5+AAV(z);
 DO(m, *zv=*yv++; zv+=c;);
 R z;
}    /* 13!:13 function call matrix */
