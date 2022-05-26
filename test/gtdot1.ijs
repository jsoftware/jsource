prolog './gtdot1.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. j904 64-bit only

TASK=: <: 9!:56'maxtasks'
TASK1=: 4 ] 12     NB. 12 crash   limit error of 1!:20

1: 0&T."1^:(0 < #) ''$~ (0 >. TASK-1 T. ''),0

STRIDE=: 3000                 NB. stride between tasks
NX=: 20
MINLEN=: 1e5
MLEN=: <.MINLEN%2

1: (2!:0 :: 1:)^:IFUNIX 'rm -rf ',jpath '~temp/tdot'
1: (1!:55 ::1:)^:IFWIN ((jpath'~temp/tdot/')&,)&.> {."1[ 1!:0 jpath '~temp/tdot/*' 
mkdir_j_ jpath '~temp/tdot'

NB. test parallel erase/write
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then erased
t1=: 4 : 0"_ 0
NB. write file
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
for_xno. i.x do.
 f=. 'dat',~ jpath '~temp/tdot/f',":y+xno        NB. unique file name for each task
 l=. MINLEN+?MLEN              NB. file length at least MINLEN
 (<f) 1!:2~ a=. a.{~?l#256     NB. write random data
 1!:55 <f                      NB. erase file
end.
1
)

NB. test erase/write on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel write/read
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
t1=: 4 : 0"_ 0
NB. write file
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
z=. 1                          NB. initialize result
for_xno. i.x do.
 f=. 'dat',~ jpath '~temp/tdot/f',":y+xno        NB. unique file name for each task
 l=. MINLEN+?MLEN              NB. file length at least MINLEN
 (<f) 1!:2~ a=. a.{~?l#256     NB. write random data
 z=. z *. a -: 1!:1 <f         NB. assert each read is successful
end.
z
)

NB. test write/read on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''


NB. test parallel read directroy
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
t1=: 4 : 0"_ 0
NB. read directory
for_xno. i.x do.
 1: 1!:0 '*'                   NB. read directory
end.
)

NB. test read directory on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
10 (t1 t.'')"0 [ i.TASK          NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel read/write/list
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
NB. some threads list directory
t1=: 4 : 0"_ 0
assert. x<STRIDE NB. check for overlapping file names between tasks
z=. 1                          NB. initialize result
if. 0=2|y do.                  NB. write file even y
 y=. STRIDE * y                NB. offset of file name
NB. write file
 for_xno. i.x do.
  f=. 'dat',~ jpath '~temp/tdot/f',":y+xno       NB. unique file name for each task
  l=. MINLEN+?MLEN             NB. file length at least MINLEN
  (<f) 1!:2~ a=. a.{~?l#256    NB. write random data
  z=. z *. a -: 1!:1 <f        NB. assert each read is successful
 end.
else.
NB. list directory for odd y
 z=. z *. 1: 1!:0 '*'          NB. ignore actual result
end.
z
)

NB. test read/write/list on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel append
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat appended around 100KB 100 times
t1=: 4 : 0"_ 0
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
NB. append file
for_xno. i.x do.
 f=. 'dat',~ jpath '~temp/tdot/f',":y+xno        NB. unique file name for each task
 l=. MINLEN+?MLEN              NB. file length at least MINLEN
 1!:55 ::1: <f                 NB. erase file
 for_i. 100 do.                NB. append 100 times
  (<f) 1!:3~ (MINLEN+?MLEN)#'a'    NB. append MLEN character each time
 end.
end.
1
)

NB. test append on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK        NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel file size
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
t1=: 4 : 0"_ 0
NB. append file
for_xno. i.x do.
 s=. 1!:0 'f*dat'              NB. all files in directory
 1: 1!:4 {."1 s                NB. file size of all files, ignore result
end.
1
)

NB. test file size on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
10 (t1 t.'')"0 [ i.TASK          NB. start task
)

NB. run & open the futures results
; s1''

