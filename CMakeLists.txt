cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(agnicalculator)

# CGAL and its components
find_package( CGAL QUIET COMPONENTS Core GMP MPFI )

if ( NOT SEING_FOUND )

    message(STATUS "This project requires the SEING library, and will not be compiled.")
    return()

endif()

# include helper file
include( ${CGAL_USE_FILE} )


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

    message(STATUS "This project requires the Boost library, and will not be compiled.")

    return()

endif()

set(SOURCE_DIR "src/agnicalculator")
include_directories(${SOURCE_DIR})


add_subdirectory(lib/pybind11)
pybind11_add_module(seing_agnicalculator ${SOURCES} "${SOURCE_DIR}/seing_agnicalculator.cpp")

