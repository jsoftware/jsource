NB. 5!:0 ----------------------------------------------------------------

fx =: 5!:0
ar =: 5!:1

((ar <'a') fx) -: a =: 2=?20$2
((ar <'a') fx) -: a =: a.{~?2 3 4$#a.
((ar <'a') fx) -: a =: ?2 3$2000
((ar <'a') fx) -: a =: o.?20$100
((ar <'a') fx) -: a =: ^0j1*?3 4 5$100
((ar <'a') fx) -: a =: +&.>?4 3$100

((ar <'a') fx) -: a =: 0 4 5 $ 2=?20$2
((ar <'a') fx) -: a =: 0 0   $ a.{~?2 3 4$#a.
((ar <'a') fx) -: a =: 4 0 0 $ ?2 3$2000
((ar <'a') fx) -: a =: 0     $ o.?20$100
((ar <'a') fx) -: a =: 4 0 5 $ ^0j1*?3 4 5$100
((ar <'a') fx) -: a =: 5 0   $ +&.>?4 3$100

tv   =: 1 : '=/(ar<''x'') fx `(x f.)'
eq   =: ar&<@[ -: ar&<@]
each =: 1 : 'x f.&.>'
pow  =: 2 : ('i=.>:y'; 't=.]'; 'while. i=.<:i do. t=.x&t f. end.')

+             tv
+/ . *        tv
+/ pow 4      tv
|. each       tv

+/ : *        tv
+/ : [:       tv
3  : '2*y'    tv
[: : (3&*)    tv
[: : [:       tv

(+%)          tv
(=<.)         tv
(+,-)         tv
(0&<: *. <.)  tv

f =: \.
g =: (ar <'f') fx
'f' eq 'g'

f =: :
g =: (ar <'f') fx
'f' eq 'g'

f =: eq
g =: (ar <'f') fx
'f' eq 'g'

f =: each
g =: (ar <'f') fx
'f' eq 'g'

f =: pow
g =: (ar <'f') fx
'f' eq 'g'

a. -: (<'a.') fx
=/ (<'+') fx ` +
(+/a) -: + (<'/') fx a=:?20$100

f =: +&.^.
g =: + ((<'&.') fx) ^.
'f' eq 'g'

f =: +/.
g =: (<'/.';<''`+) fx
'f' eq 'g'

f =: /.
g =: (<'/.') fx
'f' eq 'g'

f =: &
g =: (<,<,'&') fx
'f' eq 'g'


NB. 5!:0 ----------------------------------------------------------------

fx  =: 5!:0
ar  =: 5!:1

'rank error'     -: ex '( 3 4$<9   ) fx'
'rank error'     -: ex '(<3 4$<9   ) fx'                  
'rank error'     -: ex '(<3 4$''a'') fx'                  

'domain error'   -: ex '0            fx'                         
'domain error'   -: ex '34           fx'                         
'domain error'   -: ex '3.5          fx'                         
'domain error'   -: ex '3j4          fx'                         
'domain error'   -: ex '+            fx'

'length error'   -: ex '(<''+'';3;4) fx'

'domain error'   -: ex '(<0 1 0  )   fx'
'domain error'   -: ex '(<3 4 5  )   fx'
'domain error'   -: ex '(<3.5 4 5)   fx'
'domain error'   -: ex '(<3j5 4 5)   fx'

'spelling error' -: ex '(<''asdf:'') fx'
'spelling error' -: ex '(<128{a.)    fx'
'spelling error' -: ex '(<156{a.)    fx'
'spelling error' -: ex '(<254{a.)    fx'
'spelling error' -: ex '(<255{a.)    fx'

'spelling error' -: ex '(<''*%'';4         ) fx'
'spelling error' -: ex '(<''*%''           ) fx'
'spelling error' -: ex '(<''*.%''          ) fx'
'spelling error' -: ex '(<''ab.''          ) fx'
'domain error'   -: ex '(<''=.''           ) fx'
'domain error'   -: ex '(<''=:''           ) fx'
'domain error'   -: ex '(<''x.''           ) fx'
'domain error'   -: ex '(<''0''            ) fx'
'domain error'   -: ex '(<''2''            ) fx'
'domain error'   -: ex '(<''3''            ) fx'
'domain error'   -: ex '(<3   ;4           ) fx'
'domain error'   -: ex '(<3j4 ;5           ) fx'
'domain error'   -: ex '(<(<''+'');4       ) fx'
'rank error'     -: ex '(<(3 4$''+'');4    ) fx'

'domain error'   -: ex '(<''/.'';<4        ) fx'
'domain error'   -: ex '(<''/.'';<''a''    ) fx'
'domain error'   -: ex '(<''/.'';<3.5      ) fx'
'domain error'   -: ex '(<''/.'';<3j4      ) fx'
'rank error'     -: ex '(<''/.'';<3 4$<9   ) fx'
'length error'   -: ex '(<''+.'';<+`%      ) fx'
'length error'   -: ex '(<''/.'';<+`%      ) fx'
'length error'   -: ex '(<''&'';<''''`%    ) fx'
'length error'   -: ex '(<''&'';<3$+`%     ) fx'

noun=.3 4
cn =. ar <'noun' 
verb=.+
cv =. ar <'verb'

f=:+%
(<(,'2');<+ ` %) -: ar <'f'
'domain error' -: ex '(<''2'';<3 4         ) fx'            
'syntax error' -: ex '(<''2'';<cv,cn       ) fx'            
'syntax error' -: ex '(<''2'';<cn,cv       ) fx'            
'syntax error' -: ex '(<''2'';<cn,cn       ) fx'            
'domain error' -: ex '(<''2''              ) fx'            
'length error' -: ex '(<''2'';<<''%''      ) fx'            
'length error' -: ex '(<''2'';<+ ` % `-    ) fx'            

f=:+,*
(<(,'3');<+ ` , ` *) -: ar <'f' 
'domain error' -: ex '(<''3''              ) fx'           
'syntax error' -: ex '(<''3'';<cv,cv,cn    ) fx'           
'syntax error' -: ex '(<''3'';<cv,cn,cv    ) fx'           
'syntax error' -: ex '(<''3'';<cv;cv;''/'' ) fx'           
'length error' -: ex '(<''3'';0$<i.0       ) fx'           
'length error' -: ex '(<''3'';<+ ` ,       ) fx'
'length error' -: ex '(<''3'';<4$ , ` *    ) fx'           

10 -: (<'/';<<'+') fx i.5
'domain error' -: ex '(<''/''; <<''\''     ) fx'             
'domain error' -: ex '(<''&''; <''/'';''@'') fx'             
'length error' -: ex '(<''a.'';<<''+''     ) fx'


4!:55 ;:'a ar cn cv each eq f fx g noun pow tv verb '


