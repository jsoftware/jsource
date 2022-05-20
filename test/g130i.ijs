prolog './g130i.ijs'

mtchinf =: [: -. 1 (e. ,) ~: *. *:&(e.&_ __)

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

(%/   mtchinf div/  ) x=:_1e2+?    23$2e2
(%/   mtchinf div/  ) x=:_1e2+?4   23$2e2
(%/"1 mtchinf div/"1) x
(%/   mtchinf div/  ) x=:_1e2+?7 5 23$2e2
(%/"1 mtchinf div/"1) x
(%/"2 mtchinf div/"2) x


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

(%/   mtchinf div/  ) x=:j./0.1*_1e2+?2     23$2e2
(%/   mtchinf div/  ) x=:j./0.1*_1e2+?2 4   23$2e2
(%/"1 mtchinf div/"1) x
(%/   mtchinf div/  ) x=:j./0.1*_1e2+?2 7 5 23$2e2
(%/"1 mtchinf div/"1) x
(%/"2 mtchinf div/"2) x

4!:55 ;:'div mtchinf x'



epilog''

