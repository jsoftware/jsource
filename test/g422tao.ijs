prolog './g422tao.ijs'
NB. /:y -----------------------------------------------------------------

NB. The TAO (total array ordering) of J

ge=: 0 1 -: \:@,&<
gt=: 1 0 -: /:@,&<
le=: 0 1 -: /:@,&<
lt=: 1 0 -: \:@,&<
eq=: -:!.0

1 =   1 2 3 gt 0 1 2 3 4
0 =   1 2 3 gt i.2 5
0 =   1 2 3 gt i.2 5
0 =   123 gt '123'
1 =   3j4 gt 2j7

4!:55 ;:'eq ge gt le lt'

epilog''

