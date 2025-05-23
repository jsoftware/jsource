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


1: (6!:5) ] _5000  NB. engage test mode

p1=: 3 : 0
for_i. y do.
ALL=: 15!:15^:i ALL
end.
i. 0 0
)

t1=: 4 : 0
ALL=: 128$00
pyx=. (p1 t.''@y)"0 x#0
1:&>pyx
i. 0 0
)

p2=: 3 : 0
for_i. i.100 do.
ALL=: ALL, ,~(3 T.'')
end.
i. 0 0
)

t2=: 3 : 0
ALL=: 0 0$0
pyx=. (p2 t.'')"0  i. y
1:&>pyx
i. 0 0
)

p3=: 3 : 0
ALL=. 0$0
for_i. i.100 do.
ALL=. ALL, (3 T.'')
end.
#ALL
)

t3=: 3 : 0
pyx=. (p3 t.'')"0  i. y
1:&>pyx
i. 0 0
)

p4=: 3 : 0
ALL=. 0 0$0
for_i. i.100 do.
ALL=. ALL, ,~(3 T.'')
end.
#ALL
)

t4=: 3 : 0
pyx=. (p4 t.'')"0  i. y
1:&>pyx
i. 0 0
)


NB. Insert p1 here to override the default from above

1000 t1 0.5 < 1000 ?@$ 0

t3 1000

t2 1000
t4 1000

1: 6!:5 ] 0

delth''

4!:55 ;:'ALL delth N p1 p2 p3 p4 t1 t2 t3 t4 sleep wthr'

epilog''

1: 0 : 0  NB. Move these tests to be executed, one by one

p1=: {{  NB. scaf
label_loop. ALL=:ALL, ((,0.5)) goto_loop.
EMPTY
}}

p1=: {{  NB. scaf
label_loop. ALL=:ALL, ,0.5 goto_loop.
EMPTY
}}

NB. ***** current test *****
p1=: {{  NB. scaf
while. do. ALL=:ALL, ((,0.5)) end.
EMPTY
}}

p1=: {{  NB. scaf
for. 1e7#' ' do. ALL=:ALL, ((,0.5)) end.
EMPTY
}}

p1=: {{  NB. scaf
for. 1e7#' ' do. ALL=:ALL (, >:) ((,0.5)) end.
EMPTY
}}

p1=: {{  NB. scaf
for. 1e7#' ' do. ALL=:ALL, ,0.5 end.
EMPTY
}}

p1=: {{
for_i. (1000 * 3 T. '') + i.1e7 do.
ALL=: ALL, i
end.
EMPTY
}}

p1=: {{
for_i. (1000 * 3 T. '') + i.1e7 do.
ALL=: ALL, ,i
end.
EMPTY
}}

p1=: {{
for_i. (1000 * 3 T. '') + i.1e7 do.
ALL=: ALL, ,i [ echo i
end.
EMPTY
}}

)

