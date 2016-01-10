NB. 128!:5 --------------------------------------------------------------

isnan=: 128!:5

(($x)$0) -: isnan x=: 31 17 3 ?@$ 2
(($x)$0) -: isnan x=: 31    3 ?@$ 2e9
(($x)$0) -: isnan x=: 31      ?@$ 0
(($x)$0) -: isnan x=: j./2 31 ?@$ 0
(($x)$0) -: isnan x=: 31 17   ?@$ 200x
(($x)$0) -: isnan x=: % /2 31 ?@$ 20x
(($x)$0) -: isnan x=: 'Barack Obama vs. Hillary Clinton'
(($x)$0) -: isnan x=: ;:'John McCain'

(($x)$0) -: isnan x=: _ __ 567
(($x)$0) -: isnan x=: _ __ 5j7

((i.#x) e. i) -: isnan _.      (i=: 100 ?@$ #x)}x=: 1e4 ?@$ 0
((i.#x) e. i) -: isnan (<_.  ) (i=: 100 ?@$ #x)}x=: 1e4 $ a:
((i.#x) e. i) -: isnan (<<<_.) (i=: 100 ?@$ #x)}x=: 1e4 $ a:

1 = isnan <^:10 ] _.
0 = isnan <20 ?@$ 2
0 = isnan <20 ?@$ 2e9
0 = isnan <'abc'

x=: 1e6 ?@$ 0
b=: (#x) ?@$ 2
x=: (-~0j1)+_. (I.b)}x
b -: isnan x

'domain error' -: 0 (128!:5) etx 3 4 5
'domain error' -: 1 (128!:5) etx 3 4 5


NB. 128!:5 and mapped boxed arrays --------------------------------------

0!:0 <testpath,'gmbx.ijs'

r=: x=: 1 2 3 ; _. ; 3j4 _. ; 'abc'
(isnan x) -: isnan r

r=: x=: (5!:1 <'mean') ; _. ; (<<<3j4 _.) ; 'abc'
(isnan x) -: isnan r

0 = unmap_jmf_ 'q'
0 = unmap_jmf_ 'r'


4!:55 ;:'b f f1 g i isnan mean q r x'
