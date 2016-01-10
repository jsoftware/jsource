/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing; see APL Dictionary, pp. 12-13 & 38.                            */

#include "j.h"
#include "p.h"


/* NVR - named value reference                                          */
/* a value referenced in the parser which is the value of a name        */
/* (that is, in some symbol table).                                     */
/*                                                                      */
/* jt->nvra      NVR stack a: stack of A values                         */
/* jt->nvrav     AAV(jt->nvra)                                          */
/* jt->nvrb      NVR stack b: corresponding to stack a --               */
/*               1 if unchanged; 0 if redefined                         */
/* jt->nvrbv     BAV(jt->nvrb)                                          */
/* jt->nvrtop    index of top of stack                                  */
/*                                                                      */
/* Each call of the parser records the current NVR stack top (nvrtop),  */
/* (nvrtop), and pop stuff off the stack back to that top on exit       */
/*                                                                      */
/* nvrpush(w):   w is a named value just moved from the parser queue    */
/*               to the parser stack.  Push w onto the NVR stack.       */
/* nvrpop(otop): pop stuff off the NVR stack back to the old top otop   */
/* nvrredef(w):  w is the value of a name about to be redefined         */
/*               (reassigned or erased).  checks whether w is in the    */
/*               NVR stack                                              */


B jtparseinit(J jt){A x;
 GA(x,INT,20,1,0); ra(x); jt->nvra=x; jt->nvrav=AAV(x);
 GA(x,B01,20,1,0); ra(x); jt->nvrb=x; jt->nvrbv=BAV(x);
 R 1;
}

static F1(jtnvrpush){
 if(jt->nvrtop==AN(jt->nvra)){
  RZ(jt->nvra=ext(1,jt->nvra)); jt->nvrav=AAV(jt->nvra);
  while(AN(jt->nvrb)<AN(jt->nvra))RZ(jt->nvrb=ext(1,jt->nvrb)); jt->nvrbv=BAV(jt->nvrb);
 }
 jt->nvrav[jt->nvrtop]=w;
 jt->nvrbv[jt->nvrtop]=1;
 ++jt->nvrtop;
 R w;
}

static void jtnvrpop(J jt,I otop){A*v=otop+jt->nvrav;B*b=otop+jt->nvrbv;
 DO(jt->nvrtop-otop, if(!*b++)fa(*v); ++v;);
 jt->nvrtop=otop;
}

void jtnvrredef(J jt,A w){A*v=jt->nvrav;B*b=jt->nvrbv;
 DO(jt->nvrtop, if(w==*v++){if(b[i]){ra(w); b[i]=0;} break;});
}    /* stack handling for w which is about to be redefined */


ACTION(jtmonad  ){R dfs1(stack[e],stack[b]);}
ACTION(jtdyad   ){R dfs2(stack[b],stack[e],stack[1+b]);}
ACTION(jtadv    ){R dfs1(stack[b],stack[e]);}
ACTION(jtconj   ){R dfs2(stack[b],stack[e],stack[1+b]);}
ACTION(jttrident){R folk(stack[b],stack[1+b],stack[e]);}
ACTION(jtbident ){R hook(stack[b],stack[e]);}
ACTION(jtpunc   ){R stack[e-1];}

static ACTION(jtmove){A z;
 z=stack[MAX(0,e)];
 if(!(NAME&AT(z))||ASGN&AT(stack[b]))R z;
 RZ(z=jt->xdefn&&NMDOT&NAV(z)->flag?symbrd(z):nameref(z));
 R nvrpush(z);
}

static F2(jtisf){R symbis(onm(a),CALL1(jt->pre,w,0L),jt->symb);} 

ACTION(jtis){A f,n,v;B ger=0;C c,*s;
 n=stack[b]; v=stack[e];
 if(LIT&AT(n)&&1>=AR(n)){
  ASSERT(1>=AR(n),EVRANK); 
  s=CAV(n); ger=CGRAVE==*s; 
  RZ(n=words(ger?str(AN(n)-1,1+s):n));
  if(1==AN(n))RZ(n=head(n));
 }
 ASSERT(AN(n)||!IC(v),EVILNAME);
 f=stack[1+b]; c=*CAV(f); jt->symb=jt->local&&c==CASGN?jt->local:jt->global;
 if(NAME&AT(n)) symbis(n,v,jt->symb);
 else if(!AR(n))symbis(onm(n),v,jt->symb);
 else           {ASSERT(1==AR(n),EVRANK); jt->pre=ger?jtfxx:jtope; rank2ex(n,v,0L,-1L,-1L,jtisf);}
 jt->symb=0;
 RNE(v);
}


