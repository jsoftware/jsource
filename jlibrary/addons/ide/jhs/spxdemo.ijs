0 : 0
running spxdemo.ijs under spx demonstrates
how spx runs and displays script lines
)

0 : 0
read spxdemo.ijs as you step through it
JHS: enter open SPXFILE and click link
JQt: enter open SPXFILE
jconsole: open SPXFILE in your editor
)

0 : 0
spx can run any script for managed execution
tutorial scripts take advantage of spx features
)

NB. : to ) lines display in a single advance
f=: 3 : 0
*:y
)

0 : 0
0 : 0 by itself displays lines at the left
the lines are green in JHS
)

0 : 0
after any step you can experiment
enter a sentence using verb f
)

spx 0 NB. echo spx status

NB. spx 19 will execute line 19

NB. spx 1 19 will execute lines 1 to 19

0 : 'spx hr' NB. horizontal rule
NB. previous line displays as horizontal rule
NB.  line(s) starting with NB.
NB.   display in a single advance
NB. display at the left and are green in JHS

0 : 0
=: replaces =. so defns are global
next advance is =: although the script line is =.
)
a=. 5

NB. noun spxhelp has spx info