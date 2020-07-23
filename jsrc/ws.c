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
#if 0 // obsolete
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
#else
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
#endif
static C spellintab3[][4] = {
/* err */ {0, 0, 0, 0},
/* { */ {0, 0, 0, CFETCH},
/* } */ {0, 0, 0, CEMEND},
/* p */ {CPDERIV, 0, 0, 0},
/* & */ {0, CUNDCO, 0, 0},
/* F */ {CFDOTDOT, CFDOTCO, CFCODOT, CFCOCO},
};
// The spelling is encoded (littleendian) as (graphic) followed by 4 bits of inflection1 followed by 4 bits of inflection2: 0000=NUL 0111=. 1110=:
#define DOT0 0x700
#define CO0 0xe00
#define DOT1 0x7000
#define CO1 0xe000
static US spellouttab[256] = {
#if 1
// obsolete [C0]=(UC)'\000', [C1]=(UC)'\001',
// 2-3 free
// obsolete [COFF]=(UC)'\004',
// 5-8 free
// obsolete [CTAB]=(UC)'\011', [CLF]=(UC)'\012',
// 11-12 free
// obsolete [CCR]=(UC)'\015', 
// 14-15 free
//[CBW0000]=(UC)'\020', [CBW0001]=(UC)'\021',[CBW0010]=(UC)'\022',[CBW0011]=(UC)'\023',[CBW0100]=(UC)'\024',[CBW0101]=(UC)'\025',[CBW0110]=(UC)'\026',[CBW0111]=(UC)'\027',
//[CBW1000]=(UC)'\030',[CBW1001]=(UC)'\031',[CBW1010]=(UC)'\032',[CBW1011]=(UC)'\033',[CBW1100]=(UC)'\034',[CBW1101]=(UC)'\035',[CBW1110]=(UC)'\036',[CBW1111]=(UC)'\037',
[CBANG]=(UC)'!',[CQQ]=(UC)'\"',[CPOUND]=(UC)'#',[CDOLLAR]=(UC)'$',[CDIV]=(UC)'%',[CAMP]=(UC)'&',
// obsolete [CLPAR]=(UC)'(',[CRPAR]=(UC)')',
[CSTAR]=(UC)'*',[CPLUS]=(UC)'+',[CCOMMA]=(UC)',',[CMINUS]=(UC)'-',[CDOT]=(UC)'.',[CSLASH]=(UC)'/',
[CNOUN]=(UC)'0', [CHOOK]=(UC)'2',[CFORK]=(UC)'3',[CADVF]=(UC)'4',
[CCOLON]=(UC)':',[CSEMICO]=(UC)';',[CLT]=(UC)'<',[CEQ]=(UC)'=',[CGT]=(UC)'>',[CQUERY]=(UC)'?',
[CAT]=(UC)'@', 
[CLEFT]=(UC)'[',[CBSLASH]=(UC)'\\',[CRIGHT]=(UC)']',[CEXP]=(UC)'^',[CINF]=(UC)'_',
[CGRAVE]=(UC)'`', 
[CLBRACE]=(UC)'{',[CSTILE]=(UC)'|',[CRBRACE]=(UC)'}',[CTILDE]=(UC)'~',
[CLPAR]=(UC)'(', [CRPAR]=(UC)')',
// 127 free
#endif
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
// obsolete   C *bp=(C*)&spellintab2[p-0x20];  // base address of 1-2-byte chars
  I inf=-1;  // will hold inflections: init to 0 for 1-bytes, -1 for 2-byte: this will be the value for invalid inflections
  inf=s[n-1]==CESC1?1:inf; inf=s[n-1]==CESC2?2:inf; inf&=1-n;  // no inflection=0, .=1, :=2, erroneous inflection=-1 
// obsolete   bp+=inf; /* obsolete bp=n==1?s:bp;*/  // point bp to the inflected char, or the input byte if 1-byte char
  R BETWEENO((C)inf|p,0x20,0x80)?spellintab2[p-0x20][inf]:0;  // if inflection error, or not ASCII graphic, return 0 (error)
// obsolete   R *bp;  // return the character
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
// obsolete  I spell=spellouttab[c&127]; spell=c&128?spell:c; // Fetch inf2/inf1 graphic in case inflected; if not inflected, use the char itself, with inflections 0
 I spell=spellouttab[c]; // Fetch inf2/inf1 graphic
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
