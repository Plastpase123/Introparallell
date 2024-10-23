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

int main(int argc, char *argv[])
{

  if (argc != 2) {
    usage(argv[0]);
  }


  int max = std::stoi(argv[1]);
  int sqrtmax = sqrt(max);
  int elements = max; // Amount of elements to check
  int DEBUG = 0;

  MPI_Init(&argc, &argv);

  int chunk_size, world_size;

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  chunk_size = elements / world_size;
  int world_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  //auto start_time = std::chrono::system_clock::now();

  double time = MPI_Wtime();

  int start, stop, offset;
  if (DEBUG) std::cout << "testing. This is processor " << world_rank << std::endl;

  if (world_rank != 0) {start = (world_rank * chunk_size);} else {start = 0;}
  
  std::vector<bool>primes; // Initiate primes list, will be set depending on process rank

  if (world_rank != world_size - 1) { 
    stop = start + chunk_size;
    std::vector<bool> temp_list(chunk_size, true);
    primes = temp_list;
  } else { 
    stop = max;
    int size = max - start;
    if (DEBUG) std::cout << "start: " << start << " size: " << size << std::endl;
    std::vector<bool> temp_list(size + 1, true);
    primes = temp_list;
  }

  if (DEBUG) std::cout << "first checkpoint" << std::endl;
  int k = 2;
  int first_multiple;

  if (world_rank == 0) {
    compute_primes(sqrtmax + 1 , std::ref(primes)); // compute primes up until sqrt(max) + 1
  }

  while (k*k <= max) {
    if (start % k == 0) {first_multiple = 0;} else {first_multiple = k - start % k;}

    for (int i = first_multiple; i < primes.size(); i += k) {
      primes[i] = false;}

    if (world_rank == 0) {primes[k] = true;}


    if (world_rank == 0) {
      int next_k = k + 1;
      while (!primes[next_k]) {next_k += 1;}
      k = next_k;
      if (DEBUG) {std::cout << "The value of k is " << k << std::endl;}

      for (int i = 1; i < world_size; i++) {
        int message = k;
        MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        if (DEBUG) {std::cout << "sent message: " << message << " to process: " << i << std::endl;}
      }
    } 

    else {
      int recieved_message;
      MPI_Recv(&recieved_message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if (DEBUG) {std::cout << "Recieved message: " << recieved_message << std::endl;}
      k = recieved_message;
    }
  }

    int local_sum = 0;
    for(int i = 0; i < chunk_size; i++){
        if (primes[i]){local_sum++;}
    }

    int global_sum = 0;

    if (world_rank == 0) {
      int local_sum_msg;
      for (int i = 1; i < world_size; i++) {
        MPI_Recv(&local_sum_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recieve local sums from other processes
        global_sum += local_sum_msg;
      }
      global_sum += local_sum; // Add P0:s sum to global sum
    } else {
      MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

  if (world_rank == 0) {
    //std::cout << "num primes: " << global_sum << std::endl;
    time = MPI_Wtime() - time;
    std::cout << "time taken: " << time << std::endl;
  }

  MPI_Finalize();

  return 0;
}
