NB. f/ timing tests -----------------------------------------------------

ss =: +/ @: *:
rsq=: [: -. ss@(- +/ % #)@[ %~ ss@:-

f=: 3 : ',/(y,2 3)$''x'''
y=: timer 'f ',"1 ": ,. x=: 2^8+i.12
threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ',./(y,5 3)$''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
threshold < y rsq y (] +/ .* %.) x^/0 1

t=: (timer ',./x'),timer '|:x' [ x=: a.{~7001 131?@$#a.
(1-threshold) > s=: (|@-/ % >./) t

f=: 3 : ',.&.>/y$(5 3$''a'');5 2$''b'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
threshold < y rsq y (] +/ .* %.) x^/0 1

f=: 3 : ';/(y,4 3)$''a'''
y=: timer 'f ',"1 ":,. x=: 2^8+i.12
threshold < y rsq y (] +/ .* %.) x^/0 1


4!:55 ;:'f rsq s ss t x y'


