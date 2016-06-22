require'~addons/convert/json/json.ijs'

coclass'jtable'
coinsert'jhs'

HBS=: 0 : 0
'<link rel="stylesheet" href="~addons/ide/jhs/js/jquery/smoothness/jquery-ui.custom.css" />'
'<script src="~addons/ide/jhs/js/jquery/jquery-2.0.3.min.js"></script>'
'<script src="~addons/ide/jhs/js/jquery/jquery-ui.min.js"></script>'
'<script rel="stylesheet" href="~addons/ide/jhs/js/jquery/jquery-ui.css"></script>'
'<script src="~addons/ide/jhs/js/jquery/handsontable.js"></script>'
'<link rel="stylesheet" href="~addons/ide/jhs/js/jquery/handsontable.css">'
'<button type="button" id="save">save</button>'
'<input type="text" id="n" name="n" value="<NAME>" autofocus="autofocus" size="22"/>'
'example' jhdiv ''
'<div id="dialog" title="Table Editor Error"></div>'
)

ercom=: '<br/><br/>see jhelp for more info'

jev_get=: 3 : 0
'jtable'jhr''
)

validate=: 3 : 0
n=. dltb y
n=. n,>('_'={:n){'__';''
assert 0=nc<n
d=. n~
assert 2=$$d
assert 2>L.d
d
)

NB. wid_body_load_data_jtable_ values are not cleaned up
ev_body_load=: 3 : 0
try.
 a=. '_ev_body_load_data_jtable_',~getv'jwid' 
 if. 0~:nc<a do. (a)=: '<NAME>' end.
 n=. a~
 d=. validate n
 if. 0=L.d do. d=. <"0 d end.
 d=. enc_json <"1 d
 if. d-:'[]' do. d=. '[[]]' end.
catch.
 jhrajax'not valid table for editing',ercom
 return.
end. 
jhrajax JASEP,d,JASEP,n
)

ev_save_click=: 3 : 0
try.
 n=. dltb getv'n'
 n,>('_'={:n){'__';''
 d=. getv'jdata'
 if. '[[]]'-:d do. jhrajax 'nothing to save',ercom return. end.
 d=. _1 _1}.>dec_json d
 if. -.2 e. ,>3!:0 each d do. d=. >d end.
 (n)=: d
 d=. JASEP
catch.
 d=. 'save failed',ercom
end. 
jhrajax d
)

CSS=: ''   NB. styles

JS=: 0 : 0 NB. javascript
var data= [[]];

// must use JHS framework load handler instead of jquery - $(document).ready(function() 
function ev_body_load()
{
 document.title= window.name;
 $(function(){$("#dialog").dialog({autoOpen:false,modal:true});});
 bindenter();
 bindsave();
 jdoajax([]);
}

function bindsave(){$('body').on('click','button[id=save]',function(){jscdo("save");});}

function bindenter()
{
 $(document).on("keypress","#n",function(e)
 {
  if (e.which == 13)
  {
   jscdo("save");
   return false; // critical - avoid default form enter event
  }
 });
}

function ev_save_click(){jdoajax("n",JSON.stringify(data));}

function ajax(ts)
{
 if(0!=(ts[0].length)) 
 {
     $("#dialog").html(ts[0]);
     $("#dialog").dialog("open");
     return;
 }
 if("save"==jform.jmid.value)return;
 var sf= (ts[1].indexOf('"')===-1)?"numeric":null; 
 data= eval(ts[1]);
 $("#n").val(ts[2]);
 $('#example').handsontable({
  data: data,
  minSpareRows: 1,
  minSpareCols: 1,
  colHeaders: true,
  rowHeaders: true,
  contextMenu: true,
  type: sf,
  undo: true
 });
}
)
