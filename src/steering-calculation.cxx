/*
 *  steering-calculation.cxx
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

#include "steering-calculation.hxx"
#include <cmath>
#include <algorithm>
#include <limits>
#include <optional>

float steering_angle(const std::vector<cv::Rect> &contours_left, const std::vector<cv::Rect> &contours_right) {
    if (contours_left.empty() && contours_right.empty())
        return 0;

    int left_point = 0;
    int right_point = 0;

    const auto empty_int = std::optional < int > {std::nullopt};
    auto x_left = empty_int;
    auto y_left = empty_int;
    auto x_right = empty_int;
    auto y_right = empty_int;

    size_t left_min = 1000;
    if (!contours_left.empty()) {
        for (size_t i = 0; i < contours_left.size(); i++) {
            if (contours_left[i].tl().y < left_min) {
                left_min = contours_left[i].tl().y;
                left_point = i;
            }
        }
        x_left = contours_left[left_point].tl().x;
        y_left = contours_left[left_point].tl().y;
    }

    size_t right_min = 1000;
    if (!contours_right.empty()) {
        for (size_t i = 0; i < contours_right.size(); i++) {
            if (contours_right[i].tl().y < right_min) {
                right_min = contours_right[i].tl().y;
                right_point = i;
            }
        }
        x_right = contours_right[right_point].tl().x;
        y_right = contours_right[right_point].tl().y;
    }

    auto x_diff = 0.0f;
    if (x_left && x_right) {
        x_diff = (*x_right - *x_left) / 2.0f;
    } else if (x_left) {
        x_diff = (720 - *x_left);
    } else if (x_right) {
        x_diff = *x_right;
    }

    auto right_angle = 0.0f;
    auto left_angle = 0.0f;
    constexpr auto float_max = std::numeric_limits<float>::max();
    if (y_right && y_left) {
        right_angle = std::atan(*y_right / std::clamp(x_diff, 1.0f, float_max));
        left_angle = std::atan(*y_left / std::clamp(x_diff, 1.0f, float_max));
    } else if (y_left) {
        left_angle = std::atan(*y_left / std::clamp(x_diff, 1.0f, float_max));
        right_angle = left_angle * 1.15f;
    } else if (y_right) {
        right_angle = std::atan(*y_right / std::clamp(x_diff, 1.0f, float_max));
        left_angle = right_angle * 1.15f;
    }

    auto res = static_cast<float>(left_angle - right_angle);
    return res;
}
