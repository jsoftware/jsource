NB. core javascript utilities for all pages
coclass'jhs'
NB. JS could be src= and with cache would load faster
NB. JS is small and doing it inline (not src=) is easier

0 : 0
onunload/onbeforeunload events would be nice if they worked 'properly'
would allow app to clean up client side and (ajax) server side
there are limitations (ajax, etc.) in the handlers
and nasty cross browser differences
problems mostly in the ajax call

chrome requires onbeforeunload with async false
firefox requires onbeforeunload but doesn't care about async
who knows about safari and other
and if the server is hung then there are other problems

the framework set the unload event the same way it set load event
but it doesn't trigger and something is wrong

the ajax calls with body are suspect - sid "" should probably be null to avoid body*

no current pages really require client/server side cleanup
the main requirement would be a page that required a locale to hold state
that needed to be properly released

this could probably be handled by the framework as follows:

window.onbeforeunload= xxxclose;
function xxxclose(){return dirty?""prefered way to leave this page is menu close or close button ×;null;

a page could manage the dirty flag and do proper shutdown in the close handler
)

JSCORE=: 0 : 0
var JASEP= '\1'; // delimit substrings in ajax response
var jform;       // page form
var jevev;       // event handler event object
var jevtarget=null;   // event handler target object
var AGENT=navigator.userAgent;
var jisIE=-1!=AGENT.search(/MSIE/);
var jisiPhone= -1!=AGENT.indexOf('iPhone');
var jisiPod  = -1!=AGENT.indexOf('iPod');
var jisiPad  = -1!=AGENT.indexOf('iPad');
var jisiX    = jisiPhone||jisiPod||jisiPad;
var VKB      = 0;  // iX kb height
var LS= location.href; // localStorage key
var i= LS.indexOf("#");
if(-1!=i) LS= LS.substring(0,i) // strip off # fragment
LS+= ".";
var logit= "";

function jbyid(id){return document.getElementById(id);}
function jsubmit(s){jform.jdo.value=jevsentence;jform.submit();}

// convert to event for mid [sid=""] [type="click"]
function jscdo(mid,sid,type) // click handler for mid [sid] type
{
 jevtarget=null; 
 jform.jtype.value= type?type:"click";
 if('undefined'==typeof sid)
 {
  jform.jid.value= mid;
  jform.jmid.value=mid;
  jform.jsid.value="";
 }
 else
 {
  jform.jid.value= mid+'*'+sid;
  jform.jmid.value=mid;
  jform.jsid.value=sid;
 }
 jevdo();
}

// set caret in element id - collapse 0 start, 1 end
function jsetcaret(id,collapse)
{
 var p= jbyid(id);
 if(null==p)return;
 if (window.getSelection)
  window.getSelection().collapse(p,collapse);
 else
 {
  var tst= document.selection.createRange();
  tst.moveToElementText(p);
  tst.collapse(!collapse);
  tst.select();
 }
}

function jsetcaretn(node)
{
 if (window.getSelection)
 {
  var sel,rng;
  node.scrollIntoView(false);
  sel=window.getSelection();
  sel.removeAllRanges();
  rng=document.createRange();
  rng.selectNode(node);
  sel.addRange(rng);
 }
 else
 {
  var tst= document.selection.createRange();
  tst.moveToElementText(node);
  tst.select();
 }
}

function jcollapseselection(d)
{
 try
 {
  if (window.getSelection)
  {
   var sel,rng;
   sel=window.getSelection();
   rng=sel.getRangeAt(0);
   sel.removeAllRanges();
   rng.collapse(d);
   sel.addRange(rng);
  }
  else
  {
   var tst;
   tst=document.selection.createRange();
   tst.collapse(d);
   tst.select();
  }
 }catch(e){;}
}

