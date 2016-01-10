NB. 8!:n ----------------------------------------------------------------

NB. require 'numeric'           NB. for round
round=: [ * [: <. 0.5 + %~

afzrnd=: *@] * [ round |@]  NB. away from zero rounding 

fmt =: 8!:0
fmt1=: 8!:1
fmt2=: 8!:2
NB.    or 
NB. load j model

t=: '' fmt y=: 0.01 * _2000 + 4 5 ?@$ 10000
32=type t
($t) -: $y
2=type&>t
-. ' 'e.&>t
-. '_'e.&>t
(0>y)='-'={.&>t

('' fmt i.0) -: '' fmt ''
('' fmt s$0) -: '' fmt s$'' [ s=: 0 (?5)}5 ?@$ 10

(":&.>y) -: '0' fmt y=:     10 ?@$ 100
(":&.>y) -: '0' fmt y=: 2 3 10 ?@$ 100

('domain error';'assertion failure') e.~< 3              fmt  etx i.2 3
('domain error';'assertion failure') e.~< 'cc'           fmt  etx i.2 3
('domain error';'assertion failure') e.~< 'clc'          fmt  etx i.2 3
('domain error';'assertion failure') e.~< 'p<x>r<*>p(y)' fmt  etx i.2 3
('domain error';'assertion failure') e.~< 'p<x({open[})' fmt  etx i.2 3

('rank error'  ;'assertion failure') e.~< ''      fmt1 etx i.2 3 4
('rank error'  ;'assertion failure') e.~< ''      fmt2 etx i.2 3 4

('length error';'assertion failure') e.~< ',,'    fmt  etx i.2 4
('length error';'assertion failure') e.~< (3$<'') fmt  etx i.2 4

('rank error'  ;'assertion failure') e.~< ''      fmt  etx 2  ;2 3$'abc'
('rank error'  ;'assertion failure') e.~< ''      fmt  etx 2 3;'abc'


NB. 8!:n : various right arguments --------------------------------------

(<@,"1 x) -: '' fmt x=: 'abc'
(<@,"1 x) -: '' fmt x=: a.
(<@,"1 x) -: '' fmt x=: a. {~ 2 3 4 ?@$ #a.
(<@,"1 x) -: '' fmt x=: 2 3 0 4$''
(<@,"1 x) -: '' fmt x=: 2 3 0 4$0

('' fmt 123;'') -: '' fmt 123;i.0


NB. 8!:n : insufficient width -------------------------------------------

(<8$'*') -: '8.2'  fmt  123456.7
(<8$'*') -: 'c8.2' fmt  12345.7
(<8$'*') -: '8.2'  fmt  _12345
(<8$'*') -: 'c8.2' fmt  _1234


NB. 8!:n : b modifier ---------------------------------------------------

(2$<'nil')       -: 'b<nil>2' fmt 0.004 _0.004
(2$<'')          -: 'b2'      fmt 0.004 _0.004

('0.01';'-0.01') -: 'b<nil>2' fmt 0.005 _0.005
('0.01';'-0.01') -: 'b<nil>2' fmt 0.006 _0.006


NB. 8!:n : c modifier ---------------------------------------------------

(< '1,234.56') -: 'c2' fmt 1234.56
(<'-1,234.56') -: 'c2' fmt _1234.56

(,'1')           -: > 'c' fmt  1
 '12'            -: > 'c' fmt  12
 '123'           -: > 'c' fmt  123
 '1,234'         -: > 'c' fmt  1234
 '12,345'        -: > 'c' fmt  12345
 '123,456'       -: > 'c' fmt  123456
 '1,234,567'     -: > 'c' fmt  1234567
 '12,345,678'    -: > 'c' fmt  12345678
 '123,456,789'   -: > 'c' fmt  123456789
 '1,234,567,890' -: > 'c' fmt  1234567890
 '1.234567890e10'-: > 'c' fmt  12345678901

(,'-1')          -: > 'c' fmt _1
'-12'            -: > 'c' fmt _12
'-123'           -: > 'c' fmt _123
'-1,234'         -: > 'c' fmt _1234
'-12,345'        -: > 'c' fmt _12345
'-123,456'       -: > 'c' fmt _123456
'-1,234,567'     -: > 'c' fmt _1234567
'-12,345,678'    -: > 'c' fmt _12345678
'-123,456,789'   -: > 'c' fmt _123456789
'-1,234,567,890' -: > 'c' fmt _1234567890
'-1.234567890e10'-: > 'c' fmt _12345678901

('domain error';'assertion failure') e.~< 'c<xx>' fmt etx 2 3 4


NB. 8!:n : l modifier ---------------------------------------------------

(<'234.56   ') -: 'l9.2' fmt  234.56
(<'   234.56') -:  '9.2' fmt  234.56

(<'-234.56  ') -: 'l9.2' fmt _234.56
(<'  -234.56') -:  '9.2' fmt _234.56

NB. ('domain error';'assertion failure') e.~< 'l3'  fmt  etx 2 3 4
('domain error';'assertion failure') e.~< 'l<xx>' fmt etx 2 3 4


NB. 8!:n : m & n modifiers ----------------------------------------------

(<'mmm123.45nn') -: 'm<mmm>n<nn>'  fmt  _123.45
(<,'0')          -: 'm<mmm>n<nn>'  fmt  0
(<'0.00')        -: 'm<mmm>n<nn>2' fmt   0.004
(<'0.00')        -: 'm<mmm>n<nn>2' fmt  _0.004

(<'123.45')      -: 'm<mmm>n<nn>'  fmt 123.45


NB. 8!:n : p & q modifiers ----------------------------------------------

(<'ppp123.45qq') -: 'p<ppp>q<qq>'  fmt  123.45
(<'ppp0qq')      -: 'p<ppp>q<qq>'  fmt  0
(<'ppp0.00qq')   -: 'p<ppp>q<qq>2' fmt   0.004
(<'ppp0.00qq')   -: 'p<ppp>q<qq>2' fmt  _0.004

(<'-123.45')     -: 'p<ppp>q<qq>'  fmt _123.45


NB. 8!:n : r modifier ---------------------------------------------------

'abababa12345.67' -: > 'r<ab>15.2'       fmt y=: 12345.67
'ababa++12345.67' -: > 'p<++>r<ab>15.2'  fmt y
'ababab-12345.67' -: > 'r<ab>15.2'       fmt -y
'ababa12345.67--' -: > 'n<-->r<ab>15.2'  fmt -y
'12345.67abababa' -: > 'lr<ab>15.2'      fmt y
'ababab12,345.67' -: > 'cr<ab>15.2'      fmt y


NB. 8!:n : s modifier ---------------------------------------------------

y=: 0.01 * _3000 + 4 5 ?@$ 10000

(0>y)='~'={.&>'s<-~>' fmt y

s=: ''        fmt y
t=: 's<.!>'   fmt y
s ([: *./ = +. '.'&i.@[ = '!'&i.@])&> t

s=: ''        fmt y
t=: 's<-~.!>' fmt y
s ([: *./ = +. '-.'&i.@[ = '~!'&i.@])&> t

(<'1 234,56') -: 'cs<, .,>2' fmt 1234.56
(<'1.234^19') -: 's<e^>'     fmt 1.234e19
(<'!!!!!!!!') -: 'cs<*!>8.2' fmt 12345

('domain error';'assertion failure') e.~< 's< $>'   fmt etx i.2 3
('domain error';'assertion failure') e.~< 's<1 >'   fmt etx i.2 3

('domain error';'assertion failure') e.~< 's<.>'    fmt etx i.2 3
('domain error';'assertion failure') e.~< 's<. *>'  fmt etx i.2 3
('domain error';'assertion failure') e.~< 's<. .,>' fmt etx i.2 3


NB. 8!:n : # of decimal places ------------------------------------------

(<x) -: '' fmt ".x=: '5',''
(<x) -: '' fmt ".x=: '5.1'
(<x) -: '' fmt ".x=: '5.12'
(<x) -: '' fmt ".x=: '5.123'
(<x) -: '' fmt ".x=: '5.1234'
(<x) -: '' fmt ".x=: '5.12345'
(<x) -: '' fmt ".x=: '5.123456'
(<x) -: '' fmt ".x=: '5.1234567'
(<x) -: '' fmt ".x=: '5.12345678'
(<x) -: '' fmt ".x=: '5.123456789'

(< '12') -: '' fmt  12
(<'-12') -: '' fmt _12
(<,'0' ) -: '' fmt 0

NB. the next line occassionally fails for having the "wrong" value of x on the
NB. left side 'cause it pastes in the value from above.  This happens when you
NB. run 0!:2 from inside an explicit verb.   Presumably this is because of x=.
NB. vs x=:  I believe this is actually the correct behavior.
x -: ''&fmt@".&> x=:  '1.'         &,&.> (}.i.10){.&.> <'123456789'
x -: ''&fmt@".&> x=:  '12345.'     &,&.> (}.i.10){.&.> <'123456789'
x -: ''&fmt@".&> x=:  '1234567890.'&,&.> (}.i.5 ){.&.> <'123456789'

x -: ''&fmt@".&> x=: '-1.'         &,&.> (}.i.10){.&.> <'123456789'
x -: ''&fmt@".&> x=: '-12345.'     &,&.> (}.i.10){.&.> <'123456789'
x -: ''&fmt@".&> x=: '-1234567890.'&,&.> (}.i.5 ){.&.> <'123456789'


NB. 8!:n : various data types -------------------------------------------

('' fmt y) -: '' fmt x:       y=: 0.01 * 3 4 ?@$ 1000
('' fmt y) -: '' fmt (-~0.5)+ y=:        3 4 ?@$ 2
('' fmt y) -: '' fmt (-~0.5)+ y=:        3 4 ?@$ 1000
('' fmt y) -: '' fmt (-~0j5)+ y=: 0.01 * 3 4 ?@$ 1000
('' fmt y) -: '' fmt $.       y=: 0.01 * 3 4 ?@$ 1000

(( u:'5') fmt y) -:   '5'  fmt y=: 3 4 ?@$ 0
((<u:'5') fmt y) -: (<'5') fmt y=: 3 4 ?@$ 0

('domain error';'assertion failure') e.~< '' fmt etx 1 2 3 4j5

('rank error';  'assertion failure') e.~< '' fmt etx 1 2;3
('rank error';  'assertion failure') e.~< '' fmt etx 1;i.2 3

('rank error';  'assertion failure') e.~< (3 3$'4') fmt etx i.2 3


NB. 8!:n : miscellaneous regression checks ------------------------------

('' fmt y) -: ($0) fmt y=: 0.01 * _2000 + 4 5 ?@$ 10000

(<@,"0 '01234') -: fmt i.5

a: -:    fmt ''
a: -: '' fmt ''

('3e9';,'3') -: '' fmt 3e9 3
('3e9';,'3') -:    fmt 3e9 3

(<;._1' na1 __ na2 na3e-10 3e-10') -: 'm<na>s<-->' fmt _1 __ _2 _3e_10 3e_10
(<;._1' na1 __ na2 na3e-10 3e-10') -: 'm<na>'      fmt _1 __ _2 _3e_10 3e_10

((0$a:)&fmt -: ''&fmt) 1p1
((0$a:)&fmt -: ''&fmt) i.3

(,3) -: $'' fmt1 i.3
1 3 -: $fmt2 i.3
1 0 -: $fmt2 i.0

((<y) -: x fmt y)*. ((,<,:y) -: x fmt1 y)*. (,:y) -: x fmt2 y=:'abc' [ x=:1p1
((<y) -: x fmt y)*. ((,<,:y) -: x fmt1 y)*. (,:y) -: x fmt2 y=:'abc' [ x=:0j1
((<y) -: x fmt y)*. ((,<,:y) -: x fmt1 y)*. (,:y) -: x fmt2 y=:'abc' [ x=:'s<X>'

(<'_3') -: 's<-_>' fmt _3

(<14{.x) -: 'c' fmt 0".x=:'-219.47193390041042'

('12345.0';'0.0';'0.5') -: '' fmt 12345 0 0.5
(<;._1 x) -: '' fmt ".x=:' 889.460 219.138 786.039 922.395 991.019'

NB. 8!:n : d modifier ---------------------------------------------------

(<,'_') -:  '' fmt _
(<,'__') -: '' fmt __
(<,'_.') -: '' fmt _.
((,'_');'__';'_.';,'0') -: fmt _ __ _. 0

(''&fmt -: '3'&fmt) x=:_ 0.123 __
(''&fmt -: '0'&fmt)    _ 0 4   __ _. 1 2 3

(3$<'BOOM') -: 'd<BOOM>' fmt _ __ _.

s=: 'd<n/a>' fmt y {~ x=: 1000 ?@$ 100 [ y=: _ __ _. , 0.01 * 97 ?@$ 10000
(<'n/a') -:"0 s #~ x e. i.3 
(s-.<'n/a') -: '' fmt y{~ x -. i.3

s=: 'd<n/a>' fmt y {~ x=: 1000 ?@$ 100 [ y=: _ __ _. ,        97 ?@$ 0
(<'n/a') -:"0 s #~ x e. i.3 
(s-.<'n/a') -: '' fmt y{~ x -. i.3


NB. 8!:n : rank 1 right arguments ---------------------------------------

(<@,"0 '012') -: '' fmt i.3
(<@,:@,"0 '012') -: '' fmt1 i.3
(,: '012') -: '' fmt2 i.3
2 = #@$ '' fmt2 i.3
2 = #@$ '' fmt2 i.0


NB. 8!:n : # of decimal places ------------------------------------------

(<11{.x) -: '' fmt ".x=: '5.123456789'
(<11{.x) -: '' fmt ".x=: '5.1234567891'
(<11{.x) -: '' fmt ".x=: '5.12345678912'
(<11{.x) -: '' fmt ".x=: '5.123456789123'
(<11{.x) -: '' fmt ".x=: '5.1234567891234'

(<11{.x) -: '9' fmt ".x=: '5.1234123412'
(<10{.x) -: '8' fmt ".x
(< 9{.x) -: '7' fmt ".x
(< 8{.x) -: '6' fmt ".x
(< 7{.x) -: '5' fmt ".x
(< 6{.x) -: '4' fmt ".x
(< 5{.x) -: '3' fmt ".x
(< 4{.x) -: '2' fmt ".x
(< 3{.x) -: '1' fmt ".x
(< 1{.x) -: '0' fmt ".x

('domain error';'assertion failure') e.~< '10' fmt etx ".x
('domain error';'assertion failure') e.~< '-1' fmt etx ".x
('domain error';'assertion failure') e.~< '_1' fmt etx ".x
('domain error';'assertion failure') e.~< '11' fmt etx ".x
('domain error';'assertion failure') e.~< '20.10' fmt etx ".x
('domain error';'assertion failure') e.~< '20.19' fmt etx ".x


NB. 8!:n : rounding -----------------------------------------------------

(<'0',~}:x-.'8') -: '' fmt ".x=: '5.1234567899'
(<'0',~}:x-.'8') -: '' fmt ".x=: '5.1234567896'
(<'0',~}:x-.'8') -: '' fmt ".x=: '5.1234567895'
(<11{.   x     ) -: '' fmt ".x=: '5.1234567894'
(<'5.123456781') -: '' fmt ".x=: '5.1234567805'

NB. test Chris's rounding examples
(<@":"0@:(0.1&round) -: '1'&fmt) x=:1.05 1.15 1.25 1.35 1.45 1.55 1.65 1.75
(,:'  2,231,834.54') -: 'c14.2' fmt2 o. 710415

NB. check positive & negative 4r10 5r10 6r10 roundings... (away from zero) 
(<@":"0(1& round) x)         -: y=: 'm<_>0' fmt x=: 0.4 + i:5
(<@":"0(1&afzrnd) x)         -: y
('s<-_>'fmt   0+i: 5       ) -: y

(<@":"0(1&afzrnd) x)         -: y=: 'm<_>0' fmt x=: 0.5 + i:5
('s<-_>'fmt (->:i._5),>:i.6) -: y

(<@":"0(1& round) x)         -: y=: 'm<_>0' fmt x=: 0.6 + i:5
(<@":"0(1&afzrnd) x)         -: y
('s<-_>'fmt   1+i: 5       ) -: y

(<@":"0(1& round) x)         -: y=: 'm<_>0' fmt x=:_0.4 + i:5
(<@":"0(1&afzrnd) x)         -: y
('s<-_>'fmt   0+i: 5       ) -: y

(<@":"0(1&afzrnd) x)         -: y=: 'm<_>0' fmt x=:_0.5 + i:5
('s<-_>'fmt (->:i._6),>:i.5) -: y

(<@":"0(1& round) x)         -: y=: 'm<_>0' fmt x=:_0.6 + i:5
(<@":"0(1&afzrnd) x)         -: y
('s<-_>'fmt  _1+i: 5       ) -: y

NB. exponential rounding
'3.123456777e9'   -: >'' fmt 3123456777.4
'3.123456778e9'   -: >'' fmt 3123456777.5
'3.123456778e9'   -: >'' fmt 3123456777.6
'3.123456777e-10' -: >'' fmt 3.1234567774e_10
'3.123456778e-10' -: >'' fmt 3.1234567775e_10
'3.123456778e-10' -: >'' fmt 3.1234567776e_10
'3.123456777e-23' -: >'' fmt 3.1234567774e_23
'3.123456778e-23' -: >'' fmt 3.1234567775e_23
'3.123456778e-23' -: >'' fmt 3.1234567776e_23
'3.123456777e23'  -: >'' fmt 3.1234567774e23
'3.123456778e23'  -: >'' fmt 3.1234567775e23
'3.123456778e23'  -: >'' fmt 3.1234567776e23

NB. in small neighborhoods:
NB. these numbers are adjacent floating point numbers, 
NB. and should have perfect decimal <-> binary conversion
('70940822612508'&,&.> 4 5#'89') -: '0' fmt x=: 709408226125088 + 8 %~ i.9

NB. ugly rounding examples :-)
'806392019.591013' -: > '6' fmt 806392019.5910134315490722656250000000000000000

 709408226.1250885 =!.(0)       709408226.1250884532928466796875000000000000
'709408226.125089' -: > '6' fmt 709408226.1250884532928466796875000000000000

 695833212.8014305 =!.(0)       695833212.8014304637908935546875000000000000
'695833212.801431' -: > '6' fmt 695833212.8014304637908935546875000000000000

'-7369.780882677' -: > '' fmt _7369.7808826765003686887212097644805908203125000
'-7369.780882676' -: > '' fmt _7369.7808826764994591940194368362426757812500000
'-7369.780882676' -: > '' fmt _7369.7808826764985496993176639080047607421875000

'-7425.108822418' -: > '' fmt _7425.1088224183995407656766474246978759765625000

'5.123456790' -: > '' fmt 5.123456789499999608494817948667332530021667480468750
'5.123456789' -: > '' fmt 5.123456789499998720316398248542100191116333007812500
'5.123456789' -: > '' fmt 5.123456789499989838532201247289776802062988281250000

'5.123456790' -:  >'9' fmt 5.123456789500004049386916449293
'5.123456790' -:  >'9' fmt 5.123456789500003161208496749168
'5.123456790' -:  >'9' fmt 5.123456789500002273030077049043
'5.123456790' -:  >'9' fmt 5.123456789500001384851657348918
'5.123456790' -:  >'9' fmt 5.123456789500000496673237648793
 5.1234567895 =!.(0)       5.123456789499999608494817948667
'5.123456790' -:  >'9' fmt 5.123456789499999608494817948667
'5.123456789' -:  >'9' fmt 5.123456789499998720316398248542
'5.123456789' -:  >'9' fmt 5.123456789499997832137978548417
'5.123456789' -:  >'9' fmt 5.123456789499996943959558848292
'5.123456789' -:  >'9' fmt 5.123456789499996055781139148166
'5.123456789' -:  >'9' fmt 5.123456789499995167602719448041


NB. 8!:n : multiple modifiers -------------------------------------------

('domain error';'assertion failure') e.~< 'cc'             fmt etx 10 ?@$ 1e6
('domain error';'assertion failure') e.~< 'b<not>cb'       fmt etx 10 ?@$ 1e6
('domain error';'assertion failure') e.~< 'dd<x>'          fmt etx 10 ?@$ 1e6
('domain error';'assertion failure') e.~< 'p<p>q<q>p<p>'   fmt etx 10 ?@$ 1e6
('domain error';'assertion failure') e.~< 'lllll10.3'      fmt etx 10 ?@$ 1e6
('domain error';'assertion failure') e.~< 'm<mm>n<n>cm<p>' fmt etx 10 ?@$ 1e6


NB. 8!:n : m, n, p, & q modifiers with empty string ---------------------

(' 3.14';' 3.14') -: 'mnpq5.2' fmt 1p1;_1p1
('' fmt |x) -: 'm' fmt x=:(_1 1{~100 ?@$ 2) * 100 ?@$ 0


NB. 8!:n : s modifier with the empty string -----------------------------

(  'c'&fmt -:'cs'   &fmt) x=: _1 1.123 12345, 1e9*1p1 
(   ''&fmt -: 's'   &fmt) x
('3.1'&fmt -: 's3.1'&fmt) 12345

(  'c'&fmt -:'cs<>'   &fmt) x=: _1 1.123 12345, 1e9*1p1 
(   ''&fmt -: 's<>'   &fmt) x
('3.1'&fmt -: 's<>3.1'&fmt) 12345


NB. 8!:n : _ __ _. & other modifiers ------------------------------------

(''&fmt -:            's<-->'&fmt) _ __ _.
(''&fmt -:            's<-X>'&fmt) _ __ _.
(''&fmt -:   'cs<eE*S-?,X.D>'&fmt) _ __ _.
(''&fmt -: 'm<M>n<N>p<P>q<Q>'&fmt) _ __ _.
(''&fmt -:         'cb<ZeRo>'&fmt) _ __ _.

('_  ';'__ ';'_. ') -: 'l3.3' fmt  _ __ _.

(<@,"0 '_**') -:        '1.0' fmt _ __ _.
(<@,"0 '_XX') -: 's<-z*X>1.0' fmt _ __ _.


NB. 8!:n : exponential notation -----------------------------------------

s=:(,'0');('12' <@,"0 _ '000000000'), '3456789' <@,"0 _ 'e9' 
(x{s) -: '' fmt 1e9*x=:10 12 ?@$ 10

-. 'e' e.      , '' fmt2   1e9  * 100 ?@$ 0
   'e' e.      , '' fmt2   1e11 * 100 ?@$ 0
   'e' e.      , '' fmt2   1e_9 * 100 ?@$ 0

   '-' e.      , '' fmt2   1e_9 * 100 ?@$ 0
-. '_' e.      , '' fmt2   1e_9 * 100 ?@$ 0
   
-.  _  e. _&".&> '' fmt x=:1e11 * 100 ?@$ 0
-.  _  e. _&".&> '' fmt x=:1e_9 * 100 ?@$ 0

'M3e-11' -: , 'm<M>'      fmt2 _3e_11
 '3e-11' -: , 'm<M>'      fmt2  3e_11
'M3e_11' -: , 'm<M>s<-_>' fmt2 _3e_11
 '3e_11' -: , 'm<M>s<-_>' fmt2  3e_11
'-3e-11' -: ,          '' fmt2 _3e_11

4 -: 3!:0 y=:".x=:'2.01e9'
x -: >'' fmt y

4 -: 3!:0 y=:".x=:'2.123456789e9'
x -: >'' fmt y

NB. exponents of large magnitude
t=:(y=:(1+1000 ?@$ 9) + 1000 ?@$ 0) * 10 ^ x=:   10 + 1000 ?@$ 308 - 10
'e'&e.&> s=: '' fmt t
1e_9 > | y - ([:".i.&'e'{.])&> s
x = ([:".([:>:i.&'e')}.])&> s

t=:(y=:(1+1000 ?@$ 9) + 1000 ?@$ 0) * 10 ^ x=: - 10 + 1000 ?@$ 308 - 10
'e'&e.&> s=: '' fmt t
1e_9 > | y - ([:".i.&'e'{.])&> s
x = ([:".([:>:i.&'e')}.])&> s


NB. 8!:n : modifier text using <> () [] ---------------------------------

'<<12>>>'      -: > 'p(<<)q[>>>]' fmt 12
'**<<12.00>>>' -: > 'r<*>p(<<)q[>>>]12.2' fmt 12
'<<12.00>>>**' -: > 'r<*>p(<<)lq[>>>]12.2' fmt 12


NB. 8!:n : monad --------------------------------------------------------

((    2$a:)&fmt -: fmt) x=:   10  2 ?@$ 0
((    4$a:)&fmt -: fmt) x=:    3  4 ?@$ 0
((   20$a:)&fmt -: fmt) x=:   10 20 ?@$ 0
((    3$a:)&fmt -: fmt) x=: 1  2  3 ?@$ 0
((    5$a:)&fmt -: fmt) x=: 3  4  5 ?@$ 0
((    5$a:)&fmt -: fmt) x=: 123.4 123.45 123.456 123.4567 123.45678

((({:y)$a:)&fmt -: fmt) x=: 0 ?@$~ y=:   ?   10
((({:y)$a:)&fmt -: fmt) x=: 0 ?@$~ y=: 2 ?@$ 10
((({:y)$a:)&fmt -: fmt) x=: 0 ?@$~ y=: 3 ?@$ 10
((({:y)$a:)&fmt -: fmt) x=: 0 ?@$~ y=: 4 ?@$ 7

((({:y)$a:)&fmt -: fmt) x=: 1e_3 * 1e6 ?@$~ y=:   ?   10
((({:y)$a:)&fmt -: fmt) x=: 1e_3 * 1e6 ?@$~ y=: 2 ?@$ 10
((({:y)$a:)&fmt -: fmt) x=: 1e_3 * 1e6 ?@$~ y=: 3 ?@$ 10
((({:y)$a:)&fmt -: fmt) x=: 1e_3 * 1e6 ?@$~ y=: 4 ?@$ 7


NB. 8!:1 ----------------------------------------------------------------

   (,:&.>@,@fmt -: fmt1) y=:        ? 0
   (,:&.>@,@fmt -: fmt1) y=:        ? 1e3
   (,:&.>@,@fmt -: fmt1) y=: 1e_3 * ? 1e6
   (fmt1 -: fmt1@,)      y=: ? 0
'' (fmt1 -: fmt1@,)      y=: ? 0
   
   (,:&.>@fmt -: fmt1) y=:        (1+?20) ?@$ 0
   (,:&.>@fmt -: fmt1) y=:        (1+?20) ?@$ 1e3
   (,:&.>@fmt -: fmt1) y=: 1e_3 * (1+?20) ?@$ 1e6
'' (,:&.>@fmt -: fmt1) y=:        (1+?20) ?@$ 0
'' (,:&.>@fmt -: fmt1) y=:        (1+?20) ?@$ 1e3
'' (,:&.>@fmt -: fmt1) y=: 1e_3 * (1+?20) ?@$ 1e6

   (,:&.>@,@fmt -: fmt1) ''
'' (,:&.>@,@fmt -: fmt1) ''

   (<@:>"1@:|:@fmt -: fmt1) y=:        (1+?10 10) ?@$ 0
   (<@:>"1@:|:@fmt -: fmt1) y=:        (1+?10 10) ?@$ 1e3
   (<@:>"1@:|:@fmt -: fmt1) y=: 1e_3 * (1+?10 10) ?@$ 1e6
'' (<@:>"1@:|:@fmt -: fmt1) y=:        (1+?10 10) ?@$ 0
'' (<@:>"1@:|:@fmt -: fmt1) y=:        (1+?10 10) ?@$ 1e3
'' (<@:>"1@:|:@fmt -: fmt1) y=: 1e_3 * (1+?10 10) ?@$ 1e6

1 = #@$      fmt1 y=:(1+?10 10) ?@$ 1000
1 = #@$   '' fmt1 y=:(1+?10 10) ?@$ 1000
2 = #@$&>    fmt1 y=:(1+?10 10) ?@$ 1000
2 = #@$&> '' fmt1 y=:(1+?10 10) ?@$ 1000

NB. fmt1 on empties, chars
f=:,@:(<@,:"1)@]
''  (f -: fmt1) y=: (1 + ?10 10)$'abcdefg'
''  (f -: fmt1) y=: (  0, 1+?10)$'abcdefg'
''  (f -: fmt1) y=: ((1+?10), 0)$'abcdefg'
''  (f -: fmt1) y=:         0 0 $'abcdefg'
''  (f -: fmt1) y=: (  0, 1+?10)$0
''  (f -: fmt1) y=: ((1+?10), 0)$0
    (f -: fmt1) y=: (1 + ?10 10)$'abcdefg'
    (f -: fmt1) y=: (  0, 1+?10)$'abcdefg'
    (f -: fmt1) y=: ((1+?10), 0)$'abcdefg'
    (f -: fmt1) y=:         0 0 $'abcdefg'
    (f -: fmt1) y=: (  0, 1+?10)$0
    (f -: fmt1) y=: ((1+?10), 0)$0
    (f -: fmt1) y=: (1+?10) $ 'abcdefg'
    (f -: fmt1) y=:       0 $ 'abcdefg'
    (f -: fmt1) y=:       0 $ 0

2 = #fmt1 2 3$'abc'


NB. 8!:2 ----------------------------------------------------------------

NB. fmt2 on character and empty matrices
f=:,:@,@]
   (f-:fmt2) y=: (1 + ?10 10)$'abcdefg'
   (f-:fmt2) y=: (  0, 1+?10)$'abcdefg'
   (f-:fmt2) y=: ((1+?10), 0)$'abcdefg'
   (f-:fmt2) y=:         0 0 $'abcdefg'
   (f-:fmt2) y=: (  0, 1+?10)$0
   (f-:fmt2) y=: ((1+?10), 0)$0
'' (f-:fmt2) y=: (1 + ?10 10)$'abcdefg'
'' (f-:fmt2) y=: (  0, 1+?10)$'abcdefg'
'' (f-:fmt2) y=: ((1+?10), 0)$'abcdefg'
'' (f-:fmt2) y=:         0 0 $'abcdefg'
'' (f-:fmt2) y=: (  0, 1+?10)$0
'' (f-:fmt2) y=: ((1+?10), 0)$0

'8.5' (fmt2 -: ,./@:>@:(<@:>"1)@:|:@fmt) y=: (2+?10 10) ?@$ 0
'0.5' (fmt2 -: ,./@:>@:(<@:>"1)@:|:@fmt) y=: (2+?10 10) ?@$ 0
'0.0' (fmt2 -: ,./@:>@:(<@:>"1)@:|:@fmt) y=: (2+?10 10) ?@$ 1e6

NB. fmt2 always has a matrix result
2 = #@$ fmt2 '1'
2 = #@$ fmt2  1
2 = #@$ fmt2 1p1
2 = #@$ fmt2 y=: (1+?10) ?@$ 100
2 = #@$ fmt2 $0
2 = #@$ fmt2 ''

NB. data type doesn't matter
'' -:&fmt2 $0

NB. fmt2 on scalars
      (fmt2 -: ,:@>@fmt) y=: ? 0 
'0.5' (fmt2 -: ,:@>@fmt) y=: ? 0 
''    (fmt2 -: ,:@>@fmt) y=: ? 0 

NB. fmt2 on vectors
      (fmt2 -: ,:@;@fmt) y=:0.01 * (1+?100) ?@$ 1e6
'0.5' (fmt2 -: ,:@;@fmt) y=:0.01 * (1+?100) ?@$ 1e6
''    (fmt2 -: ,:@;@fmt) y=:0.01 * (1+?100) ?@$ 1e6

NB. character vecs
(,:-:fmt2) y=: (1+?10)$'abcdefg'
(,:-:fmt2) y=:       0$'abcdefg'

NB. scalar character
(,:@,: -: fmt2) y=: (?#a.){a.


NB. 8!:0 : empties ------------------------------------------------------

(<@,"1 -: fmt) y=: (1 + ?10 10)$'abcdefg'
(<@,"1 -: fmt) y=: (  0, 1+?10)$'abcdefg'
(<@,"1 -: fmt) y=: ((1+?10), 0)$'abcdefg'
(<@,"1 -: fmt) y=:         0 0 $'abcdefg'
(<@,"1 -: fmt) y=: (  0, 1+?10)$0
(<@,"1 -: fmt) y=: ((1+?10), 0)$0


NB. 8!:0 : atomic arguments ---------------------------------------------

(''&$@fmt@, -: fmt) 0
(''&$@fmt@, -: fmt) ?0
(''&$@fmt@, -: fmt) ?1e6


NB. 8!:n : exponential notation # of decimal places ---------------------

(<     x      ) -: '' fmt ".x=: '5e9'
(<     x      ) -: '' fmt ".x=: '5.1e9'
(<     x      ) -: '' fmt ".x=: '5.12e9'
(<     x      ) -: '' fmt ".x=: '5.123e9'
(<     x      ) -: '' fmt ".x=: '5.1234e9'
(<     x      ) -: '' fmt ".x=: '5.12345e9'
(<     x      ) -: '' fmt ".x=: '5.123456e9'
(<     x      ) -: '' fmt ".x=: '5.1234567e9'
(<     x      ) -: '' fmt ".x=: '5.12345678e9'
(<     x      ) -: '' fmt ".x=: '5.123456789e9'
(<(11{.x),'e9') -: '' fmt ".x=: '5.123456789e9'
(<(11{.x),'e9') -: '' fmt ".x=: '5.1234567891e9'
(<(11{.x),'e9') -: '' fmt ".x=: '5.12345678912e9'
(<(11{.x),'e9') -: '' fmt ".x=: '5.123456789123e9'
(<(11{.x),'e9') -: '' fmt ".x=: '5.1234567891234e9'

(<     x        ) -: '' fmt 0".x=: '5e-10'
(<     x        ) -: '' fmt 0".x=: '5.1e-10'
(<     x        ) -: '' fmt 0".x=: '5.12e-10'
(<     x        ) -: '' fmt 0".x=: '5.123e-10'
(<     x        ) -: '' fmt 0".x=: '5.1234e-10'
(<     x        ) -: '' fmt 0".x=: '5.12345e-10'
(<     x        ) -: '' fmt 0".x=: '5.123456e-10'
(<     x        ) -: '' fmt 0".x=: '5.1234567e-10'
(<     x        ) -: '' fmt 0".x=: '5.12345678e-10'
(<     x        ) -: '' fmt 0".x=: '5.123456789e-10'
(<(11{.x),'e-10') -: '' fmt 0".x=: '5.123456789e-10'
(<(11{.x),'e-10') -: '' fmt 0".x=: '5.1234567891e-10'
(<(11{.x),'e-10') -: '' fmt 0".x=: '5.12345678912e-10'
(<(11{.x),'e-10') -: '' fmt 0".x=: '5.123456789123e-10'
(<(11{.x),'e-10') -: '' fmt 0".x=: '5.1234567891234e-10'

(< '1.2e10') -: '' fmt  12e9
(<'-1.2e10') -: '' fmt _12e9


NB. 8!:n : minimum width ------------------------------------------------

   (#@":"0   -: #&>@fmt) x=:i.10 20 10
   (#@":"0   -: #&>@fmt) x=:5 4 ?@$ 100
   (#@":"0   -: #&>@fmt) x=:  4 ?@$ 100
'' (#@":"0@] -: #&>@fmt) x=:i.10 20 10
'' (#@":"0@] -: #&>@fmt) x=:5 4 ?@$ 100
'' (#@":"0@] -: #&>@fmt) x=:  4 ?@$ 100

x=:10 20 ?@$ 20000 [ y=: 1000 %~ 10 20 ?@$ 1000
x    (#@":"0@[ -: i.&'.'&>     @    fmt @:+) y
x ((-:"1{.)@:(([:#i.&'.'}.])&>)@    fmt @:+) y
x    (#@":"0@[ -: i.&'.'&>     @(''&fmt)@:+) y
x ((-:"1{.)@:(([:#i.&'.'}.])&>)@(''&fmt)@:+) y

y=: 10000 %~ 10#. 10 20 4 ?@$ 10 10 5 5
x    (#@":"0@[ -: i.&'.'&>     @    fmt @:+) y
x ((-:"1{.)@:(([:#i.&'.'}.])&>)@    fmt @:+) y
x    (#@":"0@[ -: i.&'.'&>     @(''&fmt)@:+) y
x ((-:"1{.)@:(([:#i.&'.'}.])&>)@(''&fmt)@:+) y


NB. 8!:n : computed width -----------------------------------------------

'0.0' (>./@:(#@":"0)@] -:"2 #&>@fmt) x=:i.10 20 10
'0.0' (>./@:(#@":"0)@] -:"1 #&>@fmt) x=:5 4 ?@$ 100
'0.0' (>./@:(#@":"0)@] -:"0 #&>@fmt) x=:  4 ?@$ 100

(i.10) (>./@:(#@(j.@[":])"0) -:"1 #&>@('0.'&,@":@[ fmt ]))"0 _ x=: 5 4 ?@$ 100
(i.10) (>./@:(#@(j.@[":])"0) -:"1 #&>@('0.'&,@":@[ fmt ]))"0 _ x=: 5 4 ?@$ 0


NB. 8!:n : multiple format phrases --------------------------------------

y=: (1 _1 {~ 10 ?@$ 2) * (10 ?@$ 10000) + 10 ?@$ 0
x=: ',,c,m<MINUS>,p<+>,n<\/>q</\>,0,0.0,0.4,6.1,6.5,20.5,0.8'
s=: |: ;(#<;._1 x)#<,:y
((   }. x) fmt  s) -:     (<;._1 x) fmt   s
((   }. x) fmt  s) -: ,./ (<;._1 x) fmt&> <|:,:y
((   }. x) fmt1 s) -: (}.x) <@:>"1@|:@fmt s
((<;._1 x) fmt1 s) -: (}.x) <@:>"1@|:@fmt s
(<;._1 x) -:&(fmt&s) }.x 
(<;._1 x) -:&(fmt1&s) }.x 
(<;._1 x) -:&(fmt2&s) }.x 

y=: (20 ?@$ 1000) + 20 ?@$ 0
x=: ',0,1,2,3,4,5,6,7,8,9,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9'
s=: |: ;(#<;._1 x)#<,:y
((   }. x) fmt  s) -:     (<;._1 x) fmt   s
((   }. x) fmt  s) -: ,./ (<;._1 x) fmt&> <|:,:y
((   }. x) fmt1 s) -: (}.x) <@:>"1@|:@fmt s
((<;._1 x) fmt1 s) -: (}.x) <@:>"1@|:@fmt s
(<;._1 x) -:&(fmt&s) }.x 
(<;._1 x) -:&(fmt1&s) }.x 
(<;._1 x) -:&(fmt2&s) }.x 

f=:<"_1 @: (0 1&|:)
y=: 1e_3 * 2 3 4 ?@$ 1e6
('0,1,2,3' fmt y) -: f^:_1 '0123'  fmt&.> f y
('0,1,2,3' fmt y) -:  (<"0 '0123') fmt y 

f=:<"_1 @: (0 1 2&|:)
y=: 1e_3 * 4 3 2 5?@$ 1e6
('0,1,2,3,4' fmt y) -: f^:_1 '01234'  fmt&.> f y
('0,1,2,3,4' fmt y) -:  (<"0 '01234') fmt y 


NB. 8!:n : boxed right arguments ----------------------------------------

x=: (10 ?@$ 10) <@(?@$ { a."_)"0 _] 256
x=: x,<"0 ] (10 ?@$ 0) , (10 ?@$ 1e3) , 1e_3 * 10 ?@$ 1e6

(                      fmt y) -:                fmt&> y=:x {~ 10 ?@$ #x
(                   '' fmt y) -:       (10$ a:) fmt&> y=:x {~ 10 ?@$ #x
(              (9$',') fmt y) -:       (10$<'') fmt&> y=:x {~ 10 ?@$ #x
('0,1,2,3,4,5,6,7,8,9' fmt y) -: ({.@":"0 i.10) fmt&> y=:x {~ 10 ?@$ #x

y=:2 3 4$4#1 10 100 1e3 1e4 1e5
x=:'0.1,0.2,0.3,0.4'
       (="1 {.@{.) #&> x fmt y NB. columns have uniform computed width
8 9 10 11 = {.@{.  #&> x fmt y NB. and its what we're expecting 
     2 -~/\ {.@{.  #&> x fmt y NB. decimals grow by one  => width grows by one

NB. but when we box it, it grows in both directions, since the computed width
NB. is now independent of the rest of the column
2 -~/\"1      #&> x fmt <"0 y  NB. decimals grow by one  => width grows by one
2 -~/\"1 |:,/ #&> x fmt <"0 y  NB. magnitude grow by ten => width grows by one

y=: ,/y
NB. same as above, but now y is rank-2
       (="1 {.   ) #&> x fmt y NB. columns have uniform computed width
8 9 10 11 = {.     #&> x fmt y NB. and its what we're expecting 
     2 -~/\ {.     #&> x fmt y NB. decimals grow by one  => width grows by one
2 -~/\"1      #&> x fmt <"0 y  NB. decimals grow by one  => width grows by one
2 -~/\"1 |:   #&> x fmt <"0 y  NB. magnitude grow by ten => width grows by one

NB. with fmt1, we'll rank pad on the left instead of computed-width padding
NB. on the right when we box.  so boxing in this case should be the same as
NB. left justify.  same deal for fmt2.
(('l'&,&.> <;._1 ',',x) fmt1 y) -: x fmt1 <"0 y
(('l'&,&.> <;._1 ',',x) fmt2 y) -: x fmt2 <"0 y

('domain error';'assertion failure') e.~< '' fmt  etx <<1p1
('domain error';'assertion failure') e.~< '' fmt1 etx <<101
('domain error';'assertion failure') e.~< '' fmt2 etx <<100

('' fmt <  'abcdef') -: '' fmt <  u:'abcdef'
('' fmt <, '5'     ) -: '' fmt <, u:'5'
('' fmt <0$'5'     ) -: '' fmt <0$u:'5'
('' fmt <  '5'     ) -: '' fmt <  u:'5'


4!:55 ;:'afzrnd f fmt fmt1 fmt2 s t x y'


