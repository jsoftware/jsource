coclass'jdemo5'
coinsert'jhs'

require '~addons/ide/jhs/jgcp.ijs'

HBS=: 0 : 0
           jhh1'Plot demo'
'sentence' jhtext'<SENTENCE>';20
           jhhr
           '<PLOT>'
           jhdemo''
)

create=: 3 : 0
'jdemo5'jhr'SENTENCE PLOT';y
)

jev_get=: 3 : 0 NB. browser get request
s=. '10?1000'
create s;jgcimg'reset xline &chtt=demo+plot'jgc ".s
)

NB. javascript handlers default to post J handlers
ev_sentence_enter=: 3 : 0
s=. getv'sentence'
try.
 t=. 'My+New+Plot'
 d=.".s
catch.
 t=. 'Sentence+Failed'
 d=. 0
end.
create s;jgcimg('reset xline &chtt=',t)jgc d
)

JS=: 0 : 0
function ev_body_load(){jbyid("sentence").focus();}
function ev_sentence_enter(){jsubmit();}
)
