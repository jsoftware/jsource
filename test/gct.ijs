NB. comparison tolerance ------------------------------------------------

e=: 0, 2 ^ - 42 43  45 46  52 53

f=: 3 : 0
 xx=: y
 yy=: y*1-e
 assert. 1 0 0 1 1 1 1   -: xx =  yy
 assert. 0 1 1 0 0 0 0   -: xx ~: yy
 assert. 0 0 0 0 0 0 0   -: xx <  yy
 assert. 1 0 0 1 1 1 1   -: xx <: yy
 assert. 0 1 1 0 0 0 0   -: xx >  yy
 assert. 1 1 1 1 1 1 1   -: xx >: yy
 assert. 0 1 2 0 0 0 0   -: i.~  yy
 assert. 0 1 2 0 0 0 0   -: yy i. 0+yy
 assert. 6 1 2 6 6 6 6   -: i:~  yy
 assert. 6 1 2 6 6 6 6   -: yy i: 0+yy
 assert. 1 1 1 0 0 0 0   -: ~:   yy
 assert. 0 1 2           -: I.@~: yy
 assert. (3{.yy)         -: ~.   yy
 assert. (1 2{yy)        -: yy -. xx
 assert. (1 2{yy)        -: yy -. 13$xx
 assert. (i < /.i.#e)    -: yy < /.i.#e [ i=: i.~yy
 assert. (i +//.i.#e)    -: yy +//.i.#e [ i=: i.~yy
 assert. (i # /.i.#e)    -: yy # /.i.#e [ i=: i.~yy
 1
)

f"0 ]1
f"0 ]0.001 * 5 10 ?@$ 2e9


4!:55 ;:'e f i xx yy'


