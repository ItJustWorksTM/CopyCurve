/*
 *  util.hxx
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

#ifndef COPYCURVE_UTIL_HXX
#define COPYCURVE_UTIL_HXX

#include <type_traits>

template<class... Base>
struct Visitor : Base ... {
    template<class... Ts>
    constexpr Visitor(Ts &&... ts) noexcept((std::is_nothrow_move_constructible_v<Base> && ...)) : Base{
        std::forward<Ts>(ts)}... {}

    using Base::operator()...;
};

template<class... Ts>
Visitor(Ts...) -> Visitor<std::remove_cvref_t<Ts>...>;

#endif //COPYCURVE_UTIL_HXX
