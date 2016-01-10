NB. x;:y ----------------------------------------------------------------

NB. example 0: English words
NB. rows are sp A; cols are sp A

me=: (i.#a.) e. (a.i.''''),,(a.i.'Aa')+/i.26
se=: 2 2 2 $ 0 0 1 1  0 3 1 0

(;:    -: (0;se;me)&;:) y=: 'Now is the time all good men'
(<;._1 -: (0;se;me)&;:) y=: ' fourscore and ten years ago our fathers brought forth on this continent'


NB. example 1: separating quoted strings from non-quoted strings
NB. rows are 0 NQ Q Q1; cols are NQ Q

mq =: ''''=a.
sq =: 4 2 2 $ 1 1 2 1  1 0 2 2  2 0 3 0  1 2 2 0
sqx=: 4 2 2 $ 1 1 2 0  1 0 2 3  2 0 3 0  1 1 2 0

remq =: (+: ~:/\)@(''''&=) # ]
remq1=: 3 : '(,"0 (({:"1 t)e.2 3)#}:"1 t=. (4;sq;mq) ;: y) ;@:(<;.0) y'
remq2=: (1;sqx;mq)&;:

(remq -: remq1) y=: '''Don''''t tread on me!'', he said with some feeling '
(remq -: remq2) y
(remq -: remq2) y=: '''The Power of the Powerless'' by Havel; ''1984'' by Orwell'

(  3&$"0&.>(0;sq ;mq);:y) -: (0;(sq ,"2 ]0);<  3&$"0&.>(a.-.'''');'''');:  3&$"0 y
(2 3&$"0&.>(0;sq ;mq);:y) -: (0;(sq ,"2 ]0);<2 3&$"0&.>(a.-.'''');'''');:2 3&$"0 y

(  3&$"0   (1;sqx;mq);:y) -: (1;(sqx,"2 ]0);<  3&$"0&.>(a.-.'''');'''');:  3&$"0 y
(2 3&$"0   (1;sqx;mq);:y) -: (1;(sqx,"2 ]0);<2 3&$"0&.>(a.-.'''');'''');:2 3&$"0 y

t=: (1;sqx;mq) ;: 'a''',1e6$'x'
t -: ,'a'
1000 > 7!:5 <'t'


NB. example 2: names and numbers with vector notation

mv=: (#a.)$0                   NB. X other
mv=: 1 (a.i.'_0123456789')}mv  NB. 9 digits and _
mv=: 2 ((a.i.'Aa')+/i.26)}mv   NB. A A-Z a-z
sv=: 0 3 2$0                 
NB.          X    9    A 
sv=:sv,_2]\ 0 0  1 1  2 1      NB. 0 other
sv=:sv,_2]\ 0 5  1 0  1 0      NB. 1 number
sv=:sv,_2]\ 0 2  2 0  2 0      NB. 2 name


NB. example 3: J sentences

