#!/bin/ash
# SPDX-License-Identifier: GPL-2.0-or-later OR MIT
# shellcheck shell=dash

. /lib/functions.sh

MAGIC_XIAOMI_HDR1="48445231"     # "HDR1" - xiaomi image header
MAGIC_XIAOMI_BLK="beba0000"
#MAGIC_UIMAGE="27051956"         # uImage header
MAGIC_FIT="d00dfeed"             # FIT header
MAGIC_UBI="55424923"             # "UBI#"
#MAGIC_UBIFS="31181006"
#MAGIC_HSQS="68737173"           # "hsqs"
MAGIC_SYSUPG="7379737570677261"  # TAR "sysupgrade"

XIAOMI_PAGESIZE=2048

XIAOMI_BOARD=
XIAOMI_FW_FILE=""
XIAOMI_FW_SIZE=0
XIAOMI_FW_MAGIC=
XIAOMI_FW_MODEL=
XIAOMI_KERNEL_PART=$CI_KERNPART
XIAOMI_ROOTFS_PART=$CI_UBIPART
XIAOMI_ROOTFS_PARTSIZE=

XIAOMI_RESTORE_ROOTFS2=
XIAOMI_RESTORE_NVRAM=

log_msg() {
	echo "$@"
}

log_err() {
	echo "ERROR: $*" >&2
}

die() {
	log_err "$@"
	echo "========================================================="
	sleep 1
	exit 1
}

get_uint8_at() {
	local offset=$1
	local hex
	[ $(( offset + 1 )) -gt "$XIAOMI_FW_SIZE" ] && { echo ""; return; }
	hex=$( dd if="$XIAOMI_FW_FILE" skip="$offset" bs=1 count=1 2>/dev/null \
		| hexdump -v -e '"%02x"' )
	printf "%d" 0x"$hex"
}

get_uint32_at() {
	local offset=$1
	local endianness=$2
	local hex
	[ $(( offset + 4 )) -gt "$XIAOMI_FW_SIZE" ] && { echo ""; return; }
	if [ "$endianness" = "be" ]; then
		hex=$( dd if="$XIAOMI_FW_FILE" skip="$offset" bs=1 count=4 2>/dev/null \
			| hexdump -v -n 4 -e '1/1 "%02x"' )
	else
		hex=$( dd if="$XIAOMI_FW_FILE" skip="$offset" bs=1 count=4 2>/dev/null \
			| hexdump -v -e '1/4 "%02x"' )
	fi
	printf "%d" 0x"$hex"
}

get_hexdump_at() {
	local offset=$1
	local size=$2
	[ $(( offset + size )) -gt "$XIAOMI_FW_SIZE" ] && { echo ""; return; }
	dd if="$XIAOMI_FW_FILE" skip="$offset" bs=1 count="$size" 2>/dev/null \
		| hexdump -v -n "$size" -e '1/1 "%02x"'
}

get_round_up() {
	local value=$1
	local base=$2
	local pad=0
	if [ -z "$base" ]; then
		base=$XIAOMI_PAGESIZE
	else
		base=$( printf "%d" "$base" )
	fi
	if [ $(( value % base )) != 0 ]; then
		pad=$(( base - value % base ))
	fi
	echo $(( value + pad ))
}

get_part_size() {
	local part_name=$1
	local part
	local mtd_size_hex
	part=$( grep "\"$part_name\"" < "/proc/mtd" )
	if [ -z "$part" ]; then
		mtd_size_hex=0
	else
		mtd_size_hex=$( echo "$part" | awk '{print "0x"$2}' )
	fi
	printf "%d" "$mtd_size_hex"
}

xiaomi_check_sizes() {
	local part_name=$1
	local img_offset=$2
	local img_size=$3
	local mtd_size
	local img_end

	mtd_size=$( get_part_size "$part_name" )
	if [ "$mtd_size" = "0" ]; then
		echo "cannot find mtd partition with name '$part_name'"
		return 1
	fi	
	img_end=$(( img_offset + img_size ))
	if [ $img_end -gt "$XIAOMI_FW_SIZE" ]; then
		echo "incorrect image size (part: '$part_name')"
		return 1
	fi
	if [ "$img_size" -gt "$mtd_size" ]; then
		echo "image is greater than partition '$part_name'"
		return 1
	fi
	echo ""
	return 0
}

xiaomi_mtd_write() {
	local part_name=$1
	local img_offset=$2
	local img_size=$3
	local part_skip=$4
	local err
	local count

	img_size=$( get_round_up "$img_size" )
	err=$( xiaomi_check_sizes "$part_name" "$img_offset" "$img_size" )
	if [ -n "$err" ]; then
		log_err "$err"
		return 1
	fi
	count=$(( img_size / XIAOMI_PAGESIZE ))
	if [ -n "$part_skip" ]; then
		dd if="$XIAOMI_FW_FILE" iflag=skip_bytes skip="$img_offset" bs="$XIAOMI_PAGESIZE" count="$count" \
			| mtd -f -p "$part_skip" write - "$part_name"
	else
		dd if="$XIAOMI_FW_FILE" iflag=skip_bytes skip="$img_offset" bs="$XIAOMI_PAGESIZE" count="$count" \
			| mtd -f write - "$part_name"
	fi
	if [ "$( echo -n $? )" -ne 0 ]; then
		log_err "Failed to flash '$part_name'"
		return 1
	fi
	return 0
}

xiaomi_flash_images() {
	local kernel_offset=$1
	local kernel_size=$2
	local rootfs_offset=$3
	local rootfs_size=$4
	local fitubi_offset
	local fitubi_size
	local err
	local part_skip=0
	local ksize
	local mtd_size

	if [ "$XIAOMI_RESTORE_ROOTFS2" = "true" ]; then
		fitubi_offset="$kernel_offset"
		fitubi_size="$kernel_size"
		kernel_size=$( get_part_size "$XIAOMI_KERNEL_PART" )
		rootfs_offset=$(( fitubi_offset + kernel_size ))
		rootfs_size=$(( fitubi_size - kernel_size ))
		rootfs_size=$( get_round_up "$rootfs_size" )
	else
		kernel_size=$( get_round_up "$kernel_size" )
		rootfs_size=$( get_round_up "$rootfs_size" )
	fi

	err=$( xiaomi_check_sizes "$XIAOMI_KERNEL_PART" "$kernel_offset" "$kernel_size" )
	[ -n "$err" ] && { log_err "$err"; return 1; }

	err=$( xiaomi_check_sizes "$XIAOMI_ROOTFS_PART" "$rootfs_offset" "$rootfs_size" )
	[ -n "$err" ] && { log_err "$err"; return 1; }

	if [ "$XIAOMI_RESTORE_ROOTFS2" = "true" ] && [ -n "$XIAOMI_ROOTFS_PARTSIZE" ]; then
		part_skip=$( printf "%d" "$XIAOMI_ROOTFS_PARTSIZE" )
		part_skip=$(( part_skip - kernel_size ))
		if [ "$part_skip" -lt 1000000 ]; then
			part_skip=0
		fi
	fi

	if [ "$part_skip" -gt 0 ]; then
		ksize=$(( part_skip + fitubi_size ))
		mtd_size=$( get_part_size "$XIAOMI_ROOTFS_PART" )
		if [ "$ksize" -gt "$mtd_size" ]; then
			log_err "double rootfs is greater than partition '$XIAOMI_ROOTFS_PART'"
			return 1
		fi
	fi

	mtd erase "$XIAOMI_ROOTFS_PART" || {
		log_err "Failed to erase partition '$part_name'"
		return 1
	}

	if [ "$XIAOMI_RESTORE_ROOTFS2" = "true" ] && [ -n "$XIAOMI_RESTORE_NVRAM" ]; then
		eval "$XIAOMI_RESTORE_NVRAM"
	fi

	xiaomi_mtd_write "$XIAOMI_KERNEL_PART" "$kernel_offset" "$kernel_size" || {
		log_err "Failed flash data to '$XIAOMI_KERNEL_PART' partition"
		return 1
	}
	log_msg "Kernel image flashed to '$XIAOMI_KERNEL_PART'"

	xiaomi_mtd_write "$XIAOMI_ROOTFS_PART" "$rootfs_offset" "$rootfs_size" || {
		log_err "Failed flash data to '$XIAOMI_ROOTFS_PART' partition"
		return 1
	}
	log_msg "Rootfs image flashed to '$XIAOMI_ROOTFS_PART'!"
	
	if [ "$part_skip" -gt 0 ]; then
		xiaomi_mtd_write "$XIAOMI_ROOTFS_PART" "$fitubi_offset" "$fitubi_size" "$part_skip" || {
			log_err "Failed flash data to '$XIAOMI_ROOTFS_PART' partition (2)"
			return 1
		}
		log_msg "Rootfs image flashed to '$XIAOMI_ROOTFS_PART':$XIAOMI_ROOTFS_PARTSIZE"
	fi

	log_msg "Firmware write successful! Reboot..."
	log_msg "==================================================="
	sync
	umount -a
	reboot -f
	exit 0
}

