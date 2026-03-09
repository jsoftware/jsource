ODIR = ../bin/$(jplatform)/$(j64x)

COPTION =
ifeq (j64arm,$(j64x))
 TARGET_CPU = arm64
else ifeq (j64avx512,$(j64x))
 TARGET_CPU = x64
else ifeq (j64avx2,$(j64x))
 TARGET_CPU = x64
else ifeq (j64,$(j64x))
 TARGET_CPU = x64
else
 TARGET_CPU = x86
endif

ifeq (1,$(USE_LINENOISE))
 READLINE = 1
endif
ifneq ($(_DEBUG),)
 COPTION := $(COPTION) /FS /Z7 /DDEBUG=1
 LDEBUG = /DEBUG
endif
CINCLUDE = -I../mpir/include
ifeq (1,$(USE_PYXES))
 CINCLUDE := $(CINCLUDE) -I../pthreads4w/include
 COPTION := $(COPTION) /DPYXES=1
else
 COPTION := $(COPTION) /DPYXES=0
endif

ifeq (arm64,$(TARGET_CPU))
 CF1 = -target arm64-pc-windows-msvc -march=armv8-a+crc+crypto -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-unused-variable
 CFLAGS = $(COPTION) $(CINCLUDE) /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /STACK:0xc00000 /MACHINE:ARM64 /SUBSYSTEM:CONSOLE,5.02
else ifeq (x64,$(TARGET_CPU))
 CF1 = -maes -msse3 -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-unused-variable
 CFLAGS = $(COPTION) $(CINCLUDE) /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /STACK:0xc00000 /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
else
 CF1 = -m32 -msse2 -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-unused-variable
 CFLAGS = $(COPTION) $(CINCLUDE) /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /STACK:0xc00000 /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
endif

.SUFFIXES: .o

OBJS = \
 jconsole.o \
 jeload.o

ifdef READLINE
 OBJSLN =  linenoise.o
 CFLAGSLN = -DREADLINE -DUSE_LINENOISE
endif

RCS = ../makevs/jconsole/jconsole.res

ifndef LD
 LD = lld-link
endif

%.o : %.c
	$(CC) $(CFLAGS) $(CFLAGSLN) /Fo$@ /c $<

%.res : %.rc
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $<

TARGET = jconsole.exe

$(ODIR)/$(TARGET) : $(OBJS) $(OBJSLN) $(RCS)
	$(LD) /NOLOGO $(FLAGS) /out:$@ $(OBJS) $(OBJSLN) $(RCS)

all : $(ODIR)/$(TARGET)

noclean :
	#

clean :
	rm -f $(ODIR)/*.ilk $(ODIR)/*.pdb $(OBJS) $(OBJSLN) $(RCS)

.PHONY: all clean noclean
