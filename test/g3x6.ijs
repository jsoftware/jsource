prolog './g3x6.ijs'
NB. 3!:6 ----------------------------------------------------------------

f=. <jpath'~temp/test.ijl'
d=.'abc=:i.5'
(3!:6 d) 1!:2 f
-.d-:1!:1 f
abc=: 5
0!:0 f
abc-:i.5
1!:55 f

(3!:6 'sq=: *:',LF,'mean=: +/ % #') 1!:2 f
0!:0 f

(mean -: +/ % #) x=: 100 ?@$ 0
(sq   -: *:) x

sq1=: *:
'*:' -: 5!:5 <'sq1'

(5!:1 <'mean') -:  <'mean'
(5!:2 <'mean') -: ,<'mean'
(5!:4 <'mean') -: ,:(2${:9!:6 ''),' mean'
(5!:5 <'mean') -: 'mean'
(5!:6 <'mean') -: 'mean'

m1=: mean f.
(5!:5 <'m1') -: 'mean'

m1=: 'mean' f.
(5!:5 <'m1') -: 'mean'

'domain error' -: 15!:6 etx <'mean'

1!:55 f

NB. Test locking non-file data
0!:1 (3!:6 'sq=: *:',LF,'mean=: +/ % #')
(mean -: +/ % #) x=: 100 ?@$ 0
(sq   -: *:) x

sq1=: *:
'*:' -: 5!:5 <'sq1'

(5!:1 <'mean') -:  <'mean'
(5!:2 <'mean') -: ,<'mean'
(5!:4 <'mean') -: ,:(2${:9!:6 ''),' mean'
(5!:5 <'mean') -: 'mean'
(5!:6 <'mean') -: 'mean'

m1=: mean f.
(5!:5 <'m1') -: 'mean'

m1=: 'mean' f.
(5!:5 <'m1') -: 'mean'

'domain error' -: 15!:6 etx <'mean'



4!:55 ;:'abc d f m1 mean sq sq1 x'



epilog''

