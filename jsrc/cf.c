/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Forks                                                     */

#include "j.h"

#define TDECL           V*sv=FAV(self);A fs=sv->fgh[0],gs=sv->fgh[1],hs=sv->fgh[2]



// 9!:63 return [((AC of x) ; x) ;] inplacingflags ; < (AC of y) ; y
F1(jtshowinplacing1){F1PREFIP;
R link(sc((I)jtinplace&JTFLAGMSK),box(link(sc(AC(w)),w)));
}
F2(jtshowinplacing2){F2PREFIP;
R link(link(sc(AC(a)),a),link(sc((I)jtinplace&JTFLAGMSK),box(link(sc(AC(w)),w))));
}

FORK1(fork100,0x00) FORK1(fork101,0x01) FORK1(fork110,0x10) FORK1(fork111,0x11) FORK1(fork120,0x20) FORK1(fork121,0x21) 
static AF fork1tbl[3][2]={ {fork100, fork101}, {fork110, fork111}, {fork120, fork121} };

FORK2(fork200,0x00) FORK2(fork201,0x01) FORK2(fork202,0x02) FORK2(fork204,0x04) FORK2(fork208,0x08) FORK2(fork210,0x10) FORK2(fork211,0x11) FORK2(fork212,0x12) FORK2(fork214,0x14) FORK2(fork218,0x18)
FORK2(fork220,0x20) FORK2(fork221,0x21) FORK2(fork222,0x22) FORK2(fork224,0x24) FORK2(fork228,0x28) FORK2(fork240,0x40) FORK2(fork241,0x41) FORK2(fork242,0x42) FORK2(fork244,0x44) FORK2(fork248,0x48)
FORK2(fork280,0x80) FORK2(fork281,0x81) FORK2(fork282,0x82) FORK2(fork284,0x84) FORK2(fork288,0x88) FORK2(fork230,0x30) FORK2(fork231,0x31) FORK2(fork232,0x32) FORK2(fork234,0x34) FORK2(fork238,0x38)
static AF fork2tbl[6][5]={
{fork200, fork201, fork202, fork204, fork208},
{fork210, fork211, fork212, fork214, fork218},
{fork220, fork221, fork222, fork224, fork228},
{fork240, fork241, fork242, fork244, fork248},
{fork280, fork281, fork282, fork284, fork288},
{fork230, fork231, fork232, fork234, fork238},
};

FORK2(jtfolk2,0x1000)    // this version used by reversions, where localuse may not be set

// see if f is defined as [:, as a single name
static B jtcap(J jt,A x){V*v;L *l;
 if(v=VAV(x),CTILDE==v->id&&NAME&AT(v->fgh[0])&&(l=syrd(v->fgh[0],jt->locsyms))&&(x=l->val))v=VAV(x);  // don't go through chain of names, since it might loop (on u) and it's ugly to chase the chain
 R CCAP==v->id;
}

static DF1(jtcharmapa){V*v=FAV(self); R charmap(w,FAV(v->fgh[2])->fgh[0],v->fgh[0]);}
static DF1(jtcharmapb){V*v=FAV(self); R charmap(w,FAV(v->fgh[0])->fgh[0],FAV(v->fgh[2])->fgh[0]);}

FORK2(jthook2cell,0x118)
FORK1(jthook1cell,0x110)


