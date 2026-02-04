/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Error Signalling and Display                                     */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif
#include <stdarg.h>

#include "j.h"
#include "d.h"

// All the 'display' routines in this file simply add characters to the error buffer.  They can't be typed
// here, because the error may be captured by a higher-level routine

// add n chars at *s to the error buffer at jt->etxn, increment jt->etxn
static void jtep(J jt,I n,C*s){I m;
 m=NETX-jt->etxn; m=MIN(n,m); 
 if(0<m){MC(jt->etxinfo->etx+jt->etxn,s,m); jt->etxn+=m;}
}

static void jteputs(J jt,C*s){ep((I)strlen(s),s);}

static void jteputc(J jt,C c){ep(1L,&c);}

static void jteputlnolf(J jt,A w){ep(AN(w),CAV(w));}
static void jteputl(J jt,A w){jteputlnolf(jt,w); eputc(CLF);}

static void jteputv(J jt,A w){I m=NETX-jt->etxn; if(m>0){jt->etxn+=thv(w,~MIN(m,200),jt->etxinfo->etx+jt->etxn);}} // stop writing when there is no room in the buffer.  Complement for decoration
     /* numeric vector w */

static void jteputq(J jt,A w,I nflag){C q=CQUOTE,*s;
 if(equ(ds(CALP),w))eputs(" a."+!(nflag&1));
 else{
  eputc(q);
  s=CAV(w); DO(AN(w), eputc(s[i]); if(q==s[i])eputc(q););
  eputc(q);
}}   /* string w, possibly with quotes */

static void jtefmt(J jt,C*s,I i){
 if(15<NETX-jt->etxn){C*v=jt->etxinfo->etx+jt->etxn; sprintf(v,s,i); jt->etxn+=strlen(v);}
}

// jt has the typeout flags.  Display error text if any, then reset errors
// If the sentence fails, there should always be an error message EXCEPT for when debug exits: that fails quietly
void jtshowerr(J jt){F1PREFJT;C b[1+2*NETX],*p,*q,*r;
 if(jt->etxn&&!((I)jtfg&JTPRNOSTDOUT)){  // if there is a message and it is not suppressed...
  p=b; q=jt->etxinfo->etx; r=q+jt->etxn;
  NOUNROLL while(q<r&&p<b+2*NETX-3){if(*q==CLF){strcpy(p,OUTSEQ); p+=strlen(OUTSEQ); ++q;}else *p++=*q++;}  // avoid buffer overrun on huge typeouts
  *p=0;
#ifdef ANDROID
  A z=tocesu8(str(strlen(b),b));
  CAV(z)[AN(z)]=0;
  jsto(jt,MTYOER,CAV(z));
#else
  jsto(jt,MTYOER,b);
#endif
 }
 RESETERR   // leave the error text until we start a new sentence from jdo, so that if the user looks at the stack it will still be there
}

static I jtdisp(J jt,A w,I nflag);
// format one non-noun entity into the error line
// nflag contains display flags: 1=space before number, 2=parens around non-primitive, 4=space before {
// The entity came from a single sentence but may be compound.  Display it on a single line
// returns new nflag, which is 4 if the primitive was { alone (the 1 and 2 flags must be 0 after primitive output)
// maintenance note: don't use GA().  This gets called after jbreak, which causes all memory requests to fail.
static I jtdspell(J jt,C id,A w,I nflag){C c,s[5];
 // constant verbs require looking at h
 if(id==CFCONS){if((nflag&1))eputc(' '); eputv(FAV(w)->fgh[2]); eputc(':'); nflag=0;
 }else{
  // get fgh if any.  Format f if any, then the primitive, then g if any.  fgh are present only in ACV type (ASGN doesn't have them at all), and not in primitives
  A f,g,h;
  if(AT(w)&VERB+ADV+CONJ){f=FAV(w)->fgh[0], g=id==CBOX?0:FAV(w)->fgh[1], h=FAV(w)->fgh[2];}else{f=g=h=0;}  // plain value for fgh; ignore g field in BOX, which is there to resemble <@]
  if(id==CFORK){if(h==0){h=g; g=f; f=ds(CCAP);}}else h=0;  // reconstitute [: g h; otherwise we display h only for fork
  if(g&&!f){f=g; g=0;}  // if adverb has its arg in g rather than f, restore it to f
  // if this is not a primitive, we must enclose it in parentheses if we are told to in nflag or if it was originally defined as a hook/fork which must have used parentheses
  I invisiblemod=BETWEENC(id,CHOOK,CADVF);  // true if hook/fork/advf
  I parenhere=(g||h)&&(invisiblemod||nflag&2);  // set if we need parens around our value
  if(parenhere)eputc('(');
  if(f)nflag=disp(f,0);  // display left side if any
  // display the primitive, with a leading space if it begins with inflection, a digit, or {.  Don't display the code for an invisible modifier - that's used only for ARs
  if(!invisiblemod){
   s[0]=' '; s[4]=0;
   spellit(id,1+s);
   c=s[1]; 
   eputs(s+!(c==CESC1||c==CESC2||(nflag&4)&&c=='{'||(nflag&1)&&((ctype[(UC)c]&~CA)==0)));
  }
  nflag=(id==CFROM)<<2;  // set flag bit 2 if { bare
  if(g)nflag=disp(g,2|nflag);  // display right side if any
  if(h)nflag=disp(h,2|nflag);  // display end of fork/trident if any
  if(parenhere)eputc(')');
 }
 R nflag;
}

