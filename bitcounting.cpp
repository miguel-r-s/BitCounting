#include<cstdint>
#include<array>
#include<vector>
#include<chrono>
#include<numeric>
#include<iostream>
#include<ratio> 
#include<algorithm> 

using namespace std::chrono; 

struct BitCounter {
    
  double bitcounting_total_time(const std::vector<uint32_t>& v) {
        
    /*
    Used to guarantee the compiler does not optimize out calls 
    to count_bits. The final dummy is simply print to stdout which 
    keeps the compiler from simply ignoring the output of count_bits. */
    uint8_t dummy = 0;
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
        
    /* Call count_bits for each element of the array */
    for(const auto& vv : v) {
      dummy |= count_bits(vv);  
     }
        
     high_resolution_clock::time_point t2 = high_resolution_clock::now();
        
     /* Printing this keeps the compiler from discarding the variable */
     std::cout << dummy << std::endl;
    
     return duration_cast<duration<double>>(t2-t1).count();
  }
      
  virtual uint8_t count_bits(uint32_t) = 0;
};

struct NaiveBitCounter : BitCounter {
  
  uint8_t count_bits(uint32_t n) override {
    
    uint8_t counter = 0;
    for( ; n; n>>=1 )
      /* Same as if(n&1){++counter;}, but avoids branching */ 
      counter += (n & 1); 
    
    return counter;
  }
};

struct LookupTableBitCounter : BitCounter {

  using LookupTable_t = std::array<uint8_t, 256>;
  static const LookupTable_t lookup_table; 
  
  uint8_t count_bits(uint32_t n) override {
    return lookup_table[n       & 0xff] +
           lookup_table[n >>  8 & 0xff] +
           lookup_table[n >> 16 & 0xff] +
           lookup_table[n >> 24 & 0xff]; 
  }
};

const LookupTableBitCounter::LookupTable_t LookupTableBitCounter::lookup_table = { 
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

struct SWARBitCounter : BitCounter {
  
  uint8_t count_bits(uint32_t n) override {
    /* Using https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel */
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }
};

int main() {

  NaiveBitCounter       nbc;
  LookupTableBitCounter lbc;
  SWARBitCounter        sbc;

  const int n_elements = 5E8;
  std::vector<uint32_t> numbers(n_elements);
  
  /* Fill the array with integers from zero to n_elements - 1 */
  std::iota(std::begin(numbers), std::end(numbers), 0);
  
  std::cout << "Testing for an array of sequential integers" << std::endl;
  std::cout << "NaiveBitCounter      : " << nbc.bitcounting_total_time(numbers) << std::endl;
  std::cout << "LookupTableBitCounter: " << lbc.bitcounting_total_time(numbers) << std::endl;
  std::cout << "SWARBitCounter       : " << sbc.bitcounting_total_time(numbers) << std::endl;

  /* Shuffle the array */
  std::random_shuffle(std::begin(numbers), std::end(numbers));
  
  std::cout << "Testing for an array of shuffled integers" << std::endl;
  std::cout << "NaiveBitCounter      : " << nbc.bitcounting_total_time(numbers) << std::endl;
  std::cout << "LookupTableBitCounter: " << lbc.bitcounting_total_time(numbers) << std::endl;
  std::cout << "SWARBitCounter       : " << sbc.bitcounting_total_time(numbers) << std::endl;

  /* Constant data: fill all entries of the vector with the first element of the shuffled array */
  std::fill(std::begin(numbers), std::end(numbers), numbers.at(0));
  
  std::cout << "Testing for an array of similar integers" << std::endl;
  std::cout << "NaiveBitCounter      : " << nbc.bitcounting_total_time(numbers) << std::endl;
  std::cout << "LookupTableBitCounter: " << lbc.bitcounting_total_time(numbers) << std::endl;
  std::cout << "SWARBitCounter       : " << sbc.bitcounting_total_time(numbers) << std::endl;
   
  return 0;
}
