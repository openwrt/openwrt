#
# Copyright (C) 2011 OpenWrt.org
#

. /lib/ar71xx.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/ar71xx.sh

CI_BLKSZ=65536
CI_LDADR=0x80060000

platform_find_partitions() {
	local first dev size erasesize name
	while read dev size erasesize name; do
		name=${name#'"'}; name=${name%'"'}
		case "$name" in
			vmlinux.bin.l7|vmlinux|kernel|linux|rootfs|filesystem)
				if [ -z "$first" ]; then
					first="$name"
				else
					echo "$erasesize:$first:$name"
					break
				fi
			;;
		esac
	done < /proc/mtd
}

platform_find_kernelpart() {
	local part
	for part in "${1%:*}" "${1#*:}"; do
		case "$part" in
			vmlinux.bin.l7|vmlinux|kernel|linux)
				echo "$part"
				break
			;;
		esac
	done
}

platform_do_upgrade_combined() {
	local partitions=$(platform_find_partitions)
	local kernelpart=$(platform_find_kernelpart "${partitions#*:}")
	local erase_size=$((0x${partitions%%:*})); partitions="${partitions#*:}"
	local kern_length=0x$(dd if="$1" bs=2 skip=1 count=4 2>/dev/null)
	local kern_blocks=$(($kern_length / $CI_BLKSZ))
	local root_blocks=$((0x$(dd if="$1" bs=2 skip=5 count=4 2>/dev/null) / $CI_BLKSZ))

	if [ -n "$partitions" ] && [ -n "$kernelpart" ] && \
	   [ ${kern_blocks:-0} -gt 0 ] && \
	   [ ${root_blocks:-0} -gt ${kern_blocks:-0} ] && \
	   [ ${erase_size:-0} -gt 0 ];
	then
		local append=""
		[ -f "$CONF_TAR" -a "$SAVE_CONFIG" -eq 1 ] && append="-j $CONF_TAR"

		( dd if="$1" bs=$CI_BLKSZ skip=1 count=$kern_blocks 2>/dev/null; \
		  dd if="$1" bs=$CI_BLKSZ skip=$((1+$kern_blocks)) count=$root_blocks 2>/dev/null ) | \
			mtd -r $append -F$kernelpart:$kern_length:$CI_LDADR,rootfs write - $partitions
	fi
}

tplink_get_image_hwid() {
	get_image "$@" | dd bs=4 count=1 skip=16 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

platform_check_image() {
	local board=$(ar71xx_board_name)
	local magic="$(get_magic_word "$1")"
	local magic_long="$(get_magic_long "$1")"

	[ "$ARGC" -gt 1 ] && return 1

	case "$board" in
	all0258n )
		platform_check_image_all0258n "$1" && return 0
		return 1
		;;
	alfa-ap96 | \
	alfa-nx | \
	ap113 | \
	ap121 | \
	ap121-mini | \
	ap96 | \
	db120 | \
	hornet-ub | \
	zcn-1523h-2 | \
	zcn-1523h-5)
		[ "$magic_long" != "68737173" -a "$magic_long" != "19852003" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	ap81 | \
	ap83 | \
	dir-600-a1 | \
	dir-615-c1 | \
	dir-615-e4 | \
	dir-825-b1 | \
	mzk-w04nu | \
	mzk-w300nh | \
	tew-632brp | \
	tew-673gru | \
	wrt400n | \
	airrouter | \
	bullet-m | \
	nanostation-m | \
	rocket-m | \
	rw2458n | \
	wzr-hp-g300nh2 | \
	wzr-hp-g300nh | \
	wzr-hp-g450h | \
	wzr-hp-ag300h | \
	whr-g301n | \
	whr-hp-g300n | \
	whr-hp-gn | \
	nbg460n_550n_550nh | \
	unifi )
		[ "$magic" != "2705" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	tl-mr11u | \
	tl-mr3020 | \
	tl-mr3220 | \
	tl-mr3420 | \
	tl-wa901nd | \
	tl-wa901nd-v2 | \
	tl-wr703n | \
	tl-wr741nd | \
	tl-wr741nd-v4 | \
	tl-wr841n-v1 | \
	tl-wr841n-v7 | \
	tl-wr941nd | \
	tl-wr1043nd | \
	tl-wr2543n)
		[ "$magic" != "0100" ] && {
			echo "Invalid image type."
			return 1
		}

		local hwid
		local imageid

		hwid=$(tplink_get_hwid)
		imageid=$(tplink_get_image_hwid "$1")

		[ "$hwid" != "$imageid" ] && {
			echo "Invalid image, hardware ID mismatch, hw:$hwid image:$imageid."
			return 1
		}

		return 0
		;;
	wndr3700)
		local hw_magic

		hw_magic="$(ar71xx_get_mtd_part_magic firmware)"
		[ "$magic_long" != "$hw_magic" ] && {
			echo "Invalid image, hardware ID mismatch, hw:$hw_magic image:$magic_long."
			return 1
		}
		return 0
		;;
	wrt160nl)
		[ "$magic" != "4e4c" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	routerstation | \
	routerstation-pro | \
	ls-sr71 | \
	pb42 | \
	pb44 | \
	eap7660d | \
	ja76pf | \
	ja76pf2)
		[ "$magic" != "4349" ] && {
			echo "Invalid image. Use *-sysupgrade.bin files on this board"
			return 1
		}

		local md5_img=$(dd if="$1" bs=2 skip=9 count=16 2>/dev/null)
		local md5_chk=$(dd if="$1" bs=$CI_BLKSZ skip=1 2>/dev/null | md5sum -); md5_chk="${md5_chk%% *}"

		if [ -n "$md5_img" -a -n "$md5_chk" ] && [ "$md5_img" = "$md5_chk" ]; then
			return 0
		else
			echo "Invalid image. Contents do not match checksum (image:$md5_img calculated:$md5_chk)"
			return 1
		fi
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(ar71xx_board_name)

	case "$board" in
	routerstation | \
	routerstation-pro | \
	ls-sr71 | \
	eap7660d | \
	pb42 | \
	pb44 | \
	ja76pf | \
	ja76pf2)
		platform_do_upgrade_combined "$ARGV"
		;;
	all0258n )
		platform_do_upgrade_all0258n "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog
