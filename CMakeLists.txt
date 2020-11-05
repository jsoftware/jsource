cmake_minimum_required(VERSION 3.17)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING
        "Semicolon separated list of supported configuration types")
    mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
elseif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_C_FLAGS)
    message(WARNING "No CMAKE_BUILD_TYPE is selected")
endif()

project(j)
enable_language(C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(OptimizeForArchitecture)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(OpenMP)

set(TARGET_ARCHITECTURE "skylake" CACHE STRING "CPU architecture")
OptimizeForArchitecture()
add_compile_options("$<$<COMPILE_LANGUAGE:C>:${Vc_ARCHITECTURE_FLAGS}>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${Vc_ARCHITECTURE_FLAGS}>")

add_subdirectory(jsrc)
add_subdirectory(sleef)
add_subdirectory(base64)
add_subdirectory(dllsrc)

target_link_libraries(j PRIVATE j-blis j-openssl-sha j-sleef j-base64)
if(WIN32)
    target_link_libraries(j PRIVATE j-dll)
    target_sources(j PRIVATE dllsrc/jdll.def dllsrc/jdll.rc dllsrc/jdll.tlb)
endif()
