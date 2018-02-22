PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	avm,fritz3370-rev2-hynix|\
	avm,fritz3370-rev2-micron|\
	avm,fritz7362sl|\
	avm,fritz7412|\
	bt,homehub-v2b|\
	bt,homehub-v3a|\
	bt,homehub-v5a|\
	zyxel,p-2812hnu-f1|\
	zyxel,p-2812hnu-f3|\
	lantiq,vgv952cjw33-e-ir)
		nand_do_upgrade $1
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

platform_nand_pre_upgrade() {
	local board=$(board_name)
	echo "platform_nand_pre_upgrade()"

	case "$board" in
	lantiq,vgv952cjw33-e-ir )
		# This is a global defined in nand.sh, sets another VID header offset than default.
		echo "Set header offset"
		CU_UBI_VID_HEADER_OFFSET="2048"
		;;
	esac
}
