/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Character Definitions                                                   */

// if code&~CA==0 (i. e. bits 0 & 3 clear), the value is a letter if code&~(CA|C9) (i. e. bit 0 set), it is not alphameric
#define CX         11            /* other                                   */
#define CS         13            /* space or tab                            */
#define CA         6            /* letter                                  */
#define CN         4            /* N (capital N)                           */
#define CB         2            /* B (capital B)                           */
#define C9         8            /* digit or sign (underscore)              */
#define CD         1            /* dot                                     */
#define CC         3            /* colon                                   */
#define CQ         15            /* quote                                   */
#define CU         12           // like CX, but uninflectable.  Used for DDSEP.  it doesn't follow the alphanumeric rules so mustn't be a valid word for parse
#define CDD        5            // DDBGN {
#define CDDZ       9            // DDEND }

// Character names
#define C0         (C)'\000'       /*   0 000 00                              */
#define C1         (C)'\001'       /*   1 001 01                              */
#define COFF       (C)'\004'       /*   4 004 04     ctrl d                   */
#define CTAB       (C)'\011'       /*   9 011 09     tab                      */
#define CLF        (C)'\012'       /*  10 012 0a     line feed                */
#define CCR        (C)'\015'       /*  13 015 0d     carriage return          */
#define CQUOTE     (C)'\''   // '
#define CESC1      (C)'.'  //     1st escape char
#define CESC2      (C)':'  //     2nd escape char
#define CLBKTC     (C)'['
#define CGRAVEC    (C)'`'
#define CSIGN      (C)'_'  //  minus sign
#define CFF        (C)'\377'       /* 255 377 ff                              */

// Internal IDs
// Affinities:
// oblique  + comp, min/max B0001 B0110 B0111

// Current primitive order:
// fills with inverses to populate the area near 0
// !. ^: #. }: }. !: p. ^. |. o. |: \
// seldom-used verbs
// -. ? ?. I. 0:
// structural
// &. ,. ,: [ ] , ;
// the heart of the order: arithmetic
// j. *: +: /: ^ % - * +. *. +
// comparison
// = ~: <: >: <  > e. E.   <. >.
// modifiers (must start at '0'
// NOUN / HOOK FORK ADVF [: & @ &: @:  @.
// seldom-used verbs
// $: `: : ~ u. v.
// more structural
// {. {:   # $ /. :: "   #: i. i: {   -: ~.
// verbs with inverses
//  \.p.. x: ": s: u:    \: ;: $. ;. A .p: q:
// verbs with identities
// . %. C. | r. %: ! b.

