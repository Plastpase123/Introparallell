#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <vector>

std::mutex lock;

/* integral from 0 to 1 of 4/(1+x^2) dx */

double fun(double x){

    return 4/(1+(x*x));
}

int main(int argc, char *argv[])
{   
 
    double w = std::stod(argv[1]);
    std::vector<double> points(w+1);

    points[0] = 0;

    for(double i = 1; i <= w; i++){
        points[i] =(i*(1/w));
    }
    for( double n : points)
    {
        std::cout << n;
        std::cout << " ";
    }

    double total = 0;

    for(int i = 0; i <= w; i++){
        double length = 1/w; 
        double heigth_x1 = fun(i+1);
        double heigth_x = fun(i);

    }

    std::cout << "\n";
    std::cout << total;
    std::cout << "\n";


    return 0;
}