coclass 'jdict'

issym=:0  NB. dicts created through jdict are not symbols
check1x =: (_1:^:(_&-:))@:(13!:8@3^:(<&0))@:(13!:8@14^:('' -.@-: $))  NB. verify no get from empty values

create =: {{)m
if. 'literal' -: datatype y do.
  index_type =. y
  creation_parameters =. ''
else.
  'index_type creation_parameters' =. y
end.

NB. Names of params, and defaults
long_param_names =. ;: 'valuetype valueshape keytype keyshape    keyhash    keycompare   initcapacity name occupancy'
(long_param_names) =.      4   ;   (i. 0) ;     4  ;  (i. 0)  ; (16!:0`'') ; (16!:0`'') ;       100   ; '' ; 0.5
short_param_names =. _2 <\ 'vtvsktks'

if. (-: (index_type {.~ -@#)) 'concurrent' do.
  singlethreaded =. 0
  index_type =. (- # ' concurrent') }. index_type
else.
  singlethreaded =. 1
end.

if. #creation_parameters do. names =. {."1 creation_parameters [ values =. {:"1 creation_parameters else. names =. values =. 0$a: end.  NB. requests
names =. (((#short_param_names){.long_param_names),names) {~ (short_param_names,names) i. names  NB. xlate short names to long equivs
long_param_names =. 2 }.^:(issym) }:^:(index_type -.@-: 'hash') long_param_names  NB. disallow value for symbols, and occupancy for tree
namex =. long_param_names i. names
if. (#long_param_names) e. namex do.  13!:8&3 'incorrect attribute: ' , ;:^:_1 names #~ (#long_param_names) e. namex end.
(,names) =. ,values  NB. assign values to local names (, to unbox if singleton)

if. 2 = 3!:0 keytype do. keytype =. typeid_from_typename keytype end.  NB. translate type to internal code
if. 2 = 3!:0 valuetype do. valuetype =. typeid_from_typename valuetype end.
name =. ,&'__'^:('_' ~: {:!.'_') name  NB. if name is not empty and doesn't end with _, append __

select. index_type   NB. set up params for create, based on map type
case. 'hash' do.
  if. issym do.   NB. override & audit parms for symbols dict
    valueshape=.0 [ valuetype=.0  NB. type=0 signals symbols
    if. keytype e. 1 4 8 16 64 128 5 6 7 9 10 11 do. 13!:8&3 'symbol key cannot be numeric' end.
  elseif. valuetype=0 do. 13!:8&3 'valuetype invalid'
  end.
  internal_parameters =. (0 , initcapacity , <. initcapacity % occupancy) ; singlethreaded ; (keytype ; keyshape) ; < (valuetype ; valueshape)
case. 'tree' do.
  if. issym do. 13!:8@3 'symbol requires hash' end.
  internal_parameters =. (0 , initcapacity) ; singlethreaded ; (keytype ; keyshape) ; < (valuetype ; valueshape)
case. do.
  13!:8&3 'Incorrect index type'
end.

NB. Create the map, which remains as dict.  dict is marked nondisplayable because 1 {:: dict is.
NB. 1 2{::dict appear to be empty so that the user can't crash by touching them.  To see the actual values, use 1 2 (16!:_8) dict which returns a virtual block.
NB. dict is a local name; after this function completes the only link to dict is through the verbs.  This is because there might be no object
if. keyhash -: keycompare do. keyfn =. keyhash `: 6 else. keyfn =. keyhash `: 6 : (keycompare `: 6) end.
dict =. keyfn f. (16!:_1) internal_parameters

NB. Assign names.
clsnms =. 'get put items has count items invalidate close'
('get',name) =: dict 16!:_2
('put',name) =: dict 16!:_3
if. -.issym do. ('del',name) =: dict 16!:_4 [. clsnms =. clsnms , ' del' end.
('has',name) =: dict 16!:_12
('count',name) =: {.@(0&(16!:_8))@dict
('invalidate',name) =: dict 16!:_5~ 1:
if. index_type -: 'tree' do.
  valuemask =. 2b100 * valueshape -.@-: 0
  ('mget',name) =: dict 16!:_6
  ('min',name) =: [: 13!:8@6^:(0 = +/@:(#@>)) (2b11000 + valuemask)&(dict 16!:_6)
  ('max',name) =: [: 13!:8@6^:(0 = +/@:(#@>)) (2b11001 + valuemask)&(dict 16!:_6)
  ('items',name) =:(_1 ,~ 2b11001 + valuemask)&(dict 16!:_6)
  ('after',name) =: _&$: : (((dict 16!:_6)~ (2b101010 + valuemask) , check1x_jdict_)~)
  ('since',name) =: _&$: : (((dict 16!:_6)~ (2b101011 + valuemask) , check1x_jdict_)~)
  ('before',name) =: _&$: : (((dict 16!:_6)~ (2b101000 + valuemask) , check1x_jdict_)~)
  ('until',name) =: _&$: : (((dict 16!:_6)~ (2b101001 + valuemask) , check1x_jdict_)~)
  ('range',name) =: 1 1&$: : (((dict 16!:_6)~ 2b1001000 + valuemask + #.@:|.@:(13!:8@3^:(1 1 -.@-: e.&0 1))@:(13!:8@14^:((, 2) -.@-: $)))~)
  ('audittree',name) =: 16!:_7&dict
  clsnms =. clsnms , ' mget min max after since before until range audittree' 
else.
  ('items',name) =: dict {{
    (0 0) 16!:_9 m NB. read-lock.
    r =. memu (<<<0 (16!:_5) m) { 1 (16!:_8) m
    if. y do.
      r =. r ,&< memu (<<<0 (16!:_5) m) { 2 (16!:_8) m
    end.
    (1 0) 16!:_9 m
    r
  }}@(0~:valueshape)    NB. values only if they are not empty
end.
  ('close',name) =: (4!:55@;:@(,&name&.>&.;: clsnms)) , (dict 16!:_5~ 1:)  NB. clear the empty chain in the keys to avoid errors freeing it
dict  NB. We never use the dict itself, but in case a user wants to, this is a hook that they can use to get to it
}}

destroy =: {{
close''  NB. delete all verbs, which also deletes the dict
codestroy y  NB. destroy the locale, freeing everything
}}

NB. Resize operation.  Nilad.  Allocate a larger/smaller dictionary and repopulate its keys.  For the nonce we assume the call is to upsize
NB. The dictionary might not be in its own class (i. e. it might have a name), so we must not refer to class variables.  The dict is
NB. the y argument
resize =: {{)m
'cardinality minsize maxsize hashsiz' =. 0 (16!:_8) y  NB. fetch cardinality,minsize,maxsize,size of hash 0 if red/black)
maxsize =. 2 * maxsize 
NB. We allocate a new DIC block of the correct maxsize.  This is a temp whose contents, when filled, will be exchanged into (y)
NB. This also allocates new areas for the keys, vals, and hash/tree
if. hashsiz do.
  newy =. y (16!:_1) 0 , maxsize , 2 * hashsiz  NB. allocate new DIC (hashed)
  NB. for hashing: call (newy 16!:_3) to rehash all the keys.  Limit the number of kvs per install to reduce temp space needed.
  NB. Install the kvs from y into newy to allow the key block to be freed.  Then (<<<e) { keys/vals gives the kvs:
  empties =. (0) 16!:_5 y   NB. get list of empties in y
  (2 (16!:_8) y) (newy 16!:_3)&((<<<empties)&{) (1 (16!:_8) y)  NB. Install all keys from y into newy
  (1) 16!:_5 y  NB. Delete chains; prevents free error when releasing y
else.
  newy =. y (16!:_1) 0 , maxsize  NB. allocate new DIC (tree)
  NB. for red/black: copying the keys, vals, and tree from y to newy is done in JE when we return
end.
newy  NB. Return the new block.  Its contents will be swapped with the old block so that the EPILOG for the resize will free the old keys/vals/hash
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

coclass'jsymbol'  NB. The only extra thing about the symbol class is the path
issym=:1  NB. symbols are created through this locale
coinsert'jdict'

coclass'jdict'
