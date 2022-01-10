/*
 *  scaffolding.hxx
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

#ifndef COPYCURVE_SCAFFOLDING_HXX
#define COPYCURVE_SCAFFOLDING_HXX

#include <functional>
#include <memory>
#include <string_view>
#include <vector>
#include <deque>
#include <variant>
#include <future>
#include <thread>
#include <memory_resource>
#include <stop_token>
#include "scaffolding/frame.hxx"
#include "util.hxx"

struct TaskData {
    std::pmr::vector<unsigned char> buf;
    Frame frame;
};

struct ResultData {
    TaskData task_data;
    std::optional<double> calculated;
};

template<class Engine, class Compute, class Callback>
class Scaffolding : public Engine {
    Callback m_callback;
    Compute m_compute;

    using QueueItem = std::variant<TaskData, std::future<ResultData>, ResultData>;
    std::deque<QueueItem> m_work_queue;

    std::mutex m_work_queue_mut;
    std::condition_variable m_work_queue_cv;
    std::condition_variable_any m_work_cv;
    std::condition_variable_any m_collect_cv;

    std::jthread m_worker{[&](auto tk) { worker_func(tk); }};
    std::jthread m_collector{[&](auto tk) { collect_results(tk); }};

    std::atomic_bool continue_work;

public:
    bool run(std::size_t argc, char **argv) {
        std::vector<std::string_view> args{};
        args.reserve(argc);
        std::copy_n(argv, argc, std::back_inserter(args));
        return run(args);
    }

    bool run(const std::vector<std::string_view> &args) {
        continue_work = true;
        const auto ret = static_cast<Engine &>(*this).run(args,
                                                          std::bind(&Scaffolding::on_frame, this,
                                                                    std::placeholders::_1));
        std::unique_lock lk{m_work_queue_mut};
        m_work_queue_cv.wait(lk, [&] {
            return m_work_queue.empty();
        });
        return ret;
    }

    explicit Scaffolding(Compute &&compute, Callback &&callback)
            : m_compute{std::forward<Compute>(compute)}, m_callback{std::forward<Callback>(callback)} {
    }

    void worker_func(std::stop_token stop_token) {
        while (!stop_token.stop_requested()) {

            std::unique_lock lk{m_work_queue_mut};
            m_work_cv.wait(lk, stop_token,
                           [&] { return !m_work_queue.empty(); });

            if (stop_token.stop_requested())
                return;

            std::visit(Visitor{
                    [&](TaskData &task_data) {
                        auto task = std::move(task_data);
                        std::promise<ResultData> p;
                        m_work_queue.back() = p.get_future();
                        lk.unlock();
                        m_work_queue_cv.notify_all();
                        const auto res = m_compute(task.frame);
                        p.set_value(ResultData{.task_data = std::move(task), .calculated = res});
                        m_collect_cv.notify_all();
                    },
                    [](auto &&...) {}
            }, m_work_queue.back());
        }
    }

    void collect_results(std::stop_token stop_token) {
        while (!stop_token.stop_requested()) {

            std::unique_lock lk{m_work_queue_mut};
            m_collect_cv.wait(lk, stop_token, [&] { return !m_work_queue.empty(); });

            if (stop_token.stop_requested())
                return;

            bool modified = false;
            while (!m_work_queue.empty()) {
                auto val = std::visit(Visitor{
                        [&](ResultData &data) {
                            return std::optional{std::move(data)};
                        },
                        [&](std::future<ResultData> &data) {
                            if (data.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                                return std::optional{std::move(data.get())};
                            return std::optional < ResultData > {};
                        },
                        [](auto &&...) { return std::optional < ResultData > {}; }
                }, m_work_queue.front());

                if (!val)
                    break;
                modified = true;
                const auto stop = std::invoke(m_callback, val->calculated, val->task_data.frame);
                m_work_queue.pop_front();
                if (!stop) {
                    continue_work = stop;
                    m_work_queue.clear();
                }
            }
            lk.unlock();
            if (modified)
                m_work_queue_cv.notify_all();
        }
    }

    bool on_frame(TaskData &&frame) {
        if (!continue_work)
            return false;
        {
            std::scoped_lock lk{m_work_queue_mut};
            if (!m_work_queue.empty()) {
                std::visit(Visitor{
                        [&](TaskData &data) {
                            m_work_queue.back() = ResultData{.task_data = std::move(data)};
                        },
                        [](auto &&...) {}
                }, m_work_queue.back());
            }
            m_work_queue.emplace_back(std::move(frame));
        }

        m_work_cv.notify_one();
        m_work_queue_cv.notify_all();

        return continue_work;
    }
};

template<class Engine, class Compute, class Callback>
auto make_scaffolding(Compute &&compute, Callback &&callback) {
    return Scaffolding<Engine, Compute, Callback>{std::forward<Compute>(compute), std::forward<Callback>(callback)};
}

#endif // CC_SCAFFOLDING_HXX