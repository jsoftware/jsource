NB. 6!: -----------------------------------------------------------------

ts   =: 6!:0
tss  =: 6!:1
time =: 6!:2
dl   =: 6!:3

t =: ts ''
6 = $t
(-: <.) 5{.t
*./0<:t
1990<:0{t
*./ (1&<: *. <:&12 31) 1 2{t
24 60 60 *./ . > 3 4 5{t

NB. 0.5>|(x=:t-~tss '')-[dl 5 [ t=:tss ''

0<:time 't=:+/i.5000'
t -: +/i.5000
NB. 0.5>|5-x=:time 'dl 5'

'domain error' -: time etx 0 1     
'domain error' -: time etx 3 4 _5    
'domain error' -: time etx 3j4        
'domain error' -: time etx 3;4 5 

'domain error' -: 'j'   time etx '3+4' 
'domain error' -: 3.4   time etx '3+4' 
'domain error' -: 3j4   time etx '3+4'  
'domain error' -: (3;4) time etx '3+4'
'domain error' -: 3r4   time etx '3+4'

'domain error' -: dl etx '5'
'domain error' -: dl etx 3j4        
'domain error' -: dl etx <3


NB. 6!:8 and 6!:9  ------------------------------------------------------

qpf=: 6!:8
qpc=: 6!:9

x=: qpf ''
8 = type x
0 = #$x
0 < x

x=: qpc"1 i.21 0
8 = type x
1 = #$x
0 <: x
(}.x) >: }:x


NB. 6!:10 11 12 13 ------------------------------------------------------

bpe     =: IF64{28 56   NB. bytes per entry
bhdr    =: IF64{16 32   NB. bytes for header

pmdata  =: 6!:10
pmunpack=: 6!:11
pmctr   =: 6!:12
pmstats =: 6!:13

0 = pmdata ''
0 = 5{pmstats ''
'domain error' -: pmctr etx 0

avg=: 3 : 0
 n=. #y
 s=. +/ y
 s % n
)

200 -: x=: pmdata (bhdr+bpe*200)$'c'

1 = x=: pmctr 1
1 = 5{pmstats ''
n=: 1+?35
1 [ avg"1 ?(n,11)$1000
0 = x=: pmctr _1

s=: pmstats ''
t=: pmunpack ''

4 = type s
s -: 0 0 200,(200<.4+2*n),(200<:4+2*n),0

f=: 4 : 0
 t=. y
 assert. (32=type t) *. ((,7) -: $t) *. (*./ 1 = #@$&>t) *. *./ x = 6{.#&>t
 assert. (type&.>t) e.&> 4;4;4;4;1 4 8;1 4 8;32
 assert. *./ (>0{t) e. i.#>6{t
 assert. *./ (>1{t) e. i.#>6{t
 assert. *./ (>2{t) e. 1 2
 assert. (xx -: <.xx) *. *./ 0 <: xx=: >4{t
 assert. (*./ 0<:xx) *. *./ (}.xx) >: }:xx=: >5{t
 assert. (*./ (xx e. a:) +. 2=type&>xx) *. *./ 1 = #@$&>xx=: >6{t
 1
)

(4+2*n) f t
({&:>/0 6{t) -: (1 2,(2*n),1)#;:'pmctr pmstats avg pmctr'
({&:>/1 6{t) e. <'base'
(>3{t) -: _2 _1 _2,((2*n)$_1 _2),_1

200 -: x=: 1 0 pmdata (bhdr+bpe*200)$'c'

1 = x=: pmctr 1
1 = 5{pmstats ''
n=: 1+?35
1 [ avg"1 ?(n,11)$1000
0 = x=: pmctr _1

s=: pmstats ''
t=: pmunpack ''

4 = type s
s -: 1 0 200,(200<.4+5*n),(200<:4+5*n),0

(4+5*n) f t
({&:>/0 6{t) -: (1 2,(5*n),1)#;:'pmctr pmstats avg pmctr'
({&:>/1 6{t) e. <'base'
(>3{t) -: _2 _1 _2,((5*n)$_1 0 1 2 _2),_1

sum_foo_=: 3 : 0
 +/y
)

sum_z_=: sum_foo_

mean_l6x_=: 3 : 0
 (sum y) % #y
)

100 -: 1 pmdata (bhdr+bpe*100)$'x'
1 -: pmctr 1
1 [ mean_l6x_ i.12
0 -: pmctr _1
t=: 6!:11 ''
10 f t
({&:>/0 6{t) -: ;:'pmctr mean_l6x_ mean_l6x_ sum sum_foo_ sum_foo_ sum_foo_ sum mean_l6x_ pmctr'
({&:>/1 6{t) -: ;:'base  l6x       l6x       l6x foo      foo      foo      l6x l6x       base'

mean_aa_=: 3 : 0
 sum=. +/
 (sum y) % #y
)

100 -: 1 pmdata (bhdr+bpe*100)$'x'
1 -: pmctr 1
1 [ mean_aa_ i.12
0 -: pmctr _1
t=: 6!:11 ''
8 f t
({&:>/0 6{t) -: ;:'pmctr mean_aa_ mean_aa_ mean_aa_ sum sum mean_aa_ pmctr'
({&:>/1 6{t) -: ;:'base  aa       aa       aa       aa  aa  aa       base'

f=: 255$'a'
12 -: ". f,'=: 12'
4!:55 <f

sp=: 7!:0
m=: sp ''
m=: sp ''

". f,'=: 3 : ''(+/y) % #y'''
20000 -: x=: pmdata (bhdr+bpe*20000)$'c'
1 = pmctr 1
1 [ ". f,' i.1234'
0 = pmctr _1
1 [ pmunpack ''
0 = pmdata ''
4!:55 <f

100 > (sp '') - m

". f,'=: 3 : ''(+/y) % #y'''
100 -: pmdata (bhdr+bpe*100)$'c'
1 = pmctr 1
1 [ ". f,'"1 i.100 14'
0 = pmctr _1
0 0 100 100 1 0 -: pmstats ''
1 [ pmunpack ''
0 = pmdata ''
4!:55 <f

100 > (sp '') - m

0 -: pmdata ''

'domain error' -: pmdata etx 1 0 1
'domain error' -: pmdata etx 1 2 3
'domain error' -: pmdata etx 1.2 3
'domain error' -: pmdata etx 1j2 3
'domain error' -: pmdata etx 1 2 3x
'domain error' -: pmdata etx 1r2 3

'rank error'   -: pmdata etx 3 20$'a'

'domain error' -: 1 2     pmdata etx 20$'x'
'domain error' -: 1.2     pmdata etx 20$'x'
'domain error' -: 1j2     pmdata etx 20$'x'
'domain error' -: 1 2x    pmdata etx 20$'x'
'domain error' -: 1r2     pmdata etx 20$'x'
'domain error' -: '01'    pmdata etx 20$'x'
'domain error' -: (1;0)   pmdata etx 20$'x'

'rank error'   -: (1 2$0) pmdata etx 20$'x'

'length error' -: 0 1 0   pmdata etx 20$'x'

'domain error' -: pmctr etx 1.5
'domain error' -: pmctr etx 1j5
'domain error' -: pmctr etx 10^100x
'domain error' -: pmctr etx 1r5
'domain error' -: pmctr etx <2

'rank error'   -: pmctr etx 1 2

'domain error' -: pmunpack etx ''

1 [ pmdata 5000$'c'
1 = pmctr 1
1 [ avg"1 i.3 12
0 = pmctr _1
7 -: # pmunpack ''

'domain error' -: pmunpack etx 1.2 3
'domain error' -: pmunpack etx 1j2 3
'domain error' -: pmunpack etx 1r2 3
'domain error' -: pmunpack etx 1;2 3

'index error'  -: pmunpack etx 8
'index error'  -: pmunpack etx _9

0 -: pmdata ''

18!:55 ;:'aa l6x foo'

4!:55 ;:'avg bhdr bpe dl f m n '
4!:55 ;:'pmctr pmdata pmstats pmunpack qpc qpf '
4!:55 ;:'s sp sum_z_ t time ts tss x xx '


