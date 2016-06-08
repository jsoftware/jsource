/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Explicit Definition : and Associates                      */

/* Usage of the f,g,h fields of : defined verbs:                           */
/*  f  character matrix of  left argument to :                             */
/*  g  character matrix of right argument to :                             */
/*  h  4-element vector of boxes                                           */
/*       0  vector of boxed tokens for f                                   */
/*       1  vector of triples of control information                       */
/*       2  vector of boxed tokens for g                                   */
/*       3  vector of triples of control information                       */

#include "j.h"
#include "d.h"
#include "p.h"
#include "w.h"

#define BASSERT(b,e)   {if(!(b)){jsignal(e); i=-1; z=0; continue;}}
#define BGA(v,t,n,r,s) BZ(v=ga(t,(I)(n),(I)(r),(I*)(s)))
#define BZ(e)          if(!(e)){i=-1; z=0; continue;}

#define LINE(sv)       {A x; \
                        h=sv->h; hv=AAV(sv->h); hi=a&&w?HN:0; \
                        line=AAV(hv[hi]); x=hv[1+hi]; n=AN(x); cw=(CW*)AV(x);}

typedef struct{A t,x,line;C*iv,*xv;I j,k,n,w;} CDATA;
/* for_xyz. t do. control data   */
/* line  'for_xyz.'              */
/* t     iteration array         */
/* n     #t                      */
/* k     length of name xyz      */
/* x     text xyz_index          */
/* xv    ptr to text xyz_index   */
/* iv    ptr to text xyz         */
/* j     iteration index         */
/* w     cw code                 */

#define WCD            (sizeof(CDATA)/sizeof(I))

typedef struct{I d,t,e;} TD;
#define WTD            (sizeof(TD)/sizeof(I))
#define NTD            17     /* maximum nesting for try/catch */


static B jtforinit(J jt,CDATA*cv,A t){A x;C*s,*v;I k;
 ASSERT(t,EVCTRL);
 cv->t=ra(t);                            /* iteration array     */
 cv->n=IC(t);                            /* # of items in t     */
 cv->j=-1;                               /* iteration index     */
 cv->x=0;
 cv->k=k=AN(cv->line)-5;                 /* length of item name */
 if(0<k&&cv->n){                         /* for_xyz.            */
  s=4+CAV(cv->line); RZ(cv->x=x=ra(str(6+k,s))); 
  cv->xv=v=CAV(x); MC(k+v,"_index",6L);  /* index name          */
  cv->iv=s;                              /* item name           */
 }
 R 1;
}    /* for. do. end. initializations */

static B jtunstackcv(J jt,CDATA*cv){
 if(cv->x){ex(link(cv->x,str(cv->k,cv->iv))); fa(cv->x);}
 fa(cv->t); 
 memset(cv,C0,WCD*SZI); 
 R 1;
}

static void jttryinit(J jt,TD*v,I i,CW*cw){I j=i,t=0;
 v->d=v->t=0;
 while(t!=CEND){
  j=(j+cw)->go;
  switch(t=(j+cw)->type){
   case CCATCHD: v->d=j; break;
   case CCATCHT: v->t=j; break;
   case CEND:    v->e=j; break;
}}}  /* processing on hitting try. */


#define CHECKNOUN if (!(NOUN&AT(t))){   /* error, T block not creating noun */ \
    /* Recreate the execution of the failing sentence, and show an error for it */ \
    i = ti; parsex(vec(BOX, cw[ti].n, line + cw[ti].i), -1, &cw[ti], d); \
    /* go to error loc; if we are in a try., send this error to the catch. */ \
    i = cw[ti].go; if (i<SMAX){ RESETERR; if (tdi){ --tdi; jt->db = od; } }  \
    break; }

