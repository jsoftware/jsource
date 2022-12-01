(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin') i. <UNAME) pick ;:'linux darwin win'
testres=: 'test',os,'.txt'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

ddall=: ddall -. blacklist=: blacklist, (GITHUBCI*.'Win'-:UNAME)#(<testpath),each <'g7x6.ijs' NB. temporarily disable

NB. smoke test
NB. RES=: RUN (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot.ijs';'gtdot3.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

NB. RUN1^:(GITHUBCI*.'Linux'-:UNAME) 'g128x'

NB. RUN1^:IFWIN 'g640'
NB. RUN1^:IFWIN 'g7x6'
NB. exit^:IFWIN 1

RES=: RUN ddall

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
