. /lib/functions.sh

# Read or update the "bootnum" in the "persist" partition
#
# parameters:
#   $1: index of active partition to set (1/2)
#
# operations:
#   read : mstc_rw_bootnum
#   write: mstc_rw_bootnum <index>
mstc_rw_bootnum() {
	local mtd
	local curval setval="$1"

	case "$setval" in
	1|2|"") ;;
	*) echo "invalid bootnum specified \"$setval\""; return 1 ;;
	esac

	mtd=$(find_mtd_part "persist")
	if [ -z "$mtd" ]; then
		echo "cannot find "persist" mtd partition"
		return 1
	fi

	curval=$(hexdump -s 4 -n 1 -e '"%d"' $mtd)
	[ -z "$setval" ] &&
		echo "$curval" && return 0

	[ "$curval" != "$setval" ] &&
		printf "\x$setval" | \
			dd of=$mtd bs=1 seek=4 conv=notrunc
}
