prolog './g331col.ijs'
NB. x <;.n y for boxed x and matrix y -----------------------------------

cutcol=: 2 : 0
  :
 assert. 2=$#y
 assert. (,2)-:$x
 assert. 32=3!:0 x
 assert. 1>:#@$&>x
 assert. (0&=#&>x)+.($y)=#&>x
 assert. (a:=x)+.1=3!:0&>x
 'b c'=. x
 'p q'=. $y
 if. ''-:b do. i=. 0 
 else.         i=. b {.;.n i.#b [ p=. b # ;.n b end.
 if. ''-:c do. j=. 0 
 else.         j=. c {.;.n i.#c [ q=. c # ;.n c end.
 ((i,.p),."1/ j,.q) u;.0 y
)

test=: 3 : 0
 'b c yy'=: y
 for_k. 1 _1 2 _2 do.
  assert. ('';'') (< cutcol k -: <;. k) yy
  assert. ('';c ) (< cutcol k -: <;. k) yy
  assert. (b ;'') (< cutcol k -: <;. k) yy
  assert. (b ;c ) (< cutcol k -: <;. k) yy
 end.
 1
)

m=: 50
n=: 300
b=: (i.m)e.?20$m
c=: (i.n)e.?20$n
y=: a{~?(m,n)$#a=: ' abcd efgh ijkl mnop qrst uvwy xz '
y1=: a{~?(m,n)$#a=: u:' abcd efgh ijkl mnop qrst uvwy xz '
y2=: a{~?(m,n)$#a=: 10&u:' abcd efgh ijkl mnop qrst uvwy xz '
y3=: a{~?(m,n)$#a=: s:@<"0 ' abcd efgh ijkl mnop qrst uvwy xz '

test b    ;c    ;y
test b    ;(0*c);y
test (0*b);c    ;y
test (0*b);(0*c);y

test b    ;c    ;y1
test b    ;(0*c);y1
test (0*b);c    ;y1
test (0*b);(0*c);y1

test b    ;c    ;y2
test b    ;(0*c);y2
test (0*b);c    ;y2
test (0*b);(0*c);y2

test b    ;c    ;y3
test b    ;(0*c);y3
test (0*b);c    ;y3
test (0*b);(0*c);y3




epilog''

