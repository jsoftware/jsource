NB. this stub will be removed in future releases
NB. please, update your sources:
NB. - replace string 'addons/math/lapack/dgeev.ijs' by 'addons/math/lapack/geev.ijs'
NB. - replace strings 'dgeev' by 'geev', 'tdgeev' by 'tgeev', 'testdgeev' by 'testgeev'

require '~addons/math/lapack/geev.ijs'

coclass 'jlapack'

dgeev=: geev
tdgeev=: tgeev
testdgeev=: testgeev
