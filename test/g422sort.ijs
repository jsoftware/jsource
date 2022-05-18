prolog './g422sort.ijs'
NB. y/:y and y\:y -------------------------------------------------------

randuni''

test=: 3 : 0
 t=. (/:y){y
 assert. t -: y/:y
 assert. t -: /:~ y
 ips =: y
 ips =: /:~ ips  NB. test inplace
 assert. t -: ips
 if. (1=#$y)*.0=1{.0$y do. assert. (}.t)>:}:t end. 
 t=. (\:y){y
 assert. t -: y\:y
 assert. t -: \:~ y
 ips =: \:~ y
 assert. t -: ips
 if. (1=#$y)*.0=1{.0$y do. assert. (}.t)<:}:t end. 
 1
)

test1=: 3 : 0
 yy =: y
 t=: (/:"1 y){"1 y
 assert. t -: t1 =: y/:"1 y
 assert. t -: t1 =: /:~"1 y
 assert. t -: t1 =: /:"1~ y
 ips =: y
 ips =: /:"1~ ips  NB. test inplace
 assert. t -: ips [  0
 t=: (\:"1 y){"1 y
 assert. t -: t1 =: y\:"1 y
 assert. t -: t1 =: \:~"1 y
 assert. t -: t1 =: \:"1~ y
 ips =: \:"1~ ips  NB. test inplace
 assert. t -: ips [ 1
 1
)

*./ b=: test @:(]   ?@$ 2:)"0 ] 500+i.(QKTEST{2000 1000)
*./ b=: test1@:(3&, ?@$ 2:)"0 ] 500+i.(QKTEST{2000 1000)

test   a.{~ 1000 ?@$ #a.
test   a.{~ 1001 ?@$ #a.
test   a.{~ 1002 ?@$ #a.
test   a.{~ 1003 ?@$ #a.
test   a.{~ 1004 ?@$ #a.
test   a.{~ 1005 ?@$ #a.
test   a.{~ 1006 ?@$ #a.
test   a.{~ 1007 ?@$ #a.

test   a.{~ 1000 2 ?@$ #a.
test   a.{~ 1001 2 ?@$ #a.
test   a.{~ 1002 2 ?@$ #a.
test   a.{~ 1003 2 ?@$ #a.

test   a.{~ 7000 2 ?@$ #a.
test   a.{~ 7001 2 ?@$ #a.
test   a.{~ 7002 2 ?@$ #a.
test   a.{~ 7003 2 ?@$ #a.

test     u: 1000 ?@$ 65536
test     u: 1001 ?@$ 65536
test     u: 1002 ?@$ 65536
test     u: 1003 ?@$ 65536

test     u: 7000 ?@$ 65536
test     u: 7001 ?@$ 65536
test     u: 7002 ?@$ 65536
test     u: 7003 ?@$ 65536

test     adot1{~  ?1000 $# adot1
test     adot1{~  ?1001 $# adot1
test     adot1{~  ?1002 $# adot1
test     adot1{~  ?1003 $# adot1

test     adot1{~  ?7000 $# adot1
test     adot1{~  ?7001 $# adot1
test     adot1{~  ?7002 $# adot1
test     adot1{~  ?7003 $# adot1

test  10&u: RAND32 1000 ?@$ C4MAX
test  10&u: RAND32 1001 ?@$ C4MAX
test  10&u: RAND32 1002 ?@$ C4MAX
test  10&u: RAND32 1003 ?@$ C4MAX

test  10&u: RAND32 7000 ?@$ C4MAX
test  10&u: RAND32 7001 ?@$ C4MAX
test  10&u: RAND32 7002 ?@$ C4MAX
test  10&u: RAND32 7003 ?@$ C4MAX

test     adot2{~  ?1000 $# adot2
test     adot2{~  ?1001 $# adot2
test     adot2{~  ?1002 $# adot2
test     adot2{~  ?1003 $# adot2

test     adot2{~  ?7000 $# adot2
test     adot2{~  ?7001 $# adot2
test     adot2{~  ?7002 $# adot2
test     adot2{~  ?7003 $# adot2

test     sdot0{~  ?1000 $# sdot0
test     sdot0{~  ?1001 $# sdot0
test     sdot0{~  ?1002 $# sdot0
test     sdot0{~  ?1003 $# sdot0

test     sdot0{~  ?7000 $# sdot0
test     sdot0{~  ?7001 $# sdot0
test     sdot0{~  ?7002 $# sdot0
test     sdot0{~  ?7003 $# sdot0

test        1000 ?@$ 1e4
test        1001 ?@$ 1e4
test        1002 ?@$ 1e4
test        1003 ?@$ 1e4

test    -1+ 1000 ?@$ 1e4
test    -1+ 1001 ?@$ 1e4
test    -1+ 1002 ?@$ 1e4
test    -1+ 1003 ?@$ 1e4

test  _5e3+ 1000 ?@$ 1e4
test  _5e3+ 1001 ?@$ 1e4
test  _5e3+ 1002 ?@$ 1e4
test  _5e3+ 1003 ?@$ 1e4

test        1000 ?@$ 1e9
test        1001 ?@$ 1e9
test        1002 ?@$ 1e9
test        1003 ?@$ 1e9

test        1000 ?@$ IF64{1e9 1e18
test        1001 ?@$ IF64{1e9 1e18
test        1002 ?@$ IF64{1e9 1e18
test        1003 ?@$ IF64{1e9 1e18

test  - 1 + 1000 ?@$ IF64{1e9 1e18
test  - 1 + 1001 ?@$ IF64{1e9 1e18
test  - 1 + 1002 ?@$ IF64{1e9 1e18
test  - 1 + 1003 ?@$ IF64{1e9 1e18

test(--:n)+ 1000 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 1001 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 1002 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 1003 ?@$ n=: IF64{1e9 1e18

test  0.01*     1000 ?@$ IF64{1e9 1e18
test  0.01*     1001 ?@$ IF64{1e9 1e18
test  0.01*     1002 ?@$ IF64{1e9 1e18
test  0.01*     1003 ?@$ IF64{1e9 1e18

test  0.01*  -1+1000 ?@$ IF64{1e9 1e18
test  0.01*  -1+1001 ?@$ IF64{1e9 1e18
test  0.01*  -1+1002 ?@$ IF64{1e9 1e18
test  0.01*  -1+1003 ?@$ IF64{1e9 1e18

test  0.01*(--:n)+ 1000 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 1001 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 1002 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 1003 ?@$ n=: IF64{1e9 1e18


test        51000 ?@$ 1e4
test        51001 ?@$ 1e4
test        51002 ?@$ 1e4
test        51003 ?@$ 1e4

test    -1+ 51000 ?@$ 1e4
test    -1+ 51001 ?@$ 1e4
test    -1+ 51002 ?@$ 1e4
test    -1+ 51003 ?@$ 1e4

test  _5e3+ 51000 ?@$ 1e4
test  _5e3+ 51001 ?@$ 1e4
test  _5e3+ 51002 ?@$ 1e4
test  _5e3+ 51003 ?@$ 1e4

test        51000 ?@$ 1e9
test        51001 ?@$ 1e9
test        51002 ?@$ 1e9
test        51003 ?@$ 1e9

test        51000 ?@$ IF64{1e9 1e18
test        51001 ?@$ IF64{1e9 1e18
test        51002 ?@$ IF64{1e9 1e18
test        51003 ?@$ IF64{1e9 1e18

test  - 1 + 51000 ?@$ IF64{1e9 1e18
test  - 1 + 51001 ?@$ IF64{1e9 1e18
test  - 1 + 51002 ?@$ IF64{1e9 1e18
test  - 1 + 51003 ?@$ IF64{1e9 1e18

test(--:n)+ 51000 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 51001 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 51002 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 51003 ?@$ n=: IF64{1e9 1e18

test  0.01*     51000 ?@$ IF64{1e9 1e18
test  0.01*     51001 ?@$ IF64{1e9 1e18
test  0.01*     51002 ?@$ IF64{1e9 1e18
test  0.01*     51003 ?@$ IF64{1e9 1e18

test  0.01*  -1+51000 ?@$ IF64{1e9 1e18
test  0.01*  -1+51001 ?@$ IF64{1e9 1e18
test  0.01*  -1+51002 ?@$ IF64{1e9 1e18
test  0.01*  -1+51003 ?@$ IF64{1e9 1e18

test  0.01*(--:n)+ 51000 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 51001 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 51002 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 51003 ?@$ n=: IF64{1e9 1e18


test1  a.{~ 3 1000 ?@$ #a.
test1  a.{~ 3 1001 ?@$ #a.
test1  a.{~ 3 1002 ?@$ #a.
test1  a.{~ 3 1003 ?@$ #a.

test1  a.{~ 3 1000 2 ?@$ #a.
test1  a.{~ 3 1001 2 ?@$ #a.
test1  a.{~ 3 1002 2 ?@$ #a.
test1  a.{~ 3 1003 2 ?@$ #a.

test1  a.{~ 3 1000 2 ?@$ #a.
test1  a.{~ 3 1001 2 ?@$ #a.
test1  a.{~ 3 1002 2 ?@$ #a.
test1  a.{~ 3 1003 2 ?@$ #a.

test1    u: 3 1000 ?@$ 65536
test1    u: 3 1001 ?@$ 65536
test1    u: 3 1002 ?@$ 65536
test1    u: 3 1003 ?@$ 65536

test1    u: 3 1000 ?@$ 65536
test1    u: 3 1001 ?@$ 65536
test1    u: 3 1002 ?@$ 65536
test1    u: 3 1003 ?@$ 65536

test1 10&u: RAND32 3 1000 ?@$ C4MAX
test1 10&u: RAND32 3 1001 ?@$ C4MAX
test1 10&u: RAND32 3 1002 ?@$ C4MAX
test1 10&u: RAND32 3 1003 ?@$ C4MAX

test1 10&u: RAND32 3 1000 ?@$ C4MAX
test1 10&u: RAND32 3 1001 ?@$ C4MAX
test1 10&u: RAND32 3 1002 ?@$ C4MAX
test1 10&u: RAND32 3 1003 ?@$ C4MAX

test1       3 1000 ?@$ 1e4
test1       3 1001 ?@$ 1e4
test1       3 1002 ?@$ 1e4
test1       3 1003 ?@$ 1e4

test1   -1+ 3 1000 ?@$ 1e4
test1   -1+ 3 1001 ?@$ 1e4
test1   -1+ 3 1002 ?@$ 1e4
test1   -1+ 3 1003 ?@$ 1e4

test1 _5e4+ 3 1000 ?@$ 1e4
test1 _5e4+ 3 1001 ?@$ 1e4
test1 _5e4+ 3 1002 ?@$ 1e4
test1 _5e4+ 3 1003 ?@$ 1e4

test1       3 1000 ?@$ 1e9
test1       3 1001 ?@$ 1e9
test1       3 1002 ?@$ 1e9
test1       3 1003 ?@$ 1e9

test1       3 1000 ?@$ IF64{1e9 1e18
test1       3 1001 ?@$ IF64{1e9 1e18
test1       3 1002 ?@$ IF64{1e9 1e18
test1       3 1003 ?@$ IF64{1e9 1e18

test1   -1+ 3 1000 ?@$ 1e9
test1   -1+ 3 1001 ?@$ 1e9
test1   -1+ 3 1002 ?@$ 1e9
test1   -1+ 3 1003 ?@$ 1e9

test1   -1+ 3 1000 ?@$ IF64{1e9 1e18
test1   -1+ 3 1001 ?@$ IF64{1e9 1e18
test1   -1+ 3 1002 ?@$ IF64{1e9 1e18
test1   -1+ 3 1003 ?@$ IF64{1e9 1e18

test1 _5e8+ 3 1000 ?@$ 1e9
test1 _5e8+ 3 1001 ?@$ 1e9
test1 _5e8+ 3 1002 ?@$ 1e9
test1 _5e8+ 3 1003 ?@$ 1e9

test1 (--:n) + 3 1000 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 1001 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 1002 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 1003 ?@$ n=: IF64{1e9 1e18

test1 0.01* 3 1000 ?@$ IF64{1e9 1e18
test1 0.01* 3 1001 ?@$ IF64{1e9 1e18
test1 0.01* 3 1002 ?@$ IF64{1e9 1e18
test1 0.01* 3 1003 ?@$ IF64{1e9 1e18

test1 0.01*  -1+ 3 1000 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 1001 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 1002 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 1003 ?@$ IF64{1e9 1e18

test1 0.01*(--:n) + 3 1000 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 1001 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 1002 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 1003 ?@$ n=: IF64{1e9 1e18


test1       3 51000 ?@$ 1e4
test1       3 51001 ?@$ 1e4
test1       3 51002 ?@$ 1e4
test1       3 51003 ?@$ 1e4

test1   -1+ 3 51000 ?@$ 1e4
test1   -1+ 3 51001 ?@$ 1e4
test1   -1+ 3 51002 ?@$ 1e4
test1   -1+ 3 51003 ?@$ 1e4

test1 _5e4+ 3 51000 ?@$ 1e4
test1 _5e4+ 3 51001 ?@$ 1e4
test1 _5e4+ 3 51002 ?@$ 1e4
test1 _5e4+ 3 51003 ?@$ 1e4

test1       3 51000 ?@$ 1e9
test1       3 51001 ?@$ 1e9
test1       3 51002 ?@$ 1e9
test1       3 51003 ?@$ 1e9

test1       3 51000 ?@$ IF64{1e9 1e18
test1       3 51001 ?@$ IF64{1e9 1e18
test1       3 51002 ?@$ IF64{1e9 1e18
test1       3 51003 ?@$ IF64{1e9 1e18

test1   -1+ 3 51000 ?@$ 1e9
test1   -1+ 3 51001 ?@$ 1e9
test1   -1+ 3 51002 ?@$ 1e9
test1   -1+ 3 51003 ?@$ 1e9

test1   -1+ 3 51000 ?@$ IF64{1e9 1e18
test1   -1+ 3 51001 ?@$ IF64{1e9 1e18
test1   -1+ 3 51002 ?@$ IF64{1e9 1e18
test1   -1+ 3 51003 ?@$ IF64{1e9 1e18

test1 _5e8+ 3 51000 ?@$ 1e9
test1 _5e8+ 3 51001 ?@$ 1e9
test1 _5e8+ 3 51002 ?@$ 1e9
test1 _5e8+ 3 51003 ?@$ 1e9

test1 (--:n) + 3 51000 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 51001 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 51002 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 51003 ?@$ n=: IF64{1e9 1e18

test1 0.01* 3 51000 ?@$ IF64{1e9 1e18
test1 0.01* 3 51001 ?@$ IF64{1e9 1e18
test1 0.01* 3 51002 ?@$ IF64{1e9 1e18
test1 0.01* 3 51003 ?@$ IF64{1e9 1e18

test1 0.01*  -1+ 3 51000 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 51001 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 51002 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 51003 ?@$ IF64{1e9 1e18

test1 0.01*(--:n) + 3 51000 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 51001 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 51002 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 51003 ?@$ n=: IF64{1e9 1e18

NB. Test all-equal keys, just a few nonequal keys, many equal keys
test1 100 1000 $ 4
test1 <. 0.001 + 100 10000 ?@$ 0
test1 <. 20 * 100 1000 ?@$ 0

test1 1.5 + 100 1000 $ 4
test1 1.5 + <. 0.001 + 100 10000 ?@$ 0
test1 1.5 + <. 20 * 100 1000 ?@$ 0


4!:55 ;:'adot1 adot2 sdot0 b n ips t t1 test test1 yy'
randfini''


epilog''

