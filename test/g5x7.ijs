NB. 5!:7 ----------------------------------------------------------------

xr=: 5!:7

ww=:     ;:'bblock. tblock. do. if. else.'
ww=: ww, ;:'end. while. whilst. elseif. try.'  
ww=: ww, ;:' catch. break. continue. label. goto.'
ww=: ww, ;:' return. for. do. break. select.'   
ww=: ww, ;:' case. fcase. do. end. assert. throw. catchd. catcht.'              

chk=: 4 : 0
 assert. 32 = type y
 assert. 2 = #$y
 assert. x =&* {.$y
 assert. 3 = {:$y
 assert. ({."1 y) -: <"0 i.#y
 c=. 1{"1 y
 t=. 2{"1 y
 assert. 4 = type&> c
 assert. (<,3) = $&.> c
 assert. 2 = type&> t
 assert. 1 = #@$&> t
 i=. {.&>c
 assert. i e. 1+i.#ww
 p=. ((<'for_')=4{.&.>t) +. ((<'goto_')=5{.&.>t) +. (<'label_')=6{.&.>t
 assert. (i e. 1 2) ~: (((1+t i.&> '.'){.&.> t) e. 2}.ww) +. p
 assert. (1{&>c) e. 65534 65535,i.1+#c
 assert. (2{&>c) e. i.#c
 1
)

mean=: +/ % #

0 chk 1 xr <'mean'
0 chk 2 xr <'mean'

0 chk 1 xr <'chk'
1 chk 2 xr <'chk'

perm=: 3 : 0
 z=. i.1 0
 for. i.y do. z=.,/(0,.1+z){"2 1\:"1=i.>:{:$z end.
)

1 chk 1 xr <'perm'
0 chk 2 xr <'perm'

f1 =. 3 : 0
 if. y do. goto_true. else. goto_false. end.
 label_true.  'true'  return.
 label_false. 'false' return.
)

1 chk 1 xr <'f1'
0 chk 2 xr <'f1'

f2=: 3 : 0
 3 f2 y
  :
 select.
  if. y do. 1  else. 0 end.
 fcase. 0 do.
   'zero' 
 case.  1 do.
  'one'
 end.
)

1 chk 1 xr <'f2'
1 chk 2 xr <'f2'

f3=: 4 : 0
 while.
  if. y     do. 1 
  elseif. 2  do. 3 end.
 do.
  4
  try. 5 catch. 6 end.
 end.
)

0 chk 1 xr <'f3'
1 chk 2 xr <'f3'

f4=: 3 : 0
 0 
 if. y 
 do.
  for.        1    do. 2        end.
  for_xyzabc. i.12 do. >:xyzabc end.
 else.
  whilst. 3 do. 4 end.
 end.
)

1 chk 1 xr <'f4'
0 chk 2 xr <'f4'

f5=: (3 : '*:y') : (4 : 'x*y')

0 chk 1 xr <'f5'
0 chk 2 xr <'f5'

c1=: 2 : 0
 if. ?2 do.
  x&y
 else.
  x@y
 end.
)

0 chk 1 xr <'c1'
1 chk 2 xr <'c1'

a1=: 1 : 0
 if. ?2 do.
  x/\
 else.
  x/\.
 end.
)

1 chk 1 xr <'a1'
0 chk 2 xr <'a1'

c2=: 2 : 0
 'monad'
    u^:n y
 :
 'dyad'
 x u^:n y
)

1 chk 1 xr <'c2'
1 chk 2 xr <'c2'

a2=: 2 : 0
 'monad'
   u/ y
 :
 'dyad'
 x u/ y
)

1 chk 1 xr <'a2'
1 chk 2 xr <'a2'

a3=: /\
0 chk 1 xr <'a3'
0 chk 2 xr <'a3'

'domain error'  -: 2 (5!:7) etx 0 1 0
'domain error'  -: 2 (5!:7) etx 0 1 2
'domain error'  -: 2 (5!:7) etx 0 1.2
'domain error'  -: 2 (5!:7) etx 0 1j2
'domain error'  -: 2 (5!:7) etx 0 1r2
'domain error'  -: 2 (5!:7) etx 'a2'

'domain error'  -: 0    (5!:7) etx <'a2'
'domain error'  -: 3    (5!:7) etx <'a2'
'domain error'  -: (<2) (5!:7) etx <'a2'

'domain error'  -:    5!:7  etx <'foo'
'domain error'  -: 1 (5!:7) etx <'ww'

'value error'   -: 2 (5!:7) etx <'nonexistentverb'


4!:55 ;:'a1 a2 a3 c1 c2 chk f1 f2 f3 f4 f5 mean perm ww xr'


