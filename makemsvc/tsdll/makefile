ifeq (1,${x64})
CFLAGS= /WX /W3 /Ot /Ob1 /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT
FLAGS= /DLL /MACHINE:X64 /SUBSYSTEM:WINDOWS,5.02
else
CFLAGS= /WX /W3 /Ot /Ob1 /Oy- /GS /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT
FLAGS= /DLL /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
endif

.SUFFIXES: .o

OBJS=  ../../jsrc/tsdll.o

DEF= ../../makevs/tsdll/tsdll.def

ifndef LD
LD=link
endif

%.o: %.c
	$(CC) $(CFLAGS) /Fo$@ /c $<

ifeq (1,${x64})
TARGET= tsdll.dll
else
TARGET= tsdll32.dll
endif

$(TARGET) : $(OBJS)
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS)

jddll.res : $(RCS)
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.lib *.exp *.res $(OBJS)

.PHONY: all clean
