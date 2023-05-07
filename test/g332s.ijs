prolog './g332s.ijs'
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
mv=: 2 (<"0(a.i.'Aa')+/i.26)}mv   NB. A A-Z a-z
sv=: 0 3 2$0                 
NB.          X    9    A 
sv=:sv,_2]\ 0 0  1 1  2 1      NB. 0 other
sv=:sv,_2]\ 0 5  1 0  1 0      NB. 1 number
sv=:sv,_2]\ 0 2  2 0  2 0      NB. 2 name


NB. example 3: J sentences

mj=: 256$0                     NB. X other
mj=: 1 (a.i.' ')}mj            NB. S space
mj=: 2 (<"0(a.i.'Aa')+/i.26)}mj   NB. A A-Z a-z excluding N B
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

NB. Example 5.  This went into a loop when there was a bug in ext() that didn't increase the allo size
f=: (0;(0 10#:10*".;._2]0 :0);(a.e.'ABCDEF0123456789abcdef')+(2*a.='\')+3*a.e.'Uu')&;:
 1.1  1.1  2.1 1.1
 1.0  1.0  2.2 1.0
 3.0  3.0  3.0 4.0
 1.2  1.2  2.2 1.2
 1.2  5.0  2.2 1.2
 1.2  6.0  2.2 1.2
 1.2  7.0  2.2 1.2
 1.2  3.0  2.2 1.2
)
(,<,'a') -: f ,'a'  NB. SYSTEM HANG !!!!

f=: (0;(0 10#:10*".;._2]0 :0);('abc' i. a.))&;:
 1.1  1.1  1.1 4
 2.0  1.0  2.2 4
 2.2  3.7  2.2 4
 1.2  1.2  1.2 4
 1.2  5.0  2.2 4
 1.2  6.0  2.2 4
 1.2  7.0  2.2 4
 1.2  3.0  2.2 4
)
(;:'a ab') -: f ,'aab'
(;:'aa a') -: f ,'aaa'
(;:'a aba') -: f ,'aaba'

NB. Example 6.  Storing items of y
STATE=: 1 1 ,:&:(,:~) 0 3  NB. This time with items that are not atoms
Y=:i.8
map=: <@:{.
FSM=: (1 ; STATE ; <@:map) Y
0 2 4 6 -: FSM ;: Y

STATE=: 1 1 ,:&:(,:~) 0 3  NB. This time with items that are not atoms
map =: <@:,:@{.
Y =: i. 8 2
FSM=: (1 ; STATE ; <@:map) Y
(0 2 4 6 { Y) -: FSM ;: Y

NB. Example 7.  CSV, showing empty results (from Danil Opsichuk)

(faliases)  =: i.6  [ faliases =: <;._2 'fbox flist fsl fcr fslcr ftrace '                        NB. name output functions
(opaliases) =: i.11 [ opaliases=: <;._2 'noop jtoi ewji ewnj evji evnj stop back jfwi ewfj evfj ' NB. operation codes	
(csvStates) =: i.7  [ csvStates=: <;._2 'NEWROW NEWFLD INFLD FQTE1 INQTE QQTE1 SERR '	      NB. states (rows)
(csvEvents) =: i.5  [ csvEvents=: <;._2 'eCOMMA eQUOTE eLF eOTHER eFINAL '                        NB. events (columns)

imap =: (',';'"';LF)  NB. input mapping, 2 less than number of columns in state tbl, due to the default item eOTHER and eFINAL event

s =: {{               NB. csv state in a form of a call to an anonymous verb compiling a human readable form into a table of integers
 NB.   0 COMMA       1 QUOTE      2 LF         3 OTHER     4  FINAL   <-input/ states should be the in same order as definitions above:
 l=.   NEWFLD,ewfj,  INQTE,jfwi,  NEWROW,ewfj, INFLD,noop, NEWROW,noop   NB. 0 NEWROW -- start of each row, a special case of new field in beg or after LF separator
 l=.l, NEWFLD,ewfj,  INQTE,jfwi,  NEWROW,ewfj, INFLD,noop, NEWFLD,noop   NB. 1 NEWFLD -- start of a new field, has to be a separate state to handle starting quote
 l=.l, NEWFLD,ewfj,  FQTE1,jfwi,  NEWROW,ewfj, INFLD,noop, NEWROW,ewnj   NB. 2 INFLD  -- inside of unquoted field
 l=.l, SERR,ewnj,    INFLD,noop,  SERR,ewnj,   SERR,ewnj,  SERR,ewnj 	 NB. 3 FQTE1  -- 1st quote occured in unquoted field, anything but 2nd quote is an error
 l=.l, INQTE,noop,   QQTE1,ewfj,  INQTE,noop,  INQTE,noop, SERR,ewnj     NB. 4 INQTE  -- inside of a quoted field, emit eating quote, error on final (not closed)
 l=.l, NEWFLD,ewfj,  INQTE,noop,  NEWROW,ewfj, SERR,ewnj,  NEWROW,ewnj   NB. 5 QQTE1  -- 1st quote inside quoted field, can only be followed by another quote or comma or lf
 l=.l, SERR,stop,    SERR,stop,   SERR,stop,   SERR,stop,  SERR,ewnj     NB. 6 SERR  -- an error occured, stop, last word is a cause 
 ((csvStates ,&# csvEvents), 2) $ l   NB. reshape based on events and state dims
}}''

NB. SM itself: use fslcr because both start/lengths and row/column indexes are needed to reshape words into records and to check for errors
NB. final d=4 event is needed to flag error from the errouneous states, if sm moved into it just after having handled last input item
smcsv =: (fslcr;s;imap;0 0 0 4)&;:  
traceAnnotate =: (<;._2'i j state evt next op '),((<"0)`(<"0)`(csvStates{~])`(csvEvents{~])`(csvStates{~])`(opaliases{~]))"1&.|:  NB. provide symbolic names the for sm trace output
tracecsv =: [:traceAnnotate (ftrace;s;imap;0 0 0 4)&;:  NB. human readable trace sm

unquote =: (1;(2 2 2$1 9 0 0  0 0 0 0);(<'"');0 0 0 0)&;:    NB. (not used) tiny sm removing odd (every a single or any first in a pair) quotes, from the early version
uSL =: 1 : '(,."1@[)u;.0]'  				NB. utility adverb to apply u at x start-lengths of y input, like in ex: (i. 2 2) <uSL i.10
coalescesm =: (2;(3 2 2 $ 0 0 1 1  2 0 1 0  0 3 1 2))& ;:    NB. y is bool vector: coalesc consecutive 1s with trailing 0, used later to join quote-emitted words 

csv =: {{  NB. dyad csv parses y text into boxes, bool x is a flag to enable checks, monad csv has checks disabled by default
 0 csv y	                                    NB. most files have variying number of records per line, empty lines, etc, disable checks
: 
 chk =. x                                      NB. flag to enable checks                           
 islf  =. e.&(I.NEWROW = ,{."1 s)    	      NB. a helper to check if a particular cr has been yielded as a new row
 isqte =. e.&(I.(FQTE1,QQTE1)e.~ ,{."1 s)      NB. helper to check if word was emited by quoted field state
 iserr =. e.&(I.-.(NEWFLD,NEWROW,QQTE1)e.~ ,{."1 s)  NB. helper to check if sm landed in wrong state - everything emited but the 3 is ungood
 'words cr' =. (2&{."1 ; {:"1) smcsv y         NB. run the sm and split start/lengths and coded rows/cols into a pair
 words  =. words <uSL y                        NB. convert start/lengths into boxed words
 qte =. isqte cr                               NB. mark words emited by quote
 slqte =. coalescesm qte                       NB. start/lengths of quote emitted words traling with normal emit
 qwords =. slqte (<@:;)uSL words               NB. coalesce quoted words with last normal emit
 words =. qwords (<:+/|:slqte)} words          NB. amend the normal trailings with combined words
 words =. words #~ -. qte                      NB. drop the quoted words preceding trailings which are now combined
 cr =. cr #~ -.qte                             NB. drop the cr of quoted words
 if. iserr {: cr *. chk    do.		      NB. check if the last state is error
   errmsg =. 'Malformed input around row #', (":>:+/islf cr) , ' at ' , >{:words  NB. count lines and take the last word 
   errmsg (13!:8) 1                            NB. throw a parsing error
 end.                       
 if. chk do.
   mf =. 1+(I.|~(1 + 1 i.~ ])) islf cr	      NB. collect mods of fields per each record, based on 1st row LF offset
   ei =. 1 i.~ 2 ~:/\  mf		      NB. compare number of fields per records, look for a mismatch
   if. ei <<:#mf do. ('Row ',(":2+ei),' has a different number of fields') (13!:8) 1 end. NB. throw a error of inconsistent field size
 end.
  (islf cr) [;.2 words	                NB. rearrange words into a table according to LF placement
}}

txt=: 0 : 0			      NB. an example to play with: csv txt or tracecsv txt
a,b,c
d,"hello, "" cruel"" world",f
d,e,"r"
)

(3 3$<;._1 '|a|b|c|d|hello, " cruel" world|f|d|e|r') -: csv txt



0 -: (<1 0){ (5; (2 2 2 $ 1 7 1 0   1 6 1 6);(<'<');0 0 0 _1) ;: '<'             NB. backtrack from the position 0 goes to the first item at i=0

0 -: (<2 0){ (5; (3 2 2 $ 1 7 1 0   2 7 2 0  2 6 2 6);(<'<');0 0 0 _1) ;: ' <'   NB. backtrack from the 1st position goes to the first item at i=0

  


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
''  -: ( 0;sh;mh;19 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'
'index error'  -: ( 0;sh;mh;20 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'
'index error'  -: ( 0;sh;mh;_1 _1 0 0)  ;: etx 'qqq0x30x50x40x0xxxx'


4!:55 ;:'A f me mh mj mj1 mq mv remq remq1 remq2 se sh sj sq sqx sv'
4!:55 ;:'t t testj x y STATE Y map FSM'
4!:55 ;:'faliases opaliases csvStates csvEvents fbox flist fsl fcr fslcr ftrace noop jtoi ewji ewnj evji evnj stop back jfwi ewfj evfj NEWROW NEWFLD INFLD FQTE1 INQTE QQTE1 SERR '
4!:55 ;:'eCOMMA eQUOTE eLF eOTHER eFINAL imap s '
4!:55 ;:'smcsv traceAnnotate tracecsv unquote uSL coalescesm csv txt '




epilog''

