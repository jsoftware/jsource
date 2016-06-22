coclass'jdemo10'
coinsert'jhs'

HBS=: 0 : 0
      jhh1'Ajax chunks'
'run' jhb'get ajax response in 4 chunks with 2 second delays'
jhtablea         
jhtr 'chunk:'  ; 'chunk'  jhtext '';20
jhtr 'chunks:' ; 'chunks' jhtext '';20
jhtablez
desc
jhdemo''
)

desc=: 0 : 0
<hr>
J can provide an ajax response in chunks as it comes available.<br><br>
Intermediate results can be provided in a long running request as soon
as they are ready and Javascript can do incremental and timely page updates.<br><br>
Typically the chunks for processeing need to be delimited as the data
received boundaries don't necessarily correspond with chunks.<br><br>
In this demo JASEP is used to delimit chunks.
)

create=: 3 : 0
'jdemo10'jhr''
)

NB. JASEP delimits chunks
ev_run_click=: 3 : 0
jhrajax_a 'a',JASEP     NB. first chunk
6!:3[2
jhrajax_b 'bb',JASEP    NB. annother chunk
6!:3[2
jhrajax_b 'ccc',JASEP   NB. another chunk
6!:3[2
jhrajax_z 'ddd',JASEP   NB. last chunk
i.0 0
)

jev_get=: create

CSS=: ''   NB. styles

JS=: 0 : 0 NB. javascript

function ev_run_click()
{
 jbyid("chunk").value=  "";
 jbyid("chunks").value= "";
 jdoajax([],"",jevsentence,true); // async
}

function ev_run_click_ajax(){;}

// process next chunk
function process(c)
{
 jbyid("chunk").value= c;
 jbyid("chunks").value+= c;
}

// framework calls with responseText so far
// next chunk is from rqoffset up to JASEP
function ev_run_click_ajax_chunk()
{
 while(1) // there could be 0, 1, or several chunks in the new data
 {
  var a= rq.responseText.substr(rqoffset); // not yet processed
  var i= a.indexOf(JASEP);
  if(i==-1) break;                         // do not have a chunk 
  rqchunk= a.substr(0,i);
  process(rqchunk);
  rqoffset= rqoffset+i+1;
 }
}


)