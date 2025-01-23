#include <iostream>
#include <string>

#include "MMAClass.h"
#include "WorkerThread.h"
#include "Matrix_Mul_Algorithm.h"

#define ROLL_NUM 100

int main(int arg,const char* argv[]){
    if(arg < 5){
        std::cout<<"至少需要4个参数"<<std::endl;
        return -1;
    }
    const int M = atoi(argv[1]);
    const int K = atoi(argv[2]);
    const int N = atoi(argv[3]);
    const int ThreadNum = atoi(argv[4]);

    if(M<=0 || N <= 0 || K <= 0||ThreadNum <= 0){
        std::cout<<"参数设置有误"<<std::endl;
        return -1;
    }

    if(ThreadNum == 1)/* 单线程,SSE加速 */
    {
        MMA::MMAClass<float> sample(M,K,N);

        std::cout<<"开始测试:单线程,SSE加速"<<std::endl;

        sample.MMACompute(MMA::MatrixMulOrign_v4,1,ROLL_NUM);

        std::cout<<"测试结束"<<std::endl;

        

    }
    else/* 多线程 */
    {
        MMA::MMAClass<float> sample(M,K,N);

        std::cout<<"开始测试:多线程,SSE加速+"<<ThreadNum<<"个线程"<<std::endl;

        sample.MMACompute(MMA::MatrixMulOrign_v5,ThreadNum,ROLL_NUM);

        std::cout<<"测试结束"<<std::endl;
    }

    return 0;

}