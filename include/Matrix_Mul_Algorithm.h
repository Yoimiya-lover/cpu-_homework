#pragma once

#include <iostream>
#include <xmmintrin.h>
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

    /* 循环展开+调整循环位置+寄存器 */
    void MatrixMulOrign_v2(Elemtype* A,Elemtype* B,Elemtype* C,const int M,const int N,const int K){
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





}
