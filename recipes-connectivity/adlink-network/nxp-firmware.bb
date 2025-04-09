#FILESEXTRAPATHS_prepend := "${THISDIR}/files:"


SUMMARY = "NXP/ADLINK firmware"
DESCRIPTION = "integrating wifi firmwares "
LICENSE = "CLOSED"

inherit pkgconfig

SRC_URI = "git://github.com/nxp-imx/imx-firmware.git;protocol=https;branch=lf-6.6.52_2.2.0"
SRCREV = "2978f3c88d6bcc5695a7b45f1936f18d31eebfa8"

S = "${WORKDIR}/git"

do_install() {

	install -d ${D}${nonarch_base_libdir}/firmware/nxp
	install -m 0644 ${WORKDIR}/git/nxp/wifi_mod_para.conf    ${D}${nonarch_base_libdir}/firmware/nxp

	install -m 0644 ${WORKDIR}/git/nxp/FwImage_8997/pcieuart8997_combo_v4.bin ${D}${nonarch_base_libdir}/firmware/nxp	
}

FILES:${PN} += "${nonarch_base_libdir}/firmware/nxp/* "

FILES_SOLIBSDEV = ""

do_package_qa() {
}

INSANE_SKIP_${PN} = "already-stripped"


