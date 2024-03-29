
export TERM=linux

export LS_OPTIONS='--color=auto'
eval "`dircolors`"
alias ls='ls $LS_OPTIONS'
alias ll='ls $LS_OPTIONS -l'
alias l='ls $LS_OPTIONS -lA'

alias brc='/opt/cora-z7-deployment/branch-control/bin/brc'
alias restart_sensor='/opt/utils/restart_sensor'

# EPICS
export EPICS_BASE="/opt/epics"
export EPICS_HOST_ARCH=`${EPICS_BASE}/startup/EpicsHostArch`
export PATH="${PATH}:${EPICS_BASE}/bin/${EPICS_HOST_ARCH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${EPICS_BASE}/lib/${EPICS_HOST_ARCH}"

# http proxy
export HTTP_PROXY="http://192.168.10.1:8889"
export HTTPS_PROXY="http://192.168.10.1:8889"
export http_proxy="http://192.168.10.1:8889"
export https_proxy="http://192.168.10.1:8889"

# TCL scripts and TCL->IOC FIFO
export TCL_LIB_PATH=/opt/configio-master/tcl_libraries
export IOC_FIFO_PATH=/run/fifopipe