// replace selection with val - collapse -1 none, 0 end, 1 start
function jreplace(id,collapse,val)
{
  try // mark caret location with ZWSP
  {
   if(window.getSelection)
   {
    var sel,rng;
    sel=window.getSelection();
    rng=sel.getRangeAt(0);
    if(collapse!=-1)rng.collapse(collapse);
    sel.removeAllRanges();
    sel.addRange(rng);
    document.execCommand("insertHTML",false,val);
   }
   else
   {
    var rng;
    rng= document.selection.createRange();
    if(collapse!=-1)rng.collapse(collapse);
    rng.pasteHTML(val);
   }
   return 1;
  }catch(e){return 0;}
}

function jtfromhhit(t)
{
 switch(t[1])
 {
 case "n":  return " ";
 case "l":  return "<";
 case "g":  return ">";
 case "a":  return "&";
 }
}

function jtfromh(d)
{
 d= d.replace(/\r|\n/g,"");          // IE requires
 d= d.replace(/<br><div>/g,"<div>"); // chrome - kludge (not i)
 d= d.replace(/<p>&nbsp;<\/p>|<div><br><\/div>|<br[^>]*>|<\/p>|<div>/gi,"\n");
 d= d.replace(/<\/?[^>]+(>|$)/g,""); // remove all remaining tags
 d= d.replace(/&nbsp;|&lt;|&gt;|&amp;/g,jtfromhhit);
 if('\n'!=d[d.length-1]){d=d+"\n";}
 return d
}

var JREGHFROMT=RegExp("[ \n<>&]","g");

function jhfromthit(t)
{
 switch(t[0])
 {
 case " ":  return "&nbsp;";
 case "\n": return "<br>";  
 case "<":  return "&lt;";
 case ">":  return "&gt;";
 case "&":  return "&amp;";
 }
}

function jhfromt(d){return d.replace(JREGHFROMT,jhfromthit);}

//* jshow(id)
function jshow(id){jbyid(id).style.display="block";jresize();}

//* jhide(id)
function jhide(id){jbyid(id).style.display="none";jresize();}

//* jdlgshow(id,focus) - id to show and id to focus
function jdlgshow(id,focus)
{
 if(jbyid(id).style.display=="block")
  jhide(id);
 else
 {
  jshow(id);
  jbyid(focus).focus();
 }
}

var jdwn;     // dom walk nodes
var jdwi;     // dom walk indents
var jdwr;     // dom walk recursion level

function jdominit(node)
{
 jdwn=[];
 jdwi=[];
 jdwr=0;
 jdomwalk(node);
}

function jdomwalk(node)
{
 var c,n,i;
 ++jdwr;
 c=node.childNodes;
 for(i=0;i<c.length;++i)
 {
  n=c[i];
  jdwi.push(jdwr);
  jdwn.push(n);
  jdomwalk(n);
 }
 --jdwr;
}

function jdomshow(node)
{
 var i,j,t="";
 jdominit(node);
 for(i=0;i<jdwn.length;++i)
 {
  for(j=0;j<jdwi[i];++j)t+="| ";
  name=jdwn[i].nodeName;
  if(name=="#text")
   t+=name+" "+jdwn[i].nodeValue;
  else
   t+=name;
  t+="\n";
 }
 return t;
}

// framework internals
window.onresize=jresize;

var jevsentence; // J sentence to run
var JEV;         // js handler to call

function jresize()
{
 logit+= "B";
 // IE resize multiple frames sometimes gets id as null
 if(jbyid("jresizea")==null||jbyid("jresizeb")==null)return;
 var a= jgpwindowh(); // window height
 
 logit+= " a"+a;
 
 a-= jgpbodymh();     // body margin h (top+bottom)
 a-= jgpdivh("jresizea"); // header height
 a-= 6               // fudge extra
 a-= VKB; // virtual keyboard - does not resize but overlays 
 a=  a<0?0:a;        // negative causes problems
 
 jbyid("jresizeb").style.height= a+"px";
}

