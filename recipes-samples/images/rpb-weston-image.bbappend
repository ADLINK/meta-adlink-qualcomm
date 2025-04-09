remove_etc_version () {
	rm -rf ${IMAGE_ROOTFS}${sysconfdir}/version
}
ROOTFS_POSTPROCESS_COMMAND += "remove_etc_version ; "

