#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>
#include <bits/stdc++.h>
#include <vector>

std::vector<std::vector<int>> multiply_1_collapse(int dim, std::vector<std::vector<int>> &a, std::vector<std::vector<int>> &b, std::vector<std::vector<int>> &c)
{

#pragma omp parallel default(none) shared(a, b, c, dim)
#pragma omp for collapse(1) schedule(static)
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
    return c;
}

std::vector<std::vector<int>> multiply_2_collapse(int dim, std::vector<std::vector<int>> &a, std::vector<std::vector<int>> &b, std::vector<std::vector<int>> &c)
{
#pragma omp parallel default(none) shared(a, b, c, dim)
#pragma omp for collapse(2) schedule(static)
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
    return c;
}

std::vector<std::vector<int>> multiply_3_collapse(int dim, std::vector<std::vector<int>> &a, std::vector<std::vector<int>> &b, std::vector<std::vector<int>> &c)
{
#pragma omp parallel default(none) shared(a, b, c, dim)
#pragma omp for collapse(3) schedule(static)
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
    return c;
}

std::vector<std::vector<int>> zero_matrix(int dim)
{
    std::vector<std::vector<int>> c(dim, std::vector<int>(dim));
    for (int i = 0; i < dim; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            c[i][j] = 0;
        }
    }
    return c;
}
    void usage(char *program)
    {
        std::cout << "Usage: " << program << " dim threads" << std::endl;
        exit(1);
    }

    int main(int argc, char *argv[])
    {

        if (argc != 2)
        {
            usage(argv[0]);
        }
	
	omp_set_num_threads(8);

        int dim = std::atoi(argv[1]);
        // int nthrds = std::atoi(argv[2]);

        std::vector<std::vector<int>> a(dim, std::vector<int>(dim));
        std::vector<std::vector<int>> b(dim, std::vector<int>(dim));
        std::vector<std::vector<int>> c = zero_matrix(dim);

        srand(time(0));
        // Fill the matrix with random integers
        for (int i = 0; i < dim; ++i)
        {
            for (int j = 0; j < dim; ++j)
            {
                a[i][j] = std::rand() % 10; // Random integers between 0 and 99
                b[i][j] = std::rand() % 10;
            }
        }

        double start_time = omp_get_wtime();
        c = multiply_3_collapse(dim, a, b, c);
        double stop_time = omp_get_wtime();
        std::cout << "Time taken for 1 collapse: " << stop_time - start_time << std::endl;
	

	omp_set_num_threads(32);
	c = zero_matrix(dim);
	start_time = omp_get_wtime();
	c = multiply_3_collapse(dim, a, b, c);
	stop_time = omp_get_wtime();
	std::cout << "Time taken for 3 collapse: " << stop_time - start_time << std::endl;
    
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

