coclass'app2' NB. 
coinsert'jhs'

HBS=: 0 : 0
    jhh1'app2 - JS - javascript event handlers'
    jhhr
'b1'jhb'b 1'
'b2'jhb'b 2' 
't1'jhtext'text field 1';10
't2'jhtext'text field 2';10
's1'jhspan'this is text'
    desc
)

desc=: 0 : 0
<pre>
javascript responds to events and manipulates html elements

JS noun defines the javascript for the app

skim JAVASCRIPT resources (web or books) to get a basic familiarity
no need to be an expert, but you need to know a bit
cut/paste and following patterns can get you a long ways
search for a well phrased question often gives all the answer you need

study the javascript below

try a simple change such as changing the text set when b2 is pressed
run the script, F5 refresh the app, press b2 to see your change

b1 - alert - handler not defined
b2 - s1 set                 
t1 enter - alert - handler not defined 
t2 enter - s1 set

Firefox has a good javascript debugger (crtl+shift+s)
search 'firefox javascript debugger' and read a bit
if you are familiar with visual debuggers it will be easy
to set breakpoints, step through code, and examine data
<pre>
)

NB. J verb that responds to browser request for app1 page
jev_get=: 3 : 0
'app2'jhrx(getcss''),(getjs''),gethbs''
)

JS=: 0 : 0
// javascript code
// gotchas - do not use NB. or =. or =: in javascript!
// strings quoted with ' or "  - "a b" - 'a b' - "don't"

function set(v){jbyid("s1").innerHTML=v;} // utility to set text in html element s1

function ev_b2_click(){set('b2 button was pressed');}
function ev_t2_enter(){set('text2 enter');}
)
