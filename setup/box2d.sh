#!/bin/bash

cmake -Hbox2d -Bbox2d/build
sudo cmake --build box2d/build --target install

#cd box2d || exit
#./build.sh

#mkdir -p box2d/build
#cd box2d/build || exit
#cmake ..
#make