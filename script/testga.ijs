(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin';'OpenBSD') i. <UNAME) pick ;:'linux darwin openbsd win'
os=: ((IF64{::'rpi32';'rpi64')"_)^:IFRASPI os
testres=: 'test',os,'.txt'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

NB. ddall=: ddall -. blacklist=: blacklist, ('Win'-:UNAME)#(<testpath),each <'g7x6.ijs' NB. temporarily disable
NB. ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each 'gtdot.ijs';'gtdot1.ijs';'gtdot2.ijs';'gtdot3.ijs';'gtdot4.ijs' NB. temp disable
NB. ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each 'gmmf.ijs';'gmmf1s.ijs';'gmmf1u.ijs';'gmmf1w.ijs';'g320ipt.ijs';'g7x5.ijs'

NB. smoke test
NB. RES=: RUN4 (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot.ijs';'gtdot3.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

NB. RUN1^:IFWIN 'g640'
NB. RUN1^:IFWIN 'g7x6'
NB. exit^:IFWIN 1

3 : 0''
if. 'Linux'-:UNAME do.
 GITHUBCIBUG1=: 1 e. 'CPU E5-2673 v3' E. 2!:0'cat /proc/cpuinfo'
else.
 GITHUBCIBUG1=: 0
end.
)

NB. this crash on Intel(R) Xeon(R) CPU E5-2673 v3 @ 2.40GHz
NB. echo '1: (3x ^ 2 ^ i. 10x)'
NB. 1: 1: (3x ^ 2 ^ i. 10x)
NB. echo '1: (128!:10)@(1000x ?@$~ ,~)"0 i. 15'
NB. 1: (128!:10)@(1000x ?@$~ ,~)"0 i. 15

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
