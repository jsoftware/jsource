NB. test script utilities -----------------------------------------------

cocurrent 'base'
9!:19[2^_44   NB. default but some tests require a larger ct
NB. set NORESETSTABLE to 1 to prevent restoring symbol table between files

NB. settings to change when compiling the JE in a debug mode
NB. If FORCEVIRTUALINPUTS is set, space consumption changes.  Set IGNOREIFFVI to 1: in that case
IGNOREIFFVI_z_ =: ]

3 : 0 ''
testpath=: '/',~(t i:'/'){.t=. jpath;(4!:4<'ddall'){4!:3''
if. IFWIN do.
 n=. 'tsdll.dll'
else.
 n=. 'libtsdll.',;(UNAME-:'Darwin'){'so';'dylib'
end. 
if. IFJA *. UNAME-:'Android' do.
 LIBTSDLL=: (({.~ i:&'/') LIBFILE),'/',n,' '
else.
 LIBTSDLL=: jpath'~bin/',n,' '
end.
1
)

testfiles=: 3 : 0   NB. y. is prefix - e.g., 'g' or 'gm' or 'gs'
 testpath&,&.> /:~ {."1 [1!:0 testpath,y,'*.ijs'
)

NB. black list
NB. gmbx.ijs is not an independent test
NB. gfft and glapack - run separately with additional addons
blacklist=: ((<testpath),each 'gmbx.ijs';'gfft.ijs';'glapack.ijs'),testfiles 'gmbx'  NB. mapped boxed arrays no longer supported
blacklist=: blacklist, (<testpath),each <'gregex.ijs' NB. require libjpcre2 binary
blacklist=: blacklist, (-.IF64)#(<testpath),each <'g6x14.ijs' NB. require 64-bit
blacklist=: blacklist, (IFRASPI+.UNAME-:'Android')#(<testpath),each 'g13x.ijs';'gstack.ijs'
blacklist=: blacklist, (1=9!:56'maxtasks')#(<testpath),each 'gtdot.ijs';'gtdot1.ijs';'gtdot2.ijs' NB. require multithreading
NB. OK now blacklist=: blacklist, (<testpath),each <'gcip.ijs'    NB. blacklist until fixed to allow other tests running

ddall    =: blacklist -.~ testfiles 'g'
ddgmbx   =: blacklist -.~ testfiles 'gmbx'    NB. map boxed arrays
ddgsp    =: blacklist -.~ testfiles 'gsp'     NB. sparse arrays
ddgsc    =: blacklist -.~ testfiles 'gsc'     NB. symbol arrays
ddg      =: ddall -. ddgmbx,ddgsp             NB. "ordinary"
ddgxsp   =: ddall -. ddgsp                    NB. except sparse arrays
ddtime   =: (<testpath),each <;._2[ 0 : 0     NB. timing tests
g220t.ijs
g300t.ijs
g320ipt.ijs
g321t.ijs
g330t.ijs
g420t.ijs
g530t.ijs
gft.ijs
gibst.ijs
gif.ijs
gipht.ijs
git.ijs
gss.ijs
)

NB. When a name executes cocurrent, all subsequent calls from that name use a slower linkage.  Thus we don't want RUN to call
NB. 0!:x directly, because then all the calls in RUN (after a file that does cocurrent) use slow linkage.  Interpose a name
ex01=:0!:1
ex02=:0!:2
ex03=:0!:3

etx      =: 1 : 'u :: (<:@(13!:11)@i.@0: >@{ 9!:8@i.@0:)'  NB. error message from error number
ex       =: ". etx
fex      =: }. @ (i.&(10{a.) {. ]) @ (13!:12) @ i. @ 0: @ (0!:110)
eftx     =: 1 : 'u :: ((10{a.) -.~ (13!:12) @ i. @ 0:)'   NB. full text of error message
efx      =: ". eftx

NB. prolog is run after the optional typing of testcase name.  y is './testcasename.ijs'
prolog=: {{ 1: (dbr bind Debug)@:(9!:19)2^_44[echo^:ECHOFILENAME y[RUNTIME=:6!:1'' }}
NB. epilog'' is run as the last line of each testcase
epilog=: {{ 1: echo^:ECHOFILENAME 'time(sec): ',(":RUNTIME-~6!:1''),'  memory used: ',":(7!:1,7!:7)'' }}

THRESHOLD=: 0 NB. allow timing tests to trigger failure 
THRESHOLD=: 1 NB. force timing tests to pass

threshold=: 0.75 NB. tighter timing tests
threshold=: 0.2  NB. timing tests

timer    =: 6!:2

type     =: 3!:0
imax     =: IF64{:: 2147483647; 9223372036854775807
imin     =: (-imax)-1
U4MAX    =: 16b110000
C4MAX    =: IF64{:: 2147483647;4294967296
RAND32   =: (] [`(<:@-@[)@.]"0 ?@:($&2)@:$)^:(-.IF64) NB. negate and decrement randomly
UNSGN32  =: <.@:((2^32)&|)^:(-.IF64)                  NB. unsigned 32-bit to double

scheck=: 3 : 0  NB. check sparse array
 s=. $ y
 a=. 2 $. y
 e=. 3 $. y
 i=. 4 $. y
 x=. 5 $. y

 assert. 1 = #$s                      NB. 0
 assert. s -: <.s                     NB. 1
 assert. imax >: #s                   NB. 2
 assert. *./ (0 <: s) *. s <: imax    NB. 3
 assert. _ > */s                      NB. 4

 assert. 1 = #$a                      NB. 5
 assert. *./ a e. i.#s                NB. 6
 assert. a -: ~.a                     NB. 7

 assert. 0 = #$e                      NB. 8
 assert. (type e) = <. 0.001*type y   NB. 9
 assert. (type e) = type x            NB. 10

 assert. 2 = #$i                      NB. 11
 assert. i -: <.i                     NB. 12
 assert. imax >: #i                   NB. 13
 assert. (#i) = #x                    NB. 14
 assert. ({:$i) = #a                  NB. 15
 assert. *./, (0<:i) *. i<"1 a{s      NB. 16
 assert. i -: ~. i                    NB. 17
 assert. i -: /:~i                    NB. 18

 assert. (#$x) = 1+(#s)-#a            NB. 19
 assert. (}.$x) -: ((i.#s)-.a){s      NB. 20
 1
)

comb=: 4 : 0
 c=. 1 {.~ - d=. 1+y-x
 z=. i.1 0
 for_j. (d-1+y)+/&i.d do. z=. (c#j) ,. z{~;(-c){.&.><i.{.c=. +/\.c end.
)

randuni=: 3 : 0
 initsymbolstate =: 0 s:^:(0 = {. ". 'NORESETSTABLE') 10  NB. Preserve init symbol state so it doesn't keep growing
 l2max=. 1024       NB. #literal2 sample   multiple of 256
 l4max=. 1024       NB. #literal4 sample   multiple of 256
 sbmax=. 1024       NB. #symbol sample     multiple of 256
 sblen=. 10         NB. max symbol length
NB. unique random literal2
 adot1=: u: /:~ l2max?65536
NB. unique random literal4
 if. IF64 do.
  adot2=: 10&u: /:~ l4max?C4MAX
 else.
  adot2=: /:~ 10&u: RAND32 l4max?C4MAX
 end.
NB. unique random symbols
 a=.((sblen#1),~1+?(256-sblen)#sblen) [ b=. /:~ 256?256
 s0=. a (] s:@:<@({&a.)@:+ i.@[)"0 b
 a=.(1+?((65536-sblen)<.65536<.sbmax)#sblen) [ b=. /:~ 128 + ((65536-sblen)<.65536<.sbmax)?(65536-128)-sblen
 s1=. a (] s:@:<@u:@:+ i.@[)"0 b
 a=.(1+?sbmax#sblen) [ b=. RAND32 /:~ 65536 + ((C4MAX-sblen)<.sbmax)?(C4MAX-65536)-sblen
 s2=. /:~^:(-.IF64) a (] s:@:<@(10&u:)@:+ i.@[)"0 b
 s=. ~.(?~#ss){ss=. s0,s1,s2
 sdot0=: /:~ (sbmax?#s){s
NB. validation
 assert. l2max=#adot1
 assert. l4max=#adot2
 assert. sbmax=#sdot0
 assert. 256<:#adot1
 assert. 256<:#adot2
 assert. 256<:#sdot0
 assert. 0=256|#adot1
 assert. 0=256|#adot2
 assert. 0=256|#sdot0
 assert. l2max=#~.adot1
 assert. l4max=#~.adot2
 assert. sbmax=#~.sdot0
 ''
)
randfini =: 3 : 0
if. 32 = 3!:0 initsymbolstate do. 10 s: initsymbolstate end.  NB. Reset symbol table if it was saved
4!:55 <'initsymbolstate'
)

NB. ensure ~temp is unique when running multiple instances
pidtemp=: 3 : 0
t=. (<'temp') i.~ {."1 SystemFolders_j_
u=. (<'user') i.~ {."1 SystemFolders_j_
1!:5 ::1: tmp=. < (>(<u,1){SystemFolders_j_),'/temp-',":2!:6''
SystemFolders_j_=: tmp (<t,1)}SystemFolders_j_
''
)

NB. comparisons
neareq =: = +. *.&(0 = *!.1e_12)  NB. tolerant comparison, even against 0
nearmt =: *./@,@:neareq
Neareq =: (1e_10 >(|@- % ])) +. *.&(0 = *!.1e_12)  NB. big tolerant comparison, even against 0
Nearmt =: *./@,@:Neareq

NB. ebi extensions

RSET=: 4 : '(x)=: y'
RBAD=: 3 : '>_4}.each(#testpath)}.each(-.RB)#RF'
RUN=: RBAD@('RB'&RSET)@(ex03`(0!:3)@.(*@".@'Debug'))@('RF'&RSET)
RUND=: RBAD@('RB'&RSET)@(ex02`(0!:2)@.(*@".@'Debug'))@('RF'&RSET)  NB. Run w/display

RUN1=: 13 : 'ex02`(0!:2)@.(*@".@''Debug'') <testpath,y,''.ijs'''

RESUB1=: 3 : 'y[echo >y'
RESUB2=: (13 : '-.ex03 RESUB1 y')"0
RECHO=: 13 : '+/ RESUB2 y'

NB. bill extensions

ECHOFILENAME=: 0   NB. echo file name
RUNTIME=: 0        NB. time for running each test script
Debug=: 0
QKTEST=: IFIOS+.IFRASPI+.UNAME-:'Android'  NB. run quick test

RUND1=: 4 : 0
x123=. x>.1
y123=. y
d123=. Debug
assert. (<'base')-:18!:5''
4!:55 ;:'x y oldnl RLAST'
assert. 0=(;:'x y oldnl RLAST') e. nl''
oldnl=: nl''
for_y234. y123 do.
 echo RLAST=: >y234
 for. i.x123 do.
  Debug=: 0
  0!:2 y234
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
  assert. (<'base')-:18!:5''
  assert. ((;:'oldnl y234 RLAST')-.~nl'') -: oldnl
  Debug=: 1
  0!:2 y234
  Debug=: 0
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
  assert. (<'base')-:18!:5''
  assert. ((;:'oldnl y234 RLAST')-.~nl'') -: oldnl
NB.  11 s: ''    NB. reset symbol
  echo (+/ % #) 0 s: 12
 end.
end.
4!:55 ;:'oldnl'
Debug=: d123
dbr 0
echo 'Finish'
''
)

RUND2=: 4 : 0
x123=. (0=x){x,1
y123=. y
d123=. Debug
assert. (<'base')-:18!:5''
4!:55 ;:'x y oldnl RLAST'
assert. 0=(;:'x y oldnl RLAST') e. nl''
oldnl=: nl''
while. x123~:0 do.
 for_y234. y123{~?~#y123 do.
  echo RLAST=: >y234
  save_ran=:9!:44''
  Debug=: 0
  0!:2 y234
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
  assert. (<'base')-:18!:5''
  assert. ((;:'oldnl y234 RLAST save_ran')-.~nl'') -: oldnl
  Debug=: 1
  0!:2 y234
  Debug=: 0
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
  assert. (<'base')-:18!:5''
  assert. ((;:'oldnl y234 RLAST save_ran')-.~nl'') -: oldnl
NB.   11 s: ''    NB. reset symbol
  echo (+/ % #) 0 s: 12
 end.
 x123=. <:x123
end.
4!:55 ;:'oldnl save_ran'
Debug=: d123
dbr 0
echo 'Finish'
''
)

RUND3=: 4 : 0
x123=. x>.1
y123=. y
d123=. Debug
assert. (<'base')-:18!:5''
4!:55 ;:'x y oldnl RLAST'
assert. 0=(;:'x y oldnl RLAST') e. nl''
oldnl=: nl''
for_y234. y123{~?~#y123 do.
 echo RLAST=: >y234
 for. i.x123 do.
  Debug=: 0
  0!:2 y234
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
  assert. (<'base')-:18!:5''
  assert. ((;:'oldnl y234 RLAST')-.~nl'') -: oldnl
  Debug=: 1
  0!:2 y234
  Debug=: 0
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
  assert. (<'base')-:18!:5''
  assert. ((;:'oldnl y234 RLAST')-.~nl'') -: oldnl
NB.  11 s: ''    NB. reset symbol
  echo (+/ % #) 0 s: 12
 end.
end.
4!:55 ;:'oldnl'
Debug=: d123
dbr 0
echo 'Finish'
''
)

RUN2=: 4 : 0
x123=. (0=x){x,1
y123=. y
d123=. Debug
assert. (<'base')-:18!:5''
4!:55 ;:'x y oldnl'
assert. 0=(;:'x y oldnl') e. nl''
oldnl=: nl''
while. x123~:0 do.
 Debug=: 0
 0!:2<testpath,y123,'.ijs'
 assert. 0 s: 11
 assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
 assert. (<'base')-:18!:5''
 assert. ((;:'oldnl')-.~nl'') -: oldnl
 Debug=: 1
 0!:2<testpath,y123,'.ijs'
 Debug=: 0
 assert. 0 s: 11
 assert. _1 = 4!:0 <"1 ,/ ' 0123456789' ,"0/~ a.{~,|:(i.26)+/ a.i.'Aa'
 assert. (<'base')-:18!:5''
 assert. ((;:'oldnl')-.~nl'') -: oldnl
 x123=. <:x123
NB.  11 s: ''    NB. reset symbol
 echo (+/ % #) 0 s: 12
end.
4!:55 ;:'oldnl'
Debug=: d123
dbr 0
echo 'Finish'
''
)

tsu_notes=: 0 : 0
many scripts have timing tests
typically comparing timing/result of j vs j model
these tests can be essential for new/changed code
but vary greatly across environments
and can result in false failures

THRESHOLD set to 1 by default - ignores timing test failures
threshold set to 0.2 for loose tests (0.75 for tighter tests)

THRESHOLD should be applied as false failures are discovered

gfft/glapack not in ddall - run separately with: RUN1'gfft'
g18x fails on subsequent runs - no idea why
g401 occasionally fails (random data?) but then runs clean
)

tsu_usage=: 0 : 0
   RUN  ddall  NB. report scripts that fail
   
   RUN1 'g000' NB. run script with display
 n RUN2 'g000' NB. run script with display for n times
               NB. run infinite times until failure if n<0
   
 n RUND1 ddall NB. run script with display for n times and stop on failure
               NB. n>0, RLAST is the last script
 n RUND2 ddall NB. same as RUND1 but run all scripts in random order for n times
               NB. run infinite times until failure if n<0
 n RUND3 ddall NB. same as RUND1 but run each script for n times in random order
               NB. n>0

   RBAD ''     NB. report scripts that failed
   RB          NB. 0!:3 result (0 for failure)
   RF          NB. scripts that were run
   
   RECHO ddall NB. echo script names as run and final count of failures

   Debug=: 1   NB. run test suite in debug mode
)

tsu_jd=: 0 : 0
   runjd'' NB. run Jd jdtests
)

tsu_pacman=: 0 : 0
   runpacman'' NB. update addons
)

runpacman=: 3 : 0
if. IFWIN do.
 if. -.fexist'~tools/ftp/busybox.exe' do.
  echo'copy production J ~tools/ftp folder to jbld/j64/tools'
  'need ~tools/ftp/busybox.exe'assert 0
 end.
end.
load'pacman'
'update'jpkg''
'install'jpkg'all'
load'jmf' NB. use possibly new jmf
)

runjd=: 3 : 0
load'jd'
jdtests_jd_''
)

echo 0 : 0
see: tsu_notes, tsu_usage, tsu_pacman, and tsu_jd

   RUN  ddall  NB. report scripts that fail
   RECHO ddall NB. echo script names as run and final count of failures
)

echo 9!:14''

