NB. JHS - recent files
coclass'jfiles'
coinsert'jhs'

HBS=: 0 : 0
jhresize''
files''
)

jev_get=: 3 : 0
'jfiles'jhr''
)

ev_close_click=: 3 : 0
jhrajax''
)

files=: 3 : 0
addrecent_jsp_''

if. 0=#SPFILES_jsp_ do.
 '</div>',~'<div>The recent files list is empty.'
else.
 '</div>',~'<div>',;fx each SPFILES_jsp_
end. 
)

fx=: 3 : 0
s=. ;shorts_jsp_ y
t=. jhref 'jijs';y;s NB. (jpath y);s
t=. t,(;(1>.20-#s)#<'&nbsp;'),y
NB.! t=. t,(;(1>.20-#s)#<'&nbsp;'),(_4}.isotimestamp>1{,(1!:0@jpath y)),' ',y
t,'<br>'
)


CSS=: 0 : 0
*{font-family:<PC_FONTFIXED>;}
)

JS=: 0 : 0
function ev_body_load(){jresize();}
)
