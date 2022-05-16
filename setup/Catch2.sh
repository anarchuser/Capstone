#!/bin/bash

cmake -HCatch2 -BCatch2/build -DBUILD_TESTING=OFF
sudo cmake --build Catch2/build --target install
