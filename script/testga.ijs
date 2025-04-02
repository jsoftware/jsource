(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin';'OpenBSD';'FreeBSD') i. <UNAME) pick ;:'linux darwin openbsd freebsd win'
os=: ((IF64{::'rpi32';'rpi64')"_)^:IFRASPI os
os=: ((IF64{::'win32';'win')"_)^:IFWIN os
os=: os, ((<os)e.'openbsd';'freebsd')#(('arm64'-:9!:56'cpu'){::'';'arm64')
os=: os, ((<os)e.<'darwin')#('APPLEM1'-:2!:5'APPLEM1'){::'';'arm'
os=: os, ((<os)e.<'win')#(('arm64'-:9!:56'cpu'){::'';'arm64')
testres=: 'test',os,'.txt'
IFWA64=: IFWIN*.'arm64'-:9!:56'cpu'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each <'gstack.ijs' NB. temporarily disable
ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each 'gtdot.ijs';'gtdot3.ijs';'gtdot4.ijs' NB. temporarily disable
ddall=: ~. ddall ,~ (-.IFWA64)#(IF64*.(<UNAME)e.<'Win')#(<testpath),each <'gcip1.ijs'
ddall=: ~. ddall ,~ (-.IFWA64)#((<UNAME)e.'Win';'Darwin')#(<testpath),each <'glapack.ijs'

NB. smoke test
NB. RES=: RUN4 (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot.ijs';'gtdot3.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

NB. this crash on Intel(R) Xeon(R) CPU E5-2673 v3 @ 2.40GHz
echo '1: (3x ^ 2 ^ i. 10x)'
1: 1: (3x ^ 2 ^ i. 10x)

NB. this crash on OpenBSD v7.6
echo '(128!:6)   ', (128!:6)   'abc'
echo '2&(128!:6) ', 2&(128!:6) 'abc'
echo '3&(128!:6) ', 3&(128!:6) 'abc'
echo '4&(128!:6) ', 4&(128!:6) 'abc'
echo '5&(128!:6) ', 5&(128!:6) 'abc'
echo '6&(128!:6) ', 6&(128!:6) 'abc'
echo '7&(128!:6) ', 7&(128!:6) 'abc'
echo '8&(128!:6) ', 8&(128!:6) 'abc'
echo '9&(128!:6) ', 9&(128!:6) 'abc'
echo '10&(128!:6) ', 10&(128!:6) 'abc'
echo '11&(128!:6) ', 11&(128!:6) 'abc'
echo '12&(128!:6) ', 12&(128!:6) 'abc'
echo '13&(128!:6) ', 13&(128!:6) 'abc'
echo '14&(128!:6) ', 14&(128!:6) 'abc'
echo '15&(128!:6) ', 15&(128!:6) 'abc'

1: RUN1 'g421'

echo 'avx512f: ',":9!:56'avx512f'
echo 'avx512vl: ',":9!:56'avx512vl'
echo 'avx512bw: ',":9!:56'avx512bw'
echo 'avx512vbmi: ',":9!:56'avx512vbmi'
echo 'avx512vbmi2: ',":9!:56'avx512vbmi2'

FINISH=: 3 : 0
msg=. 9!:14''
if. 0=#RES do.
  msg=. msg,LF,'all tests correct'
else.
  msg=. msg,LF,'test fails:'
  msg=. msg,;<@(LF,dtb) "1 RES
end.
msg fappends testres
echo^:(*@#RES) RES
exit *@#RES
)

echo 'RUN4 ddall'
9!:27'FINISH RES=: RUN4 ddall'
9!:29]1

1: 0 : 0
if. IFWIN *. 1 e. 'avx/' E. 9!:14'' do.
  13!:99''
end.
)

