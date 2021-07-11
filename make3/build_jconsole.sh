#!/usr/bin/env sh
set -o errexit -o nounset -o noclobber

host=$(uname --machine)

case "$host" in
	armv6l|aarch64) jplatform=${jplatform:-raspberry};;
	Darwin) jplatform=${jplatform:-darwin};;
	*) jplatform=${jplatform:-linux};;
esac
[ "${RASPI:-}" ] && jplatform=${jplatform:-raspberry};

case "$host" in
	x86_64) j64x=${j64x:-j64avx};;
	aarch64) j64x=${j64x:-j64};;
	*) j64x=${j64x:-j32};;
esac

[ "${USE_THREAD:-}" ] && USETHREAD="=DUSE_THREAD"
#[ "${USE_THREAD:-}" ] && LDTHREAD='-pthread'
[ "${VERBOSELOG:-}" ] && FVERBOSELOG="-DVERBOSELOG"

CFLAGS="${CFLAGS:-} -DREADLINE"
[ "${USE_LINENOISE:-1}" -ne 1 ] && unset USE_LINENOISE
[ "${USE_LINENOISE:-1}" ] && CFLAGS="${CFLAGS:-} -DUSE_LINENOISE"
[ "${USE_LINENOISE:-1}" ] && OBJSLN="linenoise.o"

[ -f /usr/bin/cc ] && CC=${CC:-cc}
[ -f /usr/bin/clang ] && CC=${CC:-clang}
CC=${CC:-gcc}

macmin="-mmacosx-version-min=10.6"
# compiler=$("$CC" --version | head -n 1)
compiler=$(readlink -f "$(command -v "$CC")" 2> /dev/null || echo "$CC")

##
# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5
CFLAGS="${CFLAGS:-} ${USETHREAD:-} ${FVERBOSELOG:-} -Werror -fPIC -O2 -fvisibility=hidden -fwrapv -fno-strict-aliasing -Wextra"
if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
	CFLAGS="${CFLAGS:-} -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-type-limits"
	GNUC_MAJOR=$(echo __GNUC__ | "$CC" -E -x c - | tail -n 1)
	# shellcheck disable=SC2034
	GNUC_MINOR=$(echo __GNUC_MINOR__ | "$CC" -E -x c - | tail -n 1)
	[ "$GNUC_MAJOR" -lt 5 ] && CFLAGS="${CFLAGS:-} -DC_NOMULTINTRINSIC -Wno-uninitialized"
	[ "$GNUC_MAJOR" -ge 5 ] && CFLAGS="${CFLAGS:-} -Wno-maybe-uninitialized"
	[ "$GNUC_MAJOR" -ge 6 ] && CFLAGS="${CFLAGS:-} -Wno-shift-negative-value"
	# alternatively, add comment /* fall through */
	[ "$GNUC_MAJOR" -ge 7 ] && CFLAGS="${CFLAGS:-} -Wno-implicit-fallthrough"
	[ "$GNUC_MAJOR" -ge 8 ] && CFLAGS="${CFLAGS:-} -Wno-cast-function-type"
else # clang 3.4
	CFLAGS="${CFLAGS:-} -Wextra -Wno-consumed -Wuninitialized -Wno-unused-parameter -Wsign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wunsequenced -Wno-string-plus-int -Wtautological-constant-out-of-range-compare"
	# clang 3.8
	CLANG_MAJOR=$(echo __clang_major__ | "$CC" -E -x c - | tail -n 1)
	CLANG_MINOR=$(echo __clang_minor__ | "$CC" -E -x c - | tail -n 1)
	[ "$CLANG_MAJOR" -eq 3 ] && [ "$CLANG_MINOR" -ge 8 ] && CFLAGS="${CFLAGS:-} -Wno-pass-failed"
	[ "$CLANG_MAJOR" -ge 4 ] && CFLAGS="${CFLAGS:-} -Wno-pass-failed"
	[ "$CLANG_MAJOR" -ge 10 ] && CFLAGS="${CFLAGS:-} -Wno-implicit-float-conversion"
fi

case "$jplatform" in
	windows) TARGET=jconsole.exe;;
	*) TARGET=jconsole;;
esac

case "$jplatform" in
	linux|raspberry) LDFLAGS="${LDFLAGS:-} -ldl ${LDTHREAD:-}";;
	darwin) LDFLAGS="${LDFLAGS:-} -ldl ${LDTHREAD:-} $macmin";;
	windows) LDFLAGS="${LDFLAGS:-} -Wl,--stack=0x1000000,--subsystem,console -static-libgcc ${LDTHREAD:-}" ;;
esac

case "$jplatform" in
	raspberry) CFLAGS="${CFLAGS:-} -DRASPI";;
	darwin) CFLAGS="${CFLAGS:-} $macmin" ;;
esac

case "${jplatform}_$j64x" in
	raspberry_j32) CFLAGS="${CFLAGS:-} -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp" ;;
	raspberry_j64) CFLAGS="${CFLAGS:-} -march=armv8-a+crc" ;;
	linux_j32|darwin_j32|windows_j32)
		CFLAGS="${CFLAGS:-} -m32"
		LDFLAGS="${LDFLAGS:-} -m32";;
	linux_j64|darwin_j64|windows_j64);;
	linux_j64avx|darwin_j64avx|windows_j64avx);;
	linux_j64avx2|darwin_j64avx2|windows_j64avx2);;
	*) >&2 echo "ERROR: Incompatible parameter set: $jplatform $j64x"; exit 1;;
esac

cat <<-VARS
CFLAGS=${CFLAGS:-}
LDFLAGS=${LDFLAGS:-}
TARGET=${TARGET:-}
OBJSLN=${OBJSLN:-}
jplatform=${jplatform:-}
j64x=${j64x:-}
VARS

jmake=$(dirname "$0")
jsource=$jmake/..

[ -r "$jsource/jsrc/jversion.h" ] || cp "$jsource/jsrc/jversion-x.h" "$jsource/jsrc/jversion.h"
mkdir -p "$jsource/bin/$jplatform/$j64x"
mkdir -p "$jmake/obj/$jplatform/$j64x/"
cp makefile-jconsole "obj/$jplatform/$j64x/."

export CC CFLAGS LDFLAGS TARGET OBJSLN jplatform j64x
make -C "$jmake/obj/$jplatform/$j64x/" -f makefile-jconsole
