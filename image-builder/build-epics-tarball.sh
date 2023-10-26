#!/bin/bash

sudo debos --cpus=8 --disable-fakemachine recipes/epics-tarball.yaml

echo "build tarball using EPICS build directory..."
cd tarballs
mkdir tmp ; tar -C tmp -xzvf epics-rootfs.tar.gz ./opt ; tar -C tmp -czf epics.tar.gz opt ; rm -rf tmp

echo "done"
echo
