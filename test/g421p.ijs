prolog './g421p.ijs'
NB.  x +//.@(*/) y special code -----------------------------------------

NB. test failed on small ct
ct   =: 9!:18''
9!:19[2^_40

test=: 4 : 0
 xx=: x{~53 ?@$ #x
 yy=: y{~61 ?@$ #y
 assert. (xx +//.@(*/) yy) -: +//. xx */ yy
 assert. (yy +//.@(*/) xx) -: +//. yy */ xx
 1
)

test&>/~ 0 1;(_500+?@$~1000);(100 ?@$ 0); j./2 100 ?@$ 0


9!:19 ct

4!:55 ;:'ct test xx yy'



epilog''

