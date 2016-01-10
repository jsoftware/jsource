NB. 13!: ----------------------------------------------------------------

13!:0 [1

foo       =: foo_loc1_
foo_loc1_ =: foo_loc2_ /
foo_loc2_ =: foo_loc3_ ~
foo_loc3_ =: +

commute =: ~

goo =: 4 : 0
 p=.2*x
 q=.p goo1 commute y
 q*q
)

goo1 =: goo2
goo2 =: goo3
goo3 =: +

fac =: 3 : 0
 if.     2>y do. 1 
 elseif. 2=y do. %'abc'
 elseif. 1   do. y*fac y-1 
 end.
)

conj =: 2 : 0
 if. 1=x do. y=.*:@y else. y=.%:@y end.
 %glob
 y/
)

h  =: 3 : 'y h1 2'
h1 =: 4 : 'x+y'

f1 =: 3 : '+/'
f2 =: 3 : '('

13!:0 [1
'domain error' -:   (3 : '%y' ) etx 'asdf'
'domain error' -: % (1 : 'u y') etx 'asdf'
13!:0 [0

f =: % 1 : 'u y'
'domain error' -: f etx 'asdf'

f=: 3 : 'try. 13!:8 y catch. 13!:11 $0 end.'

254 -: f 254
255 -: f 255
10  -: f 256

13!:8 :: 1: x=: ?256
x -: 13!:11 ''

'length error' -: 13!:11 etx 'junkfoo'
'length error' -: 13!:12 etx 'junkfoo'


NB. stops ---------------------------------------------------------------

'' -: 13!:2 ''
1 [ 13!:3 'sum *'
'sum *' -: 13!:2 ''
1 [ 13!:3 ''


NB. error text ----------------------------------------------------------

sum=: +/
f=: 3 : '2 3+g y'
g=: 3 : 'if. y do. y else. + end.'
h=: 2 3&+@sum

'value error: junkfoo' -: fex 'junkfoo'

'length error: fex'    -: fex '2 3+4 5 6'
'domain error: fex'    -: fex '+/1;2 3'
'domain error: sum'    -: fex 'sum ''asdf'''
'syntax error: g'      -: fex 'f 0'
'length error: f'      -: fex 'f 2 3 4'
'domain error: sum'    -: fex 'h ''asdf'''
'length error: h'      -: fex 'h i.3 4'

'length error: fex'    -: fex '2 3+4 5 6'

f=: 3 : 0
 abc=. 'abc'
 ". :: 0: 'abc,',y
 13!:12 ''
)

' ...' -: _4{. }: f 204 $'123 '
' ...' -: _4{. }: f 204 $'1.3 '
' ...' -: _4{. }: f 204 $'1j3 '
' ...' -: _4{. }: f 221 $(16$'123'),'x '
' ...' -: _4{. }: f 221 $' 1r',14$'1123'

18!:55 ;:'loc1 loc2 loc3'


NB. 13!:13  -------------------------------------------------------------

mean=: sum % #
sum =: [: +/ ".@('t=:13!:13 $0'&[) ] ]

13!:0 ]1
1: mean x=: ?4 5$100

($t) -: 2 9
(0{"1 t) -: 'sum';'mean'          NB. name
(1{"1 t) -: 0;0                   NB. error number
(2{"1 t) -: 0;0                   NB. line number
(3{"1 t) -: 3;3                   NB. name class
((<1 4){t) -: <'sum % #'          NB. definition
(_8{.&.>5{"1 t) -: 2$<'g13x.ijs'  NB. defining scripts
(6{"1 t) -: (<,<x),<,<x           NB. boxed argument(s)
(7{"1 t) -: 2$<0 2$0              NB. locals
(8{"1 t) e. ' ';'*'               NB. * if begins suspension

1: mean"1 x

($t) -: 2 9
(0{"1 t) -: 'sum';'mean'          NB. name
(1{"1 t) -: 0;0                   NB. error number
(2{"1 t) -: 0;0                   NB. line number
(3{"1 t) -: 3;3                   NB. name class
((<1 4){t) -: <'sum % #'          NB. definition
(_8{.&.>5{"1 t) -: 2$<'g13x.ijs'  NB. defining scripts
(6{"1 t) -: (<,<{:x),<,<{:x       NB. boxed argument(s)
(7{"1 t) -: 2$<0 2$a:             NB. locals
(8{"1 t) e. ' ';'*'               NB. * if begins suspension

sum=: 3 : ('z=.+/y';'t=: 13!:13 $0';'z')

1: mean"1 x

($t) -: 2 9
(0{"1 t) -: 'sum';'mean'          NB. name
(1{"1 t) -: 0;0                   NB. error number
(2{"1 t) -: 1;0                   NB. line number
(3{"1 t) -: 3;3                   NB. name class
((<1 4){t) -: <'sum % #'          NB. definition
(_8{.&.>5{"1 t) -: 2$<'g13x.ijs'  NB. defining scripts
(6{"1 t) -: (<,<{:x),<,<{:x       NB. boxed argument(s)
NB. (7{"1 t) -: (('y.';{:x),:(,'z');+/{:x);<0 2$a:  NB. locals
(>(<0 7){t) e. ('y.';{:x), ((,'y');{:x),:(,'z');+/{:x  NB. locals
(8{"1 t) e. ' ';'*'               NB. * if begins suspension

13!:0 ]0


NB. 13!:18  -------------------------------------------------------------

mean=: sum % #
sum=: 3 : ('z=.+/y';'t=: 13!:18 $0';'z')

13!:0 ]1
1: mean i.12
NB. (2{.t) -: (2,{:$t){.];.1 '|sum[1]|mean[0] !'

13!:0 ]0


4!:55 ;:'commute conj f f1 f2 fac foo '
4!:55 ;:'g goo goo1 goo2 goo3 h h1 mean sum t x '


