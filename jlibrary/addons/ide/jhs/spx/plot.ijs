0 : 0
there are several ways to visualize data:
 viewmat - table values as colors
 plot    - J native plot
 jd3     - javascript
 gnuplot - C standalone utility
 gc      - Google Charts web service 
 webgl   - javascript 3D interactive graphics
  
plots can:
 show inline in jijx session
 show in tabs (new or reused)
 show in apps with other html elements
 write a file in a common format (png/html/...)

run next line to open this script
   edit'~addons/ide/jhs/spx/plot.ijs'

there are independent sections - you can skip around
)
spx':' NB. run a line to skip to corresponding section

NB.spxsection:viewmat
require'viewmat'
viewmat 50 50$ 1 2 1
viewmat */~ i:9
viewmat #:(~:/&.#:@, +:)^:(<32) 1
jhslinkurl'code.jsoftware.com/wiki/Essays/Base_Spectrum' NB. link to Essay using viewmat

NB.spxsection:plot
jhslinkurl'code.jsoftware.com/wiki/Plot' NB. link to wiki plot doc
require'plot'
plotdef 'jijx';'plot';200 100 NB. attributes for subsequent plots
plot ?10$100
plot ?2 10$100
plotdef 'jijx';'plot';400 200
plot ?2 10$100
plotdef 'show';'plot';600 400 NB. show in tab named plot
plot ?2 10$100
load '~Demos/wdplot/plotdemos.ijs' NB. more than 50 demos
>5{.PLOTNAMES_jdplot_ NB. names of first 5 demos
plotdef 'jijx';'plot';600 400
plotdemos 0
plotdef 'show';'plot';600 400 NB. show in tab named plot
plotdemos 1
plotdemos 20
plotdemos 30
plotdef 'link';'plot';600 450    NB. link to plot
plotdemos 50
plotdef 'none';'plot';600 300    NB. create ~temp/plot.html
plotdemos 25
'plot' jhsshow '~temp/plot.html' NB. show plot in window plot
plotdef 'jijx';'';400 200
plotdemos 54

NB.spxsection:jd3
jhslinkurl'www.d3js.org' NB. link to D3 home page
jd3'help'
jd3'reset;type line;title My Data;legend "line one","line two","line three"'
'jd3;10 10'cojhs jd3x;?3 4$100
  
NB.spxsection:gnuplot
jhslinkurl'www.gnuplot.info'
0 : 0
these examples fail if gnuplot is not installed
linux installs with something like:
 sudo apt-get install gnuplot
see website for info for other systems

note: tab name (reuse a tab) is not the tab title
)
require'~addons/ide/jhs/gnuplot.ijs'
   
gpd0=: 4 : 0 NB. verb with gnuplot commands
gpinit''
gpset y
gpset 'grid'
gpset 'title "sin(exp) vs cos(exp)"'
gpset 'xlabel "x-axis"'
gpset 'ylabel "y-axis"'
gpsetwith 'with lines title "sin(exp)", with lines title "cos(exp)"'
x gpplot  (;sin@^,:cos@^) steps _1 2 100
)

small=: ' size 400,200 '
big=:   ' size 600,400 '

'd0' gpd0 term_png,small                  NB. create png file
jhspng  'plot' gpd0 term_png,small        NB. display in session
'plot'jhsshow 'plot' gpd0 term_png,small  NB. show in tab named plot
jhslink 'plot' gpd0 term_png,small        NB. link to file

'd0' gpd0 term_canvas,small               NB. create html file
'plot'jhsshow 'd2' gpd0 term_canvas,small NB. show file in tab named plot
jhslink 'd1' gpd0 term_canvas,small       NB. link to file
NB. currently gnuplot canvas is standalone and can not be displayed in jijx

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
jhspng  'plot' gpd1 term_png,small NB. display in session

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
x gpplot CP
)
jhspng  'plot' gpd2 term_png,big NB. display in session

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
jhspng  'plot' gpd3 term_png,big NB. display in session

gpd4=: 4 : 0
gpinit''
gpset y
gpsetwith'with lines'
x gpplot 10?10
)
jhspng  'plot' gpd4 term_png,small NB. display in session
   
NB.spxsection:gc
jhtml_jhs_'<div contenteditable="false"><a href="http://code.google.com/apis/chart/"  target="_blank">Google Charts</a></div>'
load'~addons/ide/jhs/jgcp.ijs'
jgc'help'  NB. plot info
jgcx''     NB. examples
NB. study the code that generates the charts
NB. note how reset clears state and additional commands add to state

NB.spxsection:webgl
rundemo_jhs_ 12

