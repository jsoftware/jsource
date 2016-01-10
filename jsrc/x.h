/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Macros and Defined Constants for !:                              */


#define XCC             127
#define XC(m,n)         (n+XCC*m)

#define FAPPEND         "ab"
#define FREAD           "rb"
#define FUPDATE         "r+b"
#define FUPDATEC        "w+b"
#define FWRITE          "wb"

#define FLAPPEND         L"ab"
#define FLREAD           L"rb"
#define FLUPDATE         L"r+b"
#define FLUPDATEC        L"w+b"
#define FLWRITE          L"wb"

#ifndef SEEK_SET
#define SEEK_SET        0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR        1
#endif
#ifndef SEEK_END
#define SEEK_END        2
#endif

#ifndef L_tmpnam
#define L_tmpnam        59
#endif

#if SY_WINCE
#define _wmkdir(x)	(!CreateDirectory (x,0))
#define _wrmdir(x)	(!RemoveDirectory (x))
#define _wunlink(x)	(!DeleteFile (x))

wchar_t *tounibuf(char * src);
char *toascbuf(wchar_t *src);
#endif
