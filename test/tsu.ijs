NB. test script utilities -----------------------------------------------

cocurrent 'base'
9!:19[2^_44   NB. default but some tests require a larger ct

3 : 0 ''
testpath=: '/',~(t i:'/'){.t=. jpath;(4!:4<'ddall'){4!:3''
if. IFWIN do.
 n=. 'tsdll.dll'
else.
 n=. 'libtsdll.',;(UNAME-:'Darwin'){'so';'dylib'
end. 
if. UNAME-:'Android' do.
 arch=. LF-.~ 2!:0'getprop ro.product.cpu.abi'
 LIBTSDLL=: jpath'~bin/../libexec/',arch,'/',n,' '
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
blacklist=: (<testpath),each 'gmbx.ijs';'gfft.ijs';'glapack.ijs'

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

etx      =: 1 : 'x :: (<:@(13!:11)@i.@0: >@{ 9!:8@i.@0:)'  NB. error message from error number
ex       =: ". etx
fex      =: }. @ (i.&(10{a.) {. ]) @ (13!:12) @ i. @ 0: @ (0!:110)
eftx     =: 1 : 'u :: ((10{a.) -.~ (13!:12) @ i. @ 0:)'   NB. full text of error message
efx      =: ". eftx



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

NB. ensure ~temp is unique when running multiple instances
pidtemp=: 3 : 0
t=. (<'temp') i.~ {."1 SystemFolders_j_
u=. (<'user') i.~ {."1 SystemFolders_j_
1!:5 ::1: tmp=. < (>(<u,1){SystemFolders_j_),'/temp-',":2!:6''
SystemFolders_j_=: tmp (<t,1)}SystemFolders_j_
''
)

NB. ebi extensions

RSET=: 4 : '(x)=: y'
RBAD=: 3 : '>_4}.each(#testpath)}.each(-.RB)#RF'
RUN=: RBAD@('RB'&RSET)@(0!:3)@('RF'&RSET)
RUND=: RBAD@('RB'&RSET)@(0!:2)@('RF'&RSET)  NB. Run w/display

RUN1=: 13 : '0!:2<testpath,y,''.ijs'''

RESUB1=: 3 : 'y[echo >y'
RESUB2=: (13 : '-.0!:3 RESUB1 y')"0
RECHO=: 13 : '+/ RESUB2 y'

NB. bill extensions

RUNN=: 4 : 0
x123=. x>.1
y123=. y
r123=. (#y123)#1
4!:55 'x';'y'
while. x123~:0 do.
 echo 'countdown ',":x123
 RUN y123
 r123=. RB=: r123*.RB
 echo RBAD''
 x123=. <:x123
NB.  11 s: ''    NB. reset symbol
 echo (+/ % #) 0 s: 12
end.
echo 'Finish'
)

RUND1=: 4 : 0
x123=. x>.1
y123=. y
4!:55 'x';'y'
oldnl=: (nl'')-.;:'x y'
for_y234. y123 do.
 echo RLAST=: >y234
 for. i.x123 do.
  0!:2 y234
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. 0=#(nl'')-.oldnl,'oldnl';'y234';'x123';'RLAST'   NB. no memory leak
  assert. _1 = 4!:0 ;:'x y'
NB.  11 s: ''    NB. reset symbol
  echo (+/ % #) 0 s: 12
 end.
end.
''
)

RUND2=: 4 : 0
x123=. x
y123=. y
4!:55 'x';'y'
oldnl=: (nl'')-.;:'x y'
while. x123~:0 do.
 for_y234. y123{~?~#y123 do.
  echo RLAST=: >y234
  0!:2 y234
  assert. 0 s: 11  NB. can cause segfault in subsequent scripts if not caught early
  assert. 0=#(nl'')-.oldnl,'oldnl';'y234';'x123';'RLAST'   NB. no memory leak
  assert. _1 = 4!:0 ;:'x y'
NB.   11 s: ''    NB. reset symbol
  echo (+/ % #) 0 s: 12
 end.
x123=. <:x123
end.
''
)

RUN2=: 4 : 0
x123=. x
y123=. y
4!:55 'x';'y'
oldnl=: (nl'')-.;:'x y'
while. x123~:0 do.
 0!:2<testpath,y123,'.ijs'
 assert. 0 s: 11
 assert. 0=#(nl'')-.oldnl,'oldnl';'x123';'RLAST'
 assert. _1 = 4!:0 ;:'x y'
 x123=. <:x123
NB.  11 s: ''    NB. reset symbol
 echo (+/ % #) 0 s: 12
end.
''
)

NB. =======================================================

echo 9!:14''

echo 0 : 0

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


   RUN  ddall  NB. report scripts that fail
 n RUNN ddall  NB. same as RUN but run for n times
   
   RUN1 'g000' NB. run script with display
 n RUN2 'g000' NB. run script with display for n times
               NB. run infinite times until failure if n<0
   
 n RUND1 ddall NB. run script with display for n times and stop on failure
               NB. RLAST is the last script
 n RUND2 ddall NB. same as RUND1 but run in random order
               NB. run infinite times until failure if n<0

   RBAD ''     NB. report scripts that failed
   RB          NB. 0!:3 result (0 for failure)
   RF          NB. scripts that were run
   
   RECHO ddall NB. echo script names as run and final count of failures
)
