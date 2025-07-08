prolog './g000.ijs'

NB. Early in the testing, verify that local assignments relocate the symbol table correctly
3 : 0 ''
st =. 0
st =. 15!:_1 <'st'
for_l1. 'abcdefghijklmnopqrstuvwxyz' do.
  for_l2. 'abcdefghijklmnopqrstuvwxyz' do.
    (l1,l2) =. 26 #: l1_index,l2_index
  end.
end.
for_l1. 'abcdefghijklmnopqrstuvwxyz' do.
  for_l2. 'abcdefghijklmnopqrstuvwxyz' do.
    assert (l1,l2)~ = 26 #: l1_index,l2_index
  end.
end.
est =. 15!:_1 <'st'
NB. This relocates when run early  if. st ~: est do. smoutput 'symbols relocated' end.
1
)

NB. =y ------------------------------------------------------------------

randuni''

(,.1) -: =1
(,.1) -: ='a'
(,.1) -: =3
(,.1) -: =4.5
(,.1) -: =3j4
(,.1) -: =<'abc'

(i.0 0) -: =0 4 5$1
(i.0 0) -: =''
(i.0 0) -: =i.0
(i.0 0) -: =o.i.0
(i.0 0) -: =0 4 5 6 7 0$<''

(1 5$1) -: =5$1
(1 5$1) -: =5$'a'
(1 5$1) -: =5$12345
(1 5$1) -: =5$3.14159
(1 5$1) -: =5$3j4
(1 5$1) -: =5$<'abc'

((n,n)$1,n$0)-:=i.n=:?25

test =: 3 : '(1=type b) , (($b)-:(#~.y),#y) , (+/,b)=#y [ b=.=y'

