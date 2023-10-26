#!/bin/bash

sudo debos --cpus=8 --disable-fakemachine recipes/openfpgaloader-tarball.yaml

echo "build tarball using openFPGALoader build directory..."
cd tarballs
mkdir tmp ; tar -C tmp -xzvf openfpgaloader-rootfs.tar.gz ./usr/local/bin/openFPGALoader ; tar -C tmp -czf openfpgaloader.tar.gz usr/local/bin/openFPGALoader ; rm -rf tmp

echo "done"
echo
