NB. n b. ----------------------------------------------------------------

0 0 0 1 -: 0 0 1 1 (1 b.) 0 1 0 1
0 1 1 1 -: 0 0 1 1 (7 b.) 0 1 0 1

0 0 0 1 -: 0 0 1 1 (+&.o.1) b. 0 1 0 1

(|:#:i.16) -: 0 0 1 1 (i.16) b. 0 1 0 1
(|:(4#2)#:_16+i.16) -: 0 0 1 1 (_16+i.16) b. 0 1 0 1

f =: (i.16) b.
(f 0 1) -: 0 f 0 1

f =: (_16+i.16) b.
(f 0 1) -: 0 f 0 1

5 6 3 0 4 -: $ (i.3 0 4) b. 5 6$1
4 3 0 2   -: $ 0 0 1 1 (i.3 0 2) b. 0 1 0 1

'domain error' -: ex '''b'' b.'
'index error'  -: ex '36    b.'
'index error'  -: ex '_17   b.'
'domain error' -: ex '3.4   b.'
'domain error' -: ex '3j4   b.'
'domain error' -: ex '(<1)  b.'

f =: (?16) b.

'domain error' -: 'a' f etx 1 
'domain error' -: 2   f etx 1      
'domain error' -: 3.4 f etx 1        
'domain error' -: 3j4 f etx 1         
'domain error' -: (<1)f etx 1            

'domain error' -: f etx 'a'   
'domain error' -: f etx 2              
'domain error' -: f etx 3.4              
'domain error' -: f etx 3j4             
'domain error' -: f etx <1     


NB. n b. bitwise operations ---------------------------------------------

N=: IF64{32 64
B=:  N $ 2 
f=: B&#:

x=: (_1^?100$2) * ?100$2e9
y=: (_1^?100$2) * ?100$2e9

(x 0"0   &f y) -: f x (16+#.0 0 0 0) b. y
(x *.    &f y) -: f x (16+#.0 0 0 1) b. y
(x >     &f y) -: f x (16+#.0 0 1 0) b. y
(x ["0   &f y) -: f x (16+#.0 0 1 1) b. y
(x <     &f y) -: f x (16+#.0 1 0 0) b. y
(x ]"0   &f y) -: f x (16+#.0 1 0 1) b. y
(x ~:    &f y) -: f x (16+#.0 1 1 0) b. y
(x +.    &f y) -: f x (16+#.0 1 1 1) b. y
(x +:    &f y) -: f x (16+#.1 0 0 0) b. y
(x =     &f y) -: f x (16+#.1 0 0 1) b. y
(x -.@]"0&f y) -: f x (16+#.1 0 1 0) b. y
(x >:    &f y) -: f x (16+#.1 0 1 1) b. y
(x -.@["0&f y) -: f x (16+#.1 1 0 0) b. y
(x <:    &f y) -: f x (16+#.1 1 0 1) b. y
(x *:    &f y) -: f x (16+#.1 1 1 0) b. y
(x 1"0   &f y) -: f x (16+#.1 1 1 1) b. y

g0=: 4 : 0
 r=. x
 x=. y
 r1=. 1+r
 assert. (0"0   /"r1&f x) -: f (16+#.0 0 0 0) b./"r x
 assert. (*.    /"r1&f x) -: f (16+#.0 0 0 1) b./"r x
 assert. (>     /"r1&f x) -: f (16+#.0 0 1 0) b./"r x
 assert. (["0   /"r1&f x) -: f (16+#.0 0 1 1) b./"r x
 assert. (<     /"r1&f x) -: f (16+#.0 1 0 0) b./"r x
 assert. (]"0   /"r1&f x) -: f (16+#.0 1 0 1) b./"r x
 assert. (~:    /"r1&f x) -: f (16+#.0 1 1 0) b./"r x
 assert. (+.    /"r1&f x) -: f (16+#.0 1 1 1) b./"r x
 assert. (+:    /"r1&f x) -: f (16+#.1 0 0 0) b./"r x
 assert. (=     /"r1&f x) -: f (16+#.1 0 0 1) b./"r x
 assert. (-.@]"0/"r1&f x) -: f (16+#.1 0 1 0) b./"r x
 assert. (>:    /"r1&f x) -: f (16+#.1 0 1 1) b./"r x
 assert. (-.@["0/"r1&f x) -: f (16+#.1 1 0 0) b./"r x
 assert. (<:    /"r1&f x) -: f (16+#.1 1 0 1) b./"r x
 assert. (*:    /"r1&f x) -: f (16+#.1 1 1 0) b./"r x
 assert. (1"0   /"r1&f x) -: f (16+#.1 1 1 1) b./"r x
 1
)

1 g0 x=: (? * _1: ^ ?@($&2)@$) 7 13 11$2e9
2 g0 x
3 g0 x

g1=: 4 : 0
 r=. x
 x=. y
 r1=. 1+r
 assert. (0"0   /\."r1&f x) -: f (16+#.0 0 0 0) b./\."r x
 assert. (*.    /\."r1&f x) -: f (16+#.0 0 0 1) b./\."r x
 assert. (>     /\."r1&f x) -: f (16+#.0 0 1 0) b./\."r x
 assert. (["0   /\."r1&f x) -: f (16+#.0 0 1 1) b./\."r x
 assert. (<     /\."r1&f x) -: f (16+#.0 1 0 0) b./\."r x
 assert. (]"0   /\."r1&f x) -: f (16+#.0 1 0 1) b./\."r x
 assert. (~:    /\."r1&f x) -: f (16+#.0 1 1 0) b./\."r x
 assert. (+.    /\."r1&f x) -: f (16+#.0 1 1 1) b./\."r x
 assert. (+:    /\."r1&f x) -: f (16+#.1 0 0 0) b./\."r x
 assert. (=     /\."r1&f x) -: f (16+#.1 0 0 1) b./\."r x
 assert. (-.@]"0/\."r1&f x) -: f (16+#.1 0 1 0) b./\."r x
 assert. (>:    /\."r1&f x) -: f (16+#.1 0 1 1) b./\."r x
 assert. (-.@["0/\."r1&f x) -: f (16+#.1 1 0 0) b./\."r x
 assert. (<:    /\."r1&f x) -: f (16+#.1 1 0 1) b./\."r x
 assert. (*:    /\."r1&f x) -: f (16+#.1 1 1 0) b./\."r x
 assert. (1"0   /\."r1&f x) -: f (16+#.1 1 1 1) b./\."r x
 1
)

1 g1 x=: (? * _1: ^ ?@($&2)@$) 7 13 11$2e9
2 g1 x
3 g1 x

g2=: 4 : 0
 r=. x
 x=. y
 r1=. 1+r
 assert. (0"0   /\"r1&f x) -: f (16+#.0 0 0 0) b./\"r x
 assert. (*.    /\"r1&f x) -: f (16+#.0 0 0 1) b./\"r x
 assert. (>     /\"r1&f x) -: f (16+#.0 0 1 0) b./\"r x
 assert. (["0   /\"r1&f x) -: f (16+#.0 0 1 1) b./\"r x
 assert. (<     /\"r1&f x) -: f (16+#.0 1 0 0) b./\"r x
 assert. (]"0   /\"r1&f x) -: f (16+#.0 1 0 1) b./\"r x
 assert. (~:    /\"r1&f x) -: f (16+#.0 1 1 0) b./\"r x
 assert. (+.    /\"r1&f x) -: f (16+#.0 1 1 1) b./\"r x
 assert. (+:    /\"r1&f x) -: f (16+#.1 0 0 0) b./\"r x
 assert. (=     /\"r1&f x) -: f (16+#.1 0 0 1) b./\"r x
 assert. (-.@]"0/\"r1&f x) -: f (16+#.1 0 1 0) b./\"r x
 assert. (>:    /\"r1&f x) -: f (16+#.1 0 1 1) b./\"r x
 assert. (-.@["0/\"r1&f x) -: f (16+#.1 1 0 0) b./\"r x
 assert. (<:    /\"r1&f x) -: f (16+#.1 1 0 1) b./\"r x
 assert. (*:    /\"r1&f x) -: f (16+#.1 1 1 0) b./\"r x
 assert. (1"0   /\"r1&f x) -: f (16+#.1 1 1 1) b./\"r x
 1
)

1 g2 x=: (? * _1: ^ ?@($&2)@$) 7 13 11$2e9
2 g2 x
3 g2 x

x=: (? * _1: ^ ?@($&2)@$) 2 3 5$2e9
y=: (? * _1: ^ ?@($&2)@$) 7 3  $2e9

(x 0"0   "1/&f y) -: f x (16+#.0 0 0 0) b./y
(x *.    "1/&f y) -: f x (16+#.0 0 0 1) b./y
(x >     "1/&f y) -: f x (16+#.0 0 1 0) b./y
(x ["0   "1/&f y) -: f x (16+#.0 0 1 1) b./y
(x <     "1/&f y) -: f x (16+#.0 1 0 0) b./y
(x ]"0   "1/&f y) -: f x (16+#.0 1 0 1) b./y
(x ~:    "1/&f y) -: f x (16+#.0 1 1 0) b./y
(x +.    "1/&f y) -: f x (16+#.0 1 1 1) b./y
(x +:    "1/&f y) -: f x (16+#.1 0 0 0) b./y
(x =     "1/&f y) -: f x (16+#.1 0 0 1) b./y
(x -.@]"0"1/&f y) -: f x (16+#.1 0 1 0) b./y
(x >:    "1/&f y) -: f x (16+#.1 0 1 1) b./y
(x -.@["0"1/&f y) -: f x (16+#.1 1 0 0) b./y
(x <:    "1/&f y) -: f x (16+#.1 1 0 1) b./y
(x *:    "1/&f y) -: f x (16+#.1 1 1 0) b./y
(x 1"0   "1/&f y) -: f x (16+#.1 1 1 1) b./y


NB. 32 33 34 b. ---------------------------------------------------------

test=: 3 : 0
 assert. xx    (f -: g)   yy [ xx=: ?4 5$2e9  [ yy=: ?4 5$2e9
 assert. xx    (f -: g)  -yy
 assert. (-xx) (f -: g)   yy
 assert. (-xx) (f -: g)  -yy
 assert. xx    (f -: g) 0*yy 
 assert. (-xx) (f -: g) 0*yy
 assert. 0     (f -: g)   yy
 assert. 0     (f -: g)  -yy
 assert. xx    (f -: g)   yy [ xx=: imin
 assert. xx    (f -: g)  -yy
 assert. xx    (f -: g)   yy [ xx=: imax
 assert. xx    (f -: g)  -yy
 assert. xx    (f -: g)   yy [ xx=: 4 5 ?@$100  [ yy=: imin
 assert. (-xx) (f -: g)   yy
 assert. xx    (f -: g)   yy [ xx=: 4 5 ?@$2e9  [ yy=: imin
 assert. (-xx) (f -: g)   yy
 assert. xx    (f -: g)   yy [ xx=: 4 5 ?@$100  [ yy=: imax
 assert. (-xx) (f -: g)   yy
 assert. xx    (f -: g)   yy [ xx=: 4 5 ?@$2e9  [ yy=: imax
 assert. (-xx) (f -: g)   yy
 1
)

f=: 4 : 'x|. B#:y' " 0
g=: B&#:@(32 b.)
test ''

f=: 4 : 'x |.!.0 B#: y' " 0
g=: B&#:@(33 b.)
test ''

f=: 4 : '(N {. (N<.|x)$(0>x)*.0>y) +. x |.!.0"0 1 B#: y' " 0
g=: B&#:@(34 b.)
test ''


NB. v b. ----------------------------------------------------------------

0 0 0 -: +  b. 0
2 _ 2 -: %. b. 0

((":/:p),'&{') -: p&{ b. _1 [ p=:?~13
'^.' -: ^ b. _1

'0 $~ }.@$' -: +     b. 1
'0 $~ }.@$' -: *&.^  b. 1
'1 $~ }.@$' -: *     b. 1
'1 $~ }.@$' -: +&.^. b. 1

_ _ _ -: asdfnotexist b. 0

'domain error' -: + b. etx 2  
'domain error' -: ^ b. etx _2           
'domain error' -: - b. etx _                
'domain error' -: ^ b. etx 0.5


NB. n b. inverse --------------------------------------------------------

x -: ]&.(12345&(22 b.)) x=: (] , -) 20 ?@$ 2e9
x -: ]&.(12345&(25 b.)) x
x -: ]&.(12345&(21 b.)) x
x -: ]&.(12345&(26 b.)) x

x -: ]&.(19 b.&12345) x
x -: ]&.(28 b.&12345) x

'domain error' -: ex '21 b.&12345   b. _1'
'domain error' -: ex '26 b.&12345   b. _1'
'domain error' -: ex '12345&(19 b.) b. _1'
'domain error' -: ex '12345&(28 b.) b. _1'


4!:55 ;:'B d f g g0 g1 g2 N p test x xx y yy'


