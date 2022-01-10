#
#   CMake/Modules/Coverage.cmake
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

include_guard ()

function (enable_coverage target)
    if (MSVC)
        message (FATAL_ERROR "Coverage is only supported on GCC and Clang")
    endif ()

    target_compile_options ("${target}" PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options ("${target}" PRIVATE -lgcov --coverage -fprofile-arcs -ftest-coverage)
endfunction ()