prolog './g522.ijs'
NB. {:y -----------------------------------------------------------------

randuni''

tail =. _1&{

4         -: {: 4
4         -: {: i.5
'.'       -: {: 'Cogito, ergo sum.'
(u:'.')   -: {: u:'Cogito, ergo sum.'
(10&u:'.')-: {: 10&u:'Cogito, ergo sum.'
(s:@<"0 '.')   -: {: s:@<"0 'Cogito, ergo sum.'
(<'sum.') -: {: ;:'Cogito, ergo sum.'
(<u:'sum.') -: {: (u:&.>) ;:'Cogito, ergo sum.'
(<10&u:'sum.') -: {: (10&u:&.>) ;:'Cogito, ergo sum.'
(s:@<"0&.> <'sum.') -: {: s:@<"0&.> ;:'Cogito, ergo sum.'
(<"0@s: <'sum.') -: {: <"0@s: ;:'Cogito, ergo sum.'
(s:@<"0&.> <'sum.') -: {: s:@<"0&.> ;:'Cogito, ergo sum.'
(<"0@s: <'sum.') -: {: <"0@s: ;:'Cogito, ergo sum.'
8 9 10 11 -: {: i.3 4

NB. Boolean
(tail -: {:) 1=?2
(tail -: {:) 1=?4$2
(tail -: {:) 1=?3 4$2
(tail -: {:) 1=?2 3 4$2

NB. literal
(tail -: {:) 'a'
(tail -: {:) a.{~?4$256
(tail -: {:) a.{~?3 4$256
(tail -: {:) a.{~?2 3 4$256

NB. literal2
(tail -: {:) u:'a'
(tail -: {:) adot1{~?4$(#adot1)
(tail -: {:) adot1{~?3 4$(#adot1)
(tail -: {:) adot1{~?2 3 4$(#adot1)

NB. literal4
(tail -: {:) 10&u:'a'
(tail -: {:) adot2{~?4$(#adot2)
(tail -: {:) adot2{~?3 4$(#adot2)
(tail -: {:) adot2{~?2 3 4$(#adot2)

NB. symbol
(tail -: {:) s:@<"0 'a'
(tail -: {:) sdot0{~?4$(#sdot0)
(tail -: {:) sdot0{~?3 4$(#sdot0)
(tail -: {:) sdot0{~?2 3 4$(#sdot0)

NB. integer
(tail -: {:) 12345
(tail -: {:) ?4$123456
(tail -: {:) ?3 4$123456
(tail -: {:) ?2 3 4$123456

NB. floating point
(tail -: {:) 123.45
(tail -: {:) o.?4$1236
(tail -: {:) o.?3 4$1256
(tail -: {:) o.?2 3 4$1456

NB. complex
(tail -: {:) 123j45
(tail -: {:) ^0j1*?4$1236
(tail -: {:) ^0j1*?3 4$1256
(tail -: {:) ^0j1*?2 3 4$1456

NB. boxed
t=.(+&.>i.5),;:'(tail -: {:) ^0j1*?3 4$1256'
(tail -: {:) <123j45
(tail -: {:) t{~?4$#t
(tail -: {:) t{~?3 4$#t
(tail -: {:) t{~?2 3 4$#t

(  3$0  ) -: {: 0   3$1
(2 3$' ') -: {: 0 2 3$'a'
(  3$0  ) -: {: 0   3$12345
(  3$0  ) -: {: 0   3$123.45
(  3$0  ) -: {: 0   3$123j45
(1 3$a: ) -: {: 0 1 3$<45

(2 3$3j4) -: {:!.3j4 i. 0 2 3


4!:55 ;:'adot1 adot2 sdot0 t tail'
randfini''


epilog''

