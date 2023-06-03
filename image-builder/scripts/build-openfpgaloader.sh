#!/bin/sh

cd /opt
git clone https://github.com/gtortone/openFPGALoader.git
cd openFPGALoader
mkdir build
cd build
cmake ..
make -j8 install
