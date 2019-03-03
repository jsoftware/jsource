NB. j to jhs server

0 : 0
JHS jjserver task
   load'~addons/ide/jhs/jjserver.ijs'
   init_jjserver_ 'pwsd'
   
J jj client task
   load'~addons/ide/jhs/jj.ijs'
   jjset 'ip:port pswd'
   '~temp/f.ijs' jhsget '~temp/g.ijs'
)   

require'tar'

coclass'jjserver'
coinsert'jhs'

jev_post_raw=: 3 : 0

try.
 d=. NV
 i=. d i. {.a.
 p=. i{.d
 'bad password'assert (0~:#PASS)*.p-:PASS
 d=. (>:i)}.d
 i=. d i. {.a.
 s=. i{.d
 d=. (>:i)}.d
 erase'y' NB. so do will see the global
 y__=: 3!:2 d
 r=. 3!:1 do__ s
catch.
 r=. 13!:12''
end. 
'jbin' gsrcf r
erase'y__'
i.0 0
)

init=: 3 : 0
'bad password'assert (2=3!:0 y)*.(1=$$y)*.1<#y
PASS=: y
addOKURL_jhs_'jjserver'
'client access with: jjset ''',(getlanip''),':',(":PORT),' ',PASS,''''
)