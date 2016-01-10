NB. f/. f\  f\. models --------------------------------------------------
en     =: #@]
em     =: (en >.@% 1&>.@|@[)`(en 0&>.@>:@- [) @. (0&<:@[)
kay    =: en`em @. (0&<@[)
omask  =: (em,en) $ ($&0@|@[ , $&1@kay)

base   =: 1&>.@-@[ * i.@em
iind   =: base ,. |@[ <. en - base
seg    =: ((+i.)/@[ { ])"1 _

infix  =: 1 : '(iind  x@seg ])"0 _'
outfix =: 1 : '(omask x@#   ])"0 _'
prefix =: 1 : '>:@,.@i.@# x@{. ]'
suffix =: 1 : ',.@i.@#    x@}. ]'

key    =: 1 : '=@[ x@# ]'

osub   =: >@]`(>@[ >@:{ ]) @. (*@#@])
oind   =: (+/&i./ </.&, i.)@(2&{.)@(,&1 1)@$
ob     =: 1 : 'oind x@osub"0 1 ,@(<"_2)'

bs     =: 1 : '(x prefix) : (x infix )'
bsd    =: 1 : '(x suffix) : (x outfix)'
sd     =: 1 : '(x ob    ) : (x key   )'


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


NB. x f/\. y ------------------------------------------------------------

testa=: 1 : 0
 f=: u/
 assert. (i:1+#a) (u/\. -: f\.)"0 _ a=: y {~ 11   ?@$ #y
 assert. (i:1+#a) (u/\. -: f\.)"0 _ a=: y {~ 11 3 ?@$ #y
 assert. (i:1+#a) (u/\. -: f\.)"0 _ a=: y {~ 12   ?@$ #y
 assert. (i:1+#a) (u/\. -: f\.)"0 _ a=: y {~ 12 3 ?@$ #y
 if. y -: 0 1 do. 
  assert. (i:8) (u/\. -: f\.)"0 1"0 _ a=: #: i.2^7
  assert. (i:9) (u/\. -: f\.)"0 1"0 _ a=: #: i.2^8
 end.
 1
)

t=: 0 1; (100 ?@$ 1e9) ; (128 %~ 100 ?@$ 1e4); (100 ?@$ 1000x) ; (%/0 1x + 2 100 ?@$ 1000)
<. testa&> t
>. testa&> t
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


4!:55 ;:'a base bs bsd em en f iind infix k kay key ob oind omask osub outfix'
4!:55 ;:'prefix sd seg suffix t testb'

