prolog './g128x7.ijs'
NB. 128!:7 AES -------------------------------------------------------

fhex=: a. {~ [: ". '16b' (,"1) _2 ]\ ]

NB. rfc3602 The AES-CBC Cipher Algorithm and Its Use with IPsec
NB. Case #1: Encrypting 16 bytes (1 block) using AES-CBC with 128-bit key
Key       =: fhex '06a9214036b8a15b512e03d534120006'
IV        =: fhex '3dafba429d9eb430b422da802c9fac41'
Plaintext =: 'Single block msg'
Ciphertext=: fhex 'e353779c1079aeb82708942dbe77181a'

Ciphertext -: (0;Key;IV;'cbc nopad') (128!:7) Plaintext 
Plaintext -: (1;Key;IV;'cbc nopad') (128!:7) Ciphertext 

NB. Case #2: Encrypting 32 bytes (2 blocks) using AES-CBC with 128-bit key
Key       =: fhex 'c286696d887c9aa0611bbb3e2025a45a'
IV        =: fhex '562e17996d093d28ddb3ba695a2e6f58'
Plaintext =: fhex '000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f'
Ciphertext=: fhex 'd296cd94c2cccf8a3a863028b5e1dc0a7586602d253cfff91b8266bea6d61ab1'

Ciphertext -: (0;Key;IV;'cbc nopad') (128!:7) Plaintext 
Plaintext -: (1;Key;IV;'cbc nopad') (128!:7) Ciphertext 

NB. Case #3: Encrypting 48 bytes (3 blocks) using AES-CBC with 128-bit key
Key       =: fhex '6c3ea0477630ce21a2ce334aa746c2cd'
IV        =: fhex 'c782dc4c098c66cbd9cd27d825682c81'
Plaintext =: 'This is a 48-byte message (exactly 3 AES blocks)'
Ciphertext=: fhex 'd0a02b3836451753d493665d33f0e8862dea54cdb293abc7506939276772f8d5021c19216bad525c8579695d83ba2684'

Ciphertext -: (0;Key;IV;'cbc nopad') (128!:7) Plaintext 
Plaintext -: (1;Key;IV;'cbc nopad') (128!:7) Ciphertext 

NB. Case #4: Encrypting 64 bytes (4 blocks) using AES-CBC with 128-bit key
Key       =: fhex '56e47a38c5598974bc46903dba290349'
IV        =: fhex '8ce82eefbea0da3c44699ed7db51b7d9'
Plaintext =: fhex 'a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedf'
Ciphertext=: fhex 'c30e32ffedc0774e6aff6af0869f71aa0f3af07a9a31a9c684db207eb0ef8e4e35907aa632c3ffdf868bb7b29d3d46ad83ce9f9a102ee99d49a53e87f4c3da55'

Ciphertext -: (0;Key;IV;'cbc nopad') (128!:7) Plaintext 
Plaintext -: (1;Key;IV;'cbc nopad') (128!:7) Ciphertext 

test_decrypt_ecb=: 3 : 0
out=. fhex '6bc1bee22e409f96e93d7e117393172a'
if. y=256 do.
  key=. fhex '603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4'
  in=. fhex 'f3eed1bdb5d2a03c064b5a7e3db181f8'
elseif. y=192 do.
  key=. fhex '8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b'
  in=. fhex 'bd334f1d6e45f25ff712a214571fa5cc'
elseif. y=128 do.
  key=. fhex '2b7e151628aed2a6abf7158809cf4f3c'
  in=. fhex '3ad77bb40d7a3660a89ecaf32466ef97'
end.
out -: (1;key;(16$' ');'ecb nopad') (128!:7) in
)


test_encrypt_ecb=: 3 : 0
in=. fhex '6bc1bee22e409f96e93d7e117393172a'
#if defined(AES256)
if. y=256 do.
  key=. fhex '603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4'
  out=. fhex 'f3eed1bdb5d2a03c064b5a7e3db181f8'
elseif. y=192 do.
  key=. fhex '8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b'
  out=. fhex 'bd334f1d6e45f25ff712a214571fa5cc'
elseif. y=128 do.
  key=. fhex '2b7e151628aed2a6abf7158809cf4f3c'
  out=. fhex '3ad77bb40d7a3660a89ecaf32466ef97'
