prolog './gctrl.ijs'
NB. control word parsing ------------------------------------------------

f0 =. 3 : 'if. if. 1 do. 2 end. do. 3 else. 4 end.'
f1 =. 3 : 'if. while. 1 do. 2 end. do. 3 end.'
f2 =. 3 : 'if. try. 1 catch. 2 end. do. 3 else. 4 end.'
f3 =. 3 : 'if. 1 do. while. 2 do. 3 end. end.'

g0 =. 3 : 'while. try. 1  catch. 2 end. do. 3 end.'
g1 =. 3 : 'while. 0 try. 1  catch. 2 end. do. 3 end.'
g2 =. 3 : 'while. if. 1 do. 2 else. 3 end. do. 4 end.'
g3 =. 3 : 'while. 1 do. if. 2 do. 3 else. 4 end. end.'
g4 =. 3 : 'while. 0 if. 1 do. break.    end. 2 do. 3 end.' 
g5 =. 3 : 'while. 0 if. 1 do. continue. end. 2 do. 3 end.' 


NB. control words -------------------------------------------------------

test =. 3 : '''control error'' -: ". etx ''3 : y'''

test 'if.'
test 'else.'
test 'elseif.'
test 'end.'
test 'do.'
test 'try.'
test 'catch.'
test 'while.'
test 'whilst.'
test 'continue.'
test 'break.'
test 'goto_.'
test 'goto_nonexist.'

test 'do. 1 2 3 end.'
test 'else. 1 2 3 end.'
test 'elseif. 1 do. 2 end.'
test 'end. 1'
test '1 end.'
test '1 end. 2'
test 'if. 1 end.'
test 'if. 1 do.'
test 'if. 1 do. 2 elseif. 3 end.'
test 'if. 1 do. 2 do. 3 end.'
test 'while. 1 do.'
test 'while. 1 end.'
test 'whilst. 1 do.'
test 'whilst. 1 end.'
test 'try. ''without catch.'''
test 'try. 1 catch.'
test 'try. 1 end.'

test 'if. y do. continue. end.'
test 'if. y do. break.    end.'
test 'if. y do. return.'

5 -: 3 : 'if. 0 do. 2 elseif. 0 do. 4 else. 5 end.'  ''
test 'if. 1 do. 2 else. if. 3 do. 4 else. 5 end.'
test 'while.  1 do. 2 else. 3 end.'
test 'whilst. 1 do. 2 else. 3 end.'

'spelling error' -: ex '3 : ''goto.'''
'spelling error' -: ex '3 : ''label.'''
'spelling error' -: ex '3 : ''repeat.'''
'spelling error' -: ex '3 : ''until.'''
'spelling error' -: ex '3 : ''begin.'''


NB. Direct definition {{ }} ----------------------------------------------------------------------

'open quote' -: ". etx '{{ a. =. }}'  NB. used to crash

NB. Note SP added after {{ and }}
f0=. 3 : 0
NB.
; {{
  '<', (|. y), '>'
 }} &.> |. 'one'; 'two'; 'three'
)
(r =: '3 : 0' , LF , 0 : 0 , ')') -:5!:5<'f0'
NB.
; {{   '<', (|. y), '>'
 }}  &.> |. 'one'; 'two'; 'three'
)

f0=. {{     NB. empty first line
  idxmut=. I. x >: (*/$y) ?@$ 0
  (populate idxmut) idxmut"_} y
}}

(r =: '4 : 0' , LF , 0 : 0 , ')') -:5!:5<'f0'
  idxmut=. I. x >: (*/$y) ?@$ 0
  (populate idxmut) idxmut"_} y
)

r =: {{ y }} :. {{ y }}
'3 : ''y '' :.(3 : ''y '')' -: 5!:5 <'r'

r =: {{
_1 = 4!:0 u
}}
(<'x') r  NB. x/y not defined in non-operator modifier execution
(<'y') r  NB. x/y not defined in non-operator modifier execution

NB. {{ }} in single-line defs
f0 =. 3 : ' 5 {{ x + y }} y'
'control error' -: ". etx '3 : '' 5 {{ x + y y'''
'3 : '' 5 {{ x + y }}  y''' -: 5!:5 <'f0'
0!:0 'f1 =. ',5!:5 <'f0'
'3 : '' 5 {{ x + y }}   y''' -: 5!:5 <'f1'
f0 =. 3 : '5 {{ x + y }} y'
'3 : ''5 {{ x + y }}  y''' -: 5!:5 <'f0'
0!:0 'f1 =. ',5!:5 <'f0'
'3 : ''5 {{ x + y }}   y''' -: 5!:5 <'f1'

