prolog './g7x5s.ijs'
NB. 7!:5 (no jmf) -------------------------------------------------------

randuni''

bp=: (IF64{1 1 4 8 16 4 4 2 4,:1 1 8 8 16 8 8 2 4) {~ 1 2 4 8 16 32 65536 131072 262144 i. 3!:0
sp=: 7!:5
f =: 3 : '7!:5 <''y'''
fmapped =: 3 : '7!:5 <''y'''

g =: 3 : 0
 w=. IF64{4 8
 z=. w*0                              NB. 0 words for memory management
 z=. z + w*7                          NB. 7 words for non-shape header words
 z=. z + w*r+(-.IF64)*0=2|r=. #$y     NB. shape, pad to doubleword boundary if 32 bits
 z=. z + ((bp y)**/$y) + (w-1)*(3!:0 y)e. 1 2 131072 262144  NB. atoms & trailing 0 word (uses whole word of padding-1)
 >.&.(2&^.) z
)
gmapped =: 3 : 0  NB. contiguous header only
 w=. IF64{4 8
 z=. w*0                              NB. 0 words for memory management
 z=. z + w*7+64+(-.IF64)                     NB. fixed shape area
 z=. z + >.&.(%&w) ((bp y)**/$y) + (w)*(3!:0 y)e. 1 2 131072 262144  NB. atoms & trailing 0 word (uses whole word of padding-1)
)


(f -: g) 23
(f -: g) 2.3
(f -: g) 2j3
(f -: g) 2j3
(f -: g) 'a'
(f -: g) u:'a'
(f -: g) 10&u:'a'
(f -: g) {.s: ' a'

(f -: g)@($&    0 1 )"0 ]200+i.4 10
(f -: g)@($&    'x' )"0 ]200+i.4 10
(f -: g)@($&(u: 'x'))"0 ]100+i.4 10
(f -: g)@($&(10&u: 'x'))"0 ]100+i.4 10
(f -: g)@($&    0 1 )"0 ]160+i.4 10
(f -: g)@($&    'x' )"0 ]160+i.4 10
(f -: g)@($&(u: 'x'))"0 ] 80+i.4 10
(f -: g)@($&(10&u: 'x'))"0 ] 80+i.4 10

(f -: g)@($&    0 1 )"1 ]1,"0 ] 200+i.4 10
(f -: g)@($&    'x' )"1 ]1,"0 ] 200+i.4 10
(f -: g)@($&(u: 'x'))"1 ]1,"0 ] 100+i.4 10
(f -: g)@($&(10&u: 'x'))"1 ]1,"0 ] 100+i.4 10
(f -: g)@($&    0 1 )"1 ]1,"0 ] 160+i.4 10
(f -: g)@($&    'x' )"1 ]1,"0 ] 160+i.4 10
(f -: g)@($&(u: 'x'))"1 ]1,"0 ]  80+i.4 10
(f -: g)@($&(10&u: 'x'))"1 ]1,"0 ]  80+i.4 10

(f -: g) x=: (?1e4)$1 0
(f -: g) x=: (?1e4)$2 3
(f -: g) x=: (?1e4)$2.3
(f -: g) x=: (?1e4)$2j3
(f -: g) x=: (?1e4)$2j3
(f -: g) x=: (?1e4)$'ab'
(f -: g) x=: (?1e4)$u:'ab'
(f -: g) x=: (?1e4)$10&u:'ab'
(f -: g) x=: (?1e4)$s: ' a b'

(f -: g) x=: (1+?100 100)$1 0
(f -: g) x=: (1+?100 100)$2 3
(f -: g) x=: (1+?100 100)$2.3
(f -: g) x=: (1+?100 100)$2j3
(f -: g) x=: (1+?100 100)$2j3
(f -: g) x=: (1+?100 100)$'ab'
(f -: g) x=: (1+?100 100)$'ab'
(f -: g) x=: (1+?100 100)$10&u:'ab'
(f -: g) x=: (1+?100 100)$s: ' a b'

(f -: g) x=: (1+?100 10 50)$1 0
(f -: g) x=: (1+?100 10 50)$2 3
(f -: g) x=: (1+?100 10 50)$2.3
(f -: g) x=: (1+?100 10 50)$2j3
(f -: g) x=: (1+?100 10 50)$2j3
(f -: g) x=: (1+?100 10 50)$'ab'
(f -: g) x=: (1+?100 10 50)$u:'ab'
(f -: g) x=: (1+?100 10 50)$10&u:'ab'
(f -: g) x=: (1+?100 10 50)$s: ' a b'

(sp ;:'f g sp') -: (sp <'f'),(sp <'g'),sp <'sp'

(sp <'sp') -: sp <'sp_base_'
(sp <'f' ) -: sp <'f__x' [ x=: <'base'

0 <: f 5!:1 <'bp'

x=: 3 : 0
 if. y do. c=.1 else. c=.2 end.
 c
)

0 <: sp <'x'

x=: 4 : 0
 if. x<y do. c=.10 else. c=.20 end.
 s=. 0
 for_xyz. i.c do.
  s=. xyz+s
 end.
)

0 <: sp <'x'

x=: 2 : 0
  :
 select. x 
  case.  1 do. u y
  case.  2 do. u 2*y
  fcase. 3 do. 3*y
  case.  4 do. u 4*y
 end.
)

0 <: sp <'x'
0 < 7!:5 <'x' [ x=: 9e4 $ <2e4$0
0 < 7!:5 <'x' [ x=: 9e4 $ <4e4$0

'value error'     -: 7!:5 etx <'nonexistent'

'domain error'    -: 7!:5 etx <i.4
'domain error'    -: 7!:5 etx <1 2.3 4
'domain error'    -: 7!:5 etx <1 2j3 4
'domain error'    -: 7!:5 etx <10&u:'abc'
'domain error'    -: 7!:5 etx <s: ' a b c'
'domain error'    -: 7!:5 etx <<'abc'
'domain error'    -: 7!:5 etx i.4
'domain error'    -: 7!:5 etx 1 2.3 4
'domain error'    -: 7!:5 etx 1 2j3 4
'domain error'    -: 7!:5 etx u:'abc'
'domain error'    -: 7!:5 etx 10&u:'abc'
'domain error'    -: 7!:5 etx s: ' a b c'

'rank error'      -: 7!:5 etx <,:'abc'

'ill-formed name' -: 7!:5 etx <'bad name'
'ill-formed name' -: 7!:5 etx <''


4!:55 ;:'adot1 adot2 fmapped gmapped sdot0 bp f g sp x '
randfini''


epilog''

