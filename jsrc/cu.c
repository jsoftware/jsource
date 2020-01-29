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
 DQ(AN(x), RZ(*zv++=CALL1(f1,*wv++,fs)););
 R z;
}

#define EVERYI(exp)  {RZ(x=exp); INCORP(x); RZ(*zv++=x); ASSERT(!(SPARSE&AT(x)),EVNONCE);}
     /* note: x can be non-noun */

// Routine for internal application of u&.>
// Does not perform the special functions of rank1ex0, namely creating recursive result and checking for BOXATOP
// Does not inplace; if we modify it to inplace, we must make sure to turn off inplacing of contents of x/y if the arg itself is not inplaceable
// We keep this around because it is used for internal calls.  Most (not all, especially calls for jtfx) have fs==0, which we could check for in rank1ex0 and treat as &.>
// It is also the recursion mechanism for L: and S:; that would require looking at a special flag during ex0 to treat that as &.>
A jtevery(J jt,A w,A fs,AF f1){A*wv,x,z,*zv;
 RZ(w);
 if(SPARSE&AT(w))R everysp(w,fs,f1);
 GATV(z,BOX,AN(w),AR(w),AS(w));
 I natoms=AN(w); if(!natoms)R z;  // exit if no result atoms
 zv=AAV(z);
 A virtw; I boxedw;
 I virtblockw[NORMAH];  // space for a virtual block of rank 0
 if(boxedw=BOX&AT(w))virtw=*(wv=AAV(w));  // if input is boxed, point to first box
 else{
  // if input is not boxed, use a faux-virtual block to point to the atoms.  Repurpose unneeded wv to hold length
  fauxvirtual(virtw,virtblockw,w,0,ACUC1); AN(virtw)=1; wv=(A*)bpnoun(AT(w));
 }
 while(1){EVERYI(CALL1(f1,virtw,fs)); if(!--natoms)break; if(boxedw)virtw=*++wv;else AK(virtw)+=(I)wv;}  // break to avoid fetching over the end of the input
 R z;
}

A jtevery2(J jt,A a,A w,A fs,AF f2){A*av,*wv,x,z,*zv;
// todo kludge should rewrite with single flag word
 RZ(a&&w); 
 // Get the number of atoms, and the number of times to repeat the short side.
 // The repetition is the count of the surplus frame.
 I rpti;  // number of times short frame must be repeated
 I natoms;  // total # cells
 C flags;  // 20=w is boxed 40=a is boxed 1=w is repeated 2=a is repeated
 {
  I ar=AR(a); I wr=AR(w);
  I cf=ar; A la=w; cf=ar<wr?cf:wr; la=ar<wr?la:a; I lr=ar+wr-cf;  // #common frame, Ablock with long shape, long rank.
  PROD(rpti,lr-cf,AS(la)+cf);
  natoms=MAX(AN(a),AN(w)); natoms=rpti==0?rpti:natoms;  // number of atoms.  Beware of empty arg with surplus frame containing 0; if an arg is empty, so is the result
  flags=(C)(REPSGN(1-rpti)&((ar<wr)+1));  // if rpti<2, no repeat; otherwise repeat short frame
  // Verify agreement
  ASSERTAGREE(AS(a),AS(w),cf);  // frames must agree
  GATV(z,BOX,natoms,lr,AS(la)); if(!natoms)R z; zv=AAV(z);  // make sure we don't fetch outside empty arg
 }
 A virtw;
 // create virtual blocks if needed
 I virtblockw[NORMAH+1];  // space for a virtual block of rank 0
 if(BOX&AT(w)){flags|=BOX; virtw=*(wv=AAV(w));}  // if input is boxed, point to first box
 else{
  // if input is not boxed, use a faux-virtual block to point to the atoms.  In this case wv is not needed and we use it for the length of an atom
  fauxvirtual(virtw,virtblockw,w,0,ACUC1); AN(virtw)=1; wv=(A*)bpnoun(AT(w));
 }
 A virta;
 I virtblocka[NORMAH+1];  // space for a virtual block of rank 0
 if(BOX&AT(a)){flags|=BOX<<1; virta=*(av=AAV(a));}  // if input is boxed, point to first box
 else{
  // if input is not boxed, use a faux-virtual block to point to the atoms.  In this case av is not needed and we use it for the length of an atom
  fauxvirtual(virta,virtblocka,a,0,ACUC1); AN(virta)=1; av=(A*)bpnoun(AT(a));
 }
 // Loop for each cell.  Increment the pointer unless the side is being repeated and the repeat-count has not expired.
 // Break in the middle of the loop to avoid fetching out of bounds to get the next address from [aw]v
 I rpt=rpti; while(1){EVERYI(CALL2(f2,virta,virtw,fs)); if(!--natoms)break; if(!(flags&2)||(--rpt==0&&(rpt=rpti,1))){if(flags&(BOX<<1))virta=*++av;else AK(virta)+=(I)av;} if(!(flags&1)||(--rpt==0&&(rpt=rpti,1))){if(flags&BOX)virtw=*++wv;else AK(virtw)+=(I)wv;} }
 R z;
}

