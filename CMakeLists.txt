#
#   CMakeLists.txt
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

project (CopyCurve)
file (WRITE "${PROJECT_BINARY_DIR}/source_location.txt" "${PROJECT_SOURCE_DIR}")
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")
include (Coverage)

include (SetupOCV)

find_package (Threads REQUIRED)
add_library (scaffolding INTERFACE)
target_compile_features (scaffolding INTERFACE cxx_std_20)
target_link_libraries(scaffolding INTERFACE Threads::Threads)
target_sources (scaffolding INTERFACE
        include/scaffolding/scaffolding.hxx
        include/scaffolding/frame.hxx
)
target_include_directories (scaffolding INTERFACE include)

add_library (stubs INTERFACE)
target_link_libraries (stubs INTERFACE scaffolding)
target_sources (stubs INTERFACE
        include/stub-compute.hxx
        src/stub-compute.cxx
        include/stub-engine.hxx
        src/stub-engine.cxx
)

include (SetupCluon)

add_library (cluon_engine OBJECT)
target_compile_features (cluon_engine PRIVATE cxx_std_20)
target_sources (cluon_engine PRIVATE include/cluon/cluon-engine.hxx src/cluon/cluon-engine.cxx)
target_link_libraries (cluon_engine PUBLIC scaffolding OpenCV cluon)

add_library(steering_algo OBJECT)
target_compile_features (steering_algo PRIVATE cxx_std_20)
target_include_directories (steering_algo PRIVATE include)
target_sources(steering_algo PRIVATE
        include/copy-curve.hxx
        include/steering-calculation.hxx
        src/steering-calculation.cxx
        include/cone-detection.hxx
        src/cone-detection.cxx
)

target_link_libraries(steering_algo PRIVATE OpenCV)

add_executable (copycurve)
target_include_directories (copycurve PRIVATE include)
target_link_libraries (copycurve PRIVATE scaffolding stubs cluon_engine steering_algo)
target_sources (copycurve PRIVATE src/main.cxx)
if (COVERAGE)
    enable_coverage (copycurve)
endif ()

include (CTest)
add_subdirectory (test)
