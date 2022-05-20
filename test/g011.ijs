prolog './g011.ijs'
NB. <.y -----------------------------------------------------------------

ir=: 3!:1

p -: <.p=:?3 4$100
4 =  3!:0 <.p

p =: o.?3 4$100
q =: <.p
q    -: <.q
($q) -: $p
(p>:q)*.q>:p-1

p =: 1e20*o.?3 4$100
q =: <.p
q    -: <.q
($q) -: $p
(p>:q)*.q>:p-1

_ __ -: <. _ __
(ir -: ir@:<.) _ __ _.

0 < <.   2 ^ 31 63
0 > <. - 2 ^ 31 63

3 : 0^:IF64 ''  NB. Large ints
fl =. 3 : '<. y'"0
cl =. 3 : '>. y'"0   NB. singletons
cl0 =. 3 : '>.!.0 y'"0
NB. i. 3 because 2000000000000000.4 turns to 2..5 on input, then rounds up on non-AVX
NB. 0.2 for same reason
assert. 2e13 2e14 2e15 = <. 2e13 2e14 2e15 +/ 1e_1 * i: 3
assert. 2e13 2e14 2e15 = >. 2e13 2e14 2e15 +/ 1e_1 * i: 3
assert. _2e13 _2e14 _2e15 = <. _2e13 _2e14 _2e15 +/ 1e_1 * i: 3
assert. _2e13 _2e14 _2e15 = >. _2e13 _2e14 _2e15 +/ 1e_1 * i: 3
assert. 2e13 2e14 2e15 = <. 2e13 2e14 2e15 +/ 0.2 + 1e_2 * i: 3
assert. (2e13 2e14 2e15) = >. 2e13 2e14 2e15 +/ 0.2 + 1e_2 * i: 3  NB. Not >:, because for big int <. y can be > y, etc 
assert. (>: 2e13 2e14 2e15) = (>.!.0) 2e13 2e14 2e15 +/ 0.2 + 1e_2 * i: 3
assert. _2e13 _2e14 _2e15 = <. _2e13 _2e14 _2e15 +/ 0.2 + 1e_2 * i: 3
assert. (_2e13 _2e14 _2e15) = >. _2e13 _2e14 _2e15 +/ 0.2 + 1e_2 * i: 3
assert. 2e13 2e14 2e15 = fl 2e13 2e14 2e15 +/ 1e_1 * i: 3
assert. 2e13 2e14 2e15 = cl 2e13 2e14 2e15 +/ 1e_1 * i: 3
assert. _2e13 _2e14 _2e15 = fl _2e13 _2e14 _2e15 +/ 1e_1 * i: 3
assert. _2e13 _2e14 _2e15 = cl _2e13 _2e14 _2e15 +/ 1e_1 * i: 3
assert. 2e13 2e14 2e15 = fl 2e13 2e14 2e15 +/ 0.2 + 1e_2 * i: 3
assert. (2e13 2e14 2e15) = cl 2e13 2e14 2e15 +/ 0.2 + 1e_2 * i: 3  NB. Not >:, because for big int <. y can be > y, etc 
assert. (>: 2e13 2e14 2e15) = cl0 2e13 2e14 2e15 +/ 0.2 + 1e_2 * i: 3
assert. _2e13 _2e14 _2e15 = cl _2e13 _2e14 _2e15 +/ 0.2 + 1e_2 * i: 3
assert. (_2e13 _2e14 _2e15) = cl _2e13 _2e14 _2e15 +/ 0.2 + 1e_2 * i: 3
1
)

NB. tolerance cases, singleton & otherwise

