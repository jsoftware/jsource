NB. ~ timing tests ------------------------------------------------------

ratio =: >./ % <./

test0=: 3 : 0
 xx=: 1e5?@$1e6
 t=: i. 0 2
 t=:t,10 timer '3+ xx',:'3+~xx' 
 t=:t,10 timer '3- xx',:'3-~xx'
 t=:t,10 timer '3* xx',:'3*~xx'
 t=:t,10 timer '3% xx',:'3%~xx'
 t=:t,10 timer 'xx+xx',:'+~xx' 
 t=:t,10 timer 'xx*xx',:'*~xx'
 5 > ratio"1 t
)

test1=: 3 : 0
 i=: ?10
 yy=: 24000 10?@$1e6
 5 > ratio t=: 100 timer 'i{"1 yy',:'yy{~"1 i'
)

test0 ''
test1 ''


4!:55 ;:'i ratio t test0 test1 xx yy'


