ODIR = ../bin/$(jplatform)/$(j64x)

COPTION =
ifeq (j64arm,$(j64x))
 TARGET_CPU = arm64
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
else ifeq (j64avx512,$(j64x))
 TARGET_CPU = x64
 COPTION := $(COPTION) /DC_AVX2=1 /DC_AVX512=1 -DHAVE_AVX2=1 -march=skylake-avx512 -mno-vzeroupper
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
else ifeq (j64avx2,$(j64x))
 TARGET_CPU = x64
 COPTION := $(COPTION) /DC_AVX2=1 -DHAVE_AVX2=1 -march=skylake -mno-vzeroupper
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
else ifeq (j64,$(j64x))
 TARGET_CPU = x64
 COPTION := $(COPTION) /DSLEEF=0 /DSLEEFQUAD=1
else
 TARGET_CPU = x86
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
endif

ifneq ($(_DEBUG),)
 COPTION := $(COPTION) /FS /Z7 /DDEBUG=1
 LDEBUG = /DEBUG
endif
CINCLUDE = -I../mpir/include
ifeq (1,$(USE_PYXES))
 ifeq (arm64,$(TARGET_CPU))
  PTHREADLIB = ../pthreads4w/lib/pthreadVC3-arm64.lib
 else ifeq (x64,$(TARGET_CPU))
  PTHREADLIB = ../pthreads4w/lib/pthreadVC3.lib
 else
  PTHREADLIB = ../pthreads4w/lib/pthreadVC3-w32.lib
 endif
 CINCLUDE := $(CINCLUDE) -I../pthreads4w/include
 COPTION := $(COPTION) /DPYXES=1
else
 COPTION := $(COPTION) /DPYXES=0
endif
ifeq (0,$(USE_EMU_AVX))
 COPTION := $(COPTION) /DEMU_AVX2=0
else
 COPTION := $(COPTION) /DEMU_AVX2=1
endif
ifeq (1,$(USE_NORMAH8))
 COPTION := $(COPTION) /DNORMAH8=1
else
 COPTION := $(COPTION) /DNORMAH8=0
endif

ifeq (1,$(USE_OPENMP))
 COPTION := $(COPTION) -Xclang -fopenmp
# LOPENMP = libomp.lib
 ifeq (arm64,$(TARGET_CPU))
  LOPENMP = ../openmp/obj/windows/libomparm64.lib
 else ifeq (x64,$(TARGET_CPU))
  LOPENMP = ../openmp/obj/windows/libomp.lib
 else
  LOPENMP = ../openmp/obj/windows/libomp32.lib
 endif
endif

ifeq (arm64,$(TARGET_CPU))
# MPIRLIB = ../mpir/dll/arm64/mpir.lib
else ifeq (x64,$(TARGET_CPU))
# MPIRLIB = ../mpir/dll/x64/mpir.lib
else
# MPIRLIB = ../mpir/dll/x86/mpir.lib
endif

ifeq (1,$(NOSYNCHLIB))
 COPTION := $(COPTION) /DNOSYNCHLIB=1
endif
ifneq ($(_MEMAUDIT),)
 COPTION := $(COPTION) /DMEMAUDIT=$(_MEMAUDIT)
endif
ifneq ($(_ASSERT),)
 COPTION := $(COPTION) /D_ASSERT
endif
ifneq ($(_ASSERT2),)
 COPTION := $(COPTION) /D_ASSERT2
endif
ifneq ($(_NAN),)
 COPTION := $(COPTION) /D_NAN
endif
ifneq ($(_NAMETRACK),)
 COPTION := $(COPTION) /DNAMETRACK=$(_NAMETRACK)
endif
ifeq (1,$(USE_BOXEDSPARSE))
 COPTION := $(COPTION) /DBOXEDSPARSE
endif
ifeq (x64,$(TARGET_CPU))
 ifneq ($(_SSE4_2),)
  COPTION := $(COPTION) -msse4.1 -msse4.2
 endif
endif

ifeq (arm64,$(TARGET_CPU))
 CF1 = -target arm64-pc-windows-msvc -march=armv8-a+crc+crypto -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype -Wno-unused-variable
 CFLAGS = $(COPTION) $(CINCLUDE) /DHAVE_NEON64=1 /D_JDLL /DC_NA=0 /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /DLL /MACHINE:ARM64 /SUBSYSTEM:WINDOWS,10.0
 ASMFLAGS = /Cx
else ifeq (x64,$(TARGET_CPU))
 CF1 = -maes -msse3 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype -Wno-unused-variable
 CFLAGS = $(COPTION) $(CINCLUDE) /D_JDLL /DC_NA=0 /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /DLL /MACHINE:X64 /SUBSYSTEM:WINDOWS,10.0
 ASMFLAGS = /Cx
else
 CF1 = -m32 -maes -msse2 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype -Wno-unused-variable
# /Og fail g310r
# /Oy crash ]^:_"1[0
CFLAGS = $(COPTION) $(CINCLUDE) /D_JDLL /WX /W3 /O2 /GS- /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
FLAGS = $(LDEBUG) psapi.lib /DLL /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
 ASMFLAGS = /Cx /coff
endif

