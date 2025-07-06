(9!:14'') 1!:2 [2

load'pacman'
'update'jpkg''
'upgrade'jpkg'dev/eformat dev/lu'

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin';'OpenBSD';'FreeBSD') i. <UNAME) pick ;:'linux darwin openbsd freebsd win'
os=: ((IF64{::'rpi32';'rpi64')"_)^:IFRASPI os
os=: ((IF64{::'win32';'win')"_)^:IFWIN os
os=: os, ((<os)e.'openbsd';'freebsd')#(('arm64'-:9!:56'cpu'){::'';'arm64')
os=: os, ((<os)e.<'darwin')#('ARM64'-:2!:5'RUNNER_ARCH'){::'';'arm'
os=: os, ((<os)e.<'win')#(('arm64'-:9!:56'cpu'){::'';'arm64')
os=: os, ((,'3')-:2!:5'_DEBUG'){::'';'d'
testres=: 'test',os,'.txt'
IFWA64=: IFWIN*.'arm64'-:9!:56'cpu'

0!:0 <testpath,'tsu.ijs'
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

echo '_DEBUG: ',": 2!:5'_DEBUG'
echo 'RUNNER_ARCH: ',": 2!:5'RUNNER_ARCH'

ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each <'gstack.ijs' NB. temporarily disable
ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each 'gtdot.ijs';'gtdot3.ijs';'gtdot4.ijs';'gtdot5.ijs' NB. temporarily disable
ddall=: ~. ddall ,~ (-.IFWA64)#((<UNAME)e.'Win';'Darwin')#(<testpath),each <'glapack.ijs'

NB. smoke test
NB. RES=: RUN4 (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot1.ijs';'gtdot5.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

echo 'avx512f: ',":9!:56'avx512f'
echo 'avx512vl: ',":9!:56'avx512vl'
echo 'avx512bw: ',":9!:56'avx512bw'
echo 'avx512vbmi: ',":9!:56'avx512vbmi'
echo 'avx512vbmi2: ',":9!:56'avx512vbmi2'

NB. this crash on Intel(R) Xeon(R) CPU E5-2673 v3 @ 2.40GHz
echo '(3x ^ 2 ^ i. 10x)'
empty (3x ^ 2 ^ i. 10x)

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

NB. this crash on J32
echo '0 0 1 2 2 2 3 3 4 5 5 5 </. i.12'
empty 0 0 1 2 2 2 3 3 4 5 5 5 </. i.12

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

