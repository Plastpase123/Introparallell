#!/bin/bash

# Output file
output_file="SoE_benchmark.txt"

# Array of max values to test
max_values=(100 10000 1000000 100000000 1000000000)

# Array of threads values to test
threads_values=(1 2 4 8 16 32)

# Clear the output file if it exists
> $output_file

# Start benchmarking
echo "Starting benchmark..." | tee -a $output_file

for max in "${max_values[@]}"
do
    for threads in "${threads_values[@]}"
    do
        echo "Running with max = $max and threads = $threads..." | tee -a $output_file
        # Run the SoE program and append output to the file
        ./SoE.o $max $threads >> $output_file 2>&1
        echo "------------------------------------------" | tee -a $output_file
    done
done

echo "Benchmark completed!" | tee -a $output_file
