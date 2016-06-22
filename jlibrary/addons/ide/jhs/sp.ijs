NB. simple project manager and related tools

require'~addons/ide/jhs/jfif.ijs'

coclass'jsp'

sp_z_       =: sp_jsp_
spf_z_      =: spf_jsp_
spr_z_      =: spr_jsp_
spx_z_      =: spx_jsp_
spxinit_z_  =: spx_z_          NB.! kill off when Jd updated
sprunner_z_ =: 3 : '0!:111 y'
sprunnerx_z_=: 3 : '0!:110 y'
sptable_z_  =: sptable_jsp_

sphelp_z_=: 0 : 0
sp (simple project) defined by ~addons/ide/jhs/sp.ijs
part of JHS and can be loaded in any front end

see spxhelp for managed execution info

simple project:
 sp  f    set SPFILE and load it
 sp''     load SPFILE
 ctrl+,   load SPFILE (JHS shortcut)

sp example:
 'echo 123'fwrite f=: '~temp/a.ijs'
 sp f
 ctrl+, (JHS shortcut)
 sp''  
 SPFILE carried over sessions
 ctrl+, in new session loads SPFILE

SPFILES/shortname:
 sp use of an ijs file adds it to SPFILES
 SPFILES carried over sessions

SPFILES/shortname example:
 'echo 456'fwrite f=: '~temp/b.ijs'
 spf f   NB. sp use adds to SPFILES
 spf 'b' NB. shortname to name
 spr 3   NB. shortnames and 3{.SPFILES
 sp  'b' NB. set SPFILE from shortname and load it
 spf ''  NB. SPFILE
 
utilities:
 sp  f    set SPFILE load it
 spr n    shortnames and n{.SPFILES
 spf f    file from filename or shortname or SPFILE
 sptable 3 2$'asdf';123;'def';(i.2 3);'q';23
)

spxhelp_z_=: 0 : 0
simple project managed execution:
 spx f        set SPXFILE
 crtl+.       JHS advance
 ctrl+j       JQt advance
 spx''        advance
 spx 0        status
 spx n        run line n
 spx n m      run lines n through m
 spx''        advance

managed execution example:
 spx'~addons/ide/jhs/spxdemo.ijs' 
)

MAXRECENT=: 40 NB. max recent files 
sprecentf=: '~temp/sp/recent.txt'
spspf    =: '~temp/sp/sp.txt'

cfile=: 3 : 0
t=. fread y
>(_1-:t){t;''
)

sp=: 3 : 0
t=. spf y
assert. (fexist t)['must exist'
SPFILE_z_=: t
t fwrite spspf
load__ t
)

