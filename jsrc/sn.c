/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Names                                                     */

#include "j.h"


B jtvnm(J jt,I n,C*s){B b=0;C c,d,t;I j,k;
 RZ(n);
 c=*s; d=*(s+n-1);
 if(jt->dotnames&&2==n&&'.'==d&&('m'==c||'n'==c||'u'==c||'v'==c||'x'==c||'y'==c))R 1;
 RZ(CA==ctype[c]);
 c='a'; 
 DO(n, d=c; c=s[i]; t=ctype[c]; RZ(t==CA||t==C9); if(c=='_'&&d=='_'&&!b&&i!=n-1){j=1+i; b=1;});
 if(c=='_'){DO(j=n-1, if('_'==s[--j])break;); k=n-j-2; R!b&&j&&(!k||vlocnm(k,s+j+1));}
 if(!b)R 1;
 k=2; DO(n-j, c=s[j+i]; if(2>k)k+='_'==c; else{RZ(CA==ctype[c]); k=0;});
 R !k;
}    /* validate name s, return type or 0 if error */

B vlocnm(I n,C*s){C c,t; 
 if(!n)R 0;
 DO(n, t=ctype[c=s[i]]; RZ(c!='_'&&(t==CA||t==C9)););
 if(C9==ctype[*s]){RZ(1==n||'0'!=*s); DO(n, c=s[i]; RZ('0'<=c&&c<='9'););}
 R 1;
}    /* validate locale name */

A jtnfs(J jt,I n,C*s){A z;C c,f,*t;I m,p;NM*zv;
 DO(n, if(' '!=*s)break; ++s; --n;); 
 t=s+n-1;
 DO(n, if(' '!=*t)break; --t; --n;);
 c=*s;if((1==n||2==n&&'.'==s[1])&&strchr("mnuvxy",c)){
  if(1==n)R c=='y'?ynam:c=='x'?xnam:c=='v'?vnam:c=='u'?unam:c=='n'?nnam:mnam;
  else    R c=='y'?ydot:c=='x'?xdot:c=='v'?vdot:c=='u'?udot:c=='n'?ndot:mdot;
 }
 ASSERT(n,EVILNAME); 
 GA(z,NAME,n,1,0); zv=NAV(z);
 memcpy(zv->s,s,n); *(n+zv->s)=0;
 f=0; m=n; p=0;
 if('_'==*t){--t; while(s<t&&'_'!=*t)--t; f=NMLOC;  p=n-2-(t-s); m=n-(2+p);}
 else DO(n, if('_'==s[i]&&'_'==s[1+i]){   f=NMILOC; p=n-2-i;     m=n-(2+p); break;});
 ASSERT(m<=255&&p<=255,EVLIMIT);
 zv->flag=f;
 zv->sn=0; zv->e=0;
 zv->m=(UC)m; zv->hash=NMHASH(m,s); 
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

static F1(jtstdnm){C*s;I j,n,p,q;
 RZ(w=vs(w));
 n=AN(w); s=CAV(w);
 RZ(n);
 j=0;   DO(n, if(' '!=s[j++])break;); p=j-1;
 j=n-1; DO(n, if(' '!=s[j--])break;); q=(n-2)-j;
 RZ(vnm(n-(p+q),p+s));
 R nfs(n-(p+q),p+s);
}    /* 0 result means error or invalid name */

F1(jtonm){A x,y; RZ(x=ope(w)); y=stdnm(x); ASSERTN(y,EVILNAME,nfs(AN(x),CAV(x))); R y;}


F1(jtnc){A*wv,x,y,z;I i,n,t,wd,*zv;L*v; 
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GA(z,INT,n,AR(w),AS(w)); zv=AV(z); 
 for(i=0;i<n;++i){
  x=0;
  RE(y=stdnm(WVR(i)));
  if(y&&(v=syrd(y,0L))){x=v->val; t=AT(x);}
  zv[i]=!y?-2:!x?-1:t&NOUN?0:t&VERB?3:t&ADV?1:2;
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
 if(m&SYMB      )RZ(z=over(nlsym(jt->stloc),z));
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
 GA(z,INT,n,AR(w),AS(w)); zv=AV(z);
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
     GA(y,LIT,k+2+p,1,0); yv=CAV(y); 
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
 GA(ch,BOX,20,1,0); m=0;
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
 GA(z,B01,n,AR(w),AS(w)); zv=BAV(z);
 for(i=0;i<n;++i){
  RE(y=stdnm(WVR(i)));
  zv[i]=1&&y;
  if(y&&(v=syrd(y,0L))){if(jt->db)RZ(redef(mark,v)); nvrredef(v->val); RZ(symfree(v));}
 }
 R z;
}    /* 4!:55 expunge */
