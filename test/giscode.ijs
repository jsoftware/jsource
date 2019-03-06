1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './giscode.ijs'
NB. i. special code -----------------------------------------------------

test=: 2 : 0
:
 assert. x (u -: v) y
 assert. y (u -: v) x
 1
)

1 2 3 (e.i.1: ) test (4 : '(x e. y)i.1') ''
1 2 3 (e.i.0: ) test (4 : '(x e. y)i.0') ''
1 2 3 (e.i:1: ) test (4 : '(x e. y)i:1') ''
1 2 3 (e.i:0: ) test (4 : '(x e. y)i:0') ''
1 2 3 (+ /@e. ) test (4 : '+ /x e. y'  ) ''
1 2 3 (+./@e. ) test (4 : '+./x e. y'  ) ''
1 2 3 (*./@e. ) test (4 : '*./x e. y'  ) ''
1 2 3 (I. @e. ) test (4 : 'I. x e. y'  ) ''


4!:55 ;:'test'


