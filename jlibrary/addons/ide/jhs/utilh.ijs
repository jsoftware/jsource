NB. html templates and utilities
coclass'jhs'

INC=: CSS=: JS=: HBS=: '' NB. overidden in app locale
NOCACHE=: 0                NB. use cached js files
NOPOPUP=: 0                NB. use popups

INC_jquery=: 0 : 0
~addons/ide/jhs/js/jquery/smoothness/jquery-ui.custom.css
~addons/ide/jhs/js/jquery/jquery-2.0.3.min.js
~addons/ide/jhs/js/jquery/jquery-ui.min.js
~addons/ide/jhs/js/jquery/jquery-ui.css"
)

INC_d3=: INC_jquery,0 : 0
~addons/ide/jhs/js/d3/d3.css
~addons/ide/jhs/js/d3/d3.min.js
)

INC_d3_basic=: INC_d3,0 : 0
~addons/ide/jhs/js/jsoftware/d3_basic.js
)

INC_handsontable=: INC_jquery, 0 : 0
~addons/ide/jhs/js/handsontable/handsontable.full.min.js
~addons/ide/jhs/js/handsontable/handsontable.full.min.css
)

INC_handsontable_basic=: INC_handsontable NB. no jsoftware stuff yet


NB.framework styles for all pages
CSSCORE=: 0 : 0
*{font-family:<PC_FONTVARIABLE>;}
*.jcode{font-family:<PC_FONTFIXED>;white-space:pre;}
*.jhab:hover{cursor:pointer;color:black;background:#ddd;}
*.jhab{text-decoration:none;}
*.jhmab:hover{cursor:pointer;}
*.jhmab{text-decoration:none;color:black;}
*.jhmg:hover{cursor:pointer;}
*.jhmg:visited{color:black;}
*.jhmg{color:black;background:#eee;}
*.jhmg{text-decoration:none;}
*.jhml{color:black;}
*.jhml:visited{color:black;}
*.jhsel{background-color:buttonface;font-family:<PC_FONTFIXED>;}
body{margin:0;}
div{padding-left:2px;}
.menu li{
 display:block;white-space:nowrap;
 padding:2px;color:#000;background:#eee;
 font-family:<PC_FONTFIXED>;
}
.menu a{font-family:<PC_FONTFIXED>;}
.menu a:hover{cursor:pointer;color:#000;background:#ddd;width:100%;}
.menu span{float:left;position:relative;}
.menu ul{
 position:absolute;top:100%;left:0%;display:none;
 list-style:none;border:1px black solid;margin:0;padding:0;
}
#jresizeb{overflow:scroll;border:solid;border-width:1px;clear:left;}
#status-busy{
 position: absolute; top: 0px; left: 70%; border: thick red solid; margin: 20px; padding: 10px;
 display: none; background: white;
 text-align:center;
}
#close{position:absolute;top:0;left:0;width:20px;height:20px;font-size:16px;background-color:firebrick;color:white;font-weight:bold;}
)

NB.#close{position:absolute;top:0;left:0;width:20px;height:20px;font-size:16px;background-color:firebrick;color:white;font-weight:bold;}

NB. extra html - e.g. <script .... src=...> - included after CSS and before JSCORE,JS
HEXTRA=: '' 

NB. core plus page styles with config replaces
NB. apply outer style tags after removing inner ones
css=: 3 : 0
t=. 'PC_FONTFIXED PC_FONTVARIABLE PC_FM_COLOR PC_ER_COLOR PC_LOG_COLOR PC_SYS_COLOR PC_FILE_COLOR'
t=. (CSSCORE,y) hrplc t;PC_FONTFIXED;PC_FONTVARIABLE;PC_FM_COLOR;PC_ER_COLOR;PC_LOG_COLOR;PC_SYS_COLOR;PC_FILE_COLOR
'<style type="text/css">',LF,t,'</style>',LF
)

NB. core plus page js
js=: 3 : 0
'<script type="text/javascript">',LF,'var TARGET="',TARGET,'"',LF,JSCORE,y,'</script>'
)

seebox=: 3 : 0
1 seebox y
:
;((x+>./>#each y){.each "1 y),.<LF
)

seehtml=: 3 : 0
y rplc '<';LF,'<'
)

