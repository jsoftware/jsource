/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Atomic, Boxed, and 5!:0                                */

#include "j.h"
#include "w.h"

static F1(jtdrr);
EVERYFS(drrself,jtdrr,0,0,VFLAGNONE)

static F1(jtdrr){PROLOG(0055);A df,dg,hs,*x,z;B b,ex,xop;C c,id;I fl,*hv,m;V*v;
 ARGCHK1(w);
 // If the input is a name, it must be from ".@'name' which turned into ".@(name+noun)  - or in debug, but that's discarded
 if(AT(w)&NAME){RZ(w=sfn(0,w));}
 // If noun, return the value of the noun.
 if(AT(w)&NOUN)R w;  // no quotes needed
 // Non-nouns and NMDOT names carry on.  Any modifiers must be primitive
 v=FAV(w); id=v->id; fl=v->flag;
 I fndx=(id==CBDOT)&&!v->fgh[0]; A fs=v->fgh[fndx]; A gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
 if(id==CATCO&&AT(w)&VERB&&FAV(gs)->id==CTDOT)R drr(gs);  // if <@:t. discard the <@:
 hs=v->fgh[2]; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
 if(id==CIBEAM&&!(AT(w)&CONJ)){fs=scib(FAV(w)->localuse.lu1.foreignmn[0]); gs=scib(FAV(w)->localuse.lu1.foreignmn[1]);} 
 if(id==CFORK&&hs==0){hs=gs; gs=fs; fs=ds(CCAP);}  // reconstitute capped fork
 if(fl&VXOPCALL)R drr(hs);
 xop=id==CCOLONE&&VXOP&fl; ex=id==CCOLONE&&!xop;  // xop=explicit operator, for which f=u, [h=v], g=the definition;  ex=explicit non-operator, for which f=defn type, g=text, h=processed text 
 b=BETWEENC(id,CHOOK,CADVF); c=id==CFORK||(id==CADVF&&hs!=0); b&=1^c;  // c if invisible trident (FORK or ADVF); b = invisible bident (HOOK or ADVF)
 m=!!fs+(gs||ex);   // m=# components of combination: test fs and gs, but in explicit definition the definition is in h, so we take that as surrogate g
 if(!m)R spella(w);  // if no components, it must be a primitive, out it
 m+=!b&&!xop||hs&&xop;   // if operator, add component for v if conjunction; if not operator, add component UNLESS w is an invisible bident: for w itself or for h
 if(evoke(w))R drr(sfne(w));  // turn nameref into string or verb; then take rep, which is the result
 if(fs)RZ(df=fl&VGERL?every(fxeach(fs,(A)&jtfxself[0]),(A)&drrself):drr(fs));  // recursively take rep of 1st component
 if(gs)RZ(dg=fl&VGERR?every(fxeach(gs,(A)&jtfxself[0]),(A)&drrself):drr(gs));  // ... and second
 if(ex)RZ(dg=unparsem(num(0),w));  // get rep of body of explicit definition, if any
 GATV0(z,BOX,m,1); x=AAV1(z);
 RZ(x[0]=incorp(df));  // always out f first
 RZ(x[1]=incorp(b||c||xop?dg:spellout(id)));  // if invisible or operator, out dg, which is g or the explicit body; otherwise out the primitive w
 if(2<m)RZ(x[2]=incorp(c||xop?drr(hs):dg));  // if there is a 3d box, out it from h if operator or trident; otherwise from g
 EPILOG(z);
}

F1(jtdrep){A z=drr(w); R z&&AT(z)&BOX?z:ravel(box(z));}

