prolog './g630.ijs'
NB. & -------------------------------------------------------------------

((|.x),|.y) -: (x=:'Cogito, ergo sum.') ,&|. y=:'I think not'
((|.x),|.y) -: (x=:u:'Cogito, ergo sum.') ,&|. y=:u:'I think not'
((|.x),|.y) -: (x=:10&u:'Cogito, ergo sum.') ,&|. y=:10&u:'I think not'
((|.x),|.y) -: (x=:s:@<"0 'Cogito, ergo sum.') ,&|. y=:s:@<"0 'I think not'

dr =: 5!:2

f =: 3 4 5 6&+
(,&.>3 4 5 6;'&';'+') -: dr <'f'
(f i.4 3) -: 3 4 5 6+"_ i.4 3

f =: -&3 4 5 6
(,&.>'-';'&';3 4 5 6) -: dr <'f'
(f i.4 3) -: (i.4 3)-"_ [3 4 5 6

1 -: 1000 (128&>:) 0


4!:55 ;:'dr f x y'



epilog''

