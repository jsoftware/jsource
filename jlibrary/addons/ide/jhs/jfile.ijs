NB. J HTTP Server - jfile app
coclass'jfile'
coinsert'jhs'

HBS=: 0 : 0
jhma''
jhjmlink''
'action'    jhmg'action';1;12
 'edit'     jhmab'edit'
 'del'      jhmab'delete'
 'deltemps' jhmab'delete temps'
 'copy'     jhmab'copy        c^'
 'cut'      jhmab'cut         x^'
 'paste'    jhmab'paste       v^'
 'rename'   jhmab'rename as...'
 'newfi'    jhmab'new file'
 'newfo'    jhmab'new folder'
jhmz''

'renamedlg'     jhdivadlg''
 'renamedo'     jhb'rename as'
  'renamex'     jhtext'';10
   'renameclose'jhb'X'
'<hr></div>'

'deletedlg'    jhdivadlg''
 'deletedo'    jhb'delete'
 'deletename'  jhspan''
 'deleteclose' jhb'X'
'<hr></div>'

'report'    jhdiv'<R>'
shorts''
jhhr
'path'      jhhidden'<F>'
 'pathd'    jhdiv'<F>'

jhresize''

'sel'       jhdiv'<FILES>'
)

LASTPATH=: jpath'~temp\'


NB. y - error;file
create=: 3 : 0
'r f'=. y
if. 0=#1!:0<(-PS={:f)}.f do. f=. jpath'~temp\' end. NB. ensure valid f
'jfile' jhr 'R F FILES';r;(jshortname f);(buttons 'files';(2#<folderinfo remlev f),<'<br>') 
)

NB. get file with mid/path opens the file
jev_get=: 3 : 0
if. 'open'-:getv'mid' do.
 ev_edit_click''
else.
 create '&nbsp;';LASTPATH
end.
)