// create AR of w
F1(jtaro){A fs,gs,hs,s,*u,*x,y,z;B ex,xop;C id;I*hv,m;V*v;
 ARGCHK1(w);
 if(FUNC&AT(w)){
  if(unlikely(AFLAG(w)&AFRO))if(AT(w)&VERB){R str(12,"cocurrent_z_");}  // readonly name, must not expand it.  We don't have access to the name used
  v=FAV(w); id=v->id;
  I fndx=(id==CBDOT)&&!v->fgh[0]; fs=v->fgh[fndx]; gs=v->fgh[fndx^1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
  if(id==CATCO&&AT(w)&VERB&&FAV(gs)->id==CTDOT)R aro(gs);  // if <@:t. discard the <@:
  hs=v->fgh[2]; if(id==CBOX)gs=0;  // ignore gs field in BOX, there to simulate BOXATOP
  if(id==CFORK&&hs==0){hs=gs; gs=fs; fs=ds(CCAP);}  // reconstitute capped fork
  if(id==CIBEAM&&!(AT(w)&CONJ)){fs=scib(FAV(w)->localuse.lu1.foreignmn[0]); gs=scib(FAV(w)->localuse.lu1.foreignmn[1]);} 
  if(id==CCOLONE&&VXOPCALL&v->flag)R aro(hs);  // if nameref, take aro of value
  xop=id==CCOLONE&&VXOP&v->flag;  // xop = this is  an operator
  ex=hs&&id==CCOLONE&&!xop;   // ex = this is a non-operator explicit defn
  m=BETWEENC(id,CFORK,CADVF)&&hs?3:!!fs+(ex||xop&&hs||!xop&&gs);  // number of components: if invisible, 2 or 3; otherwise count f g h
  if(!m)R spella(w);
  if(evoke(w)){RZ(w=sfne(w)); if(FUNC&AT(w))w=aro(w); R w;}  // keep nameref as a string, UNLESS it is NMDOT, in which case use the (f.'d) verb value
 }
 GAT0(z,BOX,2,1); x=AAV1(z);
 if(NOUN&AT(w)){RZ(x[0]=incorp(ravel(scc(CNOUN)))); if(AT(w)&NAME)RZ(w=sfn(0,w)); RZ(x[1]=INCORPNA(w)); RETF(z);}  // if name, must be ".@'name', format name as string
 GATV0(y,BOX,m,1); u=AAV1(y);
 if(0<m)RZ(u[0]=incorp(aro(fs)));
 if(1<m)RZ(u[1]=incorp(aro(ex?unparsem(num(0),w):xop?hs:gs)));
 if(2<m)RZ(u[2]=incorp(aro(hs)));
 s=xop?aro(gs):spellout(id);
 RZ(x[0]=incorp(s)); RZ(x[1]=INCORPNA(y));
 R z;
}

F1(jtarep){R box(aro(w));}

// Create A for a string - name~, a primitive, or the boxed string
static DF1(jtfxchar){A y;C c,d,id,*s;I m,n;
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);  // string must be an atom or list
 ASSERT(n!=0,EVLENGTH);
 s=CAV(w); c=s[n-1];
 DO(n, d=s[i]; ASSERT((C)(d-32)<(C)(127-32),EVSPELL););  // must be all ASCII
 if(((ctype[(UC)s[0]]&~CA)==0)&&c!=CESC1&&c!=CESC2)R swap(w);  // If name and not control word, treat as name~, create nameref
 ASSERT(id=spellin(n,s),EVSPELL);  // not name, must be control word or primitive.  Also classify string 
 if(id!=CFCONS)y=ds(id); else{m=s[n-2]-'0'; y=FCONS(CSIGN!=s[0]?scib(m):2==n?ainf:scib(-m));} // define 0:, if it's that, using boolean for 0/1
 ASSERT(y&&RHS&AT(y),EVDOMAIN);   // make sure it's a noun/verb/adv/conj
 if(!self || AT(y)&NOUN+VERB)R y;  // return any NV, or AC as well if it's not the top level
 R box(w);  // If top level, we have to make sure (<,'&')`  doesn't replace the left part with bare &
}

