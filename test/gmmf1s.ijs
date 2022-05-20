prolog './gmmf1s.ijs'
NB. memory mapped files -------------------------------------------------

t=: 18!:4
1 [ t <'base' [ load 'jmf'
3 : 0 ''
if. _1=nc<'MAPNAME_jmf_' do.
 'MAPNAME_jmf_ MAPFN_jmf_ MAPSN_jmf_ MAPFH_jmf_ MAPMH_jmf_ MAPADDRESS_jmf_ MAPHEADER_jmf_ MAPFSIZE_jmf_ MAPMSIZE_jmf_ MAPREFS_jmf_'=: i.10
end. 
1
) 
1 [ unmapall_jmf_ ''

JSB_z_=: 65536
fdir  =: 1!:0
fread =: 1!:1
fwrite=: 1!:2
ferase=: 1!:55

f0 =: <jpath '~temp/t.txt'

t=: s:' testing testing 1 2 3'  
((IF64{2 3) ic 6&s: t) fwrite f0        NB. write symbol index
t -: _6&s: (IF64{_2 _3) ic fread f0

JSB map_jmf_ 'abc';f0     NB. map abc to file
abc -: t
abc=: |.abc
abc -: |. t

NB.! ebi 'file access error' -: 6&u: fread etx f0

0 -: unmap_jmf_ 'abc'

(|.t) -: _6&s: (IF64{_2 _3) ic fread f0

t=: s:' testing testing 1 2 3'  
((IF64{2 3) ic 6&s: t) fwrite f0
t -: _6&s: (IF64{_2 _3) ic fread f0
JSB map_jmf_ 'abc';f0     NB. map abc to file
abc -: t
2 = >(<(({."1 t) i. <'abc_base_');MAPREFS_jmf_){t=: showmap_jmf_ ''
4!:55 ;:'abc'
1 = >(<(({."1 t) i. <'abc_base_');MAPREFS_jmf_){t=: showmap_jmf_ ''
0 -: unmap_jmf_ 'abc'
-. (<'abc_base_') e. {."1 t=: showmap_jmf_ ''

ferase f0

f=: <jpath '~temp/jdata.jmf'
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

jdata=: (IF64{250 125)$s:<"0 'abcd'
'allocation error' -: ex 'jdata=: (>:IF64{250 125)$s:<"0 ''zxcv'''  NB. too much data for file

jdata=: 3 3$s:<"0 'abcd'
jdata=: (s:<'x') 1} jdata      NB. amend in-place
jdata -: 3 3$s:<"0'abcxxxcda'

jdata=: i. 2 3
jdata=: jdata,0
3 = #jdata
jdata=: 23 (_1)} jdata 
jdata -: (i.2 3),23

t=: showmap_jmf_''          NB. mapping information

((<1;0){t) = <'jdata_base_'
((<1;1){t) = f
((<1;MAPMSIZE_jmf_){t) = <,1000

0 -: unmap_jmf_ 'jdata'     NB. 0 result is success
1 -: # showmap_jmf_ ''

x=: 10?1e6
map_jmf_ 'jdata';f
jdata=: x
abc=: jdata
abc -: x
3 = >(<(({."1 t) i. <'jdata_base_'),MAPREFS_jmf_){t=: showmap_jmf_ 'jdata'
2 -: unmap_jmf_ 'jdata'
2 = >(<(({."1 t) i. <'jdata_base_'),MAPREFS_jmf_){t=: showmap_jmf_ 'jdata'
4!:55 ;:'abc'
1 = >(<(({."1 t) i. <'jdata_base_'),MAPREFS_jmf_){t=: showmap_jmf_ 'jdata'
0 -: unmap_jmf_ 'jdata'

map_jmf_ 'jdata'; (>f); ''; 1  NB. read-only
x -: jdata
'read-only data' -: ex 'jdata=: 1 2 3'
0 -: unmap_jmf_ 'jdata'
map_jmf_ 'jdata';f
jdata -: x                NB. original file unchanged
0 -: unmap_jmf_ 'jdata'

map_jmf_ 'jdata'; (>f); ''; 2  NB. copy-on-write
x -: jdata
jdata=: 1 2 3
1 2 3 -: jdata            NB. copy-on-write
0 -: unmap_jmf_ 'jdata'
map_jmf_ 'jdata';f
jdata -: x                NB. original file unchanged
0 -: unmap_jmf_ 'jdata'

x=: s:' Professors in New England guard the glory that was Greece'
((IF64{2 3) ic 6&s: x) fwrite f
JSB map_jmf_ 'jdata';f
jdata -: x
0 -: unmap_jmf_ 'jdata'
JINT map_jmf_ 'jdata';f
4 -: 3!:0 jdata
0 -: unmap_jmf_ 'jdata'

(JSB;2 5) map_jmf_ 'jdata';f
jdata -: ((<.(#x)%10),2 5)$x
0 -: unmap_jmf_ 'jdata'

ferase f

18!:55 <'jmf'


4!:55 ;:'f f0 fdir ferase fread fwrite i jdata t x '



epilog''

