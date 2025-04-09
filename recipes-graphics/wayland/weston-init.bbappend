FILESEXTRAPATHS:prepend := "${THISDIR}/files:"


SRC_URI:append += "file://weston.ini"


do_install:append() {

	install -D -p -m0644 ${WORKDIR}/weston.ini ${D}${sysconfdir}/xdg/weston/weston.ini

}
