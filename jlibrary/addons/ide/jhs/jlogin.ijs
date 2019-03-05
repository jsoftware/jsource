coclass'jlogin'
coinsert'jhs'

GOURL=: 'jijx'

HBS=: 0 : 0
login
'<MSG>'
jhtablea
 jhtr 'user: ';'user' jhtext'';15
 jhtr 'pswd: ';'pass' jhpassword'';15
jhtablez
'login'jhb'login'
loggedin
)

login=: 0 : 0
<h1>J login</h1>
)

loggedin=: 0 : 0
<br/><span style="color:red;">
SECURITY! logout when finished<br/>
close all browser windows or run sentence<br/>
<tt>&nbsp;&nbsp;&nbsp;jlogoff_jhs_ 0</tt>
</span>
)

goto=: 3 : 0
NB. ".'jev_get_',(getv'radg'),'_'''''
".'jev_get_',GOURL,'_'''''
)

NB. login not allowed after LIMIT failures
NB. jum sets to _ (multiple users)
LIMIT=: 10

CSS=: 0 : 0
form{margin:20px;}
)

count=: 0

jev_get=: create

ev_login_click=: create

invalid=: '<span style="color:red;">Invalid login COUNT.</span><br><br>'

expires=: 'jcookie=; Mon, 1-Jan-2000 00:00:00 GMT;'

NB. wierd as it runs through a 1st time with a failure to show the page the 1st time
NB. called from core input if cookie required and not set
NB. valid login   - goes to page and does SetCookie
NB. invalid login - shows page with setcookie expires and no-cache
create=: 3 : 0
if. (-.URL-:'jlogin')*.-.URL-:'favicon.ico' do. GOURL=: URL end.
NB.override formtmpl to stay on users desired url
formtmpl=: formtmpl_jhs_ rplc '<LOCALE>';GOURL
p=. PASS
u=. getv'user'
p=. getv'pass'
if. (0~:#PASS)*.(u-:USER)*.p-:PASS do.
 count=: 0
 SETCOOKIE_jhs_=: 1
 PROMPT_jhs_=: '   '
 goto''
else.
 if. count>3 do. 6!:3[3 end. NB. slow down pswd attack
 b=. (jhbs HBS)hrplc 'MSG';(count>0)#invalid rplc 'COUNT';":<:count
 count=: >:count
 t=. hrtemplate rplc (CRLF,CRLF);CRLF,'Cache-Control: no-cache',CRLF,CRLF
 t=. t rplc (LF,LF);LF,'Set-Cookie: ',expires,LF,LF
 htmlresponse t hrplc'TITLE CSS JS BODY';GOURL;(css CSS);(js JS);b
end.
)

JS=: 0 : 0
function ev_body_load()  {jform.user.focus();}
function ev_user_enter() {jsubmit();}
function ev_pass_enter() {jsubmit();}
function ev_login_click(){jsubmit();}
)
