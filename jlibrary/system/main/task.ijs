NB. task
NB.
NB. executing tasks with optional timeout or I/O
NB.
NB. TASKS WITHOUT I/O
NB.
NB.   fork_jtask_ 'notepad.exe'           NB. run notepad, no wait, no I/O
NB.   5000 fork_jtask_ 'notepad.exe'      NB. run notepad, wait 5 sec, no I/O
NB.   _1 fork_jtask_ 'notepad.exe'        NB. run notepad, until closed, no I/O
NB.
NB.   5000 fork_jtask_ 'cmd /k dir'  NB. show dir in cmd window for 5 sec and close
NB.   _1 fork_jtask_ 'cmd /k dir'    NB. show dir in cmd window until user closes it
NB.
NB.   launch jpath'~system'        NB. run default application, no wait
NB.
NB. TASKS WITH I/O
NB.
NB.   spawn_jtask_ 'net users'                    NB. get stdout from process
NB.   '+/i.3 4' spawn_jtask_ 'jconsole'           NB. call process with I/O
NB.   12 15 18 21
NB.
NB. SHELL COMMANDS (WITH I/O)
NB.
NB.   shell'echo.|time'                    NB. get result of shell command
NB. The current time is:  8:04:13.09
NB. Enter the new time:
NB.
NB.   (shell'dir /b')shell'find ".dll"'    NB. get all DDL names by piping
NB. j.dll
NB. jregexp.dll
NB.
NB. NOTE: the implementation uses C-type structures
NB.       by the original Method of Named Fields
NB.
NB. Script developed by Oleg Kobchenko.

coclass <'jtask'
NB. task util

