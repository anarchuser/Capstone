#!/bin/bash

cmake -Hcapnproto -Bcapnproto/build
sudo cmake --build capnproto/build --target install
