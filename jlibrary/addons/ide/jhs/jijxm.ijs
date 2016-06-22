coclass'jijxm'
coinsert'jhs'

redirecturl=: ''  NB. used by browse_j_

body=: 0 : 0
<body onload="document.j.jdo.focus();">
<font face="courier new,courier,monospace">
<form id="j" name="j" method="post" action="jijxm">
<div id="log"><LOG></div>
&nbsp;&nbsp;&nbsp;<input type="text" id="jdo" name="jdo" size="<COLS>"
 style="font-family:courier new,courier,monospace;"
 autocapitalize="off" spellcheck="false"><input type="submit" id="run" name="" value="&crarr;" tabindex="-1">
<BODYTA>
</font></form></body>
)

bodyta=: 3 : 0
if. 0=xmrows do. '' return. end.
t=.   '<br><textarea id="ta" name="ta" wrap="off" rows="<ROWS>" cols="<COLS>"'
t=. t,'autocomplete="off" autocapitalize="off" spellcheck="false"><TA></textarea>'
t hrplc 'COLS ROWS TA';xmcols;xmrows;getv'ta'
)

jev_get=: 3 : 0
if. helpflag do. smoutput '   xmh'''' NB. show help' end.
helpflag=: 0
urlresponse''
)

urlresponse=: 3 : 0
LOG_jhs_=: LOG,LOGN
LOGN_jhs_=: ''
create''
)

NB. get last xmlines of LOG
lop=: 3 : 0
e=. '<br>'E.LOG
e=. e+.'</div>'E.LOG
if. xmlines>+/e do.
 LOG
else.
 d=. xmlines{|.e#i.#LOG
 d}.LOG
end.
)

create=: 3 : 0
if. #redirecturl_jijxm_ do.
  htmlresponse html301 hrplc 'NEWURL';redirecturl_jijxm_
  redirecturl_jijxm_=: ''
else.
  NB. should be integrated to use jhr rather than dup code
  b=. body hrplc 'COLS LOG BODYTA';(xmcols-6);(lop LOG);bodyta''
  tmpl=. hrtemplate
  if. SETCOOKIE do.
   SETCOOKIE_jhs_=: 0
   tmpl=. tmpl rplc (CRLF,CRLF);CRLF,'Set-Cookie: ',cookie,CRLF,CRLF
  end.
  htmlresponse tmpl hrplc 'TITLE CSS JS BODY';'jijxm';'';'';b
end.
)

help=: 0 : 0
   xmh''            NB. jijxm help
   xmc L C T        NB. configure
   NB. Log rows, Cols, Textarea rows
   xmx''            NB. execute T
   xmr'~temp/t.ijs' NB. read to T
   xmw'~temp/t.ijs' NB. write from T
)

helpflag=: 1

xmh_z_=: 3 : 0
help_jijxm_
)

NB. exectute ta
xmx_z_=: 3 : 0
0!:100 getv_jhs_'ta'
)

NB. read ta from file
xmr_z_=: 3 : 0
NV_jhs_=:('ta';fread y),NV_jhs_
EMPTY
)

NB. write ta to file
xmw_z_=: 3 : 0
(getv_jijxm_'ta')fwrite y
)

xmc_z_=: 3 : 0
assert y>:4 4 _1
assert y<:_ 200 200
'xmlines_jijxm_ xmcols_jijxm_ xmrows_jijxm_'=: y
)

xmc 20 30 10
