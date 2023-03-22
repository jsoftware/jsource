/* Definitions for GNU multiple precision functions.   -*- mode: c -*-

Copyright 1991, 1993, 1994, 1995, 1996, 1997, 1999, 2000, 2001, 2002, 2003,
2004, 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

/*
libgmp binaries are built from https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz
*/

#ifdef gmp_wrapper_h
#error "gmp_wrapper_h should not be defined!"
#endif
#define gmp_wrapper_h

#if defined(__wasm32__)
#include "linux/gmp-wasm32.h"
#elif defined(ANDROID)
#if defined(__aarch64__)
#include "android/gmp-arm64-v8a.h"
#elif defined(__arm__)
#ifdef ARMEABI_V7A
#include "android/gmp-armeabi-v7a.h"
#else
#include "android/gmp-armeabi.h"
#endif
#elif defined(__x86_64__)
#include "android/gmp-x86_64.h"
#elif defined(__i386__)
#include "android/gmp-x86.h"
#else
#error "The gmp-devel package is not usable with the architecture."
#endif
#elif defined(__APPLE__)
#if defined(__aarch64__)
#include "apple/gmp-arm64.h"
#elif defined(__x86_64__)
#include "apple/gmp-x86_64.h"
#else
#error "The gmp-devel package is not usable with the architecture."
#endif
#else
/* assume linux */
#if defined(__arm__)
#include "linux/gmp-arm.h"
#elif defined(__i386__)
#include "linux/gmp-i386.h"
#elif defined(__ia64__)
#include "linux/gmp-ia64.h"
#elif defined(__powerpc64__)
# if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#include "linux/gmp-ppc64.h"
# else
#include "linux/gmp-ppc64le.h"
# endif
#elif defined(__powerpc__)
# if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#include "linux/gmp-ppc.h"
# else
#include "linux/gmp-ppcle.h"
# endif
#elif defined(__s390x__)
#include "linux/gmp-s390x.h"
#elif defined(__s390__)
#include "linux/gmp-s390.h"
#elif defined(__x86_64__)
#include "linux/gmp-x86_64.h"
#elif defined(__alpha__)
#include "linux/gmp-alpha.h"
#elif defined(__sh__)
#include "linux/gmp-sh.h"
#elif defined(__sparc__) && defined (__arch64__)
#include "linux/gmp-sparc64.h"
#elif defined(__sparc__)
#include "linux/gmp-sparc.h"
#elif defined(__aarch64__)
#include "linux/gmp-aarch64.h"
#elif defined(__mips64) && defined(__MIPSEL__)
#include "linux/gmp-mips64el.h"
#elif defined(__mips64)
#include "linux/gmp-mips64.h"
#elif defined(__mips) && defined(__MIPSEL__)
#include "linux/gmp-mipsel.h"
#elif defined(__mips)
#include "linux/gmp-mips.h"
#elif defined(__riscv)
#if __riscv_xlen == 64
#include "linux/gmp-riscv64.h"
#else
#error "No support for riscv32"
#endif
#else
#error "The gmp-devel package is not usable with the architecture."
#endif
#endif

#undef gmp_wrapper_h
