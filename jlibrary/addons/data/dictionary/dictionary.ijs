coclass 'jdictionary'

SIZE_GROWTH_GEOMETRIC_STEP =: 2

create =: {{)m
if. 'literal' -: datatype y do.
  index_type =. y
  creation_parameters =. ''
else.
  'index_type creation_parameters' =. y
end.
NB. Default values of params.
keytype =: 4
keyshape =: i. 0
valuetype =: 4
valueshape =: i. 0
keyhash =: 16!:0`''
keycompare =: 16!:0`''
initcapacity =: 100

if. (-: (index_type {.~ -@#)) 'concurrent' do.
  singlethreaded =. 0
  index_type =. (- # ' concurrent') }. index_type
else.
  singlethreaded =. 1
end.

select. index_type   NB. set up params for create, based on map type
case. 'hash' do.
  itype =: 0   NB. index type 0 is hash
  occupancy =: 0.5   NB. default for occupancy
  NB. Parse params and update above attributes.
  parse^:(*@#) creation_parameters
  internal_parameters =. (0 , initcapacity , <. initcapacity % occupancy) ; singlethreaded ; (keytype ; keyshape) ; < (valuetype ; valueshape)
case. 'tree' do.
  itype =: 1  NB. index type 1 is tree
  NB. Parse params and update above attributes.
  parse^:(*@#) creation_parameters
  if. 0 <: 4!:0 < 'occupancy' do.
     13!:8 (3) [ 'Parameter not supported in tree dictionary: occupancy'
  end.
  internal_parameters =. (0 , initcapacity) ; singlethreaded ; (keytype ; keyshape) ; < (valuetype ; valueshape)
case. do.
  13!:8 (3) [ 'Incorrect index type'
end.

NB. Create the map, which remains as dict.  dict is marked nondisplayable because 1 {:: dict is.
NB. 1 2{::dict appear to be empty so that the user can't crash by touching them.  To see the actual values, use 1 2 (16!:_8) dict which returns a virtual block.
if. keyhash -: keycompare do. keyfn =. keyhash `: 6 else. keyfn =. keyhash `: 6 : (keycompare `: 6) end.
size =: initcapacity
dict =: keyfn f. (16!:_1) internal_parameters

NB. Assign names.
get =: dict 16!:_2
put =: dict 16!:_3
del =: dict 16!:_4
has =: dict 16!:_12
count =: 0&(16!:_8)@dict
if. index_type -: 'tree' do.
  valuemask =. 2b100 * valueshape -.@-: 0
  check1x =: (_1:^:(_&-:))@:(13!:8@3^:(<&0))@:(13!:8@14^:('' -.@-: $))
  mget =: dict 16!:_6
  min =: [: 13!:8@6^:(0 = +/@:(#@>)) (2b11000 + valuemask)&mget
  max =: [: 13!:8@6^:(0 = +/@:(#@>)) (2b11001 + valuemask)&mget
  after =: _&$: : ((mget~ (2b101010 + valuemask) , check1x)~)
  since =: _&$: : ((mget~ (2b101011 + valuemask) , check1x)~)
  before =: _&$: : ((mget~ (2b101000 + valuemask) , check1x)~)
  until =: _&$: : ((mget~ (2b101001 + valuemask) , check1x)~)
  range =: 1 1&$: : ((mget~ 2b1001000 + valuemask + #.@:|.@:(13!:8@3^:(1 1 -.@-: e.&0 1))@:(13!:8@14^:((, 2) -.@-: $)))~)
end.
EMPTY
}}

destroy =: {{
(1) 16!:_5 dict  NB. clear the empty chain in the keys to avoid errors freeing it
codestroy y  NB. destroy the locale, freeing everything
}}

NB. Resize operation.  Nilad.  Allocate a larger/smaller dictionary and repopulate its keys
NB. We have a lock on (dict) during this entire operation
resize =: {{)m
size =: SIZE_GROWTH_GEOMETRIC_STEP * size
NB. We allocate a new DIC block of the correct size.  This is a temp whose contents, when filled, will be exchanged into (dict)
NB. This also allocates new areas for the keys, vals, and hash/tree
select. itype
case. 0 do.
  newdict =. dict (16!:_1) 0 , size , <. size * % occupancy  NB. allocate new DIC (hashed)
NB. for hashing: call (newdict 16!:_3) to rehash all the keys.  Limit the number of kvs per install to reduce temp space needed.
NB. Install the kvs from dict into newdict.
NB. to allow the key block to be freed.  Then (<<<e) { keys/vals gives the kvs:
  empties =. (0) 16!:_5 dict   NB. get list of empties in dict
  (2 (16!:_8) dict) (newdict 16!:_3)&((<<<empties)&{) (1 (16!:_8) dict)  NB. Install all keys from dict into newdict
  (1) 16!:_5 dict  NB. Delete chains; prevents free error when releasing dict
case. 1 do.
  newdict =. dict (16!:_1) 0 , size  NB. allocate new DIC (tree)
  NB. for red/black: copying the keys, vals, and tree from dict to newdict is done in JE when we return
end.
newdict  NB. Return the new block.  Its contents will be swapped with the old block so that the EPILOG for the resize will free the old keys/vals/hash
}}

NB. Utils.
NB. Gives type ID (e.g. 4) from type name (e.g. integer).
typeid_from_typename =: {{)m
n =. 1 2 4 8 16 32 64 128 1024 2048 4096 8192 16384 32768 65536 131072 262144
n =. n , 5 6 7 9 10 11
n =. n , _1 NB. _1 if y is not a name of any type.
t =. '/boolean/literal/integer/floating/complex/boxed/extended/rational'
t =. t , '/sparse boolean/sparse literal/sparse integer/sparse floating'
t =. t , '/sparse complex/sparse boxed/symbol/unicode/unicode4'
t =. t , '/integer1/integer2/integer4/floating2/floating4/floating16'
n {~ (<;._1 t) i. < y
}}

NB. Parse attribute and set its value.
parse =: {{)m
'attribute value' =: y
if. ('literal' -: datatype value) *. (attribute -: 'keytype') +. attribute -: 'valuetype' do.
  value =. typeid_from_typename value
end.
(attribute) =: value
EMPTY
}}"1
