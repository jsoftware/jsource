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

#define CESC1      '.'          /*  46 056 2e     1st escape char          */
#define CESC2      ':'          /*  58 072 3a     2nd escape char          */

#define C0         '\000'       /*   0 000 00                              */
#define C1         '\001'       /*   1 001 01                              */
#define COFF       '\004'       /*   4 004 04     ctrl d                   */
#define CTAB       '\011'       /*   9 011 09     tab                      */
#define CLF        '\012'       /*  10 012 0a     line feed                */
#define CCR        '\015'       /*  13 015 0d     carriage return          */
#define CBW0000    '\020'       /*  16 020 10     bitwise fns              */
#define CBW0001    '\021'       /*  17 021 11                              */
#define CBW0010    '\022'       /*  18 022 12                              */
#define CBW0011    '\023'       /*  19 023 13                              */
#define CBW0100    '\024'       /*  20 024 14                              */
#define CBW0101    '\025'       /*  21 025 15                              */
#define CBW0110    '\026'       /*  22 026 16                              */
#define CBW0111    '\027'       /*  23 027 17                              */
#define CBW1000    '\030'       /*  24 030 18                              */
#define CBW1001    '\031'       /*  25 031 19                              */
#define CBW1010    '\032'       /*  26 032 1a                              */
#define CBW1011    '\033'       /*  27 033 1b                              */
#define CBW1100    '\034'       /*  28 034 1c                              */
#define CBW1101    '\035'       /*  29 035 1d                              */
#define CBW1110    '\036'       /*  30 036 1e                              */
#define CBW1111    '\037'       /*  31 037 1f                              */
#define CBANG      '!'          /*  33 041 21                              */
#define CQQ        '\042'       /*  34 042 22     double quote             */
#define CPOUND     '#'          /*  35 043 23                              */
#define CDOLLAR    '$'          /*  36 044 24                              */
#define CDIV       '%'          /*  37 045 25                              */
#define CAMP       '&'          /*  38 046 26                              */
#define CQUOTE     '\047'       /*  39 047 27     single quote             */
#define CLPAR      '('          /*  40 050 28                              */
#define CRPAR      ')'          /*  41 051 29                              */
#define CSTAR      '*'          /*  42 052 2a                              */
#define CPLUS      '+'          /*  43 053 2b                              */
#define CCOMMA     ','          /*  44 054 2c                              */
#define CMINUS     '-'          /*  45 055 2d                              */
#define CDOT       '.'          /*  46 056 2e                              */
#define CSLASH     '/'          /*  47 057 2f                              */
#define CNOUN      '0'          /*  48 060 30                              */
#define CHOOK      '2'          /*  50 062 32                              */
#define CFORK      '3'          /*  51 063 33                              */
#define CADVF      '4'          /*  52 064 34     bonded conjunction       */
#define CCOLON     ':'          /*  58 072 3a                              */
#define CSEMICO    ';'          /*  59 073 3b                              */
#define CRAZE      ';'          /*  59 073 3b                              */
#define CBOX       '<'          /*  60 074 3c                              */
#define CLT        '<'          /*  60 074 3c                              */
#define CEQ        '='          /*  61 075 3d                              */
#define COPE       '>'          /*  62 076 3e                              */
#define CGT        '>'          /*  62 076 3e                              */
#define CQUERY     '?'          /*  63 077 3f                              */
#define CAT        '@'          /*  64 100 40                              */
#define CLEFT      '['          /*  91 133 5b                              */
#define CBSLASH    '\134'       /*  92 134 5c \   backslash                */
#define CRIGHT     ']'          /*  93 135 5d                              */
#define CEXP       '^'          /*  94 136 5e                              */
#define CSIGN      '_'          /*  95 137 5f     minus sign               */
#define CINF       '_'          /*  95 137 5f     infinity                 */
#define CGRAVE     '`'          /*  96 140 60                              */
#define CLBRACE    '{'          /* 123 173 7b                              */
#define CFROM      '{'          /* 123 173 7b                              */
#define CSTILE     '|'          /* 124 174 7c                              */
#define CRBRACE    '}'          /* 125 175 7d                              */
#define CAMEND     '}'          /* 125 175 7d                              */
#define CTILDE     '~'          /* 126 176 7e                              */
#define CASGN      '\200'       /* 128 200 80 =.                           */
#define CGASGN     '\201'       /* 129 201 81 =:                           */
#define CFLOOR     '\202'       /* 130 202 82 <.                           */
#define CMIN       '\202'       /* 130 202 82 <.                           */
#define CLE        '\203'       /* 131 203 83 <:                           */
#define CCEIL      '\204'       /* 132 204 84 >.                           */
#define CMAX       '\204'       /* 132 204 84 >.                           */
#define CGE        '\205'       /* 133 205 85 >:                           */
#define CUSDOT     '\206'       /* 134 206 86 _.                           */
#define CPLUSDOT   '\210'       /* 136 210 88 +.                           */
#define CPLUSCO    '\211'       /* 137 211 89 +:                           */
#define CSTARDOT   '\212'       /* 138 212 8a *.                           */
#define CSTARCO    '\213'       /* 139 213 8b *:                           */
#define CNOT       '\214'       /* 140 214 8c -.                           */
#define CLESS      '\214'       /* 140 214 8c -.                           */
#define CHALVE     '\215'       /* 141 215 8d -:                           */
#define CMATCH     '\215'       /* 141 215 8d -:                           */
#define CDOMINO    '\216'       /* 142 216 8e %.                           */
#define CSQRT      '\217'       /* 143 217 8f %:                           */
#define CROOT      '\217'       /* 143 217 8f %:                           */
#define CLOG       '\220'       /* 144 220 90 ^.                           */
#define CPOWOP     '\221'       /* 145 221 91 ^:                           */
#define CSPARSE    '\222'       /* 146 222 92 $.                           */
#define CSELF      '\223'       /* 147 223 93 $:                           */
#define CNUB       '\224'       /* 148 224 94 ~.                           */
#define CNE        '\225'       /* 149 225 95 ~:                           */
#define CREV       '\226'       /* 150 226 96 |.                           */
#define CROT       '\226'       /* 150 226 96 |.                           */
#define CCANT      '\227'       /* 151 227 97 |:                           */
#define CEVEN      '\230'       /* 152 230 98 ..                           */
#define CODD       '\231'       /* 153 231 99 .:                           */
#define COBVERSE   '\232'       /* 154 232 9a :.                           */
#define CADVERSE   '\233'       /* 155 233 9b ::                           */
#define CCOMDOT    '\234'       /* 156 234 9c ,.                           */
#define CLAMIN     '\235'       /* 157 235 9d ,:                           */
#define CCUT       '\236'       /* 158 236 9e ;.                           */
#define CWORDS     '\237'       /* 159 237 9f ;:                           */
#define CBASE      '\240'       /* 160 240 a0 #.                           */
#define CABASE     '\241'       /* 161 241 a1 #:                           */
#define CFIT       '\242'       /* 162 242 a2 !.                           */
#define CIBEAM     '\243'       /* 163 243 a3 !:                           */
#define CSLDOT     '\244'       /* 164 244 a4 /.                           */
#define CGRADE     '\245'       /* 165 245 a5 /:                           */
#define CBSDOT     '\246'       /* 166 246 a6 \.                           */
#define CDGRADE    '\247'       /* 167 247 a7 \:                           */
#define CLEV       '\250'       /* 168 250 a8 [.                           */
#define CCAP       '\251'       /* 169 251 a9 [:                           */
#define CDEX       '\252'       /* 170 252 aa ].                           */
#define CIDA       '\253'       /* 171 253 ab ]:                           */
#define CHEAD      '\254'       /* 172 254 ac {.                           */
#define CTAKE      '\254'       /* 172 254 ac {.                           */
#define CTAIL      '\255'       /* 173 255 ad {:                           */
#define CBEHEAD    '\256'       /* 174 256 ae }.                           */
#define CDROP      '\256'       /* 174 256 ae }.                           */
#define CCTAIL     '\257'       /* 175 257 af }:                           */
#define CEXEC      '\260'       /* 176 260 b0 ".                           */
#define CTHORN     '\261'       /* 177 261 b1 ":                           */
#define CGRDOT     '\262'       /* 178 262 b2 `.                           */
#define CGRCO      '\263'       /* 179 263 b3 `:                           */
#define CATDOT     '\264'       /* 180 264 b4 @.                           */
#define CATCO      '\265'       /* 181 265 b5 @:                           */
#define CUNDER     '\266'       /* 182 266 b6 &.                           */
#define CAMPCO     '\267'       /* 183 267 b7 &:                           */
#define CQRYDOT    '\270'       /* 184 270 b8 ?.                           */
#define CQRYCO     '\271'       /* 185 271 b9 ?:                           */

