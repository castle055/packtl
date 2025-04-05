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
#      - Configure dependencies                                                #
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

cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -pthread -fexperimental-library")

project(packtl LANGUAGES CXX C)

#set(CMAKE_CXX_EXTENSIONS OFF)
SET(PACKTL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(PACKTL_TEST_DIR ${PACKTL_DIR}/test)
SET(PACKTL_INCLUDE_DIR ${PACKTL_DIR}/include)
SET(PACKTL_SOURCE_DIR ${PACKTL_DIR}/src)
SET(PACKTL_CMAKE_DIR ${PACKTL_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PACKTL_CMAKE_DIR}/Modules)


################################################################################
#[[                          CONFIGURE DEPENDENCIES                          ]]#
################################################################################
include(ExternalProject)
include(FetchContent)
find_package(PkgConfig)

#[[                           CONFIGURE STD MODULE                           ]]#
add_library(__unused_std_target STATIC)
target_sources(__unused_std_target
        PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS /usr/share/libc++/v1
        FILES /usr/share/libc++/v1/std.cppm /usr/share/libc++/v1/std.compat.cppm)
target_compile_features(__unused_std_target PUBLIC cxx_std_23)

#[[                         CONFIGURE CMAKE UTILS                            ]]#
FetchContent_Declare(cmake_utils
        GIT_REPOSITORY https://github.com/castle055/cmake-utils.git
        GIT_TAG main
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cmake_utils)


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
target_compile_options(packtl PUBLIC -stdlib=libc++)
target_sources(packtl
        PUBLIC ${SRC_LIST}
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS ${PACKTL_SOURCE_DIR} ${PACKTL_INCLUDE_DIR}
        FILES ${SRC_MOD_LIST}
)


################################################################################
#[[                             CONFIGURE TESTS                              ]]#
################################################################################
if (PACKTL_DEV_MODE AND PACKTL_BUILD_TESTS)
    target_configure_test_directory(packtl ${PACKTL_TEST_DIR} TEST_TARGET_LIST)
endif ()


################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
if (PACKTL_DEV_MODE AND PACKTL_BUILD_DOCS)
    target_configure_doxygen(packtl
            DOXYFILE ${PACKTL_DIR}/Doxyfile
            BASE_DIR ${PACKTL_DIR}
    )
endif ()

