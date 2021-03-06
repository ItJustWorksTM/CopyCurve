/*
 *  stub-engine.hxx
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

#ifndef COPYCURVE_STUB_ENGINE_HXX
#define COPYCURVE_STUB_ENGINE_HXX

#include <functional>
#include <string_view>
#include <vector>

class TaskData;

struct StubEngine {
    bool run(std::vector<std::string_view> args, std::function<bool(TaskData&&)> on_frame) noexcept;
};

#endif // COPYCURVE_STUB_ENGINE_HXX