static F1(jtsfn0){F12IP;R sfn(0,w);}  // return string form of full name for a NAME block
EVERYFS(sfn0overself,jtsfn0,jtover,0,VFLAGNONE)

// print a word; nflag bits if (space needed before name/numeric),(parens needed),(space before { needed); return new value of nflag
// maintenance note: don't use GA().  This gets called after jbreak, which causes all memory requests to fail.
static I jtdisp(J jt,A w,I nflag){B b=1&&AT(w)&NAME+NUMERIC;   // b if this is name or numeric, which needs a space before the next name/numeric
 // if this is a noun from a (( )) block or anything from (), we have to take its linear rep, since it might not be displayable in 1 line
 if(AFLAG(w)&AFDPAREN){
  I wisnoun=AT(w)&NOUN; if(wisnoun)eputc('(');  // noun must have come from (( )), so add one ()
  // linear rep may fail, or parts of it may fail; so we must reset errors.  We set etxn neg to indicate that the error line is frozen
  I se=jt->jerr; jt->jerr=0; I sn=jt->etxn; jt->etxn=-sn; w=jtlrep((J)((I)jt|JTOUTERPARENS+JTPRFORSCREEN),w); jt->jerr=se; jt->etxn=sn; RZ(w); ep(AN(w),CAV(w)); // out (lin rep)
  if(wisnoun)eputc(')'); 
  R 0;  // new nflag - none since we added )
 }
 // If this is a PPPP, enclose it in ()
 if(AFLAG(w)&AFDPAREN)eputc('(');  // leading ( of PPPP
 if(b&&(nflag&1))eputc(' ');  // if prev was name/numeric and this is too, put in the space
 switch(CTTZ(AT(w))){
 case B01X:
 case INTX: case INT2X: case INT4X: 
 case FLX:
 case CMPXX: case QPX:
 case XNUMX: 
 case RATX:  eputv(w);                break;
 case BOXX:
  if(!(AT(w)&BOXMULTIASSIGN)){eputs(" a:"+!(nflag&1)); break;}
  // If this is an array of names, turn it back into a character string with spaces between
  // we can't do this by simply executing }: (string&.> names) ,&.> ' ' because if we are out of memory we need to get the string out.  So we do it by hand
  eputc('\''); DO(AN(w), if(i!=0)eputc(' '); A bx=AAV(w)[i]; ep(NAV(bx)->n,NAV(bx)->s);) eputc('\''); break;
 case LITX:  eputq(w,(nflag&1));                break;
 case NAMEX: ep(NAV(w)->n,NAV(w)->s); if(unlikely((AT(w)&NAMEABANDON)!=0)){ep(2,"_:");}     break;
 case LPARX: eputc('(');              break;
 case RPARX: eputc(')');              break;
 case ASGNX: dspell(CAV(w)[0],w,(nflag&1));       break;
 case MARKX:                          break;
 default:   b|=dspell(FAV(w)->id,w,(nflag&5)|(AFLAG(w)&AFDPAREN?0:2));     break;  // VERB comes here - force parens on non-primitive if not PPPP, and pass space-before-{ flag in & out
 }
 if(AFLAG(w)&AFDPAREN){eputc(')'); b&=~4;}  // trailing ) of PPPP, which extinguishes the need for space before {
 R b;  // new nflag
}

