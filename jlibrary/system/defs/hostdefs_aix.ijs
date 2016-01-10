NB. do not edit -- created by sym2ijs
cocurrent<'jdefs'

F_OK=: 0
R_OK=: 4
W_OK=: 2
X_OK=: 1
STDIN_FILENO=: 0
STDOUT_FILENO=: 1
STDERR_FILENO=: 2
SEEK_CUR=: 1
SEEK_END=: 2
SEEK_SET=: 0

O_APPEND=: 8
O_CREAT=: 256
O_EXCL=: 1024
O_RDONLY=: 0
O_RDWR=: 2
O_TRUNC=: 512
O_WRONLY=: 1

O_ACCMODE=: 3
O_NOCTTY=: 2048
O_NONBLOCK=: 4
FD_CLOEXEC=: 1
F_DUPFD=: 0
F_GETFD=: 1
F_SETFD=: 2
F_GETFL=: 3
F_SETFL=: 4
F_SETLK=: 6
F_SETLKW=: 7
F_GETLK=: 5
F_UNLCK=: 3
F_WRLCK=: 2
flock_sz=: 24
l_len_off=: 8
l_len_sz=: 4
l_pid_off=: 16
l_pid_sz=: 4
l_start_off=: 4
l_start_sz=: 4
l_type_off=: 0
l_type_sz=: 2
l_whence_off=: 2
l_whence_sz=: 2

PROT_READ=: 1
PROT_WRITE=: 2
PROT_EXEC=: 4
PROT_NONE=: 0
MAP_SHARED=: 1
MAP_PRIVATE=: 2
MAP_FIXED=: 256

REG_EXTENDED=: 1
REG_ICASE=: 2
REG_NOSUB=: 8
REG_NEWLINE=: 4

regex_t_sz=: 360
re_nsub_off=: 0
re_nsub_sz=: 4
regmatch_t_sz=: 8
rm_so_off=: 0
rm_so_sz=: 4
rm_eo_off=: 4
rm_eo_sz=: 4


fd_set_sz=: 4096
fds_bits_off=: 0
fds_bits_sz=: 4096
FD_SETSIZE=: 32767
timeval_sz=: 8
tv_sec_off=: 0
tv_sec_sz=: 4
tv_usec_off=: 4
tv_usec_sz=: 4

E2BIG=: 7
EFAULT=: 14
ENFILE=: 23
ENOTTY=: 25
EACCES=: 13
EFBIG=: 27
ENODEV=: 19
ENXIO=: 6
EAGAIN=: 11
ENOENT=: 2
EPERM=: 1
EBADF=: 9
EINTR=: 4
ENOEXEC=: 8
EPIPE=: 32
EINVAL=: 22
ENOLCK=: 49
ERANGE=: 34
EBUSY=: 16
EIO=: 5
ENOMEM=: 12
EROFS=: 30
EISDIR=: 21
ENOSPC=: 28
ESPIPE=: 29
ECHILD=: 10
EMFILE=: 24
ENOSYS=: 109
ESRCH=: 3
EDEADLK=: 45
EMLINK=: 31
ENOTDIR=: 20
EDOM=: 33
ENOTEMPTY=: 17
EXDEV=: 18
EEXIST=: 17
ENAMETOOLONG=: 86

EINPROGRESS=: 55
ECANCELED=: 117
ETIMEDOUT=: 78
EMSGSIZE=: 59
ENOTSUP=: 124
