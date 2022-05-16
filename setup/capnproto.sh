#!/bin/bash

mkdir -p capnproto/build
cmake -Hcapnproto -Bcapnproto/build
sudo cmake --build capnproto/build --target install
