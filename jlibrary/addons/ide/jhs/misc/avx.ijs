NB. j64-805
NB.  load avx.ijs
NB.  run'' - save results in ~temp/avxrun.dat
NB. j64avx-806
NB.  load avx.ijs
NB.  run'' - save results in ~temp/avxrun.dat
NB.  rep'' - report ~temp/avxrun.dat vs j805-user-temp/avxrun.dat
NB.  rep p - report ~temp/avxrun.dat vs p,'/temp/avxrun.dat' 

0 : 0
improvements to i. automatically apply to
x i. y
x i: y
x e. y
~. y
~: y
I.@:~: y
x -. y
x (e. i. 0:) y
x (e. i. 1:) y
x (e. i: 0:) y
x (e. i: 1:) y
x +/@:e. y
x +./@:e. y
x *./@:e. y
x I.@:e. y
)

version=: 1 NB. compare only same version
types=: ;:'intsr intbr char float float0'
counts=: (UNAME-:'Android'){:: ('1e7 1e3';'1e5 1e3') ,&< ('1e6 1e3';'1e5 1e3')
mtimes=: 3 NB. multiple times to run a test
NB. mtimes=: 1 NB.!

getproctype=: 3 : 0
select. UNAME
case. 'Linux' do.
 t=. <;._2 shell'cat /proc/cpuinfo'
 13}.;t{~(13{.each t)i.<'model name',TAB,': '
case. 'Win' do.
 dltb;1{<;._2 CR-.~shell'wmic cpu get name'
case. 'Darwin' do.
 shell'sysctl -n machdep.cpu.brand_string'
case. 'Android' do.
 LF-.~ 2!:0 'getprop ro.product.model'
end. 
)

set=: 4 : 0
erase'a b'
'C c'=. 0".y
select. ;x
case. 'intsr' do. b=: (<.-:#a)+ c ?. c [ a=: C ?. C
case. 'intbr' do. b=: (c?.#a){a [ a=: C ?. @$ <:2^63
case. 'char'  do. b=: (c?.#a){a [ a=: >,.~":each <"0 [C ?. @$ <:2^63
case. 'float';'float0' do. b=: (c?.#a){a [ a=: 0.1+C ?. @$ <:2^63
case. 'mmint'     do. b=: a=:     ?(C,C)$1000
case. 'mmfloat'   do. b=: a=: 0.1+a['mmint'set y
case. 'mmcomplex' do. b=: a=: 1j1+a['mmint'set y
end.
i.0 0
)

tests=: 0 : 0
a i. a
a i. b
b i. a
a e. b
b e. a
a (+/@:e.) b
a (e. i. 1:) b 
~.a
~:a
/:a
/:~a
)

tests_float=: tests_char=: tests_intbr=: tests_intsr =: tests
tests_float0=: tests rplc 'i.';'(i.!.0)';'e.';'(e.!.0)'

NB. small timings can be greatly affected by hiccups
NB. take <./ multiple timings - least hiccups
rtime=: 3 : 0
<./;timex each mtimes$<y
)

runsub=: 3 : 0
r=. i. 0,#types
for_t. types do.
 t set y
 r=. r,;rtime each <;._2 ('tests_',;t)~
end.
|:r
)

run=: 3 : 0
r=. 1 1$<version
r=. r,<9!:14''
r=. r,<getproctype''
r=. r,<<;._2 tests
r=. r,<counts
r=. r,<runsub ;0{counts
r=. r,<runsub ;1{counts
'mmint'set '1e3'
d=. rtime 'a +/ . * b'
'mmfloat'set '1e3'
d=. d,rtime 'a +/ . * b'
'mmcomplex'set '1e3'
r=. r,<d,rtime 'a +/ . * b'
(3!:1 r)fwrite '~temp/avxrun.dat'
i.0 0
)

rep=: 3 : 0
y=. ;(y-:''){y;'j64-805-user'
old=: 3!:2 fread y,'/temp/avxrun.dat'
new=: 3!:2 fread '~temp/avxrun.dat'
'versions differ'  assert (0{old)=0{new
'processors differ'assert (2{old)=2{new
'tests differ'     assert  (3{old)=3{new
'counts differ'    assert  (4{old)=4{new
r=. (":5{.6!:0''),LF,(;2{old),LF,(;1{old),LF,;1{new
d=. (".each}:(<'datas_'),each types),.(<' NB. '),each }:types
r=. r,LF,'intsr (small range) special code avoids hash - intbr (big range)'
r=. r,LF,'float0 tests use !.0 where appropriate'
r=. r,LF,'N in tables below indicate avx JE runs N times faster than 805',LF
r=. r,LF,'   ''type'' set ',":;0{counts
a=. ;5{new
b=. ;5{old
d=. 5j1":each<"0 >b%each a
d=. d,.<;._2 tests
if. IFJHS do.
r=. r,LF,seebox_jhs_ (types,<'test'),d
else.
r=. r,LF, , ,&LF"1 ~.":(types,<'test'),d
end.
r=. r,LF,'   ''type'' set ',":;1{counts
a=. ;6{new
b=. ;6{old
d=. 4j1":each<"0 >b%each a
d=. d,.<;._2 tests
if. IFJHS do.
r=. r,LF,seebox_jhs_ (types,<'test'),d
else.
r=. r,LF, , ,&LF"1 ~.":(types,<'test'),d
end.
r=. r,LF,'   ''type'' set ''1e3'''
a=. ;7{new
b=. ;7{old
d=. ;:'mmint mmfloat mmcomplex test'
d=. d,((0j1":each<"0 b%a),(<'a +/ . * b')),:(0j1":each<"0 (2 2 6)*(1e3^3)%a*1e9),<'GFlops'
if. IFJHS do.
r=. r,LF,seebox_jhs_ d
else.
r=. r,LF, , ,&LF"1 ~.":d
end.
)
