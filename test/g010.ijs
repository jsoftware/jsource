prolog './g010.ijs'
NB. < y -----------------------------------------------------------------

test =: 3 : '(0=#$t), (-.t-:y),  y-:>t=.<y'

test ?10$2
test 'a'
test u:'a'
test 10&u:'a'
test s:@<"0 'a'
test ?2 3 4$1000
test o.?2 3 4$1000
test j.?2 3 4$1000
test <123;123

test i.0
test i.0 2 3

IGNOREIFFVI 2000 > 7!:2 '<x' [ x=: i.1e5
('a';'b') -: [&.>@:(<"0) 'ab'  NB. crashed when virtuals flagged as PRISTINE


NB. x<y -----------------------------------------------------------------

(14 11#1 0) -: 1< 1+ 10^-i.25

NB. Boolean
0 1 0 0 -: 0 0 1 1 < 0 1 0 1
*./ 0 < 1 1 1
*./ 0 0 0 < 1

NB. integer
*./, (x-1) < x=: ?20$10000123

NB. floating point
*./, (x-1) < x=: o.?3 4$10000

NB. symbol
*./, (s:@<"0 }:a.) < s:@<"0 }.a.

'domain error' -: 'abc'  < etx 3 4 5
'domain error' -: 'abc'  <~etx 3 4 5 
'domain error' -: 'ab'   < etx 'cd'  
'domain error' -: (u:'abc')  < etx 3 4 5
'domain error' -: (u:'abc')  <~etx 3 4 5 
'domain error' -: (u:'ab')   < etx 'cd'  
'domain error' -: (10&u:'abc')  < etx 3 4 5
'domain error' -: (10&u:'abc')  <~etx 3 4 5 
'domain error' -: (10&u:'ab')   < etx 'cd'  
'domain error' -: (s:@<"0 'abc')  < etx 3 4 5
'domain error' -: (s:@<"0 'abc')  <~etx 3 4 5 
'domain error' -: (s:@<"0 'ab')   < etx 'cd'  
'domain error' -: 3      < etx <3 4   
'domain error' -: 3      <~etx <3 4  
'domain error' -: 3.4    < etx 1 2 3j4 
'domain error' -: 3.4    <~etx 1 2 3j4 
'domain error' -: 3j4    < etx 1 2 3j4 
'domain error' -: 'abc'  < etx 1 2 3j4
'domain error' -: 'abc'  <~etx 1 2 3j4 
'domain error' -: (u:'abc')  < etx 1 2 3j4
'domain error' -: (u:'abc')  <~etx 1 2 3j4 
'domain error' -: (10&u:'abc')  < etx 1 2 3j4
'domain error' -: (10&u:'abc')  <~etx 1 2 3j4 
'domain error' -: (s:@<"0 'abc')  < etx 1 2 3j4
'domain error' -: (s:@<"0 'abc')  <~etx 1 2 3j4 

'length error' -: 3 4    < etx 5 6 7  
'length error' -: 3 4    <~etx 5 6 7        
'length error' -: (i.3 4)< etx i.5 4   
'length error' -: (i.3 4)<~etx i.5 4




epilog''