// display DCPARSE stack frame
static void jtseeparse(J jt,DC d){A*v;
 v=(A*)d->dcy;  /* list of tokens */
 I etok=(US)d->dcix-1;         /* index of active token when error found */
 I ptok=(d->dcix>>16)-1; ptok=ptok==etok?0:ptok;  // get location of paren error.  If same as error token, give only one spacing
 I nflag=0;
 I m1=jt->etxn;  // starting index of sentence text
 DO(d->dcn, if(i==etok||i==ptok)eputs("    "); nflag=disp(QCWORD(v[i]),nflag););  // display tokens with spaces before error(s)
 if(jt->etxn<NETX){  // if we overran the buffer, don't reformat it.  Reformatting requires splitting to words
  // We displayed the sentence.  See if it contains (9 :'string'); if so, replace with {{ string }}
  fauxblock(fauxw); A z=(A)&fauxw;
  AK(z)=jt->etxinfo->etx+m1-(C*)z; AFLAGFAUX(z,0) AT(z)=LIT; ACFAUX(z,ACUC1) AR(z)=1; AN(z)=AS(z)[0]=jt->etxn-m1;  // point to etx for parsed line
  jtunDD((J)((I)jt+(JTINPLACEW|JTINPLACEA)),z);  // reformat in place
  jt->etxn=m1+AN(z);  // set new end-of-sentence pointer
 }
}    /* display error line */

// w is a list of words, originally from a single sentence.  Result is string form of the words.
// nflag is display status: bit0=space needed before numeric value, bit1=parens required around non-primitive
A jtunparse(J jt,A w,I nflag){A*v,z;
 ARGCHK1(w);
 jt->etxn=0;
 v=AAV(w); DO(AN(w), nflag=disp(QCWORD(v[i]),nflag);); z=str(jt->etxn,jt->etxinfo->etx);  // No C(): messages and sentences may contain LPAR/PYX
 jt->etxn=0;
 R z;
}

// Display DCCALL stack frame
static void jtseecall(J jt,DC d){A a;
 if(a=d->dca)ep(NAV(a)->n,NAV(a)->s); 
 efmt(d->dcx&&d->dcy?"[:"FMTI"]":"["FMTI"]",lnumsi(d));
}    /* display function line */

// display error-message line
static void jtdhead(J jt,C k,DC d){C s[]="    "; 
 s[0]=d&&d->dcsusp?'*':'|'; 
 ep(k+1L,s);
}    /* preface stack display line */

void jtdebdisp(J jt,DC d){A*x,y;I e,t;
 e=d->dcj;   // error #, or 0 if no error (if DCCALL or DCPARSE frame)
 t=d->dctype;
 if(e&&!jt->etxn&&(t==DCPARSE||t==DCCALL)){  // if error, and nothing formatted already, display error header
  dhead(0,0L); if(e<=NEVM){eputl(AAV(JT(jt,evm))[e]);}else{eputs("Application error "); C buf[10]; sprintf(buf,"%i",(US)e); eputs(buf); eputc(CLF);}
 }
 switch(t){
  case DCPARSE:  dhead(3,d); seeparse(d); if(NETX==jt->etxn)--jt->etxn; eputc(CLF); break;
  case DCCALL:   dhead(0,d); seecall(d);  eputc(CLF); break;
  case DCSCRIPT: dhead(0,d); efmt("[-"FMTI"] ", d->dcn);  // keep the line number as 1-origin since that's what editors do 
                 if(0<=d->dcm){READLOCK(JT(jt,startlock)) y=AAV(JT(jt,slist))[d->dcm]; ep(AN(y),CAV(y)); READUNLOCK(JT(jt,startlock))}
                 eputc(CLF); break;
}}

