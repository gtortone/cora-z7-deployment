#!/bin/sh

cd /opt
git clone https://github.com/gtortone/xvc-adaptive.git
cd /opt/xvc-adaptive
make -j8