#define AVN   (     ADV+VERB+NOUN)
#define CAVN  (CONJ+ADV+VERB+NOUN)
#define EDGE  (MARK+ASGN+LPAR)

PT cases[] = {
 EDGE,      VERB,      NOUN, ANY,       jtmonad,   jtvmonad, 1,2,1,
 EDGE+AVN,  VERB,      VERB, NOUN,      jtmonad,   jtvmonad, 2,3,2,
 EDGE+AVN,  NOUN,      VERB, NOUN,      jtdyad,    jtvdyad,  1,3,2,
 EDGE+AVN,  VERB+NOUN, ADV,  ANY,       jtadv,     jtvadv,   1,2,1,
 EDGE+AVN,  VERB+NOUN, CONJ, VERB+NOUN, jtconj,    jtvconj,  1,3,1,
 EDGE+AVN,  VERB+NOUN, VERB, VERB,      jttrident, jtvfolk,  1,3,1,
 EDGE,      CAVN,      CAVN, ANY,       jtbident,  jtvhook,  1,2,1,
 NAME+NOUN, ASGN,      CAVN, ANY,       jtis,      jtvis,    0,2,1,
 LPAR,      CAVN,      RPAR, ANY,       jtpunc,    jtvpunc,  0,2,0,
};


F1(jtparse){A*u,*v,y,z;I n;
 RZ(w);
 n=AN(w); v=AAV(w);
 GA(y,BOX,5+n,1,0); u=AAV(y);
 RZ(deba(DCPARSE,0L,w,0L));
 *u++=mark; DO(n, *u++=*v++;); *u++=mark; *u++=mark; *u++=mark; *u++=mark;
 z=parsea(y);
 debz();
 R z;
}

F1(jtparsea){A*s,*stack,y,z;AF f;I b,*c,e,i,j,k,m,n,otop=jt->nvrtop,*sp;                  
 RZ(w);
 n=m=AN(w)-4; stack=AAV(w); jt->asgn=0; ++jt->parsercalls;                                     
 if(1>=n)R mark;
 RZ(y=IX(AN(w))); sp=AV(y);  /* current location in tokens */
 do{                                                                      
  for(i=0;i<NCASES;i++){                                                 
   c=cases[i].c; s=n+stack;                                              
   if(*c++&AT(*s++)&&*c++&AT(*s++)&&*c++&AT(*s++)&&*c++&AT(*s++)) break; 
  }                                                             
  if(i<NCASES){                                                   
   b=cases[i].b; j=n+b;                                           
   e=cases[i].e; k=n+e;                                            
   jt->sitop->dci=sp[k]=sp[n+cases[i].k];                                     
   f=cases[i].f;                                              
   jt->asgn=f==jtis;                                                           
   stack[k]=y=f(jt,j,k,stack);                                              
   DO(b, stack[--k]=stack[--j]; sp[k]=sp[j];); n=k;                      
  }else{                                                                 
   jt->sitop->dci=sp[MAX(0,n-1)]=sp[MAX(0,m-1)];                               
   stack[n-1]=y=move(n,m-1,stack);                                       
   n-=0<m--;                                                             
 }} while(y&&0<=m);
 nvrpop(otop);
 RZ(y);                                                             
 z=stack[1+n];                                                      
 ASSERT(AT(z)&MARK+CAVN&&AT(stack[2+n])&MARK,EVSYNTAX);        
 R z;                                                          
}

/* locals in parsea                                             */
/* b:     beginning index in stack that action applies to       */
/* c:     temp on cases[] used to match 4-patterns              */
/* e:     ending    index in stack that action applies to       */
/* f:     current action                                        */
/* i:     index in cases[] of matching 4-pattern                */
/* j:     absolute index corresponding to b                     */
/* k:     absolute index corresponding to e                     */
/* m:     current # of words in the queue                       */
/* n:     index of top of stack                                 */
/* otop:  old value of jt->nvrtop                               */
/* s:     temp on stack used to match 4-patterns                */
/* sp:    index in original sentence of stack elements          */
/* stack: parser stack as described in dictionary               */
/* w:     argument; contains both the queue and the stack       */
/* y:     array temp                                            */
/* z:     result                                                */
