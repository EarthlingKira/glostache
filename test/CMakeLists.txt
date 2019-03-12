project(glostache_test)


add_executable(glostache_test
    main.cpp
)

include_directories(test)
    
target_link_libraries(glostache_test PRIVATE glostache)


install(TARGETS glostache_test
        RUNTIME DESTINATION bin)
