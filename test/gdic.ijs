prolog './gdic.ijs'
NB. addon/dictionary

require 'data/dictionary'

cocurrent 'base'

INDEX_TYPES_CONCURRENT =: 'hash concurrent' ; 'tree concurrent'
INDEX_TYPES =: INDEX_TYPES_CONCURRENT , 'hash' ; 'tree'
KEYHASHES =: 16!:0`'' , 7"1`'' , {{ 16!:0 y }}"1`''
KEYCOMPARES =: 16!:0`'' , -@:(16!:0)`'' , {{ x 16!:0 y }}`''
COMBINATIONS =: INDEX_TYPES ,"1"0 _ KEYHASHES ,."0 _ KEYCOMPARES

NB. INITIALIZATION.
NB. Test name attribute.
NB. x is boxed name of index type.
NB. y is name of dictionary without locale (different locales are added in the test).
test_dict_name =: {{)d
op_names_boxed =. (y , '_del') ; (y , '_get') ; (y , '_put') ; (y , '_has') ; (y , '_count')
op_names_string =. ' ' joinstring op_names_boxed
check_names_z_ =: [: (5 -: +/@,) op_names_boxed E."0 1 nl

params =. x , < 'name' ; y , '_abc_'
dict =. params conew 'jdictionary'
assert. check_names_abc_ ''
assert. destroy__dict ''
erase_abc_ op_names_string

params =. x , < 'name' ; y
dict =. params conew 'jdictionary'
assert. check_names_base_ ''
assert. destroy__dict ''
erase_base_ op_names_string

params =. x , < 'name' ; y , '__'
dict =. params conew 'jdictionary'
assert. check_names__ ''
assert. destroy__dict ''
erase__ op_names_string

EMPTY
}}"0 _

INDEX_TYPES test_dict_name 'mydict'
INDEX_TYPES test_dict_name 'my_d_ict'
INDEX_TYPES test_dict_name 'd_i_c_t'

NB. INCORRECT KEYS/VALUES.

GetError =: {{)a
u :: (<:@(13!:11)@'' >@{ 9!:8@'')
}}

{{
assert. 'length error' -: 'jdictionary' conew~ GetError y , < 'keyshape' ; 2 0 2
assert. 'length error' -: 'jdictionary' conew~ GetError y , < 'keyshape' ; 0
assert. 'length error' -: 'jdictionary' conew~ GetError y , < 'valueshape' ; 7 0
assert. 'length error' -: 'jdictionary' conew~ GetError y , < 'valueshape' ; 0 0
coreset ''
jdict =. 'jdictionary' conew~ 'hash' ,&< 'keyhash' ,&< _1"0`''
'domain error' -: put__jdict GetError~ 7
destroy__jdict ''
params =. y , < 'valueshape' ; 5
jdict =. params conew 'jdictionary'
assert. 'domain error' -: (i. 5) put__jdict GetError 2.3 3.4
assert. 'domain error' -: (2 5 $ 'abcde') put__jdict GetError 0 1
assert. 'domain error' -: (3 5 $ 3.14) put__jdict GetError 2 3 4
assert. 'rank error' -: _1 get__jdict GetError 100
assert. 'domain error' -: get__jdict GetError 3.14
assert. 'domain error' -: has__jdict GetError 2r3 3r4
assert. 'domain error' -: del__jdict GetError 'abc'
destroy__jdict ''
EMPTY
}}"0 INDEX_TYPES

NB. BASIC.

