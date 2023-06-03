#!/bin/sh

cd /opt
git clone https://github.com/gtortone/cora-z7-deployment.git
cd cora-z7-deployment/branch-control
make -j8
