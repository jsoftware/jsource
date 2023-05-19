prolog './g128x10.ijs'
NB. 128!:10/11 base64 -------------------------------------------------------

NB. base64

BASE64=: (a.{~ ,(a.i.'Aa') +/i.26),'0123456789+/'
BASE64_2=: (,/ (,"0)/~i.64){BASE64

NB. =========================================================
NB.*tobase64_old v To base64 representation
tobase64_old=: 3 : 0
res=. BASE64 {~ #. _6 [\ , (8#2) #: a. i. y
res, (0 2 1 i. 3 | # y) # '='
)

NB. =========================================================
NB.*frombase64_old v From base64 representation
frombase64_old=: 3 : 0
pad=. _2 >. (y i. '=') - #y
pad }. a. {~ #. _8 [\ , (6#2) #: BASE64 i. y
)

f =: 3!:10
'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'
'domain error' -: f etx u: 'abc'
'domain error' -: f etx 10&u: 'abc'

'' -: 3!:10 ''
'Zg==' -: 3!:10 'f'
'Zg==' -: 3!:10 ,'f'
'Zm8=' -: 3!:10 'fo'
'Zm9v' -: 3!:10 'foo'
'Zm9vYg==' -: 3!:10 'foob'
'Zm9vYmE=' -: 3!:10 'fooba'
'Zm9vYmFy' -: 3!:10 'foobar'

f =: 3!:11
'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'
'domain error' -: f etx u: 'abc'
'domain error' -: f etx 10&u: 'abc'
'domain error' -: 3!:11 etx 'a'
'domain error' -: 3!:11 etx ',a'

(3!:11 '') -:  ''
(3!:11 'Zg==') -: ,'f'
(3!:11 'Zm8=') -:  'fo'
(3!:11 'Zm9v') -:  'foo'
(3!:11 'Zm9vYg==') -:  'foob'
(3!:11 'Zm9vYmE=') -:  'fooba'
(3!:11 'Zm9vYmFy') -:  'foobar'
(-:   3!:11 @: (3!:10)) a. {~ 50 37 ?@$ 256
(-:   3!:11 @: (3!:10)) a. {~ 35 36 ?@$ 256
(-:   3!:11 @: (3!:10)) a. {~ 41 35 ?@$ 256

(-:   (frombase64_old"1) @: (3!:10)) a. {~ 50 37 ?@$ 256
(-:   3!:11 @: (tobase64_old"1)) a. {~ 35 36 ?@$ 256
(-:   (frombase64_old"1) @: (tobase64_old"1)) a. {~ 41 35 ?@$ 256


L0=: QKTEST{9000 900
S1=: a.

test1=: 4 : 0

A=. 1#<(7, L0)$x
B=. 1#<(3, 123*L0)$x
C=. 1#<(3, 543*L0)$x

techo^:PRINTMSG 'short literal'
techo^:PRINTMSG 20&(6!:2) 'PA=.  tobase64_old"1&.> A'
techo^:PRINTMSG 20&(6!:2) 'QA=.  (3!:10)&.> A'
assert. (PA-:QA)
techo^:PRINTMSG 20&(6!:2) 'P=.  frombase64_old"1&.> PA'
techo^:PRINTMSG 20&(6!:2) 'Q=.  (3!:11)&.> QA'
assert. (P-:Q)
techo^:PRINTMSG 'long literal'
techo^:PRINTMSG 1&(6!:2) 'PB=.  tobase64_old"1&.> B'
techo^:PRINTMSG 5&(6!:2) 'QB=.  (3!:10)&.> B'
assert. (PB-:QB)
techo^:PRINTMSG 1&(6!:2) 'P=.  frombase64_old"1&.> PB'
techo^:PRINTMSG 5&(6!:2) 'Q=.  (3!:11)&.> QB'
assert. (P-:Q)
techo^:PRINTMSG 1&(6!:2) 'PC=.  tobase64_old"1&.> C'
techo^:PRINTMSG 5&(6!:2) 'QC=.  (3!:10)&.> C'
assert. (PC-:QC)
techo^:PRINTMSG 1&(6!:2) 'P=.  frombase64_old"1&.> PC'
techo^:PRINTMSG 5&(6!:2) 'Q=.  (3!:11)&.> QC'
assert. (P-:Q)
''
)

S1 test1 0

4!:55 ;:'BASE64 BASE64_2 tobase64_old frombase64_old f L0 S1 test1'

epilog''
