ifdef x64
CFLAGS= /D _JDLL /DC_NA=0 /WX /W3 /O2 /Ot /Ob1 /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT /D NDEBUG /D WIN32 /D _WINDOWS
FLAGS= /DLL /NOLOGO /MACHINE:X64 /SUBSYSTEM:WINDOWS,5.02
else
CFLAGS= /D _JDLL /WX /W3 /O2 /Ot /Ob1 /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS /MT /D NDEBUG /D WIN32 /D _WINDOWS
FLAGS= /DLL /NOLOGO /MACHINE:X86 /SUBSYSTEM:WINDOWS,5.01
endif

.SUFFIXES: .o

OBJS=  \
 ../../jsrc/a.o \
 ../../jsrc/ab.o \
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
 ../../jsrc/i.o \
 ../../jsrc/io.o \
 ../../jsrc/j.o \
 ../../dllsrc/jdll.o \
 ../../dllsrc/jdllcomx.o \
 ../../jsrc/jdlllic.o \
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
 ../../jsrc/vgsp.o \
 ../../jsrc/vi.o \
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
 ../../jsrc/xt.o \
 ../../jsrc/xu.o

DEF= ../../dllsrc/jdll.def
RCS= ../../dllsrc/jdll.rc
ifdef x64
ASM= ../../makevs/jdll/winasm64.obj 
else
ASM=
endif

ifndef LD
LD=link
endif

%.o: %.c
	$(CC) $(CFLAGS) /Fo$@ /c $<

%.o: %.cpp
	$(CC) $(CFLAGS) /Fo$@ /c $<

ifdef x64
TARGET= j.dll
else
TARGET= j32.dll
endif

$(TARGET) : $(OBJS) jdll.res
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS) $(ASM) jdll.res ole32.lib advapi32.lib oleaut32.lib

jdll.res : $(RCS)
	$(RC) /L 0x409 /D NDEBUG /D _USING_V110_SDK71_ -R -FO$@ $(RCS)

all: $(TARGET)

clean:
	rm -f *.lib *.exp *.res $(OBJS)

.PHONY: all clean
