SUMMARY = "This is tool to read and write MII registers from ethernet physicals under linux. It has been tested with Realtek and Marvell PHY's connected via PCIe and should work with all drivers implementing the mdio ioctls."

HOMEPAGE = "https://github.com/PieVo/mdio-tool"

LICENSE = "CLOSED"

SRCREV = "${AUTOREV}"
SRC_URI = "git://github.com/PieVo/mdio-tool.git;protocol=https;branch=master"

SRC_DRIECTORY = "${WORKDIR}/git"

do_compile() {
        ${CC} ${SRC_DRIECTORY}/mdio-tool.c -o ${SRC_DRIECTORY}/mdio-tool
}

do_install() {
    install -d -m 0755 ${D}/usr${base_bindir}
    install -m 0755 ${SRC_DRIECTORY}/mdio-tool ${D}/usr${base_bindir}/
}

do_package_qa() {
}

FILES:${PN} += " /usr${base_bindir}/mdio-tool"

