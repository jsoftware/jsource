ifeq (1,$(_DEBUG))
 COPTION = /FS /Zi /DDEBUG=1
 LDEBUG = /DEBUG
else
 COPTION =
endif
ifeq (ARM64,$(TARGET_CPU))
 CF1 = -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
 CFLAGS = $(COPTION) -target arm64-pc-windows-msvc -march=armv8-a+crc /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /DLL /MACHINE:ARM64 /SUBSYSTEM:WINDOWS,5.02
else ifeq (x64,$(TARGET_CPU))
 CF1 = -maes -msse3 -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
 CFLAGS = $(COPTION) /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /DLL /MACHINE:X64 /SUBSYSTEM:WINDOWS,5.02
else
 CF1 = -m32 -msse2 -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
 CFLAGS = $(COPTION) /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /DLL /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
endif

.SUFFIXES: .o

OBJS = ../../jsrc/tsdll.o

DEF = ../../makevs/tsdll/tsdll.def

ifndef LD
 LD = lld-link
endif

%.o : %.c
	$(CC) $(CFLAGS) /Fo$@ /c $<

ifeq (ARM64,$(TARGET_CPU))
 TARGET = tsdllarm64.dll
else ifeq (x64,$(TARGET_CPU))
 TARGET = tsdll.dll
else
 TARGET = tsdll32.dll
endif

$(TARGET) : $(OBJS)
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS)

jddll.res : $(RCS)
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $(RCS)

all : $(TARGET)

noclean :
	#

clean :
	rm -f *.lib *.exp *.res $(OBJS)

.PHONY: all clean noclean
