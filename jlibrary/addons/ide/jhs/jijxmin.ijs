NB. minimum jijx

coclass'jijxmin'
coinsert'jhs'

urlget=: mtv

urlresponse=: 3 : 0
LOG_jhs_=: LOG,LOGN
LOGN_jhs_=: ''
f=. form rplc '<SENTENCE>';ht'jsentence';'';20
ba=. '<body onload="evonload();"><font face="monospace">',LOG
bz=. '</font>'
hrstd 'jijxmin';'';'';ba;f;bz;sc
)

form=: 0 : 0
<form id="j" name="j" method="post" action="jijxmin">
<SENTENCE>
</form>
)

sc=: 0 : 0
function evonload(){document.getElementById("jsentence").focus();}
)
