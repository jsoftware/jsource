prolog './g321t.ijs'
NB. ,.&.>/y timing tests ------------------------------------------------

f  =: >@(,.&.>/)
ss =: +/ @: *:
rsq=: [: -. ss@(- +/ % #)@[ %~ ss@:-

([ -: f@:(<"1)@|:) x=:     10 17?@$2
([ -: f@:(<"1)@|:) x=:    (10 17?@$#a.){a.
([ -: f@:(<"1)@|:) x=:     10 17?@$2e9
([ -: f@:(<"1)@|:) x=:j./2 10 17?@$2e9
([ -: f@:(<"1)@|:) x=:    (10 17?@$#t){t=:;:'Sui generis ec cle si as tic'
([ -: f@:(<"1)@|:) x=:    (10 17?@$#t){t=:(u:&.>) ;:'Sui generis ec cle si as tic'
([ -: f@:(<"1)@|:) x=:    (10 17?@$#t){t=:(10&u:&.>) ;:'Sui generis ec cle si as tic'
([ -: f@:(<"1)@|:) x=:    (10 17?@$#t){t=:s:@<"0&.> ;:'Sui generis ec cle si as tic'
([ -: f@:(<"1)@|:) x=:    (10 17?@$#t){t=:<"0@s: ;:'Sui generis ec cle si as tic'

(|:>34$<x) -: f 17$<x,.x=:(10?@$#a.){a.

y=: ". 'timer ''f t'' [ t=: (<12?@$1e9) $~ ',"1 ": ,. x=: 11000*1+i.9
THRESHOLD +. threshold < y rsq y (] +/ .* %.) x^/0 1


4!:55 ;:'f rsq ss t x y '



epilog''

