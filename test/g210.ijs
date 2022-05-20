prolog './g210.ijs'
NB. $y ------------------------------------------------------------------

randuni''

NB. Boolean
s -: $ s $ 0 1 [ s =. $0
s -: $ s $ 0 1 [ s =. ,?20
s -: $ s $ 0 1 [ s =. ?20 30
s -: $ s $ 0 1 [ s =. ?(?7)$10

NB. literal
s -: $ s $ 'abc' [ s =. $0
s -: $ s $ 'abc' [ s =. ,?20
s -: $ s $ 'abc' [ s =. ?20 30
s -: $ s $ 'abc' [ s =. ?(?7)$5

NB. literal2
s -: $ s $ (u:'abc') [ s =. $0
s -: $ s $ (u:'abc') [ s =. ,?20
s -: $ s $ (u:'abc') [ s =. ?20 30
s -: $ s $ (u:'abc') [ s =. ?(?7)$5

NB. literal4
s -: $ s $ (10&u:'abc') [ s =. $0
s -: $ s $ (10&u:'abc') [ s =. ,?20
s -: $ s $ (10&u:'abc') [ s =. ?20 30
s -: $ s $ (10&u:'abc') [ s =. ?(?7)$5

NB. symbol
s -: $ s $ (s:@<"0 'abc') [ s =. $0
s -: $ s $ (s:@<"0 'abc') [ s =. ,?20
s -: $ s $ (s:@<"0 'abc') [ s =. ?20 30
s -: $ s $ (s:@<"0 'abc') [ s =. ?(?7)$5

NB. integer
s -: $ s $ 4 0 [ s =. $0
s -: $ s $ 4 0 [ s =. ,?20
s -: $ s $ 4 0 [ s =. ?20 30
s -: $ s $ 4 0 [ s =. ?(?7)$10

NB. floating point
s -: $ s $ o.4 2 [ s =. $0
s -: $ s $ o.4 2 [ s =. ,?20
s -: $ s $ o.4 2 [ s =. ?10 7
s -: $ s $ o.4 2 [ s =. ?(?7)$5

NB. complex
s -: $ s $ ^0j1 4 [ s =. $0
s -: $ s $ ^0j1 4 [ s =. ,?20
s -: $ s $ ^0j1 4 [ s =. ?4 7
s -: $ s $ ^0j1 4 [ s =. ?(?7)$5

NB. boxed
s -: $ s $ 0j1;'abc' [ s =. $0
s -: $ s $ 0j1;'abc' [ s =. ,?20
s -: $ s $ 0j1;'abc' [ s =. ?4 7
s -: $ s $ 0j1;'abc' [ s =. ?(?7)$5

'' -: $ 0
'' -: $ 'a'
'' -: $ u:'a'
'' -: $ 10&u:'a'
'' -: $ s:@<"0 'a'
'' -: $ 4
'' -: $ _3.456
'' -: $ 3j4
'' -: $ <'Cogito'
'' -: $ <u:'Cogito'
'' -: $ <10&u:'Cogito'
'' -: $ s:@<"0&.> <'Cogito'
'' -: $ <"0@s: <'Cogito'

(,1) -: $ ,0
(,1) -: $ ,'x'
(,1) -: $ ,u:'x'
(,1) -: $ ,10&u:'x'
(,1) -: $ ,s:@<"0 'x'
(,1) -: $ ,_5
(,1) -: $ ,3.4
(,1) -: $ ,5j6.7
(,1) -: $ ,<'ergo'
(,1) -: $ ,<u:'ergo'
(,1) -: $ ,<10&u:'ergo'
(,1) -: $ ,s:@<"0&.> <'ergo'
(,1) -: $ ,<"0@s: <'ergo'

(3!:0 ]10x) -: 3!:0 $ 10 20 30x
(3!:0 ]10x) -: 3!:0 $ 5 % 10 20 30x


NB. x$y -----------------------------------------------------------------

res =. (, }.@$) $ ,@]

NB. Boolean
(4 5 res 0)         -: 4 5$0
(4 5 res b)         -: 4 5$b=.1=?20$2
(4 5 res 3 4$b)     -: 4 5$3 4$b=.1=?20$2
(4 5 0 res b)       -: 4 5 0$b=.1=?20$2
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. literal
(4 5 res 'x')       -: 4 5$'x'
(4 5 res b)         -: 4 5$b=.a.{~?20$$a.
(4 5 res 3 4$b)     -: 4 5$3 4$b=.a.{~?20$$a.
(4 5 0 res b)       -: 4 5 0$b=.a.{~?20$$a.
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. literal2
(4 5 res u:'x')       -: 4 5$u:'x'
(4 5 res b)         -: 4 5$b=.adot1{~?20$$adot1
(4 5 res 3 4$b)     -: 4 5$3 4$b=.adot1{~?20$$adot1
(4 5 0 res b)       -: 4 5 0$b=.adot1{~?20$$adot1
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. literal4
(4 5 res 10&u:'x')       -: 4 5$10&u:'x'
(4 5 res b)         -: 4 5$b=.adot2{~?20$$adot2
(4 5 res 3 4$b)     -: 4 5$3 4$b=.adot2{~?20$$adot2
(4 5 0 res b)       -: 4 5 0$b=.adot2{~?20$$adot2
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. symbol
(4 5 res s:@<"0 'x')       -: 4 5$s:@<"0 'x'
(4 5 res b)         -: 4 5$b=.sdot0{~?20$$sdot0
(4 5 res 3 4$b)     -: 4 5$3 4$b=.sdot0{~?20$$sdot0
(4 5 0 res b)       -: 4 5 0$b=.sdot0{~?20$$sdot0
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. integer
(4 5 res 8)         -: 4 5$8
(4 5 res b)         -: 4 5$b=.?20$12345
(4 5 res 3 4$b)     -: 4 5$3 4$b=.?20$12345
(4 5 0 res b)       -: 4 5 0$b=.?20$12345
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. floating point
(4 5 res 4.5)       -: 4 5$4.5
(4 5 res b)         -: 4 5$b=.o.?20$12345
(4 5 res 3 4$b)     -: 4 5$3 4$b=.o.?20$12345
(4 5 0 res b)       -: 4 5 0$b=.o.?20$12345
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. complex
(4 5 res 4j5)       -: 4 5$4j5
(4 5 res b)         -: 4 5$b=.^0j1*?20$12345
(4 5 res 3 4$b)     -: 4 5$3 4$b=.^0j1*?20$12345
(4 5 0 res b)       -: 4 5 0$b=.^0j1*?20$12345
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

NB. boxed
(4 5 res <9)        -: 4 5$<9
(4 5 res b)         -: 4 5$b=.;:'(4 5 res b) -: 4 5$b=.^0j1*?20$12345'
(4 5 res 3 4$b)     -: 4 5$3 4$b
(4 5 0 res b)       -: 4 5 0$b
(4 5 0 res 0 6$b)   -: 4 5 0$0 6$b
(4 5 0 res 6 0 7$b) -: 4 5 0$6 0 7$b

1 1 1 -: $1 1 1$123
1 2 3 -: $((o.1 2 3)%o.1)$i.6
1 2 3 -: $(-j.j.1 2 3)$'abc'
1 2 3 -: $(-j.j.1 2 3)$u:'abc'
1 2 3 -: $(-j.j.1 2 3)$10&u:'abc'
1 2 3 -: $(-j.j.1 2 3)$s:@<"0 'abc'

0 0 0 0 0 0     -: 6$0
0 1 1 0 0 1     -: 6$0 1 1 0
0 1 0 0 1 1     -: 6$0 1 0 0 1 1
0 1 0 0 1 1     -: 6$0 1 0 0 1 1 0 1 0 1 1 0

'eeeeee'        -: 6$'e'
'eieiei'        -: 6$'ei'
'eioeio'        -: 6$'eio'
'romero'        -: 6$'rome'
'lieb l'        -: 6$'lieb '
'lieber'        -: 6$'lieber'
'Weltan'        -: 6$'Weltanschauung'
'eeeeee'        -: 6$u:'e'
'eieiei'        -: 6$u:'ei'
'eioeio'        -: 6$u:'eio'
'romero'        -: 6$u:'rome'
'lieb l'        -: 6$u:'lieb '
'lieber'        -: 6$u:'lieber'
'Weltan'        -: 6$u:'Weltanschauung'
'eeeeee'        -: 6$10&u:'e'
'eieiei'        -: 6$10&u:'ei'
'eioeio'        -: 6$10&u:'eio'
'romero'        -: 6$10&u:'rome'
'lieb l'        -: 6$10&u:'lieb '
'lieber'        -: 6$10&u:'lieber'
'Weltan'        -: 6$10&u:'Weltanschauung'
(s:@<"0 'eeeeee')        -: 6$s:@<"0 'e'
(s:@<"0 'eieiei')        -: 6$s:@<"0 'ei'
(s:@<"0 'eioeio')        -: 6$s:@<"0 'eio'
(s:@<"0 'romero')        -: 6$s:@<"0 'rome'
(s:@<"0 'lieb l')        -: 6$s:@<"0 'lieb '
(s:@<"0 'lieber')        -: 6$s:@<"0 'lieber'
(s:@<"0 'Weltan')        -: 6$s:@<"0 'Weltanschauung'

0 1 2 3 0 1     -: 6$i.4
0 1 2 3 4 5     -: 6$i.6
0 1 2 3 4 5     -: 6$i.12

3.4 3.4 3.4 3.4 -: 4$3.4
3.4 0 1 3.4     -: 4$3.4 0 1
3.4 0 1 2       -: 4$3.4,i.12

3j4 3j4 3j4 3j4 -: 4$3j4
3j4 0 1 3j4     -: 4$3j4 0 1
3j4 0 1 2       -: 4$3j4,i.12

(1 2;1 2;1 2)   -: 3$<1 2
(1 2;'a';1 2)   -: 3$1 2;'a'
(1 2;'a';3.4)   -: 3$1 2;'a';3.4;<"0 i.7
(1 2;'a';1 2)   -: 3$1 2;u:'a'
(1 2;'a';3.4)   -: 3$1 2;(u:'a');3.4;<"0 i.7
(1 2;'a';1 2)   -: 3$1 2;10&u:'a'
(1 2;'a';3.4)   -: 3$1 2;(10&u:'a');3.4;<"0 i.7

f=: 4 : '(x$y) -: (x$i.#y){y' " 0 _

NB. literal
(    i.11) f&>/ (>:i.10)$&.>(0=?10$4){a.
(200+i.11) f&>/ (>:i.10)$&.>(0=?10$4){a.
(    i.11) f&>/ (>:i.10)$&.>0{a.
(200+i.11) f&>/ (>:i.10)$&.>0{a.
(    i.11) f&>/ (>:i.10)$&.>1{a.
(200+i.11) f&>/ (>:i.10)$&.>1{a.

NB. literal2
(    i.11) f&>/ (>:i.10)$&.>(0=?10$4){adot1
(200+i.11) f&>/ (>:i.10)$&.>(0=?10$4){adot1
(    i.11) f&>/ (>:i.10)$&.>0{adot1
(200+i.11) f&>/ (>:i.10)$&.>0{adot1
(    i.11) f&>/ (>:i.10)$&.>1{adot1
(200+i.11) f&>/ (>:i.10)$&.>1{adot1

NB. literal4
(    i.11) f&>/ (>:i.10)$&.>(0=?10$4){adot2
(200+i.11) f&>/ (>:i.10)$&.>(0=?10$4){adot2
(    i.11) f&>/ (>:i.10)$&.>0{adot2
(200+i.11) f&>/ (>:i.10)$&.>0{adot2
(    i.11) f&>/ (>:i.10)$&.>1{adot2
(200+i.11) f&>/ (>:i.10)$&.>1{adot2

(200+i.11) f    16}.(3!:1) _
(200+i.11) f |. 16}.(3!:1) _
(200+i.11) f    16}.(3!:1) __
(200+i.11) f |. 16}.(3!:1) __
(200+i.11) f    16}.(3!:1) _.
(200+i.11) f |. 16}.(3!:1) _.
(200+i.11) f    16}.(3!:1) o.1
(200+i.11) f |. 16}.(3!:1) o.1

'domain error' -: 'abc'$ etx 'abc'
'domain error' -: 3.5  $ etx 'abc'
'domain error' -: 3j4  $ etx 'abc'
'domain error' -: (<5) $ etx 'abc'
'domain error' -: _3   $ etx 'abc'
'domain error' -: 'abc'$ etx u:'abc'
'domain error' -: 3.5  $ etx u:'abc'
'domain error' -: 3j4  $ etx u:'abc'
'domain error' -: (<5) $ etx u:'abc'
'domain error' -: _3   $ etx u:'abc'
'domain error' -: 'abc'$ etx 10&u:'abc'
'domain error' -: 3.5  $ etx 10&u:'abc'
'domain error' -: 3j4  $ etx 10&u:'abc'
'domain error' -: (<5) $ etx 10&u:'abc'
'domain error' -: _3   $ etx 10&u:'abc'
'domain error' -: 'abc'$ etx s:@<"0 'abc'
'domain error' -: 3.5  $ etx s:@<"0 'abc'
'domain error' -: 3j4  $ etx s:@<"0 'abc'
'domain error' -: (<5) $ etx s:@<"0 'abc'
'domain error' -: _3   $ etx s:@<"0 'abc'

'length error' -: 3 4  $ etx ''
'length error' -: 3 4  $ etx u:''
'length error' -: 3 4  $ etx 10&u:''
'length error' -: 3 4  $ etx s:''
'length error' -: 3 4  $ etx i.0 1 2


NB. x$"r y --------------------------------------------------------------

f =: 4 : 'x$y' " 1 _

2 3 4 (f"1 -: $"1) ?2 3 4$2
2 3 4 (f"1 -: $"1) a.{~?2 3 4$256
2 3 4 (f"1 -: $"1) ?2 3 4$1000
2 3 4 (f"1 -: $"1) o.?2 3 4$1000
2 3 4 (f"1 -: $"1) r./?2 2 3 4$1000
2 3 4 (f"1 -: $"1) 2 3 4$;:'Cogito, ergo sum. sui generis'
2 3 4 (f"1 -: $"1) 2 3 4$(u:&.>) ;:'Cogito, ergo sum. sui generis'
2 3 4 (f"1 -: $"1) 2 3 4$(10&u:&.>) ;:'Cogito, ergo sum. sui generis'
2 3 4 (f"1 -: $"1) 2 3 4$s:@<"0&.> ;:'Cogito, ergo sum. sui generis'
2 3 4 (f"1 -: $"1) 2 3 4$<"0@s: ;:'Cogito, ergo sum. sui generis'
 
2 3 4 (f"2 -: $"2) ?2 3 4$2
2 3 4 (f"2 -: $"2) a.{~?2 3 4$256
2 3 4 (f"2 -: $"2) ?2 3 4$1000
2 3 4 (f"2 -: $"2) o.?2 3 4$1000
2 3 4 (f"2 -: $"2) r./?2 2 3 4$1000
2 3 4 (f"2 -: $"2) 2 3 4$;:'When eras die their thoughts are left to strange police'
2 3 4 (f"2 -: $"2) 2 3 4$(u:&.>) ;:'When eras die their thoughts are left to strange police'
2 3 4 (f"2 -: $"2) 2 3 4$(10&u:&.>) ;:'When eras die their thoughts are left to strange police'
2 3 4 (f"2 -: $"2) 2 3 4$s:@<"0&.> ;:'When eras die their thoughts are left to strange police'
2 3 4 (f"2 -: $"2) 2 3 4$<"0@s: ;:'When eras die their thoughts are left to strange police'

2     (f"2 -: $"2) ?7 3 4$2
2     (f"2 -: $"2) a.{~?7 3 4$256
2     (f"2 -: $"2) ?7 3 4$1000
2     (f"2 -: $"2) o.?7 3 4$1000
2     (f"2 -: $"2) r./?2 7 3 4$1000
2     (f"2 -: $"2) 7 3 4$;:'salt of the earth boustrophedonic bull of Bashan'
2     (f"2 -: $"2) 7 3 4$(u:&.>) ;:'salt of the earth boustrophedonic bull of Bashan'
2     (f"2 -: $"2) 7 3 4$(10&u:&.>) ;:'salt of the earth boustrophedonic bull of Bashan'
2     (f"2 -: $"2) 7 3 4$s:@<"0&.> ;:'salt of the earth boustrophedonic bull of Bashan'
2     (f"2 -: $"2) 7 3 4$<"0@s: ;:'salt of the earth boustrophedonic bull of Bashan'

1e6 2 3 4 5 0 1e4 -: $ 2 3 4 5 0 $"1 2 i.1e6 0 1e4 
1e6 2 3 4 5 0 0   -: $ 2 3 4 5 0 $"1 2 i.1e6 1e4 0

NB. x ($,)"r y -------------------------------------------------------

f =: 3 : 0
ya =: ,: ,: y  NB. Test with leading axes of 1
for. #$ya do.
 xa =: ? +: $ ya
 while. #xa do.
  for_r. i.>:#$ya do.
   rk =: r
   yb =: xa ($,)"(1,r) ya
   assert. yb -: xa 4 : 'x $ , y'"(1,r) ya
   NB. repeat inplaceable
   yc =: 1 |. _1 |. ya
   yc =: xa ($,)"(1,r) yc
   assert. yc -: xa 4 : 'x $ , y'"(1,r) ya
  end.
  xa =: }: xa
 end.
 ya =: ,/ ya
end.
1
)

f ?2 3 4$2
f a.{~?2 3 4$256
f ?2 3 4$1000
f o.?2 3 4$1000
f r./?2 2 3 4$1000
f 2 3 4$;:'Cogito, ergo sum. sui generis'
f 2 3 4$(u:&.>) ;:'Cogito, ergo sum. sui generis'
f 2 3 4$(10&u:&.>) ;:'Cogito, ergo sum. sui generis'
f 2 3 4$s:@<"0&.> ;:'Cogito, ergo sum. sui generis'
f 2 3 4$<"0@s: ;:'Cogito, ergo sum. sui generis'

NB. x$!.f y -------------------------------------------------------------

(2 3 4$2 3 4 5,   20$0  ) -: 2 3 4$!.'' 2 3 4 5
(2 3 4$'chthonic',16$' ') -: 2 3 4$!.'' 'chthonic'
(2 3 4$t,         20$<$0) -: 2 3 4$!.'' t=.;:'Cogito, ergo sum.'
(2 3 4$'chthonic',16$' ') -: 2 3 4$!.'' u:'chthonic'
(2 3 4$t,         20$<$0) -: 2 3 4$!.'' t=.(u:&.>) ;:'Cogito, ergo sum.'
(2 3 4$'chthonic',16$' ') -: 2 3 4$!.'' 10&u:'chthonic'
(2 3 4$t,         20$<$0) -: 2 3 4$!.'' t=.(10&u:&.>) ;:'Cogito, ergo sum.'
(2 3 4$(s:@<"0 'chthonic'),16${.s:'') -: 2 3 4$!.'' s:@<"0 'chthonic'
(2 3 4$t,         20$<$0) -: 2 3 4$!.'' t=.s:@<"0&.> ;:'Cogito, ergo sum.'
(2 3 4$t,         20$<$0) -: 2 3 4$!.'' t=.<"0@s: ;:'Cogito, ergo sum.'

(2 15$0 1 0,27$1  ) -: 2 15$!.1   [ 0 1 0
(2 15$1 2 3,27$1  ) -: 2 15$!.1   [ 1 2 3
(2 15$1.2 3,28$1  ) -: 2 15$!.1   [ 1.2 3
(2 15$1 2j3,28$1  ) -: 2 15$!.1   [ 1 2j3

(2 15$0 1 0,27$_1 ) -: 2 15$!._1  [ 0 1 0
(2 15$1 2 3,27$_1 ) -: 2 15$!._1  [ 1 2 3
(2 15$1.2 3,28$_1 ) -: 2 15$!._1  [ 1.2 3
(2 15$1 2j3,28$_1 ) -: 2 15$!._1  [ 1 2j3

(2 15$0 1 0,27$3.5) -: 2 15$!.3.5 [ 0 1 0
(2 15$1 2 3,27$3.5) -: 2 15$!.3.5 [ 1 2 3
(2 15$1.2 3,28$3.5) -: 2 15$!.3.5 [ 1.2 3
(2 15$1 2j3,28$3.5) -: 2 15$!.3.5 [ 1 2j3

(2 15$0 1 0,27$3j5) -: 2 15$!.3j5 [ 0 1 0
(2 15$1 2 3,27$3j5) -: 2 15$!.3j5 [ 1 2 3
(2 15$1.2 3,28$3j5) -: 2 15$!.3j5 [ 1.2 3
(2 15$1 2j3,28$3j5) -: 2 15$!.3j5 [ 1 2j3

((i.4),"0 1 [4$8) -: 5$!.8"0 i.4
(4 5$8)           -: 5$!.8"1 i.4 0

'domain error' -: 9 3 $!.'a'  etx 4
'domain error' -: 9 3 $!.'a'  etx 'b';'c';'d'
'domain error' -: 9 3 $!.4    etx 'sui generis'
'domain error' -: 9 3 $!.4    etx ;:'Cogito, ergo sum.'
'domain error' -: 9 3 $!.(<4) etx 'eleemosynary'
'domain error' -: 9 3 $!.(u:'a')  etx 4
'domain error' -: 9 3 $!.(u:'a')  etx 'b';'c';'d'
'domain error' -: 9 3 $!.4    etx u:'sui generis'
'domain error' -: 9 3 $!.4    etx u:&.> ;:'Cogito, ergo sum.'
'domain error' -: 9 3 $!.(<4) etx u:'eleemosynary'
'domain error' -: 9 3 $!.(10&u:'a')  etx 4
'domain error' -: 9 3 $!.(10&u:'a')  etx 'b';'c';'d'
'domain error' -: 9 3 $!.4    etx 10&u:'sui generis'
'domain error' -: 9 3 $!.4    etx 10&u:&.> ;:'Cogito, ergo sum.'
'domain error' -: 9 3 $!.(<4) etx 10&u:'eleemosynary'
'domain error' -: 9 3 $!.(s:@<"0 'a')  etx 4
'domain error' -: 9 3 $!.(s:@<"0 'a')  etx <"0@s: 'b';'c';'d'
'domain error' -: 9 3 $!.4    etx s:@<"0 'sui generis'
'domain error' -: 9 3 $!.4    etx s:@<"0&.> ;:'Cogito, ergo sum.'
'domain error' -: 9 3 $!.4    etx <"0@s: ;:'Cogito, ergo sum.'
'domain error' -: 9 3 $!.(<4) etx s:@<"0 'eleemosynary'
'domain error' -: 9 3 $!.(<4) etx i.2 3

4!:55 ;:'adot1 adot2 sdot0 b f res rk s t ya xa yb yc'
randfini''


epilog''

