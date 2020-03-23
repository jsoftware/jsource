/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Spelling                                                         */

#include "j.h"
#include "w.h"

#if 0 // obsolete
static const C spell[3][72]={
 '=',     '<',     '>',     '_',     '+',     '*',     '-',     '%',
 '^',     '$',     '~',     '|',     '.',     ':',     ',',     ';',
 '#',     '@',     '/',     CBSLASH, '[',     ']',     '{',     '}',
 '`',     CQQ,     '&',     '!',     '?',     'a',     'A',     'b',
 'c',     'C',     'd',     'D',     'e',     'E',     'f',     'H',
 'i',     'I',     'j',     'L',     'm',     'M',     'n',     'o',
 'p',     'q',     'r',     's',     'S',     't',     'T',     'u',
 'v',     'x',     'y',     '0',     '1',     '2',     '3',     '4',
 '5',     '6',     '7',     '8',     '9',     'F',     'Z',     0,

 CASGN,   CFLOOR,  CCEIL,   1,       CPLUSDOT,CSTARDOT,CNOT,    CDOMINO,
 CLOG,    CSPARSE, CNUB,    CREV,    CEVEN,   COBVERSE,CCOMDOT, CCUT,
 CBASE,   CATDOT,  CSLDOT,  CBSDOT,  CLEV,    CDEX,    CTAKE,   CDROP,
 CGRDOT,  CEXEC,   CUNDER,  CFIT,    CQRYDOT, CALP,    CATOMIC, CBDOT,
 CCDOT,   CCYCLE,  1,      1,        CEPS,    CEBAR,   CFIX,    CHGEOM,
 CIOTA,   CICAP,   CJDOT,   CLDOT,   CMDOT,   CMCAP,   CNDOT,   CCIRCLE,
 CPOLY,   1,       CRDOT,   1,       1,       CTDOT,   CTCAP,   CUDOT,
 CVDOT,   CXDOT,   CYDOT,   1,       1,       1,       1,       1,
 1,       1,       1,       1,       1,       CFDOT,   1,       0,

 CGASGN,  CLE,     CGE,     CFCONS,  CPLUSCO, CSTARCO, CMATCH,  CROOT,
 CPOWOP,  CSELF,   CNE,     CCANT,   CODD,    CADVERSE,CLAMIN,  CWORDS,
 CABASE,  CATCO,   CGRADE,  CDGRADE, CCAP,    CIDA,    CTAIL,   CCTAIL,
 CGRCO,   CTHORN,  CAMPCO,  CIBEAM,  CQRYCO,  CACE,    1,       1,
 1,       1,       1,       1,       1,       1,       1,       1,       
 CICO,    1,       1,       CLCAPCO, 1,       1,       1,       1,
 CPCO,    CQCO,    1,       CSCO,    CSCAPCO, CTCO,    1,       CUCO,    
 1,       CXCO,    1,       CFCONS,  CFCONS,  CFCONS,  CFCONS,  CFCONS,  
 CFCONS,  CFCONS,  CFCONS,  CFCONS,  CFCONS,  CFCO,    CZCO,    0,
};

static const C sp3[4][10]={
 CFETCH, CEMEND, CPDERIV, CUNDCO, CFDOTDOT, CFDOTCO, CFCODOT, CFCOCO, CATDOTCO, 0,
 '{',    '}',    'p',     '&',    'F',      'F',     'F',     'F',    '@',      0,
 CESC2,  CESC2,  CESC1,   CESC1,  CESC1,    CESC1,   CESC2,   CESC2,  CESC1,    0,
 CESC2,  CESC2,  CESC1,   CESC2,  CESC1,    CESC2,   CESC1,   CESC2,  CESC2,    0,
};   /* trigraphs */

