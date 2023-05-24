(myconda) root@PbjbMD:~/Desktop/git/ParallelPrimeFinder# mpicc sieve0.c -o my_program -lm
(myconda) root@PbjbMD:~/Desktop/git/ParallelPrimeFinder# mpirun --allow-run-as-root -np 4 my_program 10000000000
There are 455052511 primes less than or equal to 10000000000
SIEVE (4)  93.368185
(myconda) root@PbjbMD:~/Desktop/git/ParallelPrimeFinder# mpirun --allow-run-as-root -np 8 my_program 10000000000
There are 455052511 primes less than or equal to 10000000000
SIEVE (8)  52.963621
(myconda) root@PbjbMD:~/Desktop/git/ParallelPrimeFinder# mpirun --allow-run-as-root -np 2 my_program 10000000000
There are 455052511 primes less than or equal to 10000000000
SIEVE (2) 180.255660

jiajindadeMacBook-Pro:ParallelPrimeFinder jiajinda$ mpirun -np 2 program 1000000000
There are 50847534 primes less than or equal to 1000000000
SIEVE (2)  11.846289
jiajindadeMacBook-Pro:ParallelPrimeFinder jiajinda$ mpirun -np 1 program 1000000000
There are 0 primes less than or equal to 1000000000
SIEVE (1)  17.345623

Environment:
矩阵云 https://matpool.com/
Intel Xeon Platinum 8260 CPU
16vCPU*小时
内存（GB）
80
硬盘（GB）
200

Openmpi 2.1.1
遇到问题：时间并不会根据process的变多而减少，因为process的rank不是0，1，2，3这样的名字

Original:
n = 10^9
process = 2
time = 20.002139

Part1:

n = 10^9
process = 4
time = 10.090305
process = 6
time = 7.226855
process = 8
time = 6.178213
process = 12
time = 4.552182
process = 16
time = 3.559303
process = 24
time = 4.424872
process = 32
time = 4.809480
process = 40
time = 4.954141
process = 48
time = 5.166163

n = 10^10
process = 2
time = 217.066358
process = 4
time = 114.980206
process = 6
time = 79.651480
process = 8
time = 60.756687
process = 12
time = 44.913472
process = 16
time = 38.630366
process = 24
time = 44.896393
process = 32
time = 43.708701
process = 40
time = 41.726906
process = 48
time = 45.551066

Part2
n = 10^10
process = 2
time = 104.036097
process = 4
time = 55.539209
process = 6
time = 35.828890
process = 8
time = 30.222937
process = 12
time = 24.481947
process = 16
time = 19.798627
process = 24
time = 23.285952
process = 32
time = 22.980402
process = 40
time = 23.521662
process = 48
time = 25.851293

Part3
n = 10^10
process = 2
time = 104.424584
process = 4
time = 55.730943
process = 6
time = 38.704724
process = 8
time = 29.696898
process = 12
time = 21.812350
process = 16
time = 18.949695
process = 24
time = 20.028688
process = 32
time = 19.638265
process = 40
time = 20.145718
process = 48
time = 18.619690

Part4
n = 10^10
cache size = 32768

version: normal
process = 2
time = 71.998348
process = 4
time = 37.379297
process = 6
time = 25.333567
process = 8
time = 15.576962
process = 12
time = 13.478113
process = 16
time = 10.231363
process = 24
time = 10.063106
process = 32
time = 9.430258
process = 40
time = 9.483221
process = 48
time = 9.317979


version: update
process = 2
time = 59.681592
process = 4
time = 30.604949
process = 6
time = 21.212724
process = 8
time = 15.576962
process = 12
time = 10.892023
process = 16
time = 8.259007
process = 24
time = 8.499409
process = 32
time = 7.788888
process = 40
time = 7.825964
process = 48
time = 7.800874