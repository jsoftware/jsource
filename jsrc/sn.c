/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Names                                                     */

#include "j.h"

// validate fullname (possibly locative).  s->name, n=length.  Returns 1 if name valid, 0 if not
B jtvnm(J jt,I n,C*s){C c,d,t;I j,k;
 RZ(n);  // error if empty string
 c=*s; d=*(s+n-1);   // c = first char of name, d is the last
 if(jt->dotnames&&2==n&&'.'==d&&('m'==c||'n'==c||'u'==c||'v'==c||'x'==c||'y'==c))R 1;  // if x. y. ..., that's OK
 RZ(CA==ctype[(UC)c]);   // first char must be alphabetic
 // c='a';    // Now c='this character', d='previous character'; assign c to harmless value (not needed)
 j=0;  // Init no indirect locative found
 // scan the string: verify all remaining characters alphameric (incl _); set j=index of first indirect locative (pointing to the __), or 0 if no ind loc
  // (the string can't start with _)
 DO(n, d=c; c=s[i]; t=ctype[(UC)c]; RZ(t==CA||t==C9); if(c=='_'&&d=='_'&&!j&&i!=n-1){j=i-1;});
 // If the last char is _, any ind loc is invalid; scan to find previous _ (call its index j, error if 0); audit locale name, or OK if empty (base locale)
 if(c=='_'){RZ(!j); DO(j=n-1, if('_'==s[--j])break;); RZ(j); k=n-j-2; R(!k||vlocnm(k,s+j+1));}
 // Here last char was not _, and j is still pointed after __ if any
 if(j==0)R 1;  // If no ind loc, OK
 // There is an indirect locative.  Scan all of them, verifying first char of each name is alphabetic (all chars were verified alphameric above)
 // Also verify that any _ is preceded or followed by _
 DO(n-j-1, if(s[j+i]=='_'){if(s[j+i+1]=='_'){RZ(CA==ctype[(UC)s[j+i+2]]);}else{RZ(s[j+i-1]=='_');}});
 R 1;
}    /* validate name s, return 1 if name well-formed or 0 if error */

B vlocnm(I n,C*s){C c,t;
 if(!n)R 0;  // error is name empty
 DO(n, t=ctype[(UC)(c=s[i])]; RZ(c!='_'&&(t==CA||t==C9)););  // error if non-alphameric or _
 if(C9==ctype[(UC)*s]){RZ('0'!=*s||1==n); DO(n, c=s[i]; RZ('0'<=c&&c<='9'););}  // if numeric locale, verify first char not '0' unless it's just 1 char; and all chars numeric
 R 1;
}    /* validate locale name: 1 if locale-name OK, 0 if error */

// s-> a string of length n.  If the name is valid, create a NAME block for it
// Possible errors: EVILNAME, EVLIMIT (if name too long), or memory error
A jtnfs(J jt,I n,C*s){A z;C c,f,*t;I m,p;NM*zv;
 // Discard leading and trailing blanks.  Leave t pointing to the last character
 DO(n, if(' '!=*s)break; ++s; --n;); 
 t=s+n-1;
 DO(n, if(' '!=*t)break; --t; --n;);
 // If the name is the special x y.. or x. y. ..., return a copy of the preallocated block for that name (we may have to add flags to it)
 c=*s;if((1==n||2==n&&'.'==s[1])&&strchr("mnuvxy",c)){
  if(1==n){R ca(c=='y'?ynam:c=='x'?xnam:c=='v'?vnam:c=='u'?unam:c=='n'?nnam:mnam);
  }else{    R ca(c=='y'?ydot:c=='x'?xdot:c=='v'?vdot:c=='u'?udot:c=='n'?ndot:mdot);}
 }
 ASSERT(n,EVILNAME);   // error if name is empty
 // The name may not be valid, but we will allocate a NAME block for it anyway
 GATV(z,NAME,n,1,0); zv=NAV(z);
 MC(zv->s,s,n); *(n+zv->s)=0;  // copy in the name, null-terminate it
 f=0; m=n; p=0;
 // Split name into simplename and locale, verify length of each; set flag for locative/indirect locative
 if('_'==*t){--t; while(s<t&&'_'!=*t)--t; f=NMLOC;  p=n-2-(t-s); m=n-(2+p);}  // t->_ before localename, p=#locale name, m=#simplename
 else DO(n, if('_'==s[i]&&'_'==s[1+i]){   f=NMILOC; p=n-2-i;     m=n-(2+p); break;});  // p=#locales, m=#simplename
 ASSERT(m<=255&&p<=255,EVLIMIT);  // error if name too long.  NOTE kludge: fails if total length of __locs exceeds 255
 zv->flag=f;  // Install locative flag
 zv->m=(UC)m; zv->hash=nmhash(m,s); // Install length, and calculate quick-and-dirty CJS hash of name
 R z;
}    /* name from string */

