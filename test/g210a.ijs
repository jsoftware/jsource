NB. $ shape overflow ----------------------------------------------------

NB. Some tests fail because memory is obtained in 65536 word increments, 
NB. and malloc does not always fail gracefully.  
NB. Other tests fail because they try to grab the entire result array
NB. at the outset, and fail with a limit error or an out of memory.

test  =: 1 : '<@(x etx) e. (''out of memory'';''limit error'')"_'

m     =: <. 2^IF64{16 32
m2    =: <. m%2
m4    =: <. m%4


NB. 32 bit specific shape overflow tests --------------------------------

3 : 0 ''
 if. IF64 do. 1 return. end.

 assert. (32768$'a') ="0 _ test 65536$'a'
 assert. (65536$'a') ="0 _ test 65536$'a'

 assert. > test (i.65536);65536$<1
 assert. > test (i.65536);65535$<1
 assert. > test (i.65536);32768$<1
 assert. > test (i.65536);32767$<1
 assert. > test (i.65536);16384$<1
 assert. > test (i.65536);16383$<1
 assert. > test (i.65536); 8192$<1
 assert. > test (i.65536); 8191$<1
 assert. > test (i.32768);65536$<1
 assert. > test (i.32768);65535$<1
 assert. > test (i.32768);32768$<1
 assert. > test (i.32768);32767$<1
 assert. > test (i.32768);16384$<1
 assert. > test (i.32768);16383$<1

 assert. (i.16384) >."0 _ test i.16384
 assert. (i.16384) >."0 _ test i.32768
 assert. (i.16384) >."0 _ test i.32 1024
 assert. (i.32768) >."0 _ test i.32768
 assert. (i.32768) >."0 _ test i.65536
 assert. (i.65536) >."0 _ test i.65536
 assert. (i.32767) >."0 _ test i.65536
 assert. (i.32767) >."0 _ test i.16$2

 f=: 4 : 'x>.y'
 assert. (i.32768) f "0 _ test i.65536
 assert. (i.65536) f "0 _ test i.65536

 assert. 32768 65536 $ test 'a'
 assert.  8192 65536 $ test 4
 assert.  4096 65536 $ test 4.8
 assert.  2048 65536 $ test 4j8
 assert. 65536 32768 $ test 'a'
 assert. 16384 32768 $ test 4
 assert.  8192 32768 $ test 4.8
 assert.  4096 32768 $ test 4j8

 assert.  (i.65536 1) +/ .* test i.1 65536
 assert.  (i.65536 1) +/ .* test i.1 65535
 assert.  (i.65536 1) +/ .* test i.1 32768
 assert.  (i.65536 1) +/ .* test i.1 32767
 assert.  (i.65536 1) +/ .* test i.1 16384
 assert.  (i.65536 1) +/ .* test i.1 16383
 assert.  (i.65536 1) +/ .* test i.1  8192

 assert.  (65536$'a') ,"0 1 test 65536$'b'
 assert.  (65536$'a') ,"0 1 test 32768$'b'

 assert. [;.1 test (32767$'x'),65536{.'x'
 assert. [;.1 test (32768$'x'),65536{.'x'
 assert. [;.1 test (65535$'x'),65536{.'x'
 assert. [;.1 test (65536$'x'),65536{.'x'

 assert.  65536 # test 65536$'a'
 assert.  65536 # test 65535$'a'
 assert.  65536 # test 32768$'a'
 assert.  65536 # test 32767$'a'
 assert.  65536 123# test 2 65536$'a'
 assert.  65536 123# test 2 65535$'a'
 assert.  65536 123# test 2 32768$'a'
 assert.  65536 123# test 2 32767$'a'

 assert.  (i.65536) ["1 test 65536 1$'a'
 assert.  (i.65536) ["1 test 32768 1$'a'
 assert.  (i.65536) ["1 test 16384 1$'a'
 assert.  (i.65536) ["1 test  8192 1$'a'

 assert.  (<(65536 ?@$ 2); 65536 ?@$ 3){ test 2 3$'abcdef'
 assert.  (<(32768 ?@$ 2); 65536 ?@$ 3){ test 2 3$'abcdef'
 assert.  (<(16384 ?@$ 2); 65536 ?@$ 3){ test 2 3$'abcdef'
 assert.  (65650 ?@$ 2){ test 2 32767$'abc'
 assert.  (65650 ?@$ 2){ test 2 32768$'abc'
 assert.  (65650 ?@$ 2){ test 2 32769$'abc'
 assert.  (65650 ?@$ 2){ test 2 65535$'abc'
 assert.  (65650 ?@$ 2){ test 2 65536$'abc'
 assert.  (65650 ?@$ 2){ test 2 65537$'abc'
 assert.  (65536 ?@$ 2){ test 2 65536$'abc'
 assert.  (32768 ?@$ 2){ test 2 65536$'abc'

 assert.  65536 65536 {. test 65536 4$'a'
 assert.  65535 65536 {. test 65535 4$'a'
 assert.  32768 65536 {. test 32768 4$'a'
 assert.  65536 {."1 test 65536 2$'a' 
 assert.  65536 {."1 test 65535 2$'a' 
 assert.  65536 {."1 test 32768 2$'a' 

 assert. (16384 2$'a') i."1 _ test 16384 2$'a'
 assert. (16384 2$'a') i."1 _ test 32768 2$'a'
 assert. (32768 2$'a') i."1 _ test 32768 2$'a'
 assert. (32768 2$'a') i."1 _ test 65536 2$'a'
 assert. (65536 2$'a') i."1 _ test 65536 2$'a'
 assert. (32767 2$'a') i."1 _ test 65536 2$'a'

 assert. 'limit error' -: ;  etx ;~i.(2^30),0
 assert. 'limit error' -: ,~ etx   i.(2^30),0
 1
)


NB. general shape overflow tests ----------------------------------------

<"2 test i.m, m    ,0 2
<"2 test i.m,(m -1),0 2
<"2 test i.m, m2   ,0 2
<"2 test i.m,(m2-1),0 2
<"2 test i.m, m4   ,0 2
<"2 test i.m,(m4-1),0 2
<"2 test i.m,(m%8) ,0 2

(0 $~ m,m ,0) +/ .* test 0 1$0
(0 $~ m,m2,0) +/ .* test 0 1$0
(1 0$0) +/ .* test 0 $~ 0,m,m
(1 0$0) +/ .* test 0 $~ 0,m,m2

('a' $~ m,m ,0) ,"1 test 'b'
('a' $~ m,m2,0) ,"1 test 'b'

#"2 test ((16 $~ IF64{ 8 16),0 2)$'a'
#"2 test (( 2 $~ IF64{31 63),0 2)$'a'

/:"2 test i.m,32768 0
/:"2 test i.m,32767 0
/:"2 test i.m,16384 0
/:"2 test i.m,16383 0
/:"2 test i.m, 8192 0
/:"2 test i.256 256, m2   ,0
/:"2 test i.256 256,(m2-1),0
/:"2 test i.256 256, m4   ,0
/:"2 test i.256 256,(m%8) ,0

(-: ]"2) i.m ,m ,0 2
(-: ]"2) i.m ,m2,0 2
(-: ]"2) i.m ,m4,0 2
(-: ]"2) i.m2,m ,0 2
(-: ]"2) i.m4,m ,0 2

(i.m,0) ]"1 test 65536$'a'
(i.m,0) ]"1 test 32768$'a'

(    -2^IF64{31 63) {. test i.    4 0
(    -2^IF64{31 63) {. test i.  3 4 0
(2 3,-2^IF64{31 63) {. test i.2 3 4 0

i.@2:"1 test i. (2^IF64{30 62),0

f=: 3 : '<y'
f"2 test i.m, m    ,0 2
f"2 test i.m,(m -1),0 2
f"2 test i.m, m2   ,0 2
f"2 test i.m,(m2-1),0 2
f"2 test i.m, m4   ,0 2
f"2 test i.m,(m4-1),0 2
f"2 test i.m,(m%8) ,0 2

f=: 4 : 'x>.y'
17 f"0 1 test i.m,m ,0
17 f"0 1 test i.m,m2,0
(i.m,m ,0) f"1 0 test 17
(i.m,m2,0) f"1 0 test 17

": test <i. m ,m ,0
": test <i. m ,m2,0
": test <i. m ,m4,0
": test <i. m2,m ,0
": test <i.(3$<.2^IF64{12 22),1 0

": test (i.0,m-3);i.(m -2),0
": test (i.0,m-3);i.(m2-2),0

": test <(0,imax)$0
": test ((0,<.imax%2)$0);(0,>.imax%2)$0
": test (i. 1 0);i. 0,<.imax%3
": test (i. 2 0);i. 0,<.imax%4
": test (i. 3 0);i. 0,<.imax%5


4!:55 ;:'f m m2 m4 test'


