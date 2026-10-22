REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN="tr"

HEROSPEED_FLS_HEADER_LEN=24
HEROSPEED_FLS_ENTRY_LEN=256
HEROSPEED_FLS_ENTRY_MAGIC=74565

herospeed_fls_u32_get() {
	local bytes

	bytes="$(dd if="$1" bs=1 skip="$2" count=4 2>/dev/null |
		hexdump -v -e '1/1 "%u "')"
	set -- $bytes
	echo $(($1 | ($2 << 8) | ($3 << 16) | ($4 << 24)))
}

herospeed_fls_str_get() {
	dd if="$1" bs=1 skip="$2" count="$3" 2>/dev/null | tr -d '\000'
}

herospeed_fls_entry_pos() {
	echo $((HEROSPEED_FLS_HEADER_LEN + HEROSPEED_FLS_ENTRY_LEN * $1))
}

herospeed_fls_match() {
	[ "$(wc -c < "$1")" -ge "$HEROSPEED_FLS_HEADER_LEN" ] || return 1
	[ "$(herospeed_fls_str_get "$1" 0 8)" = RV1126 ] || return 1
	[ "$(herospeed_fls_str_get "$1" 8 8)" = IMX415 ]
}

herospeed_part_size() {
	echo $(($(cat "/sys/class/block/${1##*/}/size") * 512))
}

herospeed_fls_check_image() {
	local file="$1" size count i pos name len offset other part
	local have_boot= have_system=

	[ "$#" -gt 1 ] && return 1
	size="$(wc -c < "$file")"
	[ "$(herospeed_fls_u32_get "$file" 16)" -eq "$size" ] || return 1
	count="$(herospeed_fls_u32_get "$file" 20)"
	[ "$count" -gt 0 ] || return 1
	[ $((HEROSPEED_FLS_HEADER_LEN + HEROSPEED_FLS_ENTRY_LEN * count)) \
		-le "$size" ] || return 1
	other="$(rkab_slot_other "$(rkab_slot_current)")"
	[ -n "$other" ] || other=a

	i=0
	while [ "$i" -lt "$count" ]; do
		pos="$(herospeed_fls_entry_pos "$i")"
		name="$(herospeed_fls_str_get "$file" "$pos" 128)"
		len="$(herospeed_fls_u32_get "$file" $((pos + 128)))"
		offset="$(herospeed_fls_u32_get "$file" $((pos + 132)))"
		i=$((i + 1))

		[ "$(herospeed_fls_u32_get "$file" $((pos + 140)))" -eq \
			"$HEROSPEED_FLS_ENTRY_MAGIC" ] || return 1
		[ $((offset + len)) -le "$size" ] || return 1

		case "$name" in
		uboot)
			;;
		boot|system)
			part="$(find_mmc_part "${name}_$other" "$RKAB_DISK")"
			[ -n "$part" ] || return 1
			[ "$len" -le "$(herospeed_part_size "$part")" ] || {
				echo "FLS entry $name does not fit $part" >&2
				return 1
			}
			[ "$name" = boot ] && have_boot=1
			[ "$name" = system ] && have_system=1
			;;
		/var/cfg/*)
			case "$name" in
			*..*) return 1 ;;
			esac
			;;
		*)
			echo "unsupported FLS entry $name" >&2
			return 1
			;;
		esac
	done

	[ -n "$have_boot" ] && [ -n "$have_system" ]
}

herospeed_fls_entry_write() {
	local file="$1" name="$2" dest="$3" count i pos

	count="$(herospeed_fls_u32_get "$file" 20)"
	i=0
	while [ "$i" -lt "$count" ]; do
		pos="$(herospeed_fls_entry_pos "$i")"
		i=$((i + 1))
		[ "$(herospeed_fls_str_get "$file" "$pos" 128)" = "$name" ] ||
			continue
		dd if="$file" of="$dest" bs=1M iflag=skip_bytes,count_bytes \
			skip="$(herospeed_fls_u32_get "$file" $((pos + 132)))" \
			count="$(herospeed_fls_u32_get "$file" $((pos + 128)))" \
			2>/dev/null
		return $?
	done
	return 1
}

herospeed_fls_var_install() {
	local file="$1" dev mnt=/tmp/herospeed-var count i pos name dest

	dev="$(find_mmc_part var "$RKAB_DISK")"
	[ -n "$dev" ] || return 1
	mkdir -p "$mnt"
	mount -t ext4 "$dev" "$mnt" || return 1

	count="$(herospeed_fls_u32_get "$file" 20)"
	i=0
	while [ "$i" -lt "$count" ]; do
		pos="$(herospeed_fls_entry_pos "$i")"
		i=$((i + 1))
		name="$(herospeed_fls_str_get "$file" "$pos" 128)"
		case "$name" in
		/var/cfg/*) ;;
		*) continue ;;
		esac
		dest="$mnt/${name#/var/}"
		mkdir -p "${dest%/*}"
		herospeed_fls_entry_write "$file" "$name" "$dest" || {
			umount "$mnt"
			return 1
		}
		chmod 0755 "$dest"
	done

	sync
	umount "$mnt"
}

herospeed_fls_do_upgrade() {
	local file="$1" other kern root

	other="$(rkab_slot_other "$(rkab_slot_current)")"
	[ -n "$other" ] || other=a
	kern="$(find_mmc_part "boot_$other" "$RKAB_DISK")"
	root="$(find_mmc_part "system_$other" "$RKAB_DISK")"

	[ -n "$kern" ] && [ -n "$root" ] || return 1
	dd if=/dev/zero of="$kern" bs=512 count=8 2>/dev/null || return 1
	sync
	herospeed_fls_entry_write "$file" system "$root" || return 1
	sync
	herospeed_fls_entry_write "$file" boot "$kern" || return 1
	sync
	herospeed_fls_var_install "$file" || return 1
	rkab_mark_active "$other"
}

herospeed_ab_check_image() {
	local members

	herospeed_fls_match "$1" && {
		herospeed_fls_check_image "$@"
		return $?
	}

	[ "$#" -gt 1 ] && return 1
	members="$(tar tf "$1" 2>/dev/null)" || return 1
	echo "$members" | grep -q '^sysupgrade-[^/]*/kernel$' || return 1
	echo "$members" | grep -q '^sysupgrade-[^/]*/root$' || return 1
	return 0
}

herospeed_ab_do_upgrade() {
	local slot other

	herospeed_fls_match "$1" && {
		herospeed_fls_do_upgrade "$1"
		return $?
	}

	slot="$(rkab_slot_current)"
	other="$(rkab_slot_other "$slot")"
	[ -n "$other" ] || other=a
	CI_ROOTDEV="$RKAB_DISK"
	CI_KERNPART="boot_$other"
	CI_ROOTPART="system_$other"
	emmc_do_upgrade "$1"
	rkab_mark_active "$other"
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	herospeed,rv1126-imx415)
		herospeed_ab_check_image "$@"
		return $?
		;;
	*)
		return 1
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	herospeed,rv1126-imx415)
		herospeed_ab_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	local board=$(board_name)

	case "$board" in
	herospeed,rv1126-imx415)
		emmc_copy_config
		;;
	esac
}