check_ubi_header() {
	local offset=$1
	local magic
	local magic_ubi2="55424921"  # "UBI!"

	magic=$( get_hexdump_at "$offset" 4 )
	[ "$magic" != $MAGIC_UBI ] && { echo ""; return 1; }

	offset=$(( offset + XIAOMI_PAGESIZE ))
	magic=$( get_hexdump_at "$offset" 4 )
	[ "$magic" != "$magic_ubi2" ] && { echo ""; return 1; }

	echo "true"
	return 0
}

get_rootfs_offset() {
	local start=$1
	local pos
	local offset
	local align
	local end

	for offset in 0 1 2 3 4; do
		pos=$(( start + offset ))
		[ -n "$( check_ubi_header "$pos" )" ] && { echo "$pos"; return 0; }
	done

	for align in 4 8 16 32 64 128 256 512 1024 2048 4096; do
		pos=$( get_round_up "$start" "$align" )
		[ -n "$( check_ubi_header "$pos" )" ] && { echo "$pos"; return 0; }
	done

	align=65536
	pos=$( get_round_up "$start" "$align" )
	end=$(( pos + 3000000 ))
	while true; do
		[ $(( pos + 150000 )) -gt "$XIAOMI_FW_SIZE" ] && break
		[ -n "$( check_ubi_header "$pos" )" ] && { echo "$pos"; return 0; }
		pos=$(( pos + align ))
		[ "$pos" -ge "$end" ] && break
	done

	echo ""
	return 1
}

xiaomi_do_factory_upgrade() {
	local err
	local magic
	local kernel_offset  kernel_size
	local rootfs_offset  rootfs_size
	local kernel_mtd

	kernel_mtd=$( find_mtd_index "$XIAOMI_KERNEL_PART" )
	if [ -z "$kernel_mtd" ]; then
		die "partition '$XIAOMI_KERNEL_PART' not found"
	fi
	log_msg "Forced factory upgrade..."

	kernel_offset=0
	kernel_size=$( get_uint32_at 4 "be" )

	rootfs_offset=$( get_rootfs_offset "$kernel_size" )
	if [ -z "$rootfs_offset" ]; then
		die "can't find ubinized rootfs in the firmware image"
	fi
	rootfs_size=$(( XIAOMI_FW_SIZE - rootfs_offset ))
	#local rootfs_end=$(( rootfs_offset + rootfs_size ))

	XIAOMI_RESTORE_ROOTFS2=false
	xiaomi_flash_images "$kernel_offset" "$kernel_size" "$rootfs_offset" "$rootfs_size" || {
		die "can't flash factory image"
	}
	log_msg "================================================="
	exit 0
}

