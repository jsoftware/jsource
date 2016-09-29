NB. ipad editor
coclass'jijsipad'
coinsert'jhs'

HBS=: 0 : 0
'read'jhb'RD'
'write'jhb'WR'
'load'jhb'LD'
'file'jhtext'<FILE>';40
'<br>','ijs'jhtextarea'';20;60 NB. height width
)

jev_get=: create

NB. iphone=. 0<#('iPhone'ss t),'iPod'ss t=. gethv_jhs_ 'User-Agent:'

create=: 3 : 0
ferase f=. jshortname jnew''
'jijsipad' jhr 'FILE';f
)

ev_write_click=: 3 : 0
(getv'ijs')fwrite getv'file'
jhrajax''
)

ev_read_click=: 3 : 0
t=. fread getv'file'
jhrajax >(_1-:t){t;''
)

ev_load_click=: 3 : 0
f=. jpath getv'file'
try.
 (getv'ijs')fwrite f
 load__ f
 smoutput'load ',f,': no error'
 jhrajax 'ran saved without error'
catch.
 smoutput 13!:12''
 jhrajax 13!:12''
end.
)

CSS=: 0 : 0
*{font-family:<PC_FONTFIXED>;}
form{margin-top:0;margin-bottom:0;}
*.jhb  {margin-left:0;margin-right:0;}
)

JS=: 0 : 0

function ev_body_load(){jbyid("ijs").focus();}

function ev_write_click(){jdoajax(["file","ijs"]);}
function ev_read_click() {jdoajax(["file"]);}
function ev_write_click_ajax(ts){jbyid("ijs").focus();}
function ev_load_click_ajax(ts){jbyid("ijs").focus();}

function ev_load_click() {jdoajax(["file","ijs"]);}
function ev_load_click_ajax(ts){jbyid("ijs").focus();}

function ev_read_click_ajax(ts)
{
 jbyid("ijs").value=ts[0];
 jbyid("ijs").focus();
}

)
