/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Under and Each                                            */

#include "j.h"
#include "ve.h"


static A jteverysp(J jt,A w,A fs,AF f1){A*wv,x,z,*zv;P*wp,*zp;
 RZ(w);
 ASSERT(SBOX&AT(w),EVNONCE);
 RZ(z=ca(w));
 wp=PAV(w); x=SPA(wp,x); wv=AAV(x);
 zp=PAV(z); x=SPA(zp,x); zv=AAV(x);
 DO(AN(x), RZ(*zv++=CALL1(f1,*wv++,fs)););
 R z;
}

#define EVERYI(exp)  {RZ(x=exp); INCORP(x); RZ(*zv++=x); ASSERT(!(SPARSE&AT(x)),EVNONCE);}
     /* note: x can be non-noun */

// u&.> work routine.  Does not inplace; if we modify it to inplace, we must make sure to turn off inplacing of contents of x/y if the arg itself is not inplaceable
// We keep this around because it is used for internal calls.  Most (not all, especially calls for jtfx) have fs==0, which we could check for in rank1ex0 and treat as &.>
// It is also the recursion mechanism for L: and S:; that would require looking at a special flag during ex0 to treat that as &.>
A jtevery(J jt,A w,A fs,AF f1){A*wv,x,z,*zv;
 RZ(w);
 if(SPARSE&AT(w))R everysp(w,fs,f1);
 if(!(BOX&AT(w)))RZ(w=box0(w));
 GATV(z,BOX,AN(w),AR(w),AS(w));
 zv=AAV(z); wv=AAV(w); 
 DO(AN(w), EVERYI(CALL1(f1,WVR(i),fs)););
 R z;
}

A jtevery2(J jt,A a,A w,A fs,AF f2){A*av,*wv,x,z,*zv;B ab,b,wb;I an,ar,*as,wn,wr,*ws;
// todo kludge should rewrite with single flag word
 RZ(a&&w); 
 an=AN(a); ar=AR(a); as=AS(a); ab=!!(BOX&AT(a));
 wn=AN(w); wr=AR(w); ws=AS(w); wb=!!(BOX&AT(w));
 b=!ar||!wr||ar==wr; if(b&&ar&&wr)DO(ar, b&=as[i]==ws[i];);
 if(!b)R df2(a,w,atop(ds(CBOX),amp(fs,ds(COPE))));
 GATV(z,BOX,ar?an:wn,ar?ar:wr,ar?as:ws);  zv=AAV(z);
 if(ar&&!ab)RZ(a=box0(a)); av=AAV(a); 
 if(wr&&!wb)RZ(w=box0(w)); wv=AAV(w); 
 if(ar&&wr)                   DO(an, EVERYI(CALL2(f2,AVR(i),      WVR(i),      fs))) 
 else if(wr){if(ab)a=AAV0(a); DO(wn, EVERYI(CALL2(f2,a,           WVR(i),      fs)));}
 else if(ar){if(wb)w=AAV0(w); DO(an, EVERYI(CALL2(f2,AVR(i),      w,           fs)));}
 else                                EVERYI(CALL2(f2,ab?AAV0(a):a,wb?AAV0(w):w,fs)) ;
 R z;
}

// apply f2 on operate on items of a or w.  Pass on rank of f2 to reduce rank nesting
DF2(jteachl){RZ(a&&w&&self); R rank2ex(a,w,self,FAV(self)->lr,FAV(self)->rr,-1L, RMAX,FAV(self)->valencefns[1]);}
DF2(jteachr){RZ(a&&w&&self); R rank2ex(a,w,self,FAV(self)->lr,FAV(self)->rr,RMAX,-1L, FAV(self)->valencefns[1]);}

