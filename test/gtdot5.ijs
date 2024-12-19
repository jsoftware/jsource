prolog './gtdot5.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. 64-bit only

3 : 0''
if. IFWIN do.
 sleep=: usleep@>.@(1e6&*)
else.
 sleep=: 6!:3
end.
1
)

NB. wait until there are y waiting threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

N=: 24  NB. max # worker threads

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N = 1 T.''
wthr N

p1=: 3 : 0
for_i. i.100 do.
ALL=: ALL, (3 T.'')
end.
EMPTY
)

p2=: 3 : 0
for_i. i.100 do.
ALL=: ALL, ,~(3 T.'')
end.
EMPTY
)

t1=: 3 : 0
ALL=: 0$0
pyx=. (p1 t.'')"0  i. 16
1:&>pyx
echo #~.ALL
echo #/.~ ALL
EMPTY
)

t2=: 3 : 0
ALL=: 0 0$0
pyx=. (p2 t.'')"0  i. 16
1:&>pyx
echo #~.ALL
echo #/.~ ALL
EMPTY
)

t1''
t2''

delth''

4!:55 ;:'ALL delth N p1 p2 t1 t2 sleep wthr'

epilog''

