#!/bin/bash
# Official SDL page: https://wiki.libsdl.org/Installation

cd SDL || exit
mkdir build
cd build || exit
../configure
make
sudo make install
