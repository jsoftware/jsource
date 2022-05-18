prolog './gmbx.ijs'
NB. mapped boxed arrays -------------------------------------------------

load 'jmf'
18!:4 <'base'

1 [ unmap_jmf_ 'q'
f=: <jpath'~temp/q.jmf'
1 [ createjmf_jmf_ f,<4e6      NB. 4e6 bytes for data
map_jmf_ (<'q'),f,'';0         NB. map q to jmf file
'' -: q

1 [ unmap_jmf_ 'r'
f1=: <jpath'~temp/r.jmf'
1 [ createjmf_jmf_ f1,<7e5 
map_jmf_ (<'r'),f1,'';0
'' -: r

g   =: 6 7 4 5 2 3 0 1&{"1 @: (5&}.) @: (3!:3) @: ({."1) 
mean=: +/ % #


NB. do not erase names
NB. 4!:55 ;:'f f1 g mean q r'

