NB. x #/. y -------------------------------------------------------------

tally=: 3 : '#y'

test=: 3 : 0
 assert. (#/.~ -: tally/.~) xx=: y{~?1000  $#y
 assert. (#/.~ -: tally/.~) xx=: y{~ 1000  $0
 assert. (#/.~ -: tally/.~) xx=: y{~    0  $#y
 assert. (#/.~ -: tally/.~) xx=: y{~?1000 2$#y
 assert. (#/.~ -: tally/.~) xx=: y{~?1000 5$#y
 assert. (#/.~ -: tally/.~) xx=: y{~?1000 5$#y
 assert. (#/.~ -: tally/.~) xx=: y{~ 1000 5$0
 assert. (#/.~ -: tally/.~) xx=: y{~    0 5$#y
 assert. (#/.~ -: tally/.~) xx=: 0#y
 assert. (#/.~ -: tally/.~) xx=: '' ($,)y
 1
)

test 0 1
test 1 0
test a.
test      i.900
test _450+i.900
test  1e6+i.900
test _1e6+i.900
test u: 100 ?@$ 65536
test o.i.900
test o._450+i.900
test <"0 ?40$100


'length error' -: 1 2 3 #/. etx i.4 


4!:55 ;:'tally test x'

