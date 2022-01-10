/*
 *  cone-detection.hxx
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

#ifndef COPYCURVE_CONE_DETECTION_HXX
#define COPYCURVE_CONE_DETECTION_HXX

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

std::pair<std::vector<cv::Rect>,std::vector<cv::Rect>> get_color_mask(cv::Mat& img);
std::vector<cv::Rect> cone_contours(cv::Mat& mask);

#endif // COPYCURVE_CONE_DETECTION_HXX
