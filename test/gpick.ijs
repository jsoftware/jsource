NB. {:: -----------------------------------------------------------------

boxed=: (32&=)@(3!:0)
cat  =: { @: (i.&.>) @: $

pick=: >@({&>/)@(<"0@|.@[ , <@]) " 1 _

map =: a:&mapp
mapp=: 4 : 'if. boxed y do. (<"0 x,&.><"0 cat y) mapp&.> y else. >x end.'

sz  =: 3 : '>spread_temp'
si  =: 3 : 'y [ spread_temp=:$0'
sc  =: 3 : '0: spread_temp=:spread_temp,<y'
S   =: 2 : 'sz@(sc@x L: y)&si'

t=: 5!:2 <'pick'

({:: -: map) t
({:: -: map) cat L:0 t

(< S: 0 -: < S 0) t
(< S: 0 -: < S 0) ,~^:5 t
(< S: 1 -: < S 1) {:: t
(< S: 1 -: < S 1) {:: cat L: 0 t

(< S: 0 t) -: (< S: 1 {::t){::&.><t 
(<@": S: 0 t) -: (;:5!:5 <'pick') -. ;:'()'


4!:55 ;:'boxed cat map mapp pick S sc spread_temp si sz t'


