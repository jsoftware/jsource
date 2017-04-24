/* Copyright 1990-2000, Jsoftware Inc.  All rights reserved. */
/* Licensed use only. Any other use is in violation of copyright. */
/*                                                                         */
typedef int (_stdcall JDoType)(void*, LPSTR);
typedef int (_stdcall JBreakType)(void*);
typedef int (_stdcall JIsBusyType)(void*);
typedef int (_stdcall JGetType)(void*, LPSTR, VARIANT*);
typedef int (_stdcall JSetType)(void*, LPSTR, VARIANT*);
typedef int (_stdcall JGetMType)(void*, LPSTR, long*, long*, long*, long*);
typedef int (_stdcall JSetMType)(void*, LPSTR, long*, long*, long*, long*);
typedef int (_stdcall JErrorTextType)(void*, long, VARIANT*);
typedef int (_stdcall JErrorTextMType)(void*, long, long*);
typedef int (_stdcall JClearType)(void*);
typedef int (_stdcall JShowType)(void*, long);
typedef int (_stdcall JLogType)(void*, long);
typedef int (_stdcall JQuitType)(void*, long);
typedef int (_stdcall JTransposeType)(void*, long);
typedef int (_stdcall JErrorTextBType)(void*, long, VARIANT* pbstr);
typedef int (_stdcall JGetBType)(void*, LPSTR, VARIANT*);
typedef int (_stdcall JSetBType)(void*, LPSTR, VARIANT*);
typedef int (_stdcall JDoRType)(void*, LPSTR, VARIANT*);
typedef int (_stdcall JInt64RType)(void*, long);
