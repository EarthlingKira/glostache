project(glostache_test)


add_executable(glostache_test
    main.cpp
)
    
target_link_libraries(glostache_test PRIVATE glostache_lib)


install(TARGETS glostache_test
        RUNTIME DESTINATION bin)
