prolog './g421p.ijs'
NB.  x +//.@(*/) y special code -----------------------------------------

NB. test failed on small ct
ct   =: 9!:18''
9!:19[5e_11

test=: 4 : 0
 xx=: x{~53 ?@$ #x
 yy=: y{~61 ?@$ #y
 assert. (xx +//.@(*/) yy) Nearmt +//. xx */ yy
 assert. (yy +//.@(*/) xx) Nearmt +//. yy */ xx
 1
)

test&>/~ 0 1;(_500+?@$~1000);(100 ?@$ 0); j./2 100 ?@$ 0


9!:19 ct




epilog''

