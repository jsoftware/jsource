#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#ifdef sun
#include <inet/tcp.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif
#include <sys/types.h>
#define offset(r,f) (((char*)&((r*)0)->f)-((char*)((r*)0)))
main(){
	printf ("NB. do not edit -- created by sym2ijs\n\n");
	printf ("cocurrent <'jdefs'\n\n");
	printf ("FIONBIO=: %d\n",FIONBIO);
	printf ("FIONREAD=: %d\n",FIONREAD);
	printf ("FD_SETSIZE=: %d\n",FD_SETSIZE);
	puts("");
	printf ("sockaddr_sz=: %d\n",sizeof (struct sockaddr));
	printf ("sa_family_off=: %d\n",offset(struct sockaddr,sa_family));
	printf ("sa_family_sz=: %d\n",sizeof(((struct sockaddr*)0)->sa_family));
	printf ("sa_data_off=: %d\n",offset(struct sockaddr,sa_data));
	printf ("sa_data_sz=: %d\n",sizeof(((struct sockaddr*)0)->sa_data));
	puts("");
	printf ("sockaddr_in_sz=: %d\n",sizeof (struct sockaddr_in));
	printf ("sin_family_off=: %d\n",offset(struct sockaddr_in,sin_family));
	printf ("sin_family_sz=: %d\n",sizeof(((struct sockaddr_in*)0)->sin_family));
	printf ("sin_port_off=: %d\n",offset(struct sockaddr_in,sin_port));
	printf ("sin_port_sz=: %d\n",sizeof(((struct sockaddr_in*)0)->sin_port));
	printf ("sin_addr_off=: %d\n",offset(struct sockaddr_in,sin_addr));
	printf ("sin_addr_sz=: %d\n",sizeof(((struct sockaddr_in*)0)->sin_addr));
	puts("");
	printf ("in_addr_sz=: %d\n",sizeof (struct in_addr));
	printf ("s_addr_off=: %d\n",offset(struct in_addr,s_addr));
	printf ("s_addr_sz=: %d\n",sizeof(((struct in_addr*)0)->s_addr));
	puts("");
	printf ("hostent_sz=: %d\n",sizeof (struct hostent));
	printf ("h_name_off=: %d\n",offset(struct hostent,h_name));
	printf ("h_name_sz=: %d\n",sizeof(((struct hostent*)0)->h_name));
	printf ("h_aliases_off=: %d\n",offset(struct hostent,h_aliases));
	printf ("h_aliases_sz=: %d\n",sizeof(((struct hostent*)0)->h_aliases));
	printf ("h_addrtype_off=: %d\n",offset(struct hostent,h_addrtype));
	printf ("h_addrtype_sz=: %d\n",sizeof(((struct hostent*)0)->h_addrtype));
	printf ("h_length_off=: %d\n",offset(struct hostent,h_length));
	printf ("h_length_sz=: %d\n",sizeof(((struct hostent*)0)->h_length));
	printf ("h_addr_list_off=: %d\n",offset(struct hostent,h_addr_list));
	printf ("h_addr_list_sz=: %d\n",sizeof(((struct hostent*)0)->h_addr_list));
	puts("");
	puts("");
	printf ("SIOCATMARK=: %d\n",SIOCATMARK);
	puts("");
	puts("");
	printf ("IPPROTO_IP=: %d\n",IPPROTO_IP);
	printf ("IPPROTO_ICMP=: %d\n",IPPROTO_ICMP);
	printf ("IPPROTO_IGMP=: %d\n",IPPROTO_IGMP);
	printf ("IPPROTO_TCP=: %d\n",IPPROTO_TCP);
	printf ("IPPROTO_PUP=: %d\n",IPPROTO_PUP);
	printf ("IPPROTO_UDP=: %d\n",IPPROTO_UDP);
	printf ("IPPROTO_IDP=: %d\n",IPPROTO_IDP);
	printf ("IPPROTO_RAW=: %d\n",IPPROTO_RAW);
	printf ("IPPROTO_MAX=: %d\n",IPPROTO_MAX);
	puts("");
	puts("");
	printf ("INADDR_ANY=: %d\n",INADDR_ANY);
	printf ("INADDR_LOOPBACK=: %d\n",INADDR_LOOPBACK);
	printf ("INADDR_BROADCAST=: %d\n",INADDR_BROADCAST);
#ifdef INADDR_NONE
	printf ("INADDR_NONE=: %d\n",INADDR_NONE);
#else
	puts ("INADDR_NONE=: _1");
#endif
	puts("");
#ifndef _WIN32
	printf ("INADDR_UNSPEC_GROUP=: %d\n",INADDR_UNSPEC_GROUP);
	printf ("INADDR_ALLHOSTS_GROUP=: %d\n",INADDR_ALLHOSTS_GROUP);
	printf ("INADDR_MAX_LOCAL_GROUP=: %d\n",INADDR_MAX_LOCAL_GROUP);
	printf ("IN_LOOPBACKNET=: %d\n",IN_LOOPBACKNET);
#endif
	puts("");
	printf ("SOCK_STREAM=: %d\n",SOCK_STREAM);
	printf ("SOCK_DGRAM=: %d\n",SOCK_DGRAM);
	printf ("SOCK_RAW=: %d\n",SOCK_RAW);
	printf ("SOCK_RDM=: %d\n",SOCK_RDM);
	printf ("SOCK_SEQPACKET=: %d\n",SOCK_SEQPACKET);
	puts("");
	printf ("SOL_SOCKET=: %d\n",SOL_SOCKET);
	printf ("SO_DEBUG=: %d\n",SO_DEBUG);
	printf ("SO_REUSEADDR=: %d\n",SO_REUSEADDR);
	printf ("SO_KEEPALIVE=: %d\n",SO_KEEPALIVE);
	printf ("SO_DONTROUTE=: %d\n",SO_DONTROUTE);
	printf ("SO_BROADCAST=: %d\n",SO_BROADCAST);
	printf ("SO_LINGER=: %d\n",SO_LINGER);
	printf ("SO_OOBINLINE=: %d\n",SO_OOBINLINE);
	puts("");
	printf ("SO_SNDBUF=: %d\n",SO_SNDBUF);
	printf ("SO_RCVBUF=: %d\n",SO_RCVBUF);
	printf ("SO_SNDLOWAT=: %d\n",SO_SNDLOWAT);
	printf ("SO_RCVLOWAT=: %d\n",SO_RCVLOWAT);
	printf ("SO_SNDTIMEO=: %d\n",SO_SNDTIMEO);
	printf ("SO_RCVTIMEO=: %d\n",SO_RCVTIMEO);
	printf ("SO_ERROR=: %d\n",SO_ERROR);
	printf ("SO_TYPE=: %d\n",SO_TYPE);
	puts("");
	printf ("linger_sz=: %d\n",sizeof (struct linger));
	printf ("l_onoff_off=: %d\n",offset(struct linger,l_onoff));
	printf ("l_onoff_sz=: %d\n",sizeof(((struct linger*)0)->l_onoff));
	printf ("l_linger_off=: %d\n",offset(struct linger,l_linger));
	printf ("l_linger_sz=: %d\n",sizeof(((struct linger*)0)->l_linger));
	puts("");
	printf ("AF_UNSPEC=: %d\n",AF_UNSPEC);
	printf ("AF_UNIX=: %d\n",AF_UNIX);
	printf ("AF_INET=: %d\n",AF_INET);
	printf ("AF_SNA=: %d\n",AF_SNA);
	printf ("AF_DECnet=: %d\n",AF_DECnet);
	printf ("AF_APPLETALK=: %d\n",AF_APPLETALK);
	printf ("AF_IPX=: %d\n",AF_IPX);
	printf ("AF_MAX=: %d\n",AF_MAX);
	printf ("PF_UNSPEC=: %d\n",PF_UNSPEC);
	printf ("PF_UNIX=: %d\n",PF_UNIX);
	printf ("PF_INET=: %d\n",PF_INET);
	printf ("PF_SNA=: %d\n",PF_SNA);
	printf ("PF_DECnet=: %d\n",PF_DECnet);
	printf ("PF_APPLETALK=: %d\n",PF_APPLETALK);
	printf ("PF_IPX=: %d\n",PF_IPX);
	printf ("PF_MAX=: %d\n",PF_MAX);
	puts("");
	printf ("SOMAXCONN=: %d\n",SOMAXCONN);
	printf ("MSG_OOB=: %d\n",MSG_OOB);
	printf ("MSG_PEEK=: %d\n",MSG_PEEK);
	printf ("MSG_DONTROUTE=: %d\n",MSG_DONTROUTE);
	puts("");
#ifndef _WIN32
	printf ("msghdr_sz=: %d\n",sizeof (struct msghdr));
	printf ("msg_name_off=: %d\n",offset(struct msghdr,msg_name));
	printf ("msg_name_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_name));
	printf ("msg_namelen_off=: %d\n",offset(struct msghdr,msg_namelen));
	printf ("msg_namelen_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_namelen));
	printf ("msg_iov_off=: %d\n",offset(struct msghdr,msg_iov));
	printf ("msg_iov_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_iov));
	printf ("msg_iovlen_off=: %d\n",offset(struct msghdr,msg_iovlen));
	printf ("msg_iovlen_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_iovlen));
#if defined(linux) || defined(Darwin)
	printf ("msg_control_off=: %d\n",offset(struct msghdr,msg_control));
	printf ("msg_control_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_control));
	printf ("msg_controllen_off=: %d\n",offset(struct msghdr,msg_controllen));
	printf ("msg_controllen_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_controllen));
	printf ("msg_flags_off=: %d\n",offset(struct msghdr,msg_flags));
	printf ("msg_flags_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_flags));
#else
	printf ("msg_accrights_off=: %d\n",offset(struct msghdr,msg_accrights));
	printf ("msg_accrights_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_accrights));
	printf ("msg_accrightslen_off=: %d\n",offset(struct msghdr,msg_accrightslen));
	printf ("msg_accrightslen_sz=: %d\n",sizeof(((struct msghdr*)0)->msg_accrightslen));
#endif
#endif
	puts("");
#ifdef _WIN32
	printf ("SOCKET_ERROR=: %d\n",SOCKET_ERROR);
	printf ("FD_READ=: %d\n",FD_READ);
	printf ("FD_WRITE=: %d\n",FD_WRITE);
	printf ("FD_OOB=: %d\n",FD_OOB);
	printf ("FD_ACCEPT=: %d\n",FD_ACCEPT);
	printf ("FD_CONNECT=: %d\n",FD_CONNECT);
	printf ("FD_CLOSE=: %d\n",FD_CLOSE);
	printf ("WSABASEERR=: %d\n",WSABASEERR);
	printf ("WSAEINTR=: %d\n",WSAEINTR);
	printf ("WSAEBADF=: %d\n",WSAEBADF);
	printf ("WSAEACCES=: %d\n",WSAEACCES);
	printf ("WSAEFAULT=: %d\n",WSAEFAULT);
	printf ("WSAEINVAL=: %d\n",WSAEINVAL);
	printf ("WSAEMFILE=: %d\n",WSAEMFILE);
	printf ("WSAEWOULDBLOCK=: %d\n",WSAEWOULDBLOCK);
	printf ("WSAEINPROGRESS=: %d\n",WSAEINPROGRESS);
	printf ("WSAEALREADY=: %d\n",WSAEALREADY);
	printf ("WSAENOTSOCK=: %d\n",WSAENOTSOCK);
	printf ("WSAEDESTADDRREQ=: %d\n",WSAEDESTADDRREQ);
	printf ("WSAEMSGSIZE=: %d\n",WSAEMSGSIZE);
	printf ("WSAEPROTOTYPE=: %d\n",WSAEPROTOTYPE);
	printf ("WSAENOPROTOOPT=: %d\n",WSAENOPROTOOPT);
	printf ("WSAEPROTONOSUPPORT=: %d\n",WSAEPROTONOSUPPORT);
	printf ("WSAESOCKTNOSUPPORT=: %d\n",WSAESOCKTNOSUPPORT);
	printf ("WSAEOPNOTSUPP=: %d\n",WSAEOPNOTSUPP);
	printf ("WSAEPFNOSUPPORT=: %d\n",WSAEPFNOSUPPORT);
	printf ("WSAEAFNOSUPPORT=: %d\n",WSAEAFNOSUPPORT);
	printf ("WSAEADDRINUSE=: %d\n",WSAEADDRINUSE);
	printf ("WSAEADDRNOTAVAIL=: %d\n",WSAEADDRNOTAVAIL);
	printf ("WSAENETDOWN=: %d\n",WSAENETDOWN);
	printf ("WSAENETUNREACH=: %d\n",WSAENETUNREACH);
	printf ("WSAENETRESET=: %d\n",WSAENETRESET);
	printf ("WSAECONNABORTED=: %d\n",WSAECONNABORTED);
	printf ("WSAECONNRESET=: %d\n",WSAECONNRESET);
	printf ("WSAENOBUFS=: %d\n",WSAENOBUFS);
	printf ("WSAEISCONN=: %d\n",WSAEISCONN);
	printf ("WSAENOTCONN=: %d\n",WSAENOTCONN);
	printf ("WSAESHUTDOWN=: %d\n",WSAESHUTDOWN);
	printf ("WSAETOOMANYREFS=: %d\n",WSAETOOMANYREFS);
	printf ("WSAETIMEDOUT=: %d\n",WSAETIMEDOUT);
	printf ("WSAECONNREFUSED=: %d\n",WSAECONNREFUSED);
	printf ("WSAELOOP=: %d\n",WSAELOOP);
	printf ("WSAENAMETOOLONG=: %d\n",WSAENAMETOOLONG);
	printf ("WSAEHOSTDOWN=: %d\n",WSAEHOSTDOWN);
	printf ("WSAEHOSTUNREACH=: %d\n",WSAEHOSTUNREACH);
	printf ("WSAENOTEMPTY=: %d\n",WSAENOTEMPTY);
	printf ("WSAEPROCLIM=: %d\n",WSAEPROCLIM);
	printf ("WSAEUSERS=: %d\n",WSAEUSERS);
	printf ("WSAEDQUOT=: %d\n",WSAEDQUOT);
	printf ("WSAESTALE=: %d\n",WSAESTALE);
	printf ("WSAEREMOTE=: %d\n",WSAEREMOTE);
	printf ("WSASYSNOTREADY=: %d\n",WSASYSNOTREADY);
	printf ("WSAVERNOTSUPPORTED=: %d\n",WSAVERNOTSUPPORTED);
	printf ("WSANOTINITIALISED=: %d\n",WSANOTINITIALISED);
	printf ("WSAHOST_NOT_FOUND=: %d\n",WSAHOST_NOT_FOUND);
	printf ("HOST_NOT_FOUND=: %d\n",HOST_NOT_FOUND);
	printf ("WSATRY_AGAIN=: %d\n",WSATRY_AGAIN);
	printf ("TRY_AGAIN=: %d\n",TRY_AGAIN);
	printf ("WSANO_RECOVERY=: %d\n",WSANO_RECOVERY);
	printf ("NO_RECOVERY=: %d\n",NO_RECOVERY);
	printf ("WSANO_DATA=: %d\n",WSANO_DATA);
	printf ("NO_DATA=: %d\n",NO_DATA);
	printf ("WSANO_ADDRESS=: %d\n",WSANO_ADDRESS);
	printf ("NO_ADDRESS=: %d\n",NO_ADDRESS);
	printf ("WM_USER=: %d\n",WM_USER);
#endif
	exit (0);
}
