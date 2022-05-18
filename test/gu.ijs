prolog './gu.ijs'
NB. Unicode 2-byte characters -------------------------------------------

UTYPE=: 131072   NB. type code for Unicode

domerr=: 1 : 0
 if. 1 e. y do. 
  assert. 'domain error' -: u f. etx u: 'abcd'
 end.
 if. 2 e. y do.
  assert. 'domain error' -: 2          u f. etx u: 'abc'
  assert. 'domain error' -: (u: 'xyz') u f. etx 2
  assert. 'domain error' -: (u: 'xyz') u f. etx u: 'abc'
 end.
 if. 'l' e. y do.
  assert. 'domain error' -: (u: 'ab')  u f. etx i.2 3
 end.
 if. 'r' e. y do.
  assert. 'domain error' -: 2 3 4      u f. etx u: 'abc'
 end.
 1
)


NB. u: ------------------------------------------------------------------

x=: u: i=: _65536+2 3 4 ?@$ 2*65536
($x) -: $i
UTYPE -: type x
(65536|i) -: 3 u: x

x=: u: t=: a.{~ i=: ?2 3 4$#a.
($x) -: $i
UTYPE -: type x
x -: t
t -: 1 u: x
i -: 3 u: x

x=: u: y=: u: i=: _65536+2 3 4?@$ 2*65536
($x) -: $i
UTYPE -: type x
(65536|i) -: 3 u: x
x -: y
(3!:1 x) -: 3!:1 y

x=: a.{~?3 5 7 2$#a.
y=: 6 u: x
($y) -: (}:$x),-:{:$x

lr=: 1 : '5!:5 <''u'''
2&u: lr -: 1&u: b. _1
01&u: lr -: 2&u: b. _1
4&u: lr -: 3&u: b. _1
3&u: lr -: 4&u: b. _1

'domain error' -: u: etx 0 1.2
'domain error' -: u: etx 0 1j2
'domain error' -: u: etx 0 1r2
'domain error' -: u: etx 0 1;2

'domain error' -:  0 u: etx 0 1 2
'domain error' -: _1 u: etx 0 1 2
'domain error' -: _2 u: etx 0 1 2
'domain error' -: _3 u: etx 0 1 2
'domain error' -: _4 u: etx 0 1 2

'domain error' -:  0 u: etx 'abc'
'domain error' -: _1 u: etx 'abc'
'domain error' -: _2 u: etx 'abc'
'domain error' -: _3 u: etx 'abc'
'domain error' -: _4 u: etx 'abc'

'index error'  -:  4 u: etx 1 2 3 65536
'domain error' -:  3 u: etx 2 3
'' -: 3 u: 0$0
'' -: 3 u: 0$a:
'' -: 3 u: 0$1.0
'' -: 3 u: 0$''

'length error' -:  6 u: etx 'abc'
'length error' -:  6 u: etx 'abcde'

NB. 7&u:  8&u:

NB.  bmp pane
(,0)    -: 3 u: 8 u: u: 0
(,16b7f)-: 3 u: 8 u: u: 16b7f
194 128 -: 3 u: 8 u: u: 16b80
223 191 -: 3 u: 8 u: u: 16b7ff
224 160 128 -: 3 u: 8 u: u: 16b800
239 191 191 -: 3 u: 8 u: u: 16bffff
16b1234 16b5678 -: 3&u: u: 16b1234 16b5678
225 136 180 229 153 184 -: 3&u: 8&u: u: 16b1234 16b5678

NB. null
0 128 0 129 0 0 65535 0 -: 3&u: u: 0 128 0 129 0 0 65535 0
0 194 128 0 194 129 0 0 239 191 191 0 -: 3 u: 8 u: u: 0 128 0 129 0 0 65535 0
(7 u: a.{~0 194 128 0 194 129 0 0 239 191 191 0) -: u: 0 128 0 129 0 0 65535 0

NB. invalid/overlong utf-8
'domain error' -: 7 u: etx a.{~128
'domain error' -: 7 u: etx a.{~193
'domain error' -: 7 u: etx a.{~245
'domain error' -: 7 u: etx a.{~224
'domain error' -: 7 u: etx a.{~248
'domain error' -: 7 u: etx a.{~128 191
'domain error' -: 7 u: etx a.{~192 191
'domain error' -: 7 u: etx a.{~224 191
'domain error' -: 7 u: etx a.{~224 159 191
'domain error' -: 7 u: etx a.{~248 191
'domain error' -: 7 u: etx a.{~128 128 191
'domain error' -: 7 u: etx a.{~192 128 191
'domain error' -: 7 u: etx a.{~224 128 191
'domain error' -: 7 u: etx a.{~248 128 191
'domain error' -: 7 u: etx a.{~240 143 191 191

NB. invalid utf-16 surrogate
237 159 191 -: 3 u: 8 u: u: 16bd7ff
238 128 128 -: 3 u: 8 u: u: 16be000
'domain error' -: 8 u: etx u: 16bd800
'domain error' -: 8 u: etx u: 16bdfff

'domain error' -: 8 u: etx u: 16bd800 16bd7ff
'domain error' -: 8 u: etx u: 16bd800 16be000
'domain error' -: 8 u: etx u: 16bdbff 16bd7ff
'domain error' -: 8 u: etx u: 16bdbff 16be000

'domain error' -: 8 u: etx u: 16bd800 16bd800
'domain error' -: 8 u: etx u: 16bd800 16bdbff
'domain error' -: 8 u: etx u: 16bdbff 16bd800
'domain error' -: 8 u: etx u: 16bdbff 16bdbff

'domain error' -: 8 u: etx u: 16bdc00 16bd800
'domain error' -: 8 u: etx u: 16bdc00 16bdbff
'domain error' -: 8 u: etx u: 16bdfff 16bd800
'domain error' -: 8 u: etx u: 16bdfff 16bdbff

'domain error' -: 8 u: etx u: 16bd800 16bdfff 16bd800
'domain error' -: 8 u: etx u: 16bd800 16bdfff 16bdfff

NB. non-bmp pane
NB. U+10000 (utf-8 f0 90 80 80)
240 144 128 128 -: 3 u: 8 u: u: 16bd800 16bdc00
NB. U+10ffff (utf-8 f4 8f bf bf)
244 143 191 191 -: 3 u: 8 u: u: 16bdbff 16bdfff
NB. U+110000 (utf-8 f4 90 80 80) invalid
'domain error' -: 7 u: etx a.{~244 144 128 128

NB. U+12345
55304 57157 -: 3&u: u: 16bd808 16bdf45
240 146 141 133 -: 3&u: 8&u: u: 16bd808 16bdf45
(7 u: a.{~240 146 141 133) -: u: 16bd808 16bdf45

NB. errors in various primitives ----------------------------------------

<       domerr   2
<.      domerr 1 2
<:      domerr 1 2
>       domerr   2
>.      domerr 1 2
>:      domerr 1 2
+       domerr 1 2
+.      domerr 1 2
+:      domerr 1 2
*       domerr 1 2
*.      domerr 1 2
*:      domerr 1 2
-       domerr 1 2
-.      domerr 1
-:      domerr 1
%       domerr 1 2
%.      domerr 1 2
%:      domerr 1 2
^       domerr 1 2
^.      domerr 1 2
$       domerr 'l'
$.      domerr 1 2
|       domerr 1 2
|.      domerr 'l'
|:      domerr 'l'
+/ .*   domerr 1 2
<;.1    domerr 'l'
#.      domerr 1 2
#:      domerr 1 2
!       domerr 1 2
]\      domerr 'l'
]\.     domerr 'l'
{       domerr 'l'
{.      domerr 'l'
}.      domerr 'l'
?       domerr 1 2
?.      domerr 1 2
? @$    domerr   2
? @#    domerr   2
?.@$    domerr   2
?.@#    domerr   2
i.      domerr 1
i:      domerr 1
j.      domerr 1 2
o.      domerr 1 2
p.      domerr 1 2
p:      domerr 1
q:      domerr 1 2
r.      domerr 1 2
x:      domerr 1 2


NB. = -------------------------------------------------------------------

x=: u: i=: t{~?2 3 41$#t=: ?19$65536
y=: u: j=: t{~?2 3 41$#t
z=: u: k=: t{~?2 3   $#t

(=x) -: =i
(=y) -: =j
(=z) -: =k

(x =   y  ) -: i =   j
(x =   z  ) -: i =   k
(x =/  z  ) -: i =/  k
(x ="2 {.y) -: i ="2 {.j

x=: u: y=: a.{~?2 3 4$#a.
UTYPE -: type x
2     -: type y
*./@, x = y


NB. < -------------------------------------------------------------------

x=: u: a.{~?257##a.
x -: ><   x
x -: ><"0 x


NB. > -------------------------------------------------------------------

x=: u: i=: t{~?2 3 41$#t=: ?19$65536
y=: u: j=: t{~?2 3 41$#t
z=: a.{~ ? 2 3$#a.

(>x;y) -: u: >i;j
(>x;z) -: x ,: ($x){.,:z

'domain error' -: > etx x;0 1 0
'domain error' -: > etx x;1 2 3
'domain error' -: > etx x;1 2.3
'domain error' -: > etx x;1 2j3
'domain error' -: > etx x;1 2 3x
'domain error' -: > etx x;1 2r3
'domain error' -: > etx x;<<2 3


NB. -. ------------------------------------------------------------------

x=: u: i=: t{~?41$#t=: ?41 7 3$65536
y=: u: j=: t{~?23$#t
(x -. y) -: u: i -. j

x=: u: i=: t{~?41$#t=: a.{~ ?41 7 3$256
y=: u: j=: t{~?41$#t

(x -.   y) -: u: i -.   j
(x -.   y) -: u: i -.   y
(x -.   y) -: u: x -.   j

(x -."1 y) -: u: i -."1 j
(x -."1 y) -: u: i -."1 y
(x -."1 y) -: u: x -."1 j

(x -."2 y) -: u: i -."2 j
(x -."2 y) -: u: i -."2 y
(x -."2 y) -: u: x -."2 j


NB. -: ------------------------------------------------------------------

x=: u: y=: a.{~?317##a.
x -: ,x
x -: y


NB. $ --------------------------------------------------------------------

(31     $ u: i) -: u: 31     $ i=: ?7$65536
(31 4   $ u: i) -: u: 31 4   $ i=: ?7$65536
(31 4 5 $ u: i) -: u: 31 4 5 $ i=: ?7$65536


NB. ~ --------------------------------------------------------------------

f=: +/
45 -: (u: 'f')~ i.10


NB. ~. -------------------------------------------------------------------

(~. u: i) -: u: ~. i=: t{~?541    $#t=: ?97$65536
(~. u: i) -: u: ~. i=: t{~?541   3$#t=: ?13$65536
(~. u: i) -: u: ~. i=: t{~?541 2 3$#t=: ? 5$65536

(~.t) -: ~.t,u:   t=: a.{~?100  $#a.
(~.t) -: ~.t,u:   t=: a.{~?200 3$#a.

(~.t) -: ~.t,u:&.>t=: <"_1 a.{~   ?100  $#a.
(~.t) -: ~.t,u:&.>t=: <"_1 a.{~97+?200 3$3


NB. ~: ------------------------------------------------------------------

x=: u: i=: t{~?2 3 41$#t=: ?19$65536
y=: u: j=: t{~?2 3 41$#t
z=: u: k=: t{~?2 3   $#t

(x ~:   y  ) -: i ~:   j
(x ~:   z  ) -: i ~:   k
(x ~:/  z  ) -: i ~:/  k
(x ~:"2 {.y) -: i ~:"2 {.j

x=: u: y=: a.{~?2 3 4$#a.
UTYPE -: type x
2     -: type y
-. +./@, x ~: y


NB. |. -------------------------------------------------------------------

x=: u: i=: t{~?41 2 3$#t=: ?119$65536
(|.   x) -: u: |.   i
(|."1 x) -: u: |."1 i
(|."2 x) -: u: |."2 i

(j|.   x) -: u: j|.   i [ j=: ?0{$x
(j|."1 x) -: u: j|."1 i [ j=: ?1{$x
(j|."2 x) -: u: j|."2 i [ j=: ?2{$x


NB. |: -------------------------------------------------------------------

x=: u: i=: t{~?2 3 5 7 11$#t=: ?119$65536
(|:   x) -: u: |:   i
(|:"1 x) -: u: |:"1 i
(|:"2 x) -: u: |:"2 i

(j|:x) -: u: j|:i [ j=: ?~#$x
(j|:x) -: u: j|:i [ j=: ?~#$x
(j|:x) -: u: j|:i [ j=: ?~#$x

(j|:x) -: u: j|:i [ j=: C. ?~#$x
(j|:x) -: u: j|:i [ j=: C. ?~#$x
(j|:x) -: u: j|:i [ j=: C. ?~#$x


NB. : -------------------------------------------------------------------

x -: 0 : x=: 'super duper stare decisis'

f=: 1 : (u: 'u/')
45 -: +f i.10

f=: 2 : (u: 'u&.:v')
5 -: +/ f *: 3 4

f=: 3 : (u: '(* f@<:)^:(1<y) 1>.y')
1 1 120 720 -: f"0 ]0 1 5 6

f=: 3 : (u:&.> 't=.y+1';'t*t')
25 -: f 4

f=: 4 : (u: 'x+y')
45 -: f/ i.10

f=: 13 : (u: 'x*y')
120 -: f/1+i.5


NB. ;. ------------------------------------------------------------------

x=: ' boustrophedonic triskaidekaphobia deipnosophist chthonic'
(<   ;.1 u: x) -: u:&.>    <;.1 x
([   ;.1 u: x) -: u:       [;.1 x
(,   ;.1 u: x) -: u:       ,;.1 x
(#   ;.1 u: x) -:          #;.1 x
(<@}.;.1 u: x) -: u:&.> <@}.;.1 x
(<@}:;.1 u: x) -: u:&.> <@}:;.1 x


NB. ;: ------------------------------------------------------------------

(;: -: ;:@u:) '3+4'
(;: -: ;:@u:) '+'

0 $ 0 : 0
sq=: 4 2 2$ 1 1 2 1  1 0 2 2  2 0 3 0  1 2 2 0
x=: 0;sq;''''=a.
(x&;: -: x&;:@u:) '''The Power of the Powerlesss'' by Havel'
)

'domain error' -: ;: etx (u: 257),'3+4'


NB. 0!: -----------------------------------------------------------------

0!:0 u: 'x=: i.2 3 4'
x -: i.2 3 4


NB. 1!: -----------------------------------------------------------------

(1!:0 '*.*') -: 1!:0 u: '*.*'
x=: ' triskaidekaphobia deipnosophist boustrophedonic octothorpe'
(8 u: u: x) 1!:2 <8 u: u: 'sdfa'
x -: 1!:1 <8 u: u: 'sdfa'

x=: u: ?237$65536
x 1!:2 <'sdfa'
x -: 6 u: 1!:1 <'sdfa'
x -: 6 u: 1!:1 <8 u: u: 'sdfa'

x=: 'everything not forbidden is mandatory'
x 1!:2 <8 u: u: 'sdfa'
x -: 1!:1 <8 u: u: 'sdfa'
'01234' 1!:3 <8 u: 'sdfa'
(x,'01234') -: 1!:1 <8 u: u: 'sdfa'
(5+#x) -: 1!:4 <8 u: u: 'sdfa'

3 : 0 ''
 if. (9!:12 '') e. 6 do.  NB. do only if PC
  assert. (1!:6@< -: 1!:6@<@(8&u:)@u:) 'sdfa'
  assert. (1!:6 <'sdfa') 1!:6 <8 u: u: 'sdfa'
  assert. (1!:7@< -: 1!:7@<@(8&u:)@u:) 'sdfa'
  assert. (1!:7 <'sdfa') -: 1!:7 <8 u: u: 'sdfa'
 end.
 1
)

'01234' -: 1!:11 (8 u: 'sdfa');(#x),5
'abcde' 1!:12 (8 u: 'sdfa');#x
'abcde' -: 1!:11 (8 u: 'sdfa');(#x),5

t=: 1!:21 <8 u: u: 'sdfa'
+./@('sdfa'&E.)&> {:"1 (1!:20) ''
1!:22 t

1!:55 <8 u: u: 'sdfa'

NB. unicode

(1!:0 '*.*') -: 1!:0 u: '*.*'
x=: ' triskaidekaphobia deipnosophist boustrophedonic octothorpe'
(8 u: u: x) 1!:2 <7 u: 10 u: 100000 + a.i.'sdfa'
x -: 1!:1 <7 u: 10 u: 100000 + a.i.'sdfa'

x=: u: ?237$65536
x 1!:2 <7 u: 10 u: 100000 + a.i.'sdfa'
x -: 6 u: 1!:1 <7 u: 10 u: 100000 + a.i.'sdfa'
x -: 6 u: 1!:1 <8 u: 10 u: 100000 + a.i.'sdfa'

x=: 'everything not forbidden is mandatory'
x 1!:2 <7 u: 10 u: 100000 + a.i.'sdfa'
x -: 1!:1 <7 u: 10 u: 100000 + a.i.'sdfa'
'01234' 1!:3 <7 u: 10 u: 100000 + a.i.'sdfa'
(x,'01234') -: 1!:1 <7 u: 10 u: 100000 + a.i.'sdfa'
(5+#x) -: 1!:4 <7 u: 10 u: 100000 + a.i.'sdfa'

3 : 0 ''
 if. (9!:12 '') e. 6 do.  NB. do only if PC
  assert. (1!:6@< -: 1!:6@<@(8&u:)@u:) 7 u: 10 u: 100000 + a.i.'sdfa'
  assert. (1!:6 <8 u: 10 u: 100000 + a.i.'sdfa') 1!:6 <7 u: 10 u: 100000 + a.i.'sdfa'
  assert. (1!:7@< -: 1!:7@<@(8&u:)@u:) 7 u: 10 u: 100000 + a.i.'sdfa'
  assert. (1!:7 <8 u: 10 u: 100000 + a.i.'sdfa') -: 1!:7 <7 u: 10 u: 100000 + a.i.'sdfa'
 end.
 1
)

'01234' -: 1!:11 (7 u: 10 u: 100000 + a.i. 'sdfa');(#x),5
'abcde' 1!:12 (8 u: 10 u: 100000 + a.i. 'sdfa');#x
'abcde' -: 1!:11 (7 u: 10 u: 100000 + a.i. 'sdfa');(#x),5

t=: 1!:21 <7 u: 10 u: 100000 + a.i. 'sdfa'
+./@((8 u: 10 u: 100000 + a.i. 'sdfa')&E.)&> {:"1 (1!:20) ''
1!:22 t

1!:55 <7 u: 10 u: 100000 + a.i. 'sdfa'

NB. 3!:n ----------------------------------------------------------------

UTYPE -: 3!:0 u: 'abc'

f=: 3 : 0
 assert. y -: (3!:2) 00 (3!:1) y
 assert. y -: (3!:2) 01 (3!:1) y
 assert. y -: (3!:2) 10 (3!:1) y
 assert. y -: (3!:2) 11 (3!:1) y
 assert. y -: (3!:2) 00 (3!:3) y
 assert. y -: (3!:2) 01 (3!:3) y
 assert. y -: (3!:2) 10 (3!:3) y
 assert. y -: (3!:2) 11 (3!:3) y
 1
)

f x=: u: 0
f x=: u: 65535
f x=: u: ?257      $65536
f x=: u: ?257 2    $65536
f x=: u: ?257 2 3  $65536
f x=: u: ?257 2 3 5$65536
f x=: u: ?257 2147483647 2e9 2e9 2e9 0$65536


NB. /: ------------------------------------------------------------------

(/:   i) -: /:   x=: u: i=: ?  313 1$65536
(/:   i) -: /:   x=: u: i=: ?  313 2$65536
(/:   i) -: /:   x=: u: i=: ?  313 3$65536
(/:   i) -: /:   x=: u: i=: ?  313 4$65536
(/:   i) -: /:   x=: u: i=: ?  313 5$65536
(/:   i) -: /:   x=: u: i=: ?  313 6$65536

(/:"2 i) -: /:"2 x=: u: i=: ?7 313 1$65536
(/:"2 i) -: /:"2 x=: u: i=: ?7 313 2$65536
(/:"2 i) -: /:"2 x=: u: i=: ?7 313 3$65536
(/:"2 i) -: /:"2 x=: u: i=: ?7 313 4$65536
(/:"2 i) -: /:"2 x=: u: i=: ?7 313 5$65536
(/:"2 i) -: /:"2 x=: u: i=: ?7 313 6$65536

(/:   i) -: /:   x=: u: i=: ?13 1231$65536
(/:   i) -: /:   x=: u: i=: ?13 1232$65536
(/:   i) -: /:   x=: u: i=: ?13 1233$65536
(/:   i) -: /:   x=: u: i=: ?13 1234$65536
(/:   i) -: /:   x=: u: i=: ?13 1235$65536
(/:   i) -: /:   x=: u: i=: ?13 1236$65536

(/:"1 i) -: /:"1 x=: u: i=: ?13 1231$65536
(/:"1 i) -: /:"1 x=: u: i=: ?13 1232$65536
(/:"1 i) -: /:"1 x=: u: i=: ?13 1233$65536
(/:"1 i) -: /:"1 x=: u: i=: ?13 1234$65536
(/:"1 i) -: /:"1 x=: u: i=: ?13 1235$65536
(/:"1 i) -: /:"1 x=: u: i=: ?13 1236$65536


NB. \: ------------------------------------------------------------------

(\:   i) -: \:   x=: u: i=: ?  313 1$65536
(\:   i) -: \:   x=: u: i=: ?  313 2$65536
(\:   i) -: \:   x=: u: i=: ?  313 3$65536
(\:   i) -: \:   x=: u: i=: ?  313 4$65536
(\:   i) -: \:   x=: u: i=: ?  313 5$65536
(\:   i) -: \:   x=: u: i=: ?  313 6$65536

(\:"2 i) -: \:"2 x=: u: i=: ?7 313 1$65536
(\:"2 i) -: \:"2 x=: u: i=: ?7 313 2$65536
(\:"2 i) -: \:"2 x=: u: i=: ?7 313 3$65536
(\:"2 i) -: \:"2 x=: u: i=: ?7 313 4$65536
(\:"2 i) -: \:"2 x=: u: i=: ?7 313 5$65536
(\:"2 i) -: \:"2 x=: u: i=: ?7 313 6$65536

(\:   i) -: \:   x=: u: i=: ?13 1231$65536
(\:   i) -: \:   x=: u: i=: ?13 1232$65536
(\:   i) -: \:   x=: u: i=: ?13 1233$65536
(\:   i) -: \:   x=: u: i=: ?13 1234$65536
(\:   i) -: \:   x=: u: i=: ?13 1235$65536
(\:   i) -: \:   x=: u: i=: ?13 1236$65536

(\:"1 i) -: \:"1 x=: u: i=: ?13 1231$65536
(\:"1 i) -: \:"1 x=: u: i=: ?13 1232$65536
(\:"1 i) -: \:"1 x=: u: i=: ?13 1233$65536
(\:"1 i) -: \:"1 x=: u: i=: ?13 1234$65536
(\:"1 i) -: \:"1 x=: u: i=: ?13 1235$65536
(\:"1 i) -: \:"1 x=: u: i=: ?13 1236$65536


NB. ". ------------------------------------------------------------------

3 4 5 -:    ". u: '3 4 5'
3 4 5 -: _1 ". u: '3 4 5'


NB. E. ------------------------------------------------------------------

x=:                           a.{~(a.i.'a')+?  31$26
y=: x (<"0(?19$y-&#x)+/i.#x)}y=: a.{~(a.i.'a')+?3111$26
(x E. y) -: x E.&u: y

x=:                           ?  31$65536
y=: x (<"0(?19$y-&#x)+/i.#x)}y=: ?9111$65536
(x E. y) -: x E.&u: y


NB. i. ------------------------------------------------------------------

x=: u: i=: t{~?257$#t=: ?19$65536
y=: u: j=: t{~?521$#t
(x i. y) -: i i. j
(y i. x) -: j i. i

x=: u: i=: t{~?257$#t=: a.
y=: u: j=: t{~?521$#t
(x i. y) -: i i. j
(y i. x) -: j i. i

x=: u: i=: t{~?257 2 3$#t=: ?7$65536
y=: u: j=: t{~?521 2 3$#t
(x i. y) -: i i. j
(y i. x) -: j i. i

x=: u: i=: t{~?257 2 3$#t=: a.
y=: u: j=: t{~?521 2 3$#t
(x i. y) -: i i. j
(y i. x) -: j i. i

x=: a.{~97+?10$26
y=: u: x
(x i. 1{x) -: y i. 1{y
(x i. 'x') -: y i. u: 'x'
(x i. '9') -: y i. u: '9'

(t i.&:> t,t) -: t        i. t, u:&.>t=: <"_1 a.{~   ?100  $#a.
(t i.&:> t,t) -: t        i. t,~u:&.>t
(t i.&:> t,t) -: (u:&.>t) i. t, u:&.>t
(t i.&:> t,t) -: (u:&.>t) i. t,~u:&.>t

(t i.&:> t,t) -: t        i. t, u:&.>t=: <"_1 a.{~97+?100 3$3
(t i.&:> t,t) -: t        i. t,~u:&.>t
(t i.&:> t,t) -: (u:&.>t) i. t, u:&.>t
(t i.&:> t,t) -: (u:&.>t) i. t,~u:&.>t

f=: 3 : 'a.{~ ?(y,6)$#a.  '
g=: 3 : 'u:   ?(y,6)$65536'


NB. i: ------------------------------------------------------------------

x=: u: i=: t{~?257$#t=: ?19$65536
y=: u: j=: t{~?521$#t
(x i: y) -: i i: j
(y i: x) -: j i: i

x=: u: i=: t{~?257$#t=: a.
y=: u: j=: t{~?521$#t
(x i: y) -: i i: j
(y i: x) -: j i: i

x=: u: i=: t{~?257 2 3$#t=: ?7$65536
y=: u: j=: t{~?521 2 3$#t
(x i: y) -: i i: j
(y i: x) -: j i: i

x=: u: i=: t{~?257 2 3$#t=: a.
y=: u: j=: t{~?521 2 3$#t
(x i: y) -: i i: j
(y i: x) -: j i: i

x=: a.{~97+?10$26
y=: u: x
(x i: 1{x) -: y i: 1{y
(x i: 'x') -: y i: u: 'x'
(x i: '9') -: y i: u: '9'

(t i:&:> t,t) -: t        i: t, u:&.>t=: <"_1 a.{~   ?100  $#a.
(t i:&:> t,t) -: t        i: t,~u:&.>t
(t i:&:> t,t) -: (u:&.>t) i: t, u:&.>t
(t i:&:> t,t) -: (u:&.>t) i: t,~u:&.>t

(t i:&:> t,t) -: t        i: t, u:&.>t=: <"_1 a.{~97+?100 3$3
(t i:&:> t,t) -: t        i: t,~u:&.>t
(t i:&:> t,t) -: (u:&.>t) i: t, u:&.>t
(t i:&:> t,t) -: (u:&.>t) i: t,~u:&.>t


4!:55 ;:'domerr f g i j k lr sq t UTYPE x y z'



epilog''

