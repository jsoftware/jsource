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
os=: os, (IFRASPI<(<os)e.<'linux')#(IF64{::'32';'')
os=: os, ((,'3')-:2!:5'_DEBUG'){::'';'d'
testres=: 'test',os,'.txt'
IFWA64=: IFWIN*.'arm64'-:9!:56'cpu'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

echo '_DEBUG: ',": 2!:5'_DEBUG'
echo 'RUNNER_ARCH: ',": 2!:5'RUNNER_ARCH'

ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each <'gstack.ijs' NB. temporarily disable
ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each 'gtdot.ijs';'gtdot3.ijs';'gtdot4.ijs';'gtdot5.ijs' NB. temporarily disable
ddall=: ~. ddall ,~ (-.IFWA64)#((<UNAME)e.'Win';'Darwin')#(<testpath),each <'glapack.ijs'


oldnl=: (;:'x y') -.~ ~. oldnl, (nl__ i.4) , ;:'FINISH RES'

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

