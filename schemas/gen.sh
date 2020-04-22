#!/usr/bin/env bash

# TODO: don't check in generated files, have cmake generate them

for file in *.fbs;
do
    flatc --cpp -o ../src/ $file
done
