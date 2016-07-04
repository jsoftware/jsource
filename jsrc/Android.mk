
LOCAL_PATH := $(call my-dir)

LOCAL_MODULE    := j
LOCAL_MODULE_FILENAME    := libj

ifeq ($(TARGET_ARCH),arm)
  ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS := -fPIC -O1 -Werror -fno-strict-aliasing -fsigned-char -DC_64=0 -DSY_GETTOD -DC_CD_ARMEL -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a
    LOCAL_ARM_MODE := arm
  else
    LOCAL_CFLAGS := -fPIC -O1 -Werror -fno-strict-aliasing -fsigned-char -DC_64=0 -DSY_GETTOD -DC_CD_ARMEL -mfloat-abi=softfp -mfpu=vfp -march=armv6
    LOCAL_ARM_MODE := arm
  endif
endif
ifeq ($(TARGET_ARCH),x86)
  LOCAL_CFLAGS := -fPIC -O1 -Werror -fno-strict-aliasing -DC_64=0 -DSY_GETTOD -malign-double -msse3 -mfpmath=sse -fno-stack-protector
endif
LOCAL_LDLIBS := -nostdlib -ldl -lc -llog

LOCAL_SRC_FILES := a.c ab.c af.c ai.c am.c am1.c amn.c ao.c ap.c ar.c as.c au.c c.c ca.c cc.c cd.c cf.c cg.c ch.c cip.c cl.c cp.c cpdtsp.c cr.c crs.c \
	ct.c cu.c cv.c cx.c d.c dc.c dss.c dstop.c dsusp.c dtoa.c f.c f2.c i.c io.c j.c jdlllic.c k.c m.c mbx.c p.c pv.c px.c r.c rl.c rt.c s.c sc.c sl.c \
	sn.c t.c u.c v.c v0.c v1.c v2.c va1.c va2.c va2s.c va2ss.c vamultsp.c vb.c vbang.c vbit.c vcant.c vchar.c vcat.c vcatsp.c vcomp.c vcompsc.c vd.c vdx.c ve.c \
	vf.c vfft.c vfrom.c  vfromsp.c vg.c vgauss.c vgcomp.c vgranking.c vgsort.c vgsp.c vi.c viix.c visp.c vm.c vo.c vp.c vq.c vrand.c vrep.c vs.c vsb.c \
	vt.c vu.c vx.c vz.c w.c wc.c wn.c ws.c x.c x15.c xa.c xb.c xc.c xcrc.c xd.c xf.c xfmt.c xh.c xi.c xl.c xo.c xs.c xt.c xu.c j-jni-interface.c \

include $(BUILD_SHARED_LIBRARY)
