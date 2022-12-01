/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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
 if(0<m){MC(jt->etx+jt->etxn,s,m); jt->etxn+=m;}
}

static void jteputs(J jt,C*s){ep((I)strlen(s),s);}

static void jteputc(J jt,C c){ep(1L,&c);}

static void jteputlnolf(J jt,A w){ep(AN(w),CAV(w));}
static void jteputl(J jt,A w){jteputlnolf(jt,w); eputc(CLF);}

static void jteputv(J jt,A w){I m=NETX-jt->etxn; if(m>0){jt->etxn+=thv(w,MIN(m,200),jt->etx+jt->etxn);}} // stop writing when there is no room in the buffer
     /* numeric vector w */

static void jteputq(J jt,A w,I nflag){C q=CQUOTE,*s;
 if(equ(ds(CALP),w))eputs(" a."+!(nflag&1));
 else{
  eputc(q);
  s=CAV(w); DO(AN(w), eputc(s[i]); if(q==s[i])eputc(q););
  eputc(q);
}}   /* string w, possibly with quotes */

static void jtefmt(J jt,C*s,I i){
 if(15<NETX-jt->etxn){C*v=jt->etx+jt->etxn; sprintf(v,s,i); jt->etxn+=strlen(v);}
}

// jt has the typeout flags.  Display error text if any, then reset errors
void jtshowerr(J jt){F1PREFJT;C b[1+2*NETX],*p,*q,*r;
 if(jt->etxn&&!((I)jtinplace&JTPRNOSTDOUT)){  // if there is a message and it is not suppressed...
  p=b; q=jt->etx; r=q+jt->etxn;
  NOUNROLL while(q<r&&p<b+2*NETX-3){if(*q==CLF){strcpy(p,OUTSEQ); p+=strlen(OUTSEQ); ++q;}else *p++=*q++;}  // avoid buffer overrun on huge typeouts
  *p=0;
#ifdef ANDROID
  A z=tocesu8(str(strlen(b),b));
  CAV(z)[AN(z)]=0;
  jsto(JJTOJ(jt),MTYOER,CAV(z));
#else
  jsto(JJTOJ(jt),MTYOER,b);
#endif
 }
 RESETERR
// obsolete  jt->etxn=0;
}

static I jtdisp(J jt,A w,I nflag);
// format one non-noun entity into the error line
// nflags contains display flags: 1=space before number, 2=parens around non-primitive
// The entity came from a single sentence but may be compound.  Display it on a single line
static void jtdspell(J jt,C id,A w,I nflag){C c,s[5];
 // constant verbs require looking at h
 if(id==CFCONS){if((nflag&1))eputc(' '); eputv(FAV(w)->fgh[2]); eputc(':');}
 else{
  // get fgh if any.  Format f if any, then the primitive, then g if any.  fgh are present only in ACV type (ASGN doesn't have them at all)
  A f,g,h;
  if(AT(w)&VERB+ADV+CONJ){f=FAV(w)->fgh[0], g=id==CBOX?0:FAV(w)->fgh[1], h=FAV(w)->fgh[2];}else{f=g=h=0;}  // plain value for fgh; ignore g field in BOX, which is there to resemble <@]
  if(id==CFORK){if(h==0){h=g; g=f; f=ds(CCAP);}}else h=0;  // reconstitute [: g h; otherwise we display h only for fork
  if(g&&!f){f=g; g=0;}  // if adverb has its arg in g rather than f, restore it to f
  // if this is not a primitive, we must enclose it in parentheses if we are told to in nflag or if it was originally defined as a hook/fork which must have used parentheses
  I invisiblemod=BETWEENC(id,CHOOK,CADVF);  // true if hook/fork/advf
  I parenhere=(g||h)&&(invisiblemod||nflag&2);  // set if we need parens around our value
  if(parenhere)eputc('(');
  if(f)nflag=disp(f,0);  // display left side if any
  // display the primitive, with a leading space if it begins with inflection or a digit.  Don't display the code for an invisible modifier - that's used only for ARs
  if(!invisiblemod){
   s[0]=' '; s[4]=0;
   spellit(id,1+s);
   c=s[1]; 
   eputs(s+!(c==CESC1||c==CESC2||(nflag&1)&&((ctype[(UC)c]&~CA)==0)));
  }
  if(g)nflag=disp(g,2);  // display right side if any
  if(h)nflag=disp(h,2);  // display end of fork/trident if any
  if(parenhere)eputc(')');
 }
}