// Create the derived verb for a fork.  Insert in-placeable flags based on routine, and asgsafe based on fgh
A jtfolk(J jt,A f,A g,A h){F2PREFIP;A p,q,x,y;AF f1=0,f2=0;B b;C c,fi,gi,hi;I flag,flag2=0,j,m=-1,fline,hcol;V*fv,*gv,*hv,*v;
 RZ(f&&g&&h);
 // by the parsing rules, g and h must be verbs here
 gv=FAV(g); gi=gv->id;
 hv=FAV(h); hi=hv->id;
 D cct=0.0;  // cct to use, 0='use default'
 // Start flags with ASGSAFE (if g and h are safe), and with INPLACEOK to match the setting of f1,f2.  Turn off inplacing that neither f nor h can handle
 if(NOUN&AT(f)){  /* nvv, including y {~ x i. ] */
  flag=(hv->flag&(VJTFLGOK1|VJTFLGOK2))+((gv->flag&hv->flag)&VASGSAFE);  // We accumulate the flags for the derived verb.  Start with ASGSAFE if all descendants are.
  RZ(f=makenounasgsafe(jt, f))   // adjust usecount so that the value cannot be inplaced
  fline=5;  // set left argtype
  if(((AT(f)^B01)|AR(f)|BAV0(f)[0])==0&&BOTHEQ8(gi,hi,CEPS,CDOLLAR))f1=jtisempty;  // 0 e. $, accepting only boolean 0
  if(LIT&AT(f)&&1==AR(f)&&BOTHEQ8(gi,hi,CTILDE,CFORK)&&CFROM==ID(gv->fgh[0])){
   x=hv->fgh[0];
   if(LIT&AT(x)&&1==AR(x)&&CIOTA==ID(hv->fgh[1])&&CRIGHT==ID(hv->fgh[2])){f1=jtcharmapa;  flag &=~(VJTFLGOK1);}  // (N {~ N i. ])
  }
 }else{
  // not nvv
  fv=FAV(f); fi=cap(f)?CCAP:fv->id; // if f is a name defined as [:, detect that now & treat it as if capped fork
  if(fi!=CCAP){
   // vvv fork.  inplace if f or h can handle it, ASGSAFE only if all 3 verbs can
   flag=((fv->flag|hv->flag)&(VJTFLGOK1|VJTFLGOK2))+((fv->flag&gv->flag&hv->flag)&VASGSAFE);  // We accumulate the flags for the derived verb.  Start with ASGSAFE if all descendants are.
   // if g has WILLOPEN, indicate WILLBEOPENED in f/h
  }
  switch(fi){
  case CCAP:
   // capped fork.  inplace if h can handle it, ASGSAFE if gh are safe
   flag=(hv->flag&(VJTFLGOK1|VJTFLGOK2))+((gv->flag&hv->flag)&VASGSAFE);  // We accumulate the flags for the derived verb.  Start with ASGSAFE if all descendants are, and inplacing if h handles it
   // Copy the open/raze status from v into u@v
   flag2 |= hv->flag2&(VF2WILLOPEN1|VF2WILLOPEN2W|VF2WILLOPEN2A|VF2USESITEMCOUNT1|VF2USESITEMCOUNT2W|VF2USESITEMCOUNT2A);
   if(gi==CBOX)flag2|=VF2BOXATOP1|VF2BOXATOP2;   // [: < h
   f1=on1cell; f2=jtupon2cell;
   if(BOTHEQ8(gi,hi,CSLASH,CDOLLAR)&&FAV(gv->fgh[0])->id==CSTAR){f1=jtnatoms;}  // [: */ $
   if(gi==CPOUND){f1=hi==CCOMMA?jtnatoms:f1; f1=hi==CDOLLAR?jtrank:f1;}  // [: # ,   [: # $
               break; /* [: g h */
  case CSLASH: if(BOTHEQ8(gi,hi,CDIV,CPOUND)&&CPLUS==FAV(fv->fgh[0])->id){f1=jtmean; flag|=VIRS1; flag &=~(VJTFLGOK1);} break;  /* +/%# */
  case CAMP:   /* x&i.     { y"_ */
  case CFORK:  /* (x i. ]) { y"_ */
   if(hi==CQQ&&(y=hv->fgh[0],LIT&AT(y)&&1==AR(y))&&equ(ainf,hv->fgh[1])&&
       (x=fv->fgh[0],LIT&AT(x)&&1==AR(x))&&CIOTA==ID(fv->fgh[1])&&
       (fi==CAMP||CRIGHT==ID(fv->fgh[2]))){f1=jtcharmapb; flag &=~(VJTFLGOK1);} break;
  case CAT:    /* <"1@[ { ] */
   if(BOTHEQ8(gi,hi,CLBRACE,CRIGHT)){                                   
    p=fv->fgh[0]; q=fv->fgh[1]; 
    if(CQQ==FAV(p)->id&&CLEFT==ID(q)&&(CLT==ID(FAV(p)->fgh[0])&&FAV(p)->fgh[1]==num(1))){f2=jtsfrom; flag &=~(VJTFLGOK2);}
   }
   break;
 // special code for x ((>[!.0] |) * ]) y, implemented as if !.0, also if >:
 #if (C_AVX&&SY_64) || EMU_AVX
  case CHOOK:    // (< |[!.0]) * ]  or  ] * (< |[!.0])
   if(BOTHEQ8(gi,hi,CSTAR,CRIGHT) || BOTHEQ8(gi,fi,CSTAR,CRIGHT)){        
    V *hka=hi==CRIGHT?fv:hv;  // point to the time that must be the hook                           
    p=hka->fgh[0]; q=hka->fgh[1];
    if(FAV(q)->id==CSTILE){
     I d=FAV(p)->id; d=d==CFIT&&FAV(p)->localuse.lu1.cct==1.0?FAV(FAV(p)->fgh[0])->id:d;  // left side of hook, optionally with !.0
     if((d&~1)==CGE)f2=jtdeadband;  // accept > or >:
    }
   }
   break;
 #endif
  } 

  // m will be 0-7 for a comparison combination m+II0EPS
  switch(fi==CCAP?gi:hi){
  case CQUERY:  if((hi&~1)==CPOUND){f2=jtrollk; flag &=~(VJTFLGOK2);}  break;  // [: ? #  or  [: ? $
  case CQRYDOT: if((hi&~1)==CPOUND){f2=jtrollkx; flag &=~(VJTFLGOK2);} break;  // [: ?. #  or  [: ?. $ 
  case CICAP:   if(fi==CCAP){if(hi==CNE)f1=jtnubind; else if(FIT0(CNE,hv)){f1=jtnubind0; flag &=~(VJTFLGOK1);}}else if(hi==CEBAR){f2=jtifbebar; flag&=~VJTFLGOK2;} break;
  case CSLASH:  c=ID(gv->fgh[0])+1; m=-1;m=BETWEENC(c,CPLUS+1,CSTARDOT+1)?c:m;  // set m to 4-6 if [: + +. *./  h  [or  f   + +. *. mod    h/, never used]
                if(fi==CCAP&&FAV(gv->fgh[0])->flag&FAV(h)->flag&VISATOMIC2){f2=jtfslashatg;}  // [: f/ g when f and g are both atomic, treat as special
                break;
  case CFCONS:  if(hi==CFCONS){x=hv->fgh[2]; m=gi+BAV(x)[0]; m=(UI)(B01&AT(x))>((UI)(gi&~2)-CIOTA)?m:-1;} break;  // x-> constant; must be boolean, and i./i:   non-[: i. 0:/1: sets m to 0/1
  case CRAZE:
   if(hi==CCUT){   // [: ; h;.n
    j=hv->localuse.lu1.gercut.cutn;  // cut type
    if(hv->valencefns[1]==jtboxcut0){f2=jtrazecut0; flag &=~(VJTFLGOK2);}
    else if(boxatop(h)){  // h is <@g;.j   detect ;@:(<@(f/\);._2 _1 1 2
     if((((I)1)<<(j+3))&0x36) { // fbits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
      A wf=hv->fgh[0]; V *wfv=FAV(wf); A hg=wfv->fgh[1]; V *hgv=FAV(hg);  // w is <@g;.k  find g
      if((I)(((hgv->id^CBSLASH)-1)|((hgv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
       A hgf=hgv->fgh[0]; V *hgfv=FAV(hgf);  // find gf
       if(hgfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
        f1=jtrazecut1; f2=jtrazecut2; flag&=~(VJTFLGOK1|VJTFLGOK2);
       }
      }
     }
    }
   } break;
  }

#if C_CRC32C && SY_64
  if(unlikely(fi==CLEFT)){
   I d=gv->id; d=d==CFIT&&gv->localuse.lu1.cct==1.0?FAV(gv->fgh[0])->id:d;  // middle -. of [-.-. .  We implement as if !.0 given, so we allow the user to give that
   I c=hv->id; I e=c; if(unlikely(e==CFIT)){cct=hv->localuse.lu1.cct; e=FAV(hv->fgh[0])->id;}  // comparison op, possibly from u!.f
   if(BOTHEQ8(d,e,CLESS,CLESS)){  // ([ -. -.)  and ([ -. -.!.f) - the middle -. can also be !.0.  It is implemented as !.0
    f2=jtintersect;  // treat the compound as a primitive of its own
    flag|=(7+(((IINTER-II0EPS)&0xf)<<3));  // flag it like -.
    // if tolerance given on second -., it is now in cct
   }
 }
#endif

  // comparison combinations II0EPS-IIFBEPS.  If the comparison is e. these go through indexofsub, but first the ranks have to be tested in compsc
  if(0<=m){  // comparison combiner has been found.  Is there a comparison?
   // m has information about the comparison combiner.  See if there is a comparison
   V *cv=(m&=7)>=4?hv:fv;  // cv point to comp in comp i. 0:  or [: +/ comp
   I d=cv->id; I e=d; e=e==CFIT&&(cct=cv->localuse.lu1.cct,1)?FAV(cv->fgh[0])->id:e;  // comparison op, possibly from u!.0
   if(BETWEENC(e,CEQ,CEPS)){
    // valid comparison combination.  m is the combiner, e is the comparison
    f2=atcomp;  // valid comparison type: switch to it
    flag+=(e-CEQ)+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2);  // set comp type mmmeee; entry point does not allow jt flags
   }
  }

  // the stored form of a capped fork shifts gh to the place of fg and leaves h==0.  This allows the code for
  // [: g h to be the same as f@:g
  if(fi==CCAP){f=g; g=h; h=0;}  // dehydrate capped fork
  // If this fork is not a special form, set the flags to indicate whether the f verb does not use an
  // argument.  In that case h can inplace the unused argument.
  fline=(CTTZI(atoplr(f)|0x80)+1)&7;  // codes are none,[,],@[,@],noun
 }
 hcol=(CTTZI(atoplr(h)|0x80)+1)&7;

 A z=fdef(flag2,CFORK,VERB, f1,f2, f,g,h, flag, RMAX,RMAX,RMAX);
 RZ(z);
 
 // set localuse: for intersect or comparison combination, cct; for echt fork, the h routine to call
 if(!f2){
  // if using the default handler, set the entry point
  AF hfn=fork2tbl[fline][hcol]; hfn=(I)jtinplace&JTFOLKNOHFN?jtfolk2:hfn;  // NOHFN means the caller is going to fool with the result fork, so the EP is unreliable
  FAV(z)->valencefns[1]=hfn;
  FAV(z)->localuse.lu1.fork2hfn=hcol<=2?hv->valencefns[1]:FAV(hv->fgh[0])->valencefns[0];
 }else{FAV(z)->localuse.lu1.cct=cct;
 }
 if(!f1){
  fline=(0x200110>>(fline<<2))&3;  // 0/3/4->0,  1/2->1, 5->2
  hcol=(0b00110>>hcol)&1;  // / 0/3/4->0,  1/2->1
  FAV(z)->valencefns[0]=fork1tbl[fline][hcol];
 }
 R z;
}

// Handlers for to  handle w (aa), w (vc), w (cv)
static DF1(taa){TDECL; A z,t; df1(t,w,fs); ASSERT(!t||AT(t)&NOUN+VERB,EVSYNTAX); R df1(z,t,gs);}
static DF1(tvc){TDECL; A z; R df2(z,fs,w,gs);}  /* also nc */
static DF1(tcv){TDECL; A z; R df2(z,w,gs,fs);}  /* also cn */


static DF1(jthkiota){DECLFG;A a,e;I n;P*p;
 ARGCHK1(w);
 SETIC(w,n);
 if((AT(w)&SPARSE+B01)==SPARSE+B01&&1==AR(w)){
  p=PAV(w); a=SPA(p,a); e=SPA(p,e); 
  R BAV(e)[0]||equ(mtv,a) ? repeat(w,IX(n)) : repeat(SPA(p,x),ravel(SPA(p,i)));
 }
 R B01&AT(w)&&1>=AR(w) ? ifb(n,BAV(w)) : repeat(w,IX(n));
}    /* special code for (# i.@#) */

static DF1(jthkodom){DECLFG;B b=0;I n,*v;
 ARGCHK1(w);
 if(INT&AT(w)&&1==AR(w)){n=AN(w); v=AV(w); DO(n, if(b=0>v[i])break;); if(!b)R odom(2L,n,v);}
 R CALL2(f2,w,CALL1(g1,w,gs),fs);
}    /* special code for (#: i.@(* /)) */

static DF1(jthkindexofmaxmin){
 ARGCHK2(w,self);
 // The code for ordinary search and min/max is very fast.  There's no value in trying to improve it.  The only
 // thing we have to add is setting intolerant comparison on the search, since we know we will be looking for something that is en exact match
 PUSHCCT(1.0) A z=jthook1cell(jt,w,self); POPCCT RETF(z);
}    /* special code for (i.<./) (i.>./) (i:<./) (i:>./) */

// (compare L.) dyadic
static DF2(jthklvl2){
 F2RANK(0,RMAX,jthklvl2,self);
 I comparand; RE(comparand=i0(a));  // get value to compare against
 RETF(num(((VAV(self)->flag>>VFHKLVLGTX)&1)^levelle(w,comparand-(VAV(self)->flag&VFHKLVLDEC))));  // decrement for < or >:; complement for > >:
}

F2(jthook){AF f1=0,f2=0;C c,d,e,id;I flag=VFLAGNONE,linktype=0;V*u,*v;
 ARGCHK2(a,w);
 if(AT(a)&AT(w)&VERB){
  // This is the (V V) case, producing a verb
  u=FAV(a); c=u->id; f1=jthook1cell; f2=jthook2cell;
  v=FAV(w); d=v->id; e=ID(v->fgh[0]);
  // Set flag to use: ASGSAFE if both operands are safe; and FLGOK init to OK as for hook, but change as needed to match f1,f2
  flag=((u->flag&v->flag)&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2);  // start with in-place enabled, as befits hook1/hook2
  if(d==CCOMMA)switch(c){   // all of this except for ($,) is handled by virtual blocks
   case CDOLLAR: f2=jtreshape; flag+=VIRS2; break;  // ($,) is inplace
  }else if(d==CBOX){
   if(c==CRAZE){f2=jtlink; linktype=ACINPLACE;  // (;<)
   }else if(c==CCOMMA){f2=jtlink; linktype=ACINPLACE+1;  // (,<)
   }
  }else if(d==CLDOT){   // (compare L.)
   I comptype=0; comptype=c==CLT?VFHKLVLGT:comptype; comptype=c==CGT?VFHKLVLDEC:comptype; comptype=c==CLE?VFHKLVLDEC+VFHKLVLGT:comptype; comptype=c==CGE?4:comptype;
   if(comptype){flag|=comptype; f2=jthklvl2; flag &=~VJTFLGOK2;}
  }else{
   switch(c){
   case CSLDOT:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->fgh[1])){  // (f/. i.@#)  or @: & &:
                  if(CBOX==ID(u->fgh[0])){f1=jtkeybox; flag &=~VJTFLGOK1;} // (</. i.@#)
                  else if(u->valencefns[1]==jtkeyheadtally){f1=jtkeyheadtally; flag &=~VJTFLGOK1;} // ((#,{.)/. i.@#) or  (({.,#)/. i.@#)
                 } break;
   case CPOUND:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->fgh[1])){f1=jthkiota; flag &=~VJTFLGOK1;} break;  // (# i.@#))
   case CABASE:  if(COMPOSE(d)&&e==CIOTA&&CSLASH==ID(v->fgh[1])&&CSTAR==ID(FAV(v->fgh[1])->fgh[0])){f1=jthkodom; flag &=~VJTFLGOK1;} break;  // (#: i.@(*/))
   case CIOTA:   
   case CICO:    if(BOTHEQ8(d,(e&~1),CSLASH,CMIN)){f1=jthkindexofmaxmin; flag &=~VJTFLGOK1;} break;  // >./ <./
   case CFROM:   if(d==CGRADE){f2=jtordstati; flag &=~VJTFLGOK2;} else if(d==CTILDE&&e==CGRADE){f2=jtordstat; flag &=~VJTFLGOK2;}
   }
  }
  // Return the derived verb
  A z;RZ(z=fdef(0,CHOOK, VERB, f1,f2, a,w,0L, flag, RMAX,RMAX,RMAX));
  FAV(z)->localuse.lu1.linkvb=linktype; R z;  // if it's a form of ;, install the form
 // All other cases produce an adverb
 }else if(AT(a)&AT(w)&ADV){
  f1=taa;
 }else if(AT(a)&NOUN+VERB&&AT(w)&CONJ){
  f1=tvc; id=FAV(w)->id;
  if(BOX&AT(a)&&(id==CATDOT||id==CGRAVE||id==CGRCO)&&gerexact(a))flag+=VGERL;
 }else if(AT(w)&NOUN+VERB&&AT(a)&CONJ){
  f1=tcv; id=FAV(a)->id;
  if(BOX&AT(w)&&(id==CGRAVE||id==CPOWOP&&1<AN(w))&&gerexact(w))flag+=VGERR;
 }else{ASSERT(0,EVSYNTAX);}  // Note: EDGE CAVN ASGN (always an error) passes through here

 R fdef(0,CADVF, ADV, f1,0L, a,w,0L, flag, 0L,0L,0L);
}