// *s is a string with length n representing a primitive.  Convert the primitive to
// a 1-byte pseudocharacter number.  Return value of 0 means error
static C spellin1(I n,C*s){C c,d,p=*s,*t;I j;
 // p is the first character, c the second, d the third
 switch(n){
  case 1:
   // For 1-byte characters, the pseudocharacter is the same as the character itself
   R p;
  case 2:
   // For 2-byte characters, look the character up in the table and choose the appropriate inflection
   c=s[1]; j=c==CESC1; j=c==CESC2?2:j;  // 0 only if erroneous user AR
   R j&&(t=(C*)strchr(spell[0],p)) ? spell[j][t-spell[0]] : 0;  // if inflection is not . or :, or character not found, return error
  case 3:
   c=s[1]; d=s[2];
   if(p==CSIGN&&d==CESC2&&BETWEENC(c,'1','9'))R CFCONS;  // lump all _0-9: as CFCONS
   // sp3 desribes a character in a column.  Row 1 is the uninflected character, rows 2-3 give a supported inflection.  If those match,
   // the pseudocharacter in in row 0
   if(t=(C*)strchr(sp3[1],p)){for(j=t-sp3[1];sp3[1][j]==p;++j){if(c==sp3[2][j]&&d==sp3[3][j])R sp3[0][j];} R 0;}
  default:  /* note: fall through if character does not support 2 inflections */
   // invalid inflection, return 0
   R 0;
}}
#endif
static C spellintab2[128-0x20][2] = {
['='-0x20]={CASGN,CGASGN},      ['<'-0x20]={CFLOOR,CLE},        ['>'-0x20]={CCEIL,CGE},         ['_'-0x20]={0,CFCONS},          ['+'-0x20]={CPLUSDOT,CPLUSCO},  ['*'-0x20]={CSTARDOT,CSTARCO},  
['-'-0x20]={CNOT,CMATCH},       ['%'-0x20]={CDOMINO,CROOT},     ['^'-0x20]={CLOG,CPOWOP},       ['$'-0x20]={CSPARSE,CSELF},     ['~'-0x20]={CNUB,CNE},          ['|'-0x20]={CREV,CCANT},        
['.'-0x20]={CEVEN,CODD},        [':'-0x20]={COBVERSE,CADVERSE}, [','-0x20]={CCOMDOT,CLAMIN},    [';'-0x20]={CCUT,CWORDS},       ['#'-0x20]={CBASE,CABASE},      ['@'-0x20]={CATDOT,CATCO},      
['/'-0x20]={CSLDOT,CGRADE},     [CBSLASH-0x20]={CBSDOT,CDGRADE},['['-0x20]={CLEV,CCAP},         [']'-0x20]={CDEX,CIDA},         ['{'-0x20]={CTAKE,CTAIL},       ['}'-0x20]={CDROP,CCTAIL},      
['`'-0x20]={CGRDOT,CGRCO},      [CQQ-0x20]={CEXEC,CTHORN},      ['&'-0x20]={CUNDER,CAMPCO},     ['!'-0x20]={CFIT,CIBEAM},       ['?'-0x20]={CQRYDOT,CQRYCO},    ['a'-0x20]={CALP,CACE},         
['A'-0x20]={CATOMIC,0},         ['b'-0x20]={CBDOT,0},           ['c'-0x20]={CCDOT,0},           ['C'-0x20]={CCYCLE,0},          
['e'-0x20]={CEPS,0},            ['E'-0x20]={CEBAR,0},           ['f'-0x20]={CFIX,0},            ['H'-0x20]={CHGEOM,0},          ['i'-0x20]={CIOTA,CICO},        ['I'-0x20]={CICAP,0},           
['j'-0x20]={CJDOT,0},           ['L'-0x20]={CLDOT,CLCAPCO},     ['m'-0x20]={CMDOT,0},           ['M'-0x20]={CMCAP,0},           ['n'-0x20]={CNDOT,0},           ['o'-0x20]={CCIRCLE,0},         
['p'-0x20]={CPOLY,CPCO},        ['q'-0x20]={0,CQCO},            ['r'-0x20]={CRDOT,0},           ['s'-0x20]={0,CSCO},            ['S'-0x20]={0,CSCAPCO},         ['t'-0x20]={CTDOT,CTCO},        
['T'-0x20]={CTCAP,0},           ['u'-0x20]={CUDOT,CUCO},        ['v'-0x20]={CVDOT,0},           ['x'-0x20]={CXDOT,CXCO},        ['y'-0x20]={CYDOT,0},           ['0'-0x20]={0,CFCONS},          
['1'-0x20]={0,CFCONS},          ['2'-0x20]={0,CFCONS},          ['3'-0x20]={0,CFCONS},          ['4'-0x20]={0,CFCONS},          ['5'-0x20]={0,CFCONS},          ['6'-0x20]={0,CFCONS},          
['7'-0x20]={0,CFCONS},          ['8'-0x20]={0,CFCONS},          ['9'-0x20]={0,CFCONS},          ['F'-0x20]={CFDOT,CFCO},        ['Z'-0x20]={0,CZCO},
};
static C spellintab3[][4] = {
/* err */ {0, 0, 0, 0},
/* { */ {0, 0, 0, CFETCH},
/* } */ {0, 0, 0, CEMEND},
/* p */ {CPDERIV, 0, 0, 0},
/* & */ {0, CUNDCO, 0, 0},
/* F */ {CFDOTDOT, CFDOTCO, CFCODOT, CFCOCO},
/* @ */ {0, CATDOTCO, 0, 0},
};
// The spelling is encoded (littleendian) as (graphic) followed by 4 bits of inflection1 followed by 4 bits of inflection2: 0000=NUL 0111=. 1110=:
#define DOT0 0x700
#define CO0 0xe00
#define DOT1 0x7000
#define CO1 0xe000
static US spellouttab[128] = {
[CASGN   -128]=(UC)'='+DOT0,       [CGASGN  -128]=(UC)'='+CO0,       [CFLOOR  -128]=(UC)'<'+DOT0,       [CMIN    -128]=(UC)'<'+DOT0,       
[CLE     -128]=(UC)'<'+CO0,       [CCEIL   -128]=(UC)'>'+DOT0,       [CMAX    -128]=(UC)'>'+DOT0,       [CGE     -128]=(UC)'>'+CO0,       
[CUSDOT  -128]=(UC)'_'+DOT0,       [CPLUSDOT-128]=(UC)'+'+DOT0,       [CPLUSCO -128]=(UC)'+'+CO0,       [CSTARDOT-128]=(UC)'*'+DOT0,       
[CSTARCO -128]=(UC)'*'+CO0,       [CNOT    -128]=(UC)'-'+DOT0,       [CLESS   -128]=(UC)'-'+DOT0,       [CHALVE  -128]=(UC)'-'+CO0,       
[CMATCH  -128]=(UC)'-'+CO0,       [CDOMINO -128]=(UC)'%'+DOT0,       [CSQRT   -128]=(UC)'%'+CO0,       [CROOT   -128]=(UC)'%'+CO0,       
[CLOG    -128]=(UC)'^'+DOT0,       [CPOWOP  -128]=(UC)'^'+CO0,       [CSPARSE -128]=(UC)'$'+DOT0,       [CSELF   -128]=(UC)'$'+CO0,       
[CNUB    -128]=(UC)'~'+DOT0,       [CNE     -128]=(UC)'~'+CO0,       [CREV    -128]=(UC)'|'+DOT0,       [CROT    -128]=(UC)'|'+DOT0,       
[CCANT   -128]=(UC)'|'+CO0,       [CEVEN   -128]=(UC)'.'+DOT0,       [CODD    -128]=(UC)'.'+CO0,       [COBVERSE-128]=(UC)':'+DOT0,       
[CADVERSE-128]=(UC)':'+CO0,       [CCOMDOT -128]=(UC)','+DOT0,       [CLAMIN  -128]=(UC)','+CO0,       [CCUT    -128]=(UC)';'+DOT0,       
[CWORDS  -128]=(UC)';'+CO0,       [CBASE   -128]=(UC)'#'+DOT0,       [CABASE  -128]=(UC)'#'+CO0,       [CFIT    -128]=(UC)'!'+DOT0,       
[CIBEAM  -128]=(UC)'!'+CO0,       [CSLDOT  -128]=(UC)'/'+DOT0,       [CGRADE  -128]=(UC)'/'+CO0,       [CBSDOT  -128]=(UC)'\\'+DOT0,       
[CDGRADE -128]=(UC)'\\'+CO0,      [CLEV    -128]=(UC)'['+DOT0,       [CCAP    -128]=(UC)'['+CO0,       [CDEX    -128]=(UC)']'+DOT0,       
[CIDA    -128]=(UC)']'+CO0,       [CHEAD   -128]=(UC)'{'+DOT0,       [CTAKE   -128]=(UC)'{'+DOT0,       [CTAIL   -128]=(UC)'{'+CO0,       
[CBEHEAD -128]=(UC)'}'+DOT0,       [CDROP   -128]=(UC)'}'+DOT0,       [CCTAIL  -128]=(UC)'}'+CO0,       [CEXEC   -128]=(UC)'"'+DOT0,       
[CTHORN  -128]=(UC)'"'+CO0,       [CGRDOT  -128]=(UC)'`'+DOT0,       [CGRCO   -128]=(UC)'`'+CO0,       [CATDOT  -128]=(UC)'@'+DOT0,       
[CATCO   -128]=(UC)'@'+CO0,       [CAMPCO  -128]=(UC)'&'+CO0,       [CATDOTCO-128]=(UC)'@'+DOT0,       [CUNDER  -128]=(UC)'&'+DOT0,       
[CQRYDOT -128]=(UC)'?'+DOT0,       [CQRYCO  -128]=(UC)'?'+CO0,       [CALP    -128]=(UC)'a'+DOT0,       [CATOMIC -128]=(UC)'A'+DOT0,       
[CACE    -128]=(UC)'a'+CO0,       [CBDOT   -128]=(UC)'b'+DOT0,       [CCDOT   -128]=(UC)'c'+DOT0,       [CCYCLE  -128]=(UC)'C'+DOT0,       
[CEPS    -128]=(UC)'e'+DOT0,       [CEBAR   -128]=(UC)'E'+DOT0,       [CFIX    -128]=(UC)'f'+DOT0,       [CFCAPCO -128]=(UC)'F'+CO0,       
[CHGEOM  -128]=(UC)'H'+DOT0,       [CIOTA   -128]=(UC)'i'+DOT0,       [CICO    -128]=(UC)'i'+CO0,       [CICAP   -128]=(UC)'I'+DOT0,       
[CICAPCO -128]=(UC)'I'+CO0,       [CJDOT   -128]=(UC)'j'+DOT0,       [CLDOT   -128]=(UC)'L'+DOT0,       [CLCAPCO -128]=(UC)'L'+CO0,       
[CMDOT   -128]=(UC)'m'+DOT0,       [CMCAP   -128]=(UC)'M'+DOT0,       [CNDOT   -128]=(UC)'n'+DOT0,       [CCIRCLE -128]=(UC)'o'+DOT0,       
[CPOLY   -128]=(UC)'p'+DOT0,       [CPCO    -128]=(UC)'p'+CO0,       [CQCAPCO -128]=(UC)'Q'+CO0,       [CQCO    -128]=(UC)'q'+CO0,       
[CRDOT   -128]=(UC)'r'+DOT0,       [CSCO    -128]=(UC)'s'+CO0,       [CSCAPCO -128]=(UC)'S'+CO0,       [CTDOT   -128]=(UC)'t'+DOT0,       
[CTCO    -128]=(UC)'t'+CO0,       [CTCAP   -128]=(UC)'T'+DOT0,       [CUDOT   -128]=(UC)'u'+DOT0,       [CVDOT   -128]=(UC)'v'+DOT0,       
[CUCO    -128]=(UC)'u'+CO0,       [CXDOT   -128]=(UC)'x'+DOT0,       [CXCO    -128]=(UC)'x'+CO0,       [CYDOT   -128]=(UC)'y'+DOT0,       
[CFCONS  -128]=(UC)'0'+CO0,       [CAMIP   -128]=(UC)'}',             [CCASEV  -128]=(UC)'}',             [CFETCH  -128]=(UC)'{'+CO0+CO1,
[CMAP    -128]=(UC)'{'+CO0+CO1,[CEMEND  -128]=(UC)'}'+CO0+CO1,[CUNDCO  -128]=(UC)'&'+DOT0+CO1,[CPDERIV -128]=(UC)'p'+DOT0+DOT1,
[CFDOT   -128]=(UC)'F'+DOT0,       [CFDOTCO -128]=(UC)'F'+DOT0+CO1,[CFDOTDOT-128]=(UC)'F'+DOT0+DOT1,[CZCO    -128]=(UC)'Z'+CO0,       
[CFCO    -128]=(UC)'F'+CO0,       [CFCOCO  -128]=(UC)'F'+CO0+CO1,[CFCODOT -128]=(UC)'F'+CO0+DOT1,       
   };