// Convert an AR to an A block.  w is an AR that has been opened.  If it originally came from a verb it can't be a pyx, but it may contain pyxes
// self is normally 0; if nonzero, we return a noun type ('0';<value) as is rather than returning value, and leave adv/conj ARs looking like nouns
DF1(jtfx){A f,fs,g,h,p,q,*wv,y,*yv;C id;I m,n=0;
 ARGCHK1(w);
 // if string, handle that special case (name/primitive)
 if(LIT&AT(w))R fxchar(w,self);
 // otherwise, it had better be boxed with rank 0 or 1, and 1 or 2 atoms
 m=AN(w);   // m=#atoms
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(BETWEENC(m,1,2),EVLENGTH);
 wv=AAV(w); y=C(wv[0]);  // set wv->box pointers, y->first box
 // If the first box contains boxes, they are ARs - go expand them and save as fs
 // id will contains the type of the AR: 0=another AR, '0'=noun, other=id of primitive or hook/fork
 ASSERT(AN(y)!=0,EVLENGTH)  // empty not allowed
 if(BOX&AT(y)){RZ(fs=fx(y)); id=0;}
 else{RZ(y=vs(y)); ASSERT(id=spellin(AN(y),CAV(y)),EVSPELL);}
 if(1<m){
  // here it's not a primitive verb
  y=C(wv[1]); n=AN(y); yv=AAV(y); 
  if(id==CNOUN)R self?box(w):y;
  ASSERT(1>=AR(y),EVRANK);
  ASSERT(BOX&AT(y),EVDOMAIN);
 }
 switch(id){
 case CHOOK:
  ASSERT(2==n,EVLENGTH);  // len must be 2 for hook, 2 or 3 for ADVF.  fall through...
 case CADVF:  // yv must have been set
  // invisible bident/trident except for N/V V V fork
  ASSERT(2==(n&-2),EVLENGTH);  // len must be 2 for hook, 2 or 3 for ADVF
  A h3=(n==3)?fx(C(yv[2])):mark;  // if 3d parm not given, use mark
  R hook(fx(C(yv[0])),fx(C(yv[1])),h3);
 case CFORK:
  ASSERT(3==n,EVLENGTH);   // yv must have been set
  RZ(f=fx(C(yv[0]))); ASSERT(AT(f)&VERB+NOUN,EVSYNTAX);
  RZ(g=fx(C(yv[1]))); ASSERT(AT(g)&VERB,     EVSYNTAX);
  RZ(h=fx(C(yv[2]))); ASSERT(AT(h)&VERB,     EVSYNTAX);
  R folk(f,g,h);
 default:
  if(id)fs=ds(id);
  ASSERT(fs&&RHS&AT(fs),EVDOMAIN);
  if(!n)R fs;
  ASSERT(1==n&&ADV&AT(fs)||2==n&&CONJ&AT(fs),EVLENGTH);  // after this test, yv is known set
  if(0<n){RZ(p=fx(C(yv[0]))); ASSERT(AT(p)&NOUN+VERB,EVDOMAIN);}
  if(1<n){RZ(q=fx(C(yv[1]))); ASSERT(AT(q)&NOUN+VERB,EVDOMAIN);}
  R 1==n ? df1(g,p,fs) : df2(g,p,q,fs);
 }
}

