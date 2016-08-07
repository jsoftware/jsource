#!/bin/bash
# $1 is j32 or j64
cd ~
. jvars.sh

# gcc-6 -O1 needs -fno-if-conversion2 for x15.c double trick
# but clang does not have this flag
# gcc
common="-fPIC -O1 -fno-if-conversion2 -fno-strict-aliasing -DSY_GETTOD -Wextra -Wno-maybe-uninitialized -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses"
# clang
# common="-Werror -fPIC -O1 -fno-strict-aliasing -DSY_GETTOD -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-unsequenced -Wno-string-plus-int"
darwin="-fPIC -O1 -fno-strict-aliasing -DSY_GETTOD -Wno-string-plus-int -Wno-empty-body -Wno-unsequenced -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-return-type -Wno-constant-logical-operand -Wno-comment -Wno-unsequenced"

case $jplatform\_$1 in

linux_j32) # linux x86
TARGET=libj.so
COMPILE="$common -m32 -DC_64=0"
LINK=" -shared -Wl,-soname,libj.so -m32 -lm -ldl -o libj.so "
;;
linux_j64) # linux x86
TARGET=libj.so
COMPILE="$common -DC_NA=0"
LINK=" $jgit/asm/linuxasm64.o -shared -Wl,-soname,libj.so -lm -ldl -o libj.so "
;;

raspberry_j32) # linux raspian arm
TARGET=libj.so
COMPILE="$common -fsigned-char -DC_64=0 -DC_CD_ARMHF -DRASPI"
LINK=" -shared -Wl,-soname,libj.so -lm -ldl -o libj.so "
;;

darwin_j32) # darwin x86
TARGET=libj.dylib
COMPILE="$darwin -m32 -DC_64=0 -mmacosx-version-min=10.5"
LINK=" -dynamiclib -lm -ldl -m32 -mmacosx-version-min=10.5 -o libj.dylib"
;;
darwin_j64) # darwin x86
TARGET=libj.dylib
COMPILE="$darwin -mmacosx-version-min=10.5"
LINK=" -dynamiclib -lm -ldl -mmacosx-version-min=10.5 -o libj.dylib"
;;
*)
echo no case for those parameters
exit
esac

OBJS="\
 a.o \
 ab.o \
 af.o \
 ai.o \
 am.o \
 am1.o \
 amn.o \
 ao.o \
 ap.o \
 ar.o \
 as.o \
 au.o \
 c.o \
 ca.o \
 cc.o \
 cd.o \
 cf.o \
 cg.o \
 ch.o \
 cip.o \
 cl.o \
 cp.o \
 cpdtsp.o \
 cr.o \
 crs.o \
 ct.o \
 cu.o \
 cv.o \
 cx.o \
 d.o \
 dc.o \
 dss.o \
 dstop.o \
 dsusp.o \
 dtoa.o \
 f.o \
 f2.o \
 i.o \
 io.o \
 j.o \
 jdlllic.o \
 k.o \
 m.o \
 mbx.o \
 p.o \
 pv.o \
 px.o \
 r.o \
 rl.o \
 rt.o \
 s.o \
 sc.o \
 sl.o \
 sn.o \
 t.o \
 u.o \
 v.o \
 v0.o \
 v1.o \
 v2.o \
 va1.o \
 va2.o \
 va2s.o \
 va2ss.o \
 vamultsp.o \
 vb.o \
 vbang.o \
 vbit.o \
 vcant.o \
 vchar.o \
 vcat.o \
 vcatsp.o \
 vcomp.o \
 vcompsc.o \
 vd.o \
 vdx.o \
 ve.o \
 vf.o \
 vfft.o \
 vfrom.o \
 vfromsp.o \
 vg.o \
 vgauss.o \
 vgcomp.o \
 vgranking.o \
 vgsort.o \
 vgsp.o \
 vi.o \
 viix.o \
 visp.o \
 vm.o \
 vo.o \
 vp.o \
 vq.o \
 vrand.o \
 vrep.o \
 vs.o \
 vsb.o \
 vt.o \
 vu.o \
 vx.o \
 vz.o \
 w.o \
 wc.o \
 wn.o \
 ws.o \
 x.o \
 x15.o \
 xa.o \
 xb.o \
 xc.o \
 xcrc.o \
 xd.o \
 xf.o \
 xfmt.o \
 xh.o \
 xi.o \
 xl.o \
 xo.o \
 xs.o \
 xt.o \
 xu.o "

export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