// display the current stack frame; if it is PARSE, also display the previous frame if it is SCRIPT or CALL with explicit definition
// This is where we format the error message that goes to display, 1 or 2 lines.
// Instead, when there is a failure outside of debug, jtxdefn & jtline, & possibly jtunquote (i. e. those places that would have had a CALL or SCRIPT frame)
// should append their error info as they fail up the line.
static B jtdebsi1(J jt,DC d){I t;
 RZ(d);
 t=d->dctype;
 debdisp(d);   // display the top frame, whatever it is
 RZ(t==DCPARSE&&d->dcm==0);  // continue if this is a PARSE frame not being executed for ". (dcm).  ". breaks the connection between PARSE and the preceding frame
 d=d->dclnk;  // advance to next frame if any
 RZ(d&&(d->dctype==DCSCRIPT||(d->dctype==DCCALL&&d->dcc)));  // continue if next frame is SCRIPT or CALL for explicit defn
 debdisp(d);
 R 1;
}

F1(jtdbstack){F12IP;DC d=jt->sitop; 
 ASSERTMTV(w);
 if(d){if(DCCALL!=d->dctype)d=d->dclnk; NOUNROLL while(d){debdisp(d); d=d->dclnk;}}
 R mtm;
}    /* 13!:1  display SI stack */

F1(jtdbstackz){F12IP;A y,z; 
 RE(dbstack(w)); 
 RZ(y=str(jt->etxn,jt->etxinfo->etx)); 
 jt->etxn=0; 
 R dfv1(z,y,cut(ds(CLEFT),num(-2)));
}    /* 13!:18  SI stack as result */

