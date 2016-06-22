NB. J HTTP Server - jhelp app
coclass'jfiles'
coinsert'jhs'

HBS=: 0 : 0
jhma''
jhjmlink''
jhmz''
jhresize''
files''
)

jev_get=: 3 : 0
'jfiles'jhr''
)

files=: 3 : 0
addrecent_jsp_''
'</div>',~'<div>',;fx each SPFILES_jsp_
)

fx=: 3 : 0
s=. ;shorts_jsp_ y
t=. (JIJSAPP_jhs_,'?mid=open&path=',jpath y)jhref_jhs_ s
t=. t,(;(1>.20-#s)#<'&nbsp;'),y
t,'<br>'
)


CSS=: 0 : 0
*{font-family:"courier new","courier","monospace";}
)

JS=: 0 : 0
function ev_body_load(){jresize();}
)
