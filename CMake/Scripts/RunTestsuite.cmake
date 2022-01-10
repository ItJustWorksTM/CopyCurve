#
#   CMake/Scripts/RunTestsuite.cmake
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

if (NOT EXISTS "${BUILD_DIR}")
    message (FATAL_ERROR "No such directory \"${BUILD_DIR}\"")
elseif (NOT IS_DIRECTORY "${BUILD_DIR}")
    message (FATAL_ERROR "Build location \"${BUILD_DIR}\" is not a directory")
endif ()

file (READ "${BUILD_DIR}/source_location.txt" SOURCE_DIR)

if (COVERAGE OR "$ENV{COVERAGE}")
    # Regenerate with Coverage
    execute_process (COMMAND "${CMAKE_COMMAND}" -DCOVERAGE=On "${BUILD_DIR}")
endif ()

execute_process (COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target ExperimentalBuild
        RESULT_VARIABLE BUILD_TESTS_RESULT)
if (BUILD_TESTS_RESULT)
    message (FATAL_ERROR "Build invocation failed: subprocess exited with error ${BUILD_TESTS_RESULT}")
endif ()

if (VALGRIND)
    set (CTEST_TARGET_SUFFIX MemCheck)
else ()
    set (CTEST_TARGET_SUFFIX Test)
endif ()

set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)
execute_process (COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "Experimental${CTEST_TARGET_SUFFIX}"
                 WORKING_DIRECTORY "${BUILD_DIR}"
                 RESULT_VARIABLE RUN_TEST_RESULT)
if (RUN_TEST_RESULT)
    message (FATAL_ERROR "Build invocation failed: subprocess exited with error ${RUN_TEST_RESULT}")
endif ()

if (COVERAGE OR "$ENV{COVERAGE}")
    execute_process (COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "ExperimentalCoverage"
                     WORKING_DIRECTORY "${BUILD_DIR}"
                     RESULT_VARIABLE RUN_TEST_RESULT)
    if (RUN_TEST_RESULT)
        message (FATAL_ERROR "Build invocation failed: subprocess exited with error ${RUN_TEST_RESULT}")
    endif ()

    find_program (GCOVR_EXECUTABLE gcovr)
    if (GCOVR_EXECUTABLE)
        if (PRINT_COVERAGE OR $ENV{PRINT_COVERAGE})
            execute_process (COMMAND "${GCOVR_EXECUTABLE}" -r "${SOURCE_DIR}" -f "src/" -f "test/" -e "src/main.cxx" --exclude-unreachable-branches)
        endif ()
        execute_process (COMMAND "${GCOVR_EXECUTABLE}" --xml-pretty -r "${SOURCE_DIR}" -f "src/" -f "test/" -e "src/main.cxx" --exclude-unreachable-branches --print-summary -o "${BUILD_DIR}/coverage.xml")
    endif ()
endif ()