static A gahzap(J jt,I r,A w){RZ(w=gah(r,w)) ACINITUNPUSH(w); R w;}  // allocate header and ra()
// ratify the current emsg and call eformat_j_ to give a full message
// This is called from CALL[12] when we know the arguments and self.  The error code will have been signaled earlier
// self is the self for the failing entity; but if self is 0, a is a string to display for the error
// a is the x/u arg to the failing entity
// w is the y/n arg to the failing entity
// m is the m argument for adverbs
// the args to eformat_j_ are error#;curname;jt->ranks/empty if m};AR of self;a/AR(a)[;w/AR(w)}[;m]
// Result is always 0
A jteformat(J jtfg,A self,A a,A w,A m){F12IP;
  if(!(jt->emsgstate&EMSGSTATEFORMATTED+EMSGSTATENOEFORMAT)&&likely(self!=DUMMYSELF)){  // If we have already formatted, don't do it again.  If we expect error, return fast.  If we are called without a real self, we must be executing something internal.  Format it later when we have a real self
  C e=jt->jerr;
  if(e!=0 && e!=EVABORTEMPTY && e!=EVSTACK && e!=EVWSFULL){   // if no error, don't do it again.  Don't waste time on aborts.  If we have run out of memory or stack, don't call eformat, which will probably fail too
   if(!jt->glock){ // if we are locked, show nothing;
    A saverr;   // savearea for the initial message
    A *old=jt->tnextpushp;  // we must free all memory that we allocate here
    if((saverr=str(jt->etxn,jt->etxinfo->etx))!=0){  // save error code and message; if error in str, skip formatting
     A msg=0, m1ah=0, w1ah=0, a1ah=0; // indicate no formatted message; headers for the arguments, which we will delete before exit
    if(self){
      if(AT(self)!=0){   // if the self was FUNCTYPE0 eg, a placeholder, don't try to format with it
       // we are going to try to run eformat.
       // we have to reset the state of the error system after saving what we will need
       I pareninfo = (jt->emsgstate&EMSGSTATEPAREN)>>EMSGSTATEPARENX;  // unbalanced-paren info from infererrtok
       RESETERR; jt->emsgstate|=EMSGSTATEFORMATTED; // clear error system; indicate that we are starting to format, so that the error line will not be modified during eformat
       A nam; if((nam=nfs(10,"eformat_j_",0))==0)goto noeformat; A val; if((val=syrd(nam,jt->locsyms))==0)goto noeformat;
       if((val=QCWORD(namerefacv(nam,QCWORD(val))))==0)goto noeformat; if(!(val&&LOWESTBIT(AT(val))&VERB))goto noeformat;  // there is always a ref, but it may be to [:.   namerefacv will undo ra() in syrd
       // we also have to reset processing state: ranks.  It seems too hard to force eformat to infer the ranks from the args
       // other internal state (i. e. from !.n) will have been restored before we get here  
       // establish internal-state args: jt->ranks.
       A rnk; if((rnk=v2((I)(B)jt->ranks,(I)(B)(jt->ranks>>RANKTX)))==0)goto noeformat; // cell ranks
       RESETRANK;   //  We have to reset the rank before we call internal functions
       // if : name was formatted into the error message, extract the name (which might have been generated by the caller)
       A namestg=mtv; I i; for(i=0;i<AN(saverr)&&CAV(saverr)[i]!=':';++i)if(CAV(saverr)[i]==CLF)i=AN(saverr)-1; i+=2;
       if(i<AN(saverr)){I j; for(j=i;j<AN(saverr)&&CAV(saverr)[j]!=' '&&CAV(saverr)[j]!=CLF;++j); RZGOTO(namestg=str(j-i,CAV(saverr)+i),noeformat)}
       // we also have to isolate the user's a/w/m so that we do not disturb any flags or usecounts.  We build headers for the nouns
       // The headers are like virtual blocks but they don't increment the usecount of the backer.  That means that if further execution frees the backer
       // the header is left pointing to garbage.  To avoid trouble we zap the headers here and free them by hand after we call eformat
       // Since we can't gah() for sparse blocks, we abort eformatting if we encounter one.
       A awm=0;   // place to build the arg list for eformat
       if(m&&!ISSPARSE(AT(m))){A m1; rnk=mtv; if((m1=m1ah=gahzap(jt,AR(m),m))==0)goto noeformat; MCISH(AS(m1),AS(m),AR(m)) if((awm=box(m1))==0)goto noeformat;  // if m exists, make it the last arg, and set rank to ''
       }else if(e==EVASSEMBLY){
        // assembly errors are special.  They require an info vector, which has been stored in jt->etxinfo.  We pass this vector as m
        if((awm=box(vec(INT,jt->etxinfo->asseminfo.assemframelen+(offsetof(struct assem,assemshape)/sizeof(I)),&jt->etxinfo->asseminfo)))==0)goto noeformat;
       }
       // Convert self to AR.  If self is not a verb convert a/w to AR also
       I selft=AT(self);
       A selfar; if((selfar=arep(self))==0)goto noeformat;
       if(w&&!ISSPARSE(AT(w))&&((selft&CONJ)||(AT(w)&NOUN)))  // if w is valid
         {A w1=w; if(AT(w1)&NOUN){ if((w1=w1ah=gahzap(jt,AR(w),w))==0)goto noeformat; MCISH(AS(w1),AS(w),AR(w)) } if(!(selft&VERB))if((w1=arep(w1))==0)goto noeformat; if((awm=awm?jlink(w1,awm):box(w1))==0)goto noeformat;}
       if(a&&!ISSPARSE(AT(a))){A a1=a; if(AT(a1)&NOUN){if((a1=a1ah=gahzap(jt,AR(a),a))==0)goto noeformat; MCISH(AS(a1),AS(a),AR(a))} if(!(selft&VERB))if((a1=arep(a1))==0)goto noeformat; if((awm=awm?jlink(a1,awm):box(a1))==0)goto noeformat;}
       // run the analyzer.  Fold the unbalanced-paren info into the error number
       deba(DCJUNK,0,0,0);  // create spacer frame so eformat calls don't overwrite stack
      WITHDEBUGOFF(msg=jtunquote(jt,jlink(sc(e|(pareninfo<<8)),jlink(namestg,jlink(rnk,jlink(selfar,awm)))),val,val););   // run eformat_j_
       debz();
      }
     }else msg=a;  // self not given, use given message text
 noeformat: ;
     if(m1ah)fa(m1ah); if(w1ah)fa(w1ah); if(a1ah)fa(a1ah);  // free the headers, which are orphans
     jt->jerr=jt->jerr1=e; jt->etxn=0;
     C *savtext=CAV(saverr); I copyoffset=0;  // pointer to old emsg text, and offset to copy from
     if(msg&&(AT(msg)&LIT)&&AN(msg)>0){
      // eformat_j_ returned a message.  Replace the first line of saverr with it UNLESS self=0: then insert it after the first line of saverr
      DO(AN(saverr), if(savtext[i]==CLF){copyoffset=i; break;})  // count up to but not including including first LF
      if(self==0){ep(copyoffset+1,savtext);}  // if internal msg, it is terse-msg LF internal-msg LF remaining lines
      I waslf=1; C *msgtext=CAV(msg); DO(AN(msg) , if(waslf)dhead(0,0L); eputc(msgtext[i]); waslf=msgtext[i]==CLF;)  // copy out message if any; every nonempty line must start with the suspension/error prefix * or |
     }
     if(AN(saverr)==0)eputc(CLF);else ep(AN(saverr)-copyoffset,savtext+copyoffset);  // copy out the rest (or all) of the original message, plus trailing LF; but if the message was never formatted (AN=0), just add LF
     jt->etxn1=jt->etxn;
    }
    // We have moved the message, including the saved one, into the message buffer.  OK to pop now
    DC savpm=jt->pmstacktop; jt->pmstacktop=0; tpop(old); jt->pmstacktop=savpm;   // We MUST pop what we did here because the headers contain unprotected pointers that are wiped out by symfreeha
   }
   // some errors are distinguished internally to make eformat_j_ easier.  We revert them to the normal message after eformatting
   C oe=e; e=oe==EVINHOMO?EVDOMAIN:e; e=oe==EVINDEXDUP?EVINDEX:e; e=oe==EVEMPTYT?EVCTRL:e; e=oe==EVEMPTYDD?EVCTRL:e; e=oe==EVMISSINGGMP?EVFACE:e;
   e=oe==EVSIDAMAGE?EVSTACK:e; e=oe==EVASSEMBLY?EVDOMAIN:e; e=oe==EVTYPECHG?EVDOMAIN:e;  // revert internal numbers to external codes after formatting 

   jt->jerr=jt->jerr1=e;  // save reverted value
  }
  jt->emsgstate|=EMSGSTATEFORMATTED;  // indicate formatting attempted even if we skipped it
 }
 R 0;
}

