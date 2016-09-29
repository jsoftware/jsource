NB. configure

cocurrent 'jcfg'
coinsert 'j'

jdefs=: 0 : 0
Browser
Browser_nox
DirTreeX
Format
MemoryLimit
EPSReader
ImageViewer
ImageViewer_nox
PDFReader
PDFReader_nox
ReadCatalog
RecentMax
XDiff
Tags
UserDict
Editor
Editor_nox
RGBSEQ
)

cbname=: {.~ i.&' ' <. i.&'='
foldernub=: #~ [: ~:&.|. {."1

NB. =========================================================
cpath=: 3 : 0
if. -. '~' e. {.&> {:"1 y do. y return. end.
({."1 y),.jpath each {:"1 y
)

NB. =========================================================
cbread=: 3 : 0
dat=. 'b' freads y
if. dat -: _1 do. return. end.
dat=. dat #~ (<'NB.') ~: 3 {. each dat
dat=. dat #~ '#' ~: {.&> dat
dat=. deb each dat
dat #~ 0 < #&> dat
)

NB. =========================================================
cbread1=: 3 : 0
r=. cbread jpath '~config/',y
if. r -: _1 do. cbread jpath '~system/config/',y end.
)

NB. =========================================================
cbread2=: 3 : 0
r0=. cbread jpath '~system/config/',y
if. r0 -: _1 do. r0=. <'' end.   NB. avoid domain error
r1=. cbread jpath '~config/',y
if. (0=#r1) +. r1 -: _1 do. r0 return. end.
n0=. cbname each r0
n1=. cbname each r1
if. 0 = #n0 -. n1 do. r1 return. end.
new=. (-.n0 e. n1) # r0
r=. LF2,'NB. ',(50#'='),LF,'NB. new configs',LF,;new,each LF
r fappends jpath '~config/',y
r1,new
)

NB. =========================================================
configbase=: 3 : 0
Snapshots_j_=: 0
0!:100 ;LF ,each cbread2 'base.cfg'
9!:3 DisplayForm
9!:7 BoxForm { Boxes
9!:17 BoxPos
if. MemoryLimit<1000 do.
  9!:21[2^MemoryLimit
elseif. MemoryLimit<_ do.
  9!:21 MemoryLimit
end.
9!:37 Output
0!:100 ;(}:,'_j_=:',]) each <;.2 jdefs
EMPTY
)

NB. =========================================================
configfolders=: 3 : 0
UserFolders_j_=: i.0 2
BINPATH_z_=: filecase BINPATH_z_
sf=. filecase each {:"1 SystemFolders_j_
SystemFolders_j_=: ({."1 SystemFolders_j_),.sf
dat=. cbread1 'folders.cfg'
if. (0=#dat) +. dat -: _1 do. return. end.
ndx=. dat i.&> ' '
ids=. ndx {.each dat
pts=. jpath each filecase each (ndx+1) }. each dat
uf=. ids,.pts
msk=. (=tolower) {.&>ids
SystemFolders_j_=: foldernub SystemFolders,msk#uf
UserFolders_j_=: foldernub (-.msk)#uf
SystemFolders_j_=: cpath SystemFolders_j_
UserFolders_j_=: cpath UserFolders_j_
EMPTY
)

NB. =========================================================
configrecent=: 3 : 0
Folder=: RecentDirmatch=: RecentFif=: RecentFiles=: RecentProjects=: ''
0!:0 ::] < jpath '~config/recent.dat'
Folder_j_=: Folder #~ (<Folder) e. {."1 UserFolders
RecentDirmatch_j_=: ~. fexists cutLF RecentDirmatch
RecentFif_j_=: ~. cutLF RecentFif
RecentFiles_j_=: ~. fexists cutLF RecentFiles
RecentProjects_j_=: ~. fexists cutLF RecentProjects
RecentProjects_j_=: RecentProjects_j_ #~ '~' = {.&> tofoldername each RecentProjects_j_
EMPTY
)

NB. =========================================================
configrun=: 3 : 0
configbase''
configfolders''
configrecent''
coerase <'jcfg'
18!:4<'z'
)

configrun$0
