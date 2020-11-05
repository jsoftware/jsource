add_library(j-base64 OBJECT)
target_compile_definitions(j-base64 PRIVATE HAVE_AVX2=1)
target_sources(j-base64 PRIVATE
    include/libbase64.h
    lib/arch/avx/codec-avx.c
    lib/arch/avx2/codec-avx2.c
    lib/arch/generic/codec-generic.c
    lib/arch/neon64/codec-neon64.c
    lib/arch/sse41/codec-sse41.c
    lib/arch/sse42/codec-sse42.c
    lib/arch/ssse3/codec-ssse3.c
    lib/codecs.h
    lib/codec_choose.c
    lib/config.h
    lib/env.h
    lib/lib.c
    lib/tables/tables.c
    lib/tables/tables.h
    lib/tables/table_dec_32bit.h
    lib/tables/table_enc_12bit.h
)
