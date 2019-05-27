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


    -march=core-avx-i
)

add_library(glostache STATIC
    ${cpp_files}
)

option(ENABLE_LTO "enable LTO" OFF)

if(ENABLE_LTO)
    target_compile_options(glostache PUBLIC $<$<CONFIG:Release>:-flto=thin>)

    target_link_options(glostache PUBLIC $<$<CONFIG:Release>:-flto=thin -Wl,--thinlto-cache-dir=${PROJECT_BINARY_DIR}/lto.cache>)
endif()

target_link_options(glostache PUBLIC -fuse-ld=lld -Wl,--threads$<$<CONFIG:Release>:,--strip-all>)

set_property(TARGET glostache PROPERTY CXX_STANDARD 17)

target_link_libraries(glostache PRIVATE stdc++fs)


install(TARGETS glostache
        ARCHIVE DESTINATION lib64)

install(DIRECTORY
        ../include/ DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)


