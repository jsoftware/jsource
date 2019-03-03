NB. JHS - core services
require 'socket'

coclass'jhs'

0 : 0

*** timer - wd docs
timer i ; set interval timer to i milliseconds.
Event systimer occurs when time has elapsed.
The timer keeps triggering events until the timer is turned off.
An argument of 0 turns the timer off.
The systimer event may be delayed if J is busy,
and it is possible for several delayed events to be reported as a single event.

*** Cache-Control: no-cache
Browser caching can be confusing and is quite different
from a desktop application.

Back/forward, switching tabs, switching browser apps, are
showing cached pages. A get (typed into the URL box or from
favorites) shows a cached page if possible. And exactly when
it shows a cached page and when it gets a fresh page varies
from browser to browser and the phase of the moon. This can
be confusing if you have the expectation of a new page with
current information.

Ajax requests (in particular JIJX) have no-cache as old
pages in this area are more confusing and than useful.

All other pages allow cache as the efficiency of mucking
around pages without dealing with the server is significant.
Sometimes this means that when you want a fresh page with
latest info you are in getting a cached version.

Some browsers have a transmission progress bar indicator.
No flash means you are getting a cached page and a flash
means you getting a new page.

Refresh (F5 on some browsers) gets a fresh page and is a
useful stab poke if confused.

*** login/bind/cookie/security overview

listening socket binds to any
localhost is relatively secure
firewalls provide protection
localhost is relatively secure and would gain little from login
tunnel to localhost provides good security
non-localhost requires a login
login is provided by a cookie
cookie set in the response to providing a valid password
cookie is then included in the header of all requests - validated by server
cookie is non-persistent and is deleted when browser closes.
tabs do not need to login, but a new browser does.

*** app overview
URL == APP == LOCALE

Browser request runs first available sentence from:
 post          - jdo
 get URL has . - jev_get_jfilesrc_ URL_jhs_
 get           - jev_get_URL_''

Post can be submit (html for new page) or ajax (for page upates).

The sentence can send a response (closing SKSERVER).

urlresponse_URL_ run if response has not been sent
when new input required. jijx does this as the response
requires J output/prompt that are not available until then.

Use XMLHttpRequest() for AJAX style single page app.
Post request for new data to update page. jijx app does
this for significant benefit (faster and no flicker).

Form has hidden:
 button to absorb enter not in input text (required in FF)
 jdo="" submit sentence

Enter in input text field caught by element keydown event handler.

*** event overview
Html element id has main part and optional sub part mid[*sid].

<... id="mid[*sid]" ... ontype="return jev(e)"

jev(event)
{
 sets evid,evtype,evmid,evsid,evev
 onclick is type click etc
 try eval ev_mid_type()
 returns true or false
}

If ev_mid_type returns value, it is returned to the onevent caller,
otherwise a calculated value is returned.

ev_mid_type can ajax or submit J sentence.
Ajax has explicit nv pairs in post data and result.
Submit has normal form nv pairs in post data and result is new page

*** gotchas

Form elements use name="...". Submit of hidden element requires
name and the element will not be included in post data with just id.

Javascript works with id. In general a form input element should have
the same value for both id and name. The exception is radio where id
is unique and name is the same across a set of radio buttons.

***
1. depends on cross platform javascript and styles

2. 127.0.0.1 seems faster than localhost
   wonder if dot ip name is faster than www.jsoftware.com

3. Enter with only text has no button.
   Enter with buttons submits as if first button pressed.

4. html pattern
<!DOCTYPE html>
<html>
 <head>
  <meta...>
  <title>...</title>
  [<style type="text/css">...</style>...]
 </head>
 <body>
  ...
 </body>
 [<script>...</script>...]
</html>

5. autocomplete and wrap fail validator - but are necessary
)

JIJSAPP=: 'jijs' NB. 'jijsm' for simple jijs editor
PROMPT=: '   '
JZWSPU8=: 226 128 139{a. NB. empty prompt kludge - &#8203; \200B

NB. prevent child inherit - critical with fork
cloexec=: 3 : 0
if. -.IFUNIX do. return. end.
((unxlib 'c'),' fcntl i i i i') cd y,F_SETFD_jsocket_,FD_CLOEXEC_jsocket_
)

