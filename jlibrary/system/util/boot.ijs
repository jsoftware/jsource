18!:4 <'z' NB. start in z

NB. =========================================================
startupconsole=: 3 : 0
f=. jpath '~config/startup_console.ijs'
if. 1!:4 :: 0: <f do.
  18!:4 <'base'
  try.
    load f
  catch.
    smoutput 'An error occurred when loading startup script: ',f
  end.
end.
)

NB. =========================================================
startupide=: 3 : 0
startup=. jpath '~config/startup.ijs'
if. 1!:4 :: 0: <startup do.
  18!:4 <'base'
  try.
    load startup
  catch.
    smoutput 'An error occurred when loading startup script: ',startup
  end.
end.
)

NB. =========================================================
NB. android specific
startupandroid=: 3 : 0
load^:IFQT^:(fexist@jpath) '~addons/gui/android/android.ijs'
dver_z_=: 3 : '1!:55 ::0: <jpath ''~install/assets_version.txt'''
if. (0~:4!:0<'UserNumber_ja_')>IFQT do.
NB. assume user 0, but may need to override in startup.ijs per user
  UserNumber_ja_=: 0- 402 > 100#. 2{. 0&". ;._1'.', LF-.~ 2!:0 'getprop ro.build.version.release'
end.
welcome=. jpath '~install/welcome.ijs'
if. 1!:4 :: 0: <welcome do.
  18!:4 <'base'
  0!:0 <welcome
end.
)

NB. =========================================================
3 : 0''
ndx=. ({."1 SystemFolders_j_) i. <'system'
sys=. '/' ,~ > 1 { ndx { SystemFolders_j_

NB. ---------------------------------------------------------
NB. following assumes Displayload is set in immex, then
NB. the profile loaded. No need for another ijx window
if. 0=4!:0<'Displayload_j_' do.
  Displayload_j_=: 1
  boot=. 3 : ('0!:0 <y[y 1!:2[2') @ jpathsep @ (sys&,)
else.
  Displayload_j_=: 0
  boot=. 3 : '0!:0 <y' @ jpathsep @ (sys&,)
end.

NB. ---------------------------------------------------------
boot 'main/stdlib.ijs'
load '~system/util/scripts.ijs'
load 'regex'
load 'task'
load '~system/util/configure.ijs'
load '~system/main/ctag.ijs'
load '~system/util/jadetag.ijs'
load^:IFQT^:(fexist@jpath) '~addons/ide/qt/qt.ijs'
startupandroid^:('Android'-:UNAME)''
startupide''

NB. ---------------------------------------------------------
NB. JVERSION_z_ (used in about box)
r=. 'Engine: ',9!:14''
r=. r,LF,'Library: ',LF -.~ 1!:1<jpath '~system/config/version.txt'
if. IFQT do.
  r=. r,LF,'Qt IDE: ',wd'version'
end.
r=. r,LF,'Platform: ',UNAME,' ',IF64 pick '32';'64'
r=. r,LF,'Installer: ',LF -.~ 1!:1 :: ('unknown'"_) <jpath'~bin/installer.txt'
r=. r,LF,'InstallPath: ',jpath '~install'
JVERSION=: toJ r

NB. ---------------------------------------------------------
NB. set break
if. -. IFIOS +. 'Android'-:UNAME do.
  setbreak 'default'
end.

NB. ---------------------------------------------------------
ndx=. <./ ARGV i. '-jp';'-jprofile'
jsx=. ARGV i. <'-js'
if. ndx < #ARGV do. p=. 2 + ndx else. p=. 1 end.
load__ >{. p }. jsx {. ARGV

if. jsx<#ARGV do.
  ARGVVERB_z_=: 3 : ((>:jsx)}.ARGV) NB. define in z
  ARGVVERB__'' NB. run in base
end.

EMPTY
)

NB. =========================================================
18!:4 <'base' NB. end in base
