prolog './g011p.ijs'
randuni''

NB. <./\ B ---------------------------------------------------------------

(0 0 1 1 ,: 0 0 0 1) -: <./\ 0 0 1 1 ,: 0 1 0 1
(20$1) -: <./\20$1
(20$0) -: <./\20$0

min=: 4 : 'x<.y'

(<./\   -: min/\  ) x=:?    13$2
(<./\   -: min/\  ) x=:?7   13$2
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:?3 5 13$2
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x
(<./\   -: min/\  ) x=:?    12$2
(<./\   -: min/\  ) x=:?4   12$2
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:?4 8 12$2
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x


NB. <./\ I ---------------------------------------------------------------

min=: 4 : 'x<.y'

(<./\ -: min/\) x=:1 2 3 1e9 2e9
(<./\ -: min/\) |.x

(<./\   -: min/\  ) x=:_1e4+?    13$2e4
(<./\   -: min/\  ) x=:_1e4+?4   13$2e4
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:_1e4+?3 5 13$2e4
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x

(<./\   -: min/\  ) x=:_1e9+?    13$2e9
(<./\   -: min/\  ) x=:_1e9+?4   13$2e9
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:_1e9+?3 5 13$2e9
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x


NB. <./\ D ---------------------------------------------------------------

min=: 4 : 'x<.y'

(<./\   -: min/\  ) x=:0.01*_1e4+?    13$2e4
(<./\   -: min/\  ) x=:0.01*_1e4+?4   13$2e4
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:0.01*_1e4+?3 5 13$2e4
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x


NB. <./\. Z ---------------------------------------------------------------

min=: 4 : 'x<.y'

(<./\   -: min/\  ) x=:[&.j. 0.1*_1e2+?    13$2e2
(<./\   -: min/\  ) x=:[&.j. 0.1*_1e2+?4   13$2e2
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:[&.j. 0.1*_1e2+?3 5 13$2e2
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x

NB. <./\ SB ---------------------------------------------------------------

min=: 4 : 'x<.y'

(<./\ -: min/\) x=:sdot0
(<./\ -: min/\) |.x

(<./\   -: min/\  ) x=:sdot0{~ ?    13$#sdot0
(<./\   -: min/\  ) x=:sdot0{~ ?4   13$#sdot0
(<./\"1 -: min/\"1) x
(<./\   -: min/\  ) x=:sdot0{~ ?3 5 13$#sdot0
(<./\"1 -: min/\"1) x
(<./\"2 -: min/\"2) x


(,'j')    -: <./\'j'
(,<'ace') -: <./\<'ace'
(,'j')    -: <./\u:'j'
(,<'ace') -: <./\<u:'ace'
(,'j')    -: <./\10&u:'j'
(,<'ace') -: <./\<10&u:'ace'
(,s:@<"0 'j')    -: <./\s:@<"0 'j'
(,s:@<"0&.> <'ace') -: <./\s:@<"0&.> <'ace'
(,<"0@s: <'ace') -: <./\<"0@s: <'ace'

'domain error' -: <./\ etx 'deipnosophist'
'domain error' -: <./\ etx ;:'peace in our time'
'domain error' -: <./\ etx u:'deipnosophist'
'domain error' -: <./\ etx u:&.> ;:'peace in our time'
'domain error' -: <./\ etx 10&u:'deipnosophist'
'domain error' -: <./\ etx 10&u:&.> ;:'peace in our time'

randfini''


epilog''

