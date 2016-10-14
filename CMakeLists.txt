cmake_minimum_required(VERSION 3.3)
project (Game)
set(CMAKE_BUILD_TYPE Debug)
include_directories(include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_definitions(-std=c++1z)


find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
LIST(APPEND INC ${GLFW_INCLUDE_DIRS})
LIST(APPEND LIB ${GLFW_LIBRARIES})

find_package(GLEW REQUIRED)
LIST(APPEND INC ${GLEW_INCLUDE_DIRS})
LIST(APPEND LIB ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
LIST(APPEND LIB ${OPENGL_LIBRARIES})

# find_package(Lua 5.1 REQUIRED)
# LIST(APPEND LIB ${LUA_LIBRARIES})
LIST(APPEND LIB "luajit-5.1")
# LIST(APPEND LIB "lua")

include_directories(INC)
include_directories(include/extern)
add_subdirectory(src)
