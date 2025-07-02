FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://platform-top.h file://bsp.cfg file://0001-add-default-environment.patch"
SRC_URI += "file://user_2025-07-01-09-39-00.cfg"

