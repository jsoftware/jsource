NB. ,&.>/"r y -----------------------------------------------------------

c=: ,

a=: ' 0123456789',(,65 97+/i.26){a.

f=: 3 : 0
 assert. (c&.>/ -: ,&.>/) y
 for_r. i.#$y do.
  assert. (c&.>/"r -: ,&.>/"r) y
 end.
 1
)

f y=: <"0 ] _5e5+11 19 13 ?@$ 1e6 
f y=: <"0 ] 11 19 13 ?@$ 0 
f y=: <"0 a{~11 19 13 ?@$ #a

NB. unit length axis

f y=: <"0 a{~11 19  1 ?@$ #a
f y=: <"0 a{~11  1 13 ?@$ #a
f y=: <"0 a{~ 1 19 13 ?@$ #a

f y=: <"0 a{~1 1 1 ?@$ #a

NB. 0-length axis

f y=: <"0 a{~11 19  0 ?@$ #a
f y=: <"0 a{~11  0 13 ?@$ #a
f y=: <"0 a{~ 0 19 13 ?@$ #a

f y=: <"0 a{~0 0 0 ?@$ #a

NB. non-boxed arguments

f y=: _5e5+11 19 13 ?@$ 1e6 
f y=: 11 19 13 ?@$ 0 
f y=: a{~11 19 13 ?@$ #a

NB. scalar arguments

f y=: <4
f y=: <4.5
f y=: <2 3$'abcd'
f y=: 4
f y=: 4.5
f y=: 'a'


4!:55 ;:'a c f r y'