NB. J needs input - y is prompt - '' '   ' '      '
input=: 3 : 0
logapp 'jhs input prompt: ',":#y
try.
if. _1~:SKSERVER do. try. ".'urlresponse_',URL,'_ y' catch. end. end. NB. jijx
if. _1~:SKSERVER do. jbad'' end.
getdata'' NB. get and parse http request
if. 1=NVDEBUG do. smoutput seebox NV end. NB. HNV,NV
if. -. ((<URL)e.boxopen OKURL)+.(cookie-:gethv'Cookie:')+.PEER-:LOCALHOST
                       do. r=. 'jev_get_jlogin_ 0'
elseif. 1=RAW          do. r=. 'jev_post_raw_',URL,'_'''''
elseif. 'post'-:METHOD do. r=. getv'jdo'
elseif. '.'e.URL       do. r=. 'jev_get_jfilesrc_ URL_jhs_'
elseif. 1              do. r=. 'jev_get_',URL,'_'''''
end.
logapp 'jhs sentence: ',r
if. JZWSPU8-:3{.r do. r=. 3}.r end. NB. empty prompt kludge
r NB. J sentence to run

catch.
 logappx 'input error'
 exit'' NB. 2!:55[11 crashes
end.
)

CHUNKY_jhs_=: 0

jhsexit=: 0 : 0
Your JHS server has exited.
Manually close all pages for that server.
They won't work properly without a server
and will be confused if the server restarts.
)

