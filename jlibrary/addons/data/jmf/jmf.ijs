cocurrent 'jmf'
jsystemdefs 'hostdefs'
coinsert 'jdefs'

doc=: 0 : 0
map name;filename [;sharename [;mt] ]
 map jmf file (self-describing)

(type [;tshape]) map name;filename [;sharename [;mt] ]
 map data file

 types from dll.ijs: JB01,JCHAR,JCHAR2,JCHAR4,JINT,JFL,JCMPX,JSB

 tshape - trailing shape - }.shape    (default '')

mt (map type):
 0 - MTRW - default read/write mapping 
 1 - MTRO - read-only mapping - map jmf file copies header to private area
 2 - MTCW - copy-on-write - private mapping - changes not reflected in file

[force] unmap name - result 0 ok, 1 not mapped, 2 refs prevent unmap

createjmf filename;msize  - create jmf file as empty vector (self-describing)
unmapall''                - unmap all
showmap''                 - map info with col headers and extras
mappings                  - map info
share name;sharename[;mt] - share 'sharename' as name

MAPNAME,MAPFN,... showmap col indexes 
)
memhad_z_=: [: {: [: memr 0 2 4 ,~ (15!:6)@<
memdad_z_=: 15!:14@<

0 : 0
807 made changes to the header that affect jmf J code
before 807 - HADR field bytes are (lilendian) rrrr (j32) rrrrrrrr (j64)
after  807 - HADR field bytes are (lilendian) rrhh (j32) rrhhxxxx (j64)
flipped for bigendian (fill,hh,rr)
hh field flags must not be touched by jmf
newheader is 1 if 807 header format
)
IFBE=: 'a'~:{.2 ic a.i.'a'
SZI=: IF64{4 8
'MAPNAME MAPFN MAPSN MAPFH MAPMH MAPADDRESS MAPHEADER MAPFSIZE MAPJMF MAPMT MAPMSIZE MAPREFS'=: i.12
'MTRW MTRO MTCW'=: i.3
'HADK HADFLAG HADM HADT HADC HADN HADR HADS'=: SZI*i.8
HADRUS=: HADR+IFBE*IF64{2 6
HADCN=: <.HADC%SZI
HSN=: 7+64
HS=: SZI*HSN
AFRO=: 1
AFNJA=: 2
NULLPTR=: <0
allochdr=: 3 : 'r[2 setHADC r=.15!:8 y'
freehdr=: 15!:9
msize=: gethadmsize=: 3 : 'memr y,HADM,1,JINT'
fullname=: 3 : 0
t=. y-.' '
t,('_'~:{:t)#'_base_'
)
newheader=: 0~:memr (memhad'SZI_jmf_'),HADR,1,JINT

setheader=: 4 : 0
if. newheader do.
 (6{.x) memw y,0,6,JINT
 (6{x)  setHADR y
 (7}.x) memw y,HADS,(#7}.x),JINT
else.
 x memw y,0,(#x),JINT
end.
)

getHADR=: 3 : 0
if. newheader do.
 _1 (3!:4) memr y,HADRUS,2,JCHAR
else.
 memr y,HADR,1,JINT
end.
)

setHADR=: 4 : 0
if. newheader do.
 (1 (3!:4) x) memw y,HADRUS,2,JCHAR
else.
 x memw y,HADR,1,JINT
end.
)

getHADC=: 3 : '  memr y,HADC,1,JINT'
setHADC=: 4 : 'x memw y,HADC,1,JINT'
refcount=: getHADC

3 : 0''
if. IFUNIX do.
  lib=. ' ',~ unxlib 'c'
  api=. 1 : ('(''',lib,''',m) & cd')
  c_open=: 'open i *c i i' api
  c_open_va=: 'open i *c i x x x x x x i' api
  c_close=: 'close i i' api
  c_read=: 'read x i * x' api
  c_write=: 'write x i * x' api
  c_lseek=: 'lseek x i x i' api
  c_ftruncate=: 'ftruncate i i x' api
  c_mmap=: 'mmap * * x i i i x' api
  c_munmap=: 'munmap i * x' api

  t=.           O_RDWR,   (PROT_WRITE+PROT_READ),  MAP_SHARED
  t=. t,:       O_RDONLY, PROT_READ,               MAP_SHARED
  mtflags=:  t, O_RDWR,   (PROT_WRITE+PROT_READ),  MAP_PRIVATE
else.
  CREATE_ALWAYS=: 2
  CREATE_NEW=: 1
  FALSE=: 0
  FILE_BEGIN=: 0
  FILE_END=: 2
  FILE_MAP_COPY=: 1
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

  t=.           (GENERIC_READ+GENERIC_WRITE), PAGE_READWRITE,  FILE_MAP_WRITE
  t=.       t,: GENERIC_READ,                 PAGE_READONLY,   FILE_MAP_READ
  mtflags=: t,  (GENERIC_READ+GENERIC_WRITE), PAGE_READWRITE,  FILE_MAP_COPY

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
  mappings=: i.0 10
end.
empty''
)

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
had=. memhad name
'flag msize'=. memr had,HADFLAG,2,JINT
'not mapped and writeable' assert 2=3 (17 b.) flag
size=. (JTYPES i.type){JSIZES
ts=. size**/shape
'msize too small' assert ts<:msize
type memw had,HADT,1,JINT
(*/shape) memw had,HADN,1,JINT
rank setHADR had
shape memw had,HADS,(#shape),JINT
i.0 0
)
validate=: 3 : 0
'ts had'=. y
if. ts>:HS do.
  d=. memr had,0 4,JINT
  *./((HS,ts-HS)=0 2{d),1 2 4 8 16 32 131072 262144 65536 e.~ nountype 3{d
else. 0 end.
)
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
createjmf=: 3 : 0
'fn msize'=. y
fn=. jpath fn
msize=. <. msize
ts=. HS+msize
if. IFUNIX do.
  if. ('Darwin'-:UNAME) *. 'arm64'-:3 :'try.9!:56''cpu''catch.''''end.' '' do.
  fh=. 0 pick c_open_va fn; (OR O_RDWR, O_CREAT, O_TRUNC); (6#<00) ,< 8b666
  else.
  fh=. 0 pick c_open fn; (OR O_RDWR, O_CREAT, O_TRUNC); 8b666
  end.
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
readonly=: 3 : 0
AFRO(17 b.) a.i.memr (HADFLAG+memhad fullname y),0 1,JCHAR
:
ad=. HADFLAG+memhad fullname y
flags=. a.i.memr ad,0 1,JCHAR
flags=. flags(17 b.)(26 b.)AFRO
flags=. flags(23 b.)AFRO*0~:x
(flags{a.) memw ad,0 1,JCHAR
i. 0 0
)
showmap=: 3 : 0
h=. 'name';'fn';'sn';'fh';'mh';'address';'header';'fsize';'jmf';'mt';'msize';'refs'
hads=. 6{"1 mappings
h,mappings,.(gethadmsize each hads),.refcount each hads
)
mapsub=: 3 : 0
'name fn sn ro'=. y
ts=. 1!:4 <fn
if. IFUNIX do.
  'Unix sharename must be same as filename' assert (sn-:'')+.sn-:fn
  'FO FMP FMM'=. ro{mtflags
  if. ('Darwin'-:UNAME) *. 'arm64'-:3 :'try.9!:56''cpu''catch.''''end.' '' do.
  fh=. >0 { c_open_va fn;FO;(6#<00),<0
  else.
  fh=. >0 { c_open fn;FO;0
  end.
  'bad file name/access' assert fh~:_1
  mh=. ts
  fad=. >0{ c_mmap (<0);ts;FMP;FMM;fh;0
  if. fad e. 0 _1 do. 'bad view' assert 0[free fh,mh,0 end.
else.
  'Win sharename must not have /' assert -.'/'e.sn
  'fa ma va'=. ro{mtflags



  fh=. CreateFileR (uucp fn,{.a.);fa;(OR FILE_SHARE_WRITE, FILE_SHARE_READ);NULLPTR;OPEN_EXISTING;0;0
  if. fh=_1 do.
   6!:3[2
   fh=. CreateFileR (uucp fn,{.a.);fa;(OR FILE_SHARE_WRITE, FILE_SHARE_READ);NULLPTR;OPEN_EXISTING;0;0
   'bad file name/access'assert fh~:_1
  end. 
  mh=: CreateFileMappingR fh;NULLPTR;ma;0;0;(0=#sn){(uucp sn,{.a.);<NULLPTR
  if. mh=0 do. 'bad mapping'assert 0[free fh,0,0 end.
  fad=. MapViewOfFileR mh;va;0;0;0
  if. fad=0 do.
    errno=. GetLastError''
    free fh,mh,0
    0 assert~;(8=errno){'bad view';'not enough memory'
  end.
end.
name;fn;sn;fh;mh;fad;0;ts;0;ro
)
map=: 3 : 0
0 map y
:
if. 0=L.x do. t=. <&> x else. t=. x end.
'type tshape hsize'=. 3 {. t, a:
type =. nountype type

'trailing shape may not be zero' assert -. 0 e. tshape

'name fn sn ro'=. 4{.y,(#y)}.'';'';'';0
fn=. jpath fn
sn=. '/' (('\'=sn)#i.#sn)} sn
name=. fullname name
c=. #mappings

'maptype must be 0 (MTRW), 1 (MTRO), or 2 (MTCW - copy on write)' assert ro e. 0 1 2
'name already mapped'assert c=({."1 mappings)i.<name
'filename already mapped'assert c=(1{"1 mappings)i.<fn
'sharename already mapped'assert (''-:sn)+.c=(2{"1 mappings)i.<sn
4!:55 ::] <name
'bad noun name'assert ('_'={:name)*._1=nc<name
aa=. AFNJA+AFRO*ro=1

m=. mapsub name;fn;sn;ro
'fh mh fad had ts'=. (MAPFH,MAPMH,MAPADDRESS,MAPHEADER,MAPFSIZE){m

if. ro*.0=type do.
  had=. allochdr 63
  d=. memr fad,0,HSN,JINT
  d=. (sfu HS+-/ufs fad,had),aa,2}.d
  d=. 1 HADCN} d
  d setheader had
elseif. 0=type do.
  had=. fad
  if. 0=validate ts,had do. 'bad jmf header' assert 0[free fh,mh,fad end.
  aa memw had,HADFLAG,1,JINT
  if. sn-:'' do.
    t=. 0
  else.
    t=. 10000+ getHADC had
  end.
  (,t+1) setHADC had
elseif. 1 do.
  had=. allochdr 63
  'JBOXED (non-jmf) not supported' assert JBOXED~:type
  bx=. JBOXED=type
  hs=. +/hsize [ asize=. JSIZES {~ JTYPES i. type
  lshape=. bx}.<.(ts-hs)%(*/tshape)*asize
  d=. sfu hs+-/ufs fad,had
  h=. d,aa,ts,type,1,(*/lshape,tshape),((-.bx)+#tshape),lshape,tshape
  h setheader had
end.

m=. (had;0=type) (MAPHEADER,MAPJMF)}m
mappings=: mappings,m
(name)=: 15!:7 had
i.0 0
)
remap=: 3 : 0
name=. fullname y
row=. ({."1 mappings)i.<name
'remap: not mapped' assert row<#mappings
m=. row{mappings
fn=. ;1{m
ro=.  >MAPMT{m
jmf=. >MAPJMF{m
hs=. HS*jmf
'sn fh mh fad had'=. 5{.2}.m
free fh,mh,fad
m=. mapsub name;fn;sn;ro
m=. (had;jmf) (MAPHEADER,MAPJMF)}m
mappings=: m row}mappings
fad=. >MAPADDRESS{m
d=. sfu hs+-/ufs fad,had
d memw had,HADK,1,JINT
((>MAPFSIZE{m)-hs) memw had,HADM,1,JINT
i.0 0
)
share=: 3 : 0
'name sn ro'=. 3{.y,<0
map name;sn;sn;ro
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
'sn fh mh fad had jmf ts'=. (MAPSN,MAPFH,MAPMH,MAPADDRESS,MAPHEADER,MAPJMF,,MAPFSIZE){m

if. *./(-.x),(0=#sn),1~:getHADC had do. 2 return. end.
if. -.jmf do. freehdr had end.
if. _1=newsize do.
  free fh,mh,fad
else.
  newsize=. <.newsize
  totsize=. newsize + jmf*HS
  free _1,mh,fad
  if. IFUNIX do.
    if. totsize>ts do.
      c_lseek fh;(<:totsize);SEEK_SET
      c_write fh;(,0{a.);0+1
    elseif. totsize<ts do.
      c_ftruncate fh;totsize
    end.
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
'y ro'=. 2{.(boxopen y),<0
lpShareName=. y,{.a.
mh=. OpenFileMappingR (ro{FILE_MAP_WRITE,FILE_MAP_READ); bNo_Inherit_Handle; uucp lpShareName
('Unable to map ',y) assert mh~:0

addr=. MapViewOfFileR mh; (ro{FILE_MAP_WRITE,FILE_MAP_READ); 0; 0; 0
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
