NB. D. scalar functions -------------------------------------------------

D =: D. 1
E =: 1 : '3 : (x,'' y'') D'

test =. 2 : 0
 f=: x D
 g=: y E
 h=: $@f -: $@g
 b=.$0
 b=.b,1e_4>|(f-g)2+0.01*?50
 b=.b,h t=: 0.0001*2+?    4$50000
 b=.b,h t=: 0.0001*2+?  3 2$50000
 b=.b,h t=: 0.0001*2+?2 3 1$50000
)

<:        test '<:'
>:        test '>:'
+:        test '+:'
*:        test '*:'
-         test '- '
-.        test '-.'
%         test '% '
%:        test '%:'
^         test '^ '
^.        test '^.'
[         test '['
]         test ']'
j.        test 'j.'
o.        test 'o.'
r.        test 'r.'

3&+       test '3&+'
+&3       test '+&3'
3&*       test '3&*'
*&3       test '*&3'
3&-       test '3&-'
-&3       test '-&3'
3&%       test '3&%'
%&3       test '%&3'
1.2&^     test '1.2&^'
^&3       test '^&3'
3&^.      test '3&^.'
^.&3      test '^.&3'
1&o.      test '1&o.'
2&o.      test '2&o.'
3&o.      test '3&o.'
5&o.      test '5&o.'
6&o.      test '6&o.'
7&o.      test '7&o.'

12"_      test '12"_'
_17"_     test '_17"_'
*:"1      test '*:"1'
4&%"1     test '4&%"1'
^&1.7"2   test '^&1.7"2'

*:@>:     test '*:@>:'
<:@+:     test '<:@+:'
^.@+:     test '^.@+: '
*:@(1&o.) test '*:@(1&o.)'

(*:   + ] )     test '(*:   + ] )'
([    * ^ )     test '([    * ^ )'
(2&o. - %:)     test '(2&o. - %:)'
(-:   % ^.)     test '(-:   % ^.)'
(*: + +: + 1"0) test '(*: + +: + 1"0)'
(*: % +: + 1"0) test '(*: % +: + 1"0)'
(*:@>: * 2&*)   test '(*:@>: * 2&*)'

'length error' -: 1 2 3&* D etx i.7
'length error' -: 1 2 3&* D etx i.7 3


NB. D. non-scalar functions ---------------------------------------------

eq =: 2 : '''x'' -:&(5!:1)&< ''y'''
dc =: }.@(* i.@#)

x=.(>:?10),~_5+?5$20
(x&p. d. 1)    eq ((dc    x)&p.)
(x&p. d. 2)    eq ((dc^:2 x)&p.)
(x&p. d. 3)    eq ((dc^:3 x)&p.)
(x&p. d. 4)    eq ((dc^:4 x)&p.)
(x&p. d. 0)    eq ((dc^:0 x)&p."0)
(x&p. d. 0)    eq (x&p."0)
x=.j./_5+?2 5$20
((<x)&p. d. 1) eq ((dc p.<x)&p.)

_0.5 1&p. eq (2&! d. 1)

((10"_ * %:) D. 1) eq ((10 * %:) D. 1)


4!:55 ;:'D E b dc eq f g h test x '


