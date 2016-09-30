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

Listening socket can bind localhost or any. What about lan?

Localhost is relatively secure.
Firewalls provide some any protection.

Localhost is relatively secure and gains little from login.

Non-localhost should require a login.

Login is provided by a cookie.
The cookie is set in the response to providing a password.
That cookie is then included in the header of all requests
and is validated by the server.

The cookie is non-persistent and is deleted when browser closes.
New tabs do not need to login, but a new browser does.

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

NB. J needs input - y is prompt - '' '   ' '      '
input=: 3 : 0
logapp 'jhs input prompt: ',":#y
try.
if. _1~:SKSERVER do. try. ".'urlresponse_',URL,'_ y' catch. end. end. NB. jijx
if. _1~:SKSERVER do. jbad'' end.
getdata'' NB. get and parse http request
if. 1=NVDEBUG do. smoutput seebox NV end. NB. HNV,NV
if. (-.(<URL)e.boxopen OKURL)*.(0~:#PASS)*.(1~:+/cookie E. gethv'Cookie:')*.-.LHOK*.PEER-:LOCALHOST
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
if. 5=x do. jhrajax 'Your J HTTP Server has exited.<br/><div id="prompt" class="log">&nbsp;&nbsp;&nbsp;</div>'[PROMPT_jhs_=:'   ' end.
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
jev=: 3 : 0
try.
 ".t=. 'ev_',(getv'jmid'),'_',(getv'jtype'),' 0'
catch.
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
 SKSERVER_jhs_=: 0 pick sdcheck_jsocket_ sdaccept_jsocket_ SKLISTEN

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
 sdclose_jsocket_ SKSERVER
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

NB. z local utilities

dbon_z_=: 3 : 0
13!:15 'smoutput dbes dbestack_z_=:13!:18'''''
9!:27 '13!:0[1'
9!:29 [1
i.0 0
)

dboff_z_=: 3 : 0
13!:15 ''
9!:27 '13!:0[0'
9!:29 [1
i.0 0
)

dbcutback_z_=: 13!:19
dbstep_z_=:    13!:20
dbstepin_z_=:  13!:21
dbstepout_z_=: 13!:22

NB. display numbered explicit defn
dbsd_z_=: 3 : 0
if. -.1 2 3 e.~nc<y do. 'not an explicit definition' return. end.
raw=. 5!:5<y
t=.<;.2 LF,~raw
if. 1=#t do. '0 ',raw return. end.
i=.t i.<':',LF
if. ('3'={.raw)*.i~:#t do.
 j=. (_1,i.<:i),_1,(i.<:<:(#t)-i),_1
else.
 j=. _1,(i._2+#t),_1
end.
n=. ":each<"0 j
n=. a: ((n=<'_1')#i.#n)} n
n=. <"1 ' ',.~' ',.~>n
;n,each t
)

NB. debug stop manager
NB. dbsm'name'     - display numbered explicit defn
NB. dbsm'~...'     - remove stops starting with ...
NB. dbsm'name n:n' - add stops
NB. dbsm''         - display stops
dbsm_z_=: 3 : 0
if. ('~'~:{.y)*.1=#;:y do. dbsd y return. end.
if.'~'={.y do.
 s=. deb each<;._2 (dbsq''),';'
 a=. }.y
 s=. (-.(<a)=(#a){.each s)#s
else.
 s=. deb each<;._2 (dbsq''),y,';'
end.
s=. ~./:~(s~:a:)#s
dbss ;s,each<' ; '
dbsq''
)

NB. show execution stack as set by last supension
dbes_z_=: 3 : 0
len=. >./dbestack i."1 ' '
t=. |."2[dbestack
r=. ''
while. #t do.
 d=. }.dtb{.t
 d=. (len>.#d){.d
 t=. }.t
 if. ' '~:1{d do.
  n=. dltb}.{.t
  if. 2~:#t do. n=. n rplc '    ';'' end.
  r=. r,<d,n
  t=. }.t
 else.
  r=. r,<d rplc '    ';''
 end.
end.
'_',(>coname''),'_',LF,;|.r,each LF
)

open_z_=: 3 : 0
t=. (JIJSAPP_jhs_,'?mid=open&path=',jpath spf y)jhref_jhs_ spf y
jhtml'<div contenteditable="false">',t,'</div>'
)

jlogoff_z_=: 3 : 'htmlresponse_jhs_ hajaxlogoff_jhs_'

NB. one very long line as LF is <br>
jhtml_z_=: 3 : 0
a=. 9!:36''
9!:37[ 4$0,1000+#y NB. allow lots of html formatted output
smoutput jmarka_jhs_,y,jmarkz_jhs_
9!:37 a
i.0 0
)

jaudio_z_=: 3 : 0
assert fexist y
jhtml'<audio controls="controls"><source src="',y,'" type="audio/mp3">not supported</audio>'
)

NB. eval javascript sentences
NB. starting without ';' is evaluated only in ajax
NB. starting with ';' is evaluated in ajax and in refresh
jjs_z_=:3 : 0
jhtml jmarkjsa_jhs_,y,jmarkjsz_jhs_
)

NB. eval javascript sentences - eval again in refresh
jjsx_z_=: 3 : 0
jjs';',y
)

NB. pop-up window - jtable/jd3
windowopen=: 4 : 0
m=. x,' pop-up blocked\nadjust browser settings to allow localhost pop-up\nsee jhelp section pop-up'
jjs'if(null==window.open("<X>","<Y>"))alert("<M>");'rplc '<X>';x;'<Y>';y;'<M>';m
)

jtable_z_=: 3 : 0
require'~addons/ide/jhs/jtable.ijs'
't n'=. y
n=. dltb n
n=. n,>('_'={:n){'__';''
validate_jtable_ n
(t,'_ev_body_load_data_jtable_')=: n
'jtable' windowopen_jhs_ t
)

NB. jd3 plots

jd3doc_z_=: 0 : 0
jd3'help'
jd3'help options'

jd3'reset'         - jd3x=: '' - clear options  
jd3'title TITLE'   - add title option to jd3x

jd3 tab;options;data
 tab     - browser tab title
 options - list of jd3 options
 data    - data to plot

example:
jd3'reset'
jd3'type line'
jd3'title My Data'
jd3'legend ["line one","line two","line three"]'
jd3'line_plot';jd3x;?3 4$100

jd3'type bar'
jd3'label ["a","b","c","d"]'
jd3'bar_plot';jd3x;?3 4$100

jd3'type pie'
jd3'pie_plot';jd3x;?4$100
)

jd3docoptions_z_=: 0 : 0
jd3'option arg'    - add option to jd3x__
 type        line      - line or pie or bar   
 title       Good Data
 titlesize   12pt      - html font size
 minh        100       - min graph pixel height
 maxh        300       - max graph pixel height
 linewidth   2
 barwidth    20
 legend      ["a","b","c"]
 lable       ["s","d","f"]
 header      how now<hr>
 header_css  "font-size":"24pt","margin-left":50
 footer      <hr>how now
 footer_css  "font-size":"24pt","margin-left":50
 data        formatted data - as formatted by jd3data
)

jd3_z_=: 3 : 0
require'~addons/ide/jhs/jd3.ijs'
if. 'literal'-:datatype y do.
 i=. y i.' '
 c=. dltb i{.y
 a=. dltb i}.y
 b=. '"',a,'"'
 select. c
 fcase.'' do.
 case.'help' do.
  ;(a-:'options'){jd3doc;jd3docoptions return.
 case.'reset' do. jd3x=: '' return.
 case.'header' do.
  t=. '$("#ahtml").html(',b,')'
 case.'footer' do.
  t=. '$("#zhtml").html(',b,')'
 case.'header_css' do.
  t=. '$("#ahtml").css({',a,'})'
 case.'footer_css' do.
  t=. '$("#zhtml").css({',a,'})'
 case. ;:'type title titlesize minh maxh' do.
  t=. c,'=',b
 case. ;:'legend label' do.
  t=. c,'=',a
 case. do. ('jd3 unknown option: ',y)assert 0  
 end.
 jd3x=: jd3x,t,LF
 i.0 0
 return.
end.
if. 3=#y do.
 't p d'=. y
  p=. p,jd3data d
else.  
 't p'=. y
end. 
(t,'_ev_body_load_data_jd3_')=: p
'jd3' windowopen_jhs_ t
)

jd3data_z_=: 3 : 0
d=. ":each <"1 y
d=. d rplc each <' ';','
d=. d rplc each <'_';'-'
d=. ']',~each '[',each d
']',~'data=[',;d,each','
)

NB. somewhat unique query string - avoid cache - not quaranteed to be unique!
jhsuqs_z_=: 3 : 0
canvasnum_jhs_=: >:canvasnum_jhs_
'?',((":6!:0'')rplc' ';'_';'.';'_'),'_',":canvasnum_jhs_
)

NB. f file.png
jhspng_z_=: 3 : 0
d=. fread y
w=. 256#.a.i.4{.16}.d
h=. 256#.a.i.4{.20}.d
t=. '<img width=<WIDTH>px height=<HEIGHT>px src="<FILE><UQS>" ></img>'
jhtml t hrplc_jhs_ 'WIDTH HEIGHT FILE UQS';w;h;y;jhsuqs''
)

NB. TARGET f URL
jhslink_z_=: 3 : 0
'_blank' jhslink y
:
t=. '<a href="<REF><UQS>" target="<TARGET>" class="jhref" ><TEXT></a>'
t=. t hrplc_jhs_ 'TARGET REF UQS TEXT';x;y;(jhsuqs'');y
jhtml'<div contenteditable="false">',t,'</div>'
)

NB. TARGET f URL
jhsshow_z_=: 3 : 0
'_blank' jhsshow y
:
jjs 'window.open("',(y,jhsuqs''),'","',x,'");'
)

plotjijx_z_=: 3 : 0
canvasnum_jhs_=: >:canvasnum_jhs_
canvasname=. 'canvas',":canvasnum_jhs_
d=. fread y
c=. (('<canvas 'E.d)i.1)}.d
c=. (9+('</canvas>'E.c)i.1){.c
c=. c rplc 'canvas1';canvasname
d=. (('function graph()'E.d)i.1)}.d
d=. (('</script>'E.d)i.1){.d
d=. d,'graph();'
d=. d rplc'canvas1';canvasname
jhtml c
jjsx d
)
NB. f type;window;width height[;output]
NB. type selects case in plotcanvas/plotcairo
plotdef_z_=: 3 : 0
if. 'cairo'-:_1{::y=. 4{.y,<'canvas' do.
 'CAIRO_DEFSHOW_jzplot_ CAIRO_DEFWINDOW_jzplot_ CAIRO_DEFSIZE_jzplot_ JHSOUTPUT_jzplot_'=: y
else.
 'CANVAS_DEFSHOW_jzplot_ CANVAS_DEFWINDOW_jzplot_ CANVAS_DEFSIZE_jzplot_ JHSOUTPUT_jzplot_'=: y
NB. default output
 JHSOUTPUT_jzplot_=: 'canvas'
end.
i.0 0
)

plotcanvas_z_=: 3 : 0
f=. '~temp/plot.html' NB. CANVAS_DEFFILE
d=. fread f
d=. d rplc'<h1>plot</h1>';''
d=. d rplc'#canvas1 { margin-left:80px; margin-top:40px; }';'#canvas1{margin-left:0; margin-top:0;}'
d fwrite f
w=. CANVAS_DEFWINDOW_jzplot_
select. CANVAS_DEFSHOW_jzplot_
 case. 'show' do. w jhsshow f
 case. 'link' do. w jhslink f
 case. 'jijx' do. plotjijx f
 case. 'none' do.
 case.        do. plotjijx f
end.
i.0 0
)

plotcairo_z_=: 3 : 0
f=. '~temp/plot.png' NB. CAIRO_DEFFILE
w=. CAIRO_DEFWINDOW_jzplot_
select. CAIRO_DEFSHOW_jzplot_
 case. 'show' do. w jhsshow f
 case. 'link' do. w jhslink f
 case. 'jijx' do. jhspng f
 case. 'none' do.
 case.        do. jhspng f
end.
i.0 0
)

jhsrefresh_z_=: 3 : 0
y,'?refresh=',(":6!:0'')rplc' ';'-'
)

jbd_z_=: 3 : '9!:7[y{Boxes_j_' NB. select boxdraw (PC_BOXDRAW)

decho_z_=: echo_z_

NB. JHS echo to console - should be in JHS core or even stdlib
echoc_z_=: 3 : 0
if. IFJHS do.
 try.
  jbd 1
  jfe_jhs_ 0
  echo y
 catch.
 end.
 jfe_jhs_ 1
 jbd 0
else.
 echo y
end. 
)

dechoc_z_=: echoc_z_
NB. toggle jfe behavior
jfe=: 3 : 0
15!:16 y
i.0 0
)

console_welcome=: 0 : 0

J HTTP Server - init OK

Requires HTML 5 browser with javascript.

On many systems localhost is the same as <LOCAL>. 

Ctrl+c here signals an interrupt to J.

A : separates ip address from port.
<REMOTE>
Start a web browser on this machine and enter URL:
   http://<LOCAL>:<PORT>/jijx
)

remoteaccess=: 0 : 0

Access from another machine:
   http://SERVER_IP_ADDRESS:<PORT>/jijx
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
LHOK=: 1           NB. 0 if localhost requires user/pass login
BIND=: 'localhost' NB. 'any'  - access from any machine
USER=: ''          NB. 'john' - login
PASS=: ''          NB. 'abra' - login
TIPX=: ''          NB. tab title prefix - distinguish sessions

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
SKLISTEN=: 0 pick sdcheck_jsocket_ sdsocket_jsocket_''
if. IFUNIX do.
  ((unxlib 'c'),' fcntl i i i i') cd SKLISTEN,F_SETFD_jsocket_,FD_CLOEXEC_jsocket_
  sdsetsockopt_jsocket_ SKLISTEN;SOL_SOCKET_jsocket_;SO_REUSEADDR_jsocket_;2-1
end.
sdbind_jsocket_ SKLISTEN;AF_INET_jsocket_;y;PORT
)

nextport=: 3 : 0
while.
 PORT=: >:PORT
 r=.dobind y
 sdclose_jsocket_ SKLISTEN
 sdcleanup_jsocket_''
 erase'SKLISTEN_jhs_'
 10048=r
do. end.
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
'BIND invalid' assert +./(<BIND)='any';'localhost'
'LHOK invalid' assert +./LHOK=0 1
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
BIND=: >(BIND-:'any'){'127.0.0.1';''
)

NB. SO_REUSEADDR allows server to kill/exit and restart immediately
NB. FD_CLOEXEC prevents inheritance by new tasks (JUM startask)
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
r=. dobind BIND
if. r=10048 do.
 echo console_failed hrplc 'PORT';":PORT
 'JHS init failed'assert 0
end.
sdcheck_jsocket_ r
sdcheck_jsocket_ sdlisten_jsocket_ SKLISTEN,5 NB. queue length
SKSERVER_jhs_=: _1
boxdraw_j_ PC_BOXDRAW
remote=. >(BIND-:''){'';remoteaccess hrplc 'PORT';":PORT
smoutput console_welcome hrplc 'PORT LOCAL REMOTE';(":PORT);LOCALHOST;remote
startupjhs''
if. 0~:#PASS do.
 cookie=: 'jcookie=',0j4":{:6!:0''
else.
 cookie=: ''
end.
input_jfe_=: input_jhs_  NB. only use jfe locale to redirect input/output
output_jfe_=: output_jhs_
jfe 1
)

NB. load rest of JHS core
load__'~addons/ide/jhs/utilh.ijs'
load__'~addons/ide/jhs/utiljs.ijs'
load__'~addons/ide/jhs/sp.ijs'

stub=: 3 : 0
'jev_get y[load''~addons/ide/jhs/',y,'.ijs'''
)

NB. app stubs to load app file
jev_get_jijx_=:    3 : (stub'jijx')
jev_get_jijxaz_=:  3 : (stub'jijxaz')
jev_get_jfile_=:   3 : (stub'jfile')
jev_get_jfiles_=:  3 : (stub'jfiles')
jev_get_jijs_=:    3 : (stub'jijs')
jev_get_jfif_=:    3 : (stub'jfif')
jev_get_jal_=:     3 : (stub'jal')
jev_get_jtable_=:  3 : (stub'jtable')
jev_get_jhelp_=:   3 : (stub'jhelp')
jev_get_jdemo_=:   3 : (stub'jdemo')
jev_get_jlogin_=:  3 : (stub'jlogin')
jev_get_jijxh_=:   3 : (stub'jijxh')
jev_get_jijxm_=:   3 : (stub'jijxm')
jev_get_jfilesrc_=:3 : (stub'jfilesrc')
jev_get_jijxipad_=:3 : (stub'jijxipad')
jev_get_jijsipad_=:3 : (stub'jijsipad')

NB. simple wget with sockets - used to get google charts png

NB. jwget 'host';'file'
NB. jwget 'chart.apis.google.com';'chart?&cht=p3....'
NB. simplistic - needs work to be robust and general
NB.! JHS get/put and jwget should probably share code
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
 sdclose_jsocket_ sk
 smoutput 13!:12''
 'get error' assert 0
end.
sdclose_jsocket_ sk
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

NB. viewmat - previously in jgcp - should come from addon eventually
coclass'jgcp'
NB. viewmat stuff - subset borrowed from viewmat addon

viewmat=: 3 : 0
t=. (<6#16)#: each <"0>1{''getvm_jgcp_ y
t=. '#',each t{each <'0123456789abcdef'
a=. (<'<font ',LF,'style="background-color:'),each t
a=. a,each (<'; color:'),each t
a=. a,each <';">ww</font>'
jhtml ;a,.<'<br>'
)

3 : 0
if. 803 > ". '.'-.~ 4{. LF -.~ 1!:1<jpath '~system/config/version.txt' do.
  viewmat_z_=: viewmat_jgcp_
end.
''
)

finite=: x: ^: _1
intersect=: e. # [
citemize=: ,: ^: (2: > #@$)
rndint=: <.@:+&0.5
tomatrix=: (_2 {. 1 1 , $) $ ,

delinf=: 3 : 0
if. +:/ _ __ e. ,y do. y return. end.
sc=. 0.1
a=. (,y) -. _ __
max=. >./a
min=. <./a
ext=. sc * max - min
(min-ext) >. y <. max+ext
)

NB. =========================================================
NB.*gethue v generate color from color set
NB. x is color set
NB. y is values from 0 to 1, selecting color
gethue=: 4 : 0
y=. y*<:#x
b=. x {~ <.y
t=. x {~ >.y
k=. y-<.y
(t*k) + b*-.k
)

NB. =========================================================
NB. getvm
NB.
NB. form: hue getvm data [;title]
NB.
NB. hue may be empty, in which case a default is used
NB. hue may be a N by 3 matrix of colors or a vector
NB.     of RGB integers.
NB.
NB. returns:
NB.   original data
NB.   scaled data matrix
NB.   angle (if any)
NB.   title
getvm=: 4 : 0
'dat tit'=. 2 {. boxopen y
tit=. ": tit
tit=. tit, (0=#tit) # 'viewmat'
'mat ang'=. x getvm1 dat
dat ; mat ; ang ; tit
)

NB. =========================================================
getvm1=: 4 : 0
hue=. x
mat=. y
ang=. ''

NB. ---------------------------------------------------------
if. 2 > #$hue do.
  hue=. |."1 [ 256 256 256 #: ,hue
end.

NB. ---------------------------------------------------------
select. 3!:0 mat
case. 2;32 do.
  mat=. (, i. ]) mat
case. 16 do.
  ang=. * mat
  mat=. delinf | mat
case. do.
  mat=. finite mat
end.

NB. ---------------------------------------------------------
select. #$mat
case. 0 do.
  mat=. 1 1$mat
case. 1 do.
  mat=. citemize mat
case. 3 do.
  'mat ang'=. mat
end.

NB. ---------------------------------------------------------
if. */ (,mat) e. 0 1 do.
  if. #hue do.
    h=. <. 0 _1 { hue
  else.
    h=. 0 ,: 255 255 255
  end.
  mat=. mat { h

else.
  if. #hue do.
    h=. hue
  else.
    h=. 255 * #: 7 | 3^i.6
  end.
  val=. ,mat
  max=. >./ val
  min=. <./ val
  mat=. <. h gethue (mat - min) % max - min
end.

mat=. mat +/ .* 65536 256 1

mat ; ang

)