test_basic1 =: {{
'index_type keyhash keycompare' =. <"0 y
params =. index_type , < ('keyhash' ,&< keyhash) , ('keycompare' ,&< keycompare) , ('initsize' ; 4) , ('keytype' ; 'integer') , ('valueshape' ; 3) , ('keyshape' ; 2) ,: ('valuetype' ; 4)
mydict =. params conew 'jdictionary'
assert. 0 -: count__mydict ''
1 2 3 put__mydict 2 3
assert. 1 2 3 -: get__mydict 2 3
7 8 9 put__mydict 2 3
assert. 7 8 9 -: get__mydict 2 3
(1 2 3,:4 5 6) put__mydict 2 3,:2 3  NB. Double put existent
assert. 4 5 6 -: get__mydict 2 3
assert. 1 -: count__mydict ''
assert. 1 -: del__mydict 2 3
assert. 0 -: count__mydict ''
assert. 100 100 100 -: 100 100 100 get__mydict 2 3
(5 6 7,:1 2 3) put__mydict 2 3,:2 3  NB. Double put nonexistent
assert. 1 -: count__mydict ''
assert. 1 2 3 -: get__mydict 2 3
4 5 6 put__mydict 1 3
assert. 2 -: count__mydict ''
assert. 4 5 6 -: get__mydict 1 3
2 3 4 put__mydict 4 5
assert. 3 -: count__mydict ''
3 4 5 put__mydict 5 6
assert. 4 -: count__mydict ''
assert. (_3 ]\ 1 2 3 2 3 4 3 4 5) -: get__mydict 2 3 , 4 5 ,: 5 6
assert. 4 5 6 -: get__mydict 1 3
(4 5 6,:5 6 7) put__mydict 6 7,:7 8
assert. 3 4 5 -: get__mydict 5 6
assert. 100 -: get__mydict :: (100"_) 7 7
assert. 90 91 92 -: 90 91 92 get__mydict 7 7
50 51 52 put__mydict 7 7  NB. Put to nonexistent
assert. 50 51 52 -: get__mydict 7 7
60 61 62 put__mydict 7 7  NB. Put to existent
assert. 60 61 62 -: get__mydict 7 7
(10 11 12,:20 21 22) put__mydict 8 8,:8 8  NB. Double put nonexistent
assert. 20 21 22 -: get__mydict 8 8
(30 31 32,40 41 42,:50 51 52) put__mydict 9 9,9 9,:9 9  NB. Triple put nonexistent - keeps last
assert. 50 51 52 -: get__mydict 9 9
(60 61 62,:70 71 72) put__mydict 8 8,:8 8  NB. Double put existent
assert. 70 71 72 -: get__mydict 8 8
(80 81 82,83 84 85,:90 91 92) put__mydict 9 9,9 9,:9 9  NB. Triple put existent - keeps last
assert. 90 91 92 -: get__mydict 9 9
assert. 1 1 -: has__mydict 8 8,:9 9
assert. 9 -: count__mydict ''
assert. 1 -: del__mydict 9 9
assert. 1 0 -: has__mydict 8 8,:9 9
assert. 1 -: del__mydict 8 8
assert. 0 -: has__mydict 8 8
assert. 0 0 -: del__mydict 8 8,:9 9
destroy__mydict ''
EMPTY
}}"1

test_basic2 =: {{
'index_type keyhash keycompare' =. <"0 y
params =. index_type , < ('keyhash' ,&< keyhash) , ('keycompare' ,&< keycompare) , ('initsize' ; 4) , ('keytype' ; 'boxed') , ('valueshape' ; 3) , ('keyshape' ; 2) ,: ('valuetype' ; 4)
mydict =. params conew 'jdictionary'
1 2 3 put__mydict <"(0) 2 3
assert. 1 2 3 -: get__mydict <"(0) 2 3
4 5 6 put__mydict <"(0) 1 3
assert. 4 5 6 -: get__mydict <"(0) 1 3
2 3 4 put__mydict <"(0) 4 5
3 4 5 put__mydict <"(0) 5 6
assert. (_3 ]\ 1 2 3 2 3 4 3 4 5) -: get__mydict <"(0) 2 3 , 4 5 ,: 5 6
assert. 4 5 6 -: get__mydict <"(0) 1 3
(4 5 6,:5 6 7) put__mydict <"(0) 6 7,:7 8
assert. 3 4 5 -: get__mydict <"(0) 5 6
assert. 100 -: get__mydict :: (100"_) <"(0) 7 7
assert. 90 91 92 -: 90 91 92 get__mydict <"(0) 7 7
50 51 52 put__mydict <"(0) 7 7  NB. Put to nonexistent
assert. 50 51 52 -: get__mydict <"(0) 7 7
60 61 62 put__mydict <"(0) 7 7  NB. Put to existent
assert. 60 61 62 -: get__mydict <"(0) 7 7
(10 11 12,:20 21 22) put__mydict <"(0) 8 8,:8 8  NB. Double put nonexistent
assert. 20 21 22 -: get__mydict <"(0) 8 8
(30 31 32,40 41 42,:50 51 52) put__mydict <"(0) 9 9,9 9,:9 9  NB. Triple put nonexistent - keeps last
assert. 50 51 52 -: get__mydict <"(0) 9 9
(60 61 62,:70 71 72) put__mydict <"(0) 8 8,:8 8  NB. Double put existent
assert. 70 71 72 -: get__mydict <"(0) 8 8
(80 81 82,83 84 85,:90 91 92) put__mydict <"(0) 9 9,9 9,:9 9  NB. Triple put existent - keeps last
assert. 90 91 92 -: get__mydict <"(0) 9 9
assert. 1 -: del__mydict <"(0) 8 8
assert. 1 -: del__mydict <"(0) 9 9
destroy__mydict ''
EMPTY
}}"1

