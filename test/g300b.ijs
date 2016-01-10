NB. boolean inner products, +./ .*. and ~:/ .*. , etc. ------------------

ip=: 4 : 0
 assert. x (~:/ .=  -: ~:/@(= "1 _)) y
 assert. x (~:/ .~: -: ~:/@(~:"1 _)) y
 assert. x (~:/ .+. -: ~:/@(+."1 _)) y
 assert. x (~:/ .+: -: ~:/@(+:"1 _)) y
 assert. x (~:/ .*. -: ~:/@(*."1 _)) y
 assert. x (~:/ .*: -: ~:/@(*:"1 _)) y
 assert. x (~:/ .<  -: ~:/@(< "1 _)) y
 assert. x (~:/ .<: -: ~:/@(<:"1 _)) y
 assert. x (~:/ .>  -: ~:/@(> "1 _)) y
 assert. x (~:/ .>: -: ~:/@(>:"1 _)) y
 assert. x (+./ .=  -: +./@(= "1 _)) y
 assert. x (+./ .~: -: +./@(~:"1 _)) y
 assert. x (+./ .+. -: +./@(+."1 _)) y
 assert. x (+./ .+: -: +./@(+:"1 _)) y
 assert. x (+./ .*. -: +./@(*."1 _)) y
 assert. x (+./ .*: -: +./@(*:"1 _)) y
 assert. x (+./ .<  -: +./@(< "1 _)) y
 assert. x (+./ .<: -: +./@(<:"1 _)) y
 assert. x (+./ .>  -: +./@(> "1 _)) y
 assert. x (+./ .>: -: +./@(>:"1 _)) y
 assert. x (*./ .=  -: *./@(= "1 _)) y
 assert. x (*./ .~: -: *./@(~:"1 _)) y
 assert. x (*./ .+. -: *./@(+."1 _)) y
 assert. x (*./ .+: -: *./@(+:"1 _)) y
 assert. x (*./ .*. -: *./@(*."1 _)) y
 assert. x (*./ .*: -: *./@(*:"1 _)) y
 assert. x (*./ .<  -: *./@(< "1 _)) y
 assert. x (*./ .<: -: *./@(<:"1 _)) y
 assert. x (*./ .>  -: *./@(> "1 _)) y
 assert. x (*./ .>: -: *./@(>:"1 _)) y
 1
)

test=: 4 : 0
 xx=: x ?@$ 2
 yy=: y ?@$ 2
 assert. xx ip yy
 xx=: (|.y) ?@$ 2
 yy=: (|.x) ?@$ 2
 assert. xx ip yy
 1
)

''    test ''
''    test 16
''    test 17
''    test 13 16
''    test 13 17
''    test 1 2 8
''    test 1 3 7
13    test 13
13    test 13 0
13    test 13 1
13    test 13 16
13    test 13 17
16    test 16
16    test 16 0
16    test 16 1
16    test 16 16
16    test 16 17
13 16 test 16 0
13 16 test 16 1
13 16 test 16 19
13 16 test 16 24
13 17 test 17 0
13 17 test 17 1
13 17 test 17 19
13 17 test 17 24
16 16 test 16 19
16 16 test 16 24
16 17 test 17 19
16 17 test 17 24

0 -: 0     +./ .*. 0
0 -: 0     +./ .*.~0
0 -: 0 0 0 +./ .*. 0
0 -: 0 0 0 +./ .*.~0
0 -: 0 0 0 +./ .*. 0 0 0

0 -: 0     ~:/ .*. 0
0 -: 0     ~:/ .*.~0
0 -: 0 0 0 ~:/ .*. 0
0 -: 0 0 0 ~:/ .*.~0
0 -: 0 0 0 ~:/ .*. 0 0 0

sh =. $@(+./ .*.) -: }:@$@[ , }.@$@]

1              sh 0
1              sh ,0
0              sh 12    ?@$ 2
1              sh 1 4 5 ?@$ 2
1              sh i.0
0              sh i.0 0 0
(,0)           sh ,1
(,0)           sh 1 4 5 1 ?@$ 2
(,0)           sh 1 0 0   ?@$ 2
(i.0)          sh 0
(i.0)          sh i.0
(i.0)          sh i.0 3 4 5
''             sh i.59$0
(i.3 0)        sh i.0 5
(i.3 0 0)      sh i.0
(i.0 5 0)      sh i.12$0
(4 5 1 ?@$ 2)  sh i.1 2 0

'domain error' -: (3 4 ?@$ 2) +./ .*. etx 4 5$'c'
'domain error' -: (3 4 ?@$ 2) +./ .*. etx 4 5$<'c'
'domain error' -: (3 4 ?@$ 2) +./ .*. etx u: 4 5 ?@$ 100

'domain error' -: (4 3$'c'  ) +./ .*. etx 3 4 ?@$ 2
'domain error' -: (4 3$<'c' ) +./ .*. etx 3 4 ?@$ 2
'domain error' -: (4 3$u: 12) +./ .*. etx 3 4 ?@$ 2

'length error' -: (3 4 ?@$ 2) +./ .*. etx 5$1
'length error' -: (3 4 ?@$ 2) +./ .*. etx 3 4 ?@$ 2
'length error' -: (3 4$5)   +./ .*. etx ,1
'length error' -: (3 4$5)   +./ .*. etx i.1 4
'length error' -: (3 4$5)   +./ .*. etx i.1 0 4
'length error' -: (3 4$5)   +./ .*. etx i.0 4
'length error' -: (3 4$5)   +./ .*. etx i.0

'length error' -: (i.0)     +./ .*. etx i.3 4
'length error' -: (i.0)     +./ .*. etx 3 4 5
'length error' -: (i.0)     +./ .*. etx i.3 4 0
'length error' -: (i.0)     +./ .*. etx ,3j4
'length error' -: (i.3 4 0) +./ .*. etx i.3 4

'length error' -: (i.1)     +./ .*. etx i.3 4
'length error' -: (i.1)     +./ .*. etx 3 4 5
'length error' -: (i.1)     +./ .*. etx i.3 4 0
'length error' -: (i.3 4 1) +./ .*. etx i.3 4
'length error' -: (i.3 4 1) +./ .*. etx i.3 0 0


4!:55 ;:'ip sh test xx yy'


