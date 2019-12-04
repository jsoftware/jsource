1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g130i.ijs'
NB. %/ B ----------------------------------------------------------------

div=: 4 : 'x%y'

(%/   -: div/  ) x=:?3 5 23$2
(%/"1 -: div/"1) x
(%/"2 -: div/"2) x

(%/   -: div/  ) x=:?3 5 32$2
(%/"1 -: div/"1) x
(%/"2 -: div/"2) x


NB. %/ I ----------------------------------------------------------------

div=: 4 : 'x%y'

(%/   -: div/  ) x=:_1e2+?    23$2e2
(%/   -: div/  ) x=:_1e2+?4   23$2e2
(%/"1 -: div/"1) x
(%/   -: div/  ) x=:_1e2+?7 5 23$2e2
(%/"1 -: div/"1) x
(%/"2 -: div/"2) x


mtchinf =: [: -. 1 (e. ,) ~: *. *:&(e.&_ __)

NB. %/ D ----------------------------------------------------------------

div=: 4 : 'x%y'"0  NB. %/ is inaccurate in its treatment of -0 intermediate values.  The singleton code preserves this inaccuracy, so we match against that

(%/   mtchinf div/  ) x=:0.1*_1e2+?    23$2e2
(%/   mtchinf div/  ) x=:0.1*_1e2+?4   23$2e2
(%/"1 mtchinf div/"1) x
(%/   mtchinf div/  ) x=:0.1*_1e2+?7 5 23$2e2
(%/"1 mtchinf div/"1) x
(%/"2 mtchinf div/"2) x


NB. %/ Z ----------------------------------------------------------------

div=: 4 : 'x%y'

(%/   -: div/  ) x=:j./0.1*_1e2+?2     23$2e2
(%/   -: div/  ) x=:j./0.1*_1e2+?2 4   23$2e2
(%/"1 -: div/"1) x
(%/   -: div/  ) x=:j./0.1*_1e2+?2 7 5 23$2e2
(%/"1 -: div/"1) x
(%/"2 -: div/"2) x

4!:55 ;:'div mtchinf x'


