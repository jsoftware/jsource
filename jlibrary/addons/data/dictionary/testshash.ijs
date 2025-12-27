cocurrent 'base'

require 'format/printf'   NB. for debug only

PRBASIC =: 0   NB. 1 to display simple ops
PRBATCH =: 0   NB. 1 to display batch ops

NB. INITIALIZATION.
NB. Test name attribute.
NB. y is name of dictionary without locale (different locales are added in the test).
test_dict_name =: {{)m
op_names_boxed =. (y , '_del') ; (y , '_get') ; y , '_put'
op_names_string =. ' ' joinstring op_names_boxed
check_names_z_ =: [: (3 -: +/@,) op_names_boxed E."0 1 nl

params =. 'hash' ,&< 'name' ; y , '_abc_'
dict =. params conew 'jdictionary'
assert. check_names_abc_ ''
assert. destroy__dict ''
erase_abc_ op_names_string

params =. 'hash' ,&< 'name' ; y
dict =. params conew 'jdictionary'
assert. check_names_base_ ''
assert. destroy__dict ''
erase_base_ op_names_string

params =. 'hash' ,&< 'name' ; y , '__'
dict =. params conew 'jdictionary'
assert. check_names__ ''
assert. destroy__dict ''
erase__ op_names_string

EMPTY
}}

test_dict_name 'mydict'
test_dict_name 'my_d_ict'
test_dict_name 'd_i_c_t'

NB. BASIC GET, PUT.
cocurrent 'base'
]params =: 'hash' ,&< ('initsize' ; 4) , ('occupancy' ; 0.66) , ('keytype' ; 'integer') , ('valueshape' ; 3) , ('keyshape' ; 2) ,: ('valuetype' ; 4)
mydict =: params conew 'jdictionary'

{{
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
1 2 3 put__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 1 2 3 -: get__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
7 8 9 put__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 7 8 9 -: get__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
(1 2 3,:4 5 6) put__mydict 2 3,:2 3  NB. Double put existent
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 4 5 6 -: get__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
del__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
100 100 100 -: 100 100 100 get__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
(5 6 7,:1 2 3) put__mydict 2 3,:2 3  NB. Double put nonexistent
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 1 2 3 -: get__mydict 2 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
4 5 6 put__mydict 1 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 4 5 6 -: get__mydict 1 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
2 3 4 put__mydict 4 5
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
3 4 5 put__mydict 5 6
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. (_3 ]\ 1 2 3 2 3 4 3 4 5) -: get__mydict 2 3 , 4 5 ,: 5 6
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 4 5 6 -: get__mydict 1 3
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
(4 5 6,:5 6 7) put__mydict 6 7,:7 8
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 3 4 5 -: get__mydict 5 6
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 100 -: get__mydict :: (100"_) 7 7
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 90 91 92 -: 90 91 92 get__mydict 7 7
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 50 51 52 put__mydict 7 7  NB. Put to nonexistent
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 50 51 52 -: get__mydict 7 7
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 60 61 62 put__mydict 7 7  NB. Put to existent
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 60 61 62 -: get__mydict 7 7
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. (10 11 12,:20 21 22) put__mydict 8 8,:8 8  NB. Double put nonexistent
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 20 21 22 -: get__mydict 8 8
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. (30 31 32,40 41 42,:50 51 52) put__mydict 9 9,9 9,:9 9  NB. Triple put nonexistent - keeps last
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 50 51 52 -: get__mydict 9 9
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. (60 61 62,:70 71 72) put__mydict 8 8,:8 8  NB. Double put existent
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 70 71 72 -: get__mydict 8 8
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. (80 81 82,83 84 85,:90 91 92) put__mydict 9 9,9 9,:9 9  NB. Triple put existent - keeps last
qprintf^:PRBASIC'0(16!:_5)dict__mydict a.i.0{::dict__mydict }.dict__mydict '
assert. 90 91 92 -: get__mydict 9 9
EMPTY
}} ''

destroy__mydict ''

