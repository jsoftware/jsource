prolog './g128x3.ijs'
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
bitshifts=: 34 b.

shift=: |.!.''

mask32  =: <._1+IF64*2^32
crcpolyb=: |. 0 0 0 0  0 1 0 0  1 1 0 0  0 0 0 1  0 0 0 1  1 1 0 1  1 0 1 1  0 1 1 1
crcpolyi=: mask32 bitand (_2 _2,30$2) #. crcpolyb
crctbli =: (_1&bitshift)`(crcpolyi&bitxor@(_1&bitshift))@.(2&|)^:8"0
crctblb =: bitshift`(crcpolyb&bitxor@shift)@.{:^:8"0
prep    =: |. @ (mask32&,) @ (a.&i.^:(2:=3!:0))
crcbyte =: {&(crctbli i.256)@(255&bitand)@bitxor bitxor _8&bitshift@]
crc     =: (_32 bitshifts 32 bitshift ])^:IF64 @ (_1&bitxor) @ (crcbyte/) @ prep

f=: 128!:3

_873187034 -:               f x=: '123456789'
_873187034 -: crcpolyi      f x
_873187034 -: crcpolyb      f x
_873187034 -: (<crcpolyi)   f x
_873187034 -: (<crcpolyb)   f x
_873187034 -: (crcpolyi;_1) f x
_873187034 -: (crcpolyb;_1) f x

g =: crcpolyi&(128!:3)
_873187034 -: g x
g =: crcpolyb&(128!:3)
_873187034 -: g x
g =: (<crcpolyi)&(128!:3)
_873187034 -: g x
g =: (<crcpolyb)&(128!:3)
_873187034 -: g x
g =: (crcpolyi;_1)&(128!:3)
_873187034 -: g x
g =: (crcpolyb;_1)&(128!:3)
_873187034 -: g x

(f -: crc) x
(f -: crc) x=: 'assiduously avoid any and all asinine alliterations'

NB. literal2/literal4
NB. x=: 'assiduously avoid any and all asinine iterations'
NB. (f 6&u: x) = f x
NB. (f 10&u: (IF64{_2 _4) ic x) = f , _4 Endian \ x

b=: 32 ?@$ 2

'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'

'rank error'   -: f etx 3 4$'abc'
'rank error'   -: f etx 3 4$u:'abc'
'rank error'   -: f etx 3 4$10&u:'abc'
'rank error'   -: f etx 3 4$s:@<"0 'abc'

'domain error' -: 123           f etx 3 4 5
'domain error' -: 123           f etx 3 4 5x
'domain error' -: 123           f etx 3.4 5
'domain error' -: 123           f etx 3j4 5
'domain error' -: 123           f etx 3r4 5

'domain error' -: '34'          f etx 'xyz'
'domain error' -: (u:'34')      f etx 'xyz'
'domain error' -: (10&u:'34')   f etx 'xyz'
'domain error' -: (s:@<"0 '34') f etx 'xyz'
'domain error' -: 3.4           f etx 'xyz'
'domain error' -: 3j4           f etx 'xyz'
'domain error' -: (<'abc')      f etx 'xyz'
'domain error' -: (<u:'abc')    f etx 'xyz'
'domain error' -: (<10&u:'abc') f etx 'xyz'
'domain error' -: (34;'a')      f etx 'xyz'
'domain error' -: (b;'a')       f etx 'xyz'

'length error' -: (34;56;3)     f etx 'xyz'
'length error' -: (b;_1;3)      f etx 'xyz'

NB. base64

f =: 3!:10
'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'
'domain error' -: f etx u: 'abc'
'domain error' -: f etx 10&u: 'abc'

'' -: 3!:10 ''
'Zg==' -: 3!:10 'f'
'Zg==' -: 3!:10 ,'f'
'Zm8=' -: 3!:10 'fo'
'Zm9v' -: 3!:10 'foo'
'Zm9vYg==' -: 3!:10 'foob'
'Zm9vYmE=' -: 3!:10 'fooba'
'Zm9vYmFy' -: 3!:10 'foobar'

f =: 3!:11
'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'
'domain error' -: f etx u: 'abc'
'domain error' -: f etx 10&u: 'abc'
'domain error' -: 3!:11 etx 'a'
'domain error' -: 3!:11 etx ',a'

(3!:11 '') -:  ''
(3!:11 'Zg==') -: ,'f'
(3!:11 'Zm8=') -:  'fo'
(3!:11 'Zm9v') -:  'foo'
(3!:11 'Zm9vYg==') -:  'foob'
(3!:11 'Zm9vYmE=') -:  'fooba'
(3!:11 'Zm9vYmFy') -:  'foobar'

(-:   3!:11 @: (3!:10)) a. {~ 50 37 ?@$ 256
(-:   3!:11 @: (3!:10)) a. {~ 35 36 ?@$ 256
(-:   3!:11 @: (3!:10)) a. {~ 41 35 ?@$ 256



4!:55 ;:'b bitand bitshift bitshifts bitxor crc crcbyte crcpoly crcpolyb crcpolyi crctbl'
4!:55 ;:'crctblb crctbli'
4!:55 ;:'f g mask32 p prep shift x '




epilog''

