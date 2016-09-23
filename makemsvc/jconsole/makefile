ifdef x64
CFLAGS= /WX /W3 /O2 /Ot /Ob1 /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT /D NDEBUG /D WIN32 /D _CONSOLE
FLAGS= /NOLOGO /STACK:10000000 /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
else
CFLAGS= /WX /W3 /O2 /Ot /Ob1 /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT /D NDEBUG /D WIN32 /D _CONSOLE
FLAGS= /NOLOGO /STACK:10000000 /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
endif

.SUFFIXES: .o

OBJS=  \
 ../../jsrc/jconsole.o \
 ../../jsrc/jeload.o

RCS= ../../makevs/jconsole/jconsole.rc

ifndef LD
LD=link
endif

%.o: %.c
	$(CC) $(CFLAGS) /Fo$@ /c $<

ifdef x64
TARGET= jconsole.exe
else
TARGET= jconsole32.exe
endif

$(TARGET) : $(OBJS) jconsole.res
	$(LD) /NOLOGO $(FLAGS) /out:$@ $(OBJS) jconsole.res

jconsole.res : $(RCS)
	$(RC) /L 0x409 /D NDEBUG /D _USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.res $(OBJS)

.PHONY: all clean
