coclass'jdemo6'
coinsert'jhs'

HBS=: 0 : 0
jhh1'Grid Demo'
jhhr
jgridnumedit'g0';'';'';'gdata0__'
jhhr
jgridnumedit'g1';'';'';'gdata1__'
jhhr
desc
jhdemo''
)

create=: 3 : 0
CSS=: jgridnumeditcss'g0';'80px'
CSS=: CSS,jgridnumeditcss'g1';'40px'
'jdemo6'jhr''
)

jev_get=: create NB. browser get request

ev_g0_dd_enter=: gup
ev_g1_dd_enter=: gup

gup=: 3 : 0
mid=. getv'jmid'
sid=. getv'jsid'
gid=. mid{.~mid i.'_'
name=. getv gid,'_hh'
r=. (sid i.'*'){.sid
c=. (>:sid i.'*')}.sid
v=. {.0".getv gid,'_vv' NB. new grid cell value
d=. ".name 
d=. v (P=: <(".r);".c)}d
".name,'=: d'
ctotal=. ":(".c){+/d
rtotal=. ":(".r){+/"1 d
d=. mid,'*',sid,JASEP,(":v),JASEP,gid,'_cf*0*',c,JASEP,ctotal,JASEP,gid,'_rf*',r,'*0',JASEP,rtotal
NB. htmlresponse d,~hajax rplc '<LENGTH>';":#d
jhrajax d
)

desc=: 0 : 0
<hr>Edit global nouns gdata0__ and gdata1__.<br><br>

The grid numeric editor uses ajax. When a cell value is
changed, just 3 numbers (value,row,column) are sent to
to the server. The server updates the noun, calculates
new totals, and sends back 3 numbers
(possibly corrected value,new column total,new row total),
and then javascript updates just the affected cells.
)

JS=: 0 : 0
function gup()
{
 var t= jform.jmid.value;
 var gid= t.substring(0,t.indexOf("_"));
 jform.jtype.value= 'enter'; // change becomes enter
 jbyid(gid+"_vv").value= jbyid(gid+"_dd*"+jform.jsid.value).value;
 jdoajax([gid+"_hh",gid+"_vv"],"");
}

function ev_g0_dd_enter(){gup();}
function ev_g1_dd_enter(){gup();}

// leaving changed cell (tab,mouse,...)
// process as enter and return true to continue tab or ...
function ev_g0_dd_change(){gup();return true;}
function ev_g1_dd_change(){gup();return true;}

function ajax(ts)
{
 if(6!=ts.length)alert("wrong number of ajax results");
 jbyid(ts[0]).value=ts[1];
 jbyid(ts[2]).value=ts[3];
 jbyid(ts[4]).value=ts[5];
}

)