NB. {{ }} in boxed def
f0 =. 3 : (<'vb =. {{ 0 }}')
'3 : ''vb =. {{ 0 }}''' -: 5!:5 <'f0'

NB. verb DD followed by noun DD
(' ') -: {{y}} {{)n }}
(' a') -: {{y}} {{)n }} , 'a'

NB. open quote
'aCasey''s Tool Works' -: {{ y , {{)nCasey's Tool Works}} }} 'a'
('aCasey''s Tool Works',LF) -: {{ y , {{)n
Casey's Tool Works
}} }} 'a'
'open quote' -:  0!:100 etx '{{',LF,'1+y',LF,'''',LF,'}}'


'control error' -: ". etx '{{ while. do. elseif. do.  end. }}'

'noun result was required' -: {{ {{ }} }} etx ''

'noun result was required' -: ({. ({{ {{ y }} }})`'') 5!:0 etx ''
'' -: ({. ({{ {{ y }} y }})`'') 5!:0  ''

10 -: ({. ({{
a =. {{
y
}}
+: y
}})`'') 5!:0 (5)

f0 =. {{ u }}
1 = 4!:0<'f0'
f0 =. {{ u v }}
2 = 4!:0<'f0'
f0 =. {{ v }}
2 = 4!:0<'f0'
f0 =. {{ u. }}
1 = 4!:0<'f0'
f0 =. {{ v. }}
2 = 4!:0<'f0'
f0 =. {{ u. v. }}
2 = 4!:0<'f0'

NB. DD displays as DD
f0 =. {{ y }}
'{{ y }}' -: '' 5!:5 <'f0'
f0 =. {{ u y }}
'{{ u y }}' -:  '' 5!:5 <'f0'
f0 =. {{ 5
x }}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{ 5
x }}
)
f0 =. {{
x + y }}"0
'{{ x + y }}"0' -:  '' 5!:5 <'f0'
f0 =. {{
x + y
5
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{ x + y
5}}
)
f0 =. {{x
:
x+y
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{ x
:
x+y}}
)
f0 =. {{)a
y
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{)a
y}}
)
f0 =. {{)c
x + y
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{)c
:
x + y}}
)
f0 =. {{)*
x + y
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{)d
x + y}}
)
f0 =. {{)m
x + y
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{)m
x + y}}
)
f0 =. {{)*
y
}}
(}: 0 : 0) -:  '' 5!:5 <'f0'
{{)m
y}}
)

NB. Assigned vbl not detected as auto
f0 =. {{ x =. y }}
'valence error' -: 3 f0 etx 5
f0 =. {{ m =. y }}
3 = 4!:0 <'f0'
 




NB. Nameref caching  -------------------------------------------------------------------------------
NB. names are cached
4!:55 ;:'vb__ vb_z_'
vb_z_ =: 5:

9!:5 (0)
g0 =. 3 : 0
vb y
)
9!:5 (2)
g1 =. 3 : 0
vb y  NB. This ref should be cached
)
9!:5 (0)

5 -: g0 ''   NB. run the verbs to establish caching for the names
5 -: g1 ''
5 -: g0 ''
5 -: g1 ''

vb__ =: 9:
9 -: g0 ''
5 -: g1 ''

NB. caching continues over deletion
vb_z_ =: 7:
9 -: g0 ''
5 -: g1 ''

4!:55 <'vb__'
7 -: g0 ''
5 -: g1 ''

9!:5 (2)
g1 =. 3 : 0
vb y
)
9!:5 (0)
7 -: g0 ''
7 -: g1 ''

NB. cached name removed after deletion
vb__ =: 3:
9!:5 (2)
g1 =. 3 : 0
vb y
)
9!:5 (0)
3 -: g0 ''
3 -: g1 ''

4!:55<'vb__'
7 -: g0 ''
3 -: g1 ''

NB. deleting locale leaves cached name
('a';'z') copath <'base'
vb_a_ =: 4:

9!:5 (2)
g1 =. 3 : 0
vb y
)
9!:5 (0)
4 -: g0 ''
4 -: g1 ''
(271828) 18!:55 <'a'
7 -: g0 ''
4 -: g1 ''

NB. reference using ''~ not cached
9!:5 (2)
g1 =. 3 : 0
'vb'~ y
)
9!:5 (0)
7 -: g0 ''
7 -: g1 ''

vb__ =: 6:
6 -: g0 ''
6 -: g1 ''

NB. Not even if caching continuously on
4!:55 <'vb__'
9!:5 (2)
g1 =. 3 : 0
'vb'~ y
)
7 -: g0 ''
7 -: g1 ''

vb__ =: 6:
6 -: g0 ''
6 -: g1 ''
9!:5 (0)

NB. Repeat for tacit verb
4!:55 ;:'vb__ vb_z_'
vb_z_ =: 5:

9!:5 (0)
g0 =. >:@<:@vb
9!:5 (2)
g1 =. >:@<:@vb
9!:5 (0)

5 -: g0 ''
5 -: g1 ''
5 -: g0 ''
5 -: g1 ''

vb__ =: 9:
9 -: g0 ''
5 -: g1 ''

NB. caching continues over deletion
vb_z_ =: 7:
9 -: g0 ''
5 -: g1 ''

4!:55 <'vb__'
7 -: g0 ''
5 -: g1 ''

9!:5 (2)
g1 =. >:@<:@vb
9!:5 (0)
7 -: g0 ''
7 -: g1 ''

NB. cached name removed after deletion
vb__ =: 3:
9!:5 (2)
g1 =. >:@<:@vb
9!:5 (0)
3 -: g0 ''
3 -: g1 ''

4!:55<'vb__'
7 -: g0 ''
3 -: g1 ''

NB. deleting locale leaves cached name
('a';'z') copath <'base'
vb_a_ =: 4:

9!:5 (2)
g1 =. >:@<:@vb
9!:5 (0)
4 -: g0 ''
4 -: g1 ''
(271828) 18!:55 <'a'
7 -: g0 ''
4 -: g1 ''

NB. reference using ''~ not cached
9!:5 (2)
g1 =. >:@<:@('vb'~)
9!:5 (0)
7 -: g0 ''
7 -: g1 ''

vb__ =: 6:
6 -: g0 ''
6 -: g1 ''

NB. Not even if caching continuously on
4!:55 <'vb__'
9!:5 (2)
g1 =. >:@<:@('vb'~)
7 -: g0 ''
7 -: g1 ''

vb__ =: 6:
6 -: g0 ''
6 -: g1 ''
9!:5 (0)

NB. numbered locale not cached
4!:55 <'vb__'
vb_z_=: 7:
l =. cocreate''
(l,<'z') copath <'base'
vb__l =: 8:

9!:5 (2)
g1 =. 3 : 0
vb y
)
9!:5 (0)
8 -: g1 ''
8 -: g1 ''
(<'z') copath <'base'
18!:55 l
7 -: g1 ''

NB. local name not cached
9!:5 (2)
g1 =. 4 : 0
(x) =. y"_
s ''
)
9!:5 (0)
5 -: 's' g1 5
6 -: 's' g1 6
'value error' -: 't' g1 etx 6

NB. indirect locative not cachable
4!:55 ;:'vb__ vb_z_'
vb_z_ =: 5:

9!:5 (2)
g1 =. 3 : 0
l=.<'base'
vb__l y
)
9!:5 (0)

5 -: g1 ''
5 -: g1 ''

vb__ =: 9:
9 -: g1 ''

NB. direct locative cachable and applies locative
vb_z_ =: 1: # coname
vb__ -: 2: # coname
9!:5 (2)
g1 =. 3 : 0
vb_z_ y
)
9!:5 (0)

(,<,'z') -: g1 ''
(,<,'z') -: g1 ''
4!:55 <'vb__'
(,<,'z') -: g1 ''
(,<,'z') -: g1 ''

NB. reference is cachable outside original context
4!:55 <'vb__'
vb_z_ =: 5:
9!:5 (2)
g1 =. 3 : 0
gvb =: vb
''
)
9!:5 (0)
g1''
5 -: gvb ''
5 -: gvb ''
vb__=: 2:
5 -: gvb''

NB. reference remains cachable after verb deleted
4!:55 <'vb__'
9!:5 (2)
g1 =. 3 : 0
gvb =: vb
''
)
9!:5 (0)
g1''
5 -: gvb ''  NB. this caches the name in g1
4!:55 <'g1'
5 -: gvb ''
5 -: gvb ''
vb__=: 2:
5 -: gvb''
4!:55 <'gvb'

NB. reference is cached before verb is deleted
4!:55 <'vb__'
vb_z_ =: 5:
9!:5 (2)
g1 =. 3 : 0
gvb =: vb
''
)
9!:5 (0)
g1''
4!:55 <'g1'
5 -: gvb ''
5 -: gvb ''
vb__=: 2:
5 -: gvb''
4!:55 <'gvb'

NB. Repeat for nameless modifier
NB. names are cached
4!:55 ;:'vb__ vb_z_'
vb_z_ =: @5:

9!:5 (0)
g0 =. 3 : 0
]vb y
)
9!:5 (2)
g1 =. 3 : 0
]vb y
)
9!:5 (0)

5 -: g0 ''
5 -: g1 ''
5 -: g0 ''
5 -: g1 ''

vb__ =: @9:
9 -: g0 ''
5 -: g1 ''

NB. caching continues over deletion
vb_z_ =: @7:
9 -: g0 ''
5 -: g1 ''

4!:55 <'vb__'
7 -: g0 ''
5 -: g1 ''

9!:5 (2)
g1 =. 3 : 0
]vb y
)
9!:5 (0)
7 -: g0 ''
7 -: g1 ''

NB. cached name removed after deletion
vb__ =: @3:
9!:5 (2)
g1 =. 3 : 0
]vb y
)
9!:5 (0)
3 -: g0 ''
3 -: g1 ''

4!:55<'vb__'
7 -: g0 ''
3 -: g1 ''

NB. 4!:8
'value error' -: ". etx '''undefname'' 4!:8'
4!:55 <'vb__'
vb_z_ =: 9:
9!:5 (2)
g1__=. 3 : 0
vb=:'vb' 4!:8
''
)
9!:5 (0)
numloc1=:cocreate''
(;:'base z') copath numloc1
g1__numloc1''
numloc2=:cocreate''
(;:'base z') copath numloc2
vb__ =: 5:
g1__numloc2''
9 -: vb__numloc1 ''  NB. cached refs are distinguished
5 -: vb__numloc2 ''
4!:55 ;:'vb_z_ vb__'
9 -: vb__numloc1 ''  NB. cached refs are really cached
5 -: vb__numloc2 ''
18!:55 numloc1,numloc2

NB. Combining attributes are cached, making the second execution faster
9!:5 (2)
vb__ =: +
f0 =. 3 : 0
y vb"+ y
) 
9!:5 (0)
r =: 1e6 ?@$ 0
THRESHOLD +. (6!:2 'f0 r') < 0.5 * (6!:2 'f0 r')

