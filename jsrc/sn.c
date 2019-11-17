/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Names                                                     */

#include "j.h"

// validate fullname (possibly locative).  s->name, n=length.  Returns 1 if name valid, 0 if not
B jtvnm(J jt,I n,C*s){C c,d,t;I j,k;
 if(!(n))R 0;  // error if empty string
 c=*s; d=*(s+n-1);   // c = first char of name, d is the last
 if(!(CA==ctype[(UC)c]))R 0;   // first char must be alphabetic
 // c='a';    // Now c='this character', d='previous character'; assign c to harmless value (not needed)
 j=0;  // Init no indirect locative found
 // scan the string: verify all remaining characters alphameric (incl _); set j=index of first indirect locative (pointing to the __), or 0 if no ind loc
  // (the string can't start with _)
 DO(n, d=c; c=s[i]; t=ctype[(UC)c]; if(!(t==CA||t==C9))R 0; if(c=='_'&&d=='_'&&!j&&i!=n-1){j=i-1;});
 // If the last char is _, any ind loc is invalid; scan to find previous _ (call its index j, error if 0); audit locale name, or OK if empty (base locale)
 if(c=='_'){if(!(!j))R 0; DQ(j=n-1, if('_'==s[--j])break;); if(!(j))R 0; k=n-j-2; R(!k||vlocnm(k,s+j+1));}
 // Here last char was not _, and j is still pointed after __ if any
 if(j==0)R 1;  // If no ind loc, OK
 // There is an indirect locative.  Scan all of them, verifying first char of each name is alphabetic (all chars were verified alphameric above)
 // Also verify that any _ is preceded or followed by _
 DO(n-j-1, if(s[j+i]=='_'){if(s[j+i+1]=='_'){if(!(CA==ctype[(UC)s[j+i+2]]))R 0;}else{if(!(s[j+i-1]=='_'))R 0;}});
 R 1;
}    /* validate name s, return 1 if name well-formed or 0 if error */

B vlocnm(I n,C*s){
 I accummask=0; DO(n, UC c=s[i]; C t=ctype[c]; t=c=='_'?CX:t; accummask|=(I)1<<t;)  // create mask of types encountered.  Treat  '_' as nonalpha
 if(accummask&~(((I)1<<CA)|((I)1<<C9)))R 0;  // error if any non-alphameric encountered
 if(n<2)R (B)n;  // error if n=0; OK if n=1 (any alphameric is OK then)
 if(s[0]>'9')R 1;  // if nonnumeric locale, alphameric name must be OK
 if(s[0]=='0'||n>(SZI==8?18:9))R 0;  // numeric locale: if (multi-digit) locale starts with '0', or number is too long for an INt (conservatively), error
 R accummask==((I)1<<C9);   // if there are any alphabetics, give error
}    /* validate locale name: 1 if locale-name OK, 0 if error */

static C argnames[7]={'m','n','u','v','x','y',0};
// s-> a string of length n.  If the name is valid, create a NAME block for it
// Possible errors: EVILNAME, EVLIMIT (if name too long), or memory error
A jtnfs(J jt,I n,C*s){A z;C f,*t;I m,p;NM*zv;
 // Discard leading and trailing blanks.  Leave t pointing to the last character
 DQ(n, if(' '!=*s)break; ++s; --n;); 
 t=s+n-1;
 DQ(n, if(' '!=*t)break; --t; --n;);
 // If the name is the special x y.. or x. y. ..., return a copy of the preallocated block for that name (we may have to add flags to it)
// obsolete  c=*s;if((1==n)&&strchr("mnuvxy",c)){
// obsolete   R ca(c=='y'?ynam:c=='x'?xnam:c=='v'?vnam:c=='u'?unam:c=='n'?nnam:mnam);
 C *nmp;if((1==n)&&(nmp=strchr(argnames,*s))){  // if an argument name
  R ca(mnuvxynam[nmp-argnames]);  // return a clone of the argument block (because flags may be added)
 }
 ASSERT(n,EVILNAME);   // error if name is empty
 // The name may not be valid, but we will allocate a NAME block for it anyway
 GATV0(z,NAME,n,1); zv=NAV(z);   // the block is cleared to 0
 MC(zv->s,s,n); *(n+zv->s)=0;  // copy in the name, null-terminate it
 f=0; m=n; p=0;
 // Split name into simplename and locale, verify length of each; set flag and hash for locative/indirect locative
 if('_'==*t){
   // name ends with _: direct locative
   --t; while(s<t&&'_'!=*t)--t; f=NMLOC; p=n-2-(t-s); m=n-(2+p);  // t->_ before localename, p=#locale name, m=#simplename
   // install hash/number for the direct locale
   zv->bucketx=BUCKETXLOC(p,t+1);  // number if numeric, hash otherwise
 }else{
   // otherwise either simple name or indirect locative.  Look for the __
   DO(n, if('_'==s[i]&&'_'==s[1+i]){ f=NMILOC; p=n-2-i; for(m=n; s[m-1]!='_'||s[m-2]!='_';--m); zv->bucketx=(I)nmhash(n-m,s+m); m=n-(2+p); break;});  // p=#locales, m=#simplename, hash last indirect if there is one
 }
 ASSERT(m<=255&&p<=255,EVLIMIT);  // error if name too long.
 zv->flag=f;  // Install locative flag
 zv->m=(UC)m; zv->hash=(UI4)nmhash(m,s); // Install length, and calculate hash of simple name
 RETF(z);
}    /* name from string */

