#!/bin/bash

# c - create files
# d - delete files
# r - remove zip files

if [[ $1 == "c" ]]; then
    # create mixed size files
    # in mixed_files directory
    # create a directory for mixed files
    mkdir -p mixed_files
    ./scripts/build_file.sh create -s s -n 450
    mv small_files/* mixed_files/
    dd if=/dev/urandom of="mixed_files/file_1.dat" bs=2M count=35 2>/dev/null #70 MB
    dd if=/dev/urandom of="mixed_files/file_2.dat" bs=2M count=50 2>/dev/null #100 MB

    # create a bunch of small files
    # in small_files directory
    ./scripts/build_file.sh create -s s -n 1000

    # create a bunch of big files
    # in big_files directory
    mkdir -p big_files
    dd if=/dev/urandom of="big_files/file_1.dat" bs=2M count=15 2>/dev/null #30 MB
    dd if=/dev/urandom of="big_files/file_2.dat" bs=2M count=50 2>/dev/null #100 MB
    dd if=/dev/urandom of="big_files/file_3.dat" bs=2M count=25 2>/dev/null #50 MB
    dd if=/dev/urandom of="big_files/file_4.dat" bs=2M count=100 2>/dev/null #200 MB

elif [[ $1 == "d" ]]; then
    # remove everything created by this file
    rm -rf mixed_files
    rm -rf small_files
    rm -rf big_files
elif [[ $1 == "r" ]]; then
    # remove zip files
    rm mixed_files/*.zip
    rm small_files/*.zip
    rm big_files/*.zip
else
    echo "Invalid option. Use 'c' to create files, 'd' to delete them, or r to remove .zip files."
    exit 1
fi