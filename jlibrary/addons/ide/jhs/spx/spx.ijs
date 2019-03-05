0 : 0
spx does managed execution of any script
you can step through or jump around

this script demonstrates spx features
tutorial scripts can use them to advantage
they are not required and spx can manage any script

next step opens this script - put it in another window
so you can read along as spx steps through
)

edit'~addons/ide/jhs/spx/spx.ijs'

require'plot'

NB. : to ) lines display in a single advance
f=: 3 : 0
*:y
)

0 : 0
0 : 0 by itself displays
the lines as comments
)

0 : 0
after any step you can experiment
enter a sentence using verb f
)

spx 0 NB. echo spx status

NB. spx 19 will execute line 19
NB. spx 1 19 will execute lines 1 to 19

NB.spxhr:
NB. previous line displays as horizontal rule
NB.  sequential lines starting with NB.
NB.   display in a single advance

0 : 0
=: replaces =. so defns are global
next advance is =: although the script line is =.
)
a=. 5

NB.spxhtml:<div><font style="color:red;font-size:32px;font-style:italic;">html, latex, and plots can be included</font></div>

NB.spxlatex:1+sin(x)

NB.spxlatex:^2+y^2+z^2

NB.spxlatex:\frac{1+sin(x)}{x^3}

plot 20?20

0 : 0
often scripts need to be stepped through in order
tutorial scripts can have independent sections
and you can jump to a section
)

NB.spxsection:aaa
a=. i.5

NB.spxsection:bbb
a=. i.23

spx':' NB. display sections

NB. run 1 of the previous lines to jump to that section

NB. noun spxhelp has spx info
spxhelp
