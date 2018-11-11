/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Atop and Ampersand                                        */

#include "j.h"


static DF1(jtonf1){PROLOG(0021);DECLFG;A z;I flag=sv->flag,m=jt->xmode;
 PREF1(jtonf1);
 if(primitive(gs))if(flag&VFLR)jt->xmode=XMFLR; else if(flag&VCEIL)jt->xmode=XMCEIL;
 if(RAT&AT(w))RZ(w=pcvt(XNUM,w));
 RZ(z=CALL1(f1,CALL1(g1,w,gs),fs));
 jt->xmode=m;
 EPILOG(z);
}

static DF2(jtuponf2){PROLOG(0022);DECLFG;A z;I flag=sv->flag,m=jt->xmode;
 RZ(a&&w);
 if(primitive(gs))if(flag&VFLR)jt->xmode=XMFLR; else if(flag&VCEIL)jt->xmode=XMCEIL;
 if(RAT&AT(a))RZ(a=pcvt(XNUM,a));
 if(RAT&AT(w))RZ(w=pcvt(XNUM,w));
 RZ(z=INT&AT(a)&&INT&AT(w)&&CDIV==ID(gs)?intdiv(a,w):CALL1(f1,CALL2(g2,a,w,gs),fs));
 jt->xmode=m;
 EPILOG(z);
}

static X jtxmodpow(J jt,A a,A w,A h){A ox,z;
 if(!(XNUM&AT(a)))RZ(a=cvt(XNUM,a));
 if(!(XNUM&AT(w)))RZ(w=cvt(XNUM,w));
 if(!(XNUM&AT(h)))RZ(h=cvt(XNUM,h));
 ox=jt->xmod; jt->xmod=h;
 GAT(z,XNUM,1,0,0); *XAV(z)=xpow(*XAV(a),*XAV(w));
 jt->xmod=ox;
 RNE(z);
}

#define DMOD 46340         /* <. %: _1+2^31 */

#if SY_64
#define XMOD 3037000499    /* <. %: _1+2^63 */
#else
#define XMOD 94906265      /* <. %: _1+2^53 */
static I dmodpow(D x,I n,D m){D z=1; while(n){if(1&n)z=fmod(z*x,m); x=fmod(x*x,m); n>>=1;} R(I)z;}
#endif

static I imodpow(I x,I n,I m){I z=1; while(n){if(1&n)z=(z*x)%m;     x=(x*x)%m;     n>>=1;} R   z;}

static DF2(jtmodpow2){A h;B b,c;I at,m,n,wt,x,z;
 PREF2(jtmodpow2);
 h=FAV(self)->fgh[2]; 
 if(RAT&AT(a))RZ(a=pcvt(XNUM,a)) else if(!(AT(a)&INT+XNUM))RZ(a=pcvt(INT,a)); 
 if(RAT&AT(w))RZ(w=pcvt(XNUM,w)) else if(!(AT(w)&INT+XNUM))RZ(w=pcvt(INT,w));
 at=AT(a); wt=AT(w);
 if((AT(h)&XNUM||at&XNUM||wt&XNUM)&&at&XNUM+INT&&wt&INT+XNUM){A z;
  z=xmodpow(a,w,h); if(!jt->jerr)R z; RESETERR; R residue(h,expn2(a,w)); 
 }
 n=*AV(w);
 if(!(INT&at&&INT&wt&&0<=n))R residue(h,expn2(a,w));
 m=*AV(h); x=*AV(a);
 if(!m)R expn2(a,w);
 if(XMOD<m||XMOD<-m||m==IMIN||x==IMIN)R cvt(INT,xmodpow(a,w,h));
 if(b=0>m)m=-m;
 if(c=0>x)x=-x; x=x%m; if(c)x=m-x;
#if SY_64
 z=imodpow(x,n,m);
#else
 z=m>DMOD?dmodpow((D)x,n,(D)m):imodpow(x,n,m);
#endif
 R sc(b?z-m:z);
}    /* a m&|@^ w ; m guaranteed to be INT or XNUM */

static DF1(jtmodpow1){A g=FAV(self)->fgh[1]; R rank2ex(FAV(g)->fgh[0],w,self,0L,0L,0L,0L,jtmodpow2);}  // m must be an atom; I think n can have shape.  But we treat w as atomic
     /* m&|@(n&^) w ; m guaranteed to be INT or XNUM */

// u@v and u@:v
// If the CS? loops, it will be noninplaceable because the calls come from rank?ex.  If it is executed just once, we can inplace it.
// TODO: no  need for protw checking?
CS1IP(,on1, \
{PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); \
A gx; RZ(gx=(g1)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + ((-((FAV(gs)->flag>>VINPLACEOK1X)&JTINPLACEW)) & FAV(fs)->flag2 & JTWILLBEOPENED+JTCOUNTITEMS)),w,gs));  /* inplace g.  jtinplace is set for g */ \
/* inplace gx unless it is protected */ \
POPZOMB; \
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+((gx!=protw)*JTINPLACEW));  \
jtinplace=FAV(fs)->flag&VINPLACEOK1?jtinplace:jt; \
RZ(z=(f1)(jtinplace,gx,fs));} \
,0113)