buttons=: 3 : 0
'mid sids values sep'=. y
p=. ''
for_i. i.#sids do.
 id=. mid,'*',>i{sids
 p=. p,sep,~id jhab i{values
end.
)

shorts=: 3 : 0
buttons 'paths';(2#<((roots''),~{."1 UserFolders_j_,SystemFolders_j_)-.<'Demos'),<' '
)

ev_paths_click=: 3 : 0
f=. getv'jsid'
if. ':'={:f do.
 f=. f,'/'
elseif. -.f-:,'/' do.
 f=. '~',f,'/'
end. 
LASTPATH=: f=. jpath f
jhrajax (jshortname f),JASEP,buttons 'files';(2#<folderinfo remlev f),<'<br>'
)

ev_recall_click=: 3 : 0
sid=. getv'jsid'
f=. jpath sid
LASTPATH=: f
jhrajax (jshortname f),JASEP,buttons 'files';(2#<folderinfo remlev f),<'<br>'
)

NB. folder clicked (file click handled in js)
ev_files_click=: 3 : 0
sid=. getv'jsid'
path=. jpath getv'path'
sid=. sid-.PS
sid=. 6}.sid NB. drop markfolder prefix
if. sid-:'..' do.
 f=. PS,~remlev remlev path 
else.
 f=. (remlev path),PS,sid,PS
end.
LASTPATH=: f
jhrajax (jshortname f),JASEP,buttons 'files';(2#<folderinfo remlev f),<'<br>'
)

nsort=: 3 : '/:~y'

markfolder=: <'&nbsp;&nbsp;&nbsp;'

NB. y path - result is folders,files
folderinfo=: 3 : 0
a=. 1!:0 <jpath y,'/*'
n=. {."1 a
d=. 'd'=;4{each 4{"1 a
(markfolder,each'/',~each(<'..'),nsort d#n),nsort (-.d)#n
)

NFI=: 'newfile'
NFO=: 'newfolder'

ev_newfi_click=: 3 : 0
F=. jpath getv'path'
f=. (remlev F),PS,NFI
if. fexist f do.
 r=. NFI,' already exists'
else.
 try. 
  ''1!:2<f
  r=. NFI,' created'
 catch.
  r=. NFI,' create failed'
 end.
end.
create r;f
)

ev_newfo_click=: 3 : 0
F=. jpath getv'path'
f=. (remlev F),PS,NFO,PS
try.
 1!:5<f
 r=. NFO,' created'
catch.
 r=. NFO,' create failed'
end.
create r;f
)

ev_renamedo_click=: ev_renamex_enter

ev_renamex_enter=: 3 : 0
F=. jpath getv'path'
n=. getv'renamex'
if. PS-:{:F do.
 f=. (remlev remlev F),PS,n,PS
else.
 f=. (remlev F),PS,n
end.
if. f frename F do.
 create'Rename: ok';f
else.
 create'Rename: failed';F
end.
)

ev_deletedo_click=: 3 : 0
f=. jgetfile F=. jpath getv'path'
newf=. PS,~remlev F
t=. jpath'~temp/deleted/'
if. PS={:F do. NB. delete folder 
 srcfolder=. F
 snkfolder=. jpath'~temp/deleted/',jgetfile remlev srcfolder
 if. t-:(#t){.srcfolder do.
  deletefolder }:srcfolder
  create ('Delete: deleted ',jshortname srcfolder);newf
  return.
 end.
 deletefolder snkfolder
 1!:5<snkfolder
 copyfiles (srcfolder,'*');snkfolder
 deletefolder }:srcfolder
 create ('Delete: folder saved as ',jshortname snkfolder);newf 
 return.
else. NB. delete file
 try.
  if. t-:(#t){.F do.
   1!:55 <F
   create ('Delete: deleted ',jshortname F);newf
   return.
  end.
  d=. 1!:1<jpath F
  1!:5 :: [ <jpath'~temp/deleted'
  d 1!:2    <jpath'~temp/deleted/',jgetfile F
  1!:55 <F
  create ('Delete: file saved as ~temp/deleted/',f);newf
 catch.
  create ('Delete "',f,'" failed.');newf
 end.
end.
)

ev_deltemps_click=: 3 : 0
t=.{."1[1!:0 jpath'~temp/*ijs'
n=. (_4}.each t)-.each<'0123456789'
t=.(-.n~:<'')#t
t=. (<jpath'~temp/'),each t
for_f. t do. 1!:55 f end. 
create '&nbsp;';jpath'~temp\'
)

NB. folder dblclick??? not a problem, but is puzzling

NB. ev_files_dblclick=: ev_edit_click

ev_edit_click=: 3 : 0
f=. jgetfile F=. jpath getv'path'
if. f-:'' do.
 create'No file selected to edit.';F
else.
 require'~addons/ide/jhs/jijs.ijs' NB. ensure loaded
 create_jijs_ F
end.
)

copy=: _1 NB. _1 not ready, 0 copy, 1 cut 

copycut=: 3 : 0
copy=: y
srcfile=: jpath getv'path'
create 'Ready for paste';srcfile
)

ev_copy_click=: 3 : 'copycut 0'
ev_cut_click=:  3 : 'copycut 1'

ev_paste_click=: 3 : 0
F=. jpath getv'path'
f=. jgetfile srcfile
if. ''-:f do.
 folder=. jgetfile remlev srcfile
 srcfolder=. srcfile
 snkfolder=. F,jgetfile remlev srcfile
 if. srcfile-:(#srcfile){.snkfolder do.
  create 'Paste: destination can not be in source';F
  return.
 end.
 try.
  1!:5<snkfolder
 catch.
  create 'Paste: folder already exists';F
  return.
 end.
 copyfiles (srcfile,'*');snkfolder
 if. copy=1 do. deletefolder }:srcfile end.
 create ('Paste: created folder ',jshortname snkfolder);F  
 return.
end.
d=. fread srcfile
i=. f i: '.'
a=. i{.f
z=. i}.f
c=. 0
while. fexist snkfile=. (remlev F),PS,f do.
 c=. >:c
 f=. a,'-Copy(',(":c),')',z
end.
if. _1=copy              do. create 'Paste: no copy or cut';F          return. end.
if. _1-:d                do. create 'Paste: read source file failed';F return. end.
if. _1-:d fwrite snkfile do. create 'Paste: write newfile failed';F    return. end.
if. copy=1 do. try. 1!:55 <srcfile catch. end. end.
create ('Paste: created file ',f);F
)

