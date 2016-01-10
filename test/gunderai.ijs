NB. u&.(a.&i.) ----------------------------------------------------------

s=: 1000  ?@$ 256
t=: 13 47 ?@$ 256

f=: 256|>:
(a.{~ f s) -: f f.&.(a.&i.) a.{~s
(a.{~ f t) -: f f.&.(a.&i.) a.{~t

f=: 256|<:
(a.{~ f s) -: f f.&.(a.&i.) a.{~s
(a.{~ f t) -: f f.&.(a.&i.) a.{~t

f=: -
(a.{~ f s) -: f f.&.(a.&i.) a.{~s
(a.{~ f t) -: f f.&.(a.&i.) a.{~t

p=: 22 (2?256)} 123 (22 b.) i.256
f=: {&p 
(a.{~ f s) -: f f.&.(a.&i.) a.{~s
(a.{~ f t) -: f f.&.(a.&i.) a.{~t

test1=: 4 : 0 " 0
 assert. (a.{~ y x b. s) -: y&(x b.)  &.(a.&i.) a.{~s
 assert. (a.{~ y x b. t) -: y&(x b.)  &.(a.&i.) a.{~t
 assert. (a.{~ s x b. y) -:   (x b.)&y&.(a.&i.) a.{~s
 assert. (a.{~ t x b. y) -:   (x b.)&y&.(a.&i.) a.{~t
 assert. (a.{~ y x b. s) -: ((a.{~y       x b. i.256) {~ a.i.]) a.{~s
 assert. (a.{~ y x b. t) -: ((a.{~y       x b. i.256) {~ a.i.]) a.{~t
 assert. (a.{~ s x b. y) -: ((a.{~(i.256) x b. y    ) {~ a.i.]) a.{~s
 assert. (a.{~ t x b. y) -: ((a.{~(i.256) x b. y    ) {~ a.i.]) a.{~t
 1
)

*./ , (16+i.16) test1/ i.256


NB. x m b.&.(a.&i.) y -------------------------------------------------

test2=: 3 : 0 " 0
 g=: y b.
 f=: y b.&.(a.&i.)
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=: 13 47 ?@$ 256 [ q=: 13 47 ?@$ 256
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=: 13 47 ?@$ 256 [ q=:       ?   256
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=:       ?   256 [ q=: 13 47 ?@$ 256
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=: 13 47 ?@$ 256 [ q=: 13    ?@$ 256
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=: 13    ?@$ 256 [ q=: 13 47 ?@$ 256
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=: 13 48 ?@$ 256 [ q=: 13    ?@$ 256
 assert. (a.{~p g q) -: (p{a.) f q{a. [ p=: 13    ?@$ 256 [ q=: 13 48 ?@$ 256
 1
)

test2 16+i.16

NB. x0=: a. {~ 10 100000 ?@$ 256
NB. x1=: a. {~ 10 100001 ?@$ 256
NB. y =: a. {~ 10       ?@$ 256
NB. t0=: 6!:2 'x0 22 b.&.(a.&i.) y'
NB. t1=: 6!:2 'x1 22 b.&.(a.&i.) y'
NB. 1.2 < t1 % t0

NB. x0=: a. {~ (?256) 23 b. i.256
NB. x1=: a. {~ ?~ 256
NB. y =: a. {~ 5e5 ?@$ 256
NB. t0=: 6!:2 '(x0 {~ a. i. ]) y'
NB. t1=: 6!:2 '(x1 {~ a. i. ]) y'
NB. 2 < t1 % t0


NB. x m b./&.(a.&i.) y -------------------------------------------------

test3=: 3 : 0 " 0
 g=: y b./
 f=: y b./&.(a.&i.)
 assert. (a.{~g q) -: f q{a. [ q=:    31 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=:    32 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=:    63 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=:    64 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=: 17 31 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=: 17 32 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=: 13 63 ?@$ 256
 assert. (a.{~g q) -: f q{a. [ q=: 13 64 ?@$ 256
 1
)

test3 16+i.16

NB. y =: a. {~ (1+5e5) ?@$ 256
NB. t0=: 6!:2 '    22 b./&.(a.i.]) y'
NB. t1=: 6!:2 'a.{~22 b./   a.i.   y'
NB. 2 < t1 % t0

NB. y =: a. {~ 1373 61 ?@$ 256
NB. t0=: 6!:2 '    27 b./&.(a.i.]) y'
NB. t1=: 6!:2 'a.{~27 b./   a.i.   y'
NB. 2 < t1 % t0


4!:55 ;:'f g p q s t t0 t1 test1 test2 test3 x0 x1 y'


