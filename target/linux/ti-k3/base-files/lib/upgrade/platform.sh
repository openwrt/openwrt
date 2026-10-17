# SPDX-License-Identifier: GPL-2.0-only

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	legacy_sdcard_check_image "$1"
}

platform_copy_config() {
	legacy_sdcard_copy_config
}

platform_do_upgrade() {
	legacy_sdcard_do_upgrade "$1"
}
