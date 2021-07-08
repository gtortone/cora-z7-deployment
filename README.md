# Digilent Cora Z7 deployment

## Cora Z7 deployment repository
```
git clone https://github.com/gtortone/cora-z7-deployment.git
```

# Vivado board files
```
git clone https://github.com/Digilent/vivado-boards.git

cp -r * /opt/Xilinx/Vivado/2018.3/data/boards/board_files/
```

# Prerequisites

```
apt-get install gcc-arm-linux-gnueabihf

apt-get install libncurses5 libncurses5-dev openssl libssl-dev bison flex \
	qemu-user-static debootstrap device-tree-compiler u-boot-tools
```

## U-Boot

```
git clone http://github.com/Xilinx/u-boot-xlnx.git

cd u-boot-xlnx

git checkout xilinx-v2018.3
```

### files to copy from cora-z7-deployment repository

https://raw.githubusercontent.com/gtortone/cora-z7-deployment/main/u-boot/arch/arm/dts/Makefile
https://raw.githubusercontent.com/gtortone/cora-z7-deployment/main/u-boot/arch/arm/dts/zynq-coraz7.dts
https://raw.githubusercontent.com/gtortone/cora-z7-deployment/main/u-boot/configs/zynq_coraz7_defconfig

```
export CROSS_COMPILE=arm-linux-gnueabihf-

make zynq_coraz7_defconfig

make menuconfig

make u-boot.elf
```

### output files
```
u-boot.elf
```

## Kernel

```
git clone https://github.com/Xilinx/linux-xlnx.git

cd linux-xlnx

git checkout xilinx-v2018.3
```

### build

```
export CROSS_COMPILE=arm-linux-gnueabihf-

make ARCH=arm xilinx_zynq_defconfig

make ARCH=arm menuconfig

make -j4 ARCH=arm UIMAGE_LOADADDR=0x8000 uImage dtbs modules

make -j4 ARCH=arm UIMAGE_LOADADDR=0x8000 modules_install INSTALL_MOD_PATH=<rootfs_directory>

```

### output files
```
arch/arm/boot/uImage
```

## FSBL

```
hsi

set_param board.repoPaths <path>

	(<vivado_path>/data/boards/board_files)

set hwdsgn [open_hw_design <hdf_file>]

generate_app -hw $hwdsgn -os standalone -proc ps7_cortexa9_0 -app zynq_fsbl -compile -sw fsbl -dir fsbl

quit
```

### output files
```
executable.elf
```

## Device Tree

```
git clone https://github.com/Xilinx/device-tree-xlnx.git

cd device-tree-xlnx

git checkout xilinx-v2018.3
```

```
hsi

set_param board.repoPaths <path>

	(<vivado_path>/data/boards/board_files)

open_hw_design <design_name>.hdf

	(contained in <project>.sdk directory)

set_repo_path <path to device-tree-xlnx repository>

create_sw_design device-tree -os device_tree -proc ps7_cortexa9_0

generate_target -dir my_dts
```

### compile device tree blob (.dtb) from device tree source (.dts)

(copy device-tree/cora-z7.dts from cora-z7-deployment repository to 'my_dts')

```
cd my_dts

cpp -nostdinc -I include -I arch  -undef -x assembler-with-cpp cora-z7.dts > system-top-preproc.dts

dtc -I dts -O dtb system-top-preproc.dts -o devicetree.dtb
```

### output files
```
my_dts/devicetree.dtb
```

## Root Filesystem

```
sudo debootstrap --verbose --arch armhf --variant=minbase --foreign buster debian10 http://ftp.it.debian.org/debian

sudo mount -t devpts devpts debian10/dev/pts
sudo mount -t proc proc debian10/proc

sudo chroot debian10 /bin/bash
```

### inside chroot
```
/debootstrap/debootstrap --second-stage

cat <<! >> /etc/apt/sources.list
deb http://ftp.it.debian.org/debian/ buster main 
deb http://security.debian.org/ buster/updates main
!

apt-get update
apt-get upgrade

export LANG=C
apt-get -y install apt-utils dialog locales
dpkg-reconfigure locales
export LANG=en_US.UTF-8
apt-get -y install udev netbase ifupdown openssh-server iputils-ping wget net-tools ntpdate nano less kmod vim
apt-get -y install sysvinit-core sysvinit-utils

cat <<! >> /etc/fstab
tmpfs    /tmp tmpfs defaults 0 0
!

echo "Zynq" > /etc/hostname
passwd

cat <<! >> /etc/securetty
ttyPS0
ttyPS1
!
```

### enable serial console

comment out the tty1 to tty6 specifications in /etc/inittab and add the serial console
```
T0:23:respawn:/sbin/getty -L ttyPS0 115200 vt100
```

### exit from chroot
```
exit
```

### copy rootfs to SD
```
sudo rsync -va debian10/ <mountpoint>/ROOTFS
```

## MicroSD preparation

(as root)

```
fdisk /dev/sdb
```

### partitioning

1)	~500 MB (fat32)
2)	remaining space	(ext4)

### labeling
```
dosfslabel /dev/sdb1 BOOT
e2label /dev/sdb2 ROOTFS
```

### on BOOT partition
```
boot.bin
uImage
devicetree.dtb
```

In order to properly configure MAC address create uEnv.txt file with this content:
```
ethaddr=00:18:3E:03:66:F6
```

### on ROOTFS partition
```
rsync -va debian10/ <mountpoint>/ROOTFS
```

## Boot image build (boot.bin)

### create boot.bif
```
image : {
        [bootloader]fsbl/executable.elf
        mybitstream.bit
        u-boot.elf
}
```

### generate boot.bin
```
bootgen -image boot.bif -o boot.bin -w on
```
