#pragma once

#include <iostream>
#include <vector>
#include <thread>

namespace MMA{
    class WorkerThread{  
        private:
            std::thread worker;

        public:
            template <typename Func, typename... Args>
            WorkerThread(Func&& func, Args&&... args) {
            worker = std::move(std::thread(std::forward<Func>(func), std::forward<Args>(args)...));
            }

            void join(){
                
            }
    };

    
}