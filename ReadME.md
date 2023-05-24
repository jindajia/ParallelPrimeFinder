## Parallel Sieve of Eratosthenes for Finding All Prime Numbers within 10^10

### 1. How to run
#### Compile
```
mpicc sieve.c -o program
```
#### Runing
#### sieve0~3
you can get the number of the primes which are smaller than **max_number**
```
mpirun -np <process_size> program <max_number> 
```
#### sieve4
sieve4 is a little different, because you can change **cache block size** to increase speed. You can use 32768 which is 32K to set it.
```
mpirun -np <process_size> program <max_number> <cache_block_size>
```
### 2. Part4 - Optimized Version
For Part4, I  segmented the array, each subarray should have L1 cache size number of elements.  This will increase cache hit probability.    

However, reorganized loop will lead to another problem.  

After mark all mutiples of current subarray, we need to get next prime, however when parent_primes size is lage, **<while (parent_primes[++index])>** , this way is not efficient. Because it will be excuted a lot of times.  A better way to do that is using an array to add every primes smaller than sqrt(n);  

sieve4_update is an updated version of sieve4, the only difference is sieve4_update use another array to store primes used to sieve.

### 3. Benchmark
[Test Results](https://github.com/jindajia/ParallelPrimeFinder/blob/main/test_result.md)  
#### Environment
[**矩阵云**](https://matpool.com)
Intel Xeon Platinum 8260 CPU  
16vCPU  
RAM（GB）80  
HDD（GB）200  
![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure_1.png)


