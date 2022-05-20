prolog './gpick.ijs'
NB. {:: -----------------------------------------------------------------

boxed=: (32&=)@(3!:0)
cat  =: { @: (i.&.>) @: $

pick=: >@({&>/)@(<"0@|.@[ , <@]) " 1 _

map =: a:&mapp
mapp=: 4 : 'if. boxed y do. (<"0 x,&.><"0 cat y) mapp&.> y else. >x end.'

sz  =: 3 : '>spread_temp'
si  =: 3 : 'y [ spread_temp=:$0'
sc  =: 3 : '0: spread_temp=:spread_temp,<y'
S   =: 2 : 'sz@(sc@u L: v)&si'

t=: 5!:2 <'pick'

({:: -: map) t
({:: -: map) cat L:0 t

(< S: 0 -: < S 0) t
(< S: 0 -: < S 0) ,~^:5 t
(< S: 1 -: < S 1) {:: t
(< S: 1 -: < S 1) {:: cat L: 0 t

(< S: 0 t) -: (< S: 1 {::t){::&.><t 
(<@": S: 0 t) -: (;:5!:5 <'pick') -. ;:'()'

(2 2 $ 3 4   2 0) -: (,. 2 1) {:: 'a';2;3 4

'index error'  -: 2 {:: etx 'a';'b'
'index error'  -: _3 {:: etx 'a';'b'
'index error'  -: (<2) {:: etx 'a';'b'
'index error'  -: (<_3) {:: etx 'a';'b'
t =: 'a';'b'
'index error'  -: 2 {:: etx t
'index error'  -: _3 {:: etx t
'index error'  -: (<2) {:: etx t
'index error'  -: (<_3) {:: etx t
t =: 3;4
6 = 3 + 0 {:: t
t -: 3;4   NB. Verify the value fetched from t was non-inplaceable
(3;4) -: ([  4 + 0&{::) > < 3 ; 4   NB. Value fetched from box is non-inplaceable
'Note asdfasdfs' -: ]^:('NB.' -: 3 {. _1 {:: ])&.;: 'Note  asdfasdfs'
'Noteasdfasdfs' -: ; (] [ 3 {. _1 {:: ]) ;: 'Note  asdfasdfs'

6 -: 0 {:: <6
6 7 -: 0 {:: <6 7
'index error' -: 1 {:: etx <6
'index error' -: 1 {:: etx 6
'length error' -: (,0) {:: etx <6
8 -: 0 {:: 8
6 -: 0 {:: 6 7
7 -: 1 {:: 6 7
'a' -: 0 {:: 'ab'
'b' -: 1 {:: 'ab'
(0;1) -: 0 {:: <"0 i. 2 2
(0;1) -: (,0) {:: <"0 i. 2 2
'a' -: (0;0) {:: <"0 'abc'
'a' -: (0;0) {:: <@,"0 'abc'

4!:55 ;:'boxed cat map mapp pick S sc spread_temp si sz t'



epilog''

