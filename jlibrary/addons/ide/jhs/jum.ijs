NB. J HTTP Server - jum app
coclass'jum'
coinsert'jhs'

3 : 0''
if. IF64 do.
 JHS=: jpath'~user/jhs/'
else.
 JHS=: jpath'~home/j805-user/jhs/'
end.
)

NB. ALLPORTS jum ports
NB. HOSTNAME localhost or www.jsoftware.com or ...
NB. JUMPASS  jum password
startjum=: 3 : 0
'ALLPORTS HOSTNAME JUMPASS'=:y
OKURL_jhs_=:'jum' NB. jum no login
HOSTIP=: >2{sdgethostbyname_jsocket_ HOSTNAME NB. is 202.67.223.49 faster than www.jsoftware.com ?
VALIDNAMECHARS=:'-_',((i.26)+a.i.'a'){a.
i.0 0
)

startjum_localhost=: 3 : 0
startjum (65002+i.3);'localhost';'jumjum'
)

startjum_jsoftware=: 3 : 0
ssend_jhs_=: jum_ssend_jhs_
logappx'start'
startjum (50248+123*i.60);'www.jsoftware.com';'jumjum'
)

NB. jsoftware jum crashes occasionally
NB. logging extra ssend info might help catch the bug
jum_ssend_jhs_=: 3 : 0
logapp'send: ',":#y
z=. sdselect_jsocket_ '';SKSERVER;'';PC_SENDTIMEOUT
logapp'send z: ',;',',each":each z
'send not ready' serror SKSERVER~:>2{z
if. PC_SENDSLOW do.
 6!:3[0.2
 y=. (100<.#y){.y NB. 100 byte chunks with delay
end.
logapp'before sdsend'
try.
 'c r'=. y sdsend_jsocket_ SKSERVER,0
catch.
 logapp'send catch!!!!!!!!!'
end.
logapp'after sdsend'
('send error: ',":c) serror 0~:c
'send no data' serror 0=r
logapp'send c r: ',":c,r
r NB. bytes sent
)


SIGCONT=: >('Darwin'-:UNAME){'18';'19'

HBS=: 0 : 0
jhh1'J User Manager'
'msg'jhdiv' '
jhhr
jhh1'manage your account'
jhtablea
 jhtr 'user: ';'user' jhtext'';15
 jhtr 'pass: ';'pass' jhpassword'';15
jhtablez
'status' jhb'status'
'attn'   jhb'attn'
'kill'   jhb'kill'
'start'  jhb'start'
'go'     jhb'go'
jhhr
jhh1'create your account'
jhtablea
 jhtr 'user: '       ;'usern' jhtext'';15
 jhtr 'pass: '       ;'passn' jhpassword'';15
 jhtr 'repeat pass: ';'repeat'jhpassword'';15
 jhtr 'jum pass:  '  ;'jum'jhpassword'';15
jhtablez
'new'    jhb'new'
)

nopid=: ,'0'
invalid=: 'invalid user/pass'

NB. check user;pass
NB. return 0 invalid, 1 valid user/pass
check=: 3 : 0
'user pass'=. y
try.
 t=. fread jpath JHS,user,'/config/jhs.ijs'
 t=. (1 i.~'PASS=:'E. t)}.t
 t=. (>:t i.'''')}.t
 t=. (t i.''''){.t
 >(pass-:t){0;1
catch.
0
end.
)

log=: 3 : 0
s=. y rplc LF;' ';'<br>';'';'&nbsp;';' ';'</a>';''
'a b'=. s i. '<>'
if. (a<b)*.b<#s do. s=. (a{.s),(>:b)}.s end. NB. remove <link>
but=. getv'jbutton'
but=. >(0-:but){(6{._7}.but);'status'
logapp_jhs_ but,' ; ',s
)

NB. y is paths to jum user folders
validatepids=: 3 : 0
if. IFUNIX do.
 for_n. y do. 
  pid=. readpid >n
  r=. 1:@(2!:0) :: 0: 'kill -s ',SIGCONT,' ',pid NB. SIGCONT
  if. -.r do. nopid writepid >n end.
 end.
else.
 f=. jpath'~temp/tasklist.txt'
 doscmd'tasklist /FI "IMAGENAME eq jconsole.exe">',f
 6!:3[0.25 NB. ugh - give doscmd a chance finish
 t=. <;._2 fread f
 pids=.>1{each 0".each((<'jconsole.exe')=12{.each t)#t
 r=. ;0".each getpids y
 ps=. (-.r e. pids,0)#y
 (<nopid)writepid each ps NB. write dead pids
end.
)

NB. y paths to user folders
getpids=: 3 : 0
readpid each y
)

NB. y path to user folder
readpid=: 3 : 0
f=. y,'/.jhspid'
t=. fread f
if. _1-:t do. (t=.nopid)fwrite f end.
t 
)

NB. pid writepid userfolder
writepid=: 3 : 0
:
x fwrite y,'/.jhspid'
)

NB. y is user
getpid=: 3 : 0
validatepids getupaths''
readpid JHS,y
)

