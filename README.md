# Methods

* Naively checking each bit
* Lookup table for each byte
* [SWAR bit counting](http://webpages.charter.net/tlikens/tech/bitmaps/bit_popcnt.html#divide_and_conquer)

# Comparison

I tested three different bitcounting strategies: a Naive count of each bit, a lookup table for each byte, and SWAR bit counting. 
The Lookup Table outperformed SWAR bit counting by a small margin, and as expected, the naive bit counting strategy performed significantly worse than the other two. 

# Bitten by my CPU's Branch Prediction

For the first time, I was bitten by branch-prediction in the Naive implementation of a bit counter.

The first implementation was the following: 

```c
 uint8_t count_bits(uint32_t n) {
         
          uint8_t counter = 0;
          for( ; n; n>>=1 )
            if(n & 1)
              ++counter;
            
          return counter;
 }
```

This proved to process a sorted array much faster than an unsorted array. A bit of googling took me to [this](https://stackoverflow.com/questions/11227809/why-is-it-faster-to-process-a-sorted-array-than-an-unsorted-array) SO question where it is determined that branch prediction is the culprit. 
I tried to fight the faults of branch prediction with the following:

 ```c
 uint8_t count_bits(uint32_t n) {
         
          uint8_t counter = 0;
          for( ; n; n>>=1 )
            counter += (n & 1);
            
          return counter;
 }
```

But much to my surprise this didn't solve the discrepancy in processing time for sorted and unsorted arrays of `uint32_t`.
Branch prediction also happens in the `for` loop whenever the break condition `n` is evaluated.

The third attempt was:

 ```c
 uint8_t count_bits(uint32_t n) {
         
          uint8_t counter = 0;
          for(int i = 0; i < 32; ++i, n>>=1 )
            counter += (n & 1);
            
          return counter;
 }
```

This successfully eliminates the discrepancies in processing time for sorted/unsorted arrays, at the cost of _worse_ performance for the sorted case. 
Manually unrolling the loop seemed to work though! 

 ```c
 uint8_t count_bits(uint32_t n) {
         
          uint8_t counter = 0;
          
          counter += (n & 1); n>>=1;
          counter += (n & 1); n>>=1;
          counter += (n & 1); n>>=1;
          (repeat 32 times in total...)
          
          return counter;
 }
```

Using gcc flags for loop unrolling (`__attribute__((optimize("unroll-loops")))`) helped, but actually performed worse than manually unwinding the loop.

Of course, manual unwinding like this is not practical, so my workaround was:

```c
#define REPEAT4(x) x;x;x;x;
#define REPEAT2(x) x;x;
#define REPEAT32(x) REPEAT2(REPEAT4(REPEAT4(x)))

uint8_t count_bits(uint32_t n) {
         
          uint8_t counter = 0;
          
          REPEAT32(counter += (n & 1);n>>=1;);
          
          return counter;
 }
```

Not pretty, but hey, it beats everything else I've tried! 
