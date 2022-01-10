#
#   CMake/Modules/SetupCatch.cmake
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

if (NOT EXTMODULE_FETCHCONTENT_INCLUDED)
    include (FetchContent RESULT_VARIABLE EXTMODULE_FETCHCONTENT_INCLUDED)
endif ()

FetchContent_Declare (catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        devel)
FetchContent_GetProperties (catch)
if (NOT catch_POPULATED)
    FetchContent_Populate (catch)
endif ()
add_subdirectory (${catch_SOURCE_DIR} ${catch_BINARY_DIR})
list (APPEND CMAKE_MODULE_PATH "${catch_SOURCE_DIR}/extras")

include (Catch)

# For CTest to let us use it
find_program (MEMORYCHECK_COMMAND valgrind)
set (MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
