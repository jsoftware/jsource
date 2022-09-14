/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Macros and Defined Constants for !:                              */



/* adding suffix _O to avoid possible name conflict with fcntl.h */
#define FAPPEND_O         "ab"
#define FREAD_O           "rb"
#define FUPDATE_O         "r+b"
#define FUPDATEC_O        "w+b"
#define FWRITE_O          "wb"

#define FLAPPEND_O         L"ab"
#define FLREAD_O           L"rb"
#define FLUPDATE_O         L"r+b"
#define FLUPDATEC_O        L"w+b"
#define FLWRITE_O          L"wb"

#ifndef SEEK_SET
#define SEEK_SET        0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR        1
#endif
#ifndef SEEK_END
#define SEEK_END        2
#endif

#ifdef L_tmpnam
#undef L_tmpnam
#endif
#define L_tmpnam        300  /* full path name can be very long */

#if SY_WINCE
#define _wmkdir(x)	(!CreateDirectory (x,0))
#define _wrmdir(x)	(!RemoveDirectory (x))
#define _wunlink(x)	(!DeleteFile (x))

wchar_t *tounibuf(char * src);
char *toascbuf(wchar_t *src);
#endif