// Convert any DD (i. e. 9 : string or quoted string containing LF) found in a line to DD form for display
// w is a string block, and so is the result.
// If w is abandoned (which it is for recursive calls), the result is formed inplace over w
// if JTINPLACEA is set, make sure the result fits on one line for error display: stop copying if we hit LF and emit '...',
// and don't put spaces before/after the delimiters
// result is always incorpable
A jtunDD(J jt, A w){F1PREFIP;
 I shortres=(I)jtinplace&JTINPLACEA;  // set for short, one-line result
 C *wv=CAV(w);  // start of word string
 // quick scan for 9, :, '; if not, return the input
 I scan; I qc9=0; for(scan=0;scan<AN(w);++scan){I t=0; t=wv[scan]=='\''?4:t; t=wv[scan]==':'?2:t; t=wv[scan]=='9'?1:t; qc9|=t;}  // check for ' : 9 chars
 if(qc9==7){  // if there is possibly a DD...
  // make input writable if it is not recursive; find words
  if(!((I)jtinplace&JTINPLACEW))RZ(w=ca(w)); wv=CAV(w); A wil; RZ(wil=wordil(w)); ASSERT(AM(wil)>=0,EVOPENQ)
  // loop until no more DDs found
  I (*wilv)[2]=voidAV(wil); // pointer to wordlist: (start,end+1) pairs
  I inx=0;  // next input character that has not been copied to the result
  I outx=0;  // next output position, built over the input
  I wilx=0;  // index in wil of next candidate DD
  while(1){
   // find next 9 : if any.  We do some fancy skipping based on word length
   while(wilx<=AS(wil)[0]-5){
    if(wilv[wilx+2][1]-wilv[wilx+2][0]>1)wilx+=3;  // : too big, skip it
    else if(wv[wilv[wilx+2][0]]!=':')wilx+=1;  // not :, skip it
    else if(wilv[wilx+1][1]-wilv[wilx+1][0]>1)wilx+=2;  // 9 too big, skip it
    else if(wv[wilv[wilx+1][0]]!='9')wilx+=1;  // not :, skip it
    // the rest of the test is a formality
    else if(wilv[wilx][1]-wilv[wilx][0]>1)wilx+=1;  // ( too big, skip it
    else if(wv[wilv[wilx][0]]!='(')wilx+=1;  // not (, skip it
    else if(wilv[wilx+3][1]-wilv[wilx+3][0]<2)wilx+=1;  // '' too small, skip it
    else if(wv[wilv[wilx+3][0]]!='\'')wilx+=1;  // not ', skip it
    else if(wilv[wilx+4][1]-wilv[wilx+4][0]>1)wilx+=1;  // ) too big, skip it
    else if(wv[wilv[wilx+4][0]]!=')')wilx+=1;  // not ', skip it
    else break;  // if we matched, go handle it
   }
   wilx=wilx>AS(wil)[0]-5?AS(wil)[0]:wilx;  // if no more DDs possible, pick entire rest of input
   // pack everything before the ( 9 : string ) down into the result.  We include space after
   // the last token in case it is needed for inflections
   // the ending index is the start pos of the new first word, but total length if we copy everything
   I endx=wilx==AS(wil)[0]?AN(w):wilv[wilx][0];
   // if we are about to move a LF character when we are limited to a single line, stop and output ...
   C currc=0;
   if(inx!=outx||shortres){
    DQ(endx-inx, currc=wv[inx++]; if(shortres&&currc==CLF)break;wv[outx++]=currc;)  // copy in to end
   }else{
    // we are at the beginning.  No need to move
    inx=outx=endx;
   }
   if(wilx==AS(wil)[0])break;  // break if no more DDs
   if(shortres&&currc==CLF){wv[outx++]='.'; wv[outx++]='.'; wv[outx++]='.'; break;}  // stop if we exceeded single line
   // install leading DD delimiter
   wv[outx++]='{'; wv[outx++]='{'; if(!shortres)wv[outx++]=' ';
   // dequote the string and move it down into the result
   I startddx=outx;  // remember where the DD starts, because its length may be reduced
   inx=wilv[wilx+3][0]+1; endx=wilv[wilx+3][1]-1; NOUNROLL while(inx<endx){if(wv[inx]=='\'')++inx; wv[outx++]=wv[inx++];}
   inx=wilv[wilx+4][1];  // next input character will pick up after the final )
   // recur on the string to handle any 9 : it holds; update length when finished
   fauxblock(fauxw); A z; fauxvirtual(z,fauxw,w,1,ACUC1); AK(z)+=startddx; AN(z)=AS(z)[0]=outx-startddx;
   RZ(jtunDD((J)((I)jt|JTINPLACEW|shortres),z));
   // the recursion leaves the DD in place, but it may have become shorter if it too contained DDs (the {{ }}
   // overhead is always less than the ( 9 : '' ) overhead)
   // We are going to install a trailing blank.  To prevent accumulation of blanks, discard any trailing blanks in the definition
   for(outx=startddx+AN(z);outx>startddx&&wv[outx-1]==' ';--outx);
   // install trailing DD delimiter
   if(!shortres)wv[outx++]=' '; wv[outx++]='}'; wv[outx++]='}';
   // skip wordlist pointer to the next candidate
   wilx+=5;
  }
  // Install the length of the final result
  AN(w)=AS(w)[0]=outx;  // number of chars we transferred
 }else{
  // The input did not contain 9 : .  Keep it all,  UNLESS we need a short result.  In that case scan for LF,
  // and replace the LF with ... .
  if(shortres){
   DO(AN(w), if(wv[i]==CLF){wv[i++]='.'; if(i<AN(w)){wv[i++]='.'; if(i<AN(w))wv[i++]='.';} AN(w)=AS(w)[0]=i; break;})
  }
 }
 // Any 9 : string has been handled.  Any residual quoted strings containing LF must be noun DDs, and must be represented as such
 // so that the result will be executable
 // We just look for quoted strings containing LF, then replace with {{)n,unquoted string}}
 if(!shortres){  // if we can't add LF, don't expand strings
  I stringstartx;  // starting index of current ' string
  scan=0;  // next position to examine
  while(1){  // till all nounDDs emitted
   // look for next string.  scan is the position to start looking at, just after the previous string if there was one
   // if we encounter NB., abort the scan
   for(stringstartx=scan;stringstartx<AN(w);++stringstartx)if(wv[stringstartx]=='\''||((wv[stringstartx]=='N'&&stringstartx<=AN(w)-3&&wv[stringstartx+1]=='B'&&wv[stringstartx+2]=='.')))break;
   if(stringstartx==AN(w))break;  // if none, we're through
   if(wv[stringstartx]!='\''){stringstartx=AN(w); break;}  // if we stopped at NB., we're through
   C hasLF=0;
   I numqu=0;
   for(scan=stringstartx+1;scan<AN(w);++scan){
    if(wv[scan]==CLF)hasLF=1;  // see if nounDD needed
    if(wv[scan]=='\''){if(scan+1<AN(w)&&wv[scan+1]=='\'')++scan, ++numqu;else break;}  // exit loop at ondoubled quote
   }
   // here scan is pointing to the final quote
   if(hasLF){
    I finalLF=wv[scan-1]==CLF;  // 1 if string ends with LF and thus must start with one
    // We must insert a nounDD.  We will allocate the string and copy header, unquoted middle, and trailer.
    // If the string doesn't end with LF, we use a one-line form; otherwise multiline
    // We could try to reduce number of copies, but this just isn't very common.  Unfortunately the nounDD form is bigger than the quoted form
    // length of the revised string is 6 ({{)n}}) plus len+finalLF-2-numqu
    A neww; GATV0(neww,LIT,AN(w)+6-2-numqu+finalLF,1); C *newwv=CAV1(neww);
    MC(newwv,wv,stringstartx); newwv+=stringstartx;  // pre-string, moving newwv to start of dequoted section
    MC(newwv,"{{)n\n",5); newwv+=4+finalLF;  // write the header of the nounDD, possibly starting with LF
    for(++stringstartx;stringstartx<scan;++stringstartx){  // skip the leading quote
     *newwv++=wv[stringstartx];
     if(stringstartx+1<scan&&wv[stringstartx+1]=='\'')++stringstartx;  // dedouble quote
    }  // move the quoted part
    ++scan;  // advance past the final quote
    MC(newwv,"}}",2); newwv+=2;  // trailer of nounDD
    MC(newwv,wv+scan,AN(w)-scan); // the rest of the input
    scan=newwv-CAV(neww);  // adjust input pointer to the correct position in the new string
    w=neww; wv=CAV(w); // pick up processing the modified string
   }else ++scan;  // no LF - step over the trailing quote
  }
 }
 // make result incorpable
 RETF(incorp(w));
}