end.
out -: (0;key;(16$' ');'ecb nopad') (128!:7) in
)

test_decrypt_cbc=: 3 : 0
iv=. fhex '000102030405060708090a0b0c0d0e0f'
out=. fhex '6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710'
#if defined(AES256)
if. y=256 do.
  key=. fhex '603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4'
  in=. fhex 'f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b'
elseif. y=192 do.
  key=. fhex '8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b'
  in=. fhex '4f021db243bc633d7178183a9fa071e8b4d9ada9ad7dedf4e5e738763f69145a571b242012fb7ae07fa9baac3df102e008b0e27988598881d920a9e64f5615cd'
elseif. y=128 do.
  key=. fhex '2b7e151628aed2a6abf7158809cf4f3c'
  in=. fhex '7649abac8119b246cee98e9b12e9197d5086cb9b507219ee95db113a917678b273bed6b8e3c1743b7116e69e222295163ff1caa1681fac09120eca307586e1a7'
end.
out -: (1;key;iv;'cbc nopad') (128!:7) in
)

test_encrypt_cbc=: 3 : 0
iv=. fhex '000102030405060708090a0b0c0d0e0f'
in=. fhex '6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710'
if. y=256 do.
  key=. fhex '603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4'
  out=. fhex 'f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b'
elseif. y=192 do.
  key=. fhex '8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b'
  out=. fhex '4f021db243bc633d7178183a9fa071e8b4d9ada9ad7dedf4e5e738763f69145a571b242012fb7ae07fa9baac3df102e008b0e27988598881d920a9e64f5615cd'
elseif. y=128 do.
  key=. fhex '2b7e151628aed2a6abf7158809cf4f3c'
  out=. fhex '7649abac8119b246cee98e9b12e9197d5086cb9b507219ee95db113a917678b273bed6b8e3c1743b7116e69e222295163ff1caa1681fac09120eca307586e1a7'
end.
out -: (0;key;iv;'cbc nopad') (128!:7) in
)

test_xcrypt_ctr=: 4 : 0
iv=. fhex 'f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff'
out=. fhex '6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710'
if. y=256 do.
  key=. fhex '603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4'
  in=. fhex '601ec313775789a5b7a7f504bbf3d228f443e3ca4d62b59aca84e990cacaf5c52b0930daa23de94ce87017ba2d84988ddfc9c58db67aada613c2dd08457941a6'
elseif. y=192 do.
  key=. fhex '8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b'
  in=. fhex '1abc932417521ca24f2b0459fe7e6e0b090339ec0aa6faefd5ccc2c6f4ce8e941e36b26bd1ebc670d1bd1d665620abf74f78a7f6d29809585a97daec58c6b050'
elseif. y=128 do.
  key=. fhex '2b7e151628aed2a6abf7158809cf4f3c'
  in=. fhex '874d6191b620e3261bef6864990db6ce9806f66b7970fdff8617187bb9fffdff5ae4df3edbd5d35e5b4f09020db03eab1e031dda2fbe03d1792170a0f3009cee'
end.
out -: (x;key;iv;'ctr nopad') (128!:7) in
)

test_encrypt_ecb 128
test_decrypt_ecb 128
test_encrypt_cbc 128
test_decrypt_cbc 128
0&test_xcrypt_ctr 128
1&test_xcrypt_ctr 128

test_encrypt_ecb 192
test_decrypt_ecb 192
test_encrypt_cbc 192
test_decrypt_cbc 192
0&test_xcrypt_ctr 192
1&test_xcrypt_ctr 192

test_encrypt_ecb 256
test_decrypt_ecb 256
test_encrypt_cbc 256
test_decrypt_cbc 256
0&test_xcrypt_ctr 256
1&test_xcrypt_ctr 256

NB. test non-accelerated

test_1=: 3 : 0
assert. test_encrypt_ecb 128
assert. test_decrypt_ecb 128
assert. test_encrypt_cbc 128
assert. test_decrypt_cbc 128
assert. 0&test_xcrypt_ctr 128
assert. 1&test_xcrypt_ctr 128