// common error-analysis-and-display entry point
// e is the error number plus flags
// this routine also does the work for jtjsigd (domain error with message text), jtjsignal (old-fashioned terse display),
// jtjsignal3 (terse display for lines known to be from an explicit definition, giving line#/column), and
// x 13!:8 y (display text x, set error# y) 
//
// we look at emsgstate and do as little as possible if the user isn't going to see the message
//
// return value is always 0
A jtjsignale(J jt,I eflg,A line,I info){
 // if a message has already been stored, ignore any subsequent one.  This should happen only in code that generates the same error in an inner loop over data
 if(likely(jt->jerr==0)){
  if(unlikely(!(jt->emsgstate&EMSGSTATEFORMATTED))){   // if not first error or formatting the line is in progress, ignore altogether.  Also used when we expect an error and don't need jt->jerr set, which is why the paradoxical unlikely
   C e=eflg&EMSGE; jt->jerr1=jt->jerr=jt->jerrraw=e;   // extract error# & save it.  Even if eformat is running we need to set the error to cut off invalid internal paths.  jerr1 is for later sentences, jerrraw for more detail about this sentence
   if(jt->etxn>=0){  // if the error line is frozen, don't touch it
    jt->etxn=0;  // clear error-message area indicating message not installed yet
    // if the user will never see the error, exit without further ado - keeps u :: v fast
    if(!(jt->emsgstate&EMSGSTATENOTEXT) && (eflg&EMSGLINEISA || BETWEENC(e,1,NEVM))){  // message text not suppressed and not internal-only (but not e=0, which is sigd): the number is all we need, skip the rest of the processing
     // we will format for display.  *** STOP HERE to stop on unmasked error <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     if(e!=EVSTOP)moveparseinfotosi(jt);  // before we display, move error info from parse variables to si; but if STOP, it's already installed
     // if debug is set, turn it off, with message, if there is not enough memory to run it
     if((jt->uflags.trace&TRACEDB)&&!spc()){eputs("ws full (can not suspend)"); eputc(CLF); jt->uflags.trace&=~TRACEDB;}  // spc sees that you can malloc 1000 bytes
     // format the message lines according to the various types of call
     dhead(0,0L);  // display suspension/error prefix: *      or |     
     // start with terse message [: name]
     A msg=eflg&EMSGLINEISTERSE?line:AAV(JT(jt,evm))[e];  // jsignal/jsignal3/13!:8 dyad.  Use the terse string except for 13!:8
     jteputlnolf(jt,msg);  // header of first line: terse string
     // Decide what name, if any, to show.  By default, the name of the executing entity, but if no name has started after the last suspension,
     // it means the user has typed a failing sentence like (1 + 'a') into suspension: it is not part of the suspended name and should not be so decorated.
     // If there is no suspension, we type the name, but this is trickier than it appears: during superdebug we may turn off debug while a SCRIPT type is
     // supplying the sentence lines, and that script will keep running, preventing the clear-suspension message from being passed up the line to clear
     // the debug stack.  
     DC d; for(d=jt->sitop;d;d=d->dclnk){if(d->dctype==DCSCRIPT)break; if(d->dcsusp)break; if(d->dctype==DCCALL)break; }  // stop on EOC, SCRIPT/CALL, or suspension
     A nameblok=(d&&d->dctype!=DCCALL&&d->dcsusp)?0:jt->curname;  // use name unless we reached suspension without seeing a CALL
     // Value error, which may come up after the stack is long gone, sets EMSGLINEISNAME to override the name
     nameblok=eflg&EMSGLINEISNAME?line:nameblok;  // if caller overrides the name, use the caller's name
     if(nameblok){if(!jt->glock){eputs(": "); ep(NAV(nameblok)->n,NAV(nameblok)->s);}}  // ...followed by name of running entity
     if(eflg&EMSGFROMPYX)eputs(" (from pyx)");   // if the message came from a pyx, mark it as such
     eputc(eflg&EMSGSPACEAFTEREVM?' ':CLF);  // ... that's the first line, unless user wants added text on the same line
     if(!jt->glock){  // suppress detail if locked
     if((line!=0) && !(eflg&EMSGNOMSGLINE) && !(jt->emsgstate&EMSGSTATENOLINE)){  // if there is a user line, and its display not suppressed
       // display the message in line, according to its mode
       C *text; I textlen;
       if(eflg&EMSGLINEISA){text=CAV(line); textlen=AN(line);}else{text=(C*)line; textlen=info;}  // addr/len of data to type
       if(eflg&EMSGINVCHAR){eputs("(invalid character in sentence, code unit "); efmt("%d)\n",(I)text[info]);}  // if invalid character, show the value
       if(eflg&EMSGINVINFL){eputs("(invalid inflection)\n");}  // if invalid inflection, say so
       if(eflg&EMSGCXINFO){ // if line has decoration from tokenizing
        dhead(3,0L);  // start with error header
        if(e==EVCTRL)efmt("["FMTI"]",info);  // control error during explicit definition: prefix with line#
       }
       ep(textlen,text); eputc(CLF); // out the message text terminated by LF
       if((eflg&EMSGCXINFO) && !(e==EVCTRL)){
        dhead(3,0L); DQ(info, eputc(' ');); eputc('^'); eputc(CLF);  // third line is ^ indicating error location
       }
      }
      // last line is the failing line, with spaces before the error point
      debsi1(jt->sitop);
      // if the line was echoed from a pyx, append the message for the pyx
      if(eflg&EMSGFROMPYX){eputs("emsg in pyx:\n"); ep(AN(line),CAV(line)); eputc(CLF);}  // finish with the original message
     }
    }
    jt->etxn1=jt->etxn;  // save length of finished message
   }
   // if this error was forwarded from a pyx or 13!:8, we can't eformat it - we have no self/arguments.  Set that we have tried formatting already to suppress further formatting
   if(unlikely(eflg&EMSGNOEFORMAT))jt->emsgstate|=EMSGSTATEFORMATTED;
  }
 }
 R 0;
}

