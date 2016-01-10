NB. ;:y -----------------------------------------------------------------

('Cogito';(,',');'ergo';'sum.')     -: ;:'Cogito, ergo sum.'
('Opposable';'Thumbs')              -: ;:'Opposable Thumbs'
('1 2';'+.';'/:';'..';'*.';'4 5 6') -: ;:'1 2+./: ..*.4 5 6'
'' -: ;:''
'' -: ;:'      '

rhet=: 'anaphora anthimeria antithesis cacozelia comprobatio '
rhet=: rhet, 'epiphonema erotema erotesis hypophora metaphor '
rhet=: rhet, 'metonymy occultatio oxymoron polyptoton simile '
rhet=: rhet, 'syllepsis synecdoche tasis'
rhet=: ;:rhet
1 = #$rhet

(,<,t) -: ;: t=.'+'
(,<,t) -: ;: t=.'+.'
(,<,t) -: ;: t=.'+:'
(,<,t) -: ;: t=.'.'
(,<,t) -: ;: t=.'..'
(,<,t) -: ;: t=.'.:'

(,<t-.' ') -: ;: t=.' - '
(,<t-.' ') -: ;: t=.'- '
(,<t-.' ') -: ;: t=.' -'
(,<t-.' ') -: ;: t=.' :'
(,<t-.' ') -: ;: t=.': '
(,<t-.' ') -: ;: t=.' : '

(,&':'&":&.>i.10)  -: ;:'0: 1:2:3: 4: 5: 6: 7: 8:9:'
('2:';'_3 4';'5:') -: ;:'2: _3 4 5:'

(,<,t) -: ;:t=. '3 1.5 _4 _1.23e_45j_6.7e_8 5 _ __ 9 2 6'
(,<,t) -: ;:t=. '+.'
(,<,t) -: ;:t=. 'abc...'
(,<,t) -: ;:t=. 'abc:::'
(,<,t) -: ;:t=. 'abc.:.'
(,<,t) -: ;:t=. 'supercalifragilisticexpialidocious'
(,<,t) -: ;:t=. 'supercalifragilisticexpialidocious.'
(,<,t) -: ;:t=. 'supercalifragilisticexpialidocious:'
(,<,t) -: ;:t=. 'dichloro_diphenol_trichloroethane'
5 = $;:'dichloro-diphenol-trichloroethane'

(,<'9 10')   -: ;:'   9 10  '
(,<'9   10') -: ;:'   9   10   '
('ab';'cd')  -: ;:'   ab     cd '
(,<'0. 2 3') -: ;:'0. 2 3'
(,<'1. 2 3') -: ;:'1. 2 3'
(,<'_. 2 3') -: ;:'_. 2 3'
('0:';'2 3') -: ;:'0: 2 3'
('1:';'2 3') -: ;:'1: 2 3'
('_:';'2 3') -: ;:'_: 2 3'
(,<'2 3 0.') -: ;:'2 3 0.'
(,<'2 3 1.') -: ;:'2 3 1.'
(,<'2 3 _.') -: ;:'2 3 _.'
('2 3';'0:') -: ;:'2 3 0:'
('2 3';'1:') -: ;:'2 3 1:'
('2 3';'_:') -: ;:'2 3 _:'

q =. ''''
(,<,t) -: ;: t =. q,q
(,<,t) -: ;: t =. q,'Ich liebe dich',q
(,<,t) -: ;: t =. q,'Je ',q,q,' te aime',q
(,<,t) -: ;: t =. q,'un deux trois ...',q
(,<,t) -: ;: t =. q,'un 2 trois ...',q

((q,'abc',q);'9 10') -: ;: q,'abc',q,'9 10'
((q,'abc',q);'cd')   -: ;: q,'abc',q,'cd'
((q,'abc',q);,'.')   -: ;: q,'abc',q,'.'
((q,'abc',q);,':')   -: ;: q,'abc',q,':'
('9 10';q,'abc',q)   -: ;: '9 10',q,'abc',q
('cd'  ;q,'abc',q)   -: ;: 'cd'  ,q,'abc',q
((,'.');q,'abc',q)   -: ;: '.'   ,q,'abc',q
((,':');q,'abc',q)   -: ;: ':'   ,q,'abc',q

(,<,t) -: ;: t =. '5'
(,<,t) -: ;: t =. '_5'
(,<,t) -: ;: t =. '_'
(,<,t) -: ;: t =. '__'
(,<,t) -: ;: t =. '.'
(,<,t) -: ;: t =. ':'
(,<,t) -: ;: t =. '+'
(,<,t) -: ;: t =. 'a'

(,<'56.')      -: ;:'56.'
((,'.');'56')  -: ;:'.56'
(,<'_.56')     -: ;:'_.56'
(,<'_0.56')    -: ;:'_0.56'

(,<,t) -: ;: t =. 'Ich_liebe_dich'
(,<,t) -: ;: t =. 'Ich_liebe_dich_'
(,<,t) -: ;: t =. 'Ich_liebe_dich_.'
(,<,t) -: ;: t =. 'Ich_liebe_dich_:'

(,<,t) -: ;: t =. '+.'
(,<,t) -: ;: t =. '+..'
(,<,t) -: ;: t =. '+...'
(,<,t) -: ;: t =. '+:'
(,<,t) -: ;: t =. '+::'
(,<,t) -: ;: t =. '+:::'
(,<,t) -: ;: t =. '+.:.:'
(,<,t) -: ;: t =. 'a.'
(,<,t) -: ;: t =. 'a..'
(,<,t) -: ;: t =. 'a...'
(,<,t) -: ;: t =. 'a:'
(,<,t) -: ;: t =. 'a::'
(,<,t) -: ;: t =. 'a:::'
(,<,t) -: ;: t =. 'a.:.:'
(,<,t) -: ;: t =. '.'
(,<,t) -: ;: t =. '..'
(,<,t) -: ;: t =. '...'
(,<,t) -: ;: t =. ':.:.'
(,<,t) -: ;: t =. ':'
(,<,t) -: ;: t =. '::'
(,<,t) -: ;: t =. ':::'
(,<,t) -: ;: t =. '.:.:'
(,<,t) -: ;: t =. '.:'
(,<,t) -: ;: t =. ':.'

7=3+4 NB. Don't thread on me!
((,&.>'7=3+4'),<'NB. Don''t!') -: ;:'7=3+4 NB. Don''t!'
NB.
'' -: ". 'NB.'
(,<'NB.')  -: ;:'NB.'
(,<'NB. ') -: ;:'NB. '

eq  =. 3 : 0
 :
 f =. 3 : (('a=.',x);('b=.',y);'=/5!:1 ''a'';''b''')
 1
)

