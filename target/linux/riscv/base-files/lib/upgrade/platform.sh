PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_check_image() {
	fit_check_image "$1"
	return $?
}

platform_do_upgrade() {
	fit_do_upgrade "$1"
}
