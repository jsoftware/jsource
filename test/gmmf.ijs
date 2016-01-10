NB. memory mapped files -------------------------------------------------

1 [ 18!:4 <'base' [ load 'jmf'
1 [ unmapall_jmf_ ''

fdir  =: 1!:0
fread =: 1!:1
fwrite=: 1!:2
ferase=: 1!:55

f0 =: <jpath '~temp\t.txt'

t=: 'testing testing 1 2 3'  
t fwrite f0
t -: fread f0

JCHAR map_jmf_ 'abc';f0     NB. map abc to file characters
abc -: t
abc=: |.abc
abc -: |. t

NB.! ebi 'file access error' -: fread etx f0

0 -: unmap_jmf_ 'abc'

(|.t) -: fread f0

t=: 'testing testing 1 2 3'  
t fwrite f0
t -: fread f0
JCHAR map_jmf_ 'abc';f0     NB. map abc to file characters
abc -: t
2 = >(<(({."1 t) i. <'abc_base_');9){t=: showmap_jmf_ ''
4!:55 ;:'abc'
1 = >(<(({."1 t) i. <'abc_base_');9){t=: showmap_jmf_ ''
0 -: unmap_jmf_ 'abc'
-. (<'abc_base_') e. {."1 t=: showmap_jmf_ ''

ferase f0

f=: <jpath '~temp\jdata.jmf'
1 [ createjmf_jmf_ f,<1000  NB. 1000 bytes for data
t=: fdir f
1 = #t
+./ ({.{.t) E.&> f

map_jmf_ 'jdata';f          NB. map jdata to jmf file
'' -: jdata
0 -: 4!:0 <'jdata'

t=: ?2 20$1e6
jdata=: t
t -: jdata
0 -: unmap_jmf_ 'jdata'
_1 = 4!:0 <'jdata'

map_jmf_ 'jdata';f
0 -: 4!:0 <'jdata'
t -: jdata

jdata=: 1000$'abcd'
'allocation error' -: ex 'jdata=: 1001$''zxcv'''  NB. too much data for file

jdata=: 3 3$'abcd'
jdata=: 'xxx' 1} jdata      NB. amend in-place
jdata -: 3 3$'abcxxxcda'

jdata=: i. 2 3
additem_jmf_ 'jdata'
3 = #jdata
jdata=: 23 (_1)} jdata 
jdata -: (i.2 3),23

t=: showmap_jmf_''          NB. mapping information

((<1;0){t) = <'jdata_base_'
((<1;1){t) = f
((<1;8){t) = <,1000

0 -: unmap_jmf_ 'jdata'     NB. 0 result is success
1 -: # showmap_jmf_ ''

x=: 10?1e6
map_jmf_ 'jdata';f
jdata=: x
abc=: jdata
abc -: x
3 = >(<(({."1 t) i. <'jdata_base_'),9){t=: showmap_jmf_ 'jdata'
2 -: unmap_jmf_ 'jdata'
2 = >(<(({."1 t) i. <'jdata_base_'),9){t=: showmap_jmf_ 'jdata'
4!:55 ;:'abc'
1 = >(<(({."1 t) i. <'jdata_base_'),9){t=: showmap_jmf_ 'jdata'
0 -: unmap_jmf_ 'jdata'

map_jmf_ 'jdata'; (>f); ''; 1  NB. read-only
x -: jdata
'read-only data' -: ex 'jdata=: 1 2 3'
0 -: unmap_jmf_ 'jdata'

x=: 'Professors in New England guard the glory that was Greece'
x fwrite f
JCHAR map_jmf_ 'jdata';f
jdata -: x
0 -: unmap_jmf_ 'jdata'
JINT map_jmf_ 'jdata';f
4 -: 3!:0 jdata
0 -: unmap_jmf_ 'jdata'

(JCHAR;2 5) map_jmf_ 'jdata';f
jdata -: ((<.(#x)%10),2 5)$x
0 -: unmap_jmf_ 'jdata'

ferase f

18!:55 <'jmf'


4!:55 ;:'f f0 fdir ferase fread fwrite i jdata t x '


