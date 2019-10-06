/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Character Definitions                                                   */


#define CX         0            /* other                                   */
#define CS         1            /* space or tab                            */
#define CA         2            /* letter                                  */
#define CN         3            /* N (capital N)                           */
#define CB         4            /* B (capital B)                           */
#define C9         5            /* digit or sign (underscore)              */
#define CD         6            /* dot                                     */
#define CC         7            /* colon                                   */
#define CQ         8            /* quote                                   */

#define CESC1      (C)'.'          /*  46 056 2e     1st escape char          */
#define CESC2      (C)':'          /*  58 072 3a     2nd escape char          */

#define C0         (C)'\000'       /*   0 000 00                              */
#define C1         (C)'\001'       /*   1 001 01                              */
// 2-3 free
#define COFF       (C)'\004'       /*   4 004 04     ctrl d                   */
// 5-8 free
#define CTAB       (C)'\011'       /*   9 011 09     tab                      */
#define CLF        (C)'\012'       /*  10 012 0a     line feed                */
// 11-12 free
#define CCR        (C)'\015'       /*  13 015 0d     carriage return          */
// 14-15 free
#define CBW0000    (C)'\020'       /*  16 020 10     bitwise fns              */
#define CBW0001    (C)'\021'       /*  17 021 11                              */
#define CBW0010    (C)'\022'       /*  18 022 12                              */
#define CBW0011    (C)'\023'       /*  19 023 13                              */
#define CBW0100    (C)'\024'       /*  20 024 14                              */
#define CBW0101    (C)'\025'       /*  21 025 15                              */
#define CBW0110    (C)'\026'       /*  22 026 16                              */
#define CBW0111    (C)'\027'       /*  23 027 17                              */
#define CBW1000    (C)'\030'       /*  24 030 18                              */
#define CBW1001    (C)'\031'       /*  25 031 19                              */
#define CBW1010    (C)'\032'       /*  26 032 1a                              */
#define CBW1011    (C)'\033'       /*  27 033 1b                              */
#define CBW1100    (C)'\034'       /*  28 034 1c                              */
#define CBW1101    (C)'\035'       /*  29 035 1d                              */
#define CBW1110    (C)'\036'       /*  30 036 1e                              */
#define CBW1111    (C)'\037'       /*  31 037 1f                              */
// 32 free
#define CBANG      (C)'!'          /*  33 041 21                              */
#define CQQ        (C)'\042'       /*  34 042 22     double quote             */
#define CPOUND     (C)'#'          /*  35 043 23                              */
#define CDOLLAR    (C)'$'          /*  36 044 24                              */
#define CDIV       (C)'%'          /*  37 045 25                              */
#define CQUOTE     (C)'\047'       /*  39 047 27     single quote             */
#define CLPAR      (C)'('          /*  40 050 28                              */
#define CRPAR      (C)')'          /*  41 051 29                              */
#define CSTAR      (C)'*'          /*  42 052 2a                              */
#define CPLUS      (C)'+'          /*  43 053 2b                              */
#define CCOMMA     (C)','          /*  44 054 2c                              */
#define CMINUS     (C)'-'          /*  45 055 2d                              */
#define CDOT       (C)'.'          /*  46 056 2e                              */
#define CSLASH     (C)'/'          /*  47 057 2f                              */
#define CNOUN      (C)'0'          /*  48 060 30                              */
// 49 free
#define CHOOK      (C)'2'          /*  50 062 32                              */
#define CFORK      (C)'3'          /*  51 063 33                              */
#define CADVF      (C)'4'          /*  52 064 34     bonded conjunction       */
// 53-57 free
#define CCOLON     (C)':'          /*  58 072 3a                              */
#define CSEMICO    (C)';'          /*  59 073 3b                              */
#define CRAZE      (C)';'          /*  59 073 3b                              */
#define CBOX       (C)'<'          /*  60 074 3c                              */
#define CLT        (C)'<'          /*  60 074 3c                              */
#define CEQ        (C)'='          /*  61 075 3d                              */
#define COPE       (C)'>'          /*  62 076 3e                              */
#define CGT        (C)'>'          /*  62 076 3e                              */
#define CQUERY     (C)'?'          /*  63 077 3f                              */
// 65-90 free
#define CLEFT      (C)'['          /*  91 133 5b                              */
#define CBSLASH    (C)'\134'       /*  92 134 5c \   backslash                */
#define CRIGHT     (C)']'          /*  93 135 5d                              */
#define CEXP       (C)'^'          /*  94 136 5e                              */
#define CSIGN      (C)'_'          /*  95 137 5f     minus sign               */
#define CINF       (C)'_'          /*  95 137 5f     infinity                 */
#define CGRAVE     (C)'`'          /*  96 140 60                              */
// 97-122 free
#define CAT        (C)'@'          /*  64 100 40                              */
#define CAMP       (C)'&'          /*  38 046 26                              */
#define CATCO      (C)'\265'       /* 181 265 b5 @:                           */
#define CAMPCO     (C)'\267'       /* 183 267 b7 &:                           */

