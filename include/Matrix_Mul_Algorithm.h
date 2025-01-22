#pragma once

#include <iostream>
#include <xmmintrin.h>
#include <vector>
#include "WorkerThread.h"

namespace MMA{
    using Elemtype = float;
    /* A:M*K,B:K*N,C:M*N,原始矩阵乘法实现方式 */
    void MatrixMulOrign(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
        for(int i = 0 ;i <M;i++){
            for(int j = 0;j < N;j++){
                for(int k = 0;k < K;k++){
                    C[i * N + j] = A[i * K + k] * B[k * N + j];
                }
            }
        }

    }

    /* 循环展开 */
      void MatrixMulOrign_v0(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
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
    void MatrixMulOrign_v1(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
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
    void MatrixMulOrign_v2(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
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
    void MatrixMulOrign_v3(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
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

    //ElemType* B_ptr = const_cast<ElemType*>(B)+b_base_pos+j;

    /* SSE指令优化+循环展开+调整循环位置+寄存器索引 ，暂时存疑*/
    void MatrixMulOrign_v4(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
        #pragma unroll
        register int a_pos = 0;
        register int c_pos = 0;
        for(int i = 0 ;i < M;i++){
            register int b_pos = 0;
            #pragma unroll
            for(int k = 0;k < K;k += 4){
                //register Elemtype tmp_a = static_cast<Elemtype>(A[a_pos + k]);
                Elemtype* A_ptr = const_cast<Elemtype*>(A)+a_pos + k;
                __m128 a = _mm_loadu_ps(A_ptr);
                __m128 c = _mm_setzero_ps();
                #pragma unroll
                for(int j = 0;j < N;j += 4){
                    //C[c_pos + j] = tmp_a * B[b_pos + j];
                    

                    Elemtype tmp_b[4];
                    tmp_b[0] = B[b_pos + j];
                    tmp_b[1] = B[b_pos + j+1 * N];
                    tmp_b[2] = B[b_pos + j+2 * N];
                    tmp_b[3] = B[b_pos + j+3 * N];

                    __m128 b = _mm_loadu_ps(tmp_b);
                    c = _mm_add_ps(c,_mm_mul_ps(a,b));
                    Elemtype* C_ptr = const_cast<Elemtype*>(C) + c_pos + j;
                    _mm_storeu_ps(C_ptr,c);
                    
                }
                b_pos += N;
                
            }
            a_pos += K;
            c_pos += N;
        }

    }

    /* SSE指令优化+循环展开+调整循环位置+寄存器索引 + 多线程*/
    void MatrixMulOrign_v5(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K,const int ThreadNum){
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

            Workers[i] = new MMA::WorkerThread(MatrixMulOrign_v4, A_new_ptr, B_new_ptr, C_new_ptr, M_NEW, K_NEW, N_NEW);//new出来的注意delete

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
