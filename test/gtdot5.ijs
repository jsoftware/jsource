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

N=: 62  NB. max # worker threads

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N = 1 T.''
wthr N

p2 =: ".
(i. 100) -: > (p2@> t.'')"0  <@":"0 i. 100   NB. test executing tacit verb with no parser running

p1=: 3 : 0
for_i. 8 c. (1000 * 3 T. '') + i.100000 do.
ALL=: ALL, ,i
end.
EMPTY
)

p1=: 3 : 0  NB. scaf
label_loop. ALL=: i. 2040 goto_loop.
EMPTY
)

p2=: 3 : 0
for_i. i.100 do.
ALL=: ALL, ,~(3 T.'')
end.
EMPTY
)

t1=: 3 : 0
ALL=: 128$00
pyx=. (p1 t.'')"0  i. 100
1:&>pyx
EMPTY
)

t2=: 3 : 0
ALL=: 0 0$0
pyx=. (p2 t.'')"0  i. 100
1:&>pyx
EMPTY
)

p3=: 3 : 0
ALL=. 0$0
for_i. i.100 do.
ALL=. ALL, (3 T.'')
end.
#ALL
)

p4=: 3 : 0
ALL=. 0 0$0
for_i. i.100 do.
ALL=. ALL, ,~(3 T.'')
end.
#ALL
)

t3=: 3 : 0
pyx=. (p3 t.'')"0  i. 100
1:&>pyx
EMPTY
)

t4=: 3 : 0
pyx=. (p4 t.'')"0  i. 100
1:&>pyx
EMPTY
)

1: 6!:5 ] 1  NB. test mode
{{ for. y$' ' do. t1 '' end. 1 }} 20000  NB. scaf 200
1: 6!:5 ] 0
0 : 0
t3''

t2''
t4''
)
delth''

4!:55 ;:'ALL delth N p1 p2 p3 p4 t1 t2 t3 t4 sleep wthr'

epilog''

