/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Spelling                                                         */

#include "j.h"
#include "w.h"

// These are the codes for each char that can present as a primitive.  Alphabetics cannot unless inflected.  Numerics can in inflected with :
// OR when they are generated internally, where 0 2 3 4 are used as primitives for invisible modifiers (noun, hook, fork, adv)
static C spellintab2[128-0x20][3] = {
['='-0x20]={CEQ,CASGN,CGASGN},      ['<'-0x20]={CLT,CFLOOR,CLE},        ['>'-0x20]={CGT,CCEIL,CGE},         ['_'-0x20]={CINF,0,CFCONS},          ['+'-0x20]={CPLUS,CPLUSDOT,CPLUSCO},  ['*'-0x20]={CSTAR,CSTARDOT,CSTARCO},  
['-'-0x20]={CMINUS,CNOT,CMATCH},       ['%'-0x20]={CDIV,CDOMINO,CROOT},     ['^'-0x20]={CEXP,CLOG,CPOWOP},       ['$'-0x20]={CDOLLAR,CSPARSE,CSELF},     ['~'-0x20]={CTILDE,CNUB,CNE},          ['|'-0x20]={CSTILE,CREV,CCANT},        
['.'-0x20]={CDOT,0,0},        [':'-0x20]={CCOLON,COBVERSE,CADVERSE}, [','-0x20]={CCOMMA,CCOMDOT,CLAMIN},    [';'-0x20]={CSEMICO,CCUT,CWORDS},       ['#'-0x20]={CPOUND,CBASE,CABASE},      ['@'-0x20]={CAT,CATDOT,CATCO},      
['/'-0x20]={CSLASH,CSLDOT,CGRADE},     ['\\'-0x20]={CBSLASH,CBSDOT,CDGRADE},['['-0x20]={CLEFT,0,CCAP},         [']'-0x20]={CRIGHT,0,0},         ['{'-0x20]={CLBRACE,CTAKE,CTAIL},       ['}'-0x20]={CRBRACE,CDROP,CCTAIL},      
['`'-0x20]={CGRAVE,CGRDOT,CGRCO},      ['\"'-0x20]={CQQ,CEXEC,CTHORN},      ['&'-0x20]={CAMP,CUNDER,CAMPCO},     ['!'-0x20]={CBANG,CFIT,CIBEAM},       ['?'-0x20]={CQUERY,CQRYDOT,CQRYCO},
['('-0x20]={CLPAR,0,0},    [')'-0x20]={CRPAR,0,0},    

['a'-0x20]={0,CALP,CACE},        ['A'-0x20]={0,CATOMIC,0},         ['b'-0x20]={0,CBDOT,0},           ['c'-0x20]={0,CCDOT,0},           ['C'-0x20]={0,CCYCLE,0},          
['e'-0x20]={0,CEPS,0},            ['E'-0x20]={0,CEBAR,0},           ['f'-0x20]={0,CFIX,0},            ['H'-0x20]={0,CHGEOM,0},          ['i'-0x20]={0,CIOTA,CICO},        ['I'-0x20]={0,CICAP,0},           
['j'-0x20]={0,CJDOT,0},           ['L'-0x20]={0,CLDOT,CLCAPCO},     ['m'-0x20]={0,CMDOT,0},           ['M'-0x20]={0,CMCAP,0},           ['n'-0x20]={0,CNDOT,0},           ['o'-0x20]={0,CCIRCLE,0},         
['p'-0x20]={0,CPOLY,CPCO},        ['q'-0x20]={0,0,CQCO},            ['r'-0x20]={0,CRDOT,0},           ['s'-0x20]={0,0,CSCO},            ['S'-0x20]={0,0,CSCAPCO},
['u'-0x20]={0,CUDOT,CUCO},        ['v'-0x20]={0,CVDOT,0},           ['x'-0x20]={0,0,CXCO},           ['0'-0x20]={CNOUN,0,CFCONS},          
['1'-0x20]={0,0,CFCONS},          ['2'-0x20]={CHOOK,0,CFCONS},      ['3'-0x20]={CFORK,0,CFCONS},      ['4'-0x20]={CADVF,0,CFCONS},      ['5'-0x20]={0,0,CFCONS},          ['6'-0x20]={0,0,CFCONS},          
['7'-0x20]={0,0,CFCONS},          ['8'-0x20]={0,0,CFCONS},          ['9'-0x20]={0,0,CFCONS},          ['F'-0x20]={0,CFDOT,CFCO},        ['Z'-0x20]={0,0,CZCO},
};
static C spellintab3[][4] = {
/* err */ {0, 0, 0, 0},
/* { */ {0, 0, 0, CFETCH},
/* } */ {0, 0, 0, CEMEND},
/* p */ {CPDERIV, 0, 0, 0},
/* & */ {0, CUNDCO, 0, 0},
/* F */ {CFDOTDOT, CFDOTCO, CFCODOT, CFCOCO},
};
// The spelling is encoded (littleendian) as (graphic) followed by 2 bits of inflection1 followed by 2 bits of inflection2: 00=NUL 01=. 10=: 11=SP
#define DOT0 0x100
#define CO0 0x200
#define SP0 0x300
#define DOT1 0x400
#define CO1 0x800
static US spellouttab[256] = {
// 2-3 free
// 5-8 free
// 11-12 free
// 14-15 free
//[CBW0000]=(UC)'\020', [CBW0001]=(UC)'\021',[CBW0010]=(UC)'\022',[CBW0011]=(UC)'\023',[CBW0100]=(UC)'\024',[CBW0101]=(UC)'\025',[CBW0110]=(UC)'\026',[CBW0111]=(UC)'\027',
//[CBW1000]=(UC)'\030',[CBW1001]=(UC)'\031',[CBW1010]=(UC)'\032',[CBW1011]=(UC)'\033',[CBW1100]=(UC)'\034',[CBW1101]=(UC)'\035',[CBW1110]=(UC)'\036',[CBW1111]=(UC)'\037',
[CBANG]=(UC)'!',[CQQ]=(UC)'\"',[CPOUND]=(UC)'#',[CDOLLAR]=(UC)'$',[CDIV]=(UC)'%',[CAMP]=(UC)'&',
[CSTAR]=(UC)'*',[CPLUS]=(UC)'+',[CCOMMA]=(UC)',',[CMINUS]=(UC)'-',[CDOT]=(UC)'.',[CSLASH]=(UC)'/',
[CNOUN]=(UC)'0', [CHOOK]=(UC)'2',[CFORK]=(UC)'3',[CADVF]=(UC)'4',
[CCOLON]=(UC)':',[CSEMICO]=(UC)';',[CLT]=(UC)'<',[CEQ]=(UC)'=',[CGT]=(UC)'>',[CQUERY]=(UC)'?',
[CAT]=(UC)'@', 
[CLEFT]=(UC)'[',[CBSLASH]=(UC)'\\',[CRIGHT]=(UC)']',[CEXP]=(UC)'^',[CINF]=(UC)'_',
[CGRAVE]=(UC)'`', 
[CLBRACE]=(UC)'{',[CSTILE]=(UC)'|',[CRBRACE]=(UC)'}',[CTILDE]=(UC)'~',
[CLPAR]=(UC)'(', [CRPAR]=(UC)')',
// 127 free
[CASGN   ]=(UC)'='+DOT0,       [CGASGN  ]=(UC)'='+CO0,       [CFLOOR  ]=(UC)'<'+DOT0,
[CLE     ]=(UC)'<'+CO0,       [CCEIL   ]=(UC)'>'+DOT0,       [CGE     ]=(UC)'>'+CO0,       
[CUSDOT  ]=(UC)'_'+DOT0,       [CPLUSDOT]=(UC)'+'+DOT0,       [CPLUSCO ]=(UC)'+'+CO0,       [CSTARDOT]=(UC)'*'+DOT0,       
[CSTARCO ]=(UC)'*'+CO0,       [CNOT    ]=(UC)'-'+DOT0,       
[CMATCH  ]=(UC)'-'+CO0,       [CDOMINO ]=(UC)'%'+DOT0,       [CSQRT   ]=(UC)'%'+CO0,
[CLOG    ]=(UC)'^'+DOT0,       [CPOWOP  ]=(UC)'^'+CO0,       [CSPARSE ]=(UC)'$'+DOT0,       [CSELF   ]=(UC)'$'+CO0,       
[CNUB    ]=(UC)'~'+DOT0,       [CNE     ]=(UC)'~'+CO0,       [CREV    ]=(UC)'|'+DOT0,
[CCANT   ]=(UC)'|'+CO0,       [COBVERSE]=(UC)':'+DOT0,       
[CADVERSE]=(UC)':'+CO0,       [CCOMDOT ]=(UC)','+DOT0,       [CLAMIN  ]=(UC)','+CO0,       [CCUT    ]=(UC)';'+DOT0,       
[CWORDS  ]=(UC)';'+CO0,       [CBASE   ]=(UC)'#'+DOT0,       [CABASE  ]=(UC)'#'+CO0,       [CFIT    ]=(UC)'!'+DOT0,       
[CIBEAM  ]=(UC)'!'+CO0,       [CSLDOT  ]=(UC)'/'+DOT0,       [CGRADE  ]=(UC)'/'+CO0,       [CBSDOT  ]=(UC)'\\'+DOT0,       
[CDGRADE ]=(UC)'\\'+CO0,      [CCAP    ]=(UC)'['+CO0,
[CHEAD   ]=(UC)'{'+DOT0,       [CTAIL   ]=(UC)'{'+CO0,       
[CBEHEAD ]=(UC)'}'+DOT0,       [CCTAIL  ]=(UC)'}'+CO0,       [CEXEC   ]=(UC)'"'+DOT0,       
[CTHORN  ]=(UC)'"'+CO0,       [CGRDOT  ]=(UC)'`'+DOT0,       [CGRCO   ]=(UC)'`'+CO0,       [CATDOT  ]=(UC)'@'+DOT0,       
[CATCO   ]=(UC)'@'+CO0,       [CAMPCO  ]=(UC)'&'+CO0,        [CUNDER  ]=(UC)'&'+DOT0,       
[CQRYDOT ]=(UC)'?'+DOT0,       [CQRYCO  ]=(UC)'?'+CO0,       [CALP    ]=(UC)'a'+DOT0,       [CATOMIC ]=(UC)'A'+DOT0,       
[CACE    ]=(UC)'a'+CO0,       [CBDOT   ]=(UC)'b'+DOT0,       [CCDOT   ]=(UC)'c'+DOT0,       [CCYCLE  ]=(UC)'C'+DOT0,       
[CEPS    ]=(UC)'e'+DOT0,       [CEBAR   ]=(UC)'E'+DOT0,       [CFIX    ]=(UC)'f'+DOT0,       [CFCAPCO ]=(UC)'F'+CO0,       
[CHGEOM  ]=(UC)'H'+DOT0,       [CIOTA   ]=(UC)'i'+DOT0,       [CICO    ]=(UC)'i'+CO0,       [CICAP   ]=(UC)'I'+DOT0,       
[CICAPCO ]=(UC)'I'+CO0,       [CJDOT   ]=(UC)'j'+DOT0,       [CLDOT   ]=(UC)'L'+DOT0,       [CLCAPCO ]=(UC)'L'+CO0,       
[CMDOT   ]=(UC)'m'+DOT0,       [CMCAP   ]=(UC)'M'+DOT0,       [CNDOT   ]=(UC)'n'+DOT0,       [CCIRCLE ]=(UC)'o'+DOT0,       
[CPOLY   ]=(UC)'p'+DOT0,       [CPCO    ]=(UC)'p'+CO0,       [CQCAPCO ]=(UC)'Q'+CO0,       [CQCO    ]=(UC)'q'+CO0,       
[CRDOT   ]=(UC)'r'+DOT0,       [CSCO    ]=(UC)'s'+CO0,       [CSCAPCO ]=(UC)'S'+CO0,
[CUDOT   ]=(UC)'u'+DOT0,       [CVDOT   ]=(UC)'v'+DOT0,       
[CUCO    ]=(UC)'u'+CO0,       [CXCO    ]=(UC)'x'+CO0,
[CFCONS  ]=(UC)'0'+CO0,       [CAMIP   ]=(UC)'}',             [CCASEV  ]=(UC)'}',             [CFETCH  ]=(UC)'{'+CO0+CO1,
[CMAP    ]=(UC)'{'+CO0+CO1,[CEMEND  ]=(UC)'}'+CO0+CO1,[CUNDCO  ]=(UC)'&'+DOT0+CO1,[CPDERIV ]=(UC)'p'+DOT0+DOT1,
[CFDOT   ]=(UC)'F'+DOT0,       [CFDOTCO ]=(UC)'F'+DOT0+CO1,[CFDOTDOT]=(UC)'F'+DOT0+DOT1,[CZCO    ]=(UC)'Z'+CO0,       
[CFCO    ]=(UC)'F'+CO0,       [CFCOCO  ]=(UC)'F'+CO0+CO1,[CFCODOT ]=(UC)'F'+CO0+DOT1,       
   };
