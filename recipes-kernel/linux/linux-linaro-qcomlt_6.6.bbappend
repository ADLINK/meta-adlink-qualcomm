# do not override KBRANCH and SRCREV_machine, use default ones.
COMPATIBLE_MACHINE:qcom = "qcom-armv8a|qcom-armv7a|lec-rb5n|lec-rb5"

FILESEXTRAPATHS:prepend:qcom := "${THISDIR}/files:"

# include all Qualcomm-specific files
#SRC_URI:append:qcom = " \
#    file://qcom.scc \
#"
SRC_URI:append = " \
    file://qrb5165-rb5.dts;subdir=git/arch/arm64/boot/dts/qcom \
    file://defconfig;subdir=git/arch/arm64/configs \
    file://sm8250.dtsi;subdir=git/arch/arm64/boot/dts/qcom \
    file://0001-Regulator-support-USB-and-Ethernet.patch \
    file://0002-Disable-USB-LPM.patch \
    file://0003-Support-for-wifi-bluetooth-chipset.patch \
    file://0004-LT9611-bridge-to-support-FHD-resolution.patch \
    file://0005-Remove-DSI-attach-to-bind-simple-panel.patch \ 
    file://0006-Uart-fix-zero-Rx-bytes.patch \
    file://0007-UFS-clock-failure-restart-kernel.patch \
    "
