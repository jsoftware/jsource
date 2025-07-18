prolog './g121.ijs'
NB. -.y -----------------------------------------------------------------

randuni''

not =: 1&-

NB. Boolean
(-. -: not) 1=?2 3 4$2
(-. -: not) 1=?2
(-. -: not) 1=i.2 3 0

NB. integer
(-. -: not) ?2 3 4$2123
(-. -: not) _1000+?1 2 3 4$2123
(-. -: not) ?22334
(-. -: not) i.2 3 0

NB. floating point
(-. -: not) o.?2 3 4$2123
(-. -: not) o._1000+?1 2 3 4$2123
(-. -: not) o.?22334
(-. -: not) o.i.2 3 0

NB. complex
(-. -: not) ^j.?2 3 4$2123
(-. -: not) ^j._1000+?1 2 3 4$2123
(-. -: not) ^j.?22334
(-. -: not) ^j.i.2 3 0

(-. -: not) ''
(-. -: not) u:''
(-. -: not) 10&u:''
(-. -: not) s:''

'domain error' -: -. etx 'abc'
'domain error' -: -. etx u:'abc'
'domain error' -: -. etx 10&u:'abc'
'domain error' -: -. etx s:@<"0 'abc'
'domain error' -: -. etx 3;1 2


NB. x-.y ----------------------------------------------------------------

