#include <iostream>
#include <papi.h>
#include "bitcounting.hpp"

int main() {

  NaiveBitCounter       nbc;
  LookupTableBitCounter lbc;
  SWARBitCounter        sbc;

  const unsigned long n_elements = 1E7;
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
