coclass'jd3'
coinsert'jhs'

shown=: 0

create=: 3 : 0
try.
 'opt d'=. y
 data=: (opt rplc LF;'\n'),jd3data d
 title=: ;(''-:title){title;'jd3'
catchd.
 ('create failed:',LF,13!:12'') assert 0
end.
)

show=: 3 : 0
shown=: 1
y open ,~coname''
)

destroy=: 3 : 0
if. shown do. close ;coname'' end.
codestroy''
)

NB. J handlers for app events
jev_get=: 3 : 0
title jhrx(getcss''),(getjs'TABDATA';data),gethbs''
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

NB. css/js library files to include
INC=: INC_d3_basic 

NB. J sentences - create html body
HBS=: 0 : 0
jhclose''
'ga'jhd3_basic'' NB. ga,ga_... divs for d3_basic plot
)

CSS=: 0 : 0
)

NB. javascript
JS=: 0 : 0

tabdata="<TABDATA>"; // set by J jev_get handler

function ev_body_load()
{
 if (window.frameElement){jhide("close");}
 resize();
 window.onresize= resize;
}

function resize()
{
 jbyid("ga_box").style.width=window.innerWidth-20+"px";
 jbyid("ga_box").style.height=window.innerHeight-20+"px";
 plot("ga",tabdata);
}

function ajax(ts){;}

)