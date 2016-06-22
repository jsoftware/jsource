NB. J HTTP Server - jbinfo app - show browswer info
coclass'jbinfo'
coinsert'jhs'

urlget=: 3 : 0
NB.      TITLE CSS JS BO BA FORM BZ
hrbase 'jbinfo';'';'';'';js;'<body>'
)

js=: 0 : 0
document.onkeyup= kup;

function kup(ev)
{
 var e=window.event||ev;
 var t='key up';
 t+= '\nchar : '+e.charCode;
 t+= '\nkey  : '+e.keyCode;
 t+= '\nctrl : '+e.ctrlKey;
 t+= '\nshift: '+e.shiftKey;
 alert(t);
 return true;
}

var b= "Browser info:<br><br>";
for(var pn in navigator)
{
 b+= pn+": "+navigator[pn]+"<br>";
}
b+= "<br>Javascript info:<br><br>";
b+= "all: "+document.all+"<br>";
b+= "getElementById: "+document.getElementById+"<br>";
b+= "addEventListener: "+window.addEventListener+"<br>";
b+= "attachEvent: "+window.attachEvent+"<br>";
b+= "scrollTo: "+window.scrollTo+"<br>";
b+= "XMLHttpRequest: "+(new XMLHttpRequest())+'<br>';
document.write(b);
)

