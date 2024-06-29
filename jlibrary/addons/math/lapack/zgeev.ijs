NB. this stub will be removed in future releases
NB. please, update your sources:
NB. - replace string 'addons/math/lapack/zgeev.ijs' by 'addons/math/lapack/geev.ijs'
NB. - replace strings 'zgeev' by 'geev', 'tzgeev' by 'tgeev', 'testzgeev' by 'testgeev'

require '~addons/math/lapack/geev.ijs'

coclass 'jlapack'

zgeev=: geev
tzgeev=: tgeev
testzgeev=: testgeev