rank =: #@$
dr   =: rank@] - 0&>.@<:@rank@[
res  =: (dr (*/@{. , }.) $@]) $ ,@]
less =: [`(([ -.@e. res) # [)@.((<: >:)&rank)

NB. Boolean
t (-. -: less) (?30   $#t){t=:1=?100 2 3$3
t (-. -: less) (?2 15 $#t){t=:1=?100 2 $2
t (-. -: less) (?4 3 2$#t){t=:1=?100 $2
(3 2$1 0) (-. -: less) 3 2$0 1
(3 2 5 4$1 0) (-. -: less) 5 4$1 0
(3 2 5 4$1 0) (-. -: less) 2 5 4$1 0
(,1) -: 1 -. 2 0 4 0
''   -: 1 -. i.2 3

NB. literal
t (-. -: less) (?30   $#t){t=:(?100 2 3$256){a.
t (-. -: less) (?2 15 $#t){t=:(?100 2  $256){a.
t (-. -: less) (?4 3 2$#t){t=:(?100    $256){a.
(3 2$'abc') (-. -: less) 3 2$'xyz'
(3 2 5 1$'abdef') (-. -: less) 5 1$'abdef'
(3 2 5 1$'abdef') (-. -: less) 2 5 1$'abdef'
(,'&') -: '&' -. 'adsfb=12as'
''     -: '&' -. 2 3$'=1&2];'

NB. literal2
t (-. -: less) (?30   $#t){t=:(?100 2 3$(#adot1)){adot1
t (-. -: less) (?2 15 $#t){t=:(?100 2  $(#adot1)){adot1
t (-. -: less) (?4 3 2$#t){t=:(?100    $(#adot1)){adot1
(3 2$'abc') (-. -: less) 3 2$'xyz'
(3 2 5 1$'abdef') (-. -: less) 5 1$'abdef'
(3 2 5 1$'abdef') (-. -: less) 2 5 1$'abdef'
(,'&') -: '&' -. u:'adsfb=12as'
''     -: '&' -. 2 3$u:'=1&2];'

NB. literal4
t (-. -: less) (?30   $#t){t=:(?100 2 3$(#adot2)){adot2
t (-. -: less) (?2 15 $#t){t=:(?100 2  $(#adot2)){adot2
t (-. -: less) (?4 3 2$#t){t=:(?100    $(#adot2)){adot2
(3 2$'abc') (-. -: less) 3 2$'xyz'
(3 2 5 1$'abdef') (-. -: less) 5 1$'abdef'
(3 2 5 1$'abdef') (-. -: less) 2 5 1$'abdef'
(,'&') -: '&' -. 10&u:'adsfb=12as'
''     -: '&' -. 2 3$10&u:'=1&2];'

NB. symbol
t (-. -: less) (?30   $#t){t=:(?100 2 3$(#sdot0)){sdot0
t (-. -: less) (?2 15 $#t){t=:(?100 2  $(#sdot0)){sdot0
t (-. -: less) (?4 3 2$#t){t=:(?100    $(#sdot0)){sdot0
(3 2$s:@<"0 'abc') (-. -: less) 3 2$s:@<"0 'xyz'
(3 2 5 1$s:@<"0 'abdef') (-. -: less) 5 1$s:@<"0 'abdef'
(3 2 5 1$s:@<"0 'abdef') (-. -: less) 2 5 1$s:@<"0 'abdef'
(,s:@<"0 '&') -: (s:@<"0 '&') -. s:@<"0 'adsfb=12as'
''     -: (s:@<"0 '&') -. 2 3$s:@<"0 '=1&2];'

NB. integer
t (-. -: less) (?30   $#t){t=:?100 2 3$1233
t (-. -: less) (?2 15 $#t){t=:_1000+?100 2 $2123
t (-. -: less) (?4 3 2$#t){t=:?100 $212312
(3 2$4 5) (-. -: less) 3 2$5 4
(i.3 2 5 1) (-. -: less) i.5 1
(i.3 2 5 1) (-. -: less) i.2 5 1
(,_17) -: _17 -. 2 0 4 0 3j4
''     -: _17 -. 2 3$99 _17 0 9.7 _12

NB. floating point
t (-. -: less) (?30   $#t){t=:o.?100 2 3$1233
t (-. -: less) (?2 15 $#t){t=:o._1000+?100 2 $2123
t (-. -: less) (?4 3 2$#t){t=:o.?100 $212312
(3 2$4.5) (-. -: less) 3 2$4.51
(o.i.3 2 4 5) (-. -: less) o.i.4 5
(o.i.3 2 4 5) (-. -: less) o.i.2 4 5
(,2.7) -: 2.7 -. 2 0 4 0 3j4
''     -: 2.7 -. 2 3$99 2.7 _17 0 3j4 7 1

NB. complex
t (-. -: less) (?30   $#t){t=:^j.?100 2 3$1233
t (-. -: less) (?2 15 $#t){t=:^j.?100 2 $2123
t (-. -: less) (?4 3 2$#t){t=:^j.?100 $212312
(3 2$4j5) (-. -: less) 3 2$5j4
(j.i.3 2 4 5) (-. -: less) j.i.4 5
(j.i.3 2 4 5) (-. -: less) j.i.2 4 5
(,2j7) -: 2j7 -. 2 0 4 0 6
''     -: 2j7 -. 2 3 4$99 2j7 _17 0 3j4 7 1

NB. boxed
t (-. -: less) (?30   $#t){t=: ":&.>?100 2 3$1233
t (-. -: less) (?2 15 $#t){t=: ":&.>_1000+?100 2 $2123
t (-. -: less) (?4 3 2$#t){t=: ":&.>?100 $212312
(":&.>3 2$4 5  ) (-. -: less) ":&.>3 2$5 4
(":&.>i.3 2 5 1) (-. -: less) ":&.>i.5 1
(":&.>i.3 2 5 1) (-. -: less) ":&.>i.2 5 1
(,<_17) -: (<_17) -. <"0 ] 2 0 4 0 3j4
''      -: (<_17) -. <"0 ] 2 3$99 _17 0 9.7 _12

NB. empties
(3 0$'') (-. -: less) i.5 0
(4 0$'') (-. -: less) i.0 0
(4 0$'') (-. -: less) i.1 0
(9 0$'') (-. -: less) i.2 3 4 0

(,1) -: $ x -. 0#x=: a:    
(,1) -: $ x -. 0#x=: 0
(,1) -: $ x -. 0#x=: 2
(,1) -: $ x -. 0#x=: 1j0
(,1) -: $ x -. 0#x=: 1x
(,1) -: $ x -. 0#x=: 1r2
(,1) -: $ x -. 0#x=: 4.
x -: x -. 0#x=:     1000  ?@$2
x -: x -. 0#x=:     1000 2?@$2
x -: x -. 0#x=:     1000 4?@$2
x -: x -. 0#x=:     1000 9?@$2
x -: x -. 0#x=: a.{~1000  ?@$#a.
x -: x -. 0#x=: a.{~1000 2?@$#a.
x -: x -. 0#x=: a.{~1000 4?@$#a.
x -: x -. 0#x=: a.{~1000 9?@$#a.
x -: x -. 0#x=: adot1{~1000  ?@$#adot1
x -: x -. 0#x=: adot1{~1000 2?@$#adot1
x -: x -. 0#x=: adot1{~1000 4?@$#adot1
x -: x -. 0#x=: adot1{~1000 9?@$#adot1
x -: x -. 0#x=: adot2{~1000  ?@$#adot2
x -: x -. 0#x=: adot2{~1000 2?@$#adot2
x -: x -. 0#x=: adot2{~1000 4?@$#adot2
x -: x -. 0#x=: adot2{~1000 9?@$#adot2
x -: x -. 0#x=: sdot0{~1000  ?@$#sdot0
x -: x -. 0#x=: sdot0{~1000 2?@$#sdot0
x -: x -. 0#x=: sdot0{~1000 4?@$#sdot0
x -: x -. 0#x=: sdot0{~1000 9?@$#sdot0
x -: x -. 0#x=: u:  1000  ?@$65536
x -: x -. 0#x=: u:  1000 3?@$65536
x -: x -. 0#x=: 10&u: RAND32 1000  ?@$C4MAX
x -: x -. 0#x=: 10&u: RAND32 1000 3?@$C4MAX
x -: x -. 0#x=:     1000  ?@$50
x -: x -. 0#x=:     1000 2?@$50
x -: x -. 0#x=:     1000  ?@$2e9
x -: x -. 0#x=:     1000 2?@$2e9
x -: x -. 0#x=: t{~ 1000  ?@$#t=: ":&.>?100$1000
x -: x -. 0#x=: t{~ 1000 2?@$#t=: ":&.>?100$1000
x -: x -. 0#x=: u:  1000  ?@$65536
x -: x -. 0#x=: u:  1000 3?@$65536

NB. non-homogeneous data

x -: x -. (5  $'a') [ x=:     1000  ?@$2
x -: x -. (5 2$'a') [ x=:     1000 2?@$2
x -: x -. (5 4$'a') [ x=:     1000 4?@$2
x -: x -. (5 9$'a') [ x=:     1000 9?@$2
x -: x -. (5  $123) [ x=: a.{~1000  ?@$#a.
x -: x -. (5 2$123) [ x=: a.{~1000 2?@$#a.
x -: x -. (5 4$123) [ x=: a.{~1000 4?@$#a.
x -: x -. (5 9$123) [ x=: a.{~1000 9?@$#a.
x -: x -. (5  $123) [ x=: adot1{~1000  ?@$#adot1
x -: x -. (5 2$123) [ x=: adot1{~1000 2?@$#adot1
x -: x -. (5 4$123) [ x=: adot1{~1000 4?@$#adot1
x -: x -. (5 9$123) [ x=: adot1{~1000 9?@$#adot1
x -: x -. (5  $123) [ x=: adot2{~1000  ?@$#adot2
x -: x -. (5 2$123) [ x=: adot2{~1000 2?@$#adot2
x -: x -. (5 4$123) [ x=: adot2{~1000 4?@$#adot2
x -: x -. (5 9$123) [ x=: adot2{~1000 9?@$#adot2
x -: x -. (5  $123) [ x=: sdot0{~1000  ?@$#sdot0
x -: x -. (5 2$123) [ x=: sdot0{~1000 2?@$#sdot0
x -: x -. (5 4$123) [ x=: sdot0{~1000 4?@$#sdot0
x -: x -. (5 9$123) [ x=: sdot0{~1000 9?@$#sdot0
x -: x -. (5 9$123) [ x=: u:  1000  ?@$65536
x -: x -. (5 3$123) [ x=: u:  1000 3?@$65536
x -: x -. (5 9$123) [ x=: 10&u: RAND32 1000  ?@$C4MAX
x -: x -. (5 3$123) [ x=: 10&u: RAND32 1000 3?@$C4MAX
x -: x -. (5  $'a') [ x=:     1000  ?@$50
x -: x -. (5 2$'a') [ x=:     1000 2?@$50
x -: x -. (5  $'a') [ x=:     1000  ?@$2e9
x -: x -. (5 2$'a') [ x=:     1000 2?@$2e9
x -: x -. (5  $'a') [ x=: t{~ 1000  ?@$#t=: ":&.>?100$1000
x -: x -. (5 2$'a') [ x=: t{~ 1000 2?@$#t=: ":&.>?100$1000

(,0) -: -.&(,2) 0
(,1) -: -.&(,2) 1
('') -: -.&(,2) 2
(,3) -: -.&(,2) 3

f=: 4 : 0
 xx=: x
 yy=: y
 assert. (x-.y) -: -.&y x
 1
)

f=:4 :('xx=: x';'yy=: y';'assert. (x-.y) -: -.&y x';'1')


s=: 1 0 1 1 0; 3 1 4 5 0; 3 1 4 5 0 0.2; 3 1 4 5 0j2; 3 4 5 5 0x; '3145'
t=: 0 ; 'abc'; 0 3 4; 0 3.4; 0 3j4; 0 3 4x; <0 3;4
s f&>/ t

NB. obsolete 'length error' -: -.&0 1 2     etx i.5 2
NB. obsolete 'length error' -: -.&0 1 2     etx i.5 4
NB. obsolete 'length error' -: -.&(i.2 5 3) etx i.5 4

NB. comparison tolerance
0 = # (1 + 1e_15 * i. 10) -. 1
9 = # (1 + 1e_15 * i. 10) (-.!.0) 1

s =: i. 10000 5  NB. removing empty is lean - viavx only
s -: s -. i. 0 5
(-.IF64) +. 4000 > 7!:2 's -. i. 0 5' 
(-.IF64) +. 4000 > 7!:2 's -.~ i. 0 5' 

NB. x ([ -. -.) y  set intersection ----------------------------------------------------------------

rank =: #@$
dr   =: rank@] - 0&>.@<:@rank@[
res  =: (dr (*/@{. , }.) $@]) $ ,@]
intersect =: (0$[)`(([ e. res) # [)@.((<: >:)&rank)

NB. Boolean
t (([ -. -.) -: intersect) (?30   $#t){t=:1=?100 2 3$3
t (([ -. -.) -: intersect) (?2 15 $#t){t=:1=?100 2 $2
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:1=?100 $2
(3 2$1 0) (([ -. -.) -: intersect) 3 2$0 1
(3 2 5 4$1 0) (([ -. -.) -: intersect) 5 4$1 0
(3 2 5 4$1 0) (([ -. -.) -: intersect) 2 5 4$1 0
'' -: 1 ([ -. -.) 2 0 4 0
(,1)   -: 1 ([ -. -.) i.2 3

NB. literal
t (([ -. -.) -: intersect) (?30   $#t){t=:(?100 2 3$256){a.
t (([ -. -.) -: intersect) (?2 15 $#t){t=:(?100 2  $256){a.
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:(?100    $256){a.
(3 2$'abc') (([ -. -.) -: intersect) 3 2$'xyz'
(3 2 5 1$'abdef') (([ -. -.) -: intersect) 5 1$'abdef'
(3 2 5 1$'abdef') (([ -. -.) -: intersect) 2 5 1$'abdef'
'' -: '&' ([ -. -.) 'adsfb=12as'
(,'&')     -: '&' ([ -. -.) 2 3$'=1&2];'

NB. literal2
t (([ -. -.) -: intersect) (?30   $#t){t=:(?100 2 3$(#adot1)){adot1
t (([ -. -.) -: intersect) (?2 15 $#t){t=:(?100 2  $(#adot1)){adot1
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:(?100    $(#adot1)){adot1
(3 2$'abc') (([ -. -.) -: intersect) 3 2$'xyz'
(3 2 5 1$'abdef') (([ -. -.) -: intersect) 5 1$'abdef'
(3 2 5 1$'abdef') (([ -. -.) -: intersect) 2 5 1$'abdef'
'' -: '&' ([ -. -.) u:'adsfb=12as'
(,'&')     -: '&' ([ -. -.) 2 3$u:'=1&2];'

NB. literal4
t (([ -. -.) -: intersect) (?30   $#t){t=:(?100 2 3$(#adot2)){adot2
t (([ -. -.) -: intersect) (?2 15 $#t){t=:(?100 2  $(#adot2)){adot2
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:(?100    $(#adot2)){adot2
(3 2$'abc') (([ -. -.) -: intersect) 3 2$'xyz'
(3 2 5 1$'abdef') (([ -. -.) -: intersect) 5 1$'abdef'
(3 2 5 1$'abdef') (([ -. -.) -: intersect) 2 5 1$'abdef'
'' -: '&' ([ -. -.) 10&u:'adsfb=12as'
(,'&')     -: '&' ([ -. -.) 2 3$10&u:'=1&2];'

NB. symbol
t (([ -. -.) -: intersect) (?30   $#t){t=:(?100 2 3$(#sdot0)){sdot0
t (([ -. -.) -: intersect) (?2 15 $#t){t=:(?100 2  $(#sdot0)){sdot0
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:(?100    $(#sdot0)){sdot0
(3 2$s:@<"0 'abc') (([ -. -.) -: intersect) 3 2$s:@<"0 'xyz'
(3 2 5 1$s:@<"0 'abdef') (([ -. -.) -: intersect) 5 1$s:@<"0 'abdef'
(3 2 5 1$s:@<"0 'abdef') (([ -. -.) -: intersect) 2 5 1$s:@<"0 'abdef'
'' -: (s:@<"0 '&') ([ -. -.) s:@<"0 'adsfb=12as'
(,s:@<"0 '&')     -: (s:@<"0 '&') ([ -. -.) 2 3$s:@<"0 '=1&2];'

NB. integer
t (([ -. -.) -: intersect) (?30   $#t){t=:?100 2 3$1233
t (([ -. -.) -: intersect) (?2 15 $#t){t=:_1000+?100 2 $2123
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:?100 $212312
(3 2$4 5) (([ -. -.) -: intersect) 3 2$5 4
(i.3 2 5 1) (([ -. -.) -: intersect) i.5 1
(i.3 2 5 1) (([ -. -.) -: intersect) i.2 5 1
'' -: _17 ([ -. -.) 2 0 4 0 3j4
(,_17)     -: _17 ([ -. -.) 2 3$99 _17 0 9.7 _12

NB. floating point
t (([ -. -.) -: intersect) (?30   $#t){t=:o.?100 2 3$1233
t (([ -. -.) -: intersect) (?2 15 $#t){t=:o._1000+?100 2 $2123
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:o.?100 $212312
(3 2$4.5) (([ -. -.) -: intersect) 3 2$4.51
(o.i.3 2 4 5) (([ -. -.) -: intersect) o.i.4 5
(o.i.3 2 4 5) (([ -. -.) -: intersect) o.i.2 4 5
'' -: 2.7 ([ -. -.) 2 0 4 0 3j4
(,2.7)     -: 2.7 ([ -. -.) 2 3$99 2.7 _17 0 3j4 7 1

NB. complex
t (([ -. -.) -: intersect) (?30   $#t){t=:^j.?100 2 3$1233
t (([ -. -.) -: intersect) (?2 15 $#t){t=:^j.?100 2 $2123
t (([ -. -.) -: intersect) (?4 3 2$#t){t=:^j.?100 $212312
(3 2$4j5) (([ -. -.) -: intersect) 3 2$5j4
(j.i.3 2 4 5) (([ -. -.) -: intersect) j.i.4 5
(j.i.3 2 4 5) (([ -. -.) -: intersect) j.i.2 4 5
'' -: 2j7 ([ -. -.) 2 0 4 0 6
(,2j7)     -: 2j7 ([ -. -.) 2 3 4$99 2j7 _17 0 3j4 7 1

NB. boxed
t (([ -. -.) -: intersect) (?30   $#t){t=: ":&.>?100 2 3$1233
t (([ -. -.) -: intersect) (?2 15 $#t){t=: ":&.>_1000+?100 2 $2123
t (([ -. -.) -: intersect) (?4 3 2$#t){t=: ":&.>?100 $212312
(":&.>3 2$4 5  ) (([ -. -.) -: intersect) ":&.>3 2$5 4
(":&.>i.3 2 5 1) (([ -. -.) -: intersect) ":&.>i.5 1
(":&.>i.3 2 5 1) (([ -. -.) -: intersect) ":&.>i.2 5 1
'' -: (<_17) ([ -. -.) <"0 ] 2 0 4 0 3j4
(,<_17)      -: (<_17) ([ -. -.) <"0 ] 2 3$99 _17 0 9.7 _12

NB. empties
(3 0$'') (([ -. -.) -: intersect) i.5 0
(4 0$'') (([ -. -.) -: intersect) i.0 0
(4 0$'') (([ -. -.) -: intersect) i.1 0
(9 0$'') (([ -. -.) -: intersect) i.2 3 4 0

(,0) -: $ x ([ -. -.) 0#x=: a:    
(,0) -: $ x ([ -. -.) 0#x=: 0
(,0) -: $ x ([ -. -.) 0#x=: 2
(,0) -: $ x ([ -. -.) 0#x=: 1j0
(,0) -: $ x ([ -. -.) 0#x=: 1x
(,0) -: $ x ([ -. -.) 0#x=: 1r2
(,0) -: $ x ([ -. -.) 0#x=: 4.
(,0) -: $ x ([ -. -.)~ 0#x=: a:    
(,0) -: $ x ([ -. -.)~ 0#x=: 0
(,0) -: $ x ([ -. -.)~ 0#x=: 2
(,0) -: $ x ([ -. -.)~ 0#x=: 1j0
(,0) -: $ x ([ -. -.)~ 0#x=: 1x
(,0) -: $ x ([ -. -.)~ 0#x=: 1r2
(,0) -: $ x ([ -. -.)~ 0#x=: 4.

(0$x) -: x ([ -. -.)~ 0#x=:     1000  ?@$2
(0$x) -: x ([ -. -.)~ 0#x=:     1000 2?@$2
(0$x) -: x ([ -. -.)~ 0#x=:     1000 4?@$2
(0$x) -: x ([ -. -.)~ 0#x=:     1000 9?@$2
(0$x) -: x ([ -. -.)~ 0#x=: a.{~1000  ?@$#a.
(0$x) -: x ([ -. -.)~ 0#x=: a.{~1000 2?@$#a.
(0$x) -: x ([ -. -.)~ 0#x=: a.{~1000 4?@$#a.
(0$x) -: x ([ -. -.)~ 0#x=: a.{~1000 9?@$#a.
(0$x) -: x ([ -. -.)~ 0#x=: adot1{~1000  ?@$#adot1
(0$x) -: x ([ -. -.)~ 0#x=: adot1{~1000 2?@$#adot1
(0$x) -: x ([ -. -.)~ 0#x=: adot1{~1000 4?@$#adot1
(0$x) -: x ([ -. -.)~ 0#x=: adot1{~1000 9?@$#adot1
(0$x) -: x ([ -. -.)~ 0#x=: adot2{~1000  ?@$#adot2
(0$x) -: x ([ -. -.)~ 0#x=: adot2{~1000 2?@$#adot2
(0$x) -: x ([ -. -.)~ 0#x=: adot2{~1000 4?@$#adot2
(0$x) -: x ([ -. -.)~ 0#x=: adot2{~1000 9?@$#adot2
(0$x) -: x ([ -. -.)~ 0#x=: sdot0{~1000  ?@$#sdot0
(0$x) -: x ([ -. -.)~ 0#x=: sdot0{~1000 2?@$#sdot0
(0$x) -: x ([ -. -.)~ 0#x=: sdot0{~1000 4?@$#sdot0
(0$x) -: x ([ -. -.)~ 0#x=: sdot0{~1000 9?@$#sdot0
(0$x) -: x ([ -. -.)~ 0#x=: u:  1000  ?@$65536
(0$x) -: x ([ -. -.)~ 0#x=: u:  1000 3?@$65536
(0$x) -: x ([ -. -.)~ 0#x=: 10&u: RAND32 1000  ?@$C4MAX
(0$x) -: x ([ -. -.)~ 0#x=: 10&u: RAND32 1000 3?@$C4MAX
(0$x) -: x ([ -. -.)~ 0#x=:     1000  ?@$50
(0$x) -: x ([ -. -.)~ 0#x=:     1000 2?@$50
(0$x) -: x ([ -. -.)~ 0#x=:     1000  ?@$2e9
(0$x) -: x ([ -. -.)~ 0#x=:     1000 2?@$2e9
(0$x) -: x ([ -. -.)~ 0#x=: t{~ 1000  ?@$#t=: ":&.>?100$1000
(0$x) -: x ([ -. -.)~ 0#x=: t{~ 1000 2?@$#t=: ":&.>?100$1000
(0$x) -: x ([ -. -.)~ 0#x=: u:  1000  ?@$65536
(0$x) -: x ([ -. -.)~ 0#x=: u:  1000 3?@$65536


NB. non-homogeneous data

(0$x) -: x ([ -. -.) (5  $'a') [ x=:     1000  ?@$2
(0$x) -: x ([ -. -.) (5 2$'a') [ x=:     1000 2?@$2
(0$x) -: x ([ -. -.) (5 4$'a') [ x=:     1000 4?@$2
(0$x) -: x ([ -. -.) (5 9$'a') [ x=:     1000 9?@$2
(0$x) -: x ([ -. -.) (5  $123) [ x=: a.{~1000  ?@$#a.
(0$x) -: x ([ -. -.) (5 2$123) [ x=: a.{~1000 2?@$#a.
(0$x) -: x ([ -. -.) (5 4$123) [ x=: a.{~1000 4?@$#a.
(0$x) -: x ([ -. -.) (5 9$123) [ x=: a.{~1000 9?@$#a.
(0$x) -: x ([ -. -.) (5  $123) [ x=: adot1{~1000  ?@$#adot1
(0$x) -: x ([ -. -.) (5 2$123) [ x=: adot1{~1000 2?@$#adot1
(0$x) -: x ([ -. -.) (5 4$123) [ x=: adot1{~1000 4?@$#adot1
(0$x) -: x ([ -. -.) (5 9$123) [ x=: adot1{~1000 9?@$#adot1
(0$x) -: x ([ -. -.) (5  $123) [ x=: adot2{~1000  ?@$#adot2
(0$x) -: x ([ -. -.) (5 2$123) [ x=: adot2{~1000 2?@$#adot2
(0$x) -: x ([ -. -.) (5 4$123) [ x=: adot2{~1000 4?@$#adot2
(0$x) -: x ([ -. -.) (5 9$123) [ x=: adot2{~1000 9?@$#adot2
(0$x) -: x ([ -. -.) (5  $123) [ x=: sdot0{~1000  ?@$#sdot0
(0$x) -: x ([ -. -.) (5 2$123) [ x=: sdot0{~1000 2?@$#sdot0
(0$x) -: x ([ -. -.) (5 4$123) [ x=: sdot0{~1000 4?@$#sdot0
(0$x) -: x ([ -. -.) (5 9$123) [ x=: sdot0{~1000 9?@$#sdot0
(0$x) -: x ([ -. -.) (5 9$123) [ x=: u:  1000  ?@$65536
(0$x) -: x ([ -. -.) (5 3$123) [ x=: u:  1000 3?@$65536
(0$x) -: x ([ -. -.) (5 9$123) [ x=: 10&u: RAND32 1000  ?@$C4MAX
(0$x) -: x ([ -. -.) (5 3$123) [ x=: 10&u: RAND32 1000 3?@$C4MAX
(0$x) -: x ([ -. -.) (5  $'a') [ x=:     1000  ?@$50
(0$x) -: x ([ -. -.) (5 2$'a') [ x=:     1000 2?@$50
(0$x) -: x ([ -. -.) (5  $'a') [ x=:     1000  ?@$2e9
(0$x) -: x ([ -. -.) (5 2$'a') [ x=:     1000 2?@$2e9
(0$x) -: x ([ -. -.) (5  $'a') [ x=: t{~ 1000  ?@$#t=: ":&.>?100$1000
(0$x) -: x ([ -. -.) (5 2$'a') [ x=: t{~ 1000 2?@$#t=: ":&.>?100$1000

(,0) -: ([ -. -.)&(,0) 0
(,1) -: ([ -. -.)&(,1) 1
('') -: ([ -. -.)&(,1) 2
(,3) -: ([ -. -.)&(,3) 3

f=: 4 : 0
 xx=: x
 yy=: y
 assert. (x([ -. -.)y) -: ([ -. -.)&y x
 1
)

s=: 1 0 1 1 0; 3 1 4 5 0; 3 1 4 5 0 0.2; 3 1 4 5 0j2; 3 4 5 5 0x; '3145'
t=: 0 ; 'abc'; 0 3 4; 0 3.4; 0 3j4; 0 3 4x; <0 3;4
s f&>/ t

NB. comparison tolerance
10 = # (1 + 1e_15 * i. 10) ([ -. -.) 1
1 = # (1 + 1e_15 * i. 10) ([ -.!.0 -.!.0) 1

NB. Verify -.!.0 in the middle is treated like -.
yy =: 1e6 $ 1. 2.
10000 > (7!:2 'yy ([ -. -.) 1') - (7!:2 'yy ([ -.!.0 -.) 1')
(-.IF64) +. (7!:2 'yy ([ -.!.1e_12 -.) 1') > 1.3 * (7!:2 'yy ([ -.!.0 -.) 1')

s =: i. 10000 5  NB. empty arg is lean
(-.IF64) +. 4000 > 7!:2 's ([ -. -.) i. 0 5' 
(-.IF64) +. 4000 > 7!:2 's ([ -. -.)~ i. 0 5' 

16 18 196 -: 196 195 16 18 24 22 28 (] -. -.~) 16 46 136 166 18 106 196 48
16 18 196 -: 196 195 16 18 24 22 28 (] -. -.~)&memu 16 46 136 166 18 106 196 48
196 16 18 -: 196 195 16 18 24 22 28 ([ -. -.) 16 46 136 166 18 106 196 48
196 16 18 -: 196 195 16 18 24 22 28 ([ -. -.)&memu 16 46 136 166 18 106 196 48
16 46 136 166 18 106 196 48 -: 196 195 16 18 24 22 28 (] [ ([ -. -.)~)&memu 16 46 136 166 18 106 196 48
16 46 136 166 18 106 196 48 -: 196 195 16 18 24 22 28 (] [ ([ -. -.)~) 16 46 136 166 18 106 196 48


randfini''


epilog''

