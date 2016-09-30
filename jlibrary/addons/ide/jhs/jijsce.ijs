NB. J HTTP Server - ijs app
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
 'findd'    jhmab'find...     q^'
 'replace'  jhmab'replace...'
'option'    jhmg'option';1;8
 'ro'       jhmab'readonly    t^'
 'color'    jhmab'color       c^'
 'number'   jhmab'number      n^'
jhmz''

'saveasdlg'    jhdivadlg''
 'saveasdo'    jhb'save as'
 'saveasx'     jhtext'';10
  'saveasclose'jhb'X'
'<hr></div>'

'finddlg'    jhdivadlg''
 'findtop'   jhb'top'
 'findnext'  jhb 'dn'
 'findprev'  jhb 'up'
 'what'      jhtext'';10
 'context'   jhselect(<;._2 FIFCONTEXT);1;0
 'matchcase' jhcheckbox'case';1
 'findclose' jhb'X'
'<hr></div>'

'repldlg'      jhdivadlg''
 'repldo'      jhb'Replace'
  'replforward'jhb'Replace Forward'
   'replnew'   jhtext'';10
    'replclose'jhb'X'
'<hr></div>'

'rep'         jhdiv'<REP>'

'filename'    jhhidden'<FILENAME>'
'filenamed'   jhdiv'<FILENAME>'

jhresize''

('num'        jhec'') jhfix'float:left';'tabindex="-1"'
'ijs'         jhec'<DATA>'

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
(jgetfile y) jhr 'FILENAME REP DATA';y;rep;jhfroma d
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

NB. same as in jfif
FIFCONTEXT=: 0 : 0
any
name
=: or =.
=:
=.
)


NB. p{} klduge because IE inserts <p> instead of <br> for enter
CSS=: 0 : 0
#rep{color:red}
#filenamed{color:blue;}
*{font-family:<PC_FONTFIXED>;}
p{margin:0;}
)

0 : 0
browser keyboard keydown keypress keyup info

ie+chrome - bs/del/etc not keypress
ff - bs/del/etc are keypress 

    keypress event (charcode,keycode)
        ie      chrome   ff
return  13 13   und 13   0 13
bs      x       x        0 8
del     x       x        0 46
)

JS=: 0 : 0
var ta,rep,readonly,colorflag,saveasx,toid=0,undotoid=0;
var markcaret="&#8203;"; // \u200B
var rwhat="";            // find regexp
var spanmark="<span class=\"mark\" style=\"background-color:#D7D7D7\">";


function ev_body_load()
{
 ce= jbyid("ijs");
 rep= jbyid("rep");
 ta= jbyid("textarea");
 saveasx=jbyid("saveasx");
 ro(0!=ce.innerHTML.length);
 jbyid("num").setAttribute("contenteditable","false");
 jbyid("num").style.background="#eee";
 colorflag=1;
 jresize();
 color();
 undoadd();
}

function ev_body_unload()
{
 ta.value= jtfromh(ce.innerHTML);
 jdoajax(["filename","textarea","saveasx"],"",jevsentence,false);
}

function ro(only)
{
 readonly= only;
 ce.setAttribute("contenteditable",readonly?"false":"true");
 ce.style.background= readonly?"#eee":"#fff";
}

function click(){ta.value= jtfromh(ce.innerHTML);jdoajax(["filename","textarea","saveasx"]);}
function ev_save_click() {click();}
function ev_runw_click() {click();}
function ev_runwd_click(){click();}

function ev_saveasdo_click(){click();}
function ev_saveasx_enter() {click();}

function ev_saveas_click()     {jdlgshow("saveasdlg","saveasx");}
function ev_saveasclose_click(){jhide("saveasdlg");}

function ev_replnew_enter(){;}
function ev_reply_enter(){;}
function ev_replace_click()  {jdlgshow("repldlg","replnew");}
function ev_replclose_click(){jhide("repldlg");}

function ev_findd_click(){jdlgshow("finddlg","what");}

function ev_findclose_click(){jhide("finddlg");}

function ev_ro_click(){ro(readonly= !readonly);}

function ev_color_click()
{
 colorflag=!colorflag;
 color();
}

function ev_number_click()
{
 numberflag=!numberflag;
 number();
}

function ev_c_shortcut(){jscdo("color");}
function ev_n_shortcut(){jscdo("number");}
function ev_q_shortcut(){jscdo("findd");}

var alphanum= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

