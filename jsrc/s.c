/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table                                                            */

#include "j.h"


/* a symbol table aka locale is a type INT vector                          */
/* the length is prime and is one of ptab[i]                               */
/* zero elements mean unused entry                                         */
/* non-zero elements are indices in the global symbol pool and             */
/*     are head pointers to a linked list                                  */
/* the first element is symbol pool index for locale info                  */

/* the global symbol pool is a type INT matrix                             */
/* the number of columns is symcol=ceiling(sizeof(L)/sizeof(I))            */
/* elements are interpreted per type L (see jtype.h)                       */
/*  A name - A name on LHS of assignment or locale name                    */
/*  A val  - value        or locale search path                            */
/*  I sn   - script index                                                  */
/*  I flag - various flags                                                 */
/*  I next - pointer to   successor in linked list                         */
/*  I prev - pointer to predecessor in linked list                         */
/*           if no predecessor then pointer to hash table entry, and       */
/*           flag will include LHEAD                                       */
/* a stack of free entries is kept using the next pointer                  */
/* jt->symp:     symbol pool array                                         */
/* jt->sympv:    symbol pool array pointer, (L*)AV(jt->symp)               */
/* jt->symindex: symbol table index (monotonically increasing)             */

/* numbered locales:                                                       */
/* jt->stnum: -1 means free; others are numbers in use                     */
/* jt->stptr:  0 means free; others are symbol tables                      */
/* jt->stused: # entries in stnum/stptr in use                             */
/* jt->stmax:  1 + maximum number extant                                   */

/* named locales:                                                          */
/* jt->stloc:  locales symbol table                                        */

static I symcol=(sizeof(L)+SZI-1)/SZI;

B jtsymext(J jt,B b){A x,y;I j,m,n,s[2],*v,xn,yn;L*u;
 if(b){y=jt->symp; j=((MS*)y-1)->j; n=*AS(y); yn=AN(y);}
 else {            j=12;            n=1;      yn=0;    }
 m=msize[1+j];                              /* new size in bytes           */
 m-=sizeof(MS)+SZI*(AH+2);                  /* less array overhead         */
 m/=symcol*SZI;                             /* new # rows                  */
 s[0]=m; s[1]=symcol; xn=m*symcol;          /* new pool array shape        */
 GA(x,INT,xn,2,s); v=AV(x);                 /* new pool array              */
 if(b)ICPY(v,AV(y),yn);                     /* copy old data to new array  */
 memset(v+yn,C0,SZI*(xn-yn));               /* 0 unused area for safety    */
 u=n+(L*)v; j=1+n;
 DO(m-n-1, u++->next=j++;);                 /* build free list extension   */
 if(b)u->next=jt->sympv->next;              /* push extension onto stack   */
 ((L*)v)->next=n;                           /* new stack top               */
 jt->symp =ra(x);                           /* preserve new array          */
 jt->sympv=(L*)AV(x);                       /* new array value ptr         */
 if(b)fa(y);                                /* release old array           */
 R 1;
}    /* 0: initialize (no old array); 1: extend old array */

L* jtsymnew(J jt,I*hv){I j;L*u,*v;
 while(!(j=jt->sympv->next))RZ(symext(1));  /* extend pool if req'd        */
 jt->sympv->next=(j+jt->sympv)->next;       /* new top of stack            */
 u=j+jt->sympv;
 if(u->next=*hv){v=*hv+jt->sympv; v->prev=j; v->flag^=LHEAD;}
 u->prev=(I)hv; u->flag=LHEAD;
 *hv=j;
 R u;
}    /* allocate a new pool entry and insert into hash table entry hv */

B jtsymfree(J jt,L*u){I q;
 q=u->next;
 if(q)(q+jt->sympv)->prev=u->prev; 
 if(LHEAD&u->flag){*(I*)u->prev=q; if(q)(q+jt->sympv)->flag|=LHEAD;}
 else (u->prev+jt->sympv)->next=q;
 fa(u->name); u->name=0;                    /* zero out data fields        */
 fa(u->val ); u->val =0; u->sn=u->flag=u->prev=0;
 u->next=jt->sympv->next;                   /* point to old top of stack   */
 jt->sympv->next=u-jt->sympv;               /* new          top of stack   */
 R 1;
}    /* free pool entry pointed to by u */

static SYMWALK(jtsymfreeha, B,B01,100,1, 1, RZ(symfree(d)))   /* free pool table entries      */