function jevload()
{
 jform= document.j;
 jevsentence= "jev_"+jform.jlocale.value+"_ 0"; 
 jscdo("body","","load");
 return false
}

function jevunload(){jscdo("body","","unload");return false;}
 
function jevfocus()
{
 // return false; // IE onfocus before onload
 if(jform=="")return false;
 jscdo("body","","focus");
 return false;
}

// event handler onclick etc - id is mid[*sid]
function jev(event){
 jmenuhide(event);
 jevev= window.event||event;
 jevtarget=(typeof jevev.target=='undefined')?jevev.srcElement:jevev.target;
 var type=jevev.type;
 jform.jtype.value= type;
 var id=jevtarget.id;
 var i= id.indexOf('*');
 jform.jid.value  = id;
 jform.jmid.value = (-1==i)?id:id.substring(0,i);
 jform.jsid.value = (-1==i)?"":id.substring(++i,id.length);
 if(type=='keydown'&&27==jevev.keyCode)return false; // IE ignore esc
 if(type=='keydown'&&13==jevev.keyCode&&!jevev.ctrlKey&&!jevev.shiftKey)
  {jform.jtype.value="enter";return jevdo();} 
 return jevdo();
}

function jevdo()
{
 JEV= "ev_"+jform.jmid.value+"_"+jform.jtype.value;
 // alert(JEV);
 //try{eval(JEV)}
 //catch(ex)
 if('undefined'==eval("typeof "+JEV))
 {
  // undef returns true or does jsubmit for buttons 
  if(null==jevtarget)return true;
  var c=jevtarget.getAttribute("class");
  if(c=="jhb"||c=="jhab"||c=="jhmab")jsubmit();
  return true;
 }
 try{var r= eval(JEV+"();")}
 catch(ex){alert(JEV+" failed: "+ex);return false;}
 if('undefined'!=typeof r) return r;
 return false;
}

// ajax
var rq,rqchunk,rqstate=0,rqoffset=0;

// xmlhttprequest not supported in kongueror 3.2.1 (c) 2003
function newrq()
{
 try{return new XMLHttpRequest();} catch(e){}
 try{return new ActiveXObject("Msxml2.XMLHTTP.6.0");} catch(e){}
 try{return new ActiveXObject("Msxml2.XMLHTTP.3.0");} catch(e){}
 try{return new ActiveXObject("Msxml2.XMLHTTP");} catch(e){}
 alert("XMLHttpRequest not supported.");
}

// ajax request to J
//  ev_mid_type() -> jdoaajax(ids,data)
//   -> ev_mid_type=:  (getv...)
//    -> jhrajax (JASEP delimited responses)
//      -> jdor -> ajax(ts) or ev_mid_type_ajax(ts)
//         ts is array of JASEP delimited responses
// ids is array of form element names (values)
// data is JASEP delimited data to send 
// sentence (usually elided to use jevsentence)
// async is true for asynch and false for synch
// default is synch
function jdoajax(ids,data,sentence,async)
{
 if(0!=rqstate)
 { 
  // return; // previously - alert("busy - wait for previous request to finish");
  if(null!=jbyid("status-busy")) jbyid("status-busy").style.display="block";
  return;
 }
 async= (!async)?false:async;
 sentence=sentence||jevsentence;
 data=data||"";
 ids=ids||[];
 rq= newrq();
 rqoffset= 0;
 if(async) rq.onreadystatechange= jdor;
 rq.open("POST",jform.jlocale.value,async); // true for async call
 jform.jdo.value= ('undefined'==typeof sentence)?jevsentence:sentence;
 rq.send(jpostargs(ids)+"&jdata="+jencode(data)+"&jwid="+jencode(window.name));
 if(!async)jdor();
}

