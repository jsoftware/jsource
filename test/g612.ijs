NB. x`:y ----------------------------------------------------------------

f =: - ` % ` ^ `: 0
y =: 2 3
x =: 5 7
(( -y),( %y),: ^y) -:   f y
((5-y),(5%y),:5^y) -: 5 f y
((x-y),(x%y),:x^y) -: x f y

(;/ -: ;`;`:3) x=: 10   ?@$ 1e6
(;/ -: ;`;`:3) x=: 10 3 ?@$ 1e6

'domain error' -: ex '-`a`:0 ' [ a=: 5!:1 <'plus' [ plus=: '+'
'domain error' -: ex '-`a/ '
'domain error' -: ex '-`a/.'
'domain error' -: ex '-`a\ '
'domain error' -: ex '-`a\.'
'domain error' -: ex '-`a;. 1'
'domain error' -: ex '-`a;._1'
'domain error' -: ex '-`a;. 2'
'domain error' -: ex '-`a;._2'


4!:55 ;:'a f x y'


