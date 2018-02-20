NB. test keyfiles

FL=: jpath '~temp/foo'

tkeyfiles=: 3 : 0
keycreate FL
(K1=: i.2 3 4) keywrite FL;'one'
(K2=: <'two';2 4) keywrite FL;'two'
(K3=: <'three';'123') keywrite FL;'three'
(K2=: <'two';'again') keywrite FL;'two'
(K4=: <'four';,.i.10) keywrite FL;'four'
(K3=: <'') keywrite FL;'three'
(K5=: <'five';,.i.10) keywrite FL;'five'
)

ASSERT=: 0 : 0
(;:'one two three four five') -: keydir FL
K1 -: >keyread FL;'one'
K2 -: keyread FL;'two'
K3 -: keyread FL;'three'
K5 -: keyread FL;'five'
(K1;K2,K3,K4,K5) -: keyread FL
(K5,K2,<K1) -: keyread FL;<'five';'two';'one'
keydrop FL;<'two';'five'
(;:'one three four') -: keydir FL
K4 -: keyread FL;'four'
(K1;K3,K4) -: keyread FL
)

tkeyfiles''

0!:1 ASSERT

tkeyfiles=: 3 : 0
keycreate FL
(K1=: i.2 3 4) keywritex FL;'one'
(K2=: <'two';2 4) keywritex FL;'two'
(K3=: <'three';'123') keywritex FL;'three'
(K2=: <'two';'again') keywritex FL;'two'
(K4=: <'four';,.i.10) keywritex FL;'four'
(K3=: <'') keywritex FL;'three'
(K5=: <'five';,.i.10) keywritex FL;'five'
)

ASSERT=: 0 : 0
(;:'one two three four five') -: keydir FL
K1 -: >keyreadx FL;'one'
K2 -: keyreadx FL;'two'
K3 -: keyreadx FL;'three'
K5 -: keyreadx FL;'five'
(K1;K2,K3,K4,K5) -: keyreadx FL
(K5,K2,<K1) -: keyreadx FL;<'five';'two';'one'
keydrop FL;<'two';'five'
(;:'one three four') -: keydir FL
K4 -: keyreadx FL;'four'
(K1;K3,K4) -: keyreadx FL
)

tkeyfiles''

0!:1 ASSERT