// *s is a string with length n representing a primitive.  Convert the primitive to
// a 1-byte pseudocharacter number.  Return value of 0 means error.  This is called to audit ARs & thus must work with unverified input
C spellin(I n,C*s){
 C p=s[0];  // fetch the base character.  Must be displayable ASCII
 if(n<3){
  // 1- and 2-byte strings, with no data-dependent branches.  The dominant case
  I inf=-1;  // will hold inflections: init to 0 for 1-bytes, -1 for 2-byte: this will be the value for invalid inflections
  inf=s[n-1]==CESC1?1:inf; inf=s[n-1]==CESC2?2:inf; inf&=1-n;  // no inflection=0, .=1, :=2, erroneous inflection=-1 
  R BETWEENO((C)inf|p,0x20,0x80)?spellintab2[p-0x20][inf]:0;  // if inflection error, or not ASCII graphic, return 0 (error)
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
void spellit(UC c,UC *s){
 I spell=spellouttab[c]; // Fetch inf2/inf1 graphic
 s[0]=(C)spell; s[1]=(C)(0x203a2e00>>(((spell>>8)&0x3)<<3)); s[2]=(C)(0x203a2e00>>(((spell>>10)&0x3)<<3));   // 00100000 00111010 00101110 00000000  for NUL . :, with overlap
}

A jtspella(J jt,A w){C c,s[3];V*v;
 ARGCHK1(w);
 v=FAV(w); c=v->id;
 if(c==CFCONS)R apip(thorn1(v->fgh[2]),chrcolon); 
 spellit(c,s); 
 R str(1+!!s[1]+!!s[2],s);
}

// returns the string for primitive whose pseudochar is c - 1, 2, or 3 characters long
A jtspellout(J jt,C c){C s[3]; spellit(c,s); R str(1+!!s[1]+!!s[2],s);}
