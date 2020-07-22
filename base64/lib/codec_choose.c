#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/libbase64.h"
#include "codecs.h"
#include "config.h"
#include "env.h"

// Function declarations:
#define BASE64_CODEC_FUNCS(arch)	\
	BASE64_ENC_FUNCTION(arch);	\
	BASE64_DEC_FUNCTION(arch);	\

BASE64_CODEC_FUNCS(avx2)
// BASE64_CODEC_FUNCS(neon32)
BASE64_CODEC_FUNCS(neon64)
BASE64_CODEC_FUNCS(plain)
BASE64_CODEC_FUNCS(ssse3)
BASE64_CODEC_FUNCS(sse41)
BASE64_CODEC_FUNCS(sse42)
BASE64_CODEC_FUNCS(avx)

void
codec_choose (struct codec *codec, int flags)
{
	if (flags & BASE64_FORCE_AVX2) {
		codec->enc = base64_stream_encode_avx2;
		codec->dec = base64_stream_decode_avx2;
		return;
	}
// 	if (flags & BASE64_FORCE_NEON32) {
// 		codec->enc = base64_stream_encode_neon32;
// 		codec->dec = base64_stream_decode_neon32;
// 		return;
// 	}
	if (flags & BASE64_FORCE_NEON64) {
		codec->enc = base64_stream_encode_neon64;
		codec->dec = base64_stream_decode_neon64;
		return;
	}
	if (flags & BASE64_FORCE_PLAIN) {
		codec->enc = base64_stream_encode_plain;
		codec->dec = base64_stream_decode_plain;
		return;
	}
	if (flags & BASE64_FORCE_SSSE3) {
		codec->enc = base64_stream_encode_ssse3;
		codec->dec = base64_stream_decode_ssse3;
		return;
	}
	if (flags & BASE64_FORCE_SSE41) {
		codec->enc = base64_stream_encode_sse41;
		codec->dec = base64_stream_decode_sse41;
		return;
	}
	if (flags & BASE64_FORCE_SSE42) {
		codec->enc = base64_stream_encode_sse42;
		codec->dec = base64_stream_decode_sse42;
		return;
	}
	if (flags & BASE64_FORCE_AVX) {
		codec->enc = base64_stream_encode_avx;
		codec->dec = base64_stream_decode_avx;
		return;
	}
	return;
}

