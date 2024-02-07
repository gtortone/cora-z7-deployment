#!/bin/sh

echo "- +" | sfdisk -N 2 /dev/mmcblk0 --force
partprobe
resize2fs /dev/mmcblk0p2

# postinstall 

if [ -d "/boot/overlay" ]; then
   cp -a /boot/overlay/* /
fi

if [ -d "/boot/scripts" ]; then
   for f in /boot/scripts/*; do
      $f
   done
fi

systemctl disable firstboot.service
rm -f /etc/systemd/system/firstboot.service
rm -f /firstboot.sh

# reboot to apply settings
reboot