1: {{)n  NB. deleting the next 2 tests because they test the OS more than JE, and take 1 minute
NB. test parallel create directory
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each directory with name f([0..x-1]+STRIDE*y)dir created
t1=: 4 : 0"_ 0
NB. create directory
assert. x<STRIDE NB. check for overlapping file names between tasks
if. 0=2|y do.                  NB. create directory even y
 y=. STRIDE * y                NB. offset of file name
 for_xno. i.x do.
  f=. 'dir',~ jpath '~temp/tdot/f',":y+xno       NB. unique directory name for each task
  1: 1!:5 <f                   NB. create directory
 end.
else.
NB. list directory for odd y
 1: 1!:0 '*'                   NB. ignore actual result
end.
)

NB. test create directory on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
1!:55 :: 1: "0 -.&' '&.> <"1 'dir',~"1 '~temp/tdot/f',"1 ": ,. i.TASK*STRIDE   NB. clear all directories
3 (t1 t.'')"0 [ i.3          NB. start task
)

NB. run & open the futures results
; s1''

NB. test parallel indexed write/read
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
t1=: 4 : 0"_ 0
NB. write file
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
z=. 1                          NB. initialize result
for_xno. i.x do.
 f=. 'dat',~ jpath '~temp/tdot/f',":y+xno        NB. unique file name for each task
 l=. ?1000<.MINLEN             NB. file length at least MINLEN
 p=. ?MINLEN                   NB. random index position
 (f;p) 1!:12~ a=. l#'a'        NB. write l#'a' at position p
 z=. z *. a -: 1!:11 f;p,l     NB. assert each read is successful
end.
z
)

NB. test indexed write/read on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
1!:55 :: 1: "0 -.&' '&.> <"1 'dat',~"1 '~temp/tdot/f',"1 ": ,. i.TASK*STRIDE   NB. clear all files
]&.> 5 (t1 t.'')"0 [ i.3  NB. start task, too many files limit error if x is too large
)

NB. run & open the futures results
; s1''
}}  NB. end of commented-out tests


NB. test parallel file open/close
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
t1=: 4 : 0"_ 0
NB. write file
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
z=. 1                          NB. initialize result
fs=. ''                        NB. file name list
fns=. 0$0                      NB. file number list
for_xno. i.x do.
 fs=. fs, <f=. winpathsep^:IFWIN 'dat',~ jpath '~temp/tdot/f',":y+xno    NB. unique file name for each task
 l=. MINLEN+?MLEN              NB. file length at least MINLEN
 1!:55 ::1:<f                  NB. erase file if exist
 fns=. fns, fn=. 1!:21 <f      NB. open file
 (fn) 1!:2~ a=. a.{~?l#256     NB. write random data
 z=. z *. a -: 1!:1 fn         NB. assert each read is successful
end.
assert. z
z=. z *. *./ ((<"0 fns),.fs) e. 1!:20''    NB. file name and list should agree with 1!:20
z=. z *. *./ 1!:22 fns                     NB. close all files
z=. z *. *./ 0= ((<"0 fns),.fs) e. 1!:20'' NB. file name and list should not be inside 1!:20
z
)

NB. test file open/close on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK1  NB. start task, too many files limit error if x it too large
)

NB. run & open the futures results
; s1''


NB. test parallel file indexed read/write
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
t1=: 4 : 0"_ 0
NB. write file
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
z=. 1                          NB. initialize result
fs=. ''                        NB. file name list
fns=. 0$0                      NB. file number list
for_xno. i.x do.
 fs=. fs, <f=. winpathsep^:IFWIN 'dat',~ jpath '~temp/tdot/f',":y+xno    NB. unique file name for each task
 l=. ?MINLEN                   NB. file length at least MINLEN
 p=. ?MINLEN                   NB. random index position
 1!:55 ::1:<f                  NB. erase file if exist
 fns=. fns, fn=. 1!:21 <f      NB. open file
 (fn,p) 1!:12~ a=. a.{~?l#256  NB. write random data
 z=. z *. a -: 1!:11 fn,p,l    NB. assert each read is successful
end.
assert. z
z=. z *. *./ ((<"0 fns),.fs) e. 1!:20''    NB. file name and list should agree with 1!:20
z=. z *. *./ 1!:22 fns                     NB. close all files
z=. z *. *./ 0= ((<"0 fns),.fs) e. 1!:20'' NB. file name and list should not be inside 1!:20
z
)

NB. test file indexed read/write on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
]&.> NX (t1 t.'')"0 [ i.TASK1  NB. start task, too many files limit error if x it too large
)

NB. run & open the futures results
; s1''

NB. test parallel getcwd/chdir directory
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each directory with name f([0..x-1]+STRIDE*y)dir created
NB. !!! current directory is per process
t1=: 4 : 0"_ 0
NB. getcwd/chdir
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
a=. 1!:43''                    NB. original directory
for_xno. i.x do.
 f=. 'dir',~ jpath '~temp/tdot/f',":y+xno        NB. unique directory name for each task
 1!:5 <f                       NB. create directory
 6!:3]0.001
 1!:44 f                       NB. chdir new directory
 6!:3]0.001
 1!:43''                       NB. getcwd, result may NOT be f
 6!:3]0.001
 1!:44 a                       NB. restore to original directory
 6!:3]0.001
 1!:55 <f                      NB. erase directory
end.
1
)

