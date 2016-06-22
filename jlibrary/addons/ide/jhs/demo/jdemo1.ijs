coclass'jdemo1'
coinsert'jhs'

NB. sentences to create html body
NB. jhbshtml_jdemo1_'' shows HBS html
HBS=: 0 : 0
jhh1'Roll with submit'
'roll'   jhb'rollem'
'<RESULT>'
desc
jhdemo''
)

NB. create html page from HBS CSS JS
NB. HBS body can have TAGS that are replaced
create=: 3 : 0
'jdemo1'jhr'RESULT';y NB. <RESULT> tag replaced by y
)

NB. called when browser gets this page
jev_get=: create

NB. event handler - called by js handler jsubmit()
NB. 6 numbers replace <RESULT> tag
ev_roll_click=: 3 : 'create >:6?49'

desc=: 0 : 0
<hr>Pressing rollem button:<br>
calls js function ev_roll_click()<br>
which calls jsubmit()<br>
which calls verb ev_roll_click<br>
which returns a new page<br><br>

The browser sends all form data with the submit request
to the server. The server creates a complete new page and
sends it to the browser. There is little form data in this example
and the page is small so there is not much data sloshing.
But in a real app it could be lots and the app could be sluggish,
especially over a network.
)

CSS=: ''   NB. styles

JS=: 0 : 0 NB. javascript
function ev_roll_click(){jsubmit();} // submit form - calls J ev_roll_click
)