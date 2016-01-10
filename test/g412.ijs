NB. !: treatement of placeholder argument -------------------------------

F=: 2 : 0
 assert. 'rank error'   -: m!:n etx 0
 assert. 'rank error'   -: m!:n etx 'a'
 assert. 'length error' -: m!:n etx 0 1
 assert. 'length error' -: m!:n etx 'ab'
 1
)

pc=: (9!:12 '') e. 0 1 2 6 7

1  F 20
". pc#'1 F 30'
4  F 3
6  F 8
6  F 9
7  F 0
7  F 3
9  F 0
9  F 2
9  F 6
9  F 8
9  F 10
9  F 12
9  F 14
9  F 16
9  F 18
9  F 20
9  F 24
9  F 26
9  F 28
9  F 32
9  F 34
9  F 36
9  F 38
9  F 40
13 F 1
13 F 2
13 F 4
13 F 5
13 F 13
13 F 14
13 F 17
13 F 18
18 F 5


4!:55 ;:'F pc'


