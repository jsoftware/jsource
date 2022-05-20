prolog './gregex.ijs'
NB. regex ------------------------------------------------------------------

load 'regex'

rxutf8 1

A=: '今日は良い天気です'
B=: 'は良い'
C=: 8 u: '.' 1 } 7 u: B

(,:6 9) -: B rxmatch A
(,:6 9) -: C rxmatch A

D=: utf8 4 u: 3 u: {. 7 u: C
(,:6 3) -: D rxmatch A

16b306f = 3 u: 7 u: D
E=: '\x{306f}' NB. codepoint of D
(,:6 3) -: E rxmatch A

4!:55 ;:'A B C D E'

epilog''