A jtsfn(J jt,B b,A w){NM*v; RZ(w); v=NAV(w); R str(b?v->m:AN(w),v->s);}
     /* string from name: 0=b full name; 1=b non-locale part of name */

F1(jtnfb){A y;C*s;I n;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(!AR(w),EVRANK);
 RZ(y=vs(ope(w)));
 n=AN(y); s=CAV(y);
 ASSERTN(vnm(n,s),EVILNAME,nfs(n,s));
 R nfs(n,s);
}    /* name from scalar boxed string */

// w is an A for a name; return NAME block or 0 if error
static F1(jtstdnm){C*s;I j,n,p,q;
 RZ(w=vs(w));  // convert to ASCII
 n=AN(w); s=CAV(w);  // n = #characters, s->string
 RZ(n);
 j=0;   DO(n, if(' '!=s[j++])break;); p=j-1;
 j=n-1; DO(n, if(' '!=s[j--])break;); q=(n-2)-j;
 RZ(vnm(n-(p+q),p+s));   // Validate name
 R nfs(n-(p+q),p+s);   // Create NAME block for name
}    /* 0 result means error or invalid name */

// x is a (possibly) boxed string; result is NAME block for name x, error if invalid name
F1(jtonm){A x,y; RZ(x=ope(w)); y=stdnm(x); ASSERTN(y,EVILNAME,nfs(AN(x),CAV(x))); R y;}

// w is array of boxed strings; result is name class for each
F1(jtnc){A*wv,x,y,z;I i,n,t,wd,*zv;L*v; 
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);  // n=#names  wv->first box
 ASSERT(!n||BOX&AT(w),EVDOMAIN);   // verify boxed input (unless empty)
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);   // Allocate z=result, same shape as input; zv->first result
 for(i=0;i<n;++i){   // for each name...
  RE(y=stdnm(WVR(i)));  // point to (the possibly relative) name, audit for validity
  if(y){if(v=syrd(y,0L)){x=v->val; t=AT(x);}else{x=0; if(jt->jerr){y=0; RESETERR;}}}  // If valid, see if the name is defined
  // syrd can fail if a numbered locative is retrograde.  Call that an invalid name, rather than an error, here; thus the RESETERR
  // kludge: if the locale is not defined, syrd will create it.  Better to use a version/parameter to syrd to control that?
  //   If that were done, we could dispense with the error check here (but invalid locale would be treated as undefined rather than invalid).
  // Would have to mod locindirect too
  zv[i]=!y?-2:!x?-1:t&NOUN?0:t&VERB?3:t&ADV?1:2;  // calculate the type, store in result array
 }
 R z;
}    /* 4!:0  name class */


static SYMWALK(jtnlxxx, A,BOX,20,1, jt->nla[*((UC*)NAV(d->name)->s)]&&jt->nlt&AT(d->val), 
    RZ(*zv++=sfn(1,d->name)) )

       SYMWALK(jtnlsym, A,BOX,20,1, jt->nla[*((UC*)NAV(d->name)->s)],
    RZ(*zv++=sfn(1,d->name)) )

static I nlmask[] = {NOUN,ADV,CONJ,VERB, MARK,MARK,SYMB,MARK};

