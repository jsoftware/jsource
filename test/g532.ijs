prolog './g532.ijs'
NB. }:y -----------------------------------------------------------------

randuni''

curtail =. _1&}.

NB. Boolean
(curtail -: }:) 1=?2
(curtail -: }:) 1=?4$2
(curtail -: }:) 1=?3 4$2
(curtail -: }:) 1=?2 3 4$2
(curtail -: }:) 1=?0 3$2

NB. literal
(curtail -: }:) 'a'
(curtail -: }:) a.{~?4$256
(curtail -: }:) a.{~?3 4$256
(curtail -: }:) a.{~?2 3 4$256
(curtail -: }:) 1=?0 3$256

NB. literal2
(curtail -: }:) u:'a'
(curtail -: }:) adot1{~?4$(#adot1)
(curtail -: }:) adot1{~?3 4$(#adot1)
(curtail -: }:) adot1{~?2 3 4$(#adot1)
(curtail -: }:) 1=?0 3$256

NB. literal4
(curtail -: }:) 10&u:'a'
(curtail -: }:) adot2{~?4$(#adot2)
(curtail -: }:) adot2{~?3 4$(#adot2)
(curtail -: }:) adot2{~?2 3 4$(#adot2)
(curtail -: }:) 1=?0 3$256

NB. symbol
(curtail -: }:) s:@<"0 'a'
(curtail -: }:) sdot0{~?4$(#sdot0)
(curtail -: }:) sdot0{~?3 4$(#sdot0)
(curtail -: }:) sdot0{~?2 3 4$(#sdot0)
(curtail -: }:) 1=?0 3$256

NB. integer
(curtail -: }:) 12345
(curtail -: }:) ?4$123456
(curtail -: }:) ?3 4$123456
(curtail -: }:) ?2 3 4$123456
(curtail -: }:) ?0 3$123456

NB. floating point
(curtail -: }:) 123.45
(curtail -: }:) o.?4$1236
(curtail -: }:) o.?3 4$1256
(curtail -: }:) o.?2 3 4$1456
(curtail -: }:) 0 3$123.456

NB. complex
(curtail -: }:) 123j45
(curtail -: }:) ^0j1*?4$1236
(curtail -: }:) ^0j1*?3 4$1256
(curtail -: }:) ^0j1*?2 3 4$1456
(curtail -: }:) 0 3$123j56

NB. boxed
t=.(+&.>i.5),;:'(raze a) -: }: a=. ^0j1*?3 4$1256'
(curtail -: }:) <123j45
(curtail -: }:) t{~?4$#t
(curtail -: }:) t{~?3 4$#t
(curtail -: }:) t{~?2 3 4$#t
(curtail -: }:) 0 3$<123456

4!:55 ;:'adot1 adot2 sdot0 curtail t '
randfini''


epilog''

