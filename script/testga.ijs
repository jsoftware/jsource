(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin';'OpenBSD';'FreeBSD') i. <UNAME) pick ;:'linux darwin openbsd freebsd win'
os=: ((IF64{::'rpi32';'rpi64')"_)^:IFRASPI os
os=: os, ((<os)e.'openbsd';'freebsd')#(('arm64'-:9!:56'cpu'){::'';'arm64')
testres=: 'test',os,'.txt'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

NB. ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each <'gstack.ijs' NB. temporarily disable
NB. ddall=: ddall -. blacklist=: blacklist, ('FreeBSD'-:UNAME)#(<testpath),each <'gtdot3.ijs' NB. temporarily disable

NB. smoke test
NB. RES=: RUN4 (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot.ijs';'gtdot3.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

NB. this crash on Intel(R) Xeon(R) CPU E5-2673 v3 @ 2.40GHz
echo '1: (3x ^ 2 ^ i. 10x)'
1: 1: (3x ^ 2 ^ i. 10x)
echo '1: (128!:10)@(1000x ?@$~ ,~)"0 i. 15'
1: (128!:10)@(1000x ?@$~ ,~)"0 i. 15

echo 'RUN4 ddall'
RES=: RUN4 ddall

3 : 0''
msg=. 9!:14''
if. 0=#RES do.
  msg=. msg,LF,'all tests correct'
else.
  msg=. msg,LF,'test fails:'
  msg=. msg,;<@(LF,dtb) "1 RES
end.
msg fappends testres
)

1: 0 : 0
if. (os -: 'win') *. 1 e. 'avx/' E. 9!:14'' do.
  13!:99''
end.
)

echo^:(*@#RES) RES
exit *@#RES
