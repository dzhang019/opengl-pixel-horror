cmake_minimum_required(VERSION 3.5)
set(ROOT "${CMAKE_CURRENT_LIST_DIR}")
project(shaderpipeline)
option(USE_SOLUTION "Use solution"  OFF)
# Add your project files
include_directories("include/")
# create executable
# include(CheckCXXCompilerFlag)
# CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
# CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
# if(COMPILER_SUPPORTS_CXX11)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# elseif(COMPILER_SUPPORTS_CXX0X)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
# else()
#   message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
# endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# OpenGL library
if (NOT CMAKE_VERSION VERSION_LESS "3.11")
  cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)

if(USE_SOLUTION)
  add_definitions("-DUSE_SOLUTION")
endif()

if(NOT TARGET glfw)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
  add_subdirectory(${ROOT}/glfw glfw)
endif()

add_subdirectory(${ROOT}/glad glad)

add_executable(${PROJECT_NAME} main.cpp src/camera.cpp src/object.cpp src/per_vertex_normals.cpp src/triangle_area_normal.cpp src/plane.cpp)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/eigen/)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/glfw/include)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/glad/include)
# target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/libigl/include)
# target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/libigl/external/stb)
target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_gl_LIBRARY} glad igl)
add_library(igl INTERFACE)
target_include_directories(igl INTERFACE
    ${ROOT}/libigl/include          # parent of "igl/readOBJ.h"
    ${ROOT}/eigen/
    ${ROOT}/libigl/external/stb
)