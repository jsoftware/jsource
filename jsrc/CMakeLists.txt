add_library(j)
target_compile_definitions(j PRIVATE C_NA=0 C_AVX=1 C_AVX2=1 EMU_AVX=1)
target_compile_definitions(j PRIVATE $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>)
target_link_libraries(j PRIVATE ${STANDARD_MATH_LIBRARY})
set_source_files_properties(aes-ni.c PROPERTIES COMPILE_FLAGS -maes)
target_sources(j PRIVATE
    a.h
    aes-arm_table.h
    aes-c.h
    ar.h
    avx2intrin-emu.h
    avxintrin-emu.h
    avxintrin-neon.h
    cip_t.h
    cipfloatmm_t.h
    cpuinfo.h
    cr_t.h
    crc32c.h
    crc32ctables.h
    d.h
    dtoa.h
    fnmatch.h
    gemm.h
    j.h
    ja.h
    jc.h
    je.h
    jerr.h
    jfex.h
    js.h
    jt.h
    jtype.h
    jversion.h
    linenoise.h
    m.h
    p.h
    result.h
    s.h
    sse2neon.h
    sse2neon2.h
    va.h
    vasm.h
    vcomp.h
    vdx.h
    ve.h
    vg.h
    vgmerge.h
    vgmergemincomp.h
    vgsort.h
    vgsortq.h
    vq.h
    vx.h
    vz.h
    w.h
    x.h
    xc.h
    a.c
    ab.c
    aes-c.c
    aes-ni.c
    aes-sse2.c
    af.c
    ai.c
    am.c
    am1.c
    amn.c
    ao.c
    ap.c
    ar.c
    as.c
    au.c
    c.c
    ca.c
    cc.c
    cd.c
    cf.c
    cg.c
    ch.c
    cip.c
    cl.c
    cp.c
    cpdtsp.c
    cpuinfo.c
    cr.c
    crs.c
    ct.c
    cu.c
    cv.c
    cx.c
    d.c
    dc.c
    dss.c
    dstop.c
    dsusp.c
    dtoa.c
    f.c
    f2.c
    fbu.c
    gemm.c
    i.c
    io.c
    j.c
    jdlllic.c
    k.c
    m.c
    mbx.c
    p.c
    pv.c
    px.c
    r.c
    rl.c
    rt.c
    s.c
    sc.c
    sl.c
    sn.c
    t.c
    u.c
    v.c
    v0.c
    v1.c
    v2.c
    va1.c
    va1ss.c
    va2.c
    va2s.c
    va2ss.c
    vamultsp.c
    vb.c
    vbang.c
    vbit.c
    vcant.c
    vcat.c
    vcatsp.c
    vchar.c
    vcomp.c
    vcompsc.c
    vd.c
    vdx.c
    ve.c
    vf.c
    vfft.c
    vfrom.c
    vfromsp.c
    vg.c
    vgauss.c
    vgcomp.c
    vgranking.c
    vgsort.c
    vgsp.c
    vi.c
    viavx.c
    viix.c
    visp.c
    vm.c
    vo.c
    vp.c
    vq.c
    vrand.c
    vrep.c
    vs.c
    vsb.c
    vt.c
    vu.c
    vx.c
    vz.c
    w.c
    wc.c
    wn.c
    ws.c
    x.c
    x15.c
    xa.c
    xaes.c
    xb.c
    xc.c
    xcrc.c
    xd.c
    xf.c
    xfmt.c
    xh.c
    xi.c
    xl.c
    xo.c
    xs.c
    xsha.c
    xt.c
    xu.c
    crc32c.c
)

configure_file(jversion-x.h ${CMAKE_CURRENT_SOURCE_DIR}/jversion.h COPYONLY)

if(OpenMP_C_FOUND)
    target_link_libraries(j PRIVATE OpenMP::OpenMP_C)

    if(DEFINED OpenMP_libomp_DLL)
        add_custom_command(
            TARGET j POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${OpenMP_libomp_DLL}
                "${CMAKE_CURRENT_BINARY_DIR}$<${is_multi_config}:/$<CONFIG>>"
        )
    endif()
endif()

target_sources(j PRIVATE
    blis.h
    blis/gemm_c-ref.c
    blis/gemm_int-aarch64.c
    blis/gemm_int-avx.c
    blis/gemm_int-fma.c
    blis/gemm_int-sse2.c
    blis/gemm_vec-ref.c
)

if(NOT BUILD_SHARED_LIBS)
    return()
endif()

add_library(tsdll SHARED)
target_link_libraries(tsdll PRIVATE ${STANDARD_MATH_LIBRARY})
target_sources(tsdll PRIVATE
    tsdll.c
    ../makevs/tsdll/tsdll.def
)

add_library(linenoise OBJECT EXCLUDE_FROM_ALL)
target_compile_definitions(linenoise INTERFACE USE_LINENOISE)
target_compile_definitions(linenoise PUBLIC $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>)
target_sources(linenoise PRIVATE
    linenoise.h
    linenoise.c
)

add_executable(jconsole)
target_compile_definitions(jconsole PRIVATE READLINE)
if(NOT UNIX OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^(aarch64|arm)")
    target_link_libraries(jconsole PRIVATE linenoise)
endif()
if("${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
    target_link_options(jconsole PRIVATE /STACK:0x1000000)
endif()
target_link_libraries(jconsole PRIVATE ${CMAKE_DL_LIBS})
target_sources(jconsole PRIVATE
    jconsole.c
    jeload.h
    jeload.c
    jlib.h
    ../makevs/jconsole/jconsole.rc
    ../makevs/jconsole/resource1.h
    ../makevs/jconsole/jgray.ico
)

file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/jlibrary/bin" J_BINPATH)
file(TO_NATIVE_PATH "/profile.ijs" J_PROFILE_SCRIPT)
file(GENERATE
    OUTPUT "$<${is_multi_config}:$<CONFIG>/>profile.ijs"
    CONTENT "NB. loaded under debug
BINPATH_z_=: '${J_BINPATH}'
0!:0<BINPATH,'${J_PROFILE_SCRIPT}'"
)
