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
det    =: 2 : 'y/@,`({."1 x .y $:@minors)@.(1&<@{:@$)"2'
(-/det* a) -: -/ .* a=:?3 3$1000


NB. : fn call nesting ---------------------------------------------------

f=: 3 : 'if. 0<y do. f<:y else. 1 end.'

f 14
f 15
f 30
f 60


NB. 13 : ----------------------------------------------------------------

Tv  =: 1 : '13 : x'
ar  =: 5!:1
eq  =: 2 : '(ar<''x'') -: (ar<''y'')'

(+/ % #) eq ('(+/y)%#y'    Tv)
+        eq ('x+y'         Tv)
(+*-)    eq ('(x+y)*(x-y)' Tv)


NB. : -------------------------------------------------------------------

nl =: 10{a.                  NB. system independent new-line

monad =: *:@:>:
dyad  =: *:@:>:@:+

x =: <;._1 '/t=.y+1/t^2/:/t=.x+y/t=.t+1/t^2'

f0  =: 3 : x                 NB. boxed sentences
(f0 -: monad) ?30$100
(?31$100) (f0 -: dyad) ?31$100

f1  =: 3 : (>x)              NB. character matrix
(f1 -: monad) ?30$100
(?31$100) (f1 -: dyad) ?31$100

f2  =: 3 : (;x,&.><nl)       NB. string (with embedded newlines)
(f2 -: monad) ?30$100
(?31$100) (f2 -: dyad) ?31$100

f3  =: 3 : (}:;x,&.><nl)     NB. string without final newline
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

f6a =: 3 : ('(y+1)^2',nl)    NB. string monad only
(f6a -: monad) ?30$100

f6b =: 3 : (':',       nl,'(x+y+1)^2')   NB. string dyad only
(?31$100) (f6b -: dyad) o.?31$100

f6c =: 3 : (':       ',nl,'(x+y+1)^2')   NB. string dyad only
(?31$100) (f6c -: dyad) o.?31$100

f7  =: 3 : '7'               NB. scalar string
7 -: f7 ?30$100

x=: 'abc',nl,(a.{~190+i.4),nl
('p=: 0 : 0',nl,x,')',nl) 1!:2 <'asdf.txt'
0!:0 <'asdf.txt'
p -: x
1!:55 <'asdf.txt'


NB. : treatment of comments and white space -----------------------------

9!:41 ]1  NB. retain comments and whitespace

f1 =: 3 : 0

  1  NB. aa


  4  NB. bbb

)

x=: ];._1 '//  1  NB. aa///  4  NB. bbb/'
(5!:1 <'f1') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f1') -: 3;(,':');,.x
(5!:5 <'f1') -: '3 : 0',nl,nl,'  1  NB. aa',nl,nl,nl,'  4  NB. bbb',nl,nl,')'

f2 =: 3 : 0

 :

  1  NB. aa


  4  NB. bbb
)

x=: ];._1 '//://  1  NB. aa///  4  NB. bbb'
(5!:1 <'f2') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f2') -: 3;(,':');,.x
(5!:5 <'f2') -: '3 : 0',nl,nl,':',nl,nl,'  1  NB. aa',nl,nl,nl,'  4  NB. bbb',nl,')'

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
(5!:5 <'f3') -: '3 : 0',nl,nl,'  11  NB. aaaa',nl,'  12  NB. b',nl,nl,nl,'  15  NB. cc',nl,nl,nl,':',nl,nl,'  21  NB. ddd',nl,nl,nl,'  24  NB. e',nl,'      NB. ff',nl,')'

9!:41 ]0  NB. discard comments and white space

f1 =: 3 : 0

  1  NB. aa


  4  NB. bbb

)

x=: ];._1 '//1///4'
(5!:1 <'f1') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f1') -: 3;(,':');,.x
(5!:5 <'f1') -: '3 : 0',nl,nl,'1',nl,nl,nl,'4',nl,')'

f2 =: 3 : 0

 :

  1  NB. aa


  4  NB. bbb
)

x=: ];._1 '/://1///4'
(5!:1 <'f2') -: <(,':');<((,'0');3);<(,'0');,.x
(5!:2 <'f2') -: 3;(,':');,.x
(5!:5 <'f2') -: '3 : 0',nl,':',nl,nl,'1',nl,nl,nl,'4',nl,')'

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
(5!:5 <'f3') -: '3 : 0',nl,nl,'11',nl,'12',nl,nl,nl,'15',nl,':',nl,nl,'21',nl,nl,nl,'24',nl,')'

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

'syntax error' -: (3 : '+') etx 4


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


4!:55 ;:'a ar det dyad eq f f0 f1 f2 f3 f4'
4!:55 ;:'f4a f5 f5a f6 f6a f6b f6c f7'
4!:55 ;:'minors monad nl p q Tv x'


