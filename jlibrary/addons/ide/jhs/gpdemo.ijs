NB. gnuplot demos

require 'numeric trig'

gphelp=: 0 : 0
requires JAL general/misc and math/misc (numeric and trig)
jhsshow popup may require being unblocked
term_canvas - mouse support - left click labels, right drag zoom
gpd0 to gpd4 create plot files

   'd0' gpd0 term_png                  NB. create png file
   'd0' gpd0 term_canvas               NB. create html file

   jhspng  'd0' gpd0 term_png          NB. display in session
   jhslink 'd1' gpd1 term_canvas       NB. link to file
   jhsshow 'd2' gpd2 term_canvas       NB. show file in popup

   'one' jhslink 'd0' gpd0 term_canvas NB. window named one
   'one' jhslink 'd1' gpd1 term_canvas

   'one' jhsshow 'd2' gpd2 term_canvas
   'one' jhsshow 'd3' gpd3 term_canvas

   'one' jhsshow 'd4' gpd4 term_canvas
   'one' jhsshow 'd4' gpd4 term_canvas
)

term_png             =: 'term png tiny size 400,200 xffffff'
term_canvas          =: gpcanvas 400 200;1;'plot'
term_canvas_mouseless=: gpcanvas 400 200;0;'plot'


gpd0=: 4 : 0
gpinit''
gpset y
gpset 'grid'
gpset 'title "sin(exp) vs cos(exp)"'
gpset 'xlabel "x-axis"'
gpset 'ylabel "y-axis"'
gpsetwith 'with lines title "sin(exp)", with lines title "cos(exp)"'
x gpplot  (;sin@^,:cos@^) steps _1 2 100
)

gpd1=: 4 : 0
wiggle=. 4
points=. 200
X=. (3 % <:points) * i.points
fn=. +/@((0.9&^) * cos@((3&^ * (+/&X))))@i.
XY=. fn wiggle
gpinit''
gpset y
gpsetwith 'with lines'
x gpplot X;XY
)

gpd2=: 4 : 0
xd=. range _3 3 0.2
yd=. range _3 3 0.2
zd=. sin xd +/ sin yd
CP=. xd;yd;zd
gpinit''
gpset y
gpset 'title "sin(x+sin(y))"'
gpset 'parametric'
gpsetwith 'with lines'
gpsetsurface 1
smoutput 'asdf'
x gpplot CP
)

gpd3=: 4 : 0
xd=. range _3 3 0.1
yd=. range _3 3 0.1
SP=. xd;yd;(sin xd) */ sin yd
gpinit''
gpset y
gpset 'title "sin(x)*sin(y)) contour plot"'
gpset 'parametric;contour;cntrparam levels 20;view 0,0,1;nosurface'
gpsetwith 'with lines'
gpsetsurface 1
x gpplot SP NB. surface plot
)

gpd4=: 4 : 0
gpinit''
gpset y
gpsetwith'with lines'
x gpplot 10?10
)