// 0 reserved for 'invalid'
#define CFIT       (C)0x03  // !.
#define CPOWOP     (C)0x04  // ^:
#define CBASE      (C)0x05  // #.
#define CBEHEAD    (C)0x06  // }.
#define CDROP      (C)0x06  // }.
#define CCTAIL     (C)0x07  // }:
#define CIBEAM     (C)0x08  // !:
#define CPOLY      (C)0x09  // p.
#define CLOG       (C)0x0a  // ^.
#define CREV       (C)0x0b  // |.
#define CROT       (C)0x0b  // |.
#define CCIRCLE    (C)0x0c  // o.
#define CCANT      (C)0x0d  // |:
#define CBSLASH    (C)0x0e  // \   backslash must be 0x40 from \.
#define CNOT       (C)0x0f  // -.
#define CLESS      (C)0x0f  // -.
#define CICAP      (C)0x10  // I.
#define CQUERY     (C)0x11  // ?
#define CQRYDOT    (C)0x12  // ?.
#define CFCONS     (C)0x13  // 0: 1: 2: etc.
#define CUNDER     (C)0x14  // &.
#define CCOMDOT    (C)0x15  // ,.
#define CLEFT      (C)0x16  // [   must be paired with CRIGHT
#define CRIGHT     (C)0x17  // ]
#define CLAMIN     (C)0x18  // ,:
#define CCOMMA     (C)0x19  // ,
#define CSEMICO    (C)0x1a  // ;
#define CRAZE      (C)0x1a  //
#define CJDOT      (C)0x1b  // j.
#define CGRADE     (C)0x1c  // /:
#define CEXP       (C)0x1d  // ^
#define CDIV       (C)0x1e  // %
#define CMINUS     (C)0x1f  // -
#define CSTARCO    (C)0x20  // *:  20-2f used in cip.c and ar.c
#define CPLUSCO    (C)0x21  // +:
#define CSTAR      (C)0x22  // *
#define CPLUS      (C)0x23  // + +. *. must stay in order.  Low digit is combining type-1
#define CPLUSDOT   (C)0x24  // +.  +. *. x ~:  are so tested
#define CSTARDOT   (C)0x25  // *.
#define CEQ        (C)0x26  // =   must be paired with ~:  = through e. are in the order used by comparison combination
#define CNE        (C)0x27  // ~:
#define CLT        (C)0x28  // <   must be paired with <:
#define CBOX       (C)0x28  // <
#define CLE        (C)0x29  // <:
#define CGE        (C)0x2a  // >:   must be paired with >
#define COPE       (C)0x2b  // >
#define CGT        (C)0x2b  // >
#define CEBAR      (C)0x2c  // E.  must pair with e.
#define CEPS       (C)0x2d  // e.
#define CMIN       (C)0x2e  // <.
#define CFLOOR     (C)0x2e  // <.
#define CMAX       (C)0x2f  // >. must be paired with <. as upper of pair
#define CCEIL      (C)0x2f  // >. 20-2f used in cip.c and ar.c
#define CNOUN      (C)0x30  // '0' - used in AR for noun
#define CSLASH     (C)0x31  // /
#define CHOOK      (C)0x32  // '2' - used in AR for hook  is paired with ADVF 2 distant
#define CFORK      (C)0x33  // '3' - used in AR for fork is paired with CCAP 3 distant
#define CADVF      (C)0x34  // '4' - used in AR for any train
#define CCAP       (C)0x35  // [: - block HOOK-CCAP must be togather
#define CAMPCO     (C)0x36  // &:  must be paired with &
#define CAMP       (C)0x37  // &  must be CFORK^4
#define CATCO      (C)0x38  // @:  must be paired with @
#define CAT        (C)0x39  // @
#define CATDOT     (C)0x3a  // @. paired with `:
#define CGRCO      (C)0x3b  // `:
#define CSELF      (C)0x3c  // $: @. `: $: all in a row
#define CCOLON     (C)0x3d  // :
#define CUDOT      (C)0x3e  // u.  must be paired v.
#define CVDOT      (C)0x3f  // v.
#define CTAIL      (C)0x40  // {:
#define CTAKE      (C)0x41  // {.
#define CHEAD      (C)0x41  // {.
#define CPOUND     (C)0x42  // #  must pair with $
#define CDOLLAR    (C)0x43  // $
#define CTILDE     (C)0x44  // ~
#define CSLDOT     (C)0x45  // /.
#define CADVERSE   (C)0x46  // ::
#define CQQ        (C)0x47  // "    double quote
#define CIOTA      (C)0x48  // i. i. and i: must be 2 apart.  low digit is combining code
#define CABASE     (C)0x49  // #:
#define CICO       (C)0x4a  // i:
#define CLBRACE    (C)0x4b  // {
#define CFROM      (C)0x4b  // {
#define CHALVE     (C)0x4c  // -:
#define CMATCH     (C)0x4c  // -:
#define CNUB       (C)0x4d  // ~.
#define CBSDOT     (C)0x4e  // \. must be 0x40 from backslash
#define CPDERIV    (C)0x4f  // p..
#define CXCO       (C)0x50  // x:
#define CTHORN     (C)0x51  // ":
#define CSCO       (C)0x52  // s:
#define CUCO       (C)0x53  // u:
#define CDGRADE    (C)0x54  // \:
#define CWORDS     (C)0x55  // ;:
#define CSPARSE    (C)0x56  // $.
#define CCUT       (C)0x57  // ;.
#define CUNDCO     (C)0x58  // &.:
#define COBVERSE   (C)0x59  // :.
#define CATOMIC    (C)0x5a  // A.
#define CPCO       (C)0x5b  // p:
#define CQCO       (C)0x5c  // q:
#define CDOT       (C)0x5d  // .
#define CDOMINO    (C)0x5e  // %.
#define CCYCLE     (C)0x5f  // C.
#define CSTILE     (C)0x60  // |
#define CRDOT      (C)0x61  // r.
#define CSQRT      (C)0x62  // %:
#define CROOT      (C)0x62  // %:
#define CBANG      (C)0x63  // !
#define CBDOT      (C)0x64  // b.
#define CSLDOTDOT  (C)0x65  // /..

