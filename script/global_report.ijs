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