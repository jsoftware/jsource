(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=: (('Linux';'Darwin') i. <UNAME) pick ;:'linux darwin win'
testres=: 'test',os,'.txt'

0!:0 <testpath,'tsu.ijs'
GITHUBCI=: 1       NB. testing on github
ECHOFILENAME=: 1   NB. echo file name

stdout LF ,~ 9!:14''

NB. RES=: RUN (<testpath),each 'gtdot.ijs';'gtdot.ijs';'gtdot1.ijs';'gtdot1.ijs';'gtdot2.ijs';'gtdot2.ijs';'gtdot3.ijs';'gtdot3.ijs';'gtdot4.ijs';'gtdot4.ijs';'gtdot3.ijs';'gtdot2.ijs';'gtdot1.ijs';'gtdot.ijs'
NB. echo^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

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
