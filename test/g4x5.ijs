NB. 4!:5 ----------------------------------------------------------------

nch     =: 4!:5
nch_bar_=: 4!:5
erase   =: 4!:55

1 [ nch 1
abc=: 1 2 3
foo_bar_=:3 4 5
foo=:'in base locale'
boo__=:12
(t=:nch 1) -: ;:'abc_base_ boo_base_ foo_bar_ foo_base_'

abc=: 1 2 3
foo_bar_=:3 4 5
foo=:'in base locale'
boo__=:12
(t=:nch_bar_ 1) -: ;:'abc_base_ boo_base_ foo_bar_ foo_base_ t_base_'

abc=: 1 2 3
abc=: 'xyz'
(t=:nch 1) -: ;:'abc_base_ t_base_'

1 [ ".('a',.>":&.>i.30),"1 '=:123'
erase 'a',&.>":&.>i.5
(t=:nch 1) -: /:~ ,&'_base_'&.> (<,'t'),'a',&.>":&.>5+i.25
erase 'a',&.>":&.>5+i.25

nch 0
abc=: 1 2 3
nch 1
foo_bar_=:3 4 5
boo__=:12
(t=:nch 1) -: ;:'boo_base_ foo_bar_'

f=: 3 : 0
 abc=: 6 49
 i=.1+y
 foo_bar_=: 'hope to do good'
 boo=.19
 1
)

f 12
f 15
(t=:nch 1) -: ;:'abc_base_ f_base_ foo_bar_ t_base_'

'domain error' -: nch etx 34
'domain error' -: nch etx 3.4
'domain error' -: nch etx 3j4
'domain error' -: nch etx 'a'        
'domain error' -: nch etx <1

'rank error'   -: nch etx 1 0 1
'rank error'   -: nch etx ''

nch 0

18!:55 ;:'bar'

4!:55 ;:'abc boo erase f foo nch t'


