NB. J profile
NB. JFE sets BINPATH_z_ and ARGV_z_
NB. add your sentences in startup.ijs

systype=. 9!:12''
jpathsep_z_=: '/'&(('\' I.@:= ])})
BINPATH_z_=: jpathsep BINPATH_z_
ifios=. (IFIOS"_)^:(0=4!:0<'IFIOS') 0
ifwasm=. 'Wasm' -: (UNAME"_)^:(0=4!:0<'UNAME_z_')''
omitversion=. ifios +. ifwasm +. 'Android' -: (UNAME"_)^:(0=4!:0<'UNAME_z_')''

NB. create SystemFolders
bin=. BINPATH
install=. 3 : 0 bin
if. 0=*@#1!:0<install,'/system/util/boot.ijs' [ install=. (bin i: '/'){.bin=. y do.
  for_i. i.9 do.
    if. *@#1!:0<bin,(p=. ,i#,:'/..'),'/jlibrary/system/util/boot.ijs' do. install=. bin,p,'/jlibrary' break. end.
  end.
end.
install
)
fhs=. (FHS"_)^:(0=4!:0<'FHS')(5=systype)*.0=#1!:0<install,'/system/util/boot.ijs'
install=. (0&~:fhs){::install;install,'/share/j/',omitversion{::'9.7';'0'
install=. (INSTALLROOT"_)^:(0=4!:0<'INSTALLROOT') install
addons=. install,'/addons'
system=. install,'/system'
tools=. install,'/tools'
home=. >(systype-5){(2!:5'HOME');2!:5'USERPROFILE'
home=. >(0-:home){home;,'/'
home=. ifios{::home;home,'/Documents/j'
home=. (home-:,'/'){::home;install
1!:44^:(ifios+.ifwasm) install
isroot=. (0=#1!:0'/data') *. ('root'-:2!:5'USER') +. (<home) e. '/var/root';'/root';'';,'/'
userx=. omitversion{::'/j9.7-user';'/j-user'
user=. home,userx
user=. >isroot{user;install,'/user'
home=. >isroot{home;install
break=. user,'/break'
config=. user,'/config'
snap=. user,'/snap'
temp=. user,'/temp'
temp=. >isroot{temp;(*#1!:0'/tmp'){::'/tmp';~(0-:2!:5'TMPDIR'){::(2!:5'TMPDIR');temp
ids=. ;:'addons bin break config home install snap system tools temp user'

0!:0^:(1:@(1!:4) ::0:) <(({.~ i:&'/') jpathsep >{.4!:3''),'/profilex.ijs' NB. override

SystemFolders_j_=: ids,.jpathsep@".&.>ids

md=. 3 : 0 NB. recursive makedir
a=. jpathsep y,'/'
if. ('root'-:2!:5'USER') +. ('//'-:2{.a)+.('/root/'-:6{.a)+.('/var/root/'-:10{.a)+.('/usr/'-:5{.a)+.('/tmp'-:a) do. return. end. NB. installed under / /root /usr
if. -.#1!:0 }:a do.
  for_n. I. a='/' do. 1!:5 :: [ <n{.a end.
end.
)

NB. try to ensure user folders exist
md user,'/projects'
md break
md config
md snap
md temp

NB. boot up J and load startup.ijs if it exists
4!:55 (<'system') -.~ (;:'systype fhs isroot userx ids md omitversion'), ids
0!:0 <jpathsep system_:,'/util/boot.ijs'