// unparse a line of an explicit definition corresponding to a control word for the line
// c points to the control word
// x is boxed array of tokens for the line
// j is the line number we are accumulating
// y is unparse of previous CWs found on the line
static A jtunparse1(J jt,CW*c,A x,I j,A y){A q,z;C*s;I t;
 // for BBLOCK/TBLOCK types, convert the lines to displayable by displaying them as if for error messages, and copying the result
 switch(t=(c->tcesx>>TCESXTYPEX)&31){
 case CBBLOCK: case CBBLOCKEND: case CTBLOCK: RZ(z=unparse(x));  break;
 case CASSERT:               RZ(q=unparse(x)); GATV0(z,LIT,8+AN(q),1); s=CAV1(z); 
                             MC(s,"assert. ",8L); MC(8+s,CAV(q),AN(q)); break;
 case CLABEL:  case CGOTO:   RZ(z=ca(AAV(x)[0])); break;
 case CFOR:                  RZ(z=(c[1].tcesx-c[0].tcesx)&TCESXSXMSK?AAV(x)[0]:spellcon(t)); break;
 default:                    RZ(z=spellcon(t)); break;
 }
 // if the CW we processed comes from the same source line, append it and return the combination; otherwise return the new
 if(j==c->source){
  GATV0(q,LIT,1+AN(y)+AN(z),1); s=CAV1(q); 
  MC(s,CAV(y),AN(y)); s+=AN(y); *s++=' '; MC(s,CAV(z),AN(z)); 
  z=q;
 }
 R z;
}    /* unparse a single line */

