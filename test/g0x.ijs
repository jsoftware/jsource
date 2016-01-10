NB. 0!: -----------------------------------------------------------------

sc00 =: 0!:0

lf      =: 10{a.
mtv     =: ''
mtm     =: i.0 0

mtm -: sc00 '3+4',lf,'#''Cogito, ergo sum.'''
mtm -: sc00 'i.2 3 4',lf,'3+4'
mtm -: sc00 'i.2 2 2 2 2 2',lf,';:''Cogito, ergo sum.'''
mtm -: sc00 ''
mtm -: sc00 20$' '

l0  =: 'i.20',lf,'mtm -: sc00 l1',lf,'i.20'
l1  =: 'i.21',lf,'mtm -: sc00 l2',lf,'i.21'
l2  =: 'i.22'
mtm -: sc00 l0

t=:,('abc',.'=:',"1 ":x=:?3 2000$10000),"1 lf
mtm -: sc00 t
a -: 0{x
b -: 1{x
c -: 2{x

1 [ 0!:0 'x=: ''',(1050$'x'),''''

1 [ 0!:100 'a=: 12345',:'b=: i.3 4'
a -: 12345
b -: i.3 4

'domain error' -: 0!: 0 etx 0
'domain error' -: 0!: 0 etx 2
'domain error' -: 0!: 1 etx 0
'domain error' -: 0!: 1 etx 2
'domain error' -: 0!:10 etx 0
'domain error' -: 0!:10 etx 2
'domain error' -: 0!:11 etx 0
'domain error' -: 0!:11 etx 2
'domain error' -: 0!:11 etx 0
'domain error' -: 0!:11 etx 2

4!:55 ;:'a b c l0 '
4!:55 ;:'l1 l2 lf mtm mtv s sc00 t x '