.SUFFIXES: .o .S

OBJS = \
 a.o \
 ab.o \
 aes-arm.o \
 aes-c.o \
 aes-ni.o \
 aes-sse2.o \
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
 blis/gemm_int-fma.o \
 blis/gemm_int-sse2.o \
 blis/gemm_vec-ref.o \
 c.o \
 ca.o \
 cblas.o \
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
 crc32c.o \
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
 jgmpinit.o \
 k.o \
 m.o \
 mbx.o \
 mt.o \
 openssl/sha/keccak1600.o \
 openssl/sha/md4_dgst.o \
 openssl/sha/md4_one.o \
 openssl/sha/md5_dgst.o \
 openssl/sha/md5_one.o \
 openssl/sha/openssl-util.o \
 openssl/sha/sha1_one.o \
 openssl/sha/sha256.o \
 openssl/sha/sha3.o \
 openssl/sha/sha512.o \
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
 str.o \
 strptime.o \
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
 vcat.o \
 vcatsp.o \
 vchar.o \
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
 vgsortiqavx512.o \
 vgsp.o \
 vi.o \
 viavx.o \
 viavx1-2.o \
 viavx1-4.o \
 viavx2.o \
 viavx3.o \
 viavx4.o \
 viavx5.o \
 viavx6.o \
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
 xaes.o \
 xb.o \
 xc.o \
 xcrc.o \
 xd.o \
 xdic.o \
 xf.o \
 xfmt.o \
 xh.o \
 xi.o \
 xl.o \
 xlp.o \
 xo.o \
 xs.o \
 xsha.o \
 xt.o \
 xu.o \
 ../base64/lib/arch/avx/codec.o \
 ../base64/lib/arch/avx2/codec.o \
 ../base64/lib/arch/avx512/codec.o \
 ../base64/lib/arch/generic/codec.o \
 ../base64/lib/arch/neon32/codec.o \
 ../base64/lib/arch/neon64/codec.o \
 ../base64/lib/arch/sse41/codec.o \
 ../base64/lib/arch/sse42/codec.o \
 ../base64/lib/arch/ssse3/codec.o \
 ../base64/lib/codec_choose.o \
 ../base64/lib/lib.o \
 ../base64/lib/tables/tables.o \
 ../dllsrc/jdll.o \
 ../dllsrc/jdllcomx.o

ifdef NO_SHA_ASM
 CFLAGS1 = /DNO_SHA_ASM
else ifeq (arm64,$(TARGET_CPU))
 OBJS_ASM = \
 ../openssl-asm/md5-aarch64-nasm.o \
 ../openssl-asm/keccak1600-armv8-nasm.o \
 ../openssl-asm/sha1-armv8-nasm.o \
 ../openssl-asm/sha256-armv8-nasm.o \
 ../openssl-asm/sha512-armv8-nasm.o
else ifeq (x64,$(TARGET_CPU))
 OBJS_ASM = \
 ../openssl-asm/md5-x86_64-nasm.o \
 ../openssl-asm/keccak1600-x86_64-nasm.o \
 ../openssl-asm/sha1-x86_64-nasm.o \
 ../openssl-asm/sha256-x86_64-nasm.o \
 ../openssl-asm/sha512-x86_64-nasm.o
else
 OBJS_ASM = \
 ../openssl-asm/md5-586-nasm.o \
 ../openssl-asm/keccak1600-mmx-nasm.o \
 ../openssl-asm/sha1-586-nasm.o \
 ../openssl-asm/sha256-586-nasm.o \
 ../openssl-asm/sha512-586-nasm.o
endif

ifeq (j64avx512,$(j64x))
 OBJS_SIMDUTF8= \
 utf/utf8_to_utf16le_avx512.o \
 utf/utf16le_to_utf8_avx512.o
endif

DEF = ../dllsrc/jdll.def
RCS = ../dllsrc/jdll.res

ifndef LD
 LD = lld-link
endif

%.o : %.c
	$(CC) $(CFLAGS) $(CFLAGS1) /Fo$@ /c $<

%.o : %.cpp
	$(CC) $(CFLAGS) /Fo$@ /c $<

%.o : %.S
	$(CC) $(CFLAGS) /Fo$@ /c $<

%.o : %.asm
	$(ML) $(ASMFLAGS) /Fo$@ /c $<

%.res : %.rc
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $<

TARGET = j.dll

ifeq (1,$(NOSYNCHLIB))
 SYNCLIB =
else
 SYNCLIB = synchronization.lib
endif

$(ODIR)/$(TARGET) : $(OBJS) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SIMDUTF8) $(ASM) $(PTHREADLIB) $(RCS)
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SIMDUTF8) $(ASM) $(PTHREADLIB) $(RCS) $(MPIRLIB) ole32.lib advapi32.lib oleaut32.lib onecore.lib $(SYNCLIB) $(LOPENMP)

all : $(ODIR)/$(TARGET)

noclean :
	#

clean :
	rm -f $(ODIR)/*.lib $(ODIR)/*.exp $(ODIR)/*.ilk $(ODIR)/*.pdb $(OBJS) $(OBJS_SIMDUTF8) $(SRC_ASM) $(RCS)

.PHONY: all clean noclean
