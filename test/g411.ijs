prolog './g411.ijs'
NB. ":!.n ---------------------------------------------------------------

randuni''

'1.23457'       -: ":x=:1.23456789012345
'1.23457'       -: ":!.6 x
'1.23456789012' -: ":!.12 x
'123456789'     -: ":y=:123456789
'123456789'     -: ":!.4 y

'domain error' -: ex '":!.''8'''
'domain error' -: ex '":!.0    '
'domain error' -: ex '":!.0.7  '
'domain error' -: ex '":!.(<5) '
'rank error'   -: ex '":!.12 13'
'limit error'  -: ex '":!.67   '


NB. ^!.n ----------------------------------------------------------------

k=:0.25*_50+?100
x=:0.25*_50+?100
f =: 4 : '*/x+(i.y)*k' " 0

x (^!.k -: f) 0
x (^!.k -: f) 1
x (^!.k -: f) 2
x (^!.k -: f) 3

k=:0.25*_50+?   100
x=:0.25*_50+?13$100 
x (^!.k/ -: f/) n=:?7$20

'domain error' -: ex '^!.''8'''
'domain error' -: ex '^!.(<8) '


NB. p.^!.n --------------------------------------------------------------

k =:0.01*_100+?200
f =: 4 : '+/x*y^!.k i.#x' " 1 0
p =: _10+?5$20
p (p.!.k Nearmt f) x=:?3 5$10

'domain error' -: ex 'p.!.''8'''
'domain error' -: ex 'p.!.(<8) '

