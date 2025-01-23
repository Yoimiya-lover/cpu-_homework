#矩阵乘法的CPU性能优化

mkdir build

cd build

cmake .. && make -j && make install

cd bin

./cpu_test 1024 128 1024 1
