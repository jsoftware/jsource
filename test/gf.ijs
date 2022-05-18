prolog './gf.ijs'
NB. f. ------------------------------------------------------------------

ws=: 9!:40 ''
9!:41 ]1  NB. retain comments and whitespace

ar =: 5!:1

mat =: ?3 4$1e5
mat -: 'mat' f.

slash =: /
plus  =: +
v     =: plus slash
f     =: 'v' f.
g     =: +/
(ar<'f') -: ar<'g'
(+/y)    -: 'v' f. y=: _1e7+?12$2e7

eachh=: &.>
f    =: 'eachh' f.
g    =: &.>
(ar<'f') -: ar<'g'
(|.&.>y) -: |. 'eachh' f. y=: ;:'Cogito, ergo sum.'

eachh=: + t. ''
f    =: 'eachh' f.
g    =: + t. ''
(ar<'f') -: ar<'g'

f=: undefined_no
'value error' -: ex 'f f.'

t=: ~
s=: 't' f.
(5!:1 <'s') -: 5!:1 <'t'

s=: `:6
t=: ((&.>) (`<)) s
f=: 't' f.
g=: ((&.>)(`<))(`:6)
(5!:1 <'f') -: 5!:1 <'g'

t =: 5
5 -: ".@'t' 0
5 -: ".@'t' f. 0
s =: ".@'t' f.
t =: 6
6 -: s 0
f =: s f.
t =: 7
7 -: f 0



NB. f. and $: -----------------------------------------------------------

lr   =: 1 : '5!:5 <''u'''
sgn  =: *
dec  =: <:
inc  =: >:
fact =: 1:`(* $:@<:)@.*"0
fact1=: 1:`(* $:@dec)@.sgn"0

NB. Verify that f. verbs are functionally equivalent, without checking reps
(fact i. 5) -: (fact f. i. 5)
(fact i. 5) -: (fact1 i. 5)
(fact1 i. 5) -: (fact1 f. i. 5)
(fact@>: i. 5) -: (fact@>: f. i. 5)
(fact@>: i. 5) -: (fact1@>: i. 5)
(fact1@>: i. 5) -: (fact1@>: f. i. 5)
(>:@fact i. 5) -: (>:@fact f. i. 5)
(>:@fact i. 5) -: (>:@fact1 i. 5)
(>:@fact1 i. 5) -: (>:@fact1 f. i. 5)
(fact@>: : (+ fact)  i. 5) -: (fact@>: : (+ fact) f. i. 5)
((>: i. 5) fact@>: : (+ fact)  i. 5) -: ((>: i. 5) fact@>: : (+ fact) f. i. 5)
((>: i. 5) fact@>: : (+ fact)  i. 5) -: ((>: i. 5) fact1@>: : (+ fact1) i. 5)
((>: i. 5) fact1@>: : (+ fact1)  i. 5) -: ((>: i. 5) fact1@>: : (+ fact1) f. i. 5)

'1:`(* $:@<:)@.*"0' -: fact      f. lr
'1:`(* $:@<:)@.*"0' -: fact1     f. lr

   s =: 4 : 0
0
0
)
i =: 0&$: : s
'domain error' -: ex 'i&> b. 1'  NB. used to hang

load 'foo.ijs' [(3!:6 'a001 =: ]',LF,'a001`]@.1: f.') 1!:2 <'foo.ijs'


1: 0 : 0
'>:@(3 : ''1:`(* $:@<:)@.* y'' :(4 : ''x 1:`(* $:@<:)@.* y''))' -: inc@fact  f. lr
'3 : ''1:`(* $:@<:)@.* y'' :(4 : ''x 1:`(* $:@<:)@.* y'')&.<:'  -: fact&.dec f. lr

s    =: $:
cap  =: [:

monad=: 3 : '$: y'
dyad =: 4 : 'x $: y'
ambi =: (3 : '$: y') : (4 : 'x $: y')

eq=: 2 : 0
 f=: u f.
 g=: v f.
 assert. (5!:1 <'f') -: 5!:1 <'g'
 1
))

>:@ s          eq (>:@ ambi       )
>:@:s          eq (>:@:ambi       )
>:@ s@ *:      eq (>:@ monad@ *:  )
>:@:s@:*:      eq (>:@:monad@:*:  )
s@ *:          eq (monad@ *:      )
s@:*:          eq (monad@:*:      )
s@ *:@ >:      eq (monad@ *:@ >:  )
s@:*:@:>:      eq (monad@:*:@:>:  )

s & *:         eq (ambi & *:      )
s &:*:         eq (ambi &:*:      )
s & *:@>:      eq (monad& *:@>:   )
*& s           eq (* & monad      )
NB. illegal *&.s           eq (* &.monad      )
*&:s           eq (* &:monad      )
)
'domain error' -: ex '*&.$:'
'domain error' -: ex '*&.i.'
'domain error' -: ex '*&.:$:'
'domain error' -: ex '*&.:i.'

1: 0 : 0
s : +          eq (monad : +      )
* : s          eq (*     : dyad   )

s~             eq (dyad  ~        )

s  ;.1         eq (  monad ;. 1   )
+&s;._3        eq (+&monad ;. _3  )

(+   s -     ) eq (+   dyad   -   )
([:  + s     ) eq ([:  +      ambi)
([:  s -     ) eq ([:  monad  -   )
(cap s -     ) eq (cap monad  -   )
(+   - s     ) eq (+   -      ambi)
(* : (+ - s) ) eq (* : (+ - dyad) )
((+ - s) : > ) eq ((+ - monad) : >)

(+ s         ) eq (+ monad        )
(s   >:      ) eq (dyad   >:      )
(s/          ) eq (dyad /         )
(s/.         ) eq (monad/.        )
(s\          ) eq (monad\         )
(s\.         ) eq (monad\.        )

(   s`!`+@.] ) eq (   ambi `!`+@.]       )
(   ^`!`+@.s ) eq (   ^    `!`+@.ambi    )
(%&(s`!`+@.])) eq (%&(monad`!`+@.]    )  )
(%&(^`!`+@.s)) eq (%&(^    `!`+@.monad)  )
(s`!`+@.]@*  ) eq (   monad`!`+@.]    @ *)
(^`!`+@.s@*  ) eq (   ^    `!`+@.monad@ *)

s"2@*:         eq (monad"2@*:   )
s"2@*:@>:      eq (monad"2@*:@>:)
s"2~           eq (dyad"2~      )
(s"2 >: )      eq (dyad"2 >:    )
)

C=: 0:`0:`1:`($:&<: + ($: <:)) @. ([: #. <:,0<[)
f=: (C +:)"0 f.
4!:55 ;:'C'
(f -: (!+:)) y=: 10 ?@$ 6


4!:55 ;:'a a001 ambi ar b C cap dec dyad eachh f fact fact1 eq g i inc lr '
4!:55 ;:'mat monad plus s sgn slash t v ws y'



epilog''