test_basic1 COMBINATIONS
test_basic2 COMBINATIONS

NB. DESTROY.

{{
jdict =. 'jdictionary' conew~ > y
put__jdict~ i. 7
assert. 'rank error' -: destroy__jdict GetError 0
assert. 'untimely request' -: get__jdict GetError 0
assert. 'untimely request' -: put__jdict GetError~ i. 10
assert. 'untimely request' -: has__jdict GetError i. 10
assert. 'untimely request' -: del__jdict GetError 0
assert. 'untimely request' -: count__jdict GetError ''
assert. 1 -: destroy__jdict ''
EMPTY
}}"0 INDEX_TYPES

NB. CUSTOM KEYHASH & KEYCOMPARE

{{
params =. y , < ('keycompare' ,&< 13!:8@16`'') ,: ('keyhash' ,&< 13!:8@16`'') NB. Return spelling error.
jdict =. params conew 'jdictionary'
42 put__jdict GetError 7 NB. No comparison for empty tree dictionary.
assert. 'spelling error' -: 43 put__jdict GetError 8
assert. 'spelling error' -: get__jdict GetError 7
assert. 'spelling error' -: has__jdict GetError 7
assert. 'spelling error' -: del__jdict GetError 7
destroy__jdict ''
EMPTY
}}"0 INDEX_TYPES

