add_library(j-dll OBJECT)
target_compile_definitions(j-dll PRIVATE $<$<BOOL:BUILD_SHARED_LIBS>:_JDLL>)
target_compile_definitions(j-dll PRIVATE _CRT_SECURE_NO_WARNINGS)
set_target_properties(j-dll PROPERTIES EXCLUDE_FROM_ALL YES)
target_sources(j-dll PRIVATE
    jdll.c
    jdll.h
    jdllcom.h
    jdllcomx.cpp
    jdlltype.h
    jexe.h
)