// Processing of explicit definitions, line by line
static DF2(jtxdefn){F2PREF;PROLOG;A cd,cl,cn,h,*hv,*line,loc=jt->local,t,td,u,v,z;B b,fin,lk,named,ox=jt->xdefn;CDATA*cv;
  CW *ci,*cw;DC d=0;I bi,hi,i=0,j,m,n,od=jt->db,old,r=0,st,tdi=0,ti;TD*tdv;V*sv;X y;
 RE(0);
 // z is the final result (initialized here in case there are no lines)
 // t is the result of the current t block, or 0 if not in t block
 // u,v are the operand(s), if this is an explicit modifier
 // sv->text of this explicit entity, st is its type
 // r is the count of empty stack frames that have been allocated
 // bi is the control-word number for the last b-block sentence executed
 // *tci is the CW info for the last t-block sentence executed
 z=mtm; cd=t=u=v=0; sv=VAV(self); st=AT(self);
 // lk=this definition is locked; named=this definition is named; cl=current name, cl=current locale
 lk=jt->glock||VLOCK&sv->flag; named=VNAMED&sv->flag?1:0; cn=jt->curname; cl=jt->curlocn;
 d=named&&jt->db&&DCCALL==jt->sitop->dctype?jt->sitop:0; /* stack entry for dbunquote for this fn */
 // If this is a verb referring to x or y, set u, v to the operands, and sv to the saved text
 if(VXOP&sv->flag){u=sv->f; v=sv->h; sv=VAV(sv->g);}
 // If this is adv/conj, it must be 1/2 : executed with no x or y
 if(st&ADV+CONJ){u=a; v=w;}
 // Read the info for the parsed definition, including control table and number of lines
 LINE(sv); ASSERT(n,EVDOMAIN);
 // Create symbol table for this execution
 RZ(jt->local=stcreate(2,1L,0L,0L));
 // If the verb contains try., allocate a try-stack area for it
 if(sv->flag&VTRY1+VTRY2){GA(td,INT,NTD*WTD,2,0); *AS(td)=NTD; *(1+AS(td))=WTD; tdv=(TD*)AV(td);}
 FDEPINC(1);   // do not use error exit after this point; use BASSERT, BGA, BZ
 jt->xdefn=1;   // Indicate explicit definition running
 // Assign the special names x y m n u v
 IS(xnam,a); if(u){IS(unam,u); if(NOUN&AT(u))IS(mnam,u);}
 IS(ynam,w); if(v){IS(vnam,v); if(NOUN&AT(v))IS(nnam,v);}
 if(jt->dotnames){
  IS(xdot,a); if(u){IS(udot,u); if(NOUN&AT(u))IS(mdot,u);}
  IS(ydot,w); if(v){IS(vdot,v); if(NOUN&AT(v))IS(ndot,v);}
 }
 // If we are in debug mode, and the current stack frame has the DCCALL type, set up
 // so that the debugger can look inside this execution: point to the local symbols,
 // to the control-word table, and to where we store the currently-executing line number
 if(jt->db&&jt->sitop&&DCCALL==jt->sitop->dctype&&self==jt->sitop->dcf){
  jt->sitop->dcloc=jt->local; jt->sitop->dcc=hv[1+hi]; jt->sitop->dci=(I)&i;
 }
 // remember tbase.  We will tpop after every sentence to free blocks
 old=jt->tbase+jt->ttop; 
 // loop over each sentence
 while(0<=i&&i<n){
  // if performance monitor is on, collect data for it
  if(0<jt->pmctr&&C1==jt->pmrec&&named)pmrecord(cn,cl,i,a?VAL2:VAL1);
  // ?? debugging, something to do with redefinition of executing verb?
  if(jt->redefined&&jt->sitop&&jt->redefined==jt->sitop->dcn&&DCCALL==jt->sitop->dctype&&self!=jt->sitop->dcf){
   self=jt->sitop->dcf; sv=VAV(self); LINE(sv); jt->sitop->dcc=hv[1+hi];
   jt->redefined=0;
   if(i>=n)break;
  }
  // i holds the control-word number of the current control word
  ci=i+cw;   // ci->control-word info
  // process the control word according to its type
  switch(ci->type){
   case CTRY:
    // try.  create a try-stack entry, step to next line
    BASSERT(tdi<NTD,EVLIMIT);
    tryinit(tdv+tdi,i,cw);
    if(jt->db)jt->db=(tdv+tdi)->d?jt->dbuser:DBTRY;
    ++tdi; ++i; 
    break;
   case CCATCH: case CCATCHD: case CCATCHT:
    // catch.  pop the try-stack, go to end., reset debug state.  here should always be a try. stack here
    if(tdi){--tdi; i=1+(tdv+tdi)->e; jt->db=od;}else i=ci->go; break;
   case CTHROW:
    // throw.  Create a faux error
    BASSERT(0,EWTHROW);
   case CBBLOCK:
    // B-block (present on every sentence in the B-block)
    // run the sentence
    tpop(old); z=parsex(vec(BOX,ci->n,line+ci->i),lk,ci,d);
    // if there is no error, or ?? debug mode, step to next line
    if(z||DB1==jt->db||DBERRCAP==jt->db||!jt->jerr)bi=i,++i;
    // if the error is THROW, and there is a catcht. block, go there, otherwise error
    else if(EWTHROW==jt->jerr){if(tdi&&(j=(tdv+tdi-1)->t)){i=1+j; RESETERR;}else BASSERT(0,EWTHROW);}
    // for other error, go to the error location; if that's out of range, keep the error; if not,
    // it must be a try. block, so clear the error.  Pop the stack, in case we're continuing
    // NOTE ERROR: if we are in a for. or select., going to the catch. will leave the stack corrupted,
    // with the for./select. structures hanging on.  Solution would be to save the for/select stackpointer in the
    // try. stack, so that when we go to the catch. we can cut the for/select stack back to where it
    // was when the try. was encountered
    else{i=ci->go; if(i<SMAX){RESETERR; if(tdi){--tdi; jt->db=od;}}}
    break;
   case CASSERT:
    // assert.  If assertions disabled, skip the line and continue
    if(!jt->assert){++i; break;}
    // otherwise fall through to process the line.
   case CTBLOCK:
    // execute and parse line as if for B block, except save the result in t
    gc(z,old); t=parsex(vec(BOX,ci->n,line+ci->i),lk,ci,d);
    if(t||DB1==jt->db||DBERRCAP==jt->db||!jt->jerr)ti=i,++i;
    else if(EWTHROW==jt->jerr){if(tdi&&(j=(tdv+tdi-1)->t)){i=1+j; RESETERR;}else BASSERT(0,EWTHROW);}
    else{i=ci->go; if(i<SMAX){RESETERR; if(tdi){--tdi; jt->db=od;}}}
    break;
   case CFOR:
   case CSELECT: case CSELECTN:
    // for./select. push the stack.  If the stack has not been allocated, start with 9 entries.  After that,
    // if it fills up, extend it by 1 as required
    if(!r)
     if(cd){m=AN(cd)/WCD; BZ(cd=ext(1,cd)); cv=(CDATA*)AV(cd)+m-1; r=AN(cd)/WCD-m;}
     else  {r=9; BGA(cd,INT,r*WCD,1,0); cv=(CDATA*)AV(cd)-1; ra(cd);}
    ++cv; --r; 
    // indicate no t result (test value for select., iteration array for for.) and clear iteration index
    // remember the line number of the for./select.
    cv->t=cv->x=0; cv->line=line[ci->i]; cv->w=ci->type; ++i;
    break;
   case CDOF:   // do. after for.
    // do. after for. .  If this is first time, initialize the iterator
    if(!cv->t){
     BASSERT(t,EVCTRL);   // Error if no sentences in T-block
     CHECKNOUN    // if t is not a noun, signal error on the last line executed in the T block
     BZ(forinit(cv,t)); t=0;
    }
    ++cv->j;  // step to first (or next) iteration
    if(cv->j<cv->n){  // if there are more iterations to do...
     if(cv->x){A x;  // assign xyz and xyz_index for for_xyz.
      symbis(nfs(6+cv->k,cv->xv),x=sc(cv->j),  jt->local);
      symbis(nfs(  cv->k,cv->iv),from(x,cv->t),jt->local);
     }
     ++i; continue;   // advance to next line and process it
    }
    // if there are no more iterations, fall through... (this deallocates the loop variables)
   case CENDSEL:
    // end. for select., and do. for for. after the last iteration, must pop the stack - just once
    rat(z); unstackcv(cv); --cv; ++r; 
    i=ci->go;    // continue at new location
    break;
   case CBREAK:
   case CCONT:
    // break.-in-while./continue must pop the stack if they are in a select. structure.  Then, they must pop
    // until they have popped the SELECT type.  It would be possible for the initial analysis to
    // decide whether any popping is required, but we didn't do that.
    if(cd&&(cv->w==CSELECT||cv->w==CSELECTN)){
     rat(z);   // protect possible result from pop
     do{fin=cv->w==CSELECT; unstackcv(cv); --cv; ++r;}while(!fin);
    }
    i=ci->go;   // After popping any select. off the stack, continue at new address
    break;
   case CBREAKF:
    // break. in a for. must first pop any active select., and then pop the for.
    // We just pop till we have popped a non-select.
    rat(z);   // protect possible result from pop
    do{fin=cv->w!=CSELECT&&cv->w!=CSELECTN; unstackcv(cv); --cv; ++r;}while(!fin);
    i=ci->go;     // continue at new location
    break;
   case CRETURN:
    // return.  Increment the use-count of the result, pop the stack back to empty, set i (which will exit)
    if(cd){rat(z); DO(AN(cd)/WCD-r, unstackcv(cv); --cv; ++r;);}
    i=ci->go;
    break;
   case CCASE:
   case CFCASE:
    // case. and fcase. are used to start a selection.  t has the result of the T block; we check to
    // make sure this is a noun, and save it on the stack in cv->t.  Then clear t
    if(!cv->t){
     BASSERT(t,EVCTRL);
     CHECKNOUN    // if t is not a noun, signal error on the last line executed in the T block
     BZ(cv->t=ra(boxopen(t))); t=0;
    }
    i=ci->go;  // Go to next sentence, which might be in the default case (if T block is empty)
    break;
   case CDOSEL:   // do. after case. or fcase.
    // do. for case./fcase. evaluates the condition.  t is the result (a T block); if it is nonexistent
    // or not all 0, we advance to the next sentence (in the case); otherwise skip to next test/end

    if(t){CHECKNOUN}    // if t is not a noun, signal error on the last line executed in the T block

    i=t&&all0(eps(cv->t,boxopen(t)))?ci->go:1+i; // cv +./@:e. boxopen t; go to miscompare point if no match
    // Clear t to ensure that the next case./fcase. does not think it's the first one
    t=0; 
    break;
   case CDO:
    // do. here is one following if., elseif., or while. .  It always follows a T block, and skips the
    // following B block if the condition is false.  Set b to 1 iff the condition is true
    //  Start by assuming condition is true; set to move to the next line then
    ++i; b=1;
    // If there is no t, that's true
    if(t){
     if(SPARSE&AT(t))BZ(t=denseit(t));   // convert sparse to dense
     
     CHECKNOUN    // if t is not a noun, signal error on the last line executed in the T block

     switch(AN(t)?AT(t):0){   // empty t is also true
      // Check for nonzero.  Nonnumeric types always test true.  Comparisons against 0 are exact.
      case RAT:
      case XNUM: y=*XAV(t); b=*AV(y)||1<AN(y); break;  // rat/xnum true if first word non0, or multiple words
      case CMPX: b=0!=*DAV(t)||0!=*(1+DAV(t)); break;  // complex if either part nonzero
      case FL:   b=0!=*DAV(t);                 break;
      case INT:  b=0!=*AV(t);                  break;
      case B01:  b=*BAV(t);
    }}
    t=0;  // Indicate no T block, now that we have processed it
    if(b)break;  // if true, step to next sentence.  Otherwise
    // fall through to...
   default:   // including BREAK and END
    JBREAK0;   // Check for interrupts
    i=ci->go;  // Go to the next sentence, whatever it is
 }}
 // If we are executing a verb (whether or not it started with 3 : or [12] :), make sure the result is a noun.
 // If it isn't, abortively reexecute the sentence that created the noun-noun result, and flag it as error
 // The -1 means 'flag as non-noun, don't actually execute'
 if(z&&!(st&ADV+CONJ)&&!(AT(z)&NOUN))i=bi, parsex(vec(BOX, cw[bi].n, line + cw[bi].i), -1, &cw[bi], d);
 FDEPDEC(1);  // OK to ASSERT now
 z=jt->jerr?0:z?ra(z):mtm;  // If no error, increment use count in result to protect it from tpop
 fa(cd);   // deallocate the explicit-entity stack
 // Deallocate the local symbol table; pop the locale stack and xdefn; set no assignment (to call for result display)
 symfreeh(jt->local,0L); jt->local=loc; jt->asgn=0; jt->xdefn=ox;
 tpop(_ttop);   // finish freeing memory
 // Give this result a short lease on life
 tpush(z);
 R z;
}