NB. form template - form, hidden handler sentence, hidden button for form enter (ff)
formtmpl=: 0 : 0 -. LF
<form id="j" name="j" method="post" action="<LOCALE>">
<input type="hidden" name="jdo"     value="">
<input type="hidden" name="jlocale" value="<LOCALE>">
<input type="hidden" name="jid"     value="">
<input type="hidden" name="jtype"   value="">
<input type="hidden" name="jmid"    value="">
<input type="hidden" name="jsid"    value="">
<input type="submit" value="" onclick="return false;" style="display:none;width:0px;height:0px;border:none">
)

NB. form urlencoded has + for blank
jurldecodeplus=: 3 : 0
jurldecode y rplc '+';' '
)

jurldecode=: 3 : 0
t=. ~.<"1 (1 2 +"1 0 (y='%')#i.#y){y
d=. ".each(<'16b'),each tolower each t
d=. d{each <a.
t=. '%',each t
,t,.d
y rplc ,t,.d
)

jurlencode=: 3 : 0
,'%',.(16 16#:a.i.,y){'0123456789ABCDEF'
)

jmarkjsa=: '<!-- j js a --><!-- '
jmarkjsz=: ' --><!-- j js z -->'

jmarka=:     '<!-- j html output a -->'
jmarkz=:     '<!-- j html output z -->'
jmarkc=: #jmarka


NB. unique query string - avoid cache
uqs=: 3 : 0
canvasnum=: >:canvasnum
'?',((":6!:0'')rplc' ';'_';'.';'_'),'_',":canvasnum
)

NB. output starting with jmarka and ending with jmarkz,LF
NB.  is assumed to be html and is not touched
jhtmlfroma=: 3 : 0
if. (jmarka-:jmarkc{.y)*.jmarkz-:(-jmarkc){.}:y do. y return. end.
jhfroma y
)

bad=: 1{a. NB. this character hangs the browser

jhfroma=: 3 : 0
y rplc '<';'&lt;';'>';'&gt;';'&';'&amp;';'"';'&quot;';CRLF;'<br>';LF;'<br>';CR;'<br>';' ';'&nbsp;';bad;''
)

NB. app did not send response - send one now
jbad=: 3 : 0
echo NV
smoutput'*** response not sent for ',URL
if. METHOD-:'get' do.
 htmlresponse html409 NB. conflict - not working properly - reload
 smoutput'*** html409 Conflict'
else.
 htmlresponse html409 NB. conflict - not working properly - reload
 smoutput'*** html409 Conflict'
end.  
)

htmlresponse=: 3 : 0
logapp'htmlresponse'
NB. y 1!:2<jpath'~temp/lastreponse.txt'
LASTTS=: 6!:1''
putdata LASTRESPONSE=: y
shutdownJ_jsocket_ SKSERVER ; 2
sdclose_jsocket_ ::0: SKSERVER
SKSERVER_jhs_=: _1
i.0 0 NB. nothing to display if final J result
)

NB. x hrplc 'aa bb cb';daa;dbb;dcc
NB. aa treated as <aa>
NB. numbers converted to string
hrplc=: 4 : 0
x rplc ('<',each (;:>{.y),each'>'),. ": each }.y
)

NB. grid stuff

