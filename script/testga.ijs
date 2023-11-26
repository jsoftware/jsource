(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin';'OpenBSD';'FreeBSD') i. <UNAME) pick ;:'linux darwin openbsd freebsd win'
os=: ((IF64{::'rpi32';'rpi64')"_)^:IFRASPI os
os=: ((IF64{::'win32';'win')"_)^:IFWIN os
os=: os, ((<os)e.'openbsd';'freebsd')#(('arm64'-:9!:56'cpu'){::'';'arm64')
testres=: 'test',os,'.txt'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each <'gstack.ijs' NB. temporarily disable
ddall=: ddall -. blacklist=: blacklist, ('FreeBSD'-:UNAME)#(<testpath),each 'gtdot.ijs';'gtdot1.ijs';'gtdot2.ijs';'gtdot3.ijs';'g13x.ijs';'g7x.ijs' NB. temporarily disable

NB. smoke test
NB. RES=: RUN4 (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot.ijs';'gtdot3.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

NB. this crash on Intel(R) Xeon(R) CPU E5-2673 v3 @ 2.40GHz
echo '1: (3x ^ 2 ^ i. 10x)'
1: 1: (3x ^ 2 ^ i. 10x)

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
if. (os -: 'win') *. 1 e. 'avx/' E. 9!:14'' do.
  13!:99''
end.
)

