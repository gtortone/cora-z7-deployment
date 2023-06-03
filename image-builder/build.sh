#!/bin/bash

PL_PROJECT_BASE=~/devel/PHI/petalinux/pl-coraz7/images/linux

echo "copy PetaLinux files..."
cp $PL_PROJECT_BASE/image.ub overlays/boot
cp $PL_PROJECT_BASE/modules.tar.gz overlays/boot
echo "start Linux image build..."
sudo debos -t image:coraz7-pl-debian11.img --cpus=8 --disable-fakemachine debimage-coraz7.yaml

sudo losetup -D

echo "Note: remember to copy in FAT partition BOOT.bin and system.bit.bin"
echo
