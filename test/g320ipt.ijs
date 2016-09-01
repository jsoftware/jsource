NB. x,y Append in place timing tests ------------------------------------

ss   =: +/ @: *:
rsq  =: [: -. ss@(- +/ % #)@[ %~ ss@:-

f=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. z,i
 end.
)

(i. -: f)"0 ] 2 10?@$2e3

x =: 800 * 2^i.8
y =: timer 'f ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
THRESHOLD +. threshold < y rsq y1

h=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. i ,~ z
 end.
)

(i. -: h)"0 ] 2 10?@$2e3

x =: 800 * 2^i.8
y =: timer 'h ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
THRESHOLD +. threshold < y rsq y1

h1=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. i ,!.5~ z
 end.
)

x =: 800 * 2^i.8
y =: timer 'h1 ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
THRESHOLD +. threshold < y rsq y1

x =: i. 1e6
tip =: 6!:2 '1 ,~ 2 ,~ 3 ,~ 4 ,~ 5 ,~ 6 ,~ 7 ,~ x'
tnip =: 6!:2 '1 , 2 , 3 , 4 , 5 , 6 , 7 , x'
THRESHOLD +. tip < 0.25 * tnip

tip =: 6!:2 '1 ,!.0~ 2 ,!.0~ 3 ,!.0~ 4 ,!.0~ 5 ,!.0~ 6 ,!.0~ 7 ,!.0~ x'
tnip =: 6!:2 '1 ,!.(0) 2 ,!.(0) 3 ,!.(0) 4 ,!.(0) 5 ,!.(0) 6 ,!.(0) 7 ,!.(0) x'
THRESHOLD +. tip < 0.25 * tnip

0!:0 <testpath,'gmbx.ijs'

g=: 3 : 0
 q=: i.0
 for_i. i.y do.
  q=: q,i
 end.
)

(i. -: g)"0 ] 2 10?@$2e3

x =: 800 * 2^i.8
y =: timer 'g ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
THRESHOLD +. threshold < y rsq y1

1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'


4!:55 ;:'f f1 g h h1 mean q r rsq ss tip tnip x x1 y y1'