CS2IP(,jtupon2, \
{PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); A gx; \
RZ(gx=(g2)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + ((-((FAV(gs)->flag>>VINPLACEOK2X)&JTINPLACEW)) & FAV(fs)->flag2 & JTWILLBEOPENED+JTCOUNTITEMS)),a,w,gs));  /* inplace g */ \
/* inplace gx unless it is protected */ \
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+((gx!=prota)&(gx!=protw)*JTINPLACEW));  \
jtinplace=FAV(fs)->flag&VINPLACEOK1?jtinplace:jt; \
RZ(z=(f1)(jtinplace,gx,fs));} \
,0114)
// special case for rank 0.  Transfer to loop.  
// if there is only one cell, process it through on1, which understands this type
static DF1(jton10){R jtrank1ex0(jt,w,self,on1cell);}  // pass inplaceability through
static DF2(jtupon20){R jtrank2ex0(jt,a,w,self,jtupon2cell);}  // pass inplaceability through

// u@n
static DF1(onconst1){DECLFG;R (f1)(jt,gs,fs);}
static DF2(onconst2){DECLFG;R (f1)(jt,gs,fs);}

// x u&v y
#if 0 // scaf
static DF2(on2){F2PREFIP;PROLOG(0023);DECLFG;A ga,gw,z; 
PREF2(on2); PUSHZOMB;
// obsolete  // here for execution on a single cell
A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA));
// obsolete  // take inplaceability of each monad from the corresponding dyad argument
// obsolete  // obsolete RZ(gw=(g1)((VAV(gs)->flag&VINPLACEOK1)?(J)(intptr_t)((I)jtinplace&~JTINPLACEA):jt,w,gs));
RZ(gw=(g1)((J)(intptr_t)((I)jtinplace&~JTINPLACEA),w,gs));
// obsolete RZ(ga=(g1)((VAV(gs)->flag&VINPLACEOK1)?(J)(intptr_t)((I)jt+(((I)jtinplace&JTINPLACEA)>>1)):jt,a,gs));
 RZ(ga=(g1)((J)(intptr_t)(((I)jtinplace>>JTINPLACEAX)+(((I)jtinplace>>JTINPLACEAX)&(~JTINPLACEW))),a,gs));  // Move bit 1 to bit 0, clear bit 1
  POPZOMB; jtinplace=(J)(intptr_t)((I)jt+(ga!=prota)*JTINPLACEA+(gw!=protw)*JTINPLACEW); jtinplace=FAV(fs)->flag&VINPLACEOK2?jtinplace:jt; RZ(z=(f2)(jtinplace,ga,gw,fs)); 
  EPILOG(z);
}
#else
// We don't bother passing WILLOPEN from u into v, since it's rarely used.  We do pass WILLOPEN into u.
CS2IP(static,on2, \
 A ga;A gw;PUSHZOMB; \
 /* here for execution on a single cell */ \
 A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); \
 /* take inplaceability of each monad from the corresponding dyad argument */ \
 RZ(gw=(g1)((J)(intptr_t)((I)jtinplace&~(JTINPLACEA+JTWILLBEOPENED+JTCOUNTITEMS)),w,gs)); \
 RZ(ga=(g1)((J)(intptr_t)((I)jt+(((I)jtinplace>>JTINPLACEAX)&JTINPLACEW)),a,gs));  /* Move bit 1 to bit 0, clear bit 1 */ \
 POPZOMB; jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEA+JTINPLACEW))+(ga!=prota)*JTINPLACEA+(gw!=protw)*JTINPLACEW); jtinplace=FAV(fs)->flag&VINPLACEOK2?jtinplace:jt; \
 RZ(z=(f2)(jtinplace,ga,gw,fs)); \
,0023)
#endif
static DF2(on20){R jtrank2ex0(jt,a,w,self,on2cell);}  // pass inplaceability through

static DF2(atcomp){AF f;
 RZ(a&&w); 
 f=atcompf(a,w,self); 
 R f?f(jt,a,w,self):upon2(a,w,self);
}

static DF2(atcomp0){A z;AF f;D oldct=jt->ct;
 RZ(a&&w);
 f=atcompf(a,w,self);
 jt->ct=0; z=f?f(jt,a,w,self):upon2(a,w,self); jt->ct=oldct; 
 RETF(z);
}

