prolog './gtdot5.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. 64-bit only

NB. wait until there are y waiting threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads


NB. tests of locales keyword
delth''

0!:_1`1:@.(9!:56'supportafinity') '$'   NB. skip if not supportaffinity
NB. populate shared locale
create_shared_ =: {{ gbl =: >y }}
destroy_shared_ =: 4!:55@coname
double_shared =: {{ ({. 3 T. '') , gbl =: +: gbl }}

1: (0 T. [: < 'coremask';2&^)"0 ] 1 2 3  NB. create 3 threads, tied to 3 worker cores
locs =: 0&".@>@(conew&'shared')@> locgbls =: 10;20;30;i. 10    NB. Create 4 numbered locales as instances of shared
(1 2 3 (, 2&+)&.> }. locgbls) -: {{ ({. 3 T. '') , gbl =: gbl + y }} t. (<'locales';<14;1 2 3{locs) 2
(0 2 2 2 +&.> locgbls) -: {{ gbl__y }}&.> locs
(0 1 2 3 ,&.> 3&*&.> 0 2 2 2 +&.> locgbls) -: {{ ({. 3 T. '') , gbl =: gbl * y }} t. (<'locales';<15;locs) 3
(3&*&.> 0 2 2 2 +&.> locgbls) -: {{ gbl__y }}&.> locs

'domain error' -: ". etx {{)n + t. ('locales' ,&< 7 ; i. 4) }}
'domain error' -: ". etx {{)n + t. (<<'locales') }}
'domain error' -: ". etx {{)n + t. (<'locales' ,&< i. 2) }}
'rank error' -: ". etx {{)n + t. (<'locales' ,&< ,: 1;2) }}
'length error' -: ". etx {{)n + t. (<'locales' ,&< 1;2;3) }}
'domain error' -: ". etx {{)n + t. (<'locales' ,&< 1.5;2) }}
'rank error' -: ". etx {{)n + t. (<'locales' ,&< (,2);2) }}
'domain error' -: ". etx {{)n + t. (<'locales' ,&< 3;0 1) }}
'rank error' -: ". etx {{)n + t. (<'locales' ,&< 3;i. 1 2) }}
'would deadlock' -: ". etx {{)n + t. (<'locales' ,&< 3;0 1 2) }}
'would deadlock' -: ". etx {{)n + t. (<'locales' ,&< 1;2) }}
'' -: ". etx {{)n + t. (<'locales' ,&< 2;2) }}
'domain error' -: ". etx {{)n + t. ((<'locales' ,&< 2;2),(<'locales' ,&< 2;2)) }}
18!:55 ;:'shared'
18!:55 locs
NB.$

3 : 0''
if. IFWIN do.
 sleep=: usleep@>.@(1e6&*)
else.
 sleep=: 6!:3
end.
1
)

delth''  NB. make sure we start with an empty system
N=: 62  NB. max # worker threads

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N = 1 T.''
wthr N

p2 =: ".
(i. 100) -: > (p2@> t.'')"0  <@":"0 i. 100   NB. test executing tacit verb with no parser running



p2=: 3 : 0
for_i. i.100 do.
ALL=: ALL, ,~({. 3 T.'')
end.
i. 0 0
)

t2=: 3 : 0
ALL=: 0 0$0
pyx=. (p2 t.'')"0  i. y
1:&>pyx
i. 0 0
)

p3=: 3 : 0
ALL=. 0$0
for_i. i.100 do.
ALL=. ALL, ({. 3 T.'')
end.
#ALL
)

t3=: 3 : 0
pyx=. (p3 t.'')"0  i. y
1:&>pyx
i. 0 0
)

p4=: 3 : 0
ALL=. 0 0$0
for_i. i.100 do.
ALL=. ALL, ,~({. 3 T.'')
end.
#ALL
)

t4=: 3 : 0
pyx=. (p4 t.'')"0  i. y
1:&>pyx
i. 0 0
)

NB. 1: (9!:_5) ] _5000  NB. engage test mode before defining the verbs & names, to avoid crash freeing

NB. Settings
TESTS234 =: 0  NB. set to 1 to allow other tests here to run
TEST1CT =: 1000   NB. set to 1e8 to repeat test1 forever here
REASSIGNFRAC =: 0.5  NB. set to 0 to suppress reassignment, 1.0 to force all assignments to reassign
ALLOSDURINGASSIGN =: 0  NB. set to 1 to cause many more buffer allocations per assignment


p1=: 3 : 0
for_i. y do.
ALL=: 15!:15^:i ALL [ i."0^:ALLOSDURINGASSIGN  20 # 128
end.
i. 0 0
)

t1=: 4 : 0
ALL=: 128$00
pyx=. (p1 t.''@y)"0 x#0
1:&>pyx
i. 0 0
)



TEST1CT t1 REASSIGNFRAC > 1000 ?@$ 0

0!:_1@'$'^:(-.TESTS234) 1  NB. optionally skip other tests

t3 100

t2 100
t4 100

NB. 1: 9!:_5 ] 0

delth''



1: 0 : 0  NB. Move these tests to be executed, one by one

p1=: {{  NB. scaf
label_loop. ALL=:ALL, ((,0.5)) goto_loop.
EMPTY
}}

p1=: {{  NB. scaf
label_loop. ALL=:ALL, ,0.5 goto_loop.
EMPTY
}}

NB. ***** current test *****
p1=: {{  NB. scaf
while. do. ALL=:ALL, ((,0.5)) end.
EMPTY
}}

p1=: {{  NB. scaf
for. 1e7#' ' do. ALL=:ALL, ((,0.5)) end.
EMPTY
}}

p1=: {{  NB. scaf
for. 1e7#' ' do. ALL=:ALL (, >:) ((,0.5)) end.
EMPTY
}}

p1=: {{  NB. scaf
for. 1e7#' ' do. ALL=:ALL, ,0.5 end.
EMPTY
}}

p1=: {{
for_i. (1000 * {. 3 T. '') + i.1e7 do.
ALL=: ALL, i
end.
EMPTY
}}

p1=: {{
for_i. (1000 * {. 3 T. '') + i.1e7 do.
ALL=: ALL, ,i
end.
EMPTY
}}

p1=: {{
for_i. (1000 * {. 3 T. '') + i.1e7 do.
ALL=: ALL, ,i [ echo i
end.
EMPTY
}}

)
NB.$  end of skip
epilog''
  
