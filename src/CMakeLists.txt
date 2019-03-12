project(glostache_src)



include_directories(SYSTEM
    $ENV{HOME}/local/include
)
include_directories(SYSTEM
    $ENV{HOME}/local/include
)

set(cpp_files
    glo/stache.cpp
    )

set(libraries)

add_library(glostache_lib STATIC
    ${cpp_files}
)

# add_executable(glostache_bin
#     main.cpp
# )

# target_link_libraries(glostache_bin PRIVATE
#     glostache_lib
#     ${libraries}
# )


install(TARGETS glostache_lib
        ARCHIVE DESTINATION lib64)
        
install(DIRECTORY
        ../include/ DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

        
