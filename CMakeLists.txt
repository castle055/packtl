# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later


################################################################################
#[[                            PROJECT OVERVIEW                              ]]#
################################################################################
#                                                                    PackTL    #
#                                                                              #
#[[  AVAILABLE OPTIONS:                                                      ]]#
#      - PACKTL_BUILD_TESTS ............................. DEV_MODE only, ON    #
#            whether or not tests should be built                              #
#      - PACKTL_BUILD_DOCS .............................. DEV_MODE only, ON    #
#            whether or not the documentation should be built                  #
#                                                                              #
#[[  CMAKE STRUCTURE:                                                        ]]#
#      - Project setup                                                         #
#      - Configure dependencies:                                               #
#          - STD Module   (system)                                             #
#      - Configure PackTL                                                      #
#      - Configure tests                                                       #
#      - Configure Doxygen documentation                                       #
#                                                                              #
################################################################################


################################################################################
#[[                              PROJECT SETUP                               ]]#
################################################################################
#[[                          PREVENT IN-TREE BUILD                           ]]#
if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. Create a `build/` directory and call `cmake ..` from there.")
endif ()

#[[                          CHECK DEVELOPER MODE                            ]]#
set(PACKTL_DEV_MODE OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(PACKTL_DEV_MODE ON)
endif ()

#[[                         DEVELOPER MODE OPTIONS                           ]]#
if(PACKTL_DEV_MODE)
    option(PACKTL_BUILD_TESTS "whether or not tests should be built" ON)
    option(PACKTL_BUILD_DOCS "whether or not the documentation should be built" ON)
endif ()

# Select 'Release' build type by default.
# Has to be done before the call to `project()`.
# Use `-DCMAKE_BUILD_TYPE=` to override this.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")

cmake_minimum_required(VERSION 3.29.20240518 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(packtl LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -std=c++23")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

message("CMAKE VERSION: " ${CMAKE_VERSION})
message("COMPILER: " ${CMAKE_CXX_COMPILER})

SET(PACKTL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(PACKTL_TEST_DIR ${PACKTL_DIR}/test)
SET(PACKTL_INCLUDE_DIR ${PACKTL_DIR}/include)
SET(PACKTL_SOURCE_DIR ${PACKTL_DIR}/src)
SET(PACKTL_CMAKE_DIR ${PACKTL_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PACKTL_CMAKE_DIR}/Modules)


include(cmake/compiler_flags.cmake)
compiler_flags(
        "-stdlib=libc++"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"

#        "-Wliteral-suffix"

        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        "-Wno-error=unused"
        "-Wno-unused-local-typedefs"

        "-Wno-unknown-pragmas"
#        "-freport-bug"
)


################################################################################
#[[                          CONFIGURE DEPENDENCIES                          ]]#
################################################################################
include(ExternalProject)
include(FetchContent)
find_package(PkgConfig)

#[[                           CONFIGURE STD MODULE                           ]]#
add_library(std)
target_sources(std
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS /usr/share/libc++/v1
        FILES /usr/share/libc++/v1/std.cppm
        /usr/share/libc++/v1/std.compat.cppm
)
target_compile_options(std
    PUBLIC
    -Wno-reserved-module-identifier
    -Wno-deprecated-declarations
)

################################################################################
#[[                           CONFIGURE CYD PACKTL                           ]]#
################################################################################
add_library(packtl)

FILE(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        ${PACKTL_INCLUDE_DIR}/*.h
        ${PACKTL_INCLUDE_DIR}/*.hpp
        ${PACKTL_INCLUDE_DIR}/*.cpp
        ${PACKTL_SOURCE_DIR}/*.h
        ${PACKTL_SOURCE_DIR}/*.hpp
        ${PACKTL_SOURCE_DIR}/*.cpp
)
FILE(GLOB_RECURSE SRC_MOD_LIST CONFIGURE_DEPENDS
        ${PACKTL_INCLUDE_DIR}/*.cppm
        ${PACKTL_SOURCE_DIR}/*.cppm
)

target_include_directories(packtl PUBLIC ${PACKTL_INCLUDE_DIR})
target_compile_features(packtl PUBLIC cxx_std_23)
target_sources(packtl
        PUBLIC ${SRC_LIST}
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS ${PACKTL_SOURCE_DIR} ${PACKTL_INCLUDE_DIR}
        FILES ${SRC_MOD_LIST}
)
target_link_libraries(packtl
        PUBLIC
        std
)


################################################################################
#[[                             CONFIGURE TESTS                              ]]#
################################################################################
include(${PACKTL_CMAKE_DIR}/ctest.cmake)

if (PACKTL_DEV_MODE AND PACKTL_BUILD_TESTS)
    target_configure_test_directory(packtl ${PACKTL_TEST_DIR})
endif ()


################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
include(${PACKTL_CMAKE_DIR}/docs.cmake)

if (PACKTL_DEV_MODE AND PACKTL_BUILD_DOCS)
    target_configure_doxygen(packtl
            DOXYFILE ${PACKTL_DIR}/Doxyfile
            BASE_DIR ${PACKTL_DIR}
    )
endif ()