#define CLBRACE    (C)'{'          /* 123 173 7b                              */
#define CFROM      (C)'{'          /* 123 173 7b                              */
#define CSTILE     (C)'|'          /* 124 174 7c                              */
#define CRBRACE    (C)'}'          /* 125 175 7d                              */
#define CAMEND     (C)'}'          /* 125 175 7d                              */
#define CTILDE     (C)'~'          /* 126 176 7e                              */
// 127 free
#define CASGN      (C)'\200'       /* 128 200 80 =.                           */
#define CGASGN     (C)'\201'       /* 129 201 81 =:                           */
#define CFLOOR     (C)'\202'       /* 130 202 82 <.                           */
#define CMIN       (C)'\202'       /* 130 202 82 <.                           */
#define CLE        (C)'\203'       /* 131 203 83 <:                           */
#define CCEIL      (C)'\204'       /* 132 204 84 >.                           */
#define CMAX       (C)'\204'       /* 132 204 84 >.                           */
#define CGE        (C)'\205'       /* 133 205 85 >:                           */
#define CUSDOT     (C)'\206'       /* 134 206 86 _.                           */
// 135 free
#define CPLUSDOT   (C)'\210'       /* 136 210 88 +.                           */
#define CPLUSCO    (C)'\211'       /* 137 211 89 +:                           */
#define CSTARDOT   (C)'\212'       /* 138 212 8a *.                           */
#define CSTARCO    (C)'\213'       /* 139 213 8b *:                           */
#define CNOT       (C)'\214'       /* 140 214 8c -.                           */
#define CLESS      (C)'\214'       /* 140 214 8c -.                           */
#define CHALVE     (C)'\215'       /* 141 215 8d -:                           */
#define CMATCH     (C)'\215'       /* 141 215 8d -:                           */
#define CDOMINO    (C)'\216'       /* 142 216 8e %.                           */
#define CSQRT      (C)'\217'       /* 143 217 8f %:                           */
#define CROOT      (C)'\217'       /* 143 217 8f %:                           */
#define CLOG       (C)'\220'       /* 144 220 90 ^.                           */
#define CPOWOP     (C)'\221'       /* 145 221 91 ^:                           */
#define CSPARSE    (C)'\222'       /* 146 222 92 $.                           */
#define CSELF      (C)'\223'       /* 147 223 93 $:                           */
#define CNUB       (C)'\224'       /* 148 224 94 ~.                           */
#define CNE        (C)'\225'       /* 149 225 95 ~:                           */
#define CREV       (C)'\226'       /* 150 226 96 |.                           */
#define CROT       (C)'\226'       /* 150 226 96 |.                           */
#define CCANT      (C)'\227'       /* 151 227 97 |:                           */
#define CEVEN      (C)'\230'       /* 152 230 98 ..                           */
#define CODD       (C)'\231'       /* 153 231 99 .:                           */
#define COBVERSE   (C)'\232'       /* 154 232 9a :.                           */
#define CADVERSE   (C)'\233'       /* 155 233 9b ::                           */
#define CCOMDOT    (C)'\234'       /* 156 234 9c ,.                           */
#define CLAMIN     (C)'\235'       /* 157 235 9d ,:                           */
#define CCUT       (C)'\236'       /* 158 236 9e ;.                           */
#define CWORDS     (C)'\237'       /* 159 237 9f ;:                           */
#define CBASE      (C)'\240'       /* 160 240 a0 #.                           */
#define CABASE     (C)'\241'       /* 161 241 a1 #:                           */
#define CFIT       (C)'\242'       /* 162 242 a2 !.                           */
#define CIBEAM     (C)'\243'       /* 163 243 a3 !:                           */
#define CSLDOT     (C)'\244'       /* 164 244 a4 /.                           */
#define CGRADE     (C)'\245'       /* 165 245 a5 /:                           */
#define CBSDOT     (C)'\246'       /* 166 246 a6 \.                           */
#define CDGRADE    (C)'\247'       /* 167 247 a7 \:                           */
#define CLEV       (C)'\250'       /* 168 250 a8 [.                           */
#define CCAP       (C)'\251'       /* 169 251 a9 [:                           */
#define CDEX       (C)'\252'       /* 170 252 aa ].                           */
#define CIDA       (C)'\253'       /* 171 253 ab ]:                           */
#define CHEAD      (C)'\254'       /* 172 254 ac {.                           */
#define CTAKE      (C)'\254'       /* 172 254 ac {.                           */
#define CTAIL      (C)'\255'       /* 173 255 ad {:                           */
#define CBEHEAD    (C)'\256'       /* 174 256 ae }.                           */
#define CDROP      (C)'\256'       /* 174 256 ae }.                           */
#define CCTAIL     (C)'\257'       /* 175 257 af }:                           */
#define CEXEC      (C)'\260'       /* 176 260 b0 ".                           */
#define CTHORN     (C)'\261'       /* 177 261 b1 ":                           */
#define CGRDOT     (C)'\262'       /* 178 262 b2 `.                           */
#define CGRCO      (C)'\263'       /* 179 263 b3 `:                           */
#define CATDOT     (C)'\264'       /* 180 264 b4 @.                           */
#define CUNDER     (C)'\266'       /* 182 266 b6 &.                           */
#define CQRYDOT    (C)'\270'       /* 184 270 b8 ?.                           */
#define CQRYCO     (C)'\271'       /* 185 271 b9 ?:                           */

