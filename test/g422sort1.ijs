NB. y/:y and y\:y -------------------------------------------------------
NB. 65536<#elements 

randuni''

test=: 3 : 0
 t=. (/:y){y
 assert. t -: y/:y
 assert. t -: /:~ y
 if. (1=#$y)*.0=1{.0$y do. assert. (}.t)>:}:t end. 
 t=. (\:y){y
 assert. t -: y\:y
 assert. t -: \:~ y
 if. (1=#$y)*.0=1{.0$y do. assert. (}.t)<:}:t end. 
 1
)

test1=: 3 : 0
 t=. (/:"1 y){"1 y
 assert. t -: y/:"1 y
 assert. t -: /:~"1 y
 assert. t -: /:"1~ y
 t=. (\:"1 y){"1 y
 assert. t -: y\:"1 y
 assert. t -: \:~"1 y
 assert. t -: \:"1~ y
 1
)

*./ b=: test @:(]   ?@$ 2:)"0 ] 500+i.10000
*./ b=: test1@:(3&, ?@$ 2:)"0 ] 500+i.10000

test   a.{~ 100000 ?@$ #a.
test   a.{~ 100100 ?@$ #a.
test   a.{~ 100200 ?@$ #a.
test   a.{~ 100300 ?@$ #a.
test   a.{~ 100400 ?@$ #a.
test   a.{~ 100500 ?@$ #a.
test   a.{~ 100600 ?@$ #a.
test   a.{~ 100700 ?@$ #a.

test   a.{~ 100000 2 ?@$ #a.
test   a.{~ 100100 2 ?@$ #a.
test   a.{~ 100200 2 ?@$ #a.
test   a.{~ 100300 2 ?@$ #a.

test   a.{~ 700000 2 ?@$ #a.
test   a.{~ 700100 2 ?@$ #a.
test   a.{~ 700200 2 ?@$ #a.
test   a.{~ 700300 2 ?@$ #a.

test     u: 100000 ?@$ 65536
test     u: 100100 ?@$ 65536
test     u: 100200 ?@$ 65536
test     u: 100300 ?@$ 65536

test     u: 700000 ?@$ 65536
test     u: 700100 ?@$ 65536
test     u: 700200 ?@$ 65536
test     u: 700300 ?@$ 65536

test     adot1{~  ?100000 $# adot1
test     adot1{~  ?100100 $# adot1
test     adot1{~  ?100200 $# adot1
test     adot1{~  ?100300 $# adot1

test     adot1{~  ?700000 $# adot1
test     adot1{~  ?700100 $# adot1
test     adot1{~  ?700200 $# adot1
test     adot1{~  ?700300 $# adot1

test  10&u: RAND32 100000 ?@$ C4MAX
test  10&u: RAND32 100100 ?@$ C4MAX
test  10&u: RAND32 100200 ?@$ C4MAX
test  10&u: RAND32 100300 ?@$ C4MAX

test  10&u: RAND32 700000 ?@$ C4MAX
test  10&u: RAND32 700100 ?@$ C4MAX
test  10&u: RAND32 700200 ?@$ C4MAX
test  10&u: RAND32 700300 ?@$ C4MAX

test     adot2{~  ?100000 $# adot2
test     adot2{~  ?100100 $# adot2
test     adot2{~  ?100200 $# adot2
test     adot2{~  ?100300 $# adot2

test     adot2{~  ?700000 $# adot2
test     adot2{~  ?700100 $# adot2
test     adot2{~  ?700200 $# adot2
test     adot2{~  ?700300 $# adot2

test     sdot{~  ?100000 $# sdot
test     sdot{~  ?100100 $# sdot
test     sdot{~  ?100200 $# sdot
test     sdot{~  ?100300 $# sdot

test     sdot{~  ?700000 $# sdot
test     sdot{~  ?700100 $# sdot
test     sdot{~  ?700200 $# sdot
test     sdot{~  ?700300 $# sdot

test        100000 ?@$ 1e4
test        100100 ?@$ 1e4
test        100200 ?@$ 1e4
test        100300 ?@$ 1e4

test    -1+ 100000 ?@$ 1e4
test    -1+ 100100 ?@$ 1e4
test    -1+ 100200 ?@$ 1e4
test    -1+ 100300 ?@$ 1e4

test  _5e3+ 100000 ?@$ 1e4
test  _5e3+ 100100 ?@$ 1e4
test  _5e3+ 100200 ?@$ 1e4
test  _5e3+ 100300 ?@$ 1e4

test        100000 ?@$ 1e9
test        100100 ?@$ 1e9
test        100200 ?@$ 1e9
test        100300 ?@$ 1e9

test        100000 ?@$ IF64{1e9 1e18
test        100100 ?@$ IF64{1e9 1e18
test        100200 ?@$ IF64{1e9 1e18
test        100300 ?@$ IF64{1e9 1e18

test  - 1 + 100000 ?@$ IF64{1e9 1e18
test  - 1 + 100100 ?@$ IF64{1e9 1e18
test  - 1 + 100200 ?@$ IF64{1e9 1e18
test  - 1 + 100300 ?@$ IF64{1e9 1e18

test(--:n)+ 100000 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 100100 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 100200 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 100300 ?@$ n=: IF64{1e9 1e18

test  0.01*     100000 ?@$ IF64{1e9 1e18
test  0.01*     100100 ?@$ IF64{1e9 1e18
test  0.01*     100200 ?@$ IF64{1e9 1e18
test  0.01*     1003 ?@$ IF64{1e9 1e18

test  0.01*  -1+100000 ?@$ IF64{1e9 1e18
test  0.01*  -1+100100 ?@$ IF64{1e9 1e18
test  0.01*  -1+100200 ?@$ IF64{1e9 1e18
test  0.01*  -1+100300 ?@$ IF64{1e9 1e18

test  0.01*(--:n)+ 100000 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 100100 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 100200 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 100300 ?@$ n=: IF64{1e9 1e18


test1  a.{~ 3 100000 ?@$ #a.
test1  a.{~ 3 100100 ?@$ #a.
test1  a.{~ 3 100200 ?@$ #a.
test1  a.{~ 3 100300 ?@$ #a.

test1  a.{~ 3 100000 2 ?@$ #a.
test1  a.{~ 3 100100 2 ?@$ #a.
test1  a.{~ 3 100200 2 ?@$ #a.
test1  a.{~ 3 100300 2 ?@$ #a.

test1  a.{~ 3 100000 2 ?@$ #a.
test1  a.{~ 3 100100 2 ?@$ #a.
test1  a.{~ 3 100200 2 ?@$ #a.
test1  a.{~ 3 100300 2 ?@$ #a.

test1    u: 3 100000 ?@$ 65536
test1    u: 3 100100 ?@$ 65536
test1    u: 3 100200 ?@$ 65536
test1    u: 3 100300 ?@$ 65536

test1    u: 3 100000 ?@$ 65536
test1    u: 3 100100 ?@$ 65536
test1    u: 3 100200 ?@$ 65536
test1    u: 3 100300 ?@$ 65536

test1 10&u: RAND32 3 100000 ?@$ C4MAX
test1 10&u: RAND32 3 100100 ?@$ C4MAX
test1 10&u: RAND32 3 100200 ?@$ C4MAX
test1 10&u: RAND32 3 100300 ?@$ C4MAX

test1 10&u: RAND32 3 100000 ?@$ C4MAX
test1 10&u: RAND32 3 100100 ?@$ C4MAX
test1 10&u: RAND32 3 100200 ?@$ C4MAX
test1 10&u: RAND32 3 100300 ?@$ C4MAX

test1       3 100000 ?@$ 1e4
test1       3 100100 ?@$ 1e4
test1       3 100200 ?@$ 1e4
test1       3 100300 ?@$ 1e4

test1   -1+ 3 100000 ?@$ 1e4
test1   -1+ 3 100100 ?@$ 1e4
test1   -1+ 3 100200 ?@$ 1e4
test1   -1+ 3 100300 ?@$ 1e4

test1 _5e4+ 3 100000 ?@$ 1e4
test1 _5e4+ 3 100100 ?@$ 1e4
test1 _5e4+ 3 100200 ?@$ 1e4
test1 _5e4+ 3 100300 ?@$ 1e4

test1       3 100000 ?@$ 1e9
test1       3 100100 ?@$ 1e9
test1       3 100200 ?@$ 1e9
test1       3 100300 ?@$ 1e9

test1       3 100000 ?@$ IF64{1e9 1e18
test1       3 100100 ?@$ IF64{1e9 1e18
test1       3 100200 ?@$ IF64{1e9 1e18
test1       3 100300 ?@$ IF64{1e9 1e18

test1   -1+ 3 100000 ?@$ 1e9
test1   -1+ 3 100100 ?@$ 1e9
test1   -1+ 3 100200 ?@$ 1e9
test1   -1+ 3 100300 ?@$ 1e9

test1   -1+ 3 100000 ?@$ IF64{1e9 1e18
test1   -1+ 3 100100 ?@$ IF64{1e9 1e18
test1   -1+ 3 100200 ?@$ IF64{1e9 1e18
test1   -1+ 3 100300 ?@$ IF64{1e9 1e18

test1 _5e8+ 3 100000 ?@$ 1e9
test1 _5e8+ 3 100100 ?@$ 1e9
test1 _5e8+ 3 100200 ?@$ 1e9
test1 _5e8+ 3 100300 ?@$ 1e9

test1 (IF64{_5e8 _5e17)+ 3 100000 ?@$ IF64{1e9 1e18
test1 (IF64{_5e8 _5e17)+ 3 100100 ?@$ IF64{1e9 1e18
test1 (IF64{_5e8 _5e17)+ 3 100200 ?@$ IF64{1e9 1e18
test1 (IF64{_5e8 _5e17)+ 3 100300 ?@$ IF64{1e9 1e18

test1 (--:n) + 3 100000 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 100100 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 100200 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 100300 ?@$ n=: IF64{1e9 1e18

test1 0.01* 3 100000 ?@$ IF64{1e9 1e18
test1 0.01* 3 100100 ?@$ IF64{1e9 1e18
test1 0.01* 3 100200 ?@$ IF64{1e9 1e18
test1 0.01* 3 100300 ?@$ IF64{1e9 1e18

test1 0.01*  -1+ 3 100000 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 100100 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 100200 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 100300 ?@$ IF64{1e9 1e18

test1 0.01*(--:n) + 3 100000 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 100100 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 100200 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 100300 ?@$ n=: IF64{1e9 1e18


4!:55 ;:'adot1 adot2 sdot b n test test1'