// unparse a definition, by going through the control words
// *hv is the compile cw/sentence data
// m is #CWs to process
// result points after the last line stored into zv
static A*jtunparse1a(J jt,I m,A*hv,A*zv){A x,y;A *v;I i,j,k;
 y=hv[0]; v=CWBASE(y); I n=CWNC(y);
 y=0; j=k=-1;
 UI4 prevorigt=0;  // we must revert the change of BBEND END BB[END] to BBEND BBEND BB[END]
 for(i=0;i<m;++i){  // for each word
  UI4 tcesx=CWTCESX(v,~i); I newt=tcesx>>TCESXTYPEX; if(prevorigt==CBBLOCKEND&&newt==CBBLOCKEND){newt=CEND; tcesx^=(CBBLOCKEND^CEND)<<TCESXTYPEX;} prevorigt=newt;  // undo the BBLOCKEND swap
  CW u[2]; u[0].tcesx=tcesx; u[0].go=~CWGO(v,-n,~i);  u[0].source=CWSOURCE(v,-n,~i); u[1].tcesx=CWTCESX(v,~(i+1));   // create CW to use
  RZ(x=unparse1(u,vec(BOX,(u[1].tcesx-u[0].tcesx)&TCESXSXMSK,&v[u[0].tcesx&TCESXSXMSK]),j,y)); // append new line to y or else return it as x if it is on a new line.
  k=u->source;
  if(j<k){if(y)*zv++=jtunDD(jt,y); DQ(k-j-1, *zv++=mtv;);}  // if we are about to move to a new line, save y and zap the surplus control words on the line to empties
  y=x; j=k;
 }
 if(y)*zv++=jtunDD(jt,y);  // repeat to out last line
 DQ(k-j-1, *zv++=mtv;);
 R zv;
}

// w is a def.  Return unparsed form (list of boxed character strings)
// JT flags indicate valences to suppress.  If we suppress one, we also suppress the :
F2(jtunparsem){F2PREFIP;A h,*hv,dc,ds,mc,ms,z,*zu,*zv;I dn,m,mn,n,p;V*wv;
 ARGCHK2(a,w);
 wv=VAV(w); h=wv->fgh[2]; hv=AAV(h);  // h[2][HN] is preparsed def
 mc=hv[0];    ms=hv[2];    m=mn=CWNC(mc)-1;  // mc->control words ms->commented text  m,mn = #control words
 dc=hv[0+HN]; ds=hv[2+HN]; n=dn=CWNC(dc)-1;
 m=AN(mc)==0?-1:m; n=AN(dc)==0?-1:n;  // mc is normally a compiled defn (boxed rank 1), but it can also be an empty list of boxes if the valence is not defined.  Make mc/nc=-1 in that case.
 p=!((I)jtinplace&JTEXPVALENCEOFF)&&(n>=0)&&((m>=0)||3==i0(wv->fgh[0])||VXOPR&wv->flag);  // p=2 valences present: no suppressed valence, dyad given, and  it's a verb or an operator referring to x 
 if(equ(mtv,hv[2])&&equ(mtv,hv[2+HN])){
  // no comments: recover the original by unparsing
  // if there is a definition (even if there are no control words), set mn to the inferred #source lines
  if(m>=0)mn=1+CWSOURCE(CWBASE(mc),-CWNC(mc),~(CWNC(mc)-2));else mn=0; mn=(I)jtinplace&JTEXPVALENCEOFFM?0:mn;  // clear mn if monad suppressed
  if(n>=0)dn=1+CWSOURCE(CWBASE(dc),-CWNC(dc),~(CWNC(dc)-2));else dn=0; dn=(I)jtinplace&JTEXPVALENCEOFFD?0:dn;
  GATV0(z,BOX,p+mn+dn,1); zu=zv=AAV1(z);   // allocate the inferred #lines
  RZ(zv=unparse1a(MAX(m,0),hv,   zv)); if(p)RZ(*zv++=chrcolon);
  RZ(zv=unparse1a(MAX(n,0),hv+HN,zv));
  ASSERTSYS(AN(z)==zv-zu,"unparsem zn");
 }else{
  // commented text found.  Use it
  mn=AN(ms); dn=AN(ds); mn=(I)jtinplace&JTEXPVALENCEOFFM?0:mn; dn=(I)jtinplace&JTEXPVALENCEOFFD?0:dn;  // length of text; clear suppressed valence
  GATV0(z,BOX,p+mn+dn,1); zv=AAV1(z);
  DO(mn, *zv++=jtunDD(jt,AAV(ms)[i]);); if(p)RZ(*zv++=chrcolon);
  DO(dn, *zv++=jtunDD(jt,AAV(ds)[i]););
 }
 if(a==num(0)){RZ(z=ope(z)); if(1==AR(z))z=table(z);}
 R z;
}    /* convert h parameter for : definitions; open if a is 0 */

