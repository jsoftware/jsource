ifeq (1,${JAVX2})
FAVX2= /DC_AVX=1 /DC_AVX2=1 -DHAVE_AVX2=1 -march=haswell
else
ifeq (1,${JAVX})
FAVX= /DC_AVX=1 /arch:AVX -DHAVE_SSSE3=1 -DHAVE_AVX=1
endif
endif
ifeq (1,${USE_OPENMP})
FOPENMP= -Xclang -fopenmp
ifeq (1,${x64})
OBJS_OPENMP=../../openmp/obj/windows/libomp.lib
else
OBJS_OPENMP=../../openmp/obj/windows/libomp32.lib
endif
endif
USE_SLEEF=1
ifeq (1,${x64})
ifeq (1,${JAVX2})
FSLEEF= /DSLEEF=1 /DENABLE_AVX2 /DSLEEF_STATIC_LIBS /DDORENAME=1 -I../../sleef/src/common -I../../sleef/src/arch
else
ifeq (1,${JAVX})
FSLEEF= /DSLEEF=1 /DENABLE_AVX /DSLEEF_STATIC_LIBS /DDORENAME=1 -I../../sleef/src/common -I../../sleef/src/arch
else
FSLEEF= /DSLEEF=1 /DENABLE_SSE2 /DSLEEF_STATIC_LIBS /DDORENAME=1 -I../../sleef/src/common -I../../sleef/src/arch
endif
endif
else
FSLEEF= /DSLEEF=1 /DENABLE_SSE2 /DSLEEF_STATIC_LIBS /DDORENAME=1 -I../../sleef/src/common -I../../sleef/src/arch
endif
ifeq (1,${USE_PYXES})
PTHREADINC = -I../../pthreads4w/include -DPYXES=1
ifeq (1,${x64})
PTHREADLIB = ../../pthreads4w/lib/pthreadVC3.lib
else
PTHREADLIB = ../../pthreads4w/lib/pthreadVC3-w32.lib
endif
endif
ifeq (1,${DEBUG})
FDEBUG = /FS /Zi /DDEBUG=1
LDEBUG = /DEBUG
endif
ifeq (1,${USE_BOXEDSPARSE})
FBOXEDSPARSE = /DBOXEDSPARSE
endif

ifeq (1,${x64})
CF1= -maes -msse3 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
CFLAGS= ${FDEBUG} /D_JDLL /DC_NA=0 /DEMU_AVX=1 ${FAVX} ${FAVX2} ${FOPENMP} ${FSLEEF} ${FBOXEDSPARSE} ${PTHREADINC} /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT ${CF1}
FLAGS= ${LDEBUG} /DLL /MACHINE:X64 /SUBSYSTEM:WINDOWS,5.02
ASMFLAGS= /Cx
else
CF1= -m32 -maes -msse2 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
# /Og fail g310r
# /Oy crash ]^:_"1[0
CFLAGS= ${FDEBUG} /D_JDLL ${FOPENMP} ${FSLEEF} ${FBOXEDSPARSE} ${PTHREADINC} /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT ${CF1}
FLAGS= ${LDEBUG} /DLL /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
ASMFLAGS= /Cx /coff
endif

.SUFFIXES: .o

