NB. utils

coclass'jhs'
NB.* see ~addons/ide/jhs/util.ijs for complete information
NB.* 
NB.* jhs locale definitions:
NB.* 

NB.*
NB.* close - close wid
close=: 3 : 0
jjs_jhs_'w=window.open("","',y,'");w.close();'
)

NB.* doc - doc '' or 'html' or 'js'
doc=: 3 : 0
select. y
case. 'html' do.
 NB. should be changed be simpler util.ijs comments
 r=. ''
 t=. <;.2 LF,~fread '~addons/ide/jhs/utilh.ijs'
 for_n. t do.
  n=. >n
  if. 'NB.* '-:5{.n do.
   n=. 5}.n
   i=. n i.'*'
   if. i~:#n do.
    n=.(10{.i{.n),' ',}.i}.n
   end.
   r=. r,n
  end.
  r
 end.
case. 'js'   do.
 NB. should be expanded and use simpler comments similar to util.ijs
 r=. docjsn
 t=. <;.2 LF,~fread jpath'~addons/ide/jhs/utiljs.ijs'
 for_n. t do.
  n=. >n
  if. '//* '-:4{.n do.
   n=. 5}.n
   i=. n i.'*'
   if. i~:#n do.
    n=.(10{.i{.n),' ',}.i}.n
   end.
   r=. r,n
  end.
 end.
 r
case.        do.
 t=. <;.2 LF,~fread '~addons/ide/jhs/util.ijs'
 ;5}.each t#~(<'NB.* ')=5{.each t
end.
)

NB. thanks to Raul Miller for this forum contribution
fmt0=:3 :0 L:0
if.#$y do. ,@(,"1&LF)"2^:(_1 + #@$) ":y else. ":y end.
)

NB.* jbd - jbd 0 or 1 - select boxdraw
jbd=: 3 : '9!:7[y{Boxes_j_' NB. select boxdraw (PC_BOXDRAW)

NB.* jfe - jfe 0 or 1 - toggle jconsole vs JHS as front end
jfe=: 3 : 0
15!:16 y
i.0 0
)

NB.* jlogoff - jlogoff'' - clear login cookie
jlogoff=: 3 : 'htmlresponse hajaxlogoff'

NB.* jhtml - jhtml'<i>foo</i>' - echo html into jijx log
NB.*    jhtml'&lt;font style="color:red;"&gt;A&lt;/font&gt;'
jhtml=: 3 : 0
a=. 9!:36''
9!:37[ 4$0,1000+#y NB. allow lots of html formatted output
smoutput jmarka_jhs_,y,jmarkz_jhs_
9!:37 a
i.0 0
)

NB.* jjs - jjs 'alert("foo");' - eval javascript sentences
NB.*    starting without ';' is evaluated only in ajax
NB.*    starting with ';' is also evaluated in refresh
jjs=:3 : 0
jhtml jmarkjsa,y,jmarkjsz
)

NB.* jsfromtable - jsfromtable i.3 4 - javascript table from J table
NB.*    see tablefromjs - does not handel _123
jsfromtable=: 3 : 0
d=. y
if. 0=L.d do. d=. <"0 d end.
d=. enc_json <"1 d
if. d-:'[]' do. d=. '[[]]' end.
)

NB.* jwatch - 'W1' jwatch '?2 3$100' - watch any expression
jwatch=: 4 : 0
require'~addons/ide/jhs/cojhs/jwatch.ijs'
x gd_set_jwatch_ y
open_jhs_ ('jwatch?jwid=',x);x
)

NB.* open - [xywh] open url [; jwid ]
NB.*    xywh elided is broswer default (probably new tab)
NB.*    xywh gives xywh for new window
NB.*    wh elided defaults to 500 500
NB.*    jwid elided is the same as url
NB.*    open 'jfif'
NB.*    10 10 open'jfif'
NB.*    ?jwid= is only url paramter supported - see kludge below
NB.*    javascript window open URL,jwid,specs,replace
NB.*    jwid is the window id - javascript uses term window name
NB.*    jwid is not the page or tab title
NB.*    NOPOPUP not supported
open=: 3 : 0
''open y
:
assert 0=NOPOPUP
a=. boxopen y
if. 1=#a do. a=. a,a end.
'a b'=. a
if. ''-:x do.
 s=: ''
else.
 'bad xywh' assert (4=3!:0 x+0)*.(1=$$x)*.+./2 4 e.#x
 s=. 'left=<X>,top=<Y>,width=<W>,height=<H>'hrplc 'X Y W H';":each 4{.x,500 500
