coclass'app3'
coinsert'jhs'

HBS=: 0 : 0
    jhh1'app3 - J event handlers'
    jhhr
'b1'jhb'b 1'
'b2'jhb'b 2' 
'b3'jhb'b 3'
'b4'jhb'b 4'
'b5'jhb'b 5'
'b6'jhb'b 6'
't1'jhtext't1';10
't2'jhtext't2';10
's1'jhspan'this is text'
    desc
)

desc=: 0 : 0
<pre>

ajax is the javascript mechanism for calling a J handler

study the J handlers

b1 - alert - javascript handler not defined
b2 - set html element s1                  
b3 - alert - J did not provide ajax response - press enter in jijx tab to see J error
b4 - alert - javascript did not handle J repsponse to ajax request
b5 - send t1 to J and set J response in s1
b6 - send t1,t2,and "extra data" to J  and set J response in s1
</pre>
)

jev_get=: 3 : 0
'app3'jhrx(getcss''),(getjs''),gethbs''
)

ev_b4_click=: 3 : 0
jhrajax 'J result: ',getv't1'
)

ev_b5_click=: 3 : 0
jhrajax 'J result: ',getv't1'
)

ev_b6_click=: 3 : 0
echo NV
jhrajax 'J result: ',|.(getv't1'),' and ',(getv't2'),' and ',getv'jdata'
)

JS=: 0 : 0

function set(v){jbyid("s1").innerHTML=v;} // set text in html element s1

// javascript event handlers
function ev_b2_click(){set('b2 button pressed');}
function ev_b3_click(){set('b3');jdoajax(["t1"]);} // run J ev_button_click verb - t1 is arg
function ev_b4_click(){set('b4');jdoajax(["t1"]);}
function ev_b5_click(){set('b5');jdoajax(["t1"]);}
function ev_b6_click(){set('b6');jdoajax(["t1","t2"],"extra data");}

// javascript handlers for J responses to ajax requests
function ev_b5_click_ajax(ts){set(ts[0]);}
function ev_b6_click_ajax(ts){set(ts[0]);}
)
