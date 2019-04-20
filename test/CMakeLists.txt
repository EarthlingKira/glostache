project(glostache_test)


add_executable(glostache_test EXCLUDE_FROM_ALL
    main.cpp
)
set_property(TARGET glostache_test PROPERTY CXX_STANDARD 20)

include_directories(test)
    
target_link_libraries(glostache_test PRIVATE glostache)


add_dependencies(glostache_test glostache)
