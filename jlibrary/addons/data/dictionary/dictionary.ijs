coclass 'jdictionary'

SIZE_GROWTH_GEOMETRIC_STEP =: 2

create =: {{)m
'index_type creation_parameters' =. y

NB. Default values of params.
keytype =: 4
keyshape =: i. 0
valuetype =: 4
valueshape =: i. 0
keyhash =: 16!:0`''
keycompare =: 16!:0`''
initsize =: 100
name =: ''

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
  internal_parameters =. (0 , initsize , <. initsize % occupancy) ; singlethreaded ; (keytype ; keyshape) ; < (valuetype ; valueshape)
case. 'tree' do.
  itype =: 1  NB. index type 1 is tree
  NB. Parse params and update above attributes.
  parse^:(*@#) creation_parameters
  if. 0 <: 4!:0 < 'occupancy' do.
     13!:8 (3) [ 'Parameter not supported in tree dictionary: occupancy'
  end.
  internal_parameters =. (0 , initsize) ; singlethreaded ; (keytype ; keyshape) ; < (valuetype ; valueshape)
case. do.
  13!:8 (3) [ 'Incorrect index type'
end.

NB. Create the map, which remains as dict.  dict is marked nondisplayable because 1 {:: dict is.
NB. If 1 {:: dict (keys) is an indirect type, it is death to touch or display any part of 1 {:: dict that is on the empty list.
NB. It might be better not to assign dict, to make it impossible to access 1 {:: dict from console level.  But we have to be able to run 16!:_5 on it - make 16!:_5 an adverb
if. keyhash -: keycompare do. keyfn =. keyhash `: 6 else. keyfn =. keyhash `: 6 : (keycompare `: 6) end.
size =: initsize
dict =: keyfn f. (16!:_1) internal_parameters

NB. Assign names.
if. name -: '' do. prefix =. suffix =. '' else. 'prefix suffix' =. (,{:)&.>/\. split_name name end.
(prefix , 'get' , suffix) =: dict 16!:_2
(prefix , 'put' , suffix) =: dict 16!:_3
(prefix , 'del' , suffix) =: dict 16!:_4
(prefix , 'has' , suffix) =: dict 16!:_12
(prefix , 'len' , suffix) =: 0&(16!:_8)@dict
if. index_type -: 'tree' do. (prefix , 'getkv' , suffix) =: dict 16!:_6 end.  NB. getkv only on rb trees
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
NB. Install the kvs from dict into newdict.  (e =. 1&(16!:_5) dict) (1) returns the list of empty key indexes; (2) erase the empty chains
NB. to allow the key block to be freed.  Then (<<<e) { keys/vals gives the kvs:
  empties =. (1) 16!:_5 dict   NB. get list of empties in dict, then erase the empty chains.  Prevents free error when releasing dict
  (newdict 16!:_3)&((<<<empties)&{)&:>/ 2 1 { dict  NB. Install all keys from dict into newdict
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

NB. y is string representing a name with possibly specified locale. Returns two boxes: name ; suffix with locale.
NB. Right part of hook.
NB. If '_' is the last character of y then the name has explicitly specified locale, so get the indexes of '_'.
NB. Left part of hook.
NB. Use the indexes to split the name or if the number of indexes is less than 2 then suppose name is from base.
NB. Note that number of indexes may be smaller then number of '_' e.g. 0 when condition from right part of the hook was false.
split_name =: ('__' ,~&< [)`(({. ,&< }.)~ _2&{)@.(2 <: #@])  ''"_`([: I. '_'&=)@.('_' -: {:)