NB. template gridgen mid;vals
NB. mid*row*col
NB. template has <ID> and <VALUE> (other repaces already done)
jgridgen=: 4 : 0
'mid v'=. y
'r c'=. $v
d=. ''
for_i. i.r do.
 for_j. i.c do.
  id=. mid,'*',(":i),'*',":j
  d=. d,<x hrplc 'ID VALUE';id;j{i{v
 end.
end.
($v)$d
)

NB. create grid for editing named numeric matrix
NB. y is gid;colheads;rowheads;name
NB. gid makes up family of mids - gid_dd gid_ch ...
NB. colheads/rowheads'' gets defaults
NB. rowheads must be column
NB. edit events are gid_dd_enter gid_dd_change
NB. gid_hh contains the edited noun name for the event
NB. gid_vv contains new cell value for the event
jgridnumedit=: 3 : 0
'gid colh rowh name'=. y
try.
 assert. 0=nc <name
 data=. ".name
 assert. (2=$$data)*.(3!:0 data) e. 4 8 16
catch.
 data=. i.2 3
 ".name,'=: data'
end.
'r c'=. $data
if. ''-:colh do. colh=.   ,:(<'C'),each ":each i.c end.
if. ''-:rowh do. rowh=. |:,:(<'R'),each ":each i.r end.
assert. c={:$colh
assert. r=#rowh

mid=. gid,'_dd'
t=. jhtx'<ID>';'<VALUE>';10;mid;' onchange="return jev(event)"'
dd=. t jgridgen mid;<data
mid=. gid,'_ch'
t=. jhtx'<ID>';'<VALUE>';10;'<MID>';'readonly="readonly" tabindex="-1" '
ch=. (t rplc '<MID>';mid) jgridgen mid;<colh
mid=. gid,'_cf'
cf=. (t rplc '<MID>';mid) jgridgen mid;<,:+/data
mid=. gid,'_rh'
rh=. (t rplc '<MID>';mid) jgridgen mid;<rowh
mid=. gid,'_rf'
rf=. (t rplc '<MID>';mid) jgridgen mid;<|:,:+/"1 data
mid=. gid,'_xx'
co=. (t rplc '<MID>';mid) jgridgen mid;<,:,<''
co=. <name
cx=. <'+/'
t=. co,.ch,.cx
m=. rh,.dd,.rf
b=. cx,.cf,.<''
d=. t,m,b
d=. (<'<td>'),each(<'</td>'),~each d
d=. (<'<tr>'),.(<'</tr>'),.~d
d=. ('</table>'),~,('<table id="',gid,'" cellpadding="0" cellspacing="0">'),;d
d=. ((gid,'_hh') jhhidden name),d NB. name of data for this grid
d=. ((gid,'_vv') jhhidden''),d   NB. new value for this grid
)

NB. gid;80px
jgridnumeditcss=: 3 : 0
'gid width'=. y
t=.   '.',gid,'_dd{text-align:right;width:',width,';}',LF 
t=. t,'.',gid,'_ch{text-align:left; width:',width,';}',LF
t=. t,'.',gid,'_cf{text-align:right;width:',width,';}',LF
t=. t,'.',gid,'_rh{text-align:left; width:',width,';}',LF
t=. t,'.',gid,'_rf{text-align:right;width:',width,';}',LF
t=. t,'.',gid,'_xx{text-align:right;width:',width,';}',LF
)

NB. Cache-Control: no-cache

NB. html template <XXX>
NB. TITLE
NB. CSS   - styles
NB. JS    - javascript
NB. BODY  - body
hrtemplate=: toCRLF 0 : 0
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Connection: close

<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title><TITLE></title>
<CSS>
<HEXTRA>
<JS>
</head>
<BODY>
</html>
)

hrxtemplate=: toCRLF 0 : 0
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Connection: close

<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title><TITLE></title>
)

NB. html 204 response (leave the page as is)
html204=: toCRLF 0 : 0
HTTP/1.1 204 OK
Content-Type: text/html; charset=utf-8
Connection: close

)

NB. html 301 response (redirect to another url)
html301=: toCRLF 0 : 0
HTTP/1.1 301 Permanently moved
Location: <NEWURL>
Cache-Control: no-cache
Pragma: no-cache
Expires: 0
Connection: close


)

NB. html 409 Conflict response (J code didn't provide result)
html409=: toCRLF 0 : 0
HTTP/1.1 409 Conflict
Content-Type: text/html; charset=utf-8
Connection: close


get/post request failed<br>
response code 409<br>
application did not produce result<br>
try browsing to url again<br>
additional info in jijx
)

gsrchead=: toCRLF 0 : 0
HTTP/1.1 200 OK
Server: JHS
Last-Modified: Mon, 01 Mar 2010 00:23:24 GMT
Accept-Ranges: bytes
Content-Length: <LENGTH>
Keep-Alive: timeout=15, max=100
Connection: Keep-Alive
Content-Type: <TYPE>

)

gsrcf=: 4 : 0
htmlresponse y,~gsrchead rplc '<TYPE>';x;'<LENGTH>';":#y
)

fsrchead=: toCRLF 0 : 0
HTTP/1.1 200 OK
Server: JHS
Content-Type: <TYPE>

)