{{
params =. y , < ('keycompare' ,&< 0:`'') ,: ('keyhash' ,&< 0"0`'')
mydict =. params conew 'jdictionary'
put__mydict~ i. 5 [ put__mydict~ 2 3 [ put__mydict~ 1
assert. 1 -: count__mydict ''
destroy__mydict ''
EMPTY
}}"0 INDEX_TYPES

NB. EMPTY.

{{
'index_type keyshape valshape' =. y
params =. index_type ,&< ('keyshape' ; keyshape) ,: ('valueshape' ; valshape)
mydict =. params conew 'jdictionary'
frames =. 0 0 ; 0 2 ; 2 0 3 4 ; , 0
'keys vals' =. <"1 frames (0 $~ ,)&.>"_ 0 keyshape ; valshape
assert. frames (-: $@:has__mydict)&> keys
if. valshape -: 0 do.
  assert. (4 # ,: 'domain error') -: get__mydict GetError@> keys
else.
  assert. vals (-: get__mydict)&> keys
end.
assert. frames (-: $@:del__mydict)&> keys
destroy__mydict ''
}}"1 INDEX_TYPES ,"1"0 _ (,."0 _ (0)&;) 7 ; 2 3 4 ; i. 0

NB. SPARSE ARRAYS.

{{
'index_type sparsetype' =. y
assert. 'domain error' -: 'jdictionary' conew~ GetError index_type ,&< 'keytype' ; sparsetype
assert. 'domain error' -: 'jdictionary' conew~ GetError index_type ,&< 'valuetype' ; sparsetype
EMPTY
}}"1 INDEX_TYPES (, <)"0"_ 0 ] 2 ^ 10 + i. 6

coreset ''

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

count =: {{ # keys }}

cocurrent 'base'

rand_integer =: {{ >: ? <. 3 }}
rand_floating =: {{ 8 c. -: rand_integer '' }}
rand_boolean =: {{ ? 2 }}
rand_byte =: {{ ({~ ?@#) 'abc' }}
rand_complex =: {{ (rand_floating '') j. rand_floating '' }}
rand_extended =: {{ x: rand_integer '' }}
rand_rational =: {{ x: rand_floating '' }}
rand_boxed =: {{ < (2 3 $ rand_complex '') ; 1 2 $ rand_integer '' }}
rand_sparse_floating =: {{ $. 1 2.5 999 (3 ? 14)} 14 $ 0 }}

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
  assert. (count__x '') -: count__naivedict ''
  NB. Put.
  keys =. (batchshape , keyshape) genkey"0@$ 0
  vals =. (batchshape , valshape) genval"0@$ 0
  vals put__naivedict"(valrank , keyrank) keys
  assert. EMPTY -: vals put__x keys
  assert. (count__x '') -: count__naivedict ''
  NB. Get.
  keys =. (batchshape , keyshape) genkey"0@$ 0
  vals =. (batchshape , valshape) genval"0@$ 0
  naivemask =. has__naivedict"keyrank keys
  jhasans =. has__x keys
  assert. jhasans -: naivemask
  if. -. valshape -: 0 do.
    fillatom =. {. valuetype__x c. ''
    jgetans =. (valshape $ fillatom) get__x keys
    jmask =. +./@,"valrank ] fillatom ~: jgetans
    assert. jmask -: naivemask
    naivegetans =. get__naivedict"keyrank (, naivemask) # (_ , keyshape) ($ ,) keys
    if. 1 -: +./ , naivemask do.
      assert. naivegetans -: (, jmask) # (_ , valshape) ($ ,) jgetans
    end.
  end.
  NB. Delete.
  batchshapefordel =. <. -: batchshape
  keys =. (batchshapefordel , keyshape) genkey"0@$ 0
  naivemask =. has__naivedict"keyrank keys
  del__naivedict"keyrank keys
  jdelans =. del__x keys
  assert. (count__x '') -: count__naivedict ''
  assert. jdelans -: naivemask *. batchshapefordel ($ ,) ~:&.|. (_ , keyshape) ($ ,) keys
end.
destroy__naivedict ''
destroy__x ''
EMPTY
}}

0 1 {{
n_iter =. QKTEST{10 1

keyshape =. 3 2
valueshape =. 0:^:x 5 8
params =. y , < ('keytype' ; 'boolean') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_boolean`'' ; rand_integer`'' ; keyshape ; valueshape ; 5 25 ; n_iter

keyshape =. 2 3
valueshape =. 0:^:x 4 5 4
params =. y , < ('valuetype' ; 'floating') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_integer`'' ; rand_floating`'' ; keyshape ; valueshape ; 100 ; n_iter

keyshape =. 7 1
valueshape =. 0:^:x 1 7
params =. y , < ('keytype' ; 'floating') , ('valuetype' ; 'complex') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_floating`'' ; rand_complex`'' ; keyshape ; valueshape ; 10 10 ; n_iter

keyshape =. 3 3
valueshape =. 0:^:x 2 2 2
params =. y , < ('keytype' ; 'complex') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_complex`'' ; rand_integer`'' ; keyshape ; valueshape ; 2 3 4 5 ; n_iter

keyshape =. 9
valueshape =. 0:^:x 10
params =. y , < ('keytype' ; 'extended') , ('valuetype' ; 'rational') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_extended`'' ; rand_rational`'' ; keyshape ; valueshape ; 7 7 ; n_iter

keyshape =. 5
valueshape =. 0:^:x 6
params =. y , < ('keytype' ; 'rational') , ('valuetype' ; 'literal') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_rational`'' ; rand_byte`'' ; keyshape ; valueshape ; 3 2 ; n_iter

keyshape =. 8
valueshape =. 0:^:x 5 5
params =. y , < ('keytype' ; 'literal') , ('valuetype' ; 'extended') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_byte`'' ; rand_extended`'' ; keyshape ; valueshape ; 100 ; n_iter

keyshape =. 2 2
valueshape =. 0:^:x 2 4
params =. y , < ('keytype' ; 'boxed') , ('valuetype' ; 'boxed') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_boxed`'' ; rand_boxed`'' ; keyshape ; valueshape ; 25 2 ; n_iter
}}"0"_ 0 INDEX_TYPES

0 1 {{
n_iter =. QKTEST{300 1

keyshape =. 3 2
valueshape =. 0:^:x 5 8
params =. y , < ('keytype' ; 'boolean') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_boolean`'' ; rand_integer`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 2 3
valueshape =. 0:^:x 4 5 4
params =. y , < ('valuetype' ; 'floating') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_integer`'' ; rand_floating`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 7 1
valueshape =. 0:^:x 1 7
params =. y , < ('keytype' ; 'floating') , ('valuetype' ; 'complex') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_floating`'' ; rand_complex`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 3 3
valueshape =. 0:^:x 2 2 2
params =. y , < ('keytype' ; 'complex') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_complex`'' ; rand_integer`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 9
valueshape =. 0:^:x 10
params =. y , < ('keytype' ; 'extended') , ('valuetype' ; 'rational') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_extended`'' ; rand_rational`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 5
valueshape =. 0:^:x 6
params =. y , < ('keytype' ; 'rational') , ('valuetype' ; 'literal') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_rational`'' ; rand_byte`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 8
valueshape =. 0:^:x 5 5
params =. y , < ('keytype' ; 'literal') , ('valuetype' ; 'extended') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_byte`'' ; rand_extended`'' ; keyshape ; valueshape ; (i. 0) ; n_iter

keyshape =. 2 2
valueshape =. 0:^:x 2 4
params =. y , < ('keytype' ; 'boxed') , ('valuetype' ; 'boxed') , ('keyshape' ; keyshape) ,: ('valueshape' ; valueshape)
echo params
jdict =. params conew 'jdictionary'
jdict test_type rand_boxed`'' ; rand_boxed`'' ; keyshape ; valueshape ; (i. 0) ; n_iter
}}"0"_ 0 INDEX_TYPES

NB. GET, PUT, DEL, HAS IN BATCHES.
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

NB. x is boxed name of index type.
NB. y is initial size ; number of batches (iterations) ; size of batch ; max element.
test_batches =: {{)d
'initsz n_iter sz mx' =. y
refdict =. conew 'refdictionary'
params =. x , < ('initsize' ; initsz) ,: ('keytype' ; 'boxed')
jdict =. params conew 'jdictionary'
for. i. n_iter do.
  keys =. <@":"0 vals =. sz ?@$ mx NB. Keys, vals for put.
  keys (>@[ set__refdict ])"0 vals
  assert. EMPTY -: vals put__jdict keys
  keys =. <@":"0 vals =. sz ?@$ mx NB. New keys, vals for queries.
  refmask =. has__refdict@> keys
  jgetans =. _1 get__jdict keys
  jhasans =. has__jdict keys
  jmask =. 0 <: jgetans
  assert. jmask -: jhasans
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
}}"0 _

NB. RUN BATCHES.

9!:39 ] 3 13 NB. It makes refdictionary faster.
INDEX_TYPES test_batches 3 ; 1000 ; 10 ; 20
INDEX_TYPES test_batches 142 ; 100 ; 100 ; 200
INDEX_TYPES test_batches 20001 ; 3 ; 10000 ; 20000
9!:39 ] 3 2

NB. MULTITHREADING.

NB. Create/remove threads so that there are exactly y worker threads.
NB. y is number of worker threads.
set_threads =: {{)m
{{55 T. 0}}^:] 0 >. y -~ 1 T. ''
{{0 T. 0}}^:] 0 >. y - 1 T. ''
assert. y -: 1 T. ''
EMPTY
}}

NB. BASIC FROM MUTIPLE THREADS.
NB. Each thread creates, uses and destroys its own dictionary.

NB. y is number of threads.
test_basic_multithreading =: {{
set_threads y
> pyxes =. test_basic1 t.''"1 COMBINATIONS
> pyxes =. test_basic2 t.''"1 COMBINATIONS
EMPTY
}}

NB. GET, PUT IN MULTIPLE THREADS.
NB. Keys and values are integers (permutations).

NB. x is boxed name of index type.
NB. y is number of iterations , number of worker threads , number of keys and values.
test_multithreading1 =: {{)d
'n_iter n_threads sz' =. y
jdict =. (x , < '') conew 'jdictionary'
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
    vals put__x keys NB. Put exactly the same keys and values.
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
}}"0 _

NB. Given matrix represented by dyadic verb magic which returns value of matrix at index (x, y).
NB. Each thread (task per column) will add values from one column to dictionary which stores sums of rows.
NB. Dictionary stores pairs of atomic values (currect_row_sum and counter_of_additions).
NB. When a row is handled by all threads, the last thread (according to counter for the row) removes it from dictionary and stores value in global array.
NB. x is boxed name of index type.
NB. y is number of rows , number of columns , number of threads
test_multithreading2 =: {{)d
'rows cols n_threads' =. y
params =. x , < ('valueshape' ; 2) ,: 'valuetype' ; 'boxed'
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
}}"0 _

