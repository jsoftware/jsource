ifeq (1,$(JAVX512))
FAVX512= /DC_AVX2=1 /DC_AVX512=1 -DHAVE_AVX2=1 -DBASE64_AVX2_USE_ASM=1 -march=skylake-avx512 -mno-vzeroupper
else ifeq (1,$(JAVX2))
FAVX2= /DC_AVX2=1 -DHAVE_AVX2=1 -DBASE64_AVX2_USE_ASM=1 -march=skylake -mno-vzeroupper
endif
ifeq (1,$(USE_OPENMP))
FOPENMP= -Xclang -fopenmp
# LOPENMP= libomp.lib
ifeq (ARM64,$(TARGET_CPU))
LOPENMP= ../../openmp/obj/windows/libomparm64.lib
else ifeq (x64,$(TARGET_CPU))
LOPENMP= ../../openmp/obj/windows/libomp.lib
else
LOPENMP= ../../openmp/obj/windows/libomp32.lib
endif
endif
USE_SLEEF=1
ifeq (ARM64,$(TARGET_CPU))
FSLEEF= /DSLEEF=1 /DSLEEFQUAD=1 /DENABLE_ADVSIMD
else ifeq (x64,$(TARGET_CPU))
ifeq (1,$(JAVX512))
FSLEEF= /DSLEEF=1 /DSLEEFQUAD=1 /DENABLE_AVX2
else ifeq (1,$(JAVX2))
FSLEEF= /DSLEEF=1 /DSLEEFQUAD=1 /DENABLE_AVX2
else
FSLEEF= /DSLEEF=1 /DSLEEFQUAD=1 /DENABLE_SSE2
endif
else
FSLEEF= /DSLEEF=1 /DSLEEFQUAD=1 /DENABLE_SSE2
endif
ifeq (1,$(USE_PYXES))
PTHREADINC = -I../../pthreads4w/include
DPYXES = /DPYXES=1
ifeq (ARM64,$(TARGET_CPU))
PTHREADLIB = ../../pthreads4w/lib/pthreadVC3-arm64.lib
else ifeq (x64,$(TARGET_CPU))
PTHREADLIB = ../../pthreads4w/lib/pthreadVC3.lib
else
PTHREADLIB = ../../pthreads4w/lib/pthreadVC3-w32.lib
endif
else
PTHREADINC = 
DPYXES = /DPYXES=0
endif
MPIRINC = -I../../mpir/include

ifeq (ARM64,$(TARGET_CPU))
# MPIRLIB = ../../mpir/dll/arm64/mpir.lib
else ifeq (x64,$(TARGET_CPU))
# MPIRLIB = ../../mpir/dll/x64/mpir.lib
else
# MPIRLIB = ../../mpir/dll/x86/mpir.lib
endif

ifeq (1,$(NOSYNCHLIB))
FNOSYNCHLIB = /DNOSYNCHLIB=1
endif
ifneq ($(_DEBUG),)
FDEBUG = /FS /Zi /DDEBUG=1
LDEBUG = /DEBUG
endif
ifneq ($(_MEMAUDIT),)
FDEBUG := $(FDEBUG) /DMEMAUDIT=$(_MEMAUDIT)
endif
ifneq ($(_ASSERT),)
FDEBUG := $(FDEBUG) /D_ASSERT
endif
ifeq (1,$(USE_BOXEDSPARSE))
FBOXEDSPARSE = /DBOXEDSPARSE
endif

ifeq (ARM64,$(TARGET_CPU))
CF1= -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype
CFLAGS= $(FDEBUG) $(DPYXES) -target arm64-pc-windows-msvc -march=armv8-a+crc+crypto /DHAVE_NEON64=1 /D_JDLL /DC_NA=0 /DEMU_AVX2=1 $(FAVX2) $(FAVX512) $(FOPENMP) $(FSLEEF) $(FNOSYNCHLIB) $(FBOXEDSPARSE) $(PTHREADINC) $(MPIRINC) /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
FLAGS= $(LDEBUG) /DLL /MACHINE:ARM64 /SUBSYSTEM:WINDOWS,10.0
ASMFLAGS= /Cx
else ifeq (x64,$(TARGET_CPU))
CF1= -maes -msse3 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype
CFLAGS= $(FDEBUG) $(DPYXES) /D_JDLL /DC_NA=0 /DEMU_AVX2=1 $(FAVX2) $(FAVX512) $(FOPENMP) $(FSLEEF) $(FNOSYNCHLIB) $(FBOXEDSPARSE) $(PTHREADINC) $(MPIRINC) /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
FLAGS= $(LDEBUG) /DLL /MACHINE:X64 /SUBSYSTEM:WINDOWS,10.0
ASMFLAGS= /Cx
else
CF1= -m32 -maes -msse2 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype
# /Og fail g310r
# /Oy crash ]^:_"1[0
CFLAGS= $(FDEBUG) $(DPYXES) /D_JDLL $(FOPENMP) $(FSLEEF) $(FNOSYNCHLIB) $(FBOXEDSPARSE) $(PTHREADINC) $(MPIRINC) /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
FLAGS= $(LDEBUG) psapi.lib /DLL /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
ASMFLAGS= /Cx /coff
endif

