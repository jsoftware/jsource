prolog './g200p.ijs'
NB. ^/\ B ---------------------------------------------------------------

(0 0 1 1,: 1 0 1 1) -: ^/\ 0 0 1 1 ,: 0 1 0 1
(20$1) -: ^/\20$1
(20$0 1)   -: ^/\20$0

pow=: 4 : 'x^y'

(^/\"1 -: pow/\"1) #:i.16
(^/\"1 -: pow/\"1) #:i.32

(^/\   -: pow/\  ) x=:0=?    13$4
(^/\   -: pow/\  ) x=:0=?7   13$4
(^/\"1 -: pow/\"1) x
(^/\   -: pow/\  ) x=:0=?3 5 13$4
(^/\"1 -: pow/\"1) x
(^/\"2 -: pow/\"2) x

(^/\   -: pow/\  ) x=:0=?    16$4
(^/\   -: pow/\  ) x=:0=?8   16$4
(^/\"1 -: pow/\"1) x
(^/\   -: pow/\  ) x=:0=?2 4 16$4
(^/\"1 -: pow/\"1) x
(^/\"2 -: pow/\"2) x

(,'j')    -: ^/\'j'
(,<'ace') -: ^/\<'ace'
(,'j')    -: ^/\u:'j'
(,<'ace') -: ^/\<u:'ace'
(,'j')    -: ^/\10&u:'j'
(,<'ace') -: ^/\<10&u:'ace'
(,s:@<"0 'j')    -: ^/\s:@<"0 'j'
(,s:@<"0&.> <'ace') -: ^/\s:@<"0&.> <'ace'
(,<"0@s: <'ace') -: ^/\<"0@s: <'ace'

'domain error' -: ^/\ etx 'triskaidekaphobia'
'domain error' -: ^/\ etx ;:'ex cathedra'
'domain error' -: ^/\ etx u:'triskaidekaphobia'
'domain error' -: ^/\ etx u:&.> ;:'ex cathedra'
'domain error' -: ^/\ etx 10&u:'triskaidekaphobia'
'domain error' -: ^/\ etx 10&u:&.> ;:'ex cathedra'
'domain error' -: ^/\ etx s:@<"0 'triskaidekaphobia'
'domain error' -: ^/\ etx s:@<"0&.> ;:'ex cathedra'
'domain error' -: ^/\ etx <"0@s: ;:'ex cathedra'

4!:55 ;:'pow x'



epilog''

