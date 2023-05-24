## Parallel Sieve of Eratosthenes for Finding All Prime Numbers within 10^10

## 1. How to run
#### Files and Branch
| Files | description |
|---------|---------| 
| sieve1.c | code after finish Part1 | 
| sieve2.c | code after finish Part2 | 
| sieve3.c | code after finish Part3 | 
| sieve4.c | code after finish Part4 |
| sieve4_update.c | updated version of sieve4.c |
| test_result.md | experimental results of each Part |


#### Compile
```
mpicc <source_code_name> -o <runnable_file>
//example to build program4 
mpicc sieve4.c -o program4
```
#### Runing
#### sieve0~3
you can get the number of the primes which are smaller than **max_number**
```
mpirun -np <process_size> program <max_number> 
//example run program2
mpirun -np 32 program2 1000000000 
```
#### sieve4
sieve4 is a little different, because you can change **cache block size** to increase speed. You can use 32768 which is 32K to set it.
```
mpirun -np <process_size> program <max_number> <cache_block_size>
//example run for part4
mpirun -np 32 program4 10000000000 32768
```
## 2. Implementation
#### Part 2 -  delete even number
We know that, all even number except 2 are not primes, so keep these members in our array is not effcient for memory and for time. We can simply remove all these members include 2, because 2's mutiplies are even, so 2 is no use during seive process.  
Remove even from array can save half of memory. Take a look of how to arrange index.
##### Array Index
```
index:                      0  1  2  3  4  5  6  7  8  9 10
number_before_remove_even:  2  3  4  5  6  7  8  9 10 11 12
number_after_remove_even.   3  5  7  9 11 13 15 17 19 21 23
```
Obviously, with a fixed length array, after removing even number, we can expand the range x2.
##### Parallel segment array
Input: p and n
Output: number of primes within n  
id: rank of processor
We also should know the subarray length to malloc and the lower bound and upper bound of each processor's subarray  
Firstly, we should konw the length of global array. 
```
global_length = (n-3)/2 + 1
```
We use method#2 block decomposition.  
```
First element controlled by i: i*n/p (here n is the global array length)
Last element controlled by process i: (i+1)*n/p-1
```
```
   low_value = 3 + id*((n-1)/2)/p*2;
   high_value = 1 + (id+1)*((n-1)/2)/p*2;
   size = (high_value - low_value)/2 + 1;
```
 
