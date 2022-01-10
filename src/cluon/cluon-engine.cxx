/*
 *  cluon-engine.cxx
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

#include <condition_variable>
#include <thread>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <cluon-complete.hpp>
#include <opendlv-standard-message-set.hpp>

#include "cluon/cluon-engine.hxx"
#include "scaffolding/frame.hxx"
#include "scaffolding/scaffolding.hxx"

auto parse_commandline(const std::vector<std::string_view> &args) {
    struct Options {
        uint32_t image_width;
        uint32_t image_height;
        std::string shm_name;
        uint16_t cid;
        bool verbose;
        int break_limit = 8;
    } ret;

    std::vector<char *> argv{};
    std::transform(args.begin(), args.end(), std::back_inserter(argv),
                   [](auto view) { return const_cast<char *>(view.data()); });

    const auto parsed_args = cluon::getCommandlineArguments(argv.size(), argv.data());
    const auto has_not = [&](auto... key) { return (!parsed_args.count(key) || ...); };
    if (has_not("cid", "name", "width", "height")) {
        std::cerr << "Required command line arguments:\n";
        std::cerr << " --cid:    CID of the OD4Session to send and receive messages\n";
        std::cerr << " --name:   name of the shared memory area to attach\n";
        std::cerr << " --width:  width of the frame\n";
        std::cerr << " --height: height of the frame\n";
        return std::optional<Options>{};
    }

    ret.image_width = static_cast<uint32_t>(std::stoi(parsed_args.at("width")));
    ret.image_height = static_cast<uint32_t>(std::stoi(parsed_args.at("height")));
    ret.shm_name = parsed_args.at("name");
    ret.cid = static_cast<uint16_t>(std::stoi(parsed_args.at("cid")));
    ret.verbose = parsed_args.count("verbose") != 0;
    if (parsed_args.contains("break-limit"))
        ret.break_limit = std::stoi(parsed_args.at("break-limit"));

    return std::optional{ret};
}

using namespace opendlv::proxy;

bool CluonEngine::run(std::vector<std::string_view> args, std::function<bool(TaskData&&)> on_frame) noexcept {

    const auto opts = parse_commandline(args);
    if (!opts)
        return false;

    std::mutex shm_mut;
    std::unique_lock lk{shm_mut};

    cluon::OD4Session od4{opts->cid};
    if (!od4.isRunning())
        return false;

    std::condition_variable_any shm_cond;
    std::optional<cluon::SharedMemory> shm;

    // Once we get an arbitrary data trigger we can be relative sure the shared memory is available
    // assuming `opendlv-video-h264-replay` is used.
    od4.dataTrigger(AngularVelocityReading::ID(), [&](auto &&msg) {
        if (shm && shm->valid())
            return;
        std::scoped_lock lk{shm_mut};
        shm.emplace(opts->shm_name);

        shm_cond.notify_all();
    });

    shm_cond.wait(lk, [&] {
        return shm && shm->valid();
    });

    Frame empty_frame{.image = {}, .image_width = opts->image_width, .image_height = opts->image_height};
    Frame frame = empty_frame;
    std::mutex frame_mut{};

    auto into = [&od4]<class T, class Out>(auto &out, std::mutex &mut, uint32_t stamp_id, Out (*setter)(const T &)) {
        return od4.dataTrigger(T::ID(), [&, setter, stamp_id](cluon::data::Envelope &&env) {
            if (stamp_id != env.senderStamp())
                return;
            auto res = cluon::extractMessage<T>(std::move(env));
            std::scoped_lock lk{mut};
            out = (*setter)(res);
        });
    };

#define XYZ(type, func) +[](const type& res) { return Vec3<float>{res.func##X(), res.func##Y(), res.func##Z()}; }
#define K(x, y) +[](const x& res) { return res.y(); }
#define N(z, stamp_id, y) frame.z, frame_mut, stamp_id, y
    into(N(acceleration, 0, XYZ(AccelerationReading, acceleration)));
    into(N(angular_acceleration, 0, XYZ(AngularVelocityReading, angularVelocity)));
    into(N(pedal_position, 0, K(PedalPositionRequest, position)));
    into(N(ground_steering, 0, K(GroundSteeringRequest, groundSteering)));
#undef N
#undef K
#undef XYZ

    int breaks = 0;
    while (od4.isRunning()) {

        if(!shm->valid()) {
            if (breaks >= opts->break_limit) {
                std::cerr << "Shm break limit reached, too unstable!\n";
                break;
            }
            std::cerr << "Shm broken! confirming breakage..\n";
            shm.emplace(opts->shm_name);
            if (!shm->valid()) {
                std::cerr << "Shm broken forever..\n";
                break;
            }
            std::cerr << "Shm reconnected!\n";
            ++breaks;
        }

        shm->wait();
        if (!shm->valid())
            continue;
        shm->lock();

        TaskData task_data {.buf = std::pmr::vector<unsigned char>{&mem_pool}};
        task_data.buf.resize(shm->size());
        std::memcpy(task_data.buf.data(), shm->data(), shm->size());

        {
            std::scoped_lock frame_lk{frame_mut};
            frame.timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>{
                    std::chrono::microseconds{cluon::time::toMicroseconds(shm->getTimeStamp().second)}};
            frame.image = {task_data.buf.data(), task_data.buf.size()};

            task_data.frame = frame;
            frame = empty_frame;
        }

        shm->unlock();

        if (opts->verbose) {
            try {
                cv::Mat img{static_cast<int>(task_data.frame.image_height), static_cast<int>(task_data.frame.image_width), CV_8UC4,
                            const_cast<unsigned char*>(task_data.frame.image.data())};
                cv::imshow(shm->name(), img);
                cv::waitKey(1);
            } catch (...) {
                std::cerr << "Failed to open opencv window\n";
            }
        }

        if (!on_frame(std::move(task_data)))
            break;

    }

    std::cerr << "Exiting cluon engine\n";
    return true;
}
