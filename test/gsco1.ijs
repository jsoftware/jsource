NB. s: ------------------------------------------------------------------

0 s: 11
data=: 0 s: 10

a=:   ;:'A AAPL AMAT AMD AMZN ATT BA CRA CSCO DELL F GE GM HWP IBM INTC'
a=: a,;:'JDSU LLY LU MOT MSFT NOK NT PFE PG QCOM RMBS T XRX YHOO'
b=: ;:'NY SF LDN TOK HK FF TOR'
c=: ;:'Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec'
d=: <;._1 ' 00 01 02 03 04 05 06 07 08 09'
e=: ;:'open high low close'

t=: }.@;&.>{' ',&.>&.>a;b;c;d;<e

q=: ":&.>?100$1e9

1 [ ":&.>t
1 [ ":&.>q
d=:p=:s=: 911
b0=: 911
b1=: 911
s0=: 7!:3 ''
s1=: 7!:3 ''

s0=: 7!:3 ''
b0=: 7!:0 ''

s=: s: t
t -: 5 s: s
0 s: 11

d=: 0 s: 10
(3!:1 d) 1!:2 <'foo.x'

p=: s: q
q -: 5 s: p
0 s: 11

d -: 3!:2 (1!:1) <'foo.x'
10 s: d
0 s: 11
d -: 0 s: 10
t -: 5 s: s

1!:55 <'foo.x'

10 s: data
0 s: 11

d=:p=:s=: 911
s1=: 7!:3 ''
b1=: 7!:0 ''

b0 -: b1
NB. IF64 +. s0 -: s1


4!:55 ;:'a b b0 b1 c d data e p q s s0 s1 t'