// return post args from standard form ids and extra form ids
function jpostargs(ids)
{
 var d,t="",s="",a=["jdo","jtype","jmid","jsid"].concat(ids);
 for(var i=0;i<a.length;++i)
 {
  d= eval("jform."+a[i]+".value");
  t+= s+a[i]+"="+jencode(d);
  s= "&";
 }
 return t;
}

function jencode(d){return(encodeURIComponent(d)).replace("/%20/g","+");}

// rprocess ajax response(s) from J
// 0 or more state 3 and then state 4
// response header (Transfer-Encoding: chunked) indicates chunked

// not chunked - call ev_mid_type_ajax(ts) - if not defined call ajax(ts)
// ts is rq.responseText split on JASEP
// function can use rq.responseText directly instead of ts

// chunked - ev_mid_type_ajax_chunk() for each state 3 and ev_mid_type_ajax() for state 4
// ..._ajax_chunk and ..._ajax must keep track of what parts of rq.responseText to use
// meaningful chunk boundaries do not match with how responseText data is received
function jdor()
{
 var d,f;
 rqstate= rq.readyState;
 f= "ev_"+jform.jmid.value+"_"+jform.jtype.value+"_ajax";
 
 if(rqstate==3&&null!=rq.getResponseHeader("Transfer-Encoding")) // chunked
 {
  f+= "_chunk";
  if("function"==eval("typeof "+f))
    try{eval(f+"()")}catch(e){alert(f+" failed: "+e);}
  return;
 }
 
 if(rqstate==4)
 {
  if(null!=jbyid("status-busy")) jbyid("status-busy").style.display="none";
  if(200!=rq.status)
  {
   if(403==rq.status)
    location="jlogin";
   else
   {
    var t="ajax request failed\n"
    t+=   "response code "+rq.status+"\n";
    t+=   "application did not produce result\n"
    t+=   "try browsing to url again\n"
    t+=   "additional info in jijx"
    alert(t);
   }
  }
  else
  {
   d=rq.responseText.split(JASEP);
   if("function"==eval("typeof "+f))
    f+="(d)";
   else
    f="ajax(d)";
   try{eval(f)}catch(e){alert(f+" failed: "+e);}
  }
  rqstate= 0; rqoffset= 0;
 }
}

// app keyboard shortcuts

document.onkeyup= keyup; // bad things happen if this is keydown
document.onkeypress= keypress;

var jsc= 0;

function jscset(){jsc=1;}

// page redefines to avoid std shortcuts
// '2' shortcut implemented as ev_2_shortcut for each page
function jdostdsc(c)
{
 switch(c)
 {
  case '1': jactivatemenu('1'); break;
  case 'j': window.open("jijx",TARGET);  break;
  case 'f': window.open("jfile",TARGET); break;
  case 'k': window.open("jfiles",TARGET); break;
  case 'h': window.open("jhelp",TARGET); break;
  case 'J': window.open("jijs",TARGET); break;
  case 'F': window.open("jfif",TARGET); break;
 }
}

// IE/FF see esc etc but Chrome/Safari do not
function keypress(ev)
{
 var e=window.event||ev;
 var c=e.charCode||e.keyCode;
 
 if(jisiX&&c==223){jsc=1;return false;}  // option+s or press letter s and slide right
 // if(c==96){alert("???";return false;} // press ` for debug alert info
 
 var s= String.fromCharCode(c);
 if(!jsc)return true;
 jsc=0;
 try{eval("ev_"+s+"_shortcut()");}
 catch(e){jdostdsc(s);}
 return false;
}

function keyup(ev)
{
 var e=window.event||ev;
 var c=e.keyCode;
 if(e.ctrlKey)
 {
  if(c==188){jscdo(e.shiftKey?"less":"comma",undefined,"ctrl");return false;}
  if(c==190){jscdo(e.shiftKey?"larger":"dot",undefined,"ctrl");return false;}
  if(c==191){jscdo(e.shiftKey?"query":"slash",undefined,"ctrl");return false;}
  if(c==38&&e.shiftKey&&'function'==typeof uarrow){uarrow();return false;}
  if(c==40&&e.shiftKey&&'function'==typeof darrow){darrow();return false;}
 }
 if(c==27&&!e.shiftKey&&!e.altKey)
 {
  jsc=!jsc;return !jsc;
 }
 return true;
}

