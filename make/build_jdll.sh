#!/bin/bash
# $1 is j32 or j64

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5
 
if [ x$CC = x'' ] ; then
if [ -f "/usr/bin/cc" ]; then
CC=cc
export CC
else
CC=gcc
export CC
fi
fi
if [ $($CC -v 2>&1 | grep -c "clang\ version\|Apple\ LLVM\ version") -eq 1 ] ; then
COMPILER='clang'
else
COMPILER='gcc'
fi
export COMPILER
echo "COMPILER $COMPILER"

USE_OPENMP="${USE_OPENMP:=0}"
if [ $USE_OPENMP -eq 1 ] ; then
OPENMP=" -fopenmp "
# LDOPENMP=" -fopenmp "
# LDOPENMP32=" -l:libgomp.so.1 "    # gcc
# LDOPENMP32=" -l:libomp.so.5 "     # clang
LDOPENMP=" -fopenmp `$CC -print-file-name=libgomp.a` "   # windows -fopenmp for pthread
LDOPENMP32=" -fopenmp `$CC -print-file-name=libgomp.a` "
fi

if [ x$COMPILER = x'gcc' ] ; then
# gcc
common="$OPENMP -fPIC -O1 -fwrapv -fno-strict-aliasing -Wextra -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-shift-negative-value"
else
# clang 3.5 .. 5.0
common="$OPENMP -Werror -fPIC -O1 -fwrapv -fno-strict-aliasing -Wextra -Wno-consumed -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-unsequenced -Wno-string-plus-int"
fi
darwin="$OPENMP -fPIC -O1 -fwrapv -fno-strict-aliasing -Wno-string-plus-int -Wno-empty-body -Wno-unsequenced -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-return-type -Wno-constant-logical-operand -Wno-comment -Wno-unsequenced"

case $jplatform\_$1 in


linux_j32) # linux x86
TARGET=libj.so
# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
COMPILE="$common -m32 -msse2 -mfpmath=sse -DC_NOMULTINTRINSIC "
# slower, use 387 fpu and truncate extra precision
# COMPILE="$common -m32 -ffloat-store "
LINK=" -shared -Wl,-soname,libj.so -m32 -lm -ldl $LDOPENMP32 -o libj.so "
;;

linux_j64) # linux intel 64bit
TARGET=libj.so
COMPILE="$common $OPENMP "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libj.so "
;;

linux_j64avx) # linux intel 64bit avx
TARGET=libj.so
COMPILE="$common -mavx -DC_AVX=1 $OPENMP "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libj.so "
OBJS_FMA=" blis/gemm_int-fma.o "
;;

raspberry_j32) # linux raspbian arm
TARGET=libj.so
COMPILE="$common -DRASPI -DC_NOMULTINTRINSIC "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libj.so "
;;

darwin_j32) # darwin x86
TARGET=libj.dylib
COMPILE="$darwin -m32 -mmacosx-version-min=10.5"
LINK=" -dynamiclib -lm -ldl $LDOPENMP -m32 -mmacosx-version-min=10.5 -o libj.dylib"
;;

darwin_j64) # darwin x86
TARGET=libj.dylib
COMPILE="$darwin -mmacosx-version-min=10.5"
LINK=" -dynamiclib -lm -ldl $LDOPENMP -mmacosx-version-min=10.5 -o libj.dylib"
;;

darwin_j64avx) # darwin intel 64bit avx
TARGET=libj.dylib
COMPILE="$darwin -mavx -mmacosx-version-min=10.5 -DC_AVX=1"
LINK=" -dynamiclib -lm -ldl $LDOPENMP -mmacosx-version-min=10.5 -o libj.dylib"
OBJS_FMA=" blis/gemm_int-fma.o "
;;

windows_j32) # windows x86
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" $jgit/dllsrc/jdll.def "
LIBJRES=" jdllres.o "
TARGET=j.dll
COMPILE="$common -msse2 -mfpmath=sse -Wno-write-strings -D_FILE_OFFSET_BITS=64 -D_JDLL "
LINK=" -shared -Wl,--enable-stdcall-fixup -lm -lole32 -loleaut32 -luuid -static-libgcc -static-libstdc++ $LDOPENMP32 -o j.dll "
;;

