#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>

std::mutex lock;

double total = 0;


void help(char *program)
{
    std::cout << "\nThis is a program that calculates the integral from 0 to 1 of 4/(1+x^2). \n\n"
              << "Usage: " << program << " w threads, where:\n"
              << "w: Amount of trapezes, where 1 <= w\n"
              << "threads: Amount of threads, where 1 <= threads <= w.\n" << std::endl;
    exit(1);
}

/* integral from 0 to 1 of 4/(1+x^2) dx */
void usage(char *program)
{
    std::cout << "Usage: " << program << " w threads, 1 <= w,  1 < threads <= w" << std::endl;
    exit(1);
}

double fun(double x)
{

    return 4 / (1 + (x * x));
}

double calculate_result(double x1, double x2, double length)
{
    double f1 = fun(x1);
    double f2 = fun(x2);
    double result = ((f1 - f2) * length) / 2 + f2 * length;
    return result;
}

void compute_integral(double i1, double i2, double length)
{
    double sum_results = 0;

    for (double i = i1; i < i2; i += length)
    {
        double x1 = i;
        double x2 = i + length;
        double result = calculate_result(x1, x2, length);
        sum_results += result;
    }
    lock.lock();
    total += sum_results;
    lock.unlock();
}

int main(int argc, char *argv[])
{

    // Catch cases where arguments are wrong

    if (argc != 3)
    {
        if (argc == 2)
        {
            if (std::strcmp(argv[1],"-h") == 0) {help(argv[0]);}
        }

        else
        {
            usage(argv[0]);
        }
    }

    double w;
    int threads;
    try
    {
        w = std::stod(argv[1]);
        threads = std::stoi(argv[2]);
        std::cout << threads;
    }
    catch (std::exception const &)
    {
        usage(argv[0]);
    }

    if (w <= 0 || threads <= 0 || threads > w)
    {
        usage(argv[0]);
    }

    double length = 1 / w;

    // *** timing begins here ***
    auto start_time = std::chrono::system_clock::now();

    // configure first thread in case of uneven division of work per thread

    // if (threads > 1)
    // {
    std::thread *t = new std::thread[threads];
    double start = 0;
    int tasks_per_thread = w / threads;

    for (int i = 0; i < threads; i++)
    {
        if (i != threads - 1)
        {
            double stop = start + (tasks_per_thread * length);
            t[i] = std::thread(compute_integral, start, stop, length);
            start = stop;
        }
        else
        {
            // last thread accounts for the rest of the work
            t[i] = std::thread(compute_integral, start, 1, length);
        }
    }

    for (int i = 0; i < threads; ++i)
    {
        t[i].join();
    }
    // }
    /*
    else
    {
        compute_integral(0, 1, length);
    }
    */
    std::cout << "\n";
    std::cout << total;
    std::cout << "\n";

    std::chrono::duration<double> duration =
        (std::chrono::system_clock::now() - start_time);
    // *** timing ends here ***

    std::cout << "Finished in " << duration.count() << " seconds (wall clock)." << std::endl;

    return 0;
}