NB. y is user
starttask=: 3 : 0
t=. '-js "load''~addons/ide/jhs/core.ijs''" "init_jhs_''',y,'''"'
if. IFUNIX do.
 2!:1 ('"',jpath'~bin/jconsole'),'" ',t,' &'
else.
 doscmd ('"',jpath'~bin/jconsole.exe'),'"  ',t
end.
)

NB. & at end of command is critical
unixshell=: 3 : 0
smoutput y
2!:0 y
)

jev_get=: create

CSS=: 0 : 0
#msg{color:red;}
)

NB. y is MSG for the html result
create=: 3 : 0
logapp'jum create'
if. -.'jum'-:_3{.jpath'~user' do. y=. '<h1>WARNING: not running as jum!</h1>',y end.
'jum'jhr''
)

ev_status_click=: 3 : 0
logapp'jum status: ',(getv'user'),' ',(getv'jmid')
'user pass'=. getvs'user pass'
if. check user;pass do.
 r=. report user
else.
 r=. invalid
end.
jhrajax (getv'jmid'),': ',r
)

ev_go_click=: ev_status_click

jhscfg=: 0 : 0
PORT=: <PORT>
LHOK=: 0
BIND=: 'any'
PASS=: '<PASS>'
)

new=: 0 : 0
<div style="color:red"><PASS> is your password and is required to login.</div>
)

ev_usern_enter=:  ev_new_click
ev_passn_enter=:  ev_new_click
ev_repeat_enter=: ev_new_click
ev_jum_enter=:    ev_new_click

ev_jum_enter=: ev_new_click

NB. create new user
ev_new_click=: 3 : 0
logapp'jum new: ',getv'jmid'
'usern passn repeat jum'=.getvs'usern passn repeat jum'
ports=. ;_1".each 1{"1 usertable'' NB. ports in use
port=. {.ALLPORTS-.ports           NB. first free port
if. 0=port do. cleanx 1 end.       NB. free 'oldest' port
ports=. ;_1".each 1{"1 usertable''
port=. {.ALLPORTS-.ports
if.     -.jum-:JUMPASS do. r=.'invalid jum pass (check with JUM admin)'
elseif. (4>#usern)+.#usern-.VALIDNAMECHARS do. r=. 'user must be at least 4 chars from ',VALIDNAMECHARS
elseif. (<usern)e.{."1 usertable'' do. r=. 'user already exists'
elseif. 4>#passn do. r=. 'pass must be at least 4 chars'
elseif. -.passn-:repeat do. r=. 'repeat not the same as pass'
elseif. 0=port do. r=. 'no ports available'
elseif. 1 do.
 try.
  p=. JHS,usern
  1!:5 <p NB. create new user folders
  1!:5 <jpath p,'/config'
  1!:5 <jpath p,'/projects'
  1!:5 <jpath p,'/temp'
  (jhscfg hrplc 'PASS PORT';passn;":port) fwrite p,'/config/jhs.ijs'
  r=. 'user created'
 catch.
  r=. 'create new user failed'
 end.
end.
logapp'jum new: ',r
jhrajax 'new: ',r
)

signal_attn=: 3 : 0
:
logapp'jum attn: ',x
smoutput 'break ',x,' ',y
try.
 f=. <jpath (_4}.JHS),'break/',y,'.default'
 v=. 2<.>:a.i.1!:11 f,<0 1
 (v{a.) 1!:12 f,<0 NB. 12 not 2
catch.
end.
)

signal_kill=: 3 : 0
:
logapp'jum kill: ',x
nopid writepid JHS,x NB. clear out dead pid
if. IFUNIX do.
 unixshell 'kill -s 9 ',y
else.
 doscmd 'taskkill /f /t /pid ',y
end.
)

killallexceptjum=: 3 : 0
validatepids''
users=. ({."1 usertable'')-.<'jum'
pids=. getpid each users
users=. (-.pids=<,'0')#users
users signal_kill each getpid each users
)

signal=: 4 : 0
'user pass'=. getvs'user pass'
r=. check user;pass
if. -.r do. jhrajax x,invalid return. end.
pid=. getpid user
if. nopid-:pid do.
 r=. 'task not running'
else.
 if. y-:'BREAK' do.
  user signal_attn pid
 else.
  user signal_kill pid
 end.
 r=. user,' ',y,' signaled'
end.
jhrajax x,r
)

NB. clean_jum_(t i.' '){.t=.{.show_jum_ 6 NB. clean oldest

NB. cleanx y - kill y oldest users
cleanx=: 3 : 0
if. y>10 do. 'max 10 killed at a time' return. end.
while. y do.
 t=.{.show 6
 smoutput t
 clean(t i.' '){.t
 y=. <:y
end.
)

NB. clean user - kill task if any and delete folders
clean=: 3 : 0
user=. y
if. user-:'jum' do. smoutput'will not delete jum' return. end.
pid=. getpid user
if. nopid-:pid do.
 smoutput'task was not running'
else.
 smoutput'killing task: ',pid 
 user signal_kill pid
end.
6!:3[2 NB. give kill a chance so deletefolder will work
p=. JHS,user
smoutput 'destroying folder: ',p
'suspicious path for delete folder'assert 16<#p
deletefolder p
)

NB. deletefolder y
deletefolder=: 3 : 0
p=. <jpath y
if. 1=#1!:0 p do.
 if. 'd'=4{,>4{"1 (1!:0) p do.
  deleterecursive y
  1!:55 p
 end.
end.
i.0 0
)

NB. deletesub y
deleterecursive=: 3 : 0
assert. 3<+/PATHSEP_j_=jpath y
ns=. 1!:0 <jpath y,'\*'
for_n. ns do.
 s=. jpath y,'\',>{.n
 if. 'd'=4{>4{n do.
  deleterecursive s
 end.
 1!:55<s
end.
)

ev_attn_click=: 3 : 0
'attn: 'signal'BREAK'
)

ev_kill_click=: 3 : 0
'kill: 'signal'SIGKILL'
)

ev_start_click=: 3 : 0
logapp'jum start: ',getv'user'
'user pass'=. getvs'user pass'
if. -.check user;pass do.
 r=. invalid
else.
 pid=. getpid user
 if. nopid-:pid do.
  jhrajax 'start: task started'
  6!:3[2 NB. too quick to new task crashes jum???
  logjhs user,' starttask a'
  starttask user NB. must do jhrajax first else hangs
  logjhs user,' starttask z'
  return.
 else.
  r=. ' task already running'
 end.
end.
jhrajax 'start: ',r
)

portline=: 3 : 0
d=. <;._2 ' '-.~y,LF
b=. (<'PORT=:')=6{.each d 
5{.6}.'',;b#d
)

getports=: 3 : 0
q=. y,each<'/config/jhs.ijs'
c=. ":each fread each q
portline each c
)

getupaths=: 3 : 0
d=. 1!:0 <JHS,'*'
jpath each (<JHS),each('d'=4{"1 >4{"1 d)#,{."1 d
)

getusers=: 3 : '(#JHS)}.each y'

NB. return table of users ports pids starts input busy last
usertable=: 3 : 0
ps=. getupaths''
validatepids ps
ports=. getports ps
pids=. getpids ps
users=. getusers ps
r=. jpath each ps,each<'/.jhslog.txt'
r=. 1!:1 :: ((,LF)&[) each <each r
starts=. +/each (<'start') E.each r
inputs=. +/each (<'prompt') E.each r
last=. ;each{: each <;._2 each r
busy=: +/each(<'sentence')E.each last
users,.ports,.pids,.starts,.inputs,.busy,.last
)

NB. y is col to sort by
show=: 3 : 0
a=. usertable''
a=. ":each a/:y{"1 a
c=. 2+>./ >#each a
;"1 (($a)$c){.each a
)

NB. HOSTNAME used instead of HOSTIP - wonder if HOSTIP would be faster
report=: 3 : 0
t=. usertable''
t=. (({."1 t)i.<y){t
port=. ":>1{t
pid=.  ":>2{t
t=. >(0~:".pid){' task not running';userreport rplc '<HOSTIP>';HOSTNAME;'<PORT>';port
)

userreport=: 0 : 0
<a href="http://<HOSTIP>:<PORT>/jijx">http://<HOSTIP>:<PORT>/jijx</a>
)

NB. create jum multi-user folder
NB. insist on virgin install - that is, jhs folder is empty
NB. y is port to serve and password
createjum=: 3 : 0
if. -.IF64 do.
 'not running as normal j805-user' assert 'j805-user'-:_9{.jpath'~user'
end.
'port pass'=. y
1!:5 :: [ <jpath'~user/jhs'
p=. jpath'~user/jhs/jum'
try.
 1!:5 <p NB. create jum folders
 1!:5 <jpath p,'/config'
 1!:5 <jpath p,'/projects'
 1!:5 <jpath p,'/temp'
catch. end.
(jhscfg hrplc 'PASS PORT';pass;":port) fwrite p,'/config/jhs.ijs'
(":port),' ',pass
i.0 0
)

JS=: 0 : 0
function ev_body_load(){jbyid("user").focus();}
function ev_status_click(){jdoajax(["user","pass"]);}
function ev_attn_click(){jdoajax(["user","pass"]);}
function ev_kill_click(){jdoajax(["user","pass"]);}
function ev_start_click(){jdoajax(["user","pass"]);}
function ev_go_click(){jdoajax(["user","pass"]);}

function ev_usern_enter() {ev_new_click();}
function ev_passn_enter() {ev_new_click();}
function ev_repeat_enter(){ev_new_click();}
function ev_jum_enter()   {ev_new_click();}
function ev_new_click(){jdoajax(["usern","passn","jum","repeat"]);}

function ajax(ts)
{
 var i,t;
 jbyid("msg").innerHTML=ts[0];
 if("new: user created"==ts[0]) // created new user
 {
  jbyid("user").value=jbyid("usern").value;
  jbyid("pass").value=jbyid("passn").value;
  jbyid("usern").value="";
  jbyid("passn").value="";
  jbyid("repeat").value="";
  jbyid("jum").value="";
 }
 if("click"==jform.jtype.value)
 {
  if("go"==jform.jmid.value)
  {
   i=ts[0].indexOf("http://");
   if(-1==i)
    jbyid("user").focus()
   else
   {
    t=ts[0].substr(i);
    location=t.substr(0,t.indexOf("\""));
   }  
  }
  else if("new"==jform.jmid.value)
   jbyid("usern").focus();
  else
   jbyid("user").focus();
 }
}
)

NB. windows createprocess stuff for windows testing

NB. && separates multiple commands
NB. use /S with quotes around complete command to preserve quotes
NB. 2>&1 to send stderr to stdout

doscmd=: 3 : 0
WaitForSingleObject=. 'kernel32 WaitForSingleObject i x i'&cd
CloseHandle=. 'kernel32 CloseHandle i x'&cd"0
CreateProcess=. 'kernel32 CreateProcessW i x *w x x i  i x x *c *c'&cd
CREATE_NO_WINDOW=. 16b8000000
CREATE_NEW_CONSOLE=. 16b00000010
f=. CREATE_NO_WINDOW
f=. CREATE_NEW_CONSOLE
NB. /S strips leading " and last " and leaves others alone
c=. uucp 'cmd /S /C "',y,'"'
si=. (68{a.),67${.a.
pi=. 16${.a.
'r i1 c i2 i3 i4 f i5 i6 si pi'=. CreateProcess 0;c;0;0;0;f;0;0;si;pi
ph=. _2 ic 4{.pi
CloseHandle ph
)