// return 0 not assign, 1 =:, 2 =.
function asschk(j,s)
{
 while(j<s.length&&" "==s.charAt(j))++j;
 if(j==s.length||s.charAt(j)!="=")return 0;
 if(++j==s.length)return 0;
 var c=s.charAt(j);
 if(c==":")return 1;
 return(c==".")?2:0;
}

function colmark(d,i,s) // mark find
{
 var a=" ",b=" ",m="\u0004"+d+"\u0000";
 var t=jbyid("context").selectedIndex;
 if(0==t)return m;
 if(0!=i)a=s.charAt(i-1);
 if(s.length!=i+d.length)b=s.charAt(d.length+i);
 if(-1!=alphanum.indexOf(a)||-1!=alphanum.indexOf(b))return d;
 switch(t)
 {
  case 1: // name
   return m;
   break;
  case 2: // =. or =:
   return(0==asschk(i+d.length,s))?d:m;
   break;
  case 3: // =:
   return(1==asschk(i+d.length,s))?m:d;
   break;
  case 4: // =.
   return(2==asschk(i+d.length,s))?m:d;
   break;
 }
}

function colhit(d) // syntax color
{
 var t;
 if("'"==d[0])
  t="\u0003";
 else
  if("NB."==d.substr(0,3))
   t="\u0002";
  else
   if(2<d.length)
    t="\u0001";
   else
    return d;
 return t+d+"\u0000";
}

function findmark(t){return('string'==typeof rwhat)?t:t.replace(rwhat,colmark);}

// '...' or '...LF or NB....LF or name.
var COLR=RegExp("('[^'\u000A]*['\u000A])|(NB\..*)|([a-zA-Z][a-zA-Z0-9_\u200B]*[\.])","g");

// caret preserved by
//  inserting ZWSP, manipulating, ZWSP to span id, selecting span id
// rwhat global to mark finds
function color()
{
 var t,mark;
 ce.focus();
 mark=jreplace("ijs",1,markcaret);
 t= ce.innerHTML;
 t=jtfromh(t);
 t=findmark(t);
 if(colorflag)t=t.replace(COLR,colhit);
 t=xhfromt(t);
 rwhat="";
 ce.innerHTML= t;
 if(mark)jsetcaret("caret",0);
}

var XREGHFROMT=RegExp("[ \n<>&\u0000\u0001\u0002\u0003\u0004\u200B]","g");

function xhfromthit(t)
{
 switch(t[0])
 {
 case " ":      return "&nbsp;";
 case "\n":     return "<br>";  
 case "<":      return "&lt;";
 case ">":      return "&gt;";
 case "&":      return "&amp;";
 case "\u0000": return "</span>";
 case "\u0001": return "<span style=\"color:red\">";  
 case "\u0002": return "<span style=\"color:green\">";
 case "\u0003": return "<span style=\"color:blue\">";
 case "\u0004": return spanmark;
 case "\u200B": return spancaret;
 }
}

function xhfromt(d){return d.replace(XREGHFROMT,xhfromthit);}

var numberflag=0;
var spancaret="<span id=\"caret\" class=\"caret\"></span>";

function number()
{
 var t,i,j,b,lines=0;
 if(numberflag)
 {
  t=jtfromh(ce.innerHTML);
  lines=0;
  for(i=0;i<t.length;++i)
   lines+='\n'==t[i];
  t="";
  for(i=1;i<=lines;++i)t+=i+"&nbsp;<br>";
  jbyid("num").innerHTML=t;
  jshow("num");
 }
 else
 {
  jbyid("num").innerHTML="";
  jhide("num");
 }
}

function update()
{
 if(colorflag)color();
 if(numberflag)number();
}

var undos=[];
var undoslen=0;
var undosn=0;
var undostate=0;

function undoadd()
{
 if(readonly)return;
 var t=ce.innerHTML;
 if(t==undos[undos.length-1])return;
 while(undoslen>3000000)
  undoslen-=undos.shift().length;
 undos[undos.length]=t;
 undoslen+=t.length;
 undosn=undos.length;
}

function doset()
{ 
 rep.innerHTML="undo "+undosn+" of "+(undos.length-1);
 var t=undos[undosn];
 ce.innerHTML=t;
 jsetcaret("caret",0);
}

function undo()
{
 var t;
 if(readonly)return;
 if(!undostate){undoadd();undosn=undos.length;undostate=1;}
 --undosn;
 if(undosn>=0)
  doset();
 else
  undosn=0;
}

