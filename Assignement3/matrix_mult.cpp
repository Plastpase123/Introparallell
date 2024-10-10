// #include <omp.h>
#include <iostream>
/* static scheduling of matrix multiplication loops */

void print_matrix(int matrix[100][100], int dim)
{
    std::cout << "Matrix:" << std::endl;
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    int dim = 100;
    int a[100][100];
    int b[100][100];
    int c[100][100];

    // Initialize matrices
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            a[i][j] = i + j;
            b[i][j] = i - j;
        }
    }

    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            c[i][j] = 0;
            for (int k = 0; k < dim; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    print_matrix(a, dim);
    print_matrix(b, dim);
    print_matrix(c, dim);
}
