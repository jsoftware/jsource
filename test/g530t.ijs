NB. a=: c}x,y,... ,:z timing tests --------------------------------------

f=: 3 : 0
 b=: ?5 7 11 13 17$2
 c=: ?($b)$3
 select. y 
  case. 'B' do.
   xx=: ?($b)$2
   yy=: ?($b)$2
   zz=: ?($b)$2
  case. 'I' do.
   xx=: _500+?($b)$1000
   yy=: _500+?($b)$1000
   zz=: _500+?($b)$1000
  case. 'D' do.
   xx=: o._500+?($b)$1000
   yy=: o._500+?($b)$1000
   zz=: o._500+?($b)$1000
  case. 'Z' do. 
   xx=: j./_500+?(2,$b)$1000
   yy=: j./_500+?(2,$b)$1000
   zz=: j./_500+?(2,$b)$1000
  end.
 i.0 0
)

g6=: 3 : 0  NB. good timing, boolean selection
 f y
 t0=: timer 'q=: (xx*b)+yy*-.b'
 t1=: timer 'yy=: b}yy,:xx'
 assert. q -: yy
 assert. THRESHOLD+. 5 > (t1-t0)%t0 NB. big timing difference allowed
 1
)

g7=: 3 : 0  NB. good timing, integer selection
 f y
 t0=: timer 'q=: (xx*0=c)+(yy*1=c)+zz*2=c'
 t1=: timer 'yy=: c}xx,yy,:zz'
 assert. q -: yy
 assert. THRESHOLD+. (1-threshold) > (t1-t0)%t0
 1
)

g6 "0 'BIDZ'
g7 "0 'BIDZ'


4!:55 ;:'b c f g6 g7 q t0 t1 xx yy zz'




