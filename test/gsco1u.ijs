prolog './gsco1u.ijs'
NB. s: unicode ------------------------------------------------------------------

0 s: 11
data=: 0 s: 10

a=:   ;:'A AAPL AMAT AMD AMZN ATT BA CRA CSCO DELL F GE GM HWP IBM INTC'
a=: ([: 10&u: 65536+a.&i.)&.> a,;:'JDSU LLY LU MOT MSFT NOK NT PFE PG QCOM RMBS T XRX YHOO'
b=: ([: 10&u: 65536+a.&i.)&.> ;:'NY SF LDN TOK HK FF TOR'
c=: ([: 10&u: 65536+a.&i.)&.> ;:'Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec'
d=: ([: 10&u: 65536+a.&i.)&.> <;._1 ' 00 01 02 03 04 05 06 07 08 09'
e=: ([: 10&u: 65536+a.&i.)&.> ;:'open high low close'

1 [ 9!:57 (1)  NB. disable auditing, since next line is slow
t=: }.@;&.>{' ',&.>&.>a;b;c;d;<e
1 [ 9!:57 (2)

q=: ":&.>?100$1e9

1 [ 9!:57 (1)  NB. disable auditing, since next line is slow
1 [ ":&.>t
1 [ 9!:57 (2)
1 [ ":&.>q
d=:p=:s=: 911
b0=: 911
b1=: 911
s0=: 7!:3 ''
s1=: 7!:3 ''

IGNOREIFFVI 1  NB. allow garbage collection to run
s0=: 7!:3 ''
b0=: 7!:0 ''

s=: s: t
t -: 5 s: s

d=: 0 s: 10
(3!:1 d) 1!:2 <jpath '~temp/foo.x'

p=: s: q
q -: 5 s: p

d -: 3!:2 (1!:1) <jpath '~temp/foo.x'
10 s: d
0 s: 11
d -: 0 s: 10
t -: 5 s: s

1!:55 <jpath '~temp/foo.x'

10 s: data
0 s: 11
 
d=:p=:s=: 911
IGNOREIFFVI 1  NB. allow garbage collection to run
s1=: 7!:3 ''
b1=: 7!:0 ''

(b0 + 2000) > b1
NB. IF64 +. s0 -: s1

0 s: 11




epilog''