end.
NB. a=. jurlencode a - should work, but it doesn't - only encode after the ?jwid=
i=. 1 i.~a E. '?jwid='
if. i<#a do.
 i=. i+6
 a=. (i{.a),jurlencode i}.a
end.
JWID=: b
m=. a,' pop-up blocked\nadjust browser settings to allow localhost pop-up\nsee wiki JHS help pop-up section'
jjs'w=window.open("","<Y>");if(null==w)alert("<M>");w.close();window.open("<X>","<Y>","<S>")'rplc '<X>';a;'<Y>';b;'<S>';s;'<M>';m
)

NB.* openreport - openreport'' - report wid,tab,class,locale
openreport=: 3 : 0
t=. conl 1
t=. t#~;(<'jhs')=>1{each copath each conl 1
loc=. tab=. wid=. class=. ''
for_c. t do.
 if. COCREATOR__c-:<'base' do.
  tab=. tab,<JTITLE__c
  wid=. wid,<JWID__c
  loc=. loc,c
  class=. class,{.copath c
 end.
end.
(;:'wid tab class loc'),wid,.tab,.class,.loc
)


NB.* utf8_from_jboxdraw - utf8_from_jboxdraw string - rplc i.11 boxdraw with utf8
utf8_from_jboxdraw=: 3 : 'y rplc (<"0 [11{.16}.a.),.<"1 [11 3$8 u: u:9484 9516 9488 9500 9532 9508 9492 9524 9496 9474 9472'

NB.* printstyle=: 'font-family"courier new";font-size:16px;'
printstyle=: 'font-family:"courier new";font-size:16px;'

NB.* printwidth=: 80 - truncate long lines with ... 
printwidth=: 80