// return menu group node n
function jfindmenu(n)
{
 var nodes= document.getElementsByTagName("a");
 var i,node,cnt=0,last,len= nodes.length;
 for(i=0;i<len;++i)
 {
  node= nodes[i];
  if("jhmg"!=node.getAttribute("class")) continue;
  if(n==++cnt) return node;
  last= node;
 }
 return last;
}

// tar is current node
// c is 37 38 39 40 - left up right down
// navigate to node based on c and focus
function jmenunav(tar,c)
{
 var i,n,nn,nc,node,cnt=0,last,len,cl,m=[];
 var nodes=document.getElementsByTagName("a");
 len=nodes.length
 for(i=0;i<len;++i)
 {
  node=nodes[i];
  cl=node.getAttribute("class");
  if("jhmg"==cl||"jhml"==cl||"jhmab"==cl)
  {
   m[m.length]=node;
   if(tar==node)n=i;
  }
 }
 len=m.length;
 nn=m[n];                        // nn node
 nc= m[n].getAttribute("class"); // nc class
 if(c==39) // right
 {
  for(i=n+1;i<len;++i)
   if(jmenunavfocushmg(m,i))return;
  jmenunavfocushmg(m,0); 
 }
 else if(c==37) // left
 {
  for(n;n>=0;--n) // back n up to current group
   if(1==jmenunavinfo(m,n))break;
  for(i=n-1;i>=0;--i)
   if(jmenunavfocushmg(m,i))return;
  for(i=len-1;i>=0;--i) // focus last hmg
   if(jmenunavfocushmg(m,i))return;
 }
 else if(c==38) // up
 {
  if("jhmg"==nc) return;
  if(jmenunavfocus(m,n-1))return;
  else
  {
   for(i=n;i<len;++i) // forward to hmg then back one
    if(2!=jmenunavinfo(m,i))break;
   jmenunavfocus(m,i-1);
   return;
  }
 }
 else if(c==40) // dn
 {
  if("jhmg"==nc)
  {
   jmenuhide();
   nn.focus();
   jmenushow(nn);
  }
  if(jmenunavfocus(m,n+1))return;
  else
  {
   for(i=n;i>=0;--i) // back up to hmg then forward 1
    if(1==jmenunavinfo(m,i))break;
   jmenunavfocus(m,i+1);
   return;
  }
 }
}

// focus,show if hmg - return 1 if focus is done
function jmenunavfocushmg(m,n)
{
 if(1!=jmenunavinfo(m,n))return 0;
 m[n].focus();jmenushow(m[n]);
 return 1;
}

// focus if hml/jmab - return 1 if focus is done
function jmenunavfocus(m,n)
{
  if(2!=jmenunavinfo(m,n))return 0;
  m[n].focus();
  return 1;
}

// return m[n] info - 0 none, 1 hmg, 2 hml or hmab
function jmenunavinfo(m,n)
{
 if(n==m.length)return 0;
 return ("jhmg"==m[n].getAttribute("class"))?1:2
}

// activate menu group n
function jactivatemenu(n)
{
 jmenuhide();
 var node= jfindmenu(n);
 if('undefined'==typeof node) return;
 node.focus(); 
}

var menublock= null; // menu ul element with display:block
var menulast= null;  // menu ul element just hidden 

function jmenuclick(ev)
{
 jmenuhide(ev);
 var e=window.event||ev;
 var tar=(typeof e.target=='undefined')?e.srcElement:e.target;
 var id=tar.id;
 var idul= id+"_ul";
 jbyid(id).focus(); // required on mac
 if(jbyid(idul).style.display=="block")
 {
  menublock= null;
  jbyid(idul).style.display= "none";
 }
 else
 {
  if(menulast!=jbyid(idul))
  {
   menublock= jbyid(idul);
   menublock.style.display= "block";
  }
 }
}

