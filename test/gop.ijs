prolog './gop.ijs'
NB. o./\ B --------------------------------------------------------------

cir=: 4 : 'x o. y'

(o./\"1 -: cir/\"1) #:i.16

(o./\   -: cir/\  ) x=:0<?    13$4
(o./\   -: cir/\  ) x=:0<?7   13$4
(o./\"1 -: cir/\"1) x
(o./\   -: cir/\  ) x=:0<?3 5 13$4
(o./\"1 -: cir/\"1) x
(o./\"2 -: cir/\"2) x

(o./\   -: cir/\  ) x=:0<?    16$4
(o./\   -: cir/\  ) x=:0<?8   16$4
(o./\"1 -: cir/\"1) x
(o./\   -: cir/\  ) x=:0<?2 4 16$4
(o./\"1 -: cir/\"1) x
(o./\"2 -: cir/\"2) x

(,'j')    -: o./\'j'
(,<'ace') -: o./\<'ace'

'domain error' -: o./\ etx 'sesquipedalian'
'domain error' -: o./\ etx ;:'super cali fragi listic espi ali do cious'
'domain error' -: o./\ etx u:'sesquipedalian'
'domain error' -: o./\ etx u:&.> ;:'super cali fragi listic espi ali do cious'
'domain error' -: o./\ etx 10&u:'sesquipedalian'
'domain error' -: o./\ etx 10&u:&.> ;:'super cali fragi listic espi ali do cious'
'domain error' -: o./\ etx s:@<"0 'sesquipedalian'
'domain error' -: o./\ etx s:@<"0&.> ;:'super cali fragi listic espi ali do cious'
'domain error' -: o./\ etx <"0@s: ;:'super cali fragi listic espi ali do cious'




epilog''

