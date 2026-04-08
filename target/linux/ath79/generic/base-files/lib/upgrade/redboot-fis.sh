platform_check_image_redboot_fis() {
	if [ "$(get_magic_word "$1")" != "7379" ]; then
		v "Unknown image format, aborting!"
		return 1
	else
		return 0
	fi
}

platform_do_upgrade_redboot_fis() {
	local append
	local sysup_file="$1"
	local kern_part="$2"

	if [ "$(get_magic_word "$sysup_file")" = "7379" ]; then
		local board_dir=$(tar tf $sysup_file | grep -m 1 '^sysupgrade-.*/$')

		[ -f "$UPGRADE_BACKUP" ] && append="-j $UPGRADE_BACKUP"

		if grep -q "mtd1.*loader" /proc/mtd; then
			tar xf $sysup_file ${board_dir}kernel ${board_dir}root -O | \
				mtd -r $append write - loader:firmware

		else
			local kern_length=$(tar xf $sysup_file ${board_dir}kernel -O | wc -c)

			tar xf $sysup_file ${board_dir}kernel ${board_dir}root -O | \
				mtd -r $append -F$kern_part:$kern_length:0x80060000,rootfs write - $kern_part:rootfs
		fi
	fi
}