NB. html for jajax response
NB. no-cache critical - otherwise we get old
NB. answers to the same question!
hajax=: toCRLF 0 : 0
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Cache-Control: no-cache
Content-Length: <LENGTH>

)

NB. html for ajax response in chunks
hajax_chunk=: toCRLF 0 : 0
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Cache-Control: no-cache
Transfer-Encoding: chunked

)

hajaxlogoff=: toCRLF 0 : 0
HTTP/1.1 403 OK
Content-Type: text/html; charset=utf-8
Cache-Control: no-cache
Content-Length: <LENGTH>
Set-Cookie: jcookie=0

)

NB. jhbs body builders

NB. HBS is LF delimited list of sentences
NB. jhbs returns list of sentence results
jhbs=: 3 : 0
t=. <;._2 y
t=. ;LF,~each jhbsex each t
i=. 1 i.~'</div><div id="jresizeb">'E.t
if. i~:#t do.
 t=. '<div id="jresizea">',t,'</div>'
end.
t=. '<div id="status-busy"><br>server busy<br>event ignored<br><br></div>',t
'<body onload="jevload();" onunload="jevunload();" onfocus="jevfocus();">',LF,(jhform''),LF,t,LF,'</form></body>'
)

jhbsex=: 3 : 0
try.
 t=. }.' ',,".y NB. need lit list
catch.
 smoutput t=.'HBS error:',(>coname''),' ',y
 t=.'<div>',t,'</div>'
end.
t
) 

jmon=: 3 : 0
t=.   ' onblur="return jmenublur(event);"'
t=. t,' onfocus="return jmenufocus(event);"'
t=. t,' onkeyup="return jmenukeyup(event);"'
t=. t,' onkeydown="return jmenukeydown(event);"'
    t,' onkeypress="return jmenukeypress(event);"'
)

jhmx=: 3 : 0
if. '^'={:y do.
 s=. ' ','Esc+',_2{y
 t=. _2}.y
elseif. '*'={:y do.
 s=. ' ','Ctrl+',_2{y
 t=. _2}.y
elseif. 1 do.
 s=. '  '
 t=. y
end.
(dltb t);s
)

NB.? autocapitalize="none"
jeditatts=: ' autocomplete="off" autocapitalize="off" autocorrect="off" spellcheck="false" '

jhform=: 3 : 0
formtmpl hrplc 'LOCALE';>coname''
)

JASEP=: 1{a. NB. delimit substrings in ajax response

jgetfile=: 3 : '(>:y i: PS)}.y'
jgetpath=: 3 : '(>:y i: PS){.y'

NB. standard demo html boilerplate
jhdemo=: 3 : 0
c=. '.ijs',~>coname''
p=. jpath'~addons/ide/jhs/demo/',c
'<hr>',jhref_jhs_ 'jijs';p;c
)

NB. jgrid - special jht for grid
jhtx=: 3 : 0
t=. '<input type="text" id="<ID>" name="<ID>" class="<CLASS>" <EXTRAS>',jeditatts,'value="<VALUE>" size="<SIZE>" onkeydown="return jev(event)" >'
t hrplc 'ID VALUE SIZE CLASS EXTRAS';5{.y,(#y)}.'';'';'';'ht';''
)

NB.* see ~addons/ide/jhs/utilh.ijs for complete information
NB.* 
NB.* HBS verbs with id
NB.* jhab*id jhab text - anchor button
jhab=: 4 : 0
t=. '<a id="<ID>" href="#" name="<ID>" class="jhab" onclick="return jev(event)"'
t=. t,' ondblclick="return jev(event)"><VALUE></a>'
t hrplc 'ID VALUE';x;y
)

NB.* jhb*id jhb text - button
jhb=: 4 : 0
t=. '<input type="submit" id="<ID>" name="<ID>" value="<VALUE>" class="jhb" onclick="return jev(event)">'
t hrplc 'ID VALUE';x;y
)


NB.* jhclose*jhclose'' - cojhs close button and spacer
jhclose=: 3 : 0
'close'jhb''
)

NB.* jhcheckbox*id jhcheckbox text;checked (checked 0 or 1)
jhcheckbox=: 4 : 0
'value checked'=. y
checked=. >checked{'';'checked="checked"'
t=.   '<input type="checkbox" id="<ID>" value="<ID>" class="jhcheckbox" <CHECKED>'
t=. t,' name="<ID>" onclick="return jev(event)"/><label for="<ID>"><VALUE></label>'
t hrplc 'ID VALUE CHECKED';x;value;checked
)

