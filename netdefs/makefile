ifeq (ARM64,$(TARGET_CPU))
CFLAGS= -target arm64-pc-windows-msvc -Wno-format /W1 /MT
FLAGS= /MACHINE:ARM64 /SUBSYSTEM:CONSOLE,5.02
else ifeq (x64,$(TARGET_CPU))
CFLAGS= /W1 /MT
FLAGS= /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
else
CFLAGS= -m32 /W1 /MT
FLAGS= /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
endif

.SUFFIXES: .o

OBJS=  netdefs.o

ifndef LD
LD=lld-link
endif

%.o: %.c
	$(CC) $(CFLAGS) /Fo$@ /c $<

ifeq (ARM64,$(TARGET_CPU))
TARGET= netdefs-arm64.exe
else ifeq (x64,$(TARGET_CPU))
TARGET= netdefs.exe
else
TARGET= netdefs32.exe
endif

$(TARGET) : $(OBJS)
	$(LD) /NOLOGO $(FLAGS) /out:$@ $(OBJS)

all: $(TARGET)

clean:
	rm -f *.res *.ilk *.pdb $(OBJS)

.PHONY: all clean
