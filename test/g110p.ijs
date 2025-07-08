prolog './g110p.ijs'
NB. */\ B ---------------------------------------------------------------

(0 0 1 1 ,: 0 0 0 1) -: */\ 0 0 1 1 ,: 0 1 0 1
(20$1) -: */\20$1
(20$0) -: */\20$0

times=: 4 : 'x*y'

(*/\   -: times/\  ) x=:?    13$2
(*/\   -: times/\  ) x=:?7   13$2
(*/\"1 -: times/\"1) x
(*/\   -: times/\  ) x=:?3 5 13$2
(*/\"1 -: times/\"1) x
(*/\"2 -: times/\"2) x
(*/\   -: times/\  ) x=:?    12$2
(*/\   -: times/\  ) x=:?4   12$2
(*/\"1 -: times/\"1) x
(*/\   -: times/\  ) x=:?4 8 12$2
(*/\"1 -: times/\"1) x
(*/\"2 -: times/\"2) x


NB. */\ I ---------------------------------------------------------------

times=: 4 : 'x*y'

(*/\ -: times/\) x=:1 2 3 1e9 2e9
(*/\ -: times/\) |.x

(*/\   -: times/\  ) x=:_10+?    7$20
(*/\   -: times/\  ) x=:_10+?4   7$20
(*/\"1 -: times/\"1) x
(*/\   -: times/\  ) x=:_10+?3 5 7$20
(*/\"1 -: times/\"1) x
(*/\"2 -: times/\"2) x

(*/\   -: times/\  ) x=:_1e9+?    13$2e9
(*/\   -: times/\  ) x=:_1e9+?4   13$2e9
(*/\"1 -: times/\"1) x
(*/\   -: times/\  ) x=:_1e9+?3 5 13$2e9
(*/\"1 -: times/\"1) x
(*/\"2 -: times/\"2) x


NB. */\ D ---------------------------------------------------------------

times=: 4 : 'x*y'

(*/\   -: times/\  ) x=:0.1*_1e2+?    13$2e2
(*/\   -: times/\  ) x=:0.1*_1e2+?4   13$2e2
(*/\"1 -: times/\"1) x
(*/\   -: times/\  ) x=:0.1*_1e2+?3 5 13$2e2
(*/\"1 -: times/\"1) x
(*/\"2 -: times/\"2) x


NB. */\. Z ---------------------------------------------------------------

times=: 4 : 'x*y'

(*/\   -: times/\  ) x=:j./0.1*_1e2+?2     13$2e2
(*/\   -: times/\  ) x=:j./0.1*_1e2+?2 4   13$2e2
(*/\"1 -: times/\"1) x
(*/\   -: times/\  ) x=:j./0.1*_1e2+?2 3 5 13$2e2
(*/\"1 -: times/\"1) x
(*/\"2 -: times/\"2) x

(,'j')    -: */\'j'
(,<'ace') -: */\<'ace'
(,'j')    -: */\u:'j'
(,<'ace') -: */\<u:'ace'
(,'j')    -: */\10&u:'j'
(,<'ace') -: */\<10&u:'ace'
(,s:@<"0 'j')    -: */\s:@<"0 'j'
(,s:@<"0&.> <'ace') -: */\s:@<"0&.> <'ace'
(,<"0@s: <'ace') -: */\<"0@s: <'ace'

'domain error' -: */\ etx 'deipnosophist'
'domain error' -: */\ etx ;:'peace in our time'
'domain error' -: */\ etx u:'deipnosophist'
'domain error' -: */\ etx u:&.> ;:'peace in our time'
'domain error' -: */\ etx 10&u:'deipnosophist'
'domain error' -: */\ etx 10&u:&.> ;:'peace in our time'
'domain error' -: */\ etx s:@<"0 'deipnosophist'
'domain error' -: */\ etx s:@<"0&.> ;:'peace in our time'
'domain error' -: */\ etx <"0@s: ;:'peace in our time'




epilog''

