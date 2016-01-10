NB. }:y -----------------------------------------------------------------

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

4!:55 ;:'curtail t '


