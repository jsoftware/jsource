NB. J HTTP Server - jijx app
coclass'jijx'
coinsert'jhs'

HBS=: 0 : 0
jhma''
jhjmlink''
'tool'   jhmg'tool';1;7
 'table'   jhmab'table'
 'jd3'     jhmab'plot-d3'
 'print'   jhmab'print'
 'watch'   jhmab'watch'
 'sp'      jhmab'sp'
 'doc'     jhmab'doc'
 'debug'   jhmab'debug'
'tour'     jhmg'tour';1;5
 'plot'    jhmab'plot'
 'spx'     jhmab'spx'
 'demo'    jhmab'demo'
 'app'     jhmab'app'
 'labs'    jhmab'labs'
'help'          jhmg'help';1;10
 'helpwikijhs'   jhmab'wiki JHS'
 'helpwikinuvoc' jhmab'wiki nuvoc'
 'helphelp'      jhmab'help'
 'helpinfo'      jhmab'info'
 'helpvocab'     jhmab'vocabulary'
 'helpconstant'  jhmab'constant'
 'helpcontrol'   jhmab'control'
 'helpforeign'   jhmab'foreign'
 'helpdictionary'jhmab'dictionary'
 'helpstdlib'    jhmab'stdlib'
 'about'         jhmab'about'
'adv'jhmg '>';0;10
jhmz''
jhresize''
'log' jhec'<LOG>'
)

jev_get=: create

NB. move new transaction(s) to log
uplog=: 3 : 0
LOG_jhs_=: LOG,LOGN
LOGN_jhs_=: ''
)

NB. y is J prompt - '' '   ' or '      '
NB. called at start of input
NB. ff/safari/chrome collapse empty div (hence bull)
NB. empty prompt is &bull; which is removed if present from input
urlresponse=: 3 : 0
if. 0=#y do.
 t=. JZWSPU8
 PROMPT_jhs_=: JZWSPU8
else.
 t=. (6*#y)$'&nbsp;'
 PROMPT_jhs_=: y
end.
t=. '<div id="prompt" class="log">',t,'</div>'
d=. LOGN,t
uplog''
if. METHOD-:'post' do.
 if. CHUNKY do.
  CHUNKY_jhs_=: 0
  jhrajax_z d
 else.
  jhrajax d
 end. 
else.
 create''
end.
)

NB. refresh response - not jajax
create=: 3 : 0
uplog''
'jijx' jhr 'LOG';LOG
)

ev_advance_click=: 3 : 0
select. ADVANCE
case. 'spx' do. spx__''
case. 'lab' do. lab 0
case.       do. echo 'no open lab/spx to advance'
end.
)

jloadnoun_z_=: 0!:100

ev_clearrefresh_click=: 3 : 'LOG_jhs_=: '''''

ev_about_click=: 3 : 0
jhtml'<hr/>'
echo JVERSION
echo' '
echo'Copyright 1994-2017 Jsoftware Inc.'
jhtml'<hr/>'
)

rundemo_jhs_=: 3 : 0
t=. 'jdemo',":y
require'~addons/ide/jhs/demo/jdemo',(":y),'.ijs'
select. y
case. 14 do. 'jdemo14;0 0 800 600;'cojhs 'temp' [ temp__=: ?5 12$200
case.    do. open t
end.
)

runapp_jhs_=: 3 : 0
t=. 'app',":y
a=. t,'.ijs'
f=. '~temp/app/',a
1!:5 :: [ <jpath'~temp/app'
(fread '~addons/ide/jhs/app/',a)fwrite f
load f
edit f
if. y=6 do.
 'app6'cojhs'calendar'
else.
 open t
end. 
)

tour=: 4 : 0
jhtml'<hr>'
echo x
spx '~addons/ide/jhs/spx/',y
jhtml'<hr/>'
)

ev_jijs_click=: 3 : 0
edit jnew_jhs_''
)

ev_j1_click=:  3 : 0
'J 1'tour'j1.ijs'
)

ev_j2_click=:  3 : 0
'J 2'tour'j2.ijs'
)

ev_j3_click=:  3 : 0
'J 3'tour'j3.ijs'
)

ev_plot_click=:  3 : 0
'plot tour'tour'plot.ijs'
)

ev_spx_click=:  3 : 0
'spx tour'tour'spx.ijs'
)

NB. default ctrl+,./ handlers
ADVANCE=: 'none'
ev_comma_ctrl =: 3 : 'sp__'''''
ev_dot_ctrl=: ev_advance_click
ev_slash_ctrl  =: 3 : 'i.0 0'
ev_less_ctrl   =: 3 : 'i.0 0'
ev_larger_ctrl =: 3 : 'i.0 0'
ev_query_ctrl =: 3 : 'i.0 0'
ev_semicolon_ctrl =:   3 : 'echo''semicolon'''
ev_colon_ctrl =:       3 : 'echo''colon'''