static F1(jtsfn0){R sfn(0,w);}  // return string form of full name for a NAME block
EVERYFS(sfn0overself,jtsfn0,jtover,0,VFLAGNONE)

// print a noun; nflag if space needed before name/numeric; return new value of nflag
static I jtdisp(J jt,A w,I nflag){B b=1&&AT(w)&NAME+NUMERIC;
 // if this is a noun from a (( )) block, we have to take its linear rep, since it might not be displayable in 1 line
 if(AFLAG(w)&AFDPAREN&&AT(w)&NOUN){
  // linear rep may fail, or parts of it may fail; so we must reset errors.  We set etxn neg to indicate that the error line is frozen
  eputc('('); eputc('('); I se=jt->jerr; jt->jerr=0; I sn=jt->etxn; jt->etxn=-sn; w=lrep(w); jt->jerr=se; jt->etxn=sn; RZ(w); ep(AN(w),CAV(w)); eputc(')'); eputc(')');  // out the lin rep
  R 0;  // new nflag - none since we added )
 }
 // If this is a PPPP, enclose it in ()
 if(AFLAG(w)&AFDPAREN)eputc('(');  // leading ( of PPPP
 if(b&&(nflag&1))eputc(' ');
 switch(CTTZ(AT(w))){
 case B01X:
 case INTX:
 case FLX:
 case CMPXX: 
 case XNUMX: 
 case RATX:  eputv(w);                break;
 case BOXX:
  if(!(AT(w)&BOXMULTIASSIGN)){eputs(" a:"+!(nflag&1)); break;}
  // If this is an array of names, turn it back into a character string with spaces between
  else{w=curtail(raze(every2(every(w,(A)&sfn0overself),chrspace,(A)&sfn0overself)));}  // }: (string&.> names) ,&.> ' '  then fall through to display it
 case LITX:  eputq(w,(nflag&1));                break;
 case NAMEX: ep(AN(w),NAV(w)->s); if(unlikely((AT(w)&NAMEABANDON)!=0)){ep(2,"_:");}     break;
 case LPARX: eputc('(');              break;
 case RPARX: eputc(')');              break;
 case ASGNX: dspell(CAV(w)[0],w,(nflag&1));       break;
 case MARKX:                          break;
 default:   dspell(FAV(w)->id,w,(nflag&1)|(AFLAG(w)&AFDPAREN?0:2));     break;  // force parens on non-primitive if not PPPP
 }
 if(AFLAG(w)&AFDPAREN)eputc(')');  // trailing ) of PPPP
 R b;  // new nflag
}

// display DCPARSE stack frame
static void jtseeparse(J jt,DC d){A*v;I m;
 v=(A*)d->dcy;  /* list of tokens */
 m=d->dcix-1;         /* index of active token when error found */
 I nflag=0;
 I m1=jt->etxn;  // starting index of sentence text
 DO(d->dcn, if(i==m)eputs("    "); nflag=disp(QCWORD(v[i]),nflag););  // display tokens with spaces before error
 if(jt->etxn<NETX){  // if we overran the buffer, don't reformat it.  Reformatting requires splitting to words
  // We displayed the sentence.  See if it contains (9 :'string'); if so, replace with {{ string }}
  fauxblock(fauxw); A z=(A)&fauxw;
  AK(z)=jt->etx+m1-(C*)z; AFLAGFAUX(z,0) AT(z)=LIT; ACFAUX(z,ACUC1) AR(z)=1; AN(z)=AS(z)[0]=jt->etxn-m1;  // point to etx for parsed line
  jtunDD((J)((I)jt|JTINPLACEW|JTINPLACEA),z);  // reformat in place
  jt->etxn=m1+AN(z);  // set new end-of-sentence pointer
 }
}    /* display error line */