// u&.v    kludge should calculate fullf as part of under/undco & pass in via h
// PUSH/POP ZOMB is performed in atop/amp/ampco
// under is for when we could not precalculate the inverse
static DF1(jtunder1){F1PREFIP;DECLFG;A fullf; RZ(fullf=atop(invrecur(fix(gs)),amp(fs,gs))); R (FAV(fullf)->valencefns[0])(FAV(fullf)->flag&VJTFLGOK1?jtinplace:jt,w,fullf);}
static DF2(jtunder2){F2PREFIP;DECLFG;A fullf; RZ(fullf=atop(invrecur(fix(gs)),amp(fs,gs))); R (FAV(fullf)->valencefns[1])(FAV(fullf)->flag&VJTFLGOK2?jtinplace:jt,a,w,fullf);}
// underh has the inverse precalculated, and the inplaceability set from it.  It handles &. and &.: which differ only in rank
static DF1(jtunderh1){F1PREFIP;DECLFGH; R (FAV(hs)->valencefns[0])(jtinplace,w,hs);}
static DF2(jtunderh2){F2PREFIP;DECLFGH; R (FAV(hs)->valencefns[1])(jtinplace,a,w,hs);}
// undco is for when we could not precalculate the inverse
static DF1(jtundco1){F1PREFIP;DECLFG;A fullf; RZ(fullf=atop(inv(gs),ampco(fs,gs))); R (FAV(fullf)->valencefns[0])(FAV(fullf)->flag&VJTFLGOK1?jtinplace:jt,w,fullf);}
static DF2(jtundco2){F2PREFIP;DECLFG;A fullf; RZ(fullf=atop(inv(gs),ampco(fs,gs))); R (FAV(fullf)->valencefns[1])(FAV(fullf)->flag&VJTFLGOK2?jtinplace:jt,a,w,fullf);}

// u&.> main entry point.  Does not support inplacing.
static DF2(jteach2){DECLF; R every2(a,w,fs,f2);}
// versions for rank 0 (including each).  Passes inplaceability through
// if there is only one cell, process it through under[h]1, which understands this type; if more, loop through
static DF1(jtunder10){R jtrank1ex0(jt,w,self,jtunder1);}  // pass inplaceability through
static DF1(jtunderh10){R jtrank1ex0(jt,w,self,jtunderh1);}  // pass inplaceability through
static DF2(jtunder20){R jtrank2ex0(jt,a,w,self,jtunder2);}  // pass inplaceability through
static DF2(jtunderh20){R jtrank2ex0(jt,a,w,self,jtunderh2);}  // pass inplaceability through

static DF1(jtunderai1){DECLF;A x,y,z;B b;I j,n,*u,*v;UC f[256],*wv,*zv;
 RZ(w);
 if(b=LIT&AT(w)&&256<AN(w)){
        x=df1(iota(v2(128L, 2L)),fs); b=x&&256==AN(x)&&NUMERIC&AT(x);
  if(b){y=df1(iota(v2(  8L,32L)),fs); b=y&&256==AN(y)&&NUMERIC&AT(y);}
  if(b){x=vi(x); y=vi(y); b=x&&y;} 
  if(b){u=AV(x); v=AV(y); DO(256, j=*u++; if(j==*v++&&-256<=j&&j<256)f[i]=(UC)(0<=j?j:j+256); else{b=0; break;});}
  if(jt->jerr)RESETERR;
 }         
 if(!b)R from(df1(indexof(alp,w),fs),alp);
 n=AN(w);
 GATV(z,LIT,n,AR(w),AS(w)); zv=UAV(z); wv=UAV(w);
 if(!bitwisecharamp(f,n,wv,zv))DO(n, *zv++=f[*wv++];); 
 RETF(z);
}    /* f&.(a.&i.) w */

