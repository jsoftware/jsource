NB. f. ------------------------------------------------------------------

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

each =: &.>
f    =: 'each' f.
g    =: &.>
(ar<'f') -: ar<'g'
(|.&.>y) -: |. 'each' f. y=: ;:'Cogito, ergo sum.'

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


NB. f. and $: -----------------------------------------------------------

lr   =: 1 : '5!:5 <''x'''
sgn  =: *
dec  =: <:
inc  =: >:
fact =: 1:`(* $:@<:)@.*
fact1=: 1:`(* $:@dec)@.sgn

'1:`(* $:@<:)@.*' -: fact      f. lr
'1:`(* $:@<:)@.*' -: fact1     f. lr
'>:@(3 : ''1:`(* $:@<:)@.* y'' :(4 : ''x 1:`(* $:@<:)@.* y''))' -: inc@fact  f. lr
'3 : ''1:`(* $:@<:)@.* y'' :(4 : ''x 1:`(* $:@<:)@.* y'')&.<:'  -: fact&.dec f. lr

s    =: $:
cap  =: [:

monad=: 3 : '$: y'
dyad =: 4 : 'x $: y'
ambi =: (3 : '$: y') : (4 : 'x $: y')

eq=: 2 : 0
 f=: x f.
 g=: y f.
 assert. (5!:1 <'f') -: 5!:1 <'g'
 1
)

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
*&.s           eq (* &.monad      )
*&:s           eq (* &:monad      )

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

C=: 0:`0:`1:`($:&<: + ($: <:)) @. ([: #. <:,0<[)
f=: (C +:)"0 f.
4!:55 ;:'C'
(f -: (!+:)) y=: 10 ?@$ 6


4!:55 ;:'a ambi ar b C cap dec dyad each f fact fact1 eq g inc lr '
4!:55 ;:'mat monad plus s sgn slash t v y'


