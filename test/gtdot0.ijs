prolog './gtdot0.ijs'
NB. T. t. ------------------------------------------------------------------

NB. 7!:0

0!:_1`1:@.(9!:56'pyxes') '?'   NB. skip if not pyxes

NB. **************************************** threads & tasks **********************************
NB. pyxes only

NB. wait until there are y waiting threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}

delth''  NB. make sure we start with an empty system
N=: 1  NB. max # worker threads

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N = 1 T.''
wthr N
techo m1=: 7!:0''

1: > (]@> t.'')  < 2

techo m2=: 7!:0''
m2 < m1 + 1e7

NB.?  end of skip if not pyxes

epilog''
  