// u&.v
F2(jtunder){A x;AF f1,f2;B b,b1;C c,uid;I m,r;V*u,*v;
 ASSERTVV(a,w);
 c=0; f1=0; f2=0; r=mr(w); v=FAV(w);
 // Set flag with ASGSAFE status of u/v, and inplaceable.  It will stay inplaceable unless we select an uninplaceable processing routine, of we
 // learn that v is uninplaceable.  If v is unknown, keep inplaceable, because we will later evaluate the compound & might be able to inplace then
 I flag = (FAV(a)->flag&v->flag&VASGSAFE) + (VJTFLGOK1|VJTFLGOK2);
 // If v is WILLOPEN, so will the compound be
 I flag2=FAV(w)->flag2&(VF2WILLOPEN1|VF2USESITEMCOUNT1);
 switch(v->id){
  case COPE:  f1=jtunderh10; f2=jtunderh20; flag&=~(VJTFLGOK1|VJTFLGOK2); flag2|=VF2ATOPOPEN1|VF2ATOPOPEN2A|VF2ATOPOPEN2W|VF2BOXATOP1|VF2BOXATOP2; break;   // &.>
  case CFORK: c=ID(v->fgh[2]); /* fall thru */
  case CAMP:  
   u=FAV(a);  // point to a in a&.w.  w is f1&g1 or (f1 g1 h1)
   if(b1=CSLASH==(uid=u->id)){x=u->fgh[0]; if(AT(x)&VERB){u=FAV(x);uid=u->id;}else uid=0;}   // cases: f&.{f1&g1 or (f1 g1 h1)}  b1=0    f/&.{f1&g1 or (f1 g1 h1)}   b1=1
   b=CBDOT==uid&&(x=u->fgh[0],!AR(x)&&INT&AT(x)&&(m=*AV(x),16<=m&&m<32));   // b if f=m b. where m is atomic int 16<=m<=32
   if(CIOTA==ID(v->fgh[1])&&(!c||c==CLEFT||c==CRIGHT)&&equ(alp,v->fgh[0])){   // w is  {a.&i.  or  (a. i. ][)}
    f1=b&& b1?jtbitwiseinsertchar:jtunderai1;    // m b./ &. {a.&i.  or  (a. i. ][)}   or  f &. {a.&i.  or  (a. i. ][)}
    f2=b&&!b1?(AF)jtbitwisechar:!b1&&(uid==CMAX||uid==CMIN)?(AF)jtcharfn2:f2;   // m b. &. {a.&i.  or  (a. i. ][)}   or  >. &. {a.&i.  or  (a. i. ][)}   or f &. {a.&i.  or  (a. i. ][)}
    flag&=~(VJTFLGOK1|VJTFLGOK2);   // not perfect, but ok
   }
 }
 // Create the standard g^:_1 @ (f & g) to use if we have no special processing (not needed if a.&i., but that's rare)
 A h=0; if(nameless(w)){h=atop(inv(w),amp(a,w)); ASSERT(h,EVDOMAIN);} // h must be valid for free.  If no names in w, take the inverse
 // under12 are inplaceable, and pass inplaceability based on the calculated verb.  underh just passes inplaceability through, so we have to transfer the setting from h here,
 // just in case the calculated verb is not inplaceable
 // The standard verbs start with a rank loop; set the flag indicating that
 if(!f1){f1=r?(h?jtunderh1:jtunder1):(h?jtunderh10:jtunder10); flag2|=VF2RANKATOP1; if(h)flag&=FAV(h)->flag|(~VJTFLGOK1);}  // allow inplace if v is known inplaceable
 if(!f2){f2=r?(h?jtunderh2:jtunder2):(h?jtunderh20:jtunder20); flag2|=VF2RANKATOP2; if(h)flag&=FAV(h)->flag|(~VJTFLGOK2);}  // allow inplace if v is known inplaceable
 R fdef(flag2,CUNDER,VERB,(AF)(f1),(AF)(f2),a,w,h,(flag),(I)(r),(I)(r),(I)(r));
}

F2(jtundco){AF f1,f2;
 ASSERTVV(a,w); 
 // Set flag with ASGSAFE status of u/v, and inplaceability of f1/f2
 A h=0; if(nameless(w)){h=atop(inv(w),ampco(a,w)); ASSERT(h,EVDOMAIN);} // h must be valid for free.  If no names in w, take the inverse
 f1=h?jtunderh1:jtundco1; f2=h?jtunderh2:jtundco2; I flag = (FAV(a)->flag&FAV(w)->flag&VASGSAFE) + (h?FAV(h)->flag&((VJTFLGOK1|VJTFLGOK2)):(VJTFLGOK1|VJTFLGOK2));
 R fdef(0,CUNDCO,VERB,(AF)(f1),(AF)(f2),a,w,h,flag,RMAX,RMAX,RMAX);
}
