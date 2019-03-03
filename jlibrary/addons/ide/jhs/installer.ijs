0 : 0
routines to update/install base and addons and create desktop launch icons

   installer 0   NB. update all, install qjt, and create launch icons
   
   shortcut'jc'  NB. create jconsole launch icon
   shortcut'jhs'
   shortcut'jqt'
)

JQT=: 1       NB. 0 to avoid jqt install
SHORTCUTS=: 1 NB. 0 to avoid creating shortcuts

installer=: 3 : 0
echo'these steps can take several minutes'
echo' '
update_install_all 0
echo' '
if. JQT do.
 install'qtide'
 echo' '
 if. SHORTCUTS do. shortcut'jqt' end.
end.
if. SHORTCUTS do.
 shortcut'jc'
 shortcut'jhs'
 echo'double click a desktop icon to run J with the corresponding user interface'
end. 
i.0 0
)

NB. update and install all (base library and addons)
update_install_all=: 3 : 0
require'pacman'
'update'jpkg''
'upgrade'jpkg'all'
'install'jpkg {."1'shownotinstalled'jpkg''
)

NB. shortcut 'jc' or 'jhs' or 'jqt' - create desktop launch icon
shortcut=: 3 : 0
e=. 'create ',y,' launch icon failed'
if. 2~:ftype jpath'~/Desktop' do. echo e,' - no Desktop folder' return. end.
try. ".UNAME,'_jinstaller_ y' catchd. echo e end. 
)

NB. switch to avx JE if available and it runs
avx=: 3 : 0
i=. (;:'Win Linux Darwin')i.<UNAME
pre=. ;i{'';'lib';'lib'
suf=. ;i{'dll';'so';'dylib'
t=. jpath'~bin/'
f=. pre,'j.',suf
favx=. pre,'javx.',suf
if. 'avx'-:3{.8}.9!:14'' do. echo'avx JE already running' return. end.
if. -.fexist t,favx do. echo favx,' JE does not exist' return. end.
try.
 a=. shell q=: '"',t,'jconsole','" -lib ',favx,' -js exit[0[echo[i.~i.10'
catch.
 a=. ''
end.
if. (a-.CRLF)-:":i.10 do.
 echo'avx JE runs!'
 (t,f,'.original') frename t,f
 (fread t,favx)fwrite t,favx,'.original'
 (t,f) frename t,favx
 echo'avx JE is now the default JE'
 echo'exit and restart'
else.
 echo'avx JE fails'
 echo'this system can not run the avx JE binary'
end.
)

coclass'jinstaller'

defaults=: 3 : 0
A=:   ' ~addons/ide/jhs/config/jhs.cfg'
L=:   hostpathsep jpath'~/Desktop/'
W=:   hostpathsep jpath'~'
I=:   hostpathsep jpath'~bin/icons/'
N=:   (1 2 3{9!:14''),;IF64{'-32';''
DS=:  ;(('Win';'Linux';'Darwin')i.<UNAME){'.lnk';'.desktop';'.app'
LIB=: ''
)

NB. windows
vbs=: 0 : 0
Set oWS=WScript.CreateObject("WScript.Shell")
Set oLink=oWS.CreateShortcut("<N>")
oLink.TargetPath="<C>"
oLink.Arguments="<A>"
oLink.WorkingDirectory = "<W>"
oLink.IconLocation="<I>"
oLink.Save
)

Win=: 3 : 0
defaults''
Winx y
)

Winx=: 3 : 0
select. y
case.'jc' do.
 win'jc' ;'jconsole';'jgray.ico';LIB
case. 'jhs' do. 
 win'jhs';'jconsole';'jblue.ico';LIB,A
case. 'jqt' do.
 win'jqt';'jqt'     ;'jgreen.ico';LIB
case. do.
 assert 0
end.
)

win=: 3 : 0
'type bin icon arg'=.y
f=. jpath '~temp/shortcut.vbs'
n=. L,type,N,DS
c=. hostpathsep jpath '~bin/',bin
(vbs rplc '<N>';n;'<C>';c;'<A>';arg;'<W>';W;'<I>';I,icon) fwrite f
r=. spawn_jtask_ 'cscript "',f,'"'
r assert -.'runtime error' E. r
ferase f
i.0 0
)

NB. Linux (Ubuntu)
desktop=: 0 : 0
[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Name=<N>
Exec=<E>
Path=<W>
Icon=<I>
)

