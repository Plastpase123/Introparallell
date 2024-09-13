// Program that finds prime numbers between 1 and an arbitrary number Max
#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>


void print_container(const std::vector<int>& c)
{
    for (int i : c)
        std::cout << i << ' ';
    std::cout << '\n';
}

void compute_primes(int k, int max, std::vector<int>& list){
  while(k < max){
    // Mark all multiples of k
    for (int i = k*k; i <= max; i += k){
      list[i-1] = 0;
    }

    // Find smallest unmarked number greater than k
    k++;
    while(list[k-1] == 0){
      k++;
    }
  }

}

int main(int argc, char*argv[]){

  int max = std::stoi(argv[1]);

  std::vector<int> list;

  for (int i = 1; i <= max; i++){
    list.push_back(i);
  }

  int k = 2;
  list[0] = 0;



// Compute std::round(sqrt(max))
//
// Given p threads, build p chunks covering range from sqrt(max) + 1 to max
// Numbers to cover = max - (sqrt(max) + 1)
// Numbers for thread0 = Numbers to cover / p + rest
// Numbers per thread = Numbers to cover / p
//

  //int sqrtmax = std::round(sqrt(max));
  // int span_length = max - sqrtmax + 1;
  // int span_per_thread = span_length / p;
  //

  compute_primes(k, max, list);


  std::vector<int> primes;

  //  Retrieve primes from original list
  for (int i: list){
    if (i != 0){
        primes.push_back(i);
      }
  }

  print_container(primes);

  return 0;
}
