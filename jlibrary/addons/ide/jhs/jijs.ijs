NB. J HTTP Server - ijs app - textarea version

coclass'jijs'
coinsert'jhs'

HBS=: 0 : 0 rplc 'CMV';'4.2'
'<script src="~addons/ide/jhs/js/codemirror/codemirror.min.CMV.js"></script>'
'<script src="~addons/ide/jhs/js/codemirror/util/search.CMV.js"></script>'
'<script src="~addons/ide/jhs/js/codemirror/util/searchcursor.CMV.js"></script>'
'<script src="~addons/ide/jhs/js/codemirror/util/dialog.CMV.js"></script>'
'<link rel="stylesheet" href="~addons/ide/jhs/js/codemirror/codemirror.CMV.css">'
'<link rel="stylesheet" href="~addons/ide/jhs/js/codemirror/util/dialog.CMV.css">'
'<link rel="stylesheet" href="~addons/ide/jhs/js/codemirror/j/jtheme.CMV.css">'
'<script src="~addons/ide/jhs/js/codemirror/j/j.CMV.js"></script>'
jhma''
 'action'    jhmg'action';1;11
 'runw'     jhmab'run     r*'
 'runwd'    jhmab'run display'
 'save'     jhmab'save    s*'
 'saveas'   jhmab'save as...'
 'undo'     jhmab'undo    z*'
 'redo'     jhmab'redo    y*'
 'search'   jhmab'search/ctrls'
'option'    jhmg'option';1;8
 'ro'       jhmab'readonly    t^'
 'numbers'  jhmab'numbers'
jhmz''

'saveasdlg'    jhdivadlg''
 'saveasdo'    jhb'save as'
 'saveasx'     jhtext'';40
  'saveasclose'jhb'X'
'<hr></div>'

'rep'         jhdiv'<REP>'

'filename'    jhhidden'<FILENAME>'
'filenamed'   jhdiv'<FILENAME>'

jhresize''

'ijs'         jhtextarea'<DATA>';20;10

'textarea'    jhhidden''
)

NB. y file
create=: 3 : 0
rep=.''
try.
 d=. (1!:1<jpath y) rplc '&';'&amp;';'<';'&lt;'
 addrecent_jsp_ jshortname y
catch.
 d=. ''
 rep=. 'WARNING: file read failed!'
end.
(jgetfile y) jhr 'FILENAME REP DATA';y;rep;d
)

NB. new way - jwid=~temp/foo.ijs
NB. old way - mid=open&path=...
jev_get=: 3 : 0
if. #getv'jwid' do.
 create getv'jwid'
elseif. 'open'-:getv'mid' do.
 create getv'path' 
elseif. 1 do.
 create jnew''
end.
)

NB. add dirty handler here
ev_close_click=: 3 : 0
jhrajax''
)

save=: 3 : 0
if. #USERNAME do.
 fu=. jpath'~user'
 'save only allowed to ~user paths' assert fu-:(#fu){.y
end.
(toHOST getv'textarea')fwrite y
)

ev_save_click=: 3 : 0
f=. jpath getv'filename'
t=. _8{.timestamp''
try.
 save f
 jhrajax 'saved ',t
catch.
 smoutput 13!:12''
 jhrajax 'save ',t,' failed'
end.
)

ev_runw_click=: 3 : 0
f=. jpath getv'filename'
t=. _8{.timestamp''
try.
 save f
 if. 'runw'-:getv'jmid' do.
  load__ f
 else.
  loadd__ f
 end.
 jhrajax  'ran ',t
catch.
 smoutput 13!:12''
 jhrajax 'ran ',t,' ',13!:12''
end.
)

ev_runwd_click=: ev_runw_click

ev_saveasdo_click=:ev_saveasx_enter

NB. should have replace/cancel option if file exists
ev_saveasx_enter=: 3 : 0
f=. getv'filename'
n=. getv'saveasx'
if. n-:n-.'~/\' do.
 new=. (jgetpath f),n
else.
 new=. jpath n
end.
if. fexist new do. jhrajax 'file already exists' return. end.
try.
 save new
 addrecent_jsp_ jshortname new
 jhrajax ('saved as ',n),JASEP,new
catch.
 smoutput 13!:12''
 jhrajax 'save failed'
end.
)

