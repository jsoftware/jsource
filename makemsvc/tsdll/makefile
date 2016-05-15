ifdef x64
CFLAGS= /DC_64=1 /Oi /Ot /Oy /Ob2 /Gs0 /GF /Gy /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT /D _UNICODE /D_USRDLL /D NDEBUG /D WIN32 /D _WINDOWS
FLAGS= /DLL /NOLOGO /MACHINE:X64 /SUBSYSTEM:WINDOWS,5.02
else
CFLAGS= /DC_64=0 /Gz /Oi /Ot /Oy /Ob2 /Gs0 /GF /Gy /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT /D _UNICODE /D_USRDLL /D NDEBUG /D WIN32 /D _WINDOWS
FLAGS= /DLL /NOLOGO /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
endif

.SUFFIXES: .o

OBJS=  ../../jsrc/tsdll.o

ifndef LD
LD=link
endif

%.o: %.c
	$(CC) $(CFLAGS) /Fo$@ /c $<

ifdef x64
TARGET= tsdll.dll
else
TARGET= tsdll32.dll
endif

$(TARGET) : $(OBJS)
	$(LD) $(FLAGS) /out:$@ $(OBJS)

jddll.res : $(RCS)
	$(RC) /L 0x409 /D NDEBUG /D _USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.lib *.exp *.res $(OBJS)

.PHONY: all clean
