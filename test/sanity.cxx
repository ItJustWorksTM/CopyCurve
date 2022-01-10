/*
 *  test/sanity.cxx
 *  Copyright 2021 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <climits>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/opencv.hpp>

TEST_CASE("Sanity") {
    REQUIRE(CHAR_BIT == 8);
    REQUIRE(CV_VERSION_MAJOR > 0);
}
