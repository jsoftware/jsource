NB. J HTTP Server - jijx app

coclass'jijxipad'
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
'log' jhdiv'<LOG>'
'jdo'jhtext'';40
'up'jhb'&uarr;'
'dn'jhb'&darr;'
'return'jhb'&crarr;'
'adv'jhb'.'
'jijsipad'jhrefx '&nbsp;E&nbsp;'
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
'jijxipad' jhr 'LOG FILE RECALLS';LOG;(jshortname jnew'');recalls''
)

recalls=: 3 : 0
t=.INPUT
t=.(0~:;#each t-.each' ')#t
(;t,each LF)rplc '&';'&amp;';'"';'&quot;';'<';'&lt;';'>';'&gt;'
)

labs__=: 3 : 0
if. _1=4!:0<'LABTITLES_jijx_' do.
 require'~addons/ide/jhs/jijx.ijs'
 getlabs_jijx_''
end.
if. 2=3!:0 y do.
 if. ''-:y do.
  >~.LABCATS_jijx_
 else.
  d=. (3j0":each<"0 i.#LABTITLES_jijx_),each (>:>LABTITLES_jijx_ i.each':')}.each LABTITLES_jijx_
  >((LABCATS_jijx_)=<y)#d
 end.
else.
 labopen_jijx_ y
end.
)

CSS=: 0 : 0
*{font-family:"courier new","courier","monospace";}
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
 jseval(false,jbyid("log").innerHTML); // redraw canvas elements
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
function ajax(ts){updatelog(ts[0]);jseval(true,ts[0]);if(2==ts.length)jbyid("ijs").value=ts[1];}

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
   jform.jdo.blur(); // hide the onscreen keyboard
}

function ev_return_click(){ev_jdo_enter();}
function ev_bottom_click(){ jbyid("jdo").scrollIntoView(false);}
function ev_up_click(){uarrow();}
function ev_dn_click(){darrow();}
function ev_adv_click(){jdoajax([],"","labnext_jlab_[0")}
)
