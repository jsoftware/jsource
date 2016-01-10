coclass <'jsocket'
coinsert 'jsocket jdefs'

jsystemdefs 'hostdefs'
jsystemdefs 'netdefs'
3 : 0''
assert. INADDR_ANY=0
assert. sockaddr_sz=16
if. IFUNIX do.
  assert. fds_bits_off=0
end.
)
3 : 0''
select. UNAME
case. 'Win' do.
  c=. >IFWINCE{'wsock32';'winsock'
  ccdm=: 1 : ('(''"',c,,'" '',x)&(15!:0)')
  ncdm=: ccdm
  scdm=: ccdm
  wcdm=: ccdm
  LIB=: ''
  closesocketJ=: 'closesocket i i' scdm
  ioctlsocketJ=: 'ioctlsocket i i i *i' scdm
case. do.
  c=. unxlib 'c'
  ccdm=: 1 : ('(''"',c,'" '',x)&(15!:0)')
  ncdm=: ccdm
  scdm=: ccdm
  wcdm=: 1 : ']'
  LIB=: c
  closesocketJ=: 'close i i' scdm
  ioctlsocketJ=: 'ioctl i i i *i' scdm
end.
empty''
)
gethostbyaddrJ=: 'gethostbyaddr * * i i' ncdm
gethostbynameJ=: 'gethostbyname * *c' ncdm
gethostnameJ=: 'gethostname i *c i' ncdm
inet_addrJ=: 'inet_addr i *c' ncdm
inet_ntoaJ=: 'inet_ntoa i i' ncdm
acceptJ=: 'accept i i * *i' scdm
acceptNullJ=: 'acceptNull i i *c *c' scdm
bindJ=: 'bind i i * i' scdm
connectJ=: 'connect i i * i' scdm
FD_ISSETJ=: 'FD_ISSET i i ' scdm
getpeernameJ=: 'getpeername i i * *i' scdm
getprotobynameJ=: 'getprotobyname i *c' scdm
getprotobynumberJ=: 'getprotobynumber i i' scdm
getservbynameJ=: 'getservbyname i i i' scdm
getservbyportJ=: 'getservbyport i i i' scdm
getsocknameJ=: 'getsockname i i * *i' scdm
getsockoptJ=: 'getsockopt i i i i *i *i' scdm
htonlJ=: 'htonl i i' scdm
htonsJ=: 'htons s s' scdm
listenJ=: 'listen i i i' scdm
ntohlJ=: 'ntohl i i' scdm
ntohsJ=: 'ntohs s s' scdm
recvJ=: 'recv i i * i i' scdm
recvfromJ=: 'recvfrom i i *c i i * *i' scdm
selectJ=: 'select i i * * * *' ccdm
sendJ=: 'send i i *c i i' scdm
sendtoJ=: 'sendto i i *c i i * i' scdm
setsockoptJ=: 'setsockopt i i i i *i i' scdm
shutdownJ=: 'shutdown i i i' scdm
socketJ=: 'socket i i i i' scdm

