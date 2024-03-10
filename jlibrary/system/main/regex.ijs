NB. regular expressions
NB.%regex.ijs - regular expressions
NB.-This script defines regular expression utilities
NB.-Definitions are loaded into the jregex locale with
NB.-cover functions in the z locale

coclass <'jregex'
NB. util

Rxnna=: '(^|[^[:alnum:]_])'
Rxnnz=: '($|[^[:alnum:]_.:])'
Rxass=: '[[:space:]]*=[.:]'

ischar=: 2=3!:0

NB. =========================================================
NB. user compilation flags (0=off, 1=on)
RX_OPTIONS_MULTILINE=: 1  NB. enable newline support, default on
RX_OPTIONS_UTF8=: 1  NB. enable UTF-8 support

NB. =========================================================
NB. clear last values, not saved compiles
regclear=: 3 : 0
lastcomp=: lasthandle=: lastmatch=: lastnsub=: 0
lastpattern=: ''
EMPTY
)

NB. =========================================================
regcomp=: 3 : 0
if. -.ischar y do. reghandle y return. end.
if. (0<#y) *. y-:lastpattern do. return. end.
regfree''
lastpattern=: y
msg=. ,2
off=. ,2
flg=. (PCRE2_UTF*RX_OPTIONS_UTF8)+PCRE2_MULTILINE*RX_OPTIONS_MULTILINE
lastcomp=: 0 pick rc=. jpcre2_compile (,y);(#y);flg;msg;off;<<0
'msg off'=. 4 5{rc
if. 0=lastcomp do. regerror msg,off end.
lasthandle=: 0
lastmatch=: 0 pick jpcre2_match_data_create_from_pattern (<lastcomp);<<0
lastnsub=: 0 pick jpcre2_get_ovector_count <<lastmatch
EMPTY
)

NB. =========================================================
NB. regerror
regerror=: 3 : 0
m=. regerrormsg y
lasterror=: m
regfree''
m 13!:8[12
)

NB. =========================================================
regerrormsg=: 3 : 0
'msg off'=. 2 {. y,_1
m=. (0 >. >{.rc) {. 2{::rc=. jpcre2_get_error_message msg;(256#' ');256
if. off >: 0 do.
  m=. m,' at offset ',(":off),LF
  m=. m,lastpattern,LF,(off#' '),'^',LF
end.
)

NB. =========================================================
regfree=: 3 : 0
if. lasthandle=0 do.
  if. lastmatch do.
    jpcre2_match_data_free <<lastmatch
  end.
  if. lastcomp do.
    jpcre2_code_free <<lastcomp
  end.
end.
regclear''
)

NB. =========================================================
NB. set handle
reghandle=: 3 : 0
reghandlecheck y
ndx=. cmhandles i. y
'lastcomp lastmatch lastnsub'=: ndx{cmtable
lastpattern=: ndx pick cmpatterns
lasthandle=: ndx{cmhandles
EMPTY
)

NB. =========================================================
reghandlecheck=: 3 : 0
if. y e. cmhandles do. y return. end.
m=. 'handle not found: ',":y
m 13!:8[12
)

NB. =========================================================
regmatch1=: 3 : 0
regmatchtab 0 pick jpcre2_match (<lastcomp);(,y);(#y);0;0;(<lastmatch);<<0
)

NB. =========================================================
regmatch2=: 3 : 0
's p'=. y
regmatchtab 0 pick jpcre2_match (<lastcomp);(,s);(#s);p;PCRE2_NOTBOL;(<lastmatch);<<0
)

NB. =========================================================
NB. get match table
regmatchtab=: 3 : 0
if. y >: 0 do.
  p=. 0 pick jpcre2_get_ovector_pointer <<lastmatch
  'b e'=. |:_2 [\ memr p,0,(2*lastnsub),4
  _1 0 (I.b=_1) } b,.e-b
elseif. y=_1 do.
  ,:_1 0
elseif. do.
  regerror y
end.
)

NB. =========================================================
NB. show all matches
regshow=: 4 : 0
m=. x rxmatches y
r=. ,:y
if. 0 = # m do. return. end.
for_i. i. 1 { $ m do.
  a=. i {"2 m
  x=. ;({."1 a) (+i.) each {:"1 a
  r=. r, '^' x } (#y) # ' '
end.
)

NB. =========================================================
NB. for now just check if comp is a valid compiled pattern
regvalid=: 3 : 0
len=. 0 pick jpcre2_pattern_info (<y);PCRE2_INFO_SIZE;<<0
rc=. 0 pick jpcre2_pattern_info (<y);PCRE2_INFO_SIZE;<len$' '
if. rc>:0 do. 1 else. 0[echo (regerrormsg rc),': ',":y end.
)

NB. =========================================================
showhandles=: 3 : 0
cls=. ;:'pattern handle comp match nsub valid'
if. #cmpatterns do.
  val=. regvalid &> {."1 cmtable
  dat=. (>cmpatterns);<@,."1 |: cmhandles,.cmtable,.val
else.
  dat=. (#cls)#<EMPTY
end.
cls,:dat
)

NB. =========================================================
NB. utility to show rxmatches at top level
showmatches=: 4 : 0
m=. x rxmatches y
r=. ,:y
if. 0 = # m do. return. end.
'b e'=. |:{."2 m
p=. ;b (+ i.) each e
r, '^' p } (#y) # ' '
)
NB. lib

NB. =========================================================
NB. compile flags:
PCRE2_NOTBOL=: 16b1
PCRE2_NOTEOL=: 16b2
PCRE2_MULTILINE=: 16b400
PCRE2_UTF=: 16b80000

NB. info types:
PCRE2_INFO_SIZE=: 22

NB. =========================================================
NB. pcre2 library is in bin or tools/regex
3 : 0''
select. UNAME
case. 'Win' do. t=. 'jpcre2.dll'
case. 'Darwin' do. t=. 'libjpcre2.dylib'
case. do. t=. 'libjpcre2.so'
end.

f=. BINPATH,'/',t
if. 0 = 1!:4 :: 0: <f do.
  f=. jpath '~tools/regex/',t
end.

NB. fall back one more time
if. IFUNIX *. 0 = 1!:4 :: 0: <f do.
  f=. unxlib 'pcre2'
elseif. 0 = 1!:4 :: 0: <f do.
  f=. t
end.

pcre2dll=: f
)

NB. =========================================================
makefn=: 3 : 0
'name decl'=. y
('j',name)=: ('"',pcre2dll,'" ',name,'_8 ',(IFWIN#'+ '),decl)&(15!:0)
EMPTY
)

NB. void pcre2_code_free_8 (pcre2_code_8 *);
NB. pcre2_code_8 *pcre2_compile_8 (PCRE2_SPTR8, size_t, uint32_t, int *, size_t *, pcre2_compile_context_8 *);
NB. int pcre2_get_error_message_8 (int, PCRE2_UCHAR8 *, size_t);
NB. uint32_t pcre2_get_ovector_count_8 (pcre2_match_data_8 *);
NB. size_t *pcre2_get_ovector_pointer_8 (pcre2_match_data_8 *);
NB. int pcre2_match_8 (const pcre2_code_8 *, PCRE2_SPTR8, size_t, size_t, uint32_t, pcre2_match_data_8 *, pcre2_match_context_8 *);
NB. pcre2_match_data_8 *pcre2_match_data_create_from_pattern_8 (const pcre2_code_8 *, pcre2_general_context_8 *);
NB. void pcre2_match_data_free_8 (pcre2_match_data_8 *);
NB. int pcre2_pattern_info_8 (const pcre2_code_8 *, uint32_t, void *);

makefn 'pcre2_code_free';'n *'
makefn 'pcre2_compile';'* *c x i *i *x *'
makefn 'pcre2_get_error_message';'i i *c x'
makefn 'pcre2_get_ovector_count';'i *'
makefn 'pcre2_get_ovector_pointer';'*x *'
makefn 'pcre2_match';'i * *c x x i * *'
makefn 'pcre2_match_data_create_from_pattern';'* * *'
makefn 'pcre2_match_data_free';'n *'
makefn 'pcre2_pattern_info';'i * i *c'
NB. compile
NB.
NB. globals:
NB. cmhandles is a list of handles
NB. cmpatterns is a corresponding list of patterns
NB. cmtable is a corresponding table of compile,match,nsub

NB. =========================================================
NB. compile pattern, return handle
rxcomp=: 3 : 0
if. -.ischar y do. reghandlecheck y return. end.
ndx=. cmpatterns i. <y
if. ndx < #cmpatterns do. ndx{cmhandles return. end.
regcomp y
cmpatterns=: cmpatterns,<y
cmhandles=: cmhandles, cmseq=: cmseq + 1
cmtable=: cmtable,lastcomp,lastmatch,lastnsub
lasthandle=: ndx{cmhandles
)

NB. =========================================================
rxfree1=: 3 : 0
ndx=. cmhandles i. y
if. ndx=#cmhandles do. EMPTY return. end.
if. -. lastpattern -: ndx pick cmpatterns do.
  'comp match nsub'=. ndx{cmtable
  jpcre2_match_data_free <<match
  jpcre2_code_free <<comp
end.
ndx=. <<<ndx
cmtable=: ndx{cmtable
cmhandles=: ndx{cmhandles
cmpatterns=: ndx{cmpatterns
EMPTY
)

rxfree=: EMPTY [ rxfree1 &>

NB. =========================================================
rxinfo=: 3 : 0
ndx=. cmhandles i. y
if. ndx=#cmhandles do. 'handle not found: ',":y return. end.
((<ndx;2){cmtable);ndx{cmpatterns
)

NB. =========================================================
rxhandles=: 3 : 'cmhandles'
NB. main methods

rxfrom=: ,."1@[ <;.0 ]
rxeq=: {.@rxmatch -: 0 , #@]
rxin=: _1 ~: {.@{.@rxmatch
rxindex=: #@] [^:(<&0@]) {.@{.@rxmatch
rxE=: i.@#@] e. {.@{."2 @ rxmatches
rxfirst=: {.@rxmatch >@rxfrom ]
rxall=: {."2@rxmatches rxfrom ]

NB. =========================================================
rxapply=: 1 : 0
:
if. L. x do. 'pat ndx'=. x else. pat=. x [ ndx=. ,0 end.
if. 1 ~: #$ ndx do. 13!:8[3 end.
mat=. ({.ndx) {"2 pat rxmatches y
r=. u&.> mat rxfrom y
r mat rxmerge y
)

NB. =========================================================
rxcut=: 4 : 0
if. 0 e. #x do. <y return. end.
'beg len'=. |: ,. x
if. 1<#beg do.
  whilst. 0 e. d do.
    d=. 1,<:/\ (}:len) <: 2 -~/\ beg
    beg=. d#beg
    len=. d#len
  end.
end.
a=. 0, , beg ,. beg+len
b=. 2 -~/\ a, #y
f=. < @ (({. + i.@{:)@[ { ] )
(}: , {: -. a:"_) (a,.b) f"1 y
)

NB. =========================================================
rxerror=: 3 : 'lasterror'

NB. =========================================================
rxmatch=: 4 : 0
'p n'=. 2 {. boxopen x
regcomp p
r=. regmatch1 y
if. #n do. n{r end.
)

NB. =========================================================
rxmatches=: 4 : 0
'p n'=. 2 {. boxopen x
regcomp p
m=. regmatch1 y
if. _1 = {.{.m do. i.0 1 2 return. end.
s=. 1 >. +/{.m
r=. ,: m
while. s <#y do.
  if. _1 = {.{.m=. regmatch2 y;s do. break. end.
  s=. (s+1) >. +/ {.m
  r=. r, m
end.
if. #n do. n{"2 r end.
)

NB. =========================================================
rxmerge=: 1 : 0
:
p=. _2 ]\ m rxcut y
;, ({."1 p),.(#p){.(#m)$x
)

NB. =========================================================
rxrplc=: 4 : 0
pat=. >{.x
new=. {:x
if. L. pat do. 'pat ndx'=. pat else. ndx=. ,0 end.
if. 1 ~: #$ ndx do. 13!:8[3 end.
m=. pat rxmatches y
if. 0 e. $m do. y return. end.
mat=. ({.ndx) {"2 m
new mat rxmerge y
)

NB. =========================================================
rxutf8=: 3 : '(RX_OPTIONS_UTF8=: y) ] RX_OPTIONS_UTF8'
NB. nouns for applying regular expressions to J code
NB.
NB.   Jname        name
NB.   Jnumitem     numeric item
NB.   Jnum         number or blank
NB.   Jcharitem    character item
NB.   Jchar        character
NB.   Jconst       constant
NB.
NB.   Jlassign     local assign
NB.   Jgassign     global assign
NB.   Jassign      any assign
NB.
NB.   Jlpar        left paren
NB.   Jrpar        right paren
NB.
NB.   Jsol         start of line
NB.   Jeol         end of line

Jname=: '[[:alpha:]][[:alnum:]_]*|x\.|y\.|m\.|n\.|u\.|v\.'
Jnumitem=: '[[:digit:]_][[:alnum:]_.]*'
Jnum=: '([[:digit:]_][[:alnum:]_.]*|[[:blank:]])?'
Jcharitem=: '''(''''|[^''])'''
Jchar=: '''(''''|[^''])*'''
Jconst=: '([[:digit:]_][[:alnum:]_.]*|[[:blank:]])?|''(''''|[^''])*''|a:|a\.'
Jlassign=: '=\.'
Jgassign=: '=:'
Jassign=: '=[.:]'
Jlpar=: '\('
Jrpar=: '\)'
Jsol=: '^[[:blank:]]*'
Jeol=: '[[:blank:]]*(NB\..*)?$'
NB. fini

NB. =========================================================
3 : 0''
if. _1=nc <'cmhandles' do.
  cmhandles=: cmpatterns=: $0
  cmseq=: 0
  cmtable=: i.0 3
end.
lasterror=: ''
regclear''
)

NB. =========================================================
NB. define z locale names:
nms=. 0 : 0
rxmatch rxmatches rxcomp rxfree rxhandles rxinfo rxeq
rxin rxindex rxE rxfirst rxall rxrplc rxapply rxerror
rxcut rxfrom rxmerge rxutf8
)

nms=. (nms e.' ',LF) <;._2 nms
". > nms ,each (<'_z_=:') ,each nms ,each <'_jregex_'
