NB. try/catch -----------------------------------------------------------

fa =: 3 : 'try. 3+y catch. ''bad'' end.'

(3+x) -: fa x=:?20$10000
(3+x) -: fa x=:j.&?~10$1000

'bad' -: fa 'abc'
'bad' -: fa <1 2 3

fb =: 3 : (':'; 'try. x+y catch. ''bad'' end.')

(3+x) -: 3 fb x=:?20$10000
(3+x) -: 3 fb x=:j.&?~10$1000

'bad' -: 1 2 fb 3 4 5
'bad' -: 3 fb 'abc'
'bad' -: 3 fb <1 2 3

fc =: 3 : 'while. try. a  catch. b end. do. c end.'   NB. checking parser
  
fd =: 3 : 0
 :
 try.
  try. 3+y catch. 7+x end.
 catch.
  'x and y are both bad'
 end.
)

(3+y) -: (x=:?100) fd (y=:?100)
(7+x) -: (x=:?100) fd 'triskaidekaphobia'
'x and y are both bad' -: 'foo' fd 'bar'

write =: 1!:2
erase =: 1!:55

fe =: 3 : 0
 try.
  erase y
  'ok'
 catch.
  'bad'
 end.
)

'ex temporanus' write <'foo.x'
'ok'  -: fe <'foo.x'
'bad' -: fe 123

fg =: 3 : 'try. 1!:55 <y catch. end. 1'
'dichlorodiphenoltrichloroethane' write <'foo.x'
fg 'foo.x'
fg 'nonexistent'
fg 3j4

fh =: 3 : 0
 try.
  if. 0<:y do. 'positive' return. else. t=.'negative' end.
 catch.
  t=.'caught'
 end.
 'it is ',t
)

'positive'       -: fh 7
'it is negative' -: fh _2
'it is caught'   -: fh 'huh?'

fi=: 3 : 0
 try.
  if. 3*y do. 1 else. 0 end.
 catch.
  'caught'
 end.
)

1 -: fi 3
0 -: fi 0
'caught' -: fi 'asdf'

fj =: 3 : 'try. goo y catch. ''caught'' end.'
goo=: 3 : 'if. 3*y do. 1 else. 0 end.'

1 -: fj 3
0 -: fj 0
'caught' -: fj 'asdf'

f=: 4 : 0
 try.
  x+y
 catchd.
  'in catchd.'
 end.
)

7 8          -: 2 3 f 5
'in catchd.' -: 2 3 f etx 4 5 6

f=: 4 : 0
 try.
  x+y
 catcht.
  'in catcht.'
 end.
)

f=: 3 : 0
 z=. $0
 for_x. y do.
  try. 
   if. 2|x do. z=.z,2*x else. *'error' end.
  catcht.
   'in catcht'
  catch.
   z=. z,_999
  end.
 end.
)

(f x) -: _999 ((0=2|x)#i.#x)} 2*x=: ?40$100

f=: 3 : 0
 t=. 1
 while. t do.
  try. break. catch. end.
  t =. 0
 end.
 t
)

1 -: f 0

NB. check that break. goes to 1 + the enclosing while./end.

x=: 1 (5!:7) <'f'  
y=: 2{"1 x
(1+y i: <'end.') = 1{>1{(y i. <'break.'){x

f=: 3 : 'try. 1+<2 catch. end.'
(i.0 0) -: f :: 2: ''

f=: 3 : 0
 try.    if. 0-:y do. 3 : 'throw.' 0 else. >:y end. 
 catch.  y=. 2#y
 catcht. y=. 3#y
 end.
)

0 0 0     -: f 0
2         -: f 1  
(2#'abc') -: f 'abc'


NB. catcht. check for errors ----------------------------------------------

t=: 0 : 0
 1 2 3
 try.
  4
  5
 end.
 7
)

'control error' -: ex '3 : t'    NB. 0 catches

t=: 0 : 0
 1 2 3
 try.
  4
 catcht.
  4 5 6
 catch.
  7
 catcht.
  8
 end.
 7
)

'control error' -: ex '3 : t'    NB. multiple catches 1

t=: 0 : 0
 1 2 3
 try.
  4
 catch.
  4 5 6
 catch.
  7
 catcht.
  8
 end.
 7
)

'control error' -: ex '3 : t'    NB. multiple catches 2

t=: 0 : 0
 1 2 3
 try.
  4
 catch.
  4 5 6
 catchd.
  7
 catchd.
  8
 end.
 7
)

'control error' -: ex '3 : t'    NB. multiple catches 3

t=: 0 : 0
 if. y do.
  catcht.
   4 5 6
  end.
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in do/end

t=: 0 : 0
 if. 
  catcht.
   4 5 6
  end.
 do.
  7
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in if/do

t=: 0 : 0
 if. y do.
  7
 else.
  catcht.
   4 5 6
  end.
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in else/end

t=: 0 : 0
 if. y do.
  7
 elseif. 2*y do.
  catcht.
   4 5 6
  end.
 elseif. 1 do.
  8
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in do/elseif

t=: 0 : 0
 for. i. y do. 
  catcht.
   4 5 6
  end.
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in fordo/end

t=: 0 : 0
 while. y do. 
  catcht.
   4 5 6
  end.
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in whiledo/end

t=: 0 : 0
 whilst. y do. 
  catcht.
   4 5 6
  end.
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in whilstdo/end

t=: 0 : 0
 try.  
  catcht.
   4 5 6
  end.
 catch.
  7
 end.
)

'control error' -: ex '3 : t'    NB. catcht nested in try/catch

t=: 0 : 0
 try.
  1 2 3  
  catcht.
   4 5 6
  end.
  7 8 9
 end.
 10 11 12
)

'control error' -: ex '3 : t'    NB. catcht nested in try


4!:55 ;:'erase f fa fb fc fd fe ff fg fh fi fj goo sub t write x y'


