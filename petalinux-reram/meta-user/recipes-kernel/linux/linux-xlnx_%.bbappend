FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://bsp.cfg"
KERNEL_FEATURES:append = " bsp.cfg"
SRC_URI += "file://user_2025-07-01-08-18-00.cfg \
            file://user_2025-07-01-10-03-00.cfg \
            file://user_2025-07-01-15-16-00.cfg \
            "

