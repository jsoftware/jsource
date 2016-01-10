NB. $.y -----------------------------------------------------------------

f=: 3 : 0
 (scheck s), y -: $.^:_1 s=: $. y
)

f x=:   ?     13$2
f x=:   ?  11 13$2
f x=:   ?4 11 13$2
f x=:    4 11 13$0

f x=:   ?     13$20
f x=:   ?  11 13$20
f x=:   ?4 11 13$20
f x=:    4 11 13$2-2

f x=:o. ?     13$20
f x=:o. ?  11 13$20
f x=:    4 11 13$o.0

f x=:j./?2    13  $10
f x=:j./?2 11 13  $10
f x=:j./?2 11 13 4$10
f x=:    2 11 13 4$-~1j2

f=: 4 : 0
 (scheck s), y -: $.^:_1 s=: (2;x)$. y
)

d=: 11 3 5 7 2$0
c=: ; (i.1+r) <"1@comb&.>r=:#$d

c f&> <($d)$0
c f&> <($d)$2-2
c f&> <($d)$o.0
c f&> <($d)$-~0j1

1024  -: type $. 10$1
4096  -: type $. 10$2
8192  -: type $. 10$2.7
16384 -: type $. 10$2j7

'domain error' -: $. etx 2 37x
'domain error' -: $. etx 2 3r7

'nonce error'  -: $. etx 3 4$'a'
'nonce error'  -: $. etx <"0 i.2 3


NB. x$.y ----------------------------------------------------------------

ieq=: 2 : ('yi=. y b. _1'; '(5!:5 <''x'') -: y b. _1')

$.^:_1 ieq $.
1&$.   ieq (_1&$.)
2&$.   ieq (_2&$.)
3&$.   ieq (_3&$.)
4&$.   ieq (_4&$.)
5&$.   ieq (_5&$.)
6&$.   ieq (_6&$.)
7&$.   ieq (_7&$.)
_1&$.  ieq ( 1&$.)
_2&$.  ieq ( 2&$.)
_3&$.  ieq ( 3&$.)
_4&$.  ieq ( 4&$.)
_5&$.  ieq ( 5&$.)
_6&$.  ieq ( 6&$.)
_7&$.  ieq ( 7&$.)
_8&$.  ieq ( 8&$.)

'domain error' -: 'a'  $. etx 3 4 5
'domain error' -: 3.5  $. etx 3 4 5
'domain error' -: 3j5  $. etx 3 4 5
'domain error' -: 3r5  $. etx 3 4 5
'domain error' -: 1234 $. etx 1 $. 8 9
'domain error' -: _999 $. etx 1 $. 8 9

'rank error'   -: 1 2  $. etx 3 4 5
'rank error'   -: (<3) $. etx 3 4 5

'length error' -: (3;1;4) $. etx 3.4 5


NB. 1$.y ----------------------------------------------------------------

perm=: i.@! A. i.