(4 5$!.''   x) -: 4 5$x,(20-#x)$0   [ x=:?7$1000
(4 5$!.9    x) -: 4 5$x,(20-#x)$9
(4 5$!.9.5  x) -: 4 5$x,(20-#x)$9.5
(4 5$!.9    x) -: 4 5$x,(20-#x)$9   [ x=:r.?12$111
(4 5$!.9    x) -: 4 5$x=:?22$1000
(4 5$!.''   x) -: 4 5$x,(20-#x)$' ' [ x=:'kakistocracy'
(4 5$!.'q'  x) -: 4 5$x,(20-#x)$'q'
(4 5$!.''   x) -: 4 5$x,(20-#x)$' ' [ x=:u:'kakistocracy'
(4 5$!.'q'  x) -: 4 5$x,(20-#x)$'q'
(4 5$!.''   x) -: 4 5$x,(20-#x)$' ' [ x=:10&u:'kakistocracy'
(4 5$!.'q'  x) -: 4 5$x,(20-#x)$'q'
(4 5$!.''   x) -: 4 5$x,(20-#x)$({.s:'') [ x=:s:@<"0 'kakistocracy'
(4 5$!.(s:@<"0 'q')  x) -: 4 5$x,(20-#x)$s:@<"0 'q'
(4 5$!.(<3) x) -: 4 5$x,(20-#x)$<3  [ x=:;:'anti dis establish ment arian ism'
(4 5$!.(<3) x) -: 4 5$x,(20-#x)$<3  [ x=:(u:&.>) ;:'anti dis establish ment arian ism'
(4 5$!.(<3) x) -: 4 5$x,(20-#x)$<3  [ x=:(10&u:&.>) ;:'anti dis establish ment arian ism'
(4 5$!.(<3) x) -: 4 5$x,(20-#x)$<3  [ x=:s:@<"0&.> ;:'anti dis establish ment arian ism'
(4 5$!.(<3) x) -: 4 5$x,(20-#x)$<3  [ x=:<"0@s: ;:'anti dis establish ment arian ism'

'domain error' -: 3 4 $!.'a' etx 4 5 6
'domain error' -: 3 4 $!.'a' etx 4;5;6
'domain error' -: 3 4 $!.4   etx 'abc'
'domain error' -: 3 4 $!.4   etx u:'abc'
'domain error' -: 3 4 $!.4   etx 10&u:'abc'
'domain error' -: 3 4 $!.4   etx s:@<"0 'abc'
'domain error' -: 3 4 $!.4   etx <"0 i.4
'domain error' -: 3 4 $!.(<4)etx 'abc'
'domain error' -: 3 4 $!.(<4)etx u:'abc'
'domain error' -: 3 4 $!.(<4)etx 10&u:'abc'
'domain error' -: 3 4 $!.(<4)etx s:@<"0 'abc'
'domain error' -: 3 4 $!.(<4)etx 4 5 6

'rank error'   -: ex '$!.(i.2 3) '
'length error' -: ex '$!.1 2     '
'length error' -: ex '$!.(,''a'')'


NB. #!.n ----------------------------------------------------------------

f =: 4 : '(+.x) #&, y,.k'

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:?10$1000  [ k=:123
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:?10$1000  [ k=:0
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:?10$1000  [ k=:2.3
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:?10$1000  [ k=:2j3
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:o.?10$100 [ k=:0

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:a.{~?10$#a. [ k=:'x' 
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:a.{~?10$#a. [ k=:' ' 

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:adot1{~?10$#adot1 [ k=:u:'x' 
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:adot1{~?10$#adot1 [ k=:u:' ' 

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:adot2{~?10$#adot2 [ k=:10&u:'x' 
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:adot2{~?10$#adot2 [ k=:10&u:' ' 

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:sdot0{~?10$#sdot0 [ k=:s:@<"0 'x' 
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:sdot0{~?10$#sdot0 [ k=:s:@<"0 ' ' 

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$;:'on a clear day' [ k=:<'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$;:'on a clear day' [ k=:<$0

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$(u:&.>) ;:'on a clear day' [ k=:<u:'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$(u:&.>) ;:'on a clear day' [ k=:<$0

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$(10&u:&.>) ;:'on a clear day' [ k=:<10&u:'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$(10&u:&.>) ;:'on a clear day' [ k=:<$0

(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$s:@<"0&.> ;:'on a clear day' [ k=:s:@<"0&.> <'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$<"0@s: ;:'on a clear day' [ k=:s:@<"0&.> <'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$s:@<"0&.> ;:'on a clear day' [ k=:<"0@s: <'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$<"0@s: ;:'on a clear day' [ k=:<"0@s: <'see forever'
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$s:@<"0&.> ;:'on a clear day' [ k=:s:@<"0&.> <$0
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$<"0@s: ;:'on a clear day' [ k=:s:@<"0&.> <$0
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$s:@<"0&.> ;:'on a clear day' [ k=:<"0@s: <$0
(x f y) -: x #!.k y [ x=:j./?2 10$20 [ y=:10$<"0@s: ;:'on a clear day' [ k=:<"0@s: <$0

'domain error' -: 3j4 5 #!.'a' etx 4 5
'domain error' -: 3j4 5 #!.(u:'a') etx 4 5
'domain error' -: 3j4 5 #!.(10&u:'a') etx 4 5
'domain error' -: 3j4 5 #!.(s:@<"0 'a') etx 4 5
'domain error' -: 3j4 5 #!.'a' etx 4;5
'domain error' -: 3j4 5 #!.4   etx 'ab'
'domain error' -: 3j4 5 #!.4   etx u:'ab'
'domain error' -: 3j4 5 #!.4   etx 10&u:'ab'
'domain error' -: 3j4 5 #!.4   etx s:@<"0 'ab'
'domain error' -: 3j4 5 #!.4   etx 4;5
'domain error' -: 3j4 5 #!.(<4)etx 'ab'
'domain error' -: 3j4 5 #!.(<4)etx u:'ab'
'domain error' -: 3j4 5 #!.(<4)etx 10&u:'ab'
'domain error' -: 3j4 5 #!.(<4)etx s:@<"0 'ab'
'domain error' -: 3j4 5 #!.(<4)etx 4 5

'rank error'   -: ex '#!.(i.2 3) '
'rank error'   -: ex '#!.($0)    '
'rank error' -: ex '#!.1 2     '
'rank error' -: ex '#!.(,''a'')'


NB. {.!.n ---------------------------------------------------------------

(20{.!.0    x) -: x,(20-#x)$0   [ x=:?7$1000
(20{.!.19   x) -: x,(20-#x)$19  [ x=:?7$1000
(20{.!.1.9  x) -: x,(20-#x)$1.9 [ x=:?7$1000
(20{.!.1j9  x) -: x,(20-#x)$1j9 [ x=:?7$1000
(20{.!.9    x) -: 20$x=:?25$1000
(20{.!.'x'  x) -: x,(20-#x)$'x' [ x=:'chthonic'
(20{.!.' '  x) -: x,(20-#x)$' ' 
(20{.!.'x'  x) -: x,(20-#x)$'x' [ x=:u:'chthonic'
(20{.!.' '  x) -: x,(20-#x)$' ' 
(20{.!.'x'  x) -: x,(20-#x)$'x' [ x=:10&u:'chthonic'
(20{.!.' '  x) -: x,(20-#x)$' ' 
(20{.!.(s:@<"0 'x')  x) -: x,(20-#x)$(s:@<"0 'x') [ x=:s:@<"0 'chthonic'
(20{.!.(s:@<"0 ' ')  x) -: x,(20-#x)$(s:@<"0 ' ') 
(20{.!.'x'  x) -: 20$x          [ x=:27$'rapport'
(20{.!.'x'  x) -: 20$x          [ x=:27$u:'rapport'
(20{.!.'x'  x) -: 20$x          [ x=:27$10&u:'rapport'
(20{.!.(s:@<"0 'x')  x) -: 20$x          [ x=:27$s:@<"0 'rapport'
(20{.!.(<12)x) -: x,(20-#x)$<12 [ x=:;:'Cogito, ergo sum.'
(20{.!.a:   x) -: x,(20-#x)$<$0 
(20{.!.(<12)x) -: x,(20-#x)$<12 [ x=:(u:&.>) ;:'Cogito, ergo sum.'
(20{.!.a:   x) -: x,(20-#x)$<$0 
(20{.!.(<12)x) -: x,(20-#x)$<12 [ x=:(10&u:&.>) ;:'Cogito, ergo sum.'
(20{.!.a:   x) -: x,(20-#x)$<$0 
(20{.!.(<12)x) -: x,(20-#x)$<12 [ x=:s:@<"0&.> ;:'Cogito, ergo sum.'
(20{.!.(<12)x) -: x,(20-#x)$<12 [ x=:<"0@s: ;:'Cogito, ergo sum.'
(20{.!.a:   x) -: x,(20-#x)$<$0 
(20{.!.(<12)x) -: 20$x          [ x=:27$;:'Cogito, ergo sum.'
(20{.!.(<12)x) -: 20$x          [ x=:27$(u:&.>) ;:'Cogito, ergo sum.'
(20{.!.(<12)x) -: 20$x          [ x=:27$(10&u:&.>) ;:'Cogito, ergo sum.'
(20{.!.(<12)x) -: 20$x          [ x=:27$s:@<"0&.> ;:'Cogito, ergo sum.'
(20{.!.(<12)x) -: 20$x          [ x=:27$<"0@s: ;:'Cogito, ergo sum.'

'domain error' -: 34 {.!.'a' etx 4 5 6
'domain error' -: 34 {.!.(u:'a') etx 4 5 6
'domain error' -: 34 {.!.(10&u:'a') etx 4 5 6
'domain error' -: 34 {.!.(s:@<"0 'a') etx 4 5 6
'domain error' -: 34 {.!.'a' etx 4;5;6
'domain error' -: 34 {.!.4   etx 'abc'
'domain error' -: 34 {.!.4   etx u:'abc'
'domain error' -: 34 {.!.4   etx 10&u:'abc'
'domain error' -: 34 {.!.4   etx s:@<"0 'abc'
'domain error' -: 34 {.!.4   etx <"0 i.4
'domain error' -: 34 {.!.(<4)etx 'abc'
'domain error' -: 34 {.!.(<4)etx u:'abc'
'domain error' -: 34 {.!.(<4)etx 10&u:'abc'
'domain error' -: 34 {.!.(<4)etx s:@<"0 'abc'
'domain error' -: 34 {.!.(<4)etx 4 5 6

'rank error'   -: ex '{.!.(i.2 3) '
'rank error'   -: ex '{.!.1 2     '
'rank error'   -: ex '{.!.(,''a'')'


NB. |.!.n ---------------------------------------------------------------

(2|.!.''   x) -: 2}.x,2$0   [ x=:?7$1000
(2|.!.19   x) -: 2}.x,2$19  [ x=:?7$1000
(2|.!.1.9  x) -: 2}.x,2$1.9 [ x=:?7$1000
(2|.!.1j9  x) -: 2}.x,2$1j9 [ x=:?7$1000
(0|.!.9    x) -: x=:?25$1000
(2|.!.'x'  x) -: 2}.x,2$'x' [ x=:'chthonic'
(2|.!.''   x) -: 2}.x,2$' ' 
(2|.!.'x'  x) -: 2}.x,2$'x' [ x=:u:'chthonic'
(2|.!.''   x) -: 2}.x,2$' ' 
(2|.!.'x'  x) -: 2}.x,2$'x' [ x=:10&u:'chthonic'
(2|.!.''   x) -: 2}.x,2$' ' 
(2|.!.(s:@<"0 'x')  x) -: 2}.x,2$(s:@<"0 'x') [ x=:s:@<"0 'chthonic'
(2|.!.''   x) -: 2}.x,2${.s:''
(0|.!.'x'  x) -: x=:27$'chthonic'
(0|.!.'x'  x) -: x=:27$u:'chthonic'
(0|.!.'x'  x) -: x=:27$10&u:'chthonic'
(0|.!.(s:@<"0 'x')  x) -: x=:27$s:@<"0 'chthonic'
(2|.!.(<12)x) -: 2}.x,2$<12 [ x=:;:'Cogito, ergo sum.'
(2|.!.''   x) -: 2}.x,2$<$0 
(2|.!.(<12)x) -: 2}.x,2$<12 [ x=:(u:&.>) ;:'Cogito, ergo sum.'
(2|.!.''   x) -: 2}.x,2$<$0 
(2|.!.(<12)x) -: 2}.x,2$<12 [ x=:(10&u:&.>) ;:'Cogito, ergo sum.'
(2|.!.''   x) -: 2}.x,2$<$0 
(2|.!.(<12)x) -: 2}.x,2$<12 [ x=:s:@<"0&.> ;:'Cogito, ergo sum.'
(2|.!.''   x) -: 2}.x,2$<$0 
(2|.!.(<12)x) -: 2}.x,2$<12 [ x=:<"0@s: ;:'Cogito, ergo sum.'
(2|.!.''   x) -: 2}.x,2$<$0 
(0|.!.(<12)x) -: x=:27$;:'Cogito, ergo sum.'
(0|.!.(<12)x) -: x=:27$(u:&.>) ;:'Cogito, ergo sum.'
(0|.!.(<12)x) -: x=:27$(10&u:&.>) ;:'Cogito, ergo sum.'
(0|.!.(<12)x) -: x=:27$s:@<"0&.> ;:'Cogito, ergo sum.'
(0|.!.(<12)x) -: x=:27$<"0@s: ;:'Cogito, ergo sum.'

'domain error' -: 34 |.!.'a' etx 4 5 6
'domain error' -: 34 |.!.(u:'a') etx 4 5 6
'domain error' -: 34 |.!.'a' etx 4;5;6
'domain error' -: 34 |.!.4   etx 'abc'
'domain error' -: 34 |.!.4   etx u:'abc'
'domain error' -: 34 |.!.4   etx 10&u:'abc'
'domain error' -: 34 |.!.4   etx s:@<"0 'abc'
'domain error' -: 34 |.!.4   etx <"0 i.4
'domain error' -: 34 |.!.(<4)etx 'abc'
'domain error' -: 34 |.!.(<4)etx u:'abc'
'domain error' -: 34 |.!.(<4)etx 10&u:'abc'
'domain error' -: 34 |.!.(<4)etx s:@<"0 'abc'
'domain error' -: 34 |.!.(<4)etx 4 5 6

'rank error'   -: ex '|.!.(i.2 3) '

'length error' -: ex '|.!.1 2     '
'length error' -: ex '|.!.(,''a'')'


NB. ,!.n ----------------------------------------------------------------

'domain error' -: 2 4 ,!.'a' etx 2 5$4 5 6
'domain error' -: 2 4 ,!.(u:'a') etx 2 5$4 5 6
'domain error' -: 2 4 ,!.'a' etx 2 5$4;5;6
'domain error' -: 2 4 ,!.4   etx 2 5$'abc'
'domain error' -: 2 4 ,!.4   etx 2 5$u:'abc'
'domain error' -: 2 4 ,!.4   etx 2 5$10&u:'abc'
'domain error' -: 2 4 ,!.4   etx 2 5$s:@<"0 'abc'
'domain error' -: 2 4 ,!.4   etx 2 5$<"0 i.4
'domain error' -: 2 4 ,!.4   etx 2 5$<"0 i.4
'domain error' -: 2 4 ,!.(<4)etx 2 5$'abc'
'domain error' -: 2 4 ,!.(<4)etx 2 5$u:'abc'
'domain error' -: 2 4 ,!.(<4)etx 2 5$10&u:'abc'
'domain error' -: 2 4 ,!.(<4)etx 2 5$s:@<"0 'abc'
'domain error' -: 2 4 ,!.(<4)etx 2 5$4 5 6

'rank error'   -: ex ',!.(i.2 3) '
'rank error'   -: ex ',!.1 2     '
'rank error'   -: ex ',!.(,''a'')'


NB. ,.!.n ---------------------------------------------------------------

'domain error' -: (i.2 4) ,.!.'a' etx 2 1 5$4 5 6
'domain error' -: (i.2 4) ,.!.'a' etx 2 1 5$4;5;6
'domain error' -: (i.2 4) ,.!.4   etx 2 1 5$'abc'
'domain error' -: (i.2 4) ,.!.4   etx 2 1 5$u:'abc'
'domain error' -: (i.2 4) ,.!.4   etx 2 1 5$10&u:'abc'
'domain error' -: (i.2 4) ,.!.4   etx 2 1 5$s:@<"0 'abc'
'domain error' -: (i.2 4) ,.!.4   etx 2 1 5$<"0 i.4
'domain error' -: (i.2 4) ,.!.(<4)etx 2 1 5$'abc'
'domain error' -: (i.2 4) ,.!.(<4)etx 2 1 5$u:'abc'
'domain error' -: (i.2 4) ,.!.(<4)etx 2 1 5$10&u:'abc'
'domain error' -: (i.2 4) ,.!.(<4)etx 2 1 5$s:@<"0 'abc'
'domain error' -: (i.2 4) ,.!.(<4)etx 2 1 5$4 5 6

'rank error'   -: ex ',.!.(i.2 3) '
'rank error'   -: ex ',.!.1 2     '
'rank error'   -: ex ',.!.(,''a'')'


NB. ,:!.n ---------------------------------------------------------------

'domain error' -: 3 4 ,:!.'a' etx 4 5 6
'domain error' -: 3 4 ,:!.'a' etx 4;5;6
'domain error' -: 3 4 ,:!.4   etx 'abc'
'domain error' -: 3 4 ,:!.4   etx u:'abc'
'domain error' -: 3 4 ,:!.4   etx 10&u:'abc'
'domain error' -: 3 4 ,:!.4   etx s:@<"0 'abc'
'domain error' -: 3 4 ,:!.4   etx <"0 i.4
'domain error' -: 3 4 ,:!.(<4)etx 'abc'
'domain error' -: 3 4 ,:!.(<4)etx u:'abc'
'domain error' -: 3 4 ,:!.(<4)etx 10&u:'abc'
'domain error' -: 3 4 ,:!.(<4)etx s:@<"0 'abc'
'domain error' -: 3 4 ,:!.(<4)etx 4 5 6

'rank error'   -: ex ',:!.(i.2 3) '
'rank error'   -: ex ',:!.1 2     '
'rank error'   -: ex ',:!.(,''a'')'


NB. !. comparison tolerance ---------------------------------------------

i=:i.22
v=:1+10^-i
(1=v) -: 1 =!.(2^_44) v
(1=v) -: 14 8#0 1
(1=!.0 v) *./ .<: 16 6#0 1

0 0 0 0 1 -: _  =    ] __ 1 2 3 _
0 0 0 0 1 -: _  =!.0 ] __ 1 2 3 _
1 0 0 0 0 -: __ =    ] __ 1 2 3 _
1 0 0 0 0 -: __ =!.0 ] __ 1 2 3 _

'domain error' -: ex '= !.''a''  '
'limit error' -: ex '~:!.1      '
(,0) -: $ ex '> !.3j4    '  NB. on > error is deferred to execution
'domain error' -: ex '< !.3j4    '
'domain error' -: ex '<:!.(<3 4) '
'limit error' -: ex '= !.1e_7   '

'rank error'   -: ex '>:!.1e_14 0'


NB. x v!.f y where x is extended and v requires fill --------------------

(8x         {.!.17 y) -: 8 {.!.17 y=: 1 2
((17r2-1r2) {.!.17 y) -: 8 {.!.17 y
(8x         $ !.17 y) -: 8 $ !.17 y=: 1 2
((17r2-1r2) $ !.17 y) -: 8 $ !.17 y
(8x         |.!.17 y) -: 8 |.!.17 y=: i.20
((17r2-1r2) |.!.17 y) -: 8 |.!.17 y


randfini''


epilog''