xiaomi_do_revert_stock() {
	local err
	local magic
	local blk  blkpos  blk_magic  offset  file_size
	local fitubi_offset
	local fitubi_size=0

	log_msg "Forced revert to stock firmware..."

	for blk in 16 20 24 28 32 36; do
		blkpos=$( get_uint32_at $blk )
		[ -z "$blkpos" ] && continue
		[ "$blkpos" -lt 48 ] && continue
		blk_magic=$( get_hexdump_at "$blkpos" 4 )
		[ "$blk_magic" != "$MAGIC_XIAOMI_BLK" ] && continue
		offset=$(( blkpos + 8 ))
		file_size=$( get_uint32_at "$offset" 4 )
		[ -z "$file_size" ] && continue
		[ "$file_size" -lt 4000000 ] && continue
		offset=$(( blkpos + 48 ))
		magic=$( get_hexdump_at "$offset" 4 )
		if [ "$magic" = $MAGIC_FIT ]; then
			fitubi_size=$file_size
			fitubi_offset=$offset
		fi
	done
	if [ "$fitubi_size" -eq 0 ]; then
		die "incorrect stock firmware FIT image"
	fi
	if [ $(( fitubi_size % XIAOMI_PAGESIZE )) = 4 ]; then
		# Remove DEADCODE footer
		fitubi_size=$(( fitubi_size - 4 ))
	fi

	XIAOMI_RESTORE_ROOTFS2=true
	xiaomi_flash_images "$fitubi_offset" "$fitubi_size" || {
		die "can't revert to stock firmware"
	}
	log_msg "================================================="
	exit 0
}

xiaomi_do_upgrade() {
	local kernel_mtd
	local rootfs_mtd

	if ! xiaomi_check_image "$1" "$2" "do"; then
		die "Image file '$XIAOMI_FW_FILE' is incorrect!"
	fi
	
	kernel_mtd=$( find_mtd_index "$XIAOMI_KERNEL_PART" )
	if [ -z "$kernel_mtd" ]; then
		die "cannot find mtd partition for '$XIAOMI_KERNEL_PART'"
	fi
	rootfs_mtd=$( find_mtd_index "$XIAOMI_ROOTFS_PART" )
	if [ -z "$rootfs_mtd" ]; then
		die "cannot find mtd partition for '$XIAOMI_ROOTFS_PART'"
	fi

	# Flash factory image (FIT header)
	if [ "$XIAOMI_FW_MAGIC" = $MAGIC_FIT ]; then
		XIAOMI_ROOTFS_PARTSIZE=
		xiaomi_do_factory_upgrade
		exit $?
	fi

	# Revert to stock firmware ("HDR1" header)
	if [ "$XIAOMI_FW_MAGIC" = $MAGIC_XIAOMI_HDR1 ]; then
		xiaomi_do_revert_stock
		exit $?
	fi

	log_msg "Check TAR-image..."
	xiaomi_check_tar || die "Incorrect TAR-image!"

	log_msg "SysUpgrade start..."
	nand_do_upgrade "$XIAOMI_FW_FILE"
}