(<;._1 -: ;:) ' A. a. a: b. C. c. D. D: E. e. f. H. i. j. m. n. o. p. p.. p: q:'
(<;._1 -: ;:) ' r. t. T. u. v. x. y.'


NB. ;: numeric input ----------------------------------------------------

1 = type 0001 0000
1 = type 1 0 1
1 = type 1.0 0.0 1.0e0
1 = type 1j0 0j0
1 = type 1e_9999j1e_9999

4 = type 2 3 4 5
4 = type 1.5e2 _1.5e2
4 = type 200e_2
4 = type 1e1 1e2 1e3 1e4 1e5 1e6 1e7 1e8 1e9
_4 -: -4
4  -: -_4
45 -: 5+4*10

8 = type 1e20
8 = type 1e999
8 = type _1.23e_4 5
8 = type 8.5j0.0 _7.25e23j0e99
3.5     -: 3+%2
_1      -: >._1.2345
_0.2345 -: r->.r=._1.2345
42      -: 10^.1e42
_42     -: 10^.1e_42

16 = type 0j1
16 = type 0j1e999
NB. 16 = type 1e999j1e999
16 = type 1j1 2j2 1 0 1 2
16 = type _1.23e_4j_5.6e_7 0j4 4j0 8 9 0 1
_1.23e_4 _5.6e_7 -: +. _1.23e_4j_5.6e_7
0 1 _1 -: {:@+."(0) 0 1j1 0j_1
4 5 _6 -: {.@+."(0) 4j1 5 _6j7
3j4    -: 3+0j4
3j4    -: 3+4*0j1
3j4    -: 3+4*%:_1

_  -: <./''
_  -: %0
__ -: >./''
__ -: _4%0
4  =  $ _ 1 __ 3

0.75 _0.75 _0.75 0.75 -: 3r4 3r_4 _3r4 _3r_4
0.75 7.5 75 750       -: 0.75r1 0.75r0.1 0.75r0.01 0.75r0.001

9 3 0j3 _3 0j_3 3     -: 9 3ad0 3ad90 3ad180 3ad270 3ad360
9 3 0j3 _3 0j_3 3     -: 9 3ar0 3ar1.570796326794896 3ar3.141592653589793 3ar4.712388980384689 3ar6.283185307179586
9 3 0j_3 _3 0j3 3     -: 9 3ad0 3ad_90 3ad_180 3ad_270 3ad_360
9 3 0j_3 _3 0j3 3     -: 9 3ar0 3ar_1.570796326794896 3ar_3.141592653589793 3ar_4.712388980384689 3ar_6.283185307179586

9 2p6 3p_7 _4p8 _5p_9 -: 9, 2 3 _4 _5*(o.1)^6 _7 8 _9
9 2x6 3x_7 _4x8 _5x_9 -: 9, 2 3 _4 _5*     ^6 _7 8 _9

dig=.'0123456789abcdefghijklmnopqrstuvwxyz'
9 2b1011 2b_1011      -: 9  11 _11
9 _2b1011 _2b_1011    -: 9  _9 9
9 1p1b10 1p1b.1       -: 9, (o.1),%o.1
9 1x1b10 1x1b.1       -: 9, ^1 _1
0j1b3j4               -: 0j1 #. dig i. '3j4'
3j_4boustrophedonic   -: 3j_4 #. dig i. 'oustrophedonic'
_3r4b_123.45          -: (_3r4^_2) * -_3r4 #. dig i. '12345'


NB. ;: various errors ---------------------------------------------------

'open quote'   -: ;: etx 'don''t'
'open quote'   -: ;: etx ''''

'domain error' -: ;: etx 1 0
'domain error' -: ;: etx 1 2 3
'domain error' -: ;: etx 3.4
'domain error' -: ;: etx 3j4
'domain error' -: ;: etx <'asdf'

'spelling error' -: ex '10:'
'spelling error' -: ex '_10:'
'spelling error' -: ex 'l.'
'spelling error' -: ex 'l.'

'ill-formed number' -: ex '123abc    '
'ill-formed number' -: ex '__abc     '
'ill-formed number' -: ex '123 45a   '
NB. 'ill-formed number' -: ex '123 45e   '
'ill-formed number' -: ex '123 45j   '

'ill-formed number' -: ex '5_        '
'ill-formed number' -: ex '5__       '
'ill-formed number' -: ex '__5       '
'ill-formed number' -: ex '4e5_      '
'ill-formed number' -: ex '4e5__     '
'ill-formed number' -: ex '4e__5     '
'ill-formed number' -: ex '5_e4      '
'ill-formed number' -: ex '5__e4     '
'ill-formed number' -: ex '__5e4     '
'ill-formed number' -: ex '_e        '
'ill-formed number' -: ex '_e5       '
'ill-formed number' -: ex '5e_       '
'ill-formed number' -: ex '5e__4     '
'ill-formed number' -: ex '5e0.5     '
'ill-formed number' -: ex '5e4.0     '
'ill-formed number' -: ex '4ee       '
'ill-formed number' -: ex '4e5e      '
'ill-formed number' -: ex '4e5e6     '
'ill-formed number' -: ex '4ej       '

'ill-formed number' -: ex '2 3r      '
'ill-formed number' -: ex '2 3re     '
'ill-formed number' -: ex '3rr       '
'ill-formed number' -: ex '3rj       '
'ill-formed number' -: ex '3rad      '
'ill-formed number' -: ex '3rar      '
'ill-formed number' -: ex '3rp       '
'ill-formed number' -: ex '3rx       '
'ill-formed number' -: ex '2 3r4r    '
'ill-formed number' -: ex '2 3r4j    '
'ill-formed number' -: ex '2 3r4ad   '
'ill-formed number' -: ex '2 3r4ar   '
'ill-formed number' -: ex '2 3r4p    '
'ill-formed number' -: ex '2 3r4x    '
'ill-formed number' -: ex '2 3rb     '

'ill-formed number' -: ex '4j        '
'ill-formed number' -: ex '4jj       '
'ill-formed number' -: ex '4j5j      '
'ill-formed number' -: ex '4j5j6     '
'ill-formed number' -: ex '4je       '
'ill-formed number' -: ex '4j_e      '
'ill-formed number' -: ex '4j_e5     '
'ill-formed number' -: ex '4j5e_     '
'ill-formed number' -: ex '4j5e0.5   '
'ill-formed number' -: ex '4je5      '
'ill-formed number' -: ex '4je_      '
'ill-formed number' -: ex '5j__4     '
'ill-formed number' -: ex '5j4_      '
'ill-formed number' -: ex '5j4__     '

'ill-formed number' -: ex '_3ad90    '
'ill-formed number' -: ex '2 3ad     '
'ill-formed number' -: ex '2 3ade    '
'ill-formed number' -: ex '3adr      '
'ill-formed number' -: ex '3adj      '
'ill-formed number' -: ex '3adp      '
'ill-formed number' -: ex '3adx      '
'ill-formed number' -: ex '2 3ad4r   '
'ill-formed number' -: ex '2 3ad4j   '
'ill-formed number' -: ex '2 3ad4p   '
'ill-formed number' -: ex '2 3ad4x   '
'ill-formed number' -: ex '2 3ad4b   '

'ill-formed number' -: ex '_3ar90    '
'ill-formed number' -: ex '2 3ar     '
'ill-formed number' -: ex '2 3are    '
'ill-formed number' -: ex '3arr      '
'ill-formed number' -: ex '3arj      '
'ill-formed number' -: ex '3arp      '
'ill-formed number' -: ex '3arx      '
'ill-formed number' -: ex '2 3ar4r   '
'ill-formed number' -: ex '2 3ar4j   '
'ill-formed number' -: ex '2 3ar4p   '
'ill-formed number' -: ex '2 3ar4x   '
'ill-formed number' -: ex '2 3ar4b   '

'ill-formed number' -: ex '_3b       '
'ill-formed number' -: ex '2 3b4.56.7'
'ill-formed number' -: ex '2 3b__56.7'
'ill-formed number' -: ex '2 3b4.56_ '
'ill-formed number' -: ex '2 3eb     '
'ill-formed number' -: ex '2 3rb     '
'ill-formed number' -: ex '2 3jb     '
'ill-formed number' -: ex '2 3adb    '
'ill-formed number' -: ex '2 3arb    '
'ill-formed number' -: ex '2 3pb     '
'ill-formed number' -: ex '2 3xb     '

4!:55 ;:'dig eq q r rhet t'


