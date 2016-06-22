NB. JHS Google Charts
coclass'jgcp'

GC=: 'chart.apis.google.com'

NB. z defines - user api
jgc_z_=:      gc_jgcp_      NB. edit chart state
jgcimg_z_=:   gcimg_jgcp_   NB. get html img tag
jgcchart_z_=: gcchart_jgcp_ NB. get chart?&cht...
jgcfile_z_=:  gcfile_jgcp_  NB. get png file 
jgcx_z_=:     gcx_jgcp_     NB. run examples

gc=: 3 : 0
'' jgc y
:
gcd y
gcc each <;._2 x,' '
i.0 0
)

NB. x... nouns
xline=:       '&cht=lc&chs=300x100&chds=<MIN>,<MAX>&chxt=x,y&chxr=1,<MIN>,<MAX>'
xlinexy=:     '&cht=lxy&chs=300x100&chds=<MIN>,<MAX>&chxt=y,x&chxr=0,<YMIN>,<YMAX>|1,<MIN>,<MAX>'
xsticklinexy=:'&cht=lxy&chs=300x100&chds=<MIN>,<MAX>&chxt=y,x&chxr=0,<YMIN>,<YMAX>|1,<MIN>,<MAX>&chm=v,FF0000,0,::1,2'
xpie=:        '&cht=p&chs=300x100&chds=0,<MAX>'
xpie3=:       '&cht=p3&chs=300x100&chds=0,<MAX>'

NB. process blank delimited commands
gcc=: 3 : 0 
if. 'reset'-:y do.
   gcurl=: ''
elseif. 'help'-:y do.
   smoutput help
elseif. 'lc'-:y do.
   gcurl=: '&cht=lc&chs=400x200&chds=<MIN>,<MAX>&chxt=x,y&chxr=1,<MIN>,<MAX>'
elseif. ('p'-:y)+.'p3'-:y do.
   gcurl=: '&cht=',y,'&chs=400x200&chds=0,<MAX>'
elseif. 'show'-:y do.
   jhtml gcimg''
