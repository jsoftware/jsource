NB. x i. y timing tests -------------------------------------------------

ss =: +/ @: *:
rsq=: [: -. ss@(- +/ % #)@[ %~ ss@:-

f=: 3 : 0
 10 timer 's i. t' [ s=. y$2 [ t=. y$3
)

x =: 7000+2000 * i.10
y =: f"0 x
y1=: (1,.x) +/ .* y %. 1,.x
threshold < y rsq y1

f=: 4 : 0
 10 timer 'i.~t' [ t=.a.{~(y,x)?@$#a.
)

n=: 2000*4+i.8
y=: (>:i.15) f"0/ n
y1=: y (] +/ .*"2 1 %."1 2) n^/0 1
threshold < (+/ % #), y rsq"1 y1

f=: 3 : 0
 5 timer 'i.~t' [ t=. y ?@$ y
)

n=: 8000*4+i.8
y=: f"0 n
y1=: y (] +/ .*"2 1 %."1 2) n^/0 1
threshold < y rsq y1

f=: 3 : 0
 5 timer 'i.~t' [ t=. 0.001*_5e5+y?@$1e6
)

n=: 8000*4+i.8
y=: f"0 n
y1=: y (] +/ .*"2 1 %."1 2) n^/0 1
threshold < y rsq y1

f1=: 3 : 0
 ".'timer ''i.~x'' [ x=. y+i.',"1 ":,.n
)

f2=: 3 : 0
 ".'timer ''i:~x'' [ x=. y+i.',"1 ":,.n
)

n=: 1000 * 1 2 3 4 5 6 7 14 21

y=: f1"0 (0 0.5 1.5 +/10^i.14),1.1 1x1 1p1 */ 10^i.14
y1=: y (] +/ .*"_ 1 %."1 2) n^/0 1
threshold < (+/ % #), y rsq"1 y1

y=: f2"0 (0 0.5 1.5 +/10^i.14),1.1 1x1 1p1 */ 10^i.14
y1=: y (] +/ .*"_ 1 %."1 2) n^/0 1
threshold < (+/ % #), y rsq"1 y1


4!:55 ;:'f f1 f2 n rsq ss x y y1' 


