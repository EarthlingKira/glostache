cmake_minimum_required(VERSION 3.10)


project(glostache)


set(CMAKE_CXX_STANDARD 17)


IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "$ENV{HOME}/local" CACHE PATH "Default local install" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)



include_directories(src)
include_directories(include)


include_directories(SYSTEM
    ${CMAKE_INSTALL_PREFIX}/include
)

link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib64
)


add_subdirectory(src)

add_subdirectory(test)

