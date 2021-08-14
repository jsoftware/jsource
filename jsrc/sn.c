/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Names                                                     */

#include "j.h"

// validate fullname (possibly locative).  s->name, n=length.  Returns 1 if name valid, 0 if not
B jtvnm(J jt,I n,C *s){C c,t;I j;
 s=n==0?(C*)ctype+1:s;  // if empty string, point to erroneous string (we will fetch s[0] and s[-1]) 
 c=s[0];    // c = first char of name
 t=ctype[(UC)c]; t|=t>>3;   // decode char type; move 'numeric' to 'error'.  We accumulate error in LSB of t
 // scan the string: verify all remaining characters alphameric (incl _); set j=index of first indirect locative (pointing to the __), or 0 if no ind loc
  // (the string can't start with _)
 C cn=s[n-1];  // last character
 t|=ctype[(UC)cn];   // include flags for last char
 if(n<=2)R 1^((t&1)|(cn=='_'));  // 1- or 2-char name, OK if char(s) OK & doesn't end with _
 {C prevcu0; C cu0=cn^'_';  // cu0 = 0 iff cn=='_'
  j=-1;  // Init no indirect locative found
  DQU(n-2, prevcu0=cu0; t|=ctype[(UC)(c=s[i+1])]; cu0=c^'_'; j=(cu0|prevcu0)?j:i;)
 }
 // Now t is the mask of invalidity, and j is the index one before the first __ (-1 if no __)
 if((t&1)+((cn!='_')&SGNTO0(j)))R 1^(t&1);   // Return if accumulated error, or if not trailing '_' and no __ (normal return)
 // If the last char is _, any ind loc is invalid (but not trailing __); scan to find previous _ (call its index j, error if 0); audit locale name, or OK if empty (base locale)
 if(cn=='_'){if(j>=0)R j==n-3; j=n-3; NOUNROLL do{if(s[j]=='_')R((ctype[(UC)s[j+1]]&CA)||vlocnm(n-j-2,s+j+1));}while(--j>0); R 0;}  // return if any __, including at end; find last '_', which cannot be in the last 2 chars; see if valid locale name; if no '_', error
 // Here last char was not _, and j is still pointed after __ if any
 // There is an indirect locative.  Scan all of them, verifying first char of each name is alphabetic (all chars were verified alphameric above)
 // Also verify that any _ is preceded or followed by _
 // We do this with a state machine that scans 3 characters at a time, creating 3 bits: [0]='_' [1]='_' [2]=digit (including _).  We always start pointing to the first '_'.  State result tells
 // how many characters to advance, 0 meaning error.  We stop when there are <2 characters left.  The word cannot end with '_'.  If it ends xx9_9 we will go to 9_9 and then _9?, i. e. overfetch the buffer by 1.  But that's OK on literal data.
 // advance counts are: xxa=3, xx9=2, x_a=0, x_9=1, _xa=0, _x9=0, __a=3, __9=0 
 ++j; do{I state=4*(s[j]=='_')+2*(s[j+1]=='_')+(ctype[(UC)s[j+2]]>>3); state=(0x03001023L>>(state<<2))&3; if(state==0)R 0; j+=state;}while(j<n-1);
 R 1;
}    /* validate name s, return 1 if name well-formed or 0 if error */

B vlocnm(I n,C*s){
 I accummask=0; DO(n, UC c=s[i]; C t=ctype[c]; t=c=='_'?CX:t; accummask|=t;)  // create mask of types encountered.  Treat  '_' as nonalphameric
 if(unlikely(accummask&~(CA|C9)))R 0;  // error if any non-alphameric encountered
 if(unlikely(n<2))R (B)n;  // error if n=0; OK if n=1 (any alphameric is OK then)
 if(s[0]>'9')R 1;  // if nonnumeric locale, alphameric name must be OK
 if(unlikely(s[0]=='0'))R 0;
 if(unlikely(n>(SZI==8?18:9)))R 0;  // numeric locale: if (multi-digit) locale starts with '0', or number is too long for an I (conservatively), error
 R accummask==C9;   // if there are any alphabetics, give error
}    /* validate locale name: 1 if locale-name OK, 0 if error */

