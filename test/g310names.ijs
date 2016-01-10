NB. names ----------------------------------------------------------------

abc_xyz_ -: p: i._5 [ 3 : '    abc_xyz_  =. p: i._5  ' 0
abc_xyz_ -: 12      [ 3 : '''a abc_xyz_''=. 3 12     ' 0
abc_xyz_ -: i.3 4   [ 3 : '".''abc_xyz_  =. i.3 4''  ' 0

'domain error' -: 3 : '       y  =: 12  ' etx 0
'domain error' -: 3 : '". ''  y  =: 12''' etx 0
'domain error' -: 3 : '   ''a y''=: 12 3' etx 0

'domain error' -: 3 : ('a=. 3';'      a  =:3  ') etx 0
'domain error' -: 3 : ('a=. 3';'".''  a  =:3''') etx 0
'domain error' -: 3 : ('a=. 3';'  ''c a''=:3  ') etx 0

1 [ 3 : ('a=: 99 ?.@$ 1e6';'a=.12') 0
a -: 99 ?.@$ 1e6


18!:55 <'xyz'
4!:55 ;:'a c'