// w is a list of words, originally from a single sentence.  Result is string form of the words.
// nflag is display status: bit0=space needed before numeric value, bit1=parens required around non-primitive
A jtunparse(J jt,A w,I nflag){A*v,z;
 ARGCHK1(w);
 jt->etxn=0;
 v=AAV(w); DO(AN(w), nflag=disp(QCWORD(v[i]),nflag);); z=str(jt->etxn,jt->etx);
 jt->etxn=0;
 R z;
}

// Display DCCALL stack frame
static void jtseecall(J jt,DC d){A a;
 if(a=d->dca)ep(AN(a),NAV(a)->s); 
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
 if(e&&!jt->etxn&&(t==DCPARSE||t==DCCALL)){x=e+AAV(JT(jt,evm)); dhead(0,0L); eputl(*x);}  // if error, display error header
 switch(t){
  case DCPARSE:  dhead(3,d); seeparse(d); if(NETX==jt->etxn)--jt->etxn; eputc(CLF); break;
  case DCCALL:   dhead(0,d); seecall(d);  eputc(CLF); break;
  case DCSCRIPT: dhead(0,d); efmt("[-"FMTI"] ", d->dcn);  // keep the line number as 1-origin since that's what editors do 
                 if(0<=d->dcm){READLOCK(JT(jt,startlock)) y=AAV(JT(jt,slist))[d->dcm]; ep(AN(y),CAV(y)); READUNLOCK(JT(jt,startlock))}
                 eputc(CLF); break;
}}

static B jtdebsi1(J jt,DC d){I t;
 RZ(d);
 t=d->dctype;
 debdisp(d); 
 d=d->dclnk;
 RZ(d&&t==DCPARSE);
 t=d->dctype;
 RZ(t==DCSCRIPT||t==DCCALL&&d->dcloc);
 debdisp(d);
 R 1;
}

F1(jtdbstack){DC d=jt->sitop; 
 ASSERTMTV(w);
 if(d){if(DCCALL!=d->dctype)d=d->dclnk; NOUNROLL while(d){debdisp(d); d=d->dclnk;}}
 R mtm;
}    /* 13!:1  display SI stack */

F1(jtdbstackz){A y,z; 
 RE(dbstack(w)); 
 RZ(y=str(jt->etxn,jt->etx)); 
 jt->etxn=0; 
 R df1(z,y,cut(ds(CLEFT),num(-2)));
}    /* 13!:18  SI stack as result */

#if 0
// here for errors not from explicit definition
// explicit errors also come here, so stop here to see the point at which error was detected
static void jtjsigstr(J jt,I e,I n,C*s){
 if(jt->jerr){jt->curname=0; R;}   // if not first error, trash diagnostic information and bail out
 jt->jerr=(C)e; jt->jerr1=(C)e; if(jt->etxn<0)R;  // remember error for testing, but if the error line is frozen, don't touch it
 if(e!=EVSTOP)moveparseinfotosi(jt); jt->etxn=0;  // before we display, move error info from parse variables to si; but if STOP, it's already installed
 dhead(0,0L);  // | left-header for the error line
 if((jt->uflags.trace&TRACEDB)&&!spc()){eputs("ws full (can not suspend)"); eputc(CLF); jt->uflags.trace&=~TRACEDB;}
 ep(n,s);
 if(jt->curname){if(!jt->glock){eputs(": "); ep(AN(jt->curname),NAV(jt->curname)->s);} jt->curname=0;}
 eputc(CLF);
 if(n&&!jt->glock)debsi1(jt->sitop);
 jt->etxn1=jt->etxn;
}    /* signal error e with error text s of length n */ 