WSAAsyncGetHostByAddrJ=: 'WSAAsyncGetHostByAddr i i i i i i i i' wcdm
WSAAsyncGetHostByNameJ=: 'WSAAsyncGetHostByName i i i *c i i' wcdm
WSAAsyncGetProtoByNameJ=: 'WSAAsyncGetProtoByName i i i *c i i' wcdm
WSAAsyncGetProtoByNumberJ=: 'WSAAsyncGetProtoByNumber i i i i i i' wcdm
WSAAsyncGetServByNameJ=: 'WSAAsyncGetServByName i i i *c *c i i' wcdm
WSAAsyncGetServByPortJ=: 'WSAAsyncGetServByPort i i i i *c i i' wcdm
WSAAsyncSelectJ=: 'WSAAsyncSelect i i i i i' wcdm
WSACancelAsyncRequestJ=: 'WSACancelAsyncRequest i i' wcdm
WSACancelBlockingCallJ=: 'WSACancelBlockingCall i ' wcdm
WSACleanupJ=: 'WSACleanup i ' wcdm
WSAGetLastErrorJ=: 'WSAGetLastError i ' wcdm
WSAIsBlockingJ=: 'WSAIsBlocking i ' wcdm
WSASetBlockingHookJ=: 'WSASetBlockingHook i i' wcdm
WSASetLastErrorJ=: 'WSASetLastError i i' wcdm
WSAStartupJ=: 'WSAStartup i i *' wcdm
WSAStringToAddressJ=: 'WSAStringToAddress i c i i i i' wcdm
WSAUnhookBlockingHookJ=: 'WSAUnhookBlockingHook i ' wcdm
x_WSAFDIsSetJ=: 'x_WSAFDIsSet i i i' wcdm

sderror=: 3 : 0
'num msg'=. SDERRORS
> (num i. >{.y) { msg, <'unknown error'
)
j=. <;._2 (0 : 0)
    0 no error
10004 EINTR
10009 EBADF
10011 EAGAIN
10013 EACCES
10014 EFAULT
10022 EINVAL
10024 EMFILE
10035 EWOULDBLOCK
10036 EINPROGRESS
10037 EALREADY
10038 ENOTSOCK
10039 EDESTADDRREQ
10040 EMSGSIZE
10041 EPROTOTYPE
10042 ENOPROTOOPT
10043 EPROTONOSUPPORT
10044 ESOCKTNOSUPPORT
10045 EOPNOTSUPP
10046 EPFNOSUPPORT
10047 EAFNOSUPPORT
10048 EADDRINUSE
10049 EADDRNOTAVAIL
10050 ENETDOWN
10051 ENETUNREACH
10052 ENETRESET
10053 ECONNABORTED
10054 ECONNRESET
10055 ENOBUFS
10056 EISCONN
10057 ENOTCONN
10058 ESHUTDOWN
10059 ETOOMANYREFS
10060 ETIMEDOUT
10061 ECONNREFUSED
10062 ELOOP
10063 ENAMETOOLONG
10064 EHOSTDOWN
10065 EHOSTUNREACH
10066 ENOTEMPTY
10067 EPROCLIM
10068 EUSERS
10069 EDQUOT
10070 ESTALE
10071 EREMOTE
10091 SYSNOTREADY
10092 VERNOTSUPPORTED
10093 NOTINITIALISED
10098 EADDRINUSE
10101 EDISCON
11001 HOST_NOT_FOUND
11002 TRY_AGAIN
11003 NO_RECOVERY
11004 NO_DATA
)

SDERRORS=: (0 ". 5 {. &> j) ; < 6 }.each j
SDERRORS=: ((10000*IFUNIX) | >{.SDERRORS);{:SDERRORS

tostring=: 3 : 0
}: ;'.',~each ":each a.i.y
)

data2string=: 3 : 0
tostring 4{.4}.2{::y
)

namesub=: 3 : 0
if. 0~:res y do. (sdsockerror'');0;'';0 return. end.
0;AF_INET;(data2string y);256#.a.i.2 3{2{::y
)

flip=: 'a'={.2 ic a.i.'a'
bigendian=: |.^:flip
hns=: 3 : 'a.{~256 256#:y'
hs=: 3 : 'bigendian a.{~256 256#:y'
res=: >@:{.

sockaddr_in=: 3 : 0
's fam host port'=. y
assert. fam=AF_INET
if. 0=#host do. host=. '0.0.0.0' end.
(hs AF_INET),(hns port),(afroms host),8#{.a.
)

sockaddr_split=: 3 : 0
'fam port host'=. 1 0 1 0 1 0 0 0 <;.1 (8){.y
assert. AF_INET = 256 256 #. a. i. bigendian fam
port=. 256 256 #. a. i. port
host=. }. , sfroma "0 host
host;port
)

