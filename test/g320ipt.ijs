NB. x,y Append in place timing tests ------------------------------------

ss   =: +/ @: *:
rsq  =: [: -. ss@(- +/ % #)@[ %~ ss@:-

f=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. z,i
 end.
)

(i. -: f)"0 ] 2 10?@$2e3

x =: 800 * 2^i.8
y =: timer 'f ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
threshold < y rsq y1

load 'jmf'
createjmf_jmf_ 'mmf';4e6
map_jmf_ 'q';'mmf'

g=: 3 : 0
 q=: i.0
 for_i. i.y do.
  q=: q,i
 end.
)

(i. -: g)"0 ] 2 10?@$2e3

x =: 800 * 2^i.8
y =: timer 'g ',"1 ":,.x
y1=: (1,.x) +/ .*y %. 1,.x
threshold < y rsq y1

0 = unmap_jmf_ 'q'
1!:55 <'mmf'


4!:55 ;:'f g q rsq ss x y y1'


