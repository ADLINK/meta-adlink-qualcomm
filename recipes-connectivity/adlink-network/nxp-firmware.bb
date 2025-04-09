#FILESEXTRAPATHS_prepend := "${THISDIR}/files:"


SUMMARY = "NXP/ADLINK firmware"
DESCRIPTION = "integrating wifi firmwares a "
LICENSE = "CLOSED"

inherit pkgconfig

SRC_URI = "git://github.com/NXP/imx-firmware.git;protocol=https;branch=lf-5.15.52_2.1.0"
SRCREV = "b6f070e3d4cab23932d9e6bc29e3d884a7fd68f4"

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


