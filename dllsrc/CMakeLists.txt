add_library(j-dll OBJECT)
set_source_files_properties(jdllcomx.cpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
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