static F1(jtnlx){A z=mtv;B b;I m=0,*v,x;
 RZ(w=vi(w)); v=AV(w); 
 DO(AN(w), x=*v++; m|=nlmask[x<0||6<x?7:x];); 
 jt->nlt=m&RHS; b=1&&jt->nlt&RHS;
 ASSERT(!(m&MARK),EVDOMAIN);
 if(b           )RZ(z=nlxxx(jt->global));
 if(b&&jt->local)RZ(z=over(nlxxx(jt->local),z));
 if(m==SYMB     )RZ(z=over(nlsym(jt->stloc),z));
 R nub(grade2(z,ope(z)));
}

F1(jtnl1){memset(jt->nla,C1,256L); R nlx(w);}
     /* 4!:1  name list */

F2(jtnl2){UC*u;
 RZ(a&&w);
 ASSERT(LIT&AT(a),EVDOMAIN);
 memset(jt->nla,C0,256L); 
 u=UAV(a); DO(AN(a),jt->nla[*u++]=1;);
 R nlx(w);
}    /* 4!:1  name list */


F1(jtscind){A*wv,x,y,z;I n,wd,*zv;L*v;
 RZ(w);
 n=AN(w); 
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
 DO(n, x=WVR(i); RE(y=stdnm(x)); ASSERTN(y,EVILNAME,nfs(AN(x),CAV(x))); v=syrd(y,0L); RESETERR; zv[i]=v?v->sn:-1;);
 R z;
}    /* 4!:4  script index */


static A jtnch1(J jt,B b,A w,I*pm,A ch){A*v,x,y;C*s,*yv;I*e,i,k,m,p,wn;L*d;
 RZ(w);
 wn=AN(w); e=AV(w);                                /* locale                */
 x=(A)(*e+jt->sympv)->name; p=AN(x); s=NAV(x)->s;  /* locale name           */
 m=*pm; v=AAV(ch)+m;                               /* result to appended to */
 for(i=1;i<wn;++i,++e)if(*e){
  d=*e+jt->sympv;
  while(1){
   if(LCH&d->flag&&d->name&&d->val){
    d->flag^=LCH;
    if(b){
     if(m==AN(ch)){RZ(ch=ext(0,ch)); v=m+AAV(ch);}
     x=d->name; k=NAV(x)->m;
     GATV(y,LIT,k+2+p,1,0); yv=CAV(y); 
     MC(yv,NAV(x)->s,k); MC(1+k+yv,s,p); yv[k]=yv[1+k+p]='_';
     *v++=y; ++m;
   }}
   if(!d->next)break;
   d=d->next+jt->sympv;
 }}
 *pm=m;
 R ch;
}

F1(jtnch){A ch,*pv;B b;I*e,i,m,n;L*d;
 RZ(w=cvt(B01,w)); ASSERT(!AR(w),EVRANK); b=*BAV(w);
 GAT(ch,BOX,20,1,0); m=0;
 if(jt->stch){
  n=AN(jt->stloc); e=1+AV(jt->stloc); pv=AAV(jt->stptr);
  for(i=1;i<n;++i,++e)if(*e){
   d=*e+jt->sympv;
   while(1){
    RZ(ch=nch1(b,d->val,&m,ch));
    if(!d->next)break;
    d=d->next+jt->sympv;
  }}
  n=AN(jt->stptr);
  DO(n, if(pv[i])RZ(ch=nch1(b,pv[i],&m,ch)););
 }
 jt->stch=b;
 AN(ch)=*AS(ch)=m;
 R grade2(ch,ope(ch));
}    /* 4!:5  names changed */


F1(jtex){A*wv,y,z;B*zv;I i,n,wd;L*v;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,B01,n,AR(w),AS(w)); zv=BAV(z);
 for(i=0;i<n;++i){
  RE(y=stdnm(WVR(i)));
  zv[i]=1&&y;
  // If the value is at large in the stacks, increment the use count and call for a later decrement
  if(y&&(v=syrd(y,0L))){if(jt->db)RZ(redef(mark,v)); if(nvrredef(v->val))ra(v->val); RZ(symfree(v));}
 }
 R z;
}    /* 4!:55 expunge */
