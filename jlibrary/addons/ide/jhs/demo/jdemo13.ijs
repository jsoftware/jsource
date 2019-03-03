coclass'jdemo13'
coinsert'jhs'

NB. css/js library files to include
INC=: INC_d3_basic

NB. J sentences - create html body
HBS=: 0 : 0
'ha'jhdiva''
 desc
 jhdemo''
 jhhr
 'data'jhtext'<SENTENCE>';20
 'plot'jhb'plot'
 'flip'jhb'v/h'
'</div>' 
'ga'jhd3_basic'' NB. ga,ga_... divs for d3_basic plot
'gb'jhd3_basic'' NB. gb,gb_... divs for d3_basic plot
)

desc=: 'D3 line and bar plots with same data'

NB. additional CSS - float:left allows side by side plots
CSS=: 0 : 0
#ga_box{float:left;}
#gb_box{float:left;}
)

NB. J handlers for app events

jev_get=: 3 : 0
sentence=. '?2 8$100'
data=. jd3data ".sentence
'jdemo13'jhrx(getcss''),(getjs'TABDATA';data),gethbs'SENTENCE';sentence
)

ev_plot_click=: 3 : 0
d=. getv'data'
jhrajax jd3data".d
)

NB. javascript
JS=: 0 : 0
hv= 1;
tabdata="<TABDATA>"; // from jev_get

function ev_plot_click(){jdoajax(["data"]);} // include form data/value in request

function ev_plot_click_ajax(ts)
{
tabdata=ts[0];
resize();
}

function ev_data_enter(){jscdo("plot");}

function ev_flip_click(){hv= hv?0:1;resize();}

function ev_body_load()
{
document.title= window.name;
resize();
window.onresize= resize;
}

function resize()
{
w= window.innerWidth-20;
h= window.innerHeight-20;
h= h-jbyid("ha").clientHeight;
if(hv) w= w/2; else h= h/2;
w= w+"px";
h= h+"px";
jbyid("ga_box").style.width=w;
jbyid("ga_box").style.height=h;
jbyid("gb_box").style.width=w;
jbyid("gb_box").style.height=h;
plot("ga",'type="line"\n'+tabdata);
plot("gb",'type="bar"\n'+tabdata);
}
