project(glostache_benchmark)


add_compile_options(
    -std=c++17

    -O3

    -march=native
)

add_executable(glostache_benchmark EXCLUDE_FROM_ALL
    main.cpp
)
    
target_link_libraries(glostache_benchmark PRIVATE glostache_lib benchmark mstch bustache)

add_dependencies(glostache_benchmark glostache_lib)
