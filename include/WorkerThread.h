#pragma once

#include <iostream>
#include <vector>
#include <thread>

namespace MMA{
    class WorkerThread{  
        private:
            std::thread worker;

        public:
            /* typename... Args是不定参数 
                std::forward<Func>(func)完美转发
            */
            template<typename Func, typename... Args>
            WorkerThread(Func&& func,Args&&... args){
                worker = std::move(std::thread(std::forward<Func>(func),std::forward<Args>(args)...));
            }

            WorkerThread& operator=(WorkerThread&& other) noexcept{
                if(this != &other)
                {
                    worker = std::move(other.worker);
                    std::cout << "Copy Assigning: " << std::endl;
                }
                return *this;
            }


            WorkerThread& operator=(WorkerThread&& other) noexcept {
            if (this != &other) {
                worker = std::move(other.worker);
                std::cout << "Copy Assigning: " << std::endl;
                }
            return *this;
            }

            void join(){
                if(worker.joinable()){
                    worker.join();
                }

            }

            bool joinable(){
                return worker.joinable();
            }

            ~WorkerThread(){
                if(worker.joinable()){
                    worker.join();
                }
            }

            void detach(){
                worker.detach();
            }
    };

    
}