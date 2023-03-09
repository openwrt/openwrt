. /lib/functions.sh

fortinet_fwinfo_blocks() {
	local fwinfo_mtd="$(find_mtd_part firmware-info)"
	local offset="$1"
	local len="$2"
	local blks

	if [ -z "$fwinfo_mtd" ]; then
		echo "WARN: MTD device \"firmware-info\" not found"
		return 1
	fi

	blks=$((len / 0x200))
	[ $((len % 0x200)) -gt 0 ] && blks=$((blks + 1))
	blks=$(printf "%04x" $blks)
	printf "fwinfo: offset-> 0x%x, blocks-> 0x%s (len: 0x%08x)\n" \
		$offset $blks $len

	printf "\x${blks:2:2}\x${blks:0:2}" | \
		dd bs=2 count=1 seek=$((offset / 2)) conv=notrunc of=${fwinfo_mtd}
}

fortinet_do_upgrade() {
	local board_dir="$(tar tf "$1" | grep -m 1 '^sysupgrade-.*/$')"
	local kern_mtd="$(find_mtd_part kernel)"
	local root_mtd="$(find_mtd_part rootfs)"
	local kern_len root_len

	board_dir="${board_dir%/}"

	if [ -z "$kern_mtd" ] || [ -z "$root_mtd" ]; then
		echo "ERROR: MTD device \"kernel\" or \"rootfs\" not found"
		umount -a
		reboot -f
	fi

	kern_len=$( (tar xOf "$1" "$board_dir/kernel" | wc -c) 2> /dev/null)
	root_len=$( (tar xOf "$1" "$board_dir/root" | wc -c) 2> /dev/null)

	if [ -z "$kern_len" ] || [ -z "$root_len" ]; then
		echo "ERROR: failed to get length of new kernel or rootfs"
		umount -a
		reboot -f
	fi

	fortinet_fwinfo_blocks "0x184" "$kern_len"
	fortinet_fwinfo_blocks "0x18c" "$root_len"

	tar xOf "$1" "$board_dir/kernel" | \
		mtd write - "kernel"
	tar xOf "$1" "$board_dir/root" | \
		mtd ${UPGRADE_BACKUP:+-j "${UPGRADE_BACKUP}"} write - "rootfs"
}
