#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>
#include <bits/stdc++.h>
#include <vector>
void usage(char *program)
{
    std::cout << "Usage: " << program << " dim threads" << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        usage(argv[0]);
    }

    int dim = std::atoi(argv[1]);
    int nthrds = std::atoi(argv[2]);
    std::vector<std::vector<int>> a(dim, std::vector<int>(dim));
    std::vector<std::vector<int>> b(dim, std::vector<int>(dim));
    std::vector<std::vector<int>> c(dim, std::vector<int>(dim));

    srand(time(0));
    // Fill the matrix with random integers
    for (int i = 0; i < dim; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            a[i][j] = std::rand() % 10; // Random integers between 0 and 99
            b[i][j] = std::rand() % 10;
            c[i][j] = 0;
        }
    }

    auto start_time = std::chrono::system_clock::now();

    /* static scheduling of matrix multiplication loops */
#pragma omp parallel for collapse(3) num_threads(nthrds)
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            for (int k = 0; k < dim; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    std::chrono::duration<double> duration =
        (std::chrono::system_clock::now() - start_time);
    // *** timing ends here ***
    std::cout << "Finished in " << duration.count() << " seconds (wall clock).\n";

    // Output the matrix
    /*
    std::cout << "The " << dim << "x" << dim << " matrix is: \n";
    for (int i = 0; i < dim; ++i) {
      for (int j = 0; j < dim; ++j) {
        std::cout << c[i][j] << " ";
      }
      std::cout << std::endl;
    }*/

    return 0;
}
