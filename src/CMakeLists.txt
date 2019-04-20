project(glostache_src)


set(cpp_files glo/stache.cpp)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(compiler_specific_compile_options
        -Weverything
        -Wno-c++98-compat
        -Wno-switch-enum
        -Wno-missing-braces
    )
endif()


add_compile_options(

    "$<$<CONFIG:Debug>:-O0>"
    "$<$<CONFIG:Release>:-O3>"

    ${compiler_specific_compile_options}

    "$<$<CONFIG:Debug>:-Werror>"


    -march=native
)

add_library(glostache_lib STATIC
    ${cpp_files}
)

set_property(TARGET glostache_lib PROPERTY CXX_STANDARD 20)

target_link_libraries(glostache_lib PRIVATE stdc++fs)


install(TARGETS glostache_lib
        ARCHIVE DESTINATION lib64)

install(DIRECTORY
        ../include/ DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)


