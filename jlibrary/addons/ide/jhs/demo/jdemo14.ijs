coclass'jdemo14'
coinsert'jhs'

months=: '"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"'

create=: 3 : 0
try.
 shown=: 1
 header=: dltb y
 header=: header,>('_'={:header){'__';''
 assert 0=nc<header
 d=. 0+header~
 'data must be 5 12 $ integers' assert (5 12-:$d)*.4=3!:0 d
 hot1=: 'jhot;_'cojhs ''

 NB. sets background-color of rows 6 and later (n+6)
 CSS__hot1=: CSS__hot1,'.handsontable tr:nth-child(n+6) > td {background-color: bisque;}'

 NB. sets summary rows to readonly
 option__hot1'cells: function(row, col, prop){var cellProperties = {};if(row > 4){cellProperties.readOnly = true;}return cellProperties;}'

 option__hot1'colHeaders:[',months,']'
 option__hot1'rowHeaders:["food","car","cell","rent","misc","min","max","avg","sum","tot"]'
 option__hot1'minSpareRows:0'
 option__hot1'minSpareCols:0'
 setdata__hot1 d,calc d
 
 jd3'reset'
 jd3'label ',months
 jd3'legend "foo","car","cell","rent","misc"'
 jd3'barwidth 60'
 jd3'type bar'
 jd3options=: jd3x__
 loc=: 'jd3;_;'cojhs jd3options;d
 
catch.
 ('create failed:',LF,13!:12'') assert 0
end. 
)

show=: 3 : 0
shown=: 1
y open ,~coname''
)

destroy=: 3 : 0
if. shown do. close ;coname'' end.
destroy__hot1''
destroy__loc''
codestroy''
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

HBS=: 0 : 0
jhclose''
'<span id="title">budget 2016</span>'
'<div id="hots">'
'<iframe id="hot1" name="hot1" src="',(;hot1),'"></iframe>'
'<iframe id="jd3" name="jd3" src="',(;loc),'"></iframe>'
'</div>'
)

NB. new CSS3 calc will allow better layout control
CSS=: 0 : 0
#title{margin-left:50px;text-align:center;font-size:22pt;}
#hots{height:90vh;overflow:hidden;}
#hot1{width:100%;height:50%;overflow:hidden;}
#jd3 {width:100%;height:50%;overflow:hidden;}
)

calc=: 3 : '(<./y),(>./y),(<.(+/%#) y),(+/y),:+/\ +/y'

jev_get=: 3 : 0
'jdemo14' jhrx (getcss''),(getjs''),gethbs''
)

NB. gets all the data - could get and apply just changes
jev_change=: 3 : 0
b=. 5{.data__hot1
d=. tablefromjs getv'jdata' NB. data and summary data
for_i. i.#d do.
 'r c z n'=. i{d
 b=. n (<r,c)}b NB. insert new data
end.
(header)=: b
d=. b,calc b
setdata__hot1 d
jhrajax (jsfromtable d),JASEP,jd3options,jd3data b
NB.jhrajax jsfromtable d
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

JS=: 0 : 0

var hot1;

function ev_body_load()
{
 window.frames[0].hot.addHook('afterChange', function(change,source)
 {
  if(source!="edit" && source!="paste")return;
  var s=  "jev_change_"+jform.jlocale.value+"_ 0"
  jdoajax([],JSON.stringify(change),s);
 }
 );
 // F5 refresh does not get updated jd3 - perhaps force it here?
}

function ajax(ts)
{
 var d= JSON.parse(ts[0]);
 var hot=window.frames[0].hot;
 hot.updateSettings({data: d});
 hot.render();
 document.getElementById("jd3").contentWindow.tabdata= ts[1];
 document.getElementById("jd3").contentWindow.plot("ga",ts[1]);
}
 
)