#!/bin/bash

# Script to create a bunch of files for assignment 3
# Usage: ./build_file.sh create -s [b|s|a] [-n nfiles] to create files
# Usage: ./build_file.sh clean to remove files and directories
# -s: size of files
# -n: number of files to create
# -b: big files (3-50 MB)
# -s: small files (100-700 KB)
# -a: all files (both big and small)

# Set variables
DIR="files"
NUM_FILES=100
FILE_PREFIX="file_"
FILE_SUFFIX=".dat"
SIZE="small" # Default size is small

# Function to create files
create_files() {
    # Create the directory if it doesn't exist
    mkdir -p "$DIR"

    # Create files with a specific pattern
    for ((i=1; i<=NUM_FILES; i++)); do
        if [ "$SIZE" == "small" ]; then
            RANDOM_NUMBER=$((RANDOM % 600 + 100)) # Generate a random number between 100 and 700
            dd if=/dev/urandom of="$DIR/$FILE_PREFIX$i$FILE_SUFFIX" bs=1K count="$RANDOM_NUMBER" 2>/dev/null
        elif [ "$SIZE" == "big" ]; then
            RANDOM_NUMBER=$((RANDOM % 50 + 50)) # Generate a random number between 50 and 99
            dd if=/dev/urandom of="$DIR/$FILE_PREFIX$i$FILE_SUFFIX" bs=1M count="$RANDOM_NUMBER" 2>/dev/null
        fi
    done

    echo "Created $NUM_FILES files in the $DIR directory."
}

# Function to clean up files
clean_files() {
    # Remove the directory and its contents
    rm -rf "$DIR"
    echo "Cleaned up files in the $DIR directory."
}

# Process command line arguments
case "$1" in
    create)
        if [ "$2" == "-n" ] && [ "$4" == "-s" ]; then
            NUM_FILES="$3"
            SIZE="$5"
        elif [ "$2" == "-s" ] && [ "$4" == "-n" ]; then
            SIZE="$3"
            NUM_FILES="$5"
        else
            echo "Invalid option or order. Use '-n' for number of files and '-s' for size."
            echo "Received: $2 $4"
            exit 1
        fi
        if [ "$SIZE" == "b" ]; then
            SIZE="big"
        elif [ "$SIZE" == "s" ]; then
            SIZE="small"
        elif [ "$SIZE" == "a" ]; then
            SIZE="all"
        else
            echo "Invalid size option. Use 'b' for big, 's' for small, or 'a' for all."
            echo "Received: $SIZE"
            exit 1
        fi
        create_files
        ;;
    clean)
        clean_files
        ;;
    *)
        echo "Invalid option. Use 'create' to create files or 'clean' to remove them."
        echo "Received: $1"
        exit 1
        ;;
esac