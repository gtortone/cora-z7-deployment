#!/bin/bash

sudo debos --cpus=8 --disable-fakemachine recipes/xvc-adaptive-tarball.yaml

echo "build tarball using xvc-adaptive build directory..."
cd tarballs
mkdir tmp ; tar -C tmp -xzvf xvc-adaptive-rootfs.tar.gz ./opt/xvc-adaptive/bin ; tar -C tmp -czf xvc-adaptive.tar.gz opt ; rm -rf tmp

echo "done"
echo
