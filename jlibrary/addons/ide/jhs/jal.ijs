NB. J HTTP Server - jal/pacman app
coclass'jal'
coinsert'jhs'
require'pacman'

HBS=: 0 : 0
jhma''
jhjmlink''
jhmz''

'J Active Library - pacman - Package Manager<br/>'
'<a href="http://code.jsoftware.com/wiki/addons">code.jsoftware.com/wiki/addons</a>'
'<hr>'
'upable' jhb'Upgradeable'
'remable'jhb'Removeable'
'inst'   jhb'Installed'
'notin'  jhb'Not Installed'
'desc'   jhb'Descriptions'
'buttons'jhdiv'<BUTTONS>'
jhresize''
'result' jhdiv'<RESULT>'
)

checkers=: ('check'jhb'Check all'),'uncheck'jhb'Uncheck all'

maketable=: 3 : 0
b=. ((<'c_'),each{."1 y)jhcheckbox each <'';0
NB. b=. ,.(('c'),each ":each <"0 i.#y)jhcheckbox each <'';0
t=. jhtablea
t=. t,,jhtr"1 b,.y
t,jhtablez
)

create=: 3 : 0 NB. create - y replaces <RESULT> in body
'jal'jhr'BUTTONS RESULT';y
)

jev_get=: 3 : 0
create '';('update'jpkg'')rplc LF;'<br>'
)

ev_upable_click=: 3 : 0
'update'jpkg'' NB. update to make current
d=. 'showupgrade'jpkg''
 b=. checkers,~'upgrade'jhb'Upgrade Selected'
 t=. maketable d
 create b;t
)

ev_remable_click=: 3 : 0
'update'jpkg'' NB. update to make current
d=. 'showinstalled'jpkg''
d=. d#~-.({."1 d) e. 'base library';'ide/jhs'
b=. checkers,~'remove'jhb'Remove Selected'
t=. maketable d
create b;t
)

ev_notin_click=: 3 : 0
'update'jpkg'' NB. update to make current
b=. checkers,~'install'jhb'Install Selected'
t=. maketable'shownotinstalled'jpkg''
create b;t
)

ev_inst_click=: 3 : 0
'update'jpkg'' NB. update to make current
b=. checkers,~'upgrade'jhb'Upgrade Selected'
t=. maketable'showinstalled'jpkg''
create b;t
)

doselect=: 3 : 0
d=. {."1 NV
b=. (<'c_')=2{.each d
d=. b#d
y jpkg 2}.each d
jev_get''
)

ev_install_click=: 3 : 0
doselect'install'
)

ev_upgrade_click=: 3 : 0
doselect'upgrade'
)

ev_remove_click=: 3 : 0
doselect'remove'
)

descfix=: 3 : 0
i=. y i.LF
'<span style="color:red">',(i{.y),'</span>',}.i}.y
)

ev_desc_click=: 3 : 0
d=. ('showinstalled'jpkg''),'shownotinstalled'jpkg''
d=. /:~{."1 d
d=. 'show'jpkg d
d=. d rplc '== ';0{a.
d=. <;._1 d
d=. descfix each d
r=. ;d
create '';r rplc LF;'<br>'
)

JS=: 0 : 0
function ev_body_load(){jresize();cbfocus();}

function cbfocus()
{
 var n=document.getElementsByTagName("input");
 for(var i=0;i<n.length;++i)
  if("checkbox"==n[i].getAttribute("type")){n[i].focus();break;}
}

function check(v)
{
 var n=document.getElementsByTagName("input");
 for(var i=0;i<n.length;++i)
 {
  if("checkbox"==n[i].getAttribute("type")){n[i].checked=v;}
 }
 cbfocus();
}

function ev_check_click(){check("checked");}
function ev_uncheck_click(){check("");}

)
