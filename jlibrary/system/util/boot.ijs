18!:4 <'z' NB. start in z

NB. =========================================================
NB. android specific
startupandroid=: 3 : 0
dver_z_=: 3 : '1!:55 ::0: <jpath ''~bin/../../shared_prefs/jpreferences.xml'' [ 1!:55 ::0: <jpath ''~install/assets_version.txt'''
if. 0~:4!:0<'APILEVEL_ja_' do.
  APILEVEL_ja_=: 0&". LF-.~ 2!:0 'getprop ro.build.version.sdk'
end.
if. 0~:4!:0<'UserNumber_ja_' do.
NB. multiple user space since APILEVEL 17
NB. assume user 0, but may need to override in startup.ijs per user
  UserNumber_ja_=: 0-APILEVEL_ja_<17
end.
EMPTY
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
load sys,'util/scripts.ijs'
load 'regex'
load 'task'
load sys,'util/configure.ijs'
load^:IFQT '~addons/ide/qt/qt.ijs'
load^:IFJA '~addons/ide/ja/ja.ijs'
load^:((;:'jwin32 jjava')e.~<11!:0 ::0:'qwd') 'ide/jnet'
load^:((;:'jwin32 jjava')e.~<11!:0 ::0:'qwd') 'ide/jnet/util/jadefull'

NB. ---------------------------------------------------------
NB. JVERSION_z_ (used in about box)
v=. 9!:14''
if. 6>+/v='/' do.
  r=. 'Engine: ',v
  f=. 'www.jsoftware.com'
else.
  'a b c d e f g'=. <;._1 '/',v
  r=. 'Engine: ', a,'/',b,'/',c
  r=. r,LF,(toupper {.d),(}.d),': ',e,'/',g
end.
r=. r,LF,'Library: ',LF -.~ 1!:1<jpath '~system/config/version.txt'
if. IFQT do.
  r=. r,LF,'Qt IDE: ',wd'version'
elseif. IFJA do.
  r=. r,LF,'J Android: ',wd'version'
end.
if. UNAME-:'Android' do.
  r=. r,LF,'Platform: ',UNAME,' ',(IF64 pick '32';'64'),' (', ')',~ LF-.~ 2!:0'getprop ro.product.cpu.abi'
else.
  r=. r,LF,'Platform: ',UNAME,' ',IF64 pick '32';'64'
end.
r=. r,LF,'Installer: ',LF -.~ 1!:1 :: ('unknown'"_) <jpath'~bin/installer.txt'
r=. r,LF,'InstallPath: ',jpath '~install'
r=. r,LF,'Contact: ',f
JVERSION=: toJ r

NB. ---------------------------------------------------------
18!:4 <'base'

if. 'Android'-:UNAME do.
  startupandroid''
  f=. jpath '~install/bin/startup_android.ijs'
  if. 1!:4 :: 0: <f do.
    try.
      load f
    catch.
      wdinfo ::0: 13!:12''
      2!:55[0
    end.
    EMPTY return.
  end.
end.

NB. f=. jpath '~config/startup',((-.IFQT)#'_console'),'.ijs'
f=. jpath '~config/startup.ijs'
if. 1!:4 :: 0: <f do.
  try.
    load f
  catch.
    smoutput 'An error occurred when loading startup script: ',f
  end.
end.

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

NB. ---------------------------------------------------------
if. jsx<#ARGV do.
  v=. (>:jsx)}.ARGV
  if. #;v do.
    ARGVVERB_z_=: 3 : v
    ARGVVERB__''
  end.
end.

EMPTY
)

NB. =========================================================
18!:4 <'base' NB. end in base
