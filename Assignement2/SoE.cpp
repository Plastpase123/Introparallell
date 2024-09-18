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

void compute_primes(int max, std::vector<bool>& primes){
  primes[0] = primes[1] = false;

  for (int i = 2; i*i <= max; ++i){
    if (primes[i]){
      for (int j = i*i; j <= max; j += i){
        primes[j] = false;
      }
    }
  }

}

void compute_primes_parallell(int start, int max, std::vector<bool>& primes){
  for (int i = 2; i*i <= max; ++i){
    if (primes[i]){
      int offset = std::max(i * i, ((start + i - 1) / i) * i);
      for (int j = offset; j <= max; j += i){
        //lock.lock();
        primes[j] = false;
        //lock.unlock();
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
  std::vector<bool> primes(max + 1, true);


  compute_primes(sqrtmax, std::ref(primes));



  auto start_time = std::chrono::system_clock::now();

  int start = sqrtmax + 1;
  std::thread *t = new std::thread[threads];
  for (int i = 0; i < threads; i++){
    if (i != threads - 1){
      int stop = start + chunk_size;
      t[i] = std::thread(compute_primes_parallell, start, stop, std::ref(primes));
      start = stop;
    }
    else {
      compute_primes_parallell(start, max, primes);
    }
  }

  for (int i = 0; i < threads - 1; i++) {
    t[i].join();
  }


  std::chrono::duration<double> duration =
        (std::chrono::system_clock::now() - start_time);
  // *** timing ends here ***
  std::cout << "Finished in " << duration.count() << " seconds (wall clock).\n";


  std::vector<int> prime_numbers;

  for (int i = 2; i < max; i++){
    if (primes[i]){
      prime_numbers.push_back(i);
    }
  }

  //print_container(prime_numbers);

  return 0;
}