NB.* jhdiv*id jhdiv text - <div id...>text</div>
jhdiv=: 4 : 0
'<div id="',x,'">',y,'</div>'
)

NB.* jhdivhidden*id jhdivhidden text - <div id...>text</div>
jhdivhidden=: 4 : 0
'<div id="',x,'" style="visibility:hidden">',y,'</div>'
)


NB.* jhdiva*id jhdiva text - <div id...>text
jhdiva=: 4 : 0
'<div id="',x,'">',y
)

NB.* jhdivadlg*id jhdivadlg text - <div id... display:none...>text
jhdivadlg=: 4 : 0
'<div id="',x,'" style="display:none;">',y
)

NB.* jhd3_basic*id jhd3_basic''
jhd3_basic=: 4 : 0
r=. 'redspacer'jhdiv'&nbsp;'
r=. r,LF,(x,'_error')jhdiv''
r=. r,LF,(x,'_header')jhdiv''
r=. r,LF,(x,'_title')jhdiv''
r=. r=. r,LF,x jhdiv''
r=. r,LF,(x,'_legend')jhdiv''
r=. r,LF,(x,'_footer')jhdiv''
(x,'_box')jhdiv LF,r,LF
)

NB.* jhec*id jhec '' - contenteditable div
jhec=: 4 : 0
t=. '<div id="<ID>" contenteditable="true"',jeditatts
t=. t,' style="white-space:nowrap;" '
t=. t,' onkeydown="return jev(event)"'
t=. t,' onkeypress="return jev(event)"'
t=. t,' onfocus="jecfocus();"'
t=. t,' onblur="jecblur();"'
t=. t,'>',y,'</div>'
t hrplc 'ID';x
)

NB.* jhhidden*id jhhidden value - <input type="hidden"....>
jhhidden=: 4 : 0
t=. '<input type="hidden" id="<ID>" name="<ID>" value="<VALUE>">'
t hrplc 'ID VALUE';x;y
)

NB.* jhmab*id jhmab text - menu anchor button (shortcut ^s)
jhmab=: 4 : 0
't s'=.jhmx y
t=. (JMWIDTH{.t),s
t=. x jhab t rplc ' ';'&nbsp;'
t=. t rplc 'class="jhab"';'class="jhmab"',jmon''
'<li>',t,'</li>'
)

NB.* jhmg*id jhmg text;decor;width - menu group
NB.* *decor 0 for '' and 1 for dropdown
jhmg=: 4 : 0
'text dec w'=. y
text=. text,>dec{'';'&#9660;'
t=. >(MSTATE=1){'</ul></span>';''
t=. t,'<span style="z-index:<INDEX>">'
t=. t,'<span><a href="#" id="<ID>" name="<ID>" class="jhmg"'
t=. t,' onclick="return jmenuclick(event);"'
t=. t,jmon''
t=. t,'><VALUE>&nbsp;</a></span>'
t=. t,'<ul id="<ID>_ul">'
t=. t hrplc 'ID VALUE INDEX';x;text;":MINDEX
MSTATE=: 2
MINDEX=: <:MINDEX
JMWIDTH=: w
t
)

jhmgb=: 4 : 0
t=. x jhab y
t=. t rplc 'class="jhab"';'class="jhmab"',jmon''
MSTATE=: 2
MINDEX=: <:MINDEX
JMWIDTH=: w
t
)



NB.* jhml*id jhml text - menu anchor link - (shortcut ^s)
NB. extra y element sets target - but not so useful as it isn't made current
jhml=: 4 : 0
if. 1=L.y do.
 'y target'=. y