// here for errors coming from explicit definition
A jtjsignal(J jt,I e){A x;
 R jtjsignale(jt,e,0,0);  // just the error
}

void jtjsignal2(J jt,I e,A dummy){jtjsignal(jt,e);}  // used in unquote to reschedule instructions

// display the failing sentence. e=error message#, w holds sentence text, j is column# of error.  Returns 0
A jtjsignal3(J jt,I e,A w,I j){
 R jtjsignale(jt,e|EMSGLINEISA+EMSGCXINFO,w,j);
}    /* signal error e on line w with caret at j */

A jtjsigd(J jt,C*s){
R jtjsignale(jt,EVDOMAIN|EMSGSPACEAFTEREVM,(A)s,strlen(s));
}

// display for 13!:8
static F2(jtdbsig){F12IP;I e;
 RE(0);
 if(!AN(w))R mtm;
 RZ(w=vi(w)); e=AV(w)[0]; 
 ASSERT(1<=e,EVDOMAIN);
 ASSERT(e<=255,EVLIMIT);
 if(a==ds(CACE)){RESETERR; jt->jerr=e; jt->emsgstate|=EMSGSTATEFORMATTED;  // secret debug trick: a: sets err# but no msg, suppresses eformat
 }else{
  jt->emsgstate&=~(EMSGSTATENOTEXT|EMSGSTATENOLINE|EMSGSTATEFORMATTED);  // user's message overrides anything that was given before; turn off ignore bits to ensure we process it
  if(a||e>NEVM){if(!a)a=mtv; RZ(a=vs(a)); jtjsignale(jt,e|EMSGLINEISA+EMSGLINEISTERSE+EMSGNOEFORMAT,a,0);} else jsignal(e|EMSGNOEFORMAT);  // must not run eformat, since self does not apply
 }
 R 0;
}    

