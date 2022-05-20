prolog './gimaxmin.ijs'
NB. i.<./ ---------------------------------------------------------------

test=: 3 : 0
 yy=: y
 assert. ((i.<./)yy) -: yy i.<./yy
 assert. ((i.>./)yy) -: yy i.>./yy
 assert. ((i:<./)yy) -: yy i:<./yy
 assert. ((i:>./)yy) -: yy i:>./yy
 1
)

test   1e3 ?@$ imax
test - 1e3 ?@$ imax
test (<.imax%_2) + 1e3 ?@$ imax
test   1e3 $ imax
test   1e3 $ imin

test   1e3 ?@$ 0
test - 1e3 ?@$ 0
test _10p1 + 1e3 ?@$ 60
test   1e3 $  1.234
test   1e3 $ _1.234

test   1e3 ?@$ 2
test   1e3 $ 0
test   1e3 $ 1

test i.0
test 0$0.5
test 100 3 ?@$ 4

NB. With new code, test all permutations of smallest element, repeated and not, with different spacings
*./@:,@:(test"1)@:(|.~"_ 0 i.@#)@> (; -)@> ('';0;0 0;0 0 0;0 20;0 20 21;0 20 21 22;0 20 21 22 23) ,&.>/ <@i."0 i. 10
*./@:,@:(test"1)@:(|.~"_ 0 i.@#)@> (; -)@> (0.5&+)&.> ('';0;0 0;0 0 0;0 20;0 20 21;0 20 21 22;0 20 21 22 23) ,&.>/ <@i."0 i. 10
 
'domain error' -: (i.<./) etx 'abc'
'domain error' -: (i.<./) etx u:'abc'
'domain error' -: (i.<./) etx 10&u:'abc'
'domain error' -: (i.<./) etx 1j2 3 4 
'domain error' -: (i.<./) etx 1;2;3 4 


4!:55 ;:'test yy'



epilog''