// Create 5!:7 form for the given control-word block x
A cw57rep(J jt, A x){I i, q[3];
 A *v=CWBASE(x); I n=AN(x)==0?0:CWNC(x)-1;  // empty x is not formatted control words
 A z; GATV0(z,BOX,3*n,2); I *s=AS(z); s[0]=n; s[1]=3;   // allocate result area
 A *zv=AAV2(z);
 UI4 prevorigt=0;  // we must revert the change of BBEND END BB[END] to BBEND BBEND BB[END]
 for(i=0;i<n;++i){  // loop over each line of the definition
  UI4 tcesx=CWTCESX(v,~i); I newt=tcesx>>TCESXTYPEX; if(prevorigt==CBBLOCKEND&&newt==CBBLOCKEND){newt=CEND; tcesx^=(CBBLOCKEND^CEND)<<TCESXTYPEX;} prevorigt=newt;  // undo the BBLOCKEND swap
  CW u[2]; u[0].tcesx=tcesx; u[0].go=~CWGO(v,-(n+1),~i);  u[0].source=CWSOURCE(v,-(n+1),~i); u[1].tcesx=CWTCESX(v,~(i+1));   // create CW to use
  RZ(*zv++=incorp(sc(i)));   // box 0: line#
  q[0]=u->tcesx>>TCESXTYPEX; q[1]=u->go; q[1]=q[1]>=CWMAX?65535:q[1]; q[2]=u->source; RZ(*zv++=incorp(vec(INT,3L,q)));  // Box 1: cwtype, go, source line.  65535 for testcases
  RZ(*zv++=incorp(unparse1(u,vec(BOX,(u[1].tcesx-u[0].tcesx)&TCESXSXMSK,&v[u[0].tcesx&TCESXSXMSK]),-1L,0L)));  // box 2: line text
 }
 R z;
}

// 5!:7
static F2(jtxrep){A h,*hv,*v,x,z,*zv;CW*u;I i,j,n,q[3],*s;V*wv; 
 ARGCHK2(a,w);
 RE(j=i0(a)); ASSERT(1==j||2==j,EVDOMAIN); j=1==j?0:HN;
 ASSERT(AT(w)&VERB+ADV+CONJ,EVDOMAIN);
 wv=FAV(w); h=wv->fgh[2];
 if(!(h&&CCOLONE==wv->id))R reshape(v2(0L,3L),ds(CACE));   // if not explicit defn or no body, return empty
 R cw57rep(jt,AAV(h)[j]);
}    /* explicit representation -- h parameter for : definitions */


DF1(jtarx){F1RANK(0,  jtarx,self); R arep(  symbrdlocknovalerr(nfb(w)));}  // 5!:1
DF1(jtdrx){F1RANK(0,  jtdrx,self); R drep(  symbrdlocknovalerr(nfb(w)));}  // 5!:2
DF1(jttrx){F1RANK(0,  jttrx,self); R trep(  symbrdlocknovalerr(nfb(w)));}  // 5!:4
DF1(jtlrx1){F1RANK(0,  jtlrx1,self); R lrep(  symbrdlocknovalerr(nfb(w)));}  // 5!:5 monad
DF2(jtlrx2){ASSERT(AR(w)==0,EVRANK) I mask=0; if(AN(a)==0)mask=JTPRFORSCREEN;else{RE(mask=i0(a)); ASSERT(BETWEENC(mask,1,3),EVDOMAIN) mask=(~mask<<JTEXPVALENCEOFFX)&JTEXPVALENCEOFF;}  R jtlrep((J)((I)jt|mask),  symbrdlocknovalerr(nfb(w)));}  // 5!:5 dyad, valence mask in jt; in x empty, print as if for screen
DF1(jtprx){F1RANK(0,  jtprx,self); R prep(  symbrdlocknovalerr(nfb(w)));}  // 5!:6

DF2(jtxrx){F2RANK(0,0,jtxrx,self); R xrep(a,symbrdlock(nfb(w)));}  // 5!:7