NB. new ijs temp filename
jnew=: 3 : 0
d=. 1!:0 jpath '~temp\*.ijs'
a=. 0, {.@:(0&".)@> _4 }. each {."1 d
a=. ": {. (i. >: #a) -. a
f=. <jpath'~temp\',a,'.ijs'
'' 1!:2 f
>f
)

NB. p{} klduge because IE inserts <p> instead of <br> for enter
NB. codemirror needs jresizeb without scroll
NB. codemirror requires no div padding (line number vs caret) so set padding-left:0
CSS=: 0 : 0
#rep{color:red}
#filenamed{color:blue;}
*{font-family:<PC_FONTFIXED>;}
#jresizeb{overflow:visible;border:solid;border-width:1px;clear:left;}
div{padding-left:0;}
)

JS=: 0 : 0
var ta,rep,readonly,saveasx,cm,dirty=false;

function ev_body_load()
{
 ce= jbyid("ijs");
 rep= jbyid("rep");
 ta= jbyid("textarea");
 saveasx=jbyid("saveasx");
 ce.focus();
 cm = CodeMirror.fromTextArea(ce,
  {lineNumbers: true,
   mode:  "j",
   tabSize: 1,
   gutter: false,
   onChange: setdirty,
   extraKeys: {
    "Ctrl-S": function(instance){setTimeout(TOsave,1);},
    "Ctrl-R": function(instance){setTimeout(TOrunw,1);}
   }
  }
 );
 ro(0!=ce.innerHTML.length);
 dresize();
}

function TOsave(){jscdo("save");} // firefox needs ajax outside of event
function TOrunw(){jscdo("runw");}

window.onresize= dresize;

function dresize()
{
 // IE resize multiple frames sometimes gets id as null
 if(jbyid("jresizea")==null||jbyid("jresizeb")==null)return;
 var a= jgpwindowh(); // window height
 a-= jgpbodymh();     // body margin h (top+bottom)
 a-= jgpdivh("jresizea"); // header height
 a-= 5               // fudge extra
 a=  a<0?0:a;        // negative causes problems
 cm.setSize(jgpwindoww()-10,a);
}

// should be in utiljs.ijs
function jgpwindoww()
{
 if(window.innerWidth)
  return window.innerWidth; // not IE
 else
  return document.documentElement.clientWidth;
}

window.onbeforeunload = confirmExit;
function setdirty(){dirty=true;}
function confirmExit(){return dirty?"Page has unsaved changes.":null;}

function setnamed()
{
  jbyid("filenamed").innerHTML=jbyid("filename").value;
}

function ro(only)
{
 readonly= only;
 cm.setOption('readOnly', readonly?true:false)
 cm.getWrapperElement().style.background= readonly?"#ddd":"#fff";
 ce.focus();
}

function click(){ta.value= cm.getValue().replace(/\t/g,' ');jdoajax(["filename","textarea","saveasx"]);dirty=false;}
function ev_save_click() {click();}
function ev_runw_click() {click();}
function ev_runwd_click(){click();}

var searchtxt= "<pre>             ctrl+ (cmd+)</pre>"  
searchtxt+=    "<pre>search       f\nnext         g\nprevious     G\nreplace      F\nreplace all  R</pre>"
searchtxt+=    "<pre>save         s\nrun          r\nundo         z\nredo         y</pre>" 

function ev_undo_click(){cm.undo();}
function ev_redo_click(){cm.redo();}
function ev_search_click()
{
 jbyid("rep").innerHTML= (searchtxt==jbyid("rep").innerHTML)?"":searchtxt;
}

function ev_saveasdo_click(){click();}
function ev_saveasx_enter() {click();}

function ev_saveas_click()     {jdlgshow("saveasdlg","saveasx");dresize();}
function ev_saveasclose_click(){jhide("saveasdlg");dresize();}

function ev_ro_click(){ro(readonly= !readonly);}
function ev_numbers_click()
{
 cm.setOption('lineNumbers',cm.getOption('lineNumbers')?false:true);
}

// called with ajax response
function ajax(ts)
{
 rep.innerHTML= ts[0];
 if(2==ts.length&&(jform.jmid.value=="saveasx"||jform.jmid.value=="saveasdo"))
 {
  jhide("saveasdlg");
  jbyid("filename").value=ts[1];
  setnamed();
  var t=ts[0].split('/');
  if(1==t.length)
   document.title=ts[0].substring(9);
  else
   document.title=t[t.length-1];
 }
 dresize();
}

function ev_ijs_enter(){return true;}

function ev_z_shortcut(){cm.undo();}
function ev_y_shortcut(){cm.redo();}

function ev_t_shortcut(){jscdo("ro");}
function ev_r_shortcut(){jscdo("runw");}
function ev_s_shortcut(){jscdo("save");}
function ev_2_shortcut(){ce.focus();}
)