#define CALP       (C)'\272'       /* 186 272 ba a.                           */
#define CATOMIC    (C)'\273'       /* 187 273 bb A.                           */
#define CACE       (C)'\274'       /* 188 274 bc a:                           */
#define CBDOT      (C)'\275'       /* 189 275 bd b.                           */
#define CCDOT      (C)'\276'       /* 190 276 be c.                           */
// 191 free
#define CCYCLE     (C)'\300'       /* 192 300 c0 C.                           */
// obsolete #define CDDOT      (C)'\301'       /* 193 301 c1 d.                           */
// obsolete #define CDCAP      (C)'\302'       /* 194 302 c2 D.                           */
// obsolete #define CDCAPCO    (C)'\303'       /* 195 303 c3 D:                           */
#define CEPS       (C)'\304'       /* 196 304 c4 e.                           */
#define CEBAR      (C)'\305'       /* 197 305 c5 E.                           */
#define CFIX       (C)'\306'       /* 198 306 c6 f.                           */
#define CFCAPCO    (C)'\307'       /* 199 307 c7 F:                           */
#define CHGEOM     (C)'\310'       /* 200 310 c8 H.                           */
#define CIOTA      (C)'\311'       /* 201 311 c9 i.                           */
#define CICO       (C)'\312'       /* 202 312 ca i:                           */
#define CICAP      (C)'\313'       /* 203 313 cb I.                           */
#define CICAPCO    (C)'\314'       /* 204 314 cc I:                           */
#define CJDOT      (C)'\315'       /* 205 315 cd j.                           */
#define CLDOT      (C)'\316'       /* 206 316 ce L.                           */
#define CLCAPCO    (C)'\317'       /* 207 317 cf L:                           */
#define CMDOT      (C)'\320'       /* 208 320 d0 m.                           */
#define CMCAP      (C)'\321'       /* 209 321 d1 M.                           */
#define CNDOT      (C)'\322'       /* 210 322 d2 n.                           */
#define CCIRCLE    (C)'\323'       /* 211 323 d3 o.                           */
#define CPOLY      (C)'\324'       /* 212 324 d4 p.                           */
#define CPCO       (C)'\325'       /* 213 325 d5 p:                           */
#define CQCAPCO    (C)'\326'       /* 214 326 d6 Q:                           */
#define CQCO       (C)'\327'       /* 215 327 d7 q:                           */
#define CRDOT      (C)'\330'       /* 216 330 d8 r.                           */
#define CSCO       (C)'\331'       /* 217 331 d9 s:                           */
#define CSCAPCO    (C)'\332'       /* 218 332 da S:                           */
#define CTDOT      (C)'\333'       /* 219 333 db t.                           */
#define CTCO       (C)'\334'       /* 220 334 dc t:                           */
#define CTCAP      (C)'\335'       /* 221 335 dd T.                           */
#define CUDOT      (C)'\336'       /* 222 336 de u.                           */
#define CVDOT      (C)'\337'       /* 223 337 df v.                           */
#define CUCO       (C)'\340'       /* 224 340 e0 u:                           */
#define CXDOT      (C)'\341'       /* 225 341 e1 x.                           */
#define CXCO       (C)'\342'       /* 226 342 e2 x:                           */
#define CYDOT      (C)'\343'       /* 227 343 e3 y.                           */
// 228-231 free

#define CFCONS     (C)'\350'       /* 232 350 e8 0: 1: 2: etc.                */
#define CAMIP      (C)'\351'       /* 233 351 e9 }   amend in place           */
#define CCASEV     (C)'\352'       /* 234 352 ea }   case in place            */
#define CFETCH     (C)'\353'       /* 235 353 eb {::                          */
#define CMAP       (C)'\354'       /* 236 354 ec {::                          */
#define CEMEND     (C)'\355'       /* 237 355 ed }::                          */
#define CUNDCO     (C)'\356'       /* 238 356 ee &.:                          */
#define CPDERIV    (C)'\357'       /* 239 357 ef p..                          */
// obsolete #define CAPIP      (C)'\360'       /* 240 360 f0 ,   append in place          */
#define CFDOT      (C)'\360'       // 240 360 f0 F.  the ID codes are used as flags
#define CFDOTCO    (C)'\361'       // 241 361 f1 F.:
#define CFDOTDOT   (C)'\362'       // 242 362 f2 F..
#define CZCO       (C)'\363'       // 243 363 f3 Z:
#define CFCO       (C)'\364'       // 244 364 f4 F:
#define CFCOCO     (C)'\365'       // 245 365 f5 F::
#define CFCODOT    (C)'\366'       // 246 366 f6 F:.
// 240-254 free

#define CFF        (C)'\377'       /* 255 377 ff                              */
