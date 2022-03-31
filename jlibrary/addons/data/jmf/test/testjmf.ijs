NB. test jmf (self-describing) files

dbg 1
dbstops''
F=: jpath '~temp/t1.dat'

NB. allow for integer of length 20
testjmf=: 3 : 0
unmapall_jmf_''
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

NB. jmf and nonjmf files * maptype default, RO, COW
testmaptype=: 3 : 0
echo 'testmaptype'
unmapall_jmf_''

NB. jmf file with maptype 0,1,2
createjmf_jmf_ F;10
map_jmf_ 'abc';F;'';MTRW_jmf_
abc=:'def'
unmap_jmf_'abc'

NB. test jmf file with 0/1/2 map types

map_jmf_ 'abc';F;'';MTRW_jmf_ NB. 0 maptype
assert 'def'-:abc
abc=: 'ghi'
m=. mappings
remap_jmf_'abc'
assert m-:mappings
unmap_jmf_'abc'

map_jmf_ 'abc';F;'';MTRO_jmf_ NB. RO maptype
assert 'ghi'-:abc
try. abc=: 'jkl' catch. end.
assert 'ghi'-:abc
m=. mappings
remap_jmf_'abc'
assert m-:mappings
unmap_jmf_'abc'

map_jmf_ 'abc';F;'';MTCW_jmf_ NB. COW (copy on write) maptype
assert 'ghi'-:abc
abc=: 'jkl'
assert abc-:'jkl'
unmap_jmf_'abc'
map_jmf_ 'abc';F;'';MTRO_jmf_
assert 'ghi'-:abc NB. verify =: did not change file
m=. mappings
remap_jmf_'abc'
assert m-:mappings
unmap_jmf_'abc'

NB. test nonjmf file with 0/1/2 maptypes

'1234'fwrite F

JCHAR map_jmf_ 'abc';F;'';0 NB. 0 maptype
assert '1234'-:abc
abc=: '6789'
unmap_jmf_'abc'
JCHAR map_jmf_ 'abc';F;'';0
assert '6789'-:abc
m=. mappings
remap_jmf_'abc'
assert m-:mappings
unmap_jmf_'abc'

JCHAR map_jmf_ 'abc';F;'';1 NB. RO
assert '6789'-:abc
try. abc=: '9999' catch. end.
assert '6789'-:abc
m=. mappings
remap_jmf_'abc'
assert m-:mappings
unmap_jmf_'abc'

JCHAR map_jmf_ 'abc';F;'';2 NB. COW (copy on write) maptype
assert '6789'-:abc
abc=: 'jkl'
assert abc-:'jkl'
unmap_jmf_'abc'
JCHAR map_jmf_ 'abc';F;'';0
assert '6789'-:abc NB. verify =: did not change file
m=. mappings
remap_jmf_'abc'
assert m-:mappings
unmap_jmf_'abc'
echo 'testmaptype end'
i.0 0
)

testmaptype''
