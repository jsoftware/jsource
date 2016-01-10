NB. ;y timing tests -----------------------------------------------------

ss   =: +/ @: *:
rsq  =: [: -. ss@(- +/ % #)@[ %~ ss@:-
ratio=: >./ % <./

a=: ;:'Cogito, ergo sum. boustrophedonic chthonic'
y=: ". '6!:2 ''; t'' [ t=: a $~ ',"1 ": ,. x=: 1000*1+i.9
threshold < t=: y rsq y (] +/ .* %.) x^/0 1

x=:3000$a
y=:(<$0),x
5>ratio t=:100 timer&>';x';';y'


4!:55 ;:'a ratio rsq ss t x y'