assert. test_encrypt_ecb 192
assert. test_decrypt_ecb 192
assert. test_encrypt_cbc 192
assert. test_decrypt_cbc 192
assert. 0&test_xcrypt_ctr 192
assert. 1&test_xcrypt_ctr 192

assert. test_encrypt_ecb 256
assert. test_decrypt_ecb 256
assert. test_encrypt_cbc 256
assert. test_decrypt_cbc 256
assert. 0&test_xcrypt_ctr 256
assert. 1&test_xcrypt_ctr 256

1
)

3 : 0''
if.('arm64'-:9!:56'cpu') *. 9!:56'aes' do.
  0 (9!:56) 'aes'
  test_1''
  1 (9!:56) 'aes'
elseif. (('x86'-:9!:56'cpu')+.('x86_64'-:9!:56'cpu')) *. (9!:56'sse4_1') *. (9!:56'ase_ni') do.
  0 (9!:56) 'sse4_1'
  0 (9!:56) 'aes_ni'
  test_1''
  1 (9!:56) 'sse4_1'
  1 (9!:56) 'aes_ni'
end.
1
)

f=: 128!:7

NB. null padding

msg=: 'hello world'
((0;(16{.a.);(16{.a.);'ecb nopad') f msg) -: ((0;(16{.a.);(16{.a.);'ecb nopad') f msg,5#{.a.)
((0;(16{.a.);(16{.a.);'cbc nopad') f msg) -: ((0;(16{.a.);(16{.a.);'cbc nopad') f msg,5#{.a.)
((0;(16{.a.);(16{.a.);'ctr nopad') f msg) -: ((0;(16{.a.);(16{.a.);'ctr nopad') f msg,5#{.a.)
((0;(24{.a.);(16{.a.);'ecb nopad') f msg) -: ((0;(24{.a.);(16{.a.);'ecb nopad') f msg,5#{.a.)
((0;(24{.a.);(16{.a.);'cbc nopad') f msg) -: ((0;(24{.a.);(16{.a.);'cbc nopad') f msg,5#{.a.)
((0;(24{.a.);(16{.a.);'ctr nopad') f msg) -: ((0;(24{.a.);(16{.a.);'ctr nopad') f msg,5#{.a.)
((0;(32{.a.);(16{.a.);'ecb nopad') f msg) -: ((0;(32{.a.);(16{.a.);'ecb nopad') f msg,5#{.a.)
((0;(32{.a.);(16{.a.);'cbc nopad') f msg) -: ((0;(32{.a.);(16{.a.);'cbc nopad') f msg,5#{.a.)
((0;(32{.a.);(16{.a.);'ctr nopad') f msg) -: ((0;(32{.a.);(16{.a.);'ctr nopad') f msg,5#{.a.)

((0;(16{.a.);(16{.a.);'ecb nopad') f 2#msg) -: ((0;(16{.a.);(16{.a.);'ecb nopad') f (2#msg),10#{.a.)
((0;(16{.a.);(16{.a.);'cbc nopad') f 2#msg) -: ((0;(16{.a.);(16{.a.);'cbc nopad') f (2#msg),10#{.a.)
((0;(16{.a.);(16{.a.);'ctr nopad') f 2#msg) -: ((0;(16{.a.);(16{.a.);'ctr nopad') f (2#msg),10#{.a.)
((0;(24{.a.);(16{.a.);'ecb nopad') f 2#msg) -: ((0;(24{.a.);(16{.a.);'ecb nopad') f (2#msg),10#{.a.)
((0;(24{.a.);(16{.a.);'cbc nopad') f 2#msg) -: ((0;(24{.a.);(16{.a.);'cbc nopad') f (2#msg),10#{.a.)
((0;(24{.a.);(16{.a.);'ctr nopad') f 2#msg) -: ((0;(24{.a.);(16{.a.);'ctr nopad') f (2#msg),10#{.a.)
((0;(32{.a.);(16{.a.);'ecb nopad') f 2#msg) -: ((0;(32{.a.);(16{.a.);'ecb nopad') f (2#msg),10#{.a.)
((0;(32{.a.);(16{.a.);'cbc nopad') f 2#msg) -: ((0;(32{.a.);(16{.a.);'cbc nopad') f (2#msg),10#{.a.)
((0;(32{.a.);(16{.a.);'ctr nopad') f 2#msg) -: ((0;(32{.a.);(16{.a.);'ctr nopad') f (2#msg),10#{.a.)

NB. pkcs7 padding

pkcs=: 4 : 0
key=. 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef'
iv=. 'uvwxyz0123456789'
mode=. x
msg=. ,y
pad=. >:16|->:#msg
txt=. msg,pad#pad{a.
assert. 0=16|#txt
j=. (0;key;iv;mode,' nopad') 128!:7 txt
s=. (0;key;iv;mode) 128!:7 msg
k=. (1;key;iv;mode,' nopad') 128!:7 j
t=. (1;key;iv;mode) 128!:7 s
(j-:s)*.(k-:txt)*.(t-:msg)
)

'ecb' pkcs ''
'ecb' pkcs 'h'
'ecb' pkcs 'hello world'
'ecb' pkcs 'hello world1234'
'ecb' pkcs 'hello world12345'
'ecb' pkcs 'hello world123456'

'cbc' pkcs ''
'cbc' pkcs 'h'
'cbc' pkcs 'hello world'
'cbc' pkcs 'hello world1234'
'cbc' pkcs 'hello world12345'
'cbc' pkcs 'hello world123456'

'ctr' pkcs ''
'ctr' pkcs 'h'
'ctr' pkcs 'hello world'
'ctr' pkcs 'hello world1234'
'ctr' pkcs 'hello world12345'
'ctr' pkcs 'hello world123456'

'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'
'domain error' -: f etx u: 'abc'
'domain error' -: f etx 10 u: 'abc'

'domain error' -: 123           f etx 3 4 5
'domain error' -: 123           f etx 3 4 5x
'domain error' -: 123           f etx 3.4 5
'domain error' -: 123           f etx 3j4 5
'domain error' -: 123           f etx 3r4 5

'domain error' -: 0             f etx 'xyz'
'domain error' -: 16            f etx 'xyz'
'domain error' -: _16           f etx 'xyz'
'domain error' -: '34'          f etx 'xyz'
'domain error' -: (u:'34')      f etx 'xyz'
'domain error' -: (10&u:'34')   f etx 'xyz'
'domain error' -: (s:@<"0 '34') f etx 'xyz'
'domain error' -: 3.4           f etx 'xyz'
'domain error' -: 3j4           f etx 'xyz'
'length error' -: (<'abc')      f etx 'xyz'
'length error' -: (<u:'abc')    f etx 'xyz'
'length error' -: (<10&u:'abc') f etx 'xyz'
'length error' -: (34;'a')      f etx 'xyz'
'domain error' -: (34;56;3)     f etx 'xyz'
'domain error' -: (0;(16{.a.);(16{.a.);'cbcc') f etx 'xyz'
'domain error' -: (0;(16{.a.);(16{.a.);'cbc pad') f etx 'xyz'
'domain error' -: (0;(16{.a.);(16{.a.);'cfb')  f etx 'xyz'
'domain error' -: (0;(16{.a.);(16{.a.);'ofb nopad')  f etx 'xyz'
'domain error' -: (0;(16{.a.);(16{.a.);123)    f etx 'xyz'
'domain error' -: (0;(17{.a.);(16{.a.);'cbc')  f etx 'xyz'
'domain error' -: (0;123;(16{.a.);'cbc')       f etx 'xyz'
'domain error' -: (0;(16{.a.);123)             f etx 'xyz'
'domain error' -: (2;(16{.a.);(16{.a.);'ctr')  f etx 'xyz'
'domain error' -: ('abc';(16{.a.);(16{.a.))    f etx 'xyz'
'length error' -: (1;(16{.a.);(16{.a.);'ctr';0)f etx 'xyz'
'length error' -: (1;(16{.a.))                 f etx 'xyz'
'length error' -: (1;(16{.a.);(16{.a.))        f etx 'xyz'

4!:55 ;:'f msg pkcs fhex Key IV Plaintext Ciphertext'
4!:55 ;:'test_encrypt_ecb test_decrypt_ecb test_encrypt_cbc test_decrypt_cbc test_xcrypt_ctr test_1'

epilog''

