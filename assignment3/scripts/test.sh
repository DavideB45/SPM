#!/bin/bash


PROBLEMS=(
    "small_files/"
    "big_files/"
    "mixed_files/"
)

# Create a directory for the results
mkdir -p results

./scripts/test_sequential.sh

./scripts/test_parallel.sh

./scripts/test_for_parallel.sh