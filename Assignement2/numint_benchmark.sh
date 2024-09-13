#!/usr/bin/env sh

#!/bin/bash

# File to store the results
output_file="results.txt"

# Clear the results file if it exists, or create it if it doesn't
> "$output_file"

# Array of values for w
wvalues=(1 100 10000 100000 1000000 10000000 100000000)
# Array of values for threads
tvalues=(1 3 5 7 9 11 13 15)

# Iterate over all combinations of w and threads
for w in "${wvalues[@]}"; do
    for t in "${tvalues[@]}"; do
        # Run the command and append the output to the file
        echo "Running ./numint.o with w=$w and threads=$t"
        result=$(./numint.o $w $t)
        echo "w=$w, threads=$t, Result:$result" >> "$output_file"
    done
done

echo "All results have been written to $output_file"