ev_quote_ctrl_jijx_=: 3 : 0
if. -.'*'e.{."1[13!:18'' do. 'not suspended' return. end.
dbstep''
)

ev_doublequote_ctrl =: 3 : 0
if. -.'*'e.{."1[13!:18'' do. 'not suspended' return. end.
dbstepin''
)

jhjmlink=: 3 : 0
t=.   'jmlink' jhmg 'ide';1;12
t=. t,'jfile'  jhmab'jfile    f^'
t=. t,'jfiles' jhmab'jfiles   k^'
t=. t,JIJSAPP  jhmab'jijs     J^'
t=. t,'jfif'   jhmab'jfif     F^'
t=. t,'jal'    jhmab'jal'
t=. t,'jijx'   jhmab'jijx     j^'
t=. t,'clearwindow'jhmab'clear window'
t=. t,'clearrefresh'jhmab'clear refresh'
t=. t,'clearLS'jhmab'clear LS'
t
)

CSS=: 0 : 0
*{font-family:<PC_FONTFIXED>;}
form{margin-top:0;margin-bottom:0;}
*.fm   {color:<PC_FM_COLOR>;}
*.er   {color:<PC_ER_COLOR>;}
*.log  {color:<PC_LOG_COLOR>;}
*.sys  {color:<PC_SYS_COLOR>;}
*.file {color:<PC_FILE_COLOR>;}
)

NB. *#log:focus{border:1px solid red;}
NB. *#log:focus{outline: none;} /* no focus mark in chrome */

JS=: 0 : 0
var phead= '<div id="prompt" class="log">';
var ptail= '</div>';
var globalajax; // sentence for enter setTimeout ajax
var TOT= 1;     // timeout time to let DOM settle before change
//var TOT= 100; // might need more time on slow devices???

function ev_body_focus(){if(!jisiX)setTimeout(ev_2_shortcut,TOT);}

function ev_body_load()
{
 jseval(false,jbyid("log").innerHTML); // redraw canvas elements
 newpline("   ");
 jresize();
}

var setvkb = function()
{
 var sx = document.body.scrollLeft, sy = document.body.scrollTop;
 var naturalHeight = window.innerHeight;
 window.scrollTo(sx, document.body.scrollHeight);
 VKB= naturalHeight - window.innerHeight;
 window.scrollTo(sx, sy);
 jresize();
}

function setfocus(){jbyid("log").focus();}

// iX must get VKB and resize when log gets focus
// iX does not call onfocus the first time - so we also do setvkb in enter
function jecfocus()
{
 if(jisiX)
 { 
  setTimeout(setvkb(),TOT);
  setTimeout(scrollz(),TOT);
 }
}

// iX must get VKB and resize when log loses focus
function jecblur(){if(jisiX)setTimeout(setvkb(),TOT);}

// remove id - normally 1 but could be none or multiples
// remove id parent if removal of id makes it empty
function removeid(id)
{
 var parnt;
 while(1)
 {
  p= jbyid(id);
  if(null==p) break;
  parnt= p.parentNode;
  parnt.removeChild(p);
  if(0==parnt.childNodes.length) parnt.parentNode.removeChild(parnt);
 }
}

