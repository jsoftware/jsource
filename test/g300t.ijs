prolog './g300t.ijs'
NB. -/ .* y timing tests ------------------------------------------------

h=: [: -/"1 {."2 * |."1@:({:"2)

m=: 1024 %~ _5e5* 5000 2 2 ?@$1e6
THRESHOLD +. 2   > | (-/ % <./) t=: (2 timer '-/ .* m') , 2 timer 'h m' NB. can be slow first time
THRESHOLD +. 0.4 > | (-/ % <./) t=: (2 timer '-/ .* m') , 2 timer 'h m'




epilog''