OBJS=  \
 ../../jsrc/a.o \
 ../../jsrc/ab.o \
 ../../jsrc/aes-c.o \
 ../../jsrc/aes-ni.o \
 ../../jsrc/aes-sse2.o \
 ../../jsrc/af.o \
 ../../jsrc/ai.o \
 ../../jsrc/am.o \
 ../../jsrc/am1.o \
 ../../jsrc/amn.o \
 ../../jsrc/ao.o \
 ../../jsrc/ap.o \
 ../../jsrc/ar.o \
 ../../jsrc/as.o \
 ../../jsrc/au.o \
 ../../jsrc/blis/gemm_c-ref.o \
 ../../jsrc/blis/gemm_int-aarch64.o \
 ../../jsrc/blis/gemm_int-avx.o \
 ../../jsrc/blis/gemm_int-fma.o \
 ../../jsrc/blis/gemm_int-sse2.o \
 ../../jsrc/blis/gemm_vec-ref.o \
 ../../jsrc/c.o \
 ../../jsrc/ca.o \
 ../../jsrc/cc.o \
 ../../jsrc/cd.o \
 ../../jsrc/cf.o \
 ../../jsrc/cg.o \
 ../../jsrc/ch.o \
 ../../jsrc/cip.o \
 ../../jsrc/cl.o \
 ../../jsrc/cp.o \
 ../../jsrc/cpdtsp.o \
 ../../jsrc/cpuinfo.o \
 ../../jsrc/cr.o \
 ../../jsrc/crs.o \
 ../../jsrc/ct.o \
 ../../jsrc/cu.o \
 ../../jsrc/cv.o \
 ../../jsrc/cx.o \
 ../../jsrc/d.o \
 ../../jsrc/dc.o \
 ../../jsrc/dss.o \
 ../../jsrc/dstop.o \
 ../../jsrc/dsusp.o \
 ../../jsrc/dtoa.o \
 ../../jsrc/f.o \
 ../../jsrc/f2.o \
 ../../jsrc/fbu.o \
 ../../jsrc/gemm.o \
 ../../jsrc/i.o \
 ../../jsrc/io.o \
 ../../jsrc/j.o \
 ../../dllsrc/jdll.o \
 ../../dllsrc/jdllcomx.o \
 ../../jsrc/jdlllic.o \
 ../../jsrc/k.o \
 ../../jsrc/m.o \
 ../../jsrc/mbx.o \
 ../../jsrc/p.o \
 ../../jsrc/pv.o \
 ../../jsrc/px.o \
 ../../jsrc/r.o \
 ../../jsrc/rl.o \
 ../../jsrc/rt.o \
 ../../jsrc/s.o \
 ../../jsrc/sc.o \
 ../../jsrc/sl.o \
 ../../jsrc/sn.o \
 ../../jsrc/t.o \
 ../../jsrc/u.o \
 ../../jsrc/v.o \
 ../../jsrc/v0.o \
 ../../jsrc/v1.o \
 ../../jsrc/v2.o \
 ../../jsrc/va1.o \
 ../../jsrc/va1ss.o \
 ../../jsrc/va2.o \
 ../../jsrc/va2s.o \
 ../../jsrc/va2ss.o \
 ../../jsrc/vamultsp.o \
 ../../jsrc/vb.o \
 ../../jsrc/vbang.o \
 ../../jsrc/vbit.o \
 ../../jsrc/vcant.o \
 ../../jsrc/vcat.o \
 ../../jsrc/vcatsp.o \
 ../../jsrc/vchar.o \
 ../../jsrc/vcomp.o \
 ../../jsrc/vcompsc.o \
 ../../jsrc/vd.o \
 ../../jsrc/vdx.o \
 ../../jsrc/ve.o \
 ../../jsrc/vf.o \
 ../../jsrc/vfft.o \
 ../../jsrc/vfrom.o \
 ../../jsrc/vfromsp.o \
 ../../jsrc/vg.o \
 ../../jsrc/vgauss.o \
 ../../jsrc/vgcomp.o \
 ../../jsrc/vgranking.o \
 ../../jsrc/vgsort.o \
 ../../jsrc/vgsp.o \
 ../../jsrc/vi.o \
 ../../jsrc/viavx.o \
 ../../jsrc/viix.o \
 ../../jsrc/visp.o \
 ../../jsrc/vm.o \
 ../../jsrc/vo.o \
 ../../jsrc/vp.o \
 ../../jsrc/vq.o \
 ../../jsrc/vrand.o \
 ../../jsrc/vrep.o \
 ../../jsrc/vs.o \
 ../../jsrc/vsb.o \
 ../../jsrc/vt.o \
 ../../jsrc/vu.o \
 ../../jsrc/vx.o \
 ../../jsrc/vz.o \
 ../../jsrc/w.o \
 ../../jsrc/wc.o \
 ../../jsrc/wn.o \
 ../../jsrc/ws.o \
 ../../jsrc/x.o \
 ../../jsrc/x15.o \
 ../../jsrc/xa.o \
 ../../jsrc/xaes.o \
 ../../jsrc/xb.o \
 ../../jsrc/xc.o \
 ../../jsrc/xcrc.o \
 ../../jsrc/xd.o \
 ../../jsrc/xf.o \
 ../../jsrc/xfmt.o \
 ../../jsrc/xh.o \
 ../../jsrc/xi.o \
 ../../jsrc/xl.o \
 ../../jsrc/xo.o \
 ../../jsrc/xs.o \
 ../../jsrc/xsha.o \
 ../../jsrc/xt.o \
 ../../jsrc/xu.o \
 ../../jsrc/crc32c.o \
 ../../jsrc/openssl/sha/keccak1600.o \
 ../../jsrc/openssl/sha/md4_dgst.o \
 ../../jsrc/openssl/sha/md4_one.o \
 ../../jsrc/openssl/sha/md5_dgst.o \
 ../../jsrc/openssl/sha/md5_one.o \
 ../../jsrc/openssl/sha/openssl-util.o \
 ../../jsrc/openssl/sha/sha1_one.o \
 ../../jsrc/openssl/sha/sha256.o \
 ../../jsrc/openssl/sha/sha3.o \
 ../../jsrc/openssl/sha/sha512.o \
 ../../base64/lib/arch/avx2/codec-avx2.o \
 ../../base64/lib/arch/generic/codec-generic.o \
 ../../base64/lib/arch/neon64/codec-neon64.o \
 ../../base64/lib/arch/ssse3/codec-ssse3.o \
 ../../base64/lib/arch/sse41/codec-sse41.o \
 ../../base64/lib/arch/sse42/codec-sse42.o \
 ../../base64/lib/arch/avx/codec-avx.o \
 ../../base64/lib/lib.o \
 ../../base64/lib/codec_choose.o \
 ../../base64/lib/tables/tables.o

