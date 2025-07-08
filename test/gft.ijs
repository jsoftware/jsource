prolog './gft.ijs'
NB. f. and performance --------------------------------------------------

tally=: #
plus =: +
sum  =: plus/
mean =: sum % tally
rev  =: |.

x=: 5e5?@$100
y=: 5e5?@$100

s=: 10 timer 'x #/. y'
t=: 10 timer 'x tally/.f. y'
THRESHOLD +. (1-*:threshold) > | (s-t)%s

s=: 10 timer '+/y'
t=: 10 timer 'sum f. y'
THRESHOLD +. (1-*:threshold) > | (s-t)%s

s=: 10 timer '(+/ % #) y'
t=: 10 timer 'mean f. y'
THRESHOLD +. (1-*:threshold) > | (s-t)%s

y=: (1e5 7?@$#a.){a.
s=: 10 timer '|."1 y'
t=: 10 timer 'rev"1 f. y'
THRESHOLD +. (1-*:threshold) > | (s-t)%s





epilog''