elseif. '&ch'-:3{.y do.
   gcurl=: gcurl,y   NB. might be nice to remove duplicates
elseif. 'x'={.y do.
   (y,' is not a noun') assert 0=nc<y
   gcurl=: gcurl,".y
elseif. ''-:y do.
elseif. 1 do.
   ('unknown command: ',y)assert 0
end.
i.0 0
)

NB. format data - uses least space efficient t format
gcd=: 3 : 0
if. ''-:y do.
elseif. 2=3!:0 y do.
 smoutput help
elseif. 32=3!:0 y do.
 NB. assume dead simple 1 2 3;4 5 6[;x2;y2...]
 gcdata=: y
 gcdata=:  ;(;(-:#gcdata)#<0 1)#gcdata
 GCYMIN=: '_-'charsub":<./gcdata
 GCYMAX=: '_-'charsub":>./gcdata
 gcdata=: scale gcdata
 gcdata=: ,(scale each(;(-:#y)#<1 0)#y),.((#gcdata)$((#gcdata)%(-:#y)){.1)<;.1 gcdata
 GCFDATA=: }:'e:',;',',~each enc each gcdata
 GCMIN=: '_-'charsub":<./,>{.y
 GCMAX=: '_-'charsub":>./,>{.y
elseif. 1 do.
 gcdata=: >(1=$$y){y;,:y
 GCFDATA=: }:'e:',,',',~"1 enc "1 scale gcdata
 GCMIN=: '_-'charsub":<./,y
 GCMAX=: '_-'charsub":>./,y
 GCYMIN=: GCYMAX=: 0
end.
i.0 0
)

ac=: '-.',~((65+i.26),(97+i.26),48+i.10){a.

enc=: 3 : 0
;ac{~64 64#:y
)

scale=: 3 : 0
a=. <./,y
z=. >./,y
<.0.5+4095*(y-a)%z-a
)

NB. get last y command arg
gca=: 3 : 0
c=. <;.1 gcurl
ch=. }.each(c i.each'='){.each c
i=. ch i:<y
if. i=#c do. '' else. (2+#>i{ch)}.>i{c end.
)

NB. return html img tag
gcimg=: 3 : 0
t=. 'http://',GC,'/',gcchart''
'<img width=',(":GCW),'px height=',(":GCH),'px src="',t,'"></img>'
)

NB. return chart?....
gcchart=: 3 : 0
'GCW GCH'=: wh=. 2{.0".'x 'charsub gca'chs'
'chs error' assert (wh>:25),(wh<:1000),300000>:*/wh
'cht missing' assert #gca'cht'
t=. 'chart?',gcurl,'&chd=',GCFDATA
t=. t hrplc_jhs_ 'MIN MAX YMIN YMAX COLORS RED BLUE';GCMIN;GCMAX;GCYMIN;GCYMAX;COLORS;RED;BLUE
if. 2000<#t do. smoutput 'too much data (chart url is too large)' end.
t
)

NB. return chart file data (png)
gcfile=: 3 : 0
>{:jwget GC;gcchart''
)

NB. limited support
NB.  title option
NB.  xline and xlinexy chart types
NB.  data vector
NB.  data matrix
NB.  xvals;yvals
NB. needs support for
NB.  sin=: 1&o.
NB.  cos=: 2&o.
NB.  x=: 0.1 * i: 21
NB.  gcplot x;>(sin x);(cos x)
gcplot_z_=: 3 : 0
''gcplot y
:
t=. <;._2 x,';'
stickline=. 'stick,line'-:>{.t
t=. stickline}.t
b=. (<'title ')=6{.each t
if. 0~:#;(-.b)#t do. smoutput 'unsupported options: ',x end.
if. (32=3!:0 y)*.2=#y do.
 r=.$&.>y
 if. 1 2-:;$&.>r do.
  y=.,(,.(>#&.>1{y)#0{y),.><"1&.>1{y
 end.
elseif. (32=3!:0 y)*.2~:#$>y do.
  smoutput 'unsupported data complexity'
 y=. >{:y
end.
t=. 6}.;b#t
type=. >(32=3!:0 y){'xline';'xlinexy'
if. stickline *. type-:'xlinexy' do. type=. 'xsticklinexy' end.
('reset ',type,' &chtt=',t,' show')jgc y
)

rgb=:3 : 0
(,16 16#:y){'0123456789ABCDEF'
)

AQUA=:    rgb   0 255 255
BLUE=:    rgb   0   0 255
BROWN=:   rgb 192 128   0
FUCHSIA=: rgb 255   0 255
GRAY=:    rgb 128 128 128
GREEN=:   rgb   0 128   0
LIME=:    rgb   0 255   0
MAROON=:  rgb 128   0   0
NAVY=:    rgb   0   0 128
OLIVE=:   rgb 128 128   0
PURPLE=:  rgb 128   0 128
RED=:     rgb 255   0   0
SILVER=:  rgb 192 192 192
TAN=:     rgb 210 180 140
TEAL=:    rgb   0 128 128
YELLOW=:  rgb 255 255   0
BLACK=:   rgb   0   0   0
WHITE=:   rgb 255 255 255

COLORS=: }.;',',._6[\BLUE,RED,GREEN,PURPLE,FUCHSIA,OLIVE,TEAL,YELLOW,TAN,AQUA,BROWN,GRAY

help=: 0 : 0
See code.google.com/apis/chart for Google Charts info.

chart?... in a url defines a plot.
The url from GC returns a png file.
   jgcx'' NB. see examples

jgc manages chart state.
   jgc 10?10  NB. numeric list or table
   jgc ''     NB. leaves data as is
   jgc 'help' NB. displays this help

   '...'jgc y NB. x is blank delimited commands
   'reset xline &chs=400x200 show'jgc 10?1000

jgc x command summary:
 reset  clears chart url (data unchanged)
 show   adds img tag with url to ijx
 x...   noun x... added to url
 xline  line chart
 xpie   pie chart
 xpie3  pie 3d chart
 &ch..  GC commands

Use + instead of blank in &ch.. commands!

   jgcchart'' NB. get chart?...
   jgcimg''   NB. get img tag
   jgcfile''  NB. get png file


)

examples=: 0 : 0
gcplot ?2 10$1000

'reset xline show'jgc 10?1000

'reset xline &chtt=My+Title show'jgc ?2 10$1000

'show'jgc ?3 10$1000

'&chco=<COLORS>'jgc''      NB. default series colors
'&chdl=mew|bark|hiss'jgc'' NB. data legend
'&chxl=0:|a|b|c|d|e|f|g|h|i|j|1:|lo|med|hi'jgc''
'show'jgc''

'reset xpie3 show'jgc 20+3?100

'&chco=<BLUE>'jgc''
'&chdl=one|two|three'jgc'' NB. data legend
'&chl=one|two|three'jgc''  NB. label
'show'jgc''

'reset &cht=tx&chs=200x50&chl=x=\frac{-b\pm\sqrt{b^2-4ac}}{2a} show'jgc ''

'reset &cht=map:fixed=40,-10,60,10&chs=88x140&chld=GB|GB-LND&chco=676767|FF0000|0000BB&chf=bg,s,c6eff7 show'jgc''

'reset &cht=v&chs=200x100&chco=FF6342,ADDE63,63C6DE&chdl=A|B|C show'jgc 100 80 60 30 30 10

'reset xline &chs=200x50 show'jgc 20?1000

d1=. jgcimg'reset xline &chtt=pie+on+right+if+room'jgc 10?1000
d2=. jgcimg'reset xpie3'jgc 20+4?100
jhtml d1,d2
)

gcx=: 3 : '0!:101 examples'