f =: 3 : 0
ints =. i: 5
fracs =. (, -) (1 % 16b40000000) % (20+i.6) (33 b.) 1
assert. ints (<.@:(+/) -: ([ - ([ > +/) ))"0/ fracs
assert. ints (<.@:(+/) -: ([ - ([ > +/) )) fracs
assert. 0 0 -: <. 1 % _ __
1
)
f ''
f =: 3 : 0"0
ints =. i: 5
fracs =. (, -) (1 % 16b40000000) % (20+i.6) (33 b.) 1
assert. ints (<.!.y@:(+/) -: ([ - ([ >!.y +/) ))"0/ fracs
assert. ints (<.!.y@:(+/) -: ([ - ([ >!.y +/) )) fracs
assert. 0 0 -: <.!.y 1 % _ __
1
)

f 0 , 2^_35 _44 _48
NB. Now >.
f =: 3 : 0
ints =. i: 5
fracs =. (, -) (1 % 16b40000000) % (20+i.6) (33 b.) 1
assert. ints (>.@:(+/) -: ([ + ([ < +/) ))"0/ fracs
assert. ints (>.@:(+/) -: ([ + ([ < +/) )) fracs
assert. 0 0 -: >. 1 % _ __
1
)
f ''
f =: 3 : 0"0
ints =. i: 5
fracs =. (, -) (1 % 16b40000000) % (20+i.6) (33 b.) 1
assert. ints (>.!.y@:(+/) -: ([ + ([ <!.y +/) ))"0/ fracs
assert. ints (>.!.y@:(+/) -: ([ + ([ <!.y +/) )) fracs
assert. 0 0 -: >.!.y 1 % _ __
1
)
f 0 , 2^_35 _44 _48


NB. <.z complex floor ---------------------------------------------------

zfl    =: <.!.0@+.
inc    =: (1&<:@(+/) * 1 0&=@(>:!.0/)) @ (+. - zfl)
zfloor =: zfl j./@:+ inc

a =: 3+0.1*i.10
b =: 5+0.1*i.10

(<. -: zfloor"0) j./  a ,:  b
(<. -: zfloor"0) j./  a ,: -b
(<. -: zfloor"0) j./(-a),:  b
(<. -: zfloor"0) j./(-a),: -b

y =: |: x j./ |. x=:0.1*i.10
a =:  </~  i.10
b =: (</|.)i.10
p =: (a>b)+0j1*a*.b
p -: <.y
(<. -: zfloor"0) y


NB. x<.y ----------------------------------------------------------------

3  -: 3 <. 5
_5 -: _3 <. _5
((<3 4)$&>  i.2 3) -: (   i.2 3) <.   i.2 3 3 4
((<3 4)$&>o.i.2 3) -: ( o.i.2 3) <. o.i.2 3 3 4
(-  i.2 3 3 4)     -: (-  i.2 3) <.-  i.2 3 3 4
(-o.i.2 3 3 4)     -: (-o.i.2 3) <.-o.i.2 3 3 4

(3 <. i.10)   -: (0{3 5j7) <. i.10
((i.10) <. 3) -: (i.10) <. 0{3 5j6

NB. symbol
(s:@<"0 'abaABA') -: (s:@<"0 'cbaCBA') <. s:@<"0 'abcABC'

'domain error' -: <. etx 'abc'  
'domain error' -: <. etx <'abc'
'domain error' -: <. etx u:'abc'  
'domain error' -: <. etx <u:'abc'
'domain error' -: <. etx 10&u:'abc'  
'domain error' -: <. etx <10&u:'abc'
'domain error' -: <. etx s:@<"0 'abc'  
'domain error' -: <. etx s:@<"0&.> <'abc'
'domain error' -: <. etx <"0@s: <'abc'

'domain error' -: 'abc' <.  etx 3 4 5 
'domain error' -: 'abc' <.~ etx 3 4 5
'domain error' -: (u:'abc') <.  etx 3 4 5 
'domain error' -: (u:'abc') <.~ etx 3 4 5
'domain error' -: (10&u:'abc') <.  etx 3 4 5 
'domain error' -: (10&u:'abc') <.~ etx 3 4 5
'domain error' -: (s:@<"0 'abc') <.  etx 3 4 5 
'domain error' -: (s:@<"0 'abc') <.~ etx 3 4 5
'domain error' -: 3j4   <.  etx 3 4 5
'domain error' -: 3j4   <.~ etx 3 4 5 
'domain error' -: (<34) <.  etx 3 4 5 
'domain error' -: (<34) <.~ etx 3 4 5 

'length error' -: 3 4     <. etx 5 6 7   
'length error' -: 3 4     <.~etx 5 6 7    
'length error' -: (i.3 4) <. etx i.5 4 
'length error' -: (i.3 4) <.~etx i.5 4 

4!:55 ;:'a b f inc ir p q x y zfl zfloor '



epilog''

