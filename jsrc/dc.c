/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Function Call Information                                        */

#include "j.h"
#include "d.h"


static F1(jtdfrep){ARGCHK1(w); R NOUN&AT(w)?w:lrep(w);}

static SYMWALK(jtdloc,A,BOX,5,2,1,{RZ(*zv++=incorp(sfn(0,d->name))); RZ(*zv++=incorp(dfrep(d->val)));})

static B jtdrow(J jt,DC si,DC s0,A*zv,UI ncollist,I* collist){A fs,q,*qv,y,z;C c;UI col;
 fs=si->dcf;
 GATV0(q,BOX,!!si->dcx+!!si->dcy,1); qv=AAV1(q);  // allo place to store arg list
 if(si->dcx)RZ(*qv++=incorp(dfrep(si->dcx)));   // fill in x if any
 if(si->dcy)RZ(*qv++=incorp(dfrep(si->dcy)));  // fill in y if any

 for(col=0;col<ncollist;++col){
  switch(collist[col]){
  case 0:
   RZ(*zv++=incorp(si->dca&&si->dcnmlev!=3?over(sfn(0,si->dca),str(si->dcnmlev,">>")):mtv));    // 0 name - decorated
   break;
  case 1:
   RZ(*zv++=incorp(sc(si->dcj)));                        /* 1 error number             */
   break;
  case 2:
   RZ(*zv++=incorp(sc(lnumsi(si))));                     /* 2 line number              */
   break;
  case 3:
   *zv++=num(ADV&AT(fs)?1:CONJ&AT(fs)?2:3);  /* 3 name class               */
   break;
  case 4:
   RZ(*zv++=incorp(jtlrep((J)((I)jt|(JTEXPVALENCEOFFD>>si->dcdyad)),fs)));  // 4 linear rep, but only the correct valence
   break;
  case 5:
   {
   I scriptx=-1; A snm; if(si->dca&&AN(si->dca)&&((snm=box(sfn(0,si->dca)))!=0)){  // name exists, is nonempty, & well formed
    RZ(snm=scind(snm)); scriptx=IAV(snm)[0];  // get script index from the symbol, -1 if not found
    READLOCK(JT(jt,startlock))   // lock the slist table while we refer to it
    if(BETWEENO(scriptx,0,AN(JT(jt,slist))))snm=AAV(JT(jt,slist))[scriptx]; else snm=mtv;  // get string form of filename, possibly empty
    READUNLOCK(JT(jt,startlock))
   }else snm=mtv;   // missing name, use empty string for it
   *zv++=incorp(snm);  // 5 script name
   break;
   }
  case 6:
   RZ(*zv++=incorp(q));                                  /* 6 argument list            */
   break;
  case 7:
   if(si->dcloc&&si->dcc){RZ(y=dloc(si->dcloc)); RZ(*zv++=incorp(grade2(y,ope(IRS1(y,0L,1L,jthead,z)))));}  // local symbols only if explicit defn
   else         RZ(*zv++=incorp(iota(v2(0L,2L))));   /* 7 locals                   */  // empty so cannot be readonly
   break;
  case 8:
   c=si->dcsusp||s0&&DCPARSE==s0->dctype&&s0->dcsusp?'*':' ';
   RZ(*zv++=incorp(scc(c)));                         /* 8 * if begins a suspension */
   break;
  case 9:
   y=si->dcloc?sfn(0,LOCNAME(AKGST(si->dcloc))):mtv; y=y?y:mtv;  // get implied locale from the stack frame
   RZ(*zv++=incorp(y));                         /* 8 * if begins a suspension */
   break;
  case 10:
   y=si->dcc?cw57rep(jt,si->dcc):mtv; y=y?y:mtv;  // get implied locale from the stack frame
   RZ(*zv++=incorp(y));                         /* 8 * if begins a suspension */
   break;
  }
 }
 R 1;
}    /* construct one row of function stack- called only for DCCALL type */

F1(jtdbcall){A y,*yv,z,zz,*zv;DC si,s0=0;I c=11,m=0,*s;  // c is # columns
 UI *collist, ncollist;   // arrays of columns, and its length
 ASSERT(AR(w)<2,EVRANK);  // must be atom or list
 if(AN(w)==0){collist=&iotavec[-IOTAVECBEGIN]; ncollist=9;  // default is IX(9)
 }else{RZ(w=vib(w)) collist=IAV(w); ncollist=AN(w); DO(ncollist, ASSERT(BETWEENO(collist[i],0,c),EVINDEX))
 }
 si=jt->sitop;
 NOUNROLL while(si){if(DCCALL==si->dctype)++m; si=si->dclnk;}  // count # rows in result
 GATV0(z,BOX,m*ncollist,2); s=AS(z); s[0]=m; s[1]=ncollist;  // allocate result, install shape
 si=jt->sitop; zv=AAV2(z);
 NOUNROLL while(si){if(DCCALL==si->dctype){RZ(jtdrow(jt,si,s0,zv,ncollist,collist)); zv+=ncollist;} s0=si; si=si->dclnk;}  // create one row for each CALL, in z
 RETF(z);
}    /* 13!:13 function stack */