ifdef NO_SHA_ASM
CFLAGS1= /DNO_SHA_ASM
else
ifeq (1,${x64})
OBJS_ASM= \
 ../../openssl-asm/keccak1600-x86_64-nasm.o \
 ../../openssl-asm/sha1-x86_64-nasm.o \
 ../../openssl-asm/sha256-x86_64-nasm.o \
 ../../openssl-asm/sha512-x86_64-nasm.o
else
OBJS_ASM= \
 ../../openssl-asm/keccak1600-mmx-nasm.o \
 ../../openssl-asm/sha1-586-nasm.o \
 ../../openssl-asm/sha256-586-nasm.o \
 ../../openssl-asm/sha512-586-nasm.o
endif
endif

ifeq (1,${USE_SLEEF})
OBJS_SLEEF= \
 ../../sleef/src/common/arraymap.o \
 ../../sleef/src/common/common.o \
 ../../sleef/src/libm/rempitab.o \
 ../../sleef/src/libm/sleefsimddp.o
endif

DEF= ../../dllsrc/jdll.def
RCS= ../../dllsrc/jdll.rc

ifndef LD
LD=lld-link
endif

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS1) /Fo$@ /c $<

%.o: %.cpp
	$(CC) $(CFLAGS) /Fo$@ /c $<

%.o: %.asm
	$(ML) $(ASMFLAGS) /Fo$@ /c $<

ifeq (1,${x64})
ifeq (1,${JAVX2})
TARGET= javx2.dll
else
ifeq (1,${JAVX})
TARGET= javx.dll
else
TARGET= j.dll
endif
endif
else
TARGET= j32.dll
endif

$(TARGET) : $(OBJS) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SLEEF) $(OBJS_OPENMP) $(ASM) $(PTHREADLIB) jdll.res
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SLEEF) $(OBJS_OPENMP) $(ASM) $(PTHREADLIB) jdll.res ole32.lib advapi32.lib oleaut32.lib

jdll.res : $(RCS)
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.lib *.exp *.res *.ilk *.pdb $(OBJS) $(OBJS_SLEEF) $(SRC_ASM)

.PHONY: all clean