else.
 target=. ;('jijs'-:y){y;TARGET
end.
't s'=.jhmx y
value=. t
text=. ((0>.JMWIDTH-#value)#' '),s
value=. value rplc ' ';'&nbsp;'
text=. text rplc ' ';'&nbsp;'
t=.   '<li><a href="<REF>" target="',target,'" class="jhml" onclick="return jmenuhide();"'
t=. t,jmon''
t=. t,'><VALUE></a><TEXT></li>'
t hrplc 'REF VALUE TEXT';x;value;text
)

NB.* jhpassword*id jhpassword text;size - text ignored
jhpassword=: 4 : 0
t=.   '<input type="password" id="<ID>" name="<ID>" class="jhpassword"',jeditatts,'value="" size="<SIZE>"'
t=. t,' onkeydown="return jev(event)"'
t=. t,'>'
t hrplc 'ID VALUE SIZE';x;y
)

NB.* jhradio*id jhradio value;checked;set (set groups radios)
jhradio=: 4 : 0
'value checked set'=. y
checked=. >checked{'';'checked="checked"'
t=.   '<input type="radio" id="<ID>" value="<ID>" class="jhradio" name="<SET>" <CHECKED>'
t=. t,' onclick="return jev(event)"/><label for="<ID>"><VALUE></label>'
t hrplc 'ID VALUE SET CHECKED';x;value;set;checked
)

NB.* jhref*jhref page;target;text
NB.* jhref*id jhref text - <a href="id">text</a> (deprecated - use monadic form)
jhref=: 3 : 0
'page target text'=. y
NB.! if. '~'={.target do. target=. jpath target end.
t=. '<a href="<REF>?jwid=<TARGET>" target="<TARGET>" class="jhref" ><TEXT></a>'
t hrplc 'REF TARGET TEXT';page;target;text
:
y=. boxopen y
t=. '<a href="<REF>" target="',TARGET,'" class="jhref" ><VALUE></a>'
t hrplc 'REF VALUE';x;y
)

NB.* jhselect*id jhselect texts;size;selected - selection control
jhselect=: 4 : 0
'values size sel'=. y
t=. '<select id="<ID>" name="<ID>" class="jhselect" size="<SIZE>" onchange="return jev(event)" >'
t=. t hrplc 'ID SIZE SEL';x;size;sel
opt=. '<option value="<VALUE>" label="<VALUE>" <SELECTED>><VALUE></option>'
for_i. i.#values do.
 t=. t,opt hrplc'VALUE SELECTED';(i{values),(i=sel){'';'selected="selected"'
end.
t=. t,'</select>'
)

NB.* jhspan*id jhspan text - <span id...>text</span>
jhspan=: 4 : 0
'<span id="',x,'">',y,'</span>'
)

NB.* jhtext*id jhtext text;size
jhtext=: 4 : 0
t=.   '<input type="text" id="<ID>" name="<ID>" class="jhtext"',jeditatts,'value="<VALUE>" size="<SIZE>"'
t=. t,' onkeydown="return jev(event)"'
t=. t,'>'
t hrplc 'ID VALUE SIZE';x;y
)

NB.* jhtextarea*id jhtextarea text;rows;ccols
jhtextarea=: 4 : 0
t=.   '<textarea id="<ID>" name="<ID>" class="jhtextarea" wrap="off" rows="<ROWS>" cols="<COLS>" '
t=. t,'onkeydown="return jev(event)"',jeditatts,'><DATA></textarea>'
t hrplc 'ID DATA ROWS COLS';x;y
)

NB.* 
NB.* HBS verbs without id

NB. should be smarter - no style=, replace style, replace att, : vs =, ... 
NB.* jhfix*html jhfix list - add styles and attributes to html
NB.* *html jhfix 'float:left';'tabindex="-1"';
jhfix=: 4 : 0
tx=. x
ty=. boxopen y
for_t. ty do.
 t=. >t
 if. ':'e.t do.
  tx=. tx rplc'style="';'style="',t,(';'~:{:t)#';'
 else.
  i=. tx i. ' '
  tx=. (i{.tx),' ',t,i}.tx
 end.
end.
tx
)

NB.* jhh1*jhh1 text - <h1>text</h1>
jhh1=: 3 : 0
'<h1>',y,'</h1>'
)

NB.* jhma*jhma'' - menu start
jhma=: 3 : 0
MSTATE=:1[MINDEX=:100
'<div class="menu">'
)

NB.* jhmz*jhmz'' - menu end
jhmz=: 3 : 0
MSTATE=:0
'</ul></span></div><br>'
)

NB.* jhresize*jhresize'' - separate fixed div from resizable div
jhresize=: 3 : '''</div><div id="jresizeb">'''