NB. test getcwd/chdir on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
1!:55 :: 1: "0 -.&' '&.> <"1 'dir',~"1 '~temp/tdot/f',"1 ": ,. i.TASK*STRIDE   NB. clear all directories
10 (t1 t.'')"0 [ i.TASK          NB. start task
)

NB. test parallel file lock
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
t1=: 4 : 0"_ 0
NB. write file
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
z=. 1                          NB. initialize result
fs=. ''                        NB. file name list
fns=. 0$0                      NB. file number list
for_xno. i.x do.
 fs=. fs, <f=. winpathsep^:IFWIN 'dat',~ jpath '~temp/tdot/f',":y+xno       NB. unique file name for each task
 l=. ?MINLEN                   NB. file length at least MINLEN
 p=. ?MINLEN                   NB. random index position
 1!:55 ::1:<f                  NB. erase file if exist
 fns=. fns, fn=. 1!:21 <f      NB. open file
 (fn,p) 1!:12~ a=. a.{~?l#256  NB. write random data
 z=. z *. a -: 1!:11 fn,p,l    NB. assert each read is successful
 1!:31 fn, p, l                NB. lock file fn at position p for length l
 assert. (fn,p,l) e. 1!:30 ''  NB. check file entry in lock table
 1!:32 fn, p, l                NB. unlock file
 assert. (fn,p,l) -.@e. 1!:30 ''   NB. file entry shouldn't exist in lock table
end.
assert. z
z=. z *. *./ ((<"0 fns),.fs) e. 1!:20''    NB. file name and list should agree with 1!:20
z=. z *. *./ 1!:22 fns                     NB. close all files
z=. z *. *./ 0= ((<"0 fns),.fs) e. 1!:20'' NB. file name and list should not be inside 1!:20
z
)

NB. test file lock on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
NB. file lock only available for windows
if. IFWIN do.
 ]&.> NX (t1 t.'')"0 [ i.TASK1    NB. start task, too many files limit error if x it too large
else.
 1
end.
)

NB. run & open the futures results
; s1''

NB. test parallel file attribute/permission
NB. x is number of iteration in each task
NB. y is a task number
NB. result is always 1
NB. each file with name f([0..x-1]+STRIDE*y)dat written with around 1MB and then read
t1=: 4 : 0"_ 0
NB. write file and check attribute/permission
assert. x<STRIDE NB. check for overlapping file names between tasks
y=. STRIDE * y                 NB. offset of file name
for_xno. i.x do.
 f=. 'dat',~ jpath '~temp/tdot/f',":y+xno        NB. unique file name for each task
 l=. ?MINLEN                   NB. file length at least MINLEN
 (<f) 1!:2~ a=. a.{~?l#256     NB. write random data
 b=. 1!:6 <f                   NB. query file attribute
 b 1!:6 <f                     NB. set file attribute
 b=. 1!:7 <f                   NB. query file permission
 b 1!:7 <f                     NB. set file permission
 1!:55 <f                      NB. erase file
end.
)

NB. test file attribute/permission on multiple threads
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
NB. 1!:6/7 only available for windows
if. IFWIN do.
 ]&.> NX (t1 t.'')"0 [ i.TASK       NB. start task
else.
 1
end.
)