sfroma=: 3 : 0
'.',": a. i. y
)
afroms=: 3 : 0
a.{~4{.".each '.' cutopen y
)
rc0=: 3 : 0
if. 0=>{.y do. 0 else. sdsockerror'' end.
)

sdsockaddress=: 3 : 0"0
r=. getsocknameJ y;(sockaddr_in_sz#{.a.);,sockaddr_in_sz
(rc0 r);data2string r
)
sdsend=: 4 : 0"1
if. '' -: $x do. x =. ,x end.
r=. >{.sendJ (>0{y);x;(#x);>1{y
if. _1=r do. 0;~sdsockerror'' else. 0;r end.
)
sdsendto=: 4 : 0"1
if. 3 = #y do.
  's flags saddr'=. y
  r=. >{.sendtoJ s;x;(#x);flags;saddr;sockaddr_in_sz
else.
  's flags family address port'=. y
  r=. >{.sendtoJ s;x;(#x);flags;(sockaddr_in 0 2 3 4{y);sockaddr_in_sz
end.
if. _1=r do. 0;~sdsockerror'' else. 0;r end.
)

sdcleanup=: 3 : '0[sdclose SOCKETS_jsocket_'
sdinit=: 3 : 0
if. 0=nc<'SOCKETS_jsocket_' do. 0 return. end.
SOCKETS_jsocket_=: ''
if. IFUNIX do. 0 return. end.
if. 0~:res WSAStartupJ 257;1000$' ' do. _1[mbinfo'Socket Error' else. 0 end.
)
sdrecv=: 3 : 0"1
's size'=. 2{.y
r=. recvJ s;(size#' ');size;2{3{.y
if. 0>c=. res r do. '';~sdsockerror'' return. end.
0;c{.>2{r
)
sdrecvfrom=: 3 : 0"1
's size flags'=. 3 {. y ,<0
s=. {.s
r=. recvfromJ s;(size#' ');size;flags;(sockaddr_in_sz#{.a.);,sockaddr_in_sz
'unexpected size of peer address' assert sockaddr_in_sz = 6 pick r
if. 0>c=. res r do. (sdsockerror '');'';'' return. end.
0;(c{.>2{r); 5{r
)
sdconnect=: 3 : 0"1
rc0 connectJ (>{.y);(sockaddr_in y);sockaddr_in_sz
)
sdsocket=: 3 : 0"1
s=. res socketJ <"0 [3{.y,(0=#y)#PF_INET,SOCK_STREAM,IPPROTO_TCP
if. s=_1 do. 0;~sdsockerror'' return. end.
SOCKETS_jsocket_=: SOCKETS_jsocket_,s
0;s
)
sdbind=: 3 : 0"1
rc0 bindJ (>{.y);(sockaddr_in y);sockaddr_in_sz
)
sdasync=: 3 : 0"0
if. IFUNIX do. 'not implemented under Unix - please use sdselect' assert 0 end.
flags=. OR/ FD_READ,FD_WRITE,FD_OOB,FD_ACCEPT,FD_CONNECT,FD_CLOSE
hwnd=. ".wd'qhwndx'
if. >{.WSAAsyncSelectJ ({.y);hwnd;1026;flags do. sdsockerror '' else. 0 end.
)
sdlisten=: 3 : 0"1
rc0 listenJ ;/2 {. y,<^:(L.y) SOMAXCONN
)
sdaccept=: 3 : 0"0
if. _1~:s=. res r=. acceptJ y;(sockaddr_in_sz$' ');,sockaddr_in_sz do.
  SOCKETS_jsocket_=: SOCKETS_jsocket_,s
  0;s
else. 0;~sdsockerror '' end.
)
sdgethostbyname=: 3 : 0
if. 0~:hostent=. res gethostbynameJ <y do.
  addr_list=. memr hostent, h_addr_list_off, 1, JPTR
  first_addr=. memr addr_list, 0, 1, JPTR
  'name did not resolve to address' assert first_addr ~: 0
  addr=. tostring memr first_addr,0,4
else.
  addr=. '255.255.255.255'
end.
0;PF_INET;addr
)
sdgethostbyaddr=: 3 : 0"1
'fam addr'=. y
phe=. res gethostbyaddrJ (afroms addr);4;fam
if. phe=0 do. _1;'unknown host' return. end.
a=. memr phe,h_name_off,1,JPTR
0;memr a,0,JSTR
)
sdclose=: 3 : 0"0
if. 0=res closesocketJ <y do.
  0[SOCKETS_jsocket_=: SOCKETS_jsocket_-.y
else.
  sdsockerror ''
end.
)
fdset_bytes=: 4 : 0
bitvector=. 1 y} (x*8)#0
bytes=. a. {~ _8 #.@|.\ bitvector
if. -.flip do. bytes=. , _4 |.\ bytes end.
bytes
)
fdset_fds=: 3 : 0
bytes=. y
if. -.flip do. bytes=. , _4 |.\ bytes end.
bitvec=. , _8 |.\ , (8#2)&#: a. i. bytes
I. bitvec
)
sdselect=: 3 : 0
if. 0=#y do. y=. SOCKETS_jsocket_;SOCKETS_jsocket_;SOCKETS_jsocket_;0 end.
'r w e t'=. y
time=. <<.1000000 1000000#:1000*t
if. IFUNIX do.
  max1=. >:>./r,w,e,0
  m=. 4
  n=. 32
  bytes=. m*>:<.n%~max1
  r=. bytes fdset_bytes r
  w=. bytes fdset_bytes w
  e=. bytes fdset_bytes e
  rwe=. r;w;e
else.
  max1=. 0
  rwe=. (] ,~ #) each r;w;e
end.
if. _1=res q=. selectJ (<max1),rwe,time do.
  (sdsockerror '');($0);($0);($0)
else.
  if. IFUNIX do. rwe=. fdset_fds each 2 3 4{q else. rwe=. ({.{.}.)each 2 3 4{q end.
  (<0),rwe
end.
)
sdgetsockopt=: 3 : 0
's lev name'=. y
r=. getsockoptJ s;lev;name;(,_1);,4
if. 0~:res r do. 0;~sdsockerror'' return. end.
d=. ''$>4{r
if. name-:SO_LINGER do. 0;65536 65536#:d else. 0;d end.
)
sdsetsockopt=: 3 : 0
's lev name val'=. y
if. name -: SO_LINGER do. val=. 65536 65536#.val end.
rc0 setsockoptJ s;lev;name;(,val);4
)
sdsockerror=: 3 : 0
> {. cderx ''
)
sdioctl=: 3 : 0
's option value'=. y
r=. ioctlsocketJ s;option;,value
if. 0~:res r do. 0;~sdsockerror'' else. 0;''$>3{r end.
)
sdionread=: 3 : 0
''$>{.sdcheck sdioctl y,FIONREAD,0
)
sdgethostname=: 3 : 0
if. 0=res r=. gethostnameJ (256#' ');256 do.
  0;>{.1 take (0{a.)cutopen ;1{r
else.
  0;'unknown host'
end.
)
sdgetpeername=: 3 : 0"0
namesub getpeernameJ y;(sockaddr_in_sz#{.a.);,sockaddr_in_sz
)
sdgetsockname=: 3 : 0"0
namesub getsocknameJ y;(sockaddr_in_sz#{.a.);,sockaddr_in_sz
)

sdgetsockets=: 3 : '0;SOCKETS_jsocket_'
sdcheck=: }. ` (sderror 13!:8 3:) @. (0 ~: >@{.)
INVALID_SOCKET=: 1
SOCKET_ERROR=: _1
sdinit''
