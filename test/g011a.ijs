prolog './g011a.ijs'
NB. B <. B ---------------------------------------------------------------

randuni''

x=: ?100$2
y=: ?100$2
(x<.y) -: (#.x,.y){0 0 0 1
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:?2
(x<.z) -: x<.($x)$z    [ z=:?2

(x<.y) -: (40$"0 x)<.y [ x=: ?10$2    [ y=: ?10 40$2
(x<.y) -: x<.40$"0 y   [ x=: ?10 40$2 [ y=: ?10$2

0 0 0 1 -: 0 0 1 1 <. 0 1 0 1


NB. B <. I ---------------------------------------------------------------

x=: ?100$2
y=: _1e5+?100$2e5
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:?2
(x<.z) -: x<.($x)$z    [ z=:_1e5+?2e5

(x<.y) -: (40$"0 x)<.y [ x=: ?10$2    [ y=: _1e5+?10 40$2e5
(x<.y) -: x<.40$"0 y   [ x=: ?10 40$2 [ y=: _1e5+?10$2e5

_4 0 1 _3 -: 0 0 1 1 <. _4 3 4 _3
_2147483648 _2147483648 -: 0 1 <. _2147483648
0 1 -: 0 1 <.  2147483647


NB. B <. D ---------------------------------------------------------------

x=: ?100$2
y=: o._1e5+?100$2e5
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:?2
(x<.z) -: x<.($x)$z    [ z=:o._1e5+?2e5

(x<.y) -: (40$"0 x)<.y [ x=: ?10$2    [ y=: o._1e5+?10 40$2e5
(x<.y) -: x<.40$"0 y   [ x=: ?10 40$2 [ y=: o._1e5+?10$2e5

_2.5 0 _2.5 1 -: 0 0 1 1 <. _2.5 1.2 _2.5 1.2
(20$0) -: (20$1) - 1<.1+10^-i.20
__ 0 __ 1 -: 0 0 1 1 <. __ _ __ _


NB. I <. B ---------------------------------------------------------------

x=: _1e5+?100$2e5
y=: ?100$2
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:_1e5+?2e5
(x<.z) -: x<.($x)$z    [ z=:?2

(x<.y) -: (40$"0 x)<.y [ x=: _1e5+?10$2e5    [ y=: ?10 40$2
(x<.y) -: x<.40$"0 y   [ x=: _1e5+?10 40$2e5 [ y=: ?10$2

_3 _3 0 1 -: _3 _3 4 4 <. 0 1 0 1
0 1 -:  2147483647 <. 0 1
_2147483648 _2147483648 -: _2147483648 <. 0 1


NB. I <. I ---------------------------------------------------------------

x=: _1e5+?100$2e5
y=: _1e5+?100$2e5
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:?2e6
(x<.z) -: x<.($x)$z    [ z=:_1e5+?2e5

(x<.y) -: (40$"0 x)<.y [ x=: _1e5+?10$2e5    [ y=: _1e5+?10 40$2e5
(x<.y) -: x<.40$"0 y   [ x=: _1e5+?10 40$2e5 [ y=: _1e5+?10$2e5

0 1 2 3 4 4 4 4 -: 4<.i.8
0 1 2 3 4 4 4 4 -: (i.8)<.4
(6$_2147483648) -: _2147483648 <. 2 0 1e9 2e9 _2147483648 2147483647
z -:  2147483647 <. z=:2 0 1e9 2e9 _2147483648 2147483647


NB. I <. D ---------------------------------------------------------------

x=: _1e5+?100$2e5
y=: o._1e5+?100$2e5
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:?2e6
(x<.z) -: x<.($x)$z    [ z=:o._1e5+?2e5

(x<.y) -: (40$"0 x)<.y [ x=: _1e5+?10$2e5    [ y=: o._1e5+?10 40$2e5
(x<.y) -: x<.40$"0 y   [ x=: _1e5+?10 40$2e5 [ y=: o._1e5+?10$2e5

0.5 1.5 2.5 3.5 4 4 4 4 -: 4<.0.5+i.8
0.5 1.5 2.5 3.5 4 4 4 4 -: (0.5+i.8)<.4
(6$_2147483648) -: _2147483648 <.2.5 0 1e9 2e9 _2147483648 2147483647
z -:  2147483647 <. z=:2.5 0 1e9 2e9 _2147483648 2147483647
(20$0) -: 17 - 17 <. 17*1+10^-i.20
__ 4 __ _5 -: 4 4 _5 _5 <. __ _ __ _
__ _2147483648 __ 2147483647 -: _2147483648 _2147483648 2147483647 2147483647 <. __ _ __ _


NB. D <. B ---------------------------------------------------------------

x=: o._1e5+?100$2e5
y=: ?100$2
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:o._1e5+?2e5
(x<.z) -: x<.($x)$z    [ z=:?2

(x<.y) -: (40$"0 x)<.y [ x=: o._1e5+?10$2e5    [ y=: ?10 40$2
(x<.y) -: x<.40$"0 y   [ x=: o._1e5+?10 40$2e5 [ y=: ?10$2

(20$0) -: z - (z=:1-10^-i.20) <. 1
(20$0) -: 0 - (z=:  10^-i.20) <. 0
(20$0) -: z - (z=: -10^-i.20) <. 0


NB. D <. I ---------------------------------------------------------------

x=: o._1e5+?100$2e5
y=: _1e5+?100$2e5
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:o._1e5+?2e5
(x<.z) -: x<.($x)$z    [ z=:_1e5+?2e5

(x<.y) -: (40$"0 x)<.y [ x=: o._1e5+?10$2e5    [ y=: _1e5+?10 40$2e5
(x<.y) -: x<.40$"0 y   [ x=: o._1e5+?10 40$2e5 [ y=: _1e5+?10$2e5

(20$0) -: z - (z=:17*1-10^-i.20) <. 20$17
(__ __,2}.z) -: __ __ _ _ <. z=:_1e9+?4$2e9


NB. D <. D ---------------------------------------------------------------

x=: o._1e5+?100$2e5
y=: o._1e5+?100$2e5
(x<.y) -: (z+x)<.z+y   [ z=:{.0 4.5
(x<.y) -: (z*x)<.z*y   [ z=:{.1 4j5
(z<.y) -: (($y)$z)<.y  [ z=:o._1e5+?2e5
(x<.z) -: x<.($x)$z    [ z=:o._1e5+?2e5

(x<.y) -: (40$"0 x)<.y [ x=: o._1e5+?10$2e5    [ y=: o._1e5+?10 40$2e5
(x<.y) -: x<.40$"0 y   [ x=: o._1e5+?10 40$2e5 [ y=: o._1e5+?10$2e5

(^1) -: (o.1) <. ^1
1x1 -: 1x1 <. 1p1

NB. SB <. SB ---------------------------------------------------------------

(s:' A b c D') -: (s:' a b c d') <. (s:' A b c D')
(s:' Aa b c Da') -: (s:' a b c d') <. (s:' Aa ba ca Da')
(s:' Aa baa cacc Daddd') -: (s:' a bb ccc dddd') <. (s:' Aa baa cacc Daddd')
(s:' Aa bb ccc Daddd') -: (s:' a bb ccc dddd') <. (s:' Aa bbb cccc Daddd')

(s:' A b c D') -:  (s:' A b c D') <. (s:' a b c d')
(s:' Aa b c Da') -:  (s:' Aa ba ca Da') <. (s:' a b c d')
(s:' Aa baa cacc Daddd') -:  (s:' Aa baa cacc Daddd') <. (s:' a bb ccc dddd')
(s:' Aa bb ccc Daddd') -:  (s:' Aa bbb cccc Daddd') <. (s:' a bb ccc dddd')

(}:sdot0) -: (}:sdot0) <. (}.sdot0)
(}:sdot0) -: (}.sdot0) <. (}:sdot0)

(((-:@#sdot0){.sdot0),((-@-:@#sdot0){.|.sdot0)) -: (|.sdot0) <. (sdot0)
(((-:@#sdot0){.sdot0),((-@-:@#sdot0){.|.sdot0)) -: (sdot0) <. (|.sdot0)

((2 4, 8%~#sdot0)$ }:sdot0) -: ((2 4, 8%~#sdot0)$ }:sdot0) <. ((2 4, 8%~#sdot0)$ }.sdot0)
((2 4, 8%~#sdot0)$ }:sdot0) -: ((2 4, 8%~#sdot0)$ }.sdot0) <. ((2 4, 8%~#sdot0)$ }:sdot0)

(({.(2 4, 8%~#sdot0)$ sdot0),:({:(2 4, 8%~#sdot0)$ |.sdot0)) -: ((2 4, 8%~#sdot0)$ |.sdot0) <. ((2 4, 8%~#sdot0)$ sdot0)
(({.(2 4, 8%~#sdot0)$ sdot0),:({:(2 4, 8%~#sdot0)$ |.sdot0)) -: ((2 4, 8%~#sdot0)$ sdot0) <. ((2 4, 8%~#sdot0)$ |.sdot0)

randfini''


epilog''