xiaomi_check_tar() {
	local tar_file="$XIAOMI_FW_FILE"
	local board_dir
	local control_len  kernel_len  rootfs_len

	if [ "$XIAOMI_FW_MAGIC" != $MAGIC_SYSUPG ]; then
		log_msg "incorrect TAR-image!"
		return 1
	fi	
	board_dir=$( tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$' )
	[ -z "$board_dir" ] && {
		log_msg "incorrect TAR-image! (board dir not found)"
		return 1
	}
	board_dir=${board_dir%/}

	control_len=$( (tar xf "$tar_file" "$board_dir/CONTROL" -O | wc -c) 2> /dev/null)
	if [ "$control_len" -lt 3 ]; then
		log_msg "incorrect TAR-image! (CONTROL not found)"
		return 1
	fi
	kernel_len=$( (tar xf "$tar_file" "$board_dir/kernel" -O | wc -c) 2> /dev/null)
	if [ "$kernel_len" -lt 1000000 ]; then
		log_msg "incorrect TAR-image! (kernel not found)"
		return 1
	fi
	rootfs_len=$( (tar xf "$tar_file" "$board_dir/root" -O | wc -c) 2> /dev/null)
	if [ "$rootfs_len" -lt 1000000 ]; then
		log_msg "incorrect TAR-image! (rootfs not found)"
		return 1
	fi
	return 0
}

xiaomi_init() {
	XIAOMI_BOARD="$1"
	XIAOMI_FW_FILE="$2"
	XIAOMI_FW_SIZE=$( wc -c "$XIAOMI_FW_FILE" 2> /dev/null | awk '{print $1}' )
	XIAOMI_FW_MAGIC=
	XIAOMI_FW_MODEL=
	XIAOMI_ROOTFS_PARTSIZE=
	[ -z "$XIAOMI_FW_SIZE" ] && return 1
	[ "$XIAOMI_FW_SIZE" -lt 1000000 ] && return 1
	XIAOMI_FW_MAGIC=$( get_hexdump_at 0 4 )
	local magic8
	magic8=$( get_hexdump_at 0 8 )
	if [ "$magic8" = $MAGIC_SYSUPG ]; then
		XIAOMI_FW_MAGIC="$magic8"
	fi
	XIAOMI_FW_MODEL=$( get_uint8_at 14 )
	case "$XIAOMI_BOARD" in
	xiaomi,redmi-router-ax6s)
		XIAOMI_ROOTFS_PARTSIZE=0x1e00000  # 0x20c0000 - 0x2c0000
		XIAOMI_RESTORE_NVRAM="fw_setenv boot_fw1 'run boot_rd_img2;bootm'"
		;;
	*)
		;;
	esac
	return 0
}

xiaomi_check_image() {
	local stage=$3
	local err
	if ! xiaomi_init "$1" "$2"; then
		log_msg ". Image file '$XIAOMI_FW_FILE' is incorrect!"
		return 1
	fi
	if [ -z "$XIAOMI_FW_SIZE" ]; then
		log_msg ". File '$XIAOMI_FW_FILE' not found!"
		return 1
	fi
	if [ "$XIAOMI_FW_SIZE" -lt 1000000 ]; then
		log_msg ". File '$XIAOMI_FW_FILE' is incorrect"
		return 1
	fi

	if [ "$XIAOMI_FW_MAGIC" = "$MAGIC_FIT" ]; then
		if [ "$stage" != "do" ]; then
			log_msg ". "
			#log_msg "Your configurations won't be saved if factory.bin image specified."
			#log_msg "But if you want to upgrade, please execute sysupgrade with \"-F\" option."
			return 1
		fi
		return 0
	fi

	if [ "$XIAOMI_FW_MAGIC" = "$MAGIC_XIAOMI_HDR1" ]; then
		if [ "$stage" != "do" ]; then
			log_msg ". "
			#log_msg "If you want to flash stock image, please execute sysupgrade with \"-F\" option."
			return 1
		fi
		err=". Incorrect stock firmware! Bad model number!"
		case "$XIAOMI_BOARD" in
		xiaomi,redmi-router-ax6s)
			[ "$XIAOMI_FW_MODEL" = 50 ] && err=""  # RB01
			[ "$XIAOMI_FW_MODEL" = 51 ] && err=""  # RB03
			;;
		*)
			;;
		esac
		if [ -n "$err" ]; then
			log_msg "$err"
			return 1
		fi
		return 0
	fi

	xiaomi_check_tar || return 1
	nand_do_platform_check "$XIAOMI_BOARD" "$XIAOMI_FW_FILE" || return 1
	return 0
}
