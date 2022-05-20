prolog './g320ipt.ijs'
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

x =: (QKTEST{800 400) * 2^i.8
y =: timer 'h1 ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
THRESHOLD +. threshold < y rsq y1

tbase =. 6!:2 'i. 1e6'
tip =: tbase -~ 6!:2 '1 ,~ 2 ,~ 3 ,~ 4 ,~ 5 ,~ 6 ,~ 7 ,~ i. 1e6'
tnip =: tbase -~ 6!:2 '1 , 2 , 3 , 4 , 5 , 6 , 7 , i. 1e6'
THRESHOLD +. tip < 0.25 * tnip

tip =: tbase -~ 6!:2 '1 ,!.0~ 2 ,!.0~ 3 ,!.0~ 4 ,!.0~ 5 ,!.0~ 6 ,!.0~ 7 ,!.0~ i. 1e6'
tnip =: tbase -~ 6!:2 '1 ,!.(0) 2 ,!.(0) 3 ,!.(0) 4 ,!.(0) 5 ,!.(0) 6 ,!.(0) 7 ,!.(0) i. 1e6'
THRESHOLD +. tip < 0.25 * tnip

tbase =. 6!:2 '<"0 i. 1e6'
tip =: tbase -~ 6!:2 '(<1) ,~ (<2) ,~ (<3) ,~ (<4) ,~ (<5) ,~ (<6) ,~ (<7) ,~ <"0 i. 1e6'
tnip =: tbase -~ 6!:2 '(<1) , (<2) , (<3) , (<4) , (<5) , (<6) , (<7) , <"0 i. 1e6'
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

NB. Verify that AFNOSMREL is propagated through structural verbs
f =: 1 : 0
u y
timenoa =: 6!:2 'u y'  NB. cost is verb time + free time.  free time is small unlees the verb runs EPILOGs
timea =: 6!:2 'b =: u y'  NB. cost is verb time period - should be no free or REL cost
4!:55<'b'
NB. timea < timenoa +1e_4
THRESHOLD +. threshold < (timenoa +1e_4) rsq timea
)
a =: 1000 $ < <"0 i. 1000
4!:55 <'b'

> f a
999&$ f a
|. f a
50&|. f a
|: f a
, f a
a:&, f a
,&a: f a
,. f a
,.&a f a
,: f a
,:&a: f a
;&a f a
(i. 1000)&{ f a
999&{. f a
{. f a
}: f a
}. f a
}: f a
1&}. f a
-.&a: f a
;&a f a

4!:55 ;:'a f f1 g h h1 mean q r rsq ss tbase timenoa timea tip tnip x x1 y y1'



epilog''

