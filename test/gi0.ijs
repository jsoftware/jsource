NB. x i.y on boolean scalar y -------------------------------------------

f0=: 4 : 0 " 0
 if. y<x do. v=: 0 y}x$1 else. v=: x$1 end.
 assert. (x<.y)=v i. 0
 1
)

f1=: 4 : 0 " 0
 if. y<x do. v=: 1 y}x$0 else. v=: x$0 end.
 assert. (x<.y)=v i. 1
 1
)

*./, f0/~i.40
*./, f1/~i.40


4!:55 ;:'b f0 f1 i n x'


