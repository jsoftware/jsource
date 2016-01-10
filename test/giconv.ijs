NB. almost integers as integer arguments --------------------------------

f=: 3 : 0 " 0
 yy=: y
 assert. 1: >:^:yy 0
 assert. 1: yy $ 4
 assert. 1: yy # 4
 assert. 1: yy{ i.120
 assert. 1: yy{.1 2 3
 assert. 1: yy}.1 2 3
 assert. 1: +"yy 1
 assert. 1: i. yy
 assert. 1: i: yy
 1
)

e=: 1e_14

f (1+e)*x=: >:20 ?@$100
f (1-e)*x

1 [ 1 +: (1+e),1-e
1 [ 1 +:~(1+e),1-e
1 [ 1 *: (1+e),1-e
1 [ 1 *:~(1+e),1-e

1 [ (  1+e) $. 1 2 3
1 [ (  1-e) $. 1 2 3
1 [ (2*1+e) $. $. 100 ?@$ 2
1 [ (2*1-e) $. $. 100 ?@$ 2
1 [ (3*1+e) $. $. 100 ?@$ 2
1 [ (3*1-e) $. $. 100 ?@$ 2

1 [ (2*1+e)|. i.24
1 [ (2*1-e)|. i.24

1 [ (2*1+e)|.!.0 i.24
1 [ (2*1-e)|.!.0 i.24

1 [ (2 1*1+e)|: i.2 3 4
1 [ (2 1*1-e)|: i.2 3 4

1 [ <;.(1*1+e) ' chthonic'
1 [ <;.(1*1-e) ' chthonic'
1 [ <;.(2*1+e) ' chthonic'
1 [ <;.(2*1-e) ' chthonic'
1 [ <;.(3*1+e) ' chthonic'
1 [ <;.(3*1-e) ' chthonic'


4!:55 ;:'e f x yy'


