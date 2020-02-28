0 : 0
linux objdump

data also contains rodata 
)


getglobinit=: 3 : 0
d=. <;._2 fread'git/jsource/jsrc/je.h'
d=. ('/'~:;{.each d)#d
d=. ('#'~:;{.each d)#d
d=. (d i.each';'){.each d
d=. (-.;'('e.each d)#d
d=. dltb d
d=. (0~:;#each d)#d
d=. (d i: each ' ')}.each d
d=. (d i. each '['){.each d
d=. dltb each d
d=. /:~d
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


getglobinits=: 3 : 0
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
d=. d-.;:'MC if elif endif'
d=. /:~d
)

getglobals=: 3 : 0
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
