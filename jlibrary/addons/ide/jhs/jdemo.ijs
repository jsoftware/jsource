NB. jdemo app - gives access to demos
coclass'jdemo'
coinsert'jhs'
demo=: 'jdemo.ijs'

NB. html document body built from HBS sentences
HBS=: 0 : 0
jhma''
jhjmlink''
'demo'     jhmg'demo';1;12
 'jdemo1'  jhml'1  Roll submit'
 'jdemo2'  jhml'2  Roll ajax'
 'jdemo3'  jhml'3  Flip ajax'
 'jdemo4'  jhml'4  Controls/JS/CSS'
 'jdemo5'  jhml'5  Plot'
 'jdemo6'  jhml'6  Grid editor'
 'jdemo7'  jhml'7  Table layout'
 'jdemo8'  jhml'8  Dynamic resize'
 'jdemo9'  jhml'9  Multiple frames'
 'jdemo10' jhml'10 Ajax chunks'
 'jdemo11' jhml'11 Ajax interval timer'
 'jdemo12' jhml'12 WebGL 3d graphics'
jhmz''
jhh1'JHS demos'
desc
)

jev_get=: create NB. browser get request

NB. create page and send to browser
NB. jhr builds response HBS CSS JS
create=: 3 : 0
loadall'' NB. load demo scripts
'jdemo'jhr''
)

desc=: 0 : 0
Select demo from demo menu.<br/><br/>
Demos are simple examples of JHS GUI programming.
Study them to learn how to build your own app.<br><br>

JHS combines J with the power of html, css (styles),
javascript, and the ubiquity of the browser.
J programmers can ride the coattails of www
infrastructure and standards.<br><br>

Apps are built with the JHS framework. A few useful utitlies:
<div class="jcode">
   doch_jhs_''        NB. J utilities
   docjs_jhs_''       NB. javascript overview
   jhbshtml_jdemo1_'' NB. see HBS and html
   jnv_jhs_ 1         NB. toggle name/value display
</div><br/>

Apps have the following general structure.<br>

<div class="jcode">
coclass'appname'
coinsert'jhs'

HBS=: ... NB. J sentences that produce html body
CSS=: ... NB. styles
JS=:  ... NB. javascript (event handlers)

jev_get=: create NB. browser get request

create=: 3 : 0 NB. create page and send to browser
 ...
 'title'jhr...
</div>
)

loadall=: 3 : 0
d=. (<'.ijs'),~each (<'jdemo'),each ":each >:i.12
d=. (<jpath'~addons/ide/jhs/demo/'),each d
for_n. d do.
 try. load__ >n catch. smoutput'load failed: ',>n end.
end.
)
