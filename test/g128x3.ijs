NB. 128!:3 crc-32 -------------------------------------------------------

NB. modified from Henry Rich msg to J forum 2005-01-10

NB. CRC calculation.  
NB. This builds the CRC by passing the bytes into the
NB. MSB of the shiftregister and shifting right, 
NB. applying the polynomial (inverted) using the LSB.  
NB. Some other implementations seem to process bits in a different order.  
NB. CRC-32 of '123456789' is CBF43926

NB. crcbyte: One calculation: y is shiftregister, x is new byte
NB. crc:     y is string or numeric vector; result is CRC-32

bitand  =: 17 b.
bitxor  =: 22 b.
bitshift=: 33 b.

shift=: |.!.''

mask32  =: <._1+IF64*2^32
crcpolyb=: |. 0 0 0 0  0 1 0 0  1 1 0 0  0 0 0 1  0 0 0 1  1 1 0 1  1 0 1 1  0 1 1 1
crcpolyi=: mask32 bitand (_2 _2,30$2) #. crcpolyb
crctbli =: (_1&bitshift)`(crcpolyi&bitxor@(_1&bitshift))@.(2&|)^:8"0
crctblb =: bitshift`(crcpolyb&bitxor@shift)@.{:^:8"0
prep    =: |. @ (mask32&,) @ (a.&i.^:(2:=3!:0))
crcbyte =: {&(crctbli i.256)@(255&bitand)@bitxor bitxor _8&bitshift@]
crc     =: _1&bitxor @ (crcbyte/) @ prep

f=: 128!:3

_873187034 -:               f x=: '123456789'
_873187034 -: crcpolyi      f x
_873187034 -: crcpolyb      f x
_873187034 -: (<crcpolyi)   f x
_873187034 -: (<crcpolyb)   f x
_873187034 -: (crcpolyi;_1) f x
_873187034 -: (crcpolyb;_1) f x

(f -: crc) x
(f -: crc) x=: 'assiduously avoid any and all asinine alliterations'

b=: 32 ?@$ 2

'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4

'rank error'   -: f etx 3 4$'abc'

'domain error' -: 123           f etx 3 4 5
'domain error' -: 123           f etx 3 4 5x
'domain error' -: 123           f etx 3.4 5
'domain error' -: 123           f etx 3j4 5
'domain error' -: 123           f etx 3r4 5

'domain error' -: '34'          f etx 'xyz'
'domain error' -: 3.4           f etx 'xyz'
'domain error' -: 3j4           f etx 'xyz'
'domain error' -: (<'abc')      f etx 'xyz'
'domain error' -: (34;'a')      f etx 'xyz'
'domain error' -: (b;'a')       f etx 'xyz'

'length error' -: (34;56;3)     f etx 'xyz'
'length error' -: (b;_1;3)      f etx 'xyz'


4!:55 ;:'b bitand bitshift bitxor crc crcbyte crcpoly crcpolyb crcpolyi crctbl'
4!:55 ;:'crctblb crctbli'
4!:55 ;:'f mask32 p prep shift x '



