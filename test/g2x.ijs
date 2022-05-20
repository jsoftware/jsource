prolog './g2x.ijs'
NB. 2!:x ----------------------------------------------------------------

errnox =: 2!:8

'file name error' -: 1!:1 etx <'qifwbsxjntzrmupkovalycehdg/b/c/d/e'
2 -: >{.errnox ''

1:@(1!:0) jpath '~temp'
2 -: >{.errnox ''

'length error' -: errnox etx 1 2 3
'length error' -: errnox etx 1.2 3
'length error' -: errnox etx 1j2 3
'length error' -: errnox etx 'abc'
'rank error' -: errnox etx <1 2

4!:55 ;:'errnox'



epilog''

