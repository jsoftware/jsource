NB. 5!:1 ----------------------------------------------------------------

ar  =: 5!:1
dr  =: 5!:2
nar =: <&((<,'0')&,)&<
mtv =: $0

(ar <'a') -: nar a =: 1=?10$2
(ar <'a') -: nar a =: 'Cogito, ergo sum.'
(ar <'a') -: nar a =: ?3 4$10
(ar <'a') -: nar a =: 3.14159265358979
(ar <'a') -: nar a =: ^0j1*?3 4$10
(ar <'a') -: nar a =: +&.>?10$25

(ar <'a') -: nar a =: 0 3 4 $ 1=?10$2
(ar <'a') -: nar a =: 4 0   $ 'Cogito, ergo sum.'
(ar <'a') -: nar a =: 0     $ ?3 4$10
(ar <'a') -: nar a =: 4 0 5 $ 3.14159265358979
(ar <'a') -: nar a =: 0 0   $ ^0j1*?3 4$10
(ar <'a') -: nar a =: 0 0 0 $ +&.>?10$25

f=: ^
(ar <'f') -: <,'^' 
f=: /.
(ar <'f') -: <'/.'
f=: &.
(ar <'f') -: <'&.'

plus =: +
or   =: +.
over =: ,
hook =: (plus or) f.
fork =: (plus,or) f.
(ar <'plus') -: <,'+'
(ar <'or'  ) -: <'+.'
(ar <'over') -: <,','
(ar <'hook') -: <(,'2');<ar ;:'plus      or'
(ar <'fork') -: <(,'3');<ar ;:'plus over or'

f   =: *
g   =: +
a   =: >'*y'; ':'; 'x*y'
n3  =: 3
gd  =: :
fnn =: 3 : a
fvv =: * : +
(ar <'gd' ) -: <(,':')
(ar <'fnn') -: <(,':');<ar ;:'n3 a'
(ar <'fvv') -: <(,':');<ar ;:'f  g'

one  =: 1
two  =: 2
adv  =: 1 : a=:,:'x/'
conj =: 2 : c=:,:'x&y'
(ar <'adv' ) -: <(,':');<ar ;:'one a'
(ar <'conj') -: <(,':');<ar ;:'two c'

tv  =: 2 : '(ar <''x'') -: <,y'

=  tv '='
<  tv '<'
<. tv '<.'
<: tv '<:'
>  tv '>'
>. tv '>.'
>: tv '>:'
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
/: tv '/:'
\: tv '\:'
[  tv '['
]  tv ']'
{  tv '{'
{. tv '{.'
}. tv '}.'
". tv '".'
": tv '":'
!  tv '!'
?  tv '?'
[  tv '['
]  tv ']'
A. tv 'A.'
C. tv 'C.'
E. tv 'E.'
e. tv 'e.'
i. tv 'i.'
j. tv 'j.'
o. tv 'o.'
r. tv 'r.'
0: tv '0:'
1: tv '1:'

f=: ~
(<,'~') -: ar <'f'
f=: /
(<,'/') -: ar <'f'
f=: /.
(<'/.') -: ar <'f'
f=: \
(<,'\') -: ar <'f'
f=: \.
(<'\.') -: ar <'f'
f=: }
(<,'}') -: ar <'f'

f=: ^:
(<'^:') -: ar <'f'
f=: `
(<,'`') -: ar <'f'
f=: `:
(<'`:') -: ar <'f'
f=: .
(<,'.') -: ar <'f'
f=: :.
(<':.') -: ar <'f'
f=: :
(<,':') -: ar <'f'
f=: ;.
(<';.') -: ar <'f'
f=: @
(<,'@') -: ar <'f'
f=: "
(<,'"') -: ar <'f'
f=: &
(<,'&') -: ar <'f'
f=: &.
(<'&.') -: ar <'f'
f=: !.
(<'!.') -: ar <'f'

x=:y=:f=:+/
-:&ar / ;:'f x'
-:&ar / ;:'f y'

x=:y=:f=:/
-:&ar / ;:'f x'
-:&ar / ;:'f y'

x=:y=:f=:&
-:&ar / ;:'f x'
-:&ar / ;:'f y'

4!:55 ;:'a adv ar c conj dr f fnn fork fvv g gd hook mtv n3 nar '
4!:55 ;:'one or over plus tv two x y'