static DF1(xv1){F1PREF;R df1(  w,VAV(self)->f);}
static DF2(xv2){F2PREF;R df2(a,w,VAV(self)->g);}

static DF1(xn1 ){F1PREF;R xdefn(0L,w, self);}
static DF1(xadv){F1PREF;R xdefn(w, 0L,self);}


static F1(jtxopcall){F1PREF;R jt->db&&DCCALL==jt->sitop->dctype?jt->sitop->dca:mark;}

static DF1(xop1){F1PREF;A ff,x;
 RZ(ff=fdef(CCOLON,VERB, xn1,jtxdefn, w,self,0L, VXOP|VAV(self)->flag, RMAX,RMAX,RMAX));
 RZ(x=xopcall(one));
 R x==mark?ff:namerefop(x,ff);
}

static DF2(xop2){F2PREF;A ff,x;
 RZ(ff=fdef(CCOLON,VERB, xn1,jtxdefn, a,self,w,  VXOP|VAV(self)->flag, RMAX,RMAX,RMAX));
 RZ(x=xopcall(one));
 R x==mark?ff:namerefop(x,ff);
}


// h is the compiled form of an explicit function: an array of 2*HN boxes.
// Boxes 0&3 contain the enqueued words  for the sentences, jammed together
// We return 1 if this function refers to its x/y arguments (which also requires
// a reference to mnuv operands)
static B jtxop(J jt,A w){B mnuv,xy;I i,k;
 // init flags to 'not found'
 mnuv=xy=0;
 // Loop through monad and dyad
 A *wv=AAV(w); I wd=(I)w*ARELATIVE(w);
 for(k=0;k<6;k+=3){    // for monad and dyad cases...
  A w=WVR(k);  // w is now the box containing the words of the expdef
  {A *wv=AAV(w);
   I wd=(I)w*ARELATIVE(w);
   I in=AN(w);
   // Loop over each word, starting at beginning where the references are more likely
   for(i=0;i<in;++i) {
    A w=WVR(i);  // w is box containing a queue value.  If it's a name, we inspect it
    if(AT(w)==NAME){
     // Get length/string pointer
     I n=AN(w); C *s=NAV(w)->s;
     // if dotnames allowed, and last character is '.', back up 1
     if(n){
      if(s[n-1]=='.'&&jt->dotnames)--n;
      // Set flags if this is a special name, or an indirect locative referring to a special name in the last position
      if(n==1||(n>=3&&s[n-3]=='_'&&s[n-2]=='_')){
       if(s[n-1]=='m'||s[n-1]=='n'||s[n-1]=='u'||s[n-1]=='v')mnuv=1;
       else if(s[n-1]=='x'||s[n-1]=='y')xy=1;
        // exit if we have seen enough
       if(mnuv&&xy)R 1;
      }   // 'one-character name'
     }  // 'name is not empty'
    } // 'is name'
   }  // loop for each word
  }  // namescope of new w
 }  // loop for each valence
 // If we didn't see xy and mnuv, it's not a derived function
 R 0;
}

