prolog './g128x14.ijs'

CacheLine=: 9!:56'cachelinesize'

3 : 0''
if. IFWIN do.
 sleep=: usleep@>.@(1e6&*)
else.
 sleep=: 6!:3
end.
1
)

NB. wait until there are y waiting threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

N=: <: 1 { 8 T. ''  NB. max # worker threads
NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0

NB.  **** 128!:14

NB. 'col' means pivot column, which is horizontal in Qkt.  'row' is pivot row in Qk, which comes from a col of Qkt.
NB. Call is destname;rowmasks;rowvalues;colmasks;colvalues<threshold
NB. destname is 'Qkt'.  Qkt must be assigned, aligned to cacheline boundary, unaliased.  It is modified in place
NB. rowmasks is list of boxes, one per pivot, with a 1 in the position of each column of Qk (=row of Qkt) to be modified by the pivot.
NB.     This specifies the non0 values in the column of Qkt after discarding rows that have been partial-pivoted (i. e. the cols of QK that contribute to pivot cols in the batch)
NB. rowvalues is, for each pivot,  #&(pivot column of Qkt)&.> rowmask as above
NB. colmasks is list of boxes, one per pivot, with a 1 in the position of each column of Qkt (=row of Qk) to be modified by the pivot.
NB.     This specifies the non0 values in the pivot column after discarding the pivot rows of the batch
NB. colvalues is, for each pivot,  #&(pivot column)&.> colmask as above
NB. threshold is near0 threshold for results
NB. Result is time for the operation, with the name updated
batchop =: {{
name =. 0 {:: y
NB. Because of the poor design of 128!:22, we have to know the name exactly
iqkt =. 15!:18 memu name~  NB. make a copy for our use
threshold =. 5 {:: y
rcmv =. 3 4 1 2 { y  NB. mask/values for rows/columns, transposing for 128!:22 which was written for Qk
assert. (= {.) $&.> rcmv  NB. must have same shape
assert. (<,1) ~: #@$&.> rcmv  NB. all lists

t =. 0  NB. init total time
for_p. 2 2 $"1 |: > rcmv do.
  arg =. -&.>&.(3&{) ((I.&.> {."1 p),{:"1 p),<0.0  NB. no threshold for intermediate results, and negate the op, to match 128!:14.
  t =. t + 6!:2 'iqkt =. arg 128!:22 iqkt'
end.
(name) =: 15!:18 iqkt * (name~ ~:!.0 iqkt) *: (threshold >!.0 | 8 c. iqkt)  NB. set changed values to 0 if they went below threshold 
t
}}

NB. 'col' means pivot column, which is horizontal in Qkt.  'row' is pivot row in Qk, which comes from a col of Qkt.
NB. destname is 'Qkt'.  Qkt must be assigned, aligned to cacheline boundary, unaliased.  It is modified in place
NB. rowmasks is list of boxes, one per pivot, with a 1 in the position of each column of Qk (=row of Qkt) to be modified by the pivot.
NB.     This specifies the non0 values in the column of Qkt after discarding rows that have been partial-pivoted (i. e. the cols of QK that contribute to pivot cols in the batch)
NB. rowvalues is, for each pivot,  #&(pivot column of Qkt)&.> rowmask as above
NB. colmasks is list of boxes, one per pivot, with a 1 in the position of each column of Qkt (=row of Qk) to be modified by the pivot.
NB.     This specifies the non0 values in the pivot column after discarding the pivot rows of the batch
NB. colvalues is, for each pivot,  #&(pivot column)&.> colmask as above
NB. threshold is the near0 threshold for results
NB. Result is elapsed time.  name is modified
batchopndx =: {{
0 batchopndx y
:
debugopts =. x
'name rowmasks rowvalues colmasks colvalues threshold' =. y
NB. Find the vertical stripes. 

minmaxct =. (((,. a. i. -.&({.a.))~    0 4&+"0@(4&*)) I.@(({.a.)&~:)) (#: i. 16) (i. { a. {~ +/"1@[) _ 4 $!.0!.>. +./ > rowmasks  NB. bucket by 4; get ct (as char); create mmc from position & ct

NB. Adverb.  m is max gap,max spread,max ct.  Resulting verb replaces each mmc with a new mmc: the last one stays as is, and each earlier
NB. one either coalesces to the tail, if all conditions are are right, giving an mmc with the same start as the tail; or stays unchanged to start
NB. a new tail.  After that, we keep the first mmc at each ending position
makecoal =. {{ (#~ ~:@:(1&{"1)) @: ((00&{@],01&{@[,+/&{:)^:([: *./ m >: (1 _1 1 * 1 0 2&{)@] + _1 1 1&*@[)~/\.) }}  NB. old start-new end=gap, old end-new start=spread, sum of ct=new ct

maxstripewidth =. 32 <.^:(1=#rowmasks) <. (+/ {:"1 minmaxct) % +:^:(>&1) >: 1 T. ''  NB. Limit width of stripe to leave at least 2 stripes per thread (unless single-threaded), but max 32 if only one OP to prevent result code from lagging
for_gsc. maxstripewidth ([ (<_1 _1)} (01 >. (I.~ {:"1)) {. ]) _3 ]\ 4 16 8  16 64 32  64 256 128 do.  NB. Apply coarsening sieves to the stripes, honoring the max width
  minmaxct =. (gsc makecoal) minmaxct
end.
assert. ((<_1 1) { minmaxct) <: {: $ name~

stripes =. 0 1{"1 minmaxct  NB. (start,end+1) for each stripe 
comploads =. +/ (+/@> colmasks) * (,.@(-~/\)"1 stripes)&(+/;.0)@> rowmasks    NB. table of (start,len) of each stripe; for each column, count 1s in each stripe; weight by #rows in each op; sum by stripes
NB. obsolete qprintf 'Qkt '
NB. obsolete qprintf'stripes comploads '
6!:2 'debugopts 128!:14 y,stripes;(\:comploads)'  NB. Run the pivots.  Result is time; name is modified
}}

compvers =: ".@'Qkt' -: ".@'qktcopy'  NB. compare the temp vars we use for the two version

Qkt =: (15!:18) 11 c. (38, (>.&.(%&(CacheLine%16)) 39)) $ 0.   NB. 16 is the byte size of quadprec
rm =: ,< 36 {. (16$0) , 0 1 1 0 1 1 1 1 0 1 0 1 1 0 1 1
rv =: (11 c. I.)&.> rm
cm =: ,< 37 {. 0 1 0 1 1
cv =: (11 c. 1000 + I.)&.> cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt
NB. 13!:8 ] 4

NB. 1 stripe
Qkt =: (15!:18) 11 c. (19, (>.&.(%&(CacheLine%16)) 20)) $ 0.   NB. 16 is the byte size of quadprec
rm =: ,< 18 {. 1 0 1 1 0 1 1 1 0 1
rv =: (11 c. I.)&.> rm
cm =: ,< 19 {. 0 1 0 1 1
cv =: (11 c. 1000 + I.)&.> cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt


NB. 2 stripes
Qkt =: (15!:18) 11 c. (19, (>.&.(%&(CacheLine%16)) 20)) $ 0.
rm =: ,< 18 {. 1 0 1 1 0 0 0 0 0 0 0 0 0 1 1 1 0 1
rv =: (11 c. 1 + I.)&.> rm
cm =: ,< 19 {. 0 1 0 1 1
cv =: (11 c. 1000 + I.)&.> cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt

NB. 2 OPs
Qkt =: (15!:18) 11 c. (19, (>.&.(%&(CacheLine%16)) 20)) $ 0.
rm =: <"1 (18) {."1 ]  1 0 1 1 0 0 0 0 0 1 1 1 0 1 ,: 0 1 1 1 0 0 0 0 0 0 0 1 0 1 
rv =: 0 100 (11 c. (+ I.))&.> rm
cm =: <"1 (19) {."1 ]  0 1 0 1 1 ,: 1 1 0 1 0 0 1
cv =: 0 1000 (11 c. (+ I.))&.> cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt

NB. 1 tall thin OP
Qkt =: (15!:18) 11 c. (1003, (>.&.(%&(CacheLine%16)) 20)) $ 0.
rm =: , <"1 (18) {."1 ]  0 1 
rv =: , 100 ((* i.@#) (11 c. (+ I.))&.> ]) rm
cm =: , <"1 (1001) {."1 ]  0 , 1000$1
cv =: , 10000 ((* i.@#) (11 c. (+ I.))&.> ]) cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt

NB. 1 wide short OP
Qkt =: (15!:18) 11 c. (20, (>.&.(%&(CacheLine%16)) 1004)) $ 0.
rm =: , <"1 ] 0 , 1000$1 
rv =: , 100 ((* i.@#) (11 c. (+ I.))&.> ]) rm
cm =: , <"1 ] ,1
cv =: , 10000 ((* i.@#) (11 c. (+ I.))&.> ]) cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt

NB. 2 overlapping OPs that will overrun a single thread
Qkt =: (15!:18) 11 c. (1004, (>.&.(%&(CacheLine%16)) 132)) $ 0.
rm =: , <"1 ] 2 128$1 
rv =: , 100 ((* i.@#) (11 c. (+ I.))&.> ]) rm
cm =: , <"1 ] 2 1000$1
cv =: , 10000 ((* i.@#) (11 c. (+ I.))&.> ]) cm
(batchopndx@('Qkt'&;) compvers batchop@('qktcopy'&;)) rm;rv;cm;cv;0.0 [ qktcopy =: memu Qkt

WORDER =: 0 2 4 6 8 10 12 16 13 17 14 18 15 19 (33 b.) 1  NB. P then E
setnworkers =: {{ while. 1 T. '' do. 55 T. '' end. for_c. y{.}.WORDER do. 0 T. 0;<'coremask';c end. 1 }}   NB. ensure there are exactly y worker threads

NB. y is density of non0 (col,row).  Result is rcmv for batchndx (single-boxed).  Qkt exists, and we use its shape
createop =: {{
rc =. y (> ?@$&0)&.>&|. (2 ?@$ 4) -~ $Qkt  NB. masks for r and c
, (,. (11 c. 0 ?@$~ +/)&.>) rc  NB. create a value for each mask bit, order as rm,rv,cm,cv
}}"1

NB. x is #worker threads to use.  y is rcmvt of batch of pivots.  Qkt is set.  result is (time old,time new), but aborting on mismatch of result
mtpivot =: {{
setnworkers x   NB. Use specified # workers
qktcopy =: memu Qkt
ot =. batchop 'qktcopy';y
nt =. batchopndx 'Qkt';y
assert. 1e_20 > <./ , | Qkt -&(8&c.) qktcopy  NB. results should be close
ot,nt  NB. return times
}}"0 1

NB. x is (max#worker threads to use).  y is (shape of Qkt);(table of densities);threshold.  Result is (table of old times (#threads,#pivots)),:(table of new times), but aborting on mismatch of result
mtpivottbl =: {{
'qkts dens thresh' =. y  NB. extract parms
qkts =. >.&.(%&(CacheLine%16))&.(1&{) qkts  NB. round row-len up to cacheline
Qkt =: (15!:18) 11 c. qkts $ 0   NB. Allocate Qkt
ops =. createop dens   NB. create all the ops, as a table of rm,rv,cm,cv
|: ((i. >: x) mtpivot (thresh (,<)~ <"1@:|:)@])\ ops  NB. all pivot-batches, producing (#pivots)x(#threads)x(old,new)
}}


NB. y is #threads to use, result is time for r 40-pivot batches with 100% fill, 5000x5000
NB. x is r,(0 for batched, 1 for nonbatched)
NB. 0 0 timepiv 0
timepiv =: {{
'r unbat' =. x
nthr =. y
setnworkers nthr
22 T. 0;{.WORDER
qkts =. >.&.(%&(CacheLine%16))&.(1&{) 5000 5000  NB. round row-len up to cacheline
Qkt =: (15!:18) 11 c. qkts $ 0   NB. Allocate Qkt
qktcopy =: memu Qkt
ops =. <"1@:|: createop 40$,:1.0,1.0   NB. create all the ops, as rm,rv,cm,cv  each a boxed list
tt =. 0
for. r#0 do. if. unbat do. tt =. tt , batchop 'qktcopy';ops,<0.0 else. tt =. tt , batchopndx 'Qkt';ops,<0.0 end. end.
(+/ % #) tt
}}

res =: 0 mtpivottbl (20, (>.&.(%&(CacheLine%16)) 20));(,:0.9 0.9);0.0

delth''

epilog''

