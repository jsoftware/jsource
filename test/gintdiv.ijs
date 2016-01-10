NB. integer division and remainder --------------------------------------

f1=: 4 : 0
 assert. (  x  <.@%  y) -: <.  x  %  y
 assert. (  x  <.@% -y) -: <.  x  % -y
 assert. ((-x) <.@%  y) -: <.(-x) %  y
 assert. ((-x) <.@% -y) -: <.(-x) % -y
 assert. (  x  >.@%  y) -: >.  x  %  y
 assert. (  x  >.@% -y) -: >.  x  % -y
 assert. ((-x) >.@%  y) -: >.(-x) %  y
 assert. ((-x) >.@% -y) -: >.(-x) % -y
 1
)

(   300 ?@$ 100) f1 32
(32*300 ?@$ 100) f1 32
(   300 ?@$ 100) f1 17
(17*300 ?@$ 100) f1 17

(   2 300 ?@$ 100) f1 32 17
(   2 300 ?@$ 100) f1~32 17
(32*2 300 ?@$ 100) f1 32 17
(32*2 300 ?@$ 100) f1~32 17
(17*2 300 ?@$ 100) f1 32 17
(17*2 300 ?@$ 100) f1~32 17

3 : 0 ''
 if. -. IF64 do.
  imax f1 32
  imax f1~32
  imax f1 17
  imax f1~17
  imax f1 32 17
  imax f1~32 17
  imin f1 32
  imin f1~32
  imin f1 17
  imin f1~17
  imin f1 32 17
  imin f1~32 17
 end.
 1
)

f2=: 4 : 0
 assert. (x|t) -: t - x * t <.@% x [ t=.  y
 assert. (x|t) -: t - x * t <.@% x [ t=. -y
 1
)

(2^i.12) f2"0 _ y=: 400 ?@$ 200
(2^i.12) f2"0 _ y=: 400 ?@$ 2e5

3 : 0 ''
 if. -. IF64 do.
  (2^i.12) f2"0 _ imin,_1 0 1,imax
  (2^i.12) f2"0 _~imin,_1 0 1,imax
 end.
 1
)

f3=: 4 : 0
 assert. ((0,x)#: y) -: (<.  y %x),"0 x| y
 assert. ((0,x)#:-y) -: (<.(-y)%x),"0 x|-y
 1
)

(2^i.12) f3"0 _ y=: 4000 ?@$ 200
(2^i.12) f3"0 _ y=: 4000 ?@$ 2e5

3 : 0 ''
 if. -. IF64 do.
  (2^i.12) f3"0 _ imin,_1 0 1,imax
 end.
 1
)


4!:55 ;:'f f1 f2 f3 y'



