#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <utility>
#include <functional>

namespace MMA {
    class WorkerThread {
    private:
        std::thread worker;

    public:
        template <typename Func, typename... Args>
        WorkerThread(Func&& func, Args&&... args) {
            worker = std::move(std::thread(std::forward<Func>(func), std::forward<Args>(args)...));
        }

        WorkerThread& operator=(WorkerThread&& other) noexcept {
            if (this != &other) {
                worker = std::move(other.worker);
                std::cout << "Copy Assigning: " << std::endl;
            }
            return *this;
        }

        void join() {
            if (worker.joinable()) {
                worker.join();
            }
        }

        bool joinable() {
            return worker.joinable();
        }

        ~WorkerThread() {
            if (worker.joinable()) {
                worker.join();
            }
        }

        void detach() {
            worker.detach();
        }
    };
}
