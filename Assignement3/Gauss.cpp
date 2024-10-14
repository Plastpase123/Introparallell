#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>
#include <bits/stdc++.h>
#include <vector>

void usage(char *program)
{
  std::cout << "Usage: " << program << " no_threads " << std::endl;
  exit(1);
}

void col_oriented(int n, std::vector<int> &x, std::vector<int> &b, std::vector<std::vector<int>> &A)
{
  #pragma omp parallel for
  for (int row = 0; row < n; row++)
  {
    x[row] = b[row];
  }

  
  for (int col = n - 1; col >= 0; col--)
  {
    x[col] /= A[col][col];
#pragma omp parallel for schedule(static)
    for (int row = 0; row < col; row++)
    {
      x[row] -= A[row][col] * x[col];
    }
  }

  return;
}

void row_oriented(int n, std::vector<int> &x, std::vector<int> &b, std::vector<std::vector<int>> &A)
{

  for (int row = n - 1; row >= 0; row--)
  {
    x[row] = b[row];
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int col = row + 1; col < n; col++)
    {
      sum += A[row][col] * x[col];
    }
    x[row] = (b[row] - sum) / A[row][row];
  }

  return;
}

std::vector<std::vector<int>> create_matrix(int dim)
{
  srand(time(0));

  std::vector<std::vector<int>> matrix(dim, std::vector<int>(dim));
  // #pragma omp parallel for collapse(2) num_threads(4)

  for (int i = 0; i < dim; ++i)
  {
    for (int j = i; j < dim; ++j)
    {
      matrix[i][j] = 1;
    }
  }

  return matrix;
}

std::vector<int> create_vector(int dim)
{

  std::vector<int> vector(dim, 1);

  return vector;
}

int main(int argc, char *argv[])
{

  if (argc != 2)
  {
    usage(argv[0]);
  }

  int dim = std::stoi(argv[1]);

  std::vector<int> x(dim, 0);                           // lhs
  std::vector<int> b = create_vector(dim);              // rhs
  std::vector<std::vector<int>> A = create_matrix(dim); // lhs

  for (int i = 0; i < dim; ++i)
  {
    for (int j = 0; j < dim; ++j)
    {
      //std::cout << A[i][j];
    }
    //std::cout << "\n";
  }

  double start_time, stop_time;
  start_time = omp_get_wtime();
  col_oriented(dim, x, b, A);
  stop_time = omp_get_wtime();

  for (int i = 0; i < dim; i++)
  {
    //std::cout << b[i];
  }
  std::cout << "\n";

  for (int i = 0; i < dim; i++)
  {
    std::cout << x[i];
  }
  std::cout << "\n";

  std::cout << "Time taken: " << stop_time - start_time << " sec, with dim = " << dim << std::endl;

  return 0;
}
