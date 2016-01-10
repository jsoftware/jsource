NB. 5!:4 ----------------------------------------------------------------

ar      =: 5!:1
lr      =: 3 : '5!:5 <''y'''
boxed   =: 32&= @ (3!:0)
mt      =: 0&e.@$
oarg    =: >@(1&{)
shr     =: |.!.''
shl     =: 1&(|.!.'')
mat     =: (1 1&}.)@(_1 _1&}.)@":@<
boxc    =: 9!:6 ''
dash    =: 10{boxc

extent  =: (+./\ *. +./\.) @ (' '&~:) @: ({."1)
limb1   =: 1&|.@$ 1&~: }. (10 6 0{boxc)&,@($&(9{boxc))
limb    =: -@(i.&1)@[ |. #@[ {. limb1@]
pfx     =: (limb +/)@extent ,. ]
pad     =: [ {. ] ,. dash&=@({:"1)@] {  ' '&,:@($&dash)@(-&{: $)
take    =: pad`($&' '@[) @. (mt@])
rc      =: #@>@{."1 ; >./@:({:@$@>)
kernt   =: (0{boxc)&=@shl@[ *. ' '&~:@]
kernb   =: (6{boxc)&=@] *. ' '&~:@shl@[
kern    =: (<0 0)&{&>"2 (kernt +./"1@:+. kernb) (<_1 0)&{&>"2
gap     =: ,&.>"_1 {&((0 1$' ');1 1$' ')@kern
graft   =: (pfx&.>@{. 0} ]) @ (,&.>/) @ gap @ ({@rc take&.> ])

lab     =: ,: @ (2&|.) @ ((' ',dash,dash,' ')&,)
label   =: lab`((,.dash)&[) @. (e.&'0123456789'@{.)
center  =: ((i.&1) -@+ <.@-:@(+/))@] |. #@] {. [
root    =: label@[ center extent@>@{.@]

leaf    =: ,@<@(((,:dash,' ')&[ center $&1@#) ,. ])@mat@":

trx     =: >@{. (root ; ]) graft@:(tr@>)@oarg
trgl    =: >@{. (root ; ]) graft@:(trx@>@{. , tr @>@}.)@oarg
trgr    =: >@{. (root ; ]) graft@:(tr @>@{. , trx@>@}.)@oarg
trg     =: trgr`trgl`trx @. (i.&(<,'`')@oarg)
trtil   =: trx`(leaf@oarg@>@{.@oarg) @. ((<,'0')&=@{.@>@{.@oarg)
trnoun  =: leaf @ lr @ oarg
trcase  =: trnoun`trgl`trgl`trg`trtil`trx @. ((;:'0@.`:4~')&i.@{.)
tr      =: leaf`trcase @. boxed

rep     =: 2 : 'x & ((# i.@#)@,@y@]})'
right   =: (5{boxc) rep (e.&(9{boxc) *. shr"1@(e.&dash))
cross   =: (4{boxc) rep (e.&(5{boxc) *. shl"1@(e.&dash))
left    =: (3{boxc) rep (e.&(9{boxc) *. shl"1@(e.&dash))
bot     =: (7{boxc) rep (e.&(6{boxc) *. shr"1@(e.&dash))
connect =: bot @ left @ cross @ right

tree    =: connect @ > @ (,.&.>/) @ ('0'&root ; ]) @ (tr@>@ar)


jtr  =: 5!:4
test=: 1 : '(jtr -: tree) <''x'''

+             test
+.            test
i.            test
0:            test
-             test

1 2 3&+       test
+&(i.10 2)    test
+&(i.11 1)    test
,&'abcd'      test
(<"0 i.7)&e.  test

+/ % #        test
+ %           test

+/            test
+./           test
+./ .*        test
+/ .*         test
+`*@.<        test
NB. +`-`*`:0  test

(+/`%`#)      test
(<"0 i.7)     test
''            test
(i.10 1)      test
(i.11 1)      test

(jtr -: tree)"0 ;:'ar lr boxed mt oarg shr shl mat' NB. ,' boxc dash'
(jtr -: tree)"0 ;:'extent limb1 limb pfx pad take rc'
(jtr -: tree)"0 ;:'kernt kernb kern gap graft'
(jtr -: tree)"0 ;:'lab label center root leaf'
(jtr -: tree)"0 ;:'trx trgl trgr trg trtil trnoun trcase tr'
(jtr -: tree)"0 ;:'rep right cross left bot connect tree'

a =. /
(jtr -: tree) <'a'
a =. 1 : 'x/\'
(jtr -: tree) <'a'
inv =. 1 : 'x^: _'
(jtr -: tree) <'inv'

c =. &
(jtr -: tree) <'c'
ip =. 2 : 'x @ (y"1 _)'
(jtr -: tree) <'ip'


NB. 5!:4, handling gerunds ----------------------------------------------

fx  =. 5!:0
jtr =. 5!:4
th  =. 3 : ('f=.y fx'; '{.@(]`<@.(1&<@#)) jtr<''f''') 

g =.[`((e.&' ' # i.@#)@])`]
f =. g}
h =. g`:6}
(jtr<'f') -: jtr <'h'

g =. */\.`(i.@#)`(+/~)
f =. i.^:g
h =. i.^:(g`:6)
(jtr<'f') -: jtr <'h'

g =. ((%&4@# , 4:) $ ]) ` %: ` $
f =. g@.*
h =. g`:6@*
(0 5}.jtr<'f') -: 0 4}.jtr<'h'


4!:55 ;:'a ar bot boxc boxed c center connect cross dash '
4!:55 ;:'extent f fx g gap ger graft h inv ip '
4!:55 ;:'jtr kern kernb kernt lab label leaf left limb limb1 '
4!:55 ;:'lr mat mt oarg pad pfx rc rep right root '
4!:55 ;:'shl shr take test th tr trcase tree trg trgl trgr '
4!:55 ;:'trnoun trtil trx '


