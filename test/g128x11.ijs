prolog './g128x11.ijs'
NB. 128!:11 ----------------------------------------------------------------

NB. right trim

Dtb=: #~ ([: +./\. ' '&~:)

(Dtb"1 -: 0&(128!:11)) 'a'
(Dtb"1 -: 0&(128!:11)) ' '
(Dtb"1 -: 0&(128!:11)) ''
(Dtb"1 -: 0&(128!:11)) a.
(Dtb"1 -: 0&(128!:11)) 234 123$a.
(Dtb"1 -: 0&(128!:11)) u: a.
(Dtb"1 -: 0&(128!:11)) u: 234 123$a.
(Dtb"1 -: 0&(128!:11)) 10&u: a.
(Dtb"1 -: 0&(128!:11)) 10&u: 234 123$a.
(Dtb"1 -: 0&(128!:11)) 7&u: 16b1ffff+i.256
(Dtb"1 -: 0&(128!:11)) 234 123$ 7&u: 16b1ffff+i.256
(Dtb"1 -: 0&(128!:11)) 9&u: 1bf7fff000 +i.256
(Dtb"1 -: 0&(128!:11)) 234 123$ 9&u: 1bf7fff000 +i.256
(Dtb"1 -: 0&(128!:11)) >' aa     ';'bb bbb  '
(Dtb"1 -: 0&(128!:11)) u: >' aa     ';'bb bbb  '
(Dtb"1 -: 0&(128!:11)) 10&u: >' aa     ';'bb bbb  '

(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 'a'
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ ' '
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ ''
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ a.
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 234 123$a.
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ u: a.
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ u: 234 123$a.
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 10&u: a.
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 10&u: 234 123$a.
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 7&u: 16b1ffff+i.256
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 234 123$ 7&u: 16b1ffff+i.256
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 9&u: 1bf7fff000 +i.256
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 234 123$ 9&u: 1bf7fff000 +i.256
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ >' aa     ';'bb bbb  '
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ u: >' aa     ';'bb bbb  '
(Dtb"1 -: 0&(128!:11)) 1000&{."1[ 10&u: >' aa     ';'bb bbb  '

A=: 3#<1000&{."1[ 900 11$a.
B=: 3#<1000&{."1[ 900 111$a.
A1=: 3#<1000&{."1[ u: 900 11$a.
B1=: 3#<1000&{."1[ u: 900 111$a.
A2=: 3#<1000&{."1[ 10&u: 900 11$a.
B2=: 3#<1000&{."1[ 10&u: 900 111$a.

(Dtb"1 -: 0&(128!:11))&> A
(Dtb"1 -: 0&(128!:11))&> B
(Dtb"1 -: 0&(128!:11))&> A1
(Dtb"1 -: 0&(128!:11))&> B1
(Dtb"1 -: 0&(128!:11))&> A2
(Dtb"1 -: 0&(128!:11))&> B2

(] -: 0&(128!:11)) i. 3 4
(] -: 0&(128!:11)) 0.5 + i. 3 4
(] -: 0&(128!:11)) 1j1 + i. 3 4
(] -: 0&(128!:11)) x: i. 3 4
(] -: 0&(128!:11)) 3 4$1r2 2r3 3r4
(] -: 0&(128!:11)) <a.
(] -: 0&(128!:11)) <234 123$a.
(] -: 0&(128!:11)) 3 4$<a.
(] -: 0&(128!:11)) 3 4$<234 123$a.
(] -: 0&(128!:11)) $. i. 3 4

NB. left trim

Dlb=: }.~ (=&' ' (i.) 0:)

(Dlb"1 -: 1&(128!:11)) 'a'
(Dlb"1 -: 1&(128!:11)) ' '
(Dlb"1 -: 1&(128!:11)) ''
(Dlb"1 -: 1&(128!:11)) a.
(Dlb"1 -: 1&(128!:11)) 234 123$a.
(Dlb"1 -: 1&(128!:11)) u: a.
(Dlb"1 -: 1&(128!:11)) u: 234 123$a.
(Dlb"1 -: 1&(128!:11)) 10&u: a.
(Dlb"1 -: 1&(128!:11)) 10&u: 234 123$a.
(Dlb"1 -: 1&(128!:11)) 7&u: 16b1ffff+i.256
(Dlb"1 -: 1&(128!:11)) 234 123$ 7&u: 16b1ffff+i.256
(Dlb"1 -: 1&(128!:11)) 9&u: 1bf7fff000 +i.256
(Dlb"1 -: 1&(128!:11)) 234 123$ 9&u: 1bf7fff000 +i.256
(Dlb"1 -: 1&(128!:11)) >' aa     ';'bb bbb  '
(Dlb"1 -: 1&(128!:11)) u: >' aa     ';'bb bbb  '
(Dlb"1 -: 1&(128!:11)) 10&u: >' aa     ';'bb bbb  '

(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 'a'
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ ' '
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ ''
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ a.
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 234 123$a.
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ u: a.
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ u: 234 123$a.
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 10&u: a.
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 10&u: 234 123$a.
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 7&u: 16b1ffff+i.256
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 234 123$ 7&u: 16b1ffff+i.256
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 9&u: 1bf7fff000 +i.256
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 234 123$ 9&u: 1bf7fff000 +i.256
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ >' aa     ';'bb bbb  '
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ u: >' aa     ';'bb bbb  '
(Dlb"1 -: 1&(128!:11)) _1000&{."1[ 10&u: >' aa     ';'bb bbb  '

A=: 3#<_1000&{."1[ 900 11$a.
B=: 3#<_1000&{."1[ 900 111$a.
A1=: 3#<_1000&{."1[ u: 900 11$a.
B1=: 3#<_1000&{."1[ u: 900 111$a.
A2=: 3#<_1000&{."1[ 10&u: 900 11$a.
B2=: 3#<_1000&{."1[ 10&u: 900 111$a.

(Dlb"1 -: 1&(128!:11))&> A
(Dlb"1 -: 1&(128!:11))&> B
(Dlb"1 -: 1&(128!:11))&> A1
(Dlb"1 -: 1&(128!:11))&> B1
(Dlb"1 -: 1&(128!:11))&> A2
(Dlb"1 -: 1&(128!:11))&> B2

(] -: 1&(128!:11)) i. 3 4
(] -: 1&(128!:11)) 0.5 + i. 3 4
(] -: 1&(128!:11)) 1j1 + i. 3 4
(] -: 1&(128!:11)) x: i. 3 4
(] -: 1&(128!:11)) 3 4$1r2 2r3 3r4
(] -: 1&(128!:11)) <a.
(] -: 1&(128!:11)) <234 123$a.
(] -: 1&(128!:11)) 3 4$<a.
(] -: 1&(128!:11)) 3 4$<234 123$a.
(] -: 1&(128!:11)) $. i. 3 4

NB. leftright trim

Dltb=: #~ ([: (+./\ *. +./\.) ' '&~:)

(Dltb"1 -: 2&(128!:11)) 'a'
(Dltb"1 -: 2&(128!:11)) ' '
(Dltb"1 -: 2&(128!:11)) ''
(Dltb"1 -: 2&(128!:11)) a.
(Dltb"1 -: 2&(128!:11)) 234 123$a.
(Dltb"1 -: 2&(128!:11)) u: a.
(Dltb"1 -: 2&(128!:11)) u: 234 123$a.
(Dltb"1 -: 2&(128!:11)) 10&u: a.
(Dltb"1 -: 2&(128!:11)) 10&u: 234 123$a.
(Dltb"1 -: 2&(128!:11)) 7&u: 16b1ffff+i.256
(Dltb"1 -: 2&(128!:11)) 234 123$ 7&u: 16b1ffff+i.256
(Dltb"1 -: 2&(128!:11)) 9&u: 1bf7fff000 +i.256
(Dltb"1 -: 2&(128!:11)) 234 123$ 9&u: 1bf7fff000 +i.256
(Dltb"1 -: 2&(128!:11)) >' aa     ';'bb bbb  '
(Dltb"1 -: 2&(128!:11)) u: >' aa     ';'bb bbb  '
(Dltb"1 -: 2&(128!:11)) 10&u: >' aa     ';'bb bbb  '

(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 'a'
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ ' '
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ ''
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ a.
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 234 123$a.
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ u: a.
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ u: 234 123$a.
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 10&u: a.
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 10&u: 234 123$a.
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 7&u: 16b1ffff+i.256
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 234 123$ 7&u: 16b1ffff+i.256
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 9&u: 1bf7fff000 +i.256
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 234 123$ 9&u: 1bf7fff000 +i.256
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ >' aa     ';'bb bbb  '
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ u: >' aa     ';'bb bbb  '
(Dltb"1 -: 2&(128!:11)) _1000&{."1[ 10&u: >' aa     ';'bb bbb  '

A=: 3#<_1000&{."1[ 900 11$a.
B=: 3#<_1000&{."1[ 900 111$a.
A1=: 3#<_1000&{."1[ u: 900 11$a.
B1=: 3#<_1000&{."1[ u: 900 111$a.
A2=: 3#<_1000&{."1[ 10&u: 900 11$a.
B2=: 3#<_1000&{."1[ 10&u: 900 111$a.

(Dltb"1 -: 2&(128!:11))&> A
(Dltb"1 -: 2&(128!:11))&> B
(Dltb"1 -: 2&(128!:11))&> A1
(Dltb"1 -: 2&(128!:11))&> B1
(Dltb"1 -: 2&(128!:11))&> A2
(Dltb"1 -: 2&(128!:11))&> B2

(] -: 2&(128!:11)) i. 3 4
(] -: 2&(128!:11)) 0.5 + i. 3 4
(] -: 2&(128!:11)) 1j1 + i. 3 4
(] -: 2&(128!:11)) x: i. 3 4
(] -: 2&(128!:11)) 3 4$1r2 2r3 3r4
(] -: 2&(128!:11)) <a.
(] -: 2&(128!:11)) <234 123$a.
(] -: 2&(128!:11)) 3 4$<a.
(] -: 2&(128!:11)) 3 4$<234 123$a.
(] -: 2&(128!:11)) $. i. 3 4

NB. more test

A=: 111 36$'  a b  c   d    efg  hi'
B=: u: A
C=: 10&u: A
A1=: (_55&{."1)A
B1=: (_55&{."1)B
C1=: (_55&{."1)C
NB. test misaligned access
0 ~: 32| 15!:14<'A'
0 ~: 32| 15!:14<'B'
0 ~: 32| 15!:14<'C'
0 ~: 32| 15!:14<'A1'
0 ~: 32| 15!:14<'B1'
0 ~: 32| 15!:14<'C1'
A -:&:(0&(128!:11)) B
A -:&:(0&(128!:11)) C
A -:&:(1&(128!:11)) B
A -:&:(1&(128!:11)) C
A -:&:(2&(128!:11)) B
A -:&:(2&(128!:11)) C
A1 -:&:(0&(128!:11)) B1
A1 -:&:(0&(128!:11)) C1
A1 -:&:(1&(128!:11)) B1
A1 -:&:(1&(128!:11)) C1
A1 -:&:(2&(128!:11)) B1
A1 -:&:(2&(128!:11)) C1

(Dtb"1 -: 0&(128!:11)) A
(Dtb"1 -: 0&(128!:11)) B
(Dtb"1 -: 0&(128!:11)) C
(Dlb"1 -: 1&(128!:11)) A1
(Dlb"1 -: 1&(128!:11)) B1
(Dlb"1 -: 1&(128!:11)) C1
(Dltb"1 -: 2&(128!:11)) A1
(Dltb"1 -: 2&(128!:11)) B1
(Dltb"1 -: 2&(128!:11)) C1


epilog''

