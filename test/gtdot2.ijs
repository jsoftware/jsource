prolog './gtdot2.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. j904 64-bit only

TASK=: <: 9!:56'maxtasks'

1: 0&T."1^:(0 < #) ''$~ (0 >. TASK-1 T. ''),0

STRIDE=: 3000                 NB. stride between tasks
NX=: 2000

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with name f([0..x-1]+STRIDE*y)loc
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+y*STRIDE  NB. unique locale name for each task
 1: 18!:3 <f                    NB. create new locale and keep it
end.
1
)

NB. test create locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. run & open the futures results
NB. repeat running and locales are re-created
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create/erase locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with name f([0..x-1]+STRIDE*y)loc
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+y*STRIDE  NB. unique locale name for each task
 18!:55 [ 18!:3 <f              NB. create new locale and erase immediately
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create/erase locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with same name
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+0*STRIDE  NB. same locale name
 18!:55 [ 18!:3 <f              NB. create new locale and erase immediately
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create/erase other locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with name f([0..x-1]+STRIDE*y)loc
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+y*STRIDE  NB. unique locale name for each task
 18!:3 <f                       NB. create new locale
 g=. 'loc',~'f',":xno+(TASK|y+(TASK>1))*STRIDE  NB. unique locale name in another task
 18!:55 ::1: <g                 NB. erase locale of another task, possibly not exist
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create/batch erase other locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with name f([0..x-1]+STRIDE*y)loc
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+y*STRIDE  NB. unique locale name for each task
 18!:3 <f                       NB. create new locale
end.
z=. 1                           NB. accumulator
g=. 'loc',~'f',":(x-1)+(TASK|y+(TASK>1))*STRIDE  NB. last locale name in another task
while. 0 > 18!:0 <g do. 6!:3[0.001 end. NB. wait until all tasks have created all locales
for_xno. i.x do.
 g=. 'loc',~'f',":xno+(TASK|y+(TASK>1))*STRIDE  NB. unique locale name in another task
 z=. z *. 18!:55 <g             NB. erase locale of another task
end.
z
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create the same locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with name f([0..x-1]+STRIDE*y)loc
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+0*STRIDE  NB. unique locale name for each task
 1: 18!:3 <f                    NB. re-create the samelocale
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel create numbered locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
t1=: 4 : 0"_ 0

NB. create numbered locale
for_xno. i.x do.
 1: 18!:3 ''    NB. create new numbered locale and keep it
end.
1
)

NB. test create numbered locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
z=. ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task
18!:55 [ 18!:1 [1                   NB. erase residual numbered locales in each task
z
)

NB. run & open the futures results
; s1''

NB. test parallel create/erase numbered locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. locale list is stored inside array locs_y
t1=: 4 : 0"_ 0

NB. create numbered locale
for_xno. i.x do.
 18!:55 [ 18!:3 ''                  NB. create new numbered locale and erase immediately
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK           NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel update local noun
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. name in each task is noun_y
t1=: 4 : 0"_ 0

NB. create noun
nouns=. 'noun_',(":y)
(nouns)=. ''
for_xno. i.x do.
 (nouns)=. nouns~ , <xno            NB. update local noun
end.
1
)

NB. test update local noun on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK           NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel update global noun
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. name in each task is noun_y
t1=: 4 : 0"_ 0

NB. create noun
nouns=. 'noun_',(":y)
(nouns)=: ''
for_xno. i.x do.
 (nouns)=: nouns~ , <xno            NB. update global noun
end.
1
)

NB. test update global noun on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
z=. ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task
for_t. i.TASK do.
 4!:55 <'noun_',(":t)               NB. erase global noun
end.
z
)

NB. run & open the futures results
; s1''

NB. test parallel write local noun
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. name in each task is noun_y
t1=: 4 : 0"_ 0

NB. write local noun
for_xno. i.x do.
 ('noun_',(":y),'_',(":xno))=. <xno NB. write local noun
end.
1
)

NB. test write local noun on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK           NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel write global noun
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. name in each task is noun_y
t1=: 4 : 0"_ 0

nouns=. 'noun_',(":y)                 NB. name of name list
(nouns)=: ''                          NB. initialize name list to empty
NB. write global noun
for_xno. i.x do.
 (nouns)=: nouns~ , < ('noun_',(":y),'_',(":xno))   NB. unique global name in each task
 ('noun_',(":y),'_',(":xno))=: <xno   NB. write global noun
end.
1
)

NB. test write local noun on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
z=. ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task
for_t. i.TASK do.
 4!:55 ('noun_',(":t))~             NB. erase global noun
 4!:55 <'noun_',(":t)               NB. erase name of name list
end.
)

NB. run & open the futures results
; s1''

18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales
18!:55 [ 18!:1 [1                   NB. erase residual numbered locales in each task

4!:55 ;:'NX STRIDE TASK s1 t1'

epilog''