B jtsymfreeh(J jt,A w,L*v){I*wv;L*u;
 wv=AV(w);
 ASSERTSYS(*wv,"symfreeh");
 u=*wv+jt->sympv; 
 RZ(symfree(u));
 RZ(symfreeha(w));
 memset(wv,C0,AN(w)*SZI);
 fa(w);
 if(v){v->val=0; RZ(symfree(v));}
 R 1;
}    /* free entire hash table w, (optional) pointed by v */


static SYMWALK(jtsympoola, I,INT,100,1, 1, *zv++=j;)

F1(jtsympool){A aa,*pu,q,x,y,*yv,z,*zv;I i,j,n,*u,*v,*xv;L*pv;
 RZ(w); 
 ASSERT(1==AR(w),EVRANK); 
 ASSERT(!AN(w),EVLENGTH);
 GA(z,BOX,3,1,0); zv=AAV(z);
 n=*AS(jt->symp); pv=jt->sympv;
 GA(x,INT,n*6,2,0); *AS(x)=n; *(1+AS(x))=6; xv= AV(x); zv[0]=x;
 GA(y,BOX,n,  1,0);                         yv=AAV(y); zv[1]=y;
 for(i=0;i<n;++i,++pv){         /* per pool entry       */
  *xv++=i;
  *xv++=(q=pv->val)?AT(pv->val):0;
  *xv++=pv->flag;
  *xv++=pv->sn;
  *xv++=pv->next;
  *xv++=pv->prev;
  RZ(*yv++=(q=pv->name)?sfn(1,q):mtv);
 }
 GA(y,BOX,n,1,0); yv=AAV(y); zv[2]=y;
 DO(n, yv[i]=mtv;);
 n=AN(jt->stloc); v=AV(jt->stloc); 
 for(i=0;i<n;++i)if(j=v[i]){    /* per named locales    */
  x=(j+jt->sympv)->val; 
  RZ(yv[j]=yv[*AV(x)]=aa=sfn(1,LOCNAME(x)));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 n=AN(jt->stptr); pu=AAV(jt->stptr);
 for(i=0;i<n;++i)if(x=pu[i]){   /* per numbered locales */
  RZ(      yv[*AV(x)]=aa=sfn(1,LOCNAME(x)));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 if(x=jt->local){               /* per local table      */
  RZ(      yv[*AV(x)]=aa=cstr("**local**"));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 R z;
}    /* 18!:31 symbol pool */


// a is A for name, g is symbol table
// result is L* address of the symbol-table entry for the name, or 0 if not found
L*jtprobe(J jt,A a,A g){C*s;I*hv,k,m;L*v;NM*u;
 RZ(a&&g);
 u=NAV(a); m=u->m; s=u->s; k=u->hash%AN(g); hv=AV(g)+(k?k:1);
 if(!*hv)R jt->cursymb=0;                            /* (0) empty slot    */
 v=*hv+jt->sympv;
 while(1){
  u=NAV(v->name);
  if(m==u->m&&!memcmp(s,u->s,m))R jt->cursymb=v;     /* (1) exact match   */
  if(!v->next)R jt->cursymb=0;                       /* (2) link list end */
  v=v->next+jt->sympv;
}}

// a is A for name
// g is symbol table to use
// result is L* symbol-table entry to use, also stored in jt->cursymb
// if not found, on is created
static L*jtprobeis(J jt,A a,A g){C*s;I*hv,k,m;L*v;NM*u;
 u=NAV(a); m=u->m; s=u->s; k=u->hash%AN(g); hv=AV(g)+(k?k:1);
 if(*hv){                                 /* !*hv means (0) empty slot    */
  v=*hv+jt->sympv;
  while(1){                               
   u=NAV(v->name);
   if(m==u->m&&!memcmp(s,u->s,m))R jt->cursymb=v;    /* (1) exact match   */
   if(!v->next)break;                                /* (2) link list end */
   v=v->next+jt->sympv;
 }}
// not found, create new symbol
 RZ(v=symnew(hv)); 
 v->name=ra(a);  // point symbol table to the name block, and increment its use count accordingly
 R jt->cursymb=v;
}    /* probe for assignment */

// look up a non-locative name using the locale path
// a is the name, g is the current locale, b is 1 if we should look in local nametable first
// result is L* symbol-table slot for the name, or 0 if none
// side effect: if the name is found in the local symbol table, av->e in the name is set to the symbol table entry that was found
static L*jtsyrd1(J jt,A a,A g,B b){A*v,x,y;L*e;NM*av;
 if(b&&jt->local&&(e=probe(a,jt->local))){av=NAV(a); R av->e=e;}  // return if found local
 RZ(g);  // make sure there is a locale...
 if(e=probe(a,g))R e;  // and if the name is defined there, use it.  Note av->e is not set here
 RZ(y = LOCPATH(g));   // Not found in locale.  We must use the path
 v=AAV(y); 
 DO(AN(y), x=v[i]; if(e=probe(a,stfind(1,AN(x),CAV(x))))break;);  // return when name found.  Create path locale if it does not exist
 R e;  // not found
}    /* find name a where the current locale is g */ 

// u is address of indirect locative: in a__b__c, it points to the b
// n is the length of the entire locative (4 in this example)
// result is address of symbol table to use for name lookup (if not found, it is created)
static A jtlocindirect(J jt,I n,C*u){A a,g=jt->global,x,y;B lcl=1;C*s,*v,*xv;I k,xn;L*e;
 s=n+u;
 while(u<s){
  v=s; while('_'!=*--v); ++v;
  k=s-v; s=v-2; RZ(a=nfs(k,v));
  e=syrd1(a,g,lcl); lcl=0; 
  ASSERTN(e,EVVALUE,a);
  y=e->val;  
  ASSERTN(!AR(y),EVRANK,a); 
  ASSERTN(BOX&AT(y),EVDOMAIN,a);
  x=AAV0(y); xn=AN(x); xv=CAV(x); 
  ASSERTN(1>=AR(x),EVRANK,a); 
  ASSERTN(xn,EVLENGTH,a);
  ASSERTN(LIT&AT(x),EVDOMAIN,a); 
  ASSERTN(vlocnm(xn,xv),EVILNAME,a);
  RZ(g=stfind(1,xn,xv));  // find locale st, creating it if not found
 }
 R g;
}

// look up a name (either simple or locative) using the full name resolution
// result is symbol-table slot for the name if found, or 0 if not found
// side effect: if symb is nonzero, *symb is set to the symbol table that was the base of the path
L*jtsyrd(J jt,A a,A*symb){A g=jt->global;I m,n;NM*v;
 RZ(a);
 n=AN(a); v=NAV(a); m=v->m;
 if(n>m)RZ(g=NMILOC&v->flag?locindirect(n-m-2,2+m+v->s):stfind(1,n-m-2,1+m+v->s))
   // if locative, find the indirect locale to start on, or the named locale, creating the locale if not found
 if(symb)*symb=g;  // save the starting locale if the user wants to
 R syrd1(a,g,(B)(n==m));  // look up the name in that locale; if not a locative, look in the local symbol table first
}


static A jtdllsymaddr(J jt,A w,C flag){A*wv,x,y,z;I i,n,wd,*zv;L*v;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GA(z,INT,n,AR(w),AS(w)); zv=AV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); v=syrd(nfs(AN(x),CAV(x)),0L); 
  ASSERT(v,EVVALUE);
  y=v->val;
  ASSERT(NOUN&AT(y),EVDOMAIN);
  zv[i]=flag?(I)AV(y):(I)v;
 }
 R z;
}    /* 15!:6 (0=flag) or 15!:14 (1=flag) */

F1(jtdllsymget){R dllsymaddr(w,0);}
F1(jtdllsymdat){R dllsymaddr(w,1);}

// look up the name w using full name resolution.  Return the value if found, abort if not found
F1(jtsymbrd){L*v; RZ(w); ASSERTN(v=syrd(w,0L),EVVALUE,w); R v->val;}

F1(jtsymbrdlock){A y;
 RZ(y=symbrd(w));
 R FUNC&AT(y)&&(jt->glock||VLOCK&VAV(y)->flag)?nameref(w):y;
}

B jtredef(J jt,A w,L*v){A f,oldn;DC c,d;
 d=jt->sitop; while(d&&!(DCCALL==d->dctype&&d->dcj))d=d->dclnk; if(!(d&&DCCALL==d->dctype&&d->dcj))R 1;
 oldn=jt->curname;
 if(v==(L*)d->dcn){
  jt->curname=d->dca; f=d->dcf;
  ASSERT(AT(f)==AT(w)&&(CCOLON==VAV(f)->id)==(CCOLON==VAV(w)->id),EVSTACK);
  d->dcf=w;
  if(CCOLON==VAV(w)->id)jt->redefined=(I)v;
  c=jt->sitop; while(c&&DCCALL!=c->dctype){c->dctype=DCJUNK; c=c->dclnk;}
 }
 c=d; while(c=c->dclnk){jt->curname=c->dca; ASSERT(!(DCCALL==c->dctype&&v==(L*)c->dcn),EVSTACK);}
 jt->curname=oldn;
 R 1;
}    /* check for changes to stack */

// assign symbol: assign name a in symbol table g to the value w
// Non-error result is unused (mark)
A jtsymbis(J jt,A a,A w,A g){A x;I m,n,wn,wr,wt;NM*v;L*e;V*wv;
 RZ(a&&w&&g);
 n=AN(a); v=NAV(a); m=v->m;  // n is length of name, v points to string value of name, m is length of non-locale part of name
 if(n==m)ASSERT(!(jt->local&&g==jt->global&&probe(a,jt->local)),EVDOMAIN)  // if non-locative, give error if there is a private
    // symbol table, and we are assigning to the global symbol table, and the name is defined in the local table
 else{C*s=1+m+v->s; RZ(g=NMILOC&v->flag?locindirect(n-m-2,1+s):stfind(1,n-m-2,s));}
    // locative: s is the length of name_.  Find the symbol table to use, creating one if none found
 // Now g has the symbol table to look in
 RZ(e=probeis(a,g));   // set e to symbol-table slot to use
 if(jt->db)RZ(redef(w,e));  // if debug, check for changes to stack
 wt=AT(w);   // type of the value
 if(wt&FUNC&&(wv=VAV(w),wv->f)){if(wv->id==CCOLON)wv->flag|=VNAMED; if(jt->glock)wv->flag|=VLOCK;}
   // If the value is a function created by n : m, this becomes a named function; if running a locked function, this is locked too.
   // kludge  these flags are modified in the input area (w), which means they will be improperly set in the result of the
   // assignment (ex: (nm =: 3 : '...') y).  There seems to be no ill effect, because VNAMED isn't used much.
 x=e->val;   // if x is 0, this name has not been assigned yet; if nonzero, x points to the value
 ASSERT(!(x&&AFRO&AFLAG(x)),EVRO);   // error if read-only value
 if(!(x&&AFNJA&AFLAG(x))){
  // name to be assigned is undefined, or is defined as a normal J name
  // Increment the use count of the value being assigned, to reflect the fact that the assigned name will refer to it.
  // For boxed arguments, rca may create a clone of the boxing tree.  kludge It's a pity to do this if the value doesn't need to be copied
  RZ(w=ra(AFNJA&AFLAG(w)?w:rca(w)));
  // If this is a reassignment, we need to decrement the use count in the old name, since that value is no longer used.
  // But if the value of the name is 'out there' in the sentence (coming from an earlier reference), we'd better not delete
  // that value until its last use.  So we defer the decrementing: we call nvrredef to INCREMENT the use count if the name is
  // extant, and then unconditionally decrement the use count.  Later, when it's safe, we will go back to decrement the
  // use count in the values we incremented here.  Yes, this could be done a smidgen faster, but that would require another
  // version of nvrredef, and redefinitions are rare enough to make that not worthwhile.
  if(x){nvrredef(x); fa(x);}   // if redefinition, modify the use counts
  e->val=w;  // Now that the old value has been taken care of, install the new value
 }else if(x!=w){  /* replacing mapped data */
  if(wt&BOX)R smmis(x,w);
  wn=AN(w); wr=AR(w); m=wn*bp(wt);
  ASSERT(wt&B01+INT+FL+CMPX+LIT,EVDOMAIN);
  ASSERT(AM(x)>=m,EVALLOC);
  AT(x)=wt; AN(x)=wn; AR(x)=wr; ICPY(AS(x),AS(w),wr); MC(AV(x),AV(w),m);
 }
 e->sn=jt->slisti;  // Save the script in which this name was defined
 if(jt->stch&&(m<n||jt->local!=g&&jt->stloc!=g))e->flag|=LCH;  // update 'changed' flag if enabled
 R mark;   // Return not meaningful
}    /* a: name; w: value; g: symbol table */
