ifdef DEBUG

ifdef x64
CFLAGS= /FS /Zi /WX /W3 /O2 /GS /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT
FLAGS= /DEBUG /NOLOGO /STACK:0x10000000 /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
else
CFLAGS= /FS /Zi /WX /W3 /Ot /Oy- /GS /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT
FLAGS= /DEBUG /NOLOGO /STACK:0x10000000 /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
endif

else

ifdef x64
CFLAGS= /WX /W3 /O2 /GS /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT
FLAGS= /NOLOGO /STACK:0x10000000 /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
else
CFLAGS= /WX /W3 /Ot /Oy- /GS /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT
FLAGS= /NOLOGO /STACK:0x10000000 /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
endif

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
LD=link
endif

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGSLN) /Fo$@ /c $<

ifdef x64
TARGET= jconsole.exe
else
TARGET= jconsole32.exe
endif

$(TARGET) : $(OBJS) $(OBJSLN) jconsole.res
	$(LD) /NOLOGO $(FLAGS) /out:$@ $(OBJS) $(OBJSLN) jconsole.res

jconsole.res : $(RCS)
	$(RC) /L 0x409 /D _USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.res *.ilk *.pdb $(OBJS) $(OBJSLN)

.PHONY: all clean
