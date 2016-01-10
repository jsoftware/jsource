NB. 5!:5 sparse arrays --------------------------------------------------

test=: 3 : 0
 yy=: y
 xx=: ". 5!:5 <'y'
 assert. xx -:&type yy
 assert. scheck xx
 assert. xx -: yy
 1
)

test x=: 1$.3 4 5;0 1;0
test x=: 1$.3 4 5;0 1;-~2
test x=: 1$.3 4 5;0 1;-~2.1
test x=: 1$.3 4 5;0 1;-~2j1

test x=: $. 3 4 5 ?@$ 5
test o. x
test x=: $. 10 ?@$ 5

r=: #$d=: (2 3 5 ?@$ 2) * 2 3 5 7 9 ?@$ 2
c=: (i.1+r) comb&.> r
c ([: */ (2&;@[ test@$. ])"1 _)&><d

r=: #$d=: (2 3 5 ?@$ 4) * 2 3 5 7 9 ?@$ 10
c=: (i.1+r) comb&.> r
c ([: */ (2&;@[ test@$. ])"1 _)&><d

r=: #$d=: (2 3 5 ?@$ 4) * 2 3 5 7 9 ?@$ 0
c=: (i.1+r) comb&.> r
c ([: */ (2&;@[ test@$. ])"1 _)&><d


NB. 5!:5 empty sparse arrays --------------------------------------------

s=: 0 (?#s)}s=: ?10$5
test 1$.s
test 1$.s;''
test 1$.s;i.#s
test 1$.s;5?#s
test 1$.s;(i.#s);0
test 1$.s;(5?#s);0
test 1$.s;(5?#s);-~4
test 1$.s;(5?#s);-~4.1
test 1$.s;(5?#s);-~4j1


4!:55 ;:'c d r s test x xx yy'