#define CALP       '\272'       /* 186 272 ba a.                           */
#define CATOMIC    '\273'       /* 187 273 bb A.                           */
#define CACE       '\274'       /* 188 274 bc a:                           */
#define CBDOT      '\275'       /* 189 275 bd b.                           */
#define CCDOT      '\276'       /* 190 276 be c.                           */
#define CCYCLE     '\300'       /* 192 300 c0 C.                           */
#define CDDOT      '\301'       /* 193 301 c1 d.                           */
#define CDCAP      '\302'       /* 194 302 c2 D.                           */
#define CDCAPCO    '\303'       /* 195 303 c3 D:                           */
#define CEPS       '\304'       /* 196 304 c4 e.                           */
#define CEBAR      '\305'       /* 197 305 c5 E.                           */
#define CFIX       '\306'       /* 198 306 c6 f.                           */
#define CFCAPCO    '\307'       /* 199 307 c7 F:                           */
#define CHGEOM     '\310'       /* 200 310 c8 H.                           */
#define CIOTA      '\311'       /* 201 311 c9 i.                           */
#define CICO       '\312'       /* 202 312 ca i:                           */
#define CICAP      '\313'       /* 203 313 cb I.                           */
#define CICAPCO    '\314'       /* 204 314 cc I:                           */
#define CJDOT      '\315'       /* 205 315 cd j.                           */
#define CLDOT      '\316'       /* 206 316 ce L.                           */
#define CLCAPCO    '\317'       /* 207 317 cf L:                           */
#define CMDOT      '\320'       /* 208 320 d0 m.                           */
#define CMCAP      '\321'       /* 209 321 d1 M.                           */
#define CNDOT      '\322'       /* 210 322 d2 n.                           */
#define CCIRCLE    '\323'       /* 211 323 d3 o.                           */
#define CPOLY      '\324'       /* 212 324 d4 p.                           */
#define CPCO       '\325'       /* 213 325 d5 p:                           */
#define CQCAPCO    '\326'       /* 214 326 d6 Q:                           */
#define CQCO       '\327'       /* 215 327 d7 q:                           */
#define CRDOT      '\330'       /* 216 330 d8 r.                           */
#define CSCO       '\331'       /* 217 331 d9 s:                           */
#define CSCAPCO    '\332'       /* 218 332 da S:                           */
#define CTDOT      '\333'       /* 219 333 db t.                           */
#define CTCO       '\334'       /* 220 334 dc t:                           */
#define CTCAP      '\335'       /* 221 335 dd T.                           */
#define CUDOT      '\336'       /* 222 336 de u.                           */
#define CUCO       '\337'       /* 223 337 df u:                           */
#define CVDOT      '\340'       /* 224 340 e0 v.                           */
#define CXDOT      '\341'       /* 225 341 e1 x.                           */
#define CXCO       '\342'       /* 226 342 e2 x:                           */
#define CYDOT      '\343'       /* 227 343 e3 y.                           */

#define CFCONS     '\350'       /* 232 350 e8 0: 1: 2: etc.                */
#define CAMIP      '\351'       /* 233 351 e9 }   amend in place           */
#define CCASEV     '\352'       /* 234 352 ea }   case in place            */
#define CFETCH     '\353'       /* 235 353 eb {::                          */
#define CMAP       '\354'       /* 236 354 ec {::                          */
#define CEMEND     '\355'       /* 237 355 ed }::                          */
#define CUNDCO     '\356'       /* 238 356 ee &.:                          */
#define CPDERIV    '\357'       /* 239 357 ef p..                          */
#define CAPIP      '\360'       /* 240 360 f0 ,   append in place          */

#define CFF        '\377'       /* 255 377 ff                              */
