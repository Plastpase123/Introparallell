#!/usr/bin/env sh

#!/bin/bash

# File to store the results
output_file="results_sorted_list.txt"

# Clear the results file if it exists, or create it if it doesn't
> "$output_file"

# Array of values for threads
tvalues=(1 2 4 8 16 32)

# Iterate over all combinations of w and threads
for t in "${tvalues[@]}"; do
    # Run the command and append the output to the file
    echo "Running ./benchmark_example.o with threads=$t"
    result=$(./benchmark_example.o $t)
    echo "threads=$t, Result:$result" >> "$output_file"
done

echo "All results have been written to $output_file"
