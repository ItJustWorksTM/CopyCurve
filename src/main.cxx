/*
 *  main.cxx
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

#include <cstdlib>
#include <iostream>
#include "scaffolding/scaffolding.hxx"
#include "copy-curve.hxx"
#include "cluon/cluon-engine.hxx"

int main(int argc, char **argv) {
    auto scaffold = make_scaffolding<CluonEngine>(
            CopyCurve{},
            [&](auto res, auto &frame) {
                std::cout << "group_09;" << frame.timestamp.time_since_epoch().count() << ";" << res.value_or(0)
                          << '\n';
                return true;
            });
    return scaffold.run(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
