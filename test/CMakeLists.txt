project(glostache_test)


add_executable(glostache_test EXCLUDE_FROM_ALL
    main.cpp
)

include_directories(test)
    
target_link_libraries(glostache_test PRIVATE glostache)


add_dependencies(glostache_test glostache)
