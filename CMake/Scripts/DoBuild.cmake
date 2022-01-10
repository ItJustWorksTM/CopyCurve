#
#   CMake/Scripts/DoBuild.cmake
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

cmake_minimum_required (VERSION 3.17)

if (NOT CMAKE_SCRIPT_MODE_FILE)
    message (FATAL_ERROR "This file a script, not intended to be included from a CMake build configuration")
endif ()

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
    cmake_path (GET CMAKE_SCRIPT_MODE_FILE PARENT_PATH SCRIPTS_DIR)
else ()
    get_filename_component (SCRIPTS_DIR "${CMAKE_SCRIPT_MODE_FILE}" DIRECTORY)
endif ()
set (REPOSITORY_ROOT "${SCRIPTS_DIR}/../..")
set (DEFAULT_BUILD_DIR "${REPOSITORY_ROOT}/build")

if (DEFINED ENV{BUILD_DIR})
    set (BUILD_DIR "$ENV{BUILD_DIR}")
else ()
    set (BUILD_DIR "${DEFAULT_BUILD_DIR}")
endif ()

file (MAKE_DIRECTORY "${BUILD_DIR}")

execute_process (COMMAND "${CMAKE_COMMAND}" -S "${REPOSITORY_ROOT}" -B "${BUILD_DIR}"
                 RESULT_VARIABLE CONFIG_RESULT)
if (CONFIG_RESULT)
    message (FATAL_ERROR "Configuration invocation failed: subprocess exited with error ${CONFIG_RESULT}")
endif ()

execute_process (COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "copycurve"
                 RESULT_VARIABLE BUILD_RESULT)
if (BUILD_RESULT)
    message (FATAL_ERROR "Build invocation failed: subprocess exited with error ${BUILD_RESULT}")
endif ()