static void jtjsig(J jt,I e,A x){jsigstr(e,AN(x),CAV(x));}
     /* signal error e with error text x */ 

#endif

#if 0 // obsolete 
// like jtjsignal, but with more detailed error message
// format: cf printf
// %%   literal %
// %i   I
// %5i  I(*)[5]
// %*i  I(*)[I] (length, then ptr)
// %t   type (nonspecific; eg 'number', not 'integer') as I
void jtjsignalf(J jt,I e,C *fmt,...){
 va_list ap;va_start(ap,fmt);
 C buf[1024],*bp=buf,*be=buf+1000; //bufend leave a bit of scratch space to simplify logic
 {A x=AAV(JT(jt,evm))[e];MC(bp,CAV(x),AN(x));bp+=AN(x);*bp++=':';*bp++=' ';}
 while(bp<be&&*fmt){
  //could be faster with simd scanning, but who cares?
  if(*fmt!='%'){*bp++=*fmt++;continue;}
  fmt++;
  if(*fmt=='%'){*bp++='%';fmt++;continue;}
  if(*fmt=='t'){
   fmt++;
   static const char *ntt[]={[B01X]="number",[INTX]="number",[FLX]="number",[CMPXX]="number",[XNUMX]="number",[RATX]="number",
                             [BOXX]="box",[PYXX]="box",
                             [SBTX]="symbol",
                             [LITX]="literal",[C2TX]="literal",[C4TX]="literal"};
   bp+=snprintf(bp,be-bp,"%s",ntt[CTTZ(va_arg(ap,I))]);
   continue;}
  I l=-1;
  if(BETWEENC(*fmt,'0','9')){for(l=0;BETWEENC(*fmt,'0','9');fmt++)l*=10,l+=*fmt-'0';}
  else if(*fmt=='*'){l=va_arg(ap,I);*fmt++;}
  switch(*fmt++){
   case 'i':;
    I *p,it;
    if(l<0){ it=va_arg(ap,I);p=&it;l=1; }
    else{ p=va_arg(ap,I*); }
    DO(l,if(bp>=be)break;
         I pi=p[i];
         *bp=' ';bp+=!!i;     // space between elements (other than the first)
         *bp='_';bp+=pi<0;
         pi=ABS(pi);
         if(!pi){*bp++='0';}
         else{I pil=0,tpi=pi;while(tpi>=1000)pil+=3,tpi/=1000;while(tpi)pil++,tpi/=10;
              C *nbp=bp+=pil;
              while(pi)*--nbp='0'+pi%10,pi/=10;})
    break;
   default:SEGFAULT;}}
 jsigstr(e,bp-buf,buf);}
