prolog './gmbxx1w.ijs'
NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. 1!: -----------------------------------------------------------------

q=: x=: <'asdf'
t=: ": ?20$1e6
t 1!:2 x

(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (1!:1 x)      -: 1!:1 q

(mbxcheck_jmf_ q), 1 [ t 1!:2 q

(mbxcheck_jmf_ q), 1 [ t 1!:3 q

(mbxcheck_jmf_ q), (1!:4 x)      -: 1!:4 q

q=: x=: 'asdf';2 26
(mbxcheck_jmf_ q), (1!:11 x)     -: 1!:11 q

q=: x=: 'asdf';3
y=: u:'foo upon thee'
(y 1!:12 x)   -: y 1!:12 q
q=: x=: 'asdf';3,2*#y
y             -: (6&u:) 1!:11 q
mbxcheck_jmf_ q

q=: <'asdf'
(mbxcheck_jmf_ q), 1 [ 1!:55 q


NB. 3!: -----------------------------------------------------------------

q=: x=: <"0 (?2 12$2){(<5!:2 <'g'),< i.2 3 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), q             -: 3!:2 (3!:1) q
(mbxcheck_jmf_ q), x             -: 3!:2 (3!:1) q
(mbxcheck_jmf_ q), q             -: 3!:2 (3!:3) q
(mbxcheck_jmf_ q), x             -: 3!:2 (3!:3) q
q=: x=: <"0 <"0 ?2 3 4$1e6 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), q             -: 3!:2 (3!:1) q
(mbxcheck_jmf_ q), x             -: 3!:2 (3!:1) q
(mbxcheck_jmf_ q), q             -: 3!:2 (3!:3) q
(mbxcheck_jmf_ q), x             -: 3!:2 (3!:3) q


NB. 4!: -----------------------------------------------------------------

q=: x=: (u:&.>) ;:'g mean junkfoo'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (4!:0 x)      -: 4!:0 q

q=: x=: (u:&.>) ;:'g mean junkfoo'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (4!:4 x)      -: 4!:4 q

q=: x=: t=: (u:&.>) ;:'junkfoo t'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), 1 1 -: 4!:55 q


NB. 5!: -----------------------------------------------------------------

q=: x=: <'mean'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (5!:1 x)      -: 5!:1 q
(mbxcheck_jmf_ q), (5!:2 x)      -: 5!:2 q
(mbxcheck_jmf_ q), (5!:4 x)      -: 5!:4 q
(mbxcheck_jmf_ q), (5!:5 x)      -: 5!:5 q
(mbxcheck_jmf_ q), (5!:6 x)      -: 5!:6 q

q=: x=: (5!:2 <'g'); (u:&.>) ;:'avuncular kakistocracy hermeneutics'
(mbxcheck_jmf_ q), (5!:1 <'x') -: 5!:1 <'q'
(mbxcheck_jmf_ q), (5!:2 <'x') -: 5!:2 <'q'
(mbxcheck_jmf_ q), (5!:4 <'x') -: 5!:4 <'q'
(mbxcheck_jmf_ q), (5!:5 <'x') -: 5!:5 <'q'
(mbxcheck_jmf_ q), (5!:6 <'x') -: 5!:6 <'q'


NB. 7!: -----------------------------------------------------------------

q=: x=: 5!:1 <'mean'
(mbxcheck_jmf_ q), x             -: q
(7!:5 <'q') >: 7!:5 <'x'

q=: x=: ((u:&.>) ;:'Cogito, ergo sum.'), {:: 5!:1 <'mean'
(mbxcheck_jmf_ q), x             -: q
(7!:5 <'q') >: 7!:5 <'x'

q=: x=: 5!:1 <'g'
(mbxcheck_jmf_ q), x             -: q
(7!:5 <'q') >: 7!:5 <'x'

q=: x=: ((u:&.>) ;:'Cogito, ergo sum.'), {:: 5!:1 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (7!:5 <'q') >: 7!:5 <'x'

q=: x=: (u:&.>) ;:'base z'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (7!:6 (5&u:&.>) q) -: 7!:6 (5&u:&.>) x


NB. 8!: -----------------------------------------------------------------

q=: x=: <"0 ]7 ?@$ 0
('' (8!:0) x) -: '' (8!:0) q
('' (8!:1) x) -: '' (8!:1) q
('' (8!:2) x) -: '' (8!:2) q
mbxcheck_jmf_ q

q=: x=: <'c4'
y=: 3 4 ?@$ 0
(x 8!:0 y) -: q 8!:0 y
(x 8!:1 y) -: q 8!:1 y
(x 8!:2 y) -: q 8!:2 y
mbxcheck_jmf_ q


NB. 9!: -----------------------------------------------------------------

q=: x=: 9!:8 ''
9!:9 q
mbxcheck_jmf_ q
k=: 9!:42 ''

9!:43 ]1

q=: x=: 9!:44 ''
(mbxcheck_jmf_ q), x -: q
(mbxcheck_jmf_ q), 9!:45 q
9!:43 ]2
q=: x=: 9!:44 ''
(mbxcheck_jmf_ q), x -: q
(mbxcheck_jmf_ q), 9!:45 q
9!:43 ]3
q=: x=: 9!:44 ''
(mbxcheck_jmf_ q), x -: q
(mbxcheck_jmf_ q), 9!:45 q
9!:43 ]4
q=: x=: 9!:44 ''
(mbxcheck_jmf_ q), x -: q
(mbxcheck_jmf_ q), 9!:45 q
9!:43 ]0
q=: x=: 9!:44 ''
(mbxcheck_jmf_ q), x -: q
(mbxcheck_jmf_ q), 9!:45 q

9!:43 k


NB. 15!: ----------------------------------------------------------------

NB. see gdll.ijs

load'dll'

lib=: LIBTSDLL

dcd=: 4 : '(lib,x) cd y'

q=: x=: 1.1;2.2
NB. comment out the following tests to avoid failure with C_DF=0
('ddd d d d' dcd x) -: 'ddd d d d' dcd q
mbxcheck_jmf_ q

add=: mema 2*IF64{4 8
3 4 memw add,0,2,JINT
q=: x=: (,2);2;<<add
('xbasic x *x x *x' dcd x) -: 'xbasic x *x x *x' dcd q
mbxcheck_jmf_ q
0=memf add

q=: x=: ;:'q'
(15!:6 x) -: 15!:6 q
mbxcheck_jmf_ q

'domain error' -: 15!:6 etx <'dcd'

4!:55 ;:'add dcd lib'


NB. 18!: ----------------------------------------------------------------

q=: x=: <'xyz'
r=: y=: (u:&.>) ;:'bou stro phedonic'
abc_xyz_ =: ?10$100

(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), y             -: r
(mbxcheck_jmf_ q), (abc__x)      -: abc__q
(mbxcheck_jmf_ q), (18!:2 x)     -: 18!:2 (5&u:&.>) q
(mbxcheck_jmf_ q), (((5&u:&.>) y) 18!:2 x)   -: ((5&u:&.>) r) 18!:2 (5&u:&.>) q
(mbxcheck_jmf_ q), (18!:2 x)     -: 18!:2 (5&u:&.>) q
(mbxcheck_jmf_ q), (18!:55 x)    -: 18!:55 (5&u:&.>) q

18!:55 (5&u:&.>) y


NB. 128!: ---------------------------------------------------------------

NB. q=: x=: _306674912;1234567
NB. y=: u: 256+a.i. 'assiduously avoid any and all asinine alliterations'
NB. (x 128!:3 y) -: q 128!:3 y
mbxcheck_jmf_ q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j k mean q r t x y'



epilog''