// apply f2 on items of a or w against the entirety of the other argument.  Pass on rank of f2 to reduce rank nesting
DF2(jteachl){RZ(a&&w&&self); I lcr=AR(a)-1<0?0:AR(a)-1; I lr=lr(self); lr=lcr<lr?lcr:lr; I rr=rr(self); rr=AR(w)<rr?AR(w):rr; R rank2ex(a,w,self,lr,rr,lcr,AR(w),FAV(self)->valencefns[1]);}
DF2(jteachr){RZ(a&&w&&self); I rcr=AR(w)-1<0?0:AR(w)-1; I rr=rr(self); rr=rcr<rr?rcr:rr; I lr=lr(self); lr=AR(a)<lr?AR(a):lr; R rank2ex(a,w,self,lr,rr,AR(a),rcr,FAV(self)->valencefns[1]);}

// u&.v
// PUSH/POP ZOMB is performed in atop/amp/ampco
// under is for when we could not precalculate the inverse
static DF1(jtunder1){F1PREFIP;DECLFG;A fullf; RZ(fullf=atop(invrecur(fix(gs,sc(FIXASTOPATINV))),amp(fs,gs))); R (FAV(fullf)->valencefns[0])(FAV(fullf)->flag&VJTFLGOK1?jtinplace:jt,w,fullf);}
static DF2(jtunder2){F2PREFIP;DECLFG;A fullf; RZ(fullf=atop(invrecur(fix(gs,sc(FIXASTOPATINV))),amp(fs,gs))); R (FAV(fullf)->valencefns[1])(FAV(fullf)->flag&VJTFLGOK2?jtinplace:jt,a,w,fullf);}
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
        df1(x,iota(v2(128L, 2L)),fs); b=x&&256==AN(x)&&NUMERIC&AT(x);
  if(b){df1(y,iota(v2(  8L,32L)),fs); b=y&&256==AN(y)&&NUMERIC&AT(y);}
  if(b){x=vi(x); y=vi(y); b=x&&y;} 
  if(b){u=AV(x); v=AV(y); DO(256, j=*u++; if(j==*v++&&BETWEENO(j,-256,256))f[i]=(UC)(0<=j?j:j+256); else{b=0; break;});}
  if(jt->jerr)RESETERR;
 }         
 if(!b)R from(df1(z,indexof(alp,w),fs),alp);
 n=AN(w);
 GATV(z,LIT,n,AR(w),AS(w)); zv=UAV(z); wv=UAV(w);
 if(!bitwisecharamp(f,n,wv,zv))DQ(n, *zv++=f[*wv++];); 
 RETF(z);
}    /* f&.(a.&i.) w */

// u&.v
F2(jtunder){A x;AF f1,f2;B b,b1;C c,uid;I r;V*u,*v;
 ASSERTVV(a,w);
 c=0; f1=0; f2=0; r=mr(w); v=FAV(w);
 // Set flag with ASGSAFE status of u/v, and inplaceable.  It will stay inplaceable unless we select an uninplaceable processing routine, of we
 // learn that v is uninplaceable.  If v is unknown, keep inplaceable, because we will later evaluate the compound & might be able to inplace then
 I flag = (FAV(a)->flag&v->flag&VASGSAFE) + (VJTFLGOK1|VJTFLGOK2);
 // If v is WILLOPEN, so will the compound be - for all valences
 I flag2=(FAV(w)->flag2&(VF2WILLOPEN1|VF2USESITEMCOUNT1))*((VF2WILLOPEN1+VF2WILLOPEN2A+VF2WILLOPEN2W)>>VF2WILLOPEN1X);
 switch(v->id){
  case COPE:  f1=jtunderh10; f2=jtunderh20; flag&=~(VJTFLGOK1|VJTFLGOK2); flag2|=VF2ATOPOPEN1|VF2ATOPOPEN2A|VF2ATOPOPEN2W|VF2BOXATOP1|VF2BOXATOP2; break;   // &.>
  case CFORK: c=ID(v->fgh[2]); /* fall thru */
  case CAMP:  
   u=FAV(a);  // point to a in a&.w.  w is f1&g1 or (f1 g1 h1)
   if(b1=CSLASH==(uid=u->id)){x=u->fgh[0]; if(AT(x)&VERB){u=FAV(x);uid=u->id;}else uid=0;}   // cases: f&.{f1&g1 or (f1 g1 h1)}  b1=0    f/&.{f1&g1 or (f1 g1 h1)}   b1=1
   b=CBDOT==uid&&(x=u->fgh[1],(((AR(x)-1)&SGNIF(AT(x),INTX))<0)&&BETWEENC(IAV(x)[0],16,32));   // b if f=m b. where m is atomic int 16<=m<=32
   if(CIOTA==ID(v->fgh[1])&&(!c|(c==CLEFT)|(c==CRIGHT))&&equ(alp,v->fgh[0])){   // w is  {a.&i.  or  (a. i. ][)}
    f1=b& b1?jtbitwiseinsertchar:jtunderai1;    // m b./ &. {a.&i.  or  (a. i. ][)}   or  f &. {a.&i.  or  (a. i. ][)}
    f2=((uid==CMAX)|(uid==CMIN))>b1?(AF)jtcharfn2:f2; f2=b>b1?(AF)jtbitwisechar:f2;   // m b. &. {a.&i.  or  (a. i. ][)}   or  >. &. {a.&i.  or  (a. i. ][)}   or f &. {a.&i.  or  (a. i. ][)}
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