// *s is a string with length n representing a primitive.  Convert the primitive to
// a 1-byte pseudocharacter number.  Return value of 0 means error.  This is called to audit ARs & thus must work with unverified input
C spellin(I n,C*s){
 C p=s[0];  // fetch the base character.  Must be displayable ASCII
 if(n<3){
  // 1- and 2-byte strings, with no branches.  The dominant case
  C *bp=(C*)&spellintab2[p-0x20];  // base address of 2-byte chars
  I inf=1-n;  // will hold inflections: init to 0 for 1-bytes, -1 for 2-byte: this will be the value for invalid inflections
  inf=s[n-1]==CESC1?0:inf; inf=s[n-1]==CESC2?1:inf;  // .=0, :=1
  bp+=inf; bp=n==1?s:bp;  // point bp to the inflected char, or the input byte if 1-byte char
  bp=BETWEENO((C)inf|p,0x20,0x80)?bp:(C*)&spellintab2;  // if inflection error, or not ASCII graphic, point to SP hole (error)
  R *bp;  // return the character
 }
 // 3-byte characters.  Rare, so handle individually.  Most likely these will be names that are being checked to see if they are primitives
 if(p!='_'){
  // translate the base char to a table index.  Not worth the cache misses to have a full table.  This table fits in 1/2 cache line
  I ind=0; ind=p=='{'?1:ind; ind=p=='}'?2:ind; ind=p=='p'?3:ind; ind=p=='&'?4:ind; ind=p=='F'?5:ind; ind=p=='@'?6:ind;
  // there are 4 possible inflections: decode them, checking for erroneous inflection
  I inf1=-16; inf1=s[1]==CESC1?0:inf1; inf1=s[1]==CESC2?1:inf1; I inf2=-16; inf2=s[2]==CESC1?0:inf2; inf2=s[2]==CESC2?1:inf2; inf2=2*inf1+inf2;  // inf2=0-3, or neg if error
  C *bp=&spellintab3[ind][inf2]; bp=inf2<0?&spellintab3[0][0]:bp;   // point to the result; if erroneous inflection, point to error return
  R *bp;  // return the character
 }else{
  // must be _n: for n a digit
  R BETWEENC(s[1],'0','9')&&s[2]==CESC2?CFCONS:0;
 }
}
// s is a buffer long enough to hold the longest spelling.  Fill it with the spelling of c
#if 0 // obsolete 
static void spellit1(C c,C*s){C*q;I k;
 s[1]=s[2]=0;
 if((UC)c<=127) s[0]=c;
 else if(q=(C*)strchr(spell[1],c)){k=q-spell[1]; s[0]=spell[0][k]; s[1]=CESC1;}
 else if(q=(C*)strchr(spell[2],c)){k=q-spell[2]; s[0]=spell[0][k]; s[1]=CESC2;}
 else if(q=(C*)strchr(sp3[0],  c)){k=q-sp3[0];   s[0]=sp3[1][k];   s[1]=sp3[2][k]; s[2]=sp3[3][k];}
 else if(CAMIP==c)s[0]='}';
 else if(CCASEV==c)s[0]='}';
}    /* spell out ID c in s */
#endif
void spellit(UC c,UC *s){
 I spell=spellouttab[c&127]; spell=c&128?spell:c; // Fetch inf2/inf1 graphic in case inflected; if not inflected, use the char itself, with inflections 0
 s[0]=(C)spell; s[1]=(C)(0xe9700>>((spell>>8)&0xf)); s[2]=(C)(0xe9700>>(spell>>12));   // 0011101(0) 0010111(0) 00000000  for NUL . :, with overlap
}

A jtspella(J jt,A w){C c,s[3];V*v;
 RZ(w);
 v=FAV(w); c=v->id;
 if(c==CFCONS)R over(thorn1(v->fgh[2]),chrcolon); 
 spellit(c,s); 
 R str(1+!!s[1]+!!s[2],s);
}

// returns the string for primitive whose pseudochar is c - 1, 2, or 3 characters long
A jtspellout(J jt,C c){C s[3]; spellit(c,s); R str(1+!!s[1]+!!s[2],s);}
