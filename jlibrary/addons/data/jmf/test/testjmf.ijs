NB. test jmf (self-describing) files

dbg 1
dbstops''
unmapall_jmf_''
F=: jpath '~temp/t1.dat'

NB. allow for integer of length 20
testjmf=: 3 : 0
size=. 20*IF64{4 8
createjmf_jmf_ F;size
map_jmf_ 'abc';F
abc=: i.20
unmap_jmf_ 'abc'
map_jmf_ 'def';F
smoutput def -: i.20
abc=: i.21 NB. OK
try. def=: i.21
catch. smoutput 'expected allocation error: def=: i.21' end.
NB. dbstopme''
unmap_jmf_ 'def';30*IF64{4 8
map_jmf_ 'def';F
smoutput def -: i.20
def=: i.30
try. def=: i.31
catch. smoutput 'expected allocation error: def=: i.31' end.
)

testjmf''


