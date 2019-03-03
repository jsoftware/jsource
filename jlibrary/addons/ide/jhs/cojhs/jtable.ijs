NB. handsontable table editor - includes jhot iframe
NB. 'jtable'cojhs'n'[n=. i.3 9

coclass'jtable'
coinsert'jhs'

create=: 3 : 0
try.
 shown=: 0
 header=: dltb y
 header=: header,>('_'={:header){'__';''
 title=: ;(''-:title){title;header
 assert 0=nc<header
 d=. header~
 assert 2>:$$d
 assert 2>L.d
 (header)=: (,:^:({.2-$$d)) d
 hot=: 'jhot;_'cojhs ''
 setdata__hot header~
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
destroy__hot''
codestroy''
)

HBS=: 0 : 0
jhclose''
'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;jtable: <Y>'
'<div id="hots">'
'<iframe id="hot" name="hot" src="',(;hot),'"  ></iframe>'
'</div>'
)

jev_get=: 3 : 0
title jhrx (getcss''),(getjs''),gethbs'Y';header
)

NB. currently gets all the data - could get and apply just changes
jev_change=: 3 : 0
(header)=: data__hot=: tablefromjs getv'jdata'
jhrajax''
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

CSS=: 0 : 0
body{margin:5px;}
#hots{height:90vh;}
#hot{width:100%;height:100%;}
)

JS=: 0 : 0 NB. javascript

function ev_body_load()
{
 window.frames[0].hot.addHook('afterChange', function(change,source)
 {
  if(source!="edit")return;
  var s=  "jev_change_"+jform.jlocale.value+"_ 0"
  jdoajax([],JSON.stringify(window.frames[0].data),s);
 }
 );
}

function ajax(ts){;}

)