function jmenushow(node)
{
 jmenuhide();
 var id=node.id;
 var idul= id+"_ul";
 menublock= jbyid(idul);
 menublock.style.display= "block";
}

function jmenuhide()
{
 if(tmenuid!=0) clearTimeout(tmenuid);
 tmenuid= 0;
 menulast= menublock;
 if(menublock!=null) menublock.style.display= "none";
 menublock= null;
 return true;
}

// browser differences
//  safari/chrome onblur on mousedown and onfocus on mouseup
//  onblur will hide the menu 250 after mousedown (no clear)
//  so menu item click needs to be quick

var tmenuid= 0;

function jmenublur(ev)
{
 if(tmenuid!=0) clearTimeout(tmenuid);
 tmenuid= setTimeout(jmenuhide,500)
 return true;
}

function jmenufocus(ev)
{
 if(tmenuid!=0) clearTimeout(tmenuid);
 tmenuid= 0;
 return true;
}

function jmenukeydown(ev)
{
 var e=window.event||ev;
 var c=e.keyCode;
 return(c>36&&c<41)?false:true;
}

function jmenukeypress(ev)
{
 var e=window.event||ev;
 var c=e.keyCode;
 return(c>36&&c<41)?false:true;
}

function jmenukeyup(ev)
{
 var e=window.event||ev;
 var c=e.keyCode;
 if(c<37||c>40)return false;
 var tar=(typeof e.target=='undefined')?e.srcElement:e.target;
 jmenunav(tar,c);
 return true;
}


// get pixel... - sizing/resizing

// window.onresize= resize; // required for resize
// and resize should be called in ev_body_load

// the ...h functions need simple changes to
// become the corresponding set of w functions

// body{background:aqua} can be useful in
// finding out why calculations turn out wrong

// IF and FF both have bugs with <h1>...</h1>
// vs these calculations and <hx> should not
// be used where resizing will be used

// get pixel window height
function jgpwindowh()
{
 if(window.innerHeight)
  return window.innerHeight; // not IE
 else
  return document.documentElement.clientHeight;
}

// get pixel body margin height (top+bottom)
function jgpbodymh()
{
 var h;
 if(window.getComputedStyle)
 {
  h=  parseInt(window.getComputedStyle(document.body,null).marginTop);
  h+= parseInt(window.getComputedStyle(document.body,null).marginBottom);
 }
 else
 {
  h=  parseInt(document.body.currentStyle.marginTop);
  h+= parseInt(document.body.currentStyle.marginBottom);
 }
 return h;
}

// get pixel div height - IE/FF bugs vs <h1>
function jgpdivh(id){return jbyid(id).offsetHeight;}

/*
function jgpdivh(id)
{
 var e=jbyid(id);
 if(e==null)return 50;
 //alert(e+" "+id);
 // alert(e.offsetHeight);
 //return jbyid(id).offsetHeight;
 var v=e.offsetHeight;
 //alert(id+" "+v);
 return v;
}
*/

// get pixel end

// debug

// numbers from unicode

function debcodes(t)
{
 r= "";
 for(var i=0;i<t.length;++i)
  r= r+" "+t.charCodeAt(i);
 return r;
}

// eval js sentences in s
// a is true in ajax and false in refresh
function jseval(ajax,s)
{
 var i,j,a,z,q;
 a= "<!-- j html output a --><!-- j js a --><!-- ";

 z= " --><!-- j js z --><!-- j html output z -->";
 while(0!=s.length)
 {
  i= s.indexOf(a);
  if(-1!=i)
  {
   i+= a.length;
   j= s.indexOf(z);
   q= s.substring(i,j);
   s= s.substring(j+z.length);
   
   if('!'==q.charAt(0))
   {
    var cmd= q.split(" ");
    if(4==cmd.length&&cmd[0]=="!open")
     window.open(cmd[1]+cmd[3],cmd[2]);
   }
   else
   {
    if(ajax||';'==q.charAt(0))
     try{eval(q);}catch(e){alert(e+"\n"+q);}
   }  
  }
  else
   s= "";
 }
}

