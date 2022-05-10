# READLINE=1
ifeq (1,${USE_PYXES})
PTHREADINC = -I../../pthreads4w/include -DPYXES=1
endif
ifeq (1,${DEBUG})
FDEBUG = /FS /Zi /DDEBUG=1
LDEBUG = /DEBUG
endif

ifeq (1,${x64})
CF1= -maes -msse3 -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
CFLAGS= ${PTHREADINC} ${FDEBUG} /DEMU_AVX=1 /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT ${CF1}
FLAGS= ${LDEBUG} /STACK:0x10000000 /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
else
CF1= -m32 -msse2 -fno-strict-aliasing -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-char-subscripts -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor
CFLAGS= ${PTHREADINC} ${FDEBUG} /WX /W3 /O2 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT ${CF1}
FLAGS= ${LDEBUG} /STACK:0x10000000 /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
endif

.SUFFIXES: .o

OBJS=  \
 ../../jsrc/jconsole.o \
 ../../jsrc/jeload.o

ifdef READLINE
OBJSLN=  ../../jsrc/linenoise.o
CFLAGSLN= -DREADLINE -DUSE_LINENOISE
endif

RCS= ../../makevs/jconsole/jconsole.rc

ifndef LD
LD=lld-link
endif

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGSLN) /Fo$@ /c $<

ifeq (1,${x64})
TARGET= jconsole.exe
else
TARGET= jconsole32.exe
endif

$(TARGET) : $(OBJS) $(OBJSLN) jconsole.res
	$(LD) /NOLOGO $(FLAGS) /out:$@ $(OBJS) $(OBJSLN) jconsole.res

jconsole.res : $(RCS)
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.res *.ilk *.pdb $(OBJS) $(OBJSLN)

.PHONY: all clean
