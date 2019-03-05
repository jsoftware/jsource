coclass'app4' NB. 
coinsert'jhs'

CSS=: 0 : 0
form{margin:40px;}             /* CSS comments must be of this form */
#t1{color:red;}
#s1{background-color:aqua;}
)

HBS=: 0 : 0
    jhh1'app4 - CSS'
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
CSS - cascading style sheets - fine tunes document appearance/behavior

skim CSS resources (web or books) to get a basic familiarity
no need to be an expert, but you need to know a bit
cut/paste and following patterns can get you a long ways
search for a well phrased question often gives all the answer you need

form is the html element that is the container for the other elements
css form{...} gives a margin on all sides

t1 has color red and s1 has a backround-color aqua
<pre/>
)

jev_get=: 3 : 0
'app4'jhrx(getcss''),(getjs''),gethbs''
)

ev_b5_click=: 3 : 0
jhrajax 'J result: ',getv't1'
)

ev_b6_click=: 3 : 0
echo NV
jhrajax 'J result: ',(getv't1'),' and ',(getv't2'),' and ',getv'jdata'
)

JS=: 0 : 0

function set(v){jbyid("s1").innerHTML=v;}

function ev_b2_click(){set('b2');}
function ev_b3_click(){set('b3');jdoajax(["t1"]);}
function ev_b4_click(){set('b4');jdoajax(["t1"]);}
function ev_b5_click(){set('b5');jdoajax(["t1"]);}
function ev_b6_click(){         ;jdoajax(["t1","t2"],"extra data");}


function ev_b5_click_ajax(ts){set(ts[0]);}
function ev_b6_click_ajax(ts){set(ts[0]);}
)
