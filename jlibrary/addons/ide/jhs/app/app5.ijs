coclass'app5' 
coinsert'jhs'

CSS=: 0 : 0
#d1{margin:20px;}
)

NB. INC_d3_basic_jhs - css/javascript library files required by d3_basic
NB. 'INC_'names_jhs_ 0 - INC_... that are supported
INC=: INC_d3_basic

HBS=: 0 : 0
'd1'jhdiva''
     jhh1'app5 - INC - css/js library files'
     jhhr
 'b1'jhb'plot'
 't1'jhtext'?3 4$100';10
 'ga'jhd3_basic''
'</div>'
)

jev_get=: 3 : 0
'app5'jhrx(getcss''),(getjs''),gethbs''
)

ev_b1_click=: 3 : 0
jhrajax jd3data".getv't1'
)

JS=: 0 : 0

function ev_b1_click(){jdoajax(["t1"]);}

function ev_b1_click_ajax(ts)
{
 jbyid("ga_box").style.width="400px";
 jbyid("ga_box").style.height="300px";
 plot("ga",ts[0]);
}

)
