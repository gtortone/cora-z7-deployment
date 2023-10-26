#!/bin/sh

# build EPICS base

cd /opt
wget https://epics.anl.gov/download/base/base-3.15.9.tar.gz
tar xzf base-3.15.9.tar.gz
rm base-3.15.9.tar.gz
mv base-3.15.9 epics
cd epics

make -j

mkdir -p /opt/epics/support

# build ASYN driver

cd /opt/epics/support

git clone https://github.com/epics-modules/asyn.git

cd asyn

git checkout -b R4-30 R4-30

cat > configure/RELEASE << EOF
EPICS_BASE=/opt/epics
EOF

make -j

# build Stream device

cd /opt/epics/support

git clone https://github.com/paulscherrerinstitute/StreamDevice.git

cd StreamDevice

cat > configure/RELEASE << EOF
TEMPLATE_TOP=/opt/epics/templates/makeBaseApp/top

ASYN=/opt/epics/support/asyn
PCRE_LIB=/lib/arm-linux-gnueabihf
PCRE_INCLUDE=/usr/include

EPICS_BASE=/opt/epics
EOF

make -j
