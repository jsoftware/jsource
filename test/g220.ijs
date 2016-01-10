NB. ~ -------------------------------------------------------------------

3   -: 2 -~ 5
2.5 -: 2 %~ 5

(x=:?2 3 4$1000) (-~ -: ] - [) y=:?2 3$1000
(x=:?2 3 4$1000) (%~ -: ] % [) y=:?2 3$1000

(+:t) -: +~t=:_50+?2 3 4$100
(*:t) -: *~t=:o._50+?2 3 4$100

mean=: +/ % #
(mean x) -: 'mean'~ x=: ?100$1000

'value error'  -: ex '''asdf''~ x'

'domain error' -: ex '0 0 1~ x'
'domain error' -: ex '1 2 3~ x'
'domain error' -: ex '1 2.3~ x'
'domain error' -: ex '1 2j3~ x'
'domain error' -: ex '1 23x~ x'
'domain error' -: ex '1 2r3~ x'
'domain error' -: ex '(<''mean'')~ x'

'ill-formed name' -: ex '      ''a___''~ 12'
'ill-formed name' -: ex '  ''a_gosh!_''~ 12'
'ill-formed name' -: ex '''a_gosh*@!_''~ 12'
'ill-formed name' -: ex ' ''do_gosh!_''~ 12'


4!:55 ;:'mean t x y'


