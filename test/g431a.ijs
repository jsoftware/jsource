1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g431a.ijs'
NB. f/. f\  f\. models --------------------------------------------------
randuni''

en     =: #@]
em     =: (en >.@% 1&>.@|@[)`(en 0&>.@>:@- [) @. (0&<:@[)
kay    =: en`em @. (0&<@[)
omask  =: (em,en) $ ($&0@|@[ , $&1@kay)

base   =: 1&>.@-@[ * i.@em
iind   =: base ,. |@[ <. en - base
seg    =: ((+i.)/@[ { ])"1 _

infix  =: 1 : '(iind  u@seg ])"0 _'
outfix =: 1 : '(omask u@#   ])"0 _'
prefix =: 1 : '>:@,.@i.@# u@{. ]'
suffix =: 1 : ',.@i.@#    u@}. ]'

key    =: 1 : '=@[ u@# ]'

osub   =: >@]`(>@[ >@:{ ]) @. (*@#@])
oind   =: (+/&i./ </.&, i.)@(2&{.)@(,&1 1)@$
ob     =: 1 : 'oind u@osub"0 1 ,@(<"_2)'

bs     =: 1 : '(u prefix) : (u infix )'
bsd    =: 1 : '(u suffix) : (u outfix)'
sd     =: 1 : '(u ob    ) : (u key   )'


NB. x f\. y -------------------------------------------------------------
NB. Boolean
a=:1=?11 5$2
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11
k (+./\. -: +./ bsd) a  [ k=:_4+?11

NB. literal
a=:a.{~32+?11 5$95
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11

NB. literal2
a=:adot1{~32+?11 5$95
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11

NB. literal4
a=:adot2{~32+?11 5$95
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11

NB. symbol
a=:sdot0{~32+?11 5$95
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11

NB. integer
a=:?11 5$110
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11
k (+/\.  -: +/ bsd) a   [ k=:_4+?11

NB. floating point
a=: 256 %~ _4e5+ 11 5 ?@$ 1e6
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11
k (+/\.  -: +/ bsd) a   [ k=:_4+?11

NB. complex
a=:j./ 256 %~ _4e5+ 2 11 5 ?@$ 1e6
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11
k (+/\.  -: +/ bsd) a   [ k=:_4+?11

NB. boxed
t=:(1=?70$3)<;.1 ?70$110
a=:t{~?11 3$#t
k (<\.   -: < bsd) a    [ k=:_4+?11
k (<\.   -: < bsd) ,a   [ k=:_4+?11
k (]\.   -: ] bsd) a    [ k=:_4+?11

NB. Verify special code for 2 irs/\ y
THRESHOLD +. (6!:2 '2 +/\ a') < 0.4 *  (6!:2 '2 +"0/\ a') [ a =: i. 100000
THRESHOLD +. (6!:2 '2 ~:/\ a') < 0.4 *  (6!:2 '2 ~:"0/\ a') [ a =: 100000 $ 'abcdefgh'
THRESHOLD +. (6!:2 '2 */\ a') < 0.4 *  (6!:2 '2 *"0/\ a') [ a =: i. 100000


NB. x f/\. y ------------------------------------------------------------

testa=: 1 : 0
 f=: u f./
 assert. (i:1+#a) (u f./\. -: f\.)"0 _ a=: y {~ 11   ?@$ #y
 assert. (i:1+#a) (u f./\. -: f\.)"0 _ a=: y {~ 11 3 ?@$ #y
 assert. (i:1+#a) (u f./\. -: f\.)"0 _ a=: y {~ 12   ?@$ #y
 assert. (i:1+#a) (u f./\. -: f\.)"0 _ a=: y {~ 12 3 ?@$ #y
 if. y -: 0 1 do. 
  assert. (i:8) (u f./\. -: f\.)"0 1"0 _ a=: #: i.2^7
  assert. (i:9) (u f./\. -: f\.)"0 1"0 _ a=: #: i.2^8
 end.
 1
)

<. testa&> t , < sdot0{~ 100 ?@$ #sdot0
>. testa&> t , < sdot0{~ 100 ?@$ #sdot0
+  testa&> t , < j./ 2 100 ?@$ 1000
*  testa&> t , < j./ 2 100 ?@$ 1000
+. testa&> 0 1; 100 ?@$ 1000
*. testa&> 0 1; 100 ?@$ 1000
=  testa&> 0 1; 100 ?@$ 1000
~: testa&> 0 1; 100 ?@$ 1000

testb=: 3 : 0
 f=: y b./
 assert. (i:<:#a) (y b./\. -: f\.)"0 _ a=: _1e9+11   ?@$ 2e9
 assert. (i:<:#a) (y b./\. -: f\.)"0 _ a=: _1e9+11 3 ?@$ 2e9
 assert. (i:<:#a) (y b./\. -: f\.)"0 _ a=: _1e9+12   ?@$ 2e9
 assert. (i:<:#a) (y b./\. -: f\.)"0 _ a=: _1e9+12 3 ?@$ 2e9
 1
)

testb"0 ] 22 25
testb"0 ] 16 17 19 21 23 31

testb=: 3 : 0
NB. y is a virtual atom.  Check to make sure other compounds realize it.  To be effective, This require raonly to crash if it sees virtual
t1=.y " _
1
)
testb"0 ] _2 0 2
3 : 0"0 ] _2 0 2
t2 =. 5 " y
1
)
3 : 0"0 ] _2 0 2
t3 =. ":@y
1
)
3 : 0"0 ] _2 0 2
t4 =. y&+
1
)
3 : 0"0 ] _2 0 2
t5 =. +&y
1
)
3 : 0"0 ] _2 0 2
t6 =. +^:y
1
)
3 : 0"0 ] 0 2 22 25
t7 =. y b. /
1
)
3 : 0"0 ] 0 2 22 25
t7 =. +
t7 =. y b. /\.
1
)
3 : 0"0 ] _2 0 2
t8 =. 0 H. y
1
)
3 : 0"0 ] _2 0 2
t9 =. y H. 0
1
)
3 : 0"0 ] _2 0 2
t10 =. + L: y
1
)
3 : 0"0 ] _2 0 2
t11 =. + S: y
1
)
3 : 0"0 ] _2 0 2
t12 =. (y + +)
1
)
3 : 0"0 ] _2 0 2
t13 =. + :: y
1
)
3 : 0"0 ] _2 0 2
t14 =. + ;. y
1
)
3 : 0"0 ] 1e_15 0
t1=.=!.y
1
)
3 : 0"0 ] 00 01
t1=.0!:y
1
)
3 : 0"0 ] 00 01
t1=.y!:0
1
)



NB. test case of dissimilar prefix and suffix
NB. J64
a=: ". ;._2 [ 0 : 0
92 34 99
92 99 92
 0 99  0
34 92  0
99 92 34
 0 92  0
34 99 34
 0 99 92
92 92 99
34 34 92
92 34 92
34  0 34
)

(_11 (*/\.) a) -: (*/&> _11 (<\.) a)

NB. J32
a=: ". ;._2 [ 0 : 0
65  0  0
65 65  0
84 84 79
 0 60 97
 0 79 65
65  0  0
97 84 84
84 84 65
84  0 65
60 65 65
97 65 97
)

(_10 (*/\.) a) -: (*/&> _10 (<\.) a)

4!:55 ;:'a adot1 adot2 sdot0 base bs bsd em en f iind infix k kay key ob oind omask osub outfix'
4!:55 ;:'prefix sd seg suffix t testa testb'
randfini''