static const C argnames[7]={'m','n','u','v','x','y',0};
// s-> a string of length n.  Make a NAME block for the name.  It might not be valid; use our best efforts then.  We ALWAYS look at the first and last character, even if length is 0
// Possible errors: EVILNAME, EVLIMIT (if name too long), or memory error
A jtnfs(J jt,I n,C*s){A z;C f,*t;I m,p;NM*zv;
 // Discard leading and trailing blanks.  Leave t pointing to the last character
 DQ(n, if(' '!=(f=*s))break; ++s; --n;); 
 t=s+n-1;
 DQ(n, if(' '!=*t)break; --t; --n;);
 ASSERT(n!=0,EVILNAME);   // error if name is empty  (? not required since name always valid?
 // If the name is the special x y.. or x. y. ..., return a copy of the preallocated block for that name (we may have to add flags to it)
 if(SGNTO0(n-2)&BETWEENC(f,'m','y')&(p=(0x1b03>>(f-'m')))){  // M N o p q r s t U V w X Y 1101100000011
  R ca(mnuvxynam[5-((p&0x800)>>(11-2))-((p&0x8)>>(3-1))-((p&0x2)>>(1-0))]);  // return a clone of the argument block (because flags may be added)
 }
 // The name may not be valid, but we will allocate a NAME block for it anyway
 GATV0(z,NAME,n,1); zv=NAV(z);   // the block is cleared to 0
 MC(zv->s,s,n); zv->s[n]=0;  // should copy locally, with special dispensation for <4 chars
// no because sources may be short  MCISH(zv->s,s,(n+SZI-1)>>LGSZI); *(n+zv->s)=0;  // copy in the name in fullwords (OK because NAMEs are passed, null-terminate it
 f=0; m=n; p=0;
 // Split name into simplename and locale, verify length of each; set flag and hash for locative/indirect locative
 if('_'==*t){
   // name ends with _: direct locative
   --t; NOUNROLL while(s<t&&'_'!=*t)--t; f=NMLOC; p=n-2-(t-s); m=n-(2+p);  // t->_ before localename, p=#locale name, m=#simplename
   // install hash/number for the direct locale
   zv->bucketx=BUCKETXLOC(p,t+1);  // number if numeric, hash otherwise
 }else{
   // otherwise either simple name or indirect locative.  Look for the __; if present, find & hash the last indirect name
   DO(n, if('_'==s[i]&&'_'==s[1+i]){ f=NMILOC; p=n-2-i; for(m=n; s[m-1]!='_'||s[m-2]!='_';--m); zv->bucketx=(I)nmhash(n-m,s+m); m=n-(2+p); break;});  // p=#locales, m=#simplename, hash last indirect if there is one
 }
 ASSERT((m|p)<=255,EVLIMIT);  // error if name too long.  Requires limit be power of 2
 zv->flag=f;  // Install locative flag
 zv->m=(UC)m; zv->hash=(UI4)nmhash(m,s); // Install length, and calculate hash of simple name
 RETF(z);
}    /* name from string */

// string from name: returns string for the name
// if b&SFNSIMPLEONLY, return only the simple name
A jtsfn(J jt,B b,A w){NM*v; ARGCHK1(w); v=NAV(w); R str(b&SFNSIMPLEONLY?v->m:AN(w),v->s);}

// string from name evocation: returns string for name UNLESS the name was an NMDOT type; in that case it returns w f. which will be a verb
A jtsfne(J jt,A w){ARGCHK1(w); A wn=FAV(w)->fgh[0]; if(AT(wn)&NAMEBYVALUE)R fix(w,zeroionei(0)); R sfn(0,wn);}


F1(jtnfb){A y;C*s;I n;
 ARGCHK1(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(!AR(w),EVRANK);
 RZ(y=vs(ope(w)));
 n=AN(y); s=CAV(y);
 ASSERTN(vnm(n,s),EVILNAME,nfs(n,s));
 R nfs(n,s);
}    /* name from scalar boxed string */

// w is an A for a name string; return NAME block or 0 if error
static F1(jtstdnm){C*s;I j,n,p,q;
 if(!(w=vs(w)))R 0;  // convert to ASCII
 n=AN(w); s=CAV(w);  // n = #characters, s->string
 if(!(n))R 0;
 j=0;   DQ(n, if(' '!=s[j++])break;); p=j-1;
 j=n-1; DQ(n, if(' '!=s[j--])break;); q=(n-2)-j;
 if(!(vnm(n-(p+q),p+s)))R 0;   // Validate name
 R nfs(n-(p+q),p+s);   // Create NAME block for name
}    /* 0 result means error or invalid name */

// x is a (possibly) boxed string; result is NAME block for name x, error if invalid name
F1(jtonm){A x,y; RZ(x=ope(w)); y=stdnm(x); ASSERTN(y,EVILNAME,nfs(AN(x),CAV(x))); R y;}

// w is array of boxed strings; result is name class for each
F1(jtnc){A*wv,x,y,z;I i,n,t,*zv;L*v; 
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);   // n=#names  wv->first box
 ASSERT(!n||BOX&AT(w),EVDOMAIN);   // verify boxed input (unless empty)
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);   // Allocate z=result, same shape as input; zv->first result
 for(i=0;i<n;++i){   // for each name...
  RE(y=stdnm(wv[i]));  // point to name, audit for validity
  if(y){if(v=syrd(y,jt->locsyms)){x=v->val; t=AT(x);}else{x=0; if(jt->jerr){y=0; RESETERR;}}}  // If valid, see if the name is defined
  // syrd can fail if a numbered locative is retrograde.  Call that an invalid name, rather than an error, here; thus the RESETERR
  // kludge: if the locale is not defined, syrd will create it.  Better to use a version/parameter to syrd to control that?
  //   If that were done, we could dispense with the error check here (but invalid locale would be treated as undefined rather than invalid).
  // Would have to mod locindirect too
  I zc; zc=(((1LL<<(ADVX-ADVX))|(2LL<<(CONJX-ADVX))|(3LL<<(VERBX-ADVX)))>>(CTTZ(((t&CONJ+ADV+VERB)|(1LL<<31))>>ADVX)))&3;   // ADVX, CONJx, VERBX, and the implied NOUNX=31 must all be >+ 2 bits apart
  zc=x?zc:-1; zc=y?zc:-2;
  zv[i]=zc;  // calculate the type, store in result array
 }
 RETF(z);
}    /* 4!:0  name class */

