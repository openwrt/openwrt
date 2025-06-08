. /lib/functions.sh

# Flip active 'rootfs' partition in selected 'bootconfig' mtd partition
# $1 target 'bootconfig' mtd partition name
# $2 'offset' of the active rootfs flag byte
alfa_bootconfig_rootfs_rotate() {
	local part="$1"
	local offs="$2"

	local mtdnum=$(find_mtd_index "$part")
	[ -c "/dev/mtd${mtdnum}" ] || return 1

	dd if=/dev/mtd${mtdnum} of=/tmp/mtd${mtdnum} bs=1k > /dev/null 2>&1

	local active="$(dd if=/tmp/mtd${mtdnum} bs=1 skip=${offs} count=1 2>/dev/null)"
	active=$(printf "%d\n" "\"$active")

	if [ "$active" = "1" ]; then
		printf '\x00' | dd of=/tmp/mtd${mtdnum} \
			conv=notrunc bs=1 seek=${offs} > /dev/null 2>&1
	else
		printf '\x01' | dd of=/tmp/mtd${mtdnum} \
			conv=notrunc bs=1 seek=${offs} > /dev/null 2>&1
	fi

	mtd -qq write /tmp/mtd${mtdnum} /dev/mtd${mtdnum} 2>/dev/null

	local mtdnum_sec=$(find_mtd_index "${part}1")
	[ -c "/dev/mtd${mtdnum_sec}" ] && \
		mtd -qq write \
			/tmp/mtd${mtdnum} /dev/mtd${mtdnum_sec} 2>/dev/null

	return 0
}
