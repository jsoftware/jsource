coclass'app6'
coinsert'jhs'

0 : 0
app6 uses a numbered locale for app atate
this is the way apps with state should work

app6 page is created with cojhs:
   'app6'cojhs'calendar'
   
creates numbered locale, does a coinsert of app6,
calls app6 create with right arg to set sentence,
and then shows the page

this type of app generally has a red close button (jhclose)
and close event handlers that can release resources,
close the window, and destroy locales

in jijx, create new app6 pages with different state:
   p=: 'app6;10 10'cojhs'tolower'
   sentence__p
)

shown=: 0

create=: 3 : 0
sentence=: y
title=: 'app6'
)

show=: 3 : 0
shown=: 1
y open ,~coname''
)

destroy=: 3 : 0
if. shown do. close ;coname'' end.
codestroy''
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

HBS=: 0 : 0
jhclose''
jhh1'explicit verb - debug line numbers'
'run'jhb''
'name'jhtext'<NAME>';30
'<div id="data" class="jcode"><TEXT></div>'
)

CSS=: 0 : 0
form{margin:2px;}
)

jev_get=: 3 : 0
title jhrx (getcss''),(getjs''),gethbs'NAME TEXT';sentence;jhtmlfroma dbsd sentence
)

ev_run_click=: 3 : 0
sentence=: getv'name'
jhrajax jhtmlfroma dbsd sentence
)

JS=: 0 : 0
function ev_run_click(){jdoajax(["name"]);}
function ev_run_click_ajax(ts){jbyid("data").innerHTML=ts[0];}
function ev_name_enter(){jscdo("run");}
)
