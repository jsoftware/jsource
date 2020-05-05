0 : 0
linux objdump

data also contains rodata 
)

NB. bss or data or rodata
getmap=: 3 : 0
d=. <;._2 fread'jbld/j64/bin/globals.txt'
d=. (;+./each (<y) E. each d)#d
d=. dltb each ((dltb each d)i:each' ')}.each d
d=. ('.'~:;{.each d)#d
d=. ('_'~:;{.each d)#d
d=. /:~d
)

report1=: 3 : 0
echo 'bss names that are not set by globinit'
>(getmap'bss')-.getglobinit''
)

report2=: 3 : 0
echo 'data names in je.h that should be marked constant (simple so do not need to be in globinit)'
(getmap'data')-.(getmap'data')-.getglobinit''
)

reportbss=: 3 : 0
echo 'bss names not in rodata'
(getmap'bss')-.getmap'rodata'
)

reportdata=: 3 : 0
echo 'data names not in rodata'
(getmap'data')-.getmap'rodata'
)


report3=: 3 : 0
echo 'data names that are not in rodata'
(getmap'data')-.getmap'rodata'
)

globinit_exclude=: 'jt rifvs str defined sizeof MC if elif endif makename meminit memset pinit getCpuFeatures '
globinit_exclude=: globinit_exclude,' adbreak adbreakr prioritytype typepriority typesizes qpf cpuInit' 
globinit_exclude=: globinit_exclude,' num '

getglobinit=: 3 : 0
d=. <;._2 fread'git/jsource/jsrc/i.c'
a=. 'B jtglobinit(J jt)'
i=. ((#a){.each d)i.<a
d=. }.i}.d
i=. (;{.each d)i.'}'
d=. _3}.i{.d
d=. d rplc each <'''/''';'''x'''
d=. (d i. each <'/'){.each d NB. lazy - could be made better
d=. ~.;;:each d
d=. ((;{.each d)e.26}.Alpha_j_)#d NB. assume globals all start with lowercase letter
d=. (1~:;#each d)#d               NB. assume no globals have a single letter
d=. (-'.'=;{:each d)}.each d     NB. drop trailing .
d=. d-.;:globinit_exclude
d=. /:~d,<'numv' NB. num -> numv
)

getglobal=: 3 : 0
d=. <;._2 fread'git/jsource/jsrc/j.c'
a=. '// globals start '
i=. ((#a){.each d)i.<a
d=. }.i}.d
a=. '// globals end '
i=. ((#a){.each d)i.<a
d=. i{.d
d=. ('/'~:;{.each d)#d
d=. dltb each d
d=. (0~:;#each d)#d
d=. (;d i. each ' ')}.each d
d=. (;d i.each '['){.each d
d=. (;d i.each '='){.each d
d=. (;d i.each ';'){.each d
d=. dltb each d
d=. /:~d
)

repa=: 3 : 0
echo 'getglobinit-.getglobal'
>(getglobinit'')-.getglobal''
)

repb=: 3 : 0
echo 'getglobal-getglobinit'
>(getglobal'')-.getglobinit''
)


bad1=: 3 : 0
echo 'bss - uninitialized values'
>getmap'bss'
)

bad2=: 3 : 0
echo 'data names that are not rodata'
(getmap'data')-.getmap'rodata'
)


NB. 'static const' or 'static'
repconst=: 3 : 0
fns=. 1 dir 'git/jsource/jsrc/*.c'
r=. ''
for_n. fns do.
 d=. deb <;._2 fread n
 b=. ;(<y)=(#y){.each d
 d=. b#d
 d=. (#y)}.each d
 d=. (-.;'('e. each d)#d NB. ignore fns
 i=. ;<./each d i. each <'[='
 d=. i{.each d
 d=. deb each d rplc each <'*';' '
 i=. ;d i. each ' '
 d=. i}.each d
 if. 0 e. ;#each d do. echo 'file with empty declaration: ',;n end.
 r=. r,d
end. 
r=. dltb each r
r=. (0~:;#each r)#r
)

