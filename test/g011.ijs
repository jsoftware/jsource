NB. <.y -----------------------------------------------------------------

ir=: 3!:1

p -: <.p=.?3 4$100
4 =  3!:0 <.p

p =. o.?3 4$100
q =. <.p
q    -: <.q
($q) -: $p
(p>:q)*.q>:p-1

p =. 1e20*o.?3 4$100
q =. <.p
q    -: <.q
($q) -: $p
(p>:q)*.q>:p-1

_ __ -: <. _ __
(ir -: ir@:<.) _ __ _.

0 < <.   2 ^ 31 63
0 > <. - 2 ^ 31 63


NB. <.z complex floor ---------------------------------------------------

zfl    =. <.!.0@+.
inc    =. (1&<:@(+/) * 1 0&=@(>:!.0/)) @ (+. - zfl)
zfloor =. zfl j./@:+ inc

a =. 3+0.1*i.10
b =. 5+0.1*i.10

(<. -: zfloor"0) j./  a ,:  b
(<. -: zfloor"0) j./  a ,: -b
(<. -: zfloor"0) j./(-a),:  b
(<. -: zfloor"0) j./(-a),: -b

y =. |: x j./ |. x=.0.1*i.10
a =.  </~  i.10
b =. (</|.)i.10
p =. (a>b)+0j1*a*.b
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

'domain error' -: <. etx 'abc'  
'domain error' -: <. etx <'abc'

'domain error' -: 'abc' <.  etx 3 4 5 
'domain error' -: 'abc' <.~ etx 3 4 5
'domain error' -: 3j4   <.  etx 3 4 5
'domain error' -: 3j4   <.~ etx 3 4 5 
'domain error' -: (<34) <.  etx 3 4 5 
'domain error' -: (<34) <.~ etx 3 4 5 

'length error' -: 3 4     <. etx 5 6 7   
'length error' -: 3 4     <.~etx 5 6 7    
'length error' -: (i.3 4) <. etx i.5 4 
'length error' -: (i.3 4) <.~etx i.5 4 

4!:55 ;:'a b inc ir p q x y zfl zfloor '