NB.* jhtr*jhtr rowdata - list of boxed row data html
jhtr=: 3 : 0
'<tr>','</tr>',~;(<'<td>'),each y,each<'</td>'
)

NB.* 
NB.* HBS nouns
NB.* jhtablea*jhtablea - <table>
jhtablea=: '<table>'

NB.* jhtablez*jhtablez - </table>
jhtablez=: '</table>'

NB.* jhbr*jhbr - <br/>
jhbr=: '<br/>'

NB.* jhhr*jhhr - <hr/>
jhhr=: '<hr/>'

NB.* jhbshtml*jhbshtml_jdemo1_'' -  show HBS sentences and html
jhbshtml=: 3 : 0
s=.<;._2 HBS
seebox s,.LF-.~each jhbsex each s
)

NB.* jnv*jnv_jhs_ 1 - toggle display of event name/value pairs
jnv=: 3 : 'NVDEBUG=:y' NB. toggle event name/value display

NB.* 
NB.* html response verbs

NB. build html response from page globals CSS JS HBS
NB. CSS or JS undefined allowed
NB.* jhr*title jhr names;values - names to replace with values
NB.* *send html response built from HBS CSS JS names values
jhr=: 4 : 0
if. _1=nc<'JS'  do. JS=:'' end.
if. _1=nc<'CSS' do. CSS=:'' end.
tmpl=. hrtemplate
if. SETCOOKIE do.
 SETCOOKIE_jhs_=: 0
 tmpl=. tmpl rplc (CRLF,CRLF);CRLF,'Set-Cookie: ',cookie,CRLF,CRLF
end.
htmlresponse tmpl hrplc 'TITLE CSS HEXTRA JS BODY';(TIPX,x);(css CSS);HEXTRA;(js JS);(jhbs HBS)hrplc y
)

NB.* jhrx*title jhrx (getcss'...'),(getjs'...'),getbody'...'
NB.* *send html response built from INCLUDE, CSS, JS, HBS
jhrx=: 4 : 0
JTITLE=: x
htmlresponse (hrxtemplate hrplc 'TITLE';(TIPX,x)),y
)

NB.* jhrpage*title jhrpage (getcss'...'),(getjs'...'),getbody'...'
NB.* *page response to a get
jhrpage=: 4 : 0
(hrxtemplate hrplc 'TITLE';(TIPX,x)),y
)

getincs=: 3 : 0
t=. ~.<;._2 INC
t=. (;(<y)-:each (-#y){.each t)#t
if. 0=#t do. t return. end.
b=. ;fexist each t
('INC file not found: ',>{.(-.b)#t)assert b
t
)

getcss=: 3 : 0
'getcss arg not empty'assert ''-:y
t=. getincs'.css'
t=. ;(<'<link rel="stylesheet" href="'),each t,each<'" />',LF
t,css CSS
)

fixjsi=: 3 : 0
if. NOCACHE do.
 '<script type="text/javascript">',LF,'// NOCACHE: ',y,LF,(fread y),LF,'</script>'
else.
 '<script src="',y,'"></script>'
end. 
)

getjs=: 3 : 0
t=. getincs'.js'
jsi=. ;LF,~each fixjsi each t 
y hrplc~jsi,js JS
)

gethbs=: 3 : 0
'</html>',~'</head>',y hrplc~jhbs HBS
)

NB.* jhrajax*jhrajax data - JASEP delimited data
jhrajax=: 3 : 0
htmlresponse y,~hajax rplc '<LENGTH>';":#y
)

chunk=: 3 : 0
if. 0=#y do.
 ''
else. 
 (hfd#y),CRLF,y,CRLF
end. 
)

NB.* jhrajax_a*jhrajax_a data - first chunk
jhrajax_a=: 3 : 0
putdata hajax_chunk,chunk y
)

NB.* jhrajax_b*jhrajax_b data - next chunk
jhrajax_b=: 3 : 0
putdata chunk y
)

NB.* jhrajax_b*jhrajax_z data - last chunk
jhrajax_z=: 3 : 0
putdata (chunk y),'0',CRLF,CRLF
shutdownJ_jsocket_ SKSERVER ; 2
sdclose_jsocket_ ::0: SKSERVER
SKSERVER_jhs_=: _1
)