NB. only add .ijs and non temp files
addrecent=: 3 : 0
t=. >(('.ijs'-:_4{.y)*.-.*/(;shorts_jsp_ y)e.'0123456789'){'';y
t=. ~.SPFILES,~<t
t=. (;fexist t)#t
SPFILES_z_=: (MAXRECENT<.#t){.t
(;SPFILES,each LF) fwrite sprecentf
y
)

NB. filename / '' for project / spr shortname
NB. first spr shortname prefix that matches 
NB. error if file does not exist or is not .ijs
NB. file is added to head of of recent
spf=: 3 : 0
if. ''-:y do.
 assert. fexist SPFILE['project must exist'
 r=. SPFILE
elseif. +./y e.'~/.' do.
 r=. y
elseif. 1 do.
 c=. (shorts SPFILES)=<,y
 assert. 0~:+/c['not found in recent'
  r=. ,>{.c#SPFILES
end.
assert. fexist r['must exist'
assert. '.ijs'-:_4{.r['not .ijs'
addrecent r
)

shorts=: 3 : 0
t=. '/',each boxopen y
_4}.each(>:>t i:each '/')}.each t
)

spr=: 3 : 0
addrecent'' NB. brute force cleanup
t=. y{.SPFILES
sptable(shorts_jsp_ t),.t
)

spxinit=: 3 : 0
assert. fexist spf y['must exist'
if. IFJHS do.
 ev_dot_ctrl_jijx_=: 3 : 'spx__'''''
 a=. 'ctrl+. advances'
elseif. IFQT do.
 if. _1=4!:0<'qtsave' do.
  qtsave=: 5!:1<'labs_run_jqtide_'
 end.
 labs_run_jqtide_=: 3 : 'spxqt_jsp_ y'
 a=. 'ctrl+j advances'
elseif. 1 do.
  a=. 'spx'''' NB. advances (create a shortcut key!)'
end.
echo a,' (see noun spxhelp)'
SPXFILE_z_=: spf y
SEM=: get SPXFILE
SEMN=: 1
status''
i.0 0
)

spxqt=: 3 : 0
if. 0-:y do.
 spx''
else.
 labs_run_jqtide_=: qtsave 5!:0
 labs_run_jqtide_ y
end. 
)

spx=: 3 : 0
if. (0~:#y)*.2=3!:0 y do. spxinit y return. end.
if. -.fexist SPXFILE do. smoutput 'not initialized - do spxinit' return. end.
if. ''-:y do. spx SEMN return. end.
if. 0={.y do. status'' return. end.
d=. SEM
SEM=:get SPXFILE
if. 2=#y do.
 if. -.(3!:0[4) e. 1 4 do. smoutput 'not integer line numbers' return. end.
 SEMN=: {.y
 b=. ({:y)<.#SEM
 while. SEMN<:b do.
  spx a
  a=. SEMN
 end.
 i.0 0
 return.
end.

if. (0~:$$y)+.-.(3!:0[4) e. 1 4 do. smoutput 'not integer line number' return. end.
if. y<0 do. smoutput 'not valid line number' return. end.
SEMN=: y
label_top.
if. SEMN>#SEM do. 'end of script' return. end.
ot=. 0 NB. lines
ndx=. <:SEMN
d=. >ndx{SEM
if. 0=#d-.' ' do. SEMN=:>:SEMN[echo ;IFJHS{'';LF goto_top. end.

if. '0 : ''spx hr'''-:12{.deb d do.
 SEMN=:>:SEMN
 if. IFJHS do. jhtml'<hr/>' else. echo 80$'_'end.
 goto_top.
end.

if. iscolon d do. NB. collect : lines
 ot=. 1
 c=. (dltb each ndx}.SEM) i. <,')'
 d=. '   ',;LF,~each (ndx+i.>:c){SEM
 ndx=. ndx+c
end.
if. isnb d do. NB. collect comment lines
 ot=. 2
 c=. (>(3{.each dltb each ndx}.SEM) -: each <'NB.')i.0
 d=. ;LF,~each (ndx+i.c){SEM
 ndx=. ndx+<:c
end.
NB. kludge to convert =. tp =:
i=. d i.LF
t=. i{.d
if. (<'=.')e.;:t do.
 d=. (t rplc '=.';'=:'),i}.d
end.
 select. ot
 case. 0 do. NB. single line
   sprunner__ d
 case. 1 do. NB. :
  a=. <;.2 d
  b=. ;:}:;{.a
  if. b-:;:'0 : 0' do.
   echo ;}.}:a
  else.
   if. IFJHS do.
    jhtml '<font color="blue">',(jhfroma_jhs_ ;a),'</font>'
   else.
    echo ;a
   end.
  end.
  sprunnerx__ d
  
 case. 2 do. NB. multiple NB. lines
  d=. 3}.each dltb each <;.2 d
  d=. ;(*./' '=;{.each d)}.each d
  if. IFJHS do. 
   jhtml '<font color="green">',(jhfroma_jhs_ d),'</font>'
  else.
   echo d
  end. 
 end. 

SEMN=: 2+ndx
if. (SEMN<#SEMN)*.'NB.'-:3{.dlb d do. goto_top. end.
i.0 0
)

iscolon=: 3 : 0
t=. ;:y
if. (<'define')e.t do. 1 return. end.
i=. t i. <,':'
(,each':';'0')-:(i+0 1){t,'';''
)

isnb=: 3 : 0
'NB.'-:3{.dltb y
)

get=: 3 : 0
d=. toJ fread y
d=. d,(LF~:{:d)#LF
<;._2 d
)

status=: 3 : 0
smoutput (":SEMN),' of ',(":#SEM),' in  ',SPXFILE
)

sptable_z_=: 3 : 0
t=. 9!:6''
9!:7[11$' '
d=. ":y
9!:7 t 
(-.*./"1 d="1 1[' '#~{:$d)#d
)

3 : 0''
try.
if. _1=nc<'initialized' do. 
 1!:5 :: [ <jpath '~temp/sp'
 SPFILE_z_  =: cfile spspf
 SPFILES_z_ =: <;._2 cfile sprecentf
 SPXFILE_z_ =: ''
 SEMN       =: 0
end.
initialized=: 1
catch.
 smoutput'sp initialization failed'
end.
i.0 0
)

0 : 0
bind -s ^E "sp''\n"
bind -s ^R "spx''\n"
editrc fwrite '~home/.editrc'
)

