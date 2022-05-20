prolog './g128x6.ijs'
NB. 128!:6 SHA -------------------------------------------------------

NB. keccak test vector
NB. https://emn178.github.io/online-tools/keccak_512.html

NB. from Chris Burke
NB. untested for big-endian
sha1sum=: 3 : 0
b=. ,(8#2) #: a.i.y
p=. b,1,((512|-65+#b)#0),(64#2)#:#b
h=. #: 16b67452301 16befcdab89 16b98badcfe 16b10325476 16bc3d2e1f0
q=. (|._512<\p),<h
r=. > sha1sum_process each/ q
'0123456789abcdef' {~ _4 #.\ r
)
sha1sum_process=: 4 :0
plus=. +&.((32#2)&#.)
K=. ((32#2) #: 16b5a827999 16b6ed9eba1 16b8f1bbcdc 16bca62c1d6) {~ <.@%&20
W=. (, [: , 1 |."#. _3 _8 _14 _16 ~:/@:{ ])^:64 x ]\~ _32
'A B C D E'=. y=. _32[\,y
for_t. i.80 do.
  TEMP=. (5|.A) plus (t sha1sum_step B,C,D) plus E plus (W{~t) plus K t
  E=. D
  D=. C
  C=. 30 |. B
  B=. A
  A=. TEMP
end.
,y plus A,B,C,D,:E
)
sha1sum_step=: 4 : 0
'B C D'=. _32 ]\ y
if. x < 20 do.
  (B*C)+.D>B
elseif. x < 40 do.
  B~:C~:D
elseif. x < 60 do.
  (B*C)+.(B*D)+.C*D
elseif. x < 80 do.
  B~:C~:D
end.
)

NB. Test Vectors

SHA_1=: (128!:6)
SHA_224=: 2&(128!:6)
SHA_256=: 3&(128!:6)
SHA_384=: 4&(128!:6)
SHA_512=: 5&(128!:6)
SHA_3_224=: 6&(128!:6)
SHA_3_256=: 7&(128!:6)
SHA_3_384=: 8&(128!:6)
SHA_3_512=: 9&(128!:6)
KECCAK_224=: 10&(128!:6)
KECCAK_256=: 11&(128!:6)
KECCAK_384=: 12&(128!:6)
KECCAK_512=: 13&(128!:6)
MD4=: 14&(128!:6)
MD5=: 15&(128!:6)

s=: 'abc'
(SHA_1 -: sha1sum) s

(SHA_1     s) -: ' '-.~ 'a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d'
(SHA_224   s) -: ' '-.~ '23097d22 3405d822 8642a477 bda255b3 2aadbce4 bda0b3f7 e36c9da7'
(SHA_256   s) -: ' '-.~ 'ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad'
(SHA_384   s) -: ' '-.~ 'cb00753f45a35e8b b5a03d699ac65007 272c32ab0eded163 1a8b605a43ff5bed 8086072ba1e7cc23 58baeca134c825a7'
(SHA_512   s) -: ' '-.~ 'ddaf35a193617aba cc417349ae204131 12e6fa4e89a97ea2 0a9eeee64b55d39a 2192992a274fc1a8 36ba3c23a3feebbd 454d4423643ce80e 2a9ac94fa54ca49f'
(SHA_3_224 s) -: ' '-.~ 'e642824c3f8cf24a d09234ee7d3c766f c9a3a5168d0c94ad 73b46fdf'
(SHA_3_256 s) -: ' '-.~ '3a985da74fe225b2 045c172d6bd390bd 855f086e3e9d525b 46bfe24511431532'
(SHA_3_384 s) -: ' '-.~ 'ec01498288516fc9 26459f58e2c6ad8d f9b473cb0fc08c25 96da7cf0e49be4b2 98d88cea927ac7f5 39f1edf228376d25'
(SHA_3_512 s) -: ' '-.~ 'b751850b1a57168a 5693cd924b6b096e 08f621827444f70d 884f5d0240d2712e 10e116e9192af3c9 1a7ec57647e39340 57340b4cf408d5a5 6592f8274eec53f0'
(KECCAK_224 s)-: ' '-.~ 'c30411768506ebe1 c2871b1ee2e87d38 df342317300a9b97 a95ec6a8'
(KECCAK_256 s)-: ' '-.~ '4e03657aea45a94f c7d47ba826c8d667 c0d1e6e33a64a036 ec44f58fa12d6c45'
(KECCAK_384 s)-: ' '-.~ 'f7df1165f033337b e098e7d288ad6a2f 74409d7a60b49c36 642218de161b1f99 f8c681e4afaf31a3 4db29fb763e3c28e'
(KECCAK_512 s)-: ' '-.~ '18587dc2ea106b9a 1563e32b3312421c a164c7f1f07bc922 a9c83d77cea3a1e5 d0c6991073902537 2dc14ac964262937 9540c17e2a65b19d77aa511a9d00bb96'
(MD4       s) -: ' '-.~ 'a448017aaf21d852 5fc10ae87aa6729d'
(MD5       s) -: ' '-.~ '900150983cd24fb0 d6963f7d28e17f72'

s=: ''
(SHA_1 -: sha1sum) s

(SHA_1     s) -: ' '-.~ 'da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709'
(SHA_224   s) -: ' '-.~ 'd14a028c 2a3a2bc9 476102bb 288234c4 15a2b01f 828ea62a c5b3e42f'
(SHA_256   s) -: ' '-.~ 'e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855'
(SHA_384   s) -: ' '-.~ '38b060a751ac9638 4cd9327eb1b1e36a 21fdb71114be0743 4c0cc7bf63f6e1da 274edebfe76f65fb d51ad2f14898b95b'
(SHA_512   s) -: ' '-.~ 'cf83e1357eefb8bd f1542850d66d8007 d620e4050b5715dc 83f4a921d36ce9ce 47d0d13c5d85f2b0 ff8318d2877eec2f 63b931bd47417a81 a538327af927da3e'
(SHA_3_224 s) -: ' '-.~ '6b4e03423667dbb7 3b6e15454f0eb1ab d4597f9a1b078e3f 5b5a6bc7'
(SHA_3_256 s) -: ' '-.~ 'a7ffc6f8bf1ed766 51c14756a061d662 f580ff4de43b49fa 82d80a4b80f8434a'
(SHA_3_384 s) -: ' '-.~ '0c63a75b845e4f7d 01107d852e4c2485 c51a50aaaa94fc61 995e71bbee983a2a c3713831264adb47 fb6bd1e058d5f004'
(SHA_3_512 s) -: ' '-.~ 'a69f73cca23a9ac5 c8b567dc185a756e 97c982164fe25859 e0d1dcc1475c80a6 15b2123af1f5f94c 11e3e9402c3ac558 f500199d95b6d3e3 01758586281dcd26'
(KECCAK_224 s)-: ' '-.~ 'f71837502ba8e108 37bdd8d365adb855 91895602fc552b48 b7390abd'
(KECCAK_256 s)-: ' '-.~ 'c5d2460186f7233c 927e7db2dcc703c0 e500b653ca82273b 7bfad8045d85a470'
(KECCAK_384 s)-: ' '-.~ '2c23146a63a29acf 99e73b88f8c24eaa 7dc60aa771780ccc 006afbfa8fe2479b 2dd2b21362337441 ac12b515911957ff'
(KECCAK_512 s)-: ' '-.~ '0eab42de4c3ceb92 35fc91acffe746b2 9c29a8c366b7c60e 4e67c466f36a4304 c00fa9caf9d87976 ba469bcbe06713b4 35f091ef2769fb16 0cdab33d3670680e'
(MD4       s) -: ' '-.~ '31d6cfe0d16ae931 b73c59d7e0c089c0'
(MD5       s) -: ' '-.~ 'd41d8cd98f00b204 e9800998ecf8427e'

s=: 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq'

(SHA_1 -: sha1sum) s

(SHA_1     s) -: ' '-.~ '84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1'
(SHA_224   s) -: ' '-.~ '75388b16 512776cc 5dba5da1 fd890150 b0c6455c b4f58b19 52522525'
(SHA_256   s) -: ' '-.~ '248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1'
(SHA_384   s) -: ' '-.~ '3391fdddfc8dc739 3707a65b1b470939 7cf8b1d162af05ab fe8f450de5f36bc6 b0455a8520bc4e6f 5fe95b1fe3c8452b'
(SHA_512   s) -: ' '-.~ '204a8fc6dda82f0a 0ced7beb8e08a416 57c16ef468b228a8 279be331a703c335 96fd15c13b1b07f9 aa1d3bea57789ca0 31ad85c7a71dd703 54ec631238ca3445'
(SHA_3_224 s) -: ' '-.~ '8a24108b154ada21 c9fd5574494479ba 5c7e7ab76ef264ea d0fcce33'
(SHA_3_256 s) -: ' '-.~ '41c0dba2a9d62408 49100376a8235e2c 82e1b9998a999e21 db32dd97496d3376'
(SHA_3_384 s) -: ' '-.~ '991c665755eb3a4b 6bbdfb75c78a492e 8c56a22c5c4d7e42 9bfdbc32b9d4ad5a a04a1f076e62fea1 9eef51acd0657c22'
(SHA_3_512 s) -: ' '-.~ '04a371e84ecfb5b8 b77cb48610fca818 2dd457ce6f326a0f d3d7ec2f1e91636d ee691fbe0c985302 ba1b0d8dc78c0863 46b533b49c030d99 a27daf1139d6e75e'
(KECCAK_224 s)-: ' '-.~ 'e51faa2b4655150b 931ee8d700dc202f 763ca5f962c529ea e55012b6'
(KECCAK_256 s)-: ' '-.~ '45d3b367a6904e6e 8d502ee04999a7c2 7647f91fa845d456 525fd352ae3d7371'
(KECCAK_384 s)-: ' '-.~ 'b41e8896428f1bcb b51e17abd6acc980 52a3502e0d5bf7fa 1af949b4d3c855e7 c4dc2c390326b3f3 e74c7b1e2b9a3657'
(KECCAK_512 s)-: ' '-.~ '6aa6d3669597df6d 5a007b00d09c2079 5b5c4218234e1698 a944757a488ecdc0 9965435d97ca32c3 cfed7201ff30e070 cd947f1fc12b9d92 14c467d342bcba5d'
(MD4       s) -: ' '-.~ '4691a9ec81b1a6bd 1ab8557240b245c5'
(MD5       s) -: ' '-.~ '8215ef0796a20bca aae116d3876c664a'

s=: 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu'
(SHA_1 -: sha1sum) s

(SHA_1     s) -: ' '-.~ 'a49b2446 a02c645b f419f995 b6709125 3a04a259'
(SHA_224   s) -: ' '-.~ 'c97ca9a5 59850ce9 7a04a96d ef6d99a9 e0e0e2ab 14e6b8df 265fc0b3'
(SHA_256   s) -: ' '-.~ 'cf5b16a7 78af8380 036ce59e 7b049237 0b249b11 e8f07a51 afac4503 7afee9d1'
(SHA_384   s) -: ' '-.~ '09330c33f71147e8 3d192fc782cd1b47 53111b173b3b05d2 2fa08086e3b0f712 fcc7c71a557e2db9 66c3e9fa91746039'
(SHA_512   s) -: ' '-.~ '8e959b75dae313da 8cf4f72814fc143f 8f7779c6eb9f7fa1 7299aeadb6889018 501d289e4900f7e4 331b99dec4b5433a c7d329eeb6dd2654 5e96e55b874be909'
(SHA_3_224 s) -: ' '-.~ '543e6868e1666c1a 643630df77367ae5 a62a85070a51c14c bf665cbc'
(SHA_3_256 s) -: ' '-.~ '916f6061fe879741 ca6469b43971dfdb 28b1a32dc36cb325 4e812be27aad1d18'
(SHA_3_384 s) -: ' '-.~ '79407d3b5916b59c 3e30b09822974791 c313fb9ecc849e40 6f23592d04f625dc 8c709b98b43b3852 b337216179aa7fc7'
(SHA_3_512 s) -: ' '-.~ 'afebb2ef542e6579 c50cad06d2e578f9 f8dd6881d7dc824d 26360feebf18a4fa 73e3261122948efc fd492e74e82e2189 ed0fb440d187f382 270cb455f21dd185'
(KECCAK_224 s)-: ' '-.~ '344298994b1b0687 3eae2ce739c425c4 7291a2e24189e01b 524f88dc'
(KECCAK_256 s)-: ' '-.~ 'f519747ed599024f 3882238e5ab43960 132572b7345fbeb9 a90769dafd21ad67'
(KECCAK_384 s)-: ' '-.~ 'cc063f3468513536 8b34f7449108f6d1 0fa727b09d696ec5 331771da46a923b6 c34dbd1d4f77e595 689c1f3800681c28'
(KECCAK_512 s)-: ' '-.~ 'ac2fb35251825d3a a48468a9948c0a91 b8256f6d97d8fa41 60faff2dd9dfcc24 f3f1db7a983dad13 d53439ccac0b37e2 4037e7b95f80f59f 37a2f683c4ba4682'
(MD4       s) -: ' '-.~ '2102d1d94bd58ebf 5aa25c305bb783ad'
(MD5       s) -: ' '-.~ '03dd8807a93175fb 062dfb55dc7d359c'

s=: 1e6#'a'

(SHA_1     s) -: ' '-.~ '34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f'
(SHA_224   s) -: ' '-.~ '20794655 980c91d8 bbb4c1ea 97618a4b f03f4258 1948b2ee 4ee7ad67'
(SHA_256   s) -: ' '-.~ 'cdc76e5c 9914fb92 81a1c7e2 84d73e67 f1809a48 a497200e 046d39cc c7112cd0'
(SHA_384   s) -: ' '-.~ '9d0e1809716474cb 086e834e310a4a1c ed149e9c00f24852 7972cec5704c2a5b 07b8b3dc38ecc4eb ae97ddd87f3d8985'
(SHA_512   s) -: ' '-.~ 'e718483d0ce76964 4e2e42c7bc15b463 8e1f98b13b204428 5632a803afa973eb de0ff244877ea60a 4cb0432ce577c31b eb009c5c2c49aa2e 4eadb217ad8cc09b'
(SHA_3_224 s) -: ' '-.~ 'd69335b93325192e 516a912e6d19a15c b51c6ed5c15243e7 a7fd653c'
(SHA_3_256 s) -: ' '-.~ '5c8875ae474a3634 ba4fd55ec85bffd6 61f32aca75c6d699 d0cdcb6c115891c1'
(SHA_3_384 s) -: ' '-.~ 'eee9e24d78c18553 37983451df97c8ad 9eedf256c6334f8e 948d252d5e0e7684 7aa0774ddb90a842 190d2c558b4b8340'
(SHA_3_512 s) -: ' '-.~ '3c3a876da14034ab 60627c077bb98f7e 120a2a5370212dff b3385a18d4f38859 ed311d0a9d5141ce 9cc5c66ee689b266 a8aa18ace8282a0e 0db596c90b0a7b87'
(KECCAK_224 s)-: ' '-.~ '19f9167be2a04c43 abd0ed554788101b 9c339031acc8e146 8531303f'
(KECCAK_256 s)-: ' '-.~ 'fadae6b49f129bbb 812be8407b7b2894 f34aecf6dbd1f9b0 f0c7e9853098fc96'
(KECCAK_384 s)-: ' '-.~ '0c8324e1ebc18282 2c5e2a086cac07c2 fe00e3bce61d01ba 8ad6b71780e2dec5 fb89e5ae90cb593e 57bc6258fdd94e17'
(KECCAK_512 s)-: ' '-.~ '5cf53f2e556be5a6 24425ede23d0e8b2 c7814b4ba0e4e09c bbf3c2fac7056f61 e048fc341262875e bc58a5183fea6514 47124370c1ebf4d6 c89bc9a7731063bb'
(MD4       s) -: ' '-.~ 'bbce80cc6bb65e5c 6745e30d4eeca9a4'
(MD5       s) -: ' '-.~ '7707d6ae4e027c70 eea2a935c2296f21'

NB. 2^33 bits (1 GB)
NB. s=: ,16777216#,:'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno'

NB. (SHA_1     s) -: ' '-.~ '7789f0c9 ef7bfc40 d9331114 3dfbe69e 2017f592'
NB. (SHA_224   s) -: ' '-.~ 'b5989713 ca4fe47a 009f8621 980b34e6 d63ed306 3b2a0a2c 867d8a85'
NB. (SHA_256   s) -: ' '-.~ '50e72a0e 26442fe2 552dc393 8ac58658 228c0cbf b1d2ca87 2ae43526 6fcd055e'
NB. (SHA_384   s) -: ' '-.~ '5441235cc0235341 ed806a64fb354742 b5e5c02a3c5cb71b 5f63fb793458d8fd ae599c8cd8884943 c04f11b31b89f023'
NB. (SHA_512   s) -: ' '-.~ 'b47c933421ea2db1 49ad6e10fce6c7f9 3d0752380180ffd7 f4629a712134831d 77be6091b819ed35 2c2967a2e2d4fa50 50723c9630691f1a 05a7281dbe6c1086'
NB. (SHA_3_224 s) -: ' '-.~ 'c6d66e77ae289566 afb2ce39277752d6 da2a3c46010f1e0a 0970ff60'
NB. (SHA_3_256 s) -: ' '-.~ 'ecbbc42cbf296603 acb2c6bc0410ef43 78bafb24b710357f 12df607758b33e2b'
NB. (SHA_3_384 s) -: ' '-.~ 'a04296f4fcaae148 71bb5ad33e28dcf6 9238b04204d9941b 8782e816d014bcb7 540e4af54f30d578 f1a1ca2930847a12'
NB. (SHA_3_512 s) -: ' '-.~ '235ffd53504ef836 a1342b488f483b39 6eabbfe642cf78ee 0d31feec788b23d0 d18d5c339550dd59 58a500d4b95363da 1b5fa18affc1bab2 292dc63b7d85097c'

tobin=: _2&((a. {~ 0&".)@:('16b'&,)\)

SHA_1=: _1&(128!:6)
SHA_224=: _2&(128!:6)
SHA_256=: _3&(128!:6)
SHA_384=: _4&(128!:6)
SHA_512=: _5&(128!:6)
SHA_3_224=: _6&(128!:6)
SHA_3_256=: _7&(128!:6)
SHA_3_384=: _8&(128!:6)
SHA_3_512=: _9&(128!:6)
KECCAK_224=: _10&(128!:6)
KECCAK_256=: _11&(128!:6)
KECCAK_384=: _12&(128!:6)
KECCAK_512=: _13&(128!:6)
MD4=: _14&(128!:6)
MD5=: _15&(128!:6)

s=: 'abc'

(SHA_1     s) -: tobin ' '-.~ 'a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d'
(SHA_224   s) -: tobin ' '-.~ '23097d22 3405d822 8642a477 bda255b3 2aadbce4 bda0b3f7 e36c9da7'
(SHA_256   s) -: tobin ' '-.~ 'ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad'
(SHA_384   s) -: tobin ' '-.~ 'cb00753f45a35e8b b5a03d699ac65007 272c32ab0eded163 1a8b605a43ff5bed 8086072ba1e7cc23 58baeca134c825a7'
(SHA_512   s) -: tobin ' '-.~ 'ddaf35a193617aba cc417349ae204131 12e6fa4e89a97ea2 0a9eeee64b55d39a 2192992a274fc1a8 36ba3c23a3feebbd 454d4423643ce80e 2a9ac94fa54ca49f'
(SHA_3_224 s) -: tobin ' '-.~ 'e642824c3f8cf24a d09234ee7d3c766f c9a3a5168d0c94ad 73b46fdf'
(SHA_3_256 s) -: tobin ' '-.~ '3a985da74fe225b2 045c172d6bd390bd 855f086e3e9d525b 46bfe24511431532'
(SHA_3_384 s) -: tobin ' '-.~ 'ec01498288516fc9 26459f58e2c6ad8d f9b473cb0fc08c25 96da7cf0e49be4b2 98d88cea927ac7f5 39f1edf228376d25'
(SHA_3_512 s) -: tobin ' '-.~ 'b751850b1a57168a 5693cd924b6b096e 08f621827444f70d 884f5d0240d2712e 10e116e9192af3c9 1a7ec57647e39340 57340b4cf408d5a5 6592f8274eec53f0'
(KECCAK_224 s)-: tobin ' '-.~ 'c30411768506ebe1 c2871b1ee2e87d38 df342317300a9b97 a95ec6a8'
(KECCAK_256 s)-: tobin ' '-.~ '4e03657aea45a94f c7d47ba826c8d667 c0d1e6e33a64a036 ec44f58fa12d6c45'
(KECCAK_384 s)-: tobin ' '-.~ 'f7df1165f033337b e098e7d288ad6a2f 74409d7a60b49c36 642218de161b1f99 f8c681e4afaf31a3 4db29fb763e3c28e'
(KECCAK_512 s)-: tobin ' '-.~ '18587dc2ea106b9a 1563e32b3312421c a164c7f1f07bc922 a9c83d77cea3a1e5 d0c6991073902537 2dc14ac964262937 9540c17e2a65b19d77aa511a9d00bb96'
(MD4       s) -: tobin ' '-.~ 'a448017aaf21d852 5fc10ae87aa6729d'
(MD5       s) -: tobin ' '-.~ '900150983cd24fb0 d6963f7d28e17f72'

s=: ''

(SHA_1     s) -: tobin ' '-.~ 'da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709'
(SHA_224   s) -: tobin ' '-.~ 'd14a028c 2a3a2bc9 476102bb 288234c4 15a2b01f 828ea62a c5b3e42f'
(SHA_256   s) -: tobin ' '-.~ 'e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855'
(SHA_384   s) -: tobin ' '-.~ '38b060a751ac9638 4cd9327eb1b1e36a 21fdb71114be0743 4c0cc7bf63f6e1da 274edebfe76f65fb d51ad2f14898b95b'
(SHA_512   s) -: tobin ' '-.~ 'cf83e1357eefb8bd f1542850d66d8007 d620e4050b5715dc 83f4a921d36ce9ce 47d0d13c5d85f2b0 ff8318d2877eec2f 63b931bd47417a81 a538327af927da3e'
(SHA_3_224 s) -: tobin ' '-.~ '6b4e03423667dbb7 3b6e15454f0eb1ab d4597f9a1b078e3f 5b5a6bc7'
(SHA_3_256 s) -: tobin ' '-.~ 'a7ffc6f8bf1ed766 51c14756a061d662 f580ff4de43b49fa 82d80a4b80f8434a'
(SHA_3_384 s) -: tobin ' '-.~ '0c63a75b845e4f7d 01107d852e4c2485 c51a50aaaa94fc61 995e71bbee983a2a c3713831264adb47 fb6bd1e058d5f004'
(SHA_3_512 s) -: tobin ' '-.~ 'a69f73cca23a9ac5 c8b567dc185a756e 97c982164fe25859 e0d1dcc1475c80a6 15b2123af1f5f94c 11e3e9402c3ac558 f500199d95b6d3e3 01758586281dcd26'
(KECCAK_224 s)-: tobin ' '-.~ 'f71837502ba8e108 37bdd8d365adb855 91895602fc552b48 b7390abd'
(KECCAK_256 s)-: tobin ' '-.~ 'c5d2460186f7233c 927e7db2dcc703c0 e500b653ca82273b 7bfad8045d85a470'
(KECCAK_384 s)-: tobin ' '-.~ '2c23146a63a29acf 99e73b88f8c24eaa 7dc60aa771780ccc 006afbfa8fe2479b 2dd2b21362337441 ac12b515911957ff'
(KECCAK_512 s)-: tobin ' '-.~ '0eab42de4c3ceb92 35fc91acffe746b2 9c29a8c366b7c60e 4e67c466f36a4304 c00fa9caf9d87976 ba469bcbe06713b4 35f091ef2769fb16 0cdab33d3670680e'
(MD4       s) -: tobin ' '-.~ '31d6cfe0d16ae931 b73c59d7e0c089c0'
(MD5       s) -: tobin ' '-.~ 'd41d8cd98f00b204 e9800998ecf8427e'

s=: 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq'

(SHA_1     s) -: tobin ' '-.~ '84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1'
(SHA_224   s) -: tobin ' '-.~ '75388b16 512776cc 5dba5da1 fd890150 b0c6455c b4f58b19 52522525'
(SHA_256   s) -: tobin ' '-.~ '248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1'
(SHA_384   s) -: tobin ' '-.~ '3391fdddfc8dc739 3707a65b1b470939 7cf8b1d162af05ab fe8f450de5f36bc6 b0455a8520bc4e6f 5fe95b1fe3c8452b'
(SHA_512   s) -: tobin ' '-.~ '204a8fc6dda82f0a 0ced7beb8e08a416 57c16ef468b228a8 279be331a703c335 96fd15c13b1b07f9 aa1d3bea57789ca0 31ad85c7a71dd703 54ec631238ca3445'
(SHA_3_224 s) -: tobin ' '-.~ '8a24108b154ada21 c9fd5574494479ba 5c7e7ab76ef264ea d0fcce33'
(SHA_3_256 s) -: tobin ' '-.~ '41c0dba2a9d62408 49100376a8235e2c 82e1b9998a999e21 db32dd97496d3376'
(SHA_3_384 s) -: tobin ' '-.~ '991c665755eb3a4b 6bbdfb75c78a492e 8c56a22c5c4d7e42 9bfdbc32b9d4ad5a a04a1f076e62fea1 9eef51acd0657c22'
(SHA_3_512 s) -: tobin ' '-.~ '04a371e84ecfb5b8 b77cb48610fca818 2dd457ce6f326a0f d3d7ec2f1e91636d ee691fbe0c985302 ba1b0d8dc78c0863 46b533b49c030d99 a27daf1139d6e75e'
(KECCAK_224 s)-: tobin ' '-.~ 'e51faa2b4655150b 931ee8d700dc202f 763ca5f962c529ea e55012b6'
(KECCAK_256 s)-: tobin ' '-.~ '45d3b367a6904e6e 8d502ee04999a7c2 7647f91fa845d456 525fd352ae3d7371'
(KECCAK_384 s)-: tobin ' '-.~ 'b41e8896428f1bcb b51e17abd6acc980 52a3502e0d5bf7fa 1af949b4d3c855e7 c4dc2c390326b3f3 e74c7b1e2b9a3657'
(KECCAK_512 s)-: tobin ' '-.~ '6aa6d3669597df6d 5a007b00d09c2079 5b5c4218234e1698 a944757a488ecdc0 9965435d97ca32c3 cfed7201ff30e070 cd947f1fc12b9d92 14c467d342bcba5d'
(MD4       s) -: tobin ' '-.~ '4691a9ec81b1a6bd 1ab8557240b245c5'
(MD5       s) -: tobin ' '-.~ '8215ef0796a20bca aae116d3876c664a'

s=: 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu'

(SHA_1     s) -: tobin ' '-.~ 'a49b2446 a02c645b f419f995 b6709125 3a04a259'
(SHA_224   s) -: tobin ' '-.~ 'c97ca9a5 59850ce9 7a04a96d ef6d99a9 e0e0e2ab 14e6b8df 265fc0b3'
(SHA_256   s) -: tobin ' '-.~ 'cf5b16a7 78af8380 036ce59e 7b049237 0b249b11 e8f07a51 afac4503 7afee9d1'
(SHA_384   s) -: tobin ' '-.~ '09330c33f71147e8 3d192fc782cd1b47 53111b173b3b05d2 2fa08086e3b0f712 fcc7c71a557e2db9 66c3e9fa91746039'
(SHA_512   s) -: tobin ' '-.~ '8e959b75dae313da 8cf4f72814fc143f 8f7779c6eb9f7fa1 7299aeadb6889018 501d289e4900f7e4 331b99dec4b5433a c7d329eeb6dd2654 5e96e55b874be909'
(SHA_3_224 s) -: tobin ' '-.~ '543e6868e1666c1a 643630df77367ae5 a62a85070a51c14c bf665cbc'
(SHA_3_256 s) -: tobin ' '-.~ '916f6061fe879741 ca6469b43971dfdb 28b1a32dc36cb325 4e812be27aad1d18'
(SHA_3_384 s) -: tobin ' '-.~ '79407d3b5916b59c 3e30b09822974791 c313fb9ecc849e40 6f23592d04f625dc 8c709b98b43b3852 b337216179aa7fc7'
(SHA_3_512 s) -: tobin ' '-.~ 'afebb2ef542e6579 c50cad06d2e578f9 f8dd6881d7dc824d 26360feebf18a4fa 73e3261122948efc fd492e74e82e2189 ed0fb440d187f382 270cb455f21dd185'
(KECCAK_224 s)-: tobin ' '-.~ '344298994b1b0687 3eae2ce739c425c4 7291a2e24189e01b 524f88dc'
(KECCAK_256 s)-: tobin ' '-.~ 'f519747ed599024f 3882238e5ab43960 132572b7345fbeb9 a90769dafd21ad67'
(KECCAK_384 s)-: tobin ' '-.~ 'cc063f3468513536 8b34f7449108f6d1 0fa727b09d696ec5 331771da46a923b6 c34dbd1d4f77e595 689c1f3800681c28'
(KECCAK_512 s)-: tobin ' '-.~ 'ac2fb35251825d3a a48468a9948c0a91 b8256f6d97d8fa41 60faff2dd9dfcc24 f3f1db7a983dad13 d53439ccac0b37e2 4037e7b95f80f59f 37a2f683c4ba4682'
(MD4       s) -: tobin ' '-.~ '2102d1d94bd58ebf 5aa25c305bb783ad'
(MD5       s) -: tobin ' '-.~ '03dd8807a93175fb 062dfb55dc7d359c'

s=: 1e6#'a'

(SHA_1     s) -: tobin ' '-.~ '34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f'
(SHA_224   s) -: tobin ' '-.~ '20794655 980c91d8 bbb4c1ea 97618a4b f03f4258 1948b2ee 4ee7ad67'
(SHA_256   s) -: tobin ' '-.~ 'cdc76e5c 9914fb92 81a1c7e2 84d73e67 f1809a48 a497200e 046d39cc c7112cd0'
(SHA_384   s) -: tobin ' '-.~ '9d0e1809716474cb 086e834e310a4a1c ed149e9c00f24852 7972cec5704c2a5b 07b8b3dc38ecc4eb ae97ddd87f3d8985'
(SHA_512   s) -: tobin ' '-.~ 'e718483d0ce76964 4e2e42c7bc15b463 8e1f98b13b204428 5632a803afa973eb de0ff244877ea60a 4cb0432ce577c31b eb009c5c2c49aa2e 4eadb217ad8cc09b'
(SHA_3_224 s) -: tobin ' '-.~ 'd69335b93325192e 516a912e6d19a15c b51c6ed5c15243e7 a7fd653c'
(SHA_3_256 s) -: tobin ' '-.~ '5c8875ae474a3634 ba4fd55ec85bffd6 61f32aca75c6d699 d0cdcb6c115891c1'
(SHA_3_384 s) -: tobin ' '-.~ 'eee9e24d78c18553 37983451df97c8ad 9eedf256c6334f8e 948d252d5e0e7684 7aa0774ddb90a842 190d2c558b4b8340'
(SHA_3_512 s) -: tobin ' '-.~ '3c3a876da14034ab 60627c077bb98f7e 120a2a5370212dff b3385a18d4f38859 ed311d0a9d5141ce 9cc5c66ee689b266 a8aa18ace8282a0e 0db596c90b0a7b87'
(KECCAK_224 s)-: tobin ' '-.~ '19f9167be2a04c43 abd0ed554788101b 9c339031acc8e146 8531303f'
(KECCAK_256 s)-: tobin ' '-.~ 'fadae6b49f129bbb 812be8407b7b2894 f34aecf6dbd1f9b0 f0c7e9853098fc96'
(KECCAK_384 s)-: tobin ' '-.~ '0c8324e1ebc18282 2c5e2a086cac07c2 fe00e3bce61d01ba 8ad6b71780e2dec5 fb89e5ae90cb593e 57bc6258fdd94e17'
(KECCAK_512 s)-: tobin ' '-.~ '5cf53f2e556be5a6 24425ede23d0e8b2 c7814b4ba0e4e09c bbf3c2fac7056f61 e048fc341262875e bc58a5183fea6514 47124370c1ebf4d6 c89bc9a7731063bb'
(MD4       s) -: tobin ' '-.~ 'bbce80cc6bb65e5c 6745e30d4eeca9a4'
(MD5       s) -: tobin ' '-.~ '7707d6ae4e027c70 eea2a935c2296f21'

NB. test non-accelerated

SHA_1=: (128!:6)
SHA_224=: 2&(128!:6)
SHA_256=: 3&(128!:6)
SHA_384=: 4&(128!:6)
SHA_512=: 5&(128!:6)

test_1=: 3 : 0
s=: 'abc'
assert. (SHA_1 s) -: ' '-.~ 'a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d'

s=: ''
assert. (SHA_1 s) -: ' '-.~ 'da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709'

s=: 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq'
assert. (SHA_1 s) -: ' '-.~ '84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1'

s=: 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu'
assert. (SHA_1 s) -: ' '-.~ 'a49b2446 a02c645b f419f995 b6709125 3a04a259'

s=: 1e6#'a'
assert. (SHA_1 s) -: ' '-.~ '34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f'

1
)

3 : 0''
if.('arm64'-:9!:56'cpu') *. 9!:56'sha1' do.
  0 (9!:56) 'sha1'
  test_1''
  1 (9!:56) 'sha1'
elseif. ('x86_64'-:9!:56'cpu') *. 9!:56'sha_ni' do.
  0 (9!:56) 'sha_ni'
  test_1''
  0 (9!:56) 'avx'
  test_1''
  0 (9!:56) 'ssse3'
  test_1''
  1 (9!:56) 'sha_ni'
  1 (9!:56) 'avx'
  1 (9!:56) 'ssse3'
elseif. ('x86_64'-:9!:56'cpu') *. 9!:56'avx' do.
  0 (9!:56) 'avx'
  test_1''
  0 (9!:56) 'ssse3'
  test_1''
  1 (9!:56) 'avx'
  1 (9!:56) 'ssse3'
elseif. ('x86_64'-:9!:56'cpu') *. 9!:56'ssse3' do.
  0 (9!:56) 'ssse3'
  test_1''
  1 (9!:56) 'ssse3'
end.
1
)

test_2=: 3 : 0
s=: 'abc'
assert. (SHA_224 s) -: ' '-.~ '23097d22 3405d822 8642a477 bda255b3 2aadbce4 bda0b3f7 e36c9da7'
assert. (SHA_256 s) -: ' '-.~ 'ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad'
assert. (SHA_384 s) -: ' '-.~ 'cb00753f45a35e8b b5a03d699ac65007 272c32ab0eded163 1a8b605a43ff5bed 8086072ba1e7cc23 58baeca134c825a7'
assert. (SHA_512 s) -: ' '-.~ 'ddaf35a193617aba cc417349ae204131 12e6fa4e89a97ea2 0a9eeee64b55d39a 2192992a274fc1a8 36ba3c23a3feebbd 454d4423643ce80e 2a9ac94fa54ca49f'

s=: ''
assert. (SHA_224 s) -: ' '-.~ 'd14a028c 2a3a2bc9 476102bb 288234c4 15a2b01f 828ea62a c5b3e42f'
assert. (SHA_256 s) -: ' '-.~ 'e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855'
assert. (SHA_384 s) -: ' '-.~ '38b060a751ac9638 4cd9327eb1b1e36a 21fdb71114be0743 4c0cc7bf63f6e1da 274edebfe76f65fb d51ad2f14898b95b'
assert. (SHA_512 s) -: ' '-.~ 'cf83e1357eefb8bd f1542850d66d8007 d620e4050b5715dc 83f4a921d36ce9ce 47d0d13c5d85f2b0 ff8318d2877eec2f 63b931bd47417a81 a538327af927da3e'

s=: 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq'
assert. (SHA_224 s) -: ' '-.~ '75388b16 512776cc 5dba5da1 fd890150 b0c6455c b4f58b19 52522525'
assert. (SHA_256 s) -: ' '-.~ '248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1'
assert. (SHA_384 s) -: ' '-.~ '3391fdddfc8dc739 3707a65b1b470939 7cf8b1d162af05ab fe8f450de5f36bc6 b0455a8520bc4e6f 5fe95b1fe3c8452b'
assert. (SHA_512 s) -: ' '-.~ '204a8fc6dda82f0a 0ced7beb8e08a416 57c16ef468b228a8 279be331a703c335 96fd15c13b1b07f9 aa1d3bea57789ca0 31ad85c7a71dd703 54ec631238ca3445'

s=: 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu'
assert. (SHA_224 s) -: ' '-.~ 'c97ca9a5 59850ce9 7a04a96d ef6d99a9 e0e0e2ab 14e6b8df 265fc0b3'
assert. (SHA_256 s) -: ' '-.~ 'cf5b16a7 78af8380 036ce59e 7b049237 0b249b11 e8f07a51 afac4503 7afee9d1'
assert. (SHA_384 s) -: ' '-.~ '09330c33f71147e8 3d192fc782cd1b47 53111b173b3b05d2 2fa08086e3b0f712 fcc7c71a557e2db9 66c3e9fa91746039'
assert. (SHA_512 s) -: ' '-.~ '8e959b75dae313da 8cf4f72814fc143f 8f7779c6eb9f7fa1 7299aeadb6889018 501d289e4900f7e4 331b99dec4b5433a c7d329eeb6dd2654 5e96e55b874be909'

s=: 1e6#'a'
assert. (SHA_224 s) -: ' '-.~ '20794655 980c91d8 bbb4c1ea 97618a4b f03f4258 1948b2ee 4ee7ad67'
assert. (SHA_256 s) -: ' '-.~ 'cdc76e5c 9914fb92 81a1c7e2 84d73e67 f1809a48 a497200e 046d39cc c7112cd0'
assert. (SHA_384 s) -: ' '-.~ '9d0e1809716474cb 086e834e310a4a1c ed149e9c00f24852 7972cec5704c2a5b 07b8b3dc38ecc4eb ae97ddd87f3d8985'
assert. (SHA_512 s) -: ' '-.~ 'e718483d0ce76964 4e2e42c7bc15b463 8e1f98b13b204428 5632a803afa973eb de0ff244877ea60a 4cb0432ce577c31b eb009c5c2c49aa2e 4eadb217ad8cc09b'

1
)

3 : 0''
if.('arm64'-:9!:56'cpu') *. 9!:56'sha2' do.
  0 (9!:56) 'sha2'
  test_2''
  1 (9!:56) 'sha2'
elseif.('x86_64'-:9!:56'cpu') *. 9!:56'avx2' do.
  0 (9!:56) 'avx2'
  test_2''
  0 (9!:56) 'avx'
  test_2''
  0 (9!:56) 'sse3'
  test_2''
  1 (9!:56) 'avx2'
  1 (9!:56) 'avx'
  1 (9!:56) 'sse3'
elseif.('x86_64'-:9!:56'cpu') *. 9!:56'avx' do.
  0 (9!:56) 'avx'
  test_2''
  0 (9!:56) 'sse3'
  test_2''
  1 (9!:56) 'avx'
  1 (9!:56) 'sse3'
elseif.('x86_64'-:9!:56'cpu') *. 9!:56'sse3' do.
  0 (9!:56) 'sse3'
  test_2''
  1 (9!:56) 'sse3'
end.
1
)

f=: 128!:6

'domain error' -: f etx 2 3 4
'domain error' -: f etx 2 3 4x
'domain error' -: f etx 2 3.4
'domain error' -: f etx 2 3j4
'domain error' -: f etx 2 3r4
'domain error' -: f etx 2 3;4
'domain error' -: f etx s:@<"0 'abc'
'domain error' -: f etx u: 'abc'
'domain error' -: f etx 10 u: 'abc'

(f -: f"1 0 1"1 0 1) 3 4$'abc'
(,1)  (f -: f"1 0 1"1 0 1)  'xyz'
1 2 (f -: f"1 0 1"1 0 1)  'xyz'



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
'domain error' -: (<'abc')      f etx 'xyz'
'domain error' -: (<u:'abc')    f etx 'xyz'
'domain error' -: (<10&u:'abc') f etx 'xyz'
'domain error' -: (34;'a')      f etx 'xyz'
'domain error' -: (34;56;3)     f etx 'xyz'

4!:55 ;:'s f tobin SHA_1 SHA_224 SHA_256 SHA_384 SHA_512 SHA_3_224 SHA_3_256 SHA_3_384 SHA_3_512 KECCAK_224 KECCAK_256 KECCAK_384 KECCAK_512 MD4 MD5'
4!:55 ;:'sha1sum sha1sum_process sha1sum_step test_1 test_2'

epilog''

