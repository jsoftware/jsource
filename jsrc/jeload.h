/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* utilities for JFE to load JE, initiallize, and run profile sentence     */

void jepath(char*,char *);
void jesetpath(char*);
void* jeload(void* callbacks); // returns J
int jefirst(int,char*);
A jegeta(int,char*);
I jeseta(I n,char* name,I x,char* d);
int jedo(char*);
void jeinterrupt();
void jefree();
char* jegetlocale();
void jefail(char*);
void* jega(I t, I n, I r, I*s); // returns A
void* jehjdll();