#define CGRAVE     (C)0x67 // `
#define CINF       (C)0x68  //  infinity

#define CUSDOT     (C)0x6c  // _.
#define CRBRACE    (C)0x6d  // }
#define CAMEND     (C)0x6d  // }

#define CBW0000    (C)0x70  // bitwise
#define CBW0001    (C)0x71  // bitwise
#define CBW0010    (C)0x72  // bitwise
#define CBW0011    (C)0x73  // bitwise
#define CBW0100    (C)0x74  // bitwise
#define CBW0101    (C)0x75  // bitwise
#define CBW0110    (C)0x76  // bitwise
#define CBW0111    (C)0x77  // bitwise
#define CBW1000    (C)0x78  // bitwise
#define CBW1001    (C)0x79  // bitwise
#define CBW1010    (C)0x7a  // bitwise
#define CBW1011    (C)0x7b  // bitwise
#define CBW1100    (C)0x7c  // bitwise
#define CBW1101    (C)0x7d  // bitwise
#define CBW1110    (C)0x7e  // bitwise
#define CBW1111    (C)0x7f  // bitwise

#define CASGN      (C)0x80       /* 128 200 80 =.                           */
#define CGASGN     (C)0x81       /* 129 201 81 =:                           */
#define CLPAR      (C)0x82  // (
#define CRPAR      (C)0x83  // )
#define CLEV       (C)0x84  // [.
#define CDEX       (C)0x85  // ].
#define CIDENT     (C)0x86  // ]:
#define CDUMMY     (C)0x87  // no graphic - holds DUMMYSELF, which is used for flags only.  Must be >= 0x80 so will not look like a character
#define CENQUEUE   (C)0x88  // no graphic - when used in AR, produces a single space.  Used to create recognizable AR to send to eformat from enqueue
// 88-af free
#define CEXEC      (C)0xb0       /* 176 260 b0 ".                           */
#define CGRDOT     (C)0xb2       /* 178 262 b2 `.                           */

#define CQRYCO     (C)0xb9       /* 185 271 b9 ?:                           */
#define CALP       (C)0xba       /* 186 272 ba a.                           */
#define CACE       (C)0xbc       /* 188 274 bc a:                           */
#define CCDOT      (C)0xbe       /* 190 276 be c.                           */
#define CFIX       (C)0xc6       /* 198 306 c6 f.                           */
#define CFCAPCO    (C)0xc7       /* 199 307 c7 F:                           */
#define CHGEOM     (C)0xc8       /* 200 310 c8 H.                           */
#define CICAPCO    (C)0xcc       /* 204 314 cc I:                           */
#define CLDOT      (C)0xce       /* 206 316 ce L.                           */
#define CLCAPCO    (C)0xcf       /* 207 317 cf L:                           */
#define CMDOT      (C)0xd9       /* 208 320 d0 m.                           */
#define CMCAP      (C)0xd1       /* 209 321 d1 M.                           */
#define CNDOT      (C)0xd2       /* 210 322 d2 n.                           */
#define CTDOT      (C)0xd3  // t.
#define CTCAPDOT   (C)0xd4  // T.
#define CQCAPCO    (C)0xd6       /* 214 326 d6 Q:                           */
#define CSCAPCO    (C)0xda       /* 218 332 da S:                           */
// db-e8 free

#define CAMIP      (C)0xe9       /* 233 351 e9 }   amend in place           */
#define CCASEV     (C)0xea       /* 234 352 ea }   case in place            */
#define CFETCH     (C)0xeb       /* 235 353 eb {::                          */
#define CMAP       (C)0xec       /* 236 354 ec {::                          */
#define CEMEND     (C)0xed       /* 237 355 ed }::                          */
#define CFDOT      (C)0xf0       // 240 360 f0 F.  the ID codes are used as flags
#define CFDOTCO    (C)0xf1       // 241 361 f1 F.:
#define CFDOTDOT   (C)0xf2       // 242 362 f2 F..
#define CZCO       (C)0xf3       // 243 363 f3 Z:
#define CFCO       (C)0xf4       // 244 364 f4 F:
#define CFCOCO     (C)0xf5       // 245 365 f5 F::
#define CFCODOT    (C)0xf6       // 246 366 f6 F:.
#define CCYCITER   (C)0xf7  // cyclic iterator
// f8-ff free
