add_library(j-dll OBJECT)
target_compile_definitions(j-dll PRIVATE _JDLL)
target_compile_definitions(j-dll PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(j-dll PRIVATE
    jdll.c
    jdll.h
    jdllcom.h
    jdllcomx.cpp
    jdlltype.h
    jexe.h
)
