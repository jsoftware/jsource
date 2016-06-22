coclass'jdemo11'
coinsert'jhs'

HBS=: 0 : 0
        jhh1'Ajax interval timer'
'start' jhb 'start'
'stop'  jhb 'stop' 
'tf'    jhtext '';20
        desc
        jhdemo ''
)

create=: 3 : 0
count=: 0
'jdemo11'jhr''
)

jev_get=: create

jev_sys_time=: 3 : 'jhrajax ":count=: >:count'

desc=: 0 : 0
<hr>Javascript interval timer can trigger J execution.
)

CSS=: ''   NB. styles

JS=: 0 : 0 NB. javascript
var interval=null;

function ev_start_click()
{
 if(interval==null)
 {
  timer_event();
  interval= setInterval(function(){timer_event()},1000);
 }
}

function ev_stop_click()
{
 if(interval!=null) clearInterval(interval);
 interval= null;
}

function ajax(){ jbyid("tf").value= rq.responseText;}

function timer_event()
{
 jdoajax([],"","jev_sys_time_"+jform.jlocale.value+"_[0",true); // async
}

)