function redo()
{
 if(readonly)return;
 if(!undostate)return;
 ++undosn;
 if(undosn<undos.length)
  doset();
 else
  undosn=undos.length-1;
}

// ctrl+x comes here for ff, but not for others
// could be simplified - readonly has contenteditable false
function ev_ijs_keypress()
{
 var k=jevev.keyCode;c=jevev.charCode;ctrl=jevev.ctrlKey;
 var f=k>36&&k<41; // left up right down 37 38 39 40
 f=f||ctrl&&c==99; // ctrl+c
 if(readonly&&!ctrl&&!f){if(jisIE)window.event.returnValue=false; return false;}
 if(jsc||0==c||ctrl)return true; // ignore shortcuts,arrows,bs,del,enter,ctrls,etc.
 if(undotoid!=0)clearTimeout(undotoid);
 undotoid=setTimeout(undoadd,2000);
 if(toid!=0)clearTimeout(toid);
 if(colorflag||numberflag)toid=setTimeout(update,100);
 return true;
}

function ev_what_enter(){jscdo("find");}

function ev_refind_click(){jscdo("find");}

function findzwsp(d){return d+"\u200B*";}

function find()
{
 var t=jform.what.value;
 if(0==t.length)return;
 t=t.replace(/./g,findzwsp);
 flags=(jform.matchcase.checked?1:0)?"g":"gi"
 rwhat=RegExp(t,flags);
 color();
}

function ev_find_click(){find();}

function ev_findtop_click(){
 var i,len,nodes;
 find();
 nodes= ce.getElementsByTagName("span");
 len=nodes.length;
 for(i=0;i<len;++i)
 {
  if("mark"==nodes[i].getAttribute("class"))
  {
   jsetcaretn(nodes[i]);
   return;
  }
 }
}

var found;

function ev_findnext_click()
{
 var i,len,nodes,c=0;
 found=0;
 jcollapseselection(0);
 find();
 nodes= ce.getElementsByTagName("span");
 len=nodes.length;
 for(i=0;i<len;++i)
 {
  if("caret"==nodes[i].getAttribute("class"))c=1;
  if(1==c&&"mark"==nodes[i].getAttribute("class"))
  {
   jsetcaretn(nodes[i]);
   found=1;
   return;
  }
 }
}

function ev_findprev_click()
{
 var i,len,nodes,mark=-1;
 jcollapseselection(1);
 find();
 nodes= ce.getElementsByTagName("span");
 len=nodes.length;
 for(i=0;i<len;++i)
 {
  if("caret"==nodes[i].getAttribute("class"))break;
  if("mark"==nodes[i].getAttribute("class"))mark=i;
 }
 if(mark!=-1)jsetcaretn(nodes[mark]);
}

function ev_repldo_click()
{
 jcollapseselection(1);
 ev_findnext_click();
 if(found)
 {
  jreplace("ijs",-1,jbyid("replnew").value);
  ev_findnext_click();
 }
}

function ev_replforward_click()
{
 while(1)
 {
  jcollapseselection(1);
  ev_findnext_click();
  if(!found)break;
  jreplace("ijs",-1,jbyid("replnew").value);
 }
}

// undo z 90, redo y 89
// could be simplified - readonly has contenteditable false
function ev_ijs_keydown()
{
 var c=jevev.keyCode,ctrl=jevev.ctrlKey,shift=jevev.shiftKey;
 if(readonly&&(c==8||c==46||(ctrl&&c==88)||(ctrl&&c==86))) // bs del cut paste undo redo
 {
  if(jisIE)window.event.returnValue=false;
  return false;
 }
 if(ctrl&&!shift)
 {
  if(c==90){undo();return false;}
  if(c==89){redo();return false;}
 }
 if(undostate){undostate=0;rep.innerHTML="";}
 return true;
}

// called with ajax response
function ajax(ts)
{
 rep.innerHTML= ts[0];
 if(2==ts.length&&(jform.jmid.value=="saveasx"||jform.jmid.value=="saveasdo"))
 {
  jhide("saveasdlg");
  jbyid("filenamed").innerHTML=ts[1];
  jbyid("filename").value=ts[1];
 }
}

function ev_ijs_enter(){return true;}

function ev_t_shortcut(){jscdo("ro");}
function ev_r_shortcut(){jscdo("runw");}
function ev_s_shortcut(){jscdo("save");}
function ev_2_shortcut(){ce.focus();}
)