NB. J has output - x is type, y is string
NB. MTYOFM  1 formatted result array
NB. MTYOER  2 error
NB. MTYOLOG  3 log
NB. MTYOSYS  4 system assertion failure
NB. MTYOEXIT 5 exit - not used
NB. MTYOFILE 6 output 1!:2[2
NB. x is type, y is string
output=: 4 : 0
logapp 'output type : ',":x
if. 5=x do.
 NB. jhrajax 'Your J HTTP Server has exited.<br/><div id="prompt" class="log">&nbsp;&nbsp;&nbsp;</div>'[PROMPT_jhs_=:'   '
 jhrajax'<font style="color:red;"><pre>',jhsexit,'</pre></font>'
 jfe_jhs_ 0
 2!:55[0
end.
try.
 s=. y NB. output string
 type=. x NB. MTYO type
 class=. >type{'';'fm';'er';'log';'sys';'';'file'
 
 if. (6=type)*.URL-:'jijx' do.
  t=. jhtmlfroma s
  if. '<br>'-:_4{.t do. t=. _4}.t end.
  LOGN=: LOGN,'<div class="',class,'">',t,'</div><!-- chunk -->'
  LOG_jhs_=: LOG,LOGN
  if. -.CHUNKY do.
   jhrajax_a LOGN
  else.
   jhrajax_b LOGN
  end.
  LOGN=: ''
  CHUNKY_jhs_=: 1
 
 elseif. (3~:type)+.-.'jev_'-:4{.dlb s do. NB. jev_... lines not logged
  if. 3=type do. s=. PROMPT,dlb s end.
  t=. jhtmlfroma s
  if. '<br>'-:_4{.t do. t=. _4}.t end.
  LOGN=: LOGN,'<div class="',class,'">',t,'</div>'
 end.
catch.
 logappx'output'
 exit''
end.
)

NB. event handler called by js event
NB. catch. changed to catchd. - december 2017 - for debug step with error
jev=: 3 : 0
try.
 ".t=. 'ev_',(getv'jmid'),'_',(getv'jtype'),' 0'
catchd.
 smoutput LF,'*** event handler error',LF,t,LF,(13!:12''),seebox NV
end.
)

NB. get/post data - headers end with CRLF,CRLF
NB. post has Content-Length: bytes after the header
NB. listen and read until a complete request is ready
getdata=: 3 : 0
RAW=: 0
while. 1 do.
 logapp 'getdata loop'
 cloexec SKSERVER_jhs_=: 0 pick sdcheck_jsocket_ sdaccept_jsocket_ SKLISTEN

 NB. JHS runs blocking sockets and uses sdselect for timeouts
 NB. sdioctl_jsocket_ SKSERVER,FIONBIO_jsocket_,1

 try.
  PEER=: >2{sdgetpeername_jsocket_ SKSERVER
  d=. h=. ''
  while. 1 do.
   h=. h,  srecv''
   i=. (h E.~ CRLF,CRLF)i.1
   if. i<#h do. break. end.
  end.
  i=. 4+i
  d=. i}.h
  h=. i{.h
  parseheader h
  if. '100-continue'-:gethv'Expect:' do.
   hr=. 'HTTP/1.1 100 Continue',CRLF,CRLF    NB. inform client to send request body
   try.
    while. #hr do. hr=. (ssend hr)}.hr end.
   catch.
    logapp '100-continue error: ',13!:12''
   end.
  end.
  if. 'POST '-:5{.h do.
   len=.".gethv'Content-Length:'
   while. len>#d do. d=. d,srecv'' end.
   d=. len{.d
   METHOD=: 'post'
   seturl'POST'
   if. 3=nc<'jev_post_raw_',URL,'_' do.
    RAW=: 1
    NV=: d
   else.
    parse d
   end.
  else.
   METHOD=: 'get'
   seturl'GET'
   t=. (t i.' '){.t=. gethv 'GET'
   parse (>:t i.'?')}.t
  end.
  return.

 catch.
  NB. t=. 13!:12''
  NB. if. -.'|recv timeout:'-:14{.t do. NB. recv timeout expected
  NB.  smoutput '*** getdata error: ',t
  NB. end.
  NB. recv errors expected and are not displayed
  logapp 'getdata error: ',t
 end.
end.
)

NB. possibly interesting idea - urlmod - use modifier at end of URL to customize J/Javascript
seturl=: 3 : 0
URL=: jurldecode}.(<./t i.' ?'){.t=. gethv y
)

serror=: 4 : 0
if. y do.
 shutdownJ_jsocket_ SKSERVER ; 2
 sdclose_jsocket_ ::0: SKSERVER
 logapp x
 x 13!:8[3
end.
)

NB. return SKSERVER data (toJ)
NB. serror on
NB.  timeout, socket error, or no data (disconnect)
NB. PC_RECVSLOW 1 gets small chunks with time delay

srecv=: 3 : 0
z=. sdselect_jsocket_ SKSERVER;'';'';PC_RECVTIMEOUT
if. -.SKSERVER e.>1{z do.
 'recv timeout' serror 1  NB.0;'';'';'' is a timeout
end.

'recv not ready' serror SKSERVER~:>1{z
if. PC_RECVSLOW do.
 6!:3[1
 bs=. 100 NB. 100 byte chunks with 1 second delay
else.
 bs=. PC_RECVBUFSIZE
end.
'c r'=. sdrecv_jsocket_ SKSERVER,bs,0
('recv error: ',":c) serror 0~:c
'recv no data' serror 0=#r
r NB. used to do toJ here!
)

secs=: 3 : 0
":60#.4 5{6!:0''
)

NB. return count of bytes sent to SKSERVER
NB. serror on
NB.  timeout, socket error, no data sent (disconnect)
NB. PC_SENDSLOW 1 simulates slow connection
ssend=: 3 : 0
z=. sdselect_jsocket_ '';SKSERVER;'';PC_SENDTIMEOUT
'send not ready' serror SKSERVER~:>2{z
if. PC_SENDSLOW do.
 6!:3[0.2
 y=. (100<.#y){.y NB. 100 byte chunks with delay
end.
'c r'=. y sdsend_jsocket_ SKSERVER,0
('send error: ',":c) serror 0~:c
'send no data' serror 0=r
r NB. bytes sent
)

putdata=: 3 : 0
logapp'putdata'
try.
 while. #y do. y=. (ssend y)}.y end.
catch.
 logapp 'putdata error: ',13!:12''
end.
)

NB. set HNV from request headers
parseheader=: 3 : 0
y=. toJ y
a=. <;._2 y
i=. (y i.' '),>:}.>a i. each ':'
HNV=: (i{.each a),.dlb each i}.each a
)

NB. global NV set from get/post data
NB. name/values delimited by & but no trailing &
NB. namevalue is name[=[value]]
NB. name0value[&name1value1[&name2...]]
parse=: 3 : 0
try.
 y=. toJ y
 d=. <;._2 y,'&'#~0~:#y
 d=. ;d,each('='e.each d){'=&';'&'
 d=. <;._2 d rplc '&';'='
 NV=: jurldecodeplus each (2,~(2%~#d))$d
catch.
 smoutput '*** parse failed: ',y
 NV=: 0 2$''
end.
)

gethv=: 3 : 0
i=. (toupper&.>0{"1 HNV)i.<toupper y
>1{i{HNV,0;''
)

NB. get value for name y - '' for no value
getv=: 3 : 0
i=. (0{"1 NV)i.<,y
>1{i{NV,0;''
)

NB. get values for names
getvs=: 3 : 0
((0{"1 NV)i.;:y){(1{"1 NV),<''
)

NB. ~name from full name
jshortname=: 3 : 0
p=. <jpath y
'a b'=.<"1 |:UserFolders_j_,SystemFolders_j_
c=. #each b
f=. p=(jpath each b,each'/'),each (>:each c)}.each p
if.-.+./f do. >p return. end.
d=. >#each f#b
m=. >./d
f=. >{.(d=m)#f#a
'~',f,m}.>p
)

NB. new ijs temp filename
jnew=: 3 : 0
d=. 1!:0 jpath '~temp\*.ijs'
a=. 0, {.@:(0&".)@> _4 }. each {."1 d
a=. ": {. (i. >: #a) -. a
f=. <jpath'~temp\',a,'.ijs'
'' 1!:2 f
>f
)

logclear=: 3 : ''''' 1!:2 logappfile'

NB. log timestamp
lts=: 3 : 0
20{.4 3 3 3 3 3":<.6!:0''
)

logapp=: 3 : 0
if. -.PC_LOG do. return. end.
((lts''),(>coname''),' : ',y,LF)1!:3 logappfile
)

NB. force log of this and following messages
logappx=: 3 : 0
PC_LOG=: 1
logapp y,' error : ',13!:12''
)

logstdout=: 3 : 'i.0 0[(y,LF) 1!:2[4'

audio=: 3 : 0
assert fexist y
jhtml'<audio controls="controls"><source src="',y,'" type="audio/mp3">not supported</audio>'
)

NB. z locale utilities


console_welcome=: 0 : 0

J HTTP Server - init OK

Ctrl+c here signals an interrupt to J.

Browse to: http://<LOCAL>:<PORT>/jijx
)

console_failed=: 0 : 0

J HTTP Server - init failed

Port <PORT> already in use by JHS or another service.

If JHS is serving the port, close this task and use the running server.

If JHS server is not working, close it, close this task, and restart.

See file "~addons/ide/jhs/config/jhs.cfg" on using another PORT.
)

NB. html config parameters
configdefault=: 3 : 0
PORT=: 65001       NB. private port range 49152 to 65535
USER=: ''          NB. 'john' - login
PASS=: ''          NB. 'abra' - login
TIPX=: ''          NB. tab title prefix - distinguish sessions
AUTO=: (UNAME-:'Linux')*:0-:2!:5'DISPLAY'  NB. startup browse to http:/localhost:PORT/jijx

PC_FONTFIXED=:     '"courier new","courier","monospace"'
PC_FONTVARIABLE=:  '"sans-serif"'
PC_BOXDRAW=:       0        NB. 0 utf8, 1 +-, 2 oem

PC_FM_COLOR=:      'black'  NB. formatted output
PC_ER_COLOR=:      'red'    NB. error
PC_LOG_COLOR=:     'blue'   NB. log user input
PC_SYS_COLOR=:     'purple' NB. system error
PC_FILE_COLOR=:    'green'  NB. 1!:! file output
)

NB. undocumneted config parameters
PC_RECVSLOW=:     0        NB. 1 simulates slow recv connection
PC_SENDSLOW=:     0        NB. 1 simulates slow send connection
PC_LOG=:          0        NB. 1 to log events
PC_RECVBUFSIZE=:  10000    NB. size of recv buffer
PC_RECVTIMEOUT=:  5000     NB. seconds for recv timeout
PC_SENDTIMEOUT=:  5000     NB. seconds for send timeout

USERNAME=: '' NB. JUM left over

NB. JUM - no longer used
NB. fix userfolders for username y
NB. adjust SystemFolders for multi-users in single account
fixuf=: 3 : 0
USERNAME=: y
if. 0=#y do. return. end.
t=. SystemFolders_j_
a=. 'break';'config';'temp';'user'
i=. ({."1 t)i.a
p=. <'~user/jhs/',y
n=. a,.jpath each p,each '/break';'/config';'/temp';''
SystemFolders_j_=: n i} t
(":2!:6'') 1!:2 <jpath'~user/.jhspid'
1!:44 jpath'~user' NB. cd
)

NB. similar to startup_console in boot.ijs
startupjhs=: 3 : 0
f=. jpath '~config/startup_jhs.ijs'
if. 1!:4 :: 0: <f do.
  try.
    load__ f
  catch.
    smoutput 'An error occurred when loading startup script: ',f
  end.
  cocurrent 'jhs'
end.
)

dobind=: 3 : 0
sdcleanup_jsocket_''
cloexec SKLISTEN=: 0 pick sdcheck_jsocket_ sdsocket_jsocket_''
if. IFUNIX do.  sdsetsockopt_jsocket_ SKLISTEN;SOL_SOCKET_jsocket_;SO_REUSEADDR_jsocket_;2-1 end.
sdbind_jsocket_ SKLISTEN;AF_INET_jsocket_;y;PORT
)

nextport=: 3 : 0
while.
 PORT=: >:PORT
 r=.dobind y
 shutdownJ_jsocket_ SKLISTEN ; 2
 sdclose_jsocket_ ::0: SKLISTEN
 sdcleanup_jsocket_''
 erase'SKLISTEN_jhs_'
 10048=r
do. end.
)

addOKURL=: 3 : 0
rmOKURL y
OKURL=: OKURL,<y
)

rmOKURL=: 3 : 0
OKURL=: OKURL-.<y
)

lcfg=: 3 : 0
try. load jpath y catch. ('load failed: ',y) assert 0 end.
NB. current locale possibly changed
cocurrent 'jhs'
)

NB. simplified config
jhscfg=: 3 : 0
configdefault''
if. 3=nc<'config' do. config'' end.
'PORT invalid' assert (PORT>49151)*.PORT<2^16
'PASS invalid' assert 2=3!:0 PASS
if. _1=nc<'USER' do. USER=: '' end. NB. not in JUM config
'USER invalid' assert 2=3!:0 USER
PASS=: ,PASS
USER=: ,USER
if. _1=nc<'TIPX' do. TIPX=: '' end.
TIPX=: ,TIPX
TIPX=: TIPX,(0~:#TIPX)#'/'
'TIPX invalid' assert 2=3!:0 TIPX
if. _1=nc<'TARGET' do. TARGET=: '_blank' end.
if. _1=nc<'OKURL' do. OKURL=: '' end. NB. URL allowed without login
)

NB. SO_REUSEADDR allows server to kill/exit and restart immediately
init=: 3 : 0
'already initialized' assert _1=nc<'SKLISTEN'
IFJHS_z_=: 1
if. 3=nc<'getignore_j_' do. getignore_j_'' end. NB. not defined in 801
canvasnum_jhs_=: 1
jhscfg''
PATH=: jpath'~addons/ide/jhs/'
NB. IP=: getexternalip''
LOCALHOST=: >2{sdgethostbyname_jsocket_'localhost'
logappfile=: <jpath'~user/.applog.txt' NB. username
SETCOOKIE=: 0
NVDEBUG=: 0 NB. 1 shows NV on each input
NB. leading &nbsp; for Chrome delete all
LOG=: jmarka,'<div>&nbsp;<font style="font-size:20px; color:red;" >J Http Server</font></div>',jmarkz
LOGN=: ''
PDFOUTPUT=: 'output pdf "',(jpath'~temp\pdf\plot.pdf'),'" 480 360;'
DATAS=: ''
PS=: '/'
cfgfile=. jpath'~addons/ide/jhs/config/jhs_default.ijs'
r=. dobind''
if. r=10048 do.
 echo console_failed hrplc 'PORT';":PORT
 'JHS init failed'assert 0
end.
sdcheck_jsocket_ r
sdcheck_jsocket_ sdlisten_jsocket_ SKLISTEN,5 NB. queue length
SKSERVER_jhs_=: _1
boxdraw_j_ PC_BOXDRAW
smoutput console_welcome hrplc 'PORT LOCAL';(":PORT);LOCALHOST
startupjhs''
cookie=: 'jcookie=',":{:6!:0''
input_jfe_=: input_jhs_  NB. only use jfe locale to redirect input/output
output_jfe_=: output_jhs_

if. AUTO do.
 url=. 'http://localhost:PORT/jijx'rplc'PORT';":PORT
 try.
  select. UNAME
  case. 'Win'    do. shell_jtask_'start ',url
  case. 'Linux'  do.
   if. (0;'') -.@e.~ <2!:5 'DISPLAY' do.
    assert. *#t=. dfltbrowser_j_''
    2!:1 t,' ',url,' >/dev/null 2>&1 &'
   end.
  case. 'Darwin' do. 2!:0'open ',url,' &'
  end.
 catch.
  echo'AUTO failed: ',url
 end.
end.
jfe 1
)

NB. load rest of JHS core
load__'~addons/ide/jhs/util.ijs'
load__'~addons/ide/jhs/utilh.ijs'
load__'~addons/ide/jhs/utiljs.ijs'
load__'~addons/ide/jhs/sp.ijs'
load__'~addons/ide/jhs/tool.ijs'
load__'~addons/ide/jhs/d3.ijs'
load__'~addons/ide/jhs/debug.ijs'

NB. load addons, but do not fail init if not found
load__ :: ['~addons/convert/json/json.ijs'

stub=: 3 : 0
'jev_get y[load''~addons/ide/jhs/',y,'.ijs'''
)

NB. app stubs to load app file
jev_get_jijx_=:    3 : (stub'jijx')
jev_get_jfile_=:   3 : (stub'jfile')
jev_get_jfiles_=:  3 : (stub'jfiles')
jev_get_jijs_=:    3 : (stub'jijs')
jev_get_jfif_=:    3 : (stub'jfif')
jev_get_jal_=:     3 : (stub'jal')
jev_get_jdemo_=:   3 : (stub'jdemo')
jev_get_jlogin_=:  3 : (stub'jlogin')
jev_get_jfilesrc_=:3 : (stub'jfilesrc')

NB. simple wget with sockets - used to get google charts png

NB. jwget 'host';'file'
NB. jwget 'chart.apis.google.com';'chart?&cht=p3....'
NB. simplistic - needs work to be robust and general
NB. JHS get/put and jwget should probably share code
wget=: 3 : 0
'host file'=. y
ip=. >2{sdgethostbyname_jsocket_ host
try.
 sk=. >0{sdcheck_jsocket_ sdsocket_jsocket_''
 sdcheck_jsocket_ sdconnect_jsocket_ sk;AF_INET_jsocket_;ip;80
 t=. gettemplate rplc 'FILE';file
 while. #t do. t=.(>sdcheck_jsocket_ t sdsend_jsocket_ sk,0)}.t end.
 h=. d=. ''
 cl=. 0
 while. (0=#h)+.cl>#d do. NB. read until we have header and all data
  z=. sdselect_jsocket_ sk;'';'';5000
  assert sk e.>1{z NB. timeout
  'c r'=. sdrecv_jsocket_ sk,10000,0
  assert 0=c
  assert 0~:#r
  d=. d,r
  if. 0=#h do. NB. get headers
   i=. (d E.~ CRLF,CRLF)i.1 NB. headers CRLF delimited with CRLF at end
   if. i<#d do. NB. have headers
    i=. 4+i
    h=. i{.d NB. headers
    d=. i}.d
    i=. ('Content-Length:'E. h)i.1
    assert i<#h
    t=. (15+i)}.h
    t=. (t i.CR){.t
    cl=. _1".t
    assert _1~:cl
   end.
  end.
 end.
catch.
 shutdownJ_jsocket_ sk ; 2
 sdclose_jsocket_ ::0: sk
 smoutput 13!:12''
 'get error' assert 0
end.
shutdownJ_jsocket_ sk ; 2
sdclose_jsocket_ ::0: sk
h;d
)

jwget_z_=: wget_jhs_

NB. jwget template
gettemplate=: toCRLF 0 : 0
GET /FILE HTTP/1.1
Host: 127.0.0.1
Accept: image/gif,image/png,*/*
Accept-Language: en-ca
UA-CPU: x86
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0; WOW64; SLCC1; .NET CLR 2.0.50727; Media Center PC 5.0; .NET CLR 3.5.30729; .NET CLR 3.0.30729)
Connection: Keep-Alive

)

NB. return first ip address that is not localhost
getlanip=: 3 : 0
if. IFWIN do.
 r=. deb each<;._2 spawn_jtask_'ipconfig'
 r=. ((<'IPv4 Address')=12{.each r)#r
 r=. (>:;r i.each':')}.each r
elseif. UNAME-:'Darwin' do.
 r=. <;._2[2!:0'ifconfig'
 r=. deb each r rplc each <TAB;' ' 
 r=. ((<'inet ')=5{.each r)#r
 r=. 5}.each r
elseif. 1 do.
 r=. deb each<;._2[2!:0'ifconfig'
 r=. ((<'inet addr:')=10{.each r)#r
 r=. 10}.each r
end.
r=. deb each r
r=. deb each(r i.each' '){.each r
r=. r-.<'127.0.0.1'
'no lan ip' assert 0<#r
if. 1<#r do. echo 'multiple lan ips: ',LF,;LF,~each' ',each r end.
;{.r
)

getexternalip=: 3 : 0
z=. >2{sdgethostbyname_jsocket_ >1{sdgethostname_jsocket_''
if. ('255.255.255.255'-:z) +. ('127.0.'-:6{.z) +. '192.168.'-:8{.z do.
 if. UNAME-:'Linux' do.
  a=. , 2!:0 ::_1: 'wget -q --timeout=7 -O - http://www.checkip.org/'
 elseif. UNAME-:'Darwin' do.
  a=. , 2!:0 ::_1: 'curl -s --max-time 7 -o - http://www.checkip.org/'
 elseif. UNAME-:'Win' do.
  a=. , spawn_jtask_ '"',(jpath '~tools/ftp/wget.exe'),'" -q --timeout=7 -O - http://www.checkip.org/'
 elseif. do.
  a=. ,_1
 end.
 if. 1 e. r=. '<h1>Your IP Address:' E. a do.
   z=. ({.~ i.&'<') (}.~ [: >: i.&'>') (20+{.I.r)}.a
 end.
end.
z
)

NB. shutdown JHS
NB. y: integer return code for 2!:55
NB.    ''  just shutdown JHS, J not exit
shutdown=: 3 : 0
 shutdownJ_jsocket_ SKLISTEN ; 2
 sdclose_jsocket_ ::0: SKLISTEN
 sdcleanup_jsocket_''
 erase'SKLISTEN'
 IFJHS_z_=: 0
 jfe 0
 2!:55^:(''-.@-:y)y
) 
