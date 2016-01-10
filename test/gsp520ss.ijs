NB. x{y for sparse x, sparse y ------------------------------------------

p=: (?6 3$2) * ?6 3 4$7
q=: (?11 7 9$2) * 0,?10 7 9 8$124 
pc=: ; (i.1+pr) <"1@comb&.>pr=:#$p
qc=: ; (i.1+qr) <"1@comb&.>qr=:#$q

f=: 4 : '(scheck z) *. (p{q) -: z=: (s=: (2;x)$.p) { t=:(2;y)$.q'
pc f&>/ qc

f=: 4 : '(scheck z) *. (p{"_ 1 q) -: z=: (s=: (2;x)$.p) {"_ 1 t=:(2;y)$.q'
pc f&>/ qc [ q=: (0,7$1)*"1 (?11 7 9$2) * ?11 7 9 8$124

f=: 4 : '(scheck z) *. (p{"_ 2 q) -: z=: (s=: (2;x)$.p) {"_ 2 t=:(2;y)$.q'
pc f&>/ qc [ q=: (0,8$1)*"2 (?11 7 9$2) * ?11 7 9 8$124

f=: 4 : '(scheck z) *. (p{"_ 3 q) -: z=: (s=: (2;x)$.p) {"_ 3 t=:(2;y)$.q'
pc f&>/ qc [ q=: (0,6$1)*"3 (?11 7 9$2) * ?11 7 9 8$124

'domain error' -: ($.o.i.2 3)      { etx $.i.9

'index error'  -: ($.i.2 3)        { etx $.0,i.3 5
'index error'  -: ((3;6)$.$.i.2 3) { etx $.0,i.5 2 1

'non-unique sparse elements' -: ($.i.2 3){     etx $.i.6 3
'non-unique sparse elements' -: ($.i.2 3){"2 1 etx $.i.2 6


4!:55 ;:'f p pc pr q qc qr s t z'