NB. Linux (RedHat and Centos)
desktoprh=: 0 : 0
[Desktop Entry]
Version=1.0
Type=Application
Terminal=<TT>
Name=<N>
Exec=<E>
Path=<W>
Icon=<I>
)

lter=: 0 : 0

x-terminal-emulator does not exist and does not link to your preferred terminal
edit launch icon properties and change x-terminal-emulator to be your preferred terminal
 or
create it with: sudo update-alternatives --config x-terminal-emulator

)

get_terminal=: 3 : 0
t=. 'x-terminal-emulator'
if. 0=shell :: 0:'which ',t do. echo lter end.
t
)

Linux=: 3 : 0
defaults''
Linuxx y
)

Linuxx=: 3 : 0
select. y
case.'jc' do.
 linux'jc' ;'jconsole';'jgray.png';LIB
case. 'jhs' do. 
 linux'jhs';'jconsole';'jblue.png';LIB,A
case. 'jqt' do.
 linux'jqt';'jqt'     ;'jgreen.png';LIB
case. do.
 assert 0
end.
i.0 0
)

linux=: 3 : 0
'type bin icon arg'=.y
n=. type,N
f=. L,type,N,DS
c=. hostpathsep jpath '~bin/',bin
rh=. 1<#fread '/etc/redhat-release' 
if. rh do.
 if. type-:'jqt' do.
  e=. c
 else.
  e=. c,' ',arg
 end.
else.
 if. type-:'jqt' do.
  e=. '"',c,'"'
 else.
  e=. '<T> -e "\"<C>\"<A>"'rplc '<T>';(get_terminal'');'<C>';c;'<A>';arg
 end.
end.

if. rh do.
 r=. desktoprh rplc '<N>';n
 r=. r rplc '<E>';e
 r=. r rplc '<W>';W
 r=. r rplc '<I>';I,icon
 r=. r rplc '<TT>';(type-:'jc'){'false';'true'
 r fwrite f
 2!:0'chmod +x ',f
else.
 r=. desktop rplc '<N>';n
 r=. r rplc '<E>';e
 r=. r rplc '<W>';W
 r=. r rplc '<I>';I,icon
 r fwrite f
 2!:0'chmod +x ',f
end.
)

NB. Darwin
plist=: 0 : 0
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>apprun</string>
<key>CFBundleIconFile</key><string>i.icns</string>
<key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
<key>CFBundleName</key><string>j</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleVersion</key><string>1.0</string>
</dict></plist>
)

COM=: jpath'~temp/launch.command'

Darwin=: 3 : 0
defaults''
Darwinx y
)


Darwinx=: 3 : 0
select. y
case.'jc' do.
 darwin'jc' ;'jconsole';'jgray.icns';LIB
case. 'jhs' do. 
 darwin'jhs';'jconsole';'jblue.icns';LIB,A
case. 'jqt' do.
 darwin'jqt';'jqt'     ;'jgreen.icns';LIB
case. do.
 assert 0
end.
i.0 0
)

NB. 1 jhs - 0 jconsole
darwin=: 3 : 0
'type bin icon arg'=.y
n=. type,N
f=. L,type,N,DS
c=. hostpathsep jpath '~bin/',bin
select. type
case.'jc' do.
 NB. r=. '#!/bin/sh',LF,'open "',c,'"'
 r=. launch rplc '<COM>';COM;'<C>';(hostpathsep jpath '~bin/jconsole');'<A>';LIB
case. 'jhs' do.
 r=. launch rplc '<COM>';COM;'<C>';(hostpathsep jpath '~bin/jconsole');'<A>';LIB,A
case. 'jqt' do.
 r=.'#!/bin/sh',LF,'"',c,'.command" ',LIB
end.
fpathcreate f,'/Contents/MacOS'
fpathcreate f,'/Contents/Resources'
plist fwrite f,'/Contents/info.plist'
r fwrite f,'/Contents/MacOS/apprun'
(fread '~bin/icons/',icon) fwrite f,'/Contents/Resources/i.icns'
2!:0'chmod -R +x ',f
)

launch=: 0 : 0 
#!/bin/sh
echo '#!/bin/sh' > "<COM>"
echo '"<C>" <A>' >> "<COM>"
chmod +x <COM>
open "<COM>"
)
