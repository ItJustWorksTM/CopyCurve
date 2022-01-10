/*
 *  frame.hxx
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

#ifndef COPYCURVE_FRAME_HXX
#define COPYCURVE_FRAME_HXX

#include <chrono>
#include <optional>
#include <string_view>

template<class T>
struct Vec3 {
    T x;
    T y;
    T z;
};

struct Frame {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> timestamp;
    std::basic_string_view<unsigned char> image;
    uint32_t image_width;
    uint32_t image_height;

    float pedal_position;
    float ground_steering;

    Vec3<float> acceleration;
    Vec3<float> angular_acceleration;
};

#endif // COPYCURVE_FRAME_HXX
