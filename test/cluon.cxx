/*
 *  test/cluon.cxx
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

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <random>
#include <boost/process.hpp>
#include <opencv2/core.hpp>
#include <cmath>
#include "scaffolding/scaffolding.hxx"
#include "cluon/cluon-engine.hxx"
#include "copy-curve.hxx"

namespace fs = boost::filesystem;
namespace bp = boost::process;
using namespace std::chrono_literals;

auto get_rec_files() {
    const auto video_path = fs::path{"./videos"};
    REQUIRE(fs::exists(video_path));
    std::vector<fs::path> rec_files;
    for (const auto &path : fs::directory_iterator(video_path)) {
        if (path.path().extension() == ".rec") {
            rec_files.push_back(path.path());
        }
    }
    REQUIRE(!rec_files.empty());
    return std::move(rec_files);
}

auto start_opendlv_replay(int cid, const std::string &shm_name, const fs::path &rec_file) {
    REQUIRE(fs::exists(rec_file));
    auto opendlv_bin = bp::search_path("opendlv-video-h264-replay");
    REQUIRE(!opendlv_bin.empty());
    auto child = bp::child(opendlv_bin, "--cid=" + std::to_string(cid), "--name=" + shm_name, rec_file.string());
//    REQUIRE(child.running());
    return std::move(child);
}

using Result = std::tuple<decltype(Frame::timestamp), float, float>;

auto calc_stats(const std::vector<Result> &data) {
    std::vector<double> proxim_all{};
    proxim_all.reserve(data.size());

    std::transform(data.begin(), data.end(), std::back_inserter(proxim_all), [](const auto &res) {
        const auto &[_, original, calculated] = res;
        const auto proximity = 1 - std::abs(calculated - original) / 0.3;
//        std::cout << original << " vs " << calculated << " = " << proximity * 100 << "%\n";
        return proximity;
    });

    std::vector<double> filtered_proxim_all;
    std::copy_if(proxim_all.begin(), proxim_all.end(), std::back_inserter(filtered_proxim_all), [](auto val) {
        return val > 0.5;
    });

    const auto avrg_proximity =
            std::accumulate(proxim_all.begin(), proxim_all.end(), 0.0, std::plus{}) / proxim_all.size();

    const auto goodness = filtered_proxim_all.size() / static_cast<double>(proxim_all.size());

    std::cout << "Avrg proximity: " << avrg_proximity * 100.0 << "%\n";
    std::cout << "% above 50% : " << goodness * 100 << "%\n";
}


auto write_csv(const fs::path &csv_path, const std::vector<Result> &results) {

    std::stringstream csv;

    for (const auto&[time_stamp, original, calculated] : results) {
        csv << "group09;" << time_stamp.time_since_epoch().count() << ";" << original << ";"
            << calculated
            << "\n";
    }

    const auto base_dir = csv_path.parent_path();

    if (!fs::exists(base_dir))
        fs::create_directories(base_dir);

    std::ofstream file{csv_path, std::ofstream::trunc};
    REQUIRE(file.is_open());

    file << csv.str();
    file.close();

}


auto test_with_cluon(const fs::path &rec_file, auto algorithm, auto callback, unsigned time_limit = 0) {
    std::cerr << "Starting cluon test on: " << rec_file.string() << "\n";
    const auto width = 640;
    const auto height = 480;
    const auto cid = 253;
    const auto shm_name = std::string{"img"};
    std::vector<Result> results{};

    std::optional<decltype(std::chrono::steady_clock::now())> first;

    auto scaffolding = make_scaffolding<CluonEngine>(algorithm, [&](auto calc, const Frame &frame) {
        if (time_limit != 0 && !first)
            first.emplace(std::chrono::steady_clock::now());

        results.emplace_back(frame.timestamp, frame.ground_steering, calc.value_or(0));

        // some badness to allow for void return typed callback
        if constexpr(std::is_convertible_v<std::invoke_result_t<decltype(callback), decltype(calc), decltype(frame)>, bool>) {
            if (!callback(calc, frame))
                return false;
        } else { callback(calc, frame); }


        return time_limit == 0 || (std::chrono::steady_clock::now() - *first < std::chrono::seconds(time_limit));
    });

    auto replay = start_opendlv_replay(cid, shm_name, rec_file);

    const auto res = scaffolding.run(
            {"--cid=" + std::to_string(cid), "--name=" + shm_name, "--width=" + std::to_string(width),
             "--height=" + std::to_string(height)});
    std::cerr << "scaffolding returned \n";
    REQUIRE(res);

    if (!replay.wait_for(std::chrono::seconds(1)))
        replay.terminate();

    replay.wait();

    REQUIRE(!replay.running());

    return std::move(results);
}

TEST_CASE("Cluon - Single Loop") {

    const auto compute = [](auto &) { return 1.0; };
    const auto callback = [&](auto calc, auto &) {
        if (!calc)
            return true;
        REQUIRE(*calc == 1.0);
        return false;
    };

    const auto rec_files = get_rec_files();

    test_with_cluon(rec_files.front(), compute, callback);
}

//TEST_CASE("Cluon - 10 seconds") {
//    const auto rec_files = get_rec_files();
//
//    for (const auto &rec_file : rec_files) {
//        auto compute = [](auto &) { return 0.0; };
//        auto result = test_with_cluon(rec_file, compute, [](auto, auto &) {}, 10);
//    }
//
//}

TEST_CASE("Cluon - CopyCurve") {
    const auto rec_files = get_rec_files();

    std::vector<std::pair<fs::path, std::vector<Result>>> results;

    for (const auto &rec_file : rec_files) {
        auto result = test_with_cluon(rec_file, CopyCurve{}, [](auto, auto &) {}, 0);
        results.emplace_back(rec_file, std::move(result));
        std::cerr << "sleeping\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    for (const auto&[rec_file, result] : results) {
        std::cout << rec_file.string() << ":\n";
        calc_stats(result);
        write_csv(fs::path{"./results"} / (rec_file.stem().string() + "-CopyCurve.csv"), result);
    }
}
