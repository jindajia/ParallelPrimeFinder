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
| slurm1.job | slurm command to run on RM-shared with 8 cores |
| slurm2.job | slurm command to run on RM-shared with 64 cores |
| slurm3.job | slurm command to run on RM with 4 nodes |
| slurm-1.out | slurm1 result |
| slurm-2.out | slurm2 result |
| slurm-3.out | slurm3 result |

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
id: rank of process
We also should know the subarray length to malloc and the lower bound and upper bound of each process's subarray  
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
**Full Test Result**

If you want to check full test result, please read **slurm-1.out, slurm-2.out, slurm-3.out**

### Environment
[**Bridges-2 Regular Memory**](https://www.psc.edu/resources/bridges-2/)  

**For Signle RM Node**
-   NVMe SSD (3.84TB)
-   Mellanox ConnectX-6 HDR Infiniband 200Gb/s Adapter
-   Two AMD EPYC 7742 CPUS, each with:
    -   64 cores
    -   2.25-3.40GHz
    -   256MB L3
    -   8 memory channels  

**For RM-shared Node**
- Core count default: 1
- Core count max: 64
  

### Preset
```
Input: n, p
Output: number of primes within n
n = 10^10
p = [2, 4, 6, 8, 16, 24, 32, 40, 48, 64, 128, 256, 384, 512]
```
### Performance
#### Part1 - Adapting a Bigger Range  
We know size of data type **Int** are 4 bytes, it's range is from -2,147,483,648 to  
2,147,483,647, which is not enough to calculate primes within 10^10.  
A **long long type** data  will guaranteed a minumim size with 8bytes. It will cover a range from -2<sup>63</sup> to 2<sup>63</sup> -1.  
Belowed table showing the running time with different number of cores. 
| Cores | 2   | 4   | 6   | 8   | 16  | 24  | 32  | 40  | 48  | 64  | 128 | 256 | 384 | 512 |
|------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Time(sec)  | 131.340222 | 74.648559 | 69.071211 | 65.641554 | 33.30611 | 22.234583 | 19.148118 | 15.87571 | 14.331761 | 12.111386 | 6.971425 | 3.155523 | 2.10833 | 1.520438 |
![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part1.png)It can be observed that as the number of cores increases, the running time decreases. 
In an ideal scenario, when we double the number of cores, we expect the program's running time to be halved, or in other words, we hope to achieve a **linear relationship** between speed and number of cores. 
 $$speed = k \*p $$
Speed represents the program's execution speed, p represents the number of cores, and k is a constant. Here, we can represent the value of speed using the measured data of running time, denoted as `t`, where `speed = 1/t`.
![Speed-Core](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Speed-Core-Part1.png)
It can be observed that the running speed and the number of cores satisfy the predicted linear relationship. As the number of cores increases, the running speed linearly increases.

**Two main time-consuming factors**
For a single process i, it has two main time consuming factors during sieving. 
**1. marking array 2. waiting for next prime**  

**Marking Array**  
Suppose process i just receive a new prime **pme** for sieving. Process will hold an array, it will represent **(n-1)/p** continous number. To sieve multiples of **pme**, we need to mark **m** elements. 
$$m = (n-1)/p/pme $$ 
**Waiting for next Prime**  
When sieving primes, process0 need to broadcast next prime to all the other processs. 
Suppose each broadcast iteration takes x sec, we have p processs, the last process will get the message later than the first reciver d seconds. 

$$d = x \left \lfloor  \log_{2}{p}  \right \rfloor$$


#### Part2 - Deleting even numbers

**Analysis**  

We have already konwn that, two main time-consuming factors are **loop of marking** and **broadcasting**. For single process, it marks `m = (n-1)/p/pme` elements when sieving `pme`. 
After removing even number from array, the number of elements we need to mark is reduced to half of its original quantity. Let's take a look.  
Suppose we have n and p and process i. It has an array and the length of it is roughly (n-1)/p/2. When marking this array, because there is no even number in the array, so we can add 2*pme each time. The new marking time is m<sub>2</sub>. It's only half of m.
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

**Prediction**
Based on the analysis above, we can speculate that the running time of Part2 will be half that of Part1, and the running speed will be twice as fast as Part1.

**Result**

Part2 is the testing result after deleting even number from array.  
| Cores | 2   | 4   | 6   | 8   | 16  | 24  | 32  | 40  | 48  | 64  | 128 | 256 | 384 | 512 |
|-------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Part1 | 131.340222 | 74.648559 | 69.071211 | 65.641554 | 33.306110 | 22.234583 | 19.148118 | 15.875710 | 14.331761 | 12.111386 | 6.971425 | 3.155523 | 2.108330 | 1.520438 |
| Part2 | 58.344136 | 36.322337 | 32.985175 | 30.329684 | 12.396677 | 11.436865 | 9.723257 | 8.230217 | 7.189540 | 6.156372 | 3.005913 | 1.425318 | 0.910984 | 0.624564 |  
![Part2_Part1_Speed](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part2vsPart1.png)
![Part2vsPart1](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part2_Part1_Speed.png)
After conducting experiments with different numbers of cores and comparing the running speeds of Part1 and Part2, it was observed that the running speed of Part2 is approximately twice as fast as Part1. The experiment aligns with the expected results.  

#### Part3 - Removing Broadcast
**When Pme is Large**  
It is evident that the prime values used for sieving will become larger and larger. For n = 10<sup>10</sup>, the maximum prime value used for sieving is 10<sup>5</sup>. 
Consequently, the number of elements we need to mark decreases as the prime values increase.    
$$ m \downarrow \left (  pme\to \sqrt{n} \right ) $$  

However, the broadcasting time is independent of the size of the prime values. As the prime number used for filtering increases, the proportion of time spent on broadcasting also gradually increases, thereby affecting efficiency.

**Accumulation of Broadcasting Time**  
We already know that, process0 need to broadcast all primes within `sqrt(n)` to other processes. When n is large, process0 need to broadcast many times. 
Suppose we have  **n = 10<sup>10</sup>**, process0 need to broadcast all primes within **sqrt(n) = 10<sup>5</sup>** to others. After calculation, there are 1229 primes within 10^5, so process0 will broadcast  1228 times. The total delay caused by broadcast is D.  
$$D = 1228 \* d = 1228\*x \left \lfloor  \log_{2}{p}  \right \rfloor$$

**Iteration within a single Broadcasting**

OpenMPI has offered 6 broadcasting algorithms which are basic linear, chain, pipeline, split binary tree, binary tree, binomial tree. In this report, we only care about binary tree algorithm, which is more common and efficient.
When the number of processs increased, each time of broadcasting will take more iterations. $$iterations =  \left \lfloor  \log_{2}{p}  \right \rfloor$$
Because each iteration takes roughly same amount of time, increasing processes will also increasing time on broadcasting. The single broadcast time rises logarithmically.  
**Broadcasting**
With much more processes, process0 will take more times to broadcast next prime. Suppose the Broadcast Algorithm is Binary tree broadcasting.
For instance, when we only have 4 processes, 2 iterations is enough, when the number of processes is 100, it will take 6 iterations.  
| Iterations | 1 | 2 | 3 | 4 | 5 | 6 |
|------------|---|---|---|---|---|---|
| Max Receivers | 3 | 7 | 15 | 31 | 63 | 127 |
**Result**
Let's take a look of our result. Part3 is the result after removing broadcasting mechanism.
| Cores | 2   | 4   | 6   | 8   | 16  | 24  | 32  | 40  | 48  | 64  | 128 | 256 | 384 | 512 |
|-------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Part2 | 58.344136 | 36.322337 | 32.985175 | 30.329684 | 12.396677 | 11.436865 | 9.723257 | 8.230217 | 7.189540 | 6.156372 | 3.005913 | 1.425318 | 0.910984 | 0.624564 |
| Part3 | 63.402072 | 35.648821 | 30.611544 | 31.492965 | 15.529814 | 11.318744 | 9.435255 | 7.968104 | 6.596050 | 5.218180 | 2.919558 | 1.383011 | 0.850719 | 0.576781 |

![Part3_Part2_Speed](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part3vsPart2.png)
![Part3vsPart2](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part3_Part2_Speed.png)
![Part3vsPart2_Ratio](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part3_Part2_Ratio.png)

Based on the above charts, we can observe that when the number of processes is small, the impact of removing the broadcasting mechanism on the computation time is minimal. 

In fact, in some cases, when the number of processes is extremely low, the computation time may be slightly higher without the broadcasting mechanism. 

**Number of Proceses** 

In the previous part, we talked about iteration times when broadcasting a message. When the number of processes are large, each time of broadcasting takes more time.  

As the number of processes increases, when it exceeds 256, the computation time with the broadcasting mechanism removed is only about 91% of the original time, resulting in approximately 9% savings in computation time.

**Why Part2 is slightly faster than Part3 when processes are not to many?**  

To remove broadcasting, each process will hold a prime array used for sieving. Thus, not only process0 need to calculate primes within sqrt(n), all processs should do that.  

*Therefore, when the number of processes is small, the time saved by reducing the broadcasting is not substantial. However, computing all prime numbers less than the square root of n takes more time.*
#### Part4 - Increasing Cache Hit
After Part3, we already removed broadcasting mechanism, so we only care about **time spend on marking**.  

**Time Spend on Marking**  

There are also two factors when marking array. 
**First**, **how many loops it will take** is important.  
**Second**, when marking an element, we simlpy set this element's value to 1 `(marked[i] = 1)`. To do that, we need to access this element. How long will **a single time of access operation takes** affects time spend on marking.  
To reduce the access time, I attempted to increase the L1 cache hit rate. In my running environment, the process had an L1 cache size of 32K (in bytes).  
Therefore, I divided the arrays assigned to each process into segments of length 32K, and the test results are as follows.
| Cores | 2   | 4   | 6   | 8   | 16  | 24  | 32  | 40  | 48  | 64  | 128 | 256 | 384 | 512 |
|-------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Part3 | 63.402072 | 35.648821 | 30.611544 | 31.492965 | 15.529814 | 11.318744 | 9.435255 | 7.968104 | 6.596050 | 5.218180 | 2.919558 | 1.383011 | 0.850719 | 0.576781 |
| Part4 | 55.732597 | 30.266950 | 20.624443 | 16.080076 | 8.027016 | 5.428616 | 3.839528 | 3.076881 | 2.510904 | 2.150971 | 1.042354 | 0.477985 | 0.341598 | 0.247489 |

![Part4_Part3_Speed](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part4vsPart3.png)
![Part4vsPart3](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part4_Part3_Speed.png)
![Part4vsPart3_Ratio](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part4_Part3_Ratio.png)

We can see that, after increasing hiting rate, the execution time will reduced significanly, when there are 6 processes, the running time will be reduced 33% of the original time. When the number of processes are more than 8, running time will be reduced more than 50%.  

When the amount of processes are  large, the running time takes half of the original time.  

**When number of processes are small, improvement is not as much as when we have a lot of processes**  

When the number of processes is large, increasing the cache hit rate can lead to an efficiency close to 100%. However, why is it that when the number of processes is small, the performance improvement does not reach 100%, but only around 70%?   

**My hypothesis** 

For a fixed value of `n`, with a larger number of processes, the length of the array assigned to each process becomes relatively shorter. Let's assume that the array length of process `i` is `l`, and the chosen segmentation length is `s` (in this experiment, I set `s` to 32K). Since `s` is fixed, when the number of processes is small, `l` becomes particularly large, resulting in a very small ratio between `s` and `l`.  

$$segment~~ ratio = s:l$$

A smaller `segment ratio`  `s:l` implies a more scattered array segmentation. Consequently, the number of iterations increases relatively. Since at the start of each iteration, there are some works about initilization, some of which can be time-consuming, this leads to some overhead.

**When the lenght of segmented array is too small, the time will increase**. 

We already found that if we segmented the array length too short, the runtime efficiency would significantly decrease. After analysis, I hypothesized that this phenomenon is mainly caused by an increase in **iterations of the marking operation**. At the beginning of each iteration, certain calculations are required, including:  

**1. Finding the next prime number as a multiplier for  sieving.**  

**2. Performing the multiplication operation `prime * prime` to set the position of the first index.** When the value of `prime` is large, this multiplication operation becomes time-consuming.  

## 4.Improvement of Part4
In the previous part, we observed that, when the length of  segemented array is too small, it will decrease the performance. And one of the two main reason is **For  each outer loop, we need to find the next prime used for sieving** Belowed code is the main operation to find next prime. 
```while (marked[++index]); ```
We already known that for nth prime p<sub>n</sub>, we can get an estimation p<sub>n</sub>
$$p_{n} \approx n\ln_{}{n} ~ (n \to \infty )$$
When n is relatively big, finding next prime will take z times itereation. 
$$z = p_{n+1}- p_{n} \approx \ln_{}{n} ~ (n \to \infty )$$ Because we segmented the original `l`length array to `s`length. Each sgemented array need find next prime used for sieving `from 3 to sqrt(n)`, so the total itereation times will approximately be u.  

$$ u \approx l/s\*(p_{n+1}- p_{n}) ~(n ~from~ 1~ to\sqrt{n}  )$$  

**Store required prime numbers in a contiguous array.**  

After storing required prime in advance, the program yielded the following test results.
| Cores | 2   | 4   | 6   | 8   | 16  | 24  | 32  | 40  | 48  | 64  | 128 | 256 | 384 | 512 |
|-------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Part4 | 55.732597 | 30.266950 | 20.624443 | 16.080076 | 8.027016 | 5.428616 | 3.839528 | 3.076881 | 2.510904 | 2.150971 | 1.042354 | 0.477985 | 0.341598 | 0.247489 |
| Update| 57.627297 | 28.875504 | 17.369307 | 13.346055 | 7.176643 | 4.355179 | 3.355896 | 2.464973 | 2.332710 | 1.629199 | 0.832257 | 0.402538 | 0.249635 | 0.185892 |


![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part4vsUpdate.png)
![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Part4_Update_Speed.png)
The improved Part 4 utilizes a contiguous array to store the necessary prime data in advance. When number of processes are 512, after storing primes in advance, 25% running time will be reduced.

**Benchmark** 
Depicting the results of these five different test cases in a chart, as shown below.
![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Benchmark_Time.png)
![Chart](https://github.com/jindajia/ParallelPrimeFinder/raw/main/Figure/Benchmark_Speed.png)

### Future Improvement Maybe
**Pre store prime*prime**
At the beginning of the marking process, we need to calculate the initial index position using the result of `prime * prime`. We already reorganized the loop structure in Part4 to increase cache hits. The same prime is used up to `l/s` times in each process.  
**One effective approach is to store the `prime * prime` values for frequently used larger primes, allowing for direct retrieval in subsequent iterations.** 
However, the selection of primes presents a research-worthy question. When primes are small, the time spent searching for the corresponding square value in the hash array may exceed the time required for direct calculation. When primes are large, their probability of being used for sieving decreases.

