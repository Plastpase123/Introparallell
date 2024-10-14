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



void col_oriented(int n, std::vector<int> &x, std::vector<int> &b,  std::vector<std::vector<int>> &A) {

  for (int row = 0; row < n; row++) {
    x[row] = b[row];
  }

  for(int col = n - 1; col >= 0; col--) {
    x[col] /= A[col][col];
    for (int row = 0; row < col; row++) {
      x[row] -= A[row][col] * x[col];
    }
  }


  return;
}


void row_oriented(int n, std::vector<int> &x, std::vector<int> &b,  std::vector<std::vector<int>> &A) {

  for (int row = n - 1; row >= 0; row--) {
    x[row] = b[row];
    for (int col = row + 1; col < n; col++) {
      x[row] -= A[row][col] * x[col];
    }
    x[row] /= A[row][row];
  }

  return;
}




int main(int argc, char *argv[]) {

  int dim = 3;
  std::vector<int> x(dim, 0); // lhs
  //std::vector<int> b = {3, 7, 5}; // rhs
  //std::vector<std::vector<int>> A = {{2,  -3,  0},
                                     //{4,  -5,  1},
                                     //{2,  -1, -3}};

  std::vector<int> b = {3, 1, 0}; // rhs
  std::vector<std::vector<int>> A = {{2,  -3,  0},
                                     {0,  1,  1},
                                     {0,  0, -5}};


  row_oriented(dim, x, b, A);
  for (int i = 0; i < dim; i++) {
    std::cout << x[i] << "\n";
  }

  return 0;
}
