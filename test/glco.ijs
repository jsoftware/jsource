NB. f L: n --------------------------------------------------------------

Level =: 2 : 0
 m=. 0{ 3&$&.|. n
 ly=. L. y  if. 0>m do. m=.0>.m+ly end.
 if. m>:ly do. u y else. u Level m&.> y end.
   :
 'l r'=. 1 2{ 3&$&.|. n
 lx=. L. x  if. 0>l do. l=.0>.l+lx end.
 ly=. L. y  if. 0>r do. r=.0>.r+ly end.
 b=. (l,r)>:lx,ly
 if.     b-: 0 0 do. x    u Level(l,r)&.> y
 elseif. b-: 0 1 do. x    u Level(l,r)&.><y
 elseif. b-: 1 0 do. (<x) u Level(l,r)&.> y
 elseif. 1       do. x u y
 end.
)

totient=: * -.@%@~.&.q:
a =: 5!:2 <'totient'

test=: 4 : '(0: L: x y) -: (0: Level x y)'

(i:5) test"0 _ a
_  test a
__ test a

test=: 4 : '(2 # L: x y) -: (2 # Level x y)'
0 1 2 test"0 _ a

((5;2) # L:  0 a) -: (5;2) # Level  0 a
((5;2) # L: _1 a) -: (5;2) # Level _1 a

f=: 4 : '<^:(1+x) ?y$1000'
a=: (?3 5$10) f"0 ?3 5$30
(+ /      L:  0 a) -: + /      Level  0 a
(>./      L:  0 a) -: >./      Level  0 a
((+/ % #) L:  0 a) -: (+/ % #) Level  0 a
(a + L: 0 a) -: a + Level 0 a

'domain error' -: ex '1 2 3 L: 4'
'domain error' -: ex '<: L: ''abc'''
'domain error' -: ex '<: L: (<123) '
'domain error' -: ex '<: L: 3.4    '
'domain error' -: ex '<: L: 3j4    '

'length error' -: ex '<: L: 1 2 3 4'
'length error' -: ex '<: L: (i.0)  '
'length error' -: ex '<: L: ''''   '

'rank error'   -: ex '<: L: (i.1 2)'


4!:55 ;:'a f Level test totient'


