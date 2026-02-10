prolog './gdic2.ijs'
NB. addon/dictionary

1: 0!:_1 '$'   NB. skip this test

dumpchk=: 4 : 0
echo x,':'
echo 'count: ', ":count__y''
(1) 16!:_7 dict__y
EMPTY
)

require 'data/dictionary'

coclass 'naivedictionary'

create=: {{
keys=: 0 $ a:
vals=: 0 $ a:
}}

destroy=: codestroy

has=: {{ keys e.~ < y }}

put=: {{
del y
keys=: keys , < y
vals=: vals , < x
EMPTY
}}

get=: {{ > vals {~ keys i. < y }}

del=: {{
idx=. keys i. < y
if. idx < # keys do.
  keys=: keys -. < y
  vals=: (idx {. vals) , (>: idx) }. vals
end.
assert. (# keys) -: # vals
EMPTY
}}

count=: {{ # keys }}

cocurrent 'base'

rand_integer=: {{ >: ? <. 3 }}
rand_boolean=: {{ ? 2 }}

test_type=: {{)m
batchshape=. 1
keyshape=. 3
valshape=. 5
params=. y , <,. ('keytype' ; 'boolean') , ('keyshape' ; keyshape) ,: ('valueshape' ; valshape)
echo params
mydict=. params conew 'jdictionary'

naivedict=. '' conew 'naivedictionary'
keyrank=. # keyshape
valrank=. # valshape

assert. (count__mydict '') -: count__naivedict ''
NB. Put.
keys=. 0 0 0
vals=. 2 1 2 2 2
echo 'put'
echo 'keys'
echo keys
echo 'vals'
echo vals

vals put__naivedict"(valrank , keyrank) keys
assert. EMPTY -: vals put__mydict keys
'put'dumpchk mydict
assert. (count__mydict '') -: count__naivedict ''
NB. Get.
keys=. 0 0 0
vals=. 2 2 3 1 3
echo 'get'
echo 'keys'
echo keys
echo 'vals'
echo vals
naivemask=. has__naivedict"keyrank keys
jhasans=. has__mydict keys
assert. jhasans -: naivemask
'get'dumpchk mydict
if. -. valshape -: 0 do.
  fillatom=. {. valuetype__mydict c. ''
  jgetans=. (valshape $ fillatom) get__mydict keys
  jmask=. +./@,"valrank ] fillatom ~: jgetans
  assert. jmask -: naivemask
  naivegetans=. get__naivedict"keyrank (, naivemask) # (_ , keyshape) ($ ,) keys
  if. 1 -: +./ , naivemask do.
    assert. naivegetans -: (, jmask) # (_ , valshape) ($ ,) jgetans
  end.
end.
NB. Delete.
batchshapefordel=. <. -: batchshape
keys=. (batchshapefordel , keyshape) $ 0
echo 'delete'
echo 'keys shape: ',":$keys
echo 'keys'
echo keys
naivemask=. has__naivedict"keyrank keys
del__naivedict"keyrank keys
jdelans=. del__mydict keys
'del'dumpchk mydict
echo 'count mydict ',": count__mydict ''
echo 'count naivedict ',": count__naivedict ''
assert. (count__mydict '') -: count__naivedict ''
echo 'count ok'
assert. jdelans -: naivemask *. batchshapefordel ($ ,) ~:&.|. (_ , keyshape) ($ ,) keys
echo 'jdelans ok'
destroy__naivedict ''
destroy__mydict ''
echo 'finish test_type'
EMPTY
}}"0 'tree concurrent';'tree'


NB.$    end of skip

epilog''