int=: {.@:(_2&ic)
sint=: 2&ic

i64=: {.@:(_3&ic)
si64=: 3&ic

ptr=: int`i64@.IF64
sptr=: sint`si64@.IF64

NB. =========================================================
NB. METHOD OF NAMED FIELDS
sndx=: i.@#@[ + {.@I.@E.

NB. struct=. 'valu' 'memb' sset structdef struct
sset=: 2 : '(m sndx n)}'

NB. value=. 'memb' sget structdef struct
sget=: 2 : '(m sndx n)&{'

szero=: # # (0{a.)"_

st64=: -.&'.'^:(-.IF64)

t=. 'Cbyt....Resv....Desk....Titl....XposYposXsizYsizXcntYcnt'
STARTUPINFO=:  st64 t,'FillFlagSwRs....Resv....Inph....Outh....Errh....'
PROCESSINFO=:  st64 'Proh....Thrh....PridThid'
SECURITYATTR=: st64 'Cbyt....Secd....Inhe'

'Outh Errh Inph Proh Thrh'=: ,"1&'....'^:IF64>;:'Outh Errh Inph Proh Thrh'

STARTF_USESTDHANDLES=: 16b100
STARTF_USESHOWWINDOW=: 1
WAIT_TIMEOUT=: 258
CREATE_NEW_CONSOLE=: 16b10
DUPLICATE_SAME_ACCESS=: 2

cdk=: 1 : '(''kernel32 '',m)&cd'

WaitForSingleObject=: 'WaitForSingleObject > i x i' cdk
CloseHandle=:         'CloseHandle         > i x' cdk"0
TerminateProcess=:    'TerminateProcess    > i x i' cdk
ReadFile=:            'ReadFile            > i x *c i *i x' cdk
WriteFile=:           'WriteFile           > i x *c i *i x' cdk
GetCurrentProcess=:   'GetCurrentProcess   > x' cdk

DuplicateHandleF=:    'DuplicateHandle     > i  x x  x *x  i i i' cdk
CreatePipeF=:         'CreatePipe          > i *x *x *c i' cdk
CreateProcessF=:      'CreateProcessW      > i x *w x x i  i x x *c *c' cdk

DuplicateHandle=: 3 : 0
p=. GetCurrentProcess ''
r=. DuplicateHandleF p;y;p;(h=.,_1);0;0;DUPLICATE_SAME_ACCESS
CloseHandle y
{.h
)

NB. =========================================================
NB. 'hRead hWrite'=. CreatePipe Inheritable=0
NB.    ... FileRead/FileWrite ...
NB. CloseHandle hRead,hWrite
NB.
NB. Inheritable: 0 none, 1 for read, 2 for write
CreatePipe=: 3 : 0
'inh size'=. 2{.y,0
sa=. szero SECURITYATTR
sa=. (sint #SECURITYATTR) 'Cbyt' sset SECURITYATTR sa
sa=. (sint *inh) 'Inhe' sset SECURITYATTR sa
r=. CreatePipeF (hRead=.,_1);(hWrite=.,_1);sa;size
hRead=. {. hRead
hWrite=. {. hWrite
if. 1=inh do. hRead=. DuplicateHandle hRead end.
if. 2=inh do. hWrite=. DuplicateHandle hWrite end.
hRead,hWrite
)

NB. =========================================================
NB. hProcess=. [hWriteOut[,hReadIn]] CreateProcess 'program agr1 agr2 ...'
NB.    ...
NB. CloseHandle hProcess
CreateProcess=: 3 : 0
'' CreateProcess y
:
'ow ir'=. 2{.x,0
si=. szero STARTUPINFO
si=. (sint #STARTUPINFO) 'Cbyt' sset STARTUPINFO si
f=. inh=. 0
if. +/ir,ow do.
  inh=. 1
  f=. CREATE_NEW_CONSOLE
  si=. (sint STARTF_USESTDHANDLES+STARTF_USESHOWWINDOW) 'Flag' sset STARTUPINFO si
  if. ow do.
    si=. (sptr ow) Outh sset STARTUPINFO si
    si=. (sptr ow) Errh sset STARTUPINFO si
  end.
  if. ir do. si=. (sptr ir) Inph sset STARTUPINFO si end.
end.
pi=. szero PROCESSINFO
r=. CreateProcessF 0;(uucp y);0;0;inh; f;0;0;si;pi
if. 0=r do. 0 return. end.
ph=. ptr Proh sget PROCESSINFO pi
th=. ptr Thrh sget PROCESSINFO pi
CloseHandle th
ph
)

NB. =========================================================
NB. ph=. h CreateProcess 'program agr1 agr2 ...'
NB.    ...
NB. Wait ph;5000
NB. CloseHandle ph
Wait=: 3 : 0
r=. WaitForSingleObject y
if. WAIT_TIMEOUT=r do. TerminateProcess (0 pick y);_1 end.
)

NB. =========================================================
NB. ph=. h CreateProcess 'program agr1 agr2 ...'
NB.    ...
NB. r=. ReadAll h
NB. CloseHandle h,ph
ReadAll=: 3 : 0
ret=. ''
str=. 4096#'z'
while. 1 do.
  r=. ReadFile y;str;(#str);(len=.,_1);0
  len=. {.len
  if. (0=r)+.0=len do.
    'ec es'=: cderx''
    if. -.ec e.0 109 do. ret=. _1 end.
    break.
  end.
  ret=. ret,len{.str
end.
ret
)

NB. =========================================================
NB. ph=. hr,hw CreateProcess 'program agr1 agr2 ...'
NB. r=. WriteAll hw
NB. CloseHandle hw
NB. r=. ReadAll hr
NB. CloseHandle hr,ph
WriteAll=: 3 : 0
:
while. #x do.
  r=. WriteFile y;x;(#x);(len=.,_1);0
  len=. {. len
  if. (0=r)+.0=len do.
    'ec es'=: cderx''
    if. -.ec e.0 109 do. ret=. _1 end.
    break.
  end.
  x=. len}.x
end.
1
)
NB. task main

NB. =========================================================
NB.*fork v run task and optionally wait for completion
NB.
NB. form: [timeout=0] fork cmdline
NB.
NB.   timeout: 0 no wait, _1 infinite, >0 timeout
NB.   cmdline: 'shortcmd arg1 arg2 ...'
NB.   cmdline: '"command with space" arg1 ...'
NB.
NB. e.g. fork_jtask_ 'notepad.exe'
fork=: (3 : 0)`([: 2!:1 '(' , ')&' ,~ ])@.IFUNIX
0 fork y
:
ph=. CreateProcess y
if. x do. Wait ph;x end.
CloseHandle ph
empty''
)

NB. =========================================================
NB.*spawn v [monad] get stdout of executed task
NB.
NB. form:  stdout=. spawn cmdline
NB.
NB.   stdout: _1 fail, '' or stdout stream value if success
NB.   cmdline: 'shortcmd arg1 arg2 ...'
NB.   cmdline: '"command with space" arg1 ...'
NB.
NB. e.g. spawn 'net users'

NB.*spawn v [dyad] send stdin and get stdout of task
NB.
NB. form: stdout=. [stdin=''] spawn cmdline
NB.
NB.   stdin: input to stream as stdin, '' no input
NB.
NB. e.g. 'i.3 4'spawn'jconsole'
spawn=: (3 : 0)`(2!:0@])@.IFUNIX
'' spawn y
:
'or ow'=. CreatePipe 1
'ir iw'=. CreatePipe 2,#x
ph=. (ow,ir) CreateProcess y
CloseHandle ir
if. #x do. x WriteAll iw end.
CloseHandle iw
CloseHandle ow
r=. ReadAll or
CloseHandle or
CloseHandle ph
r
)

NB. =========================================================
NB.*shell v [monad] get stdout of shell command
NB.
NB.   e.g. shell 'dir /b/s'

NB.*shell v [dyad] send stdin and get stdout of shell command
NB.
NB.   e.g. (shell 'dir /b/s') shell 'find ".dll"'
SHELL=: IFUNIX{::'cmd /c ';''

shell=: ''&$: : (spawn SHELL,])

NB. =========================================================
NB.*launch v [monad] launch default application for parameter
NB.
NB.    launch jpath'~system'              NB. file manager
NB.    launch jpath'~bin/installer.txt'   NB. text editor
NB.    launch 'http://jsoftware.com'      NB. web browser

3 : 0''
LAUNCH=: ('gnome-open';'open';'start';'') {::~ ('Linux';'Darwin';'Win')i.<UNAME
if. 0=nc<'LAUNCH_j_'do.if. 0<#LAUNCH_j_ do.LAUNCH=: LAUNCH_j_ end.end.
)
launch=: 3 : 'shell LAUNCH,'' '',y'
NB. task zdefs

NB. fork_z_=: fork_jtask_
NB. spawn_z_=: spawn_jtask_
shell_z_=: shell_jtask_
launch_z_=: launch_jtask_