mj=: 256$0                     NB. X other
mj=: 1 (a.i.' ')}mj            NB. S space
mj=: 2 ((a.i.'Aa')+/i.26)}mj   NB. A A-Z a-z excluding N B
mj=: 3 (a.i.'N')}mj            NB. N the letter N
mj=: 4 (a.i.'B')}mj            NB. B the letter B
mj=: 5 (a.i.'0123456789_')}mj  NB. 9 digits and _
mj=: 6 (a.i.'.')}mj            NB. D .
mj=: 7 (a.i.':')}mj            NB. C :
mj=: 8 (a.i.'''')}mj           NB. Q quote
t=. 0 9 2$0 
NB.        X    S    A    N    B    9    D    C    Q
t=.t,_2]\ 1 1  0 0  2 1  3 1  2 1  6 1  1 1  1 1  7 1  NB. 0 space
t=.t,_2]\ 1 2  0 3  2 2  3 2  2 2  6 2  1 0  1 0  7 2  NB. 1 other
t=.t,_2]\ 1 2  0 3  2 0  2 0  2 0  2 0  1 0  1 0  7 2  NB. 2 alphanumeric
t=.t,_2]\ 1 2  0 3  2 0  2 0  4 0  2 0  1 0  1 0  7 2  NB. 3 N
t=.t,_2]\ 1 2  0 3  2 0  2 0  2 0  2 0  5 0  1 0  7 2  NB. 4 NB
t=.t,_2]\ 9 0  9 0  9 0  9 0  9 0  9 0  1 0  1 0  9 0  NB. 5 NB.
t=.t,_2]\ 1 4  0 5  6 0  6 0  6 0  6 0  6 0  1 0  7 4  NB. 6 numeric
t=.t,_2]\ 7 0  7 0  7 0  7 0  7 0  7 0  7 0  7 0  8 0  NB. 7 quote
t=.t,_2]\ 1 2  0 3  2 2  3 2  2 2  6 2  1 2  1 2  7 0  NB. 8 even quotes
t=.t,_2]\ 9 0  9 0  9 0  9 0  9 0  9 0  9 0  9 0  9 0  NB. 9 trailing comment
sj=: t

A=: 'NB' -.~ a.{~,(a.i.'Aa')+/i.26
mj1=: (a.-.;t);t=. ' '; A; 'N'; 'B'; '0123456789_'; '.'; ':'; ''''


NB. Example 4: Detecting 0xABC hex strings (test end-of-input)
NB. m: 3='0', 2='x', 1=nonzero hexdigit, 0=other

mh=: a. e. '0x123456789abcdefABCDEF'
mh=: mh + a. e. '0x'
mh=: mh + a. e. '0'
NB.             ghi  0aA   x    0
sh=:    1 4 2 $ 0 0  0 0  0 0  1 1  NB. awaiting 0
sh=: sh , 4 2 $ 0 0  0 0  2 0  0 0  NB. found 0, awaiting x
sh=: sh , 4 2 $ 0 0  3 0  0 0  3 0  NB. found 0x, awaiting hexdigit
sh=: sh , 4 2 $ 0 3  3 0  0 3  3 0  NB. found hexdigit, awaiting end-of-string

('0x30';'0x40'        ) -:  (0;sh;mh;0 _1 0 0) ;: 'qqq0x30x30x40x0xxxx'
('0x30';'0x40'        ) -:  (0;sh;mh;0 _1 0 0) ;: 'qqq0x30x30x40x0'
('0x30';'0x40'        ) -:  (0;sh;mh;0 _1 0 0) ;: 'qqq0x30x30x40x0x'
('0x30';'0x40';'0x3'  ) -:  (0;sh;mh;0 _1 0 0) ;: 'qqq0x30x30x40x0x3'
('0x30';'0x40';'0x34a') -:  (0;sh;mh;0 _1 0 0) ;: 'qqq0x30x30x40x0x34a'
NB. Using the same machine, test ijr
('0x50' ;'0x0' ) -:  (0;sh;mh;4 _1 0 0) ;: 'qqq0x30x50x40x0xxxx'
('0x30' ;'0x40') -:  (0;sh;mh;4  3 1 0) ;: 'qqq0x30x50x40x0xxxx'
('q0x30';'0x40') -:  (0;sh;mh;4  2 1 0) ;: 'qqq0x30x50x40x0xxxx'


f=: ;:

(<;._1 ' Cogito ergo sum')    -: (0;se;me) f y=: 'Cogito, ergo sum.'
(<;._1 ' Cogito ergo sum')    -: (0;se;me) f y,5$' '
(<;._1 ' Cogito ergo sum')    -: (2;se;me) (,"0@f <;.0 ]) y
(<;._1 ' Cogito ergo sum')    -: (2;se;me) (,"0@f <;.0 ]) y,5$' '
(<;._1 ' Don''t tread on me') -: (0;se;me) f y=: 'Don''t tread on me!'
(<;._1 ' Don''t tread on me') -: (0;se;me) f y,' '
(<;._1 ' Don''t tread on me') -: (2;se;me) (,"0@f <;.0 ]) y
(<;._1 ' Don''t tread on me') -: (2;se;me) (,"0@f <;.0 ]) y,5$' '

testj=: 4 : 0
 assert. x -: (0;sj;mj) f y
 assert. x -: (0;sj;mj) f y,5$' '
 assert. x -: (2;sj;mj) (,"0@f <;.0 ]) y
 assert. x -: (2;sj;mj) (,"0@f <;.0 ]) y,5$' '
 t=: (2;sj;mj) f y
 assert. 0<:t
 assert. t -: /:~ t
 assert. 0<1{"1 t
 assert. (i e. i.#y) *. i-:~.i=. ; <@(+i.)/@}:"1 t
 t=: (3;sj;mj) f y
 assert. 1=#$t
 assert. t e. i.*/}:$sj
 1
)

(,<y) testj y=: '_3.4e_5j_6.7e_8'
(,<y) testj y=: '1 2 _3.4e_5j_6.7e_8 9'
(,<y) testj y=: 'abc'
(,<y) testj y=: 'abc_locale59_'
(,<y) testj y=: 'abc__59'
(,<y) testj y=: '''To quote or not to quote'''
(,<y) testj y=: '''Don''''t tread on me!'''

(<;._1 ' abc _59')         testj y=: 'abc _59'
(<;._1 ';abc;''def'';1 2') testj y=: 'abc ''def'' 1 2'

y=: 'sum=. (i.3 4)+/ .*0j4+pru 4'
x=: <;._1 ';sum;=.;(;i.;3 4;);+;/;.;*;0j4;+;pru;4' 
x testj y


'domain error' -: 0 1 0                 ;: etx 'abcd montegu'
'domain error' -: 'abc'                 ;: etx 'abcd montegu'
'domain error' -: 1 2 3                 ;: etx 'abcd montegu'
'domain error' -: 1 2 3.5               ;: etx 'abcd montegu'
'domain error' -: 1 2 3j5               ;: etx 'abcd montegu'

'domain error' -: (0;se;256$0 3.5)      ;: etx 'abcd montegu'
'domain error' -: (0;se;256$0 3j5)      ;: etx 'abcd montegu'
'domain error' -: (0;se;256$'abc')      ;: etx 'abcd montegu'
'domain error' -: (0;se)                ;: etx 'abcd montegu'
'domain error' -: (0;sh;mh;<<0 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'
'domain error' -: (0;sh;mh;0.5 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'
'domain error' -: (0;sh;mh;0j1 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'
'domain error' -: (0;sh;mh;'a'       )  ;: etx 'qqq0x30x50x40x0xxxx'

'rank error'   -: 0                     ;: etx 'abcd montegu'
'rank error'   -: (,:0;se;' '~:a.)      ;: etx 'abcd montegu'
'rank error'   -: (,:0;(,se);me)        ;: etx 'abcd montegu'
'rank error'   -: (0;(0{se);me)         ;: etx 'abcd montegu'
'rank error'   -: ( 0;sh;mh;,:0 _1 0 0) ;: etx 'qqq0x30x50x40x0xxxx'

'index error'  -: (_1;se;me)            ;: etx 'abcd montegu'
'index error'  -: ( 6;se;me)            ;: etx 'abcd montegu'
'index error'  -: ( 0;se;256$0 _15)     ;: etx 'abcd montegu'
'index error'  -: ( 0;se;256$0,#se)     ;: etx 'abcd montegu'
'index error'  -: ( 0;sh;mh;4 _1 1 0)   ;: etx 'qqq0x30x50x40x0xxxx'  NB. invalid output string
'index error'  -: ( 0;sh;mh;0 _1 0 4)   ;: etx 'qqq0x30x50x40x0xxxx'
'index error'  -: ( 0;sh;mh;19 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'
'index error'  -: ( 0;sh;mh;_1 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'


4!:55 ;:'A f me mh mj mj1 mq mv remq remq1 remq2 se sh sj sq sqx sv'
4!:55 ;:'t testj x y'


