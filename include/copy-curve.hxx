/*
 *  test/copy-curve.hxx
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

#ifndef COPYCURVE_COPY_CURVE_HXX
#define COPYCURVE_COPY_CURVE_HXX

#include "scaffolding/scaffolding.hxx"
#include "steering-calculation.hxx"
#include "cone-detection.hxx"

struct CopyCurve {
    bool checked_dir = false;
    bool blue_left = true;

    float operator()(const Frame &frame) {
        cv::Mat img{static_cast<int>(frame.image_height), static_cast<int>(frame.image_width), CV_8UC4,
                    const_cast<unsigned char *>(frame.image.data())};
        auto[contours_blue, contours_yellow] = get_color_mask(img);

        // hack to find what color is left
        if (!checked_dir && !contours_blue.empty()) {
            blue_left = contours_blue[0].tl().x < 360;
            checked_dir = true;
        }

        if (blue_left)
            return steering_angle(contours_blue, contours_yellow);
        else
            return steering_angle(contours_yellow, contours_blue);

    }
};

#endif //COPYCURVE_COPY_CURVE_HXX
