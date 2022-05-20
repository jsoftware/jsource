prolog './g330.ijs'
NB. ;y ------------------------------------------------------------------

randuni''

maxr  =: [: >./ #@$&>
templ =: $&0^:2 @ (1&>.) @ maxr
raze  =: (1&#) @ > @ (,&.>/) @ (,<@templ) @ ,

(; -: raze) 1 2 3 4;5 6 7 8;i.3 4
(; -: raze) (i.3 4);1 2 3 4;5 6 7 8

NB. Boolean
(; -: raze) 0 1 0;1 0;0
(; -: raze) 0  ;1=?3 4$2
(; -: raze) 0 1;1=?3 4$2
(; -: raze) <"0 t=:?2 3 4$2
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. literal
(; -: raze) 'abc';'dc';'a'
(; -: raze) 'a' ;3 4$'abc'
(; -: raze) 'ad';3 4$'abc'
(; -: raze) <"0 t=:(?2 3 4 2$#a.){a.
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. literal2
(; -: raze) u:&.>'abc';'dc';'a'
(; -: raze) u:&.>'a' ;3 4$'abc'
(; -: raze) u:&.>'ad';3 4$'abc'
(; -: raze) <"0 t=:(?2 3 4 2$#adot1){adot1
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. literal4
(; -: raze) 10&u:&.>'abc';'dc';'a'
(; -: raze) 10&u:&.>'a' ;3 4$'abc'
(; -: raze) 10&u:&.>'ad';3 4$'abc'
(; -: raze) <"0 t=:(?2 3 4 2$#adot2){adot2
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. symbol
(; -: raze) s:@<"0&.> 'abc';'dc';'a'
(; -: raze) <"0@s: 'abc';'dc';'a'
(; -: raze) (s:@<"0 'a') ;3 4$s:@<"0 'abc'
(; -: raze) (s:@<"0 'ad');3 4$s:@<"0 'abc'
(; -: raze) <"0 t=:(?2 3 4 2$#sdot0){sdot0
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. integer
(; -: raze) 0 1 2;3 4;2
(; -: raze) 2  ;?3 4$100000
(; -: raze) 2 3;?3 4$100000
(; -: raze) <"0 t=:?2 3 4 2$100000
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. floating point
(; -: raze) 1.2 3.4 5.6;_12.6 17;o.1
(; -: raze) 2.5  ;o.?3 4$100000
(; -: raze) 2.5 6;o.?3 4$100000
(; -: raze) <"0 t=:o.?2 3 4 2$100000
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

NB. complex
(; -: raze) 1j2 3j4 5j6;_12j6 17;0j1
(; -: raze) 2j5  ;^0j1*?3 4$100000
(; -: raze) 2j5 6;^0j1*?3 4$100000
(; -: raze) <"0 t=:^0j1*?2 3 4 2$100000
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t
(; -: raze) 0 1 0; 123 45; 3.4 _5.67e8; 6j7

NB. boxed
(; -: raze) (1;'two';'drei');(3 4;3 4$6);<<i.2 3 4
(; -: raze) (1;u:&.>'two';'drei');(3 4;3 4$6);<<i.2 3 4
(; -: raze) (1;10&u:&.>'two';'drei');(3 4;3 4$6);<<i.2 3 4
(; -: raze) (1;s:@<"0&.> 'two';'drei');(3 4;3 4$6);<<i.2 3 4
(; -: raze) (1;<"0@s: 'two';'drei');(3 4;3 4$6);<<i.2 3 4
(; -: raze) (<123)       ;<<"0?3 4$100
(; -: raze) ((<123),<'a');<<"0?3 4$100
(; -: raze) t=:(?2 3 4 2$#x){x=:<&.>(<"0?30$100),;:'(; -: raz)^0j1*?2 3$10'
(; -: raze) <"0 t
(; -: raze) <"1 t
(; -: raze) <"2 t
(; -: raze) <"3 t
(; -: raze) <"4 t

($0)   -: ;''
($0)   -: ;u:''
($0)   -: ;10&u:''
($0)   -: ;s:''
($0)   -: ;i.0 4 5
(,0)   -: ;0
(,0)   -: ;<0
(,'a') -: ;'a'
(,'a') -: ;<'a'
(,'a') -: ;u:'a'
(,'a') -: ;<u:'a'
(,'a') -: ;10&u:'a'
(,'a') -: ;<10&u:'a'
(,s:@<"0 'a') -: ;s:@<"0 'a'
(,s:@<"0 'a') -: ;s:@<"0&.> <'a'
(,s:@<"0 'a') -: ;<"0@s: <'a'
(,9)   -: ;9
(,9)   -: ;<9
(,3.4) -: ;3.4
(,3.4) -: ;<3.4
(,3j4) -: ;3j4
(,3j4) -: ;<3j4

x -: ; 10$<x=:i.0 
x -: ; 10$<x=:i.0 2
x -: ; 10$<x=:i.0 2 3
x -: ; 10$<x=:i.0 2 3 4

(i. (+/x),y) -: ; (x=:?10$10)$&.><i.1,y=:0
(i. (+/x),y) -: ; (x=:?10$10)$&.><i.1,y=:0 2
(i. (+/x),y) -: ; (x=:?10$10)$&.><i.1,y=:0 2 3
(i. (+/x),y) -: ; (x=:?10$10)$&.><i.1,y=:0 2 3 4

(; -: raze) (?#x) A. x=:1 2 3;1.2 3;'';0 1;3j4
(; -: raze) (?#x) A. x
(; -: raze) (?#x) A. x

(; -: raze) ; (<0 5$<123),<2 3 4$<'a'
(; -: raze) ; (<0 5$<123),<2 3 4$<u:'a'
(; -: raze) ; (<0 5$<123),<2 3 4$<10&u:'a'
(; -: raze) ; (<0 5$<123),<2 3 4$s:@<"0&.> <'a'
(; -: raze) ; (<0 5$<123),<2 3 4$<"0@s: <'a'

(1 2 4,:3 4 5) -: ;!.4 (1 2 ; ,: 3 4 5)
(1 2 4,:3 4 5) -: ;!.'a' (1 2 4 ; ,: 3 4 5)
(,:2#0) -: ; (0 2$4);''
(,:2#a:) -: ; (0 2$a:);''
(,:2#'a') -: ;!.'a' (0 2$4);''
(,:2#'a') -: ;!.'a' (0 2$a:);''
(,:2#2) -: ;!.2 (0 2$a:);''
(,:'aa') -: ;!.'a' (0 2$a:) ; 0$5
(,:2#0) -: ; (0 2$4);u:''
(,:2#a:) -: ; (0 2$a:);u:''
(,:2#'a') -: ;!.(u:'a') (0 2$4);u:''
(,:2#'a') -: ;!.(u:'a') (0 2$a:);u:''
(,:2#2) -: ;!.2 (0 2$a:);u:''
(,:'aa') -: ;!.(u:'a') (0 2$a:) ; 0$5
(,:2#0) -: ; (0 2$4);10&u:''
(,:2#a:) -: ; (0 2$a:);10&u:''
(,:2#'a') -: ;!.(10&u:'a') (0 2$4);10&u:''
(,:2#'a') -: ;!.(10&u:'a') (0 2$a:);10&u:''
(,:2#2) -: ;!.2 (0 2$a:);10&u:''
(,:'aa') -: ;!.(10&u:'a') (0 2$a:) ; 0$5
(,:2{.s:'') -: ; (0 2$4);s:''
(,:2{.s:'') -: ; (0 2$a:);s:''
(,:2#s:@<"0 'a') -: ;!.(s:@<"0 'a') (0 2$4);s:''
(,:2#s:@<"0 'a') -: ;!.(s:@<"0 'a') (0 2$a:);s:''
(,:2#2) -: ;!.2 (0 2$a:);s:''
(,:s:@<"0 'aa') -: ;!.(s:@<"0 'a') (0 2$a:) ; 0$5
(0 0 0 0 0 0 0 0,:1j1 1j1 1j1 1j1 1j1 1j1 1j1 1j1 ) -: ;!.1j1 (1 8$0) ; (0 $ 2.2)
(4 4 4 4 4 4 4 4 ,: 4 4 4 4 0.5 0.5 0.5 0.5) -: ;!.0.5 (1 8$4) ; (4$4)
(,: 4 4 4 4) -: ;!.'a' (1 4$4);(0 2$a:)
(,: 4 4 4 4) -: ; (1 4$4);(0 2$a:)
(,: 4 4 4 4 0 0) -: ; (1 4$4);(0 6$a:)
(2 1$a:) -: ; (2 0$4);(0 1$a:)
(2 1$'a') -: ;!.'a' (2 0$4);(0 1$a:)
2 -: type ;!.'a' (0 $ a:) ; (0$a:)
2 -: type ;!.'a' (2 0 $ a:) ; (2 0$a:)
2 -: type ;!.'a' (2 0 $ a:) ; (0$a:)
2 -: type ;!.'a' (2 0 $ 4) ; (0 1$a:)
131072 -: type ;!.(u:'a') (0 $ a:) ; (0$a:)
131072 -: type ;!.(u:'a') (2 0 $ a:) ; (2 0$a:)
131072 -: type ;!.(u:'a') (2 0 $ a:) ; (0$a:)
131072 -: type ;!.(u:'a') (2 0 $ 4) ; (0 1$a:)
262144 -: type ;!.(10&u:'a') (0 $ a:) ; (0$a:)
262144 -: type ;!.(10&u:'a') (2 0 $ a:) ; (2 0$a:)
262144 -: type ;!.(10&u:'a') (2 0 $ a:) ; (0$a:)
262144 -: type ;!.(10&u:'a') (2 0 $ 4) ; (0 1$a:)
65536 -: type ;!.(s:@<"0 'a') (0 $ a:) ; (0$a:)
65536 -: type ;!.(s:@<"0 'a') (2 0 $ a:) ; (2 0$a:)
65536 -: type ;!.(s:@<"0 'a') (2 0 $ a:) ; (0$a:)
65536 -: type ;!.(s:@<"0 'a') (2 0 $ 4) ; (0 1$a:)
32 -: type ; (2 0 $ 4) ; (0 1$a:)
2 -: type ;!.'a' (0 2 0 $ a:) ; (0 0 0$a:)
32 -: type ;!.'a' (1 1 1 $ a:) ;< (1 1 1$a:)
32 -: type ;!.'a' (1 1 1 $ a:) ;< (1 1$a:)
32 -: type ;!.'a' (1 1 1 $ a:) ;< (1$a:)
32 -: type ;!.'a' (2 1 1 $ a:) ;< (1 1 1$a:)
32 -: type ;!.'a' (2 1 1 $ a:) ;< (1 1$a:)
32 -: type ;!.'a' (2 1 1 $ a:) ;< (1$a:)


'domain error' -: ;!.'a' etx  (1 2 ; ,: 3 4 5)
'domain error' -: ;!.'a' etx  (1 4$4);(0 6$a:)
'domain error' -: ;!.'a' etx  (2 1$a:);(0 2$a:)
'domain error' -: ;!.(u:'a') etx  (1 2 ; ,: 3 4 5)
'domain error' -: ;!.(u:'a') etx  (1 4$4);(0 6$a:)
'domain error' -: ;!.(u:'a') etx  (2 1$a:);(0 2$a:)
'domain error' -: ;!.(10&u:'a') etx  (1 2 ; ,: 3 4 5)
'domain error' -: ;!.(10&u:'a') etx  (1 4$4);(0 6$a:)
'domain error' -: ;!.(10&u:'a') etx  (2 1$a:);(0 2$a:)
'domain error' -: ;!.(s:@<"0 'a') etx  (1 2 ; ,: 3 4 5)
'domain error' -: ;!.(s:@<"0 'a') etx  (1 4$4);(0 6$a:)
'domain error' -: ;!.(s:@<"0 'a') etx  (2 1$a:);(0 2$a:)
'domain error' -: ;!.2 etx  (1 2$a:) ; (0 $ a:)
'domain error' -: ;!.2 etx  (2 2 1 $ a:) ;< (1 1$a:)
'domain error' -: ;!.2 etx  (2 2 1 $ a:) ;< (1$a:)


'domain error' -: ; etx 1 2 ; 'abc'
'domain error' -: ; etx 1 2 ;~'abc'
'domain error' -: ; etx 1 2 ; <<'abc'
'domain error' -: ; etx 1 2 ;~<<'abc'
'domain error' -: ; etx 'ab'; <<'a'
'domain error' -: ; etx 'ab';~<<'a'
'domain error' -: ; etx 1 2 ; u:'abc'
'domain error' -: ; etx 1 2 ;~u:'abc'
'domain error' -: ; etx 1 2 ; <<u:'abc'
'domain error' -: ; etx 1 2 ;~<<u:'abc'
'domain error' -: ; etx 'ab'; <<u:'a'
'domain error' -: ; etx 'ab';~<<u:'a'
'domain error' -: ; etx 1 2 ; 10&u:'abc'
'domain error' -: ; etx 1 2 ;~10&u:'abc'
'domain error' -: ; etx 1 2 ; <<10&u:'abc'
'domain error' -: ; etx 1 2 ;~<<10&u:'abc'
'domain error' -: ; etx 'ab'; <<10&u:'a'
'domain error' -: ; etx 'ab';~<<10&u:'a'
'domain error' -: ; etx 1 2 ; s:@<"0 'abc'
'domain error' -: ; etx 1 2 ;~s:@<"0 'abc'
'domain error' -: ; etx 1 2 ; <s:@<"0&.> <'abc'
'domain error' -: ; etx 1 2 ; <<"0@s: <'abc'
'domain error' -: ; etx 1 2 ;~<s:@<"0&.> <'abc'
'domain error' -: ; etx 1 2 ;~<<"0@s: <'abc'
'domain error' -: ; etx (s:@<"0 'ab'); <s:@<"0&.> <'a'
'domain error' -: ; etx (s:@<"0 'ab'); <<"0@s: <'a'
'domain error' -: ; etx (s:@<"0 'ab');~<s:@<"0&.> <'a'
'domain error' -: ; etx (s:@<"0 'ab');~<<"0@s: <'a'


NB. x;y -----------------------------------------------------------------

boxed =: e.&32 64@(3!:0)
mt    =: 0&e.@$
link  =: <@[ , <`]@.(boxed *. -.@mt)@]
f =. 4 : 0
assert. x (; -: link) y
assert. x ((;<) -: (link<)) y
assert. x (,&< -: (link<)) y
if. boxed x do. assert. x ((,<) -: (,"] <)) y end.
1
)

1 2          f 3 4 5
''           f ''
'abc'        f +&.>i.3 4
(u:'abc')    f +&.>i.3 4
(10&u:'abc') f +&.>i.3 4
(s:@<"0 'abc')f +&.>i.3 4
(+&.>i.3 4)  f 'abc'
(+&.>i.3 4)  f u:'abc'
(+&.>i.3 4)  f 10&u:'abc'
(+&.>i.3 4)  f s:@<"0 'abc'
(<i.3 4)  f 'abc'
(,<i.3 4)  f 'abc'
(4 2 { <"1 i.5 4)  f 'abc'  NB. nonrecursive
(3 { <"1 i.5 4)  f 'abc'  NB. nonrecursive
(2 3;i.3 4)  f 'abc'
(<i.3 4)  f u:'abc'
(<i.3 4)  f 10&u:'abc'
(<i.3 4)  f s:@<"0 'abc'
''           f 0$<''
''           f 0$<u:''
''           f 0$<10&u:''
''           f 0$s:@<"0&.> <''
''           f 0$<"0@s: <''
(0$<'')      f ''
(0$<'')      f u:''
(0$<'')      f 10&u:''
(0$<'')      f s:''

IGNOREIFFVI (IF64{1200 2800) > 7!:2 'x;y' [ x=: i.1e4 [ y=: 2e4$'chthonic'
IGNOREIFFVI (IF64{1200 2800) > 7!:2 'x;y' [ x=: i.1e4 [ y=: 2e4$u:'chthonic'
IGNOREIFFVI (IF64{1200 2800) > 7!:2 'x;y' [ x=: i.1e4 [ y=: 2e4$10&u:'chthonic'
IGNOREIFFVI (IF64{1200 2800) > 7!:2 'x;y' [ x=: i.1e4 [ y=: 2e4$s:@<"0 'chthonic'

'domain error' -: ex '>''abc''; 2 3 4     '
'domain error' -: ex '>''abc'';~2 3 4     '
'domain error' -: ex '>''abc''; 2;3;4     '
'domain error' -: ex '>''abc'';~2;3;4     '
'domain error' -: ex '>2 3 4; 2;3;''1234'''
'domain error' -: ex '>2 3 4;~2;3;''1234'''
'domain error' -: ex '>(u:''abc''); 2 3 4     '
'domain error' -: ex '>(u:''abc'');~2 3 4     '
'domain error' -: ex '>(u:''abc''); 2;3;4     '
'domain error' -: ex '>(u:''abc'');~2;3;4     '
'domain error' -: ex '>2 3 4; 2;3;(u:''1234'')'
'domain error' -: ex '>2 3 4;~2;3;(u:''1234'')'
'domain error' -: ex '>(10&u:''abc''); 2 3 4     '
'domain error' -: ex '>(10&u:''abc'');~2 3 4     '
'domain error' -: ex '>(10&u:''abc''); 2;3;4     '
'domain error' -: ex '>(10&u:''abc'');~2;3;4     '
'domain error' -: ex '>2 3 4; 2;3;(10&u:''1234'')'
'domain error' -: ex '>2 3 4;~2;3;(10&u:''1234'')'
'domain error' -: ex '>(s:@<"0 ''abc''); 2 3 4     '
'domain error' -: ex '>(s:@<"0 ''abc'');~2 3 4     '
'domain error' -: ex '>(s:@<"0 ''abc''); 2;3;4     '
'domain error' -: ex '>(s:@<"0 ''abc'');~2;3;4     '
'domain error' -: ex '>2 3 4; 2;3;(s:@<"0 ''1234'')'
'domain error' -: ex '>2 3 4;~2;3;(s:@<"0 ''1234'')'

'domain error' -: ex '>''abc'',&< 2 3 4     '
'domain error' -: ex '>''abc'',&<~2 3 4     '
'domain error' -: ex '>''abc'',&< 2,&<3,&<4     '
'domain error' -: ex '>''abc'',&<~2,&<3,&<4     '
'domain error' -: ex '>2 3 4,&< 2,&<3,&<''1234'''
'domain error' -: ex '>2 3 4,&<~2,&<3,&<''1234'''
'domain error' -: ex '>(u:''abc''),&< 2 3 4     '
'domain error' -: ex '>(u:''abc''),&<~2 3 4     '
'domain error' -: ex '>(u:''abc''),&< 2,&<3,&<4     '
'domain error' -: ex '>(u:''abc''),&<~2,&<3,&<4     '
'domain error' -: ex '>2 3 4,&< 2,&<3,&<(u:''1234'')'
'domain error' -: ex '>2 3 4,&<~2,&<3,&<(u:''1234'')'
'domain error' -: ex '>(10&u:''abc''),&< 2 3 4     '
'domain error' -: ex '>(10&u:''abc''),&<~2 3 4     '
'domain error' -: ex '>(10&u:''abc''),&< 2,&<3,&<4     '
'domain error' -: ex '>(10&u:''abc''),&<~2,&<3,&<4     '
'domain error' -: ex '>2 3 4,&< 2,&<3,&<(10&u:''1234'')'
'domain error' -: ex '>2 3 4,&<~2,&<3,&<(10&u:''1234'')'
'domain error' -: ex '>(s:@<"0 ''abc''),&< 2 3 4     '
'domain error' -: ex '>(s:@<"0 ''abc''),&<~2 3 4     '
'domain error' -: ex '>(s:@<"0 ''abc''),&< 2,&<3,&<4     '
'domain error' -: ex '>(s:@<"0 ''abc''),&<~2,&<3,&<4     '
'domain error' -: ex '>2 3 4,&< 2,&<3,&<(s:@<"0 ''1234'')'
'domain error' -: ex '>2 3 4,&<~2,&<3,&<(s:@<"0 ''1234'')'

'domain error' -: ex '>''abc''(;<) 2 3 4     '
'domain error' -: ex '>''abc''(;<)~2 3 4     '
'domain error' -: ex '>''abc''(;<) 2(;<)3(;<)4     '
'domain error' -: ex '>''abc''(;<)~2(;<)3(;<)4     '
'domain error' -: ex '>2 3 4(;<) 2(;<)3(;<)''1234'''
'domain error' -: ex '>2 3 4(;<)~2(;<)3(;<)''1234'''
'domain error' -: ex '>(u:''abc'')(;<) 2 3 4     '
'domain error' -: ex '>(u:''abc'')(;<)~2 3 4     '
'domain error' -: ex '>(u:''abc'')(;<) 2(;<)3(;<)4     '
'domain error' -: ex '>(u:''abc'')(;<)~2(;<)3(;<)4     '
'domain error' -: ex '>2 3 4(;<) 2(;<)3(;<)(u:''1234'')'
'domain error' -: ex '>2 3 4(;<)~2(;<)3(;<)(u:''1234'')'
'domain error' -: ex '>(10&u:''abc'')(;<) 2 3 4     '
'domain error' -: ex '>(10&u:''abc'')(;<)~2 3 4     '
'domain error' -: ex '>(10&u:''abc'')(;<) 2(;<)3(;<)4     '
'domain error' -: ex '>(10&u:''abc'')(;<)~2(;<)3(;<)4     '
'domain error' -: ex '>2 3 4(;<) 2(;<)3(;<)(10&u:''1234'')'
'domain error' -: ex '>2 3 4(;<)~2(;<)3(;<)(10&u:''1234'')'
'domain error' -: ex '>(s:@<"0 ''abc'')(;<) 2 3 4     '
'domain error' -: ex '>(s:@<"0 ''abc'')(;<)~2 3 4     '
'domain error' -: ex '>(s:@<"0 ''abc'')(;<) 2(;<)3(;<)4     '
'domain error' -: ex '>(s:@<"0 ''abc'')(;<)~2(;<)3(;<)4     '
'domain error' -: ex '>2 3 4(;<) 2(;<)3(;<)(s:@<"0 ''1234'')'
'domain error' -: ex '>2 3 4(;<)~2(;<)3(;<)(s:@<"0 ''1234'')'

'domain error' -: ex '1 2 3(,<) 2 3 4     '
'domain error' -: ex '1(,<) 2 3 4     '
(,<1 2 3) -: (0$a:) (,<) 1 2 3
16b1000020 = 1 { 13!:83 (>:  i. 3) ; (>: i. 4)  NB. pristine recursive
16b1000020 = 1 { 13!:83 (>:  i. 3) ; (< >: i. 4)
16b1000020 = 1 { 13!:83 (>:  i. 3) (;<) (>: i. 4)
16b1000020 = 1 { 13!:83 (>:  i. 3) (,&<) (>: i. 4)
16b1000020 = 1 { 13!:83 (< >:  i. 3) (,<) (>: i. 4)
16b20 = 1 { 13!:83 (>:  i. 3) ; (i. 4)
3 4 -: # S:0 (< >:  i. 3) (,<) (>: i. 4)

1: ": }: (0, 2);(0;0);a=. 0;0   NB. crashed when (0,2) was left inplaceable
4 -: (3!:0) > _3 }. (0, 2);(0;0);a=. 0;0


4!:55 ;:'a adot1 adot2 sdot0 boxed f link maxr mt raze t templ x y'
randfini''


epilog''

