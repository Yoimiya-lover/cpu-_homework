#pragma once

#include <iostream>
#include <xmmintrin.h>
#include <vector>
#include "WorkerThread.h"

namespace MMA{
    using Elemtype = float;
    /* A:M*K,B:K*N,C:M*N,原始矩阵乘法实现方式 */
    void MatrixMulOrign(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N){
        for(int i = 0 ;i <M;i++){
            for(int j = 0;j < N;j++){
                for(int k = 0;k < K;k++){
                    C[i * N + j] = A[i * K + k] * B[k * N + j];
                }
            }
        }

    }

    /* 循环展开 */
      void MatrixMulOrign_v0(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N){
        #pragma unroll
        for(int i = 0 ;i <M;i++){
            #pragma unroll
            for(int j = 0;j < N;j++){
                #pragma unroll
                for(int k = 0;k < K;k++){
                    C[i * N + j] = A[i * K + k] * B[k * N + j];
                }
            }
        }

    }

    /* 循环展开+寄存器 */
    void MatrixMulOrign_v1(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N){
        #pragma unroll
        for(int i = 0 ;i <M;i++){
            #pragma unroll
            for(int j = 0;j < N;j++){
                #pragma unroll
                register Elemtype tmp_c = static_cast<Elemtype>(0);/* 使用静态转换 */
                for(int k = 0;k < K;k++){
                    tmp_c += A[i * K + k] * B[k * N + j];
                }
                C[i * N + j] = tmp_c;
            }
        }

    }

    /* 循环展开+调整循环位置+寄存器值 */
    void MatrixMulOrign_v2(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N){
        #pragma unroll
        for(int i = 0 ;i < M;i++){
            #pragma unroll
            for(int k = 0;k < K;k++){
                register Elemtype tmp_a = static_cast<Elemtype>(A[i * K + k]);
                #pragma unroll
                for(int j = 0;j < N;j++){
                    C[i * N + j] = tmp_a * B[k * N + j];
                }
                
            }
        }

    }

    /* 循环展开+调整循环位置+寄存器索引 */
    void MatrixMulOrign_v3(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N){
        #pragma unroll
        register int a_pos = 0;
        register int c_pos = 0;
        for(int i = 0 ;i < M;i++){
            register int b_pos = 0;
            #pragma unroll
            for(int k = 0;k < K;k++){
                register Elemtype tmp_a = static_cast<Elemtype>(A[a_pos + k]);
                #pragma unroll
                for(int j = 0;j < N;j++){
                    C[c_pos + j] = tmp_a * B[b_pos + j];
                }
                b_pos += N;
                
            }
            a_pos += K;
            c_pos += N;
        }

    }

    //Elemtype* B_ptr = const_cast<Elemtype*>(B)+b_base_pos+j;

    /* SSE指令优化+循环展开+调整循环位置+寄存器索引 ，A矩阵按列访问，B矩阵按照访问 */
    void MatrixMulOrign_v4(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N,const int Thread_Num){
    register int b_base_pos = 0;

    #pragma unroll
    for(int k = 0; k < K; k++) {
        register int a_base_pos = 0;
        register int c_base_pos = 0;

        #pragma unroll
        for(int i = 0; i < M; i++) {
            register Elemtype tmp_ar = static_cast<Elemtype>(A[a_base_pos+k]);
            __m128 tmp_a = _mm_setr_ps(tmp_ar, tmp_ar, tmp_ar, tmp_ar);
            #pragma unroll
            for(int j = 0; j < N; j += 4) {
                
                // 依赖关系：1->3->5->6->7; 2->4->6->7。所以在这里指令1,3,5和指令2,4交叉执行以换取指令流水线最大的吞吐量
                Elemtype* B_ptr = const_cast<Elemtype*>(B)+b_base_pos+j;     //instruction 1
                Elemtype* C_ptr = const_cast<Elemtype*>(C)+c_base_pos+j;     //instruction 2

                __m128 tmp_b = _mm_loadu_ps(B_ptr);         //instruction 3
                __m128 tmp_c = _mm_loadu_ps(C_ptr);         //instruction 4
                __m128 mul_c = _mm_mul_ps(tmp_a, tmp_b);    //instruction 5
                tmp_c = _mm_add_ps(mul_c, tmp_c);           //instruction 6
                _mm_storeu_ps(C_ptr, tmp_c);                //instruction 7
            }

            a_base_pos += K;
            c_base_pos += N;
        }
        b_base_pos += N;
    }

    }

    /* SSE指令优化+循环展开+调整循环位置+寄存器索引 + 多线程*/
    void MatrixMulOrign_v5(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int K,const int N,const int ThreadNum){
        const int row_block_size = (M + ThreadNum - 1) / ThreadNum;//向上取整，每个线程处理行数
        std::vector<MMA::WorkerThread*> Workers(ThreadNum);
        for(int i = 0;i < ThreadNum;i++){
            int cur_pos = i * row_block_size;//cur_pos为每个线程开始的行数
            Elemtype* A_new_ptr = &A[cur_pos * K];
            Elemtype* B_new_ptr = B;
            Elemtype* C_new_ptr = &C[cur_pos * N];
            int M_NEW = std::min(cur_pos + row_block_size,M) - cur_pos;
            int K_NEW = K;
            int N_NEW = N;
            //std::cout<< i << "---> 输出A_new_ptr="<<A_new_ptr << "; B_new_ptr=" << B_new_ptr << "C_new_ptr="<< C_new_ptr <<std::endl;
            Workers[i] = new MMA::WorkerThread(MatrixMulOrign_v4, A_new_ptr, B_new_ptr, C_new_ptr, M_NEW, K_NEW, N_NEW, 1);//new出来的注意delete

        }
        for(int i = 0;i < ThreadNum;i++)
        {
            Workers[i]->join();
        }
        for(int i = 0;i < ThreadNum;i++)
        {
            delete Workers[i];
        }
        
    }







}