F1(jtdbsig1){F12IP;R dbsig(0L,w);}   /* 13!:8  signal error */
F2(jtdbsig2){F12IP;R dbsig(a, w);}

// 9!:59 set emsgslvl, return previous
DF1(jtemsglevel){F12IP;
 if(!AN(w))R mtm;
 RZ(w=vi(w)); I e=AV(w)[0]; 
 ASSERT(e<=7,EVDOMAIN);  // must be integer in range 0-7
 I ostate=jt->emsgstate; jt->emsgstate=(jt->emsgstate&~(EMSGSTATENOTEXT|EMSGSTATENOLINE|EMSGSTATENOEFORMAT))|e;
 R sc(ostate);
}


F1(jtdberr){F12IP;ASSERTMTV(w); R sc(jt->jerr1);}           /* 13!:11 y  last error number   */
// 13!:12 last error text.  If there is no error, show no text.  If there is an error with no text, we must have suppressed
// loading the terse message; return it now
F1(jtdbetx){F12IP;
 ASSERTMTV(w);
 if(jt->jerr1==0)R mtv;  // if no error, no text either
 if(jt->etxn1==0 && BETWEENC(jt->jerr1,1,NEVM))R AAV(JT(jt,evm))[jt->jerr1];  // no text, supply it now
 R str(jt->etxn1,jt->etxinfo->etx);  // leave the text that's there
}


A jtjerrno(J jt){
#if !SY_WINCE
 switch(errno){
 case EMFILE:
 case ENFILE: jsignal(EVLIMIT  ); R 0;
 case ENOENT: jsignal(EVFNAME  ); R 0;
 case EBADF:  jsignal(EVFNUM   ); R 0;
 case EACCES: jsignal(EVFACCESS); R 0;
#else /* WINCE: */
 switch(GetLastError()){
 case ERROR_DISK_FULL:
 case ERROR_FILENAME_EXCED_RANGE:
 case ERROR_NO_MORE_FILES:
 case ERROR_NOT_ENOUGH_MEMORY:
 case ERROR_NOT_ENOUGH_QUOTA:
 case ERROR_TOO_MANY_OPEN_FILES:    jsignal(EVLIMIT  ); R 0;
 case ERROR_BAD_PATHNAME:
 case ERROR_INVALID_NAME:           jsignal(EVDOMAIN ); R 0;
 case ERROR_ALREADY_EXISTS:
 case ERROR_FILE_EXISTS:
 case ERROR_PATH_NOT_FOUND:
 case ERROR_FILE_NOT_FOUND:         jsignal(EVFNAME  ); R 0;
 case ERROR_ACCESS_DENIED:
 case ERROR_WRITE_PROTECT:
 case ERROR_SHARING_VIOLATION:      jsignal(EVFACCESS); R 0;
#endif
 default:     jsignal(EVFACE); R 0;
 }
}   /* see <errno.h> / <winerror.h> */