// these functions are called with an a arg that is a 256-char rank-1 boolean map giving the initial characters wanted, and AS(a)[0] is a mask of allowed types
static SYMWALK(jtnlxxx, A,BOX,20,1, CAV1(a)[((UC*)NAV(d->name)->s)[0]]&&AS(a)[0]&AT(d->val), 
    RZ(*zv++=incorp(sfn(SFNSIMPLEONLY,d->name))) )

SYMWALK(jtnlsym, A,BOX,20,1, LOCPATH(d->val)&&CAV1(a)[((UC*)NAV(d->name)->s)[0]],
    RZ(*zv++=incorp(sfn(SFNSIMPLEONLY,d->name))) )

static const I nlmask[] = {NOUN,ADV,CONJ,VERB, MARK,MARK,SYMB,MARK};

// a is the rank-1 256-byte initial-letter mask
static F2(jtnlx){A z=mtv;B b;I m=0,*v,x;
 RZ(w=vi(w)); v=AV(w); 
 DQ(AN(w), x=*v++; m|=nlmask[BETWEENC(x,0,6)?x:7];); 
 AS(a)[0]=m&RHS; b=1&&AS(a)[0]&RHS;  // AS(a)[0] is used for the type mask
 ASSERT(!(m&MARK),EVDOMAIN);
 if(b           )RZ(z=nlxxx(a,jt->global));
 if(b&&(AN(jt->locsyms)>1))RZ(z=over(nlxxx(a,jt->locsyms),z));
 if(m==SYMB     )RZ(z=over(nlsym(a,JT(jt,stloc)),z));
 R nub(grade2(z,ope(z)));
}

F1(jtnl1){A a; GAT0(a,B01,256,1) mvc(256L,CAV1(a),1,MEMSET01); R nlx(a,w);}
     /* 4!:1  name list */

F2(jtnl2){UC*u;
 ARGCHK2(a,w);
 ASSERT(LIT&AT(a),EVDOMAIN);
 A tmp; GAT0(tmp,B01,256,1) mvc(256L,CAV1(tmp),1,MEMSET00);
 u=UAV(a); DQ(AN(a),CAV1(a=tmp)[*u++]=1;);
 R nlx(tmp,w);
}    /* 4!:1  name list */


F1(jtscind){A*wv,x,y,z;I n,*zv;L*v;
 ARGCHK1(w);
 n=AN(w); 
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 wv=AAV(w); 
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
 DO(n, x=wv[i]; RE(y=stdnm(x)); ASSERTN(y,EVILNAME,nfs(AN(x),CAV(x))); v=syrd(y,jt->locsyms); RESETERR; zv[i]=v?v->sn:-1;);
 RETF(z);
}    /* 4!:4  script index */


static A jtnch1(J jt,B b,A w,I*pm,A ch){A*v,x,y;C*s,*yv;LX *e;I i,k,m,p,wn;L*d;
 ARGCHK1(w);
 wn=AN(w); e=LXAV0(w);                                /* locale                */
 x=(A)(*e+JT(jt,sympv))->name; p=AN(x); s=NAV(x)->s;  /* locale name/number           */
 m=*pm; v=AAV(ch)+m;                               /* result to appended to */
 for(i=SYMLINFOSIZE;i<wn;++i,++e)if(*e){
  d=SYMNEXT(*e)+JT(jt,sympv);
  while(1){
   if(LCH&d->flag&&d->name&&d->val){
    d->flag^=LCH;
    if(b){
     if(m==AN(ch)){RZ(ch=ext(0,ch)); v=m+AAV(ch);}
     x=d->name; k=NAV(x)->m;
     GATV0(y,LIT,k+2+p,1); yv=CAV(y); 
     MC(yv,NAV(x)->s,k); MC(1+k+yv,s,p); yv[k]=yv[1+k+p]='_';
     *v++=incorp(y); ++m;
    }
   }
   if(!d->next)break;
   d=SYMNEXT(d->next)+JT(jt,sympv);
  }
 }
 *pm=m;
 R ch;
}

