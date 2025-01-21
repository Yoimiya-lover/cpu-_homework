#include <iostream>
#include <xmmintrin.h>
#include <thread>
#include <vector>

void MatrixMul_SSE(float A[1024][128],float B[128][1024],float C[1024][1024],int N,const int start,const int end){

    for(int i = start;i < end ;i++){
        for(int j = 0;j < N;j++){
            __m128 c = _mm_setzero_ps();
            for(int k = 0;k < N;k += 4){
                __m128 a = _mm_loadu_ps(&A[i][k]);/* 一次加载四个数 */

                float temp[4];
                for(int s = 0;s <4 ;s++)
                {
                    temp[s] = B[k + s][j];
                }
                __m128 b = _mm_loadu_ps(temp);

                c = _mm_add_ps(_mm_mul_ps(a,b),c);
            }

            float temp_sum[4];
            _mm_storeu_ps(temp_sum,c);

            for(int k = 0;k < 4;k++){
                C[i][j] += temp_sum[k];
            }

            for(int k = N - N % 4 ;k < N; k++){
                C[i][j] += A[i][k] * B[k][j];
            }


        }
    }


}

void Thread_Martrix_Mul(float A[1024][128],float B[128][1024],float C[1024][1024],int threadNum,int N){
     std::vector<std::thread> threads;
     int block_size = N / threadNum;

     for(int i = 0; i < threadNum; i++){
        int start = i * block_size;
        int end = (i == threadNum - 1) ? N : start + block_size ;
        threads.emplace_back(MatrixMul_SSE,A,B,C,1024,start,end);
     }

     for(int i = 0 ;i < threadNum; i++){
        threads[i].join();
     }


}

int main(){
    float A[1024][128];
    float B[128][1024];
    
    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 128; ++j) {
            A[i][j] = 1.0f;  
        }
    }
    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 1024; ++j) {
            B[i][j] = 1.0f;  
        }
    }

    float C[1024][1024] = {0};

    Thread_Martrix_Mul(A,B,C,4,1024);
    std::cout << "Result Matrix C:" << std::endl;
    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 1024; ++j) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;



}