NB. Repeat for boxed key
]params =: 'hash' ,&< ('initsize' ; 10) , ('occupancy' ; 0.66) , ('keytype' ; 'boxed') , ('valueshape' ; 3) , ('keyshape' ; 2) ,: ('valuetype' ; 4)
mydict =: params conew 'jdictionary'

{{
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
1 2 3 put__mydict <"(0) 2 3
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 1 2 3 -: get__mydict <"(0) 2 3
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
4 5 6 put__mydict <"(0) 1 3
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 4 5 6 -: get__mydict <"(0) 1 3
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
2 3 4 put__mydict <"(0) 4 5
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
3 4 5 put__mydict <"(0) 5 6
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. (_3 ]\ 1 2 3 2 3 4 3 4 5) -: get__mydict <"(0) 2 3 , 4 5 ,: 5 6
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 4 5 6 -: get__mydict <"(0) 1 3
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
(4 5 6,:5 6 7) put__mydict <"(0) 6 7,:7 8
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 3 4 5 -: get__mydict <"(0) 5 6
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 100 -: get__mydict :: (100"_) <"(0) 7 7
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 90 91 92 -: 90 91 92 get__mydict <"(0) 7 7
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
50 51 52 put__mydict <"(0) 7 7  NB. Put to nonexistent
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 50 51 52 -: get__mydict <"(0) 7 7
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
60 61 62 put__mydict <"(0) 7 7  NB. Put to existent
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 60 61 62 -: get__mydict <"(0) 7 7
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
(10 11 12,:20 21 22) put__mydict <"(0) 8 8,:8 8  NB. Double put nonexistent
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 20 21 22 -: get__mydict <"(0) 8 8
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
(30 31 32,40 41 42,:50 51 52) put__mydict <"(0) 9 9,9 9,:9 9  NB. Triple put nonexistent - keeps last
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 50 51 52 -: get__mydict <"(0) 9 9
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
(60 61 62,:70 71 72) put__mydict <"(0) 8 8,:8 8  NB. Double put existent
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 70 71 72 -: get__mydict <"(0) 8 8
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
(80 81 82,83 84 85,:90 91 92) put__mydict <"(0) 9 9,9 9,:9 9  NB. Triple put existent - keeps last
qprintf^:PRBASIC'(<@<@<@<@(0&(16!:_5)){&.>2&{.@}.)dict__mydict '
assert. 90 91 92 -: get__mydict <"(0) 9 9
EMPTY
}} ''

destroy__mydict ''

erase 'mydict params'

NB. INCORRECT KEYS/VALUES.

get_error =: {{)a
u :: (<:@(13!:11)@'' >@{ 9!:8@'')
}}

{{
params =. 'hash' ,&< 'valueshape' ; 5
jdict =. params conew 'jdictionary'
assert. 'domain error' -: (i. 5) put__jdict get_error 2.3 3.4
assert. 'domain error' -: (2 5 $ 'abcde') put__jdict get_error 0 1
assert. 'domain error' -: (3 5 $ 3.14) put__jdict get_error 2 3 4
assert. 'rank error' -: _1 get__jdict get_error 100
assert. 'domain error' -: del__jdict get_error 'abc'
destroy__jdict ''
EMPTY
}} ''

NB. TYPES AND SHAPES.

coclass 'naivedictionary'

create =: {{
keys =: 0 $ a:
vals =: 0 $ a:
}}

destroy =: codestroy

has =: {{ keys e.~ < y }}

put =: {{
del y
keys =: keys , < y
vals =: vals , < x
EMPTY
}}

