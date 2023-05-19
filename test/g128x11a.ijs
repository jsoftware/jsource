prolog './g128x11a.ijs'
NB. 128!:11 ----------------------------------------------------------------

NB. benchmark for 128!:11

L0=: (1[QKTEST){90000 9000
S1=: a.
S2=: (300$AlphaNum_j_) (300{.I.0=|?2000#5)}2000#' '
S3=: (300$AlphaNum_j_) (300{.I.0=|?2000#200)}2000#' '

Dtb=: #~ ([: +./\. ' '&~:) 
Dlb=: }.~ (=&' ' (i.) 0:)
Dltb=: #~ ([: (+./\ *. +./\.) ' '&~:)

dtb1=: Dtb"1^:(2 131072 262144 e.~ 3!:0)
dtb2=: 0&(128!:11)^:(2 131072 262144 e.~ 3!:0)
dlb1=: Dlb"1^:(2 131072 262144 e.~ 3!:0)
dlb2=: 1&(128!:11)^:(2 131072 262144 e.~ 3!:0)
dltb1=: Dltb"1^:(2 131072 262144 e.~ 3!:0)
dltb2=: 2&(128!:11)^:(2 131072 262144 e.~ 3!:0)

test1=: 4 : 0

A=. 3#<(L0, 17)$x
B=. 3#<255{."1[ (L0, 111)$x
C=. 3#<_255{."1[ (L0, 111)$x
A1=. 3#<u: (L0, 17)$x
B1=. 3#<255{."1[ u: (L0, 111)$x
C1=. 3#<_255{."1[ u: (L0, 111)$x
A2=. 3#<10&u: (L0, 17)$x
B2=. 3#<255{."1[ 10&u: (L0, 111)$x
C2=. 3#<_255{."1[ 10&u: (L0, 111)$x

if. 0=y do.
case1=. dtb1 f.
case2=. dtb2 f.
elseif. 1=y do.
case1=. dlb1 f.
case2=. dlb2 f.
elseif. 2=y do.
case1=. dltb1 f.
case2=. dltb2 f.
end.

fn=. y{::' rtrim';' ltrim';' lrtrim'
techo^:PRINTMSG 'short literal',fn
techo^:PRINTMSG (6!:2) 'PA=. case1&.> A'
techo^:PRINTMSG (6!:2) 'QA=. case2&.> A'
assert. (PA-:QA)
techo^:PRINTMSG 'long literal',fn
techo^:PRINTMSG (6!:2) 'PB=. case1&.> B'
techo^:PRINTMSG (6!:2) 'QB=. case2&.> B'
techo^:PRINTMSG (6!:2) 'PC=. case1&.> C'
techo^:PRINTMSG (6!:2) 'QC=. case2&.> C'
assert. (PB-:QB)
assert. (PC-:QC)
techo^:PRINTMSG 'short literal2',fn
techo^:PRINTMSG (6!:2) 'PA1=. case1&.> A1'
techo^:PRINTMSG (6!:2) 'QA1=. case2&.> A1'
assert. (PA1-:QA1)
techo^:PRINTMSG 'long literal2',fn
techo^:PRINTMSG (6!:2) 'PB1=. case1&.> B1'
techo^:PRINTMSG (6!:2) 'QB1=. case2&.> B1'
techo^:PRINTMSG (6!:2) 'PC1=. case1&.> C1'
techo^:PRINTMSG (6!:2) 'QC1=. case2&.> C1'
assert. (PB1-:QB1)
assert. (PC1-:QC1)
techo^:PRINTMSG 'short literal4',fn
techo^:PRINTMSG (6!:2) 'PA2=. case1&.> A2'
techo^:PRINTMSG (6!:2) 'QA2=. case2&.> A2'
assert. (PA2-:QA2)
techo^:PRINTMSG 'long literal4',fn
techo^:PRINTMSG (6!:2) 'PB2=. case1&.> B2'
techo^:PRINTMSG (6!:2) 'QB2=. case2&.> B2'
techo^:PRINTMSG (6!:2) 'PC2=. case1&.> C2'
techo^:PRINTMSG (6!:2) 'QC2=. case2&.> C2'
assert. (PB2-:QB2)
assert. (PC2-:QC2)
''
)

S1 test1 0
S2 test1 0
S3 test1 0
S1 test1 1
S2 test1 1
S3 test1 1
S1 test1 2
S2 test1 2
S3 test1 2

4!:55 ;:'Dlb dlb1 dlb2 Dtb dtb1 dtb2 Dltb dltb1 dltb2 L0 S1 S2 S3 test1'

epilog''
