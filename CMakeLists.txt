cmake_minimum_required(VERSION 3.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# 
# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Release)
# endif()
# 
# set(CMAKE_CXX_FLAGS "-O3")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(PYTHON_EXECUTABLE "/home/minion/anaconda3/bin/python")
set(PYTHON_LIBRARY_DIR "/home/minion/anaconda3/lib")

project(seing4python)

# include_directories("${CMAKE_SOURCE_DIR}/src/")
include_directories("${CMAKE_SOURCE_DIR}/python")

file (GLOB SOURCE_FILES "src/*.cpp")
file (GLOB HEADER_FILES "src/*.h")
file (GLOB PYTHON_FILES "python/*.cpp" "python/*.hpp")

# Set up such that XCode organizes the files
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES} )

# include(pybind11.cmake)
find_package(pybind11 REQUIRED)
pybind11_add_module(seing4python
	${SOURCE_FILES}
	${HEADER_FILES}
	${PYTHON_FILES}
)

# target_link_libraries(seing4python PUBLIC)

# install(TARGETS seing4python
#   COMPONENT python
#   LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}"
#   )
