coclass'jdemo3'
coinsert'jhs'

HBS=:  0 : 0
       jhh1'Flip - ajax args and results'
'flip' jhb'flipem'    
't1'   jhtext'some text';10
't2'   jhtext'more text';10
       desc  
       jhdemo''
)

create=: 3 : 0
'jdemo3'jhr''
)

NB. NV_jhs_ has request name/value pairs
NB. getvs't1 t2' returns values for names t1 and t2
NB. smoutput seebox NV can help debugging
NB. JASEP separates ajax results
ev_flip_click=: 3 : 0
smoutput seebox NV
't1 t2'=. getvs't1 t2'
jhrajax (8 u:|.7 u: t1),JASEP,(8 u:|.7 u: t2)
)

jev_get=: create NB. browser get request

JS=: 0 : 0
// send t1 and t2 name/value pairs to J
function ev_flip_click(){jdoajax(["t1","t2"],"");}

// firefox enter in textarea does not trigger default button event
function ev_t1_enter(){jscdo("flip");}
function ev_t2_enter(){jscdo("flip");}

// ts is list of JASEP delimited strings
function ajax(ts)
{
 if(2!=ts.length)alert("wrong number of ajax results");
 jbyid("t1").value=ts[0];
 jbyid("t2").value=ts[1];
}
)

desc=: 0 : 0
<hr>An ajax request sends only required data to the server.
The JHS framework sends values of html form elements
that have been listed in the jdoajax arg. Hidden text
elements are useful for data that should not be visible to
the user.<br><br>

An ajax result can be any string of bytes. The JHS framework
convention is to send strings of data delimited by the byte
value JASEP.
)

