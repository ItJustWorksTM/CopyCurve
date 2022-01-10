/*
 *  cone-detection.cxx
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

#include "cone-detection.hxx"

std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>> get_color_mask(cv::Mat &img) {
    const auto yellow_lower_range = cv::Scalar{17, 138, 26};
    const auto yellow_upper_range = cv::Scalar{75, 242, 255};

    const auto blue_lower_range = cv::Scalar{43, 138, 26};
    const auto blue_upper_range = cv::Scalar{141, 242, 255};

    const auto[width, height] = img.size();
    cv::rectangle(img, {0, static_cast<int>(height / 1.28)}, {width, height}, {0, 0, 0}, -1);
    cv::rectangle(img, {0, 0}, {width, height / 2}, {0, 0, 0}, -1);

    cv::Mat img_hsv;
    cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);

    const auto get_range = [&](auto &lower, auto &upper) {
        cv::Mat tmp;
        cv::Mat ret;
        cv::inRange(img_hsv, lower, upper, tmp);
        cv::GaussianBlur(tmp, ret, {11, 11}, 0, 0);
        return ret;
    };

    auto yellow_mask = get_range(yellow_lower_range, yellow_upper_range);
    auto blue_mask = get_range(blue_lower_range, blue_upper_range);
    return {cone_contours(blue_mask), cone_contours(yellow_mask)};
}

std::vector<cv::Rect> cone_contours(cv::Mat &mask) {
    constexpr auto threshold = 100;

    cv::Mat canny_output;
    cv::Canny(mask, canny_output, threshold, threshold * 2);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(canny_output, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    std::vector<cv::Rect> boundRect(contours.size());
    // Debug
    cv::Mat drawing{mask};
    for (size_t i = 0; i < contours.size(); i++) {
        std::vector<cv::Point> poly;
        cv::approxPolyDP(contours[i], poly, 3, true);
        boundRect[i] = cv::boundingRect(poly);
        // Debug
        cv::drawContours(drawing, contours, static_cast<int>(i), {255, 255, 255}, 2, cv::LINE_8, hierarchy,
                         0);
        cv::rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), {255, 255, 255}, 2);
    }

    return std::move(boundRect);
}