function updatelog(t)
{
 var p,parent,n= document.createElement("div");
 n.innerHTML= t;
 removeid("prompt");
 jbyid("log").appendChild(n);
 setTimeout(scrollz,TOT); // allow doc to update
}

function scrollz()
{
 setfocus(); // required by ff
 if(null==jbyid("prompt"))return;
 jsetcaret("prompt",1);
 jbyid("prompt").scrollIntoView(false);
}

function scrollchunk(){jbyid("chunk").scrollIntoView(false);}

// ajax update window with new output
function ajax(ts)
{
 updatelog(ts[0]);
 jseval(true,ts[0]);
}

// ajax update window with new output
function ev_log_enter_ajax()
{
 updatelog(rq.responseText.substr(rqoffset));
 jseval(true,rq.responseText.substr(rqoffset));
}

function ev_log_enter_ajax_chunk()
{
 // jijx echo marks end of chunk with <!-- chunk --> 
 jbyid("log").blur();
 var i=rq.responseText.lastIndexOf("<!-- chunk -->");
 if(i>rqoffset) // have a chunk
 {
  if(rqoffset==0) removeid("prompt"); // 1st chunk contains input line so must remove original
  rqchunk= rq.responseText.substr(rqoffset,i-rqoffset);
  rqoffset= i+14; // skip <!-- chunk -->
  var n= document.createElement("div");
  removeid("chunk");
  n.innerHTML= rqchunk+'<div id="chunk"></div>';
  jbyid("log").appendChild(n);
  jseval(true,rqchunk);
  setTimeout(scrollchunk,1);
 } 
}

function ev_2_shortcut(){scrollz();}

