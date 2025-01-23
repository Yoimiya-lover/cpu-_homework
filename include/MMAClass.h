#pragma once

#include <iostream>
#include <ctime>
#include <functional>
#include <algorithm>

namespace MMA{
    template<typename T>
    class MMAClass{
        private:
            /* 各个矩阵大小 */
            int _A_size;
            int _B_size;
            int _C_size;

            /* 存放矩阵地方 */
            T* _A_data;
            T* _B_data;
            T* _C_data;
            const int _M;
            const int _N;
            const int _K;

            //初始化为随机数
            void InitMatric(int size,T* data){
                for(int i = 0;i < size;i++){
                    data[i] = static_cast<T>(rand());
                }
            }
            /* 初始化为val */
            void InitMatric(int size,T* data,int val){
                for(int i = 0;i < size;i++){
                    data[i] = static_cast<T>(val);
                }
            }

        
        public:
            MMAClass(int M,int K,int N):_M(M),_K(K),_N(N){
                _A_size = _M * _K;
                _B_size = _K * _N;
                _C_size = _M * _N;

                std::cout<<"开始创建矩阵"<<std::endl;
                _A_data = new T[_A_size];
                _B_data = new T[_B_size];
                _C_data = new T[_C_size];
                std::cout<<"创建完成"<<std::endl;

                std::cout<<"初始化矩阵"<<std::endl;
                InitMatric(_A_size,_A_data);
                InitMatric(_B_size,_B_data);
                InitMatric(_C_size,_C_data,0);
                std::cout<<"初始化完成"<<std::endl;
            }
            ~MMAClass(){
                delete[] _A_data;
                delete[] _B_data;
                delete[] _C_data;
                std::cout<<"删除矩阵成功"<<std::endl;
            }
            template<typename Func>
            void MMACompute(Func func,int Thread_num,int num){
                InitMatric(_C_size,_C_data,0);
                std::clock_t c_start = std::clock();
                for(int i = 0;i < num ;i++){
                    Compute(func,Thread_num);
                }
                std::clock_t c_end = std::clock();
                float time_cost = (c_end - c_start) / CLOCKS_PER_SEC / num * 1000;
                std::cout<<"-----------------------------"<<std::endl;
                std::cout<<"线程数为"<<Thread_num <<" 所耗费的时间为: "<<time_cost<<"ms"<<std::endl;
                std::cout<<"-----------------------------"<<std::endl;

                void MMAOrigin();
                
            }

            template<typename Func>
            void Compute(Func func,int Thread_num){
                func(this->_A_data,this->_B_data,this->_C_data,this->_M,this->_N,this->_K,Thread_num);
            }

            // template <typename Func>
            // bool CheckMulMul(Func func,int Thread_num){
            //     std::cout<<"检查计算是否有问题"<<std::endl;
            //     //InitMatric(_C_size,_C_size,0);
            //     Compute(func,Thread_num);
            //     return MMAOrigin();
            // }

            void MMAOrigin(){
                #pragma unroll
                for(int i = 0 ;i <_M;i++){
                    #pragma unroll
                    for(int j = 0;j < _N;j++){
                        #pragma unroll
                        register T tmp_c = static_cast<T>(0);/* 使用静态转换 */
                        for(int k = 0;k < _K;k++){
                            tmp_c += _A_data[i * _K + k] * _B_data[k * _N + j];
                        }
                        if(_C_data[i * _N + j] != tmp_c){
                            std::cout<<"计算有误"<<"C["<<i<<"]"<<"["<<j<<"]"<<std::endl;
                            //return false;
                        }
                    }
                }
                std::cout<<"计算无误"<<std::endl;
                //return true;

            }

    };

}