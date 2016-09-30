NB. J HTTP Server - jijx app
coclass'jijxh'
coinsert'jhs'

NB.override formtmpl to remove hidden jdo so ours will work
formtmpl=: 0 : 0 -. LF
<form id="j" name="j" method="post" action="<LOCALE>">
<input type="hidden" name="jlocale" value="<LOCALE>">
<input type="hidden" name="jid"     value="">
<input type="hidden" name="jtype"   value="">
<input type="hidden" name="jmid"    value="">
<input type="hidden" name="jsid"    value="">
<input type="submit" value="" onclick="return false;" style="display:none;width:0px;height:0px;border:none">
)

HBS=: 0 : 0
'bottom'jhb'&or;'
'log' jhdiv'<LOG>'
'jdo'jhtext'';60
'<br>'
'return'jhb'&crarr;'
'up'jhb'&uarr;'
'dn'jhb'&darr;'
'load'jhb'L'
'read'jhb'R'
'write'jhb'W'
'help'jhb'?'
'<br>','file'jhtext'<FILE>';60
'<br>','ijs'jhtextarea'';100;60
'recalls'jhhidden'<RECALLS>'
)

jev_get=: create

NB. move new transaction(s) to log
uplog=: 3 : 0
LOG_jhs_=: LOG,LOGN
LOGN_jhs_=: ''
)

NB. y is J prompt - '' '   ' or '      '
NB. called at start of input
NB. ff/safari/chrome collapse empty div (hence JZWSPU8)
NB. empty prompt is JZWSPU8; which is removed from input
urlresponse=: 3 : 0
if. 0=#y do.
 t=. JZWSPU8
 PROMPT_jhs_=: JZWSPU8
else.
 t=. (6*#y)$'&nbsp;'
 PROMPT_jhs_=: y
end.
d=. LOGN
uplog''
if. METHOD-:'post' do.
 jhrajax d
else.
 create''
end.
)

NB. iphone=. 0<#('iPhone'ss t),'iPod'ss t=. gethv_jhs_ 'User-Agent:'

NB. refresh response - not jajax
create=: 3 : 0
uplog''
'jijxh' jhr 'LOG FILE RECALLS';LOG;(jshortname jnew'');recalls''
)

recalls=: 3 : 0
t=.INPUT
t=.(0~:;#each t-.each' ')#t
(;t,each LF)rplc '&';'&amp;';'"';'&quot;';'<';'&lt;';'>';'&gt;'
)

load_jijxh_=: 3 : 0
(getv'ijs')fwrite getv'file'
load__ getv'file'
)

ev_write_click=: 3 : 0
(getv'ijs')fwrite getv'file'
i.0 0
)

ev_read_click=: 3 : 0
t=. fread getv'file'
jhrajax >(_1-:t){t;''
)

ev_help_click=: 3 : 0
jhrajax jhfroma help hrplc 'LA UA DA';(226 134 181{a.);(226 134 145{a.);226 134 147{a.
)

help=: 0 : 0
<LA> run sentence 
<UA> up recall
<DA> dn recall
L write and load
R read file
W write file
)

CSS=: 0 : 0
*{font-family:<PC_FONTFIXED>;}
form{margin-top:0;margin-bottom:0;}
*.fm   {color:<PC_FM_COLOR>;}
*.er   {color:<PC_ER_COLOR>;}
*.log  {color:<PC_LOG_COLOR>;}
*.sys  {color:<PC_SYS_COLOR>;}
*.file {color:<PC_FILE_COLOR>;}
*.jhb  {margin-left:0;margin-right:0;}
)

JS=: 0 : 0
var recs;
var reci= -1;
var phead= '<div id="prompt" class="log">';
var ptail= '</div>';

// function ev_body_focus(){setTimeout(scrollz();}

function ev_body_load()
{
 var t=jbyid("recalls").value;
 if(0==t.length)
  recs=[];
 else
  recs=t.split("\n");
 setTimeout(scrollz,10); // allow doc to update
}

function updatelog(t)
{
 var p,parent,n= document.createElement("div");
 n.innerHTML= t;
 jbyid("log").appendChild(n);
 jbyid("jdo").value="";
 setTimeout(scrollz,10); // allow doc to update
}

// ajax update window with new output
function ajax(ts){updatelog(ts[0]); if(2==ts.length)jbyid("ijs").value=ts[1];}

// add sentence to log unless blank or same as last
function addrecall(a)
{
 var i,blank=0,same=0;
 for(i=0;i<a.length;++i)
  blank+= ' '==a.charAt(i);

 if(0!=recs.length && a.length==recs[0].length)
 {
  for(i=0;i<a.length;++i)
   same+= a.charAt(i)==recs[0].charAt(i);
 }

 if(blank!=a.length && same!=a.length)
  recs.unshift(a); reci=-1; // recalls
}

function scrollz(){
 jbyid("jdo").focus();
 jbyid("jdo").scrollIntoView(false);
}

function newpline(t){jform.jdo.value=t;}

function uarrow()
{
 if(++reci>=recs.length) reci= recs.length-1;
 if(reci==-1)
  newpline("   ");
 else
  newpline(recs[reci]);
}

function darrow()
{
 var t;
 if(--reci<0)
  {reci= -1; t= "   ";}
 else
  t= recs[reci]
 newpline(t);
}

function ev_jdo_enter()
{
  var t=jform.jdo.value;
  addrecall(t);
  if(0!=t.length)
   jdoajax([],"",t);
  else
   jdoajax([],"","\u200B");
}

function ev_return_click(){ev_jdo_enter();}
function ev_bottom_click(){ jbyid("jdo").scrollIntoView(false);}
function ev_up_click(){uarrow();}
function ev_dn_click(){darrow();}
function ev_load_click(){jdoajax(["file","ijs"],"","load_jijxh_''");}
function ev_write_click(){jdoajax(["file","ijs"]);}
function ev_read_click(){jdoajax(["file"]);}
function ev_help_click(){jdoajax();}

function ev_read_click_ajax(ts)
{
 jbyid("ijs").value=ts[0];
 jbyid("jdo").value="";
 jbyid("ijs").focus();
 jbyid("load").scrollIntoView(true);
}
)
