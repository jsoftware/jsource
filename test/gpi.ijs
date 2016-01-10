NB. pi ------------------------------------------------------------------

NB. pi and the common mean (AGM, arithmetic-geometric mean)
NB. see http://www.shef.ac.uk/~puremath/theorems/amgm.html

am=: +/ %   #
gm=: */ %:~ #
cm=: [: {. (am,gm)^:_

t=: (am,gm)^:(i.10) 1,%%:2
'a b'=: |: t
(o.1) e. (}. 2**:a) % }: 1 - +/\ (2^i.#a) * a-&*:b


4!:55 ;:'a am b cm gm t'