// The combining modifiers keep track of some flags used by partitions and Result Assembly.
//
// VF2BOXATOP? in the verb V means that V ends with <@(f) or <@:(f) and thus produces a single box per execution.
// Compounds can call f directly and tell RA to supply the boxing.  If they do, they must be careful not to recognize the flag
// until it is executed at a rank that will produce a single boxed result.
//
// VF2RANKONLY? is set when V is u"r.   A sequence of these can be combined into a single rank loop for any monad and for dyads as long as the maximum number of ranks -
// two for each argument - is not exceeded.
//
// VF2RANKATOP? is set when V starts with a rank loop, perhaps with F?RANK.  The rank loop for V can be subsumed into an outer rank loop as long as the rank
// limits are not exceeded.
//
// VF2WILLOPEN indicates that monad V's first action will be opening its argument.  Knowing that the next verb in a sequence WILLOPEN, RA can produce a boxed result
// more efficiently: it need not make the result recursive, it can include a virtual block as contents without realizing them, and it can avoid EPILOG
//
// VF2USESITEMCOUNT indicates that monad V's first action is RAZE; thus, WILLOPEN will also be set.  If the next verb in a sequence is USESITEMCOUNT,
// RA can perform the first step of raze processing (counting the items and checking shapes) as the items are calculated.  This will save a pass over
// tzhe items, which is valuable if the result is larger than cache.
//
// obsolete // VF2WILLBEOPENED and VF2COUNTITEMS are stored into a verb V to hold the values of VF2WILLOPEN/VF2USESITEMCOUNT in the NEXT verb to be executed.  RA in V looks at
// obsolete // VF2WILLBEOPENED/VF2COUNTITEMS to perform the actions mentioned above.  VF2WILLBEOPENED can be set any time the next verb opens.  VF2COUNTITEMS should be set
// obsolete // only when the result of RA will be fed directly into the raze: it's not fatal to set it otherwise, but it does waste time on an item-count that is not used.
// obsolete // Setting COUNTITEMS requires understanding the processing of the partitioning modifier it is being set in.  We recognize the following cases:
// obsolete // ;@:(<@v)  (since COUNTITEMS without BOXATOP is ignored, we set for any ;@:(u@v) )
// obsolete // ;@:(<@v"r)  (since COUNTITEMS without BOXATOP is ignored, we set for any ;@:(u"r) )
// obsolete // ;@:(<@:v;.0)   ditto  also &:
// obsolete // ;@(<@:v;._3 _2 _1 1 2 3)  also &:
// obsolete //
// obsolete // we copy into another verb only if it is inplaceable, as it normally is
// obsolete 
// u@v
F2(jtatop){A f,g,h=0,x;AF f1=on1,f2=jtupon2;B b=0,j;C c,d,e;I flag, flag2=0,m=-1;V*av,*wv;
 ASSERTVVn(a,w);
 av=FAV(a); c=av->id;
 if(AT(w)&NOUN){  // u@n
  if(c==CEXEC){  // ".@n
   // See if the argument is a string containing a single name.  If so, pass the name into the verb.
   // We can't lex a general sentence because lexing requires context to know how to treat assignments.  And,
   // there's no use for ".@const besides delayed name resolution
   // We give the w the strange flagging of NAME AND ALSO LIT - it will be handled as a name when executed, but as a noun for representations
   if(AR(w)<=1 && (g=tokens(vs(w),1)) && AN(g)==1 && AT(AAV(g)[0])&NAME){w=rifvs(AAV(g)[0]); AT(w)|=LIT;}
  }
  R fdef(0,CAT,VERB, onconst1,onconst2, a,w,h, VFLAGNONE, RMAX,RMAX,RMAX);
 }
 wv=FAV(w); d=wv->id;
 // Set flag with ASGSAFE status from f/g; keep INPLACE? in sync with f1,f2.  But we can turn off inplacing that is not supported by v, which may
 // save a few tests during execution and is vital for handling <@v, where we may execute v directly without going through @ and therefore mustn't inplace
 // unless v can handle it
 flag = ((av->flag&wv->flag)&VASGSAFE)+(wv->flag&(VINPLACEOK1|VINPLACEOK2));
 // special cases of u
 switch(c){
  case CBOX:    flag2 |= (VF2BOXATOP1|VF2BOXATOP2); break;  // mark this as <@f 
  case CNOT:    if(d==CMATCH){f2=jtnotmatch; flag+=VIRS2; flag&=~VINPLACEOK2;} break;
  case CGRADE:  if(d==CGRADE){f1=jtranking; flag+=VIRS1; flag&=~VINPLACEOK1;} break;
  case CSLASH:  if(d==CCOMMA){f1=jtredravel; } break;
  case CCEIL:   f1=jtonf1; f2=jtuponf2; flag+=VCEIL; flag&=~(VINPLACEOK1|VINPLACEOK2); break;
  case CFLOOR:  f1=jtonf1; f2=jtuponf2; flag+=VFLR; flag&=~(VINPLACEOK1|VINPLACEOK2);  break;
  case CICAP:   if(d==CNE){f1=jtnubind; flag&=~VINPLACEOK1;} else if(FIT0(CNE,wv)){f1=jtnubind0; flag&=~VINPLACEOK1;} break;
  case CQUERY:  if(d==CDOLLAR||d==CPOUND){f2=jtrollk; flag&=~VINPLACEOK2;} break;
  case CQRYDOT: if(d==CDOLLAR||d==CPOUND){f2=jtrollkx; flag&=~VINPLACEOK2;} break;
  case CRAZE:  // detect ;@(<@(f/\));.
   if(d==CCUT&&boxatop(w)){  // w is <@g;.k
    if((1LL<<(*AV(wv->fgh[1])+3))&0x36) { // fetch k (cut type); bits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
     A wf=wv->fgh[0]; V *wfv=FAV(wf); A g=wfv->fgh[1]; V *gv=FAV(g);  // w is <@g;.k  find g
     if((I)(((gv->id^CBSLASH)-1)|((gv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
      A gf=gv->fgh[0]; V *gfv=FAV(gf);  // find gf
      if(gfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
       f1=jtrazecut1; f2=jtrazecut2; flag&=~(VINPLACEOK1|VINPLACEOK2);
      }
     }
    }
   }
   break;
  case CSLDOT:  if(d==CSLASH&&CSLASH==ID(av->fgh[0])){f2=jtpolymult; flag&=~VINPLACEOK2;} break;
  case CQQ:     if(d==CTHORN&&CEXEC==ID(av->fgh[0])&&equ(zero,av->fgh[1])){f1=jtdigits10; flag&=~VINPLACEOK1;} break;
  case CEXP:    if(d==CCIRCLE){f1=jtexppi; flag&=~VINPLACEOK1;} break;
  case CAMP:
   x=av->fgh[0]; if(RAT&AT(x))RZ(x=pcvt(XNUM,x));
   if((d==CEXP||d==CAMP&&CEXP==ID(wv->fgh[1]))&&AT(x)&INT+XNUM&&!AR(x)&&CSTILE==ID(av->fgh[1])){
    h=x; flag+=VMOD; 
    if(d==CEXP){f2=jtmodpow2; flag&=~VINPLACEOK2;} else{f1=jtmodpow1; flag&=~VINPLACEOK1;}
  }
 }
 // special cases of v
// bug: +/@e.&m y does ,@e. not e.
// if(d==CEBAR||(b=FIT0(CEPS,wv))){
 if(d==CEBAR||d==CEPS||(b=FIT0(CEPS,wv))){
  f=av->fgh[0]; g=av->fgh[1]; e=ID(f); if(b)d=ID(wv->fgh[0]);
  if(c==CICAP)m=7;
  else if(c==CSLASH)m=e==CPLUS?4:e==CPLUSDOT?5:e==CSTARDOT?6:-1;
  else if(c==CAMP&&(g==zero||g==one)){j=*BAV(g); m=e==CIOTA?j:e==CICO?2+j:-1;}
  switch(0<=m?d:-1){
   case CEBAR: f2=b?atcomp0:atcomp; flag+=6+8*m; flag&=~VINPLACEOK2; break;
   case CEPS:  f2=b?atcomp0:atcomp; flag+=7+8*m; flag&=~VINPLACEOK2; break;
  }
 }
 if(d==COPE&&!(flag2&VF2BOXATOP1))flag2|=VF2ATOPOPEN1;  // @>, but not <@> which would be confused with &.>

 // Copy the open/raze status from v into u@v
 flag2 |= wv->flag2&(VF2WILLOPEN1|VF2WILLOPEN2W|VF2WILLOPEN2A|VF2USESITEMCOUNT1|VF2USESITEMCOUNT2W|VF2USESITEMCOUNT2A);

 // Copy WILLOPEN from u to WILLBEOPENED in v, and COUNTITEMS too if we have an allowable form.  Only if wv is not shared
 // 
// obsolete  if(ACIPISOK(w)){I flag2copy=0;
// obsolete    // look for ;@(<@:v;._3 _2 _1 1 2 3)  also &:
// obsolete   if(av->flag2&VF2USESITEMCOUNT){
// obsolete    if(d==CCUT){I wgi=IAV(wv->fgh[1])[0]; // wfv;.wgi
// obsolete     if(wgi>=-3 && wgi <= 3 && wgi!=0){V *wfv=FAV(wv->fgh[0]);
// obsolete      if(wfv->mr==RMAX){  // wfv has infinite rank, i. e <@:() or <@("_)
// obsolete       flag2copy |= (wfv->flag2&VF2BOXATOP1)<<(VF2USESITEMCOUNTX-VF2BOXATOP1X);  // if it is BOXATOP, enable copying USESITEMCOUNT
// obsolete      }
// obsolete     }
// obsolete    }
// obsolete   }
// obsolete   wv->flag2 |= (av->flag2&(flag2copy|VF2WILLOPEN))<<(VF2WILLBEOPENEDX-VF2WILLOPENX);  //  always take WILLOPEN; ITEMCOUNT only if needed
// obsolete  }
// obsolete 
 // Install the flags to indicate that this function starts out with a rank loop, and thus can be subsumed into a higher rank loop
 // If the compound has rank 0, switch to the loop for that; if rank is infinite, avoid the loop
 if(f1==on1){flag2|=VF2RANKATOP1; if(wv->mr==RMAX)f1=on1cell; else{if(wv->mr==0)f1=jton10;}}  // obsolete  flag2|=(f2==jtupon2)<<VF2RANKATOP2X;
 if(f2==jtupon2){flag2|=VF2RANKATOP2; if((wv->lr&wv->rr)==RMAX)f2=jtupon2cell; else{if((wv->lr|wv->rr)==0)f2=jtupon20;}}

 R fdef(flag2,CAT,VERB, f1,f2, a,w,h, flag, (I)wv->mr,(I)wv->lr,(I)wv->rr);
}

// u@:v
F2(jtatco){A f,g;AF f1=on1cell,f2=jtupon2cell;B b=0;C c,d,e;I flag, flag2=0,j,m=-1;V*av,*wv;
 ASSERTVV(a,w);
 av=FAV(a); c=av->id; f=av->fgh[0]; g=av->fgh[1]; e=ID(f); 
 wv=FAV(w); d=wv->id;
 // Set flag with ASGSAFE status from f/g; keep INPLACE? in sync with f1,f2.  But we can turn off inplacing that is not supported by v, which may
 // save a few tests during execution and is vital for handling <@v, where we may execute v directly without going through @ and therefore mustn't inplace
 // unless v can handle it
 flag = ((av->flag&wv->flag)&VASGSAFE)+(wv->flag&(VINPLACEOK1|VINPLACEOK2));
 switch(c){
  case CBOX:    flag2 |= (VF2BOXATOP1|VF2BOXATOP2); break;  // mark this as <@f 
  case CNOT:    if(d==CMATCH){f2=jtnotmatch; flag+=VIRS2; flag&=~VINPLACEOK2;} break;
  case CGRADE:  if(d==CGRADE){f1=jtranking; flag+=VIRS1; flag&=~VINPLACEOK1;} break;
  case CCEIL:   f1=jtonf1; f2=jtuponf2; flag=VCEIL; flag&=~(VINPLACEOK1|VINPLACEOK2); break;
  case CFLOOR:  f1=jtonf1; f2=jtuponf2; flag=VFLR; flag&=~(VINPLACEOK1|VINPLACEOK2); break;
  case CQUERY:  if(d==CDOLLAR||d==CPOUND){f2=jtrollk; flag&=~VINPLACEOK2;}  break;
  case CQRYDOT: if(d==CDOLLAR||d==CPOUND){f2=jtrollkx; flag&=~VINPLACEOK2;} break;
  case CICAP:   m=7; if(d==CNE){f1=jtnubind; flag&=~VINPLACEOK1;} else if(FIT0(CNE,wv)){f1=jtnubind0; flag&=~VINPLACEOK1;} break;
  case CAMP:    if(g==zero||g==one){j=*BAV(g); m=e==CIOTA?j:e==CICO?2+j:-1;} break;
  case CSLASH:  
   if(vaid(f)&&vaid(w)){f2=jtfslashatg; flag&=~VINPLACEOK2;}
   if(d==CCOMMA){f1=jtredravel; } else m=e==CPLUS?4:e==CPLUSDOT?5:e==CSTARDOT?6:-1;
   break;

  case CSEMICO:
   if(d==CLBRACE){f2=jtrazefrom; flag&=~VINPLACEOK2;}  // detect ;@:{
   else if(d==CCUT){
    j=*AV(wv->fgh[1]);   // cut type
    if(CBOX==ID(wv->fgh[0])&&!j){f2=jtrazecut0; flag&=~VINPLACEOK2;}  // detect ;@:(<;.0), used for substring extraction
    else if(boxatop(w)){  // w is <@g;.j   detect ;@:(<@(f/\);._2 _1 1 2
     if((1LL<<(j+3))&0x36) { // fbits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
      A wf=wv->fgh[0]; V *wfv=FAV(wf); A g=wfv->fgh[1]; V *gv=FAV(g);  // w is <@g;.k  find g
      if((I)(((gv->id^CBSLASH)-1)|((gv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
       A gf=gv->fgh[0]; V *gfv=FAV(gf);  // find gf
       if(gfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
        f1=jtrazecut1; f2=jtrazecut2; flag&=~(VINPLACEOK1|VINPLACEOK2);
       }
      }
     }
    }
   }
 }
 if(0<=m){
  b=d==CFIT&&equ(zero,wv->fgh[1]);
  switch(b?ID(wv->fgh[0]):d){
   case CEQ:   f2=b?atcomp0:atcomp; flag+=0+8*m; flag&=~VINPLACEOK2; break;
   case CNE:   f2=b?atcomp0:atcomp; flag+=1+8*m; flag&=~VINPLACEOK2; break;
   case CLT:   f2=b?atcomp0:atcomp; flag+=2+8*m; flag&=~VINPLACEOK2; break;
   case CLE:   f2=b?atcomp0:atcomp; flag+=3+8*m; flag&=~VINPLACEOK2; break;
   case CGE:   f2=b?atcomp0:atcomp; flag+=4+8*m; flag&=~VINPLACEOK2; break;
   case CGT:   f2=b?atcomp0:atcomp; flag+=5+8*m; flag&=~VINPLACEOK2; break;
   case CEBAR: f2=b?atcomp0:atcomp; flag+=6+8*m; flag&=~VINPLACEOK2; break;
// Bug in special code for f@:e. when rank of e. result > 1
//   case CEPS:  f2=b?atcomp0:atcomp; flag+=7+8*m; flag&=~VINPLACEOK2; break;
   case CEPS:  f2=b?atcomp0:atcomp; flag+=7+8*m; flag&=~VINPLACEOK2; break;
 }}
 // Copy the open/raze status from v into u@v
 flag2 |= wv->flag2&(VF2WILLOPEN1|VF2WILLOPEN2W|VF2WILLOPEN2A|VF2USESITEMCOUNT1|VF2USESITEMCOUNT2W|VF2USESITEMCOUNT2A);

// obsolete  // Copy WILLOPEN from u to WILLBEOPENED in v, and COUNTITEMS too if we have an allowable form.  Only if wv is not shared
// obsolete  // 
// obsolete  if(ACIPISOK(w)){I flag2copy=0;
// obsolete   // ;@:(<@v)  (since COUNTITEMS without BOXATOP is ignored, we set for any ;@:(u@v) )
// obsolete   // ;@:(<@v"r)  (since COUNTITEMS without BOXATOP is ignored, we set for any ;@:(u"r) )
// obsolete   // ;@:(<@:v;.0)   ditto  also &:
// obsolete   if(av->flag2&VF2USESITEMCOUNT){
// obsolete    if(d==CCUT){I wgi=IAV(wv->fgh[1])[0]; // wfv;.wgi
// obsolete     if(wgi==0){V *wfv=FAV(wv->fgh[0]);
// obsolete      if(wfv->mr==RMAX){  // wfv has infinite rank, i. e <@:() or <@("_)
// obsolete       flag2copy |= (wfv->flag2&VF2BOXATOP1)<<(VF2USESITEMCOUNTX-VF2BOXATOP1X);  // if it is BOXATOP, enable copying USESITEMCOUNT
// obsolete      }
// obsolete     }
// obsolete    }else if(d==CBSLASH||d==CBSDOT||d==CAT||d==CQQ)flag2copy|=VF2USESITEMCOUNT;  // accept ITEMCOUNT if \ \. " or @ (not @:)
// obsolete   }
// obsolete   wv->flag2 |= (av->flag2&(flag2copy|VF2WILLOPEN))<<(VF2WILLBEOPENEDX-VF2WILLOPENX);  //  always take WILLOPEN; ITEMCOUNT only if needed
// obsolete  }

 // Install the flags to indicate that this function starts out with a rank loop, and thus can be subsumed into a higher rank loop
 // This is appropriate even though we elide the loop
 flag2|=(f1==on1cell)<<VF2RANKATOP1X;  flag2|=(f2==jtupon2cell)<<VF2RANKATOP2X; 
 R fdef(flag2,CATCO,VERB, f1,f2, a,w,0L, flag, RMAX,RMAX,RMAX);
}

// u&:v
F2(jtampco){AF f1=on1cell;C c,d;I flag,flag2=0;V*wv;
 ASSERTVV(a,w);
 c=ID(a); wv=FAV(w); d=wv->id;  // c=pseudochar for u, d=pseudochar for v
 // Set flag wfith ASGSAFE status from f/g; keep INPLACE? in sync with f1,f2.  Inplace only if monad v can handle it
 flag = ((FAV(a)->flag&wv->flag)&VASGSAFE)+((wv->flag&VINPLACEOK1)*((VINPLACEOK2+VINPLACEOK1)/VINPLACEOK1));
 if(c==CBOX){flag2 |= VF2BOXATOP1;}  // mark this as <@f - monad only
 else if(c==CSLASH&&d==CCOMMA)         {f1=jtredravel; }
 else if(c==CRAZE&&d==CCUT&&boxatop(w)){  // w is <@g;.k    detect ;&:(<@(f/\));._2 _1 1 2
  if((1LL<<(*AV(wv->fgh[1])+3))&0x36) { // fetch k (cut type); bits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
   A wf=wv->fgh[0]; V *wfv=FAV(wf); A g=wfv->fgh[1]; V *gv=FAV(g);  // w is <@g;.k  find g
   if((I)(((gv->id^CBSLASH)-1)|((gv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
    A gf=gv->fgh[0]; V *gfv=FAV(gf);  // find gf
    if(gfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
     f1=jtrazecut1; flag&=~(VINPLACEOK1);
    }
   }
  }
 }
 else if(c==CGRADE&&d==CGRADE)         {f1=jtranking;  flag&=~VINPLACEOK1;flag+=VIRS1;}

 // Copy the monad open/raze status from v into u&:v
 flag2 |= wv->flag2&(VF2WILLOPEN1|VF2USESITEMCOUNT1);

 // Install the flags to indicate that this function starts out with a rank loop, and thus can be subsumed into a higher rank loop
 flag2|=(f1==on1cell)<<VF2RANKATOP1X;  flag2|=VF2RANKATOP2; 
 R fdef(flag2,CAMPCO,VERB, f1,on2cell, a,w,0L, flag, RMAX,RMAX,RMAX);
}

// m&v and u&n.  Never inplace the noun argument, since the verb may
// be repeated; preserve the inplacing of the argument given (i. e. move w to a for u&n).  Bit 1 of jtinplace is always 0 for monad.
// We marked the derived verb inplaceable only if the dyad of u/v was inplaceable
// This supports IRS so that it can pass the rank on to the called function
// We pass the WILLOPEn flags through
// obsolete static DF1(withl){F1PREFIP;DECLFG; R jt->rank?irs2(fs,w,gs,AR(fs),jt->rank[1],g2):(g2)(jtinplace,fs,w,gs);}
// obsolete static DF1(withr){F1PREFIP;DECLFG; R jt->rank?irs2(w,gs,fs,jt->rank[1],AR(gs),f2):(f2)((J)(intptr_t)(((I)jtinplace+JTINPLACEW)&~JTINPLACEW),w,gs,fs);}
static DF1(withl){F1PREFIP;DECLFG; I r=(RANKT)jt->ranks; R r!=(RANKT)~0?jtirs2(jtinplace,fs,w,gs,RMAX,(RANKT)r,g2):(RESETRANK,(g2)(jtinplace,fs,w,gs));}
static DF1(withr){F1PREFIP;DECLFG; jtinplace=(J)(intptr_t)((I)jtinplace+((I)jtinplace&JTINPLACEW)); I r=(RANKT)jt->ranks; R r!=(RANKT)~0?jtirs2(jtinplace,w,gs,fs,r,RMAX,f2):(RESETRANK,(f2)(jtinplace,w,gs,fs));}

// Here for m&i. and m&i:, computing a prehashed table from a
// v->fgh[2] is the info/hash/bytemask result from calculating the prehash
static DF1(ixfixedleft  ){V*v=FAV(self); R indexofprehashed(v->fgh[0],w,v->fgh[2]);}
// Here for compounds like (i.&0@:e.)&n or -.&n that compute a prehashed table from w
static DF1(ixfixedright ){V*v=FAV(self); R indexofprehashed(v->fgh[1],w,v->fgh[2]);}

// Here if ct was 0 when the compound was created - we must keep it 0
static DF1(ixfixedleft0 ){A z;D old=jt->ct;V*v=FAV(self); 
 jt->ct=0.0; z=indexofprehashed(v->fgh[0],w,v->fgh[2]); jt->ct=old; 
 R z;
}

static DF1(ixfixedright0){A z;D old=jt->ct;V*v=FAV(self); 
 jt->ct=0.0; z=indexofprehashed(v->fgh[1],w,v->fgh[2]); jt->ct=old; 
 R z;
}

static DF2(with2){R df1(w,powop(self,a,0));}

// u&v
F2(jtamp){A h=0;AF f1,f2;B b;C c,d=0;D old=jt->ct;I flag,flag2=0,mode=-1,p,r;V*u,*v;
 RZ(a&&w);
 switch(CONJCASE(a,w)){
 default: ASSERTSYS(0,"amp");
 case NN: ASSERT(0,EVDOMAIN);
 case NV:
  f1=withl; v=FAV(w); c=v->id;
  // set flag according to ASGSAFE of verb, and INPLACE and IRS from the dyad of the verb
  flag=((v->flag&(VINPLACEOK2|VIRS2))>>1)+(v->flag&VASGSAFE);
  // Mark the noun as non-inplaceable.  If the derived verb is used in another sentence, it must first be
  // assigned to a name, which will protect values inside it.
  ACIPNO(a);
  if(AN(a)&&AR(a)){
    // c holds the pseudochar for the v op.  If v is u!.n, replace c with the pseudochar for n
    // Also set b if the fit is !.0
   if(b=c==CFIT&&equ(zero,v->fgh[1]))c=ID(v->fgh[0]); 
   mode=c==CIOTA?IIDOT:c==CICO?IICO:-1;
  }
  if(0<=mode){
   if(b){jt->ct=0.0; h=indexofsub(mode,a,mark); jt->ct=old; f1=ixfixedleft0; flag&=~VINPLACEOK1;}
   else {            h=indexofsub(mode,a,mark);             f1=ixfixedleft ; flag&=~VINPLACEOK1;}
  }else switch(c){
   case CWORDS: RZ(a=fsmvfya(a)); f1=jtfsmfx; flag&=~VINPLACEOK1; break;
   case CIBEAM: if(v->fgh[0]&&v->fgh[1]&&128==i0(v->fgh[0])&&3==i0(v->fgh[1])){RZ(h=crccompile(a)); f1=jtcrcfixedleft; flag&=~VINPLACEOK1;}
  }
  R fdef(0,CAMP,VERB, f1,with2, a,w,h, flag, RMAX,RMAX,RMAX);
 case VN: 
  f1=withr; v=FAV(a);
  // set flag according to ASGSAFE of verb, and INPLACE and IRS from the dyad of the verb 
  // kludge mark it not ASGSAFE in case it is a name that is being reassigned.  We could use nvr stack to check for that.
  flag=((v->flag&(VINPLACEOK2|VIRS2))>>1)+(v->flag&VASGSAFE);
  // Mark the noun as non-inplaceable.  If the derived verb is used in another sentence, it must first be
  // assigned to a name, which will protects values inside it.
  ACIPNO(w);
  if(AN(w)&&AR(w)){
    // c holds the pseudochar for the v op.  If v is u!.n, replace c with the pseudochar for n
    // Also set b if the fit is !.0
   c=v->id; p=v->flag&255; if(b=c==CFIT&&equ(zero,v->fgh[1]))c=ID(v->fgh[0]);
   if(7==(p&7))mode=II0EPS+(p>>3);  /* (e.i.0:)  etc. */
   else      mode=c==CEPS?IEPS:-1;
  }
  if(0<=mode){
   if(b){jt->ct=0.0; h=indexofsub(mode,w,mark); jt->ct=old; f1=ixfixedright0; flag&=~VINPLACEOK1;}
   else {            h=indexofsub(mode,w,mark);             f1=ixfixedright ; flag&=~VINPLACEOK1;}
  }
  R fdef(0,CAMP,VERB, f1,with2, a,w,h, flag, RMAX,RMAX,RMAX);
 case VV:
  // u&v
  f1=on1; f2=on2;
  v=FAV(w); c=v->id; r=v->mr;   // c=pseudochar for v
  // Set flag with ASGSAFE status from f/g; keep INPLACE? in sync with f1,f2.  To save tests later, inplace only if monad v can handle it
  flag = ((FAV(a)->flag&v->flag)&VASGSAFE)+((v->flag&VINPLACEOK1)*((VINPLACEOK2+VINPLACEOK1)/VINPLACEOK1));
  if(c==CFORK||c==CAMP){
   if(c==CFORK)d=ID(v->fgh[2]);
   if(CIOTA==ID(v->fgh[1])&&(!d||d==CLEFT||d==CRIGHT)&&equ(alp,v->fgh[0])){  // a.&i. or (a. i. ][)
    u=FAV(a); d=u->id;
    if(d==CLT||d==CLE||d==CEQ||d==CNE||d==CGE||d==CGT){f2=jtcharfn2; flag&=~VINPLACEOK2;}
   }
  }
  switch(ID(a)){   // if we matched the a.&i. code above, a must be a. and its ID will be 0
  case CBOX:   flag |= VF2BOXATOP1; break;  // mark this as <@f for the monad
  case CGRADE: if(c==CGRADE){f1=jtranking; flag+=VIRS1; flag&=~VINPLACEOK1;} break;
  case CSLASH: if(c==CCOMMA){f1=jtredravel; } break;
  case CCEIL:  f1=jtonf1; flag+=VCEIL; flag&=~VINPLACEOK1; break;
  case CFLOOR: f1=jtonf1; flag+=VFLR; flag&=~VINPLACEOK1; break;
  case CRAZE:  // detect ;@(<@(f/\));.
   if(c==CCUT&&boxatop(w)){  // w is <@g;.k
    if((1LL<<(*AV(v->fgh[1])+3))&0x36) { // fetch k (cut type); bits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
     A wf=v->fgh[0]; V *wfv=FAV(wf); A g=wfv->fgh[1]; V *gv=FAV(g);  // w is <@g;.k  find g
    if((I)(((gv->id^CBSLASH)-1)|((gv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
      A gf=gv->fgh[0]; V *gfv=FAV(gf);  // find gf
      if(gfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
       f1=jtrazecut1; flag&=~(VINPLACEOK1);
      }
     }
    }
   }
   break;
 }
 if(c==COPE)flag2|=flag2&VF2BOXATOP1?VF2ATOPOPEN2A|VF2ATOPOPEN2W:VF2ATOPOPEN1|VF2ATOPOPEN2A|VF2ATOPOPEN2W;  // &>, but not <&> which would be confused with &.>

 // Copy the monad open/raze status from v into u&v
 flag2 |= v->flag2&(VF2WILLOPEN1|VF2USESITEMCOUNT1);

// obsolete   // Install the flags to indicate that this function starts out with a rank loop, and thus can be subsumed into a higher rank loop
// obsolete    flag2|=(f1==on1)<<VF2RANKATOP1X;  flag2|=(f2==on2)<<VF2RANKATOP2X; 
 // Install the flags to indicate that this function starts out with a rank loop, and thus can be subsumed into a higher rank loop
 // If the compound has rank 0, switch to the loop for that; if infinite rank, avoid the loop
 // Even though we don't test for infinite, allow this node to be flagged as rankloop so it can combine with others
  if(f1==on1){flag2|=VF2RANKATOP1; if(r==RMAX)f1=on1cell; else{if(r==0)f1=jton10;}}
// obsolete   flag2|=(f2==on2)<<VF2RANKATOP2X;
  if(f2==on2){flag2|=VF2RANKATOP2; if(r==RMAX)f2=on2cell; else{if(r==0)f2=on20;}}
  R fdef(flag2,CAMP,VERB, f1,f2, a,w,0L, flag, r,r,r);
 }
}
