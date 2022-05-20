prolog './g310.ijs'
NB. : -------------------------------------------------------------------

p =: ?4 10$500
q =: ?4 10$50

f =:  3 : 'y%3' : *
(q%3) -: f q
(p*q) -: p f q

f =: ^ : (3 : (':';'x+y'))
(^p)  -: f p
(p+q) -: p f q

f =: %&>: : |
(%1+q) -: f q
(p|q)  -: p f q

f =: 4 : 'x+y'

3 4 5 (f -: +) x=:?3$1e9

f=: 3 : '1 [ big=.y$''abc'''
p=: 7!:0 ''
f 2e5
q=: 7!:0 ''
2000>q-p

minors =: }."1@(1&([\.))
det    =: 2 : 'v/@,`({."1 u .v $:@minors)@.(1&<@{:@$)"2'
(-/det* a) -:!.1e_12 -/ .* a=:?3 3$1000

'domain error' -: ". etx 'undefname : 0'


NB. : fn call nesting ---------------------------------------------------

f=: 3 : 'if. 0<y do. f<:y else. 1 end.'

f 14
f 15
f 30
f 60


NB. 13 : ----------------------------------------------------------------

Tv  =: 1 : '13 : u'
ar  =: 5!:1
eq  =: 2 : '(ar<''u'') -: (ar<''v'')'

(+/ % #) eq ('(+/y)%#y'    Tv)
+        eq ('x+y'         Tv)
(+*-)    eq ('(x+y)*(x-y)' Tv)


NB. : -------------------------------------------------------------------

nln =: 10{a.                  NB. system independent new-line

monad =: *:@:>:
dyad  =: *:@:>:@:+

x =: <;._1 '/t=.y+1/t^2/:/t=.x+y/t=.t+1/t^2'

f0  =: 3 : x                 NB. boxed sentences
(f0 -: monad) ?30$100
(?31$100) (f0 -: dyad) ?31$100

f1  =: 3 : (>x)              NB. character matrix
(f1 -: monad) ?30$100
(?31$100) (f1 -: dyad) ?31$100

f2  =: 3 : (;x,&.><nln)       NB. string (with embedded newlines)
(f2 -: monad) ?30$100
(?31$100) (f2 -: dyad) ?31$100

f3  =: 3 : (}:;x,&.><nln)     NB. string without final newline
(f3 -: monad) ?30$100
(?31$100) (f3 -: dyad) ?31$100

f4  =: 3 : (2{.x)            NB. boxed monad only
(f4 -: monad) ?30$100

f4a =: 3 : (>2{.x)           NB. matrix monad only
(f4a -: monad) ?30$100

f5  =: 3 : (2}.x)            NB. boxed dyad only
(?31$100) (f5 -: dyad) ?31$100

f5a =: 3 : (>2}.x)           NB. matrix dyad only
(?31$100) (f5a -: dyad) ?31$100

f6  =: 3 : '(y+1)^2'         NB. string monad only
(f6 -: monad) ?30$100

f6a =: 3 : ('(y+1)^2',nln)    NB. string monad only
(f6a -: monad) ?30$100

f6b =: 3 : (':',       nln,'(x+y+1)^2')   NB. string dyad only
(?31$100) (f6b -: dyad) o.?31$100

f6c =: 3 : (':       ',nln,'(x+y+1)^2')   NB. string dyad only
(?31$100) (f6c -: dyad) o.?31$100

f7  =: 3 : '7'               NB. scalar string
7 -: f7 ?30$100

x=: 'abc',nln,(a.{~190+i.4),nln
('p=: 0 : 0',nln,x,')',nln) 1!:2 <'asdf.txt'
0!:0 <'asdf.txt'
p -: x
1!:55 <'asdf.txt'


NB. : treatment of comments and white space -----------------------------

ws=: 9!:40 ''
9!:41 ]1  NB. retain comments and whitespace

f1 =: 3 : 0

  1  NB. aa


  4  NB. bbb

)

x=: ];._1 '//  1  NB. aa///  4  NB. bbb/'
(5!:1 <'f1') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f1') -: 3;(,':');,.x
(5!:5 <'f1') -: '3 : 0',nln,nln,'  1  NB. aa',nln,nln,nln,'  4  NB. bbb',nln,nln,')'

f2 =: 3 : 0

 :

  1  NB. aa


  4  NB. bbb
)

x=: ];._1 '//://  1  NB. aa///  4  NB. bbb'
(5!:1 <'f2') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f2') -: 3;(,':');,.x
(5!:5 <'f2') -: '3 : 0',nln,nln,':',nln,nln,'  1  NB. aa',nln,nln,nln,'  4  NB. bbb',nln,')'

f3 =: 3 : 0

  11  NB. aaaa
  12  NB. b


  15  NB. cc


 :

  21  NB. ddd


  24  NB. e
      NB. ff
)

x=: ];._1 '//  11  NB. aaaa/  12  NB. b///  15  NB. cc///://  21  NB. ddd///  24  NB. e/      NB. ff'
(5!:1 <'f3') -: <(,':');<((,'0');3);<(,'0');x
(5!:2 <'f3') -: 3;(,':');x
(5!:5 <'f3') -: '3 : 0',nln,nln,'  11  NB. aaaa',nln,'  12  NB. b',nln,nln,nln,'  15  NB. cc',nln,nln,nln,':',nln,nln,'  21  NB. ddd',nln,nln,nln,'  24  NB. e',nln,'      NB. ff',nln,')'

9!:41 ]0  NB. discard comments and white space

