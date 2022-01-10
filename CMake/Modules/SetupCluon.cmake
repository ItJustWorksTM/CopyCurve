#
#   CMake/Modules/SetupCluon.cmake
#   Copyright 2021 ItJustWorksTM
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#

include_guard (GLOBAL)

set (CLUON_VERSION 0.0.140)
set (ODLV_VERSION 0.9.9)

set (CLUON_SRC_URL "https://github.com/chrberger/libcluon/raw/gh-pages/headeronly/cluon-complete-v${CLUON_VERSION}.hpp")
set (CLUON_DLV_URL "https://github.com/chalmers-revere/opendlv.standard-message-set/raw/v${ODLV_VERSION}/opendlv.odvd")

find_package (Threads REQUIRED)
add_library (cluon INTERFACE)
target_link_libraries (cluon INTERFACE Threads::Threads)

if (UNIX)
    target_link_libraries (cluon INTERFACE rt)
endif ()

if (NOT EXTMODULE_FETCHCONTENT_INCLUDED)
    include (FetchContent RESULT_VARIABLE EXTMODULE_FETCHCONTENT_INCLUDED)
endif ()

set (cluon_SOURCE_DIR "${PROJECT_BINARY_DIR}/_deps/cluon-src")
set (cluon_BINARY_DIR "${PROJECT_BINARY_DIR}/_deps/cluon-build")
file (MAKE_DIRECTORY "${cluon_SOURCE_DIR}/include")
file (MAKE_DIRECTORY "${cluon_BINARY_DIR}")
if (NOT EXISTS "${cluon_SOURCE_DIR}/cluon-complete-src.cpp")
    file (DOWNLOAD "${CLUON_SRC_URL}" "${cluon_SOURCE_DIR}/include/cluon-complete.hpp")
    file (WRITE "${cluon_SOURCE_DIR}/cluon-complete-src.cpp" [[#include "cluon-complete.hpp"]])
endif ()

if (NOT DEFINED CLUON_MSC_EXECUTABLE)
    file (WRITE "${cluon_SOURCE_DIR}/CMakeLists.txt" [[
        cmake_minimum_required (VERSION 3.17)
        project (cluon-msc)
        find_package (Threads REQUIRED)
        add_executable (cluon-msc cluon-complete-src.cpp)
        target_include_directories (cluon-msc PRIVATE include)
        target_compile_definitions (cluon-msc PRIVATE HAVE_CLUON_MSC)
        target_link_libraries (cluon-msc PRIVATE Threads::Threads)
        file (GENERATE OUTPUT "${PROJECT_BINARY_DIR}/cluon-location.txt" CONTENT "$<TARGET_FILE:cluon-msc>")
    ]])
    execute_process (COMMAND "${CMAKE_COMMAND}" -S "${cluon_SOURCE_DIR}" -B "${cluon_BINARY_DIR}")
    execute_process (COMMAND "${CMAKE_COMMAND}" --build "${cluon_BINARY_DIR}")
    file (READ "${cluon_BINARY_DIR}/cluon-location.txt" CLUON_MSC_EXECUTABLE)
    if (NOT EXISTS "${CLUON_MSC_EXECUTABLE}")
        message (FATAL_ERROR "Failed to build cluon-msc")
    endif ()
set (CLUON_MSC_EXECUTABLE "${CLUON_MSC_EXECUTABLE}" CACHE INTERNAL "")
endif ()

if (NOT EXISTS "${cluon_SOURCE_DIR}/opendlv.odvd")
    file (DOWNLOAD "${CLUON_DLV_URL}" "${cluon_SOURCE_DIR}/opendlv.odvd")
    file (MAKE_DIRECTORY "${cluon_SOURCE_DIR}/include")
    execute_process (COMMAND "${CLUON_MSC_EXECUTABLE}" --cpp "--out=${cluon_SOURCE_DIR}/include/opendlv-standard-message-set.hpp" "${cluon_SOURCE_DIR}/opendlv.odvd")
endif ()

target_include_directories (cluon INTERFACE "${cluon_SOURCE_DIR}/include")
target_precompile_headers (cluon INTERFACE "<cluon-complete.hpp>")
