NB. J HTTP Server - ijs app - textarea version

coclass'jijs'
coinsert'jhs'

HBS=: 0 : 0
jhma''
jhjmlink''
'action'    jhmg'action';1;10
 'runw'     jhmab'run         r^'
 'runwd'    jhmab'run display'
 'save'     jhmab'save        s^'
 'saveas'   jhmab'save as...'
'option'    jhmg'option';1;8
 'ro'       jhmab'readonly    t^'
jhmz''

'saveasdlg'    jhdivadlg''
 'saveasdo'    jhb'save as'
 'saveasx'     jhtext'';10
  'saveasclose'jhb'X'
'<hr></div>'

'rep'         jhdiv'<REP>'

'filename'    jhhidden'<FILENAME>'
'filenamed'   jhdiv'<FILENAME>'

jhresize''

'ijs'         jhtextarea'<DATA>';20;10

'textarea'    jhhidden''
)

FNMS=: ''

NB. y file
create=: 3 : 0
rep=. >(FNMS e.~<y){'';'WARNING: file already open!' 
FNMS=: FNMS,<y
try. d=. 1!:1<y catch.
 d=. ''
 rep=. 'WARNING: file read failed!'
end.
(jgetfile y) jhr 'FILENAME REP DATA';y;rep;d
)

jev_get=: 3 : 0
if. 'open'-:getv'mid' do.
 create getv'path' 
else.
 create jnew''
end.
)

save=: 3 : 0
if. #USERNAME do.
 fu=. jpath'~user'
 'save only allowed to ~user paths' assert fu-:(#fu){.y
end.
new=. toHOST getv'textarea'
old=. fread y
if. -._1-:old do. old=. toHOST old end.
if. new-:old do.
 smoutput'jijs not saved (unchanged): ',y
else.
 smoutput'jijs saved: ',y
end. 
new  fwrite y
)

ev_save_click=: 3 : 0
f=. jpath getv'filename'
try.
 save f
 jhrajax 'saved without error'
catch.
 jhrajax 'save failed'
end.
)

ev_runw_click=: 3 : 0
f=. jpath getv'filename'
try.
 save f
 if. 'runw'-:getv'jmid' do.
  load__ f
 else.
  loadd__ f
 end.  
 jhrajax 'ran saved without error'
catch.
 jhrajax 13!:12''
end.
)

ev_runwd_click=: ev_runw_click

ev_saveasdo_click=:ev_saveasx_enter

NB. should have replace/cancel option if file exists
ev_saveasx_enter=: 3 : 0
f=. getv'filename'
n=. getv'saveasx'
if. n-:n-.'~/' do.
 new=. (jgetpath f),n
else.
 new=. jpath n
end.
if. fexist new do. jhrajax 'file already exists' return. end.
try.
 save new
 jhrajax ('saved as ',n),JASEP,new
catch.
 jhrajax 'save failed'
end.
)

ev_body_unload=: 3 : 0
f=. getv'filename'
save f
FNMS=:(-.(i.#FNMS)=(FNMS=<f)i.1)#FNMS
jhrajax''
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
CSS=: 0 : 0
#rep{color:red}
#filenamed{color:blue;}
*{font-family:<PC_FONTFIXED>;}
p{margin:0;}
)

JS=: 0 : 0
var ta,rep,readonly,saveasx;

function ev_body_load()
{
 ce= jbyid("ijs");
 rep= jbyid("rep");
 ta= jbyid("textarea");
 saveasx=jbyid("saveasx");
 ro(0!=ce.innerHTML.length);
 dresize();
 ce.focus();
}

window.onresize= dresize;

function dresize()
{
 // IE resize multiple frames sometimes gets id as null
 if(jbyid("jresizea")==null||jbyid("jresizeb")==null)return;
 var a= jgpwindowh(); // window height
 a-= jgpbodymh();     // body margin h (top+bottom)
 a-= jgpdivh("jresizea"); // header height
 a-= 26               // fudge extra
 a=  a<0?0:a;        // negative causes problems
 ce.style.height= a+"px";
 ce.style.width= (jgpwindoww()-26)+"px";
}

// should be in utiljs.ijs
function jgpwindoww()
{
 if(window.innerWidth)
  return window.innerWidth; // not IE
 else
  return document.documentElement.clientWidth;
}

function ev_body_unload()
{
 ta.value= ce.value;
 jdoajax(["filename","textarea","saveasx"],"",jevsentence,false);
}

function ro(only)
{
 readonly= only;
 ce.readOnly=readonly?true:false;
 ce.style.background= readonly?"#eee":"#fff";
}

function click(){ta.value= ce.value;jdoajax(["filename","textarea","saveasx"]);}
function ev_save_click() {click();}
function ev_runw_click() {click();}
function ev_runwd_click(){click();}

function ev_saveasdo_click(){click();}
function ev_saveasx_enter() {click();}

function ev_saveas_click()     {jdlgshow("saveasdlg","saveasx");}
function ev_saveasclose_click(){jhide("saveasdlg");}

function ev_ro_click(){ro(readonly= !readonly);}

// called with ajax response
function ajax(ts)
{
 rep.innerHTML= ts[0];
 if(2==ts.length&&(jform.jmid.value=="saveasx"||jform.jmid.value=="saveasdo"))
 {
  jhide("saveasdlg");
  jbyid("filenamed").innerHTML=ts[1];
  jbyid("filename").value=ts[1];
  document.title=ts[0].substring(9);
 }
}

function ev_ijs_enter(){return true;}

function ev_t_shortcut(){jscdo("ro");}
function ev_r_shortcut(){jscdo("runw");}
function ev_s_shortcut(){jscdo("save");}
function ev_2_shortcut(){ce.focus();}
)
