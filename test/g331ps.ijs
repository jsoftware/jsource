prolog './g331ps.ijs'
NB. special code for [: ; <@(f/\);.n and [: ; <@(f/\.);.n ---------------

test=: 1 : 0
 :
 f=: u
 neareq =.  [: *./ (1e_12 >. 1e_13 * >.&:|) > |@:-
 if. *./0=#;.1~x do.
  assert. (0#y)                    -: x ([: ; <@(u f./\ );. 1) etx y
  assert. (0#y)                    -: x ([: ; <@(u f./\.);. 1) etx y
 else.
  assert. (; x <@(u f./\ );. 1 etx y) neareq x ([: ; <@(u f./\ );. 1) etx y
  assert. (; x <@(u f./\.);. 1 etx y) neareq x ([: ; <@(u f./\.);. 1) etx y
 end.
 if. *./0=#;.2~x do.
  assert. (0#y)                    -: x ([: ; <@(u f./\ );. 2) etx y
  assert. (0#y)                    -: x ([: ; <@(u f./\.);. 2) etx y
 else.
  assert. (; x <@(u f./\ );. 2 etx y) neareq x ([: ; <@(u f./\ );. 2) etx y
  assert. (; x <@(u f./\.);. 2 etx y) neareq x ([: ; <@(u f./\.);. 2) etx y
 end.
 if. *./0=#;._1~x do.
  assert. (0#y)                    -: x ([: ; <@(u f./\ );._1) etx y
  assert. (0#y)                    -: x ([: ; <@(u f./\.);._1) etx y
 else.
  assert. (; x <@(u f./\ );._1 etx y) neareq x ([: ; <@(u f./\ );._1) etx y
  assert. (; x <@(u f./\.);._1 etx y) neareq x ([: ; <@(u f./\.);._1) etx y
 end.
 if. *./0=#;._2~x do.
  assert. (0#y)                    -: x ([: ; <@(u f./\ );._2) etx y
  assert. (0#y)                    -: x ([: ; <@(u f./\.);._2) etx y
 else.
  assert. (; x <@(u f./\ );._2 etx y) neareq x ([: ; <@(u f./\ );._2) etx y
  assert. (; x <@(u f./\.);._2 etx y) neareq x ([: ; <@(u f./\.);._2) etx y
 end.
 1
)

testb=: 4 : 0
 x <  test y
 x <. test y
 x <: test y
 x >  test y
 x >. test y
 x >: test y
 x =  test y
 x +  test y
 x -  test y
 x +. test y
 x +: test y
 x *  test y
 x *. test y
 x *: test y
 x ~: test y
)

n=: 547

x testb y [ x=:  n$0 [ y=: ? n   $2
x testb y [ x=:  n$0 [ y=: ?(n,7)$2
x testb y [ x=:  n$1 [ y=: ? n   $2
x testb y [ x=:  n$1 [ y=: ?(n,7)$2
x testb y [ x=: ?n$2 [ y=: ? n   $2
x testb y [ x=: ?n$2 [ y=: ?(n,7)$2

testn=: 4 : 0
if. 1 = 3!:0 y do.
 x =  test y
 x <  test y
 x +. test y
end.
 x <. test y
 x >. test y
 x +  test y
 x -  test y
 x *  test y
)

x testn y [ x=:  n$0 [ y=:     _5e3+? n   $1e4
x testn y [ x=:  n$0 [ y=: 0.1*_5e3+? n   $1e4
x testn y [ x=:  n$0 [ y=:     _5e3+?(n,3)$1e4
x testn y [ x=:  n$0 [ y=: 0.1*_5e3+?(n,3)$1e4
x testn y [ x=:  n$1 [ y=:     _5e3+? n   $1e4
x testn y [ x=:  n$1 [ y=: 0.1*_5e3+? n   $1e4
x testn y [ x=:  n$1 [ y=:     _5e3+?(n,3)$1e4
x testn y [ x=:  n$1 [ y=: 0.1*_5e3+?(n,3)$1e4
x testn y [ x=: ?n$2 [ y=:     _5e3+? n   $1e4
x testn y [ x=: ?n$2 [ y=: 0.1*_5e3+? n   $1e4
x testn y [ x=: ?n$2 [ y=:     _5e3+?(n,3)$1e4
x testn y [ x=: ?n$2 [ y=: 0.1*_5e3+?(n,3)$1e4

testbw=: 4 : 0
 for_i. 16+i.16 do.
  x i b. test y
 end.
)

x testbw y [ x=:  n$0 [ y=: _5e8+?n$1e9
x testbw y [ x=:  n$1 [ y=: _5e8+?n$1e9
x testbw y [ x=: ?n$2 [ y=: _5e8+?n$1e9

testov=: 4 : 0
 x +  test y
 x -  test y
 x >. test y
 x <. test y
)

x testov y [ x=:  n$0 [ y=: (?n$#y){y=: _1 1 , <.(_1+2^31),-2^31
x testov y [ x=:  n$1
x testov y [ x=: ?n$2


NB. special code for [: ; <@f;.n ----------------------------------------

(x ([: ; <@|.;.1) etx y) -: ; etx x <@|.;.1 etx y=: $. (#x)$100

P=: 1 : '[: ; <@:u;.1'
x=: 1 (0)}0=1019 ?@$ 4

(x  +/;.1 y) -: x  +/P y=: (#x) ?@$ 100
(x >./;.1 y) -: x >./P y
(x  +/;.1 y) -: x  +/P y=: (#x) ?@$ 0
(x >./;.1 y) -: x >./P y

(x  ([: ,  +/;.1) y) -: x  +/P y=: ((#x),3) ?@$ 100
(x  ([: , >./;.1) y) -: x >./P y
(x  ([: ,  +/;.1) y) -: x  +/P y=: ((#x),3) ?@$ 0
(x  ([: , >./;.1) y) -: x >./P y

2 3 2 2 -: ;@:(<@(,:"0);._1) 1 2 3 1 2 1 2
(,. 2 3 2 2) -: ;@:(<@:(,:"0);._1) 1 2 3 1 2 1 2




epilog''

