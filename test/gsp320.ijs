prolog './gsp320.ijs'
NB. , -------------------------------------------------------------------

a=: (?11 5 7$2) * ?11 5 7 3$4
c=: ; (i.1+r) <"1@comb&.>r=:#$a

f=: 4 : '(*./ scheck t) *. (,"x a) -: t=: ,"x s=:(2;y)$.a'
(i.>:r) f&>/ c

s=: 1 $. 2e9 3 4;0;0
s=: (?7 3 4$4) (?7$2e9)}s
scheck s
scheck ,"2 s

'limit error' -: ,   etx 1 $.(3$  2e9);0 1 2;0
'limit error' -: ,"2 etx 1 $.(>IF64{2 4 2e9;2 1e10 2e9);0 1 2;0
'domain error' -: ($. i. 2 3) , etx 1r2
'domain error' -: ($. i. 2 3) , etx 1r2 1r3
'domain error' -: ($. i. 2 3) , etx 3 3 $ 1r2 1r3

d=: (?6 3 5$2) * ?6 3 5 2$5
c=: ; (i.1+r) <"1@comb&.>r=:#$d

f=: 4 : '((d,"r  0) -: s,"r  0) *. ( 0,"r d) -:  0,"r s=:(2;y)$.d [ r=:x'
g=: 4 : '((d,"r 99) -: s,"r 99) *. (99,"r d) -: 99,"r s=:(2;y)$.d [ r=:x'
h=: 4 : '(((97+d),"r 99) -: (97+s),"r 99) *. (99,"r 97+d) -: 99,"r 97+s=:(2;y)$.d [ r=:x'

(i.1+#$d) f&>/c
(i.1+#$d) g&>/c
(i.1+#$d) h&>/c

'limit error' -: 3, etx 1$. imax
'domain error' -: 1$. etx >:imax

'limit error' -: 3, etx~ 1$. imax

f=: 4 : '(a ,"r        b) -: (s=: (2;x)$.a) ,"r        t=: (2;y)$.b'
h=: 4 : '(a ,"r&(97&+) b) -: (s=: (2;y)$.a) ,"r&(97&+) t=: (2;y)$.b'

c=: ; (i.1+r) <"1@comb&.>r=:#$a=: (?2 5 3$2) * ?2 5 3 4$5
d=: ; (i.1+r) <"1@comb&.>r=:#$b=: (?2 5 3$2) * ?2 5 3 4$5

c f&>/ d [ r=: 0
c f&>/ d [ r=: 1
c f&>/ d [ r=: 2
c f&>/ d [ r=: 3
c f&>/ d [ r=: 4

c h&>/ d [ r=: 0
c h&>/ d [ r=: 1
c h&>/ d [ r=: 2
c h&>/ d [ r=: 3
c h&>/ d [ r=: 4

2 3 4 5 -:  2 3,$. 4 5
2 3 0 1 0 -: 2 3,$. 0 1 0




epilog''

