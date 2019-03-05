coclass'jwatch'
coinsert'jhs'

shown=: 0

create=: 3 : 0
sentence=: y
title=: 'jwatch'
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
'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' NB. red spacer
'run'jhb''
'sentence'jhtext'<SENTENCE>';30
'<div id="data" class="jcode"><TEXT></div>'
)

CSS=: 0 : 0
form{margin:2px;}
#sentence{width:50%;}
)

calc=: 3 : 0
try. r=. ":do__ y catch. r=. 13!:12'' end. 
if. 2=$$r do. r=. ,r,.LF end.
utf8_from_jboxdraw jhtmlfroma fmt0 r
)

jev_get=: 3 : 0
title jhrx (getcss''),(getjs''),gethbs'SENTENCE TEXT';sentence;calc sentence
)

ev_run_click=: 3 : 0
sentence=: getv'sentence'
jhrajax calc sentence
)

JS=: 0 : 0
function ev_run_click(){jdoajax(["sentence"]);}
function ev_run_click_ajax(ts){jbyid("data").innerHTML=ts[0];}
function ev_sentence_enter(){jscdo("run");}
function ajax(ts){;}
)
