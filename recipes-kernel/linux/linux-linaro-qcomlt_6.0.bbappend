FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://qrb5165-rb5.dts;subdir=git/arch/arm64/boot/dts/qcom \
    file://sm8250.dtsi;subdir=git/arch/arm64/boot/dts/qcom \
    file://defconfig;subdir=git/arch/arm64/configs \
    file://0001-usb-ethenet-bringup-fix.patch \
    file://0001-usb-lpm-disable.patch \
    file://0001-wifi-bt-6-.1.22-add-kernel.patch \
	"