// string from name: returns string for the name
// if b&SFNSIMPLEONLY, return only the simple name
A jtsfn(J jt,B b,A w){NM*v; RZ(w); v=NAV(w); R str(b&SFNSIMPLEONLY?v->m:AN(w),v->s);}

// string from name evocation: returns string for name UNLESS the name was an NMDOT type; in that case it returns w f. which will be a verb
A jtsfne(J jt,A w){RZ(w); A wn=FAV(w)->fgh[0]; /* obsolete NM *v=NAV(wn);*/ if(AT(wn)&NAMEBYVALUE/*obsolete v->flag&NMDOT*/)R fix(w,zeroionei[0]); R sfn(0,wn);}


F1(jtnfb){A y;C*s;I n;
 RZ(w);
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
 RZ(w);
 n=AN(w); wv=AAV(w);   // n=#names  wv->first box
 ASSERT(!n||BOX&AT(w),EVDOMAIN);   // verify boxed input (unless empty)
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);   // Allocate z=result, same shape as input; zv->first result
 for(i=0;i<n;++i){   // for each name...
  RE(y=stdnm(wv[i]));  // point to (the possibly relative) name, audit for validity
  if(y){if(v=syrd(y)){x=v->val; t=AT(x);}else{x=0; if(jt->jerr){y=0; RESETERR;}}}  // If valid, see if the name is defined
  // syrd can fail if a numbered locative is retrograde.  Call that an invalid name, rather than an error, here; thus the RESETERR
  // kludge: if the locale is not defined, syrd will create it.  Better to use a version/parameter to syrd to control that?
  //   If that were done, we could dispense with the error check here (but invalid locale would be treated as undefined rather than invalid).
  // Would have to mod locindirect too
  I zc=2; zc=(0x21c>>((t>>(VERBX-1))&0xe))&3;   // C A V N = 4 2 1 0 -> 2 1 3 0    10 xx 01 11 00
  zc=x?zc:-1; zc=y?zc:-2;
// obsolete   zv[i]=!y?-2:!x?-1:t&NOUN?0:t&VERB?3:t&ADV?1:2;  // calculate the type, store in result array
  zv[i]=zc;  // calculate the type, store in result array
 }
 RETF(z);
}    /* 4!:0  name class */


static SYMWALK(jtnlxxx, A,BOX,20,1, jt->workareas.namelist.nla[*((UC*)NAV(d->name)->s)]&&jt->workareas.namelist.nlt&AT(d->val), 
    RZ(*zv++=rifvs(sfn(SFNSIMPLEONLY,d->name))) )

       SYMWALK(jtnlsym, A,BOX,20,1, jt->workareas.namelist.nla[*((UC*)NAV(d->name)->s)],
    RZ(*zv++=rifvs(sfn(SFNSIMPLEONLY,d->name))) )

static I nlmask[] = {NOUN,ADV,CONJ,VERB, MARK,MARK,SYMB,MARK};

static F1(jtnlx){A z=mtv;B b;I m=0,*v,x;
 RZ(w=vi(w)); v=AV(w); 
 DQ(AN(w), x=*v++; m|=nlmask[x<0||6<x?7:x];); 
 jt->workareas.namelist.nlt=m&RHS; b=1&&jt->workareas.namelist.nlt&RHS;
 ASSERT(!(m&MARK),EVDOMAIN);
 if(b           )RZ(z=nlxxx(jt->global));
 if(b&&(AN(jt->locsyms)>1))RZ(z=over(nlxxx(jt->locsyms),z));
 if(m==SYMB     )RZ(z=over(nlsym(jt->stloc),z));
 R nub(grade2(z,ope(z)));
}

F1(jtnl1){memset(jt->workareas.namelist.nla,C1,256L); R nlx(w);}
     /* 4!:1  name list */

F2(jtnl2){UC*u;
 RZ(a&&w);
 ASSERT(LIT&AT(a),EVDOMAIN);
 memset(jt->workareas.namelist.nla,C0,256L); 
 u=UAV(a); DQ(AN(a),jt->workareas.namelist.nla[*u++]=1;);
 R nlx(w);
}    /* 4!:1  name list */


