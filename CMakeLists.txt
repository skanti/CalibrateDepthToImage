cmake_minimum_required(VERSION 3.0)
project(SD)

# dependencies
find_package(GameFramework REQUIRED)
include_directories(${GAMEFRAMEWORK_INCLUDE_DIRS}) 

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

find_package(glfw3 3.2 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(Eigen3 3.3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpnmio/src)
# <-

# -DEIGEN_NO_DEBUG
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DGLEW_NO_GLU")

set(SOURCE_FILES_CPP )

set(SOURCE_FILES_H src/Timer.h)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpnmio/lib)

add_executable(main src/main.cpp ${SOURCE_FILES_CPP} ${SOURCE_FILES_H})
target_link_libraries(main pnmio glfw ${GAMEFRAMEWORK_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} )