// 784 adsf    das f a sdf  a sdf a sdf asdf asdf asdf a

function getlstf(key)
{
// var t= getls(key);
// return (t==null || t=="true")?1:0;
 return 1;
}

function getls(key){return localStorage.getItem(LS+key);}

function setls(key,v){localStorage.setItem(LS+key,v);}

function adrecall(id,a,start)
{
 setls(id+".index",start);
 if(0==a.length) return;
 var t= getls(id);
 if(t==null) t= "";
 var i,blank=0,same=0;
 for(i=0;i<a.length;++i)
  blank+= ' '==a.charAt(i);
 a= a+"\n";
 for(i=0;i<a.length;++i)
  same+= a.charAt(i)==t.charAt(i);
 if(blank!=a.length && same!=a.length)
 {
  t= a+t;
  if('\n'==t.charAt(t.length-1))
   t= t.substring(0,t.length-1); // drop trailing \n so split works
  if(1000<t.length)t= t.substring(0,t.lastIndexOf("\n"));
  setls(id,t);
 }
}

function uarrow(a){udarrow(a,1);}
function darrow(a){udarrow(a,0);}

function udarrow(a,up)
{
 var a,id,v,t,n;
 if(a==null) a= document.activeElement;
 if(a==null || a.type!="text")
  id= "document";
 else 
  id= a.id; 
 t= getls(id);
 n= getls(id+".index");
 if(t==null || n==null)return;
 t= t.split("\n");
 if(up)
 {
  if(++n>=t.length) n= t.length-1;
  if(n==-1)
   v= t[0];
  else
   v= t[n];
 }
 else
 {
  if(--n<0)
   {n= 0; v= t[0];}
  else
   v= t[n];
 }
 setls(id+".index",n);
 if(id=="document")
  document_recall(v);
 else
  a.value= v; 
}

function setlast(id)
{
  var a= jbyid(id);
  setls(id+".index","-1");
  udarrow(a,1);
}

)

docjs=: 3 : 0
docjsn
)

docjsn=: 0 : 0

see ~addons/ide/jhs/utiljs.ijs for complete information

html element ids are mid[*sid] (main id and optional sub id)

functions defined by you:

ev_body_load()   - page load (open)
ev_body_unload() - page unload (close)
ev_body_focus()  - page focus
  
ev_mid_event() - event handler - click, change, keydown, ...

js event handler:
  jevev is event object
  event ignored if not defined
  jsubmit() runs J ev_mid_event and response is new page
  jdoajax([...],"...") runs J ev_mid_event
    ajax(ts) (if defined) is run with J ajax response
    ev_mid_event_ajax(ts) is run if ajax() not defined
  returns true (to continue processing) or false
)

0 : 0
/* contenteditable to/from text
IE:
 <BR>             <-> N (\n)
 </P>              -> N
 <P>&nbsp;</P      -> N (can not tell emtpy from 1 blank)
 can have \r\n !

Chrome:
 <br>            <-> N
 <div>            -> N
 <div><br></div>  -> N
 saw nested divs, but do not know how to get them
 starting div so break on div rather than /div
 
FF:
 <br>            <-> N
 has (and needs) <br> at end

Portable rules (all case insensitive):
 remove \r \n
 <p>&nbsp;</p>    -> N
 <div><br></div>  -> N
 <br>            <-> N
 </p>             -> N
 </div>           -> N
 always have <br> at end (read/write)
 &lt;...         <-> < > & space
*/
)
