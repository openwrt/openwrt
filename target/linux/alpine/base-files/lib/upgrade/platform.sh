REQUIRE_IMAGE_METADATA=1

# NAND/sysupgrade path intentionally disabled until platform NAND layout is finalized.
platform_check_image() {
	echo "platform_check_image: sysupgrade is disabled on this target for now."
	return 1
}

platform_do_upgrade() {
	echo "platform_do_upgrade: NAND upgrade path is not ready yet. Aborting."
	return 1
}
