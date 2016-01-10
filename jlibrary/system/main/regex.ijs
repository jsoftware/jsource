NB. Regular expression pattern matching
NB.
NB. PCRE: Perl-compatible regular expression library
NB. with POSIX interface
NB.
NB. =========================================================
NB. main definitions:
NB.   rxmatch          single match
NB.   rxmatches        all matches
NB.
NB.   rxcomp           compile pattern
NB.   rxfree           free pattern handles
NB.   rxhandles        list pattern handles
NB.   rxinfo           info on pattern handles
NB.
NB. regex utilities:
NB.   rxeq             -:
NB.   rxin             e.
NB.   rxindex          i.
NB.   rxE              E.
NB.   rxfirst          {.@{    (first match)
NB.   rxall            {       (all matches)
NB.   rxrplc           search and replace
NB.   rxapply          apply verb to pattern
NB.
NB.   rxerror          last regex error message
NB.
NB. other utilities:
NB.   rxcut            cut string into nomatch/match list
NB.   rxfrom           matches from string
NB.   rxmerge          replace matches in string
NB.
NB. =========================================================
NB. Form:
NB.   here:  pat      = pattern, or pattern handle
NB.          phnd     = pattern handle
NB.          patndx   = pattern;index  or  phnd;index
NB.          str      = character string
NB.          bstr     = boxed list of str
NB.          mat      = result of regex search
NB.          nsub     = #subexpressions in pattern
NB.
NB.  mat=.  pat or patndx   rxmatch   str
NB.  mat=.  pat or patndx   rxmatches str
NB.
NB.  phnd=.                 rxcomp    pat
NB.  empty=.                rxfree    phnd
NB.  phnds=.                rxhandles ''
NB.  'nsub pat'=.           rxinfo    phnd
NB.
NB.  boolean=.        pat   rxeq      str
NB.  index=.          pat   rxindex   str
NB.  mask=.           pat   rxE       str
NB.  bstr=.           pat   rxfirst   str
NB.  bstr=.           pat   rxall     str
NB.  str=.     (patndx;new) rxrplc    str
NB.  str=.     patndx (verb rxapply)  str
NB.
NB.  errormsg=.             rxerror   ''
NB.
NB.  bstr             mat   rxcut     str
NB.  bstr=.           mat   rxfrom    str
NB.  str=.         new (mat rxmerge)  str

NB. =========================================================
NB. following defined in z:
NB.*rxmatch v single match
NB.*rxmatches v all matches
NB.*rxcomp v compile pattern
NB.*rxfree v free pattern handles
NB.*rxhandles v list pattern handles
NB.*rxinfo v info on pattern handles
NB.*rxeq v regex equivalent of -:
NB.*rxin v regex equivalent of e.
NB.*rxindex v regex equivalent of i.
NB.*rxE v regex equivalent of E.
NB.*rxfirst v regex equivalent of {.@{ (first match)
NB.*rxall v regex equivalent of { (all matches)
NB.*rxrplc v search and replace
NB.*rxapply v apply verb to pattern
NB.*rxerror v last regex error message
NB.*rxcut v cut string into nomatch/match list
NB.*rxfrom v matches from string
NB.*rxmerge v replace matches in string
NB.*rxutf8 v set UTF-8 support 1=on(default), 0=off

coclass <'jregex'

NB. =========================================================
NB. flag to enable UTF-8 support
RX_OPTIONS_UTF8=: 1

Rxnna=: '(^|[^[:alnum:]_])'
Rxnnz=: '($|[^[:alnum:]_.:])'
Rxass=: '[[:space:]]*=[.:]'
NB. defs

NB. =========================================================
NB. rxdll is in bin or tools/regex
3 : 0''
select. UNAME
case. 'Win' do. t=. 'jpcre.dll'
case. 'Darwin' do. t=. 'libjpcre.dylib'
case. do. t=. 'libjpcre.so'
end.
if. (0~:FHS) *: 'Linux'-:UNAME do.
  f=. BINPATH,'/',t
  if. 0 = 1!:4 :: 0: <f do.
    f=. jpath '~tools/regex/',t
  end.

NB. fall back one more time for android
  if. ('Android'-:UNAME) *. 0 = 1!:4 :: 0: <f do.
    f=. AndroidLibPath,'/',t
  end.

  rxdll=. '"',f,'"'

NB. =========================================================
NB. J DLL calls corresponding to the four extended regular expression
NB. functions defined in The Single Unix Specification, Version 2
  jregcomp=: (rxdll,' regcomp + i *x *c i')&(15!:0)
  jregexec=: (rxdll,' regexec + i *x *c x *i i')&(15!:0)
  jregerror=: (rxdll,' regerror + x i * *c x')&(15!:0)
  jregfree=: (rxdll,' regfree + n *x')&(15!:0)

else.

  rxdll=. 'libpcreposix.so.3'

  jregcomp=: (rxdll,' pcreposix_regcomp + i *x *c i')&(15!:0)
  jregexec=: (rxdll,' pcreposix_regexec + i *x *c x *i i')&(15!:0)
  jregerror=: (rxdll,' pcreposix_regerror + x i * *c x')&(15!:0)
  jregfree=: (rxdll,' pcreposix_regfree + n *x')&(15!:0)

end.
''
)
NB. regex

NB. =========================================================
NB. Global definitions used by the regex script functions
rxmp=: 50 NB. Allocation granule size for compiled patterns.
rxms=: 50 NB. Maximum number of sub-expressions per pattern.
rxszi=: IF64{4 8
rxregxsz=: 3 NB. J ints for pcre regex_t
re_nsub_off=: 1
rxlastrc=: 0
rxlastxrp=: rxregxsz$2-2
NB. rxpatterns defined only if not already defined
rxpatterns_jregex_=: (3 0 $ _1 ; rxlastxrp ; '') [^:(0:=#@]) ". 'rxpatterns_jregex_'

NB. =========================================================
NB. rxmatch
rxmatch=: 4 : 0
if. lb=. 32 = 3!:0 x do. ph=. >0{x else. ph=. x end.
if. cx=. 2 = 3!:0 ph do. hx=. rxcomp ph
else. rxlastxrp=: > 1{((hx=. ph) - 1) ({"1) rxpatterns end.
nsub=. rxnsub rxlastxrp
rxlastrc=: >0{rv=. jregexec rxlastxrp ; (,y) ; rxms ; ((2*rxms)$_1 0) ; 0
if. cx do. rxfree hx end.
m=. (nsub,2)$>4{rv
t=. (0{"1 m)
m=. t,.-~/"1 m
m=. _1 0 ((t=_1)#i.#t)} m
if. lb do. (>1{x){ m else. m end.
)

NB. =========================================================
NB. rxmatches
rxmatches=: 4 : 0
if. lb=. 32 = 3!:0 x do.
  ph=. >0{x else. ph=. x end.
if. cx=. 2 = 3!:0 ph do.
  hx=. rxcomp ph else.	NB. rxcomp sets rxlastxrp
  rxlastxrp=: > 1{((hx=. ph) - 1) ({"1) rxpatterns end.
nsub=. rxnsub rxlastxrp
o=. 0
rxm=. (0, nsub, 2)$0
while. 1 do.
  m=. hx rxmatch o}.y
  if. 0 e. $m do. break. end.
  if. _1 = 0{0{m do. break. end.
  m=. m+ ($m)$o,0
  rxm=. rxm , m
NB. Advance the offset o beyond this match.
NB. The match length can be zero (with the *? operators),
NB. so take special care to advance at least to the next
NB. position.  If that reaches beyond the end, exit the loop.
  o=. (>:o) >. +/0{m
  if. o >: #y do. break. end.
end.
if. cx do. rxfree hx end.
if. lb do. (>1{x){"2 rxm else. rxm end.
)

NB. =========================================================
NB. rxcomp
NB.
NB. options rxcomp pattern
rxcomp=: 3 : 0
'rxlastrc rxlastxrp'=: 2 {. jregcomp (rxregxsz$2-2); (,y); 2 + RX_OPTIONS_UTF8*16b40
if. rxlastrc do. (rxerror'') 13!:8 [12 end.
if. ({:$rxpatterns) = hx=. (<_1) i.~ 0 { rxpatterns do.
  rxpatterns=: rxpatterns ,. (rxmp$<_1),(rxmp$<rxregxsz$2-2), ,:rxmp$<''
end.
rxpatterns=: ((hx+1);rxlastxrp;y) (<a:;hx)} rxpatterns
hx + 1
)

NB. =========================================================
rxnsub=: [: >: 1&{   NB. Number of main+sub-expressions from Perl regex_t

NB. =========================================================
NB. rxerror
rxerror=: 3 : 0
r=. >3{jregerror rxlastrc;rxlastxrp;(80#' ');80
({.~ i.&(0{a.)) r
)

NB. =========================================================
rxfree=: 3 : 0
hx=. ,y - 1
while. 0<#hx do.
  ix=. 0{hx
  jregfree 1{ix ({"_1) rxpatterns
  rxpatterns=: ((<_1),(<rxregxsz$2-2),<'') (<(<$0);ix)} rxpatterns
  hx=. }.hx
end.
i.0 0
)

NB. =========================================================
NB. rxhandles
rxhandles=: 3 : 0
h=. >0{rxpatterns
(h~:_1)#h
)

NB. =========================================================
NB. rxinfo
rxinfo=: 3 : 0
i=. (y-1){"1 rxpatterns
|:(<"_1 rxnsub >1{i) ,: 2{i
)

NB. =========================================================
NB. rxfrom=: <@({~ (+ i.)/)"1~
rxfrom=: ,."1@[ <;.0 ]
rxeq=: {.@rxmatch -: 0: , #@]
rxin=: _1: ~: {.@{.@rxmatch
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
mat=. ({.ndx) {"2 pat rxmatches y
new mat rxmerge y
)

NB. =========================================================
NB. set UTF-8 support on/off
NB. result is previous setting
rxutf8=: 3 : 0
(RX_OPTIONS_UTF8=: y) ] RX_OPTIONS_UTF8
)
NB. zdefs

NB. =========================================================
NB. define z locale names:
nms=. 0 : 0
rxmatch rxmatches rxcomp rxfree rxhandles rxinfo rxeq
rxin rxindex rxE rxfirst rxall rxrplc rxapply rxerror
rxcut rxfrom rxmerge rxutf8
)

nms=. (nms e.' ',LF) <;._2 nms
". > nms ,each (<'_z_=:') ,each nms ,each <'_jregex_'
