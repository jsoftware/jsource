NB. x{y for sparse x, dense y -------------------------------------------

p=: (?7 3 5$2) * ?7 3 5 4$10
c=: ; (i.1+r) <"1@comb&.>r=:#$p

f=: 4 : '(scheck t) *. (p{"(_,x) q) -: t=. ((2;y)$.p) {"(_,x) q'

1 2 3 f&>/c [ q=: (-1+$q){.q=:    ?  9 10 12$2
1 2 3 f&>/c [ q=: (-1+$q){.q=:    ?  9 10 12$100
1 2 3 f&>/c [ q=: (-1+$q){.q=: o. ?  9 10 12$100
1 2 3 f&>/c [ q=: (-1+$q){.q=: j./?2 9 10 12$100

f=: 3 : '(scheck t) *. ((0*p){"_ 0 q) -: t=. (0*(2;y)$.p) {"_ 0 q'

f&> c [ q=: 1 1 1
f&> c [ q=: 2 3$?2e6
f&> c [ q=: 2 1 3$o.?2e6
f&> c [ q=: 2 1 1 3$j./?2$2e6

'nonce error'  -: ($.i.2 3)        { etx 'abcdef'
'nonce error'  -: ($.i.2 3)        { etx ;:'a b c d e f'
'nonce error'  -: ($.i.2 3)        { etx i.7x
'nonce error'  -: ($.i.2 3)        { etx 9 % 1+i.7x
'nonce error'  -: ($.i.2 3)        { etx s: ' a b c d e f'

'domain error' -: ($.o.i.2 3)      { etx i.9

'index error'  -: ($.i.2 3)        { etx i.4
'index error'  -: ((3;6)$.$.i.2 3) { etx i.6

'non-unique sparse elements' -: ($.i.2 3){     etx i.6 3
'non-unique sparse elements' -: ($.i.2 3){"2 1 etx i.2 6


4!:55 ;:'c f p q r'


