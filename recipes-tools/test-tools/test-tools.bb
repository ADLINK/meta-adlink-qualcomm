FILESEXTRAPATHS:prepend := "${THISDIR}/files:"


SUMMARY = "QUALCOMM add service recipe"
DESCRIPTION = "Add service file and test Tools and startup script "
LICENSE = "CLOSED"

inherit module pkgconfig 



SRC_URI:append = "file://adlink_pwm.c \
file://gpiopwm_routines.c \
file://gpiopwm_routines.h \
file://spidev_test.c \
file://test.wav \
file://cpu_freq.sh \
file://sd_test.sh \
file://sd_val.sh \
file://eltt2.c \
file://eltt2.h \
file://gpiotest.c \
file://oemedit.c \
file://ad-litbmc-fwupd-src.c \
file://ad-litbmc-fwupd-src.h \
"
TARGET_CC_ARCH += "${LDFLAGS}"
do_compile() {
        ${CC} -o ${WORKDIR}/spidev ${WORKDIR}/spidev_test.c
        ${CC} -o ${WORKDIR}/eltt2 ${WORKDIR}/eltt2.c
        ${CC} -o ${WORKDIR}/gpiotest ${WORKDIR}/gpiotest.c
        ${CC} -o ${WORKDIR}/oemedit ${WORKDIR}/oemedit.c
        ${CC} -o ${WORKDIR}/ad-litbmc-fwupd ${WORKDIR}/ad-litbmc-fwupd-src.c
        ${CC} -o ${WORKDIR}/adlink_pwm ${WORKDIR}/adlink_pwm.c ${WORKDIR}/gpiopwm_routines.c
}

do_install() {

	install -d -m 0755 ${D}${sysconfdir}
        install -d -m 0755 ${D}${bindir}

        install -m 0644 ${WORKDIR}/spidev -D ${D}${bindir}
        install -m 0644 ${WORKDIR}/eltt2 -D ${D}${bindir}
        install -m 0644 ${WORKDIR}/gpiotest -D ${D}${bindir}
        install -m 0644 ${WORKDIR}/oemedit -D ${D}${bindir}
        install -m 0644 ${WORKDIR}/ad-litbmc-fwupd -D ${D}${bindir}

        install -m 0644 ${WORKDIR}/adlink_pwm -D ${D}${bindir}
        install -m 0644 ${WORKDIR}/cpu_freq.sh -D ${D}${bindir}

        install -m 0644 ${WORKDIR}/sd_test.sh  -D ${D}${sysconfdir}/
        install -m 0644 ${WORKDIR}/sd_val.sh  -D ${D}${sysconfdir}/
        install -m 0644 ${WORKDIR}/test.wav  -D ${D}${sysconfdir}/
	
        chmod +x ${D}${bindir}/spidev
        chmod +x ${D}${bindir}/eltt2
        chmod +x ${D}${bindir}/gpiotest
        chmod +x ${D}${bindir}/oemedit
        chmod +x ${D}${bindir}/ad-litbmc-fwupd
        chmod +x ${D}${bindir}/cpu_freq.sh
        chmod +x ${D}${bindir}/adlink_pwm
        chmod +x ${D}${sysconfdir}/sd_test.sh
        chmod +x ${D}${sysconfdir}/sd_val.sh

}

FILES:${PN} += "${bindir} ${sysconfdir} "

FILES_SOLIBSDEV = ""


INSANE_SKIP_${PN} = "already-stripped"


