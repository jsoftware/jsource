prolog './g111p.ijs'
NB. *./\ B --------------------------------------------------------------

(0 0 1 1,:0 0 0 1) -: *./\ 0 0 1 1 ,: 0 1 0 1
(20$1) -: *./\20$1
(20$0) -: *./\20$0

and=: 4 : 'x*.y'

(*./\"1 -: and/\"1) #:i.16
(*./\"1 -: and/\"1) #:i.32

(*./\   -: and/\  ) x=:?    13$2
(*./\   -: and/\  ) x=:?7   13$2
(*./\"1 -: and/\"1) x
(*./\   -: and/\  ) x=:?3 5 13$2
(*./\"1 -: and/\"1) x
(*./\"2 -: and/\"2) x

(*./\   -: and/\  ) x=:?    16$2
(*./\   -: and/\  ) x=:?8   16$2
(*./\"1 -: and/\"1) x
(*./\   -: and/\  ) x=:?2 4 16$2
(*./\"1 -: and/\"1) x
(*./\"2 -: and/\"2) x

(,'j')    -: *./\'j'
(,<'ace') -: *./\<'ace'
(,'j')    -: *./\u:'j'
(,<'ace') -: *./\<u:'ace'
(,'j')    -: *./\10&u:'j'
(,<'ace') -: *./\<10&u:'ace'
(,s:@<"0 'j')    -: *./\s:@<"0 'j'
(,s:@<"0&.> <'ace') -: *./\s:@<"0&.> <'ace'
(,<"0@s: <'ace') -: *./\<"0@s: <'ace'

'domain error' -: *./\ etx 'deipnosophist'
'domain error' -: *./\ etx ;:'peace in our time'
'domain error' -: *./\ etx u:'deipnosophist'
'domain error' -: *./\ etx u:&.> ;:'peace in our time'
'domain error' -: *./\ etx 10&u:'deipnosophist'
'domain error' -: *./\ etx 10&u:&.> ;:'peace in our time'
'domain error' -: *./\ etx s:@<"0 'deipnosophist'
'domain error' -: *./\ etx s:@<"0&.> ;:'peace in our time'
'domain error' -: *./\ etx <"0@s: ;:'peace in our time'

4!:55 ;:'and x'



epilog''