#else
// ratify the current emsg and call eformat_j_ to give a full message
// This is called from CALL[12] when we know the arguments and self.  The error code will have been signaled earlier
// self is the self for the failing entity; but if self is 0, a is a string to display for the error
// a is the x/u arg to the failing entity
// w is the y/n arg to the failing entity
// m is the m argument for adverbs
// the args to eformat_j_ are error#;curname;jt->ranks/empty if m};AR of self;a/AR(a)[;w/AR(w)}[;m]
// Result is always 0
A jteformat(J jt,A self,A a,A w,A m){
 F1PREFIP;
 C e=jt->jerr;
 if(e!=0 && !jt->emsgstate&EMSGSTATEFORMATTED){   // if no error, or we have already run eformat on this error, don't do it again
  if(!jt->glock && !(jt->emsgstate&EMSGSTATENOEFORMAT)){ // if we are locked, show nothing; if eformat suppressed, leave the error line as is
   A msg=0;  // indicate no formatted message
   A saverr; if((saverr=str(jt->etxn,jt->etx))!=0){  // save error code and message; if error in str, skip formatting
    if(self){
     if(AT(self)!=0){   // if the self was FUNCTYPE0 eg, a placeholder, don't try to format with it
      // we are going to try to run eformat.
      // we have to reset the state of the error system after saving what we will need
      RESETERR; jt->emsgstate|=EMSGSTATEFORMATTED; // clear error system; indicate that we are starting to format, so that the error line will not be modified during eformat
      A nam=nfs(10,"eformat_j_"); A val; if((val=syrd(nam,jt->locsyms))==0)goto noeformat; if((val=QCWORD(namerefacv(nam,val)))==0)goto noeformat;
      if(!(val&&LOWESTBIT(AT(val))&VERB))goto noeformat;  // there is always a ref, but it may be to [:.  Undo ra() in syrd
      // we also have to reset processing state: ranks.  It seems too hard to force eformat to infer the ranks from the args
      // other internal state (i. e. from !.n) will have been restored before we get here
      // establish internal-state args: jt->ranks.
      A rnk; if((rnk=v2((I)(B)jt->ranks,(I)(B)(jt->ranks>>RANKTX)))==0)goto noeformat; // cell ranks
      RESETRANK;   //  We have to reset the rank before we call internal functions
      A namestg=mtv; if(jt->curname!=0)RZGOTO(namestg=str(AN(jt->curname),NAV(jt->curname)->s),noeformat)
      // we also have to isolate the user's a/w/m so that we do not disturb any flags or usecounts.  We build headers for the nouns
      A awm=0; // where we build the a/w/m arguments
      if(m){A m1; rnk=mtv; if((m1=gah(AR(m),m))==0)goto noeformat; MCISH(AS(m1),AS(m),AR(m)) if((awm=box(m1))==0)goto noeformat;}  // if m exists, make it the last arg, and set rank to ''
      if(w&&((AT(self)&CONJ)||(AT(w)&NOUN)))  // if w is valid
       {A w1=w; if(AT(w1)&NOUN){if((w1=gah(AR(w),w))==0)goto noeformat; MCISH(AS(w1),AS(w),AR(w))} if(!(AT(self)&VERB))if((w1=arep(w1))==0)goto noeformat; if((awm=awm?jlink(w1,awm):box(w1))==0)goto noeformat;}
      if(a){A a1=a; if(AT(a1)&NOUN){if((a1=gah(AR(a),a))==0)goto noeformat; MCISH(AS(a1),AS(a),AR(a))} if(!(AT(self)&VERB))if((a1=arep(a1))==0)goto noeformat; if((awm=awm?jlink(a1,awm):box(a1))==0)goto noeformat;}
      // Convert self to AR.  If self is not a verb convert a/w to AR also
      A selfar; if((selfar=arep(self))==0)goto noeformat;
      // run the analyzer
      WITHDEBUGOFF(df1(msg,jlink(sc(e),jlink(namestg,jlink(rnk,jlink(selfar,awm)))),val);)  // run eformat_j_
     }
    }else msg=a;  // self not given, use given message text
noeformat: ;
    jt->jerr=jt->jerr1=e; jt->etxn=0;
    C *savtext=CAV(saverr); I copyoffset=0;  // pointer to old emsg text, and offset to copy from
    if(msg&&(AT(msg)&LIT)&&AN(msg)>0){
     // eformat_j_ returned a message.  Replace the first line of saverr with it UNLESS self=0: then insert it after the first line of saverr
     DO(AN(saverr), if(savtext[i]==CLF){copyoffset=i; break;})  // count up to but not including including first LF
     if(self==0){ep(copyoffset+1,savtext);}  // if internal msg, it is terse-msg LF internal-msg LF remaining lines
     I waslf=1; C *msgtext=CAV(msg); DO(AN(msg) , if(waslf)dhead(0,0L); eputc(msgtext[i]); waslf=msgtext[i]==CLF;)  // copy out message if any; every nonempty line must start with the suspension/error prefix * or |
    }
    ep(AN(saverr)-copyoffset,savtext+copyoffset);  // copy out the rest (or all) of the original message, plus trailing LF
    jt->etxn1=jt->etxn;
   }
  }
  // some errors are distinguished internally to make eformat easier.  We revert them to the normal message after eformatting
  e=e==EVINHOMO?EVDOMAIN:e; e=e==EVINDEXDUP?EVINDEX:e; e=e==EVEMPTYT?EVCTRL:e; e=e==EVEMPTYDD?EVCTRL:e;  // revert internal numbers to external codes after formatting

  jt->jerr=jt->jerr1=e;  // save reverted value
 }
 jt->emsgstate|=EMSGSTATEFORMATTED;  // indicate formatting attempted even if we skipped it
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
 // if a message has already been stored, ignore any subsequent one
 if(jt->jerr==0 && !(jt->emsgstate&EMSGSTATEFORMATTED)){   // if not first error or formatting the line is in progress, ignore: clear error-name and continue
  C e=eflg&EMSGE;   // extract error#
  // store the error message#
  jt->jerr=e; jt->jerr1=e;   // remember error for testing
  if(jt->etxn>=0){  // if the error line is frozen, don't touch it
   jt->etxn=0;  // clear error-message area indicating message not installed yet
   // if the user will never see the error, exit without further ado - keeps u :: v fast
   if(!(jt->emsgstate&EMSGSTATENOTEXT) && (eflg&EMSGLINEISA || BETWEENC(e,1,NEVM))){  // message text not suppressed and not internal-only (but not e=0, which is sigd): the number is all we need, skip the rest of the processing
    // we will format for display
    if(e!=EVSTOP)moveparseinfotosi(jt);  // before we display, move error info from parse variables to si; but if STOP, it's already installed
    // if debug is set, turn it off, with message, if there is not enough memory to run it
    if((jt->uflags.trace&TRACEDB)&&!spc()){eputs("ws full (can not suspend)"); eputc(CLF); jt->uflags.trace&=~TRACEDB;}  // spc sees that you can malloc 1000 bytes
     // format the message lines according to the various types of call
     dhead(0,0L);  // display suspension/error prefix: *      or |     
     // start with terse message [: name]
     A msg=eflg&EMSGLINEISTERSE?line:AAV(JT(jt,evm))[e];  // jsignal/jsignal3/13!:8 dyad.  Use the terse string except for 13!:8
     jteputlnolf(jt,msg);  // header of first line: terse string
     A nameblok=jt->curname; nameblok=eflg&EMSGLINEISNAME?line:nameblok;  // if user overrides the name, use the user's name
     if(nameblok){if(!jt->glock){eputs(": "); ep(AN(nameblok),NAV(nameblok)->s);}}  // ...followed by name of running entity
     if(eflg&EMSGFROMPYX)eputs(" (from pyx)");   // if the message came from a pyx, mark it as such
     eputc(eflg&EMSGSPACEAFTEREVM?' ':CLF);  // ... that's the first line, unless user wants added text on the same line
     if(!jt->glock){  // suppress detail if locked
      if((line!=0) && !(eflg&EMSGLINEISTERSE) && !(jt->emsgstate&EMSGSTATENOLINE)){  // if there is a user line, and its display not suppressed
       // display the message in line, according to its mode
       C *text; I textlen;
       if(eflg&EMSGLINEISA){text=CAV(line); textlen=AN(line);}else{text=(C*)line; textlen=info;}  // addr/len of data to type
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
     }
    }
    jt->etxn1=jt->etxn;  // save length of finished message
   }
  }
  // if this error was forwarded from a pyx or 13!:8, we can't eformat it - we have no self/arguments.  Set that we have tried formatting already to suppress further formatting
  if(eflg&EMSGNOEFORMAT)jt->emsgstate|=EMSGSTATEFORMATTED;
// obsolete    jt->curname=0;  // clear the name always
 R 0;
}
#endif


// here for errors coming from explicit definition
A jtjsignal(J jt,I e){A x;
#if 0
// template for debug break point
// if(EVDOMAIN==e){
// fprintf(stderr,"domain error\n");
// }
 // Errors > NEVM are internal-only errors that should never make it to the end of execution.
 // Ignore them here - they will not be displayed
 x=BETWEENC(e,1,NEVM)?AAV(JT(jt,evm))[e]:mtv; jsigstr(e,AN(x),CAV(x));
#else
 R jtjsignale(jt,e,0,0);  // just the error
#endif
}

void jtjsignal2(J jt,I e,A dummy){jtjsignal(jt,e);}  // used in unquote to reschedule instructions

// display the failing sentence. e=error message#, w holds sentence text, j is column# of error.  Returns 0
A jtjsignal3(J jt,I e,A w,I j){
#if 0 // obsolete
 if(jt->jerr)R; 
 jt->jerr=(C)e; jt->jerr1=(C)e; if(jt->etxn<0)R;  // remember error for testing, but if the error line is frozen, don't touch it
 moveparseinfotosi(jt); jt->etxn=0;  // before we display, move error info from parse variables to si
 dhead(0,0L);
 if((jt->uflags.trace&TRACEDB)&&!spc()){eputs("ws full (can not suspend)"); eputc(CLF); jt->uflags.trace&=~TRACEDB;}
 eputl(AAV(JT(jt,evm))[jt->jerr]);
 if(!jt->glock){
  if(e==EVCTRL){dhead(3,0L); efmt("["FMTI"]",j); eputl(w);}
  else{
   dhead(3,0L); eputl(w);
   dhead(3,0L); DQ(j, eputc(' ');); eputc('^'); eputc(CLF);
  }
  debsi1(jt->sitop);
 }
 jt->etxn1=jt->etxn;
#else
 R jtjsignale(jt,e|EMSGLINEISA+EMSGCXINFO,w,j);
#endif
}    /* signal error e on line w with caret at j */

A jtjsigd(J jt,C*s){
#if 0 // obsolete
C buf[100],*d="domain error: ";I m,n,p;
 m=strlen(d); MC(buf,d,m);
 n=strlen(s); p=MIN(n,100-m); MC(buf+m,s,p);
 jsigstr(EVDOMAIN,m+p,buf);
#else
R jtjsignale(jt,EVDOMAIN|EMSGSPACEAFTEREVM,(A)s,strlen(s));
#endif
}

// display for 13!:8
static F2(jtdbsig){I e;
 RE(0);
 if(!AN(w))R mtm;
 RZ(w=vi(w)); e=AV(w)[0]; 
 ASSERT(1<=e,EVDOMAIN);
 ASSERT(e<=255,EVLIMIT);
 if(a||e>NEVM){if(!a)a=mtv; RZ(a=vs(a)); jtjsignale(jt,e|EMSGLINEISA+EMSGLINEISTERSE+EMSGNOEFORMAT,a,0);} else jsignal(e|EMSGNOEFORMAT);  // must not run eformat, since self does not apply
 R 0;
}    

F1(jtdbsig1){R dbsig(0L,w);}   /* 13!:8  signal error */
F2(jtdbsig2){R dbsig(a, w);}

// 9!:59 set emsgslvl, return previous
DF1(jtemsglevel){
 if(!AN(w))R mtm;
 RZ(w=vi(w)); I e=AV(w)[0]; 
 ASSERT(e<=7,EVDOMAIN);  // must be integer in range 0-7
 I ostate=jt->emsgstate; jt->emsgstate=(jt->emsgstate&~7)|e;
 R sc(ostate);
}


F1(jtdberr){ASSERTMTV(w); R sc(jt->jerr1);}           /* 13!:11 y  last error number   */
// 13!:12 last error text.  If there is no error, show no text.  If there is an error with no text, we must have suppressed
// loading the terse message; return it now
F1(jtdbetx){
 ASSERTMTV(w);
 if(jt->jerr1==0)R mtv;  // if no error, no text either
 if(jt->etxn1==0 && BETWEENC(jt->jerr1,1,NEVM))R AAV(JT(jt,evm))[jt->jerr1];  // no text, supply it now
 R str(jt->etxn1,jt->etx);  // leave the text that's there
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
}}   /* see <errno.h> / <winerror.h> */
