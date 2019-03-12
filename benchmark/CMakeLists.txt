project(glostache_benchmark)


add_compile_options(
    -std=c++17

    -O3

    -march=native
)

add_executable(glostache_benchmark
    main.cpp
)
    
target_link_libraries(glostache_benchmark PRIVATE glostache benchmark mstch bustache)


install(TARGETS glostache_benchmark
        RUNTIME DESTINATION bin)
