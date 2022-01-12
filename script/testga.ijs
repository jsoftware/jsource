
(9!:14'') 1!:2 [2

testpath=: (1!:43''),'/test/'

os=. (('Linux';'Darwin') i. <UNAME) pick ;:'linux darwin win'
testres=: 'test',os,'.txt'

0!:0 <testpath,'tsu.ijs'

stdout LF ,~ 9!:14''

NB. RES=: RUN ddall
NB. RES=: RUN ddall -. < testpath,'gstack.ijs'
RES=: RUN ~. ddall ,~ < testpath,'gstack.ijs'

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

exit ''