scheck@(1&$.)"(1) 3 5 7 11;"1 ,/(perm 1){"(2 1) 1 comb 4
scheck@(1&$.)"(1) 3 5 7 11;"1 ,/(perm 2){"(2 1) 2 comb 4
scheck@(1&$.)"(1) 3 5 7 11;"1 ,/(perm 3){"(2 1) 3 comb 4
scheck@(1&$.)"(1) 3 5 7 11;"1 ,/(perm 4){"(2 1) 4 comb 4

'domain error' -: 1$. etx 'abc'
'domain error' -: 1$. etx 'abc';0 1;0
'domain error' -: 1$. etx (<2 3 4);0 1;0
'domain error' -: 1$. etx (>IF64{3e9 4;3e19 4);0 1;0
'domain error' -: 1$. etx 2 3 4;0 1;2x
'domain error' -: 1$. etx 2 3 4;0 1;2r3

'length error' -: 1$. etx 2 3 4;0 1;0;99

'rank error'   -: 1$. etx 2 3$2 3 4;0 1;0
'rank error'   -: 1$. etx (i.2 3);0 1;0
'rank error'   -: 1$. etx 2 3 4 5 6;(i.2 2);0
'rank error'   -: 1$. etx 2 3 4;0 1;0 1
'rank error'   -: 1$. etx 2 3 4;0 1;''

'index error'  -: 1$. etx 2 3 4;0 0;0
'index error'  -: 1$. etx 2 3 4;0 3;0
'index error'  -: 1$. etx 2 3 4;2 _1;0
'index error'  -: 1$. etx 2 3 4;_4;0
'index error'  -: 1$. etx 2 3 4;0 0;0

'nonce error'  -: 1$. etx 2 3 4;0 1;' '
'nonce error'  -: 1$. etx 2 3 4;0 1;<a:


NB. 2$.y ----------------------------------------------------------------

x=: ?5 6$10
y=: $. x
(i.#$x) -: 2 $.y

(/:~a) -: 2 $. 1 $. 2 3 4 5 6;a=:3?5

scheck (2;2 0) $. $. x=: ? 3 5 7$1000
scheck (2;'' ) $. $. x

f=: 4 : 'd -: $.^:_1 (2;x)$.(2;y)$.d'

d=: ?5 4 1 3 2$3
c=: ; (i.1+r) <"1@comb&.> r=:#$d
] b=: f&>/~c

d=: ?7 4 6 5 1 2 3$3
r=: #$d
g=: >:@?@(r"_) ? r"_
f&g~"0 i.5 10

d -: (2;_1   )&$.&.$. d
d -: (2;_1 _2)&$.&.$. d

a=: ?. 8 5 6 4 2$3
b=: $. a
c=: (2;0    )$.a
d=: (2;0 1  )$.a
e=: (2;0 1 2)$.a

(7$.b) = (2 2;i.5)$.a
(7$.b) = (2 2;i.5)$.c
(7$.b) = (2 2;i.5)$.d
(7$.b) = (2 2;i.5)$.e
(7$.c) = (2 2;0  )$.a
(7$.c) = (2 2;0  )$.b
(7$.c) = (2 2;0  )$.d
(7$.c) = (2 2;0  )$.e
(7$.d) = (2 2;0 1)$.a
(7$.d) = (2 2;0 1)$.b
(7$.d) = (2 2;0 1)$.c
(7$.d) = (2 2;0 1)$.e
(7$.e) = (2 2;i.3)$.a
(7$.e) = (2 2;i.3)$.b
(7$.e) = (2 2;i.3)$.c
(7$.e) = (2 2;i.3)$.d

'domain error' -: 2 $.etx 0 1 2x 
'domain error' -: 2 $.etx 0 1r2 

'domain error' -: (2;'ab') $. etx $. i.2 3
'domain error' -: (2;<<2 ) $. etx $. i.2 3

'rank error'   -: (2;1 1$0)$. etx $. i.2 3

'index error'  -: (2;0   ) $. etx 9
'index error'  -: (2;0 3 ) $. etx    i.2 3 4
'index error'  -: (2;0 3 ) $. etx $. i.2 3 4
'index error'  -: (2;0 0 ) $. etx    i.2 3 4
'index error'  -: (2;0 0 ) $. etx $. i.2 3 4
'index error'  -: (2;_1 2) $. etx    i.2 3 4
'index error'  -: (2;_1 2) $. etx $. i.2 3 4


NB. 3$.y ----------------------------------------------------------------

x=: ?5 6$10
y=: $. x
0 -: 3 $.y

1  -: type 3 $. $. 10$1
4  -: type 3 $. $. 10$2
8  -: type 3 $. $. 10$2.7
16 -: type 3 $. $. 10$2j7

(type@(3&$.) -: type@(5&$.)) $. 10$1
(type@(3&$.) -: type@(5&$.)) $. 10$2
(type@(3&$.) -: type@(5&$.)) $. 10$2.7
(type@(3&$.) -: type@(5&$.)) $. 10$2j7

(scheck x), 1024  -: type x=:(3;1  ) $. $. 1 1 0
(scheck x), 4096  -: type x=:(3;4  ) $. $. 1 1 0
(scheck x), 8192  -: type x=:(3;4.5) $. $. 1 1 0
(scheck x), 16384 -: type x=:(3;4j5) $. $. 1 1 0

(scheck x), 4096  -: type x=:(3;1  ) $. $. 1 2 0
(scheck x), 4096  -: type x=:(3;4  ) $. $. 1 2 0
(scheck x), 8192  -: type x=:(3;4.5) $. $. 1 2 0
(scheck x), 16384 -: type x=:(3;4j5) $. $. 1 2 0

(scheck x), 8192  -: type x=:(3;1  ) $. $. 1.2 0
(scheck x), 8192  -: type x=:(3;4  ) $. $. 1.2 0
(scheck x), 8192  -: type x=:(3;4.5) $. $. 1.2 0
(scheck x), 16384 -: type x=:(3;4j5) $. $. 1.2 0

(scheck x), 16384 -: type x=:(3;1  ) $. $. 1j2 0
(scheck x), 16384 -: type x=:(3;4  ) $. $. 1j2 0
(scheck x), 16384 -: type x=:(3;4.5) $. $. 1j2 0
(scheck x), 16384 -: type x=:(3;4j5) $. $. 1j2 0

'domain error' -: 3 $.etx 0 1 
'domain error' -: 3 $.etx 'abc'
'domain error' -: 3 $.etx 0 1 2
'domain error' -: 3 $.etx 0 1.2 
'domain error' -: 3 $.etx 0 1j2 
'domain error' -: 3 $.etx 0 1 2x 
'domain error' -: 3 $.etx 0 1r2 
'domain error' -: 3 $.etx 0 1;2 

'domain error' -: (3;'a')   $. etx $. i.2 3
'domain error' -: (3;4x )   $. etx $. i.2 3
'domain error' -: (3;4r5)   $. etx $. i.2 3
'domain error' -: (3;<<1)   $. etx $. i.2 3

'rank error'   -: (1 3$3;1) $. etx $. i.2 3
'rank error'   -: (3;,2)    $. etx $. i.2 3
'rank error'   -: (3;'')    $. etx $. i.2 3

'length error' -: (3;1;2)   $. etx $. i.2 3


NB. 4$.y ----------------------------------------------------------------

f=: 3 : 0
 yy=. $. y
 i=. 4$.yy
 assert. (#i) -: #5$.yy
 assert. ({:$i) = #2$.yy
 assert. 4 -: type i
 assert. 2 -: #$i
 assert. *./, (0<:i)*.i<"1 $yy
 1
)

f ?3 4 5$2
f ?3 4 5$20
f o._10+?3 24$20
f j./o._10+?2 34$20

f=: $ #: I.@,
g=: 4 $. $.
(f -: g) x=: 97 101   ?@$ 2
(f -: g) x=: 97 101 2 ?@$ 2
(f -: g) x=: 97 101 3 ?@$ 2

'domain error' -: 4 $.etx 0 1 
'domain error' -: 4 $.etx 'abc'
'domain error' -: 4 $.etx 0 1 2
'domain error' -: 4 $.etx 0 1.2 
'domain error' -: 4 $.etx 0 1j2 
'domain error' -: 4 $.etx 0 1 2x 
'domain error' -: 4 $.etx 0 1r2 
'domain error' -: 4 $.etx 0 1;2 


NB. 5$.y ----------------------------------------------------------------

f=: 3 : 0
 yy=. $. y
 x =. 5$.yy
 assert. (type x) -: type y
 assert. (type x) -: type 3$.yy
 assert. (#x) -: # 4$.yy
 assert. (}.$x) -: (-.(i.#$yy)e.2$.yy)#$yy
 1
)

f ?3 4 5$2
f ?3 4 5$20
f o.?3 45$20
f j./o.?2 45$20

'domain error' -: 5 $.etx 0 1 
'domain error' -: 5 $.etx 'abc'
'domain error' -: 5 $.etx 0 1 2
'domain error' -: 5 $.etx 0 1.2 
'domain error' -: 5 $.etx 0 1j2 
'domain error' -: 5 $.etx 0 1 2x 
'domain error' -: 5 $.etx 0 1r2 
'domain error' -: 5 $.etx 0 1;2 


NB. 7$.y ----------------------------------------------------------------

x=: ?5 6$10
y=: $. x
n=: 7 $. y
n -: +/ ,0~:x
n -: #@(4&$.) y
n -: #@(5&$.) y


NB. 8$.y ----------------------------------------------------------------

f=: 3 : '(*./ scheck s) *. (m*d) -: s=: 8 $. m * (2;y)$.d'

d=: ?5 4 6 3 2$3
m=: ?5 4 6    $2
c=: ; (i.1+r) <"1@comb&.> r=:#$d
f&> c

g=: 3 : '(*./ scheck s) *. (110+m*d) -: s=: 8 $. 110 + m * (2;y)$.d'
g&> c

h=: 3 : '(*./ scheck s) *. (3.4+m*d) -: s=: 8 $. 3.4 + m * (2;y)$.d'
h&> c


NB. $.y large integers in shape -----------------------------------------

n=: IF64 { 2e9 9e18
scheck x=: 1 $. n
n = # x
(,n) -: $x
i=: 1000 ?@$ #x
d=: 1000 ?@$ 0
scheck x=: d i}x
(+/x) -: +/d
(,. /:~ ~. i) -: 4 $. x
b=: (i.#i)=i:~i
d=: (b#d)/:b#i
i=: /:~b#i
d -: 5 $. x

scheck |.x
(|.d) -: 5 $. |.x
(,.(n-1)-|.i) -: 4 $. |. x

4!:55 ;:'a b c d e f g h i ieq m n perm r s x y '


