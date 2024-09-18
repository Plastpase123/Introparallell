// Program that finds prime numbers between 1 and an arbitrary number Max
#include <iostream>
#include <mutex>
#include <list>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include <thread>

std::mutex lock;

void usage(char *program)
{
    std::cout << "Usage: " << program << " max threads, 1 <= max,  1 < threads <= max" << std::endl;
    exit(1);
}

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

void compute_primes_parallell(int start, int max, std::vector<int>& primes, std::vector<int>& list){
  for (int k : primes){
    for (int i = start; i <= max; i++){
      if (list[i-1] % k == 0){
        list[i-1] = 0;
      }
    }
  }
}



int main(int argc, char*argv[]){

  if (argc != 3){
    usage(argv[0]);
  }

  int max = std::stoi(argv[1]);
  int sqrtmax = sqrt(max);
  int elements = max - sqrtmax;
  int threads = std::stoi(argv[2]);
  int chunk_size = elements / threads;
  std::vector<int> list;



  for (int i = 1; i <= max; i++){
    list.push_back(i);
  }

  int k = 2;
  list[0] = 0;


  compute_primes(k, sqrtmax, list);

  std::vector<int> primes;

  //  Retrieve primes from original list
  for(int i = 0; i < sqrtmax; i++){
    if (list[i] != 0){
        primes.push_back(list[i]);
      }
  }

  int start = sqrtmax + 1;

  auto start_time = std::chrono::system_clock::now();


  std::thread *t = new std::thread[threads];
  for (int i = 0; i < threads; i++){
    if (i != threads - 1){
      t[i] = std::thread(compute_primes_parallell, start*(i+1), start*(i+1) + chunk_size, std::ref(primes), std::ref(list));
    }
    else {
      compute_primes_parallell(start*(i+1), max, primes, list);
    }
  }

  for(int i = start; i < max; i++){
    if (list[i] != 0){
        primes.push_back(list[i]);
      }
  }

  std::chrono::duration<double> duration =
        (std::chrono::system_clock::now() - start_time);
  // *** timing ends here ***
  std::cout << "Finished in " << duration.count() << " seconds (wall clock)." << std::endl;

  //print_container(primes);
  return 0;
}