NB. run & open the futures results
; s1''

NB. x is number of iterations
NB. y is a task number
NB. result is always 1
NB. task 0 repeatedly changes the path for base locale; task 1 repeatedly accesses a name
t1=: 4 : 0"_ 0
if. y=0 do.
  while. -. t1done do.
    (<'base') 18!:2~ (#~ 2 2 2 #: [: >: [: ? 7:) ;: 'l1 l2 l3'
  end.
else.
  while. (,<,'z') -: 18!:2 <'base' do. end.  NB. wait for task 0 to start 
  for_xno. i.x do.
   assert. 1 0 1 0 1 0 -: (a,b,a,b,a,b) < 2.5
  end.
  t1done__=:1
end.
1
)

NB. test name access while path is being changed
NB. Result is list of results from each thread
s1=: 3 : 0
 ]&.> 5e5 (t1 f. t.'')"0  y       NB. start task
)

t1done=: 0
'a_l1_ a_l2_ a_l3_ b_l1_ b_l2_ b_l3_' =: i. 6
; }. shrxno =: 1 ; s1 0 1
t1done=: 0
NB. scaf test ; }. shrxno =: 1 ; s1 0 0 1 1


NB. x is number of iterations
NB. y is a task number
NB. result is always 1
NB. task 0 repeatedly changes globals; task 1 repeatedly accesses them
t1=: 4 : 0"_ 0
if. y=0 do.
  while. -. t1done do.  NB. as long as other is running
   'a_l1_ b_l1_ a_l1_ b_l1_ a_l1_ b_l1_ a_l1_ b_l1_ a_l1_ b_l1_ ' =: (10 $ 0 3) + ?0
   4!:55 <'a_l1_' 
   'a_l1_ b_l1_ a_l1_ b_l1_ a_l1_ b_l1_ a_l1_ b_l1_ a_l1_ b_l1_ ' =: (10 $ 0 3) + ?0
   4!:55 <'b_l1_' 
  end.
  1
else.
  res =. $0
  for_xno. i.x do.
   res =. res ,  1 0 1 0 1 0 -: (a,b,a,b,a,b) < 2.5
  end.
  t1done=: 1
  *./ res
end.

)

NB. test name access while name is being changed/deleted
NB. Nilad.  Result is list of results from each thread
s1=: 3 : 0
 ]&.> 5e5 (t1 f. t.'')"0 y       NB. start task
)

'a_l1_ a_l2_ b_l1_ b_l2_ ' =: 0 1 3 4
(;:'l1 l2') 18!:2 <'base'
t1done=: 0
; }. shrxno =: 2 ; s1 0 1
t1done=: 0
NB. scaf test ; }. shrxno =: 2 ; s1 0 0 1 1

(<'z') 18!:2 <'base'
18!:55 ;:'l1 l2 l3'



1: (2!:0 :: 1:)^:IFUNIX 'rm -rf ',jpath '~temp/tdot'
1: (1!:55 ::1:)^:IFWIN ((jpath'~temp/tdot/')&,)&.> {."1[ 1!:0 jpath '~temp/tdot/*' 

4!:55 ;:'MINLEN MLEN NX STRIDE TASK1 TASK s1 t1 t1done shrxno '
epilog''

