#!/bin/bash

# File to store the results
output_file="results.txt"

# Clear the results file if it exists, or create it if it doesn't
> "$output_file"

# Array of values for T and N
values=(1 2 4 8 16 32)

# Iterate over all combinations of T and N
for T in "${values[@]}"; do
    for N in "${values[@]}"; do
        # Run the command and append the output to the file
        echo "Running ./performance.o with T=$T and N=$N" 
        result=$(./performance.o $T $N)
        echo "T=$T, N=$N, Result=$result" >> "$output_file"
    done
done

echo "All results have been written to $output_file"