function newpline(t)
{
 t= t.replace(/&/g,"&amp;");
 t= t.replace(/</g,"&lt;");
 t= t.replace(/>/g,"&gt;");
 t= t.replace(/ /g,"&nbsp;");
 t= t.replace(/-/g,"&#45;");
 t= t.replace(/\"/g,"&quot;");
 updatelog(phead+t+ptail);
}

// function keyp(){jbyid("kbsp").style.display= "block";scrollz();return true;} // space for screen kb

function ev_up_click(){uarrow();}
function ev_dn_click(){darrow();}

// log enter - contenteditable
// run or recall line with caret
// ignore multiple-line selection for now
function ev_log_enter()
{
 var t,sel,rng,tst,n,i,j,k,p,q,recall=0,name;
 if(jisiX)setvkb();
 if(window.getSelection)
 {
  sel= window.getSelection();
  rng= sel.getRangeAt(0);
  if(0!=rng.toString().length)return;
  jdominit(jbyid("log"));
  q=rng.commonAncestorContainer;

  for(i=0;i<jdwn.length;++i) // find selection in dom
   if(q==jdwn[i])break;

  for(j=i;j>=0;--j)   // backup find start DIV/P/BR
  {name=jdwn[j].nodeName;if(name=="DIV"||name=="BR"||name=="P")break;}

  for(k=i+1;k<jdwn.length;++k) // forward to find end DIV/P/BR or end
  {name=jdwn[k].nodeName;if(name=="DIV"||name=="BR"||name=="P")break;}

  rng.setStart(jdwn[j],0);
  recall=!(k==jdwn.length||k==jdwn.length-1);
  if(!k==jdwn.length)
    rng.setEnd(jdwn[k],0);
  else
    rng.setEndAfter(jdwn[k-1],0)
  t= rng.toString();
  t= t.replace(/\u00A0/g," "); // &nbsp;
 }
 else
 {
  p= jbyid("prompt");
  rng= document.selection.createRange();
  t= rng.text;
  if(0!=t.length) return; // IE selection has LF, but ignore for now
  // IE -  move left until CR or NaN - move right til no change
  while(1)
  {
    rng.moveStart('character',-1);
    t= rng.text;
    if(t.charAt(0)=='\r'||isNaN(t.charCodeAt(0))){rng.moveStart('character',1); break;}
  }
  while(1)
  {
    n= rng.text.length; // no size change for CRLF
    rng.moveEnd('character',1);
    if(n==rng.text.length){rng.moveEnd('character',-1); break;}
  }
  t= rng.text;
  if(null!=p)// last line exec vs old line recall
  {
   tst= document.selection.createRange();
   tst.moveToElementText(jbyid("prompt"));
   tst.collapse(true);
   recall= -1!=tst.compareEndPoints("EndToEnd",rng);
  }
 }
 if(recall)
 {
  t= t.replace(/ /g,"\u00A0"); // space -> &nbsp;
  newpline(t);
 }
 else
 {
  adrecall("document",t,"-1");
  globalajax= t;
  setTimeout(TOajax,TOT);
 }
}

// firefox can't do ajax call withint event handler (default action runs)
function TOajax(){jdoajax([],"",globalajax,true);}

function document_recall(v){newpline(v);}

function ev_advance_click(){jdoajax([]);}

function ev_print_click() {jdoajax([]);}
function ev_app_click() {jdoajax([]);}
function ev_doc_click() {jdoajax([]);}
function ev_demo_click(){jdoajax([]);}
function ev_j1_click(){jdoajax([]);}
function ev_j2_click(){jdoajax([]);}
function ev_j3_click(){jdoajax([]);}
function ev_plot_click(){jdoajax([]);}
function ev_table_click(){jdoajax([]);}
function ev_jd3_click(){jdoajax([]);}
function ev_spx_click(){jdoajax([]);}
function ev_watch_click(){jdoajax([]);}
function ev_debug_click(){jdoajax([]);}
function ev_sp_click(){jdoajax([]);}
function ev_spx_click(){jdoajax([]);}
function ev_labs_click(){jdoajax([]);}
function ev_about_click(){jdoajax([]);}
function ev_clearwindow_click(){jbyid("log").innerHTML= "";newpline("   ");}
function ev_clearrefresh_click(){jdoajax([]);}
function ev_clearLS_click(){localStorage.clear();};

function linkclick(a)
{
 w=window.open("",a);
 w.close();
 window.open(a,a);
}

function ev_jfile_click(){linkclick("jfile");}
function ev_jfiles_click(){linkclick("jfiles");}
function ev_jfif_click(){linkclick("jfif");}
function ev_jal_click(){linkclick("jal");}
function ev_jijs_click(){jdoajax([]);}
function ev_jijx_click(){linkclick("jijx");}

function ev_helphelp_click(){linkclick("~addons/docs/help/index.htm")};
function ev_helpinfo_click(){linkclick("~addons/docs/help/user/product.htm")};
function ev_helpvocab_click(){linkclick("~addons/docs/help/dictionary/vocabul.htm")};
function ev_helpwikinuvoc_click(){linkclick("http://code.jsoftware.com/wiki/NuVoc")};
function ev_helpconstant_click(){linkclick("~addons/docs/help/dictionary/dcons.htm")};
function ev_helpcontrol_click(){linkclick("~addons/docs/help/dictionary/ctrl.htm")};
function ev_helpforeign_click(){linkclick("~addons/docs/help/dictionary/xmain.htm")};
function ev_helpdictionary_click(){linkclick("~addons/docs/help/dictionary/contents.htm")};
function ev_helpstdlib_click(){linkclick("~addons/docs/help/user/library.htm")};
function ev_helpwikijhs_click(){linkclick("http://code.jsoftware.com/wiki/Guides/JHS")};

function ev_comma_ctrl(){jdoajax([]);}
function ev_dot_ctrl(){jdoajax([]);}
function ev_slash_ctrl(){jdoajax([]);}
function ev_less_ctrl(){jdoajax([]);}
function ev_larger_ctrl(){jdoajax([]);}
function ev_query_ctrl(){jdoajax([]);}
function ev_semicolon_ctrl(){jdoajax([]);}
function ev_quote_ctrl(){jdoajax([]);}
function ev_colon_ctrl(){jdoajax([]);}
function ev_doublequote_ctrl(){jdoajax([]);}
function ev_close_click(){window.close();} // close ignored if not opened by another window
)
