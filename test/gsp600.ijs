NB. " monad -------------------------------------------------------------

d=: (13 5 7?@$2)*13 5 7?@$5
c=: ; (i.1+r) <"1@comb&.>r=:#$d

double=: 3 : '+.y'

f=: 3 : 0
 b=. (double"0 d) -: t=. double"0 s=:(2;y)$.d
 if. b *. 0<#$t do. *./ scheck t else. b end.
)

f&>c

A=: 1 : 0
  :
 b=. (u"x d) -: t=. u"x s=:(2;y)$.d
 if. b *. 0<#$t do. *./ scheck t else. b end.
)

c=: ; (i.1+r) <"1@comb&.>r=:#$d

ravel=: 3 : ', y'
sum  =: 3 : '+/y'
rev  =: 3 : '|.y'
mean =: +/ % #

(i.>:r) ]     A&>/ c
(i.>:r) sum   A&>/ c
(i.>:r) ravel A&>/ c
(i.>:r) rev   A&>/ c
(i.>:r) #     A&>/ c
(i.>:r) mean  A&>/ c

d=: 2 3 4$0
c=: ; (i.1+r) <"1@comb&.>r=:#$d
(i.>:r) mean  A&>/c

($"2 x) -: $"2 $. x=:2 3 3$12

(<1 0) 10 11&(4 : 'x o. y') A&>/c

'non-unique sparse elements' -: $"2 etx $. 2 3 4$12


NB. " dyad --------------------------------------------------------------

A2=: 1 : 0
  :
 xx=: (2;x)$.x0
 yy=: (2;y)$.y0
 b=. (x0 u"r y0) -: t=: xx u"r yy
 if. b *. 0<#$t do.*./ scheck t else. b end.
)

minus=: 4 : 'x -  y'
cat  =: 4 : 'x ,  y'
rot  =: 4 : 'x |. y'

x0=: (13 5?@$2)*13 5 7?@$500
y0=: (13 5?@$2)*13 5 7?@$500
c=: ; (i.1+r) <"1@comb&.>r=:#$x0

minus A2&>/~c [ r=: 0
minus A2&>/~c [ r=: 1
minus A2&>/~c [ r=: 2
minus A2&>/~c [ r=: 3

cat   A2&>/~c [ r=: 0
cat   A2&>/~c [ r=: 1
cat   A2&>/~c [ r=: 2
cat   A2&>/~c [ r=: 3

x0=: (13 5 7?@$2)*13 5 7?@$5
y0=:              13 5 7  $0
c=: ; (i.1+r) <"1@comb&.>r=:#$x0

minus A2&>/~c [ r=: 0
minus A2&>/~c [ r=: 1
minus A2&>/~c [ r=: 2
minus A2&>/~c [ r=: 3

cat   A2&>/~c [ r=: 0
cat   A2&>/~c [ r=: 1
cat   A2&>/~c [ r=: 2
cat   A2&>/~c [ r=: 3

x0=:              13 5 7  $0
y0=: (13 5 7?@$2)*13 5 7?@$5
c=: ; (i.1+r) <"1@comb&.>r=:#$x0

minus A2&>/~c [ r=: 0
minus A2&>/~c [ r=: 1
minus A2&>/~c [ r=: 2
minus A2&>/~c [ r=: 3

cat   A2&>/~c [ r=: 0
cat   A2&>/~c [ r=: 1
cat   A2&>/~c [ r=: 2
cat   A2&>/~c [ r=: 3

x0=: 13 5 7$0
y0=: 13 5 7$0
c=: ; (i.1+r) <"1@comb&.>r=:#$x0

minus A2&>/~c [ r=: 0
minus A2&>/~c [ r=: 1
minus A2&>/~c [ r=: 2
minus A2&>/~c [ r=: 3

cat   A2&>/~c [ r=: 0
cat   A2&>/~c [ r=: 1
cat   A2&>/~c [ r=: 2
cat   A2&>/~c [ r=: 3

x0=: (13 5  ?@$2)*13 5  ?@$5
y0=: (13 5 7?@$2)*13 5 7?@$5
c=: ; (i.1+r) <"1@comb&.>r=:#$x0
d=: ; (i.1+r) <"1@comb&.>r=:#$y0
c rot A2&>/ d [ r=: 0 1

x0=: (13  ?@$2)*13 2   ?@$5
y0=: (13 7?@$2)*13 7 17?@$5
c=: ; (i.1+r) <"1@comb&.>r=:#$x0
d=: ; (i.1+r) <"1@comb&.>r=:#$y0
c rot A2&>/ d [ r=: 1 2

f=: 3 : 0
 (10 11 o."1 0 xx) -: 10 11 o."1 0 s=: (2;y)$.xx
)

xx=: (13 7?@$2)*13 7 4 2?@$5
c=: ; (i.1+r) <"1@comb&.>r=:#$xx
f&>c


4!:55 ;:'A A2 c cat d double f mean minus q r ravel rev rot s sum t x x0 xx y y0 yy'


