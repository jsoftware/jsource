NB. 5!:30 ---------------------------------------------------------------

cr  =: 13{a.
lf  =: 10{a.
crlf=: 13 10{a.

f   =: 5!:30

dtb =: -@(+/"1)@(*./\."1)@(' '&=) <@}."_1 ]
g   =: ; @: (,&lf&.>) @: dtb @: (1 1&}.) @: (_1 _1&}.) @: ": @: <

t=: 0 250 _ _ f x=: i.4 5
t -: ,(":i.4 5),.lf

t=: 0 250 5 10 f x=: i.300
t -: (250{.":x),'...',lf

t=: 0 _ _ _ f x=: ?2 3 4 5 3$1e6
t -: g x

t=: 0 250 5 10 f i.40 1
16 = +/ t=lf
t -: (,' ',.'01234',.lf),'...',lf,,(2":30+i.10 1),.lf

t=: 0 _ 5 10 f x=: lf (100?2000)}2000$'x'
16 = +/ t=lf
i =: 1 i.~ '...' E. t
t -: (i{.x),'...',lf,(((5+i)-#t){.x),lf

'domain error' -: 'abc'       5!:30 etx 4 5 6
'domain error' -: (1 2 3 4.5) 5!:30 etx 4 5 6
'domain error' -: (1 2 3 4j5) 5!:30 etx 4 5 6
'domain error' -: (1 2 3 4r5) 5!:30 etx 4 5 6
'domain error' -: (1;2;3;455) 5!:30 etx 4 5 6

'domain error' -: (0 _1 9 9 ) 5!:30 etx 4 5 6
'domain error' -: (0 9 _1 9 ) 5!:30 etx 4 5 6
'domain error' -: (0 9 9 _1 ) 5!:30 etx 4 5 6

'rank error'   -: (1        ) 5!:30 etx 4 5 6
'rank error'   -: (,:1 2 3 4) 5!:30 etx 4 5 6

'length error' -: (i.5      ) 5!:30 etx 4 5 6
'length error' -: (i.3      ) 5!:30 etx 4 5 6

'index error'  -: (3 10 10 9) 5!:30 etx 4 5 6


4!:55 ;:'cr crlf dtb f g i lf t x'


