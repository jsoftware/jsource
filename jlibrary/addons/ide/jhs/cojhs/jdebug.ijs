coclass'jdebug'
coinsert'jhs'

shown=: 0

create=: 3 : 0
name=: y
nameq=: y,('_'~:{:y)#'__' NB. unqualified name treated as if in base
namex=: ((}:nameq)i:'_'){.nameq
title=: 'jdebug'
)

show=: 3 : 0
shown=: 1
y open ,~coname''
)

destroy=: 3 : 0
if. shown do. close ;coname'' end.
codestroy''
)

ev_close_click=: 3 : 0
jhrajax''
destroy''
)

HBS=: 0 : 0
jhclose''
'<div>     <NAME></div>'
'sel'jhdiv'<FILES>'
)

CSS=: 0 : 0
form{margin:2px;}
div{font-family:<PC_FONTFIXED>;white-space:pre;}
*.jhab{font-family:<PC_FONTFIXED>;white-space:pre;}
)

buttons=: 3 : 0
'mid sids values sep'=. y
p=. ''
for_i. i.#sids do.
 id=. mid,'*',>i{sids
 p=. p,sep,~id jhab i{values
end.
)

getcurrent=: 3 : 0
t=. 13!:13''
s=. ;8{"1 t
i=. s i. '*'
if. i=#s do. '-1' return. end.
A__=: a=. i{t
if. -.namex-:;{.a do. '-1' return. end. NB. not us - need __ 
n=. ;2{a
((1=#>6{a){'dm'),sidpad{.":n
)

jev_sys_time=: 3 : 0
jhrajax (getdata nameq),JASEP,getcurrent''
)

ev_files_click=: 3 : 0
sid=. getv'jsid'
n=. 0".}.sid
select. {.sid
case. 'm' do.
 if. n e. mstops do. mstops=: mstops-.n else.  mstops=: /:~mstops,n end. 
case. 'd' do.
 if. n e. dstops do. dstops=: dstops-.n else. dstops=: /:~dstops,n end. 
end.
setstops''
jhrajax (getdata nameq),JASEP,getcurrent''
)

setstops=: 3 : 0
t=. 13!:2''
t=. deb each <;.2 t,';'#~';'~:{:t
t=. t#~-.(<namex)=(#namex){.each t
t=. t,<';',~namex,' ',(":mstops),(0~:#dstops)#' : ',":dstops
13!:3 ;t-.<,';'
)

getstops=: 3 : 0
t=. 13!:2''
t=. dltb each <;.2 t,';'#~';'~:{:t
i=. ((#namex){.each t)i:<namex
if. i=#t do.
 mstops=: dstops=: ''
else.
 t=. (#namex)}.;i{t
 i=. t i.':'
 t=. t rplc':';' ';';';' '
 mstops=: /:~0".i{.t
 dstops=: /:~0".i}.t
end.
)

getdata=: 3 : 0
if. -.1 2 3 e.~nc<y do. 'not an explicit definition' return. end.
last=: t=.  <;._2 LF,~5!:5<y
if. 1=#t do. +a. end. NB. single line display

colon=. b=. i=. 0
select. {.>{.t
case. '1';'2' do.
 i=. _2+#t
case. '3' do.
 i=.t i.<,':'
 colon=. i~:#t
 if. colon do.
  i=. i-1
  b=. (#t)-i+3
 else. 
  i=. _2+#t
 end. 
case. '4' do.
 b=. _2+#t
end. 

head=. <"1>(<'a'),('m',each ":each<"0 i.i),(colon#<'b'),('d',each ":each<"0 i.b),<'c'
sidpad=: #":i>.b NB. ids are padded
t=. (}.each head),each' ',each last
getstops''
e=. ((i.i)e.mstops){' x'
r=. ((i.b)e.dstops){' x'
stops=. <"0' ',e,(colon#' '),r,' '
buttons 'files';(<head),(<stops,each ' ',each t),<'<br>'
)

jev_get=: 3 : 0
title jhrx (getcss''),(getjs''),gethbs'NAME FILES';name;getdata''
)

JS=: 0 : 0

var current= -1;

function ajax(ts)
{
 var node;
 node= jbyid("sel");
 node.innerHTML= ts[0];
 
 if(current!=-1)
 {
  node= jbyid("files*"+current);
  node.style.backgroundColor= "white";
 }
 current= ts[1];
 if(current!=-1)
 {
  node= jbyid("files*"+current);
  node.style.backgroundColor= "#D7D7D7";
 }
}

function ev_files_click(){jdoajax();}

function ev_body_load()
{
  timer_event();
  setInterval(function(){timer_event()},750);
}

function timer_event()
{
 jdoajax([],"","jev_sys_time_"+jform.jlocale.value+"_[0",true); // async
}

)
