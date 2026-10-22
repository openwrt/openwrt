. /lib/functions.sh

RKAB_DISK=mmcblk0
RKAB_OFFSET=2048
RKAB_SIZE=32
RKAB_CRC_SIZE=28
RKAB_MAX_PRIORITY=15
RKAB_MAX_TRIES=7
RKAB_MAGIC="0 65 66 48"
RKAB_LAST_BOOT_POS=17

rkab_slot_current() {
	local suffix

	suffix="$(cmdline_get_var androidboot.slot_suffix)"
	echo "${suffix#_}"
}

rkab_slot_other() {
	case "$1" in
	a) echo b ;;
	b) echo a ;;
	esac
}

rkab_slot_index() {
	case "$1" in
	a) echo 0 ;;
	b) echo 1 ;;
	esac
}

rkab_crc32() {
	local crc=$((0xFFFFFFFF)) byte bit

	for byte in "$@"; do
		crc=$((crc ^ byte))
		for bit in 1 2 3 4 5 6 7 8; do
			if [ $((crc & 1)) -eq 1 ]; then
				crc=$(((crc >> 1) ^ 0xEDB88320))
			else
				crc=$((crc >> 1))
			fi
		done
	done
	echo $(((crc ^ 0xFFFFFFFF) & 0xFFFFFFFF))
}

rkab_read() {
	local dev="$1"

	dd if="$dev" bs=1 skip=$RKAB_OFFSET count=$RKAB_SIZE 2>/dev/null |
		hexdump -v -e '1/1 "%u "'
}

rkab_write() {
	local dev="$1"
	shift
	local byte escapes=""

	for byte in "$@"; do
		escapes="$escapes$(printf '\\%03o' "$byte")"
	done
	printf "$escapes" |
		dd of="$dev" bs=1 seek=$RKAB_OFFSET conv=notrunc 2>/dev/null
}

rkab_fresh() {
	echo "$RKAB_MAGIC 1 0 0 0 $RKAB_MAX_PRIORITY 0 1 0 14 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0"
}

rkab_load() {
	local dev="$1" data first4 crc stored

	data="$(rkab_read "$dev")"
	set -- $data
	first4="$1 $2 $3 $4"
	if [ "$first4" != "$RKAB_MAGIC" ] || [ $# -ne $RKAB_SIZE ]; then
		rkab_fresh
		return
	fi
	crc="$(rkab_crc32 $(echo "$data" | cut -d' ' -f1-$RKAB_CRC_SIZE))"
	stored=$(( (${29} << 24) | (${30} << 16) | (${31} << 8) | ${32} ))
	[ "$crc" = "$stored" ] || {
		rkab_fresh
		return
	}
	echo "$data"
}

rkab_store() {
	local dev="$1"
	shift
	local crc head

	head="$(echo "$@" | cut -d' ' -f1-$RKAB_CRC_SIZE)"
	crc="$(rkab_crc32 $head)"
	rkab_write "$dev" $head \
		$(((crc >> 24) & 255)) $(((crc >> 16) & 255)) \
		$(((crc >> 8) & 255)) $((crc & 255))
}

rkab_field_set() {
	local pos="$1" value="$2" byte i=1
	shift 2

	for byte in "$@"; do
		[ $i -eq "$pos" ] && byte="$value"
		printf '%s ' "$byte"
		i=$((i + 1))
	done
}

rkab_mark_active() {
	local slot="$1" dev data idx other_idx base other_base

	dev="$(find_mmc_part misc $RKAB_DISK)"
	[ -n "$dev" ] || return 1
	idx="$(rkab_slot_index "$slot")"
	other_idx=$((1 - idx))
	base=$((9 + idx * 4))
	other_base=$((9 + other_idx * 4))
	data="$(rkab_load "$dev")"
	data="$(rkab_field_set $base $RKAB_MAX_PRIORITY $data)"
	data="$(rkab_field_set $((base + 1)) $RKAB_MAX_TRIES $data)"
	data="$(rkab_field_set $((base + 2)) 0 $data)"
	set -- $data
	eval "[ \${$other_base} -gt $((RKAB_MAX_PRIORITY - 1)) ]" &&
		data="$(rkab_field_set $other_base $((RKAB_MAX_PRIORITY - 1)) $data)"
	rkab_store "$dev" $data
}

rkab_mark_successful() {
	local slot="$1" dev data idx base

	dev="$(find_mmc_part misc $RKAB_DISK)"
	[ -n "$dev" ] || return 1
	idx="$(rkab_slot_index "$slot")"
	base=$((9 + idx * 4))
	data="$(rkab_load "$dev")"
	data="$(rkab_field_set $((base + 1)) 0 $data)"
	data="$(rkab_field_set $((base + 2)) 1 $data)"
	data="$(rkab_field_set $RKAB_LAST_BOOT_POS $idx $data)"
	rkab_store "$dev" $data
}
