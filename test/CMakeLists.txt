#
#   test/CMakeLists.txt
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

include (SetupCatch)

add_executable (Sanity_Tests sanity.cxx)
target_link_libraries (Sanity_Tests Catch2::Catch2WithMain OpenCV)
catch_discover_tests (Sanity_Tests)
if (COVERAGE)
    enable_coverage (Sanity_Tests)
endif ()

find_package(Boost REQUIRED filesystem)
add_executable (Cluon_Tests cluon.cxx)
target_link_libraries (Cluon_Tests Catch2::Catch2WithMain steering_algo scaffolding cluon_engine Boost::filesystem)
catch_discover_tests (Cluon_Tests)
if (COVERAGE)
    enable_coverage (Cluon_Tests)
endif ()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/videos" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res.plot" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")


add_executable (Stubs_Tests stubs.cxx)
target_link_libraries (Stubs_Tests Catch2::Catch2WithMain stubs scaffolding)
catch_discover_tests (Stubs_Tests)
if (COVERAGE)
    enable_coverage (Stubs_Tests)
endif ()
