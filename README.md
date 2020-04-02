scrabble
========

Implementation of Scrabble AI from:
    https://www.cs.cmu.edu/afs/cs/academic/class/15451-s06/www/lectures/scrabble.pdf
    https://ericsink.com/downloads/faster-scrabble-gordon.pdf


Compilation on Ubuntu 16.04 (there's a linker bug?):
    $ CC=gcc-8 CXX=g++-8 cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=gold ..