9!:5 (2)
vb__ =: ;
f0 =. 4 : 0
x vb@:((<@{.)"0 _) y
) 
9!:5 (0)
l =. 1000 + i. 100
r =: 1e6 ?@$ 0
(7!:2 'l f0 r') < 0.5 * (7!:2 'l f0 r')



fp=: ([ C."1 C.) :. ([ C.^:_1"1 C.^:_1)

tmonad=: 2 : 0
  '`vgety vgeto'=. n
  argy=. vgety y
  t=. timex 'ret=. ' , m , ' argy'
  NB. this line crashes JE
  out=. vgeto ret
  42
)

testhetrf=: 3 : 0
  calcA=: fp^:_1
  log=. (']' tmonad ((1&{::)`calcA)) 1 ;< 1;2;3
1
)
testhetrf''

NB. PPPP
4!:55 ;:'f0 f1 f3'
f0 =. 1000
f0 =. 7!:0''
f1 =. f0
f3 =. 3 : 'i. 1e6'
f0 > _5000 + 7!:0''
f3 =. 3 : '(i. 1e6)'
f0 > _5000 + 7!:0''
f3 =. 3 : '] ] ] ] ] ] y'
f1 =. 7!:0''
f3 =. 3 : '(] ] ] ] ] ]) y'
f1 < _250 + 7!:0''
f3 =. 3 : '((i. 1e6))'
f0 < 100000 + 7!:0''

'|syntax error in efx, executing 3:...|       ((3 3$0 1 2 1 2 3 2 3 4))+' -:&(10&{. , i:&' ' }. ]) efx '3 : ''((+/~ i. 3)) +'' 5'
1: {{)n
9!:41(0)  NB. lose comments
f0 =. 3 : '(+/ #  *) y'
'3 : ''(+/#*)y''' -: 5!:5 <'f0'
f0 =. 3 : '(*: >:) (/  / ) y'
'3 : ''(*:>:)(//)y''' -: 5!:5 <'f0'
f0 =. 3 : '((+/~ i. 3)) + y'
'3 : ''((3 3$0 1 2 1 2 3 2 3 4))+y''' -: 5!:5 <'f0'

f0 =. {{ (u. f. b. 0) }}   NB. u. does not invoke PPPP

9!:41(1)
}}

NB. 9.7 'control error' -:  0!:100 etx 'tvbl =. 3 : 0',LF,'5'  NB. EOF in definition
NB. 9.7 _1 = 4!:0 <'tvbl'

{{ dodothrow =. {{ if. dothrow'' do. end. }} [. (dothrow =: {{ throw. }}) try. dodothrow'' 0 catcht. 1 end. }} ''  NB. throw passed through T-block

4!:55 ;:'vb_z_'



epilog''

