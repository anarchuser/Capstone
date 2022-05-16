#!/bin/bash

cmake -Hbox2d -Bbox2d/build
sudo cmake --build box2d/build --target install
