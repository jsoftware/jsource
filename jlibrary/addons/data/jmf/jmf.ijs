cocurrent 'jmf'

jsystemdefs 'hostdefs'

coinsert 'jdefs'


doc=: 0 : 0

 map name;filename [;sharename;readonly]
       - map jmf file (self-describing)

 opt map name;filename [;sharename;readonly]
       - map data file (opt is description)

     where:  opt=type [;trailing_shape]

        types are defined in dll.ijs as:
            JB01      boolean
            JCHAR     character
            JCHAR2    unicode
            JCHAR4    unicode4
            JINT      integer
            JFL       floating point
            JCMPX     complex
            JSB       symbol

         trailing_shape= }. shape    (default '')

 [force] unmap name
      0 ok
      1 not mapped
      2 refs

  unmapall''                  - unmap all

  createjmf filename;msize    - creates jmf file as empty vector
                                (self-describing)

  additem name                - add an item to a name

  share name;sharedname       - share 'sharedname' as name

  showle name                 - show locale entry and header for name

  showmap''                   - show all maps

)
SZI=: IF64{4 8
'HADK HADFLAG HADM HADT HADC HADN HADR HADS'=: SZI*i.8
HADCN=: <.HADC%SZI

HSN=: 7+64
HS=: SZI*HSN
AFRO=: 1
AFNJA=: 2
NULLPTR=: <0
3 : 0''
if. IFUNIX do.
  lib=. ' ',~ unxlib 'c'
  api=. 1 : ('(''',lib,''',x) & cd')
  c_open=: 'open i *c i i' api
  c_close=: 'close i i' api
  c_read=: 'read x i * x' api
  c_write=: 'write x i * x' api
  c_lseek=: 'lseek x i x i' api
  c_mmap=: 'mmap * * x i i i x' api
  c_munmap=: 'munmap i * x' api
else.
  CREATE_ALWAYS=: 2
  CREATE_NEW=: 1
  FALSE=: 0
  FILE_BEGIN=: 0
  FILE_END=: 2
  FILE_MAP_READ=: 4
  FILE_MAP_WRITE=: 2
  FILE_SHARE_READ=: 1
  FILE_SHARE_WRITE=: 2
  GENERIC_READ=: _2147483648
  GENERIC_WRITE=: 1073741824
  OPEN_ALWAYS=: 4
  OPEN_EXISTING=: 3
  PAGE_READONLY=: 2
  PAGE_READWRITE=: 4
  TRUNCATE_EXISTING=: 5

  j=. (GENERIC_READ+GENERIC_WRITE),PAGE_READWRITE,FILE_MAP_WRITE
  RW=: j,:GENERIC_READ,PAGE_READONLY,FILE_MAP_READ

  CloseHandleR=: 'kernel32 CloseHandle > i x'&(15!:0)
  CreateFileMappingR=: 'kernel32 CreateFileMappingW > x x * i i i *w'&(15!:0)
  CreateFileR=: 'kernel32 CreateFileW > x *w i i * i i x'&(15!:0)
  GetLastError=: 'kernel32 GetLastError > i'&(15!:0)
  FlushViewOfFileR=: 'kernel32 FlushViewOfFile > i * x'&(15!:0)
  MapViewOfFileR=: >@{.@('kernel32 MapViewOfFile * x i i i x'&(15!:0))
  OpenFileMappingR=: 'kernel32 OpenFileMappingW > x i i *w'&(15!:0)
  SetEndOfFile=: 'kernel32 SetEndOfFile > i x'&(15!:0)
  UnmapViewOfFileR=: 'kernel32 UnmapViewOfFile > i *'&(15!:0)
  WriteFile=: 'kernel32 WriteFile i x * i *i *'&(15!:0)
  if. IF64 do.
    GetFileSizeR=: 2 >@{ 'kernel32 GetFileSizeEx i x *x' 15!:0 ;&(,2)
    SetFilePointerR=: 'kernel32 SetFilePointerEx > i x x *x i'&(15!:0)
  else.
    GetFileSizeR=: 'kernel32 GetFileSize > i x *i' 15!:0 ;&(<NULLPTR)
    SetFilePointerR=: 'kernel32 SetFilePointer > i x i *i i'&(15!:0)
  end.
end.

if. _1 = 4!:0<'mappings' do.
  mappings=: i.0 8
end.
empty''
)
symget=: 15!:6
symset=: 15!:7
allochdr=: 3 : 'r[2 memw (r=.15!:8 y),HADC,1,JINT'

freehdr=: 15!:9
msize=: 3 : 'memr y,HADM,1,JINT'
refcount=: 3 : 'memr y,HADC,1,JINT'

nountype =: 17 b.&16b1fffff
MAXINTU=: 2 ^ IF64{32 64x
MAXINTS=: <: 2 ^ IF64{31 63x
ufs=: + MAXINTU * 0 > ]
sfu=: _1 x: ] - MAXINTU * MAXINTS < ]
findkey=: 4 : 'I. (<x) = {."1 y'
free=: 3 : 0
'fh mh fad'=. y
if. IFUNIX do.
  if. fad do. c_munmap (<fad);mh end.
  if. fh~:_1 do. c_close fh end.
else.
  if. fad do. UnmapViewOfFileR <<fad end.
  if. mh do. CloseHandleR mh end.
  if. fh~:_1 do. CloseHandleR fh end.
end.
)
fullname=: 3 : 0
t=. y-.' '
t,('_'~:{:t)#'_base_'
)
mbxcheck=: 3 : 0
x=. 15!:12 y
b=. 0={:"1 x
'a s c'=. |: (-.b)#x
'u n z'=. ,b#x
z=. *./ c e. 1 2
z=. z, (-:<.) 2^.s
z=. z, (}.a)-:}:a+s
z=. z, u = {.a
z=. z, ({:a+s) <: u+n
z=. z, (-: <.) 64 %~ +/s
z=. z, (+/s) = <.&.(%&64) n
z=. z, *./ 0 = 8|a
)
settypeshape=: 3 : 0
'name type shape'=: y
type =: nountype type
rank=. #shape
sad=. symget <fullname name
'bad name' assert sad
had=. 1{s=. memr sad,0 4,JINT
'flag msize'=. memr had,HADFLAG,2,JINT
'not mapped and writeable' assert 2=flag
size=. (JTYPES i.type){JSIZES
ts=. size**/shape
'msize too small' assert ts<:msize
type memw had,HADT,1,JINT
((*/shape),rank,shape) memw had,HADN,(2+rank),JINT
i.0 0
)
validate=: 3 : 0
'ts had'=. y
if. ts>:HS do.
  d=. memr had,0 4,JINT
  *./((HS,ts-HS)=0 2{d),1 2 4 8 16 32 131072 262144 65536 e.~ nountype 3{d
else. 0 end.
)
ERROR_NOT_ENOUGH_MEMORY=: 8
j=. <;._2 (0 : 0)
1  ERROR_INVALID_FUNCTION
2  ERROR_FILE_NOT_FOUND
3  ERROR_PATH_NOT_FOUND
4  ERROR_TOO_MANY_OPEN_FILES
5  ERROR_ACCESS_DENIED
6  ERROR_INVALID_HANDLE
7  ERROR_ARENA_TRASHED
8  ERROR_NOT_ENOUGH_MEMORY
9  ERROR_INVALID_BLOCK
10 ERROR_BAD_ENVIRONMENT
11 ERROR_BAD_FORMAT
12 ERROR_INVALID_ACCESS
13 ERROR_INVALID_DATA
14 ERROR_OUTOFMEMORY
15 ERROR_INVALID_DRIVE
16 ERROR_CURRENT_DIRECTORY
17 ERROR_NOT_SAME_DEVICE
18 ERROR_NO_MORE_FILES
19 ERROR_WRITE_PROTECT
20 ERROR_BAD_UNIT
21 ERROR_NOT_READY
22 ERROR_BAD_COMMAND
23 ERROR_CRC
24 ERROR_BAD_LENGTH
25 ERROR_SEEK
26 ERROR_NOT_DOS_DISK
27 ERROR_SECTOR_NOT_FOUND
28 ERROR_OUT_OF_PAPER
29 ERROR_WRITE_FAULT
30 ERROR_READ_FAULT
31 ERROR_GEN_FAILURE
32 ERROR_SHARING_VIOLATION
33 ERROR_LOCK_VIOLATION
34 ERROR_WRONG_DISK
)

WINERRNOS=: 0 ". 2 {.&> j
WINERRMSG=: 3 }.each j
additem=: 3 : 0
sad=. symget <fullname y
'bad name' assert sad
had=. 1{s=. memr sad,0 4,JINT
'flag msize type rank'=. 1 2 3 6{memr had,0 28,JINT
type =. nountype type 
'not mapped and writeable' assert 2=flag
'scalar' assert 0~:rank
'not supported for boxed data' assert 32~:type
shape=. memr had,HADS,rank,JINT
shape=. shape+1,0#~rank-1
size=. (JTYPES i.type){JSIZES
ts=. size**/shape
'msize too small' assert ts<:msize
((*/shape),rank,shape) memw had,HADN,(2+rank),JINT
i.0 0
)
createjmf=: 3 : 0
'fn msize'=. y
msize=. <. msize
ts=. HS+msize
if. IFUNIX do.
  fh=. 0 pick c_open fn; (OR O_RDWR, O_CREAT, O_TRUNC); 8b666
  c_lseek fh;(<:ts);SEEK_SET
  c_write fh; (,0{a.); 0+1
  c_lseek fh;0 ;SEEK_SET
  d=. HS,AFNJA,msize,JINT,0,0,1,0
  c_write fh;d;(SZI*#d)
  c_close fh
else.
  fh=. CreateFileR (uucp fn,{.a.);(GENERIC_READ+GENERIC_WRITE);0;NULLPTR;CREATE_ALWAYS;0;0
  SetFilePointerR fh;ts;NULLPTR;FILE_BEGIN
  SetEndOfFile fh
  SetFilePointerR fh;0;NULLPTR;FILE_BEGIN
  d=. HS,AFNJA,msize,JINT,0,0,1,0
  WriteFile fh;d;(SZI*#d);(,0);<NULLPTR
  CloseHandleR fh
end.
i.0 0
)
share=: 3 : 0
'name sn ro'=. 3{.y,<0
sn=. '/' (('\'=sn)#i.#sn)} sn
if. IFUNIX do.
  map name;sn;sn;ro
else.
  name=. fullname name
  c=. #mappings
  assert c=({."1 mappings)i.<name['noun already mapped'
  4!:55 ::] <name
  'bad noun name'assert ('_'={:name)*._1=nc<name
  fh=. _1
  fn=. ''
  mh=. OpenFileMappingR (ro{FILE_MAP_WRITE,FILE_MAP_READ);0;uucp sn,{.a.
  if. mh=0 do. assert 0[CloseHandleR fh['bad mapping' end.
  fad=. MapViewOfFileR mh;FILE_MAP_WRITE;0;0;0
  if. fad=0 do. assert 0[CloseHandleR mh[CloseHandleR fh['bad view' end.
  had=. fad
  hs=: 0
  ts=. memr had,HADM,1,JINT
  mappings=: mappings,name;fn;sn;fh;mh;fad;had;ts
  (name)=: symset had
  i.0 0
end.
)
getflagsad=: 3 : 0
SZI+1{memr (symget <fullname y),0 4,JINT
)
readonly=: 3 : 0
AFRO(17 b.)memr (getflagsad y),0 1,JINT
:
flagsad=. getflagsad y
flags=. memr flagsad,0 1,JINT
flags=. flags(17 b.)(26 b.)AFRO
flags=. flags(23 b.)AFRO*0~:x
flags memw flagsad,0 1,JINT
i. 0 0
)
showle=: 3 : 0
le=. memr (symget <fullname y),0 4,JINT
had=. 1{le
h=. memr had,0 7,JINT
s=. memr had,HADS,(6{h),JINT
le;h;s
)
showmap=: 3 : 0
h=. 'name';'fn';'sn';'fh';'mh';'address';'header';'ts';'msize';'refs'
hads=. 6{"1 mappings
h,mappings,.(msize each hads),.refcount each hads
)
map=: 3 : 0
0 map y
:
if. 0=L.x do. t=. <&> x else. t=. x end.
'type tshape hsize'=. 3 {. t, a:
type =. nountype type

'trailing shape may not be zero' assert -. 0 e. tshape

'name fn sn ro'=. 4{.y,(#y)}.'';'';'';0
sn=. '/' (('\'=sn)#i.#sn)} sn
name=. fullname name
c=. #mappings

'name already mapped'assert c=({."1 mappings)i.<name
'filename already mapped'assert c=(1{"1 mappings)i.<fn
'sharename already mapped'assert (''-:sn)+.c=(2{"1 mappings)i.<sn
4!:55 ::] <name
'bad noun name'assert ('_'={:name)*._1=nc<name

ro=. 0~:ro
aa=. AFNJA+AFRO*ro

if. IFUNIX do.
  'Unix sharename must be same as filename' assert (sn-:'')+.sn-:fn
  ts=. 1!:4 <fn
  fh=. >0 { c_open fn;(ro{O_RDWR,O_RDONLY);0
  'bad file name/access' assert fh~:_1
  mh=. ts
  fad=. >0{ c_mmap (<0);ts;(OR ro}. PROT_WRITE, PROT_READ);MAP_SHARED;fh;0
  if. fad e. 0 _1 do.
    'bad view' assert 0[free fh,mh,0
  end.
else.
  'fa ma va'=. ro{RW
  fh=. CreateFileR (uucp fn,{.a.);fa;(FILE_SHARE_READ+FILE_SHARE_WRITE);NULLPTR;OPEN_EXISTING;0;0
  'bad file name/access'assert fh~:_1
  ts=. GetFileSizeR fh
  mh=: CreateFileMappingR fh;NULLPTR;ma;0;0;(0=#sn){(uucp sn,{.a.);<NULLPTR
  if. mh=0 do. 'bad mapping'assert 0[free fh,0,0 end.
  fad=. MapViewOfFileR mh;va;0;0;0
  if. fad=0 do.
    errno=. GetLastError''
    free fh,mh,0
    if. ERROR_NOT_ENOUGH_MEMORY-:errno do.
      'not enough memory' assert 0
    else.
      'bad view' assert 0
    end.
  end.
end.

if. ro*.0=type do.
  had=. allochdr 127
  d=. memr fad,0,HSN,JINT
  d=. (sfu HS+-/ufs fad,had),aa,2}.d
  d=. 1 HADCN} d
  d memw had,0,HSN,JINT
elseif. 0=type do.
  had=. fad
  if. 0=validate ts,had do. 'bad jmf header' assert 0[free fh,mh,fad end.
  aa memw had,HADFLAG,1,JINT
  if. sn-:'' do.
    t=. 0
  else.
    t=. 10000+ memr had,HADC,1,JINT
  end.
  (,t+1) memw had,HADC,1,JINT
elseif. 1 do.
  had=. allochdr 127
  bx=. JBOXED=type
  hs=. (+/hsize)*asize=. JSIZES {~ JTYPES i. type
  lshape=. bx}.<.(ts-hs)%(*/tshape)*asize
  d=. sfu hs+-/ufs fad,had
  h=. d,aa,ts,type,1,(*/lshape,tshape),((-.bx)+#tshape),lshape,tshape
  h memw had,0,(#h),JINT
end.

mappings=: mappings,name;fn;sn;fh;mh;fad;had
(name)=: symset had
i.0 0
)
unmap=: 3 : 0
0 unmap y
:
'y newsize'=. 2{.(boxopen y),<_1
n=. <fullname y
row=. ({."1 mappings)i.n
if. row=#mappings do. 1 return. end.
m=. row{mappings
4!:55 ::] n
'sn fh mh fad had'=. 5{.2}.m

if. *./(-.x),(0=#sn),1~:memr had,HADC,1,JINT do. 2 return. end.

jmf=. fad = had
if. -.jmf do. freehdr had end.
if. _1=newsize do.
  free fh,mh,fad
else.
  newsize=. <.newsize
  totsize=. newsize + jmf*HS
  free _1,mh,fad
  if. IFUNIX do.
    c_lseek fh;(<:totsize);SEEK_SET
    c_write fh;(,0{a.);0+1
    if. jmf do.
      c_lseek fh;(SZI*2);SEEK_SET
      c_write fh;(,newsize);SZI
    end.
    c_close fh
  else.
    SetFilePointerR fh;totsize;NULLPTR;FILE_BEGIN
    SetEndOfFile fh
    if. jmf do.
      SetFilePointerR fh;(SZI*2);NULLPTR;FILE_BEGIN
      WriteFile fh;(,newsize);SZI;(,0);<NULLPTR
    end.
    CloseHandleR fh
  end.
end.
mappings=: (row~:i.#mappings)#mappings
0
)
unmapall=: 3 : '>unmap each 0{"1 mappings'
memshare=: 3 : 0
bNo_Inherit_Handle=. FALSE
lpShareName=. y,{.a.
mh=. OpenFileMappingR (FILE_MAP_READ+FILE_MAP_WRITE); bNo_Inherit_Handle; uucp lpShareName
('Unable to map ',y) assert mh~:0

addr=. MapViewOfFileR mh; (FILE_MAP_READ+FILE_MAP_WRITE); 0; 0; 0
if. addr=0 do. 'MapViewOfFile failed' assert 0[CloseHandleR mh end.
".(_1=4!:0<'mapTable')#'mapTable=:i.0,3'
mapTable=: mapTable, y; mh; addr
addr
)
memshareclose=: 3 : 0
r=. y findkey mapTable
'Unknown share name' assert 0~:$r
'mh addr'=. {:(<r; 1 2){mapTable
('Unable to close share: ', y) assert $mh > 0
UnmapViewOfFileR <<addr
if. CloseHandleR mh do.
  mapTable=: (<((i.#mapTable)-.r); i.{:$mapTable){mapTable
end.
)