F1(jtnch){A ch;B b;LX *e;I i,m,n;L*d;
 RZ(w=cvt(B01,w)); ASSERT(!AR(w),EVRANK); b=BAV(w)[0];
 GAT0(ch,BOX,20,1); m=0;
 if(JT(jt,stch)){
  n=AN(JT(jt,stloc)); e=SYMLINFOSIZE+LXAV0(JT(jt,stloc));
  // named locales first
  for(i=1;i<n;++i,++e)if(*e){
   d=SYMNEXT(*e)+JT(jt,sympv);
   NOUNROLL while(1){
    RZ(ch=nch1(b,d->val,&m,ch));
    if(!d->next)break;
    d=SYMNEXT(d->next)+JT(jt,sympv);
  }}
  // now numbered locales
  DO(jtcountnl(jt), A loc=jtindexnl(jt,i); if(loc)RZ(ch=nch1(b,loc,&m,ch)););
 }
 JT(jt,stch)=b;
 AN(ch)=AS(ch)[0]=m;
 R grade2(ch,ope(ch));
}    /* 4!:5  names changed */


F1(jtex){A*wv,y,z;B*zv;I i,n;L*v;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(((n-1)|SGNIF(AT(w),BOXX))<0,EVDOMAIN);
 GATV(z,B01,n,AR(w),AS(w)); zv=BAV(z);
 for(i=0;i<n;++i){
  RE(y=stdnm(wv[i]));
  zv[i]=1&&y;
  // If the name is defined and is an ACV, invalidate all looked-up ACVs
  // If the value is at large in the stacks and not deferred-freed, increment the use count and deferred-free it
  // If the name is assigned in a local symbol table, we ASSUME it is at large in the stacks and incr/deferred-free it.  We sidestep the nvr stack for local nouns
  if(y&&(v=syrd(y,jt->locsyms))){
   if(jt->uflags.us.cx.cx_c.db)RZ(redef(mark,v));
   A locfound=syrdforlocale(y);  // get the locale in which the name is defined
   if(!(AFLAG(v->val)&AFNJA+AFVIRTUAL)){I am,nam;  // If the AM field is not under name semantics, just go free the name immediately.  Virtuals cannot be on the NVR stack
    // it is still possible that the value is LABANDONED, if it has never been reassigned.  We are about to delete it, so it is safe to switch to NVR semantics
    AMNVRCINI(v->val);  // establish NCR semantics in AM field.  If this block is LABANDONED it MUST go away when the sentence ends
    // process the AM field as if for an assignment.  Since we must free the name & value below, here we do ras() to protect the value where the assignment code does NOT free the value
    // we treat assignment to local sym as a NON-FINAL assignment so that it will be deferred
    AMNVRFREEACT(v->val,locfound!=jt->locsyms,am,nam)   // analyze AM
    if(!(am==nam)){  // look at AM field, loaded into AM.  If value is on NVR and already deferred-free, OR if not on NVR and this is final assignment, we can free the value.  skip the next block
     // Here the free must be deferred.
     if(unlikely((am&-AMNVRCT)==0)){  // deferral needed, but NVR stack empty.  push then
      A nvra=jt->nvra;
      if(unlikely((I)(jt->parserstackframe.nvrtop+1U) > AN(nvra))){RZ(extnvr((L*)1)); nvra=jt->nvra;}  // Extend nvr stack if necessary.  copied from parser
      AAV1(nvra)[jt->parserstackframe.nvrtop++] = v->val;   // record the place where the value was protected (i. e. this sentence); it will be freed when this sentence finishes
     }
     // if the block was on the NVR stack and not freed, we have marked it freed and we will just wait for the eventual deletion
     ras(v->val);
    }
   }
   L *zombsym; if(unlikely((zombsym=jtprobedel((J)((I)jt+NAV(v->name)->m),NAV(v->name)->s,NAV(v->name)->hash,locfound))!=0)){fa(zombsym->name); zombsym->name=0;};  // delete the symbol (incl name and value) in the locale in which it is defined; leave orphan value with no name
             // if the probe returns nonzero, it was a cached value which is not unmoored: we must free the name
  }
 }
 RETF(z);
}    /* 4!:55 expunge */
