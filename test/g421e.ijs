NB. f//. y --------------------------------------------------------------

data=: 4 : 0
 select. x 
  case. 'b' do. y ?@$ 2 
  case. 'i' do. y ?@$ 1000
  case. 'I' do. y ?@$ 1e9
  case. 'd' do.     1e_4 * _5e6 +    y  ?@$ 1e7
  case. 'z' do. j./ 1e_4 * _5e6 + (2,y) ?@$ 1e7
  case. 'x' do. x:         _5e8 +    y  ?@$ 1e9
  case. 'q' do. x:  1e_4 * _5e8 +    y  ?@$ 1e9
 end.
)

test=: 1 : 0
 yy=: y data 17 19
 assert. (u/"_/. -: u//.)   yy
 assert. (u/"_/. -: u//.) |:yy
 1
)

test2=: 2 : 0
 xx=: y data 17
 yy=: y data 23
 assert. xx (u//.@(v/) -: u/"_/.@(v/"_)) yy
 assert. yy (u//.@(v/) -: u/"_/.@(v/"_)) xx
 1
)

+  test"0 'biIdxqz'
>. test"0 'biIdxq'
<. test"0 'biIdxq'

+. test 'b'
*. test 'b'
~: test 'b'
=  test 'b'
<  test 'b'
<: test 'b'
>  test 'b'
>: test 'b'

17 b. test"0 'iI'
22 b. test"0 'iI'
23 b. test"0 'iI'

+ test2 *"0 'biIdxqz'

~: test2 *. 'b'

22 b. test2 (17 b.)"0 'iI'


4!:55 ;:'data test test2 xx yy'


