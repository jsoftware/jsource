if(WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_library(j-sleef OBJECT)
target_include_directories(j-sleef PRIVATE src/arch src/common)
target_compile_definitions(j-sleef INTERFACE SLEEF=1)
target_compile_definitions(j-sleef PRIVATE SLEEF_STATIC_LIBS ENABLE_AVX2 DORENAME=1)
target_sources(j-sleef PRIVATE
    include/sleef.h
    src/arch/avxintrin-emu.h
    src/arch/helperadvsimd.h
    src/arch/helperavx.h
    src/arch/helperavx2.h
    src/arch/helpersse2.h
    src/arch/helpervecext.h
    src/common/arraymap.c
    src/common/arraymap.h
    src/common/common.c
    src/common/common.h
    src/common/misc.h
    src/libm/dd.h
    src/libm/estrin.h
    src/libm/funcproto.h
    src/libm/importsample.h
    src/libm/norename.h
    src/libm/rempitab.c
    src/libm/rename.h
    src/libm/renameadvsimd.h
    src/libm/renameadvsimdnofma.h
    src/libm/renameavx.h
    src/libm/renameavx2.h
    src/libm/renamesse2.h
    src/libm/renamevecext.h
    src/libm/sleefsimddp.c
)
