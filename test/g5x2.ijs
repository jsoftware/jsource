prolog './g5x2.ijs'
NB. 5!:2 ----------------------------------------------------------------

ar     =: 5!:1
boxed  =: 32&=@(3!:0)
oarg   =: >@(1&{)

bxroot =: (<1 0)&C.@,`] @. (e.&(,&.>'0123456789')@[)

bxx    =: {. bxroot bx&.>@oarg
bxgl   =: {. bxroot (bxx&.>@{. , bx &.>@}.)@oarg
bxgr   =: {. bxroot (bx &.>@{. , bxx&.>@}.)@oarg
bxg    =: bxgr`bxgl`bxx @. (i.&(<,'`')@oarg)
bxtil  =: bxx`(oarg@>@{.@oarg) @. ((<,'0')&=@{.@>@{.@oarg)
bxcase =: oarg`bxgl`bxgl`bxg`bxtil`bxx @. ((;:'0@.`:4~')&i.@{.)
bx     =: ]`bxcase @. boxed

brep   =: ,@<`[ @. boxed @ bx @ > @ ar

br     =: 5!:2
test1  =: (br -: brep) " 0
*./ test1 ;:'ar bx boxed oarg'
*./ test1 ;:'bxroot bxx bxgl bxgr bxg bxtil bxcase bx'

test =: 1 : '(br -: brep) <''u''' 

+   test
+.  test
i.  test
0:  test
-   test
+ t. '' test

1 2 3&+      test
,&'abcd'     test
(<"0 i.7)&e. test

(+/ % #)     test
(+%)         test

+/           test
+./          test
+./ .*       test
+/  .*       test
+`*@.<       test
+`-`*`:0     test
brep         test
bxcase       test

(br -: brep) <'ger' [ ger =: +/`%`#
(br -: brep) <'a'   [ a =:<"0 i.7
(br -: brep) <'a'   [ a =: ''

(br -: brep) <'test'
a =: /
(br -: brep) <'a'
a =: 1 : 'u/\'
(br -: brep) <'a'
inv =: 1 : 'u^:_'
(br -: brep) <'inv'

c =: &
(br -: brep) <'c'
ip =: 2 : 'u@ (v"(0 _1"1 _))'
(br -: brep) <'ip'

a =: i.3 4
(br <'a') -: ,<a
f =: ^
(br <'f') -: ,<,'^'
f =: /.
(br <'f') -: ,<'/.'
f =: &.
(br <'f') -: ,<'&.'

plus =: +
or   =: +.
hook =: (plus or) f.
fork =: (plus,or) f.
(br <'plus') -: ,<,'+'
(br <'or'  ) -: ,<'+.'
(br <'hook') -: ;:'++.'
(br <'fork') -: ;:'+,+.'

gd  =: :
f1  =: 3 : n1 =: 'y+2'
f2  =: 3 : n2 =: (       ':'; 't=.x+y'; 't^2')
f12 =: 3 : n12=: ('y+2'; ':'; 't=.x+y'; 't^2')
fvv =: * : +
(br <'gd' ) -: ;:':'
(br <'f1' ) -: (3;,':'),<,:n1
(br <'f2' ) -: (3;,':'),<>n2
(br <'f12') -: (3;,':'),<>n12
(br <'fvv') -: ;:'* : +'

adv  =: 1 : 'u/f.'  
conj =: 2 : 'u&v f.' 
(br <'adv' ) -: (1;,':'),<,:'u/f.'  
(br <'conj') -: (2;,':'),<,:'u&v f.'

t=:'i=.0'; 't=.]'; 'while. v>i do.'; 'i=.1+i'; 't=.u@t f.'; 'end.'
pow =: 2 : t 
(br <'pow') -: (2;,':'),<>t

f=:o. pow 0
r=:,<,']'
(br <'f') -: r

f=:o. pow 1
r=:(<'o.'),(<,'@'),r
(br <'f') -: r

f=:o. pow 2
r=:(<'o.'),(<,'@'),<r
(br <'f') -: r

f=:o. pow 3
r=:(<'o.'),(<,'@'),<r
(br <'f') -: r

tv =: 2 : '(br <''u'') -: ,<,v' 
=  tv '='
<  tv '<'
<. tv '<.'
<: tv '<:'
>  tv '>'
>. tv '>.'
>: tv '>:'
_: tv '_:'
__: tv '__:'
+  tv '+'
+. tv '+.'
+: tv '+:'
*  tv '*'
*. tv '*.'
*: tv '*:'
-  tv '-'
-. tv '-.'
-: tv '-:'
%  tv '%'
%. tv '%.'
%: tv '%:'
^  tv '^'
^. tv '^.'
$  tv '$'
~. tv '~.'
~: tv '~:'
|  tv '|'
|. tv '|.'
|: tv '|:'
,  tv ','
,. tv ',.'
,: tv ',:'
;  tv ';'
;: tv ';:'
#  tv '#'
#. tv '#.'
#: tv '#:'
!  tv '!'
/: tv '/:'
\: tv '\:'
[  tv '['
]  tv ']'
{  tv '{'
{. tv '{.'
{: tv '{:'
}. tv '}.'
}: tv '}:'
". tv '".'
": tv '":'
?  tv '?'
A. tv 'A.'
C. tv 'C.'
E. tv 'E.'
e. tv 'e.'
i. tv 'i.'
j. tv 'j.'
o. tv 'o.'
p. tv 'p.'
r. tv 'r.'
0: tv '0:'
1: tv '1:'

f=: ~
(,<,'~' ) -: br <'f'
f=: /
(,<,'/' ) -: br <'f'
f=: /.
(,<,'/.') -: br <'f'
f=: \
(,<,'\' ) -: br <'f'
f=: \.
(,<,'\.') -: br <'f'
f=: }
(,<,'}' ) -: br <'f'
f=: b.
(,<,'b.') -: br <'f'
f=: f.
(,<,'f.') -: br <'f'

f=: ^:
(,<,'^:') -: br <'f'
f=: .
(,<,'.' ) -: br <'f'
NB. f=: ..
NB. (,<,'..') -: br <'f'
NB. f=: .:
NB. (,<,'.:') -: br <'f' 
f=: :
(,<,':' ) -: br <'f'
f=: :.
(,<,':.') -: br <'f' 
f=: ;.
(,<,';.') -: br <'f'
f=: !.
(,<,'!.') -: br <'f'
f=: !:
(,<,'!:') -: br <'f' 
f=: "
(,<,'"' ) -: br <'f'
f=: `
(,<,'`' ) -: br <'f'
f=: `:
(,<,'`:') -: br <'f'
f=: @
(,<,'@' ) -: br <'f'
f=: @.
(,<,'@.') -: br <'f' 
f=: @:
(,<,'@:') -: br <'f'
f=: &
(,<,'&' ) -: br <'f'
f=: &.
(,<,'&.') -: br <'f'
f=: &:
(,<,'&:') -: br <'f'
f=: &.:
(,<,'&.:') -: br <'f'
f=:&&
(;:'&&') -: br <'f'
f=:&+
(;:'&+') -: br <'f'
f=:+&
(;:'+&') -: br <'f'
f=:@/
(;:'@/') -: br <'f'
f=:/@
(;:'/@') -: br <'f'
f=:&&&
(;:'&&&') -: br <'f'
f=:+F.-
(;:'+F.-') -: br <'f'
f=:+F..-
(;:'+F..-') -: br <'f'
f=:+F.:-
(;:'+F.:-') -: br <'f'
f=:+F:-
(;:'+F:-') -: br <'f'
f=:+F:.-
(;:'+F:.-') -: br <'f'
f=:+F::-
(;:'+F::-') -: br <'f'


NB. 5!:2, handling gerunds ----------------------------------------------

fx =: 5!:0
br =: 5!:2
th =: 3 : ('f=.y fx'; '{.@(]`<@.(1&<@#)) br<''f''') 

g =: [`((e.&' ' # i.@#)@])`]
f =: g}
(br<'f') -: (th"0 g);,'}'
f =: (1{g) fx}
(br<'f') -: (>th 1{g);,'}'

g =: */\.`(i.@#)`(+/~)
f =: i.^:g
(br<'f') -: 'i.';'^:';<th"0 g
f =: i.^:*
(br<'f') -: ;:'i.^:*'

g =: ]`(+%)
f =: g`:0
(br<'f') -: (th"0 g);'`:';0

g =: ((%&4@# , 4:) $ ]) ` %: ` $
f =: g@.*
(br<'f') -: (th"0 g);'@.';,'*'





epilog''

