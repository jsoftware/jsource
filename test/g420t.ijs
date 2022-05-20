prolog './g420t.ijs'
NB. f/ timing tests -----------------------------------------------------

randuni''

ss =: +/ @: *:
rsq=: [: -. ss@(- +/ % #)@[ %~ ss@:-

f=: 3 : ',/(y,2 3)$''x'''
y=: timer 'f ',"1 ": ,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',/(y,2 3)$u:''x'''
y=: timer 'f ',"1 ": ,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',/(y,2 3)$10&u:''x'''
y=: timer 'f ',"1 ": ,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',/(y,2 3)$s:@<"0 ''x'''
y=: timer 'f ',"1 ": ,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',./(y,5 3)$''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',./(y,5 3)$u:''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',./(y,5 3)$10&u:''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',./(y,5 3)$s:@<"0 ''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

NB. ,./ does not call jtredstitch for rank-2 array
t=: (timer ',./x'),timer '|:x' [ x=: a.{~7001 131?@$#a.
1: THRESHOLD +. (1-threshold) > s=: (|@-/ % >./) t

t=: (timer ',./x'),timer '|:x' [ x=: adot1{~7001 131?@$#adot1
1: THRESHOLD +. (1-threshold) > s=: (|@-/ % >./) t

t=: (timer ',./x'),timer '|:x' [ x=: adot2{~7001 131?@$#adot2
1: THRESHOLD +. (1-threshold) > s=: (|@-/ % >./) t

t=: (timer ',./x'),timer '|:x' [ x=: sdot0{~7001 131?@$#sdot0
1: THRESHOLD +. (1-threshold) > s=: (|@-/ % >./) t

f=: 3 : ',.&.>/y$(5 3$''a'');5 2$''b'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',.&.>/y$(5 3$u:''a'');5 2$u:''b'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',.&.>/y$(5 3$10&u:''a'');5 2$10&u:''b'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',.&.>/y$(5 3$s:@<"0 ''a'');5 2$s:@<"0 ''b'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ';/(y,4 3)$''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ';/(y,4 3)$u:''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ';/(y,4 3)$10&u:''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ';/(y,4 3)$s:@<"0 ''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1


4!:55 ;:'adot1 adot2 sdot0 f rsq s ss t x y'
randfini''


epilog''