test x=:?2
test x=:a.{~?2$3
test x=:?4023
test x=:o.?4023
test x=:j./?2$3
test x=:(;:'foo upon thee'){~?3

test x=:?40 3$2
test x=:a.{~50+?40 2$3
test x=:?40 2$3
test x=:o.?40 2$3
test x=:j./?2 82 2$3
test x=:(;:'foo upon thee'){~?40 2$3


NB. x=y -----------------------------------------------------------------

NB. Boolean
1 0 0 1 -: 0 0 1 1=0 1 0 1

NB. literal
(($t)$1) -: t = t=:a.{~?2 3 4$#a.
(($t)$0) -: (?($t)$2)          = t=:a.{~?2 3 4$#a.
(($t)$0) -: (_5e8+?($t)$1e9)   = t=:a.{~?2 3 4$#a.
(($t)$0) -: (o._5e8+?($t)$1e9) = t=:a.{~?2 3 4$#a.
(($t)$0) -: (r._5e6+?($t)$1e8) = t=:a.{~?2 3 4$#a.
(($t)$0) -: (($t)$;:'8-+.abc') = t=:a.{~?2 3 4$#a.

NB. literal2
(($t)$1) -: t = t=:adot1{~?2 3 4$#adot1
(($t)$0) -: (?($t)$2)          = t=:adot1{~?2 3 4$#adot1
(($t)$0) -: (_5e8+?($t)$1e9)   = t=:adot1{~?2 3 4$#adot1
(($t)$0) -: (o._5e8+?($t)$1e9) = t=:adot1{~?2 3 4$#adot1
(($t)$0) -: (r._5e6+?($t)$1e8) = t=:adot1{~?2 3 4$#adot1
(($t)$0) -: (($t)$;:'8-+.abc') = t=:adot1{~?2 3 4$#adot1

NB. literal4
(($t)$1) -: t = t=:adot2{~?2 3 4$#adot2
(($t)$0) -: (?($t)$2)          = t=:adot2{~?2 3 4$#adot2
(($t)$0) -: (_5e8+?($t)$1e9)   = t=:adot2{~?2 3 4$#adot2
(($t)$0) -: (o._5e8+?($t)$1e9) = t=:adot2{~?2 3 4$#adot2
(($t)$0) -: (r._5e6+?($t)$1e8) = t=:adot2{~?2 3 4$#adot2
(($t)$0) -: (($t)$;:'8-+.abc') = t=:adot2{~?2 3 4$#adot2

NB. symbol
(($t)$1) -: t = t=:sdot0{~?2 3 4$#sdot0
(($t)$0) -: (?($t)$2)          = t=:sdot0{~?2 3 4$#sdot0
(($t)$0) -: (_5e8+?($t)$1e9)   = t=:sdot0{~?2 3 4$#sdot0
(($t)$0) -: (o._5e8+?($t)$1e9) = t=:sdot0{~?2 3 4$#sdot0
(($t)$0) -: (r._5e6+?($t)$1e8) = t=:sdot0{~?2 3 4$#sdot0
(($t)$0) -: (($t)$;:'8-+.abc') = t=:sdot0{~?2 3 4$#sdot0

NB. integer
(($t)$1) -: t = t=:_1e9+?2 3 4$2e9
(a=b)-:0=a-b     [ a=:_5+?200$10         [ b=:_5+?200$10
(a=b)-:a=b{0 1 2 [ a=:(?100$2){0 1 2     [ b=:?100$2
(a=b)-:a=}.3.4,b [ a=:?200$10            [ b=:?200$10

NB. real
(($t)$1) -: t = t=:o._1e9+?2 3 4$2e9
(a=b)-:0=a-b     [ a=:o._5+?200$10       [ b=:o._5+?200$10
(a=b)-:a=}.3.4,b [ a=:}.3.4,?100$2       [ b=:?100$2
(a=b)-:a=}.3.4,b [ a=:}.3.4,_5+?200$10   [ b=:_5+?200$10
(a=b)-:a=}.3j4,b [ a=:o._5+?200$10       [ b=:o._5+?200$10

NB. complex
(($t)$1) -: t = t=:j./_1e9+?2 3 4$2e9
(a=b)-:0=a-b     [ a=:r._5+?200$10       [ b=:r._5+?200$10
(a=b)-:a=}.3j4,b [ a=:}.3j4,?100$2       [ b=:?100$2
(a=b)-:a=}.3j4,b [ a=:}.3j4,_5+?200$10   [ b=:_5+?200$10
(a=b)-:a=}.3j4,b [ a=:}.3j4,o._5+?200$10 [ b=:o._5+?200$10

NB. boxed
(($t)$1) -: t = t=:<"1?2 3 4 5$10
(($t)$0) -: (?($t)$2)          = t=:<"1?2 3 4$10
(($t)$0) -: (($t)$'8-+.abc')   = t=:<"1?2 3 4$10
(($t)$0) -: (_5e8+?($t)$1e9)   = t=:<"1?2 3 4$10
(($t)$0) -: (o._5e8+?($t)$1e9) = t=:<"1?2 3 4$10
(($t)$0) -: (r._5e6+?($t)$1e8) = t=:<"1?2 3 4$10

NB. extended

(($t)$1) -: t = t=:x: _1e9+?2 3 4$2e9
(=/~i. 20 ) -: =/~ 2^i. 20x
(=/~i. 20 ) -: =/~ 2x ^ 2 ^ i. 20x

NB. rational
(($t)$1) -: t = t=:(x: _1e9+?2 3 4$2e9) % >: 2 3 4$2e9
(=/~i. 10 * 20) -: =/~ , (2^i. 20x) %/&, (3^i. 10x)
(=/~i. 10 * 20 ) -: =/~ , (2x ^ 2 ^ i. 20x) %/&, (3x ^ 2 ^ i. 10x)


'length error' -: 3 4     = etx  5 6 7
'length error' -: 3 4     =~etx 5 6 7
'length error' -: (i.3 4) = etx i.5 4
'length error' -: (i.3 4) =~etx i.5 4    


randfini''


epilog''