static F1(jtcolon0){F1PREF;A l,z;C*p,*q,*s;I m,n;
 n=0; RZ(z=exta(LIT,1L,1L,300L)); s=CAV(z);
 while(1){
  RE(l=jgets("\001"));
  if(!l)break;
  m=AN(l); p=q=CAV(l); 
  if(m){while(' '==*p)++p; if(')'==*p){while(' '==*++p); if(p>=m+q)break;}}
  while(AN(z)<=n+m){RZ(z=ext(0,z)); s=CAV(z);}
  MC(s+n,q,m); n+=m; *(s+n)=CLF; ++n;
 }
 R str(n,s);
}    /* enter nl terminated lines; ) on a line by itself to exit */

static F1(jtlineit){F1PREF;
 R 1<AR(w)?ravel(stitch(w,scc(CLF))):AN(w)&&CLF==cl(w)?w:over(w,scc(CLF));
}

static B jtsent12c(J jt,A w,A*m,A*d){C*p,*q,*r,*s,*x;
 ASSERT(!AN(w)||LIT&AT(w),EVDOMAIN);
 ASSERT(2>=AR(w),EVRANK);
 RZ(w=lineit(w));
 x=p=r=CAV(w);  /* p: monad start; r: dyad start */
 q=s=p+AN(w);   /* q: monad end;   s: dyad end   */
 while(x<s){
  q=x;
  while(' '==*x)++x; if(':'==*x){while(' '==*++x); if(CLF==*x){r=++x; break;}}
  while(CLF!=*x++);
 }
 if(x==s)q=r=s;
 *m=df1(str(q-p,p),cut(ds(CBOX),num[-2]));
 *d=df1(str(s-r,r),cut(ds(CBOX),num[-2]));
 R 1;
}    /* literal fret-terminated or matrix sentences into monad/dyad */

