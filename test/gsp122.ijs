prolog './gsp122.ijs'
NB. -: ------------------------------------------------------------------

(scheck -: $.x), (-: -: -:&.$.) x=:     _10+?    405$2
(scheck -: $.x), (-: -: -:&.$.) x=:     _10+?  3 4 5$21
(scheck -: $.x), (-: -: -:&.$.) x=:   o._10+?  3 4 5$21
(scheck -: $.x), (-: -: -:&.$.) x=:j./o._10+?2 3 4 5$21

0 -: 3 $. -: $. 0 1 0
0 -: 3 $. -: $. 2 3 4
0 -: 3 $. -: $. 2 3.4
0 -: 3 $. -: $. 2 3j4

x -: (3;17)$.x=: $. 1+?3 4 5$100

f=: 4 : '((2;x)$.s) -: (2;y)$.s'

d=: ?6 4 5 3$7
s=: $. d
d -: s

c=: ; (i.1+r) <"1@comb&.>r=:#$s
f&>/~c

x=: 2 3$0
y=: 0 * $.i.2 3
x -:  y
x -:~ y
((3;5)$.$.x) -:  5 + y
((3;5)$.$.x) -:~ 5 + y

x=: 0*$.?4 10$2
y=: 0*$.?4 10$2
x          -: y
(x+5)      -: y+10%2
((3;0)$.x) -: (3;o.0)$.y

x=: 20 (<1 2)}1$.3 4;0 1;10
y=: 10 (<2 3)}1$.3 4;0 1;20
0 = x -: y
0 = y -: x

x=: (3;10) $. $. 3 4$100
y=: (3;20) $. $. 3 4$100
x -: y
y -: x

x=:        $. 4 4 4
y=: (3;4)$.$. 0 4 4
x -: y
y -: x

x=: ?17 41$5
y=: ?17 41$5
s=: ($.x) = $.y
t=: $. x=y
s -: t

x=: 1$.1e8 2e8 3e8;0 1;o.0
y=: 1$.1e8 2e8 3e8;0 1;o.0
x -: y

NB. non-covered data in a must match we

p=: 1+?6 7 5 3$10

f=: 3 : '((5 1#0 88)+(5 1#1 0)*p) -: s=: (3;88)$.(2;y)$.(5 1#1 0)*p'
g=: 3 : '((5 1#0 88)+(5 1#1 0)*p) -:~s=: (3;88)$.(2;y)$.(5 1#1 0)*p'
f&>0;0 1;0 1 2;0 1 2 3
g&>0;0 1;0 1 2;0 1 2 3

NB. unequal sparse element but complete coverage in sparse data

p=: ?6 7 5 3$10

f=: 3 : '(1+p) -: s=: (3;88)$.(2;y)$.1+p'
g=: 3 : '(1+p) -:~s=: (3;88)$.(2;y)$.1+p'
f&>c=: ; (i.1+r) <"1@comb&.>r=:#$p
g&>c

NB. same indices but different values

p=: ?6 7 5 3$10

f=: 3 : '-. (2*p) -:  s=: (2;y)$.p'
g=: 3 : '-. (2*p) -:~ s=: (2;y)$.p'
f&>c=: ; (i.1+r) <"1@comb&.>r=:#$p
g&>c





epilog''

