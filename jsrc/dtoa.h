/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/* Jsoftware Copyright applies only to changes made by Jsoftware           */
#include <setjmp.h>

#if SYS & SYS_MACOSX
/* Use the non-signal restoring pair */
#define setjmp _setjmp
#define longjmp _longjmp
#endif

#ifndef HAVE_BIGINT
struct dtoa_info {
	jmp_buf _env;
};
#else
struct dtoa_info {
	jmp_buf _env;
	Bigint *_p5s;
	double *_pmem_next;
	double _private_mem[PRIVATE_mem];
	Bigint *_freelist[Kmax+1];
	void *jt;
	int ndp;
	char *result;
};
#endif