NB. createfolder path - ensure path y exists
createfolder=: 3 : 0
for_n. (PS=t)#i.#t=. jpath y,'\'  do.
 1!:5 :: [ <n{.jpath y
end.
i.0 0
)

NB. copyfiles src;snk
NB. src ends with \fspec which can have wildcards
copyfiles=: 3 : 0
'src snk'=. y
src=. jpath src
snk=. jpath snk
i=. src i:PS
fspec=. (>:i)}.src
src=. i{.src
createfolder snk
ns=. 1!:0 <jpath src,'\',fspec
for_n. ns do.
 pn=. jpath'\',>{.n
 srcpn=.src,pn
 snkpn=.snk,pn
 if. 'd'~:4{>4{n do.
  t=. 1!:1<srcpn
  p=. 1!:7<srcpn
  t 1!:2<snkpn
  p 1!:7<snkpn NB. permissions
 else.
  if. -.srcpn-:snk do.
   1!:5 :: [ <snkpn
   copyfiles (srcpn,'\*');snkpn
  end.
 end.
end.
i.0 0
)

NB. deletefolder y
deletefolder=: 3 : 0
p=. <jpath y
if. 1=#1!:0 p do.
 if. 'd'=4{,>4{"1 (1!:0) p do.
  deleterecursive y
  1!:55 p
 end.
end.
i.0 0
)

NB. deletesub y
deleterecursive=: 3 : 0
assert. 3<+/PS=jpath y
ns=. 1!:0 <jpath y,'\*'
for_n. ns do.
 s=. jpath y,'\',>{.n
 if. 'd'=4{>4{n do.
  deleterecursive s
 end.
 1!:55<s
end.
)

NB. newname frename oldname - return 1 if rename ok
frename=: 4 : 0
if. x -: y do. return. end.
if. IFUNIX do.
  0=((unxlib 'c'),' rename > i *c *c') 15!:0 y;x
else.
  'kernel32 MoveFileA > i *c *c' 15!:0 y;x
end.
)

remlev=: 3 : '(y i: PS){.y'    NB. remove level from path

roots=: 3 : 0
if. IFWIN do.
 a=. 26}.Alpha_j_
 b=. (<"0 a),each <':/'
 d=. a#~0~:;#each dir each b
 (<"0 d),each <':'
else.
 <'/'
end.
)

CSS=: 0 : 0
#report{color:red}
#pathd{color:blue;}
*{font-family:<PC_FONTFIXED>;}
)

JS=: 0 : 0
var recall=1

function ev_body_load(){jresize();}

function repclr(){jbyid("report").innerHTML = "&nbsp;";}
function setpath(t){jform.path.value= t;jbyid("pathd").innerHTML= t;}
function ev_paths_click(){repclr();jdoajax(["path"]);}
function ev_paths_dblclick(){;}
function ev_recall_click(){repclr();jdoajax(["path"]);}

function document_recall(v){recall=0;jform.path.value= v;jbyid("pathd").innerHTML= v;jscdo("recall",v);}

function ev_x_shortcut(){jscdo("cut");}
function ev_c_shortcut(){jscdo("copy");}
function ev_v_shortcut(){jscdo("paste");}
function ev_2_shortcut(){jbyid("sel").childNodes[0].focus();}

function ev_files_click() // file select
{
 repclr();
 if('/'!=jform.jsid.value.charAt(jform.jsid.value.length-1))
 {
  var t= jform.path.value;
  var i= t.lastIndexOf('/');
  setpath(t.substring(0,++i)+jform.jsid.value);
 }
 else
 {
  jdoajax(["path"]);
 }
}

function ev_files_dblclick()
{
 if('/'!=jform.jsid.value.charAt(jform.jsid.value.length-1))
  window.open('jijs?mid=open&path='+jform.path.value,TARGET);
} 

function ev_rename_click()     {jdlgshow("renamedlg","renamex");}
function ev_renameclose_click(){jhide("renamedlg");}

function ev_del_click()
{
 jbyid("deletename").innerHTML=jform.path.value;
 jdlgshow("deletedlg","deleteclose");
}

function ev_deleteclose_click(){jhide("deletedlg");}

function ajax(ts)
{
 if(2!=ts.length) alert("wrong number of ajax results");
 setpath(ts[0]);
 if(recall)
 {
  adrecall("document",ts[0],"0");
 }
 recall=1;
 jbyid("sel").innerHTML= ts[1];
}

)

