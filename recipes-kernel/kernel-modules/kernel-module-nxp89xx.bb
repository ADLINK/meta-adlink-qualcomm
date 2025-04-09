# Copyright 2023 ADLINK Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LICENSE = "CLOSED"

SRC_URI = " \
  file://mlan.conf \
  file://moal.conf \
"


do_install () {
   install -d ${D}${sysconfdir}/modprobe.d
   # if building wifi/bt, then copy mlan.conf moal.conf
   install -m 644 ${WORKDIR}/mlan.conf ${D}${sysconfdir}/modprobe.d/
   install -m 644 ${WORKDIR}/moal.conf ${D}${sysconfdir}/modprobe.d/
}

FILES:${PN} += "${sysconfdir}/modprobe.d"