## 3. Experiment
[Full Test Results](https://github.com/jindajia/ParallelPrimeFinder/blob/main/test_result.md)  
### Environment
[**矩阵云**](https://matpool.com)  
Intel Xeon Platinum 8260 CPU  
16vCPU  
RAM（GB）80  
HDD（GB）200  

### Preset
```
Input: n, p
Output: number of primes within n
n = 10^10
p = [2,4,6,8,12,16,24,32,40,48]
```
### Performance
#### Part1 - Adapting a Bigger Range  
We know size of data type **Int** are 4 bytes, it's range is from -2,147,483,648 to  
2,147,483,647, which is not enough to calculate primes within 10^10.  
A **long long type** data  will guaranteed a minumim size with 8bytes. It will cover a range from -2<sup>63</sup> to 2<sup>63</sup> -1.  
Belowed table showing the running time with different number of processors. 
| Processors | 2   | 4   | 6   | 8   | 12  |
|------------|-----|-----|-----|-----|-----|
| Time(sec)  | 217.066358 | 114.980206 | 79.651480 | 60.756687 | 44.913472 |
| Processors | 16  | 24  | 32  | 40  | 48  |
| Time(sec)  | 38.630366 | 44.896393 | 43.708701 | 41.726906 | 45.551066 |

When the number of processors is less than 16, doubling processors roughly reduces the running time to half of the orignal time.  
However, when the number of processes exceeds 16, simply increasing the number of processes does not reduce the execution time; instead, it may cause a slight increase in the execution time.  
**Boundary.** With much more processors, processor0 will take more time to broadcast next prime. Suppose the BroadCast Algorithm is Binary tree algorithm.
For instance, when we only have 4 processors, 2 iterations is enough, when the number of processors is 100, it will take 6 iterations.  
| Iterations | 1 | 2 | 3 | 4 | 5 | 6 |
|------------|---|---|---|---|---|---|
| Max Receivers | 3 | 7 | 15 | 31 | 63 | 127 |

**Two main time-consuming factors**
For a single processor i, it has two main time consuming factors during seiving. 
**1. marking array 2. waiting for new prime**
**Marking Array**
Suppose processor i just receive a new prime **pme** for sieving. Processor will hold an array, it will represent **(n-1)/p** continous number. To sieve **pme**, the for loop will roughly iterate **(n-1)/p/pme** times. Thus, marking will take m seconds.
$$m = (n-1)/p/pme $$ 
**Waiting for new Prime**
When sieving primes, processor0 need to broadcast next prime to all the other processors. 
Suppose each broadcast iteration takes x sec, we have p processors, the last processor will get the message later than the first reciver d seconds. 

$$d = x \left \lfloor  \log_{2}{p}  \right \rfloor$$
**When Pme is Large**
It is evident that the prime values used for sieving will become larger and larger. For n = 10<sup>10</sup>, the maximum prime value used for sieving is 10<sup>5</sup>. Consequently, the marking time decreases as the prime values increase.   
$$m \downarrow \left (  pme\to \sqrt{n} \right ) $$  

However, the broadcasting time is independent of the size of the prime values. As the prime values increase, broadcasting becomes the primary factor contributing to the overall execution time.
**Accumulation of Broadcasting Time**
When n is very large, processor0 will broadcast more time. With all these times accumulated, it may take a lot. 
Suppose we have a  **n = 10<sup>10</sup>**, processor0 need to broadcast all primes within **sqrt(n) = 10<sup>5</sup>** to others. After calculation, there are 1229 primes within 10^5, so processor0 will broadcast  1228 times. The total delay caused by broadcast is D.$$D = 1228 * d = 1228*x \left \lfloor  \log_{2}{p}  \right \rfloor$$
#### Part2 - Deleting even number
The table belowed is the testing result after deleting even number from array.  
Comparing belowed chart, we can find that with same number of processors, Part2's running time is roughly half of Part1's.
| Processors | 2  | 8  | 16 | 32 |
|------------|----|----|----|----|
| Part1      | 217.066358 | 60.756687 | 38.630366 | 43.708701 |
| Part2      | 104.036097 | 30.222937 | 19.798627 | 22.980402 |

##### Calculations
We have already konwn that, two main time-consuming factors are **loop of marking** and **broadcasting**
After removing even number from array. It will reduce the marking time half of the original one. Let's take a look.  
Suppose we have n and p and processor i. It has an array and the length of it is roughly (n-1)/p/2. When marking this array, because there is no even number in the array, so we can add 2*pme each time. The new marking time is m<sub>2</sub>. It's only half of Part1 marking time m.
$$m_{2}  = (n-1)/p/2/pme $$ 
Let's see an example.
```
Suppose n = 100, p = 2 pme = 3
We only care about process 1
Part1: first_element = 51 last element = 100 length = 50
Part2: first_element = 51 last element = 99  length = 25
```
| index | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | ... | 49 |
|-------|----|----|----|----|----|----|----|----|----|----|----|-----|----|
| Part1 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 | 60 | 61 | ... | 100 |
| index | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | ... | 24 |
| Part2 | 51 | 53 | 55 | 57 | 59 | 61 | 63 | 65 | 67 | 69 | 71 | ... | 99 |

Part1 will mark elements representing [51, 54, 57, 60, 63, 66 ... 99], total 16 members.
Part2 will mark elements representing [51,       57,        63,          99], total   9 members.

#### Part3 - Removing Broadcast
One of time-consuming operations is broadcasting. OpenMPI has offered 6 broadcasting algorithms which are basic linear, chain, pipeline, split binary tree, binary tree, binomial tree. In this report, we only care about binary tree algorithm, which is more common and efficient.
When there are more processor, each time of broadcasting will take more iterations. $$iterations =  \left \lfloor  \log_{2}{p}  \right \rfloor$$
because each iteration takes roughly same time, increasing processors will also increasing times on broadcasting. The single broadcast time rises logarithmically.  
Let's take a look of our result.
| Processors | 2  | 4  | 6  | 8  | 12 |
|------------|----|----|----|----|----|
| Part2      | 104.036097 | 55.539209 | 35.828890 | 30.222937 | 24.481947 |
| Part3      | 104.424584 | 55.730943 | 38.704724 | 29.696898 | 21.812350 |
| Processors | 16 | 24 | 32 | 40 | 48 |
| Part2      | 19.798627 | 23.285952 | 22.980402 | 23.521662 | 25.851293 |
| Part3      | 18.949695 | 20.028688 | 19.638265 | 20.145718 | 18.619690 |

We can see that, when the number of  processors are less or equal than 6, Part2 is slightly faster than Part3. However, when processors more than 6, Part3 will save more excution time.  
**Size of Processors** In the previous part, we talked about iteration times when broadcasting messages. When the number of processors is large, each time of broadcasting takes more time.  
**Why Part2 is slightly faster than Part3 when processors is not to many?**
To remove broadcasting, each processor will hold a prime array used for sieving. Thus, not only processor0 need to calculate primes within sqrt(n), all processors should do that.  

*Therefore, when the number of processors is small, the time saved by reducing the broadcasting is not substantial. However, computing all prime numbers less than the square root of n takes more time.*
#### Part4 - Increasing Cache Hit
After Part3, we already removed broadcasting mechanism, so we only care about **time spend on marking**.  
**Time Spend on Marking** 
There are also two factors when marking array. **First**, **how many loops it will take** when marking array is influential. **Second**, when marking an element, we simlpy set this element's value to 1. To do that, we need to access this element. So how long will **a single time of access operation takes**  affecting  marking time.
To reduce the access time, I attempted to increase the L1 cache hit rate. In my running environment, the Intel processor had an L1 cache size of 32K (in bytes). Therefore, I divided the arrays assigned to each process into segments of length 32K, and the test results are as follows.
| Processors | 2  | 4  | 6  | 8  | 12 |
|------------|----|----|----|----|----|
| Part3      | 104.424584 | 55.730943 | 38.704724 | 29.696898 | 21.812350 |
| Part4      | 71.998348 | 37.379297 | 25.333567 | 15.576962 | 13.478113 |
| Processors | 16 | 24 | 32 | 40 | 48 |
| Part3      | 18.949695 | 20.028688 | 19.638265 | 20.145718 | 18.619690 |
| Part4      | 10.231363 | 10.063106 | 9.430258 | 9.483221 | 9.317979 |

We can see that, after increasing hiting rate, the execution time will reduced significanly, when there are 4 processors, the running time will be reduced 33% of the original time. When the number of processors are more than 24, running time will be reduced 50%.  

**Observation** With these results, we can see that, when the amount of processors are  large, the running time takes half of the original time.  

**When number of processors are small, improvement is not as much as when we have a lot of processors** 
When the number of processes is large, increasing the cache hit rate can lead to an efficiency close to 100%. However, why is it that when the number of processes is small, the performance improvement does not reach 100%, but only around 70%?   

My hypothesis is as follows: for a fixed value of `n`, with a larger number of processes, the length of the array assigned to each process becomes relatively shorter. Let's assume that the array length of process `i` is `l`, and the chosen segmentation length is `s` (in this experiment, I set `s` to 32K). Since `s` is fixed, when the number of processes is small, `l` becomes particularly large, resulting in a very small ratio between `s` and `l`.  $$ segment~~  ratio = s:l$$ A smaller `segment ratio`  `s:l` implies a more scattered array segmentation. Consequently, the number of iterations increases relatively. Since at the start of each iteration, there are some works about initilization, some of which can be time-consuming, this leads to some overhead.

**When the lenght of segmented array is too small, the time will increase**
We already found that if we segmented the array length too short, the runtime efficiency would significantly decrease. After analysis, I hypothesized that this phenomenon is mainly caused by an increase in **iterations of the marking operation**. At the beginning of each iteration, certain calculations are required, including: 
**1. Finding the next prime number as a multiplier for the sieve.**  
**2. Performing the multiplication operation `prime * prime` to set the position of the first index.** When the value of `prime` is large, this multiplication operation becomes time-consuming.  
## 4.Improvement of Part4
In the previous part, we observed that, when the length of  segemented array is too small, it will decrease the performance. And one of the two main reason is **For  each outer loop, we need to find the next prime used for sieving** Belowed code is the main operation to find next prime. 
```while (marked[++index]); ```
We already known that for nth prime p<sub>n</sub>, we can get an estimation p<sub>n</sub>
$$p_{n} \approx n\ln_{}{n} ~ (n \to \infty )$$
When n is relatively big, finding next prime will take z times itereation. 
$$z = p_{n+1}- p_{n} \approx \ln_{}{n} ~ (n \to \infty )$$ Because we segmented the original `l`length array to `s`length. Each sgemented array need find next prime used for sieving from `3 to sqrt(n)`, so the total itereation times will approximately be f.
$$ u \approx l/s*(p_{n+1}- p_{n}) ~(n ~from~ 1~ to\sqrt{n}  )$$
**In advance, store the required prime numbers in a contiguous array.** After the improvement, the program yielded the following test results.
| Processors | 2  | 4  | 6  | 8  | 12 |
|------------|----|----|----|----|----|
| Part4      | 71.998348 | 37.379297 | 25.333567 | 15.576962 | 13.478113 |
| Update     | 59.681592 | 30.604949 | 21.212724 | 15.576962 | 10.892023 |
| Processors | 16 | 24 | 32 | 40 | 48 |
| Part4      | 10.231363 | 10.063106 | 9.430258 | 9.483221 | 9.317979 |
| Update     | 8.259007 | 8.499409 | 7.788888 | 7.825964 | 7.800874 |

The improved Part 4 utilizes a contiguous array to pre-store the necessary prime data. We observe a 17% reduction in the algorithm's runtime compared to the original version.  

**Benchmark** 
Depicting the results of these five different test cases in a chart, as shown below.
![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure_1.png)

### Future Improvement Maybe
**Pre store prime*prime**
At the beginning of the marking process, we need to calculate the initial index position using the result of `prime * prime`. We already reorganized the loop structure in Part4 to increase cache hits. The same prime is used up to `l/s` times in each processor.  
**One effective approach is to store the `prime * prime` values for frequently used larger primes, allowing for direct retrieval in subsequent iterations.** 
However, the selection of primes presents a research-worthy question. When primes are small, the time spent searching for the corresponding square value in the hash array may exceed the time required for direct calculation. When primes are large, their probability of being used for sieving decreases.