F1(jtscind){A*wv,x,y,z;I n,*zv;L*v;
 RZ(w);
 n=AN(w); 
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 wv=AAV(w); 
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
 DO(n, x=wv[i]; RE(y=stdnm(x)); ASSERTN(y,EVILNAME,nfs(AN(x),CAV(x))); v=syrd(y); RESETERR; zv[i]=v?v->sn:-1;);
 RETF(z);
}    /* 4!:4  script index */


static A jtnch1(J jt,B b,A w,I*pm,A ch){A*v,x,y;C*s,*yv;LX *e;I i,k,m,p,wn;L*d;
 RZ(w);
 wn=AN(w); e=LXAV0(w);                                /* locale                */
 x=(A)(*e+jt->sympv)->name; p=AN(x); s=NAV(x)->s;  /* locale name/number           */
 m=*pm; v=AAV(ch)+m;                               /* result to appended to */
 for(i=SYMLINFOSIZE;i<wn;++i,++e)if(*e){
  d=*e+jt->sympv;
  while(1){
   if(LCH&d->flag&&d->name&&d->val){
    d->flag^=LCH;
    if(b){
     if(m==AN(ch)){RZ(ch=ext(0,ch)); v=m+AAV(ch);}
     x=d->name; k=NAV(x)->m;
     GATV0(y,LIT,k+2+p,1); yv=CAV(y); 
     MC(yv,NAV(x)->s,k); MC(1+k+yv,s,p); yv[k]=yv[1+k+p]='_';
     *v++=y; ++m;
   }}
   if(!d->next)break;
   d=d->next+jt->sympv;
 }}
 *pm=m;
 R ch;
}

F1(jtnch){A ch;B b;LX *e;I i,m,n;L*d;
 RZ(w=cvt(B01,w)); ASSERT(!AR(w),EVRANK); b=*BAV(w);
 GAT0(ch,BOX,20,1); m=0;
 if(jt->stch){
  n=AN(jt->stloc); e=SYMLINFOSIZE+LXAV0(jt->stloc);
  // named locales first
  for(i=1;i<n;++i,++e)if(*e){
   d=*e+jt->sympv;
   while(1){
    RZ(ch=nch1(b,d->val,&m,ch));
    if(!d->next)break;
    d=d->next+jt->sympv;
  }}
  // now numbered locales
  DO(jtcountnl(jt), A loc=jtindexnl(jt,i); if(loc)RZ(ch=nch1(b,loc,&m,ch)););
 }
 jt->stch=b;
 AN(ch)=*AS(ch)=m;
 R grade2(ch,ope(ch));
}    /* 4!:5  names changed */


F1(jtex){A*wv,y,z;B*zv;I i,n;L*v;I modifierchg=0;
 RZ(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,B01,n,AR(w),AS(w)); zv=BAV(z);
 for(i=0;i<n;++i){
  RE(y=stdnm(wv[i]));
  zv[i]=1&&y;
  // If the name is defined and is an ACV, invalidate all looked-up ACVs
  // If the value is at large in the stacks and not deferred-freed, increment the use count and deferred-free it
  // If the name is assigned in a local symbol table, we ASSUME it is at large in the stacks and incr/deferred-free it.  We sidestep the nvr stack for local nouns
  if(y&&(v=syrd(y))){
   if(jt->uflags.us.cx.cx_c.db)RZ(redef(mark,v));
   A locfound=syrdforlocale(y);  // get the locale in which the name is defined
   if(locfound==jt->locsyms||AFLAG(v->val)&AFNVRUNFREED){  // see if local or NVR
    if(!(AFLAG(v->val)&AFNVR)){
     // The symbol is a local symbol not on the NVR stack.  We must put it onto the NVR stack.
     A *nvrav=jt->nvrav;
     if((jt->parserstackframe.nvrtop+1U) > jt->nvran)RZ(nvrav=extnvr());  // Extend nvr stack if necessary.  copied from parser
     nvrav[jt->parserstackframe.nvrtop++] = v->val;   // record the place where the value was protected; it will be freed when this sentence finishes
     AFLAG(v->val) |= AFNVR;  // mark the value as protected
    }
    AFLAG(v->val)&=~AFNVRUNFREED; ras(v->val);  // indicate deferred free, and protect from the upcoming free
   }
   if(!(v->name->flag&NMDOT)&&v->val&&AT(v->val)&(VERB|ADV|CONJ))modifierchg=1;  // if we delete a modifier, remember that fact
   probedel(NAV(v->name)->m,NAV(v->name)->s,NAV(v->name)->hash,locfound);  // delete the symbol (incl name and value) in the locale in which it is defined
  }
 }
 jt->modifiercounter+=modifierchg;
 RETF(z);
}    /* 4!:55 expunge */
