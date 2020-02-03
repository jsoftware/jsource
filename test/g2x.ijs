1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g2x.ijs'
NB. 2!:x ----------------------------------------------------------------

errnox =: 2!:8

'file name error' -: 1!:1 etx <'qifwbsxjntzrmupkovalycehdg/b/c/d/e'
(2;'No such file or directory') -: errnox ''

1:@(1!:0) jpath '~temp'
(2;'No such file or directory') -: errnox ''

'length error' -: errnox etx 1 2 3
'length error' -: errnox etx 1.2 3
'length error' -: errnox etx 1j2 3
'length error' -: errnox etx 'abc'
'rank error' -: errnox etx <1 2

4!:55 ;:'errnox'


