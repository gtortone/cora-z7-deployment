#!/bin/sh

cd /opt
git clone https://github.com/gtortone/leaf-monitor.git

# EPICS
export EPICS_BASE="/opt/epics"
export EPICS_HOST_ARCH=`${EPICS_BASE}/startup/EpicsHostArch`
export PATH="${PATH}:${EPICS_BASE}/bin/${EPICS_HOST_ARCH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${EPICS_BASE}/lib/${EPICS_HOST_ARCH}"

pip3 install requests pyyaml pcaspy psutil
