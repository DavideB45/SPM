#!/bin/bash

# Script to create a bunch of files for assignment 3
# Usage: ./build_file.sh create [-n nfiles] to create files
# Usage: ./build_file.sh clean to remove files and directories

# Set variables
DIR="files"
NUM_FILES=100
FILE_PREFIX="file_"
FILE_SUFFIX=".dat"

# Function to create files
create_files() {
    # Create the directory if it doesn't exist
    mkdir -p "$DIR"

    # Create files with a specific pattern
    for ((i=1; i<=NUM_FILES; i++)); do
        RANDOM_NUMBER=$((RANDOM % 600 + 100)) # Generate a random number between 100 and 700
        dd if=/dev/urandom of="$DIR/$FILE_PREFIX$i$FILE_SUFFIX" bs=1K count="$RANDOM_NUMBER" 2>/dev/null
    done

    echo "Created $NUM_FILES files in the $DIR directory."
}

# Function to clean up files
clean_files() {
    # Remove the directory and its contents
    rm -rf "$DIR"
    echo "Cleaned up files in the $DIR directory."
}

# Check command line arguments
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 [create|clean]"
    exit 1
fi

# Check for optional number of files
if [ "$1" == "create" ] && [ "$2" == "-n" ]; then
    if [[ "$3" =~ ^[0-9]+$ ]]; then
        NUM_FILES="$3"
    else
        echo "Invalid number of files. Using default: $NUM_FILES"
    fi
fi

# Process command line arguments
case "$1" in
    create)
        create_files
        ;;
    clean)
        clean_files
        ;;
    *)
        echo "Invalid option. Use 'create' to create files or 'clean' to remove them."
        exit 1
        ;;
esac