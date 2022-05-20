prolog './g421e.ijs'
NB. f//. y --------------------------------------------------------------

randuni''

NB. test with zero tolerance - no, can't, because +/ may reorder the inputs.  In fact, we have to INCREASE the tolerance
ct   =: 9!:18''
9!:19[1e_12

data=: 4 : 0
 select. x 
  case. 'b' do. y ?@$ 2 
  case. 'i' do. y ?@$ 1000
  case. 'I' do. y ?@$ 1e9
  case. 'd' do.     1e_4 * _5e6 +    y  ?@$ 1e7
  case. 'z' do. j./ 1e_4 * _5e6 + (2,y) ?@$ 1e7
  case. 'x' do. x:         _5e8 +    y  ?@$ 1e9
  case. 'q' do. x:  1e_4 * _5e8 +    y  ?@$ 1e9
  case. 's' do. sdot0{~ y ?@$ #sdot0
 end.
)

test=: 1 : 0
 yy=: y data 17 19
 assert. (u/"]/. -: u//.)   yy
 assert. (u/"]/. -: u//.) |:yy
 1
)

test2=: 2 : 0
 xx=: y data 17
 yy=: y data 23
 assert. xx (u//.@(v/) -: u/"]/.@(v/"])) yy
 assert. yy (u//.@(v/) -: u/"]/.@(v/"])) xx
 1
)

+  test"0 'biIdxqz'
>. test"0 'biIdxqs'
<. test"0 'biIdxqs'

+. test 'b'
*. test 'b'
~: test 'bs'
=  test 'bs'
<  test 'bs'
<: test 'bs'
>  test 'bs'
>: test 'bs'

17 b. test"0 'iI'
22 b. test"0 'iI'
23 b. test"0 'iI'

+ test2 *"0 'biIdxqz'

~: test2 *. 'bs'

22 b. test2 (17 b.)"0 'iI'

9!:19 ct

4!:55 ;:'adot1 adot2 sdot0 ct data test test2 xx yy'
randfini''


epilog''

