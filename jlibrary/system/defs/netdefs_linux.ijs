NB. do not edit -- created by sym2ijs
cocurrent<'jdefs'

FIONBIO=: 21537
FIONREAD=: 21531
FD_SETSIZE=: 1024

sockaddr_sz=: 16
sa_family_off=: 0
sa_family_sz=: 2
sa_data_off=: 2
sa_data_sz=: 14

sockaddr_in_sz=: 16
sin_family_off=: 0
sin_family_sz=: 2
sin_port_off=: 2
sin_port_sz=: 2
sin_addr_off=: 4
sin_addr_sz=: 4

in_addr_sz=: 4
s_addr_off=: 0
s_addr_sz=: 4

hostent_sz=: 20
h_name_off=: 0
h_name_sz=: 4
h_aliases_off=: 4
h_aliases_sz=: 4
h_addrtype_off=: 8
h_addrtype_sz=: 4
h_length_off=: 12
h_length_sz=: 4
h_addr_list_off=: 16
h_addr_list_sz=: 4


SIOCATMARK=: 35077


IPPROTO_IP=: 0
IPPROTO_ICMP=: 1
IPPROTO_IGMP=: 2
IPPROTO_TCP=: 6
IPPROTO_PUP=: 12
IPPROTO_UDP=: 17
IPPROTO_IDP=: 22
IPPROTO_RAW=: 255
IPPROTO_MAX=: 256


INADDR_ANY=: 0
INADDR_LOOPBACK=: 2130706433
INADDR_BROADCAST=: -1
INADDR_NONE=: -1

INADDR_UNSPEC_GROUP=: -536870912
INADDR_ALLHOSTS_GROUP=: -536870911
INADDR_MAX_LOCAL_GROUP=: -536870657
IN_LOOPBACKNET=: 127

SOCK_STREAM=: 1
SOCK_DGRAM=: 2
SOCK_RAW=: 3
SOCK_RDM=: 4
SOCK_SEQPACKET=: 5

SOL_SOCKET=: 1
SO_DEBUG=: 1
SO_REUSEADDR=: 2
SO_KEEPALIVE=: 9
SO_DONTROUTE=: 5
SO_BROADCAST=: 6
SO_LINGER=: 13
SO_OOBINLINE=: 10

SO_SNDBUF=: 7
SO_RCVBUF=: 8
SO_SNDLOWAT=: 19
SO_RCVLOWAT=: 18
SO_SNDTIMEO=: 21
SO_RCVTIMEO=: 20
SO_ERROR=: 4
SO_TYPE=: 3

linger_sz=: 8
l_onoff_off=: 0
l_onoff_sz=: 4
l_linger_off=: 4
l_linger_sz=: 4

AF_UNSPEC=: 0
AF_UNIX=: 1
AF_INET=: 2
AF_SNA=: 22
AF_DECnet=: 12
AF_APPLETALK=: 5
AF_IPX=: 4
AF_MAX=: 32
PF_UNSPEC=: 0
PF_UNIX=: 1
PF_INET=: 2
PF_SNA=: 22
PF_DECnet=: 12
PF_APPLETALK=: 5
PF_IPX=: 4
PF_MAX=: 32

SOMAXCONN=: 128
MSG_OOB=: 1
MSG_PEEK=: 2
MSG_DONTROUTE=: 4

msghdr_sz=: 28
msg_name_off=: 0
msg_name_sz=: 4
msg_namelen_off=: 4
msg_namelen_sz=: 4
msg_iov_off=: 8
msg_iov_sz=: 4
msg_iovlen_off=: 12
msg_iovlen_sz=: 4
msg_control_off=: 16
msg_control_sz=: 4
msg_controllen_off=: 20
msg_controllen_sz=: 4
msg_flags_off=: 24
msg_flags_sz=: 4