static B jtsent12b(J jt,A w,A*m,A*d){A t,*wv,y,*yv;I j,*v,wd;
 ASSERT(1>=AR(w),EVRANK);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 GA(y,BOX,AN(w),AR(w),AS(w)); yv=AAV(y);
 DO(AN(w), RZ(yv[i]=vs(WVR(i))););
 RZ(t=indexof(y,link(chr[':'],str(1L,":")))); v=AV(t); j=MIN(*v,*(1+v));
 *m=take(sc(j  ),y); 
 *d=drop(sc(j+1),y);
 R 1;
}    /* boxed sentences into monad/dyad */

F2(jtcolon){F2PREF;A d,h,*hv,m;B b;C*s;I flag=0,n,p;
 RZ(a&&w);
 if(VERB&AT(a)&&VERB&AT(w)){V*v;
  v=VAV(a); if(CCOLON==v->id&&VERB&AT(v->f)&&VERB&AT(v->g))a=v->f;
  v=VAV(w); if(CCOLON==v->id&&VERB&AT(v->f)&&VERB&AT(v->g))w=v->g;
  R fdef(CCOLON,VERB,xv1,xv2,a,w,0L,0L,mr(a),lr(w),rr(w));
 }
 RE(n=i0(a));
 if(equ(w,zero)){RZ(w=colon0(mark)); if(!n)R w;}
 if(C2T&AT(w))RZ(w=cvt(LIT,w));
 if(10<n){s=CAV(w); p=AN(w); if(p&&CLF==s[p-1])RZ(w=str(p-1,s));}
 else{
  RZ(BOX&AT(w)?sent12b(w,&m,&d):sent12c(w,&m,&d));
  if(4==n){if(AN(m)&&!AN(d))d=m; m=mtv;}
  GA(h,BOX,2*HN,1,0); hv=AAV(h);
  RE(b=preparse(m,hv,   hv+1   )); if(b)flag|=VTRY1; hv[2   ]=jt->retcomm?m:mtv;
  RE(b=preparse(d,hv+HN,hv+HN+1)); if(b)flag|=VTRY2; hv[2+HN]=jt->retcomm?d:mtv;
 }
 if(!n)R ca(w);
 if(2>=n){
  RE(b=xop(h)); 
  if(b)flag|=VXOPR; 
  else if(2==n&&AN(m)&&!AN(d)){A*u=hv,*v=hv+HN,x; DO(HN, x=*u; *u++=*v; *v++=x;);}
 }
 flag|=VFIX;  // ensures that f. will not look inside n : n
 switch(n){
  case 1:  R fdef(CCOLON, ADV,  b?xop1:xadv,0L,    num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 2:  R fdef(CCOLON, CONJ, 0L,b?xop2:jtxdefn, num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 3:  R fdef(CCOLON, VERB, xn1,jtxdefn,       num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 4:  R fdef(CCOLON, VERB, xn1,jtxdefn,       num[n],0L,h, flag, RMAX,RMAX,RMAX);
  case 13: R vtrans(w);
  default: ASSERT(0,EVDOMAIN);
}}