f1 =: 3 : 0

  1  NB. aa


  4  NB. bbb

)

x=: ];._1 '//1///4'
(5!:1 <'f1') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f1') -: 3;(,':');,.x
(5!:5 <'f1') -: '3 : 0',nln,nln,'1',nln,nln,nln,'4',nln,')'

f2 =: 3 : 0

 :

  1  NB. aa


  4  NB. bbb
)

x=: ];._1 '/://1///4'
(5!:1 <'f2') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f2') -: 3;(,':');,.x
(5!:5 <'f2') -: '3 : 0',nln,':',nln,nln,'1',nln,nln,nln,'4',nln,')'

f3 =: 3 : 0

  11  NB. aaaa
  12  NB. b


  15  NB. cc


 :

  21  NB. ddd


  24  NB. e
      NB. ff
)

x=: ];._1 '//11/12///15/://21///24'
(5!:1 <'f3') -: <(,':');<((,'0');3);<(,'0');x
(5!:2 <'f3') -: 3;(,':');x
(5!:5 <'f3') -: '3 : 0',nln,nln,'11',nln,'12',nln,nln,nln,'15',nln,':',nln,nln,'21',nln,nln,nln,'24',nln,')'

9!:41 ]1  NB. retain comments and white space


NB. : -------------------------------------------------------------------

'domain error' -: ex '''y.'': ''x.+y.'''
'domain error' -: ex '3.5   : ''x.''   '
'domain error' -: ex '3j4   : ''x.''   '

'domain error' -: ex '''y.'' : 0  '
'domain error' -: ex '''x.'' : 1  '
'domain error' -: ex '''y.'' : 2  '
'domain error' -: ex '''y.'' : 3  '
'domain error' -: ex '''y.'' : 3.5'
'domain error' -: ex '''y.'' : 3j4'

'rank error'   -: ex '1 2   : ''x.'''
'rank error'   -: ex '(,:1) : ''x.'''
'rank error'   -: ex '3 : (2 3 4$''a'')'
'rank error'   -: ex '2 : (2 3$<''a'')'

'noun result was required' -: (3 : '+') etx 4
f =: 3 : 0
r =. y + 5
y + undefname
if. 0: y -: 6 do.
  r =. r + 1
end.
)
'|noun result was required: f|       y+undefname' -: efx 'f 4'

'|domain error|   a:    +(3 :''+'') ::0:0' -: efx 'a: + (3 : ''+'') :: 0: 0'   NB. pee inside obverse

7 -: 2 %: 2 : 'x + y' *: 5
'domain error' -: 2 (1) : 'x + y' etx 5
7 -: 2 *: (1) : 'x + y' 5
'domain error' -: *: (1) : 'x + y' etx 5
'domain error' -: 2 %: ((2) : 'y') *: etx 5
5 -: %: (2) : 'y' *: 5
5 -: *: (1) : 'y' 5

13 -: +: 2 : '(u 2) + (v 3)' *: 
NB. : empty defn --------------------------------------------------------

f=: 3 : ''

'domain error' -:   f etx 4
'domain error' -: 3 f etx 4

f=: 3 : (i.0)

'domain error' -:   f etx 4
'domain error' -: 3 f etx 4

f=: 4 : ''

'domain error' -:   f etx 4
'domain error' -: 3 f etx 4

f=: 4 : (i.0)

'domain error' -:   f etx 4
'domain error' -: 3 f etx 4

f =: (3 : '') : +

'domain error' -: f\       etx 0 1 0     
'domain error' -: f;._1    etx 0 1 0
'domain error' -: f/.      etx i.3 5    
'domain error' -: 0 1 ,&f  etx 1 0  
'domain error' -: 0 1 f@*  etx 1 0  
'domain error' -: (2: * f) etx 1 0 
'domain error' -: (+ f)    etx 5      

f1 =: 3 : 'y'
f2 =: 3 : (':'; 'x+y')
f3 =: 4 : 'x+y'

'domain error' -: 3 f1 etx 4
'domain error' -:   f2 etx 4
'domain error' -:   f3 etx 4

'domain error' -: ~./      etx 0 1 0
'domain error' -: 0 1 ~./  etx 1 0
'domain error' -: 0 1 ~.&* etx 1 0
'domain error' -: 0 1 *@~. etx 1 0
'domain error' -: (+ ~. -) etx 1 0
'domain error' -: (~. -)   etx 1 0

NB. : detection of x y etc.

a =: 1 : 0
f__u y
)
f =: -
_5 -: (<'base') a 5

a =: 1 : 0
f__y m
)
_5 -: 5 a <'base'

c =: 2 : 0
f__u y
)
f =: -
_5 -: (<'base') c [: 5

c =: 2 : 0
:
x f__u y
)
f =: -
2 -: 7 (<'base') c [: 5

NB. obsolete svxy =: 9!:48''
NB. obsolete 9!:49 (1)
NB. obsolete c =: 2 : 0
NB. obsolete f__u y.
NB. obsolete )
NB. obsolete f =: -
NB. obsolete _5 -: (<'base') c [: 5

NB. obsolete c =: 2 : 0
NB. obsolete :
NB. obsolete x. f__u 5
NB. obsolete )
NB. obsolete f =: -
NB. obsolete 2 -: 7 (<'base') c [: 5

NB. obsolete 9!:49 svxy
9!:41 ws

4!:55 ;:'a ar c det dyad eq f f0 f1 f2 f3 f4'
4!:55 ;:'f4a f5 f5a f6 f6a f6b f6c f7'
4!:55 ;:'minors monad nln p q svxy Tv ws x'



epilog''

