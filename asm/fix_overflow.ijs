NB. win64 convert asm integer routines to have overflow checks
require'files'

3 : 0''
if. IFUNIX do.
 path=: jpath '~home/dev/j/asm/'
 pre=: ''
 suf=: 's'
 mov50=: <TAB,'movl $50,%eax',LF
 else.
 path=: '\dev\j\asm\'
 pre=: 'win'
 suf=: 'asm'
 mov50=: <TAB,'mov eax,50',LF
end.
)

src=: path,'asm64noovf.',suf
snk=: path,pre,'asm64.',suf

testover=: 3 : 0
+./ (TAB,'not') E. >y
)

testret=: 3 : 0
'ret'-:}.4{.>y
)

curop=: 3 : 0
t=. 7{.>y
line=. (((>y)i.' '){.>y)-.CRLF,':'
if. 'asmplus'-:t do.
 CURLINE=: line
elseif. 'asmminu'-:t do.
 CURLINE=: line
elseif. 'asmtyme'-:t do.
 CURLINE=: line
elseif. 'asminne'-:t do.
 CURLINE=: line
end.
''
)

remove=: 3 : 0
'include'-:7{.tolower >y
)

show=: 3 : 0
smoutput '     ',}:>y
)

fix=: 3 : 0
r=.''
cnt=. 0
flag=. 0
d=. <;.2 toJ fread src
for_t. d do.
 curop t
 if. remove t do. continue. end.
 if. testover t do.
  smoutput CURLINE
  over=: 'over',CURLINE-.LF,':'
  cnt=. 1+cnt
  r=.r,<TAB,'jo ',over,LF
  continue.
 end.
 if. testret t do.
  r=. r,t,<over,':',LF
  r=. r,mov50
  r=. r,<TAB,'ret',LF
  continue.
 end.  
 r=. r,t
end.
Z=: r
if. IFUNIX do. r=. unixprocess r end.
(toHOST ;r) fwrite snk
cnt
)

chk=: 4 : 0
(TAB,x)-:(>:#x){.y
)


NB. move jo lines in front of any testq cmpq movq leaq and addqTAB$ lines
unixprocess=: 3 : 0
r=. ''
d=.y
i=.#d
while. i=.i-1 do.
 a=. i{d
 if. 'jo'chk >a do.
  b=. (i-1){d
  if. 'testq'chk >b do.
   smoutput 'testq'
   d=. (b,a) (i-0 1)}d
   continue.
  end.   
  if. 'cmpq'chk >b do.
   smoutput 'cmpq'
   d=. (b,a) (i-0 1)}d
   continue.
  end.
  if. 'movq'chk >b do.
   smoutput 'movq'
   d=. (b,a) (i-0 1)}d
   continue.
  end.
  if. 'leaq'chk >b do.
   smoutput 'leaq'
   d=. (b,a) (i-0 1)}d
   continue.
  end.
  if. ('addq',TAB,'$')chk >b do.
   smoutput 'addq $'
   d=. (b,a) (i-0 1)}d
   continue.
  end.
 end.
end. 
d
)