windows_j32avx) # windows x86
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" $jgit/dllsrc/jdll.def "
LIBJRES=" jdllres.o "
TARGET=j.dll
COMPILE="$common -msse2 -mfpmath=sse -Wno-write-strings -D_FILE_OFFSET_BITS=64 -D_JDLL -mavx -DC_AVX=1 "
LINK=" -shared -Wl,--enable-stdcall-fixup -lm -lole32 -loleaut32 -luuid -static-libgcc -static-libstdc++ $LDOPENMP32 -o j.dll "
OBJS_FMA=" blis/gemm_int-fma.o "
;;

windows_j64) # windows x86
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" $jgit/dllsrc/jdll.def "
LIBJRES=" jdllres.o "
TARGET=j.dll
COMPILE="$common -Wno-write-strings -D_FILE_OFFSET_BITS=64 -D_JDLL "
LINK=" -shared -lm -lole32 -loleaut32 -luuid -static-libgcc -static-libstdc++ $LDOPENMP -o j.dll "
;;

windows_j64avx) # windows x86
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" $jgit/dllsrc/jdll.def "
LIBJRES=" jdllres.o "
TARGET=j.dll
COMPILE="$common -Wno-write-strings -D_FILE_OFFSET_BITS=64 -D_JDLL -mavx -DC_AVX=1 "
LINK=" -shared -lm -lole32 -loleaut32 -luuid -static-libgcc -static-libstdc++ $LDOPENMP -o j.dll "
OBJS_FMA=" blis/gemm_int-fma.o "
;;

*)
echo no case for those parameters
exit
esac

OBJS="\
 a.o \
 ab.o \
 af.o \
 ai.o \
 am.o \
 am1.o \
 amn.o \
 ao.o \
 ap.o \
 ar.o \
 as.o \
 au.o \
 blis/gemm_c-ref.o \
 blis/gemm_int-aarch64.o \
 blis/gemm_int-avx.o \
 blis/gemm_int-sse2.o \
 blis/gemm_vec-ref.o \
 c.o \
 ca.o \
 cc.o \
 cd.o \
 cf.o \
 cg.o \
 ch.o \
 cip.o \
 cl.o \
 cp.o \
 cpdtsp.o \
 cpuinfo.o \
 cr.o \
 crs.o \
 ct.o \
 cu.o \
 cv.o \
 cx.o \
 d.o \
 dc.o \
 dss.o \
 dstop.o \
 dsusp.o \
 dtoa.o \
 f.o \
 f2.o \
 fbu.o \
 gemm.o \
 i.o \
 io.o \
 j.o \
 jdlllic.o \
 k.o \
 m.o \
 mbx.o \
 p.o \
 pv.o \
 px.o \
 r.o \
 rl.o \
 rt.o \
 s.o \
 sc.o \
 sl.o \
 sn.o \
 t.o \
 u.o \
 v.o \
 v0.o \
 v1.o \
 v2.o \
 va1.o \
 va1ss.o \
 va2.o \
 va2s.o \
 va2ss.o \
 vamultsp.o \
 vb.o \
 vbang.o \
 vbit.o \
 vcant.o \
 vchar.o \
 vcat.o \
 vcatsp.o \
 vcomp.o \
 vcompsc.o \
 vd.o \
 vdx.o \
 ve.o \
 vf.o \
 vfft.o \
 vfrom.o \
 vfromsp.o \
 vg.o \
 vgauss.o \
 vgcomp.o \
 vgranking.o \
 vgsort.o \
 vgsp.o \
 vi.o \
 viavx.o \
 viix.o \
 visp.o \
 vm.o \
 vo.o \
 vp.o \
 vq.o \
 vrand.o \
 vrep.o \
 vs.o \
 vsb.o \
 vt.o \
 vu.o \
 vx.o \
 vz.o \
 w.o \
 wc.o \
 wn.o \
 ws.o \
 x.o \
 x15.o \
 xa.o \
 xb.o \
 xc.o \
 xcrc.o \
 xd.o \
 xf.o \
 xfmt.o \
 xh.o \
 xi.o \
 xl.o \
 xo.o \
 xs.o \
 xt.o \
 xu.o "

export OBJS OBJS_FMA COMPILE LINK TARGET
export DLLOBJS LIBJDEF LIBJRES
$jmake/domakewin.sh $1

