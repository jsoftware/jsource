prolog './g1x43.ijs'
NB. 1!:43 and 1!:44 -----------------------------------------------------

(1=#$s) *. 2=type s=: 1!:43 ''
'' -: 1!:44 s

'domain error' -: 1!:44 etx 0 1 0
'domain error' -: 1!:44 etx 1 2 3
'domain error' -: 1!:44 etx 1 2j3
'domain error' -: 1!:44 etx <s

'rank error'   -: 1!:44 etx ,:s

'length error' -: 1!:44 etx ''

'interface error' -: 1!:44 etx 'undefinedpath'

NB. 1!:46 ---------------------------------------------------------------

('Wasm'-:UNAME) +. *@# 1!:46''


epilog''

