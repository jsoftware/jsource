NB. DLL call ------------------------------------------------------------

load'dll'

lib=: LIBTSDLL

dcd=: 4 : '(lib,x) cd y'

NB. test integer types
a=: 4 u: +/401 402 403
b=: 4 u: 402 403
a1=: 10&u: +/401 402 403
b1=: 10&u: 402 403
('&';(,'&');'a';'bc')=   'cbasic c *c c *c' dcd (,'a');'a';'bc'
(a;(,a);(4 u: 401);b)=   'wbasic w *w w *w' dcd (,4 u: 400);(4 u: 401);4 u: 402 403
(a1;(,a1);(10&u: 401);b)='ubasic u *u u *u' dcd (,10&u: 400);(10&u: 401);10&u: 402 403
(9;(,9);2;3 4)=          'sbasic s *s s *s' dcd (,2);2;3 4
(9;(,9);2;1 ic 3 4)=     'sbasic s *s s *s' dcd (,2);2;1 ic 3 4  NB. shorts in chars
(9;(,9);2;3 4)=          'ibasic i *i i *i' dcd (,2);2;3 4
(9;(,.9);2;,.3 4)=       'ibasic i *i i *i' dcd (,.2);2;,.3 4 NB. allow rank>1
(9;(,9);2;3 4)=          'xbasic x *x x *x' dcd (,2);2;3 4
(2;(,2);1;0 1)=          'ibasic i *i i *i' dcd (,1);1;0 1 NB. boolean promotion to int

NB. declaration (left argument) and parameter (right argument) checking
'limit error'  -: (lib,'ibasic i *i i *i',2300$' '  ) cd etx (,2);2;3 4
'limit error'  -: ((2300$' '),lib,'ibasic i *i i *i') cd etx (,2);2;3 4

(1 0 -: cder '') *. 'domain error' -: ((1200$'x'),' proc i i i') cd etx 2;3
(1 0 -: cder '') *. 'domain error' -: ('xxxx proc i i i'       ) cd etx 2;3
(1 0 -: cder '') *. 'domain error' -: ('xxxx proc i i i'       ) cd etx 2;3

(2 0 -: cder '') *. 'domain error' -: (lib,(1200$'x'),' i i i' ) cd etx 2;3
(2 0 -: cder '') *. 'domain error' -: (lib,'xxxx i i i'        ) cd etx 2;3
(2 0 -: cder '') *. 'domain error' -: (lib,'xxxx i i i'        ) cd etx 2;3

(4 0 -: cder '') *. 'domain error' -: (lib,'ibasic ',400$'i '  ) cd etx 2;3
(4 0 -: cder '') *. 'domain error' -: 'ibasic i *i i *i' dcd etx (,2);2;3 4;'abcd'
(4 0 -: cder '') *. 'domain error' -: 'ibasic i *i i *i' dcd etx (,2);2

(5 0 -: cder '') *. 'domain error' -: 'ibasic I *i i *i' dcd etx (,2);2;3 4
(5 1 -: cder '') *. 'domain error' -: 'ibasic i *I i *i' dcd etx (,2);2;3 4
(5 2 -: cder '') *. 'domain error' -: 'ibasic i *i I *i' dcd etx (,2);2;3 4
(5 3 -: cder '') *. 'domain error' -: 'ibasic i *i i *I' dcd etx (,2);2;3 4

(6 0 -: cder '') *. 'domain error' -: 'ibasic i *i i *i' dcd etx ('abc');2  ;3 4
(6 1 -: cder '') *. 'domain error' -: 'ibasic i *i i *i' dcd etx (,2)   ;4.5;3 4
(6 2 -: cder '') *. 'domain error' -: 'ibasic i *i i *i' dcd etx (,2)   ;2  ;3j4 5

NB. 'rank error' -: 'ibasic i *i i *i' dcd etx ,:(,2);2;3 4
NB. 'rank error' -: 'ibasic i *i i *i' dcd etx ,.(,2);2;3 4

NB. mema memory
add=: mema 2*IF64{4 8
3 4 memw add,0,2,JINT
(9;(,9);2;<<add)=         'xbasic x *x x *x' dcd (,2);2;<<add
0=memf add

NB. l type is same as x on J64 and and error on J32
3 : 0''
if. IF64 do.
 assert. (9;(,9);2;3 4) =  'xbasic l *l l *l' dcd (,2);2;3 4
else.
 assert. 'domain error'-:  'xbasic l *l l *l' dcd etx (,2);2;3 4 
 assert. 5 0 -: cder ''  NB. error 5, result/arg declaration 0
end.
)

NB. pointer result
address=. 0{::'pc *c' dcd ''
'test'-:memr address,0,_1
address=. 0{::'pc *' dcd ''
'test'-:memr address,0,_1

NB. use of > parameter
24        -: 'altinci >+ i i' dcd 23
(>:x)     -: 'altinci >+ i i' dcd ,. x=: 17 ?@$ 1e6
(>:x)     -: 'altinci >+ i i' dcd ,. <"0 x




f=: 3 : 0
 if. (9!:12 '') e. 6 do.  NB. do only under Windows
  'ole32.dll CoCreateGuid i *c' 15!:0 y
 else.
  0;16$' '
 end.
)

('';,16) = $&.> x=: f  <16$' '
('';,16) = $&.> x=: f ,<16$' '


NB. 0 procaddress
xbasic_add=: ":>{.'xbasic_add x' dcd ''

NB. alternate ways to get proc address
NB. 0 ~: f =: 15!:20 }:lib
NB. (f 15!:21 'xbasic') = 15!:21 lib,'xbasic'
NB. xbasic_add -: ": 15!:21 lib,'xbasic'

(9;(,9);2;3 4) = ('0 ',xbasic_add,' x *x x *x') cd (,2);2;3 4

(2 0 -: cder '') *. 'domain error' -: '0  1e4 x x' cd etx (,2);2;3 4
(2 0 -: cder '') *. 'domain error' -: '0 _1e4 x x' cd etx (,2);2;3 4
(2 0 -: cder '') *. 'domain error' -: '0 abc  x x' cd etx (,2);2;3 4
(2 0 -: cder '') *. 'domain error' -: '0 34aa x x' cd etx (,2);2;3 4

(2 0 -: cder '') *. 'domain error' -: ('0  ',(>IF64{'2333444555';19$'93'),' x x') cd etx (,2);2;3 4
(2 0 -: cder '') *. 'domain error' -: ('0 _',(>IF64{'2333444555';19$'93'),' x x') cd etx (,2);2;3 4


4!:55 ;:'a a1 add address b b1 dcd f lib obj_add pc s0 s1 td td1a td3 td4 tf tf3'
4!:55 ;:'v0 v1 v2 v3 v4 v5 x xbasic_add xx yy z'

