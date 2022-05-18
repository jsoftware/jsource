prolog './g410p.ijs'
NB. !/\ B ---------------------------------------------------------------

(0 0 1 1,: 1 1 0 1) -: !/\ 0 0 1 1 ,: 0 1 0 1
(20$ 1 ) -: !/\20$1
(0<i.20) -: !/\20$0

bang=: 4 : 'x!y'

(!/\"1 -: bang/\"1) #:i.16
(!/\"1 -: bang/\"1) #:i.32

(!/\   -: bang/\  ) x=:0<?    13$4
(!/\   -: bang/\  ) x=:0<?7   13$4
(!/\"1 -: bang/\"1) x
(!/\   -: bang/\  ) x=:0<?3 5 13$4
(!/\"1 -: bang/\"1) x
(!/\"2 -: bang/\"2) x

(!/\   -: bang/\  ) x=:0<?    16$4
(!/\   -: bang/\  ) x=:0<?8   16$4
(!/\"1 -: bang/\"1) x
(!/\   -: bang/\  ) x=:0<?2 4 16$4
(!/\"1 -: bang/\"1) x
(!/\"2 -: bang/\"2) x

(,'j')    -: !/\'j'
(,<'ace') -: !/\<'ace'

'domain error' -: !/\ etx 'sesquipedalian'
'domain error' -: !/\ etx ;:'super cali fragi listic espi ali do cious'
'domain error' -: !/\ etx u:'sesquipedalian'
'domain error' -: !/\ etx u:&.> ;:'super cali fragi listic espi ali do cious'
'domain error' -: !/\ etx 10&u:'sesquipedalian'
'domain error' -: !/\ etx 10&u:&.> ;:'super cali fragi listic espi ali do cious'
'domain error' -: !/\ etx s:@<"0 'sesquipedalian'
'domain error' -: !/\ etx s:@<"0&.> ;:'super cali fragi listic espi ali do cious'
'domain error' -: !/\ etx <"0@s: ;:'super cali fragi listic espi ali do cious'

4!:55 ;:'bang x'



epilog''