.SUFFIXES: .o .S

OBJS=  \
 ../../jsrc/a.o \
 ../../jsrc/ab.o \
 ../../jsrc/aes-arm.o \
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
 ../../jsrc/cblas.o \
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
 ../../jsrc/jgmpinit.o \
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
 ../../jsrc/vgsortiqavx512.o \
 ../../jsrc/vgsp.o \
 ../../jsrc/vi.o \
 ../../jsrc/viavx.o \
 ../../jsrc/viavx1-2.o \
 ../../jsrc/viavx1-4.o \
 ../../jsrc/viavx2.o \
 ../../jsrc/viavx3.o \
 ../../jsrc/viavx4.o \
 ../../jsrc/viavx5.o \
 ../../jsrc/viavx6.o \
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
 ../../jsrc/str.o \
 ../../jsrc/strptime.o \
 ../../jsrc/mt.o \
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
 ../../base64/lib/arch/avx512/codec-avx512.o \
 ../../base64/lib/arch/generic/codec-generic.o \
 ../../base64/lib/arch/neon32/codec-neon32.o \
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
else ifeq (ARM64,$(TARGET_CPU))
OBJS_ASM= \
 ../../openssl-asm/md5-aarch64-nasm.o \
 ../../openssl-asm/keccak1600-armv8-nasm.o \
 ../../openssl-asm/sha1-armv8-nasm.o \
 ../../openssl-asm/sha256-armv8-nasm.o \
 ../../openssl-asm/sha512-armv8-nasm.o "
else ifeq (x64,$(TARGET_CPU))
OBJS_ASM= \
 ../../openssl-asm/md5-x86_64-nasm.o \
 ../../openssl-asm/keccak1600-x86_64-nasm.o \
 ../../openssl-asm/sha1-x86_64-nasm.o \
 ../../openssl-asm/sha256-x86_64-nasm.o \
 ../../openssl-asm/sha512-x86_64-nasm.o
else
OBJS_ASM= \
 ../../openssl-asm/md5-586-nasm.o \
 ../../openssl-asm/keccak1600-mmx-nasm.o \
 ../../openssl-asm/sha1-586-nasm.o \
 ../../openssl-asm/sha256-586-nasm.o \
 ../../openssl-asm/sha512-586-nasm.o
endif

ifeq (1,$(JAVX512))
OBJS_SIMDUTF8= \
 ../../jsrc/utf/utf8_to_utf16le_avx512.o \
 ../../jsrc/utf/utf16le_to_utf8_avx512.o
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

%.o: %.S
	$(CC) $(CFLAGS) /Fo$@ /c $<

%.o: %.asm
	$(ML) $(ASMFLAGS) /Fo$@ /c $<

ifeq (ARM64,$(TARGET_CPU))
TARGET= jarm64.dll
else ifeq (x64,$(TARGET_CPU))
ifeq (1,$(JAVX512))
TARGET= javx512.dll
else ifeq (1,$(JAVX2))
TARGET= javx2.dll
else
TARGET= j.dll
endif
else
TARGET= j32.dll
endif

ifeq (1,$(NOSYNCHLIB))
SYNCLIB=
else
SYNCLIB= synchronization.lib
endif

$(TARGET) : $(OBJS) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SIMDUTF8) $(ASM) $(PTHREADLIB) jdll.res
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SIMDUTF8) $(ASM) $(PTHREADLIB) $(MPIRLIB) jdll.res ole32.lib advapi32.lib oleaut32.lib onecore.lib $(SYNCLIB) $(LOPENMP)

jdll.res : $(RCS)
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

noclean:
	#

clean:
	rm -f *.lib *.exp *.ilk *.pdb $(OBJS) $(OBJS_SIMDUTF8) $(SRC_ASM)

.PHONY: all clean noclean