NB. x is boxed name of index type.
NB. y is number of iterations , number of worker threads.
test_multithreading3 =: {{)d
'n_iter n_threads' =. y
jdict =. (x , < '') conew 'jdictionary'
NB. x is jdict
NB. y is number of iterations
prog =: {{)d
  for_k. i. y do.
    _1 get__x 2 | k
    k put__x 2 | >: k
    has__x 2 | >: k
    del__x 2 | k
  end.
  }}
NB. Run prog in each thread.
set_threads n_threads
> pyxes =. jdict prog t.''"0 n_threads $ n_iter
erase 'prog'
destroy__jdict ''
EMPTY
}}"0 _

NB. RUN MULTITHREADING.
{{
test_basic_multithreading"0 i. 5
INDEX_TYPES_CONCURRENT test_multithreading1 1000 5 200
INDEX_TYPES_CONCURRENT test_multithreading2 1 10000 3
{{ for. i. 10 do. INDEX_TYPES_CONCURRENT test_multithreading2 50 70 5 end. }} ''
{{ for. i. 10 do. INDEX_TYPES_CONCURRENT test_multithreading3 1000 3 end. }} ''
set_threads 0
EMPTY
}}^:((2 *@(17 b.) 9!:56'MEMAUDIT') < 9!:56'PYXES') ''


epilog''

