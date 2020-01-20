/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Spelling                                                         */

#include "j.h"
#include "w.h"


static C spell[3][72]={
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

static C sp3[4][9]={
 CFETCH, CEMEND, CPDERIV, CUNDCO, CFDOTDOT, CFDOTCO, CFCODOT, CFCOCO, 0,
 '{',    '}',    'p',     '&',    'F',      'F',     'F',     'F',    0,
 CESC2,  CESC2,  CESC1,   CESC1,  CESC1,    CESC1,   CESC2,   CESC2,  0,
 CESC2,  CESC2,  CESC1,   CESC2,  CESC1,    CESC2,   CESC1,   CESC2,  0,
};   /* trigraphs */

// *s is a string with length n representing a primitive.  Convert the primitive to
// a 1-byte pseudocharacter number.  Return value of 0 means error
C spellin(I n,C*s){C c,d,p=*s,*t;I j;
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

void spellit(C c,C*s){C*q;I k;
 s[1]=s[2]=0;
 if((UC)c<=127) s[0]=c;
 else if(q=(C*)strchr(spell[1],c)){k=q-spell[1]; s[0]=spell[0][k]; s[1]=CESC1;}
 else if(q=(C*)strchr(spell[2],c)){k=q-spell[2]; s[0]=spell[0][k]; s[1]=CESC2;}
 else if(q=(C*)strchr(sp3[0],  c)){k=q-sp3[0];   s[0]=sp3[1][k];   s[1]=sp3[2][k]; s[2]=sp3[3][k];}
 else if(CAMIP==c)s[0]='}';
}    /* spell out ID c in s */

A jtspella(J jt,A w){C c,s[3];V*v;
 RZ(w);
 v=FAV(w); c=v->id;
 if(c==CFCONS)R over(thorn1(v->fgh[2]),chr[':']); 
 spellit(c,s); 
 R str(1+!!s[1]+!!s[2],s);
}

// returns the string for primitive whose pseudochar is c - 1, 2, or 3 characters long
A jtspellout(J jt,C c){C s[3]; spellit(c,s); R str(1+!!s[1]+!!s[2],s);}
