// Program that finds prime numbers between 1 and an arbitrary number Max
#include <iostream>
#include <mutex> 
#include <list>
#include <vector>
#include <cmath>
#include <thread>
#include <mpi.h>

std::mutex lock;

void usage(char *program) {
  std::cout << "Usage: " << program << " max threads, 1 <= max,  1 < threads <= max" << std::endl;
  exit(1);
}


void print_container(const std::vector<int> &c) {
  for (int i : c)
    std::cout << i << ' ';
  std::cout << '\n';
}

// Sequential computing of primes
void compute_primes(int max, std::vector<bool> &primes) {
  primes[0] = primes[1] = false;

  for (int i = 2; i * i <= max; ++i) {
    if (primes[i]) {
      for (int j = i * i; j <= max; j += i) {
        primes[j] = false;
      }
    }
  }
}

void compute_primes_parallell(int start, int max, std::vector<bool> &primes) {
  for (int i = 2; i * i <= max; ++i) {
    if (primes[i]) {
      int offset = std::max(i * i, ((start + i - 1) / i) * i);
      for (int j = offset; j <= max; j += i) {
        primes[j] = false;
      }
    }
  }
}

int main(int argc, char *argv[])
{

  if (argc != 2) {
    usage(argv[0]);
  }


  int max = std::stoi(argv[1]);
  int sqrtmax = sqrt(max);
  int elements = max - sqrtmax; // Amount of elements to check
  //int threads = std::stoi(argv[2]);
  std::vector<bool> primes(max + 1, true); // List to store primes. true = prime, false = non-prime

  MPI_Init(&argc, &argv);

  int chunk_size, world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  chunk_size = elements / world_size;

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  auto start_time = std::chrono::system_clock::now();

  std::cout << "testing. This is processor " << world_rank << std::endl;

  std::chrono::duration<double> duration =
      (std::chrono::system_clock::now() - start_time);
  // *** timing ends here ***
  std::cout << "Finished in " << duration.count() << " seconds (wall clock).\n";

  MPI_Finalize();
  std::vector<int> prime_numbers;
/*
    for (int i = 2; i < max; i++){
      if (primes[i]){
        prime_numbers.push_back(i);
      }
      }


  print_container(prime_numbers);
*/
  return 0;
}
