prolog './g631.ijs'
NB. u&.v ----------------------------------------------------------------

12 _12 _12 12 = 3 3 _3 _3 +&.^. 4 _4 4 _4
7 _1 1 _7     = 3 3 _3 _3 *&.^  4 _4 4 _4

1 -: +&.^./''
0 -: *&.^ /''

totient =. * -.@%@~.&.q:
t1      =. 1: #. 1: = (+.i.)
(t1 -: totient)"0 x=:>:?2 10$1000


NB. f&.> ----------------------------------------------------------------

(100$<1) -: #&.>?100$1000
(,&.>6 1 4 4) = $&.>;:'Cogito, ergo sum.'
(,&.>6 1 4 4) = $&.>(u:&.>) ;:'Cogito, ergo sum.'
(,&.>6 1 4 4) = $&.>(10&u:&.>) ;:'Cogito, ergo sum.'
(,&.>6 1 4 4) = $&.>s:@<"0 &.> ;:'Cogito, ergo sum.'

(<"0 (>x)+ >y) -: x  +&.>y [ x=:   ?1000     [ y=:   ?1000
(<"0 (>x)>.>y) -: x >.&.>y [ x=:   ?1000     [ y=:<"0?1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?1000     [ y=:   ?1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?1000     [ y=:<"0?1000

(<"0 (>x)+ >y) -: x  +&.>y [ x=:   ?1000     [ y=:   ?100$1000
(<"0 (>x)>.>y) -: x >.&.>y [ x=:   ?1000     [ y=:<"0?100$1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?1000     [ y=:   ?100$1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?1000     [ y=:<"0?100$1000
                                                         
(<"0 (>x)+ >y) -: x  +&.>y [ x=:   ?100$1000 [ y=:   ?1000
(<"0 (>x)>.>y) -: x >.&.>y [ x=:   ?100$1000 [ y=:<"0?1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?100$1000 [ y=:   ?1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?100$1000 [ y=:<"0?1000

(<"0 (>x)+ >y) -: x  +&.>y [ x=:   ?100$1000 [ y=:   ?100$1000
(<"0 (>x)>.>y) -: x >.&.>y [ x=:   ?100$1000 [ y=:<"0?100$1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?100$1000 [ y=:   ?100$1000
(<"0 (>x)* >y) -: x * &.>y [ x=:<"0?100$1000 [ y=:<"0?100$1000

'length error' -: (i.12)  +&.> etx i.3 4
'length error' -: (i.3 4) +&.> etx i.4 3

NB. f&> ----------------------------------------------------------------

NB. unaligned memory access
(,:'bbbbb') -: (,3) }.&> ,<'aa bbbbb'
11111 22222 33333 555555 -: ". 36 37 38 31 }.&> 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 11111'; 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 22222'; 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 33333'; 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 555555'

NB. structural under
(|."1 |. i. 3 3) -: |.&., i. 3 3
(|."1 |. >: i. 3 3) -: |.&., >: i. 3 3
(3 3 $ }. i. 9) -: }.&., i. 3 3
(3 3 $ }. >: i. 9) -: }.&., >: i. 3 3
(3 3 4 ($,) 2 5 $ }. i. 36) -: 2 5&$@}.&., i. 3 3 4
(3 3 4 ($,) 2 5 $ }. >: i. 36) -: 2 5&$@}.&., >: i. 3 3 4

a =: i. 3 4
((>: b { a) b} a) -: >:&.(b&{) a [ b =: 1
((>: b { a) b} a) -: >:&.(b&{) a [ b =: 1 2
((>: b { a) b} a) -: >:&.(b&{) a [ b =: <1
((>: b { a) b} a) -: >:&.(b&{) a [ b =: <2 0 ,: 1 3
((>: b { a) b} a) -: >:&.(b&{) a [ b =: <1;2 0
a =: i. 3 1e6
3000 > 7!:2 'a =: >:&.(b&{) a' [ b =: 1
3000 > 7!:2 'a =: >:&.(b&{) a' [ b =: 1 2
1 = (0.9 1.1 * 2e6 * SZI) I. 7!:2 'a =: >:&.(b&{) a' [ b =: 2 1
1 = (0.9 1.1 * 4e6 * SZI) I. 7!:2 'a =: >:&.(b&{) a' [ b =: 2 1 0
3000 > 7!:2 'a =: >:&.(b&{) a' [ b =: <2 0 ,: 1 3

f=: ((; (% +/!.0&.:*:)&>/@(% ::(*@[)"0 L:0) >./@]) |)"1
g=: 9&o.&.(0&({ ))@(*@{. * f@:+)"1
(9&o.&.(0&({ ))@(*@{. * f@:+)"1&.((< a: ; _2 _1)&{) i. 10 3)  -: g&.((< a: ; _2 _1)&{) i. 10 3 

(4 5$9.5) -: (+/%#)&., i. 4 5   NB. u does not support inplacing




NB. semiduals x u&.(a:`v) y  and  x u&.(v`:a:) y -----------------------------------------------------------

'valence error' -: +&.(a:`^.) etx i. 5
'valence error' -: +:`*: "1&.(a:`>) etx i. 5
'valence error' -: +&.(^.`a:) etx i. 5
'domain error' -: ex '+&.(^.`^.)'
'domain error' -: ex '+&.(a:`a:)'
'domain error' -: ex '+&.(^.`a:`a:)'
'domain error' -: ex '+&.(^.`1) etx'
'domain error' -: ex '+&.(1`^.) etx'
'domain error' -: ex '+&.(,. ^.`a:)'
(^ 1 + ^. y) -: 1 +&.(a:`^.) y =: i. 5
(^ 1 + ^. x) -: (x =: i. 5) +&.(^.`a:) 1
(^ x + ^. y) -: (x =: 1 2) +&.(a:`^.) y =: i. 2 5
(x <@(+"1 >)"1 0 y) -: (x =: 1 2) +"1&.(a:`>) y =: <"2 i. 5 2
(x <@(+"1 2 >)"1 0 y) -: (x =: 1 2) +"1 2&.(a:`>) y =: <"2 i. 2 5
(x <@(+"1 2 >)"1 0 y) -: (x =: 1 2) +"1 2&.(a:`>) y =: (<"2 i. 2 5) ,. (<"2 i. 2 7)

0 0 0 -: +&.(a:`^.) b. 0
0 1 0 -: p.&.(a:`^.) b. 0
0 0 1 -: +"1&.(^.`a:) b. 0
_ _ _ -: +&.:(a:`^.) b. 0



NB. Verify that named adverbs are stacked if the value is nameless
o =: >
a =: &.o
pe =: >: a
(<5) -: pe <4
o =: &
'domain error' -: pe etx <4
'>:&.o' -: 5!:5 <'pe'
e =: &.>
(<5) -: (e =: >:)e <4  NB. If e were stacked by reference, this would fail
e =: 1 : 'u&.>'
'domain error' -: ". etx '(e =: >:)e <4'  NB. Locative produces reference
exx_z_ =: &.>
'domain error' -: ". etx '(exx_z_ =: >:)exx_z_ <4'  NB. Locative produces reference
o =: >
3 : 0 ''
try.
a =: &.o
(a =: >:)a etx <4  NB. fails when a changes part of speech
0
catch.
1
end.
)
a =: &.o
(<<5) -: (>: a =: &.>)a 4
a =: &.o
5 -: (>: a =: &.])a 4   NB. OK to redefine a nameful entity as nameless; the new definition is used both places

NB. Pristinity and inplacing in u&.>

NB. Verify pristine results produced where appropriate
NB. Tools for 13!:_4:
isprist =: ([: * 16b1000000 (17 b.) 1&{)
ispristorunbox =: ([: * 16b1000000 (17 b.) 1&{) +. 32 ~: [: (17 b. -) 3&{
isvirt =: [: * 16b20000 (17 b.) 1&{
isro =: [: * 16b1 (17 b.) 1&{
isip =: 0 > 4&{
countis =: 4&{
(isro,-.@isprist) 13!:_4 i. 20  NB. Short vec is readonly
(-.@isprist) 13!:_4 < i. 20  NB. not prist when boxed
(-.@isro,-.@isprist) 13!:_4 i. 400  NB. Short vec is readonly
(isprist) 13!:_4 < i. 400  NB. prist when boxed
(isprist) a =: 13!:_4 < i. 400
((0>countis),isprist) a   NB. name can be pristine even if not inplaceable
(isprist) a =: 13!:_4 <"0  i. 5
isprist 13!:_4 ;: 'a b c'   NB. Verbs that produce boxes should produce pristine
-. isprist 13!:_4 (5 6) ; 3 4 5   NB. Verbs that produce boxes should produce pristine
isprist 13!:_4 ] (+: 5 6) ; (+: 3 4 5)
-. isprist 13!:_4 < 5
isprist 13!:_4 < +: 5
isprist 13!:_4 C. 2 3 4 5

NB. x is 5!:5 of left arg, y is shape of right arg, u is verb.
NB. n is (0 if verb produces normal block, 1 if virtual, 2 if self-virtual, 3 if virtual block but not of the input),
NB.      (1 if verb produces pristine/unboxed block on inplaceable input),
NB.      (1 if x/y remains pristine or becomes unboxed, 2 if y only, 3 if x only),
NB.      (1 if result pristine/unboxed on noninplaceable input)
NB. If y is negative a pristine scalar box is used
ckprist =: 2 : 0
boxr =. 0 [ boxs =. y
if. 8 = 3!:0 y do. iboxs =. y
elseif. y < 0 do. boxr =. _ [ iboxs =. i. 1000   NB. scalar box, not RO
else. iboxs =. i. boxs end.
'virt prist wprist rprist' =. 4 {. n
NB. virt: produces virtual result when applied inplace
NB. virtnip: produces virtual result when applied not-in-place
NB. virtprist: produces virtual result leaves pristine set in the arg it came from
'virt virtnip virtprist' =. virt { _3 ]\ 0 0 0  1 1 1  0 1 1  1 1 0
NB. Verify taking from a pristine loses its pristinity
a =: <"boxr memu iboxs  NB. Sets prist
1: u a
assert. ((virtprist +. wprist) = ispristorunbox) 13!:_4 a   NB. Making a virtual does not turn off pristinity in the backer
NB. Verify pristinity is passed to an only successor but not a shared successor
assert. ((rprist , virtnip) = ispristorunbox , isvirt) (0: 13!:_4@] u) <"boxr memu iboxs
assert. ((prist , virt) = ispristorunbox , isvirt) (u 13!:_4@[ 0:) <"boxr memu iboxs
1 return. y
:
NB. Verify taking from a pristine loses its pristinity
boxr =. 0 [ boxs =. y
if. y < 0 do. boxr =. _ [ boxs =. 1000 end.   NB. scalar box, not RO
'virt prist wprist rprist' =. 4 {. n
'virt virtnip virtprist' =. virt { _3 ]\ 0 0 0  1 1 1  0 1 1  1 1 0
'wpristx wpristy' =. wprist { _2 ]\ 0 0   1 1   0 1   1 0
xisprist =. isprist 13!:_4 ". x
yisprist =. isprist 13!:_4 <"boxr i. boxs
a =: <"boxr i. boxs  NB. Sets prist
1: (".x) u a
assert. ((yisprist *. virtprist +. wpristy) = isprist) 13!:_4 a [ 'right'
a =: ". x  NB. Sets prist
1: a u <"boxr i. boxs
assert. ((xisprist *. virtprist +. wpristx) = isprist) 13!:_4 a [ 'left'
NB. Verify pristinity is passed to an only successor but not a shared successor
assert. ((rprist , virtnip) = ispristorunbox , isvirt) (".x) (0: 13!:_4@] u) <"boxr i. boxs
assert. ((prist , virt) = ispristorunbox , isvirt) (".x) (u 13!:_4@[ 0:) <"boxr i. boxs
1 return. y
)
{. ckprist 0 1 ] 5
{. ckprist 1 1 ] 4 5
'2' {. ckprist 1 1 ] 5  NB. virtual, and pristine if inplaceable
'2' {. ckprist 1 1 ] 2 5
'2 2' {. ckprist 0 0 ] 2 5  NB. complex take does not check pristinity
}. ckprist 1 1 ] 5
}. ckprist 1 1 ] 4 5
'2' }. ckprist 1 1 ] 5
'2' }. ckprist 1 1 ] 2 5
'2 2' }. ckprist 0 1 ] 2 5
}: ckprist 1 1 ] 5
}: ckprist 1 1 ] 4 5
{: ckprist 0 1 ] 5  NB. not virtual for atom
{: ckprist 1 1 ] 4 5   NB. virtual for list
= ckprist 0 1 1 1 ] 5
> ckprist 0 1 0 1 ] _1  NB. result exposes a
> ckprist 0 1 1 1 ] 5  NB. result is unboxed, but doesn't disturb a
+:&.> ckprist 0 1 1 1 ] 5   NB. result is itself pristine
+&.> ckprist 0 1 0 0 ] 5  NB. Result is a passthrough, not pristine unless inplace
NB. dyad doesn't support prist yet '2' +&.> ckprist 0 1 1 ] 5  NB. scaf
< ckprist 0 0 0 0 ] 5
<"0 ckprist 0 0 0 0 ] 5
'32' $ ckprist 1 0 0 0 ] 36   NB. produces virtual not pristine len >= MINVIRTSIZE
'36' $ ckprist 0 0 0 0 ] 35
'32' $"1 ckprist 0 0 0 0 ] 4 36
'6' $"1 ckprist 0 0 0 0 ] 4 5
'32' ($,) ckprist 1 0 0 0 ] 36   NB. produces virtual not pristine
'36' ($,) ckprist 0 0 0 0 ] 35
~. ckprist 0 1 0 0 ] 5
~. ckprist 0 1 0 0 ] 4 5
|. ckprist 0 1 0 0 ] 5
|.!.a: ckprist 0 0 0 0 ] 5
'3' |. ckprist 0 1 0 0 ] 5
'3' |.!.a: ckprist 0 0 0 0 ] 5
'3' |."0 ckprist 0 1 0 0 ] 5
|: ckprist 0 1 1 1 ] 5  NB. On list, this looks like ]
|: ckprist 0 1 0 0 ] 4 5
|:"1 ckprist 0 1 1 1 ] 4 5  NB. On list, this looks like ]
|:"2 ckprist 0 1 0 0 ] 4 5 6
'(0 ,: 0)' |: ckprist 0 0 0 0 ] 5
, ckprist 0 1 1 1 ] 5
, ckprist 2 1 0 0 ] 4 5
,"1 ckprist 0 1 1 1 ] 4 5
,"0 ckprist 1 1 0 0 ] 4 5
'(<"0 i. 5)' , ckprist 0 1 0 ] 5
'(<i. 1000)' , ckprist 0 1 0 ] _1
,~ ckprist 0 0 0 ] 5  NB. not pristine when duplicating self
'(<"0 i. 2 5)' , ckprist 0 1 0 ] 5  NB. append in place
'(<"0 i. 2 6)' , ckprist 0 1 0 ] 5
'(<"0 i. 2 3)' , ckprist 0 0 0 ] 5  NB. append not in place
,. ckprist 0 1 1 1 ] 4 5
,. ckprist 1 1 0 0 ] 5
,."2 ckprist 0 1 1 1 ] 3 4 5
,."1 ckprist 1 1 0 0 ] 4 5
'<"0 i. 5' ,. ckprist 0 0 0 0 ] 5  NB. with frame we don't track pristinity - we could
'<{. i. 1' ,. ckprist 0 1 0 0 ] _1  NB. no frame
'<"0 i. 5' ,: ckprist 0 0 0 0 ] 5
'<i. 1000' ,: ckprist 0 0 0 0 ] _1  NB. not virtual - too short
,: ckprist 0 0 0 0 ] _1
,: ckprist 0 0 0 0 ] 4 5
,: ckprist 1 0 0 0 ] 3 4 5  NB. this is long enough
,: ckprist 0 0 0 0 ] 5
'<"0 i. 5' ,: ckprist 0 0 0 0 ] 4 5
'<"0 i. 4 5' ,: ckprist 0 0 0 0 ] 4 5
]@]"1 ckprist 0 0 ] 4 5   NB. Not prist because virtual boxed doesn't inplace
]&.>"1 ckprist 0 1 ] 4 5 NB. exception made for &.>
]@]"0 ckprist 0 0 ] 4 5 NB. similar, for rankex0
]&.>"0 ckprist 0 1 ] 4 5
{.@]"1 ckprist 0 0 0 0 ] 4 5   NB. Goes through from, but block is not marked inplaceable in " because not in TYPEVIPOK
{.@]"2 ckprist 0 0 0 0 ] 3 4 5  NB. {. produces virtual result which loses pristinity
'(<"0 i. 4 5)' ,&]"1 ckprist 0 0 0 ] 4 5
,&]"1~ ckprist 0 0 0 ] 5  NB. not pristine when duplicating self
'(<"0 i. 2 5)' ,&]"1 ckprist 0 0 0 ] 5
'(<"0 i. 2 6)' ,&]"1 ckprist 0 0 0 ] 5
; ckprist 0 1 1 1 ] 4 5  NB. open result
; ckprist 0 1 0 1 ] _1  NB. one box, its contents escape
'i. 1000' ; ckprist 0 1  ] _1
'i. 1000' ; ckprist 0 1  ] 5
'0' # ckprist 0 0 1 ] 5
'1' # ckprist 0 1 1 1 ] 5
'1 0 1 0 1' # ckprist 0 1 ] 5  NB. No repetitions with boolean
'1 0 2 0 1' # ckprist 0 0 ] 5
'1 0 2j1 0 1' # ckprist 0 0 ] 5
];.1 ckprist 0 0 ] 5  NB. Cannot inplace the partition because it's not DIRECT
]@];.1 ckprist 0 0 ] 5
]&.>;.1 ckprist 0 0 ] 5  NB. doesn't inplace because w is used internally
>;.1 ckprist 0 1 1 1 ] 5
1 0 1 0 1&#;.1 ckprist 0 0 ] 5  NB. Cannot inplace the partition because it's not DIRECT
1 0 2 0 1&#;.1 ckprist 0 0 ] 5
'1 0 0 1 0' ]&.>;.1 ckprist 0 1 ] 5  NB. inplaces when &.>
0:/ ckprist 0 1 1 1 ] 5   NB. result is open; operation never exposes a cell of y
]/ ckprist 0 0 0 0 ] 5   NB. last cell escapes
[/ ckprist 0 0 0 0 ] 5   NB. first cell escapes
(>@]^:((<4)-:]))/ ckprist 0 1 0 1 ] 5  NB. result is open, but a cell of y escapes
0:/. ckprist 0 1 0 1 ] 5   NB. result is open; operation never exposes a cell of y, but we still have to remove pristinity of noninplaceable block 
]/. ckprist 0 0 ] 5     NB. Cannot inplace the partition because it's not DIRECT
[/. ckprist 0 0 ] 5   NB. Cannot inplace the partition because it's not DIRECT
[&.>/. ckprist 0 1 ] 5   NB. Inplaces with &.>
(>@]^:((<4)-:]))/. ckprist 0 0  ] 5  NB. partition cannot be inplaced and sometimes returns
<@0:/\ ckprist 0 0 0 0 ] 5   NB. result is open; operation never exposes a cell of y, but we still have to remove pristinity of noninplaceable block.  f/\ does not inplace anything
]/\ ckprist 0 0 0 0 ] 5  NB. /\. does not inplace virtw, so the result is not pristine   
[/\ ckprist 0 0 0 0 ] 5 
(<@>@]^:((<4)-:]))/\ ckprist 0 0  ] 5  NB. not inplace virtw
<@0:/\. ckprist 0 0 1 0 ] 5   NB. result is open; operation never exposes a cell of y, but we still have to remove pristinity of noninplaceable block 
]/\. ckprist 0 0 0 0 ] 5  NB. /\. does not inplace virtw, so the result is not pristine   
[/\. ckprist 0 0 0 0 ] 5 
'2' ]\ ckprist 0 0 0 0 ] 5  NB. Inputs escape, not pristine
'2' [\ ckprist 0 0 0 0 ] 5
'2' ,\ ckprist 0 0 0 0 ] 5
(<@>@]^:((<4)-:]))/\. ckprist 0 0  ] 5  NB. not inplace virtw
/:~ ckprist 0 1 ] 5  NB. Passes pristinity through 
/:~ ckprist 0 1 ] 4 1  NB. Creates nonvirtual block
/:~ ckprist 0 1 ] 0.5 +|. i. 4 10  NB. creates nonvirtual block
/:~ ckprist 0 1 ] 4 10  NB. return original y arg
\:~ ckprist  0 1 ] 5  NB. This could pass pristinity through 
\:~ ckprist  0 1 ] 4 5  NB. This could pass pristinity through 
'<"0 i. 4 3 5' \:"1 ckprist  0 0 2 ] 4 5  NB. Repeated cells - not pristine 
'<"0 i. 4 5' \:"1 ckprist  0 1 2 ] 4 5  NB. no repeated cells
'<"0 i. 5 2' ]"0 ckprist 0 0 3 ] 5  NB. x arg ignored - y is marked as repeated
'<"0 i. 5' ]"0 ckprist 0 1 1 1 ] 5  NB. passes y through unmodified
'<"0 i. 5' ["0 ckprist 0 1 1 1 ] 5  NB. passes x through unmodified
'<"0 i. 5' ["0 ckprist 0 0 2 ] 5 2  NB. y arg ignored - x is marked as repeated
'2' { ckprist 0 1  ] 4 5   NB. pristine because too short for virtual
'2' { ckprist 1 1  ] 4 64   NB. virtual+pristine because this goes through virtualip
'1' { ckprist 1 1  ] 4 64
'1 3' { ckprist 0 0  ] 4 5   NB. not pristine, because indexes could be repeated
'2 3' { ckprist 0 0  ] 4 5   NB. pristine because too short for virtual
'2 3' { ckprist 1 1  ] 4 50   NB. virtual+pristine because this goes through virtualip
'<1' { ckprist 1 1  ] 4 50   NB. virtual block, does not clear w prist
'<1' { ckprist 0 0  ] 4 1   NB. nonvirtual block, clears w prist
'2' A. ckprist 0 0  ] 4 5   NB. result is permutation, which is not virtual
'1' A. ckprist 0 0  ] 4 5
'1 3' A. ckprist 0 0  ] 4 5   NB. not pristine, because indexes could be repeated
'2' C. ckprist 0 0  ] 4 5   NB. result is permutation, which is not virtual
'1' C. ckprist 0 0  ] 4 5
'1 3' C. ckprist 0 0  ] 4 5   NB. not pristine, because indexes could be repeated
isprist 13!:_4 C. 0 2 1 4 3  NB. cyclic perm is always created pristine
'2' {:: ckprist 0 1 0 1 ] 5  NB. fetch 
-. isprist 13!:_4 (2) {:: <"0 i. 5  NB. Result is not boxed
-. isprist 13!:_4 (1;0) {:: (a:;((<<5),<3);3)  NB. Result is not pristine
] L: 0 ckprist 0 0 ] 5  NB. fauxvirtual block is copied & becomes non-pristine
] L: 0 ckprist 0 0 ] 4 5
'<"0 i. 5' ] L: 0 ckprist 0 0 ] 5
'<"0 i. 4 5' ] L: 0 ckprist 0 0 ] 4 5
+: L: 0 ckprist 0 1 0 1 ] 5
+: L: 0 ckprist 0 1 0 1 ] 4 5
'<"0 i. 5' + L: 0 ckprist 0 1 0 1 ] 5
'<"0 i. 4 5' + L: 0 ckprist 0 1 0 1 ] 4 5
] S: 0 ckprist 0 1 0 1 ] 5  NB. result is open
] S: 0 ckprist 0 1 0 1 ] 4 5
'<"0 i. 5' ] S: 0 ckprist 0 1 0 1 ] 5
'<"0 i. 4 5' ] S: 0 ckprist 0 1 0 1 ] 4 5
+: S: 0 ckprist  0 1 0 1 ] 5
+: S: 0 ckprist 0 1 0 1 ] 4 5
<@:+: S: 0 ckprist  0 0 ] 5  NB. S: does not check for pristine result
'<"0 i. 5' + S: 0 ckprist 0 1 0 1 ] 5
'<"0 i. 4 5' + S: 0 ckprist 0 1 0 1 ] 4 5
] S: 0 ckprist 0 1 0 1 ] 5  NB. result is open
] S: 0 ckprist 0 1 0 1 ] 4 5
'<"0 i. 5' ] S: 0 ckprist 0 1 0 1 ] 5
'<"0 i. 4 5' ] S: 0 ckprist 0 1 0 1 ] 4 5
-. isprist +: S: 0 <"0 i. 5
-. isprist +: S: 0 <"0 i. 4 5
-. isprist (<"0 i. 5) + S: 0 <"0 i. 5
-. isprist (<"0 i. 4 5) + S: 0 <"0 i. 4 5
-. isprist ] S: 0 <"0 i. 5  NB. result is open
-. isprist ] S: 0 <"0 i. 4 5
-. isprist (<"0 i. 5) ] S: 0 <"0 i. 5
-. isprist (<"0 i. 4 5) ] S: 0 <"0 i. 4 5

NB. n is (0 if verb produces normal block, 1 if virtual, 2 if self-virtual, 3 if virtual block but not of the input),
NB.      (1 if verb produces pristine/unboxed block on inplaceable input),
NB.      (1 if x/y remains pristine or becomes unboxed, 2 if y only, 3 if x only),
NB.      (1 if result pristine/unboxed on noninplaceable input)

NB. Verify pristinity of BOXATOP results
isprist 13!:_4 <@:+:"1 i. 4 5
isprist 13!:_4 <@:+:"1 i. 5
isprist 13!:_4 (i. 5 ) <@:+"1 i. 4 5
isprist 13!:_4 'abcda' </. i. 5   NB. w is permuted before use
IF64 = isprist 13!:_4 'abcda' <@]/. +: i. 5
isprist 13!:_4 'abcda' <@:+:/. i. 5
isprist 13!:_4 (1 0 0 1 0) <;.1 i. 5  NB. i. 5 is not inplaceable but special code does the copy
isprist 13!:_4 (1 0 0 1 0) <;.1 +: i. 5
-. isprist 13!:_4 (1 0 0 1 0) <@];._1 i. 5
IF64 = isprist 13!:_4 (1 0 0 1 0) <@];._1 +: i. 5
isprist 13!:_4 (1 0 0 1 0) <@:+:;._2 i. 5
isprist 13!:_4 <;.2 'abcdeabac'   NB. LIT is never inplaceable but special code does the copy
-. isprist 13!:_4  <@:];.1 'abcdeabac'
isprist 13!:_4  <@(2&#);.1 'abcdeabac'
isprist 13!:_4 <;.2 (3 1 4 1 5 9 2 5 3 1)
-. isprist 13!:_4  <@:];.1 (3 1 4 1 5 9 2 5 3 1)
isprist 13!:_4 <;.2 +: (3 1 4 1 5 9 2 5 3 1)
IF64 = isprist 13!:_4  <@:];.1 +: (3 1 4 1 5 9 2 5 3 1)
isprist 13!:_4  <@(2&#);.1 (3 1 4 1 5 9 2 5 3 1)


NB. Verify inplacing of u&.> on pristine
'>:&.>' (7!:2@, (< 1.1&*) 7!:2@]) '<"1 i. 100 1000'
NB. requires dyad '>.&.>/\.' (7!:2@, (< 1.1&*) 7!:2@]) '<"1 i. 100 1000'
'(<"_1 i. 100 1000) +&.> <"_1 i. 100 1000' (< 1.1&*)&(7!:2) '(<"_1 i. 100 1000) 0:@] <"_1 i. 100 1000'
'(<"_1 i. 100) +&.> <"_1 i. 100 1000' (< 1.1&*)&(7!:2) '(<"_1 i. 100) 0:@] <"_1 i. 100 1000'
'(<"_1 i. 100 1000) +&.> <"_1 i. 100' (< 1.1&*)&(7!:2) '(<"_1 i. 100 1000) 0:@] <"_1 i. 100'
'(10 {. 1) 0:@:>:&.>;.1' (7!:2@, (< 1.05&*) 7!:2@]) '<"1 i. 10 10000'
-. isprist 13!:_4 (<"0 i. 2) [&.> <"0 i. 2 5
isprist 13!:_4 (<"0 i. 2 5) [&.> <"0 i. 2  NB. arg escapes and is repeated
isprist 13!:_4 (<"0 i. 2) ]&.> <"0 i. 2 5
-. isprist 13!:_4 (<"0 i. 2 5) ]&.> <"0 i. 2
isprist 13!:_4 (<"0 i. 2 5) ]&.> <"0 i. 2 5
isprist 13!:_4 (<"0 i. 2 5) [&.> <"0 i. 2 5
a =: <"1 i. 100 1000
5000 > 7!:2 'a =: >:&.> a'

NB. x,y transfers ownership to result if both args are abandoned pristine (but not VIRTUAL).  But not in the first set below: now all exec results are made recursive
16b20 = 16b20 (17 b.) 1 { 13!:_4 (;:'a b c d e f g') , (4) { <"0 i. 6  NB. RHS is not recursible.  But apip keeps result recursible
16b20 = 16b20 (17 b.) 1 { 13!:_4 (;:'a b c d e f g h') , (4) { <"0 i. 6  NB. no apip; new block not recursible
16b1000020 = 16b1000020 (17 b.) 1 { 13!:_4 (;:'a b c d e f g h') , (;:'a b')  NB. both sides abandoned recursible pristine, transferred to result
16b1000020 = 16b1000020 (17 b.) 1 { 13!:_4 (;:'a b c d e f g h') , (}. ;:'a b')  NB. if a value is virtual, can't transfer ownership, because the virtual doesn't really own it
16b0000020 = 16b1000020 (17 b.) 1 { 13!:_4 ,~ (;:'a b c d e f g h')  NB. if sides equal, blocks are repeated, not pristine, and cannot take ownership because usecount repeated

16b20 = 16b20 (17 b.) 1 { (;:'a b c d e f g') 13!:_4@, (4) { <"0 i. 6  NB. RHS is not recursible.  But apip keeps result recursible
0 = 16b20 (17 b.) 1 {  (2 1 3 2 4 3 5 4&{ 13!:_4@, 2 1&{)   <"0 i. 6  NB. no apip; new block not recursible
16b1000020 = 16b1000020 (17 b.) 1 {  (;:'a b c d e f g h') 13!:_4@, (;:'a b')  NB. both sides abandoned recursible pristine, transferred to result
16b1000000 = 16b1000020 (17 b.) 1 {  (;:'a b c d e f g h') 13!:_4@, (}. ;:'a b')  NB. if a value is virtual, can't transfer ownership, because the virtual doesn't really own it
16b0000000 = 16b1000020 (17 b.) 1 { 13!:_4@,~ (;:'a b c d e f g h')  NB. if sides equal, blocks are repeated, not pristine, and cannot take ownership because usecount repeated



NB. not if args identical
NB. not if one arg is virtual

NB. Verify that result loops perform inplacing, including assignment inplacing

NB. Virtual blocks passed into explicit definition do not have to be realized
v1 =: 3 : 'v2 y'
v2 =: 3 : 'v3 y'
v3 =: 3 : 0
4!:55 <'gname'
+/ y
)
gname =: 100 100000 $ 4
50000 > 7!:2 'v1"1 gname'
v3 =: 3 : 0
gname =: 100 100000?@$ 8
+/ y
)
gname =: 100 100000 $ 4
(100 # 400000) -: v1"1 gname

NB. Check for memory leak of virtual assignment
a =: 7!:0 ''
a =: 7!:0 ''
1: 3 : 'y =. 6' 5 {. i. 20 20
a > _1000 + 7!:0 ''

(,<,<0) -: ((}: , 0: each@:{:) each) @: (00"_ each each) ,<,<,<97   NB. used to free block prematurely

NB. READONLY result of explicit defn is not inplaceable
(isro *. -.@isip) 13!:_4 {{ i. y }} 7
isip 13!:_4 {{ i. y }} 1e6
1: 00 * {{ i. y }} 7
0 1 2 3 4 5 6 7 8 9 -: i.10

4!:55 ;:'a b ckprist countis e exx_z_ f g gname isip isprist ispristorunbox isro isvirt o pe t1 totient v1 v2 v3 x y '

epilog''