NB.* print - [x] print 23;noun - x default 1 - x 0 for manual print
print=: 3 : 0
1 print y
:
t=. fmt0 ":y
t=. t,;(LF={:t){LF;''
t=. <;._2 t
c=. ;#each t
t=. (c<.printwidth){.each t
t=. t,each ;(c>printwidth){each <'';'...'
t=. ;t,each LF
t=. utf8_from_jboxdraw jhfroma t
x printsub t
)

NB.* printscript - [x] printscript '~temp/abc.ijs' - x same as for print
printscript=: 3 : 0
1 printscript y
:
y=. spf y
t=. toJ fread y
t=. t,;(LF={:t){LF;''
d=. printwidth foldtext each <;._2 t
d=. (' ',~each 5":each <"0 >:i.$d),each d
d=. d rplc each <LF;LF,9#' '
t=. jhfroma y,LF,;d,each LF
x printsub t
)

printsub=: 4 : 0
s=. printstyle rplc '"';''''
jjs'win=window.open("");win.document.write("<style type=''text/css''>pre{',s,'}</style><pre>',y,'<pre/>");',x#'win.print();win.close();'
)

NB.* tablefromjs - tablefromjs '[[1,2,3],[4,5,6]]' 
NB.*    see jsfromtable
tablefromjs=: 3 : 0
d=. >dec_json y
b=. d=<'json_null'
if. *./*./b do. i.0 0 return. end.
d=. (>:(*./"1 b)i:0){.d
d=. (>:(*./ b)i:0){."1 d
if. -.2 e. ,>3!:0 each d do. d=. >d end.
d
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

documented functions:

)

coclass'z'
NB.* 
NB.* z locale definitions:
NB.* 

NB.* cojhs - locale=. 'class;show;title'cojhs data
NB.*    show _ no show, empty default show, x y [w h] window location
NB.*    title - tab title -  empty class default
NB.     'table;10 10;abc'cojhs'n' [ n=. i.2 5
cojhs=: 4 : 0
d=. dltb each<;._2 x,';'
'c s t'=. 3{.d
if. -.(<c)e. conl 0 do. load'~addons/ide/jhs/cojhs/',c,'.ijs' end.
s=. 0".s
'show invalid'assert (_-:s)+.0 2 4 e.~#s
r=. conew c
title__r=: t
create__r y
if. -._-:s do. show__r s end.
r
)

NB.* edit - [xywh] edit'~temp/abc.ijs
NB.*    uses sp shortname
edit=: 3 : 0
''edit y
:
x open_jhs_'jijs?jwid=',jshortname_jhs_ jpath spf y
)

NB.* jd3 - jd3''
jd3=: jd3_jhs_


NB.* decho - decho string - same as echo - easy removal after debugging
decho=: echo_z_

NB.* dechoc - dechoc string - same as echoc - easy removal after debugging
dechoc=: echoc_z_

NB.* echoc - echoc string - echo to console - useful in some kinds of debugging
echoc=: 3 : 0
if. IFJHS do.
 try.
  jbd_jhs_ 1
  jfe_jhs_ 0
  echo y
 catch.
 end.
 jfe_jhs_ 1
 jbd_jhs_ 0
else.
 echo y
end. 
)

NB. in general add new things to jhs locale, not z locale

NB. utils - plot related - not doc documented

NB. f file.png
jhspng=: 3 : 0
d=. fread y
w=. 256#.a.i.4{.16}.d
h=. 256#.a.i.4{.20}.d
t=. '<img width=<WIDTH>px height=<HEIGHT>px src="<FILE><UQS>" ></img>'
jhtml t hrplc_jhs_ 'WIDTH HEIGHT FILE UQS';w;h;y;uqs_jhs_''
)

NB. [TARGET] f URL - url is server page or file with UQS 
jhslink=: 3 : 0
'_blank' jhslink y
:
t=. '<a href="<REF><UQS>" target="<TARGET>" class="jhref" ><TEXT></a>'
t=. t hrplc_jhs_ 'TARGET REF UQS TEXT';x;y;(uqs_jhs_'');y
jhtml'<div contenteditable="false">',t,'</div>'
)

NB. f URL - url is www url
jhslinkurl=: 3 : 0
jhtml'<div contenteditable="false"><a href="http://',y,'" target="_blank">',y,'</a></span>'
)

NB. TARGET f URL
jhsshow=: 3 : 0
'_blank' jhsshow y
:
jjs_jhs_ 'window.open("',(y,uqs_jhs_''),'","',x,'");'
)

plotjijx=: 3 : 0
canvasnum_jhs_=: >:canvasnum_jhs_
canvasname=. 'canvas',":canvasnum_jhs_
d=. fread y
c=. (('<canvas 'E.d)i.1)}.d
c=. (9+('</canvas>'E.c)i.1){.c
c=. c rplc 'canvas1';canvasname
d=. (('function graph()'E.d)i.1)}.d
d=. (('</script>'E.d)i.1){.d
d=. d,'graph();'
d=. d rplc'canvas1';canvasname
jhtml c
jjs_jhs_ ';',d
)

NB. f type;window;width height[;output]
NB. type selects case in plotcanvas/plotcairo
plotdef=: 3 : 0
if. 'cairo'-:_1{::y=. 4{.y,<'canvas' do.
 'CAIRO_DEFSHOW_jzplot_ CAIRO_DEFWINDOW_jzplot_ CAIRO_DEFSIZE_jzplot_ JHSOUTPUT_jzplot_'=: y
else.
 'CANVAS_DEFSHOW_jzplot_ CANVAS_DEFWINDOW_jzplot_ CANVAS_DEFSIZE_jzplot_ JHSOUTPUT_jzplot_'=: y
NB. default output
 JHSOUTPUT_jzplot_=: 'canvas'
end.
i.0 0
)

plotcanvas=: 3 : 0
f=. '~temp/plot.html' NB. CANVAS_DEFFILE
d=. fread f
d=. d rplc'<h1>plot</h1>';''
d=. d rplc'#canvas1 { margin-left:80px; margin-top:40px; }';'#canvas1{margin-left:0; margin-top:0;}'
d fwrite f
w=. CANVAS_DEFWINDOW_jzplot_
select. CANVAS_DEFSHOW_jzplot_
 case. 'show' do. w jhsshow f
 case. 'link' do. w jhslink f
 case. 'jijx' do. plotjijx f
 case. 'none' do.
 case.        do. plotjijx f
end.
i.0 0
)

plotcairo=: 3 : 0
f=. '~temp/plot.png' NB. CAIRO_DEFFILE
w=. CAIRO_DEFWINDOW_jzplot_
select. CAIRO_DEFSHOW_jzplot_
 case. 'show' do. w jhsshow f
 case. 'link' do. w jhslink f
 case. 'jijx' do. jhspng f
 case. 'none' do.
 case.        do. jhspng f
end.
i.0 0
)

jhsuqs=: uqs_jhs_  NB. viewmat
jhtml=: jhtml_jhs_ NB. viewmat