get =: {{ > vals {~ keys i. < y }}

del =: {{
idx =. keys i. < y
if. idx < # keys do.
  keys =: keys -. < y
  vals =: (idx {. vals) , (>: idx) }. vals
end.
assert. (# keys) -: # vals
EMPTY
}}

cocurrent 'base'

rand_integer =: {{ ? <. 3 }}
rand_floating =: {{ 8 c. -: rand_integer '' }}
rand_boolean =: {{ ? 2 }}
rand_byte =: {{ ({~ ?@#) 'abc' }}
rand_complex =: {{ (rand_floating '') j. rand_floating '' }}
rand_extended =: {{ x: rand_integer '' }}
rand_rational =: {{ x: rand_floating '' }}
rand_boxed =: {{ < (2 3 $ rand_complex '') ; 1 2 $ rand_integer '' }}

NB. x is jdict
NB. y is gerund generating atom of keytype  ;
NB.      gerund generating atom of valuetype ;
NB.      shape of key ;
NB.      shape of value ;
NB.      shape of batch ;
NB.      number of iterations
test_type =: {{)d
'genkey genval keyshape valshape batchshape n_iter' =. y
naivedict =. '' conew 'naivedictionary'
keyrank =. # keyshape
valrank =. # valshape
for. i. n_iter do.
  NB. Put.
  keys =. (batchshape , keyshape) genkey"0@$ 0
  vals =. (batchshape , valshape) genval"0@$ 0
  vals put__naivedict"(valrank , keyrank) keys
  vals put__x keys
  NB. Get.
  keys =. (batchshape , keyshape) genkey"0@$ 0
  vals =. (batchshape , valshape) genval"0@$ 0
  naivemask =. has__naivedict"keyrank keys
  jgetans =. (valshape $ _1) get__x keys NB. Obviously (valshape $ _1) does not work for boolean, literal and boxed values.
  jmask =. +./@,"valrank ] _1 ~: jgetans
  assert. jmask -: naivemask
  naivegetans =. get__naivedict"keyrank (, naivemask) # (_ , keyshape) ($ ,) keys
  if. 1 -: +./ , naivemask do.
    assert. naivegetans -: (, jmask) # (_ , valshape) ($ ,) jgetans
  end.
  NB. Delete.
  keys =. ((<. -: batchshape) , keyshape) genkey"0@$ 0
  del__naivedict"keyrank keys
  del__x keys
end.
destroy__naivedict ''
destroy__x ''
EMPTY
}}

keyshape =: 3 2
valueshape =: 5 8
params =. 'hash' ,&< ('keytype' ; 'boolean') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_boolean`'' ; rand_integer`'' ; keyshape ; valueshape ; 5 25 ; 100

keyshape =: 2 3
valueshape =: 4 5 4
params =. 'hash' ,&< ('valuetype' ; 'floating') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_integer`'' ; rand_floating`'' ; keyshape ; valueshape ; 100 ; 100

keyshape =: 7 1
valueshape =: 1 7
params =. 'hash' ,&< ('keytype' ; 'floating') , ('valuetype' ; 'complex') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_floating`'' ; rand_complex`'' ; keyshape ; valueshape ; 10 10 ; 100

keyshape =: 3 3
valueshape =: 2 2 2
params =. 'hash' ,&< ('keytype' ; 'complex') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_complex`'' ; rand_integer`'' ; keyshape ; valueshape ; 2 3 4 5 ; 100

keyshape =: 10
valueshape =: 10
params =. 'hash' ,&< ('keytype' ; 'extended') , ('valuetype' ; 'rational') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_extended`'' ; rand_rational`'' ; keyshape ; valueshape ; 7 7 ; 100

keyshape =: 10
valueshape =: 5 5
params =. 'hash' ,&< ('keytype' ; 'literal') , ('valuetype' ; 'extended') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_byte`'' ; rand_extended`'' ; keyshape ; valueshape ; 100 ; 100

keyshape =: 2 2
valueshape =: 10 10
params =. 'hash' ,&< ('keytype' ; 'boxed') , ('valuetype' ; 'floating') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
jdict =: params conew 'jdictionary'
jdict test_type rand_boxed`'' ; rand_floating`'' ; keyshape ; valueshape ; 25 2 ; 100

erase 'jdict keyshape valueshape rand_boolean rand_boxed rand_byte rand_complex rand_integer rand_floating rand_extended rand_rational'

NB. GET, PUT, DEL IN BATCHES.

NB. Keys are boxed strings, values are integers.

NB. https://code.jsoftware.com/wiki/Essays/DataStructures
coclass 'refdictionary'
create =: 0:
destroy =: codestroy
okchar =: ~. (,toupper) '0123456789abcdefghijklmnopqrstuz'
ok =: ] [ [: assert [: *./ e.&okchar
intern =: [: ('z' , ok)&.> boxxopen
has =: _1 < nc@intern
set =: {{ empty (intern x) =: y }}
get =: ".@>@intern
del =: erase@intern

cocurrent 'base'
9!:39 ] 3 13 NB. It makes refdictionary faster.

NB. x is initsize ; occupancy.
NB. y is number of batches (iterations) ; size of batch ; max element.
test_batches =: {{)d
'initsz occupancy' =. x
'n_iter sz mx' =. y
refdict =. conew 'refdictionary'
params =. 'hash' ,&< ('initsize' ; initsz) , ('occupancy' ; occupancy) ,: ('keytype' ; 'boxed')
jdict =. params conew 'jdictionary'
for. i. n_iter do.
  keys =. <@":"0 vals =. sz ?@$ mx NB. Keys, vals for put.
  keys (>@[ set__refdict ])"0 vals
qprintf^:PRBATCH'$keys keys $vals vals '
  vals put__jdict keys
  keys =. <@":"0 vals =. sz ?@$ mx NB. New keys, vals for queries.
qprintf^:PRBATCH'$keys keys $vals vals '
  refmask =. has__refdict@> keys
  jgetans =. _1 get__jdict keys
  jmask =. 0 <: jgetans
qprintf^:PRBATCH'jgetans refmask jmask '
  assert. jmask -: refmask
  if. 1 -: +./ refmask do. NB. Reference dictionary fails for get__refdict@> 0 $ 0
    assert. (refmask # vals) -: get__refdict@> refmask # keys
  end.
  assert. (jmask # vals) -: jmask # jgetans
  del__refdict keys =. (sz ?@$ 2) # keys NB. Delete some keys.
  del__jdict keys
end.
destroy__refdict ''
destroy__jdict ''
EMPTY
}}

NB. ADVANCED.

NB. Create/remove threads so that there are exactly y worker threads.
NB. y is number of worker threads.
set_threads =: {{)m
{{55 T. 0}}^:] 0 >. y -~ 1 T. ''
{{0 T. 0}}^:] 0 >. y - 1 T. ''
assert. y -: 1 T. ''
}}

NB. GET, PUT IN MULTIPLE THREADS.
NB. Keys and values are integers (permutations).

NB. y is number of iterations , number of worker threads , number of keys and values
test_multithreading1 =: {{)m
'n_iter n_threads sz' =. y
jdict =. ('hash' ; '') conew 'jdictionary'
'keys vals' =. ?~ ,~ sz
NB. smoutput'put init',' ',":{.3 T.''
vals put__jdict keys
NB. x is jdict
NB. y is number of iterations ; keys
prog =: {{)d
  'n_iter keys' =. y
  sz =. # keys
  for_el. i. n_iter do.
    vals =. get__x keys
NB. smoutput'put ',":el,{.3 T.''
    vals put__x keys NB. Put exactly the same keys and values.
NB. smoutput'fin put ',":el,{.3 T.''
    keys =. vals
  end.
  }}
NB. Run prog in main thread.
res_mt =. jdict prog n_iter ; keys
NB. Copy input for each thread and run prog in each thread.
set_threads n_threads
res_wts =. jdict prog t.''"1 (n_threads , 2) $ n_iter ; keys
NB. Check if result from each thread is the same as the result from main thread.
mask =. (res_mt -: >)"0 res_wts
erase 'prog'
destroy__jdict ''
assert. mask -: n_threads $ 1
EMPTY
}}

NB. Given matrix represented by dyadic verb magic which returns value of matrix at index (x, y).
NB. Each thread (task per column) will add values from one column to dictionary which stores sums of rows.
NB. Dictionary stores pairs of atomic values (currect_row_sum and counter_of_additions).
NB. When a row is handled by all threads, the last thread (according to counter for the row) removes it from dictionary and stores value in global array.
NB. y is number of rows , number of columns , number of threads
test_multithreading2 =: {{)m
'rows cols n_threads' =. y
params =. 'hash' ,&< ('valueshape' ; 2) ,: 'valuetype' ; 'boxed'
jdict =. params conew 'jdictionary'
(16&T."0 (rows , 2) $ 0) put__jdict i. rows
row_sums =: rows $ 0
magic =: {{ (2 * x + 3) + 3 * y + 7 }}
NB. x is jdict
NB. y is number of rows , number of columns, index of column
prog =: {{)d
  'rows cols col' =. y
  for_row. i. rows do.
    res =. row magic col
    val =. get__x row
    17 T. ({. val) , < res NB. Update sum of row.
    if. cols -: >: 17 T. ({: val) , < 1 do. NB. Update and check counter - is it the last thread?
      row_sums =: (17 T. ({. val) , < 0) row} row_sums NB. Store sum of row in global array row_sums.
      del__x row
    end.
  end.
  EMPTY
  }}
set_threads n_threads
> pyxes =. jdict prog t.''"1 ] (rows , cols)&,"0 i. cols
correct_row_sums =. +/"1 (i. rows) magic"0/ i. cols
assert. row_sums -: correct_row_sums
erase 'magic'
erase 'prog'
erase 'row_sums'
destroy__jdict ''
EMPTY
}}

NB. RUN BATCHES & ADVANCED.

(21 ; 0.9) test_batches 10000 ; 10 ; 20
(3 ; 0.1) test_batches 10000 ; 10 ; 20
(142 ; 0.6) test_batches 1000 ; 100 ; 200
(7 ; 0.6) test_batches 100 ; 1000 ; 2000
(200001 ; 0.8) test_batches 5 ; 100000 ; 200000

test_multithreading1 10000 5 200

test_multithreading2 1 10000 3
{{ for. i. 30 do. test_multithreading2 100 200 5 end. }} ''

NB. BENCHMARKS.

NB. y is number of threads ;
NB.      number of keys and values ;
NB.      shape of key and value ;
NB.      number of batches ;
NB.      size of batch
benchmark_multithreading =: {{)m
'n_threads n_kvs kv_shape n_b b_sz' =. y
ks =. (n_kvs , kv_shape) ?@$ 1e9 NB. Generate random keys.
vs =. (?~ n_kvs) { ks NB. Values are shuffled keys.
NB. Generate batches.
bs_ks =. ((n_b , >. 0.9 * b_sz) ?@$ >. 0.1 * # vs) { vs NB. 90% of batch chosen from 10% of keys.
bs_ks =. bs_ks ,"_1 ((n_b , >. 0.1 * b_sz) ?@$ # vs) { vs NB. 10% of batch chosen from all keys.
NB. Initialize dictionary.
params =. 'hash' ,&< ('valueshape' ; kv_shape) ,: 'keyshape' ; kv_shape
jdict =. params conew 'jdictionary'
vs put__jdict ks
NB. Define verbs for thread.
NB. x is jdict.
NB. y is list of batches.
prog_get =: {{)d
  y =. (?~ # y) { y NB. Shuffle batches.
  f =. {{)d
    for_b. y do.
      get__x^:100 b
    end.
    EMPTY
    }}
  timex 'x f y' NB. Return time of execution.
  }}
prog_get_put =: {{)d
  f =. {{)d
    for_b. y do.
      ks =. get__x^:100 b
      vs =. (? # ks) |. ks NB. Shuffle by cyclic-shift by random number.
      vs put__x ks
    end.
    EMPTY
    }}
  timex 'x f y' NB. Return time of execution.
  }}
set_threads n_threads
inputs =. n_threads $ ,: bs_ks
times_get =. > jdict prog_get t.''"_1 inputs
times_get_put =. > jdict prog_get_put t.''"_1 inputs
erase 'prog_get prog_get_put'
destroy__jdict ''
(n_threads * n_b * 100 * b_sz) % >./ times_get ,. times_get_put
}}

benchmark_multithreading 3 ; 1e6 ; 4 5 ; 100 ; 100
