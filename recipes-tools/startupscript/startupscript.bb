FILESEXTRAPATHS:prepend := "${THISDIR}/files:"


SUMMARY = "QUALCOMM add service recipe"
DESCRIPTION = "Add service file , enabling auto boot GUI add startup scripts "
LICENSE = "CLOSED"

inherit pkgconfig systemd update-rc.d 

do_rm_work[noexec] = "1"

SRC_URI:append = "file://99-rtc1.rules \
file://wifimodule.tar;unpack=0 \
file://gpio_enable.sh \
file://gpio.service \
file://version.sh \
file://version.service \
file://version.c \
file://fastboot.sh \
file://checkinterface.c \
"
SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_PACKAGES += "${PN}"
SYSTEMD_SERVICE:${PN} += "version.service"
SYSTEMD_SERVICE:${PN} += "gpio.service"

INITSCRIPT_NAME = "adlinkstartupscript"
INITSCRIPT_PARAMS = "start 99 2 3 4 5 . stop 19 0 1 6 ."


do_compile() {
	${CC} -o ${WORKDIR}/version ${WORKDIR}/version.c
	${CC} -o ${WORKDIR}/device-info ${WORKDIR}/checkinterface.c
}

do_install() {

        install -d -m 0755 ${D}/usr${base_bindir}


        install -d ${D}${sysconfdir}/udev/
        install -d ${D}${sysconfdir}/udev/rules.d/

        install -d ${D}${sysconfdir}/systemd/
        install -d ${D}${sysconfdir}/systemd/system
        install -d ${D}${sysconfdir}/init.d
        install -d ${D}${sysconfdir}/rc1.d


        install -m 0644 ${WORKDIR}/99-rtc1.rules -D ${D}${sysconfdir}/udev/rules.d/
        install -m 0644 ${WORKDIR}/gpio_enable.sh -D ${D}${sysconfdir}/
        install -m 0644 ${WORKDIR}/gpio.service -D ${D}${sysconfdir}/systemd/system/


        install -m 0644 ${WORKDIR}/version.sh -D ${D}${sysconfdir}/
        install -m 0644 ${WORKDIR}/version.service -D ${D}${sysconfdir}/systemd/system/

	
        install -m 0644 ${WORKDIR}/version -D ${D}${bindir}
        install -m 0644 ${WORKDIR}/device-info -D ${D}${bindir}


        install -m 0644 ${WORKDIR}/fastboot.sh -D ${D}${sysconfdir}/

        chmod +x ${D}${sysconfdir}/gpio_enable.sh
        chmod +x ${D}${sysconfdir}/version.sh
	chmod +x ${D}${bindir}/version 
	chmod +x ${D}${bindir}/device-info 


}

FILES:${PN} += "${bindir}/* ${sysconfdir}/* ${D}${sysconfdir}/systemd/system/ "

FILES_SOLIBSDEV = ""

do_package_qa() {
}

INSANE_SKIP_${PN} = "already-stripped"


