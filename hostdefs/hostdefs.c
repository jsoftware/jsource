#ifndef _WIN32
#include <unistd.h>
#include <sys/mman.h>
#include <regex.h>
#include <sys/time.h>
#include <sys/types.h>
#else
#include <stdio.h>
#include <winsock.h>
#include "../regex/rxposix.h"
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#define offset(r,f) (((char*)&((r*)0)->f)-((char*)((r*)0)))
main(){
	printf ("NB. do not edit -- created by sym2ijs\n\n");
	printf ("cocurrent <'jdefs'\n\n");
#ifndef _WIN32
	printf ("F_OK=: %d\n",F_OK);
	printf ("R_OK=: %d\n",R_OK);
	printf ("W_OK=: %d\n",W_OK);
	printf ("X_OK=: %d\n",X_OK);
	printf ("STDIN_FILENO=: %d\n",STDIN_FILENO);
	printf ("STDOUT_FILENO=: %d\n",STDOUT_FILENO);
	printf ("STDERR_FILENO=: %d\n",STDERR_FILENO);
#else
	puts ("F_OK=:0");
	puts ("R_OK=:4");
	puts ("W_OK=:2");
	puts ("X_OK=:1");
#endif
	printf ("SEEK_CUR=: %d\n",SEEK_CUR);
	printf ("SEEK_END=: %d\n",SEEK_END);
	printf ("SEEK_SET=: %d\n",SEEK_SET);
	puts("");
	printf ("O_APPEND=: %d\n",O_APPEND);
	printf ("O_CREAT=: %d\n",O_CREAT);
	printf ("O_EXCL=: %d\n",O_EXCL);
	printf ("O_RDONLY=: %d\n",O_RDONLY);
	printf ("O_RDWR=: %d\n",O_RDWR);
	printf ("O_TRUNC=: %d\n",O_TRUNC);
	printf ("O_WRONLY=: %d\n",O_WRONLY);
	puts("");
#ifndef _WIN32
	printf ("O_ACCMODE=: %d\n",O_ACCMODE);
	printf ("O_NOCTTY=: %d\n",O_NOCTTY);
	printf ("O_NONBLOCK=: %d\n",O_NONBLOCK);
	printf ("FD_CLOEXEC=: %d\n",FD_CLOEXEC);
	printf ("F_DUPFD=: %d\n",F_DUPFD);
	printf ("F_GETFD=: %d\n",F_GETFD);
	printf ("F_SETFD=: %d\n",F_SETFD);
	printf ("F_GETFL=: %d\n",F_GETFL);
	printf ("F_SETFL=: %d\n",F_SETFL);
	printf ("F_SETLK=: %d\n",F_SETLK);
	printf ("F_SETLKW=: %d\n",F_SETLKW);
	printf ("F_GETLK=: %d\n",F_GETLK);
	printf ("F_UNLCK=: %d\n",F_UNLCK);
	printf ("F_WRLCK=: %d\n",F_WRLCK);
	printf ("flock_sz=: %d\n",sizeof (struct flock));
	printf ("l_len_off=: %d\n",offset(struct flock,l_len));
	printf ("l_len_sz=: %d\n",sizeof(((struct flock*)0)->l_len));
	printf ("l_pid_off=: %d\n",offset(struct flock,l_pid));
	printf ("l_pid_sz=: %d\n",sizeof(((struct flock*)0)->l_pid));
	printf ("l_start_off=: %d\n",offset(struct flock,l_start));
	printf ("l_start_sz=: %d\n",sizeof(((struct flock*)0)->l_start));
	printf ("l_type_off=: %d\n",offset(struct flock,l_type));
	printf ("l_type_sz=: %d\n",sizeof(((struct flock*)0)->l_type));
	printf ("l_whence_off=: %d\n",offset(struct flock,l_whence));
	printf ("l_whence_sz=: %d\n",sizeof(((struct flock*)0)->l_whence));
	puts("");
	printf ("PROT_READ=: %d\n",PROT_READ);
	printf ("PROT_WRITE=: %d\n",PROT_WRITE);
	printf ("PROT_EXEC=: %d\n",PROT_EXEC);
	printf ("PROT_NONE=: %d\n",PROT_NONE);
	printf ("MAP_SHARED=: %d\n",MAP_SHARED);
	printf ("MAP_PRIVATE=: %d\n",MAP_PRIVATE);
	printf ("MAP_FIXED=: %d\n",MAP_FIXED);
#endif
	puts("");
	printf ("REG_EXTENDED=: %d\n",REG_EXTENDED);
	printf ("REG_ICASE=: %d\n",REG_ICASE);
	printf ("REG_NOSUB=: %d\n",REG_NOSUB);
	printf ("REG_NEWLINE=: %d\n",REG_NEWLINE);
	puts("");
	printf ("regex_t_sz=: %d\n",sizeof (regex_t));
	printf ("re_nsub_off=: %d\n",offset(regex_t,re_nsub));
	printf ("re_nsub_sz=: %d\n",sizeof(((regex_t*)0)->re_nsub));
	printf ("regmatch_t_sz=: %d\n",sizeof (regmatch_t));
	printf ("rm_so_off=: %d\n",offset(regmatch_t,rm_so));
	printf ("rm_so_sz=: %d\n",sizeof(((regmatch_t*)0)->rm_so));
	printf ("rm_eo_off=: %d\n",offset(regmatch_t,rm_eo));
	printf ("rm_eo_sz=: %d\n",sizeof(((regmatch_t*)0)->rm_eo));
	puts("");
#ifdef linux
#define fds_bits __fds_bits
#endif
	puts("");
#ifndef _WIN32
	printf ("fd_set_sz=: %d\n",sizeof (fd_set));
	printf ("fds_bits_off=: %d\n",offset(fd_set,fds_bits));
	printf ("fds_bits_sz=: %d\n",sizeof(((fd_set*)0)->fds_bits));
#endif
	printf ("FD_SETSIZE=: %d\n",FD_SETSIZE);
	printf ("timeval_sz=: %d\n",sizeof (struct timeval));
	printf ("tv_sec_off=: %d\n",offset(struct timeval,tv_sec));
	printf ("tv_sec_sz=: %d\n",sizeof(((struct timeval*)0)->tv_sec));
	printf ("tv_usec_off=: %d\n",offset(struct timeval,tv_usec));
	printf ("tv_usec_sz=: %d\n",sizeof(((struct timeval*)0)->tv_usec));
	puts("");
	printf ("E2BIG=: %d\n",E2BIG);
	printf ("EFAULT=: %d\n",EFAULT);
	printf ("ENFILE=: %d\n",ENFILE);
	printf ("ENOTTY=: %d\n",ENOTTY);
	printf ("EACCES=: %d\n",EACCES);
	printf ("EFBIG=: %d\n",EFBIG);
	printf ("ENODEV=: %d\n",ENODEV);
	printf ("ENXIO=: %d\n",ENXIO);
	printf ("EAGAIN=: %d\n",EAGAIN);
	printf ("ENOENT=: %d\n",ENOENT);
	printf ("EPERM=: %d\n",EPERM);
	printf ("EBADF=: %d\n",EBADF);
	printf ("EINTR=: %d\n",EINTR);
	printf ("ENOEXEC=: %d\n",ENOEXEC);
	printf ("EPIPE=: %d\n",EPIPE);
	printf ("EINVAL=: %d\n",EINVAL);
	printf ("ENOLCK=: %d\n",ENOLCK);
	printf ("ERANGE=: %d\n",ERANGE);
	printf ("EBUSY=: %d\n",EBUSY);
	printf ("EIO=: %d\n",EIO);
	printf ("ENOMEM=: %d\n",ENOMEM);
	printf ("EROFS=: %d\n",EROFS);
	printf ("EISDIR=: %d\n",EISDIR);
	printf ("ENOSPC=: %d\n",ENOSPC);
	printf ("ESPIPE=: %d\n",ESPIPE);
	printf ("ECHILD=: %d\n",ECHILD);
	printf ("EMFILE=: %d\n",EMFILE);
	printf ("ENOSYS=: %d\n",ENOSYS);
	printf ("ESRCH=: %d\n",ESRCH);
	printf ("EDEADLK=: %d\n",EDEADLK);
	printf ("EMLINK=: %d\n",EMLINK);
	printf ("ENOTDIR=: %d\n",ENOTDIR);
	printf ("EDOM=: %d\n",EDOM);
	printf ("ENOTEMPTY=: %d\n",ENOTEMPTY);
	printf ("EXDEV=: %d\n",EXDEV);
	printf ("EEXIST=: %d\n",EEXIST);
	printf ("ENAMETOOLONG=: %d\n",ENAMETOOLONG);
	puts("");
#ifndef _WIN32
	printf ("EINPROGRESS=: %d\n",EINPROGRESS);
	printf ("ECANCELED=: %d\n",ECANCELED);
	printf ("ETIMEDOUT=: %d\n",ETIMEDOUT);
	printf ("EMSGSIZE=: %d\n",EMSGSIZE);
	printf ("ENOTSUP=: %d\n",ENOTSUP);
#endif
	exit (0);
}
