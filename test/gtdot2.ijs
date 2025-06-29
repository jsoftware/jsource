prolog './gtdot2.ijs'
NB. T. t. ------------------------------------------------------------------
NB. locale

void freesymb(J jt, A w){I j,wn=AN(w); LX k,* RESTRICT wv=LXAV0(w);
NB. 64-bit only
NB. delete all worker threads
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}
delth''  NB. make sure we start with an empty system

TASK=: <: 1 { 8 T. ''  NB. max # worker threads

1: 0&T."1^:(0 < #) ''$~ (0 >. TASK-1 T. ''),0

STRIDE=: 3000                 NB. stride between tasks
NX=: QKTEST{2000 500

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

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
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task 0
)

NB. run & open the futures results
; s1''

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

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
 (271828) 18!:55 [ 18!:3 <f              NB. create new locale and erase immediately
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task 1
)

NB. run & open the futures results
; s1''

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

NB. test parallel create/erase locale
NB. x is number of iteration in each task
NB. y is a task number
NB. result is 1
NB. each locale with same name
t1=: 4 : 0"_ 0
NB. create locale
for_xno. i.x do.
 f=. 'loc',~'f',":xno+0*STRIDE  NB. same locale name
 (271828) 18!:55 [ 18!:3 <f              NB. create new locale and erase immediately
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task 2
)

NB. run & open the futures results
; s1''

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

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
 (271828) 18!:55 ::1: <g                 NB. erase locale of another task, possibly not exist
end.
1
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task 3
)

NB. run & open the futures results
; s1''

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

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
 z=. z *. (271828) 18!:55 <g             NB. erase locale of another task
end.
z
)

NB. test create/erase locale on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task 4
)

NB. run & open the futures results
; s1''

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales

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
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task 5
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
z=. ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task 6
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
]&.> NX (t1 t.'')"0 [ i.TASK           NB. start task 7
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
]&.> NX (t1 t.'')"0 [ i.TASK           NB. start task 8
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
z=. ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task 9
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
]&.> NX (t1 t.'')"0 [ i.TASK           NB. start task 10
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
z=. ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task 11
for_t. i.TASK do.
 4!:55 ('noun_',(":t))~             NB. erase global noun
 4!:55 <'noun_',(":t)               NB. erase name of name list
end.
)

NB. run & open the futures results
; s1''

(271828) 18!:55 :: 1: "0 -.&' '&.> <"1 'loc',~"1 'f',"1 ": ,. i.TASK*STRIDE   NB. clear all locales
18!:55 [ 18!:1 [1                   NB. erase residual numbered locales in each task

1: 0!:_1 '$'   NB. skip this test pending design
NB. x is number of iterations
NB. y is a task number
NB. result is always 1
NB. task 0 repeatedly delete locale; task 1 repeatedly accesses them
t1=: 4 : 0"_ 0
if. y=0 do.
  while. 0<t1done do.
   (271828) 18!:55 ;:'l1'
   'a_l1_ a_l2_ b_l1_ b_l2_ ' =: 1 1 5 5
  end.
  1
else.
  for_xno. i.x do.
   a + b
  end.
  t1done=: <:t1done
  1
end.
)

s1=: 3 : 0
 100 (t1 f. t.'')"0 y       NB. start task 14
)

'a_l1_ a_l2_ b_l1_ b_l2_ ' =: 1 1 5 5
(;:'l1 l2') 18!:2 <'base'
4!:55 ;:'a b'
4!:55 ;:'a b'
t1done=: 10
]&> s1 0, 10#1
NB.$    end of skip

(<'z') 18!:2 <'base'
(271828) 18!:55 ;:'l1 l2'

4!:55 ;:'NX STRIDE TASK delth s1 t1 t1done'

epilog''

