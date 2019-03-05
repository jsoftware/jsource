NB. handsontable - minimal
NB. 'jhot'cojhs'n'[n=..3 9

coclass'jhot'
coinsert'jhs'
shown=: 0
options=: 0 : 0
data: data,
minSpareRows: 1,
minSpareCols: 1,
contextMenu: true,
undo: true,
)  

create=: 3 : 0
shown=: 0
setdata i.2 3
)

show=: 3 : 0
shown=: 1
y open ,~coname''
)
 
destroy=: 3 : 0
if. shown do. close ;coname'' end.
codestroy''
)

setdata=: 3 : 0
'bad data'assert (2=$$y)*.2>L.y
option'type: ',;((0~:L.y)+.2=3!:0 y){'"numeric"';'"text"'
data=: y
)

NB. option'type:'          - remove option
NB. option'type:"numeric"' - new option
option=: 3 : 0
d=. <;.2 options
a=. (>:;d i.each':'){.each d
i=. >:y i.':'
t=. dltb i{.y
p=. dltb i}.y
'trailing , not allowed'assert ','~:{:p
d=. ;(a~:<t)#d
if. #p do. d=. d,t,p,',',LF end.
options=: d
)

INC=: INC_handsontable_basic 

HBS=: 0 : 0
'hot'jhdiv''
)

CUSTOM=: 0 : 0
 data= <DATA>
 $('#hot').handsontable({<OPTIONS>});
 hot = $('#hot').handsontable("getInstance");
 // hot.addHook('afterChange', function(){pagedirty=1;});
) 

jev_get=: 3 : 0
js=. getjs'CUSTOM';CUSTOM hrplc 'OPTIONS DATA';options;jsfromtable data
'jhot' jhrx (getcss''),js,gethbs''
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

JS=: 0 : 0 NB. javascript
function ev_body_load(){<CUSTOM>}


)
