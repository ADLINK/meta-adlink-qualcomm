FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SUMMARY = "ADLINK weston image"
DESCRIPTION = "added weston adlink image on background "
LICENSE = "CLOSED"

SRC_URI = "file://adlink.jpg"

do_install() {

        install -d -m 0755 ${D}/${sysconfdir}
        install -m 0644 ${WORKDIR}/adlink.jpg -D ${D}${sysconfdir}/
}



FILES_${PN} += "${sysconfdir}/* ${D}${sysconfdir}/* "

FILES_SOLIBSDEV = ""

do_package_qa() {
}

INSANE_SKIP_${PN} = "already-stripped"

ROOTFS_POSTPROCESS_COMMAND ="do_install; "
#ROOTFS_POSTPROCESS_COMMAND ="do_remove